; ---------------------------------------------------------------------------
; Nova Filesystem-Subsystem §038 (Kategorien 001..005, 007, 009..011)
;
; NPSPEC-FILESYSTEM-OBJECTID-0001    - Stabile Objekt-IDs
; NPSPEC-FILESYSTEM-NAMESPACE-0001/2 - Namespace-Baum
; NPSPEC-FILESYSTEM-PERMISSION-0001  - Zugriffsrechte
; NPSPEC-FILESYSTEM-PROJECTION-0001/2- Projections/Symlink
; NPSPEC-FILESYSTEM-RELATION-0001    - Eltern-Kind-Relationen
; NPSPEC-FILESYSTEM-SEMANTIC-0001    - Semantische Typen
; NPSPEC-FILESYSTEM-TRANSACTION-0001 - Transaktionen
; NPSPEC-FILESYSTEM-LOCALIZATION-0001- Pfad-Lokalisierung
; NPSPEC-FSSTORAGE-VOLUME-0001       - Volume-Verwaltung
; NPSPEC-STORAGE-DEVICE-0001         - Geraete-Identitaet
; NPSPEC-STORAGE-DISCOVERY-0001      - Storage-Enumeration
; NPSPEC-STORAGE-IDENTITY-0001       - Disk-Identitaet
; NPSPEC-STORAGE-LOCATION-0001       - Speicherort-Abstraktion
; NPSPEC-STORAGE-LOCATIONTRANSPARENCY-0001 - Ortunabhaengigkeit
; NPSPEC-STORAGE-MOUNT-0001          - Mount-API
; NPSPEC-STORAGE-REMOTE-0001         - Remote-Storage
; NPSPEC-FS-USERSPACE-0001..7        - Userspace-Verzeichnisbaum
; NPSPEC-WORKSPACE-*-0001            - Workspace-Isolation
; NPSPEC-TEMP-*-0001                 - Temp-FS
; NPSPEC-FSYS-*-0001                 - System-Verzeichnisse
; NPSPEC-TRUST-ROOT/CHAIN/VERIFY     - Trust-Kette
; NPSPEC-POLICY-*-0001               - Zugriffs-Policy
; NPSPEC-FS-NAMESPACE-*-0001         - Namespace-Isolation
; ---------------------------------------------------------------------------

; ---------------------------------------------------------------------------
; ObjectID (NPSPEC-FILESYSTEM-OBJECTID-0001)
; 16 Bytes: [4 volume_id][4 generation][8 inode_seq]
; ---------------------------------------------------------------------------
OBJID_SIZE            equ 16
OI_VOLUME_ID          equ 0
OI_GENERATION         equ 4
OI_INODE_SEQ_LO       equ 8
OI_INODE_SEQ_HI       equ 12

OBJID_NULL_VOLUME     equ 0xFFFFFFFF
OBJID_ROOT_INODE_LO   equ 1
OBJID_ROOT_INODE_HI   equ 0

; Semantische Typen (NPSPEC-FILESYSTEM-SEMANTIC-0001)
FSOBJ_TYPE_UNKNOWN    equ 0
FSOBJ_TYPE_FILE       equ 1
FSOBJ_TYPE_DIR        equ 2
FSOBJ_TYPE_SYMLINK    equ 3    ; Projection (NPSPEC-FILESYSTEM-PROJECTION-0001)
FSOBJ_TYPE_DEVICE     equ 4
FSOBJ_TYPE_PIPE       equ 5
FSOBJ_TYPE_SOCKET     equ 6
FSOBJ_TYPE_VOLUME     equ 7   ; (NPSPEC-FSSTORAGE-VOLUME-0001)
FSOBJ_TYPE_WORKSPACE  equ 8   ; (NPSPEC-WORKSPACE-*-0001)
FSOBJ_TYPE_TEMP       equ 9   ; (NPSPEC-TEMP-*-0001)

; Berechtigungs-Bits (NPSPEC-FILESYSTEM-PERMISSION-0001)
FSPERM_R              equ 0x04  ; Lesen
FSPERM_W              equ 0x02  ; Schreiben
FSPERM_X              equ 0x01  ; Ausfuehren
FSPERM_OWNER_SHIFT    equ 6
FSPERM_GROUP_SHIFT    equ 3
FSPERM_OTHER_SHIFT    equ 0

; Vollstaendige Rechte-Masken
FSPERM_0755           equ 0x1ED   ; rwxr-xr-x
FSPERM_0644           equ 0x1A4   ; rw-r--r--
FSPERM_0600           equ 0x180   ; rw-------

