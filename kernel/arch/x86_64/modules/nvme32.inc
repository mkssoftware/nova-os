; ---------------------------------------------------------------------------
; NVMe PCIe Controller Driver
; NPSPEC-NVME-ARCHITECTURE-0001 / NPSPEC-NVME-CONTROLLER-0001
; NPSPEC-NVME-QUEUE-0001 / NPSPEC-NVME-COMMAND-0001
; NPSPEC-NVME-NAMESPACE-0001 / NPSPEC-NVME-IO-0001
; NPSPEC-NVME-DMA-0001 / NPSPEC-NVME-INTERRUPT-0001
; NPSPEC-NVME-ERROR-0001 / NPSPEC-NVME-HEALTH-0001 / NPSPEC-NVME-POWER-0001
; ---------------------------------------------------------------------------

; NVMe MMIO Register-Offsets (BAR0-relativ)
NVME_CAP_LO     equ 0x00   ; Controller Capabilities (lo32)
NVME_CAP_HI     equ 0x04   ; Controller Capabilities (hi32)
NVME_VS         equ 0x08   ; Version
NVME_INTMS      equ 0x0C   ; Interrupt Mask Set
NVME_INTMC      equ 0x10   ; Interrupt Mask Clear
NVME_CC         equ 0x14   ; Controller Configuration
NVME_CSTS       equ 0x1C   ; Controller Status
NVME_NSSR       equ 0x20   ; NVM Subsystem Reset
NVME_AQA        equ 0x24   ; Admin Queue Attributes
NVME_ASQ_LO     equ 0x28   ; Admin SQ Base (lo32)
NVME_ASQ_HI     equ 0x2C   ; Admin SQ Base (hi32)
NVME_ACQ_LO     equ 0x30   ; Admin CQ Base (lo32)
NVME_ACQ_HI     equ 0x34   ; Admin CQ Base (hi32)

; CC-Felder
NVME_CC_EN      equ 0x00000001   ; Enable
NVME_CC_CSS_NVM equ 0x00000000   ; I/O Command Set = NVM
NVME_CC_MPS_4K  equ 0x00000000   ; Memory Page Size = 4 KiB (Wert 0 = 4K)
NVME_CC_AMS_RR  equ 0x00000000   ; Round Robin
NVME_CC_IOSQES  equ 0x00060000   ; SQ Entry Size = 2^6 = 64 B
NVME_CC_IOCQES  equ 0x00400000   ; CQ Entry Size = 2^4 = 16 B
NVME_CC_DEFAULT equ (NVME_CC_CSS_NVM | NVME_CC_MPS_4K | NVME_CC_AMS_RR | NVME_CC_IOSQES | NVME_CC_IOCQES)

; CSTS-Bits
NVME_CSTS_RDY   equ 0x00000001
NVME_CSTS_CFS   equ 0x00000002

; Admin Opcodes
NVME_ADM_DELETE_SQ  equ 0x00
NVME_ADM_CREATE_SQ  equ 0x01
NVME_ADM_DELETE_CQ  equ 0x04
NVME_ADM_CREATE_CQ  equ 0x05
NVME_ADM_IDENTIFY   equ 0x06
NVME_ADM_GET_FEAT   equ 0x0A

; I/O Opcodes
NVME_IO_FLUSH   equ 0x00
NVME_IO_WRITE   equ 0x01
NVME_IO_READ    equ 0x02

; Queue-Tiefen (muessen 2^n sein, max aus CAP.MQES)
NVME_ADMIN_DEPTH  equ 16   ; 16 Eintraege Adminqueue
NVME_IO_DEPTH     equ 32   ; 32 Eintraege I/O-Queue

; TRB-/Entry-Groessen
NVME_SQ_ENTRY     equ 64   ; Submission Queue Entry: 64 Bytes
NVME_CQ_ENTRY     equ 16   ; Completion Queue Entry: 16 Bytes

; SQ-Entry-Offsets (64 Bytes)
NVME_SQE_OPCODE   equ 0    ; Byte: Opcode
NVME_SQE_FLAGS    equ 1    ; Byte: Flags
NVME_SQE_CID      equ 2    ; Word: Command ID
NVME_SQE_NSID     equ 4    ; DWord: Namespace ID
NVME_SQE_RSVD     equ 8    ; 8 Bytes reserviert
NVME_SQE_MPTR     equ 16   ; 8 Bytes Metadata Ptr
NVME_SQE_PRP1_LO  equ 24   ; PRP1 lo32
NVME_SQE_PRP1_HI  equ 28   ; PRP1 hi32
NVME_SQE_PRP2_LO  equ 32   ; PRP2 lo32
NVME_SQE_PRP2_HI  equ 36   ; PRP2 hi32
NVME_SQE_CDW10    equ 40   ; Command DWORD 10
NVME_SQE_CDW11    equ 44
NVME_SQE_CDW12    equ 48
NVME_SQE_CDW13    equ 52
NVME_SQE_CDW14    equ 56
NVME_SQE_CDW15    equ 60

