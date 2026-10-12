; NPSPEC-CONCURRENCY-TASK/STRUCTURED/CANCELLATION-0001
; ---------------------------------------------------------------------------

TASK_SCOPE_API_SIZE        equ 32
TASK_SCOPE_CAPACITY        equ 8
TASK_SCOPE_RECORD_SIZE     equ 32
TASK_SCOPE_STATE_EMPTY     equ 0
TASK_SCOPE_STATE_OPEN      equ 1
TASK_SCOPE_STATE_CANCELLING equ 2
TASK_SCOPE_STATE_CLOSED    equ 3
TASK_SCOPE_CAP_HIERARCHY   equ 0x00000001
TASK_SCOPE_CAP_CANCELLATION equ 0x00000002
TASK_SCOPE_CAP_BOUNDED     equ 0x00000004
TASK_SCOPE_ID              equ 0
TASK_SCOPE_OWNER           equ 4
TASK_SCOPE_PARENT          equ 8
TASK_SCOPE_STATE           equ 12
TASK_SCOPE_CHILDREN        equ 16
TASK_SCOPE_FLAGS           equ 20
TASK_SCOPE_CANCEL_REASON   equ 24
TASK_SCOPE_ACTIVE_TASKS    equ 28

task_scope_manager_initialize:
    mov edi, task_scope_table
    xor eax, eax
    mov ecx, (TASK_SCOPE_CAPACITY * TASK_SCOPE_RECORD_SIZE) / 4
    rep stosd
    mov dword [task_scope_count], 0
    mov dword [task_scope_next_id], 1
    mov dword [task_scope_kernel_root_id], 0

    ; Der initiale Kernelprozess besitzt einen dauerhaften Root-Scope. Jeder
    ; spaeter registrierte Kernelthread wird diesem Scope zugeordnet.
    mov eax, 1
    xor edx, edx
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [task_scope_kernel_root_id], eax
    clc
    ret
.invalid:
    stc
    ret

; EAX=Owner-PID, EDX=Parent-Scope oder 0, EBX=Flags. EAX=Scope-ID.
task_scope_create:
    pushfd
    cli
    mov [task_scope_temp_owner], eax
    mov [task_scope_temp_parent], edx
    mov [task_scope_temp_flags], ebx

    call process_lookup
    jc .invalid

    cmp dword [task_scope_temp_parent], 0
    je .find_slot
    mov eax, [task_scope_temp_parent]
    call task_scope_lookup
    jc .invalid
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_OPEN
    jne .invalid
    mov edx, [task_scope_temp_owner]
    cmp [eax + TASK_SCOPE_OWNER], edx
    jne .invalid

.find_slot:
    xor ecx, ecx
.scan:
    cmp ecx, TASK_SCOPE_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 5
    add edi, task_scope_table
    cmp dword [edi + TASK_SCOPE_STATE], TASK_SCOPE_STATE_EMPTY
    je .slot
    cmp dword [edi + TASK_SCOPE_STATE], TASK_SCOPE_STATE_CLOSED
    je .slot
    inc ecx
    jmp .scan
.slot:
    mov eax, [task_scope_next_id]
    mov [edi + TASK_SCOPE_ID], eax
    mov edx, [task_scope_temp_owner]
    mov [edi + TASK_SCOPE_OWNER], edx
    mov edx, [task_scope_temp_parent]
    mov [edi + TASK_SCOPE_PARENT], edx
    mov dword [edi + TASK_SCOPE_STATE], TASK_SCOPE_STATE_OPEN
    mov dword [edi + TASK_SCOPE_CHILDREN], 0
    mov edx, [task_scope_temp_flags]
    mov [edi + TASK_SCOPE_FLAGS], edx
    mov dword [edi + TASK_SCOPE_CANCEL_REASON], 0
    mov dword [edi + TASK_SCOPE_ACTIVE_TASKS], 0

    cmp dword [task_scope_temp_parent], 0
    je .created
    mov eax, [task_scope_temp_parent]
    call task_scope_lookup
    jc .invalid
    inc dword [eax + TASK_SCOPE_CHILDREN]
.created:
    inc dword [task_scope_count]
    inc dword [task_scope_next_id]
    mov eax, [edi + TASK_SCOPE_ID]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Scope-ID. EAX=Datensatz oder 0.
task_scope_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, TASK_SCOPE_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 5
    add edx, task_scope_table
    cmp dword [edx + TASK_SCOPE_STATE], TASK_SCOPE_STATE_EMPTY
    je .next
    cmp [edx + TASK_SCOPE_ID], eax
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

; EAX=Scope-ID, EDX=Cancellation-Grund. Propagiert hierarchisch.
task_scope_cancel:
    pushfd
    cli
    mov [task_scope_temp_cancel_id], eax
    mov [task_scope_temp_cancel_reason], edx
    call task_scope_lookup
    jc .invalid
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_CLOSED
    je .invalid
    mov dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_CANCELLING
    mov edx, [task_scope_temp_cancel_reason]
    mov [eax + TASK_SCOPE_CANCEL_REASON], edx

    ; Bei jeder Runde werden alle direkten Kinder bereits abgebrochener
    ; Scopes markiert. Die feste Tabellenkapazitaet begrenzt Laufzeit und
    ; Speicherverbrauch deterministisch.
.propagate:
    xor ebp, ebp
    xor ecx, ecx
