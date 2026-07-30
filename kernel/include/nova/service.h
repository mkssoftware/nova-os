#ifndef NOVA_SERVICE_H
#define NOVA_SERVICE_H
#include <stdint.h>
#include <nova/object.h>
#define NOVA_SERVICE_ABI_MAJOR 1u
#define NOVA_SERVICE_ABI_MINOR 0u
#define NOVA_SERVICE_CAPACITY 8u
typedef enum nova_service_state {
    NOVA_SERVICE_EMPTY = 0,
    NOVA_SERVICE_AVAILABLE = 1
} nova_service_state_t;
typedef struct nova_service_record {
    uint32_t ServiceId;
    uint32_t Version;
    uint32_t OwnerComponentId;
    uint32_t State;
    nova_handle_t ObjectHandle;
    uint32_t EndpointAddress;
    uint32_t Flags;
    uint32_t Reserved;
} nova_service_record_t;
typedef struct nova_service_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t RegisterEntry;
    uint32_t LookupEntry;
    uint32_t CountAddress;
    uint32_t TableAddress;
    uint32_t IpcApiAddress;
} nova_service_api_t;
_Static_assert(sizeof(nova_service_record_t) == 32,
               "nova_service_record_t ABI size changed");
_Static_assert(sizeof(nova_service_api_t) == 32,
               "nova_service_api_t ABI size changed");
#endif
