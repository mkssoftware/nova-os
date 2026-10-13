; ---------------------------------------------------------------------------
; Nova Compatibility Layer
; NPSPEC-COMPAT-ABI-0001           - ABI-Kompatibilitaet
; NPSPEC-COMPAT-ABITRANSLATION-0001- ABI-Uebersetzung
; NPSPEC-COMPAT-API-0001           - API-Mapping
; NPSPEC-COMPAT-BINARYTRANSLATION-0001 - Binaer-Uebersetzung (Stub)
; NPSPEC-COMPAT-CPUEMULATION-0001  - CPU-Emulation (Stub)
; NPSPEC-COMPAT-FILEFORMAT-0001    - Dateiformat-Erkennung
; NPSPEC-COMPAT-HARDWAREEMULATION-0001 - Hardware-Emulation (Stub)
; NPSPEC-COMPAT-INTROSPECTION-0001 - Introspection
; NPSPEC-COMPAT-LEGACYDRIVER-0001  - Legacy-Treiber-Shim
; NPSPEC-COMPAT-LEGACYRUNTIME-0001 - Legacy-Runtime
; NPSPEC-COMPAT-LINUX-0001         - Linux-Personality
; NPSPEC-COMPAT-PERSONALITY-0001   - Personality-Modell
; NPSPEC-COMPAT-POSIX-0001         - POSIX-Mapping
; NPSPEC-COMPAT-PROTOCOL-0001      - Protokoll-Kompatibilitaet
; NPSPEC-COMPAT-SANDBOX-0001       - Compat-Sandbox
; NPSPEC-COMPAT-SYSCALL-0001       - Syscall-Uebersetzung
; NPSPEC-COMPAT-TESTING-0001       - Compat-Tests
; NPSPEC-COMPAT-UBUNTU-0001        - Ubuntu-Personality (Stub)
; NPSPEC-COMPAT-VERSIONING-0001    - Versionsmanagement
; NPSPEC-COMPAT-WIN32-0001         - Win32-Personality (Stub)
; ---------------------------------------------------------------------------

; Personality-IDs (NPSPEC-COMPAT-PERSONALITY-0001)
PERS_NONE             equ 0    ; Keine Kompatibilitaet (nativ Nova)
PERS_POSIX            equ 1    ; POSIX.1-2017 (NPSPEC-COMPAT-POSIX-0001)
PERS_LINUX            equ 2    ; Linux-ABI (NPSPEC-COMPAT-LINUX-0001)
PERS_LINUX_UBUNTU     equ 3    ; Ubuntu-Profil (NPSPEC-COMPAT-UBUNTU-0001)
PERS_WIN32            equ 4    ; Win32 (NPSPEC-COMPAT-WIN32-0001, Stub)
PERS_LEGACY_NOVA      equ 5    ; Aeltere Nova-ABI-Version

; Personality-Flags
PERS_FLAG_ACTIVE      equ 0x01  ; Personality aktiv geladen
PERS_FLAG_SANDBOX     equ 0x02  ; In Sandbox ausgefuehrt (NPSPEC-COMPAT-SANDBOX-0001)
PERS_FLAG_SYSCALL_TBL equ 0x04  ; Eigene Syscall-Uebersetzungstabelle
PERS_FLAG_FILEFORMAT  equ 0x08  ; Dateiformat-Handler registriert

; ABI-Klassen (NPSPEC-COMPAT-ABI-0001)
ABI_NOVA              equ 0
ABI_SYSV_AMD64        equ 1    ; System V AMD64 ABI
ABI_WIN64             equ 2    ; Microsoft x64 ABI
ABI_IA32              equ 3    ; IA-32 (32-Bit Legacy)
ABI_ARM64             equ 4    ; AArch64 (Emulation, Stub)

; Syscall-Uebersetzungs-Ergebnis (NPSPEC-COMPAT-SYSCALL-0001)
SCTRANS_OK            equ 0    ; Uebersetzt, ausfuehren
SCTRANS_PASSTHROUGH   equ 1    ; Direkt an Nova-Kernel weiterleiten
SCTRANS_UNSUPPORTED   equ 2    ; Nicht unterstuetzt -> ENOSYS
SCTRANS_BLOCKED       equ 3    ; Von Sandbox blockiert
SCTRANS_ERROR         equ 4    ; Uebersetzungsfehler

