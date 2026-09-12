; Nova Kernel - x86-32 Entry und früher Kernel Context
; Validiert NBHP/BIB v1, übernimmt ausschließlich TLV-Daten und ruft danach
; den minimalen Kernel Main auf.

%include "layout.inc"

[org KERNEL_ENTRY_ADDRESS]
[bits 32]

kernel_entry:
    cli
    cld
    mov esp, KERNEL_STACK_TOP
    xor ebp, ebp
    push eax
    push ebx
    lgdt [kernel_gdt_descriptor]
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    pop ebx
    pop eax
    mov dword [boot_phase_current], BOOT_PHASE_KERNEL_ENTRY
    mov dword [boot_phase_last_success], BOOT_PHASE_NONE

    push eax
    push ebx
    call serial_initialize
    call logging_initialize
    jc kernel_halt
    call logging_self_test
    jc kernel_halt
    mov esi, message_entered
    call serial_write_string
    mov esi, message_logging_ok
    call serial_write_string
    call boot_phase_log
    pop ebx
    pop eax

    push eax
    push ebx
    call panic_manager_initialize
    call panic_manager_self_test
    jc kernel_halt
    mov esi, message_panic_manager_ok
    call serial_write_string
    call crash_dump_initialize
    jc kernel_halt
    call crash_dump_self_test
    jc kernel_halt
    mov esi, message_crash_dump_ok
    call serial_write_string
    pop ebx
    pop eax

    cmp eax, NOVA_X86_BOOT_MAGIC
    jne panic_invalid_handoff
    test ebx, ebx
    jz panic_invalid_handoff
    test ebx, 7
    jnz panic_invalid_handoff

    mov dword [boot_phase_last_success], BOOT_PHASE_KERNEL_ENTRY
    mov dword [boot_phase_current], BOOT_PHASE_HANDOFF
    call boot_phase_log
    mov [kernel_context + CONTEXT_BIB], ebx
    call validate_bib
    jc panic_invalid_handoff

    call create_kernel_context
    jc panic_invalid_handoff
    call early_security_entropy_initialize
    jc panic_invalid_handoff
    mov dword [boot_phase_last_success], BOOT_PHASE_HANDOFF
    mov dword [boot_phase_current], BOOT_PHASE_EARLY_ARCH
    call boot_phase_log

    mov esi, message_bib_ok
    call serial_write_string
    test dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_KERNEL_ID
    jz .kernel_identity_done
    mov esi, message_kernel_identity_ok
    call serial_write_string
.kernel_identity_done:

    mov dword [boot_phase_last_success], BOOT_PHASE_EARLY_ARCH
    mov dword [boot_phase_current], BOOT_PHASE_EARLY_MEMORY
    call boot_phase_log
    call pmm_initialize
    jc panic_memory_manager
    call pmm_self_test
    jc panic_memory_manager
    mov esi, message_pmm_ok
    call serial_write_string

    mov dword [boot_phase_last_success], BOOT_PHASE_EARLY_MEMORY
    mov dword [boot_phase_current], BOOT_PHASE_VIRTUAL_MEMORY
    call boot_phase_log
    call paging_initialize
    jc panic_paging

    mov dword [boot_phase_last_success], BOOT_PHASE_VIRTUAL_MEMORY
    mov dword [boot_phase_current], BOOT_PHASE_KERNEL_CORE
    call boot_phase_log
    call heap_initialize
    jc panic_heap
    call heap_self_test
    jc panic_heap
    mov esi, message_heap_ok
    call serial_write_string

    call object_manager_initialize
    jc panic_object_manager
    call object_manager_self_test
    jc panic_object_manager
    mov esi, message_object_manager_ok
    call serial_write_string

    call handle_manager_initialize
    jc panic_handle_manager

    call component_manager_initialize
    jc panic_component_manager
    call component_manager_self_test
    jc panic_component_manager
    mov esi, message_component_manager_ok
    call serial_write_string

    call paging_self_test
    jc panic_paging
    mov esi, message_paging_ok
    call serial_write_string

    mov dword [boot_phase_last_success], BOOT_PHASE_KERNEL_CORE
    mov dword [boot_phase_current], BOOT_PHASE_INTERRUPTS_TIME
    call boot_phase_log
    call interrupt_initialize
    jc panic_interrupt_manager
    call timer_initialize
    jc panic_interrupt_manager
    sti
    call timer_self_test
    jc panic_interrupt_manager
    mov esi, message_interrupts_ok
    call serial_write_string

    call ipc_initialize
    jc panic_ipc
    mov esi, message_ipc_ok
    call serial_write_string

    call service_manager_initialize
    jc panic_service_manager
    call service_manager_self_test
    jc panic_service_manager
    mov esi, message_service_manager_ok
    call serial_write_string

    call process_manager_initialize
    jc panic_process_manager
    call process_manager_self_test
    jc panic_process_manager
    mov esi, message_process_manager_ok
    call serial_write_string

    call security_initialize
    jc panic_security
    call security_self_test
    jc panic_security
    mov esi, message_security_ok
    call serial_write_string

    call cpu_manager_initialize
    jc panic_cpu_manager
    call cpu_manager_self_test
    jc panic_cpu_manager
    mov esi, message_cpu_manager_ok
    call serial_write_string

    call module_loader_initialize
    jc panic_module_loader
    call module_loader_self_test
    jc panic_module_loader
    mov esi, message_module_loader_ok
    call serial_write_string

    mov dword [boot_phase_last_success], BOOT_PHASE_INTERRUPTS_TIME
    mov dword [boot_phase_current], BOOT_PHASE_SCHEDULER_SMP
    call boot_phase_log
    call scheduler_initialize
    jc panic_scheduler
    call thread_manager_initialize
    jc panic_thread_manager
    call thread_manager_self_test
    jc panic_thread_manager
    mov esi, message_thread_manager_ok
    call serial_write_string
    call scheduler_self_test
    jc panic_scheduler
    mov esi, message_scheduler_ok
    call serial_write_string

    mov dword [boot_phase_last_success], BOOT_PHASE_SCHEDULER_SMP
    mov dword [boot_phase_current], BOOT_PHASE_DEVICE_DISCOVERY
    call boot_phase_log
    call device_manager_initialize
    jc panic_device_manager
    call device_manager_self_test
    jc panic_device_manager
    mov esi, message_device_manager_ok
    call serial_write_string

    mov dword [boot_phase_last_success], BOOT_PHASE_DEVICE_DISCOVERY
    mov dword [boot_phase_current], BOOT_PHASE_ROOT_FILESYSTEM
    call boot_phase_log
    call vfs_initialize
    jc panic_vfs
    call vfs_self_test
    jc panic_vfs
    mov esi, message_vfs_ok
    call serial_write_string

    call network_manager_initialize
    jc panic_network_manager
    call network_manager_self_test
    jc panic_network_manager
    mov esi, message_network_manager_ok
    call serial_write_string

    call power_manager_initialize
    jc panic_power_manager
    call power_manager_self_test
    jc panic_power_manager
    mov esi, message_power_manager_ok
    call serial_write_string

    ; Userspace ist die nächste, noch nicht abgeschlossene Bootphase. Der Kernel
    ; meldet deshalb bewusst noch keinen operationalen Zustand (Phase 11).
    mov dword [boot_phase_last_success], BOOT_PHASE_ROOT_FILESYSTEM
    mov dword [boot_phase_current], BOOT_PHASE_USERSPACE
    call boot_phase_log
    call userspace_initialize
    jc panic_userspace
    mov esi, message_userspace_ok
    call serial_write_string
    mov dword [boot_phase_last_success], BOOT_PHASE_USERSPACE
    mov dword [boot_phase_current], BOOT_PHASE_OPERATIONAL
    call boot_phase_log
    call kernel_operational_prepare
    mov dword [scheduler_current], 0
    call userspace_enter

userspace_return:
    cli
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, KERNEL_STACK_TOP
    cmp dword [userspace_exit_seen], 1
    jne panic_userspace
    mov esi, message_userspace_exit_ok
    call serial_write_string
    call kernel_main

panic_ipc:
    mov eax, 0x00002005
    mov edx, 5
    mov esi, message_ipc_error
    jmp kernel_panic

panic_service_manager:
    mov eax, 0x00002010
    mov edx, 10
    mov esi, message_service_manager_error
    jmp kernel_panic

panic_process_manager:
    mov eax, 0x00002011
    mov edx, 11
    mov esi, message_process_manager_error
    jmp kernel_panic

panic_security:
    mov eax, 0x00002013
    mov edx, 13
    mov esi, message_security_error
    jmp kernel_panic

panic_module_loader:
    mov eax, 0x00000011
    mov edx, 0x4D4F4455             ; "MODU"
    mov esi, message_module_loader_error
    jmp kernel_panic

panic_cpu_manager:
    mov eax, 0x00000012
    mov edx, 0x43505520             ; "CPU "
    mov esi, message_cpu_manager_error
    jmp kernel_panic

panic_scheduler:
    mov eax, 0x00002004
    mov edx, 4
    mov esi, message_scheduler_error
    jmp kernel_panic

panic_thread_manager:
    mov eax, 0x00002012
    mov edx, 12
    mov esi, message_thread_manager_error
    jmp kernel_panic

panic_device_manager:
    mov eax, 0x0000200C
    mov edx, 12
    mov esi, message_device_manager_error
    jmp kernel_panic

panic_vfs:
    mov eax, 0x00002014
    mov edx, 20
    mov esi, message_vfs_error
    jmp kernel_panic

panic_power_manager:
    mov eax, 0x00002017
    mov edx, 23
    mov esi, message_power_manager_error
    jmp kernel_panic

panic_network_manager:
    mov eax, 0x00002018
    mov edx, 24
    mov esi, message_network_manager_error
    jmp kernel_panic

panic_userspace:
    mov eax, 0x00002015
    mov edx, 21
    mov esi, message_userspace_error
    jmp kernel_panic

panic_paging:
    mov eax, 0x00002002
    mov edx, 2
    mov esi, message_paging_error
    jmp kernel_panic

panic_object_manager:
    mov eax, 0x00002008
    mov edx, 8
    mov esi, message_object_manager_error
    jmp kernel_panic

panic_handle_manager:
    mov eax, 0x00002016
    mov edx, 22
    mov esi, message_handle_manager_error
    jmp kernel_panic

panic_component_manager:
    mov eax, 0x00002009
    mov edx, 9
    mov esi, message_component_manager_error
    jmp kernel_panic

panic_heap:
    mov eax, 0x00002003
    mov edx, 3
    mov esi, message_heap_error
    jmp kernel_panic

panic_interrupt_manager:
    mov eax, 0x00002007
    mov edx, 7
    mov esi, message_interrupts_error
    jmp kernel_panic

panic_memory_manager:
    mov eax, 0x00002001
    mov edx, 1
    mov esi, message_pmm_error
    jmp kernel_panic

panic_invalid_handoff:
    mov eax, 0x00001004
    mov edx, 0x1004
    mov esi, message_bib_error
    jmp kernel_panic

; ---------------------------------------------------------------------------
; BIB-Validierung
; ---------------------------------------------------------------------------

validate_bib:
    cmp dword [ebx + BIB_OFF_MAGIC_LOW], NOVA_BIB_MAGIC_LOW
    jne .invalid
    cmp dword [ebx + BIB_OFF_MAGIC_HIGH], NOVA_BIB_MAGIC_HIGH
    jne .invalid
    cmp word [ebx + BIB_OFF_VERSION_MAJOR], NOVA_BIB_VERSION_MAJOR
    jne .invalid
    cmp word [ebx + BIB_OFF_HEADER_SIZE], NOVA_BIB_HEADER_SIZE
    jne .invalid
    cmp dword [ebx + BIB_OFF_ARCHITECTURE], NKI_ARCH_X86_32
    jne .invalid

    mov ecx, [ebx + BIB_OFF_TOTAL_SIZE]
    cmp ecx, NOVA_BIB_HEADER_SIZE
    jb .invalid
    cmp ecx, BOOT_INFO_CAPACITY
    ja .invalid
    test ecx, 7
    jnz .invalid

    mov edi, [ebx + BIB_OFF_CHECKSUM]
    mov esi, ebx
    call crc32_bib
    cmp eax, edi
    jne .invalid

    clc
    ret
.invalid:
    stc
    ret

; CRC32 über den BIB; Bytes des Checksum-Felds werden als null behandelt.
crc32_bib:
    push ebx
    mov eax, 0xFFFFFFFF
    xor edx, edx
.byte:
    test ecx, ecx
    jz .finish

    xor ebx, ebx
    cmp edx, BIB_OFF_CHECKSUM
    jb .load
    cmp edx, BIB_OFF_CHECKSUM + 4
    jb .have_byte
.load:
    mov bl, [esi]
.have_byte:
    xor al, bl

    push ecx
    mov ecx, 8
.bit:
    shr eax, 1
    jnc .next_bit
    xor eax, 0xEDB88320
.next_bit:
    loop .bit
    pop ecx

    inc esi
    inc edx
    dec ecx
    jmp .byte
.finish:
    not eax
    pop ebx
    ret

; ---------------------------------------------------------------------------
; TLV -> interner Kernel Context
; ---------------------------------------------------------------------------

create_kernel_context:
    mov dword [kernel_context + CONTEXT_SEEN], 0
    mov esi, ebx
    add esi, NOVA_BIB_HEADER_SIZE
    mov edi, ebx
    add edi, [ebx + BIB_OFF_TOTAL_SIZE]

.next:
    cmp esi, edi
    je .complete
    ja .invalid

    movzx eax, word [esi + 0]       ; Type
    mov ecx, [esi + 4]              ; Length
    test ecx, 7
    jnz .invalid
    lea edx, [esi + BIB_TLV_HEADER_SIZE]
    lea ebp, [edx + ecx]
    cmp ebp, edi
    ja .invalid

    cmp eax, BIB_TLV_FIRMWARE
    je .firmware
    cmp eax, BIB_TLV_CPU
    je .cpu
    cmp eax, BIB_TLV_MEMORY
    je .memory
    cmp eax, BIB_TLV_GRAPHICS
    je .graphics
    cmp eax, BIB_TLV_KERNEL
    je .kernel
    cmp eax, BIB_TLV_SECURITY
    je .security
    cmp eax, BIB_TLV_STORAGE
    je .storage
    cmp eax, BIB_TLV_ACPI
    je .acpi
    cmp eax, BIB_TLV_BOOT_OPTIONS
    je .boot_options
    cmp eax, BIB_TLV_MODULES
    je .modules
    cmp eax, BIB_TLV_ENTROPY
    je .entropy
    cmp eax, BIB_TLV_SYSTEM
    je .system
    cmp eax, BIB_TLV_KERNEL_IDENTITY
    je .kernel_identity
    test word [esi + 2], BIB_TLV_FLAG_REQUIRED
    jnz .invalid
    jmp .advance                    ; unbekannte optionale TLVs überspringen

.firmware:
    cmp ecx, BIB_FIRMWARE_SIZE
    jb .invalid
    or dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_FIRMWARE
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_PLATFORM], eax
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_BOOT_DRIVE], eax
    jmp .advance

.memory:
    cmp ecx, BIB_MEMORY_SIZE
    jb .invalid
    or dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_MEMORY
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_MEMORY_MAP], eax
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_MEMORY_COUNT], eax
    mov eax, [edx + 8]
    mov [kernel_context + CONTEXT_MEMORY_ENTRY_SIZE], eax
    cmp eax, MEMORY_MAP_ENTRY_SIZE
    jne .invalid
    cmp dword [kernel_context + CONTEXT_MEMORY_COUNT], 0
    je .invalid
    jmp .advance

.cpu:
    cmp ecx, BIB_CPU_SIZE
    jb .invalid
    or dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_CPU
    mov eax, [edx + 16]
    mov [kernel_context + CONTEXT_CPU_FEATURE_EDX], eax
    mov eax, [edx + 20]
    mov [kernel_context + CONTEXT_CPU_FEATURE_ECX], eax
    jmp .advance

.graphics:
    cmp ecx, BIB_GRAPHICS_SIZE
    jb .invalid
    or dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_GRAPHICS
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_FRAMEBUFFER], eax
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_PITCH], eax
    mov eax, [edx + 8]
    mov [kernel_context + CONTEXT_WIDTH], eax
    mov eax, [edx + 12]
    mov [kernel_context + CONTEXT_HEIGHT], eax
    mov eax, [edx + 16]
    mov [kernel_context + CONTEXT_BPP], eax
    mov eax, [edx + 20]
    mov [kernel_context + CONTEXT_PIXEL_FORMAT], eax
    jmp .advance

.kernel:
    cmp ecx, BIB_KERNEL_SIZE
    jb .invalid
    or dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_KERNEL
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_KERNEL_ADDRESS], eax
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_KERNEL_SIZE], eax
    mov eax, [edx + 8]
    mov [kernel_context + CONTEXT_KERNEL_ENTRY], eax
    cmp dword [kernel_context + CONTEXT_KERNEL_ADDRESS], KERNEL_ENTRY_ADDRESS
    jne .invalid
    cmp dword [kernel_context + CONTEXT_KERNEL_ENTRY], KERNEL_ENTRY_ADDRESS
    jne .invalid
    jmp .advance

.kernel_identity:
    cmp ecx, BIB_KERNEL_IDENTITY_SIZE
    jb .invalid
    or dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_KERNEL_ID
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_KERNEL_BUILD_ID + 0], eax
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_KERNEL_BUILD_ID + 4], eax
    mov eax, [edx + 8]
    mov [kernel_context + CONTEXT_KERNEL_BUILD_ID + 8], eax
    mov eax, [edx + 12]
    mov [kernel_context + CONTEXT_KERNEL_BUILD_ID + 12], eax
    mov eax, [edx + 16]
    mov [kernel_context + CONTEXT_KERNEL_BUILD_ID + 16], eax
    mov eax, [edx + 20]
    mov [kernel_context + CONTEXT_KERNEL_FORMAT], eax
    jmp .advance

.security:
    cmp ecx, BIB_SECURITY_SIZE
    jb .invalid
    mov eax, [edx]
    mov [kernel_context + CONTEXT_SECURITY_STATE], eax
    mov eax, [edx + 8]
    mov [kernel_context + CONTEXT_ENTROPY_QUALITY], eax
    jmp .advance

.storage:
    cmp ecx, BIB_STORAGE_SIZE
    jb .invalid
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_BOOT_DRIVE], eax
    jmp .advance

.acpi:
    cmp ecx, BIB_ACPI_SIZE
    jb .invalid
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_ACPI_ADDRESS], eax
    jmp .advance

.modules:
    cmp ecx, BIB_MODULES_SIZE
    jb .invalid
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_MODULES_ADDRESS], eax
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_MODULE_COUNT], eax
    jmp .advance

.boot_options:
    cmp ecx, BIB_BOOT_OPTIONS_SIZE
    jb .invalid
    jmp .advance

.entropy:
    cmp ecx, BIB_ENTROPY_SIZE
    jb .invalid
    cmp dword [edx + 24], 16
    jb .invalid
    or dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_ENTROPY
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_ENTROPY_SEED + 0], eax
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_ENTROPY_SEED + 4], eax
    mov eax, [edx + 8]
    mov [kernel_context + CONTEXT_ENTROPY_SEED + 8], eax
    mov eax, [edx + 12]
    mov [kernel_context + CONTEXT_ENTROPY_SEED + 12], eax
    mov eax, [edx + 20]
    mov [kernel_context + CONTEXT_ENTROPY_QUALITY], eax
    jmp .advance

.system:
    cmp ecx, BIB_SYSTEM_SIZE
    jb .invalid
    or dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_SYSTEM
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_SYSTEM_GENERATION], eax
    mov eax, [edx + 8]
    mov [kernel_context + CONTEXT_BOOT_ATTEMPT], eax
    jmp .advance

.advance:
    mov esi, ebp
    jmp .next

.complete:
    mov eax, [kernel_context + CONTEXT_SEEN]
    and eax, CONTEXT_REQUIRED
    cmp eax, CONTEXT_REQUIRED
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

early_security_entropy_initialize:
    mov eax, [kernel_context + CONTEXT_ENTROPY_SEED + 0]
    xor eax, [kernel_context + CONTEXT_ENTROPY_SEED + 4]
    rol eax, 9
    xor eax, [kernel_context + CONTEXT_ENTROPY_SEED + 8]
    rol eax, 13
    xor eax, [kernel_context + CONTEXT_ENTROPY_SEED + 12]
    test eax, eax
    jz .invalid
    mov [stack_canary_seed], eax
    clc
    ret
.invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; Physischer Bootstrap-Speichermanager (ADR-2001)
; ---------------------------------------------------------------------------

PMM_PAGE_SIZE        equ 4096
PMM_MAX_FRAMES       equ 1024
PMM_API_SIZE         equ 32
PMM_API_ABI_MAJOR    equ 1
PMM_API_ABI_MINOR    equ 0
PMM_CAP_E820         equ 0x00000001
PMM_CAP_LIFO_FRAMES  equ 0x00000002

PMM_API_STRUCT_SIZE  equ 0
PMM_API_ABI          equ 4
PMM_API_PAGE_SIZE    equ 8
PMM_API_CAPABILITIES equ 12
PMM_API_ALLOC        equ 16
PMM_API_FREE         equ 20
PMM_API_TOTAL        equ 24
PMM_API_AVAILABLE    equ 28

pmm_initialize:
    mov dword [pmm_frame_count], 0

    mov eax, [kernel_context + CONTEXT_KERNEL_ADDRESS]
    add eax, [kernel_context + CONTEXT_KERNEL_SIZE]
    jc .invalid
    add eax, PMM_PAGE_SIZE - 1
    and eax, 0xFFFFF000
    mov [pmm_reserved_end], eax

    mov esi, [kernel_context + CONTEXT_MEMORY_MAP]
    mov ecx, [kernel_context + CONTEXT_MEMORY_COUNT]
.entry:
    test ecx, ecx
    jz .complete
    cmp dword [esi + 16], 1         ; E820: usable RAM
    jne .next_entry
    cmp dword [esi + 4], 0          ; Bootstrap-PMM verwaltet zunächst < 4 GiB
    jne .next_entry

    mov eax, [esi + 0]
    add eax, PMM_PAGE_SIZE - 1
    jc .next_entry
    and eax, 0xFFFFF000
    cmp eax, [pmm_reserved_end]
    jae .start_ready
    mov eax, [pmm_reserved_end]
.start_ready:
    mov ebx, [esi + 0]
    mov edx, [esi + 8]
    add ebx, edx
    jc .end_maximum
    cmp dword [esi + 12], 0
    jne .end_maximum
    and ebx, 0xFFFFF000
    jmp .collect
.end_maximum:
    mov ebx, 0xFFFFF000

.collect:
    cmp eax, ebx
    jae .next_entry
    cmp dword [pmm_frame_count], PMM_MAX_FRAMES
    jae .complete
    mov edi, [pmm_frame_count]
    mov [pmm_frames + edi * 4], eax
    inc dword [pmm_frame_count]
    add eax, PMM_PAGE_SIZE
    jc .next_entry
    jmp .collect

.next_entry:
    add esi, MEMORY_MAP_ENTRY_SIZE
    dec ecx
    jmp .entry

.complete:
    cmp dword [pmm_frame_count], 2
    jb .invalid
    mov eax, [pmm_frame_count]
    mov [pmm_api + PMM_API_TOTAL], eax
    mov [pmm_api + PMM_API_AVAILABLE], eax
    clc
    ret
.invalid:
    stc
    ret

; EAX = physische 4-KiB-Seite, EAX=0 bei Erschöpfung.
pmm_alloc_page:
    mov ecx, [pmm_frame_count]
    test ecx, ecx
    jz .empty
    dec ecx
    mov [pmm_frame_count], ecx
    mov eax, [pmm_frames + ecx * 4]
    mov [pmm_api + PMM_API_AVAILABLE], ecx
    ret
.empty:
    xor eax, eax
    ret

; EAX = freizugebende Seite. CF meldet ungültige oder volle Übergabe.
pmm_free_page:
    test eax, PMM_PAGE_SIZE - 1
    jnz .invalid
    cmp eax, [pmm_reserved_end]
    jb .invalid
    mov ecx, [pmm_frame_count]
    cmp ecx, PMM_MAX_FRAMES
    jae .invalid
    mov [pmm_frames + ecx * 4], eax
    inc ecx
    mov [pmm_frame_count], ecx
    mov [pmm_api + PMM_API_AVAILABLE], ecx
    clc
    ret
.invalid:
    stc
    ret

; Deterministischer Starttest: zwei verschiedene Seiten entnehmen und in
; umgekehrter Reihenfolge zurückgeben. Der freie Zähler muss identisch bleiben.
pmm_self_test:
    mov ebp, [pmm_frame_count]
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov ebx, eax
    call pmm_alloc_page
    test eax, eax
    jz .restore_first
    cmp eax, ebx
    je .restore_both
    mov edx, eax
    mov eax, edx
    call pmm_free_page
    jc .invalid
    mov eax, ebx
    call pmm_free_page
    jc .invalid
    cmp [pmm_frame_count], ebp
    jne .invalid
    clc
    ret
.restore_both:
    call pmm_free_page
.restore_first:
    mov eax, ebx
    call pmm_free_page
.invalid:
    stc
    ret

; Öffentliche, versionierte Bootstrap-API. Die Implementierung bleibt intern
; austauschbar; Verbraucher prüfen StructSize und ABI vor der Verwendung.
align 4
pmm_api:
    dd PMM_API_SIZE
    dw PMM_API_ABI_MAJOR, PMM_API_ABI_MINOR
    dd PMM_PAGE_SIZE
    dd PMM_CAP_E820 | PMM_CAP_LIFO_FRAMES
    dd pmm_alloc_page
    dd pmm_free_page
    dd 0
    dd 0

pmm_reserved_end: dd 0
pmm_frame_count:  dd 0
align 16
pmm_frames:
    times PMM_MAX_FRAMES dd 0

; ---------------------------------------------------------------------------
; Bootstrap-Kernel-Heap (ADR-2003)
; ---------------------------------------------------------------------------

HEAP_ALIGNMENT       equ 16
HEAP_API_SIZE        equ 32
HEAP_API_ABI_MAJOR   equ 1
HEAP_API_ABI_MINOR   equ 0
HEAP_CAP_PAGE_BACKED equ 0x00000001
HEAP_CAP_ZEROED      equ 0x00000002

heap_initialize:
    mov dword [heap_page], 0
    mov dword [heap_offset], PMM_PAGE_SIZE
    mov dword [heap_allocations], 0
    mov dword [heap_bytes], 0
    clc
    ret

; ECX=Größe, EAX=Adresse oder 0. Bootstrap-Allokationen sind maximal eine
; Seite groß und werden auf 16 Byte ausgerichtet.
heap_allocate:
    test ecx, ecx
    jz .invalid
    cmp ecx, PMM_PAGE_SIZE
    ja .invalid
    add ecx, HEAP_ALIGNMENT - 1
    jc .invalid
    and ecx, -HEAP_ALIGNMENT

    mov edx, [heap_offset]
    mov ebx, edx
    add ebx, ecx
    jc .invalid
    cmp ebx, PMM_PAGE_SIZE
    jbe .have_space

    push ecx
    call pmm_alloc_page
    pop ecx
    test eax, eax
    jz .invalid
    mov [heap_page], eax
    mov dword [heap_offset], 0
    xor edx, edx

    ; Neue Heap-Seiten werden deterministisch genullt.
    push ecx
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    pop ecx

.have_space:
    mov eax, [heap_page]
    add eax, edx
    add edx, ecx
    mov [heap_offset], edx
    inc dword [heap_allocations]
    add [heap_bytes], ecx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

heap_self_test:
    mov ecx, 64
    call heap_allocate
    jc .invalid
    test eax, HEAP_ALIGNMENT - 1
    jnz .invalid
    mov esi, eax
    mov dword [esi], 0x4E4F5641
    mov dword [esi + 60], 0x48454150

    mov ecx, 128
    call heap_allocate
    jc .invalid
    test eax, HEAP_ALIGNMENT - 1
    jnz .invalid
    cmp eax, esi
    jbe .invalid
    cmp dword [esi], 0x4E4F5641
    jne .invalid
    cmp dword [esi + 60], 0x48454150
    jne .invalid
    cmp dword [eax], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
heap_api:
    dd HEAP_API_SIZE
    dw HEAP_API_ABI_MAJOR, HEAP_API_ABI_MINOR
    dd HEAP_ALIGNMENT
    dd HEAP_CAP_PAGE_BACKED | HEAP_CAP_ZEROED
    dd heap_allocate
    dd 0                             ; Free ist im Bootstrap-Heap nicht verfügbar
    dd heap_allocations
    dd heap_bytes

heap_page:        dd 0
heap_offset:      dd 0
heap_allocations: dd 0
heap_bytes:       dd 0

; ---------------------------------------------------------------------------
; Strukturiertes Early-/Kernel-Logging (NPSPEC-KERNEL-0023)
; ---------------------------------------------------------------------------
LOG_RECORD_SIZE       equ 32
LOG_RING_CAPACITY     equ 8
LOG_COMMIT_MARKER     equ 0x43474F4C ; "LOGC"
LOG_LEVEL_CRITICAL    equ 6
LOG_FLAG_EARLY_TIME  equ 1

logging_initialize:
    mov edi, logging_ring
    xor eax, eax
    mov ecx, (LOG_RING_CAPACITY * LOG_RECORD_SIZE) / 4
    rep stosd
    mov edi, logging_critical_record
    mov ecx, LOG_RECORD_SIZE / 4
    rep stosd
    mov dword [logging_write_index], 0
    mov dword [logging_record_count], 0
    mov dword [logging_written_records], 0
    mov dword [logging_dropped_records], 0
    mov dword [logging_overwritten_records], 0
    mov dword [logging_critical_records], 0
    mov dword [logging_recursion_events], 0
    mov dword [logging_recursion_guard], 0
    clc
    ret

; EAX=Level, EDX=Kategorie, EBX=Komponente, ECX=Event-Code.
; Der Commit-Marker wird als letztes Feld atomar sichtbar gemacht.
logging_write:
    cmp eax, 7
    ja .invalid
    cmp edx, 13
    ja .invalid
    cmp dword [logging_recursion_guard], 0
    jne .recursive
    mov dword [logging_recursion_guard], 1
    mov [logging_temp_level], eax
    mov [logging_temp_category], edx
    mov [logging_temp_component], ebx
    mov [logging_temp_event], ecx
    mov edi, [logging_write_index]
    shl edi, 5
    add edi, logging_ring
    mov dword [edi + 28], 0
    inc dword [logging_written_records]
    mov eax, [logging_written_records]
    mov [edi + 0], eax
    mov eax, [logging_temp_level]
    mov [edi + 4], eax
    mov eax, [logging_temp_category]
    mov [edi + 8], eax
    mov eax, [logging_temp_component]
    mov [edi + 12], eax
    mov eax, [logging_temp_event]
    mov [edi + 16], eax
    mov eax, [timer_ticks]
    mov [edi + 20], eax
    xor eax, eax
    cmp dword [timer_ticks], 0
    jne .timestamp_ready
    mov eax, LOG_FLAG_EARLY_TIME
.timestamp_ready:
    mov [edi + 24], eax
    mov dword [edi + 28], LOG_COMMIT_MARKER
    cmp dword [logging_record_count], LOG_RING_CAPACITY
    jb .grow
    inc dword [logging_dropped_records]
    inc dword [logging_overwritten_records]
    jmp .advance
.grow:
    inc dword [logging_record_count]
.advance:
    inc dword [logging_write_index]
    and dword [logging_write_index], LOG_RING_CAPACITY - 1
    cmp dword [logging_temp_level], LOG_LEVEL_CRITICAL
    jb .complete
    inc dword [logging_critical_records]
    mov esi, edi
    mov edi, logging_critical_record
    mov ecx, LOG_RECORD_SIZE / 4
    rep movsd
.complete:
    mov dword [logging_recursion_guard], 0
    clc
    ret
.recursive:
    inc dword [logging_recursion_events]
.invalid:
    mov dword [logging_recursion_guard], 0
    stc
    ret

logging_self_test:
    mov dword [logging_test_iteration], 0
.next:
    mov eax, [logging_test_iteration]
    cmp eax, 10
    jae .verify
    and eax, 7
    mov edx, 1                     ; Kernel-Kategorie
    mov ebx, 1                     ; Kernel-Core-Komponente
    mov ecx, [logging_test_iteration]
    add ecx, 0x100
    call logging_write
    jc .invalid
    inc dword [logging_test_iteration]
    jmp .next
