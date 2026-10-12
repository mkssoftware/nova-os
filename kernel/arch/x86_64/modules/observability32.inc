; ===========================================================================
; §104 – Kernel Diagnostics Framework 1.0 (NPSPEC-KERNEL-0104)
; ===========================================================================

; ---------------------------------------------------------------------------
; Diagnosestufen (§9)
; ---------------------------------------------------------------------------
DIAG_LEVEL_TRACE    equ 0
DIAG_LEVEL_DEBUG    equ 1
DIAG_LEVEL_INFO     equ 2
DIAG_LEVEL_NOTICE   equ 3
DIAG_LEVEL_WARNING  equ 4
DIAG_LEVEL_ERROR    equ 5
DIAG_LEVEL_CRITICAL equ 6
DIAG_LEVEL_FATAL    equ 7

; Health-Zustände (§35)
DIAG_HEALTH_UNKNOWN    equ 0
DIAG_HEALTH_HEALTHY    equ 1
DIAG_HEALTH_DEGRADED   equ 2
DIAG_HEALTH_UNHEALTHY  equ 3
DIAG_HEALTH_FAILED     equ 4
DIAG_HEALTH_RECOVERING equ 5
DIAG_HEALTH_OFFLINE    equ 6

; Built-in Diagnosequellen-IDs (§7)
DIAG_SRC_KERNEL     equ 1
DIAG_SRC_KOG        equ 2
DIAG_SRC_EVBUS      equ 3
DIAG_SRC_UOBJ       equ 4
DIAG_SRC_CAP        equ 5
DIAG_SRC_DIAG       equ 6

; Pool-Grenzen und Deskriptorgrößen
DIAG_MAX_SOURCES        equ 16
DIAG_SRC_SIZE           equ 32
DIAG_LOG_MAX_RECORDS    equ 64
DIAG_LOG_RECORD_SIZE    equ 16
DIAG_MAX_HEALTH         equ 16
DIAG_HEALTH_SIZE        equ 4

; Diagnosequellen-Deskriptor-Offsets (§7)
DIAG_OFF_SRC_ID         equ 0    ; dd  source_id
DIAG_OFF_SRC_FLAGS      equ 4    ; dd  flags
DIAG_OFF_SRC_LEVEL      equ 8    ; db  default_level
DIAG_OFF_SRC_PRESENT    equ 9    ; db  1 = belegt
                                 ; dw  Padding bei Offset 10
DIAG_OFF_SRC_NAME       equ 12   ; 20 × db  null-terminierter Name

; Log-Record-Offsets (§10, §13)
DIAG_OFF_LOG_LEVEL      equ 0    ; db  DIAG_LEVEL_*
DIAG_OFF_LOG_SRC_ID     equ 1    ; db  source_id (Byte-Wert)
DIAG_OFF_LOG_FLAGS      equ 2    ; dw  Flags
DIAG_OFF_LOG_EVENT_ID   equ 4    ; dd  Event-ID
DIAG_OFF_LOG_SEQ        equ 8    ; dd  Sequenznummer
DIAG_OFF_LOG_VALUE      equ 12   ; dd  optionaler u32-Wert

; Health-Tabellen-Offsets (§35)
DIAG_OFF_HLTH_SRC_ID    equ 0    ; db  source_id
DIAG_OFF_HLTH_STATE     equ 1    ; db  DIAG_HEALTH_*
DIAG_OFF_HLTH_PRESENT   equ 2    ; db  1 = belegt
                                 ; db  Padding

; Event-Typ-ID (§44, nach CAP-Events 9–11)
NP_EVENT_DIAG_HEALTH_CHANGED equ 12

; Fehlercodes (§68)
NP_ERR_DIAG_DISABLED    equ -70
NP_ERR_DIAG_FILTERED    equ -71
NP_ERR_DIAG_BUFFER_FULL equ -72
NP_ERR_DIAG_TOO_LARGE   equ -73
NP_ERR_DIAG_NOT_FOUND   equ -74
NP_ERR_DIAG_LIMIT       equ -75

; ---------------------------------------------------------------------------
; Compile-Zeit-Invarianten
; ---------------------------------------------------------------------------
%if DIAG_SRC_SIZE != 32
    %error "DIAG_SRC_SIZE muss 32 Bytes sein"
%endif
%if DIAG_LOG_RECORD_SIZE != 16
    %error "DIAG_LOG_RECORD_SIZE muss 16 Bytes sein"
%endif
%if DIAG_HEALTH_SIZE != 4
    %error "DIAG_HEALTH_SIZE muss 4 Bytes sein"
%endif

; ---------------------------------------------------------------------------
; diag_find_source – Diagnosequelle anhand ID suchen (intern)
; EAX=src_id → CF=0 EBX=ptr / CF=1 EAX=NP_ERR_DIAG_NOT_FOUND
; ---------------------------------------------------------------------------
diag_find_source:
    push edi
    push ecx
    mov edi, diag_sources
    xor ecx, ecx
.dfs_scan:
    cmp ecx, DIAG_MAX_SOURCES
    jae .dfs_miss
    cmp byte [edi + DIAG_OFF_SRC_PRESENT], 0
    je .dfs_next
    cmp dword [edi + DIAG_OFF_SRC_ID], eax
    je .dfs_hit
.dfs_next:
    add edi, DIAG_SRC_SIZE
    inc ecx
    jmp .dfs_scan
.dfs_hit:
    mov ebx, edi
    pop ecx
    pop edi
    clc
    ret
