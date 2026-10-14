; ---------------------------------------------------------------------------
; Nova Execution-Engine §027
; NPSPEC-EXECUTION-ALGORITHM-0001      - Algorithmen-Auswahl
; NPSPEC-EXECUTION-CONTRACT-0001       - Ausfuehrungs-Vertrag
; NPSPEC-EXECUTION-DEADLINE-0001       - Deadline-Ausfuehrung
; NPSPEC-EXECUTION-DETERMINISM-0001    - Determinismus
; NPSPEC-EXECUTION-ENERGY-0001         - Energie-Budget
; NPSPEC-EXECUTION-LATENCY-0001        - Latenz-Ziele
; NPSPEC-EXECUTION-PROVIDER-0001       - Ausfuehrungs-Provider
; NPSPEC-EXECUTION-RESOURCEBUDGET-0001 - Ressourcen-Budget
; NPSPEC-EXECUTION-SEMANTICTYPES-0001  - Semantische Typen
; NPSPEC-EXECUTION-SOVEREIGNTY-0001    - Ausfuehrungs-Souveraenitaet
; NPSPEC-EXECUTION-TRUST-0001          - Vertrauens-Stufe
; ---------------------------------------------------------------------------

; Provider-Typen (NPSPEC-EXECUTION-PROVIDER-0001)
EXEC_PROV_CPU         equ 0
EXEC_PROV_GPU         equ 1
EXEC_PROV_NPU         equ 2
EXEC_PROV_DSP         equ 3
EXEC_PROV_REMOTE      equ 4    ; (NPSPEC-EXECUTION-SOVEREIGNTY-0001)

; Ausfuehrungs-Flags
EXEC_FLAG_DETERMINISTIC equ 0x01  ; (NPSPEC-EXECUTION-DETERMINISM-0001)
EXEC_FLAG_DEADLINE    equ 0x02  ; (NPSPEC-EXECUTION-DEADLINE-0001)
EXEC_FLAG_ENERGY_CAP  equ 0x04  ; (NPSPEC-EXECUTION-ENERGY-0001)
EXEC_FLAG_SOVEREIGN   equ 0x08  ; (NPSPEC-EXECUTION-SOVEREIGNTY-0001)
EXEC_FLAG_TRUSTED     equ 0x10  ; (NPSPEC-EXECUTION-TRUST-0001)

; Execution-Contract-Record (48 Bytes, NPSPEC-EXECUTION-CONTRACT-0001)
EXEC_CTR_SIZE         equ 48
EC_ID                 equ 0
EC_PROVIDER           equ 4    ; EXEC_PROV_*
EC_FLAGS              equ 8
EC_DEADLINE_TICKS     equ 12   ; (NPSPEC-EXECUTION-DEADLINE-0001)
EC_LATENCY_NS         equ 16   ; Max-Latenz in ns (NPSPEC-EXECUTION-LATENCY-0001)
EC_CPU_BUDGET         equ 20   ; CPU-Einheiten (NPSPEC-EXECUTION-RESOURCEBUDGET-0001)
EC_MEM_BUDGET         equ 24   ; Speicher-Seiten
EC_ENERGY_BUDGET      equ 28   ; Energie-uWh (NPSPEC-EXECUTION-ENERGY-0001)
EC_ALGO_ID            equ 32   ; Algorithmus-ID (NPSPEC-EXECUTION-ALGORITHM-0001)
EC_SEM_TYPE           equ 36   ; Semantischer Typ (NPSPEC-EXECUTION-SEMANTICTYPES-0001)
EC_TRUST_LEVEL        equ 40   ; (NPSPEC-EXECUTION-TRUST-0001)
; 44..47: reserviert

EXEC_CTR_CAPACITY     equ 8

; Provider-Record (32 Bytes)
EXEC_PROV_REC_SIZE    equ 32
EP_ID                 equ 0
EP_TYPE               equ 4
EP_CAPACITY           equ 8
EP_USED               equ 12
EP_FLAGS              equ 16
; 20..31: reserviert

EXEC_PROV_CAPACITY    equ 4

