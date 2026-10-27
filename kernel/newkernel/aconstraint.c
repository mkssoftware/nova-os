/* NovaOS C-Kernel - Nova Autonomy Constraint Engine (Port aus
   adaptive32.inc). */
#include "aconstraint.h"

typedef struct {
    uint32_t id; /* 0 = frei */
    uint32_t domain;
    uint32_t limit_type;
    uint32_t threshold;
    uint32_t violations;
} aconsrt_rec_t;

static aconsrt_rec_t aconsrt_table[ACONSRT_CAPACITY];
static int aconsrt_ready = 0;

void aconstraint_initialize(void) {
    if (aconsrt_ready == 1) return;

    for (uint32_t i = 0; i < ACONSRT_CAPACITY; ++i) aconsrt_table[i] = (aconsrt_rec_t){0};
    aconsrt_ready = 1;
}

int aconstraint_register(uint32_t constraint_id, uint32_t domain, uint32_t limit_type,
                          uint32_t threshold, uint32_t *out_slot) {
    if (aconsrt_ready != 1 || constraint_id == 0) return -1;

    for (uint32_t i = 0; i < ACONSRT_CAPACITY; ++i) {
        if (aconsrt_table[i].id == 0) {
            aconsrt_table[i].id = constraint_id;
            aconsrt_table[i].domain = domain;
            aconsrt_table[i].limit_type = limit_type;
            aconsrt_table[i].threshold = threshold;
            aconsrt_table[i].violations = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int aconstraint_enforce(uint32_t constraint_id, uint32_t observed_value) {
    if (aconsrt_ready != 1) return -1;

    for (uint32_t i = 0; i < ACONSRT_CAPACITY; ++i) {
        aconsrt_rec_t *rec = &aconsrt_table[i];
        if (rec->id != constraint_id) continue;

        int allowed;
        if (rec->limit_type == ACONSRT_MIN) {
            allowed = (observed_value >= rec->threshold);
        } else {
            /* MAX und RANGE (vereinfacht wie im alten ASM): obere Schranke */
            allowed = (observed_value <= rec->threshold);
        }

        if (!allowed) {
            ++rec->violations;
            return -1;
        }
        return 0;
    }
    return -1;
}

int aconstraint_self_test(void) {
    uint32_t slot;
    if (aconstraint_register(1, ACONSRT_DOM_CPU, ACONSRT_MAX, 200, &slot) != 0) return -1;

    if (aconstraint_enforce(1, 100) != 0) return -1; /* erlaubt */
    if (aconstraint_enforce(1, 201) == 0) return -1; /* verweigert */

    if (aconsrt_table[slot].violations != 1) return -1;

    return 0;
}
