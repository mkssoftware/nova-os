/* NovaOS C-Kernel - Nova Self-Diagnosis (Port aus adaptive32.inc). */
#include "sdiag.h"

typedef struct {
    uint32_t id; /* 0 = frei */
    uint32_t symptom;
    uint32_t diagnosis;
    uint32_t count;
} sdiag_rec_t;

static sdiag_rec_t sdiag_table[SDIAG_CAPACITY];
static int sdiag_ready = 0;

void sdiag_initialize(void) {
    if (sdiag_ready == 1) return;

    for (uint32_t i = 0; i < SDIAG_CAPACITY; ++i) sdiag_table[i] = (sdiag_rec_t){0};
    sdiag_ready = 1;
}

int sdiag_register(uint32_t diag_id, uint32_t symptom_type, uint32_t diagnosis, uint32_t *out_slot) {
    if (sdiag_ready != 1 || diag_id == 0) return -1;

    for (uint32_t i = 0; i < SDIAG_CAPACITY; ++i) {
        if (sdiag_table[i].id == 0) {
            sdiag_table[i].id = diag_id;
            sdiag_table[i].symptom = symptom_type;
            sdiag_table[i].diagnosis = diagnosis;
            sdiag_table[i].count = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int sdiag_diagnose(uint32_t symptom_type, uint32_t *out_diagnosis, uint32_t *out_count) {
    if (sdiag_ready != 1) return -1;

    for (uint32_t i = 0; i < SDIAG_CAPACITY; ++i) {
        sdiag_rec_t *rec = &sdiag_table[i];
        if (rec->id == 0 || rec->symptom != symptom_type) continue;

        ++rec->count;
        *out_diagnosis = rec->diagnosis;
        *out_count = rec->count;
        return 0;
    }

    *out_diagnosis = SDIAG_DIAG_UNKNOWN;
    *out_count = 0;
    return -1;
}

int sdiag_self_test(void) {
    uint32_t slot;
    if (sdiag_register(1, SDIAG_SYM_MEMORY, SDIAG_DIAG_LEAK, &slot) != 0) return -1;

    uint32_t diagnosis, count;
    if (sdiag_diagnose(SDIAG_SYM_MEMORY, &diagnosis, &count) != 0) return -1;
    if (diagnosis != SDIAG_DIAG_LEAK) return -1;

    if (sdiag_diagnose(99, &diagnosis, &count) == 0) return -1;
    if (diagnosis != SDIAG_DIAG_UNKNOWN) return -1;

    return 0;
}
