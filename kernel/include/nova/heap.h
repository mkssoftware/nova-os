#ifndef NOVA_KERNEL_HEAP_H
#define NOVA_KERNEL_HEAP_H

#include <stdint.h>

#define NOVA_HEAP_ABI_MAJOR 1u
#define NOVA_HEAP_ABI_MINOR 0u
#define NOVA_HEAP_ALIGNMENT 16u

#define NOVA_HEAP_CAP_PAGE_BACKED (1u << 0)
#define NOVA_HEAP_CAP_ZEROED      (1u << 1)

typedef struct NovaHeapApiV1 {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Alignment;
    uint32_t Capabilities;
    uint32_t AllocateEntry;
    uint32_t FreeEntry;
    uint32_t AllocationCountAddress;
    uint32_t AllocatedBytesAddress;
} NovaHeapApiV1;

_Static_assert(sizeof(NovaHeapApiV1) == 32, "NovaHeapApiV1 ABI size");

#endif
