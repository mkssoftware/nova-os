
IPC_MESSAGE_SIZE   equ 16
IPC_QUEUE_CAPACITY equ 16
IPC_API_SIZE       equ 32
IPC_CAP_FIFO       equ 0x00000001
IPC_CAP_IRQ_SAFE   equ 0x00000002

ipc_initialize:
    mov dword [ipc_head], 0
    mov dword [ipc_tail], 0
    mov dword [ipc_count], 0
    mov dword [ipc_sent], 0
    mov dword [ipc_received], 0
    mov dword [ipc_last_sequence], 0
    mov dword [ipc_error], 0
    mov edi, ipc_messages
    xor eax, eax
    mov ecx, (IPC_MESSAGE_SIZE * IPC_QUEUE_CAPACITY) / 4
    rep stosd
    clc
    ret

; §015 §38: Self-Test – 7 Tests (FIFO-Invarianten, Send/Receive, Grenzen)
ipc_self_test:
    pushfd
    cli
    push ebx
    push esi
    push edi
    sub esp, 32                         ; [esp+0..15]=Sendepuffer, [esp+16..31]=Empfangspuffer
    xor ebx, ebx                        ; Fehler-Zähler

    ; Test 1: Queue leer nach init
    cmp dword [ipc_count], 0
    je .t2
    inc ebx

.t2:
    ; Test 2: ipc_send mit Testinhalt
    mov dword [esp + 0],  0xDEADBEEF
    mov dword [esp + 4],  0xCAFEBABE
    mov dword [esp + 8],  0x12345678
    mov dword [esp + 12], 0xABCDABCD
    lea esi, [esp + 0]
    call ipc_send
    cmp eax, 1
    je .t3
    inc ebx

.t3:
    ; Test 3: ipc_count == 1 nach send
    cmp dword [ipc_count], 1
    je .t4
    inc ebx

.t4:
    ; Test 4: ipc_receive
    lea edi, [esp + 16]
    call ipc_receive
    cmp eax, 1
    je .t5
    inc ebx

.t5:
    ; Test 5: empfangene Daten identisch mit gesendeten
    cmp dword [esp + 16], 0xDEADBEEF
    jne .t5_fail
    cmp dword [esp + 20], 0xCAFEBABE
    je .t6
.t5_fail:
    inc ebx

.t6:
    ; Test 6: ipc_count == 0 nach receive
    cmp dword [ipc_count], 0
    je .t7
    inc ebx

.t7:
    ; Test 7: receive auf leere Queue → EAX=0
    lea edi, [esp + 16]
    call ipc_receive
    test eax, eax
    jz .done
    inc ebx

.done:
    test ebx, ebx
    jnz .selftest_fail
    add esp, 32
    pop edi
    pop esi
    pop ebx
    popfd
    clc
    ret
.selftest_fail:
    add esp, 32
    pop edi
    pop esi
    pop ebx
    popfd
    stc
    ret

; ESI zeigt auf eine 16-Byte-Nachricht. EAX=1 bei Erfolg, sonst 0.
ipc_send:
    pushfd
    cli
    cmp dword [ipc_count], IPC_QUEUE_CAPACITY
    jae .full
    mov eax, [ipc_tail]
    imul eax, IPC_MESSAGE_SIZE
    lea edi, [ipc_messages + eax]
    mov eax, [esi + 0]
    mov [edi + 0], eax
    mov eax, [esi + 4]
    mov [edi + 4], eax
    mov eax, [esi + 8]
    mov [edi + 8], eax
    mov eax, [esi + 12]
    mov [edi + 12], eax
    inc dword [ipc_tail]
    and dword [ipc_tail], IPC_QUEUE_CAPACITY - 1
    inc dword [ipc_count]
    popfd
    mov eax, 1
    ret
.full:
    popfd
    xor eax, eax
    ret

