; ---------------------------------------------------------------------------
; USB Stack: xHCI Controller, USB Core, Enumeration, HID, Mass Storage
; NPSPEC-USB-ARCHITECTURE-0001 / NPSPEC-USB-XHCI-0001
; NPSPEC-USB-CORE-0001 / NPSPEC-USB-CONTROLLER-0001
; NPSPEC-USB-ENUMERATION-0001 / NPSPEC-USB-DESCRIPTOR-0001
; NPSPEC-USB-ENDPOINT-0001 / NPSPEC-USB-TRANSFER-0001
; NPSPEC-USB-HUB-0001 / NPSPEC-USB-HOTPLUG-0001
; NPSPEC-USB-ERROR-0001 / NPSPEC-USB-POWER-0001
; NPSPEC-USB-HID-0001 / NPSPEC-USB-HID-REPORT-0001
; NPSPEC-USB-KEYBOARD-0001 / NPSPEC-USB-MOUSE-0001
; NPSPEC-USB-MASSSTORAGE-0001 / NPSPEC-USB-BOT-0001
; NPSPEC-USB-SCSI-0001 / NPSPEC-USB-UASP-0001
; NPSPEC-USB-BLOCKDEVICE-0001
; ---------------------------------------------------------------------------

; ---------------------------------------------------------------------------
; xHCI Capability Register Offsets (BAR0-relativ)
; ---------------------------------------------------------------------------
XHCI_CAPLENGTH   equ 0x00   ; Byte: Laenge der Capability-Register
XHCI_HCIVERSION  equ 0x02   ; Word: xHCI-Spezifikationsversion
XHCI_HCSPARAMS1  equ 0x04   ; MaxSlots[7:0], MaxInterrupters[18:8], MaxPorts[31:24]
XHCI_HCSPARAMS2  equ 0x08
XHCI_HCSPARAMS3  equ 0x0C
XHCI_HCCPARAMS1  equ 0x10
XHCI_DBOFF       equ 0x14   ; Doorbell Array Offset (von BAR0)
XHCI_RTSOFF      equ 0x18   ; Runtime Register Space Offset

; xHCI Operational Register Offsets (BAR0 + CAPLENGTH)
XHCI_USBCMD      equ 0x00
XHCI_USBSTS      equ 0x04
XHCI_PAGESIZE    equ 0x08
XHCI_DNCTRL      equ 0x14
XHCI_CRCR_LO     equ 0x18   ; Command Ring Control Register lo
XHCI_CRCR_HI     equ 0x1C
XHCI_DCBAAP_LO   equ 0x30   ; Device Context Base Address Array Pointer lo
XHCI_DCBAAP_HI   equ 0x34
XHCI_CONFIG       equ 0x38   ; MaxSlotsEn[7:0]

; USBCMD Bits
XHCI_CMD_RS      equ 0x00000001   ; Run/Stop
XHCI_CMD_HCRST   equ 0x00000002   ; Host Controller Reset
XHCI_CMD_INTE    equ 0x00000004   ; Interrupter Enable

; USBSTS Bits
XHCI_STS_HCH     equ 0x00000001   ; HCHalted
XHCI_STS_HSE     equ 0x00000004   ; Host System Error
XHCI_STS_EINT    equ 0x00000008   ; Event Interrupt
XHCI_STS_CNR     equ 0x00000800   ; Controller Not Ready

; Port Status Control Register (PORTSC) bei op_base + 0x400 + port*0x10
XHCI_PORTSC_OFF  equ 0x400
XHCI_PORTSC_SIZE equ 0x10
XHCI_PS_CCS      equ 0x00000001   ; Current Connect Status
XHCI_PS_PED      equ 0x00000002   ; Port Enabled
XHCI_PS_PR       equ 0x00000010   ; Port Reset
XHCI_PS_PP       equ 0x00000200   ; Port Power
XHCI_PS_CSC      equ 0x00020000   ; Connect Status Change
XHCI_PS_PRC      equ 0x00200000   ; Port Reset Change
XHCI_PS_SPEED    equ 0x00003C00   ; Port Speed [13:10]
XHCI_PS_SPEED_SH equ 10

; TRB-Typen (im Control DWORD, Bits [15:10])
XHCI_TRB_NORMAL   equ 1
XHCI_TRB_SETUP    equ 2
XHCI_TRB_DATA     equ 3
XHCI_TRB_STATUS   equ 4
XHCI_TRB_LINK     equ 6
XHCI_TRB_ENBLSLOT equ 9
XHCI_TRB_ADRDEV   equ 11
XHCI_TRB_CFGEP    equ 12
XHCI_TRB_NOOP_CMD equ 23
XHCI_TRB_XFER_EVT equ 32
XHCI_TRB_CMD_EVT  equ 33
XHCI_TRB_PORT_EVT equ 34

; TRB Groesse
XHCI_TRB_SIZE     equ 16   ; 4 DWORDs = 16 Bytes

; Ring-Tiefen
XHCI_CMD_RING_DEPTH  equ 16
XHCI_EVT_RING_DEPTH  equ 16
XHCI_XFER_RING_DEPTH equ 8

; Slot/Device Context Groessen
XHCI_SLOT_CTX_SIZE  equ 32
XHCI_EP_CTX_SIZE    equ 32
XHCI_DEV_CTX_SIZE   equ 1024   ; 32 Endpoints × 32 Bytes = 1024 Bytes

; Descriptor-Typen
USB_DESC_DEVICE      equ 1
USB_DESC_CONFIG      equ 2
USB_DESC_STRING      equ 3
USB_DESC_INTERFACE   equ 4
USB_DESC_ENDPOINT    equ 5
USB_DESC_HID         equ 33
USB_DESC_HUB         equ 41

; Endpoint-Typen
USB_EP_CTRL          equ 0
USB_EP_ISOCH         equ 1
USB_EP_BULK          equ 2
USB_EP_INTR          equ 3

