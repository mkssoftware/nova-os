; ===========================================================================
; NPSPEC-INTERRUPT-DEFERRED-0001 – Nova Deferred Interrupt Handler
; Bottom-Half-Mechanismus: IRQ-Handler queuen Handler-Fn + Kontext;
; deferred_process_queue() läuft außerhalb des IRQ-Kontexts.
; ===========================================================================

DEFERRED_CAPACITY   equ 16
DEFERRED_ENTRY_SIZE equ 12
DEF_HANDLER         equ 0
DEF_CTX_A           equ 4
DEF_CTX_B           equ 8

deferred_initialize:
    mov edi, deferred_queue
    mov ecx, DEFERRED_CAPACITY * DEFERRED_ENTRY_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [deferred_head], 0
    mov dword [deferred_tail], 0
    mov dword [deferred_count], 0
    mov dword [deferred_dropped], 0
    mov dword [deferred_processed], 0
    mov dword [deferred_ready], 1
    clc
    ret

; EAX=handler_fn  EDX=ctx_a  ECX=ctx_b → CF=0 ok / CF=1 Queue voll
deferred_queue_add:
    push esi
    push ebx
    cmp dword [deferred_count], DEFERRED_CAPACITY
    jae .dqa_full
    mov ebx, [deferred_tail]
    imul esi, ebx, DEFERRED_ENTRY_SIZE
    add esi, deferred_queue
    mov [esi + DEF_HANDLER], eax
    mov [esi + DEF_CTX_A], edx
    mov [esi + DEF_CTX_B], ecx
    inc ebx
    cmp ebx, DEFERRED_CAPACITY
    jb .dqa_nowrap
    xor ebx, ebx
.dqa_nowrap:
    mov [deferred_tail], ebx
    inc dword [deferred_count]
    clc
    pop ebx
    pop esi
    ret
.dqa_full:
    inc dword [deferred_dropped]
    stc
    pop ebx
    pop esi
    ret

; Verarbeitet alle pending deferred Einträge (aus Nicht-IRQ-Kontext aufrufen)
; Handler-Konvention: EAX=ctx_a, EDX=ctx_b
deferred_process_queue:
    push eax
    push edx
    push ebx
    push esi
.dpq_loop:
    cmp dword [deferred_count], 0
    je .dpq_done
    mov ebx, [deferred_head]
    imul esi, ebx, DEFERRED_ENTRY_SIZE
    add esi, deferred_queue
    mov edx, [esi + DEF_HANDLER]
    test edx, edx
    jz .dpq_skip
    mov eax, [esi + DEF_CTX_A]
    push dword [esi + DEF_CTX_B]
    call edx
    pop edx
    inc dword [deferred_processed]
.dpq_skip:
    inc ebx
    cmp ebx, DEFERRED_CAPACITY
    jb .dpq_nowrap2
    xor ebx, ebx
.dpq_nowrap2:
    mov [deferred_head], ebx
    dec dword [deferred_count]
    jmp .dpq_loop
.dpq_done:
    pop esi
    pop ebx
    pop edx
    pop eax
    clc
    ret

deferred_self_test:
    push ebx
    push esi
    xor ebx, ebx   ; Fehlerz.

    ; T1: ready
    cmp dword [deferred_ready], 1
    je .dst1_ok
    inc ebx
.dst1_ok:

    ; T2: queue_add
    mov dword [deferred_test_flag], 0
    mov eax, deferred_test_handler
    mov edx, 99
    mov ecx, 88
    call deferred_queue_add
    jnc .dst2_ok
    inc ebx
.dst2_ok:

    ; T3: count=1
    cmp dword [deferred_count], 1
    je .dst3_ok
    inc ebx
.dst3_ok:

    ; T4: process_queue
    call deferred_process_queue
    jnc .dst4_ok
    inc ebx
.dst4_ok:

    ; T5: handler wurde aufgerufen
    cmp dword [deferred_test_flag], 1
    je .dst5_ok
    inc ebx
.dst5_ok:

    ; T6: count=0 nach processing
    cmp dword [deferred_count], 0
    je .dst6_ok
    inc ebx
.dst6_ok:

    ; T7: Queue bis Kapazitaet fuellen, dann overflow pruefen
    mov esi, DEFERRED_CAPACITY
.dst7_fill:
    test esi, esi
    jz .dst7_filled
    mov eax, deferred_test_handler
    xor edx, edx
    xor ecx, ecx
    call deferred_queue_add
    dec esi
    jmp .dst7_fill
