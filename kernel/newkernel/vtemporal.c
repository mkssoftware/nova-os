/* NovaOS C-Kernel - Nova Temporal Invariant Verifier (Port aus
   adaptive32.inc). */
#include "vtemporal.h"
#include "sdiag.h"

typedef struct {
    uint32_t inv_id; /* 0 = frei */
    uint32_t anchor_tick;
    uint32_t min_gap;
    uint32_t max_gap;
} vtemporal_rec_t;

static vtemporal_rec_t vtemporal_table[VTEMPORAL_CAPACITY];
static int vtemporal_ready = 0;

void vtemporal_initialize(void) {
    if (vtemporal_ready == 1) return;

    for (uint32_t i = 0; i < VTEMPORAL_CAPACITY; ++i) vtemporal_table[i] = (vtemporal_rec_t){0};
    vtemporal_ready = 1;
}

int vtemporal_register(uint32_t inv_id, uint32_t anchor_tick, uint32_t min_gap) {
    if (vtemporal_ready != 1 || inv_id == 0) return -1;

    for (uint32_t i = 0; i < VTEMPORAL_CAPACITY; ++i) {
        if (vtemporal_table[i].inv_id == 0) {
            vtemporal_table[i].inv_id = inv_id;
            vtemporal_table[i].anchor_tick = anchor_tick;
            vtemporal_table[i].min_gap = min_gap;
            vtemporal_table[i].max_gap = min_gap * 2u;
            return 0;
        }
    }
    return -1;
}

int vtemporal_check(uint32_t inv_id, uint32_t event_tick) {
    if (vtemporal_ready != 1) return -1;

    for (uint32_t i = 0; i < VTEMPORAL_CAPACITY; ++i) {
        vtemporal_rec_t *rec = &vtemporal_table[i];
        if (rec->inv_id != inv_id) continue;

        uint32_t gap = event_tick - rec->anchor_tick;
        if (gap >= rec->min_gap && gap <= rec->max_gap) return 0;

        uint32_t diagnosis, count;
        sdiag_diagnose(SDIAG_SYM_LATENCY, &diagnosis, &count);
        return -1;
    }
    return -1;
}

int vtemporal_self_test(void) {
    if (vtemporal_register(1, 100, 10) != 0) return -1;

    if (vtemporal_check(1, 115) != 0) return -1;
    if (vtemporal_check(1, 200) == 0) return -1;

    return 0;
}