.dfs_miss:
    mov eax, NP_ERR_DIAG_NOT_FOUND
    pop ecx
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; diag_register_source – Diagnosequelle registrieren (§8)
; EAX=src_id, EBX=flags, ECX=default_level
; CF=0 OK / CF=1 Fehler (doppelt/voll)
; ---------------------------------------------------------------------------
diag_register_source:
    push esi
    push edi

    mov [diag_tmp_src_id], eax
    mov [diag_tmp_flags],  ebx
    mov [diag_tmp_level],  ecx

    ; Doppelte Registrierung ablehnen
    call diag_find_source      ; EAX=src_id → EBX=ptr / CF=1 nicht gefunden
    jnc .drs_dup               ; CF=0 = bereits vorhanden

    ; Freien Slot suchen
    mov edi, diag_sources
    xor ecx, ecx
.drs_scan:
    cmp ecx, DIAG_MAX_SOURCES
    jae .drs_full
    cmp byte [edi + DIAG_OFF_SRC_PRESENT], 0
    je .drs_found
    add edi, DIAG_SRC_SIZE
    inc ecx
    jmp .drs_scan
.drs_found:
    mov eax, [diag_tmp_src_id]
    mov dword [edi + DIAG_OFF_SRC_ID],  eax
    mov eax, [diag_tmp_flags]
    mov [edi + DIAG_OFF_SRC_FLAGS], eax
    mov al, [diag_tmp_level]   ; Byte-Zugriff: low byte von ECX-Speicher
    mov [edi + DIAG_OFF_SRC_LEVEL], al
    mov byte [edi + DIAG_OFF_SRC_PRESENT], 1

    pop edi
    pop esi
    clc
    ret
.drs_dup:
    pop edi
    pop esi
    stc
    ret
.drs_full:
    mov eax, NP_ERR_DIAG_LIMIT
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; diag_log – Log-Eintrag in Ringpuffer schreiben (§13, §21)
; EAX=src_id, EBX=log_level, ECX=event_id, EDX=value
; CF=0 OK / CF=1 EAX=Fehler (gefiltert/nicht gefunden)
; ---------------------------------------------------------------------------
diag_log:
    push esi
    push edi

    mov [diag_tmp_src_id], eax
    mov [diag_tmp_flags],  ebx   ; log_level
    mov [diag_tmp_event],  ecx
    mov [diag_tmp_value],  edx

    ; Quelle prüfen
    call diag_find_source        ; EAX=src_id → EBX=ptr
    jc .dl_not_found

    ; Filterung: log_level < source.default_level → gefiltert (§27)
    movzx eax, byte [ebx + DIAG_OFF_SRC_LEVEL]
    cmp [diag_tmp_flags], eax
    jb .dl_filtered

    ; Overflow-Erkennung (§24): wenn total >= MAX → Overwrite, lost++
    mov eax, [diag_log_total]
    cmp eax, DIAG_LOG_MAX_RECORDS
    jb .dl_no_overflow
    lock inc dword [diag_log_lost]
.dl_no_overflow:

    ; Slot-Adresse berechnen (Ringpuffer, §22)
    mov edi, [diag_log_write]
    imul esi, edi, DIAG_LOG_RECORD_SIZE
    add esi, diag_log_buf

    ; Record schreiben
    mov al, [diag_tmp_flags]             ; log_level → byte
    mov [esi + DIAG_OFF_LOG_LEVEL], al
    mov al, [diag_tmp_src_id]            ; src_id → byte
    mov [esi + DIAG_OFF_LOG_SRC_ID], al
    mov word [esi + DIAG_OFF_LOG_FLAGS], 0
    mov eax, [diag_tmp_event]
    mov [esi + DIAG_OFF_LOG_EVENT_ID], eax
    mov eax, [diag_log_seq]
    inc dword [diag_log_seq]
    mov [esi + DIAG_OFF_LOG_SEQ], eax
    mov eax, [diag_tmp_value]
    mov [esi + DIAG_OFF_LOG_VALUE], eax

    ; Schreibzeiger zirkulär vorrücken
    mov eax, edi
    inc eax
    cmp eax, DIAG_LOG_MAX_RECORDS
    jb .dl_no_wrap
    xor eax, eax
.dl_no_wrap:
    mov [diag_log_write], eax
    lock inc dword [diag_log_total]
    lock inc dword [diag_stat_logged]

    pop edi
    pop esi
    clc
    ret

.dl_not_found:
    mov eax, NP_ERR_DIAG_NOT_FOUND
    pop edi
    pop esi
    stc
    ret
.dl_filtered:
    lock inc dword [diag_stat_filtered]
    mov eax, NP_ERR_DIAG_FILTERED
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; diag_health_report – Health-Zustand eines Subsystems setzen (§37)
; EAX=src_id, EBX=health_state (DIAG_HEALTH_*)
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
diag_health_report:
    push esi
    push edi

    mov [diag_tmp_src_id], eax
    mov [diag_tmp_flags],  ebx   ; health_state

    ; Quelle muss registriert sein
    call diag_find_source
    jc .dhr_not_found

    ; Gesundheitseintrag suchen oder anlegen
    mov edi, diag_health_tab
    xor ecx, ecx
.dhr_scan:
    cmp ecx, DIAG_MAX_HEALTH
    jae .dhr_full
    cmp byte [edi + DIAG_OFF_HLTH_PRESENT], 0
    je .dhr_new                    ; freier Slot
    movzx eax, byte [edi + DIAG_OFF_HLTH_SRC_ID]
    cmp eax, [diag_tmp_src_id]
    je .dhr_update                 ; bereits vorhanden
    add edi, DIAG_HEALTH_SIZE
    inc ecx
    jmp .dhr_scan
.dhr_new:
    mov al, [diag_tmp_src_id]
    mov [edi + DIAG_OFF_HLTH_SRC_ID], al
    mov byte [edi + DIAG_OFF_HLTH_PRESENT], 1
