/* NovaOS C-Kernel - IPC-Queue, Device Manager, Driver Framework,
   VFS-Bootstrap und Service Manager (Port aus ipc32.inc). */
#include "ipc.h"
#include "object.h"
#include "component.h"
#include "bib.h"
#include <stddef.h>

/* --------------------------------------------------------------------- */
/* IPC-Message-Queue                                                     */
/* --------------------------------------------------------------------- */

static ipc_message_t ipc_messages[IPC_QUEUE_CAPACITY];
static uint32_t ipc_head = 0;
static uint32_t ipc_tail = 0;
static uint32_t ipc_count = 0;

void ipc_initialize(void) {
    ipc_head = 0;
    ipc_tail = 0;
    ipc_count = 0;
    for (uint32_t i = 0; i < IPC_QUEUE_CAPACITY; ++i) {
        ipc_messages[i] = (ipc_message_t){0};
    }
}

int ipc_send(const ipc_message_t *msg) {
    __asm__ volatile ("cli");
    if (ipc_count >= IPC_QUEUE_CAPACITY) {
        __asm__ volatile ("sti");
        return 0;
    }
    ipc_messages[ipc_tail] = *msg;
    ipc_tail = (ipc_tail + 1) & (IPC_QUEUE_CAPACITY - 1);
    ++ipc_count;
    __asm__ volatile ("sti");
    return 1;
}

int ipc_receive(ipc_message_t *out_msg) {
    __asm__ volatile ("cli");
    if (ipc_count == 0) {
        __asm__ volatile ("sti");
        return 0;
    }
    *out_msg = ipc_messages[ipc_head];
    ipc_head = (ipc_head + 1) & (IPC_QUEUE_CAPACITY - 1);
    --ipc_count;
    __asm__ volatile ("sti");
    return 1;
}

int ipc_self_test(void) {
    if (ipc_count != 0) return -1;

    ipc_message_t msg = {{0xDEADBEEFu, 0xCAFEBABEu, 0x12345678u, 0xABCDABCDu}};
    if (ipc_send(&msg) != 1) return -1;
    if (ipc_count != 1) return -1;

    ipc_message_t received = {{0}};
    if (ipc_receive(&received) != 1) return -1;
    if (received.words[0] != 0xDEADBEEFu || received.words[1] != 0xCAFEBABEu) return -1;
    if (ipc_count != 0) return -1;

    if (ipc_receive(&received) != 0) return -1; /* leere Queue -> 0 */

    return 0;
}

/* --------------------------------------------------------------------- */
/* NPSPEC-IPC-0001: echtes Endpoint-Modell (Phase 1, siehe ipc.h)        */
/* --------------------------------------------------------------------- */

typedef struct {
    uint32_t id;
    uint32_t state;
    uint32_t owner_pid;
    uint32_t capacity;
    ipc_message_t queue[IPC_ENDPOINT_QUEUE_CAPACITY];
    uint32_t head, tail, count;
    uint32_t allowed_senders[IPC_ENDPOINT_ALLOWED_SENDERS];
    uint32_t allowed_count;
    uint32_t sent_total, received_total, dropped_total, denied_total;
} ipc_endpoint_t;

static ipc_endpoint_t ipc_endpoints[IPC_ENDPOINT_CAPACITY];
static uint32_t ipc_endpoint_next_id = 1;
static int ipc_endpoint_mgr_ready = 0;

void ipc_endpoint_manager_initialize(void) {
    for (uint32_t i = 0; i < IPC_ENDPOINT_CAPACITY; ++i) {
        ipc_endpoints[i] = (ipc_endpoint_t){0};
    }
    ipc_endpoint_next_id = 1;
    ipc_endpoint_mgr_ready = 1;
}

static ipc_endpoint_t *ipc_endpoint_find(uint32_t endpoint_id) {
    if (endpoint_id == 0) return 0;
    for (uint32_t i = 0; i < IPC_ENDPOINT_CAPACITY; ++i) {
        if (ipc_endpoints[i].id == endpoint_id) {
            return &ipc_endpoints[i];
        }
    }
    return 0;
}

