; Nova Boot Manager - BIOS Stage 2
; Referenzimplementierung für NKI v1 und NBHP/BIB v1.

%include "layout.inc"

[org STAGE2_LOAD_ADDRESS]
[bits 16]

BIB_FIRMWARE_OFFSET     equ NOVA_BIB_HEADER_SIZE
BIB_MEMORY_OFFSET       equ BIB_FIRMWARE_OFFSET + BIB_TLV_HEADER_SIZE + BIB_FIRMWARE_SIZE
BIB_GRAPHICS_OFFSET     equ BIB_MEMORY_OFFSET + BIB_TLV_HEADER_SIZE + BIB_MEMORY_SIZE
BIB_KERNEL_OFFSET       equ BIB_GRAPHICS_OFFSET + BIB_TLV_HEADER_SIZE + BIB_GRAPHICS_SIZE
BIB_SECURITY_OFFSET     equ BIB_KERNEL_OFFSET + BIB_TLV_HEADER_SIZE + BIB_KERNEL_SIZE
BIB_OPTIONS_OFFSET      equ BIB_SECURITY_OFFSET + BIB_TLV_HEADER_SIZE + BIB_SECURITY_SIZE
BIB_CPU_OFFSET          equ BIB_OPTIONS_OFFSET + BIB_TLV_HEADER_SIZE + BIB_BOOT_OPTIONS_SIZE
BIB_STORAGE_OFFSET      equ BIB_CPU_OFFSET + BIB_TLV_HEADER_SIZE + BIB_CPU_SIZE
BIB_ACPI_OFFSET         equ BIB_STORAGE_OFFSET + BIB_TLV_HEADER_SIZE + BIB_STORAGE_SIZE
BIB_MODULES_OFFSET      equ BIB_ACPI_OFFSET + BIB_TLV_HEADER_SIZE + BIB_ACPI_SIZE
BIB_TIMING_OFFSET       equ BIB_MODULES_OFFSET + BIB_TLV_HEADER_SIZE + BIB_MODULES_SIZE
BIB_ENTROPY_OFFSET      equ BIB_TIMING_OFFSET + BIB_TLV_HEADER_SIZE + BIB_TIMING_SIZE
BIB_SYSTEM_OFFSET       equ BIB_ENTROPY_OFFSET + BIB_TLV_HEADER_SIZE + BIB_ENTROPY_SIZE
BIB_KERNEL_ID_OFFSET    equ BIB_SYSTEM_OFFSET + BIB_TLV_HEADER_SIZE + BIB_SYSTEM_SIZE
BIB_TOTAL_SIZE          equ BIB_KERNEL_ID_OFFSET + BIB_TLV_HEADER_SIZE + BIB_KERNEL_IDENTITY_SIZE

BM_ENTRY_SIZE           equ 8
BM_ENTRY_LABEL          equ 0
BM_ENTRY_ICON           equ 4
BM_ENTRY_ACTION         equ 5
BM_ACTION_START         equ 0
BM_ACTION_UNAVAILABLE   equ 1
BM_ACTION_DIAGNOSTICS   equ 2
BM_ACTION_RECOVERY      equ 3
BM_ACTION_POWEROFF      equ 4

stage2_start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, STAGE2_STACK_TOP
    cld
    sti

    mov [boot_drive], dl
    mov si, message_start
    call print_string

    call initialize_bib
    call collect_cpu_and_entropy
    call collect_memory_map
    call initialize_vbe
    call load_kernel_image
    call validate_kernel_image
    call finalize_bib
    call validate_bib
    call enable_a20
    call enter_protected_mode
    jmp fatal_halt

; ---------------------------------------------------------------------------
; Boot Information Block v1
; Alle TLVs sind 8-Byte-ausgerichtet und Little-Endian kodiert.
; ---------------------------------------------------------------------------

initialize_bib:
    xor ax, ax
    mov es, ax
    mov di, BOOT_INFO_ADDRESS
    mov cx, BOOT_INFO_CAPACITY / 2
    rep stosw

    mov dword [BOOT_INFO_ADDRESS + BIB_OFF_MAGIC_LOW], NOVA_BIB_MAGIC_LOW
    mov dword [BOOT_INFO_ADDRESS + BIB_OFF_MAGIC_HIGH], NOVA_BIB_MAGIC_HIGH
    mov word [BOOT_INFO_ADDRESS + BIB_OFF_VERSION_MAJOR], NOVA_BIB_VERSION_MAJOR
    mov word [BOOT_INFO_ADDRESS + BIB_OFF_VERSION_MINOR], NOVA_BIB_VERSION_MINOR
    mov word [BOOT_INFO_ADDRESS + BIB_OFF_VERSION_PATCH], NOVA_BIB_VERSION_PATCH
    mov word [BOOT_INFO_ADDRESS + BIB_OFF_HEADER_SIZE], NOVA_BIB_HEADER_SIZE
    mov dword [BOOT_INFO_ADDRESS + BIB_OFF_TOTAL_SIZE], BIB_TOTAL_SIZE
    mov dword [BOOT_INFO_ADDRESS + BIB_OFF_ARCHITECTURE], NKI_ARCH_X86_32

    ; Firmware TLV
    mov word [BOOT_INFO_ADDRESS + BIB_FIRMWARE_OFFSET + 0], BIB_TLV_FIRMWARE
    mov word [BOOT_INFO_ADDRESS + BIB_FIRMWARE_OFFSET + 2], BIB_TLV_FLAG_REQUIRED
    mov dword [BOOT_INFO_ADDRESS + BIB_FIRMWARE_OFFSET + 4], BIB_FIRMWARE_SIZE
    mov dword [BOOT_INFO_ADDRESS + BIB_FIRMWARE_OFFSET + 8], NOVA_BOOT_PLATFORM_BIOS
    xor eax, eax
    mov al, [boot_drive]
    mov dword [BOOT_INFO_ADDRESS + BIB_FIRMWARE_OFFSET + 12], eax

    ; Memory TLV
    mov word [BOOT_INFO_ADDRESS + BIB_MEMORY_OFFSET + 0], BIB_TLV_MEMORY
    mov word [BOOT_INFO_ADDRESS + BIB_MEMORY_OFFSET + 2], BIB_TLV_FLAG_REQUIRED
    mov dword [BOOT_INFO_ADDRESS + BIB_MEMORY_OFFSET + 4], BIB_MEMORY_SIZE
    mov dword [BOOT_INFO_ADDRESS + BIB_MEMORY_OFFSET + 8], MEMORY_MAP_ADDRESS
    mov dword [BOOT_INFO_ADDRESS + BIB_MEMORY_OFFSET + 16], MEMORY_MAP_ENTRY_SIZE

    ; Graphics TLV
    mov word [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 0], BIB_TLV_GRAPHICS
    mov dword [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 4], BIB_GRAPHICS_SIZE

    ; Kernel TLV
    mov word [BOOT_INFO_ADDRESS + BIB_KERNEL_OFFSET + 0], BIB_TLV_KERNEL
    mov word [BOOT_INFO_ADDRESS + BIB_KERNEL_OFFSET + 2], BIB_TLV_FLAG_REQUIRED
    mov dword [BOOT_INFO_ADDRESS + BIB_KERNEL_OFFSET + 4], BIB_KERNEL_SIZE

    ; Security TLV: Status 0 = nicht verifiziert, transparent gemeldet.
    mov word [BOOT_INFO_ADDRESS + BIB_SECURITY_OFFSET + 0], BIB_TLV_SECURITY
    mov word [BOOT_INFO_ADDRESS + BIB_SECURITY_OFFSET + 2], BIB_TLV_FLAG_REQUIRED
    mov dword [BOOT_INFO_ADDRESS + BIB_SECURITY_OFFSET + 4], BIB_SECURITY_SIZE

    ; Boot Options TLV: Standardprofil, kein Debug-/Recovery-Modus.
    mov word [BOOT_INFO_ADDRESS + BIB_OPTIONS_OFFSET + 0], BIB_TLV_BOOT_OPTIONS
    mov word [BOOT_INFO_ADDRESS + BIB_OPTIONS_OFFSET + 2], BIB_TLV_FLAG_REQUIRED
    mov dword [BOOT_INFO_ADDRESS + BIB_OPTIONS_OFFSET + 4], BIB_BOOT_OPTIONS_SIZE

    ; Erweiterte NBHP/BIB-1.1-Informationen. Nicht verfügbare optionale
    ; Zeiger bleiben null und werden dadurch nicht als gültig ausgegeben.
    mov word [BOOT_INFO_ADDRESS + BIB_CPU_OFFSET + 0], BIB_TLV_CPU
    mov word [BOOT_INFO_ADDRESS + BIB_CPU_OFFSET + 2], BIB_TLV_FLAG_REQUIRED
    mov dword [BOOT_INFO_ADDRESS + BIB_CPU_OFFSET + 4], BIB_CPU_SIZE

    mov word [BOOT_INFO_ADDRESS + BIB_STORAGE_OFFSET + 0], BIB_TLV_STORAGE
    mov dword [BOOT_INFO_ADDRESS + BIB_STORAGE_OFFSET + 4], BIB_STORAGE_SIZE
    mov dword [BOOT_INFO_ADDRESS + BIB_STORAGE_OFFSET + 8], NOVA_BOOT_PLATFORM_BIOS
    xor eax, eax
    mov al, [boot_drive]
    mov dword [BOOT_INFO_ADDRESS + BIB_STORAGE_OFFSET + 12], eax

    mov word [BOOT_INFO_ADDRESS + BIB_ACPI_OFFSET + 0], BIB_TLV_ACPI
    mov dword [BOOT_INFO_ADDRESS + BIB_ACPI_OFFSET + 4], BIB_ACPI_SIZE

    mov word [BOOT_INFO_ADDRESS + BIB_MODULES_OFFSET + 0], BIB_TLV_MODULES
    mov dword [BOOT_INFO_ADDRESS + BIB_MODULES_OFFSET + 4], BIB_MODULES_SIZE

    mov word [BOOT_INFO_ADDRESS + BIB_TIMING_OFFSET + 0], BIB_TLV_TIMING
    mov dword [BOOT_INFO_ADDRESS + BIB_TIMING_OFFSET + 4], BIB_TIMING_SIZE

    mov word [BOOT_INFO_ADDRESS + BIB_ENTROPY_OFFSET + 0], BIB_TLV_ENTROPY
    mov word [BOOT_INFO_ADDRESS + BIB_ENTROPY_OFFSET + 2], BIB_TLV_FLAG_REQUIRED
    mov dword [BOOT_INFO_ADDRESS + BIB_ENTROPY_OFFSET + 4], BIB_ENTROPY_SIZE

    mov word [BOOT_INFO_ADDRESS + BIB_SYSTEM_OFFSET + 0], BIB_TLV_SYSTEM
    mov word [BOOT_INFO_ADDRESS + BIB_SYSTEM_OFFSET + 2], BIB_TLV_FLAG_REQUIRED
    mov dword [BOOT_INFO_ADDRESS + BIB_SYSTEM_OFFSET + 4], BIB_SYSTEM_SIZE
    mov dword [BOOT_INFO_ADDRESS + BIB_SYSTEM_OFFSET + 16], 1 ; erster Bootversuch
    mov word [BOOT_INFO_ADDRESS + BIB_KERNEL_ID_OFFSET + 0], BIB_TLV_KERNEL_IDENTITY
    mov dword [BOOT_INFO_ADDRESS + BIB_KERNEL_ID_OFFSET + 4], BIB_KERNEL_IDENTITY_SIZE
    ret

