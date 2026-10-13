; ---------------------------------------------------------------------------
; Nova Globalization / Internationalization
; NPSPEC-GLOBALIZATION-LOCALE-0001 / NPSPEC-GLOBALIZATION-LANGUAGE-0001
; NPSPEC-GLOBALIZATION-REGION-0001 / NPSPEC-GLOBALIZATION-DATETIME-0001
; NPSPEC-GLOBALIZATION-NUMBER-0001 / NPSPEC-GLOBALIZATION-COLLATION-0001
; NPSPEC-GLOBALIZATION-PLURAL-0001 / NPSPEC-GLOBALIZATION-CALENDAR-0001
; NPSPEC-GLOBALIZATION-RTL-0001 / NPSPEC-GLOBALIZATION-RESOURCE-0001
; NPSPEC-GLOBALIZATION-NEGOTIATION-0001 / NPSPEC-GLOBALIZATION-INTROSPECTION-0001
; ---------------------------------------------------------------------------

; Sprachkennungen (BCP-47-inspiriert, 2+2 Bytes = 4-Byte-Tag)
; Format: lang[2] | region[2] (ASCII, 0-terminiert im High-Byte)
LANG_NONE             equ 0x00000000
LANG_DE_DE            equ 0x65440044   ; "De" | "DE"  (vereinfacht)
LANG_DE_AT            equ 0x65440054   ; "De" | "AT"
LANG_DE_CH            equ 0x65440048   ; "De" | "CH"
LANG_EN_US            equ 0x6E450053   ; "En" | "US"
LANG_EN_GB            equ 0x6E450042   ; "En" | "GB"
LANG_FR_FR            equ 0x72460046   ; "Fr" | "FR"
LANG_JA_JP            equ 0x614A0050   ; "Ja" | "JP"
LANG_ZH_CN            equ 0x685A004E   ; "Zh" | "CN"

; Schreibrichtungen (NPSPEC-GLOBALIZATION-RTL-0001)
GLOB_DIR_LTR          equ 0    ; links-nach-rechts
GLOB_DIR_RTL          equ 1    ; rechts-nach-links
GLOB_DIR_TTB          equ 2    ; oben-nach-unten
GLOB_DIR_BIDI         equ 3    ; bidirektional

; Kalender-Typen (NPSPEC-GLOBALIZATION-CALENDAR-0001)
CAL_GREGORIAN         equ 0
CAL_ISLAMIC           equ 1
CAL_HEBREW            equ 2
CAL_JAPANESE          equ 3
CAL_CHINESE           equ 4

; Zahlenformat-Flags (NPSPEC-GLOBALIZATION-NUMBER-0001)
NUMFMT_DECIMAL_COMMA  equ 0x01   ; , als Dezimaltrennzeichen
NUMFMT_GROUP_DOT      equ 0x02   ; . als Tausendertrennzeichen
NUMFMT_GROUP_SPACE    equ 0x04   ; Leerzeichen als Tausendertrennzeichen
NUMFMT_CURRENCY_PRE   equ 0x08   ; W‰hrungssymbol vorne

; Plural-Kategorien (NPSPEC-GLOBALIZATION-PLURAL-0001)
PLURAL_ZERO           equ 0
PLURAL_ONE            equ 1
PLURAL_TWO            equ 2
PLURAL_FEW            equ 3
PLURAL_MANY           equ 4
PLURAL_OTHER          equ 5

; Locale-Record (48 Bytes, NPSPEC-GLOBALIZATION-LOCALE-0001)
LOCALE_SIZE           equ 48
LOC_LANG              equ 0    ; LANG_* (4 Bytes)
LOC_REGION            equ 4    ; Region-Code (4 Bytes)
LOC_DIRECTION         equ 8    ; GLOB_DIR_*
LOC_CALENDAR          equ 12   ; CAL_*
LOC_NUMFMT_FLAGS      equ 16   ; NUMFMT_*
LOC_DATE_ORDER        equ 20   ; 0=DMY, 1=MDY, 2=YMD
LOC_TIME_24H          equ 24   ; 1=24h, 0=12h
LOC_DECIMAL_SEP       equ 28   ; ASCII-Zeichen des Dezimaltrenners
LOC_GROUP_SEP         equ 32   ; ASCII-Zeichen des Tausendertrenners
LOC_CURRENCY_CODE     equ 36   ; ISO-4217-3-Byte-Code + 0 (4 Bytes)
LOC_RESERVED          equ 40

LOCALE_CAPACITY       equ 4    ; max. 4 Locales gleichzeitig geladen

; Sprachnegotiation-Priorit‰tsliste (NPSPEC-GLOBALIZATION-NEGOTIATION-0001)
LANG_PRIO_CAPACITY    equ 8    ; max. 8 Eintr‰ge

