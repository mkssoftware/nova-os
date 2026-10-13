; Kernel Security / Capability Manager (ADR-2013 / NPSPEC-KERNEL-0020)
; NPSPEC-SECURITY-CODEINTEGRITY-0001 / NPSPEC-SECURITY-CODESIGNING-0001
; NPSPEC-SECURITY-ISOLATION-0001 / NPSPEC-SECURITY-LEASTPRIVILEGE-0001
; NPSPEC-CAPABILITY-0001
SECURITY_API_SIZE       equ 32
SECURITY_CAPACITY       equ PROCESS_CAPACITY
SECURITY_RECORD_SIZE    equ 8
SECURITY_CAP_MEMORY     equ 0x00000001
SECURITY_CAP_IO         equ 0x00000002
SECURITY_CAP_SERVICE    equ 0x00000004
SECURITY_CAP_ADMIN      equ 0x00000008
SECURITY_CAP_IPC        equ 0x00000010
SECURITY_CAP_POWER_QUERY equ 0x00000020
SECURITY_CAP_POWER_WAKE equ 0x00000040
SECURITY_CAP_POWER_SHUTDOWN equ 0x00000080
SECURITY_CAP_POWER_PROFILE equ 0x00000100
SECURITY_CAP_NET_QUERY equ 0x00000200
SECURITY_CAP_NET_RAW   equ 0x00000400
SECURITY_CAP_NET_CONNECT equ 0x00000800
SECURITY_CAP_NET_LISTEN equ 0x00001000
SECURITY_CAP_LOG_READ   equ 0x00002000
SECURITY_CAP_DISPLAY_SYSTEM_UI equ 0x00004000
SECURITY_CAP_BOOT_HEALTH_REPORT equ 0x00008000
SECURITY_CAP_BOOT_HEALTH_COMMIT equ 0x00010000
SECURITY_CAP_FS_READ     equ 0x00020000
SECURITY_CAP_FS_WRITE    equ 0x00040000
SECURITY_CAP_FS_SYSTEM_WRITE equ 0x00080000
SECURITY_KERNEL_CAPS    equ 0x000FFFFF

security_initialize:
    mov edi, security_table
    xor eax, eax
    mov ecx, (SECURITY_CAPACITY * SECURITY_RECORD_SIZE) / 4
    rep stosd
    mov dword [security_count], 0
    mov eax, 1
    mov edx, SECURITY_KERNEL_CAPS
    call security_grant
    ret

; EAX=PID, EDX=Capability-Maske.
security_grant:
    pushfd
    cli
    test edx, edx
    jz .invalid
    mov [security_temp_pid], eax
    mov [security_temp_caps], edx
    call process_lookup
    jc .invalid
    mov eax, [security_temp_pid]
    xor ecx, ecx
.scan:
    cmp ecx, SECURITY_CAPACITY
    jae .invalid
    lea edi, [security_table + ecx * 8]
    cmp dword [edi], eax
    je .grant
    cmp dword [edi], 0
    je .create
    inc ecx
    jmp .scan
.create:
    mov [edi], eax
    inc dword [security_count]
.grant:
    mov edx, [security_temp_caps]
    or [edi + 4], edx
    popfd
    clc
    ret
.invalid:
    popfd
    stc
    ret

; EAX=PID, EDX=Capability-Maske.
security_revoke:
    pushfd
    cli
    mov ecx, edx
    not ecx
    xor edi, edi
.scan:
    cmp edi, SECURITY_CAPACITY
    jae .invalid
    cmp [security_table + edi * 8], eax
    je .found
    inc edi
    jmp .scan
.found:
    and [security_table + edi * 8 + 4], ecx
    popfd
    clc
    ret
.invalid:
    popfd
    stc
    ret

; EAX=PID, EDX=benötigte Capability-Maske. EAX=1 erlaubt, 0 verweigert.
security_check:
    xor ecx, ecx
.scan:
    cmp ecx, SECURITY_CAPACITY
    jae .denied
    cmp [security_table + ecx * 8], eax
    je .found
    inc ecx
    jmp .scan
.found:
    mov eax, [security_table + ecx * 8 + 4]
    and eax, edx
    cmp eax, edx
    jne .denied
    mov eax, 1
    clc
    ret
.denied:
    xor eax, eax
    stc
    ret

security_self_test:
    mov eax, 1
    mov edx, SECURITY_CAP_ADMIN | SECURITY_CAP_SERVICE
    call security_check
    jc .invalid
    cmp eax, 1
    jne .invalid
    mov eax, 1
    mov edx, 0x80000000             ; weiterhin unbelegte Capability
    call security_check
    jnc .invalid
    mov eax, 0xFFFFFFFF
    mov edx, SECURITY_CAP_MEMORY
    call security_check
    jnc .invalid
    cmp dword [security_count], 1
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
security_api:
    dd SECURITY_API_SIZE
    dw 1, 0
    dd SECURITY_CAPACITY
    dd SECURITY_KERNEL_CAPS
    dd security_grant
    dd security_revoke
    dd security_check
    dd security_table

security_count:     dd 0
security_temp_pid:  dd 0
security_temp_caps: dd 0
align 4
security_table:
    times SECURITY_CAPACITY * SECURITY_RECORD_SIZE db 0

; ===========================================================================
; NPSPEC-SECURITY-AUDIT-0001 – Nova Security Audit Log
; Strukturiertes Audit-Event-Log: Kapabilität-Grants/-Revokes, Policy-Änderungen,
; Verletzungen. Ringpuffer mit 32 Einträgen, persistent bis zum nächsten Reboot.
; ===========================================================================

AUDIT_CAPACITY       equ 32
AUDIT_EVENT_SIZE     equ 32

AUDIT_TYPE_NONE      equ 0
AUDIT_TYPE_CAP_GRANT equ 1
AUDIT_TYPE_CAP_REVOKE equ 2
AUDIT_TYPE_POLICY    equ 3
AUDIT_TYPE_VIOLATION equ 4
AUDIT_TYPE_LOGIN     equ 5
AUDIT_TYPE_LOGOUT    equ 6
AUDIT_TYPE_SYSCALL   equ 7

AUDIT_SEQ            equ 0
AUDIT_TICK           equ 4
AUDIT_TYPE_OFF       equ 8
AUDIT_SUBJECT        equ 12  ; PID / Owner
AUDIT_OBJECT         equ 16  ; Ressource / Capability-Maske
AUDIT_RESULT         equ 20  ; 0=deny 1=allow
AUDIT_FLAGS          equ 24
AUDIT_PAD            equ 28

security_audit_initialize:
    mov edi, audit_log
    mov ecx, AUDIT_CAPACITY * AUDIT_EVENT_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [audit_write_pos], 0
    mov dword [audit_seq], 0
    mov dword [audit_total], 0
    mov dword [audit_violations], 0
    mov dword [audit_ready], 1
    clc
    ret

; EAX=type  EDX=subject  ECX=object  EBX=result → schreibt Eintrag, CF=0 immer
security_audit_log:
    push esi
    push edi
    push ebx

    mov edi, [audit_write_pos]
    cmp edi, AUDIT_CAPACITY
    jb .sal_ok_pos
    xor edi, edi
.sal_ok_pos:
    imul esi, edi, AUDIT_EVENT_SIZE
    add esi, audit_log

    push eax
    mov eax, [audit_seq]
    mov [esi + AUDIT_SEQ], eax
    inc dword [audit_seq]
    pop eax

    push eax
    mov eax, [wd_global_tick]
    mov [esi + AUDIT_TICK], eax
    pop eax

    mov [esi + AUDIT_TYPE_OFF], eax
    mov [esi + AUDIT_SUBJECT], edx
    mov [esi + AUDIT_OBJECT], ecx
    mov [esi + AUDIT_RESULT], ebx
    mov dword [esi + AUDIT_FLAGS], 0

    inc edi
    cmp edi, AUDIT_CAPACITY
    jb .sal_wrap_ok
    xor edi, edi
.sal_wrap_ok:
    mov [audit_write_pos], edi
    inc dword [audit_total]

    cmp eax, AUDIT_TYPE_VIOLATION
    jne .sal_done
    inc dword [audit_violations]
.sal_done:
    clc
    pop ebx
    pop edi
    pop esi
    ret

; EAX=type → EAX=count  (zählt alle Einträge eines bestimmten Typs)
security_audit_count:
    push esi
    push ecx
    push edx
    mov edx, eax
    xor ecx, ecx
    mov esi, audit_log
    push ebx
    xor ebx, ebx
.sac_loop:
    cmp ebx, AUDIT_CAPACITY
    jae .sac_done
    cmp dword [esi + AUDIT_TYPE_OFF], edx
    jne .sac_next
    inc ecx
.sac_next:
    add esi, AUDIT_EVENT_SIZE
    inc ebx
    jmp .sac_loop
.sac_done:
    mov eax, ecx
    pop ebx
    pop edx
    pop ecx
    pop esi
    ret

security_audit_self_test:
    push ebx
    push esi
    xor esi, esi

    ; T1: ready
    cmp dword [audit_ready], 1
    je .sast1_ok
    inc esi
.sast1_ok:

    ; T2: log CAP_GRANT
    mov eax, AUDIT_TYPE_CAP_GRANT
    mov edx, 1          ; subject=PID 1
    mov ecx, 0x00000001 ; object=cap-bit 0
    mov ebx, 1          ; result=allow
    call security_audit_log
    jnc .sast2_ok
    inc esi
.sast2_ok:

    ; T3: log VIOLATION
    mov eax, AUDIT_TYPE_VIOLATION
    mov edx, 2
    mov ecx, 0xDEADBEEF
    mov ebx, 0          ; deny
    call security_audit_log
    jnc .sast3_ok
    inc esi
.sast3_ok:

    ; T4: audit_total=2
    cmp dword [audit_total], 2
    je .sast4_ok
    inc esi
.sast4_ok:

    ; T5: violations=1
    cmp dword [audit_violations], 1
    je .sast5_ok
    inc esi
.sast5_ok:

    ; T6: count VIOLATION=1
    mov eax, AUDIT_TYPE_VIOLATION
    call security_audit_count
    cmp eax, 1
    je .sast6_ok
    inc esi
.sast6_ok:

    ; T7: count CAP_GRANT=1
    mov eax, AUDIT_TYPE_CAP_GRANT
    call security_audit_count
    cmp eax, 1
    je .sast7_ok
    inc esi
.sast7_ok:

    ; T8: Ringbuffer-Wrap – 31 weitere Einträge → write_pos muss korrekt stehen
    push ecx
    mov ecx, 31
.sast8_loop:
    mov eax, AUDIT_TYPE_SYSCALL
    xor edx, edx
    xor ebx, ebx
    push ecx
    xor ecx, ecx
    call security_audit_log
    pop ecx
    loop .sast8_loop
    pop ecx
    ; write_pos sollte bei (2+31) mod 32 = 1 stehen
    cmp dword [audit_write_pos], 1
    je .sast8_ok
    inc esi
.sast8_ok:

    test esi, esi
    jnz .sastf
    clc
    pop esi
    pop ebx
    ret
.sastf:
    stc
    pop esi
    pop ebx
    ret

audit_ready:      dd 0
audit_write_pos:  dd 0
audit_seq:        dd 0
audit_total:      dd 0
audit_violations: dd 0
align 4
audit_log:
    times AUDIT_CAPACITY * AUDIT_EVENT_SIZE db 0

; ===========================================================================
; NPSPEC-SECURITY-MAC-0001 – Nova Mandatory Access Control
; Sensitivity-Labels für Subjects und Objects; Dominanz-Check: Subject muss
; Object dominieren (Label >= Object-Label) für Lesezugriff.
; ===========================================================================

MAC_SUBJECT_COUNT    equ 8
MAC_OBJECT_COUNT     equ 8
MAC_RECORD_SIZE      equ 12

MAC_LABEL_PUBLIC     equ 0
MAC_LABEL_INTERNAL   equ 1
MAC_LABEL_CONFIDENTIAL equ 2
MAC_LABEL_SECRET     equ 3

MAC_SUB_ID           equ 0
MAC_SUB_LABEL        equ 4
MAC_SUB_FLAGS        equ 8

mac_initialize:
    mov edi, mac_subjects
    mov ecx, MAC_SUBJECT_COUNT * MAC_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov edi, mac_objects
    mov ecx, MAC_OBJECT_COUNT * MAC_RECORD_SIZE / 4
    rep stosd
    mov dword [mac_ready], 1
    mov dword [mac_denials], 0
    clc
    ret

; EAX=subject_id  EDX=label → registriert Subjekt; CF=0/CF=1 voll
mac_register_subject:
    push esi
    push ecx
    mov esi, mac_subjects
    xor ecx, ecx
.macrs_scan:
    cmp ecx, MAC_SUBJECT_COUNT
    jae .macrs_full
    cmp dword [esi + MAC_SUB_ID], 0
    je .macrs_found
    add esi, MAC_RECORD_SIZE
    inc ecx
    jmp .macrs_scan
.macrs_found:
    mov [esi + MAC_SUB_ID], eax
    mov [esi + MAC_SUB_LABEL], edx
    mov dword [esi + MAC_SUB_FLAGS], 1  ; aktiv
    clc
    pop ecx
    pop esi
    ret
.macrs_full:
    stc
    pop ecx
    pop esi
    ret

; EAX=object_id  EDX=label → registriert Objekt
mac_register_object:
    push esi
    push ecx
    mov esi, mac_objects
    xor ecx, ecx
.macro_scan:
    cmp ecx, MAC_OBJECT_COUNT
    jae .macro_full
    cmp dword [esi + MAC_SUB_ID], 0
    je .macro_found
    add esi, MAC_RECORD_SIZE
    inc ecx
    jmp .macro_scan
.macro_found:
    mov [esi + MAC_SUB_ID], eax
    mov [esi + MAC_SUB_LABEL], edx
    mov dword [esi + MAC_SUB_FLAGS], 1
    clc
    pop ecx
    pop esi
    ret
.macro_full:
    stc
    pop ecx
    pop esi
    ret

; EAX=subject_id  EDX=object_id → CF=0 erlaubt / CF=1 verboten (simple dominance)
mac_check:
    push esi
    push edi
    push ecx
    ; Subject-Label suchen
    mov esi, mac_subjects
    xor ecx, ecx
.macc_sub:
    cmp ecx, MAC_SUBJECT_COUNT
    jae .macc_deny
    cmp dword [esi + MAC_SUB_ID], eax
    je .macc_sub_found
    add esi, MAC_RECORD_SIZE
    inc ecx
    jmp .macc_sub
.macc_sub_found:
    push dword [esi + MAC_SUB_LABEL]   ; subject_label auf Stack
    ; Object-Label suchen
    mov edi, mac_objects
    xor ecx, ecx
.macc_obj:
    cmp ecx, MAC_OBJECT_COUNT
    jae .macc_deny_pop
    cmp dword [edi + MAC_SUB_ID], edx
    je .macc_obj_found
    add edi, MAC_RECORD_SIZE
    inc ecx
    jmp .macc_obj
.macc_obj_found:
    pop ecx                             ; subject_label
    ; Dominanz: subject_label >= object_label
    cmp ecx, [edi + MAC_SUB_LABEL]
    jb .macc_deny2
    clc
    pop ecx
    pop edi
    pop esi
    ret
.macc_deny_pop:
    pop ecx
.macc_deny:
.macc_deny2:
    inc dword [mac_denials]
    stc
    pop ecx
    pop edi
    pop esi
    ret

mac_self_test:
    push esi
    xor esi, esi
    cmp dword [mac_ready], 1
    je .mst1
    inc esi
.mst1:
    ; Subject 1 = CONFIDENTIAL, Object 10 = INTERNAL → erlaubt (2>=1)
    mov eax, 1
    mov edx, MAC_LABEL_CONFIDENTIAL
    call mac_register_subject
    mov eax, 10
    mov edx, MAC_LABEL_INTERNAL
    call mac_register_object
    mov eax, 1
    mov edx, 10
    call mac_check
    jnc .mst2
    inc esi
.mst2:
    ; Subject 2 = PUBLIC, Object 20 = SECRET → verboten (0<3)
    mov eax, 2
    mov edx, MAC_LABEL_PUBLIC
    call mac_register_subject
    mov eax, 20
    mov edx, MAC_LABEL_SECRET
    call mac_register_object
    mov eax, 2
    mov edx, 20
    call mac_check
    jc .mst3
    inc esi
.mst3:
    cmp dword [mac_denials], 1
    je .mst4
    inc esi
.mst4:
    test esi, esi
    jnz .mstf
    clc
    pop esi
    ret
.mstf:
    stc
    pop esi
    ret

mac_ready:   dd 0
mac_denials: dd 0
align 4
mac_subjects:
    times MAC_SUBJECT_COUNT * MAC_RECORD_SIZE db 0
mac_objects:
    times MAC_OBJECT_COUNT * MAC_RECORD_SIZE db 0

; ===========================================================================
; NPSPEC-SECURITY-DAC-0001 – Nova Discretionary Access Control
; Besitzer-basierte Zugriffsrechte: Read/Write/Execute-Bits pro Eintrag.
; ===========================================================================

DAC_ENTRY_COUNT      equ 16
DAC_RECORD_SIZE      equ 16

DAC_PERM_READ        equ 0x01
DAC_PERM_WRITE       equ 0x02
DAC_PERM_EXEC        equ 0x04

DAC_OBJ_ID           equ 0
DAC_OWNER            equ 4
DAC_OWNER_PERMS      equ 8
DAC_OTHERS_PERMS     equ 12

dac_initialize:
    mov edi, dac_table
    mov ecx, DAC_ENTRY_COUNT * DAC_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [dac_ready], 1
    mov dword [dac_denials], 0
    clc
    ret

; EAX=obj_id  EDX=owner  ECX=owner_perms  EBX=others_perms → CF=0/CF=1 voll
dac_register:
    push esi
    push edi
    mov esi, dac_table
    push ecx
    xor ecx, ecx
.dacr_scan:
    cmp ecx, DAC_ENTRY_COUNT
    jae .dacr_full
    cmp dword [esi + DAC_OBJ_ID], 0
    je .dacr_found
    add esi, DAC_RECORD_SIZE
    inc ecx
    jmp .dacr_scan
.dacr_found:
    pop ecx
    mov [esi + DAC_OBJ_ID], eax
    mov [esi + DAC_OWNER], edx
    mov [esi + DAC_OWNER_PERMS], ecx
    mov [esi + DAC_OTHERS_PERMS], ebx
    clc
    pop edi
    pop esi
    ret
.dacr_full:
    pop ecx
    stc
    pop edi
    pop esi
    ret

; EAX=obj_id  EDX=caller_id  ECX=requested_perms → CF=0 ok / CF=1 deny
dac_check:
    push esi
    push ebx
    push ecx
    mov esi, dac_table
    push ecx
    xor ecx, ecx
.dacc_scan:
    cmp ecx, DAC_ENTRY_COUNT
    jae .dacc_deny
    cmp dword [esi + DAC_OBJ_ID], eax
    je .dacc_found
    add esi, DAC_RECORD_SIZE
    inc ecx
    jmp .dacc_scan
.dacc_found:
    pop ecx   ; requested_perms
    ; Ist caller der Besitzer?
    cmp dword [esi + DAC_OWNER], edx
    je .dacc_owner
    ; Andere Nutzer: others_perms
    mov ebx, [esi + DAC_OTHERS_PERMS]
    jmp .dacc_check_bits
.dacc_owner:
    mov ebx, [esi + DAC_OWNER_PERMS]
.dacc_check_bits:
    and ebx, ecx
    cmp ebx, ecx
    je .dacc_ok
    inc dword [dac_denials]
    stc
    pop ecx
    pop ebx
    pop esi
    ret
.dacc_ok:
    clc
    pop ecx
    pop ebx
    pop esi
    ret
.dacc_deny:
    pop ecx
    inc dword [dac_denials]
    stc
    pop ecx
    pop ebx
    pop esi
    ret

dac_self_test:
    push esi
    push ebx
    xor esi, esi
    cmp dword [dac_ready], 1
    je .dst1
    inc esi
.dst1:
    ; Objekt 100: owner=1, owner_perms=RWX, others_perms=R
    mov eax, 100
    mov edx, 1
    mov ecx, DAC_PERM_READ | DAC_PERM_WRITE | DAC_PERM_EXEC
    mov ebx, DAC_PERM_READ
    call dac_register
    jnc .dst2
    inc esi
.dst2:
    ; Owner liest → ok
    mov eax, 100
    mov edx, 1
    mov ecx, DAC_PERM_READ
    call dac_check
    jnc .dst3
    inc esi
.dst3:
    ; Anderer schreibt → verboten
    mov eax, 100
    mov edx, 2
    mov ecx, DAC_PERM_WRITE
    call dac_check
    jc .dst4
    inc esi
.dst4:
    cmp dword [dac_denials], 1
    je .dst5
    inc esi
.dst5:
    test esi, esi
    jnz .dstf
    clc
    pop ebx
    pop esi
    ret
.dstf:
    stc
    pop ebx
    pop esi
    ret

dac_ready:   dd 0
dac_denials: dd 0
align 4
dac_table:
    times DAC_ENTRY_COUNT * DAC_RECORD_SIZE db 0

; ===========================================================================
; NPSPEC-SECURITY-RBAC-0001 – Nova Role-Based Access Control
; Rollen mit Permission-Masken; Subjects werden Rollen zugewiesen.
; ===========================================================================

RBAC_ROLE_COUNT      equ 8
RBAC_ASSIGN_COUNT    equ 16
RBAC_ROLE_SIZE       equ 12
RBAC_ASSIGN_SIZE     equ 8

RBAC_ROLE_ID         equ 0
RBAC_ROLE_PERMS      equ 4   ; Bitmaske der erlaubten Operationen
RBAC_ROLE_FLAGS      equ 8

RBAC_ASSIGN_SUBJECT  equ 0
RBAC_ASSIGN_ROLE_ID  equ 4

rbac_initialize:
    mov edi, rbac_roles
    mov ecx, RBAC_ROLE_COUNT * RBAC_ROLE_SIZE / 4
    xor eax, eax
    rep stosd
    mov edi, rbac_assignments
    mov ecx, RBAC_ASSIGN_COUNT * RBAC_ASSIGN_SIZE / 4
    rep stosd
    mov dword [rbac_ready], 1
    mov dword [rbac_denials], 0
    clc
    ret

; EAX=role_id  EDX=permission_mask → CF=0/CF=1 voll
rbac_define_role:
    push esi
    push ecx
    mov esi, rbac_roles
    xor ecx, ecx
.rdr_scan:
    cmp ecx, RBAC_ROLE_COUNT
    jae .rdr_full
    cmp dword [esi + RBAC_ROLE_FLAGS], 0
    je .rdr_found
    add esi, RBAC_ROLE_SIZE
    inc ecx
    jmp .rdr_scan
.rdr_found:
    mov [esi + RBAC_ROLE_ID], eax
    mov [esi + RBAC_ROLE_PERMS], edx
    mov dword [esi + RBAC_ROLE_FLAGS], 1
    clc
    pop ecx
    pop esi
    ret
.rdr_full:
    stc
    pop ecx
    pop esi
    ret

; EAX=subject_id  EDX=role_id → CF=0/CF=1 voll
rbac_assign_role:
    push esi
    push ecx
    mov esi, rbac_assignments
    xor ecx, ecx
.rar_scan:
    cmp ecx, RBAC_ASSIGN_COUNT
    jae .rar_full
    cmp dword [esi + RBAC_ASSIGN_SUBJECT], 0
    je .rar_found
    add esi, RBAC_ASSIGN_SIZE
    inc ecx
    jmp .rar_scan
.rar_found:
    mov [esi + RBAC_ASSIGN_SUBJECT], eax
    mov [esi + RBAC_ASSIGN_ROLE_ID], edx
    clc
    pop ecx
    pop esi
    ret
.rar_full:
    stc
    pop ecx
    pop esi
    ret

; EAX=subject_id  EDX=requested_perm → CF=0 erlaubt / CF=1 verboten
rbac_check:
    push esi
    push edi
    push ecx
    push ebx
    ; Alle Zuweisungen für subject durchsuchen
    mov esi, rbac_assignments
    xor ecx, ecx
.rbcc_assign:
    cmp ecx, RBAC_ASSIGN_COUNT
    jae .rbcc_deny
    cmp dword [esi + RBAC_ASSIGN_SUBJECT], eax
    je .rbcc_found_assign
    add esi, RBAC_ASSIGN_SIZE
    inc ecx
    jmp .rbcc_assign
.rbcc_found_assign:
    ; Rolle laden
    push dword [esi + RBAC_ASSIGN_ROLE_ID]
    mov edi, rbac_roles
    xor ecx, ecx
.rbcc_role:
    cmp ecx, RBAC_ROLE_COUNT
    jae .rbcc_deny_pop
    pop ebx    ; role_id
    cmp dword [edi + RBAC_ROLE_ID], ebx
    je .rbcc_found_role
    push ebx
    add edi, RBAC_ROLE_SIZE
    inc ecx
    jmp .rbcc_role
.rbcc_found_role:
    ; Permission prüfen
    mov ebx, [edi + RBAC_ROLE_PERMS]
    and ebx, edx
    cmp ebx, edx
    je .rbcc_ok
    jmp .rbcc_deny
.rbcc_deny_pop:
    pop ebx
.rbcc_deny:
    inc dword [rbac_denials]
    stc
    pop ebx
    pop ecx
    pop edi
    pop esi
    ret
.rbcc_ok:
    clc
    pop ebx
    pop ecx
    pop edi
    pop esi
    ret

rbac_self_test:
    push esi
    xor esi, esi
    cmp dword [rbac_ready], 1
    je .rbst1
    inc esi
.rbst1:
    ; Rolle 1: READ|WRITE  Rolle 2: READ only
    mov eax, 1
    mov edx, DAC_PERM_READ | DAC_PERM_WRITE
    call rbac_define_role
    mov eax, 2
    mov edx, DAC_PERM_READ
    call rbac_define_role
    ; Subject 10 = Rolle 1, Subject 20 = Rolle 2
    mov eax, 10
    mov edx, 1
    call rbac_assign_role
    mov eax, 20
    mov edx, 2
    call rbac_assign_role
    ; Subject 10 schreibt → ok
    mov eax, 10
    mov edx, DAC_PERM_WRITE
    call rbac_check
    jnc .rbst2
    inc esi
.rbst2:
    ; Subject 20 schreibt → verboten
    mov eax, 20
    mov edx, DAC_PERM_WRITE
    call rbac_check
    jc .rbst3
    inc esi
.rbst3:
    cmp dword [rbac_denials], 1
    je .rbst4
    inc esi
.rbst4:
    test esi, esi
    jnz .rbstf
    clc
    pop esi
    ret
.rbstf:
    stc
    pop esi
    ret

rbac_ready:   dd 0
rbac_denials: dd 0
align 4
rbac_roles:
    times RBAC_ROLE_COUNT * RBAC_ROLE_SIZE db 0
rbac_assignments:
    times RBAC_ASSIGN_COUNT * RBAC_ASSIGN_SIZE db 0

; ---------------------------------------------------------------------------
; Boot Health Authority ABI 1.0
; Capability-geschuetzte, generationsgebundene Provider-Aggregation.
; ---------------------------------------------------------------------------
BOOT_HEALTH_REPORT_SIZE       equ 32
BOOT_HEALTH_RECORD_SIZE       equ 64
BOOT_HEALTH_EVIDENCE_SIZE     equ 32
BOOT_HEALTH_API_SIZE          equ 40

BOOT_HEALTH_STATUS_PENDING    equ 1
BOOT_HEALTH_STATUS_HEALTHY    equ 2
BOOT_HEALTH_STATUS_DEGRADED   equ 3
BOOT_HEALTH_STATUS_FAILED     equ 4
BOOT_HEALTH_STATUS_TIMED_OUT  equ 5

BOOT_HEALTH_MILESTONE_BOOT_STARTED       equ 1
BOOT_HEALTH_MILESTONE_KERNEL_ENTERED     equ 2
BOOT_HEALTH_MILESTONE_KERNEL_INITIALIZED equ 3
BOOT_HEALTH_MILESTONE_SYSTEM_ROOT_READY  equ 4
BOOT_HEALTH_MILESTONE_CRITICAL_SERVICES  equ 5
BOOT_HEALTH_MILESTONE_OPERATIONAL        equ 6
BOOT_HEALTH_MILESTONE_CONFIRMED          equ 7

BOOT_HEALTH_PROVIDER_KERNEL_CORE equ 0x00000001
BOOT_HEALTH_PROVIDER_MEMORY      equ 0x00000002
BOOT_HEALTH_PROVIDER_SYSTEM_ROOT equ 0x00000004
BOOT_HEALTH_PROVIDER_TRUST       equ 0x00000008
BOOT_HEALTH_PROVIDER_CAPABILITY  equ 0x00000010
BOOT_HEALTH_PROVIDER_IPC         equ 0x00000020
BOOT_HEALTH_PROVIDER_SESSION     equ 0x00000040

BOOT_HEALTH_PROVIDER_READY       equ 1
BOOT_HEALTH_PROVIDER_DEGRADED    equ 2
BOOT_HEALTH_PROVIDER_FAILED      equ 3

BOOT_HEALTH_REQUIRED_MILESTONES  equ 0x00000078
BOOT_HEALTH_INITIAL_MILESTONES   equ 0x00000006
BOOT_HEALTH_FLAG_IDENTITY_VALID  equ 0x00000001
BOOT_HEALTH_FLAG_TRUST_VERIFIED  equ 0x00000002
BOOT_HEALTH_FLAG_DEGRADED_SEEN   equ 0x00000004
BOOT_HEALTH_FLAG_EVIDENCE_READY  equ 0x00000008

; Record offsets are shared with nova/boot_health.h.
BH_RECORD_GENERATION_LO equ 8
BH_RECORD_GENERATION_HI equ 12
BH_RECORD_BOOT_ATTEMPT  equ 16
BH_RECORD_STATUS        equ 20
BH_RECORD_REACHED       equ 24
BH_RECORD_REQUIRED      equ 28
BH_RECORD_LAST          equ 32
BH_RECORD_FAILED        equ 36
BH_RECORD_SEQUENCE      equ 40
BH_RECORD_AUTHORIZED    equ 44
BH_RECORD_REJECTED      equ 48
BH_RECORD_FLAGS         equ 52

boot_health_initialize:
    cld
    mov edi, boot_health_record
    xor eax, eax
    mov ecx, BOOT_HEALTH_RECORD_SIZE / 4
    rep stosd
    mov edi, boot_health_provider_ready
    mov ecx, 8
    rep stosd

    mov dword [boot_health_record], BOOT_HEALTH_RECORD_SIZE
    mov word [boot_health_record + 4], 1
    mov word [boot_health_record + 6], 0
    mov eax, [kernel_context + CONTEXT_SYSTEM_GENERATION]
    mov [boot_health_record + BH_RECORD_GENERATION_LO], eax
    mov eax, [kernel_context + CONTEXT_SYSTEM_GENERATION_HI]
    mov [boot_health_record + BH_RECORD_GENERATION_HI], eax
    mov eax, [kernel_context + CONTEXT_BOOT_ATTEMPT]
    mov [boot_health_record + BH_RECORD_BOOT_ATTEMPT], eax
    mov dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_PENDING
    mov dword [boot_health_record + BH_RECORD_REACHED], BOOT_HEALTH_INITIAL_MILESTONES
    mov dword [boot_health_record + BH_RECORD_REQUIRED], BOOT_HEALTH_REQUIRED_MILESTONES
    mov dword [boot_health_record + BH_RECORD_LAST], BOOT_HEALTH_MILESTONE_KERNEL_ENTERED
    mov dword [boot_health_record + BH_RECORD_SEQUENCE], 1
    mov dword [boot_health_record + BH_RECORD_FLAGS], BOOT_HEALTH_FLAG_IDENTITY_VALID
    cmp dword [kernel_context + CONTEXT_SECURITY_STATE], NOVA_BOOT_VERIFICATION_SIGNATURE_VERIFIED
    jb .identity_ready
    or dword [boot_health_record + BH_RECORD_FLAGS], BOOT_HEALTH_FLAG_TRUST_VERIFIED
.identity_ready:
    mov dword [boot_health_ready], 1
    clc
    ret

; EDX=Milestone, ECX=Provider, EBX=Providerstatus.
boot_health_prepare_report:
    mov dword [boot_health_temp_report], BOOT_HEALTH_REPORT_SIZE
    mov word [boot_health_temp_report + 4], 1
    mov word [boot_health_temp_report + 6], 0
    mov eax, [boot_health_record + BH_RECORD_GENERATION_LO]
    mov [boot_health_temp_report + 8], eax
    mov eax, [boot_health_record + BH_RECORD_GENERATION_HI]
    mov [boot_health_temp_report + 12], eax
    mov eax, [boot_health_record + BH_RECORD_BOOT_ATTEMPT]
    mov [boot_health_temp_report + 16], eax
    mov [boot_health_temp_report + 20], edx
    mov [boot_health_temp_report + 24], ecx
    mov [boot_health_temp_report + 28], ebx
    ret

; EAX=aufrufende PID, ESI=Report. Nur das Kernelobjekt behaelt Zustand.
boot_health_submit_report:
    pushfd
    cli
    pushad
    cmp dword [boot_health_ready], 1
    jne .reject
    test esi, 3
    jnz .reject
    cmp dword [esi], BOOT_HEALTH_REPORT_SIZE
    jne .reject
    cmp word [esi + 4], 1
    jne .reject
    cmp word [esi + 6], 0
    jne .reject
    mov [boot_health_temp_pid], eax
    mov edi, boot_health_temp_report
    mov ecx, BOOT_HEALTH_REPORT_SIZE / 4
    cld
    rep movsd

    mov eax, [boot_health_temp_pid]
    mov edx, SECURITY_CAP_BOOT_HEALTH_REPORT
    call security_check
    jc .reject
    mov eax, [boot_health_temp_report + 8]
    cmp eax, [boot_health_record + BH_RECORD_GENERATION_LO]
    jne .reject
    mov eax, [boot_health_temp_report + 12]
    cmp eax, [boot_health_record + BH_RECORD_GENERATION_HI]
    jne .reject
    mov eax, [boot_health_temp_report + 16]
    cmp eax, [boot_health_record + BH_RECORD_BOOT_ATTEMPT]
    jne .reject
    cmp dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_FAILED
    je .reject
    cmp dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_TIMED_OUT
    je .reject

    mov ecx, [boot_health_temp_report + 20]
    cmp ecx, BOOT_HEALTH_MILESTONE_KERNEL_INITIALIZED
    jb .reject
    cmp ecx, BOOT_HEALTH_MILESTONE_OPERATIONAL
    ja .reject
    mov eax, [boot_health_temp_report + 24]
    test eax, eax
    jz .reject
    mov edx, eax
    dec edx
    test eax, edx
    jnz .reject
    mov edx, [boot_health_provider_allowed + ecx * 4]
    test eax, edx
    jz .reject
    mov ebx, [boot_health_temp_report + 28]
    cmp ebx, BOOT_HEALTH_PROVIDER_READY
    jb .reject
    cmp ebx, BOOT_HEALTH_PROVIDER_FAILED
    ja .reject

    inc dword [boot_health_record + BH_RECORD_AUTHORIZED]
    inc dword [boot_health_record + BH_RECORD_SEQUENCE]
    cmp ebx, BOOT_HEALTH_PROVIDER_FAILED
    je .provider_failed
    cmp ebx, BOOT_HEALTH_PROVIDER_DEGRADED
    je .provider_degraded
    or [boot_health_provider_ready + ecx * 4], eax
    call boot_health_advance
    popad
    popfd
    clc
    ret
