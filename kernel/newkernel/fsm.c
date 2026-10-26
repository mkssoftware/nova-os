/* NovaOS C-Kernel - Nova Generic Finite State Machine (Port aus
   adaptive32.inc). */
#include "fsm.h"

typedef struct {
    uint32_t from;
    uint32_t to;
    uint32_t used;
} fsm_trans_t;

typedef struct {
    uint32_t id; /* 0 = frei */
    uint32_t current_state;
    fsm_trans_t trans[FSM_TRANS_COUNT];
} fsm_rec_t;

static fsm_rec_t fsm_table[FSM_CAPACITY];
static int fsm_ready = 0;
static uint32_t fsm_denied = 0;

void fsm_initialize(void) {
    for (uint32_t i = 0; i < FSM_CAPACITY; ++i) fsm_table[i] = (fsm_rec_t){0};
    fsm_denied = 0;
    fsm_ready = 1;
}

int fsm_register(uint32_t fsm_id, uint32_t initial_state) {
    for (uint32_t i = 0; i < FSM_CAPACITY; ++i) {
        if (fsm_table[i].id == 0) {
            fsm_table[i].id = fsm_id;
            fsm_table[i].current_state = initial_state;
            for (uint32_t t = 0; t < FSM_TRANS_COUNT; ++t) fsm_table[i].trans[t] = (fsm_trans_t){0};
            return 0;
        }
    }
    return -1;
}

static fsm_rec_t *fsm_find(uint32_t fsm_id) {
    for (uint32_t i = 0; i < FSM_CAPACITY; ++i) {
        if (fsm_table[i].id == fsm_id) return &fsm_table[i];
    }
    return 0;
}

int fsm_add_transition(uint32_t fsm_id, uint32_t from_state, uint32_t to_state) {
    fsm_rec_t *rec = fsm_find(fsm_id);
    if (!rec) return -1;

    for (uint32_t t = 0; t < FSM_TRANS_COUNT; ++t) {
        if (rec->trans[t].used == 0) {
            rec->trans[t].from = from_state;
            rec->trans[t].to = to_state;
            rec->trans[t].used = 1;
            return 0;
        }
    }
    return -1;
}

int fsm_transition2(uint32_t fsm_id, uint32_t to_state) {
    fsm_rec_t *rec = fsm_find(fsm_id);
    if (!rec) return -1;

    for (uint32_t t = 0; t < FSM_TRANS_COUNT; ++t) {
        if (rec->trans[t].used != 0 && rec->trans[t].from == rec->current_state &&
            rec->trans[t].to == to_state) {
            rec->current_state = to_state;
            return 0;
        }
    }
    ++fsm_denied;
    return -1;
}

int fsm_self_test(void) {
    if (fsm_register(1, 0) != 0) return -1;
    if (fsm_add_transition(1, 0, 1) != 0) return -1;
    if (fsm_add_transition(1, 1, 2) != 0) return -1;

    if (fsm_transition2(1, 1) != 0) return -1;
    if (fsm_transition2(1, 3) == 0) return -1; /* nicht in Tabelle: verweigert */
    if (fsm_transition2(1, 2) != 0) return -1;

    return 0;
}
