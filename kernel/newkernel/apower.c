/* NovaOS C-Kernel - Nova Adaptive Power Manager (Port aus adaptive32.inc). */
#include "apower.h"
#include "apred.h"

typedef struct {
    uint32_t domain_id; /* 0 = frei */
    uint32_t pstate;
    uint32_t load;
    uint32_t transitions;
} apower_rec_t;

static apower_rec_t apower_table[APOWER_CAPACITY];
static int apower_ready = 0;

void apower_initialize(void) {
    if (apower_ready == 1) return;

    for (uint32_t i = 0; i < APOWER_CAPACITY; ++i) apower_table[i] = (apower_rec_t){0};
    apower_ready = 1;
}

int apower_register(uint32_t domain_id) {
    if (apower_ready != 1 || domain_id == 0) return -1;

    for (uint32_t i = 0; i < APOWER_CAPACITY; ++i) {
        if (apower_table[i].domain_id == 0) {
            apower_table[i].domain_id = domain_id;
            apower_table[i].pstate = 1;
            apower_table[i].load = 50;
            apower_table[i].transitions = 0;
            return 0;
        }
    }
    return -1;
}

int apower_update_load(uint32_t domain_id, uint32_t load) {
    if (apower_ready != 1) return -1;

    for (uint32_t i = 0; i < APOWER_CAPACITY; ++i) {
        apower_rec_t *rec = &apower_table[i];
        if (rec->domain_id != domain_id) continue;

        rec->load = load;

        uint32_t new_prediction;
        apred_update(domain_id, load, &new_prediction); /* Rueckgabe bewusst ignoriert */

        uint32_t new_pstate;
        if (load >= 75u) {
            new_pstate = APOWER_PSTATE_MAX;
        } else if (load <= 25u) {
            new_pstate = APOWER_PSTATE_MIN;
        } else {
            new_pstate = 1u;
        }

        if (new_pstate != rec->pstate) {
            ++rec->transitions;
            rec->pstate = new_pstate;
        }
        return 0;
    }
    return -1;
}

int apower_query(uint32_t domain_id, uint32_t *out_pstate, uint32_t *out_load,
                 uint32_t *out_transitions) {
    if (apower_ready != 1) return -1;

    for (uint32_t i = 0; i < APOWER_CAPACITY; ++i) {
        if (apower_table[i].domain_id == domain_id) {
            *out_pstate = apower_table[i].pstate;
            *out_load = apower_table[i].load;
            *out_transitions = apower_table[i].transitions;
            return 0;
        }
    }
    return -1;
}

int apower_self_test(void) {
    if (apower_register(1) != 0) return -1;

    if (apower_update_load(1, 80) != 0) return -1;
    uint32_t pstate, load, transitions;
    if (apower_query(1, &pstate, &load, &transitions) != 0) return -1;
    if (pstate != APOWER_PSTATE_MAX) return -1;

    if (apower_update_load(1, 10) != 0) return -1;
    if (apower_query(1, &pstate, &load, &transitions) != 0) return -1;
    if (pstate != APOWER_PSTATE_MIN) return -1;

    return 0;
}
