/* NovaOS C-Kernel - NovaFS On-Disk-Dateisystem (siehe novafs.h fuer
   Scope/Batch-Stand). */
#include "novafs.h"
#include "block.h"
#include "pmm.h"

#define NOVAFS_SECTORS_PER_BLOCK 8u /* 4096 / 512 */
#define NOVAFS_MAX_BITMAP_BLOCKS 4u
#define NOVAFS_JOURNAL_BLOCKS 17u /* NOVAFS_JOURNAL_SLOTS(16) + 1 */
#define NOVAFS_INCOMPAT_KNOWN 3u
#define NOVAFS_STATE_CLEAN 0u
#define NOVAFS_FIRST_DYNAMIC_ID 256u

/* Superblock-Feldoffsets (siehe novafs32.inc). */
#define NFS_SB_VMAJ           8u
#define NFS_SB_BLOCK_SIZE     12u
#define NFS_SB_FS_SIZE        16u
#define NFS_SB_TOTAL          24u
#define NFS_SB_AVAILABLE      32u
#define NFS_SB_GENERATION     40u
#define NFS_SB_OBJECT_TREE    56u
#define NFS_SB_DIRECTORY_TREE 64u
#define NFS_SB_EXTENT_TREE    72u
#define NFS_SB_FREE_SPACE     96u
#define NFS_SB_JOURNAL_BLOCK  112u
#define NFS_SB_INCOMPAT       128u
#define NFS_SB_ROCOMPAT       136u
#define NFS_SB_CHECKSUM       352u
#define NFS_SB_STATE          388u
#define NFS_SB_NEXT_OBJECT    392u
#define NFS_SB_BITMAP_BLOCKS  400u
#define NFS_SB_BACKUP_BLOCK   416u
#define NFS_SB_BITMAP_CRC     424u

static uint32_t nfs_sb_page = 0;
static uint32_t nfs_bitmap_pages[NOVAFS_MAX_BITMAP_BLOCKS] = {0};

static int nfs_mounted = 0;
static int nfs_readonly = 1;
static int nfs_used_backup = 0;
static uint32_t nfs_total_blocks_value = 0;
static uint32_t nfs_bitmap_blocks_value = 0;

/* Fuer novafs_read_block/novafs_write_block (Batch 4: B+-Tree-Kern) -
   das zuletzt per novafs_probe_volume() erfolgreich sondierte Geraet. */
static void *nfs_record = 0;
static uint32_t nfs_part_first_lba = 0;

static uint32_t crc32c_update(uint32_t crc, const uint8_t *data, uint32_t length) {
    for (uint32_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; ++bit) {
            crc = (crc >> 1) ^ ((crc & 1u) ? 0x82F63B78u : 0u);
        }
    }
    return crc;
}

/* Siegelt ein 32-Byte-Pruefsummenfeld bei Offset off (Bytes 0-3 = CRC32C
   ueber den GESAMTEN Puffer mit genau diesem Feld genullt, Bytes 4-31
   bleiben dauerhaft 0) - nur fuer den Bau des Selbsttest-Fixtures benutzt,
   da echtes Schreiben erst mit dem Journal-/Write-Batch kommt. */
static void novafs_seal(uint8_t *buf, uint32_t len, uint32_t off) {
    for (uint32_t i = 0; i < 32u; ++i) buf[off + i] = 0;
    *(uint32_t *)(buf + off) = ~crc32c_update(0xFFFFFFFFu, buf, len);
}

/* 0 = gueltig (Bytes 4..31 des Feldes sind 0 UND die CRC32C stimmt). */
static int novafs_verify(uint8_t *buf, uint32_t len, uint32_t off) {
    for (uint32_t i = 4u; i < 32u; ++i) {
        if (buf[off + i] != 0) return -1;
    }
    uint32_t saved = *(uint32_t *)(buf + off);
    *(uint32_t *)(buf + off) = 0;
    uint32_t computed = ~crc32c_update(0xFFFFFFFFu, buf, len);
    *(uint32_t *)(buf + off) = saved;
    return (computed == saved) ? 0 : -1;
}

/* CF-Aequivalent: 0 = gueltiger Superblock-Inhalt fuer eine Partition mit
   part_blocks Bloecken (Port aus novafs_superblock_validate - OHNE die
   erweiterten mount-zeitigen Pruefungen wie Pruefsummen-Array-Region, die
   erst mit einem spaeteren Batch kommen). */
static int novafs_validate_superblock(uint8_t *sb, uint32_t part_blocks) {
    static const uint8_t magic[8] = {'N', 'O', 'V', 'A', 'F', 'S', 1, 0};
    for (uint32_t i = 0; i < 8u; ++i) {
        if (sb[i] != magic[i]) return -1;
    }
    if (*(const uint16_t *)(sb + NFS_SB_VMAJ) != 1u) return -1;
    if (*(const uint32_t *)(sb + NFS_SB_BLOCK_SIZE) != NOVAFS_BLOCK_SIZE) return -1;
    if (novafs_verify(sb, NOVAFS_BLOCK_SIZE, NFS_SB_CHECKSUM) != 0) return -1;

    if (*(const uint32_t *)(sb + NFS_SB_TOTAL + 4) != 0u) return -1;
    uint32_t total = *(const uint32_t *)(sb + NFS_SB_TOTAL);
    if (total < 64u || total > part_blocks) return -1;

    if (*(const uint32_t *)(sb + NFS_SB_FS_SIZE) != (total << 12)) return -1;
    if (*(const uint32_t *)(sb + NFS_SB_FS_SIZE + 4) != (total >> 20)) return -1;

    if (*(const uint32_t *)(sb + NFS_SB_BACKUP_BLOCK) != total - 1u) return -1;
    if (*(const uint32_t *)(sb + NFS_SB_BACKUP_BLOCK + 4) != 0u) return -1;

    uint32_t bitmap_blocks = (total + 32767u) >> 15;
    if (*(const uint32_t *)(sb + NFS_SB_BITMAP_BLOCKS) != bitmap_blocks) return -1;
    if (*(const uint32_t *)(sb + NFS_SB_BITMAP_BLOCKS + 4) != 0u) return -1;
    if (bitmap_blocks > NOVAFS_MAX_BITMAP_BLOCKS) return -1;

    uint32_t journal_block = bitmap_blocks + 2u;
    if (*(const uint32_t *)(sb + NFS_SB_JOURNAL_BLOCK) != journal_block) return -1;
    if (*(const uint32_t *)(sb + NFS_SB_JOURNAL_BLOCK + 4) != 0u) return -1;

    if (*(const uint32_t *)(sb + NFS_SB_FREE_SPACE) != 2u) return -1;
    if (*(const uint32_t *)(sb + NFS_SB_FREE_SPACE + 4) != 0u) return -1;

    uint32_t first_free = journal_block + NOVAFS_JOURNAL_BLOCKS;
    uint32_t last_valid_exclusive = total - 1u;
    for (uint32_t off = NFS_SB_OBJECT_TREE; off <= NFS_SB_EXTENT_TREE; off += 8u) {
        if (*(const uint32_t *)(sb + off + 4) != 0u) return -1;
        uint32_t root = *(const uint32_t *)(sb + off);
        if (root < first_free || root >= last_valid_exclusive) return -1;
    }

    if (*(const uint32_t *)(sb + NFS_SB_NEXT_OBJECT + 4) != 0u) return -1;
    if (*(const uint32_t *)(sb + NFS_SB_NEXT_OBJECT) < NOVAFS_FIRST_DYNAMIC_ID) return -1;

    return 0;
}

static int novafs_block_used(uint32_t block) {
    uint32_t page_index = block >> 15;
    if (page_index >= nfs_bitmap_blocks_value) return 0;
    uint32_t bit = block & 0x7FFFu;
    const uint32_t *words = (const uint32_t *)(uintptr_t)nfs_bitmap_pages[page_index];
    return (int)((words[bit >> 5] >> (bit & 31u)) & 1u);
}

