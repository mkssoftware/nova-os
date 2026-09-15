; Minimaler Semantic-Type-Kern (ADR-ARCH-0008, ADR-SEMANTIC-0001/0002).
; Registry-Mutation ist ausschließlich BSP-Boot-Control-Plane und wird vor
; Scheduler/Userspace versiegelt. Fast Paths prüfen internierte Handles.
SEMANTIC_TYPE_CAPACITY       equ 8
SEMANTIC_TYPE_RECORD_SIZE    equ 80
SEMANTIC_TYPE_NAME_SIZE      equ 64
SEMANTIC_TYPE_REPRESENTATION equ 64
SEMANTIC_TYPE_VERSION        equ 68
SEMANTIC_TYPE_NAME_LENGTH    equ 72
SEMANTIC_REPR_BYTES8         equ 1
SEMANTIC_IPC_INLINE_DATA     equ 1
SEMANTIC_IPC_DIAGNOSTIC      equ 2
SEMANTIC_TYPE_VERSION_1      equ 1

semantic_initialize:
    mov edi, semantic_type_records
    xor eax, eax
    mov ecx, (SEMANTIC_TYPE_CAPACITY * SEMANTIC_TYPE_RECORD_SIZE) / 4
    rep stosd
    mov dword [semantic_type_count], 0
    mov dword [semantic_registry_sealed], 0
    mov esi, semantic_name_inline_data
    mov eax, SEMANTIC_REPR_BYTES8
    mov edx, SEMANTIC_TYPE_VERSION_1
    call semantic_register_type
    jc .bad
    cmp eax, SEMANTIC_IPC_INLINE_DATA
    jne .bad
    mov esi, semantic_name_diagnostic
    mov eax, SEMANTIC_REPR_BYTES8
    mov edx, SEMANTIC_TYPE_VERSION_1
    call semantic_register_type
    jc .bad
    cmp eax, SEMANTIC_IPC_DIAGNOSTIC
    jne .bad
    mov dword [semantic_registry_sealed], 1
    clc
    ret
.bad:
    stc
    ret

; ESI=stabiler namespacefähiger Name, EAX=technischer Repräsentationstyp,
; EDX=Version. Registrierungsrechte sind nicht an Userspace exponiert.
semantic_register_type:
    cmp dword [semantic_registry_sealed], 0
    jne .bad
    cmp eax, SEMANTIC_REPR_BYTES8
    jne .bad
    test edx, edx
    jz .bad
    mov [semantic_registration_name], esi
    mov [semantic_registration_representation], eax
    mov [semantic_registration_version], edx
    xor ecx, ecx
    xor ebx, ebx
.validate_name:
    cmp ecx, SEMANTIC_TYPE_NAME_SIZE - 1
    jae .bad
    movzx eax, byte [esi + ecx]
    test eax, eax
    jz .name_done
    cmp eax, '.'
    je .dot
    cmp eax, '-'
    je .character_ok
    cmp eax, '0'
    jb .bad
    cmp eax, '9'
    jbe .character_ok
    cmp eax, 'a'
    jb .bad
    cmp eax, 'z'
    ja .bad
.character_ok:
    inc ecx
    jmp .validate_name
.dot:
    inc ebx
    inc ecx
    jmp .validate_name
.name_done:
    cmp ecx, 3
    jb .bad
    test ebx, ebx
    jz .bad
    cmp byte [esi], '.'
    je .bad
    cmp byte [esi + ecx - 1], '.'
    je .bad
    mov [semantic_registration_length], ecx
    xor ebx, ebx
.duplicate_scan:
    cmp ebx, [semantic_type_count]
    jae .insert
    imul edi, ebx, SEMANTIC_TYPE_RECORD_SIZE
    add edi, semantic_type_records
    cmp ecx, [edi + SEMANTIC_TYPE_NAME_LENGTH]
    jne .next
    mov esi, [semantic_registration_name]
    push ecx
.compare:
    mov al, [esi]
    cmp al, [edi]
    jne .different
    inc esi
    inc edi
    loop .compare
    pop ecx
    ; Identität+Version darf nie mit neuer Bedeutung überschrieben werden.
    ; EDI wurde beim Bytevergleich verschoben, deshalb Record neu berechnen.
    imul edi, ebx, SEMANTIC_TYPE_RECORD_SIZE
    add edi, semantic_type_records
    mov eax, [semantic_registration_version]
    cmp eax, [edi + SEMANTIC_TYPE_VERSION]
    je .bad
    jmp .next
