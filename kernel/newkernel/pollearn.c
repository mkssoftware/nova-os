/* NovaOS C-Kernel - Nova Policy Learning Engine (Port aus adaptive32.inc). */
#include "pollearn.h"

typedef struct {
    uint32_t rule_id; /* 0 = frei */
    uint32_t trigger;
    uint32_t weight;
    uint32_t fires;
} pollearn_rec_t;

static pollearn_rec_t pollearn_table[POLLEARN_CAPACITY];
static int pollearn_ready = 0;

void pollearn_initialize(void) {
    if (pollearn_ready == 1) return;

    for (uint32_t i = 0; i < POLLEARN_CAPACITY; ++i) pollearn_table[i] = (pollearn_rec_t){0};
    pollearn_ready = 1;
}

int pollearn_register(uint32_t rule_id, uint32_t trigger) {
    if (pollearn_ready != 1 || rule_id == 0) return -1;

    for (uint32_t i = 0; i < POLLEARN_CAPACITY; ++i) {
        if (pollearn_table[i].rule_id == 0) {
            pollearn_table[i].rule_id = rule_id;
            pollearn_table[i].trigger = trigger;
            pollearn_table[i].weight = 128;
            pollearn_table[i].fires = 0;
            return 0;
        }
    }
    return -1;
}

int pollearn_feedback(uint32_t rule_id, uint32_t success) {
    if (pollearn_ready != 1) return -1;

    for (uint32_t i = 0; i < POLLEARN_CAPACITY; ++i) {
        pollearn_rec_t *rec = &pollearn_table[i];
        if (rec->rule_id != rule_id) continue;

        ++rec->fires;
        if (success) {
            rec->weight = (POLLEARN_ALPHA * 255u + (256u - POLLEARN_ALPHA) * rec->weight) >> 8;
        } else {
            rec->weight = ((256u - POLLEARN_ALPHA) * rec->weight) >> 8;
        }
        return 0;
    }
    return -1;
}

int pollearn_query(uint32_t rule_id, uint32_t *out_weight, uint32_t *out_fires) {
    if (pollearn_ready != 1) return -1;

    for (uint32_t i = 0; i < POLLEARN_CAPACITY; ++i) {
        if (pollearn_table[i].rule_id == rule_id) {
            *out_weight = pollearn_table[i].weight;
            *out_fires = pollearn_table[i].fires;
            return 0;
        }
    }
    return -1;
}

int pollearn_self_test(void) {
    if (pollearn_register(1, 1) != 0) return -1;

    if (pollearn_feedback(1, 1) != 0) return -1;
    uint32_t weight, fires;
    if (pollearn_query(1, &weight, &fires) != 0) return -1;
    if (weight <= 128) return -1;

    uint32_t weight_after_success = weight;
    if (pollearn_feedback(1, 0) != 0) return -1;
    if (pollearn_query(1, &weight, &fires) != 0) return -1;
    if (weight >= weight_after_success) return -1;

    return 0;
}
