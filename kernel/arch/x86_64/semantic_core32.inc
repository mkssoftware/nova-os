; Nova Semantic Core Bootstrap
; Grundlage fuer stabile IDs, Kernel-Registry und ersten Namespace.
; Entspricht den neueren NPSPECs zu ObjectID, Namespace, CapabilityID,
; SemanticTypeID und Registry. Diese Bootstrap-Implementierung ist bewusst
; statisch und wird vor Userspace/Scheduler-Mutationen vorbereitet.

NOVA_ID_KIND_OBJECT        equ 1
NOVA_ID_KIND_VOLUME        equ 2
NOVA_ID_KIND_NAMESPACE     equ 3
NOVA_ID_KIND_CAPABILITY    equ 4
NOVA_ID_KIND_SEMANTIC_TYPE equ 5

NOVA_REGISTRY_CAPACITY     equ 16
NOVA_REGISTRY_RECORD_SIZE  equ 24
NOVA_REGISTRY_KIND         equ 0
NOVA_REGISTRY_ID_HIGH      equ 4
NOVA_REGISTRY_ID_LOW       equ 8
NOVA_REGISTRY_NAME         equ 12
NOVA_REGISTRY_FLAGS        equ 16
NOVA_REGISTRY_RESERVED     equ 20

NOVA_NAMESPACE_CAPACITY    equ 8
NOVA_NAMESPACE_RECORD_SIZE equ 32
NOVA_NAMESPACE_ID_HIGH     equ 0
NOVA_NAMESPACE_ID_LOW      equ 4
NOVA_NAMESPACE_PARENT      equ 8
NOVA_NAMESPACE_NAME        equ 12
NOVA_NAMESPACE_KIND        equ 16
NOVA_NAMESPACE_FLAGS       equ 20
NOVA_NAMESPACE_OBJECT_HIGH equ 24
NOVA_NAMESPACE_OBJECT_LOW  equ 28

NOVA_PROJECTION_CAPACITY    equ 8
NOVA_PROJECTION_RECORD_SIZE equ 32
NOVA_PROJECTION_OBJECT_HIGH equ 0
NOVA_PROJECTION_OBJECT_LOW  equ 4
NOVA_PROJECTION_NAMESPACE   equ 8
NOVA_PROJECTION_NAME        equ 12
NOVA_PROJECTION_FLAGS       equ 16
NOVA_PROJECTION_TARGET_HIGH equ 20
NOVA_PROJECTION_TARGET_LOW  equ 24
NOVA_PROJECTION_RESERVED    equ 28

NOVA_NAMESPACE_ROOT        equ 0
NOVA_NAMESPACE_SYSTEM      equ 1
NOVA_NAMESPACE_BENUTZER    equ 2
NOVA_NAMESPACE_APPS        equ 3
NOVA_NAMESPACE_VOLUMES     equ 4
NOVA_NAMESPACE_BOOT        equ 5

NOVA_NS_FLAG_STABLE        equ 1
NOVA_NS_FLAG_SYSTEM        equ 2
NOVA_NS_FLAG_USER_VISIBLE  equ 4

NOVA_PROJ_FLAG_PRIMARY     equ 1
NOVA_PROJ_FLAG_STABLE      equ 2

NOVA_ID_OBJECT_HIGH        equ 0x4F424A49 ; "OBJI"
NOVA_ID_VOLUME_HIGH        equ 0x564F4C49 ; "VOLI"
NOVA_ID_NAMESPACE_HIGH     equ 0x4E534944 ; "NSID"
NOVA_ID_CAPABILITY_HIGH    equ 0x43415049 ; "CAPI"
NOVA_ID_SEMANTIC_HIGH      equ 0x53454D49 ; "SEMI"

