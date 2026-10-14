; ---------------------------------------------------------------------------
; Nova Object-Subsystem §025
; NPSPEC-OBJECT-IDENTITY-0001    - Objekt-Identitaet
; NPSPEC-OBJECT-MODEL-0001       - Objektmodell
; NPSPEC-OBJECT-PERMISSION-0001  - Objektberechtigungen
; NPSPEC-OBJECT-PIPELINE-0001    - Objekt-Pipeline
; NPSPEC-OBJECT-PROVENANCE-0001  - Herkunft
; NPSPEC-OBJECT-REACTIVE-0001    - Reaktive Objekte
; NPSPEC-OBJECT-REFERENCE-0001   - Referenzzaehlung
; NPSPEC-OBJECT-RELATIONSHIP-0001- Beziehungen
; NPSPEC-OBJECT-VERSIONING-0001  - Versionierung
; NPSPEC-OBJECT-ZEROCOPY-0001    - Zero-Copy-Transfer
; ---------------------------------------------------------------------------

; Objekt-Typen (NPSPEC-OBJECT-MODEL-0001)
OBJ_TYPE_DATA         equ 0
OBJ_TYPE_STREAM       equ 1    ; (NPSPEC-OBJECT-PIPELINE-0001)
OBJ_TYPE_REACTIVE     equ 2    ; (NPSPEC-OBJECT-REACTIVE-0001)
OBJ_TYPE_REFERENCE    equ 3    ; (NPSPEC-OBJECT-REFERENCE-0001)

; Beziehungstypen (NPSPEC-OBJECT-RELATIONSHIP-0001)
OBJ_REL_NONE          equ 0
OBJ_REL_PARENT        equ 1
OBJ_REL_CHILD         equ 2
OBJ_REL_ALIAS         equ 3
OBJ_REL_PIPELINE      equ 4    ; (NPSPEC-OBJECT-PIPELINE-0001)

; Objekt-Flags
OBJ_FLAG_IMMUTABLE    equ 0x01
OBJ_FLAG_ZEROCOPY     equ 0x02  ; (NPSPEC-OBJECT-ZEROCOPY-0001)
OBJ_FLAG_REACTIVE     equ 0x04  ; (NPSPEC-OBJECT-REACTIVE-0001)
OBJ_FLAG_VERSIONED    equ 0x08  ; (NPSPEC-OBJECT-VERSIONING-0001)

; Objekt-Record (48 Bytes)
NOVA_OBJ_SIZE         equ 48
NO_ID                 equ 0
NO_TYPE               equ 4    ; OBJ_TYPE_*
NO_FLAGS              equ 8
NO_REFCOUNT           equ 12   ; (NPSPEC-OBJECT-REFERENCE-0001)
NO_VERSION            equ 16   ; (NPSPEC-OBJECT-VERSIONING-0001)
NO_PERM_MASK          equ 20   ; (NPSPEC-OBJECT-PERMISSION-0001)
NO_PARENT_ID          equ 24   ; (NPSPEC-OBJECT-RELATIONSHIP-0001)
NO_PROV_HASH          equ 28   ; (NPSPEC-OBJECT-PROVENANCE-0001)
NO_DATA_PTR           equ 32
NO_DATA_LEN           equ 36
NO_PIPELINE_NEXT      equ 40   ; (NPSPEC-OBJECT-PIPELINE-0001)
; 44..47: reserviert

OBJ_CAPACITY          equ 16

