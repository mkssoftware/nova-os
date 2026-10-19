; NovaOS C-Kernel - Boot-Entry (Objektformat, kein 'org').
; Uebernimmt denselben UEFI-Handoff wie der alte Kernel: CPU steht bereits in
; 32-Bit Protected Mode, EAX=Boot-Magic, EBX=BIB-Pointer (NPSPEC-KERNEL-0001).
bits 32

extern kmain

global _start
section .text
_start:
    cli
    cld
    xor ebp, ebp
    mov [boot_eax], eax
    mov [boot_ebx], ebx

    lgdt [gdt_descriptor]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov esp, stack_top

    call kmain

.hang:
    cli
    hlt
    jmp .hang

section .data
align 8
gdt_start:
    dq 0x0000000000000000      ; Null-Deskriptor
    dq 0x00CF9A000000FFFF      ; Code: Base=0, Limit=4G, 32-Bit
    dq 0x00CF92000000FFFF      ; Data: Base=0, Limit=4G
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

global boot_eax
global boot_ebx
boot_eax: dd 0
boot_ebx: dd 0

section .bss
align 16
stack_bottom:
    resb 65536
stack_top:
