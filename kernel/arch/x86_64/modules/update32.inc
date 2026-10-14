; ---------------------------------------------------------------------------
; Nova Update-Manager §037
; NPSPEC-UPDATE-AB-0001           - A/B-Update
; NPSPEC-UPDATE-ATOMIC-0001       - Atomare Updates
; NPSPEC-UPDATE-BOOTLOADER-0001   - Bootloader-Update
; NPSPEC-UPDATE-CANARY-0001       - Canary-Rollout
; NPSPEC-UPDATE-CONTENTADDRESS-0001 - Content-Addressable
; NPSPEC-UPDATE-DELTA-0001        - Delta-Updates
; NPSPEC-UPDATE-DEPENDENCY-0001   - Abhaengigkeits-Auflosung
; NPSPEC-UPDATE-DRIVER-0001       - Treiber-Updates
; NPSPEC-UPDATE-FIRMWARE-0001     - Firmware-Updates
; NPSPEC-UPDATE-HOTPATCH-0001     - Hot-Patching
; NPSPEC-UPDATE-IMMUTABLE-0001    - Immutable Updates
; NPSPEC-UPDATE-LIFECYCLE-0001    - Update-Lebenszyklus
; NPSPEC-UPDATE-LIVE-0001         - Live-Updates
; NPSPEC-UPDATE-MANAGER-0001      - Update-Manager
; NPSPEC-UPDATE-PACKAGE-0001      - Paket-Updates
; NPSPEC-UPDATE-PROVENANCE-0001   - Herkunft
; NPSPEC-UPDATE-REPOSITORY-0001   - Repository
; NPSPEC-UPDATE-ROLLBACK-0001     - Rollback
; NPSPEC-UPDATE-ROLLING-0001      - Rolling Updates
; NPSPEC-UPDATE-SBOM-0001         - Software Bill of Materials
; NPSPEC-UPDATE-SIGNING-0001      - Signierung
; NPSPEC-UPDATE-SNAPSHOT-0001     - Snapshots
; NPSPEC-UPDATE-STAGED-0001       - Staged Rollout
; NPSPEC-UPDATE-TRANSACTIONAL-0001- Transaktionale Updates
; NPSPEC-UPDATE-VERIFY-0001       - Verifikation
; NPSPEC-UPDATE-VULNERABILITY-0001- Schwachstellen-Management
; ---------------------------------------------------------------------------

; Update-Typen (NPSPEC-UPDATE-MANAGER-0001)
UPD_TYPE_KERNEL       equ 0
UPD_TYPE_DRIVER       equ 1    ; (NPSPEC-UPDATE-DRIVER-0001)
UPD_TYPE_FIRMWARE     equ 2    ; (NPSPEC-UPDATE-FIRMWARE-0001)
UPD_TYPE_BOOTLOADER   equ 3    ; (NPSPEC-UPDATE-BOOTLOADER-0001)
UPD_TYPE_PACKAGE      equ 4    ; (NPSPEC-UPDATE-PACKAGE-0001)
UPD_TYPE_HOTPATCH     equ 5    ; (NPSPEC-UPDATE-HOTPATCH-0001)

; Update-Zustaende (NPSPEC-UPDATE-LIFECYCLE-0001)
UPD_STATE_PENDING     equ 0
UPD_STATE_STAGING     equ 1    ; (NPSPEC-UPDATE-STAGED-0001)
UPD_STATE_CANARY      equ 2    ; (NPSPEC-UPDATE-CANARY-0001)
UPD_STATE_ROLLING     equ 3    ; (NPSPEC-UPDATE-ROLLING-0001)
UPD_STATE_APPLIED     equ 4
UPD_STATE_FAILED      equ 5
UPD_STATE_ROLLED_BACK equ 6    ; (NPSPEC-UPDATE-ROLLBACK-0001)

