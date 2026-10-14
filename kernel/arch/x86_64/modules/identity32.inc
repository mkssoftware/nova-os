; ---------------------------------------------------------------------------
; Nova Identity-Subsystem Â§021
; NPSPEC-IDENTITY-0001            - Identitaetsmodell
; NPSPEC-IDENTITY-APPLICATION-0001- Anwendungsidentitaet
; NPSPEC-IDENTITY-CREDENTIAL-0001 - Credentials
; NPSPEC-IDENTITY-DEVICE-0001     - Geraeteidentitaet
; NPSPEC-IDENTITY-FEDERATION-0001 - Identity-Federation
; NPSPEC-IDENTITY-LIFECYCLE-0001  - Lebenszyklus
; NPSPEC-IDENTITY-LOCAL-0001      - Lokale Identitaet
; NPSPEC-IDENTITY-MFA-0001        - Multi-Faktor-Authentisierung
; NPSPEC-IDENTITY-PASSKEY-0001    - Passkey-Unterst
; NPSPEC-IDENTITY-RECOVERY-0001   - Wiederherstellung
; NPSPEC-IDENTITY-SERVICE-0001    - Service-Identitaet
; NPSPEC-IDENTITY-STEPUP-0001     - Step-Up-Authentisierung
; NPSPEC-IDENTITY-WORKLOAD-0001   - Workload-Identitaet
; ---------------------------------------------------------------------------

; Identitaetstypen (NPSPEC-IDENTITY-0001)
IDENTMGR_TYPE_USER       equ 0    ; Benutzerkonto
IDENTMGR_TYPE_DEVICE     equ 1    ; (NPSPEC-IDENTITY-DEVICE-0001)
IDENTMGR_TYPE_SERVICE    equ 2    ; (NPSPEC-IDENTITY-SERVICE-0001)
IDENT_TYPE_APP        equ 3    ; (NPSPEC-IDENTITY-APPLICATION-0001)
IDENT_TYPE_WORKLOAD   equ 4    ; (NPSPEC-IDENTITY-WORKLOAD-0001)

; Zustand (NPSPEC-IDENTITY-LIFECYCLE-0001)
IDENTMGR_STATE_ACTIVE    equ 0
IDENT_STATE_INACTIVE  equ 1
IDENT_STATE_LOCKED    equ 2
IDENTMGR_STATE_REVOKED   equ 3

; Credential-Typen (NPSPEC-IDENTITY-CREDENTIAL-0001)
CRED_TYPE_PASSWORD    equ 0
CRED_TYPE_PASSKEY     equ 1    ; (NPSPEC-IDENTITY-PASSKEY-0001)
CRED_TYPE_TOKEN       equ 2
CRED_TYPE_CERT        equ 3

; MFA-Faktoren (NPSPEC-IDENTITY-MFA-0001)
MFA_FACTOR_NONE       equ 0x00
MFA_FACTOR_PASSWORD   equ 0x01
MFA_FACTOR_TOTP       equ 0x02
MFA_FACTOR_PASSKEY    equ 0x04
MFA_FACTOR_HARDWARE   equ 0x08

; Identitaets-Record (48 Bytes)
IDENTMGR_REC_SIZE        equ 48
ID_ID                 equ 0    ; Identitaets-ID
ID_TYPE               equ 4    ; IDENT_TYPE_*
ID_STATE              equ 8    ; IDENT_STATE_*
ID_CRED_TYPE          equ 12   ; CRED_TYPE_*
ID_MFA_MASK           equ 16   ; aktive Faktoren
ID_CRED_HASH          equ 20   ; Credential-Hash (kein Klartext)
ID_FEDERATION_ID      equ 24   ; Fremde IdP-ID (NPSPEC-IDENTITY-FEDERATION-0001)
ID_STEPUP_LEVEL       equ 28   ; (NPSPEC-IDENTITY-STEPUP-0001) 0=basis
ID_RECOVERY_HASH      equ 32   ; (NPSPEC-IDENTITY-RECOVERY-0001)
ID_AUTH_TIME          equ 36   ; Letzte Authentisierung (ticks)
ID_FLAGS              equ 40   ; Local-Flag etc.
; 44..47: reserviert

IDENTMGR_CAPACITY        equ 8

