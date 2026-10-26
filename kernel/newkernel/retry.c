/* NovaOS C-Kernel - Nova Retry Manager mit Exponential Backoff (Port aus
   adaptive32.inc). */
#include "retry.h"

typedef struct {
    uint32_t id; /* 1-basiert: 0 = frei */
    uint32_t attempts;
    uint32_t max_attempts;
    uint32_t backoff;
    uint32_t base_delay;
} retry_rec_t;

static retry_rec_t retry_table[RETRY_CAPACITY];
static int retry_ready = 0;
static uint32_t retry_next_id = 1;
static uint32_t retry_total_retries = 0;
static uint32_t retry_exhausted = 0;

void retry_initialize(void) {
    for (uint32_t i = 0; i < RETRY_CAPACITY; ++i) retry_table[i] = (retry_rec_t){0};
    retry_next_id = 1;
    retry_total_retries = 0;
    retry_exhausted = 0;
    retry_ready = 1;
}

int retry_register(uint32_t max_attempts, uint32_t base_delay_ticks, uint32_t *out_slot) {
    for (uint32_t i = 0; i < RETRY_CAPACITY; ++i) {
        if (retry_table[i].id == 0) {
            retry_table[i].attempts = 0;
            retry_table[i].max_attempts = max_attempts;
            retry_table[i].base_delay = base_delay_ticks;
            retry_table[i].backoff = base_delay_ticks;
            retry_table[i].id = retry_next_id++;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int retry_attempt(uint32_t slot) {
    if (slot >= RETRY_CAPACITY) return -1;

    retry_rec_t *rec = &retry_table[slot];
    if (rec->id == 0) return -1;

    if (rec->attempts >= rec->max_attempts) {
        ++retry_exhausted;
        return -1;
    }

    ++rec->attempts;
    ++retry_total_retries;

    if (rec->backoff & 0x40000000u) {
        rec->backoff = 0x7FFFFFFFu;
    } else {
        rec->backoff <<= 1;
    }

    return 0;
}

int retry_query_backoff(uint32_t slot, uint32_t *out_backoff_ticks, uint32_t *out_attempts_remaining) {
    if (slot >= RETRY_CAPACITY) return -1;

    retry_rec_t *rec = &retry_table[slot];
    *out_backoff_ticks = rec->backoff;
    *out_attempts_remaining = rec->max_attempts - rec->attempts;
    return 0;
}

int retry_self_test(void) {
    uint32_t slot;
    if (retry_register(3, 10, &slot) != 0) return -1;

    if (retry_attempt(slot) != 0) return -1;
    if (retry_attempt(slot) != 0) return -1;
    if (retry_attempt(slot) != 0) return -1;
    if (retry_attempt(slot) == 0) return -1; /* 4. Versuch: erschoepft */

    uint32_t backoff, remaining;
    if (retry_query_backoff(slot, &backoff, &remaining) != 0) return -1;
    if (backoff < 40) return -1; /* 10->20->40->80, mind. 40 */

    return 0;
}
