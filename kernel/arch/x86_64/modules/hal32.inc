; BIB-Validierung und Firmware-Interface (NPSPEC-HAL-FIRMWARE-0001)
; NPSPEC-HAL-0001 / NPSPEC-BOOT-BIOS-0001 / NPSPEC-BOOT-SECURE-0001
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
    cmp eax, BIB_TLV_FIRMWARE_RUNTIME
    je .firmware_runtime
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

.firmware_runtime:
    cmp ecx, BIB_FIRMWARE_RUNTIME_SIZE
    jb .invalid
    cmp dword [edx + 0], NOVA_FIRMWARE_RUNTIME_PROVIDER_UEFI_X64
    jne .advance
    test dword [edx + 4], NOVA_FIRMWARE_RUNTIME_PERSIST_BOOT_HEALTH
    jz .advance
    cmp dword [edx + 12], 0
    jne .advance
    cmp dword [edx + 20], 0
    jne .advance
    cmp dword [edx + 24], 64
    jb .advance
    cmp dword [edx + 28], 0
    jne .advance
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_FIRMWARE_RUNTIME_CAPS], eax
    mov eax, [edx + 8]
    mov [kernel_context + CONTEXT_FIRMWARE_RUNTIME_CONTEXT], eax
    mov eax, [edx + 16]
    mov [kernel_context + CONTEXT_FIRMWARE_RUNTIME_ENTRY], eax
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
    cmp dword [edx + 4], 0
    jne .acpi_unaddressable
    mov [kernel_context + CONTEXT_ACPI_ADDRESS], eax
    jmp .advance
.acpi_unaddressable:
    mov dword [kernel_context + CONTEXT_ACPI_ADDRESS], 0
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
    cmp dword [edx + 0], NOVA_BOOT_MODE_DIAGNOSTIC
    ja .invalid
    or dword [kernel_context + CONTEXT_SEEN], CONTEXT_HAS_BOOT_OPTIONS
    mov eax, [edx + 0]
    mov [kernel_context + CONTEXT_BOOT_MODE], eax
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_BOOT_FLAGS], eax
    cmp dword [edx + 8], NOVA_BOOT_GENERATION_RECOVERY
    ja .invalid
    mov eax, [edx + 8]
    mov [kernel_context + CONTEXT_BOOT_GENERATION], eax
    mov eax, [edx + 12]
    mov [kernel_context + CONTEXT_FALLBACK_LEVEL], eax
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
    mov eax, [edx + 4]
    mov [kernel_context + CONTEXT_SYSTEM_GENERATION_HI], eax
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

ACPI_SRAT_CPU_CAPACITY         equ 8
ACPI_SRAT_MEMORY_CAPACITY      equ 8
ACPI_SRAT_CPU_RECORD_SIZE      equ 8
ACPI_SRAT_MEMORY_RECORD_SIZE   equ 24

; ACPI-Root-Pointer vor der Paging-Aktivierung prüfen. Ein fehlender oder
; beschädigter RSDP verhindert den UP-Boot nicht und aktiviert keinen AP.
acpi_rsdp_initialize:
    ; Der BSP ist auch ohne Firmwaretabellen als kontrollierter Fallback
    ; bekannt. Die APIC-ID stammt aus CPUID und bleibt damit Hardware-ID statt
    ; frei erfundener logischer Nummer.
    mov dword [acpi_cpu_count], 1
    mov dword [acpi_madt_valid], 0
    mov dword [acpi_srat_valid], 0
    mov dword [acpi_srat_cpu_count], 0
    mov dword [acpi_srat_memory_count], 0
    mov eax, 1
    cpuid
    shr ebx, 24
    mov [acpi_apic_ids], ebx
    mov esi, [kernel_context + CONTEXT_ACPI_ADDRESS]
    test esi, esi
    jz .bios_scan
    call acpi_rsdp_validate
    jnc .ready
    mov dword [kernel_context + CONTEXT_ACPI_ADDRESS], 0
.bios_scan:
    cmp dword [kernel_context + CONTEXT_PLATFORM], NOVA_BOOT_PLATFORM_BIOS
    jne .unavailable
    movzx esi, word [0x40E]        ; EBDA-Segment aus dem BIOS Data Area
    shl esi, 4
    cmp esi, 0x80000
    jb .high_scan
    cmp esi, 0xA0000
    jae .high_scan
    lea edx, [esi + 1024]
    call acpi_rsdp_scan_range
    jnc .found
