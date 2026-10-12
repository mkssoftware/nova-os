; NPSPEC-SCHEDULER-FAIR-0001 / NPSPEC-SCHEDULER-REALTIME-0001
; NPSPEC-SCHEDULER-QOS-0001 / NPSPEC-CONCURRENCY-WORKSTEALING-0001
; NPSPEC-SCHEDULER-TOPOLOGY-0001 / NPSPEC-SCHEDULER-NUMA-0001
; NPSPEC-SCHEDULER-CACHE-0001
; ---------------------------------------------------------------------------

scheduler_initialize:
    mov dword [scheduler_enabled], 0
    mov dword [scheduler_current], 0
    mov edi, scheduler_contexts
    xor eax, eax
    mov ecx, SCHEDULER_THREAD_COUNT
    rep stosd
    mov edi, thread_cpu_affinity
    mov eax, -1
    mov ecx, SCHEDULER_THREAD_COUNT
    rep stosd
    mov edi, thread_running_on
    mov eax, -1
    mov ecx, SCHEDULER_THREAD_COUNT
    rep stosd
    mov dword [scheduler_thread1_runs], 0
    mov dword [scheduler_thread2_runs], 0
    mov dword [scheduler_dynamic_runs], 0
    ; §133: Slot 0 (BSP-Idle/Kernel-Thread) auf CPU 0 pinnen
    mov dword [thread_cpu_affinity + 0], 0      ; nur BSP (CPU 0) darf Slot 0 laden
    mov dword [thread_running_on + 0], 0        ; Slot 0 läuft ab Start auf BSP

    mov eax, scheduler_thread1
    call scheduler_create_frame
    jc .invalid
    mov [scheduler_contexts + 4], eax

    mov eax, scheduler_thread2
    call scheduler_create_frame
    jc .invalid
    mov [scheduler_contexts + 8], eax

    mov dword [scheduler_enabled], 1
    clc
    ret
.invalid:
    stc
    ret

; EAX=Thread-Einstieg, EAX=synthetischer Interruptframe.
scheduler_create_frame:
    push ebx
    push ecx
    push edi
    mov ebx, eax
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    sub edi, SCHEDULER_FRAME_SIZE

    mov dword [edi + 0], DATA_SEGMENT
    mov dword [edi + 4], DATA_SEGMENT
    mov dword [edi + 8], DATA_SEGMENT
    mov dword [edi + 12], DATA_SEGMENT
    mov dword [edi + 16], 0          ; EDI
    mov dword [edi + 20], 0          ; ESI
    mov dword [edi + 24], 0          ; EBP
    mov dword [edi + 28], 0          ; ESP-Platzhalter von PUSHAD
    mov dword [edi + 32], 0          ; EBX
    mov dword [edi + 36], 0          ; EDX
    mov dword [edi + 40], 0          ; ECX
    mov dword [edi + 44], 0          ; EAX
    mov dword [edi + 48], 32         ; normalisierter Vektor
    mov dword [edi + 52], 0          ; Fehlercode
    mov [edi + 56], ebx              ; EIP
    mov dword [edi + 60], CODE_SEGMENT
    mov dword [edi + 64], 0x00000202 ; IF gesetzt
    mov eax, edi
    pop edi
    pop ecx
    pop ebx
    clc
    ret
.invalid:
    pop edi
    pop ecx
    pop ebx
    stc
    ret

; [ESP+4] = vollständiger Frame-Zeiger des unterbrochenen Threads.
; Nutzt scheduler_caller_cpu (unter Spinlock gesetzt) für Affinitätsprüfung (§133).
; ESI/EDI werden gesichert und restauriert.
scheduler_on_tick:
    push esi
    push edi
    ; Basisadresse: [esp+12] = Frame-Zeiger (ursprünglich [esp+4] + 2×push)
    mov eax, [esp + 12]
    cmp dword [scheduler_enabled], 1
    jne .done

    ; Aktuellen Slot freigeben: Frame sichern, als "nicht laufend" markieren
    mov edx, [scheduler_current]
    mov [scheduler_contexts + edx * 4], eax
    mov [scheduler_previous_slot], edx
    mov dword [thread_running_on + edx * 4], -1    ; §133: Slot freigeben

    cmp dword [thread_manager_ready], 1
    jne .select_next
    mov eax, [thread_task_ids + edx * 4]
    test eax, eax
    jz .select_next
    call task_lookup
    jc .select_next
    cmp dword [eax + TASK_STATE], TASK_STATE_RUNNING
    jne .select_next
    mov dword [eax + TASK_STATE], TASK_STATE_READY

.select_next:
    mov edx, [scheduler_previous_slot]
    mov esi, edx                    ; §133: Start für Wrap-Detection (kein Endlos-Loop)
    mov edi, [scheduler_caller_cpu] ; §133: aufrufende CPU (gesetzt unter Spinlock)

