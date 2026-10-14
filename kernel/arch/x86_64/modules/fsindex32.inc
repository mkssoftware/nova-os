; ---------------------------------------------------------------------------
; Nova Filesystem-Index und System-Verzeichnis §038 (Kategorien 006, 007)
;
; NPSPEC-INDEX-INTEGRITY-0001    - Index-Integritaet
; NPSPEC-INDEX-METADATA-0001     - Metadaten-Index
; NPSPEC-INDEX-NAMESPACE-0001    - Namespace-Index
; NPSPEC-INDEX-QUERY-0001        - Index-Abfragen
; NPSPEC-INDEX-REBUILD-0001      - Index-Wiederherstellung
; NPSPEC-INDEX-RELATION-0001     - Relationsindex
; NPSPEC-INDEX-SEARCH-0001       - Volltextsuche
; NPSPEC-INDEX-TRANSACTION-0001  - Transaktionaler Index
; NPSPEC-INDEX-UPDATE-0001       - Index-Updates
; NPSPEC-INDEX-WORKSPACE-0001    - Workspace-Index
;
; NPSPEC-SYSTEM-BOOT-0001        - Boot-Verzeichnisse
; NPSPEC-SYSTEM-DRIVERS-0001     - Treiber-Verzeichnisse
; NPSPEC-SYSTEM-FOUNDATION-0001  - Basis-Systemverzeichnis
; NPSPEC-SYSTEM-FRAMEWORK-0001   - Framework-Verzeichnis
; NPSPEC-SYSTEM-HAL-0001         - HAL-Verzeichnis
; NPSPEC-SYSTEM-KERNEL-0001      - Kernel-Verzeichnis
; NPSPEC-SYSTEM-LAYOUT-0001      - System-Verzeichnislayout
; NPSPEC-SYSTEM-LIBRARIES-0001   - Bibliothek-Verzeichnis
; NPSPEC-SYSTEM-LOCALE-0001      - Locale-Verzeichnis
; NPSPEC-SYSTEM-MODULES-0001     - Modul-Verzeichnis
; NPSPEC-SYSTEM-POLICY-0001      - System-Policy-Verzeichnis
; NPSPEC-SYSTEM-PROTECTION-0001  - Schutzmechanismen
; NPSPEC-SYSTEM-RECOVERY-0001    - Recovery-Verzeichnis
; NPSPEC-SYSTEM-REGISTRY-0001    - System-Registry-Verzeichnis
; NPSPEC-SYSTEM-RESOURCES-0001   - Ressource-Verzeichnis
; NPSPEC-SYSTEM-RUNTIME-0001     - Laufzeit-Verzeichnis
; NPSPEC-SYSTEM-SECURITY-0001    - Sicherheitsverzeichnis
; NPSPEC-SYSTEM-SERVICES-0001    - Service-Verzeichnis
; NPSPEC-SYSTEM-TRUST-0001       - Trust-Verzeichnis
; NPSPEC-SYSTEM-UPDATES-0001     - Update-Staging-Verzeichnis
; ---------------------------------------------------------------------------

; ---------------------------------------------------------------------------
; System-Verzeichnis-IDs (NPSPEC-SYSTEM-LAYOUT-0001)
; Entsprechen den kanonischen Pfaden unter /sys/
; ---------------------------------------------------------------------------
SYSDIR_KERNEL         equ 0    ; /sys/kernel  (NPSPEC-SYSTEM-KERNEL-0001)
SYSDIR_BOOT           equ 1    ; /sys/boot    (NPSPEC-SYSTEM-BOOT-0001)
SYSDIR_DRIVERS        equ 2    ; /sys/drivers (NPSPEC-SYSTEM-DRIVERS-0001)
SYSDIR_MODULES        equ 3    ; /sys/modules (NPSPEC-SYSTEM-MODULES-0001)
SYSDIR_FRAMEWORK      equ 4    ; /sys/fw      (NPSPEC-SYSTEM-FRAMEWORK-0001)
SYSDIR_HAL            equ 5    ; /sys/hal     (NPSPEC-SYSTEM-HAL-0001)
SYSDIR_LIBRARIES      equ 6    ; /sys/lib     (NPSPEC-SYSTEM-LIBRARIES-0001)
SYSDIR_LOCALE         equ 7    ; /sys/locale  (NPSPEC-SYSTEM-LOCALE-0001)
SYSDIR_RESOURCES      equ 8    ; /sys/res     (NPSPEC-SYSTEM-RESOURCES-0001)
SYSDIR_RUNTIME        equ 9    ; /sys/rt      (NPSPEC-SYSTEM-RUNTIME-0001)
SYSDIR_SECURITY       equ 10   ; /sys/sec     (NPSPEC-SYSTEM-SECURITY-0001)
SYSDIR_SERVICES       equ 11   ; /sys/svc     (NPSPEC-SYSTEM-SERVICES-0001)
SYSDIR_TRUST          equ 12   ; /sys/trust   (NPSPEC-SYSTEM-TRUST-0001)
SYSDIR_RECOVERY       equ 13   ; /sys/recovery (NPSPEC-SYSTEM-RECOVERY-0001)
SYSDIR_POLICY         equ 14   ; /sys/policy  (NPSPEC-SYSTEM-POLICY-0001)
SYSDIR_REGISTRY       equ 15   ; /sys/reg     (NPSPEC-SYSTEM-REGISTRY-0001)
SYSDIR_UPDATES        equ 16   ; /sys/updates (NPSPEC-SYSTEM-UPDATES-0001)
SYSDIR_FOUNDATION     equ 17   ; /sys/core    (NPSPEC-SYSTEM-FOUNDATION-0001)
SYSDIR_PROTECTION     equ 18   ; /sys/prot    (NPSPEC-SYSTEM-PROTECTION-0001)
SYSDIR_COUNT          equ 19

