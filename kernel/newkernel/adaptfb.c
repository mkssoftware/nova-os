/* NovaOS C-Kernel - Nova Adaptive Feedback Loop (Port aus adaptive32.inc). */
#include "adaptfb.h"

typedef struct {
    uint32_t id; /* 1-basiert: 0 = frei */
    uint32_t dimension;
    uint32_t expected;
    uint32_t actual;
    uint32_t misses;
} adapt_slot_t;

static adapt_slot_t adapt_slots[ADAPT_SLOT_COUNT];
static int adapt_ready = 0;
static uint32_t adapt_adjustments = 0;

void adapt_initialize(void) {
    for (uint32_t i = 0; i < ADAPT_SLOT_COUNT; ++i) adapt_slots[i] = (adapt_slot_t){0};
    adapt_adjustments = 0;
    adapt_ready = 1;
}

int adapt_register(uint32_t dimension, uint32_t expected_value, uint32_t *out_slot) {
    for (uint32_t i = 0; i < ADAPT_SLOT_COUNT; ++i) {
        if (adapt_slots[i].id == 0) {
            adapt_slots[i].dimension = dimension;
            adapt_slots[i].expected = expected_value;
            adapt_slots[i].actual = 0;
            adapt_slots[i].misses = 0;
            adapt_slots[i].id = i + 1;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int adapt_observe(uint32_t slot, uint32_t actual_value, uint32_t miss_threshold) {
    if (slot >= ADAPT_SLOT_COUNT) return -1;

    adapt_slot_t *s = &adapt_slots[slot];
    s->actual = actual_value;
    if (actual_value > s->expected) {
        ++s->misses;
        ++adapt_adjustments;
    }

    return (s->misses >= miss_threshold) ? -1 : 0;
}

int adapt_self_test(void) {
    uint32_t slot;
    if (adapt_register(ADAPT_DIM_THERMAL, 80, &slot) != 0) return -1;

    if (adapt_observe(slot, 70, 3) != 0) return -1; /* 70<=80: innerhalb Budget */

    adapt_observe(slot, 90, 3);
    adapt_observe(slot, 90, 3);
    if (adapt_observe(slot, 90, 3) == 0) return -1; /* 3. Miss erreicht Threshold */

    return 0;
}
