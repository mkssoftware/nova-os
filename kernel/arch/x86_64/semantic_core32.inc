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

NOVA_OBJECT_CAPACITY       equ 16
NOVA_OBJECT_RECORD_SIZE    equ 32
NOVA_OBJECT_ID_HIGH        equ 0
NOVA_OBJECT_ID_LOW         equ 4
NOVA_OBJECT_NAMESPACE      equ 8
NOVA_OBJECT_NAME           equ 12
NOVA_OBJECT_FLAGS          equ 16
NOVA_OBJECT_SEM_HIGH       equ 20
NOVA_OBJECT_SEM_LOW        equ 24
NOVA_OBJECT_RESERVED       equ 28

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

NOVA_CAPABILITY_CAPACITY    equ 16
NOVA_CAPABILITY_RECORD_SIZE equ 32
NOVA_CAPABILITY_ID_HIGH     equ 0
NOVA_CAPABILITY_ID_LOW      equ 4
NOVA_CAPABILITY_OBJECT_HIGH equ 8
NOVA_CAPABILITY_OBJECT_LOW  equ 12
NOVA_CAPABILITY_NAMESPACE   equ 16
NOVA_CAPABILITY_RIGHTS      equ 20
NOVA_CAPABILITY_FLAGS       equ 24
NOVA_CAPABILITY_RESERVED    equ 28

NOVA_HANDLE_CAPACITY        equ 16
NOVA_HANDLE_RECORD_SIZE     equ 32
NOVA_HANDLE_ID              equ 0
NOVA_HANDLE_OBJECT_HIGH     equ 4
NOVA_HANDLE_OBJECT_LOW      equ 8
NOVA_HANDLE_NAMESPACE       equ 12
NOVA_HANDLE_RIGHTS          equ 16
NOVA_HANDLE_FLAGS           equ 20
NOVA_HANDLE_GENERATION      equ 24
NOVA_HANDLE_RESERVED        equ 28

NOVA_NAMESPACE_ROOT        equ 0
NOVA_NAMESPACE_SYSTEM      equ 1
NOVA_NAMESPACE_BENUTZER    equ 2
NOVA_NAMESPACE_APPS        equ 3
NOVA_NAMESPACE_VOLUMES     equ 4
NOVA_NAMESPACE_BOOT        equ 5

NOVA_NS_FLAG_STABLE        equ 1
NOVA_NS_FLAG_SYSTEM        equ 2
NOVA_NS_FLAG_USER_VISIBLE  equ 4

NOVA_OBJECT_FLAG_STABLE       equ 1
NOVA_OBJECT_FLAG_SYSTEM       equ 2
NOVA_OBJECT_FLAG_USER_VISIBLE equ 4
NOVA_OBJECT_FLAG_NAMESPACE    equ 8

NOVA_PROJ_FLAG_PRIMARY     equ 1
NOVA_PROJ_FLAG_STABLE      equ 2

NOVA_CAP_FLAG_ACTIVE       equ 1
NOVA_CAP_FLAG_STABLE       equ 2

