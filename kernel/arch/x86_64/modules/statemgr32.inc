; ---------------------------------------------------------------------------
; Nova State-Manager §036
; NPSPEC-STATE-ACTUAL-0001        - Ist-Zustand
; NPSPEC-STATE-DESIRED-0001       - Soll-Zustand
; NPSPEC-STATE-GLOBAL-0001        - Globaler Zustand
; NPSPEC-STATE-HISTORY-0001       - Zustandshistorie
; NPSPEC-STATE-MACHINE-0001       - Zustandsmaschine
; NPSPEC-STATE-RECONCILIATION-0001- Abgleich
; NPSPEC-STATE-ROLLBACK-0001      - Rollback
; NPSPEC-STATE-SNAPSHOT-0001      - Snapshot
; NPSPEC-STATE-TRANSACTIONAL-0001 - Transaktionaler Zustand
; NPSPEC-STATE-VERSIONING-0001    - Versionierung
; ---------------------------------------------------------------------------

; Zustandstypen (NPSPEC-STATE-GLOBAL-0001)
SM_STATE_UNKNOWN      equ 0
SM_STATE_INITIALIZING equ 1
SM_STATE_RUNNING      equ 2
SM_STATE_DEGRADED     equ 3
SM_STATE_FAILED       equ 4
SM_STATE_SHUTDOWN     equ 5

; Reconciliation-Ergebnis (NPSPEC-STATE-RECONCILIATION-0001)
SM_RECONCILE_OK       equ 0    ; Ist == Soll
SM_RECONCILE_DRIFT    equ 1    ; Abweichung
SM_RECONCILE_CONFLICT equ 2    ; Konflikt

; State-Record (48 Bytes, NPSPEC-STATE-ACTUAL-0001 / DESIRED-0001)
SM_REC_SIZE           equ 48
SR_OBJ_ID             equ 0    ; Objekt-ID
SR_ACTUAL             equ 4    ; Ist-Zustand (NPSPEC-STATE-ACTUAL-0001)
SR_DESIRED            equ 8    ; Soll-Zustand (NPSPEC-STATE-DESIRED-0001)
SR_VERSION            equ 12   ; Versionszhaler (NPSPEC-STATE-VERSIONING-0001)
SR_FLAGS              equ 16
SR_SNAPSHOT_ID        equ 20   ; Snapshot-Ref (NPSPEC-STATE-SNAPSHOT-0001)
SR_HISTORY_HEAD       equ 24   ; Ring-Index History (NPSPEC-STATE-HISTORY-0001)
SR_MACHINE_STATE      equ 28   ; FSM-Zustand (NPSPEC-STATE-MACHINE-0001)
SR_TX_ID              equ 32   ; lfd. Transaktion (NPSPEC-STATE-TRANSACTIONAL-0001)
SR_RECONCILE_RESULT   equ 36   ; SM_RECONCILE_*
SR_TIMESTAMP          equ 40
; 44..47: reserviert

SM_CAPACITY           equ 16

; State-Flags
SM_FLAG_DIRTY         equ 0x01  ; Ungespeicherte Aenderung
SM_FLAG_LOCKED        equ 0x02  ; Gesperrt
SM_FLAG_SNAPSHOTTABLE equ 0x04  ; (NPSPEC-STATE-SNAPSHOT-0001)
SM_FLAG_ROLLBACK      equ 0x08  ; Rollback moeglich (NPSPEC-STATE-ROLLBACK-0001)

