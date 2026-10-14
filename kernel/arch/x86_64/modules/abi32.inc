; ---------------------------------------------------------------------------
; Nova ABI/API-Layer §035
; NPSPEC-ABI-INTERNAL-0001     - Internes ABI
; NPSPEC-ABI-NOVA-0001         - Nova-ABI
; NPSPEC-ABI-STABLE-0001       - Stabiles ABI
; NPSPEC-ABI-SYSCALL-0001      - Syscall-Interface
; NPSPEC-ABI-VERSIONING-0001   - ABI-Versionierung
; NPSPEC-API-CONTRACT-0001     - API-Vertrag
; NPSPEC-API-DISCOVERY-0001    - API-Entdeckung
; NPSPEC-API-INTENT-0001       - Intent-basierte API
; NPSPEC-API-NEGOTIATION-0001  - API-Aushandlung
; NPSPEC-API-SEMANTIC-0001     - Semantische API
; NPSPEC-API-VERSIONING-0001   - API-Versionierung
; NPSPEC-FFI-0001              - Foreign-Function-Interface
; ---------------------------------------------------------------------------

; ABI-Versionierung (NPSPEC-ABI-VERSIONING-0001)
NOVA_ABI_VERSION_MAJOR equ 1
NOVA_ABI_VERSION_MINOR equ 0
NOVA_ABI_VERSION_PATCH equ 0
NOVA_ABI_VERSION       equ ((NOVA_ABI_VERSION_MAJOR << 16) | (NOVA_ABI_VERSION_MINOR << 8) | NOVA_ABI_VERSION_PATCH)

; Syscall-Nummern (NPSPEC-ABI-SYSCALL-0001)
NOVA_SYSCALL_EXIT         equ 0
NOVA_SYSCALL_WRITE        equ 1
NOVA_SYSCALL_READ         equ 2
NOVA_SYSCALL_OPEN         equ 3
NOVA_SYSCALL_CLOSE        equ 4
NOVA_SYSCALL_IPC_SEND     equ 5
NOVA_SYSCALL_IPC_RECV     equ 6
NOVA_SYSCALL_CAP_CREATE   equ 7
NOVA_SYSCALL_CAP_INVOKE   equ 8
NOVA_SYSCALL_MEM_MAP      equ 9
NOVA_SYSCALL_THREAD_NEW   equ 10
NOVA_SYSCALL_COUNT        equ 11

; API-Flags (NPSPEC-ABI-STABLE-0001)
ABI_FLAG_STABLE       equ 0x01  ; Stabiles ABI
ABI_FLAG_VERSIONED    equ 0x02  ; (NPSPEC-ABI-VERSIONING-0001)
ABI_FLAG_INTENT       equ 0x04  ; Intent-basiert (NPSPEC-API-INTENT-0001)
ABI_FLAG_SEMANTIC     equ 0x08  ; Semantisch (NPSPEC-API-SEMANTIC-0001)
ABI_FLAG_FFI          equ 0x10  ; FFI-kompatibel (NPSPEC-FFI-0001)
ABI_FLAG_INTERNAL     equ 0x20  ; (NPSPEC-ABI-INTERNAL-0001)

; Syscall-Descriptor (32 Bytes, NPSPEC-ABI-SYSCALL-0001)
ABI_SYSCTL_SIZE       equ 32
AS_NUMBER             equ 0    ; Syscall-Nummer
AS_FLAGS              equ 4    ; ABI_FLAG_*
AS_HANDLER            equ 8    ; Handler-Funktionszeiger
AS_MIN_VERSION        equ 12   ; Mindest-ABI-Version
AS_MAX_VERSION        equ 16   ; Max-ABI-Version (0=unbegrenzt)
AS_ARG_COUNT          equ 20   ; Anzahl Argumente
AS_INTENT_HASH        equ 24   ; Intent-Hash (NPSPEC-API-INTENT-0001)
; 28..31: reserviert

ABI_SYSCTL_CAPACITY   equ 16   ; Genug fuer NOVA_SYSCALL_COUNT + extra