; Update-Flags
UPD_FLAG_ATOMIC       equ 0x01  ; (NPSPEC-UPDATE-ATOMIC-0001)
UPD_FLAG_SIGNED       equ 0x02  ; (NPSPEC-UPDATE-SIGNING-0001)
UPD_FLAG_IMMUTABLE    equ 0x04  ; (NPSPEC-UPDATE-IMMUTABLE-0001)
UPD_FLAG_LIVE         equ 0x08  ; (NPSPEC-UPDATE-LIVE-0001)
UPD_FLAG_AB           equ 0x10  ; (NPSPEC-UPDATE-AB-0001)
UPD_FLAG_DELTA        equ 0x20  ; (NPSPEC-UPDATE-DELTA-0001)
UPD_FLAG_VERIFIED     equ 0x40  ; (NPSPEC-UPDATE-VERIFY-0001)
UPD_FLAG_HAS_VULN     equ 0x80  ; (NPSPEC-UPDATE-VULNERABILITY-0001)

; Update-Record (64 Bytes)
UPD_REC_SIZE          equ 64
UR_ID                 equ 0
UR_TYPE               equ 4    ; UPD_TYPE_*
UR_STATE              equ 8    ; UPD_STATE_*
UR_FLAGS              equ 12
UR_TARGET_ID          equ 16   ; Ziel-Programm/Treiber-ID
UR_OLD_VERSION        equ 20
UR_NEW_VERSION        equ 24
UR_CONTENT_HASH       equ 28   ; (NPSPEC-UPDATE-CONTENTADDRESS-0001)
UR_SIGN_HASH          equ 32   ; (NPSPEC-UPDATE-SIGNING-0001)
UR_PROV_HASH          equ 36   ; (NPSPEC-UPDATE-PROVENANCE-0001)
UR_SNAPSHOT_ID        equ 40   ; Vor-Update-Snapshot (NPSPEC-UPDATE-SNAPSHOT-0001)
UR_DELTA_SIZE         equ 44   ; Groesse des Delta (NPSPEC-UPDATE-DELTA-0001)
UR_SBOM_HASH          equ 48   ; (NPSPEC-UPDATE-SBOM-0001)
UR_CANARY_PCT         equ 52   ; Canary-Prozentsatz (NPSPEC-UPDATE-CANARY-0001)
UR_TIMESTAMP          equ 56
; 60..63: reserviert

UPD_CAPACITY          equ 8

; A/B-Slot-Status (NPSPEC-UPDATE-AB-0001)
UPD_SLOT_A            equ 0
UPD_SLOT_B            equ 1
UPD_SLOT_UNKNOWN      equ 0xFF

