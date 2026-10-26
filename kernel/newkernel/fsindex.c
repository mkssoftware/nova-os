/* NovaOS C-Kernel - Filesystem-Index und System-Verzeichnis (Port aus fsindex32.inc). */
#include "fsindex.h"

typedef struct {
    uint32_t id;
    uint32_t flags;
    uint32_t protection;
    uint32_t parent_ns;
    uint32_t trust_level;
    uint32_t object_count;
    uint32_t checksum;
} sysdir_record_t;

typedef struct {
    uint32_t object_id_lo;
    uint32_t object_id_vol;
    uint32_t ns_id;
    uint32_t type;
    uint32_t size;
    uint32_t flags;
    uint32_t search_hash;
    uint32_t tx_id;
} fsindex_entry_t;

static sysdir_record_t sysdir_table[SYSDIR_COUNT];
static fsindex_entry_t fsindex_table[IDX_CAPACITY];
static uint32_t fsindex_count = 0;
static int fsindex_ready = 0;

int sysdir_register(uint32_t sysdir_id, uint32_t protection) {
    if (sysdir_id >= SYSDIR_COUNT) {
        return -1;
    }
    sysdir_record_t *dir = &sysdir_table[sysdir_id];
    dir->id = sysdir_id;
    dir->flags = IDX_FLAG_VALID;
    dir->protection = protection;
    dir->parent_ns = FS_NS_SYSTEM;

    /* Trust-Level aus Schutzklasse ableiten. */
    if (protection >= SYSDIR_PROT_SIGNED) {
        dir->trust_level = TRUST_SYSTEM;
    } else {
        dir->trust_level = TRUST_USER;
    }

    dir->object_count = 0;
    dir->checksum = 0;
    return 0;
}

void fsindex_initialize(void) {
    for (uint32_t i = 0; i < SYSDIR_COUNT; ++i) {
        sysdir_table[i] = (sysdir_record_t){0};
    }
    for (uint32_t i = 0; i < IDX_CAPACITY; ++i) {
        fsindex_table[i] = (fsindex_entry_t){0};
    }
    fsindex_count = 0;

    /* System-Verzeichnisse anlegen (NPSPEC-SYSTEM-LAYOUT-0001). */
    sysdir_register(SYSDIR_KERNEL, SYSDIR_PROT_SIGNED);
    sysdir_register(SYSDIR_BOOT, SYSDIR_PROT_SIGNED);
    sysdir_register(SYSDIR_DRIVERS, SYSDIR_PROT_SIGNED);
    sysdir_register(SYSDIR_MODULES, SYSDIR_PROT_SIGNED);
    sysdir_register(SYSDIR_FRAMEWORK, SYSDIR_PROT_SIGNED);
    sysdir_register(SYSDIR_HAL, SYSDIR_PROT_READ_ONLY);
    sysdir_register(SYSDIR_LIBRARIES, SYSDIR_PROT_READ_ONLY);
    sysdir_register(SYSDIR_LOCALE, SYSDIR_PROT_NONE);
    sysdir_register(SYSDIR_RESOURCES, SYSDIR_PROT_READ_ONLY);
    sysdir_register(SYSDIR_RUNTIME, SYSDIR_PROT_NONE);
    sysdir_register(SYSDIR_SECURITY, SYSDIR_PROT_IMMUTABLE);
    sysdir_register(SYSDIR_SERVICES, SYSDIR_PROT_SIGNED);
    sysdir_register(SYSDIR_TRUST, SYSDIR_PROT_IMMUTABLE);
    sysdir_register(SYSDIR_RECOVERY, SYSDIR_PROT_SIGNED);
    sysdir_register(SYSDIR_POLICY, SYSDIR_PROT_SIGNED);
    sysdir_register(SYSDIR_REGISTRY, SYSDIR_PROT_READ_ONLY);
    sysdir_register(SYSDIR_UPDATES, SYSDIR_PROT_NONE);
    sysdir_register(SYSDIR_FOUNDATION, SYSDIR_PROT_IMMUTABLE);
    sysdir_register(SYSDIR_PROTECTION, SYSDIR_PROT_IMMUTABLE);

    fsindex_ready = 1;
}

int fsindex_insert(uint32_t inode_lo, uint32_t vol_id, uint32_t ns_id, uint32_t type) {
    if (fsindex_count >= IDX_CAPACITY) {
        return -1;
    }
    fsindex_entry_t *entry = &fsindex_table[fsindex_count];
    entry->object_id_lo = inode_lo;
    entry->object_id_vol = vol_id;
    entry->ns_id = ns_id;
    entry->type = type;
    entry->size = 0;
    entry->flags = IDX_FLAG_VALID;
    /* Suchkeyword-Hash: einfacher Poly-Hash. */
    entry->search_hash = (inode_lo * 0x9E3779B9u) ^ vol_id ^ ns_id;
    entry->tx_id = 0;
    ++fsindex_count;
    return 0;
}

int fsindex_search(uint32_t ns_id, uint32_t type, uint32_t *out_inode_lo) {
    for (uint32_t i = 0; i < fsindex_count; ++i) {
        fsindex_entry_t *entry = &fsindex_table[i];
        if (entry->ns_id != ns_id || entry->type != type) {
            continue;
        }
        if (!(entry->flags & IDX_FLAG_VALID)) {
            continue;
        }
        if (out_inode_lo) {
            *out_inode_lo = entry->object_id_lo;
        }
        return 0;
    }
    return -1;
}

void fsindex_rebuild(void) {
    for (uint32_t i = 0; i < fsindex_count; ++i) {
        fsindex_entry_t *entry = &fsindex_table[i];
        entry->flags &= ~IDX_FLAG_DIRTY;
        entry->flags |= IDX_FLAG_VALID;
        entry->search_hash = entry->object_id_lo ^ entry->object_id_vol;
    }
}

int fsindex_self_test(void) {
    if (fsindex_ready != 1) return -1;
    if (sysdir_table[0].id != SYSDIR_KERNEL) return -1;

    if (fsindex_insert(0x100u, 0, FS_NS_SYSTEM, FSOBJ_TYPE_DIR) != 0) return -1;

    uint32_t inode = 0;
    if (fsindex_search(FS_NS_SYSTEM, FSOBJ_TYPE_DIR, &inode) != 0) return -1;

    fsindex_rebuild();

    return 0;
}
