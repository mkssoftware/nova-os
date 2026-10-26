/* NovaOS C-Kernel - Nova Recovery Policy Manager (Port aus adaptive32.inc). */
#include "rpol.h"
#include "fclass.h"

typedef struct {
    uint32_t class_id; /* 0 = frei */
    uint32_t min_severity;
    uint32_t action;
    uint32_t fires;
} rpol_rec_t;

static rpol_rec_t rpol_table[RPOL_CAPACITY];
static int rpol_ready = 0;

void rpol_initialize(void) {
    if (rpol_ready == 1) return;

    for (uint32_t i = 0; i < RPOL_CAPACITY; ++i) rpol_table[i] = (rpol_rec_t){0};
    rpol_ready = 1;
}

int rpol_register(uint32_t class_id, uint32_t min_severity, uint32_t action, uint32_t *out_slot) {
    if (rpol_ready != 1) return -1;
    if (class_id == 0) return -1;

    for (uint32_t i = 0; i < RPOL_CAPACITY; ++i) {
        if (rpol_table[i].class_id == 0) {
            rpol_table[i].class_id = class_id;
            rpol_table[i].min_severity = min_severity;
            rpol_table[i].action = action;
            rpol_table[i].fires = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int rpol_evaluate(uint32_t class_id, uint32_t observed_severity, uint32_t *out_action) {
    if (rpol_ready != 1) return -1;
    if (class_id == 0) return -1;

    for (uint32_t i = 0; i < RPOL_CAPACITY; ++i) {
        rpol_rec_t *rec = &rpol_table[i];
        if (rec->class_id != class_id) continue;
        if (observed_severity < rec->min_severity) continue;

        ++rec->fires;
        *out_action = rec->action;
        return 0;
    }
    return -1;
}

int rpol_self_test(void) {
    uint32_t slot;
    if (rpol_register(1, FCLASS_HARD, RPOL_DEGRADE, &slot) != 0) return -1;

    uint32_t action;
    if (rpol_evaluate(1, FCLASS_HARD, &action) != 0) return -1;
    if (action != RPOL_DEGRADE) return -1;

    if (rpol_evaluate(1, FCLASS_SOFT, &action) == 0) return -1; /* unter Threshold */

    return 0;
}
