#ifndef NOVA_OBJECT_H
#define NOVA_OBJECT_H

#include <stdint.h>

#define NOVA_OBJECT_ABI_MAJOR 1u
#define NOVA_OBJECT_ABI_MINOR 0u
#define NOVA_OBJECT_TABLE_CAPACITY 32u

typedef uint32_t nova_handle_t;

typedef struct nova_object_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t CreateEntry;
    uint32_t LookupEntry;
    uint32_t RetainEntry;
    uint32_t ReleaseEntry;
    uint32_t LiveCountAddress;
} nova_object_api_t;

_Static_assert(sizeof(nova_object_api_t) == 32,
               "nova_object_api_t ABI size changed");

#endif
