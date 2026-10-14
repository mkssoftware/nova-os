; ---------------------------------------------------------------------------
; Driver Discovery und Driver Lifecycle
; NPSPEC-DRIVER-DISCOVERY-0001 / NPSPEC-DRIVER-LIFECYCLE-0001
; NPSPEC-DRIVER-DMA-0001
; ---------------------------------------------------------------------------

; Discovery-Quellen
DISC_SRC_PCI     equ 0
DISC_SRC_ACPI    equ 1
DISC_SRC_USB     equ 2
DISC_SRC_VIRTUAL equ 3

; Lifecycle-Zustaende (Obermenge der ipc32.inc DRV_STATE_*)
DRV_LC_DISCOVERED equ 0
DRV_LC_MATCHED    equ 1
DRV_LC_LOADED     equ 2
DRV_LC_BOUND      equ 3
DRV_LC_RUNNING    equ 4
DRV_LC_SUSPENDED  equ 5
DRV_LC_STOPPING   equ 6
DRV_LC_UNBOUND    equ 7
DRV_LC_FAILED     equ 8

; PCI Device-Klassen fuer Discovery-Matching
PCI_CLASS_NVME   equ 0x010802   ; Mass Storage / NVM / NVMe
PCI_CLASS_XHCI   equ 0x0C0330   ; Serial Bus / USB / xHCI
PCI_CLASS_EHCI   equ 0x0C0320   ; Serial Bus / USB / EHCI
PCI_CLASS_OHCI   equ 0x0C0310   ; Serial Bus / USB / OHCI

; Discovery-Record-Layout (48 Bytes)
DISC_RECORD_SIZE  equ 48
DISC_REC_SRC      equ 0        ; Quelltyp
DISC_REC_BUS      equ 4        ; PCI-Bus (Bus<<16|Dev<<11|Func<<8)
DISC_REC_VENDOR   equ 8        ; PCI Vendor ID
DISC_REC_DEVICE   equ 12       ; PCI Device ID
DISC_REC_CLASS    equ 16       ; PCI Class (24-Bit: class|subclass|progif)
DISC_REC_STATE    equ 20       ; DRV_LC_*
DISC_REC_FLAGS    equ 24       ; Discovery-Flags
DISC_REC_HANDLE   equ 28       ; zugeordneter Treiber-Handle
DISC_REC_BAR0     equ 32       ; BAR0-Adresse (MMIO-Basis)
DISC_REC_IRQ      equ 36       ; IRQ-Nummer (0 = keiner)
DISC_REC_RESERVED equ 40       ; Reserviert

DISC_CAPACITY     equ 16       ; Maximal 16 erkannte Geraete

DISC_FLAG_MATCHED equ 0x00000001
DISC_FLAG_MMIO    equ 0x00000002
DISC_FLAG_MSI     equ 0x00000004

