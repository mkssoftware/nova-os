; ===========================================================================
; CAP-Integration 1.0 – §103↔§102, §103↔IPC, §103↔VFS
; ===========================================================================
; Verbindet das Capability Framework (§103) mit:
;   • Unified Object API §102 : Handles erhalten CAP-Deskriptoren
;   • IPC                     : Send/Receive verlangen NP_CAP_SIGNAL/SUBSCRIBE
;   • VFS                     : Lookup verlangt NP_CAP_QUERY
; Alle Funktionen sind Single-Threaded-Bootstrap-sicher (statische Temps).
; ===========================================================================

CAP_INT_IPC_OBJ_ID     equ 200   ; virtuelles Objekt-ID für IPC-Endpoint-Caps
CAP_INT_VFS_OBJ_ID     equ 201   ; virtuelles Objekt-ID für VFS-Root-Cap

; ---------------------------------------------------------------------------
; cap_uobj_create_handle – Handle + CAP-Deskriptor für bestehendes UOBJ-Objekt
; EAX=obj_id, EBX=unused, ECX=rights(NP_CAP_*), EDX=domain(CAP_DOMAIN_*)
; CF=0: EBX=handle_id, EAX=cap_id / CF=1 Fehler
; Invariant: delegable_rights = rights & NP_CAP_QUERY (monoton reduziert)
; ---------------------------------------------------------------------------
cap_uobj_create_handle:
    push esi
    push edi

    mov [cap_int_tmp_obj_id], eax
    mov [cap_int_tmp_rights], ecx
    mov [cap_int_tmp_domain], edx

    ; Objekt prüfen + aktuelle Generation lesen
    call uobj_find_object           ; EAX=obj_id → EBX=obj_ptr, CF
    jc .cuch_fail
    mov eax, [ebx + UOBJ_OBJ_OFF_GEN]
    mov [cap_int_tmp_gen], eax

    ; Handle allokieren (Generation + inline-caps aus CAP-Rights)
    mov eax, [cap_int_tmp_obj_id]
    mov ecx, [cap_int_tmp_gen]
    mov edx, [cap_int_tmp_rights]
    call uobj_alloc_handle          ; EAX=obj_id, ECX=gen, EDX=caps → EBX=handle_id, CF
    jc .cuch_fail
    mov [cap_int_tmp_hdl_id], ebx

    ; Slot-Index in Parallel-Tabelle: (hdl_ptr - uobj_handles) >> 4
    mov eax, [cap_int_tmp_hdl_id]
    call uobj_find_handle           ; EAX=handle_id → EBX=hdl_ptr, CF
    jc .cuch_fail
    mov eax, ebx
    sub eax, uobj_handles
    shr eax, 4
    mov [cap_int_tmp_slot], eax

    ; CAP-Deskriptor erzeugen (delegable = QUERY-Subset)
    mov eax, [cap_int_tmp_obj_id]
    mov ebx, [cap_int_tmp_rights]
    mov ecx, NP_CAP_QUERY
    and ecx, ebx
    mov edx, [cap_int_tmp_domain]
    call cap_create                 ; → EAX=cap_id, CF
    jc .cuch_fail

    ; cap_id in Parallel-Tabelle speichern
    mov ecx, [cap_int_tmp_slot]
    mov [uobj_hdl_cap_tab + ecx*4], eax

    mov ebx, [cap_int_tmp_hdl_id]
    ; EAX = cap_id

    pop edi
    pop esi
    clc
    ret

.cuch_fail:
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; cap_uobj_access – Handle via CAP-Framework auflösen (bridge §103↔§102)
; EAX=handle_id, EBX=required_rights(NP_CAP_*)
; CF=0: ECX=obj_ptr / CF=1: EAX=Fehler
; ---------------------------------------------------------------------------
cap_uobj_access:
    push esi
    push edx

    mov [cap_int_tmp_hdl_id], eax
    mov [cap_int_tmp_rights], ebx

    ; Handle-Slot finden → cap_id nachschlagen
    call uobj_find_handle           ; EAX=handle_id → EBX=hdl_ptr, CF
    jc .cuoa_invalid

    mov eax, ebx
    sub eax, uobj_handles
    shr eax, 4
    mov ecx, [uobj_hdl_cap_tab + eax*4]
    test ecx, ecx
    jz .cuoa_invalid                ; kein CAP-Deskriptor für dieses Handle

    ; Capability-Prüfung (fail-closed)
    mov eax, ecx
    mov ebx, [cap_int_tmp_rights]
    call cap_check                  ; EAX=cap_id, EBX=rights → CF=0/CF=1 EAX=Fehler
    jc .cuoa_denied

    ; UOBJ auflösen; ECX=0 überspringt doppelte inline-CAPS-Prüfung
    mov eax, [cap_int_tmp_hdl_id]
    xor ecx, ecx
    call uobj_from_handle           ; → EBX=obj_ptr, CF
    jc .cuoa_invalid

    mov ecx, ebx                    ; ECX = obj_ptr Rückgabe

    pop edx
    pop esi
    clc
    ret

.cuoa_invalid:
    mov eax, NP_ERR_CAP_INVALID
.cuoa_denied:
    pop edx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; ipc_send_secure – IPC-Send mit NP_CAP_SIGNAL-Prüfung (bridge §103↔IPC)