.dhr_update:
    mov al, [diag_tmp_flags]
    mov [edi + DIAG_OFF_HLTH_STATE], al

    ; NP_EVENT_DIAG_HEALTH_CHANGED publizieren (§44)
    mov eax, NP_EVENT_DIAG_HEALTH_CHANGED
    mov ecx, [diag_tmp_src_id]
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish             ; CF ignoriert

    pop edi
    pop esi
    clc
    ret
.dhr_not_found:
    pop edi
    pop esi
    stc
    ret
.dhr_full:
    mov eax, NP_ERR_DIAG_LIMIT
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; diag_health_get – Health-Zustand abfragen
; EAX=src_id → CF=0 EAX=DIAG_HEALTH_* / CF=1 EAX=NP_ERR_DIAG_NOT_FOUND
; ---------------------------------------------------------------------------
diag_health_get:
    push edi
    push ecx
    mov edi, diag_health_tab
    xor ecx, ecx
.dhg_scan:
    cmp ecx, DIAG_MAX_HEALTH
    jae .dhg_miss
    cmp byte [edi + DIAG_OFF_HLTH_PRESENT], 0
    je .dhg_next
    movzx ebx, byte [edi + DIAG_OFF_HLTH_SRC_ID]
    cmp ebx, eax
    je .dhg_hit
.dhg_next:
    add edi, DIAG_HEALTH_SIZE
    inc ecx
    jmp .dhg_scan
.dhg_hit:
    movzx eax, byte [edi + DIAG_OFF_HLTH_STATE]
    pop ecx
    pop edi
    clc
    ret
.dhg_miss:
    mov eax, NP_ERR_DIAG_NOT_FOUND
    pop ecx
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; diag_initialize – Pools leeren, Quellen und Health registrieren (§63)
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
%macro diag_reg_source 3           ; src_id, flags, default_level
    mov eax, %1
    mov ebx, %2
    mov ecx, %3
    call diag_register_source
    jc .dinit_fail
%endmacro

%macro diag_set_health 2           ; src_id, health_state
    mov eax, %1
    mov ebx, %2
    call diag_health_report
    jc .dinit_fail
%endmacro

diag_initialize:
    push esi
    push edi
    push ecx

    ; Pools leeren
    mov edi, diag_sources
    xor eax, eax
    mov ecx, (DIAG_MAX_SOURCES * DIAG_SRC_SIZE) / 4
    rep stosd
    mov edi, diag_log_buf
    mov ecx, (DIAG_LOG_MAX_RECORDS * DIAG_LOG_RECORD_SIZE) / 4
    rep stosd
    mov edi, diag_health_tab
    mov ecx, (DIAG_MAX_HEALTH * DIAG_HEALTH_SIZE) / 4
    rep stosd
    mov dword [diag_log_write],     0
    mov dword [diag_log_total],     0
    mov dword [diag_log_lost],      0
    mov dword [diag_log_seq],       0
    mov dword [diag_stat_logged],   0
    mov dword [diag_stat_filtered], 0
    mov dword [diag_stat_dropped],  0

    ; evbus-Schema für Health-Änderungen (§44)
    mov eax, NP_EVENT_DIAG_HEALTH_CHANGED
    mov ecx, NP_EVENT_CLASS_STATE
    xor edx, edx
    mov esi, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    call evbus_register_schema
    jc .dinit_fail

    ; Built-in Diagnosequellen registrieren (§7, §8)
    diag_reg_source DIAG_SRC_KERNEL, 0, DIAG_LEVEL_INFO
    diag_reg_source DIAG_SRC_KOG,    0, DIAG_LEVEL_INFO
    diag_reg_source DIAG_SRC_EVBUS,  0, DIAG_LEVEL_INFO
    diag_reg_source DIAG_SRC_UOBJ,   0, DIAG_LEVEL_INFO
    diag_reg_source DIAG_SRC_CAP,    0, DIAG_LEVEL_INFO
    diag_reg_source DIAG_SRC_DIAG,   0, DIAG_LEVEL_INFO

    ; Initiale Health-Zustände: alle HEALTHY (§35)
    diag_set_health DIAG_SRC_KERNEL, DIAG_HEALTH_HEALTHY
    diag_set_health DIAG_SRC_KOG,    DIAG_HEALTH_HEALTHY
    diag_set_health DIAG_SRC_EVBUS,  DIAG_HEALTH_HEALTHY
    diag_set_health DIAG_SRC_UOBJ,   DIAG_HEALTH_HEALTHY
    diag_set_health DIAG_SRC_CAP,    DIAG_HEALTH_HEALTHY
    diag_set_health DIAG_SRC_DIAG,   DIAG_HEALTH_HEALTHY

    ; Ersten Diagnose-Log-Eintrag schreiben: DIAG initialisiert
    mov eax, DIAG_SRC_DIAG
    mov ebx, DIAG_LEVEL_INFO
    mov ecx, 1                 ; event_id = 1 (Initialisierung)
    xor edx, edx
    call diag_log
    jc .dinit_fail

    pop ecx
    pop edi
    pop esi
    clc
    ret
