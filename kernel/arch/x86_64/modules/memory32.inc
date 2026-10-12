; ---------------------------------------------------------------------------
; Physischer Bootstrap-Speichermanager (ADR-2001 / NPSPEC-KERNEL-0006)
; NPSPEC-MEMORY-PROTECTION-0001 / NPSPEC-MEMORY-SHARED-0001 / NPSPEC-MEMORY-TLB-0001
; ---------------------------------------------------------------------------

PMM_PAGE_SIZE        equ 4096
PMM_MAX_FRAMES       equ 1024
PMM_API_SIZE         equ 48
PMM_API_ABI_MAJOR    equ 1
PMM_API_ABI_MINOR    equ 1
PMM_CAP_E820         equ 0x00000001
PMM_CAP_LIFO_FRAMES  equ 0x00000002
PMM_CAP_NUMA_TAGGED  equ 0x00000004
PMM_CAP_NUMA_PREFERRED equ 0x00000008
PMM_CAP_NUMA_STRICT  equ 0x00000010
PMM_NUMA_UNKNOWN     equ 0xFFFFFFFF

PMM_API_STRUCT_SIZE  equ 0
PMM_API_ABI          equ 4
PMM_API_PAGE_SIZE    equ 8
PMM_API_CAPABILITIES equ 12
PMM_API_ALLOC        equ 16
PMM_API_FREE         equ 20
PMM_API_TOTAL        equ 24
PMM_API_AVAILABLE    equ 28
PMM_API_ALLOC_PREFERRED equ 32
PMM_API_ALLOC_STRICT equ 36
PMM_API_NODE_FOR_PAGE equ 40
PMM_API_NUMA_UNKNOWN equ 44

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
    push eax
    call acpi_srat_memory_domain_lookup
    mov [pmm_frame_numa_nodes + edi * 4], eax
    pop eax
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

; EAX = physische 4-KiB-Seite, EDX = NUMA-Node oder PMM_NUMA_UNKNOWN.
; EAX=0 meldet Erschöpfung.
pmm_alloc_page:
    mov ecx, [pmm_frame_count]
    test ecx, ecx
    jz .empty
    dec ecx
    mov [pmm_frame_count], ecx
    mov eax, [pmm_frames + ecx * 4]
    mov edx, [pmm_frame_numa_nodes + ecx * 4]
    mov [pmm_api + PMM_API_AVAILABLE], ecx
    ret
.empty:
    xor eax, eax
    mov edx, PMM_NUMA_UNKNOWN
    ret

; EAX = gewünschter NUMA-Node. Bevorzugte Allokation fällt kontrolliert auf
; eine beliebige Seite zurück; EDX liefert immer die tatsächliche Lokalität.
pmm_alloc_page_preferred:
    push eax
    call pmm_alloc_page_strict
    test eax, eax
    jnz .found
    pop eax
    jmp pmm_alloc_page
.found:
    add esp, 4
    ret

; EAX = gewünschter NUMA-Node. Strikte Allokation liefert ausschließlich eine
; lokal getaggte Seite. Die Arrays bleiben durch Swap-with-last kompakt.
pmm_alloc_page_strict:
    mov edx, eax
    mov ecx, [pmm_frame_count]
.scan:
    test ecx, ecx
    jz .empty
    dec ecx
    cmp [pmm_frame_numa_nodes + ecx * 4], edx
    jne .scan
    mov eax, [pmm_frames + ecx * 4]
    mov ebx, [pmm_frame_count]
    dec ebx
    cmp ecx, ebx
    je .remove
    mov esi, [pmm_frames + ebx * 4]
    mov [pmm_frames + ecx * 4], esi
    mov esi, [pmm_frame_numa_nodes + ebx * 4]
    mov [pmm_frame_numa_nodes + ecx * 4], esi
.remove:
    mov [pmm_frame_count], ebx
    mov [pmm_api + PMM_API_AVAILABLE], ebx
    ret
.empty:
    xor eax, eax
    mov edx, PMM_NUMA_UNKNOWN
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
    push eax
    call acpi_srat_memory_domain_lookup
    mov [pmm_frame_numa_nodes + ecx * 4], eax
    pop eax
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
    ; Ist eine bekannte Lokalität vorhanden, müssen Strict, Preferred-Fallback
    ; und die Rückgabe des tatsächlichen Nodes deterministisch funktionieren.
    xor ecx, ecx
