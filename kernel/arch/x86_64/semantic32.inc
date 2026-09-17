; Minimaler Semantic-Type-Kern (ADR-ARCH-0008, ADR-SEMANTIC-0001/0002).
; Registry-Mutation ist ausschließlich BSP-Boot-Control-Plane und wird vor
; Scheduler/Userspace versiegelt. Fast Paths prüfen internierte Handles.
SEMANTIC_TYPE_CAPACITY       equ 8
SEMANTIC_TYPE_RECORD_SIZE    equ 80
SEMANTIC_TYPE_NAME_SIZE      equ 64
SEMANTIC_TYPE_REPRESENTATION equ 64
SEMANTIC_TYPE_VERSION        equ 68
SEMANTIC_TYPE_NAME_LENGTH    equ 72
SEMANTIC_TYPE_VALIDATOR      equ 76
SEMANTIC_REPR_BYTES8         equ 1
SEMANTIC_IPC_INLINE_DATA     equ 1
SEMANTIC_IPC_DIAGNOSTIC      equ 2
SEMANTIC_TYPE_VERSION_1      equ 1
SEMANTIC_VALIDATION_NONE     equ 0
SEMANTIC_VALIDATION_DECLARED equ 1
SEMANTIC_VALIDATION_VERIFIED equ 2
SEMANTIC_VALIDATION_INVALID  equ 3
SEMANTIC_COMPAT_UNKNOWN      equ 0
SEMANTIC_COMPAT_EXACT        equ 1
SEMANTIC_COMPAT_SUBTYPE      equ 2
SEMANTIC_COMPAT_TRAIT        equ 3
SEMANTIC_COMPAT_CONVERTIBLE  equ 4
SEMANTIC_COMPAT_INCOMPATIBLE equ 5
SEMANTIC_CONVERSION_CAPACITY equ 8
SEMANTIC_CONVERSION_SIZE     equ 24
SEMANTIC_CONVERSION_LOSSLESS equ 1
SEMANTIC_CONVERSION_LOSSY    equ 2
SEMANTIC_RULE_NONEMPTY        equ 1
SEMANTIC_RULE_DIAGNOSTIC_CODE equ 2
SEMANTIC_RULE_CONTRACT_MAX    equ 0x100
SEMANTIC_VALIDATION_ERROR_NONE     equ 0
SEMANTIC_VALIDATION_ERROR_TYPE     equ 1
SEMANTIC_VALIDATION_ERROR_VERSION  equ 2
SEMANTIC_VALIDATION_ERROR_RULE     equ 3
SEMANTIC_VALIDATION_ERROR_CONTRACT equ 4

semantic_initialize:
    mov edi, semantic_type_records
    xor eax, eax
    mov ecx, (SEMANTIC_TYPE_CAPACITY * SEMANTIC_TYPE_RECORD_SIZE) / 4
    rep stosd
    mov edi, semantic_conversion_records
    mov ecx, (SEMANTIC_CONVERSION_CAPACITY * SEMANTIC_CONVERSION_SIZE) / 4
    rep stosd
    mov dword [semantic_type_count], 0
    mov dword [semantic_conversion_count], 0
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
    mov eax, SEMANTIC_IPC_INLINE_DATA
    mov ecx, SEMANTIC_RULE_NONEMPTY
    call semantic_register_validation_rule
    jc .bad
    mov eax, SEMANTIC_IPC_DIAGNOSTIC
    mov ecx, SEMANTIC_RULE_DIAGNOSTIC_CODE
    call semantic_register_validation_rule
    jc .bad
    mov eax, SEMANTIC_IPC_INLINE_DATA
    mov ecx, SEMANTIC_IPC_DIAGNOSTIC
    mov edx, 0x44494147             ; explizite Capability-ID "DIAG"
    mov ebx, SEMANTIC_CONVERSION_LOSSY
    call semantic_register_conversion
    jc .bad
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

; EAX=Type Handle, ECX=Rule-ID. Regeln werden wie Typen ausschließlich vor
; dem Versiegeln registriert und verändern bei der Prüfung niemals den Wert.
semantic_register_validation_rule:
    cmp dword [semantic_registry_sealed], 0
    jne .bad
    cmp ecx, SEMANTIC_RULE_NONEMPTY
    je .known
    cmp ecx, SEMANTIC_RULE_DIAGNOSTIC_CODE
    jne .bad
