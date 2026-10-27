/* NovaOS C-Kernel - Nova State Desired Store (Port aus adaptive32.inc). */
#include "sdesired.h"

typedef struct {
    uint32_t obj_id; /* 0 = frei */
    uint32_t value;
    uint32_t priority;
    uint32_t writes;
} sdesired_rec_t;

static sdesired_rec_t sdesired_table[SDESIRED_CAPACITY];
static int sdesired_ready = 0;

void sdesired_initialize(void) {
    if (sdesired_ready == 1) return;

    for (uint32_t i = 0; i < SDESIRED_CAPACITY; ++i) sdesired_table[i] = (sdesired_rec_t){0};
    sdesired_ready = 1;
}

int sdesired_write(uint32_t obj_id, uint32_t desired_value, uint32_t priority) {
    if (sdesired_ready != 1 || obj_id == 0) return -1;

    for (uint32_t i = 0; i < SDESIRED_CAPACITY; ++i) {
        if (sdesired_table[i].obj_id == 0 || sdesired_table[i].obj_id == obj_id) {
            sdesired_table[i].obj_id = obj_id;
            sdesired_table[i].value = desired_value;
            sdesired_table[i].priority = priority;
            ++sdesired_table[i].writes;
            return 0;
        }
    }
    return -1;
}

int sdesired_read(uint32_t obj_id, uint32_t *out_desired_value, uint32_t *out_priority) {
    if (sdesired_ready != 1) return -1;

    for (uint32_t i = 0; i < SDESIRED_CAPACITY; ++i) {
        if (sdesired_table[i].obj_id == obj_id) {
            *out_desired_value = sdesired_table[i].value;
            *out_priority = sdesired_table[i].priority;
            return 0;
        }
    }
    return -1;
}

int sdesired_self_test(void) {
    if (sdesired_write(1, 10, 1) != 0) return -1;

    uint32_t value, priority;
    if (sdesired_read(1, &value, &priority) != 0) return -1;
    if (value != 10 || priority != 1) return -1;

    if (sdesired_write(1, 10, 5) != 0) return -1;
    if (sdesired_read(1, &value, &priority) != 0) return -1;
    if (priority != 5) return -1;

    return 0;
}
