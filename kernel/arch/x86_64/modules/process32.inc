; Kernel Process Manager (ADR-2011 / NPSPEC-KERNEL-0004)
; NPSPEC-PROCESS-LIFECYCLE-0001 / NPSPEC-PROCESS-MODEL-0001 / NPSPEC-PROCESS-SPAWN-0001
; NPSPEC-PROCESS-ISOLATION-0001
PROCESS_API_SIZE      equ 32
PROCESS_CAPACITY      equ 4
PROCESS_RECORD_SIZE   equ 32
PROCESS_STATE_EMPTY   equ 0
PROCESS_STATE_RUNNING equ 1
OBJECT_TYPE_PROCESS   equ 4
PROCESS_PID       equ 0
PROCESS_STATE     equ 4
PROCESS_CR3       equ 8
PROCESS_HANDLE    equ 12
PROCESS_PARENT    equ 16
PROCESS_FLAGS     equ 20

process_manager_initialize:
    mov edi, process_table
    xor eax, eax
    mov ecx, (PROCESS_CAPACITY * PROCESS_RECORD_SIZE) / 4
    rep stosd
    mov dword [process_count], 0
    mov dword [process_next_pid], 1
    clc
    ret

; EAX=Flags, EDX=Page-Directory. EAX=PID.
process_create:
    pushfd
    cli
    mov [process_temp_flags], eax
    mov [process_temp_cr3], edx
    xor ecx, ecx
.scan:
    cmp ecx, PROCESS_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 5
    add edi, process_table
    cmp dword [edi + PROCESS_STATE], PROCESS_STATE_EMPTY
    je .slot
    inc ecx
    jmp .scan
.slot:
    mov [process_temp_slot], edi
    mov edx, [process_next_pid]
    mov [process_temp_pid], edx
    mov eax, OBJECT_TYPE_PROCESS
    xor ebx, ebx
    call object_create
    jc .invalid
    mov edi, [process_temp_slot]
    mov edx, [process_temp_pid]
    mov [edi + PROCESS_PID], edx
    mov dword [edi + PROCESS_STATE], PROCESS_STATE_RUNNING
    mov edx, [process_temp_cr3]
    mov [edi + PROCESS_CR3], edx
    mov [edi + PROCESS_HANDLE], eax
    mov dword [edi + PROCESS_PARENT], 0
    mov edx, [process_temp_flags]
    mov [edi + PROCESS_FLAGS], edx
    inc dword [process_count]
    inc dword [process_next_pid]
    mov eax, [process_temp_pid]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=PID. EAX=Prozessdatensatz oder 0.
process_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, PROCESS_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 5
    add edx, process_table
    cmp dword [edx + PROCESS_STATE], PROCESS_STATE_EMPTY
    je .next
    cmp [edx + PROCESS_PID], eax
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

process_manager_self_test:
    mov eax, 1                      ; Kernel/System-Prozess
    mov edx, cr3
    call process_create
    jc .invalid
    cmp eax, 1
    jne .invalid
    call process_lookup
    jc .invalid
    cmp dword [eax + PROCESS_STATE], PROCESS_STATE_RUNNING
    jne .invalid
    mov edx, cr3
    cmp [eax + PROCESS_CR3], edx
    jne .invalid
    cmp dword [process_count], 1
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
process_manager_api:
    dd PROCESS_API_SIZE
    dw 1, 0
    dd PROCESS_CAPACITY
    dd process_create
    dd process_lookup
    dd process_count
    dd process_table
    dd process_next_pid

process_count:      dd 0
process_next_pid:   dd 0
process_temp_pid:   dd 0
process_temp_cr3:   dd 0
process_temp_flags: dd 0
process_temp_slot:  dd 0
align 4
process_table:
    times PROCESS_CAPACITY * PROCESS_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Strukturierte Nebenlaeufigkeit: Task-Scopes
