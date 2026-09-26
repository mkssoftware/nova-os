#ifndef NOVA_KERNEL_DMA_SCATTER_GATHER_H
#define NOVA_KERNEL_DMA_SCATTER_GATHER_H

#include <stdint.h>

#define NOVA_DMA_SG_ABI_MAJOR 1u
#define NOVA_DMA_SG_ABI_MINOR 0u
#define NOVA_DMA_SG_CAPACITY 2u
#define NOVA_DMA_SG_MAX_SEGMENTS 4u

#define NOVA_DMA_SG_FLAG_BOUNCE     (1u << 0)
#define NOVA_DMA_SG_FLAG_COHERENT   (1u << 1)
#define NOVA_DMA_SG_FLAG_RESTRICTED (1u << 2)
#define NOVA_DMA_SG_FLAG_SPLIT      (1u << 3)

typedef enum nova_dma_sg_state {
    NOVA_DMA_SG_EMPTY = 0,
    NOVA_DMA_SG_ACTIVE = 1,
    NOVA_DMA_SG_UNMAPPED = 2,
    NOVA_DMA_SG_FAULTED = 3
} nova_dma_sg_state_t;

typedef struct nova_dma_sg_segment {
    uint32_t SourceSegmentIndex;
    uint32_t BufferId;
    uint32_t BufferOffset;
    uint32_t Length;
    uint32_t DeviceAddressLow;
    uint32_t DeviceAddressHigh;
    uint32_t Permissions;
    uint32_t Flags;
} nova_dma_sg_segment_t;

typedef struct nova_dma_sg_mapping_record {
    uint32_t MappingId;
    uint32_t OwnerProcessId;
    uint32_t DeviceId;
    uint32_t RequestId;
    uint32_t DescriptorId;
    uint32_t Direction;
    uint32_t State;
    uint32_t SegmentCount;
    uint32_t TotalLength;
    uint32_t MaximumSegmentSize;
    uint32_t RequiredAlignment;
    uint32_t AddressWidth;
    uint32_t Boundary;
    uint32_t Flags;
    uint32_t Generation;
    uint32_t ErrorCode;
} nova_dma_sg_mapping_record_t;

typedef struct nova_dma_sg_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t MappingCapacity;
    uint32_t SegmentsPerMapping;
    uint32_t MapEntry;
    uint32_t UnmapEntry;
    uint32_t MappingTableAddress;
    uint32_t SegmentTableAddress;
} nova_dma_sg_api_t;

_Static_assert(sizeof(nova_dma_sg_segment_t) == 32,
               "nova_dma_sg_segment_t ABI size changed");
_Static_assert(sizeof(nova_dma_sg_mapping_record_t) == 64,
               "nova_dma_sg_mapping_record_t ABI size changed");
_Static_assert(sizeof(nova_dma_sg_api_t) == 32,
               "nova_dma_sg_api_t ABI size changed");

#endif