NOVA_HANDLE_FLAG_ACTIVE    equ 1
NOVA_HANDLE_RIGHT_READ     equ 1
NOVA_HANDLE_RIGHT_WRITE    equ 2
NOVA_HANDLE_RIGHT_EXECUTE  equ 4
NOVA_HANDLE_RIGHT_MANAGE   equ 8

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
    mov edi, nova_object_records
    mov ecx, (NOVA_OBJECT_CAPACITY * NOVA_OBJECT_RECORD_SIZE) / 4
    rep stosd
    mov edi, nova_namespace_records
    mov ecx, (NOVA_NAMESPACE_CAPACITY * NOVA_NAMESPACE_RECORD_SIZE) / 4
    rep stosd
    mov edi, nova_projection_records
    mov ecx, (NOVA_PROJECTION_CAPACITY * NOVA_PROJECTION_RECORD_SIZE) / 4
    rep stosd
    mov edi, nova_capability_records
    mov ecx, (NOVA_CAPABILITY_CAPACITY * NOVA_CAPABILITY_RECORD_SIZE) / 4
    rep stosd
    mov edi, nova_handle_records
    mov ecx, (NOVA_HANDLE_CAPACITY * NOVA_HANDLE_RECORD_SIZE) / 4
    rep stosd
    mov dword [nova_registry_count], 0
    mov dword [nova_object_count], 0
    mov dword [nova_namespace_count], 0
    mov dword [nova_projection_count], 0
    mov dword [nova_capability_count], 0
    mov dword [nova_handle_count], 0
    mov dword [nova_handle_next_id], 1
    mov dword [nova_handle_generation], 1
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

    ; Fruehe Filesystem-Object-Registry fuer stabile Namespace-Objekte.
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 1
    mov ebx, NOVA_NAMESPACE_ROOT
    mov esi, nova_ns_name_root
    mov ecx, (NOVA_OBJECT_FLAG_STABLE | NOVA_OBJECT_FLAG_SYSTEM | NOVA_OBJECT_FLAG_NAMESPACE)
    call semantic_core_object_add
    jc .bad

    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 2
    mov ebx, NOVA_NAMESPACE_SYSTEM
    mov esi, nova_ns_name_system
    mov ecx, (NOVA_OBJECT_FLAG_STABLE | NOVA_OBJECT_FLAG_SYSTEM | NOVA_OBJECT_FLAG_NAMESPACE)
    call semantic_core_object_add
    jc .bad

    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 3
    mov ebx, NOVA_NAMESPACE_BENUTZER
    mov esi, nova_ns_name_benutzer
    mov ecx, (NOVA_OBJECT_FLAG_STABLE | NOVA_OBJECT_FLAG_USER_VISIBLE | NOVA_OBJECT_FLAG_NAMESPACE)
    call semantic_core_object_add
    jc .bad

    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 4
    mov ebx, NOVA_NAMESPACE_APPS
    mov esi, nova_ns_name_apps
    mov ecx, (NOVA_OBJECT_FLAG_STABLE | NOVA_OBJECT_FLAG_USER_VISIBLE | NOVA_OBJECT_FLAG_NAMESPACE)
    call semantic_core_object_add
    jc .bad

    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 5
    mov ebx, NOVA_NAMESPACE_VOLUMES
    mov esi, nova_ns_name_volumes
    mov ecx, (NOVA_OBJECT_FLAG_STABLE | NOVA_OBJECT_FLAG_USER_VISIBLE | NOVA_OBJECT_FLAG_NAMESPACE)
    call semantic_core_object_add
    jc .bad

    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 6
    mov ebx, NOVA_NAMESPACE_BOOT
    mov esi, nova_ns_name_boot
    mov ecx, (NOVA_OBJECT_FLAG_STABLE | NOVA_OBJECT_FLAG_SYSTEM | NOVA_OBJECT_FLAG_NAMESPACE)
    call semantic_core_object_add
    jc .bad

    ; Feste fruehe Kernel-Capabilities fuer Systemdienste.
    mov eax, NOVA_ID_CAPABILITY_HIGH
    mov edx, 1
    mov ebx, NOVA_ID_OBJECT_HIGH
    mov ecx, 2
    mov esi, NOVA_NAMESPACE_SYSTEM
    mov edi, (NOVA_HANDLE_RIGHT_READ | NOVA_HANDLE_RIGHT_MANAGE)
    push dword (NOVA_CAP_FLAG_ACTIVE | NOVA_CAP_FLAG_STABLE)
    call semantic_core_capability_add
    jc .bad

    mov eax, NOVA_ID_CAPABILITY_HIGH
    mov edx, 2
    mov ebx, NOVA_ID_OBJECT_HIGH
    mov ecx, 4
    mov esi, NOVA_NAMESPACE_APPS
    mov edi, (NOVA_HANDLE_RIGHT_READ | NOVA_HANDLE_RIGHT_EXECUTE)
    push dword (NOVA_CAP_FLAG_ACTIVE | NOVA_CAP_FLAG_STABLE)
    call semantic_core_capability_add
    jc .bad

    mov eax, NOVA_ID_CAPABILITY_HIGH
    mov edx, 3
    mov ebx, NOVA_ID_OBJECT_HIGH
    mov ecx, 6
    mov esi, NOVA_NAMESPACE_BOOT
    mov edi, (NOVA_HANDLE_RIGHT_READ | NOVA_HANDLE_RIGHT_EXECUTE)
    push dword (NOVA_CAP_FLAG_ACTIVE | NOVA_CAP_FLAG_STABLE)
    call semantic_core_capability_add
    jc .bad

    mov eax, NOVA_ID_CAPABILITY_HIGH
    mov edx, 4
    mov ebx, NOVA_ID_OBJECT_HIGH
    mov ecx, 5
    mov esi, NOVA_NAMESPACE_VOLUMES
    mov edi, NOVA_HANDLE_RIGHT_READ
    push dword NOVA_CAP_FLAG_ACTIVE
    call semantic_core_capability_add
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

; EAX/EDX=ObjectID, EBX=Namespace slot, ESI=name, ECX=flags.
semantic_core_object_add:
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
    mov [nova_object_temp_high], eax
    mov [nova_object_temp_low], edx
    mov [nova_object_temp_namespace], ebx
    mov [nova_object_temp_name], esi
    mov [nova_object_temp_flags], ecx
    mov ecx, [nova_object_count]
    cmp ecx, NOVA_OBJECT_CAPACITY
    jae .bad
    imul edi, ecx, NOVA_OBJECT_RECORD_SIZE
    add edi, nova_object_records
    mov eax, [nova_object_temp_high]
    mov [edi + NOVA_OBJECT_ID_HIGH], eax
    mov eax, [nova_object_temp_low]
    mov [edi + NOVA_OBJECT_ID_LOW], eax
    mov eax, [nova_object_temp_namespace]
    mov [edi + NOVA_OBJECT_NAMESPACE], eax
    mov eax, [nova_object_temp_name]
    mov [edi + NOVA_OBJECT_NAME], eax
    mov eax, [nova_object_temp_flags]
    mov [edi + NOVA_OBJECT_FLAGS], eax
    mov dword [edi + NOVA_OBJECT_SEM_HIGH], NOVA_ID_NAMESPACE_HIGH
    mov eax, [nova_object_temp_low]
    mov [edi + NOVA_OBJECT_SEM_LOW], eax
    mov dword [edi + NOVA_OBJECT_RESERVED], 0
    inc dword [nova_object_count]
    clc
    ret
