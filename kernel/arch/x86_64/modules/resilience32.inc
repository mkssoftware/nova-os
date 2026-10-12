; ===========================================================================
; NPSPEC-RESILIENCE-WATCHDOG-0001 – Nova Watchdog Timer
; Periodisch zu kickender Timer; bei Ablauf konfigurierbare Recovery-Aktion.
; ===========================================================================

WD_CAPACITY       equ 4
WD_RECORD_SIZE    equ 40

WD_STATE_FREE     equ 0
WD_STATE_DISARMED equ 1
WD_STATE_ARMED    equ 2
WD_STATE_EXPIRED  equ 3

WD_ACTION_PANIC    equ 1
WD_ACTION_REBOOT   equ 2
WD_ACTION_LOG      equ 3
WD_ACTION_CALLBACK equ 4

WD_ID             equ 0
WD_STATE          equ 4
WD_OWNER          equ 8
WD_TIMEOUT_TICKS  equ 12
WD_LAST_KICK      equ 16
WD_ACTION_OFF     equ 20
WD_FLAGS_OFF      equ 24
WD_CALLBACK       equ 28
WD_EXPIRE_COUNT   equ 32
WD_PAD            equ 36

watchdog_initialize:
    mov edi, wd_table
    mov ecx, WD_CAPACITY * WD_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [wd_next_id], 0
    mov dword [wd_global_tick], 0
    mov dword [wd_live_count], 0
    mov dword [wd_total_expires], 0
    mov dword [wd_manager_ready], 1
    clc
    ret

; EAX=owner_pid  EDX=timeout_ticks  ECX=action → EAX=slot_index CF=0 / CF=1 full
watchdog_create:
    push ebx
    push esi
    push edi
    push edx
    push ecx
    mov esi, wd_table
    xor ebx, ebx
.wdc_scan:
    cmp ebx, WD_CAPACITY
    jae .wdc_full
    cmp dword [esi + WD_STATE], WD_STATE_FREE
    je .wdc_found
    add esi, WD_RECORD_SIZE
    inc ebx
    jmp .wdc_scan
.wdc_found:
    pop ecx        ; action
    pop edx        ; timeout
    mov edi, [wd_next_id]
    inc dword [wd_next_id]
    mov [esi + WD_ID], edi
    mov dword [esi + WD_STATE], WD_STATE_DISARMED
    mov [esi + WD_OWNER], eax
    mov [esi + WD_TIMEOUT_TICKS], edx
    mov [esi + WD_ACTION_OFF], ecx
    mov dword [esi + WD_LAST_KICK], 0
    mov dword [esi + WD_FLAGS_OFF], 0
    mov dword [esi + WD_CALLBACK], 0
    mov dword [esi + WD_EXPIRE_COUNT], 0
    mov dword [esi + WD_PAD], 0
    inc dword [wd_live_count]
    mov eax, ebx
    clc
    pop edi
    pop esi
    pop ebx
    ret
.wdc_full:
    pop ecx
    pop edx
    stc
    pop edi
    pop esi
    pop ebx
    ret

; EAX=slot → DISARMED→ARMED, last_kick=current_tick, CF=0 / CF=1
watchdog_arm:
    push esi
    push edx
    cmp eax, WD_CAPACITY
    jae .wda_fail
    imul esi, eax, WD_RECORD_SIZE
    add esi, wd_table
    cmp dword [esi + WD_STATE], WD_STATE_DISARMED
    jne .wda_fail
    mov edx, [wd_global_tick]
    mov [esi + WD_LAST_KICK], edx
    mov dword [esi + WD_STATE], WD_STATE_ARMED
    clc
    pop edx
    pop esi
    ret
.wda_fail:
    stc
    pop edx
    pop esi
    ret

; EAX=slot → last_kick=current_tick, CF=0 / CF=1 not armed
watchdog_kick:
    push esi
    push edx
    cmp eax, WD_CAPACITY
    jae .wdk_fail
    imul esi, eax, WD_RECORD_SIZE
    add esi, wd_table
    cmp dword [esi + WD_STATE], WD_STATE_ARMED
    jne .wdk_fail
    mov edx, [wd_global_tick]
    mov [esi + WD_LAST_KICK], edx
    clc
    pop edx
    pop esi
    ret
.wdk_fail:
    stc
    pop edx
    pop esi
    ret

; EAX=slot → state=DISARMED, CF=0 / CF=1 invalid
watchdog_disarm:
    push esi
    cmp eax, WD_CAPACITY
    jae .wdd_fail
    imul esi, eax, WD_RECORD_SIZE
    add esi, wd_table
    mov dword [esi + WD_STATE], WD_STATE_DISARMED
    clc
    pop esi
    ret
.wdd_fail:
    stc
    pop esi
    ret

; Inkrementiert wd_global_tick — aus Timer-IRQ aufrufen
watchdog_tick:
    inc dword [wd_global_tick]
    ret

; Prüft alle ARMED Watchdogs; verfallene werden gemäß Action behandelt
watchdog_check:
    push ebx
    push esi
    push edx
    push ecx
    mov esi, wd_table
    xor ebx, ebx
.wdchk_loop:
    cmp ebx, WD_CAPACITY
    jae .wdchk_done
    cmp dword [esi + WD_STATE], WD_STATE_ARMED
    jne .wdchk_next
    mov edx, [wd_global_tick]
    sub edx, [esi + WD_LAST_KICK]
    cmp edx, [esi + WD_TIMEOUT_TICKS]
    jb .wdchk_next
    ; abgelaufen
    inc dword [wd_total_expires]
    inc dword [esi + WD_EXPIRE_COUNT]
    mov ecx, [esi + WD_ACTION_OFF]
    cmp ecx, WD_ACTION_PANIC
    je .wdchk_panic
    mov dword [esi + WD_STATE], WD_STATE_EXPIRED
    jmp .wdchk_next
.wdchk_panic:
    mov dword [esi + WD_STATE], WD_STATE_EXPIRED
    mov eax, 0x00003033
    mov edx, 48
    mov esi, message_watchdog_error
    jmp kernel_panic
.wdchk_next:
    add esi, WD_RECORD_SIZE
    inc ebx
    jmp .wdchk_loop
.wdchk_done:
    pop ecx
    pop edx
    pop esi
    pop ebx
    clc
    ret

