/* NovaOS C-Kernel - Nova Resource Deadline Manager (Port aus
   adaptive32.inc). */
#include "rdeadline.h"
#include "aschedsw.h"

typedef struct {
    uint32_t task_id;
    uint32_t deadline_tick;
    uint32_t state; /* 0 = frei */
    uint32_t missed;
} rdeadline_rec_t;

static rdeadline_rec_t rdeadline_table[RDEAD_CAPACITY];
static int rdeadline_ready = 0;

void rdeadline_initialize(void) {
    if (rdeadline_ready == 1) return;

    for (uint32_t i = 0; i < RDEAD_CAPACITY; ++i) rdeadline_table[i] = (rdeadline_rec_t){0};
    rdeadline_ready = 1;
}

int rdeadline_register(uint32_t task_id, uint32_t deadline_tick) {
    if (rdeadline_ready != 1 || task_id == 0) return -1;

    for (uint32_t i = 0; i < RDEAD_CAPACITY; ++i) {
        if (rdeadline_table[i].state == 0) {
            rdeadline_table[i].task_id = task_id;
            rdeadline_table[i].deadline_tick = deadline_tick;
            rdeadline_table[i].state = RDEAD_ACTIVE;
            rdeadline_table[i].missed = 0;
            return 0;
        }
    }
    return -1;
}

int rdeadline_done(uint32_t task_id) {
    if (rdeadline_ready != 1) return -1;

    for (uint32_t i = 0; i < RDEAD_CAPACITY; ++i) {
        if (rdeadline_table[i].task_id == task_id) {
            rdeadline_table[i].state = RDEAD_DONE;
            return 0;
        }
    }
    return -1;
}

void rdeadline_check_all(uint32_t current_tick) {
    if (rdeadline_ready != 1) return;

    for (uint32_t i = 0; i < RDEAD_CAPACITY; ++i) {
        rdeadline_rec_t *rec = &rdeadline_table[i];
        if (rec->state != RDEAD_ACTIVE) continue;
        if (rec->deadline_tick >= current_tick) continue;

        rec->state = RDEAD_MISSED;
        ++rec->missed;
        aschedsw_feedback(4, 32);
    }
}

int rdeadline_self_test(void) {
    if (rdeadline_register(1, 100) != 0) return -1;

    rdeadline_check_all(50);
    if (rdeadline_table[0].state != RDEAD_ACTIVE) return -1;

    rdeadline_check_all(200);
    if (rdeadline_table[0].state != RDEAD_MISSED) return -1;

    return 0;
}