.bad:
    stc
    ret

; EAX/EDX=ObjectID, EDI=object pointer oder CF=1.
semantic_core_object_lookup:
    test eax, eax
    jz .bad
    test edx, edx
    jz .bad
    mov [nova_object_lookup_high], eax
    mov [nova_object_lookup_low], edx
    xor esi, esi
.scan:
    cmp esi, [nova_object_count]
    jae .bad
    imul edi, esi, NOVA_OBJECT_RECORD_SIZE
    add edi, nova_object_records
    mov eax, [nova_object_lookup_high]
    cmp [edi + NOVA_OBJECT_ID_HIGH], eax
    jne .next
    mov eax, [nova_object_lookup_low]
    cmp [edi + NOVA_OBJECT_ID_LOW], eax
    jne .next
    clc
    ret
.next:
    inc esi
    jmp .scan
.bad:
    stc
    ret

; ECX=required flags oder 0, EAX=Anzahl.
semantic_core_object_count_by_flags:
    mov [nova_object_count_flags], ecx
    mov dword [nova_object_count_result], 0
    xor esi, esi
.scan:
    cmp esi, [nova_object_count]
    jae .done
    imul edi, esi, NOVA_OBJECT_RECORD_SIZE
    add edi, nova_object_records
    mov ecx, [nova_object_count_flags]
    test ecx, ecx
    jz .match
    mov eax, [edi + NOVA_OBJECT_FLAGS]
    and eax, ecx
    cmp eax, ecx
    jne .next
.match:
    inc dword [nova_object_count_result]
.next:
    inc esi
    jmp .scan
.done:
    mov eax, [nova_object_count_result]
    clc
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

; EBX=Namespace slot, EAX=Anzahl oder CF=1.
semantic_core_projection_count_by_namespace:
    cmp ebx, [nova_namespace_count]
    jae .bad
    mov [nova_projection_enum_namespace], ebx
    mov dword [nova_projection_enum_result], 0
    mov dword [nova_projection_enum_index], 0
.scan:
    mov eax, [nova_projection_enum_index]
    cmp eax, [nova_projection_count]
    jae .done
    imul edi, eax, NOVA_PROJECTION_RECORD_SIZE
    add edi, nova_projection_records
    mov eax, [nova_projection_enum_namespace]
    cmp [edi + NOVA_PROJECTION_NAMESPACE], eax
    jne .next
    inc dword [nova_projection_enum_result]
.next:
    inc dword [nova_projection_enum_index]
    jmp .scan
.done:
    mov eax, [nova_projection_enum_result]
    clc
    ret
.bad:
    stc
    ret

; EBX=Namespace slot, EDX=Ordinal, EDI=projection pointer oder CF=1.
semantic_core_projection_at_namespace:
    cmp ebx, [nova_namespace_count]
    jae .bad
    mov [nova_projection_enum_namespace], ebx
    mov [nova_projection_enum_ordinal], edx
    mov dword [nova_projection_enum_seen], 0
    mov dword [nova_projection_enum_index], 0
.scan:
    mov eax, [nova_projection_enum_index]
    cmp eax, [nova_projection_count]
    jae .bad
    imul edi, eax, NOVA_PROJECTION_RECORD_SIZE
    add edi, nova_projection_records
    mov eax, [nova_projection_enum_namespace]
    cmp [edi + NOVA_PROJECTION_NAMESPACE], eax
    jne .next
    mov eax, [nova_projection_enum_seen]
    cmp eax, [nova_projection_enum_ordinal]
    je .found
    inc dword [nova_projection_enum_seen]
.next:
    inc dword [nova_projection_enum_index]
    jmp .scan
.found:
    clc
    ret
.bad:
    stc
    ret

; EAX/EDX=CapabilityID, EBX/ECX=ObjectID, ESI=Namespace slot,
; EDI=rights, [ESP+4]=flags. Entfernt das Flag-Argument selbst.
semantic_core_capability_add:
    test eax, eax
    jz .bad
    test edx, edx
    jz .bad
    test ebx, ebx
    jz .bad
    test ecx, ecx
    jz .bad
    cmp esi, [nova_namespace_count]
    jae .bad
    test edi, edi
    jz .bad
    mov [nova_capability_temp_id_high], eax
    mov [nova_capability_temp_id_low], edx
    mov [nova_capability_temp_object_high], ebx
    mov [nova_capability_temp_object_low], ecx
    mov [nova_capability_temp_namespace], esi
    mov [nova_capability_temp_rights], edi
    mov eax, [esp + 4]
    mov [nova_capability_temp_flags], eax
    mov ecx, [nova_capability_count]
    cmp ecx, NOVA_CAPABILITY_CAPACITY
    jae .bad
    imul edi, ecx, NOVA_CAPABILITY_RECORD_SIZE
    add edi, nova_capability_records
    mov eax, [nova_capability_temp_id_high]
    mov [edi + NOVA_CAPABILITY_ID_HIGH], eax
    mov eax, [nova_capability_temp_id_low]
    mov [edi + NOVA_CAPABILITY_ID_LOW], eax
    mov eax, [nova_capability_temp_object_high]
    mov [edi + NOVA_CAPABILITY_OBJECT_HIGH], eax
    mov eax, [nova_capability_temp_object_low]
    mov [edi + NOVA_CAPABILITY_OBJECT_LOW], eax
    mov eax, [nova_capability_temp_namespace]
    mov [edi + NOVA_CAPABILITY_NAMESPACE], eax
    mov eax, [nova_capability_temp_rights]
    mov [edi + NOVA_CAPABILITY_RIGHTS], eax
    mov eax, [nova_capability_temp_flags]
    mov [edi + NOVA_CAPABILITY_FLAGS], eax
    mov dword [edi + NOVA_CAPABILITY_RESERVED], 0
    inc dword [nova_capability_count]
    clc
    ret 4
