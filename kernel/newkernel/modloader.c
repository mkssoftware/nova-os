/* NovaOS C-Kernel - Restriktiver Kernel Module Loader (Port aus
   sync32.inc). */
#include "modloader.h"

static uint32_t module_state = 0;
static uint32_t module_instance_id = 0;
static uint32_t module_id = 0;
static uint32_t module_flags = 0;
static uint32_t module_active_calls = 0;
static uint32_t module_code_rights = 0;
static uint32_t module_data_rights = 0;
static uint32_t module_loaded_count = 0;
static uint32_t module_failed_count = 0;
static uint32_t module_unload_count = 0;
static uint8_t module_code_area[64];
static uint8_t module_data_area[64];

static int loader_ready = 0;

static uint32_t read_u32(const uint8_t *p, uint32_t offset) {
    return (uint32_t)p[offset] | ((uint32_t)p[offset + 1] << 8) |
           ((uint32_t)p[offset + 2] << 16) | ((uint32_t)p[offset + 3] << 24);
}

void module_loader_initialize(void) {
    for (uint32_t i = 0; i < sizeof(module_code_area); ++i) module_code_area[i] = 0;
    for (uint32_t i = 0; i < sizeof(module_data_area); ++i) module_data_area[i] = 0;

    module_state = MODULE_STATE_DISCOVERED;
    module_instance_id = 0;
    module_active_calls = 0;
    module_loaded_count = 0;
    module_failed_count = 0;
    module_unload_count = 0;

    loader_ready = 1;
}

static int module_reject(void) {
    module_state = MODULE_STATE_FAILED;
    ++module_failed_count;
    return -1;
}

int module_load(const uint8_t *package) {
    module_state = MODULE_STATE_VALIDATING;

    if (read_u32(package, 0) != MODULE_MAGIC) return module_reject();
    if (read_u32(package, 4) != MODULE_HEADER_SIZE) return module_reject();

    uint32_t image_size = read_u32(package, 8);
    if (image_size <= MODULE_HEADER_SIZE) return module_reject();
    if (image_size > MODULE_MAX_IMAGE_SIZE) return module_reject();

    if (read_u32(package, 12) != MODULE_ARCH_X86_32) return module_reject();

    uint32_t abi_min = read_u32(package, 16);
    uint32_t abi_max = read_u32(package, 20);
    if (abi_min > MODULE_KERNEL_ABI) return module_reject();
    if (abi_max < MODULE_KERNEL_ABI) return module_reject();

    uint32_t flags = read_u32(package, 28);
    if ((flags & MODULE_REQUIRED_FLAGS) != MODULE_REQUIRED_FLAGS) return module_reject();

    if (read_u32(package, 60) != 0) return module_reject();

    /* Code-/Datensektion muessen vollstaendig innerhalb der Datei liegen. */
    uint32_t code_offset = read_u32(package, 32);
    uint32_t code_size = read_u32(package, 36);
    if (code_offset < MODULE_HEADER_SIZE) return module_reject();
    uint32_t code_end = code_offset + code_size;
    if (code_end < code_offset) return module_reject(); /* Overflow */
    if (code_end > image_size) return module_reject();
    if (code_size > sizeof(module_code_area)) return module_reject();

    uint32_t data_offset = read_u32(package, 40);
    uint32_t data_size = read_u32(package, 44);
    if (data_offset < MODULE_HEADER_SIZE) return module_reject();
    uint32_t data_end = data_offset + data_size;
    if (data_end < data_offset) return module_reject(); /* Overflow */
    if (data_end > image_size) return module_reject();
    if (data_size > sizeof(module_data_area)) return module_reject();

    /* Ueberlappende Code-/Datensektionen werden strikt abgelehnt. */
    if (code_end > data_offset) {
        if (data_end <= code_offset) {
            /* Daten liegen vollstaendig vor dem Code - erlaubt. */
        } else {
            return module_reject();
        }
    }

    /* Signierter Inhaltsdigest (Additions-Rotations-Hash ueber die
       Nutzbytes nach dem Header). */
    uint32_t payload_len = image_size - MODULE_HEADER_SIZE;
    uint32_t digest = 0;
    for (uint32_t i = 0; i < payload_len; ++i) {
        digest += package[MODULE_HEADER_SIZE + i];
        digest = (digest << 3) | (digest >> 29);
    }
    if (digest != read_u32(package, 48)) return module_reject();

    /* Abhaengigkeit 0 bedeutet keine; andere IDs muessen bereits aktiv
       sein (nur ein Modul-Slot existiert - die "Kette" ist bewusst auf
       Tiefe 1 begrenzt, Bootstrap-Einschraenkung). */
    uint32_t dependency_id = read_u32(package, 52);
    if (dependency_id != 0) {
        if (dependency_id != module_id) return module_reject();
        if (module_state != MODULE_STATE_ACTIVE) return module_reject();
    }

    module_state = MODULE_STATE_LOADING;
    for (uint32_t i = 0; i < code_size; ++i) module_code_area[i] = package[code_offset + i];
    for (uint32_t i = 0; i < data_size; ++i) module_data_area[i] = package[data_offset + i];

    module_state = MODULE_STATE_RELOCATING;
    /* Keine Relokation ist im Format v1 gleichbedeutend mit abgeschlossen. */
    module_code_rights = 0x5; /* R-X */
    module_data_rights = 0x3; /* RW- */

    module_state = MODULE_STATE_INITIALIZING;
    module_id = read_u32(package, 56);
    module_flags = flags;
    ++module_instance_id;

    /* Erst jetzt wird die vollstaendig gepruefte Instanz atomar sichtbar. */
    module_state = MODULE_STATE_ACTIVE;
    ++module_loaded_count;
    return 0;
}