.scan_children:
    cmp ecx, TASK_SCOPE_CAPACITY
    jae .propagated
    mov edi, ecx
    shl edi, 5
    add edi, task_scope_table
    cmp dword [edi + TASK_SCOPE_STATE], TASK_SCOPE_STATE_OPEN
    jne .next_child
    mov eax, [edi + TASK_SCOPE_PARENT]
    test eax, eax
    jz .next_child
    push ecx
    push edi
    call task_scope_lookup
    pop edi
    pop ecx
    jc .invalid
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_CANCELLING
    jne .next_child
    mov dword [edi + TASK_SCOPE_STATE], TASK_SCOPE_STATE_CANCELLING
    mov eax, [eax + TASK_SCOPE_CANCEL_REASON]
    mov [edi + TASK_SCOPE_CANCEL_REASON], eax
    mov ebp, 1
.next_child:
    inc ecx
    jmp .scan_children
.propagated:
    test ebp, ebp
    jnz .propagate
    mov edx, [task_scope_temp_cancel_reason]
    call task_manager_cancel_scopes
    popfd
    clc
    ret
.invalid:
    popfd
    stc
    ret

; EAX=Scope-ID. Abschluss ist erst ohne aktive Kinder zulaessig.
task_scope_close:
    pushfd
    cli
    mov [task_scope_temp_close_id], eax
    call task_scope_lookup
    jc .invalid
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_OPEN
    je .state_valid
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_CANCELLING
    jne .invalid
.state_valid:
    cmp dword [eax + TASK_SCOPE_CHILDREN], 0
    jne .invalid
    cmp dword [eax + TASK_SCOPE_ACTIVE_TASKS], 0
    jne .invalid
    mov edi, eax
    mov eax, [edi + TASK_SCOPE_PARENT]
    test eax, eax
    jz .close
    call task_scope_lookup
    jc .invalid
    cmp dword [eax + TASK_SCOPE_CHILDREN], 0
    je .invalid
    dec dword [eax + TASK_SCOPE_CHILDREN]