int novafs_probe_volume(void *record, uint32_t part_first_lba, uint32_t part_sector_count) {
    nfs_mounted = 0;
    nfs_readonly = 1;
    nfs_used_backup = 0;

    uint32_t part_blocks = part_sector_count >> 3;
    if (part_blocks < 64u) return -1;

    uint32_t numa;
    uint32_t primary_page = pmm_alloc_page(&numa);
    uint32_t backup_page = pmm_alloc_page(&numa);
    if (primary_page == 0 || backup_page == 0) {
        if (primary_page) pmm_free_page(primary_page);
        if (backup_page) pmm_free_page(backup_page);
        return -1;
    }
    uint8_t *primary = (uint8_t *)(uintptr_t)primary_page;
    uint8_t *backup = (uint8_t *)(uintptr_t)backup_page;

    int primary_valid = 0, backup_valid = 0;

    if (block_io_submit(record, part_first_lba + 1u * NOVAFS_SECTORS_PER_BLOCK, 0u,
                         NOVAFS_SECTORS_PER_BLOCK, primary, BLK_IO_READ) == 0) {
        if (novafs_validate_superblock(primary, part_blocks) == 0) primary_valid = 1;
    }

    /* Backup liegt am Ende der PARTITION (part_blocks-1); validate prueft
       zusaetzlich, dass der dort gefundene TOTAL-Wert exakt auf diesen
       Block zeigt (Backup-Superblock eines ANDEREN, kleineren Volumes auf
       derselben Partition darf nicht uebernommen werden). */
    if (block_io_submit(record, part_first_lba + (part_blocks - 1u) * NOVAFS_SECTORS_PER_BLOCK,
                         0u, NOVAFS_SECTORS_PER_BLOCK, backup, BLK_IO_READ) == 0) {
        if (novafs_validate_superblock(backup, part_blocks) == 0 &&
            *(const uint32_t *)(backup + NFS_SB_TOTAL) == part_blocks) {
            backup_valid = 1;
        }
    }

    uint8_t *selected;
    int used_backup;
    if (primary_valid && backup_valid) {
        uint32_t pg_hi = *(const uint32_t *)(primary + NFS_SB_GENERATION + 4);
        uint32_t pg_lo = *(const uint32_t *)(primary + NFS_SB_GENERATION);
        uint32_t bg_hi = *(const uint32_t *)(backup + NFS_SB_GENERATION + 4);
        uint32_t bg_lo = *(const uint32_t *)(backup + NFS_SB_GENERATION);
        if (bg_hi > pg_hi || (bg_hi == pg_hi && bg_lo > pg_lo)) {
            selected = backup;
            used_backup = 1;
        } else {
            selected = primary;
            used_backup = 0;
        }
    } else if (primary_valid) {
        selected = primary;
        used_backup = 0;
    } else if (backup_valid) {
        selected = backup;
        used_backup = 1;
    } else {
        pmm_free_page(primary_page);
        pmm_free_page(backup_page);
        return -1;
    }

    if (nfs_sb_page == 0) {
        nfs_sb_page = pmm_alloc_page(&numa);
        if (nfs_sb_page == 0) {
            pmm_free_page(primary_page);
            pmm_free_page(backup_page);
            return -1;
        }
    }
    uint8_t *sb = (uint8_t *)(uintptr_t)nfs_sb_page;
    for (uint32_t i = 0; i < NOVAFS_BLOCK_SIZE; ++i) sb[i] = selected[i];
    pmm_free_page(primary_page);
    pmm_free_page(backup_page);

    if (*(const uint32_t *)(sb + NFS_SB_INCOMPAT + 4) != 0u) return -1;
    if (*(const uint32_t *)(sb + NFS_SB_INCOMPAT) & ~NOVAFS_INCOMPAT_KNOWN) return -1;

    if (*(const uint32_t *)(sb + NFS_SB_STATE) != NOVAFS_STATE_CLEAN) {
        /* DIRTY: Journal-Wiederherstellung (NPSPEC-NOVAFS-ONDISK-0001 §9)
           folgt erst mit dem Journal-/Write-Batch - bis dahin gilt ein
           DIRTY-Volume bewusst als nicht nutzbar statt blind weiter-
           zumachen. */
        return -1;
    }

    nfs_readonly = ((*(const uint32_t *)(sb + NFS_SB_ROCOMPAT) |
                      *(const uint32_t *)(sb + NFS_SB_ROCOMPAT + 4)) != 0u) ? 1 : 0;

    uint32_t total = *(const uint32_t *)(sb + NFS_SB_TOTAL);
    uint32_t bitmap_blocks = *(const uint32_t *)(sb + NFS_SB_BITMAP_BLOCKS);

    uint32_t crc = 0xFFFFFFFFu;
    for (uint32_t b = 0; b < bitmap_blocks; ++b) {
        if (nfs_bitmap_pages[b] == 0) {
            nfs_bitmap_pages[b] = pmm_alloc_page(&numa);
            if (nfs_bitmap_pages[b] == 0) return -1;
        }
        uint8_t *bmp = (uint8_t *)(uintptr_t)nfs_bitmap_pages[b];
        if (block_io_submit(record, part_first_lba + (2u + b) * NOVAFS_SECTORS_PER_BLOCK, 0u,
                             NOVAFS_SECTORS_PER_BLOCK, bmp, BLK_IO_READ) != 0) {
            return -1;
        }
        crc = crc32c_update(crc, bmp, NOVAFS_BLOCK_SIZE);
    }
    crc = ~crc;
    if (crc != *(const uint32_t *)(sb + NFS_SB_BITMAP_CRC)) return -1;

    /* nfs_bitmap_blocks_value MUSS vor novafs_block_used() gesetzt sein
       (das liest es fuer die Page-Index-Grenzpruefung). */
    nfs_bitmap_blocks_value = bitmap_blocks;
    nfs_total_blocks_value = total;

    if (!novafs_block_used(1u)) return -1;
    if (!novafs_block_used(total - 1u)) return -1;

    nfs_record = record;
    nfs_part_first_lba = part_first_lba;
    nfs_used_backup = used_backup;
    nfs_mounted = 1;
    return 0;
}

uint32_t novafs_total_blocks(void) {
    return nfs_total_blocks_value;
}

uint32_t novafs_bitmap_blocks(void) {
    return nfs_bitmap_blocks_value;
}

uint32_t novafs_generation_lo(void) {
    if (nfs_sb_page == 0) return 0;
    return *(const uint32_t *)((const uint8_t *)(uintptr_t)nfs_sb_page + NFS_SB_GENERATION);
}

int novafs_is_readonly(void) {
    return nfs_readonly;
}

int novafs_is_used_backup(void) {
    return nfs_used_backup;
}

/* ===================================================================== */
/* B+-Tree-Kern (Batch 4): Node-I/O, Cursor, Item-Vergleich, einfaches    */
/* Einfuegen OHNE Split/Merge (folgt erst mit einem spaeteren Batch).     */
/* ===================================================================== */

#define NOVAFS_NODE_MAGIC  0x4552544Eu /* "NTRE" */
#define NOVAFS_NODE_HEADER 80u
#define NOVAFS_INNER_ITEM  24u
#define NOVAFS_INNER_MAX   167u
#define NFS_MAX_HEIGHT     4u

#define NFS_N_MAGIC      0u
#define NFS_N_LEVEL      4u
#define NFS_N_COUNT      6u
#define NFS_N_TREE       8u
#define NFS_N_BLOCK      16u
#define NFS_N_PARENT     24u
#define NFS_N_CHECKSUM   40u
#define NFS_N_ITEM_SIZE  72u
#define NFS_N_MAX_ITEMS  74u
#define NFS_N_KEY_SIZE   76u

const novafs_tree_t novafs_tree_object    = {2u, 152u, 26u, 8u,  NFS_SB_OBJECT_TREE};
const novafs_tree_t novafs_tree_directory = {3u, 288u, 13u, 16u, NFS_SB_DIRECTORY_TREE};
const novafs_tree_t novafs_tree_extent    = {4u, 72u,  55u, 16u, NFS_SB_EXTENT_TREE};

typedef struct {
    uint32_t primary_lo, primary_hi;
    uint32_t secondary_lo, secondary_hi;
} novafs_key_internal_t;

static novafs_key_internal_t nfs_key;

/* Je eine Seite pro Pfadtiefe (0=Wurzel .. NFS_MAX_HEIGHT-1=Blatt), analog
   zu nfs_root_page/nfs_inner1_page/nfs_inner2_page/nfs_leaf_page im alten
   ASM - hier als Array statt vier Einzelvariablen. */
static uint32_t nfs_level_pages[NFS_MAX_HEIGHT] = {0};
static uint32_t nfs_path_blocks[NFS_MAX_HEIGHT] = {0};
static uint32_t nfs_path_indices[NFS_MAX_HEIGHT] = {0};
static uint32_t nfs_path_depth = 0;
static const novafs_tree_t *nfs_cur_tree = 0;
static uint32_t nfs_cur_index = 0;
static uint32_t nfs_cur_height = 0;

static int novafs_read_block(uint32_t block, uint8_t *buf) {
    if (nfs_record == 0 || block >= nfs_total_blocks_value) return -1;
    uint32_t lba = nfs_part_first_lba + block * NOVAFS_SECTORS_PER_BLOCK;
    return block_io_submit(nfs_record, lba, 0u, NOVAFS_SECTORS_PER_BLOCK, buf, BLK_IO_READ);
}

static int novafs_write_block(uint32_t block, uint8_t *buf) {
    if (nfs_record == 0 || block >= nfs_total_blocks_value) return -1;
    uint32_t lba = nfs_part_first_lba + block * NOVAFS_SECTORS_PER_BLOCK;
    return block_io_submit(nfs_record, lba, 0u, NOVAFS_SECTORS_PER_BLOCK, buf, BLK_IO_WRITE);
}

/* EDI-Aequivalent: Seite fuer Pfadtiefe depth (allokiert lazy, bleibt wie
   nfs_sb_page ueber mehrere Aufrufe hinweg bestehen). 0 bei PMM-Erschoepfung
   oder depth >= NFS_MAX_HEIGHT. */
static uint8_t *novafs_level_page(uint32_t depth) {
    if (depth >= NFS_MAX_HEIGHT) return 0;
    if (nfs_level_pages[depth] == 0) {
        uint32_t numa;
        nfs_level_pages[depth] = pmm_alloc_page(&numa);
    }
    return (uint8_t *)(uintptr_t)nfs_level_pages[depth];
}

void novafs_key_set(uint32_t primary, uint32_t secondary) {
    nfs_key.primary_lo = primary;
    nfs_key.primary_hi = 0;
    nfs_key.secondary_lo = secondary;
    nfs_key.secondary_hi = 0;
}

/* 0 = gleich, 1 = Item groesser als nfs_key, -1 = Item kleiner. */
static int novafs_compare_item(const uint8_t *item, uint32_t key_size) {
    uint32_t item_hi = *(const uint32_t *)(item + 4);
    if (item_hi > nfs_key.primary_hi) return 1;
    if (item_hi < nfs_key.primary_hi) return -1;
    uint32_t item_lo = *(const uint32_t *)(item + 0);
    if (item_lo > nfs_key.primary_lo) return 1;
    if (item_lo < nfs_key.primary_lo) return -1;
    if (key_size != 16u) {
        if (nfs_key.secondary_lo != 0u || nfs_key.secondary_hi != 0u) return -1;
        return 0;
    }
    uint32_t item_shi = *(const uint32_t *)(item + 12);
    if (item_shi > nfs_key.secondary_hi) return 1;
    if (item_shi < nfs_key.secondary_hi) return -1;
    uint32_t item_slo = *(const uint32_t *)(item + 8);
    if (item_slo > nfs_key.secondary_lo) return 1;
    if (item_slo < nfs_key.secondary_lo) return -1;
    return 0;
}

/* Innere Items haben IMMER einen 16-Byte-Schluessel, unabhaengig vom
   Schluesseltyp des Baums. */
static int novafs_compare_inner(const uint8_t *item) {
    uint32_t item_hi = *(const uint32_t *)(item + 4);
    if (item_hi > nfs_key.primary_hi) return 1;
    if (item_hi < nfs_key.primary_hi) return -1;
    uint32_t item_lo = *(const uint32_t *)(item + 0);
    if (item_lo > nfs_key.primary_lo) return 1;
    if (item_lo < nfs_key.primary_lo) return -1;
    uint32_t item_shi = *(const uint32_t *)(item + 12);
    if (item_shi > nfs_key.secondary_hi) return 1;
    if (item_shi < nfs_key.secondary_hi) return -1;
    uint32_t item_slo = *(const uint32_t *)(item + 8);
    if (item_slo > nfs_key.secondary_lo) return 1;
    if (item_slo < nfs_key.secondary_lo) return -1;
    return 0;
}