.numa_scan:
    cmp ecx, [pmm_frame_count]
    jae .complete
    mov eax, [pmm_frame_numa_nodes + ecx * 4]
    cmp eax, PMM_NUMA_UNKNOWN
    jne .numa_known
    inc ecx
    jmp .numa_scan
.numa_known:
    mov esi, eax
    call pmm_alloc_page_strict
    test eax, eax
    jz .invalid
    mov edi, eax
    cmp edx, esi
    jne .restore_numa_invalid
    mov eax, 0xFFFFFFFE
    call pmm_alloc_page_strict
    test eax, eax
    jnz .restore_numa_invalid
    mov eax, 0xFFFFFFFE
    call pmm_alloc_page_preferred
    test eax, eax
    jz .restore_numa_invalid
    mov ebx, eax
    mov eax, ebx
    call pmm_free_page
    jc .restore_numa_invalid
    mov eax, edi
    call pmm_free_page
    jc .invalid
    cmp [pmm_frame_count], ebp
    jne .invalid
.complete:
    clc
    ret
.restore_numa_invalid:
    mov eax, edi
    call pmm_free_page
    stc
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
    dd PMM_CAP_E820 | PMM_CAP_LIFO_FRAMES | PMM_CAP_NUMA_TAGGED | PMM_CAP_NUMA_PREFERRED | PMM_CAP_NUMA_STRICT
    dd pmm_alloc_page
    dd pmm_free_page
    dd 0
    dd 0
    dd pmm_alloc_page_preferred
    dd pmm_alloc_page_strict
    dd acpi_srat_memory_domain_lookup
    dd PMM_NUMA_UNKNOWN

pmm_reserved_end: dd 0
pmm_frame_count:  dd 0
align 16
pmm_frames:
    times PMM_MAX_FRAMES dd 0
pmm_frame_numa_nodes:
    times PMM_MAX_FRAMES dd PMM_NUMA_UNKNOWN

; ---------------------------------------------------------------------------
; Bootstrap-Kernel-Heap (ADR-2003 / NPSPEC-KERNEL-0008)
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

; ===========================================================================
; NPSPEC-MEMORY-SLAB-0001 – Nova Slab Allocator
; 4 feste Objekt-Klassen (16/32/64/128 Byte); je 16 Objekte pro Slab;
; alloc = pop aus freier Liste; free = push zurück.
; ===========================================================================

SLAB_CLASS_COUNT    equ 4
SLAB_OBJECTS_PER    equ 16

; Klasse 0: 16 Byte, Klasse 1: 32 Byte, Klasse 2: 64 Byte, Klasse 3: 128 Byte
; Gesamtspeicher pro Klasse: 16*16=256, 16*32=512, 16*64=1024, 16*128=2048 = 3840 Byte

SLAB_SIZE_0         equ 16
SLAB_SIZE_1         equ 32
SLAB_SIZE_2         equ 64
SLAB_SIZE_3         equ 128

; Klassen-Deskriptor (je 24 Byte):
SLABCLS_OBJ_SIZE    equ 0    ; dd Objektgröße
SLABCLS_FREE_HEAD   equ 4    ; dd Zeiger auf erste freie Zelle (0=leer)
SLABCLS_ALLOC_COUNT equ 8    ; dd Anzahl ausgelagerter Objekte
SLABCLS_FREE_COUNT  equ 12   ; dd Anzahl freier Objekte
SLABCLS_TOTAL       equ 16   ; dd Gesamtanzahl jemals alloziert
SLABCLS_PAD         equ 20   ; dd (alignment)
SLABCLS_SIZE        equ 24

slab_initialize:
    ; Klassen-Deskriptoren initialisieren
    mov dword [slab_classes + 0*SLABCLS_SIZE + SLABCLS_OBJ_SIZE], SLAB_SIZE_0
    mov dword [slab_classes + 1*SLABCLS_SIZE + SLABCLS_OBJ_SIZE], SLAB_SIZE_1
    mov dword [slab_classes + 2*SLABCLS_SIZE + SLABCLS_OBJ_SIZE], SLAB_SIZE_2
    mov dword [slab_classes + 3*SLABCLS_SIZE + SLABCLS_OBJ_SIZE], SLAB_SIZE_3
    ; Freie Listen aufbauen
    push ebx
    push esi
    push edi
    push ecx
    xor ebx, ebx    ; Klassen-Index
