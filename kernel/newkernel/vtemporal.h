/* NovaOS C-Kernel - Nova Temporal Invariant Verifier (Port aus
   adaptive32.inc, NPSPEC-VERIFY-TEMPORAL-0001). Verifiziert, dass
   zeitgebundene Invarianten gelten: Ereignis B muss innerhalb
   [min_gap, max_gap] nach Anchor-Ereignis A auftreten (max_gap = 2x
   min_gap, wie im alten ASM vereinfacht). Bei Verletzung wird das
   LATENCY-Symptom via `sdiag_diagnose` gemeldet (echte Cross-Modul-
   Verdrahtung mit sdiag.c, 1:1 wie im alten ASM). */
#ifndef NOVA_VTEMPORAL_H
#define NOVA_VTEMPORAL_H

#include <stdint.h>

#define VTEMPORAL_CAPACITY 8u

void vtemporal_initialize(void);
int vtemporal_self_test(void); /* 0 = ok */

/* 0 = Erfolg (max_gap = 2*min_gap), -1 wenn inv_id==0 oder Tabelle voll. */
int vtemporal_register(uint32_t inv_id, uint32_t anchor_tick, uint32_t min_gap);

/* 0 = innerhalb des Fensters, -1 = verletzt (sdiag_diagnose aufgerufen)
   oder inv_id unbekannt. */
int vtemporal_check(uint32_t inv_id, uint32_t event_tick);

#endif
