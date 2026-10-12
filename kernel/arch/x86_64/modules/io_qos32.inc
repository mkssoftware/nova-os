; NPSPEC-IO-PRIORITY/SCHEDULER-0001
; ---------------------------------------------------------------------------

IO_SCHEDULER_API_SIZE       equ 32
IO_SCHEDULER_CAPABILITIES   equ 0x0000000F
IO_SCHEDULER_AGING_ROUNDS   equ 4

io_scheduler_initialize:
    mov dword [io_scheduler_dispatch_count], 0
    mov dword [io_scheduler_promotion_count], 0
    mov dword [io_scheduler_ready], 1
    clc
    ret

; Waehlt einen pending Request. Deadline Requests werden nach fruehester
; Deadline sortiert; sonst entscheiden effektive Prioritaet und Wartealter.
; EAX=Request-ID, CF=1 wenn die Queue keine ausfuehrbare Arbeit enthaelt.
io_scheduler_select:
    pushfd
    cli
    mov dword [io_scheduler_candidate], 0
    mov dword [io_scheduler_candidate_slot], 0
    xor ecx, ecx
.scan:
    cmp ecx, IO_REQUEST_CAPACITY
    jae .selected
    mov edi, ecx
    shl edi, 6
    add edi, io_request_table
    cmp dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    jne .next
    mov esi, [io_scheduler_candidate]
    test esi, esi
    jz .choose
    test dword [edi + IO_REQUEST_FLAGS], IO_FLAG_DEADLINE_INHERITED
    jz .current_without_deadline
    test dword [esi + IO_REQUEST_FLAGS], IO_FLAG_DEADLINE_INHERITED
    jz .choose
    mov eax, [edi + IO_REQUEST_DEADLINE]
    sub eax, [esi + IO_REQUEST_DEADLINE]
    jl .choose
    jg .next
    jmp .priority
.current_without_deadline:
    test dword [esi + IO_REQUEST_FLAGS], IO_FLAG_DEADLINE_INHERITED
    jnz .next
.priority:
    mov eax, [edi + IO_REQUEST_EFFECTIVE_PRIORITY]
    cmp eax, [esi + IO_REQUEST_EFFECTIVE_PRIORITY]
    jb .choose
    ja .next
    mov eax, [io_request_submit_ticks + ecx * 4]
    mov edx, [io_scheduler_candidate_slot]
    cmp eax, [io_request_submit_ticks + edx * 4]
    jae .next
.choose:
    mov [io_scheduler_candidate], edi
    mov [io_scheduler_candidate_slot], ecx
.next:
    inc ecx
    jmp .scan
.selected:
    mov edi, [io_scheduler_candidate]
    test edi, edi
    jz .empty
    mov dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_RUNNING
    mov ecx, [io_scheduler_candidate_slot]
    mov eax, [timer_ticks]
    mov [io_request_dispatch_ticks + ecx * 4], eax
    mov dword [io_request_wait_rounds + ecx * 4], 0
    inc dword [io_scheduler_dispatch_count]

    ; Nicht ausgewaehlte Requests altern begrenzt bis Interactive. Realtime
    ; bleibt einer expliziten Policy vorbehalten und wird nicht erzwungen.
    xor ecx, ecx
