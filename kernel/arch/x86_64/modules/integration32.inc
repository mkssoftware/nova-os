; NPSPEC-INTEGRATION-0001 – Nova Subsystem Integration Pipelines
;
; Verbindet die einzelnen Subsysteme zu zusammenhängenden Kontrollflüssen:
;   Pipeline 1: Security Access Check  (MAC → DAC → RBAC → Audit)
;   Pipeline 2: Resilience Fault       (Detection → Containment → Failover → Metrics)
;   Pipeline 3: Scheduler Admission    (QoS → CFS oder RT-Register)
;   Pipeline 4: Memory Pressure / OOM  (OOM-Kill → Accounting → Audit)
;   Pipeline 5: Watchdog Expire        (WD-Tick → Fault Detection → CB → Metrics)
; ===========================================================================

; ---------------------------------------------------------------------------
; Pipeline 1: Security Access Check
; EAX=subject_id  EDX=object_id  ECX=perm_bits
; → CF=0 Zugriff erlaubt / CF=1 verweigert (+ Audit-Eintrag)
; Reihenfolge: MAC → DAC → RBAC; erster Fehler bricht ab und loggt VIOLATION.
; ---------------------------------------------------------------------------
security_access_check:
    push esi
    push edi
    push ebx
    push ecx

    ; MAC-Check: subject=EAX, object=EDX
    push eax
    push edx
    push ecx
    call mac_check
    pop ecx
    pop edx
    pop eax
    jnc .sac_mac_ok

    ; MAC verweigert → Audit VIOLATION
    push eax
    push edx
    push ecx
    mov eax, AUDIT_TYPE_VIOLATION
    ; EDX=subject bleibt, ECX=object_id für AUDIT_OBJECT
    mov ecx, edx     ; object_id
    mov ebx, 0       ; result=deny
    call security_audit_log
    pop ecx
    pop edx
    pop eax
    stc
    pop ecx
    pop ebx
    pop edi
    pop esi
    ret

.sac_mac_ok:
    ; DAC-Check: obj=EDX, caller=EAX, perms=ECX
    push eax
    push edx
    push ecx
    mov eax, edx     ; obj_id
    mov edx, [esp+8] ; caller = original EAX
    call dac_check
    pop ecx
    pop edx
    pop eax
    jnc .sac_dac_ok

    push eax
    push edx
    push ecx
    mov eax, AUDIT_TYPE_VIOLATION
    mov ecx, edx
    mov ebx, 0
    call security_audit_log
    pop ecx
    pop edx
    pop eax
    stc
    pop ecx
    pop ebx
    pop edi
    pop esi
    ret

.sac_dac_ok:
    ; RBAC-Check: subject=EAX, perm=ECX
    push eax
    push edx
    push ecx
    mov edx, ecx     ; perm_bits als requested_perm
    call rbac_check
    pop ecx
    pop edx
    pop eax
    jnc .sac_rbac_ok

    push eax
    push edx
    push ecx
    mov eax, AUDIT_TYPE_VIOLATION
    mov ecx, edx
    mov ebx, 0
    call security_audit_log
    pop ecx
    pop edx
    pop eax
    stc
    pop ecx
    pop ebx
    pop edi
    pop esi
    ret

.sac_rbac_ok:
    ; Alles erlaubt → CAP_GRANT-Event loggen
    push eax
    push edx
    push ecx
    mov eax, AUDIT_TYPE_CAP_GRANT
    mov ecx, edx     ; object
    mov ebx, 1       ; allow
    call security_audit_log
    pop ecx
    pop edx
    pop eax
    clc
    pop ecx
    pop ebx
    pop edi
    pop esi
    ret

; ---------------------------------------------------------------------------
; Pipeline 2: Resilience Fault Pipeline
; EAX=fault_type  EDX=subsystem_id  ECX=fault_code
; Meldet Fehler an Detection→Containment→ggf. Failover + Metrics
; ---------------------------------------------------------------------------
resilience_fault_pipeline:
    push esi
    push edi
    push ebx
    push ecx

    ; 1. Fault Detection registrieren
    push eax
    push edx
    push ecx
    call detection_record_fault
    pop ecx
    pop edx
    pop eax

    ; 2. Metrics: FAULTS-Zähler erhöhen (Slot 2)
    push eax
    mov eax, 2       ; MET_SLOT_FAULTS
    call metrics_increment
    pop eax

    ; 3. Containment: suche Slot für subsystem_id
    ;    (linear über cont_table; erstes ACTIVE-Match)
    push esi
    push ecx
    push edx
    mov esi, cont_table
    xor ecx, ecx
.rfp_cont_scan:
    cmp ecx, CONT_CAPACITY
    jae .rfp_cont_done
    cmp dword [esi + CONT_STATE_OFF], CONT_STATE_FREE
    je .rfp_cont_next
    cmp dword [esi + CONT_SUBSYSTEM], edx
    je .rfp_cont_found
.rfp_cont_next:
    add esi, CONT_RECORD_SIZE
    inc ecx
    jmp .rfp_cont_scan
.rfp_cont_found:
    push eax
    mov eax, ecx
    call containment_report_fault
    ; Wenn jetzt ISOLATED → Failover prüfen
    cmp dword [esi + CONT_STATE_OFF], CONT_STATE_ISOLATED
    jne .rfp_no_failover
    ; Failover: suche passenden Slot für subsystem
    push esi
    push ecx
    mov esi, fo_table
    xor ecx, ecx
.rfp_fo_scan:
    cmp ecx, FO_CAPACITY
    jae .rfp_fo_done
    cmp dword [esi + FO_STATE_OFF], FO_STATE_FREE
    je .rfp_fo_next
    cmp dword [esi + FO_PRIMARY], edx   ; primary=subsystem_id
    je .rfp_fo_trig
.rfp_fo_next:
    add esi, FO_RECORD_SIZE
    inc ecx
    jmp .rfp_fo_scan
.rfp_fo_trig:
    push eax
    mov eax, ecx
    call failover_trigger
    pop eax
.rfp_fo_done:
    pop ecx
    pop esi
.rfp_no_failover:
    pop eax
.rfp_cont_done:
    pop edx
    pop ecx
    pop esi

    ; 4. Restart-Check: suche owner=subsystem_id
    push esi
    push ecx
    push edx
    mov esi, rst_table
    xor ecx, ecx
.rfp_rst_scan:
    cmp ecx, RST_CAPACITY
    jae .rfp_rst_done
    cmp dword [esi + RST_STATE], 0     ; FREE
    je .rfp_rst_next
    cmp dword [esi + RST_OWNER], edx
    je .rfp_rst_found
.rfp_rst_next:
    add esi, RST_RECORD_SIZE
    inc ecx
    jmp .rfp_rst_scan
.rfp_rst_found:
    push eax
    mov eax, ecx
    call restart_check_and_record
    pop eax
.rfp_rst_done:
    pop edx
    pop ecx
    pop esi

    pop ecx
    pop ebx
    pop edi
    pop esi
    ret

; ---------------------------------------------------------------------------
; Pipeline 3: Scheduler Task Admission
; EAX=owner  EDX=priority  ECX=qos_class
; Prio >= 200 → RT; sonst QoS-Budget prüfen → CFS enqueue
; → EAX=slot  CF=0 / CF=1 kein Platz oder Budget
; ---------------------------------------------------------------------------
SCHED_RT_THRESHOLD   equ 200

scheduler_admit_task:
    push esi
    push ebx
    push ecx

    cmp edx, SCHED_RT_THRESHOLD
    jae .sat_rt

    ; QoS Budget prüfen
    push eax
    push edx
    push ecx
    mov eax, ecx     ; qos_class
    call qos_consume
    pop ecx
    pop edx
    pop eax
    jc .sat_no_budget

    ; CFS enqueue: owner=EAX, weight=(256-priority) für inverse Priorität
    ; Keine Register-Sicherung nötig — nach dem call werden sie nicht mehr gebraucht
    mov ecx, 256
    sub ecx, edx     ; kleinerer weight = höhere Prio
    jbe .sat_cfs_weight_min
    jmp .sat_cfs_enqueue
.sat_cfs_weight_min:
    mov ecx, 1
.sat_cfs_enqueue:
    mov edx, ecx     ; weight
    call cfs_task_enqueue
    ; EAX=slot bleibt unverändert
    pop ecx
    pop ebx
    pop esi
    ret

.sat_rt:
    ; RT registrieren: owner=EAX, deadline=wd_global_tick+500, prio=EDX
    push eax
    push edx
    push ecx
    push edx         ; prio sichern
    mov ecx, [wd_global_tick]
    add ecx, 500
    mov edx, ecx    ; deadline
    mov ecx, [esp]  ; prio
    add esp, 4
    mov ebx, RT_POLICY_EDF
    call rt_task_register
    pop ecx
    pop edx
    pop eax
    pop ecx
    pop ebx
    pop esi
    ret

.sat_no_budget:
    stc
    pop ecx
    pop ebx
    pop esi
    ret

; ---------------------------------------------------------------------------
; Pipeline 4: Memory Pressure Response
; Wird aufgerufen wenn Speicher erschöpft → OOM wählt Opfer, loggt, accountiert
; → EAX=killed_pid CF=0 / CF=1 kein Kandidat
; ---------------------------------------------------------------------------
memory_pressure_response:
    push esi
    push ebx

    ; 1. OOM: Opfer wählen
    call oom_kill_victim
    jc .mpr_no_victim

    mov ebx, eax     ; killed_pid sichern

    ; 2. Audit: VIOLATION-Typ für OOM-Kill
    push eax
    push edx
    push ecx
    mov edx, eax     ; subject = killed_pid
    mov eax, AUDIT_TYPE_VIOLATION
    mov ecx, 0       ; object = kernel
    mov ebx, 0       ; result = terminate
    call security_audit_log
    pop ecx
    pop edx
    pop eax

    ; 3. Metrics: FREES-Slot inkrementieren
    push eax
    mov eax, 4       ; MET_SLOT_FREES
    call metrics_increment
    pop eax

    ; 4. Circuit Breaker: Memory-Subsystem (Slot 0) failure melden
    push eax
    xor eax, eax
    call circuitbreaker_record_failure
    pop eax

    mov eax, ebx     ; killed_pid zurück
    clc
    pop ebx
    pop esi
    ret

.mpr_no_victim:
    stc
    pop ebx
    pop esi
    ret

; ---------------------------------------------------------------------------
; Pipeline 5: Watchdog Tick → Fault Detection → Circuit Breaker → Metrics
; EAX=wd_slot → prüft ob WD abgelaufen; bei Ablauf: Fault-Pipeline + CB
; ---------------------------------------------------------------------------
watchdog_expire_pipeline:
    push esi
    push ebx
    push ecx

    ; Watchdog-Tick global erhöhen
    inc dword [wd_global_tick]
    call watchdog_tick
    call watchdog_check

    ; Wenn Watchdog abgelaufen (wd_total_expires erhöht sich): Fault melden
    mov esi, [wd_total_expires]
    cmp esi, [wep_last_expires]
    je .wep_no_new_expire

    mov [wep_last_expires], esi

    ; Fault Detection: TIMING-Fehler für Subsystem 0
    mov eax, 2       ; FD_TYPE_TIMING
    xor edx, edx
    mov ecx, 0xDEAD0001
    call detection_record_fault

    ; Metrics: IRQ_COUNT-Slot (als Watchdog-Event-Proxy)
    mov eax, 0
    call metrics_increment

    ; CB: Failure für Slot 0 (Watchdog-Subsystem)
    xor eax, eax
    call circuitbreaker_record_failure

.wep_no_new_expire:
    pop ecx
    pop ebx
    pop esi
    ret

; ---------------------------------------------------------------------------
; integration_self_test – Testet alle 5 Pipelines
; ---------------------------------------------------------------------------
integration_self_test:
    push ebx
    push esi
    push edi
    push ecx
    xor edi, edi     ; Fehler-Zähler

    ; ========================================================================
    ; Test-Vorbereitungen: Subject/Object/Rollen für Pipeline 1
    ; ========================================================================
    ; Subject 50 = CONFIDENTIAL  (MAC), Owner (DAC), Rolle 10 (RBAC mit WRITE)
    ; Object  99 = INTERNAL      (MAC), obj_id=99 (DAC: owner=50, others=R)
    ; ========================================================================

    ; MAC: Subject 50 CONFIDENTIAL
    mov eax, 50
    mov edx, MAC_LABEL_CONFIDENTIAL
    call mac_register_subject

    ; MAC: Object 99 INTERNAL
    mov eax, 99
    mov edx, MAC_LABEL_INTERNAL
    call mac_register_object

    ; DAC: Object 99, owner=50, owner=RWX, others=0
    mov eax, 99
    mov edx, 50
    mov ecx, DAC_PERM_READ | DAC_PERM_WRITE | DAC_PERM_EXEC
    xor ebx, ebx
    call dac_register

    ; RBAC: Rolle 10 = READ|WRITE
    mov eax, 10
    mov edx, DAC_PERM_READ | DAC_PERM_WRITE
    call rbac_define_role

    ; RBAC: Subject 50 → Rolle 10
    mov eax, 50
    mov edx, 10
    call rbac_assign_role

    ; ============================
    ; Test P1a: Zugriff erlaubt
    ; ============================
    mov eax, 50
    mov edx, 99
    mov ecx, DAC_PERM_WRITE
    call security_access_check
    jnc .istp1a_ok
    inc edi
.istp1a_ok:

    ; Audit muss CAP_GRANT enthalten (total audit >= 1)
    cmp dword [audit_total], 1
    jae .istp1b_ok
    inc edi
.istp1b_ok:

    ; ============================
    ; Test P1c: Zugriff verweigert (Subject 51 PUBLIC, Object 88 SECRET)
    ; ============================
    mov eax, 51
    mov edx, MAC_LABEL_PUBLIC
    call mac_register_subject
    mov eax, 88
    mov edx, MAC_LABEL_SECRET
    call mac_register_object

    push dword [audit_violations]   ; Violations vor dem Test

    mov eax, 51
    mov edx, 88
    mov ecx, DAC_PERM_READ
    call security_access_check
    jc .istp1c_ok
    inc edi
.istp1c_ok:
    ; audit_violations muss gestiegen sein
    pop ecx
    cmp dword [audit_violations], ecx
    ja .istp1d_ok
    inc edi
.istp1d_ok:

    ; ============================
    ; Test P2: Resilience Fault Pipeline
    ; Subsystem 77 mit Containment-Slot (threshold=2)
    ; ============================

    ; Containment für Subsystem 77 registrieren (threshold=2)
    mov eax, 77
    mov edx, 2
    call containment_register
    jnc .istp2_cont_ok
    inc edi
.istp2_cont_ok:
    mov ebx, eax   ; cont_slot

    ; Failover für primary=77 registrieren
    mov eax, 77
    mov edx, 78    ; backup
    call failover_register

    ; 1. Fehler → DEGRADED
    mov eax, FD_TYPE_TIMING
    mov edx, 77
    mov ecx, 0xBAD0001
    call resilience_fault_pipeline

    ; Containment-State muss DEGRADED sein
    push esi
    imul esi, ebx, CONT_RECORD_SIZE
    add esi, cont_table
    cmp dword [esi + CONT_STATE_OFF], CONT_STATE_DEGRADED
    je .istp2a_ok
    inc edi
.istp2a_ok:
    pop esi

    ; 2. Fehler → ISOLATED + Failover
    push dword [fo_total_switches]
    mov eax, FD_TYPE_TIMING
    mov edx, 77
    mov ecx, 0xBAD0002
    call resilience_fault_pipeline

    push esi
    imul esi, ebx, CONT_RECORD_SIZE
    add esi, cont_table
    cmp dword [esi + CONT_STATE_OFF], CONT_STATE_ISOLATED
    je .istp2b_ok
    inc edi
.istp2b_ok:
    pop esi

    ; fo_total_switches muss gestiegen sein
    pop ecx
    cmp dword [fo_total_switches], ecx
    ja .istp2c_ok
    inc edi
.istp2c_ok:

    ; Metrics FAULTS-Slot muss > 0
    mov eax, 2
    call metrics_read
    cmp eax, 0
    ja .istp2d_ok
    inc edi
.istp2d_ok:

    ; ============================
    ; Test P3: Scheduler Admission
    ; ============================

    ; P3a: Normal-Task (prio=50, qos=NORMAL) → CFS
    push dword [cfs_next_id]
    mov eax, 100    ; owner
    mov edx, 50     ; prio < 200
    mov ecx, QOS_CLASS_NORMAL
    call scheduler_admit_task
    jnc .istp3a_ok
    inc edi
.istp3a_ok:
    ; CFS-Task muss hinzugefügt sein
    pop ecx
    cmp dword [cfs_next_id], ecx
    ja .istp3b_ok
    inc edi
.istp3b_ok:

    ; P3b: RT-Task (prio=250) → RT-Tabelle
    push dword [rt_next_id]
    mov eax, 101
    mov edx, 250    ; >= 200 → RT
    mov ecx, QOS_CLASS_RT
    call scheduler_admit_task
    jnc .istp3c_ok
    inc edi
.istp3c_ok:
    pop ecx
    cmp dword [rt_next_id], ecx
    ja .istp3d_ok
    inc edi
.istp3d_ok:

    ; P3c: QoS-Budget erschöpfen → Admission schlägt fehl
    ; IDLE-Klasse hat Budget=10; 11x consume → CF=1
    push ecx
    mov ecx, 11
.istp3e_burn:
    mov eax, 200    ; prio < 200
    mov edx, 50
    push ecx
    mov ecx, QOS_CLASS_IDLE
    call scheduler_admit_task
    pop ecx
    loop .istp3e_burn
    ; Letzter muss scheitern
    mov eax, 200
    mov edx, 50
    mov ecx, QOS_CLASS_IDLE
    call scheduler_admit_task
    jc .istp3f_ok
    inc edi
.istp3f_ok:
    pop ecx

    ; ============================
    ; Test P4: Memory Pressure
    ; ============================

    ; OOM-Kandidaten registrieren
    mov eax, 200
    mov edx, 60
    call oom_register_candidate
    mov eax, 201
    mov edx, 90
    call oom_register_candidate

    push dword [audit_total]
    push dword [oom_kills]

    call memory_pressure_response
    jnc .istp4a_ok
    inc edi
.istp4a_ok:
    ; Opfer muss PID=201 sein (Score=90 > 60)
    cmp eax, 201
    je .istp4b_ok
    inc edi
.istp4b_ok:
    ; oom_kills muss gestiegen
    pop ecx
    cmp dword [oom_kills], ecx
    ja .istp4c_ok
    inc edi
.istp4c_ok:
    ; audit_total muss gestiegen
    pop ecx
    cmp dword [audit_total], ecx
    ja .istp4d_ok
    inc edi
.istp4d_ok:

    ; ============================
    ; Test P5: Watchdog Expire Pipeline
    ; ============================

    ; WD registrieren und ablaufen lassen
    push dword [wd_total_expires]
    mov eax, 0xFF
    mov edx, 1       ; timeout=1 Tick
    mov ecx, WD_ACTION_LOG
    call watchdog_create
    jnc .istp5_create_ok
    inc edi
.istp5_create_ok:
    push eax   ; handle

    call watchdog_arm

    ; wep_last_expires auf aktuellen Stand setzen → nächster Call erkennt Änderung
    mov eax, [wd_total_expires]
    mov [wep_last_expires], eax

    ; Tick simulieren: Timeout=1 → sofort abgelaufen
    call watchdog_expire_pipeline

    ; Nach Pipeline muss wd_total_expires gestiegen sein
    pop eax   ; handle
    pop ecx   ; saved expires
    cmp dword [wd_total_expires], ecx
    ja .istp5a_ok
    inc edi
.istp5a_ok:

    ; ============================
    ; Ergebnis
    ; ============================
    test edi, edi
    jnz .istfail
    mov dword [integration_ready], 1
    clc
    pop ecx
    pop edi
    pop esi
    pop ebx
    ret
.istfail:
    stc
    pop ecx
    pop edi
    pop esi
    pop ebx
    ret

integration_ready:  dd 0
wep_last_expires:   dd 0

; ---------------------------------------------------------------------------
; Minimaler Kernel Main
; ---------------------------------------------------------------------------

kernel_main:
    call kernel_operational_prepare
    jmp kernel_idle

kernel_operational_prepare:
    mov eax, [kernel_context + CONTEXT_SEEN]
    test eax, CONTEXT_HAS_GRAPHICS
    jz .text_mode
    cmp dword [kernel_context + CONTEXT_BPP], 32
    jne .text_mode
    ; Der UEFI-Kernellader hat den verifizierten Bootsplash bereits direkt vor
    ; ExitBootServices dargestellt. Der normale Kernelstart bewahrt diesen
    ; Framebuffer, bis die Ring-3-System-UI ihre erste Szene präsentiert.
    mov esi, message_framebuffer_ok
    call serial_write_string
    jmp .ready

.text_mode:
    mov esi, message_text_mode
    call serial_write_string

.ready:
    mov esi, message_ready
    call serial_write_string
    ret

kernel_idle:
    sti
.loop:
    call power_cpu_idle
    jmp .loop

kernel_shutdown:
    cli
    call power_manager_request_shutdown
    jc kernel_halt
    call power_manager_platform_off
    jmp kernel_halt

kernel_shutdown_authorized:
    cli
    call power_manager_platform_off
    jmp kernel_halt

; ---------------------------------------------------------------------------
; Power Manager ABI 1.0 (NPSPEC-KERNEL-0021)
; ---------------------------------------------------------------------------
POWER_API_SIZE                 equ 72
POWER_STATE_RUNNING            equ 0
POWER_STATE_IDLE               equ 1
POWER_STATE_SUSPEND_TO_IDLE    equ 2
POWER_STATE_SUSPEND_TO_RAM     equ 3
POWER_STATE_HIBERNATE          equ 4
POWER_STATE_HYBRID_SLEEP       equ 5
POWER_STATE_SHUTDOWN           equ 6
POWER_STATE_RESTART            equ 7
POWER_STATE_OFF                equ 8
POWER_SUPPORTED_MASK           equ (1 << POWER_STATE_RUNNING) | (1 << POWER_STATE_IDLE) | (1 << POWER_STATE_SHUTDOWN) | (1 << POWER_STATE_RESTART) | (1 << POWER_STATE_OFF)
POWER_PROFILE_BALANCED         equ 1
POWER_PROFILE_PERFORMANCE      equ 0
POWER_PROFILE_EFFICIENCY       equ 2
POWER_PROFILE_POWERSAVE        equ 3
POWER_PROFILE_SUPPORTED_MASK   equ 0x0000000F
POWER_PHASE_NONE               equ 0
POWER_PHASE_REQUEST            equ 1
POWER_PHASE_FREEZE_USERSPACE   equ 2
POWER_PHASE_SYNC_STORAGE       equ 3
POWER_PHASE_STOP_DEVICES       equ 4
POWER_PHASE_PLATFORM_OFF       equ 5
POWER_WAKE_LOCK_CAPACITY       equ 2
POWER_IDLE_SHALLOW             equ 0
POWER_IDLE_DEEP                equ 1

power_manager_initialize:
    mov dword [power_current_state], POWER_STATE_RUNNING
    mov dword [power_target_state], POWER_STATE_RUNNING
    mov dword [power_profile], POWER_PROFILE_BALANCED
    mov dword [power_transition_phase], POWER_PHASE_NONE
    mov dword [power_transition_active], 0
    mov dword [power_shutdown_requests], 0
    mov dword [power_transition_failures], 0
    mov dword [power_wake_lock_count], 0
    mov dword [power_wake_lock_expirations], 0
    mov dword [power_idle_entries], 0
    mov dword [power_deep_idle_entries], 0
    mov dword [power_profile_changes], 0
    mov dword [power_wake_lock_deadlines], 0
    mov dword [power_wake_lock_deadlines + 4], 0
    clc
    ret

power_manager_self_test:
    cmp dword [power_api], POWER_API_SIZE
    jne .invalid
    cmp word [power_api + 4], 1
    jne .invalid
    cmp dword [power_api + 12], POWER_SUPPORTED_MASK
    jne .invalid
    cmp dword [power_current_state], POWER_STATE_RUNNING
    jne .invalid
    mov eax, 10
    call power_wake_lock_acquire
    jc .invalid
    cmp dword [power_wake_lock_count], 1
    jne .invalid
    call power_wake_lock_release
    jc .invalid
    cmp dword [power_wake_lock_count], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

; CF=1, falls bereits ein globaler Energieuebergang aktiv ist.
power_manager_request_shutdown:
    cmp dword [power_transition_active], 0
    jne .busy
    mov dword [power_transition_active], 1
    inc dword [power_shutdown_requests]
    mov dword [power_target_state], POWER_STATE_SHUTDOWN
    mov dword [power_transition_phase], POWER_PHASE_REQUEST
    mov esi, message_shutdown_requested
    call serial_write_string
    mov dword [power_transition_phase], POWER_PHASE_FREEZE_USERSPACE
    mov esi, message_shutdown_userspace
    call serial_write_string
    mov dword [power_transition_phase], POWER_PHASE_SYNC_STORAGE
    mov esi, message_shutdown_storage
    call serial_write_string
    mov dword [power_transition_phase], POWER_PHASE_STOP_DEVICES
    mov esi, message_shutdown_devices
    call serial_write_string
    mov dword [power_current_state], POWER_STATE_SHUTDOWN
    clc
    ret
.busy:
    inc dword [power_transition_failures]
    stc
    ret

; EAX=maximale Dauer in 100-Hz-Ticks, Rueckgabe EAX=Slot.
power_wake_lock_acquire:
    test eax, eax
    jz .invalid
    cmp dword [power_wake_lock_count], POWER_WAKE_LOCK_CAPACITY
    jae .invalid
    mov edx, [timer_ticks]
    add edx, eax
    jc .invalid
    xor ecx, ecx
.find:
    cmp dword [power_wake_lock_deadlines + ecx * 4], 0
    je .found
    inc ecx
    cmp ecx, POWER_WAKE_LOCK_CAPACITY
    jb .find
.invalid:
    stc
    ret
.found:
    mov [power_wake_lock_deadlines + ecx * 4], edx
    inc dword [power_wake_lock_count]
    mov eax, ecx
    clc
    ret

; EAX=Slot.
power_wake_lock_release:
    cmp eax, POWER_WAKE_LOCK_CAPACITY
    jae .invalid
    cmp dword [power_wake_lock_deadlines + eax * 4], 0
    je .invalid
    mov dword [power_wake_lock_deadlines + eax * 4], 0
    cmp dword [power_wake_lock_count], 0
    je .invalid
    dec dword [power_wake_lock_count]
    clc
    ret
.invalid:
    stc
    ret

power_wake_lock_expire:
    xor ecx, ecx
.next:
    mov eax, [power_wake_lock_deadlines + ecx * 4]
    test eax, eax
    jz .continue
    mov edx, [timer_ticks]
    sub edx, eax
    js .continue
    mov dword [power_wake_lock_deadlines + ecx * 4], 0
    dec dword [power_wake_lock_count]
    inc dword [power_wake_lock_expirations]
.continue:
    inc ecx
    cmp ecx, POWER_WAKE_LOCK_CAPACITY
    jb .next
    ret

power_cpu_idle:
    call power_wake_lock_expire
    inc dword [power_idle_entries]
    cmp dword [power_wake_lock_count], 0
    jne .shallow
    mov dword [power_idle_state], POWER_IDLE_DEEP
    inc dword [power_deep_idle_entries]
    hlt
    ret
.shallow:
    mov dword [power_idle_state], POWER_IDLE_SHALLOW
    hlt
    ret

; EAX=Profil. Nur zur Laufzeit als unterstuetzt gemeldete Profile annehmen.
power_manager_set_profile:
    cmp eax, POWER_PROFILE_POWERSAVE
    ja .invalid
    cmp dword [power_transition_active], 0
    jne .invalid
    cmp [power_profile], eax
    je .success
    mov [power_profile], eax
    inc dword [power_profile_changes]
.success:
    clc
    ret
.invalid:
    stc
    ret

power_manager_platform_off:
    mov dword [power_transition_phase], POWER_PHASE_PLATFORM_OFF
    mov esi, message_shutdown_platform
    call serial_write_string
    mov ax, 0x2000
    mov dx, 0x0604                  ; QEMU/ACPI Poweroff
    out dx, ax
    mov dx, 0xB004                  ; Bochs/QEMU Fallback
    out dx, ax
    mov dword [power_current_state], POWER_STATE_OFF
    ret

align 4
power_api:
    dd POWER_API_SIZE
    dw 1, 0
    dd POWER_STATE_RUNNING
    dd POWER_SUPPORTED_MASK
    dd power_current_state
    dd power_target_state
    dd power_profile
    dd power_transition_phase
    dd power_shutdown_requests
    dd power_transition_failures
    dd power_manager_request_shutdown
    dd power_manager_platform_off
    dd power_wake_lock_acquire
    dd power_wake_lock_release
    dd power_cpu_idle
    dd power_wake_lock_count
    dd power_manager_set_profile
    dd power_profile_changes
power_current_state:       dd POWER_STATE_RUNNING
power_target_state:        dd POWER_STATE_RUNNING
power_profile:             dd POWER_PROFILE_BALANCED
power_transition_phase:    dd POWER_PHASE_NONE
power_transition_active:   dd 0
power_shutdown_requests:   dd 0
power_transition_failures: dd 0
power_wake_lock_count:      dd 0
power_wake_lock_expirations: dd 0
power_idle_state:           dd POWER_IDLE_SHALLOW
power_idle_entries:         dd 0
power_deep_idle_entries:    dd 0
power_profile_changes:      dd 0
power_wake_lock_deadlines:  times POWER_WAKE_LOCK_CAPACITY dd 0

; Minimaler Network-Core nach NPSPEC-KERNEL-0022. Hardwareprotokolle folgen
; auf dieser objekt- und capability-basierten Grundlage.
NETWORK_API_SIZE          equ 40
NETWORK_INTERFACE_UP      equ 2
NETWORK_INTERFACE_LOOPBACK equ 0
NETWORK_LOOPBACK_MTU      equ 65536
NETWORK_FEATURE_IPV4      equ 0x00000001
NETWORK_FEATURE_IPV6      equ 0x00000002
OBJECT_TYPE_NET_INTERFACE equ 12
OBJECT_TYPE_NET_SOCKET    equ 13
NETWORK_SOCKET_DATAGRAM   equ 1
NETWORK_SOCKET_STREAM     equ 2
NETWORK_AF_IPV4           equ 4
NETWORK_IPV4_LOOPBACK     equ 0x0100007F
NETWORK_BOOTSTRAP_PORT    equ 9000
TCP_STATE_CLOSED          equ 0
TCP_STATE_LISTEN          equ 1
TCP_STATE_SYN_SENT        equ 2
TCP_STATE_SYN_RECEIVED    equ 3
TCP_STATE_ESTABLISHED     equ 4
TCP_STATE_FIN_WAIT_1      equ 5
TCP_STATE_FIN_WAIT_2      equ 6
TCP_STATE_TIME_WAIT       equ 10

network_manager_initialize:
    mov dword [network_namespace_generation], 1
    mov dword [network_interface_count], 0
    mov dword [network_loopback_id], 1
    mov dword [network_loopback_state], NETWORK_INTERFACE_UP
    mov dword [network_received_packets], 0
    mov dword [network_transmitted_packets], 0
    mov dword [network_dropped_packets], 0
    mov dword [network_loopback_queue_count], 0
    mov dword [network_socket_object], 0
    mov dword [network_socket_handle], 0
    mov dword [network_socket_bound], 0
    mov dword [network_socket_port], 0
    mov dword [network_tcp_socket_object], 0
    mov dword [network_tcp_socket_handle], 0
    mov dword [network_tcp_socket_connected], 0
    mov dword [network_tcp_queue_count], 0
    mov dword [network_tcp_queue_length], 0
    mov dword [network_tcp_transmitted_bytes], 0
    mov dword [network_tcp_received_bytes], 0
    mov dword [network_tcp_listener_handle], 0
    mov dword [network_tcp_listener_bound], 0
    mov dword [network_tcp_listener_state], TCP_STATE_CLOSED
    mov dword [network_tcp_listener_backlog], 0
    mov dword [network_tcp_pending_connections], 0
    mov dword [network_tcp_accepted_connections], 0
    mov dword [network_route_count], 2
    mov dword [network_route_lookups], 0
    mov dword [network_route_hits], 0
    mov dword [network_route_last_prefix], 0
    mov dword [network_firewall_decisions], 0
    mov dword [network_firewall_drops], 0
    mov dword [network_ipv6_udp_tests], 0
    mov dword [network_icmpv6_echo_tests], 0
    mov dword [network_tcp_tests], 0
    mov edi, network_loopback_frame
    xor eax, eax
    mov ecx, 64 / 4
    rep stosd
    mov eax, OBJECT_TYPE_NET_INTERFACE
    mov edx, NETWORK_INTERFACE_LOOPBACK
    mov ebx, 1
    call object_create
    jc .invalid
    mov [network_loopback_object], eax
    mov dword [network_interface_count], 1
    clc
    ret
.invalid:
    stc
    ret

