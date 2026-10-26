/* NovaOS C-Kernel - §103 Capability Framework 1.0 (Port aus
   security32.inc). */
#include "cap.h"
#include "evbus.h"

#define NP_EVENT_CAP_CREATED 9u
#define NP_EVENT_CAP_REVOKED 10u
#define NP_EVENT_CAP_DENIED  11u

typedef struct {
    uint32_t id;
    uint32_t obj_id;
    uint32_t rights;
    uint32_t delegable;
    uint32_t domain;
    uint32_t expiration;
    uint32_t generation;
    uint32_t flags;
    uint32_t policy_id;
    uint8_t state;
    int present;
} cap_record_t;

static cap_record_t cap_table[CAP_MAX_CAPS];
static uint32_t cap_next_id = 0;
static uint32_t cap_next_gen = 0;
static uint32_t cap_stat_created = 0;
static uint32_t cap_stat_revoked = 0;
static uint32_t cap_stat_denied = 0;
static uint32_t cap_boot_kernel_id = 0;
static uint32_t cap_boot_id = 0;

static cap_record_t *cap_find(uint32_t cap_id) {
    for (uint32_t i = 0; i < CAP_MAX_CAPS; ++i) {
        if (cap_table[i].present && cap_table[i].id == cap_id) return &cap_table[i];
    }
    return 0;
}

static cap_record_t *cap_alloc(uint32_t *out_cap_id) {
    for (uint32_t i = 0; i < CAP_MAX_CAPS; ++i) {
        if (!cap_table[i].present) {
            uint32_t id = cap_next_id++;
            *out_cap_id = id;
            return &cap_table[i];
        }
    }
    return 0;
}

int cap_create(uint32_t obj_id, uint32_t rights, uint32_t delegable_rights, uint32_t domain,
               uint32_t *out_cap_id) {
    if (delegable_rights & ~rights) return NP_ERR_CAP_RIGHTS; /* delegable muss Teilmenge sein */

    uint32_t cap_id;
    cap_record_t *rec = cap_alloc(&cap_id);
    if (!rec) return NP_ERR_CAP_LIMIT;

    rec->id = cap_id;
    rec->obj_id = obj_id;
    rec->rights = rights;
    rec->delegable = delegable_rights;
    rec->domain = domain;
    rec->expiration = 0;
    rec->generation = cap_next_gen++;
    rec->flags = 0;
    rec->policy_id = 0;
    rec->state = CAP_STATE_VALID;
    rec->present = 1;

    ++cap_stat_created;

    evbus_publish(NP_EVENT_CAP_CREATED, cap_id, 0, 0, 0);

    *out_cap_id = cap_id;
    return 0;
}

int cap_check(uint32_t cap_id, uint32_t required_rights) {
    cap_record_t *rec = cap_find(cap_id);
    if (!rec) {
        ++cap_stat_denied;
        return NP_ERR_CAP_INVALID;
    }

    if (rec->state == CAP_STATE_REVOKED) {
        ++cap_stat_denied;
        return NP_ERR_CAP_REVOKED;
    }
    if (rec->state == CAP_STATE_EXPIRED) {
        ++cap_stat_denied;
        return NP_ERR_CAP_EXPIRED;
    }
    if (rec->state != CAP_STATE_VALID) {
        ++cap_stat_denied;
        return NP_ERR_CAP_INVALID;
    }

    if ((rec->rights & required_rights) != required_rights) {
        ++cap_stat_denied;
        evbus_publish(NP_EVENT_CAP_DENIED, 0, 0, 0, 0);
        return NP_ERR_CAP_RIGHTS;
    }

    return 0;
}

