/* NovaOS C-Kernel - Nova Adaptive Prediction (EWMA) (Port aus
   adaptive32.inc, NPSPEC-ADAPTIVE-PREDICTION-0001). Exponentiell
   gewichteter gleitender Durchschnitt pro verfolgter Metrik:
   new_pred = (alpha*observed + (256-alpha)*old_pred) / 256, alpha=128
   entspricht 50% Gewichtung. */
#ifndef NOVA_APRED_H
#define NOVA_APRED_H

#include <stdint.h>

#define APRED_CAPACITY 8u

void apred_initialize(void);
int apred_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn metric_id==0, alpha==0 oder
   Tabelle voll. */
int apred_register(uint32_t metric_id, uint32_t initial_value, uint32_t alpha, uint32_t *out_slot);

/* 0 = Erfolg (*out_new_prediction gesetzt, samples erhoeht), -1 wenn
   metric_id unbekannt. */
int apred_update(uint32_t metric_id, uint32_t observed, uint32_t *out_new_prediction);

/* 0 = Erfolg (out_prediction/out_samples gesetzt), -1 wenn metric_id
   unbekannt. */
int apred_query(uint32_t metric_id, uint32_t *out_prediction, uint32_t *out_samples);

#endif
