; Nova Boot Manager - BIOS Stage 2
; Referenzimplementierung für NKI v1 und NBHP/BIB v1.

%include "layout.inc"
%include "boot-ui-tokens.inc"

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
BM_ACTION_SETTINGS      equ 5

BM_VIEW_MAIN            equ 0
BM_VIEW_SETTINGS        equ 1
BM_VIEW_DIAGNOSTICS     equ 2
BM_VIEW_RECOVERY        equ 3
BM_VIEW_UNAVAILABLE     equ 4
BM_VIEW_HELP            equ 5
BM_VIEW_DETAILS         equ 6
BM_VIEW_ADVANCED        equ 7
BM_VIEW_POWER           equ 8
BM_VIEW_SELF_HEALING    equ 9
BM_VIEW_SNAPSHOTS       equ 10
BM_VIEW_MEMORY_TEST     equ 11
BM_VIEW_DISK_TOOLS      equ 12
BM_VIEW_ENCRYPTION      equ 13
BM_VIEW_NETWORK         equ 14
BM_VIEW_FIRMWARE        equ 15

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
    mov word [vbe_best_mode], 0xFFFF
    mov byte [vbe_best_rank], 0xFF

    ; VBE 2 ControllerInfo anfordern. Das vorinitialisierte "VBE2" bittet
    ; die Firmware, die erweiterte 512-Byte-Struktur zurückzugeben.
    xor ax, ax
    mov es, ax
    mov di, VBE_CONTROLLER_INFO_ADDRESS
    mov dword [es:di], 0x32454256       ; "VBE2"
    mov ax, 0x4F00
    int 0x10
    cmp ax, 0x004F
    jne .fallback

    xor ax, ax
    mov ds, ax
    mov es, ax
    cmp dword [VBE_CONTROLLER_INFO_ADDRESS], 0x41534556 ; "VESA"
    jne .fallback
    cmp word [VBE_CONTROLLER_INFO_ADDRESS + 4], 0x0200
    jb .fallback

    mov si, [VBE_CONTROLLER_INFO_ADDRESS + 0x0E]
    mov ax, [VBE_CONTROLLER_INFO_ADDRESS + 0x10]
    mov fs, ax
    mov dx, ax
    or dx, si
    jz .fallback

    ; Die Firmwareliste ist nicht vertrauenswürdig. Eine harte Obergrenze
    ; verhindert endloses Lesen bei fehlendem 0xFFFF-Abschluss.
    mov bp, 256
.mode_loop:
    mov dx, [fs:si]
    cmp dx, 0xFFFF
    je .mode_scan_done
    mov [vbe_candidate_mode], dx

    push fs
    push si
    push bp
    mov cx, dx
    xor ax, ax
    mov es, ax
    mov di, VBE_MODE_INFO_ADDRESS
    mov ax, 0x4F01
    int 0x10
    mov bx, ax
    pop bp
    pop si
    pop fs

    xor ax, ax
    mov ds, ax
    mov es, ax
    cmp bx, 0x004F
    jne .next_mode
    call validate_vbe_mode_info
    jc .next_mode
    call vbe_mode_rank
    cmp al, [vbe_best_rank]
    jae .next_mode
    mov [vbe_best_rank], al
    mov dx, [vbe_candidate_mode]
    mov [vbe_best_mode], dx
    test al, al
    jz .mode_scan_done                 ; höchste Präferenz erreicht

.next_mode:
    add si, 2
    jnc .mode_pointer_ready
    mov ax, fs
    add ax, 0x1000
    mov fs, ax
.mode_pointer_ready:
    dec bp
    jnz .mode_loop

.mode_scan_done:
    cmp word [vbe_best_mode], 0xFFFF
    je .fallback

    ; Gewählten Descriptor unmittelbar vor und nach SetMode erneut prüfen.
    mov cx, [vbe_best_mode]
    xor ax, ax
    mov es, ax
    mov di, VBE_MODE_INFO_ADDRESS
    mov ax, 0x4F01
    int 0x10
    cmp ax, 0x004F
    jne .fallback
    xor ax, ax
    mov ds, ax
    mov es, ax
    call validate_vbe_mode_info
    jc .fallback

    mov ax, 0x4F02
    mov bx, [vbe_best_mode]
    or bx, 0x4000
    int 0x10
    cmp ax, 0x004F
    jne .fallback

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov cx, [vbe_best_mode]
    mov di, VBE_MODE_INFO_ADDRESS
    mov ax, 0x4F01
    int 0x10
    cmp ax, 0x004F
    jne .fallback
    xor ax, ax
    mov ds, ax
    mov es, ax
    call validate_vbe_mode_info
    jc .fallback

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
    mov si, message_vbe_ready
    call bios_debug_write_string
    mov al, [vbe_best_rank]
    add al, '0'
    mov [message_vbe_rank_digit], al
    mov si, message_vbe_rank
    call bios_debug_write_string
    ret

.fallback:
    xor ax, ax
    mov ds, ax
    mov es, ax
    and dword [BOOT_INFO_ADDRESS + BIB_OFF_FLAGS], ~NOVA_BOOT_FLAG_FRAMEBUFFER
    mov di, BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 8
    mov cx, 6
    xor eax, eax
    rep stosd
    mov ax, 0x0003
    int 0x10
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov si, message_vbe_text_fallback
    call bios_debug_write_string
    mov si, message_vbe_fallback
    call print_string
    ret

; CF=0 genau dann, wenn die ModeInfo als 32-Bit-BGRX-LFB sicher nutzbar ist.
validate_vbe_mode_info:
    mov ax, [VBE_MODE_INFO_ADDRESS]
    test ax, 0x0001                    ; Modus unterstützt
    jz .invalid
    test ax, 0x0010                    ; Grafikmodus
    jz .invalid
    test ax, 0x0080                    ; Linear Framebuffer
    jz .invalid
    cmp byte [VBE_MODE_INFO_ADDRESS + 25], 32
    jne .invalid
    cmp byte [VBE_MODE_INFO_ADDRESS + 27], 6 ; Direct Color
    jne .invalid
    cmp dword [VBE_MODE_INFO_ADDRESS + 40], 0
    je .invalid
    cmp word [VBE_MODE_INFO_ADDRESS + 18], 0
    je .invalid
    cmp word [VBE_MODE_INFO_ADDRESS + 20], 0
    je .invalid

    movzx eax, word [VBE_MODE_INFO_ADDRESS + 18]
    shl eax, 2
    movzx edx, word [VBE_MODE_INFO_ADDRESS + 16]
    cmp edx, eax
    jb .invalid

    ; Pitch * Höhe sowie PhysBasePtr + Größe müssen in 32 Bit passen.
    mov eax, edx
    movzx ecx, word [VBE_MODE_INFO_ADDRESS + 20]
    mul ecx
    test edx, edx
    jnz .invalid
    add eax, [VBE_MODE_INFO_ADDRESS + 40]
    jc .invalid

    cmp byte [VBE_MODE_INFO_ADDRESS + 31], 8
    jne .invalid
    cmp byte [VBE_MODE_INFO_ADDRESS + 32], 16
    jne .invalid
    cmp byte [VBE_MODE_INFO_ADDRESS + 33], 8
    jne .invalid
    cmp byte [VBE_MODE_INFO_ADDRESS + 34], 8
    jne .invalid
    cmp byte [VBE_MODE_INFO_ADDRESS + 35], 8
    jne .invalid
    cmp byte [VBE_MODE_INFO_ADDRESS + 36], 0
    jne .invalid
    cmp byte [VBE_MODE_INFO_ADDRESS + 37], 0
    je .valid
    cmp byte [VBE_MODE_INFO_ADDRESS + 37], 8
    jne .invalid
    cmp byte [VBE_MODE_INFO_ADDRESS + 38], 24
    jne .invalid
.valid:
    clc
    ret
.invalid:
    stc
    ret

; AL: 0 = beste definierte Auflösung, 6 = sonstiger kompatibler Modus.
vbe_mode_rank:
    mov al, 0
    cmp word [VBE_MODE_INFO_ADDRESS + 18], 1920
    jne .rank_1600
    cmp word [VBE_MODE_INFO_ADDRESS + 20], 1080
    je .done
.rank_1600:
    inc al
    cmp word [VBE_MODE_INFO_ADDRESS + 18], 1600
    jne .rank_1366
    cmp word [VBE_MODE_INFO_ADDRESS + 20], 900
    je .done
.rank_1366:
    inc al
    cmp word [VBE_MODE_INFO_ADDRESS + 18], 1366
    jne .rank_1280
    cmp word [VBE_MODE_INFO_ADDRESS + 20], 768
    je .done
.rank_1280:
    inc al
    cmp word [VBE_MODE_INFO_ADDRESS + 18], 1280
    jne .rank_1024
    cmp word [VBE_MODE_INFO_ADDRESS + 20], 720
    je .done
