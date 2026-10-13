; ---------------------------------------------------------------------------
; Nova Time Subsystem
; NPSPEC-TIME-ARCH-0001         - Architektur (Monoton / Wall / RTC)
; NPSPEC-TIME-CLOCKSOURCE-0001  - Clock-Source-Registry
; NPSPEC-TIME-CLOCKDOMAIN-0001  - Clock-Domains
; NPSPEC-TIME-MONOTONIC-0001    - Monotone Zeit
; NPSPEC-TIME-RTC-0001          - Real-Time-Clock (CMOS)
; NPSPEC-TIME-HIGHRES-0001      - Hochaufloesende Zeitstempel (TSC)
; NPSPEC-TIME-HIGHRESTIMER-0001 - High-Resolution-Timer
; NPSPEC-TIME-TIMER-0001        - Logische Timer-Infrastruktur
; NPSPEC-TIME-DEADLINE-0001     - Deadline-Timer
; NPSPEC-TIME-PERIODIC-0001     - Periodische Timer
; NPSPEC-TIME-COALESCING-0001   - Timer-Coalescing
; NPSPEC-TIME-TICKLESS-0001     - Tickless-Idle
; NPSPEC-TIME-CIVIL-0001        - Buergerliche Zeit (Unix-Epoch -> Datum)
; NPSPEC-TIME-CIVILSCHEDULE-0001- Buergerlicher Zeitplan
; NPSPEC-TIME-CALIBRATION-0001  - Kalibrierung (TSC-Frequenz)
; NPSPEC-TIME-NTP-0001          - NTP-Synchronisation (Stub)
; NPSPEC-TIME-PTP-0001          - PTP-Synchronisation (Stub)
; NPSPEC-TIME-SYNCHRONIZATION-0001 - Allgemeine Zeitsynchronisation
; NPSPEC-TIME-NAMESPACE-0001    - Zeitnamespaces
; NPSPEC-TIME-VIRTUAL-0001      - Virtuelle Zeit
; NPSPEC-TIME-MEDIA-0001        - Medien-Timing
; NPSPEC-TIME-TRUSTED-0001      - Vertrauenswuerdige Zeit
; NPSPEC-TIME-LEAPSECOND-0001   - Schaltsekunden
; NPSPEC-TIME-PERSISTENTTIMER-0001 - Persistente Timer
; NPSPEC-TIME-DETERMINISTIC-0001- Deterministische Zeitgeber
; NPSPEC-TIME-DISTRIBUTED-0001  - Verteilte Zeit (Stub)
; NPSPEC-TIME-INTROSPECTION-0001- Introspection
; ---------------------------------------------------------------------------

; Clock-Source-IDs (NPSPEC-TIME-CLOCKSOURCE-0001)
CLKSRC_NONE          equ 0
CLKSRC_PIT           equ 1    ; Programmable Interval Timer (100 Hz)
CLKSRC_TSC           equ 2    ; Time Stamp Counter (hochaufloesend)
CLKSRC_HPET          equ 3    ; High Precision Event Timer
CLKSRC_RTC           equ 4    ; CMOS Real-Time Clock
CLKSRC_VIRTUAL       equ 5    ; Virtualisierter Zaehler

; Clock-Domain-Typen (NPSPEC-TIME-CLOCKDOMAIN-0001)
CLKDOM_SYSTEM        equ 0    ; Monotone Systemzeit
CLKDOM_WALL          equ 1    ; Wall-Clock (UTC-basiert)
CLKDOM_LOCAL         equ 2    ; Lokale Zeit (Zeitzone)
CLKDOM_MEDIA         equ 3    ; Medien-Zeitachse
CLKDOM_VIRTUAL       equ 4    ; Virtuelle Zeit (Container)

