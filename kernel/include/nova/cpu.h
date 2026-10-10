#ifndef NOVA_CPU_H
#define NOVA_CPU_H

#include <stdint.h>

#define NOVA_CPU_ABI_MAJOR 1u
#define NOVA_CPU_ABI_MINOR 0u
#define NOVA_CPU_CAPACITY  4u
#define NOVA_CPU_TOPOLOGY_UNKNOWN UINT32_C(0xFFFFFFFF)

typedef uint32_t nova_cpu_id_t;
typedef uint64_t nova_cpu_hardware_id_t;
typedef uint32_t nova_cpu_state_t;
typedef uint32_t nova_cpu_package_id_t;
typedef uint32_t nova_cpu_die_id_t;
typedef uint32_t nova_cpu_cluster_id_t;
typedef uint32_t nova_cpu_core_id_t;
typedef uint32_t nova_cpu_thread_id_t;
typedef uint32_t nova_cpu_numa_node_id_t;

typedef struct nova_cpu_record {
    nova_cpu_id_t CpuId;
    uint32_t HardwareIdLow;
    uint32_t HardwareIdHigh;
    nova_cpu_state_t State;
    nova_cpu_package_id_t PackageId;
    nova_cpu_die_id_t DieId;
    nova_cpu_cluster_id_t ClusterId;
    nova_cpu_core_id_t CoreId;
    nova_cpu_thread_id_t ThreadId;
    nova_cpu_numa_node_id_t NumaNodeId;
    uint32_t LogicalPackageThreads;
    uint32_t Capacity;
    uint32_t LocalDataAddress;
    uint32_t Flags;
    uint32_t Vendor0;
    uint32_t Vendor1;
    uint32_t Vendor2;
    uint32_t Signature;
    uint32_t RawFeatureEdx;
    uint32_t RawFeatureEcx;
    uint32_t Features;
    uint32_t LlcId;
    uint32_t TopologyNodeId;
    uint32_t TopologyGeneration;
    uint32_t Reserved0;
    uint32_t Reserved1;
    uint32_t Reserved2;
    uint32_t Reserved3;
} nova_cpu_record_t;

typedef struct nova_cpu_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t QueryEntry;
    uint32_t OfflineEntry;
    uint32_t DiscoveredCountAddress;
    uint32_t OnlineCountAddress;
    uint32_t SystemFeaturesAddress;
    uint32_t RecordsAddress;
    uint32_t OnlineSetAddress;
    uint32_t ActiveSetAddress;
    uint32_t LocalDataAddress;
} nova_cpu_api_t;

_Static_assert(sizeof(nova_cpu_record_t) == 112,
               "nova_cpu_record_t ABI size changed");
_Static_assert(sizeof(nova_cpu_api_t) == 48,
               "nova_cpu_api_t ABI size changed");

#endif
