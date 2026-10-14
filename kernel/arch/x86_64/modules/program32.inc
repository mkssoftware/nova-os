; ---------------------------------------------------------------------------
; Nova Program-Manager, Registry und Capability-System §038
;
; NPSPEC-PROGRAM-COMPATIBILITY-0001  - Kompatibilitaet
; NPSPEC-PROGRAM-DEPENDENCY-0001/2   - Abhaengigkeiten
; NPSPEC-PROGRAM-INSTALL-0001        - Installation
; NPSPEC-PROGRAM-INSTALLSCOPE-0001   - Installationsbereich
; NPSPEC-PROGRAM-LAYOUT-0001         - Verzeichnis-Layout
; NPSPEC-PROGRAM-LEGACY-0001         - Legacy-Unterstuetzung
; NPSPEC-PROGRAM-MANIFEST-0001       - Paket-Manifest
; NPSPEC-PROGRAM-NAMESPACE-0001      - Program-Namespace
; NPSPEC-PROGRAM-PACKAGE-0001        - Paket-Format
; NPSPEC-PROGRAM-PERMISSION-0001     - Berechtigungen
; NPSPEC-PROGRAM-ROLLBACK-0001       - Rollback
; NPSPEC-PROGRAM-SYS-0001            - System-Programme
; NPSPEC-PROGRAM-SYSOVERLAY-0001     - System-Overlay
; NPSPEC-PROGRAM-TRUST-0001          - Vertrauensmodell
; NPSPEC-PROGRAM-UNINSTALL-0001      - Deinstallation
; NPSPEC-PROGRAM-UPDATE-0001         - Updates
; NPSPEC-PROGRAM-INSTALLSCOPE-0001   - Scope (System/User/Workspace)
;
; NPSPEC-REGISTRY-ALGORITHM-0001     - Algorithmus-Registry
; NPSPEC-REGISTRY-CAPABILITY-0001    - Capability-Registry
; NPSPEC-REGISTRY-CODEC-0001         - Codec-Registry
; NPSPEC-REGISTRY-DEVICE-0001        - Geraete-Registry
; NPSPEC-REGISTRY-INTEGRITY-0001     - Integritaets-Pruefung
; NPSPEC-REGISTRY-QUERY-0001         - Registry-Abfrage
; NPSPEC-REGISTRY-SERVICE-0001       - Service-Registry
; NPSPEC-REGISTRY-TYPE-0001          - Typ-Registry
; NPSPEC-REGISTRY-UPDATE-0001        - Registry-Updates
;
; NPSPEC-CAPABILITY-ACTIVATION-0001  - Capability-Aktivierung
; NPSPEC-CAPABILITY-CATEGORY-0001    - Kategorien
; NPSPEC-CAPABILITY-COMPATIBILITY-0001 - ABI-Kompatibilitaet
; NPSPEC-CAPABILITY-DEACTIVATION-0001 - Deaktivierung
; NPSPEC-CAPABILITY-ID-0001          - Eindeutige IDs
; NPSPEC-CAPABILITY-IMPLEMENTATION-0001 - Implementierung
; NPSPEC-CAPABILITY-INPUT-0001       - Eingabe-Capabilities
; NPSPEC-CAPABILITY-INTERFACE-0001   - Schnittstelle
; NPSPEC-CAPABILITY-MANIFEST-0001    - Capability-Manifest
; NPSPEC-CAPABILITY-NAMESPACE-0001   - Namespace-Binding
; NPSPEC-CAPABILITY-OUTPUT-0001      - Ausgabe-Capabilities
; NPSPEC-CAPABILITY-PERMISSIONS-0001 - Berechtigungs-Checks
; NPSPEC-CAPABILITY-PLATFORM-0001    - Plattform-Capabilities
; NPSPEC-CAPABILITY-PROVIDER-0001    - Anbieter
; NPSPEC-CAPABILITY-QUERY-0001       - Abfrage
; NPSPEC-CAPABILITY-REGISTRY-0001    - Registry
; NPSPEC-CAPABILITY-REQUIRE-0001     - Anforderungen
; NPSPEC-CAPABILITY-RESOLVER-0001    - Aufloesung
; NPSPEC-CAPABILITY-REVOKE-0001      - Widerruf
; NPSPEC-CAPABILITY-RUNTIME-0001     - Laufzeit-Capabilities
; NPSPEC-CAPABILITY-SANDBOX-0001     - Sandbox-Begrenzung
; NPSPEC-CAPABILITY-SECURITY-0001    - Sicherheitsmodell
; NPSPEC-CAPABILITY-SHIM-0001        - Shim-Layer
; NPSPEC-CAPABILITY-SIGNING-0001     - Signierung
; NPSPEC-CAPABILITY-TRANSFER-0001    - Weitergabe
; NPSPEC-CAPABILITY-VERSIONING-0001  - Versionierung
; ---------------------------------------------------------------------------