collect_cpu_and_entropy:
    xor eax, eax
    cpuid
    mov dword [BOOT_INFO_ADDRESS + BIB_CPU_OFFSET + 8], ebx
    mov dword [BOOT_INFO_ADDRESS + BIB_CPU_OFFSET + 12], edx
    mov dword [BOOT_INFO_ADDRESS + BIB_CPU_OFFSET + 16], ecx
    mov dword [BOOT_INFO_ADDRESS + BIB_CPU_OFFSET + 20], eax
    mov eax, 1
    cpuid
    mov dword [BOOT_INFO_ADDRESS + BIB_CPU_OFFSET + 24], edx
    mov dword [BOOT_INFO_ADDRESS + BIB_CPU_OFFSET + 28], ecx

    rdtsc
    mov dword [BOOT_INFO_ADDRESS + BIB_TIMING_OFFSET + 8], eax
    mov dword [BOOT_INFO_ADDRESS + BIB_TIMING_OFFSET + 12], edx
    xor eax, dword [0x046C]       ; BIOS-Tickzähler beim Eintritt
    rol edx, 13
    xor edx, eax
    mov dword [BOOT_INFO_ADDRESS + BIB_ENTROPY_OFFSET + 8], eax
    mov dword [BOOT_INFO_ADDRESS + BIB_ENTROPY_OFFSET + 12], edx
    not eax
    xor eax, 0x4E4F5641
    mov dword [BOOT_INFO_ADDRESS + BIB_ENTROPY_OFFSET + 16], eax
    rol edx, 7
    xor edx, 0x424F4F54
    mov dword [BOOT_INFO_ADDRESS + BIB_ENTROPY_OFFSET + 20], edx
    mov dword [BOOT_INFO_ADDRESS + BIB_ENTROPY_OFFSET + 24], NOVA_ENTROPY_SOURCE_RDTSC
    mov dword [BOOT_INFO_ADDRESS + BIB_ENTROPY_OFFSET + 28], NOVA_ENTROPY_QUALITY_WEAK
    mov dword [BOOT_INFO_ADDRESS + BIB_ENTROPY_OFFSET + 32], 16
    mov dword [BOOT_INFO_ADDRESS + BIB_SECURITY_OFFSET + 16], NOVA_ENTROPY_QUALITY_WEAK
    ret

collect_memory_map:
    xor ax, ax
    mov es, ax
    mov di, MEMORY_MAP_ADDRESS
    xor ebx, ebx
    xor bp, bp

.next:
    cmp bp, MEMORY_MAP_MAX_ENTRIES
    jae .done

    mov eax, 0x0000E820
    mov edx, 0x534D4150             ; "SMAP"
    mov ecx, MEMORY_MAP_ENTRY_SIZE
    int 0x15
    jc .done
    cmp eax, 0x534D4150
    jne .done
    cmp ecx, 20
    jb .done

    cmp ecx, MEMORY_MAP_ENTRY_SIZE
    jae .entry_complete
    mov dword [es:di + 20], 1

.entry_complete:
    inc bp
    add di, MEMORY_MAP_ENTRY_SIZE
    test ebx, ebx
    jnz .next

.done:
    xor ax, ax
    mov ds, ax
    mov es, ax
    movzx eax, bp
    mov dword [BOOT_INFO_ADDRESS + BIB_MEMORY_OFFSET + 12], eax
    test bp, bp
    jnz .success
    mov si, message_memory_error
    call print_string
    jmp fatal_halt

.success:
    ret

; ---------------------------------------------------------------------------
; VBE. Ein Fehler ist recoverable und aktiviert den Textmodus-Fallback.
; ---------------------------------------------------------------------------

initialize_vbe:
    mov ax, 0x4F01
    mov cx, VBE_REQUESTED_MODE
    xor bx, bx
    mov es, bx
    mov di, VBE_MODE_INFO_ADDRESS
    int 0x10
    cmp ax, 0x004F
    jne .fallback

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ax, [VBE_MODE_INFO_ADDRESS]
    test ax, 0x0001
    jz .fallback
    test ax, 0x0080
    jz .fallback
    cmp byte [VBE_MODE_INFO_ADDRESS + 25], 32
    jne .fallback
    cmp dword [VBE_MODE_INFO_ADDRESS + 40], 0
    je .fallback

    mov ax, 0x4F02
    mov bx, VBE_REQUESTED_MODE | 0x4000
    int 0x10
    cmp ax, 0x004F
    jne .fallback

    xor ax, ax
    mov ds, ax
    mov es, ax
    or dword [BOOT_INFO_ADDRESS + BIB_OFF_FLAGS], NOVA_BOOT_FLAG_FRAMEBUFFER

    mov eax, [VBE_MODE_INFO_ADDRESS + 40]
    mov dword [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 8], eax
    xor eax, eax
    mov ax, [VBE_MODE_INFO_ADDRESS + 16]
    mov dword [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 12], eax
    xor eax, eax
    mov ax, [VBE_MODE_INFO_ADDRESS + 18]
    mov dword [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 16], eax
    xor eax, eax
    mov ax, [VBE_MODE_INFO_ADDRESS + 20]
    mov dword [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 20], eax
    xor eax, eax
    mov al, [VBE_MODE_INFO_ADDRESS + 25]
    mov dword [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 24], eax
    mov dword [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 28], NOVA_PIXEL_FORMAT_BGRX8888
    ret

.fallback:
    mov ax, 0x0003
    int 0x10
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov si, message_vbe_fallback
    call print_string
    ret

; ---------------------------------------------------------------------------
; Kernel Image Loader
; ---------------------------------------------------------------------------

load_kernel_image:
    call detect_bios_extensions
    jc .chs
    mov byte [retry_count], DISK_RETRY_COUNT

.lba_retry:
    mov si, kernel_dap
    mov ah, 0x42
    mov dl, [boot_drive]
    int 0x13
    jnc .done
    call reset_disk
    dec byte [retry_count]
    jnz .lba_retry

.chs:
    call load_kernel_chs
    jc kernel_load_error

.done:
    xor ax, ax
    mov ds, ax
    mov es, ax
    ret

detect_bios_extensions:
    mov ah, 0x41
    mov bx, 0x55AA
    mov dl, [boot_drive]
    int 0x13
    jc .missing
    cmp bx, 0xAA55
    jne .missing
    test cx, 1
    jz .missing
    clc
    ret
.missing:
    stc
    ret

load_kernel_chs:
    call get_disk_geometry
    mov word [kernel_destination_offset], KERNEL_TEMP_OFFSET

    ; KERNEL_FIRST_LBA in die vom BIOS gemeldete CHS-Geometrie umrechnen.
    mov ax, KERNEL_FIRST_LBA
    xor dx, dx
    xor bx, bx
    mov bl, [sectors_per_track]
    div bx
    inc dl
    mov [kernel_sector], dl
    xor dx, dx
    xor bx, bx
    mov bl, [maximum_head]
    inc bx
    div bx
    mov [kernel_cylinder], al
    mov [kernel_head], dl
    mov byte [kernel_remaining], KERNEL_IMAGE_SECTOR_COUNT

.next_sector:
    cmp byte [kernel_remaining], 0
    je .success
    mov byte [retry_count], DISK_RETRY_COUNT
.retry:
    mov ax, KERNEL_TEMP_SEGMENT
    mov es, ax
    mov bx, [kernel_destination_offset]
    mov ah, 0x02
    mov al, 1
    mov ch, [kernel_cylinder]
    mov cl, [kernel_sector]
    mov dh, [kernel_head]
    mov dl, [boot_drive]
    int 0x13
    jnc .loaded
    call reset_disk
    dec byte [retry_count]
    jnz .retry
    stc
    ret
.loaded:
    add word [kernel_destination_offset], 512
    call advance_chs
    dec byte [kernel_remaining]
    jmp .next_sector
.success:
    clc
    ret

get_disk_geometry:
    mov byte [sectors_per_track], 18
    mov byte [maximum_head], 1
    mov ah, 0x08
    mov dl, [boot_drive]
    int 0x13
    jc .done
    mov al, cl
    and al, 0x3F
    jz .done
    mov [sectors_per_track], al
    mov [maximum_head], dh
.done:
    ret

advance_chs:
    inc byte [kernel_sector]
    mov al, [sectors_per_track]
    cmp byte [kernel_sector], al
    jbe .done
    mov byte [kernel_sector], 1
    mov al, [maximum_head]
    cmp byte [kernel_head], al
    jb .next_head
    mov byte [kernel_head], 0
    inc byte [kernel_cylinder]
    ret
.next_head:
    inc byte [kernel_head]
.done:
    ret

reset_disk:
    push ax
    push dx
    xor ax, ax
    mov dl, [boot_drive]
    int 0x13
    pop dx
    pop ax
    ret

