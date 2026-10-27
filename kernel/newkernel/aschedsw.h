/* NovaOS C-Kernel - Nova Adaptive Scheduler (Port aus adaptive32.inc,
   NPSPEC-ADAPTIVE-SCHEDULER-0001). Passt Scheduler-Klassen-Gewichte
   anhand von QoS-Feedback an. Jede verfolgte Klasse hat ein aktuelles
   Gewicht und eine Richtung, in die das Feedback es verschiebt. NICHT
   verdrahtet mit `scheduler.c`s echten QoS-Klassen - eigenstaendiges
   Datenmodell, das eigene Owner-IDs als "class_id" verwendet. */
#ifndef NOVA_ASCHEDSW_H
#define NOVA_ASCHEDSW_H

#include <stdint.h>

#define ASCHEDSW_CAPACITY 8u

void aschedsw_initialize(void);
int aschedsw_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn class_id==0 oder Tabelle voll. */
int aschedsw_register(uint32_t class_id, uint32_t initial_weight, uint32_t min_weight,
                       uint32_t max_weight, uint32_t *out_slot);

/* Erhoeht/verringert das Gewicht um delta (positiv = erhoehen, negativ =
   verringern), geklemmt auf [min_weight, max_weight]. 0 = Erfolg
   (Adjustments-Zaehler erhoeht), -1 wenn class_id unbekannt. */
int aschedsw_feedback(uint32_t class_id, int32_t delta);

/* 0 = Erfolg (out_weight/out_adjustments gesetzt), -1 wenn class_id
   unbekannt. */
int aschedsw_query(uint32_t class_id, uint32_t *out_weight, uint32_t *out_adjustments);

#endif