; ---------------------------------------------------------------------------
; Program-Install-Scope (NPSPEC-PROGRAM-INSTALLSCOPE-0001)
; ---------------------------------------------------------------------------
PROG_SCOPE_SYSTEM     equ 0    ; Systemweit (/prog/sys)
PROG_SCOPE_USER       equ 1    ; Benutzer (/prog/user)
PROG_SCOPE_WORKSPACE  equ 2    ; Workspace (/prog/ws)

; Program-Zustaende
PROG_STATE_ABSENT     equ 0
PROG_STATE_INSTALLED  equ 1
PROG_STATE_UPDATING   equ 2
PROG_STATE_ROLLBACK   equ 3   ; (NPSPEC-PROGRAM-ROLLBACK-0001)
PROG_STATE_PENDING    equ 4
PROG_STATE_BROKEN     equ 5

; Program-Flags
PROG_FLAG_SYSTEM      equ 0x01  ; (NPSPEC-PROGRAM-SYS-0001)
PROG_FLAG_LEGACY      equ 0x02  ; (NPSPEC-PROGRAM-LEGACY-0001)
PROG_FLAG_OVERLAY     equ 0x04  ; (NPSPEC-PROGRAM-SYSOVERLAY-0001)
PROG_FLAG_TRUSTED     equ 0x08  ; (NPSPEC-PROGRAM-TRUST-0001)
PROG_FLAG_HAS_CAPS    equ 0x10  ; Hat Capabilities

; Program-Record (64 Bytes)
PROG_REC_SIZE         equ 64
PKG_ID                equ 0    ; uint32 Package-ID
PKG_FLAGS             equ 4
PKG_STATE             equ 8
PKG_SCOPE             equ 12
PKG_VERSION           equ 16   ; uint32 packed: [maj8][min8][patch16]
PKG_MIN_VERSION       equ 20   ; Mindestversion fuer Kompatibilitaet
PKG_NS_ID             equ 24   ; Namespace-Bind (NPSPEC-PROGRAM-NAMESPACE-0001)
PKG_CAP_MASK_LO       equ 28   ; Capability-Bitmaske (64 Bit)
PKG_CAP_MASK_HI       equ 32
PKG_TRUST_HASH        equ 36   ; XOR-Hash der Paket-Signatur
PKG_DEP_COUNT         equ 40   ; Anzahl Abhaengigkeiten
PKG_PERM_MASK         equ 44   ; FSPOLICY_*
; 48..63: reserviert

PROG_CAPACITY         equ 8

; ---------------------------------------------------------------------------
; Registry-Eintrag (NPSPEC-REGISTRY-*-0001)
; Ein universeller Key-Value-Store fuer Algorithmen, Codecs, Devices, Services
; ---------------------------------------------------------------------------
REG_KEY_SIZE          equ 16   ; Schluesselbytes (kompakter Name)
REG_REC_SIZE          equ 32
RG_TYPE               equ 0    ; REG_TYPE_*
RG_KEY                equ 4    ; 16 Bytes Key
RG_VALUE_LO           equ 20   ; uint32 Wert (z.B. Algorithmus-ID)
RG_VALUE_HI           equ 24   ; uint32 Erweiterungswert
RG_FLAGS              equ 28

; Registry-Typen (NPSPEC-REGISTRY-TYPE-0001)
REG_TYPE_UNKNOWN      equ 0
REG_TYPE_ALGORITHM    equ 1    ; (NPSPEC-REGISTRY-ALGORITHM-0001)
REG_TYPE_CAPABILITY   equ 2    ; (NPSPEC-REGISTRY-CAPABILITY-0001)
REG_TYPE_CODEC        equ 3    ; (NPSPEC-REGISTRY-CODEC-0001)
REG_TYPE_DEVICE       equ 4    ; (NPSPEC-REGISTRY-DEVICE-0001)
REG_TYPE_SERVICE      equ 5    ; (NPSPEC-REGISTRY-SERVICE-0001)
REG_TYPE_CUSTOM       equ 6