semantic_core_initialize:
    mov edi, nova_registry_records
    xor eax, eax
    mov ecx, (NOVA_REGISTRY_CAPACITY * NOVA_REGISTRY_RECORD_SIZE) / 4
    rep stosd
    mov edi, nova_namespace_records
    mov ecx, (NOVA_NAMESPACE_CAPACITY * NOVA_NAMESPACE_RECORD_SIZE) / 4
    rep stosd
    mov edi, nova_projection_records
    mov ecx, (NOVA_PROJECTION_CAPACITY * NOVA_PROJECTION_RECORD_SIZE) / 4
    rep stosd
    mov dword [nova_registry_count], 0
    mov dword [nova_namespace_count], 0
    mov dword [nova_projection_count], 0
    mov dword [nova_object_id_counter], 1
    mov dword [nova_volume_id_counter], 1
    mov dword [nova_namespace_id_counter], 1
    mov dword [nova_capability_id_counter], 1
    mov dword [nova_semantic_type_id_counter], 1

    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 1
    mov ebx, NOVA_ID_KIND_OBJECT
    mov esi, nova_registry_name_object
    call semantic_core_registry_add
    jc .bad

    mov eax, NOVA_ID_VOLUME_HIGH
    mov edx, 1
    mov ebx, NOVA_ID_KIND_VOLUME
    mov esi, nova_registry_name_volume
    call semantic_core_registry_add
    jc .bad

    mov eax, NOVA_ID_NAMESPACE_HIGH
    mov edx, 1
    mov ebx, NOVA_ID_KIND_NAMESPACE
    mov esi, nova_registry_name_namespace
    call semantic_core_registry_add
    jc .bad

    mov eax, NOVA_ID_CAPABILITY_HIGH
    mov edx, 1
    mov ebx, NOVA_ID_KIND_CAPABILITY
    mov esi, nova_registry_name_capability
    call semantic_core_registry_add
    jc .bad

    mov eax, NOVA_ID_SEMANTIC_HIGH
    mov edx, 1
    mov ebx, NOVA_ID_KIND_SEMANTIC_TYPE
    mov esi, nova_registry_name_semantic_type
    call semantic_core_registry_add
    jc .bad

    ; Root und Standardbereiche des globalen Namespace.
    mov eax, NOVA_ID_NAMESPACE_HIGH
    mov edx, 1
    mov ebx, 0xffffffff
    mov esi, nova_ns_name_root
    mov ecx, NOVA_ID_OBJECT_HIGH
    mov edi, 1
    push dword NOVA_NS_FLAG_STABLE
    call semantic_core_namespace_add
    jc .bad
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 1
    mov ebx, NOVA_NAMESPACE_ROOT
    mov esi, nova_ns_name_root
    mov ecx, NOVA_ID_NAMESPACE_HIGH
    mov edi, 1
    push dword (NOVA_PROJ_FLAG_PRIMARY | NOVA_PROJ_FLAG_STABLE)
    call semantic_core_projection_add
    jc .bad

    mov eax, NOVA_ID_NAMESPACE_HIGH
    mov edx, 2
    mov ebx, NOVA_NAMESPACE_ROOT
    mov esi, nova_ns_name_system
    mov ecx, NOVA_ID_OBJECT_HIGH
    mov edi, 2
    push dword (NOVA_NS_FLAG_STABLE | NOVA_NS_FLAG_SYSTEM)
    call semantic_core_namespace_add
    jc .bad
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 2
    mov ebx, NOVA_NAMESPACE_SYSTEM
    mov esi, nova_ns_name_system
    mov ecx, NOVA_ID_NAMESPACE_HIGH
    mov edi, 2
    push dword (NOVA_PROJ_FLAG_PRIMARY | NOVA_PROJ_FLAG_STABLE)
    call semantic_core_projection_add
    jc .bad

    mov eax, NOVA_ID_NAMESPACE_HIGH
    mov edx, 3
    mov ebx, NOVA_NAMESPACE_ROOT
    mov esi, nova_ns_name_benutzer
    mov ecx, NOVA_ID_OBJECT_HIGH
    mov edi, 3
    push dword (NOVA_NS_FLAG_STABLE | NOVA_NS_FLAG_USER_VISIBLE)
    call semantic_core_namespace_add
    jc .bad
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 3
    mov ebx, NOVA_NAMESPACE_BENUTZER
    mov esi, nova_ns_name_benutzer
    mov ecx, NOVA_ID_NAMESPACE_HIGH
    mov edi, 3
    push dword (NOVA_PROJ_FLAG_PRIMARY | NOVA_PROJ_FLAG_STABLE)
    call semantic_core_projection_add
    jc .bad

    mov eax, NOVA_ID_NAMESPACE_HIGH
    mov edx, 4
    mov ebx, NOVA_NAMESPACE_ROOT
    mov esi, nova_ns_name_apps
    mov ecx, NOVA_ID_OBJECT_HIGH
    mov edi, 4
    push dword (NOVA_NS_FLAG_STABLE | NOVA_NS_FLAG_USER_VISIBLE)
    call semantic_core_namespace_add
    jc .bad
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 4
    mov ebx, NOVA_NAMESPACE_APPS
    mov esi, nova_ns_name_apps
    mov ecx, NOVA_ID_NAMESPACE_HIGH
    mov edi, 4
    push dword (NOVA_PROJ_FLAG_PRIMARY | NOVA_PROJ_FLAG_STABLE)
    call semantic_core_projection_add
    jc .bad

    mov eax, NOVA_ID_NAMESPACE_HIGH
    mov edx, 5
    mov ebx, NOVA_NAMESPACE_ROOT
    mov esi, nova_ns_name_volumes
    mov ecx, NOVA_ID_OBJECT_HIGH
    mov edi, 5
    push dword (NOVA_NS_FLAG_STABLE | NOVA_NS_FLAG_USER_VISIBLE)
    call semantic_core_namespace_add
    jc .bad
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 5
    mov ebx, NOVA_NAMESPACE_VOLUMES
    mov esi, nova_ns_name_volumes
    mov ecx, NOVA_ID_NAMESPACE_HIGH
    mov edi, 5
    push dword (NOVA_PROJ_FLAG_PRIMARY | NOVA_PROJ_FLAG_STABLE)
    call semantic_core_projection_add
    jc .bad

    mov eax, NOVA_ID_NAMESPACE_HIGH
    mov edx, 6
    mov ebx, NOVA_NAMESPACE_ROOT
    mov esi, nova_ns_name_boot
    mov ecx, NOVA_ID_OBJECT_HIGH
    mov edi, 6
    push dword (NOVA_NS_FLAG_STABLE | NOVA_NS_FLAG_SYSTEM)
    call semantic_core_namespace_add
    jc .bad
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 6
    mov ebx, NOVA_NAMESPACE_BOOT
    mov esi, nova_ns_name_boot
    mov ecx, NOVA_ID_NAMESPACE_HIGH
    mov edi, 6
    push dword (NOVA_PROJ_FLAG_PRIMARY | NOVA_PROJ_FLAG_STABLE)
    call semantic_core_projection_add
    jc .bad

    clc
    ret
