/* NovaOS C-Kernel - Nova Task Manager (Port aus concurrency32.inc,
   siehe task.h). */
#include "task.h"
#include "taskscope.h"
#include "process.h"

static task_record_t task_table[TASK_CAPACITY];
static uint32_t task_count_value = 0;
static uint32_t task_next_id = 1;
static int task_manager_ready = 0;

uint32_t task_count(void) {
    return task_count_value;
}

uint32_t task_slot_index(const task_record_t *rec) {
    return (uint32_t)(rec - task_table);
}

task_record_t *task_at_slot(uint32_t slot) {
    if (slot >= TASK_CAPACITY) return 0;
    return &task_table[slot];
}

task_record_t *task_lookup(uint32_t task_id) {
    for (uint32_t i = 0; i < TASK_CAPACITY; ++i) {
        task_record_t *rec = &task_table[i];
        if (rec->state == TASK_STATE_EMPTY) continue;
        if (rec->id == task_id) return rec;
    }
    return 0;
}

int task_create(uint32_t owner_pid, uint32_t scope_id, uint32_t parent_task, uint32_t flags, uint32_t *out_task_id) {
    if (process_lookup(owner_pid) == 0) return -1;

    task_scope_record_t *scope = task_scope_lookup(scope_id);
    if (!scope) return -1;
    if (scope->state != TASK_SCOPE_STATE_OPEN) return -1;
    if (scope->owner != owner_pid) return -1;

    if (parent_task != 0) {
        task_record_t *parent = task_lookup(parent_task);
        if (!parent) return -1;
        if (parent->owner != owner_pid) return -1;
        if (parent->scope != scope_id) return -1;
        if (parent->state >= TASK_STATE_CANCEL_REQUEST) return -1;
    }

    task_record_t *slot = 0;
    for (uint32_t i = 0; i < TASK_CAPACITY; ++i) {
        task_record_t *rec = &task_table[i];
        if (rec->state == TASK_STATE_EMPTY || rec->state >= TASK_STATE_COMPLETED) {
            slot = rec;
            break;
        }
    }
    if (!slot) return -1;

    uint32_t slot_index = task_slot_index(slot);
    task_group_on_slot_reused(slot_index);
    task_deadline_on_slot_reused(slot_index);

    slot->id = task_next_id;
    slot->owner = owner_pid;
    slot->scope = scope_id;
    slot->state = TASK_STATE_READY;
    slot->parent = parent_task;
    slot->flags = flags;
    slot->result = 0;
    slot->cancel_reason = 0;

    ++scope->active_tasks;
    ++task_count_value;
    ++task_next_id;
    *out_task_id = slot->id;
    return 0;
}

int task_release_scope(task_record_t *rec) {
    task_scope_record_t *scope = task_scope_lookup(rec->scope);
    if (!scope) return -1;
    if (scope->active_tasks == 0) return -1;

    --scope->active_tasks;
    --task_count_value;
    return 0;
}

int task_request_cancel(uint32_t task_id, uint32_t cancel_reason) {
    task_record_t *rec = task_lookup(task_id);
    if (!rec) return -1;
    if (rec->state < TASK_STATE_CREATED) return -1;
    if (rec->state > TASK_STATE_CANCEL_REQUEST) return -1;

    rec->state = TASK_STATE_CANCEL_REQUEST;
    rec->cancel_reason = cancel_reason;

    int changed;
    do {
        changed = 0;
        for (uint32_t i = 0; i < TASK_CAPACITY; ++i) {
            task_record_t *child = &task_table[i];
            if (child->state < TASK_STATE_CREATED) continue;
            if (child->state >= TASK_STATE_CANCEL_REQUEST) continue;
            if (child->parent == 0) continue;

            task_record_t *parent = task_lookup(child->parent);
            if (!parent) return -1;
            if (parent->state != TASK_STATE_CANCEL_REQUEST) continue;

            child->state = TASK_STATE_CANCEL_REQUEST;
            child->cancel_reason = parent->cancel_reason;
            changed = 1;
        }
    } while (changed);

    /* io_cancel_for_requested_tasks (io32.inc) wird NICHT aufgerufen -
       siehe task.h. */
    return 0;
}