; EDI zeigt auf 16 Byte Zielspeicher. EAX=1 bei Erfolg, sonst 0.
ipc_receive:
    pushfd
    cli
    cmp dword [ipc_count], 0
    je .empty
    mov eax, [ipc_head]
    imul eax, IPC_MESSAGE_SIZE
    lea esi, [ipc_messages + eax]
    mov eax, [esi + 0]
    mov [edi + 0], eax
    mov eax, [esi + 4]
    mov [edi + 4], eax
    mov eax, [esi + 8]
    mov [edi + 8], eax
    mov eax, [esi + 12]
    mov [edi + 12], eax
    inc dword [ipc_head]
    and dword [ipc_head], IPC_QUEUE_CAPACITY - 1
    dec dword [ipc_count]
    popfd
    mov eax, 1
    ret
.empty:
    popfd
    xor eax, eax
    ret

align 4
ipc_api:
    dd IPC_API_SIZE
    dw 1, 0
    dd IPC_MESSAGE_SIZE
    dd IPC_QUEUE_CAPACITY
    dd IPC_CAP_FIFO | IPC_CAP_IRQ_SAFE
    dd ipc_send
    dd ipc_receive
    dd ipc_count

ipc_head:          dd 0
ipc_tail:          dd 0
ipc_count:         dd 0
ipc_sent:          dd 0
ipc_received:      dd 0
ipc_last_sequence: dd 0
ipc_error:         dd 0
ipc_thread_message:
    times IPC_MESSAGE_SIZE db 0
ipc_receive_buffer:
    times IPC_MESSAGE_SIZE db 0
align 16
ipc_messages:
    times IPC_QUEUE_CAPACITY * IPC_MESSAGE_SIZE db 0

; Bootkritischer Device Manager (NPSPEC-KERNEL-0017 / NPSPEC-KERNEL-0002 Phase 8)
DEVICE_API_SIZE       equ 32
DEVICE_CAPACITY       equ 8
DEVICE_RECORD_SIZE    equ 32
DEVICE_STATE_ACTIVE   equ 1
OBJECT_TYPE_DEVICE    equ 6
DEVICE_ID             equ 0
DEVICE_CLASS          equ 4
DEVICE_STATE          equ 8
DEVICE_HANDLE         equ 12
DEVICE_RESOURCE_BASE  equ 16
DEVICE_RESOURCE_SIZE  equ 20
DEVICE_FLAGS          equ 24
DEVICE_DRIVER         equ 28

device_manager_initialize:
    mov edi, device_records
    xor eax, eax
    mov ecx, (DEVICE_CAPACITY * DEVICE_RECORD_SIZE) / 4
    rep stosd
    mov dword [device_count], 0
    mov eax, 1                      ; PIT/Timer
    mov edx, 1
    mov ebx, 0x40
    mov ecx, 1
    call device_register
    jc .invalid
    mov eax, 2                      ; PS/2-Tastaturcontroller
    mov edx, 2
    mov ebx, 0x60
    mov ecx, 5
    call device_register
    jc .invalid
    test dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_GRAPHICS
    jz .complete
    mov eax, 3                      ; übernommener Firmware-Framebuffer
    mov edx, 3
    mov ebx, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov ecx, [kernel_context + CONTEXT_PITCH]
    imul ecx, [kernel_context + CONTEXT_HEIGHT]
    call device_register
    jc .invalid
.complete:
    clc
    ret
.invalid:
    stc
    ret

; EAX=Geräte-ID, EDX=Klasse, EBX=Ressourcenbasis, ECX=Größe.
device_register:
    mov [device_temp_id], eax
    mov [device_temp_class], edx
    mov [device_temp_base], ebx
    mov [device_temp_size], ecx
    cmp dword [device_count], DEVICE_CAPACITY
    jae .invalid
    call device_lookup
    jnc .invalid
    mov eax, OBJECT_TYPE_DEVICE
    mov edx, [device_temp_id]
    xor ebx, ebx
    call object_create
    jc .invalid
    mov edx, [device_count]
    shl edx, 5
    add edx, device_records
    mov ecx, [device_temp_id]
    mov [edx + DEVICE_ID], ecx
    mov ecx, [device_temp_class]
    mov [edx + DEVICE_CLASS], ecx
    mov dword [edx + DEVICE_STATE], DEVICE_STATE_ACTIVE
    mov [edx + DEVICE_HANDLE], eax
    mov ecx, [device_temp_base]
    mov [edx + DEVICE_RESOURCE_BASE], ecx
    mov ecx, [device_temp_size]
    mov [edx + DEVICE_RESOURCE_SIZE], ecx
    mov dword [edx + DEVICE_FLAGS], 1 ; bootkritisch
    inc dword [device_count]
    clc
    ret