; ---------------------------------------------------------------------------
; identity_mgr_initialize
; Registriert System-Geraeteidentitaet (NPSPEC-IDENTITY-DEVICE-0001).
; CF=0 Erfolg
; ---------------------------------------------------------------------------
identity_mgr_initialize:
    push edi
    mov edi, identmgr_table
    xor eax, eax
    mov ecx, (IDENTMGR_CAPACITY * IDENTMGR_REC_SIZE) / 4
    rep stosd
    pop edi
    mov dword [identmgr_count], 0
    mov dword [identmgr_next_id], 1

    ; System-Geraete-Identitaet anlegen
    mov eax, 0              ; ID 0
    mov ebx, IDENTMGR_TYPE_DEVICE
    mov ecx, CRED_TYPE_CERT
    mov edx, MFA_FACTOR_HARDWARE
    call ident_register
    jc .fail

    mov dword [identmgr_ready], 1
    clc
    ret
.fail:
    stc
    ret

; ---------------------------------------------------------------------------
; ident_register: EAX=id, EBX=type, ECX=cred_type, EDX=mfa_mask
; ---------------------------------------------------------------------------
ident_register:
    cmp dword [identmgr_count], IDENTMGR_CAPACITY
    jae .full
    push edi
    push esi
    mov edi, identmgr_table
    push eax
    mov eax, [identmgr_count]
    imul eax, IDENTMGR_REC_SIZE
    add edi, eax
    pop eax
    mov [edi + ID_ID], eax
    mov [edi + ID_TYPE], ebx
    mov dword [edi + ID_STATE], IDENTMGR_STATE_ACTIVE
    mov [edi + ID_CRED_TYPE], ecx
    mov [edi + ID_MFA_MASK], edx
    mov dword [edi + ID_CRED_HASH], 0
    mov dword [edi + ID_FEDERATION_ID], 0xFFFFFFFF
    mov dword [edi + ID_STEPUP_LEVEL], 0
    mov dword [edi + ID_RECOVERY_HASH], 0
    mov eax, [timer_ticks]
    mov [edi + ID_AUTH_TIME], eax
    mov dword [edi + ID_FLAGS], 0
    inc dword [identmgr_count]
    pop esi
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; ident_authenticate: EAX=id, EBX=cred_hash, ECX=mfa_presented
; -> CF=0 authentifiziert
; (NPSPEC-IDENTITY-MFA-0001, NPSPEC-IDENTITY-STEPUP-0001)
; ---------------------------------------------------------------------------
ident_authenticate:
    push esi
    push edi
    mov esi, identmgr_table
    push ecx
    mov ecx, [identmgr_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + ID_ID], eax
    je .found
    add esi, IDENTMGR_REC_SIZE
    dec ecx
    jmp .scan
.found:
    pop ecx
    ; Zustand pruefen
    cmp dword [esi + ID_STATE], IDENTMGR_STATE_ACTIVE
    jne .denied
    ; Credential-Hash vergleichen (0=beliebig fuer System-Geraet)
    cmp dword [esi + ID_CRED_HASH], 0
    je .check_mfa
    cmp [esi + ID_CRED_HASH], ebx
    jne .denied
.check_mfa:
    ; MFA: alle geforderten Faktoren vorhanden?
    mov edi, [esi + ID_MFA_MASK]
    and edi, ecx
    cmp edi, [esi + ID_MFA_MASK]
    jne .denied
    ; Authentisierungszeit aktualisieren
    push eax
    mov eax, [timer_ticks]
    mov [esi + ID_AUTH_TIME], eax
    pop eax
    pop edi
    pop esi
    clc
    ret
.not_found:
    pop ecx
.denied:
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; ident_revoke: EAX=id (NPSPEC-IDENTITY-LIFECYCLE-0001)
; ---------------------------------------------------------------------------
ident_revoke:
    push esi
    push ecx
    mov esi, identmgr_table
    mov ecx, [identmgr_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + ID_ID], eax
    je .found
    add esi, IDENTMGR_REC_SIZE
    dec ecx
    jmp .scan
.found:
    mov dword [esi + ID_STATE], IDENTMGR_STATE_REVOKED
    pop ecx
    pop esi
    clc
    ret
.not_found:
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; identity_mgr_self_test
; ---------------------------------------------------------------------------
identity_mgr_self_test:
    push esi
    xor esi, esi
    cmp dword [identmgr_ready], 1
    je .t2
    inc esi
.t2:
    ; Neue App-Identitaet
    mov eax, 1
    mov ebx, IDENT_TYPE_APP
    mov ecx, CRED_TYPE_TOKEN
    mov edx, MFA_FACTOR_NONE
    call ident_register
    jnc .t3
    inc esi
.t3:
    ; Authentisierung (kein Hash-Check)
    mov eax, 1
    xor ebx, ebx
    mov ecx, MFA_FACTOR_NONE
    call ident_authenticate
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
identmgr_ready:    dd 0
identmgr_count:    dd 0
identmgr_next_id:  dd 1

align 4
identmgr_table:
    times IDENTMGR_CAPACITY * IDENTMGR_REC_SIZE db 0

