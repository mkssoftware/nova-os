/* NovaOS C-Kernel - Compatibility Layer (Port aus compat32.inc). */
#include "compat.h"

typedef struct {
    uint32_t id;
    uint32_t flags;
    uint32_t abi;
    uint32_t syscall_base;
    uint32_t syscall_count;
    uint32_t sandbox_caps;
    uint8_t name[8];
} compat_personality_t;

typedef struct {
    uint32_t foreign;
    int32_t nova; /* -1 = kein Mapping */
    uint32_t flags;
} compat_sc_map_entry_t;

static compat_personality_t compat_personalities[PERS_CAPACITY];
static compat_sc_map_entry_t compat_sc_map[SC_MAP_CAPACITY];
static uint32_t compat_pers_count = 0;
static uint32_t compat_sc_count = 0;
static uint32_t compat_translations = 0;
static uint32_t compat_blocked = 0;
static uint32_t compat_unsupported = 0;
static int compat_ready = 0;

static void compat_add_sc(uint32_t foreign, int32_t nova, uint32_t flags) {
    if (compat_sc_count >= SC_MAP_CAPACITY) {
        return;
    }
    compat_sc_map_entry_t *entry = &compat_sc_map[compat_sc_count];
    entry->foreign = foreign;
    entry->nova = nova;
    entry->flags = flags;
    ++compat_sc_count;
}

static void compat_register_linux_personality(void) {
    compat_personality_t *pers = &compat_personalities[0];
    pers->id = PERS_LINUX;
    pers->flags = PERS_FLAG_ACTIVE | PERS_FLAG_SYSCALL_TBL;
    pers->abi = ABI_SYSV_AMD64;
    pers->syscall_base = 0;
    pers->syscall_count = 256;
    pers->sandbox_caps = 0xFFFFFFFFu;
    const char *name = "LINUX";
    for (int i = 0; i < 8 && name[i]; ++i) {
        pers->name[i] = (uint8_t)name[i];
    }
    ++compat_pers_count;

    compat_add_sc(LINUX_SYS_READ, NOVA_SYS_READ, SCTRANS_OK);
    compat_add_sc(LINUX_SYS_WRITE, NOVA_SYS_WRITE, SCTRANS_OK);
    compat_add_sc(LINUX_SYS_OPEN, NOVA_SYS_OPEN, SCTRANS_OK);
    compat_add_sc(LINUX_SYS_CLOSE, NOVA_SYS_CLOSE, SCTRANS_OK);
    compat_add_sc(LINUX_SYS_EXIT, NOVA_SYS_EXIT, SCTRANS_OK);
    compat_add_sc(LINUX_SYS_EXIT_GROUP, NOVA_SYS_EXIT, SCTRANS_OK);
    compat_add_sc(LINUX_SYS_GETPID, NOVA_SYS_GETPID, SCTRANS_OK);
    compat_add_sc(LINUX_SYS_MMAP, NOVA_SYS_MMAP, SCTRANS_OK);
    /* munmap und brk: nicht vollstaendig implementiert. */
    compat_add_sc(LINUX_SYS_MUNMAP, -1, SCTRANS_UNSUPPORTED);
    compat_add_sc(LINUX_SYS_BRK, -1, SCTRANS_UNSUPPORTED);
}

void compat_initialize(void) {
    for (uint32_t i = 0; i < PERS_CAPACITY; ++i) {
        compat_personalities[i] = (compat_personality_t){0};
    }
    for (uint32_t i = 0; i < SC_MAP_CAPACITY; ++i) {
        compat_sc_map[i] = (compat_sc_map_entry_t){0};
    }
    compat_pers_count = 0;
    compat_sc_count = 0;
    compat_translations = 0;
    compat_blocked = 0;
    compat_unsupported = 0;

    compat_register_linux_personality();

    compat_ready = 1;
}

void compat_translate_syscall(uint32_t foreign_syscall_nr, uint32_t personality_id,
                               uint32_t *out_nova_nr, uint32_t *out_result) {
    (void)personality_id;

    for (uint32_t i = 0; i < compat_sc_count; ++i) {
        compat_sc_map_entry_t *entry = &compat_sc_map[i];
        if (entry->foreign != foreign_syscall_nr) {
            continue;
        }
        *out_nova_nr = (uint32_t)entry->nova;
        *out_result = entry->flags;
        ++compat_translations;

        if (entry->flags == SCTRANS_UNSUPPORTED) {
            ++compat_unsupported;
        } else if (entry->flags == SCTRANS_BLOCKED) {
            ++compat_blocked;
        }
        return;
    }

    *out_nova_nr = (uint32_t)-1;
    *out_result = SCTRANS_UNSUPPORTED;
    ++compat_unsupported;
}

uint32_t compat_detect_file_format(const uint8_t *header, uint32_t len) {
    if (len == 0) {
        return FILEFMT_UNKNOWN;
    }

    if (len >= 5 && header[0] == 0x7Fu && header[1] == 'E' && header[2] == 'L' &&
        header[3] == 'F') {
        if (header[4] == 1) return FILEFMT_ELF32;
        if (header[4] == 2) return FILEFMT_ELF64;
        return FILEFMT_UNKNOWN;
    }

    if (len >= 2 && header[0] == 'M' && header[1] == 'Z') {
        return FILEFMT_PE32; /* PE32 oder PE32+, genauere Pruefung noetig */
    }

    if (len >= 4 && header[0] == 'N' && header[1] == 'K' && header[2] == 'I' &&
        (header[3] == '1' || header[3] == '2')) {
        return FILEFMT_NOVABIN;
    }

    if (len >= 2 && header[0] == '#' && header[1] == '!') {
        return FILEFMT_SCRIPT;
    }

    return FILEFMT_UNKNOWN;
}

int compat_get_personality(uint32_t pers_id, void **out_record) {
    for (uint32_t i = 0; i < PERS_CAPACITY; ++i) {
        if (compat_personalities[i].id == pers_id) {
            if (out_record) {
                *out_record = &compat_personalities[i];
            }
            return 0;
        }
    }
    return -1;
}

int compat_introspect(uint32_t pers_id, uint32_t *out_sc_count, uint32_t *out_pers_flags) {
    void *record = 0;
    if (compat_get_personality(pers_id, &record) != 0) {
        if (out_sc_count) *out_sc_count = 0;
        if (out_pers_flags) *out_pers_flags = 0;
        return -1;
    }
    compat_personality_t *pers = (compat_personality_t *)record;
    if (out_sc_count) *out_sc_count = pers->syscall_count;
    if (out_pers_flags) *out_pers_flags = pers->flags;
    return 0;
}

int compat_self_test(void) {
    if (compat_ready != 1) return -1;

    uint8_t header[8] = {0x7Fu, 'E', 'L', 'F', 2, 0, 0, 0}; /* ELF64 */
    if (compat_detect_file_format(header, sizeof(header)) != FILEFMT_ELF64) return -1;

    uint32_t nova_nr = 0, result = 0;
    compat_translate_syscall(LINUX_SYS_READ, PERS_LINUX, &nova_nr, &result);
    if (nova_nr != NOVA_SYS_READ) return -1;

    uint32_t sc_count = 0, pers_flags = 0;
    if (compat_introspect(PERS_LINUX, &sc_count, &pers_flags) != 0) return -1;

    return 0;
}
