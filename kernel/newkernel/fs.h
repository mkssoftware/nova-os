/* NovaOS C-Kernel - Filesystem-Subsystem (Port aus fs32.inc,
   NPSPEC-FILESYSTEM-* und verwandte NPSPEC-FSSTORAGE/NPSPEC-STORAGE/
   NPSPEC-POLICY-Spezifikationen). */
#ifndef NOVA_FS_H
#define NOVA_FS_H

#include <stdint.h>

/* ObjectID (NPSPEC-FILESYSTEM-OBJECTID-0001): [volume_id][generation][inode_lo][inode_hi]. */
typedef struct {
    uint32_t volume_id;
    uint32_t generation;
    uint32_t inode_seq_lo;
    uint32_t inode_seq_hi;
} fs_objid_t;

#define OBJID_NULL_VOLUME   0xFFFFFFFFu
#define OBJID_ROOT_INODE_LO 1u
#define OBJID_ROOT_INODE_HI 0u

/* Semantische Typen (NPSPEC-FILESYSTEM-SEMANTIC-0001). */
#define FSOBJ_TYPE_UNKNOWN   0u
#define FSOBJ_TYPE_FILE      1u
#define FSOBJ_TYPE_DIR        2u
#define FSOBJ_TYPE_SYMLINK   3u
#define FSOBJ_TYPE_DEVICE    4u
#define FSOBJ_TYPE_PIPE      5u
#define FSOBJ_TYPE_SOCKET    6u
#define FSOBJ_TYPE_VOLUME    7u
#define FSOBJ_TYPE_WORKSPACE 8u
#define FSOBJ_TYPE_TEMP      9u

/* Berechtigungs-Bits (NPSPEC-FILESYSTEM-PERMISSION-0001). */
#define FSPERM_R 0x04u
#define FSPERM_W 0x02u
#define FSPERM_X 0x01u
#define FSPERM_OWNER_SHIFT 6u
#define FSPERM_GROUP_SHIFT 3u
#define FSPERM_OTHER_SHIFT 0u

#define FSPERM_0755 0x1EDu
#define FSPERM_0644 0x1A4u
#define FSPERM_0600 0x180u

/* Namespace-IDs (NPSPEC-FILESYSTEM-NAMESPACE-0001/2). */
#define FS_NS_ROOT      0u
#define FS_NS_SYSTEM    1u
#define FS_NS_USER      2u
#define FS_NS_TEMP      3u
#define FS_NS_WORKSPACE 4u
#define FS_NS_PROGRAM   5u
#define FS_NS_DEVICE    6u

/* Volume-Flags (NPSPEC-FSSTORAGE-VOLUME-0001). */
#define VOL_FLAG_MOUNTED   0x01u
#define VOL_FLAG_READONLY  0x02u
#define VOL_FLAG_REMOVABLE 0x04u
#define VOL_FLAG_ENCRYPTED 0x08u
#define VOL_FLAG_REMOTE    0x10u
#define VOL_FLAG_TRUSTED   0x20u
#define VOL_FLAG_SYSTEM    0x40u

#define VOL_CAPACITY 4u
#define NS_CAPACITY  8u
#define TX_CAPACITY  4u

/* Trust-Level (NPSPEC-TRUST-ROOT-0001). */
#define TRUST_NONE   0u
#define TRUST_SYSTEM 1u
#define TRUST_USER   2u
#define TRUST_REMOTE 3u

/* Policy-Flags (NPSPEC-POLICY-*-0001). */
#define FSPOLICY_ALLOW_READ  0x01u
#define FSPOLICY_ALLOW_WRITE 0x02u
#define FSPOLICY_ALLOW_EXEC  0x04u
#define FSPOLICY_SYS_WRITE   0x08u
#define FSPOLICY_TEMP_CLEAR  0x10u
#define FSPOLICY_WS_ISOLATED 0x20u

/* Legt Root-Volume und den Standard-Namespace-Baum (system/user/temp/
   workspace/program/device) an. */
void fs_initialize(void);
int fs_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int fs_vol_register(uint32_t vol_id, uint32_t flags, uint32_t ns_id);

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int fs_ns_register(uint32_t ns_id, uint32_t parent_ns, uint32_t policy);

/* 0 = Erfolg, -1 wenn vol_id unbekannt. */
int fs_vol_mount(uint32_t vol_id, uint32_t ns_id, uint32_t extra_flags);

/* 0 = Erfolg (out_vol_id gesetzt), -1 wenn kein Volume fuer ns_id gemountet. */
int fs_vol_lookup(uint32_t ns_id, uint32_t *out_vol_id);

/* Erzeugt eine neue ObjectID (NPSPEC-FILESYSTEM-OBJECTID-0001). */
void fs_objid_make(uint32_t volume_id, uint32_t inode_lo, fs_objid_t *out_objid);

/* 0 = erlaubt, -1 = verweigert/unbekannt. */
int fs_ns_check_policy(uint32_t ns_id, uint32_t required_policy);

/* 0 = Erfolg (out_tx_id gesetzt), -1 wenn Tabelle voll. */
int fs_tx_begin(uint32_t volume_id, uint32_t *out_tx_id);

/* 0 = Erfolg, -1 wenn tx_id unbekannt. */
int fs_tx_commit(uint32_t tx_id);
int fs_tx_rollback(uint32_t tx_id);

/* Leert alle Temp-Namespace-Eintraege (NPSPEC-TEMP-*-0001). */
void fs_temp_clear(void);

#endif