/* 0 = Erfolg (Knoten in buf, konsistent mit tree/block/level). Port aus
   novafs_node_load - OHNE die Journal-Undo-Pfade (kommen erst mit dem
   Write-Batch, dieser Knoten-Loader liest nur). expected_level: -1 = egal
   (Wurzel, Hoehe unbekannt), sonst 0=Blatt/1=innerer Knoten erwartet. */
static int novafs_node_load(uint32_t block, uint8_t *buf, const novafs_tree_t *tree,
                             int32_t expected_level) {
    if (block < nfs_bitmap_blocks_value + 2u) return -1;
    if (block >= nfs_total_blocks_value - 1u) return -1;
    if (novafs_read_block(block, buf) != 0) return -1;

    if (*(const uint32_t *)(buf + NFS_N_MAGIC) != NOVAFS_NODE_MAGIC) return -1;
    if (novafs_verify(buf, NOVAFS_BLOCK_SIZE, NFS_N_CHECKSUM) != 0) return -1;
    if (*(const uint32_t *)(buf + NFS_N_TREE) != tree->id) return -1;
    if (*(const uint32_t *)(buf + NFS_N_TREE + 4) != 0u) return -1;
    if (*(const uint32_t *)(buf + NFS_N_BLOCK) != block) return -1;
    if (*(const uint32_t *)(buf + NFS_N_BLOCK + 4) != 0u) return -1;

    uint16_t level = *(const uint16_t *)(buf + NFS_N_LEVEL);
    if (level > 1u) return -1;
    if (expected_level >= 0 && (uint32_t)expected_level != level) return -1;

    uint16_t key_size = *(const uint16_t *)(buf + NFS_N_KEY_SIZE);
    if (key_size != tree->key_size) return -1;

    uint16_t item_size, max_items;
    if (level == 0u) {
        item_size = (uint16_t)tree->item_size;
        max_items = (uint16_t)tree->max_items;
    } else {
        item_size = NOVAFS_INNER_ITEM;
        max_items = NOVAFS_INNER_MAX;
    }
    if (*(const uint16_t *)(buf + NFS_N_ITEM_SIZE) != item_size) return -1;
    if (*(const uint16_t *)(buf + NFS_N_MAX_ITEMS) != max_items) return -1;
    if (*(const uint16_t *)(buf + NFS_N_COUNT) > max_items) return -1;

    return 0;
}

/* Versiegelt und schreibt den Knoten an seine eigene Blocknummer (Port aus
   novafs_node_store - OHNE Generation-Fortschreibung/Journal-Eintrag, die
   erst mit dem Write-Batch kommen, da es hier noch keine laufende
   Aenderung/Transaktion gibt). */
static int novafs_node_store(uint8_t *buf) {
    novafs_seal(buf, NOVAFS_BLOCK_SIZE, NFS_N_CHECKSUM);
    uint32_t block = *(const uint32_t *)(buf + NFS_N_BLOCK);
    return novafs_write_block(block, buf);
}

/* Initialisiert einen leeren Knoten (Port aus novafs_node_init). */
static void novafs_node_init(uint8_t *buf, const novafs_tree_t *tree, uint32_t block,
                              uint32_t level, uint32_t parent_block) {
    for (uint32_t i = 0; i < NOVAFS_BLOCK_SIZE; ++i) buf[i] = 0;
    *(uint32_t *)(buf + NFS_N_MAGIC) = NOVAFS_NODE_MAGIC;
    *(uint16_t *)(buf + NFS_N_LEVEL) = (uint16_t)level;
    *(uint32_t *)(buf + NFS_N_TREE) = tree->id;
    *(uint32_t *)(buf + NFS_N_BLOCK) = block;
    *(uint32_t *)(buf + NFS_N_PARENT) = parent_block;
    *(uint16_t *)(buf + NFS_N_KEY_SIZE) = (uint16_t)tree->key_size;
    if (level == 0u) {
        *(uint16_t *)(buf + NFS_N_ITEM_SIZE) = (uint16_t)tree->item_size;
        *(uint16_t *)(buf + NFS_N_MAX_ITEMS) = (uint16_t)tree->max_items;
    } else {
        *(uint16_t *)(buf + NFS_N_ITEM_SIZE) = (uint16_t)NOVAFS_INNER_ITEM;
        *(uint16_t *)(buf + NFS_N_MAX_ITEMS) = (uint16_t)NOVAFS_INNER_MAX;
    }
}

/* Faehrt mit dem Cursor durch beliebig viele innere Knotenebenen (bis
   NFS_MAX_HEIGHT) hinab und positioniert ihn auf das erste Blatt-Item >=
   dem zuvor per novafs_key_set() gesetzten Schluessel (oder das Ende des
   Blatts, wenn alle Items kleiner sind - novafs_cursor_advance() steigt
   dann automatisch ins naechste Geschwisterblatt auf). 0 = Erfolg. */
int novafs_cursor_seek(const novafs_tree_t *tree) {
    nfs_cur_tree = tree;

    uint8_t *root = novafs_level_page(0u);
    if (root == 0) return -1;
    uint32_t root_block = *(const uint32_t *)((const uint8_t *)(uintptr_t)nfs_sb_page + tree->sb_root_offset);
    nfs_path_blocks[0] = root_block;
    if (novafs_node_load(root_block, root, tree, -1) != 0) return -1;
    if (*(const uint32_t *)(root + NFS_N_PARENT) != 0u ||
        *(const uint32_t *)(root + NFS_N_PARENT + 4) != 0u) {
        return -1; /* Wurzel darf keinen Parent haben */
    }

    uint32_t depth = 0;
    uint8_t *node = root;
    while (*(const uint16_t *)(node + NFS_N_LEVEL) != 0u) {
        uint32_t count = *(const uint16_t *)(node + NFS_N_COUNT);
        if (count == 0u) return -1;

        /* Groessten Separator-Index suchen, dessen Schluessel <= nfs_key
           ist (Port aus novafs_cursor_seek's .sep_scan: weiter vorruecken
           SOLANGE der Separator KLEINER als der Suchschluessel ist, beim
           ersten >= stehenbleiben). */
        uint32_t child_index = 0;
        for (uint32_t i = 1; i < count; ++i) {
            const uint8_t *sep = node + NOVAFS_NODE_HEADER + i * NOVAFS_INNER_ITEM;
            if (novafs_compare_inner(sep) == -1) {
                child_index = i;
            } else {
                break;
            }
        }
        nfs_path_indices[depth] = child_index;

        const uint8_t *entry = node + NOVAFS_NODE_HEADER + child_index * NOVAFS_INNER_ITEM;
        if (*(const uint32_t *)(entry + 20) != 0u) return -1;
        uint32_t child_block = *(const uint32_t *)(entry + 16);

        if (depth + 1u >= NFS_MAX_HEIGHT) return -1;
        ++depth;
        nfs_path_blocks[depth] = child_block;
        uint8_t *child = novafs_level_page(depth);
        if (child == 0) return -1;
        if (novafs_node_load(child_block, child, tree, -1) != 0) return -1;
        node = child;
    }

    nfs_path_depth = depth;
    nfs_cur_height = depth + 1u;

    uint32_t count = *(const uint16_t *)(node + NFS_N_COUNT);
    uint32_t index = 0;
    for (; index < count; ++index) {
        const uint8_t *item = node + NOVAFS_NODE_HEADER + index * tree->item_size;
        if (novafs_compare_item(item, tree->key_size) != -1) break;
    }
    nfs_cur_index = index;
    return 0;
}

uint8_t *novafs_cursor_item_ptr(void) {
    if (nfs_cur_tree == 0) return 0;
    uint8_t *leaf = novafs_level_page(nfs_path_depth);
    if (leaf == 0) return 0;
    uint32_t count = *(const uint16_t *)(leaf + NFS_N_COUNT);
    if (nfs_cur_index >= count) return 0;
    return leaf + NOVAFS_NODE_HEADER + nfs_cur_index * nfs_cur_tree->item_size;
}

/* Steigt den Pfad-Stack hoch und laedt das naechste Geschwisterblatt (Port
   aus novafs_cursor_advance_leaf). -1 = kein weiteres Blatt vorhanden. */
static int novafs_cursor_advance_leaf(void) {
    if (nfs_path_depth == 0u) return -1; /* Hoehe 1: kein naechstes Blatt */

    uint32_t depth = nfs_path_depth;
    for (;;) {
        if (depth == 0u) return -1;
        --depth;
        uint8_t *inner = novafs_level_page(depth);
        if (inner == 0) return -1;
        uint32_t count = *(const uint16_t *)(inner + NFS_N_COUNT);
        uint32_t next_index = nfs_path_indices[depth] + 1u;
        if (next_index < count) {
            nfs_path_indices[depth] = next_index;
            const uint8_t *entry = inner + NOVAFS_NODE_HEADER + next_index * NOVAFS_INNER_ITEM;
            if (*(const uint32_t *)(entry + 20) != 0u) return -1;
            uint32_t block = *(const uint32_t *)(entry + 16);

            uint32_t load_depth = depth + 1u;
            for (;;) {
                nfs_path_blocks[load_depth] = block;
                uint8_t *child = novafs_level_page(load_depth);
                if (child == 0) return -1;
                if (novafs_node_load(block, child, nfs_cur_tree, -1) != 0) return -1;
                if (*(const uint16_t *)(child + NFS_N_LEVEL) == 0u) {
                    nfs_path_depth = load_depth;
                    nfs_cur_height = load_depth + 1u;
                    return 0;
                }
                nfs_path_indices[load_depth] = 0;
                const uint8_t *first = child + NOVAFS_NODE_HEADER;
                if (*(const uint32_t *)(first + 20) != 0u) return -1;
                block = *(const uint32_t *)(first + 16);
                if (load_depth + 1u >= NFS_MAX_HEIGHT) return -1;
                ++load_depth;
            }
        }
        /* kein naechstes Kind auf dieser Ebene: eine Ebene hoeher versuchen */
    }
}

