/* NovaOS C-Kernel - Nova State Actual Store (Port aus adaptive32.inc). */
#include "sactual.h"

typedef struct {
    uint32_t obj_id; /* 0 = frei */
    uint32_t value;
    uint32_t tick;
    uint32_t writes;
} sactual_rec_t;

static sactual_rec_t sactual_table[SACTUAL_CAPACITY];
static int sactual_ready = 0;

static uint32_t wd_global_tick_stub(void) {
    return 0;
}

void sactual_initialize(void) {
    if (sactual_ready == 1) return;

    for (uint32_t i = 0; i < SACTUAL_CAPACITY; ++i) sactual_table[i] = (sactual_rec_t){0};
    sactual_ready = 1;
}

int sactual_write(uint32_t obj_id, uint32_t actual_value) {
    if (sactual_ready != 1 || obj_id == 0) return -1;

    for (uint32_t i = 0; i < SACTUAL_CAPACITY; ++i) {
        if (sactual_table[i].obj_id == 0 || sactual_table[i].obj_id == obj_id) {
            sactual_table[i].obj_id = obj_id;
            sactual_table[i].value = actual_value;
            sactual_table[i].tick = wd_global_tick_stub();
            ++sactual_table[i].writes;
            return 0;
        }
    }
    return -1;
}

int sactual_read(uint32_t obj_id, uint32_t *out_actual_value, uint32_t *out_tick) {
    if (sactual_ready != 1) return -1;

    for (uint32_t i = 0; i < SACTUAL_CAPACITY; ++i) {
        if (sactual_table[i].obj_id == obj_id) {
            *out_actual_value = sactual_table[i].value;
            *out_tick = sactual_table[i].tick;
            return 0;
        }
    }
    return -1;
}

int sactual_self_test(void) {
    if (sactual_write(1, 42) != 0) return -1;

    uint32_t value, tick;
    if (sactual_read(1, &value, &tick) != 0) return -1;
    if (value != 42) return -1;

    if (sactual_write(1, 99) != 0) return -1;
    if (sactual_read(1, &value, &tick) != 0) return -1;
    if (value != 99) return -1;

    return 0;
}