.si_class_loop:
    cmp ebx, SLAB_CLASS_COUNT
    jae .si_done
    ; Slab-Pool dieser Klasse verlinken
    imul esi, ebx, SLABCLS_SIZE
    add esi, slab_classes
    ; Pool-Basis berechnen
    call slab_get_pool_base   ; EBX=class → EDI=base, ECX=obj_size
    ; freie Liste als intrusive Linked List aufbauen
    mov ecx, SLAB_OBJECTS_PER
    mov eax, edi
.si_link:
    dec ecx
    jz .si_last
    push ecx
    lea ecx, [eax + edx]    ; next ptr = current + obj_size
    mov [eax], ecx           ; *current = next
    add eax, edx
    pop ecx
    jmp .si_link
.si_last:
    mov dword [eax], 0       ; letzter Eintrag: NULL
    mov [esi + SLABCLS_FREE_HEAD], edi
    mov dword [esi + SLABCLS_FREE_COUNT], SLAB_OBJECTS_PER
    mov dword [esi + SLABCLS_ALLOC_COUNT], 0
    mov dword [esi + SLABCLS_TOTAL], 0
    mov dword [esi + SLABCLS_PAD], 0
    inc ebx
    jmp .si_class_loop
.si_done:
    mov dword [slab_ready], 1
    clc
    pop ecx
    pop edi
    pop esi
    pop ebx
    ret

; intern: EBX=class_index → EDI=pool_base, EDX=obj_size
slab_get_pool_base:
    cmp ebx, 0
    jne .sgpb_1
    mov edi, slab_pool_0
    mov edx, SLAB_SIZE_0
    ret
.sgpb_1:
    cmp ebx, 1
    jne .sgpb_2
    mov edi, slab_pool_1
    mov edx, SLAB_SIZE_1
    ret
.sgpb_2:
    cmp ebx, 2
    jne .sgpb_3
    mov edi, slab_pool_2
    mov edx, SLAB_SIZE_2
    ret
.sgpb_3:
    mov edi, slab_pool_3
    mov edx, SLAB_SIZE_3
    ret

; EAX=gewünschte Größe → EAX=Zeiger auf Objekt CF=0 / CF=1 (OOM oder zu groß)
slab_alloc:
    push esi
    push edx
    ; Klasse bestimmen
    cmp eax, SLAB_SIZE_0
    jbe .sla_cls0
    cmp eax, SLAB_SIZE_1
    jbe .sla_cls1
    cmp eax, SLAB_SIZE_2
    jbe .sla_cls2
    cmp eax, SLAB_SIZE_3
    jbe .sla_cls3
    stc
    pop edx
    pop esi
    ret
.sla_cls0: mov edx, 0
    jmp .sla_alloc
.sla_cls1: mov edx, 1
    jmp .sla_alloc
.sla_cls2: mov edx, 2
    jmp .sla_alloc
.sla_cls3: mov edx, 3
.sla_alloc:
    imul esi, edx, SLABCLS_SIZE
    add esi, slab_classes
    mov eax, [esi + SLABCLS_FREE_HEAD]
    test eax, eax
    jz .sla_oom
    mov edx, [eax]           ; next freie Zelle
    mov [esi + SLABCLS_FREE_HEAD], edx
    dec dword [esi + SLABCLS_FREE_COUNT]
    inc dword [esi + SLABCLS_ALLOC_COUNT]
    inc dword [esi + SLABCLS_TOTAL]
    clc
    pop edx
    pop esi
    ret
.sla_oom:
    stc
    pop edx
    pop esi
    ret

; EAX=Zeiger  EDX=Größe → gibt Objekt zurück in passende Klasse
slab_free:
    push esi
    push ecx
    ; Klasse bestimmen
    cmp edx, SLAB_SIZE_0
    jbe .slf_cls0
    cmp edx, SLAB_SIZE_1
    jbe .slf_cls1
    cmp edx, SLAB_SIZE_2
    jbe .slf_cls2
    jmp .slf_cls3
