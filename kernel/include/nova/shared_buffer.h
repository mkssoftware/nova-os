#ifndef NOVA_KERNEL_SHARED_BUFFER_H
#define NOVA_KERNEL_SHARED_BUFFER_H

#include <stdint.h>

#define NOVA_SHARED_BUFFER_ABI_MAJOR 1u
#define NOVA_SHARED_BUFFER_ABI_MINOR 0u
#define NOVA_SHARED_BUFFER_CAPACITY  4u

#define NOVA_BUFFER_RIGHT_READ     (1u << 0)
#define NOVA_BUFFER_RIGHT_WRITE    (1u << 1)
#define NOVA_BUFFER_RIGHT_TRANSFER (1u << 2)
#define NOVA_BUFFER_RIGHT_DMA      (1u << 3)
#define NOVA_BUFFER_RIGHT_RELEASE  (1u << 4)

typedef enum nova_shared_buffer_state {
    NOVA_BUFFER_EMPTY = 0,
    NOVA_BUFFER_CPU_OWNED = 1,
    NOVA_BUFFER_PROVIDER_OWNED = 2,
    NOVA_BUFFER_RELEASED = 3
} nova_shared_buffer_state_t;

typedef struct nova_shared_buffer_record {
    uint32_t BufferId;
    uint32_t OwnerProcessId;
    uint32_t Size;
    uint32_t State;
    uint32_t ReferenceCount;
    uint32_t ActiveIoCount;
    uint32_t Rights;
    uint32_t Flags;
    uint32_t MappingCount;
    uint32_t PinnedCount;
    uint32_t BackingKind;
    uint32_t ResourceBytes;
    uint32_t CopyFallbackCount;
    uint32_t ZeroCopyLeaseCount;
    uint32_t Generation;
    uint32_t Reserved;
} nova_shared_buffer_record_t;

typedef struct nova_shared_buffer_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t CreateEntry;
    uint32_t BindIoEntry;
    uint32_t CopyEntry;
    uint32_t ReleaseEntry;
    uint32_t TableAddress;
} nova_shared_buffer_api_t;

_Static_assert(sizeof(nova_shared_buffer_record_t) == 64,
               "nova_shared_buffer_record_t ABI size changed");
_Static_assert(sizeof(nova_shared_buffer_api_t) == 32,
               "nova_shared_buffer_api_t ABI size changed");

#endif