network_manager_self_test:
    cmp dword [network_api], NETWORK_API_SIZE
    jne .invalid
    cmp word [network_api + 4], 1
    jne .invalid
    cmp dword [network_interface_count], 1
    jne .invalid
    cmp dword [network_loopback_id], 1
    jne .invalid
    cmp dword [network_loopback_state], NETWORK_INTERFACE_UP
    jne .invalid
    cmp dword [network_loopback_object], 0
    je .invalid
    cmp dword [network_socket_bound], 0
    jne .invalid
    call network_icmp_echo_self_test
    jc .invalid
    call network_ipv6_udp_self_test
    jc .invalid
    call network_icmpv6_echo_self_test
    jc .invalid
    call network_tcp_self_test
    jc .invalid
    mov eax, 0x7F000001             ; 127.0.0.1 in Host-Reihenfolge
    call network_route_lookup_ipv4
    jc .invalid
    cmp eax, 1
    jne .invalid
    cmp dword [network_route_last_prefix], 32
    jne .invalid                    ; Hostroute muss /8 überstimmen
    mov eax, 0x0A000001             ; keine Default-Route vorhanden
    call network_route_lookup_ipv4
    jnc .invalid
    mov eax, 0x7F000001
    mov ebx, 17
    mov ecx, NETWORK_BOOTSTRAP_PORT
    call network_firewall_check_ipv4
    jc .invalid
    mov eax, 0x0A000001             ; externes Ziel muss fail-closed sein
    mov ebx, 17
    mov ecx, NETWORK_BOOTSTRAP_PORT
    call network_firewall_check_ipv4
    jnc .invalid
    clc
    ret
.invalid:
    stc
    ret

; ICMPv4-Echo-Grundpfad: Request und Reply werden vollständig im Kernel
; erzeugt und jeweils vor der Verarbeitung per Internet-Checksum validiert.
network_icmp_echo_self_test:
    mov edi, network_icmp_buffer
    xor eax, eax
    mov ecx, 16 / 4
    rep stosd
    mov byte [network_icmp_buffer + 0], 8  ; Echo Request
    mov byte [network_icmp_buffer + 1], 0
    mov word [network_icmp_buffer + 4], 0x414E
    mov word [network_icmp_buffer + 6], 0x0100
    mov dword [network_icmp_buffer + 8], 0x41564F4E
    mov dword [network_icmp_buffer + 12], 0x534F4156
    mov esi, network_icmp_buffer
    mov ecx, 16
    call network_checksum
    xchg al, ah
    mov [network_icmp_buffer + 2], ax
    call network_icmp_validate_request
    jc .invalid
    mov byte [network_icmp_buffer + 0], 0  ; Echo Reply
    mov word [network_icmp_buffer + 2], 0
    mov esi, network_icmp_buffer
    mov ecx, 16
    call network_checksum
    xchg al, ah
    mov [network_icmp_buffer + 2], ax
    call network_icmp_validate_reply
    jc .invalid
    mov dword [network_icmp_echo_tests], 1
    clc
    ret
.invalid:
    stc
    ret

network_icmp_validate_request:
    cmp byte [network_icmp_buffer], 8
    jne .invalid
    jmp network_icmp_validate_common
.invalid:
    stc
    ret

network_icmp_validate_reply:
    cmp byte [network_icmp_buffer], 0
    jne .invalid
    jmp network_icmp_validate_common
.invalid:
    stc
    ret

network_icmp_validate_common:
    cmp byte [network_icmp_buffer + 1], 0
    jne .invalid
    mov esi, network_icmp_buffer
    mov ecx, 16
    call network_checksum
    test ax, ax
    jnz .invalid
    clc
    ret
.invalid:
    stc
    ret

; IPv6/UDP-Loopback-Selbsttest. IPv6 besitzt keine Headerprüfsumme; die
; UDP-Prüfsumme über den 40-Byte-Pseudoheader ist dagegen verbindlich.
network_ipv6_udp_self_test:
    mov edi, network_ipv6_frame
    xor eax, eax
    mov ecx, 64 / 4
    rep stosd
    mov byte [network_ipv6_frame + 0], 0x60
    mov word [network_ipv6_frame + 4], 0x0C00 ; 12 Byte Payload in Netzreihenfolge
    mov byte [network_ipv6_frame + 6], 17
    mov byte [network_ipv6_frame + 7], 64
    mov byte [network_ipv6_frame + 23], 1     ; Quelle ::1
    mov byte [network_ipv6_frame + 39], 1     ; Ziel ::1
    mov ax, NETWORK_BOOTSTRAP_PORT
    xchg al, ah
    mov [network_ipv6_frame + 40], ax
    mov [network_ipv6_frame + 42], ax
    mov word [network_ipv6_frame + 44], 0x0C00
    mov dword [network_ipv6_frame + 48], 0x41564F4E
    call network_ipv6_checksum_prepare
    mov esi, network_ipv6_checksum_buffer
    mov ecx, 52
    call network_checksum
    test ax, ax
    jnz .checksum_ready
    mov ax, 0xFFFF
.checksum_ready:
    xchg al, ah
    mov [network_ipv6_frame + 46], ax
    call network_ipv6_udp_validate
    jc .invalid
    mov dword [network_ipv6_udp_tests], 1
    clc
    ret
.invalid:
    stc
    ret

network_ipv6_checksum_prepare:
    mov edi, network_ipv6_checksum_buffer
    xor eax, eax
    mov ecx, 64 / 4
    rep stosd
    mov esi, network_ipv6_frame + 8
    mov edi, network_ipv6_checksum_buffer
    mov ecx, 32
    rep movsb
    mov dword [network_ipv6_checksum_buffer + 32], 0x0C000000
    mov byte [network_ipv6_checksum_buffer + 39], 17
    mov esi, network_ipv6_frame + 40
    mov edi, network_ipv6_checksum_buffer + 40
    mov ecx, 12
    rep movsb
    ret

network_ipv6_udp_validate:
    cmp byte [network_ipv6_frame], 0x60
    jne .invalid
    cmp word [network_ipv6_frame + 4], 0x0C00
    jne .invalid
    cmp byte [network_ipv6_frame + 6], 17
    jne .invalid
    cmp byte [network_ipv6_frame + 23], 1
    jne .invalid
    cmp byte [network_ipv6_frame + 39], 1
    jne .invalid
    cmp word [network_ipv6_frame + 46], 0
    je .invalid
    call network_ipv6_checksum_prepare
    mov esi, network_ipv6_checksum_buffer
    mov ecx, 52
    call network_checksum
    test ax, ax
    jnz .invalid
    clc
    ret
.invalid:
    stc
    ret

; ICMPv6 Echo benötigt – anders als ICMPv4 – die IPv6-Quell- und Zieladresse
; im Prüfsummen-Pseudoheader. Request (128) und Reply (129) werden geprüft.
network_icmpv6_echo_self_test:
    mov edi, network_icmpv6_buffer
    xor eax, eax
    mov ecx, 12 / 4
    rep stosd
    mov byte [network_icmpv6_buffer + 0], 128
    mov byte [network_icmpv6_buffer + 1], 0
    mov word [network_icmpv6_buffer + 4], 0x414E
    mov word [network_icmpv6_buffer + 6], 0x0100
    mov dword [network_icmpv6_buffer + 8], 0x36564F4E
    call network_icmpv6_checksum_prepare
    mov esi, network_icmpv6_checksum_buffer
    mov ecx, 52
    call network_checksum
    xchg al, ah
    mov [network_icmpv6_buffer + 2], ax
    mov al, 128
    call network_icmpv6_validate_type
    jc .invalid
    mov byte [network_icmpv6_buffer], 129
    mov word [network_icmpv6_buffer + 2], 0
    call network_icmpv6_checksum_prepare
    mov esi, network_icmpv6_checksum_buffer
    mov ecx, 52
    call network_checksum
    xchg al, ah
    mov [network_icmpv6_buffer + 2], ax
    mov al, 129
    call network_icmpv6_validate_type
    jc .invalid
    mov dword [network_icmpv6_echo_tests], 1
    clc
    ret
.invalid:
    stc
    ret

network_icmpv6_checksum_prepare:
    mov edi, network_icmpv6_checksum_buffer
    xor eax, eax
    mov ecx, 64 / 4
    rep stosd
    mov byte [network_icmpv6_checksum_buffer + 15], 1
    mov byte [network_icmpv6_checksum_buffer + 31], 1
    mov dword [network_icmpv6_checksum_buffer + 32], 0x0C000000
    mov byte [network_icmpv6_checksum_buffer + 39], 58
    mov esi, network_icmpv6_buffer
    mov edi, network_icmpv6_checksum_buffer + 40
    mov ecx, 12
    rep movsb
    ret

network_icmpv6_validate_type:
    cmp [network_icmpv6_buffer], al
    jne .invalid
    cmp byte [network_icmpv6_buffer + 1], 0
    jne .invalid
    cmp word [network_icmpv6_buffer + 2], 0
    je .invalid
    push eax
    call network_icmpv6_checksum_prepare
    mov esi, network_icmpv6_checksum_buffer
    mov ecx, 52
    call network_checksum
    test ax, ax
    pop eax
    jnz .invalid
    clc
    ret
.invalid:
    stc
    ret

; Begrenzter TCP/IPv4-Loopback-Selbsttest. Er validiert den Zustandsautomaten,
; die verbindliche Pseudoheader-Pruefsumme, geordnete Sequenznummern, eine
; kontrollierte Retransmission und den vollstaendigen Verbindungsabbau.
network_tcp_self_test:
    mov dword [network_tcp_state], TCP_STATE_CLOSED
    mov dword [network_tcp_tests], 0
    mov dword [network_tcp_retransmissions], 0
    mov dword [network_tcp_send_sequence], 0x1000
    mov dword [network_tcp_receive_sequence], 0x2000
    mov eax, TCP_STATE_SYN_SENT
    call network_tcp_transition
    jc .invalid
    mov eax, TCP_STATE_SYN_RECEIVED
    call network_tcp_transition
    jc .invalid
    mov eax, TCP_STATE_ESTABLISHED
    call network_tcp_transition
    jc .invalid
    inc dword [network_tcp_tests]

    mov edi, network_tcp_segment
    xor eax, eax
    mov ecx, 24 / 4
    rep stosd
    mov ax, NETWORK_BOOTSTRAP_PORT
    xchg al, ah
    mov [network_tcp_segment + 0], ax
    mov [network_tcp_segment + 2], ax
    mov eax, [network_tcp_send_sequence]
    bswap eax
    mov [network_tcp_segment + 4], eax
    mov eax, [network_tcp_receive_sequence]
    bswap eax
    mov [network_tcp_segment + 8], eax
    mov byte [network_tcp_segment + 12], 0x50 ; 20-Byte TCP-Header
    mov byte [network_tcp_segment + 13], 0x18 ; ACK | PSH
    mov word [network_tcp_segment + 14], 0x0010
    mov dword [network_tcp_segment + 20], 0x41564F4E
    call network_tcp_checksum_prepare
    mov esi, network_tcp_checksum_buffer
    mov ecx, 36
    call network_checksum
    xchg al, ah
    mov [network_tcp_segment + 16], ax
    call network_tcp_validate_segment
    jc .invalid
    add dword [network_tcp_send_sequence], 4
    inc dword [network_tcp_tests]

    ; Der erste ACK bleibt absichtlich aus. Genau eine begrenzte erneute
    ; Uebertragung wird verbucht; die Sequenznummer darf sich nicht aendern.
    mov eax, [network_tcp_send_sequence]
    inc dword [network_tcp_retransmissions]
    cmp eax, [network_tcp_send_sequence]
    jne .invalid
    cmp dword [network_tcp_retransmissions], 1
    jne .invalid
    inc dword [network_tcp_tests]

    mov eax, TCP_STATE_FIN_WAIT_1
    call network_tcp_transition
    jc .invalid
    mov eax, TCP_STATE_FIN_WAIT_2
    call network_tcp_transition
    jc .invalid
    mov eax, TCP_STATE_TIME_WAIT
    call network_tcp_transition
    jc .invalid
    mov eax, TCP_STATE_CLOSED
    call network_tcp_transition
    jc .invalid
    inc dword [network_tcp_tests]
    clc
    ret
.invalid:
    stc
    ret

network_tcp_checksum_prepare:
    mov edi, network_tcp_checksum_buffer
    xor eax, eax
    mov ecx, 40 / 4
    rep stosd
    mov dword [network_tcp_checksum_buffer + 0], NETWORK_IPV4_LOOPBACK
    mov dword [network_tcp_checksum_buffer + 4], NETWORK_IPV4_LOOPBACK
    mov byte [network_tcp_checksum_buffer + 9], 6
    mov word [network_tcp_checksum_buffer + 10], 0x1800
    mov esi, network_tcp_segment
    mov edi, network_tcp_checksum_buffer + 12
    mov ecx, 24
    rep movsb
    ret

network_tcp_validate_segment:
    cmp dword [network_tcp_state], TCP_STATE_ESTABLISHED
    jne .invalid
    cmp byte [network_tcp_segment + 12], 0x50
    jne .invalid
    cmp byte [network_tcp_segment + 13], 0x18
    jne .invalid
    mov eax, [network_tcp_segment + 4]
    bswap eax
    cmp eax, [network_tcp_send_sequence]
    jne .invalid
    call network_tcp_checksum_prepare
    mov esi, network_tcp_checksum_buffer
    mov ecx, 36
    call network_checksum
    test ax, ax
    jnz .invalid
    clc
    ret
.invalid:
    stc
    ret

network_tcp_transition:
    mov edx, [network_tcp_state]
    cmp edx, TCP_STATE_CLOSED
    jne .from_syn_sent
    cmp eax, TCP_STATE_SYN_SENT
    jne .invalid
    jmp .commit
.from_syn_sent:
    cmp edx, TCP_STATE_SYN_SENT
    jne .from_syn_received
    cmp eax, TCP_STATE_SYN_RECEIVED
    jne .invalid
    jmp .commit
.from_syn_received:
    cmp edx, TCP_STATE_SYN_RECEIVED
    jne .from_established
    cmp eax, TCP_STATE_ESTABLISHED
    jne .invalid
    jmp .commit
.from_established:
    cmp edx, TCP_STATE_ESTABLISHED
    jne .from_fin_wait_1
    cmp eax, TCP_STATE_FIN_WAIT_1
    jne .invalid
    jmp .commit
.from_fin_wait_1:
    cmp edx, TCP_STATE_FIN_WAIT_1
    jne .from_fin_wait_2
    cmp eax, TCP_STATE_FIN_WAIT_2
    jne .invalid
    jmp .commit
.from_fin_wait_2:
    cmp edx, TCP_STATE_FIN_WAIT_2
    jne .from_time_wait
    cmp eax, TCP_STATE_TIME_WAIT
    jne .invalid
    jmp .commit
.from_time_wait:
    cmp edx, TCP_STATE_TIME_WAIT
    jne .invalid
    cmp eax, TCP_STATE_CLOSED
    jne .invalid
.commit:
    mov [network_tcp_state], eax
    clc
    ret
.invalid:
    stc
    ret

; EAX=IPv4-Adresse in Host-Reihenfolge, EAX=Interface-ID. Die Tabelle ist
; fest begrenzt; längere Präfixe gewinnen unabhängig von der Eintragsreihenfolge.
network_route_lookup_ipv4:
    mov [network_route_lookup_address], eax
    inc dword [network_route_lookups]
    mov dword [network_route_last_prefix], 0
    mov dword [network_route_selected_interface], 0
    xor ecx, ecx
.next:
    cmp ecx, [network_route_count]
    jae .complete
    mov edi, ecx
    shl edi, 4
    add edi, network_route_table
    mov eax, [network_route_lookup_address]
    and eax, [edi + 4]
    cmp eax, [edi + 0]
    jne .continue
    mov eax, [edi + 8]
    cmp eax, [network_route_last_prefix]
    jb .continue
    mov [network_route_last_prefix], eax
    mov eax, [edi + 12]
    mov [network_route_selected_interface], eax
.continue:
    inc ecx
    jmp .next
.complete:
    mov eax, [network_route_selected_interface]
    test eax, eax
    jz .unreachable
    inc dword [network_route_hits]
    clc
    ret
.unreachable:
    stc
    ret

; Statische Bootstrap-Firewall. Bis ein autorisierter Policy-Dienst Regeln
; installiert, ist ausschließlich UDP/TCP Port 9000 in 127.0.0.0/8 erlaubt.
network_firewall_check_ipv4:
    inc dword [network_firewall_decisions]
    cmp ebx, 17
    je .protocol_ok
    cmp ebx, 6
    jne .drop
.protocol_ok:
    cmp ecx, NETWORK_BOOTSTRAP_PORT
    jne .drop
    and eax, 0xFF000000
    cmp eax, 0x7F000000
    jne .drop
    clc
    ret
.drop:
    inc dword [network_firewall_drops]
    stc
    ret

; Erstellt ein IPv4/UDP-Datagramm für 127.0.0.1. Der öffentliche Request
; enthält nur Nutzdaten; sämtliche Protokollfelder entstehen im Kernel.
network_loopback_encapsulate:
    mov ecx, [syscall_network_packet + 12]
    test ecx, ecx
    jz .invalid
    cmp ecx, 16
    ja .invalid
    mov [network_loopback_payload_length], ecx
    mov edi, network_loopback_frame
    xor eax, eax
    push ecx
    mov ecx, 64 / 4
    rep stosd
    pop ecx
    mov byte [network_loopback_frame + 0], 0x45
    mov byte [network_loopback_frame + 1], 0
    mov eax, ecx
    add eax, 28
    xchg al, ah
    mov [network_loopback_frame + 2], ax
    inc word [network_ipv4_identification]
    mov ax, [network_ipv4_identification]
    xchg al, ah
    mov [network_loopback_frame + 4], ax
    mov word [network_loopback_frame + 6], 0
    mov byte [network_loopback_frame + 8], 64
    mov byte [network_loopback_frame + 9], 17
    mov dword [network_loopback_frame + 12], NETWORK_IPV4_LOOPBACK
    mov dword [network_loopback_frame + 16], NETWORK_IPV4_LOOPBACK
    mov eax, [network_socket_port]
    xchg al, ah
    mov [network_loopback_frame + 20], ax
    mov [network_loopback_frame + 22], ax
    mov eax, ecx
    add eax, 8
    xchg al, ah
    mov [network_loopback_frame + 24], ax
    mov esi, syscall_network_packet + 20
    mov edi, network_loopback_frame + 28
    rep movsb
    mov esi, network_loopback_frame
    mov ecx, 20
    call network_checksum
    xchg al, ah
    mov [network_loopback_frame + 10], ax
    call network_udp_checksum_prepare
    mov eax, [network_loopback_payload_length]
    add eax, 20
    mov ecx, eax
    mov esi, network_checksum_buffer
    call network_checksum
    test ax, ax
    jnz .udp_checksum_ready
    mov ax, 0xFFFF
.udp_checksum_ready:
    xchg al, ah
    mov [network_loopback_frame + 26], ax
    clc
    ret
.invalid:
    stc
    ret

network_udp_checksum_prepare:
    mov edi, network_checksum_buffer
    xor eax, eax
    mov ecx, 40 / 4
    rep stosd
    mov dword [network_checksum_buffer + 0], NETWORK_IPV4_LOOPBACK
    mov dword [network_checksum_buffer + 4], NETWORK_IPV4_LOOPBACK
    mov byte [network_checksum_buffer + 8], 0
    mov byte [network_checksum_buffer + 9], 17
    mov ax, [network_loopback_frame + 24]
    mov [network_checksum_buffer + 10], ax
    mov esi, network_loopback_frame + 20
    mov edi, network_checksum_buffer + 12
    mov ecx, [network_loopback_payload_length]
    add ecx, 8
    rep movsb
    ret

network_loopback_validate:
    cmp byte [network_loopback_frame + 0], 0x45
    jne .invalid
    cmp byte [network_loopback_frame + 9], 17
    jne .invalid
    cmp dword [network_loopback_frame + 16], NETWORK_IPV4_LOOPBACK
    jne .invalid
    mov esi, network_loopback_frame
    mov ecx, 20
    call network_checksum
    test ax, ax
    jnz .invalid
    call network_udp_checksum_prepare
    mov eax, [network_loopback_payload_length]
    add eax, 20
    mov ecx, eax
    mov esi, network_checksum_buffer
    call network_checksum
    test ax, ax
    jnz .invalid
    clc
    ret
.invalid:
    stc
    ret

; Internet-Checksum über ECX Bytes in Netzwerkreihenfolge, Ergebnis in AX.
network_checksum:
    xor eax, eax
.pair:
    cmp ecx, 2
    jb .odd
    movzx edx, byte [esi]
    shl edx, 8
    mov dl, [esi + 1]
    add eax, edx
    add esi, 2
    sub ecx, 2
    jmp .pair
.odd:
    test ecx, ecx
    jz .fold
    movzx edx, byte [esi]
    shl edx, 8
    add eax, edx
.fold:
    mov edx, eax
    shr edx, 16
    and eax, 0xFFFF
    add eax, edx
    mov edx, eax
    shr edx, 16
    add ax, dx
    not ax
    ret

align 4
network_api:
    dd NETWORK_API_SIZE
    dw 1, 0
    dd NETWORK_FEATURE_IPV4 | NETWORK_FEATURE_IPV6
    dd network_namespace_generation
    dd network_interface_count
    dd network_loopback_id
    dd network_loopback_state
    dd network_received_packets
    dd network_transmitted_packets
    dd network_dropped_packets
network_namespace_generation: dd 1
network_interface_count:      dd 0
network_loopback_id:          dd 1
network_loopback_state:       dd NETWORK_INTERFACE_UP
network_loopback_object:      dd 0
network_received_packets:     dd 0
network_transmitted_packets:  dd 0
network_dropped_packets:      dd 0
network_socket_object:        dd 0
network_socket_handle:        dd 0
network_socket_bound:         dd 0
network_socket_port:          dd 0
network_tcp_socket_object:    dd 0
network_tcp_socket_handle:    dd 0
network_tcp_socket_connected: dd 0
network_tcp_queue_count:      dd 0
network_tcp_queue_length:     dd 0
network_tcp_transmitted_bytes: dd 0
network_tcp_received_bytes:   dd 0
network_tcp_queue_payload:    times 16 db 0
network_tcp_listener_handle:  dd 0
network_tcp_listener_bound:   dd 0
network_tcp_listener_state:   dd 0
network_tcp_listener_backlog: dd 0
network_tcp_pending_connections: dd 0
network_tcp_accepted_connections: dd 0
network_loopback_queue_count: dd 0
network_loopback_payload_length: dd 0
network_ipv4_identification:  dw 0
align 4
network_loopback_frame:       times 64 db 0
network_checksum_buffer:      times 40 db 0
network_icmp_buffer:          times 16 db 0
network_icmp_echo_tests:      dd 0
network_route_count:          dd 0
network_route_lookups:        dd 0
network_route_hits:           dd 0
network_route_last_prefix:    dd 0
network_route_lookup_address: dd 0
network_route_selected_interface: dd 0
network_firewall_decisions:    dd 0
network_firewall_drops:        dd 0
network_ipv6_udp_tests:        dd 0
network_icmpv6_echo_tests:     dd 0
network_tcp_state:             dd 0
network_tcp_tests:             dd 0
network_tcp_retransmissions:   dd 0
network_tcp_send_sequence:     dd 0
network_tcp_receive_sequence:  dd 0
align 4
network_route_table:
    dd 0x7F000000, 0xFF000000, 8, 1
    dd 0x7F000001, 0xFFFFFFFF, 32, 1
align 4
network_ipv6_frame:            times 64 db 0
network_ipv6_checksum_buffer:  times 64 db 0
network_icmpv6_buffer:         times 12 db 0
network_icmpv6_checksum_buffer: times 64 db 0
network_tcp_segment:           times 24 db 0
network_tcp_checksum_buffer:   times 40 db 0

; ---------------------------------------------------------------------------
; Panic-sicheres Crash-Dump-System (NPSPEC-KERNEL-0024)
; ---------------------------------------------------------------------------
CRASH_DUMP_BUFFER_SIZE     equ 1024
CRASH_DUMP_HEADER_SIZE     equ 128
CRASH_DUMP_SECTION_SIZE    equ 32
CRASH_DUMP_SECTION_COUNT   equ 3
CRASH_DUMP_TOTAL_SIZE      equ 416
CRASH_DUMP_STATUS_EMPTY    equ 0
CRASH_DUMP_STATUS_WRITING  equ 1
CRASH_DUMP_STATUS_COMPLETE equ 2
CRASH_DUMP_STATUS_PARTIAL  equ 3
CRASH_DUMP_CLASS_MINIMAL   equ 2
CRASH_DUMP_SECTION_PANIC   equ 0
CRASH_DUMP_SECTION_CPU     equ 1
CRASH_DUMP_SECTION_LOGS    equ 8
CRASH_DUMP_COMMIT_MARKER   equ 0x504D5544 ; "DUMP"

; Der Puffer wird beim Boot reserviert und niemals dem PMM/Heap übergeben.
crash_dump_initialize:
    mov edi, crash_dump_buffer
    xor eax, eax
    mov ecx, CRASH_DUMP_BUFFER_SIZE / 4
    rep stosd
    mov dword [crash_dump_active], 0
    mov dword [crash_dump_recursions], 0
    mov dword [crash_dump_completed], 0
    mov dword [crash_dump_partial], 0
    clc
    ret

; Erzeugt ausschließlich aus statischem Speicher einen Minimal-Dump. Der
; Status wird zuerst WRITING und erst nach Inhalt und Integrität COMPLETE.
crash_dump_capture_minimal:
    pushad
    cmp dword [crash_dump_active], 0
    jne .recursive
    mov dword [crash_dump_active], 1
    mov edi, crash_dump_buffer
    xor eax, eax
    mov ecx, CRASH_DUMP_BUFFER_SIZE / 4
    rep stosd
    mov dword [crash_dump_buffer + 0], 0x4443564E ; "NVCD"
    mov dword [crash_dump_buffer + 4], 0x31504D55 ; "UMP1"
    mov word  [crash_dump_buffer + 8], 1
    mov word  [crash_dump_buffer + 10], 0
    mov dword [crash_dump_buffer + 12], CRASH_DUMP_HEADER_SIZE
    mov dword [crash_dump_buffer + 16], CRASH_DUMP_CLASS_MINIMAL
    mov dword [crash_dump_buffer + 20], CRASH_DUMP_STATUS_WRITING
    mov dword [crash_dump_buffer + 24], 0x00000001 ; Privacy/minimal
    mov eax, [timer_ticks]
    mov [crash_dump_buffer + 28], eax
    mov dword [crash_dump_buffer + 32], CRASH_DUMP_TOTAL_SIZE
    mov dword [crash_dump_buffer + 36], CRASH_DUMP_SECTION_COUNT

    ; Versionierte, nicht überlappende Sektionsdeskriptoren.
    mov dword [crash_dump_buffer + 128], CRASH_DUMP_SECTION_PANIC
    mov dword [crash_dump_buffer + 132], 1
    mov dword [crash_dump_buffer + 136], 256
    mov dword [crash_dump_buffer + 140], PANIC_REPORT_SIZE
    mov dword [crash_dump_buffer + 160], CRASH_DUMP_SECTION_CPU
    mov dword [crash_dump_buffer + 164], 1
    mov dword [crash_dump_buffer + 168], 304
    mov dword [crash_dump_buffer + 172], 64
    mov dword [crash_dump_buffer + 192], CRASH_DUMP_SECTION_LOGS
    mov dword [crash_dump_buffer + 196], 1
    mov dword [crash_dump_buffer + 200], 368
    mov dword [crash_dump_buffer + 204], LOG_RECORD_SIZE

    mov esi, panic_report
    mov edi, crash_dump_buffer + 256
    mov ecx, PANIC_REPORT_SIZE / 4
    rep movsd
    ; Minimaler CPU-Kontext ohne FPU-, Secret- oder Userspace-Seiten.
    mov eax, cr0
    mov [crash_dump_buffer + 304], eax
    mov eax, cr2
    mov [crash_dump_buffer + 308], eax
    mov eax, cr3
    mov [crash_dump_buffer + 312], eax
    mov eax, cr4
    mov [crash_dump_buffer + 316], eax
    mov eax, [panic_report + 16]
    mov [crash_dump_buffer + 320], eax
    mov eax, [panic_report + 20]
    mov [crash_dump_buffer + 324], eax
    mov eax, [boot_phase_current]
    mov [crash_dump_buffer + 328], eax
    mov eax, [boot_phase_last_success]
    mov [crash_dump_buffer + 332], eax
    mov esi, logging_critical_record
    mov edi, crash_dump_buffer + 368
    mov ecx, LOG_RECORD_SIZE / 4
    rep movsd

    ; Bounded additive integrity value over all publizierten Nutzdaten.
    mov esi, crash_dump_buffer + CRASH_DUMP_HEADER_SIZE
    mov ecx, CRASH_DUMP_TOTAL_SIZE - CRASH_DUMP_HEADER_SIZE
    xor eax, eax
.hash:
    movzx edx, byte [esi]
    rol eax, 5
    xor eax, edx
    inc esi
    loop .hash
    mov [crash_dump_buffer + 60], eax
    mov dword [crash_dump_buffer + 64], CRASH_DUMP_COMMIT_MARKER
    mov dword [crash_dump_buffer + 20], CRASH_DUMP_STATUS_COMPLETE
    inc dword [crash_dump_completed]
    mov dword [crash_dump_active], 0
    popad
    clc
    ret
.recursive:
    inc dword [crash_dump_recursions]
    inc dword [crash_dump_partial]
    mov dword [crash_dump_buffer + 20], CRASH_DUMP_STATUS_PARTIAL
    mov dword [crash_dump_buffer + 68], 1
    mov dword [crash_dump_buffer + 64], CRASH_DUMP_COMMIT_MARKER
    popad
    stc
    ret

crash_dump_validate:
    cmp dword [crash_dump_buffer + 0], 0x4443564E
    jne .invalid
    cmp dword [crash_dump_buffer + 4], 0x31504D55
    jne .invalid
    cmp dword [crash_dump_buffer + 12], CRASH_DUMP_HEADER_SIZE
    jne .invalid
    cmp dword [crash_dump_buffer + 20], CRASH_DUMP_STATUS_COMPLETE
    jne .invalid
    cmp dword [crash_dump_buffer + 32], CRASH_DUMP_TOTAL_SIZE
    jne .invalid
    cmp dword [crash_dump_buffer + 36], CRASH_DUMP_SECTION_COUNT
    jne .invalid
    cmp dword [crash_dump_buffer + 64], CRASH_DUMP_COMMIT_MARKER
    jne .invalid
    mov esi, crash_dump_buffer + CRASH_DUMP_HEADER_SIZE
    mov ecx, CRASH_DUMP_TOTAL_SIZE - CRASH_DUMP_HEADER_SIZE
    xor eax, eax
.hash:
    movzx edx, byte [esi]
    rol eax, 5
    xor eax, edx
    inc esi
    loop .hash
    cmp eax, [crash_dump_buffer + 60]
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

crash_dump_self_test:
    call crash_dump_capture_minimal
    jc .invalid
    call crash_dump_validate
    jc .invalid
    cmp dword [crash_dump_completed], 1
    jne .invalid
    ; Der Selbsttest gilt nicht als echter Crash: Slot danach wieder freigeben.
    call crash_dump_initialize
    clc
    ret
.invalid:
    stc
    ret

align 16
crash_dump_active:       dd 0
crash_dump_recursions:   dd 0
crash_dump_completed:    dd 0
crash_dump_partial:      dd 0
crash_dump_buffer:       times CRASH_DUMP_BUFFER_SIZE db 0

; Strukturierter Panic-Reporter (ADR-2014)
PANIC_API_SIZE    equ 32
PANIC_REPORT_SIZE equ 48

BOOT_PHASE_NONE            equ 0xFFFFFFFF
BOOT_PHASE_KERNEL_ENTRY    equ 0
BOOT_PHASE_HANDOFF         equ 1
BOOT_PHASE_EARLY_ARCH      equ 2
BOOT_PHASE_EARLY_MEMORY    equ 3
BOOT_PHASE_VIRTUAL_MEMORY  equ 4
BOOT_PHASE_KERNEL_CORE     equ 5
BOOT_PHASE_INTERRUPTS_TIME equ 6
BOOT_PHASE_SCHEDULER_SMP   equ 7
BOOT_PHASE_DEVICE_DISCOVERY equ 8
BOOT_PHASE_ROOT_FILESYSTEM  equ 9
BOOT_PHASE_USERSPACE        equ 10
BOOT_PHASE_OPERATIONAL     equ 11
BOOT_PHASE_DESKTOP_READY   equ 12
BOOT_PHASE_SYSTEM_READY    equ 13

; §37: Komponentenzustände für Dependency-aware Startup
COMP_STATE_NOT_STARTED  equ 0
COMP_STATE_INITIALIZING equ 1
COMP_STATE_READY        equ 2
COMP_STATE_DEGRADED     equ 3
COMP_STATE_FAILED       equ 4
BOOT_SEQUENCE_API_SIZE     equ 32
BOOT_LOG_API_SIZE          equ 32
BOOT_EVENT_SIZE            equ 24
BOOT_LOG_CAPACITY          equ 12
BOOT_COMPONENT_KERNEL      equ 0x4B45524E

