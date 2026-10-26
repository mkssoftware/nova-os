/* NovaOS C-Kernel - Nova Adaptive Prediction (EWMA) (Port aus
   adaptive32.inc). */
#include "apred.h"

typedef struct {
    uint32_t metric_id; /* 0 = frei */
    uint32_t prediction;
    uint32_t alpha;
    uint32_t samples;
} apred_rec_t;

static apred_rec_t apred_table[APRED_CAPACITY];
static int apred_ready = 0;

void apred_initialize(void) {
    if (apred_ready == 1) return;

    for (uint32_t i = 0; i < APRED_CAPACITY; ++i) apred_table[i] = (apred_rec_t){0};
    apred_ready = 1;
}

int apred_register(uint32_t metric_id, uint32_t initial_value, uint32_t alpha, uint32_t *out_slot) {
    if (apred_ready != 1 || metric_id == 0 || alpha == 0) return -1;

    for (uint32_t i = 0; i < APRED_CAPACITY; ++i) {
        if (apred_table[i].metric_id == 0) {
            apred_table[i].metric_id = metric_id;
            apred_table[i].prediction = initial_value;
            apred_table[i].alpha = alpha;
            apred_table[i].samples = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int apred_update(uint32_t metric_id, uint32_t observed, uint32_t *out_new_prediction) {
    if (apred_ready != 1) return -1;

    for (uint32_t i = 0; i < APRED_CAPACITY; ++i) {
        apred_rec_t *rec = &apred_table[i];
        if (rec->metric_id != metric_id) continue;

        uint64_t weighted = (uint64_t)rec->alpha * observed +
                             (uint64_t)(256u - rec->alpha) * rec->prediction;
        rec->prediction = (uint32_t)(weighted >> 8);
        ++rec->samples;
        *out_new_prediction = rec->prediction;
        return 0;
    }
    return -1;
}

int apred_query(uint32_t metric_id, uint32_t *out_prediction, uint32_t *out_samples) {
    if (apred_ready != 1) return -1;

    for (uint32_t i = 0; i < APRED_CAPACITY; ++i) {
        if (apred_table[i].metric_id == metric_id) {
            *out_prediction = apred_table[i].prediction;
            *out_samples = apred_table[i].samples;
            return 0;
        }
    }
    return -1;
}

int apred_self_test(void) {
    uint32_t slot;
    if (apred_register(1, 100, 128, &slot) != 0) return -1;

    uint32_t new_prediction;
    if (apred_update(1, 200, &new_prediction) != 0) return -1;
    if (new_prediction != 150) return -1; /* (128*200+128*100)/256 = 150 */

    uint32_t prediction, samples;
    if (apred_query(1, &prediction, &samples) != 0) return -1;
    if (prediction != 150 || samples != 1) return -1;

    return 0;
}
