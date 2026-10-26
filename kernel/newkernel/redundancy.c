/* NovaOS C-Kernel - Nova Redundancy Manager (Port aus adaptive32.inc). */
#include "redundancy.h"

typedef struct {
    uint32_t group_id; /* 0 = frei */
    uint32_t active_id;
    uint32_t standby_id;
    uint32_t promotions;
} redun_rec_t;

static redun_rec_t redun_table[REDUN_CAPACITY];
static int redun_ready = 0;

void redundancy_initialize(void) {
    if (redun_ready == 1) return;

    for (uint32_t i = 0; i < REDUN_CAPACITY; ++i) redun_table[i] = (redun_rec_t){0};
    redun_ready = 1;
}

int redundancy_register(uint32_t group_id, uint32_t active_id, uint32_t standby_id, uint32_t *out_slot) {
    if (redun_ready != 1 || group_id == 0) return -1;

    for (uint32_t i = 0; i < REDUN_CAPACITY; ++i) {
        if (redun_table[i].group_id == 0) {
            redun_table[i].group_id = group_id;
            redun_table[i].active_id = active_id;
            redun_table[i].standby_id = standby_id;
            redun_table[i].promotions = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int redundancy_promote(uint32_t group_id, uint32_t *out_new_active) {
    if (redun_ready != 1 || group_id == 0) return -1;

    for (uint32_t i = 0; i < REDUN_CAPACITY; ++i) {
        redun_rec_t *rec = &redun_table[i];
        if (rec->group_id != group_id) continue;
        if (rec->standby_id == 0) return -1;

        uint32_t old_active = rec->active_id;
        rec->active_id = rec->standby_id;
        rec->standby_id = old_active;
        ++rec->promotions;
        *out_new_active = rec->active_id;
        return 0;
    }
    return -1;
}

int redundancy_self_test(void) {
    uint32_t slot;
    if (redundancy_register(1, 10, 20, &slot) != 0) return -1;

    uint32_t new_active;
    if (redundancy_promote(1, &new_active) != 0) return -1;
    if (new_active != 20) return -1;

    if (redun_table[slot].active_id != 20) return -1;
    if (redun_table[slot].standby_id != 10) return -1;
    if (redun_table[slot].promotions != 1) return -1;

    return 0;
}
