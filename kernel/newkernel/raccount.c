/* NovaOS C-Kernel - Nova Resource Accounting (Port aus adaptive32.inc). */
#include "raccount.h"

typedef struct {
    uint32_t owner_id; /* 0 = frei */
    uint32_t cpu_used;
    uint32_t mem_used;
} raccount_rec_t;

static raccount_rec_t raccount_table[RACCOUNT_CAPACITY];
static int raccount_ready = 0;

void raccount_initialize(void) {
    if (raccount_ready == 1) return;

    for (uint32_t i = 0; i < RACCOUNT_CAPACITY; ++i) raccount_table[i] = (raccount_rec_t){0};
    raccount_ready = 1;
}

static raccount_rec_t *raccount_get_or_create(uint32_t owner_id) {
    for (uint32_t i = 0; i < RACCOUNT_CAPACITY; ++i) {
        if (raccount_table[i].owner_id == owner_id) return &raccount_table[i];
    }
    for (uint32_t i = 0; i < RACCOUNT_CAPACITY; ++i) {
        if (raccount_table[i].owner_id == 0) {
            raccount_table[i].owner_id = owner_id;
            raccount_table[i].cpu_used = 0;
            raccount_table[i].mem_used = 0;
            return &raccount_table[i];
        }
    }
    return 0;
}

int raccount_charge(uint32_t owner_id, uint32_t cpu_delta, uint32_t mem_delta) {
    if (raccount_ready != 1 || owner_id == 0) return -1;

    raccount_rec_t *rec = raccount_get_or_create(owner_id);
    if (!rec) return -1;

    rec->cpu_used += cpu_delta;
    rec->mem_used += mem_delta;
    return 0;
}

int raccount_query(uint32_t owner_id, uint32_t *out_cpu_used, uint32_t *out_mem_used) {
    if (raccount_ready != 1) return -1;

    for (uint32_t i = 0; i < RACCOUNT_CAPACITY; ++i) {
        if (raccount_table[i].owner_id == owner_id) {
            *out_cpu_used = raccount_table[i].cpu_used;
            *out_mem_used = raccount_table[i].mem_used;
            return 0;
        }
    }
    return -1;
}

int raccount_self_test(void) {
    if (raccount_charge(1, 10, 20) != 0) return -1;
    if (raccount_charge(1, 5, 3) != 0) return -1;

    uint32_t cpu_used, mem_used;
    if (raccount_query(1, &cpu_used, &mem_used) != 0) return -1;
    if (cpu_used != 15 || mem_used != 23) return -1;

    return 0;
}