.high_scan:
    mov esi, 0xE0000
    mov edx, 0x100000
    call acpi_rsdp_scan_range
    jc .unavailable
.found:
    mov [kernel_context + CONTEXT_ACPI_ADDRESS], esi
.ready:
    call acpi_madt_discover
    call acpi_srat_discover
    call acpi_hpet_discover
    mov esi, message_acpi_rsdp_ok
    call serial_write_string
    cmp dword [acpi_madt_valid], 1
    jne .ready_done
    mov esi, message_acpi_cpu_count
    call serial_write_string
    mov eax, [acpi_cpu_count]
    call serial_write_hex32
    mov esi, message_line_end
    call serial_write_string
.ready_done:
    cmp dword [acpi_srat_valid], 1
    jne .srat_unavailable
    call acpi_srat_self_test
    jc .srat_unavailable
    mov esi, message_acpi_srat_ok
    call serial_write_string
    mov eax, [acpi_srat_cpu_count]
    call serial_write_hex32
    mov esi, message_topology_count_separator
    call serial_write_string
    mov eax, [acpi_srat_memory_count]
    call serial_write_hex32
    mov esi, message_line_end
    call serial_write_string
    ret
.srat_unavailable:
    mov dword [acpi_srat_valid], 0
    mov esi, message_acpi_srat_unavailable
    call serial_write_string
    ret
.unavailable:
    mov esi, message_acpi_rsdp_missing
    call serial_write_string
    ret

; ESI = Anfang, EDX = exklusives Ende; 16-Byte-RSDP-Ausrichtung.
acpi_rsdp_scan_range:
.next:
    lea eax, [esi + 36]
    cmp eax, edx
    ja .missing
    push edx
    call acpi_rsdp_validate
    pop edx
    jnc .found
    add esi, 16
    jmp .next
.found:
    clc
    ret
.missing:
    stc
    ret

acpi_rsdp_validate:
    cmp esi, 0xFFFFEFFF
    ja .invalid
    cmp dword [esi], 0x20445352 ; "RSD "
    jne .invalid
    cmp dword [esi + 4], 0x20525450 ; "PTR "
    jne .invalid
    push ecx
    push edi
    xor eax, eax
    mov edi, esi
    mov ecx, 20
.first_checksum:
    add al, [edi]
    inc edi
    loop .first_checksum
    test al, al
    jnz .checksum_invalid
    cmp byte [esi + 15], 2
    jb .valid
    mov ecx, [esi + 20]
    cmp ecx, 36
    jb .checksum_invalid
    cmp ecx, 4096
    ja .checksum_invalid
    mov edi, esi
    add edi, ecx
    jc .checksum_invalid
    mov edi, esi
    xor eax, eax
.extended_checksum:
    add al, [edi]
    inc edi
    loop .extended_checksum
    test al, al
    jnz .checksum_invalid
.valid:
    pop edi
    pop ecx
    clc
    ret
.checksum_invalid:
    pop edi
    pop ecx
.invalid:
    stc
    ret

; Physische ACPI-Tabellen werden ausschließlich im frühen Identity-Modus
; gelesen. Ein kompletter Tabellenbereich muss in einem E820/UEFI-RAM-
; Deskriptor liegen; MMIO und 64-Bit-Adressen werden nicht dereferenziert.
; ESI=Adresse, ECX=Länge, CF=0 wenn vollständig im Firmware-Memory-Map.
acpi_physical_range_valid:
    push eax
    push ebx
    push edx
    push edi
    test esi, esi
    jz .bad
    test ecx, ecx
    jz .bad
    mov edx, esi
    add edx, ecx
    jc .bad
    mov ebx, [kernel_context + CONTEXT_MEMORY_COUNT]
    cmp ebx, 256                   ; UEFI-Map 0x5800..0x7000: 256 x 24 Byte
    ja .bad
    mov edi, [kernel_context + CONTEXT_MEMORY_MAP]
.entry:
    test ebx, ebx
    jz .bad
    cmp dword [edi + 4], 0
    jne .next
    cmp dword [edi + 16], 1
    jb .next
    cmp dword [edi + 16], 4
    ja .next
    mov eax, [edi]
    cmp esi, eax
    jb .next
    cmp dword [edi + 12], 0
    jne .good
    add eax, [edi + 8]
    jc .good
    cmp edx, eax
    jbe .good
