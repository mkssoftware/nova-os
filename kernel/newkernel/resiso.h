/* NovaOS C-Kernel - Nova Resilience Isolation (Port aus adaptive32.inc,
   NPSPEC-RESILIENCE-ISOLATION-0001). Verfolgt Fault-Isolation-Domains:
   jede Domain hat ein Quarantine-Flag. Entkommt ein Fehler der
   Eindaemmung, wird seine Domain quarantiert um Kaskaden zu verhindern. */
#ifndef NOVA_RESISO_H
#define NOVA_RESISO_H

#include <stdint.h>

#define RESISO_CAPACITY 8u

void resilisolation_initialize(void);
int resilisolation_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn domain_id==0 oder Tabelle voll. */
int resilisolation_register(uint32_t domain_id, uint32_t *out_slot);

/* Zeichnet einen Fehler auf. 0 = noch unter dem Schwellwert, -1 = Domain
   wurde dadurch (neu) quarantiert ODER war bereits quarantiert ODER
   domain_id unbekannt. */
int resilisolation_record_fault(uint32_t domain_id, uint32_t fault_count_threshold);

/* 0 = Erfolg (out_fault_count, out_quarantined gesetzt), -1 wenn domain_id
   unbekannt. */
int resilisolation_query(uint32_t domain_id, uint32_t *out_fault_count, uint32_t *out_quarantined);

#endif
