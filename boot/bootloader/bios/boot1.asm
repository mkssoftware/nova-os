; Nova Boot Manager - BIOS Stage 1
; Läuft als klassischer MBR bei 0000:7C00.

[org 0x7C00]
[bits 16]

%include "layout.inc"

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, STAGE1_STACK_TOP
    cld
    sti

    mov [boot_drive], dl
    mov si, message_start
    call print_string

    ; Primärpfad: EDD/LBA lädt den zusammenhängenden Stage-2-Bereich.
    mov ah, 0x41
    mov bx, 0x55AA
    mov dl, [boot_drive]
    int 0x13
    jc .load_stage2_chs
    cmp bx, 0xAA55
    jne .load_stage2_chs
    test cx, 1
    jz .load_stage2_chs

    mov byte [retry_count], DISK_RETRY_COUNT
.load_stage2_lba:
    mov si, stage2_dap
    mov ah, 0x42
    mov dl, [boot_drive]
    int 0x13
    jnc .loaded
    call reset_disk
    dec byte [retry_count]
    jnz .load_stage2_lba

    ; Fallback für klassische 1,44-MB-Disketten ohne EDD. Sektorweise
    ; lesen, damit Spur- und Kopfgrenzen korrekt überschritten werden.
.load_stage2_chs:
    xor ax, ax
    mov es, ax
    mov bx, STAGE2_LOAD_ADDRESS
    mov byte [stage2_remaining], STAGE2_SECTOR_COUNT
    mov byte [stage2_sector], STAGE2_FIRST_SECTOR
    mov byte [stage2_head], 0
    mov byte [stage2_cylinder], 0

.next_chs_sector:
    mov byte [retry_count], DISK_RETRY_COUNT
.load_chs_retry:
    mov ah, 0x02
    mov al, 1
    mov ch, [stage2_cylinder]
    mov cl, [stage2_sector]
    mov dh, [stage2_head]
    mov dl, [boot_drive]
    int 0x13
    jnc .chs_loaded
    call reset_disk
    dec byte [retry_count]
    jnz .load_chs_retry
    jmp .read_failed

.chs_loaded:
    add bx, 512
    inc byte [stage2_sector]
    cmp byte [stage2_sector], 18
    jbe .chs_advanced
    mov byte [stage2_sector], 1
    xor byte [stage2_head], 1
    cmp byte [stage2_head], 0
    jne .chs_advanced
    inc byte [stage2_cylinder]
.chs_advanced:
    dec byte [stage2_remaining]
    jnz .next_chs_sector

.loaded:
    mov dl, [boot_drive]
    jmp 0x0000:STAGE2_LOAD_ADDRESS

.read_failed:
    mov si, message_disk_error
    call print_string
    int 0x18

.halt:
    cli
    hlt
    jmp .halt

print_string:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    xor bh, bh
    mov bl, 0x07
    int 0x10
    jmp print_string
.done:
    ret

reset_disk:
    xor ax, ax
    mov dl, [boot_drive]
    int 0x13
    ret

boot_drive:       db 0
retry_count:      db 0
stage2_remaining: db 0
stage2_sector:    db 0
stage2_head:      db 0
stage2_cylinder:  db 0

align 4
stage2_dap:
    db 0x10, 0
    dw STAGE2_SECTOR_COUNT
    dw STAGE2_LOAD_ADDRESS, 0
    dq 1

message_start:
    db "Nova Stage 1", 13, 10, 0
message_disk_error:
    db "E1: Stage 2 konnte nicht geladen werden", 13, 10, 0

; Ein echter MBR reserviert Bytes 446..509 für vier Partitionseinträge.
times 446 - ($ - $$) db 0
times 64 db 0
dw 0xAA55
