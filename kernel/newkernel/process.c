/* NovaOS C-Kernel - Kernel Process Manager (Port aus process32.inc). */
#include "process.h"
#include "object.h"
#include <stddef.h>

#define PROCESS_STATE_EMPTY   0u
#define PROCESS_STATE_RUNNING 1u

typedef struct {
    uint32_t pid;
    uint32_t state;
    uint32_t cr3;
    uint32_t handle;
    uint32_t parent;
    uint32_t flags;
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

    ++process_count_value;
    ++process_next_pid;
    return pid;
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

    return 0;
}
