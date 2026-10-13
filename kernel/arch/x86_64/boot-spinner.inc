; Boot-Spinner fuer die Kernel-Init-Phase.
; kernel_boot_spinner_tick wird aus dem IRQ0-Handler aufgerufen (jedes 16. Tick).
; Zeichnet einen 12-Punkt-Kreis-Spinner mit Farbverlauf auf den UEFI-Framebuffer.

; --- Zustand ---
kernel_spinner_active:  dd 0    ; 1 = aktiv, 0 = inaktiv
kernel_spinner_frame:   dd 0    ; aktueller Frame-Zaehler
kernel_spinner_bg_ok:   dd 0    ; 1 = Hintergrund-Snapshot gueltig

; --- Scratch-Variablen (nicht thread-safe, nur aus IRQ0 verwendet) ---
k_spin_bx:              dd 0
k_spin_by:              dd 0
k_spin_cx:              dd 0
k_spin_cy:              dd 0
k_spin_r:               dd 0
k_spin_dot:             dd 0
k_spin_f:               dd 0
k_spin_i:               dd 0
k_spin_px:              dd 0
k_spin_py:              dd 0
k_spin_slot:            dd 0

; 64x64 Pixel Hintergrund-Snapshot (16 KB)
kernel_spinner_bg:      times (64*64) dd 0

; Lookup-Tabellen fuer 12 Positionen (sin/cos x 1000, Startpunkt = oben)
k_spin_dx: dd 0, 500, 866, 1000, 866, 500, 0, -500, -866, -1000, -866, -500
k_spin_dy: dd -1000, -866, -500, 0, 500, 866, 1000, 866, 500, 0, -500, -866

; Farben: Kopf hell (weiss-blau), Schweif ausblenden (dunkelblau)
; Format: 0x00RRGGBB -> direkte Framebuffer-Schreibung (BGRA/BGR)
k_spin_colors:
    dd 0x00EAF7FF   ; i=0  Kopf: weisslich
    dd 0x0065D7FF   ; i=1  hell-cyan
    dd 0x0050B8E0   ; i=2  cyan-mittel
    dd 0x002D9CFF   ; i=3  blau hell
    dd 0x002280D0   ; i=4  blau mittel
    dd 0x001B63A0   ; i=5  blau
    dd 0x00124880   ; i=6  blau dunkel
    dd 0x000D3060   ; i=7  dunkelblau
    dd 0x00091F40   ; i=8  sehr dunkel
    dd 0x00051228   ; i=9  fast schwarz
    dd 0x00030A18   ; i=10 fast schwarz
    dd 0x00020510   ; i=11 fast schwarz

; kernel_boot_spinner_tick
; Rettet alle Register, zeichnet einen Frame, kehrt zum Aufrufer zurueck.
; Darf aus Interrupt-Kontext aufgerufen werden (kein Framebuffer-Subsystem noetig).
kernel_boot_spinner_tick:
    cmp dword [kernel_context + CONTEXT_BPP], 32
    jne .exit
    pushad

    ; cx = width / 2
    mov eax, [kernel_context + CONTEXT_WIDTH]
    shr eax, 1
    mov [k_spin_cx], eax

    ; cy = height * 700 / 945  (Spinner im unteren Splash-Bereich)
    mov eax, [kernel_context + CONTEXT_HEIGHT]
    imul eax, 700
    xor edx, edx
    mov ecx, 945
    div ecx
    mov [k_spin_cy], eax

    ; bx = cx - 32, by = cy - 32  (linke obere Ecke des 64x64 Snapshots)
    mov eax, [k_spin_cx]
    sub eax, 32
    jl .done
    mov [k_spin_bx], eax
    mov eax, [k_spin_cy]
    sub eax, 32
    jl .done
    mov [k_spin_by], eax

    cmp dword [kernel_spinner_bg_ok], 0
    je .do_snapshot

    ; --- Restore: Hintergrund-Snapshot zurueckschreiben ---
    mov esi, kernel_spinner_bg
    xor ebx, ebx