validate_kernel_image:
    push ds
    mov ax, KERNEL_TEMP_SEGMENT
    mov ds, ax

    cmp dword [NKI_OFF_MAGIC_LOW], NKI_MAGIC_LOW
    jne .try_direct_elf
    cmp dword [NKI_OFF_MAGIC_HIGH], NKI_MAGIC_HIGH
    jne .invalid_header
    cmp dword [NKI_OFF_VERSION], NKI_VERSION
    jne .invalid_header
    cmp dword [NKI_OFF_HEADER_SIZE], NKI_HEADER_SIZE
    jne .invalid_header
    cmp dword [NKI_OFF_ARCHITECTURE], NKI_ARCH_X86_32
    jne .invalid_header
    mov eax, [NKI_OFF_FLAGS]
    test eax, ~(NOVA_NKI_FLAG_ELF_BUILD_ID | NOVA_NKI_FLAG_NOVA_METADATA)
    jnz .invalid_header
    cmp dword [NKI_OFF_COMPRESSION], NKI_COMPRESSION_NONE
    jne .invalid_header
    cmp dword [NKI_OFF_ENTRY_POINT], KERNEL_ENTRY_ADDRESS
    jne .invalid_header
    cmp dword [NKI_OFF_LOAD_ADDRESS], KERNEL_ENTRY_ADDRESS
    jne .invalid_header

    mov ecx, [NKI_OFF_IMAGE_SIZE]
    test ecx, ecx
    jz .invalid_header
    cmp ecx, KERNEL_PAYLOAD_MAX_SIZE
    ja .invalid_header

    mov si, NKI_HEADER_SIZE
    call crc32_ds_si
    cmp eax, [NKI_OFF_CRC32]
    jne .invalid_checksum
    mov [cs:kernel_payload_crc], eax
    mov dword [cs:kernel_payload_base], KERNEL_TEMP_ADDRESS + NKI_HEADER_SIZE

    cmp dword [NKI_HEADER_SIZE], ELF_MAGIC
    jne .native_nki
    mov si, NKI_HEADER_SIZE
    mov ecx, [NKI_OFF_IMAGE_SIZE]
    call validate_elf_ds_si
    jc .invalid_header
    test dword [NKI_OFF_FLAGS], NOVA_NKI_FLAG_ELF_BUILD_ID
    jz .invalid_header
    test dword [NKI_OFF_FLAGS], NOVA_NKI_FLAG_NOVA_METADATA
    jz .invalid_header
    cmp byte [cs:elf_nova_metadata_found], 1
    jne .invalid_header
    mov eax, [NKI_OFF_BUILD_ID + 0]
    cmp eax, [cs:elf_build_id + 0]
    jne .invalid_header
    mov eax, [NKI_OFF_BUILD_ID + 4]
    cmp eax, [cs:elf_build_id + 4]
    jne .invalid_header
    mov eax, [NKI_OFF_BUILD_ID + 8]
    cmp eax, [cs:elf_build_id + 8]
    jne .invalid_header
    mov eax, [NKI_OFF_BUILD_ID + 12]
    cmp eax, [cs:elf_build_id + 12]
    jne .invalid_header
    jmp .publish

.native_nki:
    mov byte [cs:kernel_payload_format], NOVA_KERNEL_FORMAT_NATIVE
    mov eax, [NKI_OFF_LOAD_ADDRESS]
    mov [cs:elf_load_address], eax
    mov eax, [NKI_OFF_IMAGE_SIZE]
    mov [cs:elf_memory_size], eax
    mov eax, [NKI_OFF_ENTRY_POINT]
    mov [cs:elf_entry_point], eax
    jmp .publish

.try_direct_elf:
    cmp dword [0], ELF_MAGIC
    jne .invalid_header
    mov si, 0
    mov ecx, KERNEL_IMAGE_BYTE_COUNT
    call validate_elf_ds_si
    jc .invalid_header
    mov dword [cs:kernel_payload_base], KERNEL_TEMP_ADDRESS
    mov dword [cs:kernel_payload_crc], 0

.publish:
    pop ds
    mov eax, [elf_load_address]
    mov edx, [elf_memory_size]
    mov ecx, [elf_entry_point]
    mov ebp, [kernel_payload_crc]
    mov dword [BOOT_INFO_ADDRESS + BIB_KERNEL_OFFSET + 8], eax
    mov dword [BOOT_INFO_ADDRESS + BIB_KERNEL_OFFSET + 12], edx
    mov dword [BOOT_INFO_ADDRESS + BIB_KERNEL_OFFSET + 16], ecx
    movzx eax, byte [kernel_payload_format]
    mov dword [BOOT_INFO_ADDRESS + BIB_KERNEL_OFFSET + 20], eax
    mov dword [BOOT_INFO_ADDRESS + BIB_KERNEL_OFFSET + 24], ebp
    mov eax, [elf_build_id + 0]
    mov [BOOT_INFO_ADDRESS + BIB_KERNEL_ID_OFFSET + 8], eax
    mov eax, [elf_build_id + 4]
    mov [BOOT_INFO_ADDRESS + BIB_KERNEL_ID_OFFSET + 12], eax
    mov eax, [elf_build_id + 8]
    mov [BOOT_INFO_ADDRESS + BIB_KERNEL_ID_OFFSET + 16], eax
    mov eax, [elf_build_id + 12]
    mov [BOOT_INFO_ADDRESS + BIB_KERNEL_ID_OFFSET + 20], eax
    mov eax, [elf_build_id + 16]
    mov [BOOT_INFO_ADDRESS + BIB_KERNEL_ID_OFFSET + 24], eax
    movzx eax, byte [kernel_payload_format]
    mov [BOOT_INFO_ADDRESS + BIB_KERNEL_ID_OFFSET + 28], eax
    ret

.invalid_header:
    pop ds
    mov si, message_kernel_header_error
    call print_string
    mov byte [boot_error_code], 1
    ret
.invalid_checksum:
    pop ds
    mov si, message_kernel_checksum_error
    call print_string
    mov byte [boot_error_code], 2
    ret

; DS:SI zeigt auf ein ELF32-Image, ECX enthält die verfügbare Dateigröße.
; Alle PT_LOAD-Segmente werden vor dem späteren Kopieren vollständig geprüft.
validate_elf_ds_si:
    cmp byte [si + 4], ELFCLASS32
    je .elf32
    cmp byte [si + 4], ELFCLASS64
    je .elf64
    stc
    ret
.elf32:
    call validate_elf32_ds_si
    jc .done
    mov byte [cs:kernel_payload_format], NOVA_KERNEL_FORMAT_ELF32
.done:
    ret
.elf64:
    call validate_elf64_ds_si
    jc .done
    mov byte [cs:kernel_payload_format], NOVA_KERNEL_FORMAT_ELF64
    ret

validate_elf32_ds_si:
    pushad
    movzx esi, si
    mov edi, ecx
    cmp edi, ELF32_HEADER_SIZE
    jb .invalid
    cmp dword [esi], ELF_MAGIC
    jne .invalid
    cmp byte [esi + 4], ELFCLASS32
    jne .invalid
    cmp byte [esi + 5], ELFDATA2LSB
    jne .invalid
    cmp byte [esi + 6], 1
    jne .invalid
    cmp word [esi + 16], ELF_ET_EXEC
    jne .invalid
    cmp word [esi + 18], ELF_EM_386
    jne .invalid
    cmp dword [esi + 20], 1
    jne .invalid
    cmp word [esi + ELF32_OFF_EHSIZE], ELF32_HEADER_SIZE
    jne .invalid
    cmp word [esi + ELF32_OFF_PHENTSIZE], ELF32_PROGRAM_HEADER_SIZE
    jne .invalid
    movzx ecx, word [esi + ELF32_OFF_PHNUM]
    test ecx, ecx
    jz .invalid
    cmp ecx, 16
    ja .invalid
    mov eax, [esi + ELF32_OFF_PHOFF]
    mov edx, ecx
    shl edx, 5
    add edx, eax
    jc .invalid
    cmp edx, edi
    ja .invalid
    add eax, esi
    mov edx, eax
    mov eax, [esi + ELF32_OFF_ENTRY]
    mov [cs:elf_entry_point], eax
    mov dword [cs:elf_min_address], 0xFFFFFFFF
    mov dword [cs:elf_max_address], 0
    mov dword [cs:elf_segment_count], 0
    mov byte [cs:elf_entry_covered], 0
    mov byte [cs:elf_build_id_found], 0
    mov byte [cs:elf_nova_metadata_found], 0
.program_header:
    mov eax, [edx + ELF32_PH_OFF_TYPE]
    cmp eax, ELF_PT_DYNAMIC
    je .invalid
    cmp eax, ELF_PT_INTERP
    je .invalid
    cmp eax, ELF_PT_NOTE
    jne .check_load
    call validate_elf_note_segment
    jc .invalid
    jmp .next
.check_load:
    cmp eax, ELF_PT_LOAD
    jne .next
    mov eax, [edx + ELF32_PH_OFF_FLAGS]
    and eax, ELF_PF_EXECUTE | ELF_PF_WRITE
    cmp eax, ELF_PF_EXECUTE | ELF_PF_WRITE
    je .invalid                    ; W^X bereits im Loader erzwingen
    mov eax, [edx + ELF32_PH_OFF_FILESZ]
    cmp eax, [edx + ELF32_PH_OFF_MEMSZ]
    ja .invalid
    cmp dword [edx + ELF32_PH_OFF_MEMSZ], 0
    je .invalid
    add eax, [edx + ELF32_PH_OFF_OFFSET]
    jc .invalid
    cmp eax, edi
    ja .invalid
    mov eax, [edx + ELF32_PH_OFF_PADDR]
    cmp eax, KERNEL_ENTRY_ADDRESS
    jb .invalid
    mov ebx, eax
    add ebx, [edx + ELF32_PH_OFF_MEMSZ]
    jc .invalid
    cmp ebx, 0x01000000
    ja .invalid

    ; p_vaddr und p_offset müssen modulo p_align übereinstimmen.
    mov ebp, [edx + ELF32_PH_OFF_ALIGN]
    cmp ebp, 1
    jbe .alignment_ok
    mov eax, ebp
    dec eax
    test ebp, eax
    jnz .invalid
    mov ebp, [edx + ELF32_PH_OFF_OFFSET]
    and ebp, eax
    ; Der zweite AND benötigt align-1, nicht align.
    mov eax, [edx + ELF32_PH_OFF_ALIGN]
    dec eax
    and eax, [edx + ELF32_PH_OFF_VADDR]
    cmp ebp, eax
    jne .invalid
.alignment_ok:
    mov eax, [edx + ELF32_PH_OFF_PADDR]
    mov ebx, eax
    add ebx, [edx + ELF32_PH_OFF_MEMSZ]
    mov [cs:elf_current_start], eax
    mov [cs:elf_current_end], ebx

    ; Kein geladenes Speicherintervall darf ein früheres PT_LOAD überlappen.
    xor ebp, ebp