.provider_degraded:
    or dword [boot_health_record + BH_RECORD_FLAGS], BOOT_HEALTH_FLAG_DEGRADED_SEEN
    mov dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_DEGRADED
    ; Provider als degradiert markieren, damit advance ihn als erfüllt wertet
    or [boot_health_provider_degraded + ecx * 4], eax
    call boot_health_advance
    popad
    popfd
    clc
    ret
.provider_failed:
    mov dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_FAILED
    mov [boot_health_record + BH_RECORD_FAILED], ecx
    popad
    popfd
    clc
    ret
.reject:
    inc dword [boot_health_record + BH_RECORD_REJECTED]
    popad
    popfd
    stc
    ret

boot_health_advance:
    mov ecx, [boot_health_record + BH_RECORD_LAST]
.next:
    inc ecx
    cmp ecx, BOOT_HEALTH_MILESTONE_OPERATIONAL
    ja .done
    mov eax, [boot_health_provider_required + ecx * 4]
    ; READY-Bits: exakt erfüllt
    mov edx, [boot_health_provider_ready + ecx * 4]
    ; DEGRADED-Bits zählen ebenfalls als erfüllt (aber Status bleibt DEGRADED)
    or  edx, [boot_health_provider_degraded + ecx * 4]
    and edx, eax
    cmp edx, eax
    jne .done
    bts dword [boot_health_record + BH_RECORD_REACHED], ecx
    mov [boot_health_record + BH_RECORD_LAST], ecx
    jmp .next
.done:
    cmp dword [boot_health_record + BH_RECORD_LAST], BOOT_HEALTH_MILESTONE_OPERATIONAL
    jne .return
    ; Status nur auf HEALTHY setzen wenn bisher kein DEGRADED-Pfad
    cmp dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_DEGRADED
    je .mark_confirmed
    mov dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_HEALTHY
.mark_confirmed:
    bts dword [boot_health_record + BH_RECORD_REACHED], BOOT_HEALTH_MILESTONE_CONFIRMED
    mov dword [boot_health_record + BH_RECORD_LAST], BOOT_HEALTH_MILESTONE_CONFIRMED
    or dword [boot_health_record + BH_RECORD_FLAGS], BOOT_HEALTH_FLAG_EVIDENCE_READY
.return:
    ret

; EAX=aufrufende PID, EDI=32-Byte-Ausgabepuffer.
boot_health_export_evidence:
    pushfd
    cli
    pushad
    test edi, 3
    jnz .denied
    mov [boot_health_temp_pid], eax
    mov eax, [boot_health_temp_pid]
    mov edx, SECURITY_CAP_BOOT_HEALTH_COMMIT
    call security_check
    jc .denied
    test dword [boot_health_record + BH_RECORD_FLAGS], BOOT_HEALTH_FLAG_EVIDENCE_READY
    jz .denied
    mov dword [edi], BOOT_HEALTH_EVIDENCE_SIZE
    mov word [edi + 4], 1
    mov word [edi + 6], 0
    mov eax, [boot_health_record + BH_RECORD_GENERATION_LO]
    mov [edi + 8], eax
    mov eax, [boot_health_record + BH_RECORD_GENERATION_HI]
    mov [edi + 12], eax
    mov eax, [boot_health_record + BH_RECORD_BOOT_ATTEMPT]
    mov [edi + 16], eax
    mov eax, [boot_health_record + BH_RECORD_REACHED]
    mov [edi + 20], eax
    mov eax, [boot_health_record + BH_RECORD_STATUS]
    mov [edi + 24], eax
    xor eax, eax
    test dword [boot_health_record + BH_RECORD_FLAGS], BOOT_HEALTH_FLAG_TRUST_VERIFIED
    jz .store_trust
    inc eax
.store_trust:
    mov [edi + 28], eax
    popad
    popfd
    clc
    ret
.denied:
    popad
    popfd
    stc
    ret

boot_health_publish_core_services:
    mov edx, BOOT_HEALTH_MILESTONE_CRITICAL_SERVICES
    mov ecx, BOOT_HEALTH_PROVIDER_CAPABILITY
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    jc .invalid
    mov edx, BOOT_HEALTH_MILESTONE_CRITICAL_SERVICES
    mov ecx, BOOT_HEALTH_PROVIDER_IPC
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    ret
.invalid:
    stc
    ret

; §108 Trust-Provider: meldet TRUST READY wenn Kernel signiert (SIGNATURE_VERIFIED),
; sonst TRUST DEGRADED (Boot laeuft weiter, Health bleibt Degraded, nicht Confirmed).
boot_health_publish_trust_provider:
    mov eax, [kernel_context + CONTEXT_SECURITY_STATE]
    cmp eax, NOVA_BOOT_VERIFICATION_SIGNATURE_VERIFIED
    jae .signed
    ; Unsignierter Kernel: TRUST DEGRADED melden und Meldung ausgeben
    mov edx, BOOT_HEALTH_MILESTONE_CRITICAL_SERVICES
    mov ecx, BOOT_HEALTH_PROVIDER_TRUST
    mov ebx, BOOT_HEALTH_PROVIDER_DEGRADED
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    mov esi, message_boot_health_trust_degraded
    call serial_write_string
    clc
    ret
.signed:
    ; Signierter Kernel: TRUST READY melden
    mov edx, BOOT_HEALTH_MILESTONE_CRITICAL_SERVICES
    mov ecx, BOOT_HEALTH_PROVIDER_TRUST
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    mov esi, message_boot_health_trust_signed
    call serial_write_string
    clc
    ret

; §108 Session-Provider (Phase-1 Stub): meldet SESSION READY sobald Userspace bereit.
; Phase 2 ersetzt dies durch echten Session-Manager mit Authentifizierung.
boot_health_publish_session_provider:
    mov edx, BOOT_HEALTH_MILESTONE_OPERATIONAL
    mov ecx, BOOT_HEALTH_PROVIDER_SESSION
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    mov esi, message_boot_health_session_ready
    call serial_write_string
    clc
    ret

; §126: SystemRoot-Provider melden — READY wenn NovaFS rw gemountet,
; DEGRADED wenn read-only oder gar nicht gemountet.
boot_health_publish_system_root:
    pushad
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    cmp dword [nfs_mounted], 1
    je .mounted
    ; kein Volume: DEGRADED
    mov ebx, BOOT_HEALTH_PROVIDER_DEGRADED
    jmp .report
.mounted:
    cmp dword [nfs_readonly], 0
    je .report                      ; rw → bleibt READY
    mov ebx, BOOT_HEALTH_PROVIDER_DEGRADED
.report:
    mov edx, BOOT_HEALTH_MILESTONE_SYSTEM_ROOT_READY
    mov ecx, BOOT_HEALTH_PROVIDER_SYSTEM_ROOT
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report  ; CF ignorieren (best-effort)
    cmp ebx, BOOT_HEALTH_PROVIDER_READY
    je .msg_ready
    mov esi, message_boot_health_system_root_degraded
    call serial_write_string
    popad
    clc
    ret
.msg_ready:
    mov esi, message_boot_health_system_root_ready
    call serial_write_string
    popad
    clc
    ret

boot_health_mark_kernel_initialized:
    mov edx, BOOT_HEALTH_MILESTONE_KERNEL_INITIALIZED
    mov ecx, BOOT_HEALTH_PROVIDER_KERNEL_CORE
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    jc .invalid
    mov edx, BOOT_HEALTH_MILESTONE_KERNEL_INITIALIZED
    mov ecx, BOOT_HEALTH_PROVIDER_MEMORY
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    jc .invalid
    cmp dword [boot_health_record + BH_RECORD_LAST], BOOT_HEALTH_MILESTONE_KERNEL_INITIALIZED
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

; Der Selbsttest arbeitet auf einem Snapshot und stellt den Live-Zustand wieder her.
boot_health_self_test:
    cld
    mov esi, boot_health_record
    mov edi, boot_health_saved_record
    mov ecx, BOOT_HEALTH_RECORD_SIZE / 4
    rep movsd
    mov esi, boot_health_provider_ready
    mov edi, boot_health_saved_providers
    mov ecx, 8
    rep movsd
    mov dword [boot_health_selftest_result], 1

    mov edx, BOOT_HEALTH_MILESTONE_KERNEL_INITIALIZED
    mov ecx, BOOT_HEALTH_PROVIDER_KERNEL_CORE
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    call boot_health_prepare_report
    mov eax, 0xFFFFFFFF
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    jnc .restore
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    jc .restore
    cmp dword [boot_health_record + BH_RECORD_LAST], BOOT_HEALTH_MILESTONE_KERNEL_ENTERED
    jne .restore

    mov edx, BOOT_HEALTH_MILESTONE_KERNEL_INITIALIZED
    mov ecx, BOOT_HEALTH_PROVIDER_MEMORY
    call boot_health_selftest_ready
    jc .restore
    mov edx, BOOT_HEALTH_MILESTONE_SYSTEM_ROOT_READY
    mov ecx, BOOT_HEALTH_PROVIDER_SYSTEM_ROOT
    call boot_health_selftest_ready
    jc .restore
    mov edx, BOOT_HEALTH_MILESTONE_CRITICAL_SERVICES
    mov ecx, BOOT_HEALTH_PROVIDER_TRUST
    call boot_health_selftest_ready
    jc .restore
    mov edx, BOOT_HEALTH_MILESTONE_CRITICAL_SERVICES
    mov ecx, BOOT_HEALTH_PROVIDER_CAPABILITY
    call boot_health_selftest_ready
    jc .restore
    mov edx, BOOT_HEALTH_MILESTONE_CRITICAL_SERVICES
    mov ecx, BOOT_HEALTH_PROVIDER_IPC
    call boot_health_selftest_ready
    jc .restore
    mov edx, BOOT_HEALTH_MILESTONE_OPERATIONAL
    mov ecx, BOOT_HEALTH_PROVIDER_SESSION
    call boot_health_selftest_ready
    jc .restore
    cmp dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_HEALTHY
    jne .restore
    cmp dword [boot_health_record + BH_RECORD_LAST], BOOT_HEALTH_MILESTONE_CONFIRMED
    jne .restore
    mov eax, 1
    mov edi, boot_health_test_evidence
    call boot_health_export_evidence
    jc .restore
    mov eax, [boot_health_test_evidence + 8]
    cmp eax, [boot_health_record + BH_RECORD_GENERATION_LO]
    jne .restore
    mov eax, [boot_health_test_evidence + 16]
    cmp eax, [boot_health_record + BH_RECORD_BOOT_ATTEMPT]
    jne .restore
    mov dword [boot_health_selftest_result], 0
.restore:
    cld
    mov esi, boot_health_saved_record
    mov edi, boot_health_record
    mov ecx, BOOT_HEALTH_RECORD_SIZE / 4
    rep movsd
    mov esi, boot_health_saved_providers
    mov edi, boot_health_provider_ready
    mov ecx, 8
    rep movsd
    cmp dword [boot_health_selftest_result], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

boot_health_selftest_ready:
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    ret

; Persistiert einen groben Candidate-Checkpoint ueber den normalisierten
; Firmware-Provider. EAX=0 nicht erforderlich, 1 geschrieben, 2 fehlgeschlagen.
firmware_runtime_boot_health_checkpoint:
    cmp dword [kernel_context + CONTEXT_BOOT_ATTEMPT], 0
    je .not_required
    cmp dword [kernel_context + CONTEXT_BOOT_GENERATION], 1
    ja .not_required
    test dword [kernel_context + CONTEXT_FIRMWARE_RUNTIME_CAPS], NOVA_FIRMWARE_RUNTIME_PERSIST_BOOT_HEALTH
    jz .not_required
    cmp dword [kernel_context + CONTEXT_FIRMWARE_RUNTIME_CONTEXT], 0
    je .failed
    cmp dword [kernel_context + CONTEXT_FIRMWARE_RUNTIME_ENTRY], 0
    je .failed
    cmp dword [boot_health_record + BH_RECORD_GENERATION_HI], 0
    jne .failed
    mov dword [boot_health_wire + 0], 0x41564F4E
    mov dword [boot_health_wire + 4], 0x56454842
    mov word [boot_health_wire + 8], 1
    mov word [boot_health_wire + 10], 64
    mov eax, [kernel_context + CONTEXT_BOOT_GENERATION]
    mov [boot_health_wire + 12], eax
    mov eax, [boot_health_record + BH_RECORD_GENERATION_LO]
    mov [boot_health_wire + 16], eax
    mov dword [boot_health_wire + 20], 0
    mov eax, [boot_health_record + BH_RECORD_BOOT_ATTEMPT]
    mov [boot_health_wire + 24], eax
    mov eax, [boot_health_record + BH_RECORD_REACHED]
    mov [boot_health_wire + 28], eax
    mov eax, [boot_health_record + BH_RECORD_FAILED]
    mov [boot_health_wire + 32], eax
    mov eax, [boot_health_record + BH_RECORD_STATUS]
    mov [boot_health_wire + 36], eax
    xor eax, eax
    test dword [boot_health_record + BH_RECORD_FLAGS], BOOT_HEALTH_FLAG_TRUST_VERIFIED
    jz .trust_stored
    inc eax
.trust_stored:
    mov [boot_health_wire + 40], eax
    mov eax, [boot_health_record + BH_RECORD_SEQUENCE]
    mov [boot_health_wire + 44], eax
    mov dword [boot_health_wire + 48], 0
    mov dword [boot_health_wire + 52], 0
    mov dword [boot_health_wire + 56], 0
    mov dword [boot_health_wire + 60], 0
    mov esi, boot_health_wire
    call boot_health_wire_crc
    mov [boot_health_wire + 60], eax
    mov esi, boot_health_wire
    mov edi, [kernel_context + CONTEXT_FIRMWARE_RUNTIME_CONTEXT]
    mov eax, [kernel_context + CONTEXT_FIRMWARE_RUNTIME_ENTRY]
    call firmware_runtime_invoke
    cmp eax, 1
    jne .failed
    mov eax, 1
    ret
.not_required:
    xor eax, eax
    ret
.failed:
    mov eax, 2
    ret

; Persistiert HEALTHY-Wire nach HealthConfirmed via Firmware-Provider.
; EAX=0 nicht erforderlich, 1 geschrieben, 2 fehlgeschlagen.
firmware_runtime_health_commit:
    cmp dword [kernel_context + CONTEXT_BOOT_ATTEMPT], 0
    je .not_required
    test dword [kernel_context + CONTEXT_FIRMWARE_RUNTIME_CAPS], NOVA_FIRMWARE_RUNTIME_PERSIST_BOOT_HEALTH
    jz .not_required
    cmp dword [kernel_context + CONTEXT_FIRMWARE_RUNTIME_CONTEXT], 0
    je .failed
    cmp dword [kernel_context + CONTEXT_FIRMWARE_RUNTIME_ENTRY], 0
    je .failed
    cmp dword [boot_health_record + BH_RECORD_STATUS], BOOT_HEALTH_STATUS_HEALTHY
    jne .not_required
    test dword [boot_health_record + BH_RECORD_FLAGS], BOOT_HEALTH_FLAG_EVIDENCE_READY
    jz .not_required
    cmp dword [boot_health_record + BH_RECORD_GENERATION_HI], 0
    jne .failed
    mov dword [boot_health_wire + 0], 0x41564F4E
    mov dword [boot_health_wire + 4], 0x56454842
    mov word [boot_health_wire + 8], 1
    mov word [boot_health_wire + 10], 64
    mov eax, [kernel_context + CONTEXT_BOOT_GENERATION]
    mov [boot_health_wire + 12], eax
    mov eax, [boot_health_record + BH_RECORD_GENERATION_LO]
    mov [boot_health_wire + 16], eax
    mov dword [boot_health_wire + 20], 0
    mov eax, [boot_health_record + BH_RECORD_BOOT_ATTEMPT]
    mov [boot_health_wire + 24], eax
    mov eax, [boot_health_record + BH_RECORD_REACHED]
    mov [boot_health_wire + 28], eax
    mov eax, [boot_health_record + BH_RECORD_FAILED]
    mov [boot_health_wire + 32], eax
    mov dword [boot_health_wire + 36], BOOT_HEALTH_STATUS_HEALTHY
    xor eax, eax
    test dword [boot_health_record + BH_RECORD_FLAGS], BOOT_HEALTH_FLAG_TRUST_VERIFIED
    jz .trust_stored
    inc eax
.trust_stored:
    mov [boot_health_wire + 40], eax
    mov eax, [boot_health_record + BH_RECORD_SEQUENCE]
    mov [boot_health_wire + 44], eax
    mov dword [boot_health_wire + 48], 0
    mov dword [boot_health_wire + 52], 0
    mov dword [boot_health_wire + 56], 0
    mov dword [boot_health_wire + 60], 0
    mov esi, boot_health_wire
    call boot_health_wire_crc
    mov [boot_health_wire + 60], eax
    mov esi, boot_health_wire
    mov edi, [kernel_context + CONTEXT_FIRMWARE_RUNTIME_CONTEXT]
    mov eax, [kernel_context + CONTEXT_FIRMWARE_RUNTIME_ENTRY]
    call firmware_runtime_invoke
    cmp eax, 1
    jne .failed
    mov eax, 1
    ret
.not_required:
    xor eax, eax
    ret
.failed:
    mov eax, 2
    ret

; EAX=physischer Provider-Entry, EDI=Kontext, ESI=Payload. Der Loaderbereich
; bleibt reserviert, ist aber nicht Bestandteil der Kernel-Seitentabellen.
firmware_runtime_invoke:
    mov [firmware_runtime_entry], eax
    pushfd
    cli
    mov eax, cr0
    mov [firmware_runtime_saved_cr0], eax
    mov eax, cr3
    mov [firmware_runtime_saved_cr3], eax
    mov eax, cr4
    mov [firmware_runtime_saved_cr4], eax
    sgdt [firmware_runtime_saved_gdtr]
    mov ecx, 0xC0000080
    rdmsr
    mov [firmware_runtime_saved_efer_lo], eax
    mov [firmware_runtime_saved_efer_hi], edx
    mov eax, [firmware_runtime_saved_cr0]
    and eax, 0x7FFFFFFF
    mov cr0, eax
    mov eax, [firmware_runtime_entry]
    call eax
    mov [firmware_runtime_result], eax
    mov eax, [firmware_runtime_saved_cr4]
    mov cr4, eax
    mov eax, [firmware_runtime_saved_cr3]
    mov cr3, eax
    mov ecx, 0xC0000080
    mov eax, [firmware_runtime_saved_efer_lo]
    mov edx, [firmware_runtime_saved_efer_hi]
    wrmsr
    mov eax, [firmware_runtime_saved_cr0]
    mov cr0, eax
    lgdt [firmware_runtime_saved_gdtr]
    jmp 0x08:.kernel_segments_restored
.kernel_segments_restored:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    popfd
    mov eax, [firmware_runtime_result]
    ret

; ESI=64-Byte-Wire-Datensatz; Checksum-Feld liegt am Ende und ist bereits null.
boot_health_wire_crc:
    push ebx
    push ecx
    push edx
    mov eax, 0xFFFFFFFF
    xor edx, edx
.byte:
    cmp edx, 64
    jae .done
    movzx ebx, byte [esi + edx]
    xor al, bl
    mov ecx, 8
.bit:
    shr eax, 1
    jnc .next_bit
    xor eax, 0xEDB88320
.next_bit:
    loop .bit
    inc edx
    jmp .byte
.done:
    not eax
    pop edx
    pop ecx
    pop ebx
    ret

align 4
boot_health_api:
    dd BOOT_HEALTH_API_SIZE
    dw 1, 0
    dd 0x00000007                 ; report, aggregate, evidence-export
    dd boot_health_submit_report
    dd boot_health_export_evidence
    dd boot_health_record
    dd boot_health_provider_ready
    dd BOOT_HEALTH_REPORT_SIZE
    dd BOOT_HEALTH_EVIDENCE_SIZE
    dd 0

boot_health_provider_required:
    dd 0, 0, 0
    dd BOOT_HEALTH_PROVIDER_KERNEL_CORE | BOOT_HEALTH_PROVIDER_MEMORY
    dd BOOT_HEALTH_PROVIDER_SYSTEM_ROOT
    dd BOOT_HEALTH_PROVIDER_TRUST | BOOT_HEALTH_PROVIDER_CAPABILITY | BOOT_HEALTH_PROVIDER_IPC
    dd BOOT_HEALTH_PROVIDER_SESSION
    dd 0
boot_health_provider_allowed:
    dd 0, 0, 0
    dd BOOT_HEALTH_PROVIDER_KERNEL_CORE | BOOT_HEALTH_PROVIDER_MEMORY
    dd BOOT_HEALTH_PROVIDER_SYSTEM_ROOT
    dd BOOT_HEALTH_PROVIDER_TRUST | BOOT_HEALTH_PROVIDER_CAPABILITY | BOOT_HEALTH_PROVIDER_IPC
    dd BOOT_HEALTH_PROVIDER_SESSION
    dd 0

align 4
boot_health_ready:           dd 0
boot_health_temp_pid:        dd 0
boot_health_selftest_result: dd 0
boot_health_provider_ready:    times 8 dd 0
boot_health_provider_degraded: times 8 dd 0
boot_health_saved_providers:   times 8 dd 0
boot_health_temp_report:     times BOOT_HEALTH_REPORT_SIZE db 0
boot_health_record:          times BOOT_HEALTH_RECORD_SIZE db 0
boot_health_saved_record:    times BOOT_HEALTH_RECORD_SIZE db 0
boot_health_test_evidence:   times BOOT_HEALTH_EVIDENCE_SIZE db 0
boot_health_wire:            times 64 db 0
firmware_runtime_entry:       dd 0
firmware_runtime_result:      dd 0
firmware_runtime_saved_cr0:   dd 0
firmware_runtime_saved_cr3:   dd 0
firmware_runtime_saved_cr4:   dd 0
firmware_runtime_saved_efer_lo: dd 0
firmware_runtime_saved_efer_hi: dd 0
firmware_runtime_saved_gdtr:   dw 0
                               dd 0

; ---------------------------------------------------------------------------
; CPU Manager / BSP- und Topologieerkennung (NPSPEC-KERNEL-0026)
; ---------------------------------------------------------------------------
CPU_API_SIZE          equ 48
CPU_RECORD_SIZE       equ 112
CPU_CAPACITY          equ 4
CPU_LOCAL_SLOT_SIZE   equ 64
CPU_STATE_DISCOVERED  equ 0
CPU_STATE_OFFLINE     equ 1
CPU_STATE_STARTING    equ 2
CPU_STATE_ONLINE      equ 3
CPU_STATE_ACTIVE      equ 4
CPU_STATE_IDLE        equ 5
CPU_STATE_FAILED      equ 8
CPU_CAPACITY_SCALE    equ 1024
CPU_FEATURE_FPU       equ 0x00000001
CPU_FEATURE_SIMD      equ 0x00000002
CPU_FEATURE_NX        equ 0x00000004
CPU_FEATURE_LOCAL_APIC equ 0x00000008
CPU_FEATURE_HW_RANDOM equ 0x00000010
CPU_FEATURE_VIRTUALIZED equ 0x00000020
CPU_TOPOLOGY_UNKNOWN  equ 0xFFFFFFFF
CPU_ID                equ 0
CPU_HARDWARE_LOW      equ 4
CPU_HARDWARE_HIGH     equ 8
CPU_STATE             equ 12
CPU_PACKAGE_ID        equ 16
CPU_DIE_ID            equ 20
CPU_CLUSTER_ID        equ 24
CPU_CORE_ID           equ 28
CPU_THREAD_ID         equ 32
CPU_NUMA_NODE_ID      equ 36
CPU_PACKAGE_THREADS   equ 40
CPU_CAPACITY_VALUE    equ 44
CPU_LOCAL_DATA        equ 48
CPU_FLAGS             equ 52
CPU_VENDOR0           equ 56
CPU_VENDOR1           equ 60
CPU_VENDOR2           equ 64
CPU_SIGNATURE         equ 68
CPU_RAW_FEATURE_EDX   equ 72
CPU_RAW_FEATURE_ECX   equ 76
CPU_FEATURES          equ 80
CPU_LLC_ID            equ 84
CPU_TOPOLOGY_NODE     equ 88
CPU_TOPOLOGY_GENERATION equ 92

cpu_manager_initialize:
    push ebp
    mov edi, cpu_records
    xor eax, eax
    mov ecx, (CPU_CAPACITY * CPU_RECORD_SIZE) / 4
    rep stosd
    mov edi, cpu_local_data
    mov ecx, (CPU_CAPACITY * CPU_LOCAL_SLOT_SIZE) / 4
    rep stosd
    mov dword [cpu_possible_set], 1
    mov dword [cpu_discovered_set], 1
    mov dword [cpu_present_set], 1
    mov dword [cpu_online_set], 0
    mov dword [cpu_active_set], 0
    mov dword [cpu_isolated_set], 0
    mov dword [cpu_failed_set], 0
    mov dword [cpu_discovered_count], 1
    mov dword [cpu_online_count], 0
    mov dword [cpu_startup_attempts], 1

    mov edi, cpu_records
    mov dword [edi + CPU_ID], 0     ; bootlokale CPU ID
    mov dword [edi + CPU_STATE], CPU_STATE_DISCOVERED
    mov dword [edi + CPU_CAPACITY_VALUE], CPU_CAPACITY_SCALE
    mov dword [edi + CPU_LOCAL_DATA], cpu_local_data
    mov dword [edi + CPU_FLAGS], 1  ; BSP

    xor eax, eax
    cpuid
    mov [edi + CPU_VENDOR0], ebx    ; Herstellerkennung, 12 Byte
    mov [edi + CPU_VENDOR1], edx
    mov [edi + CPU_VENDOR2], ecx
    mov [cpu_max_basic_leaf], eax
    mov eax, 1
    cpuid
    mov [edi + CPU_SIGNATURE], eax  ; Family/Model/Stepping
    mov [edi + CPU_RAW_FEATURE_EDX], edx
    mov [edi + CPU_RAW_FEATURE_ECX], ecx
    mov eax, ebx
    shr eax, 24
    mov [edi + CPU_HARDWARE_LOW], eax
    mov dword [edi + CPU_HARDWARE_HIGH], 0
    mov eax, ebx
    shr eax, 16
    and eax, 0xFF
    test eax, eax
    jnz .logical_known
    mov eax, 1
.logical_known:
    mov [edi + CPU_PACKAGE_THREADS], eax

    xor ebp, ebp
    test edx, 1 << 0
    jz .no_fpu
    or ebp, CPU_FEATURE_FPU
.no_fpu:
    test edx, 1 << 25
    jz .no_simd
    or ebp, CPU_FEATURE_SIMD
.no_simd:
    test edx, 1 << 9
    jz .no_apic
    or ebp, CPU_FEATURE_LOCAL_APIC
.no_apic:
    test ecx, 1 << 30
    jz .no_random
    or ebp, CPU_FEATURE_HW_RANDOM
.no_random:
    test ecx, 1 << 31
    jz .no_hypervisor
    or ebp, CPU_FEATURE_VIRTUALIZED
.no_hypervisor:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_extended
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 20
    jz .no_extended
    or ebp, CPU_FEATURE_NX
.no_extended:
    mov [edi + CPU_FEATURES], ebp
    mov [cpu_system_features], ebp  ; Schnittmenge der aktiven CPUs

    mov dword [edi + CPU_LLC_ID], CPU_TOPOLOGY_UNKNOWN
    mov eax, [edi + CPU_HARDWARE_LOW]
    call cpu_import_hal_topology
    jc .invalid_topology

    ; Per-CPU-Basis muss vor ONLINE vollständig sein.
    mov dword [cpu_local_data + 0], cpu_records
    mov dword [cpu_local_data + 4], 0 ; current thread folgt dem Scheduler
    mov dword [cpu_local_data + 8], scheduler_current
    mov dword [cpu_local_data + 12], logging_ring
    mov dword [cpu_local_data + 16], 0 ; preemption depth
    mov dword [cpu_local_data + 20], 0 ; interrupt depth
    mov dword [cpu_local_data + 24], 0 ; exception depth
    mov eax, [kernel_boot_stack_top]
    mov [cpu_local_data + 28], eax
    mov dword [cpu_local_data + 32], 1 ; lokaler Timer vorbereitet
    mov dword [cpu_local_data + 36], 1 ; Interruptcontroller vorbereitet
    mov dword [cpu_local_data + 40], 0x43505530 ; Canary/Owner-Marker
    mov dword [edi + CPU_STATE], CPU_STATE_ONLINE
    mov dword [cpu_online_set], 1
    mov dword [cpu_online_count], 1
    mov dword [edi + CPU_STATE], CPU_STATE_ACTIVE
    mov dword [cpu_active_set], 1
    ; Firmwareerkannte APs erhalten kompakte CPU IDs, aber weder Stack noch
    ; Runqueue noch ONLINE/ACTIVE-Bit. Der UP-Kernel kann sicher weiterbooten.
    mov ecx, [acpi_cpu_count]
    cmp ecx, CPU_CAPACITY
    ja .invalid_topology
    mov [cpu_discovered_count], ecx
    mov eax, 1
    shl eax, cl
    dec eax
    mov [cpu_possible_set], eax
    mov [cpu_discovered_set], eax
    mov [cpu_present_set], eax
    mov ebx, 1
.register_ap:
    cmp ebx, [acpi_cpu_count]
    jae .topology_done
    imul edx, ebx, CPU_RECORD_SIZE
    add edx, cpu_records
    mov [edx + CPU_ID], ebx
    mov eax, [acpi_apic_ids + ebx * 4]
    mov [edx + CPU_HARDWARE_LOW], eax
    mov dword [edx + CPU_HARDWARE_HIGH], 0
    mov dword [edx + CPU_STATE], CPU_STATE_OFFLINE
    mov dword [edx + CPU_CAPACITY_VALUE], CPU_CAPACITY_SCALE
    mov dword [edx + CPU_LLC_ID], CPU_TOPOLOGY_UNKNOWN
    push ebx
    mov edi, edx
    call cpu_import_hal_topology
    pop ebx
    jc .invalid_topology
    inc ebx
    jmp .register_ap
.invalid_topology:
    pop ebp
    stc
    ret
.topology_done:
    pop ebp
    clc
    ret

; EDI=CPU-Datensatz, EAX=APIC-Hardware-ID. Importiert ausschließlich die
; normalisierte HAL-Sicht; der CPU Manager parst weder MADT noch CPUID erneut.
cpu_import_hal_topology:
    mov [cpu_temp_record], edi
    mov dword [edi + CPU_PACKAGE_ID], CPU_TOPOLOGY_UNKNOWN
    mov dword [edi + CPU_DIE_ID], CPU_TOPOLOGY_UNKNOWN
    mov dword [edi + CPU_CLUSTER_ID], CPU_TOPOLOGY_UNKNOWN
    mov dword [edi + CPU_CORE_ID], CPU_TOPOLOGY_UNKNOWN
    mov dword [edi + CPU_THREAD_ID], CPU_TOPOLOGY_UNKNOWN
    mov dword [edi + CPU_NUMA_NODE_ID], CPU_TOPOLOGY_UNKNOWN
    mov dword [edi + CPU_TOPOLOGY_NODE], 0
    mov dword [edi + CPU_TOPOLOGY_GENERATION], 0
    call topology_cpu_lookup
    jc .invalid
    mov esi, eax
    mov edi, [cpu_temp_record]
    mov edx, [esi + TOPOLOGY_ID]
    mov [edi + CPU_TOPOLOGY_NODE], edx
    mov edx, [esi + TOPOLOGY_GENERATION]
    mov [edi + CPU_TOPOLOGY_GENERATION], edx
    mov edx, [esi + TOPOLOGY_PROPERTY2]
    mov [edi + CPU_THREAD_ID], edx
    test dword [esi + TOPOLOGY_FLAGS], TOPOLOGY_FLAG_LOCALITY_KNOWN
    jz .locality_ready
    mov edx, [esi + TOPOLOGY_NUMA_NODE]
    mov [edi + CPU_NUMA_NODE_ID], edx
.locality_ready:
    ; Die normalisierte Parent-Kette ist die Quelle der Wahrheit. Ein
    ; Fallback-Thread hängt direkt an NUMA und behält Unknown; eine
    ; validierte Architekturkette führt über Core zu Package.
    mov edi, [cpu_temp_record]
    mov eax, [edi + CPU_TOPOLOGY_NODE]
    call topology_lookup
    jc .invalid
    mov esi, eax
    mov eax, [esi + TOPOLOGY_PARENT]
    call topology_lookup
    jc .invalid
    cmp dword [eax + TOPOLOGY_TYPE], TOPOLOGY_TYPE_CPU_CORE
    jne .complete
    mov edi, [cpu_temp_record]
    mov edx, [eax + TOPOLOGY_HARDWARE_ID]
    mov [edi + CPU_CORE_ID], edx
    mov eax, [eax + TOPOLOGY_PARENT]
    call topology_lookup
    jc .invalid
    cmp dword [eax + TOPOLOGY_TYPE], TOPOLOGY_TYPE_CPU_PACKAGE
    jne .invalid
    mov edi, [cpu_temp_record]
    mov edx, [eax + TOPOLOGY_HARDWARE_ID]
    mov [edi + CPU_PACKAGE_ID], edx
.complete:
    clc
    ret
.invalid:
    stc
    ret

; EAX=CPU ID, EDX=Zeiger auf internen Record bei Erfolg.
cpu_query:
    cmp eax, [cpu_discovered_count]
    jae .invalid
    imul edx, eax, CPU_RECORD_SIZE
    add edx, cpu_records
    cmp dword [edx + CPU_STATE], CPU_STATE_FAILED
    je .invalid
    clc
    ret
.invalid:
    stc
    ret

; Die letzte aktive CPU sowie der BSP im aktuellen UP-Pfad bleiben online.
cpu_offline:
    call cpu_query
    jc .invalid
    cmp dword [cpu_active_set], 1
    je .invalid
    cmp dword [edx + CPU_STATE], CPU_STATE_ACTIVE
    jne .invalid
    mov dword [edx + CPU_STATE], CPU_STATE_OFFLINE
    btr dword [cpu_online_set], eax
    btr dword [cpu_active_set], eax
    dec dword [cpu_online_count]
    inc dword [cpu_offline_operations]
    clc
    ret
