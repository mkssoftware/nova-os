; ---------------------------------------------------------------------------
; Interrupt- und Timer-Architektur (ADR-2006 / ADR-2007 / NPSPEC-KERNEL-0009 / NPSPEC-KERNEL-0014 / NPSPEC-HAL-INTERRUPT-0001)
; NPSPEC-INTERRUPT-0001 / NPSPEC-INTERRUPT-AFFINITY-0001 / NPSPEC-INTERRUPT-PRIORITY-0001
; ---------------------------------------------------------------------------

IDT_ENTRY_COUNT equ 256
IDT_GATE_FLAGS  equ 0x8E
PIC1_COMMAND    equ 0x20
PIC1_DATA       equ 0x21
PIC2_COMMAND    equ 0xA0
PIC2_DATA       equ 0xA1
PIC_EOI         equ 0x20
PIT_COMMAND     equ 0x43
PIT_CHANNEL0    equ 0x40
PIT_DIVISOR     equ 11932           ; ungefähr 100 Hz
INTERRUPT_API_SIZE equ 32
INTERRUPT_CAPABILITIES equ 0x00000007

interrupt_initialize:
    cli
    xor ebx, ebx
.default_gate:
    mov eax, isr_unexpected
    call idt_set_gate
    inc ebx
    cmp ebx, IDT_ENTRY_COUNT
    jb .default_gate

    xor ebx, ebx
.exception_gate:
    mov eax, [exception_stub_table + ebx * 4]
    call idt_set_gate
    inc ebx
    cmp ebx, 32
    jb .exception_gate

    mov ebx, 32
    mov eax, irq0_stub
    call idt_set_gate
    mov ebx, 33
    mov eax, irq1_stub
    call idt_set_gate
    mov ebx, 0x80
    mov eax, syscall_stub
    call idt_set_gate
    mov byte [idt_table + 0x80 * 8 + 5], 0xEE ; present, Ring-3, Interrupt-Gate
    ; SMP IPI-Vektor (Prio: nach Syscall, vor LIDT)
    mov ebx, SMP_IPI_VECTOR
    mov eax, isr_ipi
    call idt_set_gate
    ; AP LAPIC-Timer-Vektor
    mov ebx, LAPIC_TIMER_VECTOR
    mov eax, isr_ap_timer
    call idt_set_gate
    lidt [idt_descriptor]

    cmp dword [kernel_context + CONTEXT_PLATFORM], 2
    je apic_initialize

    ; UEFI/OVMF kann externe Interrupts über den APIC-Pfad hinterlassen.
    ; Der aktuelle Bootstrap-Interruptmanager arbeitet noch mit dem 8259 PIC,
    ; daher werden Local APIC und IMCR vor dessen Programmierung deterministisch
    ; in den Legacy-INTR-Zustand gebracht. Die spätere APIC-Phase übernimmt die
    ; erneute Aktivierung kontrolliert.
    mov ecx, 0x1B                   ; IA32_APIC_BASE
    rdmsr
    and eax, 0xFFFFF7FF             ; globales APIC-Enable (Bit 11) löschen
    wrmsr
    mov al, 0x70
    out 0x22, al
    in al, 0x23
    and al, 0xFE
    out 0x23, al

    ; 8259 PIC: IRQs auf Vektoren 32..47 verschieben.
    mov al, 0x11
    out PIC1_COMMAND, al
    call io_wait
    out PIC2_COMMAND, al
    call io_wait
    mov al, 0x20
    out PIC1_DATA, al
    call io_wait
    mov al, 0x28
    out PIC2_DATA, al
    call io_wait
    mov al, 0x04
    out PIC1_DATA, al
    call io_wait
    mov al, 0x02
    out PIC2_DATA, al
    call io_wait
    mov al, 0x01
    out PIC1_DATA, al
    call io_wait
    out PIC2_DATA, al
    call io_wait
    mov al, 0xFC                    ; nur Timer und Tastatur freigeben
    out PIC1_DATA, al
    mov al, 0xFF
    out PIC2_DATA, al
    clc
    ret

