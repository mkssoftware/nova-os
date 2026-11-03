/* NovaOS C-Kernel - System-Transaction-Manager (Port aus txmgr32.inc). */
#include "txmgr.h"
#include "interrupt.h"

typedef struct {
    uint32_t id;
    uint32_t type;
    uint32_t state;
    uint32_t flags;
    uint32_t participant_count;
    uint32_t log_head;
    txmgr_compensation_fn_t comp_fn;
    uint32_t barrier_id;
    uint32_t start_ticks;
    uint32_t timeout_ticks;
    uint32_t resource_mask;
} sysmgr_tx_t;

static sysmgr_tx_t sysmgr_tx_table[SYSMGR_TX_CAPACITY];
static uint32_t sysmgr_tx_count = 0;
static uint32_t sysmgr_tx_next_id = 1;
static int sysmgr_tx_ready = 0;

static uint32_t current_timer_ticks(void) {
    return timer_get_ticks();
}

void txmgr_initialize(void) {
    for (uint32_t i = 0; i < SYSMGR_TX_CAPACITY; ++i) {
        sysmgr_tx_table[i] = (sysmgr_tx_t){0};
    }
    sysmgr_tx_count = 0;
    sysmgr_tx_next_id = 1;
    sysmgr_tx_ready = 1;
}

int txmgr_begin(uint32_t type, uint32_t flags, uint32_t timeout_ticks, uint32_t *out_tx_id) {
    if (sysmgr_tx_count >= SYSMGR_TX_CAPACITY) {
        return -1;
    }
    sysmgr_tx_t *tx = &sysmgr_tx_table[sysmgr_tx_count];
    tx->id = sysmgr_tx_next_id;
    tx->type = type;
    tx->state = TX_STATE_ACTIVE;
    tx->flags = flags;
    tx->participant_count = 1;
    tx->log_head = 0;
    tx->comp_fn = 0;
    tx->barrier_id = 0;
    tx->start_ticks = current_timer_ticks();
    tx->timeout_ticks = timeout_ticks;
    tx->resource_mask = 0;

    if (out_tx_id) {
        *out_tx_id = sysmgr_tx_next_id;
    }
    ++sysmgr_tx_next_id;
    ++sysmgr_tx_count;
    return 0;
}

static sysmgr_tx_t *txmgr_find(uint32_t tx_id) {
    for (uint32_t i = 0; i < sysmgr_tx_count; ++i) {
        if (sysmgr_tx_table[i].id == tx_id) {
            return &sysmgr_tx_table[i];
        }
    }
    return 0;
}

int txmgr_register_compensation(uint32_t tx_id, txmgr_compensation_fn_t fn) {
    sysmgr_tx_t *tx = txmgr_find(tx_id);
    if (!tx) {
        return -1;
    }
    tx->comp_fn = fn;
    return 0;
}

int txmgr_commit(uint32_t tx_id) {
    sysmgr_tx_t *tx = txmgr_find(tx_id);
    if (!tx) {
        return -1;
    }
    if (tx->state != TX_STATE_ACTIVE) {
        tx->state = TX_STATE_FAILED;
        return -1;
    }
    if (tx->timeout_ticks != 0) {
        uint32_t elapsed = current_timer_ticks() - tx->start_ticks;
        if (elapsed >= tx->timeout_ticks) {
            tx->state = TX_STATE_FAILED;
            return -1;
        }
    }
    tx->state = TX_STATE_COMMITTED;
    return 0;
}

int txmgr_rollback(uint32_t tx_id) {
    sysmgr_tx_t *tx = txmgr_find(tx_id);
    if (!tx) {
        return -1;
    }
    if ((tx->flags & TX_FLAG_COMPENSATE) && tx->comp_fn) {
        tx->comp_fn(tx_id);
    }
    tx->state = TX_STATE_ROLLED_BACK;
    return 0;
}

static volatile uint32_t txmgr_self_test_comp_calls = 0;
static void txmgr_self_test_compensation(uint32_t tx_id) {
    (void)tx_id;
    ++txmgr_self_test_comp_calls;
}

int txmgr_self_test(void) {
    if (sysmgr_tx_ready != 1) return -1;

    /* Vorher wurde nur sysmgr_tx_ready geprueft - jetzt zusaetzlich ein
       echter Begin->Commit- und ein Begin->Rollback-mit-Kompensation-Weg,
       analog zu den bereits starken Selbsttests der anderen tx*.c-Module. */
    uint32_t tx_id = 0;
    if (txmgr_begin(TX_TYPE_LOCAL, 0u, 0u, &tx_id) != 0) return -1;
    if (txmgr_commit(tx_id) != 0) return -1;

    uint32_t rollback_tx_id = 0;
    if (txmgr_begin(TX_TYPE_LOCAL, TX_FLAG_COMPENSATE, 0u, &rollback_tx_id) != 0) return -1;
    if (txmgr_register_compensation(rollback_tx_id, txmgr_self_test_compensation) != 0) return -1;
    txmgr_self_test_comp_calls = 0;
    if (txmgr_rollback(rollback_tx_id) != 0) return -1;
    if (txmgr_self_test_comp_calls != 1) return -1; /* Kompensation MUSS ausgeloest worden sein */

    if (txmgr_register_compensation(0xFFFFu, txmgr_self_test_compensation) == 0) return -1; /* unbekannte tx_id */

    return 0;
}
