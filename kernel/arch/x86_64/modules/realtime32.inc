; ---------------------------------------------------------------------------
; Nova Realtime-Subsystem §032
; NPSPEC-REALTIME-BUDGET-0001     - Zeitbudget-Verwaltung
; NPSPEC-REALTIME-DEADLINE-0001   - Deadline-Scheduling
; NPSPEC-REALTIME-DETERMINISM-0001- Deterministische Ausfuehrung
; NPSPEC-REALTIME-ISOLATION-0001  - CPU-Isolation
; NPSPEC-REALTIME-JITTER-0001     - Jitter-Messung
; NPSPEC-REALTIME-LATENCY-0001    - Latenz-Garantien
; NPSPEC-REALTIME-PRIORITY-0001   - RT-Prioritaeten
; NPSPEC-REALTIME-TIMER-0001      - Hochaufloesung-Timer
; NPSPEC-REALTIME-WCET-0001       - Worst-Case Execution Time
; ---------------------------------------------------------------------------

; RT-Prioritaeten (NPSPEC-REALTIME-PRIORITY-0001)
RT_PRIO_MAX           equ 99   ; Hoechste RT-Prioritaet
RT_PRIO_HIGH          equ 80
RT_PRIO_NORMAL        equ 50
RT_PRIO_LOW           equ 20
RT_PRIO_BEST_EFFORT   equ 0    ; Kein RT

; Scheduling-Klassen (NPSPEC-REALTIME-DEADLINE-0001)
RT_SCHED_FIFO         equ 0    ; First-In-First-Out
RT_SCHED_RR           equ 1    ; Round-Robin
RT_SCHED_DEADLINE     equ 2    ; EDF/CBS
RT_SCHED_IDLE         equ 3

; Latenz-Klassen (NPSPEC-REALTIME-LATENCY-0001)
RT_LATENCY_HARD       equ 0    ; Harte Echtzeit (kein Miss erlaubt)
RT_LATENCY_SOFT       equ 1    ; Weiche Echtzeit
RT_LATENCY_BEST_EFF   equ 2

; RT-Task-Record (48 Bytes)
RT_TASK_SIZE          equ 48
RK_TASK_ID            equ 0
RK_PRIORITY           equ 4    ; RT_PRIO_*
RK_SCHED_CLASS        equ 8    ; RT_SCHED_*
RK_LATENCY_CLASS      equ 12   ; RT_LATENCY_*
RK_PERIOD_US          equ 16   ; Periode in Mikrosekunden (NPSPEC-REALTIME-TIMER-0001)
RK_DEADLINE_US        equ 20   ; Deadline in Mikrosekunden
RK_WCET_US            equ 24   ; WCET in Mikrosekunden (NPSPEC-REALTIME-WCET-0001)
RK_BUDGET_US          equ 28   ; Zeitbudget (NPSPEC-REALTIME-BUDGET-0001)
RK_JITTER_MAX_US      equ 32   ; Max. gemessener Jitter (NPSPEC-REALTIME-JITTER-0001)
RK_MISS_COUNT         equ 36   ; Deadline-Misses
RK_EXEC_COUNT         equ 40   ; Ausfuehrungen
RK_FLAGS              equ 44

; RT-Flags
RT_FLAG_ISOLATED      equ 0x01  ; (NPSPEC-REALTIME-ISOLATION-0001)
RT_FLAG_DETERMINISTIC equ 0x02  ; (NPSPEC-REALTIME-DETERMINISM-0001)
RT_FLAG_ACTIVE        equ 0x04

RT_TASK_CAPACITY      equ 8

; ---------------------------------------------------------------------------
; realtime_initialize
; Registriert den Kernel-Idle-Task als RT-Referenz.
; CF=0 Erfolg
; ---------------------------------------------------------------------------
realtime_initialize:
    push edi
    mov edi, rt_task_table
    xor eax, eax
    mov ecx, (RT_TASK_CAPACITY * RT_TASK_SIZE) / 4
    rep stosd
    pop edi
    mov dword [rt_task_count], 0
    mov dword [rt_jitter_global_us], 0
    mov dword [rt_deadline_miss_total], 0

    ; Idle-Referenz-Task
    mov eax, 0
    mov ebx, RT_PRIO_BEST_EFFORT
    mov ecx, RT_SCHED_IDLE
    mov edx, RT_LATENCY_BEST_EFF
    call rtmgr_task_register
    jc .fail

    mov dword [rtmgr_ready], 1
    clc
    ret
.fail:
    stc
    ret