int novafs_cursor_advance(void) {
    if (nfs_cur_tree == 0) return -1;
    for (;;) {
        uint8_t *leaf = novafs_level_page(nfs_path_depth);
        if (leaf == 0) return -1;
        uint32_t count = *(const uint16_t *)(leaf + NFS_N_COUNT);
        if (nfs_cur_index + 1u < count) {
            ++nfs_cur_index;
            return 0;
        }
        if (novafs_cursor_advance_leaf() != 0) return -1;
        nfs_cur_index = 0;
        uint8_t *new_leaf = novafs_level_page(nfs_path_depth);
        if (new_leaf == 0) return -1;
        if (*(const uint16_t *)(new_leaf + NFS_N_COUNT) > 0u) return 0;
        /* leeres Geschwisterblatt (sollte im Normalfall nicht vorkommen) -
           weiter zum naechsten. */
    }
}

/* EDI-Aequivalent: fuegt item an Index index in node ein (Port aus
   novafs_node_insert_at, hier ohne Inline-Assembler per Byte-Kopie). */
static void novafs_node_insert_at(uint8_t *node, uint32_t index, const uint8_t *item,
                                   uint32_t item_size) {
    uint32_t count = *(const uint16_t *)(node + NFS_N_COUNT);
    uint8_t *dst = node + NOVAFS_NODE_HEADER + index * item_size;
    uint32_t moved = (count - index) * item_size;
    if (moved > 0u) {
        for (uint32_t i = moved; i > 0u; --i) {
            dst[i - 1u + item_size] = dst[i - 1u];
        }
    }
    for (uint32_t i = 0; i < item_size; ++i) dst[i] = item[i];
    *(uint16_t *)(node + NFS_N_COUNT) = (uint16_t)(count + 1u);
}

/* Fuegt item sortiert in das Blatt ein, auf das der zuletzt per
   novafs_cursor_seek() positionierte Cursor zeigt - NUR der Fall "Blatt
   hat noch Platz" (Port aus dem Nicht-Split-Zweig von novafs_tree_insert).
   Ist das Blatt voll, schlaegt die Operation sauber mit -1 fehl (KEIN
   Split - folgt erst mit einem spaeteren Batch). */
int novafs_tree_insert_simple(const novafs_tree_t *tree, const uint8_t *item) {
    if (nfs_cur_tree != tree) return -1;
    uint8_t *leaf = novafs_level_page(nfs_path_depth);
    if (leaf == 0) return -1;

    uint32_t count = *(const uint16_t *)(leaf + NFS_N_COUNT);
    if (count >= tree->max_items) return -1;

    novafs_node_insert_at(leaf, nfs_cur_index, item, tree->item_size);
    return novafs_node_store(leaf);
}

/* ===================================================================== */
/* Verzeichnisse und Pfade (Batch 5): Object-Lookup + Namensaufloesung   */
/* ueber den Directory-Baum + Pfadaufloesung (NUR Lesen - novafs_lookup  */
/* geht gegen die bestehenden tree_*-Funktionen, kein Schreiben/Anlegen, */
/* das folgt mit dem Journal-/Write-Batch).                               */
/* ===================================================================== */

#define NFS_O_ID       0u
#define NFS_O_PARENT   8u
#define NFS_O_TYPE     16u
#define NFS_O_CHECKSUM 120u
#define NFS_O_ITEM     152u

#define NFS_D_PARENT      0u
#define NFS_D_HASH        8u
#define NFS_D_OBJECT      16u
#define NFS_D_TYPE        24u
#define NFS_D_NAME_LENGTH 28u
#define NFS_D_NAME        32u
#define NFS_D_ITEM        288u

/* Sucht object_id im Object-Baum, prueft dessen eigene Item-Pruefsumme
   (NFS_O_CHECKSUM, zusaetzlich zur Knoten-Pruefsumme aus node_load) und
   kopiert das Item (NFS_O_ITEM Byte) nach out_item (Port aus
   novafs_object_find). 0 = Erfolg, -1 = nicht gefunden/beschaedigt. */
int novafs_object_find(uint32_t object_id, uint8_t *out_item) {
    novafs_key_set(object_id, 0u);
    if (novafs_cursor_seek(&novafs_tree_object) != 0) return -1;

    uint8_t *item = novafs_cursor_item_ptr();
    if (item == 0) return -1;
    if (novafs_compare_item(item, novafs_tree_object.key_size) != 0) return -1;
    if (novafs_verify(item, NFS_O_ITEM, NFS_O_CHECKSUM) != 0) return -1;

    for (uint32_t i = 0; i < NFS_O_ITEM; ++i) out_item[i] = item[i];
    return 0;
}

/* Sucht einen Verzeichniseintrag mit Namen name (len Byte, KEIN
   Nullterminator) unter parent_id im Directory-Baum - Schluessel ist
   (parent_id, CRC32C(name)); mehrere Eintraege mit gleichem Hash (Kollision)
   werden linear nach dem tatsaechlichen Namen durchsucht (Port aus
   novafs_lookup). 0 = Erfolg (*out_object_id und *out_type gesetzt), -1 wenn
   nicht vorhanden/beschaedigt. */
int novafs_lookup(uint32_t parent_id, const char *name, uint32_t len, uint32_t *out_object_id,
                   uint32_t *out_type) {
    if (len == 0u || len > 255u) return -1;

    uint32_t hash = ~crc32c_update(0xFFFFFFFFu, (const uint8_t *)name, len);
    novafs_key_set(parent_id, hash);
    if (novafs_cursor_seek(&novafs_tree_directory) != 0) return -1;

    for (;;) {
        uint8_t *item = novafs_cursor_item_ptr();
        if (item == 0) return -1;
        if (novafs_compare_item(item, novafs_tree_directory.key_size) != 0) return -1;

        uint16_t name_len = *(const uint16_t *)(item + NFS_D_NAME_LENGTH);
        if (name_len == (uint16_t)len) {
            const uint8_t *entry_name = item + NFS_D_NAME;
            uint32_t i;
            for (i = 0; i < len; ++i) {
                if (entry_name[i] != (uint8_t)name[i]) break;
            }
            if (i == len) {
                if (*(const uint32_t *)(item + NFS_D_OBJECT + 4) != 0u) return -1;
                *out_object_id = *(const uint32_t *)(item + NFS_D_OBJECT);
                *out_type = *(const uint32_t *)(item + NFS_D_TYPE);
                return 0;
            }
        }
        if (novafs_cursor_advance() != 0) return -1;
    }
}

/* Kopiert den index-ten Verzeichniseintrag (0-basiert) von dir_id nach
   out_item (NFS_D_ITEM Byte) - fuer die Verzeichnislistung (Port aus
   novafs_dir_entry_at). Nutzt aus, dass Directory-Items nach (parent,hash)
   sortiert sind: alle Eintraege eines Verzeichnisses liegen zusammen, der
   Scan endet sauber, sobald ein Eintrag mit anderem Parent erscheint
   (naechstes Verzeichnis) oder der Baum erschoepft ist. 0 = Erfolg, -1 =
   Index >= Anzahl der Eintraege oder dir_id hat gar keine Eintraege. */
int novafs_dir_entry_at(uint32_t dir_id, uint32_t index, uint8_t *out_item) {
    novafs_key_set(dir_id, 0u);
    if (novafs_cursor_seek(&novafs_tree_directory) != 0) return -1;

    for (;;) {
        uint8_t *item = novafs_cursor_item_ptr();
        if (item == 0) return -1;
        if (*(const uint32_t *)(item + NFS_D_PARENT) != dir_id) return -1;
        if (*(const uint32_t *)(item + NFS_D_PARENT + 4) != 0u) return -1;

        if (index == 0u) {
            for (uint32_t i = 0; i < NFS_D_ITEM; ++i) out_item[i] = item[i];
            return 0;
        }
        --index;
        if (novafs_cursor_advance() != 0) return -1;
    }
}

/* Loest start_dir/path relativ zu start_dir auf - ein fuehrendes '/' im
   Pfad beginnt bei der Wurzel (ObjectID 1) und ignoriert start_dir (Port
   aus novafs_resolve_from). Leere Segmente (doppelte '/') werden
   ignoriert. 0 = Erfolg (*out_object_id und *out_type gesetzt), -1 wenn ein
   Segment nicht existiert oder ein Nicht-Verzeichnis durchquert werden
   muesste. */
int novafs_resolve_from(uint32_t start_dir, const char *path, uint32_t len, uint32_t *out_object_id,
                         uint32_t *out_type) {
    if (!nfs_mounted) return -1;
    if (len == 0u) return -1;

    uint32_t current_id = (path[0] == '/') ? 1u : start_dir;

    uint8_t obj[NFS_O_ITEM];
    if (novafs_object_find(current_id, obj) != 0) return -1;
    uint32_t current_type = *(const uint32_t *)(obj + NFS_O_TYPE);

    uint32_t pos = 0;
    while (pos < len) {
        if (path[pos] == '/') {
            ++pos;
            continue;
        }
        if (current_type != NOVAFS_TYPE_DIRECTORY) return -1;

        uint32_t seg_start = pos;
        while (pos < len && path[pos] != '/') ++pos;
        uint32_t seg_len = pos - seg_start;

        uint32_t obj_id, obj_type;
        if (novafs_lookup(current_id, path + seg_start, seg_len, &obj_id, &obj_type) != 0) {
            return -1;
        }
        current_id = obj_id;
        current_type = obj_type;
    }

    *out_object_id = current_id;
    *out_type = current_type;
    return 0;
}

/* Erfordert ein fuehrendes '/' (sonst -1) und beginnt immer bei der Wurzel
   (Port aus novafs_resolve_path). */
int novafs_resolve_path(const char *path, uint32_t len, uint32_t *out_object_id,
                         uint32_t *out_type) {
    if (len == 0u || path[0] != '/') return -1;
    return novafs_resolve_from(1u, path, len, out_object_id, out_type);
}

/* ===================================================================== */
/* Extents und Dateidaten (Batch 7): Logisch->Physisch-Abbildung ueber    */
/* den Extent-Baum + echtes Lesen von Dateiinhalten (Port aus             */
/* novafs_extent_map/novafs_read - NUR LESEN, Allokation/Schreiben folgt  */
/* mit dem Journal-/Write-Batch). Pro-Datenblock-Pruefsummen (optionales  */
/* Pruefsummen-Array, novafs_data_checksum_verify) sind bewusst NICHT     */
/* portiert - das Feld bleibt in diesem Kernel immer 0 ("kein Volume mit  */
/* aktivierten Datenpruefsummen"), was im alten ASM exakt einem no-op     */
/* entspricht, siehe NFS_SB_CHECKSUM_TREE in novafs_probe_volume. */
/* ===================================================================== */