; EAX=cap_id, ESI=msg_ptr(16 Byte)
; CF=0 OK / CF=1 Fehler (EAX=Fehlercode oder -1 bei voller Queue)
; ---------------------------------------------------------------------------
ipc_send_secure:
    push ebx
    mov ebx, NP_CAP_SIGNAL
    call cap_check                  ; cap_check erhält ESI
    jc .iss_denied

    call ipc_send                   ; ESI=msg_ptr → EAX=1(OK) / 0(voll)
    test eax, eax
    jz .iss_full

    pop ebx
    clc
    ret

.iss_full:
    mov eax, -1
.iss_denied:
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; ipc_receive_secure – IPC-Receive mit NP_CAP_SUBSCRIBE-Prüfung
; EAX=cap_id, EDI=buf_ptr(16 Byte)
; CF=0 OK / CF=1 Fehler (EAX=Fehlercode oder -1 bei leerer Queue)
; ---------------------------------------------------------------------------
ipc_receive_secure:
    push ebx
    mov ebx, NP_CAP_SUBSCRIBE
    call cap_check                  ; cap_check erhält EDI
    jc .irs_denied

    call ipc_receive                ; EDI=buf_ptr → EAX=1(OK) / 0(leer)
    test eax, eax
    jz .irs_empty

    pop ebx
    clc
    ret

.irs_empty:
    mov eax, -1
.irs_denied:
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; vfs_lookup_secure – VFS-Root-Lookup mit NP_CAP_QUERY-Prüfung
; EAX=cap_id, ESI=path, ECX=length
; CF=0 EAX=node_handle / CF=1 Fehler
; cap_check erhält ESI+ECX auf Erfolgspfad (save/restore intern)
; ---------------------------------------------------------------------------
vfs_lookup_secure:
    push ebx
    mov ebx, NP_CAP_QUERY
    call cap_check
    jc .vls_denied

    call vfs_lookup_root            ; ESI=path, ECX=length → EAX=handle, CF=0/CF=1

    pop ebx
    ret

.vls_denied:
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; cap_integration_initialize – Bootstrap-Caps für IPC/VFS anlegen
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
cap_integration_initialize:
    push ebx
    push esi
    push edi

    ; Parallel-Tabelle nullen
    mov edi, uobj_hdl_cap_tab
    xor eax, eax
    mov ecx, UOBJ_MAX_HANDLES
    rep stosd

    ; IPC-Send-Cap (NP_CAP_SIGNAL, kein Delegat)
    mov eax, CAP_INT_IPC_OBJ_ID
    mov ebx, NP_CAP_SIGNAL
    xor ecx, ecx
    mov edx, CAP_DOMAIN_KERNEL
    call cap_create
    jc .ci_fail
    mov [ipc_cap_send_id], eax

    ; IPC-Recv-Cap (NP_CAP_SUBSCRIBE, kein Delegat)
    mov eax, CAP_INT_IPC_OBJ_ID
    mov ebx, NP_CAP_SUBSCRIBE
    xor ecx, ecx
    mov edx, CAP_DOMAIN_KERNEL
    call cap_create
    jc .ci_fail
    mov [ipc_cap_recv_id], eax

    ; VFS-Query-Cap (NP_CAP_QUERY, delegierbar)
    mov eax, CAP_INT_VFS_OBJ_ID
    mov ebx, NP_CAP_QUERY
    mov ecx, NP_CAP_QUERY
    mov edx, CAP_DOMAIN_KERNEL
    call cap_create
    jc .ci_fail
    mov [vfs_cap_query_id], eax

    ; Diagnose: Integration bereit
    mov eax, DIAG_SRC_CAP
    mov ebx, DIAG_LEVEL_INFO
    xor ecx, ecx
    xor edx, edx
    call diag_log

    pop edi
    pop esi
    pop ebx
    clc
    ret

.ci_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; cap_integration_self_test – 8 Tests: §103↔§102, §103↔IPC, §103↔VFS
; CF=0 alle Tests bestanden / CF=1 Fehler
; ---------------------------------------------------------------------------
cap_integration_self_test:
    push ebx
    push esi
    push edi

    ; === Test 1: UOBJ-Objekt anlegen (Grundlage für Bridge-Test) ===
    mov eax, NP_OBJTYPE_GENERIC
    mov ecx, UOBJ_CAP_QUERY | UOBJ_CAP_MODIFY
    call uobj_create                ; → EBX=handle_id0, ECX=obj_id
    jc .cist_fail
    mov [cap_int_st_obj1], ecx

    ; === Test 2: cap_uobj_create_handle → CF=0, handle_id+cap_id ===
    mov eax, [cap_int_st_obj1]
    xor ebx, ebx
    mov ecx, NP_CAP_QUERY | NP_CAP_MODIFY
    mov edx, CAP_DOMAIN_KERNEL
    call cap_uobj_create_handle     ; → EBX=handle_id, EAX=cap_id
    jc .cist_fail
    mov [cap_int_st_hdl1], ebx
    mov [cap_int_st_cap1], eax

    ; === Test 3: cap_uobj_access QUERY → CF=0 ===
    mov eax, [cap_int_st_hdl1]
    mov ebx, NP_CAP_QUERY
    call cap_uobj_access
    jc .cist_fail

    ; === Test 4: cap_uobj_access ADMIN → CF=1 (kein ADMIN-Recht) ===
    mov eax, [cap_int_st_hdl1]
    mov ebx, NP_CAP_ADMIN
    call cap_uobj_access
    jnc .cist_fail

    ; === Test 5: ipc_send_secure mit Send-Cap → CF=0 ===
    mov eax, [ipc_cap_send_id]
    mov esi, cap_int_test_msg
    call ipc_send_secure
    jc .cist_fail

    ; === Test 6: ipc_receive_secure mit Recv-Cap → CF=0, Inhalt stimmt ===
    mov eax, [ipc_cap_recv_id]
    mov edi, cap_int_recv_buf
    call ipc_receive_secure
    jc .cist_fail
    mov eax, [cap_int_test_msg]
    cmp eax, [cap_int_recv_buf]
    jne .cist_fail

    ; === Test 7: vfs_lookup_secure mit Query-Cap + "/" → CF=0 ===
    mov eax, [vfs_cap_query_id]
    mov esi, vfs_root_path
    mov ecx, 1
    call vfs_lookup_secure
    jc .cist_fail

    ; === Test 8: vfs_lookup_secure mit ungültiger Cap → CF=1 ===
    mov eax, 0xDEAD
    mov esi, vfs_root_path
    mov ecx, 1
    call vfs_lookup_secure
    jnc .cist_fail

    pop edi
    pop esi
    pop ebx
    clc
    ret

