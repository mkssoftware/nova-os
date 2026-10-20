/* NovaOS C-Kernel - State-Manager (Port aus statemgr32.inc). */
#include "statemgr.h"

typedef struct {
    uint32_t obj_id;
    uint32_t actual;
    uint32_t desired;
    uint32_t version;
    uint32_t flags;
    uint32_t snapshot_id;
    uint32_t history_head;
    uint32_t machine_state;
    uint32_t tx_id;
    uint32_t reconcile_result;
    uint32_t timestamp;
} sm_record_t;

static sm_record_t sm_table[SM_CAPACITY];
static uint32_t sm_count = 0;
static uint32_t sm_global_state = SM_STATE_RUNNING;
static uint32_t sm_snapshot_gen = 0;
static int sm_ready = 0;

/* TODO: durch timer_ticks ersetzen, sobald das Time-Subsystem portiert ist. */
static uint32_t current_timer_ticks(void) {
    return 0;
}

void statemgr_initialize(void) {
    for (uint32_t i = 0; i < SM_CAPACITY; ++i) {
        sm_table[i] = (sm_record_t){0};
    }
    sm_count = 0;
    sm_global_state = SM_STATE_RUNNING;
    sm_snapshot_gen = 0;
    sm_ready = 1;
}

int state_register(uint32_t obj_id, uint32_t initial_state, uint32_t *out_slot) {
    if (sm_count >= SM_CAPACITY) {
        return -1;
    }
    sm_record_t *rec = &sm_table[sm_count];
    rec->obj_id = obj_id;
    rec->actual = initial_state;
    rec->desired = initial_state;
    rec->version = 1;
    rec->flags = SM_FLAG_SNAPSHOTTABLE | SM_FLAG_ROLLBACK;
    rec->snapshot_id = 0;
    rec->history_head = 0;
    rec->machine_state = initial_state;
    rec->tx_id = 0;
    rec->reconcile_result = SM_RECONCILE_OK;
    rec->timestamp = current_timer_ticks();

    if (out_slot) {
        *out_slot = sm_count;
    }
    ++sm_count;
    return 0;
}

static sm_record_t *statemgr_find(uint32_t obj_id) {
    for (uint32_t i = 0; i < sm_count; ++i) {
        if (sm_table[i].obj_id == obj_id) {
            return &sm_table[i];
        }
    }
    return 0;
}

int state_set_desired(uint32_t obj_id, uint32_t desired_state) {
    sm_record_t *rec = statemgr_find(obj_id);
    if (!rec) {
        return -1;
    }
    rec->desired = desired_state;
    rec->flags |= SM_FLAG_DIRTY;
    rec->reconcile_result = (desired_state == rec->actual) ? SM_RECONCILE_OK : SM_RECONCILE_DRIFT;
    ++rec->version;
    return 0;
}

int state_reconcile(uint32_t obj_id, uint32_t *out_result) {
    sm_record_t *rec = statemgr_find(obj_id);
    if (!rec) {
        *out_result = SM_RECONCILE_CONFLICT;
        return -1;
    }
    rec->actual = rec->desired;
    rec->machine_state = rec->desired;
    rec->flags &= ~SM_FLAG_DIRTY;
    rec->reconcile_result = SM_RECONCILE_OK;
    *out_result = SM_RECONCILE_OK;
    return 0;
}

int state_snapshot(uint32_t obj_id, uint32_t *out_snapshot_id) {
    sm_record_t *rec = statemgr_find(obj_id);
    if (!rec) {
        if (out_snapshot_id) *out_snapshot_id = 0;
        return -1;
    }
    ++sm_snapshot_gen;
    rec->snapshot_id = sm_snapshot_gen;
    if (out_snapshot_id) {
        *out_snapshot_id = sm_snapshot_gen;
    }
    return 0;
}

int statemgr_self_test(void) {
    return (sm_ready == 1) ? 0 : -1;
}