.try_slot:
    inc edx
    cmp edx, SCHEDULER_THREAD_COUNT
    jb .check_affin
    xor edx, edx                    ; Wrap um auf Slot 0

.check_affin:
    cmp edx, esi                    ; §133: alle Slots geprüft → kein kompatibler Slot
    je .no_slot_found

    ; §133: Affinitätsprüfung
    mov eax, [thread_cpu_affinity + edx * 4]
    cmp eax, -1                     ; -1 = beliebige CPU → ok
    je .check_running
    cmp eax, edi                    ; affinity = diese CPU?
    jne .try_slot                   ; nein → nächster Slot

.check_running:
    ; §133: Slot bereits auf anderer CPU laufend?
    cmp dword [thread_running_on + edx * 4], -1
    jne .try_slot                   ; belegt → nächster Slot

.slot_ok:
    ; Slot passt – übernehmen
    mov [scheduler_current], edx
    mov [scheduler_selected_slot], edx
    mov [thread_running_on + edx * 4], edi   ; §133: als "laufend auf CPU edi" markieren

    cmp dword [thread_manager_ready], 1
    jne .load_frame
    mov eax, [thread_task_ids + edx * 4]
    test eax, eax
    jz .load_frame
    call task_lookup
    jc .load_frame
    cmp dword [eax + TASK_STATE], TASK_STATE_READY
    jne .load_frame
    mov dword [eax + TASK_STATE], TASK_STATE_RUNNING

.load_frame:
    mov edx, [scheduler_selected_slot]
    mov eax, [scheduler_contexts + edx * 4]
    jmp .done

.no_slot_found:
    ; §133: Kein freier kompatibler Slot.
    ; Aktuellen Slot wieder als "laufend" markieren und gleichen Frame zurückgeben.
    mov edx, [scheduler_previous_slot]
    mov [thread_running_on + edx * 4], edi
    mov [scheduler_current], edx
    mov eax, [esp + 12]

.done:
    pop edi
    pop esi
    ret

scheduler_thread1:
    inc dword [scheduler_thread1_runs]
    mov eax, [ipc_sent]
    inc eax
    mov dword [ipc_thread_message + 0], 1
    mov [ipc_thread_message + 4], eax
    mov dword [ipc_thread_message + 8], 0x4E4F5641
    mov dword [ipc_thread_message + 12], 0
    mov esi, ipc_thread_message
    call ipc_send
    test eax, eax
    jz .pause
    inc dword [ipc_sent]
.pause:
    pause
    jmp scheduler_thread1

scheduler_thread2:
    inc dword [scheduler_thread2_runs]
    mov edi, ipc_receive_buffer
    call ipc_receive
    test eax, eax
    jz .pause
    cmp dword [ipc_receive_buffer + 0], 1
    jne .error
    cmp dword [ipc_receive_buffer + 8], 0x4E4F5641
    jne .error
    mov eax, [ipc_receive_buffer + 4]
    cmp eax, [ipc_last_sequence]
    jbe .error
    mov [ipc_last_sequence], eax
    inc dword [ipc_received]
    jmp .pause
.error:
    mov dword [ipc_error], 1
.pause:
    pause
    jmp scheduler_thread2

scheduler_dynamic_thread:
    inc dword [scheduler_dynamic_runs]
    pause
    jmp scheduler_dynamic_thread

scheduler_self_test:
    mov ecx, 100000000
.wait:
    cmp dword [scheduler_thread1_runs], 0
    je .continue
    cmp dword [scheduler_thread2_runs], 0
    je .continue
    cmp dword [scheduler_dynamic_runs], 0
    je .continue
    cmp dword [ipc_error], 0
    jne .invalid
    cmp dword [ipc_received], 4
    jae .success
.continue:
    pause
    dec ecx
    jnz .wait
.invalid:
    stc
    ret
.success:
    clc
    ret

align 4
scheduler_api:
    dd SCHEDULER_API_SIZE
    dw 1, 0
    dd SCHEDULER_THREAD_COUNT
    dd SCHEDULER_CAP_PREEMPT | SCHEDULER_CAP_RR | SCHEDULER_CAP_AFFINITY | SCHEDULER_CAP_DYNAMIC
    dd scheduler_on_tick
    dd scheduler_current
    dd scheduler_thread1_runs
    dd scheduler_thread2_runs
    dd thread_cpu_affinity          ; Zeiger auf per-Slot Affinitäts-Array (§133)
    dd thread_running_on            ; Zeiger auf per-Slot Running-On-Array (§133)
    dd scheduler_contexts           ; §134: Slot-Kontexte für dynamische Threads
    dd thread_task_ids              ; §134: Slot→Task-Zuordnung

scheduler_lock:         dd 0       ; Spinlock – nur eine CPU gleichzeitig in scheduler_on_tick
scheduler_caller_cpu:   dd 0       ; CPU-Slot des Aufrufers (unter Spinlock gesetzt, §133)
scheduler_enabled:      dd 0
scheduler_current:      dd 0
scheduler_previous_slot: dd 0
scheduler_selected_slot: dd 0
scheduler_contexts:
    times SCHEDULER_THREAD_COUNT dd 0