int module_unload(void) {
    if (module_state != MODULE_STATE_ACTIVE) return -1;
    if ((module_flags & MODULE_FLAG_UNLOADABLE) == 0) return -1;
    if (module_active_calls != 0) return -1;

    module_state = MODULE_STATE_QUIESCING;
    module_state = MODULE_STATE_UNLOADING;

    for (uint32_t i = 0; i < sizeof(module_code_area); ++i) module_code_area[i] = 0;
    for (uint32_t i = 0; i < sizeof(module_data_area); ++i) module_data_area[i] = 0;
    module_code_rights = 0;
    module_data_rights = 0;

    module_state = MODULE_STATE_UNLOADED;
    ++module_unload_count;
    return 0;
}

/* Kleines signiertes In-Kernel-Testpaket (96 Bytes: 64-Byte-Header + 16
   Byte Code + 16 Byte Daten). Digest wird ueber die 32 Nutzbytes
   gebildet - siehe module_load. */
static uint8_t module_test_image[96];

static void module_test_image_build(void) {
    uint8_t *p = module_test_image;
    for (uint32_t i = 0; i < sizeof(module_test_image); ++i) p[i] = 0;

#define PUT_U32(off, value) \
    do { \
        uint32_t v = (uint32_t)(value); \
        p[(off) + 0] = (uint8_t)(v); \
        p[(off) + 1] = (uint8_t)(v >> 8); \
        p[(off) + 2] = (uint8_t)(v >> 16); \
        p[(off) + 3] = (uint8_t)(v >> 24); \
    } while (0)

    PUT_U32(0, MODULE_MAGIC);
    PUT_U32(4, MODULE_HEADER_SIZE);
    PUT_U32(8, 96);
    PUT_U32(12, MODULE_ARCH_X86_32);
    PUT_U32(16, MODULE_KERNEL_ABI);
    PUT_U32(20, MODULE_KERNEL_ABI);
    PUT_U32(24, 5); /* unbenutztes Reserve-Feld, wie im alten ASM */
    PUT_U32(28, MODULE_FLAG_TRUSTED | MODULE_FLAG_STACK_GUARD | MODULE_FLAG_UNLOADABLE);
    PUT_U32(32, 64); /* code_offset */
    PUT_U32(36, 16); /* code_size */
    PUT_U32(40, 80); /* data_offset */
    PUT_U32(44, 16); /* data_size */
    /* digest bei Offset 48 wird unten berechnet */
    PUT_U32(52, 0); /* dependency_id */
    PUT_U32(56, 0x44494147u); /* module_id "GAID" */
    PUT_U32(60, 0); /* reserviert */

    for (uint32_t i = 0; i < 16; ++i) p[64 + i] = (uint8_t)(i + 1); /* Code: 1..16 */
    for (uint32_t i = 0; i < 16; ++i) p[80 + i] = 0x10; /* Daten: 16x 0x10 */

    uint32_t digest = 0;
    for (uint32_t i = 0; i < 32; ++i) {
        digest += p[64 + i];
        digest = (digest << 3) | (digest >> 29);
    }
    PUT_U32(48, digest);

#undef PUT_U32
}

int module_loader_self_test(void) {
    if (!loader_ready) return -1;

    module_test_image_build();

    if (module_load(module_test_image) != 0) return -1;
    if (module_state != MODULE_STATE_ACTIVE) return -1;
    if (module_code_rights != 0x5) return -1;
    if (module_data_rights != 0x3) return -1;

    module_active_calls = 1;
    if (module_unload() == 0) return -1; /* muss wegen active_calls fehlschlagen */
    module_active_calls = 0;
    if (module_unload() != 0) return -1;

    /* Manipulierte Architektur darf niemals bis zur Veroeffentlichung
       gelangen. */
    uint8_t saved_arch_byte = module_test_image[12];
    module_test_image[12] = 0xFF;
    module_test_image[13] = 0xFF;
    module_test_image[14] = 0xFF;
    module_test_image[15] = 0xFF;
    int rc = module_load(module_test_image);
    module_test_image[12] = saved_arch_byte;
    module_test_image[13] = 0;
    module_test_image[14] = 0;
    module_test_image[15] = 0;
    if (rc == 0) return -1; /* muss abgelehnt werden */

    /* Am Ende bleibt eine gueltige Diagnosemodulinstanz aktiv. */
    if (module_load(module_test_image) != 0) return -1;
    if (module_loaded_count != 2) return -1;
    if (module_failed_count != 1) return -1;
    if (module_unload_count != 1) return -1;

    return 0;
}
