/* NovaOS C-Kernel - IPC-Queue, Device Manager, Driver Framework,
   VFS-Bootstrap und Service Manager (Port aus ipc32.inc). */
#ifndef NOVA_IPC_H
#define NOVA_IPC_H

#include <stdint.h>

#define IPC_MESSAGE_SIZE   16u
#define IPC_QUEUE_CAPACITY 16u
#define IPC_CAP_FIFO       0x00000001u
#define IPC_CAP_IRQ_SAFE   0x00000002u

typedef struct {
    uint32_t words[4]; /* 16 Bytes */
} ipc_message_t;

void ipc_initialize(void);
int ipc_self_test(void); /* 0 = ok */

/* 1 = Erfolg, 0 = Queue voll. */
int ipc_send(const ipc_message_t *msg);
/* 1 = Erfolg (out_msg gefuellt), 0 = Queue leer. */
int ipc_receive(ipc_message_t *out_msg);

#define DEVICE_CAPACITY     8u
#define DEVICE_STATE_ACTIVE 1u
#define OBJECT_TYPE_DEVICE  6u

/* Registriert PIT/Timer (ID 1) und PS/2-Tastaturcontroller (ID 2), sowie
   optional den uebernommenen Firmware-Framebuffer (ID 3) falls bib_graphics()
   has_graphics meldet. */
void device_manager_initialize(void);
int device_manager_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Tabelle voll oder device_id bereits vergeben. */
int device_register(uint32_t device_id, uint32_t device_class, uint32_t resource_base, uint32_t resource_size);
/* Gibt den internen Record zurueck, oder NULL wenn device_id unbekannt. */
void *device_lookup(uint32_t device_id);

#define DRV_CAPACITY 8u

#define DRV_TYPE_KERNEL    0u
#define DRV_TYPE_USERSPACE 1u
#define DRV_TYPE_BUS       2u
#define DRV_TYPE_FUNCTION  3u
#define DRV_TYPE_FILTER    4u
#define DRV_TYPE_VIRTUAL   5u

#define DRV_STATE_LOADED  0u
#define DRV_STATE_BOUND   2u
#define DRV_STATE_RUNNING 4u
#define DRV_STATE_FAILED  9u

void driver_framework_initialize(void);
int driver_framework_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 bei Duplikat/Tabelle voll/ungueltiger ID (lo==0 && hi==0). */
int driver_register(uint32_t id_lo, uint32_t id_hi, uint32_t type, uint32_t abi_version, uint32_t flags);
/* Gibt den internen Record zurueck, oder NULL wenn nicht gefunden. */
void *driver_find(uint32_t id_lo, uint32_t id_hi);
/* 0 = Erfolg, -1 wenn nicht gefunden oder nicht im Zustand LOADED. */
int driver_bind(uint32_t id_lo, uint32_t id_hi, uint32_t device_handle);

#define VFS_FLAG_BOOTSTRAP_ROOT     0x00000001u
#define VFS_FLAG_READ_ONLY          0x00000002u
#define OBJECT_TYPE_FILESYSTEM      7u
#define OBJECT_TYPE_MOUNT_NAMESPACE 8u
#define OBJECT_TYPE_MOUNT           9u
#define OBJECT_TYPE_VFS_NODE        10u

/* Frueher VFS-Bootstrap-Root: read-only RAMFS-Platzhalter bis zum spaeteren
   Dateisystemtreiber (nicht zu verwechseln mit dem bereits portierten
   fs.c/fsindex.c - dies ist die noch nicht aufgeloeste VFS-Objektschicht
   aus ipc32.inc). */
void vfs_initialize(void);
int vfs_self_test(void); /* 0 = ok */

/* path_len muss exakt 1 sein und *path=='/' fuer Erfolg (bewusst minimal,
   wie im alten ASM). 0 = Erfolg (out_handle gesetzt), -1 sonst. */
int vfs_lookup_root(const char *path, uint32_t path_len, uint32_t *out_handle);

#define SERVICE_CAPACITY        8u
#define SERVICE_STATE_EMPTY     0u
#define SERVICE_STATE_AVAILABLE 1u
#define OBJECT_TYPE_SERVICE     3u

void service_manager_initialize(void);
/* TODO: der alte ASM-Selbsttest nutzt service_register_typed/
   service_contract_check, die auf semantic_lookup_type aus dem noch nicht
   portierten io_qos32.inc (Semantic-Type-Registry) aufbauen. Bis dahin
   testet dieser Port den einfachen (nicht typisierten) Registrierungspfad. */
int service_manager_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn id==0, owner nicht aktiv (component_lookup), Tabelle
   voll oder id bereits vergeben. */
int service_register(uint32_t id, uint32_t version, uint32_t owner_component_id);
/* Gibt den internen Record zurueck, oder NULL wenn nicht gefunden. */
void *service_lookup(uint32_t id);

#endif
