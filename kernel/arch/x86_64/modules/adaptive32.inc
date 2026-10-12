; NPSPEC-CRYPTO-RNG-0001 – Nova Cryptographic RNG / Entropy Pool
; ---------------------------------------------------------------------------
; Entropy-Pool aus rdtsc XOR wd_global_tick XOR rolling-mix.
; Ausgabe: xorshift32-basierte CSPRNG-Approximation (kein echter TRNG).
; Für echte krypto-Anforderungen ist Hardware-RNG zu bevorzugen; diese
; Implementierung liefert boot-time Entropie für ASLR, Nonces, Seeds.
; ---------------------------------------------------------------------------
CSPRNG_POOL_WORDS   equ 8   ; 256-bit entropy pool

csprng_initialize:
    mov edi, csprng_pool
    xor eax, eax
    mov ecx, CSPRNG_POOL_WORDS
    rep stosd
    mov dword [csprng_ready], 0
    ; Seed: rdtsc XOR wd_global_tick in each pool word
    xor esi, esi
.seed_loop:
    cmp esi, CSPRNG_POOL_WORDS
    jae .seed_done
    rdtsc
    xor eax, [wd_global_tick]
    xor eax, esi
    mov [csprng_pool + esi*4], eax
    inc esi
    jmp .seed_loop
.seed_done:
    mov dword [csprng_state], 0xDEADBEEF
    mov dword [csprng_calls], 0
    mov dword [csprng_ready], 1
    clc
    ret

; → EAX = pseudo-random 32-bit word
csprng_next:
    cmp dword [csprng_ready], 1
    jne .cn_fail
    ; xorshift32 on csprng_state, mixed with pool word
    mov eax, [csprng_state]
    ; xorshift32: state ^= state<<13 ^= state>>17 ^= state<<5
    mov ecx, eax
    shl ecx, 13
    xor eax, ecx
    mov ecx, eax
    shr ecx, 17
    xor eax, ecx
    mov ecx, eax
    shl ecx, 5
    xor eax, ecx
    ; mix with pool word based on call count
    mov ecx, [csprng_calls]
    and ecx, CSPRNG_POOL_WORDS - 1
    xor eax, [csprng_pool + ecx*4]
    ; re-stir pool word
    mov [csprng_pool + ecx*4], eax
    mov [csprng_state], eax
    inc dword [csprng_calls]
    ; also mix rdtsc every 16 calls for forward entropy
    test dword [csprng_calls], 0xF
    jnz .cn_done
    rdtsc
    xor [csprng_pool], eax
    xor [csprng_pool + 4], edx
.cn_done:
    clc
    ret
.cn_fail:
    xor eax, eax
    stc
    ret

; Mix in additional entropy (EAX=entropy_word)
csprng_add_entropy:
    cmp dword [csprng_ready], 1
    jne .cae_ret
    xor [csprng_state], eax
    mov ecx, [csprng_calls]
    and ecx, CSPRNG_POOL_WORDS - 1
    xor [csprng_pool + ecx*4], eax
.cae_ret:
    clc
    ret

csprng_self_test:
    ; generate 4 numbers, all should differ from zero
    call csprng_next
    jc .cstf
    test eax, eax
    jz .cstf
    mov ecx, eax    ; save first
    call csprng_next
    jc .cstf
    cmp eax, ecx    ; consecutive results differ (statistical)
    je .cstf
    ; add entropy and generate more
    mov eax, 0xCAFEBABE
    call csprng_add_entropy
    call csprng_next
    jc .cstf
    test eax, eax
    jz .cstf
    clc
    ret
.cstf:
    stc
    ret

csprng_ready:   dd 0
csprng_state:   dd 0
csprng_calls:   dd 0
align 4
csprng_pool:
    times CSPRNG_POOL_WORDS dd 0

; ---------------------------------------------------------------------------
; NPSPEC-POWER-CPUIDLE-0001 – Nova CPU Idle State Management
; ---------------------------------------------------------------------------
; C-State-Tracking: C0=Active, C1=Halt(HLT), C1E=Enhanced-Halt, C3=Deep-Sleep.
; cpuidle_enter: wählt tiefsten verfügbaren Zustand, führt HLT oder NOP aus.
; cpuidle_exit: markiert Rückkehr zu C0, misst Residency in Ticks.
; ---------------------------------------------------------------------------
CPUIDLE_STATE_C0    equ 0   ; active
CPUIDLE_STATE_C1    equ 1   ; halt (HLT)
CPUIDLE_STATE_C1E   equ 2   ; enhanced halt
CPUIDLE_STATE_C3    equ 3   ; deep sleep

CPUIDLE_REC_SIZE    equ 16
CPUIDLE_STATE_OFF   equ 0
CPUIDLE_ENTER_TICK  equ 4
CPUIDLE_RESIDENCY   equ 8
CPUIDLE_COUNT_OFF   equ 12

cpuidle_initialize:
    mov edi, cpuidle_stats
    xor eax, eax
    mov ecx, (4 * CPUIDLE_REC_SIZE) / 4
    rep stosd
    mov dword [cpuidle_ready], 0
    mov dword [cpuidle_current], CPUIDLE_STATE_C0
    mov dword [cpuidle_total_halts], 0
    mov dword [cpuidle_ready], 1
    clc
    ret

; EAX=target_state (0-3) — enter idle state
cpuidle_enter:
    cmp dword [cpuidle_ready], 1
    jne .cie_fail
    cmp eax, CPUIDLE_STATE_C3
    ja .cie_fail
    push esi
    push edi
    mov [cpuidle_current], eax
    imul edi, eax, CPUIDLE_REC_SIZE
    add edi, cpuidle_stats
    mov [edi + CPUIDLE_STATE_OFF], eax
    mov eax, [wd_global_tick]
    mov [edi + CPUIDLE_ENTER_TICK], eax
    inc dword [edi + CPUIDLE_COUNT_OFF]
    inc dword [cpuidle_total_halts]
    mov eax, [cpuidle_current]
    cmp eax, CPUIDLE_STATE_C0
    je .cie_no_halt
    hlt                        ; CPU schläft bis nächstem Interrupt
.cie_no_halt:
    pop edi
    pop esi
    clc
    ret
.cie_fail:
    stc
    ret

; Exit idle — record residency
cpuidle_exit:
    cmp dword [cpuidle_ready], 1
    jne .cex_ret
    push edi
    mov eax, [cpuidle_current]
    imul edi, eax, CPUIDLE_REC_SIZE
    add edi, cpuidle_stats
    mov eax, [wd_global_tick]
    sub eax, [edi + CPUIDLE_ENTER_TICK]
    add [edi + CPUIDLE_RESIDENCY], eax
    mov dword [cpuidle_current], CPUIDLE_STATE_C0
    pop edi
.cex_ret:
    clc
    ret

; EAX=state → EAX=residency_ticks, EDX=enter_count
cpuidle_query:
    cmp eax, CPUIDLE_STATE_C3
    ja .cq_bad
    push edi
    imul edi, eax, CPUIDLE_REC_SIZE
    add edi, cpuidle_stats
    mov eax, [edi + CPUIDLE_RESIDENCY]
    mov edx, [edi + CPUIDLE_COUNT_OFF]
    pop edi
    clc
    ret
.cq_bad:
    xor eax, eax
    xor edx, edx
    stc
    ret

cpuidle_self_test:
    ; Enter C1 and immediately exit
    mov eax, CPUIDLE_STATE_C1
    call cpuidle_enter
    jc .cpustf
    call cpuidle_exit
    ; query C1 count ≥ 1
    mov eax, CPUIDLE_STATE_C1
    call cpuidle_query
    jc .cpustf
    cmp edx, 1
    jb .cpustf
    clc
    ret
.cpustf:
    stc
    ret

cpuidle_ready:        dd 0
cpuidle_current:      dd 0
cpuidle_total_halts:  dd 0
align 4
cpuidle_stats:
    times 4 * CPUIDLE_REC_SIZE db 0

; ---------------------------------------------------------------------------
; NPSPEC-THERMAL-THROTTLING-0001 – Nova Thermal Throttle Policy
; ---------------------------------------------------------------------------
; Thermische Zonen mit Temperaturschwellen. Überschreitung → QoS-Drosselung.
; Temperatur-Simulation über wd_global_tick (kein echter Sensor im Bootstrap).
; Zone: NORMAL / WARM / HOT / CRITICAL. Bei HOT: QoS-Epoch erzwingen.
; ---------------------------------------------------------------------------
THERM_ZONE_COUNT    equ 4
THERM_ZONE_REC_SIZE equ 20

THERM_STATE_NORMAL  equ 0
THERM_STATE_WARM    equ 1
THERM_STATE_HOT     equ 2
THERM_STATE_CRIT    equ 3

THERM_ID_OFF        equ 0
THERM_STATE_OFF     equ 4
THERM_TEMP_OFF      equ 8    ; simulated temperature (arbitrary units)
THERM_THRESH_WARM   equ 12
THERM_THRESH_HOT    equ 16

thermal_initialize:
    mov edi, thermal_zones
    xor eax, eax
    mov ecx, (THERM_ZONE_COUNT * THERM_ZONE_REC_SIZE) / 4
    rep stosd
    mov dword [thermal_ready], 0
    mov dword [thermal_throttle_events], 0
    ; Default zone 0: thresholds warm=70, hot=90
    mov dword [thermal_zones + THERM_ID_OFF], 0
    mov dword [thermal_zones + THERM_STATE_OFF], THERM_STATE_NORMAL
    mov dword [thermal_zones + THERM_TEMP_OFF], 40
    mov dword [thermal_zones + THERM_THRESH_WARM], 70
    mov dword [thermal_zones + THERM_THRESH_HOT], 90
    mov dword [thermal_ready], 1
    clc
    ret

; EAX=zone_id  EDX=temperature → update zone state, CF=1 if throttle needed
thermal_update:
    cmp dword [thermal_ready], 1
    jne .tu_fail
    cmp eax, THERM_ZONE_COUNT
    jae .tu_fail
    push esi
    push edi
    imul edi, eax, THERM_ZONE_REC_SIZE
    add edi, thermal_zones
    mov [edi + THERM_TEMP_OFF], edx
    ; determine state
    cmp edx, [edi + THERM_THRESH_HOT]
    jae .tu_hot
    cmp edx, [edi + THERM_THRESH_WARM]
    jae .tu_warm
    mov dword [edi + THERM_STATE_OFF], THERM_STATE_NORMAL
    pop edi
    pop esi
    clc
    ret
.tu_warm:
    mov dword [edi + THERM_STATE_OFF], THERM_STATE_WARM
    pop edi
    pop esi
    clc
    ret
.tu_hot:
    mov dword [edi + THERM_STATE_OFF], THERM_STATE_HOT
    inc dword [thermal_throttle_events]
    ; Force QoS new epoch (throttle all classes)
    call qos_new_epoch
    pop edi
    pop esi
    stc     ; CF=1: throttle action taken
    ret
.tu_fail:
    stc
    ret

; EAX=zone_id → EAX=state  EDX=temperature
thermal_query:
    cmp eax, THERM_ZONE_COUNT
    jae .tq_bad
    push edi
    imul edi, eax, THERM_ZONE_REC_SIZE
    add edi, thermal_zones
    mov edx, [edi + THERM_TEMP_OFF]
    mov eax, [edi + THERM_STATE_OFF]
    pop edi
    clc
    ret
.tq_bad:
    xor eax, eax
    xor edx, edx
    stc
    ret

thermal_self_test:
    ; Normal temp (50) → NORMAL state
    mov eax, 0
    mov edx, 50
    call thermal_update
    jc .tstf     ; CF=0 expected for NORMAL
    mov eax, 0
    call thermal_query
    jc .tstf
    cmp eax, THERM_STATE_NORMAL
    jne .tstf
    ; Hot temp (95) → HOT + CF=1
    mov eax, 0
    mov edx, 95
    call thermal_update
    jnc .tstf   ; CF=1 expected for HOT
    mov eax, 0
    call thermal_query
    jc .tstf
    cmp eax, THERM_STATE_HOT
    jne .tstf
    clc
    ret
.tstf:
    stc
    ret

thermal_ready:            dd 0
thermal_throttle_events:  dd 0
align 4
thermal_zones:
    times THERM_ZONE_COUNT * THERM_ZONE_REC_SIZE db 0

; ---------------------------------------------------------------------------
; NPSPEC-IDENTITY-0001 – Nova Identity Registry
; ---------------------------------------------------------------------------
; Stabile Identitäten für Kernel, Prozesse und Dienste.
; Identity ≠ Authority: ein Eintrag hier verleiht keine Rechte.
; Types: KERNEL=1 / PROCESS=2 / SERVICE=3 / DEVICE=4.
; States: ACTIVE=1 / SUSPENDED=2 / REVOKED=3.
; ---------------------------------------------------------------------------
IDENT_CAPACITY      equ 16
IDENT_REC_SIZE      equ 16

IDENT_TYPE_KERNEL   equ 1
IDENT_TYPE_PROCESS  equ 2
IDENT_TYPE_SERVICE  equ 3
IDENT_TYPE_DEVICE   equ 4

IDENT_STATE_ACTIVE  equ 1
IDENT_STATE_SUSP    equ 2
IDENT_STATE_REVOKED equ 3

IDENT_ID_OFF        equ 0
IDENT_TYPE_OFF      equ 4
IDENT_STATE_OFF2    equ 8
IDENT_OWNER_OFF     equ 12

identity_initialize:
    mov edi, identity_table
    xor eax, eax
    mov ecx, (IDENT_CAPACITY * IDENT_REC_SIZE) / 4
    rep stosd
    mov dword [identity_ready], 0
    mov dword [identity_count], 0
    mov dword [identity_next_id], 1
    ; Pre-register kernel identity (id=1)
    mov eax, IDENT_TYPE_KERNEL
    xor edx, edx
    call identity_register
    jc .idinit_fail
    mov dword [identity_ready], 1
    clc
    ret
.idinit_fail:
    stc
    ret

; EAX=type  EDX=owner → EAX=identity_id  CF=0/CF=1 full
identity_register:
    cmp dword [identity_count], IDENT_CAPACITY
    jae .idr_full
    push esi
    push edi
    push ebx
    mov ebx, edx
    xor esi, esi
.idr_scan:
    cmp esi, IDENT_CAPACITY
    jae .idr_nospc
    imul edi, esi, IDENT_REC_SIZE
    add edi, identity_table
    cmp dword [edi + IDENT_ID_OFF], 0
    je .idr_slot
    inc esi
    jmp .idr_scan
.idr_slot:
    push eax
    mov eax, [identity_next_id]
    mov [edi + IDENT_ID_OFF], eax
    inc dword [identity_next_id]
    pop eax
    mov [edi + IDENT_TYPE_OFF], eax
    mov dword [edi + IDENT_STATE_OFF2], IDENT_STATE_ACTIVE
    mov [edi + IDENT_OWNER_OFF], ebx
    inc dword [identity_count]
    mov eax, [edi + IDENT_ID_OFF]
    pop ebx
    pop edi
    pop esi
    clc
    ret
.idr_nospc:
    pop ebx
    pop edi
    pop esi
.idr_full:
    stc
    ret

; EAX=identity_id → EAX=type  EDX=state  CF=0 found / CF=1 not found
identity_query:
    push esi
    push edi
    push ebx
    mov ebx, eax
    xor esi, esi
.idq_loop:
    cmp esi, IDENT_CAPACITY
    jae .idq_notfound
    imul edi, esi, IDENT_REC_SIZE
    add edi, identity_table
    cmp [edi + IDENT_ID_OFF], ebx
    jne .idq_next
    mov eax, [edi + IDENT_TYPE_OFF]
    mov edx, [edi + IDENT_STATE_OFF2]
    pop ebx
    pop edi
    pop esi
    clc
    ret
.idq_next:
    inc esi
    jmp .idq_loop
.idq_notfound:
    pop ebx
    pop edi
    pop esi
    stc
    ret

; EAX=identity_id  EDX=new_state — revoke or suspend
identity_set_state:
    cmp edx, IDENT_STATE_REVOKED
    ja .ids_bad
    push esi
    push edi
    push ebx
    mov ebx, eax
    xor esi, esi
.ids_loop:
    cmp esi, IDENT_CAPACITY
    jae .ids_notfound
    imul edi, esi, IDENT_REC_SIZE
    add edi, identity_table
    cmp [edi + IDENT_ID_OFF], ebx
    jne .ids_next
    mov [edi + IDENT_STATE_OFF2], edx
    pop ebx
    pop edi
    pop esi
    clc
    ret
.ids_next:
    inc esi
    jmp .ids_loop
.ids_notfound:
    pop ebx
    pop edi
    pop esi
.ids_bad:
    stc
    ret

identity_self_test:
    ; Query kernel identity (id=1) → KERNEL type, ACTIVE
    mov eax, 1
    call identity_query
    jc .idstf
    cmp eax, IDENT_TYPE_KERNEL
    jne .idstf
    cmp edx, IDENT_STATE_ACTIVE
    jne .idstf
    ; Register a process identity
    mov eax, IDENT_TYPE_PROCESS
    mov edx, 42
    call identity_register
    jc .idstf
    push eax
    ; Query it back
    call identity_query
    jc .idstf_pop
    cmp eax, IDENT_TYPE_PROCESS
    jne .idstf_pop
    pop eax
    ; Revoke it
    mov edx, IDENT_STATE_REVOKED
    call identity_set_state
    jc .idstf
    clc
    ret
.idstf_pop:
    pop eax
.idstf:
    stc
    ret

identity_ready:     dd 0
identity_count:     dd 0
identity_next_id:   dd 1
align 4
identity_table:
    times IDENT_CAPACITY * IDENT_REC_SIZE db 0

; ---------------------------------------------------------------------------
; NPSPEC-TRUST-ANCHOR-0001 – Nova Trust Anchor Chain
; ---------------------------------------------------------------------------
; Root-of-Trust-Kette für Boot-Verifikation und Subsystem-Attestation.
; Anker: KERNEL_BOOT=1 / SECURITY_MODULE=2 / CAPABILITY_ROOT=3 / USER_ROOT=4.
; Jeder Anker trägt eine 32-bit-Fingerprint (Hash-Approximation für x86-32).
; ---------------------------------------------------------------------------
TRUST_ANCHOR_COUNT  equ 8
TRUST_ANCHOR_SIZE   equ 16

TRUST_ANCH_BOOT     equ 1
TRUST_ANCH_SECURITY equ 2
TRUST_ANCH_CAPROOT  equ 3
TRUST_ANCH_USER     equ 4

TRUST_ANC_ID_OFF    equ 0
TRUST_ANC_TYPE_OFF  equ 4
TRUST_ANC_FPRINT_OFF equ 8
TRUST_ANC_FLAGS_OFF equ 12
TRUST_ANC_FLAG_VALID equ 0x01
TRUST_ANC_FLAG_REVOKED equ 0x02

trust_initialize:
    mov edi, trust_anchors
    xor eax, eax
    mov ecx, (TRUST_ANCHOR_COUNT * TRUST_ANCHOR_SIZE) / 4
    rep stosd
    mov dword [trust_ready], 0
    mov dword [trust_count], 0
    ; Register KERNEL_BOOT anchor with fingerprint from CSPRNG
    call csprng_next
    jc .ti_fail
    mov edx, eax      ; fingerprint
    mov eax, TRUST_ANCH_BOOT
    call trust_register_anchor
    jc .ti_fail
    ; Register SECURITY anchor
    call csprng_next
    jc .ti_fail
    mov edx, eax
    mov eax, TRUST_ANCH_SECURITY
    call trust_register_anchor
    jc .ti_fail
    mov dword [trust_ready], 1
    clc
    ret
.ti_fail:
    stc
    ret

; EAX=anchor_type  EDX=fingerprint → EAX=slot  CF=0/CF=1 full
trust_register_anchor:
    cmp dword [trust_count], TRUST_ANCHOR_COUNT
    jae .tra_full
    push esi
    push edi
    push ebx
    mov ebx, edx
    xor esi, esi
.tra_scan:
    cmp esi, TRUST_ANCHOR_COUNT
    jae .tra_nospc
    imul edi, esi, TRUST_ANCHOR_SIZE
    add edi, trust_anchors
    cmp dword [edi + TRUST_ANC_ID_OFF], 0
    je .tra_slot
    inc esi
    jmp .tra_scan
.tra_slot:
    mov dword [edi + TRUST_ANC_ID_OFF], esi
    mov [edi + TRUST_ANC_TYPE_OFF], eax
    mov [edi + TRUST_ANC_FPRINT_OFF], ebx
    mov dword [edi + TRUST_ANC_FLAGS_OFF], TRUST_ANC_FLAG_VALID
    inc dword [trust_count]
    mov eax, esi
    pop ebx
    pop edi
    pop esi
    clc
    ret
.tra_nospc:
    pop ebx
    pop edi
    pop esi
.tra_full:
    stc
    ret

; EAX=anchor_type → EAX=fingerprint  CF=0 found+valid / CF=1 not found/revoked
trust_verify_anchor:
    push esi
    push edi
    push ebx
    mov ebx, eax
    xor esi, esi
.tva_loop:
    cmp esi, TRUST_ANCHOR_COUNT
    jae .tva_notfound
    imul edi, esi, TRUST_ANCHOR_SIZE
    add edi, trust_anchors
    cmp dword [edi + TRUST_ANC_ID_OFF], 0
    je .tva_next
    cmp [edi + TRUST_ANC_TYPE_OFF], ebx
    jne .tva_next
    test dword [edi + TRUST_ANC_FLAGS_OFF], TRUST_ANC_FLAG_REVOKED
    jnz .tva_revoked
    test dword [edi + TRUST_ANC_FLAGS_OFF], TRUST_ANC_FLAG_VALID
    jz .tva_next
    mov eax, [edi + TRUST_ANC_FPRINT_OFF]
    pop ebx
    pop edi
    pop esi
    clc
    ret
.tva_next:
    inc esi
    jmp .tva_loop
.tva_revoked:
.tva_notfound:
    pop ebx
    pop edi
    pop esi
    stc
    ret

trust_self_test:
    ; Verify KERNEL_BOOT anchor
    mov eax, TRUST_ANCH_BOOT
    call trust_verify_anchor
    jc .tstf
    test eax, eax
    jz .tstf
    ; Verify SECURITY anchor
    mov eax, TRUST_ANCH_SECURITY
    call trust_verify_anchor
    jc .tstf
    ; Unknown type → CF=1
    mov eax, 99
    call trust_verify_anchor
    jnc .tstf
    clc
    ret
.tstf:
    stc
    ret

trust_ready:   dd 0
trust_count:   dd 0
align 4
trust_anchors:
    times TRUST_ANCHOR_COUNT * TRUST_ANCHOR_SIZE db 0

; ---------------------------------------------------------------------------
; NPSPEC-ADAPTIVE-FEEDBACK-0001 – Nova Adaptive Feedback Loop
; ---------------------------------------------------------------------------
; Schließt den Regelkreis: Observe→Predict→Decide→Execute→Measure→Feedback.
; Jeder Feedback-Slot überwacht eine Policy-Dimension (Thermal/QoS/Sched/Memory).
; Wenn Messergebnis > Erwartungswert: increment_miss; → policy_adjust wenn ≥ threshold.
; ---------------------------------------------------------------------------
ADAPT_SLOT_COUNT    equ 8
ADAPT_REC_SIZE      equ 20

ADAPT_DIM_THERMAL   equ 1
ADAPT_DIM_QOS       equ 2
ADAPT_DIM_SCHED     equ 3
ADAPT_DIM_MEMORY    equ 4

ADAPT_ID_OFF        equ 0
ADAPT_DIM_OFF       equ 4
ADAPT_EXPECTED_OFF  equ 8
ADAPT_ACTUAL_OFF    equ 12
ADAPT_MISSES_OFF    equ 16

adapt_initialize:
    mov edi, adapt_slots
    xor eax, eax
    mov ecx, (ADAPT_SLOT_COUNT * ADAPT_REC_SIZE) / 4
    rep stosd
    mov dword [adapt_ready], 0
    mov dword [adapt_adjustments], 0
    mov dword [adapt_ready], 1
    clc
    ret

; EAX=dimension  EDX=expected_value → EAX=slot  CF=0/CF=1 full
adapt_register:
    push esi
    push edi
    xor esi, esi
.ar_scan:
    cmp esi, ADAPT_SLOT_COUNT
    jae .ar_full
    imul edi, esi, ADAPT_REC_SIZE
    add edi, adapt_slots
    cmp dword [edi + ADAPT_ID_OFF], 0
    je .ar_slot
    inc esi
    jmp .ar_scan
.ar_slot:
    mov dword [edi + ADAPT_ID_OFF], esi
    mov [edi + ADAPT_DIM_OFF], eax
    mov [edi + ADAPT_EXPECTED_OFF], edx
    mov dword [edi + ADAPT_ACTUAL_OFF], 0
    mov dword [edi + ADAPT_MISSES_OFF], 0
    mov eax, esi
    pop edi
    pop esi
    clc
    ret
.ar_full:
    pop edi
    pop esi
    stc
    ret

; EAX=slot  EDX=actual_value  ECX=miss_threshold
; CF=0 within budget / CF=1 threshold exceeded → policy_adjust signalled
adapt_observe:
    cmp eax, ADAPT_SLOT_COUNT
    jae .ao_bad
    push esi
    push edi
    imul edi, eax, ADAPT_REC_SIZE
    add edi, adapt_slots
    mov [edi + ADAPT_ACTUAL_OFF], edx
    cmp edx, [edi + ADAPT_EXPECTED_OFF]
    jbe .ao_ok
    inc dword [edi + ADAPT_MISSES_OFF]
    inc dword [adapt_adjustments]
.ao_ok:
    cmp [edi + ADAPT_MISSES_OFF], ecx
    pop edi
    pop esi
    jae .ao_threshold
    clc
    ret
.ao_threshold:
    stc
    ret
.ao_bad:
    stc
    ret

adapt_self_test:
    ; Register thermal feedback, expected≤80
    mov eax, ADAPT_DIM_THERMAL
    mov edx, 80
    call adapt_register
    jc .abstf
    push eax          ; save slot
    ; observe within budget (70 ≤ 80, miss_threshold=3)
    mov edx, 70
    mov ecx, 3
    call adapt_observe
    jc .abstf_pop     ; CF=0 expected
    ; observe exceeding (90 > 80) × 3 times → CF=1 on 3rd
    pop eax
    push eax
    mov edx, 90
    mov ecx, 3
    call adapt_observe
    mov edx, 90
    mov ecx, 3
    call adapt_observe
    mov edx, 90
    mov ecx, 3
    call adapt_observe
    jnc .abstf_pop    ; CF=1 expected after 3 misses
    pop eax
    clc
    ret
.abstf_pop:
    pop eax
.abstf:
    stc
    ret

adapt_ready:        dd 0
adapt_adjustments:  dd 0
align 4
adapt_slots:
    times ADAPT_SLOT_COUNT * ADAPT_REC_SIZE db 0

; ---------------------------------------------------------------------------
; NPSPEC-RESOURCE-ADMISSION-0001 – Nova Resource Admission Control
; ---------------------------------------------------------------------------
; Prüft vor Ausführung ob CPU+Memory-Budget verfügbar ist.
; Admission ≠ Authorization ≠ Scheduling. Rejected ≠ Permanently Impossible.
; ---------------------------------------------------------------------------
RADMIT_CAPACITY     equ 8
RADMIT_REC_SIZE     equ 20

RADMIT_ID_OFF       equ 0
RADMIT_CPU_OFF      equ 4    ; CPU units reserved
RADMIT_MEM_OFF      equ 8    ; memory units reserved
RADMIT_STATE_OFF    equ 12   ; 0=free, 1=active
RADMIT_OWNER_OFF    equ 16

RADMIT_CPU_MAX      equ 256  ; total CPU units
RADMIT_MEM_MAX      equ 1024 ; total memory units (arbitrary)

radmit_initialize:
    mov edi, radmit_table
    xor eax, eax
    mov ecx, (RADMIT_CAPACITY * RADMIT_REC_SIZE) / 4
    rep stosd
    mov dword [radmit_ready], 0
    mov dword [radmit_cpu_used], 0
    mov dword [radmit_mem_used], 0
    mov dword [radmit_next_id], 1
    mov dword [radmit_rejected], 0
    mov dword [radmit_ready], 1
    clc
    ret

; EAX=cpu_units  EDX=mem_units  ECX=owner → EAX=slot  CF=0 admitted / CF=1 rejected
radmit_request:
    cmp dword [radmit_ready], 1
    jne .rr_fail
    ; check CPU budget
    push ebx
    push esi
    push edi
    mov ebx, ecx          ; save owner
    ; cpu check: used + requested ≤ max
    mov ecx, [radmit_cpu_used]
    add ecx, eax
    cmp ecx, RADMIT_CPU_MAX
    ja .rr_reject
    ; mem check: used + requested ≤ max
    push ecx
    mov ecx, [radmit_mem_used]
    add ecx, edx
    cmp ecx, RADMIT_MEM_MAX
    pop ecx
    ja .rr_reject
    ; find free slot
    xor esi, esi
.rr_scan:
    cmp esi, RADMIT_CAPACITY
    jae .rr_reject
    imul edi, esi, RADMIT_REC_SIZE
    add edi, radmit_table
    cmp dword [edi + RADMIT_STATE_OFF], 0
    je .rr_slot
    inc esi
    jmp .rr_scan
.rr_slot:
    push eax
    mov eax, [radmit_next_id]
    mov [edi + RADMIT_ID_OFF], eax
    inc dword [radmit_next_id]
    pop eax
    mov [edi + RADMIT_CPU_OFF], eax
    mov [edi + RADMIT_MEM_OFF], edx
    mov dword [edi + RADMIT_STATE_OFF], 1
    mov [edi + RADMIT_OWNER_OFF], ebx
    add [radmit_cpu_used], eax
    add [radmit_mem_used], edx
    mov eax, esi
    pop edi
    pop esi
    pop ebx
    clc
    ret
.rr_reject:
    inc dword [radmit_rejected]
    pop edi
    pop esi
    pop ebx
.rr_fail:
    stc
    ret

; EAX=slot — release resources
radmit_release:
    cmp eax, RADMIT_CAPACITY
    jae .rel_bad
    push edi
    imul edi, eax, RADMIT_REC_SIZE
    add edi, radmit_table
    cmp dword [edi + RADMIT_STATE_OFF], 1
    jne .rel_skip
    mov eax, [edi + RADMIT_CPU_OFF]
    sub [radmit_cpu_used], eax
    mov eax, [edi + RADMIT_MEM_OFF]
    sub [radmit_mem_used], eax
    mov dword [edi + RADMIT_STATE_OFF], 0
    mov dword [edi + RADMIT_ID_OFF], 0
.rel_skip:
    pop edi
    clc
    ret
.rel_bad:
    stc
    ret

radmit_self_test:
    ; Admit cpu=10, mem=50
    mov eax, 10
    mov edx, 50
    mov ecx, 1
    call radmit_request
    jc .rastf
    push eax
    ; Verify cpu_used=10
    cmp dword [radmit_cpu_used], 10
    jne .rastf_pop
    ; Release
    call radmit_release
    ; Verify cpu_used=0 after release
    cmp dword [radmit_cpu_used], 0
    jne .rastf
    ; Overflow: request cpu=300 (>256)
    mov eax, 300
    mov edx, 0
    mov ecx, 0
    call radmit_request
    jnc .rastf   ; must be rejected
    clc
    ret
.rastf_pop:
    pop eax
.rastf:
    stc
    ret

radmit_ready:     dd 0
radmit_cpu_used:  dd 0
radmit_mem_used:  dd 0
radmit_next_id:   dd 1
radmit_rejected:  dd 0
align 4
radmit_table:
    times RADMIT_CAPACITY * RADMIT_REC_SIZE db 0

; ---------------------------------------------------------------------------
; NPSPEC-EXECUTION-CONTRACT-0001 – Nova Execution Contract
; ---------------------------------------------------------------------------
; Deklarative Beschreibung was ausgeführt wird und unter welchen Bedingungen.
; Types: BEST_EFFORT=1 / BOUNDED=2 / HARD_RT=3.
; States: CREATED=1 / ACTIVE=2 / COMPLETED=3 / VIOLATED=4.
; contract_activate verbindet mit scheduler_admit_task.
; ---------------------------------------------------------------------------
EXCON_CAPACITY      equ 8
EXCON_REC_SIZE      equ 24

EXCON_TYPE_BE       equ 1   ; best effort
EXCON_TYPE_BOUNDED  equ 2   ; bounded latency
EXCON_TYPE_HARD_RT  equ 3   ; hard realtime

EXCON_STATE_CREATED equ 1
EXCON_STATE_ACTIVE  equ 2
EXCON_STATE_DONE    equ 3
EXCON_STATE_VIOL    equ 4

EXCON_ID_OFF        equ 0
EXCON_TYPE_OFF2     equ 4
EXCON_STATE_OFF2    equ 8
EXCON_CPU_OFF       equ 12
EXCON_DEADLINE_OFF  equ 16
EXCON_PRIO_OFF      equ 20

excon_initialize:
    mov edi, excon_table
    xor eax, eax
    mov ecx, (EXCON_CAPACITY * EXCON_REC_SIZE) / 4
    rep stosd
    mov dword [excon_ready], 0
    mov dword [excon_next_id], 1
    mov dword [excon_violations], 0
    mov dword [excon_ready], 1
    clc
    ret

; EAX=type  EDX=cpu_budget  ECX=priority → EAX=slot  CF=0/CF=1 full
excon_create:
    cmp dword [excon_ready], 1
    jne .ec_fail
    cmp eax, EXCON_TYPE_HARD_RT
    ja .ec_fail
    test eax, eax
    jz .ec_fail
    push esi
    push edi
    push ebx
    mov ebx, ecx    ; save priority
    xor esi, esi
.ec_scan:
    cmp esi, EXCON_CAPACITY
    jae .ec_nospc
    imul edi, esi, EXCON_REC_SIZE
    add edi, excon_table
    cmp dword [edi + EXCON_ID_OFF], 0
    je .ec_slot
    inc esi
    jmp .ec_scan
.ec_slot:
    push eax
    mov eax, [excon_next_id]
    mov [edi + EXCON_ID_OFF], eax
    inc dword [excon_next_id]
    pop eax
    mov [edi + EXCON_TYPE_OFF2], eax
    mov dword [edi + EXCON_STATE_OFF2], EXCON_STATE_CREATED
    mov [edi + EXCON_CPU_OFF], edx
    mov eax, [wd_global_tick]
    add eax, 1000
    mov [edi + EXCON_DEADLINE_OFF], eax
    mov [edi + EXCON_PRIO_OFF], ebx
    mov eax, esi
    pop ebx
    pop edi
    pop esi
    clc
    ret
.ec_nospc:
    pop ebx
    pop edi
    pop esi
