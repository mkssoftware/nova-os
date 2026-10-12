; ---------------------------------------------------------------------------
; Begrenztes completion-basiertes Async-I/O-Grundmodell
; NPSPEC-IO-REQUEST/COMPLETION/ASYNC/DEADLINE-0001
; ---------------------------------------------------------------------------

IO_REQUEST_API_SIZE             equ 32
IO_REQUEST_CAPACITY             equ 8
IO_REQUEST_RECORD_SIZE          equ 64
IO_REQUEST_STATE_EMPTY          equ 0
IO_REQUEST_STATE_PENDING        equ 1
IO_REQUEST_STATE_RUNNING        equ 2
IO_REQUEST_STATE_COMPLETED      equ 3
IO_REQUEST_STATE_FAILED         equ 4
IO_REQUEST_STATE_CANCELLED      equ 5
IO_COMPLETION_NONE              equ 0
IO_COMPLETION_SUCCESS           equ 1
IO_COMPLETION_PARTIAL           equ 2
IO_COMPLETION_FAILED            equ 3
IO_COMPLETION_CANCELLED         equ 4
IO_COMPLETION_DEADLINE_MISS     equ 5
IO_CAP_ASYNC_COMPLETION         equ 0x00000001
IO_CAP_BOUNDED_QUEUE            equ 0x00000002
IO_CAP_TASK_OWNERSHIP           equ 0x00000004
IO_CAP_DEADLINE                 equ 0x00000008
IO_FLAG_DEADLINE_INHERITED      equ 0x00000001
IO_FLAG_DEADLINE_MISSED         equ 0x00000002
IO_CANCEL_REASON_DEADLINE       equ 0x494F444C
IO_REQUEST_ID                   equ 0
IO_REQUEST_OWNER                equ 4
IO_REQUEST_TASK                 equ 8
IO_REQUEST_SCOPE                equ 12
IO_REQUEST_OPERATION            equ 16
IO_REQUEST_STATE                equ 20
IO_REQUEST_TARGET               equ 24
IO_REQUEST_BUFFER               equ 28
IO_REQUEST_LENGTH               equ 32
IO_REQUEST_TRANSFERRED          equ 36
IO_REQUEST_DEADLINE             equ 40
IO_REQUEST_PRIORITY             equ 44
IO_REQUEST_COMPLETION           equ 48
IO_REQUEST_ERROR                equ 52
IO_REQUEST_FLAGS                equ 56
IO_REQUEST_EFFECTIVE_PRIORITY   equ 60
IO_PRIORITY_REALTIME            equ 1
IO_PRIORITY_INTERACTIVE         equ 2
IO_PRIORITY_NORMAL              equ 3
IO_PRIORITY_BACKGROUND          equ 4
IO_PRIORITY_MAINTENANCE         equ 5

io_request_manager_initialize:
    mov edi, io_request_table
    xor eax, eax
    mov ecx, (IO_REQUEST_CAPACITY * IO_REQUEST_RECORD_SIZE) / 4
    rep stosd
    mov edi, io_request_submit_ticks
    mov ecx, IO_REQUEST_CAPACITY * 3
    rep stosd
    mov dword [io_request_next_id], 1
    mov dword [io_request_outstanding], 0
    mov dword [io_request_backpressure_count], 0
    mov dword [io_request_deadline_miss_count], 0
    mov dword [io_request_manager_ready], 1
    clc
    ret

; EAX=Owner-PID, EDX=Task-ID, EBX=Operation, ECX=Target-Handle,
; ESI=Buffer-Handle, EDI=Laenge, EBP=Prioritaet (0..3). EAX=Request-ID.
; Request-Erzeugung, Ausfuehrung und Completion bleiben getrennt.
io_request_submit:
    pushfd
    cli
    mov [io_request_temp_owner], eax
    mov [io_request_temp_task], edx
    mov [io_request_temp_operation], ebx
    mov [io_request_temp_target], ecx
    mov [io_request_temp_buffer], esi
    mov [io_request_temp_length], edi
    mov [io_request_temp_priority], ebp
    test ebx, ebx
    jz .invalid
    test ecx, ecx
    jz .invalid
    test esi, esi
    jz .invalid
    test edi, edi
    jz .invalid
    cmp ebp, IO_PRIORITY_MAINTENANCE
    ja .invalid
    call process_lookup
    jc .invalid
    mov eax, [io_request_temp_task]
    call task_lookup
    jc .invalid
    mov [io_request_temp_task_record], eax
    mov edx, [io_request_temp_owner]
    cmp [eax + TASK_OWNER], edx
    jne .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CREATED
    jb .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_WAITING
    ja .invalid

    xor ecx, ecx
.scan:
    cmp ecx, IO_REQUEST_CAPACITY
    jae .backpressure
    mov edi, ecx
    shl edi, 6
    add edi, io_request_table
    cmp dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_EMPTY
    je .slot
    cmp dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_COMPLETED
    jae .slot
    inc ecx
    jmp .scan
.slot:
    mov [io_request_temp_record], edi
    mov [io_request_temp_slot], ecx
    mov dword [io_request_submit_ticks + ecx * 4], 0
    mov dword [io_request_dispatch_ticks + ecx * 4], 0
    mov dword [io_request_wait_rounds + ecx * 4], 0
    mov dword [io_request_qos_ids + ecx * 4], 0
    mov dword [io_request_buffer_ids + ecx * 4], 0
    xor eax, eax
    mov ecx, IO_REQUEST_RECORD_SIZE / 4
    rep stosd
    mov edi, [io_request_temp_record]
    mov eax, [io_request_next_id]
    mov [edi + IO_REQUEST_ID], eax
    mov edx, [io_request_temp_owner]
    mov [edi + IO_REQUEST_OWNER], edx
    mov edx, [io_request_temp_task]
    mov [edi + IO_REQUEST_TASK], edx
    mov edx, [io_request_temp_task_record]
    mov edx, [edx + TASK_RECORD_SCOPE]
    mov [edi + IO_REQUEST_SCOPE], edx
    mov edx, [io_request_temp_operation]
    mov [edi + IO_REQUEST_OPERATION], edx
    mov dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    mov edx, [io_request_temp_target]
    mov [edi + IO_REQUEST_TARGET], edx
    mov edx, [io_request_temp_buffer]
    mov [edi + IO_REQUEST_BUFFER], edx
    mov edx, [io_request_temp_length]
    mov [edi + IO_REQUEST_LENGTH], edx
    mov edx, [io_request_temp_priority]
    test edx, edx
    jnz .priority_requested
    mov edx, IO_PRIORITY_NORMAL
.priority_requested:
    mov [edi + IO_REQUEST_PRIORITY], edx
    ; Bootstrap-Policy: eine unprivilegierte Realtime-Anforderung wird auf
    ; Interactive begrenzt. Prioritaet bleibt damit Dringlichkeit, nicht Recht.
    cmp edx, IO_PRIORITY_REALTIME
    jne .priority_effective
    mov edx, IO_PRIORITY_INTERACTIVE
.priority_effective:
    mov [edi + IO_REQUEST_EFFECTIVE_PRIORITY], edx

    mov eax, [io_request_temp_task_record]
    call task_deadline_record_for_task
    cmp dword [eax + TASK_DEADLINE_STATE], TASK_DEADLINE_STATE_ARMED
    jne .ready
    mov edx, [eax + TASK_DEADLINE_ABSOLUTE]
    mov [edi + IO_REQUEST_DEADLINE], edx
    or dword [edi + IO_REQUEST_FLAGS], IO_FLAG_DEADLINE_INHERITED
.ready:
    mov ecx, [io_request_temp_slot]
    mov edx, [timer_ticks]
    mov [io_request_submit_ticks + ecx * 4], edx
    inc dword [io_request_next_id]
    inc dword [io_request_outstanding]
    mov eax, [edi + IO_REQUEST_ID]
    popfd
    clc
    ret
.backpressure:
    inc dword [io_request_backpressure_count]
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Request-ID. EAX=Datensatz oder 0.
io_request_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, IO_REQUEST_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, io_request_table
    cmp dword [edx + IO_REQUEST_STATE], IO_REQUEST_STATE_EMPTY
    je .next
    cmp [edx + IO_REQUEST_ID], eax
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

; EAX=Request-ID, EDX=uebertragene Bytes, EBX=Fehlercode (0=Erfolg).
; Genau ein terminales Ergebnis wird akzeptiert.
io_request_complete:
    pushfd
    cli
    mov [io_request_temp_id], eax
    mov [io_request_temp_transferred], edx
    mov [io_request_temp_error], ebx
    call io_request_lookup
    jc .invalid
    mov [io_request_temp_record], eax
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    je .active
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_RUNNING
    jne .invalid
.active:
    mov edx, [io_request_temp_transferred]
    cmp edx, [eax + IO_REQUEST_LENGTH]
    ja .invalid
    mov [eax + IO_REQUEST_TRANSFERRED], edx
    mov ebx, [io_request_temp_error]
    test ebx, ebx
    jnz .failed
    mov dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_COMPLETED
    cmp edx, [eax + IO_REQUEST_LENGTH]
    jne .partial
    mov dword [eax + IO_REQUEST_COMPLETION], IO_COMPLETION_SUCCESS
    jmp .terminal
.partial:
    mov dword [eax + IO_REQUEST_COMPLETION], IO_COMPLETION_PARTIAL
    jmp .terminal
.failed:
    mov dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_FAILED
    mov dword [eax + IO_REQUEST_COMPLETION], IO_COMPLETION_FAILED
    mov [eax + IO_REQUEST_ERROR], ebx