#define NFS_E_OBJECT   0u
#define NFS_E_LOGICAL  8u
#define NFS_E_PHYSICAL 16u
#define NFS_E_COUNT    24u

#define NFS_O_SIZE 24u

static uint32_t nfs_data_page = 0;

/* Bildet logical_block (0-basierter 4-KiB-Blockindex innerhalb der Datei)
   auf einen physischen Volume-Block ab (Port aus novafs_extent_map).
   0 = Erfolg (*out_physical_block gesetzt - 0 bedeutet ein Loch/
   Sparse-Bereich, kein Fehler), -1 bei einem beschaedigten Extent-Baum. */
int novafs_extent_map(uint32_t object_id, uint32_t logical_block, uint32_t *out_physical_block) {
    novafs_key_set(object_id, 0u);
    if (novafs_cursor_seek(&novafs_tree_extent) != 0) return -1;

    uint32_t result = 0;
    for (;;) {
        uint8_t *item = novafs_cursor_item_ptr();
        if (item == 0) break; /* Baum erschoepft - kein weiteres Extent */
        if (*(const uint32_t *)(item + NFS_E_OBJECT) != object_id ||
            *(const uint32_t *)(item + NFS_E_OBJECT + 4) != 0u) {
            break; /* naechstes Objekt erreicht - keine weiteren Extents */
        }

        uint32_t logical = *(const uint32_t *)(item + NFS_E_LOGICAL);
        if ((logical & 0xFFFu) != 0u) return -1;
        if (*(const uint32_t *)(item + NFS_E_LOGICAL + 4) != 0u) return -1;
        logical >>= 12;
        if (logical > logical_block) break; /* Extent liegt bereits dahinter -> Loch */

        uint32_t count = *(const uint32_t *)(item + NFS_E_COUNT);
        if (*(const uint32_t *)(item + NFS_E_COUNT + 4) != 0u) return -1;
        if (count == 0u) return -1;

        uint32_t physical = *(const uint32_t *)(item + NFS_E_PHYSICAL);
        if (*(const uint32_t *)(item + NFS_E_PHYSICAL + 4) != 0u) return -1;

        uint32_t within = logical_block - logical;
        if (within < count) {
            uint32_t phys = physical + within;
            if (phys >= nfs_total_blocks_value) return -1;
            result = phys;
            break;
        }

        if (novafs_cursor_advance() != 0) break;
    }

    *out_physical_block = result;
    return 0;
}

/* Liest length Byte ab offset aus der Datei object_id nach out_buffer
   (gekuerzt auf die tatsaechliche Dateigroesse, siehe *out_bytes_read) -
   Port aus novafs_read. Loecher (Extent-Map liefert 0) werden als Nullen
   geliefert, ohne I/O. 0 = Erfolg, -1 wenn object_id keine Datei ist oder
   der Extent-/Datenbereich beschaedigt ist. */
int novafs_read(uint32_t object_id, uint32_t offset, uint32_t length, uint8_t *out_buffer,
                 uint32_t *out_bytes_read) {
    uint8_t obj[NFS_O_ITEM];
    if (novafs_object_find(object_id, obj) != 0) return -1;
    if (*(const uint32_t *)(obj + NFS_O_TYPE) != NOVAFS_TYPE_FILE) return -1;
    if (*(const uint32_t *)(obj + NFS_O_SIZE + 4) != 0u) return -1;
    uint32_t size = *(const uint32_t *)(obj + NFS_O_SIZE);

    if (offset >= size) {
        *out_bytes_read = 0;
        return 0;
    }
    uint32_t remaining = size - offset;
    if (length < remaining) remaining = length;

    uint32_t done = 0;
    while (remaining > 0u) {
        uint32_t logical_block = offset >> 12;
        uint32_t physical_block;
        if (novafs_extent_map(object_id, logical_block, &physical_block) != 0) return -1;

        uint32_t within_block = offset & 0xFFFu;
        uint32_t chunk = NOVAFS_BLOCK_SIZE - within_block;
        if (chunk > remaining) chunk = remaining;

        if (physical_block == 0u) {
            for (uint32_t i = 0; i < chunk; ++i) out_buffer[i] = 0;
        } else {
            if (nfs_data_page == 0) {
                uint32_t numa;
                nfs_data_page = pmm_alloc_page(&numa);
                if (nfs_data_page == 0) return -1;
            }
            uint8_t *data = (uint8_t *)(uintptr_t)nfs_data_page;
            if (novafs_read_block(physical_block, data) != 0) return -1;
            for (uint32_t i = 0; i < chunk; ++i) out_buffer[i] = data[within_block + i];
        }

        out_buffer += chunk;
        offset += chunk;
        done += chunk;
        remaining -= chunk;
    }

    *out_bytes_read = done;
    return 0;
}

/* ===================================================================== */
/* Selbsttest - synthetisches Fixture (sparse In-Memory-"Laufwerk", nur   */
/* die tatsaechlich gelesenen Bloecke 1/2/63 existieren) statt            */
/* Abhaengigkeit vom Inhalt eines echten Datentraegers.                   */
/* ===================================================================== */

#define NOVAFS_TEST_TOTAL 64u

static uint8_t *nfs_test_block1 = 0;
static uint8_t *nfs_test_block2 = 0;
static uint8_t *nfs_test_block63 = 0;

static int novafs_self_test_read(void *record, uint32_t lba_lo, uint32_t lba_hi, uint32_t count,
                                  void *buffer) {
    (void)record;
    (void)lba_hi;
    (void)count;
    uint32_t block = lba_lo / NOVAFS_SECTORS_PER_BLOCK;
    const uint8_t *src;
    if (block == 1u) src = nfs_test_block1;
    else if (block == 2u) src = nfs_test_block2;
    else if (block == 63u) src = nfs_test_block63;
    else return -1; /* im Selbsttest darf nie ein anderer Block gelesen werden */

    uint8_t *dst = (uint8_t *)buffer;
    for (uint32_t i = 0; i < NOVAFS_BLOCK_SIZE; ++i) dst[i] = src[i];
    return 0;
}

static void novafs_build_test_superblock_ex(uint8_t *sb, uint32_t total, uint32_t generation,
                                             uint32_t bitmap_crc, uint32_t object_root,
                                             uint32_t directory_root, uint32_t extent_root) {
    for (uint32_t i = 0; i < NOVAFS_BLOCK_SIZE; ++i) sb[i] = 0;
    sb[0] = 'N'; sb[1] = 'O'; sb[2] = 'V'; sb[3] = 'A'; sb[4] = 'F'; sb[5] = 'S'; sb[6] = 1; sb[7] = 0;
    *(uint16_t *)(sb + NFS_SB_VMAJ) = 1;
    *(uint32_t *)(sb + NFS_SB_BLOCK_SIZE) = NOVAFS_BLOCK_SIZE;
    *(uint32_t *)(sb + NFS_SB_FS_SIZE) = total << 12;
    *(uint32_t *)(sb + NFS_SB_FS_SIZE + 4) = total >> 20;
    *(uint32_t *)(sb + NFS_SB_TOTAL) = total;
    *(uint32_t *)(sb + NFS_SB_AVAILABLE) = total - 3u;
    *(uint32_t *)(sb + NFS_SB_GENERATION) = generation;

    uint32_t bitmap_blocks = (total + 32767u) >> 15;
    uint32_t journal_block = bitmap_blocks + 2u;
    *(uint32_t *)(sb + NFS_SB_OBJECT_TREE) = object_root;
    *(uint32_t *)(sb + NFS_SB_DIRECTORY_TREE) = directory_root;
    *(uint32_t *)(sb + NFS_SB_EXTENT_TREE) = extent_root;
    *(uint32_t *)(sb + NFS_SB_FREE_SPACE) = 2u;
    *(uint32_t *)(sb + NFS_SB_JOURNAL_BLOCK) = journal_block;
    *(uint32_t *)(sb + NFS_SB_BITMAP_BLOCKS) = bitmap_blocks;
    *(uint32_t *)(sb + NFS_SB_BACKUP_BLOCK) = total - 1u;
    *(uint32_t *)(sb + NFS_SB_BITMAP_CRC) = bitmap_crc;
    *(uint32_t *)(sb + NFS_SB_STATE) = NOVAFS_STATE_CLEAN;
    *(uint32_t *)(sb + NFS_SB_NEXT_OBJECT) = NOVAFS_FIRST_DYNAMIC_ID;

    novafs_seal(sb, NOVAFS_BLOCK_SIZE, NFS_SB_CHECKSUM);
}

static void novafs_build_test_superblock(uint8_t *sb, uint32_t total, uint32_t generation,
                                          uint32_t bitmap_crc) {
    uint32_t bitmap_blocks = (total + 32767u) >> 15;
    uint32_t journal_block = bitmap_blocks + 2u;
    uint32_t first_free = journal_block + NOVAFS_JOURNAL_BLOCKS;
    novafs_build_test_superblock_ex(sb, total, generation, bitmap_crc, first_free, first_free,
                                     first_free);
}