.rest_row:
    cmp ebx, 64
    jae .do_draw
    mov eax, [k_spin_by]
    add eax, ebx
    imul eax, [kernel_context + CONTEXT_PITCH]
    add eax, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov ecx, [k_spin_bx]
    shl ecx, 2
    add eax, ecx
    mov edi, eax
    mov ecx, 64
    rep movsd           ; esi += 256 automatisch
    inc ebx
    jmp .rest_row

    ; --- Snapshot: Hintergrund lesen ---
.do_snapshot:
    mov edi, kernel_spinner_bg
    xor ebx, ebx
.snap_row:
    cmp ebx, 64
    jae .snap_done
    mov eax, [k_spin_by]
    add eax, ebx
    imul eax, [kernel_context + CONTEXT_PITCH]
    add eax, [kernel_context + CONTEXT_FRAMEBUFFER]
    mov ecx, [k_spin_bx]
    shl ecx, 2
    add eax, ecx
    mov esi, eax
    mov ecx, 64
    rep movsd           ; edi += 256 automatisch
    inc ebx
    jmp .snap_row
.snap_done:
    mov dword [kernel_spinner_bg_ok], 1

.do_draw:
    ; r = width * 22 / 1680  (skaliert fuer beliebige Aufloesung)
    mov eax, [kernel_context + CONTEXT_WIDTH]
    imul eax, 22
    xor edx, edx
    mov ecx, 1680
    div ecx
    cmp eax, 4
    jge .r_ok
    mov eax, 4
.r_ok:
    mov [k_spin_r], eax
    ; dot = max(r / 4, 2)
    shr eax, 2
    cmp eax, 2
    jge .dot_ok
    mov eax, 2
.dot_ok:
    mov [k_spin_dot], eax

    ; f = frame % 12
    mov eax, [kernel_spinner_frame]
    xor edx, edx
    mov ecx, 12
    div ecx
    mov [k_spin_f], edx

    ; --- Dot-Loop i = 0..11 ---
    mov dword [k_spin_i], 0
.dot_loop:
    cmp dword [k_spin_i], 12
    jae .dot_done

    ; slot = (f + 12 - i) % 12
    mov eax, [k_spin_f]
    add eax, 12
    sub eax, [k_spin_i]
    xor edx, edx
    mov ecx, 12
    div ecx
    mov [k_spin_slot], edx

    ; px = cx + dx[slot] * r / 1000
    mov ecx, [k_spin_slot]
    mov eax, [k_spin_dx + ecx*4]
    imul eax, [k_spin_r]
    cdq
    mov ecx, 1000
    idiv ecx
    add eax, [k_spin_cx]
    mov [k_spin_px], eax

    ; py = cy + dy[slot] * r / 1000
    mov ecx, [k_spin_slot]
    mov eax, [k_spin_dy + ecx*4]
    imul eax, [k_spin_r]
    cdq
    mov ecx, 1000
    idiv ecx
    add eax, [k_spin_cy]
    mov [k_spin_py], eax

    ; Dot zeichnen: (px - dot/2, py - dot/2, dot, dot)
    mov eax, [k_spin_dot]
    shr eax, 1              ; half_dot
    mov ebx, [k_spin_px]
    sub ebx, eax
    jl .dot_skip
    mov ecx, [k_spin_py]
    sub ecx, eax
    jl .dot_skip

    mov esi, [k_spin_i]
    mov eax, [k_spin_colors + esi*4]
    mov edx, [k_spin_dot]
    mov esi, [k_spin_dot]
    call fill_rectangle

    jmp .dot_next
.dot_skip:
.dot_next:
    inc dword [k_spin_i]
    jmp .dot_loop

.dot_done:
    inc dword [kernel_spinner_frame]
.done:
    popad
.exit:
    ret