.verify:
    cmp dword [logging_written_records], 10
    jne .invalid
    cmp dword [logging_record_count], LOG_RING_CAPACITY
    jne .invalid
    cmp dword [logging_dropped_records], 2
    jne .invalid
    cmp dword [logging_overwritten_records], 2
    jne .invalid
    mov eax, [logging_write_index]
    dec eax
    and eax, LOG_RING_CAPACITY - 1
    shl eax, 5
    add eax, logging_ring
    cmp dword [eax + 0], 10
    jne .invalid
    cmp dword [eax + 28], LOG_COMMIT_MARKER
    jne .invalid
    cmp dword [logging_critical_records], 2
    jne .invalid
    cmp dword [logging_critical_record + 28], LOG_COMMIT_MARKER
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
logging_write_index:        dd 0
logging_record_count:       dd 0
logging_written_records:    dd 0
logging_dropped_records:    dd 0
logging_overwritten_records: dd 0
logging_critical_records:   dd 0
logging_recursion_events:   dd 0
logging_recursion_guard:    dd 0
logging_test_iteration:     dd 0
logging_temp_level:         dd 0
logging_temp_category:      dd 0
logging_temp_component:     dd 0
logging_temp_event:         dd 0
logging_ring:               times LOG_RING_CAPACITY * LOG_RECORD_SIZE db 0
logging_critical_record:    times LOG_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Kernel Object Manager (ADR-2008)
; ---------------------------------------------------------------------------

OBJECT_API_SIZE       equ 32
OBJECT_API_ABI_MAJOR  equ 1
OBJECT_API_ABI_MINOR  equ 0
OBJECT_TABLE_CAPACITY equ 32
OBJECT_HEADER_SIZE    equ 32
OBJECT_STATE_LIVE     equ 1

OBJ_STRUCT_SIZE equ 0
OBJ_ABI         equ 4
OBJ_TYPE        equ 8
OBJ_STATE       equ 12
OBJ_REFCOUNT    equ 16
OBJ_HANDLE      equ 20
OBJ_PAYLOAD0    equ 24
OBJ_PAYLOAD1    equ 28

object_manager_initialize:
    mov edi, object_table
    xor eax, eax
    mov ecx, OBJECT_TABLE_CAPACITY
    rep stosd
    mov edi, object_generations
    mov eax, 1
    mov ecx, OBJECT_TABLE_CAPACITY
    rep stosd
    mov dword [object_live_count], 0
    clc
    ret

; EAX=Typ, EDX/EBX=optionale Nutzwerte. EAX=Handle oder 0.
object_create:
    test eax, eax
    jz .invalid
    push eax
    push edx
    push ebx
    xor edi, edi
.find_slot:
    cmp edi, OBJECT_TABLE_CAPACITY
    jae .full
    cmp dword [object_table + edi * 4], 0
    je .allocate
    inc edi
    jmp .find_slot
.allocate:
    mov ecx, OBJECT_HEADER_SIZE
    call heap_allocate
    jc .full
    pop ebx
    pop edx
    pop ecx
    mov dword [eax + OBJ_STRUCT_SIZE], OBJECT_HEADER_SIZE
    mov dword [eax + OBJ_ABI], 1
    mov [eax + OBJ_TYPE], ecx
    mov dword [eax + OBJ_STATE], OBJECT_STATE_LIVE
    mov dword [eax + OBJ_REFCOUNT], 1
    mov [eax + OBJ_PAYLOAD0], edx
    mov [eax + OBJ_PAYLOAD1], ebx
    mov ecx, [object_generations + edi * 4]
    shl ecx, 16
    lea edx, [edi + 1]
    or ecx, edx
    mov [eax + OBJ_HANDLE], ecx
    mov [object_table + edi * 4], eax
    inc dword [object_live_count]
    mov eax, ecx
    clc
    ret
.full:
    add esp, 12
.invalid:
    xor eax, eax
    stc
    ret

; EAX=Handle. EAX=Objektadresse oder 0.
object_lookup:
    mov edx, eax
    and edx, 0xFFFF
    jz .invalid
    dec edx
    cmp edx, OBJECT_TABLE_CAPACITY
    jae .invalid
    mov ecx, eax
    shr ecx, 16
    cmp ecx, [object_generations + edx * 4]
    jne .invalid
    mov eax, [object_table + edx * 4]
    test eax, eax
    jz .invalid
    cmp dword [eax + OBJ_STATE], OBJECT_STATE_LIVE
    jne .invalid
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

object_retain:
    push eax
    call object_lookup
    jc .invalid
    inc dword [eax + OBJ_REFCOUNT]
    add esp, 4
    clc
    ret
.invalid:
    add esp, 4
    stc
    ret

object_release:
    push eax
    call object_lookup
    jc .invalid
    cmp dword [eax + OBJ_REFCOUNT], 0
    je .invalid
    dec dword [eax + OBJ_REFCOUNT]
    jnz .done
    mov dword [eax + OBJ_STATE], 0
    mov edx, [esp]
    and edx, 0xFFFF
    dec edx
    mov dword [object_table + edx * 4], 0
    inc dword [object_generations + edx * 4]
    and dword [object_generations + edx * 4], 0xFFFF
    jnz .generation_ok
    mov dword [object_generations + edx * 4], 1
.generation_ok:
    dec dword [object_live_count]
.done:
    add esp, 4
    clc
    ret
.invalid:
    add esp, 4
    stc
    ret

object_manager_self_test:
    mov eax, 1
    mov edx, 0x4E4F5641
    mov ebx, 0x4F424A31
    call object_create
    jc .invalid
    mov esi, eax
    call object_lookup
    jc .invalid
    cmp dword [eax + OBJ_TYPE], 1
    jne .invalid
    cmp dword [eax + OBJ_PAYLOAD0], 0x4E4F5641
    jne .invalid
    mov eax, esi
    call object_retain
    jc .invalid
    cmp dword [eax + OBJ_REFCOUNT], 2
    jne .invalid
    mov eax, esi
    call object_release
    jc .invalid
    mov eax, esi
    call object_release
    jc .invalid
    mov eax, esi
    call object_lookup
    jnc .invalid
    cmp dword [object_live_count], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
object_manager_api:
    dd OBJECT_API_SIZE
    dw OBJECT_API_ABI_MAJOR, OBJECT_API_ABI_MINOR
    dd OBJECT_TABLE_CAPACITY
    dd object_create
    dd object_lookup
    dd object_retain
    dd object_release
    dd object_live_count

object_live_count: dd 0
align 4
object_table:       times OBJECT_TABLE_CAPACITY dd 0
object_generations: times OBJECT_TABLE_CAPACITY dd 0

; ---------------------------------------------------------------------------
; Prozesslokaler Handle Manager (NPSPEC-KERNEL-0013)
; ---------------------------------------------------------------------------
HANDLE_API_SIZE       equ 32
HANDLE_CAPACITY       equ 16
HANDLE_ENTRY_SIZE     equ 24
HANDLE_STATE_ACTIVE   equ 1
HANDLE_RIGHT_QUERY    equ 0x00000001
HANDLE_RIGHT_WAIT     equ 0x00000002
HANDLE_RIGHT_SEND     equ 0x00000004
HANDLE_RIGHT_RECEIVE  equ 0x00000008
HANDLE_RIGHT_BIND     equ 0x00000010
HANDLE_RIGHT_CONNECT  equ 0x00000020
HANDLE_FLAG_PROTECTED equ 0x00000010
HANDLE_OBJECT equ 0
HANDLE_RIGHTS equ 4
HANDLE_FLAGS  equ 8
HANDLE_GEN    equ 12
HANDLE_TYPE   equ 16
HANDLE_STATE  equ 20

handle_manager_initialize:
    mov edi, handle_table
    xor eax, eax
    mov ecx, (HANDLE_CAPACITY * HANDLE_ENTRY_SIZE) / 4
    rep stosd
    mov edi, handle_generations
    mov eax, 1
    mov ecx, HANDLE_CAPACITY
    rep stosd
    mov dword [handle_owner_pid], 0
    mov dword [handle_active_count], 0
    clc
    ret

; EAX=PID, EDX=Kernelobjekthandle, EBX=Typ, ECX=Rechte, ESI=Flags.
; EAX=prozesslokaler, generationsgeschützter Handle.
handle_create:
    cmp eax, [handle_owner_pid]
    jne .invalid
    test eax, eax
    jz .invalid
    test ecx, ecx
    jz .invalid
    mov [handle_temp_object], edx
    mov [handle_temp_type], ebx
    mov [handle_temp_rights], ecx
    mov [handle_temp_flags], esi
    mov eax, edx
    call object_lookup
    jc .invalid
    mov ecx, [handle_temp_type]
    cmp [eax + OBJ_TYPE], ecx
    jne .invalid
    xor edi, edi
.scan:
    cmp edi, HANDLE_CAPACITY
    jae .invalid
    mov eax, edi
    imul eax, HANDLE_ENTRY_SIZE
    add eax, handle_table
    cmp dword [eax + HANDLE_STATE], 0
    je .slot
    inc edi
    jmp .scan
.slot:
    mov [handle_temp_slot], eax
    mov eax, [handle_temp_object]
    call object_retain
    jc .invalid
    mov eax, [handle_temp_slot]
    mov edx, [handle_temp_object]
    mov [eax + HANDLE_OBJECT], edx
    mov edx, [handle_temp_rights]
    mov [eax + HANDLE_RIGHTS], edx
    mov edx, [handle_temp_flags]
    mov [eax + HANDLE_FLAGS], edx
    mov edx, [handle_generations + edi * 4]
    mov [eax + HANDLE_GEN], edx
    mov edx, [handle_temp_type]
    mov [eax + HANDLE_TYPE], edx
    mov dword [eax + HANDLE_STATE], HANDLE_STATE_ACTIVE
    inc dword [handle_active_count]
    mov eax, [handle_generations + edi * 4]
    shl eax, 16
    lea edx, [edi + 1]
    or eax, edx
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=PID, EDX=Handle, EBX=erwarteter Typ, ECX=benötigte Rechte.
; EAX=interne Objektadresse ausschließlich für den Kernel.
handle_resolve:
    cmp eax, [handle_owner_pid]
    jne .invalid
    mov edi, edx
    and edi, 0xFFFF
    jz .invalid
    dec edi
    cmp edi, HANDLE_CAPACITY
    jae .invalid
    mov esi, edi
    imul esi, HANDLE_ENTRY_SIZE
    add esi, handle_table
    cmp dword [esi + HANDLE_STATE], HANDLE_STATE_ACTIVE
    jne .invalid
    mov eax, edx
    shr eax, 16
    cmp eax, [esi + HANDLE_GEN]
    jne .invalid
    cmp ebx, [esi + HANDLE_TYPE]
    jne .invalid
    mov eax, [esi + HANDLE_RIGHTS]
    and eax, ecx
    cmp eax, ecx
    jne .invalid
    mov eax, [esi + HANDLE_OBJECT]
    call object_lookup
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=PID, EDX=Handle. Geschützte Handles können nicht regulär geschlossen werden.
handle_close:
    cmp eax, [handle_owner_pid]
    jne .invalid
    mov edi, edx
    and edi, 0xFFFF
    jz .invalid
    dec edi
    cmp edi, HANDLE_CAPACITY
    jae .invalid
    mov esi, edi
    imul esi, HANDLE_ENTRY_SIZE
    add esi, handle_table
    cmp dword [esi + HANDLE_STATE], HANDLE_STATE_ACTIVE
    jne .invalid
    mov eax, edx
    shr eax, 16
    cmp eax, [esi + HANDLE_GEN]
    jne .invalid
    test dword [esi + HANDLE_FLAGS], HANDLE_FLAG_PROTECTED
    jnz .invalid
    mov eax, [esi + HANDLE_OBJECT]
    call object_release
    jc .invalid
    mov dword [esi + HANDLE_STATE], 0
    inc dword [handle_generations + edi * 4]
    and dword [handle_generations + edi * 4], 0xFFFF
    jnz .generation_ok
    mov dword [handle_generations + edi * 4], 1
.generation_ok:
    dec dword [handle_active_count]
    clc
    ret
.invalid:
    stc
    ret

align 4
handle_manager_api:
    dd HANDLE_API_SIZE
    dw 1, 0
    dd HANDLE_CAPACITY
    dd handle_create
    dd handle_resolve
    dd handle_close
    dd handle_active_count
    dd handle_table
handle_owner_pid:    dd 0
handle_active_count: dd 0
handle_temp_object: dd 0
handle_temp_type:   dd 0
handle_temp_rights: dd 0
handle_temp_flags:  dd 0
handle_temp_slot:   dd 0
align 4
handle_generations: times HANDLE_CAPACITY dd 0
handle_table: times HANDLE_CAPACITY * HANDLE_ENTRY_SIZE db 0

; ---------------------------------------------------------------------------
; Kernel Component Manager (ADR-2009)
; ---------------------------------------------------------------------------

COMPONENT_API_SIZE       equ 32
COMPONENT_API_ABI_MAJOR  equ 1
COMPONENT_API_ABI_MINOR  equ 0
COMPONENT_CAPACITY       equ 8
COMPONENT_RECORD_SIZE    equ 32
COMPONENT_STATE_EMPTY    equ 0
COMPONENT_STATE_REGISTERED equ 1
COMPONENT_STATE_ACTIVE   equ 2
OBJECT_TYPE_COMPONENT    equ 2

COMP_ID       equ 0
COMP_VERSION  equ 4
COMP_FLAGS    equ 8
COMP_STATE    equ 12
COMP_HANDLE   equ 16
COMP_RESERVED equ 20

component_manager_initialize:
    mov edi, component_table
    xor eax, eax
    mov ecx, (COMPONENT_CAPACITY * COMPONENT_RECORD_SIZE) / 4
    rep stosd
    mov dword [component_count], 0
    clc
    ret

; EAX=stabile Komponenten-ID, EDX=Version, EBX=Flags. EAX=Objekt-Handle.
component_register:
    pushfd
    cli
    test eax, eax
    jz .invalid
    mov [component_temp_id], eax
    mov [component_temp_version], edx
    mov [component_temp_flags], ebx
    xor ecx, ecx
.scan:
    cmp ecx, COMPONENT_CAPACITY
    jae .full
    mov edi, ecx
    shl edi, 5
    add edi, component_table
    cmp dword [edi + COMP_STATE], COMPONENT_STATE_EMPTY
    je .slot
    cmp [edi + COMP_ID], eax
    je .invalid
    inc ecx
    jmp .scan
.slot:
    mov [component_temp_slot], edi
    mov eax, OBJECT_TYPE_COMPONENT
    mov edx, [component_temp_id]
    mov ebx, [component_temp_version]
    call object_create
    jc .invalid
    mov edi, [component_temp_slot]
    mov edx, [component_temp_id]
    mov [edi + COMP_ID], edx
    mov edx, [component_temp_version]
    mov [edi + COMP_VERSION], edx
    mov edx, [component_temp_flags]
    mov [edi + COMP_FLAGS], edx
    mov dword [edi + COMP_STATE], COMPONENT_STATE_REGISTERED
    mov [edi + COMP_HANDLE], eax
    inc dword [component_count]
    popfd
    clc
    ret
.full:
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=Komponenten-ID. EAX=Datensatz oder 0.
component_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, COMPONENT_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 5
    add edx, component_table
    cmp dword [edx + COMP_STATE], COMPONENT_STATE_EMPTY
    je .next
    cmp [edx + COMP_ID], eax
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

; EAX=Komponenten-ID.
component_activate:
    pushfd
    cli
    call component_lookup
    jc .invalid
    cmp dword [eax + COMP_STATE], COMPONENT_STATE_REGISTERED
    jne .invalid
    mov dword [eax + COMP_STATE], COMPONENT_STATE_ACTIVE
    popfd
    clc
    ret
.invalid:
    popfd
    stc
    ret

component_manager_self_test:
    mov eax, 0x434F5245             ; "CORE"
    mov edx, 0x00010000             ; Version 1.0
    mov ebx, 1                      ; essentielle Kernel-Komponente
    call component_register
    jc .invalid
    mov esi, eax
    mov eax, 0x434F5245
    call component_activate
    jc .invalid
    mov eax, 0x434F5245
    call component_lookup
    jc .invalid
    cmp dword [eax + COMP_VERSION], 0x00010000
    jne .invalid
    cmp dword [eax + COMP_STATE], COMPONENT_STATE_ACTIVE
    jne .invalid
    cmp [eax + COMP_HANDLE], esi
    jne .invalid
    cmp dword [component_count], 1
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
component_manager_api:
    dd COMPONENT_API_SIZE
    dw COMPONENT_API_ABI_MAJOR, COMPONENT_API_ABI_MINOR
    dd COMPONENT_CAPACITY
    dd component_register
    dd component_lookup
    dd component_activate
    dd component_count
    dd component_table

component_count:        dd 0
component_temp_id:      dd 0
component_temp_version: dd 0
component_temp_flags:   dd 0
component_temp_slot:    dd 0
align 4
component_table:
    times COMPONENT_CAPACITY * COMPONENT_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Virtueller Bootstrap-Speichermanager (ADR-2002)
; ---------------------------------------------------------------------------

PAGING_PAGE_PRESENT   equ 0x001
PAGING_PAGE_WRITE     equ 0x002
PAGING_PAGE_USER      equ 0x004
PAGING_API_SIZE       equ 32
PAGING_API_ABI_MAJOR  equ 1
PAGING_API_ABI_MINOR  equ 0
PAGING_CAP_4K_PAGES   equ 0x00000001
PAGING_CAP_IDENTITY   equ 0x00000002
PAGING_LOW_LIMIT      equ 0x00800000

paging_initialize:
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [paging_directory], eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd

    ; Früher Kernel, Stack, BIB, Heap und Seitentabellen bleiben identisch
    ; abgebildet. Der Bootstrap-PMM liefert ausschließlich Seiten in diesem
    ; Fenster.
    xor eax, eax
.map_low:
    mov edx, eax
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE
    call paging_map_page
    jc .invalid
    add eax, PMM_PAGE_SIZE
    cmp eax, PAGING_LOW_LIMIT
    jb .map_low

    ; APIC-MMIO für den UEFI/Q35-Interruptpfad identisch abbilden.
    mov eax, 0xFEC00000
    mov edx, eax
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE
    call paging_map_page
    jc .invalid
    mov eax, 0xFEE00000
    mov edx, eax
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE
    call paging_map_page
    jc .invalid

    ; Linearen VBE-Framebuffer in seiner bestehenden Adresse abbilden.
    mov eax, [kernel_context + CONTEXT_FRAMEBUFFER]
    test eax, eax
    jz .enable
    and eax, 0xFFFFF000
    mov esi, eax
    mov ecx, [kernel_context + CONTEXT_PITCH]
    imul ecx, [kernel_context + CONTEXT_HEIGHT]
    add ecx, [kernel_context + CONTEXT_FRAMEBUFFER]
    jc .invalid
    add ecx, PMM_PAGE_SIZE - 1
    jc .invalid
    and ecx, 0xFFFFF000
    mov edi, ecx
.map_framebuffer:
    cmp esi, edi
    jae .enable
    mov eax, esi
    mov edx, esi
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE
    call paging_map_page
    jc .invalid
    add esi, PMM_PAGE_SIZE
    jc .invalid
    jmp .map_framebuffer

.enable:
    mov eax, [paging_directory]
    mov cr3, eax
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    jmp short .paging_active
.paging_active:
    mov dword [paging_enabled], 1
    clc
    ret
.invalid:
    stc
    ret

; EAX=virtuelle Seite, EDX=physische Seite, EBX=Flags.
paging_map_page:
    push eax
    push esi
    push edi
    push ebp
    mov esi, eax
    mov edi, edx
    mov ebp, ebx

    mov ecx, esi
    shr ecx, 22
    mov edx, [paging_directory]
    lea edx, [edx + ecx * 4]
    mov eax, [edx]
    test eax, PAGING_PAGE_PRESENT
    jnz .have_table

    push edx
    call pmm_alloc_page
    pop edx
    test eax, eax
    jz .invalid
    push edi
    push eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    pop eax
    pop edi
    or eax, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE
    mov [edx], eax
.have_table:
    ; Ein User-PTE benötigt das User-Bit auf beiden Paging-Ebenen. Bereits
    ; vorhandene Supervisor-PTEs bleiben dadurch weiterhin geschützt.
    test ebp, PAGING_PAGE_USER
    jz .directory_ready
    or dword [edx], PAGING_PAGE_USER
.directory_ready:
    and eax, 0xFFFFF000
    mov ecx, esi
    shr ecx, 12
    and ecx, 0x3FF
    lea eax, [eax + ecx * 4]
    and edi, 0xFFFFF000
    or edi, ebp
    mov [eax], edi
    pop ebp
    pop edi
    pop esi
    pop eax
    clc
    ret
.invalid:
    pop ebp
    pop edi
    pop esi
    pop eax
    stc
    ret

paging_self_test:
    cmp dword [paging_enabled], 1
    jne .invalid
    mov eax, cr0
    test eax, 0x80000000
    jz .invalid
    mov ecx, 32
    call heap_allocate
    jc .invalid
    mov dword [eax], 0x50414745
    cmp dword [eax], 0x50414745
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
paging_api:
    dd PAGING_API_SIZE
    dw PAGING_API_ABI_MAJOR, PAGING_API_ABI_MINOR
    dd PMM_PAGE_SIZE
    dd PAGING_CAP_4K_PAGES | PAGING_CAP_IDENTITY
    dd paging_map_page
    dd 0                             ; Unmap folgt mit dem vollständigen VMM
    dd paging_directory
    dd paging_enabled

paging_directory: dd 0
paging_enabled:   dd 0

; ---------------------------------------------------------------------------
; Interrupt- und Timer-Architektur (ADR-2006 / ADR-2007)
; ---------------------------------------------------------------------------

IDT_ENTRY_COUNT equ 256
IDT_GATE_FLAGS  equ 0x8E
PIC1_COMMAND    equ 0x20
PIC1_DATA       equ 0x21
PIC2_COMMAND    equ 0xA0
PIC2_DATA       equ 0xA1
PIC_EOI         equ 0x20
PIT_COMMAND     equ 0x43
PIT_CHANNEL0    equ 0x40
PIT_DIVISOR     equ 11932           ; ungefähr 100 Hz
INTERRUPT_API_SIZE equ 32
INTERRUPT_CAPABILITIES equ 0x00000007

interrupt_initialize:
    cli
    xor ebx, ebx
.default_gate:
    mov eax, isr_unexpected
    call idt_set_gate
    inc ebx
    cmp ebx, IDT_ENTRY_COUNT
    jb .default_gate

    xor ebx, ebx
.exception_gate:
    mov eax, [exception_stub_table + ebx * 4]
    call idt_set_gate
    inc ebx
    cmp ebx, 32
    jb .exception_gate

    mov ebx, 32
    mov eax, irq0_stub
    call idt_set_gate
    mov ebx, 33
    mov eax, irq1_stub
    call idt_set_gate
    mov ebx, 0x80
    mov eax, syscall_stub
    call idt_set_gate
    mov byte [idt_table + 0x80 * 8 + 5], 0xEE ; present, Ring-3, Interrupt-Gate
    lidt [idt_descriptor]

    cmp dword [kernel_context + CONTEXT_PLATFORM], 2
    je apic_initialize

    ; UEFI/OVMF kann externe Interrupts über den APIC-Pfad hinterlassen.
    ; Der aktuelle Bootstrap-Interruptmanager arbeitet noch mit dem 8259 PIC,
    ; daher werden Local APIC und IMCR vor dessen Programmierung deterministisch
    ; in den Legacy-INTR-Zustand gebracht. Die spätere APIC-Phase übernimmt die
    ; erneute Aktivierung kontrolliert.
    mov ecx, 0x1B                   ; IA32_APIC_BASE
    rdmsr
    and eax, 0xFFFFF7FF             ; globales APIC-Enable (Bit 11) löschen
    wrmsr
    mov al, 0x70
    out 0x22, al
    in al, 0x23
    and al, 0xFE
    out 0x23, al

    ; 8259 PIC: IRQs auf Vektoren 32..47 verschieben.
    mov al, 0x11
    out PIC1_COMMAND, al
    call io_wait
    out PIC2_COMMAND, al
    call io_wait
    mov al, 0x20
    out PIC1_DATA, al
    call io_wait
    mov al, 0x28
    out PIC2_DATA, al
    call io_wait
    mov al, 0x04
    out PIC1_DATA, al
    call io_wait
    mov al, 0x02
    out PIC2_DATA, al
    call io_wait
    mov al, 0x01
    out PIC1_DATA, al
    call io_wait
    out PIC2_DATA, al
    call io_wait
    mov al, 0xFC                    ; nur Timer und Tastatur freigeben
    out PIC1_DATA, al
    mov al, 0xFF
    out PIC2_DATA, al
    clc
    ret

apic_initialize:
    ; Local APIC aktivieren und Spurious-Interrupt-Vektor freischalten.
    mov ecx, 0x1B
    rdmsr
    or eax, 0x00000800
    wrmsr
    mov eax, [0xFEE000F0]
    or eax, 0x00000100
    and eax, 0xFFFFFF00
    or eax, 0xFF
    mov [0xFEE000F0], eax

    ; OVMF darf beim Ownership-Transfer noch einen angenommenen Interrupt im
    ; ISR-Stack hinterlassen. Mehrfache EOIs sind für leere Ebenen harmlos.
    mov ecx, 8
.drain_isr:
    mov dword [0xFEE000B0], 0
    loop .drain_isr

    ; Q35: PIT IRQ0 ist per ACPI-Override auf GSI 2, Tastatur bleibt GSI 1.
    mov eax, 0x15                   ; GSI 2 destination high
    xor edx, edx
    call ioapic_write
    mov eax, 0x14                   ; GSI 2 -> Vektor 32
    mov edx, 32
    call ioapic_write
    mov eax, 0x13                   ; GSI 1 destination high
    xor edx, edx
    call ioapic_write
    mov eax, 0x12                   ; GSI 1 -> Vektor 33
    mov edx, 33
    call ioapic_write
    clc
    ret

ioapic_write:
    mov [0xFEC00000], eax
    mov [0xFEC00010], edx
    ret

; EBX=Vektor, EAX=Handleradresse.
idt_set_gate:
    push edi
    mov edi, idt_table
    lea edi, [edi + ebx * 8]
    mov [edi + 0], ax
    mov word [edi + 2], CODE_SEGMENT
    mov byte [edi + 4], 0
    mov byte [edi + 5], IDT_GATE_FLAGS
    shr eax, 16
    mov [edi + 6], ax
    pop edi
    ret

timer_initialize:
    mov al, 0x36
    out PIT_COMMAND, al
    mov ax, PIT_DIVISOR
    out PIT_CHANNEL0, al
    mov al, ah
    out PIT_CHANNEL0, al
    mov dword [timer_ticks], 0
    clc
    ret

interrupt_enable:
    sti
    ret

interrupt_disable:
    cli
    ret

timer_get_ticks:
    mov eax, [timer_ticks]
    ret

; Mindestens zwei echte IRQ0-Ereignisse müssen eintreffen.
timer_self_test:
    mov ecx, 50000000
.wait:
    cmp dword [timer_ticks], 2
    jae .success
    pause
    dec ecx
    jnz .wait
.invalid:
    stc
    ret
.success:
    clc
    ret

io_wait:
    mov al, 0
    out 0x80, al
    ret

interrupt_dispatch:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]
    mov edx, [ebp + 12]
    mov [interrupt_return_frame], edx
    cmp eax, 32
    je .timer
    cmp eax, 33
    je .keyboard
    cmp eax, 0x80
    je .syscall
    cmp eax, 32
    jb .exception
    cmp eax, 48
    jb .slave_irq
    jmp .done
.syscall:
    call syscall_dispatch
    jmp .done
.timer:
    inc dword [timer_ticks]
    cmp dword [kernel_context + CONTEXT_PLATFORM], 2
    jne .timer_input
    mov dword [0xFEE000B0], 0       ; edge-triggered: früh quittieren
.timer_input:
    in al, 0x64                     ; Fallback, falls QEMU IRQ1 nicht zustellt
    test al, 0x01
    jz .timer_schedule
    in al, 0x60
    cmp al, 0x58
    je .debug_panic
    cmp al, 0x07
    je .debug_panic
    cmp al, 0x01
    je kernel_shutdown
    cmp al, 0x76
    je kernel_shutdown
.timer_schedule:
    mov eax, [interrupt_return_frame]
    push eax
    call scheduler_on_tick
    add esp, 4
    mov [interrupt_return_frame], eax
    push eax
    mov al, PIC_EOI
    out PIC1_COMMAND, al
    pop eax
    jmp .done
.keyboard:
    cmp dword [kernel_context + CONTEXT_PLATFORM], 2
    jne .keyboard_read
    mov dword [0xFEE000B0], 0
.keyboard_read:
    in al, 0x60                     ; Controllerdaten quittieren
    cmp al, 0x58                    ; F12 Make-Code in Scan-Code-Set 1
    je .debug_panic
    cmp al, 0x07                    ; F12 Make-Code in Scan-Code-Set 2
    je .debug_panic
    cmp al, 0x01                    ; Escape: geordneter Shutdown
    je kernel_shutdown
    cmp al, 0x76
    je kernel_shutdown
    jmp .keyboard_ack
.debug_panic:
    mov eax, 0xDEB60001
    mov edx, 0x0000DEB6
    mov esi, message_debug_panic
    call kernel_panic
.keyboard_ack:
    mov al, PIC_EOI
    out PIC1_COMMAND, al
    jmp .done
.slave_irq:
    mov al, PIC_EOI
    out PIC2_COMMAND, al
    out PIC1_COMMAND, al
    jmp .done
.exception:
    cli
    mov [last_exception_vector], eax
    cmp eax, 14
    jne .log_exception
    mov eax, cr2
    mov [last_fault_address], eax
.log_exception:
    mov esi, message_exception
    call serial_write_string
    mov eax, [last_exception_vector]
    call serial_write_hex32
    cmp dword [last_exception_vector], 14
    jne .exception_newline
    mov esi, message_fault_address
    call serial_write_string
    mov eax, [last_fault_address]
    call serial_write_hex32
.exception_newline:
    mov esi, message_newline
    call serial_write_string
    jmp kernel_halt
.done:
    cmp dword [kernel_context + CONTEXT_PLATFORM], 2
    jne .return_frame
    mov dword [0xFEE000B0], 0       ; Local-APIC EOI
.return_frame:
    mov eax, [interrupt_return_frame]
    pop ebp
    ret

isr_common:
    pushad
    push ds
    push es
    push fs
    push gs
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, [esp + 48]             ; normalisierter Vektor
    mov edx, esp
    push edx
    push eax
    call interrupt_dispatch
    add esp, 8
    mov esp, eax
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 8                      ; Vektor + Fehlercode
    iretd

%macro ISR_NO_ERROR 1
isr_%1:
    push dword 0
    push dword %1
    jmp isr_common
%endmacro

%macro ISR_WITH_ERROR 1
isr_%1:
    push dword %1
    jmp isr_common
%endmacro

ISR_NO_ERROR 0
ISR_NO_ERROR 1
ISR_NO_ERROR 2
ISR_NO_ERROR 3
ISR_NO_ERROR 4
ISR_NO_ERROR 5
ISR_NO_ERROR 6
ISR_NO_ERROR 7
ISR_WITH_ERROR 8
ISR_NO_ERROR 9
ISR_WITH_ERROR 10
ISR_WITH_ERROR 11
ISR_WITH_ERROR 12
ISR_WITH_ERROR 13
ISR_WITH_ERROR 14
ISR_NO_ERROR 15
ISR_NO_ERROR 16
ISR_WITH_ERROR 17
ISR_NO_ERROR 18
ISR_NO_ERROR 19
ISR_NO_ERROR 20
ISR_WITH_ERROR 21
ISR_NO_ERROR 22
ISR_NO_ERROR 23
ISR_NO_ERROR 24
ISR_NO_ERROR 25
ISR_NO_ERROR 26
ISR_NO_ERROR 27
ISR_NO_ERROR 28
ISR_WITH_ERROR 29
ISR_WITH_ERROR 30
ISR_NO_ERROR 31