int novafs_self_test(void) {
    static const uint8_t check_string[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    if (~crc32c_update(0xFFFFFFFFu, check_string, 9u) != 0xE3069283u) return -1;

    uint32_t numa;
    uint32_t p1 = pmm_alloc_page(&numa);
    uint32_t p2 = pmm_alloc_page(&numa);
    uint32_t p63 = pmm_alloc_page(&numa);
    if (p1 == 0 || p2 == 0 || p63 == 0) return -1;
    nfs_test_block1 = (uint8_t *)(uintptr_t)p1;
    nfs_test_block2 = (uint8_t *)(uintptr_t)p2;
    nfs_test_block63 = (uint8_t *)(uintptr_t)p63;

    for (uint32_t i = 0; i < NOVAFS_BLOCK_SIZE; ++i) nfs_test_block2[i] = 0;
    nfs_test_block2[1u >> 3] |= (uint8_t)(1u << (1u & 7u));
    nfs_test_block2[63u >> 3] |= (uint8_t)(1u << (63u & 7u));
    uint32_t bitmap_crc = ~crc32c_update(0xFFFFFFFFu, nfs_test_block2, NOVAFS_BLOCK_SIZE);

    novafs_build_test_superblock(nfs_test_block1, NOVAFS_TEST_TOTAL, 5u, bitmap_crc);
    novafs_build_test_superblock(nfs_test_block63, NOVAFS_TEST_TOTAL, 7u, bitmap_crc);

    block_register_params_t params = {
        .id = 0x4E465354u, /* "NFST" */
        .flags = 0,
        .block_size = 512u,
        .block_count_lo = NOVAFS_TEST_TOTAL * NOVAFS_SECTORS_PER_BLOCK,
        .block_count_hi = 0,
        .transport = BLOCK_TRANSPORT_VIRT,
        .driver_handle = 0,
    };
    void *record = 0;
    if (block_device_register(&params, novafs_self_test_read, 0, 0, 0, &record) != 0) {
        return -1;
    }

    /* Test A: beide Kopien gueltig, Backup hat die hoehere Generation und
       muss gewinnen. */
    if (novafs_probe_volume(record, 0u, NOVAFS_TEST_TOTAL * NOVAFS_SECTORS_PER_BLOCK) != 0) {
        return -1;
    }
    if (novafs_total_blocks() != NOVAFS_TEST_TOTAL) return -1;
    if (novafs_bitmap_blocks() != 1u) return -1;
    if (novafs_generation_lo() != 7u) return -1;
    if (!novafs_is_used_backup()) return -1;

    /* Test B: beschaedigte Bitmap-CRC muss abgewiesen werden (beide
       Superblock-Kopien bleiben fuer sich gueltig). */
    nfs_test_block2[100] ^= 0xFFu;
    if (novafs_probe_volume(record, 0u, NOVAFS_TEST_TOTAL * NOVAFS_SECTORS_PER_BLOCK) == 0) {
        return -1;
    }
    nfs_test_block2[100] ^= 0xFFu;

    /* Test C: keine gueltige Superblock-Kopie (beide Magic-Felder
       beschaedigt) -> muss sauber -1 liefern, kein Absturz. */
    nfs_test_block1[0] ^= 0xFFu;
    nfs_test_block63[0] ^= 0xFFu;
    if (novafs_probe_volume(record, 0u, NOVAFS_TEST_TOTAL * NOVAFS_SECTORS_PER_BLOCK) == 0) {
        return -1;
    }

    return 0;
}

/* ===================================================================== */
/* Selbsttest B+-Tree-Kern (Batch 4) - eigenes, eigenstaendiges Fixture   */
/* mit einem leeren Objekt-Baum-Blatt und einem zweistufigen Extent-Baum */
/* (innerer Wurzelknoten + zwei Blatt-Kinder), unabhaengig vom Fixture   */
/* von novafs_self_test() oben.                                          */
/* ===================================================================== */

#define NOVAFS_TREE_TEST_MAX_BLOCKS 8u

static uint32_t nfs_tree_test_block_numbers[NOVAFS_TREE_TEST_MAX_BLOCKS];
static uint8_t *nfs_tree_test_block_pages[NOVAFS_TREE_TEST_MAX_BLOCKS];
static uint32_t nfs_tree_test_block_count = 0;

static uint8_t *novafs_tree_test_block(uint32_t block) {
    for (uint32_t i = 0; i < nfs_tree_test_block_count; ++i) {
        if (nfs_tree_test_block_numbers[i] == block) return nfs_tree_test_block_pages[i];
    }
    if (nfs_tree_test_block_count >= NOVAFS_TREE_TEST_MAX_BLOCKS) return 0;
    uint32_t numa;
    uint32_t page = pmm_alloc_page(&numa);
    if (page == 0) return 0;
    uint8_t *buf = (uint8_t *)(uintptr_t)page;
    for (uint32_t i = 0; i < NOVAFS_BLOCK_SIZE; ++i) buf[i] = 0;
    nfs_tree_test_block_numbers[nfs_tree_test_block_count] = block;
    nfs_tree_test_block_pages[nfs_tree_test_block_count] = buf;
    ++nfs_tree_test_block_count;
    return buf;
}

static int novafs_tree_test_read(void *record, uint32_t lba_lo, uint32_t lba_hi, uint32_t count,
                                  void *buffer) {
    (void)record;
    (void)lba_hi;
    (void)count;
    uint32_t block = lba_lo / NOVAFS_SECTORS_PER_BLOCK;
    for (uint32_t i = 0; i < nfs_tree_test_block_count; ++i) {
        if (nfs_tree_test_block_numbers[i] == block) {
            const uint8_t *src = nfs_tree_test_block_pages[i];
            uint8_t *dst = (uint8_t *)buffer;
            for (uint32_t j = 0; j < NOVAFS_BLOCK_SIZE; ++j) dst[j] = src[j];
            return 0;
        }
    }
    return -1;
}

static int novafs_tree_test_write(void *record, uint32_t lba_lo, uint32_t lba_hi, uint32_t count,
                                   void *buffer) {
    (void)record;
    (void)lba_hi;
    (void)count;
    uint32_t block = lba_lo / NOVAFS_SECTORS_PER_BLOCK;
    for (uint32_t i = 0; i < nfs_tree_test_block_count; ++i) {
        if (nfs_tree_test_block_numbers[i] == block) {
            uint8_t *dst = nfs_tree_test_block_pages[i];
            const uint8_t *src = (const uint8_t *)buffer;
            for (uint32_t j = 0; j < NOVAFS_BLOCK_SIZE; ++j) dst[j] = src[j];
            return 0;
        }
    }
    return -1;
}

int novafs_tree_self_test(void) {
    uint32_t total = 64u;

    uint8_t *sb1 = novafs_tree_test_block(1u);
    uint8_t *bitmap = novafs_tree_test_block(2u);
    uint8_t *leaf_obj = novafs_tree_test_block(20u);
    uint8_t *inner_ext = novafs_tree_test_block(30u);
    uint8_t *leaf_ext_a = novafs_tree_test_block(31u);
    uint8_t *leaf_ext_b = novafs_tree_test_block(32u);
    if (!sb1 || !bitmap || !leaf_obj || !inner_ext || !leaf_ext_a || !leaf_ext_b) return -1;
    (void)novafs_tree_test_block(63u); /* bleibt ungueltig (Magic=0) - Backup wird nicht gebraucht */

    static const uint32_t used_blocks[] = {1u, 63u, 20u, 30u, 31u, 32u};
    for (uint32_t i = 0; i < sizeof(used_blocks) / sizeof(used_blocks[0]); ++i) {
        uint32_t b = used_blocks[i];
        bitmap[b >> 3] |= (uint8_t)(1u << (b & 7u));
    }
    uint32_t bitmap_crc = ~crc32c_update(0xFFFFFFFFu, bitmap, NOVAFS_BLOCK_SIZE);

    novafs_build_test_superblock_ex(sb1, total, 1u, bitmap_crc, 20u, 20u, 30u);

    /* Objekt-Baum: leeres Blatt an Block 20. */
    novafs_node_init(leaf_obj, &novafs_tree_object, 20u, 0u, 0u);
    novafs_seal(leaf_obj, NOVAFS_BLOCK_SIZE, NFS_N_CHECKSUM);

    /* Extent-Baum: innerer Wurzelknoten (Block 30) mit zwei Blatt-Kindern
       (Block 31: Schluessel 10, Block 32: Schluessel 250). */
    novafs_node_init(leaf_ext_a, &novafs_tree_extent, 31u, 0u, 30u);
    uint8_t item_a[72] = {0};
    *(uint32_t *)(item_a + 0) = 10u;
    novafs_node_insert_at(leaf_ext_a, 0u, item_a, novafs_tree_extent.item_size);
    novafs_seal(leaf_ext_a, NOVAFS_BLOCK_SIZE, NFS_N_CHECKSUM);

    novafs_node_init(leaf_ext_b, &novafs_tree_extent, 32u, 0u, 30u);
    uint8_t item_b[72] = {0};
    *(uint32_t *)(item_b + 0) = 250u;
    novafs_node_insert_at(leaf_ext_b, 0u, item_b, novafs_tree_extent.item_size);
    novafs_seal(leaf_ext_b, NOVAFS_BLOCK_SIZE, NFS_N_CHECKSUM);

    novafs_node_init(inner_ext, &novafs_tree_extent, 30u, 1u, 0u);
    uint8_t inner_entry0[24] = {0};
    *(uint32_t *)(inner_entry0 + 0) = 10u;  /* Schluessel (fuer Index 0 unbenutzt) */
    *(uint32_t *)(inner_entry0 + 16) = 31u; /* Kindblock */
    novafs_node_insert_at(inner_ext, 0u, inner_entry0, NOVAFS_INNER_ITEM);
    uint8_t inner_entry1[24] = {0};
    *(uint32_t *)(inner_entry1 + 0) = 200u; /* Separator: alles >=200 geht rechts */
    *(uint32_t *)(inner_entry1 + 16) = 32u;
    novafs_node_insert_at(inner_ext, 1u, inner_entry1, NOVAFS_INNER_ITEM);
    novafs_seal(inner_ext, NOVAFS_BLOCK_SIZE, NFS_N_CHECKSUM);

    block_register_params_t params = {
        .id = 0x4E465432u, /* "NFT2" */
        .flags = 0,
        .block_size = 512u,
        .block_count_lo = total * NOVAFS_SECTORS_PER_BLOCK,
        .block_count_hi = 0,
        .transport = BLOCK_TRANSPORT_VIRT,
        .driver_handle = 0,
    };
    void *record = 0;
    if (block_device_register(&params, novafs_tree_test_read, novafs_tree_test_write, 0, 0,
                               &record) != 0) {
        return -1;
    }

    if (novafs_probe_volume(record, 0u, total * NOVAFS_SECTORS_PER_BLOCK) != 0) return -1;

    /* Szenario 1: unsortiert einfuegen (5, 1, 10), sortiert iterieren. */
    uint8_t item5[152] = {0};
    *(uint32_t *)(item5 + 0) = 5u;
    uint8_t item1[152] = {0};
    *(uint32_t *)(item1 + 0) = 1u;
    uint8_t item10[152] = {0};
    *(uint32_t *)(item10 + 0) = 10u;

    novafs_key_set(5u, 0u);
    if (novafs_cursor_seek(&novafs_tree_object) != 0) return -1;
    if (novafs_tree_insert_simple(&novafs_tree_object, item5) != 0) return -1;

    novafs_key_set(1u, 0u);
    if (novafs_cursor_seek(&novafs_tree_object) != 0) return -1;
    if (novafs_tree_insert_simple(&novafs_tree_object, item1) != 0) return -1;

    novafs_key_set(10u, 0u);
    if (novafs_cursor_seek(&novafs_tree_object) != 0) return -1;
    if (novafs_tree_insert_simple(&novafs_tree_object, item10) != 0) return -1;

    novafs_key_set(0u, 0u);
    if (novafs_cursor_seek(&novafs_tree_object) != 0) return -1;
    const uint8_t *p = novafs_cursor_item_ptr();
    if (!p || *(const uint32_t *)(p + 0) != 1u) return -1;
    if (novafs_cursor_advance() != 0) return -1;
    p = novafs_cursor_item_ptr();
    if (!p || *(const uint32_t *)(p + 0) != 5u) return -1;
    if (novafs_cursor_advance() != 0) return -1;
    p = novafs_cursor_item_ptr();
    if (!p || *(const uint32_t *)(p + 0) != 10u) return -1;
    if (novafs_cursor_advance() == 0) return -1; /* Ende des Baums erreicht */

    /* Szenario 2: Blatt bis max_items auffuellen, danach muss ein
       weiterer Einfuegeversuch sauber scheitern (kein Split in diesem
       Batch). */
    for (uint32_t id = 100u; id < 100u + (novafs_tree_object.max_items - 3u); ++id) {
        uint8_t item[152] = {0};
        *(uint32_t *)(item + 0) = id;
        novafs_key_set(id, 0u);
        if (novafs_cursor_seek(&novafs_tree_object) != 0) return -1;
        if (novafs_tree_insert_simple(&novafs_tree_object, item) != 0) return -1;
    }
    uint8_t overflow_item[152] = {0};
    *(uint32_t *)(overflow_item + 0) = 999u;
    novafs_key_set(999u, 0u);
    if (novafs_cursor_seek(&novafs_tree_object) != 0) return -1;
    if (novafs_tree_insert_simple(&novafs_tree_object, overflow_item) == 0) return -1;

    /* Szenario 3: Extent-Baum, zweistufiger Abstieg - Schluessel 5 muss in
       das LINKE Blatt-Kind (Block 31, Schluessel 10) fuehren, Schluessel
       250 in das RECHTE (Block 32, Schluessel 250 exakt). Faengt genau den
       anfangs vorhandenen Vorzeichenfehler in der Separator-Auswahl ab
       (Routing zum falschen Kind). */
    novafs_key_set(5u, 0u);
    if (novafs_cursor_seek(&novafs_tree_extent) != 0) return -1;
    p = novafs_cursor_item_ptr();
    if (!p || *(const uint32_t *)(p + 0) != 10u) return -1;

    novafs_key_set(250u, 0u);
    if (novafs_cursor_seek(&novafs_tree_extent) != 0) return -1;
    p = novafs_cursor_item_ptr();
    if (!p || *(const uint32_t *)(p + 0) != 250u) return -1;

    return 0;
}

/* ===================================================================== */
/* Selbsttest Verzeichnisse/Pfade (Batch 5) - eigenstaendiges Fixture:   */
/* Wurzel (ObjectID 1) -> Verzeichnis "System" (ObjectID 10) -> Datei    */
/* "test.txt" (ObjectID 11). Erste echte "existiert /System/x?"-Probe.  */
/* ===================================================================== */

#define NOVAFS_PATH_TEST_MAX_BLOCKS 6u

static uint32_t nfs_path_test_block_numbers[NOVAFS_PATH_TEST_MAX_BLOCKS];
static uint8_t *nfs_path_test_block_pages[NOVAFS_PATH_TEST_MAX_BLOCKS];
static uint32_t nfs_path_test_block_count = 0;

static uint8_t *novafs_path_test_block(uint32_t block) {
    for (uint32_t i = 0; i < nfs_path_test_block_count; ++i) {
        if (nfs_path_test_block_numbers[i] == block) return nfs_path_test_block_pages[i];
    }
    if (nfs_path_test_block_count >= NOVAFS_PATH_TEST_MAX_BLOCKS) return 0;
    uint32_t numa;
    uint32_t page = pmm_alloc_page(&numa);
    if (page == 0) return 0;
    uint8_t *buf = (uint8_t *)(uintptr_t)page;
    for (uint32_t i = 0; i < NOVAFS_BLOCK_SIZE; ++i) buf[i] = 0;
    nfs_path_test_block_numbers[nfs_path_test_block_count] = block;
    nfs_path_test_block_pages[nfs_path_test_block_count] = buf;
    ++nfs_path_test_block_count;
    return buf;
}

static int novafs_path_test_read(void *record, uint32_t lba_lo, uint32_t lba_hi, uint32_t count,
                                  void *buffer) {
    (void)record;
    (void)lba_hi;
    (void)count;
    uint32_t block = lba_lo / NOVAFS_SECTORS_PER_BLOCK;
    for (uint32_t i = 0; i < nfs_path_test_block_count; ++i) {
        if (nfs_path_test_block_numbers[i] == block) {
            const uint8_t *src = nfs_path_test_block_pages[i];
            uint8_t *dst = (uint8_t *)buffer;
            for (uint32_t j = 0; j < NOVAFS_BLOCK_SIZE; ++j) dst[j] = src[j];
            return 0;
        }
    }
    return -1;
}

static void novafs_path_test_build_object(uint8_t *out, uint32_t id, uint32_t parent,
                                           uint32_t type) {
    for (uint32_t i = 0; i < NFS_O_ITEM; ++i) out[i] = 0;
    *(uint32_t *)(out + NFS_O_ID) = id;
    *(uint32_t *)(out + NFS_O_PARENT) = parent;
    *(uint32_t *)(out + NFS_O_TYPE) = type;
    novafs_seal(out, NFS_O_ITEM, NFS_O_CHECKSUM);
}

static void novafs_path_test_build_dirent(uint8_t *out, uint32_t parent, uint32_t hash,
                                           uint32_t object_id, uint32_t type, const char *name,
                                           uint32_t name_len) {
    for (uint32_t i = 0; i < 288u; ++i) out[i] = 0;
    *(uint32_t *)(out + NFS_D_PARENT) = parent;
    *(uint32_t *)(out + NFS_D_HASH) = hash;
    *(uint32_t *)(out + NFS_D_OBJECT) = object_id;
    *(uint32_t *)(out + NFS_D_TYPE) = type;
    *(uint16_t *)(out + NFS_D_NAME_LENGTH) = (uint16_t)name_len;
    for (uint32_t i = 0; i < name_len; ++i) out[NFS_D_NAME + i] = (uint8_t)name[i];
}

int novafs_path_self_test(void) {
    uint32_t total = 64u;

    uint8_t *sb1 = novafs_path_test_block(1u);
    uint8_t *bitmap = novafs_path_test_block(2u);
    uint8_t *leaf_obj = novafs_path_test_block(20u);
    uint8_t *leaf_dir = novafs_path_test_block(21u);
    if (!sb1 || !bitmap || !leaf_obj || !leaf_dir) return -1;
    (void)novafs_path_test_block(63u);

    static const uint32_t used_blocks[] = {1u, 63u, 20u, 21u};
    for (uint32_t i = 0; i < sizeof(used_blocks) / sizeof(used_blocks[0]); ++i) {
        uint32_t b = used_blocks[i];
        bitmap[b >> 3] |= (uint8_t)(1u << (b & 7u));
    }
    uint32_t bitmap_crc = ~crc32c_update(0xFFFFFFFFu, bitmap, NOVAFS_BLOCK_SIZE);

    novafs_build_test_superblock_ex(sb1, total, 1u, bitmap_crc, 20u, 21u, 20u);

    novafs_node_init(leaf_obj, &novafs_tree_object, 20u, 0u, 0u);
    uint8_t obj_root[NFS_O_ITEM], obj_system[NFS_O_ITEM], obj_file[NFS_O_ITEM];
    novafs_path_test_build_object(obj_root, 1u, 0u, NOVAFS_TYPE_DIRECTORY);
    novafs_path_test_build_object(obj_system, 10u, 1u, NOVAFS_TYPE_DIRECTORY);
    novafs_path_test_build_object(obj_file, 11u, 10u, NOVAFS_TYPE_FILE);
    novafs_node_insert_at(leaf_obj, 0u, obj_root, NFS_O_ITEM);
    novafs_node_insert_at(leaf_obj, 1u, obj_system, NFS_O_ITEM);
    novafs_node_insert_at(leaf_obj, 2u, obj_file, NFS_O_ITEM);
    novafs_seal(leaf_obj, NOVAFS_BLOCK_SIZE, NFS_N_CHECKSUM);

    static const char name_system[] = "System";
    static const char name_file[] = "test.txt";
    uint32_t hash_system = ~crc32c_update(0xFFFFFFFFu, (const uint8_t *)name_system, 6u);
    uint32_t hash_file = ~crc32c_update(0xFFFFFFFFu, (const uint8_t *)name_file, 8u);

    novafs_node_init(leaf_dir, &novafs_tree_directory, 21u, 0u, 0u);
    uint8_t dirent_system[288], dirent_file[288];
    novafs_path_test_build_dirent(dirent_system, 1u, hash_system, 10u, NOVAFS_TYPE_DIRECTORY,
                                   name_system, 6u);
    novafs_path_test_build_dirent(dirent_file, 10u, hash_file, 11u, NOVAFS_TYPE_FILE, name_file,
                                   8u);
    /* Sortiert nach Schluessel (parent,hash) einfuegen - parent=1 < parent=10. */
    novafs_node_insert_at(leaf_dir, 0u, dirent_system, 288u);
    novafs_node_insert_at(leaf_dir, 1u, dirent_file, 288u);
    novafs_seal(leaf_dir, NOVAFS_BLOCK_SIZE, NFS_N_CHECKSUM);

    block_register_params_t params = {
        .id = 0x4E465033u, /* "NFP3" */
        .flags = 0,
        .block_size = 512u,
        .block_count_lo = total * NOVAFS_SECTORS_PER_BLOCK,
        .block_count_hi = 0,
        .transport = BLOCK_TRANSPORT_VIRT,
        .driver_handle = 0,
    };
    void *record = 0;
    if (block_device_register(&params, novafs_path_test_read, 0, 0, 0, &record) != 0) {
        return -1;
    }
    if (novafs_probe_volume(record, 0u, total * NOVAFS_SECTORS_PER_BLOCK) != 0) return -1;

    uint32_t id, type;

#define NOVAFS_TEST_PATH_LEN(s) (sizeof(s) - 1u)

    /* "existiert /System?" */
    if (novafs_resolve_path("/System", NOVAFS_TEST_PATH_LEN("/System"), &id, &type) != 0) {
        return -1;
    }
    if (id != 10u || type != NOVAFS_TYPE_DIRECTORY) return -1;

    /* "existiert /System/test.txt?" */
    if (novafs_resolve_path("/System/test.txt", NOVAFS_TEST_PATH_LEN("/System/test.txt"), &id,
                             &type) != 0) {
        return -1;
    }
    if (id != 11u || type != NOVAFS_TYPE_FILE) return -1;

    /* Relative Aufloesung ab ObjectID 10 (System) muss denselben Treffer
       liefern wie der absolute Pfad. */
    if (novafs_resolve_from(10u, "test.txt", NOVAFS_TEST_PATH_LEN("test.txt"), &id, &type) != 0) {
        return -1;
    }
    if (id != 11u || type != NOVAFS_TYPE_FILE) return -1;

    /* Nicht vorhandene Datei/Pfad muessen sauber -1 liefern. */
    if (novafs_resolve_path("/System/missing.txt", NOVAFS_TEST_PATH_LEN("/System/missing.txt"),
                             &id, &type) == 0) {
        return -1;
    }
    if (novafs_resolve_path("/nope", NOVAFS_TEST_PATH_LEN("/nope"), &id, &type) == 0) return -1;

    /* Eine Datei als Verzeichnis zu durchqueren muss abgelehnt werden. */
    if (novafs_resolve_path("/System/test.txt/x", NOVAFS_TEST_PATH_LEN("/System/test.txt/x"),
                             &id, &type) == 0) {
        return -1;
    }

    /* Relativer Pfad ohne fuehrendes '/' wird von novafs_resolve_path
       (im Gegensatz zu novafs_resolve_from) abgelehnt. */
    if (novafs_resolve_path("System", NOVAFS_TEST_PATH_LEN("System"), &id, &type) == 0) {
        return -1;
    }

#undef NOVAFS_TEST_PATH_LEN

    return 0;
}

/* ===================================================================== */
/* Selbsttest Verzeichnislistung (Batch 6) - eigenstaendiges Fixture:     */
/* Wurzel (ObjectID 1) hat zwei Kinder ("System"=10, "readme.txt"=12),   */
/* "System" hat ein eigenes Kind ("test.txt"=11). Prueft, dass die       */
/* Listung pro Verzeichnis korrekt abgegrenzt ist (sortiert nach          */
/* (parent,hash) liegen alle Eintraege eines Verzeichnisses zusammen).   */
/* ===================================================================== */

#define NOVAFS_DIR_TEST_MAX_BLOCKS 6u

static uint32_t nfs_dir_test_block_numbers[NOVAFS_DIR_TEST_MAX_BLOCKS];
static uint8_t *nfs_dir_test_block_pages[NOVAFS_DIR_TEST_MAX_BLOCKS];
static uint32_t nfs_dir_test_block_count = 0;

static uint8_t *novafs_dir_test_block(uint32_t block) {
    for (uint32_t i = 0; i < nfs_dir_test_block_count; ++i) {
        if (nfs_dir_test_block_numbers[i] == block) return nfs_dir_test_block_pages[i];
    }
    if (nfs_dir_test_block_count >= NOVAFS_DIR_TEST_MAX_BLOCKS) return 0;
    uint32_t numa;
    uint32_t page = pmm_alloc_page(&numa);
    if (page == 0) return 0;
    uint8_t *buf = (uint8_t *)(uintptr_t)page;
    for (uint32_t i = 0; i < NOVAFS_BLOCK_SIZE; ++i) buf[i] = 0;
    nfs_dir_test_block_numbers[nfs_dir_test_block_count] = block;
    nfs_dir_test_block_pages[nfs_dir_test_block_count] = buf;
    ++nfs_dir_test_block_count;
    return buf;
}

static int novafs_dir_test_read(void *record, uint32_t lba_lo, uint32_t lba_hi, uint32_t count,
                                 void *buffer) {
    (void)record;
    (void)lba_hi;
    (void)count;
    uint32_t block = lba_lo / NOVAFS_SECTORS_PER_BLOCK;
    for (uint32_t i = 0; i < nfs_dir_test_block_count; ++i) {
        if (nfs_dir_test_block_numbers[i] == block) {
            const uint8_t *src = nfs_dir_test_block_pages[i];
            uint8_t *dst = (uint8_t *)buffer;
            for (uint32_t j = 0; j < NOVAFS_BLOCK_SIZE; ++j) dst[j] = src[j];
            return 0;
        }
    }
    return -1;
}

int novafs_dir_self_test(void) {
    uint32_t total = 64u;

    uint8_t *sb1 = novafs_dir_test_block(1u);
    uint8_t *bitmap = novafs_dir_test_block(2u);
    uint8_t *leaf_dir = novafs_dir_test_block(21u);
    if (!sb1 || !bitmap || !leaf_dir) return -1;
    (void)novafs_dir_test_block(63u);

    static const uint32_t used_blocks[] = {1u, 63u, 21u};
    for (uint32_t i = 0; i < sizeof(used_blocks) / sizeof(used_blocks[0]); ++i) {
        uint32_t b = used_blocks[i];
        bitmap[b >> 3] |= (uint8_t)(1u << (b & 7u));
    }
    uint32_t bitmap_crc = ~crc32c_update(0xFFFFFFFFu, bitmap, NOVAFS_BLOCK_SIZE);

    /* object_root/extent_root werden in diesem Test nie geladen (nur
       Verzeichnislistung, kein object_find) - dasselbe, bereits reale
       Blatt-Blatt 21 (directory_root) wiederverwenden ist hier zulaessig,
       da novafs_validate_superblock nur den Zahlenbereich prueft. */
    novafs_build_test_superblock_ex(sb1, total, 1u, bitmap_crc, 21u, 21u, 21u);

    static const char name_system[] = "System";
    static const char name_readme[] = "readme.txt";
    static const char name_testtxt[] = "test.txt";
    uint32_t hash_system = ~crc32c_update(0xFFFFFFFFu, (const uint8_t *)name_system, 6u);
    uint32_t hash_readme = ~crc32c_update(0xFFFFFFFFu, (const uint8_t *)name_readme, 10u);
    uint32_t hash_testtxt = ~crc32c_update(0xFFFFFFFFu, (const uint8_t *)name_testtxt, 8u);

    uint8_t dirent_system[NFS_D_ITEM], dirent_readme[NFS_D_ITEM], dirent_testtxt[NFS_D_ITEM];
    novafs_path_test_build_dirent(dirent_system, 1u, hash_system, 10u, NOVAFS_TYPE_DIRECTORY,
                                   name_system, 6u);
    novafs_path_test_build_dirent(dirent_readme, 1u, hash_readme, 12u, NOVAFS_TYPE_FILE,
                                   name_readme, 10u);
    novafs_path_test_build_dirent(dirent_testtxt, 10u, hash_testtxt, 11u, NOVAFS_TYPE_FILE,
                                   name_testtxt, 8u);

    novafs_node_init(leaf_dir, &novafs_tree_directory, 21u, 0u, 0u);
    /* Sortiert nach (parent,hash) einfuegen: beide parent=1-Eintraege nach
       ihrem Hash-Wert anordnen, parent=10 kommt in jedem Fall danach. */
    if (hash_system <= hash_readme) {
        novafs_node_insert_at(leaf_dir, 0u, dirent_system, NFS_D_ITEM);
        novafs_node_insert_at(leaf_dir, 1u, dirent_readme, NFS_D_ITEM);
    } else {
        novafs_node_insert_at(leaf_dir, 0u, dirent_readme, NFS_D_ITEM);
        novafs_node_insert_at(leaf_dir, 1u, dirent_system, NFS_D_ITEM);
    }
    novafs_node_insert_at(leaf_dir, 2u, dirent_testtxt, NFS_D_ITEM);
    novafs_seal(leaf_dir, NOVAFS_BLOCK_SIZE, NFS_N_CHECKSUM);

    block_register_params_t params = {
        .id = 0x4E465034u, /* "NFD4" */
        .flags = 0,
        .block_size = 512u,
        .block_count_lo = total * NOVAFS_SECTORS_PER_BLOCK,
        .block_count_hi = 0,
        .transport = BLOCK_TRANSPORT_VIRT,
        .driver_handle = 0,
    };
    void *record = 0;
    if (block_device_register(&params, novafs_dir_test_read, 0, 0, 0, &record) != 0) {
        return -1;
    }
    if (novafs_probe_volume(record, 0u, total * NOVAFS_SECTORS_PER_BLOCK) != 0) return -1;

    uint8_t item[NFS_D_ITEM];
    int saw_system = 0, saw_readme = 0;

    if (novafs_dir_entry_at(1u, 0u, item) != 0) return -1;
    uint32_t obj0 = *(const uint32_t *)(item + NFS_D_OBJECT);
    if (obj0 == 10u) saw_system = 1;
    else if (obj0 == 12u) saw_readme = 1;
    else return -1;

    if (novafs_dir_entry_at(1u, 1u, item) != 0) return -1;
    uint32_t obj1 = *(const uint32_t *)(item + NFS_D_OBJECT);
    if (obj1 == 10u) saw_system = 1;
    else if (obj1 == 12u) saw_readme = 1;
    else return -1;

    if (!saw_system || !saw_readme || obj0 == obj1) return -1;

    /* Kein drittes Kind unter der Wurzel. */
    if (novafs_dir_entry_at(1u, 2u, item) == 0) return -1;

    /* Verzeichnislistung von "System" (ObjectID 10) darf NICHT in die
       Eintraege der Wurzel zurueckfallen (Abgrenzung nach Parent). */
    if (novafs_dir_entry_at(10u, 0u, item) != 0) return -1;
    if (*(const uint32_t *)(item + NFS_D_OBJECT) != 11u) return -1;
    if (novafs_dir_entry_at(10u, 1u, item) == 0) return -1;

    /* Ein Verzeichnis ohne jegliche Eintraege liefert sofort -1. */
    if (novafs_dir_entry_at(999u, 0u, item) == 0) return -1;

    return 0;
}
