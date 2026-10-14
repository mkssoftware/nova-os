; ---------------------------------------------------------------------------
; Nova Data-Movement §011
; NPSPEC-DATAMOVE-DMA-0001           - DMA-Transfers
; NPSPEC-DATAMOVE-KERNELBYPASS-0001  - Kernel-Bypass
; NPSPEC-DATAMOVE-LOCALITY-0001      - Datenlokaltitaet
; NPSPEC-DATAMOVE-RINGBUFFER-0001    - Ring-Buffer
; NPSPEC-DATAMOVE-SCATTERGATHER-0001 - Scatter-Gather
; NPSPEC-DATAMOVE-SHAREDBUFFER-0001  - Shared-Buffer
; NPSPEC-DATAMOVE-ZEROCOPY-0001      - Zero-Copy
; ---------------------------------------------------------------------------

; DMA-Modus (NPSPEC-DATAMOVE-DMA-0001)
DM_DMA_MEM_TO_MEM     equ 0
DM_DMA_MEM_TO_DEV     equ 1
DM_DMA_DEV_TO_MEM     equ 2
DM_DMA_DEV_TO_DEV     equ 3

; Transfer-Flags (NPSPEC-DATAMOVE-ZEROCOPY-0001)
DM_FLAG_ZEROCOPY      equ 0x01  ; Zero-Copy (NPSPEC-DATAMOVE-ZEROCOPY-0001)
DM_FLAG_BYPASS        equ 0x02  ; Kernel-Bypass (NPSPEC-DATAMOVE-KERNELBYPASS-0001)
DM_FLAG_SCATTER       equ 0x04  ; Scatter-Gather (NPSPEC-DATAMOVE-SCATTERGATHER-0001)
DM_FLAG_LOCAL         equ 0x08  ; Locality-Hint (NPSPEC-DATAMOVE-LOCALITY-0001)
DM_FLAG_SHARED        equ 0x10  ; Shared-Buffer (NPSPEC-DATAMOVE-SHAREDBUFFER-0001)

; DMA-Transfer-Record (32 Bytes)
DM_XFER_SIZE          equ 32
DX_SRC                equ 0    ; Quell-Adresse
DX_DST                equ 4    ; Ziel-Adresse
DX_LEN                equ 8    ; Laenge in Bytes
DX_FLAGS              equ 12   ; DM_FLAG_*
DX_MODE               equ 16   ; DM_DMA_*
DX_STATUS             equ 20   ; 0=pending 1=done 2=error
DX_CHANNEL            equ 24   ; DMA-Kanal (0-7)
; 28..31: reserviert

DM_XFER_CAPACITY      equ 8

; Ring-Buffer-Descriptor (NPSPEC-DATAMOVE-RINGBUFFER-0001)
DM_RING_SIZE          equ 32
DR_BASE               equ 0    ; Basis-Adresse
DR_CAPACITY           equ 4    ; Gesamtgroesse in Bytes
DR_HEAD               equ 8    ; Schreibzeiger
DR_TAIL               equ 12   ; Lesezeiger
DR_ENTRY_SIZE         equ 16   ; Eintragsgroesse
DR_FLAGS              equ 20
; 24..31: reserviert

DM_RING_CAPACITY      equ 4

; Scatter-Gather-Entry (8 Bytes, NPSPEC-DATAMOVE-SCATTERGATHER-0001)
DM_SGE_SIZE           equ 8
DSG_ADDR              equ 0
DSG_LEN               equ 4