static int ipc_endpoint_sender_allowed(const ipc_endpoint_t *ep, uint32_t sender_pid) {
    if (sender_pid == ep->owner_pid) return 1;
    for (uint32_t i = 0; i < ep->allowed_count; ++i) {
        if (ep->allowed_senders[i] == sender_pid) return 1;
    }
    return 0;
}

int ipc_endpoint_create(uint32_t owner_pid, uint32_t capacity, uint32_t *out_endpoint_id) {
    /* Jeder CLOSED-Slot ist wiederverwendbar (auch ein zuvor schon
       genutzter und wieder geschlossener) - ipc_endpoint_info() fuer die
       ALTE ID liefert danach folgerichtig IPC_ERR_NOT_FOUND, da die neue
       Belegung eine neue ID bekommt (siehe ipc_endpoint_next_id unten). */
    ipc_endpoint_t *slot = 0;
    for (uint32_t i = 0; i < IPC_ENDPOINT_CAPACITY; ++i) {
        if (ipc_endpoints[i].state == IPC_ENDPOINT_STATE_CLOSED) {
            slot = &ipc_endpoints[i];
            break;
        }
    }
    if (!slot) return IPC_ERR_CAPACITY;

    if (capacity == 0 || capacity > IPC_ENDPOINT_QUEUE_CAPACITY) {
        capacity = IPC_ENDPOINT_QUEUE_CAPACITY;
    }

    *slot = (ipc_endpoint_t){0};
    slot->id = ipc_endpoint_next_id++;
    slot->state = IPC_ENDPOINT_STATE_OPEN;
    slot->owner_pid = owner_pid;
    slot->capacity = capacity;

    if (out_endpoint_id) *out_endpoint_id = slot->id;
    return 0;
}

int ipc_endpoint_close(uint32_t owner_pid, uint32_t endpoint_id) {
    ipc_endpoint_t *ep = ipc_endpoint_find(endpoint_id);
    if (!ep) return IPC_ERR_NOT_FOUND;
    if (ep->owner_pid != owner_pid) return IPC_ERR_DENIED;
    ep->state = IPC_ENDPOINT_STATE_CLOSED; /* id bleibt fuer ipc_endpoint_info() erhalten */
    return 0;
}

int ipc_endpoint_grant_send(uint32_t owner_pid, uint32_t endpoint_id, uint32_t sender_pid) {
    ipc_endpoint_t *ep = ipc_endpoint_find(endpoint_id);
    if (!ep) return IPC_ERR_NOT_FOUND;
    if (ep->state != IPC_ENDPOINT_STATE_OPEN) return IPC_ERR_CLOSED;
    if (ep->owner_pid != owner_pid) return IPC_ERR_DENIED;
    if (ipc_endpoint_sender_allowed(ep, sender_pid)) return 0; /* bereits erlaubt */
    if (ep->allowed_count >= IPC_ENDPOINT_ALLOWED_SENDERS) return IPC_ERR_CAPACITY;
    ep->allowed_senders[ep->allowed_count++] = sender_pid;
    return 0;
}

int ipc_endpoint_send(uint32_t sender_pid, uint32_t endpoint_id, const ipc_message_t *msg) {
    ipc_endpoint_t *ep = ipc_endpoint_find(endpoint_id);
    if (!ep) return IPC_ERR_NOT_FOUND;
    if (ep->state != IPC_ENDPOINT_STATE_OPEN) return IPC_ERR_CLOSED;
    if (!ipc_endpoint_sender_allowed(ep, sender_pid)) {
        ++ep->denied_total;
        return IPC_ERR_DENIED;
    }
    if (ep->count >= ep->capacity) {
        ++ep->dropped_total;
        return IPC_ERR_FULL;
    }
    ep->queue[ep->tail] = *msg;
    ep->tail = (ep->tail + 1u) % IPC_ENDPOINT_QUEUE_CAPACITY;
    ++ep->count;
    ++ep->sent_total;
    return 0;
}

int ipc_endpoint_receive(uint32_t owner_pid, uint32_t endpoint_id, ipc_message_t *out_msg) {
    ipc_endpoint_t *ep = ipc_endpoint_find(endpoint_id);
    if (!ep) return IPC_ERR_NOT_FOUND;
    if (ep->state != IPC_ENDPOINT_STATE_OPEN) return IPC_ERR_CLOSED;
    if (ep->owner_pid != owner_pid) return IPC_ERR_DENIED;
    if (ep->count == 0) return IPC_ERR_FULL; /* "nichts verfuegbar", siehe ipc.h */
    *out_msg = ep->queue[ep->head];
    ep->head = (ep->head + 1u) % IPC_ENDPOINT_QUEUE_CAPACITY;
    --ep->count;
    ++ep->received_total;
    return 0;
}