watchdog_self_test:
    push ebx
    push esi
    push edx
    xor esi, esi   ; Fehlerz.

    ; T1: ready
    cmp dword [wd_manager_ready], 1
    je .wdt1_ok
    inc esi
.wdt1_ok:

    ; T2: create
    mov eax, 7          ; owner
    mov edx, 50         ; timeout
    mov ecx, WD_ACTION_LOG
    call watchdog_create
    jnc .wdt2_ok
    inc esi
.wdt2_ok:
    mov ebx, eax        ; slot

    ; T3: arm
    mov eax, ebx
    call watchdog_arm
    jnc .wdt3_ok
    inc esi
.wdt3_ok:

    ; T4: state=ARMED
    push edx
    imul edx, ebx, WD_RECORD_SIZE
    add edx, wd_table
    cmp dword [edx + WD_STATE], WD_STATE_ARMED
    je .wdt4_ok
    inc esi
.wdt4_ok:

    ; T5: kick
    mov eax, ebx
    call watchdog_kick
    jnc .wdt5_ok
    inc esi
.wdt5_ok:

    ; T6: simuliere Ablauf
    imul edx, ebx, WD_RECORD_SIZE
    add edx, wd_table
    mov dword [edx + WD_LAST_KICK], 0
    mov dword [wd_global_tick], 200
    call watchdog_check
    cmp dword [edx + WD_STATE], WD_STATE_EXPIRED
    je .wdt6_ok
    inc esi
.wdt6_ok:
    pop edx

    ; T7: disarm
    mov eax, ebx
    call watchdog_disarm
    push edx
    imul edx, ebx, WD_RECORD_SIZE
    add edx, wd_table
    cmp dword [edx + WD_STATE], WD_STATE_DISARMED
    je .wdt7_ok
    inc esi
.wdt7_ok:
    pop edx

    mov dword [wd_global_tick], 0

    test esi, esi
    jnz .wdt_fail
    clc
    pop edx
    pop esi
    pop ebx
    ret
.wdt_fail:
    stc
    pop edx
    pop esi
    pop ebx
    ret

wd_manager_ready:  dd 0
wd_next_id:        dd 0
wd_global_tick:    dd 0
wd_live_count:     dd 0
wd_total_expires:  dd 0
align 4
wd_table:
    times WD_CAPACITY * WD_RECORD_SIZE db 0

; ===========================================================================
; NPSPEC-RESILIENCE-CIRCUITBREAKER-0001 – Nova Circuit Breaker
; CLOSED→OPEN bei threshold Fehlern; OPEN→HALF-OPEN nach cooldown;
; HALF-OPEN→CLOSED nach Erfolg; HALF-OPEN→OPEN bei erneutem Fehler.
; ===========================================================================

CB_CAPACITY         equ 8
CB_RECORD_SIZE      equ 32

CB_STATE_FREE       equ 0
CB_STATE_CLOSED     equ 1
CB_STATE_OPEN       equ 2
CB_STATE_HALFOPEN   equ 3

CB_ID               equ 0
CB_STATE            equ 4
CB_OWNER            equ 8
CB_FAIL_COUNT       equ 12
CB_FAIL_THRESHOLD   equ 16
CB_LAST_FAIL_TICK   equ 20
CB_COOLDOWN_TICKS   equ 24
CB_SUCCESS_COUNT    equ 28

circuitbreaker_initialize:
    mov edi, cb_table
    mov ecx, CB_CAPACITY * CB_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [cb_next_id], 0
    mov dword [cb_live_count], 0
    mov dword [cb_trips], 0
    mov dword [cb_ready], 1
    clc
    ret

; EAX=owner  EDX=fail_threshold  ECX=cooldown_ticks → EAX=slot CF=0 / CF=1 voll
circuitbreaker_create:
    push ebx
    push esi
    mov esi, cb_table
    xor ebx, ebx
.cbc_scan:
    cmp ebx, CB_CAPACITY
    jae .cbc_full
    cmp dword [esi + CB_STATE], CB_STATE_FREE
    je .cbc_found
    add esi, CB_RECORD_SIZE
    inc ebx
    jmp .cbc_scan
.cbc_found:
    push eax
    push edx
    push ecx
    mov eax, [cb_next_id]
    inc dword [cb_next_id]
    mov [esi + CB_ID], eax
    mov dword [esi + CB_STATE], CB_STATE_CLOSED
    pop ecx         ; cooldown
    pop edx         ; threshold
    pop eax         ; owner
    mov [esi + CB_OWNER], eax
    mov dword [esi + CB_FAIL_COUNT], 0
    mov [esi + CB_FAIL_THRESHOLD], edx
    mov dword [esi + CB_LAST_FAIL_TICK], 0
    mov [esi + CB_COOLDOWN_TICKS], ecx
    mov dword [esi + CB_SUCCESS_COUNT], 0
    inc dword [cb_live_count]
    mov eax, ebx
    clc
    pop esi
    pop ebx
    ret
.cbc_full:
    stc
    pop esi
    pop ebx
    ret

; EAX=slot → CF=0 Aufruf erlaubt (CLOSED/HALF-OPEN) / CF=1 OPEN gesperrt
circuitbreaker_check:
    push esi
    cmp eax, CB_CAPACITY
    jae .cbchk_fail
    imul esi, eax, CB_RECORD_SIZE
    add esi, cb_table
    mov eax, [esi + CB_STATE]
    cmp eax, CB_STATE_OPEN
    je .cbchk_fail
    cmp eax, CB_STATE_FREE
    je .cbchk_fail
    clc
    pop esi
    ret
.cbchk_fail:
    stc
    pop esi
    ret

; EAX=slot → Fehler aufzeichnen; bei threshold → OPEN
circuitbreaker_record_failure:
    push esi
    push edx
    cmp eax, CB_CAPACITY
    jae .cbrf_done
    imul esi, eax, CB_RECORD_SIZE
    add esi, cb_table
    mov edx, [esi + CB_STATE]
    cmp edx, CB_STATE_FREE
    je .cbrf_done
    inc dword [esi + CB_FAIL_COUNT]
    mov edx, [wd_global_tick]
    mov [esi + CB_LAST_FAIL_TICK], edx
    cmp dword [esi + CB_STATE], CB_STATE_HALFOPEN
    je .cbrf_reopen
    mov edx, [esi + CB_FAIL_COUNT]
    cmp edx, [esi + CB_FAIL_THRESHOLD]
    jb .cbrf_done
    mov dword [esi + CB_STATE], CB_STATE_OPEN
    inc dword [cb_trips]
    jmp .cbrf_done