; FFI-Record (NPSPEC-FFI-0001)
ABI_FFI_SIZE          equ 24
AF_LANG_ID            equ 0    ; Sprachen-ID (0=C 1=Nova 2=WASM)
AF_ENTRY              equ 4    ; Einsprungpunkt
AF_ARG_TYPES          equ 8    ; Typ-Bitmask
AF_RET_TYPE           equ 12
AF_VERSION            equ 16
; 20..23: reserviert

ABI_FFI_CAPACITY      equ 4

; ---------------------------------------------------------------------------
; abimgr_initialize
; ---------------------------------------------------------------------------
abimgr_initialize:
    push edi
    mov edi, abimgr_syscall_table
    xor eax, eax
    mov ecx, (ABI_SYSCTL_CAPACITY * ABI_SYSCTL_SIZE) / 4
    rep stosd
    mov edi, abimgr_ffi_table
    mov ecx, (ABI_FFI_CAPACITY * ABI_FFI_SIZE) / 4
    rep stosd
    pop edi
    mov dword [abimgr_syscall_count], 0
    mov dword [abimgr_ffi_count], 0
    mov dword [abimgr_version], NOVA_ABI_VERSION
    mov dword [abimgr_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; abimgr_register_syscall: EAX=number, EBX=flags, ECX=handler
; (NPSPEC-ABI-SYSCALL-0001, NPSPEC-ABI-NOVA-0001)
; CF=0 Erfolg
; ---------------------------------------------------------------------------
abimgr_register_syscall:
    cmp dword [abimgr_syscall_count], ABI_SYSCTL_CAPACITY
    jae .full
    cmp eax, NOVA_SYSCALL_COUNT
    jae .invalid
    push edi
    mov edi, abimgr_syscall_table
    push eax
    imul eax, ABI_SYSCTL_SIZE
    add edi, eax
    pop eax
    mov [edi + AS_NUMBER], eax
    mov [edi + AS_FLAGS], ebx
    mov [edi + AS_HANDLER], ecx
    mov edx, [abimgr_version]
    mov [edi + AS_MIN_VERSION], edx
    mov dword [edi + AS_MAX_VERSION], 0
    mov dword [edi + AS_ARG_COUNT], 0
    mov dword [edi + AS_INTENT_HASH], 0
    inc dword [abimgr_syscall_count]
    pop edi
    clc
    ret
.full:
.invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; abimgr_negotiate_version: EAX=requested_version -> EBX=negotiated
; (NPSPEC-API-NEGOTIATION-0001, NPSPEC-ABI-VERSIONING-0001)
; CF=0 Kompatibel
; ---------------------------------------------------------------------------
abimgr_negotiate_version:
    mov ebx, [abimgr_version]
    ; Major muss gleich sein
    mov ecx, eax
    shr ecx, 16
    push edx
    mov edx, ebx
    shr edx, 16
    cmp ecx, edx
    pop edx
    jne .incompatible
    ; Negotiated = min(requested, current)
    cmp eax, ebx
    jbe .use_requested
    mov eax, ebx
.use_requested:
    mov ebx, eax
    clc
    ret
.incompatible:
    stc
    ret

; ---------------------------------------------------------------------------
; abimgr_self_test
; ---------------------------------------------------------------------------
abimgr_self_test:
    push esi
    xor esi, esi
    cmp dword [abimgr_ready], 1
    je .t2
    inc esi
.t2:
    mov eax, NOVA_SYSCALL_WRITE
    mov ebx, ABI_FLAG_STABLE | ABI_FLAG_VERSIONED
    mov ecx, 0x100100  ; Stub-Handler
    call abimgr_register_syscall
    jnc .t3
    inc esi
.t3:
    mov eax, NOVA_ABI_VERSION
    call abimgr_negotiate_version
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
abimgr_ready:          dd 0
abimgr_syscall_count:  dd 0
abimgr_ffi_count:      dd 0
abimgr_version:        dd NOVA_ABI_VERSION

align 4
abimgr_syscall_table:
    times ABI_SYSCTL_CAPACITY * ABI_SYSCTL_SIZE db 0

align 4
abimgr_ffi_table:
    times ABI_FFI_CAPACITY * ABI_FFI_SIZE db 0