; CQ-Entry-Offsets (16 Bytes)
NVME_CQE_DW0      equ 0    ; Command-spezifisch
NVME_CQE_DW1      equ 4    ; reserviert
NVME_CQE_SQHD     equ 8    ; Word: SQ Head Pointer
NVME_CQE_SQID     equ 10   ; Word: SQ ID
NVME_CQE_CID      equ 12   ; Word: Command ID
NVME_CQE_STATUS   equ 14   ; Word: Status Field (Phase Tag + SC + SCT)

; Identify-Struktur-Offsets (4096 Bytes Puffer)
NVME_ID_CTL_VID   equ 0    ; Vendor ID
NVME_ID_CTL_MN    equ 24   ; Model Number (40 Bytes)
NVME_ID_CTL_NN    equ 516  ; Number of Namespaces
NVME_ID_NS_NSZE   equ 0    ; Namespace Size (Blocks, 8 Bytes)
NVME_ID_NS_LBAF0  equ 128  ; LBA Format 0 (LBADS in bits[19:16])

; Controller-Zustaende
NVME_STATE_UNKNOWN    equ 0
NVME_STATE_DISABLED   equ 1
NVME_STATE_ADMINREADY equ 2
NVME_STATE_RUNNING    equ 3
NVME_STATE_DEGRADED   equ 4
NVME_STATE_FAILED     equ 5

; Controller-Record (128 Bytes)
NVME_CTL_SIZE         equ 128
NVME_CTL_STATE        equ 0    ; NVME_STATE_*
NVME_CTL_BASE         equ 4    ; MMIO-Basisadresse
NVME_CTL_PCI          equ 8    ; PCI-Bus-Adresse
NVME_CTL_ASQ          equ 12   ; Admin SQ Basisadresse (phys)
NVME_CTL_ACQ          equ 16   ; Admin CQ Basisadresse (phys)
NVME_CTL_IOSQ         equ 20   ; I/O SQ Basisadresse (phys)
NVME_CTL_IOCQ         equ 24   ; I/O CQ Basisadresse (phys)
NVME_CTL_SQTAIL_ADM   equ 28   ; Admin SQ Tail (Software)
NVME_CTL_CQHEAD_ADM   equ 32   ; Admin CQ Head (Software)
NVME_CTL_PHASE_ADM    equ 36   ; Admin CQ Phase Bit
NVME_CTL_SQTAIL_IO    equ 40   ; I/O SQ Tail
NVME_CTL_CQHEAD_IO    equ 44   ; I/O CQ Head
NVME_CTL_PHASE_IO     equ 48   ; I/O CQ Phase Bit
NVME_CTL_NEXT_CID     equ 52   ; Naechste Command-ID
NVME_CTL_NS_COUNT     equ 56   ; Anzahl aktiver Namespaces
NVME_CTL_DSTRIDE      equ 60   ; Doorbell stride (4 << DSTRD)
NVME_CTL_IDENT_PAGE   equ 64   ; Seite fuer Identify-Puffer
NVME_CTL_NS1_SIZE_LO  equ 68   ; NS1 Groesse lo32
NVME_CTL_NS1_SIZE_HI  equ 72   ; NS1 Groesse hi32
NVME_CTL_NS1_BSIZE    equ 76   ; NS1 Block-Groesse
NVME_CTL_HEALTH       equ 80   ; BLOCK_HEALTH_*
NVME_CTL_BLKDEV       equ 84   ; Zeiger auf Block-Record
NVME_CTL_RESERVED     equ 88   ; 40 Bytes Reserviert

NVME_CAPACITY         equ 2    ; Maximal 2 NVMe-Controller

; Doorbell-Register-Offsets (relativ zu MMIO-Basis + 0x1000)
NVME_DB_BASE          equ 0x1000
; SQ n Tail Doorbell: MMIO + 0x1000 + n*2*dstride
; CQ n Head Doorbell: MMIO + 0x1000 + n*2*dstride + dstride

; ---------------------------------------------------------------------------
; nvme_initialize
; Scannt PCI-Bus via disc_records, initialisiert alle NVMe-Controller.
; CF=0 immer (fehlende NVMe kein Fehler)
; ---------------------------------------------------------------------------
nvme_initialize:
    mov dword [nvme_count], 0
    mov edi, nvme_controllers
    xor eax, eax
    mov ecx, (NVME_CAPACITY * NVME_CTL_SIZE) / 4
    rep stosd
    mov dword [nvme_mgr_ready], 1

    ; disc_records nach NVMe-Eintraegen durchsuchen
    mov esi, disc_records
    mov ecx, DISC_CAPACITY
