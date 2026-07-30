#ifndef NOVA_BOOT_PROTOCOL_H
#define NOVA_BOOT_PROTOCOL_H

#include <stdint.h>

#define NOVA_NKI_VERSION              1u
#define NOVA_NKI_HEADER_SIZE          64u
#define NOVA_BIB_VERSION_MAJOR        1u
#define NOVA_BIB_HEADER_SIZE          32u
#define NOVA_BOOT_ARCH_X86_32         1u
#define NOVA_BOOT_FLAG_FRAMEBUFFER    0x00000001u

typedef enum nova_bib_tlv_type {
    NOVA_BIB_TLV_FIRMWARE    = 1,
    NOVA_BIB_TLV_MEMORY      = 3,
    NOVA_BIB_TLV_GRAPHICS    = 4,
    NOVA_BIB_TLV_KERNEL      = 6,
    NOVA_BIB_TLV_SECURITY    = 7,
    NOVA_BIB_TLV_BOOT_OPTIONS = 9
} nova_bib_tlv_type_t;

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

#pragma pack(pop)

_Static_assert(sizeof(nova_nki_header_t) == 64, "NKI-Header muss 64 Byte groß sein");
_Static_assert(sizeof(nova_bib_header_t) == 32, "BIB-Header muss 32 Byte groß sein");
_Static_assert(sizeof(nova_bib_tlv_header_t) == 8, "TLV-Header muss 8 Byte groß sein");
_Static_assert(sizeof(nova_bib_firmware_t) == 16, "Firmware-TLV muss 16 Byte groß sein");
_Static_assert(sizeof(nova_bib_memory_t) == 16, "Memory-TLV muss 16 Byte groß sein");
_Static_assert(sizeof(nova_bib_graphics_t) == 32, "Graphics-TLV muss 32 Byte groß sein");
_Static_assert(sizeof(nova_bib_kernel_t) == 32, "Kernel-TLV muss 32 Byte groß sein");

#endif