.bad:
    stc
    ret 4

; EAX/EDX=CapabilityID, EDI=capability pointer oder CF=1.
semantic_core_capability_lookup:
    test eax, eax
    jz .bad
    test edx, edx
    jz .bad
    mov [nova_capability_lookup_id_high], eax
    mov [nova_capability_lookup_id_low], edx
    xor esi, esi
.scan:
    cmp esi, [nova_capability_count]
    jae .bad
    imul edi, esi, NOVA_CAPABILITY_RECORD_SIZE
    add edi, nova_capability_records
    test dword [edi + NOVA_CAPABILITY_FLAGS], NOVA_CAP_FLAG_ACTIVE
    jz .next
    mov eax, [nova_capability_lookup_id_high]
    cmp [edi + NOVA_CAPABILITY_ID_HIGH], eax
    jne .next
    mov eax, [nova_capability_lookup_id_low]
    cmp [edi + NOVA_CAPABILITY_ID_LOW], eax
    jne .next
    clc
    ret
.next:
    inc esi
    jmp .scan
.bad:
    stc
    ret

; EAX/EDX=CapabilityID. Markiert Capability inaktiv oder CF=1.
semantic_core_capability_revoke:
    call semantic_core_capability_lookup
    jc .bad
    and dword [edi + NOVA_CAPABILITY_FLAGS], ~NOVA_CAP_FLAG_ACTIVE
    clc
    ret
.bad:
    stc
    ret

; EAX/EDX=ObjectID, EBX=requested rights, EDI=capability pointer oder CF=1.
semantic_core_capability_check_object_rights:
    test eax, eax
    jz .bad
    test edx, edx
    jz .bad
    test ebx, ebx
    jz .bad
    mov [nova_capability_lookup_object_high], eax
    mov [nova_capability_lookup_object_low], edx
    mov [nova_capability_lookup_rights], ebx
    xor esi, esi
.scan:
    cmp esi, [nova_capability_count]
    jae .bad
    imul edi, esi, NOVA_CAPABILITY_RECORD_SIZE
    add edi, nova_capability_records
    test dword [edi + NOVA_CAPABILITY_FLAGS], NOVA_CAP_FLAG_ACTIVE
    jz .next
    mov eax, [nova_capability_lookup_object_high]
    cmp [edi + NOVA_CAPABILITY_OBJECT_HIGH], eax
    jne .next
    mov eax, [nova_capability_lookup_object_low]
    cmp [edi + NOVA_CAPABILITY_OBJECT_LOW], eax
    jne .next
    mov eax, [edi + NOVA_CAPABILITY_RIGHTS]
    mov ebx, [nova_capability_lookup_rights]
    and eax, ebx
    cmp eax, ebx
    jne .next
    clc
    ret
.next:
    inc esi
    jmp .scan
.bad:
    stc
    ret

; EAX/EDX=ObjectID, EBX=rights, EAX=HandleID und EDI=handle pointer oder CF=1.
semantic_core_handle_open_by_object:
    test eax, eax
    jz .bad
    test edx, edx
    jz .bad
    test ebx, ebx
    jz .bad
    mov [nova_handle_temp_object_high], eax
    mov [nova_handle_temp_object_low], edx
    mov [nova_handle_temp_rights], ebx
    call semantic_core_capability_check_object_rights
    jc .bad
    mov eax, [nova_handle_temp_object_high]
    mov edx, [nova_handle_temp_object_low]
    call semantic_core_projection_lookup_by_object
    jc .bad
    mov eax, [edi + NOVA_PROJECTION_NAMESPACE]
    mov [nova_handle_temp_namespace], eax
    xor esi, esi
.reuse_scan:
    cmp esi, [nova_handle_count]
    jae .append
    imul edi, esi, NOVA_HANDLE_RECORD_SIZE
    add edi, nova_handle_records
    test dword [edi + NOVA_HANDLE_FLAGS], NOVA_HANDLE_FLAG_ACTIVE
    jz .fill
    inc esi
    jmp .reuse_scan
.append:
    mov ecx, [nova_handle_count]
    cmp ecx, NOVA_HANDLE_CAPACITY
    jae .bad
    imul edi, ecx, NOVA_HANDLE_RECORD_SIZE
    add edi, nova_handle_records
    inc dword [nova_handle_count]