.overlap_loop:
    cmp ebp, [cs:elf_segment_count]
    jae .store_interval
    cmp eax, [cs:elf_segment_ends + ebp * 4]
    jae .overlap_next
    mov ebx, [cs:elf_segment_starts + ebp * 4]
    cmp ebx, [cs:elf_current_end]
    jb .invalid
.overlap_next:
    inc ebp
    jmp .overlap_loop
.store_interval:
    mov ebp, [cs:elf_segment_count]
    mov eax, [cs:elf_current_start]
    mov [cs:elf_segment_starts + ebp * 4], eax
    mov ebx, [cs:elf_current_end]
    mov [cs:elf_segment_ends + ebp * 4], ebx
    inc dword [cs:elf_segment_count]
    cmp eax, [cs:elf_min_address]
    jae .minimum_ok
    mov [cs:elf_min_address], eax
.minimum_ok:
    cmp ebx, [cs:elf_max_address]
    jbe .check_entry
    mov [cs:elf_max_address], ebx
.check_entry:
    test dword [edx + ELF32_PH_OFF_FLAGS], ELF_PF_EXECUTE
    jz .next
    mov ebp, [cs:elf_entry_point]
    cmp ebp, [cs:elf_current_start]
    jb .next
    cmp ebp, [cs:elf_current_end]
    jae .next
    mov byte [cs:elf_entry_covered], 1
.next:
    add edx, ELF32_PROGRAM_HEADER_SIZE
    dec ecx
    jnz .program_header
    cmp dword [cs:elf_segment_count], 0
    je .invalid
    cmp byte [cs:elf_entry_covered], 1
    jne .invalid
    mov eax, [cs:elf_min_address]
    mov [cs:elf_load_address], eax
    mov ebx, [cs:elf_max_address]
    sub ebx, eax
    mov [cs:elf_memory_size], ebx
    popad
    clc
    ret
.invalid:
    popad
    stc
    ret

validate_elf64_ds_si:
    pushad
    movzx esi, si
    mov edi, ecx
    cmp edi, ELF64_HEADER_SIZE
    jb .invalid
    cmp dword [esi], ELF_MAGIC
    jne .invalid
    cmp byte [esi + 4], ELFCLASS64
    jne .invalid
    cmp byte [esi + 5], ELFDATA2LSB
    jne .invalid
    cmp word [esi + 16], ELF_ET_EXEC
    jne .invalid
    cmp word [esi + 18], ELF_EM_X86_64
    jne .invalid
    cmp dword [esi + 20], 1
    jne .invalid
    cmp word [esi + ELF64_OFF_EHSIZE], ELF64_HEADER_SIZE
    jne .invalid
    cmp word [esi + ELF64_OFF_PHENTSIZE], ELF64_PROGRAM_HEADER_SIZE
    jne .invalid
    cmp dword [esi + ELF64_OFF_ENTRY + 4], 0
    jne .invalid
    cmp dword [esi + ELF64_OFF_PHOFF + 4], 0
    jne .invalid
    movzx ecx, word [esi + ELF64_OFF_PHNUM]
    test ecx, ecx
    jz .invalid
    cmp ecx, 16
    ja .invalid
    mov eax, [esi + ELF64_OFF_PHOFF]
    imul edx, ecx, ELF64_PROGRAM_HEADER_SIZE
    add edx, eax
    jc .invalid
    cmp edx, edi
    ja .invalid
    add eax, esi
    mov edx, eax
    mov eax, [esi + ELF64_OFF_ENTRY]
    mov [cs:elf_entry_point], eax
    mov dword [cs:elf_min_address], 0xFFFFFFFF
    mov dword [cs:elf_max_address], 0
    mov dword [cs:elf_segment_count], 0
    mov byte [cs:elf_entry_covered], 0
.ph:
    cmp dword [edx + ELF64_PH_OFF_TYPE], ELF_PT_DYNAMIC
    je .invalid
    cmp dword [edx + ELF64_PH_OFF_TYPE], ELF_PT_INTERP
    je .invalid
    cmp dword [edx + ELF64_PH_OFF_TYPE], ELF_PT_LOAD
    jne .next
    mov eax, [edx + ELF64_PH_OFF_FLAGS]
    and eax, ELF_PF_EXECUTE | ELF_PF_WRITE
    cmp eax, ELF_PF_EXECUTE | ELF_PF_WRITE
    je .invalid
    cmp dword [edx + ELF64_PH_OFF_OFFSET + 4], 0
    jne .invalid
    cmp dword [edx + ELF64_PH_OFF_PADDR + 4], 0
    jne .invalid
    cmp dword [edx + ELF64_PH_OFF_FILESZ + 4], 0
    jne .invalid
    cmp dword [edx + ELF64_PH_OFF_MEMSZ + 4], 0
    jne .invalid
    cmp dword [edx + ELF64_PH_OFF_ALIGN + 4], 0
    jne .invalid
    mov eax, [edx + ELF64_PH_OFF_FILESZ]
    cmp eax, [edx + ELF64_PH_OFF_MEMSZ]
    ja .invalid
    add eax, [edx + ELF64_PH_OFF_OFFSET]
    jc .invalid
    cmp eax, edi
    ja .invalid
    mov eax, [edx + ELF64_PH_OFF_PADDR]
    cmp eax, KERNEL_ENTRY_ADDRESS
    jb .invalid
    mov ebx, eax
    add ebx, [edx + ELF64_PH_OFF_MEMSZ]
    jc .invalid
    cmp ebx, 0x01000000
    ja .invalid
    mov [cs:elf_current_start], eax
    mov [cs:elf_current_end], ebx
    mov ebp, [edx + ELF64_PH_OFF_ALIGN]
    cmp ebp, 1
    jbe .alignment_ok
    mov eax, ebp
    dec eax
    test ebp, eax
    jnz .invalid
    mov ebp, [edx + ELF64_PH_OFF_OFFSET]
    and ebp, eax
    mov eax, [edx + ELF64_PH_OFF_VADDR]
    and eax, [edx + ELF64_PH_OFF_ALIGN]
    ; align-1 erneut bilden, damit virtuelle Adresse und Offset kongruent sind.
    mov eax, [edx + ELF64_PH_OFF_ALIGN]
    dec eax
    and eax, [edx + ELF64_PH_OFF_VADDR]
    cmp ebp, eax
    jne .invalid
.alignment_ok:
    xor ebp, ebp
.overlap:
    cmp ebp, [cs:elf_segment_count]
    jae .store
    mov eax, [cs:elf_current_start]
    cmp eax, [cs:elf_segment_ends + ebp * 4]
    jae .overlap_next
    mov eax, [cs:elf_segment_starts + ebp * 4]
    cmp eax, [cs:elf_current_end]
    jb .invalid
.overlap_next:
    inc ebp
    jmp .overlap
.store:
    mov ebp, [cs:elf_segment_count]
    mov eax, [cs:elf_current_start]
    mov [cs:elf_segment_starts + ebp * 4], eax
    mov ebx, [cs:elf_current_end]
    mov [cs:elf_segment_ends + ebp * 4], ebx
    cmp eax, [cs:elf_min_address]
    jae .min_ok
    mov [cs:elf_min_address], eax
.min_ok:
    cmp ebx, [cs:elf_max_address]
    jbe .entry
    mov [cs:elf_max_address], ebx
.entry:
    inc dword [cs:elf_segment_count]
    test dword [edx + ELF64_PH_OFF_FLAGS], ELF_PF_EXECUTE
    jz .next
    mov ebp, [cs:elf_entry_point]
    cmp ebp, eax
    jb .next
    cmp ebp, ebx
    jae .next
    mov byte [cs:elf_entry_covered], 1
.next:
    add edx, ELF64_PROGRAM_HEADER_SIZE
    dec ecx
    jnz .ph
    cmp dword [cs:elf_segment_count], 0
    je .invalid
    cmp byte [cs:elf_entry_covered], 1
    jne .invalid
    mov eax, [cs:elf_min_address]
    mov [cs:elf_load_address], eax
    mov ebx, [cs:elf_max_address]
    sub ebx, eax
    mov [cs:elf_memory_size], ebx
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .invalid
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .invalid
    popad
    clc
    ret
.invalid:
    popad
    stc
    ret

; Validiert alle ELF32-Notes eines PT_NOTE-Segments. Unbekannte Notes bleiben optional.
validate_elf_note_segment:
    pushad
    mov eax, [edx + ELF32_PH_OFF_OFFSET]
    mov ebx, [edx + ELF32_PH_OFF_FILESZ]
    add ebx, eax
    jc .invalid
    cmp ebx, edi
    ja .invalid
    add eax, esi
    add ebx, esi
    jc .invalid
    mov ebp, eax
.note:
    cmp ebp, ebx
    je .valid
    ja .invalid
    mov eax, ebx
    sub eax, ebp
    cmp eax, 12
    jb .invalid
    mov eax, [ds:ebp]
    mov ecx, [ds:ebp + 4]
    mov [cs:elf_note_name_size], eax
    mov [cs:elf_note_desc_size], ecx
    add eax, 3
    jc .invalid
    and eax, 0xFFFFFFFC
    add ecx, 3
    jc .invalid
    and ecx, 0xFFFFFFFC
    add eax, ecx
    jc .invalid
    add eax, 12
    jc .invalid
    mov ecx, ebp
    add ecx, eax
    jc .invalid
    cmp ecx, ebx
    ja .invalid
    mov [cs:elf_note_next], ecx
    cmp dword [ds:ebp + 8], ELF_NT_GNU_BUILD_ID
    jne .check_nova
    cmp dword [cs:elf_note_name_size], 4
    jne .advance
    cmp dword [cs:elf_note_desc_size], 20
    jne .advance
    cmp dword [ds:ebp + 12], 0x00554E47 ; "GNU\0"
    jne .advance
    mov byte [cs:elf_build_id_found], 1
    mov eax, [ds:ebp + 16]
    mov [cs:elf_build_id + 0], eax
    mov eax, [ds:ebp + 20]
    mov [cs:elf_build_id + 4], eax
    mov eax, [ds:ebp + 24]
    mov [cs:elf_build_id + 8], eax
    mov eax, [ds:ebp + 28]
    mov [cs:elf_build_id + 12], eax
    mov eax, [ds:ebp + 32]
    mov [cs:elf_build_id + 16], eax
    jmp .advance