int ipc_endpoint_check_sender(uint32_t endpoint_id, uint32_t sender_pid) {
    ipc_endpoint_t *ep = ipc_endpoint_find(endpoint_id);
    if (!ep) return IPC_ERR_NOT_FOUND;
    if (ep->state != IPC_ENDPOINT_STATE_OPEN) return IPC_ERR_CLOSED;
    if (!ipc_endpoint_sender_allowed(ep, sender_pid)) return IPC_ERR_DENIED;
    return 0;
}

int ipc_endpoint_info(uint32_t endpoint_id, ipc_endpoint_info_t *out_info) {
    ipc_endpoint_t *ep = ipc_endpoint_find(endpoint_id);
    if (!ep) return IPC_ERR_NOT_FOUND;
    if (out_info) {
        out_info->owner_pid = ep->owner_pid;
        out_info->state = ep->state;
        out_info->capacity = ep->capacity;
        out_info->queue_depth = ep->count;
        out_info->sent_total = ep->sent_total;
        out_info->received_total = ep->received_total;
        out_info->dropped_total = ep->dropped_total;
        out_info->denied_total = ep->denied_total;
    }
    return 0;
}

int ipc_endpoint_manager_self_test(void) {
    if (ipc_endpoint_mgr_ready != 1) return -1;

    const uint32_t OWNER_PID = 0x9001u;
    const uint32_t GRANTED_PID = 0x9002u;
    const uint32_t STRANGER_PID = 0x9003u;

    uint32_t ep_id = 0;
    if (ipc_endpoint_create(OWNER_PID, 2u /* klein, um Backpressure leicht zu erzwingen */,
                             &ep_id) != 0) {
        return -1;
    }

    /* #2/#3: Kenntnis der ID allein genuegt nicht - ein fremder Prozess
       wird abgelehnt. */
    ipc_message_t msg = {{0x1111u, 0x2222u, 0x3333u, 0x4444u}};
    if (ipc_endpoint_send(STRANGER_PID, ep_id, &msg) != IPC_ERR_DENIED) return -1;

    /* #7: explizite Autorisierung schaltet den Zugriff frei. */
    if (ipc_endpoint_grant_send(OWNER_PID, ep_id, GRANTED_PID) != 0) return -1;
    if (ipc_endpoint_send(GRANTED_PID, ep_id, &msg) != 0) return -1;

    /* Owner darf immer senden, auch ohne explizite Eintragung. */
    ipc_message_t msg2 = {{0x5555u, 0x6666u, 0x7777u, 0x8888u}};
    if (ipc_endpoint_send(OWNER_PID, ep_id, &msg2) != 0) return -1;

    /* #10 Backpressure: Kapazitaet 2 ist jetzt voll -> REJECT, kein
       stilles Verwerfen. */
    ipc_message_t msg3 = {{0x9999u, 0xAAAAu, 0xBBBBu, 0xCCCCu}};
    if (ipc_endpoint_send(OWNER_PID, ep_id, &msg3) != IPC_ERR_FULL) return -1;

    ipc_endpoint_info_t info;
    if (ipc_endpoint_info(ep_id, &info) != 0) return -1;
    if (info.owner_pid != OWNER_PID || info.state != IPC_ENDPOINT_STATE_OPEN) return -1;
    if (info.queue_depth != 2u || info.sent_total != 2u) return -1;
    if (info.dropped_total != 1u || info.denied_total != 1u) return -1;

    /* Nur der Owner darf empfangen - FIFO-Reihenfolge bleibt erhalten. */
    if (ipc_endpoint_receive(STRANGER_PID, ep_id, &msg) != IPC_ERR_DENIED) return -1;
    ipc_message_t recv1 = {{0}}, recv2 = {{0}};
    if (ipc_endpoint_receive(OWNER_PID, ep_id, &recv1) != 0) return -1;
    if (recv1.words[0] != 0x1111u) return -1; /* GRANTED_PIDs Nachricht kam zuerst */
    if (ipc_endpoint_receive(OWNER_PID, ep_id, &recv2) != 0) return -1;
    if (recv2.words[0] != 0x5555u) return -1;
    if (ipc_endpoint_receive(OWNER_PID, ep_id, &recv1) != IPC_ERR_FULL) return -1; /* leer */

    /* #11 Endpoint-Verlust MUSS erkennbar sein: nach dem Schliessen
       schlagen ALLE Operationen mit einem eigenen Fehlercode fehl, nicht
       als "unbekannt" oder stillschweigend. */
    if (ipc_endpoint_close(STRANGER_PID, ep_id) != IPC_ERR_DENIED) return -1; /* nicht Owner */
    if (ipc_endpoint_close(OWNER_PID, ep_id) != 0) return -1;
    if (ipc_endpoint_close(OWNER_PID, ep_id) != 0) return -1; /* idempotent */
    if (ipc_endpoint_send(OWNER_PID, ep_id, &msg) != IPC_ERR_CLOSED) return -1;
    if (ipc_endpoint_receive(OWNER_PID, ep_id, &recv1) != IPC_ERR_CLOSED) return -1;
    if (ipc_endpoint_grant_send(OWNER_PID, ep_id, GRANTED_PID) != IPC_ERR_CLOSED) return -1;

    /* #12 Introspection bleibt fuer einen geschlossenen Endpoint moeglich
       (letzter bekannter Zustand), zeigt aber NIE Nachrichteninhalte -
       ipc_endpoint_info_t hat strukturell gar kein Payload-Feld. */
    if (ipc_endpoint_info(ep_id, &info) != 0) return -1;
    if (info.state != IPC_ENDPOINT_STATE_CLOSED) return -1;

    if (ipc_endpoint_info(0xFFFFu, &info) != IPC_ERR_NOT_FOUND) return -1; /* unbekannte ID */

    return 0;
}

