/* NovaOS C-Kernel - Nova Desktop Wallpaper (siehe wallpaper.h fuer Scope). */
#include "wallpaper.h"
#include "novafs.h"
#include "block.h"
#include "pmm.h"

#define WALLPAPER_HEADER_SIZE 32u
#define WALLPAPER_FORMAT_RGB888 2u
#define WALLPAPER_MAX_PAYLOAD (WALLPAPER_MAX_WIDTH * WALLPAPER_MAX_HEIGHT * 3u)

static uint8_t wallpaper_buffer[WALLPAPER_MAX_PAYLOAD];
static uint32_t wallpaper_w = 0;
static uint32_t wallpaper_h = 0;
static int wallpaper_ready = 0;

/* CRC32 IEEE 802.3 (Polynom 0xEDB88320), bit-fuer-Bit ohne Tabelle - exakt
   derselbe Algorithmus wie scripts/build-wallpaper.ps1's Get-Crc32 und wie
   gpt.c's eigene (dort nicht exportierte) crc32_ieee_update; eine eigene,
   kleine Kopie statt einer gemeinsamen Funktion passt zum bestehenden
   Muster dieses Kernels (siehe novafs.h's Kommentar zu seiner eigenen,
   bewusst ANDEREN CRC32C-Variante). */
static uint32_t wallpaper_crc32_ieee(const uint8_t *data, uint32_t length) {
    uint32_t crc = 0xFFFFFFFFu;
    for (uint32_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (uint32_t bit = 0; bit < 8u; ++bit) {
            crc = (crc & 1u) ? ((crc >> 1) ^ 0xEDB88320u) : (crc >> 1);
        }
    }
    return ~crc;
}

static uint32_t wallpaper_read_u32_le(const uint8_t *buffer, uint32_t offset) {
    return (uint32_t)buffer[offset] | ((uint32_t)buffer[offset + 1] << 8) |
           ((uint32_t)buffer[offset + 2] << 16) | ((uint32_t)buffer[offset + 3] << 24);
}

int wallpaper_load_from_novafs(const char *path, uint32_t len) {
    if (!novafs_is_mounted()) return -1;

    uint32_t object_id, type;
    if (novafs_resolve_path(path, len, &object_id, &type) != 0) return -1;
    if (type != NOVAFS_TYPE_FILE) return -1;

    uint8_t header[WALLPAPER_HEADER_SIZE];
    uint32_t read_bytes = 0;
    if (novafs_read(object_id, 0u, WALLPAPER_HEADER_SIZE, header, &read_bytes) != 0) return -1;
    if (read_bytes != WALLPAPER_HEADER_SIZE) return -1;

    if (header[0] != (uint8_t)'N' || header[1] != (uint8_t)'B' ||
        header[2] != (uint8_t)'S' || header[3] != (uint8_t)'1') {
        return -1;
    }

    uint32_t header_size = wallpaper_read_u32_le(header, 4);
    uint32_t width = wallpaper_read_u32_le(header, 8);
    uint32_t height = wallpaper_read_u32_le(header, 12);
    uint32_t stride = wallpaper_read_u32_le(header, 16);
    uint32_t format = wallpaper_read_u32_le(header, 20);
    uint32_t payload_size = wallpaper_read_u32_le(header, 24);
    uint32_t stored_crc = wallpaper_read_u32_le(header, 28);

    if (header_size != WALLPAPER_HEADER_SIZE) return -1;
    if (format != WALLPAPER_FORMAT_RGB888) return -1;
    if (width == 0u || height == 0u) return -1;
    if (width > WALLPAPER_MAX_WIDTH || height > WALLPAPER_MAX_HEIGHT) return -1;
    if (stride != width * 3u) return -1;
    if (payload_size != stride * height) return -1;
    if (payload_size > WALLPAPER_MAX_PAYLOAD) return -1;

    if (novafs_read(object_id, header_size, payload_size, wallpaper_buffer, &read_bytes) != 0) {
        return -1;
    }
    if (read_bytes != payload_size) return -1;

    if (wallpaper_crc32_ieee(wallpaper_buffer, payload_size) != stored_crc) return -1;

    wallpaper_w = width;
    wallpaper_h = height;
    wallpaper_ready = 1;
    return 0;
}