.check_nova:
    cmp dword [ds:ebp + 8], ELF_NT_NOVA_REQUIREMENTS
    jne .advance
    cmp dword [cs:elf_note_name_size], 5
    jne .invalid
    cmp dword [cs:elf_note_desc_size], 16
    jne .invalid
    cmp dword [ds:ebp + 12], 0x41564F4E ; "NOVA"
    jne .invalid
    cmp byte [ds:ebp + 16], 0
    jne .invalid
    cmp dword [ds:ebp + 20], 1
    jne .invalid
    cmp dword [ds:ebp + 24], NOVA_LOADER_ABI_VERSION
    ja .invalid
    mov eax, [ds:ebp + 28]
    mov edx, [cs:BOOT_INFO_ADDRESS + BIB_CPU_OFFSET + 24]
    not edx
    test eax, edx
    jnz .invalid
    mov eax, [ds:ebp + 32]
    mov edx, [cs:BOOT_INFO_ADDRESS + BIB_CPU_OFFSET + 28]
    not edx
    test eax, edx
    jnz .invalid
    mov byte [cs:elf_nova_metadata_found], 1
.advance:
    mov ebp, [cs:elf_note_next]
    jmp .note
.valid:
    popad
    clc
    ret
.invalid:
    popad
    stc
    ret

; CRC32/ISO-HDLC über DS:SI, Länge ECX. Ergebnis EAX.
crc32_ds_si:
    mov eax, 0xFFFFFFFF
.byte:
    test ecx, ecx
    jz .finish
    xor al, [si]
    inc si
    push cx
    mov cx, 8
.bit:
    shr eax, 1
    jnc .next_bit
    xor eax, 0xEDB88320
.next_bit:
    loop .bit
    pop cx
    dec ecx
    jmp .byte
.finish:
    not eax
    ret

finalize_bib:
    mov dword [BOOT_INFO_ADDRESS + BIB_OFF_CHECKSUM], 0
    xor ax, ax
    mov ds, ax
    mov si, BOOT_INFO_ADDRESS
    mov ecx, BIB_TOTAL_SIZE
    call crc32_ds_si
    mov dword [BOOT_INFO_ADDRESS + BIB_OFF_CHECKSUM], eax
    ret

; Stage 2 übergibt ausschließlich ein strukturell konsistentes NBHP/BIB.
; Unbekannte optionale TLVs dürfte der Kernel überspringen; die von Stage 2
; erzeugten Pflicht-TLVs müssen hier jedoch vollständig und geordnet sein.
validate_bib:
    cmp dword [BOOT_INFO_ADDRESS + BIB_OFF_MAGIC_LOW], NOVA_BIB_MAGIC_LOW
    jne .invalid
    cmp dword [BOOT_INFO_ADDRESS + BIB_OFF_MAGIC_HIGH], NOVA_BIB_MAGIC_HIGH
    jne .invalid
    cmp word [BOOT_INFO_ADDRESS + BIB_OFF_HEADER_SIZE], NOVA_BIB_HEADER_SIZE
    jne .invalid
    cmp dword [BOOT_INFO_ADDRESS + BIB_OFF_TOTAL_SIZE], BIB_TOTAL_SIZE
    jne .invalid
    cmp word [BOOT_INFO_ADDRESS + BIB_FIRMWARE_OFFSET], BIB_TLV_FIRMWARE
    jne .invalid
    cmp word [BOOT_INFO_ADDRESS + BIB_MEMORY_OFFSET], BIB_TLV_MEMORY
    jne .invalid
    cmp word [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET], BIB_TLV_GRAPHICS
    jne .invalid
    cmp word [BOOT_INFO_ADDRESS + BIB_KERNEL_OFFSET], BIB_TLV_KERNEL
    jne .invalid
    cmp word [BOOT_INFO_ADDRESS + BIB_SECURITY_OFFSET], BIB_TLV_SECURITY
    jne .invalid
    cmp word [BOOT_INFO_ADDRESS + BIB_OPTIONS_OFFSET], BIB_TLV_BOOT_OPTIONS
    jne .invalid
    cmp word [BOOT_INFO_ADDRESS + BIB_CPU_OFFSET], BIB_TLV_CPU
    jne .invalid
    cmp word [BOOT_INFO_ADDRESS + BIB_ENTROPY_OFFSET], BIB_TLV_ENTROPY
    jne .invalid
    cmp word [BOOT_INFO_ADDRESS + BIB_SYSTEM_OFFSET], BIB_TLV_SYSTEM
    jne .invalid
    cmp word [BOOT_INFO_ADDRESS + BIB_KERNEL_ID_OFFSET], BIB_TLV_KERNEL_IDENTITY
    jne .invalid
    ret
.invalid:
    mov si, message_bib_error
    call print_string
    jmp fatal_halt

; ---------------------------------------------------------------------------
; CPU-Handoff
; ---------------------------------------------------------------------------

enable_a20:
    mov ax, 0x2401
    int 0x15
    in al, 0x92
    test al, 0x02
    jnz .done
    or al, 0x02
    and al, 0xFE
    out 0x92, al
.done:
    ret

enter_protected_mode:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE_SEGMENT:protected_mode_start

[bits 32]
protected_mode_start:
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, KERNEL_STACK_TOP
    cld

    cmp byte [boot_error_code], 0
    je .kernel_valid
    call boot_error_screen
.error_halt:
    cli
    hlt
    jmp .error_halt

.kernel_valid:
    call boot_manager_run
    cmp byte [kernel_payload_format], NOVA_KERNEL_FORMAT_ELF32
    je .load_elf32
    cmp byte [kernel_payload_format], NOVA_KERNEL_FORMAT_ELF64
    je .load_elf64

    mov esi, [kernel_payload_base]
    mov edi, [elf_load_address]
    mov ecx, [elf_memory_size]
    push ecx
    shr ecx, 2
    rep movsd
    pop ecx
    and ecx, 3
    rep movsb
    jmp .handoff

.load_elf32:
    mov ebp, [kernel_payload_base]
    movzx eax, word [ebp + ELF32_OFF_PHNUM]
    mov [elf_ph_remaining], eax
    mov esi, [ebp + ELF32_OFF_PHOFF]
    add esi, ebp
    mov [elf_ph_pointer], esi
.next_segment:
    cmp dword [elf_ph_remaining], 0
    je .handoff
    mov esi, [elf_ph_pointer]
    cmp dword [esi + ELF32_PH_OFF_TYPE], ELF_PT_LOAD
    jne .advance_segment
    mov ebx, [esi + ELF32_PH_OFF_FILESZ]
    mov edi, [esi + ELF32_PH_OFF_PADDR]
    mov eax, [esi + ELF32_PH_OFF_OFFSET]
    add eax, ebp
    mov esi, eax
    mov ecx, ebx
    push ecx
    shr ecx, 2
    rep movsd
    pop ecx
    and ecx, 3
    rep movsb
    mov esi, [elf_ph_pointer]
    mov ecx, [esi + ELF32_PH_OFF_MEMSZ]
    sub ecx, ebx
    xor eax, eax
    rep stosb
.advance_segment:
    add dword [elf_ph_pointer], ELF32_PROGRAM_HEADER_SIZE
    dec dword [elf_ph_remaining]
    jmp .next_segment

.load_elf64:
    mov ebp, [kernel_payload_base]
    movzx eax, word [ebp + ELF64_OFF_PHNUM]
    mov [elf_ph_remaining], eax
    mov esi, [ebp + ELF64_OFF_PHOFF]
    add esi, ebp
    mov [elf_ph_pointer], esi
.next_segment64:
    cmp dword [elf_ph_remaining], 0
    je .enter_long_mode
    mov esi, [elf_ph_pointer]
    cmp dword [esi + ELF64_PH_OFF_TYPE], ELF_PT_LOAD
    jne .advance_segment64
    mov ebx, [esi + ELF64_PH_OFF_FILESZ]
    mov edi, [esi + ELF64_PH_OFF_PADDR]
    mov eax, [esi + ELF64_PH_OFF_OFFSET]
    add eax, ebp
    mov esi, eax
    mov ecx, ebx
    push ecx
    shr ecx, 2
    rep movsd
    pop ecx
    and ecx, 3
    rep movsb
    mov esi, [elf_ph_pointer]
    mov ecx, [esi + ELF64_PH_OFF_MEMSZ]
    sub ecx, ebx
    xor eax, eax
    rep stosb
.advance_segment64:
    add dword [elf_ph_pointer], ELF64_PROGRAM_HEADER_SIZE
    dec dword [elf_ph_remaining]
    jmp .next_segment64

.enter_long_mode:
    mov edi, LONG_MODE_PML4
    xor eax, eax
    mov ecx, (3 * 4096) / 4
    rep stosd
    mov dword [LONG_MODE_PML4], LONG_MODE_PDPT | 3
    mov dword [LONG_MODE_PDPT], LONG_MODE_PD | 3
    mov edi, LONG_MODE_PD
    mov eax, 0x00000083
    mov ecx, 512
.map_2m:
    mov [edi], eax
    mov dword [edi + 4], 0
    add eax, 0x00200000
    add edi, 8
    loop .map_2m
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    mov eax, LONG_MODE_PML4
    mov cr3, eax
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    jmp LONG_CODE_SEGMENT:long_mode_start

.handoff:
    mov eax, NOVA_X86_BOOT_MAGIC
    mov ebx, BOOT_INFO_ADDRESS
    mov ecx, [elf_entry_point]
    jmp ecx

[bits 64]
long_mode_start:
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov rsp, KERNEL_STACK_TOP
    mov eax, NOVA_X86_BOOT_MAGIC
    mov ebx, BOOT_INFO_ADDRESS
    mov ecx, [abs elf_entry_point]
    jmp rcx

[bits 32]

; ---------------------------------------------------------------------------
; Grafischer Fehlerzustand. Technische Details bleiben am Debug-Port erhalten,
; auf dem Bildschirm steht eine verständliche und handlungsorientierte Meldung.
; ---------------------------------------------------------------------------

