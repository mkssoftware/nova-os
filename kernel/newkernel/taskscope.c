/* NovaOS C-Kernel - Nova Task Scope Manager (Port aus concurrency32.inc). */
#include "taskscope.h"
#include "process.h"

uint32_t task_scope_kernel_root_id = 0;

static task_scope_record_t task_scope_table[TASK_SCOPE_CAPACITY];
static uint32_t task_scope_count = 0;
static uint32_t task_scope_next_id = 1;

task_scope_record_t *task_scope_lookup(uint32_t scope_id) {
    for (uint32_t i = 0; i < TASK_SCOPE_CAPACITY; ++i) {
        task_scope_record_t *rec = &task_scope_table[i];
        if (rec->state == TASK_SCOPE_STATE_EMPTY) continue;
        if (rec->id == scope_id) return rec;
    }
    return 0;
}

int task_scope_create(uint32_t owner_pid, uint32_t parent_scope, uint32_t flags, uint32_t *out_scope_id) {
    if (process_lookup(owner_pid) == 0) return -1;

    if (parent_scope != 0) {
        task_scope_record_t *parent = task_scope_lookup(parent_scope);
        if (!parent) return -1;
        if (parent->state != TASK_SCOPE_STATE_OPEN) return -1;
        if (parent->owner != owner_pid) return -1;
    }

    task_scope_record_t *slot = 0;
    for (uint32_t i = 0; i < TASK_SCOPE_CAPACITY; ++i) {
        task_scope_record_t *rec = &task_scope_table[i];
        if (rec->state == TASK_SCOPE_STATE_EMPTY || rec->state == TASK_SCOPE_STATE_CLOSED) {
            slot = rec;
            break;
        }
    }
    if (!slot) return -1;

    slot->id = task_scope_next_id;
    slot->owner = owner_pid;
    slot->parent = parent_scope;
    slot->state = TASK_SCOPE_STATE_OPEN;
    slot->children = 0;
    slot->flags = flags;
    slot->cancel_reason = 0;
    slot->active_tasks = 0;

    if (parent_scope != 0) {
        task_scope_record_t *parent = task_scope_lookup(parent_scope);
        if (!parent) return -1;
        ++parent->children;
    }

    ++task_scope_count;
    ++task_scope_next_id;
    *out_scope_id = slot->id;
    return 0;
}

int task_scope_cancel(uint32_t scope_id, uint32_t cancel_reason) {
    task_scope_record_t *rec = task_scope_lookup(scope_id);
    if (!rec) return -1;
    if (rec->state == TASK_SCOPE_STATE_CLOSED) return -1;

    rec->state = TASK_SCOPE_STATE_CANCELLING;
    rec->cancel_reason = cancel_reason;

    /* Bei jeder Runde werden alle direkten Kinder bereits abgebrochener
       Scopes markiert. Die feste Tabellenkapazitaet begrenzt Laufzeit und
       Speicherverbrauch deterministisch. */
    int changed;
    do {
        changed = 0;
        for (uint32_t i = 0; i < TASK_SCOPE_CAPACITY; ++i) {
            task_scope_record_t *child = &task_scope_table[i];
            if (child->state != TASK_SCOPE_STATE_OPEN) continue;
            if (child->parent == 0) continue;

            task_scope_record_t *parent = task_scope_lookup(child->parent);
            if (!parent) return -1;
            if (parent->state != TASK_SCOPE_STATE_CANCELLING) continue;

            child->state = TASK_SCOPE_STATE_CANCELLING;
            child->cancel_reason = parent->cancel_reason;
            changed = 1;
        }
    } while (changed);

    task_manager_cancel_scopes(cancel_reason);
    return 0;
}

int task_scope_close(uint32_t scope_id) {
    task_scope_record_t *rec = task_scope_lookup(scope_id);
    if (!rec) return -1;
    if (rec->state != TASK_SCOPE_STATE_OPEN && rec->state != TASK_SCOPE_STATE_CANCELLING) return -1;
    if (rec->children != 0) return -1;
    if (rec->active_tasks != 0) return -1;

    if (rec->parent != 0) {
        task_scope_record_t *parent = task_scope_lookup(rec->parent);
        if (!parent) return -1;
        if (parent->children == 0) return -1;
        --parent->children;
    }

    rec->state = TASK_SCOPE_STATE_CLOSED;
    --task_scope_count;
    return 0;
}

int task_scope_manager_initialize(void) {
    for (uint32_t i = 0; i < TASK_SCOPE_CAPACITY; ++i) task_scope_table[i] = (task_scope_record_t){0};
    task_scope_count = 0;
    task_scope_next_id = 1;
    task_scope_kernel_root_id = 0;

    /* Der initiale Kernelprozess besitzt einen dauerhaften Root-Scope.
       Jeder spaeter registrierte Kernelthread wird diesem Scope
       zugeordnet. */
    uint32_t root_id;
    if (task_scope_create(1, 0, 0, &root_id) != 0) return -1;
    task_scope_kernel_root_id = root_id;
    return 0;
}

int task_scope_manager_self_test(void) {
    if (task_scope_kernel_root_id != 1) return -1;

    uint32_t parent_scope;
    if (task_scope_create(1, 0, 0x10u, &parent_scope) != 0) return -1;

    uint32_t child_scope;
    if (task_scope_create(1, parent_scope, 0x20u, &child_scope) != 0) return -1;

    if (task_scope_close(parent_scope) == 0) return -1; /* aktives Kind verhindert Scope-Ende */

    if (task_scope_cancel(parent_scope, 0x43414E43u) != 0) return -1; /* "CANC" */

    task_scope_record_t *child = task_scope_lookup(child_scope);
    if (!child) return -1;
    if (child->state != TASK_SCOPE_STATE_CANCELLING) return -1;
    if (child->cancel_reason != 0x43414E43u) return -1;

    if (task_scope_close(child_scope) != 0) return -1;
    if (task_scope_close(parent_scope) != 0) return -1;

    if (task_scope_count != 1) return -1;

    task_scope_record_t *root = task_scope_lookup(task_scope_kernel_root_id);
    if (!root) return -1;
    if (root->state != TASK_SCOPE_STATE_OPEN) return -1;
    if (root->children != 0) return -1;
    if (root->active_tasks != 0) return -1;

    return 0;
}