REG_CAPACITY          equ 32

; ---------------------------------------------------------------------------
; Capability-Kategorien (NPSPEC-CAPABILITY-CATEGORY-0001)
; ---------------------------------------------------------------------------
CAP_CAT_PLATFORM      equ 0    ; (NPSPEC-CAPABILITY-PLATFORM-0001)
CAP_CAT_INPUT         equ 1    ; (NPSPEC-CAPABILITY-INPUT-0001)
CAP_CAT_OUTPUT        equ 2    ; (NPSPEC-CAPABILITY-OUTPUT-0001)
CAP_CAT_RUNTIME       equ 3    ; (NPSPEC-CAPABILITY-RUNTIME-0001)
CAP_CAT_SANDBOX       equ 4    ; (NPSPEC-CAPABILITY-SANDBOX-0001)
CAP_CAT_SECURITY      equ 5    ; (NPSPEC-CAPABILITY-SECURITY-0001)
CAP_CAT_COUNT         equ 6

; Capability-Flags
CAP_FLAG_ACTIVE       equ 0x01  ; (NPSPEC-CAPABILITY-ACTIVATION-0001)
CAP_FLAG_REVOKED      equ 0x02  ; (NPSPEC-CAPABILITY-REVOKE-0001)
CAP_FLAG_SIGNED       equ 0x04  ; (NPSPEC-CAPABILITY-SIGNING-0001)
CAP_FLAG_TRANSFERABLE equ 0x08  ; (NPSPEC-CAPABILITY-TRANSFER-0001)
CAP_FLAG_SANDBOX      equ 0x10  ; (NPSPEC-CAPABILITY-SANDBOX-0001)
CAP_FLAG_COMPAT       equ 0x20  ; (NPSPEC-CAPABILITY-COMPATIBILITY-0001)

; Capability-Record (48 Bytes, NPSPEC-CAPABILITY-ID-0001)
CAP_REC_SIZE          equ 48
CR_ID                 equ 0    ; uint32 Capability-ID
CR_CATEGORY           equ 4    ; CAP_CAT_*
CR_FLAGS              equ 8
CR_PROGRAM_ID         equ 12   ; Inhabendes Programm
CR_NS_ID              equ 16   ; Namespace-Binding (NPSPEC-CAPABILITY-NAMESPACE-0001)
CR_VERSION_REQ        equ 20   ; Mindestversion (NPSPEC-CAPABILITY-VERSIONING-0001)
CR_PROVIDER_ID        equ 24   ; Anbieter (NPSPEC-CAPABILITY-PROVIDER-0001)
CR_PERM_MASK          equ 28   ; Erlaubte Operationen (NPSPEC-CAPABILITY-PERMISSIONS-0001)
CR_SIGN_HASH          equ 32   ; Signatur-Hash (NPSPEC-CAPABILITY-SIGNING-0001)
CR_RUNTIME_DATA       equ 36   ; Laufzeit-Daten (NPSPEC-CAPABILITY-RUNTIME-0001)
CR_SHIM_PTR           equ 40   ; Shim-Zeiger (NPSPEC-CAPABILITY-SHIM-0001)
; 44..47: reserviert

CAP_REGISTRY_CAPACITY equ 16