int wallpaper_available(void) {
    return wallpaper_ready;
}

const uint8_t *wallpaper_pixels_rgb888(void) {
    return wallpaper_buffer;
}

uint32_t wallpaper_width(void) {
    return wallpaper_w;
}

uint32_t wallpaper_height(void) {
    return wallpaper_h;
}

/* ===================================================================== */
/* Selbsttest: eigenstaendige NovaFS-Testvolume-Fixture, exakt nach dem   */
/* Muster von vfsapi.c's vfsapi_self_test (novafs_build_test_volume +     */
/* ein virtuelles BLOCK_TRANSPORT_VIRT-Geraet) - siehe dort fuer das      */
/* Vorbild. Legt eine winzige, synthetische 2x2-Pixel-NBS1-Datei an und   */
/* prueft sowohl den Erfolgs- als auch den CRC-Fehlerfall.                */
/* ===================================================================== */

#define WALLPAPER_TEST_MAX_BLOCKS 48u

static uint32_t wallpaper_test_block_numbers[WALLPAPER_TEST_MAX_BLOCKS];
static uint8_t *wallpaper_test_block_pages[WALLPAPER_TEST_MAX_BLOCKS];
static uint32_t wallpaper_test_block_count = 0;

static uint8_t *wallpaper_test_block(uint32_t block) {
    for (uint32_t i = 0; i < wallpaper_test_block_count; ++i) {
        if (wallpaper_test_block_numbers[i] == block) return wallpaper_test_block_pages[i];
    }
    if (wallpaper_test_block_count >= WALLPAPER_TEST_MAX_BLOCKS) return 0;
    uint32_t numa;
    uint32_t page = pmm_alloc_page(&numa);
    if (page == 0) return 0;
    uint8_t *buf = (uint8_t *)(uintptr_t)page;
    for (uint32_t i = 0; i < NOVAFS_BLOCK_SIZE; ++i) buf[i] = 0;
    wallpaper_test_block_numbers[wallpaper_test_block_count] = block;
    wallpaper_test_block_pages[wallpaper_test_block_count] = buf;
    ++wallpaper_test_block_count;
    return buf;
}

static int wallpaper_test_read(void *record, uint32_t lba_lo, uint32_t lba_hi,
                                uint32_t count, void *buffer) {
    (void)record;
    (void)lba_hi;
    (void)count;
    uint32_t block = lba_lo / NOVAFS_SECTORS_PER_BLOCK;
    uint8_t *page = wallpaper_test_block(block);
    if (page == 0) return -1;
    for (uint32_t j = 0; j < NOVAFS_BLOCK_SIZE; ++j) ((uint8_t *)buffer)[j] = page[j];
    return 0;
}

static int wallpaper_test_write(void *record, uint32_t lba_lo, uint32_t lba_hi,
                                 uint32_t count, void *buffer) {
    (void)record;
    (void)lba_hi;
    (void)count;
    uint32_t block = lba_lo / NOVAFS_SECTORS_PER_BLOCK;
    uint8_t *page = wallpaper_test_block(block);
    if (page == 0) return -1;
    const uint8_t *src = (const uint8_t *)buffer;
    for (uint32_t j = 0; j < NOVAFS_BLOCK_SIZE; ++j) page[j] = src[j];
    return 0;
}

static int wallpaper_test_flush(void *record) {
    (void)record;
    return 0;
}