.known:
    push ecx
    call semantic_lookup_type
    pop ecx
    jc .bad
    cmp dword [edx + SEMANTIC_TYPE_VALIDATOR], 0
    jne .bad
    mov [edx + SEMANTIC_TYPE_VALIDATOR], ecx
    clc
    ret
.bad:
    stc
    ret

; EAX/ECX=Type/Version, EDX=Wertadresse, EBX=Wertlaenge,
; ESI=optionales Contract-Maximum (0 = keine Zusatzregel).
; Der strukturierte Fehler nennt Regel, Erwartung und Istwert. Typpruefung,
; Type-Regel und Capability-Contract bleiben erkennbare Einzelschritte.
semantic_validate_value:
    mov [semantic_validation_type], eax
    mov [semantic_validation_version], ecx
    mov [semantic_validation_value], edx
    mov [semantic_validation_length], ebx
    mov [semantic_validation_contract_max], esi
    mov dword [semantic_validation_error], SEMANTIC_VALIDATION_ERROR_NONE
    mov dword [semantic_validation_rule], 0
    mov dword [semantic_validation_expected], 0
    mov dword [semantic_validation_actual], 0
    call semantic_lookup_type
    jc .type_error
    mov ecx, [semantic_validation_version]
    cmp ecx, [edx + SEMANTIC_TYPE_VERSION]
    jne .version_error
    mov eax, [edx + SEMANTIC_TYPE_VALIDATOR]
    cmp eax, SEMANTIC_RULE_NONEMPTY
    je .nonempty
    cmp eax, SEMANTIC_RULE_DIAGNOSTIC_CODE
    je .diagnostic
    test eax, eax
    jnz .rule_error
    jmp .contract
.nonempty:
    cmp dword [semantic_validation_length], 0
    je .nonempty_error
    cmp dword [semantic_validation_value], 0
    je .nonempty_error
    jmp .contract
.diagnostic:
    cmp dword [semantic_validation_length], 4
    jb .diagnostic_error
    mov edx, [semantic_validation_value]
    cmp dword [edx], 0
    je .diagnostic_error
.contract:
    mov eax, [semantic_validation_contract_max]
    test eax, eax
    jz .valid
    cmp [semantic_validation_length], eax
    ja .contract_error
.valid:
    mov dword [semantic_validation_state], SEMANTIC_VALIDATION_VERIFIED
    clc
    ret
.type_error:
    mov dword [semantic_validation_error], SEMANTIC_VALIDATION_ERROR_TYPE
    jmp .invalid
.version_error:
    mov dword [semantic_validation_error], SEMANTIC_VALIDATION_ERROR_VERSION
    mov eax, [edx + SEMANTIC_TYPE_VERSION]
    mov [semantic_validation_expected], eax
    mov eax, [semantic_validation_version]
    mov [semantic_validation_actual], eax
    jmp .invalid
.rule_error:
    mov dword [semantic_validation_error], SEMANTIC_VALIDATION_ERROR_RULE
    mov [semantic_validation_rule], eax
    jmp .invalid
.nonempty_error:
    mov dword [semantic_validation_error], SEMANTIC_VALIDATION_ERROR_RULE
    mov dword [semantic_validation_rule], SEMANTIC_RULE_NONEMPTY
    mov dword [semantic_validation_expected], 1
    mov eax, [semantic_validation_length]
    mov [semantic_validation_actual], eax
    jmp .invalid
.diagnostic_error:
    mov dword [semantic_validation_error], SEMANTIC_VALIDATION_ERROR_RULE
    mov dword [semantic_validation_rule], SEMANTIC_RULE_DIAGNOSTIC_CODE
    mov dword [semantic_validation_expected], 1
    mov dword [semantic_validation_actual], 0
    jmp .invalid