; Timer-Typen (NPSPEC-TIME-TIMER-0001)
TIMER_TYPE_ONESHOT   equ 0    ; Einmalig
TIMER_TYPE_PERIODIC  equ 1    ; Periodisch (NPSPEC-TIME-PERIODIC-0001)
TIMER_TYPE_DEADLINE  equ 2    ; Absoluter Termin (NPSPEC-TIME-DEADLINE-0001)
TIMER_TYPE_COALESCED equ 3    ; Zusammengefasst (NPSPEC-TIME-COALESCING-0001)
TIMER_TYPE_PERSIST   equ 4    ; Persistent (NPSPEC-TIME-PERSISTENTTIMER-0001)

; Timer-Zustande
TIMER_ST_FREE        equ 0
TIMER_ST_ARMED       equ 1
TIMER_ST_FIRED       equ 2
TIMER_ST_CANCELLED   equ 3

; Timer-Record (48 Bytes, NPSPEC-TIME-TIMER-0001)
TIMER_REC_SIZE       equ 48
TR_ID                equ 0    ; Timer-ID
TR_OWNER             equ 4    ; Prozess/App-ID
TR_TYPE              equ 8    ; TIMER_TYPE_*
TR_STATE             equ 12   ; TIMER_ST_*
TR_DOMAIN            equ 16   ; CLKDOM_*
TR_DEADLINE_LO       equ 20   ; Ablaufzeit (100-Hz-Ticks, lo)
TR_DEADLINE_HI       equ 24   ; Ablaufzeit (hi)
TR_PERIOD            equ 28   ; Periode in Ticks (fuer PERIODIC)
TR_TOLERANCE         equ 32   ; Coalescing-Toleranz in Ticks
TR_CALLBACK          equ 36   ; Funktionszeiger oder 0
TR_FIRE_COUNT        equ 40   ; Wie oft ausgeloest
TR_RESERVED          equ 44

TIMER_CAPACITY       equ 16   ; max. 16 logische Timer

; Clock-Source-Record (32 Bytes, NPSPEC-TIME-CLOCKSOURCE-0001)
CLKSRC_REC_SIZE      equ 32
CS_ID                equ 0
CS_TYPE              equ 4    ; CLKSRC_*
CS_FREQ_HZ           equ 8    ; Nenn-Frequenz in Hz
CS_FREQ_FRAC         equ 12   ; Feinkorrektur (ppm, NPSPEC-TIME-CALIBRATION-0001)
CS_FLAGS             equ 16   ; 0x1=verfuegbar, 0x2=aktiv, 0x4=vertrauenswuerdig
CS_SKEW_PPB          equ 20   ; Gangabweichung ppb (NTP/PTP)
CS_RESERVED          equ 24

CLKSRC_CAPACITY      equ 4

; Zeitsynchronisations-Status (NPSPEC-TIME-SYNCHRONIZATION-0001)
TIMESYNC_NONE        equ 0    ; Keine Synchronisation
TIMESYNC_RTC         equ 1    ; Nur CMOS-RTC
TIMESYNC_NTP         equ 2    ; NTP synchronisiert
TIMESYNC_PTP         equ 3    ; PTP synchronisiert
TIMESYNC_TRUSTED     equ 4    ; Vertrauenswuerdige Quelle (NPSPEC-TIME-TRUSTED-0001)

; Civil-Time-Record (24 Bytes, NPSPEC-TIME-CIVIL-0001)
CIVIL_SIZE           equ 24
CT_YEAR              equ 0    ; Jahr (z.B. 2025)
CT_MONTH             equ 4    ; Monat 1..12
CT_DAY               equ 8    ; Tag 1..31
CT_HOUR              equ 12   ; Stunde 0..23
CT_MINUTE            equ 16   ; Minute 0..59
CT_SECOND            equ 20   ; Sekunde 0..60 (Schaltsekunde: 60)

; Leapsecond-Status (NPSPEC-TIME-LEAPSECOND-0001)
LEAP_STATE_NONE      equ 0
LEAP_STATE_PENDING   equ 1    ; Schaltsekunde angekuendigt
LEAP_STATE_ACTIVE    equ 2    ; Aktuell in Schaltsekunde

; Time-Namespace-Record (16 Bytes, NPSPEC-TIME-NAMESPACE-0001)
TIMENS_SIZE          equ 16
TN_ID                equ 0
TN_OFFSET_LO         equ 4    ; Offset zur Systemzeit (Ticks)
TN_OFFSET_HI         equ 8
TN_SCALE             equ 12   ; Skalierungsfaktor (fuer virtuelle Zeit)

