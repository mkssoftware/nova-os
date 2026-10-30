/* NovaOS C-Kernel - Nova Execution Contract (Port aus adaptive32.inc). */
#include "excon.h"
#include "scheduler.h"
#include "resilience.h"

typedef struct {
    uint32_t id; /* 0 = frei */
    uint32_t type;
    uint32_t state;
    uint32_t cpu_budget;
    uint32_t deadline;
    uint32_t priority;
} excon_rec_t;

static excon_rec_t excon_table[EXCON_CAPACITY];
static int excon_ready = 0;
static uint32_t excon_next_id = 1;
static uint32_t excon_violations = 0;

void excon_initialize(void) {
    for (uint32_t i = 0; i < EXCON_CAPACITY; ++i) excon_table[i] = (excon_rec_t){0};
    excon_next_id = 1;
    excon_violations = 0;
    excon_ready = 1;
}

int excon_create(uint32_t type, uint32_t cpu_budget, uint32_t priority, uint32_t *out_slot) {
    if (excon_ready != 1) return -1;
    if (type == 0 || type > EXCON_TYPE_HARD_RT) return -1;

    for (uint32_t i = 0; i < EXCON_CAPACITY; ++i) {
        if (excon_table[i].id == 0) {
            excon_table[i].id = excon_next_id++;
            excon_table[i].type = type;
            excon_table[i].state = EXCON_STATE_CREATED;
            excon_table[i].cpu_budget = cpu_budget;
            excon_table[i].deadline = resilience_global_tick() + 1000u;
            excon_table[i].priority = priority;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int excon_activate(uint32_t slot) {
    if (slot >= EXCON_CAPACITY) return -1;

    excon_rec_t *rec = &excon_table[slot];
    if (rec->id == 0) return -1;

    rec->state = EXCON_STATE_ACTIVE;

    uint32_t sched_slot;
    return scheduler_admit_task(rec->id, rec->priority, QOS_CLASS_NORMAL, &sched_slot);
}

int excon_verify(uint32_t slot, uint32_t actual_cpu_used) {
    if (slot >= EXCON_CAPACITY) return -1;

    excon_rec_t *rec = &excon_table[slot];
    if (rec->id == 0) return -1;

    if (actual_cpu_used <= rec->cpu_budget) {
        rec->state = EXCON_STATE_DONE;
        return 0;
    }

    rec->state = EXCON_STATE_VIOL;
    ++excon_violations;
    return -1;
}

int excon_self_test(void) {
    uint32_t slot;
    if (excon_create(EXCON_TYPE_BOUNDED, 50, 100, &slot) != 0) return -1;

    if (excon_verify(slot, 30) != 0) return -1; /* 30<=50: eingehalten */

    uint32_t slot2;
    if (excon_create(EXCON_TYPE_HARD_RT, 10, 200, &slot2) != 0) return -1;

    if (excon_verify(slot2, 20) == 0) return -1; /* 20>10: Verletzung */

    return 0;
}
