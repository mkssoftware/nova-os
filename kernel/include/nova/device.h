#ifndef NOVA_DEVICE_H
#define NOVA_DEVICE_H

#include <stdint.h>

#define NOVA_DEVICE_ABI_MAJOR 1u
#define NOVA_DEVICE_ABI_MINOR 0u
#define NOVA_DEVICE_CAPACITY  8u

typedef struct NovaDeviceRecord {
    uint32_t DeviceId;
    uint32_t DeviceClass;
    uint32_t State;
    uint32_t ObjectHandle;
    uint32_t ResourceBase;
    uint32_t ResourceSize;
    uint32_t Flags;
    uint32_t DriverId;
} NovaDeviceRecord;

typedef struct NovaDeviceApiV1 {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t RegisterEntry;
    uint32_t LookupEntry;
    uint32_t CountAddress;
    uint32_t RecordsAddress;
    uint32_t Reserved;
} NovaDeviceApiV1;

_Static_assert(sizeof(NovaDeviceRecord) == 32,
               "NovaDeviceRecord ABI size changed");
_Static_assert(sizeof(NovaDeviceApiV1) == 32,
               "NovaDeviceApiV1 ABI size changed");
#endif
