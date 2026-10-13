; ---------------------------------------------------------------------------
; Nova Text-Subsystem
; NPSPEC-TEXT-UNICODE-0001      - Unicode-Zeichensatz-Modell
; NPSPEC-TEXT-UTF8-0001         - UTF-8 Kodierung/Dekodierung
; NPSPEC-TEXT-ENCODING-0001     - Encoding-Verwaltung
; NPSPEC-TEXT-BIDI-0001         - Bidirektionaler Text (RTL/LTR)
; NPSPEC-TEXT-NORMALIZATION-0001- Unicode-Normalisierung (NFC/NFD)
; NPSPEC-TEXT-GRAPHEME-0001     - Graphem-Cluster
; NPSPEC-TEXT-SCRIPT-0001       - Schrift-Erkennung
; NPSPEC-TEXT-SEGMENTATION-0001 - Wort/Satz-Segmentierung
; NPSPEC-TEXT-COLLATION-0001    - Sortierreihenfolge
; NPSPEC-TEXT-SEARCH-0001       - Text-Suche
; NPSPEC-TEXT-SHAPING-0001      - Text-Shaping (Ligaturen/Kerning)
; NPSPEC-TEXT-SECURITY-0001     - Unicode-Sicherheit
; ---------------------------------------------------------------------------

; Encoding-IDs (NPSPEC-TEXT-ENCODING-0001)
TEXT_ENC_UNKNOWN      equ 0
TEXT_ENC_ASCII        equ 1
TEXT_ENC_UTF8         equ 2
TEXT_ENC_UTF16LE      equ 3
TEXT_ENC_UTF16BE      equ 4
TEXT_ENC_UTF32LE      equ 5
TEXT_ENC_ISO8859_1    equ 6
TEXT_ENC_CP1252       equ 7

; BiDi-Richtungen (NPSPEC-TEXT-BIDI-0001)
BIDI_LTR              equ 0    ; Links-nach-rechts
BIDI_RTL              equ 1    ; Rechts-nach-links
BIDI_AUTO             equ 2    ; Automatisch erkennen

; Normalisierungsformen (NPSPEC-TEXT-NORMALIZATION-0001)
NORM_NONE             equ 0
NORM_NFC              equ 1    ; Canonical Decomposition, Canonical Composition
NORM_NFD              equ 2    ; Canonical Decomposition
NORM_NFKC             equ 3    ; Compatibility Decomposition + Composition
NORM_NFKD             equ 4    ; Compatibility Decomposition

; Script-IDs (NPSPEC-TEXT-SCRIPT-0001)
SCRIPT_UNKNOWN        equ 0
SCRIPT_LATIN          equ 1
SCRIPT_ARABIC         equ 2
SCRIPT_HEBREW         equ 3
SCRIPT_CYRILLIC       equ 4
SCRIPT_CJK            equ 5
SCRIPT_DEVANAGARI     equ 6
SCRIPT_GREEK          equ 7
SCRIPT_HANGUL         equ 8

; Segmentierungstypen (NPSPEC-TEXT-SEGMENTATION-0001)
SEG_CHAR              equ 0    ; Zeichen-Grenze
SEG_WORD              equ 1    ; Wort-Grenze
SEG_SENTENCE          equ 2    ; Satz-Grenze
SEG_LINE              equ 3    ; Zeilen-Umbruch

; Shaping-Flags (NPSPEC-TEXT-SHAPING-0001)
SHAPE_LIGATURES       equ 0x01
SHAPE_KERNING         equ 0x02
SHAPE_DIACRITICS      equ 0x04
SHAPE_GLYPH_SUBST     equ 0x08

; Sicherheits-Flags (NPSPEC-TEXT-SECURITY-0001)
TEXTSEC_NO_BIDI_OVERRIDE  equ 0x01  ; Bidi-Override-Zeichen ablehnen
TEXTSEC_NO_HOMOGRAPH      equ 0x02  ; Homograph-Angriffe blockieren
TEXTSEC_NO_INVISIBLE      equ 0x04  ; Unsichtbare Zeichen ablehnen
TEXTSEC_NFC_NORMALIZE     equ 0x08  ; Normalisierung erzwingen