void task_manager_cancel_scopes(uint32_t cancel_reason) {
    if (!task_manager_ready) return;

    for (uint32_t i = 0; i < TASK_CAPACITY; ++i) {
        task_record_t *rec = &task_table[i];
        if (rec->state < TASK_STATE_CREATED) continue;
        if (rec->state >= TASK_STATE_CANCEL_REQUEST) continue;

        task_scope_record_t *scope = task_scope_lookup(rec->scope);
        if (!scope) continue;
        if (scope->state != TASK_SCOPE_STATE_CANCELLING) continue;

        rec->state = TASK_STATE_CANCEL_REQUEST;
        rec->cancel_reason = cancel_reason;
    }
}

int task_checkpoint(uint32_t task_id, int *out_cancelled) {
    task_record_t *rec = task_lookup(task_id);
    if (!rec) return -1;

    if (rec->state != TASK_STATE_CANCEL_REQUEST) {
        *out_cancelled = 0;
        return 0;
    }

    rec->state = TASK_STATE_CANCELLED;
    task_group_on_terminal(rec, task_slot_index(rec));
    if (task_release_scope(rec) != 0) return -1;
    *out_cancelled = 1;
    return 0;
}

int task_complete(uint32_t task_id, uint32_t result) {
    task_record_t *rec = task_lookup(task_id);
    if (!rec) return -1;
    if (rec->state < TASK_STATE_CREATED) return -1;
    if (rec->state > TASK_STATE_WAITING) return -1;

    rec->state = TASK_STATE_COMPLETED;
    rec->result = result;
    task_group_on_terminal(rec, task_slot_index(rec));
    if (task_release_scope(rec) != 0) return -1;
    return 0;
}

int task_fail(uint32_t task_id, uint32_t result) {
    task_record_t *rec = task_lookup(task_id);
    if (!rec) return -1;
    if (rec->state < TASK_STATE_CREATED) return -1;
    if (rec->state > TASK_STATE_WAITING) return -1;

    rec->state = TASK_STATE_FAILED;
    rec->result = result;
    task_group_on_terminal(rec, task_slot_index(rec));
    if (task_release_scope(rec) != 0) return -1;
    return 0;
}

int task_manager_initialize(void) {
    for (uint32_t i = 0; i < TASK_CAPACITY; ++i) task_table[i] = (task_record_t){0};
    task_count_value = 0;
    task_next_id = 1;
    task_manager_ready = 1;
    return 0;
}

int task_manager_self_test(void) {
    /* Hierarchische Cancellation bleibt bis zu expliziten Checkpoints pending. */
    uint32_t scope;
    if (task_scope_create(1, task_scope_kernel_root_id, 0, &scope) != 0) return -1;

    uint32_t parent_task;
    if (task_create(1, scope, 0, 1, &parent_task) != 0) return -1;

    uint32_t child_task;
    if (task_create(1, scope, parent_task, 2, &child_task) != 0) return -1;

    if (task_scope_close(scope) == 0) return -1;

    if (task_request_cancel(parent_task, 0x43414E43u) != 0) return -1;

    task_record_t *child = task_lookup(child_task);
    if (!child) return -1;
    if (child->state != TASK_STATE_CANCEL_REQUEST) return -1;
    if (child->cancel_reason != 0x43414E43u) return -1;

    int cancelled;
    if (task_checkpoint(child_task, &cancelled) != 0) return -1;
    if (cancelled != 1) return -1;

    if (task_checkpoint(parent_task, &cancelled) != 0) return -1;
    if (cancelled != 1) return -1;

    if (task_scope_close(scope) != 0) return -1;

    /* Normaler Completion-Pfad bewahrt das Ergebnis und loest den
       Scopezaehler. */
    uint32_t scope2;
    if (task_scope_create(1, task_scope_kernel_root_id, 0, &scope2) != 0) return -1;

    uint32_t task_id;
    if (task_create(1, scope2, 0, 0, &task_id) != 0) return -1;

    if (task_complete(task_id, 0x4F4B0001u) != 0) return -1;

    task_record_t *rec = task_lookup(task_id);
    if (!rec) return -1;
    if (rec->state != TASK_STATE_COMPLETED) return -1;
    if (rec->result != 0x4F4B0001u) return -1;

    if (task_scope_close(scope2) != 0) return -1;
    if (task_count_value != 0) return -1;

    return 0;
}
