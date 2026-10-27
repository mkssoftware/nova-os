/* NovaOS C-Kernel - Nova Tx MultiService Coordinator (Port aus
   adaptive32.inc). */
#include "txmulti.h"
#include "txsys.h"

typedef struct {
    uint32_t txn_id; /* 0 = frei */
    uint32_t service_id;
    uint32_t vote;
    uint32_t applied;
} txmulti_rec_t;

static txmulti_rec_t txmulti_table[TXMULTI_CAPACITY];
static int txmulti_ready = 0;

void txmulti_initialize(void) {
    if (txmulti_ready == 1) return;

    for (uint32_t i = 0; i < TXMULTI_CAPACITY; ++i) txmulti_table[i] = (txmulti_rec_t){0};
    txmulti_ready = 1;
}

int txmulti_register_service(uint32_t ms_txn_id, uint32_t service_id) {
    if (txmulti_ready != 1 || ms_txn_id == 0) return -1;

    for (uint32_t i = 0; i < TXMULTI_CAPACITY; ++i) {
        if (txmulti_table[i].txn_id == 0) {
            txmulti_table[i].txn_id = ms_txn_id;
            txmulti_table[i].service_id = service_id;
            txmulti_table[i].vote = TXMULTI_PENDING;
            txmulti_table[i].applied = 0;
            return 0;
        }
    }
    return -1;
}

int txmulti_vote(uint32_t ms_txn_id, uint32_t service_id, uint32_t vote) {
    if (txmulti_ready != 1) return -1;

    for (uint32_t i = 0; i < TXMULTI_CAPACITY; ++i) {
        txmulti_rec_t *rec = &txmulti_table[i];
        if (rec->txn_id != ms_txn_id || rec->service_id != service_id) continue;

        rec->vote = vote;
        if (vote == TXMULTI_COMMIT) {
            txsys_participant_ready(ms_txn_id);
        }
        return 0;
    }
    return -1;
}

int txmulti_check(uint32_t ms_txn_id) {
    if (txmulti_ready != 1) return -1;

    uint32_t found_count = 0;
    for (uint32_t i = 0; i < TXMULTI_CAPACITY; ++i) {
        if (txmulti_table[i].txn_id != ms_txn_id) continue;
        ++found_count;
        if (txmulti_table[i].vote != TXMULTI_COMMIT) return -1;
    }
    if (found_count == 0) return -1;
    return 0;
}

int txmulti_self_test(void) {
    uint32_t slot;
    txsys_begin(1, 2, &slot); /* CF ignorieren - txsys koennte schon belegt sein */

    if (txmulti_register_service(1, 1) != 0) return -1;
    if (txmulti_register_service(1, 2) != 0) return -1;

    if (txmulti_vote(1, 1, TXMULTI_COMMIT) != 0) return -1;
    if (txmulti_vote(1, 2, TXMULTI_COMMIT) != 0) return -1;

    if (txmulti_check(1) != 0) return -1;

    return 0;
}