; Encoding-Deskriptor (16 Bytes)
TEXT_ENC_DESC_SIZE    equ 16
TE_ID                 equ 0
TE_BIDI_DEFAULT       equ 4   ; Standard-Richtung
TE_FLAGS              equ 8

TEXT_ENC_CAPACITY     equ 8

; ---------------------------------------------------------------------------
; text_initialize
; Initialisiert Encoding-Registry und Sicherheitseinstellungen.
; CF=0 Erfolg
; ---------------------------------------------------------------------------
text_initialize:
    push edi
    ; Encoding-Tabelle loeschen
    mov edi, text_enc_table
    xor eax, eax
    mov ecx, (TEXT_ENC_CAPACITY * TEXT_ENC_DESC_SIZE) / 4
    rep stosd
    pop edi

    ; Standard-Encodings registrieren
    mov eax, TEXT_ENC_ASCII
    mov ebx, BIDI_LTR
    call text_register_encoding
    jc .fail

    mov eax, TEXT_ENC_UTF8
    mov ebx, BIDI_AUTO
    call text_register_encoding
    jc .fail

    mov eax, TEXT_ENC_UTF16LE
    mov ebx, BIDI_AUTO
    call text_register_encoding
    jc .fail

    mov dword [text_enc_count], 3
    mov dword [text_security_flags], TEXTSEC_NO_BIDI_OVERRIDE | TEXTSEC_NFC_NORMALIZE
    mov dword [text_subsystem_ready], 1
    clc
    ret
.fail:
    stc
    ret

; ---------------------------------------------------------------------------
; text_register_encoding: EAX=enc_id, EBX=bidi_default
; ---------------------------------------------------------------------------
text_register_encoding:
    cmp dword [text_enc_count], TEXT_ENC_CAPACITY
    jae .full
    push edi
    push ecx
    mov edi, text_enc_table
    mov ecx, [text_enc_count]
    imul ecx, TEXT_ENC_DESC_SIZE
    add edi, ecx
    mov [edi + TE_ID], eax
    mov [edi + TE_BIDI_DEFAULT], ebx
    mov dword [edi + TE_FLAGS], 0
    inc dword [text_enc_count]
    pop ecx
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; text_utf8_decode: ESI=src_ptr, ECX=src_len -> EAX=codepoint, ESI=next_byte
; Dekodiert ein UTF-8-Zeichen (NPSPEC-TEXT-UTF8-0001).
; CF=0 ok, CF=1 ungueltig/Ende
; ---------------------------------------------------------------------------
text_utf8_decode:
    test ecx, ecx
    jz .end_of_input
    movzx eax, byte [esi]
    inc esi
    dec ecx
    ; ASCII: 0x00..0x7F
    cmp al, 0x80
    jb .ascii
    ; 2-Byte: 0xC0..0xDF
    cmp al, 0xC0
    jb .invalid
    cmp al, 0xE0
    jb .two_byte
    ; 3-Byte: 0xE0..0xEF
    cmp al, 0xF0
    jb .three_byte
    ; 4-Byte: 0xF0..0xF7
    cmp al, 0xF8
    jb .four_byte
.invalid:
    stc
    ret
.ascii:
    clc
    ret
.two_byte:
    and eax, 0x1F
    test ecx, ecx
    jz .invalid
    movzx edx, byte [esi]
    inc esi
    dec ecx
    and edx, 0x3F
    shl eax, 6
    or eax, edx
    clc
    ret
.three_byte:
    and eax, 0x0F
    test ecx, ecx
    jz .invalid
    movzx edx, byte [esi]
    inc esi
    dec ecx
    and edx, 0x3F
    shl eax, 6
    or eax, edx
    test ecx, ecx
    jz .invalid
    movzx edx, byte [esi]
    inc esi
    dec ecx
    and edx, 0x3F
    shl eax, 6
    or eax, edx
    clc
    ret