TIMENS_CAPACITY      equ 4

; ---------------------------------------------------------------------------
; time_subsystem_initialize
; Registriert vorhandene Clock-Sources und initialisiert Timer-Tabelle.
; CF=0 Erfolg
; ---------------------------------------------------------------------------
time_subsystem_initialize:
    ; Timer-Tabelle loeschen
    mov edi, time_timers
    xor eax, eax
    mov ecx, (TIMER_CAPACITY * TIMER_REC_SIZE) / 4
    rep stosd
    ; Clock-Source-Tabelle loeschen
    mov edi, time_clksrc_table
    mov ecx, (CLKSRC_CAPACITY * CLKSRC_REC_SIZE) / 4
    rep stosd
    ; Time-Namespace-Tabelle loeschen
    mov edi, time_namespaces
    mov ecx, (TIMENS_CAPACITY * TIMENS_SIZE) / 4
    rep stosd

    mov dword [time_timer_count], 0
    mov dword [time_clksrc_count], 0
    mov dword [time_sync_state], TIMESYNC_RTC
    mov dword [time_leap_state], LEAP_STATE_NONE
    mov dword [time_ntp_offset_lo], 0
    mov dword [time_ntp_offset_hi], 0
    mov dword [time_tsc_freq_khz], 0
    mov dword [time_ns_count], 0

    ; PIT als erste Clock-Source registrieren
    mov eax, CLKSRC_PIT
    mov ebx, 100        ; 100 Hz
    mov ecx, 0x01       ; verfuegbar
    call time_clksrc_register

    ; TSC als zweite Clock-Source registrieren (verfuegbar + aktiv + vertrauenswuerdig)
    mov eax, CLKSRC_TSC
    mov ebx, 0          ; Frequenz wird per Kalibrierung ermittelt
    mov ecx, 0x07       ; verfuegbar + aktiv + vertrauenswuerdig
    call time_clksrc_register

    ; TSC-Frequenz kalibrieren (NPSPEC-TIME-CALIBRATION-0001)
    call time_calibrate_tsc

    ; Default-Zeitnamespace (Systemraum)
    call time_ns_create_default

    mov dword [time_subsys_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; time_clksrc_register: EAX=type, EBX=freq_hz, ECX=flags
; CF=0 Erfolg
; ---------------------------------------------------------------------------
time_clksrc_register:
    cmp dword [time_clksrc_count], CLKSRC_CAPACITY
    jae .full
    push edi
    ; Freien Slot
    mov edi, time_clksrc_table
    push ecx
    mov ecx, CLKSRC_CAPACITY
.scan:
    test ecx, ecx
    jz .no_slot
    cmp dword [edi + CS_TYPE], CLKSRC_NONE
    je .got_slot
    add edi, CLKSRC_REC_SIZE
    dec ecx
    jmp .scan
.no_slot:
    pop ecx
    pop edi
    stc
    ret
.got_slot:
    pop ecx
    mov dword [edi + CS_ID], eax
    mov dword [edi + CS_TYPE], eax
    mov dword [edi + CS_FREQ_HZ], ebx
    mov dword [edi + CS_FREQ_FRAC], 0
    mov dword [edi + CS_FLAGS], ecx
    mov dword [edi + CS_SKEW_PPB], 0
    inc dword [time_clksrc_count]
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; time_calibrate_tsc
; Bestimmt TSC-Frequenz durch Messung ueber PIT-Ticks.
; (NPSPEC-TIME-CALIBRATION-0001)
; ---------------------------------------------------------------------------
time_calibrate_tsc:
    ; Einfache Kalibrierung: RDTSC vor und nach 10ms Wartezeit
    ; (10ms = 1 PIT-Tick bei 100 Hz)
    push eax
    push edx
    push ecx
    rdtsc
    mov [time_tsc_cal_start_lo], eax
    mov [time_tsc_cal_start_hi], edx

    ; 10ms warten: ~1 PIT-Tick
    mov eax, [timer_ticks]
    inc eax
.wait:
    cmp [timer_ticks], eax
    jb .wait

    rdtsc
    sub eax, [time_tsc_cal_start_lo]
    sbb edx, [time_tsc_cal_start_hi]
    ; EAX = TSC-Delta fuer ~10ms -> * 100 = Hz
    ; Vereinfacht: * 100 / 1000 = kHz
    ; Overflow-sicher: wenn Delta < 4M, * 100 passt in 32 Bit
    mov ecx, 100
    mul ecx          ; EDX:EAX = delta * 100 (Hz-Schaetzung)
    ; Ergebnis in kHz: / 1000
    mov ecx, 1000
    div ecx
    mov [time_tsc_freq_khz], eax

    ; TSC-ClockSource-Frequenz aktualisieren
    mov edi, time_clksrc_table
    mov ecx, CLKSRC_CAPACITY
.update:
    test ecx, ecx
    jz .done_cal
    cmp dword [edi + CS_TYPE], CLKSRC_TSC
    je .set_freq
    add edi, CLKSRC_REC_SIZE
    dec ecx
    jmp .update
.set_freq:
    ; Frequenz in kHz * 1000 = Hz (kann ueberlaufen, Stub-Wert)
    mov ebx, eax
    imul ebx, 1000
    mov [edi + CS_FREQ_HZ], ebx
.done_cal:
    pop ecx
    pop edx
    pop eax
    ret

; ---------------------------------------------------------------------------
; time_get_monotonic_ticks: EDX:EAX = 64-Bit Tick-Zaehler (100 Hz)
; (NPSPEC-TIME-MONOTONIC-0001)
; ---------------------------------------------------------------------------
time_get_monotonic_ticks:
    mov eax, [timer_ticks]
    xor edx, edx
    ret

; ---------------------------------------------------------------------------
; time_get_tsc: EDX:EAX = TSC-Wert
; (NPSPEC-TIME-HIGHRES-0001)
; ---------------------------------------------------------------------------
time_get_tsc:
    rdtsc
    ret

; ---------------------------------------------------------------------------
; time_tsc_to_ns: EDX:EAX = TSC-Delta -> EAX = Nanosekunden (Stub)
; Verwendet time_tsc_freq_khz fuer Umrechnung.
; ---------------------------------------------------------------------------
time_tsc_to_ns:
    ; ns = delta * 1000000 / freq_khz
    ; Stub: falls freq_khz=0, liefert 0
    push ecx
    mov ecx, [time_tsc_freq_khz]
    test ecx, ecx
    jz .zero
    ; EAX = EAX * 1000 / ecx (vereinfacht, nur lo-Wort)
    push edx
    mov edx, 0
    imul eax, 1000
    div ecx
    pop edx
    pop ecx
    ret
.zero:
    xor eax, eax
    pop ecx
    ret

; ---------------------------------------------------------------------------
; time_read_rtc: Liest CMOS-RTC -> CT_* in [EDI]
; (NPSPEC-TIME-RTC-0001)
; EDI = Zeiger auf CIVIL_SIZE-Byte-Puffer
; ---------------------------------------------------------------------------
time_read_rtc:
    push eax
    push ecx
    ; CMOS-Register: 0x00=Sekunde, 0x02=Minute, 0x04=Stunde
    ;               0x07=Tag, 0x08=Monat, 0x09=Jahr(BCD)
    ; Sekunde
    mov al, 0x00
    out 0x70, al
    in  al, 0x71
    call time_bcd_to_bin
    mov [edi + CT_SECOND], eax

    ; Minute
    mov al, 0x02
    out 0x70, al
    in  al, 0x71
    call time_bcd_to_bin
    mov [edi + CT_MINUTE], eax

    ; Stunde
    mov al, 0x04
    out 0x70, al
    in  al, 0x71
    call time_bcd_to_bin
    mov [edi + CT_HOUR], eax

    ; Tag
    mov al, 0x07
    out 0x70, al
    in  al, 0x71
    call time_bcd_to_bin
    mov [edi + CT_DAY], eax

    ; Monat
    mov al, 0x08
    out 0x70, al
    in  al, 0x71
    call time_bcd_to_bin
    mov [edi + CT_MONTH], eax

    ; Jahr (2-stellig BCD + 2000)
    mov al, 0x09
    out 0x70, al
    in  al, 0x71
    call time_bcd_to_bin
    add eax, 2000
    mov [edi + CT_YEAR], eax

    pop ecx
    pop eax
    ret

; AL = BCD-Wert -> EAX = binaer
time_bcd_to_bin:
    push ecx
    movzx eax, al
    mov ecx, eax
    shr ecx, 4       ; High-Nibble
    imul ecx, 10
    and eax, 0x0F    ; Low-Nibble
    add eax, ecx
    pop ecx
    ret

; ---------------------------------------------------------------------------
; time_timer_arm: EBX=owner, ECX=type, EDX=deadline_ticks, EAX=period_ticks
; Rueckgabe: EAX=timer_id (CF=0) oder CF=1
; (NPSPEC-TIME-TIMER-0001, NPSPEC-TIME-DEADLINE-0001, NPSPEC-TIME-PERIODIC-0001)
; ---------------------------------------------------------------------------
time_timer_arm:
    push esi
    push edi
    cmp dword [time_timer_count], TIMER_CAPACITY
    jae .full
    ; Freien Slot
    mov edi, time_timers
    push ecx
    mov ecx, TIMER_CAPACITY
.scan:
    test ecx, ecx
    jz .no_slot
    cmp dword [edi + TR_STATE], TIMER_ST_FREE
    je .got_slot
    add edi, TIMER_REC_SIZE
    dec ecx
    jmp .scan
.no_slot:
    pop ecx
    pop edi
    pop esi
    stc
    ret
.got_slot:
    pop ecx
    ; Neue Timer-ID
    inc dword [time_next_timer_id]
    mov esi, [time_next_timer_id]
    mov [edi + TR_ID], esi
    mov [edi + TR_OWNER], ebx
    mov [edi + TR_TYPE], ecx
    mov [edi + TR_STATE], TIMER_ST_ARMED
    mov [edi + TR_DOMAIN], dword CLKDOM_SYSTEM
    mov [edi + TR_DEADLINE_LO], edx
    mov [edi + TR_DEADLINE_HI], dword 0
    mov [edi + TR_PERIOD], eax
    mov [edi + TR_TOLERANCE], dword 0
    mov [edi + TR_CALLBACK], dword 0
    mov [edi + TR_FIRE_COUNT], dword 0
    inc dword [time_timer_count]
    mov eax, esi
    pop edi
    pop esi
    clc
    ret
.full:
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; time_timer_cancel: EAX=timer_id
; CF=0 Erfolg, CF=1 nicht gefunden
; ---------------------------------------------------------------------------
time_timer_cancel:
    push ecx
    push esi
    mov esi, time_timers
    mov ecx, TIMER_CAPACITY
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + TR_ID], eax
    je .found
    add esi, TIMER_REC_SIZE
    dec ecx
    jmp .scan
