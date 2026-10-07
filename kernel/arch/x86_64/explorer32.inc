; ---------------------------------------------------------------------------
; Explorer-Ansicht aus Ring 3 (Display.SubmitExplorerView, Display ABI 1.1)
;
; Der System-UI-Prozess liest ein Verzeichnis ueber die VFS-Syscalls und
; uebergibt dem Display Server eine zeigerfreie, deklarative Ansicht mit
; hoechstens acht Eintraegen. Der Kernel prueft Groessen und Zaehler, ersetzt
; nicht darstellbare Bytes und zeichnet die Ansicht in das Explorer-Fenster.
; Die Ansicht gewaehrt keinen Dateizugriff; sie ist reine Darstellung.
;
; NovaExplorerViewV1 (480 Byte):
;   +0 StructSize, +4 Version, +8 Generation (u64, > 0), +16 EntryCount (0..8),
;   +20 TotalEntries, +24 Flags (Bit 0..2: markierter Schnellzugriff 1..7,
;   0 = keiner), +28 PathLength (0..56), +32 TotalDirectories, +36 TotalFiles
;   (TotalDirectories + TotalFiles = TotalEntries),
;   +40 Path[56] (Breadcrumb), +96 Entries[8]
; NovaExplorerEntryV1 (48 Byte):
;   +0 ObjectType (1 Datei, 2 Verzeichnis), +4 Size (Bytes, gesaettigt),
;   +8 NameLength (1..32), +12 Reserved, +16 Name[32]
; ---------------------------------------------------------------------------

SYSCALL_DISPLAY_SUBMIT_EXPLORER_VIEW equ 4
EXPLORER_VIEW_SIZE        equ 480
EXPLORER_VIEW_HEADER      equ 96
EXPLORER_ENTRY_SIZE       equ 48
EXPLORER_MAX_ENTRIES      equ 8
EXPLORER_NAME_MAX         equ 32
EXPLORER_PATH_MAX         equ 56

; EDX=Syscall-Frame (Capability DISPLAY_SYSTEM_UI bereits geprueft).
; EAX=Status.
explorer_submit_view:
    cmp dword [edx + 20], EXPLORER_VIEW_SIZE
    jb .size
    mov esi, [edx + 36]
    mov edi, explorer_view_incoming
    mov ecx, EXPLORER_VIEW_SIZE
    call syscall_copy_from_user
    jc .pointer
    cmp dword [explorer_view_incoming + 0], EXPLORER_VIEW_SIZE
    jne .size
    cmp dword [explorer_view_incoming + 4], SYSCALL_ABI_VERSION
    jne .abi
    mov eax, [explorer_view_incoming + 8]
    or eax, [explorer_view_incoming + 12]
    jz .validation
    test dword [explorer_view_incoming + 24], ~7
    jnz .reserved
    mov eax, [explorer_view_incoming + 32]
    add eax, [explorer_view_incoming + 36]
    jc .validation
    cmp eax, [explorer_view_incoming + 20]
    jne .validation
    mov ecx, [explorer_view_incoming + 16]
    cmp ecx, EXPLORER_MAX_ENTRIES
    ja .validation
    cmp [explorer_view_incoming + 20], ecx
    jb .validation
    cmp dword [explorer_view_incoming + 28], EXPLORER_PATH_MAX
    ja .validation
    ; Eintraege pruefen und zaehlen
    xor ebx, ebx
    mov dword [explorer_view_dirs], 0
    mov dword [explorer_view_files], 0
.entry:
    cmp ebx, ecx
    jae .entries_ok
    imul esi, ebx, EXPLORER_ENTRY_SIZE
    add esi, explorer_view_incoming + EXPLORER_VIEW_HEADER
    cmp dword [esi + 12], 0
    jne .reserved
    mov eax, [esi + 8]
    test eax, eax
    jz .validation
    cmp eax, EXPLORER_NAME_MAX
    ja .validation
    cmp dword [esi + 0], NOVAFS_TYPE_FILE
    je .file
    cmp dword [esi + 0], NOVAFS_TYPE_DIRECTORY
    jne .validation
    inc dword [explorer_view_dirs]
    jmp .next
