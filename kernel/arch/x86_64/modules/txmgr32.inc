; ---------------------------------------------------------------------------
; Nova Transaction-Manager §034
; NPSPEC-TRANSACTION-BARRIER-0001      - Barriers
; NPSPEC-TRANSACTION-COMPENSATION-0001 - Kompensation
; NPSPEC-TRANSACTION-CONFIG-0001       - Konfiguration
; NPSPEC-TRANSACTION-LOG-0001          - Transaction-Log
; NPSPEC-TRANSACTION-MULTISERVICE-0001 - Multi-Service
; NPSPEC-TRANSACTION-RESOURCE-0001     - Ressource-Einbindung
; NPSPEC-TRANSACTION-ROLLBACK-0001     - Rollback
; NPSPEC-TRANSACTION-SYSTEM-0001       - System-Transaktionen
; ---------------------------------------------------------------------------

; Transaction-Typen (NPSPEC-TRANSACTION-SYSTEM-0001)
TX_TYPE_LOCAL         equ 0    ; Lokale Transaktion
TX_TYPE_DISTRIBUTED   equ 1    ; Verteilt (NPSPEC-TRANSACTION-MULTISERVICE-0001)
TX_TYPE_CONFIG        equ 2    ; Konfiguration (NPSPEC-TRANSACTION-CONFIG-0001)
TX_TYPE_RESOURCE      equ 3    ; Ressource (NPSPEC-TRANSACTION-RESOURCE-0001)

; Transaction-Zustaende
TX_STATE_ACTIVE       equ 0
TX_STATE_PREPARED     equ 1    ; 2-Phase-Commit
TX_STATE_COMMITTED    equ 2
TX_STATE_ROLLED_BACK  equ 3    ; (NPSPEC-TRANSACTION-ROLLBACK-0001)
TX_STATE_FAILED       equ 4

; Transaction-Flags
TX_FLAG_ATOMIC        equ 0x01
TX_FLAG_BARRIER       equ 0x02  ; (NPSPEC-TRANSACTION-BARRIER-0001)
TX_FLAG_COMPENSATE    equ 0x04  ; (NPSPEC-TRANSACTION-COMPENSATION-0001)
TX_FLAG_LOGGED        equ 0x08  ; (NPSPEC-TRANSACTION-LOG-0001)
TX_FLAG_MULTI_SVC     equ 0x10  ; (NPSPEC-TRANSACTION-MULTISERVICE-0001)

; System-Transaction-Record (48 Bytes)
SYSMGR_TX_SIZE        equ 48
ST_ID                 equ 0
ST_TYPE               equ 4    ; TX_TYPE_*
ST_STATE              equ 8    ; TX_STATE_*
ST_FLAGS              equ 12
ST_PARTICIPANT_COUNT  equ 16   ; Anzahl beteiligter Services
ST_LOG_HEAD           equ 20   ; Log-Pointer (NPSPEC-TRANSACTION-LOG-0001)
ST_COMP_FN            equ 24   ; Kompensations-Funktion (NPSPEC-TRANSACTION-COMPENSATION-0001)
ST_BARRIER_ID         equ 28   ; Barrier-ID (NPSPEC-TRANSACTION-BARRIER-0001)
ST_START_TICKS        equ 32
ST_TIMEOUT_TICKS      equ 36
ST_RESOURCE_MASK      equ 40   ; Belegte Ressourcen (NPSPEC-TRANSACTION-RESOURCE-0001)
; 44..47: reserviert

SYSMGR_TX_CAPACITY    equ 8

