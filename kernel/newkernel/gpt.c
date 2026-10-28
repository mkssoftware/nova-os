/* NovaOS C-Kernel - GPT-Partitionssuche fuer die NovaFS-Systempartition
   (siehe gpt.h fuer Scope). */
#include "gpt.h"
#include "block.h"
#include "pmm.h"

#define GPT_SECTOR_SIZE 512u

/* "NOVAFSSYSTEM0001" in GPT-Bytefolge - identisch zum alten ASM-Label
   novafs_partition_type_guid. */
static const uint8_t gpt_novafs_type_guid[16] = {
    0x41, 0x56, 0x4F, 0x4E, 0x53, 0x46, 0x59, 0x53,
    0x53, 0x54, 0x45, 0x4D, 0x30, 0x30, 0x30, 0x31,
};

/* CRC32 IEEE 802.3 (Polynom 0xEDB88320), bit-fuer-Bit ohne Tabelle - fuer
   die vergleichsweise kleinen GPT-Header/Eintrags-Mengen ausreichend
   schnell (echtes CRC32C mit Tabelle folgt erst mit novafs32.inc, wo
   grosse Datenmengen pro Block tatsaechlich eine Tabelle rechtfertigen).
   crc ist der laufende Zustand (Start 0xFFFFFFFFu), Abschluss durch den
   Aufrufer per bitweisem NOT. */
static uint32_t crc32_ieee_update(uint32_t crc, const uint8_t *data, uint32_t length) {
    for (uint32_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; ++bit) {
            crc = (crc >> 1) ^ ((crc & 1u) ? 0xEDB88320u : 0u);
        }
    }
    return crc;
}