apic_initialize:
    ; Local APIC aktivieren und Spurious-Interrupt-Vektor freischalten.
    mov ecx, 0x1B
    rdmsr
    or eax, 0x00000800
    wrmsr
    mov eax, [0xFEE000F0]
    or eax, 0x00000100
    and eax, 0xFFFFFF00
    or eax, 0xFF
    mov [0xFEE000F0], eax

    ; OVMF darf beim Ownership-Transfer noch einen angenommenen Interrupt im
    ; ISR-Stack hinterlassen. Mehrfache EOIs sind für leere Ebenen harmlos.
    mov ecx, 8
.drain_isr:
    mov dword [0xFEE000B0], 0
    loop .drain_isr

    ; Q35: PIT IRQ0 ist per ACPI-Override auf GSI 2, Tastatur bleibt GSI 1.
    mov eax, 0x15                   ; GSI 2 destination high
    xor edx, edx
    call ioapic_write
    mov eax, 0x14                   ; GSI 2 -> Vektor 32
    mov edx, 32
    call ioapic_write
    mov eax, 0x13                   ; GSI 1 destination high
    xor edx, edx
    call ioapic_write
    mov eax, 0x12                   ; GSI 1 -> Vektor 33
    mov edx, 33
    call ioapic_write
    clc
    ret

ioapic_write:
    mov [0xFEC00000], eax
    mov [0xFEC00010], edx
    ret

; EBX=Vektor, EAX=Handleradresse.
idt_set_gate:
    push edi
    mov edi, idt_table
    lea edi, [edi + ebx * 8]
    mov [edi + 0], ax
    mov word [edi + 2], CODE_SEGMENT
    mov byte [edi + 4], 0
    mov byte [edi + 5], IDT_GATE_FLAGS
    shr eax, 16
    mov [edi + 6], ax
    pop edi
    ret

; §009 interrupt_self_test – IDT-Integrität und Gate-Programmierung nach
; interrupt_initialize (NPSPEC-KERNEL-0009)
interrupt_self_test:
    push ebx
    push edi
    sub esp, 8                          ; [esp+0..5] = IDTR-Puffer (6 Byte)
    xor edi, edi                        ; Fehler-Zähler
    ; Test 1: IDT geladen – sidt → Basis == idt_table
    sidt [esp]
    mov eax, [esp + 2]
    cmp eax, idt_table
    je .t2
    inc edi
.t2:
    ; Test 2: Limit == (IDT_ENTRY_COUNT*8)-1
    movzx eax, word [esp]
    cmp eax, (IDT_ENTRY_COUNT * 8) - 1
    je .t3
    inc edi
.t3:
    ; Test 3: Vektor 0 trägt den Exception-Stub (nicht isr_unexpected)
    movzx eax, word [idt_table + 0 * 8 + 0]
    movzx ebx, word [idt_table + 0 * 8 + 6]
    shl ebx, 16
    or eax, ebx
    cmp eax, isr_unexpected
    jne .t4
    inc edi
.t4:
    ; Test 4: Vektor 32 (IRQ0/PIT-Timer) == irq0_stub
    movzx eax, word [idt_table + 32 * 8 + 0]
    movzx ebx, word [idt_table + 32 * 8 + 6]
    shl ebx, 16
    or eax, ebx
    cmp eax, irq0_stub
    je .t5
    inc edi
.t5:
    ; Test 5: Vektor 0x80 (Syscall) == syscall_stub
    movzx eax, word [idt_table + 0x80 * 8 + 0]
    movzx ebx, word [idt_table + 0x80 * 8 + 6]
    shl ebx, 16
    or eax, ebx
    cmp eax, syscall_stub
    je .t6
    inc edi
.t6:
    ; Test 6: Syscall-Gate DPL=3 (Byte 5 == 0xEE)
    movzx eax, byte [idt_table + 0x80 * 8 + 5]
    cmp eax, 0xEE
    je .done
    inc edi
