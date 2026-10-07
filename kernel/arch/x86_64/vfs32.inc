; ---------------------------------------------------------------------------
; VFS-Syscalls ABI 1.1 auf NovaFS (NPSPEC-FILESYSTEM-NAMESPACE-0002,
; NPSPEC-FILESYSTEM-PERMISSION-0001, NPSPEC-POLICY-SYSTEMWRITE-0001)
;
;   Path -> Resolve -> ObjectID -> Capability-/Policy-Pruefung -> Handle
;
; - Pfadkenntnis oder Sichtbarkeit erzeugt keine Authority: jede Operation
;   verlangt ein Handle mit passendem Recht UND die Prozess-Capability.
; - Schreiboperationen pruefen Capability und Systembereichs-Policy bei jedem
;   Aufruf erneut (Revalidierung vor Commit).
; - Ohne SECURITY_CAP_FS_SYSTEM_WRITE ist Schreiben nur unterhalb /Benutzer
;   erlaubt; Administratorstatus allein genuegt nicht.
; - Syscalls laufen als Interrupt-Gate (IF=0) und damit exklusiv; nfs_busy
;   schuetzt zusaetzlich vor Wiedereintritt.
; ---------------------------------------------------------------------------

SYSCALL_VFS_READ            equ 3
SYSCALL_VFS_WRITE           equ 4
SYSCALL_VFS_CREATE          equ 5
SYSCALL_VFS_READ_DIRECTORY  equ 6
SYSCALL_VFS_QUERY           equ 7

VFS_LOOKUP_FLAG_WRITE       equ 0x00000001
VFS_IO_MAX                  equ 4096
VFS_ENTRY_SIZE              equ 288
VFS_INFO_SIZE               equ 48

HANDLE_RIGHT_READ           equ 0x00000040
HANDLE_RIGHT_WRITE          equ 0x00000080

SYSCALL_STATUS_NOT_FOUND    equ -2
SYSCALL_STATUS_EXISTS       equ -6
SYSCALL_STATUS_IO           equ -7
SYSCALL_STATUS_NO_SPACE     equ -11
SYSCALL_STATUS_READ_ONLY    equ -12
SYSCALL_STATUS_NOT_DIRECTORY equ -20
SYSCALL_STATUS_NOT_FILE     equ -21
SYSCALL_STATUS_LIMIT        equ -24

VFS_SEEN_LOOKUP             equ 0x01
VFS_SEEN_READ               equ 0x02
VFS_SEEN_WRITE              equ 0x04
VFS_SEEN_CREATE             equ 0x08
VFS_SEEN_DIRECTORY          equ 0x10
VFS_SEEN_QUERY              equ 0x20
VFS_SEEN_POLICY             equ 0x40
VFS_SEEN_RIGHTS             equ 0x80

; EDX=Syscall-Frame. EAX=Status (0 oder negativer NovaStatus).
vfs_syscall:
    mov [vfs_frame], edx
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    mov eax, [edx + 32]
    cmp eax, SYSCALL_VFS_LOOKUP
    je vfs_op_lookup
    cmp eax, SYSCALL_VFS_READ
    je vfs_op_read
    cmp eax, SYSCALL_VFS_WRITE
    je vfs_op_write
    cmp eax, SYSCALL_VFS_CREATE
    je vfs_op_create
    cmp eax, SYSCALL_VFS_READ_DIRECTORY
    je vfs_op_read_directory
    cmp eax, SYSCALL_VFS_QUERY
    je vfs_op_query
    mov eax, SYSCALL_STATUS_OPERATION
    ret
.bad_abi:
    mov eax, SYSCALL_STATUS_ABI
    ret

; ECX=Strukturgroesse. Kopiert die Argumente nach vfs_args und prueft
; StructSize und Version. EAX=Status, CF bei Fehler.
vfs_copy_args:
    mov edx, [vfs_frame]
    cmp [edx + 20], ecx
    jb .size
    mov [vfs_args_size], ecx
    mov esi, [edx + 36]
    mov edi, vfs_args
    call syscall_copy_from_user
    jc .pointer
    mov ecx, [vfs_args_size]
    cmp [vfs_args + 0], ecx
    jne .size
    cmp dword [vfs_args + 4], SYSCALL_ABI_VERSION
    jne .abi
    xor eax, eax
    clc
    ret
.size:
    mov eax, SYSCALL_STATUS_SIZE
    stc
    ret