.found:
    mov dword [esi + TR_STATE], TIMER_ST_CANCELLED
    dec dword [time_timer_count]
    pop esi
    pop ecx
    clc
    ret
.not_found:
    pop esi
    pop ecx
    stc
    ret

; ---------------------------------------------------------------------------
; time_timer_tick
; Wird aus dem PIT-IRQ-Handler aufgerufen.
; Prueft alle bewaffneten Timer auf Ablauf.
; (NPSPEC-TIME-COALESCING-0001, NPSPEC-TIME-TICKLESS-0001)
; ---------------------------------------------------------------------------
time_timer_tick:
    push eax
    push ecx
    push esi
    mov esi, time_timers
    mov ecx, TIMER_CAPACITY
    mov eax, [timer_ticks]
.loop:
    test ecx, ecx
    jz .done
    cmp dword [esi + TR_STATE], TIMER_ST_ARMED
    jne .next
    ; Deadline erreicht? (mit Coalescing-Toleranz)
    mov edx, [esi + TR_DEADLINE_LO]
    sub edx, [esi + TR_TOLERANCE]
    cmp eax, edx
    jb .next
    ; Timer ausloesen
    inc dword [esi + TR_FIRE_COUNT]
    ; Callback aufrufen falls gesetzt
    cmp dword [esi + TR_CALLBACK], 0
    je .no_cb
    push eax
    call dword [esi + TR_CALLBACK]
    pop eax