; Dateiformat-IDs (NPSPEC-COMPAT-FILEFORMAT-0001)
FILEFMT_UNKNOWN       equ 0
FILEFMT_ELF32         equ 1    ; ELF32 IA-32
FILEFMT_ELF64         equ 2    ; ELF64 AMD64
FILEFMT_PE32          equ 3    ; PE32 (Win32)
FILEFMT_PE64          equ 4    ; PE32+ (Win64)
FILEFMT_NOVABIN       equ 5    ; Nova-Nativformat (NKI)
FILEFMT_SCRIPT        equ 6    ; Shebang-Script

; Versions-Record (16 Bytes, NPSPEC-COMPAT-VERSIONING-0001)
COMPAT_VER_SIZE       equ 16
CV_MAJOR              equ 0
CV_MINOR              equ 4
CV_PATCH              equ 8
CV_FLAGS              equ 12

; Personality-Record (48 Bytes, NPSPEC-COMPAT-PERSONALITY-0001)
PERS_REC_SIZE         equ 48
PR_ID                 equ 0    ; PERS_*
PR_FLAGS              equ 4    ; PERS_FLAG_*
PR_ABI                equ 8    ; ABI_*
PR_SYSCALL_BASE       equ 12   ; Basisnummer fremder Syscalls
PR_SYSCALL_COUNT      equ 16   ; Anzahl unterstuetzter Syscalls
PR_SANDBOX_CAPS       equ 20   ; erlaubte Capabilities in Sandbox
PR_MIN_VER            equ 24   ; Mindestversion (COMPAT_VER_SIZE)
PR_NAME               equ 40   ; 8-Byte-ASCII-Tag ("POSIX   " etc.)

PERS_CAPACITY         equ 4    ; max. 4 Personalities aktiv

; Syscall-Mapping-Eintrag (16 Bytes, NPSPEC-COMPAT-SYSCALL-0001)
SC_MAP_SIZE           equ 16
SM_FOREIGN            equ 0    ; Fremde Syscall-Nummer
SM_NOVA               equ 4    ; Nova-interne Syscall-Nummer (oder -1)
SM_FLAGS              equ 8    ; SCTRANS_*
SM_RESERVED           equ 12

SC_MAP_CAPACITY       equ 32   ; max. 32 Syscall-Mappings

; Linux-Syscall-Nummern (Subset, NPSPEC-COMPAT-LINUX-0001)
LINUX_SYS_READ        equ 0
LINUX_SYS_WRITE       equ 1
LINUX_SYS_OPEN        equ 2
LINUX_SYS_CLOSE       equ 3
LINUX_SYS_EXIT        equ 60
LINUX_SYS_EXIT_GROUP  equ 231
LINUX_SYS_GETPID      equ 39
LINUX_SYS_MMAP        equ 9
LINUX_SYS_MUNMAP      equ 11
LINUX_SYS_BRK         equ 12

; Nova-interne Syscall-Nummern (vereinfacht)
NOVA_SYS_READ         equ 0x10
NOVA_SYS_WRITE        equ 0x11
NOVA_SYS_OPEN         equ 0x12
NOVA_SYS_CLOSE        equ 0x13
NOVA_SYS_EXIT         equ 0x1E
NOVA_SYS_GETPID       equ 0x20
NOVA_SYS_MMAP         equ 0x30