boot_error_screen:
    test dword [BOOT_INFO_ADDRESS + BIB_OFF_FLAGS], NOVA_BOOT_FLAG_FRAMEBUFFER
    jz .done

    mov eax, BM_COLOR_BACKGROUND
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 16]
    mov esi, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 20]
    call bm_fill_rectangle

    mov eax, BM_COLOR_ERROR
    mov ebx, 102
    xor ecx, ecx
    mov edx, 628
    mov esi, 13
    call bm_fill_rounded_rectangle

    mov dword [bm_logo_x], 45
    mov dword [bm_logo_y], 34
    mov dword [bm_logo_color], BM_COLOR_ERROR_TEXT
    mov byte [bm_logo_compact], 0
    mov byte [bm_logo_mirror], 0
    call bm_draw_nova_logo
    mov dword [bm_error_mark_x], 650
    mov dword [bm_error_mark_y], 84
    mov dword [bm_error_mark_color], BM_COLOR_ERROR_TEXT
    call bm_draw_error_mark

    mov esi, bm_error_brand
    mov ebx, 336
    mov ecx, 70
    mov edx, BM_COLOR_BLUE
    mov ebp, 3
    call bm_draw_text

    mov esi, bm_error_title
    mov ebx, 338
    mov ecx, 134
    mov edx, BM_COLOR_ERROR_TEXT
    mov ebp, 2
    call bm_draw_text

    mov esi, bm_error_explanation_1
    mov ebx, 27
    mov ecx, 245
    mov edx, BM_COLOR_ERROR_TEXT
    mov ebp, 1
    call bm_draw_text
    mov esi, bm_error_explanation_2
    mov ebx, 27
    mov ecx, 273
    mov edx, BM_COLOR_ERROR_TEXT
    mov ebp, 1
    call bm_draw_text

    ; Dunkle Empfehlungskarte mit rotem Rahmen
    mov eax, BM_COLOR_ERROR
    mov ebx, 149
    mov ecx, 350
    mov edx, 512
    mov esi, 120
    call bm_fill_rounded_rectangle
    mov eax, BM_COLOR_PANEL
    mov ebx, 150
    mov ecx, 351
    mov edx, 510
    mov esi, 118
    call bm_fill_rounded_rectangle

    mov esi, bm_error_action_1
    mov ebx, 186
    mov ecx, 371
    mov edx, BM_COLOR_ERROR_TEXT
    mov ebp, 1
    call bm_draw_text
    mov esi, bm_error_action_2
    mov ebx, 381
    mov ecx, 399
    mov edx, BM_COLOR_ERROR_TEXT
    mov ebp, 1
    call bm_draw_text
    mov esi, bm_error_action_3
    mov ebx, 187
    mov ecx, 427
    mov edx, BM_COLOR_ERROR_TEXT
    mov ebp, 1
    call bm_draw_text

    cmp byte [boot_error_code], 2
    jne .header_code
    mov esi, bm_error_checksum_code
    jmp .draw_code
.header_code:
    mov esi, bm_error_header_code
.draw_code:
    mov ebx, 20
    mov ecx, 560
    mov edx, BM_COLOR_ERROR
    mov ebp, 1
    call bm_draw_text
.done:
    ret

; ---------------------------------------------------------------------------
; Grafischer Boot Manager
; ENTER startet Nova OS. D blendet validierte Diagnosedaten ein.
; R meldet den noch nicht implementierten Recoverypfad transparent.
; ---------------------------------------------------------------------------

boot_manager_run:
    test dword [BOOT_INFO_ADDRESS + BIB_OFF_FLAGS], NOVA_BOOT_FLAG_FRAMEBUFFER
    jz .no_graphics
    mov byte [bm_selection], 0
    mov byte [bm_countdown_active], 1
    mov byte [bm_countdown_digit], '5'
    call boot_manager_draw
    mov ebp, 5
.timeout_second:
    mov ecx, 18                     ; 18 * ~54,9 ms = rund eine Sekunde
.timeout_tick:
    call bm_wait_tick_or_key
    jc .cancel_countdown
    loop .timeout_tick
    dec ebp
    jz .start
    mov eax, ebp
    add al, '0'
    mov [bm_countdown_digit], al
    push ebp
    call bm_draw_status_countdown
    pop ebp
    jmp .timeout_second

.cancel_countdown:
    mov byte [bm_countdown_active], 0
    push eax
    call bm_clear_status_line
    pop eax
    jmp .handle_key

.timeout_complete:
    jmp .start

.wait_key:
    in al, 0x64
    test al, 0x01
    jz .wait_key
    in al, 0x60
.handle_key:
    test al, 0x80
    jnz .wait_key
    cmp al, 0x1C                    ; Enter
    je .activate
    cmp al, 0x48                    ; Pfeil hoch
    je .up
    cmp al, 0x50                    ; Pfeil runter
    je .down
    cmp al, 0x20                    ; D
    je .diagnostics
    cmp al, 0x13                    ; R
    je .recovery
    jmp .wait_key

.up:
    cmp byte [bm_selection], 0
    jne .up_dec
    mov byte [bm_selection], 5
    jmp .redraw
.up_dec:
    dec byte [bm_selection]
    jmp .redraw
.down:
    inc byte [bm_selection]
    cmp byte [bm_selection], 6
    jb .redraw
    mov byte [bm_selection], 0
.redraw:
    call boot_manager_draw
    jmp .wait_key
.activate:
    movzx eax, byte [bm_selection]
    imul eax, BM_ENTRY_SIZE
    movzx eax, byte [bm_menu_entries + eax + BM_ENTRY_ACTION]
    cmp eax, BM_ACTION_START
    je .start
    cmp eax, BM_ACTION_DIAGNOSTICS
    je .diagnostics
    cmp eax, BM_ACTION_RECOVERY
    je .recovery
    cmp eax, BM_ACTION_POWEROFF
    je .poweroff
    call boot_manager_draw_unavailable
    jmp .wait_key
.poweroff:
    mov ax, 0x2000
    mov dx, 0x0604
    out dx, ax
    mov dx, 0xB004
    out dx, ax
    jmp $

.diagnostics:
    mov esi, bm_event_diagnostics
    call bm_debug_write_string
    call boot_manager_draw_diagnostics
    jmp .wait_key
.recovery:
    mov esi, bm_event_recovery
    call bm_debug_write_string
    call boot_manager_draw_recovery_notice
    jmp .wait_key
.start:
    mov esi, bm_event_start
    call bm_debug_write_string
    call boot_manager_draw_starting
.no_graphics:
    ret

; PIT-Kanal 2 liefert einen firmwareunabhängigen Tick von rund 54,9 ms.
; CF=1 und AL=Scan-Code, sobald irgendeine Taste den Countdown abbricht.
bm_wait_tick_or_key:
    in al, 0x61
    and al, 0xFC
    out 0x61, al
    mov al, 0xB0
    out 0x43, al
    mov al, 0xFF
    out 0x42, al
    out 0x42, al
    in al, 0x61
    or al, 0x01
    and al, 0xFD
    out 0x61, al
.poll:
    in al, 0x64
    test al, 0x01
    jnz .key
    in al, 0x61
    test al, 0x20
    jz .poll
    clc
    ret
.key:
    in al, 0x60
    stc
    ret

boot_manager_draw:
    mov eax, BM_COLOR_BACKGROUND
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 16]
    mov esi, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 20]
    call bm_fill_rectangle

    ; Obere blaue Markenleiste mit sichtbaren, runden Enden.
    mov eax, BM_COLOR_BLUE
    mov ebx, 102
    xor ecx, ecx
    mov edx, 628
    mov esi, 13
    call bm_fill_rounded_rectangle

    ; Linke NovaOS-Marke
    mov dword [bm_logo_x], 32
    mov dword [bm_logo_y], 38
    mov dword [bm_logo_color], BM_COLOR_BLUE
    mov byte [bm_logo_compact], 1
    mov byte [bm_logo_mirror], 1
    call bm_draw_nova_logo
    mov esi, bm_text_brand
    mov ebx, 32
    mov ecx, 156
    mov edx, BM_COLOR_BLUE
    mov ebp, 3
    call bm_draw_text

    ; Menürahmen 184,55 bis 751,528
    mov eax, BM_COLOR_BLUE
    mov ebx, 184
    mov ecx, 55
    mov edx, 567
    mov esi, 473
    call bm_fill_rounded_rectangle
    mov eax, BM_COLOR_BACKGROUND
    mov ebx, 185
    mov ecx, 56
    mov edx, 565
    mov esi, 471
    call bm_fill_rounded_rectangle

    ; Aktuelle Auswahl
    movzx eax, byte [bm_selection]
    imul eax, 68
    add eax, 77
    mov ecx, eax
    mov eax, BM_COLOR_PANEL
    mov ebx, 204
    mov edx, 523
    mov esi, 62
    call bm_fill_rounded_rectangle

    ; Beschriftung, Symbol und Aktion stammen aus derselben Eintragstabelle.
    xor edi, edi
.draw_entry:
    cmp edi, BM_MENU_ENTRY_COUNT
    jae .entries_done
    mov eax, edi
    imul eax, BM_ENTRY_SIZE
    mov esi, [bm_menu_entries + eax + BM_ENTRY_LABEL]
    mov ebx, 278
    mov ecx, edi
    imul ecx, 68
    add ecx, 98
    mov edx, BM_COLOR_WHITE
    mov ebp, 2
    push edi
    call bm_draw_text
    pop edi

    mov eax, edi
    imul eax, BM_ENTRY_SIZE
    movzx edi, byte [bm_menu_entries + eax + BM_ENTRY_ICON]
    mov eax, BM_COLOR_WHITE
    mov ebx, 228
    mov ecx, [bm_draw_entry_index]
    imul ecx, 68
    add ecx, 101
    call bm_draw_menu_icon
    inc dword [bm_draw_entry_index]
    mov edi, [bm_draw_entry_index]
    jmp .draw_entry
.entries_done:
    mov dword [bm_draw_entry_index], 0

    cmp byte [bm_countdown_active], 0
    je .done
    call bm_draw_status_countdown
.done:
    ret

; EAX=Farbe, EBX=x, ECX=y, EDI=Symbolnummer.
; Jedes Symbol ist eine geglättete 20x20-Maske mit vier Deckungsstufen.
bm_draw_menu_icon:
    pushad
    mov [bm_icon_x], ebx
    mov [bm_icon_y], ecx
    mov [bm_text_color], eax
    mov esi, [bm_icon_table + edi * 4]
    xor ebp, ebp
.row:
    cmp ebp, 20
    jae .done
    xor ecx, ecx
