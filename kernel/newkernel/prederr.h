/* NovaOS C-Kernel - Nova Prediction Error Tracker (Port aus
   adaptive32.inc, NPSPEC-ADAPTIVE-PREDICTIONERROR-0001). Misst den
   absoluten Fehler zwischen `apred_query`-Predictions und beobachteten
   Werten, verfolgt eine EWMA des Fehlerbetrags je Metrik. Ueberschreitet
   der Fehler einen Schwellwert, ruft es `pollearn_feedback(metric_id, 0)`
   auf (echte Cross-Modul-Verdrahtung mit apred.c UND pollearn.c, 1:1 wie
   im alten ASM). */
#ifndef NOVA_PREDERR_H
#define NOVA_PREDERR_H

#include <stdint.h>

#define PREDERR_CAPACITY  8u
#define PREDERR_ALPHA     48u /* EWMA-Alpha fuer die Fehlermittelung */
#define PREDERR_THRESHOLD 40u /* avg_error darueber -> pollearn-Fehlschlag */

void prederr_initialize(void);
int prederr_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn metric_id==0 oder Tabelle voll. */
int prederr_register(uint32_t metric_id);

/* 0 = Erfolg (Samples-Zaehler erhoeht), -1 wenn metric_id unbekannt bei
   prederr ODER keine Prediction bei apred_query gefunden. */
int prederr_observe(uint32_t metric_id, uint32_t observed);

/* 0 = Erfolg (out_avg_error/out_last_error/out_samples gesetzt), -1 wenn
   metric_id unbekannt. */
int prederr_query(uint32_t metric_id, uint32_t *out_avg_error, uint32_t *out_last_error,
                   uint32_t *out_samples);

#endif
