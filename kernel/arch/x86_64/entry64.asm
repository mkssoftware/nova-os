[bits 64]
%include "layout.inc"
[org 0x00200000]

entry64:
    cli
    cmp eax, NOVA_X86_BOOT_MAGIC
    jne .failed
    cmp ebx, BOOT_INFO_ADDRESS
    jne .failed
    mov dx, 0x03F8 + 1
    xor al, al
    out dx, al
    mov dx, 0x03F8 + 3
    mov al, 0x80
    out dx, al
    mov dx, 0x03F8
    mov al, 1
    out dx, al
    mov dx, 0x03F8 + 1
    xor al, al
    out dx, al
    mov dx, 0x03F8 + 3
    mov al, 3
    out dx, al
    lea rsi, [rel message_ready]
.write:
    lodsb
    test al, al
    jz .halt
    mov dx, 0xE9
    out dx, al
    mov dx, 0x03F8
    out dx, al
    jmp .write
.failed:
    lea rsi, [rel message_failed]
    jmp .write
.halt:
    cli
    hlt
    jmp .halt

message_ready:  db "NOVA_ELF64_LONG_MODE_READY", 13, 10, 0
message_failed: db "NOVA_ELF64_HANDOFF_FAILED", 13, 10, 0
