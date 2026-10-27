/* NovaOS C-Kernel - Nova Deadlock Detection (Port aus sync32.inc,
   NPSPEC-SYNC-DEADLOCK-0001). Erkennt zyklische Warteabhaengigkeiten
   zwischen Tasks via Wait-For-Graph (Adjazenzmatrix) und DFS-basierter
   Zykluserkennung. */
#ifndef NOVA_DEADLOCK_H
#define NOVA_DEADLOCK_H

#include <stdint.h>

#define DL_NODE_COUNT 8u

void deadlock_initialize(void);
int deadlock_self_test(void); /* 0 = ok */

/* Traegt ein: Task task_id wartet auf Task waits_on_id (beide <
   DL_NODE_COUNT, != einander). 0 = ok, -1 = Deadlock erkannt (Kante
   bleibt trotzdem eingetragen, 1:1 wie im alten ASM) oder ungueltige
   Parameter. */
int deadlock_add_wait(uint32_t task_id, uint32_t waits_on_id);

/* Entfernt die Kante wieder. */
void deadlock_remove_wait(uint32_t task_id, uint32_t waits_on_id);

/* DFS-basierte Zykluserkennung ueber den gesamten Graphen.
   0 = kein Zyklus, -1 = Zyklus gefunden. */
int deadlock_detect(void);

#endif
