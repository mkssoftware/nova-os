/* NovaOS C-Kernel - Nova Prediction Error Tracker (Port aus
   adaptive32.inc). */
#include "prederr.h"
#include "apred.h"
#include "pollearn.h"

typedef struct {
    uint32_t metric_id; /* 0 = frei */
    uint32_t last_error;
    uint32_t avg_error;
    uint32_t samples;
} prederr_rec_t;

static prederr_rec_t prederr_table[PREDERR_CAPACITY];
static int prederr_ready = 0;

void prederr_initialize(void) {
    if (prederr_ready == 1) return;

    for (uint32_t i = 0; i < PREDERR_CAPACITY; ++i) prederr_table[i] = (prederr_rec_t){0};
    prederr_ready = 1;
}

int prederr_register(uint32_t metric_id) {
    if (prederr_ready != 1 || metric_id == 0) return -1;

    for (uint32_t i = 0; i < PREDERR_CAPACITY; ++i) {
        if (prederr_table[i].metric_id == 0) {
            prederr_table[i].metric_id = metric_id;
            prederr_table[i].last_error = 0;
            prederr_table[i].avg_error = 0;
            prederr_table[i].samples = 0;
            return 0;
        }
    }
    return -1;
}

int prederr_observe(uint32_t metric_id, uint32_t observed) {
    if (prederr_ready != 1) return -1;

    uint32_t prediction, pred_samples;
    if (apred_query(metric_id, &prediction, &pred_samples) != 0) return -1;

    uint32_t error = (prediction > observed) ? (prediction - observed) : (observed - prediction);

    for (uint32_t i = 0; i < PREDERR_CAPACITY; ++i) {
        prederr_rec_t *rec = &prederr_table[i];
        if (rec->metric_id != metric_id) continue;

        rec->last_error = error;
        ++rec->samples;
        rec->avg_error = (PREDERR_ALPHA * error + (256u - PREDERR_ALPHA) * rec->avg_error) >> 8;

        if (rec->avg_error > PREDERR_THRESHOLD) {
            pollearn_feedback(metric_id, 0);
        }
        return 0;
    }
    return -1;
}

int prederr_query(uint32_t metric_id, uint32_t *out_avg_error, uint32_t *out_last_error,
                   uint32_t *out_samples) {
    if (prederr_ready != 1) return -1;

    for (uint32_t i = 0; i < PREDERR_CAPACITY; ++i) {
        if (prederr_table[i].metric_id == metric_id) {
            *out_avg_error = prederr_table[i].avg_error;
            *out_last_error = prederr_table[i].last_error;
            *out_samples = prederr_table[i].samples;
            return 0;
        }
    }
    return -1;
}

int prederr_self_test(void) {
    if (prederr_register(1) != 0) return -1;

    /* Falls apred metric_id=1 noch nicht kennt: eigene Registrierung.
       Rueckgabewert bewusst ignoriert (1:1 wie im alten ASM) - apred.c's
       eigener Selbsttest koennte metric_id=1 bereits belegt haben. */
    uint32_t apred_slot;
    apred_register(1, 50, 64, &apred_slot);

    if (prederr_observe(1, 50) != 0) return -1;

    uint32_t avg_error, last_error, samples;
    if (prederr_query(1, &avg_error, &last_error, &samples) != 0) return -1;
    if (samples != 1) return -1;

    return 0;
}