.cbrf_reopen:
    mov dword [esi + CB_STATE], CB_STATE_OPEN
    mov dword [esi + CB_SUCCESS_COUNT], 0
    inc dword [cb_trips]
.cbrf_done:
    clc
    pop edx
    pop esi
    ret

; EAX=slot → Erfolg aufzeichnen; HALF-OPEN→CLOSED nach erstem Erfolg
circuitbreaker_record_success:
    push esi
    cmp eax, CB_CAPACITY
    jae .cbrs_done
    imul esi, eax, CB_RECORD_SIZE
    add esi, cb_table
    cmp dword [esi + CB_STATE], CB_STATE_HALFOPEN
    jne .cbrs_done
    inc dword [esi + CB_SUCCESS_COUNT]
    mov dword [esi + CB_STATE], CB_STATE_CLOSED
    mov dword [esi + CB_FAIL_COUNT], 0
.cbrs_done:
    clc
    pop esi
    ret

; OPEN-Circuits prüfen: bei abgelaufenem cooldown → HALF-OPEN
circuitbreaker_poll:
    push ebx
    push esi
    push edx
    mov esi, cb_table
    xor ebx, ebx
.cbp_loop:
    cmp ebx, CB_CAPACITY
    jae .cbp_done
    cmp dword [esi + CB_STATE], CB_STATE_OPEN
    jne .cbp_next
    mov edx, [wd_global_tick]
    sub edx, [esi + CB_LAST_FAIL_TICK]
    cmp edx, [esi + CB_COOLDOWN_TICKS]
    jb .cbp_next
    mov dword [esi + CB_STATE], CB_STATE_HALFOPEN
    mov dword [esi + CB_SUCCESS_COUNT], 0
.cbp_next:
    add esi, CB_RECORD_SIZE
    inc ebx
    jmp .cbp_loop
.cbp_done:
    pop edx
    pop esi
    pop ebx
    clc
    ret

circuitbreaker_self_test:
    push ebx
    push esi
    xor esi, esi    ; Fehlerz.

    ; T1: ready
    cmp dword [cb_ready], 1
    je .cbst1_ok
    inc esi
.cbst1_ok:

    ; T2: create
    mov eax, 1          ; owner
    mov edx, 3          ; fail_threshold=3
    mov ecx, 10         ; cooldown=10 ticks
    call circuitbreaker_create
    jnc .cbst2_ok
    inc esi
.cbst2_ok:
    mov ebx, eax        ; slot

    ; T3: check → CF=0 (CLOSED)
    mov eax, ebx
    call circuitbreaker_check
    jnc .cbst3_ok
    inc esi
.cbst3_ok:

    ; T4: 3 Fehler → OPEN
    mov eax, ebx
    call circuitbreaker_record_failure
    mov eax, ebx
    call circuitbreaker_record_failure
    mov eax, ebx
    call circuitbreaker_record_failure
    push edx
    imul edx, ebx, CB_RECORD_SIZE
    add edx, cb_table
    cmp dword [edx + CB_STATE], CB_STATE_OPEN
    je .cbst4_ok
    inc esi
.cbst4_ok:
    pop edx

    ; T5: check → CF=1 (OPEN gesperrt)
    mov eax, ebx
    call circuitbreaker_check
    jc .cbst5_ok
    inc esi
.cbst5_ok:

    ; T6: cooldown → HALF-OPEN (set last_fail to past)
    push edx
    imul edx, ebx, CB_RECORD_SIZE
    add edx, cb_table
    mov dword [edx + CB_LAST_FAIL_TICK], 0
    pop edx
    mov dword [wd_global_tick], 100
    call circuitbreaker_poll
    push edx
    imul edx, ebx, CB_RECORD_SIZE
    add edx, cb_table
    cmp dword [edx + CB_STATE], CB_STATE_HALFOPEN
    je .cbst6_ok
    inc esi
.cbst6_ok:
    pop edx

    ; T7: Erfolg → CLOSED
    mov eax, ebx
    call circuitbreaker_record_success
    push edx
    imul edx, ebx, CB_RECORD_SIZE
    add edx, cb_table
    cmp dword [edx + CB_STATE], CB_STATE_CLOSED
    je .cbst7_ok
    inc esi
.cbst7_ok:
    pop edx

    mov dword [wd_global_tick], 0

    test esi, esi
    jnz .cbstf
    clc
    pop esi
    pop ebx
    ret
.cbstf:
    stc
    pop esi
    pop ebx
    ret

cb_ready:      dd 0
cb_next_id:    dd 0
cb_live_count: dd 0
cb_trips:      dd 0
align 4
cb_table:
    times CB_CAPACITY * CB_RECORD_SIZE db 0

; ===========================================================================
; NPSPEC-RESILIENCE-DETECTION-0001 – Nova Fault Detection
; Strukturiertes Fault-Log: Type / Subsystem / Code / Tick / Sequence.
; ===========================================================================

FD_CAPACITY      equ 32
FD_RECORD_SIZE   equ 24

FD_TYPE_NONE     equ 0
FD_TYPE_MEMORY   equ 1
FD_TYPE_TIMING   equ 2
FD_TYPE_RESOURCE equ 3
FD_TYPE_SUBSYS   equ 4
FD_TYPE_HARDWARE equ 5
FD_TYPE_SECURITY equ 6

FD_SEQ           equ 0
FD_TYPE          equ 4
FD_SUBSYSTEM     equ 8
FD_CODE          equ 12
FD_TICK          equ 16
FD_FLAGS         equ 20

detection_initialize:
    mov edi, fd_log
    mov ecx, FD_CAPACITY * FD_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [fd_write_pos], 0
    mov dword [fd_seq], 0
    mov dword [fd_total], 0
    mov dword [fd_ready], 1
    clc
    ret

