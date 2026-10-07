; ---------------------------------------------------------------------------
; Nova Storage Bootstrap (NPSPEC-STORAGE-DEVICE-0001, NPSPEC-STORAGE-DISCOVERY-0001)
;
; PCI-Konfigurationszugriff (Mechanismus #1), AHCI-Controller im Polling-
; Betrieb, Block-Device-Schicht und GPT-Partitionssuche fuer das NovaFS-
; Systemvolume. Der Bootstrap-Treiber verwendet ausschliesslich
; identitaetsabgebildete PMM-Seiten unterhalb von 8 MiB als DMA-Ziel; die
; Anbindung an DMA-Mapping/IOMMU-Lifecycle folgt mit dem HAL-Storage-Provider.
;
; Discovery erzeugt keine Authority: Geraete werden registriert, aber erst der
; Filesystem-Mount entscheidet ueber die Verwendung.
; ---------------------------------------------------------------------------

PCI_CONFIG_ADDRESS        equ 0x0CF8
PCI_CONFIG_DATA           equ 0x0CFC
PCI_CLASS_AHCI            equ 0x010601     ; Mass Storage / SATA / AHCI 1.0

AHCI_CAP                  equ 0x00
AHCI_GHC                  equ 0x04
AHCI_IS                   equ 0x08
AHCI_PI                   equ 0x0C
AHCI_VS                   equ 0x10
AHCI_CAP2                 equ 0x24
AHCI_BOHC                 equ 0x28
AHCI_GHC_HR               equ 0x00000001
AHCI_GHC_IE               equ 0x00000002
AHCI_GHC_AE               equ 0x80000000
AHCI_CAP_SSS              equ 0x08000000
AHCI_CAP_S64A             equ 0x80000000

AHCI_PORT_BASE            equ 0x100
AHCI_PORT_SIZE            equ 0x80
AHCI_PX_CLB               equ 0x00
AHCI_PX_CLBU              equ 0x04
AHCI_PX_FB                equ 0x08
AHCI_PX_FBU               equ 0x0C
AHCI_PX_IS                equ 0x10
AHCI_PX_IE                equ 0x14
AHCI_PX_CMD               equ 0x18
AHCI_PX_TFD               equ 0x20
AHCI_PX_SIG               equ 0x24
AHCI_PX_SSTS              equ 0x28
AHCI_PX_SERR              equ 0x30
AHCI_PX_CI                equ 0x38
AHCI_PX_CMD_ST            equ 0x00000001
AHCI_PX_CMD_SUD           equ 0x00000002
AHCI_PX_CMD_POD           equ 0x00000004
AHCI_PX_CMD_FRE           equ 0x00000010
AHCI_PX_CMD_FR            equ 0x00004000
AHCI_PX_CMD_CR            equ 0x00008000
AHCI_PX_IS_TFES           equ 0x40000000
AHCI_TFD_ERR              equ 0x01
AHCI_TFD_DRQ              equ 0x08
AHCI_TFD_BSY              equ 0x80
AHCI_SIG_ATA              equ 0x00000101

ATA_CMD_IDENTIFY          equ 0xEC
ATA_CMD_READ_DMA_EXT      equ 0x25
ATA_CMD_WRITE_DMA_EXT     equ 0x35
ATA_CMD_FLUSH_CACHE_EXT   equ 0xEA

STORAGE_CAPACITY          equ 4
STORAGE_RECORD_SIZE       equ 64
STORAGE_DEVICE_ID         equ 0            ; stabile DeviceID (CRC32C Serial+Modell)
STORAGE_STATE             equ 4
STORAGE_PORT              equ 8
STORAGE_PORT_MMIO         equ 12
STORAGE_COMMAND_PAGE      equ 16
STORAGE_SECTORS_LO        equ 20
STORAGE_SECTORS_HI        equ 24
STORAGE_SECTOR_SIZE       equ 28
STORAGE_KERNEL_DEVICE     equ 32
STORAGE_READS             equ 36
STORAGE_WRITES            equ 40
STORAGE_ERRORS            equ 44
STORAGE_LAST_TFD          equ 48
STORAGE_FLAGS             equ 52
STORAGE_STATE_UNKNOWN     equ 0
STORAGE_STATE_READY       equ 1
STORAGE_STATE_FAILED      equ 2
STORAGE_FLAG_LBA48        equ 0x00000001
STORAGE_FLAG_WRITE_CACHE  equ 0x00000002

STORAGE_TIMEOUT_TICKS     equ 300          ; 3 s bei 100 Hz
STORAGE_LINK_TICKS        equ 10           ; 100 ms Link-Aufbau nach Reset
STORAGE_SPIN_LIMIT        equ 200000000    ; Schutz, falls Ticks stehen

DEVICE_CLASS_STORAGE_CONTROLLER equ 4
DEVICE_CLASS_BLOCK              equ 5
DEVICE_ID_AHCI                  equ 0x10
DEVICE_ID_BLOCK_BASE            equ 0x20

CRC_TABLE_CRC32C          equ 0
CRC_TABLE_CRC32           equ 1024

; ---------------------------------------------------------------------------
; CRC-Tabellen (CRC32C fuer NovaFS, CRC32/IEEE fuer GPT). Die Tabellen liegen
; in einer PMM-Seite, damit sie das Kernelabbild nicht vergroessern.
; ---------------------------------------------------------------------------
crc_tables_initialize:
    cmp dword [crc_table_page], 0
    jne .ready
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [crc_table_page], eax
    mov edi, eax
    xor ecx, ecx
.entry:
    mov eax, ecx
    mov ebx, eax
    mov edx, 8
.bit_c:
    shr eax, 1
    jnc .next_c
    xor eax, 0x82F63B78
.next_c:
    shr ebx, 1
    jnc .next_i
    xor ebx, 0xEDB88320
.next_i:
    dec edx
    jnz .bit_c
    mov [edi + CRC_TABLE_CRC32C + ecx * 4], eax
    mov [edi + CRC_TABLE_CRC32 + ecx * 4], ebx
    inc ecx
    cmp ecx, 256
    jb .entry
.ready:
    clc
    ret
.invalid:
    stc
    ret

; EAX=laufender Zustand, ESI=Daten, ECX=Laenge. EAX=neuer Zustand.
; Start mit 0xFFFFFFFF, Abschluss mit NOT.
crc32c_update:
    push ebx
    push ecx
    push esi
    push edi
    mov edi, [crc_table_page]
.byte:
    test ecx, ecx
    jz .done
    movzx ebx, byte [esi]
    xor bl, al
    shr eax, 8
    xor eax, [edi + CRC_TABLE_CRC32C + ebx * 4]
    inc esi
    dec ecx
    jmp .byte
.done:
    pop edi
    pop esi
    pop ecx
    pop ebx
    ret

crc32_ieee_update:
    push ebx
    push ecx
    push esi
    push edi
    mov edi, [crc_table_page]
.byte:
    test ecx, ecx
    jz .done
    movzx ebx, byte [esi]
    xor bl, al
    shr eax, 8
    xor eax, [edi + CRC_TABLE_CRC32 + ebx * 4]
    inc esi
    dec ecx
    jmp .byte
.done:
    pop edi
    pop esi
    pop ecx
    pop ebx
    ret

; ESI=Daten, ECX=Laenge -> EAX=CRC32C
crc32c_compute:
    mov eax, 0xFFFFFFFF
    call crc32c_update
    not eax
    ret

; Starttest mit den Standard-Pruefwerten fuer "123456789".
crc_self_test:
    mov esi, crc_check_string
    mov ecx, 9
    call crc32c_compute
    cmp eax, 0xE3069283
    jne .invalid
    mov esi, crc_check_string
    mov ecx, 9
    mov eax, 0xFFFFFFFF
    call crc32_ieee_update
    not eax
    cmp eax, 0xCBF43926
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; PCI-Konfigurationsraum
; ---------------------------------------------------------------------------

; EAX=Bus<<16 | Device<<11 | Function<<8 | Offset -> EAX=Wert
pci_config_read32:
    push edx
    and eax, 0x00FFFFFC
    or eax, 0x80000000
    mov dx, PCI_CONFIG_ADDRESS
    out dx, eax
    mov dx, PCI_CONFIG_DATA
    in eax, dx
    pop edx
    ret

; EAX=Adresse wie oben, ECX=Wert
pci_config_write32:
    push edx
    push eax
    and eax, 0x00FFFFFC
    or eax, 0x80000000
    mov dx, PCI_CONFIG_ADDRESS
    out dx, eax
    mov dx, PCI_CONFIG_DATA
    mov eax, ecx
    out dx, eax
    pop eax
    pop edx
    ret

; Durchsucht alle Busse nach dem ersten AHCI-Controller.
; EAX=PCI-Adresse (ohne Offset) bei Erfolg, CF=1 wenn keiner vorhanden ist.
pci_find_ahci:
    pushad
    xor ebx, ebx                    ; Bus<<16 | Device<<11 | Function<<8
.probe:
    mov eax, ebx
    call pci_config_read32
    cmp ax, 0xFFFF
    je .absent
    mov eax, ebx
    or eax, 0x08
    call pci_config_read32
    shr eax, 8
    cmp eax, PCI_CLASS_AHCI
    jne .present
    mov [esp + 28], ebx
    popad
    clc
    ret
.present:
    ; Nur Funktion 0 entscheidet ueber Multifunction-Geraete.
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
    stc
    ret

; ---------------------------------------------------------------------------
; Zeitbasis fuer Polling. EBX=Startticks, ECX=Spin-Zaehler.
; CF=1 sobald das Limit (EDX Ticks) erreicht ist.
; ---------------------------------------------------------------------------
storage_timeout_check:
    dec ecx
    jz .expired
    mov eax, [timer_ticks]
    sub eax, ebx
    cmp eax, edx
    jae .expired
    pause
    clc
    ret
.expired:
    stc
    ret

; ESI=MMIO-Adresse, EDI=Maske, EBP=Erwartungswert, EDX=Ticks. CF bei Timeout.
storage_wait_mmio:
    push eax
    push ebx
    push ecx
    mov ebx, [timer_ticks]
    mov ecx, STORAGE_SPIN_LIMIT
.poll:
    mov eax, [esi]
    and eax, edi
    cmp eax, ebp
    je .done
    call storage_timeout_check
    jnc .poll
    pop ecx
    pop ebx
    pop eax
    stc
    ret
.done:
    pop ecx
    pop ebx
    pop eax
    clc
    ret

; ---------------------------------------------------------------------------
; Storage-Initialisierung (Bootphase Device Discovery)
; ---------------------------------------------------------------------------
storage_initialize:
    mov dword [storage_count], 0
    mov dword [storage_ahci_present], 0
    mov dword [storage_ahci_abar], 0
    mov edi, storage_records
    xor eax, eax
    mov ecx, (STORAGE_CAPACITY * STORAGE_RECORD_SIZE) / 4
    rep stosd
    call crc_tables_initialize
    jc .invalid
    call crc_self_test
    jc .invalid

    call pci_find_ahci
    jc .no_controller
    mov [storage_ahci_pci], eax
    mov dword [storage_ahci_present], 1

    ; BAR5 (ABAR) muss ein 32-Bit-erreichbares Memory-BAR sein.
    mov eax, [storage_ahci_pci]
    or eax, 0x24
    call pci_config_read32
    test eax, 1
    jnz .controller_failed
    mov edx, eax
    and edx, 0x06
    cmp edx, 0x04
    jne .bar_ready
    mov eax, [storage_ahci_pci]
    or eax, 0x28
    call pci_config_read32
    test eax, eax
    jnz .controller_failed
    mov eax, [storage_ahci_pci]
    or eax, 0x24
    call pci_config_read32
.bar_ready:
    and eax, 0xFFFFF000
    jz .controller_failed
    mov [storage_ahci_abar], eax

    ; Memory Space und Bus Master aktivieren, INTx abschalten (Polling).
    mov eax, [storage_ahci_pci]
    or eax, 0x04
    call pci_config_read32
    or eax, 0x00000406
    mov ecx, eax
    and ecx, 0x0000FFFF
    mov eax, [storage_ahci_pci]
    or eax, 0x04
    call pci_config_write32

    ; ABAR (0x1100 Byte) identisch und ungecacht abbilden.
    mov esi, [storage_ahci_abar]
    mov ecx, 2
.map:
    mov eax, esi
    mov edx, esi
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE | 0x18
    push ecx
    push esi
    call paging_map_page
    pop esi
    pop ecx
    jc .controller_failed
    invlpg [esi]
    add esi, PMM_PAGE_SIZE
    loop .map

    call ahci_controller_reset
    jc .controller_failed

    mov eax, DEVICE_ID_AHCI
    mov edx, DEVICE_CLASS_STORAGE_CONTROLLER
    mov ebx, [storage_ahci_abar]
    mov ecx, 0x2000
    call device_register
    jc .controller_failed

    call ahci_probe_ports
    mov esi, message_storage_ahci_ok
    call serial_write_string
    mov eax, [storage_count]
    call serial_write_hex32
    mov esi, message_line_end
    call serial_write_string
    clc
    ret
.no_controller:
    mov esi, message_storage_no_ahci
    call serial_write_string
    clc
    ret
.controller_failed:
    mov dword [storage_ahci_present], 0
    mov esi, message_storage_ahci_failed
    call serial_write_string
    clc
    ret
.invalid:
    stc
    ret

; HBA-Reset, danach AHCI-Modus ohne Interrupts.
ahci_controller_reset:
    mov esi, [storage_ahci_abar]
    ; BIOS/OS-Handoff, falls vom Controller angeboten.
    test dword [esi + AHCI_CAP2], 1
    jz .handoff_done
    or dword [esi + AHCI_BOHC], 0x02
    push esi
    lea esi, [esi + AHCI_BOHC]
    mov edi, 0x01
    xor ebp, ebp
    mov edx, 200
    call storage_wait_mmio
    pop esi
.handoff_done:
    or dword [esi + AHCI_GHC], AHCI_GHC_AE
    or dword [esi + AHCI_GHC], AHCI_GHC_HR
    push esi
    lea esi, [esi + AHCI_GHC]
    mov edi, AHCI_GHC_HR
    xor ebp, ebp
    mov edx, 100
    call storage_wait_mmio
    pop esi
    jc .invalid
    or dword [esi + AHCI_GHC], AHCI_GHC_AE
    and dword [esi + AHCI_GHC], ~AHCI_GHC_IE
    mov dword [esi + AHCI_IS], 0xFFFFFFFF
    clc
    ret
.invalid:
    stc
    ret

; Startet alle implementierten Ports, wartet einmal gemeinsam auf den Link und
; registriert anschliessend erkannte ATA-Datentraeger.
ahci_probe_ports:
    mov esi, [storage_ahci_abar]
    mov eax, [esi + AHCI_PI]
    mov [storage_ports_implemented], eax
    xor ecx, ecx
.spin_up:
    bt dword [storage_ports_implemented], ecx
    jnc .next_spin
    mov edi, ecx
    shl edi, 7
    lea edi, [esi + AHCI_PORT_BASE + edi]
    mov dword [edi + AHCI_PX_SERR], 0xFFFFFFFF
    mov dword [edi + AHCI_PX_IS], 0xFFFFFFFF
    mov dword [edi + AHCI_PX_IE], 0
    or dword [edi + AHCI_PX_CMD], AHCI_PX_CMD_SUD | AHCI_PX_CMD_POD
.next_spin:
    inc ecx
    cmp ecx, 32
    jb .spin_up

    ; gemeinsames Link-Fenster: endet frueh, wenn alle Ports DET=3 melden
    mov ebx, [timer_ticks]
    mov ecx, STORAGE_SPIN_LIMIT
.link_wait:
    call ahci_all_links_up
    jnc .links_done
    mov edx, STORAGE_LINK_TICKS
    call storage_timeout_check
    jnc .link_wait
.links_done:

    xor ecx, ecx
.port:
    cmp dword [storage_count], STORAGE_CAPACITY
    jae .done
    bt dword [storage_ports_implemented], ecx
    jnc .next_port
    push ecx
    call ahci_port_attach
    pop ecx
.next_port:
    inc ecx
    cmp ecx, 32
    jb .port
.done:
    ret

; CF=0 wenn alle implementierten Ports eine etablierte Verbindung melden.
ahci_all_links_up:
    push ecx
    push edi
    xor ecx, ecx
.scan:
    bt dword [storage_ports_implemented], ecx
    jnc .next
    mov edi, ecx
    shl edi, 7
    add edi, [storage_ahci_abar]
    mov eax, [edi + AHCI_PORT_BASE + AHCI_PX_SSTS]
    and eax, 0x0F
    cmp eax, 3
    jne .not_ready
.next:
    inc ecx
    cmp ecx, 32
    jb .scan
    pop edi
    pop ecx
    clc
    ret
.not_ready:
    pop edi
    pop ecx
    stc
    ret

; ECX=Portnummer. Fehler an einzelnen Ports beenden die Discovery nicht.
ahci_port_attach:
    pushad
    mov [storage_temp_port], ecx
    mov edi, ecx
    shl edi, 7
    add edi, [storage_ahci_abar]
    add edi, AHCI_PORT_BASE
    mov [storage_temp_mmio], edi
    mov eax, [edi + AHCI_PX_SSTS]
    and eax, 0x0F
    cmp eax, 3
    jne .skip

    call ahci_port_stop
    jc .skip

    ; Kommandoliste (1 KiB), Received FIS (256 Byte) und Kommandotabelle in
    ; einer gemeinsamen, genullten Seite.
    call pmm_alloc_page
    test eax, eax
    jz .skip
    mov [storage_temp_page], eax
    mov edi, eax
    push eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    pop eax
    mov edi, [storage_temp_mmio]
    mov [edi + AHCI_PX_CLB], eax
    mov dword [edi + AHCI_PX_CLBU], 0
    add eax, 0x400
    mov [edi + AHCI_PX_FB], eax
    mov dword [edi + AHCI_PX_FBU], 0
    mov dword [edi + AHCI_PX_SERR], 0xFFFFFFFF
    mov dword [edi + AHCI_PX_IS], 0xFFFFFFFF
    or dword [edi + AHCI_PX_CMD], AHCI_PX_CMD_FRE

    ; Geraet muss BSY/DRQ verlassen haben und eine ATA-Signatur melden.
    lea esi, [edi + AHCI_PX_TFD]
    push edi
    mov edi, AHCI_TFD_BSY | AHCI_TFD_DRQ
    xor ebp, ebp
    mov edx, STORAGE_TIMEOUT_TICKS
    call storage_wait_mmio
    pop edi
    jc .release
    cmp dword [edi + AHCI_PX_SIG], AHCI_SIG_ATA
    jne .release
    or dword [edi + AHCI_PX_CMD], AHCI_PX_CMD_ST

    ; Datensatz vorbereiten
    mov eax, [storage_count]
    imul ebx, eax, STORAGE_RECORD_SIZE
    add ebx, storage_records
    mov [storage_temp_record], ebx
    mov eax, [storage_temp_port]
    mov [ebx + STORAGE_PORT], eax
    mov eax, [storage_temp_mmio]
    mov [ebx + STORAGE_PORT_MMIO], eax
    mov eax, [storage_temp_page]
    mov [ebx + STORAGE_COMMAND_PAGE], eax
    mov dword [ebx + STORAGE_SECTOR_SIZE], 512
    mov dword [ebx + STORAGE_STATE], STORAGE_STATE_UNKNOWN

    ; IDENTIFY DEVICE in eine eigene Seite
    call pmm_alloc_page
    test eax, eax
    jz .release
    mov [storage_temp_identify], eax
    mov dword [storage_cmd_lba_lo], 0
    mov dword [storage_cmd_lba_hi], 0
    mov dword [storage_cmd_count], 0
    mov [storage_cmd_buffer], eax
    mov dword [storage_cmd_bytes], 512
    mov byte [storage_cmd_opcode], ATA_CMD_IDENTIFY
    mov byte [storage_cmd_write], 0
    mov esi, ebx
    call ahci_execute
    jc .release_identify

    mov esi, [storage_temp_identify]
    ; Wort 83 Bit 10: 48-Bit-Adressierung (fuer Phase 1 verpflichtend)
    test word [esi + 83 * 2], 0x0400
    jz .release_identify
    ; Wort 106: logische Sektorgroesse muss 512 Byte sein
    movzx eax, word [esi + 106 * 2]
    test eax, 0x4000
    jz .sector_ok
    test eax, 0x8000
    jnz .sector_ok
    test eax, 0x1000
    jnz .release_identify
.sector_ok:
    mov eax, [esi + 100 * 2]
    mov edx, [esi + 102 * 2]
    mov [ebx + STORAGE_SECTORS_LO], eax
    mov [ebx + STORAGE_SECTORS_HI], edx
    or eax, edx
    jz .release_identify
    or dword [ebx + STORAGE_FLAGS], STORAGE_FLAG_LBA48
    test word [esi + 85 * 2], 0x0020
    jz .identity
    or dword [ebx + STORAGE_FLAGS], STORAGE_FLAG_WRITE_CACHE
.identity:
    ; Stabile DeviceID aus Seriennummer (Worte 10-19) und Modell (27-46),
    ; unabhaengig von Port, Bus oder Mountpoint.
    lea esi, [esi + 10 * 2]
    mov ecx, 20
    mov eax, 0xFFFFFFFF
    call crc32c_update
    mov esi, [storage_temp_identify]
    lea esi, [esi + 27 * 2]
    mov ecx, 40
    call crc32c_update
    not eax
    test eax, eax
    jnz .id_ready
    inc eax
.id_ready:
    mov [ebx + STORAGE_DEVICE_ID], eax
    mov eax, [storage_temp_identify]
    call pmm_free_page

    mov eax, [storage_count]
    add eax, DEVICE_ID_BLOCK_BASE
    mov [ebx + STORAGE_KERNEL_DEVICE], eax
    mov edx, DEVICE_CLASS_BLOCK
    mov ebx, [storage_temp_port]
    mov esi, [storage_temp_record]
    mov ecx, [esi + STORAGE_SECTORS_LO]
    call device_register
    mov ebx, [storage_temp_record]
    jc .release
    mov dword [ebx + STORAGE_STATE], STORAGE_STATE_READY
    inc dword [storage_count]

    mov esi, message_storage_disk
    call serial_write_string
    mov eax, [ebx + STORAGE_PORT]
    call serial_write_hex32
    mov esi, message_storage_disk_id
    call serial_write_string
    mov eax, [ebx + STORAGE_DEVICE_ID]
    call serial_write_hex32
    mov esi, message_storage_disk_sectors
    call serial_write_string
    mov eax, [ebx + STORAGE_SECTORS_LO]
    call serial_write_hex32
    mov esi, message_line_end
    call serial_write_string
    popad
    ret

.release_identify:
    mov eax, [storage_temp_identify]
    call pmm_free_page
.release:
    mov edi, [storage_temp_mmio]
    call ahci_port_stop_at
    mov eax, [storage_temp_page]
    call pmm_free_page
.skip:
    popad
    ret

; EDI wird aus [storage_temp_mmio] geladen.
ahci_port_stop:
    mov edi, [storage_temp_mmio]
ahci_port_stop_at:
    and dword [edi + AHCI_PX_CMD], ~AHCI_PX_CMD_ST
    push edi
    lea esi, [edi + AHCI_PX_CMD]
    mov edi, AHCI_PX_CMD_CR
    xor ebp, ebp
    mov edx, 50
    call storage_wait_mmio
    pop edi
    jc .invalid
    and dword [edi + AHCI_PX_CMD], ~AHCI_PX_CMD_FRE
    push edi
    lea esi, [edi + AHCI_PX_CMD]
    mov edi, AHCI_PX_CMD_FR
    xor ebp, ebp
    mov edx, 50
    call storage_wait_mmio
    pop edi
    jc .invalid
    clc
    ret
.invalid:
    stc
    ret

; Fuehrt genau ein synchrones Kommando in Slot 0 aus.
; ESI=Storage-Datensatz, Parameter in storage_cmd_*. CF bei Fehler.
ahci_execute:
    pushad
    mov ebp, esi
    mov edi, [ebp + STORAGE_PORT_MMIO]
    ; Port darf keinen laufenden Befehl besitzen.
    test dword [edi + AHCI_PX_CI], 1
    jnz .failed
    mov ebx, [ebp + STORAGE_COMMAND_PAGE]

    ; Command Header Slot 0: CFL=5 DWORDs, W-Bit, PRDTL=1
    mov eax, 5 | (1 << 16)
    cmp byte [storage_cmd_write], 0
    je .header
    or eax, 0x40
.header:
    mov [ebx + 0], eax
    mov dword [ebx + 4], 0
    lea eax, [ebx + 0x800]
    mov [ebx + 8], eax
    mov dword [ebx + 12], 0

    ; Kommandotabelle: Register H2D FIS
    lea esi, [ebx + 0x800]
    push edi
    mov edi, esi
    xor eax, eax
    mov ecx, 0x90 / 4
    rep stosd
    pop edi
    mov byte [esi + 0], 0x27
    mov byte [esi + 1], 0x80
    mov al, [storage_cmd_opcode]
    mov [esi + 2], al
    mov eax, [storage_cmd_lba_lo]
    mov [esi + 4], al
    mov [esi + 5], ah
    shr eax, 16
    mov [esi + 6], al
    mov byte [esi + 7], 0x40         ; LBA-Modus
    mov [esi + 8], ah
    mov eax, [storage_cmd_lba_hi]
    mov [esi + 9], al
    mov [esi + 10], ah
    mov eax, [storage_cmd_count]
    mov [esi + 12], al
    mov [esi + 13], ah

    ; Physical Region Descriptor
    mov ecx, [storage_cmd_bytes]
    test ecx, ecx
    jz .no_prd
    mov eax, [storage_cmd_buffer]
    mov [esi + 0x80], eax
    mov dword [esi + 0x84], 0
    mov dword [esi + 0x88], 0
    dec ecx
    mov [esi + 0x8C], ecx
    jmp .issue
.no_prd:
    and dword [ebx + 0], 0x0000FFFF  ; PRDTL=0
.issue:
    mov dword [edi + AHCI_PX_IS], 0xFFFFFFFF
    ; Speicherreihenfolge vor dem Doorbell sicherstellen
    lock or dword [esp], 0
    mov dword [edi + AHCI_PX_CI], 1

    mov ebx, [timer_ticks]
    mov ecx, STORAGE_SPIN_LIMIT
    mov edx, STORAGE_TIMEOUT_TICKS
.poll:
    test dword [edi + AHCI_PX_IS], AHCI_PX_IS_TFES
    jnz .task_error
    test dword [edi + AHCI_PX_CI], 1
    jz .completed
    call storage_timeout_check
    jnc .poll
    jmp .task_error
.completed:
    mov eax, [edi + AHCI_PX_TFD]
    mov [ebp + STORAGE_LAST_TFD], eax
    test eax, AHCI_TFD_ERR | AHCI_TFD_BSY | AHCI_TFD_DRQ
    jnz .task_error
    popad
    clc
    ret
.task_error:
    mov eax, [edi + AHCI_PX_TFD]
    mov [ebp + STORAGE_LAST_TFD], eax
    inc dword [ebp + STORAGE_ERRORS]
    ; Port kontrolliert neu starten, damit spaetere Kommandos moeglich bleiben.
    call ahci_port_stop_at
    mov dword [edi + AHCI_PX_SERR], 0xFFFFFFFF
    mov dword [edi + AHCI_PX_IS], 0xFFFFFFFF
    or dword [edi + AHCI_PX_CMD], AHCI_PX_CMD_FRE
    or dword [edi + AHCI_PX_CMD], AHCI_PX_CMD_ST
.failed:
    popad
    stc
    ret

; ---------------------------------------------------------------------------
; Block-Device-API
; EAX=Storage-Index, EDX=LBA (32 Bit), ECX=Sektoren (1..8), EDI=Puffer
; (identitaetsabgebildete, physisch zusammenhaengende Seite). CF bei Fehler.
; Fehlgeschlagene I/O gilt nie als Erfolg (NPSPEC-STORAGE-DEVICE-0001 Nr. 16).
; ---------------------------------------------------------------------------
storage_read:
    mov byte [storage_cmd_opcode], ATA_CMD_READ_DMA_EXT
    mov byte [storage_cmd_write], 0
    jmp storage_transfer
storage_write:
    mov byte [storage_cmd_opcode], ATA_CMD_WRITE_DMA_EXT
    mov byte [storage_cmd_write], 1
storage_transfer:
    pushad
    cmp eax, [storage_count]
    jae .invalid
    test ecx, ecx
    jz .invalid
    cmp ecx, 8
    ja .invalid
    test edi, 3
    jnz .invalid
    imul esi, eax, STORAGE_RECORD_SIZE
    add esi, storage_records
    cmp dword [esi + STORAGE_STATE], STORAGE_STATE_READY
    jne .invalid
    ; Bereich gegen die Kapazitaet pruefen
    cmp dword [esi + STORAGE_SECTORS_HI], 0
    jne .range_ok
    mov eax, edx
    add eax, ecx
    jc .invalid
    cmp eax, [esi + STORAGE_SECTORS_LO]
    ja .invalid
.range_ok:
    mov [storage_cmd_lba_lo], edx
    mov dword [storage_cmd_lba_hi], 0
    mov [storage_cmd_count], ecx
    mov [storage_cmd_buffer], edi
    shl ecx, 9
    mov [storage_cmd_bytes], ecx
    call ahci_execute
    jc .invalid
    cmp byte [storage_cmd_write], 0
    je .count_read
    inc dword [esi + STORAGE_WRITES]
    popad
    clc
    ret
.count_read:
    inc dword [esi + STORAGE_READS]
    popad
    clc
    ret
.invalid:
    popad
    stc
    ret

; EAX=Storage-Index. Schreibt den Geraetecache zurueck (Ordnungspunkt).
storage_flush:
    pushad
    cmp eax, [storage_count]
    jae .invalid
    imul esi, eax, STORAGE_RECORD_SIZE
    add esi, storage_records
    cmp dword [esi + STORAGE_STATE], STORAGE_STATE_READY
    jne .invalid
    mov byte [storage_cmd_opcode], ATA_CMD_FLUSH_CACHE_EXT
    mov byte [storage_cmd_write], 0
    mov dword [storage_cmd_lba_lo], 0
    mov dword [storage_cmd_lba_hi], 0
    mov dword [storage_cmd_count], 0
    mov dword [storage_cmd_bytes], 0
    call ahci_execute
    jc .invalid
    popad
    clc
    ret
.invalid:
    popad
    stc
    ret

; ---------------------------------------------------------------------------
; GPT: NovaFS-Systempartition suchen (NPSPEC-NOVAFS-ONDISK-0001 §2)
; EAX=Storage-Index. Ergebnis: EAX=erste LBA, EDX=Sektoranzahl. CF wenn keine
; gueltige Partition existiert. Header-CRC und Eintrags-CRC werden geprueft.
; ---------------------------------------------------------------------------
storage_find_novafs_partition:
    pushad
    mov [storage_gpt_device], eax
    call pmm_alloc_page
    test eax, eax
    jz .fail_nopage
    mov [storage_gpt_page], eax

    ; GPT-Header in LBA 1
    mov eax, [storage_gpt_device]
    mov edx, 1
    mov ecx, 1
    mov edi, [storage_gpt_page]
    call storage_read
    jc .fail
    mov esi, [storage_gpt_page]
    cmp dword [esi + 0], 0x20494645  ; "EFI "
    jne .fail
    cmp dword [esi + 4], 0x54524150  ; "PART"
    jne .fail
    mov ecx, [esi + 12]
    cmp ecx, 92
    jb .fail
    cmp ecx, 512
    ja .fail
    mov eax, [esi + 16]
    mov [storage_gpt_crc_saved], eax
    mov dword [esi + 16], 0
    mov eax, 0xFFFFFFFF
    call crc32_ieee_update
    not eax
    mov edx, [storage_gpt_crc_saved]
    mov [esi + 16], edx
    cmp eax, edx
    jne .fail
    cmp dword [esi + 76], 0          ; Eintrags-LBA < 2^32
    jne .fail
    cmp dword [esi + 84], 128        ; Eintragsgroesse
    jne .fail
    mov eax, [esi + 80]              ; Anzahl Eintraege
    test eax, eax
    jz .fail
    cmp eax, 128
    ja .fail
    mov [storage_gpt_entries], eax
    mov eax, [esi + 72]
    mov [storage_gpt_entry_lba], eax
    mov eax, [esi + 88]
    mov [storage_gpt_entries_crc], eax

    ; Eintraege sektorweise lesen (4 Eintraege je Sektor), CRC fortlaufend.
    mov dword [storage_gpt_found], 0
    mov dword [storage_gpt_index], 0
    mov dword [storage_gpt_crc_state], 0xFFFFFFFF
.sector:
    mov eax, [storage_gpt_index]
    cmp eax, [storage_gpt_entries]
    jae .entries_done
    shr eax, 2
    add eax, [storage_gpt_entry_lba]
    mov edx, eax
    mov eax, [storage_gpt_device]
    mov ecx, 1
    mov edi, [storage_gpt_page]
    call storage_read
    jc .fail
    xor ebx, ebx
.entry:
    mov eax, [storage_gpt_index]
    cmp eax, [storage_gpt_entries]
    jae .entries_done
    mov esi, [storage_gpt_page]
    shl ebx, 7
    add esi, ebx
    shr ebx, 7
    push esi
    mov ecx, 128
    mov eax, [storage_gpt_crc_state]
    call crc32_ieee_update
    mov [storage_gpt_crc_state], eax
    pop esi
    cmp dword [storage_gpt_found], 0
    jne .next_entry
    push esi
    mov edi, novafs_partition_type_guid
    mov ecx, 4
    repe cmpsd
    pop esi
    jne .next_entry
    ; Bereich: erste/letzte LBA < 2^32, Start auf 4-KiB-Grenze
    cmp dword [esi + 36], 0
    jne .next_entry
    cmp dword [esi + 44], 0
    jne .next_entry
    mov eax, [esi + 32]
    mov edx, [esi + 40]
    cmp edx, eax
    jb .next_entry
    test eax, 7
    jnz .next_entry
    sub edx, eax
    inc edx
    mov [storage_gpt_part_first], eax
    mov [storage_gpt_part_sectors], edx
    mov dword [storage_gpt_found], 1
.next_entry:
    inc dword [storage_gpt_index]
    inc ebx
    cmp ebx, 4
    jb .entry
    jmp .sector
.entries_done:
    mov eax, [storage_gpt_crc_state]
    not eax
    cmp eax, [storage_gpt_entries_crc]
    jne .fail
    cmp dword [storage_gpt_found], 1
    jne .fail
    ; Partition muss vollstaendig auf dem Datentraeger liegen.
    mov eax, [storage_gpt_device]
    imul esi, eax, STORAGE_RECORD_SIZE
    add esi, storage_records
    cmp dword [esi + STORAGE_SECTORS_HI], 0
    jne .bounds_ok
    mov eax, [storage_gpt_part_first]
    add eax, [storage_gpt_part_sectors]
    jc .fail
    cmp eax, [esi + STORAGE_SECTORS_LO]
    ja .fail
.bounds_ok:
    mov eax, [storage_gpt_page]
    call pmm_free_page
    mov eax, [storage_gpt_part_first]
    mov [esp + 28], eax
    mov eax, [storage_gpt_part_sectors]
    mov [esp + 20], eax
    popad
    clc
    ret
.fail:
    mov eax, [storage_gpt_page]
    call pmm_free_page
.fail_nopage:
    popad
    stc
    ret

; Starttest: ohne Datentraeger erfolgreich; mit Datentraeger muss LBA 0 per
; DMA lesbar sein und ein Bereich jenseits der Kapazitaet abgewiesen werden.
storage_self_test:
    cmp dword [storage_count], 0
    je .done
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov edi, eax
    push eax
    xor eax, eax
    xor edx, edx
    mov ecx, 1
    call storage_read
    pop eax
    jc .free_invalid
    push eax
    mov edi, eax
    xor eax, eax
    mov edx, [storage_records + STORAGE_SECTORS_LO]
    mov ecx, 1
    cmp dword [storage_records + STORAGE_SECTORS_HI], 0
    jne .range_skip
    call storage_read
    pop eax
    jnc .free_invalid
    jmp .range_done
.range_skip:
    pop eax
.range_done:
    call pmm_free_page
    mov esi, message_storage_selftest_ok
    call serial_write_string
.done:
    clc
    ret
.free_invalid:
    call pmm_free_page
.invalid:
    stc
    ret

align 4
storage_api:
    dd 32
    dw 1, 0
    dd STORAGE_CAPACITY
    dd storage_read
    dd storage_write
    dd storage_flush
    dd storage_count
    dd storage_records

novafs_partition_type_guid:
    ; 4E4F5641-4653-5359-5354-454D30303031 in GPT-Bytefolge
    db 0x41, 0x56, 0x4F, 0x4E, 0x53, 0x46, 0x59, 0x53
    db 0x53, 0x54, 0x45, 0x4D, 0x30, 0x30, 0x30, 0x31

crc_check_string:        db "123456789"
align 4
crc_table_page:          dd 0
storage_count:           dd 0
storage_ahci_present:    dd 0
storage_ahci_pci:        dd 0
storage_ahci_abar:       dd 0
storage_ports_implemented: dd 0
storage_temp_port:       dd 0
storage_temp_mmio:       dd 0
storage_temp_page:       dd 0
storage_temp_record:     dd 0
storage_temp_identify:   dd 0
storage_cmd_lba_lo:      dd 0
storage_cmd_lba_hi:      dd 0
storage_cmd_count:       dd 0
storage_cmd_buffer:      dd 0
storage_cmd_bytes:       dd 0
storage_cmd_opcode:      db 0
storage_cmd_write:       db 0
align 4
storage_gpt_device:      dd 0
storage_gpt_page:        dd 0
storage_gpt_crc_saved:   dd 0
storage_gpt_entries:     dd 0
storage_gpt_entry_lba:   dd 0
storage_gpt_entries_crc: dd 0
storage_gpt_found:       dd 0
storage_gpt_index:       dd 0
storage_gpt_crc_state:   dd 0
storage_gpt_part_first:  dd 0
storage_gpt_part_sectors: dd 0
storage_records:
    times STORAGE_CAPACITY * STORAGE_RECORD_SIZE db 0