.slf_cls0: mov ecx, 0
    jmp .slf_free
.slf_cls1: mov ecx, 1
    jmp .slf_free
.slf_cls2: mov ecx, 2
    jmp .slf_free
.slf_cls3: mov ecx, 3
.slf_free:
    imul esi, ecx, SLABCLS_SIZE
    add esi, slab_classes
    mov ecx, [esi + SLABCLS_FREE_HEAD]
    mov [eax], ecx           ; next = alte free_head
    mov [esi + SLABCLS_FREE_HEAD], eax
    inc dword [esi + SLABCLS_FREE_COUNT]
    dec dword [esi + SLABCLS_ALLOC_COUNT]
    clc
    pop ecx
    pop esi
    ret

slab_self_test:
    push ebx
    push esi
    xor esi, esi

    ; T1: ready
    cmp dword [slab_ready], 1
    je .slst1_ok
    inc esi
.slst1_ok:

    ; T2: alloc Klasse 0 (16 Byte)
    mov eax, 16
    call slab_alloc
    jnc .slst2_ok
    inc esi
    jmp .slst2_fail
.slst2_ok:
    mov ebx, eax        ; Zeiger merken
.slst2_fail:

    ; T3: free_count Klasse 0 = 15
    cmp dword [slab_classes + 0*SLABCLS_SIZE + SLABCLS_FREE_COUNT], 15
    je .slst3_ok
    inc esi
.slst3_ok:

    ; T4: alloc Klasse 2 (64 Byte)
    mov eax, 64
    call slab_alloc
    jnc .slst4_ok
    inc esi
.slst4_ok:
    push eax            ; merken

    ; T5: free Klasse 0
    mov eax, ebx
    mov edx, 16
    call slab_free
    cmp dword [slab_classes + 0*SLABCLS_SIZE + SLABCLS_FREE_COUNT], 16
    je .slst5_ok
    inc esi
.slst5_ok:

    ; T6: free Klasse 2
    pop eax
    mov edx, 64
    call slab_free
    cmp dword [slab_classes + 2*SLABCLS_SIZE + SLABCLS_FREE_COUNT], 16
    je .slst6_ok
    inc esi
.slst6_ok:

    ; T7: alloc Klasse 3 (128 Byte) Erfolg
    mov eax, 128
    call slab_alloc
    jnc .slst7_ok
    inc esi
.slst7_ok:
    ; Aufräumen
    mov edx, 128
    call slab_free

    test esi, esi
    jnz .slstf
    clc
    pop esi
    pop ebx
    ret
.slstf:
    stc
    pop esi
    pop ebx
    ret

slab_ready:     dd 0
align 4
slab_classes:   times SLAB_CLASS_COUNT * SLABCLS_SIZE db 0
slab_pool_0:    times SLAB_SIZE_0 * SLAB_OBJECTS_PER db 0
slab_pool_1:    times SLAB_SIZE_1 * SLAB_OBJECTS_PER db 0
slab_pool_2:    times SLAB_SIZE_2 * SLAB_OBJECTS_PER db 0
slab_pool_3:    times SLAB_SIZE_3 * SLAB_OBJECTS_PER db 0

; ===========================================================================
; NPSPEC-MEMORY-ASLR-0001 – Nova Address Space Layout Randomization
; Seed-basierte Offset-Erzeugung für zukünftige VA-Randomisierung.
; Im Flat-Binary-Kernel: verwaltet Randomisierungs-Zustand und -API.
; ===========================================================================

ASLR_REGION_COUNT    equ 8
ASLR_RECORD_SIZE     equ 16

ASLR_TYPE_STACK      equ 1
ASLR_TYPE_HEAP       equ 2
ASLR_TYPE_MMAP       equ 3
ASLR_TYPE_VDSO       equ 4

ASLR_REG_TYPE        equ 0
ASLR_REG_BASE        equ 4
ASLR_REG_OFFSET      equ 8
ASLR_REG_FLAGS       equ 12

aslr_initialize:
    ; LCG-Seed aus TSC + wd_global_tick
    rdtsc
    xor eax, [wd_global_tick]
    mov [aslr_seed], eax
    mov edi, aslr_regions
    mov ecx, ASLR_REGION_COUNT * ASLR_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [aslr_ready], 1
    mov dword [aslr_count], 0
    clc
    ret