.age:
    cmp ecx, IO_REQUEST_CAPACITY
    jae .return
    cmp ecx, [io_scheduler_candidate_slot]
    je .age_next
    mov esi, ecx
    shl esi, 6
    add esi, io_request_table
    cmp dword [esi + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    jne .age_next
    inc dword [io_request_wait_rounds + ecx * 4]
    cmp dword [io_request_wait_rounds + ecx * 4], IO_SCHEDULER_AGING_ROUNDS
    jb .age_next
    mov dword [io_request_wait_rounds + ecx * 4], 0
    cmp dword [esi + IO_REQUEST_EFFECTIVE_PRIORITY], IO_PRIORITY_INTERACTIVE
    jbe .age_next
    dec dword [esi + IO_REQUEST_EFFECTIVE_PRIORITY]
    inc dword [io_scheduler_promotion_count]
.age_next:
    inc ecx
    jmp .age
.return:
    mov eax, [edi + IO_REQUEST_ID]
    popfd
    clc
    ret
.empty:
    xor eax, eax
    popfd
    stc
    ret

; Provider-Backpressure darf einen ausgewaehlten Request ohne Identitaets- oder
; Altersverlust wieder in die begrenzte Scheduler-Queue stellen.
io_scheduler_requeue:
    pushfd
    cli
    call io_request_lookup
    jc .invalid
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_RUNNING
    jne .invalid
    mov dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    mov eax, [eax + IO_REQUEST_ID]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

io_scheduler_self_test:
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [io_scheduler_test_scope], eax

    ; Unprivilegiertes Realtime wird policy-konform auf Interactive begrenzt.
    mov eax, 1
    mov edx, [io_scheduler_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [io_scheduler_test_fast_task], eax
    mov eax, 1
    mov edx, [io_scheduler_test_fast_task]
    mov ebx, 1
    mov ecx, 0x300
    mov esi, 0x400
    mov edi, 256
    mov ebp, IO_PRIORITY_REALTIME
    call io_request_submit
    jc .invalid
    mov [io_scheduler_test_fast_request], eax
    call io_request_lookup
    jc .invalid
    cmp dword [eax + IO_REQUEST_PRIORITY], IO_PRIORITY_REALTIME
    jne .invalid
    cmp dword [eax + IO_REQUEST_EFFECTIVE_PRIORITY], IO_PRIORITY_INTERACTIVE
    jne .invalid

    ; Eine Maintenance-Anfrage mit Deadline geht trotz niedrigerer Prioritaet
    ; vor der rein prioritaetsgesteuerten Anfrage in Ausfuehrung.
    mov eax, 1
    mov edx, [io_scheduler_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [io_scheduler_test_deadline_task], eax
    mov edx, [timer_ticks]
    add edx, 100
    mov ebx, TASK_DEADLINE_CLASS_SOFT
    mov ecx, TASK_DEADLINE_POLICY_CANCEL
    call task_deadline_set
    jc .invalid
    mov eax, 1
    mov edx, [io_scheduler_test_deadline_task]
    mov ebx, 2
    mov ecx, 0x301
    mov esi, 0x401
    mov edi, 256
    mov ebp, IO_PRIORITY_MAINTENANCE
    call io_request_submit
    jc .invalid
    mov [io_scheduler_test_deadline_request], eax

    call io_scheduler_select
    jc .invalid
    cmp eax, [io_scheduler_test_deadline_request]
    jne .invalid
    call io_scheduler_requeue
    jc .invalid
    call io_scheduler_select
    jc .invalid
    cmp eax, [io_scheduler_test_deadline_request]
    jne .invalid
    mov edx, 256
    xor ebx, ebx
    call io_request_complete
    jc .invalid
    call io_scheduler_select
    jc .invalid
    cmp eax, [io_scheduler_test_fast_request]
    jne .invalid
    mov edx, 256
    xor ebx, ebx
    call io_request_complete
    jc .invalid
    cmp dword [io_request_outstanding], 0
    jne .invalid

    mov eax, [io_scheduler_test_fast_task]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [io_scheduler_test_deadline_task]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [io_scheduler_test_scope]
    call task_scope_close
    jc .invalid
    cmp dword [task_count], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
io_scheduler_api:
    dd IO_SCHEDULER_API_SIZE
    dw 1, 0
    dd 5
    dd IO_SCHEDULER_CAPABILITIES
    dd io_scheduler_select
    dd io_scheduler_requeue
    dd io_request_outstanding
    dd io_scheduler_dispatch_count

io_scheduler_ready:                 dd 0
io_scheduler_dispatch_count:        dd 0
io_scheduler_promotion_count:       dd 0
io_scheduler_candidate:             dd 0
io_scheduler_candidate_slot:        dd 0
io_scheduler_test_scope:            dd 0
io_scheduler_test_fast_task:        dd 0
io_scheduler_test_deadline_task:    dd 0
io_scheduler_test_fast_request:     dd 0
io_scheduler_test_deadline_request: dd 0

; ---------------------------------------------------------------------------
; I/O Quality of Service: getrennte Profile, Admission und Accounting
; NPSPEC-IO-QOS-0001 / ADR-IO-0003 und ADR-IO-0006
; ---------------------------------------------------------------------------

IO_QOS_API_SIZE             equ 32
IO_QOS_CAPACITY             equ 4
IO_QOS_RECORD_SIZE          equ 64
IO_QOS_STATE_EMPTY          equ 0
IO_QOS_STATE_ACCEPTED       equ 1
IO_QOS_STATE_DEGRADED       equ 2
IO_QOS_STATE_REJECTED       equ 3
IO_QOS_HARD_LATENCY         equ 0x00000001
IO_QOS_HARD_THROUGHPUT      equ 0x00000002
IO_QOS_HARD_BANDWIDTH       equ 0x00000004
IO_QOS_DEGRADE_THROUGHPUT   equ 0x00000001
IO_QOS_DEGRADE_BANDWIDTH    equ 0x00000002
IO_QOS_CAPABILITIES         equ 0x0000000F
IO_QOS_PROFILE_ID           equ 0
IO_QOS_OWNER                equ 4
IO_QOS_CLASS                equ 8
IO_QOS_REQUIREMENTS         equ 12
IO_QOS_LATENCY_TARGET       equ 16
IO_QOS_MIN_THROUGHPUT       equ 20
IO_QOS_MAX_BANDWIDTH        equ 24
IO_QOS_MAX_OUTSTANDING      equ 28
IO_QOS_ACTIVE_REQUESTS      equ 32
IO_QOS_STATE                equ 36
IO_QOS_OBSERVED_LATENCY     equ 40
IO_QOS_VIOLATIONS           equ 44
IO_QOS_COMPLETED_BYTES_LOW  equ 48
IO_QOS_COMPLETED_BYTES_HIGH equ 52
IO_QOS_DEGRADATION_REASON   equ 56

io_qos_initialize:
    mov edi, io_qos_table
    xor eax, eax
    mov ecx, (IO_QOS_CAPACITY * IO_QOS_RECORD_SIZE) / 4
    rep stosd
    mov edi, io_request_qos_ids
    mov ecx, IO_REQUEST_CAPACITY
    rep stosd
    mov dword [io_qos_next_id], 1
    mov dword [io_qos_rejection_count], 0
    mov dword [io_qos_throttle_count], 0
    mov dword [io_qos_manager_ready], 1
    clc
    ret

; EAX=Owner, EDX=QoS-Klasse (0=Normal), EBX=Hard-Flags,
; ECX=Latenzziel, ESI=Min-Durchsatz, EDI=Max-Bandbreite,
; EBP=maximal offene Requests (0=2). EAX=Profil-ID.
io_qos_create:
    pushfd
    cli
    mov [io_qos_temp_owner], eax
    mov [io_qos_temp_class], edx
    mov [io_qos_temp_requirements], ebx
    mov [io_qos_temp_latency], ecx
    mov [io_qos_temp_throughput], esi
    mov [io_qos_temp_bandwidth], edi
    mov [io_qos_temp_outstanding], ebp
    call process_lookup
    jc .invalid
    cmp dword [io_qos_temp_requirements], IO_QOS_HARD_LATENCY | IO_QOS_HARD_THROUGHPUT | IO_QOS_HARD_BANDWIDTH
    ja .invalid
    mov edx, [io_qos_temp_class]
    test edx, edx
    jnz .class_set
    mov edx, IO_PRIORITY_NORMAL
    mov [io_qos_temp_class], edx
.class_set:
    cmp edx, IO_PRIORITY_REALTIME
    jb .invalid
    cmp edx, IO_PRIORITY_MAINTENANCE
    ja .invalid
    mov ebp, [io_qos_temp_outstanding]
    test ebp, ebp
    jnz .outstanding_set
    mov ebp, 2
    mov [io_qos_temp_outstanding], ebp
.outstanding_set:
    cmp ebp, IO_REQUEST_CAPACITY
    ja .reject
    test dword [io_qos_temp_requirements], IO_QOS_HARD_THROUGHPUT | IO_QOS_HARD_BANDWIDTH
    jnz .reject                         ; kein Provider darf Garantie vortaeuschen
    test dword [io_qos_temp_requirements], IO_QOS_HARD_LATENCY
    jz .find
    cmp dword [io_qos_temp_latency], 2  ; Bootstrap-Admission: mindestens 20 ms
    jb .reject
.find:
    xor ecx, ecx
.scan:
    cmp ecx, IO_QOS_CAPACITY
    jae .reject
    mov edi, ecx
    shl edi, 6
    add edi, io_qos_table
    cmp dword [edi + IO_QOS_STATE], IO_QOS_STATE_EMPTY
    je .slot
    inc ecx
    jmp .scan
.slot:
    mov [io_qos_temp_record], edi
    xor eax, eax
    mov ecx, IO_QOS_RECORD_SIZE / 4
    rep stosd
    mov edi, [io_qos_temp_record]
    mov eax, [io_qos_next_id]
    mov [edi + IO_QOS_PROFILE_ID], eax
    mov edx, [io_qos_temp_owner]
    mov [edi + IO_QOS_OWNER], edx
    mov edx, [io_qos_temp_class]
    mov [edi + IO_QOS_CLASS], edx
    mov edx, [io_qos_temp_requirements]
    mov [edi + IO_QOS_REQUIREMENTS], edx
    mov edx, [io_qos_temp_latency]
    mov [edi + IO_QOS_LATENCY_TARGET], edx
    mov edx, [io_qos_temp_throughput]
    mov [edi + IO_QOS_MIN_THROUGHPUT], edx
    mov edx, [io_qos_temp_bandwidth]
    mov [edi + IO_QOS_MAX_BANDWIDTH], edx
    mov edx, [io_qos_temp_outstanding]
    mov [edi + IO_QOS_MAX_OUTSTANDING], edx
    mov dword [edi + IO_QOS_STATE], IO_QOS_STATE_ACCEPTED
    xor edx, edx
    cmp dword [edi + IO_QOS_MIN_THROUGHPUT], 0
    je .check_bandwidth
    or edx, IO_QOS_DEGRADE_THROUGHPUT
.check_bandwidth:
    cmp dword [edi + IO_QOS_MAX_BANDWIDTH], 0
    je .degradation_done
    or edx, IO_QOS_DEGRADE_BANDWIDTH
.degradation_done:
    test edx, edx
    jz .accepted
    mov dword [edi + IO_QOS_STATE], IO_QOS_STATE_DEGRADED
    mov [edi + IO_QOS_DEGRADATION_REASON], edx
.accepted:
    inc dword [io_qos_next_id]
    mov eax, [edi + IO_QOS_PROFILE_ID]
    popfd
    clc
    ret
.reject:
    inc dword [io_qos_rejection_count]
.invalid:
    xor eax, eax
    popfd
    stc
    ret

io_qos_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, IO_QOS_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, io_qos_table
    cmp dword [edx + IO_QOS_STATE], IO_QOS_STATE_EMPTY
    je .next
    cmp [edx + IO_QOS_PROFILE_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Request-ID, EDX=Profil-ID. Budget und Owner werden vor Bindung geprueft.
io_qos_attach:
    pushfd
    cli
    mov [io_qos_temp_request], eax
    mov [io_qos_temp_profile], edx
    call io_request_lookup
    jc .invalid
    mov [io_qos_temp_request_record], eax
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    jne .invalid
    mov eax, [io_qos_temp_profile]
    call io_qos_lookup
    jc .invalid
    mov [io_qos_temp_record], eax
    mov edx, [io_qos_temp_request_record]
    mov ecx, [edx + IO_REQUEST_OWNER]
    cmp [eax + IO_QOS_OWNER], ecx
    jne .invalid
    mov ecx, [eax + IO_QOS_ACTIVE_REQUESTS]
    cmp ecx, [eax + IO_QOS_MAX_OUTSTANDING]
    jae .throttle
    mov ecx, edx
    sub ecx, io_request_table
    shr ecx, 6
    cmp dword [io_request_qos_ids + ecx * 4], 0
    jne .invalid
    mov eax, [io_qos_temp_profile]
    mov [io_request_qos_ids + ecx * 4], eax
    mov eax, [io_qos_temp_record]
    inc dword [eax + IO_QOS_ACTIVE_REQUESTS]
    mov eax, [eax + IO_QOS_CLASS]
    cmp eax, IO_PRIORITY_REALTIME
    jne .class_ready
    mov eax, IO_PRIORITY_INTERACTIVE
.class_ready:
    mov edx, [io_qos_temp_request_record]
    cmp eax, [edx + IO_REQUEST_EFFECTIVE_PRIORITY]
    jae .attached
    mov [edx + IO_REQUEST_EFFECTIVE_PRIORITY], eax
.attached:
    mov eax, [io_qos_temp_request]
    popfd
    clc
    ret
.throttle:
    inc dword [io_qos_throttle_count]
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=terminaler Requestdatensatz. Beobachtete Werte bleiben vom angeforderten
; Profil getrennt; eine Zielverletzung wird explizit gezaehlt.
io_qos_on_terminal:
    cmp dword [io_qos_manager_ready], 1
    jne .done
    mov [io_qos_terminal_request], eax
    mov ecx, eax
    sub ecx, io_request_table
    shr ecx, 6
    mov eax, [io_request_qos_ids + ecx * 4]
    test eax, eax
    jz .done
    mov [io_qos_temp_slot], ecx
    call io_qos_lookup
    jc .done
    cmp dword [eax + IO_QOS_ACTIVE_REQUESTS], 0
    je .done
    dec dword [eax + IO_QOS_ACTIVE_REQUESTS]
    mov edx, [timer_ticks]
    mov ecx, [io_qos_temp_slot]
    sub edx, [io_request_submit_ticks + ecx * 4]
    mov [eax + IO_QOS_OBSERVED_LATENCY], edx
    mov ecx, [eax + IO_QOS_LATENCY_TARGET]
    test ecx, ecx
    jz .bytes
    cmp edx, ecx
    jbe .bytes
    inc dword [eax + IO_QOS_VIOLATIONS]
.bytes:
    mov edx, [io_qos_terminal_request]
    mov edx, [edx + IO_REQUEST_TRANSFERRED]
    add [eax + IO_QOS_COMPLETED_BYTES_LOW], edx
    adc dword [eax + IO_QOS_COMPLETED_BYTES_HIGH], 0
.done:
    clc
    ret

io_qos_self_test:
    ; Weiche, providerabhaengige Durchsatzwuensche werden sichtbar degradiert.
    mov eax, 1
    mov edx, IO_PRIORITY_INTERACTIVE
    xor ebx, ebx
    mov ecx, 50
    mov esi, 4096
    xor edi, edi
    mov ebp, 1
    call io_qos_create
    jc .invalid
    mov [io_qos_test_profile], eax
    call io_qos_lookup
    jc .invalid
    cmp dword [eax + IO_QOS_STATE], IO_QOS_STATE_DEGRADED
    jne .invalid
    test dword [eax + IO_QOS_DEGRADATION_REASON], IO_QOS_DEGRADE_THROUGHPUT
    jz .invalid

    ; Eine nicht belegbare harte Durchsatzgarantie wird nicht vorgetaeuscht.
    mov eax, 1
    mov edx, IO_PRIORITY_NORMAL
    mov ebx, IO_QOS_HARD_THROUGHPUT
    xor ecx, ecx
    mov esi, 4096
    xor edi, edi
    mov ebp, 1
    call io_qos_create
    jnc .invalid
    cmp dword [io_qos_rejection_count], 1
    jne .invalid

    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [io_qos_test_scope], eax
    mov eax, 1
    mov edx, [io_qos_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [io_qos_test_task], eax

    ; MaxOutstanding=1 isoliert das Profil und erzeugt beim zweiten Attach
    ; kontrollierte Drosselung statt stiller Budgetueberschreitung.
    mov eax, 1
    mov edx, [io_qos_test_task]
    mov ebx, 3
    mov ecx, 0x500
    mov esi, 0x600
    mov edi, 128
    mov ebp, IO_PRIORITY_NORMAL
    call io_request_submit
    jc .invalid
    mov [io_qos_test_request1], eax
    mov edx, [io_qos_test_profile]
    call io_qos_attach
    jc .invalid
    mov eax, 1
    mov edx, [io_qos_test_task]
    mov ebx, 3
    mov ecx, 0x501
    mov esi, 0x601
    mov edi, 128
    mov ebp, IO_PRIORITY_NORMAL
    call io_request_submit
    jc .invalid
    mov [io_qos_test_request2], eax
    mov edx, [io_qos_test_profile]
    call io_qos_attach
    jnc .invalid
    cmp dword [io_qos_throttle_count], 1
    jne .invalid
    mov eax, [io_qos_test_request1]
    mov edx, 128
    xor ebx, ebx
    call io_request_complete
    jc .invalid
    mov eax, [io_qos_test_profile]
    call io_qos_lookup
    jc .invalid
    cmp dword [eax + IO_QOS_ACTIVE_REQUESTS], 0
    jne .invalid
    cmp dword [eax + IO_QOS_COMPLETED_BYTES_LOW], 128
    jne .invalid
    mov eax, [io_qos_test_request2]
    mov edx, 0x514F5343               ; "QOSC"
    call io_request_cancel
    jc .invalid
    mov eax, [io_qos_test_task]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [io_qos_test_scope]
    call task_scope_close
    jc .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
io_qos_api:
    dd IO_QOS_API_SIZE
    dw 1, 0
    dd IO_QOS_CAPACITY
    dd IO_QOS_CAPABILITIES
    dd io_qos_create
    dd io_qos_attach
    dd io_qos_table
    dd io_qos_rejection_count

io_qos_manager_ready:        dd 0
io_qos_next_id:              dd 0
io_qos_rejection_count:      dd 0
io_qos_throttle_count:       dd 0
io_qos_temp_owner:           dd 0
io_qos_temp_class:           dd 0
io_qos_temp_requirements:    dd 0
io_qos_temp_latency:         dd 0
io_qos_temp_throughput:      dd 0
io_qos_temp_bandwidth:       dd 0
io_qos_temp_outstanding:     dd 0
io_qos_temp_record:          dd 0
io_qos_temp_request:         dd 0
io_qos_temp_profile:         dd 0
io_qos_temp_request_record:  dd 0
io_qos_terminal_request:     dd 0
io_qos_temp_slot:            dd 0
io_qos_test_profile:         dd 0
io_qos_test_scope:           dd 0
io_qos_test_task:            dd 0
io_qos_test_request1:        dd 0
io_qos_test_request2:        dd 0
align 4
io_qos_table:
    times IO_QOS_CAPACITY * IO_QOS_RECORD_SIZE db 0

io_request_manager_self_test:
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [io_request_test_scope], eax
    mov eax, 1
    mov edx, [io_request_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [io_request_test_task], eax
    mov edx, [timer_ticks]
    add edx, 100
    mov [io_request_test_deadline], edx
    mov ebx, TASK_DEADLINE_CLASS_SOFT
    mov ecx, TASK_DEADLINE_POLICY_CANCEL
    call task_deadline_set
    jc .invalid

    ; Partial Completion prueft getrennte Request-/Completion-Zustaende.
    mov eax, 1
    mov edx, [io_request_test_task]
    mov ebx, 1
    mov ecx, 0x100
    mov esi, 0x200
    mov edi, 4096
    mov ebp, 1
    call io_request_submit
    jc .invalid
    mov [io_request_test_first], eax
    call io_request_lookup
    jc .invalid
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    jne .invalid
    mov edx, [io_request_test_scope]
    cmp [eax + IO_REQUEST_SCOPE], edx
    jne .invalid
    mov edx, [io_request_test_deadline]
    cmp [eax + IO_REQUEST_DEADLINE], edx
    jne .invalid
    mov eax, [io_request_test_first]
    mov edx, 2048
    xor ebx, ebx
    call io_request_complete
    jc .invalid
    mov eax, [io_request_test_first]
    call io_request_lookup
    jc .invalid
    cmp dword [eax + IO_REQUEST_COMPLETION], IO_COMPLETION_PARTIAL
    jne .invalid

    ; Alle acht Slots belegen; der neunte Request muss Backpressure melden.
    xor ecx, ecx
.fill:
    cmp ecx, IO_REQUEST_CAPACITY
    jae .full
    push ecx
    mov eax, 1
    mov edx, [io_request_test_task]
    mov ebx, 2
    mov ecx, 0x101
    mov esi, 0x201
    mov edi, 512
    xor ebp, ebp
    call io_request_submit
    pop ecx
    jc .invalid
    test ecx, ecx
    jnz .filled
    mov [io_request_test_cancelled], eax
.filled:
    inc ecx
    jmp .fill
.full:
    mov eax, 1
    mov edx, [io_request_test_task]
    mov ebx, 2
    mov ecx, 0x102
    mov esi, 0x202
    mov edi, 512
    xor ebp, ebp
    call io_request_submit
    jnc .invalid
    cmp dword [io_request_backpressure_count], 1
    jne .invalid
    cmp dword [io_request_outstanding], IO_REQUEST_CAPACITY
    jne .invalid

    mov eax, [io_request_test_task]
    mov edx, 0x494F4341               ; "IOCA"
    call task_request_cancel
    jc .invalid
    cmp dword [io_request_outstanding], 0
    jne .invalid
    mov eax, [io_request_test_cancelled]
    call io_request_lookup
    jc .invalid
    cmp dword [eax + IO_REQUEST_COMPLETION], IO_COMPLETION_CANCELLED
    jne .invalid
    mov eax, [io_request_test_cancelled]
    mov edx, 512
    xor ebx, ebx
    call io_request_complete
    jnc .invalid                       ; terminale Completion bleibt eindeutig
    mov eax, [io_request_test_task]
    call task_checkpoint
    jc .invalid
    cmp eax, 1
    jne .invalid
    mov eax, [io_request_test_scope]
    call task_scope_close
    jc .invalid
    cmp dword [task_count], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
io_request_manager_api:
    dd IO_REQUEST_API_SIZE
    dw 1, 0
    dd IO_REQUEST_CAPACITY
    dd IO_CAP_ASYNC_COMPLETION | IO_CAP_BOUNDED_QUEUE | IO_CAP_TASK_OWNERSHIP | IO_CAP_DEADLINE
    dd io_request_submit
    dd io_request_complete
    dd io_request_cancel
    dd io_request_table

io_request_manager_ready:        dd 0
io_request_next_id:              dd 0
io_request_outstanding:          dd 0
io_request_backpressure_count:   dd 0
io_request_deadline_miss_count:  dd 0
io_request_poll_tick:            dd 0
io_request_temp_owner:           dd 0
io_request_temp_task:            dd 0
io_request_temp_operation:       dd 0
io_request_temp_target:          dd 0
io_request_temp_buffer:          dd 0
io_request_temp_length:          dd 0
io_request_temp_priority:        dd 0
io_request_temp_slot:            dd 0
io_request_temp_id:              dd 0
io_request_temp_transferred:     dd 0
io_request_temp_error:           dd 0
io_request_temp_record:          dd 0
io_request_temp_task_record:     dd 0
io_request_test_scope:           dd 0
io_request_test_task:            dd 0
io_request_test_deadline:        dd 0
io_request_test_first:           dd 0
io_request_test_cancelled:       dd 0
align 4
io_request_table:
    times IO_REQUEST_CAPACITY * IO_REQUEST_RECORD_SIZE db 0
io_request_submit_ticks:
    times IO_REQUEST_CAPACITY dd 0
io_request_dispatch_ticks:
    times IO_REQUEST_CAPACITY dd 0
io_request_wait_rounds:
    times IO_REQUEST_CAPACITY dd 0
io_request_qos_ids:
    times IO_REQUEST_CAPACITY dd 0

; ---------------------------------------------------------------------------
; Initialer x86-32 Userspace-Prozess (Bootphase 10)
; ---------------------------------------------------------------------------
USER_CODE_SELECTOR equ 0x1B
USER_DATA_SELECTOR equ 0x23
TSS_SELECTOR       equ 0x28
USER_CODE_ADDRESS  equ 0x00400000
; Drei Codeseiten (Explorer/Shell-Programm), direkt gefolgt von einer
; Stack-Seite ohne Luecke; siehe userspace_initialize.
USER_STACK_ADDRESS equ 0x00404000
PROCESS_FLAG_SYSTEM_SERVICE equ 0x00000002
PROCESS_FLAG_USERSPACE      equ 0x00000004
SYSCALL_ABI_VERSION         equ 0x00000001
SYSCALL_SERVICE_CORE        equ 1
SYSCALL_SERVICE_PROCESS     equ 2
SYSCALL_SERVICE_THREAD      equ 3
SYSCALL_SERVICE_IPC         equ 5
SYSCALL_SERVICE_VFS         equ 6
SYSCALL_SERVICE_POWER       equ 7
SYSCALL_SERVICE_NETWORK     equ 8
SYSCALL_SERVICE_LOGGING     equ 9
SYSCALL_SERVICE_DISPLAY     equ 12
SYSCALL_CORE_EXIT           equ 1
SYSCALL_CORE_READY          equ 2
SYSCALL_CORE_CLOSE_HANDLE   equ 3
SYSCALL_QUERY_SELF          equ 1
SYSCALL_IPC_SEND            equ 1
SYSCALL_IPC_RECEIVE         equ 2
SYSCALL_IPC_PACKET_ABI      equ 2
SYSCALL_VFS_OPEN_ROOT       equ 1
SYSCALL_VFS_LOOKUP          equ 2
SYSCALL_POWER_QUERY_SYSTEM  equ 1
SYSCALL_POWER_WAKE_ACQUIRE  equ 2
SYSCALL_POWER_WAKE_RELEASE  equ 3
SYSCALL_POWER_SET_PROFILE   equ 4
SYSCALL_POWER_REQUEST_STATE equ 5
SYSCALL_NETWORK_QUERY       equ 1
SYSCALL_NETWORK_RAW_OPEN    equ 2
SYSCALL_NETWORK_SOCKET_CREATE equ 3
SYSCALL_NETWORK_SEND        equ 4
SYSCALL_NETWORK_RECEIVE     equ 5
SYSCALL_NETWORK_BIND        equ 6
SYSCALL_NETWORK_STREAM_CREATE equ 7
SYSCALL_NETWORK_CONNECT     equ 8
SYSCALL_NETWORK_STREAM_SEND equ 9
SYSCALL_NETWORK_STREAM_RECEIVE equ 10
SYSCALL_NETWORK_STREAM_BIND equ 11
SYSCALL_NETWORK_LISTEN      equ 12
SYSCALL_NETWORK_ACCEPT      equ 13
SYSCALL_LOG_QUERY           equ 1
SYSCALL_LOG_READ_LATEST     equ 2
SYSCALL_DISPLAY_QUERY_PRIMARY equ 1
SYSCALL_DISPLAY_SUBMIT_SCENE equ 2
SYSCALL_DISPLAY_POLL_INPUT   equ 3
SYSCALL_STATUS_OK           equ 0
SYSCALL_STATUS_ABI          equ -1
SYSCALL_STATUS_SIZE         equ -4
SYSCALL_STATUS_RESERVED     equ -5
SYSCALL_STATUS_SERVICE      equ -8
SYSCALL_STATUS_OPERATION    equ -9
SYSCALL_STATUS_ACCESS       equ -13
SYSCALL_STATUS_POINTER      equ -15
SYSCALL_STATUS_WOULD_BLOCK  equ -19
SYSCALL_STATUS_TYPE         equ -22
SYSCALL_STATUS_VALIDATION   equ -23
; §011 neue Konstanten (NPSPEC-KERNEL-0011)
SYSCALL_CORE_QUERY_ABI      equ 4       ; §011 §26: ABI-Version abfragen
SYSCALL_CALL_DEPTH_MAX      equ 8       ; §011 §39: max. Verschachtelungstiefe
DISPLAY_INFO_SIZE           equ 40
SYSTEM_SCENE_SIZE           equ 64
SYSTEM_INPUT_EVENT_SIZE     equ 32
SYSTEM_INPUT_TOGGLE_START   equ 1
SYSTEM_INPUT_CLOSE_START    equ 2
SYSTEM_INPUT_FOCUS_NEXT     equ 3
SYSTEM_INPUT_ACTIVATE       equ 4
SYSTEM_INPUT_NAVIGATE_UP    equ 5
SYSTEM_INPUT_NAVIGATE_DOWN  equ 6
SYSTEM_INPUT_NAVIGATE_LEFT  equ 7
SYSTEM_INPUT_NAVIGATE_RIGHT equ 8
SYSTEM_INPUT_POINTER_ACTIVATE equ 9
SYSTEM_INPUT_NAVIGATE_BACK  equ 10
SYSTEM_INPUT_DELETE         equ 11
SYSTEM_INPUT_RENAME_KEY     equ 12
SYSTEM_INPUT_TEXT_CHAR      equ 13
DISPLAY_SCENE_DESKTOP       equ 0x00000001
DISPLAY_SCENE_START_MENU    equ 0x00000002
DISPLAY_SCENE_RIBBON        equ 0x00000004
DISPLAY_SCENE_TASKBAR       equ 0x00000008
DISPLAY_SCENE_ALLOWED_FLAGS equ DISPLAY_SCENE_DESKTOP | DISPLAY_SCENE_START_MENU | DISPLAY_SCENE_RIBBON | DISPLAY_SCENE_TASKBAR
USER_ADDRESS_MIN            equ USER_CODE_ADDRESS
USER_ADDRESS_MAX            equ USER_STACK_ADDRESS
; Direkt oberhalb der Stack-Seite (USER_ADDRESS_MAX/USER_STACK_ADDRESS),
; damit sie nicht mit dieser ueberlappt.
SHARED_SERVICE_ADDRESS      equ USER_STACK_ADDRESS
SHARED_SERVICE_SIGNATURE    equ 0x5353564E ; "NVSS"
SHARED_SERVICE_SIZE         equ 64
SHARED_FEATURE_INT80        equ 0x00000001
SHARED_FEATURE_COPY_IO      equ 0x00000002
SHARED_FEATURE_PREEMPT      equ 0x00000004
SHARED_FEATURE_DISPLAY      equ 0x00000008
SHARED_FEATURE_FILESYSTEM   equ 0x00000100
SHARED_FEATURE_FILESYSTEM_WRITABLE equ 0x00000200
SHARED_SERVICE_BITMAP       equ 0x000008F7 ; Core, Process, Thread, IPC, VFS, Power, Network, Display

userspace_initialize:
    ; TSS stellt für Ring-3-Interrupts einen kontrollierten Kernelstack bereit.
    mov edi, kernel_tss
    xor eax, eax
    mov ecx, 104 / 4
    rep stosd
    mov eax, [kernel_boot_stack_top]
    mov [kernel_tss + 4], eax
    mov word [kernel_tss + 8], DATA_SEGMENT
    mov word [kernel_tss + 102], 104
    mov eax, kernel_tss
    mov word [kernel_tss_descriptor + 0], 103
    mov word [kernel_tss_descriptor + 2], ax
    shr eax, 16
    mov byte [kernel_tss_descriptor + 4], al
    mov byte [kernel_tss_descriptor + 5], 0x89
    mov byte [kernel_tss_descriptor + 6], 0
    mov byte [kernel_tss_descriptor + 7], ah
    lgdt [kernel_gdt_descriptor]
    mov ax, TSS_SELECTOR
    ltr ax

    ; Eigene physische Seiten für Code und Stack; nur diese PTEs tragen U/S.
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [userspace_code_page], eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    mov esi, userspace_program_start
    mov edi, [userspace_code_page]
    mov ecx, PMM_PAGE_SIZE
    rep movsb
    mov eax, USER_CODE_ADDRESS
    mov edx, [userspace_code_page]
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_USER
    call paging_map_page
    jc .invalid

    ; Die interaktive System-UI belegt drei fest begrenzte Codeseiten. Alle
    ; sind user-lesbar und ausführbar, aber weiterhin nicht beschreibbar.
    ; Jede Seite wird unabhängig von der tatsächlichen Programmlänge mit
    ; einer vollen Seite aus dem Kernel-Abbild kopiert (nicht nur dem Rest,
    ; der laut userspace_program_end tatsächlich zum Programm gehört) –
    ; das vermeidet eine von der Programmlänge abhängige, möglicherweise
    ; über die Zielseite hinauslaufende Kopierlänge. Bytes hinter
    ; userspace_program_end liegen innerhalb des geladenen Kernel-Abbilds
    ; (der %error-Codebudget-Check weiter unten garantiert das) und werden
    ; nie ausgeführt.
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [userspace_code_page_2], eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    mov esi, userspace_program_start + PMM_PAGE_SIZE
    mov edi, [userspace_code_page_2]
    mov ecx, PMM_PAGE_SIZE
    rep movsb
    mov eax, USER_CODE_ADDRESS + PMM_PAGE_SIZE
    mov edx, [userspace_code_page_2]
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_USER
    call paging_map_page
    jc .invalid

    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [userspace_code_page_3], eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    mov esi, userspace_program_start + PMM_PAGE_SIZE * 2
    mov edi, [userspace_code_page_3]
    mov ecx, PMM_PAGE_SIZE
    rep movsb
    mov eax, USER_CODE_ADDRESS + PMM_PAGE_SIZE * 2
    mov edx, [userspace_code_page_3]
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_USER
    call paging_map_page
    jc .invalid

    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [userspace_stack_page], eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    mov eax, USER_STACK_ADDRESS - PMM_PAGE_SIZE
    mov edx, [userspace_stack_page]
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE | PAGING_PAGE_USER
    call paging_map_page
    jc .invalid
    mov eax, cr3
    mov cr3, eax                    ; TLB nach den neuen PTEs invalidieren

    call shared_service_page_initialize
    jc .invalid

    mov eax, PROCESS_FLAG_SYSTEM_SERVICE | PROCESS_FLAG_USERSPACE
    mov edx, cr3
    call process_create
    jc .invalid
    mov [userspace_pid], eax
    cmp eax, 2
    jne .invalid
    mov edx, SECURITY_CAP_SERVICE
    call security_grant
    jc .invalid
    mov eax, 2
    mov edx, SECURITY_CAP_IPC
    call security_grant
    jc .invalid
    mov eax, 2
    mov edx, SECURITY_CAP_POWER_QUERY | SECURITY_CAP_POWER_WAKE | SECURITY_CAP_POWER_PROFILE
    call security_grant
    jc .invalid
    mov eax, 2
    mov edx, SECURITY_CAP_NET_QUERY | SECURITY_CAP_NET_CONNECT | SECURITY_CAP_NET_LISTEN | SECURITY_CAP_LOG_READ
    call security_grant
    jc .invalid
    mov eax, 2
    mov edx, SECURITY_CAP_DISPLAY_SYSTEM_UI
    call security_grant
    jc .invalid
    ; Dateisystem: Lesen und Schreiben, aber keine System-Write-Authority.
    mov eax, 2
    mov edx, SECURITY_CAP_FS_READ | SECURITY_CAP_FS_WRITE
    call security_grant
    jc .invalid

    ; Der Bootstrap-Kernelthread (TID 1 / Scheduler-Slot 0) wird atomar zum
    ; ersten Userspace-Thread des neuen Prozesses weitergeführt.
    mov eax, 1
    call thread_lookup
    jc .invalid
    mov dword [eax + THREAD_PID], 2
    mov dword [eax + THREAD_ENTRY], USER_CODE_ADDRESS
    mov dword [eax + THREAD_CONTEXT], 0
    mov dword [userspace_tid], 1

    mov dword [handle_owner_pid], 2
    mov eax, 2
    call process_lookup
    jc .invalid
    mov edx, [eax + PROCESS_HANDLE]
    mov eax, 2
    mov ebx, OBJECT_TYPE_PROCESS
    mov ecx, HANDLE_RIGHT_QUERY | HANDLE_RIGHT_WAIT
    mov esi, HANDLE_FLAG_PROTECTED
    call handle_create
    jc .invalid
    mov [userspace_process_handle], eax
    mov eax, 1
    call thread_lookup
    jc .invalid
    mov edx, [eax + THREAD_HANDLE]
    mov eax, 2
    mov ebx, OBJECT_TYPE_THREAD
    mov ecx, HANDLE_RIGHT_QUERY | HANDLE_RIGHT_WAIT
    mov esi, HANDLE_FLAG_PROTECTED
    call handle_create
    jc .invalid
    mov [userspace_thread_handle], eax
    cmp dword [handle_active_count], 2
    jne .invalid
    mov edx, [userspace_process_handle]
    mov eax, 2
    mov ebx, OBJECT_TYPE_PROCESS
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jc .invalid
    mov edx, [userspace_process_handle]
    mov eax, 2
    mov ebx, OBJECT_TYPE_THREAD       ; Typverwechslung muss scheitern
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jnc .invalid
    call userspace_ipc_initialize
    jc .invalid
    call userspace_vfs_initialize
    jc .invalid

    ; Grenztests der öffentlichen Copy-in-Prüfung.
    mov esi, USER_ADDRESS_MIN
    mov ecx, 16
    call syscall_validate_user_range
    jc .invalid
    mov esi, USER_ADDRESS_MIN - 1
    mov ecx, 1
    call syscall_validate_user_range
    jnc .invalid
    mov esi, 0xFFFFFFF8
    mov ecx, 16
    call syscall_validate_user_range
    jnc .invalid
    mov dword [userspace_exit_seen], 0
    mov dword [userspace_ready_seen], 0
    clc
    ret
.invalid:
    stc
    ret

userspace_enter:
    cli
    mov ax, USER_DATA_SELECTOR
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push dword USER_DATA_SELECTOR
    push dword USER_STACK_ADDRESS
    push dword 0x00000202
    push dword USER_CODE_SELECTOR
    push dword USER_CODE_ADDRESS
    iretd

; Feste Stackbereiche des Ring-3-VFS-Tests (unterhalb der bisherigen Nutzung)
%define UFS_ADDR(x) (USER_CODE_ADDRESS + (x) - userspace_program_start)
UFS_H_FILE  equ USER_STACK_ADDRESS - 1300
UFS_H_DIR   equ USER_STACK_ADDRESS - 1304
UFS_H_NEW   equ USER_STACK_ADDRESS - 1308
UFS_INDEX   equ USER_STACK_ADDRESS - 1312
UFS_ARGS    equ USER_STACK_ADDRESS - 1408
UFS_DATA    equ USER_STACK_ADDRESS - 1536
UFS_ENTRY   equ USER_STACK_ADDRESS - 1856
UFS_H_BASE  equ USER_STACK_ADDRESS - 1316
UFS_FLAGS   equ USER_STACK_ADDRESS - 1320
UFS_PATH    equ USER_STACK_ADDRESS - 2176
UFS_VIEW    equ USER_STACK_ADDRESS - 2688
UFS_CWD_LEN equ USER_STACK_ADDRESS - 2692
UFS_HOME_LEN equ USER_STACK_ADDRESS - 2696
UFS_NEW_LEN equ USER_STACK_ADDRESS - 2700
UFS_NTH     equ USER_STACK_ADDRESS - 2704
UFS_H_TRASH equ USER_STACK_ADDRESS - 2708
UFS_CWD     equ USER_STACK_ADDRESS - 2968
UFS_HOME    equ USER_STACK_ADDRESS - 3032
UFS_H_ROOT  equ USER_STACK_ADDRESS - 3040
; Umbenennen (F2): Zustand des Editierpuffers. ACTIVE/TYPE/INDEX/LEN/OLDLEN
; sind je ein Dword, BUF und OLDNAME je EXPLORER_NAME_MAX (32) Byte lang.
UFS_RENAME_ACTIVE  equ USER_STACK_ADDRESS - 3044
UFS_RENAME_TYPE    equ USER_STACK_ADDRESS - 3048
UFS_RENAME_INDEX   equ USER_STACK_ADDRESS - 3052
UFS_RENAME_LEN     equ USER_STACK_ADDRESS - 3056
UFS_RENAME_OLDLEN  equ USER_STACK_ADDRESS - 3060
UFS_RENAME_BUF     equ USER_STACK_ADDRESS - 3092
UFS_RENAME_OLDNAME equ USER_STACK_ADDRESS - 3124

userspace_program_start:
    ; Process.QuerySelf -> Ergebnis auf dem beschreibbaren Userstack.
    mov eax, SYSCALL_SERVICE_PROCESS
    mov ebx, SYSCALL_QUERY_SELF
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 64
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 56], 2
    jne .failed
    mov eax, SYSCALL_SERVICE_PROCESS
    mov ebx, 2                       ; Process.OpenSelf
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 64
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 56], 0
    je .failed

    ; Thread.QuerySelf -> TID 1 des weitergeführten Bootstrap-Threads.
    mov eax, SYSCALL_SERVICE_THREAD
    mov ebx, SYSCALL_QUERY_SELF
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 32
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 24], 1
    jne .failed
    mov eax, SYSCALL_SERVICE_THREAD
    mov ebx, 2                       ; Thread.OpenSelf
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 32
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 24], 0
    je .failed

    ; Gleiche technische Bytes8-Repräsentation reicht nicht: ein anderer
    ; Semantic Type und eine inkompatible Version müssen vor Queue-Mutation
    ; zurückgewiesen werden. Die Capability-/Handle-Prüfung bleibt separat.
    mov esi, USER_CODE_ADDRESS + userspace_ipc_packet - userspace_program_start
    mov edi, USER_STACK_ADDRESS - 128
    mov ecx, 12
    rep movsd
    mov dword [USER_STACK_ADDRESS - 116], SEMANTIC_IPC_DIAGNOSTIC
    mov eax, SYSCALL_SERVICE_IPC
    mov ebx, SYSCALL_IPC_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 128
    mov esi, 48
    int 0x80
    cmp eax, SYSCALL_STATUS_TYPE
    jne .failed
    mov dword [USER_STACK_ADDRESS - 116], SEMANTIC_IPC_INLINE_DATA
    mov dword [USER_STACK_ADDRESS - 104], 2
    mov eax, SYSCALL_SERVICE_IPC
    mov ebx, SYSCALL_IPC_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 128
    mov esi, 48
    int 0x80
    cmp eax, SYSCALL_STATUS_TYPE
    jne .failed

    ; Typ und Version sind korrekt, aber ein leerer Inline-Wert verletzt die
    ; deklarierte NONEMPTY-Regel. Das ist ein eigener Validierungsfehler.
    mov dword [USER_STACK_ADDRESS - 104], SEMANTIC_TYPE_VERSION_1
    mov dword [USER_STACK_ADDRESS - 96], 0
    mov eax, SYSCALL_SERVICE_IPC
    mov ebx, SYSCALL_IPC_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 128
    mov esi, 48
    int 0x80
    cmp eax, SYSCALL_STATUS_VALIDATION
    jne .failed

    mov eax, SYSCALL_SERVICE_IPC
    mov ebx, SYSCALL_IPC_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_CODE_ADDRESS + userspace_ipc_packet - userspace_program_start
    mov esi, 48
    int 0x80
    test eax, eax
    jnz .failed
    mov eax, SYSCALL_SERVICE_IPC
    mov ebx, SYSCALL_IPC_RECEIVE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 128
    mov esi, 48
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 112], 0x11223344
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 96], 4
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 88], 0x41564F4E
    jne .failed

    mov eax, SYSCALL_SERVICE_VFS
    mov ebx, SYSCALL_VFS_OPEN_ROOT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 160
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 152], 0
    je .failed

    ; VFS.Lookup mit explizitem Pointer und Pfadlänge für "/".
    mov dword [USER_STACK_ADDRESS - 224], 32
    mov dword [USER_STACK_ADDRESS - 220], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 152]
    mov [USER_STACK_ADDRESS - 216], eax
    mov dword [USER_STACK_ADDRESS - 212], USER_CODE_ADDRESS + userspace_root_path - userspace_program_start
    mov dword [USER_STACK_ADDRESS - 208], 1
    mov dword [USER_STACK_ADDRESS - 204], 0
    mov dword [USER_STACK_ADDRESS - 200], 0
    mov dword [USER_STACK_ADDRESS - 196], 0
    mov eax, SYSCALL_SERVICE_VFS
    mov ebx, SYSCALL_VFS_LOOKUP
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 224
    mov esi, 32
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 200], 0
    je .failed
    mov dword [USER_STACK_ADDRESS - 256], 16
    mov dword [USER_STACK_ADDRESS - 252], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 200]
    mov [USER_STACK_ADDRESS - 248], eax
    mov dword [USER_STACK_ADDRESS - 244], 0
    mov eax, SYSCALL_SERVICE_CORE
    mov ebx, SYSCALL_CORE_CLOSE_HANDLE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 256
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed

    ; NovaFS ueber die VFS-Syscalls: nur bei gemountetem Systemvolume.
    ; Handles mit Rechten, Systembereichs-Policy und Persistenz werden aus
    ; Ring 3 geprueft; alle geoeffneten Handles werden wieder geschlossen.
    test dword [SHARED_SERVICE_ADDRESS + 12], SHARED_FEATURE_FILESYSTEM
    jz .fs_done
    ; A) Vom Kernel geschriebene Systemdatei nur lesend oeffnen
    mov edx, [USER_STACK_ADDRESS - 152]
    mov esi, UFS_ADDR(ufs_path_bootcount)
    mov ecx, ufs_path_bootcount_end - ufs_path_bootcount
    xor edi, edi
    call ufs_lookup
    test eax, eax
    jz .fs_have_system_file
    ; Ein noch nie beschreibbar gestartetes Volume besitzt die Datei nicht.
    cmp eax, SYSCALL_STATUS_NOT_FOUND
    jne .failed
    jmp .fs_write_part
