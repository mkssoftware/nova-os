/* NovaOS C-Kernel - Nova Memory Safety Verifier (Port aus adaptive32.inc). */
#include "vmemsafe.h"
#include "audit.h"

typedef struct {
    uint32_t region_id; /* 0 = frei */
    uint32_t base;
    uint32_t size;
    uint32_t violations;
} vmemsafe_rec_t;

static vmemsafe_rec_t vmemsafe_table[VMEMSAFE_CAPACITY];
static int vmemsafe_ready = 0;

void vmemsafe_initialize(void) {
    if (vmemsafe_ready == 1) return;

    for (uint32_t i = 0; i < VMEMSAFE_CAPACITY; ++i) vmemsafe_table[i] = (vmemsafe_rec_t){0};
    vmemsafe_ready = 1;
}

int vmemsafe_register(uint32_t region_id, uint32_t base, uint32_t size) {
    if (vmemsafe_ready != 1 || region_id == 0) return -1;

    for (uint32_t i = 0; i < VMEMSAFE_CAPACITY; ++i) {
        if (vmemsafe_table[i].region_id == 0) {
            vmemsafe_table[i].region_id = region_id;
            vmemsafe_table[i].base = base;
            vmemsafe_table[i].size = size;
            vmemsafe_table[i].violations = 0;
            return 0;
        }
    }
    return -1;
}

int vmemsafe_check(uint32_t region_id, uint32_t addr) {
    if (vmemsafe_ready != 1) return -1;

    for (uint32_t i = 0; i < VMEMSAFE_CAPACITY; ++i) {
        vmemsafe_rec_t *rec = &vmemsafe_table[i];
        if (rec->region_id != region_id) continue;

        if (addr >= rec->base && addr < rec->base + rec->size) return 0;

        ++rec->violations;
        security_audit_log(AUDIT_TYPE_VIOLATION, region_id, addr, 0);
        return -1;
    }
    return -1;
}

int vmemsafe_self_test(void) {
    if (vmemsafe_register(1, 0x1000, 0x100) != 0) return -1;

    if (vmemsafe_check(1, 0x1050) != 0) return -1;
    if (vmemsafe_check(1, 0x2000) == 0) return -1;

    return 0;
}