.contract_error:
    mov dword [semantic_validation_error], SEMANTIC_VALIDATION_ERROR_CONTRACT
    mov dword [semantic_validation_rule], SEMANTIC_RULE_CONTRACT_MAX
    mov [semantic_validation_expected], eax
    mov eax, [semantic_validation_length]
    mov [semantic_validation_actual], eax
.invalid:
    mov dword [semantic_validation_state], SEMANTIC_VALIDATION_INVALID
    stc
    ret

; EAX=Source-Type, ECX=Target-Type, EDX=Conversion-Capability, EBX=Loss-Klasse.
; Registrierung ist Control-Plane-only; diese Funktion konvertiert keine Daten.
semantic_register_conversion:
    cmp dword [semantic_registry_sealed], 0
    jne .bad
    test edx, edx
    jz .bad
    cmp ebx, SEMANTIC_CONVERSION_LOSSLESS
    jb .bad
    cmp ebx, SEMANTIC_CONVERSION_LOSSY
    ja .bad
    cmp eax, ecx
    je .bad
    mov [semantic_conversion_source], eax
    mov [semantic_conversion_target], ecx
    mov [semantic_conversion_capability], edx
    mov [semantic_conversion_flags], ebx
    call semantic_lookup_type
    jc .bad
    mov eax, [edx + SEMANTIC_TYPE_VERSION]
    mov [semantic_conversion_source_version], eax
    mov eax, [semantic_conversion_target]
    call semantic_lookup_type
    jc .bad
    mov eax, [edx + SEMANTIC_TYPE_VERSION]
    mov [semantic_conversion_target_version], eax
    xor ecx, ecx
.scan:
    cmp ecx, [semantic_conversion_count]
    jae .insert
    imul edi, ecx, SEMANTIC_CONVERSION_SIZE
    add edi, semantic_conversion_records
    mov eax, [semantic_conversion_source]
    cmp eax, [edi]
    jne .next
    mov eax, [semantic_conversion_target]
    cmp eax, [edi + 8]
    je .bad                       ; ein Pfad besitzt genau einen Contract
.next:
    inc ecx
    jmp .scan
.insert:
    cmp ecx, SEMANTIC_CONVERSION_CAPACITY
    jae .bad
    imul edi, ecx, SEMANTIC_CONVERSION_SIZE
    add edi, semantic_conversion_records
    mov eax, [semantic_conversion_source]
    mov [edi], eax
    mov eax, [semantic_conversion_source_version]
    mov [edi + 4], eax
    mov eax, [semantic_conversion_target]
    mov [edi + 8], eax
    mov eax, [semantic_conversion_target_version]
    mov [edi + 12], eax
    mov eax, [semantic_conversion_capability]
    mov [edi + 16], eax
    mov eax, [semantic_conversion_flags]
    mov [edi + 20], eax
    inc dword [semantic_conversion_count]
    mov eax, [semantic_conversion_count]
    clc
    ret
.bad:
    xor eax, eax
    stc
    ret

; EAX/ECX=Source Type/Version, EDX/EBX=Target Type/Version.
; EAX=Kompatibilitätsklasse, bei CONVERTIBLE zusätzlich EDX=Capability und
; EBX=Loss-Klasse. Subtype/Trait bleiben reserviert, bis deren NPSPEC vorliegt.
semantic_compatibility:
    mov [semantic_compat_source], eax
    mov [semantic_compat_source_version], ecx
    mov [semantic_compat_target], edx
    mov [semantic_compat_target_version], ebx
    call semantic_lookup_type
    jc .unknown
    cmp ecx, [edx + SEMANTIC_TYPE_VERSION]
    jne .incompatible
    mov eax, [semantic_compat_target]
    call semantic_lookup_type
    jc .unknown
    mov ebx, [semantic_compat_target_version]
    cmp ebx, [edx + SEMANTIC_TYPE_VERSION]
    jne .incompatible
    mov eax, [semantic_compat_source]
    cmp eax, [semantic_compat_target]
    je .exact
    xor ecx, ecx