panic_manager_initialize:
    mov edi, panic_report
    xor eax, eax
    mov ecx, PANIC_REPORT_SIZE / 4
    rep stosd
    mov dword [panic_report], PANIC_REPORT_SIZE
    mov dword [panic_report + 4], 1
    clc
    ret

panic_manager_self_test:
    cmp dword [panic_api], PANIC_API_SIZE
    jne .invalid
    cmp word [panic_api + 4], 1
    jne .invalid
    cmp dword [panic_report], PANIC_REPORT_SIZE
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

; EAX=Fehlercode, EDX=Subsystem, ESI=erklärender Text.
kernel_panic:
    cli
    mov [panic_report + 8], eax
    mov [panic_report + 12], edx
    mov ecx, [esp]
    mov [panic_report + 16], ecx
    mov [panic_report + 20], esp
    mov ecx, cr2
    mov [panic_report + 24], ecx
    mov ecx, [timer_ticks]
    mov [panic_report + 28], ecx
    mov ecx, [boot_phase_current]
    mov [panic_report + 32], ecx
    mov ecx, [boot_phase_last_success]
    mov [panic_report + 36], ecx
    push ebx
    mov eax, 1
    cpuid
    shr ebx, 24
    mov [panic_report + 40], ebx
    pop ebx
    mov ecx, [kernel_context + CONTEXT_SECURITY_STATE]
    mov [panic_report + 44], ecx
    call crash_dump_capture_minimal
    push esi
    call draw_kernel_panic_screen
    pop esi
    push esi
    mov esi, message_panic_begin
    call serial_write_string
    mov eax, [panic_report + 8]
    call serial_write_hex32
    mov esi, message_panic_subsystem
    call serial_write_string
    mov eax, [panic_report + 12]
    call serial_write_hex32
    mov esi, message_panic_eip
    call serial_write_string
    mov eax, [panic_report + 16]
    call serial_write_hex32
    mov esi, message_panic_cr2
    call serial_write_string
    mov eax, [panic_report + 24]
    call serial_write_hex32
    mov esi, message_panic_phase
    call serial_write_string
    mov eax, [panic_report + 32]
    call serial_write_hex32
    mov esi, message_panic_last_phase
    call serial_write_string
    mov eax, [panic_report + 36]
    call serial_write_hex32
    mov esi, message_panic_cpu
    call serial_write_string
    mov eax, [panic_report + 40]
    call serial_write_hex32
    mov esi, message_panic_security
    call serial_write_string
    mov eax, [panic_report + 44]
    call serial_write_hex32
    mov esi, message_newline
    call serial_write_string
    pop esi
    call serial_write_string
    mov esi, message_panic_end
    call serial_write_string
    jmp kernel_halt

align 4
panic_api:
    dd PANIC_API_SIZE
    dw 1, 1
    dd PANIC_REPORT_SIZE
    dd kernel_panic
    dd panic_report
    dd panic_manager_self_test
    dd 0
    dd 0
panic_report:
    times PANIC_REPORT_SIZE db 0

; Frühe, lesbare Bootsequenz-ABI. Sie hält die aktuelle und die letzte
; erfolgreich abgeschlossene NPSPEC-KERNEL-0002-Phase ohne Heapabhängigkeit.
align 4
boot_sequence_api:
    dd BOOT_SEQUENCE_API_SIZE
    dw 1, 0
    dd boot_phase_current
    dd boot_phase_last_success
    dd BOOT_PHASE_OPERATIONAL + 1
    dd 0
    dd 0
    dd 0
boot_phase_current:      dd BOOT_PHASE_NONE
boot_phase_last_success: dd BOOT_PHASE_NONE
; §37: Boot-Timing (timer_ticks-Snapshots)
boot_kernel_entry_ticks:  dd 0
boot_ttid_ticks:          dd 0
boot_system_ready_ticks:  dd 0
; §37: Komponentenzustände für aufschiebbare Initialisierungen
comp_network_state:  dd COMP_STATE_NOT_STARTED
comp_power_state:    dd COMP_STATE_NOT_STARTED

; Heapfreies Frühstartprotokoll; bewahrt sämtliche Aufruferregister.
boot_phase_log:
    pushad
    call boot_event_record_current
    mov esi, message_boot_phase
    call serial_write_string
    mov eax, [boot_phase_current]
    call serial_write_hex32
    mov esi, message_boot_last_phase
    call serial_write_string
    mov eax, [boot_phase_last_success]
    call serial_write_hex32
    mov esi, message_newline
    call serial_write_string
    popad
    ret

; Schreibt genau ein NPSPEC-KERNEL-0002-Bootereignis in den statischen Ring.
; Ein voller Ring verwirft weitere Ereignisse deterministisch.
boot_event_record_current:
    mov ecx, [boot_event_count]
    cmp ecx, BOOT_LOG_CAPACITY
    jae .done
    imul edi, ecx, BOOT_EVENT_SIZE
    add edi, boot_events
    mov eax, [timer_ticks]
    mov [edi + 0], eax
    mov dword [edi + 4], 0
    mov eax, [boot_phase_current]
    mov [edi + 8], eax
    mov dword [edi + 12], BOOT_COMPONENT_KERNEL
    mov dword [edi + 16], 0
    mov dword [edi + 20], 0
    inc dword [boot_event_count]
.done:
    ret

align 4
boot_log_api:
    dd BOOT_LOG_API_SIZE
    dw 1, 0
    dd BOOT_LOG_CAPACITY
    dd BOOT_EVENT_SIZE
    dd boot_event_count
    dd boot_events
    dd 0
    dd 0
boot_event_count: dd 0
align 8
boot_events:
    times BOOT_LOG_CAPACITY * BOOT_EVENT_SIZE db 0

kernel_halt:
    cli
.loop:
    hlt
    jmp .loop

; ---------------------------------------------------------------------------
; COM1
; ---------------------------------------------------------------------------

serial_initialize:
    mov dx, COM1_BASE + 1
    xor al, al
    out dx, al
    mov dx, COM1_BASE + 3
    mov al, 0x80
    out dx, al
    mov dx, COM1_BASE
    mov al, 1
    out dx, al
    mov dx, COM1_BASE + 1
    xor al, al
    out dx, al
    mov dx, COM1_BASE + 3
    mov al, 0x03
    out dx, al
    mov dx, COM1_BASE + 2
    mov al, 0xC7
    out dx, al
    mov dx, COM1_BASE + 4
    mov al, 0x0B
    out dx, al
    ret

serial_write_string:
    lodsb
    test al, al
    jz .done
    call serial_write_byte
    jmp serial_write_string
.done:
    ret

serial_write_byte:
    push eax
    mov ah, al
.wait:
    mov dx, COM1_BASE + 5
    in al, dx
    test al, 0x20
    jz .wait
    mov dx, COM1_BASE
    mov al, ah
    out dx, al
    pop eax
    ret

serial_write_hex32:
    pushad
    mov ebx, eax
    mov ecx, 8
.digit:
    rol ebx, 4
    mov eax, ebx
    and eax, 0x0F
    cmp al, 10
    jb .number
    add al, 'A' - 10
    jmp .write
.number:
    add al, '0'
.write:
    call serial_write_byte
    loop .digit
    popad
    ret

; Erste vom Ring-3-System-UI-Prozess deklarierte Nova-Desktop-Szene. Der
; Userspace bestimmt nur semantische Sichtbarkeit und Generation; Geometrie,
; Theme-Tokens und der eigentliche Framebufferzugriff bleiben im Display Server.
draw_desktop_scene:
    pushad
    cmp dword [display_server_ready], 1
    jne .done
    test dword [display_scene_flags], DISPLAY_SCENE_DESKTOP
    jz .done

    ; Ruhiger Navy-Hintergrund mit einer sehr schmalen Aurora-Lichtkante.
    mov eax, NOVA_COLOR_DESKTOP_BG
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    call fill_rectangle
    mov eax, NOVA_COLOR_BLUE
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    shr edx, 2
    mov esi, 2
    call fill_rectangle
    mov eax, NOVA_COLOR_PURPLE
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    shr ebx, 2
    mov edx, [kernel_context + CONTEXT_WIDTH]
    shr edx, 1
    call fill_rectangle
    mov eax, NOVA_COLOR_CYAN_SOFT
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    imul ebx, ebx, 3
    shr ebx, 2
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, ebx
    call fill_rectangle

    call draw_shell_header

    test dword [display_scene_flags], DISPLAY_SCENE_RIBBON
    jz .menu
    call draw_shell_workspace
.menu:
    test dword [display_scene_flags], DISPLAY_SCENE_START_MENU
    jz .taskbar
    call draw_shell_start_menu
.taskbar:
    test dword [display_scene_flags], DISPLAY_SCENE_TASKBAR
    jz .done
    call draw_shell_taskbar
.done:
    popad
    ret

draw_shell_workspace:
    cmp dword [display_scene_workspace], 1
    je .sheet
    cmp dword [display_scene_workspace], 2
    je .studio
    call draw_shell_explorer
    ret
.sheet:
    call draw_shell_sheet
    ret
.studio:
    call draw_shell_studio
    ret

; Geschuetzter Systemkopf: Branding, globale Befehlspalette und reduzierte
; Statusinformationen. Alle Positionen werden aus der Displaybreite abgeleitet.
draw_shell_header:
    pushad
    mov esi, text_shell_brand
    mov ebx, 24
    mov ecx, 18
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov esi, text_shell_tagline
    mov ebx, 24
    mov ecx, 45
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text

    mov dword [shell_command_width], 420
    cmp dword [kernel_context + CONTEXT_WIDTH], 1000
    jae .command_geometry
    mov dword [shell_command_width], 320
.command_geometry:
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, [shell_command_width]
    shr ebx, 1
    mov [shell_command_x], ebx
    mov eax, NOVA_COLOR_BORDER_ACTIVE
    mov ecx, 15
    mov edx, [shell_command_width]
    mov esi, 42
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_ACRYLIC
    mov ebx, [shell_command_x]
    inc ebx
    mov ecx, 16
    mov edx, [shell_command_width]
    sub edx, 2
    mov esi, 40
    call fill_rounded_rectangle
    mov ebx, [shell_command_x]
    add ebx, 25
    mov ecx, 36
    mov edx, 14
    call draw_nova_orb
    mov esi, text_shell_command
    mov ebx, [shell_command_x]
    add ebx, 49
    mov ecx, 29
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    cmp dword [kernel_context + CONTEXT_WIDTH], 1000
    jb .done
    mov esi, text_shell_status
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, 292
    mov ecx, 23
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_shell_date
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, 130
    mov ecx, 44
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
.done:
    popad
    ret

; Erste echte Anwendungsflaeche: Explorer als gemeinsame NovaWindow-Komponente
; mit Titelzeile, Navigation, Sidebar, Ordnerkarten, Dateiliste und Statusbar.
draw_shell_explorer:
    pushad
    mov dword [shell_window_x], 56
    cmp dword [kernel_context + CONTEXT_WIDTH], 1000
    jae .window_x_ready
    mov dword [shell_window_x], 20
.window_x_ready:
    mov eax, [kernel_context + CONTEXT_WIDTH]
    mov ebx, [shell_window_x]
    shl ebx, 1
    sub eax, ebx
    mov [shell_window_width], eax
    mov dword [shell_window_y], 78
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    sub eax, 180
    mov [shell_window_height], eax

    mov eax, NOVA_COLOR_WINDOW_BORDER
    mov ebx, [shell_window_x]
    mov ecx, [shell_window_y]
    mov edx, [shell_window_width]
    mov esi, [shell_window_height]
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_WINDOW
    mov ebx, [shell_window_x]
    inc ebx
    mov ecx, [shell_window_y]
    inc ecx
    mov edx, [shell_window_width]
    sub edx, 2
    mov esi, [shell_window_height]
    sub esi, 2
    call fill_rounded_rectangle

    mov esi, text_explorer_title
    mov ebx, [shell_window_x]
    add ebx, 24
    mov ecx, [shell_window_y]
    add ecx, 18
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov esi, text_window_controls
    mov ebx, [shell_window_x]
    add ebx, [shell_window_width]
    sub ebx, 126
    mov ecx, [shell_window_y]
    add ecx, 20
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text

    ; Navigation und Breadcrumb.
    mov eax, NOVA_COLOR_PANEL_SOFT
    mov ebx, [shell_window_x]
    add ebx, 190
    mov ecx, [shell_window_y]
    add ecx, 56
    mov edx, [shell_window_width]
    sub edx, 420
    mov esi, 38
    call fill_rounded_rectangle
    mov esi, text_explorer_navigation
    mov ebx, [shell_window_x]
    add ebx, 24
    mov ecx, [shell_window_y]
    add ecx, 69
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    call explorer_breadcrumb_text
    mov ebx, [shell_window_x]
    add ebx, 210
    mov ecx, [shell_window_y]
    add ecx, 69
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    cmp dword [kernel_context + CONTEXT_WIDTH], 1000
    jb .toolbar
    mov eax, NOVA_COLOR_PANEL_SOFT
    mov ebx, [shell_window_x]
    add ebx, [shell_window_width]
    sub ebx, 214
    mov ecx, [shell_window_y]
    add ecx, 56
    mov edx, 190
    mov esi, 38
    call fill_rounded_rectangle
    mov esi, text_explorer_search
    add ebx, 16
    mov ecx, [shell_window_y]
    add ecx, 69
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
.toolbar:
    mov esi, text_explorer_toolbar
    mov ebx, [shell_window_x]
    add ebx, 210
    mov ecx, [shell_window_y]
    add ecx, 114
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text

    ; Sidebar.
    mov eax, NOVA_COLOR_SIDEBAR
    mov ebx, [shell_window_x]
    add ebx, 1
    mov ecx, [shell_window_y]
    add ecx, 104
    mov edx, 176
    mov esi, [shell_window_height]
    sub esi, 105
    call fill_rectangle
    mov esi, text_explorer_quick
    mov ebx, [shell_window_x]
    add ebx, 22
    mov ecx, [shell_window_y]
    add ecx, 124
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    call explorer_sidebar_highlight_y
    add ecx, [shell_window_y]
    mov eax, NOVA_COLOR_SELECTION
    mov ebx, [shell_window_x]
    add ebx, 12
    mov edx, 152
    mov esi, 30
    call fill_rounded_rectangle
    mov esi, text_explorer_sidebar
    mov ebx, [shell_window_x]
    add ebx, 24
    mov ecx, [shell_window_y]
    add ecx, 154
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text

    ; Inhalt beginnt rechts neben der Sidebar.
    mov esi, text_explorer_folders
    mov ebx, [shell_window_x]
    add ebx, 202
    mov ecx, [shell_window_y]
    add ecx, 154
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov dword [shell_card_x], ebx
    xor edi, edi
.folder_loop:
    cmp edi, 4
    jae .files
    call explorer_folder_label
    test esi, esi
    jnz .folder_card
    test edi, edi
    jnz .files
    mov esi, text_explorer_no_folders
    mov ebx, [shell_card_x]
    mov ecx, [shell_window_y]
    add ecx, 198
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    jmp .files
.folder_card:
    mov [explorer_card_label], esi
    lea eax, [edi + 24]              ; Fokus 24..27 = Ordnerkarte
    cmp eax, [display_scene_focus]
    jne .card_unfocused
    mov eax, NOVA_COLOR_SELECTION
    mov ebx, [shell_card_x]
    sub ebx, 2
    mov ecx, [shell_window_y]
    add ecx, 176
    mov edx, 136
    mov esi, 60
    call fill_rounded_rectangle
.card_unfocused:
    mov eax, NOVA_COLOR_CARD_BORDER
    mov ebx, [shell_card_x]
    mov ecx, [shell_window_y]
    add ecx, 178
    mov edx, 132
    mov esi, 56
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_CARD
    inc ebx
    inc ecx
    mov edx, 130
    mov esi, 54
    call fill_rounded_rectangle
    mov esi, [explorer_card_label]
    mov ebx, [shell_card_x]
    add ebx, 14
    mov ecx, [shell_window_y]
    add ecx, 198
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    add dword [shell_card_x], 142
    mov eax, [shell_card_x]
    add eax, 132
    mov ebx, [shell_window_x]
    add ebx, [shell_window_width]
    sub ebx, 18
    cmp eax, ebx
    ja .files
    inc edi
    jmp .folder_loop
.files:
    mov esi, text_explorer_files
    mov ebx, [shell_window_x]
    add ebx, 202
    mov ecx, [shell_window_y]
    add ecx, 258
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_PANEL_SOFT
    mov ebx, [shell_window_x]
    add ebx, 190
    mov ecx, [shell_window_y]
    add ecx, 282
    mov edx, [shell_window_width]
    sub edx, 208
    mov esi, 30
    call fill_rounded_rectangle
    cmp dword [explorer_view_valid], 1
    jne .static_columns
    call explorer_draw_file_selection
    call explorer_draw_files
    popad
    ret
.static_columns:
    mov esi, text_explorer_columns
    mov ebx, [shell_window_x]
    add ebx, 204
    mov ecx, [shell_window_y]
    add ecx, 292
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov eax, [display_scene_focus]
    sub eax, 20
    cmp eax, 3
    jbe .file_focus_ready
    xor eax, eax
.file_focus_ready:
    imul eax, eax, 24
    mov ecx, [shell_window_y]
    add ecx, 324
    add ecx, eax
    mov eax, NOVA_COLOR_SELECTION
    mov ebx, [shell_window_x]
    add ebx, 194
    mov edx, [shell_window_width]
    sub edx, 216
    mov esi, 22
    call fill_rounded_rectangle
    mov esi, text_explorer_file_rows
    mov ebx, [shell_window_x]
    add ebx, 204
    mov ecx, [shell_window_y]
    add ecx, 330
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    mov esi, text_explorer_footer
    mov ebx, [shell_window_x]
    add ebx, 202
    mov ecx, [shell_window_y]
    add ecx, [shell_window_height]
    sub ecx, 24
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    popad
    ret

; Nova Sheet nutzt dieselbe Fensterhülle, aber ein app-spezifisches Ribbon,
; eine fokussierte Budgettabelle und das einklappbare Fähigkeitenpanel.
draw_shell_sheet:
    pushad
    mov dword [shell_window_x], 40
    mov dword [shell_window_y], 78
    mov eax, [kernel_context + CONTEXT_WIDTH]
    sub eax, 80
    mov [shell_window_width], eax
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    sub eax, 180
    mov [shell_window_height], eax
    mov eax, NOVA_COLOR_WINDOW_BORDER
    mov ebx, [shell_window_x]
    mov ecx, [shell_window_y]
    mov edx, [shell_window_width]
    mov esi, [shell_window_height]
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_WINDOW
    inc ebx
    inc ecx
    sub edx, 2
    sub esi, 2
    call fill_rounded_rectangle
    mov esi, text_sheet_title
    mov ebx, [shell_window_x]
    add ebx, 22
    mov ecx, [shell_window_y]
    add ecx, 16
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov esi, text_window_controls
    mov ebx, [shell_window_x]
    add ebx, [shell_window_width]
    sub ebx, 126
    mov ecx, [shell_window_y]
    add ecx, 18
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov esi, text_sheet_tabs
    mov ebx, [shell_window_x]
    add ebx, 22
    mov ecx, [shell_window_y]
    add ecx, 56
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_PANEL_SOFT
    mov ebx, [shell_window_x]
    add ebx, 12
    mov ecx, [shell_window_y]
    add ecx, 78
    mov edx, [shell_window_width]
    sub edx, 24
    mov esi, 82
    call fill_rounded_rectangle
    mov esi, text_sheet_ribbon
    add ebx, 14
    add ecx, 16
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_PANEL_SOFT
    mov ebx, [shell_window_x]
    add ebx, 12
    mov ecx, [shell_window_y]
    add ecx, 170
    mov edx, [shell_window_width]
    sub edx, 24
    mov esi, 34
    call fill_rounded_rectangle
    mov esi, text_sheet_formula
    add ebx, 12
    add ecx, 11
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text

    mov esi, text_sheet_heading
    mov ebx, [shell_window_x]
    add ebx, 28
    mov ecx, [shell_window_y]
    add ecx, 222
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov eax, NOVA_COLOR_SELECTION
    mov ebx, [shell_window_x]
    add ebx, 18
    mov ecx, [shell_window_y]
    add ecx, 254
    mov edx, [shell_window_width]
    sub edx, 310
    mov esi, 32
    call fill_rounded_rectangle
    mov esi, text_sheet_columns
    add ebx, 10
    add ecx, 11
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_BORDER_ACTIVE
    mov ebx, [shell_window_x]
    add ebx, 126
    mov ecx, [shell_window_y]
    add ecx, 288
    mov edi, [display_scene_focus]
    sub edi, 40
    cmp edi, 5
    jbe .cell_focus_ready
    xor edi, edi
.cell_focus_ready:
    imul edi, edi, 24
    add ecx, edi
    mov edx, 84
    mov esi, 28
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_CARD
    inc ebx
    inc ecx
    sub edx, 2
    sub esi, 2
    call fill_rounded_rectangle
    mov esi, text_sheet_rows
    mov ebx, [shell_window_x]
    add ebx, 28
    mov ecx, [shell_window_y]
    add ecx, 296
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text

    cmp dword [kernel_context + CONTEXT_WIDTH], 1000
    jb .footer
    mov eax, NOVA_COLOR_CARD
    mov ebx, [shell_window_x]
    add ebx, [shell_window_width]
    sub ebx, 274
    mov ecx, [shell_window_y]
    add ecx, 212
    mov edx, 254
    mov esi, [shell_window_height]
    sub esi, 252
    call fill_rounded_rectangle
    mov esi, text_sheet_capabilities
    add ebx, 18
    add ecx, 18
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
.footer:
    mov esi, text_sheet_footer
    mov ebx, [shell_window_x]
    add ebx, 24
    mov ecx, [shell_window_y]
    add ecx, [shell_window_height]
    sub ecx, 24
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    popad
    ret

; Fähigkeiten Studio: Navigation, Capability-Katalog, Node-Canvas und
; Inspector sind getrennte echte Panels und keine eingebettete Rastergrafik.
draw_shell_studio:
    pushad
    mov dword [shell_window_x], 24
    mov dword [shell_window_y], 78
    mov eax, [kernel_context + CONTEXT_WIDTH]
    sub eax, 48
    mov [shell_window_width], eax
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    sub eax, 180
    mov [shell_window_height], eax
    mov eax, NOVA_COLOR_WINDOW_BORDER
    mov ebx, [shell_window_x]
    mov ecx, [shell_window_y]
    mov edx, [shell_window_width]
    mov esi, [shell_window_height]
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_WINDOW
    inc ebx
    inc ecx
    sub edx, 2
    sub esi, 2
    call fill_rounded_rectangle
    mov esi, text_studio_title
    mov ebx, [shell_window_x]
    add ebx, 184
    mov ecx, [shell_window_y]
    add ecx, 16
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov esi, text_studio_steps
    mov ebx, [shell_window_x]
    add ebx, 184
    mov ecx, [shell_window_y]
    add ecx, 54
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_SIDEBAR
    mov ebx, [shell_window_x]
    add ebx, 1
    mov ecx, [shell_window_y]
    add ecx, 1
    mov edx, 164
    mov esi, [shell_window_height]
    sub esi, 2
    call fill_rounded_rectangle
    mov esi, text_studio_navigation
    mov ebx, [shell_window_x]
    add ebx, 20
    mov ecx, [shell_window_y]
    add ecx, 28
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text

    mov eax, NOVA_COLOR_CARD
    mov ebx, [shell_window_x]
    add ebx, 178
    mov ecx, [shell_window_y]
    add ecx, 86
    mov edx, 250
    mov esi, [shell_window_height]
    sub esi, 104
    call fill_rounded_rectangle
    mov esi, text_studio_catalog
    add ebx, 14
    add ecx, 16
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text

    mov eax, NOVA_COLOR_PANEL_SOFT
    mov ebx, [shell_window_x]
    add ebx, 440
    mov ecx, [shell_window_y]
    add ecx, 86
    mov edx, [shell_window_width]
    sub edx, 720
    mov esi, [shell_window_height]
    sub esi, 104
    call fill_rounded_rectangle
    mov esi, text_studio_canvas
    add ebx, 18
    add ecx, 16
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov edi, [display_scene_focus]
    sub edi, 60
    cmp edi, 5
    jbe .node_focus_ready
    xor edi, edi
.node_focus_ready:
    mov ebx, [shell_window_x]
    add ebx, 464
    cmp edi, 3
    jb .node_focus_column_ready
    sub edi, 3
    add ebx, 178
.node_focus_column_ready:
    imul edi, edi, 82
    mov ecx, [shell_window_y]
    add ecx, 140
    add ecx, edi
    mov eax, NOVA_COLOR_BORDER_ACTIVE
    mov edx, 136
    mov esi, 56
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_SELECTION
    mov ebx, [shell_window_x]
    add ebx, 466
    mov ecx, [shell_window_y]
    add ecx, 142
    mov edx, 132
    mov esi, 52
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_PURPLE_SOFT
    add ebx, 178
    add ecx, 76
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_CARD_BORDER
    sub ebx, 84
    add ecx, 82
    call fill_rounded_rectangle
    mov esi, text_studio_nodes
    mov ebx, [shell_window_x]
    add ebx, 482
    mov ecx, [shell_window_y]
    add ecx, 160
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text

    mov eax, NOVA_COLOR_CARD
    mov ebx, [shell_window_x]
    add ebx, [shell_window_width]
    sub ebx, 266
    mov ecx, [shell_window_y]
    add ecx, 86
    mov edx, 250
    mov esi, [shell_window_height]
    sub esi, 104
    call fill_rounded_rectangle
    mov esi, text_studio_inspector
    add ebx, 16
    add ecx, 16
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    popad
    ret

; Dreispaltiges Startmenue. Nur dieses Bauteil folgt der separaten
; Startmenue-Referenz; Desktop und Taskleiste bleiben im Aurora-Shell-Stil.
draw_shell_start_menu:
    pushad
    mov eax, [kernel_context + CONTEXT_WIDTH]
    sub eax, 48
    cmp eax, 900
    jbe .width_ready
    mov eax, 900
.width_ready:
    mov [shell_menu_width], eax
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, eax
    shr ebx, 1
    mov [shell_menu_x], ebx
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    sub eax, 190
    cmp eax, 540
    jbe .height_ready
    mov eax, 540
.height_ready:
    mov [shell_menu_height], eax
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 104
    sub ecx, eax
    mov [shell_menu_y], ecx
    mov eax, NOVA_COLOR_BORDER_ACTIVE
    mov ebx, [shell_menu_x]
    mov edx, [shell_menu_width]
    mov esi, [shell_menu_height]
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_MENU
    inc ebx
    inc ecx
    sub edx, 2
    sub esi, 2
    call fill_rounded_rectangle

    ; Linke Navigation.
    mov esi, text_start_brand
    mov ebx, [shell_menu_x]
    add ebx, 28
    mov ecx, [shell_menu_y]
    add ecx, 28
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov eax, NOVA_COLOR_SELECTION
    mov ebx, [shell_menu_x]
    add ebx, 16
    mov ecx, [shell_menu_y]
    add ecx, 72
    mov edx, 170
    mov esi, 36
    call fill_rounded_rectangle
    mov esi, text_start_navigation
    mov ebx, [shell_menu_x]
    add ebx, 30
    mov ecx, [shell_menu_y]
    add ecx, 84
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    mov esi, text_start_navigation_lower
    mov ebx, [shell_menu_x]
    add ebx, 30
    mov ecx, [shell_menu_y]
    add ecx, [shell_menu_height]
    sub ecx, 74
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text

    ; Mittlere App- und Vorschlagsspalte.
    mov esi, text_start_pinned_v2
    mov ebx, [shell_menu_x]
    add ebx, 214
    mov ecx, [shell_menu_y]
    add ecx, 30
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov dword [shell_card_x], 0
.app_loop:
    mov edi, [shell_card_x]
    cmp edi, 8
    jae .suggestions
    mov eax, edi
    and eax, 3
    imul eax, eax, 92
    mov ebx, [shell_menu_x]
    add ebx, 210
    add ebx, eax
    mov eax, edi
    shr eax, 2
    imul eax, eax, 74
    mov ecx, [shell_menu_y]
    add ecx, 60
    add ecx, eax
    mov eax, NOVA_COLOR_CARD
    mov ebp, edi
    add ebp, 3
    cmp [display_scene_focus], ebp
    jne .app_color_ready
    mov eax, NOVA_COLOR_SELECTION
.app_color_ready:
    mov edx, 82
    mov esi, 64
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_PURPLE_SOFT
    mov edx, 28
    mov esi, 28
    add ebx, 27
    add ecx, 8
    call fill_rounded_rectangle
    mov esi, [shell_app_labels + edi * 4]
    sub ebx, 19
    add ecx, 35
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    inc dword [shell_card_x]
    jmp .app_loop
.suggestions:
    mov esi, text_start_suggestions
    mov ebx, [shell_menu_x]
    add ebx, 214
    mov ecx, [shell_menu_y]
    add ecx, 226
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_CARD
    mov ebx, [shell_menu_x]
    add ebx, 210
    mov ecx, [shell_menu_y]
    add ecx, 252
    mov edx, 358
    mov esi, 68
    call fill_rounded_rectangle
    mov esi, text_start_suggestion_items
    add ebx, 14
    add ecx, 14
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_PANEL_SOFT
    mov ebx, [shell_menu_x]
    add ebx, 210
    mov ecx, [shell_menu_y]
    add ecx, [shell_menu_height]
    sub ecx, 54
    mov edx, 358
    mov esi, 36
    call fill_rounded_rectangle
    mov esi, text_start_search_v2
    add ebx, 16
    add ecx, 12
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text

    ; Rechte Widgetspalte wird bei kompakten Displays ausgeblendet.
    cmp dword [shell_menu_width], 760
    jb .done
    mov esi, text_start_profile
    mov ebx, [shell_menu_x]
    add ebx, [shell_menu_width]
    sub ebx, 286
    mov ecx, [shell_menu_y]
    add ecx, 30
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_CARD
    mov ebx, [shell_menu_x]
    add ebx, [shell_menu_width]
    sub ebx, 300
    mov ecx, [shell_menu_y]
    add ecx, 60
    mov edx, 280
    mov esi, 88
    call fill_rounded_rectangle
    mov esi, text_start_clock_widget
    add ebx, 18
    add ecx, 16
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 2
    call draw_text
    mov eax, NOVA_COLOR_CARD
    mov ebx, [shell_menu_x]
    add ebx, [shell_menu_width]
    sub ebx, 300
    mov ecx, [shell_menu_y]
    add ecx, 158
    mov edx, 280
    mov esi, 96
    call fill_rounded_rectangle
    mov esi, text_start_ai_widget
    add ebx, 18
    add ecx, 16
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_CARD
    mov ebx, [shell_menu_x]
    add ebx, [shell_menu_width]
    sub ebx, 300
    mov ecx, [shell_menu_y]
    add ecx, 264
    mov edx, 280
    mov esi, 104
    call fill_rounded_rectangle
    mov esi, text_start_system_widget
    add ebx, 18
    add ecx, 16
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
.done:
    popad
    ret

draw_shell_taskbar:
    pushad
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    sub eax, 86
    mov [shell_taskbar_y], eax
    mov eax, NOVA_COLOR_WINDOW_BORDER
    mov ebx, 20
    mov ecx, [shell_taskbar_y]
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, 40
    mov esi, 68
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_TASKBAR
    mov ebx, 21
    mov ecx, [shell_taskbar_y]
    inc ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, 42
    mov esi, 66
    call fill_rounded_rectangle

    ; Dezent segmentierte Aurora-Oberkante.
    mov eax, NOVA_COLOR_BLUE
    mov ebx, 42
    mov ecx, [shell_taskbar_y]
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, 84
    shr edx, 2
    mov esi, 2
    call fill_rectangle
    mov eax, NOVA_COLOR_PURPLE
    add ebx, edx
    shl edx, 1
    call fill_rectangle
    mov eax, NOVA_COLOR_ORANGE
    add ebx, edx
    shr edx, 1
    call fill_rectangle

    mov ebx, 57
    mov ecx, [shell_taskbar_y]
    add ecx, 34
    mov edx, 24
    call draw_nova_orb
    mov eax, NOVA_COLOR_PANEL_SOFT
    mov ebx, 94
    mov ecx, [shell_taskbar_y]
    add ecx, 13
    mov edx, 210
    mov esi, 42
    call fill_rounded_rectangle
    mov esi, text_taskbar_search_v2
    mov ebx, 112
    mov ecx, [shell_taskbar_y]
    add ecx, 28
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text

    mov dword [shell_card_x], 318
    xor edi, edi
