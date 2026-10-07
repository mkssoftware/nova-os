#ifndef NOVA_SYSCALL_H
#define NOVA_SYSCALL_H

#include <stddef.h>
#include <stdint.h>

#define NOVA_SYSCALL_ABI_MAJOR 1u
#define NOVA_SYSCALL_ABI_MINOR 0u
#define NOVA_SHARED_SERVICE_PAGE_ADDRESS 0x00403000u
#define NOVA_SHARED_SERVICE_PAGE_SIGNATURE 0x5353564Eu /* NVSS */

#define NOVA_SYSCALL_FEATURE_INT80          0x00000001u
#define NOVA_SYSCALL_FEATURE_COPY_IO        0x00000002u
#define NOVA_SYSCALL_FEATURE_PREEMPT        0x00000004u
#define NOVA_SYSCALL_FEATURE_DISPLAY        0x00000008u
#define NOVA_SYSCALL_FEATURE_DISCOVERY      0x00000010u
#define NOVA_SYSCALL_FEATURE_TYPED_IPC      0x00000020u
#define NOVA_SYSCALL_FEATURE_STATE_VERSION  0x00000040u
#define NOVA_SYSCALL_FEATURE_TRANSACTIONS   0x00000080u
#define NOVA_SYSCALL_FEATURE_FILESYSTEM     0x00000100u
#define NOVA_SYSCALL_FEATURE_FILESYSTEM_WRITABLE 0x00000200u

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
    NOVA_CORE_OPERATION_CLOSE_HANDLE = 3,
    NOVA_CORE_OPERATION_QUERY_ABI = 4,
    NOVA_CORE_OPERATION_DISCOVER_API = 5
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
    NOVA_VFS_OPERATION_LOOKUP = 2,
    NOVA_VFS_OPERATION_READ = 3,
    NOVA_VFS_OPERATION_WRITE = 4,
    NOVA_VFS_OPERATION_CREATE = 5,
    NOVA_VFS_OPERATION_READ_DIRECTORY = 6,
    NOVA_VFS_OPERATION_QUERY = 7,
    NOVA_VFS_OPERATION_DELETE = 8,
    NOVA_VFS_OPERATION_RENAME = 9
};

#define NOVA_VFS_LOOKUP_FLAG_WRITE 0x00000001u
#define NOVA_VFS_IO_MAX_BYTES 4096u
#define NOVA_VFS_OBJECT_FILE 1u
#define NOVA_VFS_OBJECT_DIRECTORY 2u

enum NovaDisplayOperationId {
    NOVA_DISPLAY_OPERATION_QUERY_PRIMARY = 1,
    NOVA_DISPLAY_OPERATION_SUBMIT_SYSTEM_SCENE = 2,
    NOVA_DISPLAY_OPERATION_POLL_INPUT = 3,
    NOVA_DISPLAY_OPERATION_SUBMIT_EXPLORER_VIEW = 4
};