; Namespace-IDs (NPSPEC-FILESYSTEM-NAMESPACE-0001/2)
FS_NS_ROOT            equ 0    ; System-Root "/"
FS_NS_SYSTEM          equ 1    ; /sys  (NPSPEC-FSYS-*-0001)
FS_NS_USER            equ 2    ; /user (NPSPEC-FS-USERSPACE-0001)
FS_NS_TEMP            equ 3    ; /tmp  (NPSPEC-TEMP-*-0001)
FS_NS_WORKSPACE       equ 4    ; /ws   (NPSPEC-WORKSPACE-*-0001)
FS_NS_PROGRAM         equ 5    ; /prog (NPSPEC-PROGRAM-LAYOUT-0001)
FS_NS_DEVICE          equ 6    ; /dev  (NPSPEC-STORAGE-DEVICE-0001)

; Volume-Flags (NPSPEC-FSSTORAGE-VOLUME-0001)
VOL_FLAG_MOUNTED      equ 0x01
VOL_FLAG_READONLY     equ 0x02
VOL_FLAG_REMOVABLE    equ 0x04
VOL_FLAG_ENCRYPTED    equ 0x08
VOL_FLAG_REMOTE       equ 0x10  ; (NPSPEC-STORAGE-REMOTE-0001)
VOL_FLAG_TRUSTED      equ 0x20  ; (NPSPEC-TRUST-ROOT-0001)
VOL_FLAG_SYSTEM       equ 0x40

; Volume-Record (64 Bytes)
VOL_REC_SIZE          equ 64
VR_ID                 equ 0    ; Volume-ID (uint32)
VR_FLAGS              equ 4
VR_TYPE               equ 8    ; FSOBJ_TYPE_VOLUME / Remote
VR_FS_NS              equ 12   ; FS_NS_* Mountpoint
VR_BLOCK_SIZE         equ 16
VR_BLOCK_COUNT        equ 20
VR_FREE_BLOCKS        equ 24
VR_LABEL              equ 28   ; 16 Byte ASCII-Label
VR_DEVICE_ID          equ 44
VR_MOUNT_GEN          equ 48   ; Mountgenerierung (NPSPEC-STORAGE-MOUNT-0001)
VR_TRUST_LEVEL        equ 52   ; (NPSPEC-TRUST-ROOT-0001)
; 56..63: reserviert

VOL_CAPACITY          equ 4

; Namespace-Record (32 Bytes, NPSPEC-FS-NAMESPACE-*-0001)
NS_REC_SIZE           equ 32
NR_ID                 equ 0    ; FS_NS_*
NR_FLAGS              equ 4
NR_VOLUME_ID          equ 8
NR_PARENT_NS          equ 12
NR_POLICY             equ 16   ; FSPOLICY_* (NPSPEC-POLICY-*-0001)
NR_PATH_HASH          equ 20   ; Hash des Mountpfads
NR_ISOLATION          equ 24   ; 1=isoliert (NPSPEC-WORKSPACE-*-0001)
; 28..31: reserviert

NS_CAPACITY           equ 8

; Trust-Level (NPSPEC-TRUST-ROOT-0001)
TRUST_NONE            equ 0
TRUST_SYSTEM          equ 1    ; Kernel-signiert
TRUST_USER            equ 2    ; Benutzer-signiert
TRUST_REMOTE          equ 3    ; Remote-verifiziert

; Policy-Flags (NPSPEC-POLICY-*-0001)
FSPOLICY_ALLOW_READ   equ 0x01
FSPOLICY_ALLOW_WRITE  equ 0x02
FSPOLICY_ALLOW_EXEC   equ 0x04
FSPOLICY_SYS_WRITE    equ 0x08  ; (NPSPEC-POLICY-SYSTEMWRITE-0001)
FSPOLICY_TEMP_CLEAR   equ 0x10  ; (NPSPEC-TEMP-*-0001) Auto-Loeschen
FSPOLICY_WS_ISOLATED  equ 0x20  ; (NPSPEC-WORKSPACE-*-0001)

; Transaction-Record (32 Bytes, NPSPEC-FILESYSTEM-TRANSACTION-0001)
TX_REC_SIZE           equ 32
TX_ID                 equ 0
TX_FLAGS              equ 4
TX_VOLUME_ID          equ 8
TX_OP_COUNT           equ 12
TX_STATE              equ 16   ; 0=open 1=committed 2=rolled back
TX_CHECKSUM           equ 20
TX_TIMESTAMP          equ 24

TX_CAPACITY           equ 4