.fs_have_system_file:
    mov [UFS_H_FILE], ebx
    mov edx, ebx
    call ufs_query
    test eax, eax
    jnz .failed
    cmp dword [UFS_ARGS + 24], NOVAFS_TYPE_FILE
    jne .failed
    cmp dword [UFS_ARGS + 32], 27
    jne .failed
    test dword [UFS_ARGS + 12], HANDLE_RIGHT_WRITE
    jnz .failed
    mov ebx, SYSCALL_VFS_READ
    mov edx, [UFS_H_FILE]
    mov esi, UFS_DATA
    mov ecx, 16
    xor edi, edi
    call ufs_io
    test eax, eax
    jnz .failed
    cmp ecx, 16
    jne .failed
    cmp dword [UFS_DATA], 'NOVA'
    jne .failed
    cmp dword [UFS_DATA + 4], 'FS-B'
    jne .failed
    ; Schreiben ohne WRITE-Recht muss scheitern
    mov ebx, SYSCALL_VFS_WRITE
    mov edx, [UFS_H_FILE]
    mov esi, UFS_DATA
    mov ecx, 4
    xor edi, edi
    call ufs_io
    cmp eax, SYSCALL_STATUS_ACCESS
    jne .failed
    mov edx, [UFS_H_FILE]
    call ufs_close
    test eax, eax
    jnz .failed
    ; Ein geschlossenes (veraltetes) Handle verleiht keinen Zugriff mehr
    mov edx, [UFS_H_FILE]
    call ufs_query
    cmp eax, SYSCALL_STATUS_ACCESS
    jne .failed
    ; Ein read-only gemountetes Volume (z. B. DIRTY) wird nur gelesen.
.fs_write_part:
    ; Arbeitsverzeichnis: /Benutzer/<erster Benutzer>/Dokumente, sonst /Benutzer
    call ufs_select_home
    test eax, eax
    jnz .failed
    ; Rootlayout aus Ring 3 pruefen: /Solutions muss als stabiler
    ; Root-Namespace auch ueber die VFS-Directory-Sicht sichtbar sein.
    mov edx, [USER_STACK_ADDRESS - 152]
    mov esi, UFS_ADDR(ufs_path_root)
    mov ecx, ufs_path_root_end - ufs_path_root
    xor edi, edi
    call ufs_lookup
    test eax, eax
    jnz .failed
    mov [UFS_H_ROOT], ebx
    mov dword [UFS_INDEX], 0
.fs_root_next:
    mov edx, [UFS_H_ROOT]
    mov ecx, [UFS_INDEX]
    call ufs_read_directory
    test eax, eax
    jnz .failed
    cmp dword [UFS_ENTRY + 20], ufs_name_solutions_end - ufs_name_solutions
    jne .fs_root_skip
    cmp dword [UFS_ENTRY + 16], NOVAFS_TYPE_DIRECTORY
    jne .failed
    mov esi, UFS_ENTRY + 32
    mov edi, UFS_ADDR(ufs_name_solutions)
    mov ecx, ufs_name_solutions_end - ufs_name_solutions
    repe cmpsb
    je .fs_root_found
.fs_root_skip:
    inc dword [UFS_INDEX]
    cmp dword [UFS_INDEX], 256
    jb .fs_root_next
    jmp .failed
.fs_root_found:
    mov edx, [UFS_H_ROOT]
    call ufs_close
    test eax, eax
    jnz .failed
    test dword [SHARED_SERVICE_ADDRESS + 12], SHARED_FEATURE_FILESYSTEM_WRITABLE
    jz .fs_view
    ; Schreibzugriff im Systembereich verweigert die Policy
    mov edx, [USER_STACK_ADDRESS - 152]
    mov esi, UFS_ADDR(ufs_path_bootcount)
    mov ecx, ufs_path_bootcount_end - ufs_path_bootcount
    mov edi, VFS_LOOKUP_FLAG_WRITE
    call ufs_lookup
    cmp eax, SYSCALL_STATUS_ACCESS
    jne .failed

    ; B) Im Arbeitsverzeichnis Datei suchen oder anlegen
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_welcome)
    mov ecx, ufs_name_welcome_end - ufs_name_welcome
    mov edi, VFS_LOOKUP_FLAG_WRITE
    call ufs_lookup
    test eax, eax
    jz .fs_have_file
    cmp eax, SYSCALL_STATUS_NOT_FOUND
    jne .failed
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_welcome)
    mov ecx, ufs_name_welcome_end - ufs_name_welcome
    mov edi, NOVAFS_TYPE_FILE
    call ufs_create
    test eax, eax
    jnz .failed
.fs_have_file:
    mov [UFS_H_NEW], ebx
    ; doppelter Name wird abgewiesen
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_welcome)
    mov ecx, ufs_name_welcome_end - ufs_name_welcome
    mov edi, NOVAFS_TYPE_FILE
    call ufs_create
    cmp eax, SYSCALL_STATUS_EXISTS
    jne .failed
    ; schreiben und zuruecklesen
    mov ebx, SYSCALL_VFS_WRITE
    mov edx, [UFS_H_NEW]
    mov esi, UFS_ADDR(ufs_welcome_text)
    mov ecx, ufs_welcome_text_end - ufs_welcome_text
    xor edi, edi
    call ufs_io
    test eax, eax
    jnz .failed
    cmp ecx, ufs_welcome_text_end - ufs_welcome_text
    jne .failed
    mov ebx, SYSCALL_VFS_READ
    mov edx, [UFS_H_NEW]
    mov esi, UFS_DATA
    mov ecx, 64
    xor edi, edi
    call ufs_io
    test eax, eax
    jnz .failed
    cmp ecx, ufs_welcome_text_end - ufs_welcome_text
    jne .failed
    mov esi, UFS_DATA
    mov edi, UFS_ADDR(ufs_welcome_text)
    repe cmpsb
    jne .failed
    ; C) Arbeitsverzeichnis enumerieren und die Datei samt Groesse finden
    mov dword [UFS_INDEX], 0
.fs_dir_next:
    mov edx, [UFS_H_DIR]
    mov ecx, [UFS_INDEX]
    call ufs_read_directory
    test eax, eax
    jnz .failed
    cmp dword [UFS_ENTRY + 20], ufs_name_welcome_end - ufs_name_welcome
    jne .fs_dir_skip
    mov esi, UFS_ENTRY + 32
    mov edi, UFS_ADDR(ufs_name_welcome)
    mov ecx, ufs_name_welcome_end - ufs_name_welcome
    repe cmpsb
    je .fs_dir_found
.fs_dir_skip:
    inc dword [UFS_INDEX]
    cmp dword [UFS_INDEX], 256
    jb .fs_dir_next
    jmp .failed
.fs_dir_found:
    cmp dword [UFS_ENTRY + 24], ufs_welcome_text_end - ufs_welcome_text
    jne .failed
    cmp dword [UFS_ENTRY + 16], NOVAFS_TYPE_FILE
    jne .failed
    mov edx, [UFS_H_NEW]
    call ufs_close
    test eax, eax
    jnz .failed
    ; E) Umbenennen, Verschieben und Loeschen samt Fehlerfaellen
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_temp)
    mov ecx, ufs_name_temp_end - ufs_name_temp
    mov edi, NOVAFS_TYPE_FILE
    call ufs_create
    test eax, eax
    jnz .failed
    mov [UFS_H_NEW], ebx
    mov ebx, SYSCALL_VFS_WRITE
    mov edx, [UFS_H_NEW]
    mov esi, UFS_ADDR(ufs_welcome_text)
    mov ecx, ufs_welcome_text_end - ufs_welcome_text
    xor edi, edi
    call ufs_io
    test eax, eax
    jnz .failed
    ; Umbenennen im selben Verzeichnis
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_temp)
    mov ecx, ufs_name_temp_end - ufs_name_temp
    mov edi, edx
    mov ebx, UFS_ADDR(ufs_name_temp2)
    mov ebp, ufs_name_temp2_end - ufs_name_temp2
    call ufs_rename
    test eax, eax
    jnz .failed
    ; ein vorhandenes Ziel wird nicht ueberschrieben
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_temp2)
    mov ecx, ufs_name_temp2_end - ufs_name_temp2
    mov edi, edx
    mov ebx, UFS_ADDR(ufs_name_welcome)
    mov ebp, ufs_name_welcome_end - ufs_name_welcome
    call ufs_rename
    cmp eax, SYSCALL_STATUS_EXISTS
    jne .failed
    ; in ein neues Unterverzeichnis verschieben
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_trash)
    mov ecx, ufs_name_trash_end - ufs_name_trash
    mov edi, NOVAFS_TYPE_DIRECTORY
    call ufs_create
    test eax, eax
    jnz .failed
    mov [UFS_H_TRASH], ebx
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_temp2)
    mov ecx, ufs_name_temp2_end - ufs_name_temp2
    mov edi, [UFS_H_TRASH]
    mov ebx, UFS_ADDR(ufs_name_temp)
    mov ebp, ufs_name_temp_end - ufs_name_temp
    call ufs_rename
    test eax, eax
    jnz .failed
    ; ein Verzeichnis kann nicht in sich selbst wandern
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_trash)
    mov ecx, ufs_name_trash_end - ufs_name_trash
    mov edi, [UFS_H_TRASH]
    mov ebx, esi
    mov ebp, ecx
    call ufs_rename
    cmp eax, SYSCALL_STATUS_VALIDATION
    jne .failed
    ; nicht leeres Verzeichnis bleibt bestehen
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_trash)
    mov ecx, ufs_name_trash_end - ufs_name_trash
    call ufs_delete
    cmp eax, SYSCALL_STATUS_NOT_EMPTY
    jne .failed
    mov edx, [UFS_H_TRASH]
    mov esi, UFS_ADDR(ufs_name_temp)
    mov ecx, ufs_name_temp_end - ufs_name_temp
    call ufs_delete
    test eax, eax
    jnz .failed
    ; das offene Handle auf die geloeschte Datei findet nichts mehr
    mov ebx, SYSCALL_VFS_READ
    mov edx, [UFS_H_NEW]
    mov esi, UFS_DATA
    mov ecx, 16
    xor edi, edi
    call ufs_io
    cmp eax, SYSCALL_STATUS_NOT_FOUND
    jne .failed
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ADDR(ufs_name_trash)
    mov ecx, ufs_name_trash_end - ufs_name_trash
    call ufs_delete
    test eax, eax
    jnz .failed
    mov edx, [UFS_H_NEW]
    call ufs_close
    test eax, eax
    jnz .failed
    mov edx, [UFS_H_TRASH]
    call ufs_close
    test eax, eax
    jnz .failed
.fs_view:
    ; D) Explorer-Ansicht aus dem Arbeitsverzeichnis an den Display Server.
    ; UFS_H_DIR bleibt als aktuelles Explorer-Verzeichnis geoeffnet.
    call ufs_present
    test eax, eax
    jnz .failed
