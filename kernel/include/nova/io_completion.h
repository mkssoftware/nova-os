#ifndef NOVA_KERNEL_IO_COMPLETION_H
#define NOVA_KERNEL_IO_COMPLETION_H

#include <stdint.h>

#define NOVA_IO_COMPLETION_ABI_MAJOR 1u
#define NOVA_IO_COMPLETION_ABI_MINOR 0u
#define NOVA_IO_COMPLETION_CAPACITY  16u

#define NOVA_IO_COMPLETION_CAP_FIFO       (1u << 0)
#define NOVA_IO_COMPLETION_CAP_BATCH      (1u << 1)
#define NOVA_IO_COMPLETION_CAP_TIMESTAMP  (1u << 2)
#define NOVA_IO_COMPLETION_CAP_BOUNDED    (1u << 3)

typedef struct nova_io_completion_record {
    uint32_t RequestId;
    uint32_t OwnerProcessId;
    uint32_t Status;
    uint32_t BytesTransferred;
    uint32_t ErrorCode;
    uint32_t CompletedTick;
    uint32_t LatencyTicks;
    uint32_t Flags;
} nova_io_completion_record_t;

typedef struct nova_io_completion_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t Capabilities;
    uint32_t DequeueBatchEntry;
    uint32_t QueueAddress;
    uint32_t CountAddress;
    uint32_t OverflowCountAddress;
} nova_io_completion_api_t;

_Static_assert(sizeof(nova_io_completion_record_t) == 32,
               "nova_io_completion_record_t ABI size changed");
_Static_assert(sizeof(nova_io_completion_api_t) == 32,
               "nova_io_completion_api_t ABI size changed");

#endif