.invalid:
    stc
    ret

; EAX=Geräte-ID; EAX=Datensatz bei Erfolg.
device_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, [device_count]
    jae .missing
    mov edx, ecx
    shl edx, 5
    add edx, device_records
    cmp [edx + DEVICE_ID], eax
    je .found
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.missing:
    xor eax, eax
    stc
    ret

device_manager_self_test:
    cmp dword [device_count], 2
    jb .invalid
    mov eax, 1
    call device_lookup
    jc .invalid
    cmp dword [eax + DEVICE_STATE], DEVICE_STATE_ACTIVE
    jne .invalid
    mov eax, 2
    call device_lookup
    jc .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
device_api:
    dd DEVICE_API_SIZE
    dw 1, 0
    dd DEVICE_CAPACITY
    dd device_register
    dd device_lookup
    dd device_count
    dd device_records
    dd 0
device_count:      dd 0
device_temp_id:    dd 0
device_temp_class: dd 0
device_temp_base:  dd 0
device_temp_size:  dd 0
align 4
device_records:
    times DEVICE_CAPACITY * DEVICE_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Driver Framework 1.0 (NPSPEC-KERNEL-0018)
; ---------------------------------------------------------------------------
DRV_API_SIZE       equ 32
DRV_CAPACITY       equ 8
DRV_RECORD_SIZE    equ 48               ; 12 × 4-Byte-Felder

; §018 §4: Treibertypen
DRV_TYPE_KERNEL    equ 0
DRV_TYPE_USERSPACE equ 1
DRV_TYPE_BUS       equ 2
DRV_TYPE_FUNCTION  equ 3
DRV_TYPE_FILTER    equ 4
DRV_TYPE_VIRTUAL   equ 5

; §018 §21: Treiberzustände (Bootstrap-Subset)
DRV_STATE_LOADED   equ 0
DRV_STATE_BOUND    equ 2
DRV_STATE_RUNNING  equ 4
DRV_STATE_FAILED   equ 9

; Record-Offsets
DRV_ID_LO      equ 0
DRV_ID_HI      equ 4
DRV_TYPE_OFF   equ 8
DRV_STATE_OFF  equ 12
DRV_ABI_VER    equ 16
DRV_DEV_HANDLE equ 20
DRV_FLAGS_OFF  equ 24
; Bytes 28–47: reserviert (künftige Erweiterungen)

driver_framework_initialize:
    mov edi, drv_records
    xor eax, eax
    mov ecx, (DRV_CAPACITY * DRV_RECORD_SIZE) / 4
    rep stosd
    mov dword [drv_count], 0
    mov dword [drv_mgr_initialized], 1
    clc
    ret

; EAX=id_lo, EDX=id_hi → ESI=Record-Zeiger (CF=0) oder CF=1
driver_find:
    push ecx
    push edi
    xor ecx, ecx
.scan:
    cmp ecx, DRV_CAPACITY
    jae .not_found
    mov edi, ecx
    imul edi, DRV_RECORD_SIZE
    add edi, drv_records
    cmp dword [edi + DRV_ID_LO], eax
    jne .next
    cmp dword [edi + DRV_ID_HI], edx
    je .found
.next:
    inc ecx
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

; EAX=id_lo, EDX=id_hi, EBX=type, ECX=abi_version, ESI=flags
driver_register:
    push edi
    test eax, eax
    jnz .id_ok
    test edx, edx
    jz .invalid
.id_ok:
    cmp dword [drv_count], DRV_CAPACITY
    jae .invalid
    mov [drv_tmp_id_lo], eax
    mov [drv_tmp_id_hi], edx
    mov [drv_tmp_type],  ebx
    mov [drv_tmp_abi],   ecx
    mov [drv_tmp_flags], esi
    call driver_find
    jnc .invalid                        ; Duplikat abweisen
    xor ecx, ecx