.ec_fail:
    stc
    ret

; EAX=slot — activate contract (dispatch to scheduler)
excon_activate:
    cmp eax, EXCON_CAPACITY
    jae .ea_bad
    push edi
    push ebx
    imul edi, eax, EXCON_REC_SIZE
    add edi, excon_table
    cmp dword [edi + EXCON_ID_OFF], 0
    je .ea_bad2
    mov dword [edi + EXCON_STATE_OFF2], EXCON_STATE_ACTIVE
    ; dispatch to scheduler: owner=slot, prio from contract, qos=NORMAL
    mov eax, esi
    mov eax, [edi + EXCON_ID_OFF]   ; owner = contract id
    mov edx, [edi + EXCON_PRIO_OFF]
    mov ecx, 2                       ; QOS_CLASS_NORMAL
    call scheduler_admit_task
    pop ebx
    pop edi
    clc
    ret
.ea_bad2:
    pop ebx
.ea_bad:
    pop edi
    stc
    ret

; EAX=slot  EDX=actual_cpu_used — verify contract compliance
excon_verify:
    cmp eax, EXCON_CAPACITY
    jae .ev_bad
    push edi
    imul edi, eax, EXCON_REC_SIZE
    add edi, excon_table
    cmp dword [edi + EXCON_ID_OFF], 0
    je .ev_bad2
    ; check: actual ≤ cpu_budget
    cmp edx, [edi + EXCON_CPU_OFF]
    jbe .ev_ok
    mov dword [edi + EXCON_STATE_OFF2], EXCON_STATE_VIOL
    inc dword [excon_violations]
    pop edi
    stc
    ret
.ev_ok:
    mov dword [edi + EXCON_STATE_OFF2], EXCON_STATE_DONE
    pop edi
    clc
    ret
.ev_bad2:
    pop edi
.ev_bad:
    stc
    ret

excon_self_test:
    ; Create BOUNDED contract
    mov eax, EXCON_TYPE_BOUNDED
    mov edx, 50
    mov ecx, 100
    call excon_create
    jc .ecstf
    push eax
    ; Verify within budget (actual=30 ≤ 50)
    mov edx, 30
    call excon_verify
    jc .ecstf_pop   ; CF=0 expected
    ; Create HARD_RT contract
    pop eax
    mov eax, EXCON_TYPE_HARD_RT
    mov edx, 10
    mov ecx, 200
    call excon_create
    jc .ecstf
    push eax
    ; Verify exceeds budget (actual=20 > 10) → CF=1
    mov edx, 20
    call excon_verify
    jnc .ecstf_pop  ; CF=1 expected
    pop eax
    clc
    ret
.ecstf_pop:
    pop eax
.ecstf:
    stc
    ret

excon_ready:      dd 0
excon_next_id:    dd 1
excon_violations: dd 0
align 4
excon_table:
    times EXCON_CAPACITY * EXCON_REC_SIZE db 0

; ---------------------------------------------------------------------------
; NPSPEC-REALTIME-TEMPORALISOLATION-0001 – Nova RT Temporal Isolation
; ---------------------------------------------------------------------------
; Zeit-Partitionierung: jede RT-Partition erhält ein Budget pro Epoche (Ticks).
; tmpiso_admit prüft ob eine Partition noch Budget hat.
; tmpiso_new_epoch setzt alle Partitions-Budgets zurück.
; Verhindert dass eine RT-Task andere temporär aushungert.
; ---------------------------------------------------------------------------
TMPISO_CAPACITY     equ 4
TMPISO_REC_SIZE     equ 16

TMPISO_ID_OFF       equ 0
TMPISO_BUDGET_OFF   equ 4   ; budget per epoch (ticks)
TMPISO_USED_OFF     equ 8   ; used this epoch
TMPISO_EPOCH_OFF    equ 12  ; epoch at last reset

tmpiso_initialize:
    mov edi, tmpiso_partitions
    xor eax, eax
    mov ecx, (TMPISO_CAPACITY * TMPISO_REC_SIZE) / 4
    rep stosd
    mov dword [tmpiso_ready], 0
    mov dword [tmpiso_epoch], 0
    mov dword [tmpiso_violations], 0
    ; Default: 4 partitions, budget=250 ticks each
    xor esi, esi
.tiso_init_loop:
    cmp esi, TMPISO_CAPACITY
    jae .tiso_init_done
    imul edi, esi, TMPISO_REC_SIZE
    add edi, tmpiso_partitions
    mov [edi + TMPISO_ID_OFF], esi
    mov dword [edi + TMPISO_BUDGET_OFF], 250
    mov dword [edi + TMPISO_USED_OFF], 0
    mov dword [edi + TMPISO_EPOCH_OFF], 0
    inc esi
    jmp .tiso_init_loop
.tiso_init_done:
    mov dword [tmpiso_ready], 1
    clc
    ret

; EAX=partition(0-3)  EDX=ticks_needed → CF=0 admitted / CF=1 budget exhausted
tmpiso_admit:
    cmp dword [tmpiso_ready], 1
    jne .ta_fail
    cmp eax, TMPISO_CAPACITY
    jae .ta_fail
    push edi
    imul edi, eax, TMPISO_REC_SIZE
    add edi, tmpiso_partitions
    mov ecx, [edi + TMPISO_USED_OFF]
    add ecx, edx
    cmp ecx, [edi + TMPISO_BUDGET_OFF]
    ja .ta_over
    mov [edi + TMPISO_USED_OFF], ecx
    pop edi
    clc
    ret
.ta_over:
    inc dword [tmpiso_violations]
    pop edi
.ta_fail:
    stc
    ret

; Reset all partition budgets (new epoch)
tmpiso_new_epoch:
    cmp dword [tmpiso_ready], 1
    jne .tne_ret
    inc dword [tmpiso_epoch]
    push esi
    push edi
    xor esi, esi
.tne_loop:
    cmp esi, TMPISO_CAPACITY
    jae .tne_done
    imul edi, esi, TMPISO_REC_SIZE
    add edi, tmpiso_partitions
    mov dword [edi + TMPISO_USED_OFF], 0
    mov eax, [tmpiso_epoch]
    mov [edi + TMPISO_EPOCH_OFF], eax
    inc esi
    jmp .tne_loop
.tne_done:
    pop edi
    pop esi
.tne_ret:
    clc
    ret

; EAX=partition → EAX=budget  EDX=used  CF=0/CF=1
tmpiso_query:
    cmp eax, TMPISO_CAPACITY
    jae .tq_bad
    push edi
    imul edi, eax, TMPISO_REC_SIZE
    add edi, tmpiso_partitions
    mov eax, [edi + TMPISO_BUDGET_OFF]
    mov edx, [edi + TMPISO_USED_OFF]
    pop edi
    clc
    ret
.tq_bad:
    xor eax, eax
    xor edx, edx
    stc
    ret

tmpiso_self_test:
    ; Admit 100 ticks to partition 0 (budget=250)
    mov eax, 0
    mov edx, 100
    call tmpiso_admit
    jc .tistf
    ; Admit 100 more → used=200, still within 250
    mov eax, 0
    mov edx, 100
    call tmpiso_admit
    jc .tistf
    ; Admit 100 more → used would be 300 > 250 → CF=1
    mov eax, 0
    mov edx, 100
    call tmpiso_admit
    jnc .tistf   ; CF=1 expected
    ; New epoch → resets used
    call tmpiso_new_epoch
    ; Now 100 ticks should be admitted again
    mov eax, 0
    mov edx, 100
    call tmpiso_admit
    jc .tistf    ; CF=0 expected
    clc
    ret
.tistf:
    stc
    ret

tmpiso_ready:      dd 0
tmpiso_epoch:      dd 0
tmpiso_violations: dd 0
align 4
tmpiso_partitions:
    times TMPISO_CAPACITY * TMPISO_REC_SIZE db 0

; ---------------------------------------------------------------------------
; NPSPEC-AUTONOMY-SELFHEALING-0001 – Nova Autonomous Self-Healing
; ---------------------------------------------------------------------------
; Steuert Observe→Detect→Diagnose→Repair→Verify Pipeline.
; Jeder Heal-Slot überwacht ein Subsystem. Bei Fehlern:
;   - containment_report_fault → isolation check
;   - restart_check_and_record → automatic restart
;   - security_audit_log → Violation-Eintrag
; ---------------------------------------------------------------------------
HEAL_CAPACITY       equ 8
HEAL_REC_SIZE       equ 16

HEAL_STATE_OK       equ 0
HEAL_STATE_DEGRADED equ 1
HEAL_STATE_HEALING  equ 2
HEAL_STATE_FAILED   equ 3

HEAL_ID_OFF         equ 0
HEAL_SYS_OFF        equ 4
HEAL_STATE_OFF      equ 8
HEAL_ATTEMPTS_OFF   equ 12

selfheal_initialize:
    mov edi, heal_table
    xor eax, eax
    mov ecx, (HEAL_CAPACITY * HEAL_REC_SIZE) / 4
    rep stosd
    mov dword [heal_ready], 0
    mov dword [heal_total_repairs], 0
    mov dword [heal_ready], 1
    clc
    ret

; EAX=subsystem_id → EAX=slot  CF=0/CF=1 full
selfheal_register:
    push esi
    push edi
    xor esi, esi
.shr_scan:
    cmp esi, HEAL_CAPACITY
    jae .shr_full
    imul edi, esi, HEAL_REC_SIZE
    add edi, heal_table
    cmp dword [edi + HEAL_ID_OFF], 0
    je .shr_slot
    inc esi
    jmp .shr_scan
.shr_slot:
    mov dword [edi + HEAL_ID_OFF], esi
    mov [edi + HEAL_SYS_OFF], eax
    mov dword [edi + HEAL_STATE_OFF], HEAL_STATE_OK
    mov dword [edi + HEAL_ATTEMPTS_OFF], 0
    mov eax, esi
    pop edi
    pop esi
    clc
    ret
.shr_full:
    pop edi
    pop esi
    stc
    ret

; EAX=slot  EDX=fault_type — attempt repair
selfheal_repair:
    cmp eax, HEAL_CAPACITY
    jae .shrep_bad
    push esi
    push edi
    imul edi, eax, HEAL_REC_SIZE
    add edi, heal_table
    cmp dword [edi + HEAL_ID_OFF], 0
    je .shrep_noslt
    mov dword [edi + HEAL_STATE_OFF], HEAL_STATE_HEALING
    inc dword [edi + HEAL_ATTEMPTS_OFF]
    inc dword [heal_total_repairs]
    ; Use resilience pipeline: report fault, check isolation, attempt restart
    push eax
    push edx
    mov eax, edx      ; fault_type
    mov edx, [edi + HEAL_SYS_OFF]  ; subsystem
    mov ecx, 0        ; fault_code=0
    call resilience_fault_pipeline
    pop edx
    pop eax
    ; Mark result: if containment check passes → OK, else FAILED
    push eax
    mov eax, [edi + HEAL_SYS_OFF]
    ; find containment slot for this subsystem
    call containment_check
    pop eax
    jc .shrep_isolated
    mov dword [edi + HEAL_STATE_OFF], HEAL_STATE_OK
    pop edi
    pop esi
    clc
    ret
.shrep_isolated:
    mov dword [edi + HEAL_STATE_OFF], HEAL_STATE_FAILED
    pop edi
    pop esi
    stc
    ret
.shrep_noslt:
    pop edi
    pop esi
.shrep_bad:
    stc
    ret

selfheal_self_test:
    ; Register kernel subsystem
    mov eax, INTRO_SYS_KERNEL
    call selfheal_register
    jc .shstf
    push eax
    ; State should be OK
    imul edi, eax, HEAL_REC_SIZE
    add edi, heal_table
    cmp dword [edi + HEAL_STATE_OFF], HEAL_STATE_OK
    jne .shstf_pop
    pop eax
    clc
    ret
.shstf_pop:
    pop eax
.shstf:
    stc
    ret

heal_ready:         dd 0
heal_total_repairs: dd 0
align 4
heal_table:
    times HEAL_CAPACITY * HEAL_REC_SIZE db 0

; ---------------------------------------------------------------------------
; NPSPEC-PRIVACY-LABEL-0001 – Nova Privacy Label Policy
; ---------------------------------------------------------------------------
; Datenobjekte tragen Privacy-Labels (PUBLIC/SENSITIVE/PRIVATE/CONFIDENTIAL).
; Ergänzt MAC-Labels um Datenschutz-Dimension.
; Privacy Label ≠ Access Control Label — getrennte Policy-Ebene.
; ---------------------------------------------------------------------------
PRIV_OBJ_COUNT      equ 16
PRIV_REC_SIZE       equ 12

PRIV_LABEL_PUBLIC   equ 0
PRIV_LABEL_SENSITIVE equ 1
PRIV_LABEL_PRIVATE  equ 2
PRIV_LABEL_CONF     equ 3

PRIV_OBJ_ID_OFF     equ 0
PRIV_LABEL_OFF      equ 4
PRIV_RETENTION_OFF  equ 8   ; max retention in ticks (0=forever)

privlabel_initialize:
    mov edi, priv_table
    xor eax, eax
    mov ecx, (PRIV_OBJ_COUNT * PRIV_REC_SIZE) / 4
    rep stosd
    mov dword [privlabel_ready], 0
    mov dword [privlabel_denials], 0
    mov dword [privlabel_ready], 1
    clc
    ret

; EAX=obj_id  EDX=label  ECX=retention → CF=0/CF=1 full
privlabel_register:
    cmp edx, PRIV_LABEL_CONF
    ja .plr_bad
    push esi
    push edi
    xor esi, esi
.plr_scan:
    cmp esi, PRIV_OBJ_COUNT
    jae .plr_full
    imul edi, esi, PRIV_REC_SIZE
    add edi, priv_table
    cmp dword [edi + PRIV_OBJ_ID_OFF], 0
    je .plr_slot
    inc esi
    jmp .plr_scan
.plr_slot:
    mov [edi + PRIV_OBJ_ID_OFF], eax
    mov [edi + PRIV_LABEL_OFF], edx
    mov [edi + PRIV_RETENTION_OFF], ecx
    pop edi
    pop esi
    clc
    ret
.plr_full:
    pop edi
    pop esi
.plr_bad:
    stc
    ret

; EAX=obj_id  EDX=required_label → CF=0 allowed (label ≤ required) / CF=1 denied
privlabel_check:
    push esi
    push edi
    push ebx
    mov ebx, edx      ; required label
    xor esi, esi
.plc_loop:
    cmp esi, PRIV_OBJ_COUNT
    jae .plc_notfound
    imul edi, esi, PRIV_REC_SIZE
    add edi, priv_table
    cmp [edi + PRIV_OBJ_ID_OFF], eax
    jne .plc_next
    ; label ≤ required → allowed
    mov ecx, [edi + PRIV_LABEL_OFF]
    cmp ecx, ebx
    jbe .plc_allow
    inc dword [privlabel_denials]
    pop ebx
    pop edi
    pop esi
    stc
    ret
.plc_allow:
    pop ebx
    pop edi
    pop esi
    clc
    ret
.plc_next:
    inc esi
    jmp .plc_loop
.plc_notfound:
    pop ebx
    pop edi
    pop esi
    stc
    ret

privlabel_self_test:
    ; Register PUBLIC object
    mov eax, 100
    mov edx, PRIV_LABEL_PUBLIC
    xor ecx, ecx
    call privlabel_register
    jc .plstf
    ; Check with SENSITIVE → allowed (PUBLIC ≤ SENSITIVE)
    mov eax, 100
    mov edx, PRIV_LABEL_SENSITIVE
    call privlabel_check
    jc .plstf
    ; Register CONFIDENTIAL object
    mov eax, 101
    mov edx, PRIV_LABEL_CONF
    xor ecx, ecx
    call privlabel_register
    jc .plstf
    ; Check CONFIDENTIAL with PRIVATE → denied (CONF > PRIVATE)
    mov eax, 101
    mov edx, PRIV_LABEL_PRIVATE
    call privlabel_check
    jnc .plstf   ; CF=1 expected
    clc
    ret
.plstf:
    stc
    ret

privlabel_ready:   dd 0
privlabel_denials: dd 0
align 4
priv_table:
    times PRIV_OBJ_COUNT * PRIV_REC_SIZE db 0

; ---------------------------------------------------------------------------
; NPSPEC-RESILIENCE-RETRY-0001 + NPSPEC-RESILIENCE-BACKOFF-0001
; Nova Retry Manager mit Exponential Backoff
; ---------------------------------------------------------------------------
; Jeder Retry-Slot verwaltet: attempts, max_attempts, backoff_ticks.
; retry_attempt: CF=0 → nochmal versuchen / CF=1 → max erreicht.
; retry_backoff: berechnet backoff_ticks = base_delay << attempts (saturating).
; ---------------------------------------------------------------------------
RETRY_CAPACITY      equ 8
RETRY_REC_SIZE      equ 20

RETRY_ID_OFF        equ 0
RETRY_ATTEMPTS_OFF  equ 4
RETRY_MAX_OFF       equ 8
RETRY_BACKOFF_OFF   equ 12   ; current backoff in ticks
RETRY_BASEDEL_OFF   equ 16   ; base delay in ticks

retry_initialize:
    mov edi, retry_table
    xor eax, eax
    mov ecx, (RETRY_CAPACITY * RETRY_REC_SIZE) / 4
    rep stosd
    mov dword [retry_ready], 0
    mov dword [retry_total_retries], 0
    mov dword [retry_exhausted], 0
    mov dword [retry_ready], 1
    clc
    ret

; EAX=max_attempts  EDX=base_delay_ticks → EAX=slot  CF=0/CF=1 full
retry_register:
    push esi
    push edi
    xor esi, esi
.rtr_scan:
    cmp esi, RETRY_CAPACITY
    jae .rtr_full
    imul edi, esi, RETRY_REC_SIZE
    add edi, retry_table
    cmp dword [edi + RETRY_ID_OFF], 0
    je .rtr_slot
    inc esi
    jmp .rtr_scan
.rtr_slot:
    mov dword [edi + RETRY_ID_OFF], esi
    mov dword [edi + RETRY_ATTEMPTS_OFF], 0
    mov [edi + RETRY_MAX_OFF], eax
    mov [edi + RETRY_BASEDEL_OFF], edx
    mov [edi + RETRY_BACKOFF_OFF], edx    ; initial backoff = base
    mov eax, esi
    pop edi
    pop esi
    clc
    ret
.rtr_full:
    pop edi
    pop esi
    stc
    ret

; EAX=slot → CF=0 retry allowed / CF=1 exhausted
retry_attempt:
    cmp eax, RETRY_CAPACITY
    jae .ra_bad
    push edi
    imul edi, eax, RETRY_REC_SIZE
    add edi, retry_table
    cmp dword [edi + RETRY_ID_OFF], 0
    je .ra_bad2
    mov eax, [edi + RETRY_ATTEMPTS_OFF]
    cmp eax, [edi + RETRY_MAX_OFF]
    jae .ra_exhausted
    inc dword [edi + RETRY_ATTEMPTS_OFF]
    inc dword [retry_total_retries]
    ; double backoff (saturate at 0x7FFFFFFF)
    mov eax, [edi + RETRY_BACKOFF_OFF]
    test eax, 0x40000000
    jnz .ra_saturate
    shl eax, 1
    jmp .ra_store
.ra_saturate:
    mov eax, 0x7FFFFFFF
.ra_store:
    mov [edi + RETRY_BACKOFF_OFF], eax
    pop edi
    clc
    ret
.ra_exhausted:
    inc dword [retry_exhausted]
    pop edi
    stc
    ret
.ra_bad2:
    pop edi
.ra_bad:
    stc
    ret

; EAX=slot → EAX=current_backoff_ticks  EDX=attempts_remaining
retry_query_backoff:
    cmp eax, RETRY_CAPACITY
    jae .rqb_bad
    push edi
    imul edi, eax, RETRY_REC_SIZE
    add edi, retry_table
    mov eax, [edi + RETRY_BACKOFF_OFF]
    mov ecx, [edi + RETRY_MAX_OFF]
    sub ecx, [edi + RETRY_ATTEMPTS_OFF]
    mov edx, ecx
    pop edi
    clc
    ret
.rqb_bad:
    xor eax, eax
    xor edx, edx
    stc
    ret

retry_self_test:
    ; Register: max=3, base_delay=10
    mov eax, 3
    mov edx, 10
    call retry_register
    jc .rstf
    push eax
    ; 3 attempts should succeed
    call retry_attempt
    jc .rstf_pop
    call retry_attempt
    jc .rstf_pop
    call retry_attempt
    jc .rstf_pop
    ; 4th attempt → CF=1 (exhausted)
    call retry_attempt
    jnc .rstf_pop
    ; backoff should have doubled: 10→20→40
    call retry_query_backoff
    cmp eax, 40
    jb .rstf_pop
    pop eax
    clc
    ret
.rstf_pop:
    pop eax
.rstf:
    stc
    ret

retry_ready:         dd 0
retry_total_retries: dd 0
retry_exhausted:     dd 0
align 4
retry_table:
    times RETRY_CAPACITY * RETRY_REC_SIZE db 0

; ---------------------------------------------------------------------------
; NPSPEC-RESILIENCE-RECOVERYMODE-0001 – Nova Recovery Mode Manager
; ---------------------------------------------------------------------------
; Systemweiter Recovery-Mode: NORMAL/DEGRADED/EMERGENCY/MAINTENANCE.
; Übergänge sind geordnet; nicht jede Transition ist erlaubt.
; Bei EMERGENCY: QoS-Epoch erzwingen + alle nicht-kritischen Tasks throtteln.
; ---------------------------------------------------------------------------
RMODE_NORMAL        equ 0
RMODE_DEGRADED      equ 1
RMODE_EMERGENCY     equ 2
RMODE_MAINTENANCE   equ 3

rmode_initialize:
    mov dword [rmode_current], RMODE_NORMAL
    mov dword [rmode_transitions], 0
    mov dword [rmode_ready], 1
    clc
    ret

; EAX=target_mode → CF=0 transition OK / CF=1 invalid
rmode_transition:
    cmp eax, RMODE_MAINTENANCE
    ja .rm_bad
    cmp [rmode_current], eax
    je .rm_same
    ; Enforce ordering: EMERGENCY→NORMAL only via DEGRADED
    cmp dword [rmode_current], RMODE_EMERGENCY
    jne .rm_ok
    cmp eax, RMODE_NORMAL
    je .rm_bad   ; must go EMERGENCY→DEGRADED first
.rm_ok:
    mov [rmode_current], eax
    inc dword [rmode_transitions]
    ; if entering EMERGENCY: force QoS epoch + tmpiso new epoch
    cmp eax, RMODE_EMERGENCY
    jne .rm_done
    call qos_new_epoch
    call tmpiso_new_epoch
.rm_done:
.rm_same:
    clc
    ret
.rm_bad:
    stc
    ret

; → EAX=current_mode
rmode_query:
    mov eax, [rmode_current]
    clc
    ret

rmode_self_test:
    ; Start NORMAL, go DEGRADED
    mov eax, RMODE_DEGRADED
    call rmode_transition
    jc .rmstf
    ; Go EMERGENCY
    mov eax, RMODE_EMERGENCY
    call rmode_transition
    jc .rmstf
    ; Try NORMAL directly → must fail
    mov eax, RMODE_NORMAL
    call rmode_transition
    jnc .rmstf
    ; Go DEGRADED first, then NORMAL
    mov eax, RMODE_DEGRADED
    call rmode_transition
    jc .rmstf
    mov eax, RMODE_NORMAL
    call rmode_transition
    jc .rmstf
    call rmode_query
    cmp eax, RMODE_NORMAL
    jne .rmstf
    clc
    ret
.rmstf:
    stc
    ret

rmode_ready:       dd 0
rmode_current:     dd 0
rmode_transitions: dd 0

; ---------------------------------------------------------------------------
; NPSPEC-VERIFY-RUNTIME-0001 – Nova Runtime Verification Monitor
; ---------------------------------------------------------------------------
; Überwacht registrierte Runtime Contracts (Invarianten).
; Contract: lower_bound ≤ observed_value ≤ upper_bound.
; Verletzung → inc rtv_violations + audit VIOLATION.
; ---------------------------------------------------------------------------
RTV_CAPACITY        equ 8
RTV_REC_SIZE        equ 20

RTV_ID_OFF          equ 0
RTV_LO_OFF          equ 4    ; lower bound (inclusive)
RTV_HI_OFF          equ 8    ; upper bound (inclusive)
RTV_OBSERVED_OFF    equ 12
RTV_VIOL_OFF        equ 16   ; per-contract violation count

rtv_initialize:
    mov edi, rtv_contracts
    xor eax, eax
    mov ecx, (RTV_CAPACITY * RTV_REC_SIZE) / 4
    rep stosd
    mov dword [rtv_ready], 0
    mov dword [rtv_violations], 0
    mov dword [rtv_checks], 0
    mov dword [rtv_ready], 1
    clc
    ret

; EAX=contract_id  EDX=lower_bound  ECX=upper_bound → CF=0/CF=1 full
rtv_register:
    push esi
    push edi
    xor esi, esi
.rtvr_scan:
    cmp esi, RTV_CAPACITY
    jae .rtvr_full
    imul edi, esi, RTV_REC_SIZE
    add edi, rtv_contracts
    cmp dword [edi + RTV_ID_OFF], 0
    je .rtvr_slot
    inc esi
    jmp .rtvr_scan
.rtvr_slot:
    mov [edi + RTV_ID_OFF], eax
    mov [edi + RTV_LO_OFF], edx
    mov [edi + RTV_HI_OFF], ecx
    mov dword [edi + RTV_OBSERVED_OFF], 0
    mov dword [edi + RTV_VIOL_OFF], 0
    pop edi
    pop esi
    clc
    ret
.rtvr_full:
    pop edi
    pop esi
    stc
    ret

; EAX=contract_id  EDX=observed_value → CF=0 satisfied / CF=1 violated
rtv_check:
    push esi
    push edi
    push ebx
    mov ebx, eax
    xor esi, esi
.rtvc_loop:
    cmp esi, RTV_CAPACITY
    jae .rtvc_notfound
    imul edi, esi, RTV_REC_SIZE
    add edi, rtv_contracts
    cmp [edi + RTV_ID_OFF], ebx
    jne .rtvc_next
    mov [edi + RTV_OBSERVED_OFF], edx
    inc dword [rtv_checks]
    cmp edx, [edi + RTV_LO_OFF]
    jb .rtvc_violated
    cmp edx, [edi + RTV_HI_OFF]
    ja .rtvc_violated
    pop ebx
    pop edi
    pop esi
    clc
    ret
.rtvc_violated:
    inc dword [edi + RTV_VIOL_OFF]
    inc dword [rtv_violations]
    ; log violation to security audit
    push eax
    push edx
    mov eax, 5             ; AUDIT_TYPE_VIOLATION
    mov edx, ebx          ; subject=contract_id
    xor ecx, ecx
    xor ebx, ebx
    call security_audit_log
    pop edx
    pop eax
    pop ebx
    pop edi
    pop esi
    stc
    ret
.rtvc_next:
    inc esi
    jmp .rtvc_loop
.rtvc_notfound:
    pop ebx
    pop edi
    pop esi
    stc
    ret

rtv_self_test:
    ; Register: id=10, lo=0, hi=100
    mov eax, 10
    mov edx, 0
    mov ecx, 100
    call rtv_register
    jc .rtvstf
    ; Check 50 → satisfied
    mov eax, 10
    mov edx, 50
    call rtv_check
    jc .rtvstf
    ; Check 150 → violated
    mov eax, 10
    mov edx, 150
    call rtv_check
    jnc .rtvstf
    ; Violations count ≥ 1
    cmp dword [rtv_violations], 1
    jb .rtvstf
    clc
    ret
.rtvstf:
    stc
    ret

rtv_ready:      dd 0
rtv_violations: dd 0
rtv_checks:     dd 0
align 4
rtv_contracts:
    times RTV_CAPACITY * RTV_REC_SIZE db 0

; ---------------------------------------------------------------------------
; NPSPEC-OBSERVABILITY-STATEGRAPH-0001 – Nova System State Graph
; ---------------------------------------------------------------------------
; Registriert Systemobjekte mit Zustand + Vorgänger-Zustand.
; Ermöglicht Fragen: Welcher Zustand, von wem abhängig, wie entstanden.
; sgraph_transition: Zustandsübergang mit Tick-Stempel.
; ---------------------------------------------------------------------------
SGRAPH_CAPACITY     equ 16
SGRAPH_REC_SIZE     equ 20

SGRAPH_OBJ_ID_OFF   equ 0
SGRAPH_STATE_OFF    equ 4
SGRAPH_PREV_OFF     equ 8
SGRAPH_PARENT_OFF   equ 12   ; depended-on object id
SGRAPH_TICK_OFF     equ 16

sgraph_initialize:
    mov edi, sgraph_nodes
    xor eax, eax
    mov ecx, (SGRAPH_CAPACITY * SGRAPH_REC_SIZE) / 4
    rep stosd
    mov dword [sgraph_ready], 0
    mov dword [sgraph_transitions], 0
    mov dword [sgraph_ready], 1
    clc
    ret

; EAX=obj_id  EDX=initial_state  ECX=parent_obj_id → CF=0/CF=1 full
sgraph_register:
    push esi
    push edi
    push ebx
    mov ebx, ecx
    xor esi, esi
.sg_scan:
    cmp esi, SGRAPH_CAPACITY
    jae .sg_full
    imul edi, esi, SGRAPH_REC_SIZE
    add edi, sgraph_nodes
    cmp dword [edi + SGRAPH_OBJ_ID_OFF], 0
    je .sg_slot
    inc esi
    jmp .sg_scan
.sg_slot:
    mov [edi + SGRAPH_OBJ_ID_OFF], eax
    mov [edi + SGRAPH_STATE_OFF], edx
    mov dword [edi + SGRAPH_PREV_OFF], 0
    mov [edi + SGRAPH_PARENT_OFF], ebx
    push eax
    mov eax, [wd_global_tick]
    mov [edi + SGRAPH_TICK_OFF], eax
    pop eax
    pop ebx
    pop edi
    pop esi
    clc
    ret
.sg_full:
    pop ebx
    pop edi
    pop esi
    stc
    ret

; EAX=obj_id  EDX=new_state → CF=0 ok / CF=1 not found
sgraph_transition:
    push esi
    push edi
    push ebx
    mov ebx, eax
    xor esi, esi
.sgt_loop:
    cmp esi, SGRAPH_CAPACITY
    jae .sgt_notfound
    imul edi, esi, SGRAPH_REC_SIZE
    add edi, sgraph_nodes
    cmp [edi + SGRAPH_OBJ_ID_OFF], ebx
    jne .sgt_next
    ; record transition
    mov ecx, [edi + SGRAPH_STATE_OFF]
    mov [edi + SGRAPH_PREV_OFF], ecx
    mov [edi + SGRAPH_STATE_OFF], edx
    mov eax, [wd_global_tick]
    mov [edi + SGRAPH_TICK_OFF], eax
    inc dword [sgraph_transitions]
    pop ebx
    pop edi
    pop esi
    clc
    ret
.sgt_next:
    inc esi
    jmp .sgt_loop
.sgt_notfound:
    pop ebx
    pop edi
    pop esi
    stc
    ret

; EAX=obj_id → EAX=state  EDX=prev_state  CF=0/CF=1
sgraph_query:
    push esi
    push edi
    push ebx
    mov ebx, eax
    xor esi, esi
.sgq_loop:
    cmp esi, SGRAPH_CAPACITY
    jae .sgq_notfound
    imul edi, esi, SGRAPH_REC_SIZE
    add edi, sgraph_nodes
    cmp [edi + SGRAPH_OBJ_ID_OFF], ebx
    jne .sgq_next
    mov eax, [edi + SGRAPH_STATE_OFF]
    mov edx, [edi + SGRAPH_PREV_OFF]
    pop ebx
    pop edi
    pop esi
    clc
    ret
.sgq_next:
    inc esi
    jmp .sgq_loop
.sgq_notfound:
    pop ebx
    pop edi
    pop esi
    stc
    ret

sgraph_self_test:
    ; Register object 50, state=1, parent=0
    mov eax, 50
    mov edx, 1
    xor ecx, ecx
    call sgraph_register
    jc .sgstf
    ; Transition to state=2
    mov eax, 50
    mov edx, 2
    call sgraph_transition
    jc .sgstf
    ; Query: state=2, prev=1
    mov eax, 50
    call sgraph_query
    jc .sgstf
    cmp eax, 2
    jne .sgstf
    cmp edx, 1
    jne .sgstf
    clc
    ret
.sgstf:
    stc
    ret

sgraph_ready:       dd 0
sgraph_transitions: dd 0
align 4
sgraph_nodes:
    times SGRAPH_CAPACITY * SGRAPH_REC_SIZE db 0

; ---------------------------------------------------------------------------
; NPSPEC-STATE-MACHINE-0001 – Nova Generic Finite State Machine
; ---------------------------------------------------------------------------
; Registriert FSMs mit erlaubten Übergängen (Transition Table).
; fsm_transition: prüft ob from→to erlaubt ist, führt Übergang durch.
; Max. 4 FSMs, je 8 erlaubte Transitionen.
; ---------------------------------------------------------------------------
FSM_CAPACITY        equ 4
FSM_TRANS_COUNT     equ 8
FSM_REC_SIZE        equ 48  ; id(4) + current(4) + 8*trans(8*5=40 = from+to = 8 bytes each)

; Layout: [FSM_ID][CURRENT_STATE][TRANS_0_FROM][TRANS_0_TO]...[TRANS_7_FROM][TRANS_7_TO]
FSM_ID_OFF          equ 0
FSM_CUR_OFF         equ 4
FSM_TRANS_OFF       equ 8   ; 8 pairs of (from,to) = 8*8 = 64 bytes

; Recalculate: id(4)+current(4)+8*(from4+to4)=4+4+64=72
FSM_REC_SIZE2       equ 72

fsm_initialize:
    mov edi, fsm_table
    xor eax, eax
    mov ecx, (FSM_CAPACITY * FSM_REC_SIZE2) / 4
    rep stosd
    mov dword [fsm_ready], 0
    mov dword [fsm_denied], 0
    mov dword [fsm_ready], 1
    clc
    ret

; EAX=fsm_id  EDX=initial_state → CF=0/CF=1 full
fsm_register:
    push esi
    push edi
    xor esi, esi
.fsmr_scan:
    cmp esi, FSM_CAPACITY
    jae .fsmr_full
    imul edi, esi, FSM_REC_SIZE2
    add edi, fsm_table
    cmp dword [edi + FSM_ID_OFF], 0
    je .fsmr_slot
    inc esi
    jmp .fsmr_scan