; System-Verzeichnis-Schutzklassen (NPSPEC-SYSTEM-PROTECTION-0001)
SYSDIR_PROT_NONE      equ 0    ; Kein besonderer Schutz
SYSDIR_PROT_READ_ONLY equ 1    ; Nur Lesen (Laufzeit)
SYSDIR_PROT_IMMUTABLE equ 2    ; Unveraenderlich
SYSDIR_PROT_SIGNED    equ 3    ; Signaturpruefung (NPSPEC-SYSTEM-TRUST-0001)

; Sysdir-Record (32 Bytes)
SYSDIR_REC_SIZE       equ 32
SD_ID                 equ 0
SD_FLAGS              equ 4
SD_PROTECTION         equ 8
SD_PARENT_NS          equ 12   ; FS_NS_SYSTEM
SD_TRUST_LEVEL        equ 16
SD_OBJECT_COUNT       equ 20   ; Anzahl Objekte im Verzeichnis
SD_CHECKSUM           equ 24   ; (NPSPEC-INDEX-INTEGRITY-0001)
; 28..31: reserviert

; ---------------------------------------------------------------------------
; Index-Eintrag (NPSPEC-INDEX-METADATA-0001)
; ---------------------------------------------------------------------------
IDX_REC_SIZE          equ 32
IX_OBJECT_ID_LO       equ 0    ; ObjectID Inode-Lo
IX_OBJECT_ID_VOL      equ 4    ; Volume-ID
IX_NS_ID              equ 8    ; Namespace (NPSPEC-INDEX-NAMESPACE-0001)
IX_TYPE               equ 12   ; FSOBJ_TYPE_*
IX_SIZE               equ 16   ; Objektgroesse in Bytes
IX_FLAGS              equ 20
IX_SEARCH_HASH        equ 24   ; Hash fuer Suche (NPSPEC-INDEX-SEARCH-0001)
IX_TX_ID              equ 28   ; Letzte Transaktion (NPSPEC-INDEX-TRANSACTION-0001)

; Index-Flags
IDX_FLAG_VALID        equ 0x01
IDX_FLAG_DIRTY        equ 0x02  ; Muss rebuildiert werden (NPSPEC-INDEX-REBUILD-0001)
IDX_FLAG_WORKSPACE    equ 0x04  ; (NPSPEC-INDEX-WORKSPACE-0001)
IDX_FLAG_RELATION     equ 0x08  ; Hat Relationen (NPSPEC-INDEX-RELATION-0001)

IDX_CAPACITY          equ 32