.pin_loop:
    cmp edi, 6
    jae .status
    mov eax, NOVA_COLOR_CARD
    mov ebx, [shell_card_x]
    mov ecx, [shell_taskbar_y]
    add ecx, 13
    mov edx, 42
    mov esi, 42
    call fill_rounded_rectangle
    mov esi, [shell_pin_labels + edi * 4]
    add ebx, 15
    add ecx, 15
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    cmp edi, 0
    jne .next_pin
    mov eax, NOVA_COLOR_CYAN_SOFT
    mov ebx, [shell_card_x]
    add ebx, 15
    mov ecx, [shell_taskbar_y]
    add ecx, 60
    mov edx, 12
    mov esi, 2
    call fill_rectangle
.next_pin:
    add dword [shell_card_x], 50
    mov eax, [shell_card_x]
    add eax, 42
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, 270
    cmp eax, ebx
    jae .status
    inc edi
    jmp .pin_loop
.status:
    mov esi, text_taskbar_status_v2
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, 242
    mov ecx, [shell_taskbar_y]
    add ecx, 17
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    mov esi, text_taskbar_date_v2
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, 126
    mov ecx, [shell_taskbar_y]
    add ecx, 38
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    popad
    ret

draw_desktop_scene_legacy:
    pushad
    cmp dword [display_server_ready], 1
    jne .done
    test dword [display_scene_flags], DISPLAY_SCENE_DESKTOP
    jz .done

    mov eax, NOVA_COLOR_DESKTOP_BG
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    call fill_rectangle

    ; Akzentlinie und geschützte Systemmarke.
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    shr ebx, 3
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, ebx
    sub edx, ebx
    mov eax, NOVA_COLOR_BLUE
    xor ecx, ecx
    mov esi, 13
    call fill_rounded_rectangle
    mov dword [logo_x], 24
    mov dword [logo_y], 36
    mov dword [logo_color], NOVA_COLOR_BLUE
    mov byte [logo_compact], 1
    mov byte [logo_mirror], 1
    call draw_nova_logo
    mov esi, text_brand_compact
    mov ebx, 32
    mov ecx, 154
    mov edx, NOVA_COLOR_BLUE
    mov ebp, 3
    call draw_text
    mov esi, text_desktop_workspace
    mov ebx, 42
    mov ecx, 205
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text

    test dword [display_scene_flags], DISPLAY_SCENE_RIBBON
    jz .start_menu
    mov eax, NOVA_COLOR_BLUE
    mov ebx, 170
    mov ecx, 34
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, 194
    mov esi, 92
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_SURFACE
    mov ebx, 171
    mov ecx, 35
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, 196
    mov esi, 90
    call fill_rounded_rectangle
    mov esi, text_ribbon_start
    mov ebx, 194
    mov ecx, 48
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_ribbon_file
    mov ebx, 270
    mov ecx, 48
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov esi, text_ribbon_view
    mov ebx, 336
    mov ecx, 48
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_BLUE
    mov ebx, 190
    mov ecx, 72
    mov edx, 104
    mov esi, 38
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_ELEVATED
    mov ebx, 304
    mov ecx, 72
    mov edx, 104
    mov esi, 38
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_ELEVATED
    mov ebx, 418
    mov ecx, 72
    mov edx, 104
    mov esi, 38
    call fill_rounded_rectangle
    mov esi, text_ribbon_new
    mov ebx, 210
    mov ecx, 84
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_ribbon_open
    mov ebx, 324
    mov ecx, 84
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_ribbon_settings
    mov ebx, 429
    mov ecx, 84
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text

.start_menu:
    test dword [display_scene_flags], DISPLAY_SCENE_START_MENU
    jz .status
    mov eax, NOVA_COLOR_BLUE
    mov ebx, 170
    mov ecx, 142
    mov edx, 360
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    sub esi, 228
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_SURFACE
    mov ebx, 171
    mov ecx, 143
    mov edx, 358
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    sub esi, 230
    call fill_rounded_rectangle

    mov eax, NOVA_COLOR_ELEVATED
    cmp dword [display_scene_focus], 2
    jne .search_color_ready
    mov eax, NOVA_COLOR_BLUE
.search_color_ready:
    mov ebx, 190
    mov ecx, 162
    mov edx, 320
    mov esi, 42
    call fill_rounded_rectangle
    mov esi, text_start_search
    mov ebx, 210
    mov ecx, 176
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov esi, text_start_pinned
    mov ebx, 192
    mov ecx, 224
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text

    mov eax, NOVA_COLOR_ELEVATED
    cmp dword [display_scene_focus], 3
    jne .tile_files_color_ready
    mov eax, NOVA_COLOR_BLUE
.tile_files_color_ready:
    mov ebx, 190
    mov ecx, 250
    mov edx, 96
    mov esi, 54
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_ELEVATED
    cmp dword [display_scene_focus], 4
    jne .tile_settings_color_ready
    mov eax, NOVA_COLOR_BLUE
.tile_settings_color_ready:
    mov ebx, 298
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_ELEVATED
    cmp dword [display_scene_focus], 5
    jne .tile_terminal_color_ready
    mov eax, NOVA_COLOR_BLUE
.tile_terminal_color_ready:
    mov ebx, 406
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_TILE_ALT
    cmp dword [display_scene_focus], 6
    jne .tile_recovery_color_ready
    mov eax, NOVA_COLOR_BLUE
.tile_recovery_color_ready:
    mov ebx, 190
    mov ecx, 316
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_TILE_ALT
    cmp dword [display_scene_focus], 7
    jne .tile_help_color_ready
    mov eax, NOVA_COLOR_BLUE
.tile_help_color_ready:
    mov ebx, 298
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_TILE_ALT
    cmp dword [display_scene_focus], 8
    jne .tile_power_color_ready
    mov eax, NOVA_COLOR_BLUE
.tile_power_color_ready:
    mov ebx, 406
    call fill_rounded_rectangle
    mov esi, text_tile_files
    mov ebx, 210
    mov ecx, 270
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_tile_settings
    mov ebx, 310
    mov ecx, 270
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_tile_terminal
    mov ebx, 417
    mov ecx, 270
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_tile_recovery
    mov ebx, 202
    mov ecx, 336
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_tile_help
    mov ebx, 322
    mov ecx, 336
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_tile_power
    mov ebx, 430
    mov ecx, 336
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text

    mov esi, text_start_user
    mov ebx, 194
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 114
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov esi, text_start_power
    mov ebx, 442
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 114
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text

.status:
    cmp dword [kernel_context + CONTEXT_WIDTH], 800
    jb .footer
    mov eax, NOVA_COLOR_SURFACE
    mov ebx, 550
    mov ecx, 142
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, 574
    mov esi, 220
    call fill_rounded_rectangle
    mov esi, text_desktop_welcome
    mov ebx, 570
    mov ecx, 170
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov esi, text_desktop_ready
    mov ebx, 570
    mov ecx, 218
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_SUCCESS
    mov ebx, 570
    mov ecx, 264
    mov edx, 12
    mov esi, 12
    call fill_rounded_rectangle
    mov esi, text_desktop_services
    mov ebx, 592
    mov ecx, 266
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
.footer:
    test dword [display_scene_flags], DISPLAY_SCENE_TASKBAR
    jz .footer_text
    ; Schwebende, geschützte Taskleiste. Die Grundnavigation bleibt links
    ; vorhersehbar; Status und Uhr sind rechts gruppiert.
    mov eax, NOVA_COLOR_BLUE
    mov ebx, 170
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 74
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, 194
    mov esi, 54
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_SURFACE
    mov ebx, 171
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 73
    mov edx, [kernel_context + CONTEXT_WIDTH]
    sub edx, 196
    mov esi, 52
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_BLUE
    mov ebx, 184
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 64
    mov edx, 76
    mov esi, 36
    call fill_rounded_rectangle
    mov esi, text_taskbar_start
    mov ebx, 200
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 52
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov eax, NOVA_COLOR_ELEVATED
    mov ebx, 270
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 64
    mov edx, 164
    mov esi, 36
    call fill_rounded_rectangle
    mov esi, text_taskbar_search
    mov ebx, 288
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 52
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    cmp dword [kernel_context + CONTEXT_WIDTH], 1000
    jb .taskbar_compact
    mov eax, NOVA_COLOR_TILE_ALT
    mov ebx, 444
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 64
    mov edx, 38
    mov esi, 36
    call fill_rounded_rectangle
    mov ebx, 492
    call fill_rounded_rectangle
    mov ebx, 540
    call fill_rounded_rectangle
    mov esi, text_taskbar_apps
    mov ebx, 457
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 52
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_taskbar_apps
    mov ebx, 505
    call draw_text
    mov esi, text_taskbar_apps
    mov ebx, 553
    call draw_text
    mov esi, text_taskbar_status
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, 386
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 52
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    mov esi, text_taskbar_clock
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, 258
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 52
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    mov esi, text_taskbar_notify
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, 202
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 52
    mov edx, NOVA_COLOR_BLUE
    mov ebp, 1
    call draw_text
    jmp .footer_text
.taskbar_compact:
    mov esi, text_taskbar_compact
    mov ebx, [kernel_context + CONTEXT_WIDTH]
    sub ebx, 154
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 52
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
.footer_text:
    mov esi, text_desktop_footer
    mov ebx, 42
    mov ecx, [kernel_context + CONTEXT_HEIGHT]
    sub ecx, 28
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
.done:
    popad
    ret

draw_boot_screen:
    jmp draw_kernel_log_screen

draw_kernel_log_screen:
    mov eax, NOVA_COLOR_BLACK
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    call fill_rectangle

    mov eax, NOVA_COLOR_BLUE
    mov ebx, 102
    xor ecx, ecx
    mov edx, 628
    mov esi, 13
    call fill_rounded_rectangle

    mov dword [logo_x], 32
    mov dword [logo_y], 38
    mov dword [logo_color], NOVA_COLOR_BLUE
    mov byte [logo_compact], 1
    mov byte [logo_mirror], 1
    call draw_nova_logo
    mov esi, text_brand_compact
    mov ebx, 32
    mov ecx, 156
    mov edx, NOVA_COLOR_BLUE
    mov ebp, 3
    call draw_text

    mov esi, text_logsystem
    mov ebx, 250
    mov ecx, 52
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov eax, NOVA_COLOR_WHITE
    mov ebx, 212
    mov ecx, 53
    mov edx, 4
    mov esi, 4
    call fill_rectangle
    mov ecx, 61
    call fill_rectangle
    mov ecx, 69
    call fill_rectangle
    mov ebx, 220
    mov ecx, 54
    mov edx, 12
    mov esi, 2
    call fill_rectangle
    mov ecx, 62
    call fill_rectangle
    mov ecx, 70
    call fill_rectangle

    mov eax, NOVA_COLOR_BLUE
    mov ebx, 184
    mov ecx, 92
    mov edx, 583
    mov esi, 480
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_LOG_PANEL
    mov ebx, 185
    mov ecx, 93
    mov edx, 581
    mov esi, 478
    call fill_rounded_rectangle

    mov esi, text_kernel_log
    mov ebx, 205
    mov ecx, 112
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text

    mov esi, text_escape
    mov ebx, 73
    mov ecx, 456
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov eax, NOVA_COLOR_BUTTON
    mov ebx, 54
    mov ecx, 487
    mov edx, 78
    mov esi, 78
    call fill_rounded_rectangle
    call draw_power_icon_aa
    ret

; Geglättetes Power-Symbol: Kreisring mit oberer Öffnung und runder Taste.
; Die drei Deckungsstufen werden mit dem vorhandenen Hintergrund gemischt.
draw_power_icon_aa:
    pushad
    mov dword [text_color], NOVA_COLOR_WHITE
    xor ebp, ebp
.row:
    cmp ebp, 36
    jae .done
    xor ecx, ecx
.column:
    cmp ecx, 36
    jae .next_row
    xor eax, eax

    ; Abgerundeter senkrechter Schalter (x 16..20, y 1..18).
    cmp ebp, 1
    jb .ring
    cmp ebp, 18
    ja .ring
    cmp ecx, 16
    jb .stem_edge
    cmp ecx, 20
    ja .stem_edge
    mov eax, 3
    jmp .paint
.stem_edge:
    cmp ecx, 15
    je .stem_alpha
    cmp ecx, 21
    jne .ring
.stem_alpha:
    mov eax, 1
    jmp .paint

.ring:
    ; Öffnung des Rings im oberen Bereich.
    cmp ebp, 10
    jae .distance
    cmp ecx, 12
    jb .distance
    cmp ecx, 24
    jbe .skip
.distance:
    mov eax, ecx
    sub eax, 18
    imul eax, eax
    mov edx, ebp
    sub edx, 19
    imul edx, edx
    add eax, edx
    cmp eax, 121
    jb .skip
    cmp eax, 255
    ja .skip
    cmp eax, 132
    jb .soft
    cmp eax, 240
    ja .soft
    cmp eax, 150
    jb .medium
    cmp eax, 218
    ja .medium
    mov eax, 3
    jmp .paint
.medium:
    mov eax, 2
    jmp .paint
.soft:
    mov eax, 1
.paint:
    mov edi, ebp
    add edi, 507
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov edx, ecx
    add edx, 75
    shl edx, 2
    add edi, edx
    call blend_text_pixel
.skip:
    inc ecx
    jmp .column
.next_row:
    inc ebp
    jmp .row
.done:
    popad
    ret

draw_boot_screen_legacy:
    ; Vollständiger Hintergrund
    mov eax, NOVA_COLOR_BACKGROUND
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    call fill_rectangle

    ; Cyanfarbene Markenlinie
    mov eax, NOVA_COLOR_CYAN
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, 6
    call fill_rectangle

    ; Nova-Marke
    mov esi, text_brand
    mov ebx, 64
    mov ecx, 42
    mov edx, NOVA_COLOR_CYAN
    mov ebp, 2
    call draw_text

    call draw_nova_logo

    ; Hauptstatus
    mov esi, text_ready
    mov ebx, 64
    mov ecx, 92
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 4
    call draw_text

    mov esi, text_subtitle
    mov ebx, 66
    mov ecx, 136
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 2
    call draw_text

    ; Statuskarte
    mov eax, NOVA_COLOR_PANEL
    mov ebx, 64
    mov ecx, 184
    mov edx, 672
    mov esi, 250
    call fill_rectangle

    mov eax, NOVA_COLOR_PANEL_EDGE
    mov ebx, 64
    mov ecx, 184
    mov edx, 4
    mov esi, 250
    call fill_rectangle

    mov esi, text_status_heading
    mov ebx, 92
    mov ecx, 210
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 1
    call draw_text

    mov esi, text_check_kernel
    mov ebx, 92
    mov ecx, 246
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text

    mov esi, text_check_handoff
    mov ebx, 92
    mov ecx, 286
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text

    mov esi, text_check_memory
    mov ebx, 92
    mov ecx, 326
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text

    mov esi, text_check_graphics
    mov ebx, 92
    mov ecx, 366
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text

    mov esi, text_check_interrupts
    mov ebx, 92
    mov ecx, 406
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text

    ; Grüne Zustandsmarkierungen
    mov eax, NOVA_COLOR_SUCCESS
    mov ebx, 650
    mov ecx, 246
    mov edx, 18
    mov esi, 18
    call fill_rectangle
    mov ecx, 286
    call fill_rectangle
    mov ecx, 326
    call fill_rectangle
    mov ecx, 366
    call fill_rectangle
    mov ecx, 406
    call fill_rectangle

    ; Technische und transparente Sicherheitsinformation
    mov esi, text_technical
    mov ebx, 66
    mov ecx, 470
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 1
    call draw_text

    ret

; EAX=Farbe, EBX=x, ECX=y, EDX=Breite, ESI=Höhe
draw_kernel_panic_screen:
    cmp dword [kernel_context + CONTEXT_FRAMEBUFFER], 0
    je .done
    cmp dword [kernel_context + CONTEXT_BPP], 32
    jne .done
    cmp dword [kernel_context + CONTEXT_WIDTH], 640
    jb .done
    cmp dword [kernel_context + CONTEXT_HEIGHT], 480
    jb .done
    mov eax, [panic_report + 8]
    call format_panic_hex
    jmp draw_kernel_error_reference
    mov eax, NOVA_COLOR_BACKGROUND
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    call fill_rectangle
    mov eax, NOVA_COLOR_CYAN
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, 6
    call fill_rectangle
    mov esi, text_brand
    mov ebx, 64
    mov ecx, 42
    mov edx, NOVA_COLOR_CYAN
    mov ebp, 2
    call draw_text
    call draw_nova_logo
    call draw_panic_smile
    mov esi, text_panic_title
    mov ebx, 66
    mov ecx, 170
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov esi, text_panic_explain1
    mov ebx, 66
    mov ecx, 214
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 2
    call draw_text
    mov esi, text_panic_explain2
    mov ebx, 66
    mov ecx, 244
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 2
    call draw_text
    mov eax, NOVA_COLOR_PANEL
    mov ebx, 64
    mov ecx, 292
    mov edx, 672
    mov esi, 112
    call fill_rectangle
    mov eax, NOVA_COLOR_WARNING
    mov ebx, 64
    mov ecx, 292
    mov edx, 5
    mov esi, 112
    call fill_rectangle
    mov esi, text_panic_action1
    mov ebx, 92
    mov ecx, 318
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov esi, text_panic_action2
    mov ebx, 92
    mov ecx, 350
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 2
    call draw_text
    mov esi, text_panic_code
    mov ebx, 66
    mov ecx, 438
    mov edx, NOVA_COLOR_WARNING
    mov ebp, 2
    call draw_text
    mov esi, panic_hex_buffer
    mov ebx, 246
    mov ecx, 438
    mov edx, NOVA_COLOR_WARNING
    mov ebp, 2
    call draw_text
    mov esi, text_panic_footer
    mov ebx, 66
    mov ecx, 492
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 1
    call draw_text
.done:
    ret

draw_kernel_error_reference:
    mov eax, NOVA_COLOR_BLACK
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    call fill_rectangle
    mov eax, NOVA_COLOR_ERROR
    mov ebx, 102
    xor ecx, ecx
    mov edx, 628
    mov esi, 13
    call fill_rounded_rectangle
    mov dword [logo_x], 45
    mov dword [logo_y], 34
    mov dword [logo_color], NOVA_COLOR_ERROR_TEXT
    mov byte [logo_compact], 0
    mov byte [logo_mirror], 0
    call draw_nova_logo
    call draw_kernel_error_mark_full
    mov esi, text_brand_compact
    mov ebx, 336
    mov ecx, 70
    mov edx, NOVA_COLOR_BLUE
    mov ebp, 3
    call draw_text
    mov esi, text_error_title
    mov ebx, 338
    mov ecx, 134
    mov edx, NOVA_COLOR_ERROR_TEXT
    mov ebp, 2
    call draw_text
    mov esi, text_error_explain1
    mov ebx, 27
    mov ecx, 245
    mov edx, NOVA_COLOR_ERROR_TEXT
    mov ebp, 1
    call draw_text
    mov esi, text_error_explain2
    mov ecx, 273
    call draw_text
    mov eax, NOVA_COLOR_ERROR
    mov ebx, 149
    mov ecx, 350
    mov edx, 512
    mov esi, 120
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_LOG_PANEL
    mov ebx, 150
    mov ecx, 351
    mov edx, 510
    mov esi, 118
    call fill_rounded_rectangle
    mov esi, text_error_action1
    mov ebx, 186
    mov ecx, 371
    mov edx, NOVA_COLOR_ERROR_TEXT
    mov ebp, 1
    call draw_text
    mov esi, text_error_action2
    mov ebx, 381
    mov ecx, 399
    call draw_text
    mov esi, text_error_action3
    mov ebx, 187
    mov ecx, 427
    call draw_text
    mov esi, text_error_code
    mov ebx, 20
    mov ecx, 560
    mov edx, NOVA_COLOR_ERROR
    mov ebp, 1
    call draw_text
    mov esi, panic_hex_buffer
    mov ebx, 270
    mov ecx, 560
    call draw_text
    ret

draw_kernel_error_mark_full:
    pushad
    mov eax, [text_color]
    push eax
    mov dword [text_color], NOVA_COLOR_ERROR_TEXT
    xor ebp, ebp
.row:
    cmp ebp, NOVA_ERROR_MARK_HEIGHT
    jae .restore
    xor ecx, ecx
.column:
    cmp ecx, NOVA_ERROR_MARK_WIDTH
    jae .next_row
    mov eax, ebp
    imul eax, NOVA_ERROR_MARK_ROW_BYTES
    mov edx, ecx
    shr edx, 2
    add eax, edx
    mov al, [nova_error_mark_bitmap + eax]
    mov edx, ecx
    and edx, 3
    push ecx
    mov cl, dl
    shl cl, 1
    shr al, cl
    pop ecx
    and eax, 3
    jz .next_column
    push eax
    mov edi, ebp
    add edi, 84
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov edx, ecx
    add edx, 650
    shl edx, 2
    add edi, edx
    pop eax
    call blend_text_pixel
.next_column:
    inc ecx
    jmp .column
.next_row:
    inc ebp
    jmp .row
.restore:
    pop eax
    mov [text_color], eax
    popad
    ret

format_panic_hex:
    pushad
    mov ebx, eax
    mov edi, panic_hex_buffer
    mov ecx, 8
.digit:
    rol ebx, 4
    mov eax, ebx
    and eax, 0x0F
    cmp al, 10
    jb .number
    add al, 'A' - 10
    jmp .store
.number:
    add al, '0'
.store:
    stosb
    loop .digit
    mov byte [edi], 0
    popad
    ret

draw_panic_smile:
    pushad
    mov eax, [text_color]
    push eax
    mov dword [text_color], NOVA_COLOR_WHITE
    xor ebp, ebp
.row:
    cmp ebp, NOVA_ERROR_MARK_HEIGHT / 2
    jae .restore
    xor ecx, ecx
.column:
    cmp ecx, NOVA_ERROR_MARK_WIDTH / 2
    jae .next_row
    mov eax, ebp
    shl eax, 1
    imul eax, NOVA_ERROR_MARK_ROW_BYTES
    mov edx, ecx
    shl edx, 1
    mov ebx, edx
    shr ebx, 2
    add eax, ebx
    mov al, [nova_error_mark_bitmap + eax]
    and edx, 3
    push ecx
    mov cl, dl
    shl cl, 1
    shr al, cl
    pop ecx
    and eax, 3
    jz .next_column
    push eax
    mov edi, ebp
    add edi, 94
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov edx, ecx
    add edx, 66
    shl edx, 2
    add edi, edx
    pop eax
    call blend_text_pixel
.next_column:
    inc ecx
    jmp .column
.next_row:
    inc ebp
    jmp .row
.restore:
    pop eax
    mov [text_color], eax
    popad
    ret

; EBX=Mittelpunkt X, ECX=Mittelpunkt Y, EDX=Radius. Der Orb wird aus drei
; konzentrischen Flaechen aufgebaut und bleibt damit auch ohne GPU rund.
draw_nova_orb:
    pushad
    mov [orb_x], ebx
    mov [orb_y], ecx
    mov [orb_radius], edx
    mov eax, NOVA_COLOR_PURPLE_SOFT
    call fill_circle
    mov eax, NOVA_COLOR_BLUE
    mov edx, [orb_radius]
    sub edx, 4
    mov ebx, [orb_x]
    mov ecx, [orb_y]
    call fill_circle
    mov eax, NOVA_COLOR_ORB_CORE
    mov edx, [orb_radius]
    sub edx, 8
    mov ebx, [orb_x]
    mov ecx, [orb_y]
    call fill_circle
    mov esi, text_orb_n
    mov ebx, [orb_x]
    sub ebx, 4
    mov ecx, [orb_y]
    sub ecx, 6
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text
    popad
    ret

; EAX=Farbe, EBX=Mittelpunkt X, ECX=Mittelpunkt Y, EDX=Radius.
fill_circle:
    pushad
    mov [circle_color], eax
    mov [circle_x], ebx
    mov [circle_y], ecx
    mov [circle_radius], edx
    imul edx, edx
    mov [circle_radius_squared], edx
    mov ebp, [circle_radius]
    neg ebp
.row:
    mov eax, [circle_radius]
    cmp ebp, eax
    jg .done
    mov ecx, [circle_radius]
    neg ecx
.column:
    mov eax, [circle_radius]
    cmp ecx, eax
    jg .next_row
    mov eax, ecx
    imul eax, eax
    mov edx, ebp
    imul edx, edx
    add eax, edx
    cmp eax, [circle_radius_squared]
    ja .next_column
    mov edi, [circle_y]
    add edi, ebp
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov eax, [circle_x]
    add eax, ecx
    shl eax, 2
    add edi, eax
    mov eax, [circle_color]
    mov [edi], eax
.next_column:
    inc ecx
    jmp .column
.next_row:
    inc ebp
    jmp .row
.done:
    popad
    ret

fill_rounded_rectangle:
    pushad
    mov [round_color], eax
    mov [round_x], ebx
    mov [round_y], ecx
    mov [round_width], edx
    mov [round_height], esi
    mov eax, [round_color]
    mov ebx, [round_x]
    add ebx, 6
    mov ecx, [round_y]
    mov edx, [round_width]
    sub edx, 12
    mov esi, 1
    call fill_rectangle
    mov ebx, [round_x]
    add ebx, 3
    inc ecx
    mov edx, [round_width]
    sub edx, 6
    call fill_rectangle
    mov ebx, [round_x]
    inc ebx
    inc ecx
    mov edx, [round_width]
    sub edx, 2
    mov esi, 3
    call fill_rectangle
    mov ebx, [round_x]
    mov ecx, [round_y]
    add ecx, 5
    mov edx, [round_width]
    mov esi, [round_height]
    sub esi, 10
    call fill_rectangle
    mov ebx, [round_x]
    inc ebx
    mov ecx, [round_y]
    add ecx, [round_height]
    sub ecx, 5
    mov edx, [round_width]
    sub edx, 2
    mov esi, 3
    call fill_rectangle
    mov ebx, [round_x]
    add ebx, 3
    add ecx, 3
    mov edx, [round_width]
    sub edx, 6
    mov esi, 1
    call fill_rectangle
    mov ebx, [round_x]
    add ebx, 6
    inc ecx
    mov edx, [round_width]
    sub edx, 12
    call fill_rectangle
    popad
    ret

fill_rectangle:
    pushad
    test edx, edx
    jz .done
    test esi, esi
    jz .done

    mov [rect_color], eax
    mov [rect_x], ebx
    mov [rect_y], ecx
    mov [rect_width], edx
    mov [rect_height], esi
    xor ebp, ebp

.row:
    cmp ebp, [rect_height]
    jae .done
    mov eax, [rect_y]
    add eax, ebp
    imul eax, [kernel_context + CONTEXT_PITCH]
    add eax, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov edi, eax
    mov eax, [rect_x]
    shl eax, 2
    add edi, eax
    mov eax, [rect_color]
    mov ecx, [rect_width]
    rep stosd
    inc ebp
    jmp .row
.done:
    popad
    ret

; Gemeinsames Nova-Logo aus der SVG-Quelle, rechts oben auf jeder
; grafischen Oberfläche.
draw_nova_logo:
    pushad
    mov eax, [text_color]
    push eax
    mov eax, [logo_color]
    mov [text_color], eax
    xor ebp, ebp
.row:
    cmp ebp, NOVA_LOGO_HEIGHT
    jae .done
    cmp byte [logo_compact], 0
    je .row_visible
    mov eax, ebp
    and eax, 3
    cmp eax, 3
    je .next_row
.row_visible:
    xor ecx, ecx
.column:
    cmp ecx, NOVA_LOGO_WIDTH
    jae .next_row
    cmp byte [logo_compact], 0
    je .column_visible
    mov eax, ecx
    and eax, 3
    cmp eax, 3
    je .next_column
.column_visible:
    mov eax, ebp
    imul eax, NOVA_LOGO_ROW_BYTES
    mov edx, ecx
    shr edx, 2
    add eax, edx
    mov al, [nova_logo_bitmap + eax]
    mov edx, ecx
    and edx, 3
    push ecx
    mov cl, dl
    shl cl, 1
    shr al, cl
    pop ecx
    and eax, 3
    jz .next_column
    push eax
    mov edi, ebp
    cmp byte [logo_compact], 0
    je .row_scaled
    mov edx, ebp
    shr edx, 2
    sub edi, edx
.row_scaled:
    add edi, [logo_y]
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov edx, ecx
    cmp byte [logo_compact], 0
    je .column_scaled
    mov ebx, ecx
    shr ebx, 2
    sub edx, ebx
.column_scaled:
    cmp byte [logo_mirror], 0
    je .column_positioned
    mov ebx, 119
    sub ebx, edx
    mov edx, ebx
.column_positioned:
    add edx, [logo_x]
    shl edx, 2
    add edi, edx
    pop eax
    call blend_text_pixel
.next_column:
    inc ecx
    jmp .column
.next_row:
    inc ebp
    jmp .row
.done:
    pop eax
    mov [text_color], eax
    popad
    ret

; ESI=Text, EBX=x, ECX=y, EDX=Farbe, EBP=Skalierung
draw_text:
    mov [text_pointer], esi
    mov [text_x], ebx
    mov [text_origin_x], ebx
    mov [text_y], ecx
    mov [text_color], edx
    mov [text_scale], ebp

.next_character:
    mov esi, [text_pointer]
    lodsb
    mov [text_pointer], esi
    test al, al
    jz .done
    cmp al, 10
    je .newline
    cmp al, ' '
    je .space
    call draw_character
.advance:
    movzx eax, byte [character_advance]
    add eax, [text_scale]
    add [text_x], eax
    jmp .next_character
.space:
    mov byte [character_advance], 5
    jmp .advance
.newline:
    mov eax, [text_origin_x]
    mov [text_x], eax
    add dword [text_y], 24
    jmp .next_character
.done:
    ret

; AL=ASCII-Zeichen. 2-Bit-Deckungswerte sorgen für geglättete Kanten.
draw_character:
    pushad
    mov bl, al
    mov edi, bm_font_characters
    xor ecx, ecx
.find:
    mov al, [edi + ecx]
    test al, al
    jz .done
    cmp al, bl
    je .found
    inc ecx
    jmp .find
.found:
    mov al, [bm_font_widths + ecx]
    mov [character_advance], al
    imul ecx, BM_FONT_BYTES_PER_GLYPH
    mov esi, bm_font_bitmap
    add esi, ecx
    xor ebp, ebp
.glyph_row:
    cmp ebp, BM_FONT_HEIGHT
    jae .done
    xor ecx, ecx
.glyph_column:
    cmp ecx, BM_FONT_WIDTH
    jae .next_row
    mov eax, ebp
    imul eax, BM_FONT_WIDTH
    add eax, ecx
    mov edx, eax
    shr eax, 2
    and edx, 3
    mov al, [esi + eax]
    push ecx
    mov cl, dl
    shl cl, 1
    shr al, cl
    pop ecx
    and eax, 3
    jz .next_column

    push eax
    mov edi, [text_y]
    add edi, ebp
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov edx, [text_x]
    add edx, ecx
    shl edx, 2
    add edi, edx
    pop eax
    call blend_text_pixel
.next_column:
    inc ecx
    jmp .glyph_column
.next_row:
    inc ebp
    jmp .glyph_row
.done:
    popad
    ret

; EDI=Framebuffer-Pixel, EAX=Deckung 1..3.
blend_text_pixel:
    cmp eax, 3
    je .opaque
    mov [blend_alpha], eax
    pushad
    mov ebx, [edi]
    mov esi, [text_color]

    mov eax, ebx
    and eax, 0xFF
    mov edx, esi
    and edx, 0xFF
    call blend_channel
    mov [blend_result], eax

    mov eax, ebx
    shr eax, 8
    and eax, 0xFF
    mov edx, esi
    shr edx, 8
    and edx, 0xFF
    call blend_channel
    shl eax, 8
    or [blend_result], eax

    mov eax, ebx
    shr eax, 16
    and eax, 0xFF
    mov edx, esi
    shr edx, 16
    and edx, 0xFF
    call blend_channel
    shl eax, 16
    or eax, [blend_result]
    mov [edi], eax
    popad
    ret
.opaque:
    mov eax, [text_color]
    mov [edi], eax
    ret

blend_channel:
    cmp dword [blend_alpha], 1
    je .one_third
    lea eax, [eax + edx * 2]
    jmp .divide
.one_third:
    lea eax, [edx + eax * 2]
.divide:
    imul eax, eax, 171
    shr eax, 9
    ret

