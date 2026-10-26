/* NovaOS C-Kernel - Nova State Reconciliation (Port aus adaptive32.inc). */
#include "srec.h"

typedef struct {
    uint32_t id; /* 0 = frei */
    uint32_t desired;
    uint32_t actual;
    uint32_t diverged;
} srec_rec_t;

static srec_rec_t srec_table[SREC_CAPACITY];
static int srec_ready = 0;

void srec_initialize(void) {
    if (srec_ready == 1) return;

    for (uint32_t i = 0; i < SREC_CAPACITY; ++i) srec_table[i] = (srec_rec_t){0};
    srec_ready = 1;
}

int srec_register(uint32_t rec_id, uint32_t desired_state, uint32_t *out_slot) {
    if (srec_ready != 1 || rec_id == 0) return -1;

    for (uint32_t i = 0; i < SREC_CAPACITY; ++i) {
        if (srec_table[i].id == 0) {
            srec_table[i].id = rec_id;
            srec_table[i].desired = desired_state;
            srec_table[i].actual = 0;
            srec_table[i].diverged = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

static srec_rec_t *srec_find(uint32_t rec_id) {
    for (uint32_t i = 0; i < SREC_CAPACITY; ++i) {
        if (srec_table[i].id == rec_id) return &srec_table[i];
    }
    return 0;
}

int srec_reconcile(uint32_t rec_id, uint32_t actual_state) {
    if (srec_ready != 1 || rec_id == 0) return -1;

    srec_rec_t *rec = srec_find(rec_id);
    if (!rec) return -1;

    rec->actual = actual_state;
    if (actual_state == rec->desired) return 0;

    ++rec->diverged;
    return -1;
}

int srec_query(uint32_t rec_id, uint32_t *out_desired, uint32_t *out_actual, uint32_t *out_diverge_count) {
    if (srec_ready != 1 || rec_id == 0) return -1;

    srec_rec_t *rec = srec_find(rec_id);
    if (!rec) return -1;

    *out_desired = rec->desired;
    *out_actual = rec->actual;
    *out_diverge_count = rec->diverged;
    return 0;
}

int srec_self_test(void) {
    uint32_t slot;
    if (srec_register(1, 2, &slot) != 0) return -1;

    if (srec_reconcile(1, 2) != 0) return -1; /* aligned */
    if (srec_reconcile(1, 3) == 0) return -1; /* diverged */

    uint32_t desired, actual, diverge_count;
    if (srec_query(1, &desired, &actual, &diverge_count) != 0) return -1;
    if (diverge_count != 1) return -1;

    return 0;
}
