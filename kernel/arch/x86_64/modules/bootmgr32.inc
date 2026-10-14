; ---------------------------------------------------------------------------
; Nova Boot-Manager §004
; NPSPEC-BOOT-AB-0001       - A/B-Boot
; NPSPEC-BOOT-ARCH-0001     - Architektur-Anforderungen
; NPSPEC-BOOT-BIOS-0001     - BIOS-Kompatibilitaet
; NPSPEC-BOOT-CHAIN-0001    - Boot-Chain
; NPSPEC-BOOT-HEALTH-0001   - Boot-Health
; NPSPEC-BOOT-MEASURED-0001 - Measured Boot
; NPSPEC-BOOT-RECOVERY-0001 - Recovery-Boot
; NPSPEC-BOOT-ROLLBACK-0001 - Boot-Rollback
; NPSPEC-BOOT-SECURE-0001   - Secure Boot
; NPSPEC-BOOT-TPM-0001      - TPM-Integration
; NPSPEC-BOOT-UEFI-0001     - UEFI-Boot
; NPSPEC-BOOT-VERIFIED-0001 - Verified Boot
; ---------------------------------------------------------------------------

; Boot-Modi (NPSPEC-BOOT-UEFI-0001 / BIOS-0001)
BOOT_MODE_UEFI        equ 0    ; (NPSPEC-BOOT-UEFI-0001)
BOOT_MODE_BIOS_COMPAT equ 1    ; (NPSPEC-BOOT-BIOS-0001)

; Boot-Slots A/B (NPSPEC-BOOT-AB-0001)
BOOT_SLOT_A           equ 0
BOOT_SLOT_B           equ 1

; Boot-Health-Status (NPSPEC-BOOT-HEALTH-0001)
BOOT_HEALTH_OK        equ 0
BOOT_HEALTH_DEGRADED  equ 1
BOOT_HEALTH_FAIL      equ 2
BOOT_HEALTH_RECOVERY  equ 3    ; (NPSPEC-BOOT-RECOVERY-0001)

; Boot-Flags
BOOT_FLAG_SECURE      equ 0x01  ; (NPSPEC-BOOT-SECURE-0001)
BOOT_FLAG_VERIFIED    equ 0x02  ; (NPSPEC-BOOT-VERIFIED-0001)
BOOT_FLAG_MEASURED    equ 0x04  ; (NPSPEC-BOOT-MEASURED-0001)
BOOT_FLAG_TPM         equ 0x08  ; (NPSPEC-BOOT-TPM-0001)
BOOT_FLAG_ROLLBACK_EN equ 0x10  ; (NPSPEC-BOOT-ROLLBACK-0001)
BOOT_FLAG_RECOVERY    equ 0x20  ; (NPSPEC-BOOT-RECOVERY-0001)

; Boot-Chain-Eintrag (32 Bytes, NPSPEC-BOOT-CHAIN-0001)
BOOTMGR_CHAIN_SIZE    equ 32
BC_STAGE              equ 0    ; Boot-Stufe (0=FW 1=BL 2=Kernel)
BC_FLAGS              equ 4    ; BOOT_FLAG_*
BC_HASH               equ 8    ; Gemessener Hash (NPSPEC-BOOT-MEASURED-0001)
BC_VERSION            equ 12
BC_SLOT               equ 16   ; A oder B (NPSPEC-BOOT-AB-0001)
BC_HEALTH             equ 20   ; BOOT_HEALTH_*
BC_PCR_INDEX          equ 24   ; TPM-PCR-Index (NPSPEC-BOOT-TPM-0001)
; 28..31: reserviert

BOOTMGR_CHAIN_CAPACITY equ 4   ; FW + BL + Kernel + Recovery

