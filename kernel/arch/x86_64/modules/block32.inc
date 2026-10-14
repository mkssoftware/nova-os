; ---------------------------------------------------------------------------
; Storage Block Layer
; NPSPEC-STORAGE-BLOCK-0001 / NPSPEC-STORAGE-HOTPLUG-0001
; NPSPEC-STORAGE-INTEGRATION-0001
; ---------------------------------------------------------------------------

; Transporttypen
BLOCK_TRANSPORT_AHCI  equ 0
BLOCK_TRANSPORT_NVME  equ 1
BLOCK_TRANSPORT_USB   equ 2
BLOCK_TRANSPORT_VIRT  equ 3

; Block-Device-Flags
BLOCK_FLAG_READONLY   equ 0x00000001
BLOCK_FLAG_REMOVABLE  equ 0x00000002
BLOCK_FLAG_FLUSH_CAP  equ 0x00000004
BLOCK_FLAG_ONLINE     equ 0x00000008
BLOCK_FLAG_DEGRADED   equ 0x00000010

; Gesundheitsstatus
BLOCK_HEALTH_OK       equ 0
BLOCK_HEALTH_WARN     equ 1
BLOCK_HEALTH_FAIL     equ 2
BLOCK_HEALTH_UNKNOWN  equ 3

; Block-Device-Record (64 Bytes)
BLOCK_RECORD_SIZE     equ 64
BLK_ID                equ 0    ; Stabile Geraete-ID (CRC32)
BLK_FLAGS             equ 4    ; BLOCK_FLAG_*
BLK_BLOCK_SIZE        equ 8    ; Bytes pro Block
BLK_BLOCK_COUNT_LO    equ 12   ; Blockanzahl (niedrig)
BLK_BLOCK_COUNT_HI    equ 16   ; Blockanzahl (hoch)
BLK_TRANSPORT         equ 20   ; BLOCK_TRANSPORT_*
BLK_DRIVER_HANDLE     equ 24   ; Treiber-interener Handle
BLK_READS             equ 28   ; Zaehler
BLK_WRITES            equ 32   ; Zaehler
BLK_ERRORS            equ 36   ; Zaehler
BLK_HEALTH            equ 40   ; BLOCK_HEALTH_*
BLK_READ_FN           equ 44   ; Zeiger auf read-Funktion
BLK_WRITE_FN          equ 48   ; Zeiger auf write-Funktion
BLK_FLUSH_FN          equ 52   ; Zeiger auf flush-Funktion
BLK_OFFLINE_FN        equ 56   ; Zeiger auf offline-Funktion
BLK_RESERVED          equ 60   ; Reserviert

BLOCK_CAPACITY        equ 8    ; Maximal 8 Blockgeraete

; Hotplug-Ereignistypen (NPSPEC-STORAGE-HOTPLUG-0001)
BLK_EVT_ADDED         equ 1
BLK_EVT_REMOVED       equ 2
BLK_EVT_MEDIA_CHANGED equ 3
BLK_EVT_OFFLINE       equ 4
BLK_EVT_RECOVERED     equ 5

; I/O-Richtungen
BLK_IO_READ           equ 0
BLK_IO_WRITE          equ 1
BLK_IO_FLUSH          equ 2