.scan:
    test ecx, ecx
    jz .done
    cmp dword [esi + DISC_REC_CLASS], PCI_CLASS_NVME
    jne .next
    test dword [esi + DISC_REC_FLAGS], DISC_FLAG_MATCHED
    jz .next
    cmp dword [nvme_count], NVME_CAPACITY
    jae .next
    push esi
    push ecx
    call nvme_probe_controller    ; ESI = disc_record
    pop ecx
    pop esi
.next:
    add esi, DISC_RECORD_SIZE
    dec ecx
    jmp .scan
.done:
    clc
    ret

; ---------------------------------------------------------------------------
; nvme_probe_controller: ESI=disc_record
; Initialisiert einen einzelnen NVMe-Controller vollstaendig.
; CF=0 Erfolg, CF=1 Fehler
; ---------------------------------------------------------------------------
nvme_probe_controller:
    pushad

    ; Controller-Slot bestimmen
    mov ebx, [nvme_count]
    imul ebx, NVME_CTL_SIZE
    add ebx, nvme_controllers   ; EBX = Controller-Record-Zeiger

    ; PCI-Enable: Bus Master, Memory Space
    mov eax, [esi + DISC_REC_BUS]
    call driver_enable_bus_master

    ; BAR0 (MMIO-Basis) aus disc_record holen
    mov eax, [esi + DISC_REC_BAR0]
    test eax, eax
    jz .failed
    mov [ebx + NVME_CTL_BASE], eax
    mov eax, [esi + DISC_REC_BUS]
    mov [ebx + NVME_CTL_PCI], eax
    mov dword [ebx + NVME_CTL_STATE], NVME_STATE_DISABLED

    ; 8 Seiten MMIO mappen (32 KiB reicht fuer NVMe-Register + Doorbells)
    mov edi, [ebx + NVME_CTL_BASE]
    mov ecx, 8
.map_bar:
    test ecx, ecx
    jz .bar_mapped
    mov eax, edi
    mov edx, edi
    mov ebp_save, ebx
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE | 0x18
    push ecx
    push edi
    push ebp_save
    call paging_map_page
    pop ebp_save
    pop edi
    pop ecx
    jc .failed_unmap
    add edi, 4096
    dec ecx
    jmp .map_bar
.bar_mapped:
    mov ebx, ebp_save

    ; Schritt 1: Controller deaktivieren (CC.EN = 0)
    mov eax, [ebx + NVME_CTL_BASE]
    mov edx, [eax + NVME_CC]
    and edx, ~NVME_CC_EN
    mov [eax + NVME_CC], edx
    ; Warten bis CSTS.RDY = 0
    mov ecx, 1000000
.wait_dis:
    test ecx, ecx
    jz .failed
    mov edx, [eax + NVME_CSTS]
    test edx, NVME_CSTS_RDY
    jz .disabled
    dec ecx
    jmp .wait_dis
.disabled:

    ; Schritt 2: Admin-Queue-Seiten allozieren (SQ + CQ)
    ; Admin SQ: NVME_ADMIN_DEPTH * 64 Bytes = 1024 Bytes = 1 Seite
    call pmm_alloc_page
    test eax, eax
    jz .failed
    mov [ebx + NVME_CTL_ASQ], eax
    ; Admin SQ Seite nullen
    push edi
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    pop edi
    ; Admin CQ: NVME_ADMIN_DEPTH * 16 Bytes = 256 Bytes = 1 Seite
    call pmm_alloc_page
    test eax, eax
    jz .failed
    mov [ebx + NVME_CTL_ACQ], eax
    push edi
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    pop edi

    ; Schritt 3: AQA, ASQ, ACQ programmieren
    mov eax, [ebx + NVME_CTL_BASE]
    ; AQA: ACQS[27:16] = ADMIN_DEPTH-1, ASQS[11:0] = ADMIN_DEPTH-1
    mov edx, ((NVME_ADMIN_DEPTH-1) << 16) | (NVME_ADMIN_DEPTH-1)
    mov [eax + NVME_AQA], edx
    ; ASQ (physische Adresse der Admin SQ)
    mov edx, [ebx + NVME_CTL_ASQ]
    mov [eax + NVME_ASQ_LO], edx
    mov dword [eax + NVME_ASQ_HI], 0
    ; ACQ
    mov edx, [ebx + NVME_CTL_ACQ]
    mov [eax + NVME_ACQ_LO], edx
    mov dword [eax + NVME_ACQ_HI], 0

    ; Schritt 4: Controller-Konfiguration setzen und aktivieren
    mov edx, NVME_CC_DEFAULT | NVME_CC_EN
    mov [eax + NVME_CC], edx
    ; Memory-Barrier (OUT 0x80 als I/O-Zaun)
    xor eax, eax
    out 0x80, al

    ; Schritt 5: CSTS.RDY abwarten (Timeout ~500ms)
    mov eax, [ebx + NVME_CTL_BASE]
    mov ecx, 5000000