.next:
    add edi, MEMORY_MAP_ENTRY_SIZE
    dec ebx
    jmp .entry
.good:
    pop edi
    pop edx
    pop ebx
    pop eax
    clc
    ret
.bad:
    pop edi
    pop edx
    pop ebx
    pop eax
    stc
    ret

; ESI=physischer Tabellenanfang, EAX=erwartete Signatur;
; ECX=geprüfte Länge bei Erfolg. Maximal 4 KiB Bootstrap-Tabellen.
acpi_table_validate:
    push eax
    push edx
    push edi
    mov ecx, 36
    call acpi_physical_range_valid
    jc .bad
    mov edx, [esi]
    cmp edx, eax
    jne .bad
    mov ecx, [esi + 4]
    cmp ecx, 36
    jb .bad
    cmp ecx, 4096
    ja .bad
    call acpi_physical_range_valid
    jc .bad
    mov edx, ecx
    mov edi, esi
    xor eax, eax
.checksum:
    add al, [edi]
    inc edi
    loop .checksum
    mov ecx, edx
    test al, al
    jnz .bad
    pop edi
    pop edx
    pop eax
    clc
    ret
.bad:
    pop edi
    pop edx
    pop eax
    stc
    ret

; BIOS/UEFI-MADT vor Paging in eine kleine, unveränderliche ID-Liste kopieren.
; Der BSP bleibt CPU 0; APs werden erst später separat gestartet.
acpi_madt_discover:
    mov dword [acpi_cpu_count], 1
    mov dword [acpi_madt_valid], 0
    mov eax, 1
    cpuid
    shr ebx, 24
    mov [acpi_apic_ids], ebx
    mov ebp, [kernel_context + CONTEXT_ACPI_ADDRESS]
    cmp byte [ebp + 15], 2
    jb .rsdt
    cmp dword [ebp + 28], 0
    jne .rsdt
    mov esi, [ebp + 24]
    mov eax, 0x54445358         ; XSDT
    call acpi_table_validate
    jc .rsdt
    mov dword [acpi_root_entry_size], 8
    jmp .root_ready
.rsdt:
    mov esi, [ebp + 16]
    mov eax, 0x54445352         ; RSDT
    call acpi_table_validate
    jc .missing
    mov dword [acpi_root_entry_size], 4
.root_ready:
    mov eax, ecx
    sub eax, 36
    xor edx, edx
    div dword [acpi_root_entry_size]
    test edx, edx
    jnz .missing
    mov [acpi_root_entries_left], eax
    lea eax, [esi + 36]
    mov [acpi_root_cursor], eax
.root_next:
    cmp dword [acpi_root_entries_left], 0
    je .missing
    mov ebx, [acpi_root_cursor]
    mov esi, [ebx]
    cmp dword [acpi_root_entry_size], 8
    jne .candidate
    cmp dword [ebx + 4], 0
    jne .advance
.candidate:
    mov eax, 0x43495041         ; APIC/MADT
    call acpi_table_validate
    jc .advance
    call acpi_madt_parse
    jc .advance
    mov dword [acpi_madt_valid], 1
    ret
.advance:
    mov eax, [acpi_root_entry_size]
    add [acpi_root_cursor], eax
    dec dword [acpi_root_entries_left]
    jmp .root_next
.missing:
    mov dword [acpi_cpu_count], 1
    ret

; ESI=geprüfter MADT, ECX=geprüfte Gesamtlänge.
acpi_madt_parse:
    cmp ecx, 44
    jb .bad
    mov dword [acpi_cpu_count], 1
    mov dword [acpi_madt_bsp_seen], 0
    mov edi, esi
    add edi, 44
    mov ebp, esi
    add ebp, ecx
.entry:
    cmp edi, ebp
    je .complete
    lea eax, [edi + 2]
    cmp eax, ebp
    ja .bad
    movzx ebx, byte [edi + 1]
    cmp ebx, 2
    jb .bad
    mov eax, edi
    add eax, ebx
    cmp eax, ebp
    ja .bad
    cmp byte [edi], 0
    je .lapic
    cmp byte [edi], 9
    je .x2apic
    jmp .next
.lapic:
    cmp ebx, 8
    jb .bad
    test dword [edi + 4], 1
    jz .next
    movzx eax, byte [edi + 3]
    call acpi_madt_add_id
    jmp .next