.pointer:
    mov eax, SYSCALL_STATUS_POINTER
    stc
    ret
.abi:
    mov eax, SYSCALL_STATUS_ABI
    stc
    ret

; Schreibt vfs_args an die Benutzeradresse zurueck. EAX=Status.
vfs_copy_args_back:
    mov edx, [vfs_frame]
    mov edi, [edx + 36]
    mov esi, vfs_args
    mov ecx, [vfs_args_size]
    call syscall_copy_buffer_to_user
    jc .pointer
    xor eax, eax
    ret
.pointer:
    mov eax, SYSCALL_STATUS_POINTER
    ret

; EDX=Benutzer-Handle, ECX=benoetigte Rechte.
; EAX=NovaFS-ObjectID, EBX=Handle-Rechte. CF mit EAX=Status bei Fehler.
vfs_resolve_handle:
    mov [vfs_temp_handle], edx
    mov eax, [userspace_pid]
    mov ebx, OBJECT_TYPE_VFS_NODE
    call handle_resolve
    jc .denied
    mov eax, [eax + OBJ_PAYLOAD0]
    test eax, eax
    jz .denied
    ; Rechte des Handles fuer Query und Rueckgabe
    mov edx, [vfs_temp_handle]
    and edx, 0xFFFF
    dec edx
    imul edx, HANDLE_ENTRY_SIZE
    mov ebx, [handle_table + edx + HANDLE_RIGHTS]
    clc
    ret
.denied:
    or byte [vfs_seen], VFS_SEEN_RIGHTS
    mov eax, SYSCALL_STATUS_ACCESS
    stc
    ret

; EDX=Prozess-Capability. CF mit EAX=ACCESS, wenn sie fehlt.
vfs_require_cap:
    mov eax, [userspace_pid]
    call security_check
    jc .denied
    xor eax, eax
    ret
.denied:
    mov eax, SYSCALL_STATUS_ACCESS
    stc
    ret

; EAX=ObjectID. Prueft Schreib-Policy, Capability und Mountzustand.
; CF mit EAX=Status bei Ablehnung.
vfs_check_write:
    mov [vfs_temp_object], eax
    cmp dword [nfs_mounted], 1
    jne .not_found
    cmp dword [nfs_readonly], 0
    jne .read_only
    mov edx, SECURITY_CAP_FS_WRITE
    call vfs_require_cap
    jc .denied
    mov eax, [vfs_temp_object]
    call novafs_requires_system_write
    jnc .allowed
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_FS_SYSTEM_WRITE
    call security_check
    jnc .allowed
    test byte [vfs_seen], VFS_SEEN_POLICY
    jnz .denied
    or byte [vfs_seen], VFS_SEEN_POLICY
    mov esi, message_vfs_policy_denied
    call serial_write_string
.denied:
    mov eax, SYSCALL_STATUS_ACCESS
    stc
    ret
.allowed:
    xor eax, eax
    clc
    ret
.read_only:
    mov eax, SYSCALL_STATUS_READ_ONLY
    stc
    ret
.not_found:
    mov eax, SYSCALL_STATUS_NOT_FOUND
    stc
    ret

; EAX=ObjectID, ECX=Rechte. EAX=neues Benutzer-Handle, CF mit Status.
; Das Handle haelt die einzige Referenz auf das VFS-Node-Objekt.
vfs_new_handle:
    mov [vfs_temp_rights], ecx
    mov edx, eax
    mov eax, OBJECT_TYPE_VFS_NODE
    xor ebx, ebx
    call object_create
    jc .limit
    mov [vfs_temp_kobject], eax
    mov edx, eax
    mov eax, [userspace_pid]
    mov ebx, OBJECT_TYPE_VFS_NODE
    mov ecx, [vfs_temp_rights]
    xor esi, esi
    call handle_create
    jc .release
    mov [vfs_temp_handle], eax
    mov eax, [vfs_temp_kobject]
    call object_release
    mov eax, [vfs_temp_handle]
    clc
    ret
.release:
    mov eax, [vfs_temp_kobject]
    call object_release
.limit:
    mov eax, SYSCALL_STATUS_LIMIT
    stc
    ret