; ---------------------------------------------------------------------------
; block_layer_initialize
; CF=0 Erfolg
; ---------------------------------------------------------------------------
block_layer_initialize:
    mov edi, block_records
    xor eax, eax
    mov ecx, (BLOCK_CAPACITY * BLOCK_RECORD_SIZE) / 4
    rep stosd
    mov dword [block_count], 0
    mov dword [block_layer_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; block_layer_self_test (4 Tests)
; ---------------------------------------------------------------------------
block_layer_self_test:
    push esi
    push edi
    xor edi, edi

    cmp dword [block_layer_ready], 1
    je .t2
    inc edi
.t2:
    cmp dword [block_count], 0
    je .t3
    inc edi
.t3:
    ; Test: virtuelles Geraet registrieren und wieder entfernen
    xor eax, eax               ; id=0
    mov ebx, BLOCK_FLAG_ONLINE | BLOCK_FLAG_FLUSH_CAP
    mov ecx, 512               ; block_size
    mov edx, 0                 ; count_hi
    mov esi, 1                 ; count_lo (1 Block als Dummy)
    mov [blk_tmp_id], eax
    mov [blk_tmp_flags], ebx
    mov [blk_tmp_bsize], ecx
    mov [blk_tmp_cnt_hi], edx
    mov [blk_tmp_cnt_lo], esi
    mov dword [blk_tmp_transport], BLOCK_TRANSPORT_VIRT
    mov dword [blk_tmp_handle], 0
    mov dword [blk_tmp_read_fn], 0
    mov dword [blk_tmp_write_fn], 0
    mov dword [blk_tmp_flush_fn], 0
    mov dword [blk_tmp_offline_fn], 0
    call block_device_register
    jnc .t4
    inc edi
    jmp .done
.t4:
    cmp dword [block_count], 1
    je .cleanup
    inc edi
.cleanup:
    mov dword [block_count], 0
    push edi                    ; Fehlerzähler sichern – rep stosd clobbert EDI
    mov edi, block_records
    xor eax, eax
    mov ecx, BLOCK_RECORD_SIZE / 4
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
; block_device_register
; Schreibt blk_tmp_* in einen freien Slot.
; CF=0 Erfolg (ESI = Record-Zeiger), CF=1 voll
; ---------------------------------------------------------------------------
block_device_register:
    push edi
    cmp dword [block_count], BLOCK_CAPACITY
    jae .full
    mov edi, block_records
    mov ecx, BLOCK_CAPACITY
.scan:
    test ecx, ecx
    jz .no_slot
    test dword [edi + BLK_FLAGS], BLOCK_FLAG_ONLINE
    jz .got_slot
    add edi, BLOCK_RECORD_SIZE
    dec ecx
    jmp .scan
.no_slot:
    pop edi
    stc
    ret
.got_slot:
    push ecx
    mov eax, [blk_tmp_id]
    mov [edi + BLK_ID], eax
    mov eax, [blk_tmp_flags]
    or eax, BLOCK_FLAG_ONLINE
    mov [edi + BLK_FLAGS], eax
    mov eax, [blk_tmp_bsize]
    mov [edi + BLK_BLOCK_SIZE], eax
    mov eax, [blk_tmp_cnt_lo]
    mov [edi + BLK_BLOCK_COUNT_LO], eax
    mov eax, [blk_tmp_cnt_hi]
    mov [edi + BLK_BLOCK_COUNT_HI], eax
    mov eax, [blk_tmp_transport]
    mov [edi + BLK_TRANSPORT], eax
    mov eax, [blk_tmp_handle]
    mov [edi + BLK_DRIVER_HANDLE], eax
    mov dword [edi + BLK_READS], 0
    mov dword [edi + BLK_WRITES], 0
    mov dword [edi + BLK_ERRORS], 0
    mov dword [edi + BLK_HEALTH], BLOCK_HEALTH_OK
    mov eax, [blk_tmp_read_fn]
    mov [edi + BLK_READ_FN], eax
    mov eax, [blk_tmp_write_fn]
    mov [edi + BLK_WRITE_FN], eax
    mov eax, [blk_tmp_flush_fn]
    mov [edi + BLK_FLUSH_FN], eax
    mov eax, [blk_tmp_offline_fn]
    mov [edi + BLK_OFFLINE_FN], eax
    inc dword [block_count]
    mov esi, edi
    pop ecx
    pop edi
    clc
    ret
.full:
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; block_device_offline: ESI=Record-Zeiger
; Setzt Geraet offline, bricht offene I/O ab (sofern implementiert).
; ---------------------------------------------------------------------------
block_device_offline:
    test esi, esi
    jz .done
    and dword [esi + BLK_FLAGS], ~BLOCK_FLAG_ONLINE
    or  dword [esi + BLK_FLAGS], BLOCK_FLAG_DEGRADED
    mov dword [esi + BLK_HEALTH], BLOCK_HEALTH_FAIL
.done:
    clc
    ret

; ---------------------------------------------------------------------------
; block_io_submit
; ESI=Record-Zeiger, EAX=LBA_lo, EDX=LBA_hi, ECX=Block-Anzahl,
; EBX=Puffer-Adresse, EDI=Richtung (BLK_IO_READ/WRITE/FLUSH)
; CF=0 Erfolg, CF=1 Fehler (Out-of-range, Offline, Readonly)
; ---------------------------------------------------------------------------
block_io_submit:
    test esi, esi
    jz .invalid
    ; Online-Check
    test dword [esi + BLK_FLAGS], BLOCK_FLAG_ONLINE
    jz .invalid
    ; ReadOnly-Check fuer Schreibzugriff
    cmp edi, BLK_IO_WRITE
    jne .range_check
    test dword [esi + BLK_FLAGS], BLOCK_FLAG_READONLY
    jnz .invalid
.range_check:
    ; Einfache Bereichspruefung (nur LBA_hi == 0 fuer 32-Bit-Adressen)
    test edx, edx
    jnz .range_hi
    ; LBA_lo + count <= block_count_lo (vereinfacht fuer boot context)
    push eax
    add eax, ecx
    jc .range_overflow
    cmp eax, [esi + BLK_BLOCK_COUNT_LO]
    ja .range_overflow
    pop eax
    jmp .dispatch
.range_hi:
    ; Wenn LBA_hi > block_count_hi: ablehnen
    cmp edx, [esi + BLK_BLOCK_COUNT_HI]
    jae .invalid
.dispatch:
    cmp edi, BLK_IO_READ
    je .do_read
    cmp edi, BLK_IO_WRITE
    je .do_write
    cmp edi, BLK_IO_FLUSH
    je .do_flush
    jmp .invalid
.do_read:
    cmp dword [esi + BLK_READ_FN], 0
    jz .invalid
    push edi
    mov edi, [esi + BLK_READ_FN]
    call edi
    pop edi
    jc .io_error
    inc dword [esi + BLK_READS]
    clc
    ret
.do_write:
    cmp dword [esi + BLK_WRITE_FN], 0
    jz .invalid
    push edi
    mov edi, [esi + BLK_WRITE_FN]
    call edi
    pop edi
    jc .io_error
    inc dword [esi + BLK_WRITES]
    clc
    ret
.do_flush:
    cmp dword [esi + BLK_FLUSH_FN], 0
    jz .no_flush
    push edi
    mov edi, [esi + BLK_FLUSH_FN]
    call edi
    pop edi
    jc .io_error
    clc
    ret
.no_flush:
    clc
    ret
.range_overflow:
    pop eax
.io_error:
    inc dword [esi + BLK_ERRORS]
    mov dword [esi + BLK_HEALTH], BLOCK_HEALTH_WARN
.invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; block_device_lookup_by_handle
; EAX=driver_handle -> ESI=Record (CF=0) oder CF=1
; ---------------------------------------------------------------------------
block_device_lookup_by_handle:
    push ecx
    push edi
    mov edi, block_records
    mov ecx, BLOCK_CAPACITY
.scan:
    test ecx, ecx
    jz .not_found
    cmp dword [edi + BLK_DRIVER_HANDLE], eax
    je .found
    add edi, BLOCK_RECORD_SIZE
    dec ecx
    jmp .scan
.found:
    mov esi, edi
    pop edi
    pop ecx
    clc
    ret
.not_found:
    pop edi
    pop ecx
    stc
    ret

; ---------------------------------------------------------------------------
; Statische Daten
; ---------------------------------------------------------------------------
align 4
block_layer_ready:  dd 0
block_count:        dd 0
blk_tmp_id:         dd 0
blk_tmp_flags:      dd 0
blk_tmp_bsize:      dd 0
blk_tmp_cnt_lo:     dd 0
blk_tmp_cnt_hi:     dd 0
blk_tmp_transport:  dd 0
blk_tmp_handle:     dd 0
blk_tmp_read_fn:    dd 0
blk_tmp_write_fn:   dd 0
blk_tmp_flush_fn:   dd 0
blk_tmp_offline_fn: dd 0
align 4
block_records:
    times BLOCK_CAPACITY * BLOCK_RECORD_SIZE db 0
