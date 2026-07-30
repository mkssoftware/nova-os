#ifndef NOVA_PROCESS_H
#define NOVA_PROCESS_H
#include <stdint.h>
#include <nova/object.h>
#define NOVA_PROCESS_ABI_MAJOR 1u
#define NOVA_PROCESS_ABI_MINOR 0u
#define NOVA_PROCESS_CAPACITY 4u
typedef enum nova_process_state {
    NOVA_PROCESS_EMPTY = 0,
    NOVA_PROCESS_RUNNING = 1
} nova_process_state_t;
typedef struct nova_process_record {
    uint32_t ProcessId;
    uint32_t State;
    uint32_t PageDirectory;
    nova_handle_t ObjectHandle;
    uint32_t ParentProcessId;
    uint32_t Flags;
    uint32_t Reserved[2];
} nova_process_record_t;
typedef struct nova_process_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t CreateEntry;
    uint32_t LookupEntry;
    uint32_t CountAddress;
    uint32_t TableAddress;
    uint32_t NextProcessIdAddress;
} nova_process_api_t;
_Static_assert(sizeof(nova_process_record_t) == 32,
               "nova_process_record_t ABI size changed");
_Static_assert(sizeof(nova_process_api_t) == 32,
               "nova_process_api_t ABI size changed");
#endif