.fs_done:

    ; Power.QuerySystem liefert ausschließlich capability-geschützte,
    ; aggregierte Kernelzustände an den Bootstrap-Prozess.
    mov eax, SYSCALL_SERVICE_POWER
    mov ebx, SYSCALL_POWER_QUERY_SYSTEM
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 320
    mov esi, 32
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 320], 32
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 316], SYSCALL_ABI_VERSION
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 312], POWER_STATE_RUNNING
    jne .failed
    test dword [USER_STACK_ADDRESS - 308], (1 << POWER_STATE_SHUTDOWN)
    jz .failed

    ; Zeitlich begrenzten Wake Lock anfordern und mit dem ausgegebenen Token
    ; wieder freigeben. Eine zweite Freigabe muss der Kernel ablehnen.
    mov dword [USER_STACK_ADDRESS - 352], 16
    mov dword [USER_STACK_ADDRESS - 348], SYSCALL_ABI_VERSION
    mov dword [USER_STACK_ADDRESS - 344], 100 ; maximal eine Sekunde
    mov dword [USER_STACK_ADDRESS - 340], 0
    mov eax, SYSCALL_SERVICE_POWER
    mov ebx, SYSCALL_POWER_WAKE_ACQUIRE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 352
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    mov eax, [USER_STACK_ADDRESS - 344]
    mov dword [USER_STACK_ADDRESS - 368], 16
    mov dword [USER_STACK_ADDRESS - 364], SYSCALL_ABI_VERSION
    mov [USER_STACK_ADDRESS - 360], eax
    mov dword [USER_STACK_ADDRESS - 356], 0
    mov eax, SYSCALL_SERVICE_POWER
    mov ebx, SYSCALL_POWER_WAKE_RELEASE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 368
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    mov eax, SYSCALL_SERVICE_POWER
    mov ebx, SYSCALL_POWER_WAKE_RELEASE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 368
    mov esi, 16
    int 0x80
    test eax, eax
    jz .failed

    mov dword [USER_STACK_ADDRESS - 384], 16
    mov dword [USER_STACK_ADDRESS - 380], SYSCALL_ABI_VERSION
    mov dword [USER_STACK_ADDRESS - 376], POWER_PROFILE_EFFICIENCY
    mov dword [USER_STACK_ADDRESS - 372], 0
    mov eax, SYSCALL_SERVICE_POWER
    mov ebx, SYSCALL_POWER_SET_PROFILE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 384
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed

    ; Den globalen Shutdown ohne POWER_SHUTDOWN-Capability anfordern. Der
    ; Kernel muss fail-closed antworten und darf keinen Übergang beginnen.
    mov dword [USER_STACK_ADDRESS - 416], 24
    mov dword [USER_STACK_ADDRESS - 412], SYSCALL_ABI_VERSION
    mov dword [USER_STACK_ADDRESS - 408], POWER_STATE_SHUTDOWN
    mov dword [USER_STACK_ADDRESS - 404], 1 ; Benutzeranforderung
    mov dword [USER_STACK_ADDRESS - 400], 0
    mov dword [USER_STACK_ADDRESS - 396], 0
    mov eax, SYSCALL_SERVICE_POWER
    mov ebx, SYSCALL_POWER_REQUEST_STATE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 416
    mov esi, 24
    int 0x80
    cmp eax, SYSCALL_STATUS_ACCESS
    jne .failed

    ; Capability-geschütztes Datagramm-Socket erstellen und ein Paket ohne
    ; ungeprüften Userspace-Pointer über lo0 senden und wieder empfangen.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_SOCKET_CREATE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 464
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    mov eax, [USER_STACK_ADDRESS - 456]
    test eax, eax
    jz .failed
    mov dword [USER_STACK_ADDRESS - 592], 28
    mov dword [USER_STACK_ADDRESS - 588], SYSCALL_ABI_VERSION
    mov [USER_STACK_ADDRESS - 584], eax
    mov dword [USER_STACK_ADDRESS - 580], NETWORK_AF_IPV4
    mov dword [USER_STACK_ADDRESS - 576], NETWORK_IPV4_LOOPBACK
    mov dword [USER_STACK_ADDRESS - 572], 9000
    mov dword [USER_STACK_ADDRESS - 568], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_BIND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 592
    mov esi, 28
    int 0x80
    test eax, eax
    jnz .failed
    mov dword [USER_STACK_ADDRESS - 512], 40
    mov dword [USER_STACK_ADDRESS - 508], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 456]
    mov [USER_STACK_ADDRESS - 504], eax
    mov dword [USER_STACK_ADDRESS - 500], 4
    mov dword [USER_STACK_ADDRESS - 496], 0
    mov dword [USER_STACK_ADDRESS - 492], 0x41564F4E ; "NOVA"
    mov dword [USER_STACK_ADDRESS - 488], 0
    mov dword [USER_STACK_ADDRESS - 484], 0
    mov dword [USER_STACK_ADDRESS - 480], 0
    mov dword [USER_STACK_ADDRESS - 476], 0
    ; Überlange Datagramme müssen vor jeder Queue-Änderung scheitern.
    mov dword [USER_STACK_ADDRESS - 500], 17
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 512
    mov esi, 40
    int 0x80
    cmp eax, SYSCALL_STATUS_SIZE
    jne .failed
    mov dword [USER_STACK_ADDRESS - 500], 4
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 512
    mov esi, 40
    int 0x80
    test eax, eax
    jnz .failed
    ; Die Queue ist auf ein Datagramm begrenzt. Ein zweites, nichtblockierendes
    ; Senden darf das erste Paket nicht überschreiben.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 512
    mov esi, 40
    int 0x80
    cmp eax, SYSCALL_STATUS_WOULD_BLOCK
    jne .failed
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_RECEIVE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 560
    mov esi, 40
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 548], 4
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 540], 0x41564F4E
    jne .failed
    ; Nach dem einzigen Empfang ist die Queue leer und muss WOULD_BLOCK melden.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_RECEIVE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 560
    mov esi, 40
    int 0x80
    cmp eax, SYSCALL_STATUS_WOULD_BLOCK
    jne .failed

    ; Separates TCP-Stream-Socket erzeugen und capability-geschuetzt mit dem
    ; lokalen Bootstrap-Endpunkt verbinden.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_CREATE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 640
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    mov eax, [USER_STACK_ADDRESS - 632]
    test eax, eax
    jz .failed
    mov dword [USER_STACK_ADDRESS - 672], 28
    mov dword [USER_STACK_ADDRESS - 668], SYSCALL_ABI_VERSION
    mov [USER_STACK_ADDRESS - 664], eax
    mov dword [USER_STACK_ADDRESS - 660], NETWORK_AF_IPV4
    mov dword [USER_STACK_ADDRESS - 656], NETWORK_IPV4_LOOPBACK
    mov dword [USER_STACK_ADDRESS - 652], NETWORK_BOOTSTRAP_PORT
    mov dword [USER_STACK_ADDRESS - 648], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_CONNECT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 672
    mov esi, 28
    int 0x80
    test eax, eax
    jnz .failed

    mov dword [USER_STACK_ADDRESS - 720], 40
    mov dword [USER_STACK_ADDRESS - 716], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 632]
    mov [USER_STACK_ADDRESS - 712], eax
    mov dword [USER_STACK_ADDRESS - 708], 4
    mov dword [USER_STACK_ADDRESS - 704], 0
    mov dword [USER_STACK_ADDRESS - 700], 0x41564F4E
    mov dword [USER_STACK_ADDRESS - 696], 0
    mov dword [USER_STACK_ADDRESS - 692], 0
    mov dword [USER_STACK_ADDRESS - 688], 0
    mov dword [USER_STACK_ADDRESS - 684], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 720
    mov esi, 40
    int 0x80
    test eax, eax
    jnz .failed
    ; Der einzelne Stream-Puffer ist voll und darf nicht ueberschrieben werden.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 720
    mov esi, 40
    int 0x80
    cmp eax, SYSCALL_STATUS_WOULD_BLOCK
    jne .failed
    mov dword [USER_STACK_ADDRESS - 768], 40
    mov dword [USER_STACK_ADDRESS - 764], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 632]
    mov [USER_STACK_ADDRESS - 760], eax
    mov dword [USER_STACK_ADDRESS - 756], 16
    mov dword [USER_STACK_ADDRESS - 752], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_RECEIVE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 768
    mov esi, 40
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 756], 4
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 748], 0x41564F4E
    jne .failed
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_RECEIVE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 768
    mov esi, 40
    int 0x80
    cmp eax, SYSCALL_STATUS_WOULD_BLOCK
    jne .failed

    ; Zweites Stream-Socket als lokalen Listener konfigurieren. Der Backlog
    ; ist fest begrenzt; Accept liefert ein separates Socket-Handle.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_CREATE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 800
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    mov eax, [USER_STACK_ADDRESS - 792]
    test eax, eax
    jz .failed
    mov dword [USER_STACK_ADDRESS - 832], 28
    mov dword [USER_STACK_ADDRESS - 828], SYSCALL_ABI_VERSION
    mov [USER_STACK_ADDRESS - 824], eax
    mov dword [USER_STACK_ADDRESS - 820], NETWORK_AF_IPV4
    mov dword [USER_STACK_ADDRESS - 816], NETWORK_IPV4_LOOPBACK
    mov dword [USER_STACK_ADDRESS - 812], NETWORK_BOOTSTRAP_PORT
    mov dword [USER_STACK_ADDRESS - 808], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_BIND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 832
    mov esi, 28
    int 0x80
    test eax, eax
    jnz .failed
    mov dword [USER_STACK_ADDRESS - 856], 20
    mov dword [USER_STACK_ADDRESS - 852], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 792]
    mov [USER_STACK_ADDRESS - 848], eax
    mov dword [USER_STACK_ADDRESS - 844], 1
    mov dword [USER_STACK_ADDRESS - 840], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_LISTEN
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 856
    mov esi, 20
    int 0x80
    test eax, eax
    jnz .failed
    mov dword [USER_STACK_ADDRESS - 880], 20
    mov dword [USER_STACK_ADDRESS - 876], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 792]
    mov [USER_STACK_ADDRESS - 872], eax
    mov dword [USER_STACK_ADDRESS - 868], 0
    mov dword [USER_STACK_ADDRESS - 864], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_ACCEPT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 880
    mov esi, 20
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 868], 0
    je .failed
    ; Die einzige ausstehende Verbindung ist verbraucht.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_ACCEPT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 880
    mov esi, 20
    int 0x80
    cmp eax, SYSCALL_STATUS_WOULD_BLOCK
    jne .failed

    ; Network.Query beschreibt nur den eigenen Standard-Namespace und das
    ; Loopback-Interface. Raw-Sockets bleiben ohne Sonderrecht geschlossen.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_QUERY
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 448
    mov esi, 96
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 448], 96
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 444], SYSCALL_ABI_VERSION
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 436], 1 ; genau lo0
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 428], NETWORK_INTERFACE_UP
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 424], NETWORK_LOOPBACK_MTU
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 416], 1 ; ein TX-Datagramm
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 412], 1 ; ein RX-Datagramm
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 408], 0 ; keine beschädigten Pakete
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 404], 9000
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 400], 1 ; ICMPv4 Echo Request/Reply getestet
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 396], 2 ; Loopback-Netz- und Hostroute
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 392], 2 ; Treffer und Unreachable-Test
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 388], 1 ; ein Treffer
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 384], 4 ; Selbsttests plus UDP/TCP-Senden
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 380], 1 ; fail-closed Test verworfen
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 376], 1 ; IPv6/UDP ::1 validiert
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 372], 1 ; ICMPv6 Echo Request/Reply
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 368], 4 ; TCP Aufbau, Daten, Retransmit, Abbau
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 364], 4 ; vier TCP-Nutzbytes gesendet
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 360], 4 ; vier TCP-Nutzbytes empfangen
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 356], 1 ; eine Verbindung akzeptiert
    jne .failed
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_RAW_OPEN
    mov ecx, SYSCALL_ABI_VERSION
    xor edx, edx
    xor esi, esi
    int 0x80
    cmp eax, SYSCALL_STATUS_ACCESS
    jne .failed

    mov eax, SYSCALL_SERVICE_LOGGING
    mov ebx, SYSCALL_LOG_QUERY
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 928
    mov esi, 32
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 928], 32
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 920], 10
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 916], 2
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 908], LOG_RING_CAPACITY
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 904], 10
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 900], 2
    jne .failed
    mov eax, SYSCALL_SERVICE_LOGGING
    mov ebx, SYSCALL_LOG_READ_LATEST
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 960
    mov esi, 32
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 952], 10
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 948], 1
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 944], 1
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 936], 0x109
    jne .failed

    ; Die feste Shared Service Page ist user-lesbar, aber nicht beschreibbar.
    cmp dword [SHARED_SERVICE_ADDRESS + 0], SHARED_SERVICE_SIGNATURE
    jne .failed
    cmp dword [SHARED_SERVICE_ADDRESS + 4], SHARED_SERVICE_SIZE
    jne .failed
    cmp dword [SHARED_SERVICE_ADDRESS + 8], SYSCALL_ABI_VERSION
    jne .failed
    cmp dword [SHARED_SERVICE_ADDRESS + 20], PMM_PAGE_SIZE
    jne .failed

    ; Der System-UI-Prozess sieht ausschließlich Displaymetadaten. Die
    ; physische Framebufferadresse bleibt hinter dem Display-Service verborgen.
    mov dword [USER_STACK_ADDRESS - 1200], 0
    mov eax, SYSCALL_SERVICE_DISPLAY
    mov ebx, SYSCALL_DISPLAY_QUERY_PRIMARY
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 1040
    mov esi, 40
    int 0x80
    test eax, eax
    jnz .display_unavailable
    cmp dword [USER_STACK_ADDRESS - 1040], 40
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 1032], 1
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 1028], 640
    jb .failed
    cmp dword [USER_STACK_ADDRESS - 1024], 480
    jb .failed

    mov esi, USER_CODE_ADDRESS + userspace_system_scene - userspace_program_start
    mov edi, USER_STACK_ADDRESS - 1120
    mov ecx, 16
    rep movsd
    mov eax, SYSCALL_SERVICE_DISPLAY
    mov ebx, SYSCALL_DISPLAY_SUBMIT_SCENE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 1120
    mov esi, 64
    int 0x80
    test eax, eax
    jnz .failed
    mov dword [USER_STACK_ADDRESS - 1200], 1
.display_unavailable:

    mov eax, SYSCALL_SERVICE_CORE
    mov ebx, SYSCALL_CORE_READY
    mov ecx, SYSCALL_ABI_VERSION
    xor edx, edx
    xor esi, esi
    int 0x80
    test eax, eax
    jnz .failed
.running:
    cmp dword [USER_STACK_ADDRESS - 1200], 1
    jne .idle
    mov eax, SYSCALL_SERVICE_DISPLAY
    mov ebx, SYSCALL_DISPLAY_POLL_INPUT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 1184
    mov esi, SYSTEM_INPUT_EVENT_SIZE
    int 0x80
    cmp eax, SYSCALL_STATUS_WOULD_BLOCK
    je .idle
    test eax, eax
    jnz .failed

    mov eax, [USER_STACK_ADDRESS - 1176] ; semantische Eingabeaktion
    cmp eax, SYSTEM_INPUT_TOGGLE_START
    je .toggle_start
    cmp eax, SYSTEM_INPUT_CLOSE_START
    je .close_start
    cmp eax, SYSTEM_INPUT_FOCUS_NEXT
    je .focus_next
    cmp eax, SYSTEM_INPUT_ACTIVATE
    je .activate
    cmp eax, SYSTEM_INPUT_NAVIGATE_UP
    je .navigate_previous
    cmp eax, SYSTEM_INPUT_NAVIGATE_LEFT
    je .navigate_previous
    cmp eax, SYSTEM_INPUT_NAVIGATE_DOWN
    je .navigate_next
    cmp eax, SYSTEM_INPUT_NAVIGATE_RIGHT
    je .navigate_next
    cmp eax, SYSTEM_INPUT_POINTER_ACTIVATE
    je .pointer_activate
    cmp eax, SYSTEM_INPUT_NAVIGATE_BACK
    je .navigate_back
    cmp eax, SYSTEM_INPUT_DELETE
    je .delete_entry
    cmp eax, SYSTEM_INPUT_RENAME_KEY
    je .rename_key
    cmp eax, SYSTEM_INPUT_TEXT_CHAR
    je .text_char
    jmp .failed
.toggle_start:
    xor dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    test dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    jz .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 2
    jmp .present_input_scene
.close_start:
    and dword [USER_STACK_ADDRESS - 1104], ~DISPLAY_SCENE_START_MENU
    jmp .present_input_scene
.focus_next:
    test dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    jz .navigate_next
    inc dword [USER_STACK_ADDRESS - 1092]
    cmp dword [USER_STACK_ADDRESS - 1092], 10
    jbe .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 2
    jmp .present_input_scene
.navigate_previous:
    test dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    jz .workspace_previous
    dec dword [USER_STACK_ADDRESS - 1092]
    cmp dword [USER_STACK_ADDRESS - 1092], 2
    jae .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 10
    jmp .present_input_scene
.workspace_previous:
    cmp dword [USER_STACK_ADDRESS - 1096], 1
    je .sheet_previous
    cmp dword [USER_STACK_ADDRESS - 1096], 2
    je .studio_previous
    ; Explorer: Dateizeilen 20..23, Ordnerkarten 24..27
    dec dword [USER_STACK_ADDRESS - 1092]
    mov eax, [USER_STACK_ADDRESS - 1092]
    sub eax, 20
    cmp eax, 7
    jbe .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 27
    jmp .present_input_scene
.sheet_previous:
    dec dword [USER_STACK_ADDRESS - 1092]
    cmp dword [USER_STACK_ADDRESS - 1092], 40
    jae .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 45
    jmp .present_input_scene
.studio_previous:
    dec dword [USER_STACK_ADDRESS - 1092]
    cmp dword [USER_STACK_ADDRESS - 1092], 60
    jae .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 65
    jmp .present_input_scene
.navigate_next:
    test dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    jnz .focus_next
    cmp dword [USER_STACK_ADDRESS - 1096], 1
    je .sheet_next
    cmp dword [USER_STACK_ADDRESS - 1096], 2
    je .studio_next
    inc dword [USER_STACK_ADDRESS - 1092]
    mov eax, [USER_STACK_ADDRESS - 1092]
    sub eax, 20
    cmp eax, 7
    jbe .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 20
    jmp .present_input_scene
.sheet_next:
    inc dword [USER_STACK_ADDRESS - 1092]
    cmp dword [USER_STACK_ADDRESS - 1092], 45
    jbe .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 40
    jmp .present_input_scene
.studio_next:
    inc dword [USER_STACK_ADDRESS - 1092]
    cmp dword [USER_STACK_ADDRESS - 1092], 65
    jbe .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 60
    jmp .present_input_scene
.pointer_activate:
    mov eax, [USER_STACK_ADDRESS - 1160]
    mov [USER_STACK_ADDRESS - 1092], eax
    test dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    jnz .activate
    cmp eax, 24                      ; Explorer-Ordner, Zurueck, Schnellzugriff
    jb .present_input_scene
.activate:
    cmp dword [UFS_RENAME_ACTIVE], 0
    jne .rename_commit
    mov eax, [USER_STACK_ADDRESS - 1092]
    test dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    jnz .activate_menu
    cmp dword [USER_STACK_ADDRESS - 1096], 0
    jne .activate_menu
    cmp eax, 24
    jb .open_file
    ; Explorer: 24..27 Ordnerkarte, 28 Zurueck, 30..36 Schnellzugriff
    cmp eax, 28
    je .navigate_back
    jb .explorer_child
    sub eax, 30
    cmp eax, 6
    ja .present_input_scene
    call ufs_enter_quick
    jmp .explorer_moved
.explorer_child:
    sub eax, 24
    call ufs_enter_child
    jmp .explorer_moved
.open_file:
    cmp eax, 20
    jb .activate_menu
    sub eax, 20
    mov ebx, eax
    call ufs_preview_nth
    jmp .present_input_scene
.delete_entry:
    ; Entf: 20..23 Dateizeile, 24..27 Ordnerkarte (nur im Explorer-Arbeitsbereich).
    test dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    jnz .present_input_scene
    cmp dword [USER_STACK_ADDRESS - 1096], 0
    jne .present_input_scene
    mov eax, [USER_STACK_ADDRESS - 1092]
    cmp eax, 20
    jb .present_input_scene
    cmp eax, 24
    jb .delete_file
    cmp eax, 28
    jae .present_input_scene
    sub eax, 24
    mov ebx, eax
    mov eax, NOVAFS_TYPE_DIRECTORY
    jmp .delete_go
.delete_file:
    sub eax, 20
    mov ebx, eax
    mov eax, NOVAFS_TYPE_FILE
.delete_go:
    call ufs_delete_nth
    test eax, eax
    jnz .present_input_scene
    call ufs_present
    jmp .present_input_scene
.rename_key:
    ; F2: startet die Umbenennung der fokussierten Datei-/Ordnerzeile (nur
    ; im Explorer-Arbeitsbereich, nicht im Startmenue). Ein zweites F2
    ; waehrend des Editierens bricht ohne Speichern ab.
    cmp dword [UFS_RENAME_ACTIVE], 0
    jne .rename_cancel
    test dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    jnz .present_input_scene
    cmp dword [USER_STACK_ADDRESS - 1096], 0
    jne .present_input_scene
    mov eax, [USER_STACK_ADDRESS - 1092]
    cmp eax, 20
    jb .present_input_scene
    cmp eax, 28
    jae .present_input_scene
    cmp eax, 24
    jb .rename_start_file
    sub eax, 24
    mov ebx, eax
    mov eax, NOVAFS_TYPE_DIRECTORY
    jmp .rename_start
.rename_start_file:
    sub eax, 20
    mov ebx, eax
    mov eax, NOVAFS_TYPE_FILE
.rename_start:
    call ufs_find_nth
    test eax, eax
    jnz .present_input_scene
    mov ecx, [UFS_ENTRY + 20]
    cmp ecx, EXPLORER_NAME_MAX
    jbe .rename_len_ok
    mov ecx, EXPLORER_NAME_MAX
.rename_len_ok:
    mov [UFS_RENAME_OLDLEN], ecx
    mov [UFS_RENAME_LEN], ecx
    mov esi, UFS_ENTRY + 32
    mov edi, UFS_RENAME_OLDNAME
    push ecx
    rep movsb
    pop ecx
    mov esi, UFS_ENTRY + 32
    mov edi, UFS_RENAME_BUF
    rep movsb
    mov dword [UFS_RENAME_ACTIVE], 1
    call ufs_rename_redraw
    jmp .present_input_scene
.rename_cancel:
    mov dword [UFS_RENAME_ACTIVE], 0
    call ufs_present
    jmp .present_input_scene
.text_char:
    ; Druckbares Zeichen (vom Kernel bereits in ASCII uebersetzt, siehe
    ; keyboard_ascii_table) nur waehrend aktiver Umbenennung anhaengen.
    cmp dword [UFS_RENAME_ACTIVE], 0
    je .present_input_scene
    mov eax, [UFS_RENAME_LEN]
    cmp eax, EXPLORER_NAME_MAX
    jae .present_input_scene
    mov edx, [USER_STACK_ADDRESS - 1172] ; Zeichen-Feld des Eingabeereignisses
    mov edi, UFS_RENAME_BUF
    add edi, eax
    mov [edi], dl
    inc dword [UFS_RENAME_LEN]
    call ufs_rename_redraw
    jmp .present_input_scene
.rename_backspace:
    cmp dword [UFS_RENAME_LEN], 0
    je .present_input_scene
    dec dword [UFS_RENAME_LEN]
    call ufs_rename_redraw
    jmp .present_input_scene
.rename_commit:
    mov dword [UFS_RENAME_ACTIVE], 0
    mov edx, [USER_STACK_ADDRESS - 152]
    mov esi, UFS_CWD
    mov ecx, [UFS_CWD_LEN]
    mov edi, VFS_LOOKUP_FLAG_WRITE
    call ufs_lookup
    test eax, eax
    jnz .rename_failed
    mov [UFS_H_NEW], ebx
    mov edx, [UFS_H_NEW]
    mov esi, UFS_RENAME_OLDNAME
    mov ecx, [UFS_RENAME_OLDLEN]
    mov edi, [UFS_H_NEW]
    mov ebx, UFS_RENAME_BUF
    mov ebp, [UFS_RENAME_LEN]
    call ufs_rename
    push eax
    mov edx, [UFS_H_NEW]
    call ufs_close
    pop eax
.rename_failed:
    call ufs_present
    jmp .present_input_scene
.navigate_back:
    cmp dword [UFS_RENAME_ACTIVE], 0
    jne .rename_backspace
    test dword [USER_STACK_ADDRESS - 1104], DISPLAY_SCENE_START_MENU
    jnz .present_input_scene
    cmp dword [USER_STACK_ADDRESS - 1096], 0
    jne .present_input_scene
    call ufs_enter_parent
.explorer_moved:
    ; Fehler (z. B. fehlender Ordner) lassen die bisherige Ansicht bestehen.
    test eax, eax
    jnz .present_input_scene
    mov dword [USER_STACK_ADDRESS - 1092], 24
    jmp .present_input_scene
.activate_menu:
    cmp eax, 3
    je .open_explorer
    cmp eax, 6
    je .open_sheet
    cmp eax, 9
    je .open_studio
    jmp .present_input_scene
.open_explorer:
    mov dword [USER_STACK_ADDRESS - 1096], 0
    mov dword [USER_STACK_ADDRESS - 1092], 20
    jmp .open_workspace
.open_sheet:
    mov dword [USER_STACK_ADDRESS - 1096], 1
    mov dword [USER_STACK_ADDRESS - 1092], 40
    jmp .open_workspace
.open_studio:
    mov dword [USER_STACK_ADDRESS - 1096], 2
    mov dword [USER_STACK_ADDRESS - 1092], 60
.open_workspace:
    and dword [USER_STACK_ADDRESS - 1104], ~DISPLAY_SCENE_START_MENU
.present_input_scene:
    inc dword [USER_STACK_ADDRESS - 1112]
    mov eax, SYSCALL_SERVICE_DISPLAY
    mov ebx, SYSCALL_DISPLAY_SUBMIT_SCENE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 1120
    mov esi, SYSTEM_SCENE_SIZE
    int 0x80
    test eax, eax
    jnz .failed
.idle:
    pause
    jmp .running
.failed:
    mov eax, SYSCALL_SERVICE_CORE
    mov ebx, SYSCALL_CORE_EXIT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_CODE_ADDRESS + userspace_exit_arguments - userspace_program_start
    mov esi, 16
    int 0x80
    ud2

userspace_exit_probe:
    mov eax, SYSCALL_SERVICE_CORE
    mov ebx, SYSCALL_CORE_EXIT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_CODE_ADDRESS + userspace_exit_arguments - userspace_program_start
    mov esi, 16
    int 0x80
    ud2                             ; eine erfolgreiche Exit-Operation kehrt nie zurück
;---------------------------------------------------------------------------
; Ring-3-Hilfsroutinen fuer den VFS-Test (Teil des Userspace-Abbilds)
;---------------------------------------------------------------------------
; EDX=Verzeichnis-Handle, ESI=Pfad, ECX=Laenge, EDI=Flags -> EAX=Status, EBX=Handle
ufs_lookup:
    mov dword [UFS_ARGS + 0], 32
    mov dword [UFS_ARGS + 4], SYSCALL_ABI_VERSION
    mov [UFS_ARGS + 8], edx
    mov [UFS_ARGS + 12], esi
    mov [UFS_ARGS + 16], ecx
    mov [UFS_ARGS + 20], edi
    mov dword [UFS_ARGS + 24], 0
    mov dword [UFS_ARGS + 28], 0
    mov ebx, SYSCALL_VFS_LOOKUP
    call ufs_invoke32
    mov ebx, [UFS_ARGS + 24]
    ret

; EBX=Read/Write, EDX=Handle, ESI=Puffer, ECX=Laenge, EDI=Offset
; -> EAX=Status, ECX=uebertragene Bytes
ufs_io:
    mov dword [UFS_ARGS + 0], 32
    mov dword [UFS_ARGS + 4], SYSCALL_ABI_VERSION
    mov [UFS_ARGS + 8], edx
    mov [UFS_ARGS + 12], esi
    mov [UFS_ARGS + 16], edi
    mov [UFS_ARGS + 20], ecx
    mov dword [UFS_ARGS + 24], 0
    mov dword [UFS_ARGS + 28], 0
    call ufs_invoke32
    mov ecx, [UFS_ARGS + 24]
    ret