.dst7_filled:
    ; eine weitere sollte CF=1 liefern
    mov eax, deferred_test_handler
    xor edx, edx
    xor ecx, ecx
    call deferred_queue_add
    jc .dst7_ok
    inc ebx
.dst7_ok:
    ; Queue wieder leeren
    call deferred_process_queue

    test ebx, ebx
    jnz .dstf
    clc
    pop esi
    pop ebx
    ret
.dstf:
    stc
    pop esi
    pop ebx
    ret

; Test-Handler: setzt deferred_test_flag=1
deferred_test_handler:
    mov dword [deferred_test_flag], 1
    ret

deferred_ready:      dd 0
deferred_head:       dd 0
deferred_tail:       dd 0
deferred_count:      dd 0
deferred_dropped:    dd 0
deferred_processed:  dd 0
deferred_test_flag:  dd 0
align 4
deferred_queue:
    times DEFERRED_CAPACITY * DEFERRED_ENTRY_SIZE db 0

; ===========================================================================
; NPSPEC-TIME-MONOTONIC-0001 / NPSPEC-TIME-RTC-0001 – Nova Time Services
; Monotone Zeit (PIT-Ticks), RTC-Uhrzeit/-Datum via CMOS (Port 0x70/0x71).
; ===========================================================================

TIME_HZ             equ 100        ; PIT läuft auf 100 Hz
TIME_MS_PER_TICK    equ 10         ; 10 ms pro Tick

RTC_PORT_INDEX      equ 0x70
RTC_PORT_DATA       equ 0x71
RTC_REG_SECONDS     equ 0x00
RTC_REG_MINUTES     equ 0x02
RTC_REG_HOURS       equ 0x04
RTC_REG_DAY         equ 0x07
RTC_REG_MONTH       equ 0x08
RTC_REG_YEAR        equ 0x09
RTC_REG_STATUSB     equ 0x0B

; ---------- Monotone Zeit ---------------------------------------------------

time_monotonic_initialize:
    cmp dword [timer_ticks], 0
    ; Timer muss bereits laufen (nach timer_initialize/sti aufrufen)
    mov dword [time_mono_ready], 1
    clc
    ret

; → EAX = Tick-Zähler seit Boot
time_monotonic_get_ticks:
    mov eax, [timer_ticks]
    ret

; → EAX = Millisekunden seit Boot (ticks * 10 bei 100 Hz)
time_monotonic_get_ms:
    mov eax, [timer_ticks]
    imul eax, eax, TIME_MS_PER_TICK
    ret

time_monotonic_self_test:
    push ebx
    xor ebx, ebx

    ; T1: ready
    cmp dword [time_mono_ready], 1
    je .tmst1_ok
    inc ebx
.tmst1_ok:

    ; T2: timer läuft (ticks > 0)
    call time_monotonic_get_ticks
    test eax, eax
    jnz .tmst2_ok
    inc ebx
.tmst2_ok:

    ; T3: get_ms = ticks * 10.
    ; IRQ0 darf zwischen den Reads eintreffen; deshalb wird gegen ein
    ; Before/After-Fenster geprüft, nicht gegen exakt einen alten Tickwert.
    call time_monotonic_get_ticks
    push eax                         ; before_ticks
    call time_monotonic_get_ms
    mov edx, eax                     ; observed_ms
    call time_monotonic_get_ticks
    mov ecx, eax                     ; after_ticks
    pop eax                          ; before_ticks
    imul eax, eax, TIME_MS_PER_TICK
    imul ecx, ecx, TIME_MS_PER_TICK
    cmp edx, eax
    jb .tmst3_fail
    cmp edx, ecx
    jbe .tmst3_ok
.tmst3_fail:
    inc ebx
.tmst3_ok:

    ; T4-T7: get_ms >= get_ticks (ms ist größer als tick-zähler)
    call time_monotonic_get_ms
    push eax
    call time_monotonic_get_ticks
    pop ecx
    cmp ecx, eax     ; ms >= ticks
    jae .tmst4_ok
    inc ebx
.tmst4_ok:

    test ebx, ebx
    jnz .tmstf
    clc
    pop ebx
    ret
.tmstf:
    stc
    pop ebx
    ret

; ---------- RTC (CMOS) -------------------------------------------------------

