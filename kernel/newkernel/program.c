/* NovaOS C-Kernel - Program-Manager, Registry und Capability-System
   (Port aus program32.inc). */
#include "program.h"
#include "fs.h"

typedef struct {
    uint32_t id;
    uint32_t flags;
    uint32_t state;
    uint32_t scope;
    uint32_t version;
    uint32_t min_version; /* dient bei Rollback als Vorversion */
    uint32_t ns_id;
    uint32_t cap_mask_lo;
    uint32_t cap_mask_hi;
    uint32_t trust_hash;
    uint32_t dep_count;
    uint32_t perm_mask;
} prog_record_t;

typedef struct {
    uint32_t type;
    uint32_t key_lo;
    uint32_t value_lo;
    uint32_t value_hi;
    uint32_t flags;
} reg_entry_t;

typedef struct {
    uint32_t id;
    uint32_t category;
    uint32_t flags;
    uint32_t program_id;
    uint32_t ns_id;
    uint32_t version_req;
    uint32_t provider_id;
    uint32_t perm_mask;
    uint32_t sign_hash;
    uint32_t runtime_data;
    uint32_t shim_ptr;
} cap_record_t;

static prog_record_t prog_table[PROG_CAPACITY];
static reg_entry_t prog_reg_table[REG_CAPACITY];
static cap_record_t prog_cap_table[CAP_REGISTRY_CAPACITY];
static uint32_t prog_count = 0;
static uint32_t prog_reg_count = 0;
static uint32_t prog_cap_count = 0;
static uint32_t prog_next_cap_id = 1;
static int prog_manager_ready = 0;

int cap_register(uint32_t category, uint32_t flags, uint32_t program_id, uint32_t *out_cap_id) {
    if (prog_cap_count >= CAP_REGISTRY_CAPACITY) {
        return -1;
    }
    cap_record_t *cap = &prog_cap_table[prog_cap_count];
    cap->id = prog_next_cap_id;
    cap->category = category;
    cap->flags = flags;
    cap->program_id = program_id;
    cap->ns_id = FS_NS_ROOT;
    cap->version_req = 0;
    cap->provider_id = 0xFFFFFFFFu;
    cap->perm_mask = FSPOLICY_ALLOW_READ;
    cap->sign_hash = 0;
    cap->runtime_data = 0;
    cap->shim_ptr = 0;

    *out_cap_id = prog_next_cap_id;
    ++prog_next_cap_id;
    ++prog_cap_count;
    return 0;
}

int reg_insert(uint32_t type, uint32_t key_lo, uint32_t value_lo) {
    if (prog_reg_count >= REG_CAPACITY) {
        return -1;
    }
    reg_entry_t *entry = &prog_reg_table[prog_reg_count];
    entry->type = type;
    entry->key_lo = key_lo;
    entry->value_lo = value_lo;
    entry->value_hi = 0;
    entry->flags = 0;
    ++prog_reg_count;
    return 0;
}

void program_manager_initialize(void) {
    for (uint32_t i = 0; i < PROG_CAPACITY; ++i) prog_table[i] = (prog_record_t){0};
    for (uint32_t i = 0; i < REG_CAPACITY; ++i) prog_reg_table[i] = (reg_entry_t){0};
    for (uint32_t i = 0; i < CAP_REGISTRY_CAPACITY; ++i) prog_cap_table[i] = (cap_record_t){0};

    prog_count = 0;
    prog_reg_count = 0;
    prog_cap_count = 0;
    prog_next_cap_id = 1;

    uint32_t cap_id;
    if (cap_register(CAP_CAT_PLATFORM, CAP_FLAG_ACTIVE | CAP_FLAG_SIGNED, 0xFFFFFFFFu, &cap_id) != 0) return;
    if (cap_register(CAP_CAT_SANDBOX, CAP_FLAG_ACTIVE | CAP_FLAG_SANDBOX, 0xFFFFFFFFu, &cap_id) != 0) return;

    /* Standard-Algorithmus in Registry (NPSPEC-REGISTRY-ALGORITHM-0001). */
    if (reg_insert(REG_TYPE_ALGORITHM, 0x43524333u, 0x43524333u) != 0) return; /* "CRC3" */

    prog_manager_ready = 1;
}

