#ifndef NOVA_KERNEL_TASK_H
#define NOVA_KERNEL_TASK_H

#include <stdint.h>

#define NOVA_TASK_ABI_MAJOR 1u
#define NOVA_TASK_ABI_MINOR 0u
#define NOVA_TASK_CAPACITY  8u

#define NOVA_TASK_CAP_HIERARCHY            (1u << 0)
#define NOVA_TASK_CAP_COOPERATIVE_CANCEL   (1u << 1)
#define NOVA_TASK_CAP_TERMINAL_RESULT      (1u << 2)

typedef enum nova_task_state {
    NOVA_TASK_EMPTY = 0,
    NOVA_TASK_CREATED = 1,
    NOVA_TASK_READY = 2,
    NOVA_TASK_RUNNING = 3,
    NOVA_TASK_WAITING = 4,
    NOVA_TASK_CANCELLATION_REQUESTED = 5,
    NOVA_TASK_COMPLETED = 6,
    NOVA_TASK_CANCELLED = 7,
    NOVA_TASK_FAILED = 8
} nova_task_state_t;

typedef struct nova_task_record {
    uint32_t TaskId;
    uint32_t OwnerProcessId;
    uint32_t ScopeId;
    uint32_t State;
    uint32_t ParentTaskId;
    uint32_t Flags;
    uint32_t Result;
    uint32_t CancelReason;
} nova_task_record_t;

typedef struct nova_task_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t Capabilities;
    uint32_t CreateEntry;
    uint32_t RequestCancelEntry;
    uint32_t CheckpointEntry;
    uint32_t CompleteEntry;
} nova_task_api_t;

_Static_assert(sizeof(nova_task_record_t) == 32,
               "nova_task_record_t ABI size changed");
_Static_assert(sizeof(nova_task_api_t) == 32,
               "nova_task_api_t ABI size changed");

#endif
