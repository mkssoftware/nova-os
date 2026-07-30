#ifndef NOVA_SECURITY_H
#define NOVA_SECURITY_H
#include <stdint.h>
#define NOVA_SECURITY_ABI_MAJOR 1u
#define NOVA_SECURITY_ABI_MINOR 0u
#define NOVA_CAP_MEMORY  (1u << 0)
#define NOVA_CAP_IO      (1u << 1)
#define NOVA_CAP_SERVICE (1u << 2)
#define NOVA_CAP_ADMIN   (1u << 3)
typedef struct nova_security_record {
    uint32_t ProcessId;
    uint32_t Capabilities;
} nova_security_record_t;
typedef struct nova_security_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t KernelCapabilities;
    uint32_t GrantEntry;
    uint32_t RevokeEntry;
    uint32_t CheckEntry;
    uint32_t TableAddress;
} nova_security_api_t;
_Static_assert(sizeof(nova_security_record_t) == 8,
               "nova_security_record_t ABI size changed");
_Static_assert(sizeof(nova_security_api_t) == 32,
               "nova_security_api_t ABI size changed");
#endif