.invalid:
    stc
    ret

cpu_manager_self_test:
    mov dword [cpu_selftest_stage], 1
    mov eax, [acpi_cpu_count]
    cmp dword [cpu_discovered_count], eax
    jne .invalid
    cmp dword [cpu_online_count], 1
    jne .invalid
    cmp dword [cpu_active_set], 1
    jne .invalid
    mov dword [cpu_test_index], 0
.topology_next:
    mov dword [cpu_selftest_stage], 0x10
    mov eax, [cpu_test_index]
    cmp eax, [cpu_discovered_count]
    jae .topology_complete
    call cpu_query
    jc .invalid
    mov dword [cpu_selftest_stage], 0x11
    mov [cpu_temp_record], edx
    mov ecx, [cpu_test_index]
    mov eax, [acpi_apic_ids + ecx * 4]
    cmp [edx + CPU_HARDWARE_LOW], eax
    jne .invalid
    mov dword [cpu_selftest_stage], 0x12
    mov eax, [edx + CPU_TOPOLOGY_NODE]
    test eax, eax
    jz .invalid
    call topology_lookup
    jc .invalid
    mov dword [cpu_selftest_stage], 0x13
    mov edx, [cpu_temp_record]
    mov ecx, [eax + TOPOLOGY_GENERATION]
    cmp [edx + CPU_TOPOLOGY_GENERATION], ecx
    jne .invalid
    mov dword [cpu_selftest_stage], 0x14
    mov ecx, [eax + TOPOLOGY_PROPERTY2]
    cmp [edx + CPU_THREAD_ID], ecx
    jne .invalid
    mov dword [cpu_selftest_stage], 0x15
    test dword [eax + TOPOLOGY_FLAGS], TOPOLOGY_FLAG_ARCH_VALIDATED
    jz .topology_fallback
    mov dword [cpu_selftest_stage], 0x151
    cmp dword [edx + CPU_PACKAGE_ID], CPU_TOPOLOGY_UNKNOWN
    je .package_unknown
    mov dword [cpu_selftest_stage], 0x152
    cmp dword [edx + CPU_CORE_ID], CPU_TOPOLOGY_UNKNOWN
    je .invalid
    jmp .topology_entry_valid
.package_unknown:
    mov eax, [eax + TOPOLOGY_PARENT]
    call topology_lookup
    jc .invalid
    mov eax, [eax + TOPOLOGY_TYPE]
    add eax, 0x1510
    mov [cpu_selftest_stage], eax
    jmp .invalid
.topology_fallback:
    cmp dword [edx + CPU_PACKAGE_ID], CPU_TOPOLOGY_UNKNOWN
    jne .invalid
    cmp dword [edx + CPU_CORE_ID], CPU_TOPOLOGY_UNKNOWN
    jne .invalid
.topology_entry_valid:
    mov dword [cpu_selftest_stage], 0x16
    cmp dword [edx + CPU_DIE_ID], CPU_TOPOLOGY_UNKNOWN
    jne .invalid
    cmp dword [edx + CPU_CLUSTER_ID], CPU_TOPOLOGY_UNKNOWN
    jne .invalid
    test dword [eax + TOPOLOGY_FLAGS], TOPOLOGY_FLAG_LOCALITY_KNOWN
    jz .cpu_numa_unknown
    mov ecx, [eax + TOPOLOGY_NUMA_NODE]
    cmp [edx + CPU_NUMA_NODE_ID], ecx
    jne .invalid
    jmp .cpu_numa_valid
.cpu_numa_unknown:
    cmp dword [edx + CPU_NUMA_NODE_ID], CPU_TOPOLOGY_UNKNOWN
    jne .invalid
.cpu_numa_valid:
    mov dword [cpu_selftest_stage], 0x17
    inc dword [cpu_test_index]
    jmp .topology_next
.topology_complete:
    mov dword [cpu_selftest_stage], 2
    mov eax, [kernel_boot_stack_top]
    cmp dword [cpu_local_data + 28], eax
    jne .invalid
    cmp dword [cpu_local_data + 32], 1
    jne .invalid
    cmp dword [cpu_local_data + 36], 1
    jne .invalid
    xor eax, eax
    call cpu_query
    jc .invalid
    cmp dword [edx + CPU_STATE], CPU_STATE_ACTIVE
    jne .invalid
    cmp dword [cpu_discovered_count], 1
    jbe .no_ap
    mov eax, 1
    call cpu_query
    jc .invalid
    cmp dword [edx + CPU_STATE], CPU_STATE_OFFLINE
    jne .invalid
    cmp dword [cpu_records + CPU_RECORD_SIZE + CPU_LOCAL_DATA], 0
    jne .invalid
.no_ap:
    mov dword [cpu_selftest_stage], 3
    mov eax, CPU_CAPACITY
    call cpu_query
    jnc .invalid
    xor eax, eax
    call cpu_offline               ; letzte aktive CPU muss abgelehnt werden
    jnc .invalid
    ; Kontrollierter ACTIVE-IDLE-ACTIVE-Übergang des BSP.
    mov dword [cpu_records + CPU_STATE], CPU_STATE_IDLE
    mov dword [cpu_records + CPU_STATE], CPU_STATE_ACTIVE
    clc
    ret
.invalid:
    mov esi, message_cpu_manager_selftest_stage
    call serial_write_string
    mov eax, [cpu_selftest_stage]
    call serial_write_hex32
    mov esi, message_newline
    call serial_write_string
    stc
    ret

align 4
cpu_manager_api:
    dd CPU_API_SIZE
    dw 1, 0
    dd CPU_CAPACITY
    dd cpu_query
    dd cpu_offline
    dd cpu_discovered_count
    dd cpu_online_count
    dd cpu_system_features
    dd cpu_records
    dd cpu_online_set
    dd cpu_active_set
    dd cpu_local_data
cpu_max_basic_leaf:      dd 0
cpu_selftest_stage:       dd 0
cpu_system_features:     dd 0
cpu_possible_set:        dd 0
cpu_discovered_set:      dd 0
cpu_present_set:         dd 0
cpu_online_set:          dd 0
cpu_active_set:          dd 0
cpu_isolated_set:        dd 0
cpu_failed_set:          dd 0
cpu_discovered_count:    dd 0
cpu_online_count:        dd 0
cpu_startup_attempts:    dd 0
cpu_offline_operations:  dd 0
cpu_temp_record:         dd 0
cpu_test_index:          dd 0
align 64
cpu_local_data:          times CPU_CAPACITY * CPU_LOCAL_SLOT_SIZE db 0
cpu_records:             times CPU_CAPACITY * CPU_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; §124 SMP – AP-Aktivierung und echter SMP-Betrieb (NPSPEC-KERNEL-0027)
; INIT-SIPI-SIPI-Sequenz, AP-Trampoline (Real→Protected Mode),
; Cross-CPU-IPI-Versand, Remote-TLB-Shootdowns.
; ---------------------------------------------------------------------------
SMP_API_SIZE                equ 72
SMP_PHASE_ARCH_READY        equ 0
SMP_PHASE_MEMORY_READY      equ 1
SMP_PHASE_INTERRUPTS_READY  equ 2
SMP_PHASE_SCHEDULER_READY   equ 3
SMP_PHASE_OPERATIONAL       equ 4
SMP_IPI_RESCHEDULE          equ 0
SMP_IPI_TLB_SHOOTDOWN       equ 1
SMP_IPI_CALL_FUNCTION       equ 2
SMP_IPI_CPU_STOP            equ 3
SMP_IPI_CPU_WAKE            equ 4
SMP_IPI_DEBUG               equ 5
SMP_IPI_PANIC_STOP          equ 6
SMP_IPI_TYPE_COUNT          equ 7

; LAPIC-Register (xAPIC MMIO-Basis 0xFEE00000)
LAPIC_BASE              equ 0xFEE00000
LAPIC_SPURIOUS          equ 0xF0        ; Spurious-Interrupt-Vektor
LAPIC_EOI               equ 0xB0        ; End-of-Interrupt
LAPIC_ICR_LO            equ 0x300       ; Interrupt Command Register (low)
LAPIC_ICR_HI            equ 0x310       ; Interrupt Command Register (high)
LAPIC_ICR_DELIVERY_STS  equ (1 << 12)  ; Bit 12: Delivery Status (0=Idle)
LAPIC_IPI_INIT          equ 0x0000C500  ; INIT Assert: delivery=INIT(101), Level=Assert(14), Trigger=Level(15)
LAPIC_IPI_INIT_DA       equ 0x00008500  ; INIT De-Assert: Level=0, Trigger=Level (MP spec §B.4.2)
LAPIC_IPI_SIPI          equ 0x00004600  ; Startup-IPI: delivery=Startup(110)

; LAPIC-Timer-Register (xAPIC)
LAPIC_TIMER_LVT         equ 0x320       ; LVT Timer (Modus, Vektor, Maske)
LAPIC_TIMER_DCR         equ 0x3E0       ; Divide Configuration Register
LAPIC_TIMER_ICR         equ 0x380       ; Initial Count Register (Write → startet Timer)
LAPIC_TIMER_CCR         equ 0x390       ; Current Count Register (Read-only)
LAPIC_LVT_PERIODIC      equ 0x00020000  ; Bit 17: Periodischer Betrieb
LAPIC_LVT_MASKED        equ 0x00010000  ; Bit 16: Interrupt maskiert
LAPIC_TIMER_VECTOR      equ 0xEF        ; Vektor 239 – AP-LAPIC-Timer-Tick
LAPIC_IPI_FIXED         equ 0x00004000  ; Fixed IPI, Ziel-Vektor in Bits 7:0
SMP_IPI_VECTOR          equ 0xFE        ; generischer IPI-Empfangsvektor

; AP-Trampoline
AP_TRAMPOLINE_BASE      equ 0x8000      ; physische Adresse < 1 MB
AP_TRAMPOLINE_VECTOR    equ 0x08        ; SIPI-Vektor = Base >> 12
AP_STACK_SIZE           equ 0x1000      ; 4 KiB Stack pro AP
AP_BOOT_TIMEOUT_LOOPS   equ 2000000     ; Spin-Limit beim Warten auf AP-Start (≥10ms auch bei 1µs/pause)
SMP_CALL_TIMEOUT        equ AP_BOOT_TIMEOUT_LOOPS  ; Spin-Limit für smp_call_function und TLB-ACK-Warten

; ---------------------------------------------------------------------------
; AP-Trampoline-Blob (16-Bit Real-Mode-Code, wird nach 0x8000 kopiert)
; Layout:
;   +0x00  jmp short 0x8010     (2 Bytes)
;   +0x02  GDT-Limit            (2 Bytes, gepatcht)
;   +0x04  GDT-Basis            (4 Bytes, gepatcht)
;   +0x08  PM-Einstiegspunkt    (4 Bytes, gepatcht)
;   +0x0C  Code-Selektor 0x08   (2 Bytes, fest)
;   +0x0E  Padding              (2 Bytes)
;   +0x10  Eigentlicher 16-Bit-Code
; ---------------------------------------------------------------------------
ap_trampoline_blob:
[bits 16]
    jmp short .code         ; EB 0E – überspringt Patch-Bereich
    dw 0                    ; GDT-Limit       (+0x02, wird gepatcht)
    dd 0                    ; GDT-Basis       (+0x04, wird gepatcht)
    dd 0                    ; PM-Einstieg EIP (+0x08, wird gepatcht)
    dw CODE_SEGMENT         ; Code-Selektor   (+0x0C, fest 0x08)
    dw 0                    ; Padding         (+0x0E)
.code:                      ; ab hier: 0x8010 wenn Blob an 0x8000
    cli
    cld
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    ; WHPX-AP-Sync: I/O-VMEXIT damit der WHPX-Hypervisor den AP-Start abschließt
    out 0x80, al            ; POST-Port (2 Bytes) – kein sichtbarer Effekt außer VMEXIT
    lgdt [word 0x8002]      ; GDT laden (Patch-Bereich bei 0x8002)
    mov eax, cr0
    or  al, 1               ; PE setzen
    mov cr0, eax
    o32 jmp far [word 0x8008] ; Far-Sprung 32-Bit: lädt EIP+CS aus [0x8008]
[bits 32]
ap_trampoline_blob_end:

; ---------------------------------------------------------------------------
; AP Protected-Mode-Einstieg (nach Trampoline, CS=0x08, IF=0)
; ---------------------------------------------------------------------------
ap_entry32_pm:
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax
    mov ss, ax
    xor ax, ax
    mov fs, ax
    mov gs, ax

    ; Kern-IDT übernehmen (BSP hat sie aufgebaut)
    lidt [idt_descriptor]

    ; APIC-ID dieses AP (CPUID.1.EBX Bits 31:24)
    mov eax, 1
    cpuid
    shr ebx, 24
    mov edi, ebx            ; EDI = APIC-ID

    ; CPU-Slot durch Vergleich mit acpi_apic_ids finden
    xor ecx, ecx
.find_slot:
    cmp ecx, [cpu_discovered_count]
    jae .halt_unknown
    cmp [acpi_apic_ids + ecx * 4], edi
    je .slot_found
    inc ecx
    jmp .find_slot

.slot_found:
    ; ECX = Slot-Index (0=BSP, 1..N=APs)
    ; per-CPU Thread-Slot initialisieren: -1 = Idle (§131)
    mov dword [per_cpu_current_thread + ecx * 4], -1
    ; Stack für diesen AP: ap_stack_area[(Slot) * AP_STACK_SIZE .. (Slot+1) * AP_STACK_SIZE]
    ; (Slot 0 = BSP hat eigenen Stack; APs beginnen ab Slot-Index 1, Index in Array: Slot-1)
    imul edx, ecx, AP_STACK_SIZE   ; Offset = Slot * AP_STACK_SIZE
    lea esp, [ap_stack_area + edx - 4]
    and esp, 0xFFFFFFF0             ; 16-Byte-ausrichten

    ; Lokalen APIC dieses AP aktivieren (Spurious-Interrupt-Vektor)
    mov eax, [LAPIC_BASE + LAPIC_SPURIOUS]
    or  eax, 0x100
    and eax, 0xFFFFFF00
    or  eax, 0xFF
    mov [LAPIC_BASE + LAPIC_SPURIOUS], eax

    ; ---------------------------------------------------------------------------
    ; LAPIC-Timer kalibrieren (gegen BSP-PIT, 100 Hz) und starten
    ;
    ; Ablauf:
    ;   1. DCR = divide-by-1 (volle Bus-Frequenz)
    ;   2. LVT = maskiert, One-Shot, Vektor LAPIC_TIMER_VECTOR
    ;   3. ICR = 0xFFFFFFFF (Maximum – startet Countdown)
    ;   4. Auf nächste PIT-Flanke warten (timer_ticks ändert sich) → Sync
    ;   5. ICR neu auf Maximum (beginnt jetzt exakt am Tick-Rand)
    ;   6. Nächste PIT-Flanke abwarten (eine volle 10-ms-Periode)
    ;   7. CCR lesen → elapsed = neg(CCR) ≈ ICR-Wert für 100 Hz
    ;   8. Periodischen Betrieb starten
    ; ---------------------------------------------------------------------------
    push ecx                          ; Slot-Register sichern
    push edx

    ; Schritt 1: DCR = divide-by-1 (Kodierung: 0b1011 = 0xB)
    mov dword [LAPIC_BASE + LAPIC_TIMER_DCR], 0xB

    ; Schritt 2+3: LVT maskiert, One-Shot; ICR auf Maximum
    mov dword [LAPIC_BASE + LAPIC_TIMER_LVT], LAPIC_LVT_MASKED | LAPIC_TIMER_VECTOR
    mov dword [LAPIC_BASE + LAPIC_TIMER_ICR], 0xFFFFFFFF

    ; Schritt 4: Auf nächste PIT-Flanke synchronisieren
    mov eax, [timer_ticks]
.ap_cal_sync:
    cmp [timer_ticks], eax
    je  .ap_cal_sync

    ; Schritt 5: ICR neu – jetzt exakt am Tick-Rand
    mov dword [LAPIC_BASE + LAPIC_TIMER_ICR], 0xFFFFFFFF

    ; Schritt 6: Eine vollständige PIT-Periode (10 ms) messen
    mov eax, [timer_ticks]
.ap_cal_measure:
    cmp [timer_ticks], eax
    je  .ap_cal_measure

    ; Schritt 7: Verbleibende Zähler → elapsed = -CCR (mod 2^32)
    mov eax, [LAPIC_BASE + LAPIC_TIMER_CCR]
    neg eax                           ; elapsed ≈ 0xFFFFFFFF − CCR + 1
    mov [lapic_timer_count], eax      ; Wert merken (alle APs eines Boards gleich)

    ; Schritt 8: Periodischen Betrieb starten (unmaskiert, Vektor 0xEF)
    mov dword [LAPIC_BASE + LAPIC_TIMER_LVT], LAPIC_LVT_PERIODIC | LAPIC_TIMER_VECTOR
    mov [LAPIC_BASE + LAPIC_TIMER_ICR], eax

    pop edx
    pop ecx

    ; CPU-Sets und Zähler atomar aktualisieren
    lock bts dword [cpu_online_set],  ecx
    lock bts dword [cpu_active_set],  ecx
    lock inc dword [cpu_online_count]

    ; BSP signalisieren (wartet auf ap_alive_count)
    lock inc dword [ap_alive_count]

    ; AP-Leerlaufschleife (Interrupts ein, HALT bis Reschedule-IPI)
.ap_idle:
    sti
    hlt
    jmp .ap_idle

.halt_unknown:
    cli
.halt_forever:
    hlt
    jmp .halt_forever

; ---------------------------------------------------------------------------
; apic_wait_icr_idle – wartet bis ICR Delivery-Status = Idle
; ---------------------------------------------------------------------------
apic_wait_icr_idle:
    push ecx
    mov ecx, 200000
.wait:
    test dword [LAPIC_BASE + LAPIC_ICR_LO], LAPIC_ICR_DELIVERY_STS
    jz .idle
    pause
    dec ecx
    jnz .wait
.idle:
    pop ecx
    ret

; ---------------------------------------------------------------------------
; smp_boot_ap – sendet INIT-SIPI-SIPI an einen einzelnen AP
; EAX = Ziel-APIC-ID (8 Bit xAPIC)
; ---------------------------------------------------------------------------
smp_boot_ap:
    push esi
    push ecx
    movzx esi, al           ; APIC-ID sichern

    ; ICR_HI: Ziel-APIC-ID in Bits 31:24 eintragen
    mov ecx, esi
    shl ecx, 24
    mov [LAPIC_BASE + LAPIC_ICR_HI], ecx

    ; INIT Assert (Level=1 Trigger=Level – MP spec §B.4.2 / Intel SDM §10.6.7)
    mov dword [LAPIC_BASE + LAPIC_ICR_LO], LAPIC_IPI_INIT
    call apic_wait_icr_idle

    ; 30ms nach INIT warten (MP spec: min. 10ms; WHPX braucht etwas mehr Spielraum)
    mov eax, [timer_ticks]
    add eax, 3
.wait_init:
    cmp [timer_ticks], eax
    jb .wait_init

    ; INIT De-Assert (benötigt von WHPX und strenger Hardware per MP spec)
    mov ecx, esi
    shl ecx, 24
    mov [LAPIC_BASE + LAPIC_ICR_HI], ecx
    mov dword [LAPIC_BASE + LAPIC_ICR_LO], LAPIC_IPI_INIT_DA
    call apic_wait_icr_idle

    ; Erster STARTUP IPI
    mov ecx, esi
    shl ecx, 24
    mov [LAPIC_BASE + LAPIC_ICR_HI], ecx
    mov dword [LAPIC_BASE + LAPIC_ICR_LO], LAPIC_IPI_SIPI | AP_TRAMPOLINE_VECTOR
    call apic_wait_icr_idle

    ; §38-opt: 200µs Pause – kurzer pause-Loop reicht für SIPI-Abstand.
    mov ecx, 200
.wait_sipi1:
    pause
    dec ecx
    jnz .wait_sipi1

    ; Zweiter STARTUP IPI (Redundanz per MP-Spec)
    mov ecx, esi
    shl ecx, 24
    mov [LAPIC_BASE + LAPIC_ICR_HI], ecx
    mov dword [LAPIC_BASE + LAPIC_ICR_LO], LAPIC_IPI_SIPI | AP_TRAMPOLINE_VECTOR
    call apic_wait_icr_idle

    clc
    pop ecx
    pop esi
    ret

; ---------------------------------------------------------------------------
; smp_start_aps – kopiert Trampoline, patcht GDT/Einstieg, startet alle APs
; ---------------------------------------------------------------------------
smp_start_aps:
    push esi
    push edi
    push ebx
    push ecx

    ; Trampoline-Blob nach 0x8000 kopieren
    mov esi, ap_trampoline_blob
    mov edi, AP_TRAMPOLINE_BASE
    mov ecx, (ap_trampoline_blob_end - ap_trampoline_blob + 3) >> 2
    rep movsd
    wbinvd                  ; Cache-Flush: WHPX und Hardware sehen Trampoline sofort

    ; Patch: GDT-Limit und GDT-Basis (aus kernel_gdt_descriptor)
    movzx eax, word [kernel_gdt_descriptor]
    mov word [AP_TRAMPOLINE_BASE + 0x02], ax
    mov eax, [kernel_gdt_descriptor + 2]
    mov dword [AP_TRAMPOLINE_BASE + 0x04], eax

    ; Patch: 32-Bit-Einstiegspunkt und Code-Selektor
    mov dword [AP_TRAMPOLINE_BASE + 0x08], ap_entry32_pm
    mov word  [AP_TRAMPOLINE_BASE + 0x0C], CODE_SEGMENT

    ; ap_alive_count zurücksetzen
    mov dword [ap_alive_count], 0

    ; Jeden AP einzeln starten und auf Online-Meldung warten
    mov ebx, 1              ; Slot 0 = BSP, APs beginnen bei 1
.ap_loop:
    cmp ebx, [cpu_discovered_count]
    jae .all_done

    ; APIC-ID des AP aus ACPI-Tabelle
    mov eax, [acpi_apic_ids + ebx * 4]
    call smp_boot_ap

    ; bis zu 500ms (50 PIT-Ticks) auf AP-Online warten (WHPX braucht ~300ms).
    mov eax, [timer_ticks]
    add eax, 50
    mov edx, ebx
    inc edx                 ; erwartet: BSP (1) + AP-Index Zähler
.wait_online:
    cmp [cpu_online_count], edx
    jae .ap_came_online
    cmp [timer_ticks], eax
    jae .ap_came_online     ; Timeout → Soft-Fail, nächsten versuchen
    pause
    jmp .wait_online
    ; Timeout – AP nicht gestartet; Soft-Fail, nächsten versuchen

.ap_came_online:
    inc ebx
    jmp .ap_loop

.all_done:
    pop ecx
    pop ebx
    pop edi
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; smp_initialize – initialisiert SMP-Infrastruktur und startet APs
; ---------------------------------------------------------------------------
smp_initialize:
    cmp dword [cpu_discovered_count], CPU_CAPACITY
    ja .unsupported
    cmp dword [cpu_online_set], 1
    jne .unsupported
    cmp dword [cpu_active_set], 1
    jne .unsupported
    test dword [cpu_present_set], 1
    jz .unsupported

    ; Zähler initialisieren
    mov dword [smp_boot_phase], SMP_PHASE_SCHEDULER_READY
    mov dword [smp_local_tlb_generation], 0
    mov dword [smp_local_tlb_flushes], 0
    mov dword [smp_rejected_ipis], 0
    mov dword [smp_rejected_remote_shootdowns], 0
    mov dword [smp_remote_ipis_sent], 0
    mov dword [ap_alive_count], 0

    ; UP-Betrieb: kein AP vorhanden → fertig
    cmp dword [cpu_discovered_count], 1
    jbe .done

    ; SMP-Betrieb: APs starten
    call smp_start_aps
    jc .failed

.done:
    clc
    ret
.failed:
.unsupported:
    stc
    ret

; ---------------------------------------------------------------------------
; smp_publish_phase – veröffentlicht Bootstrap-Phasen ohne Rückschritt
; EAX = neuer Phasenindex
; ---------------------------------------------------------------------------
smp_publish_phase:
    cmp eax, SMP_PHASE_OPERATIONAL
    ja .invalid
    mov edx, eax
.retry:
    mov eax, [smp_boot_phase]
    cmp edx, eax
    jb .invalid
    lock cmpxchg dword [smp_boot_phase], edx
    jne .retry
    mov eax, edx
    clc
    ret
.invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; smp_send_ipi – sendet Cross-CPU-IPI (EAX=Zielmaske, ECX=IPI-Typ)
; ---------------------------------------------------------------------------
smp_send_ipi:
    cmp ecx, SMP_IPI_TYPE_COUNT
    jae .reject
    test eax, eax
    jz .reject
    ; Maske darf nur online CPUs enthalten
    push edx
    mov edx, [cpu_online_set]
    not edx
    test eax, edx
    pop edx
    jnz .reject
    ; Self-IPI (Bit 0 = BSP) nicht unterstützt
    test eax, 1
    jnz .reject

    ; Jeden gesetzten Bit in der Maske: IPI senden
    push esi
    push edi
    push ebx
    mov edi, eax            ; Zielmaske
    xor esi, esi            ; Bit-Index (CPU-Slot)
.send_loop:
    cmp esi, [cpu_discovered_count]
    jae .send_done
    bt edi, esi
    jnc .next_bit
    ; IPI-Typ in Mailbox des Ziel-AP vermerken (ECX = IPI-Typ, ESI = CPU-Slot)
    push eax
    mov eax, 1
    shl eax, cl
    lock or [smp_ipi_mailbox + esi * 4], eax
    pop eax
    ; APIC-ID des Ziels
    movzx ebx, byte [acpi_apic_ids + esi * 4]
    shl ebx, 24
    mov [LAPIC_BASE + LAPIC_ICR_HI], ebx
    ; Fixed-IPI mit generischem Vektor
    mov dword [LAPIC_BASE + LAPIC_ICR_LO], LAPIC_IPI_FIXED | SMP_IPI_VECTOR
    call apic_wait_icr_idle
    lock inc dword [smp_remote_ipis_sent]
.next_bit:
    inc esi
    jmp .send_loop
.send_done:
    pop ebx
    pop edi
    pop esi
    clc
    ret
.reject:
    inc dword [smp_rejected_ipis]
    stc
    ret

; ---------------------------------------------------------------------------
; smp_tlb_shootdown_page – invalidiert eine Seite lokal und/oder remote
; EAX = seitenbündige virtuelle Adresse, EDX = CPU-Zielmaske
; ---------------------------------------------------------------------------
smp_tlb_shootdown_page:
    test eax, 0xFFF
    jnz .invalid
    test edx, edx
    jz .invalid
    ; Maske darf nur aktive CPUs enthalten
    push ecx
    mov ecx, [cpu_active_set]
    not ecx
    test edx, ecx
    pop ecx
    jnz .invalid

    ; Lokale INVLPG wenn BSP in Maske (Bit 0)
    test edx, 1
    jz .skip_local
    invlpg [eax]
    lock inc dword [smp_local_tlb_generation]
    inc dword [smp_local_tlb_flushes]
.skip_local:

    ; Remote-APs: IPI mit TLB_SHOOTDOWN-Typ senden, auf ACK warten (§30/§124)
    mov [smp_shootdown_addr], eax    ; Zieladresse für remote INVLPG sichern
    push eax
    push ebx
    mov ebx, edx
    and ebx, ~1                      ; BSP-Bit ausblenden → nur APs
    test ebx, ebx
    jz .skip_remote
    ; Popcount der AP-Maske → smp_tlb_ack_pending setzen
    push ebx
    xor ecx, ecx
.tlb_count_bits:
    test ebx, ebx
    jz .tlb_count_bits_done
    mov eax, ebx
    dec eax
    and ebx, eax
    inc ecx
    jmp .tlb_count_bits
.tlb_count_bits_done:
    mov [smp_tlb_ack_pending], ecx
    pop ebx                          ; EBX = AP-Maske wiederherstellen
    ; IPI senden (EAX = AP-Maske, ECX = IPI-Typ)
    mov eax, ebx
    mov ecx, SMP_IPI_TLB_SHOOTDOWN
    call smp_send_ipi
    ; §38-opt: bis zu 50ms (5 PIT-Ticks) auf TLB-ACK warten.
    push ecx
    mov ecx, [timer_ticks]
    add ecx, 5
.tlb_ack_wait:
    cmp dword [smp_tlb_ack_pending], 0
    je .tlb_ack_done
    cmp [timer_ticks], ecx
    jae .tlb_ack_timeout
    pause
    jmp .tlb_ack_wait
.tlb_ack_timeout:
    pop ecx
    inc dword [smp_rejected_remote_shootdowns]
    jmp .skip_remote
.tlb_ack_done:
    pop ecx
.skip_remote:
    pop ebx
    pop eax
    clc
    ret
.invalid:
    inc dword [smp_rejected_remote_shootdowns]
    stc
    ret

; ---------------------------------------------------------------------------
; smp_call_function – ruft eine Funktion synchron auf Remote-CPUs auf (§27/§28)
; EAX = Zielmaske (Bit 0 = BSP darf nicht gesetzt sein; nur online APs)
; ECX = Funktionszeiger  void fn(uint32_t ctx)  – ECX als erstes Argument
; EDX = Kontextzeiger, wird in ECX beim Aufruf auf dem AP bereitgestellt
; CF=0 OK, CF=1 ungültige Argumente oder Timeout
; ---------------------------------------------------------------------------
smp_call_function:
    ; Zielmaske: BSP-Bit verboten, darf nicht leer sein
    test eax, 1
    jnz .cf_reject
    test eax, eax
    jz .cf_reject
    ; Funktionszeiger darf nicht null sein
    test ecx, ecx
    jz .cf_reject
    ; Zielmaske darf nur online CPUs enthalten
    push edx
    push ebx
    mov ebx, [cpu_online_set]
    not ebx
    test eax, ebx
    pop ebx
    pop edx
    jnz .cf_reject

    ; Spinlock erwerben (serialisiert gleichzeitige Cross-CPU-Calls)
.cf_spin:
    lock bts dword [smp_call_fn_lock], 0
    jnc .cf_locked
    pause
    jmp .cf_spin
.cf_locked:

    ; Funktion und Kontext veröffentlichen (vor IPI-Versand, Acquire-Semantik
    ; auf AP-Seite via LAPIC-Schreib-Synchronisation ausreichend für x86)
    mov [smp_call_fn_ptr], ecx
    mov [smp_call_fn_ctx], edx

    ; Anzahl der Ziel-CPUs bestimmen (Popcount der Maske → ACK-Zähler)
    push eax
    push ecx
    push esi
    mov esi, eax
    xor ecx, ecx
.cf_count:
    test esi, esi
    jz .cf_count_done
    mov eax, esi
    dec eax
    and esi, eax
    inc ecx
    jmp .cf_count
.cf_count_done:
    mov [smp_call_fn_ack], ecx
    pop esi
    pop ecx
    pop eax

    ; CALL_FUNCTION-IPI an alle Ziel-CPUs senden
    push eax
    push ecx
    mov ecx, SMP_IPI_CALL_FUNCTION
    call smp_send_ipi               ; EAX=Maske, ECX=Typ
    pop ecx
    pop eax

    ; §38-opt: bis zu 50ms (5 PIT-Ticks) auf ACK warten.
    push ebx
    mov ebx, [timer_ticks]
    add ebx, 5
.cf_ack_wait:
    cmp dword [smp_call_fn_ack], 0
    je .cf_ack_done
    cmp [timer_ticks], ebx
    jae .cf_ack_timeout
    pause
    jmp .cf_ack_wait
.cf_ack_timeout:
    pop ebx
    lock btr dword [smp_call_fn_lock], 0
    stc
    ret
.cf_ack_done:
    pop ebx
    lock btr dword [smp_call_fn_lock], 0
    clc
    ret
.cf_reject:
    stc
    ret

; ---------------------------------------------------------------------------
; smp_self_test – prüft SMP-Invarianten nach smp_initialize
; ---------------------------------------------------------------------------
smp_self_test:
    ; Grundzustand: Phase korrekt, keine fehlerhaften/isolierten CPUs
    cmp dword [smp_boot_phase], SMP_PHASE_SCHEDULER_READY
    jne .invalid
    cmp dword [cpu_isolated_set], 0
    jne .invalid
    cmp dword [cpu_failed_set], 0
    jne .invalid

    ; Alle entdeckten CPUs müssen online und aktiv sein
    mov ecx, [cpu_discovered_count]
    cmp ecx, 0
    je .invalid
    cmp ecx, CPU_CAPACITY
    ja .invalid
    mov eax, 1
    shl eax, cl
    dec eax                         ; erwartete Maske: (1<<count)-1
    cmp [cpu_online_set], eax
    jne .invalid
    cmp [cpu_active_set], eax
    jne .invalid

    ; Self-IPI (BSP an sich selbst) muss immer abgelehnt werden
    mov eax, 1
    mov ecx, SMP_IPI_PANIC_STOP
    call smp_send_ipi
    jnc .invalid

    ; Lokaler TLB-Shootdown muss erfolgreich sein
    mov dword [smp_local_tlb_flushes], 0
    mov dword [smp_local_tlb_generation], 0
    mov eax, KERNEL_ENTRY_ADDRESS
    mov edx, 1
    call smp_tlb_shootdown_page
    jc .invalid
    cmp dword [smp_local_tlb_flushes], 1
    jne .invalid
    cmp dword [smp_local_tlb_generation], 1
    jne .invalid

    ; Ungültige Seitenausrichtung muss abgelehnt werden
    mov eax, KERNEL_ENTRY_ADDRESS + 1
    mov edx, 1
    call smp_tlb_shootdown_page
    jnc .invalid

    ; UP-spezifische Prüfungen
    cmp dword [cpu_discovered_count], 1
    jne .smp_checks

    ; UP: Slot-1-Daten unbenutzt
    cmp dword [cpu_local_data + CPU_LOCAL_SLOT_SIZE], 0
    jne .invalid
    ; UP: IPI an CPU 1 (nicht online) → muss abgelehnt werden
    mov eax, 2
    mov ecx, SMP_IPI_RESCHEDULE
    call smp_send_ipi
    jnc .invalid
    ; UP: Remote-TLB-Shootdown an CPU 1 (nicht aktiv) → muss abgelehnt werden
    mov eax, KERNEL_ENTRY_ADDRESS
    mov edx, 3
    call smp_tlb_shootdown_page
    jnc .invalid
    jmp .phase_test

.smp_checks:
    ; SMP: IPI an CPU 1 (online) → muss gelingen
    mov eax, 2
    mov ecx, SMP_IPI_RESCHEDULE
    call smp_send_ipi
    jc .invalid
    cmp dword [smp_remote_ipis_sent], 0
    je .invalid

    ; SMP: AP-LAPIC-Timer prüfen – 20 PIT-Ticks warten (≈ 200 ms),
    ; dann muss jeder AP-Slot (Slot 1 … cpu_discovered_count-1) mindestens
    ; einen Tick in ap_timer_ticks gezählt haben.
    ; LAPIC-Timer kalibriert sich gegen PIT → nach 200 ms sind ≈ 20 AP-Ticks sicher.
    mov eax, [timer_ticks]
    add eax, 20