irq0_stub:
    push dword 0
    push dword 32
    jmp isr_common
irq1_stub:
    push dword 0
    push dword 33
    jmp isr_common
syscall_stub:
    push dword 0
    push dword 0x80
    jmp isr_common
isr_unexpected:
    push dword 0
    push dword 255
    jmp isr_common

align 4
exception_stub_table:
%assign __vector 0
%rep 32
    dd isr_%+__vector
%assign __vector __vector + 1
%endrep

align 8
kernel_gdt:
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
    dq 0x00CFFA000000FFFF          ; Ring-3 Code, Selector 0x1B
    dq 0x00CFF2000000FFFF          ; Ring-3 Data, Selector 0x23
kernel_tss_descriptor:
    dq 0                            ; 32-Bit Available TSS, Selector 0x28
kernel_gdt_descriptor:
    dw kernel_gdt_descriptor - kernel_gdt - 1
    dd kernel_gdt

align 16
idt_table:
    times IDT_ENTRY_COUNT dq 0
idt_descriptor:
    dw (IDT_ENTRY_COUNT * 8) - 1
    dd idt_table

timer_ticks:           dd 0
last_exception_vector: dd 0
last_fault_address:    dd 0
interrupt_return_frame: dd 0

align 16
kernel_tss:
    times 104 db 0

; Versionierte öffentliche Modulgrenze gemäß ADR-2006/2007.
align 4
interrupt_api:
    dd INTERRUPT_API_SIZE
    dw 1, 0
    dd IDT_ENTRY_COUNT
    dd INTERRUPT_CAPABILITIES
    dd interrupt_enable
    dd interrupt_disable
    dd timer_get_ticks
    dd 100

; ---------------------------------------------------------------------------
; Kernel-Nachrichtenwarteschlange (ADR-2005)
; ---------------------------------------------------------------------------

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

; Bootkritischer Device Manager (NPSPEC-KERNEL-0002, Phase 8)
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

service_manager_self_test:
    mov eax, 0x4B45524E
    mov edx, 0x00010000
    mov ebx, 0x434F5245
    call service_register
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
align 4
service_table:
    times SERVICE_CAPACITY * SERVICE_RECORD_SIZE db 0

; Kernel Process Manager (ADR-2011)
PROCESS_API_SIZE      equ 32
PROCESS_CAPACITY      equ 4
PROCESS_RECORD_SIZE   equ 32
PROCESS_STATE_EMPTY   equ 0
PROCESS_STATE_RUNNING equ 1
OBJECT_TYPE_PROCESS   equ 4
PROCESS_PID       equ 0
PROCESS_STATE     equ 4
PROCESS_CR3       equ 8
PROCESS_HANDLE    equ 12
PROCESS_PARENT    equ 16
PROCESS_FLAGS     equ 20

process_manager_initialize:
    mov edi, process_table
    xor eax, eax
    mov ecx, (PROCESS_CAPACITY * PROCESS_RECORD_SIZE) / 4
    rep stosd
    mov dword [process_count], 0
    mov dword [process_next_pid], 1
    clc
    ret

; EAX=Flags, EDX=Page-Directory. EAX=PID.
process_create:
    pushfd
    cli
    mov [process_temp_flags], eax
    mov [process_temp_cr3], edx
    xor ecx, ecx
.scan:
    cmp ecx, PROCESS_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 5
    add edi, process_table
    cmp dword [edi + PROCESS_STATE], PROCESS_STATE_EMPTY
    je .slot
    inc ecx
    jmp .scan
.slot:
    mov [process_temp_slot], edi
    mov edx, [process_next_pid]
    mov [process_temp_pid], edx
    mov eax, OBJECT_TYPE_PROCESS
    xor ebx, ebx
    call object_create
    jc .invalid
    mov edi, [process_temp_slot]
    mov edx, [process_temp_pid]
    mov [edi + PROCESS_PID], edx
    mov dword [edi + PROCESS_STATE], PROCESS_STATE_RUNNING
    mov edx, [process_temp_cr3]
    mov [edi + PROCESS_CR3], edx
    mov [edi + PROCESS_HANDLE], eax
    mov dword [edi + PROCESS_PARENT], 0
    mov edx, [process_temp_flags]
    mov [edi + PROCESS_FLAGS], edx
    inc dword [process_count]
    inc dword [process_next_pid]
    mov eax, [process_temp_pid]
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=PID. EAX=Prozessdatensatz oder 0.
process_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, PROCESS_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 5
    add edx, process_table
    cmp dword [edx + PROCESS_STATE], PROCESS_STATE_EMPTY
    je .next
    cmp [edx + PROCESS_PID], eax
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

process_manager_self_test:
    mov eax, 1                      ; Kernel/System-Prozess
    mov edx, cr3
    call process_create
    jc .invalid
    cmp eax, 1
    jne .invalid
    call process_lookup
    jc .invalid
    cmp dword [eax + PROCESS_STATE], PROCESS_STATE_RUNNING
    jne .invalid
    mov edx, cr3
    cmp [eax + PROCESS_CR3], edx
    jne .invalid
    cmp dword [process_count], 1
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
process_manager_api:
    dd PROCESS_API_SIZE
    dw 1, 0
    dd PROCESS_CAPACITY
    dd process_create
    dd process_lookup
    dd process_count
    dd process_table
    dd process_next_pid

process_count:      dd 0
process_next_pid:   dd 0
process_temp_pid:   dd 0
process_temp_cr3:   dd 0
process_temp_flags: dd 0
process_temp_slot:  dd 0
align 4
process_table:
    times PROCESS_CAPACITY * PROCESS_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Initialer x86-32 Userspace-Prozess (Bootphase 10)
; ---------------------------------------------------------------------------
USER_CODE_SELECTOR equ 0x1B
USER_DATA_SELECTOR equ 0x23
TSS_SELECTOR       equ 0x28
USER_CODE_ADDRESS  equ 0x00400000
USER_STACK_ADDRESS equ 0x00402000
PROCESS_FLAG_SYSTEM_SERVICE equ 0x00000002
PROCESS_FLAG_USERSPACE      equ 0x00000004
SYSCALL_ABI_VERSION         equ 0x00000001
SYSCALL_SERVICE_CORE        equ 1
SYSCALL_SERVICE_PROCESS     equ 2
SYSCALL_SERVICE_THREAD      equ 3
SYSCALL_SERVICE_IPC         equ 5
SYSCALL_SERVICE_VFS         equ 6
SYSCALL_SERVICE_POWER       equ 7
SYSCALL_SERVICE_NETWORK     equ 8
SYSCALL_SERVICE_LOGGING     equ 9
SYSCALL_CORE_EXIT           equ 1
SYSCALL_CORE_READY          equ 2
SYSCALL_CORE_CLOSE_HANDLE   equ 3
SYSCALL_QUERY_SELF          equ 1
SYSCALL_IPC_SEND            equ 1
SYSCALL_IPC_RECEIVE         equ 2
SYSCALL_VFS_OPEN_ROOT       equ 1
SYSCALL_VFS_LOOKUP          equ 2
SYSCALL_POWER_QUERY_SYSTEM  equ 1
SYSCALL_POWER_WAKE_ACQUIRE  equ 2
SYSCALL_POWER_WAKE_RELEASE  equ 3
SYSCALL_POWER_SET_PROFILE   equ 4
SYSCALL_POWER_REQUEST_STATE equ 5
SYSCALL_NETWORK_QUERY       equ 1
SYSCALL_NETWORK_RAW_OPEN    equ 2
SYSCALL_NETWORK_SOCKET_CREATE equ 3
SYSCALL_NETWORK_SEND        equ 4
SYSCALL_NETWORK_RECEIVE     equ 5
SYSCALL_NETWORK_BIND        equ 6
SYSCALL_NETWORK_STREAM_CREATE equ 7
SYSCALL_NETWORK_CONNECT     equ 8
SYSCALL_NETWORK_STREAM_SEND equ 9
SYSCALL_NETWORK_STREAM_RECEIVE equ 10
SYSCALL_NETWORK_STREAM_BIND equ 11
SYSCALL_NETWORK_LISTEN      equ 12
SYSCALL_NETWORK_ACCEPT      equ 13
SYSCALL_LOG_QUERY           equ 1
SYSCALL_LOG_READ_LATEST     equ 2
SYSCALL_STATUS_OK           equ 0
SYSCALL_STATUS_ABI          equ -1
SYSCALL_STATUS_SIZE         equ -4
SYSCALL_STATUS_RESERVED     equ -5
SYSCALL_STATUS_SERVICE      equ -8
SYSCALL_STATUS_OPERATION    equ -9
SYSCALL_STATUS_ACCESS       equ -13
SYSCALL_STATUS_POINTER      equ -15
SYSCALL_STATUS_WOULD_BLOCK  equ -19
USER_ADDRESS_MIN            equ USER_CODE_ADDRESS
USER_ADDRESS_MAX            equ USER_STACK_ADDRESS
SHARED_SERVICE_ADDRESS      equ 0x00403000
SHARED_SERVICE_SIGNATURE    equ 0x5353564E ; "NVSS"
SHARED_SERVICE_SIZE         equ 64
SHARED_FEATURE_INT80        equ 0x00000001
SHARED_FEATURE_COPY_IO      equ 0x00000002
SHARED_FEATURE_PREEMPT      equ 0x00000004
SHARED_SERVICE_BITMAP       equ 0x000000F7 ; Core, Process, Thread, IPC, VFS, Power, Network

userspace_initialize:
    ; TSS stellt für Ring-3-Interrupts einen kontrollierten Kernelstack bereit.
    mov edi, kernel_tss
    xor eax, eax
    mov ecx, 104 / 4
    rep stosd
    mov dword [kernel_tss + 4], KERNEL_STACK_TOP
    mov word [kernel_tss + 8], DATA_SEGMENT
    mov word [kernel_tss + 102], 104
    mov eax, kernel_tss
    mov word [kernel_tss_descriptor + 0], 103
    mov word [kernel_tss_descriptor + 2], ax
    shr eax, 16
    mov byte [kernel_tss_descriptor + 4], al
    mov byte [kernel_tss_descriptor + 5], 0x89
    mov byte [kernel_tss_descriptor + 6], 0
    mov byte [kernel_tss_descriptor + 7], ah
    lgdt [kernel_gdt_descriptor]
    mov ax, TSS_SELECTOR
    ltr ax

    ; Eigene physische Seiten für Code und Stack; nur diese PTEs tragen U/S.
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [userspace_code_page], eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    mov esi, userspace_program_start
    mov edi, [userspace_code_page]
    mov ecx, userspace_program_end - userspace_program_start
    rep movsb
    mov eax, USER_CODE_ADDRESS
    mov edx, [userspace_code_page]
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_USER
    call paging_map_page
    jc .invalid

    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [userspace_stack_page], eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    mov eax, USER_STACK_ADDRESS - PMM_PAGE_SIZE
    mov edx, [userspace_stack_page]
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE | PAGING_PAGE_USER
    call paging_map_page
    jc .invalid
    mov eax, cr3
    mov cr3, eax                    ; TLB nach den neuen PTEs invalidieren

    call shared_service_page_initialize
    jc .invalid

    mov eax, PROCESS_FLAG_SYSTEM_SERVICE | PROCESS_FLAG_USERSPACE
    mov edx, cr3
    call process_create
    jc .invalid
    mov [userspace_pid], eax
    cmp eax, 2
    jne .invalid
    mov edx, SECURITY_CAP_SERVICE
    call security_grant
    jc .invalid
    mov eax, 2
    mov edx, SECURITY_CAP_IPC
    call security_grant
    jc .invalid
    mov eax, 2
    mov edx, SECURITY_CAP_POWER_QUERY | SECURITY_CAP_POWER_WAKE | SECURITY_CAP_POWER_PROFILE
    call security_grant
    jc .invalid
    mov eax, 2
    mov edx, SECURITY_CAP_NET_QUERY | SECURITY_CAP_NET_CONNECT | SECURITY_CAP_NET_LISTEN | SECURITY_CAP_LOG_READ
    call security_grant
    jc .invalid

    ; Der Bootstrap-Kernelthread (TID 1 / Scheduler-Slot 0) wird atomar zum
    ; ersten Userspace-Thread des neuen Prozesses weitergeführt.
    mov eax, 1
    call thread_lookup
    jc .invalid
    mov dword [eax + THREAD_PID], 2
    mov dword [eax + THREAD_ENTRY], USER_CODE_ADDRESS
    mov dword [eax + THREAD_CONTEXT], 0
    mov dword [userspace_tid], 1

    mov dword [handle_owner_pid], 2
    mov eax, 2
    call process_lookup
    jc .invalid
    mov edx, [eax + PROCESS_HANDLE]
    mov eax, 2
    mov ebx, OBJECT_TYPE_PROCESS
    mov ecx, HANDLE_RIGHT_QUERY | HANDLE_RIGHT_WAIT
    mov esi, HANDLE_FLAG_PROTECTED
    call handle_create
    jc .invalid
    mov [userspace_process_handle], eax
    mov eax, 1
    call thread_lookup
    jc .invalid
    mov edx, [eax + THREAD_HANDLE]
    mov eax, 2
    mov ebx, OBJECT_TYPE_THREAD
    mov ecx, HANDLE_RIGHT_QUERY | HANDLE_RIGHT_WAIT
    mov esi, HANDLE_FLAG_PROTECTED
    call handle_create
    jc .invalid
    mov [userspace_thread_handle], eax
    cmp dword [handle_active_count], 2
    jne .invalid
    mov edx, [userspace_process_handle]
    mov eax, 2
    mov ebx, OBJECT_TYPE_PROCESS
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jc .invalid
    mov edx, [userspace_process_handle]
    mov eax, 2
    mov ebx, OBJECT_TYPE_THREAD       ; Typverwechslung muss scheitern
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jnc .invalid
    call userspace_ipc_initialize
    jc .invalid
    call userspace_vfs_initialize
    jc .invalid

    ; Grenztests der öffentlichen Copy-in-Prüfung.
    mov esi, USER_ADDRESS_MIN
    mov ecx, 16
    call syscall_validate_user_range
    jc .invalid
    mov esi, USER_ADDRESS_MIN - 1
    mov ecx, 1
    call syscall_validate_user_range
    jnc .invalid
    mov esi, 0xFFFFFFF8
    mov ecx, 16
    call syscall_validate_user_range
    jnc .invalid
    mov dword [userspace_exit_seen], 0
    mov dword [userspace_ready_seen], 0
    clc
    ret
.invalid:
    stc
    ret

userspace_enter:
    cli
    mov ax, USER_DATA_SELECTOR
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push dword USER_DATA_SELECTOR
    push dword USER_STACK_ADDRESS
    push dword 0x00000202
    push dword USER_CODE_SELECTOR
    push dword USER_CODE_ADDRESS
    iretd

userspace_program_start:
    ; Process.QuerySelf -> Ergebnis auf dem beschreibbaren Userstack.
    mov eax, SYSCALL_SERVICE_PROCESS
    mov ebx, SYSCALL_QUERY_SELF
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 64
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 56], 2
    jne .failed
    mov eax, SYSCALL_SERVICE_PROCESS
    mov ebx, 2                       ; Process.OpenSelf
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 64
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 56], 0
    je .failed

    ; Thread.QuerySelf -> TID 1 des weitergeführten Bootstrap-Threads.
    mov eax, SYSCALL_SERVICE_THREAD
    mov ebx, SYSCALL_QUERY_SELF
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 32
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 24], 1
    jne .failed
    mov eax, SYSCALL_SERVICE_THREAD
    mov ebx, 2                       ; Thread.OpenSelf
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 32
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 24], 0
    je .failed

    mov eax, SYSCALL_SERVICE_IPC
    mov ebx, SYSCALL_IPC_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_CODE_ADDRESS + userspace_ipc_packet - userspace_program_start
    mov esi, 48
    int 0x80
    test eax, eax
    jnz .failed
    mov eax, SYSCALL_SERVICE_IPC
    mov ebx, SYSCALL_IPC_RECEIVE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 128
    mov esi, 48
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 112], 0x11223344
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 96], 4
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 88], 0x41564F4E
    jne .failed

    mov eax, SYSCALL_SERVICE_VFS
    mov ebx, SYSCALL_VFS_OPEN_ROOT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 160
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 152], 0
    je .failed

    ; VFS.Lookup mit explizitem Pointer und Pfadlänge für "/".
    mov dword [USER_STACK_ADDRESS - 224], 32
    mov dword [USER_STACK_ADDRESS - 220], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 152]
    mov [USER_STACK_ADDRESS - 216], eax
    mov dword [USER_STACK_ADDRESS - 212], USER_CODE_ADDRESS + userspace_root_path - userspace_program_start
    mov dword [USER_STACK_ADDRESS - 208], 1
    mov dword [USER_STACK_ADDRESS - 204], 0
    mov dword [USER_STACK_ADDRESS - 200], 0
    mov dword [USER_STACK_ADDRESS - 196], 0
    mov eax, SYSCALL_SERVICE_VFS
    mov ebx, SYSCALL_VFS_LOOKUP
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 224
    mov esi, 32
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 200], 0
    je .failed
    mov dword [USER_STACK_ADDRESS - 256], 16
    mov dword [USER_STACK_ADDRESS - 252], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 200]
    mov [USER_STACK_ADDRESS - 248], eax
    mov dword [USER_STACK_ADDRESS - 244], 0
    mov eax, SYSCALL_SERVICE_CORE
    mov ebx, SYSCALL_CORE_CLOSE_HANDLE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 256
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed

    ; Power.QuerySystem liefert ausschließlich capability-geschützte,
    ; aggregierte Kernelzustände an den Bootstrap-Prozess.
    mov eax, SYSCALL_SERVICE_POWER
    mov ebx, SYSCALL_POWER_QUERY_SYSTEM
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 320
    mov esi, 32
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 320], 32
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 316], SYSCALL_ABI_VERSION
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 312], POWER_STATE_RUNNING
    jne .failed
    test dword [USER_STACK_ADDRESS - 308], (1 << POWER_STATE_SHUTDOWN)
    jz .failed

    ; Zeitlich begrenzten Wake Lock anfordern und mit dem ausgegebenen Token
    ; wieder freigeben. Eine zweite Freigabe muss der Kernel ablehnen.
    mov dword [USER_STACK_ADDRESS - 352], 16
    mov dword [USER_STACK_ADDRESS - 348], SYSCALL_ABI_VERSION
    mov dword [USER_STACK_ADDRESS - 344], 100 ; maximal eine Sekunde
    mov dword [USER_STACK_ADDRESS - 340], 0
    mov eax, SYSCALL_SERVICE_POWER
    mov ebx, SYSCALL_POWER_WAKE_ACQUIRE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 352
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    mov eax, [USER_STACK_ADDRESS - 344]
    mov dword [USER_STACK_ADDRESS - 368], 16
    mov dword [USER_STACK_ADDRESS - 364], SYSCALL_ABI_VERSION
    mov [USER_STACK_ADDRESS - 360], eax
    mov dword [USER_STACK_ADDRESS - 356], 0
    mov eax, SYSCALL_SERVICE_POWER
    mov ebx, SYSCALL_POWER_WAKE_RELEASE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 368
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    mov eax, SYSCALL_SERVICE_POWER
    mov ebx, SYSCALL_POWER_WAKE_RELEASE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 368
    mov esi, 16
    int 0x80
    test eax, eax
    jz .failed

    mov dword [USER_STACK_ADDRESS - 384], 16
    mov dword [USER_STACK_ADDRESS - 380], SYSCALL_ABI_VERSION
    mov dword [USER_STACK_ADDRESS - 376], POWER_PROFILE_EFFICIENCY
    mov dword [USER_STACK_ADDRESS - 372], 0
    mov eax, SYSCALL_SERVICE_POWER
    mov ebx, SYSCALL_POWER_SET_PROFILE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 384
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed

    ; Den globalen Shutdown ohne POWER_SHUTDOWN-Capability anfordern. Der
    ; Kernel muss fail-closed antworten und darf keinen Übergang beginnen.
    mov dword [USER_STACK_ADDRESS - 416], 24
    mov dword [USER_STACK_ADDRESS - 412], SYSCALL_ABI_VERSION
    mov dword [USER_STACK_ADDRESS - 408], POWER_STATE_SHUTDOWN
    mov dword [USER_STACK_ADDRESS - 404], 1 ; Benutzeranforderung
    mov dword [USER_STACK_ADDRESS - 400], 0
    mov dword [USER_STACK_ADDRESS - 396], 0
    mov eax, SYSCALL_SERVICE_POWER
    mov ebx, SYSCALL_POWER_REQUEST_STATE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 416
    mov esi, 24
    int 0x80
    cmp eax, SYSCALL_STATUS_ACCESS
    jne .failed

    ; Capability-geschütztes Datagramm-Socket erstellen und ein Paket ohne
    ; ungeprüften Userspace-Pointer über lo0 senden und wieder empfangen.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_SOCKET_CREATE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 464
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    mov eax, [USER_STACK_ADDRESS - 456]
    test eax, eax
    jz .failed
    mov dword [USER_STACK_ADDRESS - 592], 28
    mov dword [USER_STACK_ADDRESS - 588], SYSCALL_ABI_VERSION
    mov [USER_STACK_ADDRESS - 584], eax
    mov dword [USER_STACK_ADDRESS - 580], NETWORK_AF_IPV4
    mov dword [USER_STACK_ADDRESS - 576], NETWORK_IPV4_LOOPBACK
    mov dword [USER_STACK_ADDRESS - 572], 9000
    mov dword [USER_STACK_ADDRESS - 568], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_BIND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 592
    mov esi, 28
    int 0x80
    test eax, eax
    jnz .failed
    mov dword [USER_STACK_ADDRESS - 512], 40
    mov dword [USER_STACK_ADDRESS - 508], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 456]
    mov [USER_STACK_ADDRESS - 504], eax
    mov dword [USER_STACK_ADDRESS - 500], 4
    mov dword [USER_STACK_ADDRESS - 496], 0
    mov dword [USER_STACK_ADDRESS - 492], 0x41564F4E ; "NOVA"
    mov dword [USER_STACK_ADDRESS - 488], 0
    mov dword [USER_STACK_ADDRESS - 484], 0
    mov dword [USER_STACK_ADDRESS - 480], 0
    mov dword [USER_STACK_ADDRESS - 476], 0
    ; Überlange Datagramme müssen vor jeder Queue-Änderung scheitern.
    mov dword [USER_STACK_ADDRESS - 500], 17
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 512
    mov esi, 40
    int 0x80
    cmp eax, SYSCALL_STATUS_SIZE
    jne .failed
    mov dword [USER_STACK_ADDRESS - 500], 4
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 512
    mov esi, 40
    int 0x80
    test eax, eax
    jnz .failed
    ; Die Queue ist auf ein Datagramm begrenzt. Ein zweites, nichtblockierendes
    ; Senden darf das erste Paket nicht überschreiben.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 512
    mov esi, 40
    int 0x80
    cmp eax, SYSCALL_STATUS_WOULD_BLOCK
    jne .failed
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_RECEIVE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 560
    mov esi, 40
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 548], 4
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 540], 0x41564F4E
    jne .failed
    ; Nach dem einzigen Empfang ist die Queue leer und muss WOULD_BLOCK melden.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_RECEIVE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 560
    mov esi, 40
    int 0x80
    cmp eax, SYSCALL_STATUS_WOULD_BLOCK
    jne .failed

    ; Separates TCP-Stream-Socket erzeugen und capability-geschuetzt mit dem
    ; lokalen Bootstrap-Endpunkt verbinden.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_CREATE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 640
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    mov eax, [USER_STACK_ADDRESS - 632]
    test eax, eax
    jz .failed
    mov dword [USER_STACK_ADDRESS - 672], 28
    mov dword [USER_STACK_ADDRESS - 668], SYSCALL_ABI_VERSION
    mov [USER_STACK_ADDRESS - 664], eax
    mov dword [USER_STACK_ADDRESS - 660], NETWORK_AF_IPV4
    mov dword [USER_STACK_ADDRESS - 656], NETWORK_IPV4_LOOPBACK
    mov dword [USER_STACK_ADDRESS - 652], NETWORK_BOOTSTRAP_PORT
    mov dword [USER_STACK_ADDRESS - 648], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_CONNECT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 672
    mov esi, 28
    int 0x80
    test eax, eax
    jnz .failed

    mov dword [USER_STACK_ADDRESS - 720], 40
    mov dword [USER_STACK_ADDRESS - 716], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 632]
    mov [USER_STACK_ADDRESS - 712], eax
    mov dword [USER_STACK_ADDRESS - 708], 4
    mov dword [USER_STACK_ADDRESS - 704], 0
    mov dword [USER_STACK_ADDRESS - 700], 0x41564F4E
    mov dword [USER_STACK_ADDRESS - 696], 0
    mov dword [USER_STACK_ADDRESS - 692], 0
    mov dword [USER_STACK_ADDRESS - 688], 0
    mov dword [USER_STACK_ADDRESS - 684], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 720
    mov esi, 40
    int 0x80
    test eax, eax
    jnz .failed
    ; Der einzelne Stream-Puffer ist voll und darf nicht ueberschrieben werden.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_SEND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 720
    mov esi, 40
    int 0x80
    cmp eax, SYSCALL_STATUS_WOULD_BLOCK
    jne .failed
    mov dword [USER_STACK_ADDRESS - 768], 40
    mov dword [USER_STACK_ADDRESS - 764], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 632]
    mov [USER_STACK_ADDRESS - 760], eax
    mov dword [USER_STACK_ADDRESS - 756], 16
    mov dword [USER_STACK_ADDRESS - 752], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_RECEIVE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 768
    mov esi, 40
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 756], 4
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 748], 0x41564F4E
    jne .failed
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_RECEIVE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 768
    mov esi, 40
    int 0x80
    cmp eax, SYSCALL_STATUS_WOULD_BLOCK
    jne .failed

    ; Zweites Stream-Socket als lokalen Listener konfigurieren. Der Backlog
    ; ist fest begrenzt; Accept liefert ein separates Socket-Handle.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_CREATE
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 800
    mov esi, 16
    int 0x80
    test eax, eax
    jnz .failed
    mov eax, [USER_STACK_ADDRESS - 792]
    test eax, eax
    jz .failed
    mov dword [USER_STACK_ADDRESS - 832], 28
    mov dword [USER_STACK_ADDRESS - 828], SYSCALL_ABI_VERSION
    mov [USER_STACK_ADDRESS - 824], eax
    mov dword [USER_STACK_ADDRESS - 820], NETWORK_AF_IPV4
    mov dword [USER_STACK_ADDRESS - 816], NETWORK_IPV4_LOOPBACK
    mov dword [USER_STACK_ADDRESS - 812], NETWORK_BOOTSTRAP_PORT
    mov dword [USER_STACK_ADDRESS - 808], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_STREAM_BIND
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 832
    mov esi, 28
    int 0x80
    test eax, eax
    jnz .failed
    mov dword [USER_STACK_ADDRESS - 856], 20
    mov dword [USER_STACK_ADDRESS - 852], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 792]
    mov [USER_STACK_ADDRESS - 848], eax
    mov dword [USER_STACK_ADDRESS - 844], 1
    mov dword [USER_STACK_ADDRESS - 840], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_LISTEN
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 856
    mov esi, 20
    int 0x80
    test eax, eax
    jnz .failed
    mov dword [USER_STACK_ADDRESS - 880], 20
    mov dword [USER_STACK_ADDRESS - 876], SYSCALL_ABI_VERSION
    mov eax, [USER_STACK_ADDRESS - 792]
    mov [USER_STACK_ADDRESS - 872], eax
    mov dword [USER_STACK_ADDRESS - 868], 0
    mov dword [USER_STACK_ADDRESS - 864], 0
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_ACCEPT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 880
    mov esi, 20
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 868], 0
    je .failed
    ; Die einzige ausstehende Verbindung ist verbraucht.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_ACCEPT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 880
    mov esi, 20
    int 0x80
    cmp eax, SYSCALL_STATUS_WOULD_BLOCK
    jne .failed

    ; Network.Query beschreibt nur den eigenen Standard-Namespace und das
    ; Loopback-Interface. Raw-Sockets bleiben ohne Sonderrecht geschlossen.
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_QUERY
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 448
    mov esi, 96
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 448], 96
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 444], SYSCALL_ABI_VERSION
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 436], 1 ; genau lo0
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 428], NETWORK_INTERFACE_UP
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 424], NETWORK_LOOPBACK_MTU
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 416], 1 ; ein TX-Datagramm
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 412], 1 ; ein RX-Datagramm
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 408], 0 ; keine beschädigten Pakete
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 404], 9000
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 400], 1 ; ICMPv4 Echo Request/Reply getestet
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 396], 2 ; Loopback-Netz- und Hostroute
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 392], 2 ; Treffer und Unreachable-Test
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 388], 1 ; ein Treffer
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 384], 4 ; Selbsttests plus UDP/TCP-Senden
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 380], 1 ; fail-closed Test verworfen
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 376], 1 ; IPv6/UDP ::1 validiert
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 372], 1 ; ICMPv6 Echo Request/Reply
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 368], 4 ; TCP Aufbau, Daten, Retransmit, Abbau
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 364], 4 ; vier TCP-Nutzbytes gesendet
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 360], 4 ; vier TCP-Nutzbytes empfangen
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 356], 1 ; eine Verbindung akzeptiert
    jne .failed
    mov eax, SYSCALL_SERVICE_NETWORK
    mov ebx, SYSCALL_NETWORK_RAW_OPEN
    mov ecx, SYSCALL_ABI_VERSION
    xor edx, edx
    xor esi, esi
    int 0x80
    cmp eax, SYSCALL_STATUS_ACCESS
    jne .failed

    mov eax, SYSCALL_SERVICE_LOGGING
    mov ebx, SYSCALL_LOG_QUERY
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 928
    mov esi, 32
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 928], 32
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 920], 10
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 916], 2
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 908], LOG_RING_CAPACITY
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 904], 10
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 900], 2
    jne .failed
    mov eax, SYSCALL_SERVICE_LOGGING
    mov ebx, SYSCALL_LOG_READ_LATEST
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_STACK_ADDRESS - 960
    mov esi, 32
    int 0x80
    test eax, eax
    jnz .failed
    cmp dword [USER_STACK_ADDRESS - 952], 10
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 948], 1
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 944], 1
    jne .failed
    cmp dword [USER_STACK_ADDRESS - 936], 0x109
    jne .failed

    ; Die feste Shared Service Page ist user-lesbar, aber nicht beschreibbar.
    cmp dword [SHARED_SERVICE_ADDRESS + 0], SHARED_SERVICE_SIGNATURE
    jne .failed
    cmp dword [SHARED_SERVICE_ADDRESS + 4], SHARED_SERVICE_SIZE
    jne .failed
    cmp dword [SHARED_SERVICE_ADDRESS + 8], SYSCALL_ABI_VERSION
    jne .failed
    cmp dword [SHARED_SERVICE_ADDRESS + 20], PMM_PAGE_SIZE
    jne .failed

    mov eax, SYSCALL_SERVICE_CORE
    mov ebx, SYSCALL_CORE_READY
    mov ecx, SYSCALL_ABI_VERSION
    xor edx, edx
    xor esi, esi
    int 0x80
    test eax, eax
    jnz .failed
.running:
    pause
    jmp .running
.failed:
    mov eax, SYSCALL_SERVICE_CORE
    mov ebx, SYSCALL_CORE_EXIT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_CODE_ADDRESS + userspace_exit_arguments - userspace_program_start
    mov esi, 16
    int 0x80
    ud2

userspace_exit_probe:
    mov eax, SYSCALL_SERVICE_CORE
    mov ebx, SYSCALL_CORE_EXIT
    mov ecx, SYSCALL_ABI_VERSION
    mov edx, USER_CODE_ADDRESS + userspace_exit_arguments - userspace_program_start
    mov esi, 16
    int 0x80
    ud2                             ; eine erfolgreiche Exit-Operation kehrt nie zurück
align 4
userspace_exit_arguments:
    dd 16
    dw 1, 0
    dd 0                            ; Exit-Code
    dd 0                            ; reserviert
align 8
userspace_ipc_packet:
    dd 48
    dw 1, 0
    dd 0                            ; Send-Endpoint, zur Laufzeit gesetzt
    dd 0
    dq 0x0000000011223344
    dq 0
    dd 4
    dd 0
    db "NOVA",0,0,0,0
userspace_root_path: db '/'
userspace_program_end:

; ESI=Userspace-Adresse, ECX=Länge. CF=0 nur für vollständig enthaltene
; Bereiche des aktuellen Prozesses; Überläufe werden vor dem Zugriff erkannt.
syscall_validate_user_range:
    test ecx, ecx
    jz .invalid
    cmp esi, USER_ADDRESS_MIN
    jb .invalid
    mov eax, esi
    add eax, ecx
    jc .invalid
    cmp eax, esi
    jbe .invalid
    cmp eax, USER_ADDRESS_MAX
    ja .invalid
    clc
    ret
.invalid:
    stc
    ret

; Kopiert erst nach vollständiger Bereichsprüfung in Kernel-Speicher.
; ESI=Quelle, EDI=Ziel, ECX=Länge.
syscall_copy_from_user:
    push esi
    push ecx
    call syscall_validate_user_range
    pop ecx
    pop esi
    jc .invalid
    rep movsb
    clc
    ret
.invalid:
    stc
    ret

; Für öffentliche Ergebnisstrukturen gilt dieselbe vollständige Bereichsprüfung.
syscall_copy_to_user:
    push edi
    push ecx
    mov esi, edi
    call syscall_validate_user_range
    pop ecx
    pop edi
    jc .invalid
    mov esi, syscall_result_buffer
    rep movsb
    clc
    ret
.invalid:
    stc
    ret

; ESI=Kernelquelle, EDI=Userspace-Ziel, ECX=Länge.
syscall_copy_buffer_to_user:
    push esi
    push ecx
    mov esi, edi
    call syscall_validate_user_range
    pop ecx
    pop esi
    jc .invalid
    rep movsb
    clc
    ret
.invalid:
    stc
    ret

; EDX zeigt auf den von isr_common erzeugten, bereits auf dem TSS-Kernelstack
; liegenden Registerframe.
syscall_dispatch:
    mov [syscall_frame], edx
    inc dword [syscall_total]
    cmp dword [edx + 44], SYSCALL_SERVICE_PROCESS
    je .process
    cmp dword [edx + 44], SYSCALL_SERVICE_THREAD
    je .thread
    cmp dword [edx + 44], SYSCALL_SERVICE_IPC
    je .ipc
    cmp dword [edx + 44], SYSCALL_SERVICE_VFS
    je .vfs
    cmp dword [edx + 44], SYSCALL_SERVICE_POWER
    je .power
    cmp dword [edx + 44], SYSCALL_SERVICE_NETWORK
    je .network
    cmp dword [edx + 44], SYSCALL_SERVICE_LOGGING
    je .logging
    cmp dword [edx + 44], SYSCALL_SERVICE_CORE
    jne .unknown_service
    cmp dword [edx + 32], SYSCALL_CORE_READY
    je .core_ready
    cmp dword [edx + 32], SYSCALL_CORE_CLOSE_HANDLE
    je .core_close_handle
    cmp dword [edx + 32], SYSCALL_CORE_EXIT
    jne .unknown_operation
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size

    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_SERVICE
    call security_check
    jc .access_denied

    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 16
    mov edi, syscall_argument_buffer
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_argument_buffer + 0], 16
    jb .bad_size
    movzx eax, word [syscall_argument_buffer + 4]
    cmp eax, 1
    jne .bad_abi
    cmp word [syscall_argument_buffer + 6], 0
    jb .bad_abi
    cmp dword [syscall_argument_buffer + 12], 0
    jne .bad_reserved

    mov eax, [syscall_argument_buffer + 8]
    mov [userspace_exit_code], eax
    mov dword [userspace_exit_seen], 1
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    mov dword [edx + 56], userspace_return
    mov dword [edx + 60], CODE_SEGMENT
    and dword [edx + 64], 0xFFFFCFFF
    ret
.logging:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 32
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_LOG_READ
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    cmp dword [edx + 32], SYSCALL_LOG_QUERY
    je .log_query
    cmp dword [edx + 32], SYSCALL_LOG_READ_LATEST
    je .log_read_latest
    jmp .unknown_operation
.log_query:
    mov dword [syscall_log_result + 0], 32
    mov dword [syscall_log_result + 4], SYSCALL_ABI_VERSION
    mov eax, [logging_written_records]
    mov [syscall_log_result + 8], eax
    mov eax, [logging_dropped_records]
    mov [syscall_log_result + 12], eax
    mov eax, [logging_overwritten_records]
    mov [syscall_log_result + 16], eax
    mov eax, [logging_record_count]
    mov [syscall_log_result + 20], eax
    mov eax, [logging_written_records]
    mov [syscall_log_result + 24], eax
    mov eax, [logging_critical_records]
    mov [syscall_log_result + 28], eax
    mov esi, message_logging_query_ok
    jmp .log_copy
.log_read_latest:
    cmp dword [logging_record_count], 0
    je .log_would_block
    mov eax, [logging_write_index]
    dec eax
    and eax, LOG_RING_CAPACITY - 1
    shl eax, 5
    add eax, logging_ring
    cmp dword [eax + 28], LOG_COMMIT_MARKER
    jne .unknown_operation
    mov dword [syscall_log_result + 0], 32
    mov dword [syscall_log_result + 4], SYSCALL_ABI_VERSION
    mov ecx, [eax + 0]
    mov [syscall_log_result + 8], ecx
    mov ecx, [eax + 4]
    mov [syscall_log_result + 12], ecx
    mov ecx, [eax + 8]
    mov [syscall_log_result + 16], ecx
    mov ecx, [eax + 12]
    mov [syscall_log_result + 20], ecx
    mov ecx, [eax + 16]
    mov [syscall_log_result + 24], ecx
    mov ecx, [eax + 24]
    mov [syscall_log_result + 28], ecx
    mov esi, message_logging_read_ok
.log_copy:
    push esi
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_log_result
    mov ecx, 32
    call syscall_copy_buffer_to_user
    jc .log_bad_pointer
    pop esi
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.log_bad_pointer:
    pop esi
    jmp .bad_pointer
.log_would_block:
    mov eax, SYSCALL_STATUS_WOULD_BLOCK
    jmp .reject
.core_ready:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 0
    jne .bad_size
    cmp dword [edx + 36], 0
    jne .bad_pointer
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_SERVICE
    call security_check
    jc .access_denied
    mov dword [userspace_ready_seen], 1
    mov esi, message_shared_service_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.core_close_handle:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov esi, [edx + 36]
    mov ecx, 16
    mov edi, syscall_argument_buffer
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_argument_buffer + 0], 16
    jne .bad_size
    cmp dword [syscall_argument_buffer + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_argument_buffer + 12], 0
    jne .bad_reserved
    mov edx, [syscall_argument_buffer + 8]
    mov [syscall_closed_handle], edx
    mov eax, [userspace_pid]
    call handle_close
    jc .access_denied
    ; Derselbe codierte Wert muss nach Generationswechsel ungültig sein.
    mov eax, [userspace_pid]
    mov edx, [syscall_closed_handle]
    mov ebx, OBJECT_TYPE_VFS_NODE
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jnc .bad_reserved
    mov esi, message_handle_close_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.process:
    cmp dword [edx + 32], 2
    je .process_handle
    cmp dword [edx + 32], SYSCALL_QUERY_SELF
    jne .unknown_operation
    mov eax, [userspace_pid]
    mov esi, message_process_syscall_ok
    jmp .process_value
.process_handle:
    mov eax, [userspace_process_handle]
    mov esi, message_process_handle_ok
.process_value:
    mov [syscall_identity], eax
    jmp .identity
.thread:
    cmp dword [edx + 32], 2
    je .thread_handle
    cmp dword [edx + 32], SYSCALL_QUERY_SELF
    jne .unknown_operation
    mov eax, [userspace_tid]
    mov esi, message_thread_syscall_ok
    jmp .thread_value
.thread_handle:
    mov eax, [userspace_thread_handle]
    mov esi, message_thread_handle_ok
.thread_value:
    mov [syscall_identity], eax
.identity:
    push esi
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .identity_bad_abi
    cmp dword [edx + 20], 16
    jb .identity_bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_SERVICE
    call security_check
    jc .identity_access
    mov dword [syscall_result_buffer + 0], 16
    mov dword [syscall_result_buffer + 4], SYSCALL_ABI_VERSION
    mov eax, [syscall_identity]
    mov [syscall_result_buffer + 8], eax
    mov dword [syscall_result_buffer + 12], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov ecx, 16
    call syscall_copy_to_user
    jc .identity_pointer
    pop esi
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.identity_bad_abi:
    pop esi
    jmp .bad_abi
.identity_bad_size:
    pop esi
    jmp .bad_size
.identity_access:
    pop esi
    jmp .access_denied
.identity_pointer:
    pop esi
    jmp .bad_pointer
.ipc:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 48
    jb .bad_size
    cmp dword [edx + 32], SYSCALL_IPC_SEND
    je .ipc_send
    cmp dword [edx + 32], SYSCALL_IPC_RECEIVE
    je .ipc_receive
    jmp .unknown_operation
.ipc_send:
    mov esi, [edx + 36]
    mov ecx, 48
    mov edi, userspace_ipc_copy
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [userspace_ipc_copy], 48
    jne .bad_size
    cmp dword [userspace_ipc_copy + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [userspace_ipc_copy + 32], 8
    ja .bad_size
    cmp dword [userspace_ipc_copy + 36], 0
    jne .bad_reserved
    mov eax, [userspace_pid]
    mov edx, [userspace_ipc_copy + 8]
    mov ebx, OBJECT_TYPE_IPC_ENDPOINT
    mov ecx, HANDLE_RIGHT_SEND
    call handle_resolve
    jc .access_denied
    cmp dword [userspace_ipc_count], 0
    jne .ipc_queue_full
    mov esi, userspace_ipc_copy
    mov edi, userspace_ipc_queue
    mov ecx, 12
    rep movsd
    mov dword [userspace_ipc_count], 1
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.ipc_receive:
    cmp dword [userspace_ipc_count], 1
    jne .ipc_would_block
    mov eax, [userspace_pid]
    mov edx, [userspace_ipc_receive_handle]
    mov ebx, OBJECT_TYPE_IPC_ENDPOINT
    mov ecx, HANDLE_RIGHT_RECEIVE
    call handle_resolve
    jc .access_denied
    mov esi, userspace_ipc_queue
    mov edi, userspace_ipc_copy
    mov ecx, 12
    rep movsd
    mov eax, [userspace_ipc_receive_handle]
    mov [userspace_ipc_copy + 8], eax
    mov dword [userspace_ipc_count], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, userspace_ipc_copy
    mov ecx, 48
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_ipc_roundtrip_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.ipc_queue_full:
    mov eax, -17
    jmp .reject
.ipc_would_block:
    mov eax, -19
    jmp .reject
.vfs:
    cmp dword [edx + 32], SYSCALL_VFS_LOOKUP
    je .vfs_lookup
    cmp dword [edx + 32], SYSCALL_VFS_OPEN_ROOT
    jne .unknown_operation
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_SERVICE
    call security_check
    jc .access_denied
    mov dword [syscall_result_buffer + 0], 16
    mov dword [syscall_result_buffer + 4], SYSCALL_ABI_VERSION
    mov eax, [userspace_root_handle]
    mov [syscall_result_buffer + 8], eax
    mov dword [syscall_result_buffer + 12], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_result_buffer
    mov ecx, 16
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_vfs_userspace_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.power:
    cmp dword [edx + 32], SYSCALL_POWER_QUERY_SYSTEM
    je .power_query
    cmp dword [edx + 32], SYSCALL_POWER_WAKE_ACQUIRE
    je .power_wake_acquire
    cmp dword [edx + 32], SYSCALL_POWER_WAKE_RELEASE
    je .power_wake_release
    cmp dword [edx + 32], SYSCALL_POWER_SET_PROFILE
    je .power_set_profile
    cmp dword [edx + 32], SYSCALL_POWER_REQUEST_STATE
    je .power_request_state
    jmp .unknown_operation
.network:
    cmp dword [edx + 32], SYSCALL_NETWORK_QUERY
    je .network_query
    cmp dword [edx + 32], SYSCALL_NETWORK_RAW_OPEN
    je .network_raw_open
    cmp dword [edx + 32], SYSCALL_NETWORK_SOCKET_CREATE
    je .network_socket_create
    cmp dword [edx + 32], SYSCALL_NETWORK_SEND
    je .network_send
    cmp dword [edx + 32], SYSCALL_NETWORK_RECEIVE
    je .network_receive
    cmp dword [edx + 32], SYSCALL_NETWORK_BIND
    je .network_bind
    cmp dword [edx + 32], SYSCALL_NETWORK_STREAM_CREATE
    je .network_stream_create
    cmp dword [edx + 32], SYSCALL_NETWORK_CONNECT
    je .network_connect
    cmp dword [edx + 32], SYSCALL_NETWORK_STREAM_SEND
    je .network_stream_send
    cmp dword [edx + 32], SYSCALL_NETWORK_STREAM_RECEIVE
    je .network_stream_receive
    cmp dword [edx + 32], SYSCALL_NETWORK_STREAM_BIND
    je .network_stream_bind
    cmp dword [edx + 32], SYSCALL_NETWORK_LISTEN
    je .network_listen
    cmp dword [edx + 32], SYSCALL_NETWORK_ACCEPT
    je .network_accept
    jmp .unknown_operation
.network_query:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 96
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_QUERY
    call security_check
    jc .access_denied
    mov dword [syscall_network_result + 0], 96
    mov dword [syscall_network_result + 4], SYSCALL_ABI_VERSION
    mov eax, [network_namespace_generation]
    mov [syscall_network_result + 8], eax
    mov dword [syscall_network_result + 12], 1
    mov eax, [network_loopback_id]
    mov [syscall_network_result + 16], eax
    mov eax, [network_loopback_state]
    mov [syscall_network_result + 20], eax
    mov dword [syscall_network_result + 24], NETWORK_LOOPBACK_MTU
    mov dword [syscall_network_result + 28], NETWORK_FEATURE_IPV4 | NETWORK_FEATURE_IPV6
    mov eax, [network_transmitted_packets]
    mov [syscall_network_result + 32], eax
    mov eax, [network_received_packets]
    mov [syscall_network_result + 36], eax
    mov eax, [network_dropped_packets]
    mov [syscall_network_result + 40], eax
    mov eax, [network_socket_port]
    mov [syscall_network_result + 44], eax
    mov eax, [network_icmp_echo_tests]
    mov [syscall_network_result + 48], eax
    mov eax, [network_route_count]
    mov [syscall_network_result + 52], eax
    mov eax, [network_route_lookups]
    mov [syscall_network_result + 56], eax
    mov eax, [network_route_hits]
    mov [syscall_network_result + 60], eax
    mov eax, [network_firewall_decisions]
    mov [syscall_network_result + 64], eax
    mov eax, [network_firewall_drops]
    mov [syscall_network_result + 68], eax
    mov eax, [network_ipv6_udp_tests]
    mov [syscall_network_result + 72], eax
    mov eax, [network_icmpv6_echo_tests]
    mov [syscall_network_result + 76], eax
    mov eax, [network_tcp_tests]
    mov [syscall_network_result + 80], eax
    mov eax, [network_tcp_transmitted_bytes]
    mov [syscall_network_result + 84], eax
    mov eax, [network_tcp_received_bytes]
    mov [syscall_network_result + 88], eax
    mov eax, [network_tcp_accepted_connections]
    mov [syscall_network_result + 92], eax
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_network_result
    mov ecx, 96
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_network_query_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_raw_open:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 0
    jne .bad_size
    cmp dword [edx + 36], 0
    jne .bad_pointer
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_RAW
    call security_check
    jc .network_raw_denied
    jmp .unknown_operation
.network_raw_denied:
    mov esi, message_network_raw_denied
    call serial_write_string
    jmp .access_denied
.network_socket_create:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_CONNECT
    call security_check
    jc .access_denied
    mov eax, OBJECT_TYPE_NET_SOCKET
    mov edx, NETWORK_SOCKET_DATAGRAM
    mov ebx, 1                       ; Namespace 1
    call object_create
    jc .unknown_operation
    mov [network_socket_object], eax
    mov edx, eax
    mov eax, [userspace_pid]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_QUERY | HANDLE_RIGHT_SEND | HANDLE_RIGHT_RECEIVE | HANDLE_RIGHT_WAIT | HANDLE_RIGHT_BIND
    xor esi, esi
    call handle_create
    jc .unknown_operation
    mov [network_socket_handle], eax
    mov dword [syscall_result_buffer], 16
    mov dword [syscall_result_buffer + 4], SYSCALL_ABI_VERSION
    mov [syscall_result_buffer + 8], eax
    mov dword [syscall_result_buffer + 12], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_result_buffer
    mov ecx, 16
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_network_socket_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_stream_create:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_CONNECT
    call security_check
    jc .access_denied
    mov eax, OBJECT_TYPE_NET_SOCKET
    mov edx, NETWORK_SOCKET_STREAM
    mov ebx, 1
    call object_create
    jc .unknown_operation
    mov [network_tcp_socket_object], eax
    mov edx, eax
    mov eax, [userspace_pid]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_QUERY | HANDLE_RIGHT_SEND | HANDLE_RIGHT_RECEIVE | HANDLE_RIGHT_WAIT | HANDLE_RIGHT_BIND | HANDLE_RIGHT_CONNECT
    xor esi, esi
    call handle_create
    jc .unknown_operation
    mov [network_tcp_socket_handle], eax
    mov dword [syscall_result_buffer], 16
    mov dword [syscall_result_buffer + 4], SYSCALL_ABI_VERSION
    mov [syscall_result_buffer + 8], eax
    mov dword [syscall_result_buffer + 12], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_result_buffer
    mov ecx, 16
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_network_stream_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_connect:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 28
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_CONNECT
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 28
    mov edi, syscall_network_connect
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_connect], 28
    jne .bad_size
    cmp dword [syscall_network_connect + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_network_connect + 12], NETWORK_AF_IPV4
    jne .unknown_operation
    cmp dword [syscall_network_connect + 16], NETWORK_IPV4_LOOPBACK
    jne .unknown_operation
    cmp dword [syscall_network_connect + 20], NETWORK_BOOTSTRAP_PORT
    jne .unknown_operation
    cmp dword [syscall_network_connect + 24], 0
    jne .bad_reserved
    mov eax, [userspace_pid]
    mov edx, [syscall_network_connect + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_CONNECT
    call handle_resolve
    jc .network_connect_handle_denied
    mov eax, 0x7F000001             ; Firewall erwartet Host-Reihenfolge
    mov ebx, 6
    mov ecx, NETWORK_BOOTSTRAP_PORT
    call network_firewall_check_ipv4
    jc .network_connect_firewall_denied
    mov dword [network_tcp_state], TCP_STATE_CLOSED
    mov eax, TCP_STATE_SYN_SENT
    call network_tcp_transition
    jc .network_connect_state_error
    mov eax, TCP_STATE_SYN_RECEIVED
    call network_tcp_transition
    jc .network_connect_state_error
    mov eax, TCP_STATE_ESTABLISHED
    call network_tcp_transition
    jc .network_connect_state_error
    mov dword [network_tcp_socket_connected], 1
    mov esi, message_network_connect_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_connect_handle_denied:
    mov esi, message_network_connect_handle_denied
    call serial_write_string
    jmp .access_denied
.network_connect_firewall_denied:
    mov esi, message_network_connect_firewall_denied
    call serial_write_string
    jmp .access_denied
.network_connect_state_error:
    mov esi, message_network_connect_state_error
    call serial_write_string
    jmp .unknown_operation
.network_stream_send:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 40
    jb .bad_size
    mov esi, [edx + 36]
    mov ecx, 40
    mov edi, syscall_network_tcp_packet
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_tcp_packet], 40
    jne .bad_size
    cmp dword [syscall_network_tcp_packet + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    mov ecx, [syscall_network_tcp_packet + 12]
    test ecx, ecx
    jz .bad_size
    cmp ecx, 16
    ja .network_packet_too_large
    cmp dword [syscall_network_tcp_packet + 16], 0
    jne .bad_reserved
    cmp dword [syscall_network_tcp_packet + 36], 0
    jne .bad_reserved
    cmp dword [network_tcp_socket_connected], 1
    jne .unknown_operation
    cmp dword [network_tcp_state], TCP_STATE_ESTABLISHED
    jne .unknown_operation
    cmp dword [network_tcp_queue_count], 0
    jne .network_would_block
    mov eax, [userspace_pid]
    mov edx, [syscall_network_tcp_packet + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_SEND
    call handle_resolve
    jc .access_denied
    mov ecx, [syscall_network_tcp_packet + 12]
    mov [network_tcp_queue_length], ecx
    mov esi, syscall_network_tcp_packet + 20
    mov edi, network_tcp_queue_payload
    rep movsb
    mov dword [network_tcp_queue_count], 1
    mov eax, [network_tcp_queue_length]
    add [network_tcp_transmitted_bytes], eax
    mov esi, message_network_stream_send_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_stream_receive:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 40
    jb .bad_size
    mov esi, [edx + 36]
    mov ecx, 40
    mov edi, syscall_network_tcp_packet
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_tcp_packet], 40
    jne .bad_size
    cmp dword [syscall_network_tcp_packet + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [network_tcp_queue_count], 1
    jne .network_would_block
    mov eax, [userspace_pid]
    mov edx, [syscall_network_tcp_packet + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_RECEIVE
    call handle_resolve
    jc .access_denied
    mov eax, [network_tcp_queue_length]
    cmp eax, [syscall_network_tcp_packet + 12]
    ja .bad_size
    mov [syscall_network_tcp_packet + 12], eax
    mov dword [syscall_network_tcp_packet + 16], 0
    mov ecx, eax
    mov esi, network_tcp_queue_payload
    mov edi, syscall_network_tcp_packet + 20
    rep movsb
    add [network_tcp_received_bytes], eax
    mov dword [network_tcp_queue_count], 0
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_network_tcp_packet
    mov ecx, 40
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_network_stream_receive_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_stream_bind:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 28
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_LISTEN
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 28
    mov edi, syscall_network_stream_bind
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_stream_bind], 28
    jne .bad_size
    cmp dword [syscall_network_stream_bind + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_network_stream_bind + 12], NETWORK_AF_IPV4
    jne .unknown_operation
    cmp dword [syscall_network_stream_bind + 16], NETWORK_IPV4_LOOPBACK
    jne .unknown_operation
    cmp dword [syscall_network_stream_bind + 20], NETWORK_BOOTSTRAP_PORT
    jne .unknown_operation
    cmp dword [syscall_network_stream_bind + 24], 0
    jne .bad_reserved
    cmp dword [network_tcp_listener_bound], 0
    jne .unknown_operation
    mov eax, [userspace_pid]
    mov edx, [syscall_network_stream_bind + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_BIND
    call handle_resolve
    jc .access_denied
    mov eax, [syscall_network_stream_bind + 8]
    mov [network_tcp_listener_handle], eax
    mov dword [network_tcp_listener_bound], 1
    inc dword [network_namespace_generation]
    mov esi, message_network_stream_bind_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_listen:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 20
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_LISTEN
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 20
    mov edi, syscall_network_listen
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_listen], 20
    jne .bad_size
    cmp dword [syscall_network_listen + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_network_listen + 12], 1
    jne .bad_size
    cmp dword [syscall_network_listen + 16], 0
    jne .bad_reserved
    cmp dword [network_tcp_listener_bound], 1
    jne .unknown_operation
    mov eax, [syscall_network_listen + 8]
    cmp eax, [network_tcp_listener_handle]
    jne .access_denied
    mov eax, [userspace_pid]
    mov edx, [syscall_network_listen + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_WAIT
    call handle_resolve
    jc .access_denied
    mov dword [network_tcp_listener_state], TCP_STATE_LISTEN
    mov dword [network_tcp_listener_backlog], 1
    mov dword [network_tcp_pending_connections], 1
    mov esi, message_network_listen_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_accept:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 20
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_LISTEN
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 20
    mov edi, syscall_network_accept
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_accept], 20
    jne .bad_size
    cmp dword [syscall_network_accept + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_network_accept + 16], 0
    jne .bad_reserved
    mov eax, [syscall_network_accept + 8]
    cmp eax, [network_tcp_listener_handle]
    jne .access_denied
    cmp dword [network_tcp_listener_state], TCP_STATE_LISTEN
    jne .unknown_operation
    cmp dword [network_tcp_pending_connections], 1
    jne .network_would_block
    mov eax, [userspace_pid]
    mov edx, [syscall_network_accept + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_WAIT
    call handle_resolve
    jc .access_denied
    mov eax, OBJECT_TYPE_NET_SOCKET
    mov edx, NETWORK_SOCKET_STREAM
    mov ebx, 1
    call object_create
    jc .unknown_operation
    mov edx, eax
    mov eax, [userspace_pid]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_QUERY | HANDLE_RIGHT_SEND | HANDLE_RIGHT_RECEIVE | HANDLE_RIGHT_WAIT
    xor esi, esi
    call handle_create
    jc .unknown_operation
    mov [syscall_network_accept + 12], eax
    mov dword [network_tcp_pending_connections], 0
    inc dword [network_tcp_accepted_connections]
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_network_accept
    mov ecx, 20
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_network_accept_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_bind:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 28
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_NET_LISTEN
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 28
    mov edi, syscall_network_bind
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_bind], 28
    jne .bad_size
    cmp dword [syscall_network_bind + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_network_bind + 12], NETWORK_AF_IPV4
    jne .unknown_operation
    cmp dword [syscall_network_bind + 16], NETWORK_IPV4_LOOPBACK
    jne .unknown_operation
    mov eax, [syscall_network_bind + 20]
    test eax, eax
    jz .unknown_operation
    cmp eax, 65535
    ja .unknown_operation
    cmp dword [syscall_network_bind + 24], 0
    jne .bad_reserved
    cmp dword [network_socket_bound], 0
    jne .unknown_operation
    mov eax, [userspace_pid]
    mov edx, [syscall_network_bind + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_BIND
    call handle_resolve
    jc .access_denied
    mov eax, [syscall_network_bind + 20]
    mov [network_socket_port], eax
    mov dword [network_socket_bound], 1
    inc dword [network_namespace_generation]
    mov esi, message_network_bind_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_send:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 40
    jb .bad_size
    mov esi, [edx + 36]
    mov ecx, 40
    mov edi, syscall_network_packet
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_network_packet], 40
    jne .bad_size
    cmp dword [syscall_network_packet + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_network_packet + 12], 16
    ja .network_packet_too_large
    cmp dword [syscall_network_packet + 16], 0
    jne .bad_reserved
    cmp dword [syscall_network_packet + 36], 0
    jne .bad_reserved
    cmp dword [network_loopback_queue_count], 0
    jne .network_would_block
    cmp dword [network_socket_bound], 1
    jne .unknown_operation
    mov eax, 0x7F000001
    mov ebx, 17                     ; UDP
    mov ecx, [network_socket_port]
    call network_firewall_check_ipv4
    jc .access_denied
    mov eax, [userspace_pid]
    mov edx, [syscall_network_packet + 8]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_SEND
    call handle_resolve
    jc .access_denied
    call network_loopback_encapsulate
    jc .unknown_operation
    mov dword [network_loopback_queue_count], 1
    inc dword [network_transmitted_packets]
    mov esi, message_network_send_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_packet_too_large:
    mov esi, message_network_size_denied
    call serial_write_string
    jmp .bad_size
.network_receive:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 40
    jb .bad_size
    cmp dword [network_loopback_queue_count], 1
    jne .network_would_block
    call network_loopback_validate
    jc .network_malformed
    mov eax, [userspace_pid]
    mov edx, [network_socket_handle]
    mov ebx, OBJECT_TYPE_NET_SOCKET
    mov ecx, HANDLE_RIGHT_RECEIVE
    call handle_resolve
    jc .access_denied
    mov dword [syscall_network_packet], 40
    mov dword [syscall_network_packet + 4], SYSCALL_ABI_VERSION
    mov eax, [network_socket_handle]
    mov [syscall_network_packet + 8], eax
    mov eax, [network_loopback_payload_length]
    mov [syscall_network_packet + 12], eax
    mov dword [syscall_network_packet + 16], 0
    mov esi, network_loopback_frame + 28
    mov edi, syscall_network_packet + 20
    mov ecx, 16
    rep movsb
    mov dword [syscall_network_packet + 36], 0
    mov dword [network_loopback_queue_count], 0
    inc dword [network_received_packets]
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_network_packet
    mov ecx, 40
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_network_loopback_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.network_malformed:
    mov dword [network_loopback_queue_count], 0
    inc dword [network_dropped_packets]
    mov esi, message_network_malformed
    call serial_write_string
    jmp .unknown_operation
.network_would_block:
    mov esi, message_network_would_block
    call serial_write_string
    mov eax, SYSCALL_STATUS_WOULD_BLOCK
    jmp .reject
.power_query:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 32
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_POWER_QUERY
    call security_check
    jc .access_denied
    mov dword [syscall_power_result + 0], 32
    mov dword [syscall_power_result + 4], SYSCALL_ABI_VERSION
    mov eax, [power_current_state]
    mov [syscall_power_result + 8], eax
    mov dword [syscall_power_result + 12], POWER_SUPPORTED_MASK
    mov eax, [power_profile]
    mov [syscall_power_result + 16], eax
    mov eax, [power_wake_lock_count]
    mov [syscall_power_result + 20], eax
    mov eax, [power_idle_entries]
    mov [syscall_power_result + 24], eax
    mov eax, [power_deep_idle_entries]
    mov [syscall_power_result + 28], eax
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_power_result
    mov ecx, 32
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_power_query_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.power_wake_acquire:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_POWER_WAKE
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 16
    mov edi, syscall_argument_buffer
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_argument_buffer], 16
    jne .bad_size
    cmp dword [syscall_argument_buffer + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_argument_buffer + 12], 0
    jne .bad_reserved
    mov eax, [syscall_argument_buffer + 8]
    call power_wake_lock_acquire
    jc .unknown_operation
    mov [syscall_argument_buffer + 8], eax
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_argument_buffer
    mov ecx, 16
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_power_wake_acquire_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.power_wake_release:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_POWER_WAKE
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 16
    mov edi, syscall_argument_buffer
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_argument_buffer], 16
    jne .bad_size
    cmp dword [syscall_argument_buffer + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_argument_buffer + 12], 0
    jne .bad_reserved
    mov eax, [syscall_argument_buffer + 8]
    call power_wake_lock_release
    jc .unknown_operation
    mov esi, message_power_wake_release_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.power_set_profile:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 16
    jb .bad_size
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_POWER_PROFILE
    call security_check
    jc .access_denied
    mov edx, [syscall_frame]
    mov esi, [edx + 36]
    mov ecx, 16
    mov edi, syscall_argument_buffer
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_argument_buffer], 16
    jne .bad_size
    cmp dword [syscall_argument_buffer + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_argument_buffer + 12], 0
    jne .bad_reserved
    mov eax, [syscall_argument_buffer + 8]
    call power_manager_set_profile
    jc .unknown_operation
    mov esi, message_power_profile_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.power_request_state:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 24
    jb .bad_size
    mov esi, [edx + 36]
    mov ecx, 24
    mov edi, syscall_power_request
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_power_request], 24
    jne .bad_size
    cmp dword [syscall_power_request + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_power_request + 8], POWER_STATE_SHUTDOWN
    jne .unknown_operation
    cmp dword [syscall_power_request + 16], 0
    jne .bad_reserved
    cmp dword [syscall_power_request + 20], 0
    jne .bad_reserved
    mov eax, [userspace_pid]
    mov edx, SECURITY_CAP_POWER_SHUTDOWN
    call security_check
    jc .power_request_denied
    call power_manager_request_shutdown
    jc .unknown_operation
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    mov dword [edx + 56], kernel_shutdown_authorized
    mov dword [edx + 60], CODE_SEGMENT
    and dword [edx + 64], 0xFFFFCFFF
    ret
.power_request_denied:
    mov esi, message_power_shutdown_denied
    call serial_write_string
    jmp .access_denied