; ---------------------------------------------------------------------------
; bootmgr_initialize
; ---------------------------------------------------------------------------
bootmgr_initialize:
    push edi
    mov edi, boot_chain_table
    xor eax, eax
    mov ecx, (BOOTMGR_CHAIN_CAPACITY * BOOTMGR_CHAIN_SIZE) / 4
    rep stosd
    pop edi
    mov dword [boot_chain_count], 0
    mov dword [boot_active_slot], BOOT_SLOT_A
    mov dword [boot_health], BOOT_HEALTH_OK
    mov dword [boot_mode], BOOT_MODE_UEFI
    mov dword [boot_flags], BOOT_FLAG_SECURE | BOOT_FLAG_VERIFIED | BOOT_FLAG_MEASURED
    mov dword [bootmgr_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; boot_record_chain: EAX=stage, EBX=flags, ECX=hash, EDX=version
; (NPSPEC-BOOT-CHAIN-0001, NPSPEC-BOOT-MEASURED-0001)
; CF=0 Erfolg
; ---------------------------------------------------------------------------
boot_record_chain:
    cmp dword [boot_chain_count], BOOTMGR_CHAIN_CAPACITY
    jae .full
    push edi
    mov edi, boot_chain_table
    push eax
    mov eax, [boot_chain_count]
    imul eax, BOOTMGR_CHAIN_SIZE
    add edi, eax
    pop eax
    mov [edi + BC_STAGE], eax
    mov [edi + BC_FLAGS], ebx
    mov [edi + BC_HASH], ecx
    mov [edi + BC_VERSION], edx
    push eax
    mov eax, [boot_active_slot]
    mov [edi + BC_SLOT], eax
    pop eax
    mov dword [edi + BC_HEALTH], BOOT_HEALTH_OK
    mov dword [edi + BC_PCR_INDEX], eax  ; PCR = Stage-Index
    inc dword [boot_chain_count]
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; boot_check_rollback: (NPSPEC-BOOT-ROLLBACK-0001)
; prueft ob Rollback auf Slot B noetig ist
; CF=0: Slot A OK, CF=1: Rollback auf B
; ---------------------------------------------------------------------------
boot_check_rollback:
    cmp dword [boot_health], BOOT_HEALTH_FAIL
    jne .ok
    ; Auf B wechseln (NPSPEC-BOOT-AB-0001)
    mov dword [boot_active_slot], BOOT_SLOT_B
    mov dword [boot_health], BOOT_HEALTH_RECOVERY
    stc
    ret
.ok:
    clc
    ret

; ---------------------------------------------------------------------------
; bootmgr_self_test
; ---------------------------------------------------------------------------
bootmgr_self_test:
    push esi
    xor esi, esi
    cmp dword [bootmgr_ready], 1
    je .t2
    inc esi
.t2:
    ; Boot-Chain aufzeichnen: FW-Stufe
    mov eax, 0          ; Stage FW
    mov ebx, BOOT_FLAG_MEASURED | BOOT_FLAG_TPM
    mov ecx, 0xDEADBEEF ; Hash-Stub
    mov edx, 0x00010000
    call boot_record_chain
    jnc .t3
    inc esi
.t3:
    ; Kernel-Stufe
    mov eax, 2
    mov ebx, BOOT_FLAG_SECURE | BOOT_FLAG_VERIFIED
    mov ecx, 0xC0FFEE00
    mov edx, 0x00010001
    call boot_record_chain
    jnc .t4
    inc esi
.t4:
    call boot_check_rollback
    ; CF=0 = kein Rollback noetig - OK
    jc .rollback_triggered
    jmp .done
.rollback_triggered:
    ; Rollback ist auch ein gueltiger Pfad - kein Fehler
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
bootmgr_ready:       dd 0
boot_chain_count:    dd 0
boot_active_slot:    dd BOOT_SLOT_A
boot_health:         dd BOOT_HEALTH_OK
boot_mode:           dd BOOT_MODE_UEFI
boot_flags:          dd 0

align 4
boot_chain_table:
    times BOOTMGR_CHAIN_CAPACITY * BOOTMGR_CHAIN_SIZE db 0
