/* NovaOS C-Kernel - Identity-Subsystem (Port aus identity32.inc). */
#include "identity.h"

typedef struct {
    uint32_t id;
    uint32_t type;
    uint32_t state;
    uint32_t cred_type;
    uint32_t mfa_mask;
    uint32_t cred_hash;
    uint32_t federation_id;
    uint32_t stepup_level;
    uint32_t recovery_hash;
    uint32_t auth_time;
    uint32_t flags;
} identmgr_record_t;

static identmgr_record_t identmgr_table[IDENTMGR_CAPACITY];
static uint32_t identmgr_count = 0;
static uint32_t identmgr_next_id = 1;
static int identmgr_ready = 0;

/* TODO: durch timer_ticks ersetzen, sobald das Time-Subsystem portiert ist. */
static uint32_t current_timer_ticks(void) {
    return 0;
}

int ident_register(uint32_t id, uint32_t type, uint32_t cred_type, uint32_t mfa_mask) {
    if (identmgr_count >= IDENTMGR_CAPACITY) {
        return -1;
    }
    identmgr_record_t *rec = &identmgr_table[identmgr_count];
    rec->id = id;
    rec->type = type;
    rec->state = IDENTMGR_STATE_ACTIVE;
    rec->cred_type = cred_type;
    rec->mfa_mask = mfa_mask;
    rec->cred_hash = 0;
    rec->federation_id = 0xFFFFFFFFu;
    rec->stepup_level = 0;
    rec->recovery_hash = 0;
    rec->auth_time = current_timer_ticks();
    rec->flags = 0;
    ++identmgr_count;
    return 0;
}

int identity_mgr_initialize(void) {
    for (uint32_t i = 0; i < IDENTMGR_CAPACITY; ++i) {
        identmgr_table[i] = (identmgr_record_t){0};
    }
    identmgr_count = 0;
    identmgr_next_id = 1;

    /* System-Geraete-Identitaet anlegen (ID 0). */
    if (ident_register(0, IDENTMGR_TYPE_DEVICE, CRED_TYPE_CERT, MFA_FACTOR_HARDWARE) != 0) {
        return -1;
    }

    identmgr_ready = 1;
    return 0;
}

static identmgr_record_t *identmgr_find(uint32_t id) {
    for (uint32_t i = 0; i < identmgr_count; ++i) {
        if (identmgr_table[i].id == id) {
            return &identmgr_table[i];
        }
    }
    return 0;
}

int ident_authenticate(uint32_t id, uint32_t cred_hash, uint32_t mfa_presented) {
    identmgr_record_t *rec = identmgr_find(id);
    if (!rec) {
        return -1;
    }
    if (rec->state != IDENTMGR_STATE_ACTIVE) {
        return -1;
    }
    /* Credential-Hash vergleichen (0=beliebig fuer System-Geraet). */
    if (rec->cred_hash != 0 && rec->cred_hash != cred_hash) {
        return -1;
    }
    /* MFA: alle geforderten Faktoren muessen vorhanden sein. */
    if ((rec->mfa_mask & mfa_presented) != rec->mfa_mask) {
        return -1;
    }
    rec->auth_time = current_timer_ticks();
    return 0;
}

int ident_revoke(uint32_t id) {
    identmgr_record_t *rec = identmgr_find(id);
    if (!rec) {
        return -1;
    }
    rec->state = IDENTMGR_STATE_REVOKED;
    return 0;
}

int identity_mgr_self_test(void) {
    if (identmgr_ready != 1) return -1;

    if (ident_register(1, IDENT_TYPE_APP, CRED_TYPE_TOKEN, MFA_FACTOR_NONE) != 0) return -1;
    if (ident_authenticate(1, 0, MFA_FACTOR_NONE) != 0) return -1;

    return 0;
}