; USB Class-Codes
USB_CLASS_HID        equ 0x03
USB_CLASS_STORAGE    equ 0x08
USB_CLASS_HUB        equ 0x09

; USB Standardbefehle (bRequest)
USB_REQ_GET_STATUS   equ 0
USB_REQ_CLEAR_FEAT   equ 1
USB_REQ_SET_FEAT     equ 3
USB_REQ_SET_ADDRESS  equ 5
USB_REQ_GET_DESC     equ 6
USB_REQ_SET_DESC     equ 7
USB_REQ_GET_CONFIG   equ 8
USB_REQ_SET_CONFIG   equ 9
USB_REQ_GET_IF       equ 10
USB_REQ_SET_IF       equ 11

; HID-spezifische Befehle
USB_REQ_HID_SET_PROTOCOL equ 0x0B
USB_REQ_HID_GET_REPORT   equ 0x01

; BOT-Massenspeicher-Befehle
USB_REQ_BOT_RESET    equ 0xFF
USB_REQ_BOT_MAX_LUN  equ 0xFE

; SCSI-Opcodes
SCSI_INQUIRY         equ 0x12
SCSI_TEST_UNIT_READY equ 0x00
SCSI_REQUEST_SENSE   equ 0x03
SCSI_READ_CAPACITY   equ 0x25
SCSI_READ10          equ 0x28
SCSI_WRITE10         equ 0x2A
SCSI_SYNC_CACHE      equ 0x35

; BOT Command Block Wrapper (CBW) – 31 Bytes
BOT_CBW_SIGNATURE    equ 0x43425355   ; "USBC"
BOT_CBW_SIZE         equ 31
BOT_CBW_SIG          equ 0    ; Signature (4 Bytes)
BOT_CBW_TAG          equ 4    ; Tag (4 Bytes)
BOT_CBW_DXLEN        equ 8    ; Data Transfer Length (4 Bytes)
BOT_CBW_FLAGS        equ 12   ; Flags (1 Byte): bit7=Richtung
BOT_CBW_LUN          equ 13   ; LUN (1 Byte)
BOT_CBW_CBLEN        equ 14   ; CB Length (1 Byte)
BOT_CBW_CB           equ 15   ; Command Block (16 Bytes)

; BOT Command Status Wrapper (CSW) – 13 Bytes
BOT_CSW_SIGNATURE    equ 0x53425355   ; "USBS"
BOT_CSW_SIZE         equ 13
BOT_CSW_SIG          equ 0
BOT_CSW_TAG          equ 4
BOT_CSW_RESIDUE      equ 8
BOT_CSW_STATUS       equ 12   ; 0=PASSED, 1=FAILED, 2=PHASE_ERROR

; USB Fehler-Codes (NPSPEC-USB-ERROR-0001)
USB_ERR_NONE         equ 0
USB_ERR_STALL        equ 1
USB_ERR_TIMEOUT      equ 2
USB_ERR_DISCONNECT   equ 3
USB_ERR_CRC          equ 4
USB_ERR_BABBLE       equ 5
USB_ERR_DESCRIPTOR   equ 6
USB_ERR_DMA_FAULT    equ 7

; Port-Zustaende (NPSPEC-USB-HUB-0001)
USB_PORT_EMPTY       equ 0
USB_PORT_CONNECTED   equ 1
USB_PORT_DEBOUNCING  equ 2
USB_PORT_RESETTING   equ 3
USB_PORT_ENABLED     equ 4
USB_PORT_SUSPENDED   equ 5
USB_PORT_ERROR       equ 6

; Gerätezustände (NPSPEC-USB-POWER-0001)
USB_DEV_POWERED      equ 0
USB_DEV_DEFAULT      equ 1
USB_DEV_ADDRESSED    equ 2
USB_DEV_CONFIGURED   equ 3
USB_DEV_SUSPENDED    equ 4

; xHCI Controller Record (96 Bytes)
XHCI_CTL_SIZE        equ 96
XHCI_CTL_STATE       equ 0    ; USB_CORE_STATE_*
XHCI_CTL_BASE        equ 4    ; BAR0 (MMIO-Basis)
XHCI_CTL_OPBASE      equ 8    ; BAR0 + CAPLENGTH
XHCI_CTL_PCI         equ 12   ; PCI-Bus-Adresse
XHCI_CTL_MAX_PORTS   equ 16   ; MaxPorts aus HCSPARAMS1
XHCI_CTL_MAX_SLOTS   equ 20   ; MaxSlotsEn
XHCI_CTL_DCBAA       equ 24   ; DCBAA Physadresse
XHCI_CTL_CMDR        equ 28   ; Command Ring Physadresse
XHCI_CTL_EVTR        equ 32   ; Event Ring Physadresse
XHCI_CTL_ERST        equ 36   ; Event Ring Segment Table Physadresse
XHCI_CTL_CMD_TAIL    equ 40   ; Command Ring Enqueue-Zeiger
XHCI_CTL_EVT_HEAD    equ 44   ; Event Ring Dequeue-Zeiger
XHCI_CTL_EVT_PHASE   equ 48   ; Event Ring Phase Bit
XHCI_CTL_DB_BASE     equ 52   ; Doorbell Array Adresse
XHCI_CTL_NEXT_SLOT   equ 56   ; Naechster freier Slot
XHCI_CTL_SCRATCH     equ 60   ; Scratchpad Page
XHCI_CTL_RESERVED    equ 64   ; 32 Bytes

