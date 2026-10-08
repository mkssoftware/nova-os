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
; §119: Mehrere kompatible Semantic Types pro Ressource
SEMANTIC_SECONDARY_CAPACITY equ 4   ; sekundäre Typen pro Objekt (Primary nicht gezählt)
SEMANTIC_OBJECT_CAPACITY    equ 64  ; muss mit Objektsystem-Kapazität übereinstimmen
; §120: Ausführung registrierter Conversion Capabilities
SEMANTIC_HANDLER_CAPACITY   equ 8   ; max. registrierte Conversion Handler
SEMANTIC_HANDLER_SIZE       equ 8   ; cap_id(4) + handler_ptr(4)
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
    ; §120: Handler-Records initialisieren
    mov edi, semantic_handler_records
    mov ecx, (SEMANTIC_HANDLER_CAPACITY * SEMANTIC_HANDLER_SIZE) / 4
    rep stosd
    mov dword [semantic_handler_count], 0
    ; §119: Sekundäre Typ-Sidecar-Felder initialisieren
    mov edi, object_semantic_secondary_types
    mov ecx, SEMANTIC_OBJECT_CAPACITY * SEMANTIC_SECONDARY_CAPACITY
    rep stosd
    mov edi, object_semantic_secondary_versions
    mov ecx, SEMANTIC_OBJECT_CAPACITY * SEMANTIC_SECONDARY_CAPACITY
    rep stosd
    mov edi, object_semantic_secondary_validation
    mov ecx, SEMANTIC_OBJECT_CAPACITY * SEMANTIC_SECONDARY_CAPACITY
    rep stosd
    mov edi, object_semantic_secondary_count_array
    mov ecx, SEMANTIC_OBJECT_CAPACITY
    rep stosd
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
    ; §120: Eingebauten Handler für INLINE_DATA→DIAGNOSTIC registrieren
    mov eax, 0x44494147             ; Capability-ID "DIAG"
    mov ecx, semantic_handler_inline_to_diag
    call semantic_register_conversion_handler
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
    ; §119: Sekundäre Slots bei Primary-Attach zurücksetzen
    mov dword [object_semantic_secondary_count_array + eax * 4], 0
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

; §119: Sekundären Typ an Objekt anhängen.
; EAX=Objekthandle, ECX=Secondary Type Handle, EDX=Version, EBX=ValidationStatus.
; Primary muss bereits gesetzt sein. Secondary != Primary, kein Duplikat.
; CF=0 bei Erfolg; CF=1 bei Fehler.
object_semantic_attach_secondary:
    cmp ebx, SEMANTIC_VALIDATION_DECLARED
    jb .bad
    cmp ebx, SEMANTIC_VALIDATION_VERIFIED
    ja .bad
    push eax                    ; [esp+12] = Objekthandle
    push ecx                    ; [esp+8]  = Secondary Type Handle
    push edx                    ; [esp+4]  = Version
    push ebx                    ; [esp+0]  = ValidationStatus
    mov eax, ecx
    call semantic_lookup_type
    jc .restore_bad
    mov ecx, [esp+4]
    cmp ecx, [edx + SEMANTIC_TYPE_VERSION]
    jne .restore_bad
    mov eax, [esp+12]
    call object_lookup
    jc .restore_bad
    mov eax, [esp+12]
    and eax, 0xFFFF
    dec eax                     ; EAX = Objektindex
    cmp dword [object_semantic_types + eax * 4], 0
    je .restore_bad             ; Primary muss zuerst gesetzt sein
    mov ecx, [esp+8]
    cmp ecx, [object_semantic_types + eax * 4]
    je .restore_bad             ; Secondary darf nicht gleich Primary sein
    mov edi, eax
    imul edi, edi, SEMANTIC_SECONDARY_CAPACITY ; EDI = Basis-Slot-Index
    xor ebx, ebx               ; EBX = Slot-Zähler
.sec_scan:
    cmp ebx, [object_semantic_secondary_count_array + eax * 4]
    jae .sec_insert
    lea esi, [edi + ebx]
    cmp ecx, [object_semantic_secondary_types + esi * 4]
    je .restore_bad             ; Duplikat abweisen
    inc ebx
    jmp .sec_scan