enum NovaSystemInputAction {
    NOVA_SYSTEM_INPUT_TOGGLE_START = 1,
    NOVA_SYSTEM_INPUT_CLOSE_START = 2,
    NOVA_SYSTEM_INPUT_FOCUS_NEXT = 3,
    NOVA_SYSTEM_INPUT_ACTIVATE = 4,
    NOVA_SYSTEM_INPUT_NAVIGATE_UP = 5,
    NOVA_SYSTEM_INPUT_NAVIGATE_DOWN = 6,
    NOVA_SYSTEM_INPUT_NAVIGATE_LEFT = 7,
    NOVA_SYSTEM_INPUT_NAVIGATE_RIGHT = 8,
    NOVA_SYSTEM_INPUT_POINTER_ACTIVATE = 9,
    NOVA_SYSTEM_INPUT_NAVIGATE_BACK = 10
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
    NOVA_STATUS_NOT_FOUND = -2,
    NOVA_STATUS_ABI_STRUCTURE_SIZE = -4,
    NOVA_STATUS_ABI_RESERVED_FIELD = -5,
    NOVA_STATUS_ALREADY_EXISTS = -6,
    NOVA_STATUS_IO_ERROR = -7,
    NOVA_STATUS_SERVICE_UNKNOWN = -8,
    NOVA_STATUS_OPERATION_UNKNOWN = -9,
    NOVA_STATUS_NO_SPACE = -11,
    NOVA_STATUS_READ_ONLY = -12,
    NOVA_STATUS_ACCESS_DENIED = -13,
    NOVA_STATUS_INVALID_USER_POINTER = -15,
    NOVA_STATUS_WOULD_BLOCK = -19,
    NOVA_STATUS_NOT_DIRECTORY = -20,
    NOVA_STATUS_NOT_FILE = -21,
    NOVA_STATUS_VALIDATION_FAILED = -23,
    NOVA_STATUS_RESOURCE_LIMIT = -24,
    NOVA_STATUS_DIRECTORY_NOT_EMPTY = -25
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

typedef struct NovaSyscallFeatureDescriptorV1 {
    uint32_t StructSize;
    NovaAbiVersion Version;
    uint32_t ArchitectureProfile;
    uint32_t FeatureFlags;
    uint32_t ServiceBitmap;
    uint32_t DeprecatedServiceBitmap;
    uint32_t MaxInlineArgumentSize;
    uint32_t MaxCopySize;
    uint64_t StableSyscallMask;
    uint64_t OptionalSyscallMask;
    uint32_t Reserved[4];
} NovaSyscallFeatureDescriptorV1;

typedef struct NovaApiDiscoveryRequestV1 {
    uint32_t StructSize;
    NovaAbiVersion Version;
    uint32_t ApiId;
    uint32_t MinVersion;
    uint32_t MaxVersion;
    uint32_t RequiredFeatures;
    uint32_t SemanticType;
    uint32_t RequiredCapabilities;
    uint32_t HardConstraints;
    uint32_t ProviderHint;
    uint64_t ExecutionContractId;
    uint32_t Reserved[4];
} NovaApiDiscoveryRequestV1;

typedef struct NovaApiDiscoveryResultV1 {
    uint32_t StructSize;
    NovaAbiVersion Version;
    uint32_t ApiId;
    uint32_t SelectedVersion;
    uint32_t ProviderId;
    uint32_t ContractId;
    uint32_t FeatureFlags;
    uint32_t AvailabilityState;
    uint32_t CompatibilityState;
    uint32_t TrustState;
    uint64_t ValidUntilGeneration;
    uint32_t Reserved[4];
} NovaApiDiscoveryResultV1;

typedef struct NovaSemanticApiDescriptorV1 {
    uint32_t StructSize;
    NovaAbiVersion Version;
    uint32_t SemanticApiId;
    uint32_t OperationId;
    uint32_t InputSemanticType;
    uint32_t OutputSemanticType;
    uint32_t RequiredCapabilities;
    uint32_t ContractId;
    uint32_t TransactionFlags;
    uint32_t SideEffectFlags;
    uint64_t ExecutionContractId;
    uint32_t Reserved[4];
} NovaSemanticApiDescriptorV1;

typedef struct NovaOperationResultV1 {
    uint32_t StructSize;
    NovaAbiVersion Version;
    int32_t Status;
    uint32_t Flags;
    uint64_t OperationId;
    uint64_t CompletedBytes;
    uint64_t RequestedBytes;
} NovaOperationResultV1;

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

/* VFS.Read / VFS.Write: höchstens NOVA_VFS_IO_MAX_BYTES je Aufruf. */
typedef struct NovaVfsIoArgumentsV1 {
    uint32_t StructSize;
    NovaAbiVersion Version;
    uint32_t Handle;
    uint32_t BufferAddress;
    uint32_t Offset;
    uint32_t Length;
    uint32_t Transferred;
    uint32_t Reserved;
} NovaVfsIoArgumentsV1;

/* VFS.Create: das Verzeichnis-Handle benötigt das Recht WRITE. */
typedef struct NovaVfsCreateArgumentsV1 {
    uint32_t StructSize;
    NovaAbiVersion Version;
    uint32_t DirectoryHandle;
    uint32_t NameAddress;
    uint32_t NameLength;
    uint32_t ObjectType;
    uint32_t ResultHandle;
    uint32_t Reserved;
} NovaVfsCreateArgumentsV1;

typedef struct NovaVfsDirectoryArgumentsV1 {
    uint32_t StructSize;
    NovaAbiVersion Version;
    uint32_t DirectoryHandle;
    uint32_t Index;
    uint32_t EntryAddress;
    uint32_t EntrySize;
    uint32_t Reserved[2];
} NovaVfsDirectoryArgumentsV1;

typedef struct NovaVfsDirectoryEntryV1 {
    uint32_t StructSize;
    NovaAbiVersion Version;
    uint64_t ObjectId;
    uint32_t ObjectType;
    uint32_t NameLength;
    uint64_t Size;
    uint8_t Name[256];
} NovaVfsDirectoryEntryV1;

typedef struct NovaVfsObjectInfoV1 {
    uint32_t StructSize;
    NovaAbiVersion Version;
    uint32_t Handle;
    uint32_t Rights;
    uint64_t ObjectId;
    uint32_t ObjectType;
    uint32_t Flags;
    uint64_t Size;
    uint64_t Generation;
} NovaVfsObjectInfoV1;

/* VFS.Delete: Datei oder leeres Verzeichnis im Verzeichnis-Handle (WRITE). */
typedef struct NovaVfsDeleteArgumentsV1 {
    uint32_t StructSize;        /* 32 */
    NovaAbiVersion Version;
    uint32_t DirectoryHandle;
    uint32_t NameAddress;
    uint32_t NameLength;        /* 1..255 */
    uint32_t Flags;             /* 0 */
    uint32_t Reserved[2];
} NovaVfsDeleteArgumentsV1;

/* VFS.Rename: Umbenennen/Verschieben; beide Handles mit WRITE. */
typedef struct NovaVfsRenameArgumentsV1 {
    uint32_t StructSize;        /* 40 */
    NovaAbiVersion Version;
    uint32_t SourceDirectoryHandle;
    uint32_t SourceNameAddress;
    uint32_t SourceNameLength;  /* 1..255 */
    uint32_t TargetDirectoryHandle;
    uint32_t TargetNameAddress;
    uint32_t TargetNameLength;  /* 1..255 */
    uint32_t Flags;             /* 0 */
    uint32_t Reserved;
} NovaVfsRenameArgumentsV1;

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

#define NOVA_EXPLORER_VIEW_MAX_ENTRIES 8u
#define NOVA_EXPLORER_VIEW_PATH_MAX    56u
#define NOVA_EXPLORER_NAME_MAX         32u
#define NOVA_EXPLORER_FLAG_HIGHLIGHT_MASK 0x7u /* 1..7 = Schnellzugriff-Zeile */

typedef struct NovaExplorerEntryV1 {
    uint32_t Type;          /* NOVA_VFS_OBJECT_FILE / _DIRECTORY */
    uint32_t Size;          /* Bytes (Dateien), sonst 0 */
    uint32_t NameLength;    /* 1..32 */
    uint32_t Reserved;
    char Name[NOVA_EXPLORER_NAME_MAX];
} NovaExplorerEntryV1;

typedef struct NovaExplorerViewV1 {
    uint32_t Size;              /* 480 */
    uint32_t Version;           /* 1 */
    uint64_t Generation;        /* > 0 */
    uint32_t EntryCount;        /* <= 8 */
    uint32_t TotalEntries;      /* == TotalDirectories + TotalFiles */
    uint32_t Flags;
    uint32_t PathLength;        /* <= 56 */
    uint32_t TotalDirectories;
    uint32_t TotalFiles;
    char Path[NOVA_EXPLORER_VIEW_PATH_MAX];
    NovaExplorerEntryV1 Entries[NOVA_EXPLORER_VIEW_MAX_ENTRIES];
} NovaExplorerViewV1;

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
_Static_assert(sizeof(NovaSyscallFeatureDescriptorV1) == 64,
               "NovaSyscallFeatureDescriptorV1 ABI size changed");
_Static_assert(sizeof(NovaApiDiscoveryRequestV1) == 64,
               "NovaApiDiscoveryRequestV1 ABI size changed");
_Static_assert(sizeof(NovaApiDiscoveryResultV1) == 64,
               "NovaApiDiscoveryResultV1 ABI size changed");
_Static_assert(sizeof(NovaSemanticApiDescriptorV1) == 64,
               "NovaSemanticApiDescriptorV1 ABI size changed");
_Static_assert(sizeof(NovaOperationResultV1) == 40,
               "NovaOperationResultV1 ABI size changed");
_Static_assert(sizeof(NovaIpcPacketV1) == 48,
               "NovaIpcPacketV1 ABI size changed");
_Static_assert(sizeof(NovaVfsLookupArgumentsV1) == 32,
               "NovaVfsLookupArgumentsV1 ABI size changed");
_Static_assert(sizeof(NovaVfsIoArgumentsV1) == 32,
               "NovaVfsIoArgumentsV1 ABI size changed");
_Static_assert(sizeof(NovaVfsCreateArgumentsV1) == 32,
               "NovaVfsCreateArgumentsV1 ABI size changed");
_Static_assert(sizeof(NovaVfsDirectoryArgumentsV1) == 32,
               "NovaVfsDirectoryArgumentsV1 ABI size changed");
_Static_assert(sizeof(NovaVfsDirectoryEntryV1) == 288,
               "NovaVfsDirectoryEntryV1 ABI size changed");
_Static_assert(offsetof(NovaVfsDirectoryEntryV1, Name) == 32,
               "NovaVfsDirectoryEntryV1 name offset changed");
_Static_assert(sizeof(NovaVfsObjectInfoV1) == 48,
               "NovaVfsObjectInfoV1 ABI size changed");
_Static_assert(offsetof(NovaVfsObjectInfoV1, Size) == 32,
               "NovaVfsObjectInfoV1 size offset changed");
_Static_assert(sizeof(NovaVfsDeleteArgumentsV1) == 32,
               "NovaVfsDeleteArgumentsV1 ABI size changed");
_Static_assert(sizeof(NovaVfsRenameArgumentsV1) == 40,
               "NovaVfsRenameArgumentsV1 ABI size changed");
_Static_assert(offsetof(NovaVfsRenameArgumentsV1, TargetDirectoryHandle) == 20,
               "NovaVfsRenameArgumentsV1 target offset changed");
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
_Static_assert(sizeof(NovaExplorerEntryV1) == 48,
               "NovaExplorerEntryV1 ABI size changed");
_Static_assert(offsetof(NovaExplorerViewV1, Path) == 40,
               "NovaExplorerViewV1 path offset changed");
_Static_assert(offsetof(NovaExplorerViewV1, Entries) == 96,
               "NovaExplorerViewV1 header size changed");
_Static_assert(sizeof(NovaExplorerViewV1) == 480,
               "NovaExplorerViewV1 ABI size changed");

/* x86-32: EAX=Service, EBX=Operation, ECX=Major|Minor<<16,
 * EDX=Argumentzeiger, ESI=Argumentgröße, EAX=Status. */
#endif