; NovaFS-Fehler (nfs_error) -> Syscall-Status in EAX
vfs_map_error:
    mov eax, [nfs_error]
    cmp eax, NOVAFS_ERR_NOT_FOUND
    je .not_found
    cmp eax, NOVAFS_ERR_EXISTS
    je .exists
    cmp eax, NOVAFS_ERR_FULL
    je .no_space
    cmp eax, NOVAFS_ERR_TREE_FULL
    je .no_space
    cmp eax, NOVAFS_ERR_READ_ONLY
    je .read_only
    cmp eax, NOVAFS_ERR_INVALID
    je .invalid
    mov eax, SYSCALL_STATUS_IO
    ret
.not_found:
    mov eax, SYSCALL_STATUS_NOT_FOUND
    ret
.exists:
    mov eax, SYSCALL_STATUS_EXISTS
    ret
.no_space:
    mov eax, SYSCALL_STATUS_NO_SPACE
    ret
.read_only:
    mov eax, SYSCALL_STATUS_READ_ONLY
    ret
.invalid:
    mov eax, SYSCALL_STATUS_VALIDATION
    ret

; Beginn einer Aenderung. CF mit Status, wenn das Volume nicht schreibbar ist.
vfs_change_begin:
    cmp dword [nfs_busy], 0
    jne .busy
    mov dword [nfs_busy], 1
    call novafs_change_begin
    jc .failed
    mov eax, [nfs_write_count]
    mov [vfs_write_mark], eax
    clc
    ret
.failed:
    mov dword [nfs_busy], 0
    mov eax, SYSCALL_STATUS_READ_ONLY
    stc
    ret
.busy:
    mov eax, SYSCALL_STATUS_WOULD_BLOCK
    stc
    ret

; CF=0: Operation erfolgreich -> Commit. CF=1: Operation fehlgeschlagen.
; Hat ein Fehlschlag bereits geschrieben, bleibt das Volume DIRTY und wird fuer
; den Rest des Starts read-only (Phase 1 ohne Rollback); sonst wird sauber
; abgeschlossen. EAX bleibt erhalten, ausser der Commit selbst scheitert.
vfs_change_end:
    push eax
    jnc .commit
    mov eax, [nfs_write_count]
    cmp eax, [vfs_write_mark]
    jne .poisoned
.commit:
    call novafs_change_commit
    jc .poisoned_commit
    pop eax
    mov dword [nfs_busy], 0
    ret
.poisoned_commit:
    pop eax
    mov eax, SYSCALL_STATUS_IO
    push eax
.poisoned:
    mov dword [nfs_readonly], 1
    mov esi, message_vfs_volume_poisoned
    call serial_write_string
    pop eax
    mov dword [nfs_busy], 0
    ret

; ---------------------------------------------------------------------------
; VFS.Lookup (2): NovaVfsLookupArgumentsV1
;   +8 DirectoryHandle, +12 PathAddress, +16 PathLength (1..255),
;   +20 Flags (Bit 0 = Schreibzugriff), +24 ResultHandle, +28 Reserved
; ---------------------------------------------------------------------------
vfs_op_lookup:
    mov ecx, 32
    call vfs_copy_args
    jc .return
    cmp dword [vfs_args + 28], 0
    jne .reserved
    test dword [vfs_args + 20], ~VFS_LOOKUP_FLAG_WRITE
    jnz .reserved
    mov ecx, [vfs_args + 16]
    test ecx, ecx
    jz .size
    cmp ecx, 255
    ja .size
    mov edx, [vfs_args + 8]
    mov ecx, HANDLE_RIGHT_QUERY
    call vfs_resolve_handle
    jc .return
    mov [vfs_temp_object], eax
    mov esi, [vfs_args + 12]
    mov ecx, [vfs_args + 16]
    mov edi, vfs_path
    call syscall_copy_from_user
    jc .pointer

    ; "/" liefert wie bisher den Bootstrap-Root-Knoten (auch ohne NovaFS).
    cmp dword [vfs_args + 16], 1
    jne .resolve
    cmp byte [vfs_path], '/'
    jne .resolve
    test dword [vfs_args + 20], VFS_LOOKUP_FLAG_WRITE
    jnz .denied
    mov esi, vfs_path
    mov ecx, 1
    call vfs_lookup_root
    jc .not_found
    mov [vfs_temp_kobject], eax
    mov dword [vfs_temp_rights], HANDLE_RIGHT_QUERY
    cmp dword [nfs_mounted], 1
    jne .root_rights
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_FS_READ
    call security_check
    jc .root_rights
    or dword [vfs_temp_rights], HANDLE_RIGHT_READ
