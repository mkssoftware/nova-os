/* NovaOS C-Kernel - Nova Retry Manager mit Exponential Backoff (Port aus
   adaptive32.inc, NPSPEC-RESILIENCE-RETRY-0001 + NPSPEC-RESILIENCE-BACKOFF-0001).
   Jeder Retry-Slot verwaltet attempts/max_attempts/backoff_ticks. */
#ifndef NOVA_RETRY_H
#define NOVA_RETRY_H

#include <stdint.h>

#define RETRY_CAPACITY 8u

void retry_initialize(void);
int retry_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn Tabelle voll. backoff startet bei
   base_delay_ticks. */
int retry_register(uint32_t max_attempts, uint32_t base_delay_ticks, uint32_t *out_slot);

/* 0 = nochmal versuchen (backoff verdoppelt, saettigt bei 0x7FFFFFFF),
   -1 = max_attempts erreicht oder slot ungueltig/frei. */
int retry_attempt(uint32_t slot);

/* 0 = Erfolg (*out_backoff_ticks, *out_attempts_remaining gesetzt),
   -1 wenn slot ungueltig. */
int retry_query_backoff(uint32_t slot, uint32_t *out_backoff_ticks, uint32_t *out_attempts_remaining);

#endif