; EAX=fault_type  EDX=subsystem  ECX=fault_code → CF=0
detection_record_fault:
    push esi
    push edi
    push ebx
    mov edi, [fd_write_pos]
    imul esi, edi, FD_RECORD_SIZE
    add esi, fd_log
    mov ebx, [fd_seq]
    mov [esi + FD_SEQ], ebx
    inc dword [fd_seq]
    mov [esi + FD_TYPE], eax
    mov [esi + FD_SUBSYSTEM], edx
    mov [esi + FD_CODE], ecx
    mov ebx, [wd_global_tick]
    mov [esi + FD_TICK], ebx
    mov dword [esi + FD_FLAGS], 0
    inc edi
    cmp edi, FD_CAPACITY
    jb .drf_nowrap
    xor edi, edi
.drf_nowrap:
    mov [fd_write_pos], edi
    inc dword [fd_total]
    clc
    pop ebx
    pop edi
    pop esi
    ret

; EAX=fault_type → EAX=Anzahl Einträge dieses Typs (scan des gesamten Logs)
detection_get_fault_count:
    push ebx
    push esi
    push ecx
    mov esi, fd_log
    xor ebx, ebx
    mov ecx, FD_CAPACITY
.dgfc_loop:
    cmp dword [esi + FD_TYPE], FD_TYPE_NONE
    je .dgfc_next
    cmp [esi + FD_TYPE], eax
    jne .dgfc_next
    inc ebx
.dgfc_next:
    add esi, FD_RECORD_SIZE
    dec ecx
    jnz .dgfc_loop
    mov eax, ebx
    pop ecx
    pop esi
    pop ebx
    ret

detection_self_test:
    push ebx
    xor ebx, ebx

    ; T1: ready
    cmp dword [fd_ready], 1
    je .dst1_ok
    inc ebx
.dst1_ok:

    ; T2: record_fault
    mov eax, FD_TYPE_MEMORY
    mov edx, 1
    mov ecx, 0xDEAD
    call detection_record_fault
    jnc .dst2_ok
    inc ebx
.dst2_ok:

    ; T3: total=1
    cmp dword [fd_total], 1
    je .dst3_ok
    inc ebx
.dst3_ok:

    ; T4: slot 0 hat korrekten Type
    cmp dword [fd_log + FD_TYPE], FD_TYPE_MEMORY
    je .dst4_ok
    inc ebx
.dst4_ok:

    ; T5: get_fault_count(MEMORY) >= 1
    mov eax, FD_TYPE_MEMORY
    call detection_get_fault_count
    cmp eax, 1
    jae .dst5_ok
    inc ebx
.dst5_ok:

    ; T6: record HARDWARE fault
    mov eax, FD_TYPE_HARDWARE
    mov edx, 2
    mov ecx, 0xBAD0
    call detection_record_fault
    mov eax, FD_TYPE_HARDWARE
    call detection_get_fault_count
    cmp eax, 1
    je .dst6_ok
    inc ebx
.dst6_ok:

    ; T7: fd_total=2
    cmp dword [fd_total], 2
    je .dst7_ok
    inc ebx
.dst7_ok:

    test ebx, ebx
    jnz .dstf
    clc
    pop ebx
    ret
.dstf:
    stc
    pop ebx
    ret

fd_ready:     dd 0
fd_write_pos: dd 0
fd_seq:       dd 0
fd_total:     dd 0
align 4
fd_log:
    times FD_CAPACITY * FD_RECORD_SIZE db 0

; ===========================================================================
; NPSPEC-RESILIENCE-RESTART-0001 – Nova Supervised Restart
; Policy-gesteuerte Komponentenneustarts: max_restarts, cooldown, Backoff.
; ===========================================================================

RST_CAPACITY         equ 8
RST_RECORD_SIZE      equ 32

RST_STATE_FREE       equ 0
RST_STATE_ACTIVE     equ 1
RST_STATE_EXHAUSTED  equ 2

RST_POLICY_IMMEDIATE equ 0   ; Sofort neustarten
RST_POLICY_BACKOFF   equ 1   ; exponentielles Backoff
RST_POLICY_COOLDOWN  equ 2   ; fester Cooldown

RST_ID               equ 0
RST_STATE            equ 4
RST_OWNER            equ 8
RST_RESTART_COUNT    equ 12
RST_MAX_RESTARTS     equ 16
RST_COOLDOWN_TICKS   equ 20
RST_LAST_RESTART     equ 24
RST_POLICY           equ 28

restart_initialize:
    mov edi, rst_table
    mov ecx, RST_CAPACITY * RST_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [rst_next_id], 0
    mov dword [rst_total_restarts], 0
    mov dword [rst_ready], 1
    clc
    ret

; EAX=owner  EDX=max_restarts  ECX=cooldown_ticks  EBX=policy → EAX=slot CF=0 / CF=1
restart_register:
    push esi
    push edi
    mov esi, rst_table
    push ecx
    push edx
    push eax
    xor edi, edi
.rsr_scan:
    cmp edi, RST_CAPACITY
    jae .rsr_full
    cmp dword [esi + RST_STATE], RST_STATE_FREE
    je .rsr_found
    add esi, RST_RECORD_SIZE
    inc edi
    jmp .rsr_scan
.rsr_found:
    pop eax         ; owner
    pop edx         ; max_restarts
    pop ecx         ; cooldown
    push edi
    mov [esi + RST_OWNER], eax
    push eax
    mov eax, [rst_next_id]
    inc dword [rst_next_id]
    mov [esi + RST_ID], eax
    pop eax
    mov dword [esi + RST_STATE], RST_STATE_ACTIVE
    mov dword [esi + RST_RESTART_COUNT], 0
    mov [esi + RST_MAX_RESTARTS], edx
    mov [esi + RST_COOLDOWN_TICKS], ecx
    mov dword [esi + RST_LAST_RESTART], 0
    mov [esi + RST_POLICY], ebx
    pop eax         ; slot index
    clc
    pop edi
    pop esi
    ret
.rsr_full:
    pop eax
    pop edx
    pop ecx
    stc
    pop edi
    pop esi
    ret

