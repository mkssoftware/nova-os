; ---------------------------------------------------------------------------
; Nova UI-Subsystem §001
; NPSPEC-UI-ACCESSIBILITYTREE-0001  - Accessibility-Tree
; NPSPEC-UI-ADAPTIVE-0001           - Adaptives UI
; NPSPEC-UI-CAPABILITYDRIVEN-0001   - Capability-gesteuertes UI
; NPSPEC-UI-COLOR-0001              - Farbverwaltung
; NPSPEC-UI-COMPONENT-0001          - UI-Komponenten
; NPSPEC-UI-COMPOSITOR-0001         - Compositor
; NPSPEC-UI-DAMAGETRACKING-0001     - Damage-Tracking
; NPSPEC-UI-DASHBOARD-0001          - Dashboard
; NPSPEC-UI-DECLARATIVE-0001        - Deklaratives UI
; NPSPEC-UI-DESKTOP-0001            - Desktop-Shell
; NPSPEC-UI-DISPLAYSERVER-0001      - Display-Server
; NPSPEC-UI-FENSTER-0001            - Fensterverwaltung
; NPSPEC-UI-FRAMESCHEDULING-0001    - Frame-Scheduling
; NPSPEC-UI-GPUCOMPOSITION-0001     - GPU-Komposition
; NPSPEC-UI-INPUTROUTING-0001       - Input-Routing
; NPSPEC-UI-RESPONSIVE-0001         - Responsives UI
; NPSPEC-UI-RETAINEDMODE-0001       - Retained-Mode
; NPSPEC-UI-RIBBON-0001             - Ribbon-Interface
; NPSPEC-UI-RIBBON-CAPABILITY-0001  - Ribbon-Capabilities
; NPSPEC-UI-SCENEGRAPH-0001         - Szenegraph
; NPSPEC-UI-SEMANTIC-0001           - Semantisches UI
; NPSPEC-UI-STARTMENUE-0001         - Startmenue
; NPSPEC-UI-TASKLEISTE-0001         - Taskleiste
; NPSPEC-UI-THEME-0001              - Themes
; NPSPEC-UI-VARIABLEREFRESH-0001    - Variable Bildwiederholung
; NPSPEC-UI-WINDOWSERVER-0001       - Window-Server
; ---------------------------------------------------------------------------

; Fenster-Flags (NPSPEC-UI-FENSTER-0001)
UI_WIN_VISIBLE        equ 0x0001
UI_WIN_FOCUSED        equ 0x0002
UI_WIN_MAXIMIZED      equ 0x0004
UI_WIN_MINIMIZED      equ 0x0008
UI_WIN_FULLSCREEN     equ 0x0010
UI_WIN_COMPOSITED     equ 0x0020  ; (NPSPEC-UI-COMPOSITOR-0001)
UI_WIN_GPU            equ 0x0040  ; (NPSPEC-UI-GPUCOMPOSITION-0001)
UI_WIN_DAMAGED        equ 0x0080  ; (NPSPEC-UI-DAMAGETRACKING-0001)
UI_WIN_RETAINED       equ 0x0100  ; (NPSPEC-UI-RETAINEDMODE-0001)
UI_WIN_ACCESSIBLE     equ 0x0200  ; (NPSPEC-UI-ACCESSIBILITYTREE-0001)
UI_WIN_SEMANTIC       equ 0x0400  ; (NPSPEC-UI-SEMANTIC-0001)
UI_WIN_CAPABILITY     equ 0x0800  ; (NPSPEC-UI-CAPABILITYDRIVEN-0001)

