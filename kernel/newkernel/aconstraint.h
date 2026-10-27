/* NovaOS C-Kernel - Nova Autonomy Constraint Engine (Port aus
   adaptive32.inc, NPSPEC-AUTONOMY-CONSTRAINT-0001). Harte Schranken, die
   autonome Aktionen gaten: bevor ein Autonomie-Subsystem (selfheal,
   selfopt, autopol) eine Aktion ausfuehrt, muss sie die Constraint-Engine
   passieren. */
#ifndef NOVA_ACONSTRAINT_H
#define NOVA_ACONSTRAINT_H

#include <stdint.h>

#define ACONSRT_CAPACITY 8u

#define ACONSRT_MAX   1u
#define ACONSRT_MIN   2u
#define ACONSRT_RANGE 3u

#define ACONSRT_DOM_CPU   1u
#define ACONSRT_DOM_MEM   2u
#define ACONSRT_DOM_RATE  3u
#define ACONSRT_DOM_STATE 4u

void aconstraint_initialize(void);
int aconstraint_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn constraint_id==0 oder Tabelle
   voll. RANGE wird (wie im alten ASM, bewusst vereinfacht) wie MAX
   behandelt: nur eine obere Schranke in threshold. */
int aconstraint_register(uint32_t constraint_id, uint32_t domain, uint32_t limit_type,
                          uint32_t threshold, uint32_t *out_slot);

/* 0 = erlaubt, -1 = verweigert (Violation-Zaehler erhoeht) oder
   constraint_id unbekannt. */
int aconstraint_enforce(uint32_t constraint_id, uint32_t observed_value);

#endif