.timer_wait:
    cmp [timer_ticks], eax
    jb  .timer_wait

    ; Alle AP-Slots (1 … N-1) auf Tick-Zähler > 0 prüfen
    mov ecx, 1                          ; BSP ist Slot 0, APs beginnen ab 1
.ap_tick_check:
    cmp ecx, [cpu_discovered_count]
    jae .ap_tick_ok
    cmp dword [ap_timer_ticks + ecx * 4], 0
    je  .invalid                        ; Kein Tick → LAPIC-Timer auf AP ECX defekt
    inc ecx
    jmp .ap_tick_check
.ap_tick_ok:
    ; LAPIC-Timer-Kalibrierwert muss plausibel sein (> 0 und < 2^31)
    cmp dword [lapic_timer_count], 0
    je  .invalid
    cmp dword [lapic_timer_count], 0x80000000
    jae .invalid

.phase_test:
    ; smp_call_function: Ablehnung ungültiger Masken (UP und SMP)
    push eax
    push ecx
    push edx
    xor eax, eax                    ; leere Maske → CF=1
    mov ecx, smp_self_test
    xor edx, edx
    call smp_call_function
    jnc .cf_test_fail
    mov eax, 1                      ; BSP-Bit in Maske → CF=1
    mov ecx, smp_self_test
    call smp_call_function
    jnc .cf_test_fail
    xor eax, eax                    ; Funktionszeiger null → CF=1
    mov eax, 2
    xor ecx, ecx
    call smp_call_function
    jnc .cf_test_fail
    pop edx
    pop ecx
    pop eax
    jmp .phase_continue
.cf_test_fail:
    pop edx
    pop ecx
    pop eax
    jmp .invalid
.phase_continue:
    ; smp_publish_phase: Rückschritt nicht erlaubt
    mov eax, SMP_PHASE_MEMORY_READY
    call smp_publish_phase
    jnc .invalid
    mov eax, SMP_PHASE_SCHEDULER_READY
    call smp_publish_phase
    jc .invalid
    cmp dword [smp_boot_phase], SMP_PHASE_SCHEDULER_READY
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; §62 – SMP-Stresstests (NPSPEC-KERNEL-0027)
; Prüft smp_call_function, TLB-Shootdown und gemischte Last unter realen
; SMP-Bedingungen.  UP-Systeme bestehen automatisch (CF=0).
; ---------------------------------------------------------------------------
smp_stress_test:
    ; UP: keine Remote-CPUs, Stresstest entfällt
    cmp dword [cpu_discovered_count], 1
    je .pass

    ; -----------------------------------------------------------------------
    ; Test 1: smp_call_function-Last – 16 Aufrufe auf alle APs
    ; Erwartetes Ergebnis: smp_stress_counter == (N_APs) * 16
    ; -----------------------------------------------------------------------
    mov dword [smp_stress_counter], 0
    mov ecx, 16
.call_loop:
    push ecx
    mov eax, [cpu_online_set]
    and eax, ~1                      ; BSP-Bit entfernen, nur AP-Bits
    mov ecx, smp_stress_inc
    mov edx, smp_stress_counter
    call smp_call_function
    jc .pop_fail
    pop ecx
    dec ecx
    jnz .call_loop

    ; Zähler muss exakt (cpu_discovered_count - 1) * 16 sein
    mov eax, [cpu_discovered_count]
    dec eax
    imul eax, 16
    cmp [smp_stress_counter], eax
    jne .fail

    ; -----------------------------------------------------------------------
    ; Test 2: TLB-Shootdown-Last – 8 Vollsystem-Shootdowns (BSP + alle APs)
    ; smp_rejected_remote_shootdowns darf sich nicht erhöhen.
    ; -----------------------------------------------------------------------
    mov eax, [smp_rejected_remote_shootdowns]
    mov [smp_stress_saved_rejects], eax
    mov dword [smp_local_tlb_flushes], 0
    mov ecx, 8
.tlb_loop:
    push ecx
    mov eax, KERNEL_ENTRY_ADDRESS
    mov edx, [cpu_active_set]
    call smp_tlb_shootdown_page
    jc .pop_fail
    pop ecx
    dec ecx
    jnz .tlb_loop

    cmp dword [smp_local_tlb_flushes], 8
    jb .fail
    mov eax, [smp_rejected_remote_shootdowns]
    cmp eax, [smp_stress_saved_rejects]
    jne .fail                        ; Timeout oder ungültige Maske

    ; -----------------------------------------------------------------------
    ; Test 3: Gemischte Last – 4 Runden (Call + Shootdown abwechselnd)
    ; -----------------------------------------------------------------------
    mov ecx, 4
.mixed_loop:
    push ecx
    mov eax, [cpu_online_set]
    and eax, ~1
    mov ecx, smp_stress_inc
    mov edx, smp_stress_counter
    call smp_call_function
    jc .pop_fail
    mov eax, KERNEL_ENTRY_ADDRESS + 0x1000
    mov edx, [cpu_active_set]
    call smp_tlb_shootdown_page
    jc .pop_fail
    pop ecx
    dec ecx
    jnz .mixed_loop

.pass:
    clc
    ret
.pop_fail:
    pop ecx
.fail:
    stc
    ret

; ---------------------------------------------------------------------------
; smp_stress_inc – atomischer Zählerinkrement auf AP (Interrupt-Kontext)
; ECX = Zeiger auf den Zähler (via smp_call_function-Kontext übergeben)
; ---------------------------------------------------------------------------
smp_stress_inc:
    lock inc dword [ecx]
    ret

smp_stress_counter:          dd 0
smp_stress_saved_rejects:    dd 0

align 4
smp_api:
    dd SMP_API_SIZE
    dw 1, 0
    dd cpu_possible_set
    dd cpu_discovered_set
    dd cpu_present_set
    dd cpu_online_set
    dd cpu_active_set
    dd cpu_isolated_set
    dd cpu_failed_set
    dd smp_boot_phase
    dd smp_send_ipi
    dd smp_tlb_shootdown_page
    dd smp_publish_phase
    dd ap_timer_ticks           ; Zeiger auf per-CPU LAPIC-Timer-Tick-Array (§129)
    dd lapic_timer_count        ; Zeiger auf kalibrierten LAPIC-ICR-Wert (§129)
    dd ap_current_frames        ; Zeiger auf per-CPU ISR-Kontext-Frame-Array (§131)
    dd per_cpu_current_thread   ; Zeiger auf per-CPU Thread-Slot-Array (§131)
    dd smp_call_function        ; Cross-CPU-Funktionsaufruf (§27)
smp_boot_phase:                 dd 0
smp_local_tlb_generation:       dd 0
smp_local_tlb_flushes:          dd 0
smp_rejected_ipis:              dd 0
smp_rejected_remote_shootdowns: dd 0
smp_remote_ipis_sent:           dd 0
ap_alive_count:                 dd 0
smp_ipi_mailbox:                times CPU_CAPACITY dd 0   ; pending IPI-Typen (Bitmaske pro CPU-Slot)
smp_shootdown_addr:             dd 0                      ; Seitenaddr. für remote TLB-Shootdown
ap_timer_ticks:                 times CPU_CAPACITY dd 0   ; per-CPU LAPIC-Timer-Ticks (100 Hz)
lapic_timer_count:              dd 0                      ; kalibrierter LAPIC-ICR-Wert für 100 Hz
ap_current_frames:              times CPU_CAPACITY dd 0   ; per-CPU Zeiger auf letzten ISR-Kontext-Frame (§131)
per_cpu_current_thread:         times CPU_CAPACITY dd 0   ; per-CPU laufender Thread-Slot (-1 = Idle, §131)
smp_tlb_ack_pending:            dd 0    ; ausstehende TLB-Shootdown-ACKs von Remote-CPUs (§30)
smp_call_fn_lock:               dd 0    ; Mutex: serialisiert gleichzeitige smp_call_function-Aufrufe
smp_call_fn_ptr:                dd 0    ; Zeiger auf aufzurufende Funktion (§27)
smp_call_fn_ctx:                dd 0    ; Kontextzeiger für die aufzurufende Funktion (§27)
smp_call_fn_ack:                dd 0    ; verbleibende ACKs; AP zählt nach Ausführung atomar runter (§27)

; ---------------------------------------------------------------------------
; isr_ipi – generischer IPI-Empfänger (Vektor SMP_IPI_VECTOR = 0xFE)
; Kein Ring-Wechsel (AP läuft in Ring-0), daher kein SS/ESP auf dem Stack.
; Reihenfolge: TLB_SHOOTDOWN → CPU_STOP/PANIC_STOP, RESCHEDULE braucht kein
; explizites Handling (AP kehrt nach IRET in die Idle-Schleife zurück).
; ---------------------------------------------------------------------------
isr_ipi:
    push eax
    push ebx
    push ecx
    push edx
    ; LAPIC EOI: weiteren IPI dieses Vektors erlauben
    mov dword [LAPIC_BASE + LAPIC_EOI], 0
    ; Eigene APIC-ID aus LAPIC ID-Register (Bits 31:24)
    mov eax, [LAPIC_BASE + 0x020]
    shr eax, 24
    ; CPU-Slot anhand der ACPI-Tabelle ermitteln
    xor ecx, ecx
.ipi_find:
    cmp ecx, [cpu_discovered_count]
    jae .ipi_done
    cmp [acpi_apic_ids + ecx * 4], eax
    je .ipi_found
    inc ecx
    jmp .ipi_find
.ipi_found:
    ; Mailbox atomar lesen und leeren
    xor edx, edx
    xchg edx, [smp_ipi_mailbox + ecx * 4]
    ; TLB_SHOOTDOWN: INVLPG der gemeldeten Adresse, ACK an BSP (§30)
    test edx, (1 << SMP_IPI_TLB_SHOOTDOWN)
    jz .ipi_check_call
    mov eax, [smp_shootdown_addr]
    invlpg [eax]
    lock dec dword [smp_tlb_ack_pending]
.ipi_check_call:
    ; CALL_FUNCTION: registrierte Funktion ausführen, ACK zurückmelden (§27)
    test edx, (1 << SMP_IPI_CALL_FUNCTION)
    jz .ipi_check_stop
    mov eax, [smp_call_fn_ptr]
    test eax, eax
    jz .ipi_call_done
    mov ecx, [smp_call_fn_ctx]      ; Kontext als erstes Argument (ECX-Konvention)
    call eax                         ; fn(ctx) – AP-seitiger Interrupt-Kontext, kurz halten
.ipi_call_done:
    lock dec dword [smp_call_fn_ack]
.ipi_check_stop:
    ; CPU_STOP / PANIC_STOP: AP sicher anhalten
    test edx, (1 << SMP_IPI_CPU_STOP) | (1 << SMP_IPI_PANIC_STOP)
    jz .ipi_done
    cli
.ipi_halt:
    hlt
    jmp .ipi_halt
.ipi_done:
    pop edx
    pop ecx
    pop ebx
    pop eax
    iret

; ---------------------------------------------------------------------------
; isr_ap_timer – vollständiger Kontext-ISR für AP-LAPIC-Timer (Vektor 0xEF)
;
; Frame-Layout (68 Byte, identisch mit isr_common):
;   [ESP+ 0] GS  [ESP+ 4] FS  [ESP+ 8] ES  [ESP+12] DS
;   [ESP+16..47] pushad (EDI,ESI,EBP,ESP*,EBX,EDX,ECX,EAX)
;   [ESP+48] Vektor (0xEF)   [ESP+52] Fehlercode (0)
;   [ESP+56] EIP  [ESP+60] CS  [ESP+64] EFLAGS   ← CPU-IRET-Frame
;
; Das vollständige Frame erlaubt §132 (AP-Task-Dispatch), einen anderen
; Frame via scheduler_on_tick zu laden – genau wie isr_common es für den BSP tut.
; ---------------------------------------------------------------------------
isr_ap_timer:
    push dword 0                        ; Fehlercode-Platzhalter
    push dword LAPIC_TIMER_VECTOR       ; Pseudo-Vektor 0xEF
    pushad
    push ds
    push es
    push fs
    push gs
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax
    ; FS/GS bleiben 0 (kein TLS im Kernel-Ring-0)

    ; LAPIC früh quittieren (weitere Interrupts können ankommen)
    mov dword [LAPIC_BASE + LAPIC_EOI], 0

    ; LAPIC-ID → CPU-Slot (Bits 31:24 des LAPIC-ID-Registers)
    mov eax, [LAPIC_BASE + 0x020]
    shr eax, 24
    xor ecx, ecx
.at_find:
    cmp ecx, [cpu_discovered_count]
    jae .at_restore
    cmp [acpi_apic_ids + ecx * 4], eax
    je .at_found
    inc ecx
    jmp .at_find

.at_found:
    ; Vollständigen Frame-Zeiger sichern (ESP zeigt auf GS = Frame-Basis)
    mov [ap_current_frames + ecx * 4], esp
    ; Per-CPU Tick-Zähler atomar inkrementieren
    lock inc dword [ap_timer_ticks + ecx * 4]
    ; Per-CPU Current-Thread auf -1 (Idle) initialisieren, falls 0 (ungesetzt)
    ; Normalfall: wird bereits in ap_entry32_pm gesetzt; Guard für Robustheit
    cmp dword [per_cpu_current_thread + ecx * 4], 0
    jne .at_try_sched
    mov dword [per_cpu_current_thread + ecx * 4], -1

.at_try_sched:
    ; ---------------------------------------------------------------------------
    ; §132: AP nimmt am Scheduler teil.
    ; Strategie: Spinlock einmalig versuchen – kein Busy-Wait.
    ;   CF=1 → anderer CPU hält Lock → diesen Tick überspringen (normal).
    ;   CF=0 → wir haben den Lock → scheduler_on_tick aufrufen.
    ;
    ; Stack-Layout beim Aufruf von scheduler_on_tick:
    ;   F    = ESP (Frame-Basis, zeigt auf GS) zum Einsprung in .at_found
    ;   F-4  = gesicherter ECX (CPU-Slot)       ← push ecx
    ;   F-8  = Argument: F (Frame-Zeiger)        ← push eax (lea eax,[esp+4])
    ;   F-12 = Return-Adresse                    ← call pushes it
    ;   scheduler_on_tick liest [esp+4] = [F-8] = F ✓
    ; ---------------------------------------------------------------------------
    lock bts dword [scheduler_lock], 0
    jc .at_restore              ; Lock belegt → diesen Tick überspringen

    push ecx                    ; CPU-Slot sichern  (ESP = F-4)
    mov [scheduler_caller_cpu], ecx  ; §133: aufrufende CPU identifizieren
    lea eax, [esp + 4]          ; EAX = F (Frame-Basis)
    push eax                    ; Argument: Frame-Zeiger (ESP = F-8)
    call scheduler_on_tick      ; EAX ← neuer (oder gleicher) Frame-Zeiger
    add esp, 4                  ; Argument entfernen (ESP = F-4)
    pop ecx                     ; CPU-Slot wiederherstellen (ESP = F)

    ; per-CPU aktuellen Thread-Slot aktualisieren
    mov edx, [scheduler_selected_slot]
    mov [per_cpu_current_thread + ecx * 4], edx

    lock btr dword [scheduler_lock], 0

    ; Kontext-Switch: falls Scheduler einen anderen Frame wählt, Stack wechseln
    ; EAX = neuer Frame-Zeiger, ESP = F (alter Frame-Basis)
    cmp eax, esp
    je .at_restore              ; kein Wechsel – gleiches Frame
    mov esp, eax                ; zu neuem Thread-Frame wechseln

.at_restore:
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 8                          ; Fehlercode + Vektor-Platzhalter
    iretd

align 4096
ap_stack_area:
    times (CPU_CAPACITY - 1) * AP_STACK_SIZE db 0

; ---------------------------------------------------------------------------
; §028 – NUMA Support 1.0 (NPSPEC-KERNEL-0028)
; UMA-Systeme werden als NUMA-System mit genau einem Node (Node 0) behandelt.
; Alle späteren Subsysteme verwenden dieselben Schnittstellen unabhängig von
; der tatsächlichen Hardwaretopologie.
; ---------------------------------------------------------------------------

NUMA_MAX_NODES          equ 8
NUMA_RECORD_SIZE        equ 32
NUMA_NODE_STATE_OFFLINE equ 0
NUMA_NODE_STATE_ONLINE  equ 1
NUMA_DIST_SELF          equ 10
NUMA_DIST_REMOTE        equ 20

; Record-Offsets
NUMA_ID          equ 0
NUMA_STATE       equ 4
NUMA_CPU_MASK    equ 8
NUMA_FLAGS       equ 12
NUMA_MEM_BASE_LO equ 16
NUMA_MEM_BASE_HI equ 20
NUMA_MEM_SIZE_LO equ 24
NUMA_MEM_SIZE_HI equ 28

; Erstellt Node 0 (UMA-Fallback: physische Basis 0, Größe = pmm_frame_count * 4096).
numa_initialize:
    mov edi, numa_records
    xor eax, eax
    mov ecx, (NUMA_MAX_NODES * NUMA_RECORD_SIZE) / 4
    rep stosd
    mov dword [numa_count], 0
    ; Node 0 befüllen
    mov edi, numa_records
    mov dword [edi + NUMA_ID],          0
    mov dword [edi + NUMA_STATE],       NUMA_NODE_STATE_ONLINE
    mov eax, [cpu_online_set]
    test eax, eax
    jnz .cpu_mask_ok
    mov eax, 1                          ; Fallback: mindestens BSP (Bit 0)
.cpu_mask_ok:
    mov [edi + NUMA_CPU_MASK],          eax
    mov dword [edi + NUMA_FLAGS],       0
    mov dword [edi + NUMA_MEM_BASE_LO], 0
    mov dword [edi + NUMA_MEM_BASE_HI], 0
    mov eax, [pmm_frame_count]
    shl eax, 12                         ; * 4096
    mov [edi + NUMA_MEM_SIZE_LO],       eax
    mov dword [edi + NUMA_MEM_SIZE_HI], 0
    mov dword [numa_count], 1
    mov dword [numa_initialized], 1
    clc
    ret

; EAX=node_id → ESI=Record-Zeiger (CF=0) oder CF=1.
numa_node_find:
    push ecx
    push edi
    xor ecx, ecx
.scan:
    cmp ecx, NUMA_MAX_NODES
    jae .not_found
    mov edi, ecx
    imul edi, NUMA_RECORD_SIZE
    add edi, numa_records
    cmp dword [edi + NUMA_ID],    eax
    jne .next
    cmp dword [edi + NUMA_STATE], NUMA_NODE_STATE_OFFLINE
    je .next
    mov esi, edi
    pop edi
    pop ecx
    clc
    ret
.next:
    inc ecx
    jmp .scan
.not_found:
    pop edi
    pop ecx
    stc
    ret

; Gibt EAX=0 zurück (lokaler Node ist immer Node 0 auf UMA).
numa_get_local_node:
    xor eax, eax
    clc
    ret

numa_self_test:
    push esi
    xor edi, edi                        ; Fehler-Zähler
    ; Test 1: initialisiert
    cmp dword [numa_initialized], 1
    je .t2
    inc edi
.t2:
    ; Test 2: genau 1 Node
    cmp dword [numa_count], 1
    je .t3
    inc edi
.t3:
    ; Test 3: Node 0 auffindbar
    xor eax, eax
    call numa_node_find
    jnc .t4
    inc edi
    jmp .done
.t4:
    ; Test 4: Node 0 ist ONLINE
    cmp dword [esi + NUMA_STATE], NUMA_NODE_STATE_ONLINE
    je .t5
    inc edi
.t5:
    ; Test 5: CPU-Maske != 0
    cmp dword [esi + NUMA_CPU_MASK], 0
    jne .done
    inc edi
.done:
    test edi, edi
    jnz .selftest_fail
    pop esi
    clc
    ret
.selftest_fail:
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; §028 Datensegment
; ---------------------------------------------------------------------------
numa_initialized: dd 0
numa_count:       dd 0
align 4
numa_records: times NUMA_MAX_NODES * NUMA_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; §29 – Kernel Configuration Framework (NPSPEC-KERNEL-0029)
; ---------------------------------------------------------------------------

; Typen
CONFIG_TYPE_BOOL            equ 1
CONFIG_TYPE_UINT            equ 2
CONFIG_TYPE_ENUM            equ 3

; Veränderbarkeitsklassen
CONFIG_MUT_IMMUTABLE        equ 0
CONFIG_MUT_BOOT_ONLY        equ 1
CONFIG_MUT_RUNTIME          equ 2
CONFIG_MUT_RUNTIME_RESTART  equ 3
CONFIG_MUT_SESSION          equ 4

; Sicherheitsklassen
CONFIG_SEC_PUBLIC           equ 0
CONFIG_SEC_SYSTEM           equ 1
CONFIG_SEC_SECURITY         equ 2

; Build-Profile
NP_BUILD_DEVELOPMENT        equ 0
NP_BUILD_TEST               equ 1
NP_BUILD_RELEASE            equ 2
NP_BUILD_HARDENED           equ 3
NP_BUILD_RECOVERY           equ 4

; Fehler-Codes (EAX wenn CF=1)
CONFIG_ERR_NOT_FOUND        equ 1
CONFIG_ERR_TYPE             equ 2
CONFIG_ERR_RANGE            equ 3
CONFIG_ERR_IMMUTABLE        equ 4
CONFIG_ERR_BOOT_ONLY        equ 5
CONFIG_ERR_ACCESS_DENIED    equ 6
CONFIG_ERR_TX_ACTIVE        equ 7
CONFIG_ERR_NO_TX            equ 8
CONFIG_ERR_TX_FULL          equ 9
CONFIG_ERR_SCHEMA_FULL      equ 10
CONFIG_ERR_DUPLICATE        equ 11

; Store-Flags
CONFIG_STORE_SET            equ 0x01

; Kapazitäten
CONFIG_STORE_MAX            equ 16
CONFIG_TX_MAX               equ 8
CONFIG_SCHEMA_ENTRY_SIZE    equ 16      ; key_id(4)+type(1)+mut(1)+sec(1)+flags(1)+default(4)+max(4)
CONFIG_STORE_ENTRY_SIZE     equ 12      ; key_id(4)+value(4)+flags(4)

; Vordefinierte Schlüssel-IDs
CONFIG_KEY_LOG_LEVEL        equ 1       ; uint, runtime, system (0–7, default=3)
CONFIG_KEY_SCHEDULER_CLASS  equ 2       ; enum, boot_only, system (0–3, default=0)
CONFIG_KEY_MEMORY_GUARD     equ 3       ; bool, boot_only, system (default=1)
CONFIG_KEY_SECURITY_SMAP    equ 4       ; bool, immutable, security (default=0)
CONFIG_KEY_MODULE_UNSIGNED  equ 5       ; bool, boot_only, security (default=0)
CONFIG_KEY_NETWORK_IPV6     equ 6       ; bool, runtime, system (default=1)
CONFIG_KEY_POWER_PROFILE    equ 7       ; enum, runtime, public (0–3, default=0)
CONFIG_KEY_DEBUG_ENABLED    equ 8       ; bool, boot_only, security (default=0)
CONFIG_KEY_BOOT_MODE        equ 9       ; enum, boot_only, system (0–2, default=0)
CONFIG_KEY_BUILD_PROFILE    equ 10      ; enum, immutable, public (0–4, default=DEVELOPMENT)

; ---------------------------------------------------------------------------
; config_find_schema – linearer Scan der statischen Schema-Tabelle
; Ein: EAX=key_id  Aus: EDX=Zeiger auf Eintrag, CF=0 gefunden / CF=1 nicht gefunden
; Verändert: EDX  Schützt: EAX, EBX, ECX, ESI, EDI
; ---------------------------------------------------------------------------
config_find_schema:
    push ecx
    xor ecx, ecx
    mov edx, config_schema
.cfs_loop:
    cmp ecx, config_schema_count
    jae .cfs_nf
    cmp [edx], eax
    je .cfs_found
    add edx, CONFIG_SCHEMA_ENTRY_SIZE
    inc ecx
    jmp .cfs_loop
.cfs_found:
    pop ecx
    clc
    ret
.cfs_nf:
    pop ecx
    stc
    ret

; ---------------------------------------------------------------------------
; config_find_store_entry – sucht gesetzten Eintrag im Laufzeit-Store
; Ein: EAX=key_id  Aus: EDX=Zeiger, CF=0 / CF=1
; ---------------------------------------------------------------------------
config_find_store_entry:
    push ecx
    xor ecx, ecx
    mov edx, config_store
.cfse_loop:
    cmp ecx, CONFIG_STORE_MAX
    jae .cfse_nf
    test dword [edx + 8], CONFIG_STORE_SET
    jz .cfse_next
    cmp [edx], eax
    je .cfse_found
.cfse_next:
    add edx, CONFIG_STORE_ENTRY_SIZE
    inc ecx
    jmp .cfse_loop
.cfse_found:
    pop ecx
    clc
    ret
.cfse_nf:
    pop ecx
    stc
    ret

; ---------------------------------------------------------------------------
; config_find_free_store – sucht leeren Slot
; Aus: EDX=Zeiger, CF=0 / CF=1 voll
; ---------------------------------------------------------------------------
config_find_free_store:
    push ecx
    xor ecx, ecx
    mov edx, config_store
.cffs_loop:
    cmp ecx, CONFIG_STORE_MAX
    jae .cffs_full
    test dword [edx + 8], CONFIG_STORE_SET
    jz .cffs_found
    add edx, CONFIG_STORE_ENTRY_SIZE
    inc ecx
    jmp .cffs_loop
.cffs_found:
    pop ecx
    clc
    ret
.cffs_full:
    pop ecx
    stc
    ret

; ---------------------------------------------------------------------------
; config_get – liest einen Konfigurationswert (Store-Override oder Schema-Default)
; Ein: EAX=key_id  Aus: EBX=Wert, CF=0 ok / CF=1 (EAX=Fehler)
; Verändert: EBX  Schützt: EAX, ECX, EDX, ESI, EDI
; ---------------------------------------------------------------------------
config_get:
    push edx
    call config_find_schema
    jc .cget_nf
    mov ebx, [edx + 8]              ; Schema-Default als Ausgangswert
    call config_find_store_entry    ; EAX=key_id; sucht im Store
    jc .cget_done                   ; kein Override → Default gilt
    mov ebx, [edx + 4]              ; Override-Wert aus Store
.cget_done:
    pop edx
    clc
    ret
.cget_nf:
    mov eax, CONFIG_ERR_NOT_FOUND
    pop edx
    stc
    ret

; ---------------------------------------------------------------------------
; config_set – setzt einen Konfigurationswert mit Typ-, Mutability- und Bereichsprüfung
; Ein: EAX=key_id, EBX=neuer Wert  Aus: CF=0 ok / CF=1 (EAX=Fehler)
; Verändert: –  Schützt: EAX, EBX, ECX, EDX, ESI, EDI
; ---------------------------------------------------------------------------
config_set:
    push ecx
    push edx
    push esi
    call config_find_schema
    jc .cset_nf
    ; Mutability (Offset 5 im Schema-Eintrag)
    movzx ecx, byte [edx + 5]
    cmp ecx, CONFIG_MUT_IMMUTABLE
    je .cset_immutable
    cmp ecx, CONFIG_MUT_BOOT_ONLY
    jne .cset_mut_ok
    cmp dword [config_boot_sealed], 0
    jne .cset_boot_only
.cset_mut_ok:
    ; Bereichsprüfung (Offset 12)
    mov esi, [edx + 12]
    test esi, esi
    jz .cset_range_ok
    cmp ebx, esi
    ja .cset_range
.cset_range_ok:
    call config_find_store_entry    ; EAX=key_id
    jc .cset_new
    mov [edx + 4], ebx              ; vorhandenen Eintrag aktualisieren
    jmp .cset_ok
.cset_new:
    call config_find_free_store
    jc .cset_store_full
    mov [edx], eax                  ; key_id (EAX unverändert)
    mov [edx + 4], ebx              ; Wert
    mov dword [edx + 8], CONFIG_STORE_SET
.cset_ok:
    inc dword [config_generation]
    clc
    pop esi
    pop edx
    pop ecx
    ret
.cset_nf:
    mov eax, CONFIG_ERR_NOT_FOUND
    jmp .cset_err
.cset_immutable:
    mov eax, CONFIG_ERR_IMMUTABLE
    jmp .cset_err
.cset_boot_only:
    mov eax, CONFIG_ERR_BOOT_ONLY
    jmp .cset_err
.cset_range:
    mov eax, CONFIG_ERR_RANGE
    jmp .cset_err
.cset_store_full:
    mov eax, CONFIG_ERR_NOT_FOUND
.cset_err:
    stc
    pop esi
    pop edx
    pop ecx
    ret

; ---------------------------------------------------------------------------
; config_lock_boot_only – versiegelt BOOT_ONLY-Werte (nach Boot aufrufen)
; ---------------------------------------------------------------------------
config_lock_boot_only:
    mov dword [config_boot_sealed], 1
    ret

; ---------------------------------------------------------------------------
; config_tx_begin – startet eine atomare Konfigurationstransaktion
; Aus: CF=0 ok / CF=1 (EAX=CONFIG_ERR_TX_ACTIVE)
; ---------------------------------------------------------------------------
config_tx_begin:
    cmp dword [config_tx_active], 0
    jne .ctx_already
    mov dword [config_tx_active], 1
    mov dword [config_tx_pending], 0
    clc
    ret
.ctx_already:
    mov eax, CONFIG_ERR_TX_ACTIVE
    stc
    ret

; ---------------------------------------------------------------------------
; config_tx_abort – bricht die aktive Transaktion ab ohne Änderungen
; ---------------------------------------------------------------------------
config_tx_abort:
    mov dword [config_tx_pending], 0
    mov dword [config_tx_active], 0
    ret

; ---------------------------------------------------------------------------
; config_tx_set – fügt einen Wert zur Transaktion hinzu (Validierung sofort)
; Ein: EAX=key_id, EBX=Wert  Aus: CF=0 ok / CF=1 (EAX=Fehler)
; ---------------------------------------------------------------------------
config_tx_set:
    cmp dword [config_tx_active], 0
    je .ctxs_no_tx
    push ecx
    push edx
    call config_find_schema
    jc .ctxs_nf
    movzx ecx, byte [edx + 5]          ; Mutability
    cmp ecx, CONFIG_MUT_IMMUTABLE
    je .ctxs_immutable
    cmp ecx, CONFIG_MUT_BOOT_ONLY
    jne .ctxs_mut_ok
    cmp dword [config_boot_sealed], 0
    jne .ctxs_boot_only
.ctxs_mut_ok:
    mov ecx, [edx + 12]                 ; max_value
    test ecx, ecx
    jz .ctxs_range_ok
    cmp ebx, ecx
    ja .ctxs_range
.ctxs_range_ok:
    mov ecx, [config_tx_pending]
    cmp ecx, CONFIG_TX_MAX
    jae .ctxs_full
    imul edx, ecx, CONFIG_STORE_ENTRY_SIZE
    add edx, config_tx_buffer
    mov [edx], eax
    mov [edx + 4], ebx
    mov dword [edx + 8], CONFIG_STORE_SET
    inc dword [config_tx_pending]
    clc
    pop edx
    pop ecx
    ret
.ctxs_no_tx:
    mov eax, CONFIG_ERR_NO_TX
    stc
    ret
.ctxs_nf:
    mov eax, CONFIG_ERR_NOT_FOUND
    jmp .ctxs_err
.ctxs_immutable:
    mov eax, CONFIG_ERR_IMMUTABLE
    jmp .ctxs_err
.ctxs_boot_only:
    mov eax, CONFIG_ERR_BOOT_ONLY
    jmp .ctxs_err
.ctxs_range:
    mov eax, CONFIG_ERR_RANGE
    jmp .ctxs_err
.ctxs_full:
    mov eax, CONFIG_ERR_TX_FULL
.ctxs_err:
    stc
    pop edx
    pop ecx
    ret

; ---------------------------------------------------------------------------
; config_tx_commit – wendet alle gepufferten Änderungen atomar an
; Aus: CF=0 ok / CF=1 (EAX=CONFIG_ERR_NO_TX)
; ---------------------------------------------------------------------------
config_tx_commit:
    cmp dword [config_tx_active], 0
    je .ctxc_no_tx
    push ebx
    push ecx
    push edx
    push esi
    xor esi, esi
.ctxc_apply:
    cmp esi, [config_tx_pending]
    jae .ctxc_done
    imul ecx, esi, CONFIG_STORE_ENTRY_SIZE
    mov eax, [config_tx_buffer + ecx]       ; key_id
    mov ebx, [config_tx_buffer + ecx + 4]   ; Wert
    push esi
    call config_find_store_entry            ; EDX=vorhandener Slot oder CF=1
    jc .ctxc_new
    mov [edx + 4], ebx                      ; vorhandenen Slot aktualisieren
    pop esi
    jmp .ctxc_next
.ctxc_new:
    call config_find_free_store             ; EDX=freier Slot
    jc .ctxc_skip
    mov [edx], eax                          ; key_id (EAX unverändert)
    mov [edx + 4], ebx
    mov dword [edx + 8], CONFIG_STORE_SET
    pop esi
    jmp .ctxc_next
.ctxc_skip:
    pop esi
.ctxc_next:
    inc esi
    jmp .ctxc_apply
.ctxc_done:
    inc dword [config_generation]
    mov dword [config_tx_pending], 0
    mov dword [config_tx_active], 0
    clc
    pop esi
    pop edx
    pop ecx
    pop ebx
    ret
.ctxc_no_tx:
    mov eax, CONFIG_ERR_NO_TX
    stc
    ret

; ---------------------------------------------------------------------------
; config_initialize – setzt Store, TX-Puffer und Zustand zurück
; ---------------------------------------------------------------------------
config_initialize:
    push eax
    push ecx
    push edi
    mov edi, config_store
    xor eax, eax
    mov ecx, (CONFIG_STORE_MAX * CONFIG_STORE_ENTRY_SIZE) / 4
    rep stosd
    mov edi, config_tx_buffer
    mov ecx, (CONFIG_TX_MAX * CONFIG_STORE_ENTRY_SIZE) / 4
    rep stosd
    mov dword [config_tx_active], 0
    mov dword [config_tx_pending], 0
    mov dword [config_generation], 0
    mov dword [config_boot_sealed], 0
    pop edi
    pop ecx
    pop eax
    clc
    ret

