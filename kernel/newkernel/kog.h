/* NovaOS C-Kernel - §100 Kernel Object Graph (Port aus security32.inc,
   NPSPEC-KERNEL-0100). Typisierter gerichteter Multigraph fuer alle
   Kernelressourcen. Bootstrap-Implementierung: feste Pools, kein
   RCU/Snapshot/Transaktion. */
#ifndef NOVA_KOG_H
#define NOVA_KOG_H

#include <stdint.h>

#define NP_OBJTYPE_KERNEL     1u
#define NP_OBJTYPE_MACHINE    2u
#define NP_OBJTYPE_NAMESPACE  3u
#define NP_OBJTYPE_PROCESS    4u
#define NP_OBJTYPE_THREAD     5u
#define NP_OBJTYPE_CPU        6u
#define NP_OBJTYPE_MEMORY     7u
#define NP_OBJTYPE_IPC        8u
#define NP_OBJTYPE_DEVICE     9u
#define NP_OBJTYPE_DRIVER     10u
#define NP_OBJTYPE_VFS_NODE   11u
#define NP_OBJTYPE_SECURITY   12u
#define NP_OBJTYPE_DIAGNOSTIC 13u
#define NP_OBJTYPE_POWER      14u
#define NP_OBJTYPE_GENERIC    15u
#define NP_OBJTYPE_COUNT      16u

#define NP_GRAPH_OBJ_INITIALIZING 0u
#define NP_GRAPH_OBJ_ACTIVE       1u
#define NP_GRAPH_OBJ_QUIESCING    2u
#define NP_GRAPH_OBJ_FAILED       3u
#define NP_GRAPH_OBJ_REMOVING     4u
#define NP_GRAPH_OBJ_DESTROYED    5u

#define NP_RELATION_OWNS       0u
#define NP_RELATION_CONTAINS   1u
#define NP_RELATION_PARENT_OF  2u
#define NP_RELATION_CHILD_OF   3u
#define NP_RELATION_DEPENDS_ON 4u
#define NP_RELATION_BOUND_TO   5u
#define NP_RELATION_PROVIDES   6u
#define NP_RELATION_CONSUMES   7u
#define NP_RELATION_MEMBER_OF  8u
#define NP_RELATION_MAPPED_TO  9u
#define NP_RELATION_OBSERVES   10u
#define NP_RELATION_SECURED_BY 11u
#define NP_RELATION_TYPE_COUNT 12u

#define NP_RELSTR_WEAK      0u
#define NP_RELSTR_REFERENCE 1u
#define NP_RELSTR_STRONG    2u
#define NP_RELSTR_OWNERSHIP 3u

#define KOG_SCHEMA_ALLOW_CYCLE 0x01u

#define KOG_MAX_OBJECTS    64u
#define KOG_MAX_RELATIONS  128u
#define KOG_CYCLE_MAX_DEPTH 8u

void kog_initialize(void); /* 0 = Erfolg (Schemata + Root-Objekte angelegt) */
int kog_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_obj_id gesetzt), -1 wenn Pool voll. */
int kog_alloc_object(uint32_t type, uint32_t *out_obj_id);
int kog_activate_object(uint32_t obj_id); /* 0 = Erfolg, -1 wenn nicht INITIALIZING/nicht gefunden */

/* 0 = Erfolg (*out_rel_id gesetzt), -1 bei ungueltigem Objekt/Schema/Zyklus/voll. */
int kog_link(uint32_t src_id, uint32_t dst_id, uint32_t rel_type, uint32_t strength, uint32_t *out_rel_id);
int kog_unlink(uint32_t rel_id); /* 0 = Erfolg, -1 wenn nicht gefunden */

/* 0 = kein Zyklus, -1 = Zyklus erkannt (nur fuer OWNS/CONTAINS/BOUND_TO relevant). */
int kog_check_cycle(uint32_t src_id, uint32_t dst_id, uint32_t rel_type);

#endif
