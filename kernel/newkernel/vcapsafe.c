/* NovaOS C-Kernel - Nova Capability Safety Verifier (Port aus
   adaptive32.inc). */
#include "vcapsafe.h"
#include "audit.h"

typedef struct {
    uint32_t cap_id; /* 0 = frei */
    uint32_t proof_token;
    uint32_t uses;
    uint32_t violations;
} vcapsafe_rec_t;

static vcapsafe_rec_t vcapsafe_table[VCAPSAFE_CAPACITY];
static int vcapsafe_ready = 0;

void vcapsafe_initialize(void) {
    if (vcapsafe_ready == 1) return;

    for (uint32_t i = 0; i < VCAPSAFE_CAPACITY; ++i) vcapsafe_table[i] = (vcapsafe_rec_t){0};
    vcapsafe_ready = 1;
}

int vcapsafe_register(uint32_t cap_id, uint32_t proof_token) {
    if (vcapsafe_ready != 1 || cap_id == 0) return -1;

    for (uint32_t i = 0; i < VCAPSAFE_CAPACITY; ++i) {
        if (vcapsafe_table[i].cap_id == 0) {
            vcapsafe_table[i].cap_id = cap_id;
            vcapsafe_table[i].proof_token = proof_token;
            vcapsafe_table[i].uses = 0;
            vcapsafe_table[i].violations = 0;
            return 0;
        }
    }
    return -1;
}

int vcapsafe_verify(uint32_t cap_id, uint32_t supplied_proof) {
    if (vcapsafe_ready != 1) return -1;

    for (uint32_t i = 0; i < VCAPSAFE_CAPACITY; ++i) {
        vcapsafe_rec_t *rec = &vcapsafe_table[i];
        if (rec->cap_id != cap_id) continue;

        ++rec->uses;
        if (rec->proof_token == supplied_proof) return 0;

        ++rec->violations;
        security_audit_log(AUDIT_TYPE_VIOLATION, cap_id, supplied_proof, 0);
        return -1;
    }
    return -1;
}

int vcapsafe_self_test(void) {
    if (vcapsafe_register(1, 0xABCD) != 0) return -1;

    if (vcapsafe_verify(1, 0xABCD) != 0) return -1;
    if (vcapsafe_verify(1, 0x1234) == 0) return -1;

    return 0;
}