thread_cpu_affinity:               ; per-Slot: -1=beliebige CPU, N=nur CPU N (§133)
    times SCHEDULER_THREAD_COUNT dd -1
thread_running_on:                 ; per-Slot: -1=frei, N=läuft auf CPU N (§133)
    times SCHEDULER_THREAD_COUNT dd -1
scheduler_thread1_runs: dd 0
scheduler_thread2_runs: dd 0
scheduler_dynamic_runs: dd 0

; ---------------------------------------------------------------------------
; Work-Stealing-Scheduler – pro-Slot lokale Task-Queues mit constraint-
; bewusster Steal-Logik (RT-Schutz, harte CPU-Affinitaet, NUMA-ready).
; NPSPEC-CONCURRENCY-WORKSTEALING-0001
; ---------------------------------------------------------------------------

WS_SLOT_COUNT       equ SCHEDULER_THREAD_COUNT
WS_QUEUE_DEPTH      equ 4
WS_TASK_ENTRY_SIZE  equ 16
WS_SLOT_RECORD_SIZE equ 24

SCHEDULER_CAP_WORKSTEAL equ 0x00000020

; Task-Eintrag-Felder
WS_TASK_ENTRY_POINT equ 0
WS_TASK_PID         equ 4
WS_TASK_CPU_MASK    equ 8
WS_TASK_FLAGS       equ 12

WS_TASK_FLAG_RT       equ 0x01     ; Hard-Realtime, nicht stehlfaehig
WS_TASK_FLAG_HARD_AFF equ 0x02     ; Strenge CPU-Affinitaet

; Slot-Queue-Header-Felder
WS_HEAD        equ 0
WS_TAIL        equ 4
WS_COUNT       equ 8
WS_CPU_MASK    equ 12
WS_FLAGS       equ 16
WS_STEAL_COUNT equ 20

worksteal_initialize:
    mov edi, ws_slot_queues
    xor eax, eax
    mov ecx, (WS_SLOT_COUNT * WS_SLOT_RECORD_SIZE) / 4
    rep stosd
    mov edi, ws_task_storage
    mov ecx, (WS_SLOT_COUNT * WS_QUEUE_DEPTH * WS_TASK_ENTRY_SIZE) / 4
    rep stosd
    ; CPU-Masken aus dem Scheduler-Affinitaets-Array initialisieren
    xor ecx, ecx
.wi_mask_loop:
    cmp ecx, WS_SLOT_COUNT
    jae .wi_mask_done
    mov edi, ecx
    imul edi, WS_SLOT_RECORD_SIZE
    add edi, ws_slot_queues
    mov eax, [thread_cpu_affinity + ecx * 4]
    cmp eax, -1
    jne .wi_has_aff
    mov eax, 0xFFFFFFFF
.wi_has_aff:
    mov [edi + WS_CPU_MASK], eax
    inc ecx
    jmp .wi_mask_loop
.wi_mask_done:
    mov dword [ws_initialized], 1
    mov dword [ws_total_steals], 0
    clc
    ret

; Reiht Task-Eintrag (16 Bytes) in Slot ein.
; EAX=Slot (0..WS_SLOT_COUNT-1), ESI=Quell-Ptr. CF=0 ok, CF=1 voll/ungueltig.
worksteal_enqueue:
    push ebx
    push ecx
    push edx
    push edi
    cmp eax, WS_SLOT_COUNT
    jae .weq_bad
    mov ebx, eax
    imul ebx, WS_SLOT_RECORD_SIZE
    add ebx, ws_slot_queues
    cmp dword [ebx + WS_COUNT], WS_QUEUE_DEPTH
    jae .weq_full
    mov edx, eax
    imul edx, WS_QUEUE_DEPTH * WS_TASK_ENTRY_SIZE
    add edx, ws_task_storage
    mov ecx, [ebx + WS_TAIL]
    imul ecx, WS_TASK_ENTRY_SIZE
    add edx, ecx
    mov edi, edx
    mov ecx, WS_TASK_ENTRY_SIZE / 4
    rep movsd
    mov ecx, [ebx + WS_TAIL]
    inc ecx
    cmp ecx, WS_QUEUE_DEPTH
    jb .weq_no_wrap
    xor ecx, ecx
.weq_no_wrap:
    mov [ebx + WS_TAIL], ecx
    inc dword [ebx + WS_COUNT]
    pop edi
    pop edx
    pop ecx
    pop ebx
    clc
    ret
.weq_full:
.weq_bad:
    pop edi
    pop edx
    pop ecx
    pop ebx
    stc
    ret

