/* NovaOS C-Kernel - Nova Thread Manager (Port aus sync32.inc, siehe
   threadmgr.h). */
#include "threadmgr.h"
#include "scheduler.h"
#include "task.h"
#include "taskscope.h"
#include "process.h"
#include "object.h"

#define OBJECT_TYPE_THREAD 5u

extern void kmain(uint32_t boot_magic, uint32_t bib_physical_address) __asm__("kmain");

static thread_record_t thread_table[THREAD_CAPACITY];
static uint32_t thread_task_ids[THREAD_CAPACITY]; /* Slot -> Task-ID, 0 = keine */
static uint32_t thread_count = 0;
static uint32_t thread_next_tid = 1;
static int thread_manager_ready = 0;

thread_record_t *thread_lookup(uint32_t tid) {
    for (uint32_t i = 0; i < THREAD_CAPACITY; ++i) {
        if (thread_table[i].tid == tid) return &thread_table[i];
    }
    return 0;
}

int thread_register(uint32_t entry_point, uint32_t pid, uint32_t slot, uint32_t *out_tid) {
    if (entry_point == 0) return -1;
    if (slot >= THREAD_CAPACITY) return -1;
    if (process_lookup(pid) == 0) return -1;
    if (thread_count >= THREAD_CAPACITY) return -1;
    if (pid != 1) return -1; /* nur der Kernelprozess darf Threads registrieren */

    uint32_t task_id;
    if (task_create(pid, task_scope_kernel_root_id, 0, 0, &task_id) != 0) return -1;

    uint32_t handle = object_create(OBJECT_TYPE_THREAD, pid, entry_point);
    if (handle == 0) return -1;

    thread_record_t *rec = &thread_table[thread_count];
    rec->tid = thread_next_tid;
    rec->pid = pid;
    rec->state = THREAD_STATE_READY;
    rec->slot = slot;
    rec->handle = handle;
    rec->entry = entry_point;
    rec->context = scheduler_context_at(slot);
    rec->scope = (pid == 1) ? task_scope_kernel_root_id : 0;

    thread_task_ids[slot] = task_id;
    ++thread_count;
    ++thread_next_tid;
    *out_tid = rec->tid;
    return 0;
}

int thread_create_dynamic(uint32_t entry_point, uint32_t pid, int32_t cpu_affinity, uint32_t *out_tid) {
    if (!thread_manager_ready) return -1;
    if (entry_point == 0) return -1;
    if (process_lookup(pid) == 0) return -1;

    uint32_t slot;
    if (scheduler_claim_dynamic_slot(entry_point, cpu_affinity, &slot) != 0) return -1;

    uint32_t tid;
    if (thread_register(entry_point, pid, slot, &tid) != 0) {
        scheduler_release_dynamic_slot(slot);
        return -1;
    }

    *out_tid = tid;
    return 0;
}

int thread_manager_initialize(void) {
    for (uint32_t i = 0; i < THREAD_CAPACITY; ++i) thread_table[i] = (thread_record_t){0};
    for (uint32_t i = 0; i < THREAD_CAPACITY; ++i) thread_task_ids[i] = 0;
    thread_count = 0;
    thread_next_tid = 1;
    thread_manager_ready = 0;

    uint32_t tid;
    if (thread_register((uint32_t)kmain, 1, 0, &tid) != 0) return -1;
    if (thread_register((uint32_t)scheduler_thread1, 1, 1, &tid) != 0) return -1;
    if (thread_register((uint32_t)scheduler_thread2, 1, 2, &tid) != 0) return -1;

    thread_manager_ready = 1;
    return 0;
}

int thread_manager_self_test(void) {
    thread_record_t *rec = thread_lookup(2);
    if (!rec) return -1;
    if (rec->pid != 1) return -1;
    if (rec->slot != 1) return -1;
    if (rec->entry != (uint32_t)scheduler_thread1) return -1;
    if (rec->scope != task_scope_kernel_root_id) return -1;

    uint32_t task_id = thread_task_ids[1];
    if (task_id == 0) return -1;
    task_record_t *task = task_lookup(task_id);
    if (!task) return -1;
    if (task->owner != 1) return -1;
    if (task->scope != task_scope_kernel_root_id) return -1;

    /* Laufzeit: einen zusaetzlichen Thread in einen freien Scheduler-
       Slot legen. CPU-ungebunden (-1), damit er von jeder CPU
       uebernommen werden kann, sobald der Scheduler ihn auswaehlt. */
    uint32_t dynamic_tid;
    if (thread_create_dynamic((uint32_t)scheduler_dynamic_thread, 1, -1, &dynamic_tid) != 0) return -1;

    thread_record_t *dyn = thread_lookup(dynamic_tid);
    if (!dyn) return -1;
    if (dyn->slot != 3) return -1;
    if (dyn->entry != (uint32_t)scheduler_dynamic_thread) return -1;
    if (scheduler_affinity_at(3) != -1) return -1;
    if (scheduler_running_on_at(3) != -1) return -1;

    if (task_count() != 4) return -1;
    if (thread_count != 4) return -1;

    return 0;
}