.bad:
    stc
    ret

; EAX=ID high, EDX=ID low, EDI=record pointer oder CF=1.
semantic_core_registry_lookup:
    test eax, eax
    jz .bad
    test edx, edx
    jz .bad
    xor esi, esi
.scan:
    cmp esi, [nova_registry_count]
    jae .bad
    imul edi, esi, NOVA_REGISTRY_RECORD_SIZE
    add edi, nova_registry_records
    cmp [edi + NOVA_REGISTRY_ID_HIGH], eax
    jne .next
    cmp [edi + NOVA_REGISTRY_ID_LOW], edx
    jne .next
    clc
    ret
.next:
    inc esi
    jmp .scan
.bad:
    stc
    ret

; EAX=ID high, EDX=ID low, EBX=kind, ESI=name pointer.
semantic_core_registry_add:
    test eax, eax
    jz .bad
    test edx, edx
    jz .bad
    test ebx, ebx
    jz .bad
    test esi, esi
    jz .bad
    mov [nova_registry_temp_high], eax
    mov [nova_registry_temp_low], edx
    mov [nova_registry_temp_kind], ebx
    mov [nova_registry_temp_name], esi
    mov ecx, [nova_registry_count]
    cmp ecx, NOVA_REGISTRY_CAPACITY
    jae .bad
    imul edi, ecx, NOVA_REGISTRY_RECORD_SIZE
    add edi, nova_registry_records
    mov eax, [nova_registry_temp_kind]
    mov [edi + NOVA_REGISTRY_KIND], eax
    mov eax, [nova_registry_temp_high]
    mov [edi + NOVA_REGISTRY_ID_HIGH], eax
    mov eax, [nova_registry_temp_low]
    mov [edi + NOVA_REGISTRY_ID_LOW], eax
    mov eax, [nova_registry_temp_name]
    mov [edi + NOVA_REGISTRY_NAME], eax
    mov dword [edi + NOVA_REGISTRY_FLAGS], 1
    mov dword [edi + NOVA_REGISTRY_RESERVED], 0
    inc dword [nova_registry_count]
    clc
    ret
