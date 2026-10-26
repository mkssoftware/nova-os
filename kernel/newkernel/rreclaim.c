/* NovaOS C-Kernel - Nova Resource Reclaim (Port aus adaptive32.inc). */
#include "rreclaim.h"

typedef struct {
    uint32_t owner_id; /* 0 = frei */
    uint32_t cpu_freed;
    uint32_t mem_freed;
    uint32_t reclaimed;
} rreclaim_rec_t;

static rreclaim_rec_t rreclaim_table[RRECLAIM_CAPACITY];
static int rreclaim_ready = 0;

void rreclaim_initialize(void) {
    if (rreclaim_ready == 1) return;

    for (uint32_t i = 0; i < RRECLAIM_CAPACITY; ++i) rreclaim_table[i] = (rreclaim_rec_t){0};
    rreclaim_ready = 1;
}

int rreclaim_post(uint32_t owner_id, uint32_t cpu_units, uint32_t mem_units) {
    if (rreclaim_ready != 1 || owner_id == 0) return -1;

    for (uint32_t i = 0; i < RRECLAIM_CAPACITY; ++i) {
        if (rreclaim_table[i].owner_id == 0 || rreclaim_table[i].owner_id == owner_id) {
            rreclaim_table[i].owner_id = owner_id;
            rreclaim_table[i].cpu_freed += cpu_units;
            rreclaim_table[i].mem_freed += mem_units;
            return 0;
        }
    }
    return -1;
}

int rreclaim_try(uint32_t owner_id, uint32_t need_cpu, uint32_t need_mem) {
    if (rreclaim_ready != 1) return -1;

    for (uint32_t i = 0; i < RRECLAIM_CAPACITY; ++i) {
        rreclaim_rec_t *rec = &rreclaim_table[i];
        if (rec->owner_id != owner_id) continue;

        if (rec->cpu_freed < need_cpu || rec->mem_freed < need_mem) return -1;

        rec->cpu_freed -= need_cpu;
        rec->mem_freed -= need_mem;
        ++rec->reclaimed;
        return 0;
    }
    return -1;
}

int rreclaim_self_test(void) {
    if (rreclaim_post(1, 16, 32) != 0) return -1;

    if (rreclaim_try(1, 10, 20) != 0) return -1;
    if (rreclaim_try(1, 10, 20) == 0) return -1; /* nur noch 6/12 uebrig */

    return 0;
}
