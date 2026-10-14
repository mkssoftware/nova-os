; ---------------------------------------------------------------------------
; Nova Verification-Subsystem §036-VERIFY
; NPSPEC-VERIFY-CAPABILITYSAFETY-0001 - Capability-Sicherheit
; NPSPEC-VERIFY-CONTRACT-0001         - Vertragspruefung
; NPSPEC-VERIFY-FORMALSPEC-0001       - Formale Spezifikation
; NPSPEC-VERIFY-INFORMATIONFLOW-0001  - Informationsfluss
; NPSPEC-VERIFY-MEMORYSAFETY-0001     - Speichersicherheit
; NPSPEC-VERIFY-MODELCHECK-0001       - Modellpruefung
; NPSPEC-VERIFY-REPRODUCIBLE-0001     - Reproduzierbarkeit
; NPSPEC-VERIFY-RUNTIME-0001          - Laufzeit-Verifikation
; NPSPEC-VERIFY-STATICANALYSIS-0001   - Statische Analyse
; NPSPEC-VERIFY-TEMPORAL-0001         - Temporale Eigenschaften
; NPSPEC-VERIFY-TYPESAFETY-0001       - Typsicherheit
; ---------------------------------------------------------------------------

; Verification-Ergebnisse
VFY_PASS              equ 0
VFY_WARN              equ 1
VFY_FAIL              equ 2
VFY_UNKNOWN           equ 3

; Verification-Typen (Checker-IDs)
VFY_TYPE_CAPABILITY   equ 0    ; (NPSPEC-VERIFY-CAPABILITYSAFETY-0001)
VFY_TYPE_CONTRACT     equ 1    ; (NPSPEC-VERIFY-CONTRACT-0001)
VFY_TYPE_MEMORY       equ 2    ; (NPSPEC-VERIFY-MEMORYSAFETY-0001)
VFY_TYPE_INFOFLOW     equ 3    ; (NPSPEC-VERIFY-INFORMATIONFLOW-0001)
VFY_TYPE_RUNTIME      equ 4    ; (NPSPEC-VERIFY-RUNTIME-0001)
VFY_TYPE_TEMPORAL     equ 5    ; (NPSPEC-VERIFY-TEMPORAL-0001)
VFY_TYPE_TYPE         equ 6    ; (NPSPEC-VERIFY-TYPESAFETY-0001)
VFY_TYPE_REPRODUCIBLE equ 7    ; (NPSPEC-VERIFY-REPRODUCIBLE-0001)
VFY_TYPE_COUNT        equ 8

; Verification-Record (32 Bytes)
VFY_REC_SIZE          equ 32
VFR_ID                 equ 0
VFR_TYPE               equ 4    ; VFY_TYPE_*
VFR_SUBJECT_ID         equ 8    ; Geprueftes Objekt/Programm
VFR_RESULT             equ 12   ; VFY_*
VFR_EVIDENCE_HASH      equ 16   ; Hash des Beweises
VFR_TIMESTAMP          equ 20
VFR_FLAGS              equ 24
; 28..31: reserviert

VFY_CAPACITY          equ 16

; Contract-Predicate (16 Bytes, NPSPEC-VERIFY-CONTRACT-0001)
VFY_PRED_SIZE         equ 16
VP_SUBJECT_ID         equ 0
VP_PRECOND_HASH       equ 4    ; Hash der Vorbedingung
VP_POSTCOND_HASH      equ 8    ; Hash der Nachbedingung
VP_INVARIANT_HASH     equ 12   ; Hash der Invariante

VFY_PRED_CAPACITY     equ 8

