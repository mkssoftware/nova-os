#ifndef NOVA_KERNEL_PMM_H
#define NOVA_KERNEL_PMM_H

#include <stdint.h>

#define NOVA_PMM_ABI_MAJOR 1u
#define NOVA_PMM_ABI_MINOR 1u
#define NOVA_PMM_PAGE_SIZE 4096u
#define NOVA_PMM_NUMA_UNKNOWN UINT32_C(0xFFFFFFFF)

#define NOVA_PMM_CAP_E820        (1u << 0)
#define NOVA_PMM_CAP_LIFO_FRAMES (1u << 1)
#define NOVA_PMM_CAP_NUMA_TAGGED  (1u << 2)
#define NOVA_PMM_CAP_NUMA_PREFERRED (1u << 3)
#define NOVA_PMM_CAP_NUMA_STRICT  (1u << 4)

/*
 * Öffentliche 32-Bit-Bootstrap-API gemäß ADR-2001.
 * Funktionsadressen werden als uint32_t transportiert, damit die Struktur
 * unabhängig von der Zeigerbreite des Werkzeugs exakt 48 Byte groß bleibt.
 * Der 32-Byte-Präfix aus ABI 1.0 bleibt unverändert.
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
    uint32_t AllocPreferredEntry;
    uint32_t AllocStrictEntry;
    uint32_t NodeForPageEntry;
    uint32_t UnknownNumaNode;
} NovaPmmApiV1;

_Static_assert(sizeof(NovaPmmApiV1) == 48, "NovaPmmApiV1 ABI size");

#endif
