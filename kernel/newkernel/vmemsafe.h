/* NovaOS C-Kernel - Nova Memory Safety Verifier (Port aus adaptive32.inc,
   NPSPEC-VERIFY-MEMORYSAFETY-0001). Bounds-Checker fuer registrierte
   Speicherregionen: eine Adresse ausserhalb von [base, base+size) einer
   Region ist eine Verletzung. Bei Verletzung wird `security_audit_log`
   aufgerufen (echte Cross-Modul-Verdrahtung mit audit.c, 1:1 wie im
   alten ASM). Letztes Modul von adaptive32.inc. */
#ifndef NOVA_VMEMSAFE_H
#define NOVA_VMEMSAFE_H

#include <stdint.h>

#define VMEMSAFE_CAPACITY 8u

void vmemsafe_initialize(void);
int vmemsafe_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn region_id==0 oder Tabelle voll. */
int vmemsafe_register(uint32_t region_id, uint32_t base, uint32_t size);

/* 0 = Adresse innerhalb [base, base+size), -1 = Verletzung (violations
   erhoeht, security_audit_log aufgerufen) oder region_id unbekannt. */
int vmemsafe_check(uint32_t region_id, uint32_t addr);

#endif