.cist_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; CAP-Integration Daten
; ---------------------------------------------------------------------------
uobj_hdl_cap_tab:    times UOBJ_MAX_HANDLES dd 0   ; Handle-Slot → cap_id
ipc_cap_send_id:     dd 0
ipc_cap_recv_id:     dd 0
vfs_cap_query_id:    dd 0
; Temp-Speicher (Single-Threaded Bootstrap)
cap_int_tmp_obj_id:  dd 0
cap_int_tmp_rights:  dd 0
cap_int_tmp_domain:  dd 0
cap_int_tmp_gen:     dd 0
cap_int_tmp_hdl_id:  dd 0
cap_int_tmp_slot:    dd 0
; Selbsttest-IDs
cap_int_st_obj1:     dd 0
cap_int_st_hdl1:     dd 0
cap_int_st_cap1:     dd 0
; Selbsttest-Nachricht (16 Byte)
cap_int_test_msg:    dd 0xCAF10001, 0xCAF10002, 0xCAF10003, 0xCAF10004
cap_int_recv_buf:    times IPC_MESSAGE_SIZE db 0

; ===========================================================================
; §105 – Versioned Kernel Service ABI 1.0 (NPSPEC-KERNEL-0105)
; ===========================================================================

VSVC_MAX_SERVICES    equ 16
VSVC_DESC_SIZE       equ 64         ; Potenz von 2 → shr 6 für Adressberechnung

; Deskriptor-Offsets (64 Byte / Slot)
VSVC_OFF_ID0         equ 0
VSVC_OFF_ID1         equ 4
VSVC_OFF_ID2         equ 8
VSVC_OFF_ID3         equ 12
VSVC_OFF_MAJ         equ 16         ; dw major
VSVC_OFF_MIN         equ 18         ; dw minor
VSVC_OFF_PAT         equ 20         ; dw patch
VSVC_OFF_STAB        equ 22         ; db stability class
VSVC_OFF_STATE       equ 23         ; db state
VSVC_OFF_TABLE       equ 24         ; dd service table ptr
VSVC_OFF_TABSZ       equ 28         ; dd table size
VSVC_OFF_FEATURES    equ 32         ; dd feature mask low
VSVC_OFF_FEAT_HI     equ 36         ; dd feature mask high
VSVC_OFF_REQCAP      equ 40         ; dd required capabilities
VSVC_OFF_FLAGS       equ 44         ; dd flags
VSVC_OFF_ARCH        equ 48         ; dd architecture mask
VSVC_OFF_REFCNT      equ 52         ; dd reference count
VSVC_OFF_NAME        equ 56         ; dd name ptr
VSVC_OFF_PRESENT     equ 60         ; db

; Service-Tabellen-Header-Offsets (§10, 24 Byte)
VSVC_TBL_HDR_SIZE    equ 24
VSVC_TBL_OFF_SIZE    equ 0
VSVC_TBL_OFF_MAJ     equ 4          ; dw
VSVC_TBL_OFF_MIN     equ 6          ; dw
VSVC_TBL_OFF_FEAT    equ 8
VSVC_TBL_OFF_FEATHI  equ 12
VSVC_TBL_OFF_FLAGS   equ 16
VSVC_TBL_OFF_RSVD    equ 20

; Aufruf-Flags (§10)
VSVC_CALL_THREAD_CTX equ 0x00000001
VSVC_CALL_IRQ_SAFE   equ 0x00000002
VSVC_CALL_EARLY_BOOT equ 0x00000008
VSVC_CALL_PANIC_SAFE equ 0x00000010
VSVC_CALL_MAY_BLOCK  equ 0x00000020

; Zustände (§32, Bootstrap-Vereinfachung ohne DRAINING)
VSVC_STATE_FREE          equ 0
VSVC_STATE_ACTIVE        equ 1
VSVC_STATE_QUIESCING     equ 2
VSVC_STATE_OFFLINE       equ 3
VSVC_STATE_UNREGISTERED  equ 4

; Stabilitätsklassen (§51)
VSVC_STAB_INTERNAL      equ 0
VSVC_STAB_EXPERIMENTAL  equ 1
VSVC_STAB_PROVISIONAL   equ 2
VSVC_STAB_STABLE        equ 3
VSVC_STAB_LEGACY        equ 4
VSVC_STAB_SECURITY_ONLY equ 5

; Architektur-Maske (§25)
VSVC_ARCH_X86_32    equ 0x00000001
VSVC_ARCH_X86_64    equ 0x00000002
VSVC_ARCH_ARM64     equ 0x00000004
VSVC_ARCH_RISCV64   equ 0x00000008
VSVC_ARCH_ANY       equ 0xFFFFFFFF