.dinit_fail:
    pop ecx
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; diag_self_test – Selbsttest §71 (8 Tests)
; CF=0 alle OK / CF=1 EAX = fehlgeschlagener Testfall
; ---------------------------------------------------------------------------
diag_self_test:
    push esi
    push edi
    push ebx

    ; Test 1: Testquelle registrieren (ID=99, default_level=WARNING)
    mov eax, 99
    xor ebx, ebx
    mov ecx, DIAG_LEVEL_WARNING
    call diag_register_source
    jc .dst_fail1

    ; Test 2: Testquelle auffinden
    mov eax, 99
    call diag_find_source
    jc .dst_fail2
    cmp byte [ebx + DIAG_OFF_SRC_PRESENT], 1
    jne .dst_fail2

    ; Test 3: diag_log mit erlaubtem Level (WARNING >= WARNING) → CF=0
    mov eax, 99
    mov ebx, DIAG_LEVEL_WARNING
    mov ecx, 100
    xor edx, edx
    call diag_log
    jc .dst_fail3

    ; Test 4: diag_log mit gefiltertem Level (DEBUG < WARNING) → CF=1 FILTERED
    mov eax, 99
    mov ebx, DIAG_LEVEL_DEBUG
    mov ecx, 101
    xor edx, edx
    call diag_log
    jnc .dst_fail4
    cmp eax, NP_ERR_DIAG_FILTERED
    jne .dst_fail4

    ; Test 5: diag_health_report → CF=0
    mov eax, 99
    mov ebx, DIAG_HEALTH_HEALTHY
    call diag_health_report
    jc .dst_fail5

    ; Test 6: diag_health_get → DIAG_HEALTH_HEALTHY
    mov eax, 99
    call diag_health_get
    jc .dst_fail6
    cmp eax, DIAG_HEALTH_HEALTHY
    jne .dst_fail6

    ; Test 7: diag_health_report mit neuem Zustand → CF=0
    mov eax, 99
    mov ebx, DIAG_HEALTH_DEGRADED
    call diag_health_report
    jc .dst_fail7

    ; Test 8: diag_find_source mit unbekannter ID → CF=1
    mov eax, 0xBEEF
    call diag_find_source
    jnc .dst_fail8

    pop ebx
    pop edi
    pop esi
    clc
    ret

.dst_fail1:  mov eax, 1
    jmp .dst_fail
.dst_fail2:  mov eax, 2
    jmp .dst_fail
.dst_fail3:  mov eax, 3
    jmp .dst_fail
.dst_fail4:  mov eax, 4
    jmp .dst_fail
.dst_fail5:  mov eax, 5
    jmp .dst_fail
.dst_fail6:  mov eax, 6
    jmp .dst_fail
.dst_fail7:  mov eax, 7
    jmp .dst_fail
.dst_fail8:  mov eax, 8
.dst_fail:
    pop ebx
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; diag_* Daten
; ---------------------------------------------------------------------------
diag_sources:       times (DIAG_MAX_SOURCES * DIAG_SRC_SIZE / 4) dd 0
diag_log_buf:       times (DIAG_LOG_MAX_RECORDS * DIAG_LOG_RECORD_SIZE / 4) dd 0
diag_health_tab:    times (DIAG_MAX_HEALTH * DIAG_HEALTH_SIZE / 4) dd 0
diag_log_write:     dd 0
diag_log_total:     dd 0
diag_log_lost:      dd 0
diag_log_seq:       dd 0
diag_stat_logged:   dd 0
diag_stat_filtered: dd 0
diag_stat_dropped:  dd 0
; Temp-Speicher (Single-Threaded Bootstrap)
diag_tmp_src_id:    dd 0
diag_tmp_flags:     dd 0
diag_tmp_level:     dd 0
diag_tmp_event:     dd 0
diag_tmp_value:     dd 0

; ===========================================================================
; NPSPEC-OBSERVABILITY-TRACING-0001 – Nova Kernel Trace Ring
; 64-Slot-Ringpuffer für strukturierte Kernel-Events (subsys/id/args/seq).
; ===========================================================================

TRACE_CAPACITY      equ 64
TRACE_EVENT_SIZE    equ 32

TRACE_SEQ           equ 0
TRACE_TIMESTAMP     equ 4
TRACE_SUBSYSTEM     equ 8
TRACE_EVENT_ID      equ 12
TRACE_ARG_A         equ 16
TRACE_ARG_B         equ 20
TRACE_ARG_C         equ 24
TRACE_FLAGS         equ 28

TRACE_SYS_KERNEL    equ 1
TRACE_SYS_IRQ       equ 2
TRACE_SYS_SCHED     equ 3
TRACE_SYS_MEM       equ 4
TRACE_SYS_IPC       equ 5

trace_initialize:
    mov edi, trace_ring
    mov ecx, TRACE_CAPACITY * TRACE_EVENT_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [trace_write_pos], 0
    mov dword [trace_seq], 0
    mov dword [trace_total_emitted], 0
    mov dword [trace_wrapped], 0
    mov dword [trace_ready], 1
    clc
    ret

; EAX=subsys  EDX=event_id  ECX=arg_a  EBX=arg_b → CF=0
; Überschreibt älteste Einträge wenn Ring voll (Ringpuffer-Semantik).
trace_emit:
    push ebx
    push esi
    push edi
    mov edi, [trace_write_pos]
    imul esi, edi, TRACE_EVENT_SIZE
    add esi, trace_ring
    push eax            ; save subsys
    mov eax, [trace_seq]
    mov [esi + TRACE_SEQ], eax
    inc dword [trace_seq]
    mov eax, [wd_global_tick]
    mov [esi + TRACE_TIMESTAMP], eax
    pop eax
    mov [esi + TRACE_SUBSYSTEM], eax
    mov [esi + TRACE_EVENT_ID], edx
    mov [esi + TRACE_ARG_A], ecx
    mov [esi + TRACE_ARG_B], ebx
    mov dword [esi + TRACE_ARG_C], 0
    mov dword [esi + TRACE_FLAGS], 0
    inc edi
    cmp edi, TRACE_CAPACITY
    jb .te_nowrap
    xor edi, edi
    inc dword [trace_wrapped]
.te_nowrap:
    mov [trace_write_pos], edi
    inc dword [trace_total_emitted]
    clc
    pop edi
    pop esi
    pop ebx
    ret

trace_self_test:
    push ebx
    push esi
    xor ebx, ebx    ; Fehlerz.

    ; T1: ready
    cmp dword [trace_ready], 1
    je .tst1_ok
    inc ebx
.tst1_ok:

    ; T2: emit ein Event
    mov eax, TRACE_SYS_KERNEL
    mov edx, 0x100
    mov ecx, 42
    mov ebx, 99
    call trace_emit
    jnc .tst2_ok
    inc ebx