; Versucht, einen Task vom Victim-Slot zu stehlen.
; EAX=victim_slot, EDX=thief_slot, EDI=Ausgabepuffer (16 Bytes).
; CF=0 gestohlen, CF=1 kein stehlfaehiger Task.
worksteal_try_steal:
    push ebx
    push ecx
    push esi
    cmp eax, WS_SLOT_COUNT
    jae .wts_fail
    cmp edx, WS_SLOT_COUNT
    jae .wts_fail
    cmp eax, edx
    je .wts_fail
    mov ebx, eax
    imul ebx, WS_SLOT_RECORD_SIZE
    add ebx, ws_slot_queues
    cmp dword [ebx + WS_COUNT], 0
    je .wts_fail
    ; ESI = Adresse des Tasks am Head des Victim-Slots
    mov ecx, eax
    imul ecx, WS_QUEUE_DEPTH * WS_TASK_ENTRY_SIZE
    add ecx, ws_task_storage
    push eax
    mov eax, [ebx + WS_HEAD]
    imul eax, WS_TASK_ENTRY_SIZE
    add ecx, eax
    pop eax
    mov esi, ecx
    ; RT-Bit: niemals stehlen
    mov ecx, [esi + WS_TASK_FLAGS]
    test ecx, WS_TASK_FLAG_RT
    jnz .wts_fail
    ; Harte Affinitaet: nur wenn CPU-Mengen sich schneiden
    test ecx, WS_TASK_FLAG_HARD_AFF
    jz .wts_eligible
    push edx
    imul edx, WS_SLOT_RECORD_SIZE
    add edx, ws_slot_queues
    mov ecx, [edx + WS_CPU_MASK]
    pop edx
    and ecx, [esi + WS_TASK_CPU_MASK]
    jz .wts_fail
.wts_eligible:
    mov ecx, WS_TASK_ENTRY_SIZE / 4
    rep movsd
    mov ecx, [ebx + WS_HEAD]
    inc ecx
    cmp ecx, WS_QUEUE_DEPTH
    jb .wts_no_wrap
    xor ecx, ecx
.wts_no_wrap:
    mov [ebx + WS_HEAD], ecx
    dec dword [ebx + WS_COUNT]
    inc dword [ebx + WS_STEAL_COUNT]
    inc dword [ws_total_steals]
    pop esi
    pop ecx
    pop ebx
    clc
    ret
.wts_fail:
    pop esi
    pop ecx
    pop ebx
    stc
    ret

; Self-Test: 7 Tests (Init, Enqueue, Count, RT-Schutz, Steal, Daten, Count-0).
worksteal_self_test:
    push ebx
    push esi
    push edi
    sub esp, 32                         ; [esp+0..15]=Task-Buf, [esp+16..31]=Steal-Buf
    xor ebx, ebx

    ; Test 1: Subsystem initialisiert
    cmp dword [ws_initialized], 1
    je .wt2
    inc ebx

.wt2:
    ; Test 2: Slot 0 nach Init leer
    mov esi, ws_slot_queues
    cmp dword [esi + WS_COUNT], 0
    je .wt3
    inc ebx

.wt3:
    ; Test 3: Normalen Task in Slot 0 einreihen
    mov dword [esp +  0], scheduler_thread1
    mov dword [esp +  4], 1
    mov dword [esp +  8], 0xFFFFFFFF
    mov dword [esp + 12], 0
    xor eax, eax
    lea esi, [esp]
    call worksteal_enqueue
    jnc .wt4
    inc ebx

.wt4:
    ; Test 4: Count == 1 in Slot 0
    mov esi, ws_slot_queues
    cmp dword [esi + WS_COUNT], 1
    je .wt5
    inc ebx

.wt5:
    ; Test 5: RT-Task in Slot 1, Steal muss scheitern
    mov dword [esp +  0], scheduler_thread2
    mov dword [esp +  4], 1
    mov dword [esp +  8], 0xFFFFFFFF
    mov dword [esp + 12], WS_TASK_FLAG_RT
    mov eax, 1
    lea esi, [esp]
    call worksteal_enqueue
    jnc .wt5_steal
    inc ebx
    jmp .wt6
.wt5_steal:
    mov eax, 1
    mov edx, 0
    lea edi, [esp + 16]
    call worksteal_try_steal
    jc .wt6                         ; CF=1 erwartet (RT-Schutz)
    inc ebx

.wt6:
    ; Test 6: Steal des normalen Tasks von Slot 0 nach Slot 1
    xor eax, eax
    mov edx, 1
    lea edi, [esp + 16]
    call worksteal_try_steal
    jnc .wt7
    inc ebx

.wt7:
    ; Test 7: Count in Slot 0 == 0 nach Steal
    mov esi, ws_slot_queues
    cmp dword [esi + WS_COUNT], 0
    je .wt_done
    inc ebx

.wt_done:
    test ebx, ebx
    jnz .wt_fail
    add esp, 32
    pop edi
    pop esi
    pop ebx
    clc
    ret
.wt_fail:
    add esp, 32
    pop edi
    pop esi
    pop ebx
    stc
    ret