.x2apic:
    cmp ebx, 16
    jb .bad
    test dword [edi + 8], 1
    jz .next
    mov eax, [edi + 4]
    call acpi_madt_add_id
.next:
    add edi, ebx
    jmp .entry
.complete:
    cmp dword [acpi_madt_bsp_seen], 1
    jne .bad
    clc
    ret
.bad:
    mov dword [acpi_cpu_count], 1
    stc
    ret

; EAX=Hardware-ID, Duplikate und BSP werden nicht als AP registriert.
acpi_madt_add_id:
    cmp eax, [acpi_apic_ids]
    jne .other
    mov dword [acpi_madt_bsp_seen], 1
    ret
.other:
    push ecx
    push edx
    xor ecx, ecx
.unique:
    cmp ecx, [acpi_cpu_count]
    jae .insert
    cmp eax, [acpi_apic_ids + ecx * 4]
    je .done
    inc ecx
    jmp .unique
.insert:
    cmp ecx, CPU_CAPACITY
    jae .done
    mov [acpi_apic_ids + ecx * 4], eax
    inc dword [acpi_cpu_count]
.done:
    pop edx
    pop ecx
    ret

; Sucht und validiert die ACPI System Resource Affinity Table getrennt von der
; MADT. Die Root-Tabelle wird erneut geprüft, damit kein impliziter Cursor-
; Zustand zwischen Firmwareprovidern geteilt wird.
acpi_srat_discover:
    mov dword [acpi_srat_valid], 0
    mov dword [acpi_srat_cpu_count], 0
    mov dword [acpi_srat_memory_count], 0
    mov dword [acpi_srat_conflicts], 0
    mov dword [acpi_srat_unsupported_memory], 0
    mov edi, acpi_srat_cpu_records
    xor eax, eax
    mov ecx, (ACPI_SRAT_CPU_CAPACITY * ACPI_SRAT_CPU_RECORD_SIZE) / 4
    rep stosd
    mov edi, acpi_srat_memory_records
    mov ecx, (ACPI_SRAT_MEMORY_CAPACITY * ACPI_SRAT_MEMORY_RECORD_SIZE) / 4
    rep stosd
    mov ebp, [kernel_context + CONTEXT_ACPI_ADDRESS]
    test ebp, ebp
    jz .missing
    cmp byte [ebp + 15], 2
    jb .rsdt
    cmp dword [ebp + 28], 0
    jne .rsdt
    mov esi, [ebp + 24]
    mov eax, 0x54445358             ; XSDT
    call acpi_table_validate
    jc .rsdt
    mov dword [acpi_srat_root_entry_size], 8
    jmp .root_ready
.rsdt:
    mov esi, [ebp + 16]
    mov eax, 0x54445352             ; RSDT
    call acpi_table_validate
    jc .missing
    mov dword [acpi_srat_root_entry_size], 4
.root_ready:
    mov eax, ecx
    sub eax, 36
    xor edx, edx
    div dword [acpi_srat_root_entry_size]
    test edx, edx
    jnz .missing
    mov [acpi_srat_root_entries_left], eax
    lea eax, [esi + 36]
    mov [acpi_srat_root_cursor], eax
.root_next:
    cmp dword [acpi_srat_root_entries_left], 0
    je .missing
    mov ebx, [acpi_srat_root_cursor]
    mov esi, [ebx]
    cmp dword [acpi_srat_root_entry_size], 8
    jne .candidate
    cmp dword [ebx + 4], 0
    jne .advance
.candidate:
    mov eax, 0x54415253             ; SRAT
    call acpi_table_validate
    jc .advance
    call acpi_srat_parse
    jc .invalid
    mov dword [acpi_srat_valid], 1
    ret
.advance:
    mov eax, [acpi_srat_root_entry_size]
    add [acpi_srat_root_cursor], eax
    dec dword [acpi_srat_root_entries_left]
    jmp .root_next
.invalid:
    mov dword [acpi_srat_conflicts], 1
.missing:
    ret

; ESI=validierte SRAT, ECX=Gesamtlänge.
acpi_srat_parse:
    cmp ecx, 48
    jb .invalid
    mov edi, esi
    add edi, 48
    mov ebp, esi
    add ebp, ecx