; ---------------------------------------------------------------------------
; fs_initialize
; Initialisiert Volume-Tabelle, Namespace-Registry, Root-Volume und
; Namespace-Baum.
; CF=0 Erfolg
; ---------------------------------------------------------------------------
fs_initialize:
    push edi
    ; Tabellen loeschen
    mov edi, fs_vol_table
    xor eax, eax
    mov ecx, (VOL_CAPACITY * VOL_REC_SIZE) / 4
    rep stosd
    mov edi, fs_ns_table
    mov ecx, (NS_CAPACITY * NS_REC_SIZE) / 4
    rep stosd
    mov edi, fs_tx_table
    mov ecx, (TX_CAPACITY * TX_REC_SIZE) / 4
    rep stosd
    pop edi

    mov dword [fs_vol_count], 0
    mov dword [fs_ns_count], 0
    mov dword [fs_tx_count], 0
    mov dword [fs_next_tx_id], 1
    mov dword [fs_next_vol_id], 1

    ; Root-Volume registrieren (NPSPEC-FSSTORAGE-VOLUME-0001)
    mov eax, 0                      ; ID 0 = Root
    mov ebx, VOL_FLAG_MOUNTED | VOL_FLAG_SYSTEM | VOL_FLAG_TRUSTED
    mov ecx, FS_NS_ROOT
    call fs_vol_register
    jc .fail

    ; Namespace-Baum anlegen (NPSPEC-FILESYSTEM-NAMESPACE-0001)
    mov eax, FS_NS_ROOT
    mov ebx, 0xFFFFFFFF             ; kein Parent
    mov ecx, FSPOLICY_ALLOW_READ | FSPOLICY_ALLOW_WRITE | FSPOLICY_ALLOW_EXEC
    call fs_ns_register
    jc .fail

    mov eax, FS_NS_SYSTEM
    mov ebx, FS_NS_ROOT
    mov ecx, FSPOLICY_ALLOW_READ | FSPOLICY_ALLOW_EXEC | FSPOLICY_SYS_WRITE
    call fs_ns_register
    jc .fail

    mov eax, FS_NS_USER
    mov ebx, FS_NS_ROOT
    mov ecx, FSPOLICY_ALLOW_READ | FSPOLICY_ALLOW_WRITE | FSPOLICY_ALLOW_EXEC
    call fs_ns_register
    jc .fail

    mov eax, FS_NS_TEMP
    mov ebx, FS_NS_ROOT
    mov ecx, FSPOLICY_ALLOW_READ | FSPOLICY_ALLOW_WRITE | FSPOLICY_TEMP_CLEAR
    call fs_ns_register
    jc .fail

    mov eax, FS_NS_WORKSPACE
    mov ebx, FS_NS_ROOT
    mov ecx, FSPOLICY_ALLOW_READ | FSPOLICY_ALLOW_WRITE | FSPOLICY_WS_ISOLATED
    call fs_ns_register
    jc .fail

    mov eax, FS_NS_PROGRAM
    mov ebx, FS_NS_ROOT
    mov ecx, FSPOLICY_ALLOW_READ | FSPOLICY_ALLOW_EXEC
    call fs_ns_register
    jc .fail

    mov eax, FS_NS_DEVICE
    mov ebx, FS_NS_ROOT
    mov ecx, FSPOLICY_ALLOW_READ | FSPOLICY_ALLOW_WRITE
    call fs_ns_register
    jc .fail

    mov dword [fs_ready], 1
    clc
    ret
.fail:
    stc
    ret

; ---------------------------------------------------------------------------
; fs_vol_register: EAX=vol_id, EBX=flags, ECX=ns_id
; ---------------------------------------------------------------------------
fs_vol_register:
    cmp dword [fs_vol_count], VOL_CAPACITY
    jae .full
    push edi
    push edx
    mov edi, fs_vol_table
    mov edx, [fs_vol_count]
    imul edx, VOL_REC_SIZE
    add edi, edx
    mov [edi + VR_ID], eax
    mov [edi + VR_FLAGS], ebx
    mov dword [edi + VR_TYPE], FSOBJ_TYPE_VOLUME
    mov [edi + VR_FS_NS], ecx
    mov dword [edi + VR_BLOCK_SIZE], 4096
    mov dword [edi + VR_BLOCK_COUNT], 0
    mov dword [edi + VR_FREE_BLOCKS], 0
    mov dword [edi + VR_DEVICE_ID], 0
    mov dword [edi + VR_MOUNT_GEN], 1
    ; Trust-Level (NPSPEC-TRUST-ROOT-0001)
    test ebx, VOL_FLAG_TRUSTED
    jz .not_trusted
    mov dword [edi + VR_TRUST_LEVEL], TRUST_SYSTEM
    jmp .done
