; Öffentliche Nova Boot ABI für NASM.
; Diese Datei enthält ausschließlich versionierte, von Bootloader und Kernel
; gemeinsam verwendete Konstanten. Private Speicherlayouts gehören nicht hierher.

%ifndef NOVA_BOOT_PROTOCOL_INC
%define NOVA_BOOT_PROTOCOL_INC

NOVA_NKI_VERSION              equ 1
NOVA_NKI_HEADER_SIZE          equ 64
NOVA_BOOT_ARCH_X86_32         equ 1
NOVA_NKI_FLAG_ELF_BUILD_ID    equ 0x00000001
NOVA_NKI_FLAG_NOVA_METADATA   equ 0x00000002
NOVA_LOADER_ABI_VERSION       equ 0x00010000

NOVA_BIB_MAGIC_LOW            equ 0x5048424E ; "NBHP"
NOVA_BIB_MAGIC_HIGH           equ 0x00424942 ; "BIB\0"
NOVA_BIB_VERSION_MAJOR        equ 1
NOVA_BIB_VERSION_MINOR        equ 2
NOVA_BIB_VERSION_PATCH        equ 0
NOVA_BIB_HEADER_SIZE          equ 32
NOVA_X86_BOOT_MAGIC           equ 0x4E4F5641

NOVA_BOOT_PLATFORM_BIOS       equ 1
NOVA_BOOT_FLAG_FRAMEBUFFER    equ 0x00000001

BIB_OFF_MAGIC_LOW             equ 0
BIB_OFF_MAGIC_HIGH            equ 4
BIB_OFF_VERSION_MAJOR         equ 8
BIB_OFF_VERSION_MINOR         equ 10
BIB_OFF_VERSION_PATCH         equ 12
BIB_OFF_HEADER_SIZE           equ 14
BIB_OFF_TOTAL_SIZE            equ 16
BIB_OFF_CHECKSUM              equ 20
BIB_OFF_ARCHITECTURE          equ 24
BIB_OFF_FLAGS                 equ 28

BIB_TLV_HEADER_SIZE           equ 8
BIB_TLV_FLAG_REQUIRED         equ 0x0001

BIB_TLV_FIRMWARE              equ 1
BIB_TLV_CPU                   equ 2
BIB_TLV_MEMORY                equ 3
BIB_TLV_GRAPHICS              equ 4
BIB_TLV_STORAGE               equ 5
BIB_TLV_KERNEL                equ 6
BIB_TLV_SECURITY              equ 7
BIB_TLV_ACPI                  equ 8
BIB_TLV_BOOT_OPTIONS          equ 9
BIB_TLV_MODULES               equ 10
BIB_TLV_TIMING                equ 11
BIB_TLV_ENTROPY               equ 12
BIB_TLV_SYSTEM                equ 13
BIB_TLV_KERNEL_IDENTITY       equ 14

BIB_FIRMWARE_SIZE             equ 16
BIB_CPU_SIZE                  equ 32
BIB_MEMORY_SIZE               equ 16
BIB_GRAPHICS_SIZE             equ 32
BIB_STORAGE_SIZE              equ 24
BIB_KERNEL_SIZE               equ 32
BIB_SECURITY_SIZE             equ 16
BIB_ACPI_SIZE                 equ 16
BIB_BOOT_OPTIONS_SIZE         equ 16
BIB_MODULES_SIZE              equ 16
BIB_TIMING_SIZE               equ 24
BIB_ENTROPY_SIZE              equ 32
BIB_SYSTEM_SIZE               equ 16
BIB_KERNEL_IDENTITY_SIZE      equ 32

NOVA_ENTROPY_SOURCE_NONE      equ 0
NOVA_ENTROPY_SOURCE_RDTSC     equ 1
NOVA_ENTROPY_QUALITY_WEAK     equ 1

NOVA_KERNEL_FORMAT_NATIVE     equ 1
NOVA_KERNEL_FORMAT_ELF32      equ 2
NOVA_KERNEL_FORMAT_ELF64      equ 3

ELF_MAGIC                     equ 0x464C457F
ELFCLASS32                    equ 1
ELFCLASS64                    equ 2
ELFDATA2LSB                   equ 1
ELF_ET_EXEC                   equ 2
ELF_EM_386                    equ 3
ELF_EM_X86_64                 equ 62
ELF_PT_LOAD                   equ 1
ELF_PT_DYNAMIC                equ 2
ELF_PT_INTERP                 equ 3
ELF_PT_NOTE                   equ 4
ELF_PF_EXECUTE                equ 1
ELF_PF_WRITE                  equ 2
ELF_NT_GNU_BUILD_ID           equ 3
ELF_NT_NOVA_REQUIREMENTS      equ 0x4E4F5601
ELF32_HEADER_SIZE             equ 52
ELF32_PROGRAM_HEADER_SIZE     equ 32

ELF32_OFF_ENTRY               equ 24
ELF32_OFF_PHOFF               equ 28
ELF32_OFF_EHSIZE              equ 40
ELF32_OFF_PHENTSIZE           equ 42
ELF32_OFF_PHNUM               equ 44

ELF32_PH_OFF_TYPE             equ 0
ELF32_PH_OFF_OFFSET           equ 4
ELF32_PH_OFF_VADDR            equ 8
ELF32_PH_OFF_PADDR            equ 12
ELF32_PH_OFF_FILESZ           equ 16
ELF32_PH_OFF_MEMSZ            equ 20
ELF32_PH_OFF_FLAGS            equ 24
ELF32_PH_OFF_ALIGN            equ 28

ELF64_HEADER_SIZE             equ 64
ELF64_PROGRAM_HEADER_SIZE     equ 56
ELF64_OFF_ENTRY               equ 24
ELF64_OFF_PHOFF               equ 32
ELF64_OFF_EHSIZE              equ 52
ELF64_OFF_PHENTSIZE           equ 54
ELF64_OFF_PHNUM               equ 56
ELF64_PH_OFF_TYPE             equ 0
ELF64_PH_OFF_FLAGS            equ 4
ELF64_PH_OFF_OFFSET           equ 8
ELF64_PH_OFF_VADDR            equ 16
ELF64_PH_OFF_PADDR            equ 24
ELF64_PH_OFF_FILESZ           equ 32
ELF64_PH_OFF_MEMSZ            equ 40
ELF64_PH_OFF_ALIGN            equ 48

%endif