.wait_rdy:
    test ecx, ecx
    jz .failed
    mov edx, [eax + NVME_CSTS]
    test edx, NVME_CSTS_CFS
    jnz .failed
    test edx, NVME_CSTS_RDY
    jnz .admin_ready
    dec ecx
    jmp .wait_rdy
.admin_ready:
    mov dword [ebx + NVME_CTL_STATE], NVME_STATE_ADMINREADY

    ; Schritt 6: Doorbell Stride aus CAP ermitteln
    mov eax, [ebx + NVME_CTL_BASE]
    mov edx, [eax + NVME_CAP_LO]
    ; DSTRD = bits [55:52] des 64-Bit-CAP -> bits [23:20] von CAP_HI
    mov eax, [ebx + NVME_CTL_BASE]
    mov edi, [eax + NVME_CAP_HI]
    mov eax, edi
    shr eax, 20
    and eax, 0xF              ; DSTRD
    mov ecx, 4
    shl ecx, eax              ; dstride = 4 << DSTRD
    mov [ebx + NVME_CTL_DSTRIDE], ecx

    ; Schritt 7: Queue-Indizes initialisieren
    mov dword [ebx + NVME_CTL_SQTAIL_ADM], 0
    mov dword [ebx + NVME_CTL_CQHEAD_ADM], 0
    mov dword [ebx + NVME_CTL_PHASE_ADM], 1
    mov dword [ebx + NVME_CTL_NEXT_CID], 1

    ; Schritt 8: Identify Controller
    push ebx
    call nvme_identify_controller
    pop ebx
    jc .failed

    mov dword [ebx + NVME_CTL_STATE], NVME_STATE_ADMINREADY

    ; Schritt 9: I/O-Queues anlegen
    push ebx
    call nvme_create_io_queues
    pop ebx
    jc .failed

    ; Schritt 10: Namespace 1 enumerieren und als Block-Device registrieren
    push ebx
    call nvme_namespace_discover
    pop ebx
    ; NS-Fehler sind kein fataler Fehler

    mov dword [ebx + NVME_CTL_STATE], NVME_STATE_RUNNING
    inc dword [nvme_count]

    popad
    clc
    ret

.failed_unmap:
.failed:
    mov dword [ebx + NVME_CTL_STATE], NVME_STATE_FAILED
    popad
    stc
    ret

; ---------------------------------------------------------------------------
; nvme_submit_admin_cmd: EBX=ctrl, ESI=SQ-Entry-Zeiger (64B)
; Schreibt SQ-Eintrag, klingelt Doorbell, wartet auf CQ-Completion.
; CF=0 Erfolg, CF=1 Fehler; EAX=CQE.DW0 bei Erfolg
; ---------------------------------------------------------------------------
nvme_submit_admin_cmd:
    push ecx
    push edx
    push edi

    ; SQ Tail berechnen: ASQ + tail * 64
    mov edi, [ebx + NVME_CTL_ASQ]
    mov ecx, [ebx + NVME_CTL_SQTAIL_ADM]
    imul ecx, NVME_SQ_ENTRY
    add edi, ecx

    ; SQ-Eintrag kopieren (64 Bytes = 16 DWORDs)
    push esi
    mov ecx, NVME_SQ_ENTRY / 4
    rep movsd
    pop esi

    ; Command-ID setzen
    mov ecx, [ebx + NVME_CTL_NEXT_CID]
    mov [edi - NVME_SQ_ENTRY + NVME_SQE_CID], cx
    inc dword [ebx + NVME_CTL_NEXT_CID]
    mov [nvme_pending_cid], ecx

    ; Memory-Barrier
    xor eax, eax
    out 0x80, al

    ; Doorbell klingeln: Admin SQ Tail
    mov edi, [ebx + NVME_CTL_SQTAIL_ADM]
    inc edi
    cmp edi, NVME_ADMIN_DEPTH
    jb .no_wrap_sq
    xor edi, edi
.no_wrap_sq:
    mov [ebx + NVME_CTL_SQTAIL_ADM], edi

    mov eax, [ebx + NVME_CTL_BASE]
    add eax, NVME_DB_BASE      ; Doorbell-Basisadresse
    mov edx, [ebx + NVME_CTL_DSTRIDE]
    ; Admin SQ Tail Doorbell: Offset = 0
    mov [eax], edi

    ; Auf CQ-Completion warten (Phase Tag)
    mov eax, [ebx + NVME_CTL_ACQ]
    mov ecx, [ebx + NVME_CTL_CQHEAD_ADM]
    imul ecx, NVME_CQ_ENTRY
    add eax, ecx              ; EAX = Zeiger auf aktuellen CQ-Eintrag

    mov edi, 2000000
