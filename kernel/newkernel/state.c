/* NovaOS C-Kernel - Nova Global-State- und Transaction-Bootstrapkern
   (siehe state.h fuer Scope). */
#include "state.h"

/* ===================================================================== */
/* State Manager                                                         */
/* ===================================================================== */

typedef struct {
    uint32_t id_high, id_low; /* id_high == 0 => Slot frei */
    uint32_t kind;
    uint32_t owner;
    uint32_t version;
    uint32_t generation;
    uint32_t previous_version;
    uint32_t validity;
} state_record_t;

static state_record_t state_records[STATE_RECORD_CAPACITY];
static uint32_t state_generation_counter = 0;

void state_manager_initialize(void) {
    for (uint32_t i = 0; i < STATE_RECORD_CAPACITY; ++i) state_records[i] = (state_record_t){0};
    state_generation_counter = 1;
}

uint32_t state_generation_counter_value(void) {
    return state_generation_counter;
}

int state_create(uint32_t id_high, uint32_t id_low, uint32_t kind, uint32_t owner,
                  uint32_t *out_slot) {
    if (id_high == 0u || id_low == 0u || kind == 0u || owner == 0u) return -1;

    for (uint32_t i = 0; i < STATE_RECORD_CAPACITY; ++i) {
        if (state_records[i].id_high == 0u) {
            state_records[i].id_high = id_high;
            state_records[i].id_low = id_low;
            state_records[i].kind = kind;
            state_records[i].owner = owner;
            state_records[i].version = 1u;
            state_records[i].generation = state_generation_counter;
            state_records[i].previous_version = 0u;
            state_records[i].validity = STATE_VALIDITY_UNKNOWN;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int state_transition(uint32_t slot, uint32_t expected_version, uint32_t new_validity) {
    if (slot >= STATE_RECORD_CAPACITY) return -1;
    if (new_validity > STATE_VALIDITY_STALE) return -1;

    state_record_t *rec = &state_records[slot];
    if (rec->id_high == 0u) return -1;

    if (rec->version != expected_version) {
        rec->validity = STATE_VALIDITY_STALE;
        return -1;
    }

    rec->previous_version = rec->version;
    ++rec->version;
    rec->validity = new_validity;
    return 0;
}

int state_manager_self_test(void) {
    uint32_t slot;
    if (state_create(STATE_BOOT_ID_HIGH, STATE_BOOT_ID_LOW, STATE_KIND_KERNEL, 1u, &slot) != 0) {
        return -1;
    }
    if (state_records[slot].validity != STATE_VALIDITY_UNKNOWN) return -1;

    if (state_transition(slot, 1u, STATE_VALIDITY_VALID) != 0) return -1;
    if (state_records[slot].version != 2u) return -1;
    if (state_records[slot].previous_version != 1u) return -1;
    if (state_records[slot].validity != STATE_VALIDITY_VALID) return -1;

    /* Veralteter expected_version (1, aktuelle Version ist jetzt 2) muss
       als Konflikt erkannt werden (Validity -> STALE), ohne die Version
       zu erhoehen. */
    if (state_transition(slot, 1u, STATE_VALIDITY_INVALID) == 0) return -1;
    if (state_records[slot].validity != STATE_VALIDITY_STALE) return -1;

    return 0;
}

/* ===================================================================== */
/* Transaction Manager                                                   */
/* ===================================================================== */

#define TXN_STATE_FREE 0u

typedef struct {
    uint32_t txn_id; /* TXN_STATE_FREE als state bedeutet frei, nicht id==0 */
    uint32_t owner;
    uint32_t scope;
    uint32_t state;
    uint32_t expected_version;
    uint32_t commit_generation;
} txn_record_t;

static txn_record_t txn_records[TRANSACTION_CAPACITY];
static uint32_t transaction_next_id = 0;
static uint32_t transaction_active_count = 0;

void transaction_manager_initialize(void) {
    for (uint32_t i = 0; i < TRANSACTION_CAPACITY; ++i) txn_records[i] = (txn_record_t){0};
    transaction_next_id = 1;
    transaction_active_count = 0;
}

static int transaction_find_slot(uint32_t txn_id) {
    for (uint32_t i = 0; i < TRANSACTION_CAPACITY; ++i) {
        if (txn_records[i].state != TXN_STATE_FREE && txn_records[i].txn_id == txn_id) {
            return (int)i;
        }
    }
    return -1;
}

int transaction_begin(uint32_t owner, uint32_t scope, uint32_t expected_version,
                       uint32_t *out_txn_id) {
    if (owner == 0u || scope == 0u) return -1;

    for (uint32_t i = 0; i < TRANSACTION_CAPACITY; ++i) {
        if (txn_records[i].state == TXN_STATE_FREE) {
            uint32_t id = transaction_next_id++;
            txn_records[i].txn_id = id;
            txn_records[i].owner = owner;
            txn_records[i].scope = scope;
            txn_records[i].state = TRANSACTION_ACTIVE;
            txn_records[i].expected_version = expected_version;
            txn_records[i].commit_generation = 0u;
            ++transaction_active_count;
            *out_txn_id = id;
            return 0;
        }
    }
    return -1;
}

int transaction_prepare(uint32_t txn_id, uint32_t expected_version) {
    int slot = transaction_find_slot(txn_id);
    if (slot < 0) return -1;
    if (txn_records[slot].state != TRANSACTION_ACTIVE) return -1;

    if (txn_records[slot].expected_version != expected_version) {
        txn_records[slot].state = TRANSACTION_FAILED;
        return -1;
    }

    txn_records[slot].state = TRANSACTION_PREPARED;
    return 0;
}

int transaction_commit(uint32_t txn_id) {
    int slot = transaction_find_slot(txn_id);
    if (slot < 0) return -1;
    if (txn_records[slot].state != TRANSACTION_PREPARED) return -1;

    txn_records[slot].state = TRANSACTION_COMMITTED;
    txn_records[slot].commit_generation = state_generation_counter_value();
    return 0;
}

int transaction_verify(uint32_t txn_id) {
    int slot = transaction_find_slot(txn_id);
    if (slot < 0) return -1;
    if (txn_records[slot].state != TRANSACTION_COMMITTED) return -1;

    txn_records[slot].state = TRANSACTION_COMPLETED;
    --transaction_active_count;
    return 0;
}

int transaction_manager_self_test(void) {
    uint32_t txn_id;
    if (transaction_begin(1u, TRANSACTION_SCOPE_SERVICE, 2u, &txn_id) != 0) return -1;
    if (transaction_prepare(txn_id, 1u) == 0) return -1; /* falsche Version -> Konflikt */

    if (transaction_begin(1u, TRANSACTION_SCOPE_SERVICE, 2u, &txn_id) != 0) return -1;
    if (transaction_prepare(txn_id, 2u) != 0) return -1;

    int slot = transaction_find_slot(txn_id);
    if (slot < 0 || txn_records[slot].state != TRANSACTION_PREPARED) return -1;

    if (transaction_commit(txn_id) != 0) return -1;
    slot = transaction_find_slot(txn_id);
    if (slot < 0 || txn_records[slot].state != TRANSACTION_COMMITTED) return -1;

    if (transaction_verify(txn_id) != 0) return -1;
    slot = transaction_find_slot(txn_id);
    if (slot < 0 || txn_records[slot].state != TRANSACTION_COMPLETED) return -1;

    return 0;
}