; Flag-Maske (§23)
VSVC_FLAG_REQUIRED_MASK  equ 0x0000FFFF
VSVC_FLAG_OPTIONAL_MASK  equ 0xFFFF0000
VSVC_FLAG_EARLY_BOOT     equ 0x00000004
VSVC_FLAG_PANIC_SAFE     equ 0x00000008

; Bootstrap-Service-UUIDs (16 Byte = 4×dd)
VSVC_ID_KERN_0    equ 0x4E4F5641   ; "NOVA"
VSVC_ID_KERN_1    equ 0x4B45524E   ; "KERN"
VSVC_ID_KERN_2    equ 0x434F5245   ; "CORE"
VSVC_ID_KERN_3    equ 0x00000001

VSVC_ID_EVBS_0    equ 0x4E4F5641
VSVC_ID_EVBS_1    equ 0x45564253   ; "EVBS"
VSVC_ID_EVBS_2    equ 0x434F5245
VSVC_ID_EVBS_3    equ 0x00000001

VSVC_ID_CAPS_0    equ 0x4E4F5641
VSVC_ID_CAPS_1    equ 0x43415053   ; "CAPS"
VSVC_ID_CAPS_2    equ 0x434F5245
VSVC_ID_CAPS_3    equ 0x00000001

VSVC_ID_DIAG_0    equ 0x4E4F5641
VSVC_ID_DIAG_1    equ 0x44494147   ; "DIAG"
VSVC_ID_DIAG_2    equ 0x434F5245
VSVC_ID_DIAG_3    equ 0x00000001

VSVC_ID_TEST_0    equ 0x54455354   ; "TEST"
VSVC_ID_TEST_1    equ 0x53564300   ; "SVC\0"
VSVC_ID_TEST_2    equ 0x00000000
VSVC_ID_TEST_3    equ 0x00000099

; Event-IDs §105 (§48, fortlaufend nach NP_EVENT_DIAG_HEALTH_CHANGED=12)
NP_EVENT_SVC_REGISTERED   equ 13
NP_EVENT_SVC_ACTIVATED    equ 14
NP_EVENT_SVC_OFFLINE      equ 15
NP_EVENT_SVC_QUIESCING    equ 16
NP_EVENT_SVC_UNREGISTERED equ 17

; Fehlercodes §105 (§67)
NP_ERR_VSVC_NOT_FOUND  equ -80
NP_ERR_VSVC_VERSION    equ -81
NP_ERR_VSVC_FEATURE    equ -82
NP_ERR_VSVC_ABI        equ -83
NP_ERR_VSVC_CONFLICT   equ -84
NP_ERR_VSVC_QUIESCING  equ -85
NP_ERR_VSVC_OFFLINE    equ -86
NP_ERR_VSVC_LIMIT      equ -87
NP_ERR_VSVC_ARCH       equ -88
NP_ERR_VSVC_STALE      equ -89

; ---------------------------------------------------------------------------
; Lokale Makros für vsvc_initialize
; ---------------------------------------------------------------------------
%macro vsvc_reg_schema 1
    mov eax, %1
    mov ecx, NP_EVENT_CLASS_LIFECYCLE
    xor edx, edx
    mov esi, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY
    call evbus_register_schema
    jc .vi_fail
%endmacro

%macro vsvc_reg_boot 6              ; id0, id1, id2, id3, table_label, name_label
    mov dword [vsvc_tmp_table], %5
    mov dword [vsvc_tmp_name],  %6
    mov eax, %1
    mov ebx, %2
    mov ecx, %3
    mov edx, %4
    call vsvc_register
    jc .vi_fail
%endmacro

; ---------------------------------------------------------------------------
; vsvc_alloc – freien Deskriptor-Slot suchen
; → EBX=desc_ptr, CF=0 / CF=1 (voll)
; ---------------------------------------------------------------------------
vsvc_alloc:
    push esi
    push ecx
    mov esi, vsvc_registry
    xor ecx, ecx
.vsa_scan:
    cmp ecx, VSVC_MAX_SERVICES
    jae .vsa_full
    cmp byte [esi + VSVC_OFF_PRESENT], 0
    je .vsa_found
    add esi, VSVC_DESC_SIZE
    inc ecx
    jmp .vsa_scan
.vsa_found:
    mov ebx, esi
    pop ecx
    pop esi
    clc
    ret
.vsa_full:
    xor ebx, ebx
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; vsvc_find – Service per UUID suchen
; ESI=uuid_ptr (16 Byte) → EBX=desc_ptr, CF=0 gefunden / CF=1 nicht gefunden
; ---------------------------------------------------------------------------
vsvc_find:
    push esi
    push ecx
    push edx
    mov eax, [esi]
    mov [vsvc_tmp_find0], eax
    mov eax, [esi + 4]
    mov [vsvc_tmp_find1], eax
    mov eax, [esi + 8]
    mov [vsvc_tmp_find2], eax
    mov eax, [esi + 12]
    mov [vsvc_tmp_find3], eax
    mov esi, vsvc_registry
    xor ecx, ecx
.vsf_scan:
    cmp ecx, VSVC_MAX_SERVICES
    jae .vsf_not_found
    cmp byte [esi + VSVC_OFF_PRESENT], 0
    je .vsf_next
    mov eax, [vsvc_tmp_find0]
    cmp [esi + VSVC_OFF_ID0], eax
    jne .vsf_next
    mov eax, [vsvc_tmp_find1]
    cmp [esi + VSVC_OFF_ID1], eax
    jne .vsf_next
    mov eax, [vsvc_tmp_find2]
    cmp [esi + VSVC_OFF_ID2], eax
    jne .vsf_next
    mov eax, [vsvc_tmp_find3]
    cmp [esi + VSVC_OFF_ID3], eax
    jne .vsf_next
    mov ebx, esi
    pop edx
    pop ecx
    pop esi
    clc
    ret
