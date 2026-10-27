/* NovaOS C-Kernel - Nova Autonomy Policy Manager (Port aus
   adaptive32.inc). */
#include "autopol.h"
#include "aconstraint.h"

typedef struct {
    uint32_t id; /* 0 = frei */
    uint32_t trigger;
    uint32_t action;
    uint32_t fires;
} autopol_rec_t;

static autopol_rec_t autopol_table[AUTOPOL_CAPACITY];
static int autopol_ready = 0;

void autopol_initialize(void) {
    if (autopol_ready == 1) return;

    for (uint32_t i = 0; i < AUTOPOL_CAPACITY; ++i) autopol_table[i] = (autopol_rec_t){0};
    autopol_ready = 1;
}

int autopol_register(uint32_t policy_id, uint32_t trigger_type, uint32_t action, uint32_t *out_slot) {
    if (autopol_ready != 1 || policy_id == 0) return -1;

    for (uint32_t i = 0; i < AUTOPOL_CAPACITY; ++i) {
        if (autopol_table[i].id == 0) {
            autopol_table[i].id = policy_id;
            autopol_table[i].trigger = trigger_type;
            autopol_table[i].action = action;
            autopol_table[i].fires = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int autopol_evaluate(uint32_t trigger_event, uint32_t *out_action, uint32_t *out_fire_count) {
    if (autopol_ready != 1) return -1;

    uint32_t last_action = 0;
    uint32_t matched = 0;

    for (uint32_t i = 0; i < AUTOPOL_CAPACITY; ++i) {
        autopol_rec_t *rec = &autopol_table[i];
        if (rec->id == 0 || rec->trigger != trigger_event) continue;

        if (aconstraint_enforce(rec->id, 0) != 0) continue; /* Constraint verweigert */

        ++rec->fires;
        last_action = rec->action;
        ++matched;
    }

    if (matched == 0) return -1;

    *out_action = last_action;
    *out_fire_count = matched;
    return 0;
}

int autopol_self_test(void) {
    /* Stellt sicher, dass fuer policy_id=1 eine erlaubende Constraint
       existiert, unabhaengig davon ob aconstraint.c's eigener Selbsttest
       in diesem Boot bereits gelaufen ist (siehe die Lehre aus
       selfopt_self_test: Modul-Zustand ist nie testlauf-isoliert). */
    aconstraint_initialize();
    uint32_t constraint_slot;
    aconstraint_register(1, ACONSRT_DOM_CPU, ACONSRT_MAX, 200, &constraint_slot);

    uint32_t slot;
    if (autopol_register(1, AUTOPOL_TRIG_FAULT, AUTOPOL_ACT_RESTART, &slot) != 0) return -1;

    uint32_t action, fire_count;
    if (autopol_evaluate(AUTOPOL_TRIG_FAULT, &action, &fire_count) != 0) return -1;
    if (action != AUTOPOL_ACT_RESTART) return -1;

    if (autopol_evaluate(AUTOPOL_TRIG_OVERLOAD, &action, &fire_count) == 0) return -1;

    return 0;
}