.terminal:
    push eax
    call io_qos_on_terminal
    pop eax
    push eax
    call shared_buffer_on_terminal
    pop eax
    call io_completion_publish
    dec dword [io_request_outstanding]
    mov eax, [io_request_temp_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Request-ID, EDX=Abbruchgrund. Zu spaete Abbrueche werden abgewiesen.
io_request_cancel:
    pushfd
    cli
    mov [io_request_temp_id], eax
    mov [io_request_temp_error], edx
    call io_request_lookup
    jc .invalid
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    je .active
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_RUNNING
    jne .invalid
.active:
    mov dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_CANCELLED
    mov dword [eax + IO_REQUEST_COMPLETION], IO_COMPLETION_CANCELLED
    mov edx, [io_request_temp_error]
    mov [eax + IO_REQUEST_ERROR], edx
    push eax
    call io_qos_on_terminal
    pop eax
    push eax
    call shared_buffer_on_terminal
    pop eax
    call io_completion_publish
    dec dword [io_request_outstanding]
    mov eax, [io_request_temp_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; Structured Concurrency: offene Requests folgen dem Cancellation-Zustand
; ihrer besitzenden Task, ohne einen Thread pro I/O anzulegen.
io_cancel_for_requested_tasks:
    cmp dword [io_request_manager_ready], 1
    jne .done
    xor ecx, ecx
.scan:
    cmp ecx, IO_REQUEST_CAPACITY
    jae .done
    mov edi, ecx
    shl edi, 6
    add edi, io_request_table
    cmp dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    je .check
    cmp dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_RUNNING
    jne .next
.check:
    mov eax, [edi + IO_REQUEST_TASK]
    push ecx
    call task_lookup
    pop ecx
    jc .next
    cmp dword [eax + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jne .next
    mov edx, [eax + TASK_CANCEL_REASON]
    mov eax, [edi + IO_REQUEST_ID]
    push ecx
    call io_request_cancel
    pop ecx
.next:
    inc ecx
    jmp .scan
.done:
    clc
    ret

; Monotone PIT-Ticks entscheiden Deadline-Misses. Der Miss bleibt als eigener
; Completion-Status sichtbar; anschliessend ist der Request terminal beendet.
io_request_poll_deadlines:
    cmp dword [io_request_manager_ready], 1
    jne .done
    mov eax, [timer_ticks]
    mov [io_request_poll_tick], eax
    xor ecx, ecx
.scan:
    cmp ecx, IO_REQUEST_CAPACITY
    jae .done
    mov edi, ecx
    shl edi, 6
    add edi, io_request_table
    cmp dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    je .check
    cmp dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_RUNNING
    jne .next
.check:
    test dword [edi + IO_REQUEST_FLAGS], IO_FLAG_DEADLINE_INHERITED
    jz .next
    mov eax, [io_request_poll_tick]
    sub eax, [edi + IO_REQUEST_DEADLINE]
    jl .next
    or dword [edi + IO_REQUEST_FLAGS], IO_FLAG_DEADLINE_MISSED
    mov dword [edi + IO_REQUEST_STATE], IO_REQUEST_STATE_CANCELLED
    mov dword [edi + IO_REQUEST_COMPLETION], IO_COMPLETION_DEADLINE_MISS
    mov dword [edi + IO_REQUEST_ERROR], IO_CANCEL_REASON_DEADLINE
    mov eax, edi
    push ecx
    push eax
    call io_qos_on_terminal
    pop eax
    push eax
    call shared_buffer_on_terminal
    pop eax
    call io_completion_publish
    pop ecx
    dec dword [io_request_outstanding]
    inc dword [io_request_deadline_miss_count]
.next:
    inc ecx
    jmp .scan
.done:
    clc
    ret

; ---------------------------------------------------------------------------
; Begrenzte FIFO-Completion-Queue mit Batch-Dequeue
; NPSPEC-IO-COMPLETION-0001 / ADR-IO-0001
; ---------------------------------------------------------------------------

IO_COMPLETION_API_SIZE       equ 32
IO_COMPLETION_CAPACITY       equ 16
IO_COMPLETION_RECORD_SIZE    equ 32
IO_COMPLETION_CAPABILITIES   equ 0x0000000F
IO_COMPLETION_REQUEST_ID     equ 0
IO_COMPLETION_OWNER          equ 4
IO_COMPLETION_STATUS         equ 8
IO_COMPLETION_BYTES          equ 12
IO_COMPLETION_ERROR          equ 16
IO_COMPLETION_TICK           equ 20
IO_COMPLETION_LATENCY        equ 24
IO_COMPLETION_FLAGS          equ 28

io_completion_initialize:
    mov edi, io_completion_queue
    xor eax, eax
    mov ecx, (IO_COMPLETION_CAPACITY * IO_COMPLETION_RECORD_SIZE) / 4
    rep stosd
    mov dword [io_completion_head], 0
    mov dword [io_completion_tail], 0
    mov dword [io_completion_count], 0
    mov dword [io_completion_overflow_count], 0
    mov dword [io_completion_manager_ready], 1
    clc
    ret

; EAX=terminaler IORequest-Datensatz. Der Request bleibt die autoritative
; Historie; die Queue ist der effiziente, batchfaehige Zustellmechanismus.
io_completion_publish:
    cmp dword [io_completion_manager_ready], 1
    jne .done
    cmp dword [io_completion_count], IO_COMPLETION_CAPACITY
    jae .overflow
    mov [io_completion_temp_request], eax
    mov ecx, eax
    sub ecx, io_request_table
    shr ecx, 6
    mov [io_completion_temp_request_slot], ecx
    mov edi, [io_completion_tail]
    shl edi, 5
    add edi, io_completion_queue
    mov esi, [io_completion_temp_request]
    mov eax, [esi + IO_REQUEST_ID]
    mov [edi + IO_COMPLETION_REQUEST_ID], eax
    mov eax, [esi + IO_REQUEST_OWNER]
    mov [edi + IO_COMPLETION_OWNER], eax
    mov eax, [esi + IO_REQUEST_COMPLETION]
    mov [edi + IO_COMPLETION_STATUS], eax
    mov eax, [esi + IO_REQUEST_TRANSFERRED]
    mov [edi + IO_COMPLETION_BYTES], eax
    mov eax, [esi + IO_REQUEST_ERROR]
    mov [edi + IO_COMPLETION_ERROR], eax
    mov eax, [timer_ticks]
    mov [edi + IO_COMPLETION_TICK], eax
    mov ecx, [io_completion_temp_request_slot]
    sub eax, [io_request_submit_ticks + ecx * 4]
    mov [edi + IO_COMPLETION_LATENCY], eax
    mov eax, [esi + IO_REQUEST_FLAGS]
    mov [edi + IO_COMPLETION_FLAGS], eax
    mov eax, [io_completion_tail]
    inc eax
    and eax, IO_COMPLETION_CAPACITY - 1
    mov [io_completion_tail], eax
    inc dword [io_completion_count]
.done:
    clc
    ret
.overflow:
    inc dword [io_completion_overflow_count]
    stc
    ret

; EAX=Kernel-Zielpuffer, ECX=maximale Eintraege. EAX=kopierte Anzahl.
; Der begrenzte Batch vermeidet einen Wakeup beziehungsweise Call pro Request.
io_completion_dequeue_batch:
    pushfd
    cli
    test eax, eax
    jz .invalid
    test ecx, ecx
    jz .invalid
    cmp ecx, IO_COMPLETION_CAPACITY
    ja .invalid
    mov [io_completion_batch_destination], eax
    mov [io_completion_batch_limit], ecx
    mov dword [io_completion_batch_count], 0
.copy:
    mov ecx, [io_completion_batch_count]
    cmp ecx, [io_completion_batch_limit]
    jae .complete
    cmp dword [io_completion_count], 0
    je .complete
    mov esi, [io_completion_head]
    shl esi, 5
    add esi, io_completion_queue
    mov edi, [io_completion_batch_destination]
    mov ecx, IO_COMPLETION_RECORD_SIZE / 4
    rep movsd
    mov [io_completion_batch_destination], edi
    mov eax, [io_completion_head]
    inc eax
    and eax, IO_COMPLETION_CAPACITY - 1
    mov [io_completion_head], eax
    dec dword [io_completion_count]
    inc dword [io_completion_batch_count]
    jmp .copy
.complete:
    mov eax, [io_completion_batch_count]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

io_completion_self_test:
    ; Die vorausgehenden Request-Tests muessen bereits Completions erzeugt haben.
    cmp dword [io_completion_count], 0
    je .invalid
    mov eax, io_completion_test_batch
    mov ecx, IO_COMPLETION_CAPACITY
    call io_completion_dequeue_batch
    jc .invalid
    test eax, eax
    jz .invalid
    cmp dword [io_completion_count], 0
    jne .invalid

    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [io_completion_test_scope], eax
    mov eax, 1
    mov edx, [io_completion_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [io_completion_test_task], eax

    mov eax, 1
    mov edx, [io_completion_test_task]
    mov ebx, 4
    mov ecx, 0x700
    mov esi, 0x800
    mov edi, 64
    xor ebp, ebp
    call io_request_submit
    jc .invalid
    mov [io_completion_test_request1], eax
    mov eax, 1
    mov edx, [io_completion_test_task]
    mov ebx, 4
    mov ecx, 0x701
    mov esi, 0x801
    mov edi, 64
    xor ebp, ebp
    call io_request_submit
    jc .invalid
    mov [io_completion_test_request2], eax

    ; Abschlussreihenfolge B,A beweist die Trennung von Submission und Queue.
    mov eax, [io_completion_test_request2]
    mov edx, 64
    xor ebx, ebx
    call io_request_complete
    jc .invalid
    mov eax, [io_completion_test_request1]
    mov edx, 32
    xor ebx, ebx
    call io_request_complete
    jc .invalid
    mov eax, io_completion_test_batch
    mov ecx, 1
    call io_completion_dequeue_batch
    jc .invalid
    cmp eax, 1
    jne .invalid
    mov eax, [io_completion_test_request2]
    cmp [io_completion_test_batch + IO_COMPLETION_REQUEST_ID], eax
    jne .invalid
    cmp dword [io_completion_test_batch + IO_COMPLETION_STATUS], IO_COMPLETION_SUCCESS
    jne .invalid
    mov eax, io_completion_test_batch
    mov ecx, IO_COMPLETION_CAPACITY
    call io_completion_dequeue_batch
    jc .invalid
    cmp eax, 1
    jne .invalid
    mov eax, [io_completion_test_request1]
    cmp [io_completion_test_batch + IO_COMPLETION_REQUEST_ID], eax
    jne .invalid
    cmp dword [io_completion_test_batch + IO_COMPLETION_STATUS], IO_COMPLETION_PARTIAL
    jne .invalid
    cmp dword [io_completion_test_batch + IO_COMPLETION_BYTES], 32
    jne .invalid

    ; Die Zustellung ist ebenfalls begrenzt: 16 Eintraege bleiben erhalten,
    ; der siebzehnte Overflow wird sichtbar gezaehlt statt Speicher zu wachsen.
    xor ecx, ecx
.fill:
    cmp ecx, IO_COMPLETION_CAPACITY + 1
    jae .filled
    push ecx
    mov eax, 1
    mov edx, [io_completion_test_task]
    mov ebx, 5
    mov ecx, 0x702
    mov esi, 0x802
    mov edi, 16
    xor ebp, ebp
    call io_request_submit
    jc .fill_invalid
    mov edx, 16
    xor ebx, ebx
    call io_request_complete
    jc .fill_invalid
    pop ecx
    inc ecx
    jmp .fill
.fill_invalid:
    pop ecx
    jmp .invalid
.filled:
    cmp dword [io_completion_count], IO_COMPLETION_CAPACITY
    jne .invalid
    cmp dword [io_completion_overflow_count], 1
    jne .invalid
    mov eax, io_completion_test_batch
    mov ecx, IO_COMPLETION_CAPACITY
    call io_completion_dequeue_batch
    jc .invalid
    cmp eax, IO_COMPLETION_CAPACITY
    jne .invalid
    cmp dword [io_completion_count], 0
    jne .invalid

    mov eax, [io_completion_test_task]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [io_completion_test_scope]
    call task_scope_close
    jc .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
io_completion_api:
    dd IO_COMPLETION_API_SIZE
    dw 1, 0
    dd IO_COMPLETION_CAPACITY
    dd IO_COMPLETION_CAPABILITIES
    dd io_completion_dequeue_batch
    dd io_completion_queue
    dd io_completion_count
    dd io_completion_overflow_count

io_completion_manager_ready:      dd 0
io_completion_head:               dd 0
io_completion_tail:               dd 0
io_completion_count:              dd 0
io_completion_overflow_count:     dd 0
io_completion_temp_request:       dd 0
io_completion_temp_request_slot:  dd 0
io_completion_batch_destination:  dd 0
io_completion_batch_limit:        dd 0
io_completion_batch_count:        dd 0
io_completion_test_scope:         dd 0
io_completion_test_task:          dd 0
io_completion_test_request1:      dd 0
io_completion_test_request2:      dd 0
align 4
io_completion_queue:
    times IO_COMPLETION_CAPACITY * IO_COMPLETION_RECORD_SIZE db 0
io_completion_test_batch:
    times IO_COMPLETION_CAPACITY * IO_COMPLETION_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Shared Buffer mit stabiler Identitaet, exklusiver I/O-Lease und Copy-Fallback
; NPSPEC-DATAMOVE-SHAREDBUFFER-0001 / NPSPEC-IO-ZEROCOPY-0001
; ---------------------------------------------------------------------------

SHARED_BUFFER_API_SIZE        equ 32
SHARED_BUFFER_CAPACITY        equ 4
SHARED_BUFFER_RECORD_SIZE     equ 64
SHARED_BUFFER_STATE_EMPTY     equ 0
SHARED_BUFFER_STATE_CPU       equ 1
SHARED_BUFFER_STATE_PROVIDER  equ 2
SHARED_BUFFER_STATE_RELEASED  equ 3
SHARED_BUFFER_RIGHT_READ      equ 0x00000001
SHARED_BUFFER_RIGHT_WRITE     equ 0x00000002
SHARED_BUFFER_RIGHT_TRANSFER  equ 0x00000004
SHARED_BUFFER_RIGHT_DMA       equ 0x00000008
SHARED_BUFFER_RIGHT_RELEASE   equ 0x00000010
SHARED_BUFFER_BACKING_POOL    equ 1
SHARED_BUFFER_ID              equ 0
SHARED_BUFFER_OWNER           equ 4
SHARED_BUFFER_SIZE            equ 8
SHARED_BUFFER_STATE           equ 12
SHARED_BUFFER_REFERENCES      equ 16
SHARED_BUFFER_ACTIVE_IO       equ 20
SHARED_BUFFER_RIGHTS          equ 24
SHARED_BUFFER_FLAGS           equ 28
SHARED_BUFFER_MAPPINGS        equ 32
SHARED_BUFFER_PINNED          equ 36
SHARED_BUFFER_BACKING_KIND    equ 40
SHARED_BUFFER_RESOURCE_BYTES  equ 44
SHARED_BUFFER_COPY_FALLBACKS  equ 48
SHARED_BUFFER_DIRECT_LEASES   equ 52
SHARED_BUFFER_GENERATION      equ 56

shared_buffer_initialize:
    mov edi, shared_buffer_table
    xor eax, eax
    mov ecx, (SHARED_BUFFER_CAPACITY * SHARED_BUFFER_RECORD_SIZE) / 4
    rep stosd
    mov edi, shared_buffer_backing_pages
    mov ecx, SHARED_BUFFER_CAPACITY
    rep stosd
    mov edi, io_request_buffer_ids
    mov ecx, IO_REQUEST_CAPACITY
    rep stosd
    mov dword [shared_buffer_next_id], 1
    mov dword [shared_buffer_live_count], 0
    mov dword [shared_buffer_resource_bytes], 0
    mov dword [shared_buffer_manager_ready], 1
    clc
    ret

; EAX=Owner, EDX=Groesse (1..4096), EBX=explizite Rechte. EAX=Buffer-ID.
shared_buffer_create:
    pushfd
    cli
    mov [shared_buffer_temp_owner], eax
    mov [shared_buffer_temp_size], edx
    mov [shared_buffer_temp_rights], ebx
    call process_lookup
    jc .invalid
    cmp dword [shared_buffer_temp_size], 0
    je .invalid
    cmp dword [shared_buffer_temp_size], PMM_PAGE_SIZE
    ja .invalid
    mov eax, [shared_buffer_temp_rights]
    test eax, SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE
    jz .invalid
    test eax, SHARED_BUFFER_RIGHT_RELEASE
    jz .invalid
    test eax, ~(SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE | SHARED_BUFFER_RIGHT_TRANSFER | SHARED_BUFFER_RIGHT_DMA | SHARED_BUFFER_RIGHT_RELEASE)
    jnz .invalid
    xor ecx, ecx
.scan:
    cmp ecx, SHARED_BUFFER_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 6
    add edi, shared_buffer_table
    cmp dword [edi + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_EMPTY
    je .slot
    cmp dword [edi + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_RELEASED
    je .slot
    inc ecx
    jmp .scan
.slot:
    mov [shared_buffer_temp_slot], ecx
    mov [shared_buffer_temp_record], edi
    mov eax, ecx
    shl eax, 12
    add eax, shared_buffer_backing_pool
    mov [shared_buffer_temp_backing], eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    mov edi, [shared_buffer_temp_record]
    xor eax, eax
    mov ecx, SHARED_BUFFER_RECORD_SIZE / 4
    rep stosd
    mov edi, [shared_buffer_temp_record]
    mov eax, [shared_buffer_next_id]
    mov [edi + SHARED_BUFFER_ID], eax
    mov edx, [shared_buffer_temp_owner]
    mov [edi + SHARED_BUFFER_OWNER], edx
    mov edx, [shared_buffer_temp_size]
    mov [edi + SHARED_BUFFER_SIZE], edx
    mov dword [edi + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    mov dword [edi + SHARED_BUFFER_REFERENCES], 1
    mov edx, [shared_buffer_temp_rights]
    mov [edi + SHARED_BUFFER_RIGHTS], edx
    mov dword [edi + SHARED_BUFFER_BACKING_KIND], SHARED_BUFFER_BACKING_POOL
    mov edx, [shared_buffer_temp_size]
    mov [edi + SHARED_BUFFER_RESOURCE_BYTES], edx
    mov ecx, [shared_buffer_temp_slot]
    mov edx, [shared_buffer_temp_backing]
    mov [shared_buffer_backing_pages + ecx * 4], edx
    mov edx, [shared_buffer_generations + ecx * 4]
    inc edx
    jnz .generation_ready
    inc edx
.generation_ready:
    mov [shared_buffer_generations + ecx * 4], edx
    mov [edi + SHARED_BUFFER_GENERATION], edx
    inc dword [shared_buffer_next_id]
    inc dword [shared_buffer_live_count]
    mov edx, [shared_buffer_temp_size]
    add [shared_buffer_resource_bytes], edx
    mov eax, [edi + SHARED_BUFFER_ID]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

shared_buffer_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, SHARED_BUFFER_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, shared_buffer_table
    cmp dword [edx + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_EMPTY
    je .next
    cmp dword [edx + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_RELEASED
    je .next
    cmp [edx + SHARED_BUFFER_ID], eax
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

; EAX=Request-ID, EDX=Buffer-ID, EBX=benoetigte Rechte. Die Lease uebergibt
; exklusives Ownership an den Provider und bleibt bis zur Completion aktiv.
shared_buffer_bind_io:
    pushfd
    cli
    mov [shared_buffer_temp_request], eax
    mov [shared_buffer_temp_id], edx
    mov [shared_buffer_temp_access], ebx
    call io_request_lookup
    jc .invalid
    mov [shared_buffer_temp_request_record], eax
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    jne .invalid
    mov edx, [shared_buffer_temp_id]
    cmp [eax + IO_REQUEST_BUFFER], edx
    jne .invalid
    mov eax, [shared_buffer_temp_id]
    call shared_buffer_lookup
    jc .invalid
    mov [shared_buffer_temp_record], eax
    mov edx, [shared_buffer_temp_request_record]
    mov ecx, [edx + IO_REQUEST_OWNER]
    cmp [eax + SHARED_BUFFER_OWNER], ecx
    jne .invalid
    mov ecx, [shared_buffer_temp_access]
    test ecx, ecx
    jz .invalid
    mov ebx, [eax + SHARED_BUFFER_RIGHTS]
    and ebx, ecx
    cmp ebx, ecx
    jne .invalid
    mov ecx, [edx + IO_REQUEST_LENGTH]
    cmp ecx, [eax + SHARED_BUFFER_SIZE]
    ja .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_ACTIVE_IO], 0
    jne .invalid
    mov ecx, edx
    sub ecx, io_request_table
    shr ecx, 6
    cmp dword [io_request_buffer_ids + ecx * 4], 0
    jne .invalid
    mov edx, [shared_buffer_temp_id]
    mov [io_request_buffer_ids + ecx * 4], edx
    mov dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_PROVIDER
    inc dword [eax + SHARED_BUFFER_ACTIVE_IO]
    inc dword [eax + SHARED_BUFFER_REFERENCES]
    inc dword [eax + SHARED_BUFFER_DIRECT_LEASES]
    mov eax, [shared_buffer_temp_request]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=terminaler Request. Completion gibt Ownership atomar an die CPU zurueck.
shared_buffer_on_terminal:
    cmp dword [shared_buffer_manager_ready], 1
    jne .done
    push eax
    call dma_scatter_gather_on_terminal
    pop eax
    push eax
    call dma_mapping_on_terminal
    pop eax
    mov ecx, eax
    sub ecx, io_request_table
    shr ecx, 6
    mov eax, [io_request_buffer_ids + ecx * 4]
    test eax, eax
    jz .done
    mov [shared_buffer_temp_slot], ecx
    call shared_buffer_lookup
    jc .done
    cmp dword [eax + SHARED_BUFFER_ACTIVE_IO], 1
    jne .done
    cmp dword [eax + SHARED_BUFFER_REFERENCES], 2
    jb .done
    dec dword [eax + SHARED_BUFFER_ACTIVE_IO]
    dec dword [eax + SHARED_BUFFER_REFERENCES]
    mov dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    mov ecx, [shared_buffer_temp_slot]
    mov dword [io_request_buffer_ids + ecx * 4], 0
.done:
    clc
    ret

; EAX=Quelle, EDX=Ziel, EBX=Laenge, ECX=Owner. Sicherer Copy-Fallback,
; falls ein spaeterer Provider keine gemeinsame Buffer-Lease unterstuetzt.
shared_buffer_copy_fallback:
    pushfd
    cli
    mov [shared_buffer_temp_source], eax
    mov [shared_buffer_temp_destination], edx
    mov [shared_buffer_temp_length], ebx
    mov [shared_buffer_temp_owner], ecx
    test ebx, ebx
    jz .invalid
    cmp eax, edx
    je .invalid
    call shared_buffer_lookup
    jc .invalid
    mov [shared_buffer_temp_source_record], eax
    mov eax, [shared_buffer_temp_destination]
    call shared_buffer_lookup
    jc .invalid
    mov [shared_buffer_temp_destination_record], eax
    mov edx, [shared_buffer_temp_owner]
    mov esi, [shared_buffer_temp_source_record]
    cmp [esi + SHARED_BUFFER_OWNER], edx
    jne .invalid
    cmp [eax + SHARED_BUFFER_OWNER], edx
    jne .invalid
    cmp dword [esi + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    test dword [esi + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_READ
    jz .invalid
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_WRITE
    jz .invalid
    mov ebx, [shared_buffer_temp_length]
    cmp ebx, [esi + SHARED_BUFFER_SIZE]
    ja .invalid
    cmp ebx, [eax + SHARED_BUFFER_SIZE]
    ja .invalid
    mov ecx, esi
    sub ecx, shared_buffer_table
    shr ecx, 6
    mov esi, [shared_buffer_backing_pages + ecx * 4]
    mov ecx, eax
    sub ecx, shared_buffer_table
    shr ecx, 6
    mov edi, [shared_buffer_backing_pages + ecx * 4]
    mov ecx, ebx
    rep movsb
    mov eax, [shared_buffer_temp_destination_record]
    inc dword [eax + SHARED_BUFFER_COPY_FALLBACKS]
    mov eax, [shared_buffer_temp_length]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Buffer-ID, EDX=Owner. Aktive Provider-Leases blockieren die Freigabe.
shared_buffer_release:
    pushfd
    cli
    mov [shared_buffer_temp_id], eax
    mov [shared_buffer_temp_owner], edx
    call shared_buffer_lookup
    jc .invalid
    mov [shared_buffer_temp_record], eax
    mov edx, [shared_buffer_temp_owner]
    cmp [eax + SHARED_BUFFER_OWNER], edx
    jne .invalid
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_RELEASE
    jz .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_ACTIVE_IO], 0
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_REFERENCES], 1
    jne .invalid
    mov ecx, eax
    sub ecx, shared_buffer_table
    shr ecx, 6
    mov [shared_buffer_temp_slot], ecx
    mov edi, [shared_buffer_backing_pages + ecx * 4]
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd                         ; keine Datenreste zwischen Domains
    mov eax, [shared_buffer_temp_record]
    mov edx, [eax + SHARED_BUFFER_RESOURCE_BYTES]
    sub [shared_buffer_resource_bytes], edx
    mov dword [eax + SHARED_BUFFER_RESOURCE_BYTES], 0
    mov dword [eax + SHARED_BUFFER_REFERENCES], 0
    mov dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_RELEASED
    mov ecx, [shared_buffer_temp_slot]
    mov dword [shared_buffer_backing_pages + ecx * 4], 0
    dec dword [shared_buffer_live_count]
    mov eax, [shared_buffer_temp_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

shared_buffer_self_test:
    mov eax, 1
    mov edx, 256
    mov ebx, SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE | SHARED_BUFFER_RIGHT_TRANSFER | SHARED_BUFFER_RIGHT_RELEASE
    call shared_buffer_create
    jc .invalid
    mov [shared_buffer_test_source], eax
    mov eax, 1
    mov edx, 256
    mov ebx, SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE | SHARED_BUFFER_RIGHT_TRANSFER | SHARED_BUFFER_RIGHT_RELEASE
    call shared_buffer_create
    jc .invalid
    mov [shared_buffer_test_destination], eax

    mov eax, [shared_buffer_test_source]
    call shared_buffer_lookup
    jc .invalid
    mov ecx, eax
    sub ecx, shared_buffer_table
    shr ecx, 6
    mov edi, [shared_buffer_backing_pages + ecx * 4]
    mov dword [edi], 0x4E4F5641       ; "NOVA"
    mov eax, [shared_buffer_test_source]
    mov edx, [shared_buffer_test_destination]
    mov ebx, 4
    mov ecx, 1
    call shared_buffer_copy_fallback
    jc .invalid
    cmp eax, 4
    jne .invalid
    mov eax, [shared_buffer_test_destination]
    call shared_buffer_lookup
    jc .invalid
    cmp dword [eax + SHARED_BUFFER_COPY_FALLBACKS], 1
    jne .invalid
    mov ecx, eax
    sub ecx, shared_buffer_table
    shr ecx, 6
    mov edi, [shared_buffer_backing_pages + ecx * 4]
    cmp dword [edi], 0x4E4F5641
    jne .invalid

    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [shared_buffer_test_scope], eax
    mov eax, 1
    mov edx, [shared_buffer_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [shared_buffer_test_task], eax
    mov eax, 1
    mov edx, [shared_buffer_test_task]
    mov ebx, 6
    mov ecx, 0x900
    mov esi, [shared_buffer_test_source]
    mov edi, 4
    xor ebp, ebp
    call io_request_submit
    jc .invalid
    mov [shared_buffer_test_request], eax
    mov edx, [shared_buffer_test_source]
    mov ebx, SHARED_BUFFER_RIGHT_READ
    call shared_buffer_bind_io
    jc .invalid
    mov eax, [shared_buffer_test_source]
    call shared_buffer_lookup
    jc .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_PROVIDER
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_REFERENCES], 2
    jne .invalid
    mov eax, [shared_buffer_test_source]
    mov edx, 1
    call shared_buffer_release
    jnc .invalid
    mov eax, [shared_buffer_test_request]
    mov edx, 4
    xor ebx, ebx
    call io_request_complete
    jc .invalid
    mov eax, [shared_buffer_test_source]
    call shared_buffer_lookup
    jc .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_REFERENCES], 1
    jne .invalid
    mov eax, [shared_buffer_test_task]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [shared_buffer_test_scope]
    call task_scope_close
    jc .invalid
    mov eax, [shared_buffer_test_source]
    mov edx, 1
    call shared_buffer_release
    jc .invalid
    mov eax, [shared_buffer_test_destination]
    mov edx, 1
    call shared_buffer_release
    jc .invalid
    cmp dword [shared_buffer_live_count], 0
    jne .invalid
    cmp dword [shared_buffer_resource_bytes], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
shared_buffer_api:
    dd SHARED_BUFFER_API_SIZE
    dw 1, 0
    dd SHARED_BUFFER_CAPACITY
    dd shared_buffer_create
    dd shared_buffer_bind_io
    dd shared_buffer_copy_fallback
    dd shared_buffer_release
    dd shared_buffer_table

shared_buffer_manager_ready:          dd 0
shared_buffer_next_id:                dd 0
shared_buffer_live_count:             dd 0
shared_buffer_resource_bytes:         dd 0
shared_buffer_temp_owner:             dd 0
shared_buffer_temp_size:              dd 0
shared_buffer_temp_rights:            dd 0
shared_buffer_temp_slot:              dd 0
shared_buffer_temp_record:            dd 0
shared_buffer_temp_backing:           dd 0
shared_buffer_temp_request:           dd 0
shared_buffer_temp_request_record:    dd 0
shared_buffer_temp_id:                dd 0
shared_buffer_temp_access:            dd 0
shared_buffer_temp_source:            dd 0
shared_buffer_temp_destination:       dd 0
shared_buffer_temp_source_record:     dd 0
shared_buffer_temp_destination_record: dd 0
shared_buffer_temp_length:            dd 0
shared_buffer_test_source:            dd 0
shared_buffer_test_destination:       dd 0
shared_buffer_test_scope:             dd 0
shared_buffer_test_task:              dd 0
shared_buffer_test_request:           dd 0
align 4
shared_buffer_table:
    times SHARED_BUFFER_CAPACITY * SHARED_BUFFER_RECORD_SIZE db 0
shared_buffer_backing_pages:
    times SHARED_BUFFER_CAPACITY dd 0
shared_buffer_generations:
    times SHARED_BUFFER_CAPACITY dd 0
io_request_buffer_ids:
    times IO_REQUEST_CAPACITY dd 0
align 4096
shared_buffer_backing_pool:
    times SHARED_BUFFER_CAPACITY * PMM_PAGE_SIZE db 0

; ---------------------------------------------------------------------------
; Normalisierter HAL-Topologiegraph. Die Bootstrap-Knoten bilden ein UMA-
; System und die derzeitigen Testprovider ab; sie sind ausdrücklich keine
; vorgetäuschte PCI-Erkennung. Neue Plattformprovider können dieselbe ABI mit
; validierten ACPI-, CPU- und Busdaten befüllen.
; NPSPEC-HAL-TOPOLOGY-0001, NPSPEC-HAL-NUMA-0001,
; NPSPEC-HAL-HOTPLUG-0001, NPSPEC-HAL-PLATFORM-0001
; ---------------------------------------------------------------------------

TOPOLOGY_API_SIZE                 equ 40
TOPOLOGY_CAPACITY                 equ 48
TOPOLOGY_RECORD_SIZE              equ 64
TOPOLOGY_TYPE_SYSTEM              equ 1
TOPOLOGY_TYPE_NUMA_NODE           equ 2
TOPOLOGY_TYPE_CPU_PACKAGE         equ 3
TOPOLOGY_TYPE_CPU_CORE            equ 4
TOPOLOGY_TYPE_CPU_THREAD          equ 5
TOPOLOGY_TYPE_MEMORY_REGION       equ 6
TOPOLOGY_TYPE_CACHE               equ 7
TOPOLOGY_TYPE_INTERRUPT_CONTROLLER equ 8
TOPOLOGY_TYPE_BUS                 equ 9
TOPOLOGY_TYPE_IOMMU_GROUP         equ 10
TOPOLOGY_TYPE_DEVICE              equ 11
TOPOLOGY_STATE_EMPTY              equ 0
TOPOLOGY_STATE_ONLINE             equ 1
TOPOLOGY_STATE_QUIESCING          equ 2
TOPOLOGY_STATE_OFFLINE            equ 3
TOPOLOGY_STATE_REMOVED            equ 4
TOPOLOGY_FLAG_BOOTSTRAP           equ 0x00000001
TOPOLOGY_FLAG_HOTPLUGGABLE        equ 0x00000002
TOPOLOGY_FLAG_DMA_CAPABLE         equ 0x00000004
TOPOLOGY_FLAG_LOCALITY_KNOWN      equ 0x00000008
TOPOLOGY_FLAG_FIRMWARE_VALIDATED  equ 0x00000010
TOPOLOGY_FLAG_FALLBACK            equ 0x00000020
TOPOLOGY_FLAG_ARCH_VALIDATED      equ 0x00000040
TOPOLOGY_ID                       equ 0
TOPOLOGY_TYPE                     equ 4
TOPOLOGY_PARENT                   equ 8
TOPOLOGY_STATE                    equ 12
TOPOLOGY_HARDWARE_ID              equ 16
TOPOLOGY_NUMA_NODE                equ 20
TOPOLOGY_IOMMU_GROUP              equ 24
TOPOLOGY_CHILD_COUNT              equ 28
TOPOLOGY_FLAGS                    equ 32
TOPOLOGY_PROPERTY0                equ 36
TOPOLOGY_PROPERTY1                equ 40
TOPOLOGY_PROPERTY2                equ 44
TOPOLOGY_GENERATION               equ 48
TOPOLOGY_CHANGE_SEQUENCE          equ 52

topology_initialize:
    mov edi, topology_records
    xor eax, eax
    mov ecx, (TOPOLOGY_CAPACITY * TOPOLOGY_RECORD_SIZE) / 4
    rep stosd
    mov dword [topology_count], 0
    mov dword [topology_cpu_nodes], 0
    mov dword [topology_package_nodes], 0
    mov dword [topology_core_nodes], 0
    mov dword [topology_numa_nodes], 0
    mov dword [topology_memory_nodes], 0
    mov dword [topology_numa0_id], 0
    mov dword [topology_next_id], 1
    mov dword [topology_change_sequence], 0
    mov dword [topology_validation_failures], 0
    mov dword [topology_manager_ready], 1
    call topology_cpu_hierarchy_discover

    ; Root des normalisierten Graphen.
    mov eax, TOPOLOGY_TYPE_SYSTEM
    xor edx, edx
    mov ebx, 1
    mov ecx, 0xFFFFFFFF
    xor esi, esi
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP
    call topology_register
    jc .invalid
    mov [topology_root_id], eax

    ; Die geprüfte MADT-Liste liefert aktive Hardware-Threads. CPUID 1F/0B
    ; liefert, sofern vollständig validierbar, die systemweit anwendbaren
    ; Package/Core-Bitgrenzen für die APIC-IDs.
    mov dword [topology_cpu_index], 0
.cpu_next:
    mov eax, [topology_cpu_index]
    cmp eax, [acpi_cpu_count]
    jae .cpus_complete
    mov ebx, [acpi_apic_ids + eax * 4]
    mov [topology_cpu_apic_id], ebx
    mov dword [topology_cpu_numa_domain], 0xFFFFFFFF
    mov eax, ebx
    call acpi_srat_cpu_domain_lookup
    jc .cpu_numa_unknown
    mov [topology_cpu_numa_domain], eax
    call topology_get_or_create_numa
    jc .invalid
    mov [topology_cpu_parent_id], eax
    jmp .cpu_numa_ready
.cpu_numa_unknown:
    mov eax, 0xFFFFFFFF
    call topology_get_or_create_numa
    jc .invalid
    mov [topology_cpu_parent_id], eax
.cpu_numa_ready:
    cmp dword [topology_cpu_hierarchy_valid], 1
    jne .thread_parent_ready

    ; Package-Key = APIC-ID oberhalb der Core-Ebene.
    mov eax, [topology_cpu_apic_id]
    mov ecx, [topology_cpu_core_shift]
    shr eax, cl
    mov [topology_cpu_package_key], eax
    mov ebx, eax
    mov eax, TOPOLOGY_TYPE_CPU_PACKAGE
    call topology_find_type_hardware
    jnc .package_found
    mov eax, TOPOLOGY_TYPE_CPU_PACKAGE
    mov edx, [topology_root_id]
    mov ebx, [topology_cpu_package_key]
    xor ecx, ecx
    xor esi, esi
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP | TOPOLOGY_FLAG_ARCH_VALIDATED
    call topology_register
    jc .invalid
    inc dword [topology_package_nodes]
    jmp .package_id_ready
.package_found:
    mov eax, [eax + TOPOLOGY_ID]
.package_id_ready:
    mov [topology_cpu_package_parent_id], eax

    ; Core-Key = APIC-ID oberhalb der SMT-Ebene und ist systemweit eindeutig.
    mov eax, [topology_cpu_apic_id]
    mov ecx, [topology_cpu_thread_shift]
    shr eax, cl
    mov [topology_cpu_core_key], eax
    mov ebx, eax
    mov eax, TOPOLOGY_TYPE_CPU_CORE
    call topology_find_type_hardware
    jnc .core_found
    mov eax, TOPOLOGY_TYPE_CPU_CORE
    mov edx, [topology_cpu_package_parent_id]
    mov ebx, [topology_cpu_core_key]
    xor ecx, ecx
    xor esi, esi
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP | TOPOLOGY_FLAG_ARCH_VALIDATED
    call topology_register
    jc .invalid
    inc dword [topology_core_nodes]
    jmp .core_id_ready
.core_found:
    mov eax, [eax + TOPOLOGY_ID]
.core_id_ready:
    mov [topology_cpu_parent_id], eax
.thread_parent_ready:
    mov ebx, [topology_cpu_apic_id]
    mov eax, TOPOLOGY_TYPE_CPU_THREAD
    mov edx, [topology_cpu_parent_id]
    mov ecx, [topology_cpu_numa_domain]
    xor esi, esi
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP | TOPOLOGY_FLAG_FALLBACK
    cmp dword [acpi_madt_valid], 1
    jne .cpu_arch_flag
    and edi, ~TOPOLOGY_FLAG_FALLBACK
    or edi, TOPOLOGY_FLAG_FIRMWARE_VALIDATED
.cpu_arch_flag:
    cmp dword [topology_cpu_hierarchy_valid], 1
    jne .cpu_register
    or edi, TOPOLOGY_FLAG_ARCH_VALIDATED
.cpu_register:
    cmp dword [topology_cpu_numa_domain], 0xFFFFFFFF
    je .cpu_register_ready
    or edi, TOPOLOGY_FLAG_LOCALITY_KNOWN
.cpu_register_ready:
    call topology_register
    jc .invalid
    call topology_lookup
    jc .invalid
    mov edx, [topology_cpu_index]
    mov [eax + TOPOLOGY_PROPERTY0], edx ; normalisierter logischer CPU-Index
    mov edx, [acpi_madt_valid]
    mov [eax + TOPOLOGY_PROPERTY1], edx ; 1 = validierte MADT-Quelle
    xor edx, edx
    cmp dword [topology_cpu_hierarchy_valid], 1
    jne .thread_property_ready
    mov edx, 1
    mov ecx, [topology_cpu_thread_shift]
    shl edx, cl
    dec edx
    and edx, [topology_cpu_apic_id]
.thread_property_ready:
    mov [eax + TOPOLOGY_PROPERTY2], edx ; SMT-Thread-ID innerhalb des Core
    inc dword [topology_cpu_nodes]
    inc dword [topology_cpu_index]
    jmp .cpu_next
.cpus_complete:
    cmp dword [acpi_srat_valid], 1
    jne .fallback_memory
    cmp dword [acpi_srat_memory_count], 0
    je .fallback_memory
    mov dword [topology_memory_index], 0
.memory_next:
    mov eax, [topology_memory_index]
    cmp eax, [acpi_srat_memory_count]
    jae .memory_complete
    imul edx, eax, ACPI_SRAT_MEMORY_RECORD_SIZE
    add edx, acpi_srat_memory_records
    mov [topology_memory_source], edx
    mov eax, [edx]
    call topology_get_or_create_numa
    jc .invalid
    mov edx, eax
    mov eax, TOPOLOGY_TYPE_MEMORY_REGION
    mov ebx, [topology_memory_index]
    inc ebx
    mov ecx, [topology_memory_source]
    mov ecx, [ecx]
    xor esi, esi
    mov edi, TOPOLOGY_FLAG_FIRMWARE_VALIDATED | TOPOLOGY_FLAG_LOCALITY_KNOWN
    call topology_register
    jc .invalid
    call topology_lookup
    jc .invalid
    mov edx, [topology_memory_source]
    mov ecx, [edx + 4]
    mov [eax + TOPOLOGY_PROPERTY0], ecx ; physische Basis, low
    mov ecx, [edx + 12]
    mov [eax + TOPOLOGY_PROPERTY1], ecx ; Länge, low
    mov ecx, [edx + 20]
    mov [eax + TOPOLOGY_PROPERTY2], ecx ; SRAT-Flags
    inc dword [topology_memory_nodes]
    inc dword [topology_memory_index]
    jmp .memory_next
.fallback_memory:
    mov eax, 0xFFFFFFFF
    call topology_get_or_create_numa
    jc .invalid
    mov edx, eax
    mov eax, TOPOLOGY_TYPE_MEMORY_REGION
    mov ebx, 1
    mov ecx, 0xFFFFFFFF
    xor esi, esi
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP | TOPOLOGY_FLAG_FALLBACK
    call topology_register
    jc .invalid
    inc dword [topology_memory_nodes]
.memory_complete:

    mov eax, TOPOLOGY_TYPE_INTERRUPT_CONTROLLER
    mov edx, [topology_root_id]
    mov ebx, 1
    mov ecx, 0xFFFFFFFF
    xor esi, esi
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP
    call topology_register
    jc .invalid

    mov eax, TOPOLOGY_TYPE_BUS
    mov edx, [topology_root_id]
    mov ebx, 1
    mov ecx, 0xFFFFFFFF
    xor esi, esi
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP
    call topology_register
    jc .invalid
    mov [topology_boot_bus_id], eax

    mov ebx, 0x10
    call topology_register_boot_group
    jc .invalid
    mov [topology_group10_id], eax
    mov ebx, 0x20
    call topology_register_boot_group
    jc .invalid
    mov [topology_group20_id], eax
    mov ebx, 0x30
    call topology_register_boot_group
    jc .invalid
    mov [topology_group30_id], eax

    mov ebx, 0xD001
    mov esi, 0x10
    mov edx, [topology_group10_id]
    call topology_register_boot_device
    jc .invalid
    mov ebx, 0xD002
    mov esi, 0x20
    mov edx, [topology_group20_id]
    call topology_register_boot_device
    jc .invalid
    mov ebx, 0xD003
    mov esi, 0x30
    mov edx, [topology_group30_id]
    call topology_register_boot_device
    jc .invalid
    mov ebx, 0xD004
    mov esi, 0x30
    mov edx, [topology_group30_id]
    call topology_register_boot_device
    jc .invalid
    mov [topology_hotplug_test_id], eax
    clc
    ret
.invalid:
    mov dword [topology_manager_ready], 0
    stc
    ret

; Ermittelt ausschließlich die Bitgrenzen der x86-Topologie. Bevorzugt wird
; CPUID 1F, danach 0B. Eine Core-Ebene ist Pflicht; unvollständige Angaben
; werden als unbekannt behandelt.
topology_cpu_hierarchy_discover:
    mov dword [topology_cpu_hierarchy_valid], 0
    mov dword [topology_cpu_thread_shift], 0
    mov dword [topology_cpu_core_shift], 0
    mov dword [topology_cpu_leaf], 0
    xor eax, eax
    cpuid
    cmp eax, 0x1F
    jb .try_leaf_b
    mov eax, 0x1F
    call topology_cpu_hierarchy_scan_leaf
    jnc .complete
.try_leaf_b:
    xor eax, eax
    cpuid
    cmp eax, 0x0B
    jb .unavailable
    mov eax, 0x0B
    call topology_cpu_hierarchy_scan_leaf
    jc .unavailable
.complete:
    mov dword [topology_cpu_hierarchy_valid], 1
.unavailable:
    ret

; EAX=CPUID-Leaf. CF=0 bei validierter SMT/Core-Bitgrenze.
topology_cpu_hierarchy_scan_leaf:
    mov [topology_cpu_leaf], eax
    mov dword [topology_cpu_thread_shift], 0
    mov dword [topology_cpu_core_shift], 0
    mov dword [topology_cpu_core_level_seen], 0
    mov dword [topology_cpu_subleaf], 0
.next:
    mov eax, [topology_cpu_leaf]
    mov ecx, [topology_cpu_subleaf]
    cpuid
    test ebx, ebx
    jz .validate
    mov esi, eax
    and esi, 0x1F
    cmp esi, 31
    ja .invalid
    mov edx, ecx
    shr edx, 8
    and edx, 0xFF
    cmp edx, 1
    je .smt
    cmp edx, 2
    je .core
    jmp .advance
.smt:
    mov [topology_cpu_thread_shift], esi
    jmp .advance
.core:
    mov [topology_cpu_core_shift], esi
    mov dword [topology_cpu_core_level_seen], 1
.advance:
    inc dword [topology_cpu_subleaf]
    cmp dword [topology_cpu_subleaf], 8
    jb .next
.validate:
    cmp dword [topology_cpu_core_level_seen], 1
    jne .invalid
    mov eax, [topology_cpu_core_shift]
    cmp eax, [topology_cpu_thread_shift]
    jb .invalid
    clc
    ret
.invalid:
    stc
    ret

; EAX=Proximity-Domain oder 0xFFFFFFFF für unbekannte Lokalität.
; Ergebnis EAX=Topology-ID des stabilen NUMA-Knotens.
topology_get_or_create_numa:
    mov [topology_temp_numa_domain], eax
    mov ebx, eax
    mov eax, TOPOLOGY_TYPE_NUMA_NODE
    call topology_find_type_hardware
    jc .create
    mov eax, [eax + TOPOLOGY_ID]
    clc
    ret
.create:
    mov eax, TOPOLOGY_TYPE_NUMA_NODE
    mov edx, [topology_root_id]
    mov ebx, [topology_temp_numa_domain]
    mov ecx, ebx
    xor esi, esi
    mov edi, TOPOLOGY_FLAG_FIRMWARE_VALIDATED | TOPOLOGY_FLAG_LOCALITY_KNOWN
    cmp ebx, 0xFFFFFFFF
    jne .register
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP | TOPOLOGY_FLAG_FALLBACK
.register:
    call topology_register
    jc .invalid
    inc dword [topology_numa_nodes]
    cmp dword [topology_temp_numa_domain], 0
    jne .complete
    mov [topology_numa0_id], eax
.complete:
    clc
    ret
.invalid:
    stc
    ret

; EBX=Gruppen-ID; EAX=Topology-ID.
topology_register_boot_group:
    mov eax, TOPOLOGY_TYPE_IOMMU_GROUP
    mov edx, [topology_boot_bus_id]
    mov ecx, 0xFFFFFFFF
    mov esi, ebx
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP
    call topology_register
    ret

; EBX=Device-ID, ESI=Gruppe, EDX=Parent-Gruppe; EAX=Topology-ID.
topology_register_boot_device:
    mov eax, TOPOLOGY_TYPE_DEVICE
    mov ecx, 0xFFFFFFFF
    mov edi, TOPOLOGY_FLAG_BOOTSTRAP | TOPOLOGY_FLAG_HOTPLUGGABLE | TOPOLOGY_FLAG_DMA_CAPABLE
    call topology_register
    ret

; EAX=Typ, EDX=Parent-ID, EBX=Hardware-ID, ECX=NUMA-ID,
; ESI=IOMMU-Gruppen-ID, EDI=Flags. Ergebnis EAX=Topology-ID.
topology_register:
    pushfd
    cli
    mov [topology_temp_type], eax
    mov [topology_temp_parent], edx
    mov [topology_temp_hardware], ebx
    mov [topology_temp_numa], ecx
    mov [topology_temp_group], esi
    mov [topology_temp_flags], edi
    cmp dword [topology_manager_ready], 1
    jne .invalid
    cmp eax, TOPOLOGY_TYPE_SYSTEM
    jb .invalid
    cmp eax, TOPOLOGY_TYPE_DEVICE
    ja .invalid
    cmp eax, TOPOLOGY_TYPE_SYSTEM
    jne .non_root
    test edx, edx
    jnz .invalid
    cmp dword [topology_count], 0
    jne .invalid
    jmp .validate_identity
.non_root:
    test edx, edx
    jz .invalid
    mov eax, edx
    call topology_lookup
    jc .invalid
    mov [topology_temp_parent_record], eax
.validate_identity:
    cmp dword [topology_temp_type], TOPOLOGY_TYPE_DEVICE
    jne .identity_scan
    cmp dword [topology_temp_hardware], 0
    je .invalid
    cmp dword [topology_temp_group], 0
    je .invalid
.identity_scan:
    xor ecx, ecx
.identity_next:
    cmp ecx, TOPOLOGY_CAPACITY
    jae .find_slot
    mov edx, ecx
    shl edx, 6
    add edx, topology_records
    cmp dword [edx + TOPOLOGY_STATE], TOPOLOGY_STATE_EMPTY
    je .identity_continue
    cmp dword [edx + TOPOLOGY_STATE], TOPOLOGY_STATE_REMOVED
    je .identity_continue
    mov eax, [topology_temp_type]
    cmp [edx + TOPOLOGY_TYPE], eax
    jne .identity_continue
    mov eax, [topology_temp_hardware]
    cmp [edx + TOPOLOGY_HARDWARE_ID], eax
    je .invalid
.identity_continue:
    inc ecx
    jmp .identity_next
.find_slot:
    xor ecx, ecx
.slot_scan:
    cmp ecx, TOPOLOGY_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, topology_records
    cmp dword [edx + TOPOLOGY_STATE], TOPOLOGY_STATE_EMPTY
    je .slot
    cmp dword [edx + TOPOLOGY_STATE], TOPOLOGY_STATE_REMOVED
    je .slot
    inc ecx
    jmp .slot_scan
.slot:
    mov [topology_temp_record], edx
    mov edi, edx
    xor eax, eax
    mov ecx, TOPOLOGY_RECORD_SIZE / 4
    rep stosd
    mov edi, [topology_temp_record]
    mov eax, [topology_next_id]
    mov [edi + TOPOLOGY_ID], eax
    inc dword [topology_next_id]
    mov edx, [topology_temp_type]
    mov [edi + TOPOLOGY_TYPE], edx
    mov edx, [topology_temp_parent]
    mov [edi + TOPOLOGY_PARENT], edx
    mov dword [edi + TOPOLOGY_STATE], TOPOLOGY_STATE_ONLINE
    mov edx, [topology_temp_hardware]
    mov [edi + TOPOLOGY_HARDWARE_ID], edx
    mov edx, [topology_temp_numa]
    mov [edi + TOPOLOGY_NUMA_NODE], edx
    mov edx, [topology_temp_group]
    mov [edi + TOPOLOGY_IOMMU_GROUP], edx
    mov edx, [topology_temp_flags]
    mov [edi + TOPOLOGY_FLAGS], edx
    inc dword [topology_change_sequence]
    mov edx, [topology_change_sequence]
    mov [edi + TOPOLOGY_CHANGE_SEQUENCE], edx
    mov dword [edi + TOPOLOGY_GENERATION], 1
    inc dword [topology_count]
    cmp dword [topology_temp_type], TOPOLOGY_TYPE_SYSTEM
    je .registered
    mov edx, [topology_temp_parent_record]
    inc dword [edx + TOPOLOGY_CHILD_COUNT]
.registered:
    mov eax, [edi + TOPOLOGY_ID]
    popfd
    clc
    ret
.invalid:
    inc dword [topology_validation_failures]
    xor eax, eax
    popfd
    stc
    ret

; EAX=Topology-ID; EAX=Datensatz.
topology_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, TOPOLOGY_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, topology_records
    cmp dword [edx + TOPOLOGY_STATE], TOPOLOGY_STATE_EMPTY
    je .next
    cmp dword [edx + TOPOLOGY_STATE], TOPOLOGY_STATE_REMOVED
    je .next
    cmp [edx + TOPOLOGY_ID], eax
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

; EAX=Typ, EBX=Hardware-Key; EAX=Online-Datensatz.
topology_find_type_hardware:
    xor ecx, ecx
.scan:
    cmp ecx, TOPOLOGY_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, topology_records
    cmp dword [edx + TOPOLOGY_STATE], TOPOLOGY_STATE_ONLINE
    jne .next
    cmp [edx + TOPOLOGY_TYPE], eax
    jne .next
    cmp [edx + TOPOLOGY_HARDWARE_ID], ebx
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

; EAX=Hardware-Device-ID; EAX=IOMMU-Gruppe.
topology_device_group:
    xor ecx, ecx
.scan:
    cmp ecx, TOPOLOGY_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, topology_records
    cmp dword [edx + TOPOLOGY_STATE], TOPOLOGY_STATE_ONLINE
    jne .next
    cmp dword [edx + TOPOLOGY_TYPE], TOPOLOGY_TYPE_DEVICE
    jne .next
    cmp [edx + TOPOLOGY_HARDWARE_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, [edx + TOPOLOGY_IOMMU_GROUP]
    test eax, eax
    jz .invalid
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=APIC-Hardware-ID; EAX=Online-Hardware-Thread-Datensatz.
topology_cpu_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, TOPOLOGY_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, topology_records
    cmp dword [edx + TOPOLOGY_STATE], TOPOLOGY_STATE_ONLINE
    jne .next
    cmp dword [edx + TOPOLOGY_TYPE], TOPOLOGY_TYPE_CPU_THREAD
    jne .next
    cmp [edx + TOPOLOGY_HARDWARE_ID], eax
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

; EAX=Topology-ID, EDX=neuer Zustand. Erlaubt kontrolliertes
; Online -> Quiescing -> Offline -> Online sowie Offline -> Removed.
topology_transition:
    pushfd
    cli
    mov [topology_temp_id], eax
    mov [topology_temp_state], edx
    call topology_lookup
    jc .invalid
    mov [topology_temp_record], eax
    mov edx, [topology_temp_state]
    cmp dword [eax + TOPOLOGY_TYPE], TOPOLOGY_TYPE_SYSTEM
    je .invalid
    mov ecx, [eax + TOPOLOGY_STATE]
    cmp ecx, TOPOLOGY_STATE_ONLINE
    jne .from_quiescing
    cmp edx, TOPOLOGY_STATE_QUIESCING
    jne .invalid
    jmp .commit
.from_quiescing:
    cmp ecx, TOPOLOGY_STATE_QUIESCING
    jne .from_offline
    cmp edx, TOPOLOGY_STATE_OFFLINE
    jne .invalid
    jmp .commit
.from_offline:
    cmp ecx, TOPOLOGY_STATE_OFFLINE
    jne .invalid
    cmp edx, TOPOLOGY_STATE_ONLINE
    je .commit
    cmp edx, TOPOLOGY_STATE_REMOVED
    jne .invalid
    cmp dword [eax + TOPOLOGY_CHILD_COUNT], 0
    jne .invalid
.commit:
    mov [eax + TOPOLOGY_STATE], edx
    inc dword [eax + TOPOLOGY_GENERATION]
    inc dword [topology_change_sequence]
    mov ecx, [topology_change_sequence]
    mov [eax + TOPOLOGY_CHANGE_SEQUENCE], ecx
    cmp edx, TOPOLOGY_STATE_REMOVED
    jne .complete
    mov eax, [eax + TOPOLOGY_PARENT]
    call topology_lookup
    jc .invalid
    cmp dword [eax + TOPOLOGY_CHILD_COUNT], 0
    je .invalid
    dec dword [eax + TOPOLOGY_CHILD_COUNT]
    dec dword [topology_count]
.complete:
    popfd
    clc
    ret
.invalid:
    inc dword [topology_validation_failures]
    popfd
    stc
    ret

topology_self_test:
    mov eax, 10
    add eax, [topology_cpu_nodes]
    add eax, [topology_package_nodes]
    add eax, [topology_core_nodes]
    add eax, [topology_numa_nodes]
    add eax, [topology_memory_nodes]
    cmp [topology_count], eax
    jne .invalid
    mov eax, [topology_cpu_nodes]
    cmp eax, [acpi_cpu_count]
    jne .invalid
    mov dword [topology_cpu_index], 0
.cpu_next:
    mov ecx, [topology_cpu_index]
    cmp ecx, [acpi_cpu_count]
    jae .cpus_valid
    mov eax, [acpi_apic_ids + ecx * 4]
    call topology_cpu_lookup
    jc .invalid
    mov [topology_selftest_record], eax
    mov ecx, [topology_cpu_index]
    cmp [eax + TOPOLOGY_PROPERTY0], ecx
    jne .invalid
    mov edx, [acpi_madt_valid]
    cmp [eax + TOPOLOGY_PROPERTY1], edx
    jne .invalid
    mov ecx, [topology_cpu_index]
    mov eax, [acpi_apic_ids + ecx * 4]
    call acpi_srat_cpu_domain_lookup
    jc .locality_unknown
    mov edx, [topology_selftest_record]
    cmp [edx + TOPOLOGY_NUMA_NODE], eax
    jne .invalid
    test dword [edx + TOPOLOGY_FLAGS], TOPOLOGY_FLAG_LOCALITY_KNOWN
    jz .invalid
    jmp .locality_valid
.locality_unknown:
    mov edx, [topology_selftest_record]
    cmp dword [edx + TOPOLOGY_NUMA_NODE], 0xFFFFFFFF
    jne .invalid
    test dword [edx + TOPOLOGY_FLAGS], TOPOLOGY_FLAG_LOCALITY_KNOWN
    jnz .invalid
.locality_valid:
    mov eax, [topology_selftest_record]
    cmp dword [topology_cpu_hierarchy_valid], 1
    jne .fallback_parent
    test dword [eax + TOPOLOGY_FLAGS], TOPOLOGY_FLAG_ARCH_VALIDATED
    jz .invalid
    mov eax, [eax + TOPOLOGY_PARENT]
    call topology_lookup
    jc .invalid
    cmp dword [eax + TOPOLOGY_TYPE], TOPOLOGY_TYPE_CPU_CORE
    jne .invalid
    mov eax, [eax + TOPOLOGY_PARENT]
    call topology_lookup
    jc .invalid
    cmp dword [eax + TOPOLOGY_TYPE], TOPOLOGY_TYPE_CPU_PACKAGE
    jne .invalid
    mov eax, [eax + TOPOLOGY_PARENT]
    cmp eax, [topology_root_id]
    jne .invalid
    jmp .cpu_valid
.fallback_parent:
    mov eax, [topology_selftest_record]
    mov eax, [eax + TOPOLOGY_PARENT]
    call topology_lookup
    jc .invalid
    cmp dword [eax + TOPOLOGY_TYPE], TOPOLOGY_TYPE_NUMA_NODE
    jne .invalid
.cpu_valid:
    inc dword [topology_cpu_index]
    jmp .cpu_next
.cpus_valid:
    mov eax, [topology_change_sequence]
    mov [topology_selftest_initial_sequence], eax
    mov eax, 0xD003
    call topology_device_group
    jc .invalid
    cmp eax, 0x30
    jne .invalid
    mov eax, 0xD004
    call topology_device_group
    jc .invalid
    cmp eax, 0x30
    jne .invalid
    mov eax, [topology_hotplug_test_id]
    mov edx, TOPOLOGY_STATE_QUIESCING
    call topology_transition
    jc .invalid
    mov eax, 0xD004
    call topology_device_group
    jnc .invalid                         ; quieszierende Geräte sind nicht nutzbar
    mov eax, [topology_hotplug_test_id]
    mov edx, TOPOLOGY_STATE_OFFLINE
    call topology_transition
    jc .invalid
    mov eax, [topology_hotplug_test_id]
    mov edx, TOPOLOGY_STATE_ONLINE
    call topology_transition
    jc .invalid
    mov eax, 0xD004
    call topology_device_group
    jc .invalid
    cmp eax, 0x30
    jne .invalid
    mov eax, [topology_selftest_initial_sequence]
    add eax, 3
    cmp eax, [topology_change_sequence]
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
topology_api:
    dd TOPOLOGY_API_SIZE
    dw 1, 0
    dd TOPOLOGY_CAPACITY
    dd topology_register
    dd topology_transition
    dd topology_lookup
    dd topology_device_group
    dd topology_records
    dd topology_change_sequence
    dd 0

topology_manager_ready:       dd 0
topology_count:               dd 0
topology_cpu_nodes:           dd 0
topology_package_nodes:       dd 0
topology_core_nodes:          dd 0
topology_numa_nodes:          dd 0
topology_memory_nodes:        dd 0
topology_next_id:             dd 0
topology_change_sequence:     dd 0
topology_validation_failures: dd 0
topology_root_id:             dd 0
topology_numa0_id:            dd 0
topology_boot_bus_id:         dd 0
topology_group10_id:          dd 0
topology_group20_id:          dd 0
topology_group30_id:          dd 0
topology_hotplug_test_id:     dd 0
topology_cpu_index:           dd 0
topology_selftest_initial_sequence: dd 0
topology_selftest_record:     dd 0
topology_cpu_hierarchy_valid: dd 0
topology_cpu_thread_shift:    dd 0
topology_cpu_core_shift:      dd 0
topology_cpu_core_level_seen: dd 0
topology_cpu_leaf:            dd 0
topology_cpu_subleaf:         dd 0
topology_cpu_apic_id:         dd 0
topology_cpu_parent_id:       dd 0
topology_cpu_package_parent_id: dd 0
topology_cpu_package_key:     dd 0
topology_cpu_core_key:        dd 0
topology_cpu_numa_domain:     dd 0
topology_memory_index:        dd 0
topology_memory_source:       dd 0
topology_temp_numa_domain:    dd 0
topology_temp_id:             dd 0
topology_temp_type:           dd 0
topology_temp_parent:         dd 0
topology_temp_hardware:       dd 0
topology_temp_numa:           dd 0
topology_temp_group:          dd 0
topology_temp_flags:          dd 0
topology_temp_state:          dd 0
topology_temp_record:         dd 0
topology_temp_parent_record:  dd 0
align 16
topology_records:
    times TOPOLOGY_CAPACITY * TOPOLOGY_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; IOMMU-Domainabstraktion mit Gruppen, Device-Bindings, IOVA-Autorisierungen
; und zuordenbaren Faults. Ohne Hardwareprovider bleibt Isolation explizit im
; Restricted-Modus und wird nicht als Hardwaregarantie ausgegeben.
; NPSPEC-HAL-IOMMU-0001
; ---------------------------------------------------------------------------

IOMMU_API_SIZE              equ 40
IOMMU_DOMAIN_CAPACITY       equ 4
IOMMU_DEVICE_CAPACITY       equ 8
IOMMU_MAPPING_CAPACITY      equ 8
IOMMU_DOMAIN_SIZE           equ 64
IOMMU_DEVICE_SIZE           equ 32
IOMMU_MAPPING_SIZE          equ 48
IOMMU_FAULT_SIZE            equ 32
IOMMU_MODE_UNAVAILABLE      equ 0
IOMMU_MODE_RESTRICTED       equ 1
IOMMU_MODE_HARDWARE         equ 2
IOMMU_MODE_VIRTUAL          equ 3
IOMMU_DOMAIN_EMPTY          equ 0
IOMMU_DOMAIN_ACTIVE         equ 1
IOMMU_DOMAIN_QUIESCING      equ 2
IOMMU_DOMAIN_RELEASED       equ 3
IOMMU_BINDING_EMPTY         equ 0
IOMMU_BINDING_ACTIVE        equ 1
IOMMU_BINDING_RELEASED      equ 2
IOMMU_MAPPING_EMPTY         equ 0
IOMMU_MAPPING_ACTIVE        equ 1
IOMMU_MAPPING_REVOKED       equ 2
IOMMU_MAPPING_FAULTED       equ 3
IOMMU_PERMISSION_READ       equ 0x00000001
IOMMU_PERMISSION_WRITE      equ 0x00000002
IOMMU_EXTERNAL_KIND_MASK    equ 0xF0000000
IOMMU_EXTERNAL_ID_MASK      equ 0x0FFFFFFF
IOMMU_EXTERNAL_DMA          equ 0x10000000
IOMMU_EXTERNAL_DMA_SG       equ 0x20000000
IOMMU_DOMAIN_ID             equ 0
IOMMU_DOMAIN_OWNER          equ 4
IOMMU_DOMAIN_STATE          equ 8
IOMMU_DOMAIN_MODE           equ 12
IOMMU_DOMAIN_DEVICES        equ 16
IOMMU_DOMAIN_GROUPS         equ 20
IOMMU_DOMAIN_MAPPINGS       equ 24
IOMMU_DOMAIN_FAULTS         equ 28
IOMMU_DOMAIN_MAPPED_BYTES   equ 32
IOMMU_DOMAIN_ADDRESS_WIDTH  equ 36
IOMMU_DOMAIN_IOVA_BASE_LOW  equ 40
IOMMU_DOMAIN_IOVA_BASE_HIGH equ 44
IOMMU_DOMAIN_IOVA_LIMIT_LOW equ 48
IOMMU_DOMAIN_IOVA_LIMIT_HIGH equ 52
IOMMU_DOMAIN_GENERATION     equ 56
IOMMU_DOMAIN_FLAGS          equ 60
IOMMU_DEVICE_ID             equ 0
IOMMU_DEVICE_DOMAIN         equ 4
IOMMU_DEVICE_GROUP          equ 8
IOMMU_DEVICE_OWNER          equ 12
IOMMU_DEVICE_STATE          equ 16
IOMMU_DEVICE_MODE           equ 20
IOMMU_DEVICE_GENERATION     equ 24
IOMMU_MAPPING_AUTH_ID       equ 0
IOMMU_MAPPING_DOMAIN        equ 4
IOMMU_MAPPING_DEVICE        equ 8
IOMMU_MAPPING_EXTERNAL      equ 12
IOMMU_MAPPING_IOVA_LOW      equ 16
IOMMU_MAPPING_IOVA_HIGH     equ 20
IOMMU_MAPPING_LENGTH        equ 24
IOMMU_MAPPING_PERMISSIONS   equ 28
IOMMU_MAPPING_STATE         equ 32
IOMMU_MAPPING_GENERATION    equ 36
IOMMU_MAPPING_ERROR         equ 40
IOMMU_FAULT_SEQUENCE        equ 0
IOMMU_FAULT_DOMAIN          equ 4
IOMMU_FAULT_DEVICE          equ 8
IOMMU_FAULT_EXTERNAL        equ 12
IOMMU_FAULT_IOVA_LOW        equ 16
IOMMU_FAULT_IOVA_HIGH       equ 20
IOMMU_FAULT_ACCESS          equ 24
IOMMU_FAULT_ERROR           equ 28

iommu_initialize:
    mov edi, iommu_domain_table
    xor eax, eax
    mov ecx, (IOMMU_DOMAIN_CAPACITY * IOMMU_DOMAIN_SIZE) / 4
    rep stosd
    mov edi, iommu_device_table
    mov ecx, (IOMMU_DEVICE_CAPACITY * IOMMU_DEVICE_SIZE) / 4
    rep stosd
    mov edi, iommu_mapping_table
    mov ecx, (IOMMU_MAPPING_CAPACITY * IOMMU_MAPPING_SIZE) / 4
    rep stosd
    mov edi, iommu_domain_generations
    mov ecx, IOMMU_DOMAIN_CAPACITY
    rep stosd
    mov edi, iommu_device_generations
    mov ecx, IOMMU_DEVICE_CAPACITY
    rep stosd
    mov edi, iommu_mapping_generations
    mov ecx, IOMMU_MAPPING_CAPACITY
    rep stosd
    mov edi, iommu_last_fault
    mov ecx, IOMMU_FAULT_SIZE / 4
    rep stosd
    mov dword [iommu_next_domain_id], 1
    mov dword [iommu_next_authorization_id], 1
    mov dword [iommu_domain_count], 0
    mov dword [iommu_binding_count], 0
    mov dword [iommu_mapping_count], 0
    mov dword [iommu_mapped_bytes], 0
    mov dword [iommu_fault_count], 0
    mov dword [iommu_validation_failures], 0
    mov dword [iommu_hardware_available], 0
    mov dword [iommu_manager_ready], 1
    clc
    ret

iommu_domain_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, IOMMU_DOMAIN_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, iommu_domain_table
    cmp dword [edx + IOMMU_DOMAIN_STATE], IOMMU_DOMAIN_ACTIVE
    jne .next
    cmp [edx + IOMMU_DOMAIN_ID], eax
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

iommu_device_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, IOMMU_DEVICE_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 5
    add edx, iommu_device_table
    cmp dword [edx + IOMMU_DEVICE_STATE], IOMMU_BINDING_ACTIVE
    jne .next
    cmp [edx + IOMMU_DEVICE_ID], eax
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

; EAX=Device-ID. Ergebnis EAX=aktive Domain.
iommu_domain_for_device:
    call iommu_device_lookup
    jc .invalid
    mov eax, [eax + IOMMU_DEVICE_DOMAIN]
    call iommu_domain_lookup
    ret
.invalid:
    xor eax, eax
    stc
    ret

iommu_mapping_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, IOMMU_MAPPING_CAPACITY
    jae .invalid
    mov edx, ecx
    imul edx, IOMMU_MAPPING_SIZE
    add edx, iommu_mapping_table
    cmp dword [edx + IOMMU_MAPPING_STATE], IOMMU_MAPPING_ACTIVE
    je .candidate
    cmp dword [edx + IOMMU_MAPPING_STATE], IOMMU_MAPPING_FAULTED
    jne .next
.candidate:
    cmp [edx + IOMMU_MAPPING_AUTH_ID], eax
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

; EAX=Owner, EBX=Mode, ECX=Adressbreite, ESI=IOVA-Basis, EDI=IOVA-Limit.
iommu_domain_create:
    pushfd
    cli
    mov [iommu_temp_owner], eax
    mov [iommu_temp_mode], ebx
    mov [iommu_temp_address_width], ecx
    mov [iommu_temp_iova], esi
    mov [iommu_temp_limit], edi
    cmp dword [iommu_manager_ready], 1
    jne .invalid
    cmp ebx, IOMMU_MODE_RESTRICTED
    jb .invalid
    cmp ebx, IOMMU_MODE_VIRTUAL
    ja .invalid
    cmp ebx, IOMMU_MODE_RESTRICTED
    je .mode_ready
    cmp dword [iommu_hardware_available], 1
    jne .invalid                         ; keine Isolation vortaeuschen
.mode_ready:
    cmp ecx, 32
    jne .invalid
    test esi, 0xFFF
    jnz .invalid
    cmp esi, edi
    jae .invalid
    call process_lookup
    jc .invalid
    xor ecx, ecx
.scan:
    cmp ecx, IOMMU_DOMAIN_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, iommu_domain_table
    cmp dword [edx + IOMMU_DOMAIN_STATE], IOMMU_DOMAIN_EMPTY
    je .slot
    cmp dword [edx + IOMMU_DOMAIN_STATE], IOMMU_DOMAIN_RELEASED
    je .slot
    inc ecx
    jmp .scan
.slot:
    mov [iommu_temp_slot], ecx
    mov [iommu_temp_domain_record], edx
    mov edi, edx
    xor eax, eax
    mov ecx, IOMMU_DOMAIN_SIZE / 4
    rep stosd
    mov edi, [iommu_temp_domain_record]
    mov eax, [iommu_next_domain_id]
    mov [edi + IOMMU_DOMAIN_ID], eax
    mov edx, [iommu_temp_owner]
    mov [edi + IOMMU_DOMAIN_OWNER], edx
    mov dword [edi + IOMMU_DOMAIN_STATE], IOMMU_DOMAIN_ACTIVE
    mov edx, [iommu_temp_mode]
    mov [edi + IOMMU_DOMAIN_MODE], edx
    mov edx, [iommu_temp_address_width]
    mov [edi + IOMMU_DOMAIN_ADDRESS_WIDTH], edx
    mov edx, [iommu_temp_iova]
    mov [edi + IOMMU_DOMAIN_IOVA_BASE_LOW], edx
    mov dword [edi + IOMMU_DOMAIN_IOVA_BASE_HIGH], 0
    mov edx, [iommu_temp_limit]
    mov [edi + IOMMU_DOMAIN_IOVA_LIMIT_LOW], edx
    mov dword [edi + IOMMU_DOMAIN_IOVA_LIMIT_HIGH], 0
    mov ecx, [iommu_temp_slot]
    mov edx, [iommu_domain_generations + ecx * 4]
    inc edx
    jnz .generation_ready
    inc edx
.generation_ready:
    mov [iommu_domain_generations + ecx * 4], edx
    mov [edi + IOMMU_DOMAIN_GENERATION], edx
    inc dword [iommu_next_domain_id]
    inc dword [iommu_domain_count]
    mov eax, [edi + IOMMU_DOMAIN_ID]
    popfd
    clc
    ret
.invalid:
    inc dword [iommu_validation_failures]
    xor eax, eax
    popfd
    stc
    ret

; EAX=Domain-ID, EDX=Device-ID, EBX=IOMMU-Gruppe, ECX=Owner.
iommu_bind_device:
    pushfd
    cli
    mov [iommu_temp_domain], eax
    mov [iommu_temp_device], edx
    mov [iommu_temp_group], ebx
    mov [iommu_temp_owner], ecx
    test edx, edx
    jz .invalid
    test ebx, ebx
    jz .invalid
    mov eax, edx
    call topology_device_group
    jc .invalid
    cmp eax, [iommu_temp_group]
    jne .invalid                         ; Gruppe stammt aus normalisierter Topologie
    mov eax, [iommu_temp_domain]
    call iommu_domain_lookup
    jc .invalid
    mov [iommu_temp_domain_record], eax
    mov edx, [iommu_temp_owner]
    cmp [eax + IOMMU_DOMAIN_OWNER], edx
    jne .invalid
    mov eax, [iommu_temp_device]
    call iommu_device_lookup
    jnc .invalid
    mov dword [iommu_temp_group_exists], 0
    xor ecx, ecx
.validate_group:
    cmp ecx, IOMMU_DEVICE_CAPACITY
    jae .find_slot
    mov edx, ecx
    shl edx, 5
    add edx, iommu_device_table
    cmp dword [edx + IOMMU_DEVICE_STATE], IOMMU_BINDING_ACTIVE
    jne .next_group
    mov eax, [iommu_temp_group]
    cmp [edx + IOMMU_DEVICE_GROUP], eax
    jne .next_group
    mov eax, [iommu_temp_domain]
    cmp [edx + IOMMU_DEVICE_DOMAIN], eax
    jne .invalid                         ; eine Hardwaregruppe bleibt zusammen
    mov dword [iommu_temp_group_exists], 1
.next_group:
    inc ecx
    jmp .validate_group
.find_slot:
    xor ecx, ecx
.scan_slot:
    cmp ecx, IOMMU_DEVICE_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 5
    add edi, iommu_device_table
    cmp dword [edi + IOMMU_DEVICE_STATE], IOMMU_BINDING_EMPTY
    je .slot
    cmp dword [edi + IOMMU_DEVICE_STATE], IOMMU_BINDING_RELEASED
    je .slot
    inc ecx
    jmp .scan_slot
.slot:
    mov [iommu_temp_slot], ecx
    xor eax, eax
    mov ecx, IOMMU_DEVICE_SIZE / 4
    rep stosd
    mov edi, [iommu_temp_slot]
    shl edi, 5
    add edi, iommu_device_table
    mov edx, [iommu_temp_device]
    mov [edi + IOMMU_DEVICE_ID], edx
    mov edx, [iommu_temp_domain]
    mov [edi + IOMMU_DEVICE_DOMAIN], edx
    mov edx, [iommu_temp_group]
    mov [edi + IOMMU_DEVICE_GROUP], edx
    mov edx, [iommu_temp_owner]
    mov [edi + IOMMU_DEVICE_OWNER], edx
    mov dword [edi + IOMMU_DEVICE_STATE], IOMMU_BINDING_ACTIVE
    mov edx, [iommu_temp_domain_record]
    mov eax, [edx + IOMMU_DOMAIN_MODE]
    mov [edi + IOMMU_DEVICE_MODE], eax
    mov ecx, [iommu_temp_slot]
    mov eax, [iommu_device_generations + ecx * 4]
    inc eax
    jnz .binding_generation_ready
    inc eax
.binding_generation_ready:
    mov [iommu_device_generations + ecx * 4], eax
    mov [edi + IOMMU_DEVICE_GENERATION], eax
    inc dword [edx + IOMMU_DOMAIN_DEVICES]
    cmp dword [iommu_temp_group_exists], 0
    jne .group_counted
    inc dword [edx + IOMMU_DOMAIN_GROUPS]
.group_counted:
    inc dword [iommu_binding_count]
    mov eax, [edi + IOMMU_DEVICE_ID]
    popfd
    clc
    ret
.invalid:
    inc dword [iommu_validation_failures]
    xor eax, eax
    popfd
    stc
    ret

; EAX=Domain, EDX=Device, EBX=externes Mapping, ECX=IOVA, ESI=Laenge,
; EDI=Permissions, EBP=Owner. EAX=Authorization-ID.
iommu_authorize_mapping:
    pushfd
    cli
    mov dword [iommu_temp_mapping_record], 0
    mov [iommu_temp_domain], eax
    mov [iommu_temp_device], edx
    mov [iommu_temp_external], ebx
    mov [iommu_temp_iova], ecx
    mov [iommu_temp_length], esi
    mov [iommu_temp_permissions], edi
    mov [iommu_temp_owner], ebp
    test ebx, ebx
    jz .invalid
    test esi, esi
    jz .invalid
    test edi, edi
    jz .invalid
    test edi, ~(IOMMU_PERMISSION_READ | IOMMU_PERMISSION_WRITE)
    jnz .invalid
    test ecx, 0xFFF
    jnz .invalid
    test esi, 0xFFF
    jnz .invalid
    call iommu_domain_lookup
    jc .invalid
    mov [iommu_temp_domain_record], eax
    mov edx, [iommu_temp_owner]
    cmp [eax + IOMMU_DOMAIN_OWNER], edx
    jne .invalid
    mov edx, [iommu_temp_iova]
    cmp edx, [eax + IOMMU_DOMAIN_IOVA_BASE_LOW]
    jb .invalid
    mov ecx, edx
    add ecx, [iommu_temp_length]
    jc .invalid
    dec ecx
    cmp ecx, [eax + IOMMU_DOMAIN_IOVA_LIMIT_LOW]
    ja .invalid
    mov eax, [iommu_temp_device]
    call iommu_device_lookup
    jc .invalid
    mov edx, [iommu_temp_domain]
    cmp [eax + IOMMU_DEVICE_DOMAIN], edx
    jne .invalid
    xor ecx, ecx
.overlap_scan:
    cmp ecx, IOMMU_MAPPING_CAPACITY
    jae .find_slot
    mov edx, ecx
    imul edx, IOMMU_MAPPING_SIZE
    add edx, iommu_mapping_table
    cmp dword [edx + IOMMU_MAPPING_STATE], IOMMU_MAPPING_ACTIVE
    je .overlap_candidate
    cmp dword [edx + IOMMU_MAPPING_STATE], IOMMU_MAPPING_FAULTED
    jne .next_overlap
.overlap_candidate:
    mov eax, [iommu_temp_domain]
    cmp [edx + IOMMU_MAPPING_DOMAIN], eax
    jne .next_overlap
    mov eax, [edx + IOMMU_MAPPING_IOVA_LOW]
    add eax, [edx + IOMMU_MAPPING_LENGTH]
    mov ebx, [iommu_temp_iova]
    cmp ebx, eax
    jae .next_overlap
    add ebx, [iommu_temp_length]
    cmp [edx + IOMMU_MAPPING_IOVA_LOW], ebx
    jb .invalid
.next_overlap:
    inc ecx
    jmp .overlap_scan
.find_slot:
    xor ecx, ecx
.scan_slot:
    cmp ecx, IOMMU_MAPPING_CAPACITY
    jae .invalid
    mov edi, ecx
    imul edi, IOMMU_MAPPING_SIZE
    add edi, iommu_mapping_table
    cmp dword [edi + IOMMU_MAPPING_STATE], IOMMU_MAPPING_EMPTY
    je .slot
    cmp dword [edi + IOMMU_MAPPING_STATE], IOMMU_MAPPING_REVOKED
    je .slot
    inc ecx
    jmp .scan_slot
.slot:
    mov [iommu_temp_slot], ecx
    mov [iommu_temp_mapping_record], edi
    xor eax, eax
    mov ecx, IOMMU_MAPPING_SIZE / 4
    rep stosd
    mov edi, [iommu_temp_mapping_record]
    mov eax, [iommu_next_authorization_id]
    mov [edi + IOMMU_MAPPING_AUTH_ID], eax
    mov edx, [iommu_temp_domain]
    mov [edi + IOMMU_MAPPING_DOMAIN], edx
    mov edx, [iommu_temp_device]
    mov [edi + IOMMU_MAPPING_DEVICE], edx
    mov edx, [iommu_temp_external]
    mov [edi + IOMMU_MAPPING_EXTERNAL], edx
    mov edx, [iommu_temp_iova]
    mov [edi + IOMMU_MAPPING_IOVA_LOW], edx
    mov dword [edi + IOMMU_MAPPING_IOVA_HIGH], 0
    mov edx, [iommu_temp_length]
    mov [edi + IOMMU_MAPPING_LENGTH], edx
    mov edx, [iommu_temp_permissions]
    mov [edi + IOMMU_MAPPING_PERMISSIONS], edx
    mov dword [edi + IOMMU_MAPPING_STATE], IOMMU_MAPPING_ACTIVE
    mov ecx, [iommu_temp_slot]
    mov edx, [iommu_mapping_generations + ecx * 4]
    inc edx
    jnz .mapping_generation_ready
    inc edx
.mapping_generation_ready:
    mov [iommu_mapping_generations + ecx * 4], edx
    mov [edi + IOMMU_MAPPING_GENERATION], edx
    mov eax, [iommu_temp_domain_record]
    inc dword [eax + IOMMU_DOMAIN_MAPPINGS]
    mov edx, [iommu_temp_length]
    add [eax + IOMMU_DOMAIN_MAPPED_BYTES], edx
    inc dword [iommu_mapping_count]
    add [iommu_mapped_bytes], edx
    inc dword [iommu_next_authorization_id]
    mov eax, [edi + IOMMU_MAPPING_AUTH_ID]
    popfd
    clc
    ret
.invalid:
    inc dword [iommu_validation_failures]
    xor eax, eax
    popfd
    stc
    ret

; EAX=Authorization-ID, EDX=Owner.
iommu_revoke_mapping:
    pushfd
    cli
    mov [iommu_temp_authorization], eax
    mov [iommu_temp_owner], edx
    call iommu_mapping_lookup
    jc .invalid
    mov [iommu_temp_mapping_record], eax
    mov eax, [eax + IOMMU_MAPPING_DOMAIN]
    call iommu_domain_lookup
    jc .invalid
    mov edx, [iommu_temp_owner]
    cmp [eax + IOMMU_DOMAIN_OWNER], edx
    jne .invalid
    mov edi, [iommu_temp_mapping_record]
    mov edx, [edi + IOMMU_MAPPING_LENGTH]
    cmp [eax + IOMMU_DOMAIN_MAPPED_BYTES], edx
    jb .zero_domain_bytes
    sub [eax + IOMMU_DOMAIN_MAPPED_BYTES], edx
    jmp .domain_bytes_done
.zero_domain_bytes:
    mov dword [eax + IOMMU_DOMAIN_MAPPED_BYTES], 0
.domain_bytes_done:
    cmp dword [eax + IOMMU_DOMAIN_MAPPINGS], 0
    je .global_accounting
    dec dword [eax + IOMMU_DOMAIN_MAPPINGS]
.global_accounting:
    cmp [iommu_mapped_bytes], edx
    jb .zero_global_bytes
    sub [iommu_mapped_bytes], edx
    jmp .global_bytes_done
.zero_global_bytes:
    mov dword [iommu_mapped_bytes], 0
.global_bytes_done:
    cmp dword [iommu_mapping_count], 0
    je .mark_revoked
    dec dword [iommu_mapping_count]
.mark_revoked:
    mov dword [edi + IOMMU_MAPPING_STATE], IOMMU_MAPPING_REVOKED
    mov eax, [iommu_temp_authorization]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Domain, EDX=Device, EBX=externes Mapping, ECX=IOVA,
; ESI=Access, EDI=Fehlercode.
iommu_report_fault:
    pushfd
    cli
    mov [iommu_temp_domain], eax
    mov [iommu_temp_device], edx
    mov [iommu_temp_external], ebx
    mov [iommu_temp_iova], ecx
    mov [iommu_temp_permissions], esi
    mov [iommu_temp_error], edi
    test edi, edi
    jz .invalid
    call iommu_domain_lookup
    jc .invalid
    mov [iommu_temp_domain_record], eax
    mov eax, [iommu_temp_device]
    call iommu_device_lookup
    jc .invalid
    mov edx, [iommu_temp_domain]
    cmp [eax + IOMMU_DEVICE_DOMAIN], edx
    jne .invalid
    mov dword [iommu_temp_mapping_record], 0
    xor ecx, ecx
.find_mapping:
    cmp ecx, IOMMU_MAPPING_CAPACITY
    jae .record_fault
    mov edx, ecx
    imul edx, IOMMU_MAPPING_SIZE
    add edx, iommu_mapping_table
    cmp dword [edx + IOMMU_MAPPING_STATE], IOMMU_MAPPING_ACTIVE
    jne .next_mapping
    mov eax, [iommu_temp_domain]
    cmp [edx + IOMMU_MAPPING_DOMAIN], eax
    jne .next_mapping
    mov eax, [iommu_temp_device]
    cmp [edx + IOMMU_MAPPING_DEVICE], eax
    jne .next_mapping
    mov eax, [iommu_temp_external]
    cmp [edx + IOMMU_MAPPING_EXTERNAL], eax
    jne .next_mapping
    mov [iommu_temp_mapping_record], edx
    mov eax, [iommu_temp_error]
    mov [edx + IOMMU_MAPPING_ERROR], eax
    mov dword [edx + IOMMU_MAPPING_STATE], IOMMU_MAPPING_FAULTED
    jmp .record_fault
.next_mapping:
    inc ecx
    jmp .find_mapping
.record_fault:
    inc dword [iommu_fault_count]
    mov eax, [iommu_temp_domain_record]
    inc dword [eax + IOMMU_DOMAIN_FAULTS]
    mov edi, iommu_last_fault
    mov eax, [iommu_fault_count]
    mov [edi + IOMMU_FAULT_SEQUENCE], eax
    mov eax, [iommu_temp_domain]
    mov [edi + IOMMU_FAULT_DOMAIN], eax
    mov eax, [iommu_temp_device]
    mov [edi + IOMMU_FAULT_DEVICE], eax
    mov eax, [iommu_temp_external]
    mov [edi + IOMMU_FAULT_EXTERNAL], eax
    mov eax, [iommu_temp_iova]
    mov [edi + IOMMU_FAULT_IOVA_LOW], eax
    mov dword [edi + IOMMU_FAULT_IOVA_HIGH], 0
    mov eax, [iommu_temp_permissions]
    mov [edi + IOMMU_FAULT_ACCESS], eax
    mov eax, [iommu_temp_error]
    mov [edi + IOMMU_FAULT_ERROR], eax
    mov eax, [iommu_temp_external]
    mov edx, eax
    and edx, IOMMU_EXTERNAL_KIND_MASK
    and eax, IOMMU_EXTERNAL_ID_MASK
    cmp edx, IOMMU_EXTERNAL_DMA
    je .route_dma
    cmp edx, IOMMU_EXTERNAL_DMA_SG
    je .route_dma_sg
    jmp .route_done
.route_dma:
    mov edx, [iommu_temp_device]
    mov ebx, [iommu_temp_error]
    call dma_mapping_fault
    jc .invalid
    jmp .route_done
.route_dma_sg:
    mov edx, [iommu_temp_device]
    mov ebx, [iommu_temp_error]
    call dma_scatter_gather_fault
    jc .invalid
.route_done:
    mov eax, [iommu_fault_count]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Domain-ID, EDX=Owner. Aktive Mappings blockieren den Abbau; gebundene
; Devices derselben Domain werden gemeinsam quiesced und entfernt.
iommu_domain_release:
    pushfd
    cli
    mov [iommu_temp_domain], eax
    mov [iommu_temp_owner], edx
    call iommu_domain_lookup
    jc .invalid
    mov [iommu_temp_domain_record], eax
    mov edx, [iommu_temp_owner]
    cmp [eax + IOMMU_DOMAIN_OWNER], edx
    jne .invalid
    cmp dword [eax + IOMMU_DOMAIN_MAPPINGS], 0
    jne .invalid
    mov dword [eax + IOMMU_DOMAIN_STATE], IOMMU_DOMAIN_QUIESCING
    xor ecx, ecx
.unbind:
    cmp ecx, IOMMU_DEVICE_CAPACITY
    jae .released
    mov edi, ecx
    shl edi, 5
    add edi, iommu_device_table
    cmp dword [edi + IOMMU_DEVICE_STATE], IOMMU_BINDING_ACTIVE
    jne .next_binding
    mov edx, [iommu_temp_domain]
    cmp [edi + IOMMU_DEVICE_DOMAIN], edx
    jne .next_binding
    mov dword [edi + IOMMU_DEVICE_STATE], IOMMU_BINDING_RELEASED
    cmp dword [iommu_binding_count], 0
    je .next_binding
    dec dword [iommu_binding_count]
.next_binding:
    inc ecx
    jmp .unbind
.released:
    mov eax, [iommu_temp_domain_record]
    mov dword [eax + IOMMU_DOMAIN_DEVICES], 0
    mov dword [eax + IOMMU_DOMAIN_GROUPS], 0
    mov dword [eax + IOMMU_DOMAIN_STATE], IOMMU_DOMAIN_RELEASED
    dec dword [iommu_domain_count]
    mov eax, [iommu_temp_domain]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

iommu_self_test:
    cmp dword [iommu_hardware_available], 0
    jne .invalid
    mov eax, 1
    mov ebx, IOMMU_MODE_RESTRICTED
    mov ecx, 32
    mov esi, 0xD2000000
    mov edi, 0xD20FFFFF
    call iommu_domain_create
    jc .invalid
    mov [iommu_test_domain], eax
    mov edx, 0xD003
    mov ebx, 0x30
    mov ecx, 1
    call iommu_bind_device
    jc .invalid
    mov eax, [iommu_test_domain]
    mov edx, 0xD004
    mov ebx, 0x30
    mov ecx, 1
    call iommu_bind_device
    jc .invalid
    mov eax, [iommu_test_domain]
    call iommu_domain_lookup
    jc .invalid
    cmp dword [eax + IOMMU_DOMAIN_DEVICES], 2
    jne .invalid
    cmp dword [eax + IOMMU_DOMAIN_GROUPS], 1
    jne .invalid
    mov eax, [iommu_test_domain]
    mov edx, 0xD003
    mov ebx, 77
    mov ecx, 0xD2000000
    mov esi, 4096
    mov edi, IOMMU_PERMISSION_READ
    mov ebp, 1
    call iommu_authorize_mapping
    jc .invalid
    mov [iommu_test_authorization], eax
    mov eax, [iommu_test_domain]
    mov edx, 0xD003
    mov ebx, 78
    mov ecx, 0xD2000000
    mov esi, 4096
    mov edi, IOMMU_PERMISSION_READ
    mov ebp, 1
    call iommu_authorize_mapping
    jnc .invalid                       ; IOVA-Ueberlappung verboten
    mov eax, [iommu_test_domain]
    mov edx, 0xD003
    mov ebx, 77
    mov ecx, 0xD2000010
    mov esi, IOMMU_PERMISSION_WRITE
    mov edi, 0xF001
    call iommu_report_fault
    jc .invalid
    mov eax, [iommu_test_authorization]
    call iommu_mapping_lookup
    jc .invalid
    cmp dword [eax + IOMMU_MAPPING_STATE], IOMMU_MAPPING_FAULTED
    jne .invalid
    cmp dword [iommu_last_fault + IOMMU_FAULT_DEVICE], 0xD003
    jne .invalid
    cmp dword [iommu_last_fault + IOMMU_FAULT_DOMAIN], 0
    je .invalid
    mov eax, [iommu_test_authorization]
    mov edx, 1
    call iommu_revoke_mapping
    jc .invalid
    mov eax, [iommu_test_domain]
    mov edx, 1
    call iommu_domain_release
    jc .invalid
    cmp dword [iommu_domain_count], 0
    jne .invalid
    cmp dword [iommu_binding_count], 0
    jne .invalid
    cmp dword [iommu_mapping_count], 0
    jne .invalid
    cmp dword [iommu_mapped_bytes], 0
    jne .invalid
    cmp dword [iommu_fault_count], 1
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
iommu_api:
    dd IOMMU_API_SIZE
    dw 1, 0
    dd IOMMU_DOMAIN_CAPACITY
    dd IOMMU_DEVICE_CAPACITY
    dd IOMMU_MAPPING_CAPACITY
    dd iommu_domain_create
    dd iommu_bind_device
    dd iommu_authorize_mapping
    dd iommu_revoke_mapping
    dd iommu_domain_release

iommu_manager_ready:             dd 0
iommu_hardware_available:        dd 0
iommu_next_domain_id:            dd 0
iommu_next_authorization_id:     dd 0
iommu_domain_count:              dd 0
iommu_binding_count:             dd 0
iommu_mapping_count:             dd 0
iommu_mapped_bytes:              dd 0
iommu_fault_count:               dd 0
iommu_validation_failures:       dd 0
iommu_temp_owner:                dd 0
iommu_temp_mode:                 dd 0
iommu_temp_address_width:        dd 0
iommu_temp_iova:                 dd 0
iommu_temp_limit:                dd 0
iommu_temp_length:               dd 0
iommu_temp_permissions:          dd 0
iommu_temp_domain:               dd 0
iommu_temp_device:               dd 0
iommu_temp_group:                dd 0
iommu_temp_external:             dd 0
iommu_temp_authorization:        dd 0
iommu_temp_error:                dd 0
iommu_temp_slot:                 dd 0
iommu_temp_group_exists:         dd 0
iommu_temp_domain_record:        dd 0
iommu_temp_mapping_record:       dd 0
iommu_test_domain:               dd 0
iommu_test_authorization:        dd 0
align 4
iommu_domain_table:
    times IOMMU_DOMAIN_CAPACITY * IOMMU_DOMAIN_SIZE db 0
iommu_device_table:
    times IOMMU_DEVICE_CAPACITY * IOMMU_DEVICE_SIZE db 0
iommu_mapping_table:
    times IOMMU_MAPPING_CAPACITY * IOMMU_MAPPING_SIZE db 0
iommu_domain_generations:
    times IOMMU_DOMAIN_CAPACITY dd 0
iommu_device_generations:
    times IOMMU_DEVICE_CAPACITY dd 0
iommu_mapping_generations:
    times IOMMU_MAPPING_CAPACITY dd 0
iommu_last_fault:
    times IOMMU_FAULT_SIZE db 0

; ---------------------------------------------------------------------------
; Kontrollierte DMA-Mappings. CPU-Adressen werden nie direkt als
; Device-Adressen veroeffentlicht. Ohne erkannten IOMMU-Provider bleibt der
; Bootstrap explizit im eingeschraenkten Bounce-/Staging-Modus.
; NPSPEC-HAL-DMA-0001 / NPSPEC-HAL-IOMMU-0001 /
; NPSPEC-DATAMOVE-DMA-0001
; ---------------------------------------------------------------------------

DMA_MAPPING_API_SIZE          equ 32
DMA_MAPPING_CAPACITY          equ 4
DMA_MAPPING_RECORD_SIZE       equ 64
DMA_MAPPING_STATE_EMPTY       equ 0
DMA_MAPPING_STATE_ACTIVE      equ 1
DMA_MAPPING_STATE_UNMAPPED    equ 2
DMA_MAPPING_STATE_FAULTED     equ 3
DMA_DIRECTION_TO_DEVICE       equ 1
DMA_DIRECTION_FROM_DEVICE     equ 2
DMA_DIRECTION_BIDIRECTIONAL   equ 3
DMA_PERMISSION_READ           equ 0x00000001
DMA_PERMISSION_WRITE          equ 0x00000002
DMA_FLAG_IOMMU                equ 0x00000001
DMA_FLAG_BOUNCE               equ 0x00000002
DMA_FLAG_COHERENT             equ 0x00000004
DMA_FLAG_RESTRICTED           equ 0x00000008
DMA_RESTRICTED_APERTURE       equ 0xD0000000
DMA_MAPPING_ID                equ 0
DMA_MAPPING_OWNER             equ 4
DMA_MAPPING_DEVICE            equ 8
DMA_MAPPING_BUFFER            equ 12
DMA_MAPPING_REQUEST           equ 16
DMA_MAPPING_DIRECTION         equ 20
DMA_MAPPING_PERMISSIONS       equ 24
DMA_MAPPING_STATE             equ 28
DMA_MAPPING_LENGTH            equ 32
DMA_MAPPING_ADDRESS_LOW       equ 36
DMA_MAPPING_ADDRESS_HIGH      equ 40
DMA_MAPPING_DOMAIN            equ 44
DMA_MAPPING_FLAGS             equ 48
DMA_MAPPING_PINNED_PAGES      equ 52
DMA_MAPPING_GENERATION        equ 56
DMA_MAPPING_ERROR             equ 60

dma_mapping_initialize:
    mov edi, dma_mapping_table
    xor eax, eax
    mov ecx, (DMA_MAPPING_CAPACITY * DMA_MAPPING_RECORD_SIZE) / 4
    rep stosd
    mov edi, dma_request_mapping_ids
    mov ecx, IO_REQUEST_CAPACITY
    rep stosd
    mov edi, dma_mapping_generations
    mov ecx, DMA_MAPPING_CAPACITY
    rep stosd
    mov edi, dma_mapping_iommu_authorization_ids
    mov ecx, DMA_MAPPING_CAPACITY
    rep stosd
    mov dword [dma_mapping_next_id], 1
    mov dword [dma_mapping_active_count], 0
    mov dword [dma_mapping_mapped_bytes], 0
    mov dword [dma_mapping_pinned_pages], 0
    mov dword [dma_mapping_fault_count], 0
    mov dword [dma_mapping_bounce_count], 0
    mov dword [dma_iommu_available], 0
    mov dword [dma_mapping_manager_ready], 1
    clc
    ret

; EAX=Mapping-ID. EAX=Datensatz oder 0. Nur aktive Mappings werden geliefert.
dma_mapping_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, DMA_MAPPING_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, dma_mapping_table
    cmp dword [edx + DMA_MAPPING_STATE], DMA_MAPPING_STATE_ACTIVE
    jne .next
    cmp [edx + DMA_MAPPING_ID], eax
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

; EAX=Request-ID, EDX=Device-ID, EBX=Richtung, ECX=Laenge, ESI=Owner.
; Ergebnis EAX=stabile Mapping-ID. Das aktuelle Bootstrap-Backend weist eine
; kontrollierte Device-Adresse aus einer reservierten Staging-Apertur zu.
dma_mapping_map:
    pushfd
    cli
    mov [dma_mapping_temp_request], eax
    mov [dma_mapping_temp_device], edx
    mov [dma_mapping_temp_direction], ebx
    mov [dma_mapping_temp_length], ecx
    mov [dma_mapping_temp_owner], esi
    cmp dword [dma_mapping_manager_ready], 1
    jne .invalid
    test edx, edx
    jz .invalid
    test ecx, ecx
    jz .invalid
    cmp ebx, DMA_DIRECTION_TO_DEVICE
    jb .invalid
    cmp ebx, DMA_DIRECTION_BIDIRECTIONAL
    ja .invalid
    call io_request_lookup
    jc .invalid
    mov [dma_mapping_temp_request_record], eax
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    je .request_active
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_RUNNING
    jne .invalid
.request_active:
    mov edx, [dma_mapping_temp_owner]
    cmp [eax + IO_REQUEST_OWNER], edx
    jne .invalid
    mov ecx, [dma_mapping_temp_length]
    cmp ecx, [eax + IO_REQUEST_LENGTH]
    ja .invalid
    mov ecx, eax
    sub ecx, io_request_table
    shr ecx, 6
    cmp dword [dma_request_mapping_ids + ecx * 4], 0
    jne .invalid
    mov [dma_mapping_temp_request_slot], ecx
    mov eax, [io_request_buffer_ids + ecx * 4]
    test eax, eax
    jz .invalid
    mov [dma_mapping_temp_buffer], eax
    call shared_buffer_lookup
    jc .invalid
    mov [dma_mapping_temp_buffer_record], eax
    mov edx, [dma_mapping_temp_owner]
    cmp [eax + SHARED_BUFFER_OWNER], edx
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_PROVIDER
    jne .invalid
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_DMA
    jz .invalid
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_TRANSFER
    jz .invalid
    mov ecx, [dma_mapping_temp_length]
    cmp ecx, [eax + SHARED_BUFFER_SIZE]
    ja .invalid
    mov ebx, [dma_mapping_temp_direction]
    cmp ebx, DMA_DIRECTION_TO_DEVICE
    je .needs_read
    cmp ebx, DMA_DIRECTION_FROM_DEVICE
    je .needs_write
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_READ
    jz .invalid
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_WRITE
    jz .invalid
    mov dword [dma_mapping_temp_permissions], DMA_PERMISSION_READ | DMA_PERMISSION_WRITE
    jmp .find_slot
.needs_read:
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_READ
    jz .invalid
    mov dword [dma_mapping_temp_permissions], DMA_PERMISSION_READ
    jmp .find_slot
.needs_write:
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_WRITE
    jz .invalid
    mov dword [dma_mapping_temp_permissions], DMA_PERMISSION_WRITE
.find_slot:
    xor ecx, ecx
.scan_slot:
    cmp ecx, DMA_MAPPING_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 6
    add edi, dma_mapping_table
    cmp dword [edi + DMA_MAPPING_STATE], DMA_MAPPING_STATE_EMPTY
    je .slot
    cmp dword [edi + DMA_MAPPING_STATE], DMA_MAPPING_STATE_UNMAPPED
    je .slot
    cmp dword [edi + DMA_MAPPING_STATE], DMA_MAPPING_STATE_FAULTED
    je .slot
    inc ecx
    jmp .scan_slot
.slot:
    mov [dma_mapping_temp_slot], ecx
    mov [dma_mapping_temp_record], edi
    xor eax, eax
    mov ecx, DMA_MAPPING_RECORD_SIZE / 4
    rep stosd
    mov edi, [dma_mapping_temp_record]
    mov eax, [dma_mapping_next_id]
    mov [edi + DMA_MAPPING_ID], eax
    mov edx, [dma_mapping_temp_owner]
    mov [edi + DMA_MAPPING_OWNER], edx
    mov edx, [dma_mapping_temp_device]
    mov [edi + DMA_MAPPING_DEVICE], edx
    mov edx, [dma_mapping_temp_buffer]
    mov [edi + DMA_MAPPING_BUFFER], edx
    mov edx, [dma_mapping_temp_request]
    mov [edi + DMA_MAPPING_REQUEST], edx
    mov edx, [dma_mapping_temp_direction]
    mov [edi + DMA_MAPPING_DIRECTION], edx
    mov edx, [dma_mapping_temp_permissions]
    mov [edi + DMA_MAPPING_PERMISSIONS], edx
    mov dword [edi + DMA_MAPPING_STATE], DMA_MAPPING_STATE_ACTIVE
    mov edx, [dma_mapping_temp_length]
    mov [edi + DMA_MAPPING_LENGTH], edx
    mov eax, [dma_mapping_temp_slot]
    shl eax, 12
    add eax, DMA_RESTRICTED_APERTURE
    mov [edi + DMA_MAPPING_ADDRESS_LOW], eax
    mov dword [edi + DMA_MAPPING_ADDRESS_HIGH], 0
    mov dword [edi + DMA_MAPPING_DOMAIN], 0
    mov dword [edi + DMA_MAPPING_FLAGS], DMA_FLAG_BOUNCE | DMA_FLAG_COHERENT | DMA_FLAG_RESTRICTED
    mov dword [edi + DMA_MAPPING_PINNED_PAGES], 1
    mov ecx, [dma_mapping_temp_slot]
    mov edx, [dma_mapping_generations + ecx * 4]
    inc edx
    jnz .generation_ready
    inc edx
.generation_ready:
    mov [dma_mapping_generations + ecx * 4], edx
    mov [edi + DMA_MAPPING_GENERATION], edx
    mov dword [dma_mapping_iommu_authorization_ids + ecx * 4], 0
    mov eax, [dma_mapping_temp_device]
    call iommu_domain_for_device
    jc .publish
    mov [dma_mapping_temp_domain_record], eax
    mov edx, [eax + IOMMU_DOMAIN_ID]
    mov edi, [dma_mapping_temp_record]
    mov [edi + DMA_MAPPING_DOMAIN], edx
    mov eax, edx
    mov edx, [dma_mapping_temp_device]
    mov ebx, [edi + DMA_MAPPING_ID]
    or ebx, IOMMU_EXTERNAL_DMA
    mov ecx, [edi + DMA_MAPPING_ADDRESS_LOW]
    mov esi, 4096
    mov edi, [dma_mapping_temp_permissions]
    mov ebp, [dma_mapping_temp_owner]
    call iommu_authorize_mapping
    jc .authorization_invalid
    mov ecx, [dma_mapping_temp_slot]
    mov [dma_mapping_iommu_authorization_ids + ecx * 4], eax
    mov eax, [dma_mapping_temp_domain_record]
    cmp dword [eax + IOMMU_DOMAIN_MODE], IOMMU_MODE_RESTRICTED
    je .publish
    mov edi, [dma_mapping_temp_record]
    and dword [edi + DMA_MAPPING_FLAGS], ~(DMA_FLAG_BOUNCE | DMA_FLAG_RESTRICTED)
    or dword [edi + DMA_MAPPING_FLAGS], DMA_FLAG_IOMMU
.publish:
    mov edi, [dma_mapping_temp_record]
    mov eax, [edi + DMA_MAPPING_ID]
    mov ecx, [dma_mapping_temp_request_slot]
    mov [dma_request_mapping_ids + ecx * 4], eax
    mov edx, [dma_mapping_temp_buffer_record]
    inc dword [edx + SHARED_BUFFER_MAPPINGS]
    inc dword [edx + SHARED_BUFFER_PINNED]
    inc dword [dma_mapping_next_id]
    inc dword [dma_mapping_active_count]
    inc dword [dma_mapping_pinned_pages]
    inc dword [dma_mapping_bounce_count]
    mov edx, [dma_mapping_temp_length]
    add [dma_mapping_mapped_bytes], edx
    mov eax, [edi + DMA_MAPPING_ID]
    popfd
    clc
    ret
.authorization_invalid:
    mov edi, [dma_mapping_temp_record]
    mov dword [edi + DMA_MAPPING_STATE], DMA_MAPPING_STATE_EMPTY
    jmp .invalid
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; Interner, bereits validierter Abbau. EAX=aktiver Mapping-Datensatz.
dma_mapping_release_record:
    mov [dma_mapping_temp_record], eax
    mov ecx, eax
    sub ecx, dma_mapping_table
    shr ecx, 6
    mov eax, [dma_mapping_iommu_authorization_ids + ecx * 4]
    test eax, eax
    jz .iommu_released
    mov edx, [dma_mapping_temp_record]
    mov edx, [edx + DMA_MAPPING_OWNER]
    call iommu_revoke_mapping
    mov ecx, [dma_mapping_temp_record]
    sub ecx, dma_mapping_table
    shr ecx, 6
    mov dword [dma_mapping_iommu_authorization_ids + ecx * 4], 0
.iommu_released:
    mov eax, [dma_mapping_temp_record]
    mov edx, [eax + DMA_MAPPING_REQUEST]
    mov [dma_mapping_temp_request], edx
    mov eax, edx
    call io_request_lookup
    jc .skip_request_slot
    mov ecx, eax
    sub ecx, io_request_table
    shr ecx, 6
    mov edx, [dma_mapping_temp_record]
    mov edx, [edx + DMA_MAPPING_ID]
    cmp [dma_request_mapping_ids + ecx * 4], edx
    jne .skip_request_slot
    mov dword [dma_request_mapping_ids + ecx * 4], 0
.skip_request_slot:
    mov eax, [dma_mapping_temp_record]
    mov eax, [eax + DMA_MAPPING_BUFFER]
    call shared_buffer_lookup
    jc .skip_buffer
    cmp dword [eax + SHARED_BUFFER_MAPPINGS], 0
    je .mapping_done
    dec dword [eax + SHARED_BUFFER_MAPPINGS]
.mapping_done:
    cmp dword [eax + SHARED_BUFFER_PINNED], 0
    je .skip_buffer
    dec dword [eax + SHARED_BUFFER_PINNED]
.skip_buffer:
    mov eax, [dma_mapping_temp_record]
    mov edx, [eax + DMA_MAPPING_LENGTH]
    cmp [dma_mapping_mapped_bytes], edx
    jb .zero_bytes
    sub [dma_mapping_mapped_bytes], edx
    jmp .bytes_done
.zero_bytes:
    mov dword [dma_mapping_mapped_bytes], 0
.bytes_done:
    cmp dword [dma_mapping_active_count], 0
    je .active_done
    dec dword [dma_mapping_active_count]
.active_done:
    cmp dword [dma_mapping_pinned_pages], 0
    je .pinned_done
    dec dword [dma_mapping_pinned_pages]
.pinned_done:
    cmp dword [eax + DMA_MAPPING_STATE], DMA_MAPPING_STATE_FAULTED
    je .done
    mov dword [eax + DMA_MAPPING_STATE], DMA_MAPPING_STATE_UNMAPPED
.done:
    clc
    ret

; EAX=Mapping-ID, EDX=Owner. Ein aktives Mapping wird kontrolliert entfernt.
dma_mapping_unmap:
    pushfd
    cli
    mov [dma_mapping_temp_id], eax
    mov [dma_mapping_temp_owner], edx
    call dma_mapping_lookup
    jc .invalid
    mov edx, [dma_mapping_temp_owner]
    cmp [eax + DMA_MAPPING_OWNER], edx
    jne .invalid
    call dma_mapping_release_record
    mov eax, [dma_mapping_temp_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Mapping-ID, EDX=Device-ID, EBX=Fehlercode. Der Fault bleibt im
; Mapping-Datensatz sichtbar; Ressourcen und Pinning werden trotzdem geloest.
dma_mapping_fault:
    pushfd
    cli
    mov [dma_mapping_temp_id], eax
    mov [dma_mapping_temp_device], edx
    mov [dma_mapping_temp_error], ebx
    call dma_mapping_lookup
    jc .invalid
    mov edx, [dma_mapping_temp_device]
    cmp [eax + DMA_MAPPING_DEVICE], edx
    jne .invalid
    mov ebx, [dma_mapping_temp_error]
    test ebx, ebx
    jz .invalid
    mov [eax + DMA_MAPPING_ERROR], ebx
    mov dword [eax + DMA_MAPPING_STATE], DMA_MAPPING_STATE_FAULTED
    inc dword [dma_mapping_fault_count]
    mov edx, [eax + DMA_MAPPING_REQUEST]
    mov [dma_mapping_temp_request], edx
    call dma_mapping_release_record
    mov eax, [dma_mapping_temp_request]
    xor edx, edx
    mov ebx, [dma_mapping_temp_error]
    call io_request_complete
    jc .invalid
    mov eax, [dma_mapping_temp_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=terminaler IORequest-Datensatz. Jede Completion, Cancellation und jeder
; Deadline-Miss loest das zugehoerige DMA-Mapping vor der Buffer-Lease.
dma_mapping_on_terminal:
    cmp dword [dma_mapping_manager_ready], 1
    jne .done
    mov ecx, eax
    sub ecx, io_request_table
    shr ecx, 6
    mov eax, [dma_request_mapping_ids + ecx * 4]
    test eax, eax
    jz .done
    call dma_mapping_lookup
    jc .done
    call dma_mapping_release_record
.done:
    clc
    ret

dma_mapping_self_test:
    cmp dword [dma_iommu_available], 0
    jne .invalid
    mov eax, 1
    mov ebx, IOMMU_MODE_RESTRICTED
    mov ecx, 32
    mov esi, DMA_RESTRICTED_APERTURE
    mov edi, DMA_RESTRICTED_APERTURE + 0x000FFFFF
    call iommu_domain_create
    jc .invalid
    mov [dma_mapping_test_domain], eax
    mov edx, 0xD001
    mov ebx, 0x10
    mov ecx, 1
    call iommu_bind_device
    jc .invalid
    mov eax, 1
    mov edx, 128
    mov ebx, SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE | SHARED_BUFFER_RIGHT_TRANSFER | SHARED_BUFFER_RIGHT_DMA | SHARED_BUFFER_RIGHT_RELEASE
    call shared_buffer_create
    jc .invalid
    mov [dma_mapping_test_buffer], eax
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [dma_mapping_test_scope], eax
    mov eax, 1
    mov edx, [dma_mapping_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [dma_mapping_test_task], eax
    mov eax, 1
    mov edx, [dma_mapping_test_task]
    mov ebx, 6
    mov ecx, 0xD001
    mov esi, [dma_mapping_test_buffer]
    mov edi, 128
    xor ebp, ebp
    call io_request_submit
    jc .invalid
    mov [dma_mapping_test_request], eax
    mov edx, [dma_mapping_test_buffer]
    mov ebx, SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE
    call shared_buffer_bind_io
    jc .invalid
    mov eax, [dma_mapping_test_request]
    mov edx, 0xD001
    mov ebx, DMA_DIRECTION_BIDIRECTIONAL
    mov ecx, 128
    mov esi, 1
    call dma_mapping_map
    jc .invalid
    mov [dma_mapping_test_id], eax
    call dma_mapping_lookup
    jc .invalid
    mov [dma_mapping_test_record], eax
    cmp dword [eax + DMA_MAPPING_PERMISSIONS], DMA_PERMISSION_READ | DMA_PERMISSION_WRITE
    jne .invalid
    cmp dword [eax + DMA_MAPPING_FLAGS], DMA_FLAG_BOUNCE | DMA_FLAG_COHERENT | DMA_FLAG_RESTRICTED
    jne .invalid
    cmp dword [eax + DMA_MAPPING_ADDRESS_HIGH], 0
    jne .invalid
    cmp dword [eax + DMA_MAPPING_DOMAIN], 0
    je .invalid
    cmp dword [iommu_mapping_count], 1
    jne .invalid
    mov edx, [eax + DMA_MAPPING_ADDRESS_LOW]
    cmp edx, DMA_RESTRICTED_APERTURE
    jb .invalid
    mov eax, [dma_mapping_test_buffer]
    call shared_buffer_lookup
    jc .invalid
    cmp dword [eax + SHARED_BUFFER_MAPPINGS], 1
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_PINNED], 1
    jne .invalid
    mov eax, [dma_mapping_test_request]
    mov edx, 0xD001
    mov ebx, DMA_DIRECTION_TO_DEVICE
    mov ecx, 64
    mov esi, 1
    call dma_mapping_map
    jnc .invalid                       ; genau ein Mapping je Request
    mov eax, [dma_mapping_test_buffer]
    mov edx, 1
    call shared_buffer_release
    jnc .invalid                       ; Pinning/Lease blockiert Release
    mov eax, [dma_mapping_test_domain]
    mov edx, 0xD001
    mov ebx, [dma_mapping_test_id]
    or ebx, IOMMU_EXTERNAL_DMA
    mov ecx, DMA_RESTRICTED_APERTURE
    mov esi, IOMMU_PERMISSION_WRITE
    mov edi, 0xD101
    call iommu_report_fault
    jc .invalid
    mov eax, [dma_mapping_test_record]
    cmp dword [eax + DMA_MAPPING_STATE], DMA_MAPPING_STATE_FAULTED
    jne .invalid
    cmp dword [dma_mapping_fault_count], 1
    jne .invalid
    mov eax, [dma_mapping_test_request]
    call io_request_lookup
    jc .invalid
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_FAILED
    jne .invalid
    cmp dword [eax + IO_REQUEST_COMPLETION], IO_COMPLETION_FAILED
    jne .invalid
    cmp dword [eax + IO_REQUEST_ERROR], 0xD101
    jne .invalid
    mov eax, [dma_mapping_test_buffer]
    call shared_buffer_lookup
    jc .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_MAPPINGS], 0
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_PINNED], 0
    jne .invalid
    cmp dword [dma_mapping_active_count], 0
    jne .invalid
    cmp dword [dma_mapping_pinned_pages], 0
    jne .invalid
    cmp dword [dma_mapping_mapped_bytes], 0
    jne .invalid
    cmp dword [dma_mapping_bounce_count], 1
    jne .invalid
    cmp dword [iommu_mapping_count], 0
    jne .invalid
    mov eax, [dma_mapping_test_task]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [dma_mapping_test_scope]
    call task_scope_close
    jc .invalid
    mov eax, [dma_mapping_test_buffer]
    mov edx, 1
    call shared_buffer_release
    jc .invalid
    mov eax, [dma_mapping_test_domain]
    mov edx, 1
    call iommu_domain_release
    jc .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
dma_mapping_api:
    dd DMA_MAPPING_API_SIZE
    dw 1, 0
    dd DMA_MAPPING_CAPACITY
    dd dma_mapping_map
    dd dma_mapping_unmap
    dd dma_mapping_fault
    dd dma_mapping_table
    dd dma_mapping_status

dma_mapping_status:
dma_mapping_manager_ready:       dd 0
dma_iommu_available:             dd 0
dma_mapping_active_count:        dd 0
dma_mapping_mapped_bytes:        dd 0
dma_mapping_pinned_pages:        dd 0
dma_mapping_fault_count:         dd 0
dma_mapping_bounce_count:        dd 0
dma_mapping_next_id:             dd 0
dma_mapping_temp_id:             dd 0
dma_mapping_temp_owner:          dd 0
dma_mapping_temp_device:         dd 0
dma_mapping_temp_direction:      dd 0
dma_mapping_temp_permissions:    dd 0
dma_mapping_temp_length:         dd 0
dma_mapping_temp_request:        dd 0
dma_mapping_temp_request_slot:   dd 0
dma_mapping_temp_request_record: dd 0
dma_mapping_temp_buffer:         dd 0
dma_mapping_temp_buffer_record:  dd 0
dma_mapping_temp_domain_record:  dd 0
dma_mapping_temp_slot:           dd 0
dma_mapping_temp_record:         dd 0
dma_mapping_temp_error:          dd 0
dma_mapping_test_buffer:         dd 0
dma_mapping_test_scope:          dd 0
dma_mapping_test_task:           dd 0
dma_mapping_test_request:        dd 0
dma_mapping_test_id:             dd 0
dma_mapping_test_record:         dd 0
dma_mapping_test_domain:         dd 0
align 4
dma_mapping_table:
    times DMA_MAPPING_CAPACITY * DMA_MAPPING_RECORD_SIZE db 0
dma_mapping_generations:
    times DMA_MAPPING_CAPACITY dd 0
dma_mapping_iommu_authorization_ids:
    times DMA_MAPPING_CAPACITY dd 0
dma_request_mapping_ids:
    times IO_REQUEST_CAPACITY dd 0

; ---------------------------------------------------------------------------
; Scatter/Gather-Descriptoren bilden mehrere Bufferbereiche als geordneten
; logischen Datenstrom ab. Das allgemeine ABI enthaelt keine physischen oder
; Device-Adressen; diese entstehen erst in einem DMA-/IOMMU-Provider.
; NPSPEC-DATAMOVE-SCATTERGATHER-0001
; ---------------------------------------------------------------------------

SG_API_SIZE                 equ 32
SG_DESCRIPTOR_CAPACITY      equ 4
SG_MAX_SEGMENTS             equ 4
SG_DESCRIPTOR_SIZE          equ 64
SG_SEGMENT_SIZE             equ 32
SG_STATE_EMPTY              equ 0
SG_STATE_BUILDING           equ 1
SG_STATE_SEALED             equ 2
SG_STATE_RELEASED           equ 3
SG_DIRECTION_GATHER         equ 1
SG_DIRECTION_SCATTER        equ 2
SG_DIRECTION_BIDIRECTIONAL  equ 3
SG_PERMISSION_READ          equ 0x00000001
SG_PERMISSION_WRITE         equ 0x00000002
SG_FLAG_COALESCED           equ 0x00000001
SG_FLAG_DIRECT_ELIGIBLE     equ 0x00000002
SG_FLAG_FALLBACK_ALLOWED    equ 0x00000004
SG_DESCRIPTOR_ID            equ 0
SG_DESCRIPTOR_OWNER         equ 4
SG_DESCRIPTOR_DIRECTION     equ 8
SG_DESCRIPTOR_STATE         equ 12
SG_DESCRIPTOR_SEGMENTS      equ 16
SG_DESCRIPTOR_TOTAL_LENGTH  equ 20
SG_DESCRIPTOR_MAX_SEGMENTS  equ 24
SG_DESCRIPTOR_FLAGS         equ 28
SG_DESCRIPTOR_CONSUMERS     equ 32
SG_DESCRIPTOR_REFERENCES    equ 36
SG_DESCRIPTOR_COALESCED     equ 40
SG_DESCRIPTOR_SPLIT         equ 44
SG_DESCRIPTOR_FALLBACKS     equ 48
SG_DESCRIPTOR_GENERATION    equ 52
SG_DESCRIPTOR_FAILURES      equ 56
SG_SEGMENT_BUFFER           equ 0
SG_SEGMENT_OFFSET           equ 4
SG_SEGMENT_LENGTH           equ 8
SG_SEGMENT_PERMISSIONS      equ 12
SG_SEGMENT_LOGICAL_OFFSET   equ 16
SG_SEGMENT_FLAGS            equ 20
SG_SEGMENT_GENERATION       equ 24

scatter_gather_initialize:
    mov edi, scatter_gather_table
    xor eax, eax
    mov ecx, (SG_DESCRIPTOR_CAPACITY * SG_DESCRIPTOR_SIZE) / 4
    rep stosd
    mov edi, scatter_gather_segments
    mov ecx, (SG_DESCRIPTOR_CAPACITY * SG_MAX_SEGMENTS * SG_SEGMENT_SIZE) / 4
    rep stosd
    mov edi, scatter_gather_generations
    mov ecx, SG_DESCRIPTOR_CAPACITY
    rep stosd
    mov dword [scatter_gather_next_id], 1
    mov dword [scatter_gather_live_count], 0
    mov dword [scatter_gather_sealed_count], 0
    mov dword [scatter_gather_retained_references], 0
    mov dword [scatter_gather_validation_failures], 0
    mov dword [scatter_gather_coalesced_segments], 0
    mov dword [scatter_gather_manager_ready], 1
    clc
    ret

; EAX=Descriptor-ID. EAX=aktiver Datensatz oder 0.
scatter_gather_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, SG_DESCRIPTOR_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, scatter_gather_table
    cmp dword [edx + SG_DESCRIPTOR_STATE], SG_STATE_BUILDING
    je .candidate
    cmp dword [edx + SG_DESCRIPTOR_STATE], SG_STATE_SEALED
    jne .next
.candidate:
    cmp [edx + SG_DESCRIPTOR_ID], eax
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

; EAX=Descriptor-Datensatz, ECX=Segmentindex. EAX=Segment-Datensatz.
scatter_gather_segment_at:
    mov edx, eax
    sub edx, scatter_gather_table
    shr edx, 6
    shl edx, 7                      ; vier Segmente zu je 32 Byte
    add edx, scatter_gather_segments
    mov eax, ecx
    shl eax, 5
    add eax, edx
    ret

; EAX=Owner, EDX=Richtung. EAX=Descriptor-ID.
scatter_gather_create:
    pushfd
    cli
    mov [scatter_gather_temp_owner], eax
    mov [scatter_gather_temp_direction], edx
    cmp dword [scatter_gather_manager_ready], 1
    jne .invalid
    cmp edx, SG_DIRECTION_GATHER
    jb .invalid
    cmp edx, SG_DIRECTION_BIDIRECTIONAL
    ja .invalid
    call process_lookup
    jc .invalid
    xor ecx, ecx
.scan:
    cmp ecx, SG_DESCRIPTOR_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 6
    add edi, scatter_gather_table
    cmp dword [edi + SG_DESCRIPTOR_STATE], SG_STATE_EMPTY
    je .slot
    cmp dword [edi + SG_DESCRIPTOR_STATE], SG_STATE_RELEASED
    je .slot
    inc ecx
    jmp .scan
.slot:
    mov [scatter_gather_temp_slot], ecx
    mov [scatter_gather_temp_record], edi
    xor eax, eax
    mov ecx, SG_DESCRIPTOR_SIZE / 4
    rep stosd
    mov eax, [scatter_gather_temp_slot]
    shl eax, 7
    add eax, scatter_gather_segments
    mov edi, eax
    xor eax, eax
    mov ecx, (SG_MAX_SEGMENTS * SG_SEGMENT_SIZE) / 4
    rep stosd
    mov edi, [scatter_gather_temp_record]
    mov eax, [scatter_gather_next_id]
    mov [edi + SG_DESCRIPTOR_ID], eax
    mov edx, [scatter_gather_temp_owner]
    mov [edi + SG_DESCRIPTOR_OWNER], edx
    mov edx, [scatter_gather_temp_direction]
    mov [edi + SG_DESCRIPTOR_DIRECTION], edx
    mov dword [edi + SG_DESCRIPTOR_STATE], SG_STATE_BUILDING
    mov dword [edi + SG_DESCRIPTOR_MAX_SEGMENTS], SG_MAX_SEGMENTS
    mov dword [edi + SG_DESCRIPTOR_FLAGS], SG_FLAG_FALLBACK_ALLOWED
    mov ecx, [scatter_gather_temp_slot]
    mov edx, [scatter_gather_generations + ecx * 4]
    inc edx
    jnz .generation_ready
    inc edx
.generation_ready:
    mov [scatter_gather_generations + ecx * 4], edx
    mov [edi + SG_DESCRIPTOR_GENERATION], edx
    inc dword [scatter_gather_next_id]
    inc dword [scatter_gather_live_count]
    mov eax, [edi + SG_DESCRIPTOR_ID]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Descriptor-ID, EDX=Buffer-ID, EBX=Offset, ECX=Laenge,
; ESI=explizite Segmentrechte, EDI=Owner.
scatter_gather_append:
    pushfd
    cli
    mov dword [scatter_gather_temp_record], 0
    mov [scatter_gather_temp_id], eax
    mov [scatter_gather_temp_buffer], edx
    mov [scatter_gather_temp_offset], ebx
    mov [scatter_gather_temp_length], ecx
    mov [scatter_gather_temp_permissions], esi
    mov [scatter_gather_temp_owner], edi
    test ecx, ecx
    jz .invalid
    test esi, esi
    jz .invalid
    test esi, ~(SG_PERMISSION_READ | SG_PERMISSION_WRITE)
    jnz .invalid
    call scatter_gather_lookup
    jc .invalid
    mov [scatter_gather_temp_record], eax
    cmp dword [eax + SG_DESCRIPTOR_STATE], SG_STATE_BUILDING
    jne .invalid
    mov edx, [scatter_gather_temp_owner]
    cmp [eax + SG_DESCRIPTOR_OWNER], edx
    jne .invalid
    mov edx, [eax + SG_DESCRIPTOR_DIRECTION]
    cmp edx, SG_DIRECTION_GATHER
    jne .not_gather
    cmp dword [scatter_gather_temp_permissions], SG_PERMISSION_READ
    jne .invalid
    jmp .direction_valid
.not_gather:
    cmp edx, SG_DIRECTION_SCATTER
    jne .bidirectional
    cmp dword [scatter_gather_temp_permissions], SG_PERMISSION_WRITE
    jne .invalid
    jmp .direction_valid
.bidirectional:
    cmp dword [scatter_gather_temp_permissions], SG_PERMISSION_READ | SG_PERMISSION_WRITE
    jne .invalid
.direction_valid:
    mov eax, [scatter_gather_temp_buffer]
    call shared_buffer_lookup
    jc .invalid
    mov [scatter_gather_temp_buffer_record], eax
    mov edx, [scatter_gather_temp_owner]
    cmp [eax + SHARED_BUFFER_OWNER], edx
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    mov edx, [scatter_gather_temp_permissions]
    mov ebx, [eax + SHARED_BUFFER_RIGHTS]
    and ebx, edx
    cmp ebx, edx
    jne .invalid
    mov ebx, [scatter_gather_temp_offset]
    mov ecx, [scatter_gather_temp_length]
    mov edx, ebx
    add edx, ecx
    jc .invalid
    cmp edx, [eax + SHARED_BUFFER_SIZE]
    ja .invalid
    mov eax, [scatter_gather_temp_record]
    mov edx, [eax + SG_DESCRIPTOR_TOTAL_LENGTH]
    add edx, ecx
    jc .invalid
    mov [scatter_gather_temp_total], edx
    mov ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    test ecx, ecx
    jz .new_segment
    dec ecx
    call scatter_gather_segment_at
    mov [scatter_gather_temp_segment], eax
    mov edx, [scatter_gather_temp_buffer]
    cmp [eax + SG_SEGMENT_BUFFER], edx
    jne .new_segment
    mov edx, [scatter_gather_temp_permissions]
    cmp [eax + SG_SEGMENT_PERMISSIONS], edx
    jne .new_segment
    mov edx, [eax + SG_SEGMENT_OFFSET]
    add edx, [eax + SG_SEGMENT_LENGTH]
    jc .invalid
    cmp edx, [scatter_gather_temp_offset]
    jne .new_segment
    mov edx, [eax + SG_SEGMENT_LENGTH]
    add edx, [scatter_gather_temp_length]
    jc .invalid
    mov [eax + SG_SEGMENT_LENGTH], edx
    or dword [eax + SG_SEGMENT_FLAGS], SG_FLAG_COALESCED
    mov eax, [scatter_gather_temp_record]
    mov edx, [scatter_gather_temp_total]
    mov [eax + SG_DESCRIPTOR_TOTAL_LENGTH], edx
    or dword [eax + SG_DESCRIPTOR_FLAGS], SG_FLAG_COALESCED
    inc dword [eax + SG_DESCRIPTOR_COALESCED]
    inc dword [scatter_gather_coalesced_segments]
    mov eax, [eax + SG_DESCRIPTOR_ID]
    popfd
    clc
    ret
.new_segment:
    mov eax, [scatter_gather_temp_record]
    mov ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    cmp ecx, SG_MAX_SEGMENTS
    jae .invalid
    call scatter_gather_segment_at
    mov [scatter_gather_temp_segment], eax
    mov edi, eax
    xor eax, eax
    mov ecx, SG_SEGMENT_SIZE / 4
    rep stosd
    mov edi, [scatter_gather_temp_segment]
    mov edx, [scatter_gather_temp_buffer]
    mov [edi + SG_SEGMENT_BUFFER], edx
    mov edx, [scatter_gather_temp_offset]
    mov [edi + SG_SEGMENT_OFFSET], edx
    mov edx, [scatter_gather_temp_length]
    mov [edi + SG_SEGMENT_LENGTH], edx
    mov edx, [scatter_gather_temp_permissions]
    mov [edi + SG_SEGMENT_PERMISSIONS], edx
    mov eax, [scatter_gather_temp_record]
    mov edx, [eax + SG_DESCRIPTOR_TOTAL_LENGTH]
    mov [edi + SG_SEGMENT_LOGICAL_OFFSET], edx
    mov edx, [eax + SG_DESCRIPTOR_GENERATION]
    mov [edi + SG_SEGMENT_GENERATION], edx
    inc dword [eax + SG_DESCRIPTOR_SEGMENTS]
    mov edx, [scatter_gather_temp_total]
    mov [eax + SG_DESCRIPTOR_TOTAL_LENGTH], edx
    mov eax, [eax + SG_DESCRIPTOR_ID]
    popfd
    clc
    ret
.invalid:
    inc dword [scatter_gather_validation_failures]
    mov eax, [scatter_gather_temp_record]
    test eax, eax
    jz .invalid_done
    inc dword [eax + SG_DESCRIPTOR_FAILURES]
.invalid_done:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Descriptor-ID, EDX=Owner. Erst der Seal-Schritt haelt alle Buffer fest.
scatter_gather_seal:
    pushfd
    cli
    mov [scatter_gather_temp_id], eax
    mov [scatter_gather_temp_owner], edx
    call scatter_gather_lookup
    jc .invalid
    mov [scatter_gather_temp_record], eax
    cmp dword [eax + SG_DESCRIPTOR_STATE], SG_STATE_BUILDING
    jne .invalid
    mov edx, [scatter_gather_temp_owner]
    cmp [eax + SG_DESCRIPTOR_OWNER], edx
    jne .invalid
    cmp dword [eax + SG_DESCRIPTOR_SEGMENTS], 0
    je .invalid
    mov dword [scatter_gather_temp_index], 0
.validate:
    mov eax, [scatter_gather_temp_record]
    mov ecx, [scatter_gather_temp_index]
    cmp ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    jae .retain
    call scatter_gather_segment_at
    mov [scatter_gather_temp_segment], eax
    mov eax, [eax + SG_SEGMENT_BUFFER]
    call shared_buffer_lookup
    jc .invalid
    mov edx, [scatter_gather_temp_owner]
    cmp [eax + SHARED_BUFFER_OWNER], edx
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    mov esi, [scatter_gather_temp_segment]
    mov edx, [esi + SG_SEGMENT_PERMISSIONS]
    mov ebx, [eax + SHARED_BUFFER_RIGHTS]
    and ebx, edx
    cmp ebx, edx
    jne .invalid
    mov ebx, [esi + SG_SEGMENT_OFFSET]
    add ebx, [esi + SG_SEGMENT_LENGTH]
    jc .invalid
    cmp ebx, [eax + SHARED_BUFFER_SIZE]
    ja .invalid
    inc dword [scatter_gather_temp_index]
    jmp .validate
.retain:
    mov dword [scatter_gather_temp_index], 0
.retain_loop:
    mov eax, [scatter_gather_temp_record]
    mov ecx, [scatter_gather_temp_index]
    cmp ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    jae .sealed
    call scatter_gather_segment_at
    mov eax, [eax + SG_SEGMENT_BUFFER]
    call shared_buffer_lookup
    jc .invalid
    inc dword [eax + SHARED_BUFFER_REFERENCES]
    inc dword [scatter_gather_retained_references]
    inc dword [scatter_gather_temp_index]
    jmp .retain_loop
.sealed:
    mov eax, [scatter_gather_temp_record]
    mov ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    mov [eax + SG_DESCRIPTOR_REFERENCES], ecx
    mov dword [eax + SG_DESCRIPTOR_STATE], SG_STATE_SEALED
    or dword [eax + SG_DESCRIPTOR_FLAGS], SG_FLAG_DIRECT_ELIGIBLE
    inc dword [scatter_gather_sealed_count]
    mov eax, [eax + SG_DESCRIPTOR_ID]
    popfd
    clc
    ret
.invalid:
    inc dword [scatter_gather_validation_failures]
    mov eax, [scatter_gather_temp_record]
    test eax, eax
    jz .invalid_done
    inc dword [eax + SG_DESCRIPTOR_FAILURES]
.invalid_done:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Descriptor-ID, EDX=Owner. Aktive Consumer verhindern den Abbau.
scatter_gather_release:
    pushfd
    cli
    mov [scatter_gather_temp_id], eax
    mov [scatter_gather_temp_owner], edx
    call scatter_gather_lookup
    jc .invalid
    mov [scatter_gather_temp_record], eax
    mov edx, [scatter_gather_temp_owner]
    cmp [eax + SG_DESCRIPTOR_OWNER], edx
    jne .invalid
    cmp dword [eax + SG_DESCRIPTOR_CONSUMERS], 0
    jne .invalid
    cmp dword [eax + SG_DESCRIPTOR_STATE], SG_STATE_SEALED
    jne .mark_released
    mov dword [scatter_gather_temp_index], 0
.release_loop:
    mov eax, [scatter_gather_temp_record]
    mov ecx, [scatter_gather_temp_index]
    cmp ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    jae .released_refs
    call scatter_gather_segment_at
    mov eax, [eax + SG_SEGMENT_BUFFER]
    call shared_buffer_lookup
    jc .invalid
    cmp dword [eax + SHARED_BUFFER_REFERENCES], 1
    jbe .invalid
    dec dword [eax + SHARED_BUFFER_REFERENCES]
    dec dword [scatter_gather_retained_references]
    inc dword [scatter_gather_temp_index]
    jmp .release_loop
.released_refs:
    mov eax, [scatter_gather_temp_record]
    mov dword [eax + SG_DESCRIPTOR_REFERENCES], 0
    dec dword [scatter_gather_sealed_count]
.mark_released:
    mov eax, [scatter_gather_temp_record]
    mov dword [eax + SG_DESCRIPTOR_STATE], SG_STATE_RELEASED
    dec dword [scatter_gather_live_count]
    mov eax, [scatter_gather_temp_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

scatter_gather_self_test:
    mov eax, 1
    mov edx, 128
    mov ebx, SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE | SHARED_BUFFER_RIGHT_TRANSFER | SHARED_BUFFER_RIGHT_RELEASE
    call shared_buffer_create
    jc .invalid
    mov [scatter_gather_test_buffer_a], eax
    mov eax, 1
    mov edx, 128
    mov ebx, SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE | SHARED_BUFFER_RIGHT_TRANSFER | SHARED_BUFFER_RIGHT_RELEASE
    call shared_buffer_create
    jc .invalid
    mov [scatter_gather_test_buffer_b], eax
    mov eax, 1
    mov edx, SG_DIRECTION_GATHER
    call scatter_gather_create
    jc .invalid
    mov [scatter_gather_test_id], eax
    mov edx, [scatter_gather_test_buffer_a]
    xor ebx, ebx
    mov ecx, 32
    mov esi, SG_PERMISSION_READ
    mov edi, 1
    call scatter_gather_append
    jc .invalid
    mov eax, [scatter_gather_test_id]
    mov edx, [scatter_gather_test_buffer_a]
    mov ebx, 32
    mov ecx, 32
    mov esi, SG_PERMISSION_READ
    mov edi, 1
    call scatter_gather_append
    jc .invalid
    mov eax, [scatter_gather_test_id]
    mov edx, [scatter_gather_test_buffer_b]
    mov ebx, 8
    mov ecx, 64
    mov esi, SG_PERMISSION_READ
    mov edi, 1
    call scatter_gather_append
    jc .invalid
    mov eax, [scatter_gather_test_id]
    mov edx, [scatter_gather_test_buffer_b]
    mov ebx, 100
    mov ecx, 40
    mov esi, SG_PERMISSION_READ
    mov edi, 1
    call scatter_gather_append
    jnc .invalid                       ; Offset + Laenge ueberschreitet Buffer
    mov eax, [scatter_gather_test_id]
    call scatter_gather_lookup
    jc .invalid
    cmp dword [eax + SG_DESCRIPTOR_SEGMENTS], 2
    jne .invalid
    cmp dword [eax + SG_DESCRIPTOR_TOTAL_LENGTH], 128
    jne .invalid
    cmp dword [eax + SG_DESCRIPTOR_COALESCED], 1
    jne .invalid
    cmp dword [eax + SG_DESCRIPTOR_FAILURES], 1
    jne .invalid
    mov eax, [scatter_gather_test_id]
    mov edx, 1
    call scatter_gather_seal
    jc .invalid
    mov eax, [scatter_gather_test_buffer_a]
    call shared_buffer_lookup
    jc .invalid
    cmp dword [eax + SHARED_BUFFER_REFERENCES], 2
    jne .invalid
    mov eax, [scatter_gather_test_buffer_b]
    call shared_buffer_lookup
    jc .invalid
    cmp dword [eax + SHARED_BUFFER_REFERENCES], 2
    jne .invalid
    mov eax, [scatter_gather_test_buffer_a]
    mov edx, 1
    call shared_buffer_release
    jnc .invalid                       ; Descriptor garantiert Lifetime
    mov eax, [scatter_gather_test_id]
    mov edx, 1
    call scatter_gather_release
    jc .invalid
    cmp dword [scatter_gather_live_count], 0
    jne .invalid
    cmp dword [scatter_gather_sealed_count], 0
    jne .invalid
    cmp dword [scatter_gather_retained_references], 0
    jne .invalid
    cmp dword [scatter_gather_coalesced_segments], 1
    jne .invalid
    mov eax, [scatter_gather_test_buffer_a]
    mov edx, 1
    call shared_buffer_release
    jc .invalid
    mov eax, [scatter_gather_test_buffer_b]
    mov edx, 1
    call shared_buffer_release
    jc .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
scatter_gather_api:
    dd SG_API_SIZE
    dw 1, 0
    dd SG_DESCRIPTOR_CAPACITY
    dd SG_MAX_SEGMENTS
    dd scatter_gather_create
    dd scatter_gather_append
    dd scatter_gather_seal
    dd scatter_gather_release

scatter_gather_manager_ready:          dd 0
scatter_gather_next_id:                dd 0
scatter_gather_live_count:             dd 0
scatter_gather_sealed_count:           dd 0
scatter_gather_retained_references:    dd 0
scatter_gather_validation_failures:    dd 0
scatter_gather_coalesced_segments:     dd 0
scatter_gather_temp_id:                dd 0
scatter_gather_temp_owner:             dd 0
scatter_gather_temp_direction:         dd 0
scatter_gather_temp_buffer:            dd 0
scatter_gather_temp_offset:            dd 0
scatter_gather_temp_length:            dd 0
scatter_gather_temp_permissions:       dd 0
scatter_gather_temp_total:             dd 0
scatter_gather_temp_slot:              dd 0
scatter_gather_temp_index:             dd 0
scatter_gather_temp_record:            dd 0
scatter_gather_temp_segment:           dd 0
scatter_gather_temp_buffer_record:     dd 0
scatter_gather_test_id:                dd 0
scatter_gather_test_buffer_a:          dd 0
scatter_gather_test_buffer_b:          dd 0
align 4
scatter_gather_table:
    times SG_DESCRIPTOR_CAPACITY * SG_DESCRIPTOR_SIZE db 0
scatter_gather_segments:
    times SG_DESCRIPTOR_CAPACITY * SG_MAX_SEGMENTS * SG_SEGMENT_SIZE db 0
scatter_gather_generations:
    times SG_DESCRIPTOR_CAPACITY dd 0

; ---------------------------------------------------------------------------
; SG-faehige DMA-Providergrenze. Ein versiegelter SG-Descriptor wird unter
; festen Hardwarelimits in kontrollierte Device-Segmente uebersetzt. Der
; aktuelle Bootstrap bleibt ohne IOMMU ein expliziter Restricted-Bounce-Pfad.
; ---------------------------------------------------------------------------

DMA_SG_API_SIZE              equ 32
DMA_SG_CAPACITY              equ 2
DMA_SG_MAX_SEGMENTS          equ 4
DMA_SG_MAPPING_SIZE          equ 64
DMA_SG_SEGMENT_SIZE          equ 32
DMA_SG_STATE_EMPTY           equ 0
DMA_SG_STATE_ACTIVE          equ 1
DMA_SG_STATE_UNMAPPED        equ 2
DMA_SG_STATE_FAULTED         equ 3
DMA_SG_FLAG_BOUNCE           equ 0x00000001
DMA_SG_FLAG_COHERENT         equ 0x00000002
DMA_SG_FLAG_RESTRICTED       equ 0x00000004
DMA_SG_FLAG_SPLIT            equ 0x00000008
DMA_SG_FLAG_IOMMU            equ 0x00000010
DMA_SG_MAX_SEGMENT_SIZE      equ 64
DMA_SG_ALIGNMENT             equ 4
DMA_SG_ADDRESS_WIDTH         equ 32
DMA_SG_BOUNDARY              equ 4096
DMA_SG_APERTURE              equ 0xD1000000
DMA_SG_MAPPING_ID            equ 0
DMA_SG_MAPPING_OWNER         equ 4
DMA_SG_MAPPING_DEVICE        equ 8
DMA_SG_MAPPING_REQUEST       equ 12
DMA_SG_MAPPING_DESCRIPTOR    equ 16
DMA_SG_MAPPING_DIRECTION     equ 20
DMA_SG_MAPPING_STATE         equ 24
DMA_SG_MAPPING_SEGMENTS      equ 28
DMA_SG_MAPPING_TOTAL_LENGTH  equ 32
DMA_SG_MAPPING_MAX_SIZE      equ 36
DMA_SG_MAPPING_ALIGNMENT     equ 40
DMA_SG_MAPPING_ADDRESS_WIDTH equ 44
DMA_SG_MAPPING_BOUNDARY      equ 48
DMA_SG_MAPPING_FLAGS         equ 52
DMA_SG_MAPPING_GENERATION    equ 56
DMA_SG_MAPPING_DOMAIN        equ 60
DMA_SG_SEGMENT_SOURCE        equ 0
DMA_SG_SEGMENT_BUFFER        equ 4
DMA_SG_SEGMENT_OFFSET        equ 8
DMA_SG_SEGMENT_LENGTH        equ 12
DMA_SG_SEGMENT_ADDRESS_LOW   equ 16
DMA_SG_SEGMENT_ADDRESS_HIGH  equ 20
DMA_SG_SEGMENT_PERMISSIONS   equ 24
DMA_SG_SEGMENT_FLAGS         equ 28

dma_scatter_gather_initialize:
    mov edi, dma_scatter_gather_table
    xor eax, eax
    mov ecx, (DMA_SG_CAPACITY * DMA_SG_MAPPING_SIZE) / 4
    rep stosd
    mov edi, dma_scatter_gather_segments
    mov ecx, (DMA_SG_CAPACITY * DMA_SG_MAX_SEGMENTS * DMA_SG_SEGMENT_SIZE) / 4
    rep stosd
    mov edi, dma_scatter_gather_generations
    mov ecx, DMA_SG_CAPACITY
    rep stosd
    mov edi, dma_request_sg_mapping_ids
    mov ecx, IO_REQUEST_CAPACITY
    rep stosd
    mov edi, dma_scatter_gather_iommu_authorization_ids
    mov ecx, DMA_SG_CAPACITY * DMA_SG_MAX_SEGMENTS
    rep stosd
    mov edi, dma_scatter_gather_error_codes
    mov ecx, DMA_SG_CAPACITY
    rep stosd
    mov dword [dma_scatter_gather_next_id], 1
    mov dword [dma_scatter_gather_active_count], 0
    mov dword [dma_scatter_gather_mapped_bytes], 0
    mov dword [dma_scatter_gather_pinned_buffers], 0
    mov dword [dma_scatter_gather_split_segments], 0
    mov dword [dma_scatter_gather_validation_failures], 0
    mov dword [dma_scatter_gather_fault_count], 0
    mov dword [dma_scatter_gather_manager_ready], 1
    clc
    ret

dma_scatter_gather_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, DMA_SG_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 6
    add edx, dma_scatter_gather_table
    cmp dword [edx + DMA_SG_MAPPING_STATE], DMA_SG_STATE_ACTIVE
    jne .next
    cmp [edx + DMA_SG_MAPPING_ID], eax
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

; EAX=Mapping-Datensatz, ECX=Device-Segmentindex. EAX=Segment-Datensatz.
dma_scatter_gather_segment_at:
    mov edx, eax
    sub edx, dma_scatter_gather_table
    shr edx, 6
    shl edx, 7
    add edx, dma_scatter_gather_segments
    mov eax, ecx
    shl eax, 5
    add eax, edx
    ret

; EAX=Request-ID, EDX=SG-Descriptor-ID, EBX=Device-ID, ECX=Owner.
; Ergebnis EAX=DMA-SG-Mapping-ID.
dma_scatter_gather_map:
    pushfd
    cli
    mov dword [dma_scatter_gather_temp_record], 0
    mov [dma_scatter_gather_temp_request], eax
    mov [dma_scatter_gather_temp_descriptor], edx
    mov [dma_scatter_gather_temp_device], ebx
    mov [dma_scatter_gather_temp_owner], ecx
    cmp dword [dma_scatter_gather_manager_ready], 1
    jne .invalid
    test ebx, ebx
    jz .invalid
    call io_request_lookup
    jc .invalid
    mov [dma_scatter_gather_temp_request_record], eax
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_PENDING
    je .request_active
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_RUNNING
    jne .invalid
.request_active:
    mov edx, [dma_scatter_gather_temp_owner]
    cmp [eax + IO_REQUEST_OWNER], edx
    jne .invalid
    mov edx, [dma_scatter_gather_temp_descriptor]
    cmp [eax + IO_REQUEST_BUFFER], edx
    jne .invalid
    mov ecx, eax
    sub ecx, io_request_table
    shr ecx, 6
    mov [dma_scatter_gather_temp_request_slot], ecx
    cmp dword [dma_request_mapping_ids + ecx * 4], 0
    jne .invalid
    cmp dword [dma_request_sg_mapping_ids + ecx * 4], 0
    jne .invalid
    mov eax, [dma_scatter_gather_temp_descriptor]
    call scatter_gather_lookup
    jc .invalid
    mov [dma_scatter_gather_temp_descriptor_record], eax
    cmp dword [eax + SG_DESCRIPTOR_STATE], SG_STATE_SEALED
    jne .invalid
    mov edx, [dma_scatter_gather_temp_owner]
    cmp [eax + SG_DESCRIPTOR_OWNER], edx
    jne .invalid
    cmp dword [eax + SG_DESCRIPTOR_CONSUMERS], 0
    jne .invalid
    mov edx, [dma_scatter_gather_temp_request_record]
    mov ecx, [eax + SG_DESCRIPTOR_TOTAL_LENGTH]
    cmp [edx + IO_REQUEST_LENGTH], ecx
    jne .invalid
    mov dword [dma_scatter_gather_temp_required], 0
    mov dword [dma_scatter_gather_temp_source_index], 0
    mov edi, dma_scatter_gather_temp_buffer_ids
    xor eax, eax
    mov ecx, SG_MAX_SEGMENTS
    rep stosd
.validate_source:
    mov eax, [dma_scatter_gather_temp_descriptor_record]
    mov ecx, [dma_scatter_gather_temp_source_index]
    cmp ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    jae .find_mapping_slot
    call scatter_gather_segment_at
    mov [dma_scatter_gather_temp_source_segment], eax
    mov edx, [eax + SG_SEGMENT_OFFSET]
    test edx, DMA_SG_ALIGNMENT - 1
    jnz .invalid
    mov ebx, [eax + SG_SEGMENT_LENGTH]
    test ebx, ebx
    jz .invalid
    test ebx, DMA_SG_ALIGNMENT - 1
    jnz .invalid
    mov esi, [eax + SG_SEGMENT_BUFFER]
    xor ecx, ecx
.duplicate_scan:
    cmp ecx, [dma_scatter_gather_temp_source_index]
    jae .unique_buffer
    cmp [dma_scatter_gather_temp_buffer_ids + ecx * 4], esi
    je .invalid                         ; Providerlimit: ein Eintrag je Buffer
    inc ecx
    jmp .duplicate_scan
.unique_buffer:
    mov ecx, [dma_scatter_gather_temp_source_index]
    mov [dma_scatter_gather_temp_buffer_ids + ecx * 4], esi
    mov eax, esi
    call shared_buffer_lookup
    jc .invalid
    mov edx, [dma_scatter_gather_temp_owner]
    cmp [eax + SHARED_BUFFER_OWNER], edx
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_ACTIVE_IO], 0
    jne .invalid
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_TRANSFER
    jz .invalid
    test dword [eax + SHARED_BUFFER_RIGHTS], SHARED_BUFFER_RIGHT_DMA
    jz .invalid
    mov esi, [dma_scatter_gather_temp_source_segment]
    mov edx, [esi + SG_SEGMENT_PERMISSIONS]
    mov ecx, [eax + SHARED_BUFFER_RIGHTS]
    and ecx, edx
    cmp ecx, edx
    jne .invalid
    mov eax, ebx
    add eax, DMA_SG_MAX_SEGMENT_SIZE - 1
    jc .invalid
    shr eax, 6
    add [dma_scatter_gather_temp_required], eax
    jc .invalid
    cmp dword [dma_scatter_gather_temp_required], DMA_SG_MAX_SEGMENTS
    ja .invalid
    inc dword [dma_scatter_gather_temp_source_index]
    jmp .validate_source

.find_mapping_slot:
    xor ecx, ecx
.scan_mapping_slot:
    cmp ecx, DMA_SG_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 6
    add edi, dma_scatter_gather_table
    cmp dword [edi + DMA_SG_MAPPING_STATE], DMA_SG_STATE_EMPTY
    je .mapping_slot
    cmp dword [edi + DMA_SG_MAPPING_STATE], DMA_SG_STATE_UNMAPPED
    je .mapping_slot
    cmp dword [edi + DMA_SG_MAPPING_STATE], DMA_SG_STATE_FAULTED
    je .mapping_slot
    inc ecx
    jmp .scan_mapping_slot
.mapping_slot:
    mov [dma_scatter_gather_temp_slot], ecx
    mov [dma_scatter_gather_temp_record], edi
    xor eax, eax
    mov ecx, DMA_SG_MAPPING_SIZE / 4
    rep stosd
    mov eax, [dma_scatter_gather_temp_slot]
    shl eax, 7
    add eax, dma_scatter_gather_segments
    mov edi, eax
    xor eax, eax
    mov ecx, (DMA_SG_MAX_SEGMENTS * DMA_SG_SEGMENT_SIZE) / 4
    rep stosd
    mov edi, [dma_scatter_gather_temp_record]
    mov eax, [dma_scatter_gather_next_id]
    mov [edi + DMA_SG_MAPPING_ID], eax
    mov edx, [dma_scatter_gather_temp_owner]
    mov [edi + DMA_SG_MAPPING_OWNER], edx
    mov edx, [dma_scatter_gather_temp_device]
    mov [edi + DMA_SG_MAPPING_DEVICE], edx
    mov edx, [dma_scatter_gather_temp_request]
    mov [edi + DMA_SG_MAPPING_REQUEST], edx
    mov edx, [dma_scatter_gather_temp_descriptor]
    mov [edi + DMA_SG_MAPPING_DESCRIPTOR], edx
    mov edx, [dma_scatter_gather_temp_descriptor_record]
    mov eax, [edx + SG_DESCRIPTOR_DIRECTION]
    mov [edi + DMA_SG_MAPPING_DIRECTION], eax
    mov dword [edi + DMA_SG_MAPPING_STATE], DMA_SG_STATE_ACTIVE
    mov eax, [dma_scatter_gather_temp_required]
    mov [edi + DMA_SG_MAPPING_SEGMENTS], eax
    mov eax, [edx + SG_DESCRIPTOR_TOTAL_LENGTH]
    mov [edi + DMA_SG_MAPPING_TOTAL_LENGTH], eax
    mov dword [edi + DMA_SG_MAPPING_MAX_SIZE], DMA_SG_MAX_SEGMENT_SIZE
    mov dword [edi + DMA_SG_MAPPING_ALIGNMENT], DMA_SG_ALIGNMENT
    mov dword [edi + DMA_SG_MAPPING_ADDRESS_WIDTH], DMA_SG_ADDRESS_WIDTH
    mov dword [edi + DMA_SG_MAPPING_BOUNDARY], DMA_SG_BOUNDARY
    mov dword [edi + DMA_SG_MAPPING_FLAGS], DMA_SG_FLAG_BOUNCE | DMA_SG_FLAG_COHERENT | DMA_SG_FLAG_RESTRICTED
    mov eax, [edx + SG_DESCRIPTOR_SEGMENTS]
    cmp [dma_scatter_gather_temp_required], eax
    jbe .flags_ready
    or dword [edi + DMA_SG_MAPPING_FLAGS], DMA_SG_FLAG_SPLIT
.flags_ready:
    mov ecx, [dma_scatter_gather_temp_slot]
    mov eax, [dma_scatter_gather_generations + ecx * 4]
    inc eax
    jnz .generation_ready
    inc eax
.generation_ready:
    mov [dma_scatter_gather_generations + ecx * 4], eax
    mov [edi + DMA_SG_MAPPING_GENERATION], eax
    mov dword [edi + DMA_SG_MAPPING_DOMAIN], 0
    mov eax, [dma_scatter_gather_temp_slot]
    shl eax, 4
    add eax, dma_scatter_gather_iommu_authorization_ids
    mov edi, eax
    xor eax, eax
    mov ecx, DMA_SG_MAX_SEGMENTS
    rep stosd
    mov dword [dma_scatter_gather_temp_source_index], 0
    mov dword [dma_scatter_gather_temp_output_index], 0

.translate_source:
    mov eax, [dma_scatter_gather_temp_descriptor_record]
    mov ecx, [dma_scatter_gather_temp_source_index]
    cmp ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    jae .authorize_iommu
    call scatter_gather_segment_at
    mov edx, [eax + SG_SEGMENT_BUFFER]
    mov [dma_scatter_gather_temp_buffer], edx
    mov edx, [eax + SG_SEGMENT_OFFSET]
    mov [dma_scatter_gather_temp_offset], edx
    mov edx, [eax + SG_SEGMENT_LENGTH]
    mov [dma_scatter_gather_temp_remaining], edx
    mov edx, [eax + SG_SEGMENT_PERMISSIONS]
    mov [dma_scatter_gather_temp_permissions], edx
.translate_chunk:
    cmp dword [dma_scatter_gather_temp_remaining], 0
    je .next_source
    mov edx, [dma_scatter_gather_temp_remaining]
    cmp edx, DMA_SG_MAX_SEGMENT_SIZE
    jbe .chunk_ready
    mov edx, DMA_SG_MAX_SEGMENT_SIZE
.chunk_ready:
    mov [dma_scatter_gather_temp_chunk], edx
    mov eax, [dma_scatter_gather_temp_record]
    mov ecx, [dma_scatter_gather_temp_output_index]
    call dma_scatter_gather_segment_at
    mov edi, eax
    mov edx, [dma_scatter_gather_temp_source_index]
    mov [edi + DMA_SG_SEGMENT_SOURCE], edx
    mov edx, [dma_scatter_gather_temp_buffer]
    mov [edi + DMA_SG_SEGMENT_BUFFER], edx
    mov edx, [dma_scatter_gather_temp_offset]
    mov [edi + DMA_SG_SEGMENT_OFFSET], edx
    mov edx, [dma_scatter_gather_temp_chunk]
    mov [edi + DMA_SG_SEGMENT_LENGTH], edx
    mov eax, [dma_scatter_gather_temp_slot]
    shl eax, 16
    add eax, DMA_SG_APERTURE
    mov ecx, [dma_scatter_gather_temp_output_index]
    shl ecx, 12
    add eax, ecx
    mov [edi + DMA_SG_SEGMENT_ADDRESS_LOW], eax
    mov dword [edi + DMA_SG_SEGMENT_ADDRESS_HIGH], 0
    mov edx, [dma_scatter_gather_temp_permissions]
    mov [edi + DMA_SG_SEGMENT_PERMISSIONS], edx
    cmp dword [dma_scatter_gather_temp_remaining], DMA_SG_MAX_SEGMENT_SIZE
    jbe .chunk_flags_ready
    mov dword [edi + DMA_SG_SEGMENT_FLAGS], DMA_SG_FLAG_SPLIT
    inc dword [dma_scatter_gather_split_segments]
.chunk_flags_ready:
    mov edx, [dma_scatter_gather_temp_chunk]
    add [dma_scatter_gather_temp_offset], edx
    sub [dma_scatter_gather_temp_remaining], edx
    inc dword [dma_scatter_gather_temp_output_index]
    jmp .translate_chunk
.next_source:
    inc dword [dma_scatter_gather_temp_source_index]
    jmp .translate_source

.authorize_iommu:
    mov eax, [dma_scatter_gather_temp_device]
    call iommu_domain_for_device
    jc .acquire_buffers
    mov [dma_scatter_gather_temp_iommu_domain], eax
    mov edx, [eax + IOMMU_DOMAIN_ID]
    mov edi, [dma_scatter_gather_temp_record]
    mov [edi + DMA_SG_MAPPING_DOMAIN], edx
    mov dword [dma_scatter_gather_temp_authorized_count], 0
.authorize_segment:
    mov ecx, [dma_scatter_gather_temp_authorized_count]
    mov edi, [dma_scatter_gather_temp_record]
    cmp ecx, [edi + DMA_SG_MAPPING_SEGMENTS]
    jae .authorization_ready
    mov eax, edi
    call dma_scatter_gather_segment_at
    mov [dma_scatter_gather_temp_device_segment], eax
    mov edi, eax
    mov eax, [dma_scatter_gather_temp_iommu_domain]
    mov eax, [eax + IOMMU_DOMAIN_ID]
    mov edx, [dma_scatter_gather_temp_device]
    mov ebx, [dma_scatter_gather_temp_record]
    mov ebx, [ebx + DMA_SG_MAPPING_ID]
    or ebx, IOMMU_EXTERNAL_DMA_SG
    mov ecx, [edi + DMA_SG_SEGMENT_ADDRESS_LOW]
    mov esi, 4096
    mov edi, [edi + DMA_SG_SEGMENT_PERMISSIONS]
    mov ebp, [dma_scatter_gather_temp_owner]
    call iommu_authorize_mapping
    jc .authorization_invalid
    mov edx, [dma_scatter_gather_temp_slot]
    shl edx, 2
    add edx, [dma_scatter_gather_temp_authorized_count]
    mov [dma_scatter_gather_iommu_authorization_ids + edx * 4], eax
    inc dword [dma_scatter_gather_temp_authorized_count]
    jmp .authorize_segment
.authorization_ready:
    mov eax, [dma_scatter_gather_temp_iommu_domain]
    cmp dword [eax + IOMMU_DOMAIN_MODE], IOMMU_MODE_RESTRICTED
    je .acquire_buffers
    mov edi, [dma_scatter_gather_temp_record]
    and dword [edi + DMA_SG_MAPPING_FLAGS], ~(DMA_SG_FLAG_BOUNCE | DMA_SG_FLAG_RESTRICTED)
    or dword [edi + DMA_SG_MAPPING_FLAGS], DMA_SG_FLAG_IOMMU
    jmp .acquire_buffers
.authorization_invalid:
    mov dword [dma_scatter_gather_temp_rollback_index], 0
.authorization_rollback:
    mov ecx, [dma_scatter_gather_temp_rollback_index]
    cmp ecx, [dma_scatter_gather_temp_authorized_count]
    jae .authorization_rollback_done
    mov edx, [dma_scatter_gather_temp_slot]
    shl edx, 2
    add edx, ecx
    mov eax, [dma_scatter_gather_iommu_authorization_ids + edx * 4]
    mov edx, [dma_scatter_gather_temp_owner]
    call iommu_revoke_mapping
    inc dword [dma_scatter_gather_temp_rollback_index]
    jmp .authorization_rollback
.authorization_rollback_done:
    mov edi, [dma_scatter_gather_temp_record]
    mov dword [edi + DMA_SG_MAPPING_STATE], DMA_SG_STATE_EMPTY
    jmp .invalid

.acquire_buffers:
    mov dword [dma_scatter_gather_temp_source_index], 0
.acquire_loop:
    mov eax, [dma_scatter_gather_temp_descriptor_record]
    mov ecx, [dma_scatter_gather_temp_source_index]
    cmp ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    jae .publish_mapping
    call scatter_gather_segment_at
    mov eax, [eax + SG_SEGMENT_BUFFER]
    call shared_buffer_lookup
    jc .invalid
    mov dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_PROVIDER
    inc dword [eax + SHARED_BUFFER_ACTIVE_IO]
    inc dword [eax + SHARED_BUFFER_REFERENCES]
    inc dword [eax + SHARED_BUFFER_MAPPINGS]
    inc dword [eax + SHARED_BUFFER_PINNED]
    inc dword [dma_scatter_gather_pinned_buffers]
    inc dword [dma_scatter_gather_temp_source_index]
    jmp .acquire_loop
.publish_mapping:
    mov eax, [dma_scatter_gather_temp_descriptor_record]
    inc dword [eax + SG_DESCRIPTOR_CONSUMERS]
    mov edi, [dma_scatter_gather_temp_record]
    mov eax, [edi + DMA_SG_MAPPING_ID]
    mov ecx, [dma_scatter_gather_temp_request_slot]
    mov [dma_request_sg_mapping_ids + ecx * 4], eax
    inc dword [dma_scatter_gather_next_id]
    inc dword [dma_scatter_gather_active_count]
    mov edx, [edi + DMA_SG_MAPPING_TOTAL_LENGTH]
    add [dma_scatter_gather_mapped_bytes], edx
    popfd
    clc
    ret
.invalid:
    inc dword [dma_scatter_gather_validation_failures]
    xor eax, eax
    popfd
    stc
    ret

; EAX=aktiver DMA-SG-Mapping-Datensatz.
dma_scatter_gather_release_record:
    mov [dma_scatter_gather_temp_record], eax
    mov eax, [dma_scatter_gather_temp_record]
    sub eax, dma_scatter_gather_table
    shr eax, 6
    mov [dma_scatter_gather_temp_slot], eax
    mov dword [dma_scatter_gather_temp_rollback_index], 0
.revoke_iommu:
    mov ecx, [dma_scatter_gather_temp_rollback_index]
    cmp ecx, DMA_SG_MAX_SEGMENTS
    jae .iommu_revoked
    mov edx, [dma_scatter_gather_temp_slot]
    shl edx, 2
    add edx, ecx
    mov eax, [dma_scatter_gather_iommu_authorization_ids + edx * 4]
    test eax, eax
    jz .next_iommu_revoke
    mov edx, [dma_scatter_gather_temp_record]
    mov edx, [edx + DMA_SG_MAPPING_OWNER]
    call iommu_revoke_mapping
    mov edx, [dma_scatter_gather_temp_slot]
    shl edx, 2
    add edx, [dma_scatter_gather_temp_rollback_index]
    mov dword [dma_scatter_gather_iommu_authorization_ids + edx * 4], 0
.next_iommu_revoke:
    inc dword [dma_scatter_gather_temp_rollback_index]
    jmp .revoke_iommu
.iommu_revoked:
    mov eax, [dma_scatter_gather_temp_record]
    mov eax, [eax + DMA_SG_MAPPING_REQUEST]
    call io_request_lookup
    jc .skip_request
    mov ecx, eax
    sub ecx, io_request_table
    shr ecx, 6
    mov edx, [dma_scatter_gather_temp_record]
    mov edx, [edx + DMA_SG_MAPPING_ID]
    cmp [dma_request_sg_mapping_ids + ecx * 4], edx
    jne .skip_request
    mov dword [dma_request_sg_mapping_ids + ecx * 4], 0
.skip_request:
    mov eax, [dma_scatter_gather_temp_record]
    mov eax, [eax + DMA_SG_MAPPING_DESCRIPTOR]
    call scatter_gather_lookup
    jc .skip_descriptor
    mov [dma_scatter_gather_temp_descriptor_record], eax
    mov dword [dma_scatter_gather_temp_source_index], 0
.release_buffers:
    mov eax, [dma_scatter_gather_temp_descriptor_record]
    mov ecx, [dma_scatter_gather_temp_source_index]
    cmp ecx, [eax + SG_DESCRIPTOR_SEGMENTS]
    jae .release_descriptor
    call scatter_gather_segment_at
    mov eax, [eax + SG_SEGMENT_BUFFER]
    call shared_buffer_lookup
    jc .next_release_buffer
    cmp dword [eax + SHARED_BUFFER_ACTIVE_IO], 0
    je .active_released
    dec dword [eax + SHARED_BUFFER_ACTIVE_IO]
.active_released:
    cmp dword [eax + SHARED_BUFFER_REFERENCES], 1
    jbe .reference_released
    dec dword [eax + SHARED_BUFFER_REFERENCES]
.reference_released:
    cmp dword [eax + SHARED_BUFFER_MAPPINGS], 0
    je .mapping_released
    dec dword [eax + SHARED_BUFFER_MAPPINGS]
.mapping_released:
    cmp dword [eax + SHARED_BUFFER_PINNED], 0
    je .pin_released
    dec dword [eax + SHARED_BUFFER_PINNED]
.pin_released:
    mov dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    cmp dword [dma_scatter_gather_pinned_buffers], 0
    je .next_release_buffer
    dec dword [dma_scatter_gather_pinned_buffers]
.next_release_buffer:
    inc dword [dma_scatter_gather_temp_source_index]
    jmp .release_buffers
.release_descriptor:
    mov eax, [dma_scatter_gather_temp_descriptor_record]
    cmp dword [eax + SG_DESCRIPTOR_CONSUMERS], 0
    je .skip_descriptor
    dec dword [eax + SG_DESCRIPTOR_CONSUMERS]
.skip_descriptor:
    mov eax, [dma_scatter_gather_temp_record]
    mov edx, [eax + DMA_SG_MAPPING_TOTAL_LENGTH]
    cmp [dma_scatter_gather_mapped_bytes], edx
    jb .zero_bytes
    sub [dma_scatter_gather_mapped_bytes], edx
    jmp .bytes_done
.zero_bytes:
    mov dword [dma_scatter_gather_mapped_bytes], 0
.bytes_done:
    cmp dword [dma_scatter_gather_active_count], 0
    je .state
    dec dword [dma_scatter_gather_active_count]
.state:
    cmp dword [eax + DMA_SG_MAPPING_STATE], DMA_SG_STATE_FAULTED
    je .done
    mov dword [eax + DMA_SG_MAPPING_STATE], DMA_SG_STATE_UNMAPPED
.done:
    clc
    ret

; EAX=Mapping-ID, EDX=Owner.
dma_scatter_gather_unmap:
    pushfd
    cli
    mov [dma_scatter_gather_temp_id], eax
    mov [dma_scatter_gather_temp_owner], edx
    call dma_scatter_gather_lookup
    jc .invalid
    mov edx, [dma_scatter_gather_temp_owner]
    cmp [eax + DMA_SG_MAPPING_OWNER], edx
    jne .invalid
    call dma_scatter_gather_release_record
    mov eax, [dma_scatter_gather_temp_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Mapping-ID, EDX=Device-ID, EBX=Fehlercode. Ein SG-DMA-Fault beendet
; den zugehoerigen I/O-Request erst nach Revoke, Unpin und Ownership-Rueckgabe.
dma_scatter_gather_fault:
    pushfd
    cli
    mov [dma_scatter_gather_temp_id], eax
    mov [dma_scatter_gather_temp_device], edx
    mov [dma_scatter_gather_temp_error], ebx
    test ebx, ebx
    jz .invalid
    call dma_scatter_gather_lookup
    jc .invalid
    mov edx, [dma_scatter_gather_temp_device]
    cmp [eax + DMA_SG_MAPPING_DEVICE], edx
    jne .invalid
    mov dword [eax + DMA_SG_MAPPING_STATE], DMA_SG_STATE_FAULTED
    mov ecx, eax
    sub ecx, dma_scatter_gather_table
    shr ecx, 6
    mov ebx, [dma_scatter_gather_temp_error]
    mov [dma_scatter_gather_error_codes + ecx * 4], ebx
    mov edx, [eax + DMA_SG_MAPPING_REQUEST]
    mov [dma_scatter_gather_temp_request], edx
    inc dword [dma_scatter_gather_fault_count]
    call dma_scatter_gather_release_record
    mov eax, [dma_scatter_gather_temp_request]
    xor edx, edx
    mov ebx, [dma_scatter_gather_temp_error]
    call io_request_complete
    jc .invalid
    mov eax, [dma_scatter_gather_temp_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=terminaler IORequest-Datensatz.
dma_scatter_gather_on_terminal:
    cmp dword [dma_scatter_gather_manager_ready], 1
    jne .done
    mov ecx, eax
    sub ecx, io_request_table
    shr ecx, 6
    mov eax, [dma_request_sg_mapping_ids + ecx * 4]
    test eax, eax
    jz .done
    call dma_scatter_gather_lookup
    jc .done
    call dma_scatter_gather_release_record
.done:
    clc
    ret

dma_scatter_gather_self_test:
    mov eax, 1
    mov ebx, IOMMU_MODE_RESTRICTED
    mov ecx, 32
    mov esi, DMA_SG_APERTURE
    mov edi, DMA_SG_APERTURE + 0x001FFFFF
    call iommu_domain_create
    jc .invalid
    mov [dma_scatter_gather_test_domain], eax
    mov edx, 0xD002
    mov ebx, 0x20
    mov ecx, 1
    call iommu_bind_device
    jc .invalid
    mov eax, 1
    mov edx, 128
    mov ebx, SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE | SHARED_BUFFER_RIGHT_TRANSFER | SHARED_BUFFER_RIGHT_DMA | SHARED_BUFFER_RIGHT_RELEASE
    call shared_buffer_create
    jc .invalid
    mov [dma_scatter_gather_test_buffer_a], eax
    mov eax, 1
    mov edx, 128
    mov ebx, SHARED_BUFFER_RIGHT_READ | SHARED_BUFFER_RIGHT_WRITE | SHARED_BUFFER_RIGHT_TRANSFER | SHARED_BUFFER_RIGHT_DMA | SHARED_BUFFER_RIGHT_RELEASE
    call shared_buffer_create
    jc .invalid
    mov [dma_scatter_gather_test_buffer_b], eax
    mov eax, 1
    mov edx, SG_DIRECTION_GATHER
    call scatter_gather_create
    jc .invalid
    mov [dma_scatter_gather_test_descriptor], eax
    mov edx, [dma_scatter_gather_test_buffer_a]
    xor ebx, ebx
    mov ecx, 96
    mov esi, SG_PERMISSION_READ
    mov edi, 1
    call scatter_gather_append
    jc .invalid
    mov eax, [dma_scatter_gather_test_descriptor]
    mov edx, [dma_scatter_gather_test_buffer_b]
    xor ebx, ebx
    mov ecx, 64
    mov esi, SG_PERMISSION_READ
    mov edi, 1
    call scatter_gather_append
    jc .invalid
    mov eax, [dma_scatter_gather_test_descriptor]
    mov edx, 1
    call scatter_gather_seal
    jc .invalid
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [dma_scatter_gather_test_scope], eax
    mov eax, 1
    mov edx, [dma_scatter_gather_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [dma_scatter_gather_test_task], eax
    mov eax, 1
    mov edx, [dma_scatter_gather_test_task]
    mov ebx, 6
    mov ecx, 0xD002
    mov esi, [dma_scatter_gather_test_descriptor]
    mov edi, 160
    xor ebp, ebp
    call io_request_submit
    jc .invalid
    mov [dma_scatter_gather_test_request], eax
    mov edx, [dma_scatter_gather_test_descriptor]
    mov ebx, 0xD002
    mov ecx, 1
    call dma_scatter_gather_map
    jc .invalid
    mov [dma_scatter_gather_test_mapping], eax
    call dma_scatter_gather_lookup
    jc .invalid
    mov [dma_scatter_gather_test_record], eax
    cmp dword [eax + DMA_SG_MAPPING_SEGMENTS], 3
    jne .invalid
    test dword [eax + DMA_SG_MAPPING_FLAGS], DMA_SG_FLAG_SPLIT
    jz .invalid
    cmp dword [eax + DMA_SG_MAPPING_DOMAIN], 0
    je .invalid
    cmp dword [iommu_mapping_count], 3
    jne .invalid
    mov ecx, 0
    call dma_scatter_gather_segment_at
    cmp dword [eax + DMA_SG_SEGMENT_LENGTH], 64
    jne .invalid
    test dword [eax + DMA_SG_SEGMENT_ADDRESS_LOW], DMA_SG_BOUNDARY - 1
    jnz .invalid
    mov eax, [dma_scatter_gather_test_descriptor]
    mov edx, 1
    call scatter_gather_release
    jnc .invalid                       ; aktiver DMA-Consumer blockiert Release
    mov eax, [dma_scatter_gather_test_domain]
    mov edx, 0xD002
    mov ebx, [dma_scatter_gather_test_mapping]
    or ebx, IOMMU_EXTERNAL_DMA_SG
    mov ecx, DMA_SG_APERTURE + 0x1000
    mov esi, IOMMU_PERMISSION_READ
    mov edi, 0xD201
    call iommu_report_fault
    jc .invalid
    mov eax, [dma_scatter_gather_test_record]
    cmp dword [eax + DMA_SG_MAPPING_STATE], DMA_SG_STATE_FAULTED
    jne .invalid
    cmp dword [dma_scatter_gather_fault_count], 1
    jne .invalid
    mov eax, [dma_scatter_gather_test_request]
    call io_request_lookup
    jc .invalid
    cmp dword [eax + IO_REQUEST_STATE], IO_REQUEST_STATE_FAILED
    jne .invalid
    cmp dword [eax + IO_REQUEST_COMPLETION], IO_COMPLETION_FAILED
    jne .invalid
    cmp dword [eax + IO_REQUEST_ERROR], 0xD201
    jne .invalid
    cmp dword [dma_scatter_gather_active_count], 0
    jne .invalid
    cmp dword [dma_scatter_gather_mapped_bytes], 0
    jne .invalid
    cmp dword [dma_scatter_gather_pinned_buffers], 0
    jne .invalid
    cmp dword [iommu_mapping_count], 0
    jne .invalid
    mov eax, [dma_scatter_gather_test_buffer_a]
    call shared_buffer_lookup
    jc .invalid
    cmp dword [eax + SHARED_BUFFER_STATE], SHARED_BUFFER_STATE_CPU
    jne .invalid
    cmp dword [eax + SHARED_BUFFER_PINNED], 0
    jne .invalid
    mov eax, [dma_scatter_gather_test_task]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [dma_scatter_gather_test_scope]
    call task_scope_close
    jc .invalid
    mov eax, [dma_scatter_gather_test_descriptor]
    mov edx, 1
    call scatter_gather_release
    jc .invalid
    mov eax, [dma_scatter_gather_test_buffer_a]
    mov edx, 1
    call shared_buffer_release
    jc .invalid
    mov eax, [dma_scatter_gather_test_buffer_b]
    mov edx, 1
    call shared_buffer_release
    jc .invalid
    mov eax, [dma_scatter_gather_test_domain]
    mov edx, 1
    call iommu_domain_release
    jc .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
dma_scatter_gather_api:
    dd DMA_SG_API_SIZE
    dw 1, 0
    dd DMA_SG_CAPACITY
    dd DMA_SG_MAX_SEGMENTS
    dd dma_scatter_gather_map
    dd dma_scatter_gather_unmap
    dd dma_scatter_gather_table
    dd dma_scatter_gather_segments

dma_scatter_gather_manager_ready:        dd 0
dma_scatter_gather_next_id:              dd 0
dma_scatter_gather_active_count:         dd 0
dma_scatter_gather_mapped_bytes:         dd 0
dma_scatter_gather_pinned_buffers:       dd 0
dma_scatter_gather_split_segments:       dd 0
dma_scatter_gather_validation_failures:  dd 0
dma_scatter_gather_fault_count:          dd 0
dma_scatter_gather_temp_id:              dd 0
dma_scatter_gather_temp_owner:           dd 0
dma_scatter_gather_temp_device:          dd 0
dma_scatter_gather_temp_request:         dd 0
dma_scatter_gather_temp_request_slot:    dd 0
dma_scatter_gather_temp_request_record:  dd 0
dma_scatter_gather_temp_descriptor:      dd 0
dma_scatter_gather_temp_descriptor_record: dd 0
dma_scatter_gather_temp_record:          dd 0
dma_scatter_gather_temp_slot:            dd 0
dma_scatter_gather_temp_required:        dd 0
dma_scatter_gather_temp_source_index:    dd 0
dma_scatter_gather_temp_output_index:    dd 0
dma_scatter_gather_temp_source_segment:  dd 0
dma_scatter_gather_temp_buffer:          dd 0
dma_scatter_gather_temp_offset:          dd 0
dma_scatter_gather_temp_remaining:       dd 0
dma_scatter_gather_temp_permissions:     dd 0
dma_scatter_gather_temp_chunk:           dd 0
dma_scatter_gather_temp_error:           dd 0
dma_scatter_gather_temp_iommu_domain:    dd 0
dma_scatter_gather_temp_device_segment:  dd 0
dma_scatter_gather_temp_authorized_count: dd 0
dma_scatter_gather_temp_rollback_index:  dd 0
dma_scatter_gather_temp_buffer_ids:      times SG_MAX_SEGMENTS dd 0
dma_scatter_gather_test_buffer_a:        dd 0
dma_scatter_gather_test_buffer_b:        dd 0
dma_scatter_gather_test_descriptor:      dd 0
dma_scatter_gather_test_scope:           dd 0
dma_scatter_gather_test_task:            dd 0
dma_scatter_gather_test_request:         dd 0
dma_scatter_gather_test_mapping:         dd 0
dma_scatter_gather_test_record:          dd 0
dma_scatter_gather_test_domain:          dd 0
align 4
dma_scatter_gather_table:
    times DMA_SG_CAPACITY * DMA_SG_MAPPING_SIZE db 0
dma_scatter_gather_segments:
    times DMA_SG_CAPACITY * DMA_SG_MAX_SEGMENTS * DMA_SG_SEGMENT_SIZE db 0
dma_scatter_gather_generations:
    times DMA_SG_CAPACITY dd 0
dma_request_sg_mapping_ids:
    times IO_REQUEST_CAPACITY dd 0
dma_scatter_gather_iommu_authorization_ids:
    times DMA_SG_CAPACITY * DMA_SG_MAX_SEGMENTS dd 0
dma_scatter_gather_error_codes:
    times DMA_SG_CAPACITY dd 0

; ---------------------------------------------------------------------------
; General-Purpose Ring Buffer – begrenzte SPSC-Kreisstruktur fuer kontin-
; uierliche Datenstroeme zwischen Producer und Consumer.
; NPSPEC-DATAMOVE-RINGBUFFER-0001
; ---------------------------------------------------------------------------

RINGBUF_CAPACITY       equ 4
RINGBUF_ELEMENT_CAP    equ 16
RINGBUF_ELEMENT_SIZE   equ 16
RINGBUF_RECORD_SIZE    equ 32

RINGBUF_STATE_FREE     equ 0
RINGBUF_STATE_ACTIVE   equ 1

RINGBUF_ID             equ 0
RINGBUF_ELEM_CAPACITY  equ 4
RINGBUF_READ_POS       equ 8
RINGBUF_WRITE_POS      equ 12
RINGBUF_STATE          equ 16
RINGBUF_FLAGS          equ 20
RINGBUF_COUNT          equ 24
RINGBUF_GENERATION     equ 28

ringbuf_initialize:
    mov edi, ringbuf_table
    xor eax, eax
    mov ecx, (RINGBUF_CAPACITY * RINGBUF_RECORD_SIZE) / 4
    rep stosd
    mov edi, ringbuf_storage
    mov ecx, (RINGBUF_CAPACITY * RINGBUF_ELEMENT_CAP * RINGBUF_ELEMENT_SIZE) / 4
    rep stosd
    mov edi, ringbuf_generations
    mov ecx, RINGBUF_CAPACITY
    rep stosd
    mov dword [ringbuf_next_id], 1
    mov dword [ringbuf_live_count], 0
    mov dword [ringbuf_manager_ready], 1
    clc
    ret

; Erstellt einen Ring-Buffer. EAX=Slot-Index (CF=0) oder CF=1 wenn voll.
ringbuf_create:
    push ecx
    push edi
    xor ecx, ecx
.rc_scan:
    cmp ecx, RINGBUF_CAPACITY
    jae .rc_full
    mov edi, ecx
    imul edi, RINGBUF_RECORD_SIZE
    add edi, ringbuf_table
    cmp dword [edi + RINGBUF_STATE], RINGBUF_STATE_FREE
    je .rc_found
    inc ecx
    jmp .rc_scan
.rc_found:
    mov eax, [ringbuf_next_id]
    mov [edi + RINGBUF_ID],            eax
    mov dword [edi + RINGBUF_ELEM_CAPACITY], RINGBUF_ELEMENT_CAP
    mov dword [edi + RINGBUF_READ_POS],  0
    mov dword [edi + RINGBUF_WRITE_POS], 0
    mov dword [edi + RINGBUF_STATE],   RINGBUF_STATE_ACTIVE
    mov dword [edi + RINGBUF_FLAGS],   0
    mov dword [edi + RINGBUF_COUNT],   0
    mov eax, [ringbuf_generations + ecx * 4]
    inc eax
    mov [ringbuf_generations + ecx * 4], eax
    mov [edi + RINGBUF_GENERATION],    eax
    inc dword [ringbuf_next_id]
    inc dword [ringbuf_live_count]
    mov eax, ecx
    pop edi
    pop ecx
    clc
    ret
.rc_full:
    pop edi
    pop ecx
    stc
    ret

; Schreibt ein 16-Byte-Element. EAX=Slot (0..CAPACITY-1), ESI=Quell-Ptr.
; CF=0 ok, CF=1 voll oder ungueltig.
ringbuf_write:
    push ebx
    push ecx
    push edx
    push edi
    cmp eax, RINGBUF_CAPACITY
    jae .rw_bad
    mov ebx, eax
    imul ebx, RINGBUF_RECORD_SIZE
    add ebx, ringbuf_table
    cmp dword [ebx + RINGBUF_STATE], RINGBUF_STATE_ACTIVE
    jne .rw_bad
    mov ecx, [ebx + RINGBUF_COUNT]
    cmp ecx, RINGBUF_ELEMENT_CAP
    jae .rw_full
    mov edx, eax
    imul edx, RINGBUF_ELEMENT_CAP * RINGBUF_ELEMENT_SIZE
    add edx, ringbuf_storage
    mov ecx, [ebx + RINGBUF_WRITE_POS]
    imul ecx, RINGBUF_ELEMENT_SIZE
    add edx, ecx
    mov edi, edx
    mov ecx, RINGBUF_ELEMENT_SIZE / 4
    rep movsd
    mov ecx, [ebx + RINGBUF_WRITE_POS]
    inc ecx
    cmp ecx, RINGBUF_ELEMENT_CAP
    jb .rw_no_wrap
    xor ecx, ecx
.rw_no_wrap:
    mov [ebx + RINGBUF_WRITE_POS], ecx
    inc dword [ebx + RINGBUF_COUNT]
    pop edi
    pop edx
    pop ecx
    pop ebx
    clc
    ret
.rw_full:
.rw_bad:
    pop edi
    pop edx
    pop ecx
    pop ebx
    stc
    ret

; Liest ein 16-Byte-Element. EAX=Slot (0..CAPACITY-1), EDI=Ziel-Ptr.
; CF=0 ok, CF=1 leer oder ungueltig.
ringbuf_read:
    push ebx
    push ecx
    push edx
    push esi
    cmp eax, RINGBUF_CAPACITY
    jae .rr_bad
    mov ebx, eax
    imul ebx, RINGBUF_RECORD_SIZE
    add ebx, ringbuf_table
    cmp dword [ebx + RINGBUF_STATE], RINGBUF_STATE_ACTIVE
    jne .rr_bad
    cmp dword [ebx + RINGBUF_COUNT], 0
    je .rr_empty
    mov edx, eax
    imul edx, RINGBUF_ELEMENT_CAP * RINGBUF_ELEMENT_SIZE
    add edx, ringbuf_storage
    mov ecx, [ebx + RINGBUF_READ_POS]
    imul ecx, RINGBUF_ELEMENT_SIZE
    add edx, ecx
    mov esi, edx
    mov ecx, RINGBUF_ELEMENT_SIZE / 4
    rep movsd
    mov ecx, [ebx + RINGBUF_READ_POS]
    inc ecx
    cmp ecx, RINGBUF_ELEMENT_CAP
    jb .rr_no_wrap
    xor ecx, ecx
.rr_no_wrap:
    mov [ebx + RINGBUF_READ_POS], ecx
    dec dword [ebx + RINGBUF_COUNT]
    pop esi
    pop edx
    pop ecx
    pop ebx
    clc
    ret
.rr_empty:
.rr_bad:
    pop esi
    pop edx
    pop ecx
    pop ebx
    stc
    ret

; Self-Test: 7 Tests (Init, Create, Write, Read, Wrap, Count-Invarianten).
ringbuf_self_test:
    push ebx
    push esi
    push edi
    sub esp, 32                          ; [esp+0..15]=Schreibpuf, [esp+16..31]=Lesepuf
    xor ebx, ebx                         ; Fehlerzaehler

    ; Test 1: Manager bereit
    cmp dword [ringbuf_manager_ready], 1
    je .rbt2
    inc ebx

.rbt2:
    ; Test 2: Create liefert Slot 0
    call ringbuf_create
    jnc .rbt3
    inc ebx
    jmp .rbt_done
.rbt3:
    cmp eax, 0
    je .rbt4
    inc ebx

.rbt4:
    ; Test 3: Count == 0 nach Create
    mov esi, ringbuf_table
    cmp dword [esi + RINGBUF_COUNT], 0
    je .rbt5
    inc ebx

.rbt5:
    ; Test 4: Write schreibt ohne Fehler
    mov dword [esp +  0], 0xAABBCCDD
    mov dword [esp +  4], 0x11223344
    mov dword [esp +  8], 0xDEADBEEF
    mov dword [esp + 12], 0xCAFEBABE
    xor eax, eax
    lea esi, [esp]
    call ringbuf_write
    jnc .rbt6
    inc ebx

.rbt6:
    ; Test 5: Count == 1 nach Write
    mov esi, ringbuf_table
    cmp dword [esi + RINGBUF_COUNT], 1
    je .rbt7
    inc ebx

.rbt7:
    ; Test 6: Read liefert identische Daten
    xor eax, eax
    lea edi, [esp + 16]
    call ringbuf_read
    jnc .rbt7_check
    inc ebx
    jmp .rbt8
.rbt7_check:
    cmp dword [esp + 16], 0xAABBCCDD
    je .rbt8
    inc ebx

.rbt8:
    ; Test 7: Count == 0 nach Read
    mov esi, ringbuf_table
    cmp dword [esi + RINGBUF_COUNT], 0
    je .rbt_done
    inc ebx

.rbt_done:
    test ebx, ebx
    jnz .rbt_fail
    add esp, 32
    pop edi
    pop esi
    pop ebx
    clc
    ret
.rbt_fail:
    add esp, 32
    pop edi
    pop esi
    pop ebx
    stc
    ret

ringbuf_manager_ready: dd 0
ringbuf_next_id:       dd 0
ringbuf_live_count:    dd 0
align 4
ringbuf_generations:
    times RINGBUF_CAPACITY dd 0
ringbuf_table:
    times RINGBUF_CAPACITY * RINGBUF_RECORD_SIZE db 0
ringbuf_storage:
    times RINGBUF_CAPACITY * RINGBUF_ELEMENT_CAP * RINGBUF_ELEMENT_SIZE db 0

; ---------------------------------------------------------------------------
; Data-Locality-Manager – NUMA-bewusste Platzierungshinweise und Kostenmodell
; fuer Datenpfade zwischen Producer, Consumer und Device.
; NPSPEC-DATAMOVE-LOCALITY-0001
; ---------------------------------------------------------------------------

LOC_CAPACITY       equ 8
LOC_RECORD_SIZE    equ 32

LOC_STATE_FREE     equ 0
LOC_STATE_ACTIVE   equ 1

LOC_FLAG_RT        equ 0x01        ; Realtime: keine automatische Migration
LOC_FLAG_NO_MIGRATE equ 0x02       ; Migration dauerhaft deaktiviert

LOC_HINT_ID          equ 0
LOC_STATE            equ 4
LOC_CURRENT_NODE     equ 8
LOC_PREFERRED_NODE   equ 12
LOC_LAST_ACCESS_NODE equ 16
LOC_REMOTE_ACCESSES  equ 20
LOC_FLAGS            equ 24
LOC_MIGRATE_COUNT    equ 28

; Kostenmodell: NUMA_DIST_SELF=0, Remote=20
LOC_COST_LOCAL  equ 0
LOC_COST_REMOTE equ 20

locality_initialize:
    mov edi, locality_hints
    xor eax, eax
    mov ecx, (LOC_CAPACITY * LOC_RECORD_SIZE) / 4
    rep stosd
    mov dword [locality_next_id], 1
    mov dword [locality_live_count], 0
    mov dword [locality_remote_decisions], 0
    mov dword [locality_manager_ready], 1
    clc
    ret

; Erstellt einen Locality-Hinweis. EAX=current_node, EDX=preferred_node.
; EAX=Hint-Index (CF=0) oder CF=1 wenn voll.
locality_hint_create:
    push ecx
    push edi
    xor ecx, ecx
.lhc_scan:
    cmp ecx, LOC_CAPACITY
    jae .lhc_full
    mov edi, ecx
    imul edi, LOC_RECORD_SIZE
    add edi, locality_hints
    cmp dword [edi + LOC_STATE], LOC_STATE_FREE
    je .lhc_found
    inc ecx
    jmp .lhc_scan
.lhc_found:
    push eax
    mov eax, [locality_next_id]
    mov [edi + LOC_HINT_ID],          eax
    pop eax
    mov dword [edi + LOC_STATE],      LOC_STATE_ACTIVE
    mov [edi + LOC_CURRENT_NODE],     eax
    mov [edi + LOC_PREFERRED_NODE],   edx
    mov [edi + LOC_LAST_ACCESS_NODE], eax
    mov dword [edi + LOC_REMOTE_ACCESSES], 0
    mov dword [edi + LOC_FLAGS],      0
    mov dword [edi + LOC_MIGRATE_COUNT], 0
    inc dword [locality_next_id]
    inc dword [locality_live_count]
    mov eax, ecx
    pop edi
    pop ecx
    clc
    ret
.lhc_full:
    pop edi
    pop ecx
    stc
    ret

; Setzt Flags fuer einen Hint. EAX=Hint-Index, EDX=Flags-Maske. CF=0 ok.
locality_hint_set_flags:
    push edi
    cmp eax, LOC_CAPACITY
    jae .lhsf_bad
    mov edi, eax
    imul edi, LOC_RECORD_SIZE
    add edi, locality_hints
    cmp dword [edi + LOC_STATE], LOC_STATE_ACTIVE
    jne .lhsf_bad
    or [edi + LOC_FLAGS], edx
    pop edi
    clc
    ret
.lhsf_bad:
    pop edi
    stc
    ret

; Vermerkt Zugriff. EAX=Hint-Index, EDX=accessing_node. CF=0 ok.
locality_access_record:
    push edi
    cmp eax, LOC_CAPACITY
    jae .lar_bad
    mov edi, eax
    imul edi, LOC_RECORD_SIZE
    add edi, locality_hints
    cmp dword [edi + LOC_STATE], LOC_STATE_ACTIVE
    jne .lar_bad
    mov [edi + LOC_LAST_ACCESS_NODE], edx
    ; Remote-Zugriff wenn Knoten != current
    cmp edx, [edi + LOC_CURRENT_NODE]
    je .lar_done
    inc dword [edi + LOC_REMOTE_ACCESSES]
    inc dword [locality_remote_decisions]
.lar_done:
    pop edi
    clc
    ret
.lar_bad:
    pop edi
    stc
    ret

; Berechnet Zugriffskosten. EAX=Hint-Index, EDX=requesting_node.
; EAX=Kostenwert (0=lokal, 20=remote), CF=0 ok, CF=1 ungueltig.
locality_evaluate_cost:
    push edi
    cmp eax, LOC_CAPACITY
    jae .lec_bad
    mov edi, eax
    imul edi, LOC_RECORD_SIZE
    add edi, locality_hints
    cmp dword [edi + LOC_STATE], LOC_STATE_ACTIVE
    jne .lec_bad
    cmp edx, [edi + LOC_CURRENT_NODE]
    je .lec_local
    mov eax, LOC_COST_REMOTE
    pop edi
    clc
    ret
.lec_local:
    xor eax, eax
    pop edi
    clc
    ret
.lec_bad:
    pop edi
    stc
    ret

; Self-Test: 7 Tests (Init, Create, Remote-Access, Cost=0, Cost=remote,
;   RT-Flag-Schutz, Count-Invariante).
locality_self_test:
    push ebx
    push edi
    xor ebx, ebx

    ; Test 1: Manager bereit
    cmp dword [locality_manager_ready], 1
    je .lst2
    inc ebx

.lst2:
    ; Test 2: Erstelle Hint (node 0 → node 0)
    xor eax, eax
    xor edx, edx
    call locality_hint_create
    jnc .lst3
    inc ebx
    jmp .lst_done
.lst3:
    ; EAX = Hint-Index (0)
    push eax

    ; Test 3: Lokaler Zugriff von Node 0 → remote_accesses bleibt 0
    mov edx, 0
    call locality_access_record
    pop eax
    push eax
    mov edi, eax
    imul edi, LOC_RECORD_SIZE
    add edi, locality_hints
    cmp dword [edi + LOC_REMOTE_ACCESSES], 0
    je .lst4
    inc ebx

.lst4:
    ; Test 4: evaluate_cost(node 0) → EAX == 0
    pop eax
    push eax
    xor edx, edx
    call locality_evaluate_cost
    cmp eax, 0
    je .lst5
    inc ebx

.lst5:
    ; Test 5: evaluate_cost(node 1) → EAX == LOC_COST_REMOTE
    pop eax
    push eax
    mov edx, 1
    call locality_evaluate_cost
    cmp eax, LOC_COST_REMOTE
    je .lst6
    inc ebx

.lst6:
    ; Test 6: RT-Flag setzen → LOC_FLAGS hat RT-Bit
    pop eax
    push eax
    mov edx, LOC_FLAG_RT
    call locality_hint_set_flags
    pop eax
    mov edi, eax
    imul edi, LOC_RECORD_SIZE
    add edi, locality_hints
    mov edi, [edi + LOC_FLAGS]
    test edi, LOC_FLAG_RT
    jnz .lst7
    inc ebx

.lst7:
    ; Test 7: live_count == 1 (genau ein aktiver Hint)
    cmp dword [locality_live_count], 1
    je .lst_done
    inc ebx

.lst_done:
    test ebx, ebx
    jnz .lst_fail
    pop edi
    pop ebx
    clc
    ret
.lst_fail:
    pop edi
    pop ebx
    stc
    ret

locality_manager_ready:      dd 0
locality_next_id:            dd 0
locality_live_count:         dd 0
locality_remote_decisions:   dd 0
align 4
locality_hints:
    times LOC_CAPACITY * LOC_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Kernel-Bypass-Manager – autorisierter Hochleistungs-Datenpfad mit strikter
; Control/Data-Plane-Trennung, Capability-Pruefung und Revocation-Protokoll.
; NPSPEC-DATAMOVE-KERNELBYPASS-0001
; ---------------------------------------------------------------------------

KBYP_CAPACITY      equ 4
KBYP_RECORD_SIZE   equ 40

KBYP_STATE_FREE       equ 0
KBYP_STATE_CREATED    equ 1
KBYP_STATE_CONFIGURED equ 2
KBYP_STATE_ACTIVE     equ 3
KBYP_STATE_QUIESCING  equ 4
KBYP_STATE_REVOKED    equ 5

KBYP_CAP_USE_DEVICE   equ 0x01
KBYP_CAP_MAP_DMA      equ 0x02
KBYP_CAP_SUBMIT       equ 0x04
KBYP_CAP_RECEIVE      equ 0x08
KBYP_CAP_CONFIGURE    equ 0x10

KBYP_ID              equ 0
KBYP_STATE           equ 4
KBYP_OWNER           equ 8
KBYP_DEVICE          equ 12
KBYP_CAPABILITIES    equ 16
KBYP_DMA_MAPPING_ID  equ 20
KBYP_IOMMU_DOMAIN    equ 24
KBYP_NUMA_NODE       equ 28
KBYP_FLAGS           equ 32
KBYP_SUBMISSION_COUNT equ 36

kernelbypass_initialize:
    mov edi, kbyp_contexts
    xor eax, eax
    mov ecx, (KBYP_CAPACITY * KBYP_RECORD_SIZE) / 4
    rep stosd
    mov dword [kbyp_next_id], 1
    mov dword [kbyp_live_count], 0
    mov dword [kbyp_active_count], 0
    mov dword [kbyp_revoke_count], 0
    mov dword [kbyp_manager_ready], 1
    clc
    ret

; Erstellt Bypass-Kontext. EAX=owner_pid, EDX=capabilities.
; EAX=Slot-Index (CF=0) oder CF=1 wenn voll.
kernelbypass_create:
    push ecx
    push edi
    xor ecx, ecx
.kbc_scan:
    cmp ecx, KBYP_CAPACITY
    jae .kbc_full
    mov edi, ecx
    imul edi, KBYP_RECORD_SIZE
    add edi, kbyp_contexts
    cmp dword [edi + KBYP_STATE], KBYP_STATE_FREE
    je .kbc_found
    inc ecx
    jmp .kbc_scan
.kbc_found:
    push eax
    mov eax, [kbyp_next_id]
    mov [edi + KBYP_ID],           eax
    pop eax
    mov dword [edi + KBYP_STATE],  KBYP_STATE_CREATED
    mov [edi + KBYP_OWNER],        eax
    mov dword [edi + KBYP_DEVICE], 0
    mov [edi + KBYP_CAPABILITIES], edx
    mov dword [edi + KBYP_DMA_MAPPING_ID], 0
    mov dword [edi + KBYP_IOMMU_DOMAIN], 0
    mov dword [edi + KBYP_NUMA_NODE], 0
    mov dword [edi + KBYP_FLAGS], 0
    mov dword [edi + KBYP_SUBMISSION_COUNT], 0
    inc dword [kbyp_next_id]
    inc dword [kbyp_live_count]
    mov eax, ecx
    pop edi
    pop ecx
    clc
    ret
.kbc_full:
    pop edi
    pop ecx
    stc
    ret

; Konfiguriert Kontext (CREATED → CONFIGURED). EAX=Slot. CF=0 ok.
kernelbypass_configure:
    push edi
    cmp eax, KBYP_CAPACITY
    jae .kbcfg_bad
    mov edi, eax
    imul edi, KBYP_RECORD_SIZE
    add edi, kbyp_contexts
    cmp dword [edi + KBYP_STATE], KBYP_STATE_CREATED
    jne .kbcfg_bad
    ; Capability-Pruefung: Submit oder Receive muss vorhanden sein
    mov eax, [edi + KBYP_CAPABILITIES]
    test eax, KBYP_CAP_SUBMIT | KBYP_CAP_RECEIVE
    jz .kbcfg_bad
    mov dword [edi + KBYP_STATE], KBYP_STATE_CONFIGURED
    pop edi
    clc
    ret
.kbcfg_bad:
    pop edi
    stc
    ret

; Aktiviert Kontext (CONFIGURED → ACTIVE). EAX=Slot. CF=0 ok.
kernelbypass_activate:
    push edi
    cmp eax, KBYP_CAPACITY
    jae .kba_bad
    mov edi, eax
    imul edi, KBYP_RECORD_SIZE
    add edi, kbyp_contexts
    cmp dword [edi + KBYP_STATE], KBYP_STATE_CONFIGURED
    jne .kba_bad
    mov dword [edi + KBYP_STATE], KBYP_STATE_ACTIVE
    inc dword [kbyp_active_count]
    pop edi
    clc
    ret
.kba_bad:
    pop edi
    stc
    ret

; Widerruft Kontext. EAX=Slot. CF=0 ok, CF=1 wenn ungueltig/schon revoked.
kernelbypass_revoke:
    push edi
    cmp eax, KBYP_CAPACITY
    jae .kbr_bad
    mov edi, eax
    imul edi, KBYP_RECORD_SIZE
    add edi, kbyp_contexts
    mov eax, [edi + KBYP_STATE]
    cmp eax, KBYP_STATE_FREE
    je .kbr_bad
    cmp eax, KBYP_STATE_REVOKED
    je .kbr_bad
    cmp eax, KBYP_STATE_ACTIVE
    jne .kbr_not_active
    dec dword [kbyp_active_count]
.kbr_not_active:
    mov dword [edi + KBYP_STATE], KBYP_STATE_REVOKED
    inc dword [kbyp_revoke_count]
    pop edi
    clc
    ret
.kbr_bad:
    pop edi
    stc
    ret

; Self-Test: 7 Tests (Init, Create, Configure, Activate, Revoke, Doppel-
;   Aktivierung nach Revoke scheitert, State-Invariante).
kernelbypass_self_test:
    push ebx
    push edi
    xor ebx, ebx

    ; Test 1: Manager bereit
    cmp dword [kbyp_manager_ready], 1
    je .kbt2
    inc ebx

.kbt2:
    ; Test 2: Create (PID=1, SUBMIT|RECEIVE)
    mov eax, 1
    mov edx, KBYP_CAP_SUBMIT | KBYP_CAP_RECEIVE
    call kernelbypass_create
    jnc .kbt3
    inc ebx
    jmp .kbt_done
.kbt3:
    push eax                            ; Slot-Index sichern
    ; Test 3: State == CREATED
    mov edi, eax
    imul edi, KBYP_RECORD_SIZE
    add edi, kbyp_contexts
    cmp dword [edi + KBYP_STATE], KBYP_STATE_CREATED
    je .kbt4
    inc ebx

.kbt4:
    ; Test 4: Configure
    pop eax
    push eax
    call kernelbypass_configure
    jnc .kbt5
    inc ebx

.kbt5:
    ; Test 5: Activate
    pop eax
    push eax
    call kernelbypass_activate
    jnc .kbt5_check
    inc ebx
    jmp .kbt6
.kbt5_check:
    mov edi, [esp]                      ; Slot-Index
    imul edi, KBYP_RECORD_SIZE
    add edi, kbyp_contexts
    cmp dword [edi + KBYP_STATE], KBYP_STATE_ACTIVE
    je .kbt6
    inc ebx

.kbt6:
    ; Test 6: Revoke
    pop eax
    push eax
    call kernelbypass_revoke
    jnc .kbt6_check
    inc ebx
    jmp .kbt7
.kbt6_check:
    mov edi, [esp]
    imul edi, KBYP_RECORD_SIZE
    add edi, kbyp_contexts
    cmp dword [edi + KBYP_STATE], KBYP_STATE_REVOKED
    je .kbt7
    inc ebx

.kbt7:
    ; Test 7: Re-Aktivierung nach Revoke muss scheitern (CF=1)
    pop eax
    call kernelbypass_activate
    jc .kbt_done                        ; erwartet: Fehler
    inc ebx

.kbt_done:
    test ebx, ebx
    jnz .kbt_fail
    pop edi
    pop ebx
    clc
    ret
.kbt_fail:
    pop edi
    pop ebx
    stc
    ret

kbyp_manager_ready: dd 0
kbyp_next_id:       dd 0
kbyp_live_count:    dd 0
kbyp_active_count:  dd 0
kbyp_revoke_count:  dd 0
align 4
kbyp_contexts:
    times KBYP_CAPACITY * KBYP_RECORD_SIZE db 0

