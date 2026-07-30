#ifndef NOVA_KERNEL_PMM_H
#define NOVA_KERNEL_PMM_H

#include <stdint.h>

#define NOVA_PMM_ABI_MAJOR 1u
#define NOVA_PMM_ABI_MINOR 0u
#define NOVA_PMM_PAGE_SIZE 4096u

#define NOVA_PMM_CAP_E820        (1u << 0)
#define NOVA_PMM_CAP_LIFO_FRAMES (1u << 1)

/*
 * Öffentliche 32-Bit-Bootstrap-API gemäß ADR-2001.
 * Funktionsadressen werden als uint32_t transportiert, damit die Struktur
 * unabhängig von der Zeigerbreite des Werkzeugs exakt 32 Byte groß bleibt.
 */
typedef struct NovaPmmApiV1 {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t PageSize;
    uint32_t Capabilities;
    uint32_t AllocPageEntry;
    uint32_t FreePageEntry;
    uint32_t TotalPages;
    uint32_t AvailablePages;
} NovaPmmApiV1;

_Static_assert(sizeof(NovaPmmApiV1) == 32, "NovaPmmApiV1 ABI size");

#endif