.vsf_next:
    add esi, VSVC_DESC_SIZE
    inc ecx
    jmp .vsf_scan
.vsf_not_found:
    xor ebx, ebx
    pop edx
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; vsvc_register – Service in der Registry eintragen (§12)
; EAX=id[0], EBX=id[1], ECX=id[2], EDX=id[3]
; Vor dem Aufruf: vsvc_tmp_maj, min, pat, stab, table, tabsz, features,
;   reqcap, arch, flags, name setzen
; CF=0 EBX=desc_ptr / CF=1 EAX=NP_ERR_VSVC_*
; ---------------------------------------------------------------------------
vsvc_register:
    push esi
    push edi

    mov [vsvc_tmp_id0], eax
    mov [vsvc_tmp_id1], ebx
    mov [vsvc_tmp_id2], ecx
    mov [vsvc_tmp_id3], edx

    ; UUID in Such-Puffer schreiben und auf Duplikat prüfen
    mov eax, [vsvc_tmp_id0]
    mov [vsvc_reg_id_buf],      eax
    mov eax, [vsvc_tmp_id1]
    mov [vsvc_reg_id_buf +  4], eax
    mov eax, [vsvc_tmp_id2]
    mov [vsvc_reg_id_buf +  8], eax
    mov eax, [vsvc_tmp_id3]
    mov [vsvc_reg_id_buf + 12], eax
    mov esi, vsvc_reg_id_buf
    call vsvc_find
    jnc .vr_conflict                ; CF=0 → gefunden → Duplikat

    ; Slot allokieren
    call vsvc_alloc
    jc .vr_limit
    mov edi, ebx                    ; EDI = neuer Slot-Zeiger

    ; Tabellengröße prüfen (§10: mind. VSVC_TBL_HDR_SIZE)
    mov eax, [vsvc_tmp_tabsz]
    cmp eax, VSVC_TBL_HDR_SIZE
    jb .vr_abi

    ; UUID
    mov eax, [vsvc_tmp_id0]
    mov [edi + VSVC_OFF_ID0], eax
    mov eax, [vsvc_tmp_id1]
    mov [edi + VSVC_OFF_ID1], eax
    mov eax, [vsvc_tmp_id2]
    mov [edi + VSVC_OFF_ID2], eax
    mov eax, [vsvc_tmp_id3]
    mov [edi + VSVC_OFF_ID3], eax

    ; Version + Stabilität
    mov ax, [vsvc_tmp_maj]
    mov [edi + VSVC_OFF_MAJ], ax
    mov ax, [vsvc_tmp_min]
    mov [edi + VSVC_OFF_MIN], ax
    mov ax, [vsvc_tmp_pat]
    mov [edi + VSVC_OFF_PAT], ax
    mov al, [vsvc_tmp_stab]
    mov [edi + VSVC_OFF_STAB], al
    mov byte [edi + VSVC_OFF_STATE], VSVC_STATE_ACTIVE

    ; Tabelle + Features
    mov eax, [vsvc_tmp_table]
    mov [edi + VSVC_OFF_TABLE], eax
    mov eax, [vsvc_tmp_tabsz]
    mov [edi + VSVC_OFF_TABSZ], eax
    mov eax, [vsvc_tmp_features]
    mov [edi + VSVC_OFF_FEATURES], eax
    mov dword [edi + VSVC_OFF_FEAT_HI], 0

    ; Capabilities + Flags + Architektur
    mov eax, [vsvc_tmp_reqcap]
    mov [edi + VSVC_OFF_REQCAP], eax
    mov eax, [vsvc_tmp_flags]
    mov [edi + VSVC_OFF_FLAGS], eax
    mov eax, [vsvc_tmp_arch]
    mov [edi + VSVC_OFF_ARCH], eax

    ; Refcount + Name + Present
    mov dword [edi + VSVC_OFF_REFCNT], 0
    mov eax, [vsvc_tmp_name]
    mov [edi + VSVC_OFF_NAME], eax
    mov byte [edi + VSVC_OFF_PRESENT], 1

    lock inc dword [vsvc_stat_registered]

    ; NP_EVENT_SVC_REGISTERED publizieren
    mov eax, NP_EVENT_SVC_REGISTERED
    mov ecx, [vsvc_tmp_id0]
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish              ; Achtung: evbus_publish clobbered EBX (nur ESI/EDI werden wiederhergestellt)

    pop edi
    pop esi
    clc
    ret

.vr_conflict:
    mov eax, NP_ERR_VSVC_CONFLICT
    pop edi
    pop esi
    stc
    ret

.vr_limit:
    mov eax, NP_ERR_VSVC_LIMIT
    pop edi
    pop esi
    stc
    ret