.root_rights:
    mov edx, [vfs_temp_kobject]
    mov ecx, [vfs_temp_rights]
    mov eax, [userspace_pid]
    mov ebx, OBJECT_TYPE_VFS_NODE
    xor esi, esi
    call handle_create
    jc .limit
    mov [vfs_args + 24], eax
    call vfs_copy_args_back
    test eax, eax
    jnz .return
    mov esi, message_vfs_lookup_ok
    call serial_write_string
    xor eax, eax
    ret

.resolve:
    cmp dword [nfs_mounted], 1
    jne .not_found
    mov edx, SECURITY_CAP_FS_READ
    call vfs_require_cap
    jc .return
    mov eax, [vfs_temp_object]
    mov esi, vfs_path
    mov ecx, [vfs_args + 16]
    call novafs_resolve_from
    jc .fs_error
    mov [vfs_temp_object], eax
    mov dword [vfs_temp_rights], HANDLE_RIGHT_QUERY | HANDLE_RIGHT_READ
    test dword [vfs_args + 20], VFS_LOOKUP_FLAG_WRITE
    jz .open
    call vfs_check_write
    jc .return
    or dword [vfs_temp_rights], HANDLE_RIGHT_WRITE
.open:
    mov eax, [vfs_temp_object]
    mov ecx, [vfs_temp_rights]
    call vfs_new_handle
    jc .return
    mov [vfs_args + 24], eax
    call vfs_copy_args_back
    test eax, eax
    jnz .return
    test byte [vfs_seen], VFS_SEEN_LOOKUP
    jnz .ok
    or byte [vfs_seen], VFS_SEEN_LOOKUP
    mov esi, message_vfs_novafs_lookup_ok
    call serial_write_string
.ok:
    xor eax, eax
    ret
.fs_error:
    call vfs_map_error
    ret
.reserved:
    mov eax, SYSCALL_STATUS_RESERVED
    ret
.size:
    mov eax, SYSCALL_STATUS_SIZE
    ret
.pointer:
    mov eax, SYSCALL_STATUS_POINTER
    ret
.denied:
    mov eax, SYSCALL_STATUS_ACCESS
    ret
.not_found:
    mov eax, SYSCALL_STATUS_NOT_FOUND
    ret
.limit:
    mov eax, SYSCALL_STATUS_LIMIT
.return:
    ret

; ---------------------------------------------------------------------------
; VFS.Read (3) / VFS.Write (4): NovaVfsIoArgumentsV1
;   +8 Handle, +12 BufferAddress, +16 Offset, +20 Length (0..4096),
;   +24 Transferred, +28 Reserved
; ---------------------------------------------------------------------------
vfs_io_prepare:
    mov ecx, 32
    call vfs_copy_args
    jc .return
    cmp dword [vfs_args + 28], 0
    jne .reserved
    cmp dword [vfs_args + 20], VFS_IO_MAX
    ja .size
    mov dword [vfs_args + 24], 0
    cmp dword [nfs_mounted], 1
    jne .not_found
    call vfs_alloc_bounce
    jc .limit
    xor eax, eax
    clc
    ret
.reserved:
    mov eax, SYSCALL_STATUS_RESERVED
    stc
    ret
.size:
    mov eax, SYSCALL_STATUS_SIZE
    stc
    ret
.not_found:
    mov eax, SYSCALL_STATUS_NOT_FOUND
    stc
    ret
.limit:
    mov eax, SYSCALL_STATUS_LIMIT
    stc
.return:
    ret

vfs_alloc_bounce:
    cmp dword [vfs_bounce_page], 0
    jne .ready
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [vfs_bounce_page], eax
.ready:
    clc
    ret
.invalid:
    stc
    ret

; EAX=ObjectID, EDX=erwarteter Typ. CF mit EAX=Status bei falschem Typ.
vfs_expect_type:
    mov [vfs_temp_type], edx
    call novafs_object_find
    jc .io
    mov eax, [nfs_object + NFS_O_TYPE]
    cmp eax, [vfs_temp_type]
    jne .wrong
    xor eax, eax
    clc
    ret
.wrong:
    mov eax, SYSCALL_STATUS_NOT_FILE
    cmp dword [vfs_temp_type], NOVAFS_TYPE_DIRECTORY
    jne .wrong_ret
    mov eax, SYSCALL_STATUS_NOT_DIRECTORY
.wrong_ret:
    stc
    ret
.io:
    call vfs_map_error
    stc
    ret