.file:
    inc dword [explorer_view_files]
.next:
    inc ebx
    jmp .entry
.entries_ok:
    mov eax, [explorer_view_dirs]
    cmp eax, [explorer_view_incoming + 32]
    ja .validation
    mov eax, [explorer_view_files]
    cmp eax, [explorer_view_incoming + 36]
    ja .validation
    ; uebernehmen und Texte auf darstellbare Zeichen begrenzen
    mov esi, explorer_view_incoming
    mov edi, explorer_view
    mov ecx, EXPLORER_VIEW_SIZE / 4
    rep movsd
    mov esi, explorer_view + 40
    mov ecx, [explorer_view + 28]
    call explorer_sanitize
    xor ebx, ebx
.sanitize:
    cmp ebx, [explorer_view + 16]
    jae .accepted
    imul esi, ebx, EXPLORER_ENTRY_SIZE
    add esi, explorer_view + EXPLORER_VIEW_HEADER
    mov ecx, [esi + 8]
    add esi, 16
    call explorer_sanitize
    inc ebx
    jmp .sanitize
.accepted:
    mov dword [explorer_view_valid], 1
    mov esi, message_explorer_view_ok
    call serial_write_string
    mov eax, [explorer_view + 20]
    call serial_write_hex32
    mov esi, message_line_end
    call serial_write_string
    xor eax, eax
    ret
.size:
    mov eax, SYSCALL_STATUS_SIZE
    ret
.pointer:
    mov eax, SYSCALL_STATUS_POINTER
    ret
.abi:
    mov eax, SYSCALL_STATUS_ABI
    ret
.reserved:
    mov eax, SYSCALL_STATUS_RESERVED
    ret
.validation:
    mov eax, SYSCALL_STATUS_VALIDATION
    ret

; ESI=Text, ECX=Laenge. Steuerzeichen und Nicht-ASCII-Bytes werden zu '-';
; der Rest des Feldes ist nach dem Kopieren beliebig und wird nicht gezeichnet.
explorer_sanitize:
    test ecx, ecx
    jz .done
.byte:
    mov al, [esi]
    cmp al, 0x20
    jb .replace
    cmp al, 0x7E
    jbe .keep
.replace:
    mov byte [esi], '-'
.keep:
    inc esi
    loop .byte
.done:
    ret

; ESI=Quelle, ECX=Laenge, EDI=Ziel. Haengt Bytes an, EDI zeigt danach dahinter.
explorer_append:
    rep movsb
    ret

; ESI=nullterminierter Text, EDI=Ziel.
explorer_append_z:
.byte:
    lodsb
    test al, al
    jz .done
    stosb
    jmp .byte
.done:
    ret

; EAX=Wert, EDI=Ziel (Dezimal ohne fuehrende Nullen).
explorer_append_decimal:
    push ebx
    push ecx
    push edx
    mov ebx, 10
    xor ecx, ecx
.divide:
    xor edx, edx
    div ebx
    push edx
    inc ecx
    test eax, eax
    jnz .divide
.emit:
    pop eax
    add al, '0'
    stosb
    loop .emit
    pop edx
    pop ecx
    pop ebx
    ret

; EAX=Bytes, EDI=Ziel. "N B", "N KB" oder "N MB" (aufgerundet).
explorer_append_size:
    cmp eax, 1024
    jb .bytes
    cmp eax, 1024 * 1024
    jb .kib
    add eax, 1024 * 1024 - 1
    jc .mib_max
    shr eax, 20
    call explorer_append_decimal
    mov esi, text_explorer_unit_mb
    jmp explorer_append_z