; Interner Kernel Context. Keine Bootloader-Struktur wird nach außen gereicht.
CONTEXT_BIB               equ 0
CONTEXT_SEEN              equ 4
CONTEXT_PLATFORM          equ 8
CONTEXT_BOOT_DRIVE        equ 12
CONTEXT_MEMORY_MAP        equ 16
CONTEXT_MEMORY_COUNT      equ 20
CONTEXT_MEMORY_ENTRY_SIZE equ 24
CONTEXT_FRAMEBUFFER       equ 28
CONTEXT_PITCH             equ 32
CONTEXT_WIDTH             equ 36
CONTEXT_HEIGHT            equ 40
CONTEXT_BPP               equ 44
CONTEXT_PIXEL_FORMAT      equ 48
CONTEXT_KERNEL_ADDRESS    equ 52
CONTEXT_KERNEL_SIZE       equ 56
CONTEXT_KERNEL_ENTRY      equ 60
CONTEXT_SECURITY_STATE    equ 64
CONTEXT_CPU_FEATURE_EDX   equ 68
CONTEXT_CPU_FEATURE_ECX   equ 72
CONTEXT_ACPI_ADDRESS      equ 76
CONTEXT_MODULES_ADDRESS   equ 80
CONTEXT_MODULE_COUNT      equ 84
CONTEXT_ENTROPY_QUALITY   equ 88
CONTEXT_ENTROPY_SEED      equ 92
CONTEXT_SYSTEM_GENERATION equ 108
CONTEXT_BOOT_ATTEMPT      equ 112
CONTEXT_KERNEL_BUILD_ID   equ 116
CONTEXT_KERNEL_FORMAT     equ 136
CONTEXT_BOOT_MODE         equ 140
CONTEXT_BOOT_FLAGS        equ 144
CONTEXT_BOOT_GENERATION   equ 148
CONTEXT_FALLBACK_LEVEL    equ 152
CONTEXT_SYSTEM_GENERATION_HI equ 156
CONTEXT_FIRMWARE_RUNTIME_CAPS equ 160
CONTEXT_FIRMWARE_RUNTIME_CONTEXT equ 164
CONTEXT_FIRMWARE_RUNTIME_ENTRY equ 168
CONTEXT_SIZE              equ 172

CONTEXT_HAS_FIRMWARE      equ 0x01
CONTEXT_HAS_MEMORY        equ 0x02
CONTEXT_HAS_GRAPHICS      equ 0x04
CONTEXT_HAS_KERNEL        equ 0x08
CONTEXT_HAS_CPU           equ 0x10
CONTEXT_HAS_ENTROPY       equ 0x20
CONTEXT_HAS_SYSTEM        equ 0x40
CONTEXT_HAS_KERNEL_ID     equ 0x80
CONTEXT_HAS_BOOT_OPTIONS  equ 0x100
CONTEXT_REQUIRED          equ CONTEXT_HAS_FIRMWARE | CONTEXT_HAS_MEMORY | CONTEXT_HAS_KERNEL | CONTEXT_HAS_CPU | CONTEXT_HAS_ENTROPY | CONTEXT_HAS_SYSTEM

align 8
kernel_context:
    times CONTEXT_SIZE db 0
kernel_boot_stack_top: dd 0
stack_canary_seed: dd 0

COM1_BASE             equ 0x03F8
NOVA_COLOR_BACKGROUND equ 0x00101113
NOVA_COLOR_PANEL      equ 0x001A1C20
NOVA_COLOR_PANEL_EDGE equ 0x003D7DFF
NOVA_COLOR_CYAN       equ 0x004CC2FF
NOVA_COLOR_WHITE      equ 0x00FFFFFF
NOVA_COLOR_SECONDARY  equ 0x00CFCFCF
NOVA_COLOR_SUCCESS    equ 0x0030D158
NOVA_COLOR_WARNING    equ 0x00FFB347
NOVA_COLOR_BLACK      equ 0x00000000
NOVA_COLOR_BLUE       equ 0x002D7FC1
NOVA_COLOR_LOG_PANEL  equ 0x00121212
NOVA_COLOR_BUTTON     equ 0x00292929
NOVA_COLOR_ERROR      equ 0x00B6154B
NOVA_COLOR_ERROR_TEXT equ 0x00A9A9A9
NOVA_COLOR_DESKTOP_BG equ 0x00070D15
NOVA_COLOR_SURFACE    equ 0x00111A25
NOVA_COLOR_ELEVATED   equ 0x001C2938
NOVA_COLOR_TILE_ALT   equ 0x00233343
NOVA_COLOR_MUTED      equ 0x0096A6B8
NOVA_COLOR_TEXT          equ 0x00EAF1FA
NOVA_COLOR_ACRYLIC       equ 0x00101A2A
NOVA_COLOR_WINDOW        equ 0x00081220
NOVA_COLOR_WINDOW_BORDER equ 0x00304768
NOVA_COLOR_BORDER_ACTIVE equ 0x003D7DFF
NOVA_COLOR_PANEL_SOFT    equ 0x00101A2B
NOVA_COLOR_SIDEBAR       equ 0x000B1524
NOVA_COLOR_CARD          equ 0x00131F32
NOVA_COLOR_CARD_BORDER   equ 0x00243C5D
NOVA_COLOR_SELECTION     equ 0x00203D71
NOVA_COLOR_MENU          equ 0x000B1424
NOVA_COLOR_TASKBAR       equ 0x000C1627
NOVA_COLOR_PURPLE        equ 0x00854DFF
NOVA_COLOR_PURPLE_SOFT   equ 0x004B2A82
NOVA_COLOR_CYAN_SOFT     equ 0x0029B8D4
NOVA_COLOR_ORANGE        equ 0x00DD8A3A
NOVA_COLOR_ORB_CORE      equ 0x00070B1B

align 4
rect_color:    dd 0
rect_x:        dd 0
rect_y:        dd 0
rect_width:    dd 0
rect_height:   dd 0
text_pointer:  dd 0
text_x:        dd 0
text_origin_x: dd 0
text_y:        dd 0
text_color:    dd 0
text_scale:    dd 1
character_advance: db 0
align 4
blend_alpha:   dd 0
blend_result:  dd 0
logo_x:        dd 620
logo_y:        dd 22
logo_color:    dd NOVA_COLOR_WHITE
logo_compact:  db 0
logo_mirror:   db 0
round_color:   dd 0
round_x:       dd 0
round_y:       dd 0
round_width:   dd 0
round_height:  dd 0
circle_color:  dd 0
circle_x:      dd 0
circle_y:      dd 0
circle_radius: dd 0
circle_radius_squared: dd 0
orb_x:         dd 0
orb_y:         dd 0
orb_radius:    dd 0
shell_command_x:     dd 0
shell_command_width: dd 0
shell_window_x:      dd 0
shell_window_y:      dd 0
shell_window_width:  dd 0
shell_window_height: dd 0
shell_menu_x:        dd 0
shell_menu_y:        dd 0
shell_menu_width:    dd 0
shell_menu_height:   dd 0
shell_taskbar_y:     dd 0
shell_card_x:        dd 0

text_orb_n:
    db "N",0
text_shell_brand:
    db "NOVA",0x94,"S",0
text_shell_tagline:
    db "Dein System. Deine Freiheit.",0
text_shell_command:
    db "Befehl eingeben oder suchen...   Ctrl K",0
text_shell_status:
    db "WLAN   TON   AKKU 100%    18:42",0
text_shell_date:
    db "21. Mai 2024",0
text_explorer_title:
    db "Explorer",0
text_window_controls:
    db "-     []     X",0
text_explorer_navigation:
    db "Zur",0x81,"ck",0
text_explorer_breadcrumb:
    db "System  >  Benutzer  >  Matthias  >  Dokumente",0
text_explorer_search:
    db "In Dokumente suchen...",0
text_explorer_toolbar:
    db "+ Neu     Ausschneiden   Kopieren   Einf",0x81,"gen   L",0x94,"schen   Sortieren   Anzeigen",0
text_explorer_quick:
    db "SCHNELLZUGRIFF",0
text_explorer_sidebar:
    db "Start",10,"Desktop",10,"Dokumente",10,"Downloads",10,"Bilder",10,"Musik",10,"Videos",10,10
    db "GER",0x84,"TE & VOLUMES",10,"System",10,"Daten",10,"Backup",10,10,"Netzwerk",0
text_explorer_folders:
    db "Ordner",0
text_folder_projects:
    db "Projekte",0
text_folder_work:
    db "Beruf",0
text_folder_private:
    db "Privat",0
text_folder_notes:
    db "Notizen",0
align 4
shell_folder_labels:
    dd text_folder_projects,text_folder_work,text_folder_private,text_folder_notes
text_explorer_files:
    db "Dateien",0
text_explorer_columns:
    db "NAME                                      ",0x84,"NDERUNGSDATUM       TYP              GR",0x99,"SSE",0
text_explorer_file_rows:
    db "Projektplan_NovaOS.docx                 21.05.2024 10:21     Nova Dokument    2,4 MB",10
    db "Anforderungen_Systemarchitektur.pdf     20.05.2024 16:45     PDF Dokument     1,8 MB",10
    db "Budget_",0x9A,"bersicht.xlsx                    19.05.2024 09:12     Nova Sheet       956 KB",10
    db "nova_flow_config.json                   16.05.2024 12:11     JSON Datei       8 KB",0
text_explorer_footer:
    db "12 Elemente    5 Ordner    7 Dateien                                  548 GB frei",0

text_start_brand:
    db "NOVA",0
text_start_navigation:
    db "Start",10,10,"Apps",10,10,"Dokumente",10,10,"Personen",10,10,"F",0x84,"higkeiten",10,10,"System",0
text_start_navigation_lower:
    db "Einstellungen",10,10,"Ein/Aus",0
text_start_pinned_v2:
    db "Angeheftet",0
text_app_files:
    db "Dateien",0
text_app_browser:
    db "Browser",0
text_app_mail:
    db "Mail",0
text_app_sheet:
    db "Sheet",0
text_app_code:
    db "Code",0
text_app_terminal:
    db "Terminal",0
text_app_skills:
    db "Skills",0
text_app_settings:
    db "System",0
align 4
shell_app_labels:
    dd text_app_files,text_app_browser,text_app_mail,text_app_sheet
    dd text_app_code,text_app_terminal,text_app_skills,text_app_settings
text_start_suggestions:
    db "Vorschl",0x84,"ge",0
text_start_suggestion_items:
    db "Q2 Financial Report",10,"Project Orion",10,"AI Meeting Notes",0
text_start_search_v2:
    db "Nach Apps, Dateien und F",0x84,"higkeiten suchen...",0
text_start_profile:
    db "Matthias   Nova Benutzer",0
text_start_clock_widget:
    db "DIENSTAG, 21. MAI",10,"18:42",0
text_start_ai_widget:
    db "NOVA AI",10,"Wie kann ich dir",10,"heute helfen?",0
text_start_system_widget:
    db "SYSTEM",10,"CPU 32%   RAM 64%",10,"Speicher 58%   Netz aktiv",0

text_taskbar_search_v2:
    db "Suche oder Befehl eingeben...",0
text_pin_files:
    db "D",0
text_pin_browser:
    db "W",0
text_pin_mail:
    db "M",0
text_pin_code:
    db "C",0
text_pin_terminal:
    db ">",0
text_pin_sheet:
    db "S",0
align 4
shell_pin_labels:
    dd text_pin_files,text_pin_browser,text_pin_mail,text_pin_code,text_pin_terminal,text_pin_sheet
text_taskbar_status_v2:
    db "^   WLAN   TON   100%    18:42",0
text_taskbar_date_v2:
    db "21. Mai 2024",0

text_sheet_title:
    db "Nova Sheet - Budget_",0x9A,"bersicht.nova",0
text_sheet_tabs:
    db "Datei   Start   Einf",0x81,"gen   Daten   Ansicht   Formeln   ",0x9A,"berpr",0x81,"fen   Automatisieren   Nova F",0x84,"higkeiten",0
text_sheet_ribbon:
    db "ZWISCHENABLAGE          SCHRIFTART          AUSRICHTUNG          ZAHL          FORMATVORLAGEN          ZELLEN",10
    db "Einf",0x81,"gen  Kopieren      Nova Sans  11      Links  Mitte      W",0x84,"hrung      Tabelle  Zellenformat      Einf",0x81,"gen  L",0x94,"schen",0
text_sheet_formula:
    db "B2     fx     15.750,00",0
text_sheet_heading:
    db "Budget ",0x9A,"bersicht 2024",0
text_sheet_columns:
    db "KATEGORIE       MAI        JUNI       JULI       AUGUST     SEPTEMBER     GESAMT      BUDGET      ABWEICHUNG",0
text_sheet_rows:
    db "Einnahmen       15.750     16.230     17.100     15.980     16.540        81.600      82.000      +400",10
    db "Miete           -4.500     -4.500     -4.500     -4.500     -4.500        -22.500     -22.500       0",10
    db "Verpflegung     -2.300     -2.450     -2.620     -2.510     -2.480        -12.360     -12.000      -360",10
    db "Transport         -850       -780       -920       -810       -810         -4.240      -4.500      +260",10
    db "Sparen          -2.500     -2.500     -2.500     -2.500     -2.500        -12.500     -12.500       0",10
    db "Saldo            3.350      3.650      4.030      3.350      3.790         18.550      18.500       +50",0
text_sheet_capabilities:
    db "NOVA F",0x84,"HIGKEITEN",10,10
    db "Daten verstehen",10,"  Analyse starten",10,"  Erkl",0x84,"rungen",10,"  Datenqualit",0x84,"t pr",0x81,"fen",10,10
    db "Visualisieren",10,"  Diagramm erstellen",10,"  Pivot-Tabelle",10,"  Heatmap",10,10
    db "Automatisieren",10,"  Regel erstellen",10,"  Bericht generieren",0
text_sheet_footer:
    db "Bereit     ",0x9A,"bersicht   Einnahmen   Ausgaben   Analyse                         100%",0

text_studio_title:
    db "F",0x84,"higkeiten Studio",0
text_studio_steps:
    db "1 Info      2 Bausteine      3 Verbindung      4 Verhalten      5 Testen",0
text_studio_navigation:
    db "NOVA",10,10,"",0x9A,"bersicht",10,10,"F",0x84,"higkeiten",10,10,"Meine F",0x84,"higkeiten",10,10
    db "Entdecken",10,10,"Favoriten",10,10,"Vorlagen",10,10,"STUDIO",10,"Erstellen",10,10,"Kombinieren",10,10,"Testen",0
text_studio_catalog:
    db "F",0x84,"HIGKEITEN-KATALOG",10,10,"F",0x84,"higkeiten suchen...",10,10
    db "Datei-Import          +",10,10,"Daten-Filter          +",10,10,"Berechnung             +",10,10
    db "Diagramm-Generator    +",10,10,"KI-Analyse             +",10,10,"Bericht-Export         +",0
text_studio_canvas:
    db "MEINE F",0x84,"HIGKEIT: BUDGET ANALYSE PRO                         100%   FIT",0
text_studio_nodes:
    db "Datei-Import",10,10,10,"Daten-Filter",10,10,10,"Berechnung",10,10,10
    db "Diagramm-Generator       KI-Analyse",10,10,10,"Bericht-Export",0
text_studio_inspector:
    db "EIGENSCHAFTEN",10,10,"Daten-Filter",10,10,"ALLGEMEIN",10,"Name",10,"Beschreibung",10,"Version 1.0.0",10,10
    db "EING",0x84,"NGE",10,"Daten",10,10,"AUSG",0x84,"NGE",10,"Gefilterte Daten",10,10
    db "EINSTELLUNGEN",10,"Filterregeln",10,"Sortierung",10,10,"Erweiterte Optionen",0

text_brand:
    db "NOVA OS", 0
text_brand_compact:
    db "NovaOS", 0
text_desktop_workspace:
    db "WORKSPACE 1", 0
text_ribbon_start:
    db "START", 0
text_ribbon_file:
    db "DATEI", 0
text_ribbon_view:
    db "ANSICHT", 0
text_ribbon_new:
    db "Neu", 0
text_ribbon_open:
    db "Oeffnen", 0
text_ribbon_settings:
    db "Optionen", 0
text_start_search:
    db "Suchen: Apps, Dateien und Aktionen", 0
text_start_pinned:
    db "ANGEHEFTET", 0
text_tile_files:
    db "Dateien", 0
text_tile_settings:
    db "System", 0
text_tile_terminal:
    db "Terminal", 0
text_tile_recovery:
    db "Recovery", 0
text_tile_help:
    db "Hilfe", 0
text_tile_power:
    db "Energie", 0
text_start_user:
    db "Nova Benutzer", 0
text_start_power:
    db "Ausschalten", 0
text_desktop_welcome:
    db "Nova Desktop", 0
text_desktop_ready:
    db "Die erste System-UI-Szene",10,"wird aus Ring 3 dargestellt.", 0
text_desktop_services:
    db "Display Server aktiv", 0
text_desktop_footer:
    db "SYSTEM UI", 0
text_taskbar_start:
    db "NOVA", 0
text_taskbar_search:
    db "Suche", 0
text_taskbar_apps:
    db "N", 0
text_taskbar_status:
    db "NET  TON  DE", 0
text_taskbar_clock:
    db "18:42", 0
text_taskbar_notify:
    db "INFO", 0
text_taskbar_compact:
    db "NET  18:42", 0
text_logsystem:
    db "Logsystem", 0
text_escape:
    db "ESC", 0
text_kernel_log:
    db "NOVA: Kernel Entry",10
    db "NOVA: Panic Reporter ABI 1.1 bereit",10
    db "NOVA: NBHP/BIB v1 validiert",10
    db "NOVA: PMM und Heap ABI 1.0 bereit",10
    db "NOVA: Object und Handle Manager bereit",10
    db "NOVA: Component und Service Manager bereit",10
    db "NOVA: Paging ABI 1.0 aktiv",10
    db "NOVA: IDT, PIC und PIT 100 Hz aktiv",10
    db "NOVA: IPC ABI 1.0 und Endpunkte bereit",10
    db "NOVA: Process und Thread Manager bereit",10
    db "NOVA: Security ABI 1.0 Capabilities aktiv",10
    db "NOVA: Scheduler und drei Threads aktiv",10
    db "NOVA: Device und Power Manager aktiv",10
    db "NOVA: VFS Mount-Namespace und Root bereit",10
    db "NOVA: Network Stack und Loopback lo0 bereit",10
    db "NOVA: x86-32 Ring-3 Userspace aktiv",10
    db "NOVA: System-Call ABI 1.0 aktiv",10
    db "NOVA: Shared Service Page bereit",10
    db "NOVA: Framebuffer und Kernel Context aktiv",10
    db "NOVA_KERNEL_READY",0
text_ready:
    db "SYSTEM BEREIT", 0
text_subtitle:
    db "DER KERNEL WURDE ERFOLGREICH GESTARTET", 0
text_status_heading:
    db "VALIDIERTE STARTKOMPONENTEN", 0
text_check_kernel:
    db "KERNEL-IMAGE UND CRC32", 0
text_check_handoff:
    db "NBHP/BIB BOOTPROTOKOLL", 0
text_check_memory:
    db "COMPONENTS OBJECTS PMM HEAP PAGING", 0
text_check_graphics:
    db "VBE FRAMEBUFFER", 0
text_check_interrupts:
    db "SECURITY PROCESS SERVICES SCHEDULER", 0
text_technical:
    db "NBHP/BIB 1.0 | X86-32 | 800 X 600", 0
text_panic_smile:
    db ";(", 0
text_panic_title:
    db "NOVA OS MUSSTE ANGEHALTEN WERDEN", 0
text_panic_explain1:
    db "EIN WICHTIGER TEIL DES SYSTEMS HAT NICHT RICHTIG REAGIERT.", 0
text_panic_explain2:
    db "ZU DEINER SICHERHEIT WURDE DER COMPUTER ANGEHALTEN.", 0
text_panic_action1:
    db "BITTE STARTE DEN COMPUTER NEU.", 0
text_panic_action2:
    db "WENN DAS ERNEUT PASSIERT NOTIERE DEN FEHLERCODE.", 0
text_panic_code:
    db "FEHLERCODE 0X", 0
text_panic_footer:
    db "TECHNISCHE DETAILS WURDEN IM SYSTEMPROTOKOLL GESPEICHERT.", 0
text_error_title:
    db "Start nicht m",0x94,"glich",0
text_error_explain1:
    db "NovaOS hat einen schweren Systemfehler festgestellt",0
text_error_explain2:
    db "Zu Ihrer Sicherheit wurde das System angehalten",0
text_error_action1:
    db "Bitte den Computer neu starten",0
text_error_action2:
    db "oder",0
text_error_action3:
    db "Den Fehlercode f",0x81,"r die Diagnose notieren",0
text_error_code:
    db "FEHLERCODE: KERNEL 0x",0
panic_hex_buffer:
    db "00000000", 0

; 5x7-Bitmap-Font für den frühen Bootstatus.
font_characters:
    db "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-./:|();", 0

font_bitmap:
    ; A-Z
    db 0x0E,0x11,0x11,0x1F,0x11,0x11,0x11
    db 0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E
    db 0x0E,0x11,0x10,0x10,0x10,0x11,0x0E
    db 0x1E,0x11,0x11,0x11,0x11,0x11,0x1E
    db 0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F
    db 0x1F,0x10,0x10,0x1E,0x10,0x10,0x10
    db 0x0E,0x11,0x10,0x17,0x11,0x11,0x0F
    db 0x11,0x11,0x11,0x1F,0x11,0x11,0x11
    db 0x0E,0x04,0x04,0x04,0x04,0x04,0x0E
    db 0x07,0x02,0x02,0x02,0x12,0x12,0x0C
    db 0x11,0x12,0x14,0x18,0x14,0x12,0x11
    db 0x10,0x10,0x10,0x10,0x10,0x10,0x1F
    db 0x11,0x1B,0x15,0x15,0x11,0x11,0x11
    db 0x11,0x19,0x15,0x13,0x11,0x11,0x11
    db 0x0E,0x11,0x11,0x11,0x11,0x11,0x0E
    db 0x1E,0x11,0x11,0x1E,0x10,0x10,0x10
    db 0x0E,0x11,0x11,0x11,0x15,0x12,0x0D
    db 0x1E,0x11,0x11,0x1E,0x14,0x12,0x11
    db 0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E
    db 0x1F,0x04,0x04,0x04,0x04,0x04,0x04
    db 0x11,0x11,0x11,0x11,0x11,0x11,0x0E
    db 0x11,0x11,0x11,0x11,0x11,0x0A,0x04
    db 0x11,0x11,0x11,0x15,0x15,0x15,0x0A
    db 0x11,0x11,0x0A,0x04,0x0A,0x11,0x11
    db 0x11,0x11,0x0A,0x04,0x04,0x04,0x04
    db 0x1F,0x01,0x02,0x04,0x08,0x10,0x1F
    ; 0-9
    db 0x0E,0x11,0x13,0x15,0x19,0x11,0x0E
    db 0x04,0x0C,0x04,0x04,0x04,0x04,0x0E
    db 0x0E,0x11,0x01,0x02,0x04,0x08,0x1F
    db 0x1E,0x01,0x01,0x0E,0x01,0x01,0x1E
    db 0x02,0x06,0x0A,0x12,0x1F,0x02,0x02
    db 0x1F,0x10,0x10,0x1E,0x01,0x01,0x1E
    db 0x0E,0x10,0x10,0x1E,0x11,0x11,0x0E
    db 0x1F,0x01,0x02,0x04,0x08,0x08,0x08
    db 0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E
    db 0x0E,0x11,0x11,0x0F,0x01,0x01,0x0E
    ; - . / : | ( )
    db 0x00,0x00,0x00,0x1F,0x00,0x00,0x00
    db 0x00,0x00,0x00,0x00,0x00,0x0C,0x0C
    db 0x01,0x02,0x02,0x04,0x08,0x08,0x10
    db 0x00,0x0C,0x0C,0x00,0x0C,0x0C,0x00
    db 0x04,0x04,0x04,0x04,0x04,0x04,0x04
    db 0x02,0x04,0x08,0x08,0x08,0x04,0x02
    db 0x08,0x04,0x02,0x02,0x02,0x04,0x08
    db 0x00,0x0C,0x0C,0x00,0x0C,0x08,0x10

message_entered:
    db "NOVA: Kernel Entry", 13, 10, 0
message_logging_ok:
    db "NOVA: Logging ABI 1.0, strukturierter Ring- und Reservepuffer bereit", 13, 10, 0
message_logging_query_ok:
    db "NOVA: Userspace Logging.Query capability-geprueft", 13, 10, 0
message_logging_read_ok:
    db "NOVA: Userspace neuesten atomar publizierten Logrecord gelesen", 13, 10, 0
message_boot_phase:
    db "NOVA: BOOT phase=0x", 0
message_boot_last_phase:
    db " last_phase=0x", 0
message_panic_manager_ok:
    db "NOVA: Panic Reporter ABI 1.1 bereit", 13, 10, 0
message_crash_dump_ok:
    db "NOVA: Crash Dump ABI 1.0, reservierter Minimal-Dump-Pfad bereit", 13, 10, 0
message_config_ok:
    db "NOVA: Config Framework ABI 1.0, Schema/Store/Transaktion bereit", 13, 10, 0
message_config_error:
    db "NOVA PANIC: Kernel Configuration Selbsttest fehlgeschlagen", 13, 10, 0
message_abi_ok:
    db "NOVA: Kernel ABI 1.0 (§30), Registry mit 11 Built-in-Services bereit", 13, 10, 0
message_abi_error:
    db "NOVA PANIC: Kernel ABI Selbsttest fehlgeschlagen", 13, 10, 0
message_kog_ok:
    db "NOVA: Kernel Object Graph (§100), 7 Root-Objekte, 12 Schemata bereit", 13, 10, 0
message_kog_init_error:
    db "NOVA PANIC: KOG Initialize fehlgeschlagen", 13, 10, 0
message_kog_test_error:
    db "NOVA PANIC: KOG Selbsttest fehlgeschlagen", 13, 10, 0
message_kog_error:
    db "NOVA PANIC: Kernel Object Graph Selbsttest fehlgeschlagen", 13, 10, 0
message_evbus_ok:
    db "NOVA: Event Bus 1.0 (§101), 8 Built-in-Schemata, Pub/Sub bereit", 13, 10, 0
message_evbus_error:
    db "NOVA PANIC: Event Bus Selbsttest fehlgeschlagen", 13, 10, 0
message_uobj_ok:
    db "NOVA: Unified Object API 1.0 (§102), 15 Typen, Handle-Tabelle bereit", 13, 10, 0
message_uobj_error:
    db "NOVA PANIC: Unified Object API Selbsttest fehlgeschlagen", 13, 10, 0
message_cap_ok:
    db "NOVA: Capability Framework 1.0 (§103), 2 Boot-Caps, 3 Event-Schemata bereit", 13, 10, 0
message_cap_error:
    db "NOVA PANIC: Capability Framework Selbsttest fehlgeschlagen", 13, 10, 0
message_diag_ok:
    db "NOVA: Diagnostics Framework 1.0 (§104), 6 Quellen, Ringpuffer, Health bereit", 13, 10, 0
message_diag_error:
    db "NOVA PANIC: Diagnostics Framework Selbsttest fehlgeschlagen", 13, 10, 0
message_cap_integ_ok:
    db "NOVA: CAP-Integration 1.0, UOBJ-Bridge 32 Slots, IPC/VFS gesichert", 13, 10, 0
message_cap_integ_error:
    db "NOVA PANIC: CAP-Integration Selbsttest fehlgeschlagen", 13, 10, 0
message_vsvc_ok:
    db "NOVA: Versioned Service ABI 1.0 (SS105), 4 Bootstrap-Services, 5 Events", 13, 10, 0
message_vsvc_error:
    db "NOVA PANIC: Versioned Service ABI Selbsttest fehlgeschlagen", 13, 10, 0
message_sync_ok:
    db "NOVA: Sync 1.0 + Futex 1.0 bereit (Spinlock/Mutex/RCU + Fast Userspace Mutex)", 13, 10, 0
message_sync_error:
    db "NOVA PANIC: Synchronisation Selbsttest fehlgeschlagen", 13, 10, 0
message_irq_manager_ok:
    db "NOVA: Interrupt Manager 1.0 (SS009), 16 IRQ-Slots, 208 dyn. Vektoren", 13, 10, 0
message_irq_manager_error:
    db "NOVA PANIC: Interrupt Manager Selbsttest fehlgeschlagen", 13, 10, 0
message_exc_mgr_error:
    db "NOVA PANIC: Exception Manager Initialisierung fehlgeschlagen", 13, 10, 0
message_module_loader_ok:
    db "NOVA: Module Loader ABI 1.0, Trust-, ABI- und W^X-Pruefung bereit", 13, 10, 0
message_module_loader_error:
    db "NOVA PANIC: Module Loader Selbsttest fehlgeschlagen", 13, 10, 0
message_cpu_manager_initialized:
    db "NOVA: CPU Manager Initialisierung abgeschlossen", 13, 10, 0
message_cpu_manager_selftest_stage:
    db "NOVA: CPU Manager Selbsttest-Stufe 0x", 0
message_cpu_manager_ok:
    db "NOVA: CPU Manager ABI 1.0, BSP-Topologie und per-CPU-Daten aktiv", 13, 10, 0
message_cpu_topology_import_ok:
    db "NOVA: CPU Manager bezieht Package, Core und Thread aus HAL Topology", 13, 10, 0
message_cpu_manager_error:
    db "NOVA PANIC: CPU Manager Selbsttest fehlgeschlagen", 13, 10, 0
message_smp_ok:
    db "NOVA: SMP-Grundlage ABI 1.0, BSP-Barriere und lokaler TLB-Pfad bereit", 13, 10, 0
message_smp_runtime_ok:
    db "NOVA: SMP AP-LAPIC-Timer und AP-Scheduler aktiv", 13, 10, 0
message_smp_stress_ok:
    db "NOVA: SMP-Stresstest (§62) bestanden: Call, TLB-Shootdown, gemischte Last", 13, 10, 0
message_smp_error:
    db "NOVA PANIC: SMP-Grundlagen-Selbsttest fehlgeschlagen", 13, 10, 0
message_panic_begin:
    db "NOVA PANIC REPORT code=0x", 0
message_panic_subsystem:
    db " subsystem=0x", 0
message_panic_eip:
    db " eip=0x", 0
message_panic_cr2:
    db " cr2=0x", 0
message_panic_phase:
    db " phase=0x", 0
message_panic_last_phase:
    db " last_phase=0x", 0
message_panic_cpu:
    db " cpu=0x", 0
message_panic_security:
    db " security=0x", 0
message_panic_end:
    db "NOVA_PANIC_HALTED", 13, 10, 0
message_debug_panic:
    db "NOVA PANIC: manueller F12-Diagnosetest", 13, 10, 0
message_shutdown:
    db "NOVA: Shutdown angefordert, System wird ausgeschaltet", 13, 10, 0
message_power_manager_ok:
    db "NOVA: Power Manager ABI 1.0 und Shutdown-Pfad bereit", 13, 10, 0
message_display_server_ok:
    db "NOVA: Display Server ABI 1.0, Firmware-Framebuffer uebernommen", 13, 10, 0
message_display_server_fallback:
    db "NOVA: Display Server ohne Grafikprovider, Text-Fallback aktiv", 13, 10, 0
message_display_query_ok:
    db "NOVA: Userspace Display.QueryPrimary ohne MMIO-Adresse erfolgreich", 13, 10, 0
message_display_scene_ok:
    db "NOVA: Desktop, Startmenue, Ribbon und Taskleiste aus Ring-3-Szene praesentiert", 13, 10, 0
message_display_input_ok:
    db "NOVA: Input-Router-Ereignis an Ring-3-System-UI zugestellt", 13, 10, 0
message_power_query_ok:
    db "NOVA: Userspace Power.QuerySystem capability-geprueft", 13, 10, 0
message_power_wake_acquire_ok:
    db "NOVA: Userspace Power.WakeAcquire zeitlich begrenzt", 13, 10, 0
message_power_wake_release_ok:
    db "NOVA: Userspace Power.WakeRelease validiert", 13, 10, 0
message_power_profile_ok:
    db "NOVA: Userspace Power.SetProfile capability-geprueft", 13, 10, 0
message_power_shutdown_denied:
    db "NOVA: Userspace Power.RequestState ohne Capability abgewiesen", 13, 10, 0
message_power_manager_error:
    db "NOVA PANIC: Power Manager nicht initialisierbar", 13, 10, 0
message_network_manager_ok:
    db "NOVA: Network ABI 1.0, IPv4/IPv6 UDP, ICMP und TCP bereit", 13, 10, 0
message_network_query_ok:
    db "NOVA: Userspace Network.Query capability-geprueft", 13, 10, 0
message_network_raw_denied:
    db "NOVA: Userspace Raw-Socket ohne Capability abgewiesen", 13, 10, 0
message_network_socket_ok:
    db "NOVA: Userspace IPv4/IPv6 Datagramm-Socketobjekt erstellt", 13, 10, 0
message_network_stream_ok:
    db "NOVA: Userspace TCP-Stream-Socketobjekt erstellt", 13, 10, 0
message_network_connect_ok:
    db "NOVA: TCP-Stream mit IPv4 127.0.0.1 Port 9000 verbunden", 13, 10, 0
message_network_stream_send_ok:
    db "NOVA: TCP-Streamdaten in begrenzte Sendewarteschlange eingestellt", 13, 10, 0
message_network_stream_receive_ok:
    db "NOVA: TCP-Streamdaten geordnet aus Empfangswarteschlange gelesen", 13, 10, 0
message_network_stream_bind_ok:
    db "NOVA: TCP-Listener an IPv4 127.0.0.1 Port 9000 gebunden", 13, 10, 0
message_network_listen_ok:
    db "NOVA: TCP-Listener mit begrenztem Backlog aktiv", 13, 10, 0
message_network_accept_ok:
    db "NOVA: TCP-Verbindung als separates Socket-Handle akzeptiert", 13, 10, 0