ws_initialized:  dd 0
ws_total_steals: dd 0
align 4
ws_slot_queues:
    times WS_SLOT_COUNT * WS_SLOT_RECORD_SIZE db 0
ws_task_storage:
    times WS_SLOT_COUNT * WS_QUEUE_DEPTH * WS_TASK_ENTRY_SIZE db 0

; ===========================================================================
; NPSPEC-SCHEDULER-REALTIME-0001 – Nova RT Scheduler (EDF / FIFO-RT)
; Dupliziert keine Scheduler-Logik; verwaltet RT-Task-Tabelle mit Deadline
; und Priorität für spätere Integration mit dem Haupt-Scheduler.
; ===========================================================================

RT_CAPACITY          equ 8
RT_RECORD_SIZE       equ 32

RT_STATE_FREE        equ 0
RT_STATE_READY       equ 1
RT_STATE_RUNNING     equ 2
RT_STATE_BLOCKED     equ 3
RT_STATE_COMPLETE    equ 4

RT_POLICY_EDF        equ 1   ; Earliest Deadline First
RT_POLICY_FIFO       equ 2   ; RT FIFO (Priorität)
RT_POLICY_RR         equ 3   ; RT Round Robin

RT_ID                equ 0
RT_STATE             equ 4
RT_OWNER             equ 8
RT_DEADLINE          equ 12  ; Absolute Deadline (in Ticks)
RT_PERIOD            equ 16  ; Periodendauer (0=sporadisch)
RT_PRIORITY          equ 20  ; 0=höchste RT-Prio
RT_POLICY_OFF        equ 24
RT_WCET              equ 28  ; Worst Case Execution Time

rt_scheduler_initialize:
    mov edi, rt_table
    mov ecx, RT_CAPACITY * RT_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [rt_next_id], 0
    mov dword [rt_active_count], 0
    mov dword [rt_miss_count], 0
    mov dword [rt_ready], 1
    clc
    ret

; EAX=owner  EDX=deadline_ticks  ECX=priority  EBX=policy → EAX=slot CF=0 / CF=1
rt_task_register:
    push esi
    push edi
    mov esi, rt_table
    push eax
    push edx
    push ecx
    xor edi, edi
.rtr_scan:
    cmp edi, RT_CAPACITY
    jae .rtr_full
    cmp dword [esi + RT_STATE], RT_STATE_FREE
    je .rtr_found
    add esi, RT_RECORD_SIZE
    inc edi
    jmp .rtr_scan
.rtr_found:
    pop ecx    ; priority
    pop edx    ; deadline
    pop eax    ; owner
    push edi
    push eax
    mov eax, [rt_next_id]
    inc dword [rt_next_id]
    mov [esi + RT_ID], eax
    pop eax
    mov [esi + RT_OWNER], eax
    mov dword [esi + RT_STATE], RT_STATE_READY
    mov [esi + RT_DEADLINE], edx
    mov [esi + RT_PRIORITY], ecx
    mov [esi + RT_POLICY_OFF], ebx
    mov dword [esi + RT_PERIOD], 0
    mov dword [esi + RT_WCET], 0
    inc dword [rt_active_count]
    pop eax    ; slot
    clc
    pop edi
    pop esi
    ret
.rtr_full:
    pop ecx
    pop edx
    pop eax
    stc
    pop edi
    pop esi
    ret

; Wählt nächste RT-Task nach EDF (früheste Deadline) → EAX=slot CF=0 / CF=1 keine
rt_select_next:
    push esi
    push ebx
    push ecx
    push edx
    mov esi, rt_table
    mov eax, -1        ; kein Kandidat
    mov edx, 0xFFFFFFFF ; best deadline
    xor ecx, ecx
.rsn_loop:
    cmp ecx, RT_CAPACITY
    jae .rsn_done
    cmp dword [esi + RT_STATE], RT_STATE_READY
    jne .rsn_next
    cmp dword [esi + RT_POLICY_OFF], RT_POLICY_EDF
    jne .rsn_fifo
    mov ebx, [esi + RT_DEADLINE]
    cmp ebx, edx
    jae .rsn_next
    mov edx, ebx
    mov eax, ecx
    jmp .rsn_next
.rsn_fifo:
    ; FIFO: kleinste Prioritätsnummer gewinnt (0=höchste)
    cmp eax, -1
    jne .rsn_next
    mov eax, ecx
.rsn_next:
    add esi, RT_RECORD_SIZE
    inc ecx
    jmp .rsn_loop
.rsn_done:
    cmp eax, -1
    je .rsn_empty
    clc
    pop edx
    pop ecx
    pop ebx
    pop esi
    ret
.rsn_empty:
    stc
    pop edx
    pop ecx
    pop ebx
    pop esi
    ret

