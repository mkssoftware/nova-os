; Global-State- und Transaction-Bootstrapkern.
; Dieser Kern implementiert noch keinen universellen Zustandsspeicher. Er legt
; die stabile ABI-Kante fuer versionierte, verantwortete Teilzustaende und
; kontrollierte Transitions an, auf der Services spaeter aufbauen.

STATE_MANAGER_ABI_MAJOR       equ 1
STATE_MANAGER_ABI_MINOR       equ 0
STATE_RECORD_SIZE             equ 64
STATE_RECORD_CAPACITY         equ 8
STATE_VALIDITY_UNKNOWN        equ 0
STATE_VALIDITY_VALID          equ 1
STATE_VALIDITY_INVALID        equ 2
STATE_VALIDITY_STALE          equ 3
STATE_KIND_KERNEL             equ 1
STATE_KIND_SERVICE            equ 5
STATE_BOOT_ID_HIGH            equ 0x4E4F5641
STATE_BOOT_ID_LOW             equ 0x53544154

STATE_ID_HIGH                 equ 0
STATE_ID_LOW                  equ 4
STATE_KIND                    equ 8
STATE_OWNER                   equ 12
STATE_VERSION                 equ 16
STATE_GENERATION              equ 24
STATE_PREVIOUS_VERSION        equ 32
STATE_TRANSACTION             equ 40
STATE_VALIDITY                equ 48
STATE_FLAGS                   equ 52
STATE_VALUE                   equ 56
STATE_RESERVED                equ 60

state_manager_initialize:
    mov edi, state_records
    xor eax, eax
    mov ecx, (STATE_RECORD_CAPACITY * STATE_RECORD_SIZE) / 4
    rep stosd
    mov dword [state_record_count], 0
    mov dword [state_generation_counter], 1
    clc
    ret

; EAX=High-ID, EDX=Low-ID, EBX=Kind, ECX=Owner.
; EAX=Slotindex oder CF=1.
state_create:
    test eax, eax
    jz .bad
    test edx, edx
    jz .bad
    test ebx, ebx
    jz .bad
    test ecx, ecx
    jz .bad
    mov [state_temp_high], eax
    mov [state_temp_low], edx
    mov [state_temp_kind], ebx
    mov [state_temp_owner], ecx
    xor esi, esi
.scan:
    cmp esi, STATE_RECORD_CAPACITY
    jae .bad
    imul edi, esi, STATE_RECORD_SIZE
    add edi, state_records
    cmp dword [edi + STATE_ID_HIGH], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov eax, [state_temp_high]
    mov [edi + STATE_ID_HIGH], eax
    mov eax, [state_temp_low]
    mov [edi + STATE_ID_LOW], eax
    mov eax, [state_temp_kind]
    mov [edi + STATE_KIND], eax
    mov eax, [state_temp_owner]
    mov [edi + STATE_OWNER], eax
    mov dword [edi + STATE_VERSION], 1
    mov dword [edi + STATE_VERSION + 4], 0
    mov eax, [state_generation_counter]
    mov [edi + STATE_GENERATION], eax
    mov dword [edi + STATE_GENERATION + 4], 0
    mov dword [edi + STATE_PREVIOUS_VERSION], 0
    mov dword [edi + STATE_PREVIOUS_VERSION + 4], 0
    mov dword [edi + STATE_TRANSACTION], 0
    mov dword [edi + STATE_TRANSACTION + 4], 0
    mov dword [edi + STATE_VALIDITY], STATE_VALIDITY_UNKNOWN
    mov dword [edi + STATE_FLAGS], 0
    mov dword [edi + STATE_VALUE], 0
    mov dword [edi + STATE_RESERVED], 0
    inc dword [state_record_count]
    mov eax, esi
    clc
    ret
.bad:
    xor eax, eax
    stc
    ret

; EAX=Slot, EDX=erwartete Version low, EBX=neuer Validity-Wert.
state_transition:
    cmp eax, STATE_RECORD_CAPACITY
    jae .bad
    cmp ebx, STATE_VALIDITY_STALE
    ja .bad
    imul edi, eax, STATE_RECORD_SIZE
    add edi, state_records
    cmp dword [edi + STATE_ID_HIGH], 0
    je .bad
    cmp [edi + STATE_VERSION], edx
    jne .stale
    mov ecx, [edi + STATE_VERSION]
    mov [edi + STATE_PREVIOUS_VERSION], ecx
    mov ecx, [edi + STATE_VERSION + 4]
    mov [edi + STATE_PREVIOUS_VERSION + 4], ecx
    inc dword [edi + STATE_VERSION]
    jnz .version_ok
    inc dword [edi + STATE_VERSION + 4]