; ---------------------------------------------------------------------------
; rtmgr_task_register: EAX=task_id, EBX=priority, ECX=sched_class, EDX=latency
; CF=0 Erfolg
; ---------------------------------------------------------------------------
rtmgr_task_register:
    cmp dword [rt_task_count], RT_TASK_CAPACITY
    jae .full
    push edi
    push esi
    mov edi, rt_task_table
    push eax
    mov eax, [rt_task_count]
    imul eax, RT_TASK_SIZE
    add edi, eax
    pop eax
    mov [edi + RK_TASK_ID], eax
    mov [edi + RK_PRIORITY], ebx
    mov [edi + RK_SCHED_CLASS], ecx
    mov [edi + RK_LATENCY_CLASS], edx
    mov dword [edi + RK_PERIOD_US], 10000    ; 10ms Default
    mov dword [edi + RK_DEADLINE_US], 10000
    mov dword [edi + RK_WCET_US], 1000       ; 1ms WCET
    mov dword [edi + RK_BUDGET_US], 5000     ; 5ms Budget
    mov dword [edi + RK_JITTER_MAX_US], 0
    mov dword [edi + RK_MISS_COUNT], 0
    mov dword [edi + RK_EXEC_COUNT], 0
    mov dword [edi + RK_FLAGS], RT_FLAG_ACTIVE
    inc dword [rt_task_count]
    pop esi
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; rt_set_params: EAX=task_id, EBX=period_us, ECX=deadline_us, EDX=wcet_us
; (NPSPEC-REALTIME-DEADLINE-0001, NPSPEC-REALTIME-WCET-0001)
; ---------------------------------------------------------------------------
rt_set_params:
    push esi
    push edi
    mov esi, rt_task_table
    push ecx
    mov ecx, [rt_task_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + RK_TASK_ID], eax
    je .found
    add esi, RT_TASK_SIZE
    dec ecx
    jmp .scan
.found:
    pop ecx
    mov [esi + RK_PERIOD_US], ebx
    mov [esi + RK_DEADLINE_US], ecx
    mov [esi + RK_WCET_US], edx
    pop edi
    pop esi
    clc
    ret
.not_found:
    pop ecx
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; rt_record_jitter: EAX=task_id, EBX=jitter_us
; (NPSPEC-REALTIME-JITTER-0001)
; ---------------------------------------------------------------------------
rt_record_jitter:
    push esi
    push ecx
    mov esi, rt_task_table
    mov ecx, [rt_task_count]
.scan:
    test ecx, ecx
    jz .done
    cmp [esi + RK_TASK_ID], eax
    je .found
    add esi, RT_TASK_SIZE
    dec ecx
    jmp .scan
.found:
    cmp [esi + RK_JITTER_MAX_US], ebx
    jge .update_global
    mov [esi + RK_JITTER_MAX_US], ebx
.update_global:
    cmp [rt_jitter_global_us], ebx
    jge .done
    mov [rt_jitter_global_us], ebx
.done:
    pop ecx
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; rt_deadline_check: EAX=task_id -> CF=0 ok (innerhalb Deadline)
; (NPSPEC-REALTIME-DEADLINE-0001)
; ---------------------------------------------------------------------------
rt_deadline_check:
    push esi
    push ecx
    mov esi, rt_task_table
    mov ecx, [rt_task_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + RK_TASK_ID], eax
    je .found
    add esi, RT_TASK_SIZE
    dec ecx
    jmp .scan
.found:
    inc dword [esi + RK_EXEC_COUNT]
    ; Vereinfacht: Jitter-Check gegen halbe Deadline
    push eax
    mov eax, [esi + RK_JITTER_MAX_US]
    push edx
    mov edx, [esi + RK_DEADLINE_US]
    shr edx, 1
    cmp eax, edx
    pop edx
    pop eax
    jg .miss
    pop ecx
    pop esi
    clc
    ret
.miss:
    inc dword [esi + RK_MISS_COUNT]
    inc dword [rt_deadline_miss_total]
    pop ecx
    pop esi
    ; Bei HARD-RT: Fehler
    cmp dword [esi + RK_LATENCY_CLASS], RT_LATENCY_HARD
    je .hard_fail
    clc
    ret
.hard_fail:
    stc
    ret
.not_found:
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; realtime_self_test
; ---------------------------------------------------------------------------
realtime_self_test:
    push esi
    xor esi, esi
    cmp dword [rtmgr_ready], 1
    je .t2
    inc esi
.t2:
    mov eax, 1
    mov ebx, RT_PRIO_HIGH
    mov ecx, RT_SCHED_DEADLINE
    mov edx, RT_LATENCY_SOFT
    call rtmgr_task_register
    jnc .t3
    inc esi
.t3:
    mov eax, 1
    mov ebx, 5000    ; 5ms Periode
    mov ecx, 5000    ; 5ms Deadline
    mov edx, 500     ; 0.5ms WCET
    call rt_set_params
    jnc .t4
    inc esi
.t4:
    mov eax, 1
    mov ebx, 100     ; 100us Jitter
    call rt_record_jitter
    jnc .t5
    inc esi
.t5:
    mov eax, 1
    call rt_deadline_check
    jnc .done
    inc esi
.done:
    pop esi
    test esi, esi
    jz .ok
    stc
    ret
.ok:
    clc
    ret

align 4
rtmgr_ready:               dd 0
rt_task_count:          dd 0
rt_jitter_global_us:    dd 0
rt_deadline_miss_total: dd 0

align 4
rt_task_table:
    times RT_TASK_CAPACITY * RT_TASK_SIZE db 0
