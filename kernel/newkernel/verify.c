/* NovaOS C-Kernel - Verification-Subsystem (Port aus verify32.inc). */
#include "verify.h"

#define CAP_FLAG_REVOKED 0x01u

typedef struct {
    uint32_t id;
    uint32_t type;
    uint32_t subject_id;
    uint32_t result;
    uint32_t evidence_hash;
    uint32_t timestamp;
    uint32_t flags;
} vfy_record_t;

typedef struct {
    uint32_t subject_id;
    uint32_t precond_hash;
    uint32_t postcond_hash;
    uint32_t invariant_hash;
} vfy_predicate_t;

static vfy_record_t vfy_table[VFY_CAPACITY];
static vfy_predicate_t vfy_pred_table[VFY_PRED_CAPACITY];
static uint32_t vfy_count = 0;
static uint32_t vfy_pred_count = 0;
static uint32_t vfy_next_id = 1;
static int vfy_ready = 0;

/* TODO: durch timer_ticks ersetzen, sobald das Time-Subsystem portiert ist. */
static uint32_t current_timer_ticks(void) {
    return 0;
}

/* TODO: durch echtes Capability-Lookup ersetzen, sobald cap_integration32.inc
   portiert ist. Bis dahin: keine Capability bekannt -> immer "nicht gefunden". */
static int cap_query(uint32_t cap_id, uint32_t *out_flags) {
    (void)cap_id;
    (void)out_flags;
    return -1;
}

void verify_initialize(void) {
    for (uint32_t i = 0; i < VFY_CAPACITY; ++i) {
        vfy_table[i] = (vfy_record_t){0};
    }
    for (uint32_t i = 0; i < VFY_PRED_CAPACITY; ++i) {
        vfy_pred_table[i] = (vfy_predicate_t){0};
    }
    vfy_count = 0;
    vfy_pred_count = 0;
    vfy_next_id = 1;
    vfy_ready = 1;
}

int verify_check(uint32_t type, uint32_t subject_id, uint32_t *out_result) {
    if (type >= VFY_TYPE_COUNT) {
        return -1;
    }
    if (vfy_count >= VFY_CAPACITY) {
        *out_result = VFY_UNKNOWN;
        return 0;
    }

    uint32_t result;
    if (type == VFY_TYPE_CAPABILITY) {
        uint32_t cap_flags = 0;
        if (cap_query(subject_id, &cap_flags) != 0) {
            result = VFY_WARN;
        } else if (cap_flags & CAP_FLAG_REVOKED) {
            result = VFY_FAIL;
        } else {
            result = VFY_PASS;
        }
    } else if (type == VFY_TYPE_MEMORY) {
        result = VFY_PASS; /* statische Kern-Allokation */
    } else {
        result = (subject_id == 0) ? VFY_WARN : VFY_PASS;
    }

    vfy_record_t *rec = &vfy_table[vfy_count];
    rec->id = vfy_next_id;
    rec->type = type;
    rec->subject_id = subject_id;
    rec->result = result;
    rec->timestamp = current_timer_ticks();
    ++vfy_next_id;
    ++vfy_count;

    *out_result = result;
    return 0;
}

int verify_contract_register(uint32_t subject_id, uint32_t precond_hash, uint32_t postcond_hash) {
    if (vfy_pred_count >= VFY_PRED_CAPACITY) {
        return -1;
    }
    vfy_predicate_t *pred = &vfy_pred_table[vfy_pred_count];
    pred->subject_id = subject_id;
    pred->precond_hash = precond_hash;
    pred->postcond_hash = postcond_hash;
    pred->invariant_hash = 0;
    ++vfy_pred_count;
    return 0;
}

int verify_self_test(void) {
    if (vfy_ready != 1) return -1;

    uint32_t result = 0;
    if (verify_check(VFY_TYPE_MEMORY, 0, &result) != 0) return -1;
    if (result != VFY_PASS) return -1;

    if (verify_check(VFY_TYPE_RUNTIME, 1, &result) != 0) return -1;

    if (verify_contract_register(0x100u, 0xAABBu, 0xCCDDu) != 0) return -1;

    return 0;
}