; Fenster-Record (64 Bytes, NPSPEC-UI-FENSTER-0001 / WINDOWSERVER-0001)
UI_WIN_SIZE           equ 64
UW_ID                 equ 0
UW_FLAGS              equ 4
UW_X                  equ 8     ; Position X
UW_Y                  equ 12    ; Position Y
UW_W                  equ 16    ; Breite
UW_H                  equ 20    ; Hoehe
UW_Z_ORDER            equ 24    ; Z-Reihenfolge
UW_DAMAGE_X           equ 28    ; Damage-Region X (NPSPEC-UI-DAMAGETRACKING-0001)
UW_DAMAGE_Y           equ 32
UW_DAMAGE_W           equ 36
UW_DAMAGE_H           equ 40
UW_FRAMEBUFFER        equ 44    ; Framebuffer-Zeiger (NPSPEC-UI-COMPOSITOR-0001)
UW_INPUT_HANDLER      equ 48    ; Input-Handler (NPSPEC-UI-INPUTROUTING-0001)
UW_THEME_ID           equ 52    ; Theme-ID (NPSPEC-UI-THEME-0001)
UW_CAPABILITY_MASK    equ 56    ; (NPSPEC-UI-CAPABILITYDRIVEN-0001)
; 60..63: reserviert

UI_WIN_CAPACITY       equ 8

; Input-Event-Typen (NPSPEC-UI-INPUTROUTING-0001)
UI_INPUT_KEY_DOWN     equ 0
UI_INPUT_KEY_UP       equ 1
UI_INPUT_MOUSE_MOVE   equ 2
UI_INPUT_MOUSE_BTN    equ 3
UI_INPUT_TOUCH        equ 4

; Input-Event-Record (20 Bytes)
UI_EVT_SIZE           equ 20
UE_TYPE               equ 0
UE_KEY_CODE           equ 4
UE_X                  equ 8
UE_Y                  equ 12
UE_MODIFIERS          equ 16

UI_EVT_CAPACITY       equ 16

; Frame-Rate-Konstanten (NPSPEC-UI-VARIABLEREFRESH-0001 / FRAMESCHEDULING-0001)
UI_FPS_30             equ 30
UI_FPS_60             equ 60
UI_FPS_120            equ 120
UI_FPS_VARIABLE       equ 0xFF

; Theme-IDs (NPSPEC-UI-THEME-0001)
UI_THEME_LIGHT        equ 0
UI_THEME_DARK         equ 1
UI_THEME_HIGH_CONTRAST equ 2

; Compositor-Modi (NPSPEC-UI-COMPOSITOR-0001)
UI_COMP_CPU           equ 0
UI_COMP_GPU           equ 1    ; (NPSPEC-UI-GPUCOMPOSITION-0001)

; Scene-Graph-Node (NPSPEC-UI-SCENEGRAPH-0001)
UI_SGN_SIZE           equ 32
SG_ID                 equ 0
SG_PARENT_ID          equ 4
SG_TYPE               equ 8    ; 0=Group 1=Rect 2=Text 3=Image
SG_FLAGS              equ 12
SG_X                  equ 16
SG_Y                  equ 20
SG_W                  equ 24
SG_H                  equ 28

UI_SGN_CAPACITY       equ 16

