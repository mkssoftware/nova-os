#ifndef NOVA_NOVAFS_H
#define NOVA_NOVAFS_H

/*
 * NovaFS 1.0 On-Disk-Strukturen, Phase 1 (NPSPEC-NOVAFS-ONDISK-0001).
 * Die Kernel-Implementierung (novafs32.inc) und das Host-Werkzeug
 * (tools/novafs) verwenden exakt diese Offsets.
 */

#include <stddef.h>
#include <stdint.h>

#define NOVAFS_VERSION_MAJOR 1u
#define NOVAFS_VERSION_MINOR 0u
#define NOVAFS_BLOCK_SIZE 4096u
#define NOVAFS_NODE_HEADER_SIZE 80u
#define NOVAFS_NODE_MAGIC 0x4552544Eu /* "NTRE" */

#define NOVAFS_INCOMPAT_FREE_SPACE_BITMAP 0x1u
#define NOVAFS_INCOMPAT_FIXED_ITEM_TREES 0x2u

#define NOVAFS_TREE_OBJECT 2u
#define NOVAFS_TREE_DIRECTORY 3u
#define NOVAFS_TREE_EXTENT 4u

#define NOVAFS_OBJECT_FILE 1u
#define NOVAFS_OBJECT_DIRECTORY 2u
#define NOVAFS_ROOT_OBJECT_ID 1u
#define NOVAFS_FIRST_DYNAMIC_OBJECT_ID 256u

typedef enum novafs_state {
    NOVAFS_STATE_CLEAN = 0,
    NOVAFS_STATE_DIRTY = 1,
    NOVAFS_STATE_DEGRADED = 2,
    NOVAFS_STATE_REBUILDING = 3,
    NOVAFS_STATE_REBALANCING = 4,
    NOVAFS_STATE_RECOVERY_REQUIRED = 5,
    NOVAFS_STATE_READ_ONLY = 6,
    NOVAFS_STATE_FAILED = 7
} novafs_state_t;

typedef struct novafs_superblock {
    uint8_t magic[8];
    uint16_t version_major;
    uint16_t version_minor;
    uint32_t block_size;
    uint64_t filesystem_size;
    uint64_t total_blocks;
    uint64_t available_blocks;
    uint64_t generation;
    uint64_t root_tree_block;
    uint64_t object_tree_block;
    uint64_t directory_tree_block;
    uint64_t extent_tree_block;
    uint64_t policy_tree_block;
    uint64_t checksum_tree_block;
    uint64_t free_space_tree_block;
    uint64_t snapshot_tree_block;
    uint64_t transaction_log_block;
    uint64_t feature_flags;
    uint64_t incompat_flags;
    uint64_t readonly_compat_flags;
    uint8_t filesystem_uuid[16];
    uint8_t pool_uuid[16];
    uint8_t device_uuid[16];
    uint8_t volume_name[128];
    uint8_t public_trust_anchor_hash[32];
    uint8_t checksum[32];
    /* NovaFS-1.0-Erweiterung */
    uint32_t extension_size;
    uint32_t state;
    uint64_t next_object_id;
    uint64_t bitmap_blocks;
    uint64_t object_count;
    uint64_t backup_superblock_block;
    uint32_t bitmap_crc32c;
    uint32_t checksum_type;
    uint64_t mount_count;
    uint8_t reserved[NOVAFS_BLOCK_SIZE - 440];
} novafs_superblock_t;

typedef struct novafs_tree_node_header {
    uint32_t magic;
    uint16_t level;
    uint16_t item_count;
    uint64_t tree_id;
    uint64_t block_id;
    uint64_t parent_block;
    uint64_t generation;
    uint8_t checksum[32];
    /* Phase-1-Geometrie */
    uint16_t item_size;
    uint16_t max_items;
    uint16_t key_size;
    uint16_t reserved;
} novafs_tree_node_header_t;

typedef struct novafs_inner_item {
    uint64_t key1;
    uint64_t key2;
    uint64_t child_block;
} novafs_inner_item_t;