vfs_op_read:
    call vfs_io_prepare
    jc .return
    mov edx, SECURITY_CAP_FS_READ
    call vfs_require_cap
    jc .return
    mov edx, [vfs_args + 8]
    mov ecx, HANDLE_RIGHT_READ
    call vfs_resolve_handle
    jc .return
    mov [vfs_temp_object], eax
    mov edx, NOVAFS_TYPE_FILE
    call vfs_expect_type
    jc .return
    mov ecx, [vfs_args + 20]
    test ecx, ecx
    jz .copy_back
    mov eax, [vfs_temp_object]
    mov edx, [vfs_args + 16]
    mov edi, [vfs_bounce_page]
    call novafs_read
    jc .fs_error
    mov [vfs_args + 24], eax
    test eax, eax
    jz .copy_back
    mov edi, [vfs_args + 12]
    mov esi, [vfs_bounce_page]
    mov ecx, eax
    call syscall_copy_buffer_to_user
    jc .pointer
.copy_back:
    call vfs_copy_args_back
    test eax, eax
    jnz .return
    test byte [vfs_seen], VFS_SEEN_READ
    jnz .ok
    or byte [vfs_seen], VFS_SEEN_READ
    mov esi, message_vfs_read_ok
    call serial_write_string
.ok:
    xor eax, eax
    ret
.fs_error:
    call vfs_map_error
    ret
.pointer:
    mov eax, SYSCALL_STATUS_POINTER
.return:
    ret

vfs_op_write:
    call vfs_io_prepare
    jc .return
    mov edx, [vfs_args + 8]
    mov ecx, HANDLE_RIGHT_WRITE
    call vfs_resolve_handle
    jc .return
    mov [vfs_temp_object], eax
    ; Revalidierung: Capability, Mountzustand und Systembereichs-Policy
    call vfs_check_write
    jc .return
    mov eax, [vfs_temp_object]
    mov edx, NOVAFS_TYPE_FILE
    call vfs_expect_type
    jc .return
    mov ecx, [vfs_args + 20]
    test ecx, ecx
    jz .copy_back
    mov esi, [vfs_args + 12]
    mov edi, [vfs_bounce_page]
    call syscall_copy_from_user
    jc .pointer
    call vfs_change_begin
    jc .return
    mov eax, [vfs_temp_object]
    mov edx, [vfs_args + 16]
    mov ecx, [vfs_args + 20]
    mov esi, [vfs_bounce_page]
    call novafs_write
    jc .write_failed
    xor eax, eax
    clc
    call vfs_change_end
    test eax, eax
    jnz .return
    mov eax, [vfs_args + 20]
    mov [vfs_args + 24], eax
.copy_back:
    call vfs_copy_args_back
    test eax, eax
    jnz .return
    test byte [vfs_seen], VFS_SEEN_WRITE
    jnz .ok
    or byte [vfs_seen], VFS_SEEN_WRITE
    mov esi, message_vfs_write_ok
    call serial_write_string
.ok:
    xor eax, eax
    ret
.write_failed:
    call vfs_map_error
    stc
    call vfs_change_end
    ret
.pointer:
    mov eax, SYSCALL_STATUS_POINTER
.return:
    ret

; ---------------------------------------------------------------------------
; VFS.Create (5): NovaVfsCreateArgumentsV1
;   +8 DirectoryHandle (Recht WRITE), +12 NameAddress, +16 NameLength,
;   +20 ObjectType (1 Datei, 2 Verzeichnis), +24 ResultHandle, +28 Reserved
; ---------------------------------------------------------------------------
vfs_op_create:
    mov ecx, 32
    call vfs_copy_args
    jc .return
    cmp dword [vfs_args + 28], 0
    jne .reserved
    mov eax, [vfs_args + 20]
    cmp eax, NOVAFS_TYPE_FILE
    je .type_ok
    cmp eax, NOVAFS_TYPE_DIRECTORY
    jne .validation
