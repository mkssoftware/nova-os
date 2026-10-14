; ---------------------------------------------------------------------------
; Nova Semantic-Layer §024
; NPSPEC-SEMANTIC-CAPABILITY-0001    - Capability-Semantik
; NPSPEC-SEMANTIC-COMPATIBILITY-0001 - Kompatibilitaet
; NPSPEC-SEMANTIC-CONVERSION-0001    - Konvertierung
; NPSPEC-SEMANTIC-DISCOVERY-0001     - Entdeckung
; NPSPEC-SEMANTIC-EXECUTION-0001     - Ausfuehrungs-Semantik
; NPSPEC-SEMANTIC-FILE-0001          - Datei-Semantik
; NPSPEC-SEMANTIC-IPC-0001           - IPC-Semantik
; NPSPEC-SEMANTIC-METADATA-0001      - Metadaten
; NPSPEC-SEMANTIC-QUERY-0001         - Abfragen
; NPSPEC-SEMANTIC-RELATIONSHIP-0001  - Beziehungen
; NPSPEC-SEMANTIC-RESOURCE-0001      - Ressourcen-Semantik
; NPSPEC-SEMANTIC-TYPE-0001          - Typensystem
; NPSPEC-SEMANTIC-VALIDATION-0001    - Validierung
; ---------------------------------------------------------------------------

; Semantische Typen (NPSPEC-SEMANTIC-TYPE-0001)
STYPE_UNKNOWN         equ 0
STYPE_INTEGER         equ 1
STYPE_STRING          equ 2
STYPE_BLOB            equ 3
STYPE_CAPABILITY      equ 4    ; (NPSPEC-SEMANTIC-CAPABILITY-0001)
STYPE_RESOURCE        equ 5    ; (NPSPEC-SEMANTIC-RESOURCE-0001)
STYPE_FILE            equ 6    ; (NPSPEC-SEMANTIC-FILE-0001)
STYPE_IPC_ENDPOINT    equ 7    ; (NPSPEC-SEMANTIC-IPC-0001)
STYPE_EXECUTION_CTX   equ 8    ; (NPSPEC-SEMANTIC-EXECUTION-0001)
STYPE_COUNT           equ 9

; Relationship-Typen (NPSPEC-SEMANTIC-RELATIONSHIP-0001)
SREL_NONE             equ 0
SREL_DEPENDS_ON       equ 1
SREL_PROVIDES         equ 2
SREL_CONSUMES         equ 3
SREL_COMPATIBLE       equ 4    ; (NPSPEC-SEMANTIC-COMPATIBILITY-0001)

; Validation-Ergebnis (NPSPEC-SEMANTIC-VALIDATION-0001)
SVAL_OK               equ 0
SVAL_TYPE_MISMATCH    equ 1
SVAL_RANGE_ERROR      equ 2
SVAL_COMPAT_ERROR     equ 3    ; (NPSPEC-SEMANTIC-COMPATIBILITY-0001)

; Semantik-Record (40 Bytes)
SEM_REC_SIZE          equ 40
SE_ID                 equ 0    ; Semantik-ID
SE_NAME_HASH          equ 4    ; Hash des Namens (NPSPEC-SEMANTIC-DISCOVERY-0001)
SE_TYPE               equ 8    ; STYPE_*
SE_FLAGS              equ 12
SE_METADATA_PTR       equ 16   ; Metadaten-Zeiger (NPSPEC-SEMANTIC-METADATA-0001)
SE_CONVERT_FN         equ 20   ; Konvertierungs-Funktion (NPSPEC-SEMANTIC-CONVERSION-0001)
SE_VALIDATE_FN        equ 24   ; Validierungs-Funktion (NPSPEC-SEMANTIC-VALIDATION-0001)
SE_RELATIONSHIP       equ 28   ; SREL_*
SE_RELATED_ID         equ 32   ; Ziel der Beziehung (NPSPEC-SEMANTIC-RELATIONSHIP-0001)
; 36..39: reserviert

SEM_CAPACITY          equ 16