; ---------------------------------------------------------------------------
; program_manager_initialize
; CF=0 Erfolg
; ---------------------------------------------------------------------------
program_manager_initialize:
    push edi
    mov edi, prog_table
    xor eax, eax
    mov ecx, (PROG_CAPACITY * PROG_REC_SIZE) / 4
    rep stosd
    mov edi, prog_reg_table
    mov ecx, (REG_CAPACITY * REG_REC_SIZE) / 4
    rep stosd
    mov edi, prog_cap_table
    mov ecx, (CAP_REGISTRY_CAPACITY * CAP_REC_SIZE) / 4
    rep stosd
    pop edi

    mov dword [prog_count], 0
    mov dword [prog_reg_count], 0
    mov dword [prog_cap_count], 0
    mov dword [prog_next_id], 1
    mov dword [prog_next_cap_id], 1

    ; Plattform-Capabilities registrieren (NPSPEC-CAPABILITY-PLATFORM-0001)
    mov eax, CAP_CAT_PLATFORM
    mov ebx, CAP_FLAG_ACTIVE | CAP_FLAG_SIGNED
    mov ecx, 0xFFFFFFFF         ; System-Owner
    call cap_register
    jc .fail

    ; Sandbox-Capability (NPSPEC-CAPABILITY-SANDBOX-0001)
    mov eax, CAP_CAT_SANDBOX
    mov ebx, CAP_FLAG_ACTIVE | CAP_FLAG_SANDBOX
    mov ecx, 0xFFFFFFFF
    call cap_register
    jc .fail

    ; Standard-Algorithmen in Registry (NPSPEC-REGISTRY-ALGORITHM-0001)
    mov eax, REG_TYPE_ALGORITHM
    mov ebx, 0x43524333         ; "CRC3" kompakt
    mov ecx, 0x43524333         ; Algorithmus-ID
    call reg_insert
    jc .fail

    mov dword [prog_manager_ready], 1
    clc
    ret
.fail:
    stc
    ret

; ---------------------------------------------------------------------------
; prog_install: EAX=prog_id, EBX=flags, ECX=scope, EDX=version
; (NPSPEC-PROGRAM-INSTALL-0001)
; CF=0 Erfolg
; ---------------------------------------------------------------------------
prog_install:
    cmp dword [prog_count], PROG_CAPACITY
    jae .full
    push edi
    push esi
    mov edi, prog_table
    push ecx
    mov ecx, [prog_count]
    imul ecx, PROG_REC_SIZE
    add edi, ecx
    pop ecx
    mov [edi + PKG_ID], eax
    mov [edi + PKG_FLAGS], ebx
    mov dword [edi + PKG_STATE], PROG_STATE_INSTALLED
    mov [edi + PKG_SCOPE], ecx
    mov [edi + PKG_VERSION], edx
    mov dword [edi + PKG_MIN_VERSION], 0
    mov dword [edi + PKG_NS_ID], FS_NS_PROGRAM
    mov dword [edi + PKG_CAP_MASK_LO], 0
    mov dword [edi + PKG_CAP_MASK_HI], 0
    mov dword [edi + PKG_DEP_COUNT], 0
    ; Trust-Hash: XOR aus ID + Version (NPSPEC-PROGRAM-TRUST-0001)
    xor eax, edx
    mov [edi + PKG_TRUST_HASH], eax
    inc dword [prog_count]
    pop esi
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; prog_uninstall: EAX=prog_id
; (NPSPEC-PROGRAM-UNINSTALL-0001)
; ---------------------------------------------------------------------------
prog_uninstall:
    push esi
    push ecx
    mov esi, prog_table
    mov ecx, [prog_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + PKG_ID], eax
    je .found
    add esi, PROG_REC_SIZE
    dec ecx
    jmp .scan
.found:
    mov dword [esi + PKG_STATE], PROG_STATE_ABSENT
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
; prog_update: EAX=prog_id, EDX=new_version
; (NPSPEC-PROGRAM-UPDATE-0001, NPSPEC-PROGRAM-ROLLBACK-0001)
; Speichert alte Version in PR_MIN_VERSION fuer Rollback.
; ---------------------------------------------------------------------------
prog_update:
    push esi
    push ecx
    mov esi, prog_table
    mov ecx, [prog_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + PKG_ID], eax
    je .found
    add esi, PROG_REC_SIZE
    dec ecx
    jmp .scan
.found:
    ; Alte Version als Rollback sichern
    push eax
    mov eax, [esi + PKG_VERSION]
    mov [esi + PKG_MIN_VERSION], eax
    pop eax
    mov [esi + PKG_VERSION], edx
    mov dword [esi + PKG_STATE], PROG_STATE_INSTALLED
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
; prog_rollback: EAX=prog_id
; (NPSPEC-PROGRAM-ROLLBACK-0001)
; ---------------------------------------------------------------------------
prog_rollback:
    push esi
    push ecx
    mov esi, prog_table
    mov ecx, [prog_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + PKG_ID], eax
    je .found
    add esi, PROG_REC_SIZE
    dec ecx
    jmp .scan