.rank_1024:
    inc al
    cmp word [VBE_MODE_INFO_ADDRESS + 18], 1024
    jne .rank_800
    cmp word [VBE_MODE_INFO_ADDRESS + 20], 768
    je .done
.rank_800:
    inc al
    cmp word [VBE_MODE_INFO_ADDRESS + 18], 800
    jne .other
    cmp word [VBE_MODE_INFO_ADDRESS + 20], 600
    je .done
.other:
    mov al, 6
.done:
    ret

; Real-Mode-Debugausgabe. bm_debug_write_string ist als 32-Bit-Routine
; assembliert und darf vor dem Protected-Mode-Wechsel nicht aufgerufen werden.
bios_debug_write_string:
    push ax
    push dx
    mov dx, 0x00E9
.next:
    lodsb
    test al, al
    jz .done
    out dx, al
    jmp .next
.done:
    pop dx
    pop ax
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
    mov word [kernel_destination_segment], KERNEL_TEMP_SEGMENT

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
    mov ax, [kernel_destination_segment]
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
    jnc .destination_ready
    add word [kernel_destination_segment], 0x1000
.destination_ready:
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
    mov byte [bm_subselection], 0
    mov byte [bm_view], BM_VIEW_MAIN
    mov byte [bm_countdown_active], 1
    mov byte [bm_countdown_digit], '5'
    call bm_mouse_initialize
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
    call bm_wait_input_event
    cmp eax, 1
    je .keyboard_event
    cmp eax, 2
    je .mouse_redraw
    cmp eax, 3
    je .mouse_click
    jmp .wait_key
.keyboard_event:
    mov al, bl
    jmp .handle_key
.mouse_redraw:
    call bm_redraw_current_view
    jmp .wait_key
.mouse_click:
    call bm_mouse_hit_test
    jnc .mouse_redraw
    jmp .handle_key
.handle_key:
    test al, 0x80
    jnz .wait_key
    cmp al, 0x3B                    ; F1: kontextuelle Hilfe aus jeder Ansicht
    je .help
    cmp al, 0x23                    ; H: globaler Hochkontrastmodus
    je .toggle_contrast
    cmp byte [bm_view], BM_VIEW_ADVANCED
    je .handle_advanced_key
    cmp byte [bm_view], BM_VIEW_POWER
    je .handle_power_key
    cmp byte [bm_view], BM_VIEW_DETAILS
    je .handle_details_key
    cmp byte [bm_view], BM_VIEW_RECOVERY
    je .handle_recovery_key
    cmp byte [bm_view], BM_VIEW_MEMORY_TEST
    je .handle_memory_key
    cmp byte [bm_view], BM_VIEW_MAIN
    je .handle_main_key
    cmp al, 0x01                    ; ESC: einheitliche Zurück-Navigation
    je .back_to_main
    cmp al, 0x0E                    ; Backspace: alternative Zurück-Taste
    je .back_to_main
    cmp al, 0x20                    ; D wechselt direkt zur Diagnose
    je .diagnostics
    cmp al, 0x13                    ; R wechselt direkt zur Recovery-Seite
    je .recovery
    jmp .wait_key
.toggle_contrast:
    call bm_toggle_contrast_theme
    call bm_redraw_current_view
    jmp .wait_key
.handle_main_key:
    cmp al, 0x3E                    ; F4: Netzwerk-Boot-Status
    je .network
    cmp al, 0x3F                    ; F5: Firmwareinformationen
    je .firmware
    cmp al, 0x40                    ; F6: Verschluesselungsstatus
    je .encryption
    cmp al, 0x3C                    ; F2: Details des markierten Eintrags
    je .details
    cmp al, 0x3D                    ; F3: erweiterte Startoptionen
    je .advanced
    cmp al, 0x47                    ; Pos1
    je .first
    cmp al, 0x4F                    ; Ende
    je .last
    cmp al, 0x1F                    ; S: Einstellungen
    je .settings
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
.first:
    mov byte [bm_selection], 0
    jmp .redraw
.last:
    mov byte [bm_selection], BM_MENU_ENTRY_COUNT - 1
    jmp .redraw

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
    je .power_dialog
    cmp eax, BM_ACTION_SETTINGS
    je .settings
    mov byte [bm_view], BM_VIEW_UNAVAILABLE
    call boot_manager_draw_unavailable
    jmp .wait_key
.back_to_main:
    mov esi, bm_event_back
    call bm_debug_write_string
    cmp byte [bm_view], BM_VIEW_SELF_HEALING
    je .back_to_recovery
    cmp byte [bm_view], BM_VIEW_SNAPSHOTS
    je .back_to_recovery
    cmp byte [bm_view], BM_VIEW_MEMORY_TEST
    je .back_to_recovery
    cmp byte [bm_view], BM_VIEW_DISK_TOOLS
    je .back_to_recovery
    mov byte [bm_view], BM_VIEW_MAIN
    call boot_manager_draw
    jmp .wait_key
.back_to_recovery:
    mov byte [bm_view], BM_VIEW_RECOVERY
    call boot_manager_draw_recovery_notice
    jmp .wait_key
.poweroff:
    mov ax, 0x2000
    mov dx, 0x0604
    out dx, ax
    mov dx, 0xB004
    out dx, ax
    jmp $

.restart:
    mov al, 0xFE
    out 0x64, al
    jmp $

.details:
    mov byte [bm_view], BM_VIEW_DETAILS
    mov esi, bm_event_details
    call bm_debug_write_string
    call boot_manager_draw_details
    jmp .wait_key
.advanced:
    mov byte [bm_subselection], 0
    mov byte [bm_view], BM_VIEW_ADVANCED
    mov esi, bm_event_advanced
    call bm_debug_write_string
    call boot_manager_draw_advanced
    jmp .wait_key
.power_dialog:
    mov byte [bm_subselection], 0
    mov byte [bm_view], BM_VIEW_POWER
    mov esi, bm_event_power_dialog
    call bm_debug_write_string
    call boot_manager_draw_power_dialog
    jmp .wait_key

.handle_advanced_key:
    cmp al, 0x01
    je .back_to_main
    cmp al, 0x0E
    je .back_to_main
    cmp al, 0x48
    je .sub_up_advanced
    cmp al, 0x50
    je .sub_down_advanced
    cmp al, 0x1C
    jne .wait_key
    cmp byte [bm_subselection], 0
    je .start
    cmp byte [bm_subselection], 1
    je .recovery
    mov byte [bm_view], BM_VIEW_UNAVAILABLE
    call boot_manager_draw_unavailable
    jmp .wait_key
.sub_up_advanced:
    cmp byte [bm_subselection], 0
    jne .sub_adv_dec
    mov byte [bm_subselection], 4
    jmp .redraw_advanced
.sub_adv_dec:
    dec byte [bm_subselection]
    jmp .redraw_advanced
.sub_down_advanced:
    inc byte [bm_subselection]
    cmp byte [bm_subselection], 5
    jb .redraw_advanced
    mov byte [bm_subselection], 0
.redraw_advanced:
    call boot_manager_draw_advanced
    jmp .wait_key

.handle_details_key:
    cmp al, 0x01
    je .back_to_main
    cmp al, 0x0E
    je .back_to_main
    cmp al, 0x1C
    je .start
    cmp al, 0x3D
    je .advanced
    jmp .wait_key

.handle_power_key:
    cmp al, 0x01
    je .back_to_main
    cmp al, 0x0E
    je .back_to_main
    cmp al, 0x48
    je .sub_up_power
    cmp al, 0x50
    je .sub_down_power
    cmp al, 0x1C
    jne .wait_key
    cmp byte [bm_subselection], 0
    je .poweroff
    cmp byte [bm_subselection], 1
    je .restart
    cmp byte [bm_subselection], 2
    je .recovery
    cmp byte [bm_subselection], 3
    je .firmware
    mov byte [bm_view], BM_VIEW_UNAVAILABLE
    call boot_manager_draw_unavailable
    jmp .wait_key
.sub_up_power:
    cmp byte [bm_subselection], 0
    jne .sub_power_dec
    mov byte [bm_subselection], 3
    jmp .redraw_power
.sub_power_dec:
    dec byte [bm_subselection]
    jmp .redraw_power
.sub_down_power:
    inc byte [bm_subselection]
    cmp byte [bm_subselection], 4
    jb .redraw_power
    mov byte [bm_subselection], 0
.redraw_power:
    call boot_manager_draw_power_dialog
    jmp .wait_key

.handle_recovery_key:
    cmp al, 0x01
    je .back_to_main
    cmp al, 0x0E
    je .back_to_main
    cmp al, 0x48
    je .sub_up_recovery
    cmp al, 0x50
    je .sub_down_recovery
    cmp al, 0x1C
    jne .wait_key
    cmp byte [bm_subselection], 0
    je .self_healing
    cmp byte [bm_subselection], 1
    je .snapshots
    cmp byte [bm_subselection], 2
    je .memory_test
    jmp .disk_tools