.scan_slot:
    cmp ecx, DRV_CAPACITY
    jae .invalid
    mov edi, ecx
    imul edi, DRV_RECORD_SIZE
    add edi, drv_records
    cmp dword [edi + DRV_ID_LO], 0
    jne .scan_next
    cmp dword [edi + DRV_ID_HI], 0
    je .fill_slot
.scan_next:
    inc ecx
    jmp .scan_slot
.fill_slot:
    mov eax, [drv_tmp_id_lo]
    mov [edi + DRV_ID_LO], eax
    mov eax, [drv_tmp_id_hi]
    mov [edi + DRV_ID_HI], eax
    mov eax, [drv_tmp_type]
    mov [edi + DRV_TYPE_OFF], eax
    mov dword [edi + DRV_STATE_OFF], DRV_STATE_LOADED
    mov eax, [drv_tmp_abi]
    mov [edi + DRV_ABI_VER], eax
    mov dword [edi + DRV_DEV_HANDLE], 0
    mov eax, [drv_tmp_flags]
    mov [edi + DRV_FLAGS_OFF], eax
    inc dword [drv_count]
    pop edi
    clc
    ret
.invalid:
    pop edi
    stc
    ret

; EAX=id_lo, EDX=id_hi, EBX=device_handle
driver_bind:
    call driver_find
    jc .invalid
    cmp dword [esi + DRV_STATE_OFF], DRV_STATE_LOADED
    jne .invalid
    mov [esi + DRV_DEV_HANDLE], ebx
    mov dword [esi + DRV_STATE_OFF], DRV_STATE_BOUND
    clc
    ret
.invalid:
    stc
    ret

; §018 §60: Self-Test – 7 Tests
driver_framework_self_test:
    push esi
    push edi
    xor edi, edi                        ; Fehler-Zähler

    ; Test 1: initialisiert
    cmp dword [drv_mgr_initialized], 1
    je .t2
    inc edi

.t2:
    ; Test 2: drv_count == 0
    cmp dword [drv_count], 0
    je .t3
    inc edi

.t3:
    ; Test 3: driver_register – Test-Treiber-ID 0xDFDF0001
    mov eax, 0xDFDF0001
    xor edx, edx
    mov ebx, DRV_TYPE_KERNEL
    mov ecx, 0x00010000                 ; ABI 1.0
    xor esi, esi
    call driver_register
    jnc .t4
    inc edi
    jmp .done

.t4:
    ; Test 4: drv_count == 1
    cmp dword [drv_count], 1
    je .t5
    inc edi

.t5:
    ; Test 5: driver_find findet den Treiber
    mov eax, 0xDFDF0001
    xor edx, edx
    call driver_find
    jnc .t6
    inc edi

.t6:
    ; Test 6: driver_bind
    mov eax, 0xDFDF0001
    xor edx, edx
    mov ebx, 1
    call driver_bind
    jnc .t7
    inc edi

.t7:
    ; Test 7: Zustand == BOUND
    mov eax, 0xDFDF0001
    xor edx, edx
    call driver_find
    jc .t7_fail
    cmp dword [esi + DRV_STATE_OFF], DRV_STATE_BOUND
    je .done
.t7_fail:
    inc edi

.done:
    test edi, edi
    jnz .selftest_fail
    pop edi
    pop esi
    clc
    ret
.selftest_fail:
    pop edi
    pop esi
    stc
    ret

align 4
drv_api:
    dd DRV_API_SIZE
    dw 1, 0
    dd DRV_CAPACITY
    dd driver_register
    dd driver_find
    dd driver_bind
    dd drv_count
    dd drv_records

drv_mgr_initialized: dd 0
drv_count:           dd 0
drv_tmp_id_lo:       dd 0
drv_tmp_id_hi:       dd 0
drv_tmp_type:        dd 0
drv_tmp_abi:         dd 0
drv_tmp_flags:       dd 0
align 4
drv_records:
    times DRV_CAPACITY * DRV_RECORD_SIZE db 0