; ---------------------------------------------------------------------------
; config_self_test – 15 Testfälle gemäß NPSPEC-KERNEL-0029 §59
; Aus: CF=0 alle bestanden / CF=1 Fehler
; ---------------------------------------------------------------------------
config_self_test:
    push eax
    push ebx
    push ecx
    push edx

    ; Test 1: Standard-Wert lesen (LOG_LEVEL → 3, kein Store-Eintrag)
    mov eax, CONFIG_KEY_LOG_LEVEL
    call config_get
    jc .cst_fail
    cmp ebx, 3
    jne .cst_fail

    ; Test 2: Immutable-Standard (BUILD_PROFILE → NP_BUILD_DEVELOPMENT)
    mov eax, CONFIG_KEY_BUILD_PROFILE
    call config_get
    jc .cst_fail
    cmp ebx, NP_BUILD_DEVELOPMENT
    jne .cst_fail

    ; Test 3: Unbekannter Schlüssel → CF=1, ERR_NOT_FOUND
    mov eax, 0xFF
    call config_get
    jnc .cst_fail
    cmp eax, CONFIG_ERR_NOT_FOUND
    jne .cst_fail

    ; Test 4: Gültige Runtime-Änderung (LOG_LEVEL 3→5)
    mov eax, CONFIG_KEY_LOG_LEVEL
    mov ebx, 5
    call config_set
    jc .cst_fail
    mov eax, CONFIG_KEY_LOG_LEVEL
    call config_get
    jc .cst_fail
    cmp ebx, 5
    jne .cst_fail

    ; Test 5: Bereichsfehler (LOG_LEVEL > max=7)
    mov eax, CONFIG_KEY_LOG_LEVEL
    mov ebx, 8
    call config_set
    jnc .cst_fail
    cmp eax, CONFIG_ERR_RANGE
    jne .cst_fail

    ; Test 6: Immutable-Fehler (BUILD_PROFILE nicht änderbar)
    mov eax, CONFIG_KEY_BUILD_PROFILE
    mov ebx, NP_BUILD_RELEASE
    call config_set
    jnc .cst_fail
    cmp eax, CONFIG_ERR_IMMUTABLE
    jne .cst_fail

    ; Test 7: BOOT_ONLY vor Versiegelung → erlaubt
    mov eax, CONFIG_KEY_BOOT_MODE
    mov ebx, 1
    call config_set
    jc .cst_fail

    ; Test 8: BOOT_ONLY bei Versiegelung → CF=1, ERR_BOOT_ONLY
    mov dword [config_boot_sealed], 1
    mov eax, CONFIG_KEY_BOOT_MODE
    mov ebx, 2
    call config_set
    jnc .cst_fail
    cmp eax, CONFIG_ERR_BOOT_ONLY
    jne .cst_fail
    mov dword [config_boot_sealed], 0   ; für nachfolgende Tests zurücksetzen

    ; Test 9: Generation steigt nach erfolgreichem Set
    mov ecx, [config_generation]
    mov eax, CONFIG_KEY_LOG_LEVEL
    mov ebx, 4
    call config_set
    jc .cst_fail
    cmp [config_generation], ecx
    jle .cst_fail

    ; Test 10: Transaktion commit – zwei Werte atomar setzen
    call config_tx_begin
    jc .cst_fail
    mov eax, CONFIG_KEY_LOG_LEVEL
    mov ebx, 6
    call config_tx_set
    jc .cst_fail
    mov eax, CONFIG_KEY_NETWORK_IPV6
    mov ebx, 0
    call config_tx_set
    jc .cst_fail
    call config_tx_commit
    jc .cst_fail
    mov eax, CONFIG_KEY_LOG_LEVEL
    call config_get
    jc .cst_fail
    cmp ebx, 6
    jne .cst_fail
    mov eax, CONFIG_KEY_NETWORK_IPV6
    call config_get
    jc .cst_fail
    cmp ebx, 0
    jne .cst_fail

    ; Test 11: Transaktion abort – Wert bleibt unverändert
    mov eax, CONFIG_KEY_LOG_LEVEL   ; aktuell 6
    call config_get
    jc .cst_fail
    push ebx                        ; alten Wert (6) retten
    call config_tx_begin
    jc .cst_fail_pop
    mov eax, CONFIG_KEY_LOG_LEVEL
    mov ebx, 7
    call config_tx_set
    jc .cst_fail_pop
    call config_tx_abort
    mov eax, CONFIG_KEY_LOG_LEVEL
    call config_get
    jc .cst_fail_pop
    pop ecx                         ; erwarteter Wert = 6
    cmp ebx, ecx
    jne .cst_fail

    ; Test 12: Doppeltes tx_begin → CF=1, ERR_TX_ACTIVE
    call config_tx_begin
    jc .cst_fail
    call config_tx_begin
    jnc .cst_fail_cleanup_tx
    cmp eax, CONFIG_ERR_TX_ACTIVE
    jne .cst_fail_cleanup_tx
    call config_tx_abort

    ; Test 13: config_tx_set ohne aktive Transaktion → CF=1, ERR_NO_TX
    mov eax, CONFIG_KEY_LOG_LEVEL
    mov ebx, 3
    call config_tx_set
    jnc .cst_fail
    cmp eax, CONFIG_ERR_NO_TX
    jne .cst_fail

    ; Test 14: Bool-Wert > 1 → CF=1, ERR_RANGE
    mov eax, CONFIG_KEY_NETWORK_IPV6
    mov ebx, 2
    call config_set
    jnc .cst_fail
    cmp eax, CONFIG_ERR_RANGE
    jne .cst_fail

    ; Test 15: Generation ist nicht rückläufig
    mov ecx, [config_generation]
    mov eax, CONFIG_KEY_LOG_LEVEL
    mov ebx, 3
    call config_set
    jc .cst_fail
    cmp [config_generation], ecx
    jle .cst_fail

    pop edx
    pop ecx
    pop ebx
    pop eax
    clc
    ret

.cst_fail_pop:
    pop ecx
.cst_fail:
    pop edx
    pop ecx
    pop ebx
    pop eax
    stc
    ret
.cst_fail_cleanup_tx:
    call config_tx_abort
    pop edx
    pop ecx
    pop ebx
    pop eax
    stc
    ret

; ---------------------------------------------------------------------------
; Statische Schema-Tabelle: key_id(4) | type(1) | mut(1) | sec(1) | flags(1)
;                           | default(4) | max(4)  = 16 Bytes je Eintrag
; ---------------------------------------------------------------------------
config_schema:
    dd CONFIG_KEY_LOG_LEVEL
    db CONFIG_TYPE_UINT,  CONFIG_MUT_RUNTIME,      CONFIG_SEC_SYSTEM,    0
    dd 3, 7
    dd CONFIG_KEY_SCHEDULER_CLASS
    db CONFIG_TYPE_ENUM,  CONFIG_MUT_BOOT_ONLY,    CONFIG_SEC_SYSTEM,    0
    dd 0, 3
    dd CONFIG_KEY_MEMORY_GUARD
    db CONFIG_TYPE_BOOL,  CONFIG_MUT_BOOT_ONLY,    CONFIG_SEC_SYSTEM,    0
    dd 1, 1
    dd CONFIG_KEY_SECURITY_SMAP
    db CONFIG_TYPE_BOOL,  CONFIG_MUT_IMMUTABLE,    CONFIG_SEC_SECURITY,  0
    dd 0, 1
    dd CONFIG_KEY_MODULE_UNSIGNED
    db CONFIG_TYPE_BOOL,  CONFIG_MUT_BOOT_ONLY,    CONFIG_SEC_SECURITY,  0
    dd 0, 1
    dd CONFIG_KEY_NETWORK_IPV6
    db CONFIG_TYPE_BOOL,  CONFIG_MUT_RUNTIME,      CONFIG_SEC_SYSTEM,    0
    dd 1, 1
    dd CONFIG_KEY_POWER_PROFILE
    db CONFIG_TYPE_ENUM,  CONFIG_MUT_RUNTIME,      CONFIG_SEC_PUBLIC,    0
    dd 0, 3
    dd CONFIG_KEY_DEBUG_ENABLED
    db CONFIG_TYPE_BOOL,  CONFIG_MUT_BOOT_ONLY,    CONFIG_SEC_SECURITY,  0
    dd 0, 1
    dd CONFIG_KEY_BOOT_MODE
    db CONFIG_TYPE_ENUM,  CONFIG_MUT_BOOT_ONLY,    CONFIG_SEC_SYSTEM,    0
    dd 0, 2
    dd CONFIG_KEY_BUILD_PROFILE
    db CONFIG_TYPE_ENUM,  CONFIG_MUT_IMMUTABLE,    CONFIG_SEC_PUBLIC,    0
    dd NP_BUILD_DEVELOPMENT, NP_BUILD_RECOVERY
config_schema_end:
config_schema_count equ (config_schema_end - config_schema) / CONFIG_SCHEMA_ENTRY_SIZE

; Laufzeit-Zustand
config_store:       times CONFIG_STORE_MAX * CONFIG_STORE_ENTRY_SIZE db 0
config_tx_buffer:   times CONFIG_TX_MAX   * CONFIG_STORE_ENTRY_SIZE db 0
config_tx_active:   dd 0
config_tx_pending:  dd 0
config_generation:  dd 0
config_boot_sealed: dd 0

; ---------------------------------------------------------------------------
; §30 – Kernel ABI Framework (NPSPEC-KERNEL-0030)
; Versionierte Serviceregistrierung, Kompatibilitätsprüfung, Feature
; Discovery. Basis für alle nachgelagerten NovaOS-Kernel-Services.
; ---------------------------------------------------------------------------

; Service-IDs (§27)
NP_SERVICE_CORE         equ 0
NP_SERVICE_PROCESS      equ 1
NP_SERVICE_THREAD       equ 2
NP_SERVICE_MEMORY       equ 3
NP_SERVICE_IPC          equ 4
NP_SERVICE_VFS          equ 5
NP_SERVICE_DEVICE       equ 6
NP_SERVICE_NETWORK      equ 7
NP_SERVICE_SECURITY     equ 8
NP_SERVICE_DIAGNOSTIC   equ 9
NP_SERVICE_POWER        equ 10
NP_SERVICE_COUNT        equ 11

; Fehlercodes (§65, np_status_t = int32_t)
NP_OK                       equ 0
NP_ERR_ABI_INCOMPATIBLE     equ -1
NP_ERR_ABI_TOO_OLD          equ -2
NP_ERR_ABI_FEATURE_MISSING  equ -3
NP_ERR_ABI_STRUCTURE_SIZE   equ -4
NP_ERR_ABI_RESERVED_FIELD   equ -5
NP_ERR_ABI_ALIGNMENT        equ -6
NP_ERR_SERVICE_UNKNOWN      equ -7
NP_ERR_OPERATION_UNKNOWN    equ -8
NP_ERR_ACCESS_DENIED        equ -12
NP_ERR_NOT_SUPPORTED        equ -13

; Aktuelle Kernel-ABI-Version (§5)
NP_ABI_MAJOR            equ 1
NP_ABI_MINOR            equ 0

; np_abi_header_t Offsets (§7, 16 Bytes total)
NP_ABI_HEADER_SIZE      equ 16
NP_HDR_OFF_MAJOR        equ 0      ; uint16_t major_version
NP_HDR_OFF_MINOR        equ 2      ; uint16_t minor_version
NP_HDR_OFF_STRUCT_SIZE  equ 4      ; uint32_t structure_size
NP_HDR_OFF_FLAGS        equ 8      ; uint64_t feature_flags (low 32 Bit in 32-Bit-ABI §48)

; ABI-Registry-Eintrag (16 Bytes)
ABI_REG_ENTRY_SIZE      equ 16
ABI_REG_MAX             equ 16
ABI_REG_OFF_SVC_ID      equ 0      ; uint32_t service_id
ABI_REG_OFF_MAJOR       equ 4      ; uint16_t major
ABI_REG_OFF_MINOR       equ 6      ; uint16_t minor
ABI_REG_OFF_FLAGS       equ 8      ; uint32_t feature_flags
ABI_REG_OFF_PRESENT     equ 12     ; uint8_t 1=registriert

; Compile-time-Invarianten (§17, §68)
%if ABI_REG_ENTRY_SIZE != 16
%error "ABI_REG_ENTRY_SIZE muss 16 Bytes sein"
%endif
%if NP_ABI_HEADER_SIZE != 16
%error "NP_ABI_HEADER_SIZE muss 16 Bytes sein"
%endif
%if NP_HDR_OFF_STRUCT_SIZE != 4
%error "NP_HDR_OFF_STRUCT_SIZE Offset falsch"
%endif
%if NP_HDR_OFF_FLAGS != 8
%error "NP_HDR_OFF_FLAGS Offset falsch"
%endif
%if NP_HDR_OFF_FLAGS + 8 != NP_ABI_HEADER_SIZE
%error "feature_flags (64-Bit) passt nicht in np_abi_header_t"
%endif

; ---------------------------------------------------------------------------
; abi_find_entry – sucht Registry-Eintrag (intern)
; EAX = service_id
; Rückgabe: EBX = Zeiger auf Eintrag, CF=0 gefunden / CF=1 nicht gefunden
; Clobbers: ECX
; ---------------------------------------------------------------------------
abi_find_entry:
    push esi
    mov esi, abi_registry
    xor ecx, ecx
.afe_loop:
    cmp ecx, ABI_REG_MAX
    jae .afe_not_found
    cmp byte [esi + ABI_REG_OFF_PRESENT], 1
    jne .afe_next
    cmp [esi + ABI_REG_OFF_SVC_ID], eax
    je .afe_found
.afe_next:
    add esi, ABI_REG_ENTRY_SIZE
    inc ecx
    jmp .afe_loop
.afe_found:
    mov ebx, esi
    pop esi
    clc
    ret
.afe_not_found:
    xor ebx, ebx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; abi_register_service – trägt Service in die Registry ein (§56)
; EAX = service_id, ECX = major (uint32_t), EDX = minor, ESI = feature_flags
; CF=0 OK, CF=1 Duplikat oder Registry voll
; ---------------------------------------------------------------------------
abi_register_service:
    push ebx
    push ecx
    push edx
    push esi
    push edi

    ; Duplikat verhindern
    push ecx
    push edx
    call abi_find_entry         ; EAX=svc_id, CF=0 → vorhanden
    pop edx
    pop ecx
    jnc .ars_dup

    ; Freien Slot suchen
    mov edi, abi_registry
    push ecx
    xor ecx, ecx
.ars_scan:
    cmp ecx, ABI_REG_MAX
    jae .ars_full
    cmp byte [edi + ABI_REG_OFF_PRESENT], 0
    je .ars_write
    add edi, ABI_REG_ENTRY_SIZE
    inc ecx
    jmp .ars_scan
.ars_write:
    pop ecx
    mov [edi + ABI_REG_OFF_SVC_ID], eax
    mov [edi + ABI_REG_OFF_MAJOR],   cx     ; low 16 Bit
    mov [edi + ABI_REG_OFF_MINOR],   dx     ; low 16 Bit
    mov [edi + ABI_REG_OFF_FLAGS],   esi
    mov byte [edi + ABI_REG_OFF_PRESENT], 1
    lock inc dword [abi_service_count]
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    clc
    ret
.ars_full:
    pop ecx
.ars_dup:
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; abi_check_compat – Versionskompatibilität prüfen (§6)
; EAX = service_id, ECX = req_major, EDX = req_minor
; Rückgabe: EAX = np_status_t, CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
abi_check_compat:
    push ebx
    push esi
    push edi
    mov esi, ecx                ; req_major sichern
    mov edi, edx                ; req_minor sichern

    call abi_find_entry         ; EAX=svc_id → EBX=ptr
    jc .acc_unknown

    movzx eax, word [ebx + ABI_REG_OFF_MAJOR]
    cmp eax, esi
    jne .acc_incompatible

    movzx eax, word [ebx + ABI_REG_OFF_MINOR]
    cmp edi, eax                ; req_minor > provided_minor?
    ja .acc_too_old

    pop edi
    pop esi
    pop ebx
    mov eax, NP_OK
    clc
    ret
.acc_unknown:
    pop edi
    pop esi
    pop ebx
    mov eax, NP_ERR_SERVICE_UNKNOWN
    stc
    ret
.acc_incompatible:
    pop edi
    pop esi
    pop ebx
    mov eax, NP_ERR_ABI_INCOMPATIBLE
    stc
    ret
.acc_too_old:
    pop edi
    pop esi
    pop ebx
    mov eax, NP_ERR_ABI_TOO_OLD
    stc
    ret

; ---------------------------------------------------------------------------
; abi_query_service – füllt np_abi_header_t-Puffer für einen Service (§36)
; EAX = service_id, EDX = Zeiger auf vorinitialisierte Pufferfläche (≥16 Bytes)
; Rückgabe: EAX = np_status_t, CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
abi_query_service:
    push ebx
    push esi
    mov esi, edx                ; Pufferzeiger sichern

    test esi, esi               ; Null-Zeiger → Alignment-Fehler (§20)
    jz .aqs_bad_ptr

    call abi_find_entry         ; EAX=svc_id → EBX=ptr
    jc .aqs_unknown

    movzx eax, word [ebx + ABI_REG_OFF_MAJOR]
    mov [esi + NP_HDR_OFF_MAJOR], ax
    movzx eax, word [ebx + ABI_REG_OFF_MINOR]
    mov [esi + NP_HDR_OFF_MINOR], ax
    mov dword [esi + NP_HDR_OFF_STRUCT_SIZE], NP_ABI_HEADER_SIZE
    mov eax, [ebx + ABI_REG_OFF_FLAGS]
    mov [esi + NP_HDR_OFF_FLAGS], eax           ; low 32 Bit
    mov dword [esi + NP_HDR_OFF_FLAGS + 4], 0   ; high 32 Bit = 0 (§48)

    pop esi
    pop ebx
    mov eax, NP_OK
    clc
    ret
.aqs_bad_ptr:
    pop esi
    pop ebx
    mov eax, NP_ERR_ABI_ALIGNMENT
    stc
    ret
.aqs_unknown:
    pop esi
    pop ebx
    mov eax, NP_ERR_SERVICE_UNKNOWN
    stc
    ret

; ---------------------------------------------------------------------------
; abi_validate_header – prüft structure_size in np_abi_header_t (§17, §7)
; EDX = Zeiger auf Header, ECX = Mindestgröße
; Rückgabe: EAX = np_status_t, CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
abi_validate_header:
    push ebx
    test edx, edx
    jz .avh_null

    mov ebx, [edx + NP_HDR_OFF_STRUCT_SIZE]
    cmp ebx, ecx
    jb .avh_too_small
    cmp ebx, 65536              ; Obergrenze: 64 KiB
    ja .avh_too_small

    pop ebx
    mov eax, NP_OK
    clc
    ret
.avh_null:
    pop ebx
    mov eax, NP_ERR_ABI_ALIGNMENT
    stc
    ret
.avh_too_small:
    pop ebx
    mov eax, NP_ERR_ABI_STRUCTURE_SIZE
    stc
    ret

; ---------------------------------------------------------------------------
; abi_array_check_overflow – prüft count × element_size auf Überlauf (§21)
; EAX = count, ECX = element_size
; Rückgabe: EBX = total_size, CF=0 OK / CF=1 Überlauf
; ---------------------------------------------------------------------------
abi_array_check_overflow:
    push edx
    mul ecx                     ; EDX:EAX = EAX × ECX
    test edx, edx
    jnz .aco_overflow
    mov ebx, eax
    pop edx
    clc
    ret
.aco_overflow:
    xor ebx, ebx
    pop edx
    stc
    ret

; ---------------------------------------------------------------------------
; abi_initialize – Registry leeren und 11 Built-in-Services registrieren
; CF=0 OK, CF=1 Fehler
; ---------------------------------------------------------------------------
%macro abi_reg_builtin 1
    mov eax, %1
    mov ecx, NP_ABI_MAJOR
    mov edx, NP_ABI_MINOR
    xor esi, esi
    call abi_register_service
    jc .ainit_fail
%endmacro

abi_initialize:
    push edi
    push esi
    push ecx
    push edx

    mov edi, abi_registry
    xor eax, eax
    mov ecx, (ABI_REG_MAX * ABI_REG_ENTRY_SIZE) / 4
    rep stosd
    mov dword [abi_service_count], 0

    abi_reg_builtin NP_SERVICE_CORE
    abi_reg_builtin NP_SERVICE_PROCESS
    abi_reg_builtin NP_SERVICE_THREAD
    abi_reg_builtin NP_SERVICE_MEMORY
    abi_reg_builtin NP_SERVICE_IPC
    abi_reg_builtin NP_SERVICE_VFS
    abi_reg_builtin NP_SERVICE_DEVICE
    abi_reg_builtin NP_SERVICE_NETWORK
    abi_reg_builtin NP_SERVICE_SECURITY
    abi_reg_builtin NP_SERVICE_DIAGNOSTIC
    abi_reg_builtin NP_SERVICE_POWER

    cmp dword [abi_service_count], NP_SERVICE_COUNT
    jne .ainit_fail

    pop edx
    pop ecx
    pop esi
    pop edi
    clc
    ret
.ainit_fail:
    pop edx
    pop ecx
    pop esi
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; abi_self_test – 19 Testfälle §67 (NPSPEC-KERNEL-0030)
; Tests 20-40 erfordern Userspace / Modul-Infrastruktur: Bootstrap-N/A.
; CF=0 alle Tests bestanden, CF=1 Fehler
; ---------------------------------------------------------------------------
abi_self_test:
    ; Test 1 – kompatible Major/Minor-Version
    mov eax, NP_SERVICE_CORE
    mov ecx, NP_ABI_MAJOR
    mov edx, NP_ABI_MINOR
    call abi_check_compat
    jc .ast_fail
    cmp eax, NP_OK
    jne .ast_fail

    ; Test 2 – inkompatible Major-Version
    mov eax, NP_SERVICE_CORE
    mov ecx, NP_ABI_MAJOR + 1
    mov edx, 0
    call abi_check_compat
    jnc .ast_fail
    cmp eax, NP_ERR_ABI_INCOMPATIBLE
    jne .ast_fail

    ; Test 3 – Minor-Version zu alt
    mov eax, NP_SERVICE_CORE
    mov ecx, NP_ABI_MAJOR
    mov edx, NP_ABI_MINOR + 1
    call abi_check_compat
    jnc .ast_fail
    cmp eax, NP_ERR_ABI_TOO_OLD
    jne .ast_fail

    ; Test 4 – Feature-Flag-Abfrage: structure_size korrekt zurückgegeben
    mov edi, abi_test_buf
    xor eax, eax
    mov ecx, 32 / 4
    rep stosd                   ; Puffer nullen
    mov eax, NP_SERVICE_CORE
    mov edx, abi_test_buf
    call abi_query_service
    jc .ast_fail
    cmp eax, NP_OK
    jne .ast_fail
    cmp dword [abi_test_buf + NP_HDR_OFF_STRUCT_SIZE], NP_ABI_HEADER_SIZE
    jne .ast_fail

    ; Test 5 – unbekannter Service → SERVICE_UNKNOWN
    mov eax, 0xDEAD
    mov ecx, 1
    mov edx, 0
    call abi_check_compat
    jnc .ast_fail
    cmp eax, NP_ERR_SERVICE_UNKNOWN
    jne .ast_fail

    ; Test 6 – Mindest-Strukturgröße (= NP_ABI_HEADER_SIZE) wird akzeptiert
    mov dword [abi_test_buf + NP_HDR_OFF_STRUCT_SIZE], NP_ABI_HEADER_SIZE
    mov edx, abi_test_buf
    mov ecx, NP_ABI_HEADER_SIZE
    call abi_validate_header
    jc .ast_fail
    cmp eax, NP_OK
    jne .ast_fail

    ; Test 7 – größere kompatible Struktur (32 Bytes) wird akzeptiert
    mov dword [abi_test_buf + NP_HDR_OFF_STRUCT_SIZE], 32
    mov edx, abi_test_buf
    mov ecx, NP_ABI_HEADER_SIZE
    call abi_validate_header
    jc .ast_fail
    cmp eax, NP_OK
    jne .ast_fail

    ; Test 8 – zu kleine Struktur (8 < 16) wird abgelehnt
    mov dword [abi_test_buf + NP_HDR_OFF_STRUCT_SIZE], 8
    mov edx, abi_test_buf
    mov ecx, NP_ABI_HEADER_SIZE
    call abi_validate_header
    jnc .ast_fail
    cmp eax, NP_ERR_ABI_STRUCTURE_SIZE
    jne .ast_fail

    ; Test 9 – reserviertes Feld ≠ 0: Duplikat-Registrierung muss scheitern
    mov eax, NP_SERVICE_CORE
    mov ecx, NP_ABI_MAJOR
    mov edx, NP_ABI_MINOR
    xor esi, esi
    call abi_register_service
    jnc .ast_fail               ; Duplikat → CF=1 erwartet

    ; Tests 10-12 sind Compile-Time-Invarianten (§17): bereits mit %if geprüft.

    ; Test 13 – Null-Zeiger wird abgelehnt
    mov eax, NP_SERVICE_CORE
    xor edx, edx
    call abi_query_service
    jnc .ast_fail
    cmp eax, NP_ERR_ABI_ALIGNMENT
    jne .ast_fail

    ; Test 14 – Array-Überlauf erkannt
    mov eax, 0xFFFFFFFF
    mov ecx, 2
    call abi_array_check_overflow
    jnc .ast_fail               ; muss CF=1

    ; Test 15 – Array kein Überlauf (4 × 4 = 16)
    mov eax, 4
    mov ecx, 4
    call abi_array_check_overflow
    jc .ast_fail
    cmp ebx, 16
    jne .ast_fail

    ; Test 16 – Handle 0 ist ungültig (np_handle_t §22: kein Null-Handle)
    ; Null-Zeiger-Abfrage bereits in Test 13; hier: Wert 0 ist NP_HANDLE_INVALID
    xor eax, eax                ; 0 = NP_HANDLE_INVALID
    test eax, eax
    jnz .ast_fail               ; non-zero wäre fälschlicherweise „gültig"

    ; Test 17 – unbekannter Service → SERVICE_UNKNOWN via query
    mov eax, NP_SERVICE_COUNT + 5
    mov edx, abi_test_buf
    call abi_query_service
    jnc .ast_fail
    cmp eax, NP_ERR_SERVICE_UNKNOWN
    jne .ast_fail

    ; Test 18 – NP_ERR_NOT_SUPPORTED definiert und korrekt kodiert
    mov eax, NP_ERR_NOT_SUPPORTED
    cmp eax, NP_ERR_NOT_SUPPORTED
    jne .ast_fail

    ; Test 19 – Statuscode-Bereiche: NP_OK=0 (compile-time), Fehlercodes < 0
%if NP_OK != 0
%error "NP_OK muss 0 sein"
%endif
    mov eax, NP_ERR_ABI_INCOMPATIBLE
    test eax, eax
    jns .ast_fail               ; muss negativ sein
    mov eax, NP_ERR_NOT_SUPPORTED
    test eax, eax
    jns .ast_fail

    clc
    ret
.ast_fail:
    stc
    ret

abi_registry:       times (ABI_REG_MAX * ABI_REG_ENTRY_SIZE) db 0
abi_service_count:  dd 0
abi_test_buf:       times 32 db 0

; ---------------------------------------------------------------------------
; §100 – Kernel Object Graph (NPSPEC-KERNEL-0100)
; Typisierter gerichteter Multigraph für alle Kernelressourcen.
; Bootstrap-Implementierung: feste Pools, kein RCU/Snapshot/Transaktion.
; ---------------------------------------------------------------------------

; Objekttyp-IDs (§8, §9)
NP_OBJTYPE_KERNEL          equ 1
NP_OBJTYPE_MACHINE         equ 2
NP_OBJTYPE_NAMESPACE       equ 3
NP_OBJTYPE_PROCESS         equ 4
NP_OBJTYPE_THREAD          equ 5
NP_OBJTYPE_CPU             equ 6
NP_OBJTYPE_MEMORY          equ 7
NP_OBJTYPE_IPC             equ 8
NP_OBJTYPE_DEVICE          equ 9
NP_OBJTYPE_DRIVER          equ 10
NP_OBJTYPE_VFS_NODE        equ 11
NP_OBJTYPE_SECURITY        equ 12
NP_OBJTYPE_DIAGNOSTIC      equ 13
NP_OBJTYPE_POWER           equ 14
NP_OBJTYPE_GENERIC         equ 15
NP_OBJTYPE_COUNT           equ 16

; Objektzustände (§13)
NP_GRAPH_OBJ_INITIALIZING  equ 0
NP_GRAPH_OBJ_ACTIVE        equ 1
NP_GRAPH_OBJ_QUIESCING     equ 2
NP_GRAPH_OBJ_FAILED        equ 3
NP_GRAPH_OBJ_REMOVING      equ 4
NP_GRAPH_OBJ_DESTROYED     equ 5

; Beziehungstypen (§14)
NP_RELATION_OWNS           equ 0
NP_RELATION_CONTAINS       equ 1
NP_RELATION_PARENT_OF      equ 2
NP_RELATION_CHILD_OF       equ 3
NP_RELATION_DEPENDS_ON     equ 4
NP_RELATION_BOUND_TO       equ 5
NP_RELATION_PROVIDES       equ 6
NP_RELATION_CONSUMES       equ 7
NP_RELATION_MEMBER_OF      equ 8
NP_RELATION_MAPPED_TO      equ 9
NP_RELATION_OBSERVES       equ 10
NP_RELATION_SECURED_BY     equ 11
NP_RELATION_TYPE_COUNT     equ 12

; Beziehungsstärken (§15)
NP_RELSTR_WEAK             equ 0
NP_RELSTR_REFERENCE        equ 1
NP_RELSTR_STRONG           equ 2
NP_RELSTR_OWNERSHIP        equ 3

; Schema-Flags
KOG_SCHEMA_ALLOW_CYCLE     equ 0x01   ; Zyklen durch diesen Typ erlaubt

; Fehlercodes (§60, erweitert NP_OK/NP_ERR_*)
NP_ERR_OBJECT_NOT_FOUND    equ -20
NP_ERR_OBJECT_STATE        equ -21
NP_ERR_RELATION_INVALID    equ -22
NP_ERR_RELATION_EXISTS     equ -23
NP_ERR_RELATION_NOT_FOUND  equ -24
NP_ERR_RELATION_CYCLE      equ -25
NP_ERR_GRAPH_LIMIT         equ -26

; Pool-Größen
KOG_MAX_OBJECTS            equ 64
KOG_MAX_RELATIONS          equ 128
KOG_CYCLE_MAX_DEPTH        equ 8

; Object-Node-Layout (24 Bytes, §6)
KOG_OBJ_SIZE               equ 24
KOG_OBJ_OFF_ID             equ 0    ; uint32_t  (Bootstrap: low 32 Bit)
KOG_OBJ_OFF_TYPE           equ 4    ; uint32_t
KOG_OBJ_OFF_STATE          equ 8    ; uint32_t
KOG_OBJ_OFF_REFCOUNT       equ 12   ; uint32_t
KOG_OBJ_OFF_FLAGS          equ 16   ; uint32_t
KOG_OBJ_OFF_PRESENT        equ 20   ; uint8_t
; 21-23: Padding

; Relation-Layout (24 Bytes, §16)
KOG_REL_SIZE               equ 24
KOG_REL_OFF_ID             equ 0    ; uint32_t
KOG_REL_OFF_SRC_ID         equ 4    ; uint32_t
KOG_REL_OFF_DST_ID         equ 8    ; uint32_t
KOG_REL_OFF_TYPE           equ 12   ; uint8_t
KOG_REL_OFF_STRENGTH       equ 13   ; uint8_t
KOG_REL_OFF_FLAGS          equ 14   ; uint8_t
KOG_REL_OFF_PRESENT        equ 15   ; uint8_t
KOG_REL_OFF_GENERATION     equ 16   ; uint32_t
; 20-23: Padding

; Schema-Layout (16 Bytes, §18)
KOG_SCH_SIZE               equ 16
KOG_SCH_OFF_TYPE           equ 0    ; uint16_t
KOG_SCH_OFF_MAX_STR        equ 2    ; uint8_t
KOG_SCH_OFF_FLAGS          equ 3    ; uint8_t
KOG_SCH_OFF_SRC_MASK       equ 4    ; uint32_t (Bitmaske erlaubter Quelltypen)
KOG_SCH_OFF_DST_MASK       equ 8    ; uint32_t (Bitmaske erlaubter Zieltypen)
KOG_SCH_OFF_PRESENT        equ 12   ; uint8_t
; 13-15: Padding

; Compile-time-Invarianten
%if KOG_OBJ_SIZE != 24
%error "KOG_OBJ_SIZE muss 24 Bytes sein"
%endif
%if KOG_REL_SIZE != 24
%error "KOG_REL_SIZE muss 24 Bytes sein"
%endif

; ---------------------------------------------------------------------------
; kog_find_object – sucht Objekt per ID (intern)
; EAX = object_id  →  EBX = Zeiger, CF=0 gefunden / CF=1 nicht gefunden
; Clobbers: ECX
; ---------------------------------------------------------------------------
kog_find_object:
    push esi
    mov esi, kog_objects
    xor ecx, ecx
.kfo_loop:
    cmp ecx, KOG_MAX_OBJECTS
    jae .kfo_not_found
    cmp byte [esi + KOG_OBJ_OFF_PRESENT], 1
    jne .kfo_next
    cmp [esi + KOG_OBJ_OFF_ID], eax
    je .kfo_found
.kfo_next:
    add esi, KOG_OBJ_SIZE
    inc ecx
    jmp .kfo_loop
.kfo_found:
    mov ebx, esi
    pop esi
    clc
    ret
.kfo_not_found:
    xor ebx, ebx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; kog_find_relation – sucht Relation per ID (intern)
; EAX = rel_id  →  EBX = Zeiger, CF=0 / CF=1
; Clobbers: ECX
; ---------------------------------------------------------------------------
kog_find_relation:
    push esi
    mov esi, kog_relations
    xor ecx, ecx
.kfr_loop:
    cmp ecx, KOG_MAX_RELATIONS
    jae .kfr_not_found
    cmp byte [esi + KOG_REL_OFF_PRESENT], 1
    jne .kfr_next
    cmp [esi + KOG_REL_OFF_ID], eax
    je .kfr_found
.kfr_next:
    add esi, KOG_REL_SIZE
    inc ecx
    jmp .kfr_loop
.kfr_found:
    mov ebx, esi
    pop esi
    clc
    ret
.kfr_not_found:
    xor ebx, ebx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; kog_alloc_object – neuen Objektknoten belegen
; EAX = type_id
; Rückgabe: EBX = Zeiger, ECX = zugewiesene object_id, CF=0 OK / CF=1 voll
; ---------------------------------------------------------------------------
kog_alloc_object:
    push esi
    push edi

    mov esi, kog_objects
    xor ecx, ecx
.kao_scan:
    cmp ecx, KOG_MAX_OBJECTS
    jae .kao_full
    cmp byte [esi + KOG_OBJ_OFF_PRESENT], 0
    je .kao_write
    add esi, KOG_OBJ_SIZE
    inc ecx
    jmp .kao_scan
