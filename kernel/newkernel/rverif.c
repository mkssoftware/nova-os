/* NovaOS C-Kernel - Nova Resilience Verification (Port aus adaptive32.inc). */
#include "rverif.h"
#include "audit.h"

typedef struct {
    uint32_t subsys_id; /* 0 = frei */
    uint32_t expected_state;
    uint32_t observed_state;
    uint32_t checks;
} rverif_rec_t;

static rverif_rec_t rverif_table[RVERIF_CAPACITY];
static int rverif_ready = 0;

void rverif_initialize(void) {
    if (rverif_ready == 1) return;

    for (uint32_t i = 0; i < RVERIF_CAPACITY; ++i) rverif_table[i] = (rverif_rec_t){0};
    rverif_ready = 1;
}

int rverif_register(uint32_t subsys_id, uint32_t expected_state, uint32_t *out_slot) {
    if (rverif_ready != 1) return -1;
    if (subsys_id == 0) return -1;

    for (uint32_t i = 0; i < RVERIF_CAPACITY; ++i) {
        if (rverif_table[i].subsys_id == 0) {
            rverif_table[i].subsys_id = subsys_id;
            rverif_table[i].expected_state = expected_state;
            rverif_table[i].observed_state = RVERIF_UNKNOWN;
            rverif_table[i].checks = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

static rverif_rec_t *rverif_find(uint32_t subsys_id) {
    for (uint32_t i = 0; i < RVERIF_CAPACITY; ++i) {
        if (rverif_table[i].subsys_id == subsys_id) return &rverif_table[i];
    }
    return 0;
}

int rverif_check(uint32_t subsys_id, uint32_t observed_state) {
    if (rverif_ready != 1 || subsys_id == 0) return -1;

    rverif_rec_t *rec = rverif_find(subsys_id);
    if (!rec) return -1;

    ++rec->checks;
    rec->observed_state = observed_state;

    if (observed_state != rec->expected_state) {
        security_audit_log(AUDIT_TYPE_VIOLATION, 0x52564552u /* "RVER" */, 0, 0);
        return -1;
    }
    return 0;
}

int rverif_query(uint32_t subsys_id, uint32_t *out_observed, uint32_t *out_expected) {
    if (rverif_ready != 1 || subsys_id == 0) return -1;

    rverif_rec_t *rec = rverif_find(subsys_id);
    if (!rec) return -1;

    *out_observed = rec->observed_state;
    *out_expected = rec->expected_state;
    return 0;
}

int rverif_self_test(void) {
    uint32_t slot;
    if (rverif_register(1, RVERIF_HEALTHY, &slot) != 0) return -1;

    if (rverif_check(1, RVERIF_HEALTHY) != 0) return -1;

    uint32_t observed, expected;
    if (rverif_query(1, &observed, &expected) != 0) return -1;
    if (observed != RVERIF_HEALTHY || expected != RVERIF_HEALTHY) return -1;

    if (rverif_check(1, RVERIF_FAILED) == 0) return -1; /* Abweichung erwartet */

    return 0;
}