; Frühes VFS-Bootstrap-Root (NPSPEC-KERNEL-0019). Das RAMFS ist absichtlich
; read-only: es stellt bis zum späteren Dateisystemtreiber nur den Root-Knoten
; und dessen Mount-Namespace bereit.
VFS_API_SIZE                equ 32
VFS_STATE_SIZE              equ 32
VFS_FLAG_BOOTSTRAP_ROOT     equ 0x00000001
VFS_FLAG_READ_ONLY          equ 0x00000002
VFS_NODE_DIRECTORY          equ 1
OBJECT_TYPE_FILESYSTEM      equ 7
OBJECT_TYPE_MOUNT_NAMESPACE equ 8
OBJECT_TYPE_MOUNT           equ 9
OBJECT_TYPE_VFS_NODE        equ 10

VFS_INITIALIZED       equ 0
VFS_ROOT_READY        equ 4
VFS_FILESYSTEM_HANDLE equ 8
VFS_NAMESPACE_HANDLE  equ 12
VFS_MOUNT_HANDLE      equ 16
VFS_ROOT_NODE_HANDLE  equ 20
VFS_FLAGS             equ 24

vfs_initialize:
    mov edi, vfs_state
    xor eax, eax
    mov ecx, VFS_STATE_SIZE / 4
    rep stosd

    mov eax, OBJECT_TYPE_FILESYSTEM
    mov edx, 1                      ; Bootstrap-RAMFS-Instanz
    xor ebx, ebx
    call object_create
    jc .invalid
    mov [vfs_state + VFS_FILESYSTEM_HANDLE], eax

    mov eax, OBJECT_TYPE_MOUNT_NAMESPACE
    mov edx, 1                      ; initialer Kernel-Namespace
    xor ebx, ebx
    call object_create
    jc .invalid
    mov [vfs_state + VFS_NAMESPACE_HANDLE], eax

    mov eax, OBJECT_TYPE_MOUNT
    mov edx, 1                      ; Root-Mount
    mov ebx, [vfs_state + VFS_NAMESPACE_HANDLE]
    call object_create
    jc .invalid
    mov [vfs_state + VFS_MOUNT_HANDLE], eax

    mov eax, OBJECT_TYPE_VFS_NODE
    mov edx, 1                      ; Node-ID 1 ist das Root-Verzeichnis
    mov ebx, [vfs_state + VFS_FILESYSTEM_HANDLE]
    call object_create
    jc .invalid
    mov [vfs_state + VFS_ROOT_NODE_HANDLE], eax

    mov dword [vfs_state + VFS_FLAGS], VFS_FLAG_BOOTSTRAP_ROOT | VFS_FLAG_READ_ONLY
    mov dword [vfs_state + VFS_ROOT_READY], 1
    mov dword [vfs_state + VFS_INITIALIZED], 1
    clc
    ret
.invalid:
    stc
    ret

; ESI=Pfad, ECX=explizite Byte-Länge. Liefert in EAX den Root-Node-Handle.
; Die längenbasierte Schnittstelle akzeptiert keine implizit terminierten Pfade.
vfs_lookup_root:
    cmp dword [vfs_state + VFS_ROOT_READY], 1
    jne .missing
    cmp ecx, 1
    jne .missing
    cmp byte [esi], '/'
    jne .missing
    mov eax, [vfs_state + VFS_ROOT_NODE_HANDLE]
    test eax, eax
    jz .missing
    clc
    ret
.missing:
    xor eax, eax
    stc
    ret

vfs_self_test:
    cmp dword [vfs_state + VFS_INITIALIZED], 1
    jne .invalid
    cmp dword [vfs_state + VFS_FLAGS], VFS_FLAG_BOOTSTRAP_ROOT | VFS_FLAG_READ_ONLY
    jne .invalid
    mov esi, vfs_root_path
    mov ecx, 1
    call vfs_lookup_root
    jc .invalid
    cmp eax, [vfs_state + VFS_ROOT_NODE_HANDLE]
    jne .invalid
    mov esi, vfs_invalid_path
    mov ecx, 1
    call vfs_lookup_root
    jnc .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