.tst2_ok:
    xor ebx, ebx

    ; T3: total_emitted=1
    cmp dword [trace_total_emitted], 1
    je .tst3_ok
    inc ebx
.tst3_ok:

    ; T4: slot 0 hat korrektes Subsystem
    cmp dword [trace_ring + TRACE_SUBSYSTEM], TRACE_SYS_KERNEL
    je .tst4_ok
    inc ebx
.tst4_ok:

    ; T5: write_pos=1
    cmp dword [trace_write_pos], 1
    je .tst5_ok
    inc ebx
.tst5_ok:

    ; T6: 63 weitere Events emittieren (Ring voll = 64 total)
    mov esi, 63
.tst6_fill:
    test esi, esi
    jz .tst6_done
    mov eax, TRACE_SYS_IRQ
    mov edx, 0x200
    mov ecx, 0
    push ebx
    xor ebx, ebx
    call trace_emit
    pop ebx
    dec esi
    jmp .tst6_fill
.tst6_done:
    cmp dword [trace_total_emitted], 64
    je .tst6_ok
    inc ebx
.tst6_ok:

    ; T7: wrapped=1 nach Ring-Overflow (ein weiteres Event emittieren)
    mov eax, TRACE_SYS_SCHED
    mov edx, 0x300
    mov ecx, 0
    push ebx
    xor ebx, ebx
    call trace_emit
    pop ebx
    cmp dword [trace_wrapped], 1
    je .tst7_ok
    inc ebx
.tst7_ok:

    test ebx, ebx
    jnz .tstf
    clc
    pop esi
    pop ebx
    ret
.tstf:
    stc
    pop esi
    pop ebx
    ret

trace_ready:         dd 0
trace_write_pos:     dd 0
trace_seq:           dd 0
trace_total_emitted: dd 0
trace_wrapped:       dd 0
align 4
trace_ring:
    times TRACE_CAPACITY * TRACE_EVENT_SIZE db 0

; ===========================================================================
; NPSPEC-OBSERVABILITY-METRICS-0001 – Nova Kernel Performance Counters
; Benannte Zähler mit atomarem Inkrement; snapshot-fähig für Diagnose.
; ===========================================================================

METRICS_CAPACITY    equ 32
METRICS_RECORD_SIZE equ 24

MET_ID              equ 0
MET_VALUE           equ 4
MET_PEAK            equ 8
MET_TOTAL           equ 12
MET_SUBSYSTEM       equ 16
MET_FLAGS           equ 20

MET_FLAG_ACTIVE     equ 0x01
MET_FLAG_RATE       equ 0x02

; Vordefinierte Metrik-Slots (nach Bootstrap direkt beschreibbar)
MET_SLOT_IRQ_COUNT  equ 0
MET_SLOT_CTX_SWITCH equ 1
MET_SLOT_FAULTS     equ 2
MET_SLOT_ALLOCS     equ 3
MET_SLOT_FREES      equ 4
MET_SLOT_IPC_CALLS  equ 5
MET_SLOT_TRACE_EMIT equ 6
MET_SLOT_WD_KICKS   equ 7

metrics_initialize:
    mov edi, metrics_table
    mov ecx, METRICS_CAPACITY * METRICS_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    ; Vordefinierte Slots aktivieren
    mov esi, metrics_table
    mov ecx, METRICS_CAPACITY
    xor ebx, ebx
.mi_init_loop:
    mov [esi + MET_ID], ebx
    mov dword [esi + MET_FLAGS], MET_FLAG_ACTIVE
    add esi, METRICS_RECORD_SIZE
    inc ebx
    dec ecx
    jnz .mi_init_loop
    mov dword [metrics_ready], 1
    clc
    ret

; EAX=slot_index → inkrementiert MET_VALUE und MET_TOTAL; aktualisiert MET_PEAK
metrics_increment:
    push esi
    cmp eax, METRICS_CAPACITY
    jae .minc_done
    imul esi, eax, METRICS_RECORD_SIZE
    add esi, metrics_table
    inc dword [esi + MET_VALUE]
    inc dword [esi + MET_TOTAL]
    mov eax, [esi + MET_VALUE]
    cmp eax, [esi + MET_PEAK]
    jbe .minc_done
    mov [esi + MET_PEAK], eax
.minc_done:
    clc
    pop esi
    ret

; EAX=slot_index  EDX=delta → addiert delta auf MET_VALUE + MET_TOTAL
metrics_add:
    push esi
    push edx
    cmp eax, METRICS_CAPACITY
    jae .madd_done
    imul esi, eax, METRICS_RECORD_SIZE
    add esi, metrics_table
    add [esi + MET_VALUE], edx
    add [esi + MET_TOTAL], edx
    mov edx, [esi + MET_VALUE]
    cmp edx, [esi + MET_PEAK]
    jbe .madd_done
    mov [esi + MET_PEAK], edx
.madd_done:
    clc
    pop edx
    pop esi
    ret

; EAX=slot_index → EAX=current value, EDX=peak, ECX=total
metrics_read:
    push esi
    cmp eax, METRICS_CAPACITY
    jae .mrd_zero
    imul esi, eax, METRICS_RECORD_SIZE
    add esi, metrics_table
    mov eax, [esi + MET_VALUE]
    mov edx, [esi + MET_PEAK]
    mov ecx, [esi + MET_TOTAL]
    clc
    pop esi
    ret
.mrd_zero:
    xor eax, eax
    xor edx, edx
    xor ecx, ecx
    stc
    pop esi
    ret

; EAX=slot_index → setzt MET_VALUE=0 (Peak und Total bleiben)
metrics_reset:
    push esi
    cmp eax, METRICS_CAPACITY
    jae .mrst_done
    imul esi, eax, METRICS_RECORD_SIZE
    add esi, metrics_table
    mov dword [esi + MET_VALUE], 0
