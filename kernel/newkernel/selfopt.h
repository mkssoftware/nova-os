/* NovaOS C-Kernel - Nova Self-Optimization (Port aus adaptive32.inc,
   NPSPEC-AUTONOMY-SELFOPTIMIZATION-0001). Passt abstimmbare Parameter
   anhand von Prediction-Feedback (apred.h) an ein Optimierungsziel an.
   Jedes Ziel hat eine metric_id, einen Zielwert (goal) und eine
   Schrittweite (step). */
#ifndef NOVA_SELFOPT_H
#define NOVA_SELFOPT_H

#include <stdint.h>

#define SELFOPT_CAPACITY 8u

void selfopt_initialize(void);
int selfopt_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn target_id==0, metric_id==0 oder
   Tabelle voll. */
int selfopt_register(uint32_t target_id, uint32_t metric_id, uint32_t goal,
                      uint32_t step, uint32_t *out_slot);

/* Fragt die aktuelle Prediction via apred_query ab und vergleicht sie mit
   dem Zielwert. 0 = Erfolg (*out_direction = -1 verringern / 0 am Ziel /
   1 erhoehen - 0 auch wenn (noch) keine Prediction existiert), -1 wenn
   target_id unbekannt. */
int selfopt_step(uint32_t target_id, int32_t *out_direction);

#endif
