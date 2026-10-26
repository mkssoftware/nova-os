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
