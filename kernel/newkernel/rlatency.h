/* NovaOS C-Kernel - Nova Resource Latency Monitor (Port aus adaptive32.inc,
   NPSPEC-RESOURCE-LATENCY-0001). Trackt Round-Trip-Latenz pro Endpoint via
   EWMA. Bei SLA-Ueberschreitung: Prediction-Update via `apred_update` und
   LATENCY-Symptom via `sdiag_diagnose` (echte Cross-Modul-Verdrahtung mit
   apred.c und sdiag.c, 1:1 wie im alten ASM). */
#ifndef NOVA_RLATENCY_H
#define NOVA_RLATENCY_H

#include <stdint.h>

#define RLAT_CAPACITY 8u

void rlatency_initialize(void);
int rlatency_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn endpoint_id==0 oder Tabelle voll. */
int rlatency_register(uint32_t endpoint_id, uint32_t sla_us);

/* 0 = Erfolg (unter SLA), -1 = SLA ueberschritten (Violation gezaehlt,
   apred_update + sdiag_diagnose aufgerufen) oder Endpoint unbekannt. */
int rlatency_observe(uint32_t endpoint_id, uint32_t observed_us);

/* 0 = Erfolg, -1 wenn Endpoint unbekannt. */
int rlatency_query(uint32_t endpoint_id, uint32_t *out_avg_lat, uint32_t *out_sla_us, uint32_t *out_violations);

#endif