.close:
    mov dword [edi + TASK_SCOPE_STATE], TASK_SCOPE_STATE_CLOSED
    dec dword [task_scope_count]
    mov eax, [task_scope_temp_close_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

task_scope_manager_self_test:
    cmp dword [task_scope_kernel_root_id], 1
    jne .invalid
    mov eax, 1
    xor edx, edx
    mov ebx, 0x10
    call task_scope_create
    jc .invalid
    mov [task_scope_test_parent], eax
    mov eax, 1
    mov edx, [task_scope_test_parent]
    mov ebx, 0x20
    call task_scope_create
    jc .invalid
    mov [task_scope_test_child], eax

    mov eax, [task_scope_test_parent]
    call task_scope_close
    jnc .invalid                      ; aktives Kind verhindert Scope-Ende

    mov eax, [task_scope_test_parent]
    mov edx, 0x43414E43               ; "CANC"
    call task_scope_cancel
    jc .invalid
    mov eax, [task_scope_test_child]
    call task_scope_lookup
    jc .invalid
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_CANCELLING
    jne .invalid
    cmp dword [eax + TASK_SCOPE_CANCEL_REASON], 0x43414E43
    jne .invalid

    mov eax, [task_scope_test_child]
    call task_scope_close
    jc .invalid
    mov eax, [task_scope_test_parent]
    call task_scope_close
    jc .invalid
    cmp dword [task_scope_count], 1
    jne .invalid
    mov eax, [task_scope_kernel_root_id]
    call task_scope_lookup
    jc .invalid
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_OPEN
    jne .invalid
    cmp dword [eax + TASK_SCOPE_CHILDREN], 0
    jne .invalid
    cmp dword [eax + TASK_SCOPE_ACTIVE_TASKS], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
task_scope_manager_api:
    dd TASK_SCOPE_API_SIZE
    dw 1, 0
    dd TASK_SCOPE_CAPACITY
    dd TASK_SCOPE_CAP_HIERARCHY | TASK_SCOPE_CAP_CANCELLATION | TASK_SCOPE_CAP_BOUNDED
    dd task_scope_create
    dd task_scope_cancel
    dd task_scope_close
    dd task_scope_table

task_scope_count:              dd 0
task_scope_next_id:            dd 0
task_scope_kernel_root_id:     dd 0
task_scope_temp_owner:         dd 0
task_scope_temp_parent:        dd 0
task_scope_temp_flags:         dd 0
task_scope_temp_cancel_id:     dd 0
task_scope_temp_cancel_reason: dd 0
task_scope_temp_close_id:      dd 0
task_scope_test_parent:        dd 0
task_scope_test_child:         dd 0
align 4
task_scope_table:
    times TASK_SCOPE_CAPACITY * TASK_SCOPE_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Verwaltete Kernel-Tasks innerhalb strukturierter Scopes
; NPSPEC-CONCURRENCY-TASK/CANCELLATION-0001
; ---------------------------------------------------------------------------

TASK_API_SIZE             equ 32
TASK_CAPACITY             equ 8
TASK_RECORD_SIZE          equ 32
TASK_STATE_EMPTY          equ 0
TASK_STATE_CREATED        equ 1
TASK_STATE_READY          equ 2
TASK_STATE_RUNNING        equ 3
TASK_STATE_WAITING        equ 4
TASK_STATE_CANCEL_REQUEST equ 5
TASK_STATE_COMPLETED      equ 6
TASK_STATE_CANCELLED      equ 7
TASK_STATE_FAILED         equ 8
TASK_CAP_HIERARCHY        equ 0x00000001
TASK_CAP_CANCEL           equ 0x00000002
TASK_CAP_RESULT           equ 0x00000004
TASK_ID                   equ 0
TASK_OWNER                equ 4
TASK_RECORD_SCOPE         equ 8
TASK_STATE                equ 12
TASK_PARENT               equ 16
TASK_FLAGS                equ 20
TASK_RESULT               equ 24
TASK_CANCEL_REASON        equ 28

task_manager_initialize:
    mov edi, task_table
    xor eax, eax
    mov ecx, (TASK_CAPACITY * TASK_RECORD_SIZE) / 4
    rep stosd
    mov dword [task_count], 0
    mov dword [task_next_id], 1
    mov dword [task_manager_ready], 1
    clc
    ret

; EAX=Owner-PID, EDX=Scope-ID, EBX=Parent-Task oder 0, ECX=Flags.
; Rueckgabe EAX=Task-ID.
task_create:
    pushfd
    cli
    mov [task_temp_owner], eax
    mov [task_temp_scope], edx
    mov [task_temp_parent], ebx
    mov [task_temp_flags], ecx
    call process_lookup
    jc .invalid

    mov eax, [task_temp_scope]
    call task_scope_lookup
    jc .invalid
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_OPEN
    jne .invalid
    mov edx, [task_temp_owner]
    cmp [eax + TASK_SCOPE_OWNER], edx
    jne .invalid

    cmp dword [task_temp_parent], 0
    je .find_slot
    mov eax, [task_temp_parent]
    call task_lookup
    jc .invalid
    mov edx, [task_temp_owner]
    cmp [eax + TASK_OWNER], edx
    jne .invalid
    mov edx, [task_temp_scope]
    cmp [eax + TASK_RECORD_SCOPE], edx
    jne .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jae .invalid

.find_slot:
    xor ecx, ecx
.scan:
    cmp ecx, TASK_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 5
    add edi, task_table
    cmp dword [edi + TASK_STATE], TASK_STATE_EMPTY
    je .slot
    cmp dword [edi + TASK_STATE], TASK_STATE_COMPLETED
    jae .slot
    inc ecx
    jmp .scan
.slot:
    mov edx, edi
    sub edx, task_table
    shr edx, 5
    mov dword [task_group_ids + edx * 4], 0
    shl edx, 4
    add edx, task_deadline_table
    mov dword [edx + 0], 0
    mov dword [edx + 4], 0
    mov dword [edx + 8], 0
    mov dword [edx + 12], 0
    mov eax, [task_next_id]
    mov [edi + TASK_ID], eax
    mov edx, [task_temp_owner]
    mov [edi + TASK_OWNER], edx
    mov edx, [task_temp_scope]
    mov [edi + TASK_RECORD_SCOPE], edx
    mov dword [edi + TASK_STATE], TASK_STATE_READY
    mov edx, [task_temp_parent]
    mov [edi + TASK_PARENT], edx
    mov edx, [task_temp_flags]
    mov [edi + TASK_FLAGS], edx
    mov dword [edi + TASK_RESULT], 0
    mov dword [edi + TASK_CANCEL_REASON], 0

    mov eax, [task_temp_scope]
    call task_scope_lookup
    jc .invalid
    inc dword [eax + TASK_SCOPE_ACTIVE_TASKS]
    inc dword [task_count]
    inc dword [task_next_id]
    mov eax, [edi + TASK_ID]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Task-ID. EAX=Datensatz oder 0.
task_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, TASK_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 5
    add edx, task_table
    cmp dword [edx + TASK_STATE], TASK_STATE_EMPTY
    je .next
    cmp [edx + TASK_ID], eax
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

; EAX=Task-Datensatz. Entfernt genau eine aktive Task aus ihrem Scope.
task_release_scope:
    push edi
    mov edi, eax
    mov eax, [edi + TASK_RECORD_SCOPE]
    call task_scope_lookup
    jc .invalid
    cmp dword [eax + TASK_SCOPE_ACTIVE_TASKS], 0
    je .invalid
    dec dword [eax + TASK_SCOPE_ACTIVE_TASKS]
    dec dword [task_count]
    pop edi
    clc
    ret
.invalid:
    pop edi
    stc
    ret

; EAX=Task-ID, EDX=Grund. Fordert kooperativen Abbruch an und propagiert
; entlang der Task-Hierarchie, beendet aber keinen Task hart.
task_request_cancel:
    pushfd
    cli
    mov [task_temp_cancel_id], eax
    mov [task_temp_cancel_reason], edx
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CREATED
    jb .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    ja .invalid
    mov dword [eax + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    mov edx, [task_temp_cancel_reason]
    mov [eax + TASK_CANCEL_REASON], edx

.propagate:
    xor ebp, ebp
    xor ecx, ecx
.scan_children:
    cmp ecx, TASK_CAPACITY
    jae .propagated
    mov edi, ecx
    shl edi, 5
    add edi, task_table
    cmp dword [edi + TASK_STATE], TASK_STATE_CREATED
    jb .next_child
    cmp dword [edi + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jae .next_child
    mov eax, [edi + TASK_PARENT]
    test eax, eax
    jz .next_child
    push ecx
    push edi
    call task_lookup
    pop edi
    pop ecx
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jne .next_child
    mov dword [edi + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    mov eax, [eax + TASK_CANCEL_REASON]
    mov [edi + TASK_CANCEL_REASON], eax
    mov ebp, 1
.next_child:
    inc ecx
    jmp .scan_children
.propagated:
    test ebp, ebp
    jnz .propagate
    call io_cancel_for_requested_tasks
    popfd
    clc
    ret
.invalid:
    popfd
    stc
    ret

; EDX=Grund. Wird nach einer Scope-Cancellation aufgerufen und markiert alle
; aktiven Tasks in betroffenen Scopes zur kooperativen Beendigung.
task_manager_cancel_scopes:
    cmp dword [task_manager_ready], 1
    jne .done
    mov [task_temp_cancel_reason], edx
    xor ecx, ecx
.scan:
    cmp ecx, TASK_CAPACITY
    jae .done
    mov edi, ecx
    shl edi, 5
    add edi, task_table
    cmp dword [edi + TASK_STATE], TASK_STATE_CREATED
    jb .next
    cmp dword [edi + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jae .next
    mov eax, [edi + TASK_RECORD_SCOPE]
    push ecx
    push edi
    call task_scope_lookup
    pop edi
    pop ecx
    jc .next
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_CANCELLING
    jne .next
    mov dword [edi + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    mov eax, [task_temp_cancel_reason]
    mov [edi + TASK_CANCEL_REASON], eax
.next:
    inc ecx
    jmp .scan
.done:
    clc
    ret

; Kooperativer Cancellation Point. EAX=Task-ID, Rueckgabe EAX=1 bei Cancel.
task_checkpoint:
    pushfd
    cli
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jne .active
    mov [task_terminal_record], eax
    mov dword [eax + TASK_STATE], TASK_STATE_CANCELLED
    call task_group_on_terminal
    mov eax, [task_terminal_record]
    call task_release_scope
    jc .invalid
    mov eax, 1
    popfd
    clc
    ret
.active:
    xor eax, eax
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Task-ID, EDX=Resultat. Nur aktive Tasks duerfen abschliessen.
task_complete:
    pushfd
    cli
    mov [task_temp_complete_id], eax
    mov [task_temp_result], edx
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CREATED
    jb .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_WAITING
    ja .invalid
    mov [task_terminal_record], eax
    mov dword [eax + TASK_STATE], TASK_STATE_COMPLETED
    mov edx, [task_temp_result]
    mov [eax + TASK_RESULT], edx
    call task_group_on_terminal
    mov eax, [task_terminal_record]
    call task_release_scope
    jc .invalid
    mov eax, [task_temp_complete_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Task-ID, EDX=Fehlercode. Kontrollierter terminaler Fehlerpfad.
task_fail:
    pushfd
    cli
    mov [task_temp_complete_id], eax
    mov [task_temp_result], edx
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CREATED
    jb .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_WAITING
    ja .invalid
    mov [task_terminal_record], eax
    mov dword [eax + TASK_STATE], TASK_STATE_FAILED
    mov edx, [task_temp_result]
    mov [eax + TASK_RESULT], edx
    call task_group_on_terminal
    mov eax, [task_terminal_record]
    call task_release_scope
    jc .invalid
    mov eax, [task_temp_complete_id]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

task_manager_self_test:
    ; Hierarchische Cancellation bleibt bis zu expliziten Checkpoints pending.
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [task_test_scope], eax
    mov eax, 1
    mov edx, [task_test_scope]
    xor ebx, ebx
    mov ecx, 1
    call task_create
    jc .invalid
    mov [task_test_parent], eax
    mov eax, 1
    mov edx, [task_test_scope]
    mov ebx, [task_test_parent]
    mov ecx, 2
    call task_create
    jc .invalid
    mov [task_test_child], eax

    mov eax, [task_test_scope]
    call task_scope_close
    jnc .invalid
    mov eax, [task_test_parent]
    mov edx, 0x43414E43
    call task_request_cancel
    jc .invalid
    mov eax, [task_test_child]
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jne .invalid
    cmp dword [eax + TASK_CANCEL_REASON], 0x43414E43
    jne .invalid
    mov eax, [task_test_child]
    call task_checkpoint
    jc .invalid
    cmp eax, 1
    jne .invalid
    mov eax, [task_test_parent]
    call task_checkpoint
    jc .invalid
    cmp eax, 1
    jne .invalid
    mov eax, [task_test_scope]
    call task_scope_close
    jc .invalid

    ; Normaler Completion-Pfad bewahrt das Ergebnis und loest den Scopezaehler.
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [task_test_scope], eax
    mov eax, 1
    mov edx, [task_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [task_test_parent], eax
    mov edx, 0x4F4B0001
    call task_complete
    jc .invalid
    mov eax, [task_test_parent]
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_COMPLETED
    jne .invalid
    cmp dword [eax + TASK_RESULT], 0x4F4B0001
    jne .invalid
    mov eax, [task_test_scope]
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
task_manager_api:
    dd TASK_API_SIZE
    dw 1, 0
    dd TASK_CAPACITY
    dd TASK_CAP_HIERARCHY | TASK_CAP_CANCEL | TASK_CAP_RESULT
    dd task_create
    dd task_request_cancel
    dd task_checkpoint
    dd task_complete

task_manager_ready:       dd 0
task_count:               dd 0
task_next_id:             dd 0
task_temp_owner:          dd 0
task_temp_scope:          dd 0
task_temp_parent:         dd 0
task_temp_flags:          dd 0
task_temp_cancel_id:      dd 0
task_temp_cancel_reason:  dd 0
task_temp_complete_id:    dd 0
task_temp_result:         dd 0
task_terminal_record:     dd 0
task_test_scope:          dd 0
task_test_parent:         dd 0
task_test_child:          dd 0
align 4
task_table:
    times TASK_CAPACITY * TASK_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Hierarchische Task-Deadlines auf monotoner PIT-Zeitbasis
; NPSPEC-CONCURRENCY-DEADLINE-0001 / ADR-CONCURRENCY-0004
; ---------------------------------------------------------------------------

TASK_DEADLINE_API_SIZE       equ 32
TASK_DEADLINE_CLOCK_HZ       equ 100
TASK_DEADLINE_CLASS_HARD     equ 1
TASK_DEADLINE_CLASS_FIRM     equ 2
TASK_DEADLINE_CLASS_SOFT     equ 3
TASK_DEADLINE_POLICY_CONTINUE equ 1
TASK_DEADLINE_POLICY_CANCEL  equ 2
TASK_DEADLINE_POLICY_FAIL    equ 3
TASK_DEADLINE_STATE_NONE     equ 0
TASK_DEADLINE_STATE_ARMED    equ 1
TASK_DEADLINE_STATE_MISSED   equ 2
TASK_DEADLINE_ABSOLUTE       equ 0
TASK_DEADLINE_CLASS          equ 4
TASK_DEADLINE_POLICY         equ 8
TASK_DEADLINE_STATE          equ 12
TASK_DEADLINE_CAPABILITIES   equ 0x0000000F
TASK_CANCEL_REASON_DEADLINE  equ 0x444C4E45

task_deadline_manager_initialize:
    mov edi, task_deadline_table
    xor eax, eax
    mov ecx, (TASK_CAPACITY * 16) / 4
    rep stosd
    mov dword [task_deadline_miss_count], 0
    mov dword [task_deadline_manager_ready], 1
    clc
    ret

; EAX=Taskdatensatz. EAX=zugehoeriger Deadline-Datensatz.
task_deadline_record_for_task:
    sub eax, task_table
    shr eax, 1                       ; 32 Byte Task -> 16 Byte Deadline
    add eax, task_deadline_table
    ret

; EAX=Task-ID, EDX=absoluter Tick, EBX=Klasse, ECX=Miss-Policy.
; Eine Parent-Deadline wird niemals verlaengert.
task_deadline_set:
    pushfd
    cli
    mov [task_deadline_temp_task], eax
    mov [task_deadline_temp_tick], edx
    mov [task_deadline_temp_class], ebx
    mov [task_deadline_temp_policy], ecx
    cmp ebx, TASK_DEADLINE_CLASS_HARD
    jb .invalid
    cmp ebx, TASK_DEADLINE_CLASS_SOFT
    ja .invalid
    cmp ecx, TASK_DEADLINE_POLICY_CONTINUE
    jb .invalid
    cmp ecx, TASK_DEADLINE_POLICY_FAIL
    ja .invalid
    test edx, edx
    jz .invalid

    call task_lookup
    jc .invalid
    mov [task_deadline_temp_record], eax
    cmp dword [eax + TASK_STATE], TASK_STATE_CREATED
    jb .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_WAITING
    ja .invalid

    mov eax, [eax + TASK_PARENT]
    test eax, eax
    jz .validate_effective
    call task_lookup
    jc .invalid
    call task_deadline_record_for_task
    cmp dword [eax + TASK_DEADLINE_STATE], TASK_DEADLINE_STATE_ARMED
    jne .validate_effective
    mov edx, [task_deadline_temp_tick]
    sub edx, [eax + TASK_DEADLINE_ABSOLUTE]
    jle .inherit_class
    mov edx, [eax + TASK_DEADLINE_ABSOLUTE]
    mov [task_deadline_temp_tick], edx
.inherit_class:
    cmp dword [eax + TASK_DEADLINE_CLASS], TASK_DEADLINE_CLASS_HARD
    jne .validate_effective
    mov dword [task_deadline_temp_class], TASK_DEADLINE_CLASS_HARD

.validate_effective:
    cmp dword [task_deadline_temp_class], TASK_DEADLINE_CLASS_HARD
    jne .store
    cmp dword [task_deadline_temp_policy], TASK_DEADLINE_POLICY_CONTINUE
    je .invalid                       ; Hard Miss darf nicht still ignoriert werden
    mov eax, [task_deadline_temp_tick]
    sub eax, [timer_ticks]
    jle .invalid                       ; minimale Bootstrap-Admission-Control

.store:
    mov eax, [task_deadline_temp_record]
    call task_deadline_record_for_task
    mov edx, [task_deadline_temp_tick]
    mov [eax + TASK_DEADLINE_ABSOLUTE], edx
    mov edx, [task_deadline_temp_class]
    mov [eax + TASK_DEADLINE_CLASS], edx
    mov edx, [task_deadline_temp_policy]
    mov [eax + TASK_DEADLINE_POLICY], edx
    mov dword [eax + TASK_DEADLINE_STATE], TASK_DEADLINE_STATE_ARMED
    mov eax, [task_deadline_temp_tick]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; Wird aus IRQ0 und Tests aufgerufen. Deadline Miss und Cancellation bleiben
; getrennte Zustandsinformationen.
task_deadline_poll:
    cmp dword [task_deadline_manager_ready], 1
    jne .done
    mov eax, [timer_ticks]
    mov [task_deadline_poll_tick], eax
    xor ecx, ecx
.scan:
    cmp ecx, TASK_CAPACITY
    jae .done
    mov edi, ecx
    shl edi, 5
    add edi, task_table
    cmp dword [edi + TASK_STATE], TASK_STATE_CREATED
    jb .next
    cmp dword [edi + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jae .next
    mov eax, ecx
    shl eax, 4
    add eax, task_deadline_table
    cmp dword [eax + TASK_DEADLINE_STATE], TASK_DEADLINE_STATE_ARMED
    jne .next
    mov edx, [task_deadline_poll_tick]
    sub edx, [eax + TASK_DEADLINE_ABSOLUTE]
    jl .next
    mov dword [eax + TASK_DEADLINE_STATE], TASK_DEADLINE_STATE_MISSED
    inc dword [task_deadline_miss_count]
    mov edx, [eax + TASK_DEADLINE_POLICY]
    cmp edx, TASK_DEADLINE_POLICY_CANCEL
    je .cancel
    cmp edx, TASK_DEADLINE_POLICY_FAIL
    je .fail
    jmp .next
.cancel:
    mov eax, [edi + TASK_ID]
    mov edx, TASK_CANCEL_REASON_DEADLINE
    push ecx
    call task_request_cancel
    pop ecx
    jmp .next
.fail:
    mov dword [edi + TASK_STATE], TASK_STATE_FAILED
    mov dword [edi + TASK_RESULT], TASK_CANCEL_REASON_DEADLINE
    push edi
    mov eax, edi
    call task_group_on_terminal
    pop edi
    mov eax, edi
    push ecx
    call task_release_scope
    pop ecx
.next:
    inc ecx
    jmp .scan
.done:
    clc
    ret

task_deadline_manager_self_test:
    ; Parent 100 Ticks, Child fordert 200: effektiv bleiben 100 und Hard.
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [task_deadline_test_scope], eax
    mov eax, 1
    mov edx, [task_deadline_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [task_deadline_test_parent], eax
    mov edx, [timer_ticks]
    add edx, 100
    mov [task_deadline_test_parent_tick], edx
    mov ebx, TASK_DEADLINE_CLASS_HARD
    mov ecx, TASK_DEADLINE_POLICY_CANCEL
    call task_deadline_set
    jc .invalid

    mov eax, 1
    mov edx, [task_deadline_test_scope]
    mov ebx, [task_deadline_test_parent]
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [task_deadline_test_child], eax
    mov edx, [timer_ticks]
    add edx, 200
    mov ebx, TASK_DEADLINE_CLASS_SOFT
    mov ecx, TASK_DEADLINE_POLICY_CANCEL
    call task_deadline_set
    jc .invalid
    mov eax, [task_deadline_test_child]
    call task_lookup
    jc .invalid
    call task_deadline_record_for_task
    mov edx, [task_deadline_test_parent_tick]
    cmp [eax + TASK_DEADLINE_ABSOLUTE], edx
    jne .invalid
    cmp dword [eax + TASK_DEADLINE_CLASS], TASK_DEADLINE_CLASS_HARD
    jne .invalid
    mov eax, [task_deadline_test_child]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [task_deadline_test_parent]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [task_deadline_test_scope]
    call task_scope_close
    jc .invalid

    ; Eine bereits erreichte Firm-Deadline fordert kooperative Cancellation an.
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [task_deadline_test_scope], eax
    mov eax, 1
    mov edx, [task_deadline_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [task_deadline_test_parent], eax
    mov edx, [timer_ticks]
    mov ebx, TASK_DEADLINE_CLASS_FIRM
    mov ecx, TASK_DEADLINE_POLICY_CANCEL
    call task_deadline_set
    jc .invalid
    call task_deadline_poll
    mov eax, [task_deadline_test_parent]
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jne .invalid
    cmp dword [eax + TASK_CANCEL_REASON], TASK_CANCEL_REASON_DEADLINE
    jne .invalid
    call task_deadline_record_for_task
    cmp dword [eax + TASK_DEADLINE_STATE], TASK_DEADLINE_STATE_MISSED
    jne .invalid
    cmp dword [task_deadline_miss_count], 1
    jne .invalid
    mov eax, [task_deadline_test_parent]
    call task_checkpoint
    jc .invalid
    mov eax, [task_deadline_test_scope]
    call task_scope_close
    jc .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
task_deadline_manager_api:
    dd TASK_DEADLINE_API_SIZE
    dw 1, 0
    dd TASK_DEADLINE_CLOCK_HZ
    dd TASK_DEADLINE_CAPABILITIES
    dd task_deadline_set
    dd task_deadline_poll
    dd task_deadline_table
    dd task_deadline_miss_count

task_deadline_manager_ready:    dd 0
task_deadline_miss_count:       dd 0
task_deadline_poll_tick:        dd 0
task_deadline_temp_task:        dd 0
task_deadline_temp_tick:        dd 0
task_deadline_temp_class:       dd 0
task_deadline_temp_policy:      dd 0
task_deadline_temp_record:      dd 0
task_deadline_test_scope:       dd 0
task_deadline_test_parent:      dd 0
task_deadline_test_child:       dd 0
task_deadline_test_parent_tick: dd 0
align 4
task_deadline_table:
    times TASK_CAPACITY * 16 db 0

; ---------------------------------------------------------------------------
; Task Groups mit WaitAll, FailFast, Cancellation und Drain
; NPSPEC-CONCURRENCY-TASKGROUP-0001
; ---------------------------------------------------------------------------

TASK_GROUP_API_SIZE          equ 32
TASK_GROUP_CAPACITY          equ 4
TASK_GROUP_RECORD_SIZE       equ 32
TASK_GROUP_STATE_EMPTY       equ 0
TASK_GROUP_STATE_OPEN        equ 1
TASK_GROUP_STATE_CANCELLING  equ 2
TASK_GROUP_STATE_FAILING     equ 3
TASK_GROUP_STATE_COMPLETED   equ 4
TASK_GROUP_STATE_CANCELLED   equ 5
TASK_GROUP_STATE_FAILED      equ 6
TASK_GROUP_POLICY_WAIT_ALL   equ 1
TASK_GROUP_POLICY_FAIL_FAST  equ 2
TASK_GROUP_CAPABILITIES      equ 0x0000000F
TASK_GROUP_ID                equ 0
TASK_GROUP_OWNER             equ 4
TASK_GROUP_SCOPE             equ 8
TASK_GROUP_STATE             equ 12
TASK_GROUP_POLICY            equ 16
TASK_GROUP_REQUIRED          equ 20
TASK_GROUP_ACTIVE            equ 24
TASK_GROUP_FAILURES          equ 28
TASK_CANCEL_REASON_GROUP     equ 0x47525043

task_group_manager_initialize:
    mov edi, task_group_table
    xor eax, eax
    mov ecx, (TASK_GROUP_CAPACITY * TASK_GROUP_RECORD_SIZE) / 4
    rep stosd
    mov edi, task_group_ids
    mov ecx, TASK_CAPACITY
    rep stosd
    mov dword [task_group_count], 0
    mov dword [task_group_next_id], 1
    mov dword [task_group_manager_ready], 1
    clc
    ret

; EAX=Owner-PID, EDX=Scope-ID, EBX=Policy. EAX=Group-ID.
task_group_create:
    pushfd
    cli
    mov [task_group_temp_owner], eax
    mov [task_group_temp_scope], edx
    mov [task_group_temp_policy], ebx
    cmp ebx, TASK_GROUP_POLICY_WAIT_ALL
    jb .invalid
    cmp ebx, TASK_GROUP_POLICY_FAIL_FAST
    ja .invalid
    call process_lookup
    jc .invalid
    mov eax, [task_group_temp_scope]
    call task_scope_lookup
    jc .invalid
    cmp dword [eax + TASK_SCOPE_STATE], TASK_SCOPE_STATE_OPEN
    jne .invalid
    mov edx, [task_group_temp_owner]
    cmp [eax + TASK_SCOPE_OWNER], edx
    jne .invalid
    xor ecx, ecx
.scan:
    cmp ecx, TASK_GROUP_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 5
    add edi, task_group_table
    cmp dword [edi + TASK_GROUP_STATE], TASK_GROUP_STATE_EMPTY
    je .slot
    cmp dword [edi + TASK_GROUP_STATE], TASK_GROUP_STATE_COMPLETED
    jae .slot
    inc ecx
    jmp .scan
.slot:
    mov eax, [task_group_next_id]
    mov [edi + TASK_GROUP_ID], eax
    mov edx, [task_group_temp_owner]
    mov [edi + TASK_GROUP_OWNER], edx
    mov edx, [task_group_temp_scope]
    mov [edi + TASK_GROUP_SCOPE], edx
    mov dword [edi + TASK_GROUP_STATE], TASK_GROUP_STATE_OPEN
    mov edx, [task_group_temp_policy]
    mov [edi + TASK_GROUP_POLICY], edx
    mov dword [edi + TASK_GROUP_REQUIRED], 0
    mov dword [edi + TASK_GROUP_ACTIVE], 0
    mov dword [edi + TASK_GROUP_FAILURES], 0
    inc dword [task_group_count]
    inc dword [task_group_next_id]
    mov eax, [edi + TASK_GROUP_ID]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Group-ID. EAX=Datensatz oder 0.
task_group_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, TASK_GROUP_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 5
    add edx, task_group_table
    cmp dword [edx + TASK_GROUP_STATE], TASK_GROUP_STATE_EMPTY
    je .next
    cmp [edx + TASK_GROUP_ID], eax
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

; EAX=Group-ID, EDX=Task-ID.
task_group_attach:
    pushfd
    cli
    mov [task_group_temp_id], eax
    mov [task_group_temp_task], edx
    call task_group_lookup
    jc .invalid
    cmp dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_OPEN
    jne .invalid
    mov [task_group_temp_record], eax
    mov eax, [task_group_temp_task]
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CREATED
    jb .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_WAITING
    ja .invalid
    mov edx, [task_group_temp_record]
    mov ecx, [edx + TASK_GROUP_OWNER]
    cmp [eax + TASK_OWNER], ecx
    jne .invalid
    mov ecx, [edx + TASK_GROUP_SCOPE]
    cmp [eax + TASK_RECORD_SCOPE], ecx
    jne .invalid
    mov ecx, eax
    sub ecx, task_table
    shr ecx, 5
    cmp dword [task_group_ids + ecx * 4], 0
    jne .invalid
    mov eax, [task_group_temp_id]
    mov [task_group_ids + ecx * 4], eax
    inc dword [edx + TASK_GROUP_REQUIRED]
    inc dword [edx + TASK_GROUP_ACTIVE]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Group-ID, EDX=Grund. Markiert aktive Mitglieder kooperativ.
task_group_cancel_remaining:
    mov [task_group_temp_id], eax
    mov [task_group_temp_reason], edx
    xor ecx, ecx
.scan:
    cmp ecx, TASK_CAPACITY
    jae .done
    mov eax, [task_group_ids + ecx * 4]
    cmp eax, [task_group_temp_id]
    jne .next
    mov edi, ecx
    shl edi, 5
    add edi, task_table
    cmp dword [edi + TASK_STATE], TASK_STATE_CREATED
    jb .next
    cmp dword [edi + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jae .next
    mov eax, [edi + TASK_ID]
    mov edx, [task_group_temp_reason]
    push ecx
    call task_request_cancel
    pop ecx
.next:
    inc ecx
    jmp .scan
.done:
    clc
    ret

; EAX=Group-ID, EDX=Grund.
task_group_cancel:
    pushfd
    cli
    mov [task_group_temp_id], eax
    mov [task_group_temp_reason], edx
    call task_group_lookup
    jc .invalid
    cmp dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_OPEN
    jne .invalid
    mov dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_CANCELLING
    mov eax, [task_group_temp_id]
    mov edx, [task_group_temp_reason]
    call task_group_cancel_remaining
    popfd
    clc
    ret
.invalid:
    popfd
    stc
    ret

; EAX=terminaler Taskdatensatz. Aktualisiert Group-Policy und Drain-Zustand.
task_group_on_terminal:
    cmp dword [task_group_manager_ready], 1
    jne .done
    mov [task_group_terminal_task], eax
    mov ecx, eax
    sub ecx, task_table
    shr ecx, 5
    mov eax, [task_group_ids + ecx * 4]
    test eax, eax
    jz .done
    mov [task_group_temp_id], eax
    call task_group_lookup
    jc .invalid
    mov [task_group_temp_record], eax
    cmp dword [eax + TASK_GROUP_ACTIVE], 0
    je .invalid
    dec dword [eax + TASK_GROUP_ACTIVE]

    mov edi, [task_group_terminal_task]
    cmp dword [edi + TASK_STATE], TASK_STATE_FAILED
    jne .check_cancelled
    inc dword [eax + TASK_GROUP_FAILURES]
    cmp dword [eax + TASK_GROUP_POLICY], TASK_GROUP_POLICY_FAIL_FAST
    jne .finalize
    mov dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_FAILING
    mov eax, [task_group_temp_id]
    mov edx, [edi + TASK_RESULT]
    call task_group_cancel_remaining
    jmp .finalize_reload
.check_cancelled:
    cmp dword [edi + TASK_STATE], TASK_STATE_CANCELLED
    jne .finalize
    cmp dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_OPEN
    jne .finalize
    mov dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_CANCELLING
.finalize_reload:
    mov eax, [task_group_temp_id]
    call task_group_lookup
    jc .invalid
.finalize:
    cmp dword [eax + TASK_GROUP_ACTIVE], 0
    jne .done
    cmp dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_CANCELLING
    je .cancelled
    cmp dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_FAILING
    je .failed
    cmp dword [eax + TASK_GROUP_FAILURES], 0
    jne .failed
    mov dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_COMPLETED
    jmp .terminal
.cancelled:
    mov dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_CANCELLED
    jmp .terminal
.failed:
    mov dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_FAILED
.terminal:
    dec dword [task_group_count]
.done:
    clc
    ret
.invalid:
    stc
    ret

task_group_manager_self_test:
    ; WaitAll wird erst nach beiden erfolgreichen Tasks terminal.
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [task_group_test_scope], eax
    mov eax, 1
    mov edx, [task_group_test_scope]
    mov ebx, TASK_GROUP_POLICY_WAIT_ALL
    call task_group_create
    jc .invalid
    mov [task_group_test_group], eax
    mov eax, 1
    mov edx, [task_group_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [task_group_test_task1], eax
    mov edx, eax
    mov eax, [task_group_test_group]
    call task_group_attach
    jc .invalid
    mov eax, 1
    mov edx, [task_group_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [task_group_test_task2], eax
    mov edx, eax
    mov eax, [task_group_test_group]
    call task_group_attach
    jc .invalid
    mov eax, [task_group_test_task1]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [task_group_test_group]
    call task_group_lookup
    jc .invalid
    cmp dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_OPEN
    jne .invalid
    cmp dword [eax + TASK_GROUP_ACTIVE], 1
    jne .invalid
    mov eax, [task_group_test_task2]
    xor edx, edx
    call task_complete
    jc .invalid
    mov eax, [task_group_test_group]
    call task_group_lookup
    jc .invalid
    cmp dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_COMPLETED
    jne .invalid
    mov eax, [task_group_test_scope]
    call task_scope_close
    jc .invalid

    ; FailFast fordert fuer verbleibende Tasks Cancellation an und drainiert.
    mov eax, 1
    mov edx, [task_scope_kernel_root_id]
    xor ebx, ebx
    call task_scope_create
    jc .invalid
    mov [task_group_test_scope], eax
    mov eax, 1
    mov edx, [task_group_test_scope]
    mov ebx, TASK_GROUP_POLICY_FAIL_FAST
    call task_group_create
    jc .invalid
    mov [task_group_test_group], eax
    mov eax, 1
    mov edx, [task_group_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [task_group_test_task1], eax
    mov edx, eax
    mov eax, [task_group_test_group]
    call task_group_attach
    jc .invalid
    mov eax, 1
    mov edx, [task_group_test_scope]
    xor ebx, ebx
    xor ecx, ecx
    call task_create
    jc .invalid
    mov [task_group_test_task2], eax
    mov edx, eax
    mov eax, [task_group_test_group]
    call task_group_attach
    jc .invalid
    mov eax, [task_group_test_task1]
    mov edx, 0x4641494C
    call task_fail
    jc .invalid
    mov eax, [task_group_test_task2]
    call task_lookup
    jc .invalid
    cmp dword [eax + TASK_STATE], TASK_STATE_CANCEL_REQUEST
    jne .invalid
    mov eax, [task_group_test_task2]
    call task_checkpoint
    jc .invalid
    mov eax, [task_group_test_group]
    call task_group_lookup
    jc .invalid
    cmp dword [eax + TASK_GROUP_STATE], TASK_GROUP_STATE_FAILED
    jne .invalid
    cmp dword [eax + TASK_GROUP_FAILURES], 1
    jne .invalid
    mov eax, [task_group_test_scope]
    call task_scope_close
    jc .invalid
    cmp dword [task_group_count], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
task_group_manager_api:
    dd TASK_GROUP_API_SIZE
    dw 1, 0
    dd TASK_GROUP_CAPACITY
    dd TASK_GROUP_CAPABILITIES
    dd task_group_create
    dd task_group_attach
    dd task_group_cancel
    dd task_group_table

task_group_manager_ready: dd 0
task_group_count:         dd 0
task_group_next_id:       dd 0
task_group_temp_owner:    dd 0
task_group_temp_scope:    dd 0
task_group_temp_policy:   dd 0
task_group_temp_id:       dd 0
task_group_temp_task:     dd 0
task_group_temp_reason:   dd 0
task_group_temp_record:   dd 0
task_group_terminal_task: dd 0
task_group_test_scope:    dd 0
task_group_test_group:    dd 0
task_group_test_task1:    dd 0
task_group_test_task2:    dd 0
align 4
task_group_table:
    times TASK_GROUP_CAPACITY * TASK_GROUP_RECORD_SIZE db 0
task_group_ids:
    times TASK_CAPACITY dd 0