.entry:
    cmp edi, ebp
    je .complete
    lea eax, [edi + 2]
    cmp eax, ebp
    ja .invalid
    movzx ebx, byte [edi + 1]
    cmp ebx, 2
    jb .invalid
    mov [acpi_srat_entry_length], ebx
    mov eax, edi
    add eax, ebx
    jc .invalid
    cmp eax, ebp
    ja .invalid
    cmp byte [edi], 0
    je .lapic
    cmp byte [edi], 1
    je .memory
    cmp byte [edi], 2
    je .x2apic
    jmp .next
.lapic:
    cmp ebx, 16
    jb .invalid
    test dword [edi + 4], 1
    jz .next
    movzx eax, byte [edi + 3]
    movzx edx, byte [edi + 2]
    movzx ecx, byte [edi + 9]
    shl ecx, 8
    or edx, ecx
    movzx ecx, byte [edi + 10]
    shl ecx, 16
    or edx, ecx
    movzx ecx, byte [edi + 11]
    shl ecx, 24
    or edx, ecx
    call acpi_srat_add_cpu
    jc .invalid
    jmp .next
.x2apic:
    cmp ebx, 24
    jb .invalid
    test dword [edi + 12], 1
    jz .next
    mov eax, [edi + 8]
    mov edx, [edi + 4]
    call acpi_srat_add_cpu
    jc .invalid
    jmp .next
.memory:
    cmp ebx, 40
    jb .invalid
    test dword [edi + 28], 1
    jz .next
    mov eax, edi
    call acpi_srat_add_memory
    jc .invalid
.next:
    mov ebx, [acpi_srat_entry_length]
    add edi, ebx
    jmp .entry
.complete:
    cmp dword [acpi_srat_cpu_count], 0
    jne .valid
    cmp dword [acpi_srat_memory_count], 0
    je .invalid
.valid:
    clc
    ret
.invalid:
    mov dword [acpi_srat_cpu_count], 0
    mov dword [acpi_srat_memory_count], 0
    stc
    ret

; EAX=APIC-ID, EDX=Proximity-Domain. Nur MADT-bekannte CPUs werden akzeptiert.
acpi_srat_add_cpu:
    push edi
    mov [acpi_srat_temp_apic], eax
    mov [acpi_srat_temp_domain], edx
    xor ecx, ecx
.madt_scan:
    cmp ecx, [acpi_cpu_count]
    jae .conflict
    cmp eax, [acpi_apic_ids + ecx * 4]
    je .madt_known
    inc ecx
    jmp .madt_scan
.madt_known:
    xor ecx, ecx
.existing:
    cmp ecx, [acpi_srat_cpu_count]
    jae .insert
    mov edi, ecx
    shl edi, 3
    add edi, acpi_srat_cpu_records
    cmp [edi], eax
    jne .existing_next
    cmp [edi + 4], edx
    jne .conflict
    pop edi
    clc
    ret
.existing_next:
    inc ecx
    jmp .existing
.insert:
    cmp ecx, ACPI_SRAT_CPU_CAPACITY
    jae .conflict
    mov edi, ecx
    shl edi, 3
    add edi, acpi_srat_cpu_records
    mov [edi], eax
    mov [edi + 4], edx
    inc dword [acpi_srat_cpu_count]
    pop edi
    clc
    ret
.conflict:
    inc dword [acpi_srat_conflicts]
    pop edi
    stc
    ret

; EAX=Zeiger auf einen validierten SRAT-Memory-Affinity-Eintrag.
acpi_srat_add_memory:
    push edi
    mov esi, eax
    cmp dword [esi + 12], 0         ; aktuell nur 32-Bit-adressierbare Bereiche
    jne .unsupported
    cmp dword [esi + 20], 0
    jne .unsupported
    mov eax, [esi + 16]
    test eax, eax
    jz .conflict
    mov edx, [esi + 8]
    mov ecx, edx
    add ecx, eax
    jc .conflict
    mov [acpi_srat_temp_base], edx
    mov [acpi_srat_temp_end], ecx
    xor ebx, ebx
.overlap_scan:
    cmp ebx, [acpi_srat_memory_count]
    jae .insert
    mov edi, ebx
    imul edi, ACPI_SRAT_MEMORY_RECORD_SIZE
    add edi, acpi_srat_memory_records
    mov eax, [edi + 4]
    mov ecx, eax
    add ecx, [edi + 12]
    mov edx, [acpi_srat_temp_base]
    cmp edx, ecx
    jae .overlap_next
    mov edx, [acpi_srat_temp_end]
    cmp eax, edx
    jb .conflict
