#ifndef NOVA_BOOT_PROTOCOL_H
#define NOVA_BOOT_PROTOCOL_H

#include <stdint.h>

#define NOVA_NKI_VERSION              1u
#define NOVA_NKI_HEADER_SIZE          64u
#define NOVA_NKI_FLAG_ELF_BUILD_ID    0x00000001u
#define NOVA_NKI_FLAG_NOVA_METADATA   0x00000002u
#define NOVA_LOADER_ABI_VERSION       0x00010000u
#define NOVA_BIB_VERSION_MAJOR        1u
#define NOVA_BIB_VERSION_MINOR        2u
#define NOVA_BIB_VERSION_PATCH        0u
#define NOVA_BIB_HEADER_SIZE          32u
#define NOVA_BOOT_ARCH_X86_32         1u
#define NOVA_BOOT_FLAG_FRAMEBUFFER    0x00000001u

typedef enum nova_bib_tlv_type {
    NOVA_BIB_TLV_FIRMWARE    = 1,
    NOVA_BIB_TLV_CPU         = 2,
    NOVA_BIB_TLV_MEMORY      = 3,
    NOVA_BIB_TLV_GRAPHICS    = 4,
    NOVA_BIB_TLV_STORAGE     = 5,
    NOVA_BIB_TLV_KERNEL      = 6,
    NOVA_BIB_TLV_SECURITY    = 7,
    NOVA_BIB_TLV_ACPI        = 8,
    NOVA_BIB_TLV_BOOT_OPTIONS = 9,
    NOVA_BIB_TLV_MODULES     = 10,
    NOVA_BIB_TLV_TIMING      = 11,
    NOVA_BIB_TLV_ENTROPY     = 12,
    NOVA_BIB_TLV_SYSTEM      = 13,
    NOVA_BIB_TLV_KERNEL_IDENTITY = 14
} nova_bib_tlv_type_t;

enum { NOVA_BIB_TLV_FLAG_REQUIRED = 0x0001u };

typedef enum nova_kernel_format {
    NOVA_KERNEL_FORMAT_NATIVE = 1,
    NOVA_KERNEL_FORMAT_ELF32 = 2,
    NOVA_KERNEL_FORMAT_ELF64 = 3
} nova_kernel_format_t;

#pragma pack(push, 1)

typedef struct nova_nki_header {
    uint8_t  magic[8];
    uint32_t version;
    uint32_t header_size;
    uint32_t architecture;
    uint32_t flags;
    uint32_t entry_point;
    uint32_t load_address;
    uint32_t image_size;
    uint32_t compression;
    uint32_t payload_crc32;
    uint8_t  build_id[16];
    uint32_t reserved;
} nova_nki_header_t;

typedef struct nova_bib_header {
    uint8_t  magic[8];
    uint16_t version_major;
    uint16_t version_minor;
    uint16_t version_patch;
    uint16_t header_size;
    uint32_t total_size;
    uint32_t checksum;
    uint32_t architecture;
    uint32_t flags;
} nova_bib_header_t;

typedef struct nova_bib_tlv_header {
    uint16_t type;
    uint16_t flags;
    uint32_t length;
} nova_bib_tlv_header_t;

typedef struct nova_bib_firmware {
    uint32_t platform;
    uint32_t boot_drive;
    uint32_t firmware_flags;
    uint32_t reserved;
} nova_bib_firmware_t;

typedef struct nova_bib_memory {
    uint32_t map_address;
    uint32_t entry_count;
    uint32_t entry_size;
    uint32_t reserved;
} nova_bib_memory_t;

typedef struct nova_bib_cpu {
    uint8_t  vendor[12];
    uint32_t highest_basic_leaf;
    uint32_t feature_edx;
    uint32_t feature_ecx;
    uint32_t cpu_flags;
    uint32_t reserved;
} nova_bib_cpu_t;

