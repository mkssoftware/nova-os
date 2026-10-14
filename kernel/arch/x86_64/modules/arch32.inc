; ---------------------------------------------------------------------------
; Nova Architecture-Model §002
; NPSPEC-ARCH-DETERMINISM-0001          - Determinismus
; NPSPEC-ARCH-EXECUTIONCONTRACT-0001    - Ausfuehrungs-Vertrag
; NPSPEC-ARCH-INTROSPECTION-0001        - Introspektion
; NPSPEC-ARCH-LIVEEVOLUTION-0001        - Live-Evolution
; NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001 - Ortstransparenz
; NPSPEC-ARCH-RESOURCEECONOMY-0001      - Ressourcen-Oekonomie
; NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001- Strukturierte Nebenl.
; NPSPEC-ARCH-SYSTEMMODEL-0001          - Systemmodell
; NPSPEC-ARCH-TRANSACTIONS-0001         - Transaktionen
; NPSPEC-ARCH-ZEROCOPY-0001             - Zero-Copy
; ---------------------------------------------------------------------------

; Architektur-Eigenschaften-Flags (NPSPEC-ARCH-SYSTEMMODEL-0001)
ARCH_PROP_DETERMINISTIC  equ 0x0001  ; (NPSPEC-ARCH-DETERMINISM-0001)
ARCH_PROP_ZEROCOPY       equ 0x0002  ; (NPSPEC-ARCH-ZEROCOPY-0001)
ARCH_PROP_TRANSACTIONAL  equ 0x0004  ; (NPSPEC-ARCH-TRANSACTIONS-0001)
ARCH_PROP_LOCATION_TRANS equ 0x0008  ; (NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001)
ARCH_PROP_LIVE_EVOLVE    equ 0x0010  ; (NPSPEC-ARCH-LIVEEVOLUTION-0001)
ARCH_PROP_STRUCTURED_CC  equ 0x0020  ; (NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001)
ARCH_PROP_INTROSPECTABLE equ 0x0040  ; (NPSPEC-ARCH-INTROSPECTION-0001)
ARCH_PROP_RESOURCE_ECO   equ 0x0080  ; (NPSPEC-ARCH-RESOURCEECONOMY-0001)

; Standard-Systemmodell-Flags fuer NovaOS
ARCH_NOVA_PROPERTIES     equ (ARCH_PROP_DETERMINISTIC | ARCH_PROP_ZEROCOPY | \
                               ARCH_PROP_TRANSACTIONAL | ARCH_PROP_STRUCTURED_CC | \
                               ARCH_PROP_INTROSPECTABLE | ARCH_PROP_RESOURCE_ECO)

; Execution-Contract (NPSPEC-ARCH-EXECUTIONCONTRACT-0001)
ARCH_CONTRACT_NONE       equ 0
ARCH_CONTRACT_REALTIME   equ 1
ARCH_CONTRACT_BEST_EFFORT equ 2
ARCH_CONTRACT_DEADLINE   equ 3

; Introspection-Info-Record (32 Bytes, NPSPEC-ARCH-INTROSPECTION-0001)
ARCH_INTRO_SIZE          equ 32
AI_MODULE_ID             equ 0
AI_VERSION               equ 4
AI_PROPERTIES            equ 8    ; ARCH_PROP_* Flags
AI_CONTRACT              equ 12
AI_ENTRY_POINT           equ 16
AI_INIT_TIME             equ 20
AI_SELF_TEST_RESULT      equ 24
; 28..31: reserviert

ARCH_INTRO_CAPACITY      equ 24   ; Bis zu 24 Module registriert

; ---------------------------------------------------------------------------
; arch_initialize
; ---------------------------------------------------------------------------
arch_initialize:
    push edi
    mov edi, arch_intro_table
    xor eax, eax
    mov ecx, (ARCH_INTRO_CAPACITY * ARCH_INTRO_SIZE) / 4
    rep stosd
    pop edi
    mov dword [arch_module_count], 0
    mov dword [arch_system_properties], ARCH_NOVA_PROPERTIES
    mov dword [arch_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; arch_register_module: EAX=module_id, EBX=version, ECX=properties, EDX=entry
; (NPSPEC-ARCH-INTROSPECTION-0001, NPSPEC-ARCH-SYSTEMMODEL-0001)
; CF=0 Erfolg
; ---------------------------------------------------------------------------
arch_register_module:
    cmp dword [arch_module_count], ARCH_INTRO_CAPACITY
    jae .full
    push edi
    push esi
    mov edi, arch_intro_table
    push eax
    mov eax, [arch_module_count]
    imul eax, ARCH_INTRO_SIZE
    add edi, eax
    pop eax
    mov [edi + AI_MODULE_ID], eax
    mov [edi + AI_VERSION], ebx
    mov [edi + AI_PROPERTIES], ecx
    mov dword [edi + AI_CONTRACT], ARCH_CONTRACT_BEST_EFFORT
    mov [edi + AI_ENTRY_POINT], edx
    push eax
    mov eax, [timer_ticks]
    mov [edi + AI_INIT_TIME], eax
    pop eax
    mov dword [edi + AI_SELF_TEST_RESULT], 0
    ; System-Properties erweitern
    or [arch_system_properties], ecx
    inc dword [arch_module_count]
    pop esi
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; arch_introspect: EAX=module_id -> EDI=record_ptr, CF=0 gefunden
; (NPSPEC-ARCH-INTROSPECTION-0001)
; ---------------------------------------------------------------------------
arch_introspect:
    push esi
    push ecx
    mov esi, arch_intro_table
    mov ecx, [arch_module_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + AI_MODULE_ID], eax
    je .found
    add esi, ARCH_INTRO_SIZE
    dec ecx
    jmp .scan
.found:
    mov edi, esi
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
; arch_self_test
; ---------------------------------------------------------------------------
arch_self_test:
    push esi
    xor esi, esi
    cmp dword [arch_ready], 1
    je .t2
    inc esi
.t2:
    mov eax, 0x0001          ; Kernel-Modul-ID
    mov ebx, 0x00010000      ; Version 1.0
    mov ecx, ARCH_PROP_DETERMINISTIC | ARCH_PROP_INTROSPECTABLE
    mov edx, 0x100000        ; Kernel-Entry
    call arch_register_module
    jnc .t3
    inc esi
.t3:
    mov eax, 0x0001
    call arch_introspect
    jnc .done
    inc esi
.done:
    pop esi
    test esi, esi
    jz .ok
    stc
    ret
.ok:
    clc
    ret

align 4
arch_ready:             dd 0
arch_module_count:      dd 0
arch_system_properties: dd 0

align 4
arch_intro_table:
    times ARCH_INTRO_CAPACITY * ARCH_INTRO_SIZE db 0
