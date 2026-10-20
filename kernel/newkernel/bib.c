/* NovaOS C-Kernel - Boot Info Block (BIB) Parsing (Port aus hal32.inc). */
#include "bib.h"
#include <nova_boot_protocol.h>
#include <stddef.h>

#define BOOT_INFO_CAPACITY 0x800u

static bib_graphics_info_t graphics_info = {0};
static bib_memory_info_t memory_info = {0};
static bib_kernel_info_t kernel_info = {0};

static uint32_t crc32_bib(const uint8_t *data, uint32_t length) {
    uint32_t crc = 0xFFFFFFFFu;
    uint32_t checksum_offset = (uint32_t)offsetof(nova_bib_header_t, checksum);

    for (uint32_t i = 0; i < length; ++i) {
        uint8_t byte = (i >= checksum_offset && i < checksum_offset + 4) ? 0 : data[i];
        crc ^= byte;
        for (int bit = 0; bit < 8; ++bit) {
            crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320u : 0);
        }
    }
    return ~crc;
}

const bib_graphics_info_t *bib_graphics(void) {
    return &graphics_info;
}

const bib_memory_info_t *bib_memory(void) {
    return &memory_info;
}

const bib_kernel_info_t *bib_kernel(void) {
    return &kernel_info;
}

#define NOVA_MEMORY_MAP_ENTRY_SIZE 24u

int bib_parse(uint32_t bib_physical_address) {
    graphics_info = (bib_graphics_info_t){0};
    memory_info = (bib_memory_info_t){0};
    kernel_info = (bib_kernel_info_t){0};

    if (bib_physical_address == 0 || (bib_physical_address & 7u) != 0) {
        return -1;
    }

    const nova_bib_header_t *header = (const nova_bib_header_t *)(uintptr_t)bib_physical_address;
    static const uint8_t expected_magic[8] = {'N', 'B', 'H', 'P', 'B', 'I', 'B', 0};
    for (int i = 0; i < 8; ++i) {
        if (header->magic[i] != expected_magic[i]) return -1;
    }
    if (header->version_major != NOVA_BIB_VERSION_MAJOR) return -1;
    if (header->header_size != NOVA_BIB_HEADER_SIZE) return -1;
    if (header->architecture != NOVA_BOOT_ARCH_X86_32) return -1;

    uint32_t total_size = header->total_size;
    if (total_size < NOVA_BIB_HEADER_SIZE || total_size > BOOT_INFO_CAPACITY) return -1;
    if ((total_size & 7u) != 0) return -1;

    uint32_t computed = crc32_bib((const uint8_t *)header, total_size);
    if (computed != header->checksum) return -1;

    const uint8_t *cursor = (const uint8_t *)header + NOVA_BIB_HEADER_SIZE;
    const uint8_t *end = (const uint8_t *)header + total_size;

    while (cursor != end) {
        if (cursor > end) return -1;
        const nova_bib_tlv_header_t *tlv = (const nova_bib_tlv_header_t *)cursor;
        if ((tlv->length & 7u) != 0) return -1;
        const uint8_t *value = cursor + sizeof(nova_bib_tlv_header_t);
        const uint8_t *next = value + tlv->length;
        if (next > end) return -1;

        if (tlv->type == NOVA_BIB_TLV_GRAPHICS) {
            if (tlv->length >= sizeof(nova_bib_graphics_t)) {
                const nova_bib_graphics_t *g = (const nova_bib_graphics_t *)value;
                graphics_info.has_graphics = 1;
                graphics_info.framebuffer_address = g->framebuffer_address;
                graphics_info.pitch = g->pitch;
                graphics_info.width = g->width;
                graphics_info.height = g->height;
                graphics_info.bits_per_pixel = g->bits_per_pixel;
                graphics_info.pixel_format = g->pixel_format;
            }
        } else if (tlv->type == NOVA_BIB_TLV_MEMORY) {
            if (tlv->length >= sizeof(nova_bib_memory_t)) {
                const nova_bib_memory_t *m = (const nova_bib_memory_t *)value;
                if (m->entry_size == NOVA_MEMORY_MAP_ENTRY_SIZE && m->entry_count != 0) {
                    memory_info.has_memory_map = 1;
                    memory_info.map_address = m->map_address;
                    memory_info.entry_count = m->entry_count;
                }
            }
        } else if (tlv->type == NOVA_BIB_TLV_KERNEL) {
            if (tlv->length >= sizeof(nova_bib_kernel_t)) {
                const nova_bib_kernel_t *k = (const nova_bib_kernel_t *)value;
                kernel_info.has_kernel = 1;
                kernel_info.load_address = k->load_address;
                kernel_info.image_size = k->image_size;
            }
        }

        cursor = next;
    }

    return 0;
}