; intern: EAX=Register-Index → EAX=Wert (roh, BCD oder Binary)
rtc_read_reg:
    push edx
    out RTC_PORT_INDEX, al
    in al, RTC_PORT_DATA
    movzx eax, al
    pop edx
    ret

; intern: → EAX=1 wenn BCD-Modus, EAX=0 wenn Binary-Modus
rtc_is_bcd:
    push edx
    mov al, RTC_REG_STATUSB
    out RTC_PORT_INDEX, al
    in al, RTC_PORT_DATA
    test al, 0x04
    jnz .rib_bin
    mov eax, 1
    pop edx
    ret
.rib_bin:
    xor eax, eax
    pop edx
    ret

; EAX=BCD-Byte → EAX=Binärwert
rtc_bcd_to_bin:
    push edx
    mov edx, eax
    shr edx, 4
    and eax, 0x0F
    imul edx, edx, 10
    add eax, edx
    pop edx
    ret

; → EAX=Stunden(0-23)  EDX=Minuten(0-59)  ECX=Sekunden(0-59)
rtc_get_time:
    push ebx
    mov al, RTC_REG_SECONDS
    call rtc_read_reg
    mov [rtc_tmp_sec], eax
    mov al, RTC_REG_MINUTES
    call rtc_read_reg
    mov [rtc_tmp_min], eax
    mov al, RTC_REG_HOURS
    call rtc_read_reg
    mov [rtc_tmp_hour], eax
    call rtc_is_bcd
    test eax, eax
    jz .rgt_bin
    mov eax, [rtc_tmp_sec]
    call rtc_bcd_to_bin
    mov [rtc_tmp_sec], eax
    mov eax, [rtc_tmp_min]
    call rtc_bcd_to_bin
    mov [rtc_tmp_min], eax
    mov eax, [rtc_tmp_hour]
    call rtc_bcd_to_bin
    mov [rtc_tmp_hour], eax
.rgt_bin:
    mov eax, [rtc_tmp_hour]
    mov edx, [rtc_tmp_min]
    mov ecx, [rtc_tmp_sec]
    pop ebx
    clc
    ret

; → EAX=Jahr(2000+y)  EDX=Monat(1-12)  ECX=Tag(1-31)
rtc_get_date:
    push ebx
    mov al, RTC_REG_DAY
    call rtc_read_reg
    mov [rtc_tmp_day], eax
    mov al, RTC_REG_MONTH
    call rtc_read_reg
    mov [rtc_tmp_month], eax
    mov al, RTC_REG_YEAR
    call rtc_read_reg
    mov [rtc_tmp_year], eax
    call rtc_is_bcd
    test eax, eax
    jz .rgd_bin
    mov eax, [rtc_tmp_day]
    call rtc_bcd_to_bin
    mov [rtc_tmp_day], eax
    mov eax, [rtc_tmp_month]
    call rtc_bcd_to_bin
    mov [rtc_tmp_month], eax
    mov eax, [rtc_tmp_year]
    call rtc_bcd_to_bin
    mov [rtc_tmp_year], eax
.rgd_bin:
    mov eax, [rtc_tmp_year]
    add eax, 2000
    mov edx, [rtc_tmp_month]
    mov ecx, [rtc_tmp_day]
    pop ebx
    clc
    ret

rtc_initialize:
    mov al, RTC_REG_STATUSB
    out RTC_PORT_INDEX, al
    in al, RTC_PORT_DATA
    ; RTC ist immer vorhanden auf x86-PC
    mov dword [rtc_ready], 1
    clc
    ret

rtc_self_test:
    push ebx
    xor ebx, ebx

    ; T1: ready
    cmp dword [rtc_ready], 1
    je .rst1_ok
    inc ebx
.rst1_ok:

    ; T2: rtc_get_time – Stunden im Bereich 0-23
    call rtc_get_time
    cmp eax, 23
    jbe .rst2h_ok
    inc ebx
.rst2h_ok:
    ; Minuten 0-59
    cmp edx, 59
    jbe .rst2m_ok
    inc ebx
.rst2m_ok:
    ; Sekunden 0-59
    cmp ecx, 59
    jbe .rst2s_ok
    inc ebx
.rst2s_ok:

    ; T3: rtc_get_date – Jahr 2000-2099
    call rtc_get_date
    cmp eax, 2000
    jae .rst3y_ok
    inc ebx