; ---------------------------------------------------------------------------
; globalization_initialize
; Setzt Default-Locale auf de-DE.
; CF=0 Erfolg
; ---------------------------------------------------------------------------
globalization_initialize:
    mov edi, glob_locales
    xor eax, eax
    mov ecx, (LOCALE_CAPACITY * LOCALE_SIZE) / 4
    rep stosd

    ; Default-Locale anlegen: de-DE
    call glob_setup_default_locale

    ; Negotiation-Liste mit Default besetzen
    mov dword [glob_prio_list], LANG_DE_DE
    mov dword [glob_prio_count], 1
    mov dword [glob_current_lang], LANG_DE_DE
    mov dword [glob_current_dir], GLOB_DIR_LTR

    mov dword [glob_mgr_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; glob_setup_default_locale
; Schreibt de-DE Locale-Record in Slot 0.
; ---------------------------------------------------------------------------
glob_setup_default_locale:
    mov edi, glob_locales
    mov dword [edi + LOC_LANG], LANG_DE_DE
    mov dword [edi + LOC_REGION], LANG_DE_DE
    mov dword [edi + LOC_DIRECTION], GLOB_DIR_LTR
    mov dword [edi + LOC_CALENDAR], CAL_GREGORIAN
    mov dword [edi + LOC_NUMFMT_FLAGS], NUMFMT_DECIMAL_COMMA | NUMFMT_GROUP_DOT
    mov dword [edi + LOC_DATE_ORDER], 0        ; D.M.Y
    mov dword [edi + LOC_TIME_24H], 1          ; 24h
    mov dword [edi + LOC_DECIMAL_SEP], 0x2C   ; ','
    mov dword [edi + LOC_GROUP_SEP], 0x2E     ; '.'
    ; Currency: EUR
    mov dword [edi + LOC_CURRENCY_CODE], 0x00525545 ; "EUR"
    inc dword [glob_locale_count]
    ret

; ---------------------------------------------------------------------------
; glob_set_language: EAX=LANG_*-Tag
; Setzt die aktive Sprache; RTL-Richtung bei Bedarf.
; CF=0 Erfolg, CF=1 ung‹ltiger Tag
; ---------------------------------------------------------------------------
glob_set_language:
    test eax, eax
    jz .invalid
    mov [glob_current_lang], eax
    ; RTL-Sprachen (Arabisch=0x41720000, Hebr‰isch=0x65480000)
    cmp eax, 0x41720000
    jae .check_rtl
    jmp .ltr
.check_rtl:
    cmp eax, 0x42000000        ; Grober Bereich fuer RTL-Codes
    jb .rtl
.ltr:
    mov dword [glob_current_dir], GLOB_DIR_LTR
    clc
    ret
.rtl:
    mov dword [glob_current_dir], GLOB_DIR_RTL
    clc
    ret
.invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; glob_get_plural_category: EAX=Zahl -> EAX=PLURAL_*
; Germanische Pluralregel: n==1 -> ONE, sonst OTHER.
; (NPSPEC-GLOBALIZATION-PLURAL-0001)
; ---------------------------------------------------------------------------
glob_get_plural_category:
    cmp eax, 1
    je .one
    mov eax, PLURAL_OTHER
    ret
.one:
    mov eax, PLURAL_ONE
    ret

; ---------------------------------------------------------------------------
; glob_negotiate_language: ESI=Anforderungsliste (LANG_*-Array), ECX=Laenge
; -> EAX=ausgehandelter LANG_*-Tag oder LANG_NONE (NPSPEC-GLOBALIZATION-NEGOTIATION-0001)
; ---------------------------------------------------------------------------
glob_negotiate_language:
    push ecx
    push esi
    test ecx, ecx
    jz .none
    ; Ersten Eintrag nehmen der in der geladenen Liste ist
.scan:
    test ecx, ecx
    jz .none
    mov eax, [esi]
    push ecx
    push esi
    call glob_is_supported
    pop esi
    pop ecx
    jnc .found
    add esi, 4
    dec ecx
    jmp .scan
.found:
    pop esi
    pop ecx
    ret
.none:
    mov eax, LANG_NONE
    pop esi
    pop ecx
    ret

; ---------------------------------------------------------------------------
; glob_is_supported: EAX=LANG_* -> CF=0 wenn unterstuetzt, CF=1 wenn nicht
; ---------------------------------------------------------------------------
glob_is_supported:
    push ecx
    push edi
    mov edi, glob_locales
    mov ecx, [glob_locale_count]
.scan:
    test ecx, ecx
    jz .no
    cmp [edi + LOC_LANG], eax
    je .yes
    add edi, LOCALE_SIZE
    dec ecx
    jmp .scan
.yes:
    pop edi
    pop ecx
    clc
    ret
.no:
    pop edi
    pop ecx
    stc
    ret

; ---------------------------------------------------------------------------
; globalization_self_test
; ---------------------------------------------------------------------------
globalization_self_test:
    push esi
    xor esi, esi

    cmp dword [glob_mgr_ready], 1
    je .t2
    inc esi
.t2:
    ; Plural-Test: 1 -> ONE
    mov eax, 1
    call glob_get_plural_category
    cmp eax, PLURAL_ONE
    je .t3
    inc esi
.t3:
    ; Plural-Test: 5 -> OTHER
    mov eax, 5
    call glob_get_plural_category
    cmp eax, PLURAL_OTHER
    je .t4
    inc esi
.t4:
    ; RTL-Test: Arabisch
    mov eax, 0x41720000
    call glob_set_language
    cmp dword [glob_current_dir], GLOB_DIR_RTL
    je .t5
    inc esi
.t5:
    ; Zur‹cksetzen auf de-DE
    mov eax, LANG_DE_DE
    call glob_set_language

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
glob_mgr_ready:    dd 0
glob_locale_count: dd 0
glob_prio_count:   dd 0
glob_current_lang: dd LANG_DE_DE
glob_current_dir:  dd GLOB_DIR_LTR

align 4
glob_locales:
    times LOCALE_CAPACITY * LOCALE_SIZE db 0

align 4
glob_prio_list:
    times LANG_PRIO_CAPACITY dd 0