.sec_insert:
    cmp ebx, SEMANTIC_SECONDARY_CAPACITY
    jae .restore_bad            ; Kapazität erschöpft
    lea esi, [edi + ebx]
    mov [object_semantic_secondary_types + esi * 4], ecx
    mov ecx, [esp+4]
    mov [object_semantic_secondary_versions + esi * 4], ecx
    mov ecx, [esp]
    mov [object_semantic_secondary_validation + esi * 4], ecx
    inc dword [object_semantic_secondary_count_array + eax * 4]
    add esp, 16
    clc
    ret
.restore_bad:
    add esp, 16
.bad:
    stc
    ret

; §119: Prüft ob Primary- oder irgendein Secondary-Type mit Type Handle übereinstimmt.
; EAX=Objekthandle, ECX=gesuchter Type Handle.
; CF=0: gefunden (EDX=Version, EBX=ValidationStatus). CF=1: nicht gefunden.
object_semantic_has_type:
    push eax                    ; [esp+4] = Objekthandle
    push ecx                    ; [esp+0] = gesuchter Type Handle
    call object_lookup
    jc .has_bad
    mov eax, [esp+4]
    and eax, 0xFFFF
    dec eax                     ; Objektindex
    mov ecx, [esp]              ; gesuchter Type Handle
    cmp ecx, [object_semantic_types + eax * 4]
    jne .has_check_secondary
    mov edx, [object_semantic_versions + eax * 4]
    mov ebx, [object_semantic_validation + eax * 4]
    add esp, 8
    clc
    ret
.has_check_secondary:
    mov edi, eax
    imul edi, edi, SEMANTIC_SECONDARY_CAPACITY
    xor ebx, ebx
.has_scan:
    cmp ebx, [object_semantic_secondary_count_array + eax * 4]
    jae .has_bad
    lea esi, [edi + ebx]
    cmp ecx, [object_semantic_secondary_types + esi * 4]
    jne .has_next
    mov edx, [object_semantic_secondary_versions + esi * 4]
    mov ebx, [object_semantic_secondary_validation + esi * 4]
    add esp, 8
    clc
    ret
.has_next:
    inc ebx
    jmp .has_scan
.has_bad:
    add esp, 8
    stc
    ret

; §119: Secondary Type nach Slot-Index abfragen.
; EAX=Objekthandle, ECX=Slot-Index (0-basiert).
; CF=0: ECX=TypeHandle, EDX=Version, EBX=ValidationStatus. CF=1: ungültig.
object_semantic_query_secondary:
    push eax                    ; [esp+4] = Objekthandle
    push ecx                    ; [esp+0] = Slot-Index
    call object_lookup
    jc .qsec_bad
    mov eax, [esp+4]
    and eax, 0xFFFF
    dec eax                     ; Objektindex
    mov ecx, [esp]              ; Slot-Index
    cmp ecx, [object_semantic_secondary_count_array + eax * 4]
    jae .qsec_bad
    imul edi, eax, SEMANTIC_SECONDARY_CAPACITY
    add edi, ecx                ; EDI = absoluter Slot-Flat-Index
    mov ecx, [object_semantic_secondary_types + edi * 4]
    mov edx, [object_semantic_secondary_versions + edi * 4]
    mov ebx, [object_semantic_secondary_validation + edi * 4]
    add esp, 8
    clc
    ret
.qsec_bad:
    add esp, 8
    stc
    ret

; §119: Anzahl der sekundären Types eines Objekts.
; EAX=Objekthandle. EAX=Anzahl bei CF=0. CF=1 bei ungültigem Handle.
object_semantic_secondary_count:
    push eax
    call object_lookup
    jc .cnt_bad
    mov eax, [esp]
    and eax, 0xFFFF
    dec eax
    mov eax, [object_semantic_secondary_count_array + eax * 4]
    add esp, 4
    clc
    ret
.cnt_bad:
    add esp, 4
    stc
    ret

; §119: Sekundäre Typ-Slots eines Objekts zurücksetzen.
; Aufzurufen nach object_release, damit der Slot für neue Objekte sauber ist.
; EAX=früheres Objekthandle (Slot-Index-Berechnung; object_lookup wird nicht benötigt).
object_semantic_clear_secondary:
    and eax, 0xFFFF
    dec eax                     ; Objektindex
    mov dword [object_semantic_secondary_count_array + eax * 4], 0
    imul edi, eax, SEMANTIC_SECONDARY_CAPACITY
    xor ecx, ecx