.mib_max:
    mov eax, 4096
    call explorer_append_decimal
    mov esi, text_explorer_unit_mb
    jmp explorer_append_z
.kib:
    add eax, 1023
    shr eax, 10
    call explorer_append_decimal
    mov esi, text_explorer_unit_kb
    jmp explorer_append_z
.bytes:
    call explorer_append_decimal
    mov esi, text_explorer_unit_b
    jmp explorer_append_z

; ESI=Eintrag. ESI=Typtext nach Dateiendung.
explorer_type_text:
    mov ecx, [esi + 8]
    lea edi, [esi + 16]
    cmp ecx, 4
    jb .generic
    lea edi, [edi + ecx - 4]
    mov eax, [edi]
    or eax, 0x20202000              ; Kleinschreibung der Endung
    cmp eax, '.txt'
    je .text
    cmp eax, '.bin'
    je .binary
    cmp eax, '.png'
    je .image
    cmp ecx, 3
    jb .generic
    mov eax, [edi + 1]
    and eax, 0x00FFFFFF
    or eax, 0x00202000
    cmp eax, ('.' | ('m' << 8) | ('d' << 16))
    je .markdown
.generic:
    mov esi, text_explorer_type_file
    ret
.text:
    mov esi, text_explorer_type_text
    ret
.binary:
    mov esi, text_explorer_type_binary
    ret
.image:
    mov esi, text_explorer_type_image
    ret
.markdown:
    mov esi, text_explorer_type_markdown
    ret

; EBX=Index (n-tes Verzeichnis bzw. n-te Datei), EDX=Typ.
; ESI=Eintrag oder 0.
explorer_entry_of_type:
    push ecx
    push edi
    xor ecx, ecx
.scan:
    cmp ecx, [explorer_view + 16]
    jae .missing
    imul esi, ecx, EXPLORER_ENTRY_SIZE
    add esi, explorer_view + EXPLORER_VIEW_HEADER
    cmp [esi + 0], edx
    jne .next
    test ebx, ebx
    jz .found
    dec ebx
.next:
    inc ecx
    jmp .scan
.missing:
    xor esi, esi
.found:
    pop edi
    pop ecx
    ret

; Liefert in ESI den Breadcrumb-Text (Ansicht oder statischer Fallback).
explorer_breadcrumb_text:
    cmp dword [explorer_view_valid], 1
    jne .static
    mov esi, explorer_view + 40
    mov ecx, [explorer_view + 28]
    mov edi, explorer_line
    test ecx, ecx
    jz .root
    rep movsb
    jmp .terminate
.root:
    mov byte [edi], '/'
    inc edi
.terminate:
    mov byte [edi], 0
    mov esi, explorer_line
    ret
.static:
    mov esi, text_explorer_breadcrumb
    ret

; EDI=Index der Ordnerkarte. ESI=Beschriftung oder 0 (keine Karte).
explorer_folder_label:
    cmp dword [explorer_view_valid], 1
    jne .static
    push ebx
    push edx
    mov ebx, edi
    mov edx, NOVAFS_TYPE_DIRECTORY
    call explorer_entry_of_type
    pop edx
    pop ebx
    test esi, esi
    jz .done
    push edi
    mov ecx, [esi + 8]
    add esi, 16
    mov edi, [esp]
    shl edi, 5
    add edi, explorer_card_names
    push edi
    rep movsb
    mov byte [edi], 0
    pop esi
    pop edi
.done:
    ret
.static:
    mov esi, [shell_folder_labels + edi * 4]
    ret

; Auswahlbalken der fokussierten Dateizeile (Fokus 20..23), nur fuer
; vorhandene Zeilen.
explorer_draw_file_selection:
    pushad
    mov eax, [display_scene_focus]
    sub eax, 20
    cmp eax, 3
    jbe .focus_ready
    xor eax, eax
