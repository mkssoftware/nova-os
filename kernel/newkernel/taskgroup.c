/* NovaOS C-Kernel - Nova Task Group Manager (Port aus concurrency32.inc,
   siehe taskgroup.h). */
#include "taskgroup.h"
#include "task.h"
#include "taskscope.h"
#include "process.h"

static task_group_record_t task_group_table[TASK_GROUP_CAPACITY];
static uint32_t task_group_ids[TASK_CAPACITY]; /* 0 = keiner Gruppe zugeordnet */
static uint32_t task_group_count = 0;
static uint32_t task_group_next_id = 1;
static int task_group_ready = 0;

void task_group_on_slot_reused(uint32_t slot) {
    if (slot >= TASK_CAPACITY) return;
    task_group_ids[slot] = 0;
}

task_group_record_t *task_group_lookup(uint32_t group_id) {
    for (uint32_t i = 0; i < TASK_GROUP_CAPACITY; ++i) {
        task_group_record_t *rec = &task_group_table[i];
        if (rec->state == TASK_GROUP_STATE_EMPTY) continue;
        if (rec->id == group_id) return rec;
    }
    return 0;
}

int task_group_create(uint32_t owner_pid, uint32_t scope_id, uint32_t policy, uint32_t *out_group_id) {
    if (policy < TASK_GROUP_POLICY_WAIT_ALL || policy > TASK_GROUP_POLICY_FAIL_FAST) return -1;
    if (process_lookup(owner_pid) == 0) return -1;

    task_scope_record_t *scope = task_scope_lookup(scope_id);
    if (!scope) return -1;
    if (scope->state != TASK_SCOPE_STATE_OPEN) return -1;
    if (scope->owner != owner_pid) return -1;

    task_group_record_t *slot = 0;
    for (uint32_t i = 0; i < TASK_GROUP_CAPACITY; ++i) {
        task_group_record_t *rec = &task_group_table[i];
        if (rec->state == TASK_GROUP_STATE_EMPTY || rec->state >= TASK_GROUP_STATE_COMPLETED) {
            slot = rec;
            break;
        }
    }
    if (!slot) return -1;

    slot->id = task_group_next_id;
    slot->owner = owner_pid;
    slot->scope = scope_id;
    slot->state = TASK_GROUP_STATE_OPEN;
    slot->policy = policy;
    slot->required = 0;
    slot->active = 0;
    slot->failures = 0;

    ++task_group_count;
    ++task_group_next_id;
    *out_group_id = slot->id;
    return 0;
}

int task_group_attach(uint32_t group_id, uint32_t task_id) {
    task_group_record_t *group = task_group_lookup(group_id);
    if (!group) return -1;
    if (group->state != TASK_GROUP_STATE_OPEN) return -1;

    task_record_t *task = task_lookup(task_id);
    if (!task) return -1;
    if (task->state < TASK_STATE_CREATED || task->state > TASK_STATE_WAITING) return -1;
    if (task->owner != group->owner) return -1;
    if (task->scope != group->scope) return -1;

    uint32_t slot = task_slot_index(task);
    if (task_group_ids[slot] != 0) return -1;

    task_group_ids[slot] = group_id;
    ++group->required;
    ++group->active;
    return 0;
}

static void task_group_cancel_remaining(uint32_t group_id, uint32_t reason) {
    for (uint32_t i = 0; i < TASK_CAPACITY; ++i) {
        if (task_group_ids[i] != group_id) continue;

        task_record_t *rec = task_at_slot(i);
        if (rec->state < TASK_STATE_CREATED) continue;
        if (rec->state >= TASK_STATE_CANCEL_REQUEST) continue;

        task_request_cancel(rec->id, reason);
    }
}

int task_group_cancel(uint32_t group_id, uint32_t cancel_reason) {
    task_group_record_t *group = task_group_lookup(group_id);
    if (!group) return -1;
    if (group->state != TASK_GROUP_STATE_OPEN) return -1;

    group->state = TASK_GROUP_STATE_CANCELLING;
    task_group_cancel_remaining(group_id, cancel_reason);
    return 0;
}