.conversion_scan:
    cmp ecx, [semantic_conversion_count]
    jae .incompatible
    imul edi, ecx, SEMANTIC_CONVERSION_SIZE
    add edi, semantic_conversion_records
    cmp eax, [edi]
    jne .conversion_next
    mov edx, [semantic_compat_source_version]
    cmp edx, [edi + 4]
    jne .conversion_next
    mov edx, [semantic_compat_target]
    cmp edx, [edi + 8]
    jne .conversion_next
    mov edx, [semantic_compat_target_version]
    cmp edx, [edi + 12]
    jne .conversion_next
    mov edx, [edi + 16]
    mov ebx, [edi + 20]
    mov eax, SEMANTIC_COMPAT_CONVERTIBLE
    ret
.conversion_next:
    inc ecx
    jmp .conversion_scan
.exact:
    mov eax, SEMANTIC_COMPAT_EXACT
    ret
.incompatible:
    mov eax, SEMANTIC_COMPAT_INCOMPATIBLE
    ret
.unknown:
    mov eax, SEMANTIC_COMPAT_UNKNOWN
    ret

; EAX=Objekthandle, ECX=Semantic-Type-Handle, EDX=Version, EBX=Status.
; Typinformation und Ressourcenidentität bleiben getrennt. Ein vorhandener
; Primary Type darf nicht stillschweigend umgedeutet werden.
object_semantic_attach:
    cmp ebx, SEMANTIC_VALIDATION_DECLARED
    jb .bad
    cmp ebx, SEMANTIC_VALIDATION_VERIFIED
    ja .bad
    push eax
    push ecx
    push edx
    push ebx
    mov eax, ecx
    call semantic_lookup_type
    jc .restore_bad
    mov ecx, [esp + 4]
    cmp ecx, [edx + SEMANTIC_TYPE_VERSION]
    jne .restore_bad
    mov eax, [esp + 12]
    call object_lookup
    jc .restore_bad
    mov eax, [esp + 12]
    and eax, 0xFFFF
    dec eax
    cmp dword [object_semantic_types + eax * 4], 0
    jne .restore_bad
    mov ecx, [esp + 8]
    mov [object_semantic_types + eax * 4], ecx
    mov ecx, [esp + 4]
    mov [object_semantic_versions + eax * 4], ecx
    mov ecx, [esp]
    mov [object_semantic_validation + eax * 4], ecx
    add esp, 16
    clc
    ret
.restore_bad:
    add esp, 16
.bad:
    stc
    ret

; EAX=Objekthandle. ECX=Type, EDX=Version, EBX=Validation bei Erfolg.
object_semantic_query:
    push eax
    call object_lookup
    jc .query_bad
    mov eax, [esp]
    and eax, 0xFFFF
    dec eax
    mov ecx, [object_semantic_types + eax * 4]
    test ecx, ecx
    jz .query_bad
    mov edx, [object_semantic_versions + eax * 4]
    mov ebx, [object_semantic_validation + eax * 4]
    add esp, 4
    clc
    ret