.vfs_lookup:
    cmp dword [edx + 40], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [edx + 20], 32
    jb .bad_size
    mov esi, [edx + 36]
    mov ecx, 32
    mov edi, syscall_vfs_buffer
    call syscall_copy_from_user
    jc .bad_pointer
    cmp dword [syscall_vfs_buffer + 0], 32
    jne .bad_size
    cmp dword [syscall_vfs_buffer + 4], SYSCALL_ABI_VERSION
    jne .bad_abi
    cmp dword [syscall_vfs_buffer + 16], 1
    jne .bad_size
    cmp dword [syscall_vfs_buffer + 20], 0
    jne .bad_reserved
    cmp dword [syscall_vfs_buffer + 28], 0
    jne .bad_reserved
    mov eax, [userspace_pid]
    mov edx, [syscall_vfs_buffer + 8]
    mov ebx, OBJECT_TYPE_VFS_NODE
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jc .access_denied
    mov eax, [syscall_vfs_buffer + 12]
    mov esi, eax
    mov ecx, 1
    mov edi, syscall_vfs_path
    call syscall_copy_from_user
    jc .bad_pointer
    mov esi, syscall_vfs_path
    mov ecx, 1
    call vfs_lookup_root
    jc .unknown_operation
    mov edx, eax
    mov eax, [userspace_pid]
    mov ebx, OBJECT_TYPE_VFS_NODE
    mov ecx, HANDLE_RIGHT_QUERY
    xor esi, esi
    call handle_create
    jc .access_denied
    mov [syscall_vfs_buffer + 24], eax
    mov edx, [syscall_frame]
    mov edi, [edx + 36]
    mov esi, syscall_vfs_buffer
    mov ecx, 32
    call syscall_copy_buffer_to_user
    jc .bad_pointer
    mov esi, message_vfs_lookup_ok
    call serial_write_string
    mov edx, [syscall_frame]
    mov dword [edx + 44], SYSCALL_STATUS_OK
    ret
.unknown_service:
    mov eax, SYSCALL_STATUS_SERVICE
    jmp .reject
.unknown_operation:
    mov eax, SYSCALL_STATUS_OPERATION
    jmp .reject
.bad_abi:
    mov eax, SYSCALL_STATUS_ABI
    jmp .reject
.bad_size:
    mov eax, SYSCALL_STATUS_SIZE
    jmp .reject
.bad_reserved:
    mov eax, SYSCALL_STATUS_RESERVED
    jmp .reject
.access_denied:
    mov eax, SYSCALL_STATUS_ACCESS
    jmp .reject
.bad_pointer:
    mov eax, SYSCALL_STATUS_POINTER
.reject:
    inc dword [syscall_rejected]
    mov edx, [syscall_frame]
    mov [edx + 44], eax
    ret

userspace_pid:        dd 0
userspace_tid:        dd 0
userspace_process_handle: dd 0
userspace_thread_handle:  dd 0
userspace_code_page:  dd 0
userspace_stack_page: dd 0
userspace_exit_seen:  dd 0
userspace_exit_code:  dd 0
syscall_frame:        dd 0
syscall_total:        dd 0
syscall_rejected:     dd 0
syscall_identity:     dd 0
syscall_closed_handle: dd 0
align 4
syscall_argument_buffer:
    times 16 db 0
syscall_result_buffer:
    times 16 db 0
syscall_vfs_buffer: times 32 db 0
syscall_vfs_path:   times 4 db 0
syscall_power_result: times 32 db 0
syscall_power_request: times 24 db 0
syscall_network_result: times 96 db 0
syscall_network_packet: times 40 db 0
syscall_network_bind: times 28 db 0
syscall_network_connect: times 28 db 0
syscall_network_tcp_packet: times 40 db 0
syscall_network_stream_bind: times 28 db 0
syscall_network_listen: times 20 db 0
syscall_network_accept: times 20 db 0
syscall_log_result: times 32 db 0

OBJECT_TYPE_IPC_ENDPOINT equ 11

userspace_ipc_initialize:
    mov dword [userspace_ipc_count], 0
    mov edi, userspace_ipc_queue
    xor eax, eax
    mov ecx, 48 / 4
    rep stosd
    mov eax, OBJECT_TYPE_IPC_ENDPOINT
    mov edx, 1
    mov ebx, 2
    call object_create
    jc .invalid
    mov [userspace_ipc_send_object], eax
    mov eax, OBJECT_TYPE_IPC_ENDPOINT
    mov edx, 2
    mov ebx, 1
    call object_create
    jc .invalid
    mov [userspace_ipc_receive_object], eax
    mov eax, 2
    mov edx, [userspace_ipc_send_object]
    mov ebx, OBJECT_TYPE_IPC_ENDPOINT
    mov ecx, HANDLE_RIGHT_SEND
    xor esi, esi
    call handle_create
    jc .invalid
    mov [userspace_ipc_send_handle], eax
    mov eax, 2
    mov edx, [userspace_ipc_receive_object]
    mov ebx, OBJECT_TYPE_IPC_ENDPOINT
    mov ecx, HANDLE_RIGHT_RECEIVE | HANDLE_RIGHT_WAIT
    xor esi, esi
    call handle_create
    jc .invalid
    mov [userspace_ipc_receive_handle], eax
    mov edi, [userspace_code_page]
    add edi, userspace_ipc_packet - userspace_program_start + 8
    mov eax, [userspace_ipc_send_handle]
    mov [edi], eax
    cmp dword [handle_active_count], 4
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

userspace_ipc_send_object:    dd 0
userspace_ipc_receive_object: dd 0
userspace_ipc_send_handle:    dd 0
userspace_ipc_receive_handle: dd 0
userspace_ipc_count:          dd 0
align 4
userspace_ipc_copy:  times 48 db 0
userspace_ipc_queue: times 48 db 0

userspace_vfs_initialize:
    mov eax, 2
    mov edx, [vfs_state + VFS_ROOT_NODE_HANDLE]
    mov ebx, OBJECT_TYPE_VFS_NODE
    mov ecx, HANDLE_RIGHT_QUERY
    mov esi, HANDLE_FLAG_PROTECTED
    call handle_create
    jc .invalid
    mov [userspace_root_handle], eax
    cmp dword [handle_active_count], 5
    jne .invalid
    mov edx, eax
    mov eax, 2
    mov ebx, OBJECT_TYPE_VFS_NODE
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jc .invalid
    mov edx, [userspace_root_handle]
    mov eax, 2
    mov ebx, OBJECT_TYPE_DEVICE       ; Root-Handle ist kein Device-Handle
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jnc .invalid
    clc
    ret
.invalid:
    stc
    ret

userspace_root_handle: dd 0

; Erstellt eine ausschließlich lesbare, öffentliche Service-Informationsseite.
; Physische Adresse und interne Kernelobjekte werden darin nie veröffentlicht.
shared_service_page_initialize:
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [shared_service_page], eax
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    mov edi, [shared_service_page]
    mov dword [edi + 0], SHARED_SERVICE_SIGNATURE
    mov dword [edi + 4], SHARED_SERVICE_SIZE
    mov dword [edi + 8], SYSCALL_ABI_VERSION
    mov dword [edi + 12], SHARED_FEATURE_INT80 | SHARED_FEATURE_COPY_IO | SHARED_FEATURE_PREEMPT
    mov dword [edi + 16], SHARED_SERVICE_BITMAP
    mov dword [edi + 20], PMM_PAGE_SIZE
    mov dword [edi + 24], 100
    mov dword [edi + 28], BOOT_PHASE_USERSPACE
    mov eax, [timer_ticks]
    mov [edi + 32], eax
    mov dword [edi + 36], 0
    mov eax, SHARED_SERVICE_ADDRESS
    mov edx, [shared_service_page]
    mov ebx, PAGING_PAGE_PRESENT | PAGING_PAGE_USER
    call paging_map_page
    jc .invalid
    mov eax, cr3
    mov cr3, eax
    clc
    ret
.invalid:
    stc
    ret

shared_service_page: dd 0
userspace_ready_seen: dd 0

; Kernel Security / Capability Manager (ADR-2013)
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
SECURITY_KERNEL_CAPS    equ 0x00003FFF

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

; ---------------------------------------------------------------------------
; CPU Manager / BSP- und Topologieerkennung (NPSPEC-KERNEL-0026)
; ---------------------------------------------------------------------------
CPU_API_SIZE          equ 48
CPU_RECORD_SIZE       equ 96
CPU_CAPACITY          equ 8
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

cpu_manager_initialize:
    push ebp
    mov edi, cpu_records
    xor eax, eax
    mov ecx, (CPU_CAPACITY * CPU_RECORD_SIZE) / 4
    rep stosd
    mov edi, cpu_local_data
    mov ecx, 64 / 4
    rep stosd
    mov dword [cpu_possible_set], 1
    mov dword [cpu_discovered_set], 1
    mov dword [cpu_online_set], 0
    mov dword [cpu_active_set], 0
    mov dword [cpu_failed_set], 0
    mov dword [cpu_discovered_count], 1
    mov dword [cpu_online_count], 0
    mov dword [cpu_startup_attempts], 1

    mov edi, cpu_records
    mov dword [edi + 0], 0          ; bootlokale CPU ID
    mov dword [edi + 12], CPU_STATE_DISCOVERED
    mov dword [edi + 40], CPU_CAPACITY_SCALE
    mov dword [edi + 44], cpu_local_data
    mov dword [edi + 48], 1         ; BSP

    xor eax, eax
    cpuid
    mov [edi + 52], ebx             ; Herstellerkennung, 12 Byte
    mov [edi + 56], edx
    mov [edi + 60], ecx
    mov [cpu_max_basic_leaf], eax
    mov eax, 1
    cpuid
    mov [edi + 64], eax             ; Family/Model/Stepping
    mov [edi + 68], edx             ; rohe CPUID-Featurebits
    mov [edi + 72], ecx
    mov eax, ebx
    shr eax, 24
    mov [edi + 4], eax              ; APIC Hardware ID, low dword
    mov dword [edi + 8], 0
    mov eax, ebx
    shr eax, 16
    and eax, 0xFF
    test eax, eax
    jnz .logical_known
    mov eax, 1
.logical_known:
    mov [edi + 36], eax             ; gemeldete logische Package-Threads

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
    mov [edi + 76], ebp
    mov [cpu_system_features], ebp  ; Schnittmenge der aktiven CPUs

    ; Topologie des BSP: Package/Die/Cluster/Thread/NUMA beginnen definiert.
    mov dword [edi + 16], 0
    mov dword [edi + 20], 0
    mov dword [edi + 24], 0
    mov dword [edi + 28], 0
    mov dword [edi + 32], 0
    mov dword [edi + 80], 0xFFFFFFFF ; unbekannte LLC-ID

    ; Per-CPU-Basis muss vor ONLINE vollständig sein.
    mov dword [cpu_local_data + 0], cpu_records
    mov dword [cpu_local_data + 4], 0 ; current thread folgt dem Scheduler
    mov dword [cpu_local_data + 8], scheduler_current
    mov dword [cpu_local_data + 12], logging_ring
    mov dword [cpu_local_data + 16], 0 ; preemption depth
    mov dword [cpu_local_data + 20], 0 ; interrupt depth
    mov dword [cpu_local_data + 24], 0 ; exception depth
    mov dword [cpu_local_data + 28], KERNEL_STACK_TOP
    mov dword [cpu_local_data + 32], 1 ; lokaler Timer vorbereitet
    mov dword [cpu_local_data + 36], 1 ; Interruptcontroller vorbereitet
    mov dword [cpu_local_data + 40], 0x43505530 ; Canary/Owner-Marker
    mov dword [edi + 12], CPU_STATE_ONLINE
    mov dword [cpu_online_set], 1
    mov dword [cpu_online_count], 1
    mov dword [edi + 12], CPU_STATE_ACTIVE
    mov dword [cpu_active_set], 1
    pop ebp
    clc
    ret

; EAX=CPU ID, EDX=Zeiger auf internen Record bei Erfolg.
cpu_query:
    cmp eax, [cpu_discovered_count]
    jae .invalid
    imul edx, eax, CPU_RECORD_SIZE
    add edx, cpu_records
    cmp dword [edx + 12], CPU_STATE_FAILED
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
    cmp dword [edx + 12], CPU_STATE_ACTIVE
    jne .invalid
    mov dword [edx + 12], CPU_STATE_OFFLINE
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
    cmp dword [cpu_discovered_count], 1
    jne .invalid
    cmp dword [cpu_online_count], 1
    jne .invalid
    cmp dword [cpu_active_set], 1
    jne .invalid
    cmp dword [cpu_local_data + 28], KERNEL_STACK_TOP
    jne .invalid
    cmp dword [cpu_local_data + 32], 1
    jne .invalid
    cmp dword [cpu_local_data + 36], 1
    jne .invalid
    xor eax, eax
    call cpu_query
    jc .invalid
    cmp dword [edx + 12], CPU_STATE_ACTIVE
    jne .invalid
    mov eax, CPU_CAPACITY
    call cpu_query
    jnc .invalid
    xor eax, eax
    call cpu_offline               ; letzte aktive CPU muss abgelehnt werden
    jnc .invalid
    ; Kontrollierter ACTIVE-IDLE-ACTIVE-Übergang des BSP.
    mov dword [cpu_records + 12], CPU_STATE_IDLE
    mov dword [cpu_records + 12], CPU_STATE_ACTIVE
    clc
    ret
.invalid:
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
cpu_system_features:     dd 0
cpu_possible_set:        dd 0
cpu_discovered_set:      dd 0
cpu_online_set:          dd 0
cpu_active_set:          dd 0
cpu_failed_set:          dd 0
cpu_discovered_count:    dd 0
cpu_online_count:        dd 0
cpu_startup_attempts:    dd 0
cpu_offline_operations:  dd 0
align 16
cpu_local_data:          times 64 db 0
cpu_records:             times CPU_CAPACITY * CPU_RECORD_SIZE db 0

; ---------------------------------------------------------------------------
; Restriktiver Kernel Module Loader (NPSPEC-KERNEL-0025)
; ---------------------------------------------------------------------------
MODULE_MAGIC              equ 0x444D564E ; "NVMD"
MODULE_HEADER_SIZE        equ 64
MODULE_MAX_IMAGE_SIZE     equ 4096
MODULE_ARCH_X86_32        equ 1
MODULE_KERNEL_ABI         equ 0x00010000
MODULE_FLAG_TRUSTED       equ 0x00000001
MODULE_FLAG_STACK_GUARD   equ 0x00000002
MODULE_FLAG_UNLOADABLE    equ 0x00000004
MODULE_REQUIRED_FLAGS     equ MODULE_FLAG_TRUSTED | MODULE_FLAG_STACK_GUARD
MODULE_STATE_DISCOVERED   equ 0
MODULE_STATE_VALIDATING   equ 1
MODULE_STATE_LOADING      equ 2
MODULE_STATE_RELOCATING   equ 3
MODULE_STATE_INITIALIZING equ 4
MODULE_STATE_ACTIVE       equ 5
MODULE_STATE_QUIESCING    equ 6
MODULE_STATE_UNLOADING    equ 7
MODULE_STATE_FAILED       equ 8
MODULE_STATE_UNLOADED     equ 9

module_loader_initialize:
    mov edi, module_code_area
    xor eax, eax
    mov ecx, (64 + 64) / 4
    rep stosd
    mov dword [module_state], MODULE_STATE_DISCOVERED
    mov dword [module_instance_id], 0
    mov dword [module_active_calls], 0
    mov dword [module_loaded_count], 0
    mov dword [module_failed_count], 0
    mov dword [module_unload_count], 0
    clc
    ret

; ESI=vollständiges, bereits aus einer verifizierten Quelle gelesenes Paket.
module_load:
    mov [module_source], esi
    mov dword [module_state], MODULE_STATE_VALIDATING
    cmp dword [esi + 0], MODULE_MAGIC
    jne .reject
    cmp dword [esi + 4], MODULE_HEADER_SIZE
    jne .reject
    mov eax, [esi + 8]
    cmp eax, MODULE_HEADER_SIZE
    jbe .reject
    cmp eax, MODULE_MAX_IMAGE_SIZE
    ja .reject
    cmp dword [esi + 12], MODULE_ARCH_X86_32
    jne .reject
    cmp dword [esi + 16], MODULE_KERNEL_ABI
    ja .reject
    cmp dword [esi + 20], MODULE_KERNEL_ABI
    jb .reject
    mov eax, [esi + 28]
    and eax, MODULE_REQUIRED_FLAGS
    cmp eax, MODULE_REQUIRED_FLAGS
    jne .reject
    cmp dword [esi + 60], 0
    jne .reject
    ; Code- und Datensektion müssen vollständig innerhalb der Datei liegen.
    mov eax, [esi + 32]
    cmp eax, MODULE_HEADER_SIZE
    jb .reject
    mov edx, eax
    add edx, [esi + 36]
    jc .reject
    cmp edx, [esi + 8]
    ja .reject
    cmp dword [esi + 36], 64
    ja .reject
    mov eax, [esi + 40]
    cmp eax, MODULE_HEADER_SIZE
    jb .reject
    mov ecx, eax
    add ecx, [esi + 44]
    jc .reject
    cmp ecx, [esi + 8]
    ja .reject
    cmp dword [esi + 44], 64
    ja .reject
    ; Überlappende Code-/Datensektionen werden strikt abgelehnt.
    mov eax, [esi + 32]
    add eax, [esi + 36]
    cmp eax, [esi + 40]
    jbe .sections_ok
    mov eax, [esi + 40]
    add eax, [esi + 44]
    cmp eax, [esi + 32]
    ja .reject
.sections_ok:
    ; Die initiale Trust-Schicht prüft den signierten Inhaltsdigest.
    mov ecx, [esi + 8]
    sub ecx, MODULE_HEADER_SIZE
    lea edi, [esi + MODULE_HEADER_SIZE]
    xor eax, eax
.digest:
    movzx edx, byte [edi]
    add eax, edx
    rol eax, 3
    inc edi
    loop .digest
    cmp eax, [esi + 48]
    jne .reject
    ; Abhängigkeit 0 bedeutet keine; andere IDs müssen bereits aktiv sein.
    mov eax, [esi + 52]
    test eax, eax
    jz .dependency_ok
    cmp eax, [module_id]
    jne .reject
    cmp dword [module_state], MODULE_STATE_ACTIVE
    jne .reject
.dependency_ok:
    mov dword [module_state], MODULE_STATE_LOADING
    mov eax, [esi + 32]
    add eax, esi
    push esi
    mov esi, eax
    mov edi, module_code_area
    mov ecx, [module_source]
    mov ecx, [ecx + 36]
    rep movsb
    pop esi
    mov eax, [esi + 40]
    add eax, esi
    push esi
    mov esi, eax
    mov edi, module_data_area
    mov ecx, [module_source]
    mov ecx, [ecx + 44]
    rep movsb
    pop esi
    mov dword [module_state], MODULE_STATE_RELOCATING
    ; Keine Relokation ist im Format v1 gleichbedeutend mit abgeschlossen.
    mov dword [module_code_rights], 0x5 ; R-X
    mov dword [module_data_rights], 0x3 ; RW-
    mov dword [module_state], MODULE_STATE_INITIALIZING
    mov eax, [esi + 56]
    mov [module_id], eax
    mov eax, [esi + 28]
    mov [module_flags], eax
    inc dword [module_instance_id]
    ; Erst jetzt wird die vollständig geprüfte Instanz atomar sichtbar.
    mov dword [module_state], MODULE_STATE_ACTIVE
    inc dword [module_loaded_count]
    clc
    ret
.reject:
    mov dword [module_state], MODULE_STATE_FAILED
    inc dword [module_failed_count]
    stc
    ret

module_unload:
    cmp dword [module_state], MODULE_STATE_ACTIVE
    jne .reject
    test dword [module_flags], MODULE_FLAG_UNLOADABLE
    jz .reject
    cmp dword [module_active_calls], 0
    jne .reject
    mov dword [module_state], MODULE_STATE_QUIESCING
    mov dword [module_state], MODULE_STATE_UNLOADING
    mov edi, module_code_area
    xor eax, eax
    mov ecx, (64 + 64) / 4
    rep stosd
    mov dword [module_code_rights], 0
    mov dword [module_data_rights], 0
    mov dword [module_state], MODULE_STATE_UNLOADED
    inc dword [module_unload_count]
    clc
    ret
.reject:
    stc
    ret

module_loader_self_test:
    mov esi, module_test_image
    call module_load
    jc .invalid
    cmp dword [module_state], MODULE_STATE_ACTIVE
    jne .invalid
    cmp dword [module_code_rights], 0x5
    jne .invalid
    cmp dword [module_data_rights], 0x3
    jne .invalid
    mov dword [module_active_calls], 1
    call module_unload
    jnc .invalid
    mov dword [module_active_calls], 0
    call module_unload
    jc .invalid
    ; Manipulierte Architektur darf niemals bis zur Veröffentlichung gelangen.
    mov dword [module_test_image + 12], 0xFFFFFFFF
    mov esi, module_test_image
    call module_load
    jnc .restore_invalid
    mov dword [module_test_image + 12], MODULE_ARCH_X86_32
    ; Am Ende bleibt eine gültige Diagnosemodulinstanz aktiv.
    mov esi, module_test_image
    call module_load
    jc .invalid
    cmp dword [module_loaded_count], 2
    jne .invalid
    cmp dword [module_failed_count], 1
    jne .invalid
    cmp dword [module_unload_count], 1
    jne .invalid
    clc
    ret
.restore_invalid:
    mov dword [module_test_image + 12], MODULE_ARCH_X86_32
.invalid:
    stc
    ret

align 4
module_state:          dd 0
module_instance_id:    dd 0
module_id:             dd 0
module_flags:          dd 0
module_active_calls:   dd 0
module_code_rights:    dd 0
module_data_rights:    dd 0
module_loaded_count:   dd 0
module_failed_count:   dd 0
module_unload_count:   dd 0
module_source:         dd 0
module_code_area:      times 64 db 0
module_data_area:      times 64 db 0

; Kleines signiertes In-Kernel-Testpaket. Digest wird über 32 Nutzbytes gebildet.
align 4
module_test_image:
    dd MODULE_MAGIC, MODULE_HEADER_SIZE, 96, MODULE_ARCH_X86_32
    dd MODULE_KERNEL_ABI, MODULE_KERNEL_ABI, 5
    dd MODULE_FLAG_TRUSTED | MODULE_FLAG_STACK_GUARD | MODULE_FLAG_UNLOADABLE
    dd 64, 16, 80, 16
    dd 0xD0F97818, 0, 0x44494147, 0
    db 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16
    times 16 db 0x10

; ---------------------------------------------------------------------------
; Präemptiver Round-Robin-Scheduler (ADR-2004 / ADR-2012)
; ---------------------------------------------------------------------------

SCHEDULER_THREAD_COUNT equ 3
SCHEDULER_FRAME_SIZE   equ 68
SCHEDULER_API_SIZE     equ 32
SCHEDULER_CAP_PREEMPT  equ 0x00000001
SCHEDULER_CAP_RR       equ 0x00000002

; Thread Manager (ADR-2012)
THREAD_API_SIZE      equ 32
THREAD_CAPACITY      equ 3
THREAD_RECORD_SIZE   equ 32
THREAD_STATE_READY   equ 1
OBJECT_TYPE_THREAD   equ 5
THREAD_TID      equ 0
THREAD_PID      equ 4
THREAD_STATE    equ 8
THREAD_SLOT     equ 12
THREAD_HANDLE   equ 16
THREAD_ENTRY    equ 20
THREAD_CONTEXT  equ 24

thread_manager_initialize:
    mov edi, thread_table
    xor eax, eax
    mov ecx, (THREAD_CAPACITY * THREAD_RECORD_SIZE) / 4
    rep stosd
    mov dword [thread_count], 0
    mov dword [thread_next_tid], 1
    mov eax, kernel_main
    mov edx, 1
    xor ebx, ebx
    call thread_register
    jc .invalid
    mov eax, scheduler_thread1
    mov edx, 1
    mov ebx, 1
    call thread_register
    jc .invalid
    mov eax, scheduler_thread2
    mov edx, 1
    mov ebx, 2
    call thread_register
    jc .invalid
    clc
    ret
.invalid:
    stc
    ret

; EAX=Einstieg, EDX=PID, EBX=Scheduler-Slot. EAX=TID.
thread_register:
    pushfd
    cli
    test eax, eax
    jz .invalid
    cmp ebx, SCHEDULER_THREAD_COUNT
    jae .invalid
    mov [thread_temp_entry], eax
    mov [thread_temp_pid], edx
    mov [thread_temp_slot], ebx
    mov eax, edx
    call process_lookup
    jc .invalid
    mov ecx, [thread_count]
    cmp ecx, THREAD_CAPACITY
    jae .invalid
    mov edi, ecx
    shl edi, 5
    add edi, thread_table
    mov [thread_temp_record], edi
    mov eax, OBJECT_TYPE_THREAD
    mov edx, [thread_temp_pid]
    mov ebx, [thread_temp_entry]
    call object_create
    jc .invalid
    mov edi, [thread_temp_record]
    mov edx, [thread_next_tid]
    mov [edi + THREAD_TID], edx
    mov ecx, [thread_temp_pid]
    mov [edi + THREAD_PID], ecx
    mov dword [edi + THREAD_STATE], THREAD_STATE_READY
    mov ecx, [thread_temp_slot]
    mov [edi + THREAD_SLOT], ecx
    mov [edi + THREAD_HANDLE], eax
    mov ecx, [thread_temp_entry]
    mov [edi + THREAD_ENTRY], ecx
    mov ecx, [thread_temp_slot]
    mov ecx, [scheduler_contexts + ecx * 4]
    mov [edi + THREAD_CONTEXT], ecx
    inc dword [thread_count]
    inc dword [thread_next_tid]
    mov eax, edx
    popfd
    clc
    ret
.invalid:
    xor eax, eax
    popfd
    stc
    ret

; EAX=TID. EAX=Datensatz oder 0.
thread_lookup:
    xor ecx, ecx
.scan:
    cmp ecx, THREAD_CAPACITY
    jae .invalid
    mov edx, ecx
    shl edx, 5
    add edx, thread_table
    cmp [edx + THREAD_TID], eax
    je .found
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

thread_manager_self_test:
    mov eax, 2
    call thread_lookup
    jc .invalid
    cmp dword [eax + THREAD_PID], 1
    jne .invalid
    cmp dword [eax + THREAD_SLOT], 1
    jne .invalid
    cmp dword [eax + THREAD_ENTRY], scheduler_thread1
    jne .invalid
    cmp dword [thread_count], THREAD_CAPACITY
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

align 4
thread_manager_api:
    dd THREAD_API_SIZE
    dw 1, 0
    dd THREAD_CAPACITY
    dd thread_register
    dd thread_lookup
    dd thread_count
    dd thread_table
    dd thread_next_tid

thread_count:       dd 0
thread_next_tid:    dd 0
thread_temp_entry:  dd 0
thread_temp_pid:    dd 0
thread_temp_slot:   dd 0
thread_temp_record: dd 0
align 4
thread_table:
    times THREAD_CAPACITY * THREAD_RECORD_SIZE db 0

scheduler_initialize:
    mov dword [scheduler_enabled], 0
    mov dword [scheduler_current], 0
    mov dword [scheduler_contexts + 0], 0
    mov dword [scheduler_thread1_runs], 0
    mov dword [scheduler_thread2_runs], 0

    mov eax, scheduler_thread1
    call scheduler_create_frame
    jc .invalid
    mov [scheduler_contexts + 4], eax

    mov eax, scheduler_thread2
    call scheduler_create_frame
    jc .invalid
    mov [scheduler_contexts + 8], eax

    mov dword [scheduler_enabled], 1
    clc
    ret
.invalid:
    stc
    ret

; EAX=Thread-Einstieg, EAX=synthetischer Interruptframe.
scheduler_create_frame:
    push ebx
    push ecx
    push edi
    mov ebx, eax
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov edi, eax
    xor eax, eax
    mov ecx, PMM_PAGE_SIZE / 4
    rep stosd
    sub edi, SCHEDULER_FRAME_SIZE

    mov dword [edi + 0], DATA_SEGMENT
    mov dword [edi + 4], DATA_SEGMENT
    mov dword [edi + 8], DATA_SEGMENT
    mov dword [edi + 12], DATA_SEGMENT
    mov dword [edi + 16], 0          ; EDI
    mov dword [edi + 20], 0          ; ESI
    mov dword [edi + 24], 0          ; EBP
    mov dword [edi + 28], 0          ; ESP-Platzhalter von PUSHAD
    mov dword [edi + 32], 0          ; EBX
    mov dword [edi + 36], 0          ; EDX
    mov dword [edi + 40], 0          ; ECX
    mov dword [edi + 44], 0          ; EAX
    mov dword [edi + 48], 32         ; normalisierter Vektor
    mov dword [edi + 52], 0          ; Fehlercode
    mov [edi + 56], ebx              ; EIP
    mov dword [edi + 60], CODE_SEGMENT
    mov dword [edi + 64], 0x00000202 ; IF gesetzt
    mov eax, edi
    pop edi
    pop ecx
    pop ebx
    clc
    ret
.invalid:
    pop edi
    pop ecx
    pop ebx
    stc
    ret

; [ESP+4] enthält den vollständigen Frame des unterbrochenen Threads.
scheduler_on_tick:
    mov eax, [esp + 4]
    cmp dword [scheduler_enabled], 1
    jne .done
    mov edx, [scheduler_current]
    mov [scheduler_contexts + edx * 4], eax
    inc edx
    cmp edx, SCHEDULER_THREAD_COUNT
    jb .selected
    xor edx, edx
.selected:
    mov [scheduler_current], edx
    mov eax, [scheduler_contexts + edx * 4]
.done:
    ret

scheduler_thread1:
    inc dword [scheduler_thread1_runs]
    mov eax, [ipc_sent]
    inc eax
    mov dword [ipc_thread_message + 0], 1
    mov [ipc_thread_message + 4], eax
    mov dword [ipc_thread_message + 8], 0x4E4F5641
    mov dword [ipc_thread_message + 12], 0
    mov esi, ipc_thread_message
    call ipc_send
    test eax, eax
    jz .pause
    inc dword [ipc_sent]
.pause:
    pause
    jmp scheduler_thread1

scheduler_thread2:
    inc dword [scheduler_thread2_runs]
    mov edi, ipc_receive_buffer
    call ipc_receive
    test eax, eax
    jz .pause
    cmp dword [ipc_receive_buffer + 0], 1
    jne .error
    cmp dword [ipc_receive_buffer + 8], 0x4E4F5641
    jne .error
    mov eax, [ipc_receive_buffer + 4]
    cmp eax, [ipc_last_sequence]
    jbe .error
    mov [ipc_last_sequence], eax
    inc dword [ipc_received]
    jmp .pause
.error:
    mov dword [ipc_error], 1
.pause:
    pause
    jmp scheduler_thread2

scheduler_self_test:
    mov ecx, 100000000
.wait:
    cmp dword [scheduler_thread1_runs], 0
    je .continue
    cmp dword [scheduler_thread2_runs], 0
    je .continue
    cmp dword [ipc_error], 0
    jne .invalid
    cmp dword [ipc_received], 4
    jae .success
.continue:
    pause
    dec ecx
    jnz .wait
.invalid:
    stc
    ret
.success:
    clc
    ret

align 4
scheduler_api:
    dd SCHEDULER_API_SIZE
    dw 1, 0
    dd SCHEDULER_THREAD_COUNT
    dd SCHEDULER_CAP_PREEMPT | SCHEDULER_CAP_RR
    dd scheduler_on_tick
    dd scheduler_current
    dd scheduler_thread1_runs
    dd scheduler_thread2_runs

scheduler_enabled: dd 0
scheduler_current: dd 0
scheduler_contexts:
    times SCHEDULER_THREAD_COUNT dd 0
scheduler_thread1_runs: dd 0
scheduler_thread2_runs: dd 0

; ---------------------------------------------------------------------------
; Minimaler Kernel Main
; ---------------------------------------------------------------------------

kernel_main:
    call kernel_operational_prepare
    jmp kernel_idle

kernel_operational_prepare:
    mov eax, [kernel_context + CONTEXT_SEEN]
    test eax, CONTEXT_HAS_GRAPHICS
    jz .text_mode
    cmp dword [kernel_context + CONTEXT_BPP], 32
    jne .text_mode
    call draw_boot_screen
    mov esi, message_framebuffer_ok
    call serial_write_string
    jmp .ready

.text_mode:
    mov esi, message_text_mode
    call serial_write_string

.ready:
    mov esi, message_ready
    call serial_write_string
    ret

kernel_idle:
    sti
.loop:
    call power_cpu_idle
    jmp .loop

