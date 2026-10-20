/* NovaOS C-Kernel - Copy-on-Write-Tracking (Port aus memory32.inc). */
#include "cow.h"

#define COW_STATE_FREE   0u
#define COW_STATE_SHARED 1u
#define COW_STATE_COPIED 2u

typedef struct {
    uint32_t id;
    uint32_t state;
    uint32_t phys_addr;
    uint32_t refcount;
} cow_record_t;

static cow_record_t cow_table[COW_CAPACITY];
static uint32_t cow_ready = 0;
static uint32_t cow_next_id = 0;
static uint32_t cow_copies = 0;

void cow_initialize(void) {
    for (uint32_t i = 0; i < COW_CAPACITY; ++i) {
        cow_table[i] = (cow_record_t){0};
    }
    cow_ready = 1;
    cow_next_id = 0;
    cow_copies = 0;
}

int cow_register_page(uint32_t phys_addr) {
    for (uint32_t slot = 0; slot < COW_CAPACITY; ++slot) {
        cow_record_t *rec = &cow_table[slot];
        if (rec->state == COW_STATE_FREE) {
            rec->id = cow_next_id++;
            rec->state = COW_STATE_SHARED;
            rec->phys_addr = phys_addr;
            rec->refcount = 2; /* Minimal 2 Referenzen fuer CoW */
            return (int)slot;
        }
    }
    return -1;
}

int cow_on_write(int slot) {
    if (slot < 0 || slot >= COW_CAPACITY) {
        return -1;
    }
    cow_record_t *rec = &cow_table[slot];
    if (rec->state == COW_STATE_FREE) {
        return -1;
    }
    if (rec->refcount <= 1) {
        return 0; /* einziger Referent -> kein CoW noetig */
    }
    --rec->refcount;
    rec->state = COW_STATE_COPIED;
    ++cow_copies;
    return 0;
}

int cow_self_test(void) {
    if (cow_ready != 1) return -1;

    int slot = cow_register_page(0x00100000u);
    if (slot < 0) return -1;

    if (cow_on_write(slot) != 0) return -1;
    if (cow_copies != 1) return -1;
    if (cow_table[slot].state != COW_STATE_COPIED) return -1;

    return 0;
}
