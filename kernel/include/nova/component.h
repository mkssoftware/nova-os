#ifndef NOVA_COMPONENT_H
#define NOVA_COMPONENT_H

#include <stdint.h>
#include <nova/object.h>

#define NOVA_COMPONENT_ABI_MAJOR 1u
#define NOVA_COMPONENT_ABI_MINOR 0u
#define NOVA_COMPONENT_CAPACITY 8u

typedef enum nova_component_state {
    NOVA_COMPONENT_EMPTY = 0,
    NOVA_COMPONENT_REGISTERED = 1,
    NOVA_COMPONENT_ACTIVE = 2
} nova_component_state_t;

typedef struct nova_component_record {
    uint32_t ComponentId;
    uint32_t Version;
    uint32_t Flags;
    uint32_t State;
    nova_handle_t ObjectHandle;
    uint32_t Reserved[3];
} nova_component_record_t;

typedef struct nova_component_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t RegisterEntry;
    uint32_t LookupEntry;
    uint32_t ActivateEntry;
    uint32_t CountAddress;
    uint32_t TableAddress;
} nova_component_api_t;

_Static_assert(sizeof(nova_component_record_t) == 32,
               "nova_component_record_t ABI size changed");
_Static_assert(sizeof(nova_component_api_t) == 32,
               "nova_component_api_t ABI size changed");

#endif