int cap_derive(uint32_t src_cap_id, uint32_t new_rights, uint32_t new_delegable, uint32_t *out_cap_id) {
    cap_record_t *src = cap_find(src_cap_id);
    if (!src) return NP_ERR_CAP_INVALID;
    if (src->state != CAP_STATE_VALID) return NP_ERR_CAP_REVOKED;

    if (new_rights & ~src->rights) return NP_ERR_CAP_RIGHTS;
    if (new_delegable & ~src->delegable) return NP_ERR_CAP_DELEGATE;
    if (new_delegable & ~new_rights) return NP_ERR_CAP_RIGHTS;

    uint32_t cap_id;
    cap_record_t *rec = cap_alloc(&cap_id);
    if (!rec) return NP_ERR_CAP_LIMIT;

    rec->id = cap_id;
    rec->obj_id = src->obj_id;
    rec->rights = new_rights;
    rec->delegable = new_delegable;
    rec->domain = src->domain;
    rec->expiration = src->expiration;
    rec->generation = cap_next_gen++;
    rec->flags = 0;
    rec->policy_id = 0;
    rec->state = CAP_STATE_VALID;
    rec->present = 1;

    ++cap_stat_created;

    evbus_publish(NP_EVENT_CAP_CREATED, cap_id, 0, 0, 0);

    *out_cap_id = cap_id;
    return 0;
}

int cap_revoke(uint32_t cap_id) {
    cap_record_t *rec = cap_find(cap_id);
    if (!rec) return NP_ERR_CAP_INVALID;
    if (rec->state == CAP_STATE_REVOKED) return NP_ERR_CAP_REVOKED;

    rec->state = CAP_STATE_REVOKED;
    ++cap_stat_revoked;

    evbus_publish(NP_EVENT_CAP_REVOKED, cap_id, 0, 0, 0);
    return 0;
}

int cap_initialize(void) {
    for (uint32_t i = 0; i < CAP_MAX_CAPS; ++i) cap_table[i] = (cap_record_t){0};
    cap_next_id = 0;
    cap_next_gen = 0;
    cap_stat_created = 0;
    cap_stat_revoked = 0;
    cap_stat_denied = 0;
    cap_boot_kernel_id = 0;
    cap_boot_id = 0;

    if (evbus_register_schema(NP_EVENT_CAP_CREATED, NP_EVENT_CLASS_LIFECYCLE,
                               0, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY) != 0) return -1;
    if (evbus_register_schema(NP_EVENT_CAP_REVOKED, NP_EVENT_CLASS_STATE,
                               0, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY) != 0) return -1;
    if (evbus_register_schema(NP_EVENT_CAP_DENIED, NP_EVENT_CLASS_SECURITY,
                               0, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY) != 0) return -1;

    uint32_t cap_id;
    if (cap_create(1, NP_CAP_ADMIN | NP_CAP_QUERY | NP_CAP_MODIFY, NP_CAP_QUERY,
                    CAP_DOMAIN_KERNEL, &cap_id) != 0) return -1;
    cap_boot_kernel_id = cap_id;

    if (cap_create(2, NP_CAP_QUERY | NP_CAP_SUBSCRIBE, 0, CAP_DOMAIN_BOOT, &cap_id) != 0) return -1;
    cap_boot_id = cap_id;

    return 0;
}

int cap_self_test(void) {
    uint32_t cap1;
    if (cap_create(42, NP_CAP_QUERY | NP_CAP_MODIFY, NP_CAP_QUERY, CAP_DOMAIN_KERNEL, &cap1) != 0) return -1;

    if (cap_check(cap1, NP_CAP_QUERY) != 0) return -1;

    if (cap_check(cap1, NP_CAP_ADMIN) != NP_ERR_CAP_RIGHTS) return -1;

    uint32_t cap2;
    if (cap_derive(cap1, NP_CAP_QUERY, NP_CAP_QUERY, &cap2) != 0) return -1;

    uint32_t dummy;
    if (cap_derive(cap1, NP_CAP_QUERY | NP_CAP_MODIFY | NP_CAP_ADMIN, NP_CAP_QUERY, &dummy) == 0) return -1;

    if (cap_revoke(cap1) != 0) return -1;

    if (cap_check(cap1, NP_CAP_QUERY) != NP_ERR_CAP_REVOKED) return -1;

    if (cap_check(0xDEADu, NP_CAP_QUERY) != NP_ERR_CAP_INVALID) return -1;

    uint32_t cap3;
    if (cap_derive(cap2, NP_CAP_QUERY, 0, &cap3) != 0) return -1;

    if (cap_check(cap3, NP_CAP_MODIFY) != NP_ERR_CAP_RIGHTS) return -1;

    return 0;
}