.sub_up_recovery:
    cmp byte [bm_subselection], 0
    jne .sub_recovery_dec
    mov byte [bm_subselection], 3
    jmp .redraw_recovery
.sub_recovery_dec:
    dec byte [bm_subselection]
    jmp .redraw_recovery
.sub_down_recovery:
    inc byte [bm_subselection]
    cmp byte [bm_subselection], 4
    jb .redraw_recovery
    mov byte [bm_subselection], 0
.redraw_recovery:
    call boot_manager_draw_recovery_notice
    jmp .wait_key
.self_healing:
    mov byte [bm_view], BM_VIEW_SELF_HEALING
    mov esi, bm_event_self_healing
    call bm_debug_write_string
    call boot_manager_draw_self_healing
    jmp .wait_key
.snapshots:
    mov byte [bm_view], BM_VIEW_SNAPSHOTS
    mov esi, bm_event_snapshots
    call bm_debug_write_string
    call boot_manager_draw_snapshots
    jmp .wait_key
.memory_test:
    mov byte [bm_memory_result], 0
    mov byte [bm_view], BM_VIEW_MEMORY_TEST
    mov esi, bm_event_memory_test
    call bm_debug_write_string
    call boot_manager_draw_memory_test
    jmp .wait_key
.disk_tools:
    mov byte [bm_view], BM_VIEW_DISK_TOOLS
    mov esi, bm_event_disk_tools
    call bm_debug_write_string
    call boot_manager_draw_disk_tools
    jmp .wait_key

.handle_memory_key:
    cmp al, 0x01
    je .back_to_main
    cmp al, 0x0E
    je .back_to_main
    cmp al, 0x1C
    jne .wait_key
    call bm_run_memory_quick_test
    call boot_manager_draw_memory_test
    jmp .wait_key

.network:
    mov byte [bm_view], BM_VIEW_NETWORK
    mov esi, bm_event_network
    call bm_debug_write_string
    call boot_manager_draw_network
    jmp .wait_key
.firmware:
    mov byte [bm_view], BM_VIEW_FIRMWARE
    mov esi, bm_event_firmware
    call bm_debug_write_string
    call boot_manager_draw_firmware
    jmp .wait_key
.encryption:
    mov byte [bm_view], BM_VIEW_ENCRYPTION
    mov esi, bm_event_encryption
    call bm_debug_write_string
    call boot_manager_draw_encryption
    jmp .wait_key

.diagnostics:
    mov byte [bm_view], BM_VIEW_DIAGNOSTICS
    mov esi, bm_event_diagnostics
    call bm_debug_write_string
    call boot_manager_draw_diagnostics
    jmp .wait_key
.recovery:
    mov byte [bm_subselection], 0
    mov byte [bm_view], BM_VIEW_RECOVERY
    mov esi, bm_event_recovery
    call bm_debug_write_string
    call boot_manager_draw_recovery_notice
    jmp .wait_key
.settings:
    mov esi, bm_event_settings
    call bm_debug_write_string
    mov byte [bm_view], BM_VIEW_SETTINGS
    call boot_manager_draw_settings
    jmp .wait_key
.help:
    mov byte [bm_view], BM_VIEW_HELP
    mov esi, bm_event_help
    call bm_debug_write_string
    call boot_manager_draw_help
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
    test al, 0x20
    jnz .mouse_byte
    in al, 0x60
    stc
    ret
.mouse_byte:
    call bm_mouse_consume_byte
    jnc .poll
    xor eax, eax                    ; vollständiges Mausereignis pausiert
    stc
    ret

; Einheitliches, pollingbasiertes Eingabeereignis.
; EAX=1 Tastatur (BL=Scan-Code), EAX=2 Mausbewegung, EAX=3 Linksklick.
bm_wait_input_event:
.poll:
    in al, 0x64
    test al, 0x01
    jz .poll
    test al, 0x20
    jnz .mouse
    in al, 0x60
    mov bl, al
    mov eax, 1
    ret
.mouse:
    call bm_mouse_consume_byte
    jnc .poll
    ret

; Aktiviert das PS/2-Auxiliary-Gerät und den Standard-Streamingmodus. Alle
; Wartevorgänge sind begrenzt, damit fehlende Hardware den Boot nie blockiert.
bm_mouse_initialize:
    mov dword [bm_mouse_x], 400
    mov dword [bm_mouse_y], 300
    mov byte [bm_mouse_phase], 0
    mov byte [bm_mouse_buttons], 0
    call bm_ps2_wait_input_clear
    jc .unavailable
    mov al, 0xA8
    out 0x64, al
    mov al, 0xF6                    ; Standardwerte
    call bm_mouse_send_command
    jc .unavailable
    mov al, 0xF4                    ; Streaming aktivieren
    call bm_mouse_send_command
    jc .unavailable
    mov byte [bm_mouse_available], 1
    ret
.unavailable:
    mov byte [bm_mouse_available], 0
    ret

bm_mouse_send_command:
    push eax
    call bm_ps2_wait_input_clear
    jc .failed_pop
    mov al, 0xD4
    out 0x64, al
    call bm_ps2_wait_input_clear
    jc .failed_pop
    pop eax
    out 0x60, al
    call bm_ps2_wait_output
    jc .failed
    in al, 0x60
    cmp al, 0xFA
    jne .failed
    clc
    ret
.failed_pop:
    pop eax
.failed:
    stc
    ret

bm_ps2_wait_input_clear:
    mov ecx, 0x20000
.loop:
    in al, 0x64
    test al, 0x02
    jz .ready
    loop .loop
    stc
    ret
.ready:
    clc
    ret

bm_ps2_wait_output:
    mov ecx, 0x20000
.loop:
    in al, 0x64
    test al, 0x01
    jnz .ready
    loop .loop
    stc
    ret
.ready:
    clc
    ret

; Nimmt genau ein Byte aus Port 60 entgegen. CF=1 erst nach einem vollständigen
; 3-Byte-Paket; EAX bezeichnet dann Bewegung oder Linksklick.
bm_mouse_consume_byte:
    in al, 0x60
    movzx edx, byte [bm_mouse_phase]
    test edx, edx
    jnz .store
    test al, 0x08                    ; Synchronisationsbit des ersten Bytes
    jz .incomplete
.store:
    mov [bm_mouse_packet + edx], al
    inc edx
    cmp edx, 3
    jb .save_phase
    mov byte [bm_mouse_phase], 0
    movsx edx, byte [bm_mouse_packet + 1]
    add [bm_mouse_x], edx
    movsx edx, byte [bm_mouse_packet + 2]
    neg edx
    add [bm_mouse_y], edx
    cmp dword [bm_mouse_x], 0
    jge .x_upper
    mov dword [bm_mouse_x], 0
.x_upper:
    cmp dword [bm_mouse_x], 799
    jle .y_lower
    mov dword [bm_mouse_x], 799
.y_lower:
    cmp dword [bm_mouse_y], 0
    jge .y_upper
    mov dword [bm_mouse_y], 0
.y_upper:
    cmp dword [bm_mouse_y], 599
    jle .buttons
    mov dword [bm_mouse_y], 599
.buttons:
    mov dl, [bm_mouse_buttons]
    mov al, [bm_mouse_packet]
    and al, 0x07
    mov [bm_mouse_buttons], al
    test al, 0x01
    jz .movement
    test dl, 0x01
    jnz .movement
    mov eax, 3
    stc
    ret
.movement:
    mov eax, 2
    stc
    ret
.save_phase:
    mov [bm_mouse_phase], dl
.incomplete:
    clc
    ret

; Linksklick-Hit-Testing. CF=1 und AL=synthetischer Tastatur-Scan-Code.
bm_mouse_hit_test:
    cmp byte [bm_view], BM_VIEW_MAIN
    jne .page
    mov eax, [bm_mouse_x]
    cmp eax, 204
    jb .none
    cmp eax, 727
    ja .none
    mov eax, [bm_mouse_y]
    cmp eax, 77
    jb .none
    cmp eax, 485
    ja .none
    sub eax, 77
    xor edx, edx
    mov ecx, 68
    div ecx
    cmp eax, BM_MENU_ENTRY_COUNT
    jae .none
    mov [bm_selection], al
    mov al, 0x1C                    ; Aktivierung entspricht Enter
    stc
    ret
.page:
    mov eax, [bm_mouse_x]
    cmp eax, 190
    jb .none
    cmp eax, 520
    ja .none
    mov eax, [bm_mouse_y]
    cmp eax, 470
    jb .none
    cmp eax, 525
    ja .none
    mov al, 0x01                    ; Zurück entspricht ESC
    stc
    ret
.none:
    clc
    ret

