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
    call serial_initialize
    mov esi, message_entered
    call serial_write_string
    pop ebx
    pop eax

    push eax
    push ebx
    call panic_manager_initialize
    call panic_manager_self_test
    jc kernel_halt
    mov esi, message_panic_manager_ok
    call serial_write_string
    pop ebx
    pop eax

    cmp eax, NOVA_X86_BOOT_MAGIC
    jne panic_invalid_handoff
    test ebx, ebx
    jz panic_invalid_handoff
    test ebx, 7
    jnz panic_invalid_handoff

    mov [kernel_context + CONTEXT_BIB], ebx
    call validate_bib
    jc panic_invalid_handoff

    call create_kernel_context
    jc panic_invalid_handoff
    call early_security_entropy_initialize
    jc panic_invalid_handoff

    mov esi, message_bib_ok
    call serial_write_string
    test dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_KERNEL_ID
    jz .kernel_identity_done
    mov esi, message_kernel_identity_ok
    call serial_write_string
.kernel_identity_done:

    call pmm_initialize
    jc panic_memory_manager
    call pmm_self_test
    jc panic_memory_manager
    mov esi, message_pmm_ok
    call serial_write_string

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

    call component_manager_initialize
    jc panic_component_manager
    call component_manager_self_test
    jc panic_component_manager
    mov esi, message_component_manager_ok
    call serial_write_string

    call paging_initialize
    jc panic_paging
    call paging_self_test
    jc panic_paging
    mov esi, message_paging_ok
    call serial_write_string

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
; Kernel Object Manager (ADR-2008)
; ---------------------------------------------------------------------------

OBJECT_API_SIZE       equ 32
OBJECT_API_ABI_MAJOR  equ 1
OBJECT_API_ABI_MINOR  equ 0
OBJECT_TABLE_CAPACITY equ 16
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
    lidt [idt_descriptor]

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
    cmp eax, 32
    jb .exception
    cmp eax, 48
    jb .slave_irq
    jmp .done
.timer:
    inc dword [timer_ticks]
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

; Kernel Security / Capability Manager (ADR-2013)
SECURITY_API_SIZE       equ 32
SECURITY_CAPACITY       equ PROCESS_CAPACITY
SECURITY_RECORD_SIZE    equ 8
SECURITY_CAP_MEMORY     equ 0x00000001
SECURITY_CAP_IO         equ 0x00000002
SECURITY_CAP_SERVICE    equ 0x00000004
SECURITY_CAP_ADMIN      equ 0x00000008
SECURITY_KERNEL_CAPS    equ 0x0000000F

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
    mov edx, 0x00000010
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
    jmp kernel_idle

kernel_idle:
    sti
.loop:
    hlt
    jmp .loop

kernel_shutdown:
    cli
    mov esi, message_shutdown
    call serial_write_string
    mov ax, 0x2000
    mov dx, 0x0604                  ; QEMU/ACPI Poweroff
    out dx, ax
    mov dx, 0xB004                  ; Bochs/QEMU Fallback
    out dx, ax
    jmp kernel_halt

; Strukturierter Panic-Reporter (ADR-2014)
PANIC_API_SIZE    equ 32
PANIC_REPORT_SIZE equ 32

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
    dw 1, 0
    dd PANIC_REPORT_SIZE
    dd kernel_panic
    dd panic_report
    dd panic_manager_self_test
    dd 0
    dd 0
panic_report:
    times PANIC_REPORT_SIZE db 0

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
    db "NOVA: Panic Reporter ABI 1.0 bereit",10
    db "NOVA: NBHP/BIB v1 validiert",10
    db "NOVA: PMM ABI 1.0 und Seitentest bereit",10
    db "NOVA: Heap ABI 1.0 und Schreibtest bereit",10
    db "NOVA: Object Manager ABI 1.0 bereit",10
    db "NOVA: Component Manager ABI 1.0 bereit",10
    db "NOVA: Paging ABI 1.0 und Speichertest bereit",10
    db "NOVA: IDT, PIC und PIT 100 Hz aktiv",10
    db "NOVA: IPC ABI 1.0 FIFO bereit",10
    db "NOVA: Service Manager ABI 1.0 bereit",10
    db "NOVA: Process Manager ABI 1.0 bereit",10
    db "NOVA: Security ABI 1.0 Capabilities aktiv",10
    db "NOVA: Thread Manager ABI 1.0 bereit",10
    db "NOVA: Scheduler ABI 1.0 und zwei Threads aktiv",10
    db "NOVA: Kernel Context und Framebuffer aktiv",10
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
message_panic_manager_ok:
    db "NOVA: Panic Reporter ABI 1.0 bereit", 13, 10, 0
message_panic_begin:
    db "NOVA PANIC REPORT code=0x", 0
message_panic_subsystem:
    db " subsystem=0x", 0
message_panic_eip:
    db " eip=0x", 0
message_panic_cr2:
    db " cr2=0x", 0
message_panic_end:
    db "NOVA_PANIC_HALTED", 13, 10, 0
message_debug_panic:
    db "NOVA PANIC: manueller F12-Diagnosetest", 13, 10, 0
message_shutdown:
    db "NOVA: Shutdown angefordert, System wird ausgeschaltet", 13, 10, 0
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