; ---------------------------------------------------------------------------
; txmgr_initialize
; ---------------------------------------------------------------------------
txmgr_initialize:
    push edi
    mov edi, sysmgr_tx_table
    xor eax, eax
    mov ecx, (SYSMGR_TX_CAPACITY * SYSMGR_TX_SIZE) / 4
    rep stosd
    pop edi
    mov dword [sysmgr_tx_count], 0
    mov dword [sysmgr_tx_next_id], 1
    mov dword [sysmgr_tx_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; txmgr_begin: EAX=type, EBX=flags, ECX=timeout_ticks
; (NPSPEC-TRANSACTION-SYSTEM-0001)
; CF=0 -> EDX=tx_id
; ---------------------------------------------------------------------------
txmgr_begin:
    cmp dword [sysmgr_tx_count], SYSMGR_TX_CAPACITY
    jae .full
    push edi
    push esi
    mov edi, sysmgr_tx_table
    push eax
    mov eax, [sysmgr_tx_count]
    imul eax, SYSMGR_TX_SIZE
    add edi, eax
    pop eax
    mov edx, [sysmgr_tx_next_id]
    mov [edi + ST_ID], edx
    mov [edi + ST_TYPE], eax
    mov dword [edi + ST_STATE], TX_STATE_ACTIVE
    mov [edi + ST_FLAGS], ebx
    mov dword [edi + ST_PARTICIPANT_COUNT], 1
    mov dword [edi + ST_LOG_HEAD], 0
    mov dword [edi + ST_COMP_FN], 0
    mov dword [edi + ST_BARRIER_ID], 0
    push eax
    mov eax, [timer_ticks]
    mov [edi + ST_START_TICKS], eax
    pop eax
    mov [edi + ST_TIMEOUT_TICKS], ecx
    mov dword [edi + ST_RESOURCE_MASK], 0
    inc dword [sysmgr_tx_next_id]
    inc dword [sysmgr_tx_count]
    pop esi
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; txmgr_commit: EAX=tx_id (NPSPEC-TRANSACTION-SYSTEM-0001)
; CF=0 Erfolg
; ---------------------------------------------------------------------------
txmgr_commit:
    push esi
    push ecx
    mov esi, sysmgr_tx_table
    mov ecx, [sysmgr_tx_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + ST_ID], eax
    je .found
    add esi, SYSMGR_TX_SIZE
    dec ecx
    jmp .scan
.found:
    cmp dword [esi + ST_STATE], TX_STATE_ACTIVE
    jne .invalid_state
    ; Timeout pruefen
    test dword [esi + ST_TIMEOUT_TICKS], 0xFFFFFFFF
    jz .commit
    push eax
    mov eax, [timer_ticks]
    sub eax, [esi + ST_START_TICKS]
    cmp eax, [esi + ST_TIMEOUT_TICKS]
    pop eax
    jae .timeout
.commit:
    mov dword [esi + ST_STATE], TX_STATE_COMMITTED
    pop ecx
    pop esi
    clc
    ret
.timeout:
.invalid_state:
    mov dword [esi + ST_STATE], TX_STATE_FAILED
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
; txmgr_rollback: EAX=tx_id (NPSPEC-TRANSACTION-ROLLBACK-0001)
; ---------------------------------------------------------------------------
txmgr_rollback:
    push esi
    push ecx
    mov esi, sysmgr_tx_table
    mov ecx, [sysmgr_tx_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + ST_ID], eax
    je .found
    add esi, SYSMGR_TX_SIZE
    dec ecx
    jmp .scan
.found:
    ; Kompensation ausfuehren wenn registriert (NPSPEC-TRANSACTION-COMPENSATION-0001)
    test dword [esi + ST_FLAGS], TX_FLAG_COMPENSATE
    jz .no_comp
    cmp dword [esi + ST_COMP_FN], 0
    je .no_comp
    push eax
    call [esi + ST_COMP_FN]
    pop eax
.no_comp:
    mov dword [esi + ST_STATE], TX_STATE_ROLLED_BACK
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
; txmgr_self_test
; ---------------------------------------------------------------------------
txmgr_self_test:
    push esi
    xor esi, esi
    cmp dword [sysmgr_tx_ready], 1
    je .t2
    inc esi
.t2:
    mov eax, TX_TYPE_LOCAL
    mov ebx, TX_FLAG_ATOMIC | TX_FLAG_LOGGED
    mov ecx, 10000
    call txmgr_begin
    jnc .t3
    inc esi
.t3:
    ; tx_id=1
    mov eax, 1
    call txmgr_commit
    jnc .t4
    inc esi
.t4:
    mov eax, TX_TYPE_LOCAL
    mov ebx, TX_FLAG_COMPENSATE
    mov ecx, 5000
    call txmgr_begin
    jnc .t5
    inc esi
.t5:
    mov eax, 2
    call txmgr_rollback
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
sysmgr_tx_ready:    dd 0
sysmgr_tx_count:    dd 0
sysmgr_tx_next_id:  dd 1

align 4
sysmgr_tx_table:
    times SYSMGR_TX_CAPACITY * SYSMGR_TX_SIZE db 0
