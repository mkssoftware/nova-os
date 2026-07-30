#ifndef NOVA_THREAD_H
#define NOVA_THREAD_H
#include <stdint.h>
#include <nova/object.h>
#define NOVA_THREAD_ABI_MAJOR 1u
#define NOVA_THREAD_ABI_MINOR 0u
#define NOVA_THREAD_CAPACITY 3u
typedef enum nova_thread_state {
    NOVA_THREAD_READY = 1
} nova_thread_state_t;
typedef struct nova_thread_record {
    uint32_t ThreadId;
    uint32_t ProcessId;
    uint32_t State;
    uint32_t SchedulerSlot;
    nova_handle_t ObjectHandle;
    uint32_t EntryAddress;
    uint32_t ContextAddress;
    uint32_t Reserved;
} nova_thread_record_t;
typedef struct nova_thread_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capacity;
    uint32_t RegisterEntry;
    uint32_t LookupEntry;
    uint32_t CountAddress;
    uint32_t TableAddress;
    uint32_t NextThreadIdAddress;
} nova_thread_api_t;
_Static_assert(sizeof(nova_thread_record_t) == 32,
               "nova_thread_record_t ABI size changed");
_Static_assert(sizeof(nova_thread_api_t) == 32,
               "nova_thread_api_t ABI size changed");
#endif