boot_manager_draw:
    mov eax, [bm_theme_background]
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

    ; Seitentitel. Die Referenzauflösung verwendet 1 px = 1 DLU; spätere
    ; Backends skalieren diese logischen Koordinaten vor der Ausgabe.
    mov esi, bm_text_screen_title
    mov ebx, 204
    mov ecx, 27
    mov edx, BM_COLOR_WHITE
    mov ebp, 2
    call bm_draw_text

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
    mov ebx, NOVA_UI_PANEL_X
    mov ecx, NOVA_UI_PANEL_Y
    mov edx, NOVA_UI_PANEL_WIDTH
    mov esi, NOVA_UI_PANEL_HEIGHT
    call bm_fill_rounded_rectangle
    mov eax, [bm_theme_background]
    mov ebx, NOVA_UI_PANEL_X + 1
    mov ecx, NOVA_UI_PANEL_Y + 1
    mov edx, NOVA_UI_PANEL_WIDTH - 2
    mov esi, NOVA_UI_PANEL_HEIGHT - 2
    call bm_fill_rounded_rectangle

    ; Aktuelle Auswahl
    movzx eax, byte [bm_selection]
    imul eax, NOVA_UI_MENU_ITEM_STRIDE
    add eax, NOVA_UI_MENU_Y
    mov ecx, eax
    mov eax, [bm_theme_panel]
    mov ebx, NOVA_UI_CONTENT_X
    mov edx, NOVA_UI_CONTENT_RIGHT - NOVA_UI_CONTENT_X
    mov esi, NOVA_UI_MENU_ITEM_HEIGHT
    call bm_fill_rounded_rectangle

    ; Der Fokus wird zusätzlich zur Flächenfarbe durch einen cyanfarbenen
    ; Marker dargestellt. Damit hängt die Auswahl nicht ausschließlich von
    ; einer Farbe beziehungsweise einem schwachen Kontrastunterschied ab.
    mov eax, BM_COLOR_CYAN
    mov ebx, NOVA_UI_CONTENT_X
    movzx ecx, byte [bm_selection]
    imul ecx, NOVA_UI_MENU_ITEM_STRIDE
    add ecx, NOVA_UI_MENU_Y + 14
    mov edx, NOVA_UI_SPACE_M
    mov esi, 34
    call bm_fill_rounded_rectangle

    ; Beschriftung, Symbol und Aktion stammen aus derselben Eintragstabelle.
    xor edi, edi
.draw_entry:
    cmp edi, BM_MENU_ENTRY_COUNT
    jae .entries_done
    mov eax, edi
    imul eax, BM_ENTRY_SIZE
    mov esi, [bm_menu_entries + eax + BM_ENTRY_LABEL]
    mov ebx, NOVA_UI_MENU_TEXT_X
    mov ecx, edi
    imul ecx, NOVA_UI_MENU_ITEM_STRIDE
    add ecx, NOVA_UI_MENU_Y + 21
    mov edx, BM_COLOR_WHITE
    movzx eax, byte [bm_selection]
    cmp edi, eax
    jne .entry_text_color_ready
    mov edx, BM_COLOR_CYAN
.entry_text_color_ready:
    mov ebp, 2
    push edi
    call bm_draw_text
    pop edi

    mov eax, edi
    imul eax, BM_ENTRY_SIZE
    movzx edi, byte [bm_menu_entries + eax + BM_ENTRY_ICON]
    mov eax, BM_COLOR_WHITE
    mov edx, [bm_draw_entry_index]
    movzx ebp, byte [bm_selection]
    cmp edx, ebp
    jne .entry_icon_color_ready
    mov eax, BM_COLOR_CYAN
.entry_icon_color_ready:
    mov ebx, NOVA_UI_MENU_ICON_X
    mov ecx, [bm_draw_entry_index]
    imul ecx, NOVA_UI_MENU_ITEM_STRIDE
    add ecx, NOVA_UI_MENU_Y + 24
    call bm_draw_menu_icon
    inc dword [bm_draw_entry_index]
    mov edi, [bm_draw_entry_index]
    jmp .draw_entry
.entries_done:
    mov dword [bm_draw_entry_index], 0

    ; Der Standard-Boot-Eintrag wird unabhängig vom Fokus textlich markiert.
    mov esi, bm_text_default_badge
    mov ebx, 640
    mov ecx, 105
    mov edx, BM_COLOR_CYAN
    mov ebp, 1
    call bm_draw_text

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
    mov esi, bm_page_installation
    mov edi, 1
    call bm_draw_page_base
    mov esi, bm_text_unavailable_title
    mov ebx, 228
    mov ecx, 154
    mov edx, BM_COLOR_WARNING
    mov ebp, 2
    call bm_draw_text
    mov esi, bm_text_unavailable
    mov ebx, 228
    mov ecx, 202
    mov edx, [bm_theme_secondary]
    mov ebp, 1
    call bm_draw_text
    mov esi, bm_text_unavailable_hint
    mov ebx, 228
    mov ecx, 232
    mov edx, BM_COLOR_WARNING
    mov ebp, 1
    call bm_draw_text
    ret

bm_clear_status_line:
    mov eax, [bm_theme_background]
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
    mov edx, [bm_theme_secondary]
    mov ebp, 2
    call bm_draw_text

    ; Numerische Restzeit plus sichtbarer, deterministischer Fortschritt.
    mov eax, [bm_theme_panel]
    mov ebx, 520
    mov ecx, 558
    mov edx, 240
    mov esi, 10
    call bm_fill_rounded_rectangle
    movzx edx, byte [bm_countdown_digit]
    sub edx, '0'
    imul edx, 48
    test edx, edx
    jz .done
    mov eax, BM_COLOR_CYAN
    mov ebx, 520
    mov ecx, 558
    mov esi, 10
    call bm_fill_rounded_rectangle
.done:
    ret

boot_manager_draw_diagnostics:
    mov esi, bm_page_diagnostics
    mov edi, 3
    call bm_draw_page_base
    mov esi, bm_diag_heading
    mov ebx, 228
    mov ecx, 144
    mov edx, BM_COLOR_SUCCESS
    mov ebp, 2
    call bm_draw_text
    mov esi, bm_diag_nbhp
    mov ebx, 228
    mov ecx, 190
    mov edx, BM_COLOR_WHITE
    mov ebp, 1
    call bm_draw_text
    mov esi, bm_diag_memory
    mov ecx, 222
    call bm_draw_text
    mov esi, bm_diag_graphics
    mov ecx, 254
    call bm_draw_text
    mov esi, bm_diag_kernel
    mov ecx, 286
    call bm_draw_text
    mov esi, bm_diag_security
    mov ecx, 334
    mov edx, BM_COLOR_WARNING
    call bm_draw_text
    mov esi, bm_text_diagnostics
    mov ebx, 228
    mov ecx, 392
    mov edx, [bm_theme_secondary]
    mov ebp, 1
    call bm_draw_text
    ret

boot_manager_draw_recovery_notice:
    mov esi, bm_page_recovery
    mov edi, 4
    call bm_draw_page_base
    mov esi, bm_recovery_heading
    mov ebx, 228
    mov ecx, 140
    mov edx, BM_COLOR_WARNING
    mov ebp, 2
    call bm_draw_text
    mov ecx, 184
    mov edi, 4
    call bm_draw_subselection_surface
    mov esi, bm_recovery_action_auto
    mov ebx, 250
    mov ecx, 198
    mov edx, BM_COLOR_WHITE
    mov ebp, 1
    call bm_draw_text
    mov esi, bm_recovery_action_snapshot
    mov ecx, 246
    call bm_draw_text
    mov esi, bm_recovery_action_memory
    mov ecx, 294
    call bm_draw_text
    mov esi, bm_recovery_action_disk
    mov ecx, 342
    call bm_draw_text
    mov esi, bm_recovery_status
    mov ebx, 228
    mov ecx, 410
    mov edx, [bm_theme_secondary]
    call bm_draw_text
    ret

boot_manager_draw_settings:
    mov esi, bm_page_settings
    mov edi, 2
    call bm_draw_page_base
    mov esi, bm_settings_heading
    mov ebx, 228
    mov ecx, 144
    mov edx, BM_COLOR_CYAN
    mov ebp, 2
    call bm_draw_text
    mov esi, bm_settings_default
    mov ebx, 228
    mov ecx, 202
    mov edx, BM_COLOR_WHITE
    mov ebp, 1
    call bm_draw_text
    mov esi, bm_settings_countdown
    mov ecx, 244
    call bm_draw_text
    mov esi, bm_settings_theme_prefix
    mov ecx, 286
    call bm_draw_text
    cmp byte [bm_high_contrast], 0
    jne .high_contrast
    mov esi, bm_settings_theme_dark
    jmp .theme_value
.high_contrast:
    mov esi, bm_settings_theme_high
.theme_value:
    mov ebx, 452
    call bm_draw_text
    mov esi, bm_settings_input
    mov ecx, 328
    call bm_draw_text
    mov esi, bm_settings_note
    mov ecx, 390
    mov edx, [bm_theme_secondary]
    mov ebp, 1
    mov edi, NOVA_UI_CONTENT_RIGHT
    call bm_draw_text_wrapped
    ret