int prog_install(uint32_t prog_id, uint32_t flags, uint32_t scope, uint32_t version) {
    if (prog_count >= PROG_CAPACITY) {
        return -1;
    }
    prog_record_t *rec = &prog_table[prog_count];
    rec->id = prog_id;
    rec->flags = flags;
    rec->state = PROG_STATE_INSTALLED;
    rec->scope = scope;
    rec->version = version;
    rec->min_version = 0;
    rec->ns_id = FS_NS_PROGRAM;
    rec->cap_mask_lo = 0;
    rec->cap_mask_hi = 0;
    rec->dep_count = 0;
    rec->trust_hash = prog_id ^ version; /* NPSPEC-PROGRAM-TRUST-0001 */

    ++prog_count;
    return 0;
}

static prog_record_t *prog_find(uint32_t prog_id) {
    for (uint32_t i = 0; i < prog_count; ++i) {
        if (prog_table[i].id == prog_id) {
            return &prog_table[i];
        }
    }
    return 0;
}

int prog_uninstall(uint32_t prog_id) {
    prog_record_t *rec = prog_find(prog_id);
    if (!rec) return -1;
    rec->state = PROG_STATE_ABSENT;
    return 0;
}

int prog_update(uint32_t prog_id, uint32_t new_version) {
    prog_record_t *rec = prog_find(prog_id);
    if (!rec) return -1;
    rec->min_version = rec->version; /* alte Version fuer Rollback sichern */
    rec->version = new_version;
    rec->state = PROG_STATE_INSTALLED;
    return 0;
}

int prog_rollback(uint32_t prog_id) {
    prog_record_t *rec = prog_find(prog_id);
    if (!rec) return -1;
    uint32_t tmp = rec->version;
    rec->version = rec->min_version;
    rec->min_version = tmp;
    rec->state = PROG_STATE_ROLLBACK;
    return 0;
}

int reg_query(uint32_t type, uint32_t key_lo, uint32_t *out_value_lo) {
    for (uint32_t i = 0; i < prog_reg_count; ++i) {
        reg_entry_t *entry = &prog_reg_table[i];
        if (entry->type == type && entry->key_lo == key_lo) {
            *out_value_lo = entry->value_lo;
            return 0;
        }
    }
    return -1;
}

static cap_record_t *cap_find(uint32_t cap_id) {
    for (uint32_t i = 0; i < prog_cap_count; ++i) {
        if (prog_cap_table[i].id == cap_id) {
            return &prog_cap_table[i];
        }
    }
    return 0;
}

int cap_activate(uint32_t cap_id) {
    cap_record_t *cap = cap_find(cap_id);
    if (!cap) return -1;
    cap->flags |= CAP_FLAG_ACTIVE;
    cap->flags &= ~CAP_FLAG_REVOKED;
    return 0;
}

int pkg_cap_revoke(uint32_t cap_id) {
    cap_record_t *cap = cap_find(cap_id);
    if (!cap) return -1;
    cap->flags |= CAP_FLAG_REVOKED;
    cap->flags &= ~CAP_FLAG_ACTIVE;
    return 0;
}

int cap_query(uint32_t cap_id, uint32_t *out_category, uint32_t *out_flags) {
    cap_record_t *cap = cap_find(cap_id);
    if (!cap) return -1;
    *out_category = cap->category;
    *out_flags = cap->flags;
    return 0;
}

int cap_check_permission(uint32_t cap_id, uint32_t required_perm) {
    cap_record_t *cap = cap_find(cap_id);
    if (!cap) return -1;
    if (cap->flags & CAP_FLAG_REVOKED) return -1;
    if ((cap->flags & CAP_FLAG_ACTIVE) == 0) return -1;
    if ((cap->perm_mask & required_perm) != required_perm) return -1;
    return 0;
}

int program_manager_self_test(void) {
    if (prog_manager_ready != 1) return -1;

    if (prog_install(0x1000u, PROG_FLAG_SYSTEM | PROG_FLAG_TRUSTED, PROG_SCOPE_SYSTEM, 0x00010000u) != 0) return -1;

    uint32_t value = 0;
    if (reg_query(REG_TYPE_ALGORITHM, 0x43524333u, &value) != 0) return -1;

    uint32_t category = 0, flags = 0;
    if (cap_query(1, &category, &flags) != 0) return -1; /* Platform-Cap hat ID 1 */

    if (prog_update(0x1000u, 0x00020000u) != 0) return -1;
    if (prog_rollback(0x1000u) != 0) return -1;

    return 0;
}
