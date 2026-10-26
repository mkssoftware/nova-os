/* NovaOS C-Kernel - Nova Fault Injection Framework (Port aus
   adaptive32.inc, NPSPEC-RESILIENCE-FAULTINJECTION-0001). Kontrollierte
   Fehlerinjektion fuer Resilience-Tests: fi_inject aktiviert einen Fehler
   fuer ein Ziel-Subsystem, fi_check prueft ob ein aktiver injizierter
   Fehler vorliegt, fi_clear entfernt ihn. */
#ifndef NOVA_FI_H
#define NOVA_FI_H

#include <stdint.h>

#define FI_CAPACITY 8u

void fi_initialize(void);
int fi_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int fi_inject(uint32_t target_subsystem, uint32_t fault_type);
/* 0 = Fehler aktiv (*out_fault_type gesetzt), -1 = kein aktiver Fehler. */
int fi_check(uint32_t target_subsystem, uint32_t *out_fault_type);
void fi_clear(uint32_t target_subsystem);

#endif