; EDX=Verzeichnis-Handle, ESI=Name, ECX=Laenge, EDI=Typ -> EAX=Status, EBX=Handle
ufs_create:
    mov dword [UFS_ARGS + 0], 32
    mov dword [UFS_ARGS + 4], SYSCALL_ABI_VERSION
    mov [UFS_ARGS + 8], edx
    mov [UFS_ARGS + 12], esi
    mov [UFS_ARGS + 16], ecx
    mov [UFS_ARGS + 20], edi
    mov dword [UFS_ARGS + 24], 0
    mov dword [UFS_ARGS + 28], 0
    mov ebx, SYSCALL_VFS_CREATE
    call ufs_invoke32
    mov ebx, [UFS_ARGS + 24]
    ret

; EDX=Verzeichnis-Handle, ECX=Index -> EAX=Status, Eintrag in UFS_ENTRY
ufs_read_directory:
    mov dword [UFS_ARGS + 0], 32
    mov dword [UFS_ARGS + 4], SYSCALL_ABI_VERSION
    mov [UFS_ARGS + 8], edx
    mov [UFS_ARGS + 12], ecx
    mov dword [UFS_ARGS + 16], UFS_ENTRY
    mov dword [UFS_ARGS + 20], VFS_ENTRY_SIZE
    mov dword [UFS_ARGS + 24], 0
    mov dword [UFS_ARGS + 28], 0
    mov ebx, SYSCALL_VFS_READ_DIRECTORY
    jmp ufs_invoke32

; EDX=Handle -> EAX=Status, Ergebnis in UFS_ARGS (48 Byte)
ufs_query:
    mov edi, UFS_ARGS
    xor eax, eax
    mov ecx, VFS_INFO_SIZE / 4
    rep stosd
    mov dword [UFS_ARGS + 0], VFS_INFO_SIZE
    mov dword [UFS_ARGS + 4], SYSCALL_ABI_VERSION
    mov [UFS_ARGS + 8], edx
    mov eax, SYSCALL_SERVICE_VFS
    mov ebx, SYSCALL_VFS_QUERY
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, UFS_ARGS
    mov esi, VFS_INFO_SIZE
    int 0x80
    ret

; EDX=Handle -> EAX=Status
ufs_close:
    mov dword [UFS_ARGS + 0], 16
    mov dword [UFS_ARGS + 4], SYSCALL_ABI_VERSION
    mov [UFS_ARGS + 8], edx
    mov dword [UFS_ARGS + 12], 0
    mov eax, SYSCALL_SERVICE_CORE
    mov ebx, SYSCALL_CORE_CLOSE_HANDLE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, UFS_ARGS
    mov esi, 16
    int 0x80
    ret

; EBX=VFS-Operation mit 32-Byte-Argumenten in UFS_ARGS -> EAX=Status
ufs_invoke32:
    mov eax, SYSCALL_SERVICE_VFS
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, UFS_ARGS
    mov esi, 32
    int 0x80
    ret

; Waehlt das Arbeitsverzeichnis (UFS_H_DIR, UFS_CWD) und das Benutzerprofil
; (UFS_HOME): /Benutzer/<erster Benutzer>/Dokumente, sonst /Benutzer.
; Schreibrecht nur bei beschreibbarem Volume. EAX=Status.
ufs_select_home:
    xor eax, eax
    test dword [SHARED_SERVICE_ADDRESS + 12], SHARED_FEATURE_FILESYSTEM_WRITABLE
    jz .flags
    mov eax, VFS_LOOKUP_FLAG_WRITE
.flags:
    mov [UFS_FLAGS], eax
    mov edi, UFS_VIEW
    xor eax, eax
    mov ecx, EXPLORER_VIEW_SIZE / 4
    rep stosd
    mov esi, UFS_ADDR(ufs_path_benutzer)
    mov edi, UFS_CWD
    mov ecx, ufs_path_benutzer_end - ufs_path_benutzer
    mov [UFS_CWD_LEN], ecx
    mov [UFS_HOME_LEN], ecx
    rep movsb
    mov esi, UFS_CWD
    mov edi, UFS_HOME
    mov ecx, ufs_path_benutzer_end - ufs_path_benutzer
    rep movsb
    mov edx, [USER_STACK_ADDRESS - 152]
    mov esi, UFS_CWD
    mov ecx, ufs_path_benutzer_end - ufs_path_benutzer
    mov edi, [UFS_FLAGS]
    call ufs_lookup
    test eax, eax
    jnz .return
    mov [UFS_H_BASE], ebx
    mov [UFS_H_DIR], ebx
    mov dword [UFS_INDEX], 0
.scan:
    mov edx, [UFS_H_BASE]
    mov ecx, [UFS_INDEX]
    call ufs_read_directory
    test eax, eax
    jnz .base_only
    cmp dword [UFS_ENTRY + 16], NOVAFS_TYPE_DIRECTORY
    jne .next
    cmp dword [UFS_ENTRY + 20], 29
    ja .next
    ; "<Benutzer>/Dokumente" relativ zu /Benutzer
    mov esi, UFS_ENTRY + 32
    mov edi, UFS_PATH
    mov ecx, [UFS_ENTRY + 20]
    rep movsb
    mov esi, UFS_ADDR(ufs_suffix_documents)
    mov ecx, ufs_suffix_documents_end - ufs_suffix_documents
    rep movsb
    mov ecx, [UFS_ENTRY + 20]
    add ecx, ufs_suffix_documents_end - ufs_suffix_documents
    mov edx, [UFS_H_BASE]
    mov esi, UFS_PATH
    mov edi, [UFS_FLAGS]
    call ufs_lookup
    test eax, eax
    jnz .next
    mov [UFS_H_DIR], ebx
    ; UFS_CWD = /Benutzer/<Name>/Dokumente, UFS_HOME = /Benutzer/<Name>
    mov edi, UFS_CWD + (ufs_path_benutzer_end - ufs_path_benutzer)
    mov al, '/'
    stosb
    mov esi, UFS_ENTRY + 32
    mov ecx, [UFS_ENTRY + 20]
    rep movsb
    mov eax, edi
    sub eax, UFS_CWD
    mov [UFS_HOME_LEN], eax
    mov esi, UFS_ADDR(ufs_suffix_documents)
    mov ecx, ufs_suffix_documents_end - ufs_suffix_documents
    rep movsb
    sub edi, UFS_CWD
    mov [UFS_CWD_LEN], edi
    mov esi, UFS_CWD
    mov edi, UFS_HOME
    mov ecx, [UFS_HOME_LEN]
    rep movsb
    mov edx, [UFS_H_BASE]
    call ufs_close
    ret
.next:
    inc dword [UFS_INDEX]
    cmp dword [UFS_INDEX], 64
    jb .scan
.base_only:
    xor eax, eax
.return:
    ret

; Kandidat in UFS_PATH (ECX Bytes, absolut). Oeffnet ihn lesend; bei Erfolg
; wird er zu UFS_CWD, ersetzt UFS_H_DIR und wird angezeigt. Bei Fehlern bleibt
; die bisherige Ansicht bestehen. EAX=Status.
ufs_open_path:
    mov [UFS_NEW_LEN], ecx
    mov edx, [USER_STACK_ADDRESS - 152]
    mov esi, UFS_PATH
    xor edi, edi
    call ufs_lookup
    test eax, eax
    jnz ufs_present.return
    mov [UFS_H_NEW], ebx
    mov edx, [UFS_H_DIR]
    call ufs_close
    mov eax, [UFS_H_NEW]
    mov [UFS_H_DIR], eax
    mov esi, UFS_PATH
    mov edi, UFS_CWD
    mov ecx, [UFS_NEW_LEN]
    mov [UFS_CWD_LEN], ecx
    rep movsb
; Beschreibt UFS_CWD in UFS_VIEW (Breadcrumb, Schnellzugriff), liest UFS_H_DIR
; und uebergibt die Ansicht. Ohne Display Server wird nur uebersprungen.
ufs_present:
    ; Breadcrumb: fuehrendes '/' weglassen, '/' -> "  /  "
    mov esi, UFS_CWD + 1
    mov ecx, [UFS_CWD_LEN]
    dec ecx
    mov edi, UFS_PATH
.crumb:
    test ecx, ecx
    jle .crumb_done
    lodsb
    cmp al, '/'
    jne .crumb_store
    mov eax, '  / '
    stosd
    mov al, ' '
.crumb_store:
    stosb
    dec ecx
    jmp .crumb
.crumb_done:
    mov ecx, edi
    sub ecx, UFS_PATH
    mov esi, UFS_PATH
    mov edi, UFS_VIEW + 40
    cmp ecx, EXPLORER_PATH_MAX
    jbe .crumb_copy
    ; zu lang: ".." und das Ende des Pfads
    lea esi, [esi + ecx - (EXPLORER_PATH_MAX - 2)]
    mov ecx, EXPLORER_PATH_MAX - 2
    mov ax, '..'
    stosw
.crumb_copy:
    lea eax, [edi + ecx]
    sub eax, UFS_VIEW + 40
    mov [UFS_VIEW + 28], eax
    rep movsb
    ; Schnellzugriff: 1 = Profil, 2..7 = Profil/<Eintrag aus ufs_quick_names>
    mov dword [UFS_VIEW + 24], 0
    mov ecx, [UFS_HOME_LEN]
    cmp ecx, ufs_path_benutzer_end - ufs_path_benutzer
    jbe .view
    cmp [UFS_CWD_LEN], ecx
    jb .view
    mov esi, UFS_CWD
    mov edi, UFS_HOME
    repe cmpsb
    jne .view
    mov ecx, [UFS_CWD_LEN]
    sub ecx, [UFS_HOME_LEN]
    jnz .quick_child
    mov dword [UFS_VIEW + 24], 1
    jmp .view
.quick_child:
    cmp byte [esi], '/'
    jne .view
    inc esi
    dec ecx
    mov edi, UFS_ADDR(ufs_quick_names)
    mov ebx, 2
.quick:
    movzx edx, byte [edi]
    test edx, edx
    jz .view
    cmp edx, ecx
    jne .quick_next
    push esi
    push edi
    push ecx
    inc edi
    repe cmpsb
    pop ecx
    pop edi
    pop esi
    je .quick_found
.quick_next:
    lea edi, [edi + edx + 1]
    inc ebx
    jmp .quick
.quick_found:
    mov [UFS_VIEW + 24], ebx
.view:
    mov edx, [UFS_H_DIR]
    call ufs_build_view
    test eax, eax
    jnz .return
    mov eax, SYSCALL_SERVICE_DISPLAY
    mov ebx, SYSCALL_DISPLAY_SUBMIT_EXPLORER_VIEW
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, UFS_VIEW
    mov esi, EXPLORER_VIEW_SIZE
    int 0x80
    cmp eax, SYSCALL_STATUS_SERVICE
    jne .return
    xor eax, eax
.return:
    ret

; Explorer eine Ebene nach oben (nicht ueber /Benutzer hinaus). EAX=Status.
ufs_enter_parent:
    mov ecx, [UFS_CWD_LEN]
    cmp ecx, ufs_path_benutzer_end - ufs_path_benutzer
    jbe .top
.find:
    dec ecx
    cmp byte [UFS_CWD + ecx], '/'
    jne .find
    mov esi, UFS_CWD
    mov edi, UFS_PATH
    push ecx
    rep movsb
    pop ecx
    jmp ufs_open_path
.top:
    mov eax, SYSCALL_STATUS_NOT_FOUND
    ret

; EAX=Schnellzugriff 0..6 (Profil, Desktop, Dokumente, ...). EAX=Status.
ufs_enter_quick:
    mov ebx, eax
    mov esi, UFS_HOME
    mov edi, UFS_PATH
    mov ecx, [UFS_HOME_LEN]
    rep movsb
    test ebx, ebx
    jz .open
    mov esi, UFS_ADDR(ufs_quick_names)
.skip:
    dec ebx
    jz .append
    movzx eax, byte [esi]
    lea esi, [esi + eax + 1]
    jmp .skip
.append:
    mov al, '/'
    stosb
    movzx ecx, byte [esi]
    inc esi
    rep movsb
.open:
    mov ecx, edi
    sub ecx, UFS_PATH
    jmp ufs_open_path

; EAX=n. Oeffnet das n-te Unterverzeichnis von UFS_H_DIR. EAX=Status.
ufs_enter_child:
    mov [UFS_NTH], eax
    mov dword [UFS_INDEX], 0
.scan:
    mov edx, [UFS_H_DIR]
    mov ecx, [UFS_INDEX]
    call ufs_read_directory
    test eax, eax
    jnz .return
    inc dword [UFS_INDEX]
    cmp dword [UFS_ENTRY + 16], NOVAFS_TYPE_DIRECTORY
    jne .scan
    dec dword [UFS_NTH]
    jns .scan
    mov eax, [UFS_CWD_LEN]
    mov ecx, [UFS_ENTRY + 20]
    lea edx, [eax + ecx + 1]
    cmp edx, 255
    ja .limit
    mov esi, UFS_CWD
    mov edi, UFS_PATH
    mov ecx, eax
    rep movsb
    mov al, '/'
    stosb
    mov esi, UFS_ENTRY + 32
    mov ecx, [UFS_ENTRY + 20]
    rep movsb
    mov ecx, edx
    jmp ufs_open_path
.limit:
    mov eax, SYSCALL_STATUS_SIZE
.return:
    ret

; EDX=Verzeichnis-Handle, ESI=Name, ECX=Laenge -> EAX=Status
ufs_delete:
    mov dword [UFS_ARGS + 0], 32
    mov dword [UFS_ARGS + 4], SYSCALL_ABI_VERSION
    mov [UFS_ARGS + 8], edx
    mov [UFS_ARGS + 12], esi
    mov [UFS_ARGS + 16], ecx
    mov dword [UFS_ARGS + 20], 0
    mov dword [UFS_ARGS + 24], 0
    mov dword [UFS_ARGS + 28], 0
    mov ebx, SYSCALL_VFS_DELETE
    jmp ufs_invoke32

; EDX/ESI/ECX = Quellverzeichnis/-name/-laenge,
; EDI/EBX/EBP = Zielverzeichnis/-name/-laenge -> EAX=Status
ufs_rename:
    mov dword [UFS_ARGS + 0], VFS_RENAME_SIZE
    mov dword [UFS_ARGS + 4], SYSCALL_ABI_VERSION
    mov [UFS_ARGS + 8], edx
    mov [UFS_ARGS + 12], esi
    mov [UFS_ARGS + 16], ecx
    mov [UFS_ARGS + 20], edi
    mov [UFS_ARGS + 24], ebx
    mov [UFS_ARGS + 28], ebp
    mov dword [UFS_ARGS + 32], 0
    mov dword [UFS_ARGS + 36], 0
    mov eax, SYSCALL_SERVICE_VFS
    mov ebx, SYSCALL_VFS_RENAME
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, UFS_ARGS
    mov esi, VFS_RENAME_SIZE
    int 0x80
    ret

; EAX=NovaFS-Typ, EBX=Index -> EAX=Status (0 gefunden, Eintrag in UFS_ENTRY).
; Durchsucht UFS_H_DIR (bereits offen, nur lesend) nach dem n-ten Eintrag
; dieses Typs. Gemeinsame Suche fuer ufs_delete_nth und ufs_preview_nth.
ufs_find_nth:
    mov [UFS_FLAGS], eax
    mov [UFS_NTH], ebx
    mov dword [UFS_INDEX], 0
.scan:
    mov edx, [UFS_H_DIR]
    mov ecx, [UFS_INDEX]
    call ufs_read_directory
    test eax, eax
    jnz .return
    inc dword [UFS_INDEX]
    mov eax, [UFS_FLAGS]
    cmp [UFS_ENTRY + 16], eax
    jne .scan
    dec dword [UFS_NTH]
    jns .scan
    xor eax, eax
.return:
    ret

; EAX=NovaFS-Typ, EBX=Index (n-ter Eintrag dieses Typs im aktuellen
; Verzeichnis) -> EAX=Status. Die Explorer-Navigation oeffnet Verzeichnisse
; nur lesend (UFS_H_DIR); fuer Entf wird das Arbeitsverzeichnis (UFS_CWD)
; daher kurz mit Schreibrecht erneut geoeffnet (UFS_H_NEW, zu diesem
; Zeitpunkt frei), um den per ufs_find_nth gefundenen Namen zu loeschen.
ufs_delete_nth:
    call ufs_find_nth
    test eax, eax
    jnz .return
    mov edx, [USER_STACK_ADDRESS - 152]
    mov esi, UFS_CWD
    mov ecx, [UFS_CWD_LEN]
    mov edi, VFS_LOOKUP_FLAG_WRITE
    call ufs_lookup
    test eax, eax
    jnz .return
    mov [UFS_H_NEW], ebx
    mov edx, [UFS_H_NEW]
    mov esi, UFS_ENTRY + 32
    mov ecx, [UFS_ENTRY + 20]
    call ufs_delete
    push eax
    mov edx, [UFS_H_NEW]
    call ufs_close
    pop eax
.return:
    ret

; EBX=Index (n-te Datei im aktuellen Verzeichnis) -> EAX=Status. Zeigt
; "YYYY-MM-DD [erste 45 Byte Inhalt]" anstelle des Breadcrumbs an.
; Datum stammt aus dem VFS.Query-Timestamp (Modified), Inhalt per VFS.Read.
ufs_preview_nth:
    mov eax, NOVAFS_TYPE_FILE
    call ufs_find_nth
    test eax, eax
    jnz .return
    mov edx, [UFS_H_DIR]
    mov esi, UFS_ENTRY + 32
    mov ecx, [UFS_ENTRY + 20]
    xor edi, edi
    call ufs_lookup
    test eax, eax
    jnz .return
    mov [UFS_H_NEW], ebx
    ; Zeitstempel per VFS.Query holen
    mov edx, ebx
    call ufs_query
    ; Datum (Modified) als "YYYY-MM-DD " in UFS_VIEW + 40 formatieren
    mov eax, [UFS_ARGS + 56]        ; Modified-Timestamp (low 32 Bit)
    mov edi, UFS_VIEW + 40
    call ufs_format_date             ; schreibt 11 Byte "YYYY-MM-DD "
    ; Dateiinhalt in UFS_VIEW + 51 lesen (45 Byte nach dem Datum)
    mov ebx, SYSCALL_VFS_READ
    mov edx, [UFS_H_NEW]
    mov esi, UFS_VIEW + 51
    mov ecx, 45
    xor edi, edi
    call ufs_io
    push eax
    push ecx
    mov edx, [UFS_H_NEW]
    call ufs_close
    pop ecx
    pop eax
    test eax, eax
    jnz .return
    add ecx, 11                      ; Gesamtlänge = Datum (11) + Inhalt
    mov [UFS_VIEW + 28], ecx
    mov eax, SYSCALL_SERVICE_DISPLAY
    mov ebx, SYSCALL_DISPLAY_SUBMIT_EXPLORER_VIEW
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, UFS_VIEW
    mov esi, EXPLORER_VIEW_SIZE
    int 0x80
    xor eax, eax
.return:
    ret

; EAX=Unix-Timestamp (32-Bit), EDI=Ausgabepuffer -> 11 Byte "YYYY-MM-DD ".
; Verändert EAX, ECX, EDX; erhält EBX, ESI, EDI (nach dem Schreiben).
ufs_format_date:
    push ebx
    push esi
    push edx
    push ecx
    ; Tage seit Epoch = Timestamp / 86400
    xor edx, edx
    mov ecx, 86400
    div ecx                         ; EAX = Tage seit 1970-01-01
    ; Jahr bestimmen (EBX = laufendes Jahr, EAX = verbleibende Tage)
    mov ebx, 1970
.fd_year_loop:
    mov ecx, 365
    mov esi, ebx
    and esi, 3
    jnz .fd_not_leap
    mov ecx, 366
.fd_not_leap:
    cmp eax, ecx
    jb .fd_year_done
    sub eax, ecx
    inc ebx
    jmp .fd_year_loop
.fd_year_done:
    ; EBX = Jahr, EAX = Tag des Jahres (0-basiert)
    ; Jahr als 4 Ziffern ausgeben
    push eax
    mov eax, ebx
    xor edx, edx
    mov ecx, 1000
    div ecx
    add al, '0'
    stosb
    mov eax, edx
    xor edx, edx
    mov ecx, 100
    div ecx
    add al, '0'
    stosb
    mov eax, edx
    xor edx, edx
    mov ecx, 10
    div ecx
    add al, '0'
    stosb
    add dl, '0'
    mov al, dl
    stosb
    mov al, '-'
    stosb
    pop eax
    ; Monat aus Tabelle bestimmen
    mov esi, ufs_month_days
    mov ecx, 1
.fd_month_loop:
    movzx edx, byte [esi]
    cmp ecx, 2
    jne .fd_check_days
    ; Februar: Schaltjahr? (vereinfacht % 4 für 1970–2099)
    push eax
    mov eax, ebx
    and eax, 3
    pop eax
    jnz .fd_check_days
    mov edx, 29
.fd_check_days:
    cmp eax, edx
    jb .fd_month_done
    sub eax, edx
    inc ecx
    inc esi
    jmp .fd_month_loop
.fd_month_done:
    ; ECX = Monat (1-basiert), EAX = Tag des Monats (0-basiert)
    push eax
    mov eax, ecx
    xor edx, edx
    mov ecx, 10
    div ecx
    add al, '0'
    stosb
    add dl, '0'
    mov al, dl
    stosb
    mov al, '-'
    stosb
    pop eax
    inc eax                         ; 1-basierter Tag
    xor edx, edx
    mov ecx, 10
    div ecx
    add al, '0'
    stosb
    add dl, '0'
    mov al, dl
    stosb
    mov al, ' '
    stosb
    pop ecx
    pop edx
    pop esi
    pop ebx
    ret

ufs_month_days:
    db 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31

; Zeichnet den aktuellen Umbenennen-Puffer (UFS_RENAME_BUF/_LEN) anstelle
; des Breadcrumbs, analog zu ufs_preview_nth oben. Wird nach jeder
; Aenderung des Puffers (Start, Zeichen, Backspace) erneut aufgerufen.
ufs_rename_redraw:
    mov esi, UFS_RENAME_BUF
    mov edi, UFS_VIEW + 40
    mov ecx, [UFS_RENAME_LEN]
    rep movsb
    mov eax, [UFS_RENAME_LEN]
    mov [UFS_VIEW + 28], eax
    mov eax, SYSCALL_SERVICE_DISPLAY
    mov ebx, SYSCALL_DISPLAY_SUBMIT_EXPLORER_VIEW
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, UFS_VIEW
    mov esi, EXPLORER_VIEW_SIZE
    int 0x80
    ret