; ---------------------------------------------------------------------------
; driver_discovery_initialize
; Nullt die Discovery-Tabelle und setzt den Manager bereit.
; CF=0 Erfolg, CF=1 Fehler
; ---------------------------------------------------------------------------
driver_discovery_initialize:
    mov edi, disc_records
    xor eax, eax
    mov ecx, (DISC_CAPACITY * DISC_RECORD_SIZE) / 4
    rep stosd
    mov dword [disc_count], 0
    mov dword [disc_mgr_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; driver_discovery_self_test (5 Tests)
; ---------------------------------------------------------------------------
driver_discovery_self_test:
    push esi
    push edi
    xor edi, edi

    cmp dword [disc_mgr_ready], 1
    je .t2
    inc edi
.t2:
    cmp dword [disc_count], 0
    je .t3
    inc edi
.t3:
    ; Virtuelles Testgeraet registrieren
    mov eax, DISC_SRC_VIRTUAL
    xor edx, edx
    xor ebx, ebx
    xor ecx, ecx
    mov esi, 0
    call disc_register_device
    jnc .t4
    inc edi
    jmp .done
.t4:
    cmp dword [disc_count], 1
    je .t5
    inc edi
.t5:
    ; Registrierten Eintrag wieder auf Discovered setzen (Test-Cleanup)
    mov esi, disc_records
    mov dword [esi + DISC_REC_STATE], DRV_LC_UNBOUND
    mov dword [disc_count], 0
    push edi                    ; Fehlerzähler sichern – rep stosd clobbert EDI
    mov edi, disc_records
    xor eax, eax
    mov ecx, DISC_RECORD_SIZE / 4
    rep stosd
    pop edi                     ; Fehlerzähler wiederherstellen

.done:
    test edi, edi
    jz .ok
    pop edi
    pop esi
    stc
    ret
.ok:
    pop edi
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; disc_register_device
; EAX=source, EDX=pci_bus_addr, EBX=vendor_id, ECX=device_id, ESI=class24
; CF=0 Erfolg (ESI=Record-Zeiger), CF=1 wenn voll
; ---------------------------------------------------------------------------
disc_register_device:
    push edi
    cmp dword [disc_count], DISC_CAPACITY
    jae .full
    mov edi, disc_records
    push ecx
    mov ecx, DISC_CAPACITY
.scan:
    test ecx, ecx
    jz .no_slot
    cmp dword [edi + DISC_REC_STATE], DRV_LC_UNBOUND
    je .got_slot
    cmp dword [edi + DISC_REC_VENDOR], 0
    je .got_slot
    add edi, DISC_RECORD_SIZE
    dec ecx
    jmp .scan
.no_slot:
    pop ecx
    pop edi
    stc
    ret
.got_slot:
    pop ecx
    mov dword [edi + DISC_REC_SRC], eax
    mov dword [edi + DISC_REC_BUS], edx
    mov dword [edi + DISC_REC_VENDOR], ebx
    mov dword [edi + DISC_REC_DEVICE], ecx
    mov dword [edi + DISC_REC_CLASS], esi
    mov dword [edi + DISC_REC_STATE], DRV_LC_DISCOVERED
    mov dword [edi + DISC_REC_FLAGS], 0
    mov dword [edi + DISC_REC_HANDLE], 0
    mov dword [edi + DISC_REC_BAR0], 0
    mov dword [edi + DISC_REC_IRQ], 0
    inc dword [disc_count]
    mov esi, edi
    pop edi
    clc
    ret
.full:
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; driver_pci_scan
; Durchsucht den PCI-Bus nach NVMe- und xHCI-Controllern.
; Erkannte Geraete werden in disc_records eingetragen.
; CF=0 immer (fehlende Geraete sind kein Fehler per NPSPEC)
; ---------------------------------------------------------------------------
driver_pci_scan:
    pushad
    xor ebx, ebx          ; Bus<<16 | Device<<11 | Function<<8
.probe:
    mov eax, ebx
    call pci_config_read32
    cmp ax, 0xFFFF
    je .absent

    ; Class-Code lesen (Register 0x08, obere 3 Bytes)
    mov eax, ebx
    or eax, 0x08
    call pci_config_read32
    shr eax, 8
    mov [disc_tmp_class], eax

    ; Vendor/Device lesen (Register 0x00)
    mov eax, ebx
    call pci_config_read32
    movzx ecx, ax              ; Vendor ID
    shr eax, 16                ; Device ID
    mov [disc_tmp_vendor], ecx
    mov [disc_tmp_device], eax

    ; NVMe?
    cmp dword [disc_tmp_class], PCI_CLASS_NVME
    je .found_nvme
    ; xHCI?
    cmp dword [disc_tmp_class], PCI_CLASS_XHCI
    je .found_xhci
    jmp .present

.found_nvme:
    mov eax, DISC_SRC_PCI
    mov edx, ebx
    mov ecx, [disc_tmp_vendor]
    mov esi, [disc_tmp_class]
    push ecx
    mov ebx, [disc_tmp_device]
    xchg ebx, ecx            ; ebx=vendor, ecx=device
    call disc_register_device
    jc .present_restore
    ; BAR0 lesen und in Record eintragen
    mov eax, [esi + DISC_REC_BUS]
    or eax, 0x10
    call pci_config_read32
    and eax, 0xFFFFF000
    mov [esi + DISC_REC_BAR0], eax
    or dword [esi + DISC_REC_FLAGS], DISC_FLAG_MMIO
    mov dword [esi + DISC_REC_STATE], DRV_LC_MATCHED
    or dword [esi + DISC_REC_FLAGS], DISC_FLAG_MATCHED
    pop ecx
    jmp .present

.found_xhci:
    mov eax, DISC_SRC_PCI
    mov edx, ebx
    mov ecx, [disc_tmp_vendor]
    mov esi, [disc_tmp_class]
    push ecx
    mov ebx, [disc_tmp_device]
    xchg ebx, ecx
    call disc_register_device
    jc .present_restore
    ; BAR0
    mov eax, [esi + DISC_REC_BUS]
    or eax, 0x10
    call pci_config_read32
    and eax, 0xFFFFF000
    mov [esi + DISC_REC_BAR0], eax
    or dword [esi + DISC_REC_FLAGS], DISC_FLAG_MMIO
    mov dword [esi + DISC_REC_STATE], DRV_LC_MATCHED
    or dword [esi + DISC_REC_FLAGS], DISC_FLAG_MATCHED
    pop ecx
    jmp .present

.present_restore:
    pop ecx

.present:
    ; Multifunction-Check
    test ebx, 0x700
    jnz .next_function
    mov eax, ebx
    or eax, 0x0C
    call pci_config_read32
    test eax, 0x00800000
    jnz .next_function
    jmp .next_device

.absent:
    test ebx, 0x700
    jnz .next_function
.next_device:
    and ebx, 0xFFFFF800
    add ebx, 0x800
    jmp .bounds
.next_function:
    add ebx, 0x100
.bounds:
    cmp ebx, 0x01000000
    jb .probe

    popad
    clc
    ret

; ---------------------------------------------------------------------------
; driver_enable_bus_master: EAX=pci_bus_addr
; Setzt Memory Space Enable + Bus Master Enable
; ---------------------------------------------------------------------------
driver_enable_bus_master:
    push eax
    push ecx
    mov ecx, eax
    or ecx, 0x04
    mov eax, ecx
    call pci_config_read32
    or eax, 0x00000406       ; MemSpace + BusMaster + INTx-Disable
    xchg eax, ecx
    call pci_config_write32
    pop ecx
    pop eax
    ret

; ---------------------------------------------------------------------------
; Statische Daten
; ---------------------------------------------------------------------------
align 4
disc_mgr_ready:   dd 0
disc_count:       dd 0
disc_tmp_class:   dd 0
disc_tmp_vendor:  dd 0
disc_tmp_device:  dd 0
align 4
disc_records:
    times DISC_CAPACITY * DISC_RECORD_SIZE db 0
