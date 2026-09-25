#ifndef NOVA_KERNEL_TASK_DEADLINE_H
#define NOVA_KERNEL_TASK_DEADLINE_H

#include <stdint.h>

#define NOVA_TASK_DEADLINE_ABI_MAJOR 1u
#define NOVA_TASK_DEADLINE_ABI_MINOR 0u

#define NOVA_DEADLINE_CAP_MONOTONIC      (1u << 0)
#define NOVA_DEADLINE_CAP_HARD_FIRM_SOFT (1u << 1)
#define NOVA_DEADLINE_CAP_PARENT_CLAMP   (1u << 2)
#define NOVA_DEADLINE_CAP_CANCEL_ON_MISS (1u << 3)

typedef enum nova_deadline_class {
    NOVA_DEADLINE_HARD = 1,
    NOVA_DEADLINE_FIRM = 2,
    NOVA_DEADLINE_SOFT = 3
} nova_deadline_class_t;

typedef enum nova_deadline_miss_policy {
    NOVA_DEADLINE_MISS_CONTINUE = 1,
    NOVA_DEADLINE_MISS_CANCEL = 2,
    NOVA_DEADLINE_MISS_FAIL = 3
} nova_deadline_miss_policy_t;

typedef enum nova_deadline_state {
    NOVA_DEADLINE_NONE = 0,
    NOVA_DEADLINE_ARMED = 1,
    NOVA_DEADLINE_MISSED = 2
} nova_deadline_state_t;

typedef struct nova_task_deadline_record {
    uint32_t AbsoluteTick;
    uint32_t Class;
    uint32_t MissPolicy;
    uint32_t State;
} nova_task_deadline_record_t;

typedef struct nova_task_deadline_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t ClockHz;
    uint32_t Capabilities;
    uint32_t SetEntry;
    uint32_t PollEntry;
    uint32_t TableAddress;
    uint32_t MissCountAddress;
} nova_task_deadline_api_t;

_Static_assert(sizeof(nova_task_deadline_record_t) == 16,
               "nova_task_deadline_record_t ABI size changed");
_Static_assert(sizeof(nova_task_deadline_api_t) == 32,
               "nova_task_deadline_api_t ABI size changed");

#endif
