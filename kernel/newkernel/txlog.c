/* NovaOS C-Kernel - Nova Transaction Log (WAL-style) (Port aus
   adaptive32.inc). */
#include "txlog.h"

typedef struct {
    uint32_t txn_id;
    uint32_t op_type;
    uint32_t target;
    uint32_t value;
} txlog_rec_t;

static txlog_rec_t txlog_ring[TXLOG_CAPACITY];
static int txlog_ready = 0;
static uint32_t txlog_head = 0;
static uint32_t txlog_count = 0;

void txlog_initialize(void) {
    if (txlog_ready == 1) return;

    for (uint32_t i = 0; i < TXLOG_CAPACITY; ++i) txlog_ring[i] = (txlog_rec_t){0};
    txlog_head = 0;
    txlog_count = 0;
    txlog_ready = 1;
}

int txlog_append(uint32_t txn_id, uint32_t op_type, uint32_t target, uint32_t value) {
    if (txlog_ready != 1 || txn_id == 0) return -1;

    txlog_ring[txlog_head].txn_id = txn_id;
    txlog_ring[txlog_head].op_type = op_type;
    txlog_ring[txlog_head].target = target;
    txlog_ring[txlog_head].value = value;

    txlog_head = (txlog_head + 1 >= TXLOG_CAPACITY) ? 0 : txlog_head + 1;
    if (txlog_count < TXLOG_CAPACITY) ++txlog_count;

    return 0;
}

uint32_t txlog_count_entries(void) {
    return txlog_count;
}

int txlog_self_test(void) {
    if (txlog_append(1, TXLOG_WRITE, 0x1000, 0xABCD) != 0) return -1;
    if (txlog_append(1, TXLOG_COMMIT, 0, 0) != 0) return -1;

    if (txlog_count_entries() != 2) return -1;
    if (txlog_ring[0].txn_id != 1) return -1;
    if (txlog_ring[0].op_type != TXLOG_WRITE) return -1;
    if (txlog_ring[0].target != 0x1000) return -1;

    return 0;
}
