/* NovaOS C-Kernel - Nova Resilience Verification (Port aus adaptive32.inc,
   NPSPEC-RESILIENCE-VERIFICATION-0001). Prueft nach einer Recovery-Aktion
   ob das Subsystem in einen gesunden Zustand zurueckgekehrt ist. Bei
   Abweichung: Audit-Eintrag via security_audit_log (audit.h). */
#ifndef NOVA_RVERIF_H
#define NOVA_RVERIF_H

#include <stdint.h>

#define RVERIF_CAPACITY 8u

#define RVERIF_UNKNOWN  0u
#define RVERIF_HEALTHY  1u
#define RVERIF_DEGRADED 2u
#define RVERIF_FAILED   3u

void rverif_initialize(void);
int rverif_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn subsys_id==0 oder Tabelle voll. */
int rverif_register(uint32_t subsys_id, uint32_t expected_state, uint32_t *out_slot);

/* 0 = observed_state == expected_state, -1 = Abweichung (Audit-Log-Eintrag
   erzeugt) oder subsys_id unbekannt/ungueltig. */
int rverif_check(uint32_t subsys_id, uint32_t observed_state);

/* 0 = Erfolg (out_observed und out_expected gesetzt), -1 wenn subsys_id
   unbekannt/ungueltig. */
int rverif_query(uint32_t subsys_id, uint32_t *out_observed, uint32_t *out_expected);

#endif