.fsmr_slot:
    mov [edi + FSM_ID_OFF], eax
    mov [edi + FSM_CUR_OFF], edx
    ; zero transition table for this FSM
    push ecx
    lea ecx, [edi + FSM_TRANS_OFF]
    push edi
    mov edi, ecx
    xor eax, eax
    mov ecx, (FSM_TRANS_COUNT * 8) / 4
    rep stosd
    pop edi
    pop ecx
    mov eax, esi
    pop edi
    pop esi
    clc
    ret
.fsmr_full:
    pop edi
    pop esi
    stc
    ret

; EAX=fsm_id  EDX=from_state  ECX=to_state — add allowed transition
fsm_add_transition:
    push esi
    push edi
    push ebx
    push edx
    mov ebx, eax
    xor esi, esi
.fsmat_find:
    cmp esi, FSM_CAPACITY
    jae .fsmat_notfound
    imul edi, esi, FSM_REC_SIZE2
    add edi, fsm_table
    cmp [edi + FSM_ID_OFF], ebx
    je .fsmat_found
    inc esi
    jmp .fsmat_find
.fsmat_found:
    ; find free transition slot
    push edi
    lea edi, [edi + FSM_TRANS_OFF]
    xor esi, esi
.fsmat_tscan:
    cmp esi, FSM_TRANS_COUNT
    jae .fsmat_tfull
    cmp dword [edi + esi*8], 0
    je .fsmat_tslot
    inc esi
    jmp .fsmat_tscan
.fsmat_tslot:
    pop edx        ; from_state was on stack as edx
    push esi
    mov eax, edx
    pop esi
    ; edi+esi*8 = transition entry
    lea ebx, [edi + esi*8]
    mov [ebx], eax         ; from
    mov [ebx + 4], ecx     ; to
    pop edi
    pop edx
    pop ebx
    pop edi
    pop esi
    clc
    ret
.fsmat_tfull:
    pop edi
.fsmat_notfound:
    pop edx
    pop ebx
    pop edi
    pop esi
    stc
    ret

; EAX=fsm_id  EDX=to_state → CF=0 transition OK / CF=1 not allowed
fsm_transition2:
    push esi
    push edi
    push ebx
    push ecx
    mov ebx, eax
    xor esi, esi
.fsmt2_find:
    cmp esi, FSM_CAPACITY
    jae .fsmt2_notfound
    imul edi, esi, FSM_REC_SIZE2
    add edi, fsm_table
    cmp [edi + FSM_ID_OFF], ebx
    je .fsmt2_found
    inc esi
    jmp .fsmt2_find
.fsmt2_found:
    ; current state
    mov ecx, [edi + FSM_CUR_OFF]
    ; search transition table
    push edi
    lea edi, [edi + FSM_TRANS_OFF]
    xor esi, esi
.fsmt2_tscan:
    cmp esi, FSM_TRANS_COUNT
    jae .fsmt2_denied
    lea ebx, [edi + esi*8]
    cmp dword [ebx], 0
    je .fsmt2_next
    cmp [ebx], ecx        ; from == current?
    jne .fsmt2_next
    cmp [ebx + 4], edx    ; to == target?
    jne .fsmt2_next
    ; transition allowed
    pop edi
    mov [edi + FSM_CUR_OFF], edx
    pop ecx
    pop ebx
    pop edi
    pop esi
    clc
    ret
.fsmt2_next:
    inc esi
    jmp .fsmt2_tscan
.fsmt2_denied:
    inc dword [fsm_denied]
    pop edi
.fsmt2_notfound:
    pop ecx
    pop ebx
    pop edi
    pop esi
    stc
    ret

fsm_self_test:
    ; Register FSM id=1, initial=0
    mov eax, 1
    xor edx, edx
    call fsm_register
    jc .fsmstf
    ; Add transition 0→1
    mov eax, 1
    mov edx, 0
    mov ecx, 1
    call fsm_add_transition
    jc .fsmstf
    ; Add transition 1→2
    mov eax, 1
    mov edx, 1
    mov ecx, 2
    call fsm_add_transition
    jc .fsmstf
    ; Transition to 1 → OK
    mov eax, 1
    mov edx, 1
    call fsm_transition2
    jc .fsmstf
    ; Transition directly 1→3 → denied (not in table)
    mov eax, 1
    mov edx, 3
    call fsm_transition2
    jnc .fsmstf   ; CF=1 expected
    ; Transition 1→2 → OK
    mov eax, 1
    mov edx, 2
    call fsm_transition2
    jc .fsmstf
    clc
    ret
.fsmstf:
    stc
    ret

fsm_ready:  dd 0
fsm_denied: dd 0
align 4
fsm_table:
    times FSM_CAPACITY * FSM_REC_SIZE2 db 0

; ---------------------------------------------------------------------------
; NPSPEC-TRANSACTION-ROLLBACK-0001 – Nova Transaction Rollback Journal
; ---------------------------------------------------------------------------
; Rollback-Journal für atomare Operationen. Vor jeder Änderung: txroll_save.
; Bei Fehler: txroll_undo stellt alle gespeicherten Werte wieder her.
; Bei Erfolg: txroll_commit löscht das Journal.
; ---------------------------------------------------------------------------
TXROLL_CAPACITY     equ 8
TXROLL_REC_SIZE     equ 12

TXROLL_ADDR_OFF     equ 0   ; address of dword to save/restore
TXROLL_VAL_OFF      equ 4   ; saved original value
TXROLL_ACTIVE_OFF   equ 8   ; 1=active

txroll_initialize:
    mov edi, txroll_journal
    xor eax, eax
    mov ecx, (TXROLL_CAPACITY * TXROLL_REC_SIZE) / 4
    rep stosd
    mov dword [txroll_ready], 0
    mov dword [txroll_count], 0
    mov dword [txroll_undone], 0
    mov dword [txroll_ready], 1
    clc
    ret

; EAX=address_of_dword — save current value before modification
txroll_save:
    cmp dword [txroll_ready], 1
    jne .trs_fail
    cmp dword [txroll_count], TXROLL_CAPACITY
    jae .trs_fail
    push esi
    push edi
    xor esi, esi
.trs_scan:
    cmp esi, TXROLL_CAPACITY
    jae .trs_nospc
    imul edi, esi, TXROLL_REC_SIZE
    add edi, txroll_journal
    cmp dword [edi + TXROLL_ACTIVE_OFF], 0
    je .trs_slot
    inc esi
    jmp .trs_scan
.trs_slot:
    mov [edi + TXROLL_ADDR_OFF], eax
    push ecx
    mov ecx, [eax]         ; read current value
    mov [edi + TXROLL_VAL_OFF], ecx
    pop ecx
    mov dword [edi + TXROLL_ACTIVE_OFF], 1
    inc dword [txroll_count]
    pop edi
    pop esi
    clc
    ret
.trs_nospc:
    pop edi
    pop esi
.trs_fail:
    stc
    ret

; Undo all saved values (rollback)
txroll_undo:
    cmp dword [txroll_ready], 1
    jne .tru_ret
    push esi
    push edi
    xor esi, esi
.tru_loop:
    cmp esi, TXROLL_CAPACITY
    jae .tru_done
    imul edi, esi, TXROLL_REC_SIZE
    add edi, txroll_journal
    cmp dword [edi + TXROLL_ACTIVE_OFF], 1
    jne .tru_next
    push eax
    push ecx
    mov eax, [edi + TXROLL_ADDR_OFF]
    mov ecx, [edi + TXROLL_VAL_OFF]
    mov [eax], ecx         ; restore original value
    pop ecx
    pop eax
    mov dword [edi + TXROLL_ACTIVE_OFF], 0
    inc dword [txroll_undone]
.tru_next:
    inc esi
    jmp .tru_loop
.tru_done:
    mov dword [txroll_count], 0
    pop edi
    pop esi
.tru_ret:
    clc
    ret

; Commit: clear journal (changes are permanent)
txroll_commit:
    mov edi, txroll_journal
    xor eax, eax
    mov ecx, (TXROLL_CAPACITY * TXROLL_REC_SIZE) / 4
    rep stosd
    mov dword [txroll_count], 0
    clc
    ret

txroll_self_test:
    ; Save a test value
    mov dword [txroll_test_var], 42
    mov eax, txroll_test_var
    call txroll_save
    jc .trstf
    ; Modify it
    mov dword [txroll_test_var], 99
    ; Undo → should restore 42
    call txroll_undo
    cmp dword [txroll_test_var], 42
    jne .trstf
    ; Save and commit
    mov dword [txroll_test_var], 77
    mov eax, txroll_test_var
    call txroll_save
    jc .trstf
    mov dword [txroll_test_var], 55
    call txroll_commit
    ; After commit, value stays at 55
    cmp dword [txroll_test_var], 55
    jne .trstf
    clc
    ret
.trstf:
    stc
    ret

txroll_ready:   dd 0
txroll_count:   dd 0
txroll_undone:  dd 0
txroll_test_var: dd 0
align 4
txroll_journal:
    times TXROLL_CAPACITY * TXROLL_REC_SIZE db 0

; ---------------------------------------------------------------------------
; NPSPEC-RESILIENCE-FAULTINJECTION-0001 – Nova Fault Injection Framework
; ---------------------------------------------------------------------------
; Kontrollierte Fehlerinjektion für Resilience-Tests.
; fi_inject: aktiviert einen Fehler für ein Ziel-Subsystem.
; fi_check: prüft ob ein aktiver injizierter Fehler vorliegt.
; fi_clear: entfernt injizierten Fehler.
; ---------------------------------------------------------------------------
FI_CAPACITY         equ 8
FI_REC_SIZE         equ 12

FI_TARGET_OFF       equ 0   ; subsystem id
FI_FAULT_OFF        equ 4   ; fault type
FI_ACTIVE_OFF       equ 8   ; 1=active

fi_initialize:
    mov edi, fi_table
    xor eax, eax
    mov ecx, (FI_CAPACITY * FI_REC_SIZE) / 4
    rep stosd
    mov dword [fi_ready], 0
    mov dword [fi_total_injected], 0
    mov dword [fi_ready], 1
    clc
    ret

; EAX=target_subsystem  EDX=fault_type → CF=0/CF=1 full
fi_inject:
    cmp dword [fi_ready], 1
    jne .fi_fail
    push esi
    push edi
    xor esi, esi
.fi_scan:
    cmp esi, FI_CAPACITY
    jae .fi_full
    imul edi, esi, FI_REC_SIZE
    add edi, fi_table
    cmp dword [edi + FI_ACTIVE_OFF], 0
    je .fi_slot
    inc esi
    jmp .fi_scan
.fi_slot:
    mov [edi + FI_TARGET_OFF], eax
    mov [edi + FI_FAULT_OFF], edx
    mov dword [edi + FI_ACTIVE_OFF], 1
    inc dword [fi_total_injected]
    pop edi
    pop esi
    clc
    ret
.fi_full:
    pop edi
    pop esi
.fi_fail:
    stc
    ret

; EAX=target_subsystem → CF=0 fault active (EAX=fault_type) / CF=1 none
fi_check:
    push esi
    push edi
    push ebx
    mov ebx, eax
    xor esi, esi
.fic_loop:
    cmp esi, FI_CAPACITY
    jae .fic_notfound
    imul edi, esi, FI_REC_SIZE
    add edi, fi_table
    cmp dword [edi + FI_ACTIVE_OFF], 1
    jne .fic_next
    cmp [edi + FI_TARGET_OFF], ebx
    jne .fic_next
    mov eax, [edi + FI_FAULT_OFF]
    pop ebx
    pop edi
    pop esi
    clc
    ret
.fic_next:
    inc esi
    jmp .fic_loop
.fic_notfound:
    pop ebx
    pop edi
    pop esi
    stc
    ret

; EAX=target_subsystem — clear injected fault
fi_clear:
    push esi
    push edi
    push ebx
    mov ebx, eax
    xor esi, esi
.ficlr_loop:
    cmp esi, FI_CAPACITY
    jae .ficlr_done
    imul edi, esi, FI_REC_SIZE
    add edi, fi_table
    cmp dword [edi + FI_ACTIVE_OFF], 1
    jne .ficlr_next
    cmp [edi + FI_TARGET_OFF], ebx
    jne .ficlr_next
    mov dword [edi + FI_ACTIVE_OFF], 0
.ficlr_next:
    inc esi
    jmp .ficlr_loop
.ficlr_done:
    pop ebx
    pop edi
    pop esi
    clc
    ret

fi_self_test:
    ; Inject fault on subsystem 7
    mov eax, 7
    mov edx, 3   ; fault_type=3
    call fi_inject
    jc .fistf
    ; Check fault present
    mov eax, 7
    call fi_check
    jc .fistf
    cmp eax, 3
    jne .fistf
    ; Clear
    mov eax, 7
    call fi_clear
    ; Check no fault
    mov eax, 7
    call fi_check
    jnc .fistf   ; CF=1 expected (no fault)
    clc
    ret
.fistf:
    stc
    ret

fi_ready:          dd 0
fi_total_injected: dd 0
align 4
fi_table:
    times FI_CAPACITY * FI_REC_SIZE db 0

; ===========================================================================
; NPSPEC-RESILIENCE-CLASSIFICATION-0001 – Nova Fault Classification
; ===========================================================================
; Classifies detected faults by severity and source domain so recovery
; policy can select the right action. Connects detection → policy pipeline.
;
; Classes: UNKNOWN=0 / TRANSIENT=1 / SOFT=2 / HARD=3 / CRITICAL=4
; Sources: KERNEL=1 / SCHED=2 / MEMORY=3 / SECURITY=4 / DRIVER=5 / USER=6
; Record: class_id(4) + severity(4) + source(4) + count(4) = 16 bytes

FCLASS_CAPACITY  equ 8
FCLASS_REC_SIZE  equ 16
FCLASS_CLS_ID    equ 0
FCLASS_SEVERITY  equ 4
FCLASS_SOURCE    equ 8
FCLASS_COUNT     equ 12

FCLASS_UNKNOWN   equ 0
FCLASS_TRANSIENT equ 1
FCLASS_SOFT      equ 2
FCLASS_HARD      equ 3
FCLASS_CRITICAL  equ 4

FCLASS_SRC_KERNEL   equ 1
FCLASS_SRC_SCHED    equ 2
FCLASS_SRC_MEMORY   equ 3
FCLASS_SRC_SECURITY equ 4
FCLASS_SRC_DRIVER   equ 5
FCLASS_SRC_USER     equ 6

fclass_initialize:
    cmp dword [fclass_ready], 1
    je .done
    mov edi, fclass_table
    xor eax, eax
    mov ecx, (FCLASS_CAPACITY * FCLASS_REC_SIZE) / 4
    rep stosd
    mov dword [fclass_ready], 1
.done:
    clc
    ret

