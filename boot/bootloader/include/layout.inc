; Gemeinsames physisches Speicher- und Datenträgerlayout des BIOS-Bootpfads.
; Änderungen an diesen Konstanten müssen zusammen mit Image Builder,
; Kernel-Linkerskript und BootInfo-ABI erfolgen.

%ifndef NOVA_BOOT_LAYOUT_INC
%define NOVA_BOOT_LAYOUT_INC

%include "nova_boot_protocol.inc"

; Datenträgerlayout
STAGE2_FIRST_SECTOR        equ 2
STAGE2_SECTOR_COUNT        equ 64
STAGE2_BYTE_COUNT          equ STAGE2_SECTOR_COUNT * 512
KERNEL_FIRST_LBA           equ 65
KERNEL_PAYLOAD_MAX_SIZE    equ 40960
NKI_HEADER_SIZE            equ NOVA_NKI_HEADER_SIZE
KERNEL_IMAGE_SECTOR_COUNT  equ 81
KERNEL_IMAGE_BYTE_COUNT    equ KERNEL_IMAGE_SECTOR_COUNT * 512

DISK_RETRY_COUNT           equ 3

; Physisches Speicherlayout unterhalb von 1 MiB
BOOT_INFO_ADDRESS          equ 0x00005000
BOOT_INFO_CAPACITY         equ 0x00000800
MEMORY_MAP_ADDRESS         equ 0x00005800
MEMORY_MAP_ENTRY_SIZE      equ 24
MEMORY_MAP_MAX_ENTRIES     equ 64
VBE_MODE_INFO_ADDRESS      equ 0x00006000
VBE_CONTROLLER_INFO_ADDRESS equ 0x00006100
STAGE2_STACK_TOP           equ 0x00007800
STAGE1_STACK_TOP           equ 0x00007C00
STAGE2_LOAD_ADDRESS        equ 0x00007E00

KERNEL_TEMP_SEGMENT        equ 0x1000
KERNEL_TEMP_OFFSET         equ 0x0000
KERNEL_TEMP_ADDRESS        equ 0x00010000

; 32-Bit-Kernel
KERNEL_ENTRY_ADDRESS       equ 0x00100000
KERNEL_STACK_TOP           equ 0x00090000

; Protected-Mode-Segmente
CODE_SEGMENT               equ 0x08
DATA_SEGMENT               equ 0x10
LONG_CODE_SEGMENT          equ 0x18
LONG_MODE_PML4             equ 0x00070000
LONG_MODE_PDPT             equ 0x00071000
LONG_MODE_PD               equ 0x00072000

; Grafik
; Der Modus wird aus der VBE-Modusliste nach der in BOOTRENDER-0007
; definierten Auflösungspräferenz ausgewählt.
NOVA_PIXEL_FORMAT_BGRX8888 equ 1

; Nova Kernel Image v1 (NKI)
NKI_MAGIC_LOW              equ 0x41564F4E ; "NOVA"
NKI_MAGIC_HIGH             equ 0x00494B4E ; "NKI\0"
NKI_VERSION                equ 1
NKI_ARCH_X86_32            equ 1
NKI_COMPRESSION_NONE       equ 0

NKI_OFF_MAGIC_LOW          equ 0
NKI_OFF_MAGIC_HIGH         equ 4
NKI_OFF_VERSION            equ 8
NKI_OFF_HEADER_SIZE        equ 12
NKI_OFF_ARCHITECTURE       equ 16
NKI_OFF_FLAGS              equ 20
NKI_OFF_ENTRY_POINT        equ 24
NKI_OFF_LOAD_ADDRESS       equ 28
NKI_OFF_IMAGE_SIZE         equ 32
NKI_OFF_COMPRESSION        equ 36
NKI_OFF_CRC32              equ 40
NKI_OFF_BUILD_ID           equ 44

%endif
