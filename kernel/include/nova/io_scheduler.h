#ifndef NOVA_KERNEL_IO_SCHEDULER_H
#define NOVA_KERNEL_IO_SCHEDULER_H

#include <stdint.h>

#define NOVA_IO_SCHEDULER_ABI_MAJOR 1u
#define NOVA_IO_SCHEDULER_ABI_MINOR 0u
#define NOVA_IO_PRIORITY_CLASS_COUNT 5u

#define NOVA_IO_SCHED_CAP_PRIORITY  (1u << 0)
#define NOVA_IO_SCHED_CAP_DEADLINE  (1u << 1)
#define NOVA_IO_SCHED_CAP_FAIRNESS  (1u << 2)
#define NOVA_IO_SCHED_CAP_BOUNDED   (1u << 3)

typedef struct nova_io_scheduler_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t PriorityClassCount;
    uint32_t Capabilities;
    uint32_t SelectEntry;
    uint32_t RequeueEntry;
    uint32_t OutstandingAddress;
    uint32_t DispatchCountAddress;
} nova_io_scheduler_api_t;

_Static_assert(sizeof(nova_io_scheduler_api_t) == 32,
               "nova_io_scheduler_api_t ABI size changed");

#endif