.bad:
    stc
    ret

; EAX/EDX=NamespaceID, EBX=parent slot oder 0xffffffff, ESI=name,
; ECX/EDI=ObjectID, [ESP+4]=flags. Entfernt das Flag-Argument selbst.
semantic_core_namespace_add:
    test eax, eax
    jz .bad
    test edx, edx
    jz .bad
    test esi, esi
    jz .bad
    test ecx, ecx
    jz .bad
    test edi, edi
    jz .bad
    mov [nova_namespace_temp_high], eax
    mov [nova_namespace_temp_low], edx
    mov [nova_namespace_temp_parent], ebx
    mov [nova_namespace_temp_name], esi
    mov [nova_namespace_temp_object_high], ecx
    mov [nova_namespace_temp_object_low], edi
    mov eax, [esp + 4]
    mov [nova_namespace_temp_flags], eax
    mov ecx, [nova_namespace_count]
    cmp ecx, NOVA_NAMESPACE_CAPACITY
    jae .bad
    imul edi, ecx, NOVA_NAMESPACE_RECORD_SIZE
    add edi, nova_namespace_records
    mov eax, [nova_namespace_temp_high]
    mov [edi + NOVA_NAMESPACE_ID_HIGH], eax
    mov eax, [nova_namespace_temp_low]
    mov [edi + NOVA_NAMESPACE_ID_LOW], eax
    mov eax, [nova_namespace_temp_parent]
    mov [edi + NOVA_NAMESPACE_PARENT], eax
    mov eax, [nova_namespace_temp_name]
    mov [edi + NOVA_NAMESPACE_NAME], eax
    mov dword [edi + NOVA_NAMESPACE_KIND], NOVA_ID_KIND_NAMESPACE
    mov eax, [nova_namespace_temp_flags]
    mov [edi + NOVA_NAMESPACE_FLAGS], eax
    mov eax, [nova_namespace_temp_object_high]
    mov [edi + NOVA_NAMESPACE_OBJECT_HIGH], eax
    mov eax, [nova_namespace_temp_object_low]
    mov [edi + NOVA_NAMESPACE_OBJECT_LOW], eax
    inc dword [nova_namespace_count]
    clc
    ret 4
.bad:
    stc
    ret 4

; EAX=NamespaceID high, EDX=NamespaceID low, EDI=record pointer oder CF=1.
semantic_core_namespace_lookup:
    test eax, eax
    jz .bad
    test edx, edx
    jz .bad
    xor esi, esi
.scan:
    cmp esi, [nova_namespace_count]
    jae .bad
    imul edi, esi, NOVA_NAMESPACE_RECORD_SIZE
    add edi, nova_namespace_records
    cmp [edi + NOVA_NAMESPACE_ID_HIGH], eax
    jne .next
    cmp [edi + NOVA_NAMESPACE_ID_LOW], edx
    jne .next
    clc
    ret
.next:
    inc esi
    jmp .scan
.bad:
    stc
    ret