vfs_api:
    dd VFS_API_SIZE
    dw 1, 0
    dd vfs_state
    dd vfs_lookup_root
    dd vfs_root_path
    dd 1
    dd VFS_FLAG_BOOTSTRAP_ROOT | VFS_FLAG_READ_ONLY
    dd 0
vfs_state:
    times VFS_STATE_SIZE db 0
vfs_root_path:    db '/'
vfs_invalid_path: db 'x'
align 4

; Kernel Service Manager (ADR-2010)
SERVICE_API_SIZE        equ 32
SERVICE_CAPACITY        equ 8
SERVICE_RECORD_SIZE     equ 32
SERVICE_STATE_EMPTY     equ 0
SERVICE_STATE_AVAILABLE equ 1
OBJECT_TYPE_SERVICE     equ 3
SERVICE_ID       equ 0
SERVICE_VERSION  equ 4
SERVICE_OWNER    equ 8
SERVICE_STATE    equ 12
SERVICE_HANDLE   equ 16
SERVICE_ENDPOINT equ 20
SERVICE_FLAGS    equ 24

service_manager_initialize:
    mov edi, service_table
    xor eax, eax
    mov ecx, (SERVICE_CAPACITY * SERVICE_RECORD_SIZE) / 4
    rep stosd
    mov edi, service_input_types
    mov ecx, SERVICE_CAPACITY * 4
    rep stosd
    mov dword [service_count], 0
    clc
    ret

; EAX=Service-ID, EDX=Version, EBX=besitzende Komponenten-ID.
service_register:
    pushfd
    cli
    test eax, eax
    jz .invalid
    mov [service_temp_id], eax
    mov [service_temp_version], edx
    mov [service_temp_owner], ebx
    mov eax, ebx
    call component_lookup
    jc .invalid
    cmp dword [eax + COMP_STATE], COMPONENT_STATE_ACTIVE
    jne .invalid
    xor ecx, ecx
.scan:
    cmp ecx, SERVICE_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 5
    add edi, service_table
    cmp dword [edi + SERVICE_STATE], SERVICE_STATE_EMPTY
    je .slot
    mov eax, [service_temp_id]
    cmp [edi + SERVICE_ID], eax
    je .invalid
    inc ecx
    jmp .scan
.slot:
    mov [service_temp_slot], edi
    mov eax, OBJECT_TYPE_SERVICE
    mov edx, [service_temp_id]
    mov ebx, [service_temp_owner]
    call object_create
    jc .invalid
    mov edi, [service_temp_slot]
    mov edx, [service_temp_id]
    mov [edi + SERVICE_ID], edx
    mov edx, [service_temp_version]
    mov [edi + SERVICE_VERSION], edx
    mov edx, [service_temp_owner]
    mov [edi + SERVICE_OWNER], edx
    mov dword [edi + SERVICE_STATE], SERVICE_STATE_AVAILABLE
    mov [edi + SERVICE_HANDLE], eax
    mov dword [edi + SERVICE_ENDPOINT], ipc_messages
    mov dword [edi + SERVICE_FLAGS], IPC_CAP_FIFO | IPC_CAP_IRQ_SAFE
    inc dword [service_count]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Service-ID. EAX=Datensatz oder 0.
service_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, SERVICE_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 5
    add edx, service_table
    cmp dword [edx + SERVICE_STATE], SERVICE_STATE_EMPTY
    je .next
    cmp [edx + SERVICE_ID], eax
    je .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Service-ID, EDX=Service-Version, EBX=Provider/Owner,