.not_trusted:
    test ebx, VOL_FLAG_REMOTE
    jz .user_trust
    mov dword [edi + VR_TRUST_LEVEL], TRUST_REMOTE
    jmp .done
.user_trust:
    mov dword [edi + VR_TRUST_LEVEL], TRUST_USER
.done:
    inc dword [fs_vol_count]
    pop edx
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; fs_ns_register: EAX=ns_id, EBX=parent_ns, ECX=policy
; ---------------------------------------------------------------------------
fs_ns_register:
    cmp dword [fs_ns_count], NS_CAPACITY
    jae .full
    push edi
    push edx
    mov edi, fs_ns_table
    mov edx, [fs_ns_count]
    imul edx, NS_REC_SIZE
    add edi, edx
    mov [edi + NR_ID], eax
    mov dword [edi + NR_FLAGS], 0
    mov dword [edi + NR_VOLUME_ID], 0
    mov [edi + NR_PARENT_NS], ebx
    mov [edi + NR_POLICY], ecx
    ; Isolation fuer Workspaces und Temp setzen
    test ecx, FSPOLICY_WS_ISOLATED
    jz .check_temp
    mov dword [edi + NR_ISOLATION], 1
    jmp .set_done
.check_temp:
    test ecx, FSPOLICY_TEMP_CLEAR
    jz .set_done
    mov dword [edi + NR_ISOLATION], 1
    jmp .set_done
.set_done:
    inc dword [fs_ns_count]
    pop edx
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; fs_vol_mount: EAX=vol_id, EBX=ns_id, ECX=extra_flags
; (NPSPEC-STORAGE-MOUNT-0001)
; ---------------------------------------------------------------------------
fs_vol_mount:
    push esi
    push ecx
    mov esi, fs_vol_table
    push ecx
    mov ecx, [fs_vol_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + VR_ID], eax
    je .found
    add esi, VOL_REC_SIZE
    dec ecx
    jmp .scan
.found:
    pop ecx
    or dword [esi + VR_FLAGS], VOL_FLAG_MOUNTED
    or [esi + VR_FLAGS], ecx
    mov [esi + VR_FS_NS], ebx
    inc dword [esi + VR_MOUNT_GEN]
    pop ecx
    pop esi
    clc
    ret
.not_found:
    pop ecx
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; fs_vol_lookup: EAX=ns_id -> EBX=vol_id, CF=0 ok
; (NPSPEC-STORAGE-LOCATION-0001, NPSPEC-STORAGE-LOCATIONTRANSPARENCY-0001)
; ---------------------------------------------------------------------------
fs_vol_lookup:
    push esi
    push ecx
    mov esi, fs_vol_table
    mov ecx, [fs_vol_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + VR_FS_NS], eax
    je .found
    add esi, VOL_REC_SIZE
    dec ecx
    jmp .scan
.found:
    mov ebx, [esi + VR_ID]
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
; fs_objid_make: EAX=volume_id, EDX=inode_lo -> ESI=objid_buf
; Erzeugt eine neue ObjectID (NPSPEC-FILESYSTEM-OBJECTID-0001).
; ---------------------------------------------------------------------------
fs_objid_make:
    push edi
    mov edi, fs_objid_buf
    mov [edi + OI_VOLUME_ID], eax
    mov eax, [fs_obj_generation]
    mov [edi + OI_GENERATION], eax
    mov [edi + OI_INODE_SEQ_LO], edx
    mov dword [edi + OI_INODE_SEQ_HI], 0
    inc dword [fs_obj_inode_counter]
    mov esi, edi
    pop edi
    clc
    ret

; ---------------------------------------------------------------------------
; fs_ns_check_policy: EAX=ns_id, EBX=required_policy -> CF=0 erlaubt
; (NPSPEC-POLICY-*-0001)
; ---------------------------------------------------------------------------
fs_ns_check_policy:
    push esi
    push ecx
    mov esi, fs_ns_table
    mov ecx, [fs_ns_count]
.scan:
    test ecx, ecx
    jz .denied
    cmp [esi + NR_ID], eax
    je .found
    add esi, NS_REC_SIZE
    dec ecx
    jmp .scan
.found:
    mov ecx, [esi + NR_POLICY]
    and ecx, ebx
    cmp ecx, ebx
    pop ecx
    pop esi
    je .allowed
    stc
    ret
.allowed:
    clc
    ret