; EAX=slot  EDX=current_tick → CF=0 ok / CF=1 Deadline verpasst
rt_check_deadline:
    push esi
    cmp eax, RT_CAPACITY
    jae .rcd_fail
    imul esi, eax, RT_RECORD_SIZE
    add esi, rt_table
    cmp dword [esi + RT_STATE], RT_STATE_FREE
    je .rcd_fail
    cmp edx, [esi + RT_DEADLINE]
    jbe .rcd_ok
    inc dword [rt_miss_count]
    stc
    pop esi
    ret
.rcd_ok:
    clc
    pop esi
    ret
.rcd_fail:
    stc
    pop esi
    ret

rt_scheduler_self_test:
    push ebx
    push esi
    xor esi, esi

    ; T1: ready
    cmp dword [rt_ready], 1
    je .rtst1_ok
    inc esi
.rtst1_ok:

    ; T2: register EDF task
    mov eax, 1       ; owner
    mov edx, 1000    ; deadline=1000 ticks
    mov ecx, 0       ; priority
    mov ebx, RT_POLICY_EDF
    call rt_task_register
    jnc .rtst2_ok
    inc esi
.rtst2_ok:
    mov ebx, eax    ; slot A

    ; T3: register second EDF task (earlier deadline)
    mov eax, 2
    mov edx, 500    ; früherer deadline
    mov ecx, 0
    push ebx
    mov ebx, RT_POLICY_EDF
    call rt_task_register
    pop ebx
    jnc .rtst3_ok
    inc esi
.rtst3_ok:
    push eax        ; slot B

    ; T4: select_next → muss slot B sein (deadline 500 < 1000)
    call rt_select_next
    pop ecx         ; slot B
    jnc .rtst4_cf_ok
    inc esi
.rtst4_cf_ok:
    cmp eax, ecx
    je .rtst4_ok
    inc esi
.rtst4_ok:

    ; T5: check_deadline – tick=300 für slot B (deadline=500) → CF=0
    mov edx, 300
    mov eax, ecx
    call rt_check_deadline
    jnc .rtst5_ok
    inc esi
.rtst5_ok:

    ; T6: check_deadline – tick=600 für slot B → CF=1 miss
    mov edx, 600
    mov eax, ecx
    call rt_check_deadline
    jc .rtst6_ok
    inc esi
.rtst6_ok:

    ; T7: miss_count=1
    cmp dword [rt_miss_count], 1
    je .rtst7_ok
    inc esi
.rtst7_ok:

    test esi, esi
    jnz .rtstf
    clc
    pop esi
    pop ebx
    ret
.rtstf:
    stc
    pop esi
    pop ebx
    ret

rt_ready:        dd 0
rt_next_id:      dd 0
rt_active_count: dd 0
rt_miss_count:   dd 0
align 4
rt_table:
    times RT_CAPACITY * RT_RECORD_SIZE db 0

; ===========================================================================
; NPSPEC-SCHEDULER-FAIR-0001 – Nova Fair Scheduler (CFS-Analogon)
; Virtuelle Laufzeit (vruntime) pro Task; immer der Task mit kleinster vruntime
; wird als nächstes ausgewählt (Minimum-Scan über Task-Tabelle).
; ===========================================================================

CFS_CAPACITY         equ 8
CFS_RECORD_SIZE      equ 24

CFS_STATE_FREE       equ 0
CFS_STATE_RUNNABLE   equ 1
CFS_STATE_BLOCKED    equ 2

CFS_ID               equ 0
CFS_STATE_OFF        equ 4
CFS_OWNER            equ 8
CFS_VRUNTIME         equ 12   ; virtuelle Laufzeit in Ticks
CFS_WEIGHT           equ 16   ; Prioritäts-Gewicht (niedrig = hohe Prio)
CFS_LAST_RUN         equ 20

cfs_initialize:
    mov edi, cfs_table
    mov ecx, CFS_CAPACITY * CFS_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [cfs_ready], 1
    mov dword [cfs_next_id], 0
    mov dword [cfs_switches], 0
    clc
    ret

; EAX=owner  EDX=weight → EAX=slot CF=0/CF=1 voll
cfs_task_enqueue:
    push esi
    push ecx
    mov esi, cfs_table
    xor ecx, ecx
.cfse_scan:
    cmp ecx, CFS_CAPACITY
    jae .cfse_full
    cmp dword [esi + CFS_STATE_OFF], CFS_STATE_FREE
    je .cfse_found
    add esi, CFS_RECORD_SIZE
    inc ecx
    jmp .cfse_scan
.cfse_found:
    push eax
    mov eax, [cfs_next_id]
    inc dword [cfs_next_id]
    mov [esi + CFS_ID], eax
    pop eax
    mov dword [esi + CFS_STATE_OFF], CFS_STATE_RUNNABLE
    mov [esi + CFS_OWNER], eax
    mov dword [esi + CFS_VRUNTIME], 0
    mov [esi + CFS_WEIGHT], edx
    mov dword [esi + CFS_LAST_RUN], 0
    mov eax, ecx
    clc
    pop ecx
    pop esi
    ret
.cfse_full:
    stc
    pop ecx
    pop esi
    ret

