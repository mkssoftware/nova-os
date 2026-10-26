/* NovaOS C-Kernel - Nova RT Temporal Isolation (Port aus adaptive32.inc,
   NPSPEC-REALTIME-TEMPORALISOLATION-0001). Zeit-Partitionierung: jede
   RT-Partition erhaelt ein Budget pro Epoche (Ticks); verhindert dass eine
   RT-Task andere temporaer aushungert. NICHT zu verwechseln mit
   scheduler.c's QoS-Klassen (qos_consume/qos_new_epoch) - ein
   eigenstaendiges, schon im alten ASM paralleles Subsystem. */
#ifndef NOVA_TMPISO_H
#define NOVA_TMPISO_H

#include <stdint.h>

#define TMPISO_CAPACITY 4u

void tmpiso_initialize(void);
int tmpiso_self_test(void); /* 0 = ok */

/* 0 = zugelassen (Budget verbraucht), -1 = Budget erschoepft oder
   partition ungueltig. */
int tmpiso_admit(uint32_t partition, uint32_t ticks_needed);
void tmpiso_new_epoch(void); /* setzt alle Partitions-Budgets zurueck */
/* 0 = Erfolg (out_budget und out_used gesetzt), -1 wenn partition ungueltig. */
int tmpiso_query(uint32_t partition, uint32_t *out_budget, uint32_t *out_used);

#endif
