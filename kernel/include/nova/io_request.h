#ifndef NOVA_KERNEL_IO_REQUEST_H
#define NOVA_KERNEL_IO_REQUEST_H

#include <stdint.h>

#define NOVA_IO_REQUEST_ABI_MAJOR 1u
#define NOVA_IO_REQUEST_ABI_MINOR 0u
#define NOVA_IO_REQUEST_CAPACITY  8u

#define NOVA_IO_CAP_ASYNC_COMPLETION (1u << 0)
#define NOVA_IO_CAP_BOUNDED_QUEUE    (1u << 1)
#define NOVA_IO_CAP_TASK_OWNERSHIP   (1u << 2)
#define NOVA_IO_CAP_DEADLINE         (1u << 3)

typedef enum nova_io_request_state {
    NOVA_IO_REQUEST_EMPTY = 0,
    NOVA_IO_REQUEST_PENDING = 1,
    NOVA_IO_REQUEST_RUNNING = 2,
    NOVA_IO_REQUEST_COMPLETED = 3,
    NOVA_IO_REQUEST_FAILED = 4,
    NOVA_IO_REQUEST_CANCELLED = 5
} nova_io_request_state_t;

typedef enum nova_io_completion_status {
    NOVA_IO_COMPLETION_NONE = 0,
    NOVA_IO_COMPLETION_SUCCESS = 1,
    NOVA_IO_COMPLETION_PARTIAL = 2,
    NOVA_IO_COMPLETION_FAILED = 3,
    NOVA_IO_COMPLETION_CANCELLED = 4,
    NOVA_IO_COMPLETION_DEADLINE_MISS = 5
} nova_io_completion_status_t;

typedef enum nova_io_priority_class {
    NOVA_IO_PRIORITY_DEFAULT = 0,
    NOVA_IO_PRIORITY_REALTIME = 1,
    NOVA_IO_PRIORITY_INTERACTIVE = 2,
    NOVA_IO_PRIORITY_NORMAL = 3,
    NOVA_IO_PRIORITY_BACKGROUND = 4,
    NOVA_IO_PRIORITY_MAINTENANCE = 5
} nova_io_priority_class_t;

typedef struct nova_io_request_record {
    uint32_t RequestId;
    uint32_t OwnerProcessId;
    uint32_t TaskId;
    uint32_t ScopeId;
    uint32_t Operation;
    uint32_t State;
    uint32_t TargetHandle;
    uint32_t BufferHandle;
    uint32_t Length;
    uint32_t BytesTransferred;
    uint32_t DeadlineTick;
    uint32_t RequestedPriority;
    uint32_t CompletionStatus;
    uint32_t ErrorCode;
    uint32_t Flags;
    uint32_t EffectivePriority;
} nova_io_request_record_t;

typedef struct nova_io_request_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t Capabilities;
    uint32_t SubmitEntry;
    uint32_t CompleteEntry;
    uint32_t CancelEntry;
    uint32_t TableAddress;
} nova_io_request_api_t;

_Static_assert(sizeof(nova_io_request_record_t) == 64,
               "nova_io_request_record_t ABI size changed");
_Static_assert(sizeof(nova_io_request_api_t) == 32,
               "nova_io_request_api_t ABI size changed");

#endif