.focus_ready:
    cmp eax, [explorer_view_files]
    jae .done
    imul eax, eax, 24
    mov ecx, [shell_window_y]
    add ecx, 324
    add ecx, eax
    mov eax, NOVA_COLOR_SELECTION
    mov ebx, [shell_window_x]
    add ebx, 194
    mov edx, [shell_window_width]
    sub edx, 216
    mov esi, 22
    call fill_rounded_rectangle
.done:
    popad
    ret

; Liefert in ECX die Y-Position der Schnellzugriff-Markierung (relativ zum
; Fenster): statisch "Desktop", mit Ansicht der angegebene Eintrag.
explorer_sidebar_highlight_y:
    mov ecx, 184
    cmp dword [explorer_view_valid], 1
    jne .done
    mov eax, [explorer_view + 24]
    and eax, 7
    jz .done
    dec eax
    imul eax, eax, 24
    lea ecx, [eax + 160]
.done:
    ret

; Zeichnet Spaltenkopf, Dateizeilen und Fusszeile aus der Ansicht.
; Erwartet die Fenstergeometrie in shell_window_*.
explorer_draw_files:
    pushad
    ; Spaltenpositionen relativ zur Inhaltsbreite
    mov eax, [shell_window_width]
    sub eax, 230
    mov [explorer_content_width], eax
    mov ebx, eax
    imul ebx, 45
    mov ecx, 100
    xor edx, edx
    mov eax, ebx
    div ecx
    mov [explorer_column_date], eax
    mov eax, [explorer_content_width]
    imul eax, 65
    xor edx, edx
    div ecx
    mov [explorer_column_type], eax
    mov eax, [explorer_content_width]
    imul eax, 85
    xor edx, edx
    div ecx
    mov [explorer_column_size], eax

    mov dword [explorer_column_index], 0
.header:
    mov eax, [explorer_column_index]
    cmp eax, 4
    jae .rows
    mov esi, [explorer_column_titles + eax * 4]
    mov ebx, [shell_window_x]
    add ebx, 204
    add ebx, [explorer_column_offsets + eax * 4]
    mov ecx, [shell_window_y]
    add ecx, 292
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    inc dword [explorer_column_index]
    jmp .header

.rows:
    mov dword [explorer_row], 0
.row:
    mov ebx, [explorer_row]
    cmp ebx, 4
    jae .empty_check
    mov edx, NOVAFS_TYPE_FILE
    call explorer_entry_of_type
    test esi, esi
    jz .empty_check
    mov [explorer_entry], esi
    ; Name
    mov ecx, [esi + 8]
    add esi, 16
    mov edi, explorer_line
    rep movsb
    mov byte [edi], 0
    mov eax, [explorer_row]
    imul eax, eax, 24
    add eax, [shell_window_y]
    add eax, 330
    mov [explorer_row_y], eax
    mov esi, explorer_line
    mov ebx, [shell_window_x]
    add ebx, 204
    mov ecx, [explorer_row_y]
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    ; Aenderungsdatum: Phase 1 speichert keine Zeitstempel
    mov esi, text_explorer_no_time
    mov ebx, [shell_window_x]
    add ebx, 204
    add ebx, [explorer_column_date]
    mov ecx, [explorer_row_y]
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    ; Typ
    mov esi, [explorer_entry]
    call explorer_type_text
    mov ebx, [shell_window_x]
    add ebx, 204
    add ebx, [explorer_column_type]
    mov ecx, [explorer_row_y]
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    ; Groesse
    mov esi, [explorer_entry]
    mov eax, [esi + 4]
    mov edi, explorer_line
    call explorer_append_size
    mov byte [edi], 0
    mov esi, explorer_line
    mov ebx, [shell_window_x]
    add ebx, 204
    add ebx, [explorer_column_size]
    mov ecx, [explorer_row_y]
    mov edx, NOVA_COLOR_TEXT
    mov ebp, 1
    call draw_text
    inc dword [explorer_row]
    jmp .row