.done:
    test edi, edi
    jnz .selftest_fail
    add esp, 8
    pop edi
    pop ebx
    clc
    ret
.selftest_fail:
    add esp, 8
    pop edi
    pop ebx
    stc
    ret

timer_initialize:
    mov al, 0x36
    out PIT_COMMAND, al
    mov ax, PIT_DIVISOR
    out PIT_CHANNEL0, al
    mov al, ah
    out PIT_CHANNEL0, al
    mov dword [timer_ticks], 0
    clc
    ret

interrupt_enable:
    sti
    ret

interrupt_disable:
    cli
    ret

timer_get_ticks:
    mov eax, [timer_ticks]
    ret

; Mindestens zwei echte IRQ0-Ereignisse müssen eintreffen.
timer_self_test:
    mov ecx, 50000000
.wait:
    cmp dword [timer_ticks], 2
    jae .success
    pause
    dec ecx
    jnz .wait
.invalid:
    stc
    ret
.success:
    clc
    ret

io_wait:
    mov al, 0
    out 0x80, al
    ret

; Wandelt PS/2-Scan-Codes aus Set 1 oder 2 in wenige semantische Aktionen um.
; Ausgabe EAX: 0 = verarbeitet/ignoriert, 2 = Diagnose-Panic, 3 = Shutdown.
input_router_handle_scancode:
    movzx ebx, al
    cmp al, 0xE0
    jne .not_extended_prefix
    mov byte [keyboard_extended], 1
    xor eax, eax
    ret
.not_extended_prefix:
    cmp al, 0xF0
    jne .not_break_prefix
    mov byte [keyboard_break_pending], 1
    xor eax, eax
    ret
.not_break_prefix:
    cmp byte [keyboard_break_pending], 0
    je .not_set2_break
    mov byte [keyboard_break_pending], 0
    mov byte [keyboard_extended], 0
    xor eax, eax
    ret
.not_set2_break:
    test al, 0x80                    ; Break-Code aus Set 1
    jz .make_code
    mov byte [keyboard_extended], 0
    xor eax, eax
    ret
.make_code:
    cmp byte [keyboard_extended], 0
    je .plain
    mov byte [keyboard_extended], 0
    cmp al, 0x5B                    ; linke Windows/Nova-Taste, Set 1
    je .toggle_start
    cmp al, 0x1F                    ; linke Windows/Nova-Taste, Set 2
    je .toggle_start
    cmp al, 0x48                    ; Pfeil hoch, Set 1
    je .navigate_up
    cmp al, 0x75                    ; Pfeil hoch, Set 2
    je .navigate_up
    cmp al, 0x50                    ; Pfeil runter, Set 1
    je .navigate_down
    cmp al, 0x72                    ; Pfeil runter, Set 2
    je .navigate_down
    cmp al, 0x4B                    ; Pfeil links, Set 1
    je .navigate_left
    cmp al, 0x6B                    ; Pfeil links, Set 2
    je .navigate_left
    cmp al, 0x4D                    ; Pfeil rechts, Set 1
    je .navigate_right
    cmp al, 0x74                    ; Pfeil rechts, Set 2
    je .navigate_right
    cmp al, 0x53                    ; Entf, Set 1
    je .delete_key
    cmp al, 0x71                    ; Entf, Set 2
    je .delete_key
    xor eax, eax
    ret
