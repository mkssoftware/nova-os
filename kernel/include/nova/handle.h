#ifndef NOVA_HANDLE_H
#define NOVA_HANDLE_H
#include <stdint.h>
#define NOVA_HANDLE_ABI_MAJOR 1u
#define NOVA_HANDLE_ABI_MINOR 0u
#define NOVA_HANDLE_CAPACITY 8u
#define NOVA_HANDLE_RIGHT_QUERY 0x00000001u
#define NOVA_HANDLE_RIGHT_WAIT 0x00000002u
#define NOVA_HANDLE_PROTECTED 0x00000010u
typedef struct NovaHandleEntryV1 {
    uint32_t KernelObjectHandle;
    uint32_t Rights;
    uint32_t Flags;
    uint32_t Generation;
    uint32_t ObjectType;
    uint32_t State;
} NovaHandleEntryV1;
typedef struct NovaHandleApiV1 {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t CreateEntry;
    uint32_t ResolveEntry;
    uint32_t CloseEntry;
    uint32_t ActiveCountAddress;
    uint32_t TableAddress;
} NovaHandleApiV1;
_Static_assert(sizeof(NovaHandleEntryV1)==24,"NovaHandleEntryV1 ABI size changed");
_Static_assert(sizeof(NovaHandleApiV1)==32,"NovaHandleApiV1 ABI size changed");
#endif