; LCG-Zufall: nächster Pseudo-Zufall → EAX
aslr_next_random:
    mov eax, [aslr_seed]
    imul eax, eax, 1664525
    add eax, 1013904223
    mov [aslr_seed], eax
    ret

; EAX=region_type  EDX=base_hint → EAX=randomized_offset CF=0/CF=1 voll
aslr_randomize_region:
    push esi
    push ecx
    push ebx
    cmp dword [aslr_count], ASLR_REGION_COUNT
    jae .arr_nostore
    ; Offset erzeugen: Zufall & Alignment-Maske (4KB-Granularität)
    push eax
    push edx
    call aslr_next_random
    and eax, 0x000FF000    ; max 255 × 4 KB = 1020 KB Versatz
    mov ebx, eax           ; Offset
    pop edx
    pop eax
    ; In Tabelle speichern
    mov ecx, [aslr_count]
    imul esi, ecx, ASLR_RECORD_SIZE
    add esi, aslr_regions
    mov [esi + ASLR_REG_TYPE], eax
    mov [esi + ASLR_REG_BASE], edx
    mov [esi + ASLR_REG_OFFSET], ebx
    mov dword [esi + ASLR_REG_FLAGS], 1
    inc dword [aslr_count]
    mov eax, ebx
    clc
    pop ebx
    pop ecx
    pop esi
    ret
.arr_nostore:
    ; Kein freier Slot – trotzdem Offset zurückgeben
    call aslr_next_random
    and eax, 0x000FF000
    clc
    pop ebx
    pop ecx
    pop esi
    ret

aslr_self_test:
    push esi
    push ebx
    xor ebx, ebx
    cmp dword [aslr_ready], 1
    je .ast1
    inc ebx
.ast1:
    ; Zwei verschiedene Offsets erzeugen
    mov eax, ASLR_TYPE_STACK
    mov edx, 0xC0000000
    call aslr_randomize_region
    mov esi, eax
    push esi
    mov eax, ASLR_TYPE_HEAP
    mov edx, 0xC0100000
    call aslr_randomize_region
    pop esi
    ; Beide 4KB-aligned
    test esi, 0xFFF
    jz .ast2
    inc ebx
.ast2:
    test eax, 0xFFF
    jz .ast3
    inc ebx
.ast3:
    ; aslr_count=2
    cmp dword [aslr_count], 2
    je .ast4
    inc ebx
.ast4:
    test ebx, ebx
    jnz .astf
    clc
    pop ebx
    pop esi
    ret
.astf:
    stc
    pop ebx
    pop esi
    ret

aslr_ready:  dd 0
aslr_seed:   dd 0
aslr_count:  dd 0
align 4
aslr_regions:
    times ASLR_REGION_COUNT * ASLR_RECORD_SIZE db 0

; ===========================================================================
; NPSPEC-MEMORY-COW-0001 – Nova Copy-on-Write Tracking
; Verwaltet CoW-Referenzen: Seiten mit refcount>1 werden bei Schreibzugriff
; kopiert. Kernel-seitig: Tracking-Tabelle für CoW-Deskriptoren.
; ===========================================================================

COW_CAPACITY         equ 16
COW_RECORD_SIZE      equ 16

COW_STATE_FREE       equ 0
COW_STATE_SHARED     equ 1
COW_STATE_COPIED     equ 2

COW_ID               equ 0
COW_STATE_OFF        equ 4
COW_PHYS_ADDR        equ 8
COW_REFCOUNT         equ 12

cow_initialize:
    mov edi, cow_table
    mov ecx, COW_CAPACITY * COW_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [cow_ready], 1
    mov dword [cow_next_id], 0
    mov dword [cow_copies], 0
    clc
    ret

; EAX=phys_addr → EAX=slot CF=0 / CF=1 voll
cow_register_page:
    push esi
    push ecx
    mov esi, cow_table
    xor ecx, ecx
.cowrp_scan:
    cmp ecx, COW_CAPACITY
    jae .cowrp_full
    cmp dword [esi + COW_STATE_OFF], COW_STATE_FREE
    je .cowrp_found
    add esi, COW_RECORD_SIZE
    inc ecx
    jmp .cowrp_scan