.no_cb:
    ; Periodisch: neue Deadline setzen
    cmp dword [esi + TR_TYPE], TIMER_TYPE_PERIODIC
    jne .oneshot
    mov edx, [esi + TR_PERIOD]
    add edx, eax
    mov [esi + TR_DEADLINE_LO], edx
    jmp .next
.oneshot:
    mov dword [esi + TR_STATE], TIMER_ST_FIRED
    dec dword [time_timer_count]
.next:
    add esi, TIMER_REC_SIZE
    dec ecx
    jmp .loop
.done:
    pop esi
    pop ecx
    pop eax
    ret

; ---------------------------------------------------------------------------
; time_ns_create_default
; Legt den Standard-Zeitnamespace (ID=1, Offset=0, Scale=1) an.
; (NPSPEC-TIME-NAMESPACE-0001)
; ---------------------------------------------------------------------------
time_ns_create_default:
    mov edi, time_namespaces
    mov dword [edi + TN_ID], 1
    mov dword [edi + TN_OFFSET_LO], 0
    mov dword [edi + TN_OFFSET_HI], 0
    mov dword [edi + TN_SCALE], 0x10000   ; 1.0 als Fixpunkt 16.16
    inc dword [time_ns_count]
    ret

; ---------------------------------------------------------------------------
; time_ntp_adjust: EAX=offset_ms (signed)
; Justiert Wallclock-Offset (NPSPEC-TIME-NTP-0001, NPSPEC-TIME-SYNCHRONIZATION-0001).
; Keine echte Slewing-Implementierung, nur State-Update.
; ---------------------------------------------------------------------------
time_ntp_adjust:
    mov [time_ntp_offset_lo], eax
    mov dword [time_sync_state], TIMESYNC_NTP
    clc
    ret