/* --------------------------------------------------------------------- */
/* Bootkritischer Device Manager                                         */
/* --------------------------------------------------------------------- */

typedef struct {
    uint32_t id;
    uint32_t device_class;
    uint32_t state;
    uint32_t handle;
    uint32_t resource_base;
    uint32_t resource_size;
    uint32_t flags;
} device_record_t;

static device_record_t device_records[DEVICE_CAPACITY];
static uint32_t device_count = 0;

void *device_lookup(uint32_t device_id) {
    for (uint32_t i = 0; i < device_count; ++i) {
        if (device_records[i].id == device_id) {
            return &device_records[i];
        }
    }
    return 0;
}

int device_register(uint32_t device_id, uint32_t device_class, uint32_t resource_base, uint32_t resource_size) {
    if (device_count >= DEVICE_CAPACITY) {
        return -1;
    }
    if (device_lookup(device_id) != 0) {
        return -1; /* bereits registriert */
    }
    uint32_t handle = object_create(OBJECT_TYPE_DEVICE, device_id, 0);
    if (handle == 0) {
        return -1;
    }

    device_record_t *rec = &device_records[device_count];
    rec->id = device_id;
    rec->device_class = device_class;
    rec->state = DEVICE_STATE_ACTIVE;
    rec->handle = handle;
    rec->resource_base = resource_base;
    rec->resource_size = resource_size;
    rec->flags = 1; /* bootkritisch */
    ++device_count;
    return 0;
}

void device_manager_initialize(void) {
    for (uint32_t i = 0; i < DEVICE_CAPACITY; ++i) {
        device_records[i] = (device_record_t){0};
    }
    device_count = 0;

    device_register(1, 1, 0x40, 1); /* PIT/Timer */
    device_register(2, 2, 0x60, 5); /* PS/2-Tastaturcontroller */

    const bib_graphics_info_t *gfx = bib_graphics();
    if (gfx && gfx->has_graphics) {
        device_register(3, 3, gfx->framebuffer_address, gfx->pitch * gfx->height);
    }
}

int device_manager_self_test(void) {
    if (device_count < 2) return -1;

    device_record_t *dev1 = (device_record_t *)device_lookup(1);
    if (!dev1 || dev1->state != DEVICE_STATE_ACTIVE) return -1;

    if (device_lookup(2) == 0) return -1;

    return 0;
}