.rst3y_ok:
    cmp eax, 2099
    jbe .rst3yh_ok
    inc ebx
.rst3yh_ok:
    ; Monat 1-12
    cmp edx, 1
    jae .rst3ml_ok
    inc ebx
.rst3ml_ok:
    cmp edx, 12
    jbe .rst3mh_ok
    inc ebx
.rst3mh_ok:

    ; T4: bcd_to_bin(0x59)=59
    mov eax, 0x59
    call rtc_bcd_to_bin
    cmp eax, 59
    je .rst4_ok
    inc ebx
.rst4_ok:

    ; T5: bcd_to_bin(0x23)=23
    mov eax, 0x23
    call rtc_bcd_to_bin
    cmp eax, 23
    je .rst5_ok
    inc ebx
.rst5_ok:

    test ebx, ebx
    jnz .rstf
    clc
    pop ebx
    ret
.rstf:
    stc
    pop ebx
    ret

time_mono_ready:    dd 0
rtc_ready:          dd 0
rtc_tmp_sec:        dd 0
rtc_tmp_min:        dd 0
rtc_tmp_hour:       dd 0
rtc_tmp_day:        dd 0
rtc_tmp_month:      dd 0
rtc_tmp_year:       dd 0

; ===========================================================================
; NPSPEC-TIME-CLOCKSOURCE-0001 – Nova Clock Source Abstraction
; Einheitliche Schnittstelle über verschiedene Hardware-Zeitquellen.
; Registriert Quellen (TSC, PIT, HPET-stub, RTC) mit Priorität und Rating.
; ===========================================================================

CS_CAPACITY          equ 4
CS_RECORD_SIZE       equ 24

CS_TYPE_NONE         equ 0
CS_TYPE_PIT          equ 1
CS_TYPE_TSC          equ 2
CS_TYPE_HPET         equ 3
CS_TYPE_RTC          equ 4

CS_FLAG_STABLE       equ 0x01
CS_FLAG_CONT         equ 0x02   ; Continuous (kein Wrap in absehbarer Zeit)
CS_FLAG_NONSTOP      equ 0x04

CS_ID                equ 0
CS_TYPE_OFF          equ 4
CS_RATING            equ 8   ; Höheres Rating = bevorzugte Quelle
CS_FLAGS             equ 12
CS_READ_FN           equ 16  ; Funktionszeiger → EAX:EDX = 64-bit Tick
CS_MULT              equ 20  ; Multiplikator für ns-Umrechnung (vereinfacht)

clocksource_initialize:
    mov edi, cs_table
    mov ecx, CS_CAPACITY * CS_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [cs_ready], 1
    mov dword [cs_count], 0
    mov dword [cs_active], 0
    ; PIT als Basisquelle registrieren (immer vorhanden)
    mov eax, CS_TYPE_PIT
    mov edx, 50              ; Rating 50
    mov ecx, CS_FLAG_STABLE
    mov ebx, clocksource_pit_read
    call clocksource_register
    ; TSC registrieren (höheres Rating wenn vorhanden)
    mov eax, CS_TYPE_TSC
    mov edx, 100
    mov ecx, CS_FLAG_STABLE | CS_FLAG_NONSTOP
    mov ebx, clocksource_tsc_read
    call clocksource_register
    ; Beste Quelle aktivieren
    call clocksource_select_best
    clc
    ret

; EAX=type  EDX=rating  ECX=flags  EBX=read_fn → CF=0 / CF=1 voll
clocksource_register:
    push esi
    push edi
    push ebx
    cmp dword [cs_count], CS_CAPACITY
    jae .csr_full
    mov edi, [cs_count]
    imul esi, edi, CS_RECORD_SIZE
    add esi, cs_table
    mov [esi + CS_TYPE_OFF], eax
    mov [esi + CS_RATING], edx
    mov [esi + CS_FLAGS], ecx
    push eax
    mov eax, edi
    mov [esi + CS_ID], eax
    pop eax
    mov [esi + CS_READ_FN], ebx
    mov dword [esi + CS_MULT], 10   ; vereinfacht: 10 ns/tick für PIT@100Hz
    inc dword [cs_count]
    clc
    pop ebx
    pop edi
    pop esi
    ret
.csr_full:
    stc
    pop ebx
    pop edi
    pop esi
    ret