; EDX=Verzeichnis-Handle. Fuellt UFS_VIEW (Breadcrumb bereits gesetzt) mit
; hoechstens vier Ordnern und vier Dateien sowie den Gesamtzahlen. Zu lange
; Namen enden mit "..". EAX=Status.
ufs_build_view:
    mov [UFS_H_BASE], edx
    mov dword [UFS_VIEW + 0], EXPLORER_VIEW_SIZE
    mov dword [UFS_VIEW + 4], SYSCALL_ABI_VERSION
    mov dword [UFS_VIEW + 8], 1
    mov dword [UFS_VIEW + 12], 0
    mov dword [UFS_VIEW + 16], 0
    mov dword [UFS_VIEW + 20], 0
    mov dword [UFS_VIEW + 32], 0
    mov dword [UFS_VIEW + 36], 0
    mov dword [UFS_INDEX], 0
.entry:
    mov edx, [UFS_H_BASE]
    mov ecx, [UFS_INDEX]
    call ufs_read_directory
    cmp eax, SYSCALL_STATUS_NOT_FOUND
    je .done
    test eax, eax
    jnz .return
    inc dword [UFS_VIEW + 20]
    ; Typzaehler: +32 Ordner, +36 Dateien; je Typ hoechstens vier sichtbar
    mov ebx, UFS_VIEW + 36
    cmp dword [UFS_ENTRY + 16], NOVAFS_TYPE_DIRECTORY
    jne .counted
    mov ebx, UFS_VIEW + 32
.counted:
    inc dword [ebx]
    cmp dword [ebx], 4
    ja .next
    mov eax, [UFS_VIEW + 16]
    cmp eax, EXPLORER_MAX_ENTRIES
    jae .next
    imul edi, eax, EXPLORER_ENTRY_SIZE
    add edi, UFS_VIEW + EXPLORER_VIEW_HEADER
    mov eax, [UFS_ENTRY + 16]
    mov [edi + 0], eax
    mov eax, [UFS_ENTRY + 24]
    cmp dword [UFS_ENTRY + 28], 0
    je .size_ok
    mov eax, 0xFFFFFFFF
.size_ok:
    mov [edi + 4], eax
    mov ecx, [UFS_ENTRY + 20]
    mov dword [UFS_FLAGS], 0
    cmp ecx, EXPLORER_NAME_MAX
    jbe .name_ok
    mov ecx, EXPLORER_NAME_MAX
    mov dword [UFS_FLAGS], 1
.name_ok:
    mov [edi + 8], ecx
    mov dword [edi + 12], 0
    add edi, 16
    mov esi, UFS_ENTRY + 32
    rep movsb
    cmp dword [UFS_FLAGS], 0
    je .stored
    mov word [edi - 2], '..'
.stored:
    inc dword [UFS_VIEW + 16]
.next:
    inc dword [UFS_INDEX]
    cmp dword [UFS_INDEX], 256
    jb .entry
.done:
    xor eax, eax
.return:
    ret

ufs_quick_names:     db 7, "Desktop", 9, "Dokumente", 9, "Downloads", 6, "Bilder"
                     db 5, "Musik", 6, "Videos", 0
ufs_name_temp:       db "NovaOS-Test.tmp"
ufs_name_temp_end:
ufs_name_temp2:      db "NovaOS-Umbenannt.tmp"
ufs_name_temp2_end:
ufs_name_trash:      db "NovaOS-Testordner"
ufs_name_trash_end:
ufs_suffix_documents: db "/Dokumente"
ufs_suffix_documents_end:
ufs_path_bootcount: db "/System/Diagnose/novafs-bootcount"
ufs_path_bootcount_end:
ufs_path_root:      db "/"
ufs_path_root_end:
ufs_path_benutzer:  db "/Benutzer"
ufs_path_benutzer_end:
ufs_name_solutions: db "Solutions"
ufs_name_solutions_end:
ufs_name_welcome:   db "Willkommen.txt"
ufs_name_welcome_end:
ufs_welcome_text:   db "Willkommen bei NovaOS.", 13, 10
ufs_welcome_text_end:

align 4
userspace_exit_arguments:
    dd 16
    dw 1, 0
    dd 0                            ; Exit-Code
    dd 0                            ; reserviert
align 8
userspace_ipc_packet:
    dd 48
    dw SYSCALL_IPC_PACKET_ABI, 0
    dd 0                            ; Send-Endpoint, zur Laufzeit gesetzt
    dd SEMANTIC_IPC_INLINE_DATA      ; internierter Type Handle
    dq 0x0000000011223344
    dd SEMANTIC_TYPE_VERSION_1
    dd 0
    dd 4
    dd 0
    db "NOVA",0,0,0,0
userspace_root_path: db '/'
align 8
userspace_system_scene:
    dd 64
    dw 1, 0
    dq 1                            ; erste deklarative Scene-Generation
    dd 0x0000000F                   ; Desktop | Startmenü | Ribbon | Taskleiste
    dd 0                            ; systemweites Dark Theme
    dd 0                            ; Workspace 0
    dd 2                            ; Startmenü-Suche besitzt Fokus
    dd 0                            ; Surface.Primary
    dd 6                            ; Accent.Primary
    ; Sechs Reserve-Dwords (Offset 40..63), damit die Struktur exakt
    ; SYSTEM_SCENE_SIZE (64 Byte) erreicht. Fuenf davon (Offset 44..60)
    ; werden von SYSCALL_DISPLAY_SUBMIT_SCENE als Reserved==0 geprueft; mit
    ; nur fuenf Fuell-Dwords endete die Struktur bei Byte 60 und der letzte
    ; Reserved-Check (Offset 60) las bereits den naechsten Kernel-Bytewert.
    times 6 dd 0
userspace_program_end:

%if (userspace_program_end - userspace_program_start) > (PMM_PAGE_SIZE * 3)
    %error "Initialer Userspace-Code überschreitet seine drei 4-KiB-Seiten"
%endif

; ESI=Userspace-Adresse, ECX=Länge. CF=0 nur für vollständig enthaltene
; Bereiche des aktuellen Prozesses; Überläufe werden vor dem Zugriff erkannt.
syscall_validate_user_range:
    test ecx, ecx
    jz .invalid
    cmp esi, USER_ADDRESS_MIN
    jb .invalid
    mov eax, esi
    add eax, ecx
    jc .invalid
    cmp eax, esi
    jbe .invalid
    cmp eax, USER_ADDRESS_MAX
    ja .invalid
    clc
    ret
.invalid:
    stc
    ret

; Kopiert erst nach vollständiger Bereichsprüfung in Kernel-Speicher.
; ESI=Quelle, EDI=Ziel, ECX=Länge.
syscall_copy_from_user:
    push esi
    push ecx
    call syscall_validate_user_range
    pop ecx
    pop esi
    jc .invalid
    rep movsb
    clc
    ret
.invalid:
    stc
    ret

; Für öffentliche Ergebnisstrukturen gilt dieselbe vollständige Bereichsprüfung.
syscall_copy_to_user:
    push edi
    push ecx
    mov esi, edi
    call syscall_validate_user_range
    pop ecx
    pop edi
    jc .invalid
    mov esi, syscall_result_buffer
    rep movsb
    clc
    ret
.invalid:
    stc
    ret

; ESI=Kernelquelle, EDI=Userspace-Ziel, ECX=Länge.
syscall_copy_buffer_to_user:
    push esi
    push ecx
    mov esi, edi
    call syscall_validate_user_range
    pop ecx
    pop esi
    jc .invalid
    rep movsb
    clc
    ret
.invalid:
    stc
    ret

; EDX zeigt auf den von isr_common erzeugten, bereits auf dem TSS-Kernelstack
; liegenden Registerframe.
syscall_dispatch:
    mov [syscall_frame], edx
    inc dword [syscall_total]
    cmp dword [edx + 44], SYSCALL_SERVICE_PROCESS
    je .process
    cmp dword [edx + 44], SYSCALL_SERVICE_THREAD
    je .thread
    cmp dword [edx + 44], SYSCALL_SERVICE_IPC
    je .ipc
    cmp dword [edx + 44], SYSCALL_SERVICE_VFS
    je .vfs
    cmp dword [edx + 44], SYSCALL_SERVICE_POWER
    je .power
    cmp dword [edx + 44], SYSCALL_SERVICE_NETWORK
    je .network
    cmp dword [edx + 44], SYSCALL_SERVICE_LOGGING
    je .logging
    cmp dword [edx + 44], SYSCALL_SERVICE_DISPLAY
    je .display
    cmp dword [edx + 44], SYSCALL_SERVICE_CORE
    jne .unknown_service
    cmp dword [edx + 32], SYSCALL_CORE_READY
    je .core_ready
    cmp dword [edx + 32], SYSCALL_CORE_CLOSE_HANDLE
    je .core_close_handle
    cmp dword [edx + 32], SYSCALL_CORE_QUERY_ABI
    je .core_query_abi
    cmp dword [edx + 32], SYSCALL_CORE_EXIT
    jne .unknown_operation
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size

    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_SERVICE
    call security_check
    jc .access_denied

    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 16
    mov edi, syscall_argument_buffer
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_argument_buffer + 0], 16
    jb .bad_size
    movzx eax, word [syscall_argument_buffer + 4]
    cmp eax, 1
    jne .bad_abi
    cmp word [syscall_argument_buffer + 6], 0
    jb .bad_abi
    cmp dword [syscall_argument_buffer + 12], 0
    jne .bad_reserved

    mov eax, [syscall_argument_buffer + 8]
    mov [userspace_exit_code], eax
    mov dword [userspace_exit_seen], 1
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    mov dword [edx + 56], userspace_return
    mov dword [edx + 60], CODE_SEGMENT
    and dword [edx + 64], 0xFFFFCFFF
    ret
.logging:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 32
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_LOG_READ
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    cmp dword [edx + 32], SYSCALL_LOG_QUERY
    je .log_query
    cmp dword [edx + 32], SYSCALL_LOG_READ_LATEST
    je .log_read_latest
    jmp .unknown_operation
.log_query:
    mov dword [syscall_log_result + 0], 32
    mov dword [syscall_log_result + 4], SYSCALL_ABI_VERSION
    mov eax, [logging_written_records]
    mov [syscall_log_result + 8], eax
    mov eax, [logging_dropped_records]
    mov [syscall_log_result + 12], eax
    mov eax, [logging_overwritten_records]
    mov [syscall_log_result + 16], eax
    mov eax, [logging_record_count]
    mov [syscall_log_result + 20], eax
    mov eax, [logging_written_records]
    mov [syscall_log_result + 24], eax
    mov eax, [logging_critical_records]
    mov [syscall_log_result + 28], eax
    mov esi, message_logging_query_ok
    jmp .log_copy
.log_read_latest:
    cmp dword [logging_record_count], 0
    je .log_would_block
    mov eax, [logging_write_index]
    dec eax
    and eax, LOG_RING_CAPACITY - 1
    shl eax, 5
    add eax, logging_ring
    cmp dword [eax + 28], LOG_COMMIT_MARKER
    jne .unknown_operation
    mov dword [syscall_log_result + 0], 32
    mov dword [syscall_log_result + 4], SYSCALL_ABI_VERSION
    mov ecx, [eax + 0]
    mov [syscall_log_result + 8], ecx
    mov ecx, [eax + 4]
    mov [syscall_log_result + 12], ecx
    mov ecx, [eax + 8]
    mov [syscall_log_result + 16], ecx
    mov ecx, [eax + 12]
    mov [syscall_log_result + 20], ecx
    mov ecx, [eax + 16]
    mov [syscall_log_result + 24], ecx
    mov ecx, [eax + 24]
    mov [syscall_log_result + 28], ecx
    mov esi, message_logging_read_ok
.log_copy:
    push esi
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_log_result
    mov ecx, 32
    call syscall_copy_buffer_to_user
    jc .log_bad_pointer
    pop esi
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.display:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_DISPLAY_SYSTEM_UI
    call security_check
    jc .access_denied
    cmp dword [display_server_ready], 1
    jne .display_unavailable
    mov edx, [syscall_frame]
    cmp dword [edx + 32], SYSCALL_DISPLAY_QUERY_PRIMARY
    je .display_query
    cmp dword [edx + 32], SYSCALL_DISPLAY_SUBMIT_SCENE
    je .display_submit
    cmp dword [edx + 32], SYSCALL_DISPLAY_POLL_INPUT
    je .display_poll_input
    cmp dword [edx + 32], SYSCALL_DISPLAY_SUBMIT_EXPLORER_VIEW
    jne .unknown_operation
    call explorer_submit_view
    mov edx, [syscall_frame]
    mov [edx + 44], eax
    test eax, eax
    jz .explorer_done
    inc dword [syscall_rejected]
.explorer_done:
    ret
.display_query:
    cmp dword [edx + 20], DISPLAY_INFO_SIZE
    jb .bad_size
    mov dword [syscall_display_info + 0], DISPLAY_INFO_SIZE
    mov dword [syscall_display_info + 4], SYSCALL_ABI_VERSION
    mov eax, [display_primary_id]
    mov [syscall_display_info + 8], eax
    mov eax, [kernel_context + CONTEXT_WIDTH]
    mov [syscall_display_info + 12], eax
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    mov [syscall_display_info + 16], eax
    mov eax, [kernel_context + CONTEXT_PITCH]
    mov [syscall_display_info + 20], eax
    mov eax, [kernel_context + CONTEXT_BPP]
    mov [syscall_display_info + 24], eax
    mov dword [syscall_display_info + 28], 1000 ; DLU-Skalierung
    mov eax, [display_generation]
    mov [syscall_display_info + 32], eax
    mov dword [syscall_display_info + 36], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_display_info
    mov ecx, DISPLAY_INFO_SIZE
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_display_query_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.display_submit:
    cmp dword [edx + 20], SYSTEM_SCENE_SIZE
    jb .bad_size
    mov esi, [edx + 36]
    mov edi, syscall_display_scene
    mov ecx, SYSTEM_SCENE_SIZE
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_display_scene + 0], SYSTEM_SCENE_SIZE
    jne .bad_size
    cmp dword [syscall_display_scene + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_display_scene + 12], 0
    jne .bad_reserved
    mov eax, [syscall_display_scene + 8]
    cmp eax, [display_scene_generation]
    jbe .display_validation
    mov ecx, [syscall_display_scene + 16]
    test ecx, ~DISPLAY_SCENE_ALLOWED_FLAGS
    jnz .display_validation
    mov ebx, ecx
    and ebx, DISPLAY_SCENE_DESKTOP | DISPLAY_SCENE_RIBBON | DISPLAY_SCENE_TASKBAR
    cmp ebx, DISPLAY_SCENE_DESKTOP | DISPLAY_SCENE_RIBBON | DISPLAY_SCENE_TASKBAR
    jne .display_validation
    cmp dword [syscall_display_scene + 20], 2
    ja .display_validation
    cmp dword [syscall_display_scene + 24], 31
    ja .display_validation
    cmp dword [syscall_display_scene + 32], 13
    jae .display_validation
    cmp dword [syscall_display_scene + 36], 13
    jae .display_validation
    cmp dword [syscall_display_scene + 44], 0
    jne .bad_reserved
    cmp dword [syscall_display_scene + 48], 0
    jne .bad_reserved
    cmp dword [syscall_display_scene + 52], 0
    jne .bad_reserved
    cmp dword [syscall_display_scene + 56], 0
    jne .bad_reserved
    cmp dword [syscall_display_scene + 60], 0
    jne .bad_reserved
    mov [display_scene_generation], eax
    mov ebx, [display_scene_flags]
    mov edi, [display_scene_workspace]
    mov [display_scene_flags], ecx
    mov esi, [syscall_display_scene + 24]
    mov [display_scene_workspace], esi
    mov edx, [display_scene_focus]
    mov eax, [syscall_display_scene + 28]
    mov [display_scene_focus], eax
    ; Sichtbarkeits- und Workspacewechsel erzeugen einen Vollframe. Ein reiner
    ; Fokuswechsel zeichnet dagegen nur das betroffene Startmenue oder Fenster.
    cmp ebx, ecx
    jne .display_redraw
    cmp edi, esi
    jne .display_redraw
    cmp edx, eax
    je .display_presented
    test ecx, DISPLAY_SCENE_START_MENU
    jz .display_focus_workspace
    mov byte [mouse_cursor_valid], 0
    call draw_shell_start_menu
    jmp .display_redrawn
.display_focus_workspace:
    mov byte [mouse_cursor_valid], 0
    call draw_shell_workspace
    jmp .display_redrawn
.display_redraw:
    mov byte [mouse_cursor_valid], 0
    call draw_desktop_scene
.display_redrawn:
    cmp byte [mouse_ready], 1
    jne .display_presented
    call draw_mouse_cursor
.display_presented:
    inc dword [display_present_count]
    inc dword [display_generation]
    mov esi, message_display_scene_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.display_poll_input:
    cmp dword [edx + 20], SYSTEM_INPUT_EVENT_SIZE
    jb .bad_size
    cmp dword [display_input_pending], 1
    jne .display_input_empty
    mov dword [syscall_display_input + 0], SYSTEM_INPUT_EVENT_SIZE
    mov dword [syscall_display_input + 4], SYSCALL_ABI_VERSION
    mov eax, [display_input_action]
    mov [syscall_display_input + 8], eax
    mov eax, [display_input_scancode]
    mov [syscall_display_input + 12], eax
    mov eax, [display_input_tick]
    mov [syscall_display_input + 16], eax
    mov dword [syscall_display_input + 20], 0
    mov eax, [display_input_target]
    mov [syscall_display_input + 24], eax
    mov dword [syscall_display_input + 28], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_display_input
    mov ecx, SYSTEM_INPUT_EVENT_SIZE
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    pushfd
    cli
    mov dword [display_input_pending], 0
    popfd
    mov esi, message_display_input_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.display_input_empty:
    mov eax, SYSCALL_STATUS_WOULD_BLOCK
    jmp .reject
.display_validation:
    mov eax, SYSCALL_STATUS_VALIDATION
    jmp .reject
.display_unavailable:
    mov eax, SYSCALL_STATUS_SERVICE
    jmp .reject
.log_bad_pointer:
    pop esi
    jmp .bad_pointer
.log_would_block:
    mov eax, SYSCALL_STATUS_WOULD_BLOCK
    jmp .reject
.core_query_abi:
    ; §011 §26: ABI-Version und Feature-Flags abfragen
    call syscall_handler_query_abi
    ret

.core_ready:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 0
    jne .bad_size
    cmp dword [edx + 36], 0
    jne .bad_pointer
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_SERVICE
    call security_check
    jc .access_denied
    mov dword [userspace_ready_seen], 1
    mov esi, message_shared_service_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.core_close_handle:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov esi, [edx + 36]
    mov ecx, 16
    mov edi, syscall_argument_buffer
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_argument_buffer + 0], 16
    jne .bad_size
    cmp dword [syscall_argument_buffer + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_argument_buffer + 12], 0
    jne .bad_reserved
    mov edx, [syscall_argument_buffer + 8]
    mov [syscall_closed_handle], edx
    mov eax, [userspace_pid]
    call handle_close
    jc .access_denied
    ; Derselbe codierte Wert muss nach Generationswechsel ungültig sein.
    mov eax, [userspace_pid]
    mov edx, [syscall_closed_handle]
    mov ebx, OBJECT_TYPE_VFS_NODE
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jnc .bad_reserved
    mov esi, message_handle_close_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.process:
    cmp dword [edx + 32], 2
    je .process_handle
    cmp dword [edx + 32], SYSCALL_QUERY_SELF
    jne .unknown_operation
    mov eax, [userspace_pid]
    mov esi, message_process_syscall_ok
    jmp .process_value
.process_handle:
    mov eax, [userspace_process_handle]
    mov esi, message_process_handle_ok
.process_value:
    mov [syscall_identity], eax
    jmp .identity
.thread:
    cmp dword [edx + 32], 2
    je .thread_handle
    cmp dword [edx + 32], SYSCALL_QUERY_SELF
    jne .unknown_operation
    mov eax, [userspace_tid]
    mov esi, message_thread_syscall_ok
    jmp .thread_value
.thread_handle:
    mov eax, [userspace_thread_handle]
    mov esi, message_thread_handle_ok
.thread_value:
    mov [syscall_identity], eax
.identity:
    push esi
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .identity_bad_abi
    cmp dword [edx + 20], 16
    jb .identity_bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_SERVICE
    call security_check
    jc .identity_access
    mov dword [syscall_result_buffer + 0], 16
    mov dword [syscall_result_buffer + 4], SYSCALL_ABI_VERSION
    mov eax, [syscall_identity]
    mov [syscall_result_buffer + 8], eax
    mov dword [syscall_result_buffer + 12], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov ecx, 16
    call syscall_copy_to_user
    jc .identity_pointer
    pop esi
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.identity_bad_abi:
    pop esi
    jmp .bad_abi
.identity_bad_size:
    pop esi
    jmp .bad_size
.identity_access:
    pop esi
    jmp .access_denied
.identity_pointer:
    pop esi
    jmp .bad_pointer
.ipc:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 48
    jb .bad_size
    cmp dword [edx + 32], SYSCALL_IPC_SEND
    je .ipc_send
    cmp dword [edx + 32], SYSCALL_IPC_RECEIVE
    je .ipc_receive
    jmp .unknown_operation
