/* NovaOS C-Kernel - Nova Transaction Resource Manager (Port aus
   adaptive32.inc). */
#include "txres.h"

#define TXRES_FREE 0u
#define TXRES_HELD 1u

typedef struct {
    uint32_t txn_id;
    uint32_t resource_id;
    uint32_t state;
} txres_rec_t;

static txres_rec_t txres_table[TXRES_CAPACITY];
static int txres_ready = 0;

void txres_initialize(void) {
    if (txres_ready == 1) return;

    for (uint32_t i = 0; i < TXRES_CAPACITY; ++i) txres_table[i] = (txres_rec_t){0};
    txres_ready = 1;
}

int txres_acquire(uint32_t txn_id, uint32_t resource_id) {
    if (txres_ready != 1 || txn_id == 0) return -1;

    for (uint32_t i = 0; i < TXRES_CAPACITY; ++i) {
        if (txres_table[i].state == TXRES_HELD && txres_table[i].resource_id == resource_id) {
            return -1;
        }
    }

    for (uint32_t i = 0; i < TXRES_CAPACITY; ++i) {
        if (txres_table[i].state == TXRES_FREE) {
            txres_table[i].txn_id = txn_id;
            txres_table[i].resource_id = resource_id;
            txres_table[i].state = TXRES_HELD;
            return 0;
        }
    }
    return -1;
}

int txres_release_all(uint32_t txn_id) {
    if (txres_ready != 1 || txn_id == 0) return -1;

    for (uint32_t i = 0; i < TXRES_CAPACITY; ++i) {
        if (txres_table[i].txn_id == txn_id) {
            txres_table[i].state = TXRES_FREE;
        }
    }
    return 0;
}

int txres_self_test(void) {
    if (txres_acquire(1, 5) != 0) return -1;
    if (txres_acquire(2, 5) == 0) return -1; /* Konflikt erwartet */

    if (txres_release_all(1) != 0) return -1;

    if (txres_acquire(2, 5) != 0) return -1;
    if (txres_release_all(2) != 0) return -1;

    return 0;
}