.cowrp_found:
    push eax
    mov eax, [cow_next_id]
    inc dword [cow_next_id]
    mov [esi + COW_ID], eax
    pop eax
    mov dword [esi + COW_STATE_OFF], COW_STATE_SHARED
    mov [esi + COW_PHYS_ADDR], eax
    mov dword [esi + COW_REFCOUNT], 2   ; Minimal 2 Referenzen für CoW
    mov eax, ecx
    clc
    pop ecx
    pop esi
    ret
.cowrp_full:
    stc
    pop ecx
    pop esi
    ret

; EAX=slot → simuliert Schreibzugriff (refcount>1 → copy); CF=0/CF=1
cow_on_write:
    push esi
    cmp eax, COW_CAPACITY
    jae .cowow_fail
    imul esi, eax, COW_RECORD_SIZE
    add esi, cow_table
    cmp dword [esi + COW_STATE_OFF], COW_STATE_FREE
    je .cowow_fail
    cmp dword [esi + COW_REFCOUNT], 1
    jbe .cowow_no_copy   ; Einziger Referent → kein CoW nötig
    ; Refcount verringern (die "andere" Referenz behält die alte Seite)
    dec dword [esi + COW_REFCOUNT]
    mov dword [esi + COW_STATE_OFF], COW_STATE_COPIED
    inc dword [cow_copies]
    clc
    pop esi
    ret
.cowow_no_copy:
    clc
    pop esi
    ret
.cowow_fail:
    stc
    pop esi
    ret

cow_self_test:
    push ebx
    push esi
    xor esi, esi
    cmp dword [cow_ready], 1
    je .cwst1
    inc esi
.cwst1:
    ; Seite registrieren
    mov eax, 0x00100000
    call cow_register_page
    jnc .cwst2
    inc esi
.cwst2:
    mov ebx, eax
    ; refcount=2, on_write → COPIED
    mov eax, ebx
    call cow_on_write
    jnc .cwst3
    inc esi
.cwst3:
    cmp dword [cow_copies], 1
    je .cwst4
    inc esi
.cwst4:
    ; State=COPIED prüfen
    push ecx
    imul ecx, ebx, COW_RECORD_SIZE
    add ecx, cow_table
    cmp dword [ecx + COW_STATE_OFF], COW_STATE_COPIED
    je .cwst5
    inc esi
.cwst5:
    pop ecx
    test esi, esi
    jnz .cwstf
    clc
    pop esi
    pop ebx
    ret
.cwstf:
    stc
    pop esi
    pop ebx
    ret

cow_ready:    dd 0
cow_next_id:  dd 0
cow_copies:   dd 0
align 4
cow_table:
    times COW_CAPACITY * COW_RECORD_SIZE db 0

; ===========================================================================
; NPSPEC-MEMORY-OOM-0001 – Nova Out-of-Memory Handler
; Wenn Speicher erschöpft: Kandidaten-Auswahl nach OOM-Score, Terminierung.
; ===========================================================================

OOM_CAND_COUNT       equ 8
OOM_RECORD_SIZE      equ 16

OOM_STATE_FREE       equ 0
OOM_STATE_ACTIVE     equ 1
OOM_STATE_KILLED     equ 2

OOM_ID               equ 0
OOM_STATE_OFF        equ 4
OOM_PID              equ 8
OOM_SCORE            equ 12   ; höherer Score = wird zuerst terminiert

oom_initialize:
    mov edi, oom_table
    mov ecx, OOM_CAND_COUNT * OOM_RECORD_SIZE / 4
    xor eax, eax
    rep stosd
    mov dword [oom_ready], 1
    mov dword [oom_kills], 0
    mov dword [oom_events], 0
    clc
    ret

; EAX=pid  EDX=score → EAX=slot CF=0 / CF=1 voll
oom_register_candidate:
    push esi
    push ecx
    mov esi, oom_table
    xor ecx, ecx
.oomrc_scan:
    cmp ecx, OOM_CAND_COUNT
    jae .oomrc_full
    cmp dword [esi + OOM_STATE_OFF], OOM_STATE_FREE
    je .oomrc_found
    cmp dword [esi + OOM_STATE_OFF], OOM_STATE_KILLED
    je .oomrc_found   ; Wiederverwendbar
    add esi, OOM_RECORD_SIZE
    inc ecx
    jmp .oomrc_scan