.version_ok:
    mov [edi + STATE_VALIDITY], ebx
    mov dword [edi + STATE_TRANSACTION], 0
    mov dword [edi + STATE_TRANSACTION + 4], 0
    clc
    ret
.stale:
    mov dword [edi + STATE_VALIDITY], STATE_VALIDITY_STALE
.bad:
    stc
    ret

state_manager_self_test:
    mov eax, STATE_BOOT_ID_HIGH
    mov edx, STATE_BOOT_ID_LOW
    mov ebx, STATE_KIND_KERNEL
    mov ecx, 1
    call state_create
    jc .bad
    mov [state_test_slot], eax
    imul edi, eax, STATE_RECORD_SIZE
    add edi, state_records
    cmp dword [edi + STATE_VALIDITY], STATE_VALIDITY_UNKNOWN
    jne .bad
    mov edx, 1
    mov ebx, STATE_VALIDITY_VALID
    call state_transition
    jc .bad
    mov eax, [state_test_slot]
    imul edi, eax, STATE_RECORD_SIZE
    add edi, state_records
    cmp dword [edi + STATE_VERSION], 2
    jne .bad
    cmp dword [edi + STATE_PREVIOUS_VERSION], 1
    jne .bad
    cmp dword [edi + STATE_VALIDITY], STATE_VALIDITY_VALID
    jne .bad
    mov eax, [state_test_slot]
    mov edx, 1                         ; alter Zustand darf nicht ueberschreiben
    mov ebx, STATE_VALIDITY_INVALID
    call state_transition
    jnc .bad
    mov eax, [state_test_slot]
    imul edi, eax, STATE_RECORD_SIZE
    add edi, state_records
    cmp dword [edi + STATE_VALIDITY], STATE_VALIDITY_STALE
    jne .bad
    clc
    ret
.bad:
    stc
    ret

TRANSACTION_RECORD_SIZE       equ 64
TRANSACTION_CAPACITY          equ 4
TRANSACTION_CREATED           equ 1
TRANSACTION_ACTIVE            equ 2
TRANSACTION_PREPARED          equ 5
TRANSACTION_COMMITTED         equ 7
TRANSACTION_VERIFYING         equ 8
TRANSACTION_COMPLETED         equ 9
TRANSACTION_FAILED            equ 12
TRANSACTION_SCOPE_OBJECT      equ 1
TRANSACTION_SCOPE_SERVICE     equ 6

TXN_ID_LOW                    equ 0
TXN_ID_HIGH                   equ 4
TXN_OWNER                     equ 8
TXN_SCOPE                     equ 12
TXN_STATE                     equ 16
TXN_OPERATIONS                equ 20
TXN_EXPECTED_VERSION          equ 24
TXN_COMMIT_GENERATION         equ 32
TXN_CONFLICT_STATE            equ 40
TXN_FLAGS                     equ 48
TXN_RESERVED                  equ 52

transaction_manager_initialize:
    mov edi, transaction_records
    xor eax, eax
    mov ecx, (TRANSACTION_CAPACITY * TRANSACTION_RECORD_SIZE) / 4
    rep stosd
    mov dword [transaction_next_id], 1
    mov dword [transaction_active_count], 0
    clc
    ret

; EAX=Owner, EBX=Scope, ECX=ExpectedVersion. EAX=Transaction-ID.
transaction_begin:
    test eax, eax
    jz .bad
    test ebx, ebx
    jz .bad
    mov [transaction_temp_owner], eax
    mov [transaction_temp_scope], ebx
    mov [transaction_temp_expected], ecx
    xor esi, esi
.scan:
    cmp esi, TRANSACTION_CAPACITY
    jae .bad
    imul edi, esi, TRANSACTION_RECORD_SIZE
    add edi, transaction_records
    cmp dword [edi + TXN_STATE], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov eax, [transaction_next_id]
    inc dword [transaction_next_id]
    mov [edi + TXN_ID_LOW], eax
    mov dword [edi + TXN_ID_HIGH], 0
    mov eax, [transaction_temp_owner]
    mov [edi + TXN_OWNER], eax
    mov eax, [transaction_temp_scope]
    mov [edi + TXN_SCOPE], eax
    mov dword [edi + TXN_STATE], TRANSACTION_ACTIVE
    mov dword [edi + TXN_OPERATIONS], 0
    mov eax, [transaction_temp_expected]
    mov [edi + TXN_EXPECTED_VERSION], eax
    mov dword [edi + TXN_EXPECTED_VERSION + 4], 0
    mov dword [edi + TXN_COMMIT_GENERATION], 0
    mov dword [edi + TXN_COMMIT_GENERATION + 4], 0
    mov dword [edi + TXN_CONFLICT_STATE], 0
    mov dword [edi + TXN_CONFLICT_STATE + 4], 0
    mov dword [edi + TXN_FLAGS], 0
    mov dword [edi + TXN_RESERVED], 0
    inc dword [transaction_active_count]
    mov eax, [edi + TXN_ID_LOW]
    clc
    ret