static int gpt_guid_equal(const uint8_t *a, const uint8_t *b) {
    for (uint32_t i = 0; i < 16u; ++i) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

int gpt_find_novafs_partition(void *record, uint32_t *out_first_lba, uint32_t *out_sector_count) {
    uint32_t numa;
    uint32_t page = pmm_alloc_page(&numa);
    if (page == 0) return -1;
    uint8_t *buf = (uint8_t *)(uintptr_t)page;

    if (block_io_submit(record, 1u, 0u, 1u, buf, BLK_IO_READ) != 0) goto fail;

    if (buf[0] != 'E' || buf[1] != 'F' || buf[2] != 'I' || buf[3] != ' ') goto fail;
    if (buf[4] != 'P' || buf[5] != 'A' || buf[6] != 'R' || buf[7] != 'T') goto fail;

    uint32_t header_size = *(const uint32_t *)(buf + 12);
    if (header_size < 92u || header_size > GPT_SECTOR_SIZE) goto fail;

    uint32_t saved_crc = *(const uint32_t *)(buf + 16);
    *(uint32_t *)(buf + 16) = 0;
    uint32_t computed_crc = ~crc32_ieee_update(0xFFFFFFFFu, buf, header_size);
    *(uint32_t *)(buf + 16) = saved_crc;
    if (computed_crc != saved_crc) goto fail;

    uint32_t entries_lba_lo = *(const uint32_t *)(buf + 72);
    uint32_t entries_lba_hi = *(const uint32_t *)(buf + 76);
    uint32_t num_entries = *(const uint32_t *)(buf + 80);
    uint32_t entry_size = *(const uint32_t *)(buf + 84);
    uint32_t entries_crc_expected = *(const uint32_t *)(buf + 88);

    if (entries_lba_hi != 0u) goto fail;
    if (entry_size != 128u) goto fail;
    if (num_entries == 0u || num_entries > 128u) goto fail;

    uint32_t found = 0;
    uint32_t part_first = 0, part_sectors = 0;
    uint32_t crc_state = 0xFFFFFFFFu;

    for (uint32_t index = 0; index < num_entries; ) {
        uint32_t sector_lba = entries_lba_lo + (index >> 2);
        if (block_io_submit(record, sector_lba, 0u, 1u, buf, BLK_IO_READ) != 0) goto fail;

        uint32_t in_sector = 0;
        while (in_sector < 4u && index < num_entries) {
            const uint8_t *entry = buf + in_sector * 128u;
            crc_state = crc32_ieee_update(crc_state, entry, 128u);

            if (!found && gpt_guid_equal(entry, gpt_novafs_type_guid)) {
                uint32_t first_lba_hi = *(const uint32_t *)(entry + 36);
                uint32_t last_lba_hi = *(const uint32_t *)(entry + 44);
                uint32_t first_lba = *(const uint32_t *)(entry + 32);
                uint32_t last_lba = *(const uint32_t *)(entry + 40);
                if (first_lba_hi == 0u && last_lba_hi == 0u && last_lba >= first_lba &&
                    (first_lba & 7u) == 0u) {
                    part_first = first_lba;
                    part_sectors = last_lba - first_lba + 1u;
                    found = 1;
                }
            }

            ++in_sector;
            ++index;
        }
    }

    if (!found) goto fail;

    uint32_t entries_crc_actual = ~crc_state;
    if (entries_crc_actual != entries_crc_expected) goto fail;

    uint32_t dev_block_count_lo = 0, dev_block_count_hi = 0;
    block_record_info(record, 0, &dev_block_count_lo, &dev_block_count_hi);
    if (dev_block_count_hi == 0u) {
        uint32_t end = part_first + part_sectors;
        if (end < part_first || end > dev_block_count_lo) goto fail;
    }

    pmm_free_page(page);
    *out_first_lba = part_first;
    *out_sector_count = part_sectors;
    return 0;

fail:
    pmm_free_page(page);
    return -1;
}

/* ===================================================================== */
/* Selbsttest - synthetisches Fixture statt Abhaengigkeit vom Inhalt des  */
/* tatsaechlichen Boot-Datentraegers (der i.A. keine NovaFS-Partition     */
/* besitzt - das ist kein Fehler, siehe gpt.h).                           */
/* ===================================================================== */

static uint8_t *gpt_self_test_disk = 0;

static int gpt_self_test_read(void *record, uint32_t lba_lo, uint32_t lba_hi, uint32_t count,
                               void *buffer) {
    (void)record;
    (void)lba_hi;
    const uint8_t *src = gpt_self_test_disk + (uint32_t)lba_lo * GPT_SECTOR_SIZE;
    uint8_t *dst = (uint8_t *)buffer;
    for (uint32_t i = 0; i < count * GPT_SECTOR_SIZE; ++i) dst[i] = src[i];
    return 0;
}

int gpt_self_test(void) {
    /* CRC32-Algorithmus zuerst unabhaengig gegen den Standard-Pruefwert
       fuer "123456789" verifizieren (identisch zum alten ASM crc_self_test),
       BEVOR er zum Bau des Fixtures unten verwendet wird. */
    static const uint8_t check_string[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    if (~crc32_ieee_update(0xFFFFFFFFu, check_string, 9u) != 0xCBF43926u) return -1;

    uint32_t numa;
    uint32_t page = pmm_alloc_page(&numa);
    if (page == 0) return -1;
    uint8_t *disk = (uint8_t *)(uintptr_t)page;
    for (uint32_t i = 0; i < 4096u; ++i) disk[i] = 0;
    gpt_self_test_disk = disk;

    uint8_t *header = disk + 1u * GPT_SECTOR_SIZE;
    uint8_t *entry = disk + 2u * GPT_SECTOR_SIZE;

    /* Ein Eintrag: NovaFS-Typ-GUID, StartingLBA=8 (4-KiB-Grenze),
       EndingLBA=15 -> 8 Sektoren. */
    for (uint32_t i = 0; i < 16u; ++i) entry[i] = gpt_novafs_type_guid[i];
    *(uint32_t *)(entry + 32) = 8u;
    *(uint32_t *)(entry + 36) = 0u;
    *(uint32_t *)(entry + 40) = 15u;
    *(uint32_t *)(entry + 44) = 0u;
    uint32_t entries_crc = ~crc32_ieee_update(0xFFFFFFFFu, entry, 128u);

    header[0] = 'E'; header[1] = 'F'; header[2] = 'I'; header[3] = ' ';
    header[4] = 'P'; header[5] = 'A'; header[6] = 'R'; header[7] = 'T';
    *(uint32_t *)(header + 12) = 92u;  /* HeaderSize */
    *(uint32_t *)(header + 72) = 2u;   /* PartitionEntryLBA lo */
    *(uint32_t *)(header + 76) = 0u;   /* PartitionEntryLBA hi */
    *(uint32_t *)(header + 80) = 1u;   /* NumberOfPartitionEntries */
    *(uint32_t *)(header + 84) = 128u; /* SizeOfPartitionEntry */
    *(uint32_t *)(header + 88) = entries_crc;
    *(uint32_t *)(header + 16) = 0u;   /* HeaderCRC32 (vorerst 0 fuer die Berechnung) */
    uint32_t header_crc = ~crc32_ieee_update(0xFFFFFFFFu, header, 92u);
    *(uint32_t *)(header + 16) = header_crc;
    uint32_t saved_header_crc = header_crc;

    block_register_params_t params = {
        .id = 0x47505454u, /* "GPTT" */
        .flags = 0,
        .block_size = GPT_SECTOR_SIZE,
        .block_count_lo = 1000000u,
        .block_count_hi = 0,
        .transport = BLOCK_TRANSPORT_VIRT,
        .driver_handle = 0,
    };
    void *record = 0;
    if (block_device_register(&params, gpt_self_test_read, 0, 0, 0, &record) != 0) {
        pmm_free_page(page);
        return -1;
    }

    /* Erfolgsfall: korrekte Partition gefunden. */
    uint32_t first_lba = 0, sector_count = 0;
    if (gpt_find_novafs_partition(record, &first_lba, &sector_count) != 0) {
        pmm_free_page(page);
        return -1;
    }
    if (first_lba != 8u || sector_count != 8u) {
        pmm_free_page(page);
        return -1;
    }

    /* Negativfall 1: beschaedigte Eintrags-CRC muss abgewiesen werden.
       Die Header-CRC wird dabei bewusst neu berechnet (entries_crc liegt
       selbst INNERHALB des headerCRC-geschuetzten Bereichs), damit dieser
       Test isoliert die Eintrags-CRC-Pruefung trifft statt schon vorher
       an der Header-CRC zu scheitern. */
    uint32_t saved_entries_crc = *(uint32_t *)(header + 88);
    *(uint32_t *)(header + 88) = saved_entries_crc ^ 1u;
    *(uint32_t *)(header + 16) = 0u;
    *(uint32_t *)(header + 16) = ~crc32_ieee_update(0xFFFFFFFFu, header, 92u);
    if (gpt_find_novafs_partition(record, &first_lba, &sector_count) == 0) {
        pmm_free_page(page);
        return -1;
    }
    *(uint32_t *)(header + 88) = saved_entries_crc;
    *(uint32_t *)(header + 16) = saved_header_crc;

    /* Negativfall 2: falsche Typ-GUID darf nicht gefunden werden (sauberes
       "nicht vorhanden", kein Absturz/CRC-Fehlalarm - die Eintrags- UND
       Header-CRC werden dafuer neu berechnet, da sich der Eintragsinhalt
       aendert und beide sonst faelschlich schon vorher ablehnen wuerden). */
    uint8_t saved_guid_byte = entry[0];
    entry[0] = (uint8_t)(saved_guid_byte ^ 0xFFu);
    uint32_t corrupted_entries_crc = ~crc32_ieee_update(0xFFFFFFFFu, entry, 128u);
    *(uint32_t *)(header + 88) = corrupted_entries_crc;
    *(uint32_t *)(header + 16) = 0u;
    *(uint32_t *)(header + 16) = ~crc32_ieee_update(0xFFFFFFFFu, header, 92u);
    if (gpt_find_novafs_partition(record, &first_lba, &sector_count) == 0) {
        pmm_free_page(page);
        return -1;
    }
    entry[0] = saved_guid_byte;
    *(uint32_t *)(header + 88) = saved_entries_crc;
    *(uint32_t *)(header + 16) = saved_header_crc;

    pmm_free_page(page);
    return 0;
}
