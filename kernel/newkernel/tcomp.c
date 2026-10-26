/* NovaOS C-Kernel - Nova Transaction Compensation (Port aus
   adaptive32.inc). */
#include "tcomp.h"

typedef struct {
    uint32_t txn_id;
    tcomp_fn_t fn;
    uint32_t arg;
    uint32_t done;
} tcomp_rec_t;

static tcomp_rec_t tcomp_table[TCOMP_CAPACITY];
static int tcomp_ready = 0;
static uint32_t tcomp_count = 0;

void tcomp_initialize(void) {
    if (tcomp_ready == 1) return;

    for (uint32_t i = 0; i < TCOMP_CAPACITY; ++i) tcomp_table[i] = (tcomp_rec_t){0};
    tcomp_count = 0;
    tcomp_ready = 1;
}

int tcomp_register(uint32_t txn_id, tcomp_fn_t comp_fn, uint32_t arg, uint32_t *out_slot) {
    if (tcomp_ready != 1 || txn_id == 0 || comp_fn == 0) return -1;
    if (tcomp_count >= TCOMP_CAPACITY) return -1;

    tcomp_table[tcomp_count].txn_id = txn_id;
    tcomp_table[tcomp_count].fn = comp_fn;
    tcomp_table[tcomp_count].arg = arg;
    tcomp_table[tcomp_count].done = 0;
    *out_slot = tcomp_count;
    ++tcomp_count;
    return 0;
}

int tcomp_run_all(uint32_t txn_id) {
    if (tcomp_ready != 1) return -1;

    for (uint32_t i = tcomp_count; i > 0; --i) {
        tcomp_rec_t *rec = &tcomp_table[i - 1];
        if (rec->txn_id != txn_id || rec->done == 1) continue;

        rec->fn(rec->arg);
        rec->done = 1;
    }
    return 0;
}

static void tcomp_test_stub(uint32_t arg) {
    (void)arg;
}

int tcomp_self_test(void) {
    uint32_t slot;
    if (tcomp_register(1, tcomp_test_stub, 0, &slot) != 0) return -1;
    if (tcomp_register(1, tcomp_test_stub, 0, &slot) != 0) return -1;

    if (tcomp_run_all(1) != 0) return -1;

    if (tcomp_table[0].done != 1) return -1;
    if (tcomp_table[1].done != 1) return -1;

    return 0;
}