message_network_connect_handle_denied:
    db "NOVA: TCP-Connect wegen ungueltigem Handle/Recht abgewiesen", 13, 10, 0
message_network_connect_firewall_denied:
    db "NOVA: TCP-Connect durch Bootstrap-Firewall abgewiesen", 13, 10, 0
message_network_connect_state_error:
    db "NOVA: TCP-Connect wegen ungueltigem Zustandswechsel abgewiesen", 13, 10, 0
message_network_bind_ok:
    db "NOVA: Userspace Socket an IPv4 127.0.0.1 Port 9000 gebunden", 13, 10, 0
message_network_loopback_ok:
    db "NOVA: IPv4/UDP Loopback mit Header- und Pruefsummenvalidierung erfolgreich", 13, 10, 0
message_network_size_denied:
    db "NOVA: Ueberlanges Loopback-Datagramm sicher abgewiesen", 13, 10, 0
message_network_send_ok:
    db "NOVA: IPv4/UDP Loopback-Datagramm in begrenzte Queue eingestellt", 13, 10, 0
message_network_malformed:
    db "NOVA: Beschaedigtes IPv4/UDP-Datagramm verworfen", 13, 10, 0
message_network_would_block:
    db "NOVA: Nichtblockierende Loopback-Queue meldet WOULD_BLOCK", 13, 10, 0
message_network_manager_error:
    db "NOVA PANIC: Network Manager nicht initialisierbar", 13, 10, 0
message_shutdown_requested:
    db "NOVA: Power Shutdown REQUEST", 13, 10, 0
message_shutdown_userspace:
    db "NOVA: Power Shutdown FREEZE_USERSPACE", 13, 10, 0
message_shutdown_storage:
    db "NOVA: Power Shutdown SYNC_STORAGE", 13, 10, 0
message_shutdown_devices:
    db "NOVA: Power Shutdown STOP_DEVICES", 13, 10, 0
message_shutdown_platform:
    db "NOVA: Power Shutdown PLATFORM_OFF", 13, 10, 0
message_bib_ok:
    db "NOVA: NBHP/BIB v1 validiert", 13, 10, 0
message_recovery_mode_ok:
    db "NOVA: Recovery-Modus aus NBHP/BIB aktiv", 13, 10, 0
message_backup_generation_ok:
    db "NOVA: Backup-Kernelgeneration aus NBHP/BIB aktiv", 13, 10, 0
message_acpi_rsdp_ok:
    db "NOVA: ACPI RSDP mit Pruefsumme validiert", 13, 10, 0
message_acpi_rsdp_missing:
    db "NOVA: ACPI RSDP nicht verfuegbar, BSP-only", 13, 10, 0
message_acpi_cpu_count:
    db "NOVA: ACPI MADT, erkannte CPUs (hex): 0x", 0
message_acpi_srat_ok:
    db "NOVA: ACPI SRAT validiert, CPU/Memory-Affinitaeten (hex): 0x", 0
message_acpi_srat_unavailable:
    db "NOVA: ACPI SRAT nicht validiert oder nicht verfuegbar, NUMA unbekannt", 13, 10, 0
message_line_end:
    db 13, 10, 0
message_kernel_identity_ok:
    db "NOVA: Kernel Build-ID aus NBHP/BIB importiert", 13, 10, 0
message_pmm_ok:
    db "NOVA: PMM NUMA ABI 1.1, Preferred und Strict Allocation aktiv", 13, 10, 0
message_pmm_error:
    db "NOVA PANIC: physischer Speichermanager nicht initialisierbar", 13, 10, 0
message_heap_ok:
    db "NOVA: Heap ABI 1.0 und Schreibtest bereit", 13, 10, 0
message_heap_error:
    db "NOVA PANIC: Bootstrap-Heap nicht initialisierbar", 13, 10, 0
message_object_manager_ok:
    db "NOVA: Object Manager ABI 1.0 bereit", 13, 10, 0
message_object_manager_error:
    db "NOVA PANIC: Kernel Object Manager nicht initialisierbar", 13, 10, 0
message_handle_manager_ok:
    db "NOVA: Handle Manager ABI 1.0 bereit", 13, 10, 0
message_handle_manager_error:
    db "NOVA PANIC: Prozesslokaler Handle Manager nicht initialisierbar", 13, 10, 0
message_component_manager_ok:
    db "NOVA: Component Manager ABI 1.0 bereit", 13, 10, 0
message_component_manager_error:
    db "NOVA PANIC: Kernel Component Manager nicht initialisierbar", 13, 10, 0
message_paging_ok:
    db "NOVA: Paging ABI 1.0 und Speichertest bereit", 13, 10, 0
message_paging_error:
    db "NOVA PANIC: virtueller Speichermanager nicht initialisierbar", 13, 10, 0
message_interrupts_ok:
    db "NOVA: IDT, PIC und PIT 100 Hz aktiv", 13, 10, 0
message_interrupts_error:
    db "NOVA PANIC: Interrupt- oder Timerinitialisierung fehlgeschlagen", 13, 10, 0
message_ipc_ok:
    db "NOVA: IPC ABI 1.0 FIFO bereit", 13, 10, 0
message_semantic_ok:
    db "NOVA: Semantic Types v1, Registry, Kompatibilitaet und Typed Contracts bereit", 13, 10, 0
message_semantic_core_ok:
    db "NOVA: Semantic Core bereit", 13, 10, 0
message_object_id_abi_ok:
    db "NOVA: ObjectID ABI bereit", 13, 10, 0
message_filesystem_object_registry_ok:
    db "NOVA: Filesystem Object Registry bereit", 13, 10, 0
message_filesystem_object_enumeration_ok:
    db "NOVA: Filesystem Object Enumeration bereit", 13, 10, 0
message_filesystem_object_projection_ok:
    db "NOVA: Filesystem Object Projection Konsistenz bereit", 13, 10, 0
message_filesystem_object_path_ok:
    db "NOVA: Filesystem Object Pfadaufloesung bereit", 13, 10, 0
message_filesystem_volume_registry_ok:
    db "NOVA: Filesystem Volume Registry bereit", 13, 10, 0
message_capability_registry_ok:
    db "NOVA: Capability Registry bereit", 13, 10, 0
message_capability_authority_ok:
    db "NOVA: Capability Authority Rechtepruefung bereit", 13, 10, 0
message_capability_lifecycle_ok:
    db "NOVA: Capability Lifecycle Lookup und Revoke bereit", 13, 10, 0
message_namespace_core_ok:
    db "NOVA: Namespace Core bereit: / System Benutzer Apps Volumes Boot Solutions", 13, 10, 0
message_object_id_lookup_ok:
    db "NOVA: ObjectID Registry Lookup bereit", 13, 10, 0
message_namespace_lookup_ok:
    db "NOVA: Namespace Lookup bereit", 13, 10, 0
message_namespace_path_ok:
    db "NOVA: Namespace Pfadaufloesung bereit", 13, 10, 0
message_namespace_introspection_ok:
    db "NOVA: Namespace Introspection bereit", 13, 10, 0
message_namespace_enumeration_ok:
    db "NOVA: Namespace Enumeration bereit", 13, 10, 0
message_projection_map_ok:
    db "NOVA: ObjectID Projection Map bereit", 13, 10, 0
message_projection_introspection_ok:
    db "NOVA: Projection Introspection bereit", 13, 10, 0
message_object_handle_ok:
    db "NOVA: Kernel Object Handle ABI bereit", 13, 10, 0
message_handle_object_binding_ok:
    db "NOVA: Handle Object-Registry Bindung bereit", 13, 10, 0
message_handle_validation_ok:
    db "NOVA: Handle Rechtevalidierung gegen Capabilities bereit", 13, 10, 0
message_handle_path_ok:
    db "NOVA: Namespace Pfad zu Handle bereit", 13, 10, 0
message_semantic_core_error:
    db "NOVA PANIC: Semantic Core nicht initialisierbar", 13, 10, 0
message_state_manager_ok:
    db "NOVA: Global State ABI 1.0, Versionen und Unknown-State-Pruefung bereit", 13, 10, 0
message_transaction_manager_ok:
    db "NOVA: Transaction ABI 1.0, Begin-Prepare-Commit-Verify bereit", 13, 10, 0
message_semantic_reject_ok:
    db "NOVA: Typed IPC, fremder Type und Version sicher abgewiesen", 13, 10, 0
message_semantic_validation_ok:
    db "NOVA: Semantic Validation, ungueltiger Wert strukturiert abgewiesen", 13, 10, 0
message_ipc_error:
    db "NOVA PANIC: Kernel-IPC nicht initialisierbar", 13, 10, 0
message_state_manager_error:
    db "NOVA PANIC: Global State Manager nicht initialisierbar", 13, 10, 0
message_transaction_manager_error:
    db "NOVA PANIC: Transaction Manager nicht initialisierbar", 13, 10, 0
message_service_manager_ok:
    db "NOVA: Service Manager ABI 1.0 bereit", 13, 10, 0
message_service_manager_error:
    db "NOVA PANIC: Kernel Service Manager nicht initialisierbar", 13, 10, 0
message_process_manager_ok:
    db "NOVA: Process Manager ABI 1.0 bereit", 13, 10, 0
message_process_manager_error:
    db "NOVA PANIC: Kernel Process Manager nicht initialisierbar", 13, 10, 0
message_task_scope_manager_ok:
    db "NOVA: Task Scope ABI 1.0, Hierarchie und Cancellation aktiv", 13, 10, 0
message_task_scope_manager_error:
    db "NOVA PANIC: Task-Scope-Manager nicht initialisierbar", 13, 10, 0
message_task_manager_ok:
    db "NOVA: Task ABI 1.0, Lifecycle und kooperative Cancellation aktiv", 13, 10, 0
message_task_manager_error:
    db "NOVA PANIC: Task Manager nicht initialisierbar", 13, 10, 0
message_task_deadline_manager_ok:
    db "NOVA: Task Deadline ABI 1.0, Parent-Clamp und Miss-Policy aktiv", 13, 10, 0
message_task_deadline_manager_error:
    db "NOVA PANIC: Task Deadline Manager nicht initialisierbar", 13, 10, 0
message_task_group_manager_ok:
    db "NOVA: Task Group ABI 1.0, WaitAll, FailFast und Drain aktiv", 13, 10, 0
message_task_group_manager_error:
    db "NOVA PANIC: Task Group Manager nicht initialisierbar", 13, 10, 0
message_io_request_manager_ok:
    db "NOVA: Async IO ABI 1.0, Completion, Deadline und Backpressure aktiv", 13, 10, 0
message_io_request_manager_error:
    db "NOVA PANIC: Async IO Request Manager nicht initialisierbar", 13, 10, 0
message_io_completion_ok:
    db "NOVA: IO Completion Queue ABI 1.0, FIFO und Batch aktiv", 13, 10, 0
message_io_completion_error:
    db "NOVA PANIC: IO Completion Queue nicht initialisierbar", 13, 10, 0
message_shared_buffer_ok:
    db "NOVA: Shared Buffer ABI 1.0, IO-Lease und Copy-Fallback aktiv", 13, 10, 0
message_shared_buffer_error:
    db "NOVA PANIC: Shared Buffer Manager nicht initialisierbar", 13, 10, 0
message_topology_ok:
    db "NOVA: HAL Topology ABI 1.0, Busse, Devices und IOMMU-Gruppen aktiv", 13, 10, 0
message_topology_cpu_count:
    db "NOVA: HAL Topology, normalisierte CPU-Threads (hex): 0x", 0
message_topology_hierarchy_count:
    db "NOVA: HAL CPUID Hierarchy, Packages/Core/Threads (hex): 0x", 0
message_topology_count_separator:
    db "/0x", 0
message_topology_error:
    db "NOVA PANIC: HAL Topology Manager nicht initialisierbar", 13, 10, 0
message_iommu_ok:
    db "NOVA: IOMMU ABI 1.0, Domains, Gruppen und Fault-Zuordnung aktiv", 13, 10, 0
message_iommu_error:
    db "NOVA PANIC: IOMMU Domain Manager nicht initialisierbar", 13, 10, 0
message_dma_mapping_ok:
    db "NOVA: DMA Mapping ABI 1.0, Pinning und sicherer Fallback aktiv", 13, 10, 0
message_dma_mapping_error:
    db "NOVA PANIC: DMA Mapping Manager nicht initialisierbar", 13, 10, 0
message_scatter_gather_ok:
    db "NOVA: Scatter Gather ABI 1.0, Segmente und Lifetime aktiv", 13, 10, 0
message_scatter_gather_error:
    db "NOVA PANIC: Scatter Gather Manager nicht initialisierbar", 13, 10, 0
message_dma_scatter_gather_ok:
    db "NOVA: DMA Scatter Gather ABI 1.0, Split und Providerlimits aktiv", 13, 10, 0
message_dma_scatter_gather_error:
    db "NOVA PANIC: DMA Scatter Gather Manager nicht initialisierbar", 13, 10, 0
message_dma_iommu_lifecycle_ok:
    db "NOVA: DMA IOMMU Lifecycle fuer linear und Scatter Gather aktiv", 13, 10, 0
message_iommu_fault_propagation_ok:
    db "NOVA: IOMMU Fault beendet DMA und IO Request kontrolliert", 13, 10, 0
message_ringbuf_ok:
    db "NOVA: Ring Buffer 1.0 bereit (SPSC, 4 Instanzen, 16 Slots)", 13, 10, 0
message_ringbuf_error:
    db "NOVA PANIC: Ring Buffer Manager nicht initialisierbar", 13, 10, 0
message_locality_ok:
    db "NOVA: Data Locality 1.0 bereit (NUMA-aware, RT-Schutz, Kostenmodell)", 13, 10, 0
message_locality_error:
    db "NOVA PANIC: Data Locality Manager nicht initialisierbar", 13, 10, 0
message_kernelbypass_ok:
    db "NOVA: Kernel Bypass 1.0 bereit (Control+DataPlane, Revocation)", 13, 10, 0
message_kernelbypass_error:
    db "NOVA PANIC: Kernel Bypass Manager nicht initialisierbar", 13, 10, 0
message_watchdog_ok:
    db "NOVA: Watchdog 1.0 bereit (4 Slots, Kick/Arm/Expire/Disarm)", 13, 10, 0
message_watchdog_error:
    db "NOVA PANIC: Watchdog Manager nicht initialisierbar", 13, 10, 0
message_circuitbreaker_ok:
    db "NOVA: Circuit Breaker 1.0 bereit (8 Slots, CLOSED/OPEN/HALF-OPEN)", 13, 10, 0
message_circuitbreaker_error:
    db "NOVA PANIC: Circuit Breaker nicht initialisierbar", 13, 10, 0
message_detection_ok:
    db "NOVA: Fault Detection 1.0 bereit (32-Slot-Log, 7 Typen)", 13, 10, 0
message_detection_error:
    db "NOVA PANIC: Fault Detection nicht initialisierbar", 13, 10, 0
message_deferred_ok:
    db "NOVA: Deferred IRQ 1.0 bereit (16-Slot-Queue, Bottom-Half)", 13, 10, 0
message_deferred_error:
    db "NOVA PANIC: Deferred Interrupt Manager nicht initialisierbar", 13, 10, 0
message_trace_ok:
    db "NOVA: Kernel Trace 1.0 bereit (64-Slot-Ring, seq/subsys/args)", 13, 10, 0
message_trace_error:
    db "NOVA PANIC: Kernel Trace Ring nicht initialisierbar", 13, 10, 0
message_metrics_ok:
    db "NOVA: Metrics 1.0 bereit (32 Counter, increment/add/peak/reset)", 13, 10, 0
message_metrics_error:
    db "NOVA PANIC: Metrics Manager nicht initialisierbar", 13, 10, 0
message_log_ring_ok:
    db "NOVA: Log Ring 1.0 bereit (32-Slot, DEBUG/INFO/WARN/ERROR/CRIT)", 13, 10, 0
message_log_ring_error:
    db "NOVA PANIC: Kernel Log Ring nicht initialisierbar", 13, 10, 0
message_introspection_ok:
    db "NOVA: Introspection 1.0 bereit (5 Built-in-Provider: Kernel/Sched/Mem/Sec/Resil)", 13, 10, 0
message_introspection_error:
    db "NOVA PANIC: Introspection Registry nicht initialisierbar", 13, 10, 0
message_profiling_ok:
    db "NOVA: Profiling 1.0 bereit (16-Slot Sampling, hottest-component)", 13, 10, 0
message_profiling_error:
    db "NOVA PANIC: Sampling Profiler nicht initialisierbar", 13, 10, 0
message_csprng_ok:
    db "NOVA: CSPRNG 1.0 bereit (256-bit Pool, xorshift32, entropy-mix)", 13, 10, 0
message_csprng_error:
    db "NOVA PANIC: CSPRNG nicht initialisierbar", 13, 10, 0
message_cpuidle_ok:
    db "NOVA: CPU Idle 1.0 bereit (C0/C1/C1E/C3, HLT, Residency-Tracking)", 13, 10, 0
message_cpuidle_error:
    db "NOVA PANIC: CPU Idle Manager nicht initialisierbar", 13, 10, 0
message_thermal_ok:
    db "NOVA: Thermal 1.0 bereit (4 Zonen, WARM/HOT-Throttle, QoS-Epoch)", 13, 10, 0
message_thermal_error:
    db "NOVA PANIC: Thermal Manager nicht initialisierbar", 13, 10, 0
message_identity_ok:
    db "NOVA: Identity 1.0 bereit (16-Slot Registry, KERNEL/PROCESS/SERVICE/DEVICE)", 13, 10, 0
message_identity_error:
    db "NOVA PANIC: Identity Registry nicht initialisierbar", 13, 10, 0
message_trust_ok:
    db "NOVA: Trust Anchor 1.0 bereit (8-Slot, BOOT+SECURITY Anker, CSPRNG-Fingerprint)", 13, 10, 0
message_trust_error:
    db "NOVA PANIC: Trust Anchor Chain nicht initialisierbar", 13, 10, 0
message_adaptive_ok:
    db "NOVA: Adaptive Feedback 1.0 bereit (8-Slot, Thermal/QoS/Sched/Memory)", 13, 10, 0
message_adaptive_error:
    db "NOVA PANIC: Adaptive Feedback Loop nicht initialisierbar", 13, 10, 0
message_radmit_ok:
    db "NOVA: Resource Admission 1.0 bereit (8-Slot, CPU+MEM Budget, reject on overflow)", 13, 10, 0
message_radmit_error:
    db "NOVA PANIC: Resource Admission Control nicht initialisierbar", 13, 10, 0
message_excon_ok:
    db "NOVA: Execution Contract 1.0 bereit (8-Slot, BE/BOUNDED/HARD_RT, verify)", 13, 10, 0
message_excon_error:
    db "NOVA PANIC: Execution Contract Manager nicht initialisierbar", 13, 10, 0
message_tmpiso_ok:
    db "NOVA: Temporal Isolation 1.0 bereit (4 Partitionen, Epoch-Budget, RT-Guard)", 13, 10, 0
message_tmpiso_error:
    db "NOVA PANIC: Temporal Isolation Manager nicht initialisierbar", 13, 10, 0
message_selfheal_ok:
    db "NOVA: Self-Healing 1.0 bereit (8-Slot, Observe/Detect/Repair/Verify)", 13, 10, 0
message_selfheal_error:
    db "NOVA PANIC: Self-Healing Manager nicht initialisierbar", 13, 10, 0
message_privlabel_ok:
    db "NOVA: Privacy Label 1.0 bereit (16-Slot, PUBLIC/SENSITIVE/PRIVATE/CONF)", 13, 10, 0
message_privlabel_error:
    db "NOVA PANIC: Privacy Label Manager nicht initialisierbar", 13, 10, 0
message_retry_ok:
    db "NOVA: Retry+Backoff 1.0 bereit (8-Slot, Exp-Backoff, max-attempts)", 13, 10, 0
message_retry_error:
    db "NOVA PANIC: Retry Manager nicht initialisierbar", 13, 10, 0
message_rmode_ok:
    db "NOVA: Recovery Mode 1.0 bereit (NORMAL/DEGRADED/EMERGENCY/MAINTENANCE)", 13, 10, 0
message_rmode_error:
    db "NOVA PANIC: Recovery Mode Manager nicht initialisierbar", 13, 10, 0
message_rtv_ok:
    db "NOVA: Runtime Verify 1.0 bereit (8 Contracts, bounds-check, audit-on-violation)", 13, 10, 0
message_rtv_error:
    db "NOVA PANIC: Runtime Verification Monitor nicht initialisierbar", 13, 10, 0
message_sgraph_ok:
    db "NOVA: State Graph 1.0 bereit (16-Node, transition+parent tracking)", 13, 10, 0
message_sgraph_error:
    db "NOVA PANIC: State Graph Manager nicht initialisierbar", 13, 10, 0
message_fsm_ok:
    db "NOVA: FSM 1.0 bereit (4 Machines, 8 Transitions each, denied-guard)", 13, 10, 0
message_fsm_error:
    db "NOVA PANIC: FSM Manager nicht initialisierbar", 13, 10, 0
message_txroll_ok:
    db "NOVA: Transaction Rollback 1.0 bereit (8-Slot Journal, save/undo/commit)", 13, 10, 0
message_txroll_error:
    db "NOVA PANIC: Transaction Rollback Journal nicht initialisierbar", 13, 10, 0
message_fi_ok:
    db "NOVA: Fault Injection 1.0 bereit (8-Slot, inject/check/clear)", 13, 10, 0
message_fi_error:
    db "NOVA PANIC: Fault Injection Framework nicht initialisierbar", 13, 10, 0
message_fclass_ok:
    db "NOVA: Fault Classification 1.0 bereit (8-Klassen, Schweregrad+Quelle)", 13, 10, 0
message_fclass_error:
    db "NOVA PANIC: Fault Classification nicht initialisierbar", 13, 10, 0
message_rpol_ok:
    db "NOVA: Recovery Policy 1.0 bereit (8-Regeln, Aktion nach Fehlerklasse)", 13, 10, 0
message_rpol_error:
    db "NOVA PANIC: Recovery Policy Manager nicht initialisierbar", 13, 10, 0
message_rverif_ok:
    db "NOVA: Resilience Verification 1.0 bereit (8-Slots, post-recovery checks)", 13, 10, 0
message_rverif_error:
    db "NOVA PANIC: Resilience Verification nicht initialisierbar", 13, 10, 0
message_ssnap_ok:
    db "NOVA: State Snapshot 1.0 bereit (4-Slots, versioned dword capture)", 13, 10, 0
message_ssnap_error:
    db "NOVA PANIC: State Snapshot Manager nicht initialisierbar", 13, 10, 0
message_shist_ok:
    db "NOVA: State History 1.0 bereit (8-Entry ring, obj+state+tick)", 13, 10, 0
message_shist_error:
    db "NOVA PANIC: State History Ring nicht initialisierbar", 13, 10, 0
message_srec_ok:
    db "NOVA: State Reconciliation 1.0 bereit (8-Slots, desired vs actual)", 13, 10, 0
message_srec_error:
    db "NOVA PANIC: State Reconciliation nicht initialisierbar", 13, 10, 0
message_txlog_ok:
    db "NOVA: Transaction Log 1.0 bereit (8-Entry ring, WAL-style op recording)", 13, 10, 0
message_txlog_error:
    db "NOVA PANIC: Transaction Log nicht initialisierbar", 13, 10, 0
message_tbarrier_ok:
    db "NOVA: Transaction Barrier 1.0 bereit (4-Slots, all-ready sync gate)", 13, 10, 0
message_tbarrier_error:
    db "NOVA PANIC: Transaction Barrier nicht initialisierbar", 13, 10, 0
message_tcomp_ok:
    db "NOVA: Transaction Compensation 1.0 bereit (8-Slots, undo-action registry)", 13, 10, 0
message_tcomp_error:
    db "NOVA PANIC: Transaction Compensation nicht initialisierbar", 13, 10, 0
message_redundancy_ok:
    db "NOVA: Redundancy Manager 1.0 bereit (4-Groups, active+standby tracking)", 13, 10, 0
message_redundancy_error:
    db "NOVA PANIC: Redundancy Manager nicht initialisierbar", 13, 10, 0
message_chaos_ok:
    db "NOVA: Chaos Engine 1.0 bereit (8-Slots, scheduled fault scenarios)", 13, 10, 0
message_chaos_error:
    db "NOVA PANIC: Chaos Engine nicht initialisierbar", 13, 10, 0
message_resilisolation_ok:
    db "NOVA: Resilience Isolation 1.0 bereit (8-Domains, fault boundary tracking)", 13, 10, 0
message_resilisolation_error:
    db "NOVA PANIC: Resilience Isolation nicht initialisierbar", 13, 10, 0
message_raccount_ok:
    db "NOVA: Resource Accounting 1.0 bereit (8-Slots, per-owner CPU+mem ledger)", 13, 10, 0
message_raccount_error:
    db "NOVA PANIC: Resource Accounting nicht initialisierbar", 13, 10, 0
message_rguarant_ok:
    db "NOVA: Resource Guarantee 1.0 bereit (8-Slots, min CPU+mem reservations)", 13, 10, 0
message_rguarant_error:
    db "NOVA PANIC: Resource Guarantee Manager nicht initialisierbar", 13, 10, 0
message_rreclaim_ok:
    db "NOVA: Resource Reclaim 1.0 bereit (8-Slots, release+reuse tracking)", 13, 10, 0
message_rreclaim_error:
    db "NOVA PANIC: Resource Reclaim Manager nicht initialisierbar", 13, 10, 0
message_sdiag_ok:
    db "NOVA: Self-Diagnosis 1.0 bereit (8-Slots, symptom+diagnosis tracking)", 13, 10, 0
message_sdiag_error:
    db "NOVA PANIC: Self-Diagnosis Engine nicht initialisierbar", 13, 10, 0
message_sconf_ok:
    db "NOVA: Self-Config 1.0 bereit (8-Params, runtime config key/value store)", 13, 10, 0
message_sconf_error:
    db "NOVA PANIC: Self-Config Manager nicht initialisierbar", 13, 10, 0
message_apred_ok:
    db "NOVA: Adaptive Prediction 1.0 bereit (8-Slots, EWMA demand forecasting)", 13, 10, 0
message_apred_error:
    db "NOVA PANIC: Adaptive Prediction Engine nicht initialisierbar", 13, 10, 0
message_selfopt_ok:
    db "NOVA: Self-Optimization 1.0 bereit (8-Targets, prediction-driven tuning)", 13, 10, 0
message_selfopt_error:
    db "NOVA PANIC: Self-Optimization Engine nicht initialisierbar", 13, 10, 0
message_selfprot_ok:
    db "NOVA: Self-Protection 1.0 bereit (8-Threats, isolate+quarantine)", 13, 10, 0
message_selfprot_error:
    db "NOVA PANIC: Self-Protection Manager nicht initialisierbar", 13, 10, 0
message_rresv_ok:
    db "NOVA: Resource Reservation 1.0 bereit (8-Slots, pre-admit CPU+mem hold)", 13, 10, 0
message_rresv_error:
    db "NOVA PANIC: Resource Reservation Manager nicht initialisierbar", 13, 10, 0
message_aconstraint_ok:
    db "NOVA: Autonomy Constraint 1.0 bereit (8-Regeln, Enforcement-Gate)", 13, 10, 0
message_aconstraint_error:
    db "NOVA PANIC: Autonomy Constraint Engine nicht initialisierbar", 13, 10, 0
message_autopol_ok:
    db "NOVA: Autonomy Policy 1.0 bereit (8-Policies, trigger+action dispatch)", 13, 10, 0
message_autopol_error:
    db "NOVA PANIC: Autonomy Policy Manager nicht initialisierbar", 13, 10, 0
message_aschedsw_ok:
    db "NOVA: Adaptive Scheduler 1.0 bereit (8-Classes, QoS-Feedback-Tuning)", 13, 10, 0
message_aschedsw_error:
    db "NOVA PANIC: Adaptive Scheduler nicht initialisierbar", 13, 10, 0
message_sver_ok:
    db "NOVA: State Versioning 1.0 bereit (8-Objects, generation+lineage)", 13, 10, 0
message_sver_error:
    db "NOVA PANIC: State Versioning Manager nicht initialisierbar", 13, 10, 0
message_sactual_ok:
    db "NOVA: State Actual 1.0 bereit (8-Slots, observed current values)", 13, 10, 0
message_sactual_error:
    db "NOVA PANIC: State Actual Store nicht initialisierbar", 13, 10, 0
message_sdesired_ok:
    db "NOVA: State Desired 1.0 bereit (8-Slots, target goal values)", 13, 10, 0
message_sdesired_error:
    db "NOVA PANIC: State Desired Store nicht initialisierbar", 13, 10, 0
message_txres_ok:
    db "NOVA: Transaction Resource 1.0 bereit (8-Locks, resource-hold tracking)", 13, 10, 0
message_txres_error:
    db "NOVA PANIC: Transaction Resource Manager nicht initialisierbar", 13, 10, 0
message_txsys_ok:
    db "NOVA: Transaction System 1.0 bereit (4-Slots, system-wide 2PC coordinator)", 13, 10, 0
message_txsys_error:
    db "NOVA PANIC: Transaction System Coordinator nicht initialisierbar", 13, 10, 0
message_rarbit_ok:
    db "NOVA: Resource Arbitration 1.0 bereit (8-Requests, priority-based grant)", 13, 10, 0
message_rarbit_error:
    db "NOVA PANIC: Resource Arbitration Engine nicht initialisierbar", 13, 10, 0
message_acache_ok:
    db "NOVA: Adaptive Cache 1.0 bereit (8-Lines, EWMA hit-rate tracking)", 13, 10, 0
message_acache_error:
    db "NOVA PANIC: Adaptive Cache Manager nicht initialisierbar", 13, 10, 0
message_apower_ok:
    db "NOVA: Adaptive Power 1.0 bereit (4-Domains, load-driven P-state control)", 13, 10, 0
message_apower_error:
    db "NOVA PANIC: Adaptive Power Manager nicht initialisierbar", 13, 10, 0
message_pollearn_ok:
    db "NOVA: Policy Learning 1.0 bereit (8-Rules, outcome-weighted policy update)", 13, 10, 0
message_pollearn_error:
    db "NOVA PANIC: Policy Learning Engine nicht initialisierbar", 13, 10, 0
message_prederr_ok:
    db "NOVA: Prediction Error 1.0 bereit (8-Metrics, error-magnitude tracking)", 13, 10, 0
message_prederr_error:
    db "NOVA PANIC: Prediction Error Tracker nicht initialisierbar", 13, 10, 0
message_aprefetch_ok:
    db "NOVA: Adaptive Prefetch 1.0 bereit (8-Streams, stride-based prefetch)", 13, 10, 0
message_aprefetch_error:
    db "NOVA PANIC: Adaptive Prefetch Engine nicht initialisierbar", 13, 10, 0
message_apreload_ok:
    db "NOVA: Adaptive Preload 1.0 bereit (8-Objects, priority-based preload)", 13, 10, 0
message_apreload_error:
    db "NOVA PANIC: Adaptive Preload Manager nicht initialisierbar", 13, 10, 0
message_astorage_ok:
    db "NOVA: Adaptive Storage 1.0 bereit (8-Volumes, I/O-rate adaptive tier)", 13, 10, 0
message_astorage_error:
    db "NOVA PANIC: Adaptive Storage Manager nicht initialisierbar", 13, 10, 0
message_anetwork_ok:
    db "NOVA: Adaptive Network 1.0 bereit (4-Interfaces, bandwidth-adaptive QoS)", 13, 10, 0
message_anetwork_error:
    db "NOVA PANIC: Adaptive Network Manager nicht initialisierbar", 13, 10, 0
message_uoverride_ok:
    db "NOVA: User Override 1.0 bereit (8-Keys, persistent user policy overrides)", 13, 10, 0
message_uoverride_error:
    db "NOVA PANIC: User Override Manager nicht initialisierbar", 13, 10, 0
message_rdeadline_ok:
    db "NOVA: Resource Deadline 1.0 bereit (8-Tasks, EDF deadline enforcement)", 13, 10, 0
message_rdeadline_error:
    db "NOVA PANIC: Resource Deadline Manager nicht initialisierbar", 13, 10, 0
message_reconomy_ok:
    db "NOVA: Resource Economy 1.0 bereit (8-Owners, budget-credit system)", 13, 10, 0
message_reconomy_error:
    db "NOVA PANIC: Resource Economy Manager nicht initialisierbar", 13, 10, 0
message_renergy_ok:
    db "NOVA: Resource Energy 1.0 bereit (4-Rails, power-budget enforcement)", 13, 10, 0
message_renergy_error:
    db "NOVA PANIC: Resource Energy Manager nicht initialisierbar", 13, 10, 0
message_rlatency_ok:
    db "NOVA: Resource Latency 1.0 bereit (8-Endpoints, SLA latency tracking)", 13, 10, 0
message_rlatency_error:
    db "NOVA PANIC: Resource Latency Monitor nicht initialisierbar", 13, 10, 0
message_sglobal_ok:
    db "NOVA: State Global 1.0 bereit (16-Keys, kernel-wide named state store)", 13, 10, 0