; ---------------------------------------------------------------------------
; compat_initialize
; CF=0 Erfolg
; ---------------------------------------------------------------------------
compat_initialize:
    mov edi, compat_personalities
    xor eax, eax
    mov ecx, (PERS_CAPACITY * PERS_REC_SIZE) / 4
    rep stosd
    mov edi, compat_sc_map
    mov ecx, (SC_MAP_CAPACITY * SC_MAP_SIZE) / 4
    rep stosd
    mov dword [compat_pers_count], 0
    mov dword [compat_sc_count], 0
    mov dword [compat_translations], 0
    mov dword [compat_blocked], 0
    mov dword [compat_unsupported], 0

    ; Linux-Personality vordefinieren (NPSPEC-COMPAT-LINUX-0001)
    call compat_register_linux_personality

    mov dword [compat_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; compat_register_linux_personality
; Registriert Linux-Personality mit POSIX-ABI und Syscall-Mappings.
; ---------------------------------------------------------------------------
compat_register_linux_personality:
    push edi
    ; Personality-Slot
    mov edi, compat_personalities
    mov dword [edi + PR_ID], PERS_LINUX
    mov dword [edi + PR_FLAGS], PERS_FLAG_ACTIVE | PERS_FLAG_SYSCALL_TBL
    mov dword [edi + PR_ABI], ABI_SYSV_AMD64
    mov dword [edi + PR_SYSCALL_BASE], 0
    mov dword [edi + PR_SYSCALL_COUNT], 256
    mov dword [edi + PR_SANDBOX_CAPS], 0xFFFFFFFF
    ; Name "LINUX   "
    mov dword [edi + PR_NAME], 0x4E554C4C      ; "LLUN" (LE)
    mov dword [edi + PR_NAME + 4], 0x20205849  ; "IX  " (LE)
    inc dword [compat_pers_count]

    ; Syscall-Mappings eintragen
    ; Linux read(0) -> Nova read(0x10)
    call .add_sc
    dd LINUX_SYS_READ, NOVA_SYS_READ, SCTRANS_OK, 0
    call .add_sc
    dd LINUX_SYS_WRITE, NOVA_SYS_WRITE, SCTRANS_OK, 0
    call .add_sc
    dd LINUX_SYS_OPEN, NOVA_SYS_OPEN, SCTRANS_OK, 0
    call .add_sc
    dd LINUX_SYS_CLOSE, NOVA_SYS_CLOSE, SCTRANS_OK, 0
    call .add_sc
    dd LINUX_SYS_EXIT, NOVA_SYS_EXIT, SCTRANS_OK, 0
    call .add_sc
    dd LINUX_SYS_EXIT_GROUP, NOVA_SYS_EXIT, SCTRANS_OK, 0
    call .add_sc
    dd LINUX_SYS_GETPID, NOVA_SYS_GETPID, SCTRANS_OK, 0
    call .add_sc
    dd LINUX_SYS_MMAP, NOVA_SYS_MMAP, SCTRANS_OK, 0
    ; munmap und brk: nicht vollstaendig implementiert
    call .add_sc
    dd LINUX_SYS_MUNMAP, -1, SCTRANS_UNSUPPORTED, 0
    call .add_sc
    dd LINUX_SYS_BRK, -1, SCTRANS_UNSUPPORTED, 0

    pop edi
    ret

; Interne Hilfsfunktion: naechste 4 DWords vom Call-Stack als SC_MAP-Eintrag
.add_sc:
    pop esi                  ; Ruecksprungadresse = direkt nach dd-Daten
    push edi
    cmp dword [compat_sc_count], SC_MAP_CAPACITY
    jae .skip
    ; Slot im sc_map suchen
    mov edi, compat_sc_map
    push ecx
    mov ecx, [compat_sc_count]
    imul ecx, SC_MAP_SIZE
    add edi, ecx
    pop ecx
    ; Daten von ESI (liegt hinter dem call-Befehl)
    mov eax, [esi]
    mov [edi + SM_FOREIGN], eax
    mov eax, [esi + 4]
    mov [edi + SM_NOVA], eax
    mov eax, [esi + 8]
    mov [edi + SM_FLAGS], eax
    inc dword [compat_sc_count]
    add esi, 16              ; 4 DWords ueberspringen
.skip:
    pop edi
    jmp esi                  ; Ruecksprung hinter die dd-Daten

; ---------------------------------------------------------------------------
; compat_translate_syscall: EAX=foreign_syscall_nr, EBX=personality_id
; Rueckgabe: EAX=nova_syscall_nr, EDX=translation_result (SCTRANS_*)
; CF=0 Erfolg
; ---------------------------------------------------------------------------
compat_translate_syscall:
    push esi
    push ecx
    mov esi, compat_sc_map
    mov ecx, [compat_sc_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + SM_FOREIGN], eax
    je .found
    add esi, SC_MAP_SIZE
    dec ecx
    jmp .scan
.found:
    mov eax, [esi + SM_NOVA]
    mov edx, [esi + SM_FLAGS]
    inc dword [compat_translations]
    cmp edx, SCTRANS_UNSUPPORTED
    je .unsup
    cmp edx, SCTRANS_BLOCKED
    je .blocked
    pop ecx
    pop esi
    clc
    ret
.unsup:
    inc dword [compat_unsupported]
    pop ecx
    pop esi
    clc
    ret
.blocked:
    inc dword [compat_blocked]
    pop ecx
    pop esi
    clc
    ret
.not_found:
    mov eax, -1
    mov edx, SCTRANS_UNSUPPORTED
    inc dword [compat_unsupported]
    pop ecx
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; compat_detect_file_format: ESI=Dateikopf (mind. 8 Bytes), ECX=Laenge
; Rueckgabe: EAX=FILEFMT_* (NPSPEC-COMPAT-FILEFORMAT-0001)
; ---------------------------------------------------------------------------
compat_detect_file_format:
    test ecx, ecx
    jz .unknown

    ; ELF-Magic: 0x7F 'E' 'L' 'F'
    cmp dword [esi], 0x464C457F
    jne .try_pe
    ; ELF32 vs ELF64 (e_ident[EI_CLASS] = Byte 4)
    cmp byte [esi + 4], 1    ; ELFCLASS32
    je .elf32
    cmp byte [esi + 4], 2    ; ELFCLASS64
    je .elf64
    jmp .unknown
.elf32:
    mov eax, FILEFMT_ELF32
    ret
.elf64:
    mov eax, FILEFMT_ELF64
    ret

.try_pe:
    ; PE-Magic: 'M' 'Z' = 0x5A4D
    cmp word [esi], 0x5A4D
    jne .try_nova
    mov eax, FILEFMT_PE32    ; PE32 oder PE32+, genauere Pruefung noetig
    ret

.try_nova:
    ; NKI-Magic: 'N' 'K' 'I' '1' oder 'N' 'K' 'I' '2'
    cmp dword [esi], 0x324B4E   ; "NK2" (little endian, vereinfacht)
    je .nova
    cmp dword [esi], 0x314B4E   ; "NK1"
    jne .try_script
.nova:
    mov eax, FILEFMT_NOVABIN
    ret

.try_script:
    ; Shebang: '#' '!'
    cmp word [esi], 0x2123   ; "!#" LE -> tatsaechlich "#!" = 0x2321
    je .script
    cmp word [esi], 0x2321
    jne .unknown
.script:
    mov eax, FILEFMT_SCRIPT
    ret

.unknown:
    mov eax, FILEFMT_UNKNOWN
    ret

; ---------------------------------------------------------------------------
; compat_get_personality: EAX=PERS_* -> ESI=Record (CF=0) oder CF=1
; ---------------------------------------------------------------------------
compat_get_personality:
    push ecx
    push edi
    mov edi, compat_personalities
    mov ecx, PERS_CAPACITY
.scan:
    test ecx, ecx
    jz .not_found
    cmp dword [edi + PR_ID], eax
    je .found
    add edi, PERS_REC_SIZE
    dec ecx
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

; ---------------------------------------------------------------------------
; compat_introspect: EAX=PERS_* -> EDX=sc_count, ECX=pers_flags
; (NPSPEC-COMPAT-INTROSPECTION-0001)
; ---------------------------------------------------------------------------
compat_introspect:
    call compat_get_personality
    jc .unknown
    mov edx, [esi + PR_SYSCALL_COUNT]
    mov ecx, [esi + PR_FLAGS]
    clc
    ret
.unknown:
    xor edx, edx
    xor ecx, ecx
    stc
    ret

; ---------------------------------------------------------------------------
; compat_self_test (NPSPEC-COMPAT-TESTING-0001)
; ---------------------------------------------------------------------------
compat_self_test:
    push esi
    xor esi, esi

    cmp dword [compat_ready], 1
    je .t2
    inc esi
.t2:
    ; Dateiformat-Erkennung: ELF64-Magic
    push edi
    mov edi, compat_test_buf
    mov dword [edi], 0x464C457F  ; ELF-Magic
    mov byte [edi + 4], 2        ; ELFCLASS64
    mov esi, edi
    pop edi
    push esi
    mov ecx, 8
    call compat_detect_file_format
    pop esi
    cmp eax, FILEFMT_ELF64
    je .t3
    inc esi
.t3:
    ; Syscall-Uebersetzung: Linux read(0) -> Nova read(0x10)
    mov eax, LINUX_SYS_READ
    mov ebx, PERS_LINUX
    call compat_translate_syscall
    cmp eax, NOVA_SYS_READ
    je .t4
    inc esi
.t4:
    ; Personality-Introspection
    mov eax, PERS_LINUX
    call compat_introspect
    jnc .done
    inc esi

.done:
    test esi, esi
    jz .ok
    pop esi
    stc
    ret
.ok:
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; Statische Daten
; ---------------------------------------------------------------------------
align 4
compat_ready:          dd 0
compat_pers_count:     dd 0
compat_sc_count:       dd 0
compat_translations:   dd 0
compat_blocked:        dd 0
compat_unsupported:    dd 0

align 4
compat_personalities:
    times PERS_CAPACITY * PERS_REC_SIZE db 0

align 4
compat_sc_map:
    times SC_MAP_CAPACITY * SC_MAP_SIZE db 0

align 4
compat_test_buf:
    times 16 db 0
