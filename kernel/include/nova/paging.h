#ifndef NOVA_KERNEL_PAGING_H
#define NOVA_KERNEL_PAGING_H

#include <stdint.h>

#define NOVA_PAGING_ABI_MAJOR 1u
#define NOVA_PAGING_ABI_MINOR 0u
#define NOVA_PAGING_PAGE_SIZE 4096u

#define NOVA_PAGING_CAP_4K_PAGES (1u << 0)
#define NOVA_PAGING_CAP_IDENTITY (1u << 1)

typedef struct NovaPagingApiV1 {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t PageSize;
    uint32_t Capabilities;
    uint32_t MapPageEntry;
    uint32_t UnmapPageEntry;
    uint32_t DirectoryAddress;
    uint32_t EnabledAddress;
} NovaPagingApiV1;

_Static_assert(sizeof(NovaPagingApiV1) == 32, "NovaPagingApiV1 ABI size");

#endif