.plain:
    cmp al, 0x58                    ; F12, Set 1
    je .panic
    cmp al, 0x07                    ; F12, Set 2
    je .panic
    cmp al, 0x01                    ; Escape, Set 1
    je .escape
    cmp al, 0x76                    ; Escape, Set 2
    je .escape
    cmp al, 0x0F                    ; Tab, Set 1
    je .focus_next
    cmp al, 0x0D                    ; Tab, Set 2
    je .focus_next
    cmp al, 0x1C                    ; Enter, Set 1
    je .activate
    cmp al, 0x5A                    ; Enter, Set 2
    je .activate
    cmp al, 0x0E                    ; Backspace, Set 1
    je .navigate_back
    cmp al, 0x66                    ; Backspace, Set 2
    je .navigate_back
    cmp al, 0x3C                    ; F2, Set 1
    je .rename_key
    cmp al, 0x06                    ; F2, Set 2
    je .rename_key
    ; Alle uebrigen "flachen" Tasten: ueber eine US-QWERTY-Tabelle (nur
    ; Set 1, Kleinbuchstaben, keine Umschalt-Unterstuetzung) in ein
    ; druckbares Zeichen fuer die Texteingabe (Umbenennen) uebersetzen.
    ; Set-2-Tasten und nicht zugeordnete Set-1-Codes bleiben wirkungslos.
    movzx edx, al
    mov bl, [keyboard_ascii_table + edx]
    cmp bl, 0
    je .ignored
    mov eax, SYSTEM_INPUT_TEXT_CHAR
    call input_router_enqueue
    xor eax, eax
    ret
.ignored:
    xor eax, eax
    ret
.navigate_back:
    mov eax, SYSTEM_INPUT_NAVIGATE_BACK
    call input_router_enqueue
    xor eax, eax
    ret
.rename_key:
    mov eax, SYSTEM_INPUT_RENAME_KEY
    call input_router_enqueue
    xor eax, eax
    ret
.toggle_start:
    mov eax, SYSTEM_INPUT_TOGGLE_START
    call input_router_enqueue
    xor eax, eax
    ret
.escape:
    test dword [display_scene_flags], DISPLAY_SCENE_START_MENU
    jz .shutdown
    mov eax, SYSTEM_INPUT_CLOSE_START
    call input_router_enqueue
    xor eax, eax
    ret
.focus_next:
    mov eax, SYSTEM_INPUT_FOCUS_NEXT
    call input_router_enqueue
    xor eax, eax
    ret
.activate:
    mov eax, SYSTEM_INPUT_ACTIVATE
    call input_router_enqueue
    xor eax, eax
    ret
.navigate_up:
    mov eax, SYSTEM_INPUT_NAVIGATE_UP
    call input_router_enqueue
    xor eax, eax
    ret
.navigate_down:
    mov eax, SYSTEM_INPUT_NAVIGATE_DOWN
    call input_router_enqueue
    xor eax, eax
    ret
.delete_key:
    mov eax, SYSTEM_INPUT_DELETE
    call input_router_enqueue
    xor eax, eax
    ret
.navigate_left:
    mov eax, SYSTEM_INPUT_NAVIGATE_LEFT
    call input_router_enqueue
    xor eax, eax
    ret
.navigate_right:
    mov eax, SYSTEM_INPUT_NAVIGATE_RIGHT
    call input_router_enqueue
    xor eax, eax
    ret
.panic:
    mov eax, 2
    ret
.shutdown:
    mov eax, 3
    ret

; EAX = Aktion, EBX = ursprünglicher Scan-Code. Ein einzelner Slot begrenzt
; Speicherverbrauch und verhindert Eingabefluten im Bootstrap-Displaypfad.
input_router_enqueue:
    mov edx, [display_scene_focus]
input_router_enqueue_target:
    cmp dword [display_server_ready], 1
    jne .ignored
    cmp dword [userspace_ready_seen], 1
    jne .ignored
    cmp dword [display_input_pending], 0
    jne .dropped
    mov [display_input_action], eax
    mov [display_input_scancode], ebx
    mov ecx, [timer_ticks]
    mov [display_input_tick], ecx
    mov [display_input_target], edx
    mov dword [display_input_pending], 1
.ignored:
    ret
.dropped:
    inc dword [display_input_dropped]
    ret