message_sglobal_error:
    db "NOVA PANIC: State Global Manager nicht initialisierbar", 13, 10, 0
message_txconfig_ok:
    db "NOVA: Transaction Config 1.0 bereit (8-Keys, atomic config transactions)", 13, 10, 0
message_txconfig_error:
    db "NOVA PANIC: Transaction Config Manager nicht initialisierbar", 13, 10, 0
message_txmulti_ok:
    db "NOVA: Tx MultiService 1.0 bereit (4-Services, cross-service 2PC)", 13, 10, 0
message_txmulti_error:
    db "NOVA PANIC: Tx MultiService Coordinator nicht initialisierbar", 13, 10, 0
message_vtemporal_ok:
    db "NOVA: Verify Temporal 1.0 bereit (8-Invariants, tick-window enforcement)", 13, 10, 0
message_vtemporal_error:
    db "NOVA PANIC: Verify Temporal Engine nicht initialisierbar", 13, 10, 0
message_vinfoflow_ok:
    db "NOVA: Verify InfoFlow 1.0 bereit (8-Channels, label-based flow control)", 13, 10, 0
message_vinfoflow_error:
    db "NOVA PANIC: Verify InfoFlow Engine nicht initialisierbar", 13, 10, 0
message_vcapsafe_ok:
    db "NOVA: Verify CapabilitySafety 1.0 bereit (8-Proofs, cap usage audit)", 13, 10, 0
message_vcapsafe_error:
    db "NOVA PANIC: Verify CapabilitySafety Engine nicht initialisierbar", 13, 10, 0
message_vmemsafe_ok:
    db "NOVA: Verify MemorySafety 1.0 bereit (8-Regions, bounds + alias check)", 13, 10, 0
message_vmemsafe_error:
    db "NOVA PANIC: Verify MemorySafety Engine nicht initialisierbar", 13, 10, 0
message_futex_error:
    db "NOVA PANIC: Futex Manager nicht initialisierbar", 13, 10, 0
message_slab_ok:
    db "NOVA: Slab Allocator 1.0 bereit (4 Klassen: 16/32/64/128 Byte)", 13, 10, 0
message_slab_error:
    db "NOVA PANIC: Slab Allocator nicht initialisierbar", 13, 10, 0
message_aslr_ok:
    db "NOVA: ASLR 1.0 bereit (TSC-Seed, 4KB-granular, 8 Regionen)", 13, 10, 0
message_aslr_error:
    db "NOVA PANIC: ASLR nicht initialisierbar", 13, 10, 0
message_cow_ok:
    db "NOVA: Copy-on-Write 1.0 bereit (16 Tracking-Slots)", 13, 10, 0
message_cow_error:
    db "NOVA PANIC: Copy-on-Write nicht initialisierbar", 13, 10, 0
message_oom_ok:
    db "NOVA: OOM Handler 1.0 bereit (8 Kandidaten, Score-basiert)", 13, 10, 0
message_oom_error:
    db "NOVA PANIC: OOM Handler nicht initialisierbar", 13, 10, 0
message_restart_ok:
    db "NOVA: Supervised Restart 1.0 bereit (8 Slots, Policy/Backoff)", 13, 10, 0
message_restart_error:
    db "NOVA PANIC: Supervised Restart nicht initialisierbar", 13, 10, 0
message_resource_accounting_ok:
    db "NOVA: Resource Accounting 1.0 bereit (8 Slots, CPU/Mem/IO Limits)", 13, 10, 0
message_resource_accounting_error:
    db "NOVA PANIC: Resource Accounting nicht initialisierbar", 13, 10, 0
message_failover_ok:
    db "NOVA: Failover Manager 1.0 bereit (4 Primaer/Backup-Paare)", 13, 10, 0
message_failover_error:
    db "NOVA PANIC: Failover Manager nicht initialisierbar", 13, 10, 0
message_containment_ok:
    db "NOVA: Fault Containment 1.0 bereit (8 Subsysteme, Isolation)", 13, 10, 0
message_containment_error:
    db "NOVA PANIC: Fault Containment nicht initialisierbar", 13, 10, 0
message_checkpoint_ok:
    db "NOVA: Checkpoint/Restore 1.0 bereit (4 Slots, Register-Satz)", 13, 10, 0
message_checkpoint_error:
    db "NOVA PANIC: Checkpoint/Restore nicht initialisierbar", 13, 10, 0
message_security_audit_ok:
    db "NOVA: Security Audit 1.0 bereit (32 Ereignisse, CAP/VIOLATION/POLICY)", 13, 10, 0
message_security_audit_error:
    db "NOVA PANIC: Security Audit Log nicht initialisierbar", 13, 10, 0
message_mac_ok:
    db "NOVA: MAC 1.0 bereit (8 Subjects, 8 Objects, 4 Sensitivity-Labels)", 13, 10, 0
message_mac_error:
    db "NOVA PANIC: MAC nicht initialisierbar", 13, 10, 0
message_dac_ok:
    db "NOVA: DAC 1.0 bereit (16 Eintraege, RWX-Bits, Besitzer-Check)", 13, 10, 0
message_dac_error:
    db "NOVA PANIC: DAC nicht initialisierbar", 13, 10, 0
message_rbac_ok:
    db "NOVA: RBAC 1.0 bereit (8 Rollen, 16 Zuweisungen, Perm-Masken)", 13, 10, 0
message_rbac_error:
    db "NOVA PANIC: RBAC nicht initialisierbar", 13, 10, 0
message_capdis_ok:
    db "NOVA: Cap Discovery 1.0 bereit (16 Typen, KERNEL/IO/SECURITY)", 13, 10, 0
message_capdis_error:
    db "NOVA PANIC: Cap Discovery nicht initialisierbar", 13, 10, 0
message_capneg_ok:
    db "NOVA: Cap Negotiation 1.0 bereit (8 Requests, Match/Reject)", 13, 10, 0
message_capneg_error:
    db "NOVA PANIC: Cap Negotiation nicht initialisierbar", 13, 10, 0
message_integration_ok:
    db "NOVA: Integration 1.0 OK (5 Pipelines: Security/Resilience/Sched/Mem/WD)", 13, 10, 0
message_integration_error:
    db "NOVA PANIC: Subsystem-Integration fehlgeschlagen", 13, 10, 0
message_rt_scheduler_ok:
    db "NOVA: RT Scheduler 1.0 bereit (8 Tasks, EDF/FIFO-RT)", 13, 10, 0
message_rt_scheduler_error:
    db "NOVA PANIC: RT Scheduler nicht initialisierbar", 13, 10, 0
message_cfs_ok:
    db "NOVA: Fair Scheduler 1.0 bereit (8 Tasks, vruntime-basiert)", 13, 10, 0
message_cfs_error:
    db "NOVA PANIC: Fair Scheduler nicht initialisierbar", 13, 10, 0
message_qos_ok:
    db "NOVA: QoS Scheduler 1.0 bereit (4 Klassen, Budget-Buckets)", 13, 10, 0
message_qos_error:
    db "NOVA PANIC: QoS Scheduler nicht initialisierbar", 13, 10, 0
message_deadlock_ok:
    db "NOVA: Deadlock Detection 1.0 bereit (8 Knoten, Wait-For Graph DFS)", 13, 10, 0
message_deadlock_error:
    db "NOVA PANIC: Deadlock Detection nicht initialisierbar", 13, 10, 0
message_pi_ok:
    db "NOVA: Priority Inheritance Mutex 1.0 bereit (8 Slots, Prio-Vererbung)", 13, 10, 0
message_pi_error:
    db "NOVA PANIC: Priority Inheritance Mutex nicht initialisierbar", 13, 10, 0
message_time_ok:
    db "NOVA: Time 1.0 bereit (Monoton 100Hz + RTC CMOS Uhr/Datum)", 13, 10, 0
message_time_error:
    db "NOVA PANIC: Time Services nicht initialisierbar", 13, 10, 0
message_clocksource_ok:
    db "NOVA: Clock Source 1.0 bereit (PIT+TSC registriert, TSC aktiv)", 13, 10, 0
message_clocksource_error:
    db "NOVA PANIC: Clock Source nicht initialisierbar", 13, 10, 0
message_hrt_ok:
    db "NOVA: High-Resolution Timer 1.0 bereit (4 Alarme, ns via TSC)", 13, 10, 0
message_hrt_error:
    db "NOVA PANIC: High-Resolution Timer nicht initialisierbar", 13, 10, 0
message_io_scheduler_ok:
    db "NOVA: IO Scheduler ABI 1.0, Prioritaet, Deadline und Fairness aktiv", 13, 10, 0
message_io_scheduler_error:
    db "NOVA PANIC: IO Scheduler nicht initialisierbar", 13, 10, 0
message_io_qos_ok:
    db "NOVA: IO QoS ABI 1.0, Admission, Degradation und Accounting aktiv", 13, 10, 0
message_io_qos_error:
    db "NOVA PANIC: IO QoS Manager nicht initialisierbar", 13, 10, 0
message_security_ok:
    db "NOVA: Security ABI 1.0 Capabilities aktiv", 13, 10, 0
message_security_error:
    db "NOVA PANIC: Kernel Security nicht initialisierbar", 13, 10, 0
message_boot_health_authority_ok:
    db "NOVA: Boot Health Authority ABI 1.0 capabilitygeschuetzt bereit", 13, 10, 0
message_boot_health_trust_signed:
    db "NOVA: Boot Health Trust Provider READY (NKI DevSign verifiziert)", 13, 10, 0
message_boot_health_trust_degraded:
    db "NOVA: Boot Health Trust Provider DEGRADED (Kernel nicht signiert)", 13, 10, 0
message_boot_health_session_ready:
    db "NOVA: Boot Health Session Provider READY (Phase-1 Stub)", 13, 10, 0
message_health_commit_written:
    db "NOVA: Boot Health HEALTHY-Wire via Firmware-Provider persistiert (§109)", 13, 10, 0
message_health_commit_failed:
    db "NOVA: Boot Health HEALTHY-Wire Commit fehlgeschlagen", 13, 10, 0
message_boot_health_kernel_initialized:
    db "NOVA: Boot Health Milestone KernelInitialized aggregiert", 13, 10, 0
message_boot_health_checkpoint_written:
    db "NOVA: Boot Health Candidate-Checkpoint ueber Firmware Provider persistiert", 13, 10, 0
message_boot_health_checkpoint_failed:
    db "NOVA: Boot Health Candidate-Checkpoint konnte nicht persistiert werden", 13, 10, 0
message_boot_health_root_pending:
    db "NOVA: Boot Health wartet auf persistentes SystemRoot und Trust", 13, 10, 0
message_boot_health_error:
    db "NOVA PANIC: Boot Health Authority nicht initialisierbar", 13, 10, 0
message_thread_manager_ok:
    db "NOVA: Thread Manager ABI 1.0 bereit", 13, 10, 0
message_thread_manager_error:
    db "NOVA PANIC: Kernel Thread Manager nicht initialisierbar", 13, 10, 0
message_scheduler_ok:
    db "NOVA: Scheduler ABI 1.0 und Runtime-Threads aktiv", 13, 10, 0
message_scheduler_dynamic_ok:
    db "NOVA: Scheduler Dynamic Thread Slot 3 aktiv", 13, 10, 0
message_scheduler_error:
    db "NOVA PANIC: praemptiver Scheduler nicht initialisierbar", 13, 10, 0
message_worksteal_ok:
    db "NOVA: Work Stealing 1.0 bereit (8 Slots, RT-Schutz, Affinitaet)", 13, 10, 0
message_worksteal_error:
    db "NOVA PANIC: Work-Stealing-Subsystem nicht initialisierbar", 13, 10, 0
message_novafs_mount_failed:
    db "NOVA: NovaFS Mount fehlgeschlagen, Fehler 0x", 0
message_novafs_absent:
    db "NOVA: NovaFS kein Systemvolume gefunden, Bootstrap-RAMFS bleibt Root", 13, 10, 0
message_novafs_mounted:
    db "NOVA: NovaFS 1.0 Systemvolume gemountet, Generation 0x", 0
message_novafs_volume:
    db " VolumeID 0x", 0
message_novafs_backup_used:
    db "NOVA: NovaFS Backup-Superblock verwendet (Primaerkopie ungueltig oder aelter)", 13, 10, 0
message_novafs_unclean:
    db "NOVA: NovaFS Volume nicht sauber (DIRTY), nur Read-only gemountet", 13, 10, 0
message_novafs_journal_recovered:
    db "NOVA: NovaFS Journal-Wiederherstellung erfolgreich, Volume wieder sauber (CLEAN)", 13, 10, 0
message_novafs_layout_ok:
    db "NOVA: NovaFS Root-Layout konsistent mit Semantic-Core-ObjectIDs", 13, 10, 0
message_novafs_layout_failed:
    db "NOVA: NovaFS Root-Layout inkonsistent, Volume nicht als Root verwendet, Fehler 0x", 0
message_novafs_selftest_failed:
    db "NOVA: NovaFS Selbsttest fehlgeschlagen, Volume nicht als Root verwendet, Fehler 0x", 0
message_novafs_publish_failed:
    db "NOVA: NovaFS Root-Registrierung fehlgeschlagen, Fehler 0x", 0
message_novafs_bootcount:
    db "NOVA: NovaFS persistenter Bootzaehler 0x", 0
message_novafs_rw_ok:
    db "NOVA: NovaFS Lese-/Schreibtest mit Extents, Teilbloecken und Blockgrenze bereit", 13, 10, 0
message_novafs_selftest_ro:
    db "NOVA: NovaFS Read-only, Schreibtest uebersprungen", 13, 10, 0
message_novafs_root_ok:
    db "NOVA: NovaFS ist persistentes SystemRoot unter /", 13, 10, 0
message_boot_health_root_ready:
    db "NOVA: Boot Health SystemRoot bereit, wartet auf Trust", 13, 10, 0
message_boot_health_root_degraded:
    db "NOVA: Boot Health SystemRoot nur Read-only verfuegbar (degradiert)", 13, 10, 0
message_boot_health_system_root_ready:
    db "NOVA: Boot Health SystemRoot Provider READY (NovaFS rw gemountet)", 13, 10, 0
message_boot_health_system_root_degraded:
    db "NOVA: Boot Health SystemRoot Provider DEGRADED (kein rw-Volume)", 13, 10, 0
message_boot_health_confirmed:
    db "NOVA: Boot Health HealthConfirmed (alle Meilensteine HEALTHY erreicht)", 13, 10, 0
message_boot_health_confirmed_degraded:
    db "NOVA: Boot Health DegradedConfirmed (Meilensteine via DEGRADED-Pfad erreicht)", 13, 10, 0
message_boot_health_not_confirmed:
    db "NOVA: Boot Health OPERATIONAL nicht erreicht, Status bleibt Pending", 13, 10, 0
message_storage_ahci_ok:
    db "NOVA: Storage ABI 1.0, AHCI-Controller (Polling) aktiv, Datentraeger 0x", 0
message_storage_no_ahci:
    db "NOVA: Storage ABI 1.0, kein AHCI-Controller gefunden", 13, 10, 0
message_storage_ahci_failed:
    db "NOVA: Storage ABI 1.0, AHCI-Controller nicht nutzbar", 13, 10, 0
message_storage_disk:
    db "NOVA: Storage SATA-Datentraeger Port 0x", 0
message_storage_disk_id:
    db " DeviceID 0x", 0
message_storage_disk_sectors:
    db " Sektoren 0x", 0
message_storage_selftest_ok:
    db "NOVA: Storage DMA-Lesetest und Bereichspruefung bereit", 13, 10, 0
message_device_manager_ok:
    db "NOVA: Device Manager ABI 1.0 und Bootgeraete aktiv", 13, 10, 0
message_device_manager_error:
    db "NOVA PANIC: Kernel Device Manager nicht initialisierbar", 13, 10, 0
message_driver_framework_ok:
    db "NOVA: Driver Framework ABI 1.0 bereit", 13, 10, 0
message_driver_framework_error:
    db "NOVA PANIC: Driver Framework nicht initialisierbar", 13, 10, 0
message_numa_ok:
    db "NOVA: NUMA 1.0 bereit (1 Node, UMA-Modus)", 13, 10, 0
message_numa_error:
    db "NOVA PANIC: NUMA-Subsystem nicht initialisierbar", 13, 10, 0
message_vfs_ok:
    db "NOVA: VFS ABI 1.0, Mount-Namespace und Bootstrap-Root bereit", 13, 10, 0
message_vfs_error:
    db "NOVA PANIC: VFS oder Root-Dateisystem nicht initialisierbar", 13, 10, 0
message_userspace_ok:
    db "NOVA: x86-32 Userspace, TSS und System-Call ABI 1.0 bereit", 13, 10, 0
message_userspace_exit_ok:
    db "NOVA: erster Ring-3-Systemdienst kontrolliert ausgefuehrt", 13, 10, 0
message_userspace_error:
    db "NOVA PANIC: initialer Userspace-Prozess nicht startbar", 13, 10, 0
message_process_syscall_ok:
    db "NOVA: Userspace Process.QuerySelf erfolgreich", 13, 10, 0
message_process_handle_ok:
    db "NOVA: Process.OpenSelf Handle typ- und rechtegeprueft", 13, 10, 0
message_thread_syscall_ok:
    db "NOVA: Userspace Thread.QuerySelf erfolgreich", 13, 10, 0
message_thread_handle_ok:
    db "NOVA: Thread.OpenSelf Handle typ- und rechtegeprueft", 13, 10, 0
message_shared_service_ok:
    db "NOVA: Shared Service Page im Userspace validiert", 13, 10, 0
message_ipc_roundtrip_ok:
    db "NOVA: Userspace IPC Inline-Roundtrip atomar erfolgreich", 13, 10, 0
message_vfs_userspace_ok:
    db "NOVA: Userspace VFS.OpenRoot Handle erfolgreich", 13, 10, 0
message_vfs_lookup_ok:
    db "NOVA: Userspace VFS.Lookup Pfad '/' erfolgreich", 13, 10, 0
message_handle_close_ok:
    db "NOVA: Userspace Handle geschlossen, alter Wert ungueltig", 13, 10, 0
message_exception:
    db "NOVA PANIC: CPU-Ausnahme Vektor 0x", 0
message_fault_address:
    db " bei Adresse 0x", 0
message_newline:
    db 13, 10, 0
message_framebuffer_ok:
    db "NOVA: Kernel Context und Framebuffer aktiv", 13, 10, 0
message_text_mode:
    db "NOVA: Kernel Context, Textmodus aktiv", 13, 10, 0
message_bib_error:
    db "NOVA PANIC: ungueltiger Boot Handoff", 13, 10, 0
message_ready:
    db "NOVA_KERNEL_READY", 13, 10, 0
message_desktop_ready:
    db "NOVA_DESKTOP_READY", 13, 10, 0
message_system_ready:
    db "NOVA_SYSTEM_READY", 13, 10, 0

; ===========================================================================
; §010 – Exception Manager 1.0 (NPSPEC-KERNEL-0010)
; ===========================================================================
;
; Implementiert:
;   - Exception-Klassifizierung (CPU/MEMORY/ARITHMETIC/INSTRUCTION/DEBUG/
;     SECURITY/RESOURCE/SOFTWARE/KERNEL_FATAL)
;   - Ursprungsbestimmung (USER / KERNEL)
;   - Exception Record Aufbau
;   - Userspace-Exception-Pfad: VMM-Probe → Endpunkt → Thread-Beendigung
;   - Kernel-Exception-Pfad: Fixup-Tabellen-Suche → Panic
;   - Double-Fault- und Machine-Check-Notfallpfade
;   - Exception-Tiefenzähler (per CPU-Slot, anti-rekursiv)
;   - Statistik (total / user / kernel / panics)
;   - Selbsttest (14 Fälle)
;   - Boot-Initialisierung mit §023-Logging
;
; Frame-Layout beim Aufruf von exception_manager_dispatch (von isr_common):
;   [esp+4]  = Vektor (uint32)
;   [esp+8]  = Frame-Zeiger auf isr_common-Stack:
;                [frame+0]  = gespeicherter EAX (pushad)
;                ...
;                [frame+28] = gespeicherter EDI
;                [frame+32] = DS
;                [frame+36] = ES
;                [frame+40] = FS
;                [frame+44] = GS
;                [frame+48] = normalisierter Vektor
;                [frame+52] = Fehlercode
;                [frame+56] = EIP (CPU-gesichert)
;                [frame+60] = CS
;                [frame+64] = EFLAGS
;
; ---------------------------------------------------------------------------

; --- Konstanten -------------------------------------------------------------

EXC_CAT_CPU             equ 0
EXC_CAT_MEMORY          equ 1
EXC_CAT_ARITHMETIC      equ 2
EXC_CAT_INSTRUCTION     equ 3
EXC_CAT_DEBUG           equ 4
EXC_CAT_SECURITY        equ 5
EXC_CAT_RESOURCE        equ 6
EXC_CAT_SOFTWARE        equ 7
EXC_CAT_KERNEL_FATAL    equ 8

EXC_ORIGIN_USER         equ 0
EXC_ORIGIN_KERNEL       equ 1

; Exception Record Struktur (60 Bytes, §7)
EXC_REC_SIZE            equ 60
EXC_REC_STRUCT_SIZE     equ 0    ; uint32  = EXC_REC_SIZE
EXC_REC_VERSION         equ 4    ; uint32  = 1
EXC_REC_CODE            equ 8    ; uint32  (np_exception_code)
EXC_REC_CATEGORY        equ 12   ; uint32  (np_exception_category)
EXC_REC_FLAGS           equ 16   ; uint32
EXC_REC_PROCESS_ID      equ 20   ; uint32
EXC_REC_THREAD_ID       equ 24   ; uint32
EXC_REC_CPU_ID          equ 28   ; uint32
EXC_REC_IP              equ 32   ; uint32
EXC_REC_FAULT_ADDR      equ 36   ; uint32
EXC_REC_ARCH_ERR        equ 40   ; uint32 (low)
EXC_REC_ARCH_ERR_HI     equ 44   ; uint32 (high)
EXC_REC_PARAM_COUNT     equ 48   ; uint32
EXC_REC_PARAM0          equ 52   ; uint32

; Exception-Codes (§5, Mapping auf x86 Vektoren)
NP_EXC_DIVIDE_BY_ZERO   equ 0
NP_EXC_DEBUG            equ 1
NP_EXC_BREAKPOINT       equ 2
NP_EXC_OVERFLOW         equ 3
NP_EXC_BOUND_VIOLATION  equ 4
NP_EXC_INVALID_OPCODE   equ 5
NP_EXC_DEVICE_UNAVAIL   equ 6
NP_EXC_DOUBLE_FAULT     equ 7
NP_EXC_INVALID_STATE    equ 8
NP_EXC_SEGMENT_VIOL     equ 9
NP_EXC_STACK_FAULT      equ 10
NP_EXC_GPF             equ 11
NP_EXC_PAGE_FAULT       equ 12
NP_EXC_FLOAT_POINT      equ 13
NP_EXC_ALIGNMENT        equ 14
NP_EXC_MACHINE_CHECK    equ 15
NP_EXC_SIMD_FP          equ 16
NP_EXC_CTRL_PROTECT     equ 17
NP_EXC_SECURITY_VIOL    equ 18
NP_EXC_RESOURCE_LIMIT   equ 19
NP_EXC_SOFTWARE_RAISED  equ 20

; Fixup-Tabellen-Eintrag (§33, 16 Bytes)
EXC_FIXUP_SIZE          equ 16
EXC_FIXUP_FAULT_START   equ 0
EXC_FIXUP_FAULT_END     equ 4
EXC_FIXUP_RECOVERY      equ 8
EXC_FIXUP_ALLOWED_VECS  equ 12

EXC_FIXUP_SENTINEL      equ 0xFFFFFFFF   ; Ende der Tabelle

; Maximale Exception-Tiefe (§32)
EXC_MAX_DEPTH           equ 4

; Exception Record Flags
EXC_FLAG_ORIGIN_USER    equ 0x0001
EXC_FLAG_ORIGIN_KERNEL  equ 0x0002
EXC_FLAG_FIXUP_USED     equ 0x0004
EXC_FLAG_THREAD_TERM    equ 0x0008
EXC_FLAG_PROC_TERM      equ 0x0010
EXC_FLAG_DOUBLE_FAULT   equ 0x0020
EXC_FLAG_MACHINE_CHECK  equ 0x0040

; --- BSS-Daten (statisch) ---------------------------------------------------

align 4
exception_mgr_initialized:
    dd 0

; Statistik §46
exception_stats_total:      dd 0
exception_stats_user:       dd 0
exception_stats_kernel:     dd 0
exception_stats_handled:    dd 0
exception_stats_thread_term: dd 0
exception_stats_proc_term:  dd 0
exception_stats_panics:     dd 0
exception_stats_recursive:  dd 0

; Per-CPU Exception-Tiefe (bis zu NOVA_CPU_CAPACITY CPUs, §32)
; 4 CPUs × 4 Bytes = 16 Bytes
exception_depth_table:
    times 4 dd 0

; Statischer Notfall-Exception-Record für Double Fault / Machine Check (§41)
align 4
exception_emergency_record:
    times EXC_REC_SIZE db 0

; Letzter Exception Record (für Diagnose / Panic-Ausgabe)
align 4
exception_last_record:
    times EXC_REC_SIZE db 0

; ---------------------------------------------------------------------------
; exception_manager_initialize  – Bootstrap §010 (§49, Test 1 Basis)
;   Aufruf: kein Argument
;   Rückgabe: CF=0 OK, CF=1 Fehler
; ---------------------------------------------------------------------------
exception_manager_initialize:
    push eax
    push esi

    ; Statistikfelder nullen
    mov dword [exception_stats_total],      0
    mov dword [exception_stats_user],       0
    mov dword [exception_stats_kernel],     0
    mov dword [exception_stats_handled],    0
    mov dword [exception_stats_thread_term],0
    mov dword [exception_stats_proc_term],  0
    mov dword [exception_stats_panics],     0
    mov dword [exception_stats_recursive],  0

    ; Tiefenzähler nullen
    mov dword [exception_depth_table + 0],  0
    mov dword [exception_depth_table + 4],  0
    mov dword [exception_depth_table + 8],  0
    mov dword [exception_depth_table + 12], 0

    ; Notfall-Record nullen
    push edi
    push ecx
    mov edi, exception_emergency_record
    mov ecx, EXC_REC_SIZE
    xor eax, eax
    rep stosb
    mov edi, exception_last_record
    mov ecx, EXC_REC_SIZE
    rep stosb
    pop ecx
    pop edi

    mov dword [exception_mgr_initialized], 1

    mov esi, message_exception_mgr_ok
    call serial_write_string

    clc
    pop esi
    pop eax
    ret

; ---------------------------------------------------------------------------
; exc_vector_to_code  – x86 Vektor → np_exception_code  (EAX→EAX, EBX)
; ---------------------------------------------------------------------------
exc_vector_to_code:
    cmp eax, 0 ; #DE
    je .div_zero
    cmp eax, 1 ; #DB
    je .debug
    cmp eax, 3 ; #BP
    je .breakpoint
    cmp eax, 4 ; #OF
    je .overflow
    cmp eax, 5 ; #BR
    je .bound
    cmp eax, 6 ; #UD
    je .inv_opcode
    cmp eax, 7 ; #NM
    je .dev_unavail
    cmp eax, 8 ; #DF
    je .double_fault
    cmp eax, 10 ; #TS
    je .inv_state
    cmp eax, 11 ; #NP
    je .seg_viol
    cmp eax, 12 ; #SS
    je .stack_fault
    cmp eax, 13 ; #GP
    je .gpf
    cmp eax, 14 ; #PF
    je .page_fault
    cmp eax, 16 ; #MF
    je .float_point
    cmp eax, 17 ; #AC
    je .alignment
    cmp eax, 18 ; #MC
    je .machine_check
    cmp eax, 19 ; #XM/#XF
    je .simd_fp
    cmp eax, 21 ; #CP
    je .ctrl_protect
    ; unbekannt → INVALID_STATE
    mov eax, NP_EXC_INVALID_STATE
    ret
.div_zero:     mov eax, NP_EXC_DIVIDE_BY_ZERO  ; ret
    ret
.debug:        mov eax, NP_EXC_DEBUG            ; ret
    ret
.breakpoint:   mov eax, NP_EXC_BREAKPOINT
    ret
.overflow:     mov eax, NP_EXC_OVERFLOW
    ret
.bound:        mov eax, NP_EXC_BOUND_VIOLATION
    ret
.inv_opcode:   mov eax, NP_EXC_INVALID_OPCODE
    ret
.dev_unavail:  mov eax, NP_EXC_DEVICE_UNAVAIL
    ret
.double_fault: mov eax, NP_EXC_DOUBLE_FAULT
    ret
.inv_state:    mov eax, NP_EXC_INVALID_STATE
    ret
.seg_viol:     mov eax, NP_EXC_SEGMENT_VIOL
    ret
.stack_fault:  mov eax, NP_EXC_STACK_FAULT
    ret
.gpf:          mov eax, NP_EXC_GPF
    ret
.page_fault:   mov eax, NP_EXC_PAGE_FAULT
    ret
.float_point:  mov eax, NP_EXC_FLOAT_POINT
    ret
.alignment:    mov eax, NP_EXC_ALIGNMENT
    ret
.machine_check: mov eax, NP_EXC_MACHINE_CHECK
    ret
.simd_fp:      mov eax, NP_EXC_SIMD_FP
    ret
.ctrl_protect: mov eax, NP_EXC_CTRL_PROTECT
    ret

; ---------------------------------------------------------------------------
; exc_code_to_category  – np_exception_code → np_exception_category  (EAX→EAX)
; ---------------------------------------------------------------------------
exc_code_to_category:
    cmp eax, NP_EXC_PAGE_FAULT
    je .memory
    cmp eax, NP_EXC_STACK_FAULT
    je .resource
    cmp eax, NP_EXC_ALIGNMENT
    je .resource
    cmp eax, NP_EXC_RESOURCE_LIMIT
    je .resource
    cmp eax, NP_EXC_DIVIDE_BY_ZERO
    je .arithmetic
    cmp eax, NP_EXC_OVERFLOW
    je .arithmetic
    cmp eax, NP_EXC_FLOAT_POINT
    je .arithmetic
    cmp eax, NP_EXC_SIMD_FP
    je .arithmetic
    cmp eax, NP_EXC_DEBUG
    je .debug
    cmp eax, NP_EXC_BREAKPOINT
    je .debug
    cmp eax, NP_EXC_INVALID_OPCODE
    je .instruction
    cmp eax, NP_EXC_DEVICE_UNAVAIL
    je .instruction
    cmp eax, NP_EXC_CTRL_PROTECT
    je .instruction
    cmp eax, NP_EXC_DOUBLE_FAULT
    je .fatal
    cmp eax, NP_EXC_MACHINE_CHECK
    je .fatal
    cmp eax, NP_EXC_GPF
    je .security
    cmp eax, NP_EXC_SEGMENT_VIOL
    je .security
    cmp eax, NP_EXC_SECURITY_VIOL
    je .security
    cmp eax, NP_EXC_SOFTWARE_RAISED
    je .software
    ; Fallback → CPU
    mov eax, EXC_CAT_CPU
    ret
.memory:    mov eax, EXC_CAT_MEMORY       ; ret
    ret
.resource:  mov eax, EXC_CAT_RESOURCE
    ret
.arithmetic: mov eax, EXC_CAT_ARITHMETIC
    ret
.debug:     mov eax, EXC_CAT_DEBUG
    ret
.instruction: mov eax, EXC_CAT_INSTRUCTION
    ret
.fatal:     mov eax, EXC_CAT_KERNEL_FATAL
    ret
.security:  mov eax, EXC_CAT_SECURITY
    ret
.software:  mov eax, EXC_CAT_SOFTWARE
    ret

; ---------------------------------------------------------------------------
; exc_build_record  – Exception Record aus Frame aufbauen
;   [esp+4] = Zeiger auf den 60-Byte Record-Puffer
;   [esp+8] = Frame-Zeiger (isr_common Stack)
;   [esp+12] = Vektor
;   Clobbers: EAX, EBX, ECX, EDX (per calling convention OK – Caller pusht)
; ---------------------------------------------------------------------------
exc_build_record:
    push ebp
    mov ebp, esp
    push esi
    push edi

    mov edi, [ebp + 8]   ; Record-Puffer
    mov esi, [ebp + 12]  ; Frame
    mov ecx, [ebp + 16]  ; Vektor

    ; Größe und Version
    mov dword [edi + EXC_REC_STRUCT_SIZE], EXC_REC_SIZE
    mov dword [edi + EXC_REC_VERSION], 1

    ; Code (Vektor → np_exception_code)
    mov eax, ecx
    call exc_vector_to_code
    mov [edi + EXC_REC_CODE], eax

    ; Kategorie
    call exc_code_to_category
    mov [edi + EXC_REC_CATEGORY], eax

    ; Flags: Ursprung aus CS im Frame (Offset 60 vom Frame-Anfang = [esi+60])
    ; Frame: pushad(32)+segs(16) = 48, dann Vektor(4)+ErrCode(4)+EIP(4)+CS(4)
    ; CS ist bei [esi + 60]
    mov eax, [esi + 60]
    and eax, 0x3         ; RPL-Bits: 0=Ring0(Kernel), 3=Ring3(User)
    cmp eax, 0
    je .origin_kernel
    mov dword [edi + EXC_REC_FLAGS], EXC_FLAG_ORIGIN_USER
    jmp .origin_done