.wait_cq:
    test edi, edi
    jz .timeout
    movzx ecx, word [eax + NVME_CQE_STATUS]
    and ecx, 1                ; Phase Tag
    cmp ecx, [ebx + NVME_CTL_PHASE_ADM]
    je .cq_ready
    dec edi
    jmp .wait_cq
.timeout:
    pop edi
    pop edx
    pop ecx
    stc
    ret
.cq_ready:
    ; Status pruefen (SC in bits[14:9], SCT in bits[11:9] -> Feld[14:9])
    movzx ecx, word [eax + NVME_CQE_STATUS]
    shr ecx, 1
    and ecx, 0x7FF            ; SC+SCT
    push ecx

    ; CQ Head weiterrücken
    mov edi, [ebx + NVME_CTL_CQHEAD_ADM]
    inc edi
    cmp edi, NVME_ADMIN_DEPTH
    jb .no_wrap_cq
    xor edi, edi
    xor dword [ebx + NVME_CTL_PHASE_ADM], 1  ; Phase-Bit toggeln
.no_wrap_cq:
    mov [ebx + NVME_CTL_CQHEAD_ADM], edi

    ; CQ Head Doorbell: offset = dstride
    mov eax, [ebx + NVME_CTL_BASE]
    add eax, NVME_DB_BASE
    mov edx, [ebx + NVME_CTL_DSTRIDE]
    add eax, edx
    mov [eax], edi

    pop ecx
    test ecx, ecx
    jnz .cmd_error

    mov eax, [nvme_cqe_dw0_tmp]   ; Rueckgabewert
    pop edi
    pop edx
    pop ecx
    clc
    ret
.cmd_error:
    pop edi
    pop edx
    pop ecx
    stc
    ret

