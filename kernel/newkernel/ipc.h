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

/* --------------------------------------------------------------------- */
/* NPSPEC-IPC-0001: echtes Endpoint-Modell (Phase 1)                     */
/* --------------------------------------------------------------------- */
/* Die obige ipc_send()/ipc_receive()-Queue ist EINE einzige globale,
   ungetypte FIFO - keine Endpoint-Identitaet, keine Zugriffskontrolle
   (NPSPEC-IPC-0001 Anforderung #2/#3: "Endpunkte MUESSEN eindeutig
   identifizierbar sein", "Endpoint-Identitaet und Zugriffsberechtigung
   MUESSEN getrennt bleiben" - beides hier unerfuellt). Bleibt unveraendert
   bestehen (echte Bestandsnutzer: cap_integration.c/scheduler.c).

   Phase 1 fuehrt ECHTE, adressierbare, zugriffskontrollierte Endpoints
   ein: jeder Endpoint hat einen Owner-Prozess (einzig berechtigt zum
   Empfangen/Schliessen) und eine explizite Sender-Allowlist (Kenntnis der
   EndpointID allein gewaehrt KEINEN Zugriff - erst `ipc_endpoint_grant_
   send()` traegt einen weiteren Prozess explizit ein, NPSPEC-IPC-0001
   #3/#7 "Capability-Uebertragung MUSS explizit autorisiert sein"). Jeder
   Endpoint hat eine BEGRENZTE eigene Warteschlange (#10 Backpressure -
   bei Ueberlauf REJECT, kein stilles Verwerfen). Schliessen macht den
   Endpoint fuer alle Operationen erkennbar ungueltig (#11 "Endpoint-
   Verlust MUSS erkennbar sein"). `ipc_endpoint_info()` liefert reine
   Metadaten (Owner/Zustand/Fuellstand/Zaehler), NIE Nachrichteninhalte
   (#12 Introspection, mit dem dortigen Offenlegungsverbot).

   Bewusst NICHT Teil dieser Phase (eigene, groessere Folgeschritte, siehe
   die jeweils eigenstaendigen NPSPEC-IPC-CHANNEL/-RPC/-PUBSUB/-EVENTBUS/
   -SHAREDMEMORY/-ZEROCOPY-Spezifikationen, die bewusst ALS AUFBAU AUF
   diesem Grundmodell formuliert sind, nicht als Teil davon): typisiertes
   Request/Response mit Korrelation, Pub/Sub-Topics, ein globaler Event-
   Bus, Shared-Memory-Buffer-Referenzen, echtes Zero-Copy, entfernte
   (Multi-Node-)Endpoints. Automatisches Schliessen beim Prozess-Ende
   (process_terminate()) ist ebenfalls NICHT verdrahtet - bewusst
   entkoppelt, analog zum bereits bestehenden Muster bei object.c/heap.c
   (keine Cross-Modul-Kopplung in process.c hinein) - ein kuenftiger
   Reaper-Mechanismus kann `ipc_endpoint_close()` fuer jeden vom
   terminierten Prozess gehaltenen Endpoint aufrufen. */

#define IPC_ENDPOINT_CAPACITY         8u
#define IPC_ENDPOINT_QUEUE_CAPACITY   8u
#define IPC_ENDPOINT_ALLOWED_SENDERS  4u

#define IPC_ENDPOINT_STATE_CLOSED 0u
#define IPC_ENDPOINT_STATE_OPEN   1u

#define IPC_ERR_NOT_FOUND (-1) /* unbekannte endpoint_id */
#define IPC_ERR_CLOSED    (-2) /* Endpoint existiert, ist aber geschlossen */
#define IPC_ERR_DENIED    (-3) /* Aufrufer ist weder Owner noch auf der Allowlist */
#define IPC_ERR_FULL      (-4) /* Backpressure: Warteschlange voll, REJECT */
#define IPC_ERR_CAPACITY  (-5) /* Endpoint- oder Allowlist-Tabelle voll */

