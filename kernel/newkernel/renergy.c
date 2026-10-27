/* NovaOS C-Kernel - Nova Resource Energy Manager (Port aus adaptive32.inc). */
#include "renergy.h"
#include "apower.h"

typedef struct {
    uint32_t rail_id; /* 0 = frei */
    uint32_t power_mw;
    uint32_t cap_mw;
    uint32_t violations;
} renergy_rec_t;

#define RENRG_ALPHA 32u

static renergy_rec_t renergy_table[RENRG_CAPACITY];
static int renergy_ready = 0;

void renergy_initialize(void) {
    if (renergy_ready == 1) return;

    for (uint32_t i = 0; i < RENRG_CAPACITY; ++i) renergy_table[i] = (renergy_rec_t){0};
    renergy_ready = 1;
}

int renergy_register(uint32_t rail_id, uint32_t cap_mw) {
    if (renergy_ready != 1 || rail_id == 0) return -1;

    for (uint32_t i = 0; i < RENRG_CAPACITY; ++i) {
        if (renergy_table[i].rail_id == 0) {
            renergy_table[i].rail_id = rail_id;
            renergy_table[i].power_mw = 0;
            renergy_table[i].cap_mw = cap_mw;
            renergy_table[i].violations = 0;
            return 0;
        }
    }
    return -1;
}

int renergy_update(uint32_t rail_id, uint32_t sample_mw) {
    if (renergy_ready != 1) return -1;

    for (uint32_t i = 0; i < RENRG_CAPACITY; ++i) {
        renergy_rec_t *rec = &renergy_table[i];
        if (rec->rail_id != rail_id) continue;

        uint32_t weighted = RENRG_ALPHA * sample_mw + (256u - RENRG_ALPHA) * rec->power_mw;
        rec->power_mw = weighted >> 8;

        if (rec->power_mw <= rec->cap_mw) return 0;

        ++rec->violations;
        apower_update_load(rail_id, 5);
        return -1;
    }
    return -1;
}

int renergy_query(uint32_t rail_id, uint32_t *out_power_mw, uint32_t *out_cap_mw, uint32_t *out_violations) {
    if (renergy_ready != 1) return -1;

    for (uint32_t i = 0; i < RENRG_CAPACITY; ++i) {
        if (renergy_table[i].rail_id == rail_id) {
            *out_power_mw = renergy_table[i].power_mw;
            *out_cap_mw = renergy_table[i].cap_mw;
            *out_violations = renergy_table[i].violations;
            return 0;
        }
    }
    return -1;
}

int renergy_self_test(void) {
    if (renergy_register(1, 100) != 0) return -1;
    apower_register(1); /* ignore return, domain may already exist */

    if (renergy_update(1, 50) != 0) return -1;
    if (renergy_update(1, 800) == 0) return -1; /* EWMA ~130mW > 100mW cap */

    uint32_t power_mw, cap_mw, violations;
    if (renergy_query(1, &power_mw, &cap_mw, &violations) != 0) return -1;
    if (violations == 0) return -1;

    return 0;
}
