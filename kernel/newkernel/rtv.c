/* NovaOS C-Kernel - Nova Runtime Verification Monitor (Port aus
   adaptive32.inc). */
#include "rtv.h"
#include "audit.h"

typedef struct {
    uint32_t id;
    uint32_t has_entry;
    uint32_t lower_bound;
    uint32_t upper_bound;
    uint32_t observed;
    uint32_t violations;
} rtv_rec_t;

static rtv_rec_t rtv_contracts[RTV_CAPACITY];
static int rtv_ready = 0;
static uint32_t rtv_violations = 0;
static uint32_t rtv_checks = 0;

void rtv_initialize(void) {
    for (uint32_t i = 0; i < RTV_CAPACITY; ++i) rtv_contracts[i] = (rtv_rec_t){0};
    rtv_violations = 0;
    rtv_checks = 0;
    rtv_ready = 1;
}

int rtv_register(uint32_t contract_id, uint32_t lower_bound, uint32_t upper_bound) {
    for (uint32_t i = 0; i < RTV_CAPACITY; ++i) {
        if (rtv_contracts[i].has_entry == 0) {
            rtv_contracts[i].id = contract_id;
            rtv_contracts[i].has_entry = 1;
            rtv_contracts[i].lower_bound = lower_bound;
            rtv_contracts[i].upper_bound = upper_bound;
            rtv_contracts[i].observed = 0;
            rtv_contracts[i].violations = 0;
            return 0;
        }
    }
    return -1;
}

int rtv_check(uint32_t contract_id, uint32_t observed_value) {
    for (uint32_t i = 0; i < RTV_CAPACITY; ++i) {
        rtv_rec_t *rec = &rtv_contracts[i];
        if (rec->has_entry == 0 || rec->id != contract_id) continue;

        rec->observed = observed_value;
        ++rtv_checks;

        if (observed_value < rec->lower_bound || observed_value > rec->upper_bound) {
            ++rec->violations;
            ++rtv_violations;
            security_audit_log(AUDIT_TYPE_VIOLATION, contract_id, 0, 0);
            return -1;
        }
        return 0;
    }
    return -1;
}

int rtv_self_test(void) {
    (void)rtv_ready;

    if (rtv_register(10, 0, 100) != 0) return -1;
    if (rtv_check(10, 50) != 0) return -1;
    if (rtv_check(10, 150) == 0) return -1; /* verletzt */
    if (rtv_violations < 1) return -1;

    return 0;
}
