/* NovaOS C-Kernel - Update-Manager (Port aus update32.inc). */
#include "update.h"

typedef struct {
    uint32_t id;
    uint32_t type;
    uint32_t state;
    uint32_t flags;
    uint32_t target_id;
    uint32_t old_version;
    uint32_t new_version;
    uint32_t content_hash;
    uint32_t sign_hash;
    uint32_t prov_hash;
    uint32_t snapshot_id;
    uint32_t delta_size;
    uint32_t sbom_hash;
    uint32_t canary_pct;
    uint32_t timestamp;
} upd_record_t;

static upd_record_t upd_table[UPD_CAPACITY];
static uint32_t upd_count = 0;
static uint32_t upd_next_id = 1;
static uint32_t upd_active_slot = UPD_SLOT_A;
static int upd_ready = 0;

/* TODO: durch timer_ticks ersetzen, sobald das Time-Subsystem portiert ist. */
static uint32_t current_timer_ticks(void) {
    return 0;
}

void update_initialize(void) {
    for (uint32_t i = 0; i < UPD_CAPACITY; ++i) {
        upd_table[i] = (upd_record_t){0};
    }
    upd_count = 0;
    upd_next_id = 1;
    upd_active_slot = UPD_SLOT_A;
    upd_ready = 1;
}

int upd_submit(uint32_t type, uint32_t target_id, uint32_t new_version, uint32_t flags,
               uint32_t *out_upd_id) {
    if (upd_count >= UPD_CAPACITY) {
        return -1;
    }
    upd_record_t *rec = &upd_table[upd_count];
    rec->id = upd_next_id;
    rec->type = type;
    rec->state = UPD_STATE_PENDING;
    rec->flags = flags;
    rec->target_id = target_id;
    rec->old_version = 0;
    rec->new_version = new_version;
    rec->content_hash = target_id ^ new_version; /* XOR(target, version) */
    rec->sign_hash = 0;
    rec->prov_hash = 0;
    rec->snapshot_id = 0;
    rec->delta_size = 0;
    rec->sbom_hash = 0;
    rec->canary_pct = 100; /* Default: 100% Rollout */
    rec->timestamp = current_timer_ticks();

    if (out_upd_id) {
        *out_upd_id = upd_next_id;
    }
    ++upd_next_id;
    ++upd_count;
    return 0;
}

static upd_record_t *update_find(uint32_t upd_id) {
    for (uint32_t i = 0; i < upd_count; ++i) {
        if (upd_table[i].id == upd_id) {
            return &upd_table[i];
        }
    }
    return 0;
}

int upd_apply(uint32_t upd_id) {
    upd_record_t *rec = update_find(upd_id);
    if (!rec) {
        return -1;
    }

    /* Signaturpruefung wenn Flag gesetzt. */
    if ((rec->flags & UPD_FLAG_SIGNED) && rec->sign_hash == 0) {
        rec->state = UPD_STATE_FAILED;
        return -1;
    }

    /* A/B: Slot wechseln. */
    if (rec->flags & UPD_FLAG_AB) {
        upd_active_slot ^= 1u;
    }

    rec->flags |= UPD_FLAG_VERIFIED;
    rec->state = UPD_STATE_APPLIED;
    return 0;
}

int upd_rollback(uint32_t upd_id) {
    upd_record_t *rec = update_find(upd_id);
    if (!rec) {
        return -1;
    }
    rec->state = UPD_STATE_ROLLED_BACK;

    /* A/B: Slot zurueck. */
    if (rec->flags & UPD_FLAG_AB) {
        upd_active_slot ^= 1u;
    }
    return 0;
}

int update_self_test(void) {
    if (upd_ready != 1) return -1;

    uint32_t upd_id = 0;
    if (upd_submit(UPD_TYPE_PACKAGE, 0x1000u, 0x00020000u,
                    UPD_FLAG_ATOMIC | UPD_FLAG_AB, &upd_id) != 0) {
        return -1;
    }
    if (upd_apply(upd_id) != 0) return -1;
    if (upd_rollback(upd_id) != 0) return -1;

    return 0;
}
