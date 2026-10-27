/* NovaOS C-Kernel - Nova Adaptive Prefetch Engine (Port aus
   adaptive32.inc, NPSPEC-ADAPTIVE-PREFETCH-0001). Verfolgt Speicherzugriffs-
   Strides je Stream und signalisiert Prefetch, wenn der Stride stabil
   ist (EWMA-Konfidenz). */
#ifndef NOVA_APREFETCH_H
#define NOVA_APREFETCH_H

#include <stdint.h>

#define APREFETCH_CAPACITY 8u
#define APREFETCH_ALPHA    64u  /* Konfidenz-EWMA-Alpha */
#define APREFETCH_TRIGGER  160u /* Prefetch wenn Konfidenz > diesem Wert */

void aprefetch_initialize(void);
int aprefetch_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn stream_id==0 oder Tabelle voll. */
int aprefetch_register(uint32_t stream_id);

/* 0 = genug Konfidenz fuer eine Prefetch-Vorhersage (*out_prefetch_addr
   gesetzt), -1 = erster Zugriff ODER noch nicht konfident genug ODER
   stream_id unbekannt. */
int aprefetch_access(uint32_t stream_id, uint32_t new_addr, uint32_t *out_prefetch_addr);

#endif
