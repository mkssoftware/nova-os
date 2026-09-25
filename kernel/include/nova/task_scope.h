#ifndef NOVA_KERNEL_TASK_SCOPE_H
#define NOVA_KERNEL_TASK_SCOPE_H

#include <stdint.h>

#define NOVA_TASK_SCOPE_ABI_MAJOR 1u
#define NOVA_TASK_SCOPE_ABI_MINOR 0u
#define NOVA_TASK_SCOPE_CAPACITY  8u

#define NOVA_TASK_SCOPE_CAP_HIERARCHY    (1u << 0)
#define NOVA_TASK_SCOPE_CAP_CANCELLATION (1u << 1)
#define NOVA_TASK_SCOPE_CAP_BOUNDED      (1u << 2)

typedef enum nova_task_scope_state {
    NOVA_TASK_SCOPE_EMPTY = 0,
    NOVA_TASK_SCOPE_OPEN = 1,
    NOVA_TASK_SCOPE_CANCELLING = 2,
    NOVA_TASK_SCOPE_CLOSED = 3
} nova_task_scope_state_t;

typedef struct nova_task_scope_record {
    uint32_t ScopeId;
    uint32_t OwnerProcessId;
    uint32_t ParentScopeId;
    uint32_t State;
    uint32_t ChildCount;
    uint32_t Flags;
    uint32_t CancelReason;
    uint32_t ActiveTaskCount;
} nova_task_scope_record_t;

typedef struct nova_task_scope_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t Capabilities;
    uint32_t CreateEntry;
    uint32_t CancelEntry;
    uint32_t CloseEntry;
    uint32_t TableAddress;
} nova_task_scope_api_t;

_Static_assert(sizeof(nova_task_scope_record_t) == 32,
               "nova_task_scope_record_t ABI size changed");
_Static_assert(sizeof(nova_task_scope_api_t) == 32,
               "nova_task_scope_api_t ABI size changed");

#endif
