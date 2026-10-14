; ---------------------------------------------------------------------------
; Nova Trust-Subsystem Â§022
; NPSPEC-TRUST-0001           - Trust-Grundmodell
; NPSPEC-TRUST-AGENT-0001     - Agent-Trust
; NPSPEC-TRUST-ANCHOR-0001    - Trust-Anker
; NPSPEC-TRUST-ATTESTATION-0001 - Attestierung
; NPSPEC-TRUST-AUDIT-0001     - Auditierung
; NPSPEC-TRUST-IDENTITY-0001  - Identitaets-Trust
; NPSPEC-TRUST-INTROSPECTION-0001 - Introspection
; NPSPEC-TRUST-MEDIA-0001     - Medien-Trust
; NPSPEC-TRUST-POLICY-0001    - Trust-Policy
; NPSPEC-TRUST-PROVENANCE-0001 - Herkunft
; NPSPEC-TRUST-REVOCATION-0001 - Widerruf
; NPSPEC-TRUST-SIGNATURE-0001 - Signatur-Verifizierung
; NPSPEC-TRUST-SOFTWARE-0001  - Software-Trust
; NPSPEC-TRUST-SUPPLYCHAIN-0001 - Supply-Chain
; ---------------------------------------------------------------------------

; Trust-Stufen (NPSPEC-TRUST-0001)
TRUST_LEVEL_NONE      equ 0    ; Kein Vertrauen
TRUST_LEVEL_LOW       equ 1    ; Begrenzt
TRUST_LEVEL_MEDIUM    equ 2    ; Benutzer-vertraut
TRUST_LEVEL_HIGH      equ 3    ; System-vertraut
TRUST_LEVEL_ANCHOR    equ 4    ; Trust-Anker (NPSPEC-TRUST-ANCHOR-0001)

; Trust-Quellen
TRUST_SRC_UNKNOWN     equ 0
TRUST_SRC_KERNEL      equ 1    ; Kernel-intern
TRUST_SRC_SIGNATURE   equ 2    ; (NPSPEC-TRUST-SIGNATURE-0001)
TRUST_SRC_ATTESTATION equ 3    ; (NPSPEC-TRUST-ATTESTATION-0001)
TRUST_SRC_SUPPLY_CHAIN equ 4   ; (NPSPEC-TRUST-SUPPLYCHAIN-0001)
TRUST_SRC_MEDIA       equ 5    ; (NPSPEC-TRUST-MEDIA-0001)

; Trust-Flags
TRUST_FLAG_REVOCABLE  equ 0x01  ; (NPSPEC-TRUST-REVOCATION-0001)
TRUST_FLAG_AUDITED    equ 0x02  ; (NPSPEC-TRUST-AUDIT-0001)
TRUST_FLAG_AGENT      equ 0x04  ; (NPSPEC-TRUST-AGENT-0001)
TRUST_FLAG_SOFTWARE   equ 0x08  ; (NPSPEC-TRUST-SOFTWARE-0001)
TRUST_FLAG_REVOKED    equ 0x80

; Trust-Record (32 Bytes)
TRUST_REC_SIZE        equ 32
TR_SUBJECT_ID         equ 0    ; Subjekt-ID (Programm/Identitaet/Volume)
TR_LEVEL              equ 4    ; TRUST_LEVEL_*
TR_SOURCE             equ 8    ; TRUST_SRC_*
TR_FLAGS              equ 12
TR_SIGN_HASH          equ 16   ; Signatur-Hash (NPSPEC-TRUST-SIGNATURE-0001)
TR_PROV_HASH          equ 20   ; Provenance-Hash (NPSPEC-TRUST-PROVENANCE-0001)
TR_POLICY_MASK        equ 24   ; erlaubte Ops (NPSPEC-TRUST-POLICY-0001)
TR_ATTEST_TIME        equ 28   ; Attestierungszeitpunkt

TRUST_CAPACITY        equ 16

; Audit-Record (16 Bytes, NPSPEC-TRUST-AUDIT-0001)
TRUST_AUDIT_REC_SIZE  equ 16
TA_SUBJECT_ID         equ 0
TA_EVENT              equ 4    ; 0=grant 1=revoke 2=check
TA_RESULT             equ 8    ; 0=ok 1=denied
TA_TIMESTAMP          equ 12

TRUST_AUDIT_CAPACITY  equ 16

; ---------------------------------------------------------------------------
; trust_mgr_initialize
; ---------------------------------------------------------------------------
trust_mgr_initialize:
    push edi
    mov edi, trust_mgr_table
    xor eax, eax
    mov ecx, (TRUST_CAPACITY * TRUST_REC_SIZE) / 4
    rep stosd
    mov edi, trust_audit_table
    mov ecx, (TRUST_AUDIT_CAPACITY * TRUST_AUDIT_REC_SIZE) / 4
    rep stosd
    pop edi
    mov dword [trust_mgr_count], 0
    mov dword [trust_audit_count], 0

    ; Kernel-Trust-Anker (NPSPEC-TRUST-ANCHOR-0001)
    mov eax, 0              ; Kernel-Subjekt
    mov ebx, TRUST_LEVEL_ANCHOR
    mov ecx, TRUST_SRC_KERNEL
    mov edx, TRUST_FLAG_AUDITED | TRUST_FLAG_SOFTWARE
    call trust_grant
    jc .fail

    mov dword [trust_mgr_ready], 1
    clc
    ret