.overlap_next:
    inc ebx
    jmp .overlap_scan
.insert:
    cmp ebx, ACPI_SRAT_MEMORY_CAPACITY
    jae .conflict
    mov edi, ebx
    imul edi, ACPI_SRAT_MEMORY_RECORD_SIZE
    add edi, acpi_srat_memory_records
    mov eax, [esi + 4]
    mov [edi], eax                  ; Proximity Domain
    mov eax, [esi + 8]
    mov [edi + 4], eax              ; Base low
    mov dword [edi + 8], 0
    mov eax, [esi + 16]
    mov [edi + 12], eax             ; Length low
    mov dword [edi + 16], 0
    mov eax, [esi + 28]
    mov [edi + 20], eax             ; Enabled/Hotplug/Nonvolatile
    inc dword [acpi_srat_memory_count]
    pop edi
    clc
    ret
.unsupported:
    inc dword [acpi_srat_unsupported_memory]
    pop edi
    clc
    ret
.conflict:
    inc dword [acpi_srat_conflicts]
    pop edi
    stc
    ret

acpi_srat_self_test:
    cmp dword [acpi_srat_valid], 1
    jne .invalid
    cmp dword [acpi_srat_conflicts], 0
    jne .invalid
    xor ecx, ecx
.cpu_next:
    cmp ecx, [acpi_srat_cpu_count]
    jae .complete
    mov edx, ecx
    shl edx, 3
    mov eax, [acpi_srat_cpu_records + edx]
    xor ebx, ebx
.madt_next:
    cmp ebx, [acpi_cpu_count]
    jae .invalid
    cmp eax, [acpi_apic_ids + ebx * 4]
    je .cpu_known
    inc ebx
    jmp .madt_next
.cpu_known:
    inc ecx
    jmp .cpu_next
.complete:
    clc
    ret
.invalid:
    stc
    ret

; EAX=APIC-ID; EAX=validierte Proximity-Domain.
acpi_srat_cpu_domain_lookup:
    cmp dword [acpi_srat_valid], 1
    jne .invalid
    xor ecx, ecx
.scan:
    cmp ecx, [acpi_srat_cpu_count]
    jae .invalid
    mov edx, ecx
    shl edx, 3
    cmp eax, [acpi_srat_cpu_records + edx]
    je .found
    inc ecx
    jmp .scan
.found:
    mov eax, [acpi_srat_cpu_records + edx + 4]
    clc
    ret
.invalid:
    xor eax, eax
    stc
    ret

; EAX=physische 4-KiB-Seite; EAX=validierte Proximity-Domain.
; Die normalisierte SRAT-Tabelle ist der frühe Firmware-Provider für den PMM;
; der Hardware-Graph übernimmt anschließend dieselben Datensätze normativ.
acpi_srat_memory_domain_lookup:
    push ebx
    push ecx
    push edx
    push esi
    push edi
    mov esi, eax
    cmp dword [acpi_srat_valid], 1
    jne .invalid
    xor ecx, ecx
.scan:
    cmp ecx, [acpi_srat_memory_count]
    jae .invalid
    mov edi, ecx
    imul edi, ACPI_SRAT_MEMORY_RECORD_SIZE
    add edi, acpi_srat_memory_records
    mov ebx, [edi + 4]
    cmp esi, ebx
    jb .next
    mov edx, ebx
    add edx, [edi + 12]
    jc .invalid
    mov eax, esi
    add eax, 4096
    jc .invalid
    cmp eax, edx
    jbe .found
.next:
    inc ecx
    jmp .scan
.found:
    mov eax, [edi]
    clc
    jmp .done
.invalid:
    mov eax, 0xFFFFFFFF
    stc
.done:
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    ret

