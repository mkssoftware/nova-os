; NovaOS C-Kernel - AP-Trampoline (Port aus security32.inc §124,
; NPSPEC-KERNEL-0027). 16-Bit-Real-Mode-Code, der nach dem Kopieren auf
; physisch 0x8000 (AP_TRAMPOLINE_BASE, siehe smp.c) via INIT-SIPI-SIPI
; angesprungen wird. Patch-Bereich (GDT-Limit/-Basis, PM-Einstiegspunkt,
; Code-Selektor) wird von smp.c zur Laufzeit befuellt - siehe Layout-
; Kommentar unten. ap_entry32_pm ist regulaerer 32-Bit-Code im normalen
; Kernel-Image (KEIN Kopieren/Patchen noetig - der Fern-Sprung am Ende
; des 16-Bit-Codes liest Ziel-EIP/CS aus dem gepatchten Speicherbereich,
; aber das Ziel selbst liegt an seiner normalen, vom Linker festgelegten
; Adresse oberhalb 1 MiB).
bits 32
section .text

global ap_trampoline_blob
global ap_trampoline_blob_end
global ap_entry32_pm
extern ap_entry_c

; ---------------------------------------------------------------------------
; Layout (erweitert gegenueber dem alten ASM um ein eigenes Stack-Feld,
; da mehrere APs NICHT denselben Stack teilen duerfen - siehe smp.c:
; smp_initialize patcht dieses Feld vor JEDER einzelnen AP-SIPI neu, bevor
; der naechste AP gestartet wird (strikt sequenziell, wie schon zuvor)):
;   +0x00  jmp short .code     (2 Bytes, EB 0E)
;   +0x02  GDT-Limit           (2 Bytes, von smp.c gepatcht)
;   +0x04  GDT-Basis           (4 Bytes, von smp.c gepatcht)
;   +0x08  PM-Einstiegspunkt   (4 Bytes, von smp.c gepatcht: &ap_entry32_pm)
;   +0x0C  Code-Selektor       (2 Bytes, von smp.c gepatcht: 0x08)
;   +0x0E  Padding             (2 Bytes)
;   +0x10  AP-Stack-Spitze     (4 Bytes, von smp.c PRO AP neu gepatcht)
;   +0x14  eigentlicher 16-Bit-Code
; ---------------------------------------------------------------------------
align 16
ap_trampoline_blob:
bits 16
    jmp short .code
    dw 0                    ; +0x02 GDT-Limit (gepatcht)
    dd 0                    ; +0x04 GDT-Basis (gepatcht)
    dd 0                    ; +0x08 PM-Einstieg EIP (gepatcht)
    dw 0                    ; +0x0C Code-Selektor (gepatcht)
    dw 0                    ; +0x0E Padding
    dd 0                    ; +0x10 AP-Stack-Spitze (pro AP gepatcht)
.code:                      ; ab hier 0x8014, wenn Blob nach 0x8000 kopiert
    cli
    cld
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    ; I/O-Zugriff erzwingt bei manchen Hypervisor-Backends einen VMEXIT,
    ; der den AP-Start zuverlaessig abschliesst (WHPX-Kompatibilitaet).
    out 0x80, al
    lgdt [word 0x8002]
    mov eax, cr0
    or al, 1
    mov cr0, eax
    o32 jmp far [word 0x8008]
bits 32
ap_trampoline_blob_end:

; ---------------------------------------------------------------------------
; AP Protected-Mode-Einstieg (CS=0x08 nach dem Fern-Sprung, IF=0, kein
; Paging aktiv - der AP adressiert direkt physisch/linear flach, siehe
; smp.c fuer die Begruendung). Segmentregister MUESSEN als ALLERERSTES
; neu geladen werden, bevor irgendeine Stack-Operation (push/call/ret)
; ausgefuehrt wird - SS trug bis hier noch den realen Nullselektor.
; ---------------------------------------------------------------------------
ap_entry32_pm:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    xor ax, ax
    mov fs, ax
    mov gs, ax
    mov esp, [0x8010]         ; von smp.c pro AP gepatchte Stack-Spitze
    call ap_entry_c
.hang:
    cli
    hlt
    jmp .hang