; ---------------------------------------------------------------------------
; nvme_identify_controller: EBX=ctrl
; Fuehrt Identify(Controller) durch, liest Vendor/Model.
; CF=0 Erfolg, CF=1 Fehler
; ---------------------------------------------------------------------------
nvme_identify_controller:
    push esi
    push edi

    ; Pufferseite allozieren
    call pmm_alloc_page
    test eax, eax
    jz .failed
    mov [ebx + NVME_CTL_IDENT_PAGE], eax

    ; SQ-Eintrag aufbauen
    mov edi, nvme_sq_tmp
    xor eax, eax
    mov ecx, NVME_SQ_ENTRY / 4
    rep stosd
    mov byte [nvme_sq_tmp + NVME_SQE_OPCODE], NVME_ADM_IDENTIFY
    mov dword [nvme_sq_tmp + NVME_SQE_NSID], 0
    mov eax, [ebx + NVME_CTL_IDENT_PAGE]
    mov [nvme_sq_tmp + NVME_SQE_PRP1_LO], eax
    mov dword [nvme_sq_tmp + NVME_SQE_PRP1_HI], 0
    mov dword [nvme_sq_tmp + NVME_SQE_CDW10], 1   ; CNS=1 = Identify Controller

    mov esi, nvme_sq_tmp
    call nvme_submit_admin_cmd
    jc .failed

    ; Namespace-Anzahl lesen
    mov esi, [ebx + NVME_CTL_IDENT_PAGE]
    mov eax, [esi + NVME_ID_CTL_NN]
    test eax, eax
    jz .failed
    mov [ebx + NVME_CTL_NS_COUNT], eax

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
; nvme_create_io_queues: EBX=ctrl
; Legt ein I/O SQ/CQ Paar an.
; CF=0 Erfolg, CF=1 Fehler
; ---------------------------------------------------------------------------
nvme_create_io_queues:
    push esi
    push edi

    ; I/O CQ Seite allozieren: NVME_IO_DEPTH * 16 Bytes = 512 Bytes (1 Seite)
    call pmm_alloc_page
    test eax, eax
    jz .failed
    mov [ebx + NVME_CTL_IOCQ], eax
    push edi
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    pop edi

    ; Create I/O Completion Queue (Opcode 0x05)
    mov edi, nvme_sq_tmp
    xor eax, eax
    mov ecx, NVME_SQ_ENTRY / 4
    rep stosd
    mov byte [nvme_sq_tmp + NVME_SQE_OPCODE], NVME_ADM_CREATE_CQ
    mov eax, [ebx + NVME_CTL_IOCQ]
    mov [nvme_sq_tmp + NVME_SQE_PRP1_LO], eax
    mov dword [nvme_sq_tmp + NVME_SQE_PRP1_HI], 0
    ; CDW10: QID=1 (bits[15:0]), QSIZE=depth-1 (bits[31:16])
    mov dword [nvme_sq_tmp + NVME_SQE_CDW10], (((NVME_IO_DEPTH-1) << 16) | 1)
    ; CDW11: IV=0, IEN=0, PC=1 (Physically Contiguous)
    mov dword [nvme_sq_tmp + NVME_SQE_CDW11], 0x00000001

    mov esi, nvme_sq_tmp
    call nvme_submit_admin_cmd
    jc .failed

    ; I/O SQ Seite: NVME_IO_DEPTH * 64 Bytes = 2048 Bytes < 1 Seite
    call pmm_alloc_page
    test eax, eax
    jz .failed
    mov [ebx + NVME_CTL_IOSQ], eax
    push edi
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    pop edi

    ; Create I/O Submission Queue (Opcode 0x01)
    mov edi, nvme_sq_tmp
    xor eax, eax
    mov ecx, NVME_SQ_ENTRY / 4
    rep stosd
    mov byte [nvme_sq_tmp + NVME_SQE_OPCODE], NVME_ADM_CREATE_SQ
    mov eax, [ebx + NVME_CTL_IOSQ]
    mov [nvme_sq_tmp + NVME_SQE_PRP1_LO], eax
    mov dword [nvme_sq_tmp + NVME_SQE_PRP1_HI], 0
    ; CDW10: QID=1, QSIZE=depth-1
    mov dword [nvme_sq_tmp + NVME_SQE_CDW10], (((NVME_IO_DEPTH-1) << 16) | 1)
    ; CDW11: CQID=1, PC=1, QPRIO=0
    mov dword [nvme_sq_tmp + NVME_SQE_CDW11], 0x00010001

    mov esi, nvme_sq_tmp
    call nvme_submit_admin_cmd
    jc .failed

    ; I/O Queue-Indizes
    mov dword [ebx + NVME_CTL_SQTAIL_IO], 0
    mov dword [ebx + NVME_CTL_CQHEAD_IO], 0
    mov dword [ebx + NVME_CTL_PHASE_IO], 1

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
; nvme_namespace_discover: EBX=ctrl
; Identify Namespace 1, registriert als Block-Device.
; CF=0 immer
; ---------------------------------------------------------------------------
nvme_namespace_discover:
    push esi
    push edi

    ; Identify Namespace 1 (CDW10: NSID=1, CNS=0)
    mov edi, nvme_sq_tmp
    xor eax, eax
    mov ecx, NVME_SQ_ENTRY / 4
    rep stosd
    mov byte [nvme_sq_tmp + NVME_SQE_OPCODE], NVME_ADM_IDENTIFY
    mov dword [nvme_sq_tmp + NVME_SQE_NSID], 1
    mov eax, [ebx + NVME_CTL_IDENT_PAGE]
    ; Puffer wiederverwenden, nullen
    push edi
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    pop edi
    mov eax, [ebx + NVME_CTL_IDENT_PAGE]
    mov [nvme_sq_tmp + NVME_SQE_PRP1_LO], eax
    mov dword [nvme_sq_tmp + NVME_SQE_PRP1_HI], 0
    mov dword [nvme_sq_tmp + NVME_SQE_CDW10], 0   ; CNS=0 = Identify Namespace

    mov esi, nvme_sq_tmp
    call nvme_submit_admin_cmd
    jc .no_ns

    ; Namespace-Groesse (8 Bytes NSZE)
    mov esi, [ebx + NVME_CTL_IDENT_PAGE]
    mov eax, [esi + NVME_ID_NS_NSZE]      ; lo32
    mov edx, [esi + NVME_ID_NS_NSZE + 4]  ; hi32
    test eax, eax
    jz .no_ns
    mov [ebx + NVME_CTL_NS1_SIZE_LO], eax
    mov [ebx + NVME_CTL_NS1_SIZE_HI], edx

    ; LBA-Format 0: Block-Groesse = 2^LBADS
    mov ecx, [esi + NVME_ID_NS_LBAF0]
    shr ecx, 16
    and ecx, 0xFF              ; LBADS
    mov eax, 1
    test ecx, ecx
    jz .bsize_done
.shift:
    shl eax, 1
    dec ecx
    jnz .shift
.bsize_done:
    cmp eax, 512
    jb .no_ns                  ; ungueltige Blockgroesse
    mov [ebx + NVME_CTL_NS1_BSIZE], eax

    ; Als Block-Device registrieren
    mov eax, [ebx + NVME_CTL_NS1_SIZE_LO]
    mov dword [blk_tmp_id], 0x4E564D45   ; "NVME"
    mov dword [blk_tmp_flags], BLOCK_FLAG_FLUSH_CAP
    mov ecx, [ebx + NVME_CTL_NS1_BSIZE]
    mov [blk_tmp_bsize], ecx
    mov ecx, [ebx + NVME_CTL_NS1_SIZE_LO]
    mov [blk_tmp_cnt_lo], ecx
    mov ecx, [ebx + NVME_CTL_NS1_SIZE_HI]
    mov [blk_tmp_cnt_hi], ecx
    mov dword [blk_tmp_transport], BLOCK_TRANSPORT_NVME
    mov [blk_tmp_handle], ebx
    mov dword [blk_tmp_read_fn], nvme_blk_read
    mov dword [blk_tmp_write_fn], nvme_blk_write
    mov dword [blk_tmp_flush_fn], nvme_blk_flush
    mov dword [blk_tmp_offline_fn], 0
    call block_device_register
    jc .no_ns
    mov [ebx + NVME_CTL_BLKDEV], esi

    mov esi, message_nvme_ns_ok
    call serial_write_string

