/* NovaOS C-Kernel - Filesystem-Subsystem (Port aus fs32.inc). */
#include "fs.h"

typedef struct {
    uint32_t id;
    uint32_t flags;
    uint32_t type;
    uint32_t fs_ns;
    uint32_t block_size;
    uint32_t block_count;
    uint32_t free_blocks;
    uint32_t device_id;
    uint32_t mount_gen;
    uint32_t trust_level;
} fs_volume_t;

typedef struct {
    uint32_t id;
    uint32_t flags;
    uint32_t volume_id;
    uint32_t parent_ns;
    uint32_t policy;
    uint32_t path_hash;
    uint32_t isolation;
} fs_namespace_t;

typedef struct {
    uint32_t id;
    uint32_t flags;
    uint32_t volume_id;
    uint32_t op_count;
    uint32_t state; /* 0=open 1=committed 2=rolled back */
    uint32_t checksum;
    uint32_t timestamp;
} fs_tx_t;

static fs_volume_t fs_vol_table[VOL_CAPACITY];
static fs_namespace_t fs_ns_table[NS_CAPACITY];
static fs_tx_t fs_tx_table[TX_CAPACITY];
static uint32_t fs_vol_count = 0;
static uint32_t fs_ns_count = 0;
static uint32_t fs_tx_count = 0;
static uint32_t fs_next_tx_id = 1;
static uint32_t fs_obj_generation = 1;
static uint32_t fs_obj_inode_counter = 0;
static int fs_ready = 0;

/* TODO: durch timer_ticks ersetzen, sobald das Time-Subsystem portiert ist. */
static uint32_t current_timer_ticks(void) {
    return 0;
}

int fs_vol_register(uint32_t vol_id, uint32_t flags, uint32_t ns_id) {
    if (fs_vol_count >= VOL_CAPACITY) {
        return -1;
    }
    fs_volume_t *vol = &fs_vol_table[fs_vol_count];
    vol->id = vol_id;
    vol->flags = flags;
    vol->type = FSOBJ_TYPE_VOLUME;
    vol->fs_ns = ns_id;
    vol->block_size = 4096;
    vol->block_count = 0;
    vol->free_blocks = 0;
    vol->device_id = 0;
    vol->mount_gen = 1;

    if (flags & VOL_FLAG_TRUSTED) {
        vol->trust_level = TRUST_SYSTEM;
    } else if (flags & VOL_FLAG_REMOTE) {
        vol->trust_level = TRUST_REMOTE;
    } else {
        vol->trust_level = TRUST_USER;
    }

    ++fs_vol_count;
    return 0;
}

int fs_ns_register(uint32_t ns_id, uint32_t parent_ns, uint32_t policy) {
    if (fs_ns_count >= NS_CAPACITY) {
        return -1;
    }
    fs_namespace_t *ns = &fs_ns_table[fs_ns_count];
    ns->id = ns_id;
    ns->flags = 0;
    ns->volume_id = 0;
    ns->parent_ns = parent_ns;
    ns->policy = policy;
    ns->path_hash = 0;
    ns->isolation = (policy & (FSPOLICY_WS_ISOLATED | FSPOLICY_TEMP_CLEAR)) ? 1u : 0u;

    ++fs_ns_count;
    return 0;
}

void fs_initialize(void) {
    for (uint32_t i = 0; i < VOL_CAPACITY; ++i) {
        fs_vol_table[i] = (fs_volume_t){0};
    }
    for (uint32_t i = 0; i < NS_CAPACITY; ++i) {
        fs_ns_table[i] = (fs_namespace_t){0};
    }
    for (uint32_t i = 0; i < TX_CAPACITY; ++i) {
        fs_tx_table[i] = (fs_tx_t){0};
    }
    fs_vol_count = 0;
    fs_ns_count = 0;
    fs_tx_count = 0;
    fs_next_tx_id = 1;
    fs_obj_generation = 1;
    fs_obj_inode_counter = 0;

    if (fs_vol_register(0, VOL_FLAG_MOUNTED | VOL_FLAG_SYSTEM | VOL_FLAG_TRUSTED, FS_NS_ROOT) != 0) {
        return;
    }

    if (fs_ns_register(FS_NS_ROOT, 0xFFFFFFFFu,
                        FSPOLICY_ALLOW_READ | FSPOLICY_ALLOW_WRITE | FSPOLICY_ALLOW_EXEC) != 0) return;
    if (fs_ns_register(FS_NS_SYSTEM, FS_NS_ROOT,
                        FSPOLICY_ALLOW_READ | FSPOLICY_ALLOW_EXEC | FSPOLICY_SYS_WRITE) != 0) return;
    if (fs_ns_register(FS_NS_USER, FS_NS_ROOT,
                        FSPOLICY_ALLOW_READ | FSPOLICY_ALLOW_WRITE | FSPOLICY_ALLOW_EXEC) != 0) return;
    if (fs_ns_register(FS_NS_TEMP, FS_NS_ROOT,
                        FSPOLICY_ALLOW_READ | FSPOLICY_ALLOW_WRITE | FSPOLICY_TEMP_CLEAR) != 0) return;
    if (fs_ns_register(FS_NS_WORKSPACE, FS_NS_ROOT,
                        FSPOLICY_ALLOW_READ | FSPOLICY_ALLOW_WRITE | FSPOLICY_WS_ISOLATED) != 0) return;
    if (fs_ns_register(FS_NS_PROGRAM, FS_NS_ROOT,
                        FSPOLICY_ALLOW_READ | FSPOLICY_ALLOW_EXEC) != 0) return;
    if (fs_ns_register(FS_NS_DEVICE, FS_NS_ROOT,
                        FSPOLICY_ALLOW_READ | FSPOLICY_ALLOW_WRITE) != 0) return;

    fs_ready = 1;
}