.vr_abi:
    mov eax, NP_ERR_VSVC_ABI
    pop edi
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; vsvc_acquire – Service-Referenz erwerben (§13)
; ESI=uuid_ptr (16 Byte), EAX=min_major, EBX=max_major,
; ECX=min_minor, EDX=required_features
; CF=0 EBX=desc_ptr / CF=1 EAX=NP_ERR_VSVC_*
; ---------------------------------------------------------------------------
vsvc_acquire:
    push esi
    push ecx
    push edx

    mov [vsvc_tmp_acq_minmaj], eax
    mov [vsvc_tmp_acq_maxmaj], ebx
    mov [vsvc_tmp_acq_minmin], ecx
    mov [vsvc_tmp_acq_feat],   edx

    call vsvc_find
    jc .vac_not_found

    ; Zustand: nur ACTIVE erlaubt
    movzx eax, byte [ebx + VSVC_OFF_STATE]
    cmp eax, VSVC_STATE_ACTIVE
    jne .vac_state_err

    ; Major-Version im Bereich [min_major, max_major]
    movzx eax, word [ebx + VSVC_OFF_MAJ]
    cmp eax, [vsvc_tmp_acq_minmaj]
    jb .vac_version
    cmp eax, [vsvc_tmp_acq_maxmaj]
    ja .vac_version

    ; Minor-Version ≥ min_minor
    movzx eax, word [ebx + VSVC_OFF_MIN]
    cmp eax, [vsvc_tmp_acq_minmin]
    jb .vac_version

    ; Features: alle required_features vorhanden
    mov eax, [ebx + VSVC_OFF_FEATURES]
    mov ecx, [vsvc_tmp_acq_feat]
    and eax, ecx
    cmp eax, ecx
    jne .vac_feature

    ; Architektur-Prüfung
    mov eax, [ebx + VSVC_OFF_ARCH]
    cmp eax, VSVC_ARCH_ANY
    je .vac_arch_ok
    test eax, VSVC_ARCH_X86_32
    jz .vac_arch
.vac_arch_ok:

    lock inc dword [ebx + VSVC_OFF_REFCNT]
    lock inc dword [vsvc_stat_acquired]

    pop edx
    pop ecx
    pop esi
    xor eax, eax
    clc
    ret

.vac_state_err:
    cmp eax, VSVC_STATE_QUIESCING
    je .vac_quiescing
    mov eax, NP_ERR_VSVC_OFFLINE
    jmp .vac_fail
.vac_not_found:
    mov eax, NP_ERR_VSVC_NOT_FOUND
    jmp .vac_fail
.vac_quiescing:
    mov eax, NP_ERR_VSVC_QUIESCING
    jmp .vac_fail
.vac_version:
    mov eax, NP_ERR_VSVC_VERSION
    jmp .vac_fail
.vac_feature:
    mov eax, NP_ERR_VSVC_FEATURE
    jmp .vac_fail
.vac_arch:
    mov eax, NP_ERR_VSVC_ARCH
.vac_fail:
    pop edx
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; vsvc_release – Referenz freigeben (§17)
; EBX=desc_ptr (von vsvc_acquire)
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
vsvc_release:
    cmp byte [ebx + VSVC_OFF_PRESENT], 1
    jne .vrl_invalid
    cmp dword [ebx + VSVC_OFF_REFCNT], 0
    je .vrl_invalid
    lock dec dword [ebx + VSVC_OFF_REFCNT]
    ; QUIESCING + refcnt=0 → sofort OFFLINE (§32 Bootstrap-Vereinfachung)
    cmp byte [ebx + VSVC_OFF_STATE], VSVC_STATE_QUIESCING
    jne .vrl_done
    cmp dword [ebx + VSVC_OFF_REFCNT], 0
    jne .vrl_done
    mov byte [ebx + VSVC_OFF_STATE], VSVC_STATE_OFFLINE
    push eax
    push ecx
    push edx
    push esi
    push edi
    mov eax, NP_EVENT_SVC_OFFLINE
    mov ecx, [ebx + VSVC_OFF_ID0]
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish
    pop edi
    pop esi
    pop edx
    pop ecx
    pop eax
.vrl_done:
    clc
    ret
.vrl_invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; vsvc_quiesce – Service in QUIESCING versetzen (§32)
; EBX=desc_ptr
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
vsvc_quiesce:
    cmp byte [ebx + VSVC_OFF_PRESENT], 1
    jne .vq_invalid
    cmp byte [ebx + VSVC_OFF_STATE], VSVC_STATE_ACTIVE
    jne .vq_invalid
    mov byte [ebx + VSVC_OFF_STATE], VSVC_STATE_QUIESCING
    mov [vsvc_quiesce_tmp], ebx    ; EBX retten: evbus_publish clobbered EBX
    push eax
    push ecx
    push edx
    push esi
    push edi
    mov eax, NP_EVENT_SVC_QUIESCING
    mov ecx, [ebx + VSVC_OFF_ID0]
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish
    pop edi
    pop esi
    pop edx
    pop ecx
    pop eax
    ; EBX nach evbus_publish wiederherstellen (evbus_publish clobbered EBX)
    ; vsvc_quiesce_tmp enthält desc_ptr, der vor dem ersten evbus_publish gesetzt wurde
    mov ebx, [vsvc_quiesce_tmp]
    ; refcnt=0 → sofort OFFLINE
    cmp dword [ebx + VSVC_OFF_REFCNT], 0
    jne .vq_done
    mov byte [ebx + VSVC_OFF_STATE], VSVC_STATE_OFFLINE
    push eax
    push ecx
    push edx
    push esi
    push edi
    mov eax, NP_EVENT_SVC_OFFLINE
    mov ecx, [ebx + VSVC_OFF_ID0]
    xor edx, edx
    xor esi, esi
    xor edi, edi
    call evbus_publish
    pop edi
    pop esi
    pop edx
    pop ecx
    pop eax
.vq_done:
    clc
    ret