.ipc_send:
    mov esi, [edx + 36]
    mov ecx, 48
    mov edi, userspace_ipc_copy
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [userspace_ipc_copy], 48
    jne .bad_size
    cmp dword [userspace_ipc_copy + 4], SYSCALL_IPC_PACKET_ABI
    jne .bad_abi
    cmp dword [userspace_ipc_copy + 32], 8
    ja .bad_size
    cmp dword [userspace_ipc_copy + 36], 0
    jne .bad_reserved
    cmp dword [userspace_ipc_copy + 28], 0
    jne .bad_reserved
    mov eax, [userspace_pid]
    mov edx, [userspace_ipc_copy + 8]
    mov ebx, OBJECT_TYPE_IPC_ENDPOINT
    mov ecx, HANDLE_RIGHT_SEND
    call handle_resolve
    jc .access_denied
    mov eax, [eax + OBJ_HANDLE]
    call object_semantic_query
    jc .semantic_mismatch
    cmp ebx, SEMANTIC_VALIDATION_VERIFIED
    jne .semantic_mismatch
    push ecx                       ; Endpoint-Contract, nicht User-Claim
    mov eax, [userspace_ipc_copy + 12]
    mov ecx, [userspace_ipc_copy + 24]
    mov edx, SEMANTIC_REPR_BYTES8
    call semantic_check_claim
    pop edx
    jc .semantic_mismatch
    cmp eax, edx
    jne .semantic_mismatch
    mov eax, [userspace_ipc_copy + 12]
    mov ecx, [userspace_ipc_copy + 24]
    mov edx, userspace_ipc_copy + 40
    mov ebx, [userspace_ipc_copy + 32]
    mov esi, 8                       ; Endpoint-Capability-Contract
    call semantic_validate_value
    jc .semantic_validation_failed
    cmp dword [userspace_ipc_count], 0
    jne .ipc_queue_full
    mov esi, userspace_ipc_copy
    mov edi, userspace_ipc_queue
    mov ecx, 12
    rep movsd
    mov dword [userspace_ipc_count], 1
    mov dword [userspace_ipc_validation_state], SEMANTIC_VALIDATION_VERIFIED
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.ipc_receive:
    cmp dword [userspace_ipc_count], 1
    jne .ipc_would_block
    mov eax, [userspace_pid]
    mov edx, [userspace_ipc_receive_handle]
    mov ebx, OBJECT_TYPE_IPC_ENDPOINT
    mov ecx, HANDLE_RIGHT_RECEIVE
    call handle_resolve
    jc .access_denied
    mov eax, [eax + OBJ_HANDLE]
    call object_semantic_query
    jc .semantic_mismatch
    cmp ebx, SEMANTIC_VALIDATION_VERIFIED
    jne .semantic_mismatch
    mov ebx, ecx
    cmp dword [userspace_ipc_validation_state], SEMANTIC_VALIDATION_VERIFIED
    jne .semantic_mismatch
    mov eax, [userspace_ipc_queue + 12]
    cmp eax, ebx
    jne .semantic_mismatch
    mov ecx, [userspace_ipc_queue + 24]
    mov edx, SEMANTIC_REPR_BYTES8
    call semantic_check_claim
    jc .semantic_mismatch
    mov esi, userspace_ipc_queue
    mov edi, userspace_ipc_copy
    mov ecx, 12
    rep movsd
    mov eax, [userspace_ipc_receive_handle]
    mov [userspace_ipc_copy + 8], eax
    mov dword [userspace_ipc_count], 0
    mov dword [userspace_ipc_validation_state], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, userspace_ipc_copy
    mov ecx, 48
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_ipc_roundtrip_ok
    call serial_write_string
    cmp dword [semantic_ipc_rejected], 2
    jne .ipc_receive_done
    mov esi, message_semantic_reject_ok
    call serial_write_string
.ipc_validation_report:
    cmp dword [semantic_ipc_validation_rejected], 1
    jne .ipc_receive_done
    mov esi, message_semantic_validation_ok
    call serial_write_string
.ipc_receive_done:
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.ipc_queue_full:
    mov eax, -17
    jmp .reject
.ipc_would_block:
    mov eax, -19
    jmp .reject
.vfs:
    cmp dword [edx + 32], SYSCALL_VFS_OPEN_ROOT
    je .vfs_open_root
    ; Lookup, Read, Write, Create, ReadDirectory, Query, Delete, Rename (vfs32.inc)
    call vfs_syscall
    mov edx, [syscall_frame]
    mov [edx + 44], eax
    test eax, eax
    jz .vfs_done
    inc dword [syscall_rejected]
.vfs_done:
    ret
.vfs_open_root:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_SERVICE
    call security_check
    jc .access_denied
    mov dword [syscall_result_buffer + 0], 16
    mov dword [syscall_result_buffer + 4], SYSCALL_ABI_VERSION
    mov eax, [userspace_root_handle]
    mov [syscall_result_buffer + 8], eax
    mov dword [syscall_result_buffer + 12], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_result_buffer
    mov ecx, 16
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_vfs_userspace_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.power:
    cmp dword [edx + 32], SYSCALL_POWER_QUERY_SYSTEM
    je .power_query
    cmp dword [edx + 32], SYSCALL_POWER_WAKE_ACQUIRE
    je .power_wake_acquire
    cmp dword [edx + 32], SYSCALL_POWER_WAKE_RELEASE
    je .power_wake_release
    cmp dword [edx + 32], SYSCALL_POWER_SET_PROFILE
    je .power_set_profile
    cmp dword [edx + 32], SYSCALL_POWER_REQUEST_STATE
    je .power_request_state
    jmp .unknown_operation
.network:
    cmp dword [edx + 32], SYSCALL_NETWORK_QUERY
    je .network_query
    cmp dword [edx + 32], SYSCALL_NETWORK_RAW_OPEN
    je .network_raw_open
    cmp dword [edx + 32], SYSCALL_NETWORK_SOCKET_CREATE
    je .network_socket_create
    cmp dword [edx + 32], SYSCALL_NETWORK_SEND
    je .network_send
    cmp dword [edx + 32], SYSCALL_NETWORK_RECEIVE
    je .network_receive
    cmp dword [edx + 32], SYSCALL_NETWORK_BIND
    je .network_bind
    cmp dword [edx + 32], SYSCALL_NETWORK_STREAM_CREATE
    je .network_stream_create
    cmp dword [edx + 32], SYSCALL_NETWORK_CONNECT
    je .network_connect
    cmp dword [edx + 32], SYSCALL_NETWORK_STREAM_SEND
    je .network_stream_send
    cmp dword [edx + 32], SYSCALL_NETWORK_STREAM_RECEIVE
    je .network_stream_receive
    cmp dword [edx + 32], SYSCALL_NETWORK_STREAM_BIND
    je .network_stream_bind
    cmp dword [edx + 32], SYSCALL_NETWORK_LISTEN
    je .network_listen
    cmp dword [edx + 32], SYSCALL_NETWORK_ACCEPT
    je .network_accept
    jmp .unknown_operation
.network_query:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 96
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_QUERY
    call security_check
    jc .access_denied
    mov dword [syscall_network_result + 0], 96
    mov dword [syscall_network_result + 4], SYSCALL_ABI_VERSION
    mov eax, [network_namespace_generation]
    mov [syscall_network_result + 8], eax
    mov dword [syscall_network_result + 12], 1
    mov eax, [network_loopback_id]
    mov [syscall_network_result + 16], eax
    mov eax, [network_loopback_state]
    mov [syscall_network_result + 20], eax
    mov dword [syscall_network_result + 24], NETWORK_LOOPBACK_MTU
    mov dword [syscall_network_result + 28], NETWORK_FEATURE_IPV4 | NETWORK_FEATURE_IPV6
    mov eax, [network_transmitted_packets]
    mov [syscall_network_result + 32], eax
    mov eax, [network_received_packets]
    mov [syscall_network_result + 36], eax
    mov eax, [network_dropped_packets]
    mov [syscall_network_result + 40], eax
    mov eax, [network_socket_port]
    mov [syscall_network_result + 44], eax
    mov eax, [network_icmp_echo_tests]
    mov [syscall_network_result + 48], eax
    mov eax, [network_route_count]
    mov [syscall_network_result + 52], eax
    mov eax, [network_route_lookups]
    mov [syscall_network_result + 56], eax
    mov eax, [network_route_hits]
    mov [syscall_network_result + 60], eax
    mov eax, [network_firewall_decisions]
    mov [syscall_network_result + 64], eax
    mov eax, [network_firewall_drops]
    mov [syscall_network_result + 68], eax
    mov eax, [network_ipv6_udp_tests]
    mov [syscall_network_result + 72], eax
    mov eax, [network_icmpv6_echo_tests]
    mov [syscall_network_result + 76], eax
    mov eax, [network_tcp_tests]
    mov [syscall_network_result + 80], eax
    mov eax, [network_tcp_transmitted_bytes]
    mov [syscall_network_result + 84], eax
    mov eax, [network_tcp_received_bytes]
    mov [syscall_network_result + 88], eax
    mov eax, [network_tcp_accepted_connections]
    mov [syscall_network_result + 92], eax
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_network_result
    mov ecx, 96
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_network_query_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_raw_open:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 0
    jne .bad_size
    cmp dword [edx + 36], 0
    jne .bad_pointer
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_RAW
    call security_check
    jc .network_raw_denied
    jmp .unknown_operation
.network_raw_denied:
    mov esi, message_network_raw_denied
    call serial_write_string
    jmp .access_denied
.network_socket_create:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_CONNECT
    call security_check
    jc .access_denied
    mov eax, OBJECT_TYPE_NET_SOCKET
    mov edx, NETWORK_SOCKET_DATAGRAM
    mov ebx, 1                       ; Namespace 1
    call object_create
    jc .unknown_operation
    mov [network_socket_object], eax
    mov edx, eax
    mov eax, [userspace_pid]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_QUERY | HANDLE_RIGHT_SEND | HANDLE_RIGHT_RECEIVE | HANDLE_RIGHT_WAIT | HANDLE_RIGHT_BIND
    xor esi, esi
    call handle_create
    jc .unknown_operation
    mov [network_socket_handle], eax
    mov dword [syscall_result_buffer], 16
    mov dword [syscall_result_buffer + 4], SYSCALL_ABI_VERSION
    mov [syscall_result_buffer + 8], eax
    mov dword [syscall_result_buffer + 12], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_result_buffer
    mov ecx, 16
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_network_socket_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_stream_create:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_CONNECT
    call security_check
    jc .access_denied
    mov eax, OBJECT_TYPE_NET_SOCKET
    mov edx, NETWORK_SOCKET_STREAM
    mov ebx, 1
    call object_create
    jc .unknown_operation
    mov [network_tcp_socket_object], eax
    mov edx, eax
    mov eax, [userspace_pid]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_QUERY | HANDLE_RIGHT_SEND | HANDLE_RIGHT_RECEIVE | HANDLE_RIGHT_WAIT | HANDLE_RIGHT_BIND | HANDLE_RIGHT_CONNECT
    xor esi, esi
    call handle_create
    jc .unknown_operation
    mov [network_tcp_socket_handle], eax
    mov dword [syscall_result_buffer], 16
    mov dword [syscall_result_buffer + 4], SYSCALL_ABI_VERSION
    mov [syscall_result_buffer + 8], eax
    mov dword [syscall_result_buffer + 12], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_result_buffer
    mov ecx, 16
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_network_stream_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_connect:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 28
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_CONNECT
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 28
    mov edi, syscall_network_connect
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_connect], 28
    jne .bad_size
    cmp dword [syscall_network_connect + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_network_connect + 12], NETWORK_AF_IPV4
    jne .unknown_operation
    cmp dword [syscall_network_connect + 16], NETWORK_IPV4_LOOPBACK
    jne .unknown_operation
    cmp dword [syscall_network_connect + 20], NETWORK_BOOTSTRAP_PORT
    jne .unknown_operation
    cmp dword [syscall_network_connect + 24], 0
    jne .bad_reserved
    mov eax, [userspace_pid]
    mov edx, [syscall_network_connect + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_CONNECT
    call handle_resolve
    jc .network_connect_handle_denied
    mov eax, 0x7F000001             ; Firewall erwartet Host-Reihenfolge
    mov ebx, 6
    mov ecx, NETWORK_BOOTSTRAP_PORT
    call network_firewall_check_ipv4
    jc .network_connect_firewall_denied
    mov dword [network_tcp_state], TCP_STATE_CLOSED
    mov eax, TCP_STATE_SYN_SENT
    call network_tcp_transition
    jc .network_connect_state_error
    mov eax, TCP_STATE_SYN_RECEIVED
    call network_tcp_transition
    jc .network_connect_state_error
    mov eax, TCP_STATE_ESTABLISHED
    call network_tcp_transition
    jc .network_connect_state_error
    mov dword [network_tcp_socket_connected], 1
    mov esi, message_network_connect_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_connect_handle_denied:
    mov esi, message_network_connect_handle_denied
    call serial_write_string
    jmp .access_denied
.network_connect_firewall_denied:
    mov esi, message_network_connect_firewall_denied
    call serial_write_string
    jmp .access_denied
.network_connect_state_error:
    mov esi, message_network_connect_state_error
    call serial_write_string
    jmp .unknown_operation
.network_stream_send:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 40
    jb .bad_size
    mov esi, [edx + 36]
    mov ecx, 40
    mov edi, syscall_network_tcp_packet
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_tcp_packet], 40
    jne .bad_size
    cmp dword [syscall_network_tcp_packet + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    mov ecx, [syscall_network_tcp_packet + 12]
    test ecx, ecx
    jz .bad_size
    cmp ecx, 16
    ja .network_packet_too_large
    cmp dword [syscall_network_tcp_packet + 16], 0
    jne .bad_reserved
    cmp dword [syscall_network_tcp_packet + 36], 0
    jne .bad_reserved
    cmp dword [network_tcp_socket_connected], 1
    jne .unknown_operation
    cmp dword [network_tcp_state], TCP_STATE_ESTABLISHED
    jne .unknown_operation
    cmp dword [network_tcp_queue_count], 0
    jne .network_would_block
    mov eax, [userspace_pid]
    mov edx, [syscall_network_tcp_packet + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_SEND
    call handle_resolve
    jc .access_denied
    mov ecx, [syscall_network_tcp_packet + 12]
    mov [network_tcp_queue_length], ecx
    mov esi, syscall_network_tcp_packet + 20
    mov edi, network_tcp_queue_payload
    rep movsb
    mov dword [network_tcp_queue_count], 1
    mov eax, [network_tcp_queue_length]
    add [network_tcp_transmitted_bytes], eax
    mov esi, message_network_stream_send_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_stream_receive:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 40
    jb .bad_size
    mov esi, [edx + 36]
    mov ecx, 40
    mov edi, syscall_network_tcp_packet
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_tcp_packet], 40
    jne .bad_size
    cmp dword [syscall_network_tcp_packet + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [network_tcp_queue_count], 1
    jne .network_would_block
    mov eax, [userspace_pid]
    mov edx, [syscall_network_tcp_packet + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_RECEIVE
    call handle_resolve
    jc .access_denied
    mov eax, [network_tcp_queue_length]
    cmp eax, [syscall_network_tcp_packet + 12]
    ja .bad_size
    mov [syscall_network_tcp_packet + 12], eax
    mov dword [syscall_network_tcp_packet + 16], 0
    mov ecx, eax
    mov esi, network_tcp_queue_payload
    mov edi, syscall_network_tcp_packet + 20
    rep movsb
    add [network_tcp_received_bytes], eax
    mov dword [network_tcp_queue_count], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_network_tcp_packet
    mov ecx, 40
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_network_stream_receive_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_stream_bind:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 28
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_LISTEN
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 28
    mov edi, syscall_network_stream_bind
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_stream_bind], 28
    jne .bad_size
    cmp dword [syscall_network_stream_bind + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_network_stream_bind + 12], NETWORK_AF_IPV4
    jne .unknown_operation
    cmp dword [syscall_network_stream_bind + 16], NETWORK_IPV4_LOOPBACK
    jne .unknown_operation
    cmp dword [syscall_network_stream_bind + 20], NETWORK_BOOTSTRAP_PORT
    jne .unknown_operation
    cmp dword [syscall_network_stream_bind + 24], 0
    jne .bad_reserved
    cmp dword [network_tcp_listener_bound], 0
    jne .unknown_operation
    mov eax, [userspace_pid]
    mov edx, [syscall_network_stream_bind + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_BIND
    call handle_resolve
    jc .access_denied
    mov eax, [syscall_network_stream_bind + 8]
    mov [network_tcp_listener_handle], eax
    mov dword [network_tcp_listener_bound], 1
    inc dword [network_namespace_generation]
    mov esi, message_network_stream_bind_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_listen:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 20
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_LISTEN
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 20
    mov edi, syscall_network_listen
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_listen], 20
    jne .bad_size
    cmp dword [syscall_network_listen + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_network_listen + 12], 1
    jne .bad_size
    cmp dword [syscall_network_listen + 16], 0
    jne .bad_reserved
    cmp dword [network_tcp_listener_bound], 1
    jne .unknown_operation
    mov eax, [syscall_network_listen + 8]
    cmp eax, [network_tcp_listener_handle]
    jne .access_denied
    mov eax, [userspace_pid]
    mov edx, [syscall_network_listen + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_WAIT
    call handle_resolve
    jc .access_denied
    mov dword [network_tcp_listener_state], TCP_STATE_LISTEN
    mov dword [network_tcp_listener_backlog], 1
    mov dword [network_tcp_pending_connections], 1
    mov esi, message_network_listen_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_accept:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 20
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_LISTEN
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 20
    mov edi, syscall_network_accept
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_accept], 20
    jne .bad_size
    cmp dword [syscall_network_accept + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_network_accept + 16], 0
    jne .bad_reserved
    mov eax, [syscall_network_accept + 8]
    cmp eax, [network_tcp_listener_handle]
    jne .access_denied
    cmp dword [network_tcp_listener_state], TCP_STATE_LISTEN
    jne .unknown_operation
    cmp dword [network_tcp_pending_connections], 1
    jne .network_would_block
    mov eax, [userspace_pid]
    mov edx, [syscall_network_accept + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_WAIT
    call handle_resolve
    jc .access_denied
    mov eax, OBJECT_TYPE_NET_SOCKET
    mov edx, NETWORK_SOCKET_STREAM
    mov ebx, 1
    call object_create
    jc .unknown_operation
    mov edx, eax
    mov eax, [userspace_pid]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_QUERY | HANDLE_RIGHT_SEND | HANDLE_RIGHT_RECEIVE | HANDLE_RIGHT_WAIT
    xor esi, esi
    call handle_create
    jc .unknown_operation
    mov [syscall_network_accept + 12], eax
    mov dword [network_tcp_pending_connections], 0
    inc dword [network_tcp_accepted_connections]
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_network_accept
    mov ecx, 20
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_network_accept_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_bind:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 28
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_LISTEN
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 28
    mov edi, syscall_network_bind
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_bind], 28
    jne .bad_size
    cmp dword [syscall_network_bind + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_network_bind + 12], NETWORK_AF_IPV4
    jne .unknown_operation
    cmp dword [syscall_network_bind + 16], NETWORK_IPV4_LOOPBACK
    jne .unknown_operation
    mov eax, [syscall_network_bind + 20]
    test eax, eax
    jz .unknown_operation
    cmp eax, 65535
    ja .unknown_operation
    cmp dword [syscall_network_bind + 24], 0
    jne .bad_reserved
    cmp dword [network_socket_bound], 0
    jne .unknown_operation
    mov eax, [userspace_pid]
    mov edx, [syscall_network_bind + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_BIND
    call handle_resolve
    jc .access_denied
    mov eax, [syscall_network_bind + 20]
    mov [network_socket_port], eax
    mov dword [network_socket_bound], 1
    inc dword [network_namespace_generation]
    mov esi, message_network_bind_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_send:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 40
    jb .bad_size
    mov esi, [edx + 36]
    mov ecx, 40
    mov edi, syscall_network_packet
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_packet], 40
    jne .bad_size
    cmp dword [syscall_network_packet + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_network_packet + 12], 16
    ja .network_packet_too_large
    cmp dword [syscall_network_packet + 16], 0
    jne .bad_reserved
    cmp dword [syscall_network_packet + 36], 0
    jne .bad_reserved
    cmp dword [network_loopback_queue_count], 0
    jne .network_would_block
    cmp dword [network_socket_bound], 1
    jne .unknown_operation
    mov eax, 0x7F000001
    mov ebx, 17                     ; UDP
    mov ecx, [network_socket_port]
    call network_firewall_check_ipv4
    jc .access_denied
    mov eax, [userspace_pid]
    mov edx, [syscall_network_packet + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_SEND
    call handle_resolve
    jc .access_denied
    call network_loopback_encapsulate
    jc .unknown_operation
    mov dword [network_loopback_queue_count], 1
    inc dword [network_transmitted_packets]
    mov esi, message_network_send_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_packet_too_large:
    mov esi, message_network_size_denied
    call serial_write_string
    jmp .bad_size
.network_receive:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 40
    jb .bad_size
    cmp dword [network_loopback_queue_count], 1
    jne .network_would_block
    call network_loopback_validate
    jc .network_malformed
    mov eax, [userspace_pid]
    mov edx, [network_socket_handle]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_RECEIVE
    call handle_resolve
    jc .access_denied
    mov dword [syscall_network_packet], 40
    mov dword [syscall_network_packet + 4], SYSCALL_ABI_VERSION
    mov eax, [network_socket_handle]
    mov [syscall_network_packet + 8], eax
    mov eax, [network_loopback_payload_length]
    mov [syscall_network_packet + 12], eax
    mov dword [syscall_network_packet + 16], 0
    mov esi, network_loopback_frame + 28
    mov edi, syscall_network_packet + 20
    mov ecx, 16
    rep movsb
    mov dword [syscall_network_packet + 36], 0
    mov dword [network_loopback_queue_count], 0
    inc dword [network_received_packets]
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_network_packet
    mov ecx, 40
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_network_loopback_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_malformed:
    mov dword [network_loopback_queue_count], 0
    inc dword [network_dropped_packets]
    mov esi, message_network_malformed
    call serial_write_string
    jmp .unknown_operation
.network_would_block:
    mov esi, message_network_would_block
    call serial_write_string
    mov eax, SYSCALL_STATUS_WOULD_BLOCK
    jmp .reject
.power_query:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 32
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_POWER_QUERY
    call security_check
    jc .access_denied
    mov dword [syscall_power_result + 0], 32
    mov dword [syscall_power_result + 4], SYSCALL_ABI_VERSION
    mov eax, [power_current_state]
    mov [syscall_power_result + 8], eax
    mov dword [syscall_power_result + 12], POWER_SUPPORTED_MASK
    mov eax, [power_profile]
    mov [syscall_power_result + 16], eax
    mov eax, [power_wake_lock_count]
    mov [syscall_power_result + 20], eax
    mov eax, [power_idle_entries]
    mov [syscall_power_result + 24], eax
    mov eax, [power_deep_idle_entries]
    mov [syscall_power_result + 28], eax
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_power_result
    mov ecx, 32
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_power_query_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.power_wake_acquire:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_POWER_WAKE
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 16
    mov edi, syscall_argument_buffer
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_argument_buffer], 16
    jne .bad_size
    cmp dword [syscall_argument_buffer + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_argument_buffer + 12], 0
    jne .bad_reserved
    mov eax, [syscall_argument_buffer + 8]
    call power_wake_lock_acquire
    jc .unknown_operation
    mov [syscall_argument_buffer + 8], eax
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_argument_buffer
    mov ecx, 16
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_power_wake_acquire_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.power_wake_release:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_POWER_WAKE
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 16
    mov edi, syscall_argument_buffer
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_argument_buffer], 16
    jne .bad_size
    cmp dword [syscall_argument_buffer + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_argument_buffer + 12], 0
    jne .bad_reserved
    mov eax, [syscall_argument_buffer + 8]
    call power_wake_lock_release
    jc .unknown_operation
    mov esi, message_power_wake_release_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.power_set_profile:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_POWER_PROFILE
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 16
    mov edi, syscall_argument_buffer
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_argument_buffer], 16
    jne .bad_size
    cmp dword [syscall_argument_buffer + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_argument_buffer + 12], 0
    jne .bad_reserved
    mov eax, [syscall_argument_buffer + 8]
    call power_manager_set_profile
    jc .unknown_operation
    mov esi, message_power_profile_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.power_request_state:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 24
    jb .bad_size
    mov esi, [edx + 36]
    mov ecx, 24
    mov edi, syscall_power_request
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_power_request], 24
    jne .bad_size
    cmp dword [syscall_power_request + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_power_request + 8], POWER_STATE_SHUTDOWN
    jne .unknown_operation
    cmp dword [syscall_power_request + 16], 0
    jne .bad_reserved
    cmp dword [syscall_power_request + 20], 0
    jne .bad_reserved
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_POWER_SHUTDOWN
    call security_check
    jc .power_request_denied
    call power_manager_request_shutdown
    jc .unknown_operation
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    mov dword [edx + 56], kernel_shutdown_authorized
    mov dword [edx + 60], CODE_SEGMENT
    and dword [edx + 64], 0xFFFFCFFF
    ret
