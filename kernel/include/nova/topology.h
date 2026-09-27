#ifndef NOVA_TOPOLOGY_H
#define NOVA_TOPOLOGY_H

#include <stdint.h>

#define NOVA_TOPOLOGY_ABI_MAJOR 1u
#define NOVA_TOPOLOGY_ABI_MINOR 0u
#define NOVA_TOPOLOGY_CAPACITY  32u

typedef uint32_t nova_topology_id_t;
typedef uint32_t nova_topology_type_t;
typedef uint32_t nova_topology_state_t;
typedef uint32_t nova_topology_hardware_id_t;
typedef uint32_t nova_numa_node_id_t;
typedef uint32_t nova_iommu_group_id_t;

enum {
    NOVA_TOPOLOGY_TYPE_SYSTEM = 1u,
    NOVA_TOPOLOGY_TYPE_NUMA_NODE = 2u,
    NOVA_TOPOLOGY_TYPE_CPU_PACKAGE = 3u,
    NOVA_TOPOLOGY_TYPE_CPU_CORE = 4u,
    NOVA_TOPOLOGY_TYPE_CPU_THREAD = 5u,
    NOVA_TOPOLOGY_TYPE_MEMORY_REGION = 6u,
    NOVA_TOPOLOGY_TYPE_CACHE = 7u,
    NOVA_TOPOLOGY_TYPE_INTERRUPT_CONTROLLER = 8u,
    NOVA_TOPOLOGY_TYPE_BUS = 9u,
    NOVA_TOPOLOGY_TYPE_IOMMU_GROUP = 10u,
    NOVA_TOPOLOGY_TYPE_DEVICE = 11u
};

enum {
    NOVA_TOPOLOGY_STATE_EMPTY = 0u,
    NOVA_TOPOLOGY_STATE_ONLINE = 1u,
    NOVA_TOPOLOGY_STATE_QUIESCING = 2u,
    NOVA_TOPOLOGY_STATE_OFFLINE = 3u,
    NOVA_TOPOLOGY_STATE_REMOVED = 4u
};

enum {
    NOVA_TOPOLOGY_FLAG_BOOTSTRAP = 1u << 0,
    NOVA_TOPOLOGY_FLAG_HOTPLUGGABLE = 1u << 1,
    NOVA_TOPOLOGY_FLAG_DMA_CAPABLE = 1u << 2,
    NOVA_TOPOLOGY_FLAG_LOCALITY_KNOWN = 1u << 3,
    NOVA_TOPOLOGY_FLAG_FIRMWARE_VALIDATED = 1u << 4,
    NOVA_TOPOLOGY_FLAG_FALLBACK = 1u << 5
};

typedef struct nova_topology_record {
    nova_topology_id_t TopologyId;
    nova_topology_type_t Type;
    nova_topology_id_t ParentId;
    nova_topology_state_t State;
    nova_topology_hardware_id_t HardwareId;
    nova_numa_node_id_t NumaNodeId;
    nova_iommu_group_id_t IommuGroupId;
    uint32_t ChildCount;
    uint32_t Flags;
    uint32_t Property0;
    uint32_t Property1;
    uint32_t Property2;
    uint32_t Generation;
    uint32_t ChangeSequence;
    uint32_t Reserved0;
    uint32_t Reserved1;
} nova_topology_record_t;

typedef struct nova_topology_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t RegisterEntry;
    uint32_t TransitionEntry;
    uint32_t LookupEntry;
    uint32_t DeviceGroupEntry;
    uint32_t RecordsAddress;
    uint32_t ChangeSequenceAddress;
    uint32_t Reserved;
} nova_topology_api_t;

_Static_assert(sizeof(nova_topology_record_t) == 64,
               "nova_topology_record_t ABI size changed");
_Static_assert(sizeof(nova_topology_api_t) == 40,
               "nova_topology_api_t ABI size changed");

#endif