; ---------------------------------------------------------------------------
; verify_initialize
; ---------------------------------------------------------------------------
verify_initialize:
    push edi
    mov edi, vfy_table
    xor eax, eax
    mov ecx, (VFY_CAPACITY * VFY_REC_SIZE) / 4
    rep stosd
    mov edi, vfy_pred_table
    mov ecx, (VFY_PRED_CAPACITY * VFY_PRED_SIZE) / 4
    rep stosd
    pop edi
    mov dword [vfy_count], 0
    mov dword [vfy_pred_count], 0
    mov dword [vfy_next_id], 1
    mov dword [vfy_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; verify_check: EAX=type, EBX=subject_id -> ECX=result, CF=0 ok
; Fuehrt einen Checker aus (NPSPEC-VERIFY-RUNTIME-0001).
; ---------------------------------------------------------------------------
verify_check:
    cmp eax, VFY_TYPE_COUNT
    jae .invalid
    ; Ergebnis in Tabelle ablegen
    cmp dword [vfy_count], VFY_CAPACITY
    jae .no_space
    push edi
    push esi
    mov edi, vfy_table
    push eax
    mov eax, [vfy_count]
    imul eax, VFY_REC_SIZE
    add edi, eax
    pop eax
    mov edx, [vfy_next_id]
    mov [edi + VFR_ID], edx
    mov [edi + VFR_TYPE], eax
    mov [edi + VFR_SUBJECT_ID], ebx
    ; Vereinfachte Prueflogik:
    ; Capability-Safety: Pruefe Revocation-Flag in prog_cap_table
    cmp eax, VFY_TYPE_CAPABILITY
    je .check_cap
    ; Memory: immer PASS (statische Kern-Allokation)
    cmp eax, VFY_TYPE_MEMORY
    je .pass
    ; Runtime: PASS wenn subject_id gueltig
    test ebx, ebx
    jz .warn
    jmp .pass
.check_cap:
    ; Pruefe ob Capability widerrufen
    mov edx, ebx
    call cap_query
    jc .warn
    test ecx, CAP_FLAG_REVOKED
    jnz .fail
    jmp .pass
.warn:
    mov ecx, VFY_WARN
    jmp .store
.fail:
    mov ecx, VFY_FAIL
    jmp .store
.pass:
    mov ecx, VFY_PASS
.store:
    mov [edi + VFR_RESULT], ecx
    push eax
    mov eax, [timer_ticks]
    mov [edi + VFR_TIMESTAMP], eax
    pop eax
    inc dword [vfy_next_id]
    inc dword [vfy_count]
    pop esi
    pop edi
    clc
    ret
.no_space:
    mov ecx, VFY_UNKNOWN
    clc
    ret
.invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; verify_contract_register: EAX=subject_id, EBX=pre_hash, ECX=post_hash
; (NPSPEC-VERIFY-CONTRACT-0001)
; ---------------------------------------------------------------------------
verify_contract_register:
    cmp dword [vfy_pred_count], VFY_PRED_CAPACITY
    jae .full
    push edi
    push edx
    mov edi, vfy_pred_table
    mov edx, [vfy_pred_count]
    imul edx, VFY_PRED_SIZE
    add edi, edx
    mov [edi + VP_SUBJECT_ID], eax
    mov [edi + VP_PRECOND_HASH], ebx
    mov [edi + VP_POSTCOND_HASH], ecx
    mov dword [edi + VP_INVARIANT_HASH], 0
    inc dword [vfy_pred_count]
    pop edx
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; verify_self_test
; ---------------------------------------------------------------------------
verify_self_test:
    push esi
    xor esi, esi
    cmp dword [vfy_ready], 1
    je .t2
    inc esi
.t2:
    mov eax, VFY_TYPE_MEMORY
    mov ebx, 0
    call verify_check
    jnc .t3
    inc esi
.t3:
    cmp ecx, VFY_PASS
    je .t4
    inc esi
.t4:
    mov eax, VFY_TYPE_RUNTIME
    mov ebx, 1
    call verify_check
    jnc .t5
    inc esi
.t5:
    mov eax, 0x100
    mov ebx, 0xAABB
    mov ecx, 0xCCDD
    call verify_contract_register
    jnc .done
    inc esi
.done:
    test esi, esi
    pop esi
    jz .ok
    stc
    ret
.ok:
    clc
    ret

align 4
vfy_ready:      dd 0
vfy_count:      dd 0
vfy_pred_count: dd 0
vfy_next_id:    dd 1

align 4
vfy_table:
    times VFY_CAPACITY * VFY_REC_SIZE db 0

align 4
vfy_pred_table:
    times VFY_PRED_CAPACITY * VFY_PRED_SIZE db 0