; EAX=slot → CF=0 Neustart erlaubt, CF=1 exhausted / noch in cooldown
restart_check_and_record:
    push esi
    push edx
    cmp eax, RST_CAPACITY
    jae .rscr_fail
    imul esi, eax, RST_RECORD_SIZE
    add esi, rst_table
    cmp dword [esi + RST_STATE], RST_STATE_ACTIVE
    jne .rscr_fail
    ; max_restarts prüfen
    mov edx, [esi + RST_RESTART_COUNT]
    cmp edx, [esi + RST_MAX_RESTARTS]
    jae .rscr_exhaust
    ; cooldown prüfen
    mov edx, [wd_global_tick]
    push edx
    sub edx, [esi + RST_LAST_RESTART]
    cmp edx, [esi + RST_COOLDOWN_TICKS]
    pop edx
    jb .rscr_fail
    ; Neustart aufzeichnen
    mov [esi + RST_LAST_RESTART], edx
    inc dword [esi + RST_RESTART_COUNT]
    inc dword [rst_total_restarts]
    clc
    pop edx
    pop esi
    ret
.rscr_exhaust:
    mov dword [esi + RST_STATE], RST_STATE_EXHAUSTED
.rscr_fail:
    stc
    pop edx
    pop esi
    ret

restart_self_test:
    push ebx
    push esi
    xor esi, esi

    ; T1: ready
    cmp dword [rst_ready], 1
    je .rst1_ok
    inc esi
.rst1_ok:

    ; T2: register
    mov eax, 10        ; owner
    mov edx, 3         ; max=3
    mov ecx, 5         ; cooldown=5
    mov ebx, RST_POLICY_COOLDOWN
    call restart_register
    jnc .rst2_ok
    inc esi
.rst2_ok:
    mov ebx, eax

    ; T3: check+record (tick=0, last=0, cooldown=5 → fail bei tick=0)
    ; Setze global_tick auf 10 damit cooldown überwunden ist
    mov dword [wd_global_tick], 10
    mov eax, ebx
    call restart_check_and_record
    jnc .rst3_ok
    inc esi
.rst3_ok:

    ; T4: restart_count=1
    push edx
    imul edx, ebx, RST_RECORD_SIZE
    add edx, rst_table
    cmp dword [edx + RST_RESTART_COUNT], 1
    je .rst4_ok
    inc esi
.rst4_ok:
    pop edx

    ; T5: 2 weitere (insgesamt 3 = max)
    mov dword [wd_global_tick], 20
    mov eax, ebx
    call restart_check_and_record
    mov dword [wd_global_tick], 30
    mov eax, ebx
    call restart_check_and_record
    push edx
    imul edx, ebx, RST_RECORD_SIZE
    add edx, rst_table
    cmp dword [edx + RST_RESTART_COUNT], 3
    je .rst5_ok
    inc esi
.rst5_ok:
    pop edx

    ; T6: 4. Versuch → EXHAUSTED → CF=1
    mov dword [wd_global_tick], 40
    mov eax, ebx
    call restart_check_and_record
    jc .rst6_ok
    inc esi
.rst6_ok:

    ; T7: total_restarts=3
    cmp dword [rst_total_restarts], 3
    je .rst7_ok
    inc esi
.rst7_ok:

    mov dword [wd_global_tick], 0

    test esi, esi
    jnz .rstf
    clc
    pop esi
    pop ebx
    ret
.rstf:
    stc
    pop esi
    pop ebx
    ret

rst_ready:           dd 0
rst_next_id:         dd 0
rst_total_restarts:  dd 0
align 4
rst_table:
    times RST_CAPACITY * RST_RECORD_SIZE db 0

; ===========================================================================
; NPSPEC-RESOURCE-ACCOUNTING-0001 – Nova Resource Accounting
; Pro-Prozess-Ressourcenbudgets: CPU-Ticks, Speicher, I/O-Bytes.
; ===========================================================================

RACCT_CAPACITY      equ 8
RACCT_RECORD_SIZE   equ 40

RACCT_STATE_FREE    equ 0
RACCT_STATE_ACTIVE  equ 1

RACCT_ID            equ 0
RACCT_STATE         equ 4
RACCT_OWNER         equ 8
RACCT_CPU_USED      equ 12  ; CPU-Ticks verbraucht
RACCT_CPU_LIMIT     equ 16  ; CPU-Ticks Limit (0=unbegrenzt)
RACCT_MEM_USED      equ 20  ; Speicher in Bytes
RACCT_MEM_LIMIT     equ 24  ; Speicher-Limit
RACCT_IO_USED       equ 28  ; I/O-Bytes
RACCT_IO_LIMIT      equ 32  ; I/O-Limit
RACCT_FLAGS         equ 36

RACCT_FLAG_CPU_SOFT equ 0x01   ; Soft Limit (warnen, nicht blockieren)
RACCT_FLAG_MEM_SOFT equ 0x02
RACCT_FLAG_IO_SOFT  equ 0x04

resource_accounting_initialize:
    mov edi, racct_table
    mov ecx, RACCT_CAPACITY * RACCT_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [racct_next_id], 0
    mov dword [racct_violations], 0
    mov dword [racct_ready], 1
    clc
    ret

; EAX=owner  EDX=cpu_limit  ECX=mem_limit → EAX=slot CF=0 / CF=1
resource_accounting_create:
    push ebx
    push esi
    mov esi, racct_table
    xor ebx, ebx
.rac_scan:
    cmp ebx, RACCT_CAPACITY
    jae .rac_full
    cmp dword [esi + RACCT_STATE], RACCT_STATE_FREE
    je .rac_found
    add esi, RACCT_RECORD_SIZE
    inc ebx
    jmp .rac_scan
.rac_found:
    push eax
    push edx
    push ecx
    mov eax, [racct_next_id]
    inc dword [racct_next_id]
    mov [esi + RACCT_ID], eax
    mov dword [esi + RACCT_STATE], RACCT_STATE_ACTIVE
    pop ecx
    pop edx
    pop eax
    mov [esi + RACCT_OWNER], eax
    mov [esi + RACCT_CPU_LIMIT], edx
    mov [esi + RACCT_MEM_LIMIT], ecx
    mov dword [esi + RACCT_CPU_USED], 0
    mov dword [esi + RACCT_MEM_USED], 0
    mov dword [esi + RACCT_IO_USED], 0
    mov dword [esi + RACCT_IO_LIMIT], 0
    mov dword [esi + RACCT_FLAGS], 0
    mov eax, ebx
    clc
    pop esi
    pop ebx
    ret
.rac_full:
    stc
    pop esi
    pop ebx
    ret

