/* NovaOS C-Kernel - Nova Transaction Config Manager (Port aus
   adaptive32.inc). */
#include "txconfig.h"
#include "sconf.h"

typedef struct {
    uint32_t txn_id; /* 0 = frei */
    uint32_t key;
    uint32_t old_val;
    uint32_t new_val;
} txconfig_rec_t;

static txconfig_rec_t txconfig_table[TXCFG_CAPACITY];
static int txconfig_ready = 0;

void txconfig_initialize(void) {
    if (txconfig_ready == 1) return;

    for (uint32_t i = 0; i < TXCFG_CAPACITY; ++i) txconfig_table[i] = (txconfig_rec_t){0};
    txconfig_ready = 1;
}

int txconfig_stage(uint32_t cfg_txn_id, uint32_t key, uint32_t new_value) {
    if (txconfig_ready != 1 || cfg_txn_id == 0) return -1;

    for (uint32_t i = 0; i < TXCFG_CAPACITY; ++i) {
        if (txconfig_table[i].txn_id == 0) {
            uint32_t old_value, old_version;
            if (sconf_get(key, &old_value, &old_version) != 0) old_value = 0;

            txconfig_table[i].txn_id = cfg_txn_id;
            txconfig_table[i].key = key;
            txconfig_table[i].old_val = old_value;
            txconfig_table[i].new_val = new_value;
            return 0;
        }
    }
    return -1;
}

int txconfig_commit(uint32_t cfg_txn_id) {
    if (txconfig_ready != 1) return -1;

    for (uint32_t i = 0; i < TXCFG_CAPACITY; ++i) {
        if (txconfig_table[i].txn_id == cfg_txn_id) {
            sconf_set(txconfig_table[i].key, txconfig_table[i].new_val);
            txconfig_table[i].txn_id = 0;
        }
    }
    return 0;
}

int txconfig_abort(uint32_t cfg_txn_id) {
    if (txconfig_ready != 1) return -1;

    for (uint32_t i = 0; i < TXCFG_CAPACITY; ++i) {
        if (txconfig_table[i].txn_id == cfg_txn_id) {
            sconf_set(txconfig_table[i].key, txconfig_table[i].old_val);
            txconfig_table[i].txn_id = 0;
        }
    }
    return 0;
}

int txconfig_self_test(void) {
    if (txconfig_stage(1, 10, 42) != 0) return -1;
    if (txconfig_commit(1) != 0) return -1;

    uint32_t value, version;
    if (sconf_get(10, &value, &version) != 0) return -1;
    if (value != 42) return -1;

    if (txconfig_stage(2, 10, 99) != 0) return -1;
    if (txconfig_abort(2) != 0) return -1;

    if (sconf_get(10, &value, &version) != 0) return -1;
    if (value != 42) return -1;

    return 0;
}
