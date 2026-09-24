#ifndef NOVA_SYSCALL_H
#define NOVA_SYSCALL_H

#include <stddef.h>
#include <stdint.h>

#define NOVA_SYSCALL_ABI_MAJOR 1u
#define NOVA_SYSCALL_ABI_MINOR 0u
#define NOVA_SHARED_SERVICE_PAGE_ADDRESS 0x00403000u
#define NOVA_SHARED_SERVICE_PAGE_SIGNATURE 0x5353564Eu /* NVSS */

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
    NOVA_SERVICE_POWER = 11,
    NOVA_SERVICE_DISPLAY = 12
};

enum NovaCoreOperationId {
    NOVA_CORE_OPERATION_EXIT = 1,
    NOVA_CORE_OPERATION_READY = 2,
    NOVA_CORE_OPERATION_CLOSE_HANDLE = 3
};

enum NovaProcessOperationId {
    NOVA_PROCESS_OPERATION_QUERY_SELF = 1,
    NOVA_PROCESS_OPERATION_OPEN_SELF = 2
};

enum NovaThreadOperationId {
    NOVA_THREAD_OPERATION_QUERY_SELF = 1,
    NOVA_THREAD_OPERATION_OPEN_SELF = 2
};

enum NovaIpcOperationId {
    NOVA_IPC_OPERATION_SEND = 1,
    NOVA_IPC_OPERATION_RECEIVE = 2
};

enum NovaVfsOperationId {
    NOVA_VFS_OPERATION_OPEN_ROOT = 1,
    NOVA_VFS_OPERATION_LOOKUP = 2
};

enum NovaDisplayOperationId {
    NOVA_DISPLAY_OPERATION_QUERY_PRIMARY = 1,
    NOVA_DISPLAY_OPERATION_SUBMIT_SYSTEM_SCENE = 2,
    NOVA_DISPLAY_OPERATION_POLL_INPUT = 3
};

enum NovaSystemInputAction {
    NOVA_SYSTEM_INPUT_TOGGLE_START = 1,
    NOVA_SYSTEM_INPUT_CLOSE_START = 2,
    NOVA_SYSTEM_INPUT_FOCUS_NEXT = 3,
    NOVA_SYSTEM_INPUT_ACTIVATE = 4,
    NOVA_SYSTEM_INPUT_NAVIGATE_UP = 5,
    NOVA_SYSTEM_INPUT_NAVIGATE_DOWN = 6,
    NOVA_SYSTEM_INPUT_NAVIGATE_LEFT = 7,
    NOVA_SYSTEM_INPUT_NAVIGATE_RIGHT = 8
};

enum NovaDisplaySceneFlags {
    NOVA_DISPLAY_SCENE_DESKTOP = 1u << 0,
    NOVA_DISPLAY_SCENE_START_MENU = 1u << 1,
    NOVA_DISPLAY_SCENE_RIBBON = 1u << 2,
    NOVA_DISPLAY_SCENE_TASKBAR = 1u << 3
};