.four_byte:
    and eax, 0x07
    test ecx, ecx
    jz .invalid
    movzx edx, byte [esi]
    inc esi
    dec ecx
    and edx, 0x3F
    shl eax, 6
    or eax, edx
    test ecx, ecx
    jz .invalid
    movzx edx, byte [esi]
    inc esi
    dec ecx
    and edx, 0x3F
    shl eax, 6
    or eax, edx
    test ecx, ecx
    jz .invalid
    movzx edx, byte [esi]
    inc esi
    dec ecx
    and edx, 0x3F
    shl eax, 6
    or eax, edx
    clc
    ret
.end_of_input:
    stc
    ret

; ---------------------------------------------------------------------------
; text_utf8_encode: EAX=codepoint, EDI=dst_buf, ECX=buf_remaining
; -> EDI=next_byte, ECX=remaining; CF=0 ok
; (NPSPEC-TEXT-UTF8-0001)
; ---------------------------------------------------------------------------
text_utf8_encode:
    cmp eax, 0x7F
    jbe .ascii
    cmp eax, 0x7FF
    jbe .two
    cmp eax, 0xFFFF
    jbe .three
    ; 4-Byte
    cmp ecx, 4
    jb .no_space
    push eax
    and eax, 0x3F
    or al, 0x80
    mov [edi+3], al
    pop eax
    push eax
    shr eax, 6
    and eax, 0x3F
    or al, 0x80
    mov [edi+2], al
    pop eax
    push eax
    shr eax, 12
    and eax, 0x3F
    or al, 0x80
    mov [edi+1], al
    pop eax
    shr eax, 18
    or al, 0xF0
    mov [edi], al
    add edi, 4
    sub ecx, 4
    clc
    ret
.ascii:
    test ecx, ecx
    jz .no_space
    mov [edi], al
    inc edi
    dec ecx
    clc
    ret
.two:
    cmp ecx, 2
    jb .no_space
    push eax
    and eax, 0x3F
    or al, 0x80
    mov [edi+1], al
    pop eax
    shr eax, 6
    or al, 0xC0
    mov [edi], al
    add edi, 2
    sub ecx, 2
    clc
    ret
.three:
    cmp ecx, 3
    jb .no_space
    push eax
    and eax, 0x3F
    or al, 0x80
    mov [edi+2], al
    pop eax
    push eax
    shr eax, 6
    and eax, 0x3F
    or al, 0x80
    mov [edi+1], al
    pop eax
    shr eax, 12
    or al, 0xE0
    mov [edi], al
    add edi, 3
    sub ecx, 3
    clc
    ret
.no_space:
    stc
    ret

; ---------------------------------------------------------------------------
; text_detect_script: EAX=codepoint -> EBX=script_id
; (NPSPEC-TEXT-SCRIPT-0001)
; ---------------------------------------------------------------------------
text_detect_script:
    ; Latein: U+0041..U+007A, U+00C0..U+024F
    cmp eax, 0x0041
    jb .check_arabic
    cmp eax, 0x024F
    ja .check_cyrillic
    mov ebx, SCRIPT_LATIN
    ret
.check_arabic:
    cmp eax, 0x0600
    jb .check_hebrew
    cmp eax, 0x06FF
    ja .check_hebrew
    mov ebx, SCRIPT_ARABIC
    ret
.check_hebrew:
    cmp eax, 0x0590
    jb .check_cjk
    cmp eax, 0x05FF
    ja .check_cjk
    mov ebx, SCRIPT_HEBREW
    ret
.check_cjk:
    cmp eax, 0x4E00
    jb .check_devanagari
    cmp eax, 0x9FFF
    ja .check_devanagari
    mov ebx, SCRIPT_CJK
    ret
.check_devanagari:
    cmp eax, 0x0900
    jb .check_cyrillic
    cmp eax, 0x097F
    ja .check_cyrillic
    mov ebx, SCRIPT_DEVANAGARI
    ret
.check_cyrillic:
    cmp eax, 0x0400
    jb .check_greek
    cmp eax, 0x04FF
    ja .check_greek
    mov ebx, SCRIPT_CYRILLIC
    ret
.check_greek:
    cmp eax, 0x0370
    jb .unknown
    cmp eax, 0x03FF
    ja .unknown
    mov ebx, SCRIPT_GREEK
    ret
.unknown:
    mov ebx, SCRIPT_UNKNOWN
    ret

