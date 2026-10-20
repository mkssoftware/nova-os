/* NovaOS C-Kernel - Trust-Subsystem (Port aus trust32.inc). */
#include "trust.h"

typedef struct {
    uint32_t subject_id;
    uint32_t level;
    uint32_t source;
    uint32_t flags;
    uint32_t sign_hash;
    uint32_t prov_hash;
    uint32_t policy_mask;
    uint32_t attest_time;
} trust_record_t;

typedef struct {
    uint32_t subject_id;
    uint32_t event; /* 0=grant 1=revoke 2=check */
    uint32_t result; /* 0=ok 1=denied */
    uint32_t timestamp;
} trust_audit_t;

static trust_record_t trust_mgr_table[TRUST_CAPACITY];
static trust_audit_t trust_audit_table[TRUST_AUDIT_CAPACITY];
static uint32_t trust_mgr_count = 0;
static uint32_t trust_audit_count = 0;
static int trust_mgr_ready = 0;

/* TODO: durch timer_ticks ersetzen, sobald das Time-Subsystem portiert ist. */
static uint32_t current_timer_ticks(void) {
    return 0;
}

static void trust_audit_append(uint32_t subject_id, uint32_t event) {
    if (trust_audit_count >= TRUST_AUDIT_CAPACITY) {
        return;
    }
    trust_audit_t *entry = &trust_audit_table[trust_audit_count];
    entry->subject_id = subject_id;
    entry->event = event;
    entry->result = 0;
    entry->timestamp = current_timer_ticks();
    ++trust_audit_count;
}

int trust_grant(uint32_t subject_id, uint32_t level, uint32_t source, uint32_t flags) {
    if (trust_mgr_count >= TRUST_CAPACITY) {
        return -1;
    }
    trust_record_t *rec = &trust_mgr_table[trust_mgr_count];
    rec->subject_id = subject_id;
    rec->level = level;
    rec->source = source;
    rec->flags = flags;
    rec->sign_hash = 0;
    rec->prov_hash = 0;
    rec->policy_mask = 0xFFFFFFFFu;
    rec->attest_time = current_timer_ticks();
    ++trust_mgr_count;

    trust_audit_append(subject_id, 0 /* grant */);
    return 0;
}

static trust_record_t *trust_find(uint32_t subject_id) {
    for (uint32_t i = 0; i < trust_mgr_count; ++i) {
        if (trust_mgr_table[i].subject_id == subject_id) {
            return &trust_mgr_table[i];
        }
    }
    return 0;
}

int trust_check(uint32_t subject_id, uint32_t min_level, uint32_t *out_actual_level) {
    trust_record_t *rec = trust_find(subject_id);
    if (!rec) {
        return -1;
    }
    if (rec->flags & TRUST_FLAG_REVOKED) {
        return -1;
    }
    if (rec->level < min_level) {
        return -1;
    }
    if (out_actual_level) {
        *out_actual_level = rec->level;
    }
    return 0;
}

int trust_revoke(uint32_t subject_id) {
    trust_record_t *rec = trust_find(subject_id);
    if (!rec) {
        return -1;
    }
    rec->flags |= TRUST_FLAG_REVOKED;
    trust_audit_append(subject_id, 1 /* revoke */);
    return 0;
}

int trust_mgr_initialize(void) {
    for (uint32_t i = 0; i < TRUST_CAPACITY; ++i) {
        trust_mgr_table[i] = (trust_record_t){0};
    }
    for (uint32_t i = 0; i < TRUST_AUDIT_CAPACITY; ++i) {
        trust_audit_table[i] = (trust_audit_t){0};
    }
    trust_mgr_count = 0;
    trust_audit_count = 0;

    /* Kernel-Trust-Anker (Subjekt 0). */
    if (trust_grant(0, TRUST_LEVEL_ANCHOR, TRUST_SRC_KERNEL,
                     TRUST_FLAG_AUDITED | TRUST_FLAG_SOFTWARE) != 0) {
        return -1;
    }

    trust_mgr_ready = 1;
    return 0;
}

int trust_mgr_self_test(void) {
    if (trust_mgr_ready != 1) return -1;

    uint32_t level = 0;
    if (trust_check(0, TRUST_LEVEL_HIGH, &level) != 0) return -1;

    if (trust_grant(0x42u, TRUST_LEVEL_MEDIUM, TRUST_SRC_SIGNATURE,
                     TRUST_FLAG_REVOCABLE | TRUST_FLAG_SOFTWARE) != 0) {
        return -1;
    }
    if (trust_revoke(0x42u) != 0) return -1;

    return 0;
}
