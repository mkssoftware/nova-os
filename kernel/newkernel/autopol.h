/* NovaOS C-Kernel - Nova Autonomy Policy Manager (Port aus
   adaptive32.inc, NPSPEC-AUTONOMY-POLICY-0001). Ereignisgesteuerte
   autonome Policy: jede Policy hat eine Trigger-Bedingung und einen
   Aktionstyp. `autopol_evaluate` prueft alle passenden Policies gegen
   aconstraint.h (policy_id wird dabei als constraint_id verwendet, mit
   observed_value=0 - 1:1 wie im alten ASM, eine bewusste Vereinfachung
   des Original-NPSPECs) und dispatcht die erlaubten. */
#ifndef NOVA_AUTOPOL_H
#define NOVA_AUTOPOL_H

#include <stdint.h>

#define AUTOPOL_CAPACITY 8u

#define AUTOPOL_TRIG_FAULT     1u
#define AUTOPOL_TRIG_OVERLOAD  2u
#define AUTOPOL_TRIG_UNDERLOAD 3u
#define AUTOPOL_TRIG_TIMEOUT   4u
#define AUTOPOL_TRIG_MANUAL    5u

#define AUTOPOL_ACT_NOP        0u
#define AUTOPOL_ACT_SCALE_UP   1u
#define AUTOPOL_ACT_SCALE_DOWN 2u
#define AUTOPOL_ACT_RESTART    3u
#define AUTOPOL_ACT_ALERT      4u

void autopol_initialize(void);
int autopol_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn policy_id==0 oder Tabelle voll. */
int autopol_register(uint32_t policy_id, uint32_t trigger_type, uint32_t action, uint32_t *out_slot);

/* 0 = mindestens eine Policy gefeuert (*out_action = letzte dispatchte
   Aktion, *out_fire_count gesetzt), -1 = keine passende/erlaubte Policy. */
int autopol_evaluate(uint32_t trigger_event, uint32_t *out_action, uint32_t *out_fire_count);

#endif
