; ---------------------------------------------------------------------------
; Nova Privacy-Subsystem Â§020
; NPSPEC-PRIVACY-DIFFERENTIAL-0001    - Differential Privacy
; NPSPEC-PRIVACY-DLP-0001             - Data Loss Prevention
; NPSPEC-PRIVACY-EXPIRATION-0001      - Daten-Ablauf
; NPSPEC-PRIVACY-LABEL-0001           - Privacy-Labels
; NPSPEC-PRIVACY-MINIMIZATION-0001    - Datensparsamkeit
; NPSPEC-PRIVACY-RETENTION-0001       - Aufbewahrungsfristen
; NPSPEC-PRIVACY-SELECTIVEDISCLOSURE-0001 - Selektive Offenbarung
; NPSPEC-PRIVACY-SOVEREIGNTY-0001     - Datensouveranitaet
; ---------------------------------------------------------------------------

; Privacy-Labels (NPSPEC-PRIVACY-LABEL-0001)
PRIV2_LABEL_PUBLIC     equ 0    ; Oeffentlich
PRIV2_LABEL_INTERNAL   equ 1    ; Intern
PRIV2_LABEL_SENSITIVE  equ 2    ; Sensibel
PRIV2_LABEL_PERSONAL   equ 3    ; Personenbezogen
PRIV2_LABEL_SECRET     equ 4    ; Geheim

; Aufbewahrungsklassen (NPSPEC-PRIVACY-RETENTION-0001)
PRIV_RETAIN_SESSION   equ 0    ; Nur Sitzung
PRIV_RETAIN_DAY       equ 1    ; 1 Tag
PRIV_RETAIN_WEEK      equ 2    ; 7 Tage
PRIV_RETAIN_MONTH     equ 3    ; 30 Tage
PRIV_RETAIN_PERMANENT equ 4    ; Dauerhaft

; Souveraenitaet-Flags (NPSPEC-PRIVACY-SOVEREIGNTY-0001)
PRIV_SOV_LOCAL_ONLY   equ 0x01  ; Nur lokale Verarbeitung
PRIV_SOV_NO_EXPORT    equ 0x02  ; Kein Export
PRIV_SOV_ENCRYPTED    equ 0x04  ; Verschluesselt
PRIV_SOV_AUDITED      equ 0x08  ; Zugriff protokolliert

; DLP-Aktionen (NPSPEC-PRIVACY-DLP-0001)
PRIV_DLP_ALLOW        equ 0
PRIV_DLP_WARN         equ 1
PRIV_DLP_BLOCK        equ 2
PRIV_DLP_REDACT       equ 3    ; (NPSPEC-PRIVACY-SELECTIVEDISCLOSURE-0001)

; Privacy-Record (32 Bytes, NPSPEC-PRIVACY-LABEL-0001)
PRIV2_REC_SIZE         equ 32
PV_DATA_ID            equ 0    ; Daten-Identifikator
PV_LABEL              equ 4    ; PRIV_LABEL_*
PV_RETENTION          equ 8    ; PRIV_RETAIN_*
PV_SOV_FLAGS          equ 12   ; PRIV_SOV_*
PV_EXPIRY_TICKS       equ 16   ; (NPSPEC-PRIVACY-EXPIRATION-0001)
PV_DLP_ACTION         equ 20   ; (NPSPEC-PRIVACY-DLP-0001)
PV_NOISE_EPSILON      equ 24   ; Differential-Privacy-Epsilon *100 (NPSPEC-PRIVACY-DIFFERENTIAL-0001)
PV_ACCESS_COUNT       equ 28   ; Zugriffszaehler (Minimierung)

PRIV_CAPACITY         equ 16