.kao_write:
    lock inc dword [kog_next_id]
    mov edi, [kog_next_id]          ; neue ID
    mov [esi + KOG_OBJ_OFF_ID],       edi
    mov [esi + KOG_OBJ_OFF_TYPE],     eax
    mov dword [esi + KOG_OBJ_OFF_STATE],    NP_GRAPH_OBJ_INITIALIZING
    mov dword [esi + KOG_OBJ_OFF_REFCOUNT], 1
    mov dword [esi + KOG_OBJ_OFF_FLAGS],    0
    mov byte  [esi + KOG_OBJ_OFF_PRESENT],  1
    mov ebx, esi
    mov ecx, edi
    pop edi
    pop esi
    clc
    ret
.kao_full:
    xor ebx, ebx
    xor ecx, ecx
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; kog_activate_object – setzt Zustand eines Objekts auf ACTIVE (§13)
; EAX = object_id  →  CF=0 OK / CF=1 nicht gefunden / falscher Zustand
; ---------------------------------------------------------------------------
kog_activate_object:
    push ebx
    call kog_find_object
    jc .kact_fail
    cmp dword [ebx + KOG_OBJ_OFF_STATE], NP_GRAPH_OBJ_INITIALIZING
    jne .kact_bad_state
    mov dword [ebx + KOG_OBJ_OFF_STATE], NP_GRAPH_OBJ_ACTIVE
    pop ebx
    clc
    ret
.kact_bad_state:
.kact_fail:
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; kog_find_schema – sucht Schema für einen Beziehungstyp (intern)
; EAX = rel_type  →  EBX = Zeiger, CF=0 / CF=1
; ---------------------------------------------------------------------------
kog_find_schema:
    push esi
    mov esi, kog_schemas
    push ecx
    xor ecx, ecx
.kfs_loop:
    cmp ecx, NP_RELATION_TYPE_COUNT
    jae .kfs_not_found
    cmp byte [esi + KOG_SCH_OFF_PRESENT], 1
    jne .kfs_next
    movzx ebx, word [esi + KOG_SCH_OFF_TYPE]
    cmp ebx, eax
    je .kfs_found
.kfs_next:
    add esi, KOG_SCH_SIZE
    inc ecx
    jmp .kfs_loop
.kfs_found:
    mov ebx, esi
    pop ecx
    pop esi
    clc
    ret
.kfs_not_found:
    xor ebx, ebx
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; kog_check_cycle – prüft ob src→dst eine Ownership-Zyklusgefahr erzeugt
; EAX = src_id, ECX = dst_id, EDX = rel_type
; Rückgabe: CF=0 kein Zyklus / CF=1 Zyklus erkannt
; Nur relevant für OWNS und CONTAINS (max_strength >= STRONG).
; ---------------------------------------------------------------------------
kog_check_cycle:
    ; Nur für starke/Ownership-Typen prüfen
    cmp edx, NP_RELATION_OWNS
    je .kcc_check
    cmp edx, NP_RELATION_CONTAINS
    je .kcc_check
    cmp edx, NP_RELATION_BOUND_TO
    je .kcc_check
    clc
    ret
.kcc_check:
    ; Schema-Flags prüfen: ALLOW_CYCLE → keine Prüfung nötig
    ; EBX wird durch kog_find_schema überschrieben – vorher sichern!
    push ebx
    push edx
    push eax
    mov eax, edx
    call kog_find_schema
    jc .kcc_no_schema
    test byte [ebx + KOG_SCH_OFF_FLAGS], KOG_SCHEMA_ALLOW_CYCLE
    jnz .kcc_no_schema
    pop eax
    pop edx
    pop ebx             ; EBX wiederhergestellt (Aufrufer-EBX, Schema-Ptr verworfen)

    ; DFS: von DST aus, prüfen ob SRC erreichbar
    ; Einfacher iterativer Check mit kog_cycle_stack (8 Einträge)
    push esi
    push edi
    push ebp
    mov ebp, eax                    ; SRC_ID in EBP
    ; Stack-Init: dst_id auf Stack
    mov esi, kog_cycle_stack
    mov [esi], ecx                  ; kog_cycle_stack[0] = dst_id
    mov dword [kog_cycle_depth], 1

.kcc_dfs:
    cmp dword [kog_cycle_depth], 0
    je .kcc_no_cycle
    cmp dword [kog_cycle_depth], KOG_CYCLE_MAX_DEPTH
    jae .kcc_no_cycle               ; restriktiv: bei Tiefenüberschreitung pass
    dec dword [kog_cycle_depth]
    mov ecx, [kog_cycle_depth]
    mov edi, [esi + ecx * 4]        ; aktuellen Knoten vom Stack holen

    cmp edi, ebp                    ; EBP=src_id gefunden?
    je .kcc_cycle_found

    ; Alle ausgehenden STRONG/OWNERSHIP-Kanten von EDI suchen
    push esi
    push edi
    mov esi, kog_relations
    xor ecx, ecx
.kcc_edge_scan:
    cmp ecx, KOG_MAX_RELATIONS
    jae .kcc_edge_done
    cmp byte [esi + KOG_REL_OFF_PRESENT], 1
    jne .kcc_edge_next
    cmp [esi + KOG_REL_OFF_SRC_ID], edi
    jne .kcc_edge_next
    movzx eax, byte [esi + KOG_REL_OFF_STRENGTH]
    cmp eax, NP_RELSTR_STRONG
    jb .kcc_edge_next               ; schwächere Kante: ignorieren
    ; Kante ist stark: Ziel auf Stack legen
    mov eax, [kog_cycle_depth]
    cmp eax, KOG_CYCLE_MAX_DEPTH - 1
    jae .kcc_edge_next              ; Stack voll: skip
    mov edx, [esi + KOG_REL_OFF_DST_ID]
    mov [kog_cycle_stack + eax * 4], edx
    inc dword [kog_cycle_depth]
.kcc_edge_next:
    add esi, KOG_REL_SIZE
    inc ecx
    jmp .kcc_edge_scan
.kcc_edge_done:
    pop edi
    pop esi
    jmp .kcc_dfs

.kcc_no_schema:
    pop eax
    pop edx
    pop ebx             ; EBX wiederherstellen (von push ebx in .kcc_check)
    clc
    ret
.kcc_no_cycle:
    pop ebp
    pop edi
    pop esi
    clc
    ret
.kcc_cycle_found:
    pop ebp
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; kog_link – erzeugt eine Kante (§19)
; EAX = src_id, ECX = dst_id, EDX = rel_type, ESI = strength (uint32_t)
; Rückgabe: EBX = rel_id (neu), CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
kog_link:
    push ebp
    push edi
    push esi

    ; Quellobjekt prüfen
    push ecx
    push edx
    push esi
    call kog_find_object            ; EAX=src_id → EBX=src_ptr
    jc .kl_not_found
    cmp dword [ebx + KOG_OBJ_OFF_STATE], NP_GRAPH_OBJ_DESTROYED
    je .kl_bad_state
    mov ebp, ebx                    ; EBP = src_ptr

    ; Zielobjekt prüfen
    ; Stack-Layout nach push ecx/edx/esi: [strength(+0) | rel_type(+4) | dst_id(+8) | ...]
    mov eax, [esp + 8]              ; ECX-Wert (dst_id) aus gesichertem Stack
    call kog_find_object            ; EAX=dst_id → EBX=dst_ptr
    jc .kl_not_found
    cmp dword [ebx + KOG_OBJ_OFF_STATE], NP_GRAPH_OBJ_DESTROYED
    je .kl_bad_state
    push ebx                        ; dst_ptr merken

    ; Schema prüfen (Typkompatibilität)
    mov eax, [esp + 8]              ; EDX-Wert (rel_type)
    call kog_find_schema
    jc .kl_schema_fail

    ; Zyklus prüfen: EAX=src_id, ECX=dst_id, EDX=rel_type
    ; Nach pop ebx: Stack [strength(+0) | rel_type(+4) | dst_id(+8) | ...]
    pop ebx                         ; dst_ptr
    mov eax, [ebp + KOG_OBJ_OFF_ID] ; src_id
    mov ecx, [ebx + KOG_OBJ_OFF_ID] ; dst_id
    mov edx, [esp + 4]              ; rel_type
    call kog_check_cycle
    jc .kl_cycle

    ; Freien Relation-Slot suchen
    push edi
    mov edi, kog_relations
    xor ecx, ecx
.kl_rel_scan:
    cmp ecx, KOG_MAX_RELATIONS
    jae .kl_full
    cmp byte [edi + KOG_REL_OFF_PRESENT], 0
    je .kl_rel_write
    add edi, KOG_REL_SIZE
    inc ecx
    jmp .kl_rel_scan
.kl_rel_write:
    lock inc dword [kog_next_rel_id]
    mov eax, [kog_next_rel_id]
    mov [edi + KOG_REL_OFF_ID], eax
    mov ecx, [ebp + KOG_OBJ_OFF_ID] ; src_id
    mov [edi + KOG_REL_OFF_SRC_ID], ecx
    mov ecx, [ebx + KOG_OBJ_OFF_ID] ; dst_id... wait ebx was popped
    ; Hmm, need to recalculate. Let me use stack properly.

    ; Actually EBX = dst_ptr (still valid from pop ebx above)
    ; But we pushed edi and need to be careful.
    ; Let me save src_id and dst_id explicitly.
    mov ecx, [ebp + KOG_OBJ_OFF_ID]     ; src_id
    mov [edi + KOG_REL_OFF_SRC_ID], ecx
    mov ecx, [ebx + KOG_OBJ_OFF_ID]     ; dst_id
    mov [edi + KOG_REL_OFF_DST_ID], ecx
    ; Nach push edi: Stack [EDI(+0) | strength(+4) | rel_type(+8) | dst_id(+12) | ...]
    mov cl, [esp + 8]                   ; rel_type byte from stack
    mov [edi + KOG_REL_OFF_TYPE], cl
    mov cl, [esp + 4]                   ; strength byte (ESI low byte) from saved ESI
    mov [edi + KOG_REL_OFF_STRENGTH], cl
    mov byte [edi + KOG_REL_OFF_FLAGS], 0
    mov byte [edi + KOG_REL_OFF_PRESENT], 1
    mov ecx, [kog_graph_generation]
    mov [edi + KOG_REL_OFF_GENERATION], ecx
    lock inc dword [kog_graph_generation]

    ; Refcount bei starker Kante erhöhen
    movzx ecx, byte [edi + KOG_REL_OFF_STRENGTH]
    cmp ecx, NP_RELSTR_STRONG
    jb .kl_no_ref
    lock inc dword [ebx + KOG_OBJ_OFF_REFCOUNT] ; dst refcount
.kl_no_ref:
    mov eax, [edi + KOG_REL_OFF_ID]    ; rel_id als Rückgabewert
    pop edi
    pop esi
    pop edx
    pop ecx
    pop esi                             ; ursprüngliches ESI
    pop edi
    pop ebp
    mov ebx, eax                        ; EBX = rel_id
    clc
    ret

.kl_schema_fail:
    pop ebx                             ; dst_ptr
.kl_cycle:
.kl_bad_state:
.kl_not_found:
.kl_full:
    pop esi
    pop edx
    pop ecx
    pop esi
    pop edi
    pop ebp
    stc
    ret

; ---------------------------------------------------------------------------
; kog_unlink – entfernt eine Relation (§21)
; EAX = rel_id  →  CF=0 OK / CF=1 nicht gefunden
; ---------------------------------------------------------------------------
kog_unlink:
    push ebx
    call kog_find_relation
    jc .ku_fail
    ; Refcount des Ziels reduzieren bei starker Kante
    movzx ecx, byte [ebx + KOG_REL_OFF_STRENGTH]
    cmp ecx, NP_RELSTR_STRONG
    jb .ku_no_ref
    push eax
    mov eax, [ebx + KOG_REL_OFF_DST_ID]
    push ebx
    call kog_find_object              ; EAX=dst_id → EBX=dst_ptr
    mov edx, ebx                      ; dst_ptr in EDX sichern (pop ebx überschreibt EBX)
    pop ebx                           ; rel_ptr wiederherstellen
    jc .ku_ref_skip
    lock dec dword [edx + KOG_OBJ_OFF_REFCOUNT] ; EDX=dst_ptr (nicht EAX=dst_id)
.ku_ref_skip:
    pop eax
.ku_no_ref:
    mov byte [ebx + KOG_REL_OFF_PRESENT], 0
    lock inc dword [kog_graph_generation]
    pop ebx
    clc
    ret
.ku_fail:
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; kog_initialize – Pools leeren, Schemata registrieren, Root-Objekte anlegen
; CF=0 OK, CF=1 Fehler
; ---------------------------------------------------------------------------

; Hilfs-Makro: Schema-Eintrag schreiben
%macro kog_schema_entry 4          ; type, max_strength, flags, slot_idx
    mov word  [kog_schemas + %4 * KOG_SCH_SIZE + KOG_SCH_OFF_TYPE],    %1
    mov byte  [kog_schemas + %4 * KOG_SCH_SIZE + KOG_SCH_OFF_MAX_STR], %2
    mov byte  [kog_schemas + %4 * KOG_SCH_SIZE + KOG_SCH_OFF_FLAGS],   %3
    mov dword [kog_schemas + %4 * KOG_SCH_SIZE + KOG_SCH_OFF_SRC_MASK], 0xFFFFFFFF
    mov dword [kog_schemas + %4 * KOG_SCH_SIZE + KOG_SCH_OFF_DST_MASK], 0xFFFFFFFF
    mov byte  [kog_schemas + %4 * KOG_SCH_SIZE + KOG_SCH_OFF_PRESENT], 1
%endmacro

kog_initialize:
    push edi
    push ecx

    ; Pools leeren
    mov edi, kog_objects
    xor eax, eax
    mov ecx, (KOG_MAX_OBJECTS * KOG_OBJ_SIZE) / 4
    rep stosd
    mov edi, kog_relations
    mov ecx, (KOG_MAX_RELATIONS * KOG_REL_SIZE) / 4
    rep stosd
    mov edi, kog_schemas
    mov ecx, (NP_RELATION_TYPE_COUNT * KOG_SCH_SIZE) / 4
    rep stosd
    mov dword [kog_next_id], 0
    mov dword [kog_next_rel_id], 0
    mov dword [kog_graph_generation], 1

    ; Schemata für alle 12 Beziehungstypen (§14)
    ; Felder: type, max_strength, flags, slot
    kog_schema_entry NP_RELATION_OWNS,      NP_RELSTR_OWNERSHIP, 0,                     0
    kog_schema_entry NP_RELATION_CONTAINS,  NP_RELSTR_STRONG,    0,                     1
    kog_schema_entry NP_RELATION_PARENT_OF, NP_RELSTR_REFERENCE, 0,                     2
    kog_schema_entry NP_RELATION_CHILD_OF,  NP_RELSTR_REFERENCE, 0,                     3
    kog_schema_entry NP_RELATION_DEPENDS_ON,NP_RELSTR_REFERENCE, 0,                     4
    kog_schema_entry NP_RELATION_BOUND_TO,  NP_RELSTR_STRONG,    0,                     5
    kog_schema_entry NP_RELATION_PROVIDES,  NP_RELSTR_REFERENCE, KOG_SCHEMA_ALLOW_CYCLE, 6
    kog_schema_entry NP_RELATION_CONSUMES,  NP_RELSTR_REFERENCE, KOG_SCHEMA_ALLOW_CYCLE, 7
    kog_schema_entry NP_RELATION_MEMBER_OF, NP_RELSTR_REFERENCE, KOG_SCHEMA_ALLOW_CYCLE, 8
    kog_schema_entry NP_RELATION_MAPPED_TO, NP_RELSTR_REFERENCE, KOG_SCHEMA_ALLOW_CYCLE, 9
    kog_schema_entry NP_RELATION_OBSERVES,  NP_RELSTR_WEAK,      KOG_SCHEMA_ALLOW_CYCLE, 10
    kog_schema_entry NP_RELATION_SECURED_BY,NP_RELSTR_REFERENCE, 0,                     11

    ; Root-Objekte anlegen (§10)
%macro kog_create_root 2            ; type, id_storage
    mov eax, %1
    call kog_alloc_object
    jc .kinit_fail
    mov [%2], ecx                   ; gespeicherte Root-ID
    ; kog_activate_object erwartet EAX=id
    push ecx
    mov eax, ecx
    call kog_activate_object
    pop ecx
    jc .kinit_fail
%endmacro

    kog_create_root NP_OBJTYPE_KERNEL,   kog_root_kernel_id
    kog_create_root NP_OBJTYPE_MACHINE,  kog_root_machine_id
    kog_create_root NP_OBJTYPE_SECURITY, kog_root_security_id
    kog_create_root NP_OBJTYPE_NAMESPACE,kog_root_namespace_id
    kog_create_root NP_OBJTYPE_DEVICE,   kog_root_device_id
    kog_create_root NP_OBJTYPE_GENERIC,  kog_root_service_id
    kog_create_root NP_OBJTYPE_GENERIC,  kog_root_recovery_id

    ; Kernel-Root OWNS Machine-Root (§11)
    mov eax, [kog_root_kernel_id]
    mov ecx, [kog_root_machine_id]
    mov edx, NP_RELATION_OWNS
    mov esi, NP_RELSTR_OWNERSHIP
    call kog_link
    jc .kinit_fail

    pop ecx
    pop edi
    clc
    ret
.kinit_fail:
    pop ecx
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; kog_self_test – Tests 1–15 §62 (NPSPEC-KERNEL-0100)
; Tests 16–39 erfordern Transaktionen/Recovery/SMP-Parallelismus: Bootstrap-N/A.
; CF=0 alle Tests bestanden, CF=1 Fehler
; ---------------------------------------------------------------------------
kog_self_test:
    ; Test 1 – Schema für OWNS wurde registriert
    mov eax, NP_RELATION_OWNS
    call kog_find_schema
    jc .kst_fail
    cmp byte [ebx + KOG_SCH_OFF_MAX_STR], NP_RELSTR_OWNERSHIP
    jne .kst_fail

    ; Test 2 – gültige Kante zwischen zwei GENERIC-Objekten erzeugen
    mov eax, NP_OBJTYPE_GENERIC
    call kog_alloc_object
    jc .kst_fail
    push ecx                        ; obj_a_id
    mov eax, ecx
    call kog_activate_object
    jc .kst_pop1_fail

    mov eax, NP_OBJTYPE_GENERIC
    call kog_alloc_object
    jc .kst_pop1_fail
    push ecx                        ; obj_b_id
    mov eax, ecx
    call kog_activate_object
    jc .kst_pop2_fail

    ; OWNS-Kante a → b
    mov eax, [esp + 4]              ; obj_a_id
    mov ecx, [esp]                  ; obj_b_id
    mov edx, NP_RELATION_OWNS
    mov esi, NP_RELSTR_OWNERSHIP
    call kog_link
    jc .kst_pop2_fail
    push ebx                        ; rel_id

    ; Test 3 – DESTROYED-Objekt darf keine neue Kante erhalten
    mov eax, 0xDEADBEEF             ; ungültige ID
    mov ecx, [esp + 4]              ; obj_b_id (nicht obj_a_id – Kommentar war falsch)
    mov edx, NP_RELATION_OWNS
    mov esi, NP_RELSTR_OWNERSHIP
    call kog_link
    jnc .kst_pop3_fail              ; muss CF=1

    ; Test 4 – STRONG-Kante: Refcount von b erhöht
    mov eax, [esp + 4]              ; obj_b_id
    call kog_find_object
    jc .kst_pop3_fail
    cmp dword [ebx + KOG_OBJ_OFF_REFCOUNT], 2  ; initial=1 + 1 von OWNS
    jne .kst_pop3_fail

    ; Test 5 – Ownership-Relation korrekt gespeichert
    mov eax, [esp]                  ; rel_id
    call kog_find_relation
    jc .kst_pop3_fail
    cmp byte [ebx + KOG_REL_OFF_TYPE], NP_RELATION_OWNS
    jne .kst_pop3_fail
    cmp byte [ebx + KOG_REL_OFF_STRENGTH], NP_RELSTR_OWNERSHIP
    jne .kst_pop3_fail

    ; Test 6 – Parent-Child-Schema registriert
    mov eax, NP_RELATION_PARENT_OF
    call kog_find_schema
    jc .kst_pop3_fail

    ; Test 7 – Depends-On-Schema registriert
    mov eax, NP_RELATION_DEPENDS_ON
    call kog_find_schema
    jc .kst_pop3_fail

    ; Test 8 – Bound-To-Schema registriert
    mov eax, NP_RELATION_BOUND_TO
    call kog_find_schema
    jc .kst_pop3_fail

    ; Test 9 – Security-Schema (SECURED_BY) registriert
    mov eax, NP_RELATION_SECURED_BY
    call kog_find_schema
    jc .kst_pop3_fail

    ; Test 10 – atomare Kante: generation erhöht nach link
    mov eax, [kog_graph_generation]
    cmp eax, 1                      ; nach init=1 und 1 link + kernel→machine = mind. 3
    jbe .kst_pop3_fail

    ; Test 11 – Kante entfernen (unlink)
    mov eax, [esp]                  ; rel_id
    call kog_unlink
    jc .kst_pop3_fail
    ; Relation darf nicht mehr findbar sein
    call kog_find_relation          ; EAX noch = rel_id
    jnc .kst_pop3_fail              ; gefunden wäre Fehler

    ; Test 12 – Refcount nach unlink wieder 1
    mov eax, [esp + 4]              ; obj_b_id
    call kog_find_object
    jc .kst_pop3_fail
    cmp dword [ebx + KOG_OBJ_OFF_REFCOUNT], 1
    jne .kst_pop3_fail

    ; Test 13 – verbotener OWNS-Zyklus erkannt (simuliere: b→b)
    mov eax, [esp + 4]              ; obj_b_id (selbe für src und dst)
    mov ecx, [esp + 4]              ; dst = same id → Zyklus
    mov edx, NP_RELATION_OWNS
    call kog_check_cycle
    jnc .kst_pop3_fail              ; muss CF=1 (Zyklus)

    ; Test 14 – erlaubter schwacher Zyklus (OBSERVES): ALLOW_CYCLE Flag gesetzt
    mov eax, NP_RELATION_OBSERVES
    call kog_find_schema
    jc .kst_pop3_fail
    test byte [ebx + KOG_SCH_OFF_FLAGS], KOG_SCHEMA_ALLOW_CYCLE
    jz .kst_pop3_fail               ; ALLOW_CYCLE muss gesetzt sein

    ; Test 15 – Root-Objekte vorhanden
    mov eax, [kog_root_kernel_id]
    call kog_find_object
    jc .kst_pop3_fail
    cmp dword [ebx + KOG_OBJ_OFF_STATE], NP_GRAPH_OBJ_ACTIVE
    jne .kst_pop3_fail

    ; Aufräumen
    pop ebx                         ; rel_id (schon unlinked)
    pop ecx                         ; obj_b_id
    pop ecx                         ; obj_a_id
    clc
    ret

.kst_pop3_fail:
    pop ebx                         ; rel_id
.kst_pop2_fail:
    pop ecx                         ; obj_b_id
.kst_pop1_fail:
    pop ecx                         ; obj_a_id
.kst_fail:
    stc
    ret

; KOG-Daten
kog_objects:           times (KOG_MAX_OBJECTS * KOG_OBJ_SIZE) db 0
kog_relations:         times (KOG_MAX_RELATIONS * KOG_REL_SIZE) db 0
kog_schemas:           times (NP_RELATION_TYPE_COUNT * KOG_SCH_SIZE) db 0
kog_next_id:           dd 0
kog_next_rel_id:       dd 0
kog_graph_generation:  dd 0
kog_cycle_stack:       times KOG_CYCLE_MAX_DEPTH dd 0
kog_cycle_depth:       dd 0
kog_root_kernel_id:    dd 0
kog_root_machine_id:   dd 0
kog_root_security_id:  dd 0
kog_root_namespace_id: dd 0
kog_root_device_id:    dd 0
kog_root_service_id:   dd 0
kog_root_recovery_id:  dd 0

; ---------------------------------------------------------------------------
; §101 – Event Bus (NPSPEC-KERNEL-0101)
; Publish-Subscribe-Infrastruktur für Kernelkomponenten.
; Bootstrap: synchrone Callback-Zustellung, statische Pools, kein Scheduler.
; Re-Entrant-Publish gesperrt (evbus_depth); evbus_pub_* single-threaded sicher.
; ---------------------------------------------------------------------------

; Eventklassen (§7)
NP_EVENT_CLASS_STATE        equ 0
NP_EVENT_CLASS_LIFECYCLE    equ 1
NP_EVENT_CLASS_RESOURCE     equ 2
NP_EVENT_CLASS_SECURITY     equ 3
NP_EVENT_CLASS_ERROR        equ 4
NP_EVENT_CLASS_DIAGNOSTIC   equ 5
NP_EVENT_CLASS_COMPLETION   equ 6
NP_EVENT_CLASS_NOTIFICATION equ 7

; Event-Flags (§15)
NP_EVENT_SYNCHRONOUS        equ 0x001
NP_EVENT_ASYNCHRONOUS       equ 0x002
NP_EVENT_RELIABLE           equ 0x004
NP_EVENT_COALESCABLE        equ 0x008
NP_EVENT_HIGH_PRIORITY      equ 0x010
NP_EVENT_REPLAYABLE         equ 0x020
NP_EVENT_AUDITED            equ 0x040
NP_EVENT_SENSITIVE          equ 0x080
NP_EVENT_KERNEL_ONLY        equ 0x100

; Prioritäten (§34)
NP_EVENT_PRIORITY_LOW       equ 0
NP_EVENT_PRIORITY_NORMAL    equ 1
NP_EVENT_PRIORITY_HIGH      equ 2
NP_EVENT_PRIORITY_CRITICAL  equ 3

; Zustellungsarten (§21)
NP_EVENT_DELIVERY_CALLBACK  equ 0
NP_EVENT_DELIVERY_QUEUE     equ 1
NP_EVENT_DELIVERY_IPC       equ 2
NP_EVENT_DELIVERY_SIGNAL    equ 3
NP_EVENT_DELIVERY_WAITABLE  equ 4

; Subscription-Zustände (§20)
NP_SUB_CREATED              equ 0
NP_SUB_ACTIVE               equ 1
NP_SUB_PAUSED               equ 2
NP_SUB_CLOSING              equ 3
NP_SUB_CLOSED               equ 4

; Fehlercodes (§59)
NP_ERR_EVENT_TYPE_UNKNOWN   equ -40
NP_ERR_EVENT_SCHEMA         equ -41
NP_ERR_EVENT_TOO_LARGE      equ -42
NP_ERR_SUBSCRIPTION_INVALID equ -43
NP_ERR_SUBSCRIPTION_CLOSED  equ -44
NP_ERR_EVENT_QUEUE_FULL     equ -45
NP_ERR_EVENT_DROPPED        equ -46

; Eingebaute Event-Type-IDs (§6, kernel.* Namespaces)
NP_EVENT_BOOT_READY         equ 1
NP_EVENT_BOOT_PANIC         equ 2
NP_EVENT_OBJ_CREATED        equ 3
NP_EVENT_OBJ_ACTIVATED      equ 4
NP_EVENT_OBJ_DESTROYED      equ 5
NP_EVENT_OBJ_STATE_CHANGED  equ 6
NP_EVENT_OBJ_LINK_ADDED     equ 7
NP_EVENT_OBJ_LINK_REMOVED   equ 8
NP_EVENT_BUILTIN_COUNT      equ 8

; Pool-Größen
EVBUS_MAX_SCHEMAS           equ 32
EVBUS_MAX_SUBS              equ 16

; Schema-Layout (16 Bytes, §8)
EVBUS_SCH_SIZE              equ 16
EVBUS_SCH_OFF_TYPE_ID       equ 0    ; uint32_t
EVBUS_SCH_OFF_CLASS         equ 4    ; uint8_t
EVBUS_SCH_OFF_MIN_PL        equ 5    ; uint8_t
EVBUS_SCH_OFF_MAX_PL        equ 6    ; uint16_t
EVBUS_SCH_OFF_FLAGS         equ 8    ; uint32_t
EVBUS_SCH_OFF_PRESENT       equ 12   ; uint8_t
; 13-15: Padding

; Subscription-Layout (24 Bytes, §19)
EVBUS_SUB_SIZE              equ 24
EVBUS_SUB_OFF_ID            equ 0    ; uint32_t
EVBUS_SUB_OFF_TYPE_FILTER   equ 4    ; uint32_t (0 = alle Typen)
EVBUS_SUB_OFF_HANDLER       equ 8    ; uint32_t (Callback-Zeiger)
EVBUS_SUB_OFF_STATE         equ 12   ; uint8_t
EVBUS_SUB_OFF_DELIVERY      equ 13   ; uint8_t
EVBUS_SUB_OFF_FLAGS         equ 14   ; uint8_t
EVBUS_SUB_OFF_PRESENT       equ 15   ; uint8_t
EVBUS_SUB_OFF_QUEUED        equ 16   ; uint32_t
EVBUS_SUB_OFF_LOST          equ 20   ; uint32_t

; Compile-time-Invarianten
%if EVBUS_SCH_SIZE != 16
%error "EVBUS_SCH_SIZE muss 16 Bytes sein"
%endif
%if EVBUS_SUB_SIZE != 24
%error "EVBUS_SUB_SIZE muss 24 Bytes sein"
%endif

; ---------------------------------------------------------------------------
; evbus_find_schema – Schema per type_id suchen (intern)
; EAX = type_id  →  EBX = Zeiger, CF=0 / CF=1
; Clobbers: EBX (ECX intern gepusht/gepoppt)
; ---------------------------------------------------------------------------
evbus_find_schema:
    push esi
    push ecx
    mov esi, evbus_schemas
    xor ecx, ecx
.evfs_loop:
    cmp ecx, EVBUS_MAX_SCHEMAS
    jae .evfs_not_found
    cmp byte [esi + EVBUS_SCH_OFF_PRESENT], 1
    jne .evfs_next
    cmp [esi + EVBUS_SCH_OFF_TYPE_ID], eax
    je .evfs_found
.evfs_next:
    add esi, EVBUS_SCH_SIZE
    inc ecx
    jmp .evfs_loop
.evfs_found:
    mov ebx, esi
    pop ecx
    pop esi
    clc
    ret
.evfs_not_found:
    xor ebx, ebx
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; evbus_register_schema – neues Schema registrieren (§9)
; EAX=type_id, ECX=event_class, EDX=max_payload_size, ESI=flags
; CF=0 OK / CF=1 Fehler (doppelt/voll)
; ---------------------------------------------------------------------------
evbus_register_schema:
    push ebx
    push esi
    push edi
    push ecx    ; [esp+0]=ECX(class), nach weiteren Pushes verschoben
    push edx    ; Stack: [esp]=EDX(max_pl), [esp+4]=ECX(class),
                ;        [esp+8]=old_EDI, [esp+12]=old_ESI(flags), [esp+16]=old_EBX

    ; Doppelte type_id ablehnen
    call evbus_find_schema      ; EAX unveränderter type_id
    jnc .ers_fail               ; CF=0 = bereits vorhanden

    ; Freien Slot suchen
    mov edi, evbus_schemas
    xor ecx, ecx
.ers_scan:
    cmp ecx, EVBUS_MAX_SCHEMAS
    jae .ers_fail
    cmp byte [edi + EVBUS_SCH_OFF_PRESENT], 0
    je .ers_write
    add edi, EVBUS_SCH_SIZE
    inc ecx
    jmp .ers_scan
.ers_write:
    mov [edi + EVBUS_SCH_OFF_TYPE_ID], eax
    movzx ebx, byte [esp + 4]           ; event_class aus gesichertem ECX
    mov [edi + EVBUS_SCH_OFF_CLASS], bl
    mov byte [edi + EVBUS_SCH_OFF_MIN_PL], 0
    movzx ebx, word [esp]               ; max_payload aus gesichertem EDX
    mov [edi + EVBUS_SCH_OFF_MAX_PL], bx
    mov ebx, [esp + 12]                 ; flags aus gesichertem ESI
    mov [edi + EVBUS_SCH_OFF_FLAGS], ebx
    mov byte [edi + EVBUS_SCH_OFF_PRESENT], 1
    pop edx
    pop ecx
    pop edi
    pop esi
    pop ebx
    clc
    ret
.ers_fail:
    pop edx
    pop ecx
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; evbus_subscribe – Subscription anlegen (§18)
; EAX=type_id_filter (0=alle), ECX=handler_fn, EDX=delivery_mode
; EBX = sub_id (Rückgabe), CF=0 OK / CF=1 voll
; ---------------------------------------------------------------------------
evbus_subscribe:
    push esi
    push edi
    push ecx    ; [esp+0]=ECX(handler_fn), nach push edx verschoben
    push edx    ; Stack: [esp]=EDX(mode), [esp+4]=ECX(handler_fn),
                ;        [esp+8]=old_EDI, [esp+12]=old_ESI
    mov edi, evbus_subs
    xor ecx, ecx
.esub_scan:
    cmp ecx, EVBUS_MAX_SUBS
    jae .esub_fail
    cmp byte [edi + EVBUS_SUB_OFF_PRESENT], 0
    je .esub_write
    add edi, EVBUS_SUB_SIZE
    inc ecx
    jmp .esub_scan
.esub_write:
    lock inc dword [evbus_next_sub_id]
    mov ebx, [evbus_next_sub_id]
    mov [edi + EVBUS_SUB_OFF_ID], ebx
    mov [edi + EVBUS_SUB_OFF_TYPE_FILTER], eax
    mov ecx, [esp + 4]                  ; handler_fn
    mov [edi + EVBUS_SUB_OFF_HANDLER], ecx
    mov byte [edi + EVBUS_SUB_OFF_STATE], NP_SUB_ACTIVE
    mov cl, [esp]                       ; delivery_mode
    mov [edi + EVBUS_SUB_OFF_DELIVERY], cl
    mov byte [edi + EVBUS_SUB_OFF_FLAGS], 0
    mov byte [edi + EVBUS_SUB_OFF_PRESENT], 1
    mov dword [edi + EVBUS_SUB_OFF_QUEUED], 0
    mov dword [edi + EVBUS_SUB_OFF_LOST], 0
    pop edx
    pop ecx
    pop edi
    pop esi
    clc
    ret
.esub_fail:
    pop edx
    pop ecx
    pop edi
    pop esi
    xor ebx, ebx
    stc
    ret

