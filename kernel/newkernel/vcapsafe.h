/* NovaOS C-Kernel - Nova Capability Safety Verifier (Port aus
   adaptive32.inc, NPSPEC-VERIFY-CAPABILITYSAFETY-0001). Prueft
   Capability-Nutzung gegen einen Proof-Record: jede Nutzung muss von
   einem gueltigen Proof-Token begleitet werden. Bei Mismatch wird
   `security_audit_log` aufgerufen (echte Cross-Modul-Verdrahtung mit
   audit.c, 1:1 wie im alten ASM). */
#ifndef NOVA_VCAPSAFE_H
#define NOVA_VCAPSAFE_H

#include <stdint.h>

#define VCAPSAFE_CAPACITY 8u

void vcapsafe_initialize(void);
int vcapsafe_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn cap_id==0 oder Tabelle voll. */
int vcapsafe_register(uint32_t cap_id, uint32_t proof_token);

/* 0 = Proof stimmt (uses erhoeht), -1 = Mismatch (violations erhoeht,
   security_audit_log aufgerufen) oder cap_id unbekannt. */
int vcapsafe_verify(uint32_t cap_id, uint32_t supplied_proof);

#endif