align 4
acpi_cpu_count:          dd 1
acpi_madt_valid:         dd 0
acpi_madt_bsp_seen:      dd 0
acpi_root_entry_size:    dd 0
acpi_root_entries_left:  dd 0
acpi_root_cursor:        dd 0
acpi_apic_ids:           times 8 dd 0
; ---------------------------------------------------------------------------
; ACPI HPET-Tabelle entdecken (NPSPEC-TIME-CLOCKSOURCE-0001).
; Liest die MMIO-Basisadresse aus der ACPI HPET-Tabelle und speichert sie
; in acpi_hpet_base. Falls keine Tabelle vorhanden: acpi_hpet_base bleibt 0.
; ---------------------------------------------------------------------------
acpi_hpet_discover:
    mov dword [acpi_hpet_base], 0
    mov ebp, [kernel_context + CONTEXT_ACPI_ADDRESS]
    test ebp, ebp
    jz .hpet_missing
    ; XSDT oder RSDT wählen
    cmp byte [ebp + 15], 2
    jb .hpet_rsdt
    cmp dword [ebp + 28], 0
    jne .hpet_rsdt
    mov esi, [ebp + 24]
    mov eax, 0x54445358             ; XSDT
    call acpi_table_validate
    jc .hpet_rsdt
    mov dword [acpi_hpet_entry_size], 8
    jmp .hpet_root_ready
.hpet_rsdt:
    mov esi, [ebp + 16]
    mov eax, 0x54445352             ; RSDT
    call acpi_table_validate
    jc .hpet_missing
    mov dword [acpi_hpet_entry_size], 4
.hpet_root_ready:
    ; Anzahl der Einträge berechnen
    mov eax, ecx
    sub eax, 36
    xor edx, edx
    div dword [acpi_hpet_entry_size]
    test edx, edx
    jnz .hpet_missing
    mov [acpi_hpet_entries_left], eax
    lea eax, [esi + 36]
    mov [acpi_hpet_cursor], eax
.hpet_next:
    cmp dword [acpi_hpet_entries_left], 0
    je .hpet_missing
    mov ebx, [acpi_hpet_cursor]
    mov esi, [ebx]
    cmp dword [acpi_hpet_entry_size], 8
    jne .hpet_candidate
    cmp dword [ebx + 4], 0
    jne .hpet_advance
.hpet_candidate:
    mov eax, 0x54455048             ; HPET
    call acpi_table_validate
    jc .hpet_advance
    ; HPET-Tabelle gefunden: Mindestgröße 56 Bytes prüfen
    cmp ecx, 56
    jb .hpet_advance
    ; GAS.AddressSpaceID muss 0 sein (System Memory)
    cmp byte [esi + 40], 0
    jne .hpet_advance
    ; Hohe 32 Bit müssen 0 sein (wir arbeiten in 32-Bit)
    cmp dword [esi + 48], 0
    jne .hpet_advance
    ; Basisadresse aus GAS.Address (low 32 Bit, Offset 44)
    mov eax, [esi + 44]
    test eax, eax
    jz .hpet_advance
    mov [acpi_hpet_base], eax
    ; HPET-Zähler aktivieren: General Configuration Register bei base+0x10
    ; Bit 0 = ENABLE_CNF: Hauptzähler laufen lassen
    or dword [eax + 0x10], 1
    ret
.hpet_advance:
    mov eax, [acpi_hpet_entry_size]
    add [acpi_hpet_cursor], eax
    dec dword [acpi_hpet_entries_left]
    jmp .hpet_next
.hpet_missing:
    ret

acpi_hpet_base:          dd 0
acpi_hpet_entry_size:    dd 0
acpi_hpet_entries_left:  dd 0
acpi_hpet_cursor:        dd 0

acpi_srat_valid:         dd 0
acpi_srat_cpu_count:     dd 0
acpi_srat_memory_count:  dd 0
acpi_srat_conflicts:     dd 0
acpi_srat_unsupported_memory: dd 0
acpi_srat_root_entry_size: dd 0
acpi_srat_root_entries_left: dd 0
acpi_srat_root_cursor:   dd 0
acpi_srat_entry_length:  dd 0
acpi_srat_temp_apic:     dd 0
acpi_srat_temp_domain:   dd 0
acpi_srat_temp_base:     dd 0
acpi_srat_temp_end:      dd 0
align 4
acpi_srat_cpu_records:
    times ACPI_SRAT_CPU_CAPACITY * ACPI_SRAT_CPU_RECORD_SIZE db 0
acpi_srat_memory_records:
    times ACPI_SRAT_MEMORY_CAPACITY * ACPI_SRAT_MEMORY_RECORD_SIZE db 0

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

; stack_canary_seed != 0 nach early_security_entropy_initialize.
early_security_entropy_self_test:
    cmp dword [stack_canary_seed], 0
    jne .ok
    stc
    ret
.ok:
    clc
    ret

