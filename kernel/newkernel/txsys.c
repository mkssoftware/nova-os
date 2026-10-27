/* NovaOS C-Kernel - Nova Transaction System Coordinator (Port aus
   adaptive32.inc). */
#include "txsys.h"

typedef struct {
    uint32_t id; /* nur gueltig wenn state != IDLE */
    uint32_t participants;
    uint32_t ready_count;
    uint32_t state;
} txsys_rec_t;

static txsys_rec_t txsys_table[TXSYS_CAPACITY];
static int txsys_ready = 0;

void txsys_initialize(void) {
    if (txsys_ready == 1) return;

    for (uint32_t i = 0; i < TXSYS_CAPACITY; ++i) txsys_table[i] = (txsys_rec_t){0};
    txsys_ready = 1;
}

int txsys_begin(uint32_t sys_txn_id, uint32_t participant_count, uint32_t *out_slot) {
    if (txsys_ready != 1 || sys_txn_id == 0 || participant_count == 0) return -1;

    for (uint32_t i = 0; i < TXSYS_CAPACITY; ++i) {
        if (txsys_table[i].state == TXSYS_IDLE) {
            txsys_table[i].id = sys_txn_id;
            txsys_table[i].participants = participant_count;
            txsys_table[i].ready_count = 0;
            txsys_table[i].state = TXSYS_PREPARE;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

static txsys_rec_t *txsys_find(uint32_t sys_txn_id) {
    for (uint32_t i = 0; i < TXSYS_CAPACITY; ++i) {
        if (txsys_table[i].state != TXSYS_IDLE && txsys_table[i].id == sys_txn_id) {
            return &txsys_table[i];
        }
    }
    return 0;
}

int txsys_participant_ready(uint32_t sys_txn_id) {
    if (txsys_ready != 1) return -1;

    txsys_rec_t *rec = txsys_find(sys_txn_id);
    if (!rec || rec->state != TXSYS_PREPARE) return -1;

    ++rec->ready_count;
    if (rec->ready_count == rec->participants) {
        rec->state = TXSYS_COMMITTED;
    }
    return 0;
}

int txsys_abort(uint32_t sys_txn_id) {
    if (txsys_ready != 1) return -1;

    txsys_rec_t *rec = txsys_find(sys_txn_id);
    if (!rec) return -1;

    rec->state = TXSYS_ABORTED;
    return 0;
}

int txsys_query_state(uint32_t sys_txn_id, uint32_t *out_state) {
    if (txsys_ready != 1) return -1;

    txsys_rec_t *rec = txsys_find(sys_txn_id);
    if (!rec) return -1;

    *out_state = rec->state;
    return 0;
}

int txsys_self_test(void) {
    uint32_t slot;
    if (txsys_begin(1, 2, &slot) != 0) return -1;

    if (txsys_participant_ready(1) != 0) return -1;
    if (txsys_table[slot].state != TXSYS_PREPARE) return -1;

    if (txsys_participant_ready(1) != 0) return -1;
    if (txsys_table[slot].state != TXSYS_COMMITTED) return -1;

    /* Slot fuer den Abort-Testfall freigeben (wie im alten ASM: direkter
       Reset auf IDLE statt eines eigenen "end"-Aufrufs). */
    txsys_table[slot].state = TXSYS_IDLE;

    uint32_t slot2;
    if (txsys_begin(2, 1, &slot2) != 0) return -1;
    if (txsys_abort(2) != 0) return -1;

    return 0;
}