.clr_loop:
    cmp ecx, SEMANTIC_SECONDARY_CAPACITY
    jae .clr_done
    lea esi, [edi + ecx]
    mov dword [object_semantic_secondary_types + esi * 4], 0
    mov dword [object_semantic_secondary_versions + esi * 4], 0
    mov dword [object_semantic_secondary_validation + esi * 4], 0
    inc ecx
    jmp .clr_loop
.clr_done:
    clc
    ret

; §120: Eingebauter Konvertierungshandler: nova.kernel.ipc.inline-data → nova.kernel.ipc.diagnostic.
; EAX=src_ptr, ECX=src_len, EDX=dst_ptr, EBX=dst_max_len.
; Kopiert die ersten 4 Bytes als Diagnosecode; Code muss ungleich Null sein.
; Rückgabe: EAX=bytes_written (4) oder 0 bei Fehler.
semantic_handler_inline_to_diag:
    cmp ecx, 4
    jb .hfail
    cmp ebx, 4
    jb .hfail
    push eax
    mov eax, [eax]              ; erste 4 Bytes der Quelle = potentieller Diagnosecode
    test eax, eax
    jz .hfail_pop
    mov [edx], eax
    pop eax
    mov eax, 4
    ret
.hfail_pop:
    pop eax
.hfail:
    xor eax, eax
    ret

; §120: Conversion Handler registrieren.
; EAX=Capability-ID, ECX=Handler-Funktionszeiger. Nur vor dem Versiegeln.
semantic_register_conversion_handler:
    cmp dword [semantic_registry_sealed], 0
    jne .bad
    test eax, eax
    jz .bad
    test ecx, ecx
    jz .bad
    mov [semantic_handler_reg_cap], eax
    mov [semantic_handler_reg_ptr], ecx
    xor ebx, ebx
.handler_scan:
    cmp ebx, [semantic_handler_count]
    jae .handler_insert
    imul edi, ebx, SEMANTIC_HANDLER_SIZE
    add edi, semantic_handler_records
    mov eax, [semantic_handler_reg_cap]
    cmp eax, [edi]
    je .bad                     ; Capability-ID bereits registriert
    inc ebx
    jmp .handler_scan
.handler_insert:
    cmp ebx, SEMANTIC_HANDLER_CAPACITY
    jae .bad
    imul edi, ebx, SEMANTIC_HANDLER_SIZE
    add edi, semantic_handler_records
    mov eax, [semantic_handler_reg_cap]
    mov [edi], eax
    mov eax, [semantic_handler_reg_ptr]
    mov [edi + 4], eax
    inc dword [semantic_handler_count]
    clc
    ret
.bad:
    stc
    ret

; §120: Konvertierung tatsächlich ausführen.
; semantic_exec_* Felder vor dem Aufruf befüllen:
;   semantic_exec_src_type/version, semantic_exec_tgt_type/version,
;   semantic_exec_src_ptr, semantic_exec_src_len,
;   semantic_exec_dst_ptr, semantic_exec_dst_max.
; Ergebnis: CF=0 + EAX=bytes_written; CF=1 bei Fehler (kein Pfad, kein Handler).
semantic_execute_conversion:
    ; 1. Source-Type validieren
    mov eax, [semantic_exec_src_type]
    call semantic_lookup_type
    jc .bad
    mov ecx, [semantic_exec_src_version]
    cmp ecx, [edx + SEMANTIC_TYPE_VERSION]
    jne .bad
    ; 2. Target-Type validieren
    mov eax, [semantic_exec_tgt_type]
    call semantic_lookup_type
    jc .bad
    mov ecx, [semantic_exec_tgt_version]
    cmp ecx, [edx + SEMANTIC_TYPE_VERSION]
    jne .bad
    ; 3. Konvertierungspfad suchen
    xor ecx, ecx