.found:
    ; Tausche Version und Min-Version
    push eax
    mov eax, [esi + PKG_MIN_VERSION]
    push edx
    mov edx, [esi + PKG_VERSION]
    mov [esi + PKG_VERSION], eax
    mov [esi + PKG_MIN_VERSION], edx
    pop edx
    pop eax
    mov dword [esi + PKG_STATE], PROG_STATE_ROLLBACK
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
; reg_insert: EAX=type, EBX=key_lo, ECX=value_lo
; (NPSPEC-REGISTRY-UPDATE-0001)
; ---------------------------------------------------------------------------
reg_insert:
    cmp dword [prog_reg_count], REG_CAPACITY
    jae .full
    push edi
    push edx
    mov edi, prog_reg_table
    mov edx, [prog_reg_count]
    imul edx, REG_REC_SIZE
    add edi, edx
    mov [edi + RG_TYPE], eax
    mov [edi + RG_KEY], ebx         ; Erstes DWORD des Keys
    mov dword [edi + RG_KEY + 4], 0
    mov dword [edi + RG_KEY + 8], 0
    mov dword [edi + RG_KEY + 12], 0
    mov [edi + RG_VALUE_LO], ecx
    mov dword [edi + RG_VALUE_HI], 0
    mov dword [edi + RG_FLAGS], 0
    inc dword [prog_reg_count]
    pop edx
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; reg_query: EAX=type, EBX=key_lo -> ECX=value_lo, CF=0 ok
; (NPSPEC-REGISTRY-QUERY-0001)
; ---------------------------------------------------------------------------
reg_query:
    push esi
    push edx
    mov esi, prog_reg_table
    mov edx, [prog_reg_count]
.scan:
    test edx, edx
    jz .not_found
    cmp [esi + RG_TYPE], eax
    jne .next
    cmp [esi + RG_KEY], ebx
    je .found
.next:
    add esi, REG_REC_SIZE
    dec edx
    jmp .scan
.found:
    mov ecx, [esi + RG_VALUE_LO]
    pop edx
    pop esi
    clc
    ret
.not_found:
    pop edx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_register: EAX=category, EBX=flags, ECX=program_id
; (NPSPEC-CAPABILITY-REGISTRY-0001, NPSPEC-CAPABILITY-IMPLEMENTATION-0001)
; CF=0 Erfolg -> EDX=cap_id
; ---------------------------------------------------------------------------
cap_register:
    cmp dword [prog_cap_count], CAP_REGISTRY_CAPACITY
    jae .full
    push edi
    push esi
    mov edi, prog_cap_table
    push eax
    mov eax, [prog_cap_count]
    imul eax, CAP_REC_SIZE
    add edi, eax
    pop eax
    mov edx, [prog_next_cap_id]
    mov [edi + CR_ID], edx
    mov [edi + CR_CATEGORY], eax
    mov [edi + CR_FLAGS], ebx
    mov [edi + CR_PROGRAM_ID], ecx
    mov dword [edi + CR_NS_ID], FS_NS_ROOT
    mov dword [edi + CR_VERSION_REQ], 0
    mov dword [edi + CR_PROVIDER_ID], 0xFFFFFFFF
    mov dword [edi + CR_PERM_MASK], FSPOLICY_ALLOW_READ
    mov dword [edi + CR_SIGN_HASH], 0
    mov dword [edi + CR_RUNTIME_DATA], 0
    mov dword [edi + CR_SHIM_PTR], 0
    inc dword [prog_next_cap_id]
    inc dword [prog_cap_count]
    pop esi
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; cap_activate: EDX=cap_id
; (NPSPEC-CAPABILITY-ACTIVATION-0001)
; ---------------------------------------------------------------------------
cap_activate:
    push esi
    push ecx
    mov esi, prog_cap_table
    mov ecx, [prog_cap_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + CR_ID], edx
    je .found
    add esi, CAP_REC_SIZE
    dec ecx
    jmp .scan