boot_manager_draw_help:
    mov esi, bm_page_help
    mov edi, 3
    call bm_draw_page_base
    mov esi, bm_help_heading
    mov ebx, 228
    mov ecx, 144
    mov edx, BM_COLOR_CYAN
    mov ebp, 2
    call bm_draw_text
    mov esi, bm_help_navigation
    mov ecx, 202
    mov edx, BM_COLOR_WHITE
    mov ebp, 1
    call bm_draw_text
    mov esi, bm_help_actions
    mov ecx, 244
    call bm_draw_text
    mov esi, bm_help_shortcuts
    mov ecx, 286
    call bm_draw_text
    mov esi, bm_help_theme
    mov ecx, 328
    call bm_draw_text
    mov esi, bm_help_escape
    mov ecx, 390
    mov edx, [bm_theme_secondary]
    call bm_draw_text
    ret

boot_manager_draw_details:
    mov esi, bm_page_details
    xor edi, edi
    call bm_draw_page_base
    mov esi, bm_details_heading
    mov ebx, 228
    mov ecx, 144
    mov edx, BM_COLOR_CYAN
    mov ebp, 2
    call bm_draw_text
    mov esi, bm_details_version
    mov ecx, 194
    mov edx, BM_COLOR_WHITE
    mov ebp, 1
    call bm_draw_text
    mov esi, bm_details_kernel
    mov ecx, 226
    call bm_draw_text
    mov esi, bm_details_arch
    mov ecx, 258
    call bm_draw_text
    mov esi, bm_details_firmware
    mov ecx, 290
    call bm_draw_text
    mov esi, bm_details_integrity
    mov ecx, 338
    mov edx, BM_COLOR_SUCCESS
    call bm_draw_text
    mov esi, bm_details_signature
    mov ecx, 370
    mov edx, BM_COLOR_WARNING
    call bm_draw_text
    mov esi, bm_details_actions
    mov ecx, 424
    mov edx, [bm_theme_secondary]
    call bm_draw_text
    ret

boot_manager_draw_advanced:
    mov esi, bm_page_advanced
    xor edi, edi
    call bm_draw_page_base
    mov esi, bm_event_advanced_base
    call bm_debug_write_string
    mov esi, bm_advanced_heading
    mov ebx, 228
    mov ecx, 140
    mov edx, BM_COLOR_CYAN
    mov ebp, 2
    call bm_draw_text
    mov ecx, 184
    mov edi, 5
    call bm_draw_subselection_surface
    mov esi, bm_event_advanced_list
    call bm_debug_write_string
    mov esi, bm_advanced_normal
    mov ebx, 250
    mov ecx, 198
    mov edx, BM_COLOR_WHITE
    mov ebp, 1
    call bm_draw_text
    mov esi, bm_advanced_recovery
    mov ecx, 246
    call bm_draw_text
    mov esi, bm_advanced_safe
    mov ecx, 294
    call bm_draw_text
    mov esi, bm_advanced_network
    mov ecx, 342
    call bm_draw_text
    mov esi, bm_advanced_logging
    mov ecx, 390
    call bm_draw_text
    mov esi, bm_event_advanced_ready
    call bm_debug_write_string
    ret

boot_manager_draw_power_dialog:
    mov esi, bm_page_power
    mov edi, 5
    call bm_draw_page_base
    mov esi, bm_power_heading
    mov ebx, 228
    mov ecx, 140
    mov edx, BM_COLOR_CYAN
    mov ebp, 2
    call bm_draw_text
    mov ecx, 184
    mov edi, 4
    call bm_draw_subselection_surface
    mov esi, bm_power_off
    mov ebx, 250
    mov ecx, 198
    mov edx, BM_COLOR_WHITE
    mov ebp, 1
    call bm_draw_text
    mov esi, bm_power_restart
    mov ecx, 246
    call bm_draw_text
    mov esi, bm_power_recovery
    mov ecx, 294
    call bm_draw_text
    mov esi, bm_power_firmware
    mov ecx, 342
    call bm_draw_text
    mov esi, bm_power_note
    mov ecx, 410
    mov edx, [bm_theme_secondary]
    call bm_draw_text
    ret

boot_manager_draw_self_healing:
    mov esi, bm_page_self_healing
    mov edi, 4
    call bm_draw_page_base
    mov esi, bm_heal_heading
    mov ebx, 228
    mov ecx, 144
    mov edx, BM_COLOR_CYAN
    mov ebp, 2
    call bm_draw_text
    mov esi, bm_heal_integrity
    mov ecx, 202
    mov edx, BM_COLOR_SUCCESS
    mov ebp, 1
    call bm_draw_text
    mov esi, bm_heal_kernel
    mov ecx, 238
    call bm_draw_text
    mov esi, bm_heal_bootloader
    mov ecx, 274
    call bm_draw_text
    mov esi, bm_heal_filesystem
    mov ecx, 310
    mov edx, BM_COLOR_WARNING
    call bm_draw_text
    mov esi, bm_heal_note
    mov ecx, 374
    mov edx, [bm_theme_secondary]
    mov edi, NOVA_UI_CONTENT_RIGHT
    call bm_draw_text_wrapped
    ret

boot_manager_draw_snapshots:
    mov esi, bm_page_snapshots
    mov edi, 4
    call bm_draw_page_base
    mov esi, bm_snap_heading
    mov ebx, 228
    mov ecx, 144
    mov edx, BM_COLOR_WARNING
    mov ebp, 2
    call bm_draw_text
    mov esi, bm_snap_count
    mov ecx, 202
    mov edx, BM_COLOR_WHITE
    mov ebp, 1
    call bm_draw_text
    mov esi, bm_snap_reason
    mov ecx, 250
    mov edx, [bm_theme_secondary]
    mov edi, NOVA_UI_CONTENT_RIGHT
    call bm_draw_text_wrapped
    ret

boot_manager_draw_memory_test:
    mov esi, bm_page_memory
    mov edi, 3
    call bm_draw_page_base
    mov esi, bm_memory_heading
    mov ebx, 228
    mov ecx, 144
    mov edx, BM_COLOR_CYAN
    mov ebp, 2
    call bm_draw_text
    mov esi, bm_memory_map
    mov ecx, 202
    mov edx, BM_COLOR_WHITE
    mov ebp, 1
    call bm_draw_text
    mov esi, bm_memory_algorithm
    mov ecx, 238
    call bm_draw_text
    cmp byte [bm_memory_result], 1
    je .passed
    cmp byte [bm_memory_result], 2
    je .failed
    mov esi, bm_memory_ready
    mov edx, [bm_theme_secondary]
    jmp .result
.passed:
    mov esi, bm_memory_passed
    mov edx, BM_COLOR_SUCCESS
    jmp .result
.failed:
    mov esi, bm_memory_failed
    mov edx, BM_COLOR_ERROR
.result:
    mov ecx, 304
    call bm_draw_text
    mov eax, [bm_theme_panel]
    mov ebx, 228
    mov ecx, 354
    mov edx, 480
    mov esi, 12
    call bm_fill_rounded_rectangle
    cmp byte [bm_memory_result], 0
    je .hint
    mov eax, BM_COLOR_CYAN
    mov edx, 480
    call bm_fill_rounded_rectangle
.hint:
    mov esi, bm_memory_hint
    mov ebx, 228
    mov ecx, 402
    mov edx, [bm_theme_secondary]
    mov ebp, 1
    call bm_draw_text
    ret

boot_manager_draw_disk_tools:
    mov esi, bm_page_disk
    mov edi, 3
    call bm_draw_page_base
    mov esi, bm_disk_heading
    mov ebx, 228
    mov ecx, 144
    mov edx, BM_COLOR_CYAN
    mov ebp, 2
    call bm_draw_text
    mov esi, bm_disk_device
    mov ecx, 202
    mov edx, BM_COLOR_WHITE
    mov ebp, 1
    call bm_draw_text
    mov esi, bm_disk_layout
    mov ecx, 238
    call bm_draw_text
    mov esi, bm_disk_health
    mov ecx, 274
    mov edx, BM_COLOR_SUCCESS
    call bm_draw_text
    mov esi, bm_disk_note
    mov ecx, 338
    mov edx, [bm_theme_secondary]
    mov edi, NOVA_UI_CONTENT_RIGHT
    call bm_draw_text_wrapped
    ret

boot_manager_draw_encryption:
    mov esi, bm_page_encryption
    mov edi, 2
    call bm_draw_page_base
    mov esi, bm_encryption_heading
    mov ebx, 228
    mov ecx, 144
    mov edx, BM_COLOR_CYAN
    mov ebp, 2
    call bm_draw_text
    mov esi, bm_encryption_volume
    mov ecx, 210
    mov edx, BM_COLOR_WHITE
    mov ebp, 1
    call bm_draw_text
    mov esi, bm_encryption_state
    mov ecx, 250
    mov edx, BM_COLOR_SUCCESS
    call bm_draw_text
    mov esi, bm_encryption_note
    mov ecx, 320
    mov edx, [bm_theme_secondary]
    mov edi, NOVA_UI_CONTENT_RIGHT
    call bm_draw_text_wrapped
    ret