interrupt_dispatch:
    push ebp
    mov ebp, esp
    push ebx                            ; callee-saved; holds our frame ptr locally
    mov eax, [ebp + 8]
    mov edx, [ebp + 12]
    mov [interrupt_return_frame], edx
    mov ebx, edx                        ; keep frame ptr in EBX (race-safe local copy)
    cmp eax, 32
    je .timer
    cmp eax, 33
    je .keyboard
    cmp eax, 0x80
    je .syscall
    cmp eax, 32
    jb .exception
    cmp eax, 48
    jb .slave_irq
    jmp .done
.syscall:
    call syscall_dispatch
    jmp .done
.timer:
    inc dword [timer_ticks]
    cmp dword [kernel_spinner_active], 0
    je .timer_no_spin
    mov eax, [timer_ticks]
    test eax, 15
    jnz .timer_no_spin
    call kernel_boot_spinner_tick
.timer_no_spin:
    cmp dword [kernel_context + CONTEXT_PLATFORM], 2
    jne .timer_input
    mov dword [0xFEE000B0], 0       ; edge-triggered: früh quittieren
.timer_input:
    in al, 0x64                     ; Fallback, falls QEMU IRQ1 nicht zustellt
    test al, 0x01
    jz .timer_schedule
    mov ah, al
    in al, 0x60
    test ah, 0x20                   ; AUX-Daten stammen von der PS/2-Maus
    jnz .timer_mouse
    call input_router_handle_scancode
    cmp eax, 2
    je .debug_panic
    cmp eax, 3
    je kernel_shutdown
    jmp .timer_schedule
.timer_mouse:
    call ps2_mouse_handle_byte
.timer_schedule:
    ; Vor Scheduler-Initialisierung dient IRQ0 nur als PIT-Lebenszeichen.
    ; Deadline-, I/O- und Scheduling-Pfade greifen auf Managerzustand zu, der
    ; in der frühen Interruptphase noch nicht aufgebaut ist.
    cmp dword [scheduler_enabled], 1
    jne .timer_ack
    call task_deadline_poll
    call io_request_poll_deadlines
    call io_cancel_for_requested_tasks
    ; Scheduler-Spinlock: BSP hält ihn immer (kein Contention im Normalfall,
    ; da APs via isr_ap_timer nur versuchen, den Lock zu bekommen).
    ; lock bts setzt Bit 0 und gibt alten Wert zurück; CF=1 → schon gehalten.
    lock bts dword [scheduler_lock], 0
    jc .sched_skip              ; Sollte auf BSP nie eintreten (defensive Guard)
    mov dword [scheduler_caller_cpu], 0  ; BSP = CPU-Slot 0 (§133)
    mov eax, ebx                        ; use our local frame ptr (not shared global)
    push eax
    call scheduler_on_tick
    add esp, 4
    mov [interrupt_return_frame], eax
    mov ebx, eax                        ; keep local copy in sync
    ; §133: BSP per-CPU current thread nachführen (wie isr_ap_timer für APs)
    mov edx, [scheduler_selected_slot]
    cmp edx, -1
    je .bsp_sched_done
    mov [per_cpu_current_thread], edx   ; per_cpu_current_thread[0] = neuer Slot
.bsp_sched_done:
    lock btr dword [scheduler_lock], 0
    push eax
    mov al, PIC_EOI
    out PIC1_COMMAND, al
    pop eax
    jmp .done
.sched_skip:
.timer_ack:
    mov al, PIC_EOI
    out PIC1_COMMAND, al
    jmp .done
.keyboard:
    cmp dword [kernel_context + CONTEXT_PLATFORM], 2
    jne .keyboard_read
    mov dword [0xFEE000B0], 0
.keyboard_read:
    in al, 0x60                     ; Controllerdaten quittieren
    call input_router_handle_scancode
    cmp eax, 2
    je .debug_panic
    cmp eax, 3
    je kernel_shutdown
    jmp .keyboard_ack
.debug_panic:
    mov eax, 0xDEB60001
    mov edx, 0x0000DEB6
    mov esi, message_debug_panic
    call kernel_panic