; ---------------------------------------------------------------------------
; ui_initialize (NPSPEC-UI-WINDOWSERVER-0001 / DISPLAYSERVER-0001)
; ---------------------------------------------------------------------------
ui_initialize:
    push edi
    mov edi, ui_win_table
    xor eax, eax
    mov ecx, (UI_WIN_CAPACITY * UI_WIN_SIZE) / 4
    rep stosd
    mov edi, ui_evt_queue
    mov ecx, (UI_EVT_CAPACITY * UI_EVT_SIZE) / 4
    rep stosd
    mov edi, ui_sgn_table
    mov ecx, (UI_SGN_CAPACITY * UI_SGN_SIZE) / 4
    rep stosd
    pop edi
    mov dword [ui_win_count], 0
    mov dword [ui_evt_head], 0
    mov dword [ui_evt_tail], 0
    mov dword [ui_sgn_count], 0
    mov dword [ui_next_win_id], 1
    mov dword [ui_next_sgn_id], 1
    mov dword [ui_target_fps], UI_FPS_60
    mov dword [ui_compositor_mode], UI_COMP_CPU
    mov dword [ui_active_theme], UI_THEME_DARK
    mov dword [ui_focused_win], 0
    mov dword [ui_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; ui_window_create: EAX=x, EBX=y, ECX=w, EDX=h -> ESI=win_id
; (NPSPEC-UI-FENSTER-0001, NPSPEC-UI-WINDOWSERVER-0001)
; CF=0 Erfolg
; ---------------------------------------------------------------------------
ui_window_create:
    cmp dword [ui_win_count], UI_WIN_CAPACITY
    jae .full
    push edi
    push esi
    mov edi, ui_win_table
    push eax
    mov eax, [ui_win_count]
    imul eax, UI_WIN_SIZE
    add edi, eax
    pop eax
    mov esi, [ui_next_win_id]
    mov [edi + UW_ID], esi
    mov dword [edi + UW_FLAGS], UI_WIN_VISIBLE | UI_WIN_RETAINED
    mov [edi + UW_X], eax
    mov [edi + UW_Y], ebx
    mov [edi + UW_W], ecx
    mov [edi + UW_H], edx
    ; Z-Order = count (neue Fenster oben)
    push eax
    mov eax, [ui_win_count]
    mov [edi + UW_Z_ORDER], eax
    pop eax
    mov dword [edi + UW_DAMAGE_X], 0
    mov dword [edi + UW_DAMAGE_Y], 0
    mov [edi + UW_DAMAGE_W], ecx
    mov [edi + UW_DAMAGE_H], edx
    mov dword [edi + UW_FRAMEBUFFER], 0
    mov dword [edi + UW_INPUT_HANDLER], 0
    push eax
    mov eax, [ui_active_theme]
    mov [edi + UW_THEME_ID], eax
    pop eax
    mov dword [edi + UW_CAPABILITY_MASK], 0
    inc dword [ui_next_win_id]
    inc dword [ui_win_count]
    pop esi
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; ui_window_destroy: EAX=win_id
; (NPSPEC-UI-FENSTER-0001)
; ---------------------------------------------------------------------------
ui_window_destroy:
    push esi
    push ecx
    mov esi, ui_win_table
    mov ecx, [ui_win_count]
.scan:
    test ecx, ecx
    jz .not_found
    cmp [esi + UW_ID], eax
    je .found
    add esi, UI_WIN_SIZE
    dec ecx
    jmp .scan
.found:
    ; Fenster als unsichtbar markieren (keine Kompaktierung)
    and dword [esi + UW_FLAGS], ~UI_WIN_VISIBLE
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
; ui_compositor_frame: Setzt alle beschaedigten Fenster als neu (NPSPEC-UI-COMPOSITOR-0001)
; (NPSPEC-UI-DAMAGETRACKING-0001, NPSPEC-UI-FRAMESCHEDULING-0001)
; ---------------------------------------------------------------------------
ui_compositor_frame:
    push esi
    push ecx
    mov esi, ui_win_table
    mov ecx, [ui_win_count]
.loop:
    test ecx, ecx
    jz .done
    test dword [esi + UW_FLAGS], UI_WIN_VISIBLE
    jz .next
    ; Damage-Region zuruecksetzen
    and dword [esi + UW_FLAGS], ~UI_WIN_DAMAGED
.next:
    add esi, UI_WIN_SIZE
    dec ecx
    jmp .loop
.done:
    inc dword [ui_frame_count]
    pop ecx
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; ui_input_dispatch: EAX=type, EBX=key_code, ECX=x, EDX=y
; (NPSPEC-UI-INPUTROUTING-0001)
; ---------------------------------------------------------------------------
ui_input_dispatch:
    ; Event in Queue schreiben
    push edi
    mov edi, ui_evt_queue
    push eax
    mov eax, [ui_evt_head]
    imul eax, UI_EVT_SIZE
    add edi, eax
    pop eax
    mov [edi + UE_TYPE], eax
    mov [edi + UE_KEY_CODE], ebx
    mov [edi + UE_X], ecx
    mov [edi + UE_Y], edx
    mov dword [edi + UE_MODIFIERS], 0
    ; Head vorruecken (Ring)
    inc dword [ui_evt_head]
    push eax
    mov eax, [ui_evt_head]
    cmp eax, UI_EVT_CAPACITY
    jb .no_wrap
    mov dword [ui_evt_head], 0
.no_wrap:
    pop eax
    pop edi
    clc
    ret

; ---------------------------------------------------------------------------
; ui_set_theme: EAX=theme_id (NPSPEC-UI-THEME-0001 / ADAPTIVE-0001)
; ---------------------------------------------------------------------------
ui_set_theme:
    mov [ui_active_theme], eax
    ; Alle Fenster als beschaedigt markieren
    push esi
    push ecx
    mov esi, ui_win_table
    mov ecx, [ui_win_count]
.loop:
    test ecx, ecx
    jz .done
    or dword [esi + UW_FLAGS], UI_WIN_DAMAGED
    mov [esi + UW_THEME_ID], eax
    add esi, UI_WIN_SIZE
    dec ecx
    jmp .loop
.done:
    pop ecx
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; ui_scenegraph_add: EAX=parent_id, EBX=type, ECX=x<<16|y, EDX=w<<16|h
; (NPSPEC-UI-SCENEGRAPH-0001, NPSPEC-UI-RETAINEDMODE-0001)
; CF=0 -> ESI=node_id
; ---------------------------------------------------------------------------
ui_scenegraph_add:
    cmp dword [ui_sgn_count], UI_SGN_CAPACITY
    jae .full
    push edi
    push esi
    mov edi, ui_sgn_table
    push eax
    mov eax, [ui_sgn_count]
    imul eax, UI_SGN_SIZE
    add edi, eax
    pop eax
    mov esi, [ui_next_sgn_id]
    mov [edi + SG_ID], esi
    mov [edi + SG_PARENT_ID], eax
    mov [edi + SG_TYPE], ebx
    mov dword [edi + SG_FLAGS], 0
    ; ECX = x<<16|y
    mov eax, ecx
    shr eax, 16
    mov [edi + SG_X], eax
    and ecx, 0xFFFF
    mov [edi + SG_Y], ecx
    ; EDX = w<<16|h
    mov eax, edx
    shr eax, 16
    mov [edi + SG_W], eax
    and edx, 0xFFFF
    mov [edi + SG_H], edx
    inc dword [ui_next_sgn_id]
    inc dword [ui_sgn_count]
    pop esi
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; ui_self_test
; ---------------------------------------------------------------------------
ui_self_test:
    push esi
    xor esi, esi
    cmp dword [ui_ready], 1
    je .t2
    inc esi
.t2:
    ; Fenster erstellen (100,100, 800x600)
    mov eax, 100
    mov ebx, 100
    mov ecx, 800
    mov edx, 600
    call ui_window_create
    jnc .t3
    inc esi
.t3:
    ; Frame rendern
    call ui_compositor_frame
    jnc .t4
    inc esi
.t4:
    ; Input-Event
    mov eax, UI_INPUT_KEY_DOWN
    mov ebx, 0x41    ; 'A'
    xor ecx, ecx
    xor edx, edx
    call ui_input_dispatch
    jnc .t5
    inc esi
.t5:
    ; Fenster loeschen
    mov eax, 1
    call ui_window_destroy
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
ui_ready:            dd 0
ui_win_count:        dd 0
ui_evt_head:         dd 0
ui_evt_tail:         dd 0
ui_sgn_count:        dd 0
ui_next_win_id:      dd 1
ui_next_sgn_id:      dd 1
ui_target_fps:       dd UI_FPS_60
ui_compositor_mode:  dd UI_COMP_CPU
ui_active_theme:     dd UI_THEME_DARK
ui_focused_win:      dd 0
ui_frame_count:      dd 0

align 4
ui_win_table:
    times UI_WIN_CAPACITY * UI_WIN_SIZE db 0

align 4
ui_evt_queue:
    times UI_EVT_CAPACITY * UI_EVT_SIZE db 0

align 4
ui_sgn_table:
    times UI_SGN_CAPACITY * UI_SGN_SIZE db 0