boot_manager_draw_network:
    mov esi, bm_page_network
    mov edi, 3
    call bm_draw_page_base
    mov esi, bm_network_heading
    mov ebx, 228
    mov ecx, 144
    mov edx, BM_COLOR_WARNING
    mov ebp, 2
    call bm_draw_text
    mov esi, bm_network_adapter
    mov ecx, 210
    mov edx, BM_COLOR_WHITE
    mov ebp, 1
    call bm_draw_text
    mov esi, bm_network_server
    mov ecx, 250
    call bm_draw_text
    mov esi, bm_network_note
    mov ecx, 320
    mov edx, [bm_theme_secondary]
    mov edi, NOVA_UI_CONTENT_RIGHT
    call bm_draw_text_wrapped
    ret

boot_manager_draw_firmware:
    mov esi, bm_page_firmware
    mov edi, 2
    call bm_draw_page_base
    mov esi, bm_firmware_heading
    mov ebx, 228
    mov ecx, 144
    mov edx, BM_COLOR_CYAN
    mov ebp, 2
    call bm_draw_text
    mov esi, bm_firmware_mode
    mov ecx, 210
    mov edx, BM_COLOR_WHITE
    mov ebp, 1
    call bm_draw_text
    mov esi, bm_firmware_secure
    mov ecx, 250
    mov edx, BM_COLOR_WARNING
    call bm_draw_text
    mov esi, bm_firmware_note
    mov ecx, 320
    mov edx, [bm_theme_secondary]
    mov edi, NOVA_UI_CONTENT_RIGHT
    call bm_draw_text_wrapped
    ret

bm_run_memory_quick_test:
    push eax
    mov eax, [bm_memory_probe]
    push eax
    mov dword [bm_memory_probe], 0xAA55AA55
    cmp dword [bm_memory_probe], 0xAA55AA55
    jne .failed
    mov dword [bm_memory_probe], 0x55AA55AA
    cmp dword [bm_memory_probe], 0x55AA55AA
    jne .failed
    mov byte [bm_memory_result], 1
    mov esi, bm_event_memory_passed
    jmp .restore
.failed:
    mov byte [bm_memory_result], 2
    mov esi, bm_event_memory_failed
.restore:
    pop eax
    mov [bm_memory_probe], eax
    call bm_debug_write_string
    pop eax
    ret

; ECX=Start-y, EDI=Anzahl. Gemeinsame Focus/Selected-Flaeche fuer Listen.
bm_draw_subselection_surface:
    pushad
    movzx eax, byte [bm_subselection]
    cmp eax, edi
    jb .valid
    xor eax, eax
.valid:
    imul eax, 48
    add ecx, eax
    mov eax, [bm_theme_panel]
    mov ebx, 228
    mov edx, 480
    mov esi, 42
    call bm_fill_rounded_rectangle
    mov eax, BM_COLOR_CYAN
    mov ebx, 228
    mov edx, NOVA_UI_SPACE_M
    mov esi, 42
    call bm_fill_rounded_rectangle
    popad
    ret

; ESI=Seitentitel, EDI=Menüsymbol. Zeichnet eine gemeinsame, modal wirkende
; Seitenbasis über den Hauptbildschirm und erhält dadurch Marke, Rahmen und
; Design-Tokens auf allen Ansichten konsistent.
bm_draw_page_base:
    push esi
    push edi
    call bm_draw_page_shell
    mov eax, [bm_theme_background]
    mov ebx, 190
    mov ecx, 61
    mov edx, 555
    mov esi, 460
    call bm_fill_rectangle
    pop edi
    pop esi
    mov ebx, 228
    mov ecx, 84
    mov edx, BM_COLOR_WHITE
    mov ebp, 2
    call bm_draw_text
    mov eax, BM_COLOR_CYAN
    mov ebx, 204
    mov ecx, 87
    call bm_draw_menu_icon
    mov eax, BM_COLOR_BLUE
    mov ebx, 204
    mov ecx, 122
    mov edx, 523
    mov esi, 2
    call bm_fill_rectangle
    mov esi, bm_page_back_hint
    cmp byte [bm_view], BM_VIEW_SELF_HEALING
    je .recovery_hint
    cmp byte [bm_view], BM_VIEW_SNAPSHOTS
    je .recovery_hint
    cmp byte [bm_view], BM_VIEW_MEMORY_TEST
    je .recovery_hint
    cmp byte [bm_view], BM_VIEW_DISK_TOOLS
    jne .hint_ready
.recovery_hint:
    mov esi, bm_page_back_recovery_hint
.hint_ready:
    mov ebx, 204
    mov ecx, 488
    mov edx, [bm_theme_secondary]
    mov ebp, 1
    call bm_draw_text
    ret

; Gemeinsame statische Shell fuer Unterseiten. Sie zeichnet nur Hintergrund,
; Marke, Kopfzeile und Panel statt zuerst das komplette Hauptmenue aufzubauen.
bm_draw_page_shell:
    mov eax, [bm_theme_background]
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 16]
    mov esi, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 20]
    call bm_fill_rectangle
    mov eax, BM_COLOR_BLUE
    mov ebx, 102
    xor ecx, ecx
    mov edx, 628
    mov esi, 13
    call bm_fill_rounded_rectangle
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
    mov eax, BM_COLOR_BLUE
    mov ebx, NOVA_UI_PANEL_X
    mov ecx, NOVA_UI_PANEL_Y
    mov edx, NOVA_UI_PANEL_WIDTH
    mov esi, NOVA_UI_PANEL_HEIGHT
    call bm_fill_rounded_rectangle
    mov eax, [bm_theme_background]
    mov ebx, NOVA_UI_PANEL_X + 1
    mov ecx, NOVA_UI_PANEL_Y + 1
    mov edx, NOVA_UI_PANEL_WIDTH - 2
    mov esi, NOVA_UI_PANEL_HEIGHT - 2
    call bm_fill_rounded_rectangle
    ret

bm_redraw_current_view:
    cmp byte [bm_view], BM_VIEW_SETTINGS
    je .settings
    cmp byte [bm_view], BM_VIEW_DIAGNOSTICS
    je .diagnostics
    cmp byte [bm_view], BM_VIEW_RECOVERY
    je .recovery
    cmp byte [bm_view], BM_VIEW_UNAVAILABLE
    je .unavailable
    cmp byte [bm_view], BM_VIEW_HELP
    je .help
    cmp byte [bm_view], BM_VIEW_DETAILS
    je .details
    cmp byte [bm_view], BM_VIEW_ADVANCED
    je .advanced
    cmp byte [bm_view], BM_VIEW_POWER
    je .power
    cmp byte [bm_view], BM_VIEW_SELF_HEALING
    je .self_healing
    cmp byte [bm_view], BM_VIEW_SNAPSHOTS
    je .snapshots
    cmp byte [bm_view], BM_VIEW_MEMORY_TEST
    je .memory
    cmp byte [bm_view], BM_VIEW_DISK_TOOLS
    je .disk
    cmp byte [bm_view], BM_VIEW_ENCRYPTION
    je .encryption
    cmp byte [bm_view], BM_VIEW_NETWORK
    je .network
    cmp byte [bm_view], BM_VIEW_FIRMWARE
    je .firmware
    call boot_manager_draw
    jmp .cursor
.settings:
    call boot_manager_draw_settings
    jmp .cursor
.diagnostics:
    call boot_manager_draw_diagnostics
    jmp .cursor
.recovery:
    call boot_manager_draw_recovery_notice
    jmp .cursor
.unavailable:
    call boot_manager_draw_unavailable
    jmp .cursor
.help:
    call boot_manager_draw_help
    jmp .cursor
.details:
    call boot_manager_draw_details
    jmp .cursor
.advanced:
    call boot_manager_draw_advanced
    jmp .cursor
.power:
    call boot_manager_draw_power_dialog
    jmp .cursor
.self_healing:
    call boot_manager_draw_self_healing
    jmp .cursor
.snapshots:
    call boot_manager_draw_snapshots
    jmp .cursor
.memory:
    call boot_manager_draw_memory_test
    jmp .cursor
.disk:
    call boot_manager_draw_disk_tools
    jmp .cursor
.encryption:
    call boot_manager_draw_encryption
    jmp .cursor
.network:
    call boot_manager_draw_network
    jmp .cursor
.firmware:
    call boot_manager_draw_firmware
.cursor:
    call bm_draw_mouse_cursor
    ret

; Kleiner, geglättet wirkender Nova-Cursor aus deterministischen Primitiven.
; Er benötigt weder Bitmapressource noch dynamischen Speicher.
bm_draw_mouse_cursor:
    cmp byte [bm_mouse_available], 0
    je .done
    xor ebp, ebp