; EAX=slot  EDX=cpu_delta  ECX=mem_delta → CF=0 ok / CF=1 Hard-Limit überschritten
resource_accounting_charge:
    push esi
    push ebx
    cmp eax, RACCT_CAPACITY
    jae .racc_fail
    imul esi, eax, RACCT_RECORD_SIZE
    add esi, racct_table
    cmp dword [esi + RACCT_STATE], RACCT_STATE_ACTIVE
    jne .racc_fail
    ; CPU-Limit prüfen
    mov ebx, [esi + RACCT_CPU_LIMIT]
    test ebx, ebx
    jz .racc_cpu_ok
    mov ebx, [esi + RACCT_CPU_USED]
    add ebx, edx
    cmp ebx, [esi + RACCT_CPU_LIMIT]
    ja .racc_viol
.racc_cpu_ok:
    ; Mem-Limit prüfen
    mov ebx, [esi + RACCT_MEM_LIMIT]
    test ebx, ebx
    jz .racc_mem_ok
    mov ebx, [esi + RACCT_MEM_USED]
    add ebx, ecx
    cmp ebx, [esi + RACCT_MEM_LIMIT]
    ja .racc_viol
.racc_mem_ok:
    add [esi + RACCT_CPU_USED], edx
    add [esi + RACCT_MEM_USED], ecx
    clc
    pop ebx
    pop esi
    ret
.racc_viol:
    inc dword [racct_violations]
    stc
    pop ebx
    pop esi
    ret
.racc_fail:
    stc
    pop ebx
    pop esi
    ret

; EAX=slot → EAX=cpu_used, EDX=mem_used, ECX=io_used
resource_accounting_read:
    push esi
    cmp eax, RACCT_CAPACITY
    jae .racr_zero
    imul esi, eax, RACCT_RECORD_SIZE
    add esi, racct_table
    mov eax, [esi + RACCT_CPU_USED]
    mov edx, [esi + RACCT_MEM_USED]
    mov ecx, [esi + RACCT_IO_USED]
    clc
    pop esi
    ret
.racr_zero:
    xor eax, eax
    xor edx, edx
    xor ecx, ecx
    stc
    pop esi
    ret

resource_accounting_self_test:
    push ebx
    xor ebx, ebx

    ; T1: ready
    cmp dword [racct_ready], 1
    je .rast1_ok
    inc ebx
.rast1_ok:

    ; T2: create
    mov eax, 5       ; owner
    mov edx, 1000    ; cpu_limit
    mov ecx, 4096    ; mem_limit
    call resource_accounting_create
    jnc .rast2_ok
    inc ebx
.rast2_ok:
    push eax         ; slot

    ; T3: charge (500 CPU, 1024 Mem)
    mov edx, 500
    mov ecx, 1024
    call resource_accounting_charge
    jnc .rast3_ok
    inc ebx
.rast3_ok:

    ; T4: read → cpu=500, mem=1024
    call resource_accounting_read
    cmp eax, 500
    je .rast4a_ok
    inc ebx
.rast4a_ok:
    cmp edx, 1024
    je .rast4b_ok
    inc ebx
.rast4b_ok:

    ; T5: charge 600 CPU → überschreitet 1000 → CF=1
    mov edx, 600
    mov ecx, 0
    call resource_accounting_charge
    jc .rast5_ok
    inc ebx
.rast5_ok:
    pop eax

    ; T6-T7: violations=1
    cmp dword [racct_violations], 1
    je .rast6_ok
    inc ebx
.rast6_ok:

    test ebx, ebx
    jnz .rastf
    clc
    pop ebx
    ret
.rastf:
    stc
    pop ebx
    ret

racct_ready:       dd 0
racct_next_id:     dd 0
racct_violations:  dd 0
align 4
racct_table:
    times RACCT_CAPACITY * RACCT_RECORD_SIZE db 0

; ===========================================================================
; NPSPEC-RESILIENCE-FAILOVER-0001 – Nova Failover Manager
; Registriert Primär/Backup-Paare. Erkennt Ausfall und aktiviert Backup.
; ===========================================================================

FO_CAPACITY          equ 4
FO_RECORD_SIZE       equ 24

FO_STATE_FREE        equ 0
FO_STATE_PRIMARY_OK  equ 1
FO_STATE_FAILED      equ 2
FO_STATE_FAILOVER    equ 3

FO_ID                equ 0
FO_STATE_OFF         equ 4
FO_PRIMARY           equ 8   ; ID/Bezeichner des Primärsystems
FO_BACKUP            equ 12  ; ID/Bezeichner des Backupsystems
FO_FAIL_TICK         equ 16  ; Tick des letzten Ausfalls
FO_SWITCHES          equ 20  ; Anzahl Failover-Wechsel

failover_initialize:
    mov edi, fo_table
    mov ecx, FO_CAPACITY * FO_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [fo_ready], 1
    mov dword [fo_next_id], 0
    mov dword [fo_total_switches], 0
    clc
    ret

; EAX=primary_id  EDX=backup_id → EAX=slot CF=0 / CF=1 voll
failover_register:
    push esi
    push ecx
    mov esi, fo_table
    xor ecx, ecx
.for_scan:
    cmp ecx, FO_CAPACITY
    jae .for_full
    cmp dword [esi + FO_STATE_OFF], FO_STATE_FREE
    je .for_found
    add esi, FO_RECORD_SIZE
    inc ecx
    jmp .for_scan
.for_found:
    push eax
    mov eax, [fo_next_id]
    inc dword [fo_next_id]
    mov [esi + FO_ID], eax
    pop eax
    mov dword [esi + FO_STATE_OFF], FO_STATE_PRIMARY_OK
    mov [esi + FO_PRIMARY], eax
    mov [esi + FO_BACKUP], edx
    mov dword [esi + FO_FAIL_TICK], 0
    mov dword [esi + FO_SWITCHES], 0
    mov eax, ecx
    clc
    pop ecx
    pop esi
    ret
.for_full:
    stc
    pop ecx
    pop esi
    ret

; EAX=slot → aktiviert Failover (Backup übernimmt) CF=0 / CF=1 invalid/schon failover
failover_trigger:
    push esi
    cmp eax, FO_CAPACITY
    jae .fot_fail
    imul esi, eax, FO_RECORD_SIZE
    add esi, fo_table
    cmp dword [esi + FO_STATE_OFF], FO_STATE_PRIMARY_OK
    jne .fot_fail
    mov dword [esi + FO_STATE_OFF], FO_STATE_FAILOVER
    push eax
    mov eax, [wd_global_tick]
    mov [esi + FO_FAIL_TICK], eax
    pop eax
    inc dword [esi + FO_SWITCHES]
    inc dword [fo_total_switches]
    clc
    pop esi
    ret