.conv_scan:
    cmp ecx, [semantic_conversion_count]
    jae .bad
    imul edi, ecx, SEMANTIC_CONVERSION_SIZE
    add edi, semantic_conversion_records
    mov eax, [semantic_exec_src_type]
    cmp eax, [edi]
    jne .conv_next
    mov eax, [semantic_exec_src_version]
    cmp eax, [edi + 4]
    jne .conv_next
    mov eax, [semantic_exec_tgt_type]
    cmp eax, [edi + 8]
    jne .conv_next
    mov eax, [semantic_exec_tgt_version]
    cmp eax, [edi + 12]
    jne .conv_next
    mov eax, [edi + 16]              ; Capability-ID des Pfads
    mov [semantic_exec_capability], eax
    jmp .find_handler
.conv_next:
    inc ecx
    jmp .conv_scan
.find_handler:
    ; 4. Registrierten Handler für Capability-ID suchen
    xor ecx, ecx
.handler_find:
    cmp ecx, [semantic_handler_count]
    jae .bad
    imul edi, ecx, SEMANTIC_HANDLER_SIZE
    add edi, semantic_handler_records
    mov eax, [semantic_exec_capability]
    cmp eax, [edi]
    jne .handler_next
    ; Handler gefunden → aufrufen
    mov eax, [semantic_exec_src_ptr]
    mov ecx, [semantic_exec_src_len]
    mov edx, [semantic_exec_dst_ptr]
    mov ebx, [semantic_exec_dst_max]
    call dword [edi + 4]             ; handler(src_ptr, src_len, dst_ptr, dst_max)
    test eax, eax
    jz .bad
    mov [semantic_exec_dst_written], eax
    clc
    ret
.handler_next:
    inc ecx
    jmp .handler_find
