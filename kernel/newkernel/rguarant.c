/* NovaOS C-Kernel - Nova Resource Guarantee (Port aus adaptive32.inc). */
#include "rguarant.h"

typedef struct {
    uint32_t owner_id; /* 0 = frei */
    uint32_t min_cpu;
    uint32_t min_mem;
    uint32_t active;
} rguarant_rec_t;

static rguarant_rec_t rguarant_table[RGUARANT_CAPACITY];
static int rguarant_ready = 0;

void rguarant_initialize(void) {
    if (rguarant_ready == 1) return;

    for (uint32_t i = 0; i < RGUARANT_CAPACITY; ++i) rguarant_table[i] = (rguarant_rec_t){0};
    rguarant_ready = 1;
}

int rguarant_set(uint32_t owner_id, uint32_t min_cpu, uint32_t min_mem, uint32_t *out_slot) {
    if (rguarant_ready != 1 || owner_id == 0) return -1;

    for (uint32_t i = 0; i < RGUARANT_CAPACITY; ++i) {
        if (rguarant_table[i].owner_id == 0 || rguarant_table[i].owner_id == owner_id) {
            rguarant_table[i].owner_id = owner_id;
            rguarant_table[i].min_cpu = min_cpu;
            rguarant_table[i].min_mem = min_mem;
            rguarant_table[i].active = 1;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int rguarant_check(uint32_t owner_id, uint32_t *out_min_cpu, uint32_t *out_min_mem) {
    if (rguarant_ready != 1) return -1;

    for (uint32_t i = 0; i < RGUARANT_CAPACITY; ++i) {
        rguarant_rec_t *rec = &rguarant_table[i];
        if (rec->owner_id != owner_id || rec->active != 1) continue;

        *out_min_cpu = rec->min_cpu;
        *out_min_mem = rec->min_mem;
        return 0;
    }
    return -1;
}

int rguarant_self_test(void) {
    uint32_t slot;
    if (rguarant_set(1, 4, 8, &slot) != 0) return -1;

    uint32_t min_cpu, min_mem;
    if (rguarant_check(1, &min_cpu, &min_mem) != 0) return -1;
    if (min_cpu != 4 || min_mem != 8) return -1;

    if (rguarant_check(99, &min_cpu, &min_mem) == 0) return -1;

    return 0;
}
