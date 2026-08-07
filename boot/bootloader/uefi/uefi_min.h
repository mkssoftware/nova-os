#ifndef NOVA_UEFI_MIN_H
#define NOVA_UEFI_MIN_H

#include <stdint.h>

#define EFIAPI __attribute__((ms_abi))
#define EFI_SUCCESS 0
#define EFI_ERROR(Status) (((uint64_t)(Status) >> 63) != 0)

typedef void VOID;
typedef uint8_t BOOLEAN;
typedef uint16_t CHAR16;
typedef uint64_t UINTN;
typedef uint64_t EFI_STATUS;
typedef uint64_t EFI_PHYSICAL_ADDRESS;
typedef VOID *EFI_HANDLE;
typedef VOID *EFI_EVENT;

typedef struct {
    uint64_t Signature;
    uint32_t Revision;
    uint32_t HeaderSize;
    uint32_t CRC32;
    uint32_t Reserved;
} EFI_TABLE_HEADER;

typedef struct {
    uint16_t ScanCode;
    CHAR16 UnicodeChar;
} EFI_INPUT_KEY;

typedef struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
    EFI_STATUS (EFIAPI *Reset)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *, BOOLEAN);
    EFI_STATUS (EFIAPI *ReadKeyStroke)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *, EFI_INPUT_KEY *);
    EFI_EVENT WaitForKey;
};

typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    EFI_STATUS (EFIAPI *Reset)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *, BOOLEAN);
    EFI_STATUS (EFIAPI *OutputString)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *, const CHAR16 *);
    VOID *TestString;
    VOID *QueryMode;
    VOID *SetMode;
    VOID *SetAttribute;
    VOID *ClearScreen;
    VOID *SetCursorPosition;
    VOID *EnableCursor;
    VOID *Mode;
};

typedef struct {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t Data4[8];
} EFI_GUID;

typedef struct EFI_BOOT_SERVICES EFI_BOOT_SERVICES;
typedef struct EFI_RUNTIME_SERVICES EFI_RUNTIME_SERVICES;
typedef EFI_STATUS (EFIAPI *EFI_GET_VARIABLE)(CHAR16 *,EFI_GUID *,uint32_t *,UINTN *,VOID *);
typedef EFI_STATUS (EFIAPI *EFI_SET_VARIABLE)(CHAR16 *,EFI_GUID *,uint32_t,UINTN,VOID *);
typedef enum {
    EfiResetCold, EfiResetWarm, EfiResetShutdown, EfiResetPlatformSpecific
} EFI_RESET_TYPE;
struct EFI_RUNTIME_SERVICES {
    EFI_TABLE_HEADER Hdr;
    VOID *GetTime, *SetTime, *GetWakeupTime, *SetWakeupTime;
    VOID *SetVirtualAddressMap, *ConvertPointer;
    EFI_GET_VARIABLE GetVariable;
    VOID *GetNextVariableName;
    EFI_SET_VARIABLE SetVariable;
    VOID *GetNextHighMonotonicCount;
    VOID (EFIAPI *ResetSystem)(EFI_RESET_TYPE, EFI_STATUS, UINTN, VOID *);
};
struct EFI_BOOT_SERVICES {
    EFI_TABLE_HEADER Hdr;
    VOID *RaiseTPL, *RestoreTPL;
    VOID *AllocatePages, *FreePages, *GetMemoryMap, *AllocatePool;
    EFI_STATUS (EFIAPI *FreePool)(VOID *);
    VOID *CreateEvent, *SetTimer, *WaitForEvent, *SignalEvent, *CloseEvent, *CheckEvent;
    VOID *InstallProtocolInterface, *ReinstallProtocolInterface, *UninstallProtocolInterface;
    VOID *HandleProtocol, *Reserved, *RegisterProtocolNotify, *LocateHandle, *LocateDevicePath;
    VOID *InstallConfigurationTable;
    VOID *LoadImage, *StartImage, *Exit, *UnloadImage, *ExitBootServices;
    VOID *GetNextMonotonicCount;
    EFI_STATUS (EFIAPI *Stall)(UINTN);
    VOID *SetWatchdogTimer, *ConnectController, *DisconnectController;
    VOID *OpenProtocol, *CloseProtocol, *OpenProtocolInformation;
    VOID *ProtocolsPerHandle, *LocateHandleBuffer;
    EFI_STATUS (EFIAPI *LocateProtocol)(EFI_GUID *, VOID *, VOID **);
};

typedef struct {
    EFI_TABLE_HEADER Hdr;
    CHAR16 *FirmwareVendor;
    uint32_t FirmwareRevision;
    EFI_HANDLE ConsoleInHandle;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;
    EFI_HANDLE ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
    EFI_HANDLE StandardErrorHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *StdErr;
    EFI_RUNTIME_SERVICES *RuntimeServices;
    EFI_BOOT_SERVICES *BootServices;
    UINTN NumberOfTableEntries;
    VOID *ConfigurationTable;
} EFI_SYSTEM_TABLE;

typedef struct {
    uint32_t Version;
    uint32_t HorizontalResolution;
    uint32_t VerticalResolution;
    uint32_t PixelFormat;
    uint32_t PixelInformation[4];
    uint32_t PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
    uint32_t MaxMode;
    uint32_t Mode;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
    UINTN SizeOfInfo;
    EFI_PHYSICAL_ADDRESS FrameBufferBase;
    UINTN FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef struct EFI_GRAPHICS_OUTPUT_PROTOCOL EFI_GRAPHICS_OUTPUT_PROTOCOL;
struct EFI_GRAPHICS_OUTPUT_PROTOCOL {
    EFI_STATUS (EFIAPI *QueryMode)(EFI_GRAPHICS_OUTPUT_PROTOCOL *, uint32_t,
                                   UINTN *, EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **);
    EFI_STATUS (EFIAPI *SetMode)(EFI_GRAPHICS_OUTPUT_PROTOCOL *, uint32_t);
    VOID *Blt;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode;
};

typedef struct {
    int32_t RelativeMovementX;
    int32_t RelativeMovementY;
    int32_t RelativeMovementZ;
    BOOLEAN LeftButton;
    BOOLEAN RightButton;
} EFI_SIMPLE_POINTER_STATE;

typedef struct {
    uint64_t ResolutionX;
    uint64_t ResolutionY;
    uint64_t ResolutionZ;
    BOOLEAN LeftButton;
    BOOLEAN RightButton;
} EFI_SIMPLE_POINTER_MODE;

typedef struct EFI_SIMPLE_POINTER_PROTOCOL EFI_SIMPLE_POINTER_PROTOCOL;
struct EFI_SIMPLE_POINTER_PROTOCOL {
    EFI_STATUS (EFIAPI *Reset)(EFI_SIMPLE_POINTER_PROTOCOL *, BOOLEAN);
    EFI_STATUS (EFIAPI *GetState)(EFI_SIMPLE_POINTER_PROTOCOL *, EFI_SIMPLE_POINTER_STATE *);
    EFI_EVENT WaitForInput;
    EFI_SIMPLE_POINTER_MODE *Mode;
};

static inline void nova_debug_byte(uint8_t value) {
    __asm__ volatile ("outb %0, $0xe9" : : "a"(value));
}

static inline void nova_debug_string(const char *text) {
    while (*text) nova_debug_byte((uint8_t)*text++);
}

#endif