.found:
    or dword [esi + CR_FLAGS], CAP_FLAG_ACTIVE
    and dword [esi + CR_FLAGS], ~CAP_FLAG_REVOKED
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
; pkg_cap_revoke: EDX=cap_id
; (NPSPEC-CAPABILITY-REVOKE-0001)
; ---------------------------------------------------------------------------
pkg_cap_revoke:
    push esi
    push ecx
    mov esi, prog_cap_table
    mov ecx, [prog_cap_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + CR_ID], edx
    je .found
    add esi, CAP_REC_SIZE
    dec ecx
    jmp .scan
.found:
    or dword [esi + CR_FLAGS], CAP_FLAG_REVOKED
    and dword [esi + CR_FLAGS], ~CAP_FLAG_ACTIVE
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
; cap_query: EDX=cap_id -> EBX=category, ECX=flags, CF=0 ok
; (NPSPEC-CAPABILITY-QUERY-0001, NPSPEC-CAPABILITY-INTERFACE-0001)
; ---------------------------------------------------------------------------
cap_query:
    push esi
    push eax
    mov esi, prog_cap_table
    mov eax, [prog_cap_count]
.scan:
    test eax, eax
    jz .not_found
    cmp [esi + CR_ID], edx
    je .found
    add esi, CAP_REC_SIZE
    dec eax
    jmp .scan
.found:
    mov ebx, [esi + CR_CATEGORY]
    mov ecx, [esi + CR_FLAGS]
    pop eax
    pop esi
    clc
    ret
.not_found:
    pop eax
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_check_permission: EDX=cap_id, EAX=required_perm -> CF=0 erlaubt
; (NPSPEC-CAPABILITY-PERMISSIONS-0001)
; ---------------------------------------------------------------------------
cap_check_permission:
    push esi
    push ecx
    push ebx
    mov esi, prog_cap_table
    mov ecx, [prog_cap_count]
.scan:
    test ecx, ecx
    jz .denied
    cmp [esi + CR_ID], edx
    je .found
    add esi, CAP_REC_SIZE
    dec ecx
    jmp .scan
.found:
    ; Widerrufen?
    test dword [esi + CR_FLAGS], CAP_FLAG_REVOKED
    jnz .denied
    ; Nicht aktiv?
    test dword [esi + CR_FLAGS], CAP_FLAG_ACTIVE
    jz .denied
    ; Permission-Mask pruefen
    mov ebx, [esi + CR_PERM_MASK]
    and ebx, eax
    cmp ebx, eax
    jne .denied
    pop ebx
    pop ecx
    pop esi
    clc
    ret
.denied:
    pop ebx
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; program_manager_self_test
; ---------------------------------------------------------------------------
program_manager_self_test:
    push esi
    xor esi, esi

    cmp dword [prog_manager_ready], 1
    je .t2
    inc esi
.t2:
    ; Programm installieren
    mov eax, 0x1000       ; Test-Prog-ID
    mov ebx, PROG_FLAG_SYSTEM | PROG_FLAG_TRUSTED
    mov ecx, PROG_SCOPE_SYSTEM
    mov edx, 0x00010000   ; v1.0.0
    call prog_install
    jnc .t3
    inc esi
.t3:
    ; Registry-Abfrage (Algorithmus)
    mov eax, REG_TYPE_ALGORITHM
    mov ebx, 0x43524333
    call reg_query
    jnc .t4
    inc esi
.t4:
    ; Capability-Query: Platform-Cap hat ID 1
    mov edx, 1
    call cap_query
    jnc .t5
    inc esi
.t5:
    ; Programm-Update + Rollback
    mov eax, 0x1000
    mov edx, 0x00020000   ; v2.0.0
    call prog_update
    jnc .t6
    inc esi
.t6:
    mov eax, 0x1000
    call prog_rollback
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

; ---------------------------------------------------------------------------
; Statische Daten
; ---------------------------------------------------------------------------
align 4
prog_manager_ready:   dd 0
prog_count:           dd 0
prog_reg_count:       dd 0
prog_cap_count:       dd 0
prog_next_id:         dd 1
prog_next_cap_id:     dd 1

align 4
prog_table:
    times PROG_CAPACITY * PROG_REC_SIZE db 0

align 4
prog_reg_table:
    times REG_CAPACITY * REG_REC_SIZE db 0

align 4
prog_cap_table:
    times CAP_REGISTRY_CAPACITY * CAP_REC_SIZE db 0