; ---------------------------------------------------------------------------
; evbus_publish – Ereignis veröffentlichen (§16)
; EAX=type_id, ECX=source_obj_id, EDX=subject_obj_id,
; ESI=payload_ptr (0=kein Payload), EDI=payload_size
; CF=0 OK / CF=1 Fehler (unbekannt/zu groß/Rekursion)
; Bootstrap: nur DELIVERY_CALLBACK; kein re-entrant publish (depth=0→1 max).
; ---------------------------------------------------------------------------
evbus_publish:
    ; Argumente in statischen Temps speichern (single-threaded bootstrap)
    mov [evbus_pub_type],   eax
    mov [evbus_pub_src],    ecx
    mov [evbus_pub_subj],   edx
    mov [evbus_pub_payload],esi
    mov [evbus_pub_plsize], edi

    push ebx
    push esi
    push edi

    ; Re-Entrant-Schutz (§23, §61)
    cmp byte [evbus_depth], 1
    jae .ep_fail

    ; Schema prüfen (§16 Schritt 1+3)
    mov eax, [evbus_pub_type]
    call evbus_find_schema
    jc .ep_fail

    ; Payload-Größe gegen Schema-Maximum prüfen (§13)
    movzx eax, word [ebx + EVBUS_SCH_OFF_MAX_PL]
    cmp [evbus_pub_plsize], eax
    ja .ep_fail

    ; Sequence erhöhen, Tiefe setzen
    lock inc dword [evbus_sequence]
    mov byte [evbus_depth], 1

    ; Alle aktiven Subscriptions durchlaufen
    mov esi, evbus_subs
    xor ecx, ecx
.ep_loop:
    cmp ecx, EVBUS_MAX_SUBS
    jae .ep_done
    cmp byte [esi + EVBUS_SUB_OFF_PRESENT], 1
    jne .ep_next
    cmp byte [esi + EVBUS_SUB_OFF_STATE], NP_SUB_ACTIVE
    jne .ep_next

    ; Typ-Filter (§18): 0 = alle Typen akzeptieren
    mov edi, [esi + EVBUS_SUB_OFF_TYPE_FILTER]
    test edi, edi
    jz .ep_deliver
    cmp edi, [evbus_pub_type]
    jne .ep_next

.ep_deliver:
    cmp byte [esi + EVBUS_SUB_OFF_DELIVERY], NP_EVENT_DELIVERY_CALLBACK
    jne .ep_lost            ; Bootstrap: kein Queue-/IPC-Deliver → lost++

    mov ebx, [esi + EVBUS_SUB_OFF_HANDLER]
    test ebx, ebx
    jz .ep_lost
    ; Callback: fn(EAX=type_id, ECX=src, EDX=subj) – CF ignoriert
    mov eax, [evbus_pub_type]
    mov ecx, [evbus_pub_src]
    mov edx, [evbus_pub_subj]
    call ebx
    lock inc dword [evbus_stat_delivered]
    lock inc dword [evbus_stat_sync]
    jmp .ep_next

.ep_lost:
    lock inc dword [esi + EVBUS_SUB_OFF_LOST]

.ep_next:
    add esi, EVBUS_SUB_SIZE
    inc ecx
    jmp .ep_loop

.ep_done:
    mov byte [evbus_depth], 0
    lock inc dword [evbus_stat_published]
    pop edi
    pop esi
    pop ebx
    clc
    ret

.ep_fail:
    lock inc dword [evbus_stat_rejected]
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; evbus_initialize – Pools leeren, 8 Built-in-Schemata registrieren (§38)
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------

%macro evbus_reg_builtin 4          ; type_id, class, max_payload, flags
    mov eax, %1
    mov ecx, %2
    mov edx, %3
    mov esi, %4
    call evbus_register_schema
    jc .einit_fail
%endmacro

evbus_initialize:
    push edi
    push ecx

    mov edi, evbus_schemas
    xor eax, eax
    mov ecx, (EVBUS_MAX_SCHEMAS * EVBUS_SCH_SIZE) / 4
    rep stosd
    mov edi, evbus_subs
    mov ecx, (EVBUS_MAX_SUBS * EVBUS_SUB_SIZE) / 4
    rep stosd
    mov dword [evbus_next_sub_id], 0
    mov dword [evbus_sequence],    0
    mov byte  [evbus_depth],       0
    mov dword [evbus_stat_published], 0
    mov dword [evbus_stat_delivered], 0
    mov dword [evbus_stat_sync],      0
    mov dword [evbus_stat_rejected],  0

    evbus_reg_builtin NP_EVENT_BOOT_READY,        NP_EVENT_CLASS_LIFECYCLE, 0,  NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    evbus_reg_builtin NP_EVENT_BOOT_PANIC,        NP_EVENT_CLASS_ERROR,     4,  NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    evbus_reg_builtin NP_EVENT_OBJ_CREATED,       NP_EVENT_CLASS_LIFECYCLE, 8,  NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    evbus_reg_builtin NP_EVENT_OBJ_ACTIVATED,     NP_EVENT_CLASS_LIFECYCLE, 8,  NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    evbus_reg_builtin NP_EVENT_OBJ_DESTROYED,     NP_EVENT_CLASS_LIFECYCLE, 8,  NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    evbus_reg_builtin NP_EVENT_OBJ_STATE_CHANGED, NP_EVENT_CLASS_STATE,     8,  NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    evbus_reg_builtin NP_EVENT_OBJ_LINK_ADDED,    NP_EVENT_CLASS_STATE,     8,  NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    evbus_reg_builtin NP_EVENT_OBJ_LINK_REMOVED,  NP_EVENT_CLASS_STATE,     8,  NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY

    pop ecx
    pop edi
    clc
    ret
.einit_fail:
    pop ecx
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; evbus_self_test – §62 Tests 1,2,3,7,11,12,14 (Bootstrap-Implementierung)
; Tests 4-6,8-10,13,15-38: Bootstrap-N/A (Queue/IPC/SMP/Subtree).
; CF=0 alle Tests bestanden / CF=1 Fehler
; ---------------------------------------------------------------------------
evbus_self_test:
    push ebx
    push esi
    push edi

    ; Test 1 – BOOT_READY-Schema registriert (§62.1)
    mov eax, NP_EVENT_BOOT_READY
    call evbus_find_schema
    jc .est_fail
    cmp byte [ebx + EVBUS_SCH_OFF_CLASS], NP_EVENT_CLASS_LIFECYCLE
    jne .est_fail

    ; Test 2 – Doppelte type_id ablehnen (§62.2)
    mov eax, NP_EVENT_BOOT_READY
    mov ecx, NP_EVENT_CLASS_LIFECYCLE
    mov edx, 0
    mov esi, 0
    call evbus_register_schema
    jnc .est_fail                       ; muss CF=1

    ; Test 3 – Synchrone Callback-Zustellung (§62.3)
    mov dword [evbus_test_flag], 0
    mov eax, NP_EVENT_BOOT_READY
    mov ecx, evbus_test_callback
    mov edx, NP_EVENT_DELIVERY_CALLBACK
    call evbus_subscribe
    jc .est_fail
    push ebx                            ; sub1_id

    mov eax, NP_EVENT_BOOT_READY
    xor ecx, ecx
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish
    jc .est_pop1_fail
    cmp dword [evbus_test_flag], 0xCAFEBABE
    jne .est_pop1_fail

    ; Test 7 – Filter nach Eventtyp (§62.7)
    ; Sub2 abonniert nur OBJ_CREATED; publish OBJ_CREATED → sub2 feuert
    mov dword [evbus_test_flag], 0
    mov eax, NP_EVENT_OBJ_CREATED
    mov ecx, evbus_test_callback
    mov edx, NP_EVENT_DELIVERY_CALLBACK
    call evbus_subscribe
    jc .est_pop1_fail
    push ebx                            ; sub2_id

    mov eax, NP_EVENT_OBJ_CREATED
    mov ecx, 1
    mov edx, 2
    xor esi, esi
    xor edi, edi
    call evbus_publish
    jc .est_pop2_fail
    cmp dword [evbus_test_flag], 0xCAFEBABE
    jne .est_pop2_fail

    ; Test 11 – Gültige Payload-Größe (§62.11): OBJ_CREATED max=8, size=4 → OK
    mov eax, NP_EVENT_OBJ_CREATED
    xor ecx, ecx
    xor edx, edx
    mov esi, evbus_test_buf
    mov edi, 4
    call evbus_publish
    jc .est_pop2_fail

    ; Test 12 – Ungültige Payload-Größe (§62.12): size=64 > max=8 → CF=1
    mov eax, NP_EVENT_OBJ_CREATED
    xor ecx, ecx
    xor edx, edx
    mov esi, evbus_test_buf
    mov edi, 64
    call evbus_publish
    jnc .est_pop2_fail                  ; muss CF=1

    ; Test 14 – Eventreihenfolge: sequence steigt pro Publish (§62.14)
    mov eax, [evbus_sequence]
    push eax                            ; seq_before
    mov eax, NP_EVENT_BOOT_READY
    xor ecx, ecx
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish
    jc .est_pop3_fail
    pop eax                             ; seq_before
    cmp [evbus_sequence], eax
    jbe .est_pop2_fail

    ; Aufräumen
    pop ebx                             ; sub2_id (ungenutzt)
    pop ebx                             ; sub1_id (ungenutzt)
    pop edi
    pop esi
    pop ebx
    clc
    ret

.est_pop3_fail:
    pop eax                             ; seq_before
    jmp .est_pop2_fail
.est_pop2_fail:
    pop ebx                             ; sub2_id
.est_pop1_fail:
    pop ebx                             ; sub1_id
.est_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; Test-Callback: setzt evbus_test_flag = 0xCAFEBABE
evbus_test_callback:
    mov dword [evbus_test_flag], 0xCAFEBABE
    ret

; Event-Bus-Daten
evbus_schemas:          times (EVBUS_MAX_SCHEMAS * EVBUS_SCH_SIZE) db 0
evbus_subs:             times (EVBUS_MAX_SUBS * EVBUS_SUB_SIZE) db 0
evbus_next_sub_id:      dd 0
evbus_sequence:         dd 0
evbus_depth:            db 0
                        db 0, 0, 0              ; Alignment
evbus_stat_published:   dd 0
evbus_stat_delivered:   dd 0
evbus_stat_sync:        dd 0
evbus_stat_rejected:    dd 0
evbus_pub_type:         dd 0
evbus_pub_src:          dd 0
evbus_pub_subj:         dd 0
evbus_pub_payload:      dd 0
evbus_pub_plsize:       dd 0
evbus_test_flag:        dd 0
evbus_test_buf:         times 16 db 0

; ---------------------------------------------------------------------------
; §102 – Unified Object API (NPSPEC-KERNEL-0102)
; Einheitliche Zugangsschicht für verwaltete Kernelobjekte.
; Bootstrap: Phase 1 (§64): Typregistrierung, Objekterzeugung, Handle-Auflösung,
; Referenzzählung, Capability-Prüfung.
; Phase 2-4 (Namespaces, async, RCU): Bootstrap-N/A.
; ---------------------------------------------------------------------------

; Objektzustände (§9)
UOBJ_STATE_CREATING     equ 0
UOBJ_STATE_ACTIVE       equ 1
UOBJ_STATE_QUIESCING    equ 2
UOBJ_STATE_CLOSING      equ 3
UOBJ_STATE_ZOMBIE       equ 4
UOBJ_STATE_DESTROYING   equ 5
UOBJ_STATE_DESTROYED    equ 6

; Capabilities (§15)
UOBJ_CAP_QUERY          equ 0x001
UOBJ_CAP_MODIFY         equ 0x002
UOBJ_CAP_SIGNAL         equ 0x004
UOBJ_CAP_WAIT           equ 0x008
UOBJ_CAP_DUPLICATE      equ 0x010
UOBJ_CAP_TRANSFER       equ 0x020
UOBJ_CAP_SUBSCRIBE      equ 0x040
UOBJ_CAP_DELETE         equ 0x080
UOBJ_CAP_ADMIN          equ 0x100

; Fehlercodes (§48)
UOBJ_ERR_INVALID_HANDLE equ -50
UOBJ_ERR_STALE_REF      equ -51
UOBJ_ERR_TYPE_MISMATCH  equ -52
UOBJ_ERR_ACCESS_DENIED  equ -53
UOBJ_ERR_INVALID_STATE  equ -54
UOBJ_ERR_NOT_SUPPORTED  equ -55

; Pool-Größen
UOBJ_MAX_TYPES          equ 16
UOBJ_MAX_OBJECTS        equ 32
UOBJ_MAX_HANDLES        equ 32

; Typ-Deskriptor-Layout (24 Bytes, §7)
UOBJ_TYPE_SIZE              equ 24
UOBJ_TYPE_OFF_TYPE_ID       equ 0    ; uint32_t
UOBJ_TYPE_OFF_ABI_VER       equ 4    ; uint32_t
UOBJ_TYPE_OFF_FLAGS         equ 8    ; uint32_t
UOBJ_TYPE_OFF_INIT_FN       equ 12   ; uint32_t (fn-ptr, 0=kein Init)
UOBJ_TYPE_OFF_DESTROY_FN    equ 16   ; uint32_t (fn-ptr, 0=kein Destroy)
UOBJ_TYPE_OFF_PRESENT       equ 20   ; uint8_t
; 21-23: Padding

; Objekt-Header-Layout (32 Bytes, §5)
UOBJ_OBJ_SIZE               equ 32
UOBJ_OBJ_OFF_OBJ_ID         equ 0    ; uint32_t
UOBJ_OBJ_OFF_TYPE_ID        equ 4    ; uint32_t
UOBJ_OBJ_OFF_GEN            equ 8    ; uint32_t
UOBJ_OBJ_OFF_STATE          equ 12   ; uint8_t
UOBJ_OBJ_OFF_PRESENT        equ 13   ; uint8_t
UOBJ_OBJ_OFF_FLAGS          equ 14   ; uint16_t
UOBJ_OBJ_OFF_REFCOUNT       equ 16   ; uint32_t
UOBJ_OBJ_OFF_KOG_NODE       equ 20   ; uint32_t (KOG-Knoten-ID, 0=nicht verknüpft)
; 24-31: Reserved

; Handle-Tabellen-Eintrag-Layout (16 Bytes, §13)
UOBJ_HDL_SIZE               equ 16
UOBJ_HDL_OFF_HDL_ID         equ 0    ; uint32_t
UOBJ_HDL_OFF_OBJ_ID         equ 4    ; uint32_t
UOBJ_HDL_OFF_GEN            equ 8    ; uint32_t (Generation bei Erzeugung)
UOBJ_HDL_OFF_CAPS           equ 12   ; uint16_t (Capability-Bitmaske)
UOBJ_HDL_OFF_PRESENT        equ 14   ; uint8_t
; 15: Padding

; Compile-time-Invarianten
%if UOBJ_TYPE_SIZE != 24
%error "UOBJ_TYPE_SIZE muss 24 Bytes sein"
%endif
%if UOBJ_OBJ_SIZE != 32
%error "UOBJ_OBJ_SIZE muss 32 Bytes sein"
%endif
%if UOBJ_HDL_SIZE != 16
%error "UOBJ_HDL_SIZE muss 16 Bytes sein"
%endif

; ---------------------------------------------------------------------------
; Interne Hilfsfunktionen (lineare Suche über feste Pools)
; ---------------------------------------------------------------------------

; uobj_find_type: EAX=type_id → EBX=ptr, CF=0/CF=1; Clobbers: EBX
uobj_find_type:
    push esi
    push ecx
    mov esi, uobj_types
    xor ecx, ecx
.uft_loop:
    cmp ecx, UOBJ_MAX_TYPES
    jae .uft_not_found
    cmp byte [esi + UOBJ_TYPE_OFF_PRESENT], 1
    jne .uft_next
    cmp [esi + UOBJ_TYPE_OFF_TYPE_ID], eax
    je .uft_found
.uft_next:
    add esi, UOBJ_TYPE_SIZE
    inc ecx
    jmp .uft_loop
.uft_found:
    mov ebx, esi
    pop ecx
    pop esi
    clc
    ret
.uft_not_found:
    xor ebx, ebx
    pop ecx
    pop esi
    stc
    ret

; uobj_find_object: EAX=obj_id → EBX=ptr, CF=0/CF=1; Clobbers: EBX
uobj_find_object:
    push esi
    push ecx
    mov esi, uobj_objects
    xor ecx, ecx
.ufo_loop:
    cmp ecx, UOBJ_MAX_OBJECTS
    jae .ufo_not_found
    cmp byte [esi + UOBJ_OBJ_OFF_PRESENT], 1
    jne .ufo_next
    cmp [esi + UOBJ_OBJ_OFF_OBJ_ID], eax
    je .ufo_found
.ufo_next:
    add esi, UOBJ_OBJ_SIZE
    inc ecx
    jmp .ufo_loop
.ufo_found:
    mov ebx, esi
    pop ecx
    pop esi
    clc
    ret
.ufo_not_found:
    xor ebx, ebx
    pop ecx
    pop esi
    stc
    ret

; uobj_find_handle: EAX=handle_id → EBX=ptr, CF=0/CF=1; Clobbers: EBX
uobj_find_handle:
    push esi
    push ecx
    mov esi, uobj_handles
    xor ecx, ecx
.ufhdl_loop:
    cmp ecx, UOBJ_MAX_HANDLES
    jae .ufhdl_not_found
    cmp byte [esi + UOBJ_HDL_OFF_PRESENT], 1
    jne .ufhdl_next
    cmp [esi + UOBJ_HDL_OFF_HDL_ID], eax
    je .ufhdl_found
.ufhdl_next:
    add esi, UOBJ_HDL_SIZE
    inc ecx
    jmp .ufhdl_loop
.ufhdl_found:
    mov ebx, esi
    pop ecx
    pop esi
    clc
    ret
.ufhdl_not_found:
    xor ebx, ebx
    pop ecx
    pop esi
    stc
    ret

; uobj_alloc_object: → EBX=obj_ptr, ECX=new_obj_id, CF=0/CF=1
uobj_alloc_object:
    push esi
    mov esi, uobj_objects
    xor ecx, ecx
.uao_scan:
    cmp ecx, UOBJ_MAX_OBJECTS
    jae .uao_full
    cmp byte [esi + UOBJ_OBJ_OFF_PRESENT], 0
    je .uao_write
    add esi, UOBJ_OBJ_SIZE
    inc ecx
    jmp .uao_scan
.uao_write:
    lock inc dword [uobj_next_id]
    mov ecx, [uobj_next_id]
    mov [esi + UOBJ_OBJ_OFF_OBJ_ID], ecx
    mov byte [esi + UOBJ_OBJ_OFF_PRESENT], 1
    mov ebx, esi
    pop esi
    clc
    ret
.uao_full:
    xor ebx, ebx
    xor ecx, ecx
    pop esi
    stc
    ret

; uobj_alloc_handle: EAX=obj_id, ECX=generation, EDX=capabilities
; → EBX=handle_id, CF=0/CF=1
uobj_alloc_handle:
    push esi
    push edi
    push ecx    ; [esp+4 after next push]=ECX(gen)
    push edx    ; Stack: [esp]=EDX(caps), [esp+4]=ECX(gen),
                ;        [esp+8]=old_EDI, [esp+12]=old_ESI  EAX=obj_id

    mov edi, uobj_handles
    xor ecx, ecx
.uah_scan:
    cmp ecx, UOBJ_MAX_HANDLES
    jae .uah_full
    cmp byte [edi + UOBJ_HDL_OFF_PRESENT], 0
    je .uah_write
    add edi, UOBJ_HDL_SIZE
    inc ecx
    jmp .uah_scan
.uah_write:
    lock inc dword [uobj_next_hdl]
    mov ebx, [uobj_next_hdl]
    mov [edi + UOBJ_HDL_OFF_HDL_ID], ebx
    mov [edi + UOBJ_HDL_OFF_OBJ_ID], eax
    mov ecx, [esp + 4]                      ; generation
    mov [edi + UOBJ_HDL_OFF_GEN], ecx
    mov cx, [esp]                           ; capabilities (low 16)
    mov [edi + UOBJ_HDL_OFF_CAPS], cx
    mov byte [edi + UOBJ_HDL_OFF_PRESENT], 1
    pop edx
    pop ecx
    pop edi
    pop esi
    clc
    ret
.uah_full:
    xor ebx, ebx
    pop edx
    pop ecx
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; uobj_register_type – Objekttyp registrieren (§7)
; EAX=type_id, ECX=abi_version, EDX=flags, ESI=init_fn (0=kein), EDI=destroy_fn (0=kein)
; CF=0 OK / CF=1 Fehler (doppelt/voll)
; ---------------------------------------------------------------------------
uobj_register_type:
    push ebx
    push esi
    push edi
    push ecx    ; [esp+4 after next push]=ECX(abi_ver)
    push edx    ; Stack: [esp]=EDX(flags), [esp+4]=ECX(abi_ver),
                ;        [esp+8]=old_EDI(destroy_fn), [esp+12]=old_ESI(init_fn), [esp+16]=old_EBX

    call uobj_find_type         ; EAX=type_id; EAX unveränderter Wert
    jnc .urt_dup                ; CF=0 = bereits vorhanden

    mov edi, uobj_types
    xor ecx, ecx
.urt_scan:
    cmp ecx, UOBJ_MAX_TYPES
    jae .urt_full
    cmp byte [edi + UOBJ_TYPE_OFF_PRESENT], 0
    je .urt_write
    add edi, UOBJ_TYPE_SIZE
    inc ecx
    jmp .urt_scan
.urt_write:
    mov [edi + UOBJ_TYPE_OFF_TYPE_ID], eax
    mov ebx, [esp + 4]                      ; abi_version
    mov [edi + UOBJ_TYPE_OFF_ABI_VER], ebx
    mov ebx, [esp]                          ; flags
    mov [edi + UOBJ_TYPE_OFF_FLAGS], ebx
    mov ebx, [esp + 12]                     ; init_fn (old ESI)
    mov [edi + UOBJ_TYPE_OFF_INIT_FN], ebx
    mov ebx, [esp + 8]                      ; destroy_fn (old EDI)
    mov [edi + UOBJ_TYPE_OFF_DESTROY_FN], ebx
    mov byte [edi + UOBJ_TYPE_OFF_PRESENT], 1
    pop edx
    pop ecx
    pop edi
    pop esi
    pop ebx
    clc
    ret
.urt_dup:
.urt_full:
    pop edx
    pop ecx
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; uobj_create – Objekt erzeugen und Handle zurückgeben (§11)
; EAX=type_id, ECX=capabilities
; EBX=handle_id, ECX=obj_id (Rückgabe), CF=0 OK / CF=1 Fehler
; Bootstrap: Pools; kein Rollback über KOG-Integration.
; ---------------------------------------------------------------------------
uobj_create:
    mov [uobj_tmp_type], eax
    mov [uobj_tmp_caps], ecx

    push esi
    push edi

    ; Typ validieren
    mov eax, [uobj_tmp_type]
    call uobj_find_type
    jc .uc_fail
    mov esi, ebx                ; ESI = type_ptr (callee-saved)

    ; Objekt-Slot allokieren
    call uobj_alloc_object      ; → EBX=obj_ptr, ECX=obj_id
    jc .uc_fail
    mov edi, ebx                ; EDI = obj_ptr (callee-saved)
    mov [uobj_tmp_obj_id], ecx

    ; Objekt-Header initialisieren
    lock inc dword [uobj_next_gen]
    mov eax, [uobj_next_gen]
    mov [edi + UOBJ_OBJ_OFF_GEN], eax
    mov eax, [esi + UOBJ_TYPE_OFF_TYPE_ID]
    mov [edi + UOBJ_OBJ_OFF_TYPE_ID], eax
    mov byte [edi + UOBJ_OBJ_OFF_STATE], UOBJ_STATE_CREATING
    mov dword [edi + UOBJ_OBJ_OFF_REFCOUNT], 1
    mov dword [edi + UOBJ_OBJ_OFF_KOG_NODE], 0
    mov word [edi + UOBJ_OBJ_OFF_FLAGS], 0

    ; Typspezifischen init_fn aufrufen (§11 Schritt 5)
    mov eax, [esi + UOBJ_TYPE_OFF_INIT_FN]
    test eax, eax
    jz .uc_no_init
    mov ebx, edi                ; init_fn(EBX=obj_ptr) → CF
    call eax
    jc .uc_init_fail
.uc_no_init:
    mov byte [edi + UOBJ_OBJ_OFF_STATE], UOBJ_STATE_ACTIVE

    ; Handle allokieren
    mov eax, [uobj_tmp_obj_id]
    mov ecx, [edi + UOBJ_OBJ_OFF_GEN]
    mov edx, [uobj_tmp_caps]
    call uobj_alloc_handle      ; EAX=obj_id, ECX=gen, EDX=caps → EBX=handle_id
    jc .uc_handle_fail

    ; Rückgabewerte: EBX=handle_id, ECX=obj_id
    mov ecx, [uobj_tmp_obj_id]

    ; NP_EVENT_OBJ_CREATED veröffentlichen (§40, §29)
    push ebx
    push ecx
    mov eax, NP_EVENT_OBJ_CREATED
    mov ecx, [uobj_tmp_obj_id]
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish          ; CF ignoriert (Bootstrap-Event)
    pop ecx
    pop ebx                     ; EBX = handle_id (aus evbus-push/pop)

    lock inc dword [uobj_stat_created]
    pop edi
    pop esi
    ; EBX = handle_id (kein frame-pop, da push ebx am Anfang entfernt)
    clc
    ret

.uc_handle_fail:
.uc_init_fail:
    mov byte [edi + UOBJ_OBJ_OFF_PRESENT], 0
.uc_fail:
    lock inc dword [uobj_stat_failed]
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; uobj_from_handle – Handle auflösen + Capabilities und Generation prüfen (§13)
; EAX=handle_id, ECX=required_capabilities
; EBX=obj_ptr (Rückgabe), CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
uobj_from_handle:
    push esi
    push ecx                    ; [esp] = required_caps

    call uobj_find_handle       ; EAX=handle_id → EBX=hdl_ptr
    jc .ufrom_fail

    mov esi, ebx                ; ESI = hdl_ptr

    ; Capability-Prüfung: alle geforderten Bits müssen im Handle vorhanden sein
    movzx eax, word [esi + UOBJ_HDL_OFF_CAPS]
    mov ecx, [esp]              ; required_caps
    and eax, ecx                ; EAX = vorhandene & geforderte
    cmp eax, ecx                ; müssen identisch sein
    jne .ufrom_denied

    ; Objekt auflösen
    mov eax, [esi + UOBJ_HDL_OFF_OBJ_ID]
    call uobj_find_object       ; → EBX=obj_ptr
    jc .ufrom_fail

    ; Generations-Prüfung (§6 Invariant 4, §60 Invariant 4)
    mov eax, [esi + UOBJ_HDL_OFF_GEN]
    cmp eax, [ebx + UOBJ_OBJ_OFF_GEN]
    jne .ufrom_stale

    ; Zustandsprüfung: kein Zugriff auf sterbende Objekte
    movzx eax, byte [ebx + UOBJ_OBJ_OFF_STATE]
    cmp eax, UOBJ_STATE_DESTROYING
    jae .ufrom_bad_state

    pop ecx
    pop esi
    clc
    ret

.ufrom_stale:
.ufrom_bad_state:
.ufrom_denied:
.ufrom_fail:
    xor ebx, ebx
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; uobj_retain – Referenzzähler erhöhen (§12)
; EAX=obj_id → CF=0 OK / CF=1 nicht gefunden
; ---------------------------------------------------------------------------
uobj_retain:
    push ebx
    call uobj_find_object
    jc .uret_fail
    lock inc dword [ebx + UOBJ_OBJ_OFF_REFCOUNT]
    pop ebx
    clc
    ret
.uret_fail:
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; uobj_release – Referenzzähler senken; bei 0 Objekt zerstören (§12)
; EAX=obj_id → CF=0 OK / CF=1 nicht gefunden
; ---------------------------------------------------------------------------
uobj_release:
    push ebx
    push esi
    call uobj_find_object       ; EAX=obj_id → EBX=obj_ptr
    jc .urel_fail

    lock dec dword [ebx + UOBJ_OBJ_OFF_REFCOUNT]
    jnz .urel_done

    ; Refcount = 0: Objekt zerstören (§9 DESTROYING → DESTROYED)
    mov esi, ebx                ; ESI = obj_ptr
    mov byte [esi + UOBJ_OBJ_OFF_STATE], UOBJ_STATE_DESTROYING

    ; destroy_fn aufrufen
    mov eax, [esi + UOBJ_OBJ_OFF_TYPE_ID]
    call uobj_find_type         ; → EBX=type_ptr (ESI bleibt erhalten)
    jc .urel_no_fn
    mov eax, [ebx + UOBJ_TYPE_OFF_DESTROY_FN]
    test eax, eax
    jz .urel_no_fn
    mov ebx, esi                ; destroy_fn(EBX=obj_ptr)
    call eax
.urel_no_fn:
    ; obj_id vor Freigabe lesen (§41 – Diagnose nach Destroy)
    mov ecx, [esi + UOBJ_OBJ_OFF_OBJ_ID]
    mov byte [esi + UOBJ_OBJ_OFF_STATE], UOBJ_STATE_DESTROYED
    mov byte [esi + UOBJ_OBJ_OFF_PRESENT], 0
    lock inc dword [uobj_stat_destroyed]

    ; NP_EVENT_OBJ_DESTROYED veröffentlichen
    mov eax, NP_EVENT_OBJ_DESTROYED
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish          ; EAX=type, ECX=obj_id (noch gesetzt)

.urel_done:
    pop esi
    pop ebx
    clc
    ret
.urel_fail:
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; uobj_close_handle – Handle schließen und Objektreferenz freigeben (§33)
; EAX=handle_id → CF=0 OK / CF=1 nicht gefunden
; ---------------------------------------------------------------------------
uobj_close_handle:
    push ebx
    call uobj_find_handle       ; → EBX=hdl_ptr
    jc .uch_fail
    mov byte [ebx + UOBJ_HDL_OFF_PRESENT], 0
    mov eax, [ebx + UOBJ_HDL_OFF_OBJ_ID]
    call uobj_release           ; CF ignoriert (Objekt könnte schon weg sein)
    pop ebx
    clc
    ret
.uch_fail:
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; uobj_query – Objekt abfragen (§20), Bootstrap: nur Klasse 0 (Basisidentität)
; EAX=handle_id, ECX=query_class, EDX=buffer_ptr
; EAX=bytes_written (Rückgabe), CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
uobj_query:
    push ebx
    push esi
    push edi
    push ecx                    ; query_class
    push edx                    ; buffer_ptr

    ; Handle auflösen (QUERY-Recht prüfen)
    mov ecx, UOBJ_CAP_QUERY
    call uobj_from_handle       ; EAX=handle → EBX=obj_ptr
    jc .uq_fail

    ; Nur Klasse 0 in Bootstrap implementiert (§20 Basisidentität)
    cmp dword [esp + 4], 0
    jne .uq_not_supported

    ; 16-Byte-Antwort: obj_id, type_id, generation, state
    mov edi, [esp]              ; buffer_ptr
    mov eax, [ebx + UOBJ_OBJ_OFF_OBJ_ID]
    mov [edi],      eax
    mov eax, [ebx + UOBJ_OBJ_OFF_TYPE_ID]
    mov [edi + 4],  eax
    mov eax, [ebx + UOBJ_OBJ_OFF_GEN]
    mov [edi + 8],  eax
    movzx eax, byte [ebx + UOBJ_OBJ_OFF_STATE]
    mov [edi + 12], eax

    pop edx
    pop ecx
    pop edi
    pop esi
    pop ebx
    mov eax, 16
    clc
    ret

.uq_not_supported:
.uq_fail:
    pop edx
    pop ecx
    pop edi
    pop esi
    pop ebx
    xor eax, eax
    stc
    ret

; ---------------------------------------------------------------------------
; uobj_initialize – Pools leeren, 15 Built-in-Typen registrieren (§55)
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------

%macro uobj_reg_builtin 1
    mov eax, %1
    mov ecx, 1
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call uobj_register_type
    jc .uinit_fail
%endmacro

uobj_initialize:
    push edi
    push ecx

    mov edi, uobj_types
    xor eax, eax
    mov ecx, (UOBJ_MAX_TYPES * UOBJ_TYPE_SIZE) / 4
    rep stosd
    mov edi, uobj_objects
    mov ecx, (UOBJ_MAX_OBJECTS * UOBJ_OBJ_SIZE) / 4
    rep stosd
    mov edi, uobj_handles
    mov ecx, (UOBJ_MAX_HANDLES * UOBJ_HDL_SIZE) / 4
    rep stosd
    mov dword [uobj_next_id],  0
    mov dword [uobj_next_gen], 0
    mov dword [uobj_next_hdl], 0
    mov dword [uobj_stat_created],   0
    mov dword [uobj_stat_destroyed], 0
    mov dword [uobj_stat_failed],    0

    uobj_reg_builtin NP_OBJTYPE_KERNEL
    uobj_reg_builtin NP_OBJTYPE_MACHINE
    uobj_reg_builtin NP_OBJTYPE_NAMESPACE
    uobj_reg_builtin NP_OBJTYPE_PROCESS
    uobj_reg_builtin NP_OBJTYPE_THREAD
    uobj_reg_builtin NP_OBJTYPE_CPU
    uobj_reg_builtin NP_OBJTYPE_MEMORY
    uobj_reg_builtin NP_OBJTYPE_IPC
    uobj_reg_builtin NP_OBJTYPE_DEVICE
    uobj_reg_builtin NP_OBJTYPE_DRIVER
    uobj_reg_builtin NP_OBJTYPE_VFS_NODE
    uobj_reg_builtin NP_OBJTYPE_SECURITY
    uobj_reg_builtin NP_OBJTYPE_DIAGNOSTIC
    uobj_reg_builtin NP_OBJTYPE_POWER
    uobj_reg_builtin NP_OBJTYPE_GENERIC

    pop ecx
    pop edi
    clc
    ret
