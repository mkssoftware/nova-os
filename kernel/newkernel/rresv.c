/* NovaOS C-Kernel - Nova Resource Reservation (Port aus adaptive32.inc). */
#include "rresv.h"

typedef struct {
    uint32_t id; /* 0 = nie benutzt */
    uint32_t owner;
    uint32_t cpu;
    uint32_t mem;
    uint32_t state;
} rresv_rec_t;

static rresv_rec_t rresv_table[RRESV_CAPACITY];
static int rresv_ready = 0;
static uint32_t rresv_next_id = 1;

void rresv_initialize(void) {
    if (rresv_ready == 1) return;

    for (uint32_t i = 0; i < RRESV_CAPACITY; ++i) rresv_table[i] = (rresv_rec_t){0};
    rresv_next_id = 1;
    rresv_ready = 1;
}

int rresv_reserve(uint32_t owner_id, uint32_t cpu_hold, uint32_t mem_hold, uint32_t *out_resv_id) {
    if (rresv_ready != 1 || owner_id == 0) return -1;

    for (uint32_t i = 0; i < RRESV_CAPACITY; ++i) {
        rresv_rec_t *rec = &rresv_table[i];
        if (rec->state == RRESV_RELEASED || rec->id == 0) {
            rec->id = rresv_next_id++;
            rec->owner = owner_id;
            rec->cpu = cpu_hold;
            rec->mem = mem_hold;
            rec->state = RRESV_PENDING;
            *out_resv_id = rec->id;
            return 0;
        }
    }
    return -1;
}

int rresv_activate(uint32_t resv_id) {
    for (uint32_t i = 0; i < RRESV_CAPACITY; ++i) {
        if (rresv_table[i].id == resv_id) {
            if (rresv_table[i].state != RRESV_PENDING) return -1;
            rresv_table[i].state = RRESV_ACTIVE;
            return 0;
        }
    }
    return -1;
}

int rresv_release(uint32_t resv_id) {
    for (uint32_t i = 0; i < RRESV_CAPACITY; ++i) {
        if (rresv_table[i].id == resv_id) {
            rresv_table[i].state = RRESV_RELEASED;
            return 0;
        }
    }
    return -1;
}

int rresv_self_test(void) {
    uint32_t resv_id;
    if (rresv_reserve(1, 8, 16, &resv_id) != 0) return -1;
    if (rresv_activate(resv_id) != 0) return -1;
    if (rresv_release(resv_id) != 0) return -1;

    if (rresv_table[0].state != RRESV_RELEASED) return -1;

    return 0;
}
