#ifndef NOVA_KERNEL_DMA_MAPPING_H
#define NOVA_KERNEL_DMA_MAPPING_H

#include <stdint.h>

#define NOVA_DMA_MAPPING_ABI_MAJOR 1u
#define NOVA_DMA_MAPPING_ABI_MINOR 0u
#define NOVA_DMA_MAPPING_CAPACITY  4u

#define NOVA_DMA_PERMISSION_READ   (1u << 0)
#define NOVA_DMA_PERMISSION_WRITE  (1u << 1)

#define NOVA_DMA_FLAG_IOMMU       (1u << 0)
#define NOVA_DMA_FLAG_BOUNCE      (1u << 1)
#define NOVA_DMA_FLAG_COHERENT    (1u << 2)
#define NOVA_DMA_FLAG_RESTRICTED  (1u << 3)

typedef enum nova_dma_direction {
    NOVA_DMA_TO_DEVICE = 1,
    NOVA_DMA_FROM_DEVICE = 2,
    NOVA_DMA_BIDIRECTIONAL = 3
} nova_dma_direction_t;

typedef enum nova_dma_mapping_state {
    NOVA_DMA_MAPPING_EMPTY = 0,
    NOVA_DMA_MAPPING_ACTIVE = 1,
    NOVA_DMA_MAPPING_UNMAPPED = 2,
    NOVA_DMA_MAPPING_FAULTED = 3
} nova_dma_mapping_state_t;

typedef struct nova_dma_mapping_record {
    uint32_t MappingId;
    uint32_t OwnerProcessId;
    uint32_t DeviceId;
    uint32_t BufferId;
    uint32_t RequestId;
    uint32_t Direction;
    uint32_t Permissions;
    uint32_t State;
    uint32_t Length;
    uint32_t DeviceAddressLow;
    uint32_t DeviceAddressHigh;
    uint32_t DomainId;
    uint32_t Flags;
    uint32_t PinnedPages;
    uint32_t Generation;
    uint32_t ErrorCode;
} nova_dma_mapping_record_t;

typedef struct nova_dma_mapping_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t MapEntry;
    uint32_t UnmapEntry;
    uint32_t FaultEntry;
    uint32_t TableAddress;
    uint32_t StatusAddress;
} nova_dma_mapping_api_t;

_Static_assert(sizeof(nova_dma_mapping_record_t) == 64,
               "nova_dma_mapping_record_t ABI size changed");
_Static_assert(sizeof(nova_dma_mapping_api_t) == 32,
               "nova_dma_mapping_api_t ABI size changed");

#endif
