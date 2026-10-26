/* NovaOS C-Kernel - Nova Chaos Engine (Port aus adaptive32.inc). */
#include "chaos.h"
#include "fi.h"

typedef struct {
    uint32_t scenario_id; /* 0 = frei */
    uint32_t target;
    uint32_t fault_type;
    uint32_t runs;
    uint32_t recovered;
} chaos_rec_t;

static chaos_rec_t chaos_table[CHAOS_CAPACITY];
static int chaos_ready = 0;

void chaos_initialize(void) {
    if (chaos_ready == 1) return;

    for (uint32_t i = 0; i < CHAOS_CAPACITY; ++i) chaos_table[i] = (chaos_rec_t){0};
    chaos_ready = 1;
}

int chaos_register_scenario(uint32_t scenario_id, uint32_t target_subsystem,
                             uint32_t fault_type, uint32_t *out_slot) {
    if (chaos_ready != 1 || scenario_id == 0) return -1;

    for (uint32_t i = 0; i < CHAOS_CAPACITY; ++i) {
        if (chaos_table[i].scenario_id == 0) {
            chaos_table[i].scenario_id = scenario_id;
            chaos_table[i].target = target_subsystem;
            chaos_table[i].fault_type = fault_type;
            chaos_table[i].runs = 0;
            chaos_table[i].recovered = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

static chaos_rec_t *chaos_find(uint32_t scenario_id) {
    for (uint32_t i = 0; i < CHAOS_CAPACITY; ++i) {
        if (chaos_table[i].scenario_id == scenario_id) return &chaos_table[i];
    }
    return 0;
}

int chaos_run_scenario(uint32_t scenario_id) {
    if (chaos_ready != 1 || scenario_id == 0) return -1;

    chaos_rec_t *rec = chaos_find(scenario_id);
    if (!rec) return -1;

    ++rec->runs;
    fi_inject(rec->target, rec->fault_type);
    return 0;
}

int chaos_mark_recovered(uint32_t scenario_id) {
    chaos_rec_t *rec = chaos_find(scenario_id);
    if (!rec) return -1;

    ++rec->recovered;
    return 0;
}

int chaos_self_test(void) {
    uint32_t slot;
    if (chaos_register_scenario(1, 2, 1, &slot) != 0) return -1;

    if (chaos_run_scenario(1) != 0) return -1;
    if (chaos_mark_recovered(1) != 0) return -1;

    if (chaos_table[slot].runs != 1) return -1;
    if (chaos_table[slot].recovered != 1) return -1;

    fi_clear(2);
    return 0;
}