.arrow_row:
    cmp ebp, 12
    jae .stem
    mov eax, BM_COLOR_WHITE
    mov ebx, [bm_mouse_x]
    mov ecx, [bm_mouse_y]
    add ecx, ebp
    mov edx, ebp
    shr edx, 1
    add edx, 2
    mov esi, 1
    call bm_fill_rectangle
    inc ebp
    jmp .arrow_row
.stem:
    mov eax, BM_COLOR_CYAN
    mov ebx, [bm_mouse_x]
    add ebx, 4
    mov ecx, [bm_mouse_y]
    add ecx, 9
    mov edx, 3
    mov esi, 7
    call bm_fill_rectangle
.done:
    ret

bm_toggle_contrast_theme:
    xor byte [bm_high_contrast], 1
    cmp byte [bm_high_contrast], 0
    je .dark
    mov dword [bm_theme_background], 0x00000000
    mov dword [bm_theme_panel], 0x00252525
    mov dword [bm_theme_secondary], BM_COLOR_WHITE
    mov esi, bm_event_high_contrast
    call bm_debug_write_string
    ret
.dark:
    mov dword [bm_theme_background], BM_COLOR_BACKGROUND
    mov dword [bm_theme_panel], BM_COLOR_PANEL
    mov dword [bm_theme_secondary], BM_COLOR_SECONDARY
    mov esi, bm_event_dark_theme
    call bm_debug_write_string
    ret

boot_manager_draw_starting:
    mov eax, [bm_theme_panel]
    mov ebx, 64
    mov ecx, 448
    mov edx, 672
    mov esi, 76
    call bm_fill_rounded_rectangle
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
    mov byte [bm_text_wrap], 0
    jmp bm_draw_text_begin

; ESI=Text, EBX=x, ECX=y, EDX=Farbe, EBP=Skalierung, EDI=rechte DLU-Grenze.
; Der frühe Bootpfad verwendet deterministischen Zeichenumbruch ohne Heap.
bm_draw_text_wrapped:
    mov byte [bm_text_wrap], 1
    mov [bm_text_max_x], edi
bm_draw_text_begin:
    mov [bm_text_pointer], esi
    mov [bm_text_x], ebx
    mov [bm_text_origin_x], ebx
    mov [bm_text_y], ecx
    mov [bm_text_color], edx
    mov [bm_text_scale], ebp
.next:
.load:
    mov esi, [bm_text_pointer]
    lodsb
    mov [bm_text_pointer], esi
    test al, al
    jz .done
    cmp al, ' '
    je .space
    mov byte [bm_character_advance], 8
    call bm_draw_character
.advance:
    movzx eax, byte [bm_character_advance]
    add eax, [bm_text_scale]
    add [bm_text_x], eax
    jmp .next
.space:
    cmp byte [bm_text_wrap], 0
    je .plain_space
    mov esi, [bm_text_pointer]
    call bm_measure_word
    add eax, [bm_text_x]
    cmp eax, [bm_text_max_x]
    jae .new_line
.plain_space:
    mov byte [bm_character_advance], 5
    jmp .advance
.new_line:
    mov eax, [bm_text_origin_x]
    mov [bm_text_x], eax
    mov eax, BM_FONT_HEIGHT + NOVA_UI_SPACE_S
    imul eax, [bm_text_scale]
    add [bm_text_y], eax
    jmp .next
.done:
    ret

; ESI zeigt auf das nächste Wort. Rückgabe EAX=Breite, Eingabezeiger bleibt
; unverändert. Damit kann der Renderer vor dem Zeichnen wortweise umbrechen.
bm_measure_word:
    push esi
    push ebx
    push ecx
    push edi
    xor ebx, ebx
.next:
    lodsb
    test al, al
    jz .done
    cmp al, ' '
    je .done
    mov edi, bm_font_characters
    xor ecx, ecx
.find:
    mov dl, [edi + ecx]
    test dl, dl
    jz .fallback
    cmp dl, al
    je .found
    inc ecx
    jmp .find
.found:
    movzx eax, byte [bm_font_widths + ecx]
    jmp .add
.fallback:
    mov eax, 8
.add:
    add eax, [bm_text_scale]
    add ebx, eax
    jmp .next
.done:
    mov eax, ebx
    pop edi
    pop ecx
    pop ebx
    pop esi
    ret

; ESI=Text, EBP=Skalierung. Rückgabe: EAX=Breite in DLU, EDX=Zeilenhöhe.
bm_measure_text:
    push ebx
    push ecx
    push edi
    xor ebx, ebx
.character:
    lodsb
    test al, al
    jz .measured
    cmp al, ' '
    je .space
    mov edi, bm_font_characters
    xor ecx, ecx
.find:
    mov dl, [edi + ecx]
    test dl, dl
    jz .fallback
    cmp dl, al
    je .found
    inc ecx
    jmp .find
.found:
    movzx eax, byte [bm_font_widths + ecx]
    jmp .add
.space:
    mov eax, 5
    jmp .add
.fallback:
    mov eax, 8
.add:
    add eax, ebp
    add ebx, eax
    jmp .character
.measured:
    mov eax, ebx
    mov edx, BM_FONT_HEIGHT
    imul edx, ebp
    pop edi
    pop ecx
    pop ebx
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
    cmp edi, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 20]
    jae .next_column
    imul edi, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 12]
    add edi, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 8]
    mov edx, [bm_text_x]
    add edx, ecx
    cmp edx, [BOOT_INFO_ADDRESS + BIB_GRAPHICS_OFFSET + 16]
    jae .next_column
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
bm_text_origin_x: dd 0
bm_text_max_x:   dd NOVA_UI_CONTENT_RIGHT
bm_text_color:   dd 0
bm_text_scale:   dd 1
bm_text_wrap:    db 0
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
bm_view:         db BM_VIEW_MAIN
bm_subselection: db 0
bm_memory_result: db 0
bm_mouse_available: db 0
bm_mouse_phase:     db 0
bm_mouse_buttons:   db 0
bm_mouse_packet:    times 3 db 0
bm_high_contrast:   db 0
align 4
bm_draw_entry_index: dd 0
bm_mouse_x:       dd 400
bm_mouse_y:       dd 300
bm_theme_background: dd BM_COLOR_BACKGROUND
bm_theme_panel:      dd BM_COLOR_PANEL
bm_theme_secondary:  dd BM_COLOR_SECONDARY
bm_memory_probe:      dd 0x4E4F5641