kernel_shutdown:
    cli
    call power_manager_request_shutdown
    jc kernel_halt
    call power_manager_platform_off
    jmp kernel_halt

kernel_shutdown_authorized:
    cli
    call power_manager_platform_off
    jmp kernel_halt

; ---------------------------------------------------------------------------
; Power Manager ABI 1.0 (NPSPEC-KERNEL-0021)
; ---------------------------------------------------------------------------
POWER_API_SIZE                 equ 72
POWER_STATE_RUNNING            equ 0
POWER_STATE_IDLE               equ 1
POWER_STATE_SUSPEND_TO_IDLE    equ 2
POWER_STATE_SUSPEND_TO_RAM     equ 3
POWER_STATE_HIBERNATE          equ 4
POWER_STATE_HYBRID_SLEEP       equ 5
POWER_STATE_SHUTDOWN           equ 6
POWER_STATE_RESTART            equ 7
POWER_STATE_OFF                equ 8
POWER_SUPPORTED_MASK           equ (1 << POWER_STATE_RUNNING) | (1 << POWER_STATE_IDLE) | (1 << POWER_STATE_SHUTDOWN) | (1 << POWER_STATE_RESTART) | (1 << POWER_STATE_OFF)
POWER_PROFILE_BALANCED         equ 1
POWER_PROFILE_PERFORMANCE      equ 0
POWER_PROFILE_EFFICIENCY       equ 2
POWER_PROFILE_POWERSAVE        equ 3
POWER_PROFILE_SUPPORTED_MASK   equ 0x0000000F
POWER_PHASE_NONE               equ 0
POWER_PHASE_REQUEST            equ 1
POWER_PHASE_FREEZE_USERSPACE   equ 2
POWER_PHASE_SYNC_STORAGE       equ 3
POWER_PHASE_STOP_DEVICES       equ 4
POWER_PHASE_PLATFORM_OFF       equ 5
POWER_WAKE_LOCK_CAPACITY       equ 2
POWER_IDLE_SHALLOW             equ 0
POWER_IDLE_DEEP                equ 1

power_manager_initialize:
    mov dword [power_current_state], POWER_STATE_RUNNING
    mov dword [power_target_state], POWER_STATE_RUNNING
    mov dword [power_profile], POWER_PROFILE_BALANCED
    mov dword [power_transition_phase], POWER_PHASE_NONE
    mov dword [power_transition_active], 0
    mov dword [power_shutdown_requests], 0
    mov dword [power_transition_failures], 0
    mov dword [power_wake_lock_count], 0
    mov dword [power_wake_lock_expirations], 0
    mov dword [power_idle_entries], 0
    mov dword [power_deep_idle_entries], 0
    mov dword [power_profile_changes], 0
    mov dword [power_wake_lock_deadlines], 0
    mov dword [power_wake_lock_deadlines + 4], 0
    clc
    ret

power_manager_self_test:
    cmp dword [power_api], POWER_API_SIZE
    jne .invalid
    cmp word [power_api + 4], 1
    jne .invalid
    cmp dword [power_api + 12], POWER_SUPPORTED_MASK
    jne .invalid
    cmp dword [power_current_state], POWER_STATE_RUNNING
    jne .invalid
    mov eax, 10
    call power_wake_lock_acquire
    jc .invalid
    cmp dword [power_wake_lock_count], 1
    jne .invalid
    call power_wake_lock_release
    jc .invalid
    cmp dword [power_wake_lock_count], 0
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

; CF=1, falls bereits ein globaler Energieuebergang aktiv ist.
power_manager_request_shutdown:
    cmp dword [power_transition_active], 0
    jne .busy
    mov dword [power_transition_active], 1
    inc dword [power_shutdown_requests]
    mov dword [power_target_state], POWER_STATE_SHUTDOWN
    mov dword [power_transition_phase], POWER_PHASE_REQUEST
    mov esi, message_shutdown_requested
    call serial_write_string
    mov dword [power_transition_phase], POWER_PHASE_FREEZE_USERSPACE
    mov esi, message_shutdown_userspace
    call serial_write_string
    mov dword [power_transition_phase], POWER_PHASE_SYNC_STORAGE
    mov esi, message_shutdown_storage
    call serial_write_string
    mov dword [power_transition_phase], POWER_PHASE_STOP_DEVICES
    mov esi, message_shutdown_devices
    call serial_write_string
    mov dword [power_current_state], POWER_STATE_SHUTDOWN
    clc
    ret
.busy:
    inc dword [power_transition_failures]
    stc
    ret

; EAX=maximale Dauer in 100-Hz-Ticks, Rueckgabe EAX=Slot.
power_wake_lock_acquire:
    test eax, eax
    jz .invalid
    cmp dword [power_wake_lock_count], POWER_WAKE_LOCK_CAPACITY
    jae .invalid
    mov edx, [timer_ticks]
    add edx, eax
    jc .invalid
    xor ecx, ecx
.find:
    cmp dword [power_wake_lock_deadlines + ecx * 4], 0
    je .found
    inc ecx
    cmp ecx, POWER_WAKE_LOCK_CAPACITY
    jb .find
.invalid:
    stc
    ret
.found:
    mov [power_wake_lock_deadlines + ecx * 4], edx
    inc dword [power_wake_lock_count]
    mov eax, ecx
    clc
    ret

; EAX=Slot.
power_wake_lock_release:
    cmp eax, POWER_WAKE_LOCK_CAPACITY
    jae .invalid
    cmp dword [power_wake_lock_deadlines + eax * 4], 0
    je .invalid
    mov dword [power_wake_lock_deadlines + eax * 4], 0
    cmp dword [power_wake_lock_count], 0
    je .invalid
    dec dword [power_wake_lock_count]
    clc
    ret
.invalid:
    stc
    ret

power_wake_lock_expire:
    xor ecx, ecx
.next:
    mov eax, [power_wake_lock_deadlines + ecx * 4]
    test eax, eax
    jz .continue
    mov edx, [timer_ticks]
    sub edx, eax
    js .continue
    mov dword [power_wake_lock_deadlines + ecx * 4], 0
    dec dword [power_wake_lock_count]
    inc dword [power_wake_lock_expirations]
.continue:
    inc ecx
    cmp ecx, POWER_WAKE_LOCK_CAPACITY
    jb .next
    ret

power_cpu_idle:
    call power_wake_lock_expire
    inc dword [power_idle_entries]
    cmp dword [power_wake_lock_count], 0
    jne .shallow
    mov dword [power_idle_state], POWER_IDLE_DEEP
    inc dword [power_deep_idle_entries]
    hlt
    ret
.shallow:
    mov dword [power_idle_state], POWER_IDLE_SHALLOW
    hlt
    ret

; EAX=Profil. Nur zur Laufzeit als unterstuetzt gemeldete Profile annehmen.
power_manager_set_profile:
    cmp eax, POWER_PROFILE_POWERSAVE
    ja .invalid
    cmp dword [power_transition_active], 0
    jne .invalid
    cmp [power_profile], eax
    je .success
    mov [power_profile], eax
    inc dword [power_profile_changes]
.success:
    clc
    ret
.invalid:
    stc
    ret

power_manager_platform_off:
    mov dword [power_transition_phase], POWER_PHASE_PLATFORM_OFF
    mov esi, message_shutdown_platform
    call serial_write_string
    mov ax, 0x2000
    mov dx, 0x0604                  ; QEMU/ACPI Poweroff
    out dx, ax
    mov dx, 0xB004                  ; Bochs/QEMU Fallback
    out dx, ax
    mov dword [power_current_state], POWER_STATE_OFF
    ret

align 4
power_api:
    dd POWER_API_SIZE
    dw 1, 0
    dd POWER_STATE_RUNNING
    dd POWER_SUPPORTED_MASK
    dd power_current_state
    dd power_target_state
    dd power_profile
    dd power_transition_phase
    dd power_shutdown_requests
    dd power_transition_failures
    dd power_manager_request_shutdown
    dd power_manager_platform_off
    dd power_wake_lock_acquire
    dd power_wake_lock_release
    dd power_cpu_idle
    dd power_wake_lock_count
    dd power_manager_set_profile
    dd power_profile_changes
power_current_state:       dd POWER_STATE_RUNNING
power_target_state:        dd POWER_STATE_RUNNING
power_profile:             dd POWER_PROFILE_BALANCED
power_transition_phase:    dd POWER_PHASE_NONE
power_transition_active:   dd 0
power_shutdown_requests:   dd 0
power_transition_failures: dd 0
power_wake_lock_count:      dd 0
power_wake_lock_expirations: dd 0
power_idle_state:           dd POWER_IDLE_SHALLOW
power_idle_entries:         dd 0
power_deep_idle_entries:    dd 0
power_profile_changes:      dd 0
power_wake_lock_deadlines:  times POWER_WAKE_LOCK_CAPACITY dd 0

; Minimaler Network-Core nach NPSPEC-KERNEL-0022. Hardwareprotokolle folgen
; auf dieser objekt- und capability-basierten Grundlage.
NETWORK_API_SIZE          equ 40
NETWORK_INTERFACE_UP      equ 2
NETWORK_INTERFACE_LOOPBACK equ 0
NETWORK_LOOPBACK_MTU      equ 65536
NETWORK_FEATURE_IPV4      equ 0x00000001
NETWORK_FEATURE_IPV6      equ 0x00000002
OBJECT_TYPE_NET_INTERFACE equ 12
OBJECT_TYPE_NET_SOCKET    equ 13
NETWORK_SOCKET_DATAGRAM   equ 1
NETWORK_SOCKET_STREAM     equ 2
NETWORK_AF_IPV4           equ 4
NETWORK_IPV4_LOOPBACK     equ 0x0100007F
NETWORK_BOOTSTRAP_PORT    equ 9000
TCP_STATE_CLOSED          equ 0
TCP_STATE_LISTEN          equ 1
TCP_STATE_SYN_SENT        equ 2
TCP_STATE_SYN_RECEIVED    equ 3
TCP_STATE_ESTABLISHED     equ 4
TCP_STATE_FIN_WAIT_1      equ 5
TCP_STATE_FIN_WAIT_2      equ 6
TCP_STATE_TIME_WAIT       equ 10

network_manager_initialize:
    mov dword [network_namespace_generation], 1
    mov dword [network_interface_count], 0
    mov dword [network_loopback_id], 1
    mov dword [network_loopback_state], NETWORK_INTERFACE_UP
    mov dword [network_received_packets], 0
    mov dword [network_transmitted_packets], 0
    mov dword [network_dropped_packets], 0
    mov dword [network_loopback_queue_count], 0
    mov dword [network_socket_object], 0
    mov dword [network_socket_handle], 0
    mov dword [network_socket_bound], 0
    mov dword [network_socket_port], 0
    mov dword [network_tcp_socket_object], 0
    mov dword [network_tcp_socket_handle], 0
    mov dword [network_tcp_socket_connected], 0
    mov dword [network_tcp_queue_count], 0
    mov dword [network_tcp_queue_length], 0
    mov dword [network_tcp_transmitted_bytes], 0
    mov dword [network_tcp_received_bytes], 0
    mov dword [network_tcp_listener_handle], 0
    mov dword [network_tcp_listener_bound], 0
    mov dword [network_tcp_listener_state], TCP_STATE_CLOSED
    mov dword [network_tcp_listener_backlog], 0
    mov dword [network_tcp_pending_connections], 0
    mov dword [network_tcp_accepted_connections], 0
    mov dword [network_route_count], 2
    mov dword [network_route_lookups], 0
    mov dword [network_route_hits], 0
    mov dword [network_route_last_prefix], 0
    mov dword [network_firewall_decisions], 0
    mov dword [network_firewall_drops], 0
    mov dword [network_ipv6_udp_tests], 0
    mov dword [network_icmpv6_echo_tests], 0
    mov dword [network_tcp_tests], 0
    mov edi, network_loopback_frame
    xor eax, eax
    mov ecx, 64 / 4
    rep stosd
    mov eax, OBJECT_TYPE_NET_INTERFACE
    mov edx, NETWORK_INTERFACE_LOOPBACK
    mov ebx, 1
    call object_create
    jc .invalid
    mov [network_loopback_object], eax
    mov dword [network_interface_count], 1
    clc
    ret
.invalid:
    stc
    ret

network_manager_self_test:
    cmp dword [network_api], NETWORK_API_SIZE
    jne .invalid
    cmp word [network_api + 4], 1
    jne .invalid
    cmp dword [network_interface_count], 1
    jne .invalid
    cmp dword [network_loopback_id], 1
    jne .invalid
    cmp dword [network_loopback_state], NETWORK_INTERFACE_UP
    jne .invalid
    cmp dword [network_loopback_object], 0
    je .invalid
    cmp dword [network_socket_bound], 0
    jne .invalid
    call network_icmp_echo_self_test
    jc .invalid
    call network_ipv6_udp_self_test
    jc .invalid
    call network_icmpv6_echo_self_test
    jc .invalid
    call network_tcp_self_test
    jc .invalid
    mov eax, 0x7F000001             ; 127.0.0.1 in Host-Reihenfolge
    call network_route_lookup_ipv4
    jc .invalid
    cmp eax, 1
    jne .invalid
    cmp dword [network_route_last_prefix], 32
    jne .invalid                    ; Hostroute muss /8 überstimmen
    mov eax, 0x0A000001             ; keine Default-Route vorhanden
    call network_route_lookup_ipv4
    jnc .invalid
    mov eax, 0x7F000001
    mov ebx, 17
    mov ecx, NETWORK_BOOTSTRAP_PORT
    call network_firewall_check_ipv4
    jc .invalid
    mov eax, 0x0A000001             ; externes Ziel muss fail-closed sein
    mov ebx, 17
    mov ecx, NETWORK_BOOTSTRAP_PORT
    call network_firewall_check_ipv4
    jnc .invalid
    clc
    ret
.invalid:
    stc
    ret

; ICMPv4-Echo-Grundpfad: Request und Reply werden vollständig im Kernel
; erzeugt und jeweils vor der Verarbeitung per Internet-Checksum validiert.
network_icmp_echo_self_test:
    mov edi, network_icmp_buffer
    xor eax, eax
    mov ecx, 16 / 4
    rep stosd
    mov byte [network_icmp_buffer + 0], 8  ; Echo Request
    mov byte [network_icmp_buffer + 1], 0
    mov word [network_icmp_buffer + 4], 0x414E
    mov word [network_icmp_buffer + 6], 0x0100
    mov dword [network_icmp_buffer + 8], 0x41564F4E
    mov dword [network_icmp_buffer + 12], 0x534F4156
    mov esi, network_icmp_buffer
    mov ecx, 16
    call network_checksum
    xchg al, ah
    mov [network_icmp_buffer + 2], ax
    call network_icmp_validate_request
    jc .invalid
    mov byte [network_icmp_buffer + 0], 0  ; Echo Reply
    mov word [network_icmp_buffer + 2], 0
    mov esi, network_icmp_buffer
    mov ecx, 16
    call network_checksum
    xchg al, ah
    mov [network_icmp_buffer + 2], ax
    call network_icmp_validate_reply
    jc .invalid
    mov dword [network_icmp_echo_tests], 1
    clc
    ret
.invalid:
    stc
    ret

network_icmp_validate_request:
    cmp byte [network_icmp_buffer], 8
    jne .invalid
    jmp network_icmp_validate_common
.invalid:
    stc
    ret

network_icmp_validate_reply:
    cmp byte [network_icmp_buffer], 0
    jne .invalid
    jmp network_icmp_validate_common
.invalid:
    stc
    ret

network_icmp_validate_common:
    cmp byte [network_icmp_buffer + 1], 0
    jne .invalid
    mov esi, network_icmp_buffer
    mov ecx, 16
    call network_checksum
    test ax, ax
    jnz .invalid
    clc
    ret
.invalid:
    stc
    ret

; IPv6/UDP-Loopback-Selbsttest. IPv6 besitzt keine Headerprüfsumme; die
; UDP-Prüfsumme über den 40-Byte-Pseudoheader ist dagegen verbindlich.
network_ipv6_udp_self_test:
    mov edi, network_ipv6_frame
    xor eax, eax
    mov ecx, 64 / 4
    rep stosd
    mov byte [network_ipv6_frame + 0], 0x60
    mov word [network_ipv6_frame + 4], 0x0C00 ; 12 Byte Payload in Netzreihenfolge
    mov byte [network_ipv6_frame + 6], 17
    mov byte [network_ipv6_frame + 7], 64
    mov byte [network_ipv6_frame + 23], 1     ; Quelle ::1
    mov byte [network_ipv6_frame + 39], 1     ; Ziel ::1
    mov ax, NETWORK_BOOTSTRAP_PORT
    xchg al, ah
    mov [network_ipv6_frame + 40], ax
    mov [network_ipv6_frame + 42], ax
    mov word [network_ipv6_frame + 44], 0x0C00
    mov dword [network_ipv6_frame + 48], 0x41564F4E
    call network_ipv6_checksum_prepare
    mov esi, network_ipv6_checksum_buffer
    mov ecx, 52
    call network_checksum
    test ax, ax
    jnz .checksum_ready
    mov ax, 0xFFFF
.checksum_ready:
    xchg al, ah
    mov [network_ipv6_frame + 46], ax
    call network_ipv6_udp_validate
    jc .invalid
    mov dword [network_ipv6_udp_tests], 1
    clc
    ret
.invalid:
    stc
    ret

network_ipv6_checksum_prepare:
    mov edi, network_ipv6_checksum_buffer
    xor eax, eax
    mov ecx, 64 / 4
    rep stosd
    mov esi, network_ipv6_frame + 8
    mov edi, network_ipv6_checksum_buffer
    mov ecx, 32
    rep movsb
    mov dword [network_ipv6_checksum_buffer + 32], 0x0C000000
    mov byte [network_ipv6_checksum_buffer + 39], 17
    mov esi, network_ipv6_frame + 40
    mov edi, network_ipv6_checksum_buffer + 40
    mov ecx, 12
    rep movsb
    ret

network_ipv6_udp_validate:
    cmp byte [network_ipv6_frame], 0x60
    jne .invalid
    cmp word [network_ipv6_frame + 4], 0x0C00
    jne .invalid
    cmp byte [network_ipv6_frame + 6], 17
    jne .invalid
    cmp byte [network_ipv6_frame + 23], 1
    jne .invalid
    cmp byte [network_ipv6_frame + 39], 1
    jne .invalid
    cmp word [network_ipv6_frame + 46], 0
    je .invalid
    call network_ipv6_checksum_prepare
    mov esi, network_ipv6_checksum_buffer
    mov ecx, 52
    call network_checksum
    test ax, ax
    jnz .invalid
    clc
    ret
.invalid:
    stc
    ret

; ICMPv6 Echo benötigt – anders als ICMPv4 – die IPv6-Quell- und Zieladresse
; im Prüfsummen-Pseudoheader. Request (128) und Reply (129) werden geprüft.
network_icmpv6_echo_self_test:
    mov edi, network_icmpv6_buffer
    xor eax, eax
    mov ecx, 12 / 4
    rep stosd
    mov byte [network_icmpv6_buffer + 0], 128
    mov byte [network_icmpv6_buffer + 1], 0
    mov word [network_icmpv6_buffer + 4], 0x414E
    mov word [network_icmpv6_buffer + 6], 0x0100
    mov dword [network_icmpv6_buffer + 8], 0x36564F4E
    call network_icmpv6_checksum_prepare
    mov esi, network_icmpv6_checksum_buffer
    mov ecx, 52
    call network_checksum
    xchg al, ah
    mov [network_icmpv6_buffer + 2], ax
    mov al, 128
    call network_icmpv6_validate_type
    jc .invalid
    mov byte [network_icmpv6_buffer], 129
    mov word [network_icmpv6_buffer + 2], 0
    call network_icmpv6_checksum_prepare
    mov esi, network_icmpv6_checksum_buffer
    mov ecx, 52
    call network_checksum
    xchg al, ah
    mov [network_icmpv6_buffer + 2], ax
    mov al, 129
    call network_icmpv6_validate_type
    jc .invalid
    mov dword [network_icmpv6_echo_tests], 1
    clc
    ret
.invalid:
    stc
    ret

network_icmpv6_checksum_prepare:
    mov edi, network_icmpv6_checksum_buffer
    xor eax, eax
    mov ecx, 64 / 4
    rep stosd
    mov byte [network_icmpv6_checksum_buffer + 15], 1
    mov byte [network_icmpv6_checksum_buffer + 31], 1
    mov dword [network_icmpv6_checksum_buffer + 32], 0x0C000000
    mov byte [network_icmpv6_checksum_buffer + 39], 58
    mov esi, network_icmpv6_buffer
    mov edi, network_icmpv6_checksum_buffer + 40
    mov ecx, 12
    rep movsb
    ret

network_icmpv6_validate_type:
    cmp [network_icmpv6_buffer], al
    jne .invalid
    cmp byte [network_icmpv6_buffer + 1], 0
    jne .invalid
    cmp word [network_icmpv6_buffer + 2], 0
    je .invalid
    push eax
    call network_icmpv6_checksum_prepare
    mov esi, network_icmpv6_checksum_buffer
    mov ecx, 52
    call network_checksum
    test ax, ax
    pop eax
    jnz .invalid
    clc
    ret
.invalid:
    stc
    ret

; Begrenzter TCP/IPv4-Loopback-Selbsttest. Er validiert den Zustandsautomaten,
; die verbindliche Pseudoheader-Pruefsumme, geordnete Sequenznummern, eine
; kontrollierte Retransmission und den vollstaendigen Verbindungsabbau.
network_tcp_self_test:
    mov dword [network_tcp_state], TCP_STATE_CLOSED
    mov dword [network_tcp_tests], 0
    mov dword [network_tcp_retransmissions], 0
    mov dword [network_tcp_send_sequence], 0x1000
    mov dword [network_tcp_receive_sequence], 0x2000
    mov eax, TCP_STATE_SYN_SENT
    call network_tcp_transition
    jc .invalid
    mov eax, TCP_STATE_SYN_RECEIVED
    call network_tcp_transition
    jc .invalid
    mov eax, TCP_STATE_ESTABLISHED
    call network_tcp_transition
    jc .invalid
    inc dword [network_tcp_tests]

    mov edi, network_tcp_segment
    xor eax, eax
    mov ecx, 24 / 4
    rep stosd
    mov ax, NETWORK_BOOTSTRAP_PORT
    xchg al, ah
    mov [network_tcp_segment + 0], ax
    mov [network_tcp_segment + 2], ax
    mov eax, [network_tcp_send_sequence]
    bswap eax
    mov [network_tcp_segment + 4], eax
    mov eax, [network_tcp_receive_sequence]
    bswap eax
    mov [network_tcp_segment + 8], eax
    mov byte [network_tcp_segment + 12], 0x50 ; 20-Byte TCP-Header
    mov byte [network_tcp_segment + 13], 0x18 ; ACK | PSH
    mov word [network_tcp_segment + 14], 0x0010
    mov dword [network_tcp_segment + 20], 0x41564F4E
    call network_tcp_checksum_prepare
    mov esi, network_tcp_checksum_buffer
    mov ecx, 36
    call network_checksum
    xchg al, ah
    mov [network_tcp_segment + 16], ax
    call network_tcp_validate_segment
    jc .invalid
    add dword [network_tcp_send_sequence], 4
    inc dword [network_tcp_tests]

    ; Der erste ACK bleibt absichtlich aus. Genau eine begrenzte erneute
    ; Uebertragung wird verbucht; die Sequenznummer darf sich nicht aendern.
    mov eax, [network_tcp_send_sequence]
    inc dword [network_tcp_retransmissions]
    cmp eax, [network_tcp_send_sequence]
    jne .invalid
    cmp dword [network_tcp_retransmissions], 1
    jne .invalid
    inc dword [network_tcp_tests]

    mov eax, TCP_STATE_FIN_WAIT_1
    call network_tcp_transition
    jc .invalid
    mov eax, TCP_STATE_FIN_WAIT_2
    call network_tcp_transition
    jc .invalid
    mov eax, TCP_STATE_TIME_WAIT
    call network_tcp_transition
    jc .invalid
    mov eax, TCP_STATE_CLOSED
    call network_tcp_transition
    jc .invalid
    inc dword [network_tcp_tests]
    clc
    ret
.invalid:
    stc
    ret

network_tcp_checksum_prepare:
    mov edi, network_tcp_checksum_buffer
    xor eax, eax
    mov ecx, 40 / 4
    rep stosd
    mov dword [network_tcp_checksum_buffer + 0], NETWORK_IPV4_LOOPBACK
    mov dword [network_tcp_checksum_buffer + 4], NETWORK_IPV4_LOOPBACK
    mov byte [network_tcp_checksum_buffer + 9], 6
    mov word [network_tcp_checksum_buffer + 10], 0x1800
    mov esi, network_tcp_segment
    mov edi, network_tcp_checksum_buffer + 12
    mov ecx, 24
    rep movsb
    ret

network_tcp_validate_segment:
    cmp dword [network_tcp_state], TCP_STATE_ESTABLISHED
    jne .invalid
    cmp byte [network_tcp_segment + 12], 0x50
    jne .invalid
    cmp byte [network_tcp_segment + 13], 0x18
    jne .invalid
    mov eax, [network_tcp_segment + 4]
    bswap eax
    cmp eax, [network_tcp_send_sequence]
    jne .invalid
    call network_tcp_checksum_prepare
    mov esi, network_tcp_checksum_buffer
    mov ecx, 36
    call network_checksum
    test ax, ax
    jnz .invalid
    clc
    ret
.invalid:
    stc
    ret

network_tcp_transition:
    mov edx, [network_tcp_state]
    cmp edx, TCP_STATE_CLOSED
    jne .from_syn_sent
    cmp eax, TCP_STATE_SYN_SENT
    jne .invalid
    jmp .commit
.from_syn_sent:
    cmp edx, TCP_STATE_SYN_SENT
    jne .from_syn_received
    cmp eax, TCP_STATE_SYN_RECEIVED
    jne .invalid
    jmp .commit
.from_syn_received:
    cmp edx, TCP_STATE_SYN_RECEIVED
    jne .from_established
    cmp eax, TCP_STATE_ESTABLISHED
    jne .invalid
    jmp .commit
.from_established:
    cmp edx, TCP_STATE_ESTABLISHED
    jne .from_fin_wait_1
    cmp eax, TCP_STATE_FIN_WAIT_1
    jne .invalid
    jmp .commit
.from_fin_wait_1:
    cmp edx, TCP_STATE_FIN_WAIT_1
    jne .from_fin_wait_2
    cmp eax, TCP_STATE_FIN_WAIT_2
    jne .invalid
    jmp .commit
.from_fin_wait_2:
    cmp edx, TCP_STATE_FIN_WAIT_2
    jne .from_time_wait
    cmp eax, TCP_STATE_TIME_WAIT
    jne .invalid
    jmp .commit
.from_time_wait:
    cmp edx, TCP_STATE_TIME_WAIT
    jne .invalid
    cmp eax, TCP_STATE_CLOSED
    jne .invalid
.commit:
    mov [network_tcp_state], eax
    clc
    ret
.invalid:
    stc
    ret

; EAX=IPv4-Adresse in Host-Reihenfolge, EAX=Interface-ID. Die Tabelle ist
; fest begrenzt; längere Präfixe gewinnen unabhängig von der Eintragsreihenfolge.
network_route_lookup_ipv4:
    mov [network_route_lookup_address], eax
    inc dword [network_route_lookups]
    mov dword [network_route_last_prefix], 0
    mov dword [network_route_selected_interface], 0
    xor ecx, ecx
.next:
    cmp ecx, [network_route_count]
    jae .complete
    mov edi, ecx
    shl edi, 4
    add edi, network_route_table
    mov eax, [network_route_lookup_address]
    and eax, [edi + 4]
    cmp eax, [edi + 0]
    jne .continue
    mov eax, [edi + 8]
    cmp eax, [network_route_last_prefix]
    jb .continue
    mov [network_route_last_prefix], eax
    mov eax, [edi + 12]
    mov [network_route_selected_interface], eax
.continue:
    inc ecx
    jmp .next
.complete:
    mov eax, [network_route_selected_interface]
    test eax, eax
    jz .unreachable
    inc dword [network_route_hits]
    clc
    ret
.unreachable:
    stc
    ret

; Statische Bootstrap-Firewall. Bis ein autorisierter Policy-Dienst Regeln
; installiert, ist ausschließlich UDP/TCP Port 9000 in 127.0.0.0/8 erlaubt.
network_firewall_check_ipv4:
    inc dword [network_firewall_decisions]
    cmp ebx, 17
    je .protocol_ok
    cmp ebx, 6
    jne .drop
.protocol_ok:
    cmp ecx, NETWORK_BOOTSTRAP_PORT
    jne .drop
    and eax, 0xFF000000
    cmp eax, 0x7F000000
    jne .drop
    clc
    ret
.drop:
    inc dword [network_firewall_drops]
    stc
    ret

; Erstellt ein IPv4/UDP-Datagramm für 127.0.0.1. Der öffentliche Request
; enthält nur Nutzdaten; sämtliche Protokollfelder entstehen im Kernel.
network_loopback_encapsulate:
    mov ecx, [syscall_network_packet + 12]
    test ecx, ecx
    jz .invalid
    cmp ecx, 16
    ja .invalid
    mov [network_loopback_payload_length], ecx
    mov edi, network_loopback_frame
    xor eax, eax
    push ecx
    mov ecx, 64 / 4
    rep stosd
    pop ecx
    mov byte [network_loopback_frame + 0], 0x45
    mov byte [network_loopback_frame + 1], 0
    mov eax, ecx
    add eax, 28
    xchg al, ah
    mov [network_loopback_frame + 2], ax
    inc word [network_ipv4_identification]
    mov ax, [network_ipv4_identification]
    xchg al, ah
    mov [network_loopback_frame + 4], ax
    mov word [network_loopback_frame + 6], 0
    mov byte [network_loopback_frame + 8], 64
    mov byte [network_loopback_frame + 9], 17
    mov dword [network_loopback_frame + 12], NETWORK_IPV4_LOOPBACK
    mov dword [network_loopback_frame + 16], NETWORK_IPV4_LOOPBACK
    mov eax, [network_socket_port]
    xchg al, ah
    mov [network_loopback_frame + 20], ax
    mov [network_loopback_frame + 22], ax
    mov eax, ecx
    add eax, 8
    xchg al, ah
    mov [network_loopback_frame + 24], ax
    mov esi, syscall_network_packet + 20
    mov edi, network_loopback_frame + 28
    rep movsb
    mov esi, network_loopback_frame
    mov ecx, 20
    call network_checksum
    xchg al, ah
    mov [network_loopback_frame + 10], ax
    call network_udp_checksum_prepare
    mov eax, [network_loopback_payload_length]
    add eax, 20
    mov ecx, eax
    mov esi, network_checksum_buffer
    call network_checksum
    test ax, ax
    jnz .udp_checksum_ready
    mov ax, 0xFFFF
.udp_checksum_ready:
    xchg al, ah
    mov [network_loopback_frame + 26], ax
    clc
    ret
.invalid:
    stc
    ret

network_udp_checksum_prepare:
    mov edi, network_checksum_buffer
    xor eax, eax
    mov ecx, 40 / 4
    rep stosd
    mov dword [network_checksum_buffer + 0], NETWORK_IPV4_LOOPBACK
    mov dword [network_checksum_buffer + 4], NETWORK_IPV4_LOOPBACK
    mov byte [network_checksum_buffer + 8], 0
    mov byte [network_checksum_buffer + 9], 17
    mov ax, [network_loopback_frame + 24]
    mov [network_checksum_buffer + 10], ax
    mov esi, network_loopback_frame + 20
    mov edi, network_checksum_buffer + 12
    mov ecx, [network_loopback_payload_length]
    add ecx, 8
    rep movsb
    ret

network_loopback_validate:
    cmp byte [network_loopback_frame + 0], 0x45
    jne .invalid
    cmp byte [network_loopback_frame + 9], 17
    jne .invalid
    cmp dword [network_loopback_frame + 16], NETWORK_IPV4_LOOPBACK
    jne .invalid
    mov esi, network_loopback_frame
    mov ecx, 20
    call network_checksum
    test ax, ax
    jnz .invalid
    call network_udp_checksum_prepare
    mov eax, [network_loopback_payload_length]
    add eax, 20
    mov ecx, eax
    mov esi, network_checksum_buffer
    call network_checksum
    test ax, ax
    jnz .invalid
    clc
    ret