.mrst_done:
    clc
    pop esi
    ret

metrics_self_test:
    push ebx
    xor ebx, ebx

    ; T1: ready
    cmp dword [metrics_ready], 1
    je .mst1_ok
    inc ebx
.mst1_ok:

    ; T2: increment slot 0
    mov eax, MET_SLOT_IRQ_COUNT
    call metrics_increment
    mov eax, MET_SLOT_IRQ_COUNT
    call metrics_read      ; EAX=value
    cmp eax, 1
    je .mst2_ok
    inc ebx
.mst2_ok:

    ; T3: increment 4 more times → value=5
    mov ecx, 4
.mst3_loop:
    push ecx
    mov eax, MET_SLOT_IRQ_COUNT
    call metrics_increment
    pop ecx
    dec ecx
    jnz .mst3_loop
    mov eax, MET_SLOT_IRQ_COUNT
    call metrics_read
    cmp eax, 5
    je .mst3_ok
    inc ebx
.mst3_ok:

    ; T4: peak=5
    mov eax, MET_SLOT_IRQ_COUNT
    call metrics_read       ; EDX=peak
    cmp edx, 5
    je .mst4_ok
    inc ebx
.mst4_ok:

    ; T5: total=5
    cmp ecx, 5
    je .mst5_ok
    inc ebx
.mst5_ok:

    ; T6: metrics_add(slot1, 100)
    mov eax, MET_SLOT_CTX_SWITCH
    mov edx, 100
    call metrics_add
    mov eax, MET_SLOT_CTX_SWITCH
    call metrics_read
    cmp eax, 100
    je .mst6_ok
    inc ebx
.mst6_ok:

    ; T7: reset slot 0 → value=0, total bleibt
    mov eax, MET_SLOT_IRQ_COUNT
    call metrics_reset
    call metrics_read
    test eax, eax
    jz .mst7_ok
    inc ebx
.mst7_ok:

    test ebx, ebx
    jnz .mstf
    clc
    pop ebx
    ret
.mstf:
    stc
    pop ebx
    ret

metrics_ready:  dd 0
align 4
metrics_table:
    times METRICS_CAPACITY * METRICS_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; NPSPEC-OBSERVABILITY-LOGGING-0001 – Nova Kernel Structured Log Ring
; ---------------------------------------------------------------------------
; Strukturierter Observability-Log-Ring. Ergänzt Security Audit (SECURITY-AUDIT)
; und Trace Ring (OBSERVABILITY-TRACING). Eigene Kanäle, keine Überlappung.
; Severity: DEBUG/INFO/WARN/ERROR/CRIT. Kein dynamischer Speicher.
; Präfix OBSLOG_ um Kollision mit frühem LOG_RECORD_SIZE (line ~3794) zu vermeiden.
; ---------------------------------------------------------------------------
OBSLOG_CAPACITY     equ 32
OBSLOG_REC_SIZE     equ 32
OBSLOG_SEV_DEBUG    equ 0
OBSLOG_SEV_INFO     equ 1
OBSLOG_SEV_WARN     equ 2
OBSLOG_SEV_ERROR    equ 3
OBSLOG_SEV_CRIT     equ 4

OBSLOG_SEQ_OFF      equ 0
OBSLOG_TICK_OFF     equ 4
OBSLOG_SEV_OFF      equ 8
OBSLOG_COMP_OFF     equ 12
OBSLOG_TYPE_OFF     equ 16
OBSLOG_FLAGS_OFF    equ 20
OBSLOG_MSG_LO_OFF   equ 24
OBSLOG_MSG_HI_OFF   equ 28

obslog_initialize:
    mov edi, obslog_ring
    xor eax, eax
    mov ecx, (OBSLOG_CAPACITY * OBSLOG_REC_SIZE) / 4
    rep stosd
    mov dword [obslog_ready], 0
    mov dword [obslog_write_pos], 0
    mov dword [obslog_seq], 0
    mov dword [obslog_total], 0
    mov dword [obslog_errors], 0
    mov dword [obslog_ready], 1
    clc
    ret

; EAX=severity  EDX=component  ECX=event_type  EBX=msg_id
obslog_write:
    cmp dword [obslog_ready], 1
    jne .olw_fail
    cmp eax, OBSLOG_SEV_CRIT
    ja .olw_fail
    push esi
    push edi
    mov esi, [obslog_write_pos]
    cmp esi, OBSLOG_CAPACITY
    jb .olw_ok_pos
    xor esi, esi
.olw_ok_pos:
    imul edi, esi, OBSLOG_REC_SIZE
    add edi, obslog_ring
    push eax
    mov eax, [obslog_seq]
    mov [edi + OBSLOG_SEQ_OFF], eax
    inc dword [obslog_seq]
    pop eax
    push eax
    mov eax, [wd_global_tick]
    mov [edi + OBSLOG_TICK_OFF], eax
    pop eax
    mov [edi + OBSLOG_SEV_OFF], eax
    mov [edi + OBSLOG_COMP_OFF], edx
    mov [edi + OBSLOG_TYPE_OFF], ecx
    mov dword [edi + OBSLOG_FLAGS_OFF], 0
    mov [edi + OBSLOG_MSG_LO_OFF], ebx
    mov dword [edi + OBSLOG_MSG_HI_OFF], 0
    inc esi
    cmp esi, OBSLOG_CAPACITY
    jb .olw_no_wrap
    xor esi, esi
.olw_no_wrap:
    mov [obslog_write_pos], esi
    inc dword [obslog_total]
    cmp eax, OBSLOG_SEV_ERROR
    jb .olw_done
    inc dword [obslog_errors]
.olw_done:
    pop edi
    pop esi
    clc
    ret
.olw_fail:
    stc
    ret

