/* NovaOS C-Kernel - §101 Event Bus (Port aus security32.inc,
   NPSPEC-KERNEL-0101). Publish-Subscribe-Infrastruktur fuer
   Kernelkomponenten. Bootstrap: synchrone Callback-Zustellung, statische
   Pools, kein Scheduler. */
#ifndef NOVA_EVBUS_H
#define NOVA_EVBUS_H

#include <stdint.h>

#define NP_EVENT_CLASS_STATE        0u
#define NP_EVENT_CLASS_LIFECYCLE    1u
#define NP_EVENT_CLASS_RESOURCE     2u
#define NP_EVENT_CLASS_SECURITY     3u
#define NP_EVENT_CLASS_ERROR        4u
#define NP_EVENT_CLASS_DIAGNOSTIC   5u
#define NP_EVENT_CLASS_COMPLETION   6u
#define NP_EVENT_CLASS_NOTIFICATION 7u

#define NP_EVENT_SYNCHRONOUS   0x001u
#define NP_EVENT_ASYNCHRONOUS  0x002u
#define NP_EVENT_RELIABLE      0x004u
#define NP_EVENT_COALESCABLE   0x008u
#define NP_EVENT_HIGH_PRIORITY 0x010u
#define NP_EVENT_REPLAYABLE    0x020u
#define NP_EVENT_AUDITED       0x040u
#define NP_EVENT_SENSITIVE     0x080u
#define NP_EVENT_KERNEL_ONLY   0x100u

#define NP_EVENT_DELIVERY_CALLBACK 0u
#define NP_EVENT_DELIVERY_QUEUE    1u
#define NP_EVENT_DELIVERY_IPC      2u
#define NP_EVENT_DELIVERY_SIGNAL   3u
#define NP_EVENT_DELIVERY_WAITABLE 4u

#define NP_SUB_CREATED 0u
#define NP_SUB_ACTIVE  1u
#define NP_SUB_PAUSED  2u
#define NP_SUB_CLOSING 3u
#define NP_SUB_CLOSED  4u

#define NP_EVENT_BOOT_READY        1u
#define NP_EVENT_BOOT_PANIC        2u
#define NP_EVENT_OBJ_CREATED       3u
#define NP_EVENT_OBJ_ACTIVATED     4u
#define NP_EVENT_OBJ_DESTROYED     5u
#define NP_EVENT_OBJ_STATE_CHANGED 6u
#define NP_EVENT_OBJ_LINK_ADDED    7u
#define NP_EVENT_OBJ_LINK_REMOVED  8u
#define NP_EVENT_BUILTIN_COUNT     8u

#define EVBUS_MAX_SCHEMAS 32u
#define EVBUS_MAX_SUBS    16u

typedef void (*evbus_handler_fn_t)(uint32_t type_id, uint32_t src_id, uint32_t subj_id);

/* Leert Pools, registriert die 8 eingebauten Event-Schemata. */
void evbus_initialize(void);
int evbus_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn type_id bereits registriert oder Pool voll. */
int evbus_register_schema(uint32_t type_id, uint32_t event_class, uint32_t max_payload_size, uint32_t flags);

/* 0 = Erfolg (*out_sub_id gesetzt), -1 wenn Pool voll. type_filter=0 = alle Typen. */
int evbus_subscribe(uint32_t type_filter, evbus_handler_fn_t handler, uint32_t delivery_mode, uint32_t *out_sub_id);

/* 0 = Erfolg, -1 wenn Schema unbekannt/Payload zu gross/re-entrant. */
int evbus_publish(uint32_t type_id, uint32_t src_id, uint32_t subj_id, const void *payload, uint32_t payload_size);

#endif