void task_group_on_terminal(const task_record_t *terminal_task, uint32_t slot) {
    if (!task_group_ready) return;
    if (slot >= TASK_CAPACITY) return;

    uint32_t group_id = task_group_ids[slot];
    if (group_id == 0) return;

    task_group_record_t *group = task_group_lookup(group_id);
    if (!group) return;
    if (group->active == 0) return;

    --group->active;

    if (terminal_task->state == TASK_STATE_FAILED) {
        ++group->failures;
        if (group->policy == TASK_GROUP_POLICY_FAIL_FAST) {
            group->state = TASK_GROUP_STATE_FAILING;
            task_group_cancel_remaining(group_id, terminal_task->result);
        }
    } else if (terminal_task->state == TASK_STATE_CANCELLED) {
        if (group->state == TASK_GROUP_STATE_OPEN) {
            group->state = TASK_GROUP_STATE_CANCELLING;
        }
    }

    if (group->active != 0) return;

    if (group->state == TASK_GROUP_STATE_CANCELLING) {
        group->state = TASK_GROUP_STATE_CANCELLED;
    } else if (group->state == TASK_GROUP_STATE_FAILING) {
        group->state = TASK_GROUP_STATE_FAILED;
    } else if (group->failures != 0) {
        group->state = TASK_GROUP_STATE_FAILED;
    } else {
        group->state = TASK_GROUP_STATE_COMPLETED;
    }
    --task_group_count;
}

int task_group_manager_initialize(void) {
    for (uint32_t i = 0; i < TASK_GROUP_CAPACITY; ++i) task_group_table[i] = (task_group_record_t){0};
    for (uint32_t i = 0; i < TASK_CAPACITY; ++i) task_group_ids[i] = 0;
    task_group_count = 0;
    task_group_next_id = 1;
    task_group_ready = 1;
    return 0;
}

int task_group_manager_self_test(void) {
    /* WaitAll wird erst nach beiden erfolgreichen Tasks terminal. */
    uint32_t scope;
    if (task_scope_create(1, task_scope_kernel_root_id, 0, &scope) != 0) return -1;

    uint32_t group;
    if (task_group_create(1, scope, TASK_GROUP_POLICY_WAIT_ALL, &group) != 0) return -1;

    uint32_t task1;
    if (task_create(1, scope, 0, 0, &task1) != 0) return -1;
    if (task_group_attach(group, task1) != 0) return -1;

    uint32_t task2;
    if (task_create(1, scope, 0, 0, &task2) != 0) return -1;
    if (task_group_attach(group, task2) != 0) return -1;

    if (task_complete(task1, 0) != 0) return -1;

    task_group_record_t *grp = task_group_lookup(group);
    if (!grp) return -1;
    if (grp->state != TASK_GROUP_STATE_OPEN) return -1;
    if (grp->active != 1) return -1;

    if (task_complete(task2, 0) != 0) return -1;

    grp = task_group_lookup(group);
    if (!grp) return -1;
    if (grp->state != TASK_GROUP_STATE_COMPLETED) return -1;

    if (task_scope_close(scope) != 0) return -1;

    /* FailFast fordert fuer verbleibende Tasks Cancellation an und
       drainiert. */
    uint32_t scope2;
    if (task_scope_create(1, task_scope_kernel_root_id, 0, &scope2) != 0) return -1;

    uint32_t group2;
    if (task_group_create(1, scope2, TASK_GROUP_POLICY_FAIL_FAST, &group2) != 0) return -1;

    uint32_t task3;
    if (task_create(1, scope2, 0, 0, &task3) != 0) return -1;
    if (task_group_attach(group2, task3) != 0) return -1;

    uint32_t task4;
    if (task_create(1, scope2, 0, 0, &task4) != 0) return -1;
    if (task_group_attach(group2, task4) != 0) return -1;

    if (task_fail(task3, 0x4641494Cu) != 0) return -1;

    task_record_t *rec4 = task_lookup(task4);
    if (!rec4) return -1;
    if (rec4->state != TASK_STATE_CANCEL_REQUEST) return -1;

    int cancelled;
    if (task_checkpoint(task4, &cancelled) != 0) return -1;

    grp = task_group_lookup(group2);
    if (!grp) return -1;
    if (grp->state != TASK_GROUP_STATE_FAILED) return -1;
    if (grp->failures != 1) return -1;

    if (task_scope_close(scope2) != 0) return -1;
    if (task_group_count != 0) return -1;

    return 0;
}
