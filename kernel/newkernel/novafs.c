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

static void novafs_build_test_superblock(uint8_t *sb, uint32_t total, uint32_t generation,
                                          uint32_t bitmap_crc) {
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
    uint32_t first_free = journal_block + NOVAFS_JOURNAL_BLOCKS;
    *(uint32_t *)(sb + NFS_SB_OBJECT_TREE) = first_free;
    *(uint32_t *)(sb + NFS_SB_OBJECT_TREE + 8) = first_free; /* DIRECTORY_TREE */
    *(uint32_t *)(sb + NFS_SB_EXTENT_TREE) = first_free;
    *(uint32_t *)(sb + NFS_SB_FREE_SPACE) = 2u;
    *(uint32_t *)(sb + NFS_SB_JOURNAL_BLOCK) = journal_block;
    *(uint32_t *)(sb + NFS_SB_BITMAP_BLOCKS) = bitmap_blocks;
    *(uint32_t *)(sb + NFS_SB_BACKUP_BLOCK) = total - 1u;
    *(uint32_t *)(sb + NFS_SB_BITMAP_CRC) = bitmap_crc;
    *(uint32_t *)(sb + NFS_SB_STATE) = NOVAFS_STATE_CLEAN;
    *(uint32_t *)(sb + NFS_SB_NEXT_OBJECT) = NOVAFS_FIRST_DYNAMIC_ID;

    novafs_seal(sb, NOVAFS_BLOCK_SIZE, NFS_SB_CHECKSUM);
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