.query_bad:
    add esp, 4
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
    mov eax, SEMANTIC_IPC_INLINE_DATA
    mov ecx, SEMANTIC_TYPE_VERSION_1
    mov edx, semantic_validation_test_value
    mov ebx, 4
    mov esi, 4
    call semantic_validate_value
    jc .bad
    cmp dword [semantic_validation_state], SEMANTIC_VALIDATION_VERIFIED
    jne .bad
    mov eax, SEMANTIC_IPC_INLINE_DATA
    mov ecx, SEMANTIC_TYPE_VERSION_1
    mov edx, semantic_validation_test_value
    xor ebx, ebx
    mov esi, 4
    call semantic_validate_value
    jnc .bad
    cmp dword [semantic_validation_error], SEMANTIC_VALIDATION_ERROR_RULE
    jne .bad
    cmp dword [semantic_validation_rule], SEMANTIC_RULE_NONEMPTY
    jne .bad
    mov eax, SEMANTIC_IPC_INLINE_DATA
    mov ecx, SEMANTIC_TYPE_VERSION_1
    mov edx, SEMANTIC_IPC_INLINE_DATA
    mov ebx, SEMANTIC_TYPE_VERSION_1
    call semantic_compatibility
    cmp eax, SEMANTIC_COMPAT_EXACT
    jne .bad
    mov eax, SEMANTIC_IPC_INLINE_DATA
    mov ecx, SEMANTIC_TYPE_VERSION_1
    mov edx, SEMANTIC_IPC_DIAGNOSTIC
    mov ebx, SEMANTIC_TYPE_VERSION_1
    call semantic_compatibility
    cmp eax, SEMANTIC_COMPAT_CONVERTIBLE
    jne .bad
    cmp edx, 0x44494147
    jne .bad
    cmp ebx, SEMANTIC_CONVERSION_LOSSY
    jne .bad
    mov eax, SEMANTIC_IPC_DIAGNOSTIC
    mov ecx, SEMANTIC_TYPE_VERSION_1
    mov edx, SEMANTIC_IPC_INLINE_DATA
    mov ebx, SEMANTIC_TYPE_VERSION_1
    call semantic_compatibility
    cmp eax, SEMANTIC_COMPAT_INCOMPATIBLE
    jne .bad
    mov eax, 7
    mov ecx, SEMANTIC_TYPE_VERSION_1
    mov edx, SEMANTIC_IPC_INLINE_DATA
    mov ebx, SEMANTIC_TYPE_VERSION_1
    call semantic_compatibility
    cmp eax, SEMANTIC_COMPAT_UNKNOWN
    jne .bad
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
    mov eax, 1
    xor edx, edx
    xor ebx, ebx
    call object_create
    jc .bad
    mov [semantic_test_object], eax
    mov ecx, SEMANTIC_IPC_DIAGNOSTIC
    mov edx, SEMANTIC_TYPE_VERSION_1
    mov ebx, SEMANTIC_VALIDATION_VERIFIED
    call object_semantic_attach
    jc .release_bad
    mov eax, [semantic_test_object]
    call object_semantic_query
    jc .release_bad
    cmp ecx, SEMANTIC_IPC_DIAGNOSTIC
    jne .release_bad
    cmp edx, SEMANTIC_TYPE_VERSION_1
    jne .release_bad
    cmp ebx, SEMANTIC_VALIDATION_VERIFIED
    jne .release_bad
    ; Eine zweite Bedeutung darf nicht auf dieselbe Resource-ID gelegt werden.
    mov eax, [semantic_test_object]
    mov ecx, SEMANTIC_IPC_INLINE_DATA
    mov edx, SEMANTIC_TYPE_VERSION_1
    mov ebx, SEMANTIC_VALIDATION_DECLARED
    call object_semantic_attach
    jnc .release_bad
    mov eax, [semantic_test_object]
    call object_release
    jc .bad
    mov eax, [semantic_test_object]
    call object_semantic_query
    jnc .bad                     ; Freigabe entfernt auch Type-Sidecar
    clc
    ret
.release_bad:
    mov eax, [semantic_test_object]
    call object_release
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
semantic_test_object:                dd 0
semantic_conversion_count:           dd 0
semantic_conversion_source:          dd 0
semantic_conversion_source_version:  dd 0
semantic_conversion_target:          dd 0
semantic_conversion_target_version:  dd 0
semantic_conversion_capability:      dd 0
semantic_conversion_flags:           dd 0
semantic_compat_source:               dd 0
semantic_compat_source_version:       dd 0
semantic_compat_target:               dd 0
semantic_compat_target_version:       dd 0
semantic_validation_type:             dd 0
semantic_validation_version:          dd 0
semantic_validation_value:            dd 0
semantic_validation_length:           dd 0
semantic_validation_contract_max:     dd 0
semantic_validation_state:            dd 0
semantic_validation_error:            dd 0
semantic_validation_rule:             dd 0
semantic_validation_expected:         dd 0
semantic_validation_actual:           dd 0
semantic_validation_test_value:       dd 0x41564F4E
semantic_type_records:
    times SEMANTIC_TYPE_CAPACITY * SEMANTIC_TYPE_RECORD_SIZE db 0
semantic_conversion_records:
    times SEMANTIC_CONVERSION_CAPACITY * SEMANTIC_CONVERSION_SIZE db 0
semantic_name_inline_data:
    db 'nova.kernel.ipc.inline-data', 0
semantic_name_diagnostic:
    db 'nova.kernel.ipc.diagnostic', 0
