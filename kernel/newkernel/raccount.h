/* NovaOS C-Kernel - Nova Resource Accounting (Port aus adaptive32.inc,
   NPSPEC-RESOURCE-ACCOUNTING-0001). Per-Owner CPU/Memory-Verbrauchs-
   Ledger. Verbindet sich konzeptionell mit radmit.h (jeder zugelassene
   Slot wird dem Owner sofort belastet) - die Verdrahtung selbst ist
   NICHT portiert (radmit.h kennt keinen Owner-Charge-Callback). */
#ifndef NOVA_RACCOUNT_H
#define NOVA_RACCOUNT_H

#include <stdint.h>

#define RACCOUNT_CAPACITY 8u

void raccount_initialize(void);
int raccount_self_test(void); /* 0 = ok */

/* 0 = Erfolg (cpu_delta/mem_delta aufaddiert, Ledger-Eintrag bei Bedarf
   neu angelegt), -1 wenn owner_id==0 oder Tabelle voll. */
int raccount_charge(uint32_t owner_id, uint32_t cpu_delta, uint32_t mem_delta);

/* 0 = Erfolg (out_cpu_used/out_mem_used gesetzt), -1 wenn owner_id
   unbekannt. */
int raccount_query(uint32_t owner_id, uint32_t *out_cpu_used, uint32_t *out_mem_used);

#endif
