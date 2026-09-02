#ifndef NOVA_BOOT_LOG_H
#define NOVA_BOOT_LOG_H

#include <stdint.h>

#define NOVA_BOOT_LOG_ABI_MAJOR 1u
#define NOVA_BOOT_LOG_ABI_MINOR 0u
#define NOVA_BOOT_LOG_CAPACITY  12u

typedef struct NovaBootEvent {
    uint64_t Timestamp;
    uint32_t Phase;
    uint32_t ComponentId;
    uint32_t Status;
    uint32_t Flags;
} NovaBootEvent;

typedef struct NovaBootLogApiV1 {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t EventSize;
    uint32_t CountAddress;
    uint32_t EventsAddress;
    uint32_t Reserved[2];
} NovaBootLogApiV1;

_Static_assert(sizeof(NovaBootEvent) == 24,
               "NovaBootEvent ABI size changed");
_Static_assert(sizeof(NovaBootLogApiV1) == 32,
               "NovaBootLogApiV1 ABI size changed");

#endif