; ---------------------------------------------------------------------------
; text_bidi_classify: ESI=utf8_str, ECX=len -> EAX=bidi_direction
; (NPSPEC-TEXT-BIDI-0001): Zaehlt RTL-Zeichen; Mehrheit entscheidet.
; ---------------------------------------------------------------------------
text_bidi_classify:
    push esi
    push ecx
    push ebx
    xor edx, edx   ; LTR-Zaehler
    xor edi, edi   ; RTL-Zaehler
.loop:
    test ecx, ecx
    jz .done
    call text_utf8_decode
    jc .done
    call text_detect_script
    cmp ebx, SCRIPT_ARABIC
    je .rtl_inc
    cmp ebx, SCRIPT_HEBREW
    je .rtl_inc
    inc edx
    jmp .loop
.rtl_inc:
    inc edi
    jmp .loop
.done:
    pop ebx
    pop ecx
    pop esi
    cmp edi, edx
    jg .rtl
    mov eax, BIDI_LTR
    ret
.rtl:
    mov eax, BIDI_RTL
    ret

; ---------------------------------------------------------------------------
; text_security_check: ESI=utf8_str, ECX=len -> CF=0 ok, CF=1 verdaechtig
; (NPSPEC-TEXT-SECURITY-0001)
; ---------------------------------------------------------------------------
text_security_check:
    push esi
    push ecx
    push ebx
.loop:
    test ecx, ecx
    jz .ok
    call text_utf8_decode
    jc .ok
    ; Bidi-Override-Zeichen: U+202A..U+202E, U+2066..U+2069
    mov ebx, [text_security_flags]
    test ebx, TEXTSEC_NO_BIDI_OVERRIDE
    jz .check_invisible
    cmp eax, 0x202A
    jb .check_invisible
    cmp eax, 0x202E
    jbe .suspicious
    cmp eax, 0x2066
    jb .check_invisible
    cmp eax, 0x2069
    jbe .suspicious
.check_invisible:
    test ebx, TEXTSEC_NO_INVISIBLE
    jz .loop
    ; Unsichtbare Zeichen: U+FEFF (BOM), U+200B..U+200F (Zero-Width)
    cmp eax, 0x200B
    jb .loop
    cmp eax, 0x200F
    jbe .suspicious
    cmp eax, 0xFEFF
    je .suspicious
    jmp .loop
.ok:
    pop ebx
    pop ecx
    pop esi
    clc
    ret
.suspicious:
    pop ebx
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; text_self_test
; ---------------------------------------------------------------------------
text_self_test:
    push esi
    push edi
    push ecx
    xor ebp, ebp  ; Fehlerzaehler

    cmp dword [text_subsystem_ready], 1
    je .t2
    inc ebp
.t2:
    ; UTF-8 Kodierung/Dekodierung: 'A' = 0x41
    mov eax, 0x41
    mov edi, text_test_buf
    mov ecx, 16
    call text_utf8_encode
    jnc .t3
    inc ebp
.t3:
    mov esi, text_test_buf
    mov ecx, 1
    call text_utf8_decode
    jnc .t4
    inc ebp
.t4:
    cmp eax, 0x41
    je .t5
    inc ebp
.t5:
    ; Skript-Erkennung: 'A'=0x41 -> LATIN
    mov eax, 0x41
    call text_detect_script
    cmp ebx, SCRIPT_LATIN
    je .t6
    inc ebp
.t6:
    ; Arabisch: U+0630 -> ARABIC
    mov eax, 0x0630
    call text_detect_script
    cmp ebx, SCRIPT_ARABIC
    je .done
    inc ebp
.done:
    pop ecx
    pop edi
    pop esi
    test ebp, ebp
    jz .ok
    stc
    ret
.ok:
    clc
    ret

; ---------------------------------------------------------------------------
; Statische Daten
; ---------------------------------------------------------------------------
align 4
text_subsystem_ready: dd 0
text_enc_count:       dd 0
text_security_flags:  dd 0

align 4
text_enc_table:
    times TEXT_ENC_CAPACITY * TEXT_ENC_DESC_SIZE db 0

align 4
text_test_buf:
    times 32 db 0