; EAX=severity → EAX=count (O(n) scan)
obslog_count:
    cmp dword [obslog_ready], 1
    jne .olc_bad
    push esi
    push edi
    push ebx
    mov ebx, eax
    xor eax, eax
    xor esi, esi
.olc_loop:
    cmp esi, OBSLOG_CAPACITY
    jae .olc_done
    imul edi, esi, OBSLOG_REC_SIZE
    add edi, obslog_ring
    cmp dword [edi + OBSLOG_SEQ_OFF], 0
    je .olc_next
    cmp [edi + OBSLOG_SEV_OFF], ebx
    jne .olc_next
    inc eax
.olc_next:
    inc esi
    jmp .olc_loop
.olc_done:
    pop ebx
    pop edi
    pop esi
    clc
    ret
.olc_bad:
    xor eax, eax
    stc
    ret

obslog_self_test:
    xor esi, esi
    mov eax, OBSLOG_SEV_INFO
    mov edx, 1
    mov ecx, 1
    mov ebx, 0x4C4F4701
    call obslog_write
    jc .olstf
    inc esi
    mov eax, OBSLOG_SEV_ERROR
    mov edx, 2
    mov ecx, 2
    mov ebx, 0x4C4F4702
    call obslog_write
    jc .olstf
    inc esi
    mov eax, OBSLOG_SEV_WARN
    call obslog_count
    jc .olstf
    test eax, eax
    jnz .olstf
    inc esi
    mov eax, OBSLOG_SEV_ERROR
    call obslog_count
    jc .olstf
    test eax, eax
    jz .olstf
    inc esi
    cmp dword [obslog_errors], 1
    jb .olstf
    inc esi
    test esi, esi
    jnz .olstf_ok
.olstf:
    stc
    ret
.olstf_ok:
    clc
    ret

obslog_ready:       dd 0
obslog_write_pos:   dd 0
obslog_seq:         dd 0
obslog_total:       dd 0
obslog_errors:      dd 0
align 4
obslog_ring:
    times OBSLOG_CAPACITY * OBSLOG_REC_SIZE db 0

; ---------------------------------------------------------------------------
; NPSPEC-OBSERVABILITY-INTROSPECTION-0001 – Nova Subsystem Introspection Registry
; ---------------------------------------------------------------------------
; Einheitliche Registrierung von Subsystem-Query-Funktionen.
; Jedes Subsystem registriert eine Query-fn; Aufrufer fragen per Subsystem-ID ab.
; Vorbelegte IDs: KERNEL=1, SCHED=2, MEMORY=3, SECURITY=4, RESILIENCE=5.
; ---------------------------------------------------------------------------
INTRO_CAPACITY      equ 16
INTRO_RECORD_SIZE   equ 12

INTRO_ID_OFF        equ 0
INTRO_SYS_OFF       equ 4
INTRO_FN_OFF        equ 8

INTRO_SYS_KERNEL    equ 1
INTRO_SYS_SCHED     equ 2
INTRO_SYS_MEMORY    equ 3
INTRO_SYS_SECURITY  equ 4
INTRO_SYS_RESILIENCE equ 5

intro_initialize:
    mov edi, intro_registry
    xor eax, eax
    mov ecx, (INTRO_CAPACITY * INTRO_RECORD_SIZE) / 4
    rep stosd
    mov dword [intro_ready], 0
    mov dword [intro_count], 0
    mov dword [intro_next_id], 1
    ; Pre-register 5 built-in providers (fn=0 → stub)
    mov eax, INTRO_SYS_KERNEL
    xor edx, edx
    call intro_register
    jc .init_fail
    mov eax, INTRO_SYS_SCHED
    xor edx, edx
    call intro_register
    jc .init_fail
    mov eax, INTRO_SYS_MEMORY
    xor edx, edx
    call intro_register
    jc .init_fail
    mov eax, INTRO_SYS_SECURITY
    xor edx, edx
    call intro_register
    jc .init_fail
    mov eax, INTRO_SYS_RESILIENCE
    xor edx, edx
    call intro_register
    jc .init_fail
    mov dword [intro_ready], 1
    clc
    ret
.init_fail:
    stc
    ret

; EAX=subsystem_id  EDX=query_fn → EAX=slot  CF=0/CF=1 full
intro_register:
    cmp dword [intro_count], INTRO_CAPACITY
    jae .ir_full
    push esi
    push edi
    xor esi, esi
.ir_scan:
    cmp esi, INTRO_CAPACITY
    jae .ir_noslot
    imul edi, esi, INTRO_RECORD_SIZE
    add edi, intro_registry
    cmp dword [edi + INTRO_ID_OFF], 0
    je .ir_slot
    inc esi
    jmp .ir_scan
.ir_slot:
    push eax
    mov eax, [intro_next_id]
    mov [edi + INTRO_ID_OFF], eax
    inc dword [intro_next_id]
    pop eax
    mov [edi + INTRO_SYS_OFF], eax
    mov [edi + INTRO_FN_OFF], edx
    inc dword [intro_count]
    mov eax, esi
    pop edi
    pop esi
    clc
    ret
.ir_noslot:
    pop edi
    pop esi
.ir_full:
    stc
    ret

; EAX=subsystem_id → EAX=slot  CF=0 found / CF=1 not found
intro_query:
    push esi
    push edi
    push ebx
    mov ebx, eax
    xor esi, esi
.iq_loop:
    cmp esi, INTRO_CAPACITY
    jae .iq_notfound
    imul edi, esi, INTRO_RECORD_SIZE
    add edi, intro_registry
    cmp dword [edi + INTRO_ID_OFF], 0
    je .iq_next
    cmp [edi + INTRO_SYS_OFF], ebx
    jne .iq_next
    mov eax, esi
    pop ebx
    pop edi
    pop esi
    clc
    ret
