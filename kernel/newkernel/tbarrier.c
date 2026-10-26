/* NovaOS C-Kernel - Nova Transaction Barrier (Port aus adaptive32.inc). */
#include "tbarrier.h"

#define TBARRIER_CLOSED 0u
#define TBARRIER_OPEN   1u

typedef struct {
    uint32_t id; /* 0 = frei */
    uint32_t total;
    uint32_t arrived;
    uint32_t state;
} tbarrier_rec_t;

static tbarrier_rec_t tbarrier_table[TBARRIER_CAPACITY];
static int tbarrier_ready = 0;

void tbarrier_initialize(void) {
    if (tbarrier_ready == 1) return;

    for (uint32_t i = 0; i < TBARRIER_CAPACITY; ++i) tbarrier_table[i] = (tbarrier_rec_t){0};
    tbarrier_ready = 1;
}

int tbarrier_create(uint32_t barrier_id, uint32_t total_participants, uint32_t *out_slot) {
    if (tbarrier_ready != 1 || barrier_id == 0 || total_participants == 0) return -1;

    for (uint32_t i = 0; i < TBARRIER_CAPACITY; ++i) {
        if (tbarrier_table[i].id == 0) {
            tbarrier_table[i].id = barrier_id;
            tbarrier_table[i].total = total_participants;
            tbarrier_table[i].arrived = 0;
            tbarrier_table[i].state = TBARRIER_CLOSED;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int tbarrier_signal(uint32_t barrier_id) {
    if (tbarrier_ready != 1 || barrier_id == 0) return -1;

    for (uint32_t i = 0; i < TBARRIER_CAPACITY; ++i) {
        tbarrier_rec_t *rec = &tbarrier_table[i];
        if (rec->id != barrier_id) continue;

        ++rec->arrived;
        if (rec->arrived != rec->total) return -1;

        rec->state = TBARRIER_OPEN;
        return 0;
    }
    return -1;
}

int tbarrier_self_test(void) {
    uint32_t slot;
    if (tbarrier_create(1, 2, &slot) != 0) return -1;

    if (tbarrier_signal(1) == 0) return -1; /* noch geschlossen */
    if (tbarrier_signal(1) != 0) return -1; /* jetzt offen */

    if (tbarrier_table[slot].state != TBARRIER_OPEN) return -1;

    return 0;
}