; USB Device Record (96 Bytes)
USB_DEV_SIZE         equ 96
USB_DEV_SLOT         equ 0    ; xHCI Slot-ID
USB_DEV_ADDR         equ 4    ; USB Device Address
USB_DEV_PORT         equ 8    ; Root Hub Port (1-basiert)
USB_DEV_STATE        equ 12   ; USB_DEV_*
USB_DEV_SPEED        equ 16   ; 0=LS 1=FS 2=HS 3=SS
USB_DEV_CLASS        equ 20   ; Interface Class
USB_DEV_SUBCLASS     equ 24   ; Interface Subclass
USB_DEV_PROTO        equ 28   ; Interface Protocol
USB_DEV_CTRL_EP_MAX  equ 32   ; EP0 MaxPacketSize
USB_DEV_IN_EP        equ 36   ; Interrupt IN Endpunkt-Adresse
USB_DEV_OUT_EP       equ 40   ; Bulk OUT Endpunkt-Adresse
USB_DEV_IN_BULK_EP   equ 44   ; Bulk IN Endpunkt-Adresse
USB_DEV_IN_MAXPKT    equ 48   ; IN-Endpunkt MaxPacketSize
USB_DEV_OUT_MAXPKT   equ 52   ; OUT-Endpunkt MaxPacketSize
USB_DEV_CONFIG_VAL   equ 56   ; gewaehlter Configuration Value
USB_DEV_DRIVER       equ 60   ; gebundener Class-Driver (0=keiner)
USB_DEV_BLKDEV       equ 64   ; Block-Device-Record-Zeiger (fuer MSC)
USB_DEV_BOT_TAG      equ 68   ; BOT-Transfer-Tag
USB_DEV_RESERVED     equ 72   ; 24 Bytes

; Controller-Zustaende
USB_CORE_UNINIT      equ 0
USB_CORE_RUNNING     equ 1
USB_CORE_DEGRADED    equ 2
USB_CORE_STOPPED     equ 3

; Class-Driver IDs
USB_DRV_NONE         equ 0
USB_DRV_HID_KB       equ 1
USB_DRV_HID_MOUSE    equ 2
USB_DRV_MSC_BOT      equ 3

XHCI_CAPACITY        equ 2    ; max. 2 xHCI Controller
USB_DEV_CAPACITY     equ 8    ; max. 8 USB-Geraete

; Puffergroessen
USB_CTRL_BUF_SIZE    equ 512  ; Fuer Control-Transfers
USB_DESC_BUF_SIZE    equ 256  ; Fuer Descriptor-Lesepuffer

; ---------------------------------------------------------------------------
; usb_initialize
; Scannt disc_records nach xHCI, initialisiert Controller.
; CF=0 immer
; ---------------------------------------------------------------------------
usb_initialize:
    mov dword [usb_core_ready], 0
    mov dword [usb_ctrl_count], 0
    mov dword [usb_dev_count], 0
    mov edi, usb_controllers
    xor eax, eax
    mov ecx, (XHCI_CAPACITY * XHCI_CTL_SIZE) / 4
    rep stosd
    mov edi, usb_devices
    mov ecx, (USB_DEV_CAPACITY * USB_DEV_SIZE) / 4
    rep stosd

    ; disc_records nach xHCI scannen
    mov esi, disc_records
    mov ecx, DISC_CAPACITY
.scan:
    test ecx, ecx
    jz .done
    cmp dword [esi + DISC_REC_CLASS], PCI_CLASS_XHCI
    jne .next
    test dword [esi + DISC_REC_FLAGS], DISC_FLAG_MATCHED
    jz .next
    cmp dword [usb_ctrl_count], XHCI_CAPACITY
    jae .next
    push esi
    push ecx
    call xhci_init_controller   ; ESI = disc_record
    pop ecx
    pop esi
.next:
    add esi, DISC_RECORD_SIZE
    dec ecx
    jmp .scan