.column:
    cmp ecx, 20
    jae .next_row
    mov eax, ebp
    imul eax, 5
    mov edx, ecx
    shr edx, 2
    add eax, edx
    mov al, [esi + eax]
    mov edx, ecx
    and edx, 3
    push ecx
    mov cl, dl
    shl cl, 1
    shr al, cl
    pop ecx
    and eax, 3
    jz .next_column
    push eax
    mov edi, ebp
    add edi, [bm_icon_y]
    imul edi, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 12]
    add edi, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 8]
    mov edx, ecx
    add edx, [bm_icon_x]
    shl edx, 2
    add edi, edx
    pop eax
    call bm_blend_pixel
.next_column:
    inc ecx
    jmp .column
.next_row:
    inc ebp
    jmp .row
.done:
    popad
    ret

boot_manager_draw_unavailable:
    call bm_clear_status_line
    mov esi, bm_text_unavailable
    mov ebx, 40
    mov ecx, 553
    mov edx, BM_COLOR_WARNING
    mov ebp, 1
    call bm_draw_text
    ret

bm_clear_status_line:
    mov eax, BM_COLOR_BACKGROUND
    xor ebx, ebx
    mov ecx, 540
    mov edx, 800
    mov esi, 40
    call bm_fill_rectangle
    ret

bm_draw_status_countdown:
    call bm_clear_status_line
    mov esi, bm_text_countdown
    mov ebx, 40
    mov ecx, 553
    mov edx, BM_COLOR_SECONDARY
    mov ebp, 2
    call bm_draw_text
    ret

boot_manager_draw_diagnostics:
    call bm_clear_status_line
    mov esi, bm_text_diagnostics
    mov ebx, 40
    mov ecx, 553
    mov edx, BM_COLOR_SECONDARY
    mov ebp, 1
    call bm_draw_text
    ret

boot_manager_draw_recovery_notice:
    call bm_clear_status_line
    mov esi, bm_text_recovery
    mov ebx, 40
    mov ecx, 553
    mov edx, BM_COLOR_WARNING
    mov ebp, 1
    call bm_draw_text
    ret

boot_manager_draw_starting:
    mov eax, BM_COLOR_PANEL
    mov ebx, 64
    mov ecx, 448
    mov edx, 672
    mov esi, 76
    call bm_fill_rectangle
    mov esi, bm_text_starting
    mov ebx, 96
    mov ecx, 474
    mov edx, BM_COLOR_CYAN
    mov ebp, 2
    call bm_draw_text
    ret

; EAX=Farbe, EBX=x, ECX=y, EDX=Breite, ESI=Höhe
bm_fill_rounded_rectangle:
    pushad
    mov [bm_round_color], eax
    mov [bm_round_x], ebx
    mov [bm_round_y], ecx
    mov [bm_round_width], edx
    mov [bm_round_height], esi
    mov eax, [bm_round_color]
    mov ebx, [bm_round_x]
    add ebx, 6
    mov ecx, [bm_round_y]
    mov edx, [bm_round_width]
    sub edx, 12
    mov esi, 1
    call bm_fill_rectangle
    mov ebx, [bm_round_x]
    add ebx, 3
    inc ecx
    mov edx, [bm_round_width]
    sub edx, 6
    call bm_fill_rectangle
    mov ebx, [bm_round_x]
    inc ebx
    inc ecx
    mov edx, [bm_round_width]
    sub edx, 2
    mov esi, 3
    call bm_fill_rectangle
    mov ebx, [bm_round_x]
    mov ecx, [bm_round_y]
    add ecx, 5
    mov edx, [bm_round_width]
    mov esi, [bm_round_height]
    sub esi, 10
    call bm_fill_rectangle
    mov ebx, [bm_round_x]
    inc ebx
    mov ecx, [bm_round_y]
    add ecx, [bm_round_height]
    sub ecx, 5
    mov edx, [bm_round_width]
    sub edx, 2
    mov esi, 3
    call bm_fill_rectangle
    mov ebx, [bm_round_x]
    add ebx, 3
    add ecx, 3
    mov edx, [bm_round_width]
    sub edx, 6
    mov esi, 1
    call bm_fill_rectangle
    mov ebx, [bm_round_x]
    add ebx, 6
    inc ecx
    mov edx, [bm_round_width]
    sub edx, 12
    call bm_fill_rectangle
    popad
    ret

bm_fill_rectangle:
    pushad
    mov [bm_rect_color], eax
    mov [bm_rect_x], ebx
    mov [bm_rect_y], ecx
    mov [bm_rect_width], edx
    mov [bm_rect_height], esi
    xor ebp, ebp
.row:
    cmp ebp, [bm_rect_height]
    jae .done
    mov eax, [bm_rect_y]
    add eax, ebp
    imul eax, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 12]
    add eax, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 8]
    mov edi, eax
    mov eax, [bm_rect_x]
    shl eax, 2
    add edi, eax
    mov eax, [bm_rect_color]
    mov ecx, [bm_rect_width]
    rep stosd
    inc ebp
    jmp .row
.done:
    popad
    ret

; Zeichnet das aus der offiziellen SVG erzeugte 2-Bit-AA-Logo rechts oben.
bm_draw_nova_logo:
    pushad
    mov eax, [bm_text_color]
    push eax
    mov eax, [bm_logo_color]
    mov [bm_text_color], eax
    xor ebp, ebp
.row:
    cmp ebp, NOVA_LOGO_HEIGHT
    jae .restore
    cmp byte [bm_logo_compact], 0
    je .row_visible
    mov eax, ebp
    and eax, 3
    cmp eax, 3
    je .next_row
.row_visible:
    xor ecx, ecx
.column:
    cmp ecx, NOVA_LOGO_WIDTH
    jae .next_row
    cmp byte [bm_logo_compact], 0
    je .column_visible
    mov eax, ecx
    and eax, 3
    cmp eax, 3
    je .next_column
.column_visible:
    mov eax, ebp
    imul eax, NOVA_LOGO_ROW_BYTES
    mov edx, ecx
    shr edx, 2
    add eax, edx
    mov al, [nova_logo_bitmap + eax]
    mov edx, ecx
    and edx, 3
    push ecx
    mov cl, dl
    shl cl, 1
    shr al, cl
    pop ecx
    and eax, 3
    jz .next_column
    push eax
    mov edi, ebp
    cmp byte [bm_logo_compact], 0
    je .row_scaled
    mov edx, ebp
    shr edx, 2
    sub edi, edx
.row_scaled:
    add edi, [bm_logo_y]
    imul edi, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 12]
    add edi, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 8]
    mov edx, ecx
    cmp byte [bm_logo_compact], 0
    je .column_scaled
    mov ebx, ecx
    shr ebx, 2
    sub edx, ebx
.column_scaled:
    cmp byte [bm_logo_mirror], 0
    je .column_positioned
    mov ebx, 119
    sub ebx, edx
    mov edx, ebx
.column_positioned:
    add edx, [bm_logo_x]
    shl edx, 2
    add edi, edx
    pop eax
    call bm_blend_pixel
.next_column:
    inc ecx
    jmp .column
.next_row:
    inc ebp
    jmp .row
.restore:
    pop eax
    mov [bm_text_color], eax
    popad
    ret

; Das ;( stammt als 2-Bit-Graustufenmaske exakt aus der UI-Referenz.
bm_draw_error_mark:
    pushad
    mov eax, [bm_text_color]
    push eax
    mov eax, [bm_error_mark_color]
    mov [bm_text_color], eax
    xor ebp, ebp
.row:
    cmp ebp, NOVA_ERROR_MARK_HEIGHT
    jae .restore
    xor ecx, ecx
.column:
    cmp ecx, NOVA_ERROR_MARK_WIDTH
    jae .next_row
    mov eax, ebp
    imul eax, NOVA_ERROR_MARK_ROW_BYTES
    mov edx, ecx
    shr edx, 2
    add eax, edx
    mov al, [nova_error_mark_bitmap + eax]
    mov edx, ecx
    and edx, 3
    push ecx
    mov cl, dl
    shl cl, 1
    shr al, cl
    pop ecx
    and eax, 3
    jz .next_column
    push eax
    mov edi, ebp
    add edi, [bm_error_mark_y]
    imul edi, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 12]
    add edi, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 8]
    mov edx, ecx
    add edx, [bm_error_mark_x]
    shl edx, 2
    add edi, edx
    pop eax
    call bm_blend_pixel
.next_column:
    inc ecx
    jmp .column
.next_row:
    inc ebp
    jmp .row
.restore:
    pop eax
    mov [bm_text_color], eax
    popad
    ret

; ESI=Text, EBX=x, ECX=y, EDX=Farbe, EBP=Skalierung
bm_draw_text:
    mov [bm_text_pointer], esi
    mov [bm_text_x], ebx
    mov [bm_text_y], ecx
    mov [bm_text_color], edx
    mov [bm_text_scale], ebp
.next:
    mov esi, [bm_text_pointer]
    lodsb
    mov [bm_text_pointer], esi
    test al, al
    jz .done
    cmp al, ' '
    je .space
    call bm_draw_character
.advance:
    movzx eax, byte [bm_character_advance]
    add eax, [bm_text_scale]
    add [bm_text_x], eax
    jmp .next
.space:
    mov byte [bm_character_advance], 5
    jmp .advance
.done:
    ret

bm_draw_character:
    pushad
    mov bl, al
    mov edi, bm_font_characters
    xor ecx, ecx
.find:
    mov al, [edi + ecx]
    test al, al
    jz .done
    cmp al, bl
    je .found
    inc ecx
    jmp .find
.found:
    mov al, [bm_font_widths + ecx]
    mov [bm_character_advance], al
    imul ecx, BM_FONT_BYTES_PER_GLYPH
    mov esi, bm_font_bitmap
    add esi, ecx
    xor ebp, ebp
.glyph_row:
    cmp ebp, BM_FONT_HEIGHT
    jae .done
    xor ecx, ecx
.glyph_column:
    cmp ecx, BM_FONT_WIDTH
    jae .next_row
    mov eax, ebp
    imul eax, BM_FONT_WIDTH
    add eax, ecx
    mov edx, eax
    shr eax, 2
    and edx, 3
    mov al, [esi + eax]
    push ecx
    mov cl, dl
    shl cl, 1
    shr al, cl
    pop ecx
    and eax, 3
    jz .next_column

    push eax
    mov edi, [bm_text_y]
    add edi, ebp
    imul edi, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 12]
    add edi, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 8]
    mov edx, [bm_text_x]
    add edx, ecx
    shl edx, 2
    add edi, edx
    pop eax
    call bm_blend_pixel