.fill:
    mov eax, [nova_handle_next_id]
    mov [edi + NOVA_HANDLE_ID], eax
    mov eax, [nova_handle_temp_object_high]
    mov [edi + NOVA_HANDLE_OBJECT_HIGH], eax
    mov eax, [nova_handle_temp_object_low]
    mov [edi + NOVA_HANDLE_OBJECT_LOW], eax
    mov eax, [nova_handle_temp_namespace]
    mov [edi + NOVA_HANDLE_NAMESPACE], eax
    mov eax, [nova_handle_temp_rights]
    mov [edi + NOVA_HANDLE_RIGHTS], eax
    mov dword [edi + NOVA_HANDLE_FLAGS], NOVA_HANDLE_FLAG_ACTIVE
    mov eax, [nova_handle_generation]
    mov [edi + NOVA_HANDLE_GENERATION], eax
    mov dword [edi + NOVA_HANDLE_RESERVED], 0
    mov eax, [edi + NOVA_HANDLE_ID]
    inc dword [nova_handle_next_id]
    inc dword [nova_handle_generation]
    clc
    ret
.bad:
    stc
    ret

; EAX=HandleID, EDI=handle pointer oder CF=1.
semantic_core_handle_lookup:
    test eax, eax
    jz .bad
    mov [nova_handle_lookup_id], eax
    xor esi, esi
.scan:
    cmp esi, [nova_handle_count]
    jae .bad
    imul edi, esi, NOVA_HANDLE_RECORD_SIZE
    add edi, nova_handle_records
    mov eax, [nova_handle_lookup_id]
    cmp [edi + NOVA_HANDLE_ID], eax
    jne .next
    test dword [edi + NOVA_HANDLE_FLAGS], NOVA_HANDLE_FLAG_ACTIVE
    jz .bad
    clc
    ret
.next:
    inc esi
    jmp .scan
.bad:
    stc
    ret

; EAX=HandleID, EBX=requested rights, EDI=handle pointer oder CF=1.
semantic_core_handle_validate_rights:
    test ebx, ebx
    jz .bad
    mov [nova_handle_validate_rights], ebx
    call semantic_core_handle_lookup
    jc .bad
    mov eax, [edi + NOVA_HANDLE_RIGHTS]
    mov ebx, [nova_handle_validate_rights]
    and eax, ebx
    cmp eax, ebx
    jne .bad
    mov eax, [edi + NOVA_HANDLE_OBJECT_HIGH]
    mov edx, [edi + NOVA_HANDLE_OBJECT_LOW]
    mov ebx, [nova_handle_validate_rights]
    call semantic_core_capability_check_object_rights
    jc .bad
    clc
    ret
.bad:
    stc
    ret

; EAX=HandleID. Markiert Handle ungueltig oder CF=1.
semantic_core_handle_close:
    call semantic_core_handle_lookup
    jc .bad
    and dword [edi + NOVA_HANDLE_FLAGS], ~NOVA_HANDLE_FLAG_ACTIVE
    clc
    ret
.bad:
    stc
    ret

; ESI=absoluter Pfad, EDI=namespace record oder CF=1.
; Fruehe Bootstrap-Variante: "/", "/System", "/Benutzer", "/Apps",
; "/Volumes" und "/Boot" als ein Segment unter Root.
semantic_core_namespace_resolve_path:
    test esi, esi
    jz .bad
    cmp byte [esi], '/'
    jne .bad
    cmp byte [esi + 1], 0
    jne .child
    mov esi, NOVA_NAMESPACE_ROOT
    call semantic_core_namespace_record
    ret
.child:
    lea eax, [esi + 1]
    mov [nova_namespace_resolve_name], eax
    mov dword [nova_namespace_resolve_index], 0
.scan:
    mov eax, [nova_namespace_resolve_index]
    cmp eax, [nova_namespace_count]
    jae .bad
    imul edi, eax, NOVA_NAMESPACE_RECORD_SIZE
    add edi, nova_namespace_records
    cmp dword [edi + NOVA_NAMESPACE_PARENT], NOVA_NAMESPACE_ROOT
    jne .next
    mov esi, [nova_namespace_resolve_name]
    mov edx, [edi + NOVA_NAMESPACE_NAME]
    call semantic_core_string_equals
    jnc .found
.next:
    inc dword [nova_namespace_resolve_index]
    jmp .scan
.found:
    clc
    ret
.bad:
    stc
    ret

; EBX=parent slot, ECX=required flags oder 0, EAX=Anzahl oder CF=1.
semantic_core_namespace_count_children:
    cmp ebx, [nova_namespace_count]
    jae .bad
    mov [nova_namespace_count_parent], ebx
    mov [nova_namespace_count_flags], ecx
    mov dword [nova_namespace_count_result], 0
    mov dword [nova_namespace_count_index], 0
.scan:
    mov eax, [nova_namespace_count_index]
    cmp eax, [nova_namespace_count]
    jae .done
    imul edi, eax, NOVA_NAMESPACE_RECORD_SIZE
    add edi, nova_namespace_records
    mov eax, [nova_namespace_count_parent]
    cmp [edi + NOVA_NAMESPACE_PARENT], eax
    jne .next
    mov ecx, [nova_namespace_count_flags]
    test ecx, ecx
    jz .match
    mov eax, [edi + NOVA_NAMESPACE_FLAGS]
    and eax, ecx
    cmp eax, ecx
    jne .next
.match:
    inc dword [nova_namespace_count_result]
.next:
    inc dword [nova_namespace_count_index]
    jmp .scan
.done:
    mov eax, [nova_namespace_count_result]
    clc
    ret
.bad:
    stc
    ret