; Wählt Task mit kleinster vruntime → EAX=slot CF=0 / CF=1 kein runnable
cfs_pick_next:
    push esi
    push ecx
    push edx
    push ebx
    mov esi, cfs_table
    mov edx, 0xFFFFFFFF
    mov ebx, -1
    xor ecx, ecx
.cfspn_loop:
    cmp ecx, CFS_CAPACITY
    jae .cfspn_done
    cmp dword [esi + CFS_STATE_OFF], CFS_STATE_RUNNABLE
    jne .cfspn_next
    cmp dword [esi + CFS_VRUNTIME], edx
    jae .cfspn_next
    mov edx, [esi + CFS_VRUNTIME]
    mov ebx, ecx
.cfspn_next:
    add esi, CFS_RECORD_SIZE
    inc ecx
    jmp .cfspn_loop
.cfspn_done:
    cmp ebx, -1
    je .cfspn_empty
    mov eax, ebx
    clc
    pop ebx
    pop edx
    pop ecx
    pop esi
    ret
.cfspn_empty:
    stc
    pop ebx
    pop edx
    pop ecx
    pop esi
    ret

; EAX=slot  EDX=elapsed_ticks → vruntime += elapsed / weight; simuliert Fortschritt
cfs_update_vruntime:
    push esi
    cmp eax, CFS_CAPACITY
    jae .cfsuv_ret
    imul esi, eax, CFS_RECORD_SIZE
    add esi, cfs_table
    cmp dword [esi + CFS_STATE_OFF], CFS_STATE_FREE
    je .cfsuv_ret
    ; delta_vruntime = elapsed / weight (integer-Division)
    push eax
    push edx
    mov eax, edx
    xor edx, edx
    cmp dword [esi + CFS_WEIGHT], 0
    je .cfsuv_no_div
    div dword [esi + CFS_WEIGHT]
.cfsuv_no_div:
    add [esi + CFS_VRUNTIME], eax
    push eax
    mov eax, [wd_global_tick]
    mov [esi + CFS_LAST_RUN], eax
    pop eax
    inc dword [cfs_switches]
    pop edx
    pop eax
.cfsuv_ret:
    pop esi
    ret

cfs_self_test:
    push ebx
    push esi
    push ecx
    xor esi, esi
    cmp dword [cfs_ready], 1
    je .cfst1
    inc esi
.cfst1:
    ; Task A weight=1, Task B weight=2
    mov eax, 1
    mov edx, 1
    call cfs_task_enqueue
    jnc .cfst2
    inc esi
.cfst2:
    mov ebx, eax   ; Task A slot
    mov eax, 2
    mov edx, 2
    call cfs_task_enqueue
    jnc .cfst3
    inc esi
.cfst3:
    mov ecx, eax   ; Task B slot
    ; pick_next → beide vruntime=0, wählt Slot mit kleinerem Index
    call cfs_pick_next
    jnc .cfst4_cf
    inc esi
.cfst4_cf:
    ; update_vruntime für Task A: 10 Ticks / weight=1 = 10
    mov eax, ebx
    mov edx, 10
    call cfs_update_vruntime
    ; pick_next → Task B (vruntime=0 < 10)
    call cfs_pick_next
    jnc .cfst5_cf
    inc esi
.cfst5_cf:
    cmp eax, ecx
    je .cfst5
    inc esi
.cfst5:
    test esi, esi
    jnz .cfstf
    clc
    pop ecx
    pop esi
    pop ebx
    ret
.cfstf:
    stc
    pop ecx
    pop esi
    pop ebx
    ret

cfs_ready:    dd 0
cfs_next_id:  dd 0
cfs_switches: dd 0
align 4
cfs_table:
    times CFS_CAPACITY * CFS_RECORD_SIZE db 0

; ===========================================================================
; NPSPEC-SCHEDULER-QOS-0001 – Nova Quality-of-Service Scheduler
; QoS-Klassen mit Bandbreiten-Budgets; verhindert Starvation durch
; Budget-Buckets. Jede Klasse bekommt ihr Budget pro Epoche erneut.
; ===========================================================================

QOS_CLASS_COUNT      equ 4
QOS_RECORD_SIZE      equ 20

QOS_CLASS_RT         equ 0   ; Real-Time
QOS_CLASS_HIGH       equ 1   ; High-Priority
QOS_CLASS_NORMAL     equ 2   ; Normal
QOS_CLASS_IDLE       equ 3   ; Idle/Background

QOS_ID               equ 0
QOS_CLASS_OFF        equ 4
QOS_BUDGET           equ 8    ; verbleibendes Budget in Ticks
QOS_MAX_BUDGET       equ 12   ; Budget pro Epoche
QOS_USED             equ 16   ; verbrauchte Ticks gesamt