typedef struct nova_bib_graphics {
    uint32_t framebuffer_address;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint32_t bits_per_pixel;
    uint32_t pixel_format;
    uint32_t reserved[2];
} nova_bib_graphics_t;

typedef struct nova_bib_kernel {
    uint32_t load_address;
    uint32_t image_size;
    uint32_t entry_point;
    uint32_t image_version;
    uint32_t payload_crc32;
    uint32_t reserved[3];
} nova_bib_kernel_t;

typedef struct nova_bib_storage {
    uint32_t platform;
    uint32_t boot_drive;
    uint32_t storage_type;
    uint32_t flags;
    uint64_t partition_start_lba;
} nova_bib_storage_t;

typedef struct nova_bib_security {
    uint32_t verification_state;
    uint32_t secure_boot_state;
    uint32_t entropy_quality;
    uint32_t flags;
} nova_bib_security_t;

typedef struct nova_bib_pointer_info {
    uint64_t address;
    uint32_t flags;
    uint32_t reserved;
} nova_bib_pointer_info_t;

typedef struct nova_bib_modules {
    uint32_t array_address;
    uint32_t entry_count;
    uint32_t entry_size;
    uint32_t flags;
} nova_bib_modules_t;

typedef struct nova_bib_timing {
    uint64_t boot_tsc;
    uint64_t tsc_frequency_hz;
    uint32_t flags;
    uint32_t reserved;
} nova_bib_timing_t;

typedef struct nova_bib_entropy {
    uint8_t  seed[16];
    uint32_t source;
    uint32_t quality;
    uint32_t seed_size;
    uint32_t flags;
} nova_bib_entropy_t;

typedef struct nova_bib_system {
    uint64_t generation;
    uint32_t boot_attempt;
    uint32_t flags;
} nova_bib_system_t;

typedef struct nova_bib_kernel_identity {
    uint8_t  build_id[20];
    uint32_t kernel_format;
    uint32_t flags;
    uint32_t reserved;
} nova_bib_kernel_identity_t;

#pragma pack(pop)

_Static_assert(sizeof(nova_nki_header_t) == 64, "NKI-Header muss 64 Byte groß sein");
_Static_assert(sizeof(nova_bib_header_t) == 32, "BIB-Header muss 32 Byte groß sein");
_Static_assert(sizeof(nova_bib_tlv_header_t) == 8, "TLV-Header muss 8 Byte groß sein");
_Static_assert(sizeof(nova_bib_firmware_t) == 16, "Firmware-TLV muss 16 Byte groß sein");
_Static_assert(sizeof(nova_bib_memory_t) == 16, "Memory-TLV muss 16 Byte groß sein");
_Static_assert(sizeof(nova_bib_cpu_t) == 32, "CPU-TLV muss 32 Byte groß sein");
_Static_assert(sizeof(nova_bib_graphics_t) == 32, "Graphics-TLV muss 32 Byte groß sein");
_Static_assert(sizeof(nova_bib_kernel_t) == 32, "Kernel-TLV muss 32 Byte groß sein");
_Static_assert(sizeof(nova_bib_storage_t) == 24, "Storage-TLV muss 24 Byte groß sein");
_Static_assert(sizeof(nova_bib_security_t) == 16, "Security-TLV muss 16 Byte groß sein");
_Static_assert(sizeof(nova_bib_pointer_info_t) == 16, "Pointer-TLV muss 16 Byte groß sein");
_Static_assert(sizeof(nova_bib_modules_t) == 16, "Modules-TLV muss 16 Byte groß sein");
_Static_assert(sizeof(nova_bib_timing_t) == 24, "Timing-TLV muss 24 Byte groß sein");
_Static_assert(sizeof(nova_bib_entropy_t) == 32, "Entropy-TLV muss 32 Byte groß sein");
_Static_assert(sizeof(nova_bib_system_t) == 16, "System-TLV muss 16 Byte groß sein");

_Static_assert(sizeof(nova_bib_kernel_identity_t) == 32, "Kernel-Identity-TLV must be 32 bytes");

#endif