; EBX=parent slot, ECX=required flags oder 0, EDX=Ordinal innerhalb Filter,
; EDI=namespace record oder CF=1.
semantic_core_namespace_child_at:
    cmp ebx, [nova_namespace_count]
    jae .bad
    mov [nova_namespace_child_parent], ebx
    mov [nova_namespace_child_flags], ecx
    mov [nova_namespace_child_ordinal], edx
    mov dword [nova_namespace_child_seen], 0
    mov dword [nova_namespace_child_index], 0
.scan:
    mov eax, [nova_namespace_child_index]
    cmp eax, [nova_namespace_count]
    jae .bad
    imul edi, eax, NOVA_NAMESPACE_RECORD_SIZE
    add edi, nova_namespace_records
    mov eax, [nova_namespace_child_parent]
    cmp [edi + NOVA_NAMESPACE_PARENT], eax
    jne .next
    mov ecx, [nova_namespace_child_flags]
    test ecx, ecx
    jz .candidate
    mov eax, [edi + NOVA_NAMESPACE_FLAGS]
    and eax, ecx
    cmp eax, ecx
    jne .next
.candidate:
    mov eax, [nova_namespace_child_seen]
    cmp eax, [nova_namespace_child_ordinal]
    je .found
    inc dword [nova_namespace_child_seen]
.next:
    inc dword [nova_namespace_child_index]
    jmp .scan
.found:
    clc
    ret
.bad:
    stc
    ret

; ESI=linke nullterminierte Zeichenkette, EDX=rechte Zeichenkette.
; CF=0 bei Gleichheit, CF=1 bei Unterschied.
semantic_core_string_equals:
    test esi, esi
    jz .bad
    test edx, edx
    jz .bad
.loop:
    mov al, [esi]
    mov bl, [edx]
    cmp al, bl
    jne .bad
    test al, al
    jz .ok
    inc esi
    inc edx
    jmp .loop
.ok:
    clc
    ret
.bad:
    stc
    ret

; ESI=absoluter Namespace-Pfad, EBX=rights,
; EAX=HandleID und EDI=handle pointer oder CF=1.
semantic_core_handle_open_by_path:
    test esi, esi
    jz .bad
    test ebx, ebx
    jz .bad
    mov [nova_handle_path_rights], ebx
    call semantic_core_namespace_resolve_path
    jc .bad
    mov eax, [edi + NOVA_NAMESPACE_OBJECT_HIGH]
    mov edx, [edi + NOVA_NAMESPACE_OBJECT_LOW]
    mov ebx, [nova_handle_path_rights]
    call semantic_core_handle_open_by_object
    jc .bad
    clc
    ret
.bad:
    stc
    ret