; ---------------------------------------------------------------------------
; datamove_initialize
; ---------------------------------------------------------------------------
datamove_initialize:
    push edi
    mov edi, dm_xfer_table
    xor eax, eax
    mov ecx, (DM_XFER_CAPACITY * DM_XFER_SIZE) / 4
    rep stosd
    mov edi, dm_ring_table
    mov ecx, (DM_RING_CAPACITY * DM_RING_SIZE) / 4
    rep stosd
    pop edi
    mov dword [dm_xfer_count], 0
    mov dword [dm_ring_count], 0
    mov dword [dm_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; dm_dma_transfer: EAX=src, EBX=dst, ECX=len, EDX=flags
; (NPSPEC-DATAMOVE-DMA-0001, NPSPEC-DATAMOVE-ZEROCOPY-0001)
; CF=0 -> EDI=xfer_slot
; ---------------------------------------------------------------------------
dm_dma_transfer:
    cmp dword [dm_xfer_count], DM_XFER_CAPACITY
    jae .full
    test ecx, ecx
    jz .invalid
    push edi
    push esi
    mov edi, dm_xfer_table
    push eax
    mov eax, [dm_xfer_count]
    imul eax, DM_XFER_SIZE
    add edi, eax
    pop eax
    mov [edi + DX_SRC], eax
    mov [edi + DX_DST], ebx
    mov [edi + DX_LEN], ecx
    mov [edi + DX_FLAGS], edx
    ; Modus aus Flags ableiten
    test edx, DM_FLAG_BYPASS
    jnz .bypass
    mov dword [edi + DX_MODE], DM_DMA_MEM_TO_MEM
    jmp .set_chan
.bypass:
    mov dword [edi + DX_MODE], DM_DMA_MEM_TO_DEV
.set_chan:
    ; Kanal = count mod 8
    push eax
    mov eax, [dm_xfer_count]
    and eax, 7
    mov [edi + DX_CHANNEL], eax
    pop eax
    mov dword [edi + DX_STATUS], 1  ; sofort "done" (kein echter DMA-HW)
    inc dword [dm_xfer_count]
    pop esi
    pop edi
    clc
    ret
.full:
.invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; dm_ringbuf_init: EAX=base_addr, EBX=capacity, ECX=entry_size
; (NPSPEC-DATAMOVE-RINGBUFFER-0001)
; CF=0 -> EDX=ring_slot
; ---------------------------------------------------------------------------
dm_ringbuf_init:
    cmp dword [dm_ring_count], DM_RING_CAPACITY
    jae .full
    push edi
    mov edi, dm_ring_table
    push eax
    mov eax, [dm_ring_count]
    imul eax, DM_RING_SIZE
    add edi, eax
    pop eax
    mov [edi + DR_BASE], eax
    mov [edi + DR_CAPACITY], ebx
    mov [edi + DR_ENTRY_SIZE], ecx
    mov dword [edi + DR_HEAD], 0
    mov dword [edi + DR_TAIL], 0
    mov dword [edi + DR_FLAGS], 0
    mov edx, [dm_ring_count]
    inc dword [dm_ring_count]
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; dm_scatter_gather: EAX=sge_array_ptr, EBX=sge_count, ECX=dst, EDX=flags
; (NPSPEC-DATAMOVE-SCATTERGATHER-0001)
; ---------------------------------------------------------------------------
dm_scatter_gather:
    test ebx, ebx
    jz .invalid
    push esi
    push ecx
    push edx
    mov esi, eax
    xor edx, edx
.loop:
    test ebx, ebx
    jz .done
    ; Jeden SGE als einzelnen Transfer queuen
    mov eax, [esi + DSG_ADDR]
    mov ecx, [esi + DSG_LEN]
    push ebx
    push esi
    mov ebx, [esp + 12]    ; dst
    mov edx, DM_FLAG_ZEROCOPY | DM_FLAG_SCATTER
    call dm_dma_transfer
    pop esi
    pop ebx
    add esi, DM_SGE_SIZE
    dec ebx
    jmp .loop
.done:
    pop edx
    pop ecx
    pop esi
    clc
    ret
.invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; datamove_self_test
; ---------------------------------------------------------------------------
datamove_self_test:
    push esi
    xor esi, esi
    cmp dword [dm_ready], 1
    je .t2
    inc esi
.t2:
    mov eax, 0x200000
    mov ebx, 0x201000
    mov ecx, 0x1000
    mov edx, DM_FLAG_ZEROCOPY
    call dm_dma_transfer
    jnc .t3
    inc esi
.t3:
    mov eax, 0x210000
    mov ebx, 512
    mov ecx, 64
    call dm_ringbuf_init
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
dm_ready:        dd 0
dm_xfer_count:   dd 0
dm_ring_count:   dd 0

align 4
dm_xfer_table:
    times DM_XFER_CAPACITY * DM_XFER_SIZE db 0

align 4
dm_ring_table:
    times DM_RING_CAPACITY * DM_RING_SIZE db 0
