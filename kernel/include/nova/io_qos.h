#ifndef NOVA_KERNEL_IO_QOS_H
#define NOVA_KERNEL_IO_QOS_H

#include <stdint.h>

#define NOVA_IO_QOS_ABI_MAJOR 1u
#define NOVA_IO_QOS_ABI_MINOR 0u
#define NOVA_IO_QOS_CAPACITY  4u

#define NOVA_IO_QOS_CAP_HARD_SOFT  (1u << 0)
#define NOVA_IO_QOS_CAP_ADMISSION  (1u << 1)
#define NOVA_IO_QOS_CAP_DEGRADE    (1u << 2)
#define NOVA_IO_QOS_CAP_ACCOUNTING (1u << 3)

#define NOVA_IO_QOS_HARD_LATENCY    (1u << 0)
#define NOVA_IO_QOS_HARD_THROUGHPUT (1u << 1)
#define NOVA_IO_QOS_HARD_BANDWIDTH  (1u << 2)

typedef enum nova_io_qos_state {
    NOVA_IO_QOS_EMPTY = 0,
    NOVA_IO_QOS_ACCEPTED = 1,
    NOVA_IO_QOS_DEGRADED = 2,
    NOVA_IO_QOS_REJECTED = 3
} nova_io_qos_state_t;

typedef struct nova_io_qos_record {
    uint32_t ProfileId;
    uint32_t OwnerProcessId;
    uint32_t QosClass;
    uint32_t RequirementFlags;
    uint32_t LatencyTargetTicks;
    uint32_t MinimumThroughput;
    uint32_t MaximumBandwidth;
    uint32_t MaximumOutstanding;
    uint32_t ActiveRequests;
    uint32_t State;
    uint32_t LastObservedLatencyTicks;
    uint32_t ViolationCount;
    uint32_t CompletedBytesLow;
    uint32_t CompletedBytesHigh;
    uint32_t DegradationReason;
    uint32_t Reserved;
} nova_io_qos_record_t;

typedef struct nova_io_qos_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t Capabilities;
    uint32_t CreateEntry;
    uint32_t AttachEntry;
    uint32_t TableAddress;
    uint32_t RejectionCountAddress;
} nova_io_qos_api_t;

_Static_assert(sizeof(nova_io_qos_record_t) == 64,
               "nova_io_qos_record_t ABI size changed");
_Static_assert(sizeof(nova_io_qos_api_t) == 32,
               "nova_io_qos_api_t ABI size changed");

#endif
