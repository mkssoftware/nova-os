/* NovaOS C-Kernel - Nova Resource Arbitration (Port aus adaptive32.inc). */
#include "rarbit.h"

typedef struct {
    uint32_t req_id; /* 0 = frei */
    uint32_t resource_id;
    uint32_t priority;
    uint32_t granted;
} rarbit_rec_t;

static rarbit_rec_t rarbit_table[RARBIT_CAPACITY];
static int rarbit_ready = 0;

void rarbit_initialize(void) {
    if (rarbit_ready == 1) return;

    for (uint32_t i = 0; i < RARBIT_CAPACITY; ++i) rarbit_table[i] = (rarbit_rec_t){0};
    rarbit_ready = 1;
}

int rarbit_request(uint32_t req_id, uint32_t resource_id, uint32_t priority, uint32_t *out_slot) {
    if (rarbit_ready != 1 || req_id == 0) return -1;

    for (uint32_t i = 0; i < RARBIT_CAPACITY; ++i) {
        if (rarbit_table[i].req_id == 0) {
            rarbit_table[i].req_id = req_id;
            rarbit_table[i].resource_id = resource_id;
            rarbit_table[i].priority = priority;
            rarbit_table[i].granted = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int rarbit_arbitrate(uint32_t resource_id, uint32_t *out_granted_req_id) {
    if (rarbit_ready != 1) return -1;

    uint32_t best_priority = 0;
    int best_slot = -1;

    for (uint32_t i = 0; i < RARBIT_CAPACITY; ++i) {
        rarbit_rec_t *rec = &rarbit_table[i];
        if (rec->req_id == 0 || rec->resource_id != resource_id || rec->granted == 1) continue;
        if (rec->priority <= best_priority) continue;

        best_priority = rec->priority;
        best_slot = (int)i;
    }

    if (best_slot < 0) return -1;

    rarbit_table[best_slot].granted = 1;
    *out_granted_req_id = rarbit_table[best_slot].req_id;
    return 0;
}

int rarbit_self_test(void) {
    uint32_t slot;
    if (rarbit_request(1, 1, 5, &slot) != 0) return -1;
    if (rarbit_request(2, 1, 10, &slot) != 0) return -1;

    uint32_t granted_req_id;
    if (rarbit_arbitrate(1, &granted_req_id) != 0) return -1;
    if (granted_req_id != 2) return -1; /* hoehere Prioritaet gewinnt */

    return 0;
}