typedef struct novafs_object_record {
    uint64_t object_id;
    uint64_t parent_id;
    uint32_t object_type;
    uint32_t flags;
    uint64_t logical_size;
    uint64_t allocated_size;
    uint64_t created_time;
    uint64_t modified_time;
    uint64_t accessed_time;
    uint64_t changed_time;
    uint32_t owner_id;
    uint32_t group_id;
    uint32_t permissions;
    uint32_t link_count;
    uint64_t extent_root;
    uint64_t attribute_root;
    uint64_t protection_policy_id;
    uint64_t generation;
    uint8_t checksum[32];
} novafs_object_record_t;

typedef struct novafs_directory_item {
    uint64_t parent_id;
    uint64_t name_hash;
    uint64_t object_id;
    uint32_t object_type;
    uint16_t name_length;
    uint16_t flags;
    uint8_t name[256];
} novafs_directory_item_t;

typedef struct novafs_extent_record {
    uint64_t logical_offset;
    uint64_t physical_block;
    uint64_t block_count;
    uint64_t uncompressed_size;
    uint64_t stored_size;
    uint64_t stripe_id;
    uint64_t generation;
    uint32_t flags;
    uint16_t compression_type;
    uint16_t protection_fragment_index;
} novafs_extent_record_t;

typedef struct novafs_extent_item {
    uint64_t object_id;
    novafs_extent_record_t extent;
} novafs_extent_item_t;

/* Kernel-API (novafs_api in novafs32.inc) */
typedef struct NovaFsApiV1 {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t ResolvePathEntry;
    uint32_t LookupEntry;
    uint32_t ReadEntry;
    uint32_t WriteEntry;
    uint32_t CreateEntry;
    uint32_t ChangeBeginEntry;
    uint32_t ChangeCommitEntry;
    uint32_t MountedAddress;
    uint32_t ReadOnlyAddress;
    uint32_t Reserved;
} NovaFsApiV1;

_Static_assert(sizeof(novafs_superblock_t) == NOVAFS_BLOCK_SIZE, "NovaFS superblock size changed");
_Static_assert(offsetof(novafs_superblock_t, generation) == 40, "NovaFS superblock generation offset");
_Static_assert(offsetof(novafs_superblock_t, object_tree_block) == 56, "NovaFS object tree offset");
_Static_assert(offsetof(novafs_superblock_t, free_space_tree_block) == 96, "NovaFS free space offset");
_Static_assert(offsetof(novafs_superblock_t, incompat_flags) == 128, "NovaFS incompat offset");
_Static_assert(offsetof(novafs_superblock_t, filesystem_uuid) == 144, "NovaFS uuid offset");
_Static_assert(offsetof(novafs_superblock_t, checksum) == 352, "NovaFS checksum offset");
_Static_assert(offsetof(novafs_superblock_t, state) == 388, "NovaFS state offset");
_Static_assert(offsetof(novafs_superblock_t, next_object_id) == 392, "NovaFS next object offset");
_Static_assert(offsetof(novafs_superblock_t, bitmap_crc32c) == 424, "NovaFS bitmap crc offset");
_Static_assert(offsetof(novafs_superblock_t, mount_count) == 432, "NovaFS mount count offset");
_Static_assert(sizeof(novafs_tree_node_header_t) == NOVAFS_NODE_HEADER_SIZE, "NovaFS node header size changed");
_Static_assert(offsetof(novafs_tree_node_header_t, checksum) == 40, "NovaFS node checksum offset");
_Static_assert(sizeof(novafs_inner_item_t) == 24, "NovaFS inner item size changed");
_Static_assert(sizeof(novafs_object_record_t) == 152, "NovaFS object record size changed");
_Static_assert(offsetof(novafs_object_record_t, checksum) == 120, "NovaFS object checksum offset");
_Static_assert(sizeof(novafs_directory_item_t) == 288, "NovaFS directory item size changed");
_Static_assert(offsetof(novafs_directory_item_t, name) == 32, "NovaFS directory name offset");
_Static_assert(sizeof(novafs_extent_record_t) == 64, "NovaFS extent record size changed");
_Static_assert(sizeof(novafs_extent_item_t) == 72, "NovaFS extent item size changed");
_Static_assert(sizeof(NovaFsApiV1) == 48, "NovaFsApiV1 ABI size changed");

#endif