; EAX/EDX=ObjectID, EBX=Namespace slot, ESI=name, ECX/EDI=Target NamespaceID,
; [ESP+4]=flags. Entfernt das Flag-Argument selbst.
semantic_core_projection_add:
    test eax, eax
    jz .bad
    test edx, edx
    jz .bad
    cmp ebx, [nova_namespace_count]
    jae .bad
    test esi, esi
    jz .bad
    test ecx, ecx
    jz .bad
    test edi, edi
    jz .bad
    mov [nova_projection_temp_object_high], eax
    mov [nova_projection_temp_object_low], edx
    mov [nova_projection_temp_namespace], ebx
    mov [nova_projection_temp_name], esi
    mov [nova_projection_temp_target_high], ecx
    mov [nova_projection_temp_target_low], edi
    mov eax, [esp + 4]
    mov [nova_projection_temp_flags], eax
    mov ecx, [nova_projection_count]
    cmp ecx, NOVA_PROJECTION_CAPACITY
    jae .bad
    imul edi, ecx, NOVA_PROJECTION_RECORD_SIZE
    add edi, nova_projection_records
    mov eax, [nova_projection_temp_object_high]
    mov [edi + NOVA_PROJECTION_OBJECT_HIGH], eax
    mov eax, [nova_projection_temp_object_low]
    mov [edi + NOVA_PROJECTION_OBJECT_LOW], eax
    mov eax, [nova_projection_temp_namespace]
    mov [edi + NOVA_PROJECTION_NAMESPACE], eax
    mov eax, [nova_projection_temp_name]
    mov [edi + NOVA_PROJECTION_NAME], eax
    mov eax, [nova_projection_temp_flags]
    mov [edi + NOVA_PROJECTION_FLAGS], eax
    mov eax, [nova_projection_temp_target_high]
    mov [edi + NOVA_PROJECTION_TARGET_HIGH], eax
    mov eax, [nova_projection_temp_target_low]
    mov [edi + NOVA_PROJECTION_TARGET_LOW], eax
    mov dword [edi + NOVA_PROJECTION_RESERVED], 0
    inc dword [nova_projection_count]
    clc
    ret 4
.bad:
    stc
    ret 4

; EAX/EDX=ObjectID, EDI=projection pointer oder CF=1.
semantic_core_projection_lookup_by_object:
    test eax, eax
    jz .bad
    test edx, edx
    jz .bad
    mov [nova_projection_lookup_high], eax
    mov [nova_projection_lookup_low], edx
    xor esi, esi
.scan:
    cmp esi, [nova_projection_count]
    jae .bad
    imul edi, esi, NOVA_PROJECTION_RECORD_SIZE
    add edi, nova_projection_records
    mov eax, [nova_projection_lookup_high]
    cmp [edi + NOVA_PROJECTION_OBJECT_HIGH], eax
    jne .next
    mov eax, [nova_projection_lookup_low]
    cmp [edi + NOVA_PROJECTION_OBJECT_LOW], eax
    jne .next
    clc
    ret
.next:
    inc esi
    jmp .scan
.bad:
    stc
    ret

semantic_core_self_test:
    cmp dword [nova_registry_count], 5
    jne .bad
    cmp dword [nova_namespace_count], 6
    jne .bad
    cmp dword [nova_projection_count], 6
    jne .bad
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 1
    call semantic_core_registry_lookup
    jc .bad
    cmp dword [edi + NOVA_REGISTRY_KIND], NOVA_ID_KIND_OBJECT
    jne .bad
    mov eax, NOVA_ID_CAPABILITY_HIGH
    mov edx, 1
    call semantic_core_registry_lookup
    jc .bad
    cmp dword [edi + NOVA_REGISTRY_KIND], NOVA_ID_KIND_CAPABILITY
    jne .bad
    mov eax, NOVA_ID_NAMESPACE_HIGH
    mov edx, 2
    call semantic_core_namespace_lookup
    jc .bad
    cmp dword [edi + NOVA_NAMESPACE_NAME], nova_ns_name_system
    jne .bad
    mov esi, NOVA_NAMESPACE_ROOT
    call semantic_core_namespace_record
    jc .bad
    cmp dword [edi + NOVA_NAMESPACE_PARENT], 0xffffffff
    jne .bad
    cmp dword [edi + NOVA_NAMESPACE_ID_HIGH], NOVA_ID_NAMESPACE_HIGH
    jne .bad
    cmp dword [edi + NOVA_NAMESPACE_ID_LOW], 1
    jne .bad
    mov esi, NOVA_NAMESPACE_SYSTEM
    call semantic_core_namespace_record
    jc .bad
    cmp dword [edi + NOVA_NAMESPACE_PARENT], NOVA_NAMESPACE_ROOT
    jne .bad
    test dword [edi + NOVA_NAMESPACE_FLAGS], NOVA_NS_FLAG_SYSTEM
    jz .bad
    mov esi, NOVA_NAMESPACE_VOLUMES
    call semantic_core_namespace_record
    jc .bad
    cmp dword [edi + NOVA_NAMESPACE_PARENT], NOVA_NAMESPACE_ROOT
    jne .bad
    test dword [edi + NOVA_NAMESPACE_FLAGS], NOVA_NS_FLAG_USER_VISIBLE
    jz .bad
    mov esi, NOVA_NAMESPACE_BOOT
    call semantic_core_namespace_record
    jc .bad
    cmp dword [edi + NOVA_NAMESPACE_ID_LOW], 6
    jne .bad
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 4
    call semantic_core_projection_lookup_by_object
    jc .bad
    cmp dword [edi + NOVA_PROJECTION_NAMESPACE], NOVA_NAMESPACE_APPS
    jne .bad
    cmp dword [edi + NOVA_PROJECTION_TARGET_LOW], 4
    jne .bad
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 0x7000
    call semantic_core_projection_lookup_by_object
    jnc .bad
    clc
    ret
