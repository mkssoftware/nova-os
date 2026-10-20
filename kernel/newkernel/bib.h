/* NovaOS C-Kernel - Boot Info Block (BIB) Parsing (Port aus hal32.inc,
   NPSPEC-HAL-FIRMWARE-0001). Nutzt die geteilten Protokoll-Structs aus
   boot/include/nova_boot_protocol.h. */
#ifndef NOVA_BIB_H
#define NOVA_BIB_H

#include <stdint.h>

typedef struct {
    int has_graphics;
    uint32_t framebuffer_address;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint32_t bits_per_pixel;
    uint32_t pixel_format;
} bib_graphics_info_t;

typedef struct {
    int has_memory_map;
    uint32_t map_address; /* Zeiger auf Array von bib_memory_map_entry_t */
    uint32_t entry_count;
} bib_memory_info_t;

/* 24-Byte-Eintrag, identisch zum alten ASM-Layout (MEMORY_MAP_ENTRY_SIZE). */
typedef struct {
    uint32_t base_low;
    uint32_t base_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type; /* 1 = nutzbares RAM */
    uint32_t reserved;
} bib_memory_map_entry_t;

typedef struct {
    int has_kernel;
    uint32_t load_address;
    uint32_t image_size;
} bib_kernel_info_t;

/* Validiert den BIB (Magic, Version, Header-Groesse, Architektur, CRC32)
   und extrahiert Graphics-, Memory- und Kernel-TLV, falls vorhanden.
   0 = BIB gueltig. */
int bib_parse(uint32_t bib_physical_address);

/* Nur gueltig, wenn bib_parse() vorher 0 zurueckgegeben hat UND
   has_graphics==1 (sonst alles 0). */
const bib_graphics_info_t *bib_graphics(void);

const bib_memory_info_t *bib_memory(void);
const bib_kernel_info_t *bib_kernel(void);

#endif
