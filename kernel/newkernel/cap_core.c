/* NovaOS C-Kernel - Nova Capability Core (siehe cap_core.h fuer Scope). */
#include "cap_core.h"

typedef struct {
    uint32_t id;
    uint32_t subject;
    uint32_t rights;
    uint32_t delegable;
    uint32_t domain;
    uint32_t generation;
    uint8_t state;
    int present;
} cap_core_record_t;

static cap_core_record_t cap_core_table[CAP_CORE_MAX_CAPS];
static uint32_t cap_core_next_id = 0;
static uint32_t cap_core_next_gen = 0;
static uint32_t cap_core_stat_created = 0;
static uint32_t cap_core_stat_revoked = 0;
static uint32_t cap_core_stat_denied = 0;

void cap_core_initialize(void) {
    for (uint32_t i = 0; i < CAP_CORE_MAX_CAPS; ++i) cap_core_table[i] = (cap_core_record_t){0};
    cap_core_next_id = 0;
    cap_core_next_gen = 0;
    cap_core_stat_created = 0;
    cap_core_stat_revoked = 0;
    cap_core_stat_denied = 0;
}

static cap_core_record_t *cap_core_find(uint32_t cap_id) {
    for (uint32_t i = 0; i < CAP_CORE_MAX_CAPS; ++i) {
        if (cap_core_table[i].present && cap_core_table[i].id == cap_id) return &cap_core_table[i];
    }
    return 0;
}

static cap_core_record_t *cap_core_alloc(uint32_t *out_cap_id) {
    for (uint32_t i = 0; i < CAP_CORE_MAX_CAPS; ++i) {
        if (!cap_core_table[i].present) {
            uint32_t id = cap_core_next_id++;
            *out_cap_id = id;
            return &cap_core_table[i];
        }
    }
    return 0;
}

int cap_core_create(uint32_t subject, uint32_t rights, uint32_t delegable_rights,
                     uint32_t domain, uint32_t *out_cap_id) {
    if (delegable_rights & ~rights) return CAP_CORE_ERR_RIGHTS;

    uint32_t cap_id;
    cap_core_record_t *rec = cap_core_alloc(&cap_id);
    if (!rec) return CAP_CORE_ERR_LIMIT;

    rec->id = cap_id;
    rec->subject = subject;
    rec->rights = rights;
    rec->delegable = delegable_rights;
    rec->domain = domain;
    rec->generation = cap_core_next_gen++;
    rec->state = CAP_CORE_STATE_VALID;
    rec->present = 1;

    ++cap_core_stat_created;
    *out_cap_id = cap_id;
    return 0;
}

int cap_core_check(uint32_t cap_id, uint32_t required_rights) {
    cap_core_record_t *rec = cap_core_find(cap_id);
    if (!rec) {
        ++cap_core_stat_denied;
        return CAP_CORE_ERR_INVALID;
    }
    if (rec->state == CAP_CORE_STATE_REVOKED) {
        ++cap_core_stat_denied;
        return CAP_CORE_ERR_REVOKED;
    }
    if (rec->state != CAP_CORE_STATE_VALID) {
        ++cap_core_stat_denied;
        return CAP_CORE_ERR_INVALID;
    }
    if ((rec->rights & required_rights) != required_rights) {
        ++cap_core_stat_denied;
        return CAP_CORE_ERR_RIGHTS;
    }
    return 0;
}

int cap_core_derive(uint32_t src_cap_id, uint32_t new_rights, uint32_t new_delegable,
                     uint32_t *out_cap_id) {
    cap_core_record_t *src = cap_core_find(src_cap_id);
    if (!src) return CAP_CORE_ERR_INVALID;
    if (src->state != CAP_CORE_STATE_VALID) return CAP_CORE_ERR_REVOKED;

    if (new_rights & ~src->rights) return CAP_CORE_ERR_RIGHTS;
    if (new_delegable & ~src->delegable) return CAP_CORE_ERR_DELEGATE;
    if (new_delegable & ~new_rights) return CAP_CORE_ERR_RIGHTS;

    uint32_t cap_id;
    cap_core_record_t *rec = cap_core_alloc(&cap_id);
    if (!rec) return CAP_CORE_ERR_LIMIT;

    rec->id = cap_id;
    rec->subject = src->subject;
    rec->rights = new_rights;
    rec->delegable = new_delegable;
    rec->domain = src->domain;
    rec->generation = cap_core_next_gen++;
    rec->state = CAP_CORE_STATE_VALID;
    rec->present = 1;

    ++cap_core_stat_created;
    *out_cap_id = cap_id;
    return 0;
}

int cap_core_revoke(uint32_t cap_id) {
    cap_core_record_t *rec = cap_core_find(cap_id);
    if (!rec) return CAP_CORE_ERR_INVALID;
    if (rec->state == CAP_CORE_STATE_REVOKED) return CAP_CORE_ERR_REVOKED;

    rec->state = CAP_CORE_STATE_REVOKED;
    ++cap_core_stat_revoked;
    return 0;
}

int cap_core_self_test(void) {
    cap_core_initialize();

    uint32_t cap1;
    if (cap_core_create(42u, 0x3u, 0x1u, 1u, &cap1) != 0) return -1;

    if (cap_core_check(cap1, 0x1u) != 0) return -1;
    if (cap_core_check(cap1, 0x100u) != CAP_CORE_ERR_RIGHTS) return -1;

    uint32_t cap2;
    if (cap_core_derive(cap1, 0x1u, 0x1u, &cap2) != 0) return -1;

    uint32_t dummy;
    if (cap_core_derive(cap1, 0x3u | 0x100u, 0x1u, &dummy) == 0) return -1; /* mehr Rechte als Quelle */

    if (cap_core_revoke(cap1) != 0) return -1;
    if (cap_core_check(cap1, 0x1u) != CAP_CORE_ERR_REVOKED) return -1;
    if (cap_core_check(0xDEADu, 0x1u) != CAP_CORE_ERR_INVALID) return -1;

    /* abgeleitete Capability (cap2) bleibt unberuehrt vom Revoke der
       Quelle (keine implizite Transitivitaet in dieser Engine). */
    if (cap_core_check(cap2, 0x1u) != 0) return -1;

    uint32_t cap3;
    if (cap_core_derive(cap2, 0x1u, 0u, &cap3) != 0) return -1;
    if (cap_core_check(cap3, 0x1u) != 0) return -1;

    /* Zwei unabhaengige Subjects duerfen unabhaengig verwaltet werden
       (fuer cap.c's Bootstrap-Policy-IDs UND uobj.c's echte object.c-
       Handles gleichzeitig nutzbar, siehe cap_core.h Scope-Hinweis). */
    uint32_t cap4;
    if (cap_core_create(0u, 0x1u, 0u, 2u, &cap4) != 0) return -1; /* subject=0 ist erlaubt (opak) */
    if (cap_core_check(cap4, 0x1u) != 0) return -1;

    return 0;
}
