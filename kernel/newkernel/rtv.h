/* NovaOS C-Kernel - Nova Runtime Verification Monitor (Port aus
   adaptive32.inc, NPSPEC-VERIFY-RUNTIME-0001). Ueberwacht registrierte
   Runtime Contracts (Invarianten): lower_bound <= observed_value <=
   upper_bound. Verletzung -> security_audit_log (audit.h). NICHT zu
   verwechseln mit verify.c (verify32.inc, NPSPEC-VERIFY-*) - ein
   eigenstaendiges, schon im alten ASM paralleles Subsystem mit anderem
   Kontrakt-Modell (bounds statt typisierter Pred/Postcond-Hashes). */
#ifndef NOVA_RTV_H
#define NOVA_RTV_H

#include <stdint.h>

#define RTV_CAPACITY 8u

void rtv_initialize(void);
int rtv_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int rtv_register(uint32_t contract_id, uint32_t lower_bound, uint32_t upper_bound);

/* 0 = eingehalten, -1 = verletzt (security_audit_log aufgerufen) oder
   contract_id unbekannt. */
int rtv_check(uint32_t contract_id, uint32_t observed_value);

#endif