; ---------------------------------------------------------------------------
; fsindex_initialize
; Initialisiert Sysdir-Tabelle und Index.
; CF=0 Erfolg
; ---------------------------------------------------------------------------
fsindex_initialize:
    push edi
    ; Sysdir-Tabelle loeschen
    mov edi, sysdir_table
    xor eax, eax
    mov ecx, (SYSDIR_COUNT * SYSDIR_REC_SIZE) / 4
    rep stosd
    ; Index-Tabelle loeschen
    mov edi, fsindex_table
    mov ecx, (IDX_CAPACITY * IDX_REC_SIZE) / 4
    rep stosd
    pop edi

    mov dword [fsindex_count], 0

    ; System-Verzeichnisse anlegen (NPSPEC-SYSTEM-LAYOUT-0001)
    mov eax, SYSDIR_KERNEL
    mov ebx, SYSDIR_PROT_SIGNED
    call sysdir_register
    jc .fail

    mov eax, SYSDIR_BOOT
    mov ebx, SYSDIR_PROT_SIGNED
    call sysdir_register
    jc .fail

    mov eax, SYSDIR_DRIVERS
    mov ebx, SYSDIR_PROT_SIGNED
    call sysdir_register
    jc .fail

    mov eax, SYSDIR_MODULES
    mov ebx, SYSDIR_PROT_SIGNED
    call sysdir_register
    jc .fail

    mov eax, SYSDIR_FRAMEWORK
    mov ebx, SYSDIR_PROT_SIGNED
    call sysdir_register
    jc .fail

    mov eax, SYSDIR_HAL
    mov ebx, SYSDIR_PROT_READ_ONLY
    call sysdir_register
    jc .fail

    mov eax, SYSDIR_LIBRARIES
    mov ebx, SYSDIR_PROT_READ_ONLY
    call sysdir_register
    jc .fail

    mov eax, SYSDIR_LOCALE
    mov ebx, SYSDIR_PROT_NONE
    call sysdir_register
    jc .fail

    mov eax, SYSDIR_RESOURCES
    mov ebx, SYSDIR_PROT_READ_ONLY
    call sysdir_register
    jc .fail

    mov eax, SYSDIR_RUNTIME
    mov ebx, SYSDIR_PROT_NONE
    call sysdir_register
    jc .fail

    mov eax, SYSDIR_SECURITY
    mov ebx, SYSDIR_PROT_IMMUTABLE
    call sysdir_register
    jc .fail

    mov eax, SYSDIR_SERVICES
    mov ebx, SYSDIR_PROT_SIGNED
    call sysdir_register
    jc .fail

    mov eax, SYSDIR_TRUST
    mov ebx, SYSDIR_PROT_IMMUTABLE
    call sysdir_register
    jc .fail

    mov eax, SYSDIR_RECOVERY
    mov ebx, SYSDIR_PROT_SIGNED
    call sysdir_register
    jc .fail

    mov eax, SYSDIR_POLICY
    mov ebx, SYSDIR_PROT_SIGNED
    call sysdir_register
    jc .fail

    mov eax, SYSDIR_REGISTRY
    mov ebx, SYSDIR_PROT_READ_ONLY
    call sysdir_register
    jc .fail

    mov eax, SYSDIR_UPDATES
    mov ebx, SYSDIR_PROT_NONE
    call sysdir_register
    jc .fail

    mov eax, SYSDIR_FOUNDATION
    mov ebx, SYSDIR_PROT_IMMUTABLE
    call sysdir_register
    jc .fail

    mov eax, SYSDIR_PROTECTION
    mov ebx, SYSDIR_PROT_IMMUTABLE
    call sysdir_register
    jc .fail

    mov dword [fsindex_ready], 1
    clc
    ret
.fail:
    stc
    ret

; ---------------------------------------------------------------------------
; sysdir_register: EAX=sysdir_id, EBX=protection
; ---------------------------------------------------------------------------
sysdir_register:
    cmp eax, SYSDIR_COUNT
    jae .invalid
    push edi
    mov edi, sysdir_table
    push ecx
    mov ecx, eax
    imul ecx, SYSDIR_REC_SIZE
    add edi, ecx
    pop ecx
    mov [edi + SD_ID], eax
    mov dword [edi + SD_FLAGS], IDX_FLAG_VALID
    mov [edi + SD_PROTECTION], ebx
    mov dword [edi + SD_PARENT_NS], FS_NS_SYSTEM
    ; Trust-Level aus Schutzklasse ableiten (NPSPEC-SYSTEM-TRUST-0001)
    cmp ebx, SYSDIR_PROT_SIGNED
    jb .user_trust
    cmp ebx, SYSDIR_PROT_IMMUTABLE
    jb .system_trust
    mov dword [edi + SD_TRUST_LEVEL], TRUST_SYSTEM
    jmp .trust_done
.system_trust:
    mov dword [edi + SD_TRUST_LEVEL], TRUST_SYSTEM
    jmp .trust_done
.user_trust:
    mov dword [edi + SD_TRUST_LEVEL], TRUST_USER
.trust_done:
    mov dword [edi + SD_OBJECT_COUNT], 0
    mov dword [edi + SD_CHECKSUM], 0
    pop edi
    clc
    ret
.invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; fsindex_insert: EAX=inode_lo, EBX=vol_id, ECX=ns_id, EDX=type
; (NPSPEC-INDEX-UPDATE-0001)
; ---------------------------------------------------------------------------
fsindex_insert:
    cmp dword [fsindex_count], IDX_CAPACITY
    jae .full
    push edi
    push esi
    mov edi, fsindex_table
    push eax
    mov eax, [fsindex_count]
    imul eax, IDX_REC_SIZE
    add edi, eax
    pop eax
    mov [edi + IX_OBJECT_ID_LO], eax
    mov [edi + IX_OBJECT_ID_VOL], ebx
    mov [edi + IX_NS_ID], ecx
    mov [edi + IX_TYPE], edx
    mov dword [edi + IX_SIZE], 0
    mov dword [edi + IX_FLAGS], IDX_FLAG_VALID
    ; Suchkeyword-Hash: einfacher Poly-Hash
    push eax
    imul eax, 0x9E3779B9
    xor eax, ebx
    xor eax, ecx
    mov [edi + IX_SEARCH_HASH], eax
    pop eax
    mov dword [edi + IX_TX_ID], 0
    inc dword [fsindex_count]
    pop esi
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; fsindex_search: ECX=ns_id, EDX=type -> EAX=inode_lo, CF=0 ok
; (NPSPEC-INDEX-QUERY-0001, NPSPEC-INDEX-SEARCH-0001)
; ---------------------------------------------------------------------------
fsindex_search:
    push esi
    push ebx
    mov esi, fsindex_table
    mov ebx, [fsindex_count]
.scan:
    test ebx, ebx
    jz .not_found
    ; Beide Kriterien: NS + Typ
    cmp [esi + IX_NS_ID], ecx
    jne .next
    cmp [esi + IX_TYPE], edx
    jne .next
    test dword [esi + IX_FLAGS], IDX_FLAG_VALID
    jz .next
    mov eax, [esi + IX_OBJECT_ID_LO]
    pop ebx
    pop esi
    clc
    ret
.next:
    add esi, IDX_REC_SIZE
    dec ebx
    jmp .scan
.not_found:
    pop ebx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; fsindex_rebuild: Markiert alle Eintraege als frisch validiert
; (NPSPEC-INDEX-REBUILD-0001, NPSPEC-INDEX-INTEGRITY-0001)
; ---------------------------------------------------------------------------
fsindex_rebuild:
    push esi
    push ecx
    mov esi, fsindex_table
    mov ecx, [fsindex_count]
.loop:
    test ecx, ecx
    jz .done
    and dword [esi + IX_FLAGS], ~IDX_FLAG_DIRTY
    or dword [esi + IX_FLAGS], IDX_FLAG_VALID
    ; Pruefsumme aktualisieren
    mov eax, [esi + IX_OBJECT_ID_LO]
    xor eax, [esi + IX_OBJECT_ID_VOL]
    mov [esi + IX_SEARCH_HASH], eax
    add esi, IDX_REC_SIZE
    dec ecx
    jmp .loop
.done:
    pop ecx
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; fsindex_self_test
; ---------------------------------------------------------------------------
fsindex_self_test:
    push esi
    xor esi, esi

    cmp dword [fsindex_ready], 1
    je .t2
    inc esi
.t2:
    ; Sysdir vorhanden: SYSDIR_KERNEL
    mov edi, sysdir_table
    cmp dword [edi + SD_ID], SYSDIR_KERNEL
    je .t3
    inc esi
.t3:
    ; Index-Eintrag einfuegen
    mov eax, 0x100     ; Test-Inode
    mov ebx, 0         ; Volume 0
    mov ecx, FS_NS_SYSTEM
    mov edx, FSOBJ_TYPE_DIR
    call fsindex_insert
    jnc .t4
    inc esi
.t4:
    ; Suche nach dem eingefuegten Eintrag
    mov ecx, FS_NS_SYSTEM
    mov edx, FSOBJ_TYPE_DIR
    call fsindex_search
    jnc .t5
    inc esi
.t5:
    ; Rebuild
    call fsindex_rebuild
    jnc .done
    inc esi
.done:
    pop esi
    test esi, esi
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
fsindex_ready:    dd 0
fsindex_count:    dd 0

align 4
sysdir_table:
    times SYSDIR_COUNT * SYSDIR_REC_SIZE db 0

align 4
fsindex_table:
    times IDX_CAPACITY * IDX_REC_SIZE db 0