; ECX=Input Semantic Type, ESI=Output Semantic Type. Type Identity und
; Provider bleiben getrennte Contractbestandteile (ADR-SEMANTIC-0006).
service_register_typed:
    mov [service_temp_input_type], ecx
    mov [service_temp_output_type], esi
    push eax
    push edx
    push ebx
    mov eax, ecx
    call semantic_lookup_type
    jc .invalid
    cmp dword [edx + SEMANTIC_TYPE_VERSION], SEMANTIC_TYPE_VERSION_1
    jne .invalid
    mov eax, [service_temp_output_type]
    call semantic_lookup_type
    jc .invalid
    cmp dword [edx + SEMANTIC_TYPE_VERSION], SEMANTIC_TYPE_VERSION_1
    jne .invalid
    pop ebx
    pop edx
    pop eax
    call service_register
    jc .failed
    mov [service_temp_handle], eax
    mov eax, [service_temp_id]
    call service_lookup
    jc .failed
    sub eax, service_table
    shr eax, 5
    mov ecx, [service_temp_input_type]
    mov [service_input_types + eax * 4], ecx
    mov ecx, [service_temp_output_type]
    mov [service_output_types + eax * 4], ecx
    mov dword [service_input_versions + eax * 4], SEMANTIC_TYPE_VERSION_1
    mov dword [service_output_versions + eax * 4], SEMANTIC_TYPE_VERSION_1
    mov eax, [service_temp_handle]
    clc
    ret
.invalid:
    add esp, 12
.failed:
    stc
    ret

; EAX=Service-ID, ECX=gelieferter Input-Type, EDX=Type-Version.
; EBX=Output-Type bei exakter starker Kompatibilität. Eine Conversion wird
; nicht implizit erfunden; sie benötigt später einen expliziten Provider.
service_contract_check:
    push ecx
    push edx
    call service_lookup
    jc .invalid
    sub eax, service_table
    shr eax, 5
    pop edx
    pop ecx
    cmp ecx, [service_input_types + eax * 4]
    jne .mismatch
    cmp edx, [service_input_versions + eax * 4]
    jne .mismatch
    mov ebx, [service_output_types + eax * 4]
    test ebx, ebx
    jz .mismatch
    clc
    ret
.invalid:
    add esp, 8
.mismatch:
    stc
    ret

service_manager_self_test:
    mov eax, 0x4B45524E
    mov edx, 0x00010000
    mov ebx, 0x434F5245
    mov ecx, SEMANTIC_IPC_INLINE_DATA
    mov esi, SEMANTIC_IPC_DIAGNOSTIC
    call service_register_typed
    jc .invalid
    mov esi, eax
    mov eax, 0x4B45524E
    call service_lookup
    jc .invalid
    cmp dword [eax + SERVICE_VERSION], 0x00010000
    jne .invalid
    cmp dword [eax + SERVICE_OWNER], 0x434F5245
    jne .invalid
    cmp dword [eax + SERVICE_STATE], SERVICE_STATE_AVAILABLE
    jne .invalid
    cmp [eax + SERVICE_HANDLE], esi
    jne .invalid
    cmp dword [service_count], 1
    jne .invalid
    mov eax, 0x4B45524E
    mov ecx, SEMANTIC_IPC_INLINE_DATA
    mov edx, SEMANTIC_TYPE_VERSION_1
    call service_contract_check
    jc .invalid
    cmp ebx, SEMANTIC_IPC_DIAGNOSTIC
    jne .invalid
    mov eax, 0x4B45524E
    mov ecx, SEMANTIC_IPC_DIAGNOSTIC
    mov edx, SEMANTIC_TYPE_VERSION_1
    call service_contract_check
    jnc .invalid                   ; gleiche Bytes8-Repräsentation reicht nicht
    clc
    ret
.invalid:
    stc
    ret

align 4
service_manager_api:
    dd SERVICE_API_SIZE
    dw 1, 0
    dd SERVICE_CAPACITY
    dd service_register
    dd service_lookup
    dd service_count
    dd service_table
    dd ipc_api

service_count:        dd 0
service_temp_id:      dd 0
service_temp_version: dd 0
service_temp_owner:   dd 0
service_temp_slot:    dd 0
service_temp_input_type:  dd 0
service_temp_output_type: dd 0
service_temp_handle:      dd 0
align 4
service_table:
    times SERVICE_CAPACITY * SERVICE_RECORD_SIZE db 0
service_input_types:    times SERVICE_CAPACITY dd 0
service_output_types:   times SERVICE_CAPACITY dd 0
service_input_versions: times SERVICE_CAPACITY dd 0
service_output_versions: times SERVICE_CAPACITY dd 0