.oomrc_found:
    push eax
    mov eax, ecx
    mov [esi + OOM_ID], eax
    pop eax
    mov dword [esi + OOM_STATE_OFF], OOM_STATE_ACTIVE
    mov [esi + OOM_PID], eax
    mov [esi + OOM_SCORE], edx
    mov eax, ecx
    clc
    pop ecx
    pop esi
    ret
.oomrc_full:
    stc
    pop ecx
    pop esi
    ret

; Wählt Kandidat mit höchstem Score und "terminiert" ihn → EAX=pid CF=0 / CF=1 kein
oom_kill_victim:
    push esi
    push ecx
    push edx
    push ebx
    mov esi, oom_table
    mov edx, 0           ; bester Score
    mov ebx, -1          ; Kandidat-Slot
    xor ecx, ecx
.oomkv_scan:
    cmp ecx, OOM_CAND_COUNT
    jae .oomkv_done
    cmp dword [esi + OOM_STATE_OFF], OOM_STATE_ACTIVE
    jne .oomkv_next
    cmp dword [esi + OOM_SCORE], edx
    jbe .oomkv_next
    mov edx, [esi + OOM_SCORE]
    mov ebx, ecx
.oomkv_next:
    add esi, OOM_RECORD_SIZE
    inc ecx
    jmp .oomkv_scan
.oomkv_done:
    cmp ebx, -1
    je .oomkv_empty
    imul esi, ebx, OOM_RECORD_SIZE
    add esi, oom_table
    mov dword [esi + OOM_STATE_OFF], OOM_STATE_KILLED
    mov eax, [esi + OOM_PID]
    inc dword [oom_kills]
    inc dword [oom_events]
    clc
    pop ebx
    pop edx
    pop ecx
    pop esi
    ret
.oomkv_empty:
    inc dword [oom_events]
    stc
    pop ebx
    pop edx
    pop ecx
    pop esi
    ret

oom_self_test:
    push ebx
    push esi
    xor esi, esi
    cmp dword [oom_ready], 1
    je .oomst1
    inc esi
.oomst1:
    ; Register pid=1 score=50, pid=2 score=80
    mov eax, 1
    mov edx, 50
    call oom_register_candidate
    jnc .oomst2
    inc esi
.oomst2:
    mov eax, 2
    mov edx, 80
    call oom_register_candidate
    jnc .oomst3
    inc esi
.oomst3:
    ; Kill → muss pid=2 (höchster Score) sein
    call oom_kill_victim
    jnc .oomst4_cf
    inc esi
.oomst4_cf:
    cmp eax, 2
    je .oomst4
    inc esi
.oomst4:
    cmp dword [oom_kills], 1
    je .oomst5
    inc esi
.oomst5:
    test esi, esi
    jnz .oomstf
    clc
    pop esi
    pop ebx
    ret
.oomstf:
    stc
    pop esi
    pop ebx
    ret

oom_ready:   dd 0
oom_kills:   dd 0
oom_events:  dd 0
align 4
oom_table:
    times OOM_CAND_COUNT * OOM_RECORD_SIZE db 0

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
; Kernel Object Manager (ADR-2008 / NPSPEC-KERNEL-0012)
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
    mov edi, object_semantic_types
    xor eax, eax
    mov ecx, OBJECT_TABLE_CAPACITY * 3
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
    mov dword [object_semantic_types + edx * 4], 0
    mov dword [object_semantic_versions + edx * 4], 0
    mov dword [object_semantic_validation + edx * 4], 0
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
; Semantic Type ist ein Sidecar des stabilen Objekts und weder Objekt-ID noch
; Pfad noch allgemeines Payloadfeld (ADR-SEMANTIC-0003 Typed Resources).
object_semantic_types:      times OBJECT_TABLE_CAPACITY dd 0
object_semantic_versions:   times OBJECT_TABLE_CAPACITY dd 0
object_semantic_validation: times OBJECT_TABLE_CAPACITY dd 0

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

