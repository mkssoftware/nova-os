/* NovaOS C-Kernel - Nova Self-Optimization (Port aus adaptive32.inc). */
#include "selfopt.h"
#include "apred.h"

typedef struct {
    uint32_t target_id; /* 0 = frei */
    uint32_t metric_id;
    uint32_t goal;
    uint32_t step;
    uint32_t adjustments;
} selfopt_rec_t;

static selfopt_rec_t selfopt_table[SELFOPT_CAPACITY];
static int selfopt_ready = 0;

void selfopt_initialize(void) {
    if (selfopt_ready == 1) return;

    for (uint32_t i = 0; i < SELFOPT_CAPACITY; ++i) selfopt_table[i] = (selfopt_rec_t){0};
    selfopt_ready = 1;
}

int selfopt_register(uint32_t target_id, uint32_t metric_id, uint32_t goal,
                      uint32_t step, uint32_t *out_slot) {
    if (selfopt_ready != 1 || target_id == 0 || metric_id == 0) return -1;

    for (uint32_t i = 0; i < SELFOPT_CAPACITY; ++i) {
        if (selfopt_table[i].target_id == 0) {
            selfopt_table[i].target_id = target_id;
            selfopt_table[i].metric_id = metric_id;
            selfopt_table[i].goal = goal;
            selfopt_table[i].step = step;
            selfopt_table[i].adjustments = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int selfopt_step(uint32_t target_id, int32_t *out_direction) {
    if (selfopt_ready != 1) return -1;

    for (uint32_t i = 0; i < SELFOPT_CAPACITY; ++i) {
        selfopt_rec_t *rec = &selfopt_table[i];
        if (rec->target_id != target_id) continue;

        uint32_t prediction, samples;
        if (apred_query(rec->metric_id, &prediction, &samples) != 0) {
            *out_direction = 0; /* keine Prediction vorhanden: nichts zu tun */
            return 0;
        }

        if (prediction == rec->goal) {
            *out_direction = 0;
        } else if (prediction < rec->goal) {
            *out_direction = 1;
            ++rec->adjustments;
        } else {
            *out_direction = -1;
            ++rec->adjustments;
        }
        return 0;
    }
    return -1;
}

int selfopt_self_test(void) {
    /* metric_id bewusst eine Zahl, die apred.c's eigener Selbsttest nicht
       registriert (dieser benutzt metric_id=1) - sonst wuerde dieser Test
       beim gemeinsamen Boot auf eine bereits vorhandene Prediction aus
       apred_self_test treffen statt auf den "keine Prediction"-Pfad. */
    uint32_t slot;
    if (selfopt_register(1, 0xFFFFu, 100, 5, &slot) != 0) return -1;

    int32_t direction;
    if (selfopt_step(1, &direction) != 0) return -1; /* keine Prediction fuer metric=0xFFFF */
    if (direction != 0) return -1;

    return 0;
}
