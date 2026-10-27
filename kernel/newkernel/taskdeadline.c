/* NovaOS C-Kernel - Nova Task Deadline Manager (Port aus
   concurrency32.inc, siehe taskdeadline.h). */
#include "taskdeadline.h"
#include "task.h"
#include "taskscope.h"
#include "interrupt.h"

static task_deadline_info_t task_deadline_table[TASK_CAPACITY];
static uint32_t task_deadline_miss_count = 0;
static int task_deadline_ready = 0;

void task_deadline_on_slot_reused(uint32_t slot) {
    if (slot >= TASK_CAPACITY) return;
    task_deadline_table[slot] = (task_deadline_info_t){0};
}

int task_deadline_manager_initialize(void) {
    for (uint32_t i = 0; i < TASK_CAPACITY; ++i) task_deadline_table[i] = (task_deadline_info_t){0};
    task_deadline_miss_count = 0;
    task_deadline_ready = 1;
    return 0;
}

int task_deadline_query(uint32_t task_id, task_deadline_info_t *out_info) {
    task_record_t *rec = task_lookup(task_id);
    if (!rec) return -1;
    *out_info = task_deadline_table[task_slot_index(rec)];
    return 0;
}

int task_deadline_set(uint32_t task_id, uint32_t absolute_tick, uint32_t deadline_class,
                       uint32_t miss_policy, uint32_t *out_effective_tick) {
    if (deadline_class < TASK_DEADLINE_CLASS_HARD || deadline_class > TASK_DEADLINE_CLASS_SOFT) return -1;
    if (miss_policy < TASK_DEADLINE_POLICY_CONTINUE || miss_policy > TASK_DEADLINE_POLICY_FAIL) return -1;
    if (absolute_tick == 0) return -1;

    task_record_t *rec = task_lookup(task_id);
    if (!rec) return -1;
    if (rec->state < TASK_STATE_CREATED || rec->state > TASK_STATE_WAITING) return -1;

    uint32_t effective_tick = absolute_tick;
    uint32_t effective_class = deadline_class;

    if (rec->parent != 0) {
        task_record_t *parent = task_lookup(rec->parent);
        if (parent) {
            task_deadline_info_t *parent_deadline = &task_deadline_table[task_slot_index(parent)];
            if (parent_deadline->state == TASK_DEADLINE_STATE_ARMED) {
                if (effective_tick > parent_deadline->absolute_tick) {
                    effective_tick = parent_deadline->absolute_tick; /* niemals verlaengern */
                }
                if (parent_deadline->deadline_class == TASK_DEADLINE_CLASS_HARD) {
                    effective_class = TASK_DEADLINE_CLASS_HARD;
                }
            }
        }
    }

    if (effective_class == TASK_DEADLINE_CLASS_HARD) {
        if (miss_policy == TASK_DEADLINE_POLICY_CONTINUE) return -1; /* Hard Miss darf nicht still ignoriert werden */
        if (effective_tick <= timer_get_ticks()) return -1; /* minimale Bootstrap-Admission-Control */
    }

    task_deadline_info_t *slot = &task_deadline_table[task_slot_index(rec)];
    slot->absolute_tick = effective_tick;
    slot->deadline_class = effective_class;
    slot->policy = miss_policy;
    slot->state = TASK_DEADLINE_STATE_ARMED;

    *out_effective_tick = effective_tick;
    return 0;
}

void task_deadline_poll(void) {
    if (!task_deadline_ready) return;

    uint32_t current_tick = timer_get_ticks();

    for (uint32_t i = 0; i < TASK_CAPACITY; ++i) {
        task_record_t *rec = task_at_slot(i);
        if (rec->state < TASK_STATE_CREATED || rec->state >= TASK_STATE_CANCEL_REQUEST) continue;

        task_deadline_info_t *deadline = &task_deadline_table[i];
        if (deadline->state != TASK_DEADLINE_STATE_ARMED) continue;
        if (current_tick < deadline->absolute_tick) continue;

        deadline->state = TASK_DEADLINE_STATE_MISSED;
        ++task_deadline_miss_count;

        if (deadline->policy == TASK_DEADLINE_POLICY_CANCEL) {
            task_request_cancel(rec->id, TASK_CANCEL_REASON_DEADLINE);
        } else if (deadline->policy == TASK_DEADLINE_POLICY_FAIL) {
            rec->state = TASK_STATE_FAILED;
            rec->result = TASK_CANCEL_REASON_DEADLINE;
            task_group_on_terminal(rec, i);
            task_release_scope(rec);
        }
    }
}

int task_deadline_manager_self_test(void) {
    /* Parent 100 Ticks, Child fordert 200: effektiv bleiben 100 und Hard. */
    uint32_t scope;
    if (task_scope_create(1, task_scope_kernel_root_id, 0, &scope) != 0) return -1;

    uint32_t parent_task;
    if (task_create(1, scope, 0, 0, &parent_task) != 0) return -1;

    uint32_t parent_tick = timer_get_ticks() + 100;
    uint32_t effective;
    if (task_deadline_set(parent_task, parent_tick, TASK_DEADLINE_CLASS_HARD,
                           TASK_DEADLINE_POLICY_CANCEL, &effective) != 0) return -1;

    uint32_t child_task;
    if (task_create(1, scope, parent_task, 0, &child_task) != 0) return -1;

    uint32_t child_tick = timer_get_ticks() + 200;
    if (task_deadline_set(child_task, child_tick, TASK_DEADLINE_CLASS_SOFT,
                           TASK_DEADLINE_POLICY_CANCEL, &effective) != 0) return -1;

    task_deadline_info_t info;
    if (task_deadline_query(child_task, &info) != 0) return -1;
    if (info.absolute_tick != parent_tick) return -1;
    if (info.deadline_class != TASK_DEADLINE_CLASS_HARD) return -1;

    if (task_complete(child_task, 0) != 0) return -1;
    if (task_complete(parent_task, 0) != 0) return -1;
    if (task_scope_close(scope) != 0) return -1;

    /* Eine bereits erreichte Firm-Deadline fordert kooperative
       Cancellation an. */
    uint32_t scope2;
    if (task_scope_create(1, task_scope_kernel_root_id, 0, &scope2) != 0) return -1;

    uint32_t task_id;
    if (task_create(1, scope2, 0, 0, &task_id) != 0) return -1;

    if (task_deadline_set(task_id, timer_get_ticks(), TASK_DEADLINE_CLASS_FIRM,
                           TASK_DEADLINE_POLICY_CANCEL, &effective) != 0) return -1;

    task_deadline_poll();

    task_record_t *rec = task_lookup(task_id);
    if (!rec) return -1;
    if (rec->state != TASK_STATE_CANCEL_REQUEST) return -1;
    if (rec->cancel_reason != TASK_CANCEL_REASON_DEADLINE) return -1;

    if (task_deadline_query(task_id, &info) != 0) return -1;
    if (info.state != TASK_DEADLINE_STATE_MISSED) return -1;
    if (task_deadline_miss_count != 1) return -1;

    int cancelled;
    if (task_checkpoint(task_id, &cancelled) != 0) return -1;
    if (task_scope_close(scope2) != 0) return -1;

    return 0;
}