; ---------------------------------------------------------------------------
; semmgr_initialize
; ---------------------------------------------------------------------------
semmgr_initialize:
    push edi
    mov edi, semmgr_table
    xor eax, eax
    mov ecx, (SEM_CAPACITY * SEM_REC_SIZE) / 4
    rep stosd
    pop edi
    mov dword [semmgr_count], 0
    mov dword [semmgr_next_id], 1
    mov dword [semmgr_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; semmgr_register: EAX=name_hash, EBX=type, ECX=flags
; (NPSPEC-SEMANTIC-TYPE-0001, NPSPEC-SEMANTIC-DISCOVERY-0001)
; CF=0 -> EDX=sem_id
; ---------------------------------------------------------------------------
semmgr_register:
    cmp dword [semmgr_count], SEM_CAPACITY
    jae .full
    cmp ebx, STYPE_COUNT
    jae .invalid
    push edi
    mov edi, semmgr_table
    push eax
    mov eax, [semmgr_count]
    imul eax, SEM_REC_SIZE
    add edi, eax
    pop eax
    mov edx, [semmgr_next_id]
    mov [edi + SE_ID], edx
    mov [edi + SE_NAME_HASH], eax
    mov [edi + SE_TYPE], ebx
    mov [edi + SE_FLAGS], ecx
    mov dword [edi + SE_METADATA_PTR], 0
    mov dword [edi + SE_CONVERT_FN], 0
    mov dword [edi + SE_VALIDATE_FN], 0
    mov dword [edi + SE_RELATIONSHIP], SREL_NONE
    mov dword [edi + SE_RELATED_ID], 0
    inc dword [semmgr_next_id]
    inc dword [semmgr_count]
    pop edi
    clc
    ret
.full:
.invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; semmgr_query: EAX=name_hash -> EDI=record_ptr, CF=0 gefunden
; (NPSPEC-SEMANTIC-QUERY-0001, NPSPEC-SEMANTIC-DISCOVERY-0001)
; ---------------------------------------------------------------------------
semmgr_query:
    push esi
    push ecx
    mov esi, semmgr_table
    mov ecx, [semmgr_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + SE_NAME_HASH], eax
    je .found
    add esi, SEM_REC_SIZE
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
; semmgr_validate: EAX=sem_id, EBX=value -> ECX=result
; (NPSPEC-SEMANTIC-VALIDATION-0001)
; ---------------------------------------------------------------------------
semmgr_validate:
    push esi
    push edx
    mov esi, semmgr_table
    mov edx, [semmgr_count]
.scan:
    test edx, edx
    jz .not_found
    cmp [esi + SE_ID], eax
    je .found
    add esi, SEM_REC_SIZE
    dec edx
    jmp .scan
.found:
    ; Validate-Funktion aufrufen wenn vorhanden
    cmp dword [esi + SE_VALIDATE_FN], 0
    je .default_ok
    call [esi + SE_VALIDATE_FN]
    jmp .done
.default_ok:
    mov ecx, SVAL_OK
.done:
    pop edx
    pop esi
    clc
    ret
.not_found:
    mov ecx, SVAL_TYPE_MISMATCH
    pop edx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; semmgr_self_test
; ---------------------------------------------------------------------------
semmgr_self_test:
    push esi
    xor esi, esi
    cmp dword [semmgr_ready], 1
    je .t2
    inc esi
.t2:
    ; Capability-Typ registrieren
    mov eax, 0xCAFE0001    ; Name-Hash
    mov ebx, STYPE_CAPABILITY
    mov ecx, 0
    call semmgr_register
    jnc .t3
    inc esi
.t3:
    mov eax, 0xCAFE0001
    call semmgr_query
    jnc .t4
    inc esi
.t4:
    ; Validierung
    mov eax, 1
    mov ebx, 0
    call semmgr_validate
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
semmgr_ready:    dd 0
semmgr_count:    dd 0
semmgr_next_id:  dd 1

align 4
semmgr_table:
    times SEM_CAPACITY * SEM_REC_SIZE db 0