bm_text_brand:       db "NovaOS",0
bm_text_screen_title: db "Startoptionen",0
bm_text_default_badge: db "STANDARD",0
bm_text_title:       db "BOOT MANAGER",0
bm_text_os:          db "NOVA OS",0
bm_text_kernel:      db "STANDARDKERNEL | NKI V1 | X86-32",0
bm_text_verified:    db "STARTBEREIT",0
bm_text_security:    db "SICHERHEIT NOCH NICHT VERIFIZIERT",0
bm_text_enter:       db "ENTER  SYSTEM STARTEN",0
bm_text_actions:     db "D  DIAGNOSE     R  RECOVERY-STATUS",0
bm_text_diagnostics: db "CRC32 OK | NBHP/BIB 1.0 | MEMORY MAP OK | VBE OK",0
bm_text_recovery:    db "Recovery-Modul ist noch nicht verf",0x81,"gbar.",0
bm_text_starting:    db "NOVA OS WIRD GESTARTET",0
bm_page_settings:    db "Einstellungen",0
bm_page_diagnostics: db "Systemdiagnose",0
bm_page_recovery:    db "Wiederherstellung",0
bm_page_installation: db "Installation",0
bm_page_help:        db "Hilfe und Bedienung",0
bm_page_details:     db "Details zum Starteintrag",0
bm_page_advanced:    db "Erweiterte Startoptionen",0
bm_page_power:       db "Ausschalten und Neustarten",0
bm_page_self_healing: db "NovaOS Self-Healing",0
bm_page_snapshots:   db "Snapshot-Auswahl",0
bm_page_memory:      db "Speichertest",0
bm_page_disk:        db "Datentr",0x84,"ger und Partitionen",0
bm_page_encryption:  db "Verschl",0x81,"sselung und Entsperren",0
bm_page_network:     db "Netzwerk-Boot",0
bm_page_firmware:    db "Firmware-Einstellungen",0
bm_page_back_hint:   db "ESC  Zur",0x81,"ck zum Hauptmen",0x81,0
bm_page_back_recovery_hint: db "ESC  Zur",0x81,"ck zum Recovery-Men",0x81,0
bm_help_heading:     db "Bootmanager bedienen",0
bm_help_navigation:  db "Pfeiltasten / Maus       Auswahl bewegen",0
bm_help_actions:     db "ENTER / Mausklick        Auswahl ausf",0x81,"hren",0
bm_help_shortcuts:   db "D Diagnose   R Recovery   S Einstellungen",0
bm_help_theme:       db "H Darstellung mit hohem Kontrast umschalten",0
bm_help_escape:      db "ESC oder R",0x81,"cktaste bringt dich zur",0x81,"ck.",0
bm_details_heading:  db "NovaOS",0
bm_details_version:  db "Version                  Entwicklungsversion",0
bm_details_kernel:   db "Kernel                   NKI v1 (ELF kompatibel)",0
bm_details_arch:     db "Architektur              x86-32",0
bm_details_firmware: db "Firmwaremodus            BIOS / VBE",0
bm_details_integrity: db "Status                   Bootf",0x84,"hig, Integrit",0x84,"t gepr",0x81,"ft",0
bm_details_signature: db "Signatur                 Noch nicht verifiziert",0
bm_details_actions:  db "ENTER Starten   F3 Erweiterte Optionen   ESC Schlie",0xE1,"en",0
bm_advanced_heading: db "NovaOS Startmodus ausw",0x84,"hlen",0
bm_advanced_normal:  db "Normal starten",0
bm_advanced_recovery: db "Recovery starten",0
bm_advanced_safe:    db "Abgesicherter Modus",0
bm_advanced_network: db "Abgesicherter Modus mit Netzwerk",0
bm_advanced_logging: db "Boot-Protokollierung aktivieren",0
bm_power_heading:    db "Was soll der Computer tun?",0
bm_power_off:        db "Herunterfahren",0
bm_power_restart:    db "Neu starten",0
bm_power_recovery:   db "Neu starten in Recovery",0
bm_power_firmware:   db "Neu starten in Firmware-Einstellungen",0
bm_power_note:       db "ENTER best",0x84,"tigt die Auswahl. ESC bricht ab.",0
bm_settings_heading: db "Bootmanager konfigurieren",0
bm_settings_default: db "Standard-Bootziel        NovaOS",0
bm_settings_countdown: db "Automatischer Start       5 Sekunden",0
bm_settings_theme_prefix: db "Darstellung",0
bm_settings_theme_dark:   db "Dunkles Nova-Theme",0
bm_settings_theme_high:   db "Hoher Kontrast",0
bm_settings_input:   db "Eingabe                   Tastatur aktiv",0
bm_settings_note:    db "Speichern ist verf",0x81,"gbar, sobald NovaFS im Bootmanager eingebunden ist.",0
bm_diag_heading:     db "Alle grundlegenden Pr",0x81,"fungen bestanden",0
bm_diag_nbhp:        db "NBHP / BIB               Version 1.0 validiert",0
bm_diag_memory:      db "Speicherkarte             Verf",0x81,"gbar",0
bm_diag_graphics:    db "Grafikausgabe             VBE-Framebuffer aktiv",0
bm_diag_kernel:      db "Kernelabbild              NKI / CRC32 g",0x81,"ltig",0
bm_diag_security:    db "Sicherheitsstatus         Signatur noch nicht verifiziert",0
bm_recovery_heading: db "NovaOS Wiederherstellung",0
bm_recovery_explanation: db "Diagnose und Reparatur von Startproblemen. Es werden keine Benutzerdaten ver",0x84,"ndert.",0
bm_recovery_action_auto: db "Automatische Reparatur    Noch nicht verf",0x81,"gbar",0
bm_recovery_action_snapshot: db "Snapshot wiederherstellen Noch nicht verf",0x81,"gbar",0
bm_recovery_action_memory: db "Speicher testen",0
bm_recovery_action_disk: db "Datentr",0x84,"ger und Partitionen pr",0x81,"fen",0
bm_recovery_status: db "NovaOS erkannt | Integrit",0x84,"t OK | Recovery v1",0
bm_heal_heading: db "Systemzustand und Reparatur",0
bm_heal_integrity: db "Integrit",0x84,"t              Gepr",0x81,"ft",0
bm_heal_kernel: db "Kernelabbild             G",0x81,"ltig",0
bm_heal_bootloader: db "Bootloader               Startbereit",0
bm_heal_filesystem: db "Dateisystem              Offline-Pr",0x81,"fung erforderlich",0
bm_heal_note: db "Schreibende Reparaturen bleiben gesperrt, bis NovaFS und ein validierter Recovery-Snapshot verf",0x81,"gbar sind.",0
bm_snap_heading: db "Keine Snapshots gefunden",0
bm_snap_count: db "Verf",0x81,"gbare Snapshots      0",0
bm_snap_reason: db "Die Snapshotliste wird aktiviert, sobald das NovaFS-Recovery-Modul einen gepr",0x81,"ften Snapshot-Katalog bereitstellt.",0
bm_memory_heading: db "Schneller Speichertest",0
bm_memory_map: db "Speicherkarte            E820 erkannt",0
bm_memory_algorithm: db "Pr",0x81,"fmuster               AA55 / 55AA",0
bm_memory_ready: db "Status                   Bereit",0
bm_memory_passed: db "Status                   Test ohne Fehler abgeschlossen",0
bm_memory_failed: db "Status                   Speicherfehler erkannt",0
bm_memory_hint: db "ENTER startet den sicheren Bootmanager-Schnelltest",0
bm_disk_heading: db "Erkannter Startdatentr",0x84,"ger",0
bm_disk_device: db "Ger",0x84,"t                    BIOS-Laufwerk 0x80",0
bm_disk_layout: db "Startlayout              Nova BIOS Image",0
bm_disk_health: db "Lesestatus               Bootsektoren lesbar",0
bm_disk_note: db "Schreibende Partitionsaktionen sind gesperrt. GPT/MBR- und SMART-Details ben",0x94,"tigen das noch nicht eingebundene Storage-Modul.",0
bm_encryption_heading: db "NovaOS Volume-Sicherheit",0
bm_encryption_volume: db "Startvolume              NovaOS System",0
bm_encryption_state: db "Verschl",0x81,"sselung           Nicht erforderlich",0
bm_encryption_note: db "Es wurde kein verschl",0x81,"sseltes Startvolume angefordert. Passwort-, TPM- und Recovery-Key-Eingabe bleiben deshalb sicher deaktiviert.",0
bm_network_heading: db "Kein Bootserver gefunden",0
bm_network_adapter: db "Netzwerkadapter          Nicht initialisiert",0
bm_network_server: db "PXE / HTTP Boot          Nicht verf",0x81,"gbar",0
bm_network_note: db "Der BIOS-Pfad besitzt aktuell keinen PXE-UNDI-Treiber. Netzwerk-Boot wird erst nach validierter Adapter- und DHCP-Initialisierung freigegeben.",0
bm_firmware_heading: db "Plattforminformationen",0
bm_firmware_mode: db "Firmwaremodus            Legacy BIOS",0
bm_firmware_secure: db "Secure Boot              Nicht verf",0x81,"gbar",0
bm_firmware_note: db "Legacy BIOS bietet keinen standardisierten Neustart in das Setup. Verwende beim Einschalten die Setup-Taste des Ger",0x84,"teherstellers.",0
bm_text_unavailable_title: db "Funktion noch nicht verf",0x81,"gbar",0
bm_text_unavailable_hint: db "Diese Ansicht ist vorbereitet, ben",0x94,"tigt aber ein Installationsmodul.",0
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
    db 2, BM_ACTION_SETTINGS
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
bm_event_settings:    db "BM:SETTINGS",10,0
bm_event_back:        db "BM:BACK",10,0
bm_event_help:        db "BM:HELP",10,0
bm_event_details:     db "BM:DETAILS",10,0
bm_event_advanced:    db "BM:ADVANCED",10,0
bm_event_advanced_base: db "BM:ADVANCED-BASE",10,0
bm_event_advanced_list: db "BM:ADVANCED-LIST",10,0
bm_event_advanced_ready: db "BM:ADVANCED-READY",10,0
bm_event_power_dialog: db "BM:POWER-DIALOG",10,0
bm_event_self_healing: db "BM:SELF-HEALING",10,0
bm_event_snapshots: db "BM:SNAPSHOTS",10,0
bm_event_memory_test: db "BM:MEMORY-TEST",10,0
bm_event_memory_passed: db "BM:MEMORY-PASSED",10,0
bm_event_memory_failed: db "BM:MEMORY-FAILED",10,0
bm_event_disk_tools: db "BM:DISK-TOOLS",10,0
bm_event_encryption: db "BM:ENCRYPTION",10,0
bm_event_network: db "BM:NETWORK",10,0
bm_event_firmware: db "BM:FIRMWARE",10,0
bm_event_high_contrast: db "BM:THEME-HIGH-CONTRAST",10,0
bm_event_dark_theme:    db "BM:THEME-DARK",10,0

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
vbe_best_rank:              db 0xFF
vbe_best_mode:              dw 0xFFFF
vbe_candidate_mode:         dw 0xFFFF
kernel_destination_offset: dw 0
kernel_destination_segment: dw 0
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
message_vbe_ready:
    db "BIOS:VBE-BACKEND-READY", 13, 10, 0
message_vbe_text_fallback:
    db "BIOS:VBE-TEXT-FALLBACK", 13, 10, 0
message_vbe_rank:
    db "BIOS:VBE-MODE-RANK-"
message_vbe_rank_digit:
    db "0", 13, 10, 0
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