enum NovaStatus {
    NOVA_STATUS_OK = 0,
    NOVA_STATUS_ABI_INCOMPATIBLE = -1,
    NOVA_STATUS_ABI_STRUCTURE_SIZE = -4,
    NOVA_STATUS_ABI_RESERVED_FIELD = -5,
    NOVA_STATUS_SERVICE_UNKNOWN = -8,
    NOVA_STATUS_OPERATION_UNKNOWN = -9,
    NOVA_STATUS_ACCESS_DENIED = -13,
    NOVA_STATUS_INVALID_USER_POINTER = -15,
    NOVA_STATUS_WOULD_BLOCK = -19,
    NOVA_STATUS_VALIDATION_FAILED = -23
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

typedef struct NovaIdentityResultV1 {
    uint32_t StructSize;
    NovaAbiVersion Version;
    uint32_t Identifier;
    uint32_t Reserved;
} NovaIdentityResultV1;

typedef struct NovaHandleCloseArgumentsV1 {
    uint32_t StructSize;
    NovaAbiVersion Version;
    uint32_t Handle;
    uint32_t Reserved;
} NovaHandleCloseArgumentsV1;

typedef struct NovaSharedServicePageV1 {
    uint32_t Signature;
    uint32_t StructSize;
    NovaAbiVersion KernelAbi;
    uint32_t FeatureFlags;
    uint32_t ServiceBitmap;
    uint32_t PageSize;
    uint32_t TimerFrequencyHz;
    uint32_t BootPhase;
    uint64_t BootTickSnapshot;
    uint32_t Reserved[6];
} NovaSharedServicePageV1;

typedef struct NovaIpcPacketV1 {
    uint32_t StructSize;
    NovaAbiVersion Version;
    uint32_t EndpointHandle;
    uint32_t Flags;
    uint64_t MessageId;
    uint64_t CorrelationId;
    uint32_t PayloadSize;
    uint32_t Reserved;
    uint8_t Payload[8];
} NovaIpcPacketV1;

typedef struct NovaVfsLookupArgumentsV1 {
    uint32_t StructSize;
    NovaAbiVersion Version;
    uint32_t DirectoryHandle;
    uint32_t PathAddress;
    uint32_t PathLength;
    uint32_t Flags;
    uint32_t ResultHandle;
    uint32_t Reserved;
} NovaVfsLookupArgumentsV1;

/* Die physische Framebufferadresse bleibt ausschließlich im Kernel. */
typedef struct NovaDisplayInfoV1 {
    uint32_t StructSize;
    NovaAbiVersion Version;
    uint32_t DisplayId;
    uint32_t WidthPixels;
    uint32_t HeightPixels;
    uint32_t PitchBytes;
    uint32_t BitsPerPixel;
    uint32_t ScaleMilli;
    uint64_t Generation;
} NovaDisplayInfoV1;

/* Deklarative Bootstrap-Szene ohne Pointer oder ausführbaren Inhalt. */
typedef struct NovaSystemSceneV1 {
    uint32_t StructSize;
    NovaAbiVersion Version;
    uint64_t Generation;
    uint32_t Flags;
    uint32_t Theme;
    uint32_t Workspace;
    uint32_t FocusedElement;
    uint32_t BackgroundToken;
    uint32_t AccentToken;
    uint32_t Reserved[5];
} NovaSystemSceneV1;

typedef struct NovaSystemInputEventV1 {
    uint32_t StructSize;
    NovaAbiVersion Version;
    uint32_t Action;
    uint32_t ScanCode;
    uint64_t MonotonicTick;
    uint32_t TargetElement;
    uint32_t Reserved;
} NovaSystemInputEventV1;

_Static_assert(sizeof(NovaAbiVersion) == 4,
               "NovaAbiVersion ABI size changed");
_Static_assert(sizeof(NovaSyscallRequestV1) == 32,
               "NovaSyscallRequestV1 ABI size changed");
_Static_assert(offsetof(NovaSyscallRequestV1, Arguments) == 16,
               "NovaSyscallRequestV1 alignment changed");
_Static_assert(sizeof(NovaCoreExitArgumentsV1) == 16,
               "NovaCoreExitArgumentsV1 ABI size changed");
_Static_assert(sizeof(NovaIdentityResultV1) == 16,
               "NovaIdentityResultV1 ABI size changed");
_Static_assert(sizeof(NovaHandleCloseArgumentsV1) == 16,
               "NovaHandleCloseArgumentsV1 ABI size changed");
_Static_assert(sizeof(NovaSharedServicePageV1) == 64,
               "NovaSharedServicePageV1 ABI size changed");
_Static_assert(sizeof(NovaIpcPacketV1) == 48,
               "NovaIpcPacketV1 ABI size changed");
_Static_assert(sizeof(NovaVfsLookupArgumentsV1) == 32,
               "NovaVfsLookupArgumentsV1 ABI size changed");
_Static_assert(sizeof(NovaDisplayInfoV1) == 40,
               "NovaDisplayInfoV1 ABI size changed");
_Static_assert(offsetof(NovaDisplayInfoV1, Generation) == 32,
               "NovaDisplayInfoV1 alignment changed");
_Static_assert(sizeof(NovaSystemSceneV1) == 64,
               "NovaSystemSceneV1 ABI size changed");
_Static_assert(offsetof(NovaSystemSceneV1, Flags) == 16,
               "NovaSystemSceneV1 alignment changed");
_Static_assert(sizeof(NovaSystemInputEventV1) == 32,
               "NovaSystemInputEventV1 ABI size changed");
_Static_assert(offsetof(NovaSystemInputEventV1, MonotonicTick) == 16,
               "NovaSystemInputEventV1 alignment changed");

/* x86-32: EAX=Service, EBX=Operation, ECX=Major|Minor<<16,
 * EDX=Argumentzeiger, ESI=Argumentgröße, EAX=Status. */
#endif