.fail:
    stc
    ret

; ---------------------------------------------------------------------------
; trust_grant: EAX=subject_id, EBX=level, ECX=source, EDX=flags
; (NPSPEC-TRUST-0001)
; ---------------------------------------------------------------------------
trust_grant:
    cmp dword [trust_mgr_count], TRUST_CAPACITY
    jae .full
    push edi
    push esi
    mov edi, trust_mgr_table
    push eax
    mov eax, [trust_mgr_count]
    imul eax, TRUST_REC_SIZE
    add edi, eax
    pop eax
    mov [edi + TR_SUBJECT_ID], eax
    mov [edi + TR_LEVEL], ebx
    mov [edi + TR_SOURCE], ecx
    mov [edi + TR_FLAGS], edx
    mov dword [edi + TR_SIGN_HASH], 0
    mov dword [edi + TR_PROV_HASH], 0
    mov dword [edi + TR_POLICY_MASK], 0xFFFFFFFF
    push eax
    mov eax, [timer_ticks]
    mov [edi + TR_ATTEST_TIME], eax
    pop eax
    inc dword [trust_mgr_count]
    ; Audit-Eintrag (NPSPEC-TRUST-AUDIT-0001)
    push eax
    push ebx
    mov ebx, 0          ; event=grant
    call trust_audit_append
    pop ebx
    pop eax
    pop esi
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; trust_check: EAX=subject_id, EBX=min_level -> ECX=actual_level, CF=0 ok
; ---------------------------------------------------------------------------
trust_check:
    push esi
    push edi
    mov esi, trust_mgr_table
    push ebx
    mov ebx, [trust_mgr_count]
.scan:
    test ebx, ebx
    jz .not_found
    cmp [esi + TR_SUBJECT_ID], eax
    je .found
    add esi, TRUST_REC_SIZE
    dec ebx
    jmp .scan
.found:
    pop ebx
    ; Widerrufen?
    test dword [esi + TR_FLAGS], TRUST_FLAG_REVOKED
    jnz .revoked
    mov ecx, [esi + TR_LEVEL]
    cmp ecx, ebx
    jl .insufficient
    pop edi
    pop esi
    clc
    ret
.insufficient:
.revoked:
    pop edi
    pop esi
    stc
    ret
.not_found:
    pop ebx
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; trust_revoke: EAX=subject_id (NPSPEC-TRUST-REVOCATION-0001)
; ---------------------------------------------------------------------------
trust_revoke:
    push esi
    push ecx
    mov esi, trust_mgr_table
    mov ecx, [trust_mgr_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + TR_SUBJECT_ID], eax
    je .found
    add esi, TRUST_REC_SIZE
    dec ecx
    jmp .scan
.found:
    or dword [esi + TR_FLAGS], TRUST_FLAG_REVOKED
    pop ecx
    pop esi
    ; Audit-Eintrag
    push ebx
    mov ebx, 1      ; event=revoke
    call trust_audit_append
    pop ebx
    clc
    ret
.not_found:
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; trust_audit_append: EAX=subject_id, EBX=event (intern)
; ---------------------------------------------------------------------------
trust_audit_append:
    cmp dword [trust_audit_count], TRUST_AUDIT_CAPACITY
    jae .full
    push edi
    push ecx
    mov edi, trust_audit_table
    mov ecx, [trust_audit_count]
    imul ecx, TRUST_AUDIT_REC_SIZE
    add edi, ecx
    mov [edi + TA_SUBJECT_ID], eax
    mov [edi + TA_EVENT], ebx
    mov dword [edi + TA_RESULT], 0
    push eax
    mov eax, [timer_ticks]
    mov [edi + TA_TIMESTAMP], eax
    pop eax
    inc dword [trust_audit_count]
    pop ecx
    pop edi
.full:
    ret

; ---------------------------------------------------------------------------
; trust_mgr_self_test
; ---------------------------------------------------------------------------
trust_mgr_self_test:
    push esi
    xor esi, esi
    cmp dword [trust_mgr_ready], 1
    je .t2
    inc esi
.t2:
    ; Kernel-Anchor hat Level ANCHOR
    mov eax, 0
    mov ebx, TRUST_LEVEL_HIGH
    call trust_check
    jnc .t3
    inc esi
.t3:
    ; Neues Medium-Trust
    mov eax, 0x42
    mov ebx, TRUST_LEVEL_MEDIUM
    mov ecx, TRUST_SRC_SIGNATURE
    mov edx, TRUST_FLAG_REVOCABLE | TRUST_FLAG_SOFTWARE
    call trust_grant
    jnc .t4
    inc esi
.t4:
    ; Widerrufen
    mov eax, 0x42
    call trust_revoke
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
trust_mgr_ready:        dd 0
trust_mgr_count:        dd 0
trust_audit_count:  dd 0

align 4
trust_mgr_table:
    times TRUST_CAPACITY * TRUST_REC_SIZE db 0

align 4
trust_audit_table:
    times TRUST_AUDIT_CAPACITY * TRUST_AUDIT_REC_SIZE db 0

