/* NovaOS C-Kernel - Nova Transaction Rollback Journal (Port aus
   adaptive32.inc). */
#include "txroll.h"

typedef struct {
    uint32_t *address;
    uint32_t saved_value;
    uint32_t active;
} txroll_rec_t;

static txroll_rec_t txroll_journal[TXROLL_CAPACITY];
static int txroll_ready = 0;
static uint32_t txroll_count = 0;
static uint32_t txroll_undone = 0;

void txroll_initialize(void) {
    for (uint32_t i = 0; i < TXROLL_CAPACITY; ++i) txroll_journal[i] = (txroll_rec_t){0};
    txroll_count = 0;
    txroll_undone = 0;
    txroll_ready = 1;
}

int txroll_save(uint32_t *address) {
    if (txroll_ready != 1) return -1;
    if (txroll_count >= TXROLL_CAPACITY) return -1;

    for (uint32_t i = 0; i < TXROLL_CAPACITY; ++i) {
        if (txroll_journal[i].active == 0) {
            txroll_journal[i].address = address;
            txroll_journal[i].saved_value = *address;
            txroll_journal[i].active = 1;
            ++txroll_count;
            return 0;
        }
    }
    return -1;
}

void txroll_undo(void) {
    if (txroll_ready != 1) return;

    for (uint32_t i = 0; i < TXROLL_CAPACITY; ++i) {
        if (txroll_journal[i].active != 0) {
            *txroll_journal[i].address = txroll_journal[i].saved_value;
            txroll_journal[i].active = 0;
            ++txroll_undone;
        }
    }
    txroll_count = 0;
}

void txroll_commit(void) {
    for (uint32_t i = 0; i < TXROLL_CAPACITY; ++i) txroll_journal[i] = (txroll_rec_t){0};
    txroll_count = 0;
}

int txroll_self_test(void) {
    static uint32_t test_var;

    test_var = 42;
    if (txroll_save(&test_var) != 0) return -1;

    test_var = 99;
    txroll_undo();
    if (test_var != 42) return -1;

    test_var = 77;
    if (txroll_save(&test_var) != 0) return -1;

    test_var = 55;
    txroll_commit();
    if (test_var != 55) return -1;

    return 0;
}
