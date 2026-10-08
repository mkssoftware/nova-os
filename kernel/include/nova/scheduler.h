#ifndef NOVA_KERNEL_SCHEDULER_H
#define NOVA_KERNEL_SCHEDULER_H

#include <stdint.h>

#define NOVA_SCHEDULER_ABI_MAJOR 1u
#define NOVA_SCHEDULER_ABI_MINOR 0u

#define NOVA_SCHEDULER_CAP_PREEMPTIVE (1u << 0)
#define NOVA_SCHEDULER_CAP_ROUND_ROBIN (1u << 1)
#define NOVA_SCHEDULER_CAP_AFFINITY (1u << 2)
#define NOVA_SCHEDULER_CAP_DYNAMIC_THREADS (1u << 3)

typedef struct NovaSchedulerApiV1 {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t ThreadCount;
    uint32_t Capabilities;
    uint32_t OnTickEntry;
    uint32_t CurrentThreadAddress;
    uint32_t Thread1RunsAddress;
    uint32_t Thread2RunsAddress;
    uint32_t ThreadCpuAffinityAddress;
    uint32_t ThreadRunningOnAddress;
    uint32_t SchedulerContextsAddress;
    uint32_t ThreadTaskIdsAddress;
} NovaSchedulerApiV1;

_Static_assert(sizeof(NovaSchedulerApiV1) == 48, "NovaSchedulerApiV1 ABI size");

#endif