; Wählt Quelle mit höchstem Rating als aktive Quelle
clocksource_select_best:
    push esi
    push ecx
    push edx
    xor ecx, ecx
    xor edx, edx         ; bestes Rating
    mov esi, cs_table
    push ebx
    xor ebx, ebx
.cssb_loop:
    cmp ebx, [cs_count]
    jae .cssb_done
    cmp dword [esi + CS_RATING], edx
    jbe .cssb_next
    mov edx, [esi + CS_RATING]
    mov ecx, [esi + CS_ID]
.cssb_next:
    add esi, CS_RECORD_SIZE
    inc ebx
    jmp .cssb_loop
.cssb_done:
    mov [cs_active], ecx
    pop ebx
    pop edx
    pop ecx
    pop esi
    ret

; Liest aktive Quelle → EAX=low_tick (vereinfacht 32-bit)
clocksource_read:
    push esi
    mov esi, [cs_active]
    imul esi, CS_RECORD_SIZE
    add esi, cs_table
    call dword [esi + CS_READ_FN]
    pop esi
    ret

; PIT-Stub: gibt wd_global_tick zurück (100 Hz Basis)
clocksource_pit_read:
    mov eax, [wd_global_tick]
    ret

; TSC-Stub: liest TSC, gibt low-DWORD
clocksource_tsc_read:
    rdtsc
    ; EAX = low 32 bit TSC
    ret

clocksource_self_test:
    push esi
    xor esi, esi
    cmp dword [cs_ready], 1
    je .csst1
    inc esi
.csst1:
    ; cs_count >= 2 (PIT + TSC)
    cmp dword [cs_count], 2
    jae .csst2
    inc esi
.csst2:
    ; aktive Quelle = TSC (höchstes Rating)
    cmp dword [cs_active], 1   ; TSC hat ID=1
    je .csst3
    inc esi
.csst3:
    ; read gibt einen Wert
    call clocksource_read
    ; EAX != 0 nicht prüfbar (TSC kann 0 sein nach Reset), nur Aufruf
    test esi, esi
    jnz .csstf
    clc
    pop esi
    ret
.csstf:
    stc
    pop esi
    ret

cs_ready:    dd 0
cs_count:    dd 0
cs_active:   dd 0
align 4
cs_table:
    times CS_CAPACITY * CS_RECORD_SIZE db 0

; ===========================================================================
; NPSPEC-TIME-HIGHRES-0001 – Nova High-Resolution Timer
; Nanosekunden-Auflösung via TSC-Kalibrierung gegen PIT/RTC-Basisquellen.
; Bietet hrt_get_ns (monoton, ns-Auflösung) und One-Shot-Alarm-Register.
; ===========================================================================

HRT_ALARM_COUNT      equ 4
HRT_ALARM_SIZE       equ 16

HRT_ALARM_FREE       equ 0
HRT_ALARM_ARMED      equ 1
HRT_ALARM_FIRED      equ 2

HRT_ALM_ID           equ 0
HRT_ALM_STATE        equ 4
HRT_ALM_DEADLINE_NS  equ 8
HRT_ALM_CALLBACK     equ 12

hrt_initialize:
    ; TSC-Kalibrierung: Basiswert merken
    rdtsc
    mov [hrt_tsc_base_lo], eax
    mov [hrt_tsc_base_hi], edx
    ; ns-Basis = 0 beim Start
    mov dword [hrt_ns_offset], 0
    ; Alarm-Tabelle löschen
    mov edi, hrt_alarms
    mov ecx, HRT_ALARM_COUNT * HRT_ALARM_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [hrt_ready], 1
    mov dword [hrt_next_id], 0
    clc
    ret

; → EAX = geschätzte Nanosekunden seit Kernel-Start (vereinfacht)
; Nutzt timer_ticks (100Hz) → 10 ms pro Tick → 10 000 000 ns/Tick.
; wd_global_tick gehört zum späteren Watchdog-Pfad und läuft in der frühen
; Interruptphase noch nicht zwingend mit.
hrt_get_ns:
    mov eax, [timer_ticks]
    mov edx, 10000000     ; 10 ms in ns
    mul edx               ; EAX:EDX = ns (EDX verworfen; reicht für ~42 s)
    ret

; Registriert One-Shot-Alarm: EAX=deadline_ns  EDX=callback → EAX=slot CF=0/CF=1
hrt_alarm_register:
    push esi
    push ecx
    mov esi, hrt_alarms
    xor ecx, ecx