.power_request_denied:
    mov esi, message_power_shutdown_denied
    call serial_write_string
    jmp .access_denied
.unknown_service:
    mov eax, SYSCALL_STATUS_SERVICE
    jmp .reject
.unknown_operation:
    mov eax, SYSCALL_STATUS_OPERATION
    jmp .reject
.bad_abi:
    mov eax, SYSCALL_STATUS_ABI
    jmp .reject
.bad_size:
    mov eax, SYSCALL_STATUS_SIZE
    jmp .reject
.bad_reserved:
    mov eax, SYSCALL_STATUS_RESERVED
    jmp .reject
.access_denied:
    mov eax, SYSCALL_STATUS_ACCESS
    jmp .reject
.semantic_mismatch:
    inc dword [semantic_ipc_rejected]
    mov eax, SYSCALL_STATUS_TYPE
    jmp .reject
.semantic_validation_failed:
    inc dword [semantic_ipc_validation_rejected]
    mov eax, SYSCALL_STATUS_VALIDATION
    jmp .reject
.bad_pointer:
    mov eax, SYSCALL_STATUS_POINTER
.reject:
    inc dword [syscall_rejected]
    mov edx, [syscall_frame]
    mov [edx + 44], eax
    ret

userspace_pid:        dd 0
userspace_tid:        dd 0
userspace_process_handle: dd 0
userspace_thread_handle:  dd 0
userspace_code_page:  dd 0
userspace_code_page_2: dd 0
userspace_code_page_3: dd 0
userspace_stack_page: dd 0
userspace_exit_seen:  dd 0
userspace_exit_code:  dd 0
syscall_frame:        dd 0
syscall_total:        dd 0
syscall_rejected:     dd 0
syscall_identity:     dd 0
syscall_closed_handle: dd 0
semantic_ipc_rejected: dd 0
semantic_ipc_validation_rejected: dd 0
align 4
syscall_argument_buffer:
    times 16 db 0
syscall_result_buffer:
    times 16 db 0
syscall_vfs_buffer: times 32 db 0
syscall_vfs_path:   times 4 db 0
syscall_power_result: times 32 db 0
syscall_power_request: times 24 db 0
syscall_network_result: times 96 db 0
syscall_network_packet: times 40 db 0
syscall_network_bind: times 28 db 0
syscall_network_connect: times 28 db 0
syscall_network_tcp_packet: times 40 db 0
syscall_network_stream_bind: times 28 db 0
syscall_network_listen: times 20 db 0
syscall_network_accept: times 20 db 0
syscall_log_result: times 32 db 0
syscall_display_info: times DISPLAY_INFO_SIZE db 0
syscall_display_scene: times SYSTEM_SCENE_SIZE db 0
syscall_display_input: times SYSTEM_INPUT_EVENT_SIZE db 0

OBJECT_TYPE_IPC_ENDPOINT equ 11

userspace_ipc_initialize:
    mov dword [userspace_ipc_count], 0
    mov dword [userspace_ipc_validation_state], 0
    mov dword [semantic_ipc_rejected], 0
    mov dword [semantic_ipc_validation_rejected], 0
    mov edi, userspace_ipc_queue
    xor eax, eax
    mov ecx, 48 / 4
    rep stosd
    mov eax, OBJECT_TYPE_IPC_ENDPOINT
    mov edx, 1
    xor ebx, ebx
    call object_create
    jc .invalid
    mov [userspace_ipc_send_object], eax
    mov ecx, SEMANTIC_IPC_INLINE_DATA
    mov edx, SEMANTIC_TYPE_VERSION_1
    mov ebx, SEMANTIC_VALIDATION_VERIFIED
    call object_semantic_attach
    jc .invalid
    mov eax, OBJECT_TYPE_IPC_ENDPOINT
    mov edx, 2
    xor ebx, ebx
    call object_create
    jc .invalid
    mov [userspace_ipc_receive_object], eax
    mov ecx, SEMANTIC_IPC_INLINE_DATA
    mov edx, SEMANTIC_TYPE_VERSION_1
    mov ebx, SEMANTIC_VALIDATION_VERIFIED
    call object_semantic_attach
    jc .invalid
    mov eax, 2
    mov edx, [userspace_ipc_send_object]
    mov ebx, OBJECT_TYPE_IPC_ENDPOINT
    mov ecx, HANDLE_RIGHT_SEND
    xor esi, esi
    call handle_create
    jc .invalid
    mov [userspace_ipc_send_handle], eax
    mov eax, 2
    mov edx, [userspace_ipc_receive_object]
    mov ebx, OBJECT_TYPE_IPC_ENDPOINT
    mov ecx, HANDLE_RIGHT_RECEIVE | HANDLE_RIGHT_WAIT
    xor esi, esi
    call handle_create
    jc .invalid
    mov [userspace_ipc_receive_handle], eax
    ; Das Paket kann je nach Programmgroesse in der ersten, zweiten oder
    ; dritten Codeseite liegen.
%if (userspace_ipc_packet - userspace_program_start + 8) < PMM_PAGE_SIZE
    mov edi, [userspace_code_page]
    add edi, userspace_ipc_packet - userspace_program_start + 8
%elif (userspace_ipc_packet - userspace_program_start + 8) < (PMM_PAGE_SIZE * 2)
    mov edi, [userspace_code_page_2]
    add edi, userspace_ipc_packet - userspace_program_start + 8 - PMM_PAGE_SIZE
%else
    mov edi, [userspace_code_page_3]
    add edi, userspace_ipc_packet - userspace_program_start + 8 - (PMM_PAGE_SIZE * 2)
%endif
    mov eax, [userspace_ipc_send_handle]
    mov [edi], eax
    cmp dword [handle_active_count], 4
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

userspace_ipc_send_object:    dd 0
userspace_ipc_receive_object: dd 0
userspace_ipc_send_handle:    dd 0
userspace_ipc_receive_handle: dd 0
userspace_ipc_count:          dd 0
userspace_ipc_validation_state: dd 0
align 4
userspace_ipc_copy:  times 48 db 0
userspace_ipc_queue: times 48 db 0

userspace_vfs_initialize:
    mov eax, 2
    mov edx, [vfs_state + VFS_ROOT_NODE_HANDLE]
    mov ebx, OBJECT_TYPE_VFS_NODE
    mov ecx, HANDLE_RIGHT_QUERY
    mov esi, HANDLE_FLAG_PROTECTED
    call handle_create
    jc .invalid
    mov [userspace_root_handle], eax
    cmp dword [handle_active_count], 5
    jne .invalid
    mov edx, eax
    mov eax, 2
    mov ebx, OBJECT_TYPE_VFS_NODE
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jc .invalid
    mov edx, [userspace_root_handle]
    mov eax, 2
    mov ebx, OBJECT_TYPE_DEVICE       ; Root-Handle ist kein Device-Handle
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jnc .invalid
    clc
    ret
.invalid:
    stc
    ret

userspace_root_handle: dd 0

; Erstellt eine ausschließlich lesbare, öffentliche Service-Informationsseite.
; Physische Adresse und interne Kernelobjekte werden darin nie veröffentlicht.
shared_service_page_initialize:
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [shared_service_page], eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    mov edi, [shared_service_page]
    mov dword [edi + 0], SHARED_SERVICE_SIGNATURE
    mov dword [edi + 4], SHARED_SERVICE_SIZE
    mov dword [edi + 8], SYSCALL_ABI_VERSION
    mov dword [edi + 12], SHARED_FEATURE_INT80 | SHARED_FEATURE_COPY_IO | SHARED_FEATURE_PREEMPT | SHARED_FEATURE_DISPLAY
    cmp dword [nfs_mounted], 1
    jne .features_done
    or dword [edi + 12], SHARED_FEATURE_FILESYSTEM
    cmp dword [nfs_readonly], 0
    jne .features_done
    or dword [edi + 12], SHARED_FEATURE_FILESYSTEM_WRITABLE
.features_done:
    mov dword [edi + 16], SHARED_SERVICE_BITMAP
    mov dword [edi + 20], PMM_PAGE_SIZE
    mov dword [edi + 24], 100
    mov dword [edi + 28], BOOT_PHASE_USERSPACE
    mov eax, [timer_ticks]
    mov [edi + 32], eax
    mov dword [edi + 36], 0
    mov eax, SHARED_SERVICE_ADDRESS
    mov edx, [shared_service_page]
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_USER
    call paging_map_page
    jc .invalid
    mov eax, cr3
    mov cr3, eax
    clc
    ret
.invalid:
    stc
    ret

shared_service_page: dd 0
userspace_ready_seen: dd 0

; ---------------------------------------------------------------------------
; PS/2-Maus im pollingbasierten Bootstrap-Pfad
; ---------------------------------------------------------------------------
ps2_mouse_wait_write:
    mov ecx, 0x10000
.wait:
    in al, 0x64
    test al, 0x02
    jz .ready
    loop .wait
    stc
    ret
.ready:
    clc
    ret

ps2_mouse_wait_read:
    mov ecx, 0x10000
.wait:
    in al, 0x64
    test al, 0x01
    jnz .ready
    loop .wait
    stc
    ret
.ready:
    clc
    ret

; AL=Gerätekommando, CF=0 nur bei ACK 0xFA.
ps2_mouse_command:
    mov bl, al
    call ps2_mouse_wait_write
    jc .failed
    mov al, 0xD4
    out 0x64, al
    call ps2_mouse_wait_write
    jc .failed
    mov al, bl
    out 0x60, al
    call ps2_mouse_wait_read
    jc .failed
    in al, 0x60
    cmp al, 0xFA
    jne .failed
    clc
    ret
.failed:
    stc
    ret

ps2_mouse_initialize:
    pushfd
    cli
    pushad
    mov byte [mouse_ready], 0
    mov byte [mouse_packet_index], 0
    mov byte [mouse_buttons], 0
    mov byte [mouse_cursor_valid], 0
    mov eax, [kernel_context + CONTEXT_WIDTH]
    shr eax, 1
    mov [mouse_x], eax
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    shr eax, 1
    mov [mouse_y], eax
    call ps2_mouse_wait_write
    jc .done
    mov al, 0xA8                    ; Auxiliary-Port aktivieren
    out 0x64, al
    mov al, 0xF6                    ; Standardwerte
    call ps2_mouse_command
    jc .done
    mov al, 0xF4                    ; Datenreporting aktivieren
    call ps2_mouse_command
    jc .done
    mov byte [mouse_ready], 1
.done:
    popad
    popfd
    ret

; AL=ein Byte des dreiteiligen Standard-PS/2-Pakets.
ps2_mouse_handle_byte:
    pushad
    cmp byte [mouse_ready], 1
    jne .done
    movzx ecx, byte [mouse_packet_index]
    test ecx, ecx
    jnz .store
    test al, 0x08                   ; Synchronisationsbit des ersten Bytes
    jz .done
.store:
    mov [mouse_packet + ecx], al
    inc ecx
    cmp ecx, 3
    jb .pending
    mov byte [mouse_packet_index], 0
    movsx eax, byte [mouse_packet + 1]
    add [mouse_x], eax
    movsx eax, byte [mouse_packet + 2]
    sub [mouse_y], eax
    cmp dword [mouse_x], 0
    jge .x_upper
    mov dword [mouse_x], 0
.x_upper:
    mov eax, [kernel_context + CONTEXT_WIDTH]
    sub eax, 12
    cmp [mouse_x], eax
    jle .y_lower
    mov [mouse_x], eax
.y_lower:
    cmp dword [mouse_y], 0
    jge .y_upper
    mov dword [mouse_y], 0
.y_upper:
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    sub eax, 16
    cmp [mouse_y], eax
    jle .draw
    mov [mouse_y], eax
.draw:
    call draw_mouse_cursor
    mov al, [mouse_packet]
    and al, 1
    mov ah, [mouse_buttons]
    and ah, 1
    mov bl, [mouse_packet]
    and bl, 7
    mov [mouse_buttons], bl
    test al, al
    jz .done
    test ah, ah
    jnz .done
    call mouse_dispatch_click
    jmp .done
.pending:
    mov [mouse_packet_index], cl
.done:
    popad
    ret

mouse_dispatch_click:
    pushad
    ; Nova-Orb in der schwebenden Taskleiste.
    cmp dword [mouse_x], 92
    ja .menu
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    sub eax, 88
    cmp [mouse_y], eax
    jb .menu
    mov eax, SYSTEM_INPUT_TOGGLE_START
    call input_router_enqueue
    jmp .done
.menu:
    test dword [display_scene_flags], DISPLAY_SCENE_START_MENU
    jz .workspace
    mov eax, [mouse_x]
    sub eax, [shell_menu_x]
    sub eax, 210
    js .done
    cmp eax, 358
    jae .done
    xor edx, edx
    mov ecx, 92
    div ecx
    cmp edx, 82
    jae .done
    mov ebx, eax                    ; Spalte
    mov eax, [mouse_y]
    sub eax, [shell_menu_y]
    sub eax, 60
    js .done
    cmp eax, 138
    jae .done
    xor edx, edx
    mov ecx, 74
    div ecx
    cmp edx, 64
    jae .done
    shl eax, 2
    add eax, ebx
    add eax, 3
    cmp eax, 10
    ja .done
    mov edx, eax
    mov eax, SYSTEM_INPUT_POINTER_ACTIVATE
    call input_router_enqueue_target
    jmp .done
.workspace:
    cmp dword [display_scene_workspace], 1
    je .sheet
    cmp dword [display_scene_workspace], 2
    je .studio
    call explorer_hit_test
    test eax, eax
    jz .done
    jmp .focus
.sheet:
    mov eax, [mouse_y]
    sub eax, [shell_window_y]
    sub eax, 288
    js .done
    xor edx, edx
    mov ecx, 24
    div ecx
    cmp eax, 5
    ja .done
    add eax, 40
    jmp .focus
.studio:
    mov eax, [mouse_y]
    sub eax, [shell_window_y]
    sub eax, 140
    js .done
    xor edx, edx
    mov ecx, 82
    div ecx
    cmp eax, 2
    ja .done
    mov edx, eax
    mov eax, [mouse_x]
    sub eax, [shell_window_x]
    cmp eax, 620
    jb .studio_left
    add edx, 3
.studio_left:
    mov eax, edx
    add eax, 60
.focus:
    mov edx, eax
    mov eax, SYSTEM_INPUT_POINTER_ACTIVATE
    call input_router_enqueue_target
.done:
    popad
    ret

; Software-Cursor mit gesichertem 12x16-Hintergrund.
draw_mouse_cursor:
    pushad
    cmp byte [mouse_cursor_valid], 1
    jne .save
    xor ebp, ebp
.restore_row:
    cmp ebp, 16
    jae .save
    xor ecx, ecx
.restore_column:
    cmp ecx, 12
    jae .restore_next
    mov edi, [mouse_saved_y]
    add edi, ebp
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov eax, [mouse_saved_x]
    add eax, ecx
    shl eax, 2
    add edi, eax
    mov eax, ebp
    imul eax, 12
    add eax, ecx
    mov eax, [mouse_background + eax * 4]
    mov [edi], eax
    inc ecx
    jmp .restore_column
.restore_next:
    inc ebp
    jmp .restore_row
.save:
    mov eax, [mouse_x]
    mov [mouse_saved_x], eax
    mov eax, [mouse_y]
    mov [mouse_saved_y], eax
    xor ebp, ebp
.draw_row:
    cmp ebp, 16
    jae .complete
    xor ecx, ecx
.draw_column:
    cmp ecx, 12
    jae .draw_next
    mov edi, [mouse_y]
    add edi, ebp
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov eax, [mouse_x]
    add eax, ecx
    shl eax, 2
    add edi, eax
    mov eax, ebp
    imul eax, 12
    add eax, ecx
    mov edx, [edi]
    mov [mouse_background + eax * 4], edx
    ; Klassischer, schlanker Pfeil: linke Kante plus wachsende Diagonale.
    test ecx, ecx
    jz .paint
    cmp ebp, 12
    jae .next_pixel
    mov eax, ebp
    shr eax, 1
    cmp ecx, eax
    ja .next_pixel
.paint:
    mov dword [edi], NOVA_COLOR_WHITE
.next_pixel:
    inc ecx
    jmp .draw_column
.draw_next:
    inc ebp
    jmp .draw_row
.complete:
    mov byte [mouse_cursor_valid], 1
    popad
    ret

; ---------------------------------------------------------------------------
; Bootstrap Display Server ABI 1.0
; ---------------------------------------------------------------------------
; Der Kernel behält MMIO und die physische Framebufferadresse. Der initiale
; System-UI-Prozess reicht ausschließlich validierte semantische Szenen ein.
display_server_initialize:
    mov dword [display_server_ready], 0
    mov dword [display_primary_id], 0
    mov dword [display_generation], 1
    mov dword [display_scene_generation], 0
    mov dword [display_scene_flags], 0
    mov dword [display_scene_focus], 0
    mov dword [display_scene_workspace], 0
    mov dword [display_present_count], 0
    mov dword [display_input_pending], 0
    mov dword [display_input_action], 0
    mov dword [display_input_scancode], 0
    mov dword [display_input_tick], 0
    mov dword [display_input_target], 0
    mov dword [display_input_dropped], 0
    mov byte [keyboard_extended], 0
    mov byte [keyboard_break_pending], 0
    test dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_GRAPHICS
    jz .fallback
    cmp dword [kernel_context + CONTEXT_FRAMEBUFFER], 0
    je .fallback
    cmp dword [kernel_context + CONTEXT_BPP], 32
    jne .fallback
    cmp dword [kernel_context + CONTEXT_WIDTH], 640
    jb .fallback
    cmp dword [kernel_context + CONTEXT_HEIGHT], 480
    jb .fallback
    mov eax, [kernel_context + CONTEXT_WIDTH]
    shl eax, 2
    cmp [kernel_context + CONTEXT_PITCH], eax
    jb .invalid
    call ps2_mouse_initialize
    mov dword [display_primary_id], 1
    mov dword [display_server_ready], 1
.fallback:
    clc
    ret
.invalid:
    stc
    ret

align 4
display_server_api:
    dd 40
    dw 1, 0
    dd display_server_ready
    dd display_primary_id
    dd display_generation
    dd display_scene_generation
    dd display_present_count
    dd display_server_initialize
    dd draw_desktop_scene
display_server_ready:     dd 0
display_primary_id:       dd 0
display_generation:       dd 0
display_scene_generation: dd 0
display_scene_flags:      dd 0
display_scene_focus:      dd 0
display_scene_workspace:  dd 0
display_present_count:    dd 0
display_input_pending:    dd 0
display_input_action:     dd 0
display_input_scancode:   dd 0
display_input_tick:       dd 0
display_input_target:     dd 0
display_input_dropped:    dd 0
keyboard_extended:        db 0
keyboard_break_pending:   db 0
; US-QWERTY-Tabelle (Set 1, Make-Codes, nur Kleinbuchstaben) fuer die
; Texteingabe beim Umbenennen (F2). 0 = kein druckbares Zeichen zugeordnet.
keyboard_ascii_table:
    db 0, 0                             ; 0x00-0x01
    db '1234567890-='                   ; 0x02-0x0D
    db 0, 0                             ; 0x0E-0x0F (Backspace, Tab)
    db 'qwertyuiop'                     ; 0x10-0x19
    db 0, 0, 0, 0                       ; 0x1A-0x1D ([, ], Enter, Ctrl)
    db 'asdfghjkl'                      ; 0x1E-0x26
    db 0, 0, 0, 0, 0                    ; 0x27-0x2B (;, ', `, Shift, \)
    db 'zxcvbnm'                        ; 0x2C-0x32
    db ',./'                            ; 0x33-0x35
    db 0, 0, 0                          ; 0x36-0x38 (Shift, *, Alt)
    db ' '                              ; 0x39 (Leertaste)
    times (256 - ($ - keyboard_ascii_table)) db 0
mouse_ready:              db 0
mouse_packet_index:       db 0
mouse_buttons:            db 0
mouse_cursor_valid:       db 0
align 4
mouse_x:                  dd 0
mouse_y:                  dd 0
mouse_saved_x:            dd 0
mouse_saved_y:            dd 0
mouse_packet:             times 3 db 0
align 4
mouse_background:         times 12 * 16 dd 0
align 4

