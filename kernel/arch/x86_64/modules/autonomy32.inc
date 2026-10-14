; ---------------------------------------------------------------------------
; Nova Autonomy-Subsystem §031
; NPSPEC-AUTONOMY-ADAPTATION-0001  - Selbstanpassung
; NPSPEC-AUTONOMY-CONFIGURATION-0001 - Selbstkonfiguration
; NPSPEC-AUTONOMY-DIAGNOSIS-0001   - Selbstdiagnose
; NPSPEC-AUTONOMY-HEALING-0001     - Selbstheilung
; NPSPEC-AUTONOMY-MANAGEMENT-0001  - Autonomes Management
; NPSPEC-AUTONOMY-OPTIMIZATION-0001- Selbstoptimierung
; NPSPEC-AUTONOMY-PROTECTION-0001  - Selbstschutz
; ---------------------------------------------------------------------------

; Autonomy-Policy (NPSPEC-AUTONOMY-MANAGEMENT-0001)
AUTON_POLICY_MANUAL   equ 0    ; Manuell / kein autonomes Handeln
AUTON_POLICY_ASSIST   equ 1    ; Vorschlaege machen
AUTON_POLICY_AUTO     equ 2    ; Vollautomatisch

; Healing-Aktionen (NPSPEC-AUTONOMY-HEALING-0001)
AUTON_HEAL_RESTART    equ 0
AUTON_HEAL_RECONFIGURE equ 1
AUTON_HEAL_FAILOVER   equ 2
AUTON_HEAL_ISOLATE    equ 3

; Diagnose-Ergebnis (NPSPEC-AUTONOMY-DIAGNOSIS-0001)
AUTON_DIAG_OK         equ 0
AUTON_DIAG_WARN       equ 1
AUTON_DIAG_FAULT      equ 2
AUTON_DIAG_CRITICAL   equ 3

; Ziel-Record (32 Bytes, NPSPEC-AUTONOMY-OPTIMIZATION-0001)
AUTON_GOAL_SIZE       equ 32
AG_ID                 equ 0
AG_METRIC_ID          equ 4    ; Optimiertes Metrik
AG_TARGET_VALUE       equ 8    ; Zielwert
AG_CURRENT_VALUE      equ 12   ; Aktueller Wert
AG_POLICY             equ 16   ; AUTON_POLICY_*
AG_HEAL_ACTION        equ 20   ; AUTON_HEAL_*
AG_DIAG_STATE         equ 24   ; AUTON_DIAG_*
AG_ADAPT_COUNT        equ 28   ; Anpassungen bisher

AUTON_GOAL_CAPACITY   equ 8

