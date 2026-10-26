/* NovaOS C-Kernel - Nova Resilience Isolation (Port aus adaptive32.inc). */
#include "resiso.h"

typedef struct {
    uint32_t domain_id; /* 0 = frei */
    uint32_t faults;
    uint32_t quarantined;
} resiso_rec_t;

static resiso_rec_t resiso_table[RESISO_CAPACITY];
static int resiso_ready = 0;

void resilisolation_initialize(void) {
    if (resiso_ready == 1) return;

    for (uint32_t i = 0; i < RESISO_CAPACITY; ++i) resiso_table[i] = (resiso_rec_t){0};
    resiso_ready = 1;
}

int resilisolation_register(uint32_t domain_id, uint32_t *out_slot) {
    if (resiso_ready != 1 || domain_id == 0) return -1;

    for (uint32_t i = 0; i < RESISO_CAPACITY; ++i) {
        if (resiso_table[i].domain_id == 0) {
            resiso_table[i].domain_id = domain_id;
            resiso_table[i].faults = 0;
            resiso_table[i].quarantined = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

static resiso_rec_t *resiso_find(uint32_t domain_id) {
    for (uint32_t i = 0; i < RESISO_CAPACITY; ++i) {
        if (resiso_table[i].domain_id == domain_id) return &resiso_table[i];
    }
    return 0;
}

int resilisolation_record_fault(uint32_t domain_id, uint32_t fault_count_threshold) {
    if (resiso_ready != 1) return -1;

    resiso_rec_t *rec = resiso_find(domain_id);
    if (!rec) return -1;

    ++rec->faults;
    if (rec->quarantined == 1) return -1;

    if (rec->faults < fault_count_threshold) return 0;

    rec->quarantined = 1;
    return -1;
}

int resilisolation_query(uint32_t domain_id, uint32_t *out_fault_count, uint32_t *out_quarantined) {
    if (resiso_ready != 1) return -1;

    resiso_rec_t *rec = resiso_find(domain_id);
    if (!rec) return -1;

    *out_fault_count = rec->faults;
    *out_quarantined = rec->quarantined;
    return 0;
}

int resilisolation_self_test(void) {
    uint32_t slot;
    if (resilisolation_register(1, &slot) != 0) return -1;

    if (resilisolation_record_fault(1, 2) != 0) return -1; /* noch unter Schwelle */
    if (resilisolation_record_fault(1, 2) == 0) return -1; /* Schwelle erreicht: quarantiert */

    uint32_t fault_count, quarantined;
    if (resilisolation_query(1, &fault_count, &quarantined) != 0) return -1;
    if (fault_count != 2 || quarantined != 1) return -1;

    return 0;
}