.type_ok:
    mov ecx, [vfs_args + 16]
    test ecx, ecx
    jz .size
    cmp ecx, 255
    ja .size
    cmp dword [nfs_mounted], 1
    jne .not_found
    mov edx, [vfs_args + 8]
    mov ecx, HANDLE_RIGHT_WRITE
    call vfs_resolve_handle
    jc .return
    mov [vfs_temp_parent], eax
    call vfs_check_write
    jc .return
    mov eax, [vfs_temp_parent]
    mov edx, NOVAFS_TYPE_DIRECTORY
    call vfs_expect_type
    jc .return
    mov esi, [vfs_args + 12]
    mov ecx, [vfs_args + 16]
    mov edi, vfs_path
    call syscall_copy_from_user
    jc .pointer
    call vfs_change_begin
    jc .return
    mov eax, [vfs_temp_parent]
    mov esi, vfs_path
    mov ecx, [vfs_args + 16]
    mov edx, [vfs_args + 20]
    call novafs_create
    jc .create_failed
    mov [vfs_temp_object], eax
    xor eax, eax
    clc
    call vfs_change_end
    test eax, eax
    jnz .return
    mov eax, [vfs_temp_object]
    mov ecx, HANDLE_RIGHT_QUERY | HANDLE_RIGHT_READ | HANDLE_RIGHT_WRITE
    call vfs_new_handle
    jc .return
    mov [vfs_args + 24], eax
    call vfs_copy_args_back
    test eax, eax
    jnz .return
    test byte [vfs_seen], VFS_SEEN_CREATE
    jnz .ok
    or byte [vfs_seen], VFS_SEEN_CREATE
    mov esi, message_vfs_create_ok
    call serial_write_string
.ok:
    xor eax, eax
    ret
.create_failed:
    call vfs_map_error
    stc
    call vfs_change_end
    ret
.reserved:
    mov eax, SYSCALL_STATUS_RESERVED
    ret
.validation:
    mov eax, SYSCALL_STATUS_VALIDATION
    ret
.size:
    mov eax, SYSCALL_STATUS_SIZE
    ret
.not_found:
    mov eax, SYSCALL_STATUS_NOT_FOUND
    ret
.pointer:
    mov eax, SYSCALL_STATUS_POINTER
.return:
    ret

; ---------------------------------------------------------------------------
; VFS.ReadDirectory (6): NovaVfsDirectoryArgumentsV1
;   +8 DirectoryHandle (Recht READ), +12 Index, +16 EntryAddress,
;   +20 EntrySize (= 288), +24/+28 Reserved
; Eintrag NovaVfsDirectoryEntryV1 (288 Byte):
;   +0 StructSize, +4 Version, +8 ObjectId (u64), +16 ObjectType,
;   +20 NameLength, +24 Size (u64), +32 Name[256]
; ---------------------------------------------------------------------------
vfs_op_read_directory:
    mov ecx, 32
    call vfs_copy_args
    jc .return
    mov eax, [vfs_args + 24]
    or eax, [vfs_args + 28]
    jnz .reserved
    cmp dword [vfs_args + 20], VFS_ENTRY_SIZE
    jne .size
    cmp dword [nfs_mounted], 1
    jne .not_found
    mov edx, SECURITY_CAP_FS_READ
    call vfs_require_cap
    jc .return
    mov edx, [vfs_args + 8]
    mov ecx, HANDLE_RIGHT_READ
    call vfs_resolve_handle
    jc .return
    mov [vfs_temp_object], eax
    mov edx, NOVAFS_TYPE_DIRECTORY
    call vfs_expect_type
    jc .return
    mov eax, [vfs_temp_object]
    mov edx, [vfs_args + 12]
    call novafs_dir_entry_at
    jc .fs_error
    mov edi, vfs_entry
    xor eax, eax
    mov ecx, VFS_ENTRY_SIZE / 4
    rep stosd
    mov dword [vfs_entry + 0], VFS_ENTRY_SIZE
    mov dword [vfs_entry + 4], SYSCALL_ABI_VERSION
    mov eax, [nfs_item + NFS_D_OBJECT]
    mov [vfs_entry + 8], eax
    mov eax, [nfs_item + NFS_D_TYPE]
    mov [vfs_entry + 16], eax
    movzx ecx, word [nfs_item + NFS_D_NAME_LENGTH]
    mov [vfs_entry + 20], ecx
    mov esi, nfs_item + NFS_D_NAME
    mov edi, vfs_entry + 32
    rep movsb
    mov eax, [vfs_entry + 8]
    call novafs_object_find
    jc .fs_error
    mov eax, [nfs_object + NFS_O_SIZE]
    mov [vfs_entry + 24], eax
    mov eax, [nfs_object + NFS_O_SIZE + 4]
    mov [vfs_entry + 28], eax
    mov edi, [vfs_args + 16]
    mov esi, vfs_entry
    mov ecx, VFS_ENTRY_SIZE
    call syscall_copy_buffer_to_user
    jc .pointer
    test byte [vfs_seen], VFS_SEEN_DIRECTORY
    jnz .ok
    or byte [vfs_seen], VFS_SEEN_DIRECTORY
    mov esi, message_vfs_directory_ok
    call serial_write_string