.fot_fail:
    stc
    pop esi
    ret

; EAX=slot → EAX=state  EDX=primary  ECX=backup  CF=0 / CF=1
failover_query:
    push esi
    cmp eax, FO_CAPACITY
    jae .foq_fail
    imul esi, eax, FO_RECORD_SIZE
    add esi, fo_table
    cmp dword [esi + FO_STATE_OFF], FO_STATE_FREE
    je .foq_fail
    mov eax, [esi + FO_STATE_OFF]
    mov edx, [esi + FO_PRIMARY]
    mov ecx, [esi + FO_BACKUP]
    clc
    pop esi
    ret
.foq_fail:
    stc
    pop esi
    ret

failover_self_test:
    push ebx
    push esi
    xor esi, esi
    cmp dword [fo_ready], 1
    je .fst1
    inc esi
.fst1:
    mov eax, 10   ; primary=10
    mov edx, 20   ; backup=20
    call failover_register
    jnc .fst2
    inc esi
.fst2:
    mov ebx, eax
    ; Trigger → Failover
    mov eax, ebx
    call failover_trigger
    jnc .fst3
    inc esi
.fst3:
    ; State muss FO_STATE_FAILOVER sein
    mov eax, ebx
    call failover_query
    jnc .fst4_cf
    inc esi
.fst4_cf:
    cmp eax, FO_STATE_FAILOVER
    je .fst4
    inc esi
.fst4:
    ; fo_total_switches=1
    cmp dword [fo_total_switches], 1
    je .fst5
    inc esi
.fst5:
    ; Nochmal trigger → CF=1 (nicht PRIMARY_OK)
    mov eax, ebx
    call failover_trigger
    jc .fst6
    inc esi
.fst6:
    test esi, esi
    jnz .fstf
    clc
    pop esi
    pop ebx
    ret
.fstf:
    stc
    pop esi
    pop ebx
    ret

fo_ready:          dd 0
fo_next_id:        dd 0
fo_total_switches: dd 0
align 4
fo_table:
    times FO_CAPACITY * FO_RECORD_SIZE db 0

; ===========================================================================
; NPSPEC-RESILIENCE-CONTAINMENT-0001 – Nova Fault Containment
; Isoliert fehlerhafte Subsysteme damit Fehler nicht auf andere überspringen.
; ===========================================================================

CONT_CAPACITY        equ 8
CONT_RECORD_SIZE     equ 20

CONT_STATE_FREE      equ 0
CONT_STATE_OK        equ 1
CONT_STATE_DEGRADED  equ 2
CONT_STATE_ISOLATED  equ 3

CONT_ID              equ 0
CONT_STATE_OFF       equ 4
CONT_SUBSYSTEM       equ 8
CONT_FAULT_COUNT     equ 12
CONT_THRESHOLD       equ 16

containment_initialize:
    mov edi, cont_table
    mov ecx, CONT_CAPACITY * CONT_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [cont_ready], 1
    mov dword [cont_next_id], 0
    mov dword [cont_isolated_count], 0
    clc
    ret

; EAX=subsystem_id  EDX=fault_threshold → EAX=slot CF=0/CF=1 voll
containment_register:
    push esi
    push ecx
    mov esi, cont_table
    xor ecx, ecx
.conr_scan:
    cmp ecx, CONT_CAPACITY
    jae .conr_full
    cmp dword [esi + CONT_STATE_OFF], CONT_STATE_FREE
    je .conr_found
    add esi, CONT_RECORD_SIZE
    inc ecx
    jmp .conr_scan
.conr_found:
    push eax
    mov eax, [cont_next_id]
    inc dword [cont_next_id]
    mov [esi + CONT_ID], eax
    pop eax
    mov dword [esi + CONT_STATE_OFF], CONT_STATE_OK
    mov [esi + CONT_SUBSYSTEM], eax
    mov dword [esi + CONT_FAULT_COUNT], 0
    mov [esi + CONT_THRESHOLD], edx
    mov eax, ecx
    clc
    pop ecx
    pop esi
    ret
.conr_full:
    stc
    pop ecx
    pop esi
    ret

; EAX=slot → meldet Fehler, bei Threshold→ISOLATED; CF=0/CF=1
containment_report_fault:
    push esi
    cmp eax, CONT_CAPACITY
    jae .corf_fail
    imul esi, eax, CONT_RECORD_SIZE
    add esi, cont_table
    cmp dword [esi + CONT_STATE_OFF], CONT_STATE_FREE
    je .corf_fail
    cmp dword [esi + CONT_STATE_OFF], CONT_STATE_ISOLATED
    je .corf_already
    inc dword [esi + CONT_FAULT_COUNT]
    ; Schwellwert-Check
    mov edx, [esi + CONT_FAULT_COUNT]
    cmp edx, [esi + CONT_THRESHOLD]
    jb .corf_degraded
    mov dword [esi + CONT_STATE_OFF], CONT_STATE_ISOLATED
    inc dword [cont_isolated_count]
    clc
    pop esi
    ret
.corf_degraded:
    mov dword [esi + CONT_STATE_OFF], CONT_STATE_DEGRADED
    clc
    pop esi
    ret
.corf_already:
    clc
    pop esi
    ret
.corf_fail:
    stc
    pop esi
    ret

; EAX=slot → prüft ob isoliert; CF=0 ok / CF=1 isoliert
containment_check:
    push esi
    cmp eax, CONT_CAPACITY
    jae .coch_fail
    imul esi, eax, CONT_RECORD_SIZE
    add esi, cont_table
    cmp dword [esi + CONT_STATE_OFF], CONT_STATE_ISOLATED
    je .coch_isolated
    clc
    pop esi
    ret
.coch_isolated:
    stc
    pop esi
    ret
.coch_fail:
    stc
    pop esi
    ret

containment_self_test:
    push ebx
    push esi
    xor esi, esi
    cmp dword [cont_ready], 1
    je .cst1
    inc esi