; ---------------------------------------------------------------------------
; autonomy_initialize
; ---------------------------------------------------------------------------
autonomy_initialize:
    push edi
    mov edi, auton_goals
    xor eax, eax
    mov ecx, (AUTON_GOAL_CAPACITY * AUTON_GOAL_SIZE) / 4
    rep stosd
    pop edi
    mov dword [auton_goal_count], 0
    mov dword [auton_policy], AUTON_POLICY_ASSIST
    ; Standard-Ziel: CPU-Auslastung < 80%
    mov eax, 0
    mov ebx, 0          ; metric: CPU
    mov ecx, 80         ; Ziel: <= 80%
    call auton_register_goal
    mov dword [auton_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; auton_register_goal: EAX=id, EBX=metric_id, ECX=target_value
; ---------------------------------------------------------------------------
auton_register_goal:
    cmp dword [auton_goal_count], AUTON_GOAL_CAPACITY
    jae .full
    push edi
    push edx
    mov edi, auton_goals
    mov edx, [auton_goal_count]
    imul edx, AUTON_GOAL_SIZE
    add edi, edx
    mov [edi + AG_ID], eax
    mov [edi + AG_METRIC_ID], ebx
    mov [edi + AG_TARGET_VALUE], ecx
    mov dword [edi + AG_CURRENT_VALUE], 0
    mov eax, [auton_policy]
    mov [edi + AG_POLICY], eax
    mov dword [edi + AG_HEAL_ACTION], AUTON_HEAL_RECONFIGURE
    mov dword [edi + AG_DIAG_STATE], AUTON_DIAG_OK
    mov dword [edi + AG_ADAPT_COUNT], 0
    inc dword [auton_goal_count]
    pop edx
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; auton_update_metric: EAX=metric_id, EBX=current_value
; Prueft Ziel und fuehrt Anpassung durch (NPSPEC-AUTONOMY-ADAPTATION-0001).
; ---------------------------------------------------------------------------
auton_update_metric:
    push esi
    push ecx
    mov esi, auton_goals
    mov ecx, [auton_goal_count]
.scan:
    test ecx, ecx
    jz .done
    cmp [esi + AG_METRIC_ID], eax
    jne .next
    mov [esi + AG_CURRENT_VALUE], ebx
    ; Diagnose (NPSPEC-AUTONOMY-DIAGNOSIS-0001)
    mov ecx, [esi + AG_TARGET_VALUE]
    cmp ebx, ecx
    jle .ok_state
    ; Ziel ueberschritten
    cmp ebx, ecx
    jg .check_critical
    jmp .ok_state
.check_critical:
    push eax
    mov eax, ecx
    add eax, eax     ; 2x Ziel = kritisch
    cmp ebx, eax
    pop eax
    jge .critical
    mov dword [esi + AG_DIAG_STATE], AUTON_DIAG_WARN
    ; Auto-Policy: Anpassung vornehmen (NPSPEC-AUTONOMY-ADAPTATION-0001)
    cmp dword [esi + AG_POLICY], AUTON_POLICY_AUTO
    jne .next
    inc dword [esi + AG_ADAPT_COUNT]
    jmp .next
.critical:
    mov dword [esi + AG_DIAG_STATE], AUTON_DIAG_CRITICAL
    jmp .next
.ok_state:
    mov dword [esi + AG_DIAG_STATE], AUTON_DIAG_OK
.next:
    add esi, AUTON_GOAL_SIZE
    push ecx
    mov ecx, [auton_goal_count]
    dec ecx
    pop ecx
    dec ecx
    jmp .scan
.done:
    pop ecx
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; auton_self_heal: EAX=goal_id
; (NPSPEC-AUTONOMY-HEALING-0001, NPSPEC-AUTONOMY-PROTECTION-0001)
; ---------------------------------------------------------------------------
auton_self_heal:
    push esi
    push ecx
    mov esi, auton_goals
    mov ecx, [auton_goal_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + AG_ID], eax
    je .found
    add esi, AUTON_GOAL_SIZE
    dec ecx
    jmp .scan
.found:
    ; Nur bei CRITICAL heilen
    cmp dword [esi + AG_DIAG_STATE], AUTON_DIAG_CRITICAL
    jne .no_action
    ; Heilaktion ausfuehren (Stub, NPSPEC-AUTONOMY-HEALING-0001)
    mov dword [esi + AG_DIAG_STATE], AUTON_DIAG_WARN
    inc dword [esi + AG_ADAPT_COUNT]
.no_action:
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
; autonomy_self_test
; ---------------------------------------------------------------------------
autonomy_self_test:
    push esi
    xor esi, esi
    cmp dword [auton_ready], 1
    je .t2
    inc esi
.t2:
    mov eax, 0       ; metric CPU
    mov ebx, 50      ; 50% OK
    call auton_update_metric
    jnc .t3
    inc esi
.t3:
    mov eax, 0
    mov ebx, 200     ; 200% kritisch
    call auton_update_metric
    jnc .t4
    inc esi
.t4:
    mov eax, 0
    call auton_self_heal
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
auton_ready:      dd 0
auton_goal_count: dd 0
auton_policy:     dd AUTON_POLICY_ASSIST

align 4
auton_goals:
    times AUTON_GOAL_CAPACITY * AUTON_GOAL_SIZE db 0