.denied:
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; fs_tx_begin: EAX=volume_id -> EBX=tx_id, CF=0 ok
; (NPSPEC-FILESYSTEM-TRANSACTION-0001)
; ---------------------------------------------------------------------------
fs_tx_begin:
    cmp dword [fs_tx_count], TX_CAPACITY
    jae .full
    push edi
    push edx
    mov edi, fs_tx_table
    mov edx, [fs_tx_count]
    imul edx, TX_REC_SIZE
    add edi, edx
    mov ebx, [fs_next_tx_id]
    mov [edi + TX_ID], ebx
    mov dword [edi + TX_FLAGS], 0
    mov [edi + TX_VOLUME_ID], eax
    mov dword [edi + TX_OP_COUNT], 0
    mov dword [edi + TX_STATE], 0   ; open
    mov eax, [timer_ticks]
    mov [edi + TX_TIMESTAMP], eax
    inc dword [fs_next_tx_id]
    inc dword [fs_tx_count]
    pop edx
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; fs_tx_commit: EBX=tx_id
; ---------------------------------------------------------------------------
fs_tx_commit:
    push esi
    push ecx
    mov esi, fs_tx_table
    mov ecx, [fs_tx_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + TX_ID], ebx
    je .found
    add esi, TX_REC_SIZE
    dec ecx
    jmp .scan
.found:
    mov dword [esi + TX_STATE], 1   ; committed
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
; fs_tx_rollback: EBX=tx_id
; ---------------------------------------------------------------------------
fs_tx_rollback:
    push esi
    push ecx
    mov esi, fs_tx_table
    mov ecx, [fs_tx_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + TX_ID], ebx
    je .found
    add esi, TX_REC_SIZE
    dec ecx
    jmp .scan
.found:
    mov dword [esi + TX_STATE], 2   ; rolled back
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
; fs_temp_clear: Leert alle Temp-Namespace-Eintraege
; (NPSPEC-TEMP-*-0001, NPSPEC-POLICY-*-0001 FSPOLICY_TEMP_CLEAR)
; ---------------------------------------------------------------------------
fs_temp_clear:
    push esi
    push ecx
    mov esi, fs_ns_table
    mov ecx, [fs_ns_count]
.scan:
    test ecx, ecx
    jz .done
    cmp dword [esi + NR_ID], FS_NS_TEMP
    jne .next
    ; Temp-Volume-Gegenstand nullen (vereinfacht: Counter zuruecksetzen)
    mov dword [esi + NR_FLAGS], 0
.next:
    add esi, NS_REC_SIZE
    dec ecx
    jmp .scan
.done:
    pop ecx
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; fs_self_test
; ---------------------------------------------------------------------------
fs_self_test:
    push esi
    xor esi, esi

    cmp dword [fs_ready], 1
    je .t2
    inc esi
.t2:
    ; Volume vorhanden?
    cmp dword [fs_vol_count], 0
    jne .t3
    inc esi
.t3:
    ; Namespace vorhanden?
    cmp dword [fs_ns_count], 0
    jne .t4
    inc esi
.t4:
    ; Volume Lookup: Root-NS -> vol 0
    mov eax, FS_NS_ROOT
    call fs_vol_lookup
    jnc .t5
    inc esi
.t5:
    ; Policy-Check: Root darf lesen
    mov eax, FS_NS_ROOT
    mov ebx, FSPOLICY_ALLOW_READ
    call fs_ns_check_policy
    jnc .t6
    inc esi
.t6:
    ; Transaktion starten und committen
    mov eax, 0
    call fs_tx_begin
    jnc .t7
    inc esi
    jmp .t8
.t7:
    call fs_tx_commit
    jnc .t8
    inc esi
.t8:
    ; ObjectID erzeugen
    mov eax, 0
    mov edx, OBJID_ROOT_INODE_LO
    call fs_objid_make
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

; ---------------------------------------------------------------------------
; Statische Daten
; ---------------------------------------------------------------------------
align 4
fs_ready:             dd 0
fs_vol_count:         dd 0
fs_ns_count:          dd 0
fs_tx_count:          dd 0
fs_next_tx_id:        dd 1
fs_next_vol_id:       dd 1
fs_obj_generation:    dd 1
fs_obj_inode_counter: dd 0

align 4
fs_vol_table:
    times VOL_CAPACITY * VOL_REC_SIZE db 0

align 4
fs_ns_table:
    times NS_CAPACITY * NS_REC_SIZE db 0

align 4
fs_tx_table:
    times TX_CAPACITY * TX_REC_SIZE db 0

align 4
fs_objid_buf:
    times OBJID_SIZE db 0