qos_initialize:
    mov edi, qos_table
    mov ecx, QOS_CLASS_COUNT * QOS_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    ; Klassen mit Standard-Budgets füllen
    mov esi, qos_table
    ; RT: Budget=100 (höchste Garantie)
    mov dword [esi + QOS_ID],         QOS_CLASS_RT
    mov dword [esi + QOS_CLASS_OFF],  QOS_CLASS_RT
    mov dword [esi + QOS_BUDGET],     100
    mov dword [esi + QOS_MAX_BUDGET], 100
    add esi, QOS_RECORD_SIZE
    ; HIGH: Budget=70
    mov dword [esi + QOS_ID],         QOS_CLASS_HIGH
    mov dword [esi + QOS_CLASS_OFF],  QOS_CLASS_HIGH
    mov dword [esi + QOS_BUDGET],     70
    mov dword [esi + QOS_MAX_BUDGET], 70
    add esi, QOS_RECORD_SIZE
    ; NORMAL: Budget=40
    mov dword [esi + QOS_ID],         QOS_CLASS_NORMAL
    mov dword [esi + QOS_CLASS_OFF],  QOS_CLASS_NORMAL
    mov dword [esi + QOS_BUDGET],     40
    mov dword [esi + QOS_MAX_BUDGET], 40
    add esi, QOS_RECORD_SIZE
    ; IDLE: Budget=10
    mov dword [esi + QOS_ID],         QOS_CLASS_IDLE
    mov dword [esi + QOS_CLASS_OFF],  QOS_CLASS_IDLE
    mov dword [esi + QOS_BUDGET],     10
    mov dword [esi + QOS_MAX_BUDGET], 10
    mov dword [qos_ready], 1
    mov dword [qos_epoch], 0
    mov dword [qos_throttled], 0
    clc
    ret

; EAX=class → CF=0 Budget vorhanden (dekrementiert) / CF=1 ausgeschöpft (throttled)
qos_consume:
    push esi
    cmp eax, QOS_CLASS_COUNT
    jae .qosc_fail
    imul esi, eax, QOS_RECORD_SIZE
    add esi, qos_table
    cmp dword [esi + QOS_BUDGET], 0
    je .qosc_throttle
    dec dword [esi + QOS_BUDGET]
    inc dword [esi + QOS_USED]
    clc
    pop esi
    ret
.qosc_throttle:
    inc dword [qos_throttled]
    stc
    pop esi
    ret
.qosc_fail:
    stc
    pop esi
    ret

; Setzt alle Klassen-Budgets zurück (neue Epoche)
qos_new_epoch:
    push esi
    push ecx
    mov esi, qos_table
    xor ecx, ecx
.qosne_loop:
    cmp ecx, QOS_CLASS_COUNT
    jae .qosne_done
    push eax
    mov eax, [esi + QOS_MAX_BUDGET]
    mov [esi + QOS_BUDGET], eax
    pop eax
    add esi, QOS_RECORD_SIZE
    inc ecx
    jmp .qosne_loop
.qosne_done:
    inc dword [qos_epoch]
    pop ecx
    pop esi
    ret

; EAX=class → EAX=budget_remaining  EDX=max  CF=0/CF=1
qos_query:
    push esi
    cmp eax, QOS_CLASS_COUNT
    jae .qosq_fail
    imul esi, eax, QOS_RECORD_SIZE
    add esi, qos_table
    mov eax, [esi + QOS_BUDGET]
    mov edx, [esi + QOS_MAX_BUDGET]
    clc
    pop esi
    ret
.qosq_fail:
    stc
    pop esi
    ret

qos_self_test:
    push esi
    xor esi, esi
    cmp dword [qos_ready], 1
    je .qsst1
    inc esi
.qsst1:
    ; RT-Klasse Budget=100 prüfen
    mov eax, QOS_CLASS_RT
    call qos_query
    jnc .qsst2
    inc esi
.qsst2:
    cmp eax, 100
    je .qsst3
    inc esi
.qsst3:
    ; consume RT bis auf 0
    push ecx
    mov ecx, 100
.qsst_burn:
    mov eax, QOS_CLASS_RT
    call qos_consume
    loop .qsst_burn
    pop ecx
    ; nächster consume → CF=1
    mov eax, QOS_CLASS_RT
    call qos_consume
    jc .qsst4
    inc esi
.qsst4:
    cmp dword [qos_throttled], 1
    je .qsst5
    inc esi
.qsst5:
    ; Neue Epoche → Budget zurück
    call qos_new_epoch
    mov eax, QOS_CLASS_RT
    call qos_query
    cmp eax, 100
    je .qsst6
    inc esi
.qsst6:
    cmp dword [qos_epoch], 1
    je .qsst7
    inc esi
.qsst7:
    test esi, esi
    jnz .qsstf
    clc
    pop esi
    ret
.qsstf:
    stc
    pop esi
    ret

qos_ready:     dd 0
qos_epoch:     dd 0
qos_throttled: dd 0
align 4
qos_table:
    times QOS_CLASS_COUNT * QOS_RECORD_SIZE db 0

; ===========================================================================