.cst1:
    ; Register subsystem 1, threshold=2
    mov eax, 1
    mov edx, 2
    call containment_register
    jnc .cst2
    inc esi
.cst2:
    mov ebx, eax
    ; 1. Fault → DEGRADED
    mov eax, ebx
    call containment_report_fault
    jnc .cst3
    inc esi
.cst3:
    ; Check → noch nicht isoliert
    mov eax, ebx
    call containment_check
    jnc .cst4
    inc esi
.cst4:
    ; 2. Fault → ISOLATED (threshold=2)
    mov eax, ebx
    call containment_report_fault
    jnc .cst5
    inc esi
.cst5:
    ; Check → isoliert (CF=1)
    mov eax, ebx
    call containment_check
    jc .cst6
    inc esi
.cst6:
    cmp dword [cont_isolated_count], 1
    je .cst7
    inc esi
.cst7:
    test esi, esi
    jnz .cstf
    clc
    pop esi
    pop ebx
    ret
.cstf:
    stc
    pop esi
    pop ebx
    ret

cont_ready:          dd 0
cont_next_id:        dd 0
cont_isolated_count: dd 0
align 4
cont_table:
    times CONT_CAPACITY * CONT_RECORD_SIZE db 0

; ===========================================================================
; NPSPEC-RESILIENCE-CHECKPOINT-0001 – Nova Checkpoint / Restore
; Speichert Schnappschuss eines Task-Zustands für Wiederherstellung.
; (Kernel-seitig: Register-Satz + Stack-Pointer als leichtgewichtiger CP)
; ===========================================================================

CP_CAPACITY          equ 4
CP_RECORD_SIZE       equ 48

CP_STATE_FREE        equ 0
CP_STATE_VALID       equ 1
CP_STATE_RESTORED    equ 2

CP_ID                equ 0
CP_STATE_OFF         equ 4
CP_OWNER             equ 8
CP_TICK              equ 12
CP_EAX               equ 16
CP_EBX               equ 20
CP_ECX               equ 24
CP_EDX               equ 28
CP_ESI               equ 32
CP_EDI               equ 36
CP_EBP               equ 40
CP_ESP               equ 44

checkpoint_initialize:
    mov edi, cp_table
    mov ecx, CP_CAPACITY * CP_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [cp_ready], 1
    mov dword [cp_next_id], 0
    mov dword [cp_total_saves], 0
    mov dword [cp_total_restores], 0
    clc
    ret

; Speichert aktuellen Registerkontext; EAX=owner_id → EAX=slot CF=0/CF=1 voll
checkpoint_save:
    push esi
    push ecx
    push edi
    ; Slot suchen
    mov edi, cp_table
    xor ecx, ecx
.cps_scan:
    cmp ecx, CP_CAPACITY
    jae .cps_full
    cmp dword [edi + CP_STATE_OFF], CP_STATE_FREE
    je .cps_found
    cmp dword [edi + CP_STATE_OFF], CP_STATE_RESTORED
    je .cps_found   ; Wiederverwendbar
    add edi, CP_RECORD_SIZE
    inc ecx
    jmp .cps_scan
.cps_found:
    push eax
    push edx
    mov edx, [cp_next_id]
    inc dword [cp_next_id]
    mov [edi + CP_ID], edx
    pop edx
    pop eax
    mov dword [edi + CP_STATE_OFF], CP_STATE_VALID
    mov [edi + CP_OWNER], eax
    push eax
    mov eax, [wd_global_tick]
    mov [edi + CP_TICK], eax
    pop eax
    ; Register sichern (EAX=owner, daher aus Stack)
    mov [edi + CP_EBX], ebx
    mov [edi + CP_ECX], ecx
    mov [edi + CP_EDX], edx
    mov [edi + CP_ESI], esi
    push eax
    lea eax, [esp + 16]   ; ESP vor dem call
    mov [edi + CP_ESP], eax
    pop eax
    mov [edi + CP_EBP], ebp
    xor eax, eax           ; EAX im Checkpoint = 0 (kann nicht self-save)
    mov [edi + CP_EAX], eax
    push eax
    mov eax, ecx
    inc dword [cp_total_saves]
    pop eax
    clc
    pop edi
    pop ecx
    pop esi
    ret
.cps_full:
    stc
    pop edi
    pop ecx
    pop esi
    ret

; EAX=slot → markiert Checkpoint als wiederhergestellt (symbolisch); CF=0/CF=1
checkpoint_restore:
    push esi
    cmp eax, CP_CAPACITY
    jae .cpr_fail
    imul esi, eax, CP_RECORD_SIZE
    add esi, cp_table
    cmp dword [esi + CP_STATE_OFF], CP_STATE_VALID
    jne .cpr_fail
    mov dword [esi + CP_STATE_OFF], CP_STATE_RESTORED
    inc dword [cp_total_restores]
    clc
    pop esi
    ret
.cpr_fail:
    stc
    pop esi
    ret

checkpoint_self_test:
    push ebx
    push esi
    xor esi, esi
    cmp dword [cp_ready], 1
    je .cpst1
    inc esi
.cpst1:
    ; Save mit owner=42
    mov eax, 42
    call checkpoint_save
    jnc .cpst2
    inc esi
.cpst2:
    mov ebx, eax   ; slot
    ; Prüfe cp_total_saves=1
    cmp dword [cp_total_saves], 1
    je .cpst3
    inc esi
.cpst3:
    ; Restore
    mov eax, ebx
    call checkpoint_restore
    jnc .cpst4
    inc esi
.cpst4:
    cmp dword [cp_total_restores], 1
    je .cpst5
    inc esi
.cpst5:
    ; Nochmaliger Restore → CF=1 (nicht mehr VALID)
    mov eax, ebx
    call checkpoint_restore
    jc .cpst6
    inc esi
.cpst6:
    test esi, esi
    jnz .cpstf
    clc
    pop esi
    pop ebx
    ret
.cpstf:
    stc
    pop esi
    pop ebx
    ret

cp_ready:           dd 0
cp_next_id:         dd 0
cp_total_saves:     dd 0
cp_total_restores:  dd 0
align 4
cp_table:
    times CP_CAPACITY * CP_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Zentraler I/O-Scheduler: Deadline vor effektiver Prioritaet, danach FIFO.
; Eine begrenzte Aging-Stufe verhindert dauerhafte Starvation.