.next_column:
    inc ecx
    jmp .glyph_column
.next_row:
    inc ebp
    jmp .glyph_row
.done:
    popad
    ret

; EDI=Framebuffer-Pixel, EAX=Deckung 1..3. Mischt die Textfarbe mit
; dem vorhandenen Hintergrund, statt nur harte Ein/Aus-Pixel zu setzen.
bm_blend_pixel:
    cmp eax, 3
    je .opaque
    mov [bm_blend_alpha], eax
    pushad
    mov ebx, [edi]
    mov esi, [bm_text_color]

    mov eax, ebx
    and eax, 0xFF
    mov edx, esi
    and edx, 0xFF
    call bm_blend_channel
    mov [bm_blend_result], eax

    mov eax, ebx
    shr eax, 8
    and eax, 0xFF
    mov edx, esi
    shr edx, 8
    and edx, 0xFF
    call bm_blend_channel
    shl eax, 8
    or [bm_blend_result], eax

    mov eax, ebx
    shr eax, 16
    and eax, 0xFF
    mov edx, esi
    shr edx, 16
    and edx, 0xFF
    call bm_blend_channel
    shl eax, 16
    or eax, [bm_blend_result]
    mov [edi], eax
    popad
    ret
.opaque:
    mov eax, [bm_text_color]
    mov [edi], eax
    ret

; EAX=Hintergrundkanal, EDX=Textkanal. Ergebnis ist eine exakte
; Annäherung an (dst*(3-a) + src*a) / 3 für a=1 oder a=2.
bm_blend_channel:
    cmp dword [bm_blend_alpha], 1
    je .one_third
    lea eax, [eax + edx * 2]
    jmp .divide
.one_third:
    lea eax, [edx + eax * 2]
.divide:
    imul eax, eax, 171
    shr eax, 9
    ret

bm_debug_write_string:
    push eax
    push edx
    mov dx, 0x00E9
.next:
    lodsb
    test al, al
    jz .done
    out dx, al
    jmp .next
.done:
    pop edx
    pop eax
    ret

BM_COLOR_BACKGROUND equ 0x00101113
BM_COLOR_PANEL      equ 0x001A1C20
BM_COLOR_CYAN       equ 0x004CC2FF
BM_COLOR_WHITE      equ 0x00FFFFFF
BM_COLOR_SECONDARY  equ 0x00CFCFCF
BM_COLOR_SUCCESS    equ 0x0030D158
BM_COLOR_WARNING    equ 0x00FFB347
BM_COLOR_BLUE       equ 0x002D7FC1
BM_COLOR_ERROR      equ 0x00B6154B
BM_COLOR_ERROR_TEXT equ 0x00A9A9A9

align 4
bm_rect_color:   dd 0
bm_rect_x:       dd 0
bm_rect_y:       dd 0
bm_rect_width:   dd 0
bm_rect_height:  dd 0
bm_text_pointer: dd 0
bm_text_x:       dd 0
bm_text_y:       dd 0
bm_text_color:   dd 0
bm_text_scale:   dd 1
bm_character_advance: db 0
align 4
bm_blend_alpha:  dd 0
bm_blend_result: dd 0
bm_round_color:  dd 0
bm_round_x:      dd 0
bm_round_y:      dd 0
bm_round_width:  dd 0
bm_round_height: dd 0
bm_logo_x:       dd 620
bm_logo_y:       dd 22
bm_logo_color:   dd BM_COLOR_WHITE
bm_logo_compact: db 0
bm_logo_mirror:  db 0
align 4
bm_icon_x:        dd 0
bm_icon_y:        dd 0
bm_icon_table:
    dd bm_icon_home, bm_icon_install, bm_icon_settings
    dd bm_icon_diagnostics, bm_icon_recover, bm_icon_power
bm_error_mark_x: dd 60
bm_error_mark_y: dd 82
bm_error_mark_color: dd BM_COLOR_WHITE
bm_selection:    db 0
bm_countdown_active: db 0
align 4
bm_draw_entry_index: dd 0

bm_text_brand:       db "NovaOS",0
bm_text_title:       db "BOOT MANAGER",0
bm_text_os:          db "NOVA OS",0
bm_text_kernel:      db "STANDARDKERNEL | NKI V1 | X86-32",0
bm_text_verified:    db "STARTBEREIT",0
bm_text_security:    db "SICHERHEIT NOCH NICHT VERIFIZIERT",0
bm_text_enter:       db "ENTER  SYSTEM STARTEN",0
bm_text_actions:     db "D  DIAGNOSE     R  RECOVERY-STATUS",0
bm_text_diagnostics: db "CRC32 OK | NBHP/BIB 1.0 | MEMORY MAP OK | VBE OK",0
bm_text_recovery:    db "RECOVERY IST IN DIESEM PROTOTYP NOCH NICHT VERFUEGBAR",0
bm_text_starting:    db "NOVA OS WIRD GESTARTET",0
bm_menu_start:       db "NovaOS starten",0
bm_menu_install:     db "NovaOS installieren",0
bm_menu_settings:    db "Einstellungen",0
bm_menu_diagnostics: db "Diagnose",0
bm_menu_recover:     db "Recover",0
bm_menu_poweroff:    db "Ausschalten",0
align 4
bm_menu_entries:
    dd bm_menu_start
    db 0, BM_ACTION_START
    dw 0
    dd bm_menu_install
    db 1, BM_ACTION_UNAVAILABLE
    dw 0
    dd bm_menu_settings
    db 2, BM_ACTION_UNAVAILABLE
    dw 0
    dd bm_menu_diagnostics
    db 3, BM_ACTION_DIAGNOSTICS
    dw 0
    dd bm_menu_recover
    db 4, BM_ACTION_RECOVERY
    dw 0
    dd bm_menu_poweroff
    db 5, BM_ACTION_POWEROFF
    dw 0
BM_MENU_ENTRY_COUNT equ ($ - bm_menu_entries) / BM_ENTRY_SIZE
bm_text_countdown:   db "Auswahl startet in "
bm_countdown_digit:  db "5"
                     db " Sek.",0
bm_text_unavailable: db "Diese Funktion ist noch nicht verf",0x81,"gbar",0
bm_error_brand:         db "NovaOS",0
bm_error_title:         db "Start nicht m",0x94,"glich",0
bm_error_explanation_1: db "NovaOS hat eine besch",0x84,"digte oder unvollst",0x84,"ndige Datei gefunden",0
bm_error_explanation_2: db "Zu Ihrer Sicherheit wurde sie nicht gestartet",0
bm_error_explanation_3: db 0
bm_error_action_1:      db "Bitte das Installationsmedium neu erstellen",0
bm_error_action_2:      db "oder",0
bm_error_action_3:      db "Eine andere Kopie von NovaOS verwenden",0
bm_error_checksum_code: db "FEHLERCODE: BOOT-1002 | Pr",0x81,"fsumme Ung",0x81,"ltig",0
bm_error_header_code:   db "FEHLERCODE: BOOT-1001 | Dateiformat Ung",0x81,"ltig",0
bm_event_diagnostics: db "BM:DIAGNOSTICS",10,0
bm_event_recovery:    db "BM:RECOVERY-UNAVAILABLE",10,0
bm_event_start:       db "BM:START",10,0

%include "boot-font-aa.inc"
%include "nova-art.inc"
%include "boot-icons-aa.inc"

align 8
gdt_start:
    dq 0
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
    dq 0x00AF9A000000FFFF
gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[bits 16]
print_string:
    lodsb
    test al, al
    jz .done
    push dx
    mov dx, 0x00E9
    out dx, al
    pop dx
    mov ah, 0x0E
    xor bh, bh
    mov bl, 0x07
    int 0x10
    jmp print_string
.done:
    ret

fatal_halt:
    cli
.loop:
    hlt
    jmp .loop

kernel_load_error:
    mov si, message_kernel_load_error
    call print_string
    jmp fatal_halt

boot_drive:                 db 0
boot_error_code:            db 0
kernel_payload_format:      db 0
retry_count:                db 0
kernel_destination_offset: dw 0
kernel_cylinder:            db 0
kernel_head:                db 0
kernel_sector:              db 0
kernel_remaining:           db 0
sectors_per_track:          db 18
maximum_head:               db 1

align 4
kernel_payload_base:        dd 0
kernel_payload_crc:         dd 0
elf_load_address:           dd 0
elf_memory_size:            dd 0
elf_entry_point:            dd 0
elf_ph_pointer:             dd 0
elf_ph_remaining:           dd 0
elf_min_address:            dd 0
elf_max_address:            dd 0
elf_segment_count:          dd 0
elf_current_start:          dd 0
elf_current_end:            dd 0
elf_note_name_size:         dd 0
elf_note_desc_size:         dd 0
elf_note_next:              dd 0
elf_entry_covered:          db 0
elf_build_id_found:         db 0
elf_nova_metadata_found:    db 0

align 4
elf_build_id:               times 5 dd 0
elf_segment_starts:         times 16 dd 0
elf_segment_ends:           times 16 dd 0

align 4
kernel_dap:
    db 0x10, 0
    dw KERNEL_IMAGE_SECTOR_COUNT
    dw KERNEL_TEMP_OFFSET
    dw KERNEL_TEMP_SEGMENT
    dq KERNEL_FIRST_LBA

message_start:
    db "Nova Stage 2 / NBHP v1", 13, 10, 0
message_vbe_fallback:
    db "BOOT-W001: VBE nicht verfuegbar, Textmodus aktiv", 13, 10, 0
message_memory_error:
    db "BOOT-2001: Speicherkarte nicht verfuegbar", 13, 10, 0
message_kernel_load_error:
    db "BOOT-1001: Kernel nicht gefunden", 13, 10, 0
message_kernel_header_error:
    db "BOOT-1002: Kernel-Header ungueltig", 13, 10, 0
message_kernel_checksum_error:
    db "BOOT-1002: Kernel-Pruefsumme ungueltig", 13, 10, 0
message_bib_error:
    db "BOOT-1011: NBHP/BIB-Struktur ungueltig", 13, 10, 0

times STAGE2_BYTE_COUNT - ($ - $$) db 0