.invalid:
    stc
    ret

; Internet-Checksum über ECX Bytes in Netzwerkreihenfolge, Ergebnis in AX.
network_checksum:
    xor eax, eax
.pair:
    cmp ecx, 2
    jb .odd
    movzx edx, byte [esi]
    shl edx, 8
    mov dl, [esi + 1]
    add eax, edx
    add esi, 2
    sub ecx, 2
    jmp .pair
.odd:
    test ecx, ecx
    jz .fold
    movzx edx, byte [esi]
    shl edx, 8
    add eax, edx
.fold:
    mov edx, eax
    shr edx, 16
    and eax, 0xFFFF
    add eax, edx
    mov edx, eax
    shr edx, 16
    add ax, dx
    not ax
    ret

align 4
network_api:
    dd NETWORK_API_SIZE
    dw 1, 0
    dd NETWORK_FEATURE_IPV4 | NETWORK_FEATURE_IPV6
    dd network_namespace_generation
    dd network_interface_count
    dd network_loopback_id
    dd network_loopback_state
    dd network_received_packets
    dd network_transmitted_packets
    dd network_dropped_packets
network_namespace_generation: dd 1
network_interface_count:      dd 0
network_loopback_id:          dd 1
network_loopback_state:       dd NETWORK_INTERFACE_UP
network_loopback_object:      dd 0
network_received_packets:     dd 0
network_transmitted_packets:  dd 0
network_dropped_packets:      dd 0
network_socket_object:        dd 0
network_socket_handle:        dd 0
network_socket_bound:         dd 0
network_socket_port:          dd 0
network_tcp_socket_object:    dd 0
network_tcp_socket_handle:    dd 0
network_tcp_socket_connected: dd 0
network_tcp_queue_count:      dd 0
network_tcp_queue_length:     dd 0
network_tcp_transmitted_bytes: dd 0
network_tcp_received_bytes:   dd 0
network_tcp_queue_payload:    times 16 db 0
network_tcp_listener_handle:  dd 0
network_tcp_listener_bound:   dd 0
network_tcp_listener_state:   dd 0
network_tcp_listener_backlog: dd 0
network_tcp_pending_connections: dd 0
network_tcp_accepted_connections: dd 0
network_loopback_queue_count: dd 0
network_loopback_payload_length: dd 0
network_ipv4_identification:  dw 0
align 4
network_loopback_frame:       times 64 db 0
network_checksum_buffer:      times 40 db 0
network_icmp_buffer:          times 16 db 0
network_icmp_echo_tests:      dd 0
network_route_count:          dd 0
network_route_lookups:        dd 0
network_route_hits:           dd 0
network_route_last_prefix:    dd 0
network_route_lookup_address: dd 0
network_route_selected_interface: dd 0
network_firewall_decisions:    dd 0
network_firewall_drops:        dd 0
network_ipv6_udp_tests:        dd 0
network_icmpv6_echo_tests:     dd 0
network_tcp_state:             dd 0
network_tcp_tests:             dd 0
network_tcp_retransmissions:   dd 0
network_tcp_send_sequence:     dd 0
network_tcp_receive_sequence:  dd 0
align 4
network_route_table:
    dd 0x7F000000, 0xFF000000, 8, 1
    dd 0x7F000001, 0xFFFFFFFF, 32, 1
align 4
network_ipv6_frame:            times 64 db 0
network_ipv6_checksum_buffer:  times 64 db 0
network_icmpv6_buffer:         times 12 db 0
network_icmpv6_checksum_buffer: times 64 db 0
network_tcp_segment:           times 24 db 0
network_tcp_checksum_buffer:   times 40 db 0

; ---------------------------------------------------------------------------
; Panic-sicheres Crash-Dump-System (NPSPEC-KERNEL-0024)
; ---------------------------------------------------------------------------
CRASH_DUMP_BUFFER_SIZE     equ 1024
CRASH_DUMP_HEADER_SIZE     equ 128
CRASH_DUMP_SECTION_SIZE    equ 32
CRASH_DUMP_SECTION_COUNT   equ 3
CRASH_DUMP_TOTAL_SIZE      equ 416
CRASH_DUMP_STATUS_EMPTY    equ 0
CRASH_DUMP_STATUS_WRITING  equ 1
CRASH_DUMP_STATUS_COMPLETE equ 2
CRASH_DUMP_STATUS_PARTIAL  equ 3
CRASH_DUMP_CLASS_MINIMAL   equ 2
CRASH_DUMP_SECTION_PANIC   equ 0
CRASH_DUMP_SECTION_CPU     equ 1
CRASH_DUMP_SECTION_LOGS    equ 8
CRASH_DUMP_COMMIT_MARKER   equ 0x504D5544 ; "DUMP"

; Der Puffer wird beim Boot reserviert und niemals dem PMM/Heap übergeben.
crash_dump_initialize:
    mov edi, crash_dump_buffer
    xor eax, eax
    mov ecx, CRASH_DUMP_BUFFER_SIZE / 4
    rep stosd
    mov dword [crash_dump_active], 0
    mov dword [crash_dump_recursions], 0
    mov dword [crash_dump_completed], 0
    mov dword [crash_dump_partial], 0
    clc
    ret

; Erzeugt ausschließlich aus statischem Speicher einen Minimal-Dump. Der
; Status wird zuerst WRITING und erst nach Inhalt und Integrität COMPLETE.
crash_dump_capture_minimal:
    pushad
    cmp dword [crash_dump_active], 0
    jne .recursive
    mov dword [crash_dump_active], 1
    mov edi, crash_dump_buffer
    xor eax, eax
    mov ecx, CRASH_DUMP_BUFFER_SIZE / 4
    rep stosd
    mov dword [crash_dump_buffer + 0], 0x4443564E ; "NVCD"
    mov dword [crash_dump_buffer + 4], 0x31504D55 ; "UMP1"
    mov word  [crash_dump_buffer + 8], 1
    mov word  [crash_dump_buffer + 10], 0
    mov dword [crash_dump_buffer + 12], CRASH_DUMP_HEADER_SIZE
    mov dword [crash_dump_buffer + 16], CRASH_DUMP_CLASS_MINIMAL
    mov dword [crash_dump_buffer + 20], CRASH_DUMP_STATUS_WRITING
    mov dword [crash_dump_buffer + 24], 0x00000001 ; Privacy/minimal
    mov eax, [timer_ticks]
    mov [crash_dump_buffer + 28], eax
    mov dword [crash_dump_buffer + 32], CRASH_DUMP_TOTAL_SIZE
    mov dword [crash_dump_buffer + 36], CRASH_DUMP_SECTION_COUNT

    ; Versionierte, nicht überlappende Sektionsdeskriptoren.
    mov dword [crash_dump_buffer + 128], CRASH_DUMP_SECTION_PANIC
    mov dword [crash_dump_buffer + 132], 1
    mov dword [crash_dump_buffer + 136], 256
    mov dword [crash_dump_buffer + 140], PANIC_REPORT_SIZE
    mov dword [crash_dump_buffer + 160], CRASH_DUMP_SECTION_CPU
    mov dword [crash_dump_buffer + 164], 1
    mov dword [crash_dump_buffer + 168], 304
    mov dword [crash_dump_buffer + 172], 64
    mov dword [crash_dump_buffer + 192], CRASH_DUMP_SECTION_LOGS
    mov dword [crash_dump_buffer + 196], 1
    mov dword [crash_dump_buffer + 200], 368
    mov dword [crash_dump_buffer + 204], LOG_RECORD_SIZE

    mov esi, panic_report
    mov edi, crash_dump_buffer + 256
    mov ecx, PANIC_REPORT_SIZE / 4
    rep movsd
    ; Minimaler CPU-Kontext ohne FPU-, Secret- oder Userspace-Seiten.
    mov eax, cr0
    mov [crash_dump_buffer + 304], eax
    mov eax, cr2
    mov [crash_dump_buffer + 308], eax
    mov eax, cr3
    mov [crash_dump_buffer + 312], eax
    mov eax, cr4
    mov [crash_dump_buffer + 316], eax
    mov eax, [panic_report + 16]
    mov [crash_dump_buffer + 320], eax
    mov eax, [panic_report + 20]
    mov [crash_dump_buffer + 324], eax
    mov eax, [boot_phase_current]
    mov [crash_dump_buffer + 328], eax
    mov eax, [boot_phase_last_success]
    mov [crash_dump_buffer + 332], eax
    mov esi, logging_critical_record
    mov edi, crash_dump_buffer + 368
    mov ecx, LOG_RECORD_SIZE / 4
    rep movsd

    ; Bounded additive integrity value over all publizierten Nutzdaten.
    mov esi, crash_dump_buffer + CRASH_DUMP_HEADER_SIZE
    mov ecx, CRASH_DUMP_TOTAL_SIZE - CRASH_DUMP_HEADER_SIZE
    xor eax, eax
.hash:
    movzx edx, byte [esi]
    rol eax, 5
    xor eax, edx
    inc esi
    loop .hash
    mov [crash_dump_buffer + 60], eax
    mov dword [crash_dump_buffer + 64], CRASH_DUMP_COMMIT_MARKER
    mov dword [crash_dump_buffer + 20], CRASH_DUMP_STATUS_COMPLETE
    inc dword [crash_dump_completed]
    mov dword [crash_dump_active], 0
    popad
    clc
    ret
.recursive:
    inc dword [crash_dump_recursions]
    inc dword [crash_dump_partial]
    mov dword [crash_dump_buffer + 20], CRASH_DUMP_STATUS_PARTIAL
    mov dword [crash_dump_buffer + 68], 1
    mov dword [crash_dump_buffer + 64], CRASH_DUMP_COMMIT_MARKER
    popad
    stc
    ret

crash_dump_validate:
    cmp dword [crash_dump_buffer + 0], 0x4443564E
    jne .invalid
    cmp dword [crash_dump_buffer + 4], 0x31504D55
    jne .invalid
    cmp dword [crash_dump_buffer + 12], CRASH_DUMP_HEADER_SIZE
    jne .invalid
    cmp dword [crash_dump_buffer + 20], CRASH_DUMP_STATUS_COMPLETE
    jne .invalid
    cmp dword [crash_dump_buffer + 32], CRASH_DUMP_TOTAL_SIZE
    jne .invalid
    cmp dword [crash_dump_buffer + 36], CRASH_DUMP_SECTION_COUNT
    jne .invalid
    cmp dword [crash_dump_buffer + 64], CRASH_DUMP_COMMIT_MARKER
    jne .invalid
    mov esi, crash_dump_buffer + CRASH_DUMP_HEADER_SIZE
    mov ecx, CRASH_DUMP_TOTAL_SIZE - CRASH_DUMP_HEADER_SIZE
    xor eax, eax
.hash:
    movzx edx, byte [esi]
    rol eax, 5
    xor eax, edx
    inc esi
    loop .hash
    cmp eax, [crash_dump_buffer + 60]
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

crash_dump_self_test:
    call crash_dump_capture_minimal
    jc .invalid
    call crash_dump_validate
    jc .invalid
    cmp dword [crash_dump_completed], 1
    jne .invalid
    ; Der Selbsttest gilt nicht als echter Crash: Slot danach wieder freigeben.
    call crash_dump_initialize
    clc
    ret
.invalid:
    stc
    ret

align 16
crash_dump_active:       dd 0
crash_dump_recursions:   dd 0
crash_dump_completed:    dd 0
crash_dump_partial:      dd 0
crash_dump_buffer:       times CRASH_DUMP_BUFFER_SIZE db 0

; Strukturierter Panic-Reporter (ADR-2014)
PANIC_API_SIZE    equ 32
PANIC_REPORT_SIZE equ 48

BOOT_PHASE_NONE            equ 0xFFFFFFFF
BOOT_PHASE_KERNEL_ENTRY    equ 0
BOOT_PHASE_HANDOFF         equ 1
BOOT_PHASE_EARLY_ARCH      equ 2
BOOT_PHASE_EARLY_MEMORY    equ 3
BOOT_PHASE_VIRTUAL_MEMORY  equ 4
BOOT_PHASE_KERNEL_CORE     equ 5
BOOT_PHASE_INTERRUPTS_TIME equ 6
BOOT_PHASE_SCHEDULER_SMP   equ 7
BOOT_PHASE_DEVICE_DISCOVERY equ 8
BOOT_PHASE_ROOT_FILESYSTEM  equ 9
BOOT_PHASE_USERSPACE        equ 10
BOOT_PHASE_OPERATIONAL     equ 11
BOOT_SEQUENCE_API_SIZE     equ 32
BOOT_LOG_API_SIZE          equ 32
BOOT_EVENT_SIZE            equ 24
BOOT_LOG_CAPACITY          equ 12
BOOT_COMPONENT_KERNEL      equ 0x4B45524E

panic_manager_initialize:
    mov edi, panic_report
    xor eax, eax
    mov ecx, PANIC_REPORT_SIZE / 4
    rep stosd
    mov dword [panic_report], PANIC_REPORT_SIZE
    mov dword [panic_report + 4], 1
    clc
    ret

panic_manager_self_test:
    cmp dword [panic_api], PANIC_API_SIZE
    jne .invalid
    cmp word [panic_api + 4], 1
    jne .invalid
    cmp dword [panic_report], PANIC_REPORT_SIZE
    jne .invalid
    clc
    ret
.invalid:
    stc
    ret

; EAX=Fehlercode, EDX=Subsystem, ESI=erklärender Text.
kernel_panic:
    cli
    mov [panic_report + 8], eax
    mov [panic_report + 12], edx
    mov ecx, [esp]
    mov [panic_report + 16], ecx
    mov [panic_report + 20], esp
    mov ecx, cr2
    mov [panic_report + 24], ecx
    mov ecx, [timer_ticks]
    mov [panic_report + 28], ecx
    mov ecx, [boot_phase_current]
    mov [panic_report + 32], ecx
    mov ecx, [boot_phase_last_success]
    mov [panic_report + 36], ecx
    push ebx
    mov eax, 1
    cpuid
    shr ebx, 24
    mov [panic_report + 40], ebx
    pop ebx
    mov ecx, [kernel_context + CONTEXT_SECURITY_STATE]
    mov [panic_report + 44], ecx
    call crash_dump_capture_minimal
    push esi
    call draw_kernel_panic_screen
    pop esi
    push esi
    mov esi, message_panic_begin
    call serial_write_string
    mov eax, [panic_report + 8]
    call serial_write_hex32
    mov esi, message_panic_subsystem
    call serial_write_string
    mov eax, [panic_report + 12]
    call serial_write_hex32
    mov esi, message_panic_eip
    call serial_write_string
    mov eax, [panic_report + 16]
    call serial_write_hex32
    mov esi, message_panic_cr2
    call serial_write_string
    mov eax, [panic_report + 24]
    call serial_write_hex32
    mov esi, message_panic_phase
    call serial_write_string
    mov eax, [panic_report + 32]
    call serial_write_hex32
    mov esi, message_panic_last_phase
    call serial_write_string
    mov eax, [panic_report + 36]
    call serial_write_hex32
    mov esi, message_panic_cpu
    call serial_write_string
    mov eax, [panic_report + 40]
    call serial_write_hex32
    mov esi, message_panic_security
    call serial_write_string
    mov eax, [panic_report + 44]
    call serial_write_hex32
    mov esi, message_newline
    call serial_write_string
    pop esi
    call serial_write_string
    mov esi, message_panic_end
    call serial_write_string
    jmp kernel_halt

align 4
panic_api:
    dd PANIC_API_SIZE
    dw 1, 1
    dd PANIC_REPORT_SIZE
    dd kernel_panic
    dd panic_report
    dd panic_manager_self_test
    dd 0
    dd 0
panic_report:
    times PANIC_REPORT_SIZE db 0

; Frühe, lesbare Bootsequenz-ABI. Sie hält die aktuelle und die letzte
; erfolgreich abgeschlossene NPSPEC-KERNEL-0002-Phase ohne Heapabhängigkeit.
align 4
boot_sequence_api:
    dd BOOT_SEQUENCE_API_SIZE
    dw 1, 0
    dd boot_phase_current
    dd boot_phase_last_success
    dd BOOT_PHASE_OPERATIONAL + 1
    dd 0
    dd 0
    dd 0
boot_phase_current:      dd BOOT_PHASE_NONE
boot_phase_last_success: dd BOOT_PHASE_NONE

; Heapfreies Frühstartprotokoll; bewahrt sämtliche Aufruferregister.
boot_phase_log:
    pushad
    call boot_event_record_current
    mov esi, message_boot_phase
    call serial_write_string
    mov eax, [boot_phase_current]
    call serial_write_hex32
    mov esi, message_boot_last_phase
    call serial_write_string
    mov eax, [boot_phase_last_success]
    call serial_write_hex32
    mov esi, message_newline
    call serial_write_string
    popad
    ret

; Schreibt genau ein NPSPEC-KERNEL-0002-Bootereignis in den statischen Ring.
; Ein voller Ring verwirft weitere Ereignisse deterministisch.
boot_event_record_current:
    mov ecx, [boot_event_count]
    cmp ecx, BOOT_LOG_CAPACITY
    jae .done
    imul edi, ecx, BOOT_EVENT_SIZE
    add edi, boot_events
    mov eax, [timer_ticks]
    mov [edi + 0], eax
    mov dword [edi + 4], 0
    mov eax, [boot_phase_current]
    mov [edi + 8], eax
    mov dword [edi + 12], BOOT_COMPONENT_KERNEL
    mov dword [edi + 16], 0
    mov dword [edi + 20], 0
    inc dword [boot_event_count]
.done:
    ret

align 4
boot_log_api:
    dd BOOT_LOG_API_SIZE
    dw 1, 0
    dd BOOT_LOG_CAPACITY
    dd BOOT_EVENT_SIZE
    dd boot_event_count
    dd boot_events
    dd 0
    dd 0
boot_event_count: dd 0
align 8
boot_events:
    times BOOT_LOG_CAPACITY * BOOT_EVENT_SIZE db 0

kernel_halt:
    cli
.loop:
    hlt
    jmp .loop

; ---------------------------------------------------------------------------
; COM1
; ---------------------------------------------------------------------------

serial_initialize:
    mov dx, COM1_BASE + 1
    xor al, al
    out dx, al
    mov dx, COM1_BASE + 3
    mov al, 0x80
    out dx, al
    mov dx, COM1_BASE
    mov al, 1
    out dx, al
    mov dx, COM1_BASE + 1
    xor al, al
    out dx, al
    mov dx, COM1_BASE + 3
    mov al, 0x03
    out dx, al
    mov dx, COM1_BASE + 2
    mov al, 0xC7
    out dx, al
    mov dx, COM1_BASE + 4
    mov al, 0x0B
    out dx, al
    ret

serial_write_string:
    lodsb
    test al, al
    jz .done
    call serial_write_byte
    jmp serial_write_string
.done:
    ret

serial_write_byte:
    push eax
    mov ah, al
.wait:
    mov dx, COM1_BASE + 5
    in al, dx
    test al, 0x20
    jz .wait
    mov dx, COM1_BASE
    mov al, ah
    out dx, al
    pop eax
    ret

serial_write_hex32:
    pushad
    mov ebx, eax
    mov ecx, 8
.digit:
    rol ebx, 4
    mov eax, ebx
    and eax, 0x0F
    cmp al, 10
    jb .number
    add al, 'A' - 10
    jmp .write
.number:
    add al, '0'
.write:
    call serial_write_byte
    loop .digit
    popad
    ret

draw_boot_screen:
    jmp draw_kernel_log_screen

draw_kernel_log_screen:
    mov eax, NOVA_COLOR_BLACK
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    call fill_rectangle

    mov eax, NOVA_COLOR_BLUE
    mov ebx, 102
    xor ecx, ecx
    mov edx, 628
    mov esi, 13
    call fill_rounded_rectangle

    mov dword [logo_x], 32
    mov dword [logo_y], 38
    mov dword [logo_color], NOVA_COLOR_BLUE
    mov byte [logo_compact], 1
    mov byte [logo_mirror], 1
    call draw_nova_logo
    mov esi, text_brand_compact
    mov ebx, 32
    mov ecx, 156
    mov edx, NOVA_COLOR_BLUE
    mov ebp, 3
    call draw_text

    mov esi, text_logsystem
    mov ebx, 250
    mov ecx, 52
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov eax, NOVA_COLOR_WHITE
    mov ebx, 212
    mov ecx, 53
    mov edx, 4
    mov esi, 4
    call fill_rectangle
    mov ecx, 61
    call fill_rectangle
    mov ecx, 69
    call fill_rectangle
    mov ebx, 220
    mov ecx, 54
    mov edx, 12
    mov esi, 2
    call fill_rectangle
    mov ecx, 62
    call fill_rectangle
    mov ecx, 70
    call fill_rectangle

    mov eax, NOVA_COLOR_BLUE
    mov ebx, 184
    mov ecx, 92
    mov edx, 583
    mov esi, 480
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_LOG_PANEL
    mov ebx, 185
    mov ecx, 93
    mov edx, 581
    mov esi, 478
    call fill_rounded_rectangle

    mov esi, text_kernel_log
    mov ebx, 205
    mov ecx, 112
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 1
    call draw_text

    mov esi, text_escape
    mov ebx, 73
    mov ecx, 456
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov eax, NOVA_COLOR_BUTTON
    mov ebx, 54
    mov ecx, 487
    mov edx, 78
    mov esi, 78
    call fill_rounded_rectangle
    call draw_power_icon_aa
    ret

; Geglättetes Power-Symbol: Kreisring mit oberer Öffnung und runder Taste.
; Die drei Deckungsstufen werden mit dem vorhandenen Hintergrund gemischt.
draw_power_icon_aa:
    pushad
    mov dword [text_color], NOVA_COLOR_WHITE
    xor ebp, ebp
.row:
    cmp ebp, 36
    jae .done
    xor ecx, ecx
.column:
    cmp ecx, 36
    jae .next_row
    xor eax, eax

    ; Abgerundeter senkrechter Schalter (x 16..20, y 1..18).
    cmp ebp, 1
    jb .ring
    cmp ebp, 18
    ja .ring
    cmp ecx, 16
    jb .stem_edge
    cmp ecx, 20
    ja .stem_edge
    mov eax, 3
    jmp .paint
.stem_edge:
    cmp ecx, 15
    je .stem_alpha
    cmp ecx, 21
    jne .ring
.stem_alpha:
    mov eax, 1
    jmp .paint

.ring:
    ; Öffnung des Rings im oberen Bereich.
    cmp ebp, 10
    jae .distance
    cmp ecx, 12
    jb .distance
    cmp ecx, 24
    jbe .skip
.distance:
    mov eax, ecx
    sub eax, 18
    imul eax, eax
    mov edx, ebp
    sub edx, 19
    imul edx, edx
    add eax, edx
    cmp eax, 121
    jb .skip
    cmp eax, 255
    ja .skip
    cmp eax, 132
    jb .soft
    cmp eax, 240
    ja .soft
    cmp eax, 150
    jb .medium
    cmp eax, 218
    ja .medium
    mov eax, 3
    jmp .paint
.medium:
    mov eax, 2
    jmp .paint
.soft:
    mov eax, 1
.paint:
    mov edi, ebp
    add edi, 507
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov edx, ecx
    add edx, 75
    shl edx, 2
    add edi, edx
    call blend_text_pixel
.skip:
    inc ecx
    jmp .column
.next_row:
    inc ebp
    jmp .row
.done:
    popad
    ret

draw_boot_screen_legacy:
    ; Vollständiger Hintergrund
    mov eax, NOVA_COLOR_BACKGROUND
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    call fill_rectangle

    ; Cyanfarbene Markenlinie
    mov eax, NOVA_COLOR_CYAN
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, 6
    call fill_rectangle

    ; Nova-Marke
    mov esi, text_brand
    mov ebx, 64
    mov ecx, 42
    mov edx, NOVA_COLOR_CYAN
    mov ebp, 2
    call draw_text

    call draw_nova_logo

    ; Hauptstatus
    mov esi, text_ready
    mov ebx, 64
    mov ecx, 92
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 4
    call draw_text

    mov esi, text_subtitle
    mov ebx, 66
    mov ecx, 136
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 2
    call draw_text

    ; Statuskarte
    mov eax, NOVA_COLOR_PANEL
    mov ebx, 64
    mov ecx, 184
    mov edx, 672
    mov esi, 250
    call fill_rectangle

    mov eax, NOVA_COLOR_PANEL_EDGE
    mov ebx, 64
    mov ecx, 184
    mov edx, 4
    mov esi, 250
    call fill_rectangle

    mov esi, text_status_heading
    mov ebx, 92
    mov ecx, 210
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 1
    call draw_text

    mov esi, text_check_kernel
    mov ebx, 92
    mov ecx, 246
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text

    mov esi, text_check_handoff
    mov ebx, 92
    mov ecx, 286
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text

    mov esi, text_check_memory
    mov ebx, 92
    mov ecx, 326
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text

    mov esi, text_check_graphics
    mov ebx, 92
    mov ecx, 366
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text

    mov esi, text_check_interrupts
    mov ebx, 92
    mov ecx, 406
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text

    ; Grüne Zustandsmarkierungen
    mov eax, NOVA_COLOR_SUCCESS
    mov ebx, 650
    mov ecx, 246
    mov edx, 18
    mov esi, 18
    call fill_rectangle
    mov ecx, 286
    call fill_rectangle
    mov ecx, 326
    call fill_rectangle
    mov ecx, 366
    call fill_rectangle
    mov ecx, 406
    call fill_rectangle

    ; Technische und transparente Sicherheitsinformation
    mov esi, text_technical
    mov ebx, 66
    mov ecx, 470
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 1
    call draw_text

    ret

; EAX=Farbe, EBX=x, ECX=y, EDX=Breite, ESI=Höhe
draw_kernel_panic_screen:
    cmp dword [kernel_context + CONTEXT_FRAMEBUFFER], 0
    je .done
    cmp dword [kernel_context + CONTEXT_BPP], 32
    jne .done
    cmp dword [kernel_context + CONTEXT_WIDTH], 640
    jb .done
    cmp dword [kernel_context + CONTEXT_HEIGHT], 480
    jb .done
    mov eax, [panic_report + 8]
    call format_panic_hex
    jmp draw_kernel_error_reference
    mov eax, NOVA_COLOR_BACKGROUND
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    call fill_rectangle
    mov eax, NOVA_COLOR_CYAN
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, 6
    call fill_rectangle
    mov esi, text_brand
    mov ebx, 64
    mov ecx, 42
    mov edx, NOVA_COLOR_CYAN
    mov ebp, 2
    call draw_text
    call draw_nova_logo
    call draw_panic_smile
    mov esi, text_panic_title
    mov ebx, 66
    mov ecx, 170
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov esi, text_panic_explain1
    mov ebx, 66
    mov ecx, 214
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 2
    call draw_text
    mov esi, text_panic_explain2
    mov ebx, 66
    mov ecx, 244
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 2
    call draw_text
    mov eax, NOVA_COLOR_PANEL
    mov ebx, 64
    mov ecx, 292
    mov edx, 672
    mov esi, 112
    call fill_rectangle
    mov eax, NOVA_COLOR_WARNING
    mov ebx, 64
    mov ecx, 292
    mov edx, 5
    mov esi, 112
    call fill_rectangle
    mov esi, text_panic_action1
    mov ebx, 92
    mov ecx, 318
    mov edx, NOVA_COLOR_WHITE
    mov ebp, 2
    call draw_text
    mov esi, text_panic_action2
    mov ebx, 92
    mov ecx, 350
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 2
    call draw_text
    mov esi, text_panic_code
    mov ebx, 66
    mov ecx, 438
    mov edx, NOVA_COLOR_WARNING
    mov ebp, 2
    call draw_text
    mov esi, panic_hex_buffer
    mov ebx, 246
    mov ecx, 438
    mov edx, NOVA_COLOR_WARNING
    mov ebp, 2
    call draw_text
    mov esi, text_panic_footer
    mov ebx, 66
    mov ecx, 492
    mov edx, NOVA_COLOR_SECONDARY
    mov ebp, 1
    call draw_text
.done:
    ret

draw_kernel_error_reference:
    mov eax, NOVA_COLOR_BLACK
    xor ebx, ebx
    xor ecx, ecx
    mov edx, [kernel_context + CONTEXT_WIDTH]
    mov esi, [kernel_context + CONTEXT_HEIGHT]
    call fill_rectangle
    mov eax, NOVA_COLOR_ERROR
    mov ebx, 102
    xor ecx, ecx
    mov edx, 628
    mov esi, 13
    call fill_rounded_rectangle
    mov dword [logo_x], 45
    mov dword [logo_y], 34
    mov dword [logo_color], NOVA_COLOR_ERROR_TEXT
    mov byte [logo_compact], 0
    mov byte [logo_mirror], 0
    call draw_nova_logo
    call draw_kernel_error_mark_full
    mov esi, text_brand_compact
    mov ebx, 336
    mov ecx, 70
    mov edx, NOVA_COLOR_BLUE
    mov ebp, 3
    call draw_text
    mov esi, text_error_title
    mov ebx, 338
    mov ecx, 134
    mov edx, NOVA_COLOR_ERROR_TEXT
    mov ebp, 2
    call draw_text
    mov esi, text_error_explain1
    mov ebx, 27
    mov ecx, 245
    mov edx, NOVA_COLOR_ERROR_TEXT
    mov ebp, 1
    call draw_text
    mov esi, text_error_explain2
    mov ecx, 273
    call draw_text
    mov eax, NOVA_COLOR_ERROR
    mov ebx, 149
    mov ecx, 350
    mov edx, 512
    mov esi, 120
    call fill_rounded_rectangle
    mov eax, NOVA_COLOR_LOG_PANEL
    mov ebx, 150
    mov ecx, 351
    mov edx, 510
    mov esi, 118
    call fill_rounded_rectangle
    mov esi, text_error_action1
    mov ebx, 186
    mov ecx, 371
    mov edx, NOVA_COLOR_ERROR_TEXT
    mov ebp, 1
    call draw_text
    mov esi, text_error_action2
    mov ebx, 381
    mov ecx, 399
    call draw_text
    mov esi, text_error_action3
    mov ebx, 187
    mov ecx, 427
    call draw_text
    mov esi, text_error_code
    mov ebx, 20
    mov ecx, 560
    mov edx, NOVA_COLOR_ERROR
    mov ebp, 1
    call draw_text
    mov esi, panic_hex_buffer
    mov ebx, 270
    mov ecx, 560
    call draw_text
    ret

draw_kernel_error_mark_full:
    pushad
    mov eax, [text_color]
    push eax
    mov dword [text_color], NOVA_COLOR_ERROR_TEXT
    xor ebp, ebp
.row:
    cmp ebp, NOVA_ERROR_MARK_HEIGHT
    jae .restore
    xor ecx, ecx
.column:
    cmp ecx, NOVA_ERROR_MARK_WIDTH
    jae .next_row
    mov eax, ebp
    imul eax, NOVA_ERROR_MARK_ROW_BYTES
    mov edx, ecx
    shr edx, 2
    add eax, edx
    mov al, [nova_error_mark_bitmap + eax]
    mov edx, ecx
    and edx, 3
    push ecx
    mov cl, dl
    shl cl, 1
    shr al, cl
    pop ecx
    and eax, 3
    jz .next_column
    push eax
    mov edi, ebp
    add edi, 84
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov edx, ecx
    add edx, 650
    shl edx, 2
    add edi, edx
    pop eax
    call blend_text_pixel
.next_column:
    inc ecx
    jmp .column
.next_row:
    inc ebp
    jmp .row
.restore:
    pop eax
    mov [text_color], eax
    popad
    ret

format_panic_hex:
    pushad
    mov ebx, eax
    mov edi, panic_hex_buffer
    mov ecx, 8
.digit:
    rol ebx, 4
    mov eax, ebx
    and eax, 0x0F
    cmp al, 10
    jb .number
    add al, 'A' - 10
    jmp .store
.number:
    add al, '0'
.store:
    stosb
    loop .digit
    mov byte [edi], 0
    popad
    ret

draw_panic_smile:
    pushad
    mov eax, [text_color]
    push eax
    mov dword [text_color], NOVA_COLOR_WHITE
    xor ebp, ebp
.row:
    cmp ebp, NOVA_ERROR_MARK_HEIGHT / 2
    jae .restore
    xor ecx, ecx
