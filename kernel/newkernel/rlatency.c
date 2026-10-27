/* NovaOS C-Kernel - Nova Resource Latency Monitor (Port aus adaptive32.inc). */
#include "rlatency.h"
#include "apred.h"
#include "sdiag.h"

typedef struct {
    uint32_t endpoint_id; /* 0 = frei */
    uint32_t avg_lat;
    uint32_t sla_us;
    uint32_t violations;
} rlatency_rec_t;

#define RLAT_ALPHA 48u

static rlatency_rec_t rlatency_table[RLAT_CAPACITY];
static int rlatency_ready = 0;

void rlatency_initialize(void) {
    if (rlatency_ready == 1) return;

    for (uint32_t i = 0; i < RLAT_CAPACITY; ++i) rlatency_table[i] = (rlatency_rec_t){0};
    rlatency_ready = 1;
}

int rlatency_register(uint32_t endpoint_id, uint32_t sla_us) {
    if (rlatency_ready != 1 || endpoint_id == 0) return -1;

    for (uint32_t i = 0; i < RLAT_CAPACITY; ++i) {
        if (rlatency_table[i].endpoint_id == 0) {
            rlatency_table[i].endpoint_id = endpoint_id;
            rlatency_table[i].avg_lat = 0;
            rlatency_table[i].sla_us = sla_us;
            rlatency_table[i].violations = 0;
            return 0;
        }
    }
    return -1;
}

int rlatency_observe(uint32_t endpoint_id, uint32_t observed_us) {
    if (rlatency_ready != 1) return -1;

    for (uint32_t i = 0; i < RLAT_CAPACITY; ++i) {
        rlatency_rec_t *rec = &rlatency_table[i];
        if (rec->endpoint_id != endpoint_id) continue;

        uint32_t weighted = RLAT_ALPHA * observed_us + (256u - RLAT_ALPHA) * rec->avg_lat;
        rec->avg_lat = weighted >> 8;

        uint32_t new_prediction;
        apred_update(endpoint_id, rec->avg_lat, &new_prediction);

        if (rec->avg_lat <= rec->sla_us) return 0;

        ++rec->violations;
        uint32_t diagnosis, count;
        sdiag_diagnose(SDIAG_SYM_LATENCY, &diagnosis, &count);
        return -1;
    }
    return -1;
}

int rlatency_query(uint32_t endpoint_id, uint32_t *out_avg_lat, uint32_t *out_sla_us, uint32_t *out_violations) {
    if (rlatency_ready != 1) return -1;

    for (uint32_t i = 0; i < RLAT_CAPACITY; ++i) {
        if (rlatency_table[i].endpoint_id == endpoint_id) {
            *out_avg_lat = rlatency_table[i].avg_lat;
            *out_sla_us = rlatency_table[i].sla_us;
            *out_violations = rlatency_table[i].violations;
            return 0;
        }
    }
    return -1;
}

int rlatency_self_test(void) {
    if (rlatency_register(1, 100) != 0) return -1;

    if (rlatency_observe(1, 50) != 0) return -1;
    if (rlatency_observe(1, 500) == 0) return -1;

    uint32_t avg_lat, sla_us, violations;
    if (rlatency_query(1, &avg_lat, &sla_us, &violations) != 0) return -1;
    if (violations == 0) return -1;

    return 0;
}
