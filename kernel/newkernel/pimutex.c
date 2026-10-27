/* NovaOS C-Kernel - Nova Priority Inheritance Mutex (Port aus sync32.inc). */
#include "pimutex.h"

#define PI_STATE_FREE     0u
#define PI_STATE_UNLOCKED 1u
#define PI_STATE_LOCKED   2u

typedef struct {
    uint32_t id;
    uint32_t state;
    uint32_t owner;
    uint32_t base_prio;
    uint32_t eff_prio;
    uint32_t waiter_count;
} pi_rec_t;

static pi_rec_t pi_table[PI_CAPACITY];
static int pi_ready = 0;
static uint32_t pi_next_id = 0;
static uint32_t pi_total_inherit = 0;

void pi_initialize(void) {
    for (uint32_t i = 0; i < PI_CAPACITY; ++i) pi_table[i] = (pi_rec_t){0};
    pi_ready = 1;
    pi_next_id = 0;
    pi_total_inherit = 0;
}

int pi_mutex_create(uint32_t *out_slot) {
    for (uint32_t i = 0; i < PI_CAPACITY; ++i) {
        pi_rec_t *rec = &pi_table[i];
        if (rec->state == PI_STATE_FREE) {
            rec->id = pi_next_id;
            ++pi_next_id;
            rec->state = PI_STATE_UNLOCKED;
            rec->owner = 0;
            rec->base_prio = 0;
            rec->eff_prio = 0;
            rec->waiter_count = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int pi_mutex_lock(uint32_t slot, uint32_t task_id, uint32_t task_base_prio) {
    if (slot >= PI_CAPACITY) return -1;
    pi_rec_t *rec = &pi_table[slot];

    if (rec->state == PI_STATE_UNLOCKED) {
        rec->state = PI_STATE_LOCKED;
        rec->owner = task_id;
        rec->base_prio = task_base_prio;
        rec->eff_prio = task_base_prio;
        return 0;
    }

    /* Bereits gesperrt: Priority Inheritance - ein hoeher-prioritaerer
       Wartender hebt die effektive Prioritaet des Besitzers an. */
    ++rec->waiter_count;
    if (task_base_prio > rec->eff_prio) {
        rec->eff_prio = task_base_prio;
        ++pi_total_inherit;
    }
    return -1;
}

int pi_mutex_unlock(uint32_t slot) {
    if (slot >= PI_CAPACITY) return -1;
    pi_rec_t *rec = &pi_table[slot];
    if (rec->state != PI_STATE_LOCKED) return -1;

    rec->state = PI_STATE_UNLOCKED;
    rec->owner = 0;
    rec->eff_prio = rec->base_prio;
    rec->waiter_count = 0;
    return 0;
}

int pi_mutex_query(uint32_t slot, uint32_t *out_eff_prio, uint32_t *out_owner, uint32_t *out_waiters) {
    if (slot >= PI_CAPACITY) return -1;
    pi_rec_t *rec = &pi_table[slot];
    if (rec->state == PI_STATE_FREE) return -1;

    *out_eff_prio = rec->eff_prio;
    *out_owner = rec->owner;
    *out_waiters = rec->waiter_count;
    return 0;
}

int pi_self_test(void) {
    if (!pi_ready) return -1;

    uint32_t slot;
    if (pi_mutex_create(&slot) != 0) return -1;

    if (pi_mutex_lock(slot, 1, 5) != 0) return -1;
    if (pi_mutex_lock(slot, 2, 10) == 0) return -1; /* bereits gesperrt */

    uint32_t eff_prio, owner, waiters;
    if (pi_mutex_query(slot, &eff_prio, &owner, &waiters) != 0) return -1;
    if (eff_prio != 10) return -1;
    if (pi_total_inherit != 1) return -1;

    if (pi_mutex_unlock(slot) != 0) return -1;
    if (pi_mutex_lock(slot, 3, 7) != 0) return -1; /* wieder lock-bar */

    return 0;
}