.column:
    cmp ecx, NOVA_ERROR_MARK_WIDTH / 2
    jae .next_row
    mov eax, ebp
    shl eax, 1
    imul eax, NOVA_ERROR_MARK_ROW_BYTES
    mov edx, ecx
    shl edx, 1
    mov ebx, edx
    shr ebx, 2
    add eax, ebx
    mov al, [nova_error_mark_bitmap + eax]
    and edx, 3
    push ecx
    mov cl, dl
    shl cl, 1
    shr al, cl
    pop ecx
    and eax, 3
    jz .next_column
    push eax
    mov edi, ebp
    add edi, 94
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov edx, ecx
    add edx, 66
    shl edx, 2
    add edi, edx
    pop eax
    call blend_text_pixel
.next_column:
    inc ecx
    jmp .column
.next_row:
    inc ebp
    jmp .row
.restore:
    pop eax
    mov [text_color], eax
    popad
    ret

fill_rounded_rectangle:
    pushad
    mov [round_color], eax
    mov [round_x], ebx
    mov [round_y], ecx
    mov [round_width], edx
    mov [round_height], esi
    mov eax, [round_color]
    mov ebx, [round_x]
    add ebx, 6
    mov ecx, [round_y]
    mov edx, [round_width]
    sub edx, 12
    mov esi, 1
    call fill_rectangle
    mov ebx, [round_x]
    add ebx, 3
    inc ecx
    mov edx, [round_width]
    sub edx, 6
    call fill_rectangle
    mov ebx, [round_x]
    inc ebx
    inc ecx
    mov edx, [round_width]
    sub edx, 2
    mov esi, 3
    call fill_rectangle
    mov ebx, [round_x]
    mov ecx, [round_y]
    add ecx, 5
    mov edx, [round_width]
    mov esi, [round_height]
    sub esi, 10
    call fill_rectangle
    mov ebx, [round_x]
    inc ebx
    mov ecx, [round_y]
    add ecx, [round_height]
    sub ecx, 5
    mov edx, [round_width]
    sub edx, 2
    mov esi, 3
    call fill_rectangle
    mov ebx, [round_x]
    add ebx, 3
    add ecx, 3
    mov edx, [round_width]
    sub edx, 6
    mov esi, 1
    call fill_rectangle
    mov ebx, [round_x]
    add ebx, 6
    inc ecx
    mov edx, [round_width]
    sub edx, 12
    call fill_rectangle
    popad
    ret

fill_rectangle:
    pushad
    test edx, edx
    jz .done
    test esi, esi
    jz .done

    mov [rect_color], eax
    mov [rect_x], ebx
    mov [rect_y], ecx
    mov [rect_width], edx
    mov [rect_height], esi
    xor ebp, ebp

.row:
    cmp ebp, [rect_height]
    jae .done
    mov eax, [rect_y]
    add eax, ebp
    imul eax, [kernel_context + CONTEXT_PITCH]
    add eax, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov edi, eax
    mov eax, [rect_x]
    shl eax, 2
    add edi, eax
    mov eax, [rect_color]
    mov ecx, [rect_width]
    rep stosd
    inc ebp
    jmp .row
.done:
    popad
    ret

; Gemeinsames Nova-Logo aus der SVG-Quelle, rechts oben auf jeder
; grafischen Oberfläche.
draw_nova_logo:
    pushad
    mov eax, [text_color]
    push eax
    mov eax, [logo_color]
    mov [text_color], eax
    xor ebp, ebp
.row:
    cmp ebp, NOVA_LOGO_HEIGHT
    jae .done
    cmp byte [logo_compact], 0
    je .row_visible
    mov eax, ebp
    and eax, 3
    cmp eax, 3
    je .next_row
.row_visible:
    xor ecx, ecx
.column:
    cmp ecx, NOVA_LOGO_WIDTH
    jae .next_row
    cmp byte [logo_compact], 0
    je .column_visible
    mov eax, ecx
    and eax, 3
    cmp eax, 3
    je .next_column
.column_visible:
    mov eax, ebp
    imul eax, NOVA_LOGO_ROW_BYTES
    mov edx, ecx
    shr edx, 2
    add eax, edx
    mov al, [nova_logo_bitmap + eax]
    mov edx, ecx
    and edx, 3
    push ecx
    mov cl, dl
    shl cl, 1
    shr al, cl
    pop ecx
    and eax, 3
    jz .next_column
    push eax
    mov edi, ebp
    cmp byte [logo_compact], 0
    je .row_scaled
    mov edx, ebp
    shr edx, 2
    sub edi, edx
.row_scaled:
    add edi, [logo_y]
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov edx, ecx
    cmp byte [logo_compact], 0
    je .column_scaled
    mov ebx, ecx
    shr ebx, 2
    sub edx, ebx
.column_scaled:
    cmp byte [logo_mirror], 0
    je .column_positioned
    mov ebx, 119
    sub ebx, edx
    mov edx, ebx
.column_positioned:
    add edx, [logo_x]
    shl edx, 2
    add edi, edx
    pop eax
    call blend_text_pixel
.next_column:
    inc ecx
    jmp .column
.next_row:
    inc ebp
    jmp .row
.done:
    pop eax
    mov [text_color], eax
    popad
    ret

; ESI=Text, EBX=x, ECX=y, EDX=Farbe, EBP=Skalierung
draw_text:
    mov [text_pointer], esi
    mov [text_x], ebx
    mov [text_origin_x], ebx
    mov [text_y], ecx
    mov [text_color], edx
    mov [text_scale], ebp

.next_character:
    mov esi, [text_pointer]
    lodsb
    mov [text_pointer], esi
    test al, al
    jz .done
    cmp al, 10
    je .newline
    cmp al, ' '
    je .space
    call draw_character
.advance:
    movzx eax, byte [character_advance]
    add eax, [text_scale]
    add [text_x], eax
    jmp .next_character
.space:
    mov byte [character_advance], 5
    jmp .advance
.newline:
    mov eax, [text_origin_x]
    mov [text_x], eax
    add dword [text_y], 24
    jmp .next_character
.done:
    ret

; AL=ASCII-Zeichen. 2-Bit-Deckungswerte sorgen für geglättete Kanten.
draw_character:
    pushad
    mov bl, al
    mov edi, bm_font_characters
    xor ecx, ecx
.find:
    mov al, [edi + ecx]
    test al, al
    jz .done
    cmp al, bl
    je .found
    inc ecx
    jmp .find
.found:
    mov al, [bm_font_widths + ecx]
    mov [character_advance], al
    imul ecx, BM_FONT_BYTES_PER_GLYPH
    mov esi, bm_font_bitmap
    add esi, ecx
    xor ebp, ebp
.glyph_row:
    cmp ebp, BM_FONT_HEIGHT
    jae .done
    xor ecx, ecx
.glyph_column:
    cmp ecx, BM_FONT_WIDTH
    jae .next_row
    mov eax, ebp
    imul eax, BM_FONT_WIDTH
    add eax, ecx
    mov edx, eax
    shr eax, 2
    and edx, 3
    mov al, [esi + eax]
    push ecx
    mov cl, dl
    shl cl, 1
    shr al, cl
    pop ecx
    and eax, 3
    jz .next_column

    push eax
    mov edi, [text_y]
    add edi, ebp
    imul edi, [kernel_context + CONTEXT_PITCH]
    add edi, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov edx, [text_x]
    add edx, ecx
    shl edx, 2
    add edi, edx
    pop eax
    call blend_text_pixel
.next_column:
    inc ecx
    jmp .glyph_column
.next_row:
    inc ebp
    jmp .glyph_row
.done:
    popad
    ret

; EDI=Framebuffer-Pixel, EAX=Deckung 1..3.
blend_text_pixel:
    cmp eax, 3
    je .opaque
    mov [blend_alpha], eax
    pushad
    mov ebx, [edi]
    mov esi, [text_color]

    mov eax, ebx
    and eax, 0xFF
    mov edx, esi
    and edx, 0xFF
    call blend_channel
    mov [blend_result], eax

    mov eax, ebx
    shr eax, 8
    and eax, 0xFF
    mov edx, esi
    shr edx, 8
    and edx, 0xFF
    call blend_channel
    shl eax, 8
    or [blend_result], eax

    mov eax, ebx
    shr eax, 16
    and eax, 0xFF
    mov edx, esi
    shr edx, 16
    and edx, 0xFF
    call blend_channel
    shl eax, 16
    or eax, [blend_result]
    mov [edi], eax
    popad
    ret
.opaque:
    mov eax, [text_color]
    mov [edi], eax
    ret

blend_channel:
    cmp dword [blend_alpha], 1
    je .one_third
    lea eax, [eax + edx * 2]
    jmp .divide
.one_third:
    lea eax, [edx + eax * 2]
.divide:
    imul eax, eax, 171
    shr eax, 9
    ret

; Interner Kernel Context. Keine Bootloader-Struktur wird nach außen gereicht.
CONTEXT_BIB               equ 0
CONTEXT_SEEN              equ 4
CONTEXT_PLATFORM          equ 8
CONTEXT_BOOT_DRIVE        equ 12
CONTEXT_MEMORY_MAP        equ 16
CONTEXT_MEMORY_COUNT      equ 20
CONTEXT_MEMORY_ENTRY_SIZE equ 24
CONTEXT_FRAMEBUFFER       equ 28
CONTEXT_PITCH             equ 32
CONTEXT_WIDTH             equ 36
CONTEXT_HEIGHT            equ 40
CONTEXT_BPP               equ 44
CONTEXT_PIXEL_FORMAT      equ 48
CONTEXT_KERNEL_ADDRESS    equ 52
CONTEXT_KERNEL_SIZE       equ 56
CONTEXT_KERNEL_ENTRY      equ 60
CONTEXT_SECURITY_STATE    equ 64
CONTEXT_CPU_FEATURE_EDX   equ 68
CONTEXT_CPU_FEATURE_ECX   equ 72
CONTEXT_ACPI_ADDRESS      equ 76
CONTEXT_MODULES_ADDRESS   equ 80
CONTEXT_MODULE_COUNT      equ 84
CONTEXT_ENTROPY_QUALITY   equ 88
CONTEXT_ENTROPY_SEED      equ 92
CONTEXT_SYSTEM_GENERATION equ 108
CONTEXT_BOOT_ATTEMPT      equ 112
CONTEXT_KERNEL_BUILD_ID   equ 116
CONTEXT_KERNEL_FORMAT     equ 136
CONTEXT_SIZE              equ 140

CONTEXT_HAS_FIRMWARE      equ 0x01
CONTEXT_HAS_MEMORY        equ 0x02
CONTEXT_HAS_GRAPHICS      equ 0x04
CONTEXT_HAS_KERNEL        equ 0x08
CONTEXT_HAS_CPU           equ 0x10
CONTEXT_HAS_ENTROPY       equ 0x20
CONTEXT_HAS_SYSTEM        equ 0x40
CONTEXT_HAS_KERNEL_ID     equ 0x80
CONTEXT_REQUIRED          equ CONTEXT_HAS_FIRMWARE | CONTEXT_HAS_MEMORY | CONTEXT_HAS_KERNEL | CONTEXT_HAS_CPU | CONTEXT_HAS_ENTROPY | CONTEXT_HAS_SYSTEM

align 8
kernel_context:
    times CONTEXT_SIZE db 0
stack_canary_seed: dd 0

COM1_BASE             equ 0x03F8
NOVA_COLOR_BACKGROUND equ 0x00101113
NOVA_COLOR_PANEL      equ 0x001A1C20
NOVA_COLOR_PANEL_EDGE equ 0x003D7DFF
NOVA_COLOR_CYAN       equ 0x004CC2FF
NOVA_COLOR_WHITE      equ 0x00FFFFFF
NOVA_COLOR_SECONDARY  equ 0x00CFCFCF
NOVA_COLOR_SUCCESS    equ 0x0030D158
NOVA_COLOR_WARNING    equ 0x00FFB347
NOVA_COLOR_BLACK      equ 0x00000000
NOVA_COLOR_BLUE       equ 0x002D7FC1
NOVA_COLOR_LOG_PANEL  equ 0x00121212
NOVA_COLOR_BUTTON     equ 0x00292929
NOVA_COLOR_ERROR      equ 0x00B6154B
NOVA_COLOR_ERROR_TEXT equ 0x00A9A9A9

align 4
rect_color:    dd 0
rect_x:        dd 0
rect_y:        dd 0
rect_width:    dd 0
rect_height:   dd 0
text_pointer:  dd 0
text_x:        dd 0
text_origin_x: dd 0
text_y:        dd 0
text_color:    dd 0
text_scale:    dd 1
character_advance: db 0
align 4
blend_alpha:   dd 0
blend_result:  dd 0
logo_x:        dd 620
logo_y:        dd 22
logo_color:    dd NOVA_COLOR_WHITE
logo_compact:  db 0
logo_mirror:   db 0
round_color:   dd 0
round_x:       dd 0
round_y:       dd 0
round_width:   dd 0
round_height:  dd 0

text_brand:
    db "NOVA OS", 0
text_brand_compact:
    db "NovaOS", 0
text_logsystem:
    db "Logsystem", 0
text_escape:
    db "ESC", 0
text_kernel_log:
    db "NOVA: Kernel Entry",10
    db "NOVA: Panic Reporter ABI 1.1 bereit",10
    db "NOVA: NBHP/BIB v1 validiert",10
    db "NOVA: PMM und Heap ABI 1.0 bereit",10
    db "NOVA: Object und Handle Manager bereit",10
    db "NOVA: Component und Service Manager bereit",10
    db "NOVA: Paging ABI 1.0 aktiv",10
    db "NOVA: IDT, PIC und PIT 100 Hz aktiv",10
    db "NOVA: IPC ABI 1.0 und Endpunkte bereit",10
    db "NOVA: Process und Thread Manager bereit",10
    db "NOVA: Security ABI 1.0 Capabilities aktiv",10
    db "NOVA: Scheduler und drei Threads aktiv",10
    db "NOVA: Device und Power Manager aktiv",10
    db "NOVA: VFS Mount-Namespace und Root bereit",10
    db "NOVA: Network Stack und Loopback lo0 bereit",10
    db "NOVA: x86-32 Ring-3 Userspace aktiv",10
    db "NOVA: System-Call ABI 1.0 aktiv",10
    db "NOVA: Shared Service Page bereit",10
    db "NOVA: Framebuffer und Kernel Context aktiv",10
    db "NOVA_KERNEL_READY",0
text_ready:
    db "SYSTEM BEREIT", 0
text_subtitle:
    db "DER KERNEL WURDE ERFOLGREICH GESTARTET", 0
text_status_heading:
    db "VALIDIERTE STARTKOMPONENTEN", 0
text_check_kernel:
    db "KERNEL-IMAGE UND CRC32", 0
text_check_handoff:
    db "NBHP/BIB BOOTPROTOKOLL", 0
text_check_memory:
    db "COMPONENTS OBJECTS PMM HEAP PAGING", 0
text_check_graphics:
    db "VBE FRAMEBUFFER", 0
text_check_interrupts:
    db "SECURITY PROCESS SERVICES SCHEDULER", 0
text_technical:
    db "NBHP/BIB 1.0 | X86-32 | 800 X 600", 0
text_panic_smile:
    db ";(", 0
text_panic_title:
    db "NOVA OS MUSSTE ANGEHALTEN WERDEN", 0
text_panic_explain1:
    db "EIN WICHTIGER TEIL DES SYSTEMS HAT NICHT RICHTIG REAGIERT.", 0
text_panic_explain2:
    db "ZU DEINER SICHERHEIT WURDE DER COMPUTER ANGEHALTEN.", 0
text_panic_action1:
    db "BITTE STARTE DEN COMPUTER NEU.", 0
text_panic_action2:
    db "WENN DAS ERNEUT PASSIERT NOTIERE DEN FEHLERCODE.", 0
text_panic_code:
    db "FEHLERCODE 0X", 0
text_panic_footer:
    db "TECHNISCHE DETAILS WURDEN IM SYSTEMPROTOKOLL GESPEICHERT.", 0
text_error_title:
    db "Start nicht m",0x94,"glich",0
text_error_explain1:
    db "NovaOS hat einen schweren Systemfehler festgestellt",0
text_error_explain2:
    db "Zu Ihrer Sicherheit wurde das System angehalten",0
text_error_action1:
    db "Bitte den Computer neu starten",0
text_error_action2:
    db "oder",0
text_error_action3:
    db "Den Fehlercode f",0x81,"r die Diagnose notieren",0
text_error_code:
    db "FEHLERCODE: KERNEL 0x",0
panic_hex_buffer:
    db "00000000", 0

; 5x7-Bitmap-Font für den frühen Bootstatus.
font_characters:
    db "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-./:|();", 0

font_bitmap:
    ; A-Z
    db 0x0E,0x11,0x11,0x1F,0x11,0x11,0x11
    db 0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E
    db 0x0E,0x11,0x10,0x10,0x10,0x11,0x0E
    db 0x1E,0x11,0x11,0x11,0x11,0x11,0x1E
    db 0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F
    db 0x1F,0x10,0x10,0x1E,0x10,0x10,0x10
    db 0x0E,0x11,0x10,0x17,0x11,0x11,0x0F
    db 0x11,0x11,0x11,0x1F,0x11,0x11,0x11
    db 0x0E,0x04,0x04,0x04,0x04,0x04,0x0E
    db 0x07,0x02,0x02,0x02,0x12,0x12,0x0C
    db 0x11,0x12,0x14,0x18,0x14,0x12,0x11
    db 0x10,0x10,0x10,0x10,0x10,0x10,0x1F
    db 0x11,0x1B,0x15,0x15,0x11,0x11,0x11
    db 0x11,0x19,0x15,0x13,0x11,0x11,0x11
    db 0x0E,0x11,0x11,0x11,0x11,0x11,0x0E
    db 0x1E,0x11,0x11,0x1E,0x10,0x10,0x10
    db 0x0E,0x11,0x11,0x11,0x15,0x12,0x0D
    db 0x1E,0x11,0x11,0x1E,0x14,0x12,0x11
    db 0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E
    db 0x1F,0x04,0x04,0x04,0x04,0x04,0x04
    db 0x11,0x11,0x11,0x11,0x11,0x11,0x0E
    db 0x11,0x11,0x11,0x11,0x11,0x0A,0x04
    db 0x11,0x11,0x11,0x15,0x15,0x15,0x0A
    db 0x11,0x11,0x0A,0x04,0x0A,0x11,0x11
    db 0x11,0x11,0x0A,0x04,0x04,0x04,0x04
    db 0x1F,0x01,0x02,0x04,0x08,0x10,0x1F
    ; 0-9
    db 0x0E,0x11,0x13,0x15,0x19,0x11,0x0E
    db 0x04,0x0C,0x04,0x04,0x04,0x04,0x0E
    db 0x0E,0x11,0x01,0x02,0x04,0x08,0x1F
    db 0x1E,0x01,0x01,0x0E,0x01,0x01,0x1E
    db 0x02,0x06,0x0A,0x12,0x1F,0x02,0x02
    db 0x1F,0x10,0x10,0x1E,0x01,0x01,0x1E
    db 0x0E,0x10,0x10,0x1E,0x11,0x11,0x0E
    db 0x1F,0x01,0x02,0x04,0x08,0x08,0x08
    db 0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E
    db 0x0E,0x11,0x11,0x0F,0x01,0x01,0x0E
    ; - . / : | ( )
    db 0x00,0x00,0x00,0x1F,0x00,0x00,0x00
    db 0x00,0x00,0x00,0x00,0x00,0x0C,0x0C
    db 0x01,0x02,0x02,0x04,0x08,0x08,0x10
    db 0x00,0x0C,0x0C,0x00,0x0C,0x0C,0x00
    db 0x04,0x04,0x04,0x04,0x04,0x04,0x04
    db 0x02,0x04,0x08,0x08,0x08,0x04,0x02
    db 0x08,0x04,0x02,0x02,0x02,0x04,0x08
    db 0x00,0x0C,0x0C,0x00,0x0C,0x08,0x10

message_entered:
    db "NOVA: Kernel Entry", 13, 10, 0
message_logging_ok:
    db "NOVA: Logging ABI 1.0, strukturierter Ring- und Reservepuffer bereit", 13, 10, 0
message_logging_query_ok:
    db "NOVA: Userspace Logging.Query capability-geprueft", 13, 10, 0
message_logging_read_ok:
    db "NOVA: Userspace neuesten atomar publizierten Logrecord gelesen", 13, 10, 0
message_boot_phase:
    db "NOVA: BOOT phase=0x", 0
message_boot_last_phase:
    db " last_phase=0x", 0
message_panic_manager_ok:
    db "NOVA: Panic Reporter ABI 1.1 bereit", 13, 10, 0
message_crash_dump_ok:
    db "NOVA: Crash Dump ABI 1.0, reservierter Minimal-Dump-Pfad bereit", 13, 10, 0
message_module_loader_ok:
    db "NOVA: Module Loader ABI 1.0, Trust-, ABI- und W^X-Pruefung bereit", 13, 10, 0
message_module_loader_error:
    db "NOVA PANIC: Module Loader Selbsttest fehlgeschlagen", 13, 10, 0
message_cpu_manager_ok:
    db "NOVA: CPU Manager ABI 1.0, BSP-Topologie und per-CPU-Daten aktiv", 13, 10, 0
message_cpu_manager_error:
    db "NOVA PANIC: CPU Manager Selbsttest fehlgeschlagen", 13, 10, 0
message_panic_begin:
    db "NOVA PANIC REPORT code=0x", 0
message_panic_subsystem:
    db " subsystem=0x", 0
message_panic_eip:
    db " eip=0x", 0
message_panic_cr2:
    db " cr2=0x", 0
message_panic_phase:
    db " phase=0x", 0
message_panic_last_phase:
    db " last_phase=0x", 0
message_panic_cpu:
    db " cpu=0x", 0
message_panic_security:
    db " security=0x", 0
message_panic_end:
    db "NOVA_PANIC_HALTED", 13, 10, 0
message_debug_panic:
    db "NOVA PANIC: manueller F12-Diagnosetest", 13, 10, 0
message_shutdown:
    db "NOVA: Shutdown angefordert, System wird ausgeschaltet", 13, 10, 0
message_power_manager_ok:
    db "NOVA: Power Manager ABI 1.0 und Shutdown-Pfad bereit", 13, 10, 0
message_power_query_ok:
    db "NOVA: Userspace Power.QuerySystem capability-geprueft", 13, 10, 0
message_power_wake_acquire_ok:
    db "NOVA: Userspace Power.WakeAcquire zeitlich begrenzt", 13, 10, 0
message_power_wake_release_ok:
    db "NOVA: Userspace Power.WakeRelease validiert", 13, 10, 0
message_power_profile_ok:
    db "NOVA: Userspace Power.SetProfile capability-geprueft", 13, 10, 0
message_power_shutdown_denied:
    db "NOVA: Userspace Power.RequestState ohne Capability abgewiesen", 13, 10, 0
message_power_manager_error:
    db "NOVA PANIC: Power Manager nicht initialisierbar", 13, 10, 0
message_network_manager_ok:
    db "NOVA: Network ABI 1.0, IPv4/IPv6 UDP, ICMP und TCP bereit", 13, 10, 0
message_network_query_ok:
    db "NOVA: Userspace Network.Query capability-geprueft", 13, 10, 0
message_network_raw_denied:
    db "NOVA: Userspace Raw-Socket ohne Capability abgewiesen", 13, 10, 0
message_network_socket_ok:
    db "NOVA: Userspace IPv4/IPv6 Datagramm-Socketobjekt erstellt", 13, 10, 0
message_network_stream_ok:
    db "NOVA: Userspace TCP-Stream-Socketobjekt erstellt", 13, 10, 0
message_network_connect_ok:
    db "NOVA: TCP-Stream mit IPv4 127.0.0.1 Port 9000 verbunden", 13, 10, 0
message_network_stream_send_ok:
    db "NOVA: TCP-Streamdaten in begrenzte Sendewarteschlange eingestellt", 13, 10, 0
message_network_stream_receive_ok:
    db "NOVA: TCP-Streamdaten geordnet aus Empfangswarteschlange gelesen", 13, 10, 0
message_network_stream_bind_ok:
    db "NOVA: TCP-Listener an IPv4 127.0.0.1 Port 9000 gebunden", 13, 10, 0
message_network_listen_ok:
    db "NOVA: TCP-Listener mit begrenztem Backlog aktiv", 13, 10, 0
message_network_accept_ok:
    db "NOVA: TCP-Verbindung als separates Socket-Handle akzeptiert", 13, 10, 0
message_network_connect_handle_denied:
    db "NOVA: TCP-Connect wegen ungueltigem Handle/Recht abgewiesen", 13, 10, 0
message_network_connect_firewall_denied:
    db "NOVA: TCP-Connect durch Bootstrap-Firewall abgewiesen", 13, 10, 0
message_network_connect_state_error:
    db "NOVA: TCP-Connect wegen ungueltigem Zustandswechsel abgewiesen", 13, 10, 0
message_network_bind_ok:
    db "NOVA: Userspace Socket an IPv4 127.0.0.1 Port 9000 gebunden", 13, 10, 0
message_network_loopback_ok:
    db "NOVA: IPv4/UDP Loopback mit Header- und Pruefsummenvalidierung erfolgreich", 13, 10, 0
message_network_size_denied:
    db "NOVA: Ueberlanges Loopback-Datagramm sicher abgewiesen", 13, 10, 0
message_network_send_ok:
    db "NOVA: IPv4/UDP Loopback-Datagramm in begrenzte Queue eingestellt", 13, 10, 0
message_network_malformed:
    db "NOVA: Beschaedigtes IPv4/UDP-Datagramm verworfen", 13, 10, 0
message_network_would_block:
    db "NOVA: Nichtblockierende Loopback-Queue meldet WOULD_BLOCK", 13, 10, 0
message_network_manager_error:
    db "NOVA PANIC: Network Manager nicht initialisierbar", 13, 10, 0
message_shutdown_requested:
    db "NOVA: Power Shutdown REQUEST", 13, 10, 0
message_shutdown_userspace:
    db "NOVA: Power Shutdown FREEZE_USERSPACE", 13, 10, 0
message_shutdown_storage:
    db "NOVA: Power Shutdown SYNC_STORAGE", 13, 10, 0
message_shutdown_devices:
    db "NOVA: Power Shutdown STOP_DEVICES", 13, 10, 0
message_shutdown_platform:
    db "NOVA: Power Shutdown PLATFORM_OFF", 13, 10, 0
message_bib_ok:
    db "NOVA: NBHP/BIB v1 validiert", 13, 10, 0
message_kernel_identity_ok:
    db "NOVA: Kernel Build-ID aus NBHP/BIB importiert", 13, 10, 0
message_pmm_ok:
    db "NOVA: PMM ABI 1.0 und Seitentest bereit", 13, 10, 0
message_pmm_error:
    db "NOVA PANIC: physischer Speichermanager nicht initialisierbar", 13, 10, 0
message_heap_ok:
    db "NOVA: Heap ABI 1.0 und Schreibtest bereit", 13, 10, 0
message_heap_error:
    db "NOVA PANIC: Bootstrap-Heap nicht initialisierbar", 13, 10, 0
message_object_manager_ok:
    db "NOVA: Object Manager ABI 1.0 bereit", 13, 10, 0
message_object_manager_error:
    db "NOVA PANIC: Kernel Object Manager nicht initialisierbar", 13, 10, 0
message_handle_manager_error:
    db "NOVA PANIC: Prozesslokaler Handle Manager nicht initialisierbar", 13, 10, 0
message_component_manager_ok:
    db "NOVA: Component Manager ABI 1.0 bereit", 13, 10, 0
message_component_manager_error:
    db "NOVA PANIC: Kernel Component Manager nicht initialisierbar", 13, 10, 0
message_paging_ok:
    db "NOVA: Paging ABI 1.0 und Speichertest bereit", 13, 10, 0
message_paging_error:
    db "NOVA PANIC: virtueller Speichermanager nicht initialisierbar", 13, 10, 0
message_interrupts_ok:
    db "NOVA: IDT, PIC und PIT 100 Hz aktiv", 13, 10, 0
message_interrupts_error:
    db "NOVA PANIC: Interrupt- oder Timerinitialisierung fehlgeschlagen", 13, 10, 0
message_ipc_ok:
    db "NOVA: IPC ABI 1.0 FIFO bereit", 13, 10, 0
message_ipc_error:
    db "NOVA PANIC: Kernel-IPC nicht initialisierbar", 13, 10, 0
message_service_manager_ok:
    db "NOVA: Service Manager ABI 1.0 bereit", 13, 10, 0
message_service_manager_error:
    db "NOVA PANIC: Kernel Service Manager nicht initialisierbar", 13, 10, 0
message_process_manager_ok:
    db "NOVA: Process Manager ABI 1.0 bereit", 13, 10, 0
message_process_manager_error:
    db "NOVA PANIC: Kernel Process Manager nicht initialisierbar", 13, 10, 0
message_security_ok:
    db "NOVA: Security ABI 1.0 Capabilities aktiv", 13, 10, 0
message_security_error:
    db "NOVA PANIC: Kernel Security nicht initialisierbar", 13, 10, 0
message_thread_manager_ok:
    db "NOVA: Thread Manager ABI 1.0 bereit", 13, 10, 0
message_thread_manager_error:
    db "NOVA PANIC: Kernel Thread Manager nicht initialisierbar", 13, 10, 0
message_scheduler_ok:
    db "NOVA: Scheduler ABI 1.0 und zwei Threads aktiv", 13, 10, 0
message_scheduler_error:
    db "NOVA PANIC: praemptiver Scheduler nicht initialisierbar", 13, 10, 0
message_device_manager_ok:
    db "NOVA: Device Manager ABI 1.0 und Bootgeraete aktiv", 13, 10, 0
message_device_manager_error:
    db "NOVA PANIC: Kernel Device Manager nicht initialisierbar", 13, 10, 0
message_vfs_ok:
    db "NOVA: VFS ABI 1.0, Mount-Namespace und Bootstrap-Root bereit", 13, 10, 0
message_vfs_error:
    db "NOVA PANIC: VFS oder Root-Dateisystem nicht initialisierbar", 13, 10, 0
message_userspace_ok:
    db "NOVA: x86-32 Userspace, TSS und System-Call ABI 1.0 bereit", 13, 10, 0
message_userspace_exit_ok:
    db "NOVA: erster Ring-3-Systemdienst kontrolliert ausgefuehrt", 13, 10, 0
message_userspace_error:
    db "NOVA PANIC: initialer Userspace-Prozess nicht startbar", 13, 10, 0
message_process_syscall_ok:
    db "NOVA: Userspace Process.QuerySelf erfolgreich", 13, 10, 0
message_process_handle_ok:
    db "NOVA: Process.OpenSelf Handle typ- und rechtegeprueft", 13, 10, 0
message_thread_syscall_ok:
    db "NOVA: Userspace Thread.QuerySelf erfolgreich", 13, 10, 0
message_thread_handle_ok:
    db "NOVA: Thread.OpenSelf Handle typ- und rechtegeprueft", 13, 10, 0
message_shared_service_ok:
    db "NOVA: Shared Service Page im Userspace validiert", 13, 10, 0
message_ipc_roundtrip_ok:
    db "NOVA: Userspace IPC Inline-Roundtrip atomar erfolgreich", 13, 10, 0
message_vfs_userspace_ok:
    db "NOVA: Userspace VFS.OpenRoot Handle erfolgreich", 13, 10, 0
message_vfs_lookup_ok:
    db "NOVA: Userspace VFS.Lookup Pfad '/' erfolgreich", 13, 10, 0
message_handle_close_ok:
    db "NOVA: Userspace Handle geschlossen, alter Wert ungueltig", 13, 10, 0
message_exception:
    db "NOVA PANIC: CPU-Ausnahme Vektor 0x", 0
message_fault_address:
    db " bei Adresse 0x", 0
message_newline:
    db 13, 10, 0
message_framebuffer_ok:
    db "NOVA: Kernel Context und Framebuffer aktiv", 13, 10, 0
message_text_mode:
    db "NOVA: Kernel Context, Textmodus aktiv", 13, 10, 0
message_bib_error:
    db "NOVA PANIC: ungueltiger Boot Handoff", 13, 10, 0
message_ready:
    db "NOVA_KERNEL_READY", 13, 10, 0

%include "nova-art.inc"
%include "boot-font-aa.inc"