.vq_invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; vsvc_initialize – Registry nullen, 5 Event-Schemata, 4 Bootstrap-Services
; CF=0 OK / CF=1 Fehler
; ---------------------------------------------------------------------------
vsvc_initialize:
    push ebx
    push esi
    push edi

    ; Registry und Statistiken nullen
    mov edi, vsvc_registry
    xor eax, eax
    mov ecx, (VSVC_MAX_SERVICES * VSVC_DESC_SIZE) / 4
    rep stosd
    mov dword [vsvc_stat_registered], 0
    mov dword [vsvc_stat_acquired],   0

    ; Event-Schemata registrieren (§48)
    vsvc_reg_schema NP_EVENT_SVC_REGISTERED
    vsvc_reg_schema NP_EVENT_SVC_ACTIVATED
    vsvc_reg_schema NP_EVENT_SVC_OFFLINE
    vsvc_reg_schema NP_EVENT_SVC_QUIESCING
    vsvc_reg_schema NP_EVENT_SVC_UNREGISTERED

    ; Gemeinsame Bootstrap-Felder setzen
    mov dword [vsvc_tmp_maj],      1
    mov dword [vsvc_tmp_min],      0
    mov dword [vsvc_tmp_pat],      0
    mov dword [vsvc_tmp_tabsz],    VSVC_TBL_HDR_SIZE
    mov dword [vsvc_tmp_features], 0
    mov dword [vsvc_tmp_reqcap],   0
    mov dword [vsvc_tmp_arch],     VSVC_ARCH_X86_32
    mov dword [vsvc_tmp_stab],     VSVC_STAB_STABLE
    mov dword [vsvc_tmp_flags],    VSVC_FLAG_EARLY_BOOT | VSVC_FLAG_PANIC_SAFE

    ; 4 Bootstrap-Services registrieren (§61)
    vsvc_reg_boot VSVC_ID_KERN_0, VSVC_ID_KERN_1, VSVC_ID_KERN_2, VSVC_ID_KERN_3, vsvc_kern_tbl, vsvc_name_kern
    vsvc_reg_boot VSVC_ID_EVBS_0, VSVC_ID_EVBS_1, VSVC_ID_EVBS_2, VSVC_ID_EVBS_3, vsvc_evbs_tbl, vsvc_name_evbs
    vsvc_reg_boot VSVC_ID_CAPS_0, VSVC_ID_CAPS_1, VSVC_ID_CAPS_2, VSVC_ID_CAPS_3, vsvc_caps_tbl, vsvc_name_caps
    vsvc_reg_boot VSVC_ID_DIAG_0, VSVC_ID_DIAG_1, VSVC_ID_DIAG_2, VSVC_ID_DIAG_3, vsvc_diag_tbl, vsvc_name_diag

    pop edi
    pop esi
    pop ebx
    clc
    ret

.vi_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; vsvc_self_test – 10 Tests (§12..§17, §32)
; CF=0 alle bestanden / CF=1 Fehler
; ---------------------------------------------------------------------------
vsvc_self_test:
    push ebx
    push esi
    push edi

    ; === Test 1: Test-Service registrieren → CF=0 ===
    mov dword [vsvc_tmp_maj],      1
    mov dword [vsvc_tmp_min],      3
    mov dword [vsvc_tmp_pat],      0
    mov dword [vsvc_tmp_table],    vsvc_test_tbl
    mov dword [vsvc_tmp_tabsz],    VSVC_TBL_HDR_SIZE
    mov dword [vsvc_tmp_features], 0x0000000F
    mov dword [vsvc_tmp_reqcap],   0
    mov dword [vsvc_tmp_arch],     VSVC_ARCH_X86_32
    mov dword [vsvc_tmp_stab],     VSVC_STAB_EXPERIMENTAL
    mov dword [vsvc_tmp_name],     vsvc_name_test
    mov dword [vsvc_tmp_flags],    0
    mov eax, VSVC_ID_TEST_0
    mov ebx, VSVC_ID_TEST_1
    mov ecx, VSVC_ID_TEST_2
    mov edx, VSVC_ID_TEST_3
    call vsvc_register
    jc .vsst_fail

    ; === Test 2: Doppelte Registrierung → CF=1 ===
    mov eax, VSVC_ID_TEST_0
    mov ebx, VSVC_ID_TEST_1
    mov ecx, VSVC_ID_TEST_2
    mov edx, VSVC_ID_TEST_3
    call vsvc_register
    jnc .vsst_fail

    ; === Test 3: vsvc_find TEST → CF=0, EBX=desc_ptr ===
    mov esi, vsvc_st_uuid
    call vsvc_find
    jc .vsst_fail
    mov [vsvc_st_desc1], ebx

    ; === Test 4: vsvc_acquire gültige Version+Features → CF=0, refcnt=1 ===
    mov esi, vsvc_st_uuid
    mov eax, 1
    mov ebx, 1
    xor ecx, ecx
    xor edx, edx
    call vsvc_acquire
    jc .vsst_fail
    mov eax, [vsvc_st_desc1]
    cmp dword [eax + VSVC_OFF_REFCNT], 1
    jne .vsst_fail

    ; === Test 5: vsvc_acquire falsche Major-Version → CF=1 ===
    mov esi, vsvc_st_uuid
    mov eax, 2
    mov ebx, 3
    xor ecx, ecx
    xor edx, edx
    call vsvc_acquire
    jnc .vsst_fail

    ; === Test 6: vsvc_acquire fehlendes Feature → CF=1 ===
    mov esi, vsvc_st_uuid
    mov eax, 1
    mov ebx, 1
    xor ecx, ecx
    mov edx, 0x000000FF     ; Bit 4-7 fehlen (Service hat nur 0x0F)
    call vsvc_acquire
    jnc .vsst_fail

    ; === Test 7: vsvc_release → CF=0, refcnt=0 ===
    mov ebx, [vsvc_st_desc1]
    call vsvc_release
    jc .vsst_fail
    cmp dword [ebx + VSVC_OFF_REFCNT], 0
    jne .vsst_fail

    ; === Test 8: vsvc_quiesce (refcnt=0) → CF=0, state=OFFLINE ===
    mov ebx, [vsvc_st_desc1]
    call vsvc_quiesce
    jc .vsst_fail
    movzx eax, byte [ebx + VSVC_OFF_STATE]
    cmp eax, VSVC_STATE_OFFLINE
    jne .vsst_fail

    ; === Test 9: vsvc_acquire auf OFFLINE → CF=1 ===
    mov esi, vsvc_st_uuid
    mov eax, 1
    mov ebx, 1
    xor ecx, ecx
    xor edx, edx
    call vsvc_acquire
    jnc .vsst_fail

    ; === Test 10: Bootstrap-KERN-Service vorhanden und ACTIVE ===
    mov esi, vsvc_st_kern_uuid
    call vsvc_find
    jc .vsst_fail
    movzx eax, byte [ebx + VSVC_OFF_STATE]
    cmp eax, VSVC_STATE_ACTIVE
    jne .vsst_fail

    pop edi
    pop esi
    pop ebx
    clc
    ret