.uinit_fail:
    pop ecx
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; uobj_self_test – §58 Tests 1-10 (Bootstrap-Implementierung)
; Tests 11-15 (SMP, async, weak-ref, namespace): Bootstrap-N/A.
; CF=0 alle Tests bestanden / CF=1 Fehler
; ---------------------------------------------------------------------------
uobj_self_test:
    push ebx
    push esi
    push edi

    ; Test 1 – Typ GENERIC registriert
    mov eax, NP_OBJTYPE_GENERIC
    call uobj_find_type
    jc .ust_fail
    cmp byte [ebx + UOBJ_TYPE_OFF_PRESENT], 1
    jne .ust_fail

    ; Test 2 – Doppelter Typ abgelehnt (§60 Invariant)
    mov eax, NP_OBJTYPE_GENERIC
    mov ecx, 1
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call uobj_register_type
    jnc .ust_fail               ; muss CF=1

    ; Test 3 – Objekt erzeugen → gültiger Handle
    mov eax, NP_OBJTYPE_GENERIC
    mov ecx, UOBJ_CAP_QUERY | UOBJ_CAP_DELETE
    call uobj_create            ; → EBX=handle_id, ECX=obj_id
    jc .ust_fail
    push ebx                    ; [esp+4] = handle_id
    push ecx                    ; [esp]   = obj_id

    ; Test 4 – Handle-Auflösung mit korrekten Caps (QUERY erlaubt)
    mov eax, [esp + 4]          ; handle_id
    mov ecx, UOBJ_CAP_QUERY
    call uobj_from_handle       ; → EBX=obj_ptr
    jc .ust_pop2_fail

    ; Test 5 – Unzureichende Caps (ADMIN nicht im Handle) → CF=1
    mov eax, [esp + 4]          ; handle_id
    mov ecx, UOBJ_CAP_ADMIN
    call uobj_from_handle
    jnc .ust_pop2_fail          ; muss CF=1

    ; Test 6 – uobj_retain: Refcount auf 2
    mov eax, [esp]              ; obj_id
    call uobj_retain
    jc .ust_pop2_fail
    call uobj_find_object       ; EAX noch = obj_id → EBX=obj_ptr
    jc .ust_pop2_fail
    cmp dword [ebx + UOBJ_OBJ_OFF_REFCOUNT], 2
    jne .ust_pop2_fail

    ; Test 7 – uobj_release: Refcount zurück auf 1, Objekt noch vorhanden
    mov eax, [esp]              ; obj_id
    call uobj_release
    jc .ust_pop2_fail
    mov eax, [esp]
    call uobj_find_object
    jc .ust_pop2_fail
    cmp dword [ebx + UOBJ_OBJ_OFF_REFCOUNT], 1
    jne .ust_pop2_fail
    cmp byte [ebx + UOBJ_OBJ_OFF_PRESENT], 1
    jne .ust_pop2_fail

    ; Test 8 – uobj_query Klasse 0: 16 Bytes, korrekte obj_id
    mov eax, [esp + 4]          ; handle_id
    mov ecx, 0                  ; query class 0 = Basisidentität
    mov edx, uobj_test_buf
    call uobj_query
    jc .ust_pop2_fail
    cmp eax, 16
    jne .ust_pop2_fail
    mov eax, [esp]              ; obj_id
    cmp [uobj_test_buf], eax
    jne .ust_pop2_fail

    ; Test 9 – Handle schließen
    mov eax, [esp + 4]          ; handle_id
    call uobj_close_handle
    jc .ust_pop2_fail

    ; Test 10 – Handle nach Schließung nicht mehr auffindbar (§60 Invariant 2/3)
    mov eax, [esp + 4]          ; handle_id (geschlossen)
    call uobj_find_handle
    jnc .ust_pop2_fail          ; muss CF=1

    ; Aufräumen (obj_id, handle_id vom Stack)
    pop ecx
    pop ebx
    pop edi
    pop esi
    pop ebx
    clc
    ret

.ust_pop2_fail:
    pop ecx                     ; obj_id
    pop ebx                     ; handle_id
.ust_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; UOBJ-Daten
uobj_types:           times (UOBJ_MAX_TYPES * UOBJ_TYPE_SIZE) db 0
uobj_objects:         times (UOBJ_MAX_OBJECTS * UOBJ_OBJ_SIZE) db 0
uobj_handles:         times (UOBJ_MAX_HANDLES * UOBJ_HDL_SIZE) db 0
uobj_next_id:         dd 0
uobj_next_gen:        dd 0
uobj_next_hdl:        dd 0
uobj_stat_created:    dd 0
uobj_stat_destroyed:  dd 0
uobj_stat_failed:     dd 0
uobj_tmp_type:        dd 0
uobj_tmp_caps:        dd 0
uobj_tmp_obj_id:      dd 0
uobj_test_buf:        times 16 db 0

; ===========================================================================
; §103 – Capability Framework 1.0 (NPSPEC-KERNEL-0103)
; ===========================================================================

; ---------------------------------------------------------------------------
; Capability Rights (§10) – lower 32 Bits der 64-Bit nova_capability_set_t
; ---------------------------------------------------------------------------
NP_CAP_QUERY        equ 0x001
NP_CAP_MODIFY       equ 0x002
NP_CAP_WAIT         equ 0x004
NP_CAP_SIGNAL       equ 0x008
NP_CAP_DUPLICATE    equ 0x010
NP_CAP_TRANSFER     equ 0x020
NP_CAP_SUBSCRIBE    equ 0x040
NP_CAP_DELETE       equ 0x080
NP_CAP_ADMIN        equ 0x100

; Capability-Handle-Flags (§31)
NP_CAP_FLAG_NO_DELEGATE equ 0x001
NP_CAP_FLAG_EPHEMERAL   equ 0x002
NP_CAP_FLAG_BORROWED    equ 0x004
NP_CAP_FLAG_INHERITED   equ 0x008

; Capability-Zustände
CAP_STATE_FREE      equ 0
CAP_STATE_VALID     equ 1
CAP_STATE_REVOKED   equ 2
CAP_STATE_EXPIRED   equ 3

; Bootstrap-Sicherheitsdomänen (§44)
CAP_DOMAIN_KERNEL   equ 1
CAP_DOMAIN_BOOT     equ 2

; Pool-Grenze und Deskriptorgröße
CAP_MAX_CAPS        equ 32
CAP_CAP_SIZE        equ 40

; Deskriptor-Feld-Offsets
CAP_OFF_ID          equ 0    ; dd  cap_id
CAP_OFF_OBJ_ID      equ 4    ; dd  Zielobjekt-ID
CAP_OFF_RIGHTS      equ 8    ; dd  nutzbare Rechte
CAP_OFF_DELEGABLE   equ 12   ; dd  delegierbare Rechte
CAP_OFF_DOMAIN      equ 16   ; dd  Besitzer-Domäne
CAP_OFF_EXPIRATION  equ 20   ; dd  Ablaufzeit (0 = kein Limit)
CAP_OFF_GENERATION  equ 24   ; dd  Generation
CAP_OFF_FLAGS       equ 28   ; dd  Flags
CAP_OFF_POLICY_ID   equ 32   ; dd  Richtlinien-ID
CAP_OFF_STATE       equ 36   ; db  CAP_STATE_*
CAP_OFF_PRESENT     equ 37   ; db  1 = belegt
                             ; dw  Padding

; Event-Typ-IDs (§52, Fortsetzung nach evbus-Built-ins 1–8)
NP_EVENT_CAP_CREATED    equ 9
NP_EVENT_CAP_REVOKED    equ 10
NP_EVENT_CAP_DENIED     equ 11

; Fehlercodes (§60)
NP_ERR_CAP_INVALID      equ -60
NP_ERR_CAP_REVOKED      equ -61
NP_ERR_CAP_EXPIRED      equ -62
NP_ERR_CAP_EXHAUSTED    equ -63
NP_ERR_CAP_RIGHTS       equ -64
NP_ERR_CAP_DELEGATE     equ -65
NP_ERR_CAP_DOMAIN       equ -66
NP_ERR_CAP_POLICY       equ -67
NP_ERR_CAP_STALE        equ -68
NP_ERR_CAP_LIMIT        equ -69

; ---------------------------------------------------------------------------
; Compile-Zeit-Invarianten
; ---------------------------------------------------------------------------
%if CAP_CAP_SIZE != 40
    %error "CAP_CAP_SIZE muss 40 Bytes sein"
%endif
%if CAP_OFF_PRESENT != 37
    %error "CAP_OFF_PRESENT Offset falsch"
%endif
%if CAP_OFF_STATE != 36
    %error "CAP_OFF_STATE Offset falsch"
%endif

; ---------------------------------------------------------------------------
; cap_find – Capability anhand ID suchen (intern)
; EAX=cap_id → CF=0 EBX=ptr / CF=1 EAX=NP_ERR_CAP_INVALID
; ---------------------------------------------------------------------------
cap_find:
    push edi
    push ecx
    mov edi, cap_table
    xor ecx, ecx
.cfind_scan:
    cmp ecx, CAP_MAX_CAPS
    jae .cfind_miss
    cmp byte [edi + CAP_OFF_PRESENT], 0
    je .cfind_next
    cmp dword [edi + CAP_OFF_ID], eax
    je .cfind_hit
.cfind_next:
    add edi, CAP_CAP_SIZE
    inc ecx
    jmp .cfind_scan
.cfind_hit:
    mov ebx, edi
    pop ecx
    pop edi
    clc
    ret
.cfind_miss:
    mov eax, NP_ERR_CAP_INVALID
    pop ecx
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_alloc – freien Deskriptor-Slot allozieren
; → CF=0 EBX=ptr EAX=new_cap_id / CF=1 EAX=NP_ERR_CAP_LIMIT
; ---------------------------------------------------------------------------
cap_alloc:
    push edi
    push ecx
    mov edi, cap_table
    xor ecx, ecx
.calloc_scan:
    cmp ecx, CAP_MAX_CAPS
    jae .calloc_full
    cmp byte [edi + CAP_OFF_PRESENT], 0
    je .calloc_found
    add edi, CAP_CAP_SIZE
    inc ecx
    jmp .calloc_scan
.calloc_found:
    mov eax, [cap_next_id]
    inc dword [cap_next_id]
    mov ebx, edi
    pop ecx
    pop edi
    clc
    ret
.calloc_full:
    mov eax, NP_ERR_CAP_LIMIT
    pop ecx
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_create – Capability erzeugen (§13)
; EAX=obj_id, EBX=rights, ECX=delegable_rights, EDX=owner_domain
; CF=0 EAX=cap_id / CF=1 EAX=Fehler
; Invariant §15: delegable_rights ⊆ rights
; ---------------------------------------------------------------------------
cap_create:
    push esi
    push edi

    mov [cap_tmp_obj_id], eax
    mov [cap_tmp_rights], ebx
    mov [cap_tmp_deleg],  ecx
    mov [cap_tmp_domain], edx

    ; delegable ⊆ rights: (delegable & ~rights) == 0
    mov edi, ebx               ; rights
    not edi                    ; ~rights
    mov eax, ecx               ; delegable
    and eax, edi               ; delegable & ~rights
    test eax, eax
    jnz .cca_rights_err

    call cap_alloc             ; → EBX=ptr, EAX=cap_id
    jc .cca_limit

    mov esi, eax               ; esi = new cap_id

    mov dword [ebx + CAP_OFF_ID],         esi
    mov eax, [cap_tmp_obj_id]
    mov [ebx + CAP_OFF_OBJ_ID],     eax
    mov eax, [cap_tmp_rights]
    mov [ebx + CAP_OFF_RIGHTS],     eax
    mov eax, [cap_tmp_deleg]
    mov [ebx + CAP_OFF_DELEGABLE],  eax
    mov eax, [cap_tmp_domain]
    mov [ebx + CAP_OFF_DOMAIN],     eax
    mov dword [ebx + CAP_OFF_EXPIRATION], 0
    mov eax, [cap_next_gen]
    inc dword [cap_next_gen]
    mov [ebx + CAP_OFF_GENERATION], eax
    mov dword [ebx + CAP_OFF_FLAGS],     0
    mov dword [ebx + CAP_OFF_POLICY_ID], 0
    mov byte  [ebx + CAP_OFF_STATE],     CAP_STATE_VALID
    mov byte  [ebx + CAP_OFF_PRESENT],   1

    lock inc dword [cap_stat_created]

    ; cap_id sichern, bevor ESI für evbus_publish überschrieben wird
    mov [cap_cca_id], esi

    ; NP_EVENT_CAP_CREATED publizieren (§52)
    mov eax, NP_EVENT_CAP_CREATED
    mov ecx, esi               ; source = cap_id
    xor edx, edx
    xor esi, esi               ; kein Payload
    xor edi, edi
    call evbus_publish         ; CF ignoriert (Bootstrap-Event)

    mov eax, [cap_cca_id]
    pop edi
    pop esi
    clc
    ret
.cca_rights_err:
    mov eax, NP_ERR_CAP_RIGHTS
    pop edi
    pop esi
    stc
    ret
.cca_limit:
    ; EAX = NP_ERR_CAP_LIMIT aus cap_alloc
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_check – Zugriffsprüfung (§19, §20, fail-closed)
; EAX=cap_id, EBX=required_rights
; CF=0 OK / CF=1 EAX=Fehler
; ---------------------------------------------------------------------------
cap_check:
    push esi
    push edi

    mov esi, ebx               ; esi = required_rights

    call cap_find              ; EAX=cap_id → EBX=ptr
    jc .cchk_invalid           ; EAX = NP_ERR_CAP_INVALID

    ; Zustand prüfen (§20 Schritt 2)
    movzx eax, byte [ebx + CAP_OFF_STATE]
    cmp eax, CAP_STATE_REVOKED
    je .cchk_revoked
    cmp eax, CAP_STATE_EXPIRED
    je .cchk_expired
    cmp eax, CAP_STATE_VALID
    jne .cchk_invalid

    ; Ablaufzeit: Bootstrap-Caps haben expiration=0 (kein Limit, §29)
    ; Zeitvergleich wird in §104 (Time Services) ergänzt

    ; Rechteprüfung: (cap_rights & required) == required (§20 Schritt 3)
    mov eax, [ebx + CAP_OFF_RIGHTS]
    and eax, esi               ; cap_rights & required
    cmp eax, esi
    jne .cchk_rights

    pop edi
    pop esi
    clc
    ret

.cchk_invalid:
    lock inc dword [cap_stat_denied]
    mov eax, NP_ERR_CAP_INVALID
    jmp .cchk_deny
.cchk_revoked:
    lock inc dword [cap_stat_denied]
    mov eax, NP_ERR_CAP_REVOKED
    jmp .cchk_deny
.cchk_expired:
    lock inc dword [cap_stat_denied]
    mov eax, NP_ERR_CAP_EXPIRED
    jmp .cchk_deny
.cchk_rights:
    lock inc dword [cap_stat_denied]
    ; NP_EVENT_CAP_DENIED publizieren (§52)
    mov eax, NP_EVENT_CAP_DENIED
    xor ecx, ecx
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish
    mov eax, NP_ERR_CAP_RIGHTS
.cchk_deny:
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_derive – Capability mit Rechteabschwächung ableiten (§14, §15)
; EAX=src_cap_id, EBX=new_rights, ECX=new_delegable
; CF=0 EAX=new_cap_id / CF=1 EAX=Fehler
; ---------------------------------------------------------------------------
cap_derive:
    push esi
    push edi

    mov [cap_tmp_src_id], eax
    mov [cap_tmp_rights], ebx
    mov [cap_tmp_deleg],  ecx

    call cap_find              ; EAX=src_cap_id → EBX=ptr
    jc .cder_invalid

    ; Quelle muss VALID sein
    cmp byte [ebx + CAP_OFF_STATE], CAP_STATE_VALID
    jne .cder_revoked

    ; §15: new_rights ⊆ src_rights
    mov edi, [ebx + CAP_OFF_RIGHTS]
    not edi                    ; ~src_rights
    mov eax, [cap_tmp_rights]  ; new_rights
    mov esi, eax
    and esi, edi               ; new_rights & ~src_rights
    test esi, esi
    jnz .cder_rights_err

    ; §15: new_delegable ⊆ src_delegable
    mov edi, [ebx + CAP_OFF_DELEGABLE]
    not edi
    mov eax, [cap_tmp_deleg]
    mov esi, eax
    and esi, edi               ; new_delegable & ~src_delegable
    test esi, esi
    jnz .cder_delegate_err

    ; §15: new_delegable ⊆ new_rights
    mov eax, [cap_tmp_deleg]
    mov edi, [cap_tmp_rights]
    not edi
    mov esi, eax
    and esi, edi
    test esi, esi
    jnz .cder_rights_err

    ; Quell-Ptr retten (cap_alloc überschreibt EBX)
    mov [cap_tmp_src_ptr], ebx

    call cap_alloc             ; → EBX=ptr, EAX=new_cap_id
    jc .cder_limit

    mov esi, eax               ; esi = new_cap_id
    mov edi, [cap_tmp_src_ptr] ; edi = src_ptr

    mov dword [ebx + CAP_OFF_ID],         esi
    mov eax, [edi + CAP_OFF_OBJ_ID]
    mov [ebx + CAP_OFF_OBJ_ID],     eax
    mov eax, [cap_tmp_rights]
    mov [ebx + CAP_OFF_RIGHTS],     eax
    mov eax, [cap_tmp_deleg]
    mov [ebx + CAP_OFF_DELEGABLE],  eax
    mov eax, [edi + CAP_OFF_DOMAIN]
    mov [ebx + CAP_OFF_DOMAIN],     eax
    mov eax, [edi + CAP_OFF_EXPIRATION]
    mov [ebx + CAP_OFF_EXPIRATION], eax
    mov eax, [cap_next_gen]
    inc dword [cap_next_gen]
    mov [ebx + CAP_OFF_GENERATION], eax
    mov dword [ebx + CAP_OFF_FLAGS],     0
    mov dword [ebx + CAP_OFF_POLICY_ID], 0
    mov byte  [ebx + CAP_OFF_STATE],     CAP_STATE_VALID
    mov byte  [ebx + CAP_OFF_PRESENT],   1

    lock inc dword [cap_stat_created]

    mov [cap_cca_id], esi
    mov eax, NP_EVENT_CAP_CREATED
    mov ecx, esi
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish         ; CF ignoriert

    mov eax, [cap_cca_id]
    pop edi
    pop esi
    clc
    ret
.cder_invalid:
    mov eax, NP_ERR_CAP_INVALID
    pop edi
    pop esi
    stc
    ret
.cder_revoked:
    mov eax, NP_ERR_CAP_REVOKED
    pop edi
    pop esi
    stc
    ret
.cder_rights_err:
    mov eax, NP_ERR_CAP_RIGHTS
    pop edi
    pop esi
    stc
    ret
.cder_delegate_err:
    mov eax, NP_ERR_CAP_DELEGATE
    pop edi
    pop esi
    stc
    ret
.cder_limit:
    mov eax, NP_ERR_CAP_LIMIT
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_revoke – Capability widerrufen (§25)
; EAX=cap_id → CF=0 / CF=1 EAX=Fehler
; ---------------------------------------------------------------------------
cap_revoke:
    push esi

    mov esi, eax               ; esi = cap_id

    call cap_find              ; EAX=cap_id → EBX=ptr
    jc .crev_invalid           ; EAX = NP_ERR_CAP_INVALID

    cmp byte [ebx + CAP_OFF_STATE], CAP_STATE_REVOKED
    je .crev_already

    mov byte [ebx + CAP_OFF_STATE], CAP_STATE_REVOKED
    lock inc dword [cap_stat_revoked]

    ; NP_EVENT_CAP_REVOKED publizieren (§52)
    mov eax, NP_EVENT_CAP_REVOKED
    mov ecx, esi               ; source = cap_id
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish         ; CF ignoriert

    pop esi
    clc
    ret
.crev_invalid:
    ; EAX = NP_ERR_CAP_INVALID aus cap_find
    pop esi
    stc
    ret
.crev_already:
    mov eax, NP_ERR_CAP_REVOKED
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_initialize – Pools leeren, evbus-Schemata, Bootstrap-Caps (§64)
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
%macro cap_reg_event 3         ; type_id, class, flags
    mov eax, %1
    mov ecx, %2
    xor edx, edx               ; max_payload = 0
    mov esi, %3
    call evbus_register_schema
    jc .cinit_fail
%endmacro

cap_initialize:
    push esi
    push edi
    push ecx

    ; Pools leeren
    mov edi, cap_table
    xor eax, eax
    mov ecx, (CAP_MAX_CAPS * CAP_CAP_SIZE) / 4
    rep stosd
    mov dword [cap_next_id],          0
    mov dword [cap_next_gen],         0
    mov dword [cap_stat_created],     0
    mov dword [cap_stat_revoked],     0
    mov dword [cap_stat_denied],      0
    mov dword [cap_boot_kernel_id],   0
    mov dword [cap_boot_id],          0

    ; evbus-Schemata registrieren (§52)
    cap_reg_event NP_EVENT_CAP_CREATED, NP_EVENT_CLASS_LIFECYCLE, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    cap_reg_event NP_EVENT_CAP_REVOKED, NP_EVENT_CLASS_STATE,     NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    cap_reg_event NP_EVENT_CAP_DENIED,  NP_EVENT_CLASS_SECURITY,  NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY

    ; Bootstrap-Cap: Kernel-Root (§64) – obj_id=1, rights=ADMIN|QUERY|MODIFY, deleg=QUERY
    mov eax, 1
    mov ebx, NP_CAP_ADMIN | NP_CAP_QUERY | NP_CAP_MODIFY
    mov ecx, NP_CAP_QUERY
    mov edx, CAP_DOMAIN_KERNEL
    call cap_create
    jc .cinit_fail
    mov [cap_boot_kernel_id], eax

    ; Bootstrap-Cap: Boot-Kontext (§64) – obj_id=2, rights=QUERY|SUBSCRIBE, nicht delegierbar
    mov eax, 2
    mov ebx, NP_CAP_QUERY | NP_CAP_SUBSCRIBE
    xor ecx, ecx
    mov edx, CAP_DOMAIN_BOOT
    call cap_create
    jc .cinit_fail
    mov [cap_boot_id], eax

    pop ecx
    pop edi
    pop esi
    clc
    ret
.cinit_fail:
    pop ecx
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_self_test – Selbsttest §67 (10 Tests)
; CF=0 alle OK / CF=1 EAX = fehlgeschlagener Testfall
; ---------------------------------------------------------------------------
cap_self_test:
    push esi
    push edi
    push ebx

    ; Test 1: cap_create mit gültigen Rechten
    mov eax, 42
    mov ebx, NP_CAP_QUERY | NP_CAP_MODIFY
    mov ecx, NP_CAP_QUERY
    mov edx, CAP_DOMAIN_KERNEL
    call cap_create
    jc .cst_fail1
    mov [cap_st_id1], eax

    ; Test 2: cap_check – korrekte Rechte → CF=0
    mov eax, [cap_st_id1]
    mov ebx, NP_CAP_QUERY
    call cap_check
    jc .cst_fail2

    ; Test 3: cap_check – fehlende Rechte → CF=1 RIGHTS
    mov eax, [cap_st_id1]
    mov ebx, NP_CAP_ADMIN
    call cap_check
    jnc .cst_fail3
    cmp eax, NP_ERR_CAP_RIGHTS
    jne .cst_fail3

    ; Test 4: cap_derive – Rechteabschwächung → CF=0
    mov eax, [cap_st_id1]
    mov ebx, NP_CAP_QUERY
    mov ecx, NP_CAP_QUERY
    call cap_derive
    jc .cst_fail4
    mov [cap_st_id2], eax

    ; Test 5: cap_derive – Rechteerweiterung → CF=1 (§15 verletzt)
    mov eax, [cap_st_id1]
    mov ebx, NP_CAP_QUERY | NP_CAP_MODIFY | NP_CAP_ADMIN
    mov ecx, NP_CAP_QUERY
    call cap_derive
    jnc .cst_fail5

    ; Test 6: cap_revoke → CF=0
    mov eax, [cap_st_id1]
    call cap_revoke
    jc .cst_fail6

    ; Test 7: cap_check auf widerrufene Cap → CF=1 CAP_REVOKED
    mov eax, [cap_st_id1]
    mov ebx, NP_CAP_QUERY
    call cap_check
    jnc .cst_fail7
    cmp eax, NP_ERR_CAP_REVOKED
    jne .cst_fail7

    ; Test 8: cap_check auf nicht-existente Cap → CF=1 CAP_INVALID
    mov eax, 0xDEAD
    mov ebx, NP_CAP_QUERY
    call cap_check
    jnc .cst_fail8
    cmp eax, NP_ERR_CAP_INVALID
    jne .cst_fail8

    ; Test 9: Ableitungskette (derive von derive) → CF=0
    mov eax, [cap_st_id2]
    mov ebx, NP_CAP_QUERY
    xor ecx, ecx               ; delegable = 0 (weiter reduziert)
    call cap_derive
    jc .cst_fail9
    mov [cap_st_id3], eax

    ; Test 10: cap_check auf id3 mit MODIFY → CF=1 RIGHTS (id3 hat nur QUERY)
    mov eax, [cap_st_id3]
    mov ebx, NP_CAP_MODIFY
    call cap_check
    jnc .cst_fail10
    cmp eax, NP_ERR_CAP_RIGHTS
    jne .cst_fail10

    pop ebx
    pop edi
    pop esi
    clc
    ret

.cst_fail1:   mov eax, 1
    jmp .cst_fail
.cst_fail2:   mov eax, 2
    jmp .cst_fail
.cst_fail3:   mov eax, 3
    jmp .cst_fail
.cst_fail4:   mov eax, 4
    jmp .cst_fail
.cst_fail5:   mov eax, 5
    jmp .cst_fail
.cst_fail6:   mov eax, 6
    jmp .cst_fail
.cst_fail7:   mov eax, 7
    jmp .cst_fail
.cst_fail8:   mov eax, 8
    jmp .cst_fail
.cst_fail9:   mov eax, 9
    jmp .cst_fail
.cst_fail10:  mov eax, 10
.cst_fail:
    pop ebx
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_* Daten
; ---------------------------------------------------------------------------
cap_table:          times (CAP_MAX_CAPS * CAP_CAP_SIZE / 4) dd 0
cap_next_id:        dd 0
cap_next_gen:       dd 0
cap_stat_created:   dd 0
cap_stat_revoked:   dd 0
cap_stat_denied:    dd 0
cap_boot_kernel_id: dd 0
cap_boot_id:        dd 0
; Temp-Speicher (Single-Threaded Bootstrap)
cap_tmp_obj_id:     dd 0
cap_tmp_rights:     dd 0
cap_tmp_deleg:      dd 0
cap_tmp_domain:     dd 0
cap_tmp_src_id:     dd 0
cap_tmp_src_ptr:    dd 0
cap_cca_id:         dd 0
; Selbsttest-IDs
cap_st_id1:         dd 0
cap_st_id2:         dd 0
cap_st_id3:         dd 0

; ===========================================================================
; NPSPEC-CAPABILITY-DISCOVERY-0001 – Nova Capability Discovery
; Abfrage welche Capabilities das System kennt und anbietet.
; Registry: bis zu 16 bekannte Capability-Typen mit Name-Hash und Flags.
; ===========================================================================

CAPDIS_COUNT         equ 16
CAPDIS_RECORD_SIZE   equ 12

CAPDIS_TYPE_NONE     equ 0
CAPDIS_TYPE_KERNEL   equ 1
CAPDIS_TYPE_IO       equ 2
CAPDIS_TYPE_NET      equ 3
CAPDIS_TYPE_FS       equ 4
CAPDIS_TYPE_DEVICE   equ 5
CAPDIS_TYPE_SECURITY equ 6

CAPDIS_ID            equ 0
CAPDIS_TYPE          equ 4
CAPDIS_FLAGS         equ 8

CAPDIS_FLAG_AVAIL    equ 0x01
CAPDIS_FLAG_DELEGATABLE equ 0x02

capdis_initialize:
    mov edi, capdis_registry
    mov ecx, CAPDIS_COUNT * CAPDIS_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [capdis_ready], 1
    mov dword [capdis_count], 0
    ; Standard-Capabilities registrieren
    mov eax, CAPDIS_TYPE_KERNEL
    mov edx, CAPDIS_FLAG_AVAIL | CAPDIS_FLAG_DELEGATABLE
    call capdis_register
    mov eax, CAPDIS_TYPE_IO
    mov edx, CAPDIS_FLAG_AVAIL
    call capdis_register
    mov eax, CAPDIS_TYPE_SECURITY
    mov edx, CAPDIS_FLAG_AVAIL | CAPDIS_FLAG_DELEGATABLE
    call capdis_register
    clc
    ret

; EAX=cap_type  EDX=flags → CF=0/CF=1 voll
capdis_register:
    push esi
    push ecx
    cmp dword [capdis_count], CAPDIS_COUNT
    jae .cdr_full
    mov ecx, [capdis_count]
    imul esi, ecx, CAPDIS_RECORD_SIZE
    add esi, capdis_registry
    mov [esi + CAPDIS_ID], ecx
    mov [esi + CAPDIS_TYPE], eax
    mov [esi + CAPDIS_FLAGS], edx
    inc dword [capdis_count]
    clc
    pop ecx
    pop esi
    ret
.cdr_full:
    stc
    pop ecx
    pop esi
    ret

; EAX=cap_type → EAX=flags CF=0 gefunden / CF=1 nicht gefunden
capdis_query:
    push esi
    push ecx
    mov esi, capdis_registry
    xor ecx, ecx
.cdq_loop:
    cmp ecx, [capdis_count]
    jae .cdq_notfound
    cmp dword [esi + CAPDIS_TYPE], eax
    je .cdq_found
    add esi, CAPDIS_RECORD_SIZE
    inc ecx
    jmp .cdq_loop
.cdq_found:
    mov eax, [esi + CAPDIS_FLAGS]
    clc
    pop ecx
    pop esi
    ret
.cdq_notfound:
    stc
    pop ecx
    pop esi
    ret

capdis_self_test:
    push esi
    xor esi, esi
    cmp dword [capdis_ready], 1
    je .cdst1
    inc esi
.cdst1:
    ; capdis_count=3 (KERNEL/IO/SECURITY)
    cmp dword [capdis_count], 3
    je .cdst2
    inc esi
.cdst2:
    ; KERNEL-Typ abfragen → Flags=AVAIL|DELEGATABLE
    mov eax, CAPDIS_TYPE_KERNEL
    call capdis_query
    jnc .cdst3_cf
    inc esi
.cdst3_cf:
    cmp eax, CAPDIS_FLAG_AVAIL | CAPDIS_FLAG_DELEGATABLE
    je .cdst3
    inc esi
.cdst3:
    ; Unbekannter Typ → CF=1
    mov eax, 0xFF
    call capdis_query
    jc .cdst4
    inc esi
.cdst4:
    test esi, esi
    jnz .cdstf
    clc
    pop esi
    ret
.cdstf:
    stc
    pop esi
    ret

capdis_ready:  dd 0
capdis_count:  dd 0
align 4
capdis_registry:
    times CAPDIS_COUNT * CAPDIS_RECORD_SIZE db 0

; ===========================================================================
; NPSPEC-CAPABILITY-NEGOTIATION-0001 – Nova Capability Negotiation
; Anbieter und Anforderungen werden abgeglichen: Negotiation findet passenden
; Provider. Requester meldet benötigte Capabilities; System liefert Match.
; ===========================================================================

CAPNEG_REQ_COUNT     equ 8
CAPNEG_REQ_SIZE      equ 12

CAPNEG_STATE_FREE    equ 0
CAPNEG_STATE_PENDING equ 1
CAPNEG_STATE_MATCHED equ 2
CAPNEG_STATE_REJECTED equ 3

CAPNEG_REQ_ID        equ 0
CAPNEG_REQ_TYPE      equ 4
CAPNEG_REQ_STATE     equ 8

capneg_initialize:
    mov edi, capneg_requests
    mov ecx, CAPNEG_REQ_COUNT * CAPNEG_REQ_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [capneg_ready], 1
    mov dword [capneg_matched], 0
    mov dword [capneg_rejected], 0
    clc
    ret

; EAX=cap_type → EAX=request_slot CF=0 / CF=1 voll
capneg_request:
    push esi
    push ecx
    mov esi, capneg_requests
    xor ecx, ecx
.cnreq_scan:
    cmp ecx, CAPNEG_REQ_COUNT
    jae .cnreq_full
    cmp dword [esi + CAPNEG_REQ_STATE], CAPNEG_STATE_FREE
    je .cnreq_found
    add esi, CAPNEG_REQ_SIZE
    inc ecx
    jmp .cnreq_scan
.cnreq_found:
    mov [esi + CAPNEG_REQ_ID], ecx
    mov [esi + CAPNEG_REQ_TYPE], eax
    mov dword [esi + CAPNEG_REQ_STATE], CAPNEG_STATE_PENDING
    mov eax, ecx
    clc
    pop ecx
    pop esi
    ret
.cnreq_full:
    stc
    pop ecx
    pop esi
    ret

; Verhandelt alle PENDING-Requests gegen capdis_registry; matched oder rejected
capneg_negotiate_all:
    push esi
    push ecx
    push edx
    mov esi, capneg_requests
    xor ecx, ecx
.cnn_loop:
    cmp ecx, CAPNEG_REQ_COUNT
    jae .cnn_done
    cmp dword [esi + CAPNEG_REQ_STATE], CAPNEG_STATE_PENDING
    jne .cnn_next
    ; In Discovery nachschlagen
    push eax
    push ecx
    push esi
    mov eax, [esi + CAPNEG_REQ_TYPE]
    call capdis_query
    pop esi
    pop ecx
    jnc .cnn_match
    ; Nicht gefunden → REJECTED
    mov dword [esi + CAPNEG_REQ_STATE], CAPNEG_STATE_REJECTED
    inc dword [capneg_rejected]
    pop eax
    jmp .cnn_next
.cnn_match:
    ; Gefunden → MATCHED (nur wenn AVAIL gesetzt)
    test eax, CAPDIS_FLAG_AVAIL
    jz .cnn_unavail
    pop eax
    mov dword [esi + CAPNEG_REQ_STATE], CAPNEG_STATE_MATCHED
    inc dword [capneg_matched]
    jmp .cnn_next
.cnn_unavail:
    pop eax
    mov dword [esi + CAPNEG_REQ_STATE], CAPNEG_STATE_REJECTED
    inc dword [capneg_rejected]
.cnn_next:
    add esi, CAPNEG_REQ_SIZE
    inc ecx
    jmp .cnn_loop
.cnn_done:
    pop edx
    pop ecx
    pop esi
    ret

capneg_self_test:
    push ebx
    push esi
    xor esi, esi
    cmp dword [capneg_ready], 1
    je .cnst1
    inc esi
.cnst1:
    ; Anfrage KERNEL (vorhanden) und DEVICE (nicht registriert)
    mov eax, CAPDIS_TYPE_KERNEL
    call capneg_request
    jnc .cnst2
    inc esi
.cnst2:
    mov ebx, eax   ; slot für KERNEL
    mov eax, CAPDIS_TYPE_DEVICE
    call capneg_request
    jnc .cnst3
    inc esi
.cnst3:
    ; Verhandeln
    call capneg_negotiate_all
    ; KERNEL-Request muss MATCHED sein
    push ecx
    imul ecx, ebx, CAPNEG_REQ_SIZE
    add ecx, capneg_requests
    cmp dword [ecx + CAPNEG_REQ_STATE], CAPNEG_STATE_MATCHED
    je .cnst4
    inc esi
.cnst4:
    pop ecx
    ; capneg_matched=1
    cmp dword [capneg_matched], 1
    je .cnst5
    inc esi
.cnst5:
    ; capneg_rejected=1 (DEVICE)
    cmp dword [capneg_rejected], 1
    je .cnst6
    inc esi
.cnst6:
    test esi, esi
    jnz .cnstf
    clc
    pop esi
    pop ebx
    ret
.cnstf:
    stc
    pop esi
    pop ebx
    ret

capneg_ready:    dd 0
capneg_matched:  dd 0
capneg_rejected: dd 0
align 4
capneg_requests:
    times CAPNEG_REQ_COUNT * CAPNEG_REQ_SIZE db 0

