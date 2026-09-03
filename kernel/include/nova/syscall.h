#ifndef NOVA_SYSCALL_H
#define NOVA_SYSCALL_H

#include <stddef.h>
#include <stdint.h>

#define NOVA_SYSCALL_ABI_MAJOR 1u
#define NOVA_SYSCALL_ABI_MINOR 0u

enum NovaServiceId {
    NOVA_SERVICE_CORE = 1,
    NOVA_SERVICE_PROCESS = 2,
    NOVA_SERVICE_THREAD = 3,
    NOVA_SERVICE_MEMORY = 4,
    NOVA_SERVICE_IPC = 5,
    NOVA_SERVICE_VFS = 6,
    NOVA_SERVICE_DEVICE = 7,
    NOVA_SERVICE_NETWORK = 8,
    NOVA_SERVICE_SECURITY = 9,
    NOVA_SERVICE_DIAGNOSTIC = 10,
    NOVA_SERVICE_POWER = 11
};

enum NovaCoreOperationId {
    NOVA_CORE_OPERATION_EXIT = 1
};

enum NovaStatus {
    NOVA_STATUS_OK = 0,
    NOVA_STATUS_ABI_INCOMPATIBLE = -1,
    NOVA_STATUS_ABI_STRUCTURE_SIZE = -4,
    NOVA_STATUS_ABI_RESERVED_FIELD = -5,
    NOVA_STATUS_SERVICE_UNKNOWN = -8,
    NOVA_STATUS_OPERATION_UNKNOWN = -9,
    NOVA_STATUS_ACCESS_DENIED = -13,
    NOVA_STATUS_INVALID_USER_POINTER = -15
};

typedef struct NovaAbiVersion {
    uint16_t Major;
    uint16_t Minor;
} NovaAbiVersion;

/* Logische, architekturunabhängige Beschreibung gemäß NPSPEC-KERNEL-0030. */
typedef struct NovaSyscallRequestV1 {
    uint32_t ServiceId;
    uint32_t OperationId;
    NovaAbiVersion Version;
    uint32_t Reserved;
    uint64_t Arguments;
    uint64_t ArgumentSize;
} NovaSyscallRequestV1;

typedef struct NovaCoreExitArgumentsV1 {
    uint32_t StructSize;
    NovaAbiVersion Version;
    int32_t ExitCode;
    uint32_t Reserved;
} NovaCoreExitArgumentsV1;

_Static_assert(sizeof(NovaAbiVersion) == 4,
               "NovaAbiVersion ABI size changed");
_Static_assert(sizeof(NovaSyscallRequestV1) == 32,
               "NovaSyscallRequestV1 ABI size changed");
_Static_assert(offsetof(NovaSyscallRequestV1, Arguments) == 16,
               "NovaSyscallRequestV1 alignment changed");
_Static_assert(sizeof(NovaCoreExitArgumentsV1) == 16,
               "NovaCoreExitArgumentsV1 ABI size changed");

/* x86-32: EAX=Service, EBX=Operation, ECX=Major|Minor<<16,
 * EDX=Argumentzeiger, ESI=Argumentgröße, EAX=Status. */
#endif