/* --------------------------------------------------------------------- */
/* Driver Framework 1.0                                                  */
/* --------------------------------------------------------------------- */

typedef struct {
    uint32_t id_lo;
    uint32_t id_hi;
    uint32_t type;
    uint32_t state;
    uint32_t abi_version;
    uint32_t device_handle;
    uint32_t flags;
} drv_record_t;

static drv_record_t drv_records[DRV_CAPACITY];
static uint32_t drv_count = 0;
static int drv_mgr_initialized = 0;

void driver_framework_initialize(void) {
    for (uint32_t i = 0; i < DRV_CAPACITY; ++i) {
        drv_records[i] = (drv_record_t){0};
    }
    drv_count = 0;
    drv_mgr_initialized = 1;
}

void *driver_find(uint32_t id_lo, uint32_t id_hi) {
    for (uint32_t i = 0; i < DRV_CAPACITY; ++i) {
        drv_record_t *rec = &drv_records[i];
        if (rec->id_lo == 0 && rec->id_hi == 0) continue;
        if (rec->id_lo == id_lo && rec->id_hi == id_hi) {
            return rec;
        }
    }
    return 0;
}

int driver_register(uint32_t id_lo, uint32_t id_hi, uint32_t type, uint32_t abi_version, uint32_t flags) {
    if (id_lo == 0 && id_hi == 0) {
        return -1;
    }
    if (drv_count >= DRV_CAPACITY) {
        return -1;
    }
    if (driver_find(id_lo, id_hi) != 0) {
        return -1; /* Duplikat */
    }

    drv_record_t *slot = 0;
    for (uint32_t i = 0; i < DRV_CAPACITY; ++i) {
        if (drv_records[i].id_lo == 0 && drv_records[i].id_hi == 0) {
            slot = &drv_records[i];
            break;
        }
    }
    if (!slot) {
        return -1;
    }

    slot->id_lo = id_lo;
    slot->id_hi = id_hi;
    slot->type = type;
    slot->state = DRV_STATE_LOADED;
    slot->abi_version = abi_version;
    slot->device_handle = 0;
    slot->flags = flags;
    ++drv_count;
    return 0;
}

int driver_bind(uint32_t id_lo, uint32_t id_hi, uint32_t device_handle) {
    drv_record_t *rec = (drv_record_t *)driver_find(id_lo, id_hi);
    if (!rec) return -1;
    if (rec->state != DRV_STATE_LOADED) return -1;
    rec->device_handle = device_handle;
    rec->state = DRV_STATE_BOUND;
    return 0;
}

int driver_framework_self_test(void) {
    if (drv_mgr_initialized != 1) return -1;
    if (drv_count != 0) return -1;

    if (driver_register(0xDFDF0001u, 0, DRV_TYPE_KERNEL, 0x00010000u, 0) != 0) return -1;
    if (drv_count != 1) return -1;

    if (driver_find(0xDFDF0001u, 0) == 0) return -1;

    if (driver_bind(0xDFDF0001u, 0, 1) != 0) return -1;

    drv_record_t *rec = (drv_record_t *)driver_find(0xDFDF0001u, 0);
    if (!rec || rec->state != DRV_STATE_BOUND) return -1;

    return 0;
}

/* --------------------------------------------------------------------- */
/* Fruehes VFS-Bootstrap-Root                                            */
/* --------------------------------------------------------------------- */

typedef struct {
    int initialized;
    int root_ready;
    uint32_t filesystem_handle;
    uint32_t namespace_handle;
    uint32_t mount_handle;
    uint32_t root_node_handle;
    uint32_t flags;
} vfs_state_t;

static vfs_state_t vfs_state;

void vfs_initialize(void) {
    vfs_state = (vfs_state_t){0};

    uint32_t fs_handle = object_create(OBJECT_TYPE_FILESYSTEM, 1, 0);
    if (fs_handle == 0) return;
    vfs_state.filesystem_handle = fs_handle;

    uint32_t ns_handle = object_create(OBJECT_TYPE_MOUNT_NAMESPACE, 1, 0);
    if (ns_handle == 0) return;
    vfs_state.namespace_handle = ns_handle;

    uint32_t mount_handle = object_create(OBJECT_TYPE_MOUNT, 1, ns_handle);
    if (mount_handle == 0) return;
    vfs_state.mount_handle = mount_handle;

    uint32_t root_handle = object_create(OBJECT_TYPE_VFS_NODE, 1, fs_handle);
    if (root_handle == 0) return;
    vfs_state.root_node_handle = root_handle;

    vfs_state.flags = VFS_FLAG_BOOTSTRAP_ROOT | VFS_FLAG_READ_ONLY;
    vfs_state.root_ready = 1;
    vfs_state.initialized = 1;
}

