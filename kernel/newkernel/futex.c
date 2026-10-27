/* NovaOS C-Kernel - Nova Fast Userspace Mutex (Port aus sync32.inc). */
#include "futex.h"

#define FTX_STATE_FREE   0u
#define FTX_STATE_ACTIVE 1u

typedef struct {
    uint32_t addr;
    uint32_t waiter_count;
    uint32_t flags;
    uint32_t wake_count;
    uint32_t state; /* 0 = frei */
} futex_rec_t;

static futex_rec_t futex_table[FUTEX_CAPACITY];
static uint32_t futex_next_id = 0;
static uint32_t futex_total_waits = 0;
static uint32_t futex_total_wakes = 0;
static int futex_ready = 0;

void futex_initialize(void) {
    for (uint32_t i = 0; i < FUTEX_CAPACITY; ++i) futex_table[i] = (futex_rec_t){0};
    futex_next_id = 0;
    futex_total_waits = 0;
    futex_total_wakes = 0;
    futex_ready = 1;
}

int futex_register(uint32_t futex_addr, uint32_t flags, uint32_t *out_slot) {
    for (uint32_t i = 0; i < FUTEX_CAPACITY; ++i) {
        futex_rec_t *rec = &futex_table[i];
        if (rec->state == FTX_STATE_FREE) {
            rec->addr = futex_addr;
            rec->flags = flags;
            rec->waiter_count = 0;
            rec->wake_count = 0;
            rec->state = FTX_STATE_ACTIVE;
            ++futex_next_id;
            *out_slot = i;
            return 0;
        }
        if (rec->addr == futex_addr) {
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int futex_wait(uint32_t slot) {
    if (slot >= FUTEX_CAPACITY) return -1;
    futex_rec_t *rec = &futex_table[slot];
    if (rec->state != FTX_STATE_ACTIVE) return -1;

    ++rec->waiter_count;
    ++futex_total_waits;
    return 0;
}

int futex_wake(uint32_t slot, uint32_t max_wake) {
    if (slot >= FUTEX_CAPACITY) return 0;
    futex_rec_t *rec = &futex_table[slot];
    if (rec->state != FTX_STATE_ACTIVE) return 0;

    while (max_wake != 0 && rec->waiter_count != 0) {
        --rec->waiter_count;
        ++rec->wake_count;
        ++futex_total_wakes;
        --max_wake;
    }
    return 0;
}

int futex_self_test(void) {
    if (!futex_ready) return -1;

    uint32_t slot;
    if (futex_register(0x1000u, FTX_FLAG_PRIVATE, &slot) != 0) return -1;

    if (futex_wait(slot) != 0) return -1;
    if (futex_wait(slot) != 0) return -1;
    if (futex_table[slot].waiter_count != 2) return -1;

    if (futex_wake(slot, 1) != 0) return -1;
    if (futex_table[slot].waiter_count != 1) return -1;

    if (futex_wake(slot, 99) != 0) return -1;
    if (futex_table[slot].waiter_count != 0) return -1;

    if (futex_total_waits != 2) return -1;
    if (futex_total_wakes != 2) return -1;

    return 0;
}