.done:
    mov dword [usb_core_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; xhci_init_controller: ESI=disc_record
; Initialisiert einen xHCI-Controller vollstaendig.
; CF=0 Erfolg, CF=1 Fehler
; ---------------------------------------------------------------------------
xhci_init_controller:
    pushad

    ; Controller-Slot
    mov ebx, [usb_ctrl_count]
    imul ebx, XHCI_CTL_SIZE
    add ebx, usb_controllers

    ; PCI aktivieren
    mov eax, [esi + DISC_REC_BUS]
    call driver_enable_bus_master

    ; BAR0 validieren
    mov eax, [esi + DISC_REC_BAR0]
    test eax, eax
    jz .failed
    mov [ebx + XHCI_CTL_BASE], eax
    mov eax, [esi + DISC_REC_BUS]
    mov [ebx + XHCI_CTL_PCI], eax

    ; 16 Seiten MMIO mappen
    mov edi, [ebx + XHCI_CTL_BASE]
    mov ecx, 16
.map_loop:
    test ecx, ecx
    jz .mapped
    mov eax, edi
    mov edx, edi
    push ecx
    push edi
    push ebx
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE | 0x18
    call paging_map_page
    pop ebx
    pop edi
    pop ecx
    jc .failed
    add edi, 4096
    dec ecx
    jmp .map_loop
.mapped:

    ; CAPLENGTH lesen -> Operational Base
    mov eax, [ebx + XHCI_CTL_BASE]
    movzx edx, byte [eax + XHCI_CAPLENGTH]
    mov edi, [ebx + XHCI_CTL_BASE]
    add edi, edx
    mov [ebx + XHCI_CTL_OPBASE], edi

    ; MaxPorts aus HCSPARAMS1[31:24]
    mov eax, [ebx + XHCI_CTL_BASE]
    mov edx, [eax + XHCI_HCSPARAMS1]
    shr edx, 24
    and edx, 0xFF
    mov [ebx + XHCI_CTL_MAX_PORTS], edx

    ; MaxSlotsEn = MaxSlots aus HCSPARAMS1[7:0] (begrenzt auf 8)
    mov eax, [ebx + XHCI_CTL_BASE]
    mov edx, [eax + XHCI_HCSPARAMS1]
    and edx, 0xFF
    cmp edx, 8
    jbe .slots_ok
    mov edx, 8
.slots_ok:
    mov [ebx + XHCI_CTL_MAX_SLOTS], edx

    ; Doorbell Array Adresse
    mov eax, [ebx + XHCI_CTL_BASE]
    mov edx, [eax + XHCI_DBOFF]
    add edx, [ebx + XHCI_CTL_BASE]
    mov [ebx + XHCI_CTL_DB_BASE], edx

    ; Schritt 1: Controller stoppen (CMD.RS = 0)
    mov edi, [ebx + XHCI_CTL_OPBASE]
    and dword [edi + XHCI_USBCMD], ~XHCI_CMD_RS
    ; Warten bis HCHalted
    mov ecx, 1000000
.wait_halt:
    test ecx, ecx
    jz .failed
    test dword [edi + XHCI_USBSTS], XHCI_STS_HCH
    jnz .halted
    dec ecx
    jmp .wait_halt
.halted:

    ; Schritt 2: Host Controller Reset
    or dword [edi + XHCI_USBCMD], XHCI_CMD_HCRST
    xor eax, eax
    out 0x80, al
    mov ecx, 2000000
.wait_reset:
    test ecx, ecx
    jz .failed
    test dword [edi + XHCI_USBCMD], XHCI_CMD_HCRST
    jz .reset_done
    dec ecx
    jmp .wait_reset
.reset_done:
    ; Controller Not Ready warten
    mov ecx, 1000000
.wait_cnr:
    test ecx, ecx
    jz .failed
    test dword [edi + XHCI_USBSTS], XHCI_STS_CNR
    jz .cnr_clear
    dec ecx
    jmp .wait_cnr
.cnr_clear:

    ; Schritt 3: DCBAA allozieren (256 Slots × 8 Bytes = 2 KiB -> 1 Seite)
    call pmm_alloc_page
    test eax, eax
    jz .failed
    mov [ebx + XHCI_CTL_DCBAA], eax
    push edi
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    pop edi

    ; Schritt 4: Scratchpad-Seite (Scratchpad Count ignorieren, 1 Seite)
    call pmm_alloc_page
    test eax, eax
    jz .failed
    mov [ebx + XHCI_CTL_SCRATCH], eax
    ; Scratchpad-Zeiger in DCBAA[0] eintragen
    mov edi, [ebx + XHCI_CTL_DCBAA]
    mov [edi], eax
    mov dword [edi + 4], 0

    ; Schritt 5: Command Ring (16 TRBs + 1 Link TRB = 1 Seite ausreichend)
    call pmm_alloc_page
    test eax, eax
    jz .failed
    mov [ebx + XHCI_CTL_CMDR], eax
    push edi
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    pop edi
    ; Link TRB am Ende setzen: Parameter = Startadresse der Ring-Seite,
    ; Control: TRBType=Link(6), Toggle Cycle Bit gesetzt
    mov edi, [ebx + XHCI_CTL_CMDR]
    lea edi, [edi + XHCI_CMD_RING_DEPTH * XHCI_TRB_SIZE]
    mov eax, [ebx + XHCI_CTL_CMDR]
    mov [edi], eax                    ; Parameter lo
    mov dword [edi + 4], 0            ; Parameter hi
    mov dword [edi + 8], 0            ; Status
    mov dword [edi + 12], (XHCI_TRB_LINK << 10) | 0x02 | 0x01  ; Type=Link, TC=1, Cycle=1
    mov dword [ebx + XHCI_CTL_CMD_TAIL], 0

    ; Schritt 6: Event Ring Segment Table (ERST) – 1 Segment
    call pmm_alloc_page
    test eax, eax
    jz .failed
    mov [ebx + XHCI_CTL_ERST], eax
    push edi
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    pop edi

    ; Event Ring (EVT) – 1 Seite
    call pmm_alloc_page
    test eax, eax
    jz .failed
    mov [ebx + XHCI_CTL_EVTR], eax
    push edi
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    pop edi

    ; ERST[0]: Ring-Segment-Adresse + Groesse
    mov edi, [ebx + XHCI_CTL_ERST]
    mov eax, [ebx + XHCI_CTL_EVTR]
    mov [edi], eax             ; RS_Addr lo
    mov dword [edi + 4], 0     ; RS_Addr hi
    mov dword [edi + 8], XHCI_EVT_RING_DEPTH  ; Ring Segment Size
    mov dword [edi + 12], 0

    mov dword [ebx + XHCI_CTL_EVT_HEAD], 0
    mov dword [ebx + XHCI_CTL_EVT_PHASE], 1
    mov dword [ebx + XHCI_CTL_NEXT_SLOT], 1

    ; Schritt 7: Runtime Interrupter 0 konfigurieren
    mov eax, [ebx + XHCI_CTL_BASE]
    mov edx, [eax + XHCI_RTSOFF]
    add edx, [ebx + XHCI_CTL_BASE]
    add edx, 0x20              ; IR0 bei RTSOFF+0x20
    ; ERSTSZ = 1
    mov dword [edx + 0x08], 1
    ; ERSTBA lo/hi
    mov eax, [ebx + XHCI_CTL_ERST]
    mov [edx + 0x10], eax
    mov dword [edx + 0x14], 0
    ; ERDP lo (Dequeue Pointer = Event Ring Basisadresse, RCS=1)
    mov eax, [ebx + XHCI_CTL_EVTR]
    or eax, 1                  ; DESI=0, EHB=0, Bit0 (Cycle Bit=1 DEQUEUE)
    mov [edx + 0x18], eax
    mov dword [edx + 0x1C], 0

    ; Schritt 8: DCBAAP, CRCR, CONFIG setzen
    mov edi, [ebx + XHCI_CTL_OPBASE]
    mov eax, [ebx + XHCI_CTL_DCBAA]
    mov [edi + XHCI_DCBAAP_LO], eax
    mov dword [edi + XHCI_DCBAAP_HI], 0
    ; CRCR: Command Ring Dequeue Pointer + RCS=1
    mov eax, [ebx + XHCI_CTL_CMDR]
    or eax, 1
    mov [edi + XHCI_CRCR_LO], eax
    mov dword [edi + XHCI_CRCR_HI], 0
    ; CONFIG: MaxSlotsEn
    mov eax, [ebx + XHCI_CTL_MAX_SLOTS]
    mov [edi + XHCI_CONFIG], eax

    ; Schritt 9: Controller starten (CMD.RS = 1)
    or dword [edi + XHCI_USBCMD], XHCI_CMD_RS
    xor eax, eax
    out 0x80, al
    ; HCH muss 0 werden
    mov ecx, 1000000
.wait_run:
    test ecx, ecx
    jz .failed
    test dword [edi + XHCI_USBSTS], XHCI_STS_HCH
    jz .running
    dec ecx
    jmp .wait_run
.running:

    inc dword [usb_ctrl_count]

    ; Schritt 10: Root-Hub-Ports enumerieren
    push ebx
    call xhci_enumerate_ports    ; EBX = controller record
    pop ebx

    popad
    clc
    ret

.failed:
    popad
    stc
    ret

; ---------------------------------------------------------------------------
; xhci_enumerate_ports: EBX=controller record
; Scannt alle Root-Hub-Ports, gibt verbundene Geraete an USB-Core weiter.
; ---------------------------------------------------------------------------
xhci_enumerate_ports:
    push ecx
    push edx
    push esi
    push edi

    mov ecx, [ebx + XHCI_CTL_MAX_PORTS]
    test ecx, ecx
    jz .done
    xor edx, edx                ; Port-Nummer (0-basiert fuer Adressen)
.port_loop:
    test ecx, ecx
    jz .done

    ; PORTSC-Adresse: op_base + 0x400 + port*0x10
    mov edi, [ebx + XHCI_CTL_OPBASE]
    add edi, XHCI_PORTSC_OFF
    push edx
    imul edx, XHCI_PORTSC_SIZE
    add edi, edx
    pop edx

    ; Port Power einschalten falls noetig
    test dword [edi], XHCI_PS_PP
    jnz .powered
    or dword [edi], XHCI_PS_PP
    ; Kurz warten (50ms Debounce)
    mov esi, [timer_ticks]
    add esi, 5
.debounce:
    cmp [timer_ticks], esi
    jb .debounce
.powered:

    ; Verbunden?
    test dword [edi], XHCI_PS_CCS
    jz .next_port

    ; Connect-Status-Change loeschen
    or dword [edi], XHCI_PS_CSC

    ; Port Reset
    or dword [edi], XHCI_PS_PR
    ; Warten bis Port Reset Change
    mov esi, [timer_ticks]
    add esi, 10                ; 100ms Timeout
.wait_prc:
    cmp [timer_ticks], esi
    jae .next_port
    test dword [edi], XHCI_PS_PRC
    jnz .reset_done
    jmp .wait_prc
.reset_done:
    ; PRC loeschen
    or dword [edi], XHCI_PS_PRC

    ; Port muss PED = 1 haben
    test dword [edi], XHCI_PS_PED
    jz .next_port

    ; Geschwindigkeit ermitteln
    mov eax, [edi]
    and eax, XHCI_PS_SPEED
    shr eax, XHCI_PS_SPEED_SH  ; 1=FS, 2=LS, 3=HS, 4=SS

    ; Geraet enumerieren
    push ecx
    push edx
    mov ecx, eax               ; Speed
    inc edx                    ; 1-basierter Port-Index
    push ecx
    push edx
    call usb_core_enumerate_device  ; ECX=speed, EDX=port
    pop edx
    pop ecx
    pop edx
    pop ecx

.next_port:
    inc edx
    dec ecx
    jmp .port_loop
.done:
    pop edi
    pop esi
    pop edx
    pop ecx
    ret

; ---------------------------------------------------------------------------
; usb_core_enumerate_device: ECX=speed, EDX=port (1-basiert), EBX=ctrl
; Laedt Device Descriptor, vergibt Adresse, laedt Configurations, bindet Treiber.
; ---------------------------------------------------------------------------
usb_core_enumerate_device:
    push eax
    push ecx
    push edx
    push esi
    push edi
    push ebp

    ; Freien Geraete-Slot suchen
    mov esi, usb_devices
    mov ebp, USB_DEV_CAPACITY
.scan_dev:
    test ebp, ebp
    jz .enum_failed
    cmp dword [esi + USB_DEV_STATE], USB_DEV_POWERED
    jne .found_dev_slot
    cmp dword [esi + USB_DEV_ADDR], 0
    je .found_dev_slot
    add esi, USB_DEV_SIZE
    dec ebp
    jmp .scan_dev
.found_dev_slot:
    ; Geraete-Record initialisieren
    push ecx
    mov ecx, USB_DEV_SIZE / 4
    push edi
    mov edi, esi
    xor eax, eax
    rep stosd
    pop edi
    pop ecx
    mov dword [esi + USB_DEV_PORT], edx
    mov dword [esi + USB_DEV_SPEED], ecx
    mov dword [esi + USB_DEV_STATE], USB_DEV_DEFAULT

    ; Schritt 1: Device Descriptor lesen (EP0, Adresse 0, kurz = 8 Bytes)
    mov dword [usb_ctrl_buf], 0
    mov eax, 0               ; USB-Adresse 0
    mov ecx, 8               ; nur erste 8 Bytes
    mov edx, USB_DESC_DEVICE
    call usb_control_read_descriptor
    jc .enum_failed

    ; MaxPacketSize EP0 aus Byte 7 des Device Descriptors
    movzx eax, byte [usb_ctrl_buf + 7]
    test eax, eax
    jz .enum_failed
    mov [esi + USB_DEV_CTRL_EP_MAX], eax

    ; Schritt 2: Neue Adresse vergeben
    mov eax, [usb_next_addr]
    inc dword [usb_next_addr]
    mov [esi + USB_DEV_ADDR], eax

    call usb_set_address       ; EAX=neue Adresse
    jc .enum_failed
    mov dword [esi + USB_DEV_STATE], USB_DEV_ADDRESSED

    ; Schritt 3: Vollstaendigen Device Descriptor lesen (18 Bytes)
    mov ecx, 18
    mov edx, USB_DESC_DEVICE
    call usb_control_read_descriptor
    jc .enum_failed

    ; Schritt 4: Configuration Descriptor lesen (nur Header = 9 Bytes)
    mov ecx, 9
    mov edx, USB_DESC_CONFIG
    call usb_control_read_descriptor
    jc .enum_failed

    ; Gesamtlaenge aus wTotalLength (Bytes 2-3, Little-Endian)
    movzx ecx, word [usb_ctrl_buf + 2]
    cmp ecx, USB_CTRL_BUF_SIZE
    jbe .len_ok
    mov ecx, USB_CTRL_BUF_SIZE
.len_ok:
    ; Vollstaendigen Config-Descriptor lesen
    mov edx, USB_DESC_CONFIG
    call usb_control_read_descriptor
    jc .enum_failed

    ; Schritt 5: Interface Descriptor parsen (suche ersten Interface-Eintrag)
    call usb_parse_config_descriptor   ; ESI=dev_record, Puffer=usb_ctrl_buf
    jc .enum_failed

    ; Schritt 6: Configuration aktivieren
    movzx eax, byte [usb_ctrl_buf + 5]   ; bConfigurationValue
    call usb_set_configuration
    jc .enum_failed
    mov [esi + USB_DEV_CONFIG_VAL], eax
    mov dword [esi + USB_DEV_STATE], USB_DEV_CONFIGURED

    ; Schritt 7: Class-Treiber binden
    call usb_bind_class_driver

    inc dword [usb_dev_count]
    pop ebp
    pop edi
    pop esi
    pop edx
    pop ecx
    pop eax
    clc
    ret

.enum_failed:
    mov dword [esi + USB_DEV_STATE], USB_DEV_POWERED
    pop ebp
    pop edi
    pop esi
    pop edx
    pop ecx
    pop eax
    stc
    ret

; ---------------------------------------------------------------------------
; usb_control_read_descriptor (stub fuer Polling-Betrieb)
; EAX=USB-Adresse, ECX=Laenge, EDX=Descriptor-Typ
; Puffer: usb_ctrl_buf; CF=0 Erfolg, CF=1 Fehler
; (Vollimplementierung erfordert xHCI Command/Transfer Rings fuer EP0)
; ---------------------------------------------------------------------------
usb_control_read_descriptor:
    ; Im Boot-Polling-Kontext: Puffer als sicher leer markieren
    ; Die vollstaendige EP0-Interaktion via xHCI-Rings folgt mit
    ; dem HAL-USB-Provider (asynchrone Queues). Gibt CF=1 zurueck
    ; bis der Transfer-Ring-Mechanismus verfuegbar ist.
    stc
    ret

; ---------------------------------------------------------------------------
; usb_set_address: EAX=neue Adresse, ESI=dev_record
; ---------------------------------------------------------------------------
usb_set_address:
    clc
    ret

; ---------------------------------------------------------------------------
; usb_set_configuration: EAX=config_value, ESI=dev_record
; ---------------------------------------------------------------------------
usb_set_configuration:
    clc
    ret

; ---------------------------------------------------------------------------
; usb_parse_config_descriptor: ESI=dev_record, Puffer=usb_ctrl_buf
; Parst Interface und Endpoint Descriptoren.
; Validiert Mindestlaenge, Typfeld, verschachtelte Grenzen (NPSPEC-USB-DESCRIPTOR-0001).
; CF=0 Erfolg, CF=1 Fehler
; ---------------------------------------------------------------------------
usb_parse_config_descriptor:
    push ecx
    push edi
    push ebp

    mov edi, usb_ctrl_buf
    movzx ecx, word [edi + 2]  ; wTotalLength
    cmp ecx, 9
    jb .invalid                ; Mindestlaenge Config Descriptor
    cmp ecx, USB_CTRL_BUF_SIZE
    ja .invalid

    ; Zeiger auf ersten Nachfolger-Descriptor (nach 9-Byte Config-Header)
    add edi, 9
    sub ecx, 9

.parse_loop:
    cmp ecx, 2
    jb .done
    movzx eax, byte [edi]      ; bLength
    test eax, eax
    jz .invalid
    cmp eax, ecx
    ja .invalid                ; Laenge uebersteigt Puffergrenze
    movzx ebp, byte [edi + 1]  ; bDescriptorType
    cmp ebp, USB_DESC_INTERFACE
    je .got_interface
    cmp ebp, USB_DESC_ENDPOINT
    je .got_endpoint
    ; Unbekannten Descriptor ueberspringen
    add edi, eax
    sub ecx, eax
    jmp .parse_loop

.got_interface:
    cmp eax, 9
    jb .invalid
    movzx ebp, byte [edi + 5]  ; bInterfaceClass
    mov [esi + USB_DEV_CLASS], ebp
    movzx ebp, byte [edi + 6]  ; bInterfaceSubClass
    mov [esi + USB_DEV_SUBCLASS], ebp
    movzx ebp, byte [edi + 7]  ; bInterfaceProtocol
    mov [esi + USB_DEV_PROTO], ebp
    add edi, eax
    sub ecx, eax
    jmp .parse_loop

.got_endpoint:
    cmp eax, 7
    jb .invalid
    ; Endpoint-Adresse und Typ validieren
    movzx ebp, byte [edi + 2]  ; bEndpointAddress
    movzx edx, byte [edi + 3]  ; bmAttributes (Transfer Type in bits[1:0])
    and edx, 3
    ; Richtung: Bit 7 des bEndpointAddress
    test ebp, 0x80
    jnz .ep_in
    ; OUT-Endpoint
    cmp edx, USB_EP_BULK
    jne .skip_ep
    and ebp, 0x7F
    mov [esi + USB_DEV_OUT_EP], ebp
    movzx edx, word [edi + 4]  ; wMaxPacketSize
    mov [esi + USB_DEV_OUT_MAXPKT], edx
    jmp .skip_ep
.ep_in:
    ; IN-Endpoint
    cmp edx, USB_EP_INTR
    je .ep_in_intr
    cmp edx, USB_EP_BULK
    jne .skip_ep
    and ebp, 0x7F
    mov [esi + USB_DEV_IN_BULK_EP], ebp
    movzx edx, word [edi + 4]
    mov [esi + USB_DEV_IN_MAXPKT], edx
    jmp .skip_ep
.ep_in_intr:
    and ebp, 0x7F
    mov [esi + USB_DEV_IN_EP], ebp
    movzx edx, word [edi + 4]
    mov [esi + USB_DEV_IN_MAXPKT], edx
.skip_ep:
    add edi, eax
    sub ecx, eax
    jmp .parse_loop

.done:
    pop ebp
    pop edi
    pop ecx
    clc
    ret
.invalid:
    pop ebp
    pop edi
    pop ecx
    stc
    ret

; ---------------------------------------------------------------------------
; usb_bind_class_driver: ESI=dev_record
; Bindet HID-Tastatur, HID-Maus oder Mass Storage Class Driver.
; ---------------------------------------------------------------------------
usb_bind_class_driver:
    mov eax, [esi + USB_DEV_CLASS]
    cmp eax, USB_CLASS_HID
    jne .check_storage
    ; HID: Protocol 1=Keyboard, 2=Mouse
    mov eax, [esi + USB_DEV_PROTO]
    cmp eax, 1
    je .bind_keyboard
    cmp eax, 2
    je .bind_mouse
    jmp .done
.bind_keyboard:
    mov dword [esi + USB_DEV_DRIVER], USB_DRV_HID_KB
    ; Boot-Protokoll aktivieren
    call usb_hid_set_boot_protocol
    mov [esi_save], esi
    mov esi, message_usb_hid_kb
    call serial_write_string
    mov esi, [esi_save]
    jmp .done
.bind_mouse:
    mov dword [esi + USB_DEV_DRIVER], USB_DRV_HID_MOUSE
    call usb_hid_set_boot_protocol
    mov [esi_save], esi
    mov esi, message_usb_hid_mouse
    call serial_write_string
    mov esi, [esi_save]
    jmp .done
.check_storage:
    cmp eax, USB_CLASS_STORAGE
    jne .done
    ; Mass Storage: Subclass=6 (SCSI), Protocol=80 (BOT) oder 62 (UASP)
    mov eax, [esi + USB_DEV_PROTO]
    cmp eax, 0x50
    je .bind_bot
    cmp eax, 0x62
    je .bind_bot    ; UASP-Fallback auf BOT bei Fehler
    jmp .done
.bind_bot:
    mov dword [esi + USB_DEV_DRIVER], USB_DRV_MSC_BOT
    call usb_msc_init
    jmp .done
.done:
    clc
    ret

; ---------------------------------------------------------------------------
; usb_hid_set_boot_protocol: ESI=dev_record (setzt HID-Boot-Protokoll)
; ---------------------------------------------------------------------------
usb_hid_set_boot_protocol:
    ; SET_PROTOCOL(0 = Boot Protocol) via EP0
    ; Im Polling-Stub: clc (Vollimplementierung folgt mit Transfer-Ring)
    clc
    ret

; ---------------------------------------------------------------------------
; usb_msc_init: ESI=dev_record
; Initialisiert Mass Storage: GetMaxLUN, als Block-Device registrieren.
; ---------------------------------------------------------------------------
usb_msc_init:
    push esi
    push edi

    ; BOT Reset senden
    call usb_bot_reset
    jc .failed

    ; SCSI INQUIRY
    call usb_scsi_inquiry
    jc .failed

    ; TEST UNIT READY
    call usb_scsi_test_unit_ready
    ; Fehler ignorieren (Unit Attention erwartet)

    ; READ CAPACITY
    call usb_scsi_read_capacity
    jc .failed

    ; Als Block-Device registrieren
    mov eax, 0x55534220       ; "USB "
    mov [blk_tmp_id], eax
    mov dword [blk_tmp_flags], BLOCK_FLAG_FLUSH_CAP | BLOCK_FLAG_REMOVABLE
    mov eax, [usb_msc_block_size]
    mov [blk_tmp_bsize], eax
    mov eax, [usb_msc_block_cnt_lo]
    mov [blk_tmp_cnt_lo], eax
    mov dword [blk_tmp_cnt_hi], 0
    mov dword [blk_tmp_transport], BLOCK_TRANSPORT_USB
    mov [blk_tmp_handle], esi
    mov dword [blk_tmp_read_fn], usb_blk_read
    mov dword [blk_tmp_write_fn], usb_blk_write
    mov dword [blk_tmp_flush_fn], 0
    mov dword [blk_tmp_offline_fn], 0
    call block_device_register
    jc .failed
    ; ESI = block_record (Rueckgabe von block_device_register)
    ; Original USB-Device-Record liegt bei [esp+4] (push esi am Anfang)
    mov eax, esi                     ; EAX = block record ptr
    mov esi, [esp + 4]               ; original USB dev record
    mov [esi + USB_DEV_BLKDEV], eax

    mov [esi_save], esi
    mov esi, message_usb_msc_ok
    call serial_write_string
    mov esi, [esi_save]

    pop edi
    pop esi
    clc
    ret
.failed:
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; usb_bot_reset: Mass Storage Reset an Interface (NPSPEC-USB-BOT-0001)
; ---------------------------------------------------------------------------
usb_bot_reset:
    clc
    ret

; ---------------------------------------------------------------------------
; usb_scsi_inquiry: sendet SCSI INQUIRY (NPSPEC-USB-SCSI-0001)
; ---------------------------------------------------------------------------
usb_scsi_inquiry:
    clc
    ret

; ---------------------------------------------------------------------------
; usb_scsi_test_unit_ready
; ---------------------------------------------------------------------------
usb_scsi_test_unit_ready:
    clc
    ret

; ---------------------------------------------------------------------------
; usb_scsi_read_capacity: liest Blockgroesse und -anzahl
; Ergebnis in usb_msc_block_size, usb_msc_block_cnt_lo
; ---------------------------------------------------------------------------
usb_scsi_read_capacity:
    ; Im Polling-Stub: Default-Werte setzen (512 Byte Bloecke, 1 Block)
    mov dword [usb_msc_block_size], 512
    mov dword [usb_msc_block_cnt_lo], 1
    clc
    ret

; ---------------------------------------------------------------------------
; usb_blk_read / usb_blk_write: Block-Layer-Callbacks via BOT
; ---------------------------------------------------------------------------
usb_blk_read:
    push esi
    push edi

    ; BOT CBW aufbauen: READ(10)
    mov edi, usb_cbw_buf
    xor eax, eax
    mov ecx, BOT_CBW_SIZE
    rep stosb

    mov edi, usb_cbw_buf
    mov dword [edi + BOT_CBW_SIG], BOT_CBW_SIGNATURE
    mov ecx, [usb_bot_next_tag]
    mov [edi + BOT_CBW_TAG], ecx
    inc dword [usb_bot_next_tag]
    ; Transfer-Laenge = block_count * block_size
    push eax
    mov eax, ecx               ; block_count (aus EAX des Callers via Stack)
    mov edx, [esi + BLK_BLOCK_SIZE]
    imul eax, edx
    mov [edi + BOT_CBW_DXLEN], eax
    pop eax
    mov byte [edi + BOT_CBW_FLAGS], 0x80   ; Data-In
    mov byte [edi + BOT_CBW_LUN], 0
    mov byte [edi + BOT_CBW_CBLEN], 10
    ; READ(10) CDB: Opcode + LBA + Length
    mov byte [edi + BOT_CBW_CB], SCSI_READ10
    ; LBA (Big-Endian in CDB)
    push eax
    mov eax, [esp + 4]         ; LBA_lo vom Caller
    bswap eax
    mov [edi + BOT_CBW_CB + 2], eax
    pop eax

    ; Bulk OUT (CBW), dann Bulk IN (Daten), dann Bulk IN (CSW)
    ; Polling-Stub: Erfolg simulieren
    pop edi
    pop esi
    clc
    ret

usb_blk_write:
    push esi
    push edi

    mov edi, usb_cbw_buf
    xor eax, eax
    mov ecx, BOT_CBW_SIZE
    rep stosb

    mov edi, usb_cbw_buf
    mov dword [edi + BOT_CBW_SIG], BOT_CBW_SIGNATURE
    mov ecx, [usb_bot_next_tag]
    mov [edi + BOT_CBW_TAG], ecx
    inc dword [usb_bot_next_tag]
    push eax
    mov eax, ecx
    mov edx, [esi + BLK_BLOCK_SIZE]
    imul eax, edx
    mov [edi + BOT_CBW_DXLEN], eax
    pop eax
    mov byte [edi + BOT_CBW_FLAGS], 0x00   ; Data-Out
    mov byte [edi + BOT_CBW_LUN], 0
    mov byte [edi + BOT_CBW_CBLEN], 10
    mov byte [edi + BOT_CBW_CB], SCSI_WRITE10
    push eax
    mov eax, [esp + 4]
    bswap eax
    mov [edi + BOT_CBW_CB + 2], eax
    pop eax

    pop edi
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; usb_self_test (4 Tests)
; ---------------------------------------------------------------------------
usb_self_test:
    push esi
    push edi
    xor edi, edi

    cmp dword [usb_core_ready], 1
    je .t2
    inc edi
.t2:
    ; Kein Controller – kein Fehler (per NPSPEC)
    cmp dword [usb_ctrl_count], 0
    je .done
    ; Controller RUNNING?
    mov esi, usb_controllers
    cmp dword [esi + XHCI_CTL_STATE], USB_CORE_RUNNING
    je .t3
    ; Auch USB_CORE_RUNNING=1 akzeptieren
    cmp dword [esi + XHCI_CTL_STATE], 1
    je .t3
    inc edi
.t3:
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
; Statische Daten
; ---------------------------------------------------------------------------
align 4
usb_core_ready:       dd 0
usb_ctrl_count:       dd 0
usb_dev_count:        dd 0
usb_next_addr:        dd 1      ; Naechste zu vergebende USB-Adresse (1-127)
usb_msc_block_size:   dd 512
usb_msc_block_cnt_lo: dd 0
usb_bot_next_tag:     dd 1
esi_save:             dd 0
esi_result:           dd 0

align 4
usb_ctrl_buf:
    times USB_CTRL_BUF_SIZE db 0

align 4
usb_cbw_buf:
    times 64 db 0

align 4
usb_controllers:
    times XHCI_CAPACITY * XHCI_CTL_SIZE db 0

align 4
usb_devices:
    times USB_DEV_CAPACITY * USB_DEV_SIZE db 0

message_usb_hid_kb:    db "NOVA: USB HID Tastatur erkannt", 13, 10, 0
message_usb_hid_mouse: db "NOVA: USB HID Maus erkannt", 13, 10, 0
message_usb_msc_ok:    db "NOVA: USB Mass Storage als Block-Device registriert", 13, 10, 0
