/* NovaOS C-Kernel - Nova System State Graph (Port aus adaptive32.inc). */
#include "sgraph.h"

typedef struct {
    uint32_t obj_id; /* 0 = frei (0 ist keine gueltige Objekt-ID) */
    uint32_t state;
    uint32_t prev_state;
    uint32_t parent_obj_id;
    uint32_t tick;
} sgraph_rec_t;

static sgraph_rec_t sgraph_nodes[SGRAPH_CAPACITY];
static int sgraph_ready = 0;
static uint32_t sgraph_transitions = 0;

static uint32_t wd_global_tick_stub(void) {
    return 0;
}

void sgraph_initialize(void) {
    for (uint32_t i = 0; i < SGRAPH_CAPACITY; ++i) sgraph_nodes[i] = (sgraph_rec_t){0};
    sgraph_transitions = 0;
    sgraph_ready = 1;
}

int sgraph_register(uint32_t obj_id, uint32_t initial_state, uint32_t parent_obj_id) {
    for (uint32_t i = 0; i < SGRAPH_CAPACITY; ++i) {
        if (sgraph_nodes[i].obj_id == 0) {
            sgraph_nodes[i].obj_id = obj_id;
            sgraph_nodes[i].state = initial_state;
            sgraph_nodes[i].prev_state = 0;
            sgraph_nodes[i].parent_obj_id = parent_obj_id;
            sgraph_nodes[i].tick = wd_global_tick_stub();
            return 0;
        }
    }
    return -1;
}

int sgraph_transition(uint32_t obj_id, uint32_t new_state) {
    for (uint32_t i = 0; i < SGRAPH_CAPACITY; ++i) {
        if (sgraph_nodes[i].obj_id == obj_id) {
            sgraph_nodes[i].prev_state = sgraph_nodes[i].state;
            sgraph_nodes[i].state = new_state;
            sgraph_nodes[i].tick = wd_global_tick_stub();
            ++sgraph_transitions;
            return 0;
        }
    }
    return -1;
}

int sgraph_query(uint32_t obj_id, uint32_t *out_state, uint32_t *out_prev_state) {
    for (uint32_t i = 0; i < SGRAPH_CAPACITY; ++i) {
        if (sgraph_nodes[i].obj_id == obj_id) {
            *out_state = sgraph_nodes[i].state;
            *out_prev_state = sgraph_nodes[i].prev_state;
            return 0;
        }
    }
    return -1;
}

int sgraph_self_test(void) {
    (void)sgraph_ready;

    if (sgraph_register(50, 1, 0) != 0) return -1;
    if (sgraph_transition(50, 2) != 0) return -1;

    uint32_t state, prev;
    if (sgraph_query(50, &state, &prev) != 0) return -1;
    if (state != 2 || prev != 1) return -1;

    return 0;
}