.empty_check:
    cmp dword [explorer_row], 0
    jne .footer
    mov esi, text_explorer_no_files
    mov ebx, [shell_window_x]
    add ebx, 204
    mov ecx, [shell_window_y]
    add ecx, 330
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text

.footer:
    mov edi, explorer_line
    mov eax, [explorer_view + 20]
    call explorer_append_decimal
    mov esi, text_explorer_footer_items
    cmp dword [explorer_view + 20], 1
    jne .items
    mov esi, text_explorer_footer_item
.items:
    call explorer_append_z
    mov eax, [explorer_view + 32]
    call explorer_append_decimal
    mov esi, text_explorer_footer_dirs
    call explorer_append_z
    mov eax, [explorer_view + 36]
    call explorer_append_decimal
    mov esi, text_explorer_footer_files
    cmp dword [explorer_view + 36], 1
    jne .files
    mov esi, text_explorer_footer_file
.files:
    call explorer_append_z
    cmp dword [nfs_mounted], 1
    jne .footer_done
    ; freier Platz des Systemvolumes in MiB (256 Bloecke je MiB)
    mov eax, [nfs_sb + NFS_SB_AVAILABLE]
    shr eax, 8
    call explorer_append_decimal
    mov esi, text_explorer_footer_free
    call explorer_append_z
.footer_done:
    mov byte [edi], 0
    mov esi, explorer_line
    mov ebx, [shell_window_x]
    add ebx, 202
    mov ecx, [shell_window_y]
    add ecx, [shell_window_height]
    sub ecx, 24
    mov edx, NOVA_COLOR_MUTED
    mov ebp, 1
    call draw_text
    popad
    ret

align 4
explorer_view_valid:    dd 0
explorer_view_dirs:     dd 0
explorer_view_files:    dd 0
explorer_content_width: dd 0
explorer_column_index:  dd 0
explorer_column_offsets:
                        dd 0
explorer_column_date:   dd 0
explorer_column_type:   dd 0
explorer_column_size:   dd 0
explorer_column_titles:
    dd text_explorer_col_name, text_explorer_col_date, text_explorer_col_type, text_explorer_col_size
explorer_row:           dd 0
explorer_row_y:         dd 0
explorer_entry:         dd 0
explorer_card_label:    dd 0
explorer_view:          times EXPLORER_VIEW_SIZE db 0
explorer_view_incoming: times EXPLORER_VIEW_SIZE db 0
explorer_line:          times 128 db 0
explorer_card_names:    times 4 * 32 + 1 db 0

message_explorer_view_ok:
    db "NOVA: Explorer zeigt NovaFS-Verzeichnis aus Ring 3, Eintraege 0x", 0
text_explorer_col_name:   db "NAME", 0
text_explorer_col_date:   db 0x8E, "NDERUNGSDATUM", 0
text_explorer_col_type:   db "TYP", 0
text_explorer_col_size:   db "GR", 0x99, "SSE", 0
text_explorer_no_time:    db "-", 0
text_explorer_no_files:   db "Keine Dateien in diesem Ordner", 0
text_explorer_type_file:  db "Datei", 0
text_explorer_type_text:  db "Textdatei", 0
text_explorer_type_binary: db "Bin", 0x84, "rdatei", 0
text_explorer_type_image: db "PNG-Bild", 0
text_explorer_type_markdown: db "Markdown", 0
text_explorer_unit_b:     db " B", 0
text_explorer_unit_kb:    db " KB", 0
text_explorer_unit_mb:    db " MB", 0
text_explorer_footer_items: db " Elemente    ", 0
text_explorer_footer_item:  db " Element    ", 0
text_explorer_footer_file:  db " Datei        NovaFS: ", 0
text_explorer_no_folders:   db "Keine Unterordner", 0
text_explorer_footer_dirs:  db " Ordner    ", 0
text_explorer_footer_files: db " Dateien        NovaFS: ", 0
text_explorer_footer_free:  db " MB frei", 0