; ---------------------------------------------------------------------------
; update_initialize
; ---------------------------------------------------------------------------
update_initialize:
    push edi
    mov edi, upd_table
    xor eax, eax
    mov ecx, (UPD_CAPACITY * UPD_REC_SIZE) / 4
    rep stosd
    pop edi
    mov dword [upd_count], 0
    mov dword [upd_next_id], 1
    mov dword [upd_active_slot], UPD_SLOT_A
    mov dword [upd_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; upd_submit: EAX=type, EBX=target_id, ECX=new_version, EDX=flags
; (NPSPEC-UPDATE-MANAGER-0001, NPSPEC-UPDATE-TRANSACTIONAL-0001)
; CF=0 Erfolg -> ESI=upd_id
; ---------------------------------------------------------------------------
upd_submit:
    cmp dword [upd_count], UPD_CAPACITY
    jae .full
    push edi
    push esi
    mov edi, upd_table
    push eax
    mov eax, [upd_count]
    imul eax, UPD_REC_SIZE
    add edi, eax
    pop eax
    mov esi, [upd_next_id]
    mov [edi + UR_ID], esi
    mov [edi + UR_TYPE], eax
    mov dword [edi + UR_STATE], UPD_STATE_PENDING
    mov [edi + UR_FLAGS], edx
    mov [edi + UR_TARGET_ID], ebx
    mov dword [edi + UR_OLD_VERSION], 0
    mov [edi + UR_NEW_VERSION], ecx
    ; Content-Hash: XOR(target, version)
    push eax
    mov eax, ebx
    xor eax, ecx
    mov [edi + UR_CONTENT_HASH], eax
    pop eax
    mov dword [edi + UR_SIGN_HASH], 0
    mov dword [edi + UR_PROV_HASH], 0
    mov dword [edi + UR_SNAPSHOT_ID], 0
    mov dword [edi + UR_DELTA_SIZE], 0
    mov dword [edi + UR_SBOM_HASH], 0
    mov dword [edi + UR_CANARY_PCT], 100  ; Default: 100% Rollout
    push eax
    mov eax, [timer_ticks]
    mov [edi + UR_TIMESTAMP], eax
    pop eax
    inc dword [upd_next_id]
    inc dword [upd_count]
    pop esi
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; upd_apply: EAX=upd_id
; (NPSPEC-UPDATE-ATOMIC-0001, NPSPEC-UPDATE-AB-0001)
; ---------------------------------------------------------------------------
upd_apply:
    push esi
    push ecx
    mov esi, upd_table
    mov ecx, [upd_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + UR_ID], eax
    je .found
    add esi, UPD_REC_SIZE
    dec ecx
    jmp .scan
.found:
    ; Signaturpruefung wenn Flag gesetzt (NPSPEC-UPDATE-VERIFY-0001)
    test dword [esi + UR_FLAGS], UPD_FLAG_SIGNED
    jz .apply
    cmp dword [esi + UR_SIGN_HASH], 0
    je .set_fail
.apply:
    ; A/B: Slot wechseln (NPSPEC-UPDATE-AB-0001)
    test dword [esi + UR_FLAGS], UPD_FLAG_AB
    jz .set_applied
    mov ecx, [upd_active_slot]
    xor ecx, 1              ; Slot A<->B tauschen
    mov [upd_active_slot], ecx
.set_applied:
    or dword [esi + UR_FLAGS], UPD_FLAG_VERIFIED
    mov dword [esi + UR_STATE], UPD_STATE_APPLIED
    pop ecx
    pop esi
    clc
    ret
.set_fail:
    mov dword [esi + UR_STATE], UPD_STATE_FAILED
    pop ecx
    pop esi
    stc
    ret
.not_found:
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; upd_rollback: EAX=upd_id (NPSPEC-UPDATE-ROLLBACK-0001)
; ---------------------------------------------------------------------------
upd_rollback:
    push esi
    push ecx
    mov esi, upd_table
    mov ecx, [upd_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + UR_ID], eax
    je .found
    add esi, UPD_REC_SIZE
    dec ecx
    jmp .scan
.found:
    mov dword [esi + UR_STATE], UPD_STATE_ROLLED_BACK
    ; A/B: Slot zurueck (NPSPEC-UPDATE-AB-0001)
    test dword [esi + UR_FLAGS], UPD_FLAG_AB
    jz .done
    mov ecx, [upd_active_slot]
    xor ecx, 1
    mov [upd_active_slot], ecx
.done:
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
; update_self_test
; ---------------------------------------------------------------------------
update_self_test:
    push esi
    xor esi, esi
    cmp dword [upd_ready], 1
    je .t2
    inc esi
.t2:
    mov eax, UPD_TYPE_PACKAGE
    mov ebx, 0x1000
    mov ecx, 0x00020000
    mov edx, UPD_FLAG_ATOMIC | UPD_FLAG_AB
    call upd_submit
    jnc .t3
    inc esi
.t3:
    ; ESI hat upd_id (1)
    mov eax, 1
    call upd_apply
    jnc .t4
    inc esi
.t4:
    mov eax, 1
    call upd_rollback
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

align 4
upd_ready:        dd 0
upd_count:        dd 0
upd_next_id:      dd 1
upd_active_slot:  dd UPD_SLOT_A

align 4
upd_table:
    times UPD_CAPACITY * UPD_REC_SIZE db 0
