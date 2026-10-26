/* NovaOS C-Kernel - Nova State Reconciliation (Port aus adaptive32.inc,
   NPSPEC-STATE-RECONCILIATION-0001). Vergleicht gewuenschten Zustand gegen
   tatsaechlichen Zustand und markiert Abweichungen. */
#ifndef NOVA_SREC_H
#define NOVA_SREC_H

#include <stdint.h>

#define SREC_CAPACITY 8u

void srec_initialize(void);
int srec_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn rec_id==0 oder Tabelle voll. */
int srec_register(uint32_t rec_id, uint32_t desired_state, uint32_t *out_slot);

/* 0 = aligned (actual==desired), -1 = diverged (Zaehler erhoeht) oder
   rec_id unbekannt/ungueltig. */
int srec_reconcile(uint32_t rec_id, uint32_t actual_state);

/* 0 = Erfolg (out_desired, out_actual und out_diverge_count gesetzt),
   -1 wenn rec_id unbekannt/ungueltig. */
int srec_query(uint32_t rec_id, uint32_t *out_desired, uint32_t *out_actual, uint32_t *out_diverge_count);

#endif
