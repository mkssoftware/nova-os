/* NovaOS C-Kernel - Nova Adaptive Scheduler (Port aus adaptive32.inc). */
#include "aschedsw.h"

typedef struct {
    uint32_t class_id; /* 0 = frei */
    uint32_t weight;
    uint32_t min_weight;
    uint32_t max_weight;
    uint32_t adjustments;
} aschedsw_rec_t;

static aschedsw_rec_t aschedsw_table[ASCHEDSW_CAPACITY];
static int aschedsw_ready = 0;

void aschedsw_initialize(void) {
    if (aschedsw_ready == 1) return;

    for (uint32_t i = 0; i < ASCHEDSW_CAPACITY; ++i) aschedsw_table[i] = (aschedsw_rec_t){0};
    aschedsw_ready = 1;
}

int aschedsw_register(uint32_t class_id, uint32_t initial_weight, uint32_t min_weight,
                       uint32_t max_weight, uint32_t *out_slot) {
    if (aschedsw_ready != 1 || class_id == 0) return -1;

    for (uint32_t i = 0; i < ASCHEDSW_CAPACITY; ++i) {
        if (aschedsw_table[i].class_id == 0) {
            aschedsw_table[i].class_id = class_id;
            aschedsw_table[i].weight = initial_weight;
            aschedsw_table[i].min_weight = min_weight;
            aschedsw_table[i].max_weight = max_weight;
            aschedsw_table[i].adjustments = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int aschedsw_feedback(uint32_t class_id, int32_t delta) {
    if (aschedsw_ready != 1) return -1;

    for (uint32_t i = 0; i < ASCHEDSW_CAPACITY; ++i) {
        aschedsw_rec_t *rec = &aschedsw_table[i];
        if (rec->class_id != class_id) continue;

        if (delta >= 0) {
            uint32_t sum = rec->weight + (uint32_t)delta;
            rec->weight = (sum > rec->max_weight) ? rec->max_weight : sum;
        } else {
            uint32_t drop = (uint32_t)(-delta);
            rec->weight = (drop > rec->weight || rec->weight - drop < rec->min_weight)
                              ? rec->min_weight
                              : rec->weight - drop;
        }
        ++rec->adjustments;
        return 0;
    }
    return -1;
}

int aschedsw_query(uint32_t class_id, uint32_t *out_weight, uint32_t *out_adjustments) {
    if (aschedsw_ready != 1) return -1;

    for (uint32_t i = 0; i < ASCHEDSW_CAPACITY; ++i) {
        if (aschedsw_table[i].class_id == class_id) {
            *out_weight = aschedsw_table[i].weight;
            *out_adjustments = aschedsw_table[i].adjustments;
            return 0;
        }
    }
    return -1;
}

int aschedsw_self_test(void) {
    uint32_t slot;
    if (aschedsw_register(1, 100, 10, 200, &slot) != 0) return -1;

    uint32_t weight, adjustments;
    if (aschedsw_feedback(1, 30) != 0) return -1;
    if (aschedsw_query(1, &weight, &adjustments) != 0) return -1;
    if (weight != 130) return -1;

    if (aschedsw_feedback(1, -50) != 0) return -1;
    if (aschedsw_query(1, &weight, &adjustments) != 0) return -1;
    if (weight != 80) return -1;

    if (aschedsw_feedback(1, -200) != 0) return -1; /* geklemmt auf min=10 */
    if (aschedsw_query(1, &weight, &adjustments) != 0) return -1;
    if (weight != 10) return -1;

    return 0;
}