; ---------------------------------------------------------------------------
; execution_initialize
; ---------------------------------------------------------------------------
execution_initialize:
    push edi
    mov edi, exec_ctr_table
    xor eax, eax
    mov ecx, (EXEC_CTR_CAPACITY * EXEC_CTR_SIZE) / 4
    rep stosd
    mov edi, exec_prov_table
    mov ecx, (EXEC_PROV_CAPACITY * EXEC_PROV_REC_SIZE) / 4
    rep stosd
    pop edi
    mov dword [exec_ctr_count], 0
    mov dword [exec_prov_count], 0
    mov dword [exec_next_id], 1
    ; Standard-Provider: CPU
    push edi
    mov edi, exec_prov_table
    mov dword [edi + EP_ID], 1
    mov dword [edi + EP_TYPE], EXEC_PROV_CPU
    mov dword [edi + EP_CAPACITY], 100
    mov dword [edi + EP_USED], 0
    mov dword [edi + EP_FLAGS], EXEC_FLAG_DETERMINISTIC
    pop edi
    mov dword [exec_prov_count], 1
    mov dword [exec_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; exec_register_contract: EAX=provider, EBX=flags, ECX=deadline, EDX=cpu_budget
; (NPSPEC-EXECUTION-CONTRACT-0001)
; CF=0 -> ESI=contract_id
; ---------------------------------------------------------------------------
exec_register_contract:
    cmp dword [exec_ctr_count], EXEC_CTR_CAPACITY
    jae .full
    push edi
    push esi
    mov edi, exec_ctr_table
    push eax
    mov eax, [exec_ctr_count]
    imul eax, EXEC_CTR_SIZE
    add edi, eax
    pop eax
    mov esi, [exec_next_id]
    mov [edi + EC_ID], esi
    mov [edi + EC_PROVIDER], eax
    mov [edi + EC_FLAGS], ebx
    mov [edi + EC_DEADLINE_TICKS], ecx
    mov [edi + EC_CPU_BUDGET], edx
    mov dword [edi + EC_LATENCY_NS], 1000000  ; Default 1ms
    mov dword [edi + EC_MEM_BUDGET], 16
    mov dword [edi + EC_ENERGY_BUDGET], 0
    mov dword [edi + EC_ALGO_ID], 0
    mov dword [edi + EC_SEM_TYPE], 0
    mov dword [edi + EC_TRUST_LEVEL], 1
    inc dword [exec_next_id]
    inc dword [exec_ctr_count]
    pop esi
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; exec_check_deadline: EAX=contract_id -> CF=0 innerhalb Deadline
; (NPSPEC-EXECUTION-DEADLINE-0001)
; ---------------------------------------------------------------------------
exec_check_deadline:
    push esi
    push ecx
    mov esi, exec_ctr_table
    mov ecx, [exec_ctr_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + EC_ID], eax
    je .found
    add esi, EXEC_CTR_SIZE
    dec ecx
    jmp .scan
.found:
    test dword [esi + EC_FLAGS], EXEC_FLAG_DEADLINE
    jz .ok  ; Kein Deadline-Flag gesetzt
    cmp dword [esi + EC_DEADLINE_TICKS], 0
    je .ok
    push eax
    mov eax, [timer_ticks]
    cmp eax, [esi + EC_DEADLINE_TICKS]
    pop eax
    jae .missed
.ok:
    pop ecx
    pop esi
    clc
    ret
.missed:
.not_found:
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; execution_self_test
; ---------------------------------------------------------------------------
execution_self_test:
    push esi
    xor esi, esi
    cmp dword [exec_ready], 1
    je .t2
    inc esi
.t2:
    mov eax, EXEC_PROV_CPU
    mov ebx, EXEC_FLAG_DETERMINISTIC | EXEC_FLAG_TRUSTED
    mov ecx, 0         ; Kein Deadline
    mov edx, 10        ; 10 CPU-Einheiten
    call exec_register_contract
    jnc .t3
    inc esi
.t3:
    mov eax, 1
    call exec_check_deadline
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
exec_ready:      dd 0
exec_ctr_count:  dd 0
exec_prov_count: dd 0
exec_next_id:    dd 1

align 4
exec_ctr_table:
    times EXEC_CTR_CAPACITY * EXEC_CTR_SIZE db 0

align 4
exec_prov_table:
    times EXEC_PROV_CAPACITY * EXEC_PROV_REC_SIZE db 0
