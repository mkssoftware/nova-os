/* NovaOS C-Kernel - UI-Subsystem (Port aus ui32.inc, NPSPEC-UI-*). Window-
   Server/Retained-Mode-Datenmodell (Fenster, Input-Queue, Szenegraph) - die
   eigentliche Pixel-Komposition folgt spaeter mit dem vollen Compositor aus
   boot/bootloader/bootmenu/. */
#ifndef NOVA_UI_H
#define NOVA_UI_H

#include <stdint.h>

#define UI_WIN_VISIBLE    0x0001u
#define UI_WIN_FOCUSED    0x0002u
#define UI_WIN_MAXIMIZED  0x0004u
#define UI_WIN_MINIMIZED  0x0008u
#define UI_WIN_FULLSCREEN 0x0010u
#define UI_WIN_COMPOSITED 0x0020u
#define UI_WIN_GPU        0x0040u
#define UI_WIN_DAMAGED    0x0080u
#define UI_WIN_RETAINED   0x0100u
#define UI_WIN_ACCESSIBLE 0x0200u
#define UI_WIN_SEMANTIC   0x0400u
#define UI_WIN_CAPABILITY 0x0800u

#define UI_WIN_CAPACITY 8u

#define UI_INPUT_KEY_DOWN   0u
#define UI_INPUT_KEY_UP     1u
#define UI_INPUT_MOUSE_MOVE 2u
#define UI_INPUT_MOUSE_BTN  3u
#define UI_INPUT_TOUCH      4u

#define UI_EVT_CAPACITY 16u

#define UI_FPS_30       30u
#define UI_FPS_60       60u
#define UI_FPS_120      120u
#define UI_FPS_VARIABLE 0xFFu

#define UI_THEME_LIGHT        0u
#define UI_THEME_DARK         1u
#define UI_THEME_HIGH_CONTRAST 2u

#define UI_COMP_CPU 0u
#define UI_COMP_GPU 1u

#define UI_SGN_CAPACITY 16u

void ui_initialize(void);
int ui_self_test(void); /* 0 = ok */

/* 0 = Erfolg, *out_win_id erhaelt die ID; -1 wenn Tabelle voll. */
int ui_window_create(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t *out_win_id);

/* 0 = Erfolg (markiert nur unsichtbar, keine Kompaktierung), -1 wenn
   win_id unbekannt. */
int ui_window_destroy(uint32_t win_id);

/* Setzt bei allen sichtbaren Fenstern das Damage-Flag zurueck. Immer 0. */
int ui_compositor_frame(void);

/* Schreibt ein Event in die Ring-Queue (ueberschreibt bei vollem Ring das
   aelteste, identisch zum alten ASM - kein Voll-Check). Immer 0. */
int ui_input_dispatch(uint32_t type, uint32_t key_code, uint32_t x, uint32_t y);

/* Setzt das aktive Theme und markiert alle Fenster als beschaedigt. */
void ui_set_theme(uint32_t theme_id);

/* 0 = Erfolg, *out_node_id erhaelt die ID; -1 wenn Tabelle voll. */
int ui_scenegraph_add(uint32_t parent_id, uint32_t type, uint32_t x, uint32_t y,
                       uint32_t w, uint32_t h, uint32_t *out_node_id);

#endif