.bad:
    xor eax, eax
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
    ; §119: Mehrere Semantic Types – Secondary-Type-Anhang und Abfrage
    mov eax, 1
    xor edx, edx
    xor ebx, ebx
    call object_create
    jc .bad
    mov [semantic_test_object], eax
    ; Primary Type setzen
    mov ecx, SEMANTIC_IPC_INLINE_DATA
    mov edx, SEMANTIC_TYPE_VERSION_1
    mov ebx, SEMANTIC_VALIDATION_VERIFIED
    call object_semantic_attach
    jc .release_bad
    ; Secondary ohne Primary-Vorbedingung (neues Objekt ohne Primary darf nicht klappen)
    ; → hier Primary bereits gesetzt; Positive-Test: Secondary anhängen
    mov eax, [semantic_test_object]
    mov ecx, SEMANTIC_IPC_DIAGNOSTIC
    mov edx, SEMANTIC_TYPE_VERSION_1
    mov ebx, SEMANTIC_VALIDATION_DECLARED
    call object_semantic_attach_secondary
    jc .release_bad             ; muss gelingen
    ; Secondary Slot 0 == DIAGNOSTIC
    mov eax, [semantic_test_object]
    xor ecx, ecx
    call object_semantic_query_secondary
    jc .release_bad
    cmp ecx, SEMANTIC_IPC_DIAGNOSTIC
    jne .release_bad
    cmp edx, SEMANTIC_TYPE_VERSION_1
    jne .release_bad
    cmp ebx, SEMANTIC_VALIDATION_DECLARED
    jne .release_bad
    ; has_type: Primary muss gefunden werden
    mov eax, [semantic_test_object]
    mov ecx, SEMANTIC_IPC_INLINE_DATA
    call object_semantic_has_type
    jc .release_bad
    ; has_type: Secondary muss gefunden werden
    mov eax, [semantic_test_object]
    mov ecx, SEMANTIC_IPC_DIAGNOSTIC
    call object_semantic_has_type
    jc .release_bad
    ; Duplikat-Secondary muss abgewiesen werden
    mov eax, [semantic_test_object]
    mov ecx, SEMANTIC_IPC_DIAGNOSTIC
    mov edx, SEMANTIC_TYPE_VERSION_1
    mov ebx, SEMANTIC_VALIDATION_DECLARED
    call object_semantic_attach_secondary
    jnc .release_bad
    ; Secondary == Primary muss abgewiesen werden
    mov eax, [semantic_test_object]
    mov ecx, SEMANTIC_IPC_INLINE_DATA
    mov edx, SEMANTIC_TYPE_VERSION_1
    mov ebx, SEMANTIC_VALIDATION_DECLARED
    call object_semantic_attach_secondary
    jnc .release_bad
    ; Secondary-Count == 1
    mov eax, [semantic_test_object]
    call object_semantic_secondary_count
    jc .release_bad
    cmp eax, 1
    jne .release_bad
    ; Freigabe: Secondary-Sidecar explizit zurücksetzen, dann Objekt freigeben
    mov eax, [semantic_test_object]
    call object_semantic_clear_secondary
    mov eax, [semantic_test_object]
    call object_release
    jc .bad
    ; Nach Freigabe: object_lookup ungültig → Secondary-Abfrage muss scheitern
    mov eax, [semantic_test_object]
    xor ecx, ecx
    call object_semantic_query_secondary
    jnc .bad                    ; muss scheitern – Objekt freigegeben
    ; §120: Konvertierungsausführung INLINE_DATA→DIAGNOSTIC
    ; Quellwert aus semantic_validation_test_value (0x41564F4E "NOVA")
    mov dword [semantic_exec_src_type],    SEMANTIC_IPC_INLINE_DATA
    mov dword [semantic_exec_src_version], SEMANTIC_TYPE_VERSION_1
    mov dword [semantic_exec_tgt_type],    SEMANTIC_IPC_DIAGNOSTIC
    mov dword [semantic_exec_tgt_version], SEMANTIC_TYPE_VERSION_1
    lea eax, [semantic_validation_test_value]
    mov [semantic_exec_src_ptr], eax
    mov dword [semantic_exec_src_len],  4
    lea eax, [semantic_exec_dst_buffer]
    mov [semantic_exec_dst_ptr], eax
    mov dword [semantic_exec_dst_max],  4
    call semantic_execute_conversion
    jc .bad                              ; muss gelingen
    cmp eax, 4
    jne .bad                             ; 4 Bytes geschrieben
    ; Zielinhalt muss Quellwert enthalten (NOVA = 0x41564F4E)
    lea eax, [semantic_exec_dst_buffer]
    mov ecx, [eax]
    cmp ecx, 0x41564F4E
    jne .bad
    ; §120: Umgekehrte Richtung (DIAGNOSTIC→INLINE_DATA) nicht registriert → Fehler
    mov dword [semantic_exec_src_type],    SEMANTIC_IPC_DIAGNOSTIC
    mov dword [semantic_exec_src_version], SEMANTIC_TYPE_VERSION_1
    mov dword [semantic_exec_tgt_type],    SEMANTIC_IPC_INLINE_DATA
    mov dword [semantic_exec_tgt_version], SEMANTIC_TYPE_VERSION_1
    call semantic_execute_conversion
    jnc .bad                             ; muss scheitern – kein Konvertierungspfad
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
; §119: Sekundäre Typ-Sidecars (SEMANTIC_OBJECT_CAPACITY × SEMANTIC_SECONDARY_CAPACITY Einträge)
align 4
object_semantic_secondary_types:
    times SEMANTIC_OBJECT_CAPACITY * SEMANTIC_SECONDARY_CAPACITY dd 0
object_semantic_secondary_versions:
    times SEMANTIC_OBJECT_CAPACITY * SEMANTIC_SECONDARY_CAPACITY dd 0
object_semantic_secondary_validation:
    times SEMANTIC_OBJECT_CAPACITY * SEMANTIC_SECONDARY_CAPACITY dd 0
object_semantic_secondary_count_array:
    times SEMANTIC_OBJECT_CAPACITY dd 0
; §120: Handler-Registry und Ausführungs-Scratch
align 4
semantic_handler_count:       dd 0
semantic_handler_reg_cap:     dd 0
semantic_handler_reg_ptr:     dd 0
semantic_exec_src_type:       dd 0
semantic_exec_src_version:    dd 0
semantic_exec_tgt_type:       dd 0
semantic_exec_tgt_version:    dd 0
semantic_exec_capability:     dd 0
semantic_exec_src_ptr:        dd 0
semantic_exec_src_len:        dd 0
semantic_exec_dst_ptr:        dd 0
semantic_exec_dst_max:        dd 0
semantic_exec_dst_written:    dd 0
semantic_exec_dst_buffer:     dd 0
semantic_handler_records:
    times SEMANTIC_HANDLER_CAPACITY * SEMANTIC_HANDLER_SIZE db 0
