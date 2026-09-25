#ifndef NOVA_KERNEL_TASK_GROUP_H
#define NOVA_KERNEL_TASK_GROUP_H

#include <stdint.h>

#define NOVA_TASK_GROUP_ABI_MAJOR 1u
#define NOVA_TASK_GROUP_ABI_MINOR 0u
#define NOVA_TASK_GROUP_CAPACITY  4u

#define NOVA_TASK_GROUP_CAP_WAIT_ALL     (1u << 0)
#define NOVA_TASK_GROUP_CAP_FAIL_FAST    (1u << 1)
#define NOVA_TASK_GROUP_CAP_CANCELLATION (1u << 2)
#define NOVA_TASK_GROUP_CAP_DRAIN        (1u << 3)

typedef enum nova_task_group_state {
    NOVA_TASK_GROUP_EMPTY = 0,
    NOVA_TASK_GROUP_OPEN = 1,
    NOVA_TASK_GROUP_CANCELLING = 2,
    NOVA_TASK_GROUP_FAILING = 3,
    NOVA_TASK_GROUP_COMPLETED = 4,
    NOVA_TASK_GROUP_CANCELLED = 5,
    NOVA_TASK_GROUP_FAILED = 6
} nova_task_group_state_t;

typedef enum nova_task_group_policy {
    NOVA_TASK_GROUP_WAIT_ALL = 1,
    NOVA_TASK_GROUP_FAIL_FAST = 2
} nova_task_group_policy_t;

typedef struct nova_task_group_record {
    uint32_t GroupId;
    uint32_t OwnerProcessId;
    uint32_t ScopeId;
    uint32_t State;
    uint32_t Policy;
    uint32_t RequiredTaskCount;
    uint32_t ActiveTaskCount;
    uint32_t FailureCount;
} nova_task_group_record_t;

typedef struct nova_task_group_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t Capabilities;
    uint32_t CreateEntry;
    uint32_t AttachEntry;
    uint32_t CancelEntry;
    uint32_t TableAddress;
} nova_task_group_api_t;

_Static_assert(sizeof(nova_task_group_record_t) == 32,
               "nova_task_group_record_t ABI size changed");
_Static_assert(sizeof(nova_task_group_api_t) == 32,
               "nova_task_group_api_t ABI size changed");

#endif