typedef struct {
    uint32_t owner_pid;
    uint32_t state;       /* IPC_ENDPOINT_STATE_* */
    uint32_t capacity;
    uint32_t queue_depth; /* aktuell wartende Nachrichten */
    uint32_t sent_total;
    uint32_t received_total;
    uint32_t dropped_total; /* Backpressure-Ablehnungen (voll) */
    uint32_t denied_total;  /* Zugriffsverweigerungen (nicht autorisiert) */
} ipc_endpoint_info_t;

void ipc_endpoint_manager_initialize(void);
int ipc_endpoint_manager_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_endpoint_id gesetzt), IPC_ERR_CAPACITY wenn die
   Endpoint-Tabelle voll ist. capacity wird auf IPC_ENDPOINT_QUEUE_CAPACITY
   begrenzt (0 wird wie IPC_ENDPOINT_QUEUE_CAPACITY behandelt). */
int ipc_endpoint_create(uint32_t owner_pid, uint32_t capacity, uint32_t *out_endpoint_id);

/* Nur der Owner darf schliessen. 0 = Erfolg, IPC_ERR_NOT_FOUND/_DENIED
   sonst. Idempotent (bereits geschlossen -> weiterhin 0). */
int ipc_endpoint_close(uint32_t owner_pid, uint32_t endpoint_id);

/* Traegt sender_pid explizit in die Allowlist ein (Owner ist implizit
   IMMER sendeberechtigt, muss nicht eingetragen werden). Nur der Owner
   darf gewaehren. 0 = Erfolg, IPC_ERR_NOT_FOUND/_CLOSED/_DENIED/_CAPACITY
   (Allowlist voll). */
int ipc_endpoint_grant_send(uint32_t owner_pid, uint32_t endpoint_id, uint32_t sender_pid);

/* sender_pid MUSS Owner oder auf der Allowlist sein. 0 = Erfolg,
   IPC_ERR_NOT_FOUND/_CLOSED/_DENIED/_FULL (Backpressure-Reject, zaehlt
   dropped_total hoch). */
int ipc_endpoint_send(uint32_t sender_pid, uint32_t endpoint_id, const ipc_message_t *msg);

/* Nur der Owner darf empfangen. 0 = Erfolg (*out_msg gefuellt),
   IPC_ERR_NOT_FOUND/_CLOSED/_DENIED, oder IPC_ERR_FULL wiederverwendet
   als "Warteschlange leer" (kein eigener Code noetig - beide Faelle sind
   fuer den Aufrufer gleich zu behandeln: aktuell nichts verfuegbar). */
int ipc_endpoint_receive(uint32_t owner_pid, uint32_t endpoint_id, ipc_message_t *out_msg);

/* Reine Metadaten, NIE Nachrichteninhalte (NPSPEC-IPC-0001 Introspection-
   Offenlegungsverbot). 0 = Erfolg (*out_info gefuellt), IPC_ERR_NOT_FOUND
   sonst - funktioniert bewusst AUCH fuer geschlossene Endpoints (der
   letzte bekannte Zustand bleibt introspizierbar, bis die ID durch einen
   neuen ipc_endpoint_create()-Aufruf wiederverwendet wird). */
int ipc_endpoint_info(uint32_t endpoint_id, ipc_endpoint_info_t *out_info);

/* Wie die Zugriffspruefung innerhalb von ipc_endpoint_send(), aber OHNE
   etwas einzureihen - fuer Aufrufer (siehe rpc.c), die die Autorisierung
   eines Endpoints wiederverwenden wollen, ohne dessen rohe Byte-
   Warteschlange zu benutzen. 0 = sender_pid ist Owner oder autorisiert,
   IPC_ERR_NOT_FOUND/_CLOSED/_DENIED sonst. */
int ipc_endpoint_check_sender(uint32_t endpoint_id, uint32_t sender_pid);

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