.bad:
    stc
    ret

; ESI=slot, EDI=record pointer oder CF=1.
semantic_core_namespace_record:
    cmp esi, [nova_namespace_count]
    jae .bad
    imul edi, esi, NOVA_NAMESPACE_RECORD_SIZE
    add edi, nova_namespace_records
    clc
    ret
.bad:
    stc
    ret

align 4
nova_registry_count:          dd 0
nova_namespace_count:         dd 0
nova_projection_count:        dd 0
nova_object_id_counter:       dd 0
nova_volume_id_counter:       dd 0
nova_namespace_id_counter:    dd 0
nova_capability_id_counter:   dd 0
nova_semantic_type_id_counter: dd 0
nova_registry_temp_high:      dd 0
nova_registry_temp_low:       dd 0
nova_registry_temp_kind:      dd 0
nova_registry_temp_name:      dd 0
nova_namespace_temp_high:     dd 0
nova_namespace_temp_low:      dd 0
nova_namespace_temp_parent:   dd 0
nova_namespace_temp_name:     dd 0
nova_namespace_temp_flags:    dd 0
nova_namespace_temp_object_high: dd 0
nova_namespace_temp_object_low:  dd 0
nova_projection_temp_object_high: dd 0
nova_projection_temp_object_low:  dd 0
nova_projection_temp_namespace:   dd 0
nova_projection_temp_name:        dd 0
nova_projection_temp_flags:       dd 0
nova_projection_temp_target_high: dd 0
nova_projection_temp_target_low:  dd 0
nova_projection_lookup_high:      dd 0
nova_projection_lookup_low:       dd 0

nova_registry_records:
    times NOVA_REGISTRY_CAPACITY * NOVA_REGISTRY_RECORD_SIZE db 0
nova_namespace_records:
    times NOVA_NAMESPACE_CAPACITY * NOVA_NAMESPACE_RECORD_SIZE db 0
nova_projection_records:
    times NOVA_PROJECTION_CAPACITY * NOVA_PROJECTION_RECORD_SIZE db 0

nova_registry_name_object:
    db 'ObjectID', 0
nova_registry_name_volume:
    db 'VolumeID', 0
nova_registry_name_namespace:
    db 'NamespaceID', 0
nova_registry_name_capability:
    db 'CapabilityID', 0
nova_registry_name_semantic_type:
    db 'SemanticTypeID', 0
nova_ns_name_root:
    db '/', 0
nova_ns_name_system:
    db 'System', 0
nova_ns_name_benutzer:
    db 'Benutzer', 0
nova_ns_name_apps:
    db 'Apps', 0
nova_ns_name_volumes:
    db 'Volumes', 0
nova_ns_name_boot:
    db 'Boot', 0
