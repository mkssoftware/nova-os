/* NovaOS C-Kernel - Nova Resource Guarantee (Port aus adaptive32.inc,
   NPSPEC-RESOURCE-GUARANTEE-0001). Reserviert eine Mindest-CPU/Memory-
   Zuteilung je Owner. Verbindet sich konzeptionell mit radmit.h (die
   Garantie-Untergrenze soll vor jeder Admission eingehalten werden) -
   die Verdrahtung selbst ist NICHT portiert. */
#ifndef NOVA_RGUARANT_H
#define NOVA_RGUARANT_H

#include <stdint.h>

#define RGUARANT_CAPACITY 8u

void rguarant_initialize(void);
int rguarant_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt; ueberschreibt eine vorhandene Garantie
   desselben owner_id), -1 wenn owner_id==0 oder Tabelle voll. */
int rguarant_set(uint32_t owner_id, uint32_t min_cpu, uint32_t min_mem, uint32_t *out_slot);

/* 0 = Garantie gefunden (out_min_cpu/out_min_mem gesetzt), -1 = keine
   aktive Garantie fuer owner_id. */
int rguarant_check(uint32_t owner_id, uint32_t *out_min_cpu, uint32_t *out_min_mem);

#endif