int wallpaper_self_test(void) {
    wallpaper_test_block_count = 0;
    uint32_t total = 64u;
    if (novafs_build_test_volume(wallpaper_test_block, total, 60u, 61u, 62u) != 0) return -1;

    block_register_params_t params = {
        .id = 0x57414c50u, /* "WALP" */
        .flags = 0,
        .block_size = 512u,
        .block_count_lo = total * NOVAFS_SECTORS_PER_BLOCK,
        .block_count_hi = 0,
        .transport = BLOCK_TRANSPORT_VIRT,
        .driver_handle = 0,
    };
    void *record = 0;
    if (block_device_register(&params, wallpaper_test_read, wallpaper_test_write,
                               wallpaper_test_flush, 0, &record) != 0) {
        return -1;
    }
    if (novafs_probe_volume(record, 0u, total * NOVAFS_SECTORS_PER_BLOCK) != 0) return -1;

    /* Synthetische 2x2-NBS1-Datei: Header + 12 Byte Payload (4 feste,
       unterscheidbare Farbpixel - deckt dadurch gleich die korrekte
       Zeilen-/Byte-Reihenfolge beim Laden mit ab). */
    static const uint8_t pixels[12] = {
        0x11u, 0x22u, 0x33u, 0x44u, 0x55u, 0x66u,
        0x77u, 0x88u, 0x99u, 0xAAu, 0xBBu, 0xCCu,
    };
    uint8_t blob[WALLPAPER_HEADER_SIZE + 12u];
    blob[0] = (uint8_t)'N'; blob[1] = (uint8_t)'B'; blob[2] = (uint8_t)'S'; blob[3] = (uint8_t)'1';
    /* Little-Endian-Felder explizit schreiben (gleiche Byte-Reihenfolge
       wie scripts/build-wallpaper.ps1's Write-U32). */
    blob[4] = 32u; blob[5] = 0u; blob[6] = 0u; blob[7] = 0u;              /* header_size */
    blob[8] = 2u; blob[9] = 0u; blob[10] = 0u; blob[11] = 0u;             /* width */
    blob[12] = 2u; blob[13] = 0u; blob[14] = 0u; blob[15] = 0u;           /* height */
    blob[16] = 6u; blob[17] = 0u; blob[18] = 0u; blob[19] = 0u;           /* stride = width*3 */
    blob[20] = 2u; blob[21] = 0u; blob[22] = 0u; blob[23] = 0u;           /* format RGB888 */
    blob[24] = 12u; blob[25] = 0u; blob[26] = 0u; blob[27] = 0u;          /* payload_size */
    for (uint32_t i = 0; i < 12u; ++i) blob[WALLPAPER_HEADER_SIZE + i] = pixels[i];
    uint32_t crc = wallpaper_crc32_ieee(blob + WALLPAPER_HEADER_SIZE, 12u);
    blob[28] = (uint8_t)(crc & 0xFFu);
    blob[29] = (uint8_t)((crc >> 8) & 0xFFu);
    blob[30] = (uint8_t)((crc >> 16) & 0xFFu);
    blob[31] = (uint8_t)((crc >> 24) & 0xFFu);

    if (novafs_change_begin() != 0) return -1;
    uint32_t object_id;
    if (novafs_create(1u, NOVAFS_TYPE_FILE, "wallpaper.nbs", 13u, &object_id) != 0) return -1;
    if (novafs_write(object_id, 0u, sizeof(blob), blob) != 0) return -1;
    if (novafs_change_commit() != 0) return -1;

    wallpaper_ready = 0;
    if (wallpaper_load_from_novafs("/wallpaper.nbs", 14u) != 0) return -1;
    if (!wallpaper_available()) return -1;
    if (wallpaper_width() != 2u || wallpaper_height() != 2u) return -1;
    for (uint32_t i = 0; i < 12u; ++i) {
        if (wallpaper_pixels_rgb888()[i] != pixels[i]) return -1;
    }

    /* Beschaedigte Payload (letztes Byte gekippt) MUSS per CRC abgelehnt
       werden - und darf den zuvor erfolgreich geladenen Zustand NICHT
       veraendern (siehe wallpaper.h: Fehlerfall laesst den vorherigen
       Zustand unberuehrt). */
    blob[WALLPAPER_HEADER_SIZE + 11u] ^= 0xFFu;
    if (novafs_change_begin() != 0) return -1;
    if (novafs_write(object_id, 0u, sizeof(blob), blob) != 0) return -1;
    if (novafs_change_commit() != 0) return -1;
    if (wallpaper_load_from_novafs("/wallpaper.nbs", 14u) == 0) return -1;
    if (!wallpaper_available()) return -1; /* alter, gueltiger Zustand bleibt */

    /* Unbekannter Pfad MUSS fehlschlagen. */
    if (wallpaper_load_from_novafs("/nicht-vorhanden.nbs", 20u) == 0) return -1;

    return 0;
}