.different:
    pop ecx
.next:
    inc ebx
    jmp .duplicate_scan
.insert:
    cmp ebx, SEMANTIC_TYPE_CAPACITY
    jae .bad
    imul edi, ebx, SEMANTIC_TYPE_RECORD_SIZE
    add edi, semantic_type_records
    mov esi, [semantic_registration_name]
    mov eax, [semantic_registration_length]
    mov [edi + SEMANTIC_TYPE_NAME_LENGTH], eax
    mov eax, [semantic_registration_representation]
    mov [edi + SEMANTIC_TYPE_REPRESENTATION], eax
    mov eax, [semantic_registration_version]
    mov [edi + SEMANTIC_TYPE_VERSION], eax
    mov ecx, [semantic_registration_length]
    rep movsb
    mov byte [edi], 0
    inc dword [semantic_type_count]
    mov eax, [semantic_type_count]
    clc
    ret
.bad:
    xor eax, eax
    stc
    ret

; EAX=interniertes Handle, EDX=Definition. Die volle Name/Version-Identität
; bleibt im Registry-Record; ein bloßer gleicher Primitive Type genügt nicht.
semantic_lookup_type:
    test eax, eax
    jz .bad
    cmp eax, [semantic_type_count]
    ja .bad
    dec eax
    imul edx, eax, SEMANTIC_TYPE_RECORD_SIZE
    add edx, semantic_type_records
    inc eax
    clc
    ret
.bad:
    stc
    ret

; EAX=behauptetes Type Handle, ECX=behauptete Version,
; EDX=erforderlicher technischer Basistyp. Dies validiert nur den Type Claim;
; Payload-Validierung und Autorisierung sind eigenständige Schritte.
semantic_check_claim:
    push ebx
    mov ebx, edx
    call semantic_lookup_type
    jc .bad
    cmp ebx, [edx + SEMANTIC_TYPE_REPRESENTATION]
    jne .bad
    cmp ecx, [edx + SEMANTIC_TYPE_VERSION]
    jne .bad
    pop ebx
    clc
    ret
.bad:
    pop ebx
    stc
    ret

semantic_self_test:
    mov eax, SEMANTIC_IPC_INLINE_DATA
    mov ecx, SEMANTIC_TYPE_VERSION_1
    mov edx, SEMANTIC_REPR_BYTES8
    call semantic_check_claim
    jc .bad
    mov eax, SEMANTIC_IPC_DIAGNOSTIC
    mov ecx, SEMANTIC_TYPE_VERSION_1
    mov edx, SEMANTIC_REPR_BYTES8
    call semantic_check_claim
    jc .bad
    ; Beide besitzen Bytes8, sind aber nominal semantisch verschieden.
    cmp dword [semantic_type_count], 2
    jne .bad
    mov eax, SEMANTIC_IPC_INLINE_DATA
    mov ecx, 2
    mov edx, SEMANTIC_REPR_BYTES8
    call semantic_check_claim
    jnc .bad
    mov eax, 3
    mov ecx, SEMANTIC_TYPE_VERSION_1
    mov edx, SEMANTIC_REPR_BYTES8
    call semantic_check_claim
    jnc .bad
    mov esi, semantic_name_inline_data
    mov eax, SEMANTIC_REPR_BYTES8
    mov edx, SEMANTIC_TYPE_VERSION_1
    call semantic_register_type
    jnc .bad                     ; versiegelte Registry nicht veränderbar
    clc
    ret
.bad:
    stc
    ret

align 4
semantic_type_count:                 dd 0
semantic_registry_sealed:           dd 0
semantic_registration_name:         dd 0
semantic_registration_representation: dd 0
semantic_registration_version:      dd 0
semantic_registration_length:       dd 0
semantic_type_records:
    times SEMANTIC_TYPE_CAPACITY * SEMANTIC_TYPE_RECORD_SIZE db 0
semantic_name_inline_data:
    db 'nova.kernel.ipc.inline-data', 0
semantic_name_diagnostic:
    db 'nova.kernel.ipc.diagnostic', 0