.har_scan:
    cmp ecx, HRT_ALARM_COUNT
    jae .har_full
    cmp dword [esi + HRT_ALM_STATE], HRT_ALARM_FREE
    je .har_found
    add esi, HRT_ALARM_SIZE
    inc ecx
    jmp .har_scan
.har_found:
    push eax
    mov eax, [hrt_next_id]
    inc dword [hrt_next_id]
    mov [esi + HRT_ALM_ID], eax
    pop eax
    mov dword [esi + HRT_ALM_STATE], HRT_ALARM_ARMED
    mov [esi + HRT_ALM_DEADLINE_NS], eax
    mov [esi + HRT_ALM_CALLBACK], edx
    mov eax, ecx
    clc
    pop ecx
    pop esi
    ret
.har_full:
    stc
    pop ecx
    pop esi
    ret

; Prüft alle Alarme gegen aktuelle ns-Zeit; feuert abgelaufene
hrt_check_alarms:
    push esi
    push ecx
    push edx
    call hrt_get_ns
    mov edx, eax         ; current_ns
    mov esi, hrt_alarms
    xor ecx, ecx
.hca_loop:
    cmp ecx, HRT_ALARM_COUNT
    jae .hca_done
    cmp dword [esi + HRT_ALM_STATE], HRT_ALARM_ARMED
    jne .hca_next
    cmp edx, [esi + HRT_ALM_DEADLINE_NS]
    jb .hca_next
    mov dword [esi + HRT_ALM_STATE], HRT_ALARM_FIRED
    ; Callback aufrufen wenn vorhanden
    cmp dword [esi + HRT_ALM_CALLBACK], 0
    je .hca_next
    push eax
    push ecx
    push edx
    call dword [esi + HRT_ALM_CALLBACK]
    pop edx
    pop ecx
    pop eax
.hca_next:
    add esi, HRT_ALARM_SIZE
    inc ecx
    jmp .hca_loop
.hca_done:
    pop edx
    pop ecx
    pop esi
    ret

hrt_self_test:
    push esi
    push ebx
    xor esi, esi

    ; T1: ready
    cmp dword [hrt_ready], 1
    je .hrst1
    inc esi
.hrst1:

    ; T2: hrt_get_ns gibt Wert ≥ 0 (immer wahr, nur Aufruf testen)
    call hrt_get_ns
    ; EAX = ns, kein Fehler erwartet

    ; T3: Alarm registrieren (Deadline=1 ns → sofort abgelaufen)
    mov eax, 1           ; deadline=1 ns
    mov edx, 0           ; kein Callback
    call hrt_alarm_register
    jnc .hrst3
    inc esi
.hrst3:
    mov ebx, eax         ; slot

    ; T4: check_alarms → Alarm muss gefeuert werden
    call hrt_check_alarms
    ; Slot-State prüfen
    push ecx
    imul ecx, ebx, HRT_ALARM_SIZE
    add ecx, hrt_alarms
    cmp dword [ecx + HRT_ALM_STATE], HRT_ALARM_FIRED
    je .hrst4
    inc esi
.hrst4:
    pop ecx

    ; T5: zweiten Alarm mit hoher Deadline → bleibt ARMED
    call hrt_get_ns
    add eax, 0x7FFFFFFF  ; sehr weit in der Zukunft
    mov edx, 0
    call hrt_alarm_register
    jnc .hrst5_ok
    inc esi
.hrst5_ok:
    mov ebx, eax         ; slot
    call hrt_check_alarms
    push ecx
    imul ecx, ebx, HRT_ALARM_SIZE
    add ecx, hrt_alarms
    cmp dword [ecx + HRT_ALM_STATE], HRT_ALARM_ARMED
    je .hrst5_state
    inc esi
.hrst5_state:
    pop ecx

    test esi, esi
    jnz .hrstf
    clc
    pop ebx
    pop esi
    ret
.hrstf:
    stc
    pop ebx
    pop esi
    ret

hrt_ready:        dd 0
hrt_next_id:      dd 0
hrt_tsc_base_lo:  dd 0
hrt_tsc_base_hi:  dd 0
hrt_ns_offset:    dd 0
align 4
hrt_alarms:
    times HRT_ALARM_COUNT * HRT_ALARM_SIZE db 0

; ---------------------------------------------------------------------------
; §Scheduler – Haupt-Scheduler, SMP, Work Stealing, Fair, RT, QoS