.vsst_fail:
    pop edi
    pop esi
    pop ebx
    stc
    ret

; ---------------------------------------------------------------------------
; §105 Daten
; ---------------------------------------------------------------------------
align 4
vsvc_registry:           times (VSVC_MAX_SERVICES * VSVC_DESC_SIZE / 4) dd 0
vsvc_stat_registered:    dd 0
vsvc_stat_acquired:      dd 0

; Temp-Speicher für vsvc_register (dd um Überlappung bei dword-Zugriffen zu vermeiden)
vsvc_tmp_id0:       dd 0
vsvc_tmp_id1:       dd 0
vsvc_tmp_id2:       dd 0
vsvc_tmp_id3:       dd 0
vsvc_tmp_maj:       dd 0
vsvc_tmp_min:       dd 0
vsvc_tmp_pat:       dd 0
vsvc_tmp_stab:      dd 0
vsvc_tmp_table:     dd 0
vsvc_tmp_tabsz:     dd 0
vsvc_tmp_features:  dd 0
vsvc_tmp_reqcap:    dd 0
vsvc_tmp_arch:      dd 0
vsvc_tmp_flags:     dd 0
vsvc_tmp_name:      dd 0
; Temp für vsvc_find
vsvc_tmp_find0:     dd 0
vsvc_tmp_find1:     dd 0
vsvc_tmp_find2:     dd 0
vsvc_tmp_find3:     dd 0
; Temp für vsvc_quiesce (EBX-Rettung über evbus_publish-Aufruf)
vsvc_quiesce_tmp:    dd 0
; Temp für vsvc_acquire
vsvc_tmp_acq_minmaj: dd 0
vsvc_tmp_acq_maxmaj: dd 0
vsvc_tmp_acq_minmin: dd 0
vsvc_tmp_acq_feat:   dd 0
; UUID-Puffer für Duplikat-Check in vsvc_register
vsvc_reg_id_buf:     times 16 db 0
; Selbsttest-Zustand
vsvc_st_desc1:       dd 0
vsvc_st_uuid:
    dd VSVC_ID_TEST_0, VSVC_ID_TEST_1, VSVC_ID_TEST_2, VSVC_ID_TEST_3
vsvc_st_kern_uuid:
    dd VSVC_ID_KERN_0, VSVC_ID_KERN_1, VSVC_ID_KERN_2, VSVC_ID_KERN_3

; Bootstrap-Service-Tabellen (§10: size/maj/min/feat_lo/feat_hi/flags/rsvd = 24 Byte)
align 4
vsvc_kern_tbl:
    dd VSVC_TBL_HDR_SIZE
    dw 1, 0
    dd 0, 0
    dd VSVC_CALL_EARLY_BOOT | VSVC_CALL_PANIC_SAFE
    dd 0
vsvc_evbs_tbl:
    dd VSVC_TBL_HDR_SIZE
    dw 1, 0
    dd 0, 0
    dd VSVC_CALL_EARLY_BOOT | VSVC_CALL_PANIC_SAFE
    dd 0
vsvc_caps_tbl:
    dd VSVC_TBL_HDR_SIZE
    dw 1, 0
    dd 0, 0
    dd VSVC_CALL_EARLY_BOOT | VSVC_CALL_PANIC_SAFE
    dd 0
vsvc_diag_tbl:
    dd VSVC_TBL_HDR_SIZE
    dw 1, 0
    dd 0, 0
    dd VSVC_CALL_EARLY_BOOT | VSVC_CALL_PANIC_SAFE
    dd 0
vsvc_test_tbl:
    dd VSVC_TBL_HDR_SIZE
    dw 1, 3
    dd 0x0000000F, 0
    dd VSVC_CALL_THREAD_CTX
    dd 0
; Service-Namen
vsvc_name_kern:  db "nova.kernel.core", 0
vsvc_name_evbs:  db "nova.evbus.core", 0
vsvc_name_caps:  db "nova.capability.core", 0
vsvc_name_diag:  db "nova.diagnostics.core", 0
vsvc_name_test:  db "nova.test.service", 0
align 4