; §013 §53: Self-Test – 9 Tests, EDI = Fehler-Zähler
handle_manager_self_test:
    push ebp
    mov ebp, esp
    push ebx
    push esi
    push edi
    sub esp, 8                          ; [ebp-16]=obj_handle, [ebp-20]=proc_handle
    xor edi, edi
    mov dword [ebp - 16], 0
    mov dword [ebp - 20], 0

    ; Test 1: handle_active_count == 0 nach init
    cmp dword [handle_active_count], 0
    je .t2
    inc edi

.t2:
    ; Test 2: Testobjekt anlegen (Typ 0x48444C54 = "HDLT")
    mov dword [handle_owner_pid], 1
    mov eax, 0x48444C54
    xor edx, edx
    xor ebx, ebx
    call object_create
    jnc .t2_ok
    inc edi
    jmp .cleanup
.t2_ok:
    mov [ebp - 16], eax

.t3:
    ; Test 3: handle_create
    mov eax, 1
    mov edx, [ebp - 16]
    mov ebx, 0x48444C54
    mov ecx, HANDLE_RIGHT_QUERY | HANDLE_RIGHT_WAIT
    xor esi, esi
    call handle_create
    jnc .t3_ok
    inc edi
    jmp .cleanup_obj
.t3_ok:
    mov [ebp - 20], eax

.t4:
    ; Test 4: handle_active_count == 1
    cmp dword [handle_active_count], 1
    je .t5
    inc edi

.t5:
    ; Test 5: handle_resolve mit korrektem Typ und Rechten
    mov eax, 1
    mov edx, [ebp - 20]
    mov ebx, 0x48444C54
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jnc .t6
    inc edi

.t6:
    ; Test 6: handle_resolve mit falschem Typ → muss scheitern
    mov eax, 1
    mov edx, [ebp - 20]
    mov ebx, 0x44454144                 ; "DEAD"
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jc .t7
    inc edi

.t7:
    ; Test 7: handle_close
    mov eax, 1
    mov edx, [ebp - 20]
    call handle_close
    jnc .t8
    inc edi
    jmp .cleanup_handle

.t8:
    ; Test 8: handle nach close → veraltete Generation
    mov eax, 1
    mov edx, [ebp - 20]
    mov ebx, 0x48444C54
    mov ecx, HANDLE_RIGHT_QUERY
    call handle_resolve
    jc .t9
    inc edi

.t9:
    ; Test 9: handle_active_count == 0 nach close
    cmp dword [handle_active_count], 0
    je .cleanup_obj
    inc edi
    jmp .cleanup_obj

.cleanup_handle:
    ; Handle noch offen (t7 fehlgeschlagen): schließen
    mov eax, 1
    mov edx, [ebp - 20]
    call handle_close

.cleanup_obj:
    mov eax, [ebp - 16]
    test eax, eax
    jz .cleanup
    call object_release

.cleanup:
    mov dword [handle_owner_pid], 0
    test edi, edi
    jnz .selftest_fail
    add esp, 8
    pop edi
    pop esi
    pop ebx
    pop ebp
    clc
    ret
.selftest_fail:
    add esp, 8
    pop edi
    pop esi
    pop ebx
    pop ebp
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
; Kernel Component Manager (ADR-2009 / NPSPEC-KERNEL-0001 §Subsystem-Registry)
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
; Virtueller Bootstrap-Speichermanager (ADR-2002 / NPSPEC-KERNEL-0007)
; ---------------------------------------------------------------------------

PAGING_PAGE_PRESENT   equ 0x001
PAGING_PAGE_WRITE     equ 0x002
PAGING_PAGE_USER      equ 0x004
PAGING_API_SIZE       equ 32
PAGING_API_ABI_MAJOR  equ 1
PAGING_API_ABI_MINOR  equ 0
PAGING_CAP_4K_PAGES   equ 0x00000001
PAGING_CAP_IDENTITY   equ 0x00000002
PAGING_LOW_LIMIT      equ 0x01000000

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
    ; abgebildet. Der UEFI-Kernel liegt hoeher als der BIOS-Kernel; mit 1024
    ; Bootstrap-PMM-Seiten muss der fruehe Identity-Bereich daher bis 16 MiB
    ; reichen, sonst koennen Heap-Seiten direkt nach 8 MiB ungeplant faulten.
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