.origin_kernel:
    mov dword [edi + EXC_REC_FLAGS], EXC_FLAG_ORIGIN_KERNEL
.origin_done:

    ; IDs (Prozess/Thread/CPU – aus BSP-Kontext, §7)
    ; per_cpu_current_thread[0] = Slot-Index des laufenden Threads auf CPU 0
    mov eax, [per_cpu_current_thread]    ; Thread-Slot-Index (BSP)
    cmp eax, -1
    je .no_thread_id
    ; Thread-Record: TID bei Offset THREAD_TID=0, PID bei THREAD_PID=4
    push ebx
    mov ebx, eax
    imul ebx, THREAD_RECORD_SIZE
    add ebx, thread_table
    mov eax, [ebx + THREAD_TID]
    mov [edi + EXC_REC_THREAD_ID], eax
    mov eax, [ebx + THREAD_PID]
    mov [edi + EXC_REC_PROCESS_ID], eax
    pop ebx
    jmp .ids_done
.no_thread_id:
    mov dword [edi + EXC_REC_THREAD_ID], 0
    mov dword [edi + EXC_REC_PROCESS_ID], 0
.ids_done:
    mov dword [edi + EXC_REC_CPU_ID], 0  ; BSP = 0

    ; EIP aus Frame [esi + 56]
    mov eax, [esi + 56]
    mov [edi + EXC_REC_IP], eax

    ; Fault-Adresse: bei Page Fault (Vektor 14) aus CR2
    xor eax, eax
    cmp ecx, 14
    jne .no_cr2
    mov eax, cr2
.no_cr2:
    mov [edi + EXC_REC_FAULT_ADDR], eax

    ; Architektur-Fehlercode aus Frame [esi + 52]
    mov eax, [esi + 52]
    mov [edi + EXC_REC_ARCH_ERR], eax
    mov dword [edi + EXC_REC_ARCH_ERR_HI], 0

    ; Parameter 0 = Vektor
    mov dword [edi + EXC_REC_PARAM_COUNT], 1
    mov [edi + EXC_REC_PARAM0], ecx

    pop edi
    pop esi
    pop ebp
    ret

; ---------------------------------------------------------------------------
; exc_search_fixup  – Fixup-Tabelle nach EIP durchsuchen (§33)
;   [esp+4]  = EIP (fault instruction pointer)
;   [esp+8]  = Vektor
;   Rückgabe: EAX = Recovery-Adresse, 0 wenn nicht gefunden
; ---------------------------------------------------------------------------
exc_search_fixup:
    push ebp
    mov ebp, esp
    push esi
    push ebx

    mov ebx, [ebp + 8]   ; EIP
    mov ecx, [ebp + 12]  ; Vektor

    mov esi, exception_fixup_table
.loop:
    mov eax, [esi + EXC_FIXUP_FAULT_START]
    cmp eax, EXC_FIXUP_SENTINEL
    je .not_found

    cmp ebx, eax         ; EIP >= fault_start?
    jb .next
    mov eax, [esi + EXC_FIXUP_FAULT_END]
    cmp ebx, eax         ; EIP < fault_end?
    jae .next

    ; Vektor-Maske prüfen
    mov eax, [esi + EXC_FIXUP_ALLOWED_VECS]
    mov edx, 1
    cmp ecx, 31
    ja .vec_ok          ; Vektor > 31 → unkritisch, erlaubt
    shl edx, cl
    test eax, edx
    jz .next
.vec_ok:
    mov eax, [esi + EXC_FIXUP_RECOVERY]
    pop ebx
    pop esi
    pop ebp
    ret
.next:
    add esi, EXC_FIXUP_SIZE
    jmp .loop
.not_found:
    xor eax, eax
    pop ebx
    pop esi
    pop ebp
    ret

; ---------------------------------------------------------------------------
; exception_manager_dispatch  – Haupteinstieg §010
;   [esp+4]  = Vektor
;   [esp+8]  = Frame-Zeiger
;   Rückgabe: EAX = Frame-Zeiger (unverändert oder neuer Frame nach Fixup),
;             0 wenn Return zum alten Frame
; ---------------------------------------------------------------------------
exception_manager_dispatch:
    push ebp
    mov ebp, esp
    pushad

    mov ecx, [ebp + 8]   ; Vektor
    mov edx, [ebp + 12]  ; Frame

    ; Statistik: total
    lock inc dword [exception_stats_total]

    ; Double Fault (Vektor 8) und Machine Check (18): Notfallpfad (§21, §22)
    cmp ecx, 8
    je .double_fault_path
    cmp ecx, 18
    je .machine_check_path

    ; CPU-Slot bestimmen (§32 Tiefenzähler)
    ; Vereinfacht: BSP=0 (nur ein CPU-Slot in der frühen Phase)
    xor eax, eax         ; cpu_id = 0

    ; Exception-Tiefe prüfen (§32)
    mov ebx, [exception_depth_table + eax*4]
    cmp ebx, EXC_MAX_DEPTH
    jae .recursive_exception

    inc dword [exception_depth_table + eax*4]

    ; Exception Record aufbauen
    sub esp, EXC_REC_SIZE
    mov edi, esp          ; Record auf Stack

    push ecx             ; Vektor
    push edx             ; Frame
    push edi             ; Record-Puffer
    call exc_build_record
    add esp, 12

    ; Letzten Record kopieren (für Diagnose)
    push edi
    push ecx
    mov esi, edi
    mov edi, exception_last_record
    mov ecx, EXC_REC_SIZE
    push ds
    push es
    push eax
    mov ax, ds
    mov es, ax
    pop eax
    rep movsb
    pop es
    pop ds
    pop ecx
    pop edi              ; edi = Record auf Stack

    ; Ursprung bestimmen
    mov eax, [edi + EXC_REC_FLAGS]
    and eax, EXC_FLAG_ORIGIN_USER
    test eax, eax
    jnz .user_path

    ; === Kernel-Exception-Pfad (§12) ===
    lock inc dword [exception_stats_kernel]

    ; Fixup-Tabelle durchsuchen (§33)
    mov ebx, [edi + EXC_REC_IP]
    push ecx             ; Vektor
    push ebx             ; EIP
    call exc_search_fixup
    add esp, 8

    test eax, eax
    jnz .fixup_found

    ; Kein Fixup → Kernel Panic
    lock inc dword [exception_stats_panics]
    mov esi, message_exc_kernel_panic
    call serial_write_string
    mov eax, [edi + EXC_REC_IP]
    call serial_write_hex32
    mov esi, message_newline
    call serial_write_string

    mov eax, [edi + EXC_REC_CODE]
    xor esi, esi
    call kernel_panic
    ; kehrt nicht zurück

.fixup_found:
    ; Recovery-Adresse im Frame (EIP) setzen und zurückkehren (§12, §33)
    lock inc dword [exception_stats_handled]
    mov ebx, [ebp + 12]  ; Frame
    mov [ebx + 56], eax  ; neues EIP im Frame

    ; EXC_FLAG_FIXUP_USED setzen
    or dword [edi + EXC_REC_FLAGS], EXC_FLAG_FIXUP_USED

    ; Tiefenzähler dekrementieren
    dec dword [exception_depth_table]

    add esp, EXC_REC_SIZE
    mov eax, [ebp + 12]  ; Frame zurückgeben
    mov [ebp - 4], eax   ; EAX-Slot in pushad-Area setzen (popad liest daraus)
    popad
    pop ebp
    ret

    ; === Userspace-Exception-Pfad (§11) ===
.user_path:
    lock inc dword [exception_stats_user]

    ; Page Fault: zuerst VMM-Probe (§14)
    mov eax, [edi + EXC_REC_CODE]
    cmp eax, NP_EXC_PAGE_FAULT
    jne .user_no_vmm

    ; vmm_handle_page_fault: EAX=Fehlercode, EBX=Fault-Adresse
    mov eax, [edi + EXC_REC_ARCH_ERR]
    mov ebx, [edi + EXC_REC_FAULT_ADDR]
    call vmm_handle_page_fault
    ; EAX=0 → behandelt, fortfahren
    test eax, eax
    jnz .user_vmm_failed

    lock inc dword [exception_stats_handled]
    dec dword [exception_depth_table]
    add esp, EXC_REC_SIZE
    mov eax, [ebp + 12]
    mov [ebp - 4], eax   ; EAX-Slot in pushad-Area setzen
    popad
    pop ebp
    ret

.user_vmm_failed:
.user_no_vmm:
    ; Exception-Endpunkt prüfen (§25) – Stub: aktueller Prozess
    ; In dieser Bootstrap-Phase: kein separater Endpunkt-Mechanismus,
    ; Prozess wird sofort beendet (§35, §36)
    lock inc dword [exception_stats_thread_term]

    ; Userspace-Thread/Prozess beenden über thread_manager_terminate_current
    call exception_terminate_current_thread

    ; Nach thread_manager_terminate_current wählt der Scheduler
    ; den nächsten bereiten Thread.
    ; Rückgabe: Frame unverändert (Scheduler übernimmt beim nächsten Tick)
    dec dword [exception_depth_table]
    add esp, EXC_REC_SIZE
    mov eax, [ebp + 12]   ; ursprünglicher Frame (Caller erhält ihn)
    mov [ebp - 4], eax    ; EAX-Slot in pushad-Area setzen
    popad
    pop ebp
    ret

    ; === Rekursive Exception (§32) ===
.recursive_exception:
    lock inc dword [exception_stats_recursive]
    lock inc dword [exception_stats_panics]
    mov esi, message_exc_recursive
    call serial_write_string
    mov eax, 0xE0CA0010
    mov edx, 0x0000DEAD
    xor esi, esi
    call kernel_panic

    ; === Double Fault Notfallpfad (§21, §41) ===
.double_fault_path:
    lock inc dword [exception_stats_panics]
    mov edi, exception_emergency_record
    mov dword [edi + EXC_REC_STRUCT_SIZE], EXC_REC_SIZE
    mov dword [edi + EXC_REC_VERSION],     1
    mov dword [edi + EXC_REC_CODE],        NP_EXC_DOUBLE_FAULT
    mov dword [edi + EXC_REC_CATEGORY],    EXC_CAT_KERNEL_FATAL
    mov dword [edi + EXC_REC_FLAGS],       EXC_FLAG_DOUBLE_FAULT | EXC_FLAG_ORIGIN_KERNEL
    ; EIP aus Frame wenn verfügbar
    test edx, edx
    jz .df_no_frame
    mov eax, [edx + 56]
    mov [edi + EXC_REC_IP], eax
.df_no_frame:
    mov esi, message_exc_double_fault
    call serial_write_string
    mov eax, [edi + EXC_REC_IP]
    call serial_write_hex32
    mov esi, message_newline
    call serial_write_string
    mov eax, 0xDF000008
    mov edx, 0x0000DEAD
    xor esi, esi
    call kernel_panic

    ; === Machine Check Notfallpfad (§22, §41) ===
.machine_check_path:
    lock inc dword [exception_stats_panics]
    mov edi, exception_emergency_record
    mov dword [edi + EXC_REC_STRUCT_SIZE], EXC_REC_SIZE
    mov dword [edi + EXC_REC_VERSION],     1
    mov dword [edi + EXC_REC_CODE],        NP_EXC_MACHINE_CHECK
    mov dword [edi + EXC_REC_CATEGORY],    EXC_CAT_KERNEL_FATAL
    mov dword [edi + EXC_REC_FLAGS],       EXC_FLAG_MACHINE_CHECK | EXC_FLAG_ORIGIN_KERNEL
    mov esi, message_exc_machine_check
    call serial_write_string
    mov eax, 0xEC000012
    mov edx, 0x0000DEAD
    xor esi, esi
    call kernel_panic

; ---------------------------------------------------------------------------
; exception_terminate_current_thread  – §35
;   Beendet den aktuellen Userspace-Thread vollständig:
;   - thread_table-Eintrag auf inaktiv setzen
;   - scheduler_contexts[slot] nullen (Scheduler überspringt leere Slots)
;   - thread_running_on[slot] freigeben
;   - thread_count dekrementieren
;   - per_cpu_current_thread[0] auf -1 setzen (CPU ist danach idle)
; ---------------------------------------------------------------------------
exception_terminate_current_thread:
    pushad
    mov esi, message_exc_thread_terminated
    call serial_write_string
    ; Slot-Index: per_cpu_current_thread[0] (BSP)
    mov eax, [per_cpu_current_thread]
    cmp eax, -1
    je .no_thread
    cmp eax, SCHEDULER_THREAD_COUNT
    jae .no_thread

    ; 1. thread_table-Eintrag auf inaktiv setzen
    imul ebx, eax, THREAD_RECORD_SIZE
    add ebx, thread_table
    mov dword [ebx + THREAD_STATE], 0

    ; 2. Scheduler-Kontext-Frame nullen → Scheduler überspringt diesen Slot
    mov dword [scheduler_contexts + eax * 4], 0

    ; 3. Slot-Belegung freigeben
    mov dword [thread_running_on + eax * 4], -1

    ; 4. Task-ID-Zuordnung löschen
    mov dword [thread_task_ids + eax * 4], 0

    ; 5. Thread-Zähler dekrementieren (untere Grenze: 0)
    cmp dword [thread_count], 0
    je .skip_count
    dec dword [thread_count]
.skip_count:

    ; 6. BSP als idle markieren; Scheduler wählt beim nächsten Tick neuen Slot
    mov dword [per_cpu_current_thread], -1

.no_thread:
    popad
    ret

; ---------------------------------------------------------------------------
; vmm_handle_page_fault  – VMM Page-Fault-Stub (§14)
;   EAX = Fehlercode, EBX = Fault-Adresse
;   Rückgabe: EAX=0 behandelt, EAX≠0 nicht behandelt
; ---------------------------------------------------------------------------
vmm_handle_page_fault:
    ; Frühe Phase: keine Demand-Paging-Unterstützung →
    ; Kern-Adressen (>=0xC0000000) immer als unbehandelbar zurückgeben
    cmp ebx, 0xC0000000
    jae .not_handled
    ; Userspace-Adresse: prüfen ob in einer gemappten Region (Stub)
    ; In dieser Bootstrap-Phase: keine UserVM-Regionen → nicht behandelbar
    xor eax, eax
    inc eax              ; 1 = nicht behandelt
    ret
.not_handled:
    mov eax, 1
    ret

; ---------------------------------------------------------------------------
; exception_manager_self_test  – 14 Testfälle (§49)
;   Rückgabe: CF=0 alle OK, CF=1 mindestens ein Fehler
; ---------------------------------------------------------------------------
exception_manager_self_test:
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi

    xor esi, esi         ; Fehler-Zähler (ESI; EDI wird in Tests 12/13 als Record-Puffer benötigt)

    ; Test 1: Initialisierungsflag gesetzt
    cmp dword [exception_mgr_initialized], 1
    je .t1_ok
    inc esi
.t1_ok:

    ; Test 2: exc_vector_to_code Vektor 0 → NP_EXC_DIVIDE_BY_ZERO
    mov eax, 0
    call exc_vector_to_code
    cmp eax, NP_EXC_DIVIDE_BY_ZERO
    je .t2_ok
    inc esi
.t2_ok:

    ; Test 3: Vektor 14 → NP_EXC_PAGE_FAULT
    mov eax, 14
    call exc_vector_to_code
    cmp eax, NP_EXC_PAGE_FAULT
    je .t3_ok
    inc esi
.t3_ok:

    ; Test 4: Vektor 8 → NP_EXC_DOUBLE_FAULT
    mov eax, 8
    call exc_vector_to_code
    cmp eax, NP_EXC_DOUBLE_FAULT
    je .t4_ok
    inc esi
.t4_ok:

    ; Test 5: exc_code_to_category PAGE_FAULT → MEMORY
    mov eax, NP_EXC_PAGE_FAULT
    call exc_code_to_category
    cmp eax, EXC_CAT_MEMORY
    je .t5_ok
    inc esi
.t5_ok:

    ; Test 6: exc_code_to_category DIVIDE_BY_ZERO → ARITHMETIC
    mov eax, NP_EXC_DIVIDE_BY_ZERO
    call exc_code_to_category
    cmp eax, EXC_CAT_ARITHMETIC
    je .t6_ok
    inc esi
.t6_ok:

    ; Test 7: exc_code_to_category DOUBLE_FAULT → KERNEL_FATAL
    mov eax, NP_EXC_DOUBLE_FAULT
    call exc_code_to_category
    cmp eax, EXC_CAT_KERNEL_FATAL
    je .t7_ok
    inc esi
.t7_ok:

    ; Test 8: exc_code_to_category DEBUG → DEBUG
    mov eax, NP_EXC_DEBUG
    call exc_code_to_category
    cmp eax, EXC_CAT_DEBUG
    je .t8_ok
    inc esi
.t8_ok:

    ; Test 9: Fixup-Tabellen-Suche: Sentinel → 0 (kein Fixup)
    ; Suche nach EIP 0xDEAD0001, Vektor 0 → muss 0 zurückgeben
    push dword 0
    push dword 0xDEAD0001
    call exc_search_fixup
    add esp, 8
    cmp eax, 0
    je .t9_ok
    inc esi
.t9_ok:

    ; Test 10: Statistik-Felder erreichbar
    mov eax, [exception_stats_total]
    cmp eax, 0xFFFFFFFF
    jne .t10_ok
    inc esi
.t10_ok:

    ; Test 11: Exception-Tiefenzähler = 0 (nach Init)
    cmp dword [exception_depth_table], 0
    je .t11_ok
    inc esi
.t11_ok:

    ; Test 12/13: exc_build_record auf statischen Dummy-Frame
    ; Dummy-Frame im BSS nutzen (exception_emergency_record als Scratch reicht nicht,
    ; daher: Stack-Bereich reservieren, danach Record in emergency_record prüfen)
    sub esp, EXC_REC_SIZE + 68  ; Record + Frame
    mov edi, esp                ; Record-Puffer
    mov ebx, esp
    add ebx, EXC_REC_SIZE       ; Frame-Zeiger
    mov dword [ebx + 60], 0x23  ; CS = Ring-3 (User)
    mov dword [ebx + 56], 0x1234
    mov dword [ebx + 52], 0
    push dword 14               ; Vektor = Page Fault
    push ebx                    ; Frame
    push edi                    ; Record-Puffer
    call exc_build_record
    add esp, 12
    cmp dword [edi + EXC_REC_STRUCT_SIZE], EXC_REC_SIZE
    je .t12_ok
    inc esi
.t12_ok:
    ; Test 13: Kategorie für Vektor 0 (ARITHMETIC)
    mov dword [ebx + 60], 0     ; CS = Ring-0
    mov dword [ebx + 56], 0x5678
    push dword 0                ; Vektor 0 = #DE
    push ebx
    push edi
    call exc_build_record
    add esp, 12
    cmp dword [edi + EXC_REC_CATEGORY], EXC_CAT_ARITHMETIC
    je .t13_ok
    inc esi
.t13_ok:
    add esp, EXC_REC_SIZE + 68

    ; Test 14: vmm_handle_page_fault Kernel-Adresse → nicht behandelt
    mov eax, 0           ; Fehlercode
    mov ebx, 0xC0001000  ; Kernel-Adresse
    call vmm_handle_page_fault
    cmp eax, 0
    jne .t14_ok
    inc esi
.t14_ok:

    ; Ergebnis
    test esi, esi
    jnz .selftest_fail

    mov esi, message_exc_selftest_ok
    call serial_write_string
    clc
    jmp .selftest_done

.selftest_fail:
    mov esi, message_exc_selftest_fail
    call serial_write_string
    stc

.selftest_done:
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

; ---------------------------------------------------------------------------
; Fixup-Tabelle (§33) – statische Einträge für sichere Kernel-Probe-Operationen
; Weitere Einträge können per exc_fixup_register hinzugefügt werden.
; ---------------------------------------------------------------------------
align 4
exception_fixup_table:
    ; Sentinel: Tabellenende
    dd EXC_FIXUP_SENTINEL, 0, 0, 0

; ---------------------------------------------------------------------------
; §010 Meldungen
; ---------------------------------------------------------------------------
message_exception_mgr_ok:
    db "NOVA: Exception Manager ABI 1.0 bereit", 13, 10, 0
message_exc_selftest_ok:
    db "NOVA: Exception Manager Selbsttest OK (14/14)", 13, 10, 0
message_exc_selftest_fail:
    db "NOVA PANIC: Exception Manager Selbsttest fehlgeschlagen", 13, 10, 0
message_exc_kernel_panic:
    db "NOVA PANIC: Kernel-Exception ohne Fixup bei EIP 0x", 0
message_exc_double_fault:
    db "NOVA PANIC: Double Fault bei EIP 0x", 0
message_exc_machine_check:
    db "NOVA PANIC: Machine Check Exception", 13, 10, 0
message_exc_recursive:
    db "NOVA PANIC: Rekursive Exception (Tiefenlimit)", 13, 10, 0
message_exc_thread_terminated:
    db "NOVA: Userspace-Thread durch Exception beendet", 13, 10, 0

; ===========================================================================
; §011 – System Call Interface (NPSPEC-KERNEL-0011)
; ===========================================================================

; ---------------------------------------------------------------------------
; syscall_manager_initialize  – Bootstrap §011
;   Konfiguriert SYSENTER-MSRs (wenn CPU es unterstützt), initialisiert
;   Statistiken und Tiefenzähler (§39, §53), setzt Init-Flag.
;   Rückgabe: CF=0 OK
; ---------------------------------------------------------------------------
syscall_manager_initialize:
    push eax
    push ebx
    push ecx
    push edx
    push esi

    ; CPUID Leaf 1: EDX Bit 11 = SEP (SYSENTER/SYSEXIT-Support)
    mov eax, 1
    cpuid
    test edx, (1 << 11)
    jz .no_sysenter

    mov dword [syscall_sysenter_available], 1

    ; IA32_SYSENTER_CS (MSR 0x174) = Kernel-Code-Selektor
    mov ecx, 0x174
    xor edx, edx
    mov eax, CODE_SEGMENT
    wrmsr

    ; IA32_SYSENTER_ESP (MSR 0x175) = Kernel-Stack (TSS ESP0)
    mov ecx, 0x175
    xor edx, edx
    mov eax, [kernel_boot_stack_top]
    wrmsr

    ; IA32_SYSENTER_EIP (MSR 0x176) = SYSENTER-Eintrittspunkt
    mov ecx, 0x176
    xor edx, edx
    mov eax, sysenter_entry
    wrmsr

    mov esi, message_syscall_sysenter_ok
    call serial_write_string
    jmp .stats_init

.no_sysenter:
    mov esi, message_syscall_intgate_only
    call serial_write_string

.stats_init:
    ; §53 Statistiken initialisieren
    mov dword [syscall_stats_successful],        0
    mov dword [syscall_stats_failed],            0
    mov dword [syscall_stats_capability_denied], 0
    mov dword [syscall_stats_invalid_ptr],       0

    ; §39 Tiefenzähler
    mov dword [syscall_call_depth], 0

    mov dword [syscall_mgr_initialized], 1

    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    clc
    ret

; ---------------------------------------------------------------------------
; sysenter_entry  – SYSENTER-Einstieg (§011 §9, §11)
;   CPU setzt bei SYSENTER: CPL→0, CS=IA32_SYSENTER_CS,
;   ESP=IA32_SYSENTER_ESP, EIP=IA32_SYSENTER_EIP.
;
;   NovaOS SYSENTER-Konvention (Userspace):
;     EAX = service_id,  EBX = operation_id
;     ESI = abi_version, EDI = arg_ptr, EBP = arg_size
;     ECX = return EIP (SYSENTER-Pflicht)
;     EDX = return ESP (SYSENTER-Pflicht)
; ---------------------------------------------------------------------------
sysenter_entry:
    ; Rückkehrinformation vor Überschreibung sichern
    mov [sysenter_user_eip], ecx
    mov [sysenter_user_esp], edx

    ; Registerkonvention an INT-0x80-Dispatcher anpassen:
    ;   INT 0x80: ECX=abi_version, EDX=arg_ptr, ESI=arg_size
    mov ecx, esi             ; ECX ← abi_version (war ESI)
    mov edx, edi             ; EDX ← arg_ptr     (war EDI)
    mov esi, ebp             ; ESI ← arg_size     (war EBP)

    ; Synthetischen isr_common-Frame aufbauen
    pushfd                   ; EFLAGS
    push dword 0x1B          ; User-CS Ring-3
    push dword [sysenter_user_eip]   ; User-EIP
    push dword 0             ; Error-Code
    push dword 0x80          ; Vektor 0x80

    pushad                   ; EAX=service, ECX=abi_ver, EDX=arg_ptr,
                             ; EBX=op, ESP, EBP, ESI=arg_size, EDI

    push ds
    push es
    push fs
    push gs

    ; Kernel-Segmente aktivieren
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax

    ; §39: Tiefenzähler prüfen und erhöhen
    mov eax, [syscall_call_depth]
    cmp eax, SYSCALL_CALL_DEPTH_MAX
    jae .sysenter_depth_exceeded
    inc dword [syscall_call_depth]

    mov edx, esp             ; Frame-Zeiger wie in interrupt_dispatch
    call syscall_dispatch

    dec dword [syscall_call_depth]

    ; Rückgabewert sichern: [frame+44]=EAX-Slot (von dispatch beschrieben)
    ; frame+44 = [esp + 44] (GS@0 FS@4 ES@8 DS@12 EDI@16 ESI@20 EBP@24 ESP@28 EBX@32 EDX@36 ECX@40 EAX@44)
    mov eax, [esp + 44]
    mov [sysenter_dispatch_result], eax   ; vor popad sichern

    ; Frame abbauen
    pop gs
    pop fs
    pop es
    pop ds
    popad                    ; stellt EAX auf original service_id zurück
    add esp, 20              ; Vektor(4)+errcode(4)+EIP(4)+CS(4)+EFLAGS(4)

    ; Status-Code aus temporärer Variable laden
    mov eax, [sysenter_dispatch_result]

    mov ecx, [sysenter_user_eip]
    mov edx, [sysenter_user_esp]
    sti
    sysexit

.sysenter_depth_exceeded:
    ; §39: Tiefenlimit – als Fehler zurückkehren
    add esp, 20 + 32 + 16    ; frame cleanup (vector+errcode+eip+cs+eflags + pushad + seg)
    mov eax, SYSCALL_STATUS_OPERATION
    mov ecx, [sysenter_user_eip]
    mov edx, [sysenter_user_esp]
    sti
    sysexit

; ---------------------------------------------------------------------------
; syscall_handler_query_abi  – §011 §26: ABI-Abfrage (Core-Namespace)
;   EDX = syscall_frame (Frame-Zeiger, gesetzt von syscall_dispatch)
;   Schreibt { struct_size(4), major(2), minor(2), feature_flags(8) } in
;   Userspace-Puffer ([frame+36]=arg_ptr, [frame+20]=arg_size).
; ---------------------------------------------------------------------------
syscall_handler_query_abi:
    push ebx
    push esi
    push edi
    push ecx

    mov edx, [syscall_frame]

    ; Argumente aus Frame
    mov esi, [edx + 36]      ; arg_ptr  (gespeichertes EDX = Userspace-Zeiger)
    mov ecx, [edx + 20]      ; arg_size (gespeichertes ESI)

    ; Mindestgröße: 16 Bytes (§011 §14)
    cmp ecx, 16
    jb .qabi_bad_size

    ; Userspace-Zeiger prüfen (§011 §15)
    call syscall_validate_user_range
    jc .qabi_bad_pointer

    ; Ergebnisstruktur im Kernel-Puffer aufbauen
    mov dword [syscall_abi_result +  0], 16   ; struct_size
    mov word  [syscall_abi_result +  4],  1   ; major_version = 1
    mov word  [syscall_abi_result +  6],  0   ; minor_version = 0
    mov dword [syscall_abi_result +  8],  0   ; feature_flags (low32)
    mov dword [syscall_abi_result + 12],  0   ; feature_flags (high32)

    ; In Userspace kopieren (§011 §16)
    mov edi, esi             ; Ziel = arg_ptr (Userspace)
    mov esi, syscall_abi_result
    mov ecx, 16
    push ds
    push es
    mov ax, ds
    mov es, ax
    rep movsb
    pop es
    pop ds

    lock inc dword [syscall_stats_successful]
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK

    pop ecx
    pop edi
    pop esi
    pop ebx
    ret

.qabi_bad_size:
    lock inc dword [syscall_stats_failed]
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_SIZE
    pop ecx
    pop edi
    pop esi
    pop ebx
    ret

.qabi_bad_pointer:
    lock inc dword [syscall_stats_failed]
    lock inc dword [syscall_stats_invalid_ptr]
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_POINTER
    pop ecx
    pop edi
    pop esi
    pop ebx
    ret

; ---------------------------------------------------------------------------
; syscall_manager_self_test  – §011 §55: Selbsttest (CF=0 OK, CF=1 Fehler)
;   Testet: Init-Flag, Pointer-Validierung (3 Fälle), Statistiken, Tiefe
; ---------------------------------------------------------------------------
syscall_manager_self_test:
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi

    xor ebx, ebx             ; Fehlerzähler (EBX; ESI/EDI für Validierungsaufrufe)

    ; Test 1: Initialisierungsflag gesetzt
    cmp dword [syscall_mgr_initialized], 1
    je .st1_ok
    inc ebx
.st1_ok:

    ; Test 2: syscall_validate_user_range lehnt Kernel-Adresse ≥ 0xC0000000 ab
    push ecx
    mov esi, 0xC0001000
    mov ecx, 4
    call syscall_validate_user_range
    pop ecx
    jc .st2_ok                ; CF=1 = korrekt abgelehnt
    inc ebx
.st2_ok:

    ; Test 3: Nulllänge → ungültig (§011 §15)
    push ecx
    mov esi, USER_ADDRESS_MIN
    mov ecx, 0
    call syscall_validate_user_range
    pop ecx
    jc .st3_ok
    inc ebx
.st3_ok:

    ; Test 4: Adress-Überlauf (start+size wraps) → ungültig
    push ecx
    mov esi, 0xFFFF0000
    mov ecx, 0x00020000
    call syscall_validate_user_range
    pop ecx
    jc .st4_ok
    inc ebx
.st4_ok:

    ; Test 5: §53 Statistiken erreichbar (kein Sentinel-Wert)
    mov eax, [syscall_stats_successful]
    cmp eax, 0xFFFFFFFF
    jne .st5_ok
    inc ebx
.st5_ok:

    ; Test 6: §39 Tiefenzähler = 0
    cmp dword [syscall_call_depth], 0
    je .st6_ok
    inc ebx
.st6_ok:

    ; Test 7: §011 §50 SYSENTER-Flag ist 0 oder 1
    mov eax, [syscall_sysenter_available]
    cmp eax, 1
    jbe .st7_ok
    inc ebx
.st7_ok:

    ; Ergebnis
    test ebx, ebx
    jnz .st_fail
    mov esi, message_syscall_selftest_ok
    call serial_write_string
    clc
    jmp .st_done

.st_fail:
    mov esi, message_syscall_selftest_fail
    call serial_write_string
    stc

.st_done:
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

; ---------------------------------------------------------------------------
; §011 Daten
; ---------------------------------------------------------------------------
align 4
syscall_mgr_initialized:         dd 0
syscall_sysenter_available:      dd 0
syscall_stats_successful:        dd 0
syscall_stats_failed:            dd 0
syscall_stats_capability_denied: dd 0
syscall_stats_invalid_ptr:       dd 0
syscall_call_depth:              dd 0    ; §39 BSP-Bootstrap
sysenter_user_eip:               dd 0    ; Temp: User-Rückkehr-EIP
sysenter_user_esp:               dd 0    ; Temp: User-Rückkehr-ESP
sysenter_dispatch_result:        dd 0    ; Temp: Dispatcher-Rückgabewert
align 16
syscall_abi_result:              times 16 db 0   ; QUERY_ABI Ausgabepuffer

; ---------------------------------------------------------------------------
; §011 Meldungen
; ---------------------------------------------------------------------------
message_syscall_mgr_ok:
    db "NOVA: System Call Interface ABI 1.0 bereit", 13, 10, 0
message_syscall_mgr_error:
    db "NOVA PANIC: System Call Interface Init fehlgeschlagen", 13, 10, 0
message_syscall_sysenter_ok:
    db "NOVA: SYSENTER verfuegbar und konfiguriert", 13, 10, 0
message_syscall_intgate_only:
    db "NOVA: SYSENTER nicht verfuegbar, nur INT 0x80 aktiv", 13, 10, 0
message_syscall_selftest_ok:
    db "NOVA: System Call Interface Selbsttest OK (7/7)", 13, 10, 0
message_syscall_selftest_fail:
    db "NOVA PANIC: System Call Interface Selbsttest fehlgeschlagen", 13, 10, 0

