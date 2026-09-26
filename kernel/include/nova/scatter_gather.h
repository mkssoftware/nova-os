#ifndef NOVA_KERNEL_SCATTER_GATHER_H
#define NOVA_KERNEL_SCATTER_GATHER_H

#include <stdint.h>

#define NOVA_SCATTER_GATHER_ABI_MAJOR 1u
#define NOVA_SCATTER_GATHER_ABI_MINOR 0u
#define NOVA_SCATTER_GATHER_CAPACITY 4u
#define NOVA_SCATTER_GATHER_MAX_SEGMENTS 4u

#define NOVA_SG_PERMISSION_READ  (1u << 0)
#define NOVA_SG_PERMISSION_WRITE (1u << 1)

#define NOVA_SG_FLAG_COALESCED       (1u << 0)
#define NOVA_SG_FLAG_DIRECT_ELIGIBLE (1u << 1)
#define NOVA_SG_FLAG_FALLBACK_ALLOWED (1u << 2)

typedef enum nova_scatter_gather_direction {
    NOVA_SG_GATHER = 1,
    NOVA_SG_SCATTER = 2,
    NOVA_SG_BIDIRECTIONAL = 3
} nova_scatter_gather_direction_t;

typedef enum nova_scatter_gather_state {
    NOVA_SG_EMPTY = 0,
    NOVA_SG_BUILDING = 1,
    NOVA_SG_SEALED = 2,
    NOVA_SG_RELEASED = 3
} nova_scatter_gather_state_t;

typedef struct nova_scatter_gather_segment {
    uint32_t BufferId;
    uint32_t Offset;
    uint32_t Length;
    uint32_t Permissions;
    uint32_t LogicalOffset;
    uint32_t Flags;
    uint32_t Generation;
    uint32_t Reserved;
} nova_scatter_gather_segment_t;

typedef struct nova_scatter_gather_record {
    uint32_t DescriptorId;
    uint32_t OwnerProcessId;
    uint32_t Direction;
    uint32_t State;
    uint32_t SegmentCount;
    uint32_t TotalLength;
    uint32_t MaximumSegments;
    uint32_t Flags;
    uint32_t ActiveConsumers;
    uint32_t RetainedReferences;
    uint32_t CoalescedSegments;
    uint32_t SplitSegments;
    uint32_t FallbackCopies;
    uint32_t Generation;
    uint32_t ValidationFailures;
    uint32_t Reserved;
} nova_scatter_gather_record_t;

typedef struct nova_scatter_gather_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t DescriptorCapacity;
    uint32_t SegmentsPerDescriptor;
    uint32_t CreateEntry;
    uint32_t AppendEntry;
    uint32_t SealEntry;
    uint32_t ReleaseEntry;
} nova_scatter_gather_api_t;

_Static_assert(sizeof(nova_scatter_gather_segment_t) == 32,
               "nova_scatter_gather_segment_t ABI size changed");
_Static_assert(sizeof(nova_scatter_gather_record_t) == 64,
               "nova_scatter_gather_record_t ABI size changed");
_Static_assert(sizeof(nova_scatter_gather_api_t) == 32,
               "nova_scatter_gather_api_t ABI size changed");

#endif