; ---------------------------------------------------------------------------
; time_ptp_adjust: EAX=offset_ns (signed)
; PTP-Offset-Stub (NPSPEC-TIME-PTP-0001).
; ---------------------------------------------------------------------------
time_ptp_adjust:
    mov [time_ptp_offset], eax
    mov dword [time_sync_state], TIMESYNC_PTP
    clc
    ret

; ---------------------------------------------------------------------------
; time_media_get_position: EDX:EAX = Medien-Position in 100-ns-Einheiten
; (NPSPEC-TIME-MEDIA-0001)
; Einfache Ableitung aus Systemticks.
; ---------------------------------------------------------------------------
time_media_get_position:
    call time_get_tsc
    ; TSC-Delta * 1000 / freq_khz = Mikrosekunden * 10 = 100ns-Einheiten
    push ecx
    mov ecx, [time_tsc_freq_khz]
    test ecx, ecx
    jz .zero
    push edx
    ; Naehert sich an: EAX * 10 / freq_khz
    imul eax, 10
    xor edx, edx
    div ecx
    pop edx
    pop ecx
    ret
.zero:
    xor eax, eax
    xor edx, edx
    pop ecx
    ret

; ---------------------------------------------------------------------------
; time_subsystem_self_test
; ---------------------------------------------------------------------------
time_subsystem_self_test:
    push esi
    xor esi, esi

    cmp dword [time_subsys_ready], 1
    je .t2
    inc esi
.t2:
    ; TSC-Kalibrierung hat Ergebnis geliefert?
    cmp dword [time_tsc_freq_khz], 0
    jne .t3
    inc esi
.t3:
    ; Timer armen und canceln
    mov ebx, 1          ; owner_id
    mov ecx, TIMER_TYPE_ONESHOT
    mov edx, 0xFFFF     ; weit in der Zukunft
    xor eax, eax
    call time_timer_arm
    jnc .t4
    inc esi
    jmp .done