; ---------------------------------------------------------------------------
; object_initialize
; ---------------------------------------------------------------------------
object_initialize:
    push edi
    mov edi, obj_table
    xor eax, eax
    mov ecx, (OBJ_CAPACITY * NOVA_OBJ_SIZE) / 4
    rep stosd
    pop edi
    mov dword [obj_count], 0
    mov dword [obj_next_id], 1
    mov dword [obj_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; obj_create: EAX=type, EBX=flags, ECX=parent_id -> EDX=obj_id, CF=0
; (NPSPEC-OBJECT-IDENTITY-0001, NPSPEC-OBJECT-MODEL-0001)
; ---------------------------------------------------------------------------
obj_create:
    cmp dword [obj_count], OBJ_CAPACITY
    jae .full
    push edi
    push esi
    mov edi, obj_table
    push eax
    mov eax, [obj_count]
    imul eax, NOVA_OBJ_SIZE
    add edi, eax
    pop eax
    mov edx, [obj_next_id]
    mov [edi + NO_ID], edx
    mov [edi + NO_TYPE], eax
    mov [edi + NO_FLAGS], ebx
    mov dword [edi + NO_REFCOUNT], 1
    mov dword [edi + NO_VERSION], 1
    mov dword [edi + NO_PERM_MASK], FSPERM_0644
    mov [edi + NO_PARENT_ID], ecx
    ; Provenance-Hash: XOR(id, ticks)
    push eax
    mov eax, [timer_ticks]
    xor eax, edx
    mov [edi + NO_PROV_HASH], eax
    pop eax
    mov dword [edi + NO_DATA_PTR], 0
    mov dword [edi + NO_DATA_LEN], 0
    mov dword [edi + NO_PIPELINE_NEXT], 0xFFFFFFFF
    inc dword [obj_next_id]
    inc dword [obj_count]
    pop esi
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; obj_addref / obj_release: EDX=obj_id
; (NPSPEC-OBJECT-REFERENCE-0001)
; ---------------------------------------------------------------------------
obj_addref:
    push esi
    push ecx
    mov esi, obj_table
    mov ecx, [obj_count]
.scan_a:
    test ecx, ecx
    jz .not_found_a
    cmp [esi + NO_ID], edx
    je .found_a
    add esi, NOVA_OBJ_SIZE
    dec ecx
    jmp .scan_a
.found_a:
    inc dword [esi + NO_REFCOUNT]
    pop ecx
    pop esi
    clc
    ret
.not_found_a:
    pop ecx
    pop esi
    stc
    ret

obj_release:
    push esi
    push ecx
    mov esi, obj_table
    mov ecx, [obj_count]
.scan_r:
    test ecx, ecx
    jz .not_found_r
    cmp [esi + NO_ID], edx
    je .found_r
    add esi, NOVA_OBJ_SIZE
    dec ecx
    jmp .scan_r
.found_r:
    cmp dword [esi + NO_REFCOUNT], 0
    je .already_zero
    dec dword [esi + NO_REFCOUNT]
    pop ecx
    pop esi
    clc
    ret
.already_zero:
    pop ecx
    pop esi
    stc
    ret
.not_found_r:
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; obj_version_bump: EDX=obj_id (NPSPEC-OBJECT-VERSIONING-0001)
; ---------------------------------------------------------------------------
obj_version_bump:
    push esi
    push ecx
    mov esi, obj_table
    mov ecx, [obj_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + NO_ID], edx
    je .found
    add esi, NOVA_OBJ_SIZE
    dec ecx
    jmp .scan
.found:
    inc dword [esi + NO_VERSION]
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
; object_self_test
; ---------------------------------------------------------------------------
object_self_test:
    push esi
    xor esi, esi
    cmp dword [obj_ready], 1
    je .t2
    inc esi
.t2:
    mov eax, OBJ_TYPE_DATA
    mov ebx, OBJ_FLAG_VERSIONED
    mov ecx, 0xFFFFFFFF
    call obj_create
    jnc .t3
    inc esi
.t3:
    call obj_addref
    jnc .t4
    inc esi
.t4:
    call obj_release
    jnc .t5
    inc esi
.t5:
    call obj_version_bump
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
obj_ready:    dd 0
obj_count:    dd 0
obj_next_id:  dd 1

align 4
obj_table:
    times OBJ_CAPACITY * NOVA_OBJ_SIZE db 0