.bad:
    xor eax, eax
    stc
    ret

transaction_lookup:
    test eax, eax
    jz .bad
    xor esi, esi
.scan:
    cmp esi, TRANSACTION_CAPACITY
    jae .bad
    imul edi, esi, TRANSACTION_RECORD_SIZE
    add edi, transaction_records
    cmp [edi + TXN_ID_LOW], eax
    je .found
    inc esi
    jmp .scan
.found:
    cmp dword [edi + TXN_STATE], 0
    je .bad
    mov eax, edi
    clc
    ret
.bad:
    xor eax, eax
    stc
    ret

; EAX=Transaction-ID, EDX=ExpectedVersion. Prepared bedeutet noch nicht Commit.
transaction_prepare:
    call transaction_lookup
    jc .bad
    cmp dword [eax + TXN_STATE], TRANSACTION_ACTIVE
    jne .bad
    cmp [eax + TXN_EXPECTED_VERSION], edx
    jne .conflict
    mov dword [eax + TXN_STATE], TRANSACTION_PREPARED
    clc
    ret
.conflict:
    mov dword [eax + TXN_STATE], TRANSACTION_FAILED
.bad:
    stc
    ret

; EAX=Transaction-ID. Commit und Verification bleiben getrennte Schritte.
transaction_commit:
    call transaction_lookup
    jc .bad
    cmp dword [eax + TXN_STATE], TRANSACTION_PREPARED
    jne .bad
    mov dword [eax + TXN_STATE], TRANSACTION_COMMITTED
    mov edx, [state_generation_counter]
    mov [eax + TXN_COMMIT_GENERATION], edx
    clc
    ret
.bad:
    stc
    ret

transaction_verify:
    call transaction_lookup
    jc .bad
    cmp dword [eax + TXN_STATE], TRANSACTION_COMMITTED
    jne .bad
    mov dword [eax + TXN_STATE], TRANSACTION_COMPLETED
    dec dword [transaction_active_count]
    clc
    ret
.bad:
    stc
    ret

transaction_manager_self_test:
    mov eax, 1
    mov ebx, TRANSACTION_SCOPE_SERVICE
    mov ecx, 2
    call transaction_begin
    jc .bad
    mov [transaction_test_id], eax
    mov edx, 1                         ; falsche Version bleibt Konflikt
    call transaction_prepare
    jnc .bad
    mov eax, 1
    mov ebx, TRANSACTION_SCOPE_SERVICE
    mov ecx, 2
    call transaction_begin
    jc .bad
    mov [transaction_test_id], eax
    mov edx, 2
    call transaction_prepare
    jc .bad
    mov eax, [transaction_test_id]
    call transaction_lookup
    jc .bad
    cmp dword [eax + TXN_STATE], TRANSACTION_PREPARED
    jne .bad
    mov eax, [transaction_test_id]
    call transaction_commit
    jc .bad
    mov eax, [transaction_test_id]
    call transaction_lookup
    jc .bad
    cmp dword [eax + TXN_STATE], TRANSACTION_COMMITTED
    jne .bad
    mov eax, [transaction_test_id]
    call transaction_verify
    jc .bad
    mov eax, [transaction_test_id]
    call transaction_lookup
    jc .bad
    cmp dword [eax + TXN_STATE], TRANSACTION_COMPLETED
    jne .bad
    clc
    ret
.bad:
    stc
    ret

align 4
state_record_count:       dd 0
state_generation_counter: dd 0
state_temp_high:          dd 0
state_temp_low:           dd 0
state_temp_kind:          dd 0
state_temp_owner:         dd 0
state_test_slot:          dd 0
state_records:
    times STATE_RECORD_CAPACITY * STATE_RECORD_SIZE db 0

transaction_next_id:       dd 0
transaction_active_count:  dd 0
transaction_temp_owner:    dd 0
transaction_temp_scope:    dd 0
transaction_temp_expected: dd 0
transaction_test_id:       dd 0
transaction_records:
    times TRANSACTION_CAPACITY * TRANSACTION_RECORD_SIZE db 0
