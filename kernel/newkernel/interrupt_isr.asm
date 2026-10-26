; NovaOS C-Kernel - ISR-Trampoline (Port aus interrupt32.inc, nur die reinen
; Hardware-Einsprungpunkte; Dispatch-Logik lebt in interrupt.c als
; interrupt_dispatch_c). CODE_SEGMENT/DATA_SEGMENT entsprechen der in
; boot.asm aufgebauten flachen GDT (0x08 Code, 0x10 Data).
bits 32

%define CODE_SEG 0x08
%define DATA_SEG 0x10

extern interrupt_dispatch_c

global isr_unexpected
global irq0_stub
global irq1_stub
global syscall_stub
global exception_stub_table

section .text

isr_common:
    pushad
    push ds
    push es
    push fs
    push gs
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, [esp + 48]      ; normalisierter Vektor (liegt oberhalb der 48
                               ; Bytes aus pushad + 4 Segmentregistern)
    mov edx, esp
    push edx
    push eax
    call interrupt_dispatch_c
    add esp, 8
    mov esp, eax
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 8                ; Vektor + Fehlercode verwerfen
    iretd

%macro ISR_NO_ERROR 1
global isr_%1
isr_%1:
    push dword 0
    push dword %1
    jmp isr_common
%endmacro

%macro ISR_WITH_ERROR 1
global isr_%1
isr_%1:
    push dword %1
    jmp isr_common
%endmacro

ISR_NO_ERROR 0
ISR_NO_ERROR 1
ISR_NO_ERROR 2
ISR_NO_ERROR 3
ISR_NO_ERROR 4
ISR_NO_ERROR 5
ISR_NO_ERROR 6
ISR_NO_ERROR 7
ISR_WITH_ERROR 8
ISR_NO_ERROR 9
ISR_WITH_ERROR 10
ISR_WITH_ERROR 11
ISR_WITH_ERROR 12
ISR_WITH_ERROR 13
ISR_WITH_ERROR 14
ISR_NO_ERROR 15
ISR_NO_ERROR 16
ISR_WITH_ERROR 17
ISR_NO_ERROR 18
ISR_NO_ERROR 19
ISR_NO_ERROR 20
ISR_WITH_ERROR 21
ISR_NO_ERROR 22
ISR_NO_ERROR 23
ISR_NO_ERROR 24
ISR_NO_ERROR 25
ISR_NO_ERROR 26
ISR_NO_ERROR 27
ISR_NO_ERROR 28
ISR_WITH_ERROR 29
ISR_WITH_ERROR 30
ISR_NO_ERROR 31

irq0_stub:
    push dword 0
    push dword 32
    jmp isr_common
irq1_stub:
    push dword 0
    push dword 33
    jmp isr_common
syscall_stub:
    push dword 0
    push dword 0x80
    jmp isr_common
isr_unexpected:
    push dword 0
    push dword 255
    jmp isr_common

align 4
exception_stub_table:
%assign __vector 0
%rep 32
    dd isr_%+__vector
%assign __vector __vector + 1
%endrep
