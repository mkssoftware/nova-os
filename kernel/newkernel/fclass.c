/* NovaOS C-Kernel - Nova Fault Classification (Port aus adaptive32.inc). */
#include "fclass.h"

typedef struct {
    uint32_t class_id; /* 0 = frei */
    uint32_t severity;
    uint32_t source;
    uint32_t count;
} fclass_rec_t;

static fclass_rec_t fclass_table[FCLASS_CAPACITY];
static int fclass_ready = 0;

void fclass_initialize(void) {
    if (fclass_ready == 1) return;

    for (uint32_t i = 0; i < FCLASS_CAPACITY; ++i) fclass_table[i] = (fclass_rec_t){0};
    fclass_ready = 1;
}

int fclass_register(uint32_t class_id, uint32_t severity, uint32_t source) {
    if (fclass_ready != 1) return -1;
    if (class_id == 0 || class_id > FCLASS_CAPACITY) return -1;
    if (severity > FCLASS_CRITICAL) return -1;
    if (source == 0) return -1;

    fclass_rec_t *rec = &fclass_table[class_id - 1];
    rec->class_id = class_id;
    rec->severity = severity;
    rec->source = source;
    rec->count = 0;
    return 0;
}

int fclass_classify(uint32_t class_id, uint32_t *out_severity, uint32_t *out_source) {
    if (fclass_ready != 1) return -1;
    if (class_id == 0 || class_id > FCLASS_CAPACITY) return -1;

    fclass_rec_t *rec = &fclass_table[class_id - 1];
    if (rec->class_id == 0) return -1;

    ++rec->count;
    *out_severity = rec->severity;
    *out_source = rec->source;
    return 0;
}

int fclass_self_test(void) {
    if (fclass_register(1, FCLASS_HARD, FCLASS_SRC_KERNEL) != 0) return -1;

    uint32_t severity, source;
    if (fclass_classify(1, &severity, &source) != 0) return -1;
    if (severity != FCLASS_HARD || source != FCLASS_SRC_KERNEL) return -1;

    if (fclass_classify(1, &severity, &source) != 0) return -1;
    if (fclass_table[0].count != 2) return -1;

    return 0;
}
