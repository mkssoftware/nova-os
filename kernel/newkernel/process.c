/* NovaOS C-Kernel - Kernel Process Manager (Port aus process32.inc). */
#include "process.h"
#include "object.h"
#include <stddef.h>

typedef struct {
    uint32_t pid;
    uint32_t state;
    uint32_t cr3;
    uint32_t handle;
    uint32_t parent;
    uint32_t flags;
    uint32_t exit_status;
} process_record_t;

static process_record_t process_table[PROCESS_CAPACITY];
static uint32_t process_count_value = 0;
static uint32_t process_next_pid = 1;

void process_manager_initialize(void) {
    for (uint32_t i = 0; i < PROCESS_CAPACITY; ++i) {
        process_table[i] = (process_record_t){0};
    }
    process_count_value = 0;
    process_next_pid = 1;
}

uint32_t process_count(void) {
    return process_count_value;
}

void *process_lookup(uint32_t pid) {
    for (uint32_t i = 0; i < PROCESS_CAPACITY; ++i) {
        process_record_t *rec = &process_table[i];
        if (rec->state == PROCESS_STATE_EMPTY) {
            continue;
        }
        if (rec->pid == pid) {
            return rec;
        }
    }
    return 0;
}

uint32_t process_create(uint32_t flags, uint32_t page_directory_cr3) {
    uint32_t slot;
    for (slot = 0; slot < PROCESS_CAPACITY; ++slot) {
        if (process_table[slot].state == PROCESS_STATE_EMPTY) {
            break;
        }
    }
    if (slot >= PROCESS_CAPACITY) {
        return 0;
    }

    uint32_t pid = process_next_pid;
    uint32_t handle = object_create(OBJECT_TYPE_PROCESS, 0, 0);
    if (handle == 0) {
        return 0;
    }

    process_record_t *rec = &process_table[slot];
    rec->pid = pid;
    rec->state = PROCESS_STATE_RUNNING;
    rec->cr3 = page_directory_cr3;
    rec->handle = handle;
    rec->parent = 0;
    rec->flags = flags;
    rec->exit_status = 0;

    ++process_count_value;
    ++process_next_pid;
    return pid;
}

uint32_t process_get_cr3(uint32_t pid) {
    process_record_t *rec = (process_record_t *)process_lookup(pid);
    return rec ? rec->cr3 : 0u;
}

uint32_t process_find_by_cr3(uint32_t cr3) {
    if (cr3 == 0) {
        return 0u;
    }
    for (uint32_t i = 0; i < PROCESS_CAPACITY; ++i) {
        process_record_t *rec = &process_table[i];
        if (rec->state == PROCESS_STATE_EMPTY) {
            continue;
        }
        if (rec->cr3 == cr3) {
            return rec->pid;
        }
    }
    return 0u;
}

uint32_t process_state(uint32_t pid) {
    process_record_t *rec = (process_record_t *)process_lookup(pid);
    return rec ? rec->state : PROCESS_STATE_EMPTY;
}

int process_suspend(uint32_t pid) {
    process_record_t *rec = (process_record_t *)process_lookup(pid);
    if (!rec || rec->state != PROCESS_STATE_RUNNING) {
        return -1;
    }
    rec->state = PROCESS_STATE_SUSPENDED;
    return 0;
}

int process_resume(uint32_t pid) {
    process_record_t *rec = (process_record_t *)process_lookup(pid);
    if (!rec || rec->state != PROCESS_STATE_SUSPENDED) {
        return -1;
    }
    rec->state = PROCESS_STATE_RUNNING;
    return 0;
}

int process_terminate(uint32_t pid, uint32_t exit_status) {
    process_record_t *rec = (process_record_t *)process_lookup(pid);
    if (!rec) {
        return -1;
    }
    if (rec->state != PROCESS_STATE_RUNNING && rec->state != PROCESS_STATE_SUSPENDED) {
        return -1;
    }
    rec->state = PROCESS_STATE_TERMINATED;
    rec->exit_status = exit_status;
    return 0;
}

uint32_t process_exit_status(uint32_t pid) {
    process_record_t *rec = (process_record_t *)process_lookup(pid);
    if (!rec || rec->state != PROCESS_STATE_TERMINATED) {
        return 0u;
    }
    return rec->exit_status;
}

int process_reap(uint32_t pid) {
    process_record_t *rec = (process_record_t *)process_lookup(pid);
    if (!rec || rec->state != PROCESS_STATE_TERMINATED) {
        return -1;
    }
    *rec = (process_record_t){0};
    --process_count_value;
    return 0;
}

static inline uint32_t read_cr3(void) {
    uint32_t value;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(value));
    return value;
}

int process_manager_self_test(void) {
    uint32_t cr3 = read_cr3();
    uint32_t pid = process_create(1 /* Kernel/System-Prozess */, cr3);
    if (pid != 1) return -1;

    process_record_t *rec = (process_record_t *)process_lookup(pid);
    if (!rec) return -1;
    if (rec->state != PROCESS_STATE_RUNNING) return -1;
    if (rec->cr3 != cr3) return -1;
    if (process_count_value != 1) return -1;

    /* NPSPEC-PROZESS-LIFECYCLE-0001: vollen Lebenszyklus an einem
       EIGENEN Testprozess beweisen (nicht am obigen dauerhaften PID 1,
       der von anderen Selbsttests weiterverwendet wird). */
    uint32_t test_pid = process_create(0, cr3);
    if (test_pid == 0) return -1;
    if (process_state(test_pid) != PROCESS_STATE_RUNNING) return -1;

    if (process_suspend(test_pid) != 0) return -1;
    if (process_state(test_pid) != PROCESS_STATE_SUSPENDED) return -1;
    if (process_suspend(test_pid) == 0) return -1; /* bereits suspendiert */

    if (process_resume(test_pid) != 0) return -1;
    if (process_state(test_pid) != PROCESS_STATE_RUNNING) return -1;

    if (process_terminate(test_pid, 0x2A) != 0) return -1;
    if (process_state(test_pid) != PROCESS_STATE_TERMINATED) return -1;
    if (process_exit_status(test_pid) != 0x2A) return -1;
    if (process_terminate(test_pid, 0) == 0) return -1; /* schon beendet */

    uint32_t count_before_reap = process_count_value;
    if (process_reap(test_pid) != 0) return -1;
    if (process_count_value != count_before_reap - 1u) return -1;
    if (process_lookup(test_pid) != 0) return -1; /* Slot wirklich frei */
    if (process_exit_status(test_pid) != 0) return -1; /* mit Slot verschwunden */

    return 0;
}