.keyboard_ack:
    mov al, PIC_EOI
    out PIC1_COMMAND, al
    jmp .done
.slave_irq:
    mov al, PIC_EOI
    out PIC2_COMMAND, al
    out PIC1_COMMAND, al
    jmp .done
.exception:
    cli
    mov [last_exception_vector], eax
    mov [interrupt_return_frame], edx
    ; §010 Exception Manager: vollständige Klassifizierung und Weiterleitung
    cmp dword [exception_mgr_initialized], 1
    je .exception_mgr_dispatch
    ; Frühphase: primitiver Fallback bevor §010 bereit ist
    cmp eax, 14
    jne .log_exception_early
    mov eax, cr2
    mov [last_fault_address], eax
.log_exception_early:
    mov esi, message_exception
    call serial_write_string
    mov eax, [last_exception_vector]
    call serial_write_hex32
    cmp dword [last_exception_vector], 14
    jne .exception_newline_early
    mov esi, message_fault_address
    call serial_write_string
    mov eax, [last_fault_address]
    call serial_write_hex32
.exception_newline_early:
    mov esi, message_newline
    call serial_write_string
    jmp kernel_halt
.exception_mgr_dispatch:
    ; Übergabe an §010 exception_manager_dispatch
    ; EAX = Vektor, EDX = Frame-Zeiger (Stack bei isr_common)
    mov edx, [interrupt_return_frame]
    push edx
    push eax
    call exception_manager_dispatch
    add esp, 8
    ; EAX = neuer Frame-Zeiger (oder 0 bei Fortsetzung des alten)
    test eax, eax
    jz .exception_done
    mov [interrupt_return_frame], eax
    mov ebx, eax                        ; keep local copy in sync
.exception_done:
.done:
    cmp dword [kernel_context + CONTEXT_PLATFORM], 2
    jne .return_frame
    mov dword [0xFEE000B0], 0       ; Local-APIC EOI
.return_frame:
    mov eax, ebx                    ; use race-safe local copy instead of shared global
    pop ebx
    pop ebp
    ret

isr_common:
    pushad
    push ds
    push es
    push fs
    push gs
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, [esp + 48]             ; normalisierter Vektor
    mov edx, esp
    push edx
    push eax
    call interrupt_dispatch
    add esp, 8
    mov esp, eax
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 8                      ; Vektor + Fehlercode
    iretd

%macro ISR_NO_ERROR 1
isr_%1:
    push dword 0
    push dword %1
    jmp isr_common
%endmacro

%macro ISR_WITH_ERROR 1
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

align 8
kernel_gdt:
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
    dq 0x00CFFA000000FFFF          ; Ring-3 Code, Selector 0x1B
    dq 0x00CFF2000000FFFF          ; Ring-3 Data, Selector 0x23
kernel_tss_descriptor:
    dq 0                            ; 32-Bit Available TSS, Selector 0x28
kernel_gdt_descriptor:
    dw kernel_gdt_descriptor - kernel_gdt - 1
    dd kernel_gdt

align 16
idt_table:
    times IDT_ENTRY_COUNT dq 0
idt_descriptor:
    dw (IDT_ENTRY_COUNT * 8) - 1
    dd idt_table

timer_ticks:           dd 0
last_exception_vector: dd 0
last_fault_address:    dd 0
interrupt_return_frame: dd 0

align 16
kernel_tss:
    times 104 db 0

; Versionierte öffentliche Modulgrenze gemäß ADR-2006/2007.
align 4
interrupt_api:
    dd INTERRUPT_API_SIZE
    dw 1, 0
    dd IDT_ENTRY_COUNT
    dd INTERRUPT_CAPABILITIES
    dd interrupt_enable
    dd interrupt_disable
    dd timer_get_ticks
    dd 100

; ---------------------------------------------------------------------------
; Kernel-Nachrichtenwarteschlange (ADR-2005 / NPSPEC-KERNEL-0015)
; ---------------------------------------------------------------------------