; ---------------------------------------------------------------------------
; statemgr_initialize
; ---------------------------------------------------------------------------
statemgr_initialize:
    push edi
    mov edi, sm_table
    xor eax, eax
    mov ecx, (SM_CAPACITY * SM_REC_SIZE) / 4
    rep stosd
    pop edi
    mov dword [sm_count], 0
    mov dword [sm_global_state], SM_STATE_RUNNING
    mov dword [sm_snapshot_gen], 0
    mov dword [sm_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; state_register: EAX=obj_id, EBX=initial_state
; (NPSPEC-STATE-ACTUAL-0001)
; CF=0 -> ECX=slot
; ---------------------------------------------------------------------------
state_register:
    cmp dword [sm_count], SM_CAPACITY
    jae .full
    push edi
    mov edi, sm_table
    push eax
    mov eax, [sm_count]
    imul eax, SM_REC_SIZE
    add edi, eax
    pop eax
    mov [edi + SR_OBJ_ID], eax
    mov [edi + SR_ACTUAL], ebx
    mov [edi + SR_DESIRED], ebx
    mov dword [edi + SR_VERSION], 1
    mov dword [edi + SR_FLAGS], SM_FLAG_SNAPSHOTTABLE | SM_FLAG_ROLLBACK
    mov dword [edi + SR_SNAPSHOT_ID], 0
    mov dword [edi + SR_HISTORY_HEAD], 0
    mov [edi + SR_MACHINE_STATE], ebx
    mov dword [edi + SR_TX_ID], 0
    mov dword [edi + SR_RECONCILE_RESULT], SM_RECONCILE_OK
    push eax
    mov eax, [timer_ticks]
    mov [edi + SR_TIMESTAMP], eax
    pop eax
    mov ecx, [sm_count]
    inc dword [sm_count]
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; state_set_desired: EAX=obj_id, EBX=desired_state
; (NPSPEC-STATE-DESIRED-0001)
; ---------------------------------------------------------------------------
state_set_desired:
    push esi
    push ecx
    mov esi, sm_table
    mov ecx, [sm_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + SR_OBJ_ID], eax
    je .found
    add esi, SM_REC_SIZE
    dec ecx
    jmp .scan
.found:
    mov [esi + SR_DESIRED], ebx
    or dword [esi + SR_FLAGS], SM_FLAG_DIRTY
    ; Reconciliation pruefen (NPSPEC-STATE-RECONCILIATION-0001)
    cmp ebx, [esi + SR_ACTUAL]
    je .reconciled
    mov dword [esi + SR_RECONCILE_RESULT], SM_RECONCILE_DRIFT
    jmp .done
.reconciled:
    mov dword [esi + SR_RECONCILE_RESULT], SM_RECONCILE_OK
.done:
    inc dword [esi + SR_VERSION]
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
; state_reconcile: EAX=obj_id -> EBX=result
; (NPSPEC-STATE-RECONCILIATION-0001)
; ---------------------------------------------------------------------------
state_reconcile:
    push esi
    push ecx
    mov esi, sm_table
    mov ecx, [sm_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + SR_OBJ_ID], eax
    je .found
    add esi, SM_REC_SIZE
    dec ecx
    jmp .scan
.found:
    mov ebx, [esi + SR_DESIRED]
    ; Ist-Zustand auf Soll setzen
    mov [esi + SR_ACTUAL], ebx
    mov [esi + SR_MACHINE_STATE], ebx
    and dword [esi + SR_FLAGS], ~SM_FLAG_DIRTY
    mov dword [esi + SR_RECONCILE_RESULT], SM_RECONCILE_OK
    mov ebx, SM_RECONCILE_OK
    pop ecx
    pop esi
    clc
    ret
.not_found:
    mov ebx, SM_RECONCILE_CONFLICT
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; state_snapshot: EAX=obj_id -> EBX=snapshot_id
; (NPSPEC-STATE-SNAPSHOT-0001)
; ---------------------------------------------------------------------------
state_snapshot:
    push esi
    push ecx
    mov esi, sm_table
    mov ecx, [sm_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + SR_OBJ_ID], eax
    je .found
    add esi, SM_REC_SIZE
    dec ecx
    jmp .scan
.found:
    inc dword [sm_snapshot_gen]
    mov ebx, [sm_snapshot_gen]
    mov [esi + SR_SNAPSHOT_ID], ebx
    pop ecx
    pop esi
    clc
    ret
.not_found:
    xor ebx, ebx
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; statemgr_self_test
; ---------------------------------------------------------------------------
statemgr_self_test:
    push esi
    cmp dword [sm_ready], 1
    pop esi
    jne .fail
    clc
    ret
.fail:
    stc
    ret

align 4
sm_ready:         dd 0
sm_count:         dd 0
sm_global_state:  dd SM_STATE_RUNNING
sm_snapshot_gen:  dd 0

align 4
sm_table:
    times SM_CAPACITY * SM_REC_SIZE db 0