int vfs_lookup_root(const char *path, uint32_t path_len, uint32_t *out_handle) {
    if (!vfs_state.root_ready) return -1;
    if (path_len != 1) return -1;
    if (path[0] != '/') return -1;
    if (vfs_state.root_node_handle == 0) return -1;
    *out_handle = vfs_state.root_node_handle;
    return 0;
}

int vfs_self_test(void) {
    if (!vfs_state.initialized) return -1;
    if (vfs_state.flags != (VFS_FLAG_BOOTSTRAP_ROOT | VFS_FLAG_READ_ONLY)) return -1;

    uint32_t handle = 0;
    if (vfs_lookup_root("/", 1, &handle) != 0) return -1;
    if (handle != vfs_state.root_node_handle) return -1;

    if (vfs_lookup_root("x", 1, &handle) == 0) return -1; /* muss fehlschlagen */

    return 0;
}

/* --------------------------------------------------------------------- */
/* Service Manager                                                       */
/* --------------------------------------------------------------------- */

typedef struct {
    uint32_t id;
    uint32_t version;
    uint32_t owner;
    uint32_t state;
    uint32_t handle;
    uint32_t flags;
} service_record_t;

static service_record_t service_table[SERVICE_CAPACITY];
static uint32_t service_count = 0;

void service_manager_initialize(void) {
    for (uint32_t i = 0; i < SERVICE_CAPACITY; ++i) {
        service_table[i] = (service_record_t){0};
    }
    service_count = 0;
}

void *service_lookup(uint32_t id) {
    for (uint32_t i = 0; i < SERVICE_CAPACITY; ++i) {
        if (service_table[i].state == SERVICE_STATE_EMPTY) continue;
        if (service_table[i].id == id) {
            return &service_table[i];
        }
    }
    return 0;
}

int service_register(uint32_t id, uint32_t version, uint32_t owner_component_id) {
    if (id == 0) {
        return -1;
    }
    if (component_is_active(owner_component_id) != 0) {
        return -1;
    }

    service_record_t *slot = 0;
    for (uint32_t i = 0; i < SERVICE_CAPACITY; ++i) {
        if (service_table[i].state == SERVICE_STATE_EMPTY) {
            slot = &service_table[i];
            break;
        }
        if (service_table[i].id == id) {
            return -1; /* bereits vergeben */
        }
    }
    if (!slot) {
        return -1;
    }

    uint32_t handle = object_create(OBJECT_TYPE_SERVICE, id, owner_component_id);
    if (handle == 0) {
        return -1;
    }

    slot->id = id;
    slot->version = version;
    slot->owner = owner_component_id;
    slot->state = SERVICE_STATE_AVAILABLE;
    slot->handle = handle;
    slot->flags = IPC_CAP_FIFO | IPC_CAP_IRQ_SAFE;
    ++service_count;
    return 0;
}

int service_manager_self_test(void) {
    /* Eigene Test-Komponente als Owner (getrennt von component.c's eigenem
       "CORE"-Selbsttest-Eintrag). */
    uint32_t owner_id = 0x53455254u; /* "SERT" */
    if (component_register(owner_id, 0x00010000u, 0) == 0) return -1;
    if (component_activate(owner_id) != 0) return -1;

    uint32_t service_id = 0x4B45524Eu; /* "KERN" */
    if (service_register(service_id, 0x00010000u, owner_id) != 0) return -1;

    service_record_t *rec = (service_record_t *)service_lookup(service_id);
    if (!rec) return -1;
    if (rec->version != 0x00010000u) return -1;
    if (rec->owner != owner_id) return -1;
    if (rec->state != SERVICE_STATE_AVAILABLE) return -1;
    if (service_count != 1) return -1;

    return 0;
}