.ok:
    xor eax, eax
    ret
.fs_error:
    call vfs_map_error
    ret
.reserved:
    mov eax, SYSCALL_STATUS_RESERVED
    ret
.size:
    mov eax, SYSCALL_STATUS_SIZE
    ret
.not_found:
    mov eax, SYSCALL_STATUS_NOT_FOUND
    ret
.pointer:
    mov eax, SYSCALL_STATUS_POINTER
.return:
    ret

; ---------------------------------------------------------------------------
; VFS.Query (7): NovaVfsObjectInfoV1 (48 Byte)
;   +8 Handle, +12 Rights, +16 ObjectId (u64), +24 ObjectType, +28 Flags,
;   +32 Size (u64), +40 Generation (u64)
; ---------------------------------------------------------------------------
vfs_op_query:
    mov ecx, VFS_INFO_SIZE
    call vfs_copy_args
    jc .return
    cmp dword [nfs_mounted], 1
    jne .not_found
    mov edx, [vfs_args + 8]
    mov ecx, HANDLE_RIGHT_QUERY
    call vfs_resolve_handle
    jc .return
    mov [vfs_args + 12], ebx
    mov [vfs_args + 16], eax
    mov dword [vfs_args + 20], 0
    call novafs_object_find
    jc .fs_error
    mov eax, [nfs_object + NFS_O_TYPE]
    mov [vfs_args + 24], eax
    mov eax, [nfs_object + NFS_O_FLAGS]
    mov [vfs_args + 28], eax
    mov eax, [nfs_object + NFS_O_SIZE]
    mov [vfs_args + 32], eax
    mov eax, [nfs_object + NFS_O_SIZE + 4]
    mov [vfs_args + 36], eax
    mov eax, [nfs_object + NFS_O_GENERATION]
    mov [vfs_args + 40], eax
    mov eax, [nfs_object + NFS_O_GENERATION + 4]
    mov [vfs_args + 44], eax
    call vfs_copy_args_back
    test eax, eax
    jnz .return
    test byte [vfs_seen], VFS_SEEN_QUERY
    jnz .ok
    or byte [vfs_seen], VFS_SEEN_QUERY
    mov esi, message_vfs_query_ok
    call serial_write_string
.ok:
    xor eax, eax
    ret
.fs_error:
    call vfs_map_error
    ret
.not_found:
    mov eax, SYSCALL_STATUS_NOT_FOUND
.return:
    ret

align 4
vfs_frame:        dd 0
vfs_args_size:    dd 0
vfs_temp_handle:  dd 0
vfs_temp_object:  dd 0
vfs_temp_parent:  dd 0
vfs_temp_rights:  dd 0
vfs_temp_kobject: dd 0
vfs_temp_type:    dd 0
vfs_write_mark:   dd 0
vfs_bounce_page:  dd 0
nfs_busy:         dd 0
vfs_seen:         dd 0
vfs_args:         times 64 db 0
vfs_path:         times 256 db 0
vfs_entry:        times VFS_ENTRY_SIZE db 0

message_vfs_novafs_lookup_ok:
    db "NOVA: Userspace VFS.Lookup auf NovaFS erfolgreich", 13, 10, 0
message_vfs_read_ok:
    db "NOVA: Userspace VFS.Read erfolgreich", 13, 10, 0
message_vfs_write_ok:
    db "NOVA: Userspace VFS.Write erfolgreich", 13, 10, 0
message_vfs_create_ok:
    db "NOVA: Userspace VFS.Create erfolgreich", 13, 10, 0
message_vfs_directory_ok:
    db "NOVA: Userspace VFS.ReadDirectory erfolgreich", 13, 10, 0
message_vfs_query_ok:
    db "NOVA: Userspace VFS.Query erfolgreich", 13, 10, 0
message_vfs_policy_denied:
    db "NOVA: VFS Schreibzugriff ausserhalb /Benutzer ohne System-Write-Authority abgewiesen", 13, 10, 0
message_vfs_volume_poisoned:
    db "NOVA: VFS Schreibfehler nach Teilaenderung, NovaFS bis zum Neustart Read-only", 13, 10, 0
