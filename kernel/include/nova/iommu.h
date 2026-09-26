#ifndef NOVA_KERNEL_IOMMU_H
#define NOVA_KERNEL_IOMMU_H

#include <stdint.h>

#define NOVA_IOMMU_ABI_MAJOR 1u
#define NOVA_IOMMU_ABI_MINOR 0u
#define NOVA_IOMMU_DOMAIN_CAPACITY 4u
#define NOVA_IOMMU_DEVICE_CAPACITY 8u
#define NOVA_IOMMU_MAPPING_CAPACITY 8u

#define NOVA_IOMMU_PERMISSION_READ  (1u << 0)
#define NOVA_IOMMU_PERMISSION_WRITE (1u << 1)

typedef enum nova_iommu_isolation_mode {
    NOVA_IOMMU_UNAVAILABLE = 0,
    NOVA_IOMMU_RESTRICTED = 1,
    NOVA_IOMMU_HARDWARE = 2,
    NOVA_IOMMU_VIRTUAL = 3
} nova_iommu_isolation_mode_t;

typedef enum nova_iommu_domain_state {
    NOVA_IOMMU_DOMAIN_EMPTY = 0,
    NOVA_IOMMU_DOMAIN_ACTIVE = 1,
    NOVA_IOMMU_DOMAIN_QUIESCING = 2,
    NOVA_IOMMU_DOMAIN_RELEASED = 3
} nova_iommu_domain_state_t;

typedef struct nova_iommu_domain_record {
    uint32_t DomainId;
    uint32_t OwnerProcessId;
    uint32_t State;
    uint32_t IsolationMode;
    uint32_t DeviceCount;
    uint32_t GroupCount;
    uint32_t MappingCount;
    uint32_t FaultCount;
    uint32_t MappedBytes;
    uint32_t AddressWidth;
    uint32_t IovaBaseLow;
    uint32_t IovaBaseHigh;
    uint32_t IovaLimitLow;
    uint32_t IovaLimitHigh;
    uint32_t Generation;
    uint32_t Flags;
} nova_iommu_domain_record_t;

typedef struct nova_iommu_device_binding {
    uint32_t DeviceId;
    uint32_t DomainId;
    uint32_t GroupId;
    uint32_t OwnerProcessId;
    uint32_t State;
    uint32_t IsolationMode;
    uint32_t Generation;
    uint32_t Flags;
} nova_iommu_device_binding_t;

typedef struct nova_iommu_mapping_record {
    uint32_t AuthorizationId;
    uint32_t DomainId;
    uint32_t DeviceId;
    uint32_t ExternalMappingId;
    uint32_t IovaLow;
    uint32_t IovaHigh;
    uint32_t Length;
    uint32_t Permissions;
    uint32_t State;
    uint32_t Generation;
    uint32_t ErrorCode;
    uint32_t Reserved;
} nova_iommu_mapping_record_t;

typedef struct nova_iommu_fault_record {
    uint32_t Sequence;
    uint32_t DomainId;
    uint32_t DeviceId;
    uint32_t ExternalMappingId;
    uint32_t IovaLow;
    uint32_t IovaHigh;
    uint32_t Access;
    uint32_t ErrorCode;
} nova_iommu_fault_record_t;

typedef struct nova_iommu_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t DomainCapacity;
    uint32_t DeviceCapacity;
    uint32_t MappingCapacity;
    uint32_t CreateDomainEntry;
    uint32_t BindDeviceEntry;
    uint32_t AuthorizeMappingEntry;
    uint32_t RevokeMappingEntry;
    uint32_t ReleaseDomainEntry;
} nova_iommu_api_t;

_Static_assert(sizeof(nova_iommu_domain_record_t) == 64,
               "nova_iommu_domain_record_t ABI size changed");
_Static_assert(sizeof(nova_iommu_device_binding_t) == 32,
               "nova_iommu_device_binding_t ABI size changed");
_Static_assert(sizeof(nova_iommu_mapping_record_t) == 48,
               "nova_iommu_mapping_record_t ABI size changed");
_Static_assert(sizeof(nova_iommu_fault_record_t) == 32,
               "nova_iommu_fault_record_t ABI size changed");
_Static_assert(sizeof(nova_iommu_api_t) == 40,
               "nova_iommu_api_t ABI size changed");

#endif
