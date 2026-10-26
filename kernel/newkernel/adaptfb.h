/* NovaOS C-Kernel - Nova Adaptive Feedback Loop (Port aus adaptive32.inc,
   NPSPEC-ADAPTIVE-FEEDBACK-0001). Schliesst den Regelkreis: Observe ->
   Predict -> Decide -> Execute -> Measure -> Feedback. Jeder Slot
   ueberwacht eine Policy-Dimension (Thermal/QoS/Sched/Memory). */
#ifndef NOVA_ADAPTFB_H
#define NOVA_ADAPTFB_H

#include <stdint.h>

#define ADAPT_SLOT_COUNT 8u

#define ADAPT_DIM_THERMAL 1u
#define ADAPT_DIM_QOS     2u
#define ADAPT_DIM_SCHED   3u
#define ADAPT_DIM_MEMORY  4u

void adapt_initialize(void);
int adapt_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn Tabelle voll. */
int adapt_register(uint32_t dimension, uint32_t expected_value, uint32_t *out_slot);

/* 0 = innerhalb des Budgets, -1 wenn miss_threshold erreicht/ueberschritten
   (Policy-Anpassung signalisiert) oder slot ungueltig. */
int adapt_observe(uint32_t slot, uint32_t actual_value, uint32_t miss_threshold);

#endif
