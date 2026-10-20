/* NovaOS C-Kernel - Out-of-Memory-Handler (Port aus memory32.inc). */
#include "oom.h"

#define OOM_STATE_FREE   0u
#define OOM_STATE_ACTIVE 1u
#define OOM_STATE_KILLED 2u

typedef struct {
    uint32_t id;
    uint32_t state;
    uint32_t pid;
    uint32_t score; /* hoeherer Score = wird zuerst terminiert */
} oom_record_t;

static oom_record_t oom_table[OOM_CAND_COUNT];
static uint32_t oom_ready = 0;
static uint32_t oom_kills = 0;
static uint32_t oom_events = 0;

void oom_initialize(void) {
    for (uint32_t i = 0; i < OOM_CAND_COUNT; ++i) {
        oom_table[i] = (oom_record_t){0};
    }
    oom_ready = 1;
    oom_kills = 0;
    oom_events = 0;
}

int oom_register_candidate(uint32_t pid, uint32_t score) {
    for (uint32_t slot = 0; slot < OOM_CAND_COUNT; ++slot) {
        oom_record_t *rec = &oom_table[slot];
        if (rec->state == OOM_STATE_FREE || rec->state == OOM_STATE_KILLED) {
            rec->id = slot;
            rec->state = OOM_STATE_ACTIVE;
            rec->pid = pid;
            rec->score = score;
            return (int)slot;
        }
    }
    return -1;
}

int32_t oom_kill_victim(void) {
    uint32_t best_score = 0;
    int32_t best_slot = -1;

    for (uint32_t slot = 0; slot < OOM_CAND_COUNT; ++slot) {
        oom_record_t *rec = &oom_table[slot];
        if (rec->state != OOM_STATE_ACTIVE) {
            continue;
        }
        /* Streng "groesser als", identisch zum alten ASM (jbe .next):
           ein Kandidat mit Score 0 wird nie ausgewaehlt. */
        if (rec->score > best_score) {
            best_score = rec->score;
            best_slot = (int32_t)slot;
        }
    }

    if (best_slot == -1) {
        ++oom_events;
        return -1;
    }

    oom_record_t *victim = &oom_table[best_slot];
    victim->state = OOM_STATE_KILLED;
    uint32_t pid = victim->pid;
    ++oom_kills;
    ++oom_events;
    return (int32_t)pid;
}

int oom_self_test(void) {
    if (oom_ready != 1) return -1;

    if (oom_register_candidate(1, 50) < 0) return -1;
    if (oom_register_candidate(2, 80) < 0) return -1;

    int32_t victim_pid = oom_kill_victim();
    if (victim_pid != 2) return -1;
    if (oom_kills != 1) return -1;

    return 0;
}