int fs_vol_mount(uint32_t vol_id, uint32_t ns_id, uint32_t extra_flags) {
    for (uint32_t i = 0; i < fs_vol_count; ++i) {
        if (fs_vol_table[i].id == vol_id) {
            fs_vol_table[i].flags |= VOL_FLAG_MOUNTED | extra_flags;
            fs_vol_table[i].fs_ns = ns_id;
            ++fs_vol_table[i].mount_gen;
            return 0;
        }
    }
    return -1;
}

int fs_vol_lookup(uint32_t ns_id, uint32_t *out_vol_id) {
    for (uint32_t i = 0; i < fs_vol_count; ++i) {
        if (fs_vol_table[i].fs_ns == ns_id) {
            *out_vol_id = fs_vol_table[i].id;
            return 0;
        }
    }
    return -1;
}

void fs_objid_make(uint32_t volume_id, uint32_t inode_lo, fs_objid_t *out_objid) {
    out_objid->volume_id = volume_id;
    out_objid->generation = fs_obj_generation;
    out_objid->inode_seq_lo = inode_lo;
    out_objid->inode_seq_hi = 0;
    ++fs_obj_inode_counter;
}

int fs_ns_check_policy(uint32_t ns_id, uint32_t required_policy) {
    for (uint32_t i = 0; i < fs_ns_count; ++i) {
        if (fs_ns_table[i].id == ns_id) {
            return ((fs_ns_table[i].policy & required_policy) == required_policy) ? 0 : -1;
        }
    }
    return -1;
}

int fs_tx_begin(uint32_t volume_id, uint32_t *out_tx_id) {
    if (fs_tx_count >= TX_CAPACITY) {
        return -1;
    }
    fs_tx_t *tx = &fs_tx_table[fs_tx_count];
    tx->id = fs_next_tx_id;
    tx->flags = 0;
    tx->volume_id = volume_id;
    tx->op_count = 0;
    tx->state = 0;
    tx->timestamp = current_timer_ticks();
    *out_tx_id = fs_next_tx_id;
    ++fs_next_tx_id;
    ++fs_tx_count;
    return 0;
}

int fs_tx_commit(uint32_t tx_id) {
    for (uint32_t i = 0; i < fs_tx_count; ++i) {
        if (fs_tx_table[i].id == tx_id) {
            fs_tx_table[i].state = 1;
            return 0;
        }
    }
    return -1;
}

int fs_tx_rollback(uint32_t tx_id) {
    for (uint32_t i = 0; i < fs_tx_count; ++i) {
        if (fs_tx_table[i].id == tx_id) {
            fs_tx_table[i].state = 2;
            return 0;
        }
    }
    return -1;
}

void fs_temp_clear(void) {
    for (uint32_t i = 0; i < fs_ns_count; ++i) {
        if (fs_ns_table[i].id == FS_NS_TEMP) {
            fs_ns_table[i].flags = 0;
        }
    }
}

int fs_self_test(void) {
    if (fs_ready != 1) return -1;
    if (fs_vol_count == 0) return -1;
    if (fs_ns_count == 0) return -1;

    uint32_t vol_id = 0;
    if (fs_vol_lookup(FS_NS_ROOT, &vol_id) != 0) return -1;

    if (fs_ns_check_policy(FS_NS_ROOT, FSPOLICY_ALLOW_READ) != 0) return -1;

    uint32_t tx_id = 0;
    if (fs_tx_begin(0, &tx_id) != 0) return -1;
    if (fs_tx_commit(tx_id) != 0) return -1;

    fs_objid_t objid;
    fs_objid_make(0, OBJID_ROOT_INODE_LO, &objid);

    return 0;
}