.no_ns:
    pop edi
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; nvme_submit_io_cmd: EBX=ctrl, ESI=SQ-Entry-Zeiger
; Schreibt I/O-SQ-Eintrag, klingelt Doorbell QID=1, wartet auf CQ.
; CF=0 Erfolg, CF=1 Fehler
; ---------------------------------------------------------------------------
nvme_submit_io_cmd:
    push ecx
    push edx
    push edi

    ; SQ-Eintrag schreiben
    mov edi, [ebx + NVME_CTL_IOSQ]
    mov ecx, [ebx + NVME_CTL_SQTAIL_IO]
    imul ecx, NVME_SQ_ENTRY
    add edi, ecx

    push esi
    mov ecx, NVME_SQ_ENTRY / 4
    rep movsd
    pop esi

    ; Command-ID
    mov ecx, [ebx + NVME_CTL_NEXT_CID]
    mov [edi - NVME_SQ_ENTRY + NVME_SQE_CID], cx
    inc dword [ebx + NVME_CTL_NEXT_CID]

    xor eax, eax
    out 0x80, al

    ; SQ Tail Doorbell (QID=1): MMIO + 0x1000 + 1*2*dstride
    mov edi, [ebx + NVME_CTL_SQTAIL_IO]
    inc edi
    cmp edi, NVME_IO_DEPTH
    jb .no_wrap
    xor edi, edi
.no_wrap:
    mov [ebx + NVME_CTL_SQTAIL_IO], edi

    mov eax, [ebx + NVME_CTL_BASE]
    add eax, NVME_DB_BASE
    mov edx, [ebx + NVME_CTL_DSTRIDE]
    lea eax, [eax + edx*2]     ; QID=1: offset = 2*dstride*1
    mov [eax], edi

    ; Auf CQ-Completion warten
    mov eax, [ebx + NVME_CTL_IOCQ]
    mov ecx, [ebx + NVME_CTL_CQHEAD_IO]
    imul ecx, NVME_CQ_ENTRY
    add eax, ecx

    mov edi, 5000000
.wait:
    test edi, edi
    jz .timeout
    movzx ecx, word [eax + NVME_CQE_STATUS]
    and ecx, 1
    cmp ecx, [ebx + NVME_CTL_PHASE_IO]
    je .ready
    dec edi
    jmp .wait
.timeout:
    pop edi
    pop edx
    pop ecx
    stc
    ret
.ready:
    movzx ecx, word [eax + NVME_CQE_STATUS]
    shr ecx, 1
    and ecx, 0x7FF

    mov edi, [ebx + NVME_CTL_CQHEAD_IO]
    inc edi
    cmp edi, NVME_IO_DEPTH
    jb .no_wrap_cq
    xor edi, edi
    xor dword [ebx + NVME_CTL_PHASE_IO], 1
.no_wrap_cq:
    mov [ebx + NVME_CTL_CQHEAD_IO], edi
    ; CQ Head Doorbell (QID=1): MMIO + 0x1000 + 2*dstride + dstride
    mov eax, [ebx + NVME_CTL_BASE]
    add eax, NVME_DB_BASE
    mov edx, [ebx + NVME_CTL_DSTRIDE]
    lea eax, [eax + edx*2]
    add eax, edx
    mov [eax], edi

    test ecx, ecx
    jnz .cmd_error
    pop edi
    pop edx
    pop ecx
    clc
    ret
.cmd_error:
    pop edi
    pop ecx
    pop edx
    stc
    ret