.t4:
    ; Armierten Timer canceln
    call time_timer_cancel
    jnc .t5
    inc esi
.t5:
    ; RTC lesen (nur Aufruf, keine Wertpruefung)
    push edi
    mov edi, time_rtc_buf
    call time_read_rtc
    pop edi

.done:
    test esi, esi
    jz .ok
    pop esi
    stc
    ret
.ok:
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; Statische Daten
; ---------------------------------------------------------------------------
align 4
time_subsys_ready:      dd 0
time_timer_count:       dd 0
time_clksrc_count:      dd 0
time_ns_count:          dd 0
time_next_timer_id:     dd 0
time_sync_state:        dd TIMESYNC_NONE
time_leap_state:        dd LEAP_STATE_NONE
time_tsc_freq_khz:      dd 0
time_ntp_offset_lo:     dd 0
time_ntp_offset_hi:     dd 0
time_ptp_offset:        dd 0
time_tsc_cal_start_lo:  dd 0
time_tsc_cal_start_hi:  dd 0

align 4
time_timers:
    times TIMER_CAPACITY * TIMER_REC_SIZE db 0

align 4
time_clksrc_table:
    times CLKSRC_CAPACITY * CLKSRC_REC_SIZE db 0

align 4
time_namespaces:
    times TIMENS_CAPACITY * TIMENS_SIZE db 0

align 4
time_rtc_buf:
    times CIVIL_SIZE db 0

; ---------------------------------------------------------------------------
; Legacy-kompatible Einsprungpunkte (werden von entry32.asm aufgerufen)
; Diese Wrapper delegieren an time_subsystem_initialize.
; ---------------------------------------------------------------------------

; Deferred-Work-Queue (Stub – Arbeitslisten-Infrastruktur)
deferred_work_capacity   equ 8
deferred_work_size       equ 16   ; DW_FN=0, DW_ARG=4, DW_STATE=8, DW_RESERVED=12

deferred_initialize:
    mov edi, deferred_work_table
    xor eax, eax
    mov ecx, (deferred_work_capacity * deferred_work_size) / 4
    rep stosd
    mov dword [deferred_count], 0
    clc
    ret

deferred_self_test:
    cmp dword [deferred_count], 0
    jne .fail
    clc
    ret
.fail:
    stc
    ret

; Monotone Zeit – delegiert an time_subsystem_initialize
time_monotonic_initialize:
    call time_subsystem_initialize
    ret

time_monotonic_self_test:
    call time_subsystem_self_test
    ret

; RTC lesen und in Civil-Buffer schreiben
rtc_initialize:
    mov edi, time_rtc_buf
    call time_read_rtc
    clc
    ret

rtc_self_test:
    ; Jahr muss >= 2020 sein (CMOS-Sanity-Check)
    cmp dword [time_rtc_buf + CT_YEAR], 2020
    jb .bad
    clc
    ret
.bad:
    stc
    ret

; Clock-Source-Verwaltung
clocksource_initialize:
    ; Schon durch time_subsystem_initialize erledigt, falls ready=1
    cmp dword [time_subsys_ready], 1
    je .ok
    call time_subsystem_initialize
.ok:
    clc
    ret

clocksource_self_test:
    cmp dword [time_clksrc_count], 0
    je .fail
    clc
    ret
.fail:
    stc
    ret

; High-Resolution-Timer
hrt_initialize:
    ; HRT basiert auf TSC, keine eigene Hardware-Init noetig
    clc
    ret

hrt_self_test:
    ; TSC-Frequenz muss nach Kalibrierung bekannt sein
    cmp dword [time_tsc_freq_khz], 0
    je .fail
    clc
    ret
.fail:
    stc
    ret

; ---------------------------------------------------------------------------
; Statische Daten fuer Deferred-Work
; ---------------------------------------------------------------------------
align 4
deferred_count: dd 0

align 4
deferred_work_table:
    times deferred_work_capacity * deferred_work_size db 0