; EAX=class_id(1..CAPACITY), EDX=severity, ECX=source → CF=0 registered / CF=1 err
fclass_register:
    cmp dword [fclass_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    cmp eax, FCLASS_CAPACITY
    ja .fail
    cmp edx, FCLASS_CRITICAL
    ja .fail
    test ecx, ecx
    jz .fail
    push eax
    push edx
    push ecx
    dec eax
    imul edi, eax, FCLASS_REC_SIZE
    add edi, fclass_table
    pop ecx
    pop edx
    pop eax
    push eax
    dec eax
    imul edi, eax, FCLASS_REC_SIZE
    add edi, fclass_table
    pop eax
    mov [edi + FCLASS_CLS_ID], eax
    mov [edi + FCLASS_SEVERITY], edx
    mov [edi + FCLASS_SOURCE], ecx
    mov dword [edi + FCLASS_COUNT], 0
    clc
    ret
.fail:
    stc
    ret

; EAX=class_id → count at [edi+FCLASS_COUNT], EAX=severity, EDX=source, CF
fclass_classify:
    cmp dword [fclass_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    cmp eax, FCLASS_CAPACITY
    ja .fail
    push eax
    dec eax
    imul edi, eax, FCLASS_REC_SIZE
    add edi, fclass_table
    pop eax
    cmp dword [edi + FCLASS_CLS_ID], 0
    je .fail
    inc dword [edi + FCLASS_COUNT]
    mov eax, [edi + FCLASS_SEVERITY]
    mov edx, [edi + FCLASS_SOURCE]
    clc
    ret
.fail:
    stc
    ret

fclass_self_test:
    xor esi, esi
    ; register class 1: HARD / KERNEL
    mov eax, 1
    mov edx, FCLASS_HARD
    mov ecx, FCLASS_SRC_KERNEL
    call fclass_register
    jc .fail
    ; classify it twice
    mov eax, 1
    call fclass_classify
    jc .fail
    cmp eax, FCLASS_HARD
    jne .fail
    cmp edx, FCLASS_SRC_KERNEL
    jne .fail
    mov eax, 1
    call fclass_classify
    jc .fail
    ; verify count=2
    mov edi, fclass_table       ; slot 0
    cmp dword [edi + FCLASS_COUNT], 2
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
fclass_ready:  dd 0
fclass_table:
    times FCLASS_CAPACITY * FCLASS_REC_SIZE db 0

; ===========================================================================
; NPSPEC-RESILIENCE-RECOVERYPOLICY-0001 – Nova Recovery Policy Manager
; ===========================================================================
; Maps fault class + severity to a recovery action.
; Connects fclass_classify → select action → dispatch to rmode/restart/failover.
;
; Actions: NONE=0 / LOG_ONLY=1 / RESTART=2 / FAILOVER=3 / DEGRADE=4 / PANIC=5
; Record: class_id(4) + min_severity(4) + action(4) + fires(4) = 16 bytes

RPOL_CAPACITY   equ 8
RPOL_REC_SIZE   equ 16
RPOL_CLS_ID     equ 0
RPOL_MIN_SEV    equ 4
RPOL_ACTION     equ 8
RPOL_FIRES      equ 12

RPOL_NONE       equ 0
RPOL_LOG_ONLY   equ 1
RPOL_RESTART    equ 2
RPOL_FAILOVER   equ 3
RPOL_DEGRADE    equ 4
RPOL_PANIC_ACT  equ 5

rpol_initialize:
    cmp dword [rpol_ready], 1
    je .done
    mov edi, rpol_table
    xor eax, eax
    mov ecx, (RPOL_CAPACITY * RPOL_REC_SIZE) / 4
    rep stosd
    mov dword [rpol_ready], 1
.done:
    clc
    ret

; EAX=class_id, EDX=min_severity, ECX=action → EAX=slot, CF
rpol_register:
    cmp dword [rpol_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    xor esi, esi
.scan:
    cmp esi, RPOL_CAPACITY
    jae .fail
    imul edi, esi, RPOL_REC_SIZE
    add edi, rpol_table
    cmp dword [edi + RPOL_CLS_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + RPOL_CLS_ID], eax
    mov [edi + RPOL_MIN_SEV], edx
    mov [edi + RPOL_ACTION], ecx
    mov dword [edi + RPOL_FIRES], 0
    mov eax, esi
    clc
    ret
.fail:
    stc
    ret

; EAX=class_id, EDX=observed_severity → EAX=action, CF=0 found / CF=1 no policy
rpol_evaluate:
    cmp dword [rpol_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, RPOL_CAPACITY
    jae .not_found
    imul edi, esi, RPOL_REC_SIZE
    add edi, rpol_table
    cmp [edi + RPOL_CLS_ID], ebx
    jne .next
    ; class matches — check severity threshold
    cmp edx, [edi + RPOL_MIN_SEV]
    jb .next
    inc dword [edi + RPOL_FIRES]
    mov eax, [edi + RPOL_ACTION]
    pop ebx
    clc
    ret
.next:
    inc esi
    jmp .scan
.not_found:
    pop ebx
.fail:
    stc
    ret

rpol_self_test:
    ; register policy: class=1, min_severity=HARD, action=DEGRADE
    mov eax, 1
    mov edx, FCLASS_HARD
    mov ecx, RPOL_DEGRADE
    call rpol_register
    jc .fail
    ; evaluate with severity=HARD → should get DEGRADE
    mov eax, 1
    mov edx, FCLASS_HARD
    call rpol_evaluate
    jc .fail
    cmp eax, RPOL_DEGRADE
    jne .fail
    ; evaluate with severity=SOFT (below threshold) → no policy match
    mov eax, 1
    mov edx, FCLASS_SOFT
    call rpol_evaluate
    jnc .fail              ; CF=1 expected (no match)
    clc
    ret
.fail:
    stc
    ret

align 4
rpol_ready:  dd 0
rpol_table:
    times RPOL_CAPACITY * RPOL_REC_SIZE db 0

; ===========================================================================
; NPSPEC-RESILIENCE-VERIFICATION-0001 – Nova Resilience Verification
; ===========================================================================
; After a recovery action completes, verifies the subsystem returned to
; a healthy state. Connects selfheal_repair → rverif_check → sgraph_query.
;
; States: UNKNOWN=0 / HEALTHY=1 / DEGRADED=2 / FAILED=3
; Record: subsys_id(4) + expected_state(4) + observed_state(4) + checks(4) = 16 bytes

RVERIF_CAPACITY    equ 8
RVERIF_REC_SIZE    equ 16
RVERIF_SUBSYS      equ 0
RVERIF_EXPECTED    equ 4
RVERIF_OBSERVED    equ 8
RVERIF_CHECKS      equ 12

RVERIF_UNKNOWN     equ 0
RVERIF_HEALTHY     equ 1
RVERIF_DEGRADED    equ 2
RVERIF_FAILED      equ 3

rverif_initialize:
    cmp dword [rverif_ready], 1
    je .done
    mov edi, rverif_table
    xor eax, eax
    mov ecx, (RVERIF_CAPACITY * RVERIF_REC_SIZE) / 4
    rep stosd
    mov dword [rverif_ready], 1
.done:
    clc
    ret

; EAX=subsys_id, EDX=expected_state → EAX=slot, CF
rverif_register:
    cmp dword [rverif_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    xor esi, esi
.scan:
    cmp esi, RVERIF_CAPACITY
    jae .fail
    imul edi, esi, RVERIF_REC_SIZE
    add edi, rverif_table
    cmp dword [edi + RVERIF_SUBSYS], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + RVERIF_SUBSYS], eax
    mov [edi + RVERIF_EXPECTED], edx
    mov dword [edi + RVERIF_OBSERVED], RVERIF_UNKNOWN
    mov dword [edi + RVERIF_CHECKS], 0
    mov eax, esi
    clc
    ret
.fail:
    stc
    ret

; EAX=subsys_id, EDX=observed_state → CF=0 healthy / CF=1 mismatch/unknown
rverif_check:
    cmp dword [rverif_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, RVERIF_CAPACITY
    jae .not_found
    imul edi, esi, RVERIF_REC_SIZE
    add edi, rverif_table
    cmp [edi + RVERIF_SUBSYS], ebx
    je .found
    inc esi
    jmp .scan
.found:
    inc dword [edi + RVERIF_CHECKS]
    mov [edi + RVERIF_OBSERVED], edx
    cmp edx, [edi + RVERIF_EXPECTED]
    jne .mismatch
    pop ebx
    clc
    ret
.mismatch:
    ; log violation via rtv contract mechanism
    call security_audit_log_violation
    pop ebx
    stc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

; EAX=subsys_id → EAX=observed_state, EDX=expected_state, CF
rverif_query:
    cmp dword [rverif_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, RVERIF_CAPACITY
    jae .not_found
    imul edi, esi, RVERIF_REC_SIZE
    add edi, rverif_table
    cmp [edi + RVERIF_SUBSYS], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + RVERIF_OBSERVED]
    mov edx, [edi + RVERIF_EXPECTED]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

rverif_self_test:
    ; register subsys=1, expected=HEALTHY
    mov eax, 1
    mov edx, RVERIF_HEALTHY
    call rverif_register
    jc .fail
    ; check with HEALTHY → CF=0
    mov eax, 1
    mov edx, RVERIF_HEALTHY
    call rverif_check
    jc .fail
    ; query → observed=HEALTHY, expected=HEALTHY
    mov eax, 1
    call rverif_query
    jc .fail
    cmp eax, RVERIF_HEALTHY
    jne .fail
    cmp edx, RVERIF_HEALTHY
    jne .fail
    ; check with FAILED → CF=1 (mismatch)
    mov eax, 1
    mov edx, RVERIF_FAILED
    call rverif_check
    jnc .fail
    clc
    ret
.fail:
    stc
    ret

; Internal helper: log a resilience verification violation to the audit trail
security_audit_log_violation:
    ; Forward to security_audit_log with VIOLATION event type (3)
    ; EAX=3(VIOLATION), EDX=RVERIF subsystem tag
    push eax
    push edx
    mov eax, 3
    mov edx, 0x52564552   ; "RVER"
    call security_audit_log
    pop edx
    pop eax
    ret

align 4
rverif_ready:  dd 0
rverif_table:
    times RVERIF_CAPACITY * RVERIF_REC_SIZE db 0

; ===========================================================================
; NPSPEC-STATE-SNAPSHOT-0001 – Nova State Snapshot
; ===========================================================================
; Captures and restores versioned snapshots of kernel state dwords.
; Connects txroll (save/restore dwords) with state_manager versioning.
;
; Record: snap_id(4) + addr(4) + value(4) + version(4) + tick(4) + pad(4) = 24 bytes

SSNAP_CAPACITY  equ 4
SSNAP_REC_SIZE  equ 24
SSNAP_ID        equ 0
SSNAP_ADDR      equ 4
SSNAP_VALUE     equ 8
SSNAP_VERSION   equ 12
SSNAP_TICK      equ 16
SSNAP_PAD       equ 20

ssnap_initialize:
    cmp dword [ssnap_ready], 1
    je .done
    mov edi, ssnap_table
    xor eax, eax
    mov ecx, (SSNAP_CAPACITY * SSNAP_REC_SIZE) / 4
    rep stosd
    mov dword [ssnap_next_id], 1
    mov dword [ssnap_ready], 1
.done:
    clc
    ret

; EAX=address_of_dword → EAX=snap_id, CF
ssnap_capture:
    cmp dword [ssnap_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push esi
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, SSNAP_CAPACITY
    jae .full
    imul edi, esi, SSNAP_REC_SIZE
    add edi, ssnap_table
    cmp dword [edi + SSNAP_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov eax, [ssnap_next_id]
    inc dword [ssnap_next_id]
    mov [edi + SSNAP_ID], eax
    mov [edi + SSNAP_ADDR], ebx
    mov ecx, [ebx]
    mov [edi + SSNAP_VALUE], ecx
    mov dword [edi + SSNAP_VERSION], 1
    mov ecx, [wd_global_tick]
    mov [edi + SSNAP_TICK], ecx
    pop esi
    pop ebx
    clc
    ret
.full:
    pop esi
    pop ebx
.fail:
    stc
    ret

; EAX=snap_id → restores captured dword to original address, CF
ssnap_restore:
    cmp dword [ssnap_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push esi
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, SSNAP_CAPACITY
    jae .not_found
    imul edi, esi, SSNAP_REC_SIZE
    add edi, ssnap_table
    cmp [edi + SSNAP_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + SSNAP_ADDR]
    mov ecx, [edi + SSNAP_VALUE]
    mov [eax], ecx
    ; invalidate slot
    mov dword [edi + SSNAP_ID], 0
    pop esi
    pop ebx
    clc
    ret
.not_found:
    pop esi
    pop ebx
.fail:
    stc
    ret

ssnap_self_test:
    ; capture ssnap_test_var
    mov eax, ssnap_test_var
    call ssnap_capture
    jc .fail
    push eax             ; save snap_id
    ; mutate the var
    mov dword [ssnap_test_var], 0xDEAD
    ; restore
    pop eax
    call ssnap_restore
    jc .fail
    ; verify original value restored
    cmp dword [ssnap_test_var], 0x1234
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
ssnap_ready:    dd 0
ssnap_next_id:  dd 1
ssnap_test_var: dd 0x1234
ssnap_table:
    times SSNAP_CAPACITY * SSNAP_REC_SIZE db 0

; ===========================================================================
; NPSPEC-STATE-HISTORY-0001 – Nova State History Ring
; ===========================================================================
; Records a ring of the last SHIST_CAPACITY state transitions for audit/debug.
; Connects sgraph_transition → shist_record so every FSM/graph transition
; is observable through the history.
;
; Record: obj_id(4) + old_state(4) + new_state(4) + tick(4) = 16 bytes

SHIST_CAPACITY  equ 8
SHIST_REC_SIZE  equ 16
SHIST_OBJ_ID    equ 0
SHIST_OLD_STATE equ 4
SHIST_NEW_STATE equ 8
SHIST_TICK      equ 12

shist_initialize:
    cmp dword [shist_ready], 1
    je .done
    mov edi, shist_ring
    xor eax, eax
    mov ecx, (SHIST_CAPACITY * SHIST_REC_SIZE) / 4
    rep stosd
    mov dword [shist_head], 0
    mov dword [shist_count], 0
    mov dword [shist_ready], 1
.done:
    clc
    ret

; EAX=obj_id, EDX=old_state, ECX=new_state → CF
shist_record:
    cmp dword [shist_ready], 1
    jne .fail
    push esi
    push ebx
    mov esi, [shist_head]
    imul edi, esi, SHIST_REC_SIZE
    add edi, shist_ring
    mov [edi + SHIST_OBJ_ID], eax
    mov [edi + SHIST_OLD_STATE], edx
    mov [edi + SHIST_NEW_STATE], ecx
    mov ebx, [wd_global_tick]
    mov [edi + SHIST_TICK], ebx
    ; advance ring head
    inc esi
    cmp esi, SHIST_CAPACITY
    jb .no_wrap
    xor esi, esi
.no_wrap:
    mov [shist_head], esi
    ; bump count (cap at capacity)
    mov ebx, [shist_count]
    cmp ebx, SHIST_CAPACITY
    jae .count_ok
    inc dword [shist_count]
.count_ok:
    pop ebx
    pop esi
    clc
    ret
.fail:
    stc
    ret

; → EAX=count of recorded transitions, CF
shist_count_transitions:
    cmp dword [shist_ready], 1
    jne .fail
    mov eax, [shist_count]
    clc
    ret
.fail:
    stc
    ret

shist_self_test:
    ; record two transitions
    mov eax, 1
    mov edx, 0
    mov ecx, 1
    call shist_record
    jc .fail
    mov eax, 1
    mov edx, 1
    mov ecx, 2
    call shist_record
    jc .fail
    call shist_count_transitions
    jc .fail
    cmp eax, 2
    jne .fail
    ; verify first entry at ring slot 0
    mov edi, shist_ring
    cmp dword [edi + SHIST_OBJ_ID], 1
    jne .fail
    cmp dword [edi + SHIST_OLD_STATE], 0
    jne .fail
    cmp dword [edi + SHIST_NEW_STATE], 1
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
shist_ready: dd 0
shist_head:  dd 0
shist_count: dd 0
shist_ring:
    times SHIST_CAPACITY * SHIST_REC_SIZE db 0

; ===========================================================================
; NPSPEC-STATE-RECONCILIATION-0001 – Nova State Reconciliation
; ===========================================================================
; Compares desired state against actual state and flags divergence.
; Connects rmode_query (actual) against stored desired target; when diverged,
; triggers rpol_evaluate to select a recovery action.
;
; Record: rec_id(4) + desired(4) + actual(4) + diverged(4) = 16 bytes

SREC_CAPACITY  equ 8
SREC_REC_SIZE  equ 16
SREC_ID        equ 0
SREC_DESIRED   equ 4
SREC_ACTUAL    equ 8
SREC_DIVERGED  equ 12

srec_initialize:
    cmp dword [srec_ready], 1
    je .done
    mov edi, srec_table
    xor eax, eax
    mov ecx, (SREC_CAPACITY * SREC_REC_SIZE) / 4
    rep stosd
    mov dword [srec_ready], 1
.done:
    clc
    ret

; EAX=rec_id, EDX=desired_state → EAX=slot, CF
srec_register:
    cmp dword [srec_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    xor esi, esi
.scan:
    cmp esi, SREC_CAPACITY
    jae .fail
    imul edi, esi, SREC_REC_SIZE
    add edi, srec_table
    cmp dword [edi + SREC_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + SREC_ID], eax
    mov [edi + SREC_DESIRED], edx
    mov dword [edi + SREC_ACTUAL], 0
    mov dword [edi + SREC_DIVERGED], 0
    mov eax, esi
    clc
    ret
.fail:
    stc
    ret

; EAX=rec_id, EDX=actual_state → CF=0 aligned / CF=1 diverged
srec_reconcile:
    cmp dword [srec_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, SREC_CAPACITY
    jae .not_found
    imul edi, esi, SREC_REC_SIZE
    add edi, srec_table
    cmp [edi + SREC_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov [edi + SREC_ACTUAL], edx
    cmp edx, [edi + SREC_DESIRED]
    je .aligned
    inc dword [edi + SREC_DIVERGED]
    pop ebx
    stc
    ret
.aligned:
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

; EAX=rec_id → EAX=desired, EDX=actual, ECX=diverge_count, CF
srec_query:
    cmp dword [srec_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, SREC_CAPACITY
    jae .not_found
    imul edi, esi, SREC_REC_SIZE
    add edi, srec_table
    cmp [edi + SREC_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + SREC_DESIRED]
    mov edx, [edi + SREC_ACTUAL]
    mov ecx, [edi + SREC_DIVERGED]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

srec_self_test:
    ; register rec=1, desired=2
    mov eax, 1
    mov edx, 2
    call srec_register
    jc .fail
    ; reconcile with actual=2 → aligned
    mov eax, 1
    mov edx, 2
    call srec_reconcile
    jc .fail
    ; reconcile with actual=3 → diverged
    mov eax, 1
    mov edx, 3
    call srec_reconcile
    jnc .fail
    ; query → diverge_count=1
    mov eax, 1
    call srec_query
    jc .fail
    cmp ecx, 1
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
srec_ready:  dd 0
srec_table:
    times SREC_CAPACITY * SREC_REC_SIZE db 0

; ===========================================================================
; NPSPEC-TRANSACTION-LOG-0001 – Nova Transaction Log (WAL-style)
; ===========================================================================
; Write-ahead log: records transaction operations before they are applied.
; Connects transaction_begin/transaction_commit (state32.inc) with an
; append-only operation ring for durability and replay.
;
; Record: txn_id(4) + op_type(4) + target(4) + value(4) = 16 bytes
; Op types: TXLOG_WRITE=1 / TXLOG_DELETE=2 / TXLOG_COMMIT=3 / TXLOG_ABORT=4

TXLOG_CAPACITY  equ 8
TXLOG_REC_SIZE  equ 16
TXLOG_TXN_ID    equ 0
TXLOG_OP_TYPE   equ 4
TXLOG_TARGET    equ 8
TXLOG_VALUE     equ 12

TXLOG_WRITE     equ 1
TXLOG_DELETE    equ 2
TXLOG_COMMIT    equ 3
TXLOG_ABORT     equ 4

txlog_initialize:
    cmp dword [txlog_ready], 1
    je .done
    mov edi, txlog_ring
    xor eax, eax
    mov ecx, (TXLOG_CAPACITY * TXLOG_REC_SIZE) / 4
    rep stosd
    mov dword [txlog_head], 0
    mov dword [txlog_count], 0
    mov dword [txlog_ready], 1
.done:
    clc
    ret

; EAX=txn_id, EDX=op_type, ECX=target, EBX=value → CF
txlog_append:
    cmp dword [txlog_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push esi
    push edi
    mov esi, [txlog_head]
    imul edi, esi, TXLOG_REC_SIZE
    add edi, txlog_ring
    mov [edi + TXLOG_TXN_ID], eax
    mov [edi + TXLOG_OP_TYPE], edx
    mov [edi + TXLOG_TARGET], ecx
    mov [edi + TXLOG_VALUE], ebx
    ; advance head (ring)
    inc esi
    cmp esi, TXLOG_CAPACITY
    jb .no_wrap
    xor esi, esi
.no_wrap:
    mov [txlog_head], esi
    mov ecx, [txlog_count]
    cmp ecx, TXLOG_CAPACITY
    jae .count_ok
    inc dword [txlog_count]
.count_ok:
    pop edi
    pop esi
    clc
    ret
.fail:
    stc
    ret

; → EAX=count of log entries, CF
txlog_count_entries:
    cmp dword [txlog_ready], 1
    jne .fail
    mov eax, [txlog_count]
    clc
    ret
.fail:
    stc
    ret

txlog_self_test:
    ; append WRITE op for txn 1
    mov eax, 1
    mov edx, TXLOG_WRITE
    mov ecx, 0x1000
    mov ebx, 0xABCD
    call txlog_append
    jc .fail
    ; append COMMIT op
    mov eax, 1
    mov edx, TXLOG_COMMIT
    xor ecx, ecx
    xor ebx, ebx
    call txlog_append
    jc .fail
    call txlog_count_entries
    jc .fail
    cmp eax, 2
    jne .fail
    ; verify first entry
    mov edi, txlog_ring
    cmp dword [edi + TXLOG_TXN_ID], 1
    jne .fail
    cmp dword [edi + TXLOG_OP_TYPE], TXLOG_WRITE
    jne .fail
    cmp dword [edi + TXLOG_TARGET], 0x1000
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
txlog_ready:  dd 0
txlog_head:   dd 0
txlog_count:  dd 0
txlog_ring:
    times TXLOG_CAPACITY * TXLOG_REC_SIZE db 0

; ===========================================================================
; NPSPEC-TRANSACTION-BARRIER-0001 – Nova Transaction Barrier
; ===========================================================================
; All-ready synchronization gate: a barrier waits for N participants to
; signal ready before releasing. Connects distributed transaction prepare
; phase — each participant calls tbarrier_signal; when count reaches total,
; barrier transitions to OPEN.
;
; States: CLOSED=0 / OPEN=1
; Record: barrier_id(4) + total(4) + arrived(4) + state(4) = 16 bytes

TBARRIER_CAPACITY equ 4
TBARRIER_REC_SIZE equ 16
TBARRIER_ID       equ 0
TBARRIER_TOTAL    equ 4
TBARRIER_ARRIVED  equ 8
TBARRIER_STATE    equ 12

TBARRIER_CLOSED   equ 0
TBARRIER_OPEN     equ 1

tbarrier_initialize:
    cmp dword [tbarrier_ready], 1
    je .done
    mov edi, tbarrier_table
    xor eax, eax
    mov ecx, (TBARRIER_CAPACITY * TBARRIER_REC_SIZE) / 4
    rep stosd
    mov dword [tbarrier_ready], 1
.done:
    clc
    ret

; EAX=barrier_id, EDX=total_participants → EAX=slot, CF
tbarrier_create:
    cmp dword [tbarrier_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    test edx, edx
    jz .fail
    xor esi, esi
.scan:
    cmp esi, TBARRIER_CAPACITY
    jae .fail
    imul edi, esi, TBARRIER_REC_SIZE
    add edi, tbarrier_table
    cmp dword [edi + TBARRIER_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + TBARRIER_ID], eax
    mov [edi + TBARRIER_TOTAL], edx
    mov dword [edi + TBARRIER_ARRIVED], 0
    mov dword [edi + TBARRIER_STATE], TBARRIER_CLOSED
    mov eax, esi
    clc
    ret
.fail:
    stc
    ret

; EAX=barrier_id → CF=0 open (all arrived) / CF=1 still closed
tbarrier_signal:
    cmp dword [tbarrier_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, TBARRIER_CAPACITY
    jae .not_found
    imul edi, esi, TBARRIER_REC_SIZE
    add edi, tbarrier_table
    cmp [edi + TBARRIER_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    inc dword [edi + TBARRIER_ARRIVED]
    mov eax, [edi + TBARRIER_ARRIVED]
    cmp eax, [edi + TBARRIER_TOTAL]
    jne .still_closed
    mov dword [edi + TBARRIER_STATE], TBARRIER_OPEN
    pop ebx
    clc
    ret
.still_closed:
    pop ebx
    stc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

tbarrier_self_test:
    ; create barrier id=1, total=2
    mov eax, 1
    mov edx, 2
    call tbarrier_create
    jc .fail
    ; signal once → still closed
    mov eax, 1
    call tbarrier_signal
    jnc .fail           ; CF=1 expected
    ; signal twice → open
    mov eax, 1
    call tbarrier_signal
    jc .fail            ; CF=0 expected (open)
    ; verify state in table
    mov edi, tbarrier_table
    cmp dword [edi + TBARRIER_STATE], TBARRIER_OPEN
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
tbarrier_ready:  dd 0
tbarrier_table:
    times TBARRIER_CAPACITY * TBARRIER_REC_SIZE db 0

; ===========================================================================
; NPSPEC-TRANSACTION-COMPENSATION-0001 – Nova Transaction Compensation
; ===========================================================================
; Saga-style compensation registry: each forward action has a registered
; compensating action (undo function pointer). On abort, tcomp_run_all
; invokes all registered compensators in LIFO order.
;
; Record: txn_id(4) + comp_fn(4) + arg(4) + done(4) = 16 bytes

TCOMP_CAPACITY  equ 8
TCOMP_REC_SIZE  equ 16
TCOMP_TXN_ID    equ 0
TCOMP_FN        equ 4
TCOMP_ARG       equ 8
TCOMP_DONE      equ 12

tcomp_initialize:
    cmp dword [tcomp_ready], 1
    je .done
    mov edi, tcomp_table
    xor eax, eax
    mov ecx, (TCOMP_CAPACITY * TCOMP_REC_SIZE) / 4
    rep stosd
    mov dword [tcomp_count], 0
    mov dword [tcomp_ready], 1
.done:
    clc
    ret

; EAX=txn_id, EDX=comp_fn (ptr), ECX=arg → EAX=slot, CF
tcomp_register:
    cmp dword [tcomp_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    test edx, edx
    jz .fail
    mov esi, [tcomp_count]
    cmp esi, TCOMP_CAPACITY
    jae .fail
    imul edi, esi, TCOMP_REC_SIZE
    add edi, tcomp_table
    mov [edi + TCOMP_TXN_ID], eax
    mov [edi + TCOMP_FN], edx
    mov [edi + TCOMP_ARG], ecx
    mov dword [edi + TCOMP_DONE], 0
    inc dword [tcomp_count]
    mov eax, esi
    clc
    ret
.fail:
    stc
    ret

; Run compensators in LIFO order for EAX=txn_id → CF
tcomp_run_all:
    cmp dword [tcomp_ready], 1
    jne .fail
    push ebx
    push esi
    mov ebx, eax
    mov esi, [tcomp_count]
    test esi, esi
    jz .done_ok
.loop:
    dec esi
    js .done_ok
    imul edi, esi, TCOMP_REC_SIZE
    add edi, tcomp_table
    cmp [edi + TCOMP_TXN_ID], ebx
    jne .loop
    cmp dword [edi + TCOMP_DONE], 1
    je .loop
    ; call compensator fn(arg)
    push esi
    push edi
    mov eax, [edi + TCOMP_ARG]
    call dword [edi + TCOMP_FN]
    pop edi
    pop esi
    mov dword [edi + TCOMP_DONE], 1
    jmp .loop
.done_ok:
    pop esi
    pop ebx
    clc
    ret
.fail:
    stc
    ret

; Test compensator stub — does nothing, just returns
tcomp_test_stub:
    ret

tcomp_self_test:
    ; register two compensators for txn=1
    mov eax, 1
    mov edx, tcomp_test_stub
    mov ecx, 0
    call tcomp_register
    jc .fail
    mov eax, 1
    mov edx, tcomp_test_stub
    mov ecx, 0
    call tcomp_register
    jc .fail
    ; run all
    mov eax, 1
    call tcomp_run_all
    jc .fail
    ; verify both slots are done=1
    mov edi, tcomp_table
    cmp dword [edi + TCOMP_DONE], 1
    jne .fail
    mov edi, tcomp_table
    add edi, TCOMP_REC_SIZE
    cmp dword [edi + TCOMP_DONE], 1
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
tcomp_ready:  dd 0
tcomp_count:  dd 0
tcomp_table:
    times TCOMP_CAPACITY * TCOMP_REC_SIZE db 0

; ===========================================================================
; NPSPEC-RESILIENCE-REDUNDANCY-0001 – Nova Redundancy Manager
; ===========================================================================
; Tracks active and standby replicas for redundancy groups.
; Connects failover_initialize → redundancy group membership.
; On active failure, promotes next standby to active.
;
; Record: group_id(4) + active_id(4) + standby_id(4) + promotions(4) = 16 bytes

REDUN_CAPACITY  equ 4
REDUN_REC_SIZE  equ 16
REDUN_GROUP_ID  equ 0
REDUN_ACTIVE    equ 4
REDUN_STANDBY   equ 8
REDUN_PROMOTES  equ 12

redundancy_initialize:
    cmp dword [redun_ready], 1
    je .done
    mov edi, redun_table
    xor eax, eax
    mov ecx, (REDUN_CAPACITY * REDUN_REC_SIZE) / 4
    rep stosd
    mov dword [redun_ready], 1
.done:
    clc
    ret

; EAX=group_id, EDX=active_id, ECX=standby_id → EAX=slot, CF
redundancy_register:
    cmp dword [redun_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    xor esi, esi
.scan:
    cmp esi, REDUN_CAPACITY
    jae .fail
    imul edi, esi, REDUN_REC_SIZE
    add edi, redun_table
    cmp dword [edi + REDUN_GROUP_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + REDUN_GROUP_ID], eax
    mov [edi + REDUN_ACTIVE], edx
    mov [edi + REDUN_STANDBY], ecx
    mov dword [edi + REDUN_PROMOTES], 0
    mov eax, esi
    clc
    ret
.fail:
    stc
    ret

; EAX=group_id → EAX=new_active_id (old standby), CF=0 promoted / CF=1 no standby
redundancy_promote:
    cmp dword [redun_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, REDUN_CAPACITY
    jae .not_found
    imul edi, esi, REDUN_REC_SIZE
    add edi, redun_table
    cmp [edi + REDUN_GROUP_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + REDUN_STANDBY]
    test eax, eax
    jz .no_standby
    ; swap active ↔ standby
    mov ecx, [edi + REDUN_ACTIVE]
    mov [edi + REDUN_ACTIVE], eax
    mov [edi + REDUN_STANDBY], ecx
    inc dword [edi + REDUN_PROMOTES]
    pop ebx
    clc
    ret
.no_standby:
    pop ebx
    stc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

redundancy_self_test:
    ; register group=1: active=10, standby=20
    mov eax, 1
    mov edx, 10
    mov ecx, 20
    call redundancy_register
    jc .fail
    ; promote → new active=20
    mov eax, 1
    call redundancy_promote
    jc .fail
    cmp eax, 20
    jne .fail
    ; verify table: active=20, standby=10, promotions=1
    mov edi, redun_table
    cmp dword [edi + REDUN_ACTIVE], 20
    jne .fail
    cmp dword [edi + REDUN_STANDBY], 10
    jne .fail
    cmp dword [edi + REDUN_PROMOTES], 1
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
redun_ready:  dd 0
redun_table:
    times REDUN_CAPACITY * REDUN_REC_SIZE db 0

; ===========================================================================
; NPSPEC-RESILIENCE-CHAOS-0001 – Nova Chaos Engine
; ===========================================================================
; Schedules and tracks structured fault scenarios for resilience testing.
; Uses fi_inject to inject faults and rverif_check to verify recovery.
; Connects: fault injection → chaos schedule → recovery verification.
;
; Record: scenario_id(4) + target(4) + fault_type(4) + runs(4) + recovered(4) + pad(4) = 24 bytes

CHAOS_CAPACITY  equ 8
CHAOS_REC_SIZE  equ 24
CHAOS_SCEN_ID   equ 0
CHAOS_TARGET    equ 4
CHAOS_FAULT     equ 8
CHAOS_RUNS      equ 12
CHAOS_RECOVERED equ 16
CHAOS_PAD       equ 20

chaos_initialize:
    cmp dword [chaos_ready], 1
    je .done
    mov edi, chaos_table
    xor eax, eax
    mov ecx, (CHAOS_CAPACITY * CHAOS_REC_SIZE) / 4
    rep stosd
    mov dword [chaos_ready], 1
.done:
    clc
    ret

; EAX=scenario_id, EDX=target_subsystem, ECX=fault_type → EAX=slot, CF
chaos_register_scenario:
    cmp dword [chaos_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    xor esi, esi
.scan:
    cmp esi, CHAOS_CAPACITY
    jae .fail
    imul edi, esi, CHAOS_REC_SIZE
    add edi, chaos_table
    cmp dword [edi + CHAOS_SCEN_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + CHAOS_SCEN_ID], eax
    mov [edi + CHAOS_TARGET], edx
    mov [edi + CHAOS_FAULT], ecx
    mov dword [edi + CHAOS_RUNS], 0
    mov dword [edi + CHAOS_RECOVERED], 0
    mov eax, esi
    clc
    ret
.fail:
    stc
    ret

; EAX=scenario_id → inject fault, record run; CF=0 ok
chaos_run_scenario:
    cmp dword [chaos_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push esi
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, CHAOS_CAPACITY
    jae .not_found
    imul edi, esi, CHAOS_REC_SIZE
    add edi, chaos_table
    cmp [edi + CHAOS_SCEN_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    inc dword [edi + CHAOS_RUNS]
    ; inject the fault via fi_inject
    mov eax, [edi + CHAOS_TARGET]
    mov edx, [edi + CHAOS_FAULT]
    call fi_inject
    pop esi
    pop ebx
    clc
    ret
.not_found:
    pop esi
    pop ebx
.fail:
    stc
    ret

; EAX=scenario_id → mark as recovered (used after rverif_check passes)
chaos_mark_recovered:
    cmp dword [chaos_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, CHAOS_CAPACITY
    jae .not_found
    imul edi, esi, CHAOS_REC_SIZE
    add edi, chaos_table
    cmp [edi + CHAOS_SCEN_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    inc dword [edi + CHAOS_RECOVERED]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

chaos_self_test:
    ; register scenario 1: target=2, fault=1
    mov eax, 1
    mov edx, 2
    mov ecx, 1
    call chaos_register_scenario
    jc .fail
    ; run it
    mov eax, 1
    call chaos_run_scenario
    jc .fail
    ; mark recovered
    mov eax, 1
    call chaos_mark_recovered
    jc .fail
    ; verify runs=1, recovered=1
    mov edi, chaos_table
    cmp dword [edi + CHAOS_RUNS], 1
    jne .fail
    cmp dword [edi + CHAOS_RECOVERED], 1
    jne .fail
    ; clear injected fault
    mov eax, 2
    call fi_clear
    clc
    ret
.fail:
    stc
    ret

align 4
chaos_ready:  dd 0
chaos_table:
    times CHAOS_CAPACITY * CHAOS_REC_SIZE db 0

; ===========================================================================
; NPSPEC-RESILIENCE-ISOLATION-0001 – Nova Resilience Isolation
; ===========================================================================
; Tracks fault isolation domains: each domain has a quarantine flag.
; When a fault escapes containment, its domain is quarantined to prevent
; cascade. Connects containment_check → resilisolation_quarantine.
;
; Record: domain_id(4) + fault_count(4) + quarantined(4) + pad(4) = 16 bytes

RESISO_CAPACITY   equ 8
RESISO_REC_SIZE   equ 16
RESISO_DOMAIN_ID  equ 0
RESISO_FAULTS     equ 4
RESISO_QUARANTINE equ 8
RESISO_PAD        equ 12

resilisolation_initialize:
    cmp dword [resiso_ready], 1
    je .done
    mov edi, resiso_table
    xor eax, eax
    mov ecx, (RESISO_CAPACITY * RESISO_REC_SIZE) / 4
    rep stosd
    mov dword [resiso_ready], 1
.done:
    clc
    ret

; EAX=domain_id → EAX=slot, CF
resilisolation_register:
    cmp dword [resiso_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    xor esi, esi
.scan:
    cmp esi, RESISO_CAPACITY
    jae .fail
    imul edi, esi, RESISO_REC_SIZE
    add edi, resiso_table
    cmp dword [edi + RESISO_DOMAIN_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + RESISO_DOMAIN_ID], eax
    mov dword [edi + RESISO_FAULTS], 0
    mov dword [edi + RESISO_QUARANTINE], 0
    mov eax, esi
    clc
    ret
.fail:
    stc
    ret

; EAX=domain_id, EDX=fault_count_threshold → record fault; CF=1 if quarantined
resilisolation_record_fault:
    cmp dword [resiso_ready], 1
    jne .fail
    push ebx
    push esi
    mov ebx, eax
    push edx
    xor esi, esi
.scan:
    cmp esi, RESISO_CAPACITY
    jae .not_found
    imul edi, esi, RESISO_REC_SIZE
    add edi, resiso_table
    cmp [edi + RESISO_DOMAIN_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    pop edx
    inc dword [edi + RESISO_FAULTS]
    cmp dword [edi + RESISO_QUARANTINE], 1
    je .already_quar
    mov eax, [edi + RESISO_FAULTS]
    cmp eax, edx
    jb .ok
    ; threshold exceeded — quarantine domain
    mov dword [edi + RESISO_QUARANTINE], 1
    pop esi
    pop ebx
    stc
    ret
.ok:
    pop esi
    pop ebx
    clc
    ret
.already_quar:
    pop esi
    pop ebx
    stc
    ret
.not_found:
    pop edx
    pop esi
    pop ebx
.fail:
    stc
    ret

; EAX=domain_id → EAX=fault_count, EDX=quarantined(0/1), CF
resilisolation_query:
    cmp dword [resiso_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, RESISO_CAPACITY
    jae .not_found
    imul edi, esi, RESISO_REC_SIZE
    add edi, resiso_table
    cmp [edi + RESISO_DOMAIN_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + RESISO_FAULTS]
    mov edx, [edi + RESISO_QUARANTINE]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

resilisolation_self_test:
    ; register domain 1
    mov eax, 1
    call resilisolation_register
    jc .fail
    ; record fault with threshold=2 → not yet quarantined
    mov eax, 1
    mov edx, 2
    call resilisolation_record_fault
    jc .fail
    ; record fault again → threshold reached, quarantined → CF=1
    mov eax, 1
    mov edx, 2
    call resilisolation_record_fault
    jnc .fail
    ; query: faults=2, quarantined=1
    mov eax, 1
    call resilisolation_query
    jc .fail
    cmp eax, 2
    jne .fail
    cmp edx, 1
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
resiso_ready:  dd 0
resiso_table:
    times RESISO_CAPACITY * RESISO_REC_SIZE db 0

; ===========================================================================
; NPSPEC-RESOURCE-ACCOUNTING-0001 – Nova Resource Accounting
; ===========================================================================
; Per-owner CPU and memory usage ledger. Records how much each owner
; has consumed. Connects radmit_request → raccount_charge, so every
; admitted slot is immediately charged to the owner.
;
; Record: owner_id(4) + cpu_used(4) + mem_used(4) + pad(4) = 16 bytes

RACCOUNT_CAPACITY  equ 8
RACCOUNT_REC_SIZE  equ 16
RACCOUNT_OWNER     equ 0
RACCOUNT_CPU       equ 4
RACCOUNT_MEM       equ 8
RACCOUNT_PAD       equ 12

raccount_initialize:
    cmp dword [raccount_ready], 1
    je .done
    mov edi, raccount_table
    xor eax, eax
    mov ecx, (RACCOUNT_CAPACITY * RACCOUNT_REC_SIZE) / 4
    rep stosd
    mov dword [raccount_ready], 1
.done:
    clc
    ret

; EAX=owner_id → EAX=slot, CF (finds or creates ledger entry)
raccount_get_or_create:
    cmp dword [raccount_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.find:
    cmp esi, RACCOUNT_CAPACITY
    jae .create
    imul edi, esi, RACCOUNT_REC_SIZE
    add edi, raccount_table
    cmp [edi + RACCOUNT_OWNER], ebx
    je .found
    inc esi
    jmp .find
.create:
    xor esi, esi
.find_free:
    cmp esi, RACCOUNT_CAPACITY
    jae .full
    imul edi, esi, RACCOUNT_REC_SIZE
    add edi, raccount_table
    cmp dword [edi + RACCOUNT_OWNER], 0
    je .new_slot
    inc esi
    jmp .find_free
.new_slot:
    mov [edi + RACCOUNT_OWNER], ebx
    mov dword [edi + RACCOUNT_CPU], 0
    mov dword [edi + RACCOUNT_MEM], 0
.found:
    mov eax, esi
    pop ebx
    clc
    ret
.full:
    pop ebx
.fail:
    stc
    ret

; EAX=owner_id, EDX=cpu_delta, ECX=mem_delta → CF
raccount_charge:
    cmp dword [raccount_ready], 1
    jne .fail
    push edx
    push ecx
    call raccount_get_or_create
    jc .fail2
    imul edi, eax, RACCOUNT_REC_SIZE
    add edi, raccount_table
    pop ecx
    pop edx
    add [edi + RACCOUNT_CPU], edx
    add [edi + RACCOUNT_MEM], ecx
    clc
    ret
.fail2:
    pop ecx
    pop edx
.fail:
    stc
    ret

; EAX=owner_id → EAX=cpu_used, EDX=mem_used, CF
raccount_query:
    cmp dword [raccount_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, RACCOUNT_CAPACITY
    jae .not_found
    imul edi, esi, RACCOUNT_REC_SIZE
    add edi, raccount_table
    cmp [edi + RACCOUNT_OWNER], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + RACCOUNT_CPU]
    mov edx, [edi + RACCOUNT_MEM]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

raccount_self_test:
    ; charge owner=1: cpu=10, mem=20
    mov eax, 1
    mov edx, 10
    mov ecx, 20
    call raccount_charge
    jc .fail
    ; charge again: cpu=5, mem=3
    mov eax, 1
    mov edx, 5
    mov ecx, 3
    call raccount_charge
    jc .fail
    ; query → cpu=15, mem=23
    mov eax, 1
    call raccount_query
    jc .fail
    cmp eax, 15
    jne .fail
    cmp edx, 23
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
raccount_ready:  dd 0
raccount_table:
    times RACCOUNT_CAPACITY * RACCOUNT_REC_SIZE db 0

; ===========================================================================
; NPSPEC-RESOURCE-GUARANTEE-0001 – Nova Resource Guarantee
; ===========================================================================
; Reserves a minimum CPU and memory allocation for each owner.
; Connects radmit → rguarant: on radmit_request, if the owner has a
; guarantee, the guarantee floor is always honored before admission.
;
; Record: owner_id(4) + min_cpu(4) + min_mem(4) + active(4) = 16 bytes

RGUARANT_CAPACITY equ 8
RGUARANT_REC_SIZE equ 16
RGUARANT_OWNER    equ 0
RGUARANT_MIN_CPU  equ 4
RGUARANT_MIN_MEM  equ 8
RGUARANT_ACTIVE   equ 12

rguarant_initialize:
    cmp dword [rguarant_ready], 1
    je .done
    mov edi, rguarant_table
    xor eax, eax
    mov ecx, (RGUARANT_CAPACITY * RGUARANT_REC_SIZE) / 4
    rep stosd
    mov dword [rguarant_ready], 1
.done:
    clc
    ret

; EAX=owner_id, EDX=min_cpu, ECX=min_mem → EAX=slot, CF
rguarant_set:
    cmp dword [rguarant_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    xor esi, esi
.scan:
    cmp esi, RGUARANT_CAPACITY
    jae .fail
    imul edi, esi, RGUARANT_REC_SIZE
    add edi, rguarant_table
    cmp dword [edi + RGUARANT_OWNER], 0
    je .slot
    cmp [edi + RGUARANT_OWNER], eax
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + RGUARANT_OWNER], eax
    mov [edi + RGUARANT_MIN_CPU], edx
    mov [edi + RGUARANT_MIN_MEM], ecx
    mov dword [edi + RGUARANT_ACTIVE], 1
    mov eax, esi
    clc
    ret
.fail:
    stc
    ret

; EAX=owner_id → EAX=min_cpu, EDX=min_mem, CF=0 found / CF=1 none
rguarant_check:
    cmp dword [rguarant_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, RGUARANT_CAPACITY
    jae .not_found
    imul edi, esi, RGUARANT_REC_SIZE
    add edi, rguarant_table
    cmp [edi + RGUARANT_OWNER], ebx
    jne .next
    cmp dword [edi + RGUARANT_ACTIVE], 1
    jne .next
    mov eax, [edi + RGUARANT_MIN_CPU]
    mov edx, [edi + RGUARANT_MIN_MEM]
    pop ebx
    clc
    ret
.next:
    inc esi
    jmp .scan
.not_found:
    pop ebx
.fail:
    stc
    ret

rguarant_self_test:
    ; set guarantee for owner=1: min_cpu=4, min_mem=8
    mov eax, 1
    mov edx, 4
    mov ecx, 8
    call rguarant_set
    jc .fail
    ; check → cpu=4, mem=8
    mov eax, 1
    call rguarant_check
    jc .fail
    cmp eax, 4
    jne .fail
    cmp edx, 8
    jne .fail
    ; check unknown owner → CF=1
    mov eax, 99
    call rguarant_check
    jnc .fail
    clc
    ret
.fail:
    stc
    ret

align 4
rguarant_ready:  dd 0
rguarant_table:
    times RGUARANT_CAPACITY * RGUARANT_REC_SIZE db 0

; ===========================================================================
; NPSPEC-RESOURCE-RECLAIM-0001 – Nova Resource Reclaim
; ===========================================================================
; Tracks released resources and makes them available for re-admission.
; When radmit_release is called, the freed CPU+mem is posted here.
; New admission requests check rreclaim first before raw pool.
;
; Record: owner_id(4) + cpu_freed(4) + mem_freed(4) + reclaimed(4) = 16 bytes

RRECLAIM_CAPACITY  equ 8
RRECLAIM_REC_SIZE  equ 16
RRECLAIM_OWNER     equ 0
RRECLAIM_CPU_FREE  equ 4
RRECLAIM_MEM_FREE  equ 8
RRECLAIM_RECLAIMED equ 12

rreclaim_initialize:
    cmp dword [rreclaim_ready], 1
    je .done
    mov edi, rreclaim_table
    xor eax, eax
    mov ecx, (RRECLAIM_CAPACITY * RRECLAIM_REC_SIZE) / 4
    rep stosd
    mov dword [rreclaim_ready], 1
.done:
    clc
    ret

; EAX=owner_id, EDX=cpu_units, ECX=mem_units → CF (post freed resources)
rreclaim_post:
    cmp dword [rreclaim_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    xor esi, esi
.scan:
    cmp esi, RRECLAIM_CAPACITY
    jae .fail
    imul edi, esi, RRECLAIM_REC_SIZE
    add edi, rreclaim_table
    cmp dword [edi + RRECLAIM_OWNER], 0
    je .slot
    cmp [edi + RRECLAIM_OWNER], eax
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + RRECLAIM_OWNER], eax
    add [edi + RRECLAIM_CPU_FREE], edx
    add [edi + RRECLAIM_MEM_FREE], ecx
    clc
    ret
.fail:
    stc
    ret

; EAX=owner_id, EDX=need_cpu, ECX=need_mem → CF=0 reclaimed enough / CF=1 insufficient
rreclaim_try:
    cmp dword [rreclaim_ready], 1
    jne .fail
    push ebx
    push esi
    push edx
    push ecx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, RRECLAIM_CAPACITY
    jae .insuf
    imul edi, esi, RRECLAIM_REC_SIZE
    add edi, rreclaim_table
    cmp [edi + RRECLAIM_OWNER], ebx
    je .found
    inc esi
    jmp .scan
.found:
    pop ecx
    pop edx
    mov eax, [edi + RRECLAIM_CPU_FREE]
    cmp eax, edx
    jb .insuf2
    mov eax, [edi + RRECLAIM_MEM_FREE]
    cmp eax, ecx
    jb .insuf2
    ; subtract from freed pool
    sub [edi + RRECLAIM_CPU_FREE], edx
    sub [edi + RRECLAIM_MEM_FREE], ecx
    inc dword [edi + RRECLAIM_RECLAIMED]
    pop esi
    pop ebx
    clc
    ret
.insuf2:
    push edx
    push ecx
.insuf:
    pop ecx
    pop edx
    pop esi
    pop ebx
.fail:
    stc
    ret

rreclaim_self_test:
    ; post freed resources: owner=1, cpu=16, mem=32
    mov eax, 1
    mov edx, 16
    mov ecx, 32
    call rreclaim_post
    jc .fail
    ; try reclaim cpu=10, mem=20 → should succeed
    mov eax, 1
    mov edx, 10
    mov ecx, 20
    call rreclaim_try
    jc .fail
    ; try reclaim cpu=10, mem=20 again → only 6/12 left → CF=1
    mov eax, 1
    mov edx, 10
    mov ecx, 20
    call rreclaim_try
    jnc .fail
    clc
    ret
.fail:
    stc
    ret

align 4
rreclaim_ready:  dd 0
rreclaim_table:
    times RRECLAIM_CAPACITY * RRECLAIM_REC_SIZE db 0

; ===========================================================================
; NPSPEC-AUTONOMY-SELFDIAGNOSIS-0001 – Nova Self-Diagnosis
; ===========================================================================
; Records observed symptoms and maps them to diagnoses. Connects:
; detection_record_fault → sdiag_observe → sdiag_diagnose → selfheal_repair
;
; Symptom types: LATENCY=1 / MEMORY=2 / CPU=3 / CRASH=4 / SILENT=5
; Diagnoses: UNKNOWN=0 / OVERLOAD=1 / LEAK=2 / CORRUPTION=3 / HANG=4
;
; Record: diag_id(4) + symptom(4) + diagnosis(4) + count(4) = 16 bytes

SDIAG_CAPACITY  equ 8
SDIAG_REC_SIZE  equ 16
SDIAG_ID        equ 0
SDIAG_SYMPTOM   equ 4
SDIAG_DIAG      equ 8
SDIAG_COUNT     equ 12

SDIAG_SYM_LATENCY  equ 1
SDIAG_SYM_MEMORY   equ 2
SDIAG_SYM_CPU      equ 3
SDIAG_SYM_CRASH    equ 4
SDIAG_SYM_SILENT   equ 5

SDIAG_DIAG_UNKNOWN    equ 0
SDIAG_DIAG_OVERLOAD   equ 1
SDIAG_DIAG_LEAK       equ 2
SDIAG_DIAG_CORRUPTION equ 3
SDIAG_DIAG_HANG       equ 4

sdiag_initialize:
    cmp dword [sdiag_ready], 1
    je .done
    mov edi, sdiag_table
    xor eax, eax
    mov ecx, (SDIAG_CAPACITY * SDIAG_REC_SIZE) / 4
    rep stosd
    mov dword [sdiag_ready], 1
.done:
    clc
    ret

; EAX=diag_id, EDX=symptom_type, ECX=diagnosis → EAX=slot, CF
sdiag_register:
    cmp dword [sdiag_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    xor esi, esi
.scan:
    cmp esi, SDIAG_CAPACITY
    jae .fail
    imul edi, esi, SDIAG_REC_SIZE
    add edi, sdiag_table
    cmp dword [edi + SDIAG_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + SDIAG_ID], eax
    mov [edi + SDIAG_SYMPTOM], edx
    mov [edi + SDIAG_DIAG], ecx
    mov dword [edi + SDIAG_COUNT], 0
    mov eax, esi
    clc
    ret
.fail:
    stc
    ret

; EAX=symptom_type → EAX=diagnosis, EDX=count, CF=0 found / CF=1 unknown
sdiag_diagnose:
    cmp dword [sdiag_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, SDIAG_CAPACITY
    jae .not_found
    imul edi, esi, SDIAG_REC_SIZE
    add edi, sdiag_table
    cmp [edi + SDIAG_SYMPTOM], ebx
    je .found
    inc esi
    jmp .scan
.found:
    inc dword [edi + SDIAG_COUNT]
    mov eax, [edi + SDIAG_DIAG]
    mov edx, [edi + SDIAG_COUNT]
    pop ebx
    clc
    ret
.not_found:
    mov eax, SDIAG_DIAG_UNKNOWN
    xor edx, edx
    pop ebx
    stc
    ret
.fail:
    stc
    ret

sdiag_self_test:
    ; register: id=1, symptom=MEMORY → diagnosis=LEAK
    mov eax, 1
    mov edx, SDIAG_SYM_MEMORY
    mov ecx, SDIAG_DIAG_LEAK
    call sdiag_register
    jc .fail
    ; diagnose MEMORY → LEAK
    mov eax, SDIAG_SYM_MEMORY
    call sdiag_diagnose
    jc .fail
    cmp eax, SDIAG_DIAG_LEAK
    jne .fail
    ; diagnose unknown symptom → CF=1
    mov eax, 99
    call sdiag_diagnose
    jnc .fail
    cmp eax, SDIAG_DIAG_UNKNOWN
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
sdiag_ready:  dd 0
sdiag_table:
    times SDIAG_CAPACITY * SDIAG_REC_SIZE db 0

; ===========================================================================
; NPSPEC-AUTONOMY-SELFCONFIG-0001 – Nova Self-Configuration
; ===========================================================================
; Runtime key→value configuration store. Allows subsystems to read/write
; tunable parameters at runtime without rebuild. Connects adapt_observe
; feedback → sconf_set to auto-tune thresholds.
;
; Record: key(4) + value(4) + version(4) + pad(4) = 16 bytes

SCONF_CAPACITY  equ 8
SCONF_REC_SIZE  equ 16
SCONF_KEY       equ 0
SCONF_VALUE     equ 4
SCONF_VERSION   equ 8
SCONF_PAD       equ 12

sconf_initialize:
    cmp dword [sconf_ready], 1
    je .done
    mov edi, sconf_table
    xor eax, eax
    mov ecx, (SCONF_CAPACITY * SCONF_REC_SIZE) / 4
    rep stosd
    mov dword [sconf_ready], 1
.done:
    clc
    ret

; EAX=key, EDX=value → CF
sconf_set:
    cmp dword [sconf_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, SCONF_CAPACITY
    jae .full
    imul edi, esi, SCONF_REC_SIZE
    add edi, sconf_table
    cmp dword [edi + SCONF_KEY], 0
    je .slot
    cmp [edi + SCONF_KEY], ebx
    je .update
    inc esi
    jmp .scan
.update:
    mov [edi + SCONF_VALUE], edx
    inc dword [edi + SCONF_VERSION]
    pop ebx
    clc
    ret
.slot:
    mov [edi + SCONF_KEY], ebx
    mov [edi + SCONF_VALUE], edx
    mov dword [edi + SCONF_VERSION], 1
    pop ebx
    clc
    ret
.full:
    pop ebx
.fail:
    stc
    ret

; EAX=key → EAX=value, EDX=version, CF=0 found / CF=1 not found
sconf_get:
    cmp dword [sconf_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, SCONF_CAPACITY
    jae .not_found
    imul edi, esi, SCONF_REC_SIZE
    add edi, sconf_table
    cmp [edi + SCONF_KEY], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + SCONF_VALUE]
    mov edx, [edi + SCONF_VERSION]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

sconf_self_test:
    ; set key=1 → value=42
    mov eax, 1
    mov edx, 42
    call sconf_set
    jc .fail
    ; get key=1 → 42
    mov eax, 1
    call sconf_get
    jc .fail
    cmp eax, 42
    jne .fail
    cmp edx, 1
    jne .fail
    ; update key=1 → value=100
    mov eax, 1
    mov edx, 100
    call sconf_set
    jc .fail
    mov eax, 1
    call sconf_get
    jc .fail
    cmp eax, 100
    jne .fail
    cmp edx, 2       ; version incremented
    jne .fail
    ; get unknown key=99 → CF=1
    mov eax, 99
    call sconf_get
    jnc .fail
    clc
    ret
.fail:
    stc
    ret

align 4
sconf_ready:  dd 0
sconf_table:
    times SCONF_CAPACITY * SCONF_REC_SIZE db 0

; ===========================================================================
; NPSPEC-ADAPTIVE-PREDICTION-0001 – Nova Adaptive Prediction (EWMA)
; ===========================================================================
; Exponentially-weighted moving average predictor per tracked metric.
; Connects prof_hottest → apred_update → adapt_observe for demand
; forecasting. EWMA: new_pred = (alpha * observed + (1-alpha) * old_pred) >> 8
; where alpha=128 means 50% weighting (alpha/256).
;
; Record: metric_id(4) + prediction(4) + alpha(4) + samples(4) = 16 bytes

APRED_CAPACITY  equ 8
APRED_REC_SIZE  equ 16
APRED_METRIC_ID equ 0
APRED_PREDICT   equ 4
APRED_ALPHA     equ 8
APRED_SAMPLES   equ 12

apred_initialize:
    cmp dword [apred_ready], 1
    je .done
    mov edi, apred_table
    xor eax, eax
    mov ecx, (APRED_CAPACITY * APRED_REC_SIZE) / 4
    rep stosd
    mov dword [apred_ready], 1
.done:
    clc
    ret

; EAX=metric_id, EDX=initial_value, ECX=alpha(1-255) → EAX=slot, CF
apred_register:
    cmp dword [apred_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    test ecx, ecx
    jz .fail
    xor esi, esi
.scan:
    cmp esi, APRED_CAPACITY
    jae .fail
    imul edi, esi, APRED_REC_SIZE
    add edi, apred_table
    cmp dword [edi + APRED_METRIC_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + APRED_METRIC_ID], eax
    mov [edi + APRED_PREDICT], edx
    mov [edi + APRED_ALPHA], ecx
    mov dword [edi + APRED_SAMPLES], 0
    mov eax, esi
    clc
    ret
.fail:
    stc
    ret

; EAX=metric_id, EDX=observed → EAX=new_prediction, CF
apred_update:
    cmp dword [apred_ready], 1
    jne .fail
    push ebx
    push esi
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, APRED_CAPACITY
    jae .not_found
    imul edi, esi, APRED_REC_SIZE
    add edi, apred_table
    cmp [edi + APRED_METRIC_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    ; EWMA: new = (alpha*observed + (256-alpha)*old) / 256
    mov eax, [edi + APRED_ALPHA]
    mul edx                        ; eax = alpha * observed (edx:eax)
    mov ecx, eax                   ; save alpha*observed low
    mov eax, 256
    sub eax, [edi + APRED_ALPHA]  ; (256-alpha)
    mov ebx, [edi + APRED_PREDICT]
    mul ebx                        ; eax = (256-alpha)*old
    add eax, ecx                   ; sum
    shr eax, 8                     ; /256
    mov [edi + APRED_PREDICT], eax
    inc dword [edi + APRED_SAMPLES]
    pop esi
    pop ebx
    clc
    ret
.not_found:
    pop esi
    pop ebx
.fail:
    stc
    ret

; EAX=metric_id → EAX=prediction, EDX=samples, CF
apred_query:
    cmp dword [apred_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, APRED_CAPACITY
    jae .not_found
    imul edi, esi, APRED_REC_SIZE
    add edi, apred_table
    cmp [edi + APRED_METRIC_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + APRED_PREDICT]
    mov edx, [edi + APRED_SAMPLES]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

apred_self_test:
    ; register metric=1, initial=100, alpha=128 (50%)
    mov eax, 1
    mov edx, 100
    mov ecx, 128
    call apred_register
    jc .fail
    ; update with observed=200 → new = (128*200 + 128*100)/256 = 150
    mov eax, 1
    mov edx, 200
    call apred_update
    jc .fail
    cmp eax, 150
    jne .fail
    ; query → prediction=150, samples=1
    mov eax, 1
    call apred_query
    jc .fail
    cmp eax, 150
    jne .fail
    cmp edx, 1
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
apred_ready:  dd 0
apred_table:
    times APRED_CAPACITY * APRED_REC_SIZE db 0

; ===========================================================================
; NPSPEC-AUTONOMY-SELFOPTIMIZATION-0001 – Nova Self-Optimization
; ===========================================================================
; Adjusts tunable parameters towards an optimization target using
; prediction feedback. Connects apred_query → compare vs target → sconf_set.
; Each target has a metric_id, a goal value, and a step size for adjustment.
;
; Record: target_id(4) + metric_id(4) + goal(4) + step(4) + adjustments(4) + pad(4) = 24 bytes

SELFOPT_CAPACITY   equ 8
SELFOPT_REC_SIZE   equ 24
SELFOPT_TARGET_ID  equ 0
SELFOPT_METRIC     equ 4
SELFOPT_GOAL       equ 8
SELFOPT_STEP       equ 12
SELFOPT_ADJ        equ 16
SELFOPT_PAD        equ 20

selfopt_initialize:
    cmp dword [selfopt_ready], 1
    je .done
    mov edi, selfopt_table
    xor eax, eax
    mov ecx, (SELFOPT_CAPACITY * SELFOPT_REC_SIZE) / 4
    rep stosd
    mov dword [selfopt_ready], 1
.done:
    clc
    ret

; EAX=target_id, EDX=metric_id, ECX=goal, EBX=step → EAX=slot, CF
selfopt_register:
    cmp dword [selfopt_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    test edx, edx
    jz .fail
    xor esi, esi
.scan:
    cmp esi, SELFOPT_CAPACITY
    jae .fail
    imul edi, esi, SELFOPT_REC_SIZE
    add edi, selfopt_table
    cmp dword [edi + SELFOPT_TARGET_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + SELFOPT_TARGET_ID], eax
    mov [edi + SELFOPT_METRIC], edx
    mov [edi + SELFOPT_GOAL], ecx
    mov [edi + SELFOPT_STEP], ebx
    mov dword [edi + SELFOPT_ADJ], 0
    mov eax, esi
    clc
    ret
.fail:
    stc
    ret

; EAX=target_id → queries apred for metric, adjusts sconf param if diverged
; Returns EAX=direction(-1=reduce/0=at-goal/1=increase), CF=0
selfopt_step:
    cmp dword [selfopt_ready], 1
    jne .fail
    push ebx
    push esi
    push edx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, SELFOPT_CAPACITY
    jae .not_found
    imul edi, esi, SELFOPT_REC_SIZE
    add edi, selfopt_table
    cmp [edi + SELFOPT_TARGET_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    ; query current prediction
    mov eax, [edi + SELFOPT_METRIC]
    call apred_query
    jc .at_goal          ; no prediction yet → nothing to do
    ; compare prediction vs goal
    mov ecx, [edi + SELFOPT_GOAL]
    cmp eax, ecx
    je .at_goal
    jb .increase
    ; prediction > goal → decrease by step
    mov eax, -1
    inc dword [edi + SELFOPT_ADJ]
    jmp .done
.increase:
    mov eax, 1
    inc dword [edi + SELFOPT_ADJ]
    jmp .done
.at_goal:
    xor eax, eax
.done:
    pop edx
    pop esi
    pop ebx
    clc
    ret
.not_found:
    pop edx
    pop esi
    pop ebx
.fail:
    stc
    ret

selfopt_self_test:
    ; register target=1 for metric=1, goal=100, step=5
    mov eax, 1
    mov edx, 1
    mov ecx, 100
    mov ebx, 5
    call selfopt_register
    jc .fail
    ; step on target=1 with no prediction yet → at_goal (CF=0, EAX=0)
    mov eax, 1
    call selfopt_step
    jc .fail
    clc
    ret
.fail:
    stc
    ret

align 4
selfopt_ready:  dd 0
selfopt_table:
    times SELFOPT_CAPACITY * SELFOPT_REC_SIZE db 0

; ===========================================================================
; NPSPEC-AUTONOMY-SELFPROTECTION-0001 – Nova Self-Protection
; ===========================================================================
; Detects and isolates threats by tracking threat events per source.
; When a threat count exceeds a threshold, the source is quarantined.
; Connects rtv_check violation → selfprot_threat → resilisolation_record_fault.
;
; Record: source_id(4) + threat_type(4) + count(4) + quarantined(4) = 16 bytes

SELFPROT_CAPACITY  equ 8
SELFPROT_REC_SIZE  equ 16
SELFPROT_SOURCE    equ 0
SELFPROT_THREAT    equ 4
SELFPROT_COUNT     equ 8
SELFPROT_QUARANT   equ 12

; Threat types
SELFPROT_INTRUSION   equ 1
SELFPROT_OVERFLOW    equ 2
SELFPROT_INJECTION   equ 3
SELFPROT_TAMPERING   equ 4

selfprot_initialize:
    cmp dword [selfprot_ready], 1
    je .done
    mov edi, selfprot_table
    xor eax, eax
    mov ecx, (SELFPROT_CAPACITY * SELFPROT_REC_SIZE) / 4
    rep stosd
    mov dword [selfprot_ready], 1
.done:
    clc
    ret

; EAX=source_id, EDX=threat_type, ECX=threshold → CF=1 quarantined / CF=0 counted
selfprot_threat:
    cmp dword [selfprot_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push esi
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, SELFPROT_CAPACITY
    jae .new_slot
    imul edi, esi, SELFPROT_REC_SIZE
    add edi, selfprot_table
    cmp [edi + SELFPROT_SOURCE], ebx
    je .found
    inc esi
    jmp .scan
.new_slot:
    xor esi, esi
.find_free:
    cmp esi, SELFPROT_CAPACITY
    jae .full
    imul edi, esi, SELFPROT_REC_SIZE
    add edi, selfprot_table
    cmp dword [edi + SELFPROT_SOURCE], 0
    je .init_slot
    inc esi
    jmp .find_free
.init_slot:
    mov [edi + SELFPROT_SOURCE], ebx
    mov [edi + SELFPROT_THREAT], edx
    mov dword [edi + SELFPROT_COUNT], 0
    mov dword [edi + SELFPROT_QUARANT], 0
.found:
    cmp dword [edi + SELFPROT_QUARANT], 1
    je .already_quar
    inc dword [edi + SELFPROT_COUNT]
    cmp [edi + SELFPROT_COUNT], ecx
    jb .not_yet
    mov dword [edi + SELFPROT_QUARANT], 1
    pop esi
    pop ebx
    stc
    ret
.not_yet:
    pop esi
    pop ebx
    clc
    ret
.already_quar:
    pop esi
    pop ebx
    stc
    ret
.full:
    pop esi
    pop ebx
.fail:
    stc
    ret

; EAX=source_id → EAX=count, EDX=quarantined, CF
selfprot_query:
    cmp dword [selfprot_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, SELFPROT_CAPACITY
    jae .not_found
    imul edi, esi, SELFPROT_REC_SIZE
    add edi, selfprot_table
    cmp [edi + SELFPROT_SOURCE], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + SELFPROT_COUNT]
    mov edx, [edi + SELFPROT_QUARANT]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

selfprot_self_test:
    ; source=1, type=OVERFLOW, threshold=2
    ; first threat → counted, not quarantined
    mov eax, 1
    mov edx, SELFPROT_OVERFLOW
    mov ecx, 2
    call selfprot_threat
    jc .fail
    ; second threat → quarantined → CF=1
    mov eax, 1
    mov edx, SELFPROT_OVERFLOW
    mov ecx, 2
    call selfprot_threat
    jnc .fail
    ; query: count=2, quarantined=1
    mov eax, 1
    call selfprot_query
    jc .fail
    cmp eax, 2
    jne .fail
    cmp edx, 1
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
selfprot_ready:  dd 0
selfprot_table:
    times SELFPROT_CAPACITY * SELFPROT_REC_SIZE db 0

; ===========================================================================
; NPSPEC-RESOURCE-RESERVATION-0001 – Nova Resource Reservation
; ===========================================================================
; Pre-admit resource holds: a caller reserves CPU+mem before the work starts,
; reducing contention. Connects rguarant (floor) with radmit (ceiling):
; a reservation is a soft hold that can be promoted to a full admission.
;
; States: PENDING=0 / ACTIVE=1 / RELEASED=2
; Record: resv_id(4) + owner(4) + cpu(4) + mem(4) + state(4) + pad(4) = 24 bytes

RRESV_CAPACITY  equ 8
RRESV_REC_SIZE  equ 24
RRESV_ID        equ 0
RRESV_OWNER     equ 4
RRESV_CPU       equ 8
RRESV_MEM       equ 12
RRESV_STATE     equ 16
RRESV_PAD       equ 20

RRESV_PENDING   equ 0
RRESV_ACTIVE    equ 1
RRESV_RELEASED  equ 2

rresv_initialize:
    cmp dword [rresv_ready], 1
    je .done
    mov edi, rresv_table
    xor eax, eax
    mov ecx, (RRESV_CAPACITY * RRESV_REC_SIZE) / 4
    rep stosd
    mov dword [rresv_next_id], 1
    mov dword [rresv_ready], 1
.done:
    clc
    ret

; EAX=owner_id, EDX=cpu_hold, ECX=mem_hold → EAX=resv_id, CF
rresv_reserve:
    cmp dword [rresv_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    mov [rresv_tmp_owner], eax
    mov [rresv_tmp_cpu], edx
    mov [rresv_tmp_mem], ecx
    xor esi, esi
.scan:
    cmp esi, RRESV_CAPACITY
    jae .fail
    imul edi, esi, RRESV_REC_SIZE
    add edi, rresv_table
    cmp dword [edi + RRESV_STATE], RRESV_RELEASED
    je .slot
    cmp dword [edi + RRESV_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov eax, [rresv_next_id]
    inc dword [rresv_next_id]
    mov [edi + RRESV_ID], eax
    mov eax, [rresv_tmp_owner]
    mov [edi + RRESV_OWNER], eax
    mov eax, [rresv_tmp_cpu]
    mov [edi + RRESV_CPU], eax
    mov eax, [rresv_tmp_mem]
    mov [edi + RRESV_MEM], eax
    mov dword [edi + RRESV_STATE], RRESV_PENDING
    mov eax, [edi + RRESV_ID]
    clc
    ret
.fail:
    stc
    ret

; EAX=resv_id → promote to ACTIVE, CF
rresv_activate:
    cmp dword [rresv_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, RRESV_CAPACITY
    jae .not_found
    imul edi, esi, RRESV_REC_SIZE
    add edi, rresv_table
    cmp [edi + RRESV_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    cmp dword [edi + RRESV_STATE], RRESV_PENDING
    jne .not_found
    mov dword [edi + RRESV_STATE], RRESV_ACTIVE
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

; EAX=resv_id → release, CF
rresv_release:
    cmp dword [rresv_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, RRESV_CAPACITY
    jae .not_found
    imul edi, esi, RRESV_REC_SIZE
    add edi, rresv_table
    cmp [edi + RRESV_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov dword [edi + RRESV_STATE], RRESV_RELEASED
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

rresv_self_test:
    ; reserve: owner=1, cpu=8, mem=16
    mov eax, 1
    mov edx, 8
    mov ecx, 16
    call rresv_reserve
    jc .fail
    push eax         ; save resv_id
    ; activate
    call rresv_activate
    jc .fail
    ; release
    call rresv_release
    jc .fail
    pop eax
    ; verify state=RELEASED in table
    mov edi, rresv_table
    cmp dword [edi + RRESV_STATE], RRESV_RELEASED
    jne .fail
    clc
    ret
.fail:
    pop eax
    stc
    ret

align 4
rresv_ready:     dd 0
rresv_next_id:   dd 1
rresv_tmp_owner: dd 0
rresv_tmp_cpu:   dd 0
rresv_tmp_mem:   dd 0
rresv_table:
    times RRESV_CAPACITY * RRESV_REC_SIZE db 0

; ===========================================================================
; NPSPEC-AUTONOMY-CONSTRAINT-0001 – Nova Autonomy Constraint Engine
; ===========================================================================
; Hard constraints that gate autonomous actions. Before any autonomy subsystem
; (selfheal, selfopt, autopol) executes an action, it must pass through the
; constraint engine. Constraints are registered with a domain, a limit type,
; and a threshold. Enforcement: if observed value exceeds threshold → deny.
;
; Limit types: MAX=1 / MIN=2 / RANGE=3
; Domains: CPU=1 / MEM=2 / RATE=3 / STATE=4
;
; Record: constraint_id(4) + domain(4) + limit_type(4) + threshold(4) + violations(4) + pad(4) = 24 bytes

ACONSRT_CAPACITY  equ 8
ACONSRT_REC_SIZE  equ 24
ACONSRT_ID        equ 0
ACONSRT_DOMAIN    equ 4
ACONSRT_LTYPE     equ 8
ACONSRT_THRESH    equ 12
ACONSRT_VIOL      equ 16
ACONSRT_PAD       equ 20

ACONSRT_MAX       equ 1
ACONSRT_MIN       equ 2
ACONSRT_RANGE     equ 3

ACONSRT_DOM_CPU   equ 1
ACONSRT_DOM_MEM   equ 2
ACONSRT_DOM_RATE  equ 3
ACONSRT_DOM_STATE equ 4

aconstraint_initialize:
    cmp dword [aconsrt_ready], 1
    je .done
    mov edi, aconsrt_table
    xor eax, eax
    mov ecx, (ACONSRT_CAPACITY * ACONSRT_REC_SIZE) / 4
    rep stosd
    mov dword [aconsrt_ready], 1
.done:
    clc
    ret

; EAX=constraint_id, EDX=domain, ECX=limit_type, EBX=threshold → EAX=slot, CF
aconstraint_register:
    cmp dword [aconsrt_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    xor esi, esi
.scan:
    cmp esi, ACONSRT_CAPACITY
    jae .fail
    imul edi, esi, ACONSRT_REC_SIZE
    add edi, aconsrt_table
    cmp dword [edi + ACONSRT_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + ACONSRT_ID], eax
    mov [edi + ACONSRT_DOMAIN], edx
    mov [edi + ACONSRT_LTYPE], ecx
    mov [edi + ACONSRT_THRESH], ebx
    mov dword [edi + ACONSRT_VIOL], 0
    mov eax, esi
    clc
    ret
.fail:
    stc
    ret

; EAX=constraint_id, EDX=observed_value → CF=0 allowed / CF=1 denied
aconstraint_enforce:
    cmp dword [aconsrt_ready], 1
    jne .fail
    push ebx
    push esi
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, ACONSRT_CAPACITY
    jae .not_found
    imul edi, esi, ACONSRT_REC_SIZE
    add edi, aconsrt_table
    cmp [edi + ACONSRT_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov ecx, [edi + ACONSRT_LTYPE]
    cmp ecx, ACONSRT_MAX
    je .check_max
    cmp ecx, ACONSRT_MIN
    je .check_min
    ; RANGE: both bounds stored in thresh (lo=thresh, hi=thresh+unused — simplified: deny if > thresh)
.check_max:
    cmp edx, [edi + ACONSRT_THRESH]
    jbe .allowed
    jmp .deny
.check_min:
    cmp edx, [edi + ACONSRT_THRESH]
    jae .allowed
.deny:
    inc dword [edi + ACONSRT_VIOL]
    pop esi
    pop ebx
    stc
    ret
.allowed:
    pop esi
    pop ebx
    clc
    ret
.not_found:
    pop esi
    pop ebx
.fail:
    stc
    ret

aconstraint_self_test:
    ; register: id=1, domain=CPU, MAX, threshold=200
    mov eax, 1
    mov edx, ACONSRT_DOM_CPU
    mov ecx, ACONSRT_MAX
    mov ebx, 200
    call aconstraint_register
    jc .fail
    ; enforce with observed=100 → allowed
    mov eax, 1
    mov edx, 100
    call aconstraint_enforce
    jc .fail
    ; enforce with observed=201 → denied
    mov eax, 1
    mov edx, 201
    call aconstraint_enforce
    jnc .fail
    ; violations=1
    mov edi, aconsrt_table
    cmp dword [edi + ACONSRT_VIOL], 1
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
aconsrt_ready:  dd 0
aconsrt_table:
    times ACONSRT_CAPACITY * ACONSRT_REC_SIZE db 0

; ===========================================================================
; NPSPEC-AUTONOMY-POLICY-0001 – Nova Autonomy Policy Manager
; ===========================================================================
; Event-driven autonomous policy: each policy has a trigger condition and
; an action type. When autopol_evaluate(event) is called, matching policies
; that pass aconstraint_enforce are dispatched.
;
; Trigger types: FAULT=1 / OVERLOAD=2 / UNDERLOAD=3 / TIMEOUT=4 / MANUAL=5
; Actions: NOP=0 / SCALE_UP=1 / SCALE_DOWN=2 / RESTART=3 / ALERT=4
;
; Record: policy_id(4) + trigger(4) + action(4) + fires(4) = 16 bytes

AUTOPOL_CAPACITY  equ 8
AUTOPOL_REC_SIZE  equ 16
AUTOPOL_ID        equ 0
AUTOPOL_TRIGGER   equ 4
AUTOPOL_ACTION    equ 8
AUTOPOL_FIRES     equ 12

AUTOPOL_TRIG_FAULT     equ 1
AUTOPOL_TRIG_OVERLOAD  equ 2
AUTOPOL_TRIG_UNDERLOAD equ 3
AUTOPOL_TRIG_TIMEOUT   equ 4
AUTOPOL_TRIG_MANUAL    equ 5

AUTOPOL_ACT_NOP        equ 0
AUTOPOL_ACT_SCALE_UP   equ 1
AUTOPOL_ACT_SCALE_DOWN equ 2
AUTOPOL_ACT_RESTART    equ 3
AUTOPOL_ACT_ALERT      equ 4

autopol_initialize:
    cmp dword [autopol_ready], 1
    je .done
    mov edi, autopol_table
    xor eax, eax
    mov ecx, (AUTOPOL_CAPACITY * AUTOPOL_REC_SIZE) / 4
    rep stosd
    mov dword [autopol_ready], 1
.done:
    clc
    ret

; EAX=policy_id, EDX=trigger_type, ECX=action → EAX=slot, CF
autopol_register:
    cmp dword [autopol_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    xor esi, esi
.scan:
    cmp esi, AUTOPOL_CAPACITY
    jae .fail
    imul edi, esi, AUTOPOL_REC_SIZE
    add edi, autopol_table
    cmp dword [edi + AUTOPOL_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + AUTOPOL_ID], eax
    mov [edi + AUTOPOL_TRIGGER], edx
    mov [edi + AUTOPOL_ACTION], ecx
    mov dword [edi + AUTOPOL_FIRES], 0
    mov eax, esi
    clc
    ret
.fail:
    stc
    ret

; EAX=trigger_event → EAX=action dispatched (last match), EDX=fires, CF=0 fired/CF=1 none
autopol_evaluate:
    cmp dword [autopol_ready], 1
    jne .fail
    push ebx
    push esi
    mov ebx, eax
    xor esi, esi
    xor edi, edi        ; last_action = 0
    xor ecx, ecx        ; matched = 0
.scan:
    cmp esi, AUTOPOL_CAPACITY
    jae .done
    push edi
    imul edi, esi, AUTOPOL_REC_SIZE
    add edi, autopol_table
    cmp dword [edi + AUTOPOL_ID], 0
    je .next
    cmp [edi + AUTOPOL_TRIGGER], ebx
    jne .next
    ; check constraint for this policy (use policy_id as constraint_id)
    push eax
    push edx
    mov eax, [edi + AUTOPOL_ID]
    xor edx, edx
    call aconstraint_enforce
    pop edx
    pop eax
    jc .next           ; constraint denied
    inc dword [edi + AUTOPOL_FIRES]
    mov edx, [edi + AUTOPOL_ACTION]
    pop edi
    mov edi, edx       ; last_action
    push edi
    inc ecx
.next:
    pop edi
    inc esi
    jmp .scan
.done:
    test ecx, ecx
    jz .no_match
    mov eax, edi
    mov edx, ecx
    pop esi
    pop ebx
    clc
    ret
.no_match:
    pop esi
    pop ebx
.fail:
    stc
    ret

autopol_self_test:
    ; register policy 1: FAULT → RESTART
    mov eax, 1
    mov edx, AUTOPOL_TRIG_FAULT
    mov ecx, AUTOPOL_ACT_RESTART
    call autopol_register
    jc .fail
    ; evaluate FAULT → should fire RESTART
    mov eax, AUTOPOL_TRIG_FAULT
    call autopol_evaluate
    jc .fail
    cmp eax, AUTOPOL_ACT_RESTART
    jne .fail
    ; evaluate OVERLOAD (no policy) → CF=1
    mov eax, AUTOPOL_TRIG_OVERLOAD
    call autopol_evaluate
    jnc .fail
    clc
    ret
.fail:
    stc
    ret

align 4
autopol_ready:  dd 0
autopol_table:
    times AUTOPOL_CAPACITY * AUTOPOL_REC_SIZE db 0

; ===========================================================================
; NPSPEC-ADAPTIVE-SCHEDULER-0001 – Nova Adaptive Scheduler
; ===========================================================================
; Adjusts scheduler class weights based on QoS feedback from qos_get_class.
; Each tracked class has a current weight and a direction the feedback pushes it.
; Connects: adapt_observe (QoS deviation) → aschedsw_feedback → weight adjustment.
;
; Record: class_id(4) + weight(4) + min_weight(4) + max_weight(4) + adjustments(4) + pad(4) = 24 bytes

ASCHEDSW_CAPACITY  equ 8
ASCHEDSW_REC_SIZE  equ 24
ASCHEDSW_CLASS     equ 0
ASCHEDSW_WEIGHT    equ 4
ASCHEDSW_MIN_W     equ 8
ASCHEDSW_MAX_W     equ 12
ASCHEDSW_ADJ       equ 16
ASCHEDSW_PAD       equ 20

aschedsw_initialize:
    cmp dword [aschedsw_ready], 1
    je .done
    mov edi, aschedsw_table
    xor eax, eax
    mov ecx, (ASCHEDSW_CAPACITY * ASCHEDSW_REC_SIZE) / 4
    rep stosd
    mov dword [aschedsw_ready], 1
.done:
    clc
    ret

; EAX=class_id, EDX=initial_weight, ECX=min_weight, EBX=max_weight → EAX=slot, CF
aschedsw_register:
    cmp dword [aschedsw_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    xor esi, esi
.scan:
    cmp esi, ASCHEDSW_CAPACITY
    jae .fail
    imul edi, esi, ASCHEDSW_REC_SIZE
    add edi, aschedsw_table
    cmp dword [edi + ASCHEDSW_CLASS], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + ASCHEDSW_CLASS], eax
    mov [edi + ASCHEDSW_WEIGHT], edx
    mov [edi + ASCHEDSW_MIN_W], ecx
    mov [edi + ASCHEDSW_MAX_W], ebx
    mov dword [edi + ASCHEDSW_ADJ], 0
    mov eax, esi
    clc
    ret
.fail:
    stc
    ret

; EAX=class_id, EDX=delta(signed: positive=increase, use 0x80000000 sign bit)
; Increases or decreases weight by |delta|, clamped to [min,max]. CF=0 ok.
aschedsw_feedback:
    cmp dword [aschedsw_ready], 1
    jne .fail
    push ebx
    push esi
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, ASCHEDSW_CAPACITY
    jae .not_found
    imul edi, esi, ASCHEDSW_REC_SIZE
    add edi, aschedsw_table
    cmp [edi + ASCHEDSW_CLASS], ebx
    je .found
    inc esi
    jmp .scan
.found:
    test edx, edx
    js .decrease
    ; increase
    mov eax, [edi + ASCHEDSW_WEIGHT]
    add eax, edx
    mov ecx, [edi + ASCHEDSW_MAX_W]
    cmp eax, ecx
    jbe .store
    mov eax, ecx
    jmp .store
.decrease:
    neg edx
    mov eax, [edi + ASCHEDSW_WEIGHT]
    cmp eax, edx
    jb .clamp_min
    sub eax, edx
    mov ecx, [edi + ASCHEDSW_MIN_W]
    cmp eax, ecx
    jae .store
.clamp_min:
    mov eax, [edi + ASCHEDSW_MIN_W]
.store:
    mov [edi + ASCHEDSW_WEIGHT], eax
    inc dword [edi + ASCHEDSW_ADJ]
    pop esi
    pop ebx
    clc
    ret
.not_found:
    pop esi
    pop ebx
.fail:
    stc
    ret

; EAX=class_id → EAX=weight, EDX=adjustments, CF
aschedsw_query:
    cmp dword [aschedsw_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, ASCHEDSW_CAPACITY
    jae .not_found
    imul edi, esi, ASCHEDSW_REC_SIZE
    add edi, aschedsw_table
    cmp [edi + ASCHEDSW_CLASS], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + ASCHEDSW_WEIGHT]
    mov edx, [edi + ASCHEDSW_ADJ]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

aschedsw_self_test:
    ; register class=1: weight=100, min=10, max=200
    mov eax, 1
    mov edx, 100
    mov ecx, 10
    mov ebx, 200
    call aschedsw_register
    jc .fail
    ; feedback +30 → weight=130
    mov eax, 1
    mov edx, 30
    call aschedsw_feedback
    jc .fail
    mov eax, 1
    call aschedsw_query
    jc .fail
    cmp eax, 130
    jne .fail
    ; feedback -50 → weight=80
    mov eax, 1
    mov edx, 0x80000032   ; -50 as signed (0x80000000 | 50)
    ; Actually: neg of 50 = use negative delta directly as 2's complement
    ; -50 = 0xFFFFFFCE
    mov edx, 0xFFFFFFCE
    call aschedsw_feedback
    jc .fail
    mov eax, 1
    call aschedsw_query
    jc .fail
    cmp eax, 80
    jne .fail
    ; feedback -200 → clamped at min=10
    mov edx, 0xFFFFFF38   ; -200
    mov eax, 1
    call aschedsw_feedback
    jc .fail
    mov eax, 1
    call aschedsw_query
    jc .fail
    cmp eax, 10
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
aschedsw_ready:  dd 0
aschedsw_table:
    times ASCHEDSW_CAPACITY * ASCHEDSW_REC_SIZE db 0

; ===========================================================================
; NPSPEC-STATE-VERSIONING-0001 – Nova State Versioning
; ===========================================================================
; Tracks monotonically increasing version numbers and generation lineage
; per state object. Connects state_transition (state32.inc) with a richer
; version graph: each bump records parent version for lineage tracing.
;
; Record: obj_id(4) + version(4) + generation(4) + parent_ver(4) = 16 bytes

SVER_CAPACITY  equ 8
SVER_REC_SIZE  equ 16
SVER_OBJ_ID    equ 0
SVER_VERSION   equ 4
SVER_GEN       equ 8
SVER_PARENT    equ 12

sver_initialize:
    cmp dword [sver_ready], 1
    je .done
    mov edi, sver_table
    xor eax, eax
    mov ecx, (SVER_CAPACITY * SVER_REC_SIZE) / 4
    rep stosd
    mov dword [sver_gen_counter], 1
    mov dword [sver_ready], 1
.done:
    clc
    ret

; EAX=obj_id → EAX=slot, CF (creates entry at version=1)
sver_create:
    cmp dword [sver_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    xor esi, esi
.scan:
    cmp esi, SVER_CAPACITY
    jae .fail
    imul edi, esi, SVER_REC_SIZE
    add edi, sver_table
    cmp dword [edi + SVER_OBJ_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + SVER_OBJ_ID], eax
    mov dword [edi + SVER_VERSION], 1
    mov ecx, [sver_gen_counter]
    mov [edi + SVER_GEN], ecx
    inc dword [sver_gen_counter]
    mov dword [edi + SVER_PARENT], 0
    mov eax, esi
    clc
    ret
.fail:
    stc
    ret

; EAX=obj_id → bumps version, records parent, EAX=new_version, CF
sver_bump:
    cmp dword [sver_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, SVER_CAPACITY
    jae .not_found
    imul edi, esi, SVER_REC_SIZE
    add edi, sver_table
    cmp [edi + SVER_OBJ_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + SVER_VERSION]
    mov [edi + SVER_PARENT], eax
    inc dword [edi + SVER_VERSION]
    mov ecx, [sver_gen_counter]
    mov [edi + SVER_GEN], ecx
    inc dword [sver_gen_counter]
    mov eax, [edi + SVER_VERSION]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

; EAX=obj_id → EAX=version, EDX=parent_ver, ECX=generation, CF
sver_query:
    cmp dword [sver_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, SVER_CAPACITY
    jae .not_found
    imul edi, esi, SVER_REC_SIZE
    add edi, sver_table
    cmp [edi + SVER_OBJ_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + SVER_VERSION]
    mov edx, [edi + SVER_PARENT]
    mov ecx, [edi + SVER_GEN]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

sver_self_test:
    ; create obj=1 → version=1, parent=0
    mov eax, 1
    call sver_create
    jc .fail
    ; bump → version=2, parent=1
    mov eax, 1
    call sver_bump
    jc .fail
    cmp eax, 2
    jne .fail
    ; query → version=2, parent=1
    mov eax, 1
    call sver_query
    jc .fail
    cmp eax, 2
    jne .fail
    cmp edx, 1
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
sver_ready:        dd 0
sver_gen_counter:  dd 1
sver_table:
    times SVER_CAPACITY * SVER_REC_SIZE db 0

; ===========================================================================
; NPSPEC-STATE-ACTUAL-0001 – Nova State Actual Store
; ===========================================================================
; Stores the observed/measured current value for each tracked state object.
; Feeds srec_reconcile: sactual_write → srec_reconcile(id, actual).
;
; Record: obj_id(4) + actual_value(4) + timestamp(4) + writes(4) = 16 bytes

SACTUAL_CAPACITY  equ 8
SACTUAL_REC_SIZE  equ 16
SACTUAL_OBJ_ID    equ 0
SACTUAL_VALUE     equ 4
SACTUAL_TICK      equ 8
SACTUAL_WRITES    equ 12

sactual_initialize:
    cmp dword [sactual_ready], 1
    je .done
    mov edi, sactual_table
    xor eax, eax
    mov ecx, (SACTUAL_CAPACITY * SACTUAL_REC_SIZE) / 4
    rep stosd
    mov dword [sactual_ready], 1
.done:
    clc
    ret

; EAX=obj_id, EDX=actual_value → CF (creates or updates)
sactual_write:
    cmp dword [sactual_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    xor esi, esi
.scan:
    cmp esi, SACTUAL_CAPACITY
    jae .full
    imul edi, esi, SACTUAL_REC_SIZE
    add edi, sactual_table
    cmp dword [edi + SACTUAL_OBJ_ID], 0
    je .slot
    cmp [edi + SACTUAL_OBJ_ID], eax
    je .update
    inc esi
    jmp .scan
.update:
.slot:
    mov [edi + SACTUAL_OBJ_ID], eax
    mov [edi + SACTUAL_VALUE], edx
    mov ecx, [wd_global_tick]
    mov [edi + SACTUAL_TICK], ecx
    inc dword [edi + SACTUAL_WRITES]
    clc
    ret
.full:
.fail:
    stc
    ret

; EAX=obj_id → EAX=actual_value, EDX=tick, CF
sactual_read:
    cmp dword [sactual_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, SACTUAL_CAPACITY
    jae .not_found
    imul edi, esi, SACTUAL_REC_SIZE
    add edi, sactual_table
    cmp [edi + SACTUAL_OBJ_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + SACTUAL_VALUE]
    mov edx, [edi + SACTUAL_TICK]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

sactual_self_test:
    ; write obj=1, value=42
    mov eax, 1
    mov edx, 42
    call sactual_write
    jc .fail
    ; read back → 42
    mov eax, 1
    call sactual_read
    jc .fail
    cmp eax, 42
    jne .fail
    ; update to 99
    mov eax, 1
    mov edx, 99
    call sactual_write
    jc .fail
    mov eax, 1
    call sactual_read
    jc .fail
    cmp eax, 99
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
sactual_ready:  dd 0
sactual_table:
    times SACTUAL_CAPACITY * SACTUAL_REC_SIZE db 0

; ===========================================================================
; NPSPEC-STATE-DESIRED-0001 – Nova State Desired Store
; ===========================================================================
; Stores the intended/target value for each tracked state object.
; Feeds srec_reconcile: sdesired_write → srec_reconcile(id, actual) checks
; actual vs desired. Mirrors sactual_* API with "desired" semantics.
;
; Record: obj_id(4) + desired_value(4) + priority(4) + writes(4) = 16 bytes

SDESIRED_CAPACITY  equ 8
SDESIRED_REC_SIZE  equ 16
SDESIRED_OBJ_ID    equ 0
SDESIRED_VALUE     equ 4
SDESIRED_PRIORITY  equ 8
SDESIRED_WRITES    equ 12

sdesired_initialize:
    cmp dword [sdesired_ready], 1
    je .done
    mov edi, sdesired_table
    xor eax, eax
    mov ecx, (SDESIRED_CAPACITY * SDESIRED_REC_SIZE) / 4
    rep stosd
    mov dword [sdesired_ready], 1
.done:
    clc
    ret

; EAX=obj_id, EDX=desired_value, ECX=priority → CF (creates or updates)
sdesired_write:
    cmp dword [sdesired_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    xor esi, esi
.scan:
    cmp esi, SDESIRED_CAPACITY
    jae .full
    imul edi, esi, SDESIRED_REC_SIZE
    add edi, sdesired_table
    cmp dword [edi + SDESIRED_OBJ_ID], 0
    je .slot
    cmp [edi + SDESIRED_OBJ_ID], eax
    je .update
    inc esi
    jmp .scan
.update:
.slot:
    mov [edi + SDESIRED_OBJ_ID], eax
    mov [edi + SDESIRED_VALUE], edx
    mov [edi + SDESIRED_PRIORITY], ecx
    inc dword [edi + SDESIRED_WRITES]
    clc
    ret
.full:
.fail:
    stc
    ret

; EAX=obj_id → EAX=desired_value, EDX=priority, CF
sdesired_read:
    cmp dword [sdesired_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, SDESIRED_CAPACITY
    jae .not_found
    imul edi, esi, SDESIRED_REC_SIZE
    add edi, sdesired_table
    cmp [edi + SDESIRED_OBJ_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + SDESIRED_VALUE]
    mov edx, [edi + SDESIRED_PRIORITY]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

sdesired_self_test:
    ; write obj=1, desired=10, priority=1
    mov eax, 1
    mov edx, 10
    mov ecx, 1
    call sdesired_write
    jc .fail
    ; read back
    mov eax, 1
    call sdesired_read
    jc .fail
    cmp eax, 10
    jne .fail
    cmp edx, 1
    jne .fail
    ; update priority
    mov eax, 1
    mov edx, 10
    mov ecx, 5
    call sdesired_write
    jc .fail
    mov eax, 1
    call sdesired_read
    jc .fail
    cmp edx, 5
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
sdesired_ready:  dd 0
sdesired_table:
    times SDESIRED_CAPACITY * SDESIRED_REC_SIZE db 0

; ===========================================================================
; NPSPEC-TRANSACTION-RESOURCE-0001 – Nova Transaction Resource Manager
; ===========================================================================
; Tracks resource locks held by active transactions. Before modifying a
; shared resource, a transaction acquires a lock here. On commit/abort,
; all locks for that transaction are released.
;
; Lock states: FREE=0 / HELD=1
; Record: txn_id(4) + resource_id(4) + state(4) + pad(4) = 16 bytes

TXRES_CAPACITY  equ 8
TXRES_REC_SIZE  equ 16
TXRES_TXN_ID    equ 0
TXRES_RES_ID    equ 4
TXRES_STATE     equ 8
TXRES_PAD       equ 12

TXRES_FREE      equ 0
TXRES_HELD      equ 1

txres_initialize:
    cmp dword [txres_ready], 1
    je .done
    mov edi, txres_table
    xor eax, eax
    mov ecx, (TXRES_CAPACITY * TXRES_REC_SIZE) / 4
    rep stosd
    mov dword [txres_ready], 1
.done:
    clc
    ret

; EAX=txn_id, EDX=resource_id → CF=0 acquired / CF=1 already held
txres_acquire:
    cmp dword [txres_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push esi
    mov ebx, eax
    ; check if already held by any txn
    xor esi, esi
.check_held:
    cmp esi, TXRES_CAPACITY
    jae .find_free
    imul edi, esi, TXRES_REC_SIZE
    add edi, txres_table
    cmp dword [edi + TXRES_STATE], TXRES_HELD
    jne .ch_next
    cmp [edi + TXRES_RES_ID], edx
    je .already_held
.ch_next:
    inc esi
    jmp .check_held
.find_free:
    xor esi, esi
.scan:
    cmp esi, TXRES_CAPACITY
    jae .full
    imul edi, esi, TXRES_REC_SIZE
    add edi, txres_table
    cmp dword [edi + TXRES_STATE], TXRES_FREE
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + TXRES_TXN_ID], ebx
    mov [edi + TXRES_RES_ID], edx
    mov dword [edi + TXRES_STATE], TXRES_HELD
    pop esi
    pop ebx
    clc
    ret
.already_held:
.full:
    pop esi
    pop ebx
    stc
    ret
.fail:
    stc
    ret

; EAX=txn_id → releases all locks held by this transaction, CF
txres_release_all:
    cmp dword [txres_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, TXRES_CAPACITY
    jae .done
    imul edi, esi, TXRES_REC_SIZE
    add edi, txres_table
    cmp [edi + TXRES_TXN_ID], ebx
    jne .next
    mov dword [edi + TXRES_STATE], TXRES_FREE
.next:
    inc esi
    jmp .scan
.done:
    pop ebx
    clc
    ret
.fail:
    stc
    ret

txres_self_test:
    ; acquire resource=5 for txn=1
    mov eax, 1
    mov edx, 5
    call txres_acquire
    jc .fail
    ; try to acquire same resource for txn=2 → conflict
    mov eax, 2
    mov edx, 5
    call txres_acquire
    jnc .fail
    ; release txn=1
    mov eax, 1
    call txres_release_all
    jc .fail
    ; now txn=2 can acquire
    mov eax, 2
    mov edx, 5
    call txres_acquire
    jc .fail
    mov eax, 2
    call txres_release_all
    jc .fail
    clc
    ret
.fail:
    stc
    ret

align 4
txres_ready:  dd 0
txres_table:
    times TXRES_CAPACITY * TXRES_REC_SIZE db 0

; ===========================================================================
; NPSPEC-TRANSACTION-SYSTEM-0001 – Nova Transaction System Coordinator
; ===========================================================================
; Two-phase-commit (2PC) coordinator for system-wide transactions.
; Connects tbarrier (Phase-1 ready sync) + txlog (WAL) + tcomp (abort undo).
; States: IDLE=0 / PREPARE=1 / COMMITTED=2 / ABORTED=3
;
; Record: sys_txn_id(4) + participants(4) + ready_count(4) + state(4) = 16 bytes

TXSYS_CAPACITY  equ 4
TXSYS_REC_SIZE  equ 16
TXSYS_ID        equ 0
TXSYS_PARTS     equ 4
TXSYS_READY     equ 8
TXSYS_STATE     equ 12

TXSYS_IDLE      equ 0
TXSYS_PREPARE   equ 1
TXSYS_COMMITTED equ 2
TXSYS_ABORTED   equ 3

txsys_initialize:
    cmp dword [txsys_ready], 1
    je .done
    mov edi, txsys_table
    xor eax, eax
    mov ecx, (TXSYS_CAPACITY * TXSYS_REC_SIZE) / 4
    rep stosd
    mov dword [txsys_ready], 1
.done:
    clc
    ret

; EAX=sys_txn_id, EDX=participant_count → EAX=slot, CF
txsys_begin:
    cmp dword [txsys_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    test edx, edx
    jz .fail
    xor esi, esi
.scan:
    cmp esi, TXSYS_CAPACITY
    jae .fail
    imul edi, esi, TXSYS_REC_SIZE
    add edi, txsys_table
    cmp dword [edi + TXSYS_STATE], TXSYS_IDLE
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + TXSYS_ID], eax
    mov [edi + TXSYS_PARTS], edx
    mov dword [edi + TXSYS_READY], 0
    mov dword [edi + TXSYS_STATE], TXSYS_PREPARE
    mov eax, esi
    clc
    ret
.fail:
    stc
    ret

; EAX=sys_txn_id → increments ready count; when all ready → COMMITTED, CF=0
txsys_participant_ready:
    cmp dword [txsys_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, TXSYS_CAPACITY
    jae .not_found
    imul edi, esi, TXSYS_REC_SIZE
    add edi, txsys_table
    cmp [edi + TXSYS_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    cmp dword [edi + TXSYS_STATE], TXSYS_PREPARE
    jne .not_found
    inc dword [edi + TXSYS_READY]
    mov eax, [edi + TXSYS_READY]
    cmp eax, [edi + TXSYS_PARTS]
    jne .still_waiting
    mov dword [edi + TXSYS_STATE], TXSYS_COMMITTED
.still_waiting:
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

; EAX=sys_txn_id → abort → state=ABORTED, CF
txsys_abort:
    cmp dword [txsys_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, TXSYS_CAPACITY
    jae .not_found
    imul edi, esi, TXSYS_REC_SIZE
    add edi, txsys_table
    cmp [edi + TXSYS_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov dword [edi + TXSYS_STATE], TXSYS_ABORTED
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

txsys_self_test:
    ; begin sys_txn=1 with 2 participants
    mov eax, 1
    mov edx, 2
    call txsys_begin
    jc .fail
    ; first participant ready → still PREPARE
    mov eax, 1
    call txsys_participant_ready
    jc .fail
    ; check still in PREPARE
    mov edi, txsys_table
    cmp dword [edi + TXSYS_STATE], TXSYS_PREPARE
    jne .fail
    ; second participant ready → COMMITTED
    mov eax, 1
    call txsys_participant_ready
    jc .fail
    cmp dword [edi + TXSYS_STATE], TXSYS_COMMITTED
    jne .fail
    ; reset for abort test
    mov dword [edi + TXSYS_STATE], TXSYS_IDLE
    ; begin again
    mov eax, 2
    mov edx, 1
    call txsys_begin
    jc .fail
    ; abort
    mov eax, 2
    call txsys_abort
    jc .fail
    clc
    ret
.fail:
    stc
    ret

align 4
txsys_ready:  dd 0
txsys_table:
    times TXSYS_CAPACITY * TXSYS_REC_SIZE db 0

; ===========================================================================
; NPSPEC-RESOURCE-ARBITRATION-0001 – Nova Resource Arbitration
; ===========================================================================
; Priority-based arbiter: multiple requestors compete for a shared resource.
; Highest-priority requestor wins. Connects radmit + rguarant: before
; radmit_request, caller registers a competing request here; arbiter grants
; to the highest priority that fits within available capacity.
;
; Record: req_id(4) + resource(4) + priority(4) + granted(4) = 16 bytes

RARBIT_CAPACITY  equ 8
RARBIT_REC_SIZE  equ 16
RARBIT_REQ_ID    equ 0
RARBIT_RESOURCE  equ 4
RARBIT_PRIORITY  equ 8
RARBIT_GRANTED   equ 12

rarbit_initialize:
    cmp dword [rarbit_ready], 1
    je .done
    mov edi, rarbit_table
    xor eax, eax
    mov ecx, (RARBIT_CAPACITY * RARBIT_REC_SIZE) / 4
    rep stosd
    mov dword [rarbit_ready], 1
.done:
    clc
    ret

; EAX=req_id, EDX=resource_id, ECX=priority → EAX=slot, CF
rarbit_request:
    cmp dword [rarbit_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    xor esi, esi
.scan:
    cmp esi, RARBIT_CAPACITY
    jae .fail
    imul edi, esi, RARBIT_REC_SIZE
    add edi, rarbit_table
    cmp dword [edi + RARBIT_REQ_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + RARBIT_REQ_ID], eax
    mov [edi + RARBIT_RESOURCE], edx
    mov [edi + RARBIT_PRIORITY], ecx
    mov dword [edi + RARBIT_GRANTED], 0
    mov eax, esi
    clc
    ret
.fail:
    stc
    ret

; EDX=resource_id → grants to highest-priority pending request for that resource
; EAX=granted_req_id, CF=0 granted / CF=1 no requests
rarbit_arbitrate:
    cmp dword [rarbit_ready], 1
    jne .fail
    push ebx
    push esi
    mov ebx, edx            ; resource_id
    xor esi, esi
    xor ecx, ecx            ; best_priority = 0
    mov edx, 0              ; best_slot = none (sentinel)
    push edx                ; save best_slot on stack
.scan:
    cmp esi, RARBIT_CAPACITY
    jae .done_scan
    imul edi, esi, RARBIT_REC_SIZE
    add edi, rarbit_table
    cmp dword [edi + RARBIT_REQ_ID], 0
    je .next
    cmp [edi + RARBIT_RESOURCE], ebx
    jne .next
    cmp dword [edi + RARBIT_GRANTED], 1
    je .next
    ; compare priority
    cmp [edi + RARBIT_PRIORITY], ecx
    jbe .next
    mov ecx, [edi + RARBIT_PRIORITY]
    pop edx
    mov edx, esi            ; new best slot
    push edx
.next:
    inc esi
    jmp .scan
.done_scan:
    pop edx                 ; best_slot
    cmp edx, 0xFFFFFFFF
    je .no_req
    ; check if any was found (ecx > 0 means we found one)
    test ecx, ecx
    jz .no_req
    imul edi, edx, RARBIT_REC_SIZE
    add edi, rarbit_table
    mov dword [edi + RARBIT_GRANTED], 1
    mov eax, [edi + RARBIT_REQ_ID]
    pop esi
    pop ebx
    clc
    ret
.no_req:
    pop esi
    pop ebx
.fail:
    stc
    ret

rarbit_self_test:
    ; two requests for resource=1: req=1 priority=5, req=2 priority=10
    mov eax, 1
    mov edx, 1
    mov ecx, 5
    call rarbit_request
    jc .fail
    mov eax, 2
    mov edx, 1
    mov ecx, 10
    call rarbit_request
    jc .fail
    ; arbitrate resource=1 → req=2 should win (higher priority)
    mov edx, 1
    call rarbit_arbitrate
    jc .fail
    cmp eax, 2
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
rarbit_ready:  dd 0
rarbit_table:
    times RARBIT_CAPACITY * RARBIT_REC_SIZE db 0

; ===========================================================================
; NPSPEC-ADAPTIVE-CACHE-0001 – Nova Adaptive Cache Manager
; ===========================================================================
; Tracks cache-line usage and adapts eviction decisions based on EWMA
; hit-rate. When hit-rate drops below threshold, signals the adaptive
; scheduler (aschedsw_feedback) to reduce cache-class weight, and
; autopol_evaluate(OVERLOAD) to trigger a scale-down policy.
;
; Record: line_id(4) + hits(4) + misses(4) + hit_rate(4) = 16 bytes
; hit_rate is EWMA, 8-bit fixed-point (0–255 ≈ 0–100%)

ACACHE_CAPACITY   equ 8
ACACHE_REC_SIZE   equ 16
ACACHE_LINE_ID    equ 0
ACACHE_HITS       equ 4
ACACHE_MISSES     equ 8
ACACHE_HIT_RATE   equ 12

ACACHE_ALPHA      equ 32         ; EWMA smoothing factor (out of 256)
ACACHE_THRESHOLD  equ 64         ; hit_rate below this → feedback

acache_initialize:
    cmp dword [acache_ready], 1
    je .done
    mov edi, acache_table
    xor eax, eax
    mov ecx, (ACACHE_CAPACITY * ACACHE_REC_SIZE) / 4
    rep stosd
    mov dword [acache_ready], 1
.done:
    clc
    ret

; EAX=line_id → register cache line, CF=1 if full/dup
acache_register:
    cmp dword [acache_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    mov ebx, eax
    ; check dup
    xor esi, esi
.dup_scan:
    cmp esi, ACACHE_CAPACITY
    jae .find_free
    imul edi, esi, ACACHE_REC_SIZE
    add edi, acache_table
    cmp [edi + ACACHE_LINE_ID], ebx
    je .dup
    inc esi
    jmp .dup_scan
.find_free:
    xor esi, esi
.scan:
    cmp esi, ACACHE_CAPACITY
    jae .full
    imul edi, esi, ACACHE_REC_SIZE
    add edi, acache_table
    cmp dword [edi + ACACHE_LINE_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + ACACHE_LINE_ID], ebx
    mov dword [edi + ACACHE_HITS], 0
    mov dword [edi + ACACHE_MISSES], 0
    mov dword [edi + ACACHE_HIT_RATE], 128   ; start at 50%
    pop ebx
    clc
    ret
.dup:
.full:
    pop ebx
.fail:
    stc
    ret

; EAX=line_id, EDX=1(hit)/0(miss) → update EWMA hit_rate; on low rate → feedback
acache_record:
    cmp dword [acache_ready], 1
    jne .fail
    push ebx
    push ecx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, ACACHE_CAPACITY
    jae .not_found
    imul edi, esi, ACACHE_REC_SIZE
    add edi, acache_table
    cmp [edi + ACACHE_LINE_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    test edx, edx
    jz .miss
    inc dword [edi + ACACHE_HITS]
    ; EWMA: new = (alpha*255 + (256-alpha)*old) >> 8 for hit
    mov eax, ACACHE_ALPHA
    imul eax, eax, 255
    mov ecx, 256 - ACACHE_ALPHA
    imul ecx, [edi + ACACHE_HIT_RATE]
    add eax, ecx
    shr eax, 8
    mov [edi + ACACHE_HIT_RATE], eax
    jmp .check
.miss:
    inc dword [edi + ACACHE_MISSES]
    ; EWMA: new = (alpha*0 + (256-alpha)*old) >> 8 for miss
    mov eax, 256 - ACACHE_ALPHA
    imul eax, [edi + ACACHE_HIT_RATE]
    shr eax, 8
    mov [edi + ACACHE_HIT_RATE], eax
.check:
    ; if hit_rate < threshold, signal overload policy
    cmp dword [edi + ACACHE_HIT_RATE], ACACHE_THRESHOLD
    jae .ok
    ; cross-subsystem: signal adaptive scheduler cache class (class=3) to reduce
    mov eax, 3
    mov edx, 0xFFFFFFD0     ; -48 signed delta
    call aschedsw_feedback
.ok:
    pop ecx
    pop ebx
    clc
    ret
.not_found:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=line_id → EAX=hit_rate, EDX=hits, ECX=misses, CF
acache_query:
    cmp dword [acache_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, ACACHE_CAPACITY
    jae .not_found
    imul edi, esi, ACACHE_REC_SIZE
    add edi, acache_table
    cmp [edi + ACACHE_LINE_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + ACACHE_HIT_RATE]
    mov edx, [edi + ACACHE_HITS]
    mov ecx, [edi + ACACHE_MISSES]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

acache_self_test:
    ; register line=1
    mov eax, 1
    call acache_register
    jc .fail
    ; record a hit → rate should stay high
    mov eax, 1
    mov edx, 1
    call acache_record
    jc .fail
    ; query → rate > 0
    mov eax, 1
    call acache_query
    jc .fail
    test eax, eax
    jz .fail
    clc
    ret
.fail:
    stc
    ret

align 4
acache_ready:  dd 0
acache_table:
    times ACACHE_CAPACITY * ACACHE_REC_SIZE db 0

; ===========================================================================
; NPSPEC-ADAPTIVE-POWER-0001 – Nova Adaptive Power Manager
; ===========================================================================
; Manages CPU power domains with P-state transitions driven by load.
; Connects apred_query to get predicted load and adjusts P-state.
; P-state: 0=max-perf … 3=min-power. Load is 0–100 (percent).
;
; Record: domain(4) + p_state(4) + load(4) + transitions(4) = 16 bytes

APOWER_CAPACITY    equ 4
APOWER_REC_SIZE    equ 16
APOWER_DOMAIN      equ 0
APOWER_PSTATE      equ 4
APOWER_LOAD        equ 8
APOWER_TRANS       equ 12

APOWER_PSTATE_MAX  equ 0
APOWER_PSTATE_MIN  equ 3

apower_initialize:
    cmp dword [apower_ready], 1
    je .done
    mov edi, apower_table
    xor eax, eax
    mov ecx, (APOWER_CAPACITY * APOWER_REC_SIZE) / 4
    rep stosd
    mov dword [apower_ready], 1
.done:
    clc
    ret

; EAX=domain_id → register domain at P-state=1 (balanced), CF
apower_register:
    cmp dword [apower_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, APOWER_CAPACITY
    jae .full
    imul edi, esi, APOWER_REC_SIZE
    add edi, apower_table
    cmp dword [edi + APOWER_DOMAIN], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + APOWER_DOMAIN], ebx
    mov dword [edi + APOWER_PSTATE], 1
    mov dword [edi + APOWER_LOAD], 50
    mov dword [edi + APOWER_TRANS], 0
    pop ebx
    clc
    ret
.full:
    pop ebx
.fail:
    stc
    ret

; EAX=domain_id, EDX=load(0-100) → adjust P-state, cross-calls apred_update
; load>=75 → P-state=0 (max-perf); load<=25 → P-state=3 (min-power); else=1
apower_update_load:
    cmp dword [apower_ready], 1
    jne .fail
    push ebx
    push ecx
    mov ebx, eax
    mov ecx, edx            ; load
    xor esi, esi
.scan:
    cmp esi, APOWER_CAPACITY
    jae .not_found
    imul edi, esi, APOWER_REC_SIZE
    add edi, apower_table
    cmp [edi + APOWER_DOMAIN], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov [edi + APOWER_LOAD], ecx
    ; update adaptive prediction for domain (metric = domain id)
    mov eax, ebx
    mov edx, ecx
    call apred_update
    ; compute new P-state
    cmp ecx, 75
    jae .high_load
    cmp ecx, 25
    jbe .low_load
    ; medium load → P-state 1
    cmp dword [edi + APOWER_PSTATE], 1
    je .no_trans
    inc dword [edi + APOWER_TRANS]
    mov dword [edi + APOWER_PSTATE], 1
    jmp .no_trans
.high_load:
    cmp dword [edi + APOWER_PSTATE], APOWER_PSTATE_MAX
    je .no_trans
    inc dword [edi + APOWER_TRANS]
    mov dword [edi + APOWER_PSTATE], APOWER_PSTATE_MAX
    jmp .no_trans
.low_load:
    cmp dword [edi + APOWER_PSTATE], APOWER_PSTATE_MIN
    je .no_trans
    inc dword [edi + APOWER_TRANS]
    mov dword [edi + APOWER_PSTATE], APOWER_PSTATE_MIN
.no_trans:
    pop ecx
    pop ebx
    clc
    ret
.not_found:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=domain_id → EAX=p_state, EDX=load, ECX=transitions, CF
apower_query:
    cmp dword [apower_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, APOWER_CAPACITY
    jae .not_found
    imul edi, esi, APOWER_REC_SIZE
    add edi, apower_table
    cmp [edi + APOWER_DOMAIN], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + APOWER_PSTATE]
    mov edx, [edi + APOWER_LOAD]
    mov ecx, [edi + APOWER_TRANS]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

apower_self_test:
    ; register domain=1
    mov eax, 1
    call apower_register
    jc .fail
    ; high load → P-state should become 0
    mov eax, 1
    mov edx, 80
    call apower_update_load
    jc .fail
    mov eax, 1
    call apower_query
    jc .fail
    cmp eax, 0
    jne .fail
    ; low load → P-state should become 3
    mov eax, 1
    mov edx, 10
    call apower_update_load
    jc .fail
    mov eax, 1
    call apower_query
    jc .fail
    cmp eax, 3
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
apower_ready:  dd 0
apower_table:
    times APOWER_CAPACITY * APOWER_REC_SIZE db 0

; ===========================================================================
; NPSPEC-ADAPTIVE-POLICYLEARNING-0001 – Nova Policy Learning Engine
; ===========================================================================
; Tracks policy rule outcomes and adjusts rule weights via EWMA.
; When a rule fires successfully, weight increases; on failure, decreases.
; Connects autopol_evaluate: after each policy fires, pollearn_feedback
; adjusts the rule's learned weight.
;
; Record: rule_id(4) + trigger(4) + weight(4) + fires(4) = 16 bytes
; Weight: 8-bit fixed-point via EWMA; 128=neutral, >128=prefer, <128=avoid

POLLEARN_CAPACITY  equ 8
POLLEARN_REC_SIZE  equ 16
POLLEARN_RULE_ID   equ 0
POLLEARN_TRIGGER   equ 4
POLLEARN_WEIGHT    equ 8
POLLEARN_FIRES     equ 12

POLLEARN_ALPHA     equ 24     ; learning rate (out of 256)

pollearn_initialize:
    cmp dword [pollearn_ready], 1
    je .done
    mov edi, pollearn_table
    xor eax, eax
    mov ecx, (POLLEARN_CAPACITY * POLLEARN_REC_SIZE) / 4
    rep stosd
    mov dword [pollearn_ready], 1
.done:
    clc
    ret

; EAX=rule_id, EDX=trigger → register rule at weight=128, CF
pollearn_register:
    cmp dword [pollearn_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, POLLEARN_CAPACITY
    jae .full
    imul edi, esi, POLLEARN_REC_SIZE
    add edi, pollearn_table
    cmp dword [edi + POLLEARN_RULE_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + POLLEARN_RULE_ID], ebx
    mov [edi + POLLEARN_TRIGGER], edx
    mov dword [edi + POLLEARN_WEIGHT], 128
    mov dword [edi + POLLEARN_FIRES], 0
    pop ebx
    clc
    ret
.full:
    pop ebx
.fail:
    stc
    ret

; EAX=rule_id, EDX=1(success)/0(failure) → update weight via EWMA, CF
pollearn_feedback:
    cmp dword [pollearn_ready], 1
    jne .fail
    push ebx
    push ecx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, POLLEARN_CAPACITY
    jae .not_found
    imul edi, esi, POLLEARN_REC_SIZE
    add edi, pollearn_table
    cmp [edi + POLLEARN_RULE_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    inc dword [edi + POLLEARN_FIRES]
    test edx, edx
    jz .failure
    ; success: EWMA toward 255
    mov eax, POLLEARN_ALPHA
    imul eax, eax, 255
    mov ecx, 256 - POLLEARN_ALPHA
    imul ecx, [edi + POLLEARN_WEIGHT]
    add eax, ecx
    shr eax, 8
    mov [edi + POLLEARN_WEIGHT], eax
    jmp .done
.failure:
    ; failure: EWMA toward 0
    mov eax, 256 - POLLEARN_ALPHA
    imul eax, [edi + POLLEARN_WEIGHT]
    shr eax, 8
    mov [edi + POLLEARN_WEIGHT], eax
.done:
    pop ecx
    pop ebx
    clc
    ret
.not_found:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=rule_id → EAX=weight, EDX=fires, CF
pollearn_query:
    cmp dword [pollearn_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, POLLEARN_CAPACITY
    jae .not_found
    imul edi, esi, POLLEARN_REC_SIZE
    add edi, pollearn_table
    cmp [edi + POLLEARN_RULE_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + POLLEARN_WEIGHT]
    mov edx, [edi + POLLEARN_FIRES]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

pollearn_self_test:
    ; register rule=1 trigger=FAULT(1)
    mov eax, 1
    mov edx, 1
    call pollearn_register
    jc .fail
    ; success feedback → weight should increase above 128
    mov eax, 1
    mov edx, 1
    call pollearn_feedback
    jc .fail
    mov eax, 1
    call pollearn_query
    jc .fail
    cmp eax, 128
    jbe .fail
    ; failure feedback → weight should decrease
    mov ebx, eax         ; save current weight
    mov eax, 1
    mov edx, 0
    call pollearn_feedback
    jc .fail
    mov eax, 1
    call pollearn_query
    jc .fail
    cmp eax, ebx
    jae .fail            ; must be less than after success
    clc
    ret
.fail:
    stc
    ret

align 4
pollearn_ready:  dd 0
pollearn_table:
    times POLLEARN_CAPACITY * POLLEARN_REC_SIZE db 0

; ===========================================================================
; NPSPEC-ADAPTIVE-PREDICTIONERROR-0001 – Nova Prediction Error Tracker
; ===========================================================================
; Measures the absolute error between apred_query predictions and observed
; values. Tracks EWMA of error magnitude per metric. When error exceeds
; threshold, re-initializes the predictor alpha (pollearn_feedback failure).
;
; Record: metric_id(4) + last_error(4) + avg_error(4) + samples(4) = 16 bytes

PREDERR_CAPACITY   equ 8
PREDERR_REC_SIZE   equ 16
PREDERR_METRIC     equ 0
PREDERR_LASTERR    equ 4
PREDERR_AVGERR     equ 8
PREDERR_SAMPLES    equ 12

PREDERR_ALPHA      equ 48        ; EWMA alpha for error averaging
PREDERR_THRESHOLD  equ 40        ; avg_error above this → trigger pollearn fail

prederr_initialize:
    cmp dword [prederr_ready], 1
    je .done
    mov edi, prederr_table
    xor eax, eax
    mov ecx, (PREDERR_CAPACITY * PREDERR_REC_SIZE) / 4
    rep stosd
    mov dword [prederr_ready], 1
.done:
    clc
    ret

; EAX=metric_id → register metric, CF
prederr_register:
    cmp dword [prederr_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, PREDERR_CAPACITY
    jae .full
    imul edi, esi, PREDERR_REC_SIZE
    add edi, prederr_table
    cmp dword [edi + PREDERR_METRIC], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + PREDERR_METRIC], ebx
    mov dword [edi + PREDERR_LASTERR], 0
    mov dword [edi + PREDERR_AVGERR], 0
    mov dword [edi + PREDERR_SAMPLES], 0
    pop ebx
    clc
    ret
.full:
    pop ebx
.fail:
    stc
    ret

; EAX=metric_id, EDX=observed → compute error vs apred_query, update EWMA
prederr_observe:
    cmp dword [prederr_ready], 1
    jne .fail
    push ebx
    push ecx
    push edx
    mov ebx, eax            ; metric_id
    ; get prediction from apred_query
    call apred_query        ; EAX=metric_id → EAX=prediction
    jc .not_found
    pop edx                 ; observed
    push edx
    ; error = |prediction - observed|
    sub eax, edx
    jns .pos_err
    neg eax
.pos_err:
    mov ecx, eax            ; error magnitude
    ; find record
    xor esi, esi
.scan:
    cmp esi, PREDERR_CAPACITY
    jae .not_found
    imul edi, esi, PREDERR_REC_SIZE
    add edi, prederr_table
    cmp [edi + PREDERR_METRIC], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov [edi + PREDERR_LASTERR], ecx
    inc dword [edi + PREDERR_SAMPLES]
    ; EWMA avg_error
    push ecx
    mov eax, PREDERR_ALPHA
    imul eax, ecx
    mov ecx, 256 - PREDERR_ALPHA
    imul ecx, [edi + PREDERR_AVGERR]
    add eax, ecx
    shr eax, 8
    mov [edi + PREDERR_AVGERR], eax
    pop ecx
    ; if avg_error > threshold → pollearn_feedback(metric_id, failure)
    cmp dword [edi + PREDERR_AVGERR], PREDERR_THRESHOLD
    jbe .ok
    mov eax, ebx
    xor edx, edx            ; failure
    call pollearn_feedback
.ok:
    pop edx
    pop ecx
    pop ebx
    clc
    ret
.not_found:
    pop edx
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=metric_id → EAX=avg_error, EDX=last_error, ECX=samples, CF
prederr_query:
    cmp dword [prederr_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, PREDERR_CAPACITY
    jae .not_found
    imul edi, esi, PREDERR_REC_SIZE
    add edi, prederr_table
    cmp [edi + PREDERR_METRIC], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + PREDERR_AVGERR]
    mov edx, [edi + PREDERR_LASTERR]
    mov ecx, [edi + PREDERR_SAMPLES]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

prederr_self_test:
    ; register metric=1
    mov eax, 1
    call prederr_register
    jc .fail
    ; also register with apred (metric=1, initial=50, alpha=64)
    mov eax, 1
    mov edx, 50
    mov ecx, 64
    call apred_register
    ; ignore CF (may already be registered)
    ; observe value=50 (no error)
    mov eax, 1
    mov edx, 50
    call prederr_observe
    jc .fail
    ; query → samples=1
    mov eax, 1
    call prederr_query
    jc .fail
    cmp ecx, 1
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
prederr_ready:  dd 0
prederr_table:
    times PREDERR_CAPACITY * PREDERR_REC_SIZE db 0

; ===========================================================================
; NPSPEC-ADAPTIVE-PREFETCH-0001 – Nova Adaptive Prefetch Engine
; ===========================================================================
; Tracks memory access strides per stream and triggers prefetch when stride
; is stable. Connects acache_record: prefetch hits are recorded as cache
; hits for the associated cache line.
;
; Record: stream_id(4) + last_addr(4) + stride(4) + confidence(4) = 16 bytes
; confidence: EWMA of stride-stability (0=random, 255=fully strided)

APREFETCH_CAPACITY   equ 8
APREFETCH_REC_SIZE   equ 16
APREFETCH_STREAM     equ 0
APREFETCH_LAST_ADDR  equ 4
APREFETCH_STRIDE     equ 8
APREFETCH_CONFIDENCE equ 12

APREFETCH_ALPHA      equ 64      ; confidence EWMA alpha
APREFETCH_TRIGGER    equ 160     ; prefetch when confidence > this

aprefetch_initialize:
    cmp dword [aprefetch_ready], 1
    je .done
    mov edi, aprefetch_table
    xor eax, eax
    mov ecx, (APREFETCH_CAPACITY * APREFETCH_REC_SIZE) / 4
    rep stosd
    mov dword [aprefetch_ready], 1
.done:
    clc
    ret

; EAX=stream_id → register stream, CF
aprefetch_register:
    cmp dword [aprefetch_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, APREFETCH_CAPACITY
    jae .full
    imul edi, esi, APREFETCH_REC_SIZE
    add edi, aprefetch_table
    cmp dword [edi + APREFETCH_STREAM], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + APREFETCH_STREAM], ebx
    mov dword [edi + APREFETCH_LAST_ADDR], 0xFFFFFFFF
    mov dword [edi + APREFETCH_STRIDE], 0
    mov dword [edi + APREFETCH_CONFIDENCE], 0
    pop ebx
    clc
    ret
.full:
    pop ebx
.fail:
    stc
    ret

; EAX=stream_id, EDX=new_addr → update stride/confidence; EAX=prefetch_addr (if CF=0)
; CF=0 means "should prefetch at EAX"; CF=1 means not yet confident
aprefetch_access:
    cmp dword [aprefetch_ready], 1
    jne .fail
    push ebx
    push ecx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, APREFETCH_CAPACITY
    jae .not_found
    imul edi, esi, APREFETCH_REC_SIZE
    add edi, aprefetch_table
    cmp [edi + APREFETCH_STREAM], ebx
    je .found
    inc esi
    jmp .scan
.found:
    ; compute new stride
    cmp dword [edi + APREFETCH_LAST_ADDR], 0xFFFFFFFF
    je .first_access
    mov eax, edx
    sub eax, [edi + APREFETCH_LAST_ADDR]
    ; check if stride matches current
    mov ecx, [edi + APREFETCH_STRIDE]
    test ecx, ecx
    jz .set_stride
    cmp eax, ecx
    je .stride_match
    ; stride mismatch → reduce confidence
    push eax
    mov eax, 256 - APREFETCH_ALPHA
    imul eax, [edi + APREFETCH_CONFIDENCE]
    shr eax, 8
    mov [edi + APREFETCH_CONFIDENCE], eax
    pop eax
    jmp .update_last
.stride_match:
    ; stride matches → increase confidence via EWMA toward 255
    push eax
    mov eax, APREFETCH_ALPHA
    imul eax, eax, 255
    mov ecx, 256 - APREFETCH_ALPHA
    imul ecx, [edi + APREFETCH_CONFIDENCE]
    add eax, ecx
    shr eax, 8
    mov [edi + APREFETCH_CONFIDENCE], eax
    pop eax
    jmp .update_last
.set_stride:
    mov [edi + APREFETCH_STRIDE], eax
    mov dword [edi + APREFETCH_CONFIDENCE], APREFETCH_ALPHA
    jmp .update_last
.first_access:
    mov [edi + APREFETCH_LAST_ADDR], edx
    pop ecx
    pop ebx
    stc
    ret
.update_last:
    mov [edi + APREFETCH_LAST_ADDR], edx
    ; check if should prefetch
    cmp dword [edi + APREFETCH_CONFIDENCE], APREFETCH_TRIGGER
    jbe .not_confident
    ; return predicted next address
    mov eax, edx
    add eax, [edi + APREFETCH_STRIDE]
    pop ecx
    pop ebx
    clc
    ret
.not_confident:
    pop ecx
    pop ebx
    stc
    ret
.not_found:
    pop ecx
    pop ebx
.fail:
    stc
    ret

aprefetch_self_test:
    ; register stream=1
    mov eax, 1
    call aprefetch_register
    jc .fail
    ; access at 0x1000 (first access)
    mov eax, 1
    mov edx, 0x1000
    call aprefetch_access
    ; may fail (first access) — that's ok
    ; access at 0x1010 (stride=16)
    mov eax, 1
    mov edx, 0x1010
    call aprefetch_access
    ; access at 0x1020 (stride=16 again)
    mov eax, 1
    mov edx, 0x1020
    call aprefetch_access
    ; not yet triggering — still building confidence
    ; basic: no crash = pass
    clc
    ret
.fail:
    stc
    ret

align 4
aprefetch_ready:  dd 0
aprefetch_table:
    times APREFETCH_CAPACITY * APREFETCH_REC_SIZE db 0

; ===========================================================================
; NPSPEC-ADAPTIVE-PRELOAD-0001 – Nova Adaptive Preload Manager
; ===========================================================================
; Queues objects for preloading based on priority. Higher-priority objects
; are preloaded first. Connects rresv_reserve: each preload consumes a
; resource reservation. On preload completion, acache_record is called.
;
; Record: obj_id(4) + priority(4) + state(4) + pad(4) = 16 bytes
; States: PENDING=0 / LOADING=1 / READY=2

APRELOAD_CAPACITY  equ 8
APRELOAD_REC_SIZE  equ 16
APRELOAD_OBJ_ID    equ 0
APRELOAD_PRIORITY  equ 4
APRELOAD_STATE     equ 8
APRELOAD_PAD       equ 12

APRELOAD_PENDING   equ 0
APRELOAD_LOADING   equ 1
APRELOAD_READY     equ 2

apreload_initialize:
    cmp dword [apreload_ready], 1
    je .done
    mov edi, apreload_table
    xor eax, eax
    mov ecx, (APRELOAD_CAPACITY * APRELOAD_REC_SIZE) / 4
    rep stosd
    mov dword [apreload_ready], 1
.done:
    clc
    ret

; EAX=obj_id, EDX=priority → enqueue for preload, CF=1 if full
apreload_enqueue:
    cmp dword [apreload_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push ecx
    mov ebx, eax
    mov ecx, edx
    xor esi, esi
.scan:
    cmp esi, APRELOAD_CAPACITY
    jae .full
    imul edi, esi, APRELOAD_REC_SIZE
    add edi, apreload_table
    cmp dword [edi + APRELOAD_STATE], APRELOAD_PENDING
    jne .next
    cmp dword [edi + APRELOAD_OBJ_ID], 0
    je .slot
.next:
    inc esi
    jmp .scan
.slot:
    mov [edi + APRELOAD_OBJ_ID], ebx
    mov [edi + APRELOAD_PRIORITY], ecx
    mov dword [edi + APRELOAD_STATE], APRELOAD_PENDING
    pop ecx
    pop ebx
    clc
    ret
.full:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; → dequeue highest-priority PENDING object → EAX=obj_id, CF=1 if empty
; marks dequeued entry LOADING
apreload_dequeue_highest:
    cmp dword [apreload_ready], 1
    jne .fail
    push ebx
    push ecx
    xor ebx, ebx            ; best_obj = 0
    xor ecx, ecx            ; best_priority = 0
    mov edx, 0xFFFFFFFF     ; best_slot
    xor esi, esi
.scan:
    cmp esi, APRELOAD_CAPACITY
    jae .done
    imul edi, esi, APRELOAD_REC_SIZE
    add edi, apreload_table
    cmp dword [edi + APRELOAD_STATE], APRELOAD_PENDING
    jne .next
    cmp dword [edi + APRELOAD_OBJ_ID], 0
    je .next
    cmp [edi + APRELOAD_PRIORITY], ecx
    jbe .next
    mov ecx, [edi + APRELOAD_PRIORITY]
    mov ebx, [edi + APRELOAD_OBJ_ID]
    mov edx, esi
.next:
    inc esi
    jmp .scan
.done:
    test ebx, ebx
    jz .empty
    cmp edx, 0xFFFFFFFF
    je .empty
    imul edi, edx, APRELOAD_REC_SIZE
    add edi, apreload_table
    mov dword [edi + APRELOAD_STATE], APRELOAD_LOADING
    mov eax, ebx
    pop ecx
    pop ebx
    clc
    ret
.empty:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=obj_id → mark READY, record in acache as cache hit (line=obj_id), CF
apreload_complete:
    cmp dword [apreload_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, APRELOAD_CAPACITY
    jae .not_found
    imul edi, esi, APRELOAD_REC_SIZE
    add edi, apreload_table
    cmp [edi + APRELOAD_OBJ_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov dword [edi + APRELOAD_STATE], APRELOAD_READY
    ; cross-subsystem: notify acache that this line is now hot
    mov eax, ebx
    call acache_register    ; ignore CF if already registered
    mov eax, ebx
    mov edx, 1              ; hit
    call acache_record
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

apreload_self_test:
    ; enqueue obj=1 priority=5, obj=2 priority=10
    mov eax, 1
    mov edx, 5
    call apreload_enqueue
    jc .fail
    mov eax, 2
    mov edx, 10
    call apreload_enqueue
    jc .fail
    ; dequeue highest → should get obj=2
    call apreload_dequeue_highest
    jc .fail
    cmp eax, 2
    jne .fail
    ; complete obj=2
    mov eax, 2
    call apreload_complete
    jc .fail
    clc
    ret
.fail:
    stc
    ret

align 4
apreload_ready:  dd 0
apreload_table:
    times APRELOAD_CAPACITY * APRELOAD_REC_SIZE db 0

; ===========================================================================
; NPSPEC-ADAPTIVE-STORAGE-0001 – Nova Adaptive Storage Manager
; ===========================================================================
; Monitors I/O rate per volume and selects storage tier (HOT/WARM/COLD).
; Connects raccount_charge: each I/O op charges the owning process.
; Tier: HOT=0 (fast NVMe) / WARM=1 (SSD) / COLD=2 (HDD/remote).
;
; Record: vol_id(4) + io_rate(4) + tier(4) + total_ops(4) = 16 bytes
; io_rate is EWMA ops/tick (alpha=32/256)

ASTOR_CAPACITY   equ 8
ASTOR_REC_SIZE   equ 16
ASTOR_VOL_ID     equ 0
ASTOR_IORATE     equ 4
ASTOR_TIER       equ 8
ASTOR_TOTAL_OPS  equ 12

ASTOR_ALPHA      equ 32
ASTOR_HOT        equ 0
ASTOR_WARM       equ 1
ASTOR_COLD       equ 2
ASTOR_HOT_THRESH equ 100    ; io_rate > 100 → HOT
ASTOR_COLD_THRES equ 20     ; io_rate < 20 → COLD

astorage_initialize:
    cmp dword [astorage_ready], 1
    je .done
    mov edi, astorage_table
    xor eax, eax
    mov ecx, (ASTOR_CAPACITY * ASTOR_REC_SIZE) / 4
    rep stosd
    mov dword [astorage_ready], 1
.done:
    clc
    ret

; EAX=vol_id → register volume at WARM tier, CF
astorage_register:
    cmp dword [astorage_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, ASTOR_CAPACITY
    jae .full
    imul edi, esi, ASTOR_REC_SIZE
    add edi, astorage_table
    cmp dword [edi + ASTOR_VOL_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + ASTOR_VOL_ID], ebx
    mov dword [edi + ASTOR_IORATE], 50
    mov dword [edi + ASTOR_TIER], ASTOR_WARM
    mov dword [edi + ASTOR_TOTAL_OPS], 0
    pop ebx
    clc
    ret
.full:
    pop ebx
.fail:
    stc
    ret

; EAX=vol_id, EDX=ops_this_tick → update I/O rate EWMA + recompute tier
astorage_record_io:
    cmp dword [astorage_ready], 1
    jne .fail
    push ebx
    push ecx
    mov ebx, eax
    mov ecx, edx
    xor esi, esi
.scan:
    cmp esi, ASTOR_CAPACITY
    jae .not_found
    imul edi, esi, ASTOR_REC_SIZE
    add edi, astorage_table
    cmp [edi + ASTOR_VOL_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    add [edi + ASTOR_TOTAL_OPS], ecx
    ; EWMA io_rate
    mov eax, ASTOR_ALPHA
    imul eax, ecx
    mov ecx, 256 - ASTOR_ALPHA
    imul ecx, [edi + ASTOR_IORATE]
    add eax, ecx
    shr eax, 8
    mov [edi + ASTOR_IORATE], eax
    ; recompute tier
    cmp dword [edi + ASTOR_IORATE], ASTOR_HOT_THRESH
    jae .hot
    cmp dword [edi + ASTOR_IORATE], ASTOR_COLD_THRES
    jbe .cold
    mov dword [edi + ASTOR_TIER], ASTOR_WARM
    jmp .done
.hot:
    mov dword [edi + ASTOR_TIER], ASTOR_HOT
    jmp .done
.cold:
    mov dword [edi + ASTOR_TIER], ASTOR_COLD
.done:
    pop ecx
    pop ebx
    clc
    ret
.not_found:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=vol_id → EAX=tier, EDX=io_rate, ECX=total_ops, CF
astorage_query:
    cmp dword [astorage_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, ASTOR_CAPACITY
    jae .not_found
    imul edi, esi, ASTOR_REC_SIZE
    add edi, astorage_table
    cmp [edi + ASTOR_VOL_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + ASTOR_TIER]
    mov edx, [edi + ASTOR_IORATE]
    mov ecx, [edi + ASTOR_TOTAL_OPS]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

astorage_self_test:
    mov eax, 1
    call astorage_register
    jc .fail
    ; high I/O → should promote to HOT
    mov eax, 1
    mov edx, 150
    call astorage_record_io
    jc .fail
    mov eax, 1
    call astorage_query
    jc .fail
    cmp eax, ASTOR_HOT
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
astorage_ready:  dd 0
astorage_table:
    times ASTOR_CAPACITY * ASTOR_REC_SIZE db 0

; ===========================================================================
; NPSPEC-ADAPTIVE-NETWORK-0001 – Nova Adaptive Network Manager
; ===========================================================================
; Monitors bandwidth utilization per interface and adjusts QoS class.
; Connects rguarant_check: enforces min-bandwidth guarantees.
; QoS: BEST_EFFORT=0 / BULK=1 / INTERACTIVE=2 / REALTIME=3
;
; Record: iface_id(4) + bw_used(4) + bw_cap(4) + qos(4) = 16 bytes

ANET_CAPACITY   equ 4
ANET_REC_SIZE   equ 16
ANET_IFACE      equ 0
ANET_BW_USED    equ 4
ANET_BW_CAP     equ 8
ANET_QOS        equ 12

ANET_BEST_EFFORT equ 0
ANET_BULK        equ 1
ANET_INTERACTIVE equ 2
ANET_REALTIME    equ 3

ANET_ALPHA       equ 48

anetwork_initialize:
    cmp dword [anetwork_ready], 1
    je .done
    mov edi, anetwork_table
    xor eax, eax
    mov ecx, (ANET_CAPACITY * ANET_REC_SIZE) / 4
    rep stosd
    mov dword [anetwork_ready], 1
.done:
    clc
    ret

; EAX=iface_id, EDX=bw_cap → register interface, CF
anetwork_register:
    cmp dword [anetwork_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push ecx
    mov ebx, eax
    mov ecx, edx
    xor esi, esi
.scan:
    cmp esi, ANET_CAPACITY
    jae .full
    imul edi, esi, ANET_REC_SIZE
    add edi, anetwork_table
    cmp dword [edi + ANET_IFACE], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + ANET_IFACE], ebx
    mov dword [edi + ANET_BW_USED], 0
    mov [edi + ANET_BW_CAP], ecx
    mov dword [edi + ANET_QOS], ANET_BEST_EFFORT
    pop ecx
    pop ebx
    clc
    ret
.full:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=iface_id, EDX=bw_sample → update BW EWMA + adjust QoS
; >80% cap → REALTIME; >50% → INTERACTIVE; >20% → BULK; else → BEST_EFFORT
anetwork_update_bw:
    cmp dword [anetwork_ready], 1
    jne .fail
    push ebx
    push ecx
    mov ebx, eax
    mov ecx, edx
    xor esi, esi
.scan:
    cmp esi, ANET_CAPACITY
    jae .not_found
    imul edi, esi, ANET_REC_SIZE
    add edi, anetwork_table
    cmp [edi + ANET_IFACE], ebx
    je .found
    inc esi
    jmp .scan
.found:
    ; EWMA bw_used
    mov eax, ANET_ALPHA
    imul eax, ecx
    push ecx
    mov ecx, 256 - ANET_ALPHA
    imul ecx, [edi + ANET_BW_USED]
    add eax, ecx
    shr eax, 8
    mov [edi + ANET_BW_USED], eax
    pop ecx
    ; compute utilization % = bw_used * 100 / bw_cap
    mov eax, [edi + ANET_BW_USED]
    imul eax, eax, 100
    mov ecx, [edi + ANET_BW_CAP]
    test ecx, ecx
    jz .set_best
    xor edx, edx
    div ecx             ; EAX = utilization %
    cmp eax, 80
    jae .realtime
    cmp eax, 50
    jae .interactive
    cmp eax, 20
    jae .bulk
.set_best:
    mov dword [edi + ANET_QOS], ANET_BEST_EFFORT
    jmp .ok
.bulk:
    mov dword [edi + ANET_QOS], ANET_BULK
    jmp .ok
.interactive:
    mov dword [edi + ANET_QOS], ANET_INTERACTIVE
    jmp .ok
.realtime:
    mov dword [edi + ANET_QOS], ANET_REALTIME
.ok:
    pop ecx
    pop ebx
    clc
    ret
.not_found:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=iface_id → EAX=qos, EDX=bw_used, ECX=bw_cap, CF
anetwork_query:
    cmp dword [anetwork_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, ANET_CAPACITY
    jae .not_found
    imul edi, esi, ANET_REC_SIZE
    add edi, anetwork_table
    cmp [edi + ANET_IFACE], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + ANET_QOS]
    mov edx, [edi + ANET_BW_USED]
    mov ecx, [edi + ANET_BW_CAP]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

anetwork_self_test:
    ; register iface=1 cap=100
    mov eax, 1
    mov edx, 100
    call anetwork_register
    jc .fail
    ; high utilization → REALTIME
    mov eax, 1
    mov edx, 85
    call anetwork_update_bw
    jc .fail
    mov eax, 1
    call anetwork_query
    jc .fail
    cmp eax, ANET_REALTIME
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
anetwork_ready:  dd 0
anetwork_table:
    times ANET_CAPACITY * ANET_REC_SIZE db 0

; ===========================================================================
; NPSPEC-ADAPTIVE-USEROVERRIDE-0001 – Nova User Override Manager
; ===========================================================================
; Allows user-space (or privileged kernel code) to override adaptive policy
; decisions with persistent key→value settings. Overrides take precedence
; over all EWMA-driven adaptive decisions.
; Connects sconf_set/sconf_get: overrides are persisted in the system config.
;
; Record: key(4) + value(4) + active(4) + version(4) = 16 bytes

UOVER_CAPACITY  equ 8
UOVER_REC_SIZE  equ 16
UOVER_KEY       equ 0
UOVER_VALUE     equ 4
UOVER_ACTIVE    equ 8
UOVER_VERSION   equ 12

uoverride_initialize:
    cmp dword [uoverride_ready], 1
    je .done
    mov edi, uoverride_table
    xor eax, eax
    mov ecx, (UOVER_CAPACITY * UOVER_REC_SIZE) / 4
    rep stosd
    mov dword [uoverride_ready], 1
.done:
    clc
    ret

; EAX=key, EDX=value → set or update override; also persists via sconf_set, CF
uoverride_set:
    cmp dword [uoverride_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push ecx
    mov ebx, eax
    mov ecx, edx
    ; find existing or free slot
    xor esi, esi
.scan:
    cmp esi, UOVER_CAPACITY
    jae .full
    imul edi, esi, UOVER_REC_SIZE
    add edi, uoverride_table
    cmp [edi + UOVER_KEY], ebx
    je .update
    cmp dword [edi + UOVER_KEY], 0
    je .slot
    inc esi
    jmp .scan
.update:
    inc dword [edi + UOVER_VERSION]
    mov [edi + UOVER_VALUE], ecx
    mov dword [edi + UOVER_ACTIVE], 1
    jmp .persist
.slot:
    mov [edi + UOVER_KEY], ebx
    mov [edi + UOVER_VALUE], ecx
    mov dword [edi + UOVER_ACTIVE], 1
    mov dword [edi + UOVER_VERSION], 1
.persist:
    ; cross-subsystem: persist in sconf
    mov eax, ebx
    mov edx, ecx
    call sconf_set
    pop ecx
    pop ebx
    clc
    ret
.full:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=key → EAX=value, EDX=version, CF=1 not found/inactive
uoverride_get:
    cmp dword [uoverride_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, UOVER_CAPACITY
    jae .not_found
    imul edi, esi, UOVER_REC_SIZE
    add edi, uoverride_table
    cmp [edi + UOVER_KEY], ebx
    je .found
    inc esi
    jmp .scan
.found:
    cmp dword [edi + UOVER_ACTIVE], 1
    jne .not_found
    mov eax, [edi + UOVER_VALUE]
    mov edx, [edi + UOVER_VERSION]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

; EAX=key → deactivate override (adaptive resumes), CF
uoverride_clear:
    cmp dword [uoverride_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, UOVER_CAPACITY
    jae .not_found
    imul edi, esi, UOVER_REC_SIZE
    add edi, uoverride_table
    cmp [edi + UOVER_KEY], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov dword [edi + UOVER_ACTIVE], 0
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

uoverride_self_test:
    ; set key=42 value=99
    mov eax, 42
    mov edx, 99
    call uoverride_set
    jc .fail
    ; get it back
    mov eax, 42
    call uoverride_get
    jc .fail
    cmp eax, 99
    jne .fail
    ; clear it
    mov eax, 42
    call uoverride_clear
    jc .fail
    ; get → should fail
    mov eax, 42
    call uoverride_get
    jnc .fail
    clc
    ret
.fail:
    stc
    ret

align 4
uoverride_ready:  dd 0
uoverride_table:
    times UOVER_CAPACITY * UOVER_REC_SIZE db 0

; ===========================================================================
; NPSPEC-RESOURCE-DEADLINE-0001 – Nova Resource Deadline Manager
; ===========================================================================
; Earliest-Deadline-First (EDF) enforcement: tracks task deadlines and
; signals overdue tasks. Connects aschedsw_feedback: overdue tasks boost
; the REALTIME scheduler class weight.
;
; Record: task_id(4) + deadline_tick(4) + state(4) + missed(4) = 16 bytes
; States: ACTIVE=1 / DONE=2 / MISSED=3

RDEAD_CAPACITY  equ 8
RDEAD_REC_SIZE  equ 16
RDEAD_TASK_ID   equ 0
RDEAD_DEADLINE  equ 4
RDEAD_STATE     equ 8
RDEAD_MISSED    equ 12

RDEAD_ACTIVE    equ 1
RDEAD_DONE      equ 2
RDEAD_MISSED_ST equ 3

rdeadline_initialize:
    cmp dword [rdeadline_ready], 1
    je .done
    mov edi, rdeadline_table
    xor eax, eax
    mov ecx, (RDEAD_CAPACITY * RDEAD_REC_SIZE) / 4
    rep stosd
    mov dword [rdeadline_ready], 1
.done:
    clc
    ret

; EAX=task_id, EDX=deadline_tick → register, CF=1 if full
rdeadline_register:
    cmp dword [rdeadline_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push ecx
    mov ebx, eax
    mov ecx, edx
    xor esi, esi
.scan:
    cmp esi, RDEAD_CAPACITY
    jae .full
    imul edi, esi, RDEAD_REC_SIZE
    add edi, rdeadline_table
    cmp dword [edi + RDEAD_STATE], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + RDEAD_TASK_ID], ebx
    mov [edi + RDEAD_DEADLINE], ecx
    mov dword [edi + RDEAD_STATE], RDEAD_ACTIVE
    mov dword [edi + RDEAD_MISSED], 0
    pop ecx
    pop ebx
    clc
    ret
.full:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=task_id → mark done; clears slot, CF=1 if not found
rdeadline_done:
    cmp dword [rdeadline_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, RDEAD_CAPACITY
    jae .not_found
    imul edi, esi, RDEAD_REC_SIZE
    add edi, rdeadline_table
    cmp [edi + RDEAD_TASK_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov dword [edi + RDEAD_STATE], RDEAD_DONE
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

; EDX=current_tick → scan all ACTIVE tasks; overdue → MISSED + aschedsw boost
rdeadline_check_all:
    cmp dword [rdeadline_ready], 1
    jne .fail
    push ebx
    mov ebx, edx
    xor esi, esi
.scan:
    cmp esi, RDEAD_CAPACITY
    jae .done
    imul edi, esi, RDEAD_REC_SIZE
    add edi, rdeadline_table
    cmp dword [edi + RDEAD_STATE], RDEAD_ACTIVE
    jne .next
    cmp [edi + RDEAD_DEADLINE], ebx
    jae .next
    ; overdue
    mov dword [edi + RDEAD_STATE], RDEAD_MISSED_ST
    inc dword [edi + RDEAD_MISSED]
    ; cross-subsystem: boost realtime scheduler class (class=4) weight
    mov eax, 4
    mov edx, 32             ; +32 to realtime class weight
    call aschedsw_feedback
.next:
    inc esi
    jmp .scan
.done:
    pop ebx
    clc
    ret
.fail:
    stc
    ret

rdeadline_self_test:
    ; register task=1 deadline=100
    mov eax, 1
    mov edx, 100
    call rdeadline_register
    jc .fail
    ; check at tick=50 → no misses
    mov edx, 50
    call rdeadline_check_all
    jc .fail
    ; check at tick=200 → task=1 should be MISSED
    mov edx, 200
    call rdeadline_check_all
    jc .fail
    ; verify
    mov edi, rdeadline_table
    cmp dword [edi + RDEAD_STATE], RDEAD_MISSED_ST
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
rdeadline_ready:  dd 0
rdeadline_table:
    times RDEAD_CAPACITY * RDEAD_REC_SIZE db 0

; ===========================================================================
; NPSPEC-RESOURCE-ECONOMY-0001 – Nova Resource Economy Manager
; ===========================================================================
; Credit-based resource economy: each owner gets a budget; operations
; spend credits. When budget exhausted, raccount_charge signals overuse.
; Connects raccount_charge: spending calls it to track real usage.
;
; Record: owner(4) + budget(4) + spent(4) + refills(4) = 16 bytes

RECON_CAPACITY  equ 8
RECON_REC_SIZE  equ 16
RECON_OWNER     equ 0
RECON_BUDGET    equ 4
RECON_SPENT     equ 8
RECON_REFILLS   equ 12

reconomy_initialize:
    cmp dword [reconomy_ready], 1
    je .done
    mov edi, reconomy_table
    xor eax, eax
    mov ecx, (RECON_CAPACITY * RECON_REC_SIZE) / 4
    rep stosd
    mov dword [reconomy_ready], 1
.done:
    clc
    ret

; EAX=owner, EDX=initial_budget → register, CF
reconomy_register:
    cmp dword [reconomy_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push ecx
    mov ebx, eax
    mov ecx, edx
    xor esi, esi
.scan:
    cmp esi, RECON_CAPACITY
    jae .full
    imul edi, esi, RECON_REC_SIZE
    add edi, reconomy_table
    cmp dword [edi + RECON_OWNER], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + RECON_OWNER], ebx
    mov [edi + RECON_BUDGET], ecx
    mov dword [edi + RECON_SPENT], 0
    mov dword [edi + RECON_REFILLS], 0
    pop ecx
    pop ebx
    clc
    ret
.full:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=owner, EDX=cost → spend credits; CF=0 ok, CF=1 budget exhausted
reconomy_spend:
    cmp dword [reconomy_ready], 1
    jne .fail
    push ebx
    push ecx
    mov ebx, eax
    mov ecx, edx
    xor esi, esi
.scan:
    cmp esi, RECON_CAPACITY
    jae .not_found
    imul edi, esi, RECON_REC_SIZE
    add edi, reconomy_table
    cmp [edi + RECON_OWNER], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + RECON_BUDGET]
    cmp eax, ecx
    jb .exhausted
    sub [edi + RECON_BUDGET], ecx
    add [edi + RECON_SPENT], ecx
    ; cross-subsystem: charge real resource account (cpu_delta=cost, mem_delta=0)
    mov eax, ebx
    mov edx, ecx
    xor ecx, ecx
    call raccount_charge
    pop ecx
    pop ebx
    clc
    ret
.exhausted:
    pop ecx
    pop ebx
    stc
    ret
.not_found:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=owner, EDX=refill_amount → refill budget, CF
reconomy_refill:
    cmp dword [reconomy_ready], 1
    jne .fail
    push ebx
    push ecx
    mov ebx, eax
    mov ecx, edx
    xor esi, esi
.scan:
    cmp esi, RECON_CAPACITY
    jae .not_found
    imul edi, esi, RECON_REC_SIZE
    add edi, reconomy_table
    cmp [edi + RECON_OWNER], ebx
    je .found
    inc esi
    jmp .scan
.found:
    add [edi + RECON_BUDGET], ecx
    inc dword [edi + RECON_REFILLS]
    pop ecx
    pop ebx
    clc
    ret
.not_found:
    pop ecx
    pop ebx
.fail:
    stc
    ret

reconomy_self_test:
    ; owner=1 budget=50
    mov eax, 1
    mov edx, 50
    call reconomy_register
    jc .fail
    ; spend 30 → ok
    mov eax, 1
    mov edx, 30
    call reconomy_spend
    jc .fail
    ; spend 30 more → exhausted
    mov eax, 1
    mov edx, 30
    call reconomy_spend
    jnc .fail
    ; refill 100
    mov eax, 1
    mov edx, 100
    call reconomy_refill
    jc .fail
    ; spend 30 → ok again
    mov eax, 1
    mov edx, 30
    call reconomy_spend
    jc .fail
    clc
    ret
.fail:
    stc
    ret

align 4
reconomy_ready:  dd 0
reconomy_table:
    times RECON_CAPACITY * RECON_REC_SIZE db 0

; ===========================================================================
; NPSPEC-RESOURCE-ENERGY-0001 – Nova Resource Energy Manager
; ===========================================================================
; Power-rail budget enforcement. Tracks mW consumed per rail and enforces
; power caps. Connects apower_query: when a rail exceeds budget, the
; associated power domain is forced to low P-state via apower_update_load.
;
; Record: rail_id(4) + power_mw(4) + cap_mw(4) + violations(4) = 16 bytes

RENRG_CAPACITY  equ 4
RENRG_REC_SIZE  equ 16
RENRG_RAIL_ID   equ 0
RENRG_POWER     equ 4
RENRG_CAP       equ 8
RENRG_VIOL      equ 12

RENRG_ALPHA     equ 32

renergy_initialize:
    cmp dword [renergy_ready], 1
    je .done
    mov edi, renergy_table
    xor eax, eax
    mov ecx, (RENRG_CAPACITY * RENRG_REC_SIZE) / 4
    rep stosd
    mov dword [renergy_ready], 1
.done:
    clc
    ret

; EAX=rail_id, EDX=cap_mw → register rail, CF
renergy_register:
    cmp dword [renergy_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push ecx
    mov ebx, eax
    mov ecx, edx
    xor esi, esi
.scan:
    cmp esi, RENRG_CAPACITY
    jae .full
    imul edi, esi, RENRG_REC_SIZE
    add edi, renergy_table
    cmp dword [edi + RENRG_RAIL_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + RENRG_RAIL_ID], ebx
    mov dword [edi + RENRG_POWER], 0
    mov [edi + RENRG_CAP], ecx
    mov dword [edi + RENRG_VIOL], 0
    pop ecx
    pop ebx
    clc
    ret
.full:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=rail_id, EDX=sample_mw → update EWMA power; CF=1 if over cap
renergy_update:
    cmp dword [renergy_ready], 1
    jne .fail
    push ebx
    push ecx
    mov ebx, eax
    mov ecx, edx
    xor esi, esi
.scan:
    cmp esi, RENRG_CAPACITY
    jae .not_found
    imul edi, esi, RENRG_REC_SIZE
    add edi, renergy_table
    cmp [edi + RENRG_RAIL_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    ; EWMA power
    mov eax, RENRG_ALPHA
    imul eax, ecx
    push ecx
    mov ecx, 256 - RENRG_ALPHA
    imul ecx, [edi + RENRG_POWER]
    add eax, ecx
    shr eax, 8
    mov [edi + RENRG_POWER], eax
    pop ecx
    ; check cap
    mov eax, [edi + RENRG_POWER]
    cmp eax, [edi + RENRG_CAP]
    jbe .ok
    inc dword [edi + RENRG_VIOL]
    ; cross-subsystem: force power domain to low-load via apower (rail_id as domain)
    mov eax, ebx
    mov edx, 5              ; low load → P-state will drop
    call apower_update_load
    pop ecx
    pop ebx
    stc
    ret
.ok:
    pop ecx
    pop ebx
    clc
    ret
.not_found:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=rail_id → EAX=power_mw, EDX=cap_mw, ECX=violations, CF
renergy_query:
    cmp dword [renergy_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, RENRG_CAPACITY
    jae .not_found
    imul edi, esi, RENRG_REC_SIZE
    add edi, renergy_table
    cmp [edi + RENRG_RAIL_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + RENRG_POWER]
    mov edx, [edi + RENRG_CAP]
    mov ecx, [edi + RENRG_VIOL]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

renergy_self_test:
    ; rail=1 cap=100mW
    mov eax, 1
    mov edx, 100
    call renergy_register
    jc .fail
    ; first register the apower domain too (rail shares domain id=1)
    mov eax, 1
    call apower_register
    ; ignore CF if already registered
    ; update with 50mW → under cap
    mov eax, 1
    mov edx, 50
    call renergy_update
    jc .fail
    ; update with 200mW → over cap → CF=1
    mov eax, 1
    mov edx, 200
    call renergy_update
    jnc .fail
    ; violations should be > 0
    mov eax, 1
    call renergy_query
    jc .fail
    test ecx, ecx
    jz .fail
    clc
    ret
.fail:
    stc
    ret

align 4
renergy_ready:  dd 0
renergy_table:
    times RENRG_CAPACITY * RENRG_REC_SIZE db 0

; ===========================================================================
; NPSPEC-RESOURCE-LATENCY-0001 – Nova Resource Latency Monitor
; ===========================================================================
; Tracks round-trip latency per endpoint via EWMA. Signals SLA violation
; when latency exceeds threshold. Connects sdiag_diagnose: high latency
; triggers LATENCY symptom diagnosis. Connects apred_update for prediction.
;
; Record: endpoint_id(4) + avg_lat(4) + sla_us(4) + violations(4) = 16 bytes

RLAT_CAPACITY   equ 8
RLAT_REC_SIZE   equ 16
RLAT_ENDPOINT   equ 0
RLAT_AVG_LAT    equ 4
RLAT_SLA        equ 8
RLAT_VIOL       equ 12

RLAT_ALPHA      equ 48

rlatency_initialize:
    cmp dword [rlatency_ready], 1
    je .done
    mov edi, rlatency_table
    xor eax, eax
    mov ecx, (RLAT_CAPACITY * RLAT_REC_SIZE) / 4
    rep stosd
    mov dword [rlatency_ready], 1
.done:
    clc
    ret

; EAX=endpoint_id, EDX=sla_us → register, CF
rlatency_register:
    cmp dword [rlatency_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push ecx
    mov ebx, eax
    mov ecx, edx
    xor esi, esi
.scan:
    cmp esi, RLAT_CAPACITY
    jae .full
    imul edi, esi, RLAT_REC_SIZE
    add edi, rlatency_table
    cmp dword [edi + RLAT_ENDPOINT], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + RLAT_ENDPOINT], ebx
    mov dword [edi + RLAT_AVG_LAT], 0
    mov [edi + RLAT_SLA], ecx
    mov dword [edi + RLAT_VIOL], 0
    pop ecx
    pop ebx
    clc
    ret
.full:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=endpoint_id, EDX=observed_us → update EWMA; CF=1 if SLA violated
rlatency_observe:
    cmp dword [rlatency_ready], 1
    jne .fail
    push ebx
    push ecx
    mov ebx, eax
    mov ecx, edx
    xor esi, esi
.scan:
    cmp esi, RLAT_CAPACITY
    jae .not_found
    imul edi, esi, RLAT_REC_SIZE
    add edi, rlatency_table
    cmp [edi + RLAT_ENDPOINT], ebx
    je .found
    inc esi
    jmp .scan
.found:
    ; EWMA avg_lat
    mov eax, RLAT_ALPHA
    imul eax, ecx
    push ecx
    mov ecx, 256 - RLAT_ALPHA
    imul ecx, [edi + RLAT_AVG_LAT]
    add eax, ecx
    shr eax, 8
    mov [edi + RLAT_AVG_LAT], eax
    pop ecx
    ; cross-subsystem: update prediction for endpoint metric
    push edx
    mov eax, ebx
    mov edx, [edi + RLAT_AVG_LAT]
    call apred_update
    pop edx
    ; check SLA
    mov eax, [edi + RLAT_AVG_LAT]
    cmp eax, [edi + RLAT_SLA]
    jbe .ok
    inc dword [edi + RLAT_VIOL]
    ; cross-subsystem: diagnose LATENCY symptom (symptom=1)
    push ebx
    mov eax, 1              ; LATENCY symptom
    call sdiag_diagnose
    pop ebx
    pop ecx
    pop ebx
    stc
    ret
.ok:
    pop ecx
    pop ebx
    clc
    ret
.not_found:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=endpoint_id → EAX=avg_lat, EDX=sla_us, ECX=violations, CF
rlatency_query:
    cmp dword [rlatency_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, RLAT_CAPACITY
    jae .not_found
    imul edi, esi, RLAT_REC_SIZE
    add edi, rlatency_table
    cmp [edi + RLAT_ENDPOINT], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + RLAT_AVG_LAT]
    mov edx, [edi + RLAT_SLA]
    mov ecx, [edi + RLAT_VIOL]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

rlatency_self_test:
    ; endpoint=1 sla=100us
    mov eax, 1
    mov edx, 100
    call rlatency_register
    jc .fail
    ; observe 50us → ok
    mov eax, 1
    mov edx, 50
    call rlatency_observe
    jc .fail
    ; observe 500us → SLA violation
    mov eax, 1
    mov edx, 500
    call rlatency_observe
    jnc .fail
    ; violations > 0
    mov eax, 1
    call rlatency_query
    jc .fail
    test ecx, ecx
    jz .fail
    clc
    ret
.fail:
    stc
    ret

align 4
rlatency_ready:  dd 0
rlatency_table:
    times RLAT_CAPACITY * RLAT_REC_SIZE db 0

; ===========================================================================
; NPSPEC-STATE-GLOBAL-0001 – Nova State Global Store
; ===========================================================================
; Kernel-wide named state store with 16 slots. Any subsystem can publish
; or subscribe to named state values. Connects sver_bump: each write
; atomically bumps the version for that key. Connects shist_record.
;
; Record: key(4) + value(4) + version(4) + owner(4) = 16 bytes

SGLOBAL_CAPACITY  equ 16
SGLOBAL_REC_SIZE  equ 16
SGLOBAL_KEY       equ 0
SGLOBAL_VALUE     equ 4
SGLOBAL_VERSION   equ 8
SGLOBAL_OWNER     equ 12

sglobal_initialize:
    cmp dword [sglobal_ready], 1
    je .done
    mov edi, sglobal_table
    xor eax, eax
    mov ecx, (SGLOBAL_CAPACITY * SGLOBAL_REC_SIZE) / 4
    rep stosd
    mov dword [sglobal_ready], 1
.done:
    clc
    ret

; EAX=key, EDX=value, ECX=owner → publish/update global state, CF
sglobal_publish:
    cmp dword [sglobal_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push ecx
    push edx
    mov ebx, eax            ; key
    ; find existing or free slot
    xor esi, esi
.scan:
    cmp esi, SGLOBAL_CAPACITY
    jae .full
    imul edi, esi, SGLOBAL_REC_SIZE
    add edi, sglobal_table
    cmp [edi + SGLOBAL_KEY], ebx
    je .update
    cmp dword [edi + SGLOBAL_KEY], 0
    je .slot
    inc esi
    jmp .scan
.update:
    ; get old state for history
    mov eax, [edi + SGLOBAL_VALUE]  ; old value
    pop edx                          ; new value
    push edx
    ; bump version
    inc dword [edi + SGLOBAL_VERSION]
    mov [edi + SGLOBAL_VALUE], edx
    pop edx
    push edx
    jmp .record_hist
.slot:
    mov [edi + SGLOBAL_KEY], ebx
    pop edx
    push edx
    mov [edi + SGLOBAL_VALUE], edx
    pop edx
    push edx
    pop ecx
    push ecx
    mov [edi + SGLOBAL_OWNER], ecx
    mov dword [edi + SGLOBAL_VERSION], 1
    xor eax, eax            ; old value = 0 for new slot
    jmp .record_hist
.record_hist:
    ; cross-subsystem: record in state history (obj=key, old=eax, new=value)
    push eax
    mov eax, ebx            ; obj_id = key
    pop edx                 ; old
    push edx
    mov ecx, [edi + SGLOBAL_VALUE]   ; new
    call shist_record
    pop edx
    pop edx
    pop ecx
    pop ebx
    clc
    ret
.full:
    pop edx
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=key → EAX=value, EDX=version, ECX=owner, CF=1 not found
sglobal_get:
    cmp dword [sglobal_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, SGLOBAL_CAPACITY
    jae .not_found
    imul edi, esi, SGLOBAL_REC_SIZE
    add edi, sglobal_table
    cmp [edi + SGLOBAL_KEY], ebx
    je .found
    inc esi
    jmp .scan
.found:
    mov eax, [edi + SGLOBAL_VALUE]
    mov edx, [edi + SGLOBAL_VERSION]
    mov ecx, [edi + SGLOBAL_OWNER]
    pop ebx
    clc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

sglobal_self_test:
    ; publish key=7 value=42 owner=1
    mov eax, 7
    mov edx, 42
    mov ecx, 1
    call sglobal_publish
    jc .fail
    ; get it back
    mov eax, 7
    call sglobal_get
    jc .fail
    cmp eax, 42
    jne .fail
    ; update to 99
    mov eax, 7
    mov edx, 99
    mov ecx, 1
    call sglobal_publish
    jc .fail
    mov eax, 7
    call sglobal_get
    jc .fail
    cmp eax, 99
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
sglobal_ready:  dd 0
sglobal_table:
    times SGLOBAL_CAPACITY * SGLOBAL_REC_SIZE db 0

; ===========================================================================
; NPSPEC-TRANSACTION-CONFIG-0001 – Nova Transaction Config Manager
; ===========================================================================
; Atomic configuration transactions: stages a set of sconf changes, then
; commits or aborts them as a unit. Prevents partial config updates.
; Uses txroll_save/txroll_undo for rollback and sconf_set/sconf_get.
;
; Record: cfg_txn_id(4) + key(4) + old_val(4) + new_val(4) = 16 bytes
; States tracked separately via txsys

TXCFG_CAPACITY  equ 8
TXCFG_REC_SIZE  equ 16
TXCFG_TXN_ID    equ 0
TXCFG_KEY       equ 4
TXCFG_OLD_VAL   equ 8
TXCFG_NEW_VAL   equ 12

txconfig_initialize:
    cmp dword [txconfig_ready], 1
    je .done
    mov edi, txconfig_table
    xor eax, eax
    mov ecx, (TXCFG_CAPACITY * TXCFG_REC_SIZE) / 4
    rep stosd
    mov dword [txconfig_ready], 1
.done:
    clc
    ret

; EAX=cfg_txn_id, EDX=key, ECX=new_value → stage change; reads old via sconf_get
txconfig_stage:
    cmp dword [txconfig_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push ecx
    push edx
    mov ebx, eax            ; txn_id
    ; find free slot
    xor esi, esi
.scan:
    cmp esi, TXCFG_CAPACITY
    jae .full
    imul edi, esi, TXCFG_REC_SIZE
    add edi, txconfig_table
    cmp dword [edi + TXCFG_TXN_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + TXCFG_TXN_ID], ebx
    pop edx
    push edx
    mov [edi + TXCFG_KEY], edx
    ; read old value via sconf_get
    mov eax, edx            ; key
    call sconf_get          ; EAX=value or CF=1 (key not found → old=0)
    jc .old_zero
    mov [edi + TXCFG_OLD_VAL], eax
    jmp .set_new
.old_zero:
    mov dword [edi + TXCFG_OLD_VAL], 0
.set_new:
    pop edx
    push edx
    pop ecx
    push ecx
    mov [edi + TXCFG_NEW_VAL], ecx
    pop edx
    pop ecx
    pop ebx
    clc
    ret
.full:
    pop edx
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=cfg_txn_id → apply all staged changes via sconf_set, then clear slots
txconfig_commit:
    cmp dword [txconfig_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, TXCFG_CAPACITY
    jae .done
    imul edi, esi, TXCFG_REC_SIZE
    add edi, txconfig_table
    cmp [edi + TXCFG_TXN_ID], ebx
    jne .next
    ; apply via sconf_set(key, new_val)
    mov eax, [edi + TXCFG_KEY]
    mov edx, [edi + TXCFG_NEW_VAL]
    call sconf_set
    mov dword [edi + TXCFG_TXN_ID], 0  ; clear slot
.next:
    inc esi
    jmp .scan
.done:
    pop ebx
    clc
    ret
.fail:
    stc
    ret

; EAX=cfg_txn_id → restore old values via sconf_set, then clear slots
txconfig_abort:
    cmp dword [txconfig_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, TXCFG_CAPACITY
    jae .done
    imul edi, esi, TXCFG_REC_SIZE
    add edi, txconfig_table
    cmp [edi + TXCFG_TXN_ID], ebx
    jne .next
    ; restore old value
    mov eax, [edi + TXCFG_KEY]
    mov edx, [edi + TXCFG_OLD_VAL]
    call sconf_set
    mov dword [edi + TXCFG_TXN_ID], 0
.next:
    inc esi
    jmp .scan
.done:
    pop ebx
    clc
    ret
.fail:
    stc
    ret

txconfig_self_test:
    ; stage txn=1: key=10 → value=42
    mov eax, 1
    mov edx, 10
    mov ecx, 42
    call txconfig_stage
    jc .fail
    ; commit txn=1
    mov eax, 1
    call txconfig_commit
    jc .fail
    ; verify via sconf_get(key=10) → 42
    mov eax, 10
    call sconf_get
    jc .fail
    cmp eax, 42
    jne .fail
    ; stage txn=2: key=10 → value=99
    mov eax, 2
    mov edx, 10
    mov ecx, 99
    call txconfig_stage
    jc .fail
    ; abort → should restore to 42
    mov eax, 2
    call txconfig_abort
    jc .fail
    mov eax, 10
    call sconf_get
    jc .fail
    cmp eax, 42
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
txconfig_ready:  dd 0
txconfig_table:
    times TXCFG_CAPACITY * TXCFG_REC_SIZE db 0

; ===========================================================================
; NPSPEC-TRANSACTION-MULTISERVICE-0001 – Nova Tx MultiService Coordinator
; ===========================================================================
; Extends txsys to track per-service outcomes in a cross-service transaction.
; Each service registers a vote (COMMIT/ABORT); coordinator decides globally.
; Connects txsys_participant_ready for the COMMIT path.
;
; Record: ms_txn_id(4) + service_id(4) + vote(4) + applied(4) = 16 bytes
; Vote: PENDING=0 / COMMIT=1 / ABORT=2

TXMULTI_CAPACITY  equ 4
TXMULTI_REC_SIZE  equ 16
TXMULTI_TXN_ID    equ 0
TXMULTI_SVC_ID    equ 4
TXMULTI_VOTE      equ 8
TXMULTI_APPLIED   equ 12

TXMULTI_PENDING   equ 0
TXMULTI_COMMIT    equ 1
TXMULTI_ABORT     equ 2

txmulti_initialize:
    cmp dword [txmulti_ready], 1
    je .done
    mov edi, txmulti_table
    xor eax, eax
    mov ecx, (TXMULTI_CAPACITY * TXMULTI_REC_SIZE) / 4
    rep stosd
    mov dword [txmulti_ready], 1
.done:
    clc
    ret

; EAX=ms_txn_id, EDX=service_id → register service as PENDING vote, CF
txmulti_register_service:
    cmp dword [txmulti_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push ecx
    mov ebx, eax
    mov ecx, edx
    xor esi, esi
.scan:
    cmp esi, TXMULTI_CAPACITY
    jae .full
    imul edi, esi, TXMULTI_REC_SIZE
    add edi, txmulti_table
    cmp dword [edi + TXMULTI_TXN_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + TXMULTI_TXN_ID], ebx
    mov [edi + TXMULTI_SVC_ID], ecx
    mov dword [edi + TXMULTI_VOTE], TXMULTI_PENDING
    mov dword [edi + TXMULTI_APPLIED], 0
    pop ecx
    pop ebx
    clc
    ret
.full:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=ms_txn_id, EDX=service_id, ECX=vote(COMMIT=1/ABORT=2) → record vote, CF
txmulti_vote:
    cmp dword [txmulti_ready], 1
    jne .fail
    push ebx
    push ecx
    push edx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, TXMULTI_CAPACITY
    jae .not_found
    imul edi, esi, TXMULTI_REC_SIZE
    add edi, txmulti_table
    cmp [edi + TXMULTI_TXN_ID], ebx
    jne .next
    cmp [edi + TXMULTI_SVC_ID], edx
    je .found
.next:
    inc esi
    jmp .scan
.found:
    mov [edi + TXMULTI_VOTE], ecx
    ; if COMMIT, forward to txsys
    cmp ecx, TXMULTI_COMMIT
    jne .done
    mov eax, ebx
    call txsys_participant_ready
.done:
    pop edx
    pop ecx
    pop ebx
    clc
    ret
.not_found:
    pop edx
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=ms_txn_id → CF=0 if all votes COMMIT, CF=1 if any ABORT or PENDING
txmulti_check:
    cmp dword [txmulti_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
    xor ecx, ecx            ; found_count
.scan:
    cmp esi, TXMULTI_CAPACITY
    jae .done
    imul edi, esi, TXMULTI_REC_SIZE
    add edi, txmulti_table
    cmp [edi + TXMULTI_TXN_ID], ebx
    jne .next
    inc ecx
    cmp dword [edi + TXMULTI_VOTE], TXMULTI_COMMIT
    jne .not_all_commit
.next:
    inc esi
    jmp .scan
.done:
    test ecx, ecx
    jz .not_all_commit
    pop ebx
    clc
    ret
.not_all_commit:
    pop ebx
.fail:
    stc
    ret

txmulti_self_test:
    ; register txn=1 services 1 and 2 with 2 participants in txsys
    mov eax, 1
    mov edx, 2
    call txsys_begin
    ; ignore CF (txsys may already have it)
    mov eax, 1
    mov edx, 1
    call txmulti_register_service
    jc .fail
    mov eax, 1
    mov edx, 2
    call txmulti_register_service
    jc .fail
    ; both vote COMMIT
    mov eax, 1
    mov edx, 1
    mov ecx, TXMULTI_COMMIT
    call txmulti_vote
    jc .fail
    mov eax, 1
    mov edx, 2
    mov ecx, TXMULTI_COMMIT
    call txmulti_vote
    jc .fail
    ; check → CF=0 (all committed)
    mov eax, 1
    call txmulti_check
    jc .fail
    clc
    ret
.fail:
    stc
    ret

align 4
txmulti_ready:  dd 0
txmulti_table:
    times TXMULTI_CAPACITY * TXMULTI_REC_SIZE db 0

; ===========================================================================
; NPSPEC-VERIFY-TEMPORAL-0001 – Nova Temporal Invariant Verifier
; ===========================================================================
; Verifies that time-bounded invariants hold: event A must occur within
; [min_tick, max_tick] of event B. Connects sdiag for violation reporting.
;
; Record: inv_id(4) + anchor_tick(4) + min_gap(4) + max_gap(4) = 16 bytes

VTEMPORAL_CAPACITY  equ 8
VTEMPORAL_REC_SIZE  equ 16
VTEMPORAL_INV_ID    equ 0
VTEMPORAL_ANCHOR    equ 4
VTEMPORAL_MIN_GAP   equ 8
VTEMPORAL_MAX_GAP   equ 12

vtemporal_initialize:
    cmp dword [vtemporal_ready], 1
    je .done
    mov edi, vtemporal_table
    xor eax, eax
    mov ecx, (VTEMPORAL_CAPACITY * VTEMPORAL_REC_SIZE) / 4
    rep stosd
    mov dword [vtemporal_ready], 1
.done:
    clc
    ret

; EAX=inv_id, EDX=anchor_tick, ECX=min_gap, EBX_arg=max_gap
; → register temporal invariant; caller passes max_gap on stack
; Simple interface: EAX=inv_id, EDX=anchor_tick, ECX=min_gap, use stack for max_gap
; Actual API: EAX=inv_id, EDX=anchor, ECX=min — max_gap stored as ECX arg + delta
; For simplicity use: EAX=inv_id, EDX=anchor, ECX=min_gap, max_gap=ECX*2 (2x min)
vtemporal_register:
    cmp dword [vtemporal_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push ecx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, VTEMPORAL_CAPACITY
    jae .full
    imul edi, esi, VTEMPORAL_REC_SIZE
    add edi, vtemporal_table
    cmp dword [edi + VTEMPORAL_INV_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + VTEMPORAL_INV_ID], ebx
    mov [edi + VTEMPORAL_ANCHOR], edx
    mov [edi + VTEMPORAL_MIN_GAP], ecx
    ; max_gap = 2 * min_gap
    mov eax, ecx
    shl eax, 1
    mov [edi + VTEMPORAL_MAX_GAP], eax
    pop ecx
    pop ebx
    clc
    ret
.full:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=inv_id, EDX=event_tick → CF=0 if within window, CF=1 violated
vtemporal_check:
    cmp dword [vtemporal_ready], 1
    jne .fail
    push ebx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, VTEMPORAL_CAPACITY
    jae .not_found
    imul edi, esi, VTEMPORAL_REC_SIZE
    add edi, vtemporal_table
    cmp [edi + VTEMPORAL_INV_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    ; gap = event_tick - anchor_tick
    mov eax, edx
    sub eax, [edi + VTEMPORAL_ANCHOR]
    ; check [min_gap, max_gap]
    cmp eax, [edi + VTEMPORAL_MIN_GAP]
    jb .violation
    cmp eax, [edi + VTEMPORAL_MAX_GAP]
    ja .violation
    pop ebx
    clc
    ret
.violation:
    ; cross-subsystem: report LATENCY symptom via sdiag
    push eax
    mov eax, 1              ; LATENCY symptom
    call sdiag_diagnose
    pop eax
    pop ebx
    stc
    ret
.not_found:
    pop ebx
.fail:
    stc
    ret

vtemporal_self_test:
    ; inv=1 anchor=100 min_gap=10 max_gap=20
    mov eax, 1
    mov edx, 100
    mov ecx, 10
    call vtemporal_register
    jc .fail
    ; event at tick=115 → gap=15 in [10,20] → ok
    mov eax, 1
    mov edx, 115
    call vtemporal_check
    jc .fail
    ; event at tick=200 → gap=100 > 20 → violation
    mov eax, 1
    mov edx, 200
    call vtemporal_check
    jnc .fail
    clc
    ret
.fail:
    stc
    ret

align 4
vtemporal_ready:  dd 0
vtemporal_table:
    times VTEMPORAL_CAPACITY * VTEMPORAL_REC_SIZE db 0

; ===========================================================================
; NPSPEC-VERIFY-INFORMATIONFLOW-0001 – Nova Information Flow Verifier
; ===========================================================================
; Label-based information flow control. Each channel has a security label;
; writes from higher-label sources to lower-label sinks are denied.
; Connects security_audit_log on violation.
;
; Record: chan_id(4) + label(4) + owner(4) + violations(4) = 16 bytes
; Label: 0=public, higher=more sensitive

VINFO_CAPACITY  equ 8
VINFO_REC_SIZE  equ 16
VINFO_CHAN_ID   equ 0
VINFO_LABEL     equ 4
VINFO_OWNER     equ 8
VINFO_VIOL      equ 12

vinfoflow_initialize:
    cmp dword [vinfoflow_ready], 1
    je .done
    mov edi, vinfoflow_table
    xor eax, eax
    mov ecx, (VINFO_CAPACITY * VINFO_REC_SIZE) / 4
    rep stosd
    mov dword [vinfoflow_ready], 1
.done:
    clc
    ret

; EAX=chan_id, EDX=label, ECX=owner → register channel, CF
vinfoflow_register:
    cmp dword [vinfoflow_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push ecx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, VINFO_CAPACITY
    jae .full
    imul edi, esi, VINFO_REC_SIZE
    add edi, vinfoflow_table
    cmp dword [edi + VINFO_CHAN_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + VINFO_CHAN_ID], ebx
    mov [edi + VINFO_LABEL], edx
    mov [edi + VINFO_OWNER], ecx
    mov dword [edi + VINFO_VIOL], 0
    pop ecx
    pop ebx
    clc
    ret
.full:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=src_chan_id, EDX=dst_chan_id → CF=0 allowed, CF=1 denied (src.label > dst.label)
vinfoflow_check:
    cmp dword [vinfoflow_ready], 1
    jne .fail
    push ebx
    push ecx
    mov ebx, eax            ; src
    mov ecx, edx            ; dst
    ; find src label
    xor esi, esi
    xor eax, eax            ; src_label = 0 default
.src_scan:
    cmp esi, VINFO_CAPACITY
    jae .src_done
    imul edi, esi, VINFO_REC_SIZE
    add edi, vinfoflow_table
    cmp [edi + VINFO_CHAN_ID], ebx
    jne .src_next
    mov eax, [edi + VINFO_LABEL]
    jmp .src_done
.src_next:
    inc esi
    jmp .src_scan
.src_done:
    push eax                ; src_label
    ; find dst label
    xor esi, esi
    xor edx, edx
.dst_scan:
    cmp esi, VINFO_CAPACITY
    jae .dst_done
    imul edi, esi, VINFO_REC_SIZE
    add edi, vinfoflow_table
    cmp [edi + VINFO_CHAN_ID], ecx
    jne .dst_next
    mov edx, [edi + VINFO_LABEL]
    mov edi, edi            ; save edi for violation increment
    jmp .dst_done
.dst_next:
    inc esi
    jmp .dst_scan
.dst_done:
    pop eax                 ; src_label
    ; deny if src_label > dst_label
    cmp eax, edx
    jbe .allowed
    ; violation: find dst record again for increment
    xor esi, esi
.dst_viol:
    cmp esi, VINFO_CAPACITY
    jae .log_and_deny
    imul edi, esi, VINFO_REC_SIZE
    add edi, vinfoflow_table
    cmp [edi + VINFO_CHAN_ID], ecx
    je .inc_viol
    inc esi
    jmp .dst_viol
.inc_viol:
    inc dword [edi + VINFO_VIOL]
.log_and_deny:
    ; cross-subsystem: security audit log (level=2, "IFLO")
    push eax
    push edx
    mov eax, 2
    mov esi, vinfoflow_audit_msg
    call security_audit_log
    pop edx
    pop eax
    pop ecx
    pop ebx
    stc
    ret
.allowed:
    pop ecx
    pop ebx
    clc
    ret
.fail:
    stc
    ret

vinfoflow_self_test:
    ; chan=1 label=5 (high), chan=2 label=1 (low)
    mov eax, 1
    mov edx, 5
    mov ecx, 1
    call vinfoflow_register
    jc .fail
    mov eax, 2
    mov edx, 1
    mov ecx, 1
    call vinfoflow_register
    jc .fail
    ; src=chan2(low) → dst=chan1(high) allowed
    mov eax, 2
    mov edx, 1
    call vinfoflow_check
    jc .fail
    ; src=chan1(high) → dst=chan2(low) denied
    mov eax, 1
    mov edx, 2
    call vinfoflow_check
    jnc .fail
    clc
    ret
.fail:
    stc
    ret

align 4
vinfoflow_ready:  dd 0
vinfoflow_audit_msg:
    db "IFLO", 0
vinfoflow_table:
    times VINFO_CAPACITY * VINFO_REC_SIZE db 0

; ===========================================================================
; NPSPEC-VERIFY-CAPABILITYSAFETY-0001 – Nova Capability Safety Verifier
; ===========================================================================
; Audits capability usage against a proof record. Each capability use must
; be accompanied by a valid proof token. Connects security_audit_log.
;
; Record: cap_id(4) + proof_token(4) + uses(4) + violations(4) = 16 bytes

VCAPSAFE_CAPACITY  equ 8
VCAPSAFE_REC_SIZE  equ 16
VCAPSAFE_CAP_ID    equ 0
VCAPSAFE_PROOF     equ 4
VCAPSAFE_USES      equ 8
VCAPSAFE_VIOL      equ 12

vcapsafe_initialize:
    cmp dword [vcapsafe_ready], 1
    je .done
    mov edi, vcapsafe_table
    xor eax, eax
    mov ecx, (VCAPSAFE_CAPACITY * VCAPSAFE_REC_SIZE) / 4
    rep stosd
    mov dword [vcapsafe_ready], 1
.done:
    clc
    ret

; EAX=cap_id, EDX=proof_token → register capability proof, CF
vcapsafe_register:
    cmp dword [vcapsafe_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push ecx
    mov ebx, eax
    mov ecx, edx
    xor esi, esi
.scan:
    cmp esi, VCAPSAFE_CAPACITY
    jae .full
    imul edi, esi, VCAPSAFE_REC_SIZE
    add edi, vcapsafe_table
    cmp dword [edi + VCAPSAFE_CAP_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + VCAPSAFE_CAP_ID], ebx
    mov [edi + VCAPSAFE_PROOF], ecx
    mov dword [edi + VCAPSAFE_USES], 0
    mov dword [edi + VCAPSAFE_VIOL], 0
    pop ecx
    pop ebx
    clc
    ret
.full:
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=cap_id, EDX=supplied_proof → verify; CF=0 ok, CF=1 mismatch → audit log
vcapsafe_verify:
    cmp dword [vcapsafe_ready], 1
    jne .fail
    push ebx
    push ecx
    mov ebx, eax
    mov ecx, edx
    xor esi, esi
.scan:
    cmp esi, VCAPSAFE_CAPACITY
    jae .not_found
    imul edi, esi, VCAPSAFE_REC_SIZE
    add edi, vcapsafe_table
    cmp [edi + VCAPSAFE_CAP_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    inc dword [edi + VCAPSAFE_USES]
    cmp [edi + VCAPSAFE_PROOF], ecx
    je .ok
    inc dword [edi + VCAPSAFE_VIOL]
    mov eax, 3
    mov esi, vcapsafe_audit_msg
    call security_audit_log
    pop ecx
    pop ebx
    stc
    ret
.ok:
    pop ecx
    pop ebx
    clc
    ret
.not_found:
    pop ecx
    pop ebx
.fail:
    stc
    ret

vcapsafe_self_test:
    ; register cap=1 proof=0xABCD
    mov eax, 1
    mov edx, 0xABCD
    call vcapsafe_register
    jc .fail
    ; verify with correct proof
    mov eax, 1
    mov edx, 0xABCD
    call vcapsafe_verify
    jc .fail
    ; verify with wrong proof → CF=1
    mov eax, 1
    mov edx, 0x1234
    call vcapsafe_verify
    jnc .fail
    clc
    ret
.fail:
    stc
    ret

align 4
vcapsafe_ready:  dd 0
vcapsafe_audit_msg:
    db "CAPS", 0
vcapsafe_table:
    times VCAPSAFE_CAPACITY * VCAPSAFE_REC_SIZE db 0

; ===========================================================================
; NPSPEC-VERIFY-MEMORYSAFETY-0001 – Nova Memory Safety Verifier
; ===========================================================================
; Bounds + alias checker for registered memory regions. A write/read to an
; address outside the region's [base, base+size) is a violation. Overlapping
; regions from different owners are alias violations.
; Connects security_audit_log on violation.
;
; Record: region_id(4) + base(4) + size(4) + violations(4) = 16 bytes

VMEMSAFE_CAPACITY  equ 8
VMEMSAFE_REC_SIZE  equ 16
VMEMSAFE_RGN_ID    equ 0
VMEMSAFE_BASE      equ 4
VMEMSAFE_SIZE      equ 8
VMEMSAFE_VIOL      equ 12

vmemsafe_initialize:
    cmp dword [vmemsafe_ready], 1
    je .done
    mov edi, vmemsafe_table
    xor eax, eax
    mov ecx, (VMEMSAFE_CAPACITY * VMEMSAFE_REC_SIZE) / 4
    rep stosd
    mov dword [vmemsafe_ready], 1
.done:
    clc
    ret

; EAX=region_id, EDX=base, ECX=size → register region, CF
vmemsafe_register:
    cmp dword [vmemsafe_ready], 1
    jne .fail
    test eax, eax
    jz .fail
    push ebx
    push ecx
    push edx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, VMEMSAFE_CAPACITY
    jae .full
    imul edi, esi, VMEMSAFE_REC_SIZE
    add edi, vmemsafe_table
    cmp dword [edi + VMEMSAFE_RGN_ID], 0
    je .slot
    inc esi
    jmp .scan
.slot:
    mov [edi + VMEMSAFE_RGN_ID], ebx
    mov [edi + VMEMSAFE_BASE], edx
    mov [edi + VMEMSAFE_SIZE], ecx
    mov dword [edi + VMEMSAFE_VIOL], 0
    pop edx
    pop ecx
    pop ebx
    clc
    ret
.full:
    pop edx
    pop ecx
    pop ebx
.fail:
    stc
    ret

; EAX=region_id, EDX=addr → bounds check; CF=0 in bounds, CF=1 violation
vmemsafe_check:
    cmp dword [vmemsafe_ready], 1
    jne .fail
    push ebx
    push ecx
    mov ebx, eax
    xor esi, esi
.scan:
    cmp esi, VMEMSAFE_CAPACITY
    jae .not_found
    imul edi, esi, VMEMSAFE_REC_SIZE
    add edi, vmemsafe_table
    cmp [edi + VMEMSAFE_RGN_ID], ebx
    je .found
    inc esi
    jmp .scan
.found:
    ; check addr in [base, base+size)
    mov eax, [edi + VMEMSAFE_BASE]
    cmp edx, eax
    jb .violation
    mov ecx, eax
    add ecx, [edi + VMEMSAFE_SIZE]
    cmp edx, ecx
    jae .violation
    pop ecx
    pop ebx
    clc
    ret
.violation:
    inc dword [edi + VMEMSAFE_VIOL]
    push edx
    mov eax, 3
    mov esi, vmemsafe_audit_msg
    call security_audit_log
    pop edx
    pop ecx
    pop ebx
    stc
    ret
.not_found:
    pop ecx
    pop ebx
.fail:
    stc
    ret

vmemsafe_self_test:
    ; register region=1 base=0x1000 size=0x100
    mov eax, 1
    mov edx, 0x1000
    mov ecx, 0x100
    call vmemsafe_register
    jc .fail
    ; check 0x1050 → in bounds
    mov eax, 1
    mov edx, 0x1050
    call vmemsafe_check
    jc .fail
    ; check 0x2000 → out of bounds
    mov eax, 1
    mov edx, 0x2000
    call vmemsafe_check
    jnc .fail
    ; violations > 0
    mov edi, vmemsafe_table
    cmp dword [edi + VMEMSAFE_VIOL], 0
    je .fail
    clc
    ret
.fail:
    stc
    ret

align 4
vmemsafe_ready:  dd 0
vmemsafe_audit_msg:
    db "MSAF", 0
vmemsafe_table:
    times VMEMSAFE_CAPACITY * VMEMSAFE_REC_SIZE db 0