.iq_next:
    inc esi
    jmp .iq_loop
.iq_notfound:
    pop ebx
    pop edi
    pop esi
    stc
    ret

intro_self_test:
    ; Query KERNEL → must find
    mov eax, INTRO_SYS_KERNEL
    call intro_query
    jc .istf
    ; Query unknown (99) → CF=1
    mov eax, 99
    call intro_query
    jnc .istf
    ; Register new provider
    mov eax, 10
    mov edx, 0
    call intro_register
    jc .istf
    ; Query it back
    mov eax, 10
    call intro_query
    jc .istf
    clc
    ret
.istf:
    stc
    ret

intro_ready:      dd 0
intro_count:      dd 0
intro_next_id:    dd 1
align 4
intro_registry:
    times INTRO_CAPACITY * INTRO_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; NPSPEC-OBSERVABILITY-PROFILING-0001 – Nova Tick-Based Sampling Profiler
; ---------------------------------------------------------------------------
; Tick-basierter Sampling-Profiler. Pro Sample: component-ID + Tick aufzeichnen.
; prof_hottest liefert die Komponente mit den meisten Samples.
; prof_reset löscht alle Samples (neue Messperiode).
; ---------------------------------------------------------------------------
PROF_SAMPLE_COUNT   equ 16
PROF_RECORD_SIZE    equ 16

PROF_TICK_OFF       equ 0
PROF_COMP_OFF       equ 4
PROF_CNT_OFF        equ 8
PROF_HOT_OFF        equ 12

prof_initialize:
    mov edi, prof_samples
    xor eax, eax
    mov ecx, (PROF_SAMPLE_COUNT * PROF_RECORD_SIZE) / 4
    rep stosd
    mov dword [prof_ready], 0
    mov dword [prof_total], 0
    mov dword [prof_ready], 1
    clc
    ret

; EAX=component_id  — record sample; wraps ring
prof_sample:
    cmp dword [prof_ready], 1
    jne .ps_fail
    push esi
    push edi
    ; search existing slot for this component
    xor esi, esi
.ps_search:
    cmp esi, PROF_SAMPLE_COUNT
    jae .ps_new_slot
    imul edi, esi, PROF_RECORD_SIZE
    add edi, prof_samples
    cmp [edi + PROF_COMP_OFF], eax
    je .ps_found
    inc esi
    jmp .ps_search
.ps_found:
    ; existing entry: update tick, increment count and hotness
    push eax
    mov eax, [wd_global_tick]
    mov [edi + PROF_TICK_OFF], eax
    pop eax
    inc dword [edi + PROF_CNT_OFF]
    inc dword [edi + PROF_HOT_OFF]
    inc dword [prof_total]
    pop edi
    pop esi
    clc
    ret
.ps_new_slot:
    ; find free slot (COMP=0)
    xor esi, esi
.ps_free:
    cmp esi, PROF_SAMPLE_COUNT
    jae .ps_evict
    imul edi, esi, PROF_RECORD_SIZE
    add edi, prof_samples
    cmp dword [edi + PROF_COMP_OFF], 0
    je .ps_alloc
    inc esi
    jmp .ps_free
.ps_evict:
    ; ring: overwrite slot 0 (simplest eviction)
    mov edi, prof_samples
    mov esi, 0
.ps_alloc:
    push eax
    mov eax, [wd_global_tick]
    mov [edi + PROF_TICK_OFF], eax
    pop eax
    mov [edi + PROF_COMP_OFF], eax
    mov dword [edi + PROF_CNT_OFF], 1
    mov dword [edi + PROF_HOT_OFF], 1
    inc dword [prof_total]
    pop edi
    pop esi
    clc
    ret
.ps_fail:
    stc
    ret

; → EAX=component_id  EDX=count  CF=0 found / CF=1 empty
prof_hottest:
    cmp dword [prof_ready], 1
    jne .ph_fail
    push esi
    push edi
    xor esi, esi
    xor eax, eax   ; best component
    xor edx, edx   ; best count
.ph_loop:
    cmp esi, PROF_SAMPLE_COUNT
    jae .ph_done
    imul edi, esi, PROF_RECORD_SIZE
    add edi, prof_samples
    cmp dword [edi + PROF_COMP_OFF], 0
    je .ph_next
    cmp dword [edi + PROF_CNT_OFF], 0
    je .ph_next
    mov ecx, [edi + PROF_CNT_OFF]
    cmp ecx, edx
    jbe .ph_next
    mov edx, ecx
    mov eax, [edi + PROF_COMP_OFF]
.ph_next:
    inc esi
    jmp .ph_loop
.ph_done:
    pop edi
    pop esi
    test eax, eax
    jz .ph_fail
    clc
    ret
.ph_fail:
    xor eax, eax
    xor edx, edx
    stc
    ret

; Reset all samples (new profiling period)
prof_reset:
    mov edi, prof_samples
    xor eax, eax
    mov ecx, (PROF_SAMPLE_COUNT * PROF_RECORD_SIZE) / 4
    rep stosd
    clc
    ret

prof_self_test:
    ; sample component 1 twice
    mov eax, 1
    call prof_sample
    jc .pstf
    mov eax, 1
    call prof_sample
    jc .pstf
    ; sample component 2 once
    mov eax, 2
    call prof_sample
    jc .pstf
    ; hottest → component 1 (count=2)
    call prof_hottest
    jc .pstf
    cmp eax, 1
    jne .pstf
    cmp edx, 2
    jb .pstf
    ; reset
    call prof_reset
    ; after reset, hottest → CF=1 (empty)
    call prof_hottest
    jnc .pstf
    clc
    ret
.pstf:
    stc
    ret

prof_ready:   dd 0
prof_total:   dd 0
align 4
prof_samples:
    times PROF_SAMPLE_COUNT * PROF_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