semantic_core_self_test:
    cmp dword [nova_registry_count], 5
    jne .bad
    cmp dword [nova_object_count], 6
    jne .bad
    cmp dword [nova_namespace_count], 6
    jne .bad
    cmp dword [nova_projection_count], 6
    jne .bad
    cmp dword [nova_capability_count], 4
    jne .bad
    cmp dword [nova_handle_count], 0
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
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 4
    call semantic_core_object_lookup
    jc .bad
    cmp dword [edi + NOVA_OBJECT_NAMESPACE], NOVA_NAMESPACE_APPS
    jne .bad
    test dword [edi + NOVA_OBJECT_FLAGS], NOVA_OBJECT_FLAG_USER_VISIBLE
    jz .bad
    cmp dword [edi + NOVA_OBJECT_SEM_HIGH], NOVA_ID_NAMESPACE_HIGH
    jne .bad
    cmp dword [edi + NOVA_OBJECT_SEM_LOW], 4
    jne .bad
    mov ecx, NOVA_OBJECT_FLAG_SYSTEM
    call semantic_core_object_count_by_flags
    jc .bad
    cmp eax, 3
    jne .bad
    mov ecx, NOVA_OBJECT_FLAG_USER_VISIBLE
    call semantic_core_object_count_by_flags
    jc .bad
    cmp eax, 3
    jne .bad
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 0x7000
    call semantic_core_object_lookup
    jnc .bad
    mov eax, NOVA_ID_NAMESPACE_HIGH
    mov edx, 2
    call semantic_core_namespace_lookup
    jc .bad
    cmp dword [edi + NOVA_NAMESPACE_NAME], nova_ns_name_system
    jne .bad
    mov esi, nova_ns_path_root
    call semantic_core_namespace_resolve_path
    jc .bad
    cmp dword [edi + NOVA_NAMESPACE_ID_LOW], 1
    jne .bad
    mov esi, nova_ns_path_system
    call semantic_core_namespace_resolve_path
    jc .bad
    cmp dword [edi + NOVA_NAMESPACE_OBJECT_LOW], 2
    jne .bad
    mov esi, nova_ns_path_apps
    call semantic_core_namespace_resolve_path
    jc .bad
    cmp dword [edi + NOVA_NAMESPACE_OBJECT_LOW], 4
    jne .bad
    mov esi, nova_ns_path_missing
    call semantic_core_namespace_resolve_path
    jnc .bad
    mov ebx, NOVA_NAMESPACE_ROOT
    xor ecx, ecx
    call semantic_core_namespace_count_children
    jc .bad
    cmp eax, 5
    jne .bad
    mov ebx, NOVA_NAMESPACE_ROOT
    mov ecx, NOVA_NS_FLAG_USER_VISIBLE
    call semantic_core_namespace_count_children
    jc .bad
    cmp eax, 3
    jne .bad
    mov ebx, NOVA_NAMESPACE_ROOT
    mov ecx, NOVA_NS_FLAG_SYSTEM
    call semantic_core_namespace_count_children
    jc .bad
    cmp eax, 2
    jne .bad
    mov ebx, NOVA_NAMESPACE_ROOT
    mov ecx, NOVA_NS_FLAG_USER_VISIBLE
    xor edx, edx
    call semantic_core_namespace_child_at
    jc .bad
    cmp dword [edi + NOVA_NAMESPACE_OBJECT_LOW], 3
    jne .bad
    mov ebx, NOVA_NAMESPACE_ROOT
    mov ecx, NOVA_NS_FLAG_USER_VISIBLE
    mov edx, 2
    call semantic_core_namespace_child_at
    jc .bad
    cmp dword [edi + NOVA_NAMESPACE_OBJECT_LOW], 5
    jne .bad
    mov ebx, NOVA_NAMESPACE_ROOT
    mov ecx, NOVA_NS_FLAG_USER_VISIBLE
    mov edx, 3
    call semantic_core_namespace_child_at
    jnc .bad
    mov ebx, NOVA_NAMESPACE_ROOT
    xor ecx, ecx
    mov edx, 4
    call semantic_core_namespace_child_at
    jc .bad
    cmp dword [edi + NOVA_NAMESPACE_OBJECT_LOW], 6
    jne .bad
    mov esi, nova_ns_path_system
    mov ebx, NOVA_HANDLE_RIGHT_READ
    call semantic_core_handle_open_by_path
    jc .bad
    mov [nova_handle_path_selftest_id], eax
    cmp dword [edi + NOVA_HANDLE_OBJECT_LOW], 2
    jne .bad
    mov eax, [nova_handle_path_selftest_id]
    mov ebx, NOVA_HANDLE_RIGHT_READ
    call semantic_core_handle_validate_rights
    jc .bad
    mov eax, [nova_handle_path_selftest_id]
    call semantic_core_handle_close
    jc .bad
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
    mov ebx, NOVA_NAMESPACE_SYSTEM
    call semantic_core_projection_count_by_namespace
    jc .bad
    cmp eax, 1
    jne .bad
    mov ebx, NOVA_NAMESPACE_ROOT
    call semantic_core_projection_count_by_namespace
    jc .bad
    cmp eax, 1
    jne .bad
    mov ebx, NOVA_NAMESPACE_APPS
    xor edx, edx
    call semantic_core_projection_at_namespace
    jc .bad
    cmp dword [edi + NOVA_PROJECTION_OBJECT_LOW], 4
    jne .bad
    mov ebx, NOVA_NAMESPACE_APPS
    mov edx, 1
    call semantic_core_projection_at_namespace
    jnc .bad
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 0x7000
    call semantic_core_projection_lookup_by_object
    jnc .bad
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 2
    mov ebx, NOVA_HANDLE_RIGHT_READ
    call semantic_core_capability_check_object_rights
    jc .bad
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 2
    mov ebx, NOVA_HANDLE_RIGHT_WRITE
    call semantic_core_capability_check_object_rights
    jnc .bad
    mov eax, NOVA_ID_CAPABILITY_HIGH
    mov edx, 4
    call semantic_core_capability_lookup
    jc .bad
    cmp dword [edi + NOVA_CAPABILITY_NAMESPACE], NOVA_NAMESPACE_VOLUMES
    jne .bad
    test dword [edi + NOVA_CAPABILITY_RIGHTS], NOVA_HANDLE_RIGHT_READ
    jz .bad
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 5
    mov ebx, NOVA_HANDLE_RIGHT_READ
    call semantic_core_capability_check_object_rights
    jc .bad
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 5
    mov ebx, NOVA_HANDLE_RIGHT_READ
    call semantic_core_handle_open_by_object
    jc .bad
    mov [nova_handle_revoke_selftest_id], eax
    mov eax, [nova_handle_revoke_selftest_id]
    mov ebx, NOVA_HANDLE_RIGHT_READ
    call semantic_core_handle_validate_rights
    jc .bad
    mov eax, NOVA_ID_CAPABILITY_HIGH
    mov edx, 4
    call semantic_core_capability_revoke
    jc .bad
    mov eax, NOVA_ID_CAPABILITY_HIGH
    mov edx, 4
    call semantic_core_capability_lookup
    jnc .bad
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 5
    mov ebx, NOVA_HANDLE_RIGHT_READ
    call semantic_core_capability_check_object_rights
    jnc .bad
    mov eax, [nova_handle_revoke_selftest_id]
    mov ebx, NOVA_HANDLE_RIGHT_READ
    call semantic_core_handle_validate_rights
    jnc .bad
    mov eax, [nova_handle_revoke_selftest_id]
    call semantic_core_handle_close
    jc .bad
    mov eax, NOVA_ID_OBJECT_HIGH
    mov edx, 2
    mov ebx, NOVA_HANDLE_RIGHT_READ
    call semantic_core_handle_open_by_object
    jc .bad
    mov [nova_handle_selftest_id], eax
    cmp dword [edi + NOVA_HANDLE_NAMESPACE], NOVA_NAMESPACE_SYSTEM
    jne .bad
    test dword [edi + NOVA_HANDLE_RIGHTS], NOVA_HANDLE_RIGHT_READ
    jz .bad
    mov eax, [nova_handle_selftest_id]
    call semantic_core_handle_lookup
    jc .bad
    cmp dword [edi + NOVA_HANDLE_OBJECT_LOW], 2
    jne .bad
    mov eax, [nova_handle_selftest_id]
    call semantic_core_handle_close
    jc .bad
    mov eax, [nova_handle_selftest_id]
    call semantic_core_handle_lookup
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
nova_object_count:            dd 0
nova_namespace_count:         dd 0
nova_projection_count:        dd 0
nova_capability_count:        dd 0
nova_handle_count:            dd 0
nova_handle_next_id:          dd 0
nova_handle_generation:       dd 0
nova_object_id_counter:       dd 0
nova_volume_id_counter:       dd 0
nova_namespace_id_counter:    dd 0
nova_capability_id_counter:   dd 0
nova_semantic_type_id_counter: dd 0
nova_registry_temp_high:      dd 0
nova_registry_temp_low:       dd 0
nova_registry_temp_kind:      dd 0
nova_registry_temp_name:      dd 0
nova_object_temp_high:        dd 0
nova_object_temp_low:         dd 0
nova_object_temp_namespace:   dd 0
nova_object_temp_name:        dd 0
nova_object_temp_flags:       dd 0
nova_object_lookup_high:      dd 0
nova_object_lookup_low:       dd 0
nova_object_count_flags:      dd 0
nova_object_count_result:     dd 0
nova_namespace_temp_high:     dd 0
nova_namespace_temp_low:      dd 0
nova_namespace_temp_parent:   dd 0
nova_namespace_temp_name:     dd 0
nova_namespace_temp_flags:    dd 0
nova_namespace_temp_object_high: dd 0
nova_namespace_temp_object_low:  dd 0
nova_namespace_resolve_name:  dd 0
nova_namespace_resolve_index: dd 0
nova_namespace_count_parent:  dd 0
nova_namespace_count_flags:   dd 0
nova_namespace_count_index:   dd 0
nova_namespace_count_result:  dd 0
nova_namespace_child_parent:  dd 0
nova_namespace_child_flags:   dd 0
nova_namespace_child_ordinal: dd 0
nova_namespace_child_seen:    dd 0
nova_namespace_child_index:   dd 0
nova_projection_temp_object_high: dd 0
nova_projection_temp_object_low:  dd 0
nova_projection_temp_namespace:   dd 0
nova_projection_temp_name:        dd 0
nova_projection_temp_flags:       dd 0
nova_projection_temp_target_high: dd 0
nova_projection_temp_target_low:  dd 0
nova_projection_lookup_high:      dd 0
nova_projection_lookup_low:       dd 0
nova_projection_enum_namespace:   dd 0
nova_projection_enum_index:       dd 0
nova_projection_enum_result:      dd 0
nova_projection_enum_ordinal:     dd 0
nova_projection_enum_seen:        dd 0
nova_capability_temp_id_high:     dd 0
nova_capability_temp_id_low:      dd 0
nova_capability_temp_object_high: dd 0
nova_capability_temp_object_low:  dd 0
nova_capability_temp_namespace:   dd 0
nova_capability_temp_rights:      dd 0
nova_capability_temp_flags:       dd 0
nova_capability_lookup_object_high: dd 0
nova_capability_lookup_object_low:  dd 0
nova_capability_lookup_rights:      dd 0
nova_capability_lookup_id_high:     dd 0
nova_capability_lookup_id_low:      dd 0
nova_handle_temp_object_high:     dd 0
nova_handle_temp_object_low:      dd 0
nova_handle_temp_namespace:       dd 0
nova_handle_temp_rights:          dd 0
nova_handle_lookup_id:            dd 0
nova_handle_validate_rights:      dd 0
nova_handle_path_rights:          dd 0
nova_handle_selftest_id:          dd 0
nova_handle_revoke_selftest_id:   dd 0
nova_handle_path_selftest_id:     dd 0

nova_registry_records:
    times NOVA_REGISTRY_CAPACITY * NOVA_REGISTRY_RECORD_SIZE db 0
nova_object_records:
    times NOVA_OBJECT_CAPACITY * NOVA_OBJECT_RECORD_SIZE db 0
nova_namespace_records:
    times NOVA_NAMESPACE_CAPACITY * NOVA_NAMESPACE_RECORD_SIZE db 0
nova_projection_records:
    times NOVA_PROJECTION_CAPACITY * NOVA_PROJECTION_RECORD_SIZE db 0
nova_capability_records:
    times NOVA_CAPABILITY_CAPACITY * NOVA_CAPABILITY_RECORD_SIZE db 0
nova_handle_records:
    times NOVA_HANDLE_CAPACITY * NOVA_HANDLE_RECORD_SIZE db 0

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
nova_ns_path_root:
    db '/', 0
nova_ns_path_system:
    db '/System', 0
nova_ns_path_apps:
    db '/Apps', 0
nova_ns_path_missing:
    db '/NichtVorhanden', 0