; ---------------------------------------------------------------------------
; privacy_ext_initialize
; ---------------------------------------------------------------------------
privacy_ext_initialize:
    push edi
    mov edi, priv2_table
    xor eax, eax
    mov ecx, (PRIV_CAPACITY * PRIV2_REC_SIZE) / 4
    rep stosd
    pop edi
    mov dword [priv2_count], 0
    mov dword [priv2_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; privacy_ext_label_data: EAX=data_id, EBX=label, ECX=retention, EDX=sov_flags
; (NPSPEC-PRIVACY-LABEL-0001, NPSPEC-PRIVACY-MINIMIZATION-0001)
; CF=0 Erfolg
; ---------------------------------------------------------------------------
privacy_ext_label_data:
    cmp dword [priv2_count], PRIV_CAPACITY
    jae .full
    push edi
    push esi
    mov edi, priv2_table
    push eax
    mov eax, [priv2_count]
    imul eax, PRIV2_REC_SIZE
    add edi, eax
    pop eax
    mov [edi + PV_DATA_ID], eax
    mov [edi + PV_LABEL], ebx
    mov [edi + PV_RETENTION], ecx
    mov [edi + PV_SOV_FLAGS], edx
    ; Ablaufzeit berechnen (NPSPEC-PRIVACY-EXPIRATION-0001)
    push eax
    mov eax, [timer_ticks]
    cmp ecx, PRIV_RETAIN_SESSION
    je .session_expiry
    cmp ecx, PRIV_RETAIN_DAY
    je .day_expiry
    cmp ecx, PRIV_RETAIN_PERMANENT
    je .no_expiry
    ; Default: Woche
    add eax, 60480000   ; 7 Tage * 24h * 60min * 60s * 100Hz
    jmp .set_expiry
.session_expiry:
    add eax, 360000     ; 1h * 100Hz
    jmp .set_expiry
.day_expiry:
    add eax, 8640000    ; 24h * 100Hz
    jmp .set_expiry
.no_expiry:
    mov eax, 0xFFFFFFFF
.set_expiry:
    mov [edi + PV_EXPIRY_TICKS], eax
    pop eax
    mov dword [edi + PV_DLP_ACTION], PRIV_DLP_ALLOW
    mov dword [edi + PV_NOISE_EPSILON], 100  ; epsilon=1.0
    mov dword [edi + PV_ACCESS_COUNT], 0
    inc dword [priv2_count]
    pop esi
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; privacy_ext_check_access: EAX=data_id -> EBX=dlp_action, CF=0 ok
; (NPSPEC-PRIVACY-DLP-0001, NPSPEC-PRIVACY-EXPIRATION-0001)
; ---------------------------------------------------------------------------
privacy_ext_check_access:
    push esi
    push ecx
    mov esi, priv2_table
    mov ecx, [priv2_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + PV_DATA_ID], eax
    je .found
    add esi, PRIV2_REC_SIZE
    dec ecx
    jmp .scan
.found:
    ; Ablauf pruefen
    mov ebx, [esi + PV_EXPIRY_TICKS]
    cmp ebx, 0xFFFFFFFF
    je .check_dlp
    mov ecx, [timer_ticks]
    cmp ecx, ebx
    jb .check_dlp
    ; Abgelaufen: blockieren
    mov ebx, PRIV_DLP_BLOCK
    pop ecx
    pop esi
    clc
    ret
.check_dlp:
    ; Zugriffszaehler erhoehen (Minimierung)
    inc dword [esi + PV_ACCESS_COUNT]
    mov ebx, [esi + PV_DLP_ACTION]
    pop ecx
    pop esi
    clc
    ret
.not_found:
    mov ebx, PRIV_DLP_ALLOW   ; Unlabeled: erlaubt
    pop ecx
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; privacy_ext_self_test
; ---------------------------------------------------------------------------
privacy_ext_self_test:
    push esi
    xor esi, esi
    cmp dword [priv2_ready], 1
    je .t2
    inc esi
.t2:
    mov eax, 0x1234
    mov ebx, PRIV2_LABEL_PERSONAL
    mov ecx, PRIV_RETAIN_SESSION
    mov edx, PRIV_SOV_LOCAL_ONLY | PRIV_SOV_AUDITED
    call privacy_ext_label_data
    jnc .t3
    inc esi
.t3:
    mov eax, 0x1234
    call privacy_ext_check_access
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
priv2_ready:   dd 0
priv2_count:   dd 0

align 4
priv2_table:
    times PRIV_CAPACITY * PRIV2_REC_SIZE db 0