; ---------------------------------------------------------------------------
; nvme_blk_read: Block-Layer-Callback
; ESI=block_record, EAX=LBA_lo, EDX=LBA_hi, ECX=block_count, EBX=buffer
; CF=0 Erfolg, CF=1 Fehler
; ---------------------------------------------------------------------------
nvme_blk_read:
    push esi
    push edi
    push ebx
    push ecx
    push edx

    ; Controller aus BLK_DRIVER_HANDLE holen
    mov ebx, [esi + BLK_DRIVER_HANDLE]
    test ebx, ebx
    jz .failed

    ; SQ-Eintrag aufbauen
    mov edi, nvme_sq_tmp
    push ecx
    mov ecx, NVME_SQ_ENTRY / 4
    xor eax, eax
    rep stosd
    pop ecx
    mov byte [nvme_sq_tmp + NVME_SQE_OPCODE], NVME_IO_READ
    mov dword [nvme_sq_tmp + NVME_SQE_NSID], 1
    mov eax, [esp + 8]         ; buffer (EBX urspruenglich)
    mov [nvme_sq_tmp + NVME_SQE_PRP1_LO], eax
    mov dword [nvme_sq_tmp + NVME_SQE_PRP1_HI], 0
    mov eax, [esp + 16]        ; LBA_lo
    mov [nvme_sq_tmp + NVME_SQE_CDW10], eax
    mov eax, [esp + 12]        ; LBA_hi
    mov [nvme_sq_tmp + NVME_SQE_CDW11], eax
    mov eax, [esp + 4]         ; block_count
    dec eax                    ; NLB = count - 1
    mov [nvme_sq_tmp + NVME_SQE_CDW12], eax

    mov esi, nvme_sq_tmp
    call nvme_submit_io_cmd
    pop edx
    pop ecx
    pop ebx
    pop edi
    pop esi
    ret
.failed:
    pop edx
    pop ecx
    pop ebx
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; nvme_blk_write: Block-Layer-Callback (gleiche Parameter wie nvme_blk_read)
; ---------------------------------------------------------------------------
nvme_blk_write:
    push esi
    push edi
    push ebx
    push ecx
    push edx

    mov ebx, [esi + BLK_DRIVER_HANDLE]
    test ebx, ebx
    jz .failed

    mov edi, nvme_sq_tmp
    push ecx
    mov ecx, NVME_SQ_ENTRY / 4
    xor eax, eax
    rep stosd
    pop ecx
    mov byte [nvme_sq_tmp + NVME_SQE_OPCODE], NVME_IO_WRITE
    mov dword [nvme_sq_tmp + NVME_SQE_NSID], 1
    mov eax, [esp + 8]
    mov [nvme_sq_tmp + NVME_SQE_PRP1_LO], eax
    mov dword [nvme_sq_tmp + NVME_SQE_PRP1_HI], 0
    mov eax, [esp + 16]
    mov [nvme_sq_tmp + NVME_SQE_CDW10], eax
    mov eax, [esp + 12]
    mov [nvme_sq_tmp + NVME_SQE_CDW11], eax
    mov eax, [esp + 4]
    dec eax
    mov [nvme_sq_tmp + NVME_SQE_CDW12], eax

    mov esi, nvme_sq_tmp
    call nvme_submit_io_cmd
    pop edx
    pop ecx
    pop ebx
    pop edi
    pop esi
    ret
.failed:
    pop edx
    pop ecx
    pop ebx
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; nvme_blk_flush: Flush-Callback
; ---------------------------------------------------------------------------
nvme_blk_flush:
    push esi
    push edi

    mov ebx, [esi + BLK_DRIVER_HANDLE]
    test ebx, ebx
    jz .failed

    mov edi, nvme_sq_tmp
    push ecx
    mov ecx, NVME_SQ_ENTRY / 4
    xor eax, eax
    rep stosd
    pop ecx
    mov byte [nvme_sq_tmp + NVME_SQE_OPCODE], NVME_IO_FLUSH
    mov dword [nvme_sq_tmp + NVME_SQE_NSID], 1

    mov esi, nvme_sq_tmp
    call nvme_submit_io_cmd
    pop edi
    pop esi
    ret
.failed:
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; nvme_self_test (5 Tests)
; ---------------------------------------------------------------------------
nvme_self_test:
    push esi
    push edi
    xor edi, edi

    cmp dword [nvme_mgr_ready], 1
    je .t2
    inc edi
.t2:
    ; Kein NVMe – kein Fehler (per NPSPEC)
    cmp dword [nvme_count], 0
    je .done
    ; Controller im RUNNING-Zustand?
    mov esi, nvme_controllers
    cmp dword [esi + NVME_CTL_STATE], NVME_STATE_RUNNING
    je .t3
    inc edi
.t3:
    ; Namespace-Groesse != 0?
    cmp dword [esi + NVME_CTL_NS1_SIZE_LO], 0
    jne .done
    inc edi
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
nvme_mgr_ready:       dd 0
nvme_count:           dd 0
nvme_pending_cid:     dd 0
nvme_cqe_dw0_tmp:     dd 0
ebp_save:             dd 0

align 64
nvme_sq_tmp:
    times NVME_SQ_ENTRY db 0

align 4
nvme_controllers:
    times NVME_CAPACITY * NVME_CTL_SIZE db 0

message_nvme_ns_ok: db "NOVA: NVMe Namespace 1 als Block-Device registriert", 13, 10, 0
