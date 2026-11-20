/* NovaOS - Userspace-Systemeinstellungen (Ring 3). Fuenfter Ring-3-
   Desktop-Prozess (siehe shell_main.c/startmenu_main.c/icons_main.c/
   explorer_main.c fuer die vorherigen vier, kernel/newkernel/
   ushell_process.c fuer den Kernel-seitigen Prozess-Lader). KEINE
   Kernel-Header - laeuft vollstaendig in Ring 3, spricht nur ueber
   int-0x80-Syscalls mit dem Kernel. render2d.c/font.c werden
   UNVERAENDERT aus dem Kernel-Baum mitkompiliert (reine Pixel-/
   Glyph-Mathematik, keine Kernel-Abhaengigkeiten, siehe icons_main.c
   fuer dasselbe Muster).

   Zeigt AUSSCHLIESSLICH tatsaechlich vom Kernel gelieferte Werte -
   keine erfundenen Hardwareinformationen (siehe NovaOS-GUI-Verbot):
   Bildschirmaufloesung (SYSCALL_CORE_GET_DISPLAY_INFO, bereits
   vorhanden), freier physischer Speicher (SYSCALL_CORE_GET_SYSTEM_INFO,
   neu - liest echt pmm_frame_count()*PMM_PAGE_SIZE im Kernel), Uhrzeit
   (SYSCALL_CORE_GET_TIME, bereits vorhanden, liest die echte CMOS-RTC).
   Werte werden JEDEN Umlauf neu abgefragt, keine Caches/Platzhalter.

   Bewusst KEIN Schalter fuer Aufloesung/Hintergrund/Mausgeschwindigkeit -
   dafuer gibt es weiterhin keine echten, schreibenden System-APIs (nur
   GET_*, keine SET_*-Syscalls). Ein sichtbarer Schalter ohne echte
   Wirkung waere ein vorgetaeuschtes Bedienelement.

   ERSTER echter Schalter (2026-11-20): Taskbar-Auto-Hide
   (SYSCALL_CORE_TOGGLE_AUTOHIDE/_GET_AUTOHIDE_ENABLED) - diese
   Einstellung schreibt wirklich ein kernelweites Flag, das die Taskbar
   (shell_main.c) jeden Umlauf abfragt und danach tatsaechlich ein-/
   ausblendet (siehe dortigen Kommentar). Kein Platzhalter mehr.

   On-Demand-Start: identisches Muster wie explorer_main.c - bleibt
   dauerhaft laufend, zeigt seine Surface aber erst nach SYSCALL_CORE_
   TOGGLE_SETTINGS (vom Startmenue-"Settings"-Eintrag ausgeloest, siehe
   startmenu_main.c), pollt SYSCALL_CORE_GET_SETTINGS_VISIBLE jeden
   Umlauf. */
#include <stdint.h>
#include "render2d.h"

#define SYSCALL_SERVICE_CORE               1u
#define SYSCALL_CORE_GET_DISPLAY_INFO      3u
#define SYSCALL_CORE_GET_TIME              4u
#define SYSCALL_CORE_TOGGLE_SETTINGS       10u
#define SYSCALL_CORE_GET_SETTINGS_VISIBLE  11u
#define SYSCALL_CORE_YIELD                 13u
#define SYSCALL_CORE_GET_SYSTEM_INFO       12u
#define SYSCALL_CORE_TOGGLE_AUTOHIDE       15u
#define SYSCALL_CORE_GET_AUTOHIDE_ENABLED  16u

#define SYSCALL_SERVICE_GUI       2u
#define GUI_OP_CREATE_SURFACE     1u
#define GUI_OP_ATTACH_BUFFER      2u
#define GUI_OP_COMMIT             3u
#define GUI_OP_HIDE               6u
#define GUI_OP_MOVE               7u
#define GUI_OP_GET_RECT           8u
#define GUI_OP_SET_APPEARANCE     9u

#define SYSCALL_SERVICE_INPUT 4u
#define INPUT_OP_POLL         1u

#define SYSCALL_ABI_VERSION 1u

#define ARGS_VADDR       0x00400000u
#define GUI_BUFFER_VADDR 0x00402000u

/* Klein gehalten (2026-11-19, durch echten Boot-Test gefunden): der
   Bootstrap-PMM (pmm.c) traegt nur PMM_MAX_FRAMES=1024 Rahmen (4 MiB)
   insgesamt - bei fuenf gleichzeitig laufenden Ring-3-Desktop-Prozessen
   (Taskbar/Startmenue/Icons/Explorer/Settings) bleiben zum Zeitpunkt
   dieses fuenften Spawns nur noch ca. 95 freie, zusammenhaengende
   Rahmen uebrig - ein urspruenglich geplantes 360x220-Fenster brauchte
   78 davon und scheiterte knapp an `pmm_alloc_contiguous()`. 280x150
   braucht nur 41 Rahmen und passt mit Marge - kein Fehler im Allocator
   selbst (der findet nachweislich jeden tatsaechlich vorhandenen
   zusammenhaengenden Lauf), echte Ressourcenknappheit des bewusst
   kleinen Bootstrap-Pools. */
#define WIN_X 540u
#define WIN_Y 60u
#define WIN_W 280u
#define WIN_H 180u

#define HEADER_H       36u
#define CLOSE_BTN_SIZE 14u
#define ROW_H          24u

#define COLOR_HEADER_BG 0x12192Au
#define COLOR_BODY_BG   0x161F30u
#define COLOR_TEXT      0xF4F7FBu
#define COLOR_TEXT_SEC  0xA8B4C7u
#define COLOR_CLOSE_BTN 0xC0405Au
#define COLOR_ACCENT    0x65D9FFu

/* Taskbar-Auto-Hide-Schalter (2026-11-20, erster echter Schalter dieser
   App) - eine kleine klickbare "Pille", gefuellt (Cyan) wenn aktiv,
   sonst nur umrissen. Position in derselben Spalte wie die Werte oben
   (draw_label_value's x=180), aber vertikal zentriert in der 4.
   Zeile statt an deren Textbasislinie. */
#define AUTOHIDE_ROW_Y   (HEADER_H + 4u * ROW_H)
#define AUTOHIDE_TOGGLE_X 180u
#define AUTOHIDE_TOGGLE_W 44u
#define AUTOHIDE_TOGGLE_H 18u

typedef struct {
    uint32_t has_event;
    uint32_t type;
    uint32_t timestamp_ticks;
    uint32_t key_vkey;
    uint32_t modifiers;
    int32_t mouse_dx, mouse_dy;
    uint32_t mouse_buttons;
    uint32_t cursor_x, cursor_y;
} input_poll_args_t;

static inline uint32_t syscall_io(uint32_t eax_in, uint32_t ebx_in, uint32_t edx_in,
                                   uint32_t esi_in, uint32_t *out_edi, uint32_t *out_esi) {
    uint32_t eax = eax_in, ebx = ebx_in, ecx = SYSCALL_ABI_VERSION, edx = edx_in, esi = esi_in, edi = 0;
    __asm__ volatile(
        "int $0x80"
        : "+a"(eax), "+b"(ebx), "+c"(ecx), "+d"(edx), "+S"(esi), "+D"(edi)
        :
        : "memory"
    );
    if (out_edi) *out_edi = edi;
    if (out_esi) *out_esi = esi;
    return eax;
}

static void format_uint(uint32_t value, char *out, uint32_t out_cap) {
    if (out_cap == 0u) return;
    if (value == 0u) {
        out[0] = '0';
        if (out_cap > 1u) out[1] = '\0';
        return;
    }
    char tmp[12];
    uint32_t n = 0u;
    while (value > 0u && n < 11u) {
        tmp[n++] = (char)('0' + (value % 10u));
        value /= 10u;
    }
    uint32_t i = 0u;
    while (n > 0u && i < out_cap - 1u) out[i++] = tmp[--n];
    out[i] = '\0';
}

static void format_two_digits(uint32_t value, char *out) {
    if (value > 99u) value = 99u;
    out[0] = (char)('0' + (value / 10u));
    out[1] = (char)('0' + (value % 10u));
}

static void draw_label_value(render2d_surface_t *surf, uint32_t y, const char *label,
                              const char *value) {
    render2d_draw_text(surf, 8u, y, label, COLOR_TEXT_SEC, COLOR_BODY_BG, 0u);
    render2d_draw_text(surf, 180u, y, value, COLOR_TEXT, COLOR_BODY_BG, 0u);
}

static void draw_settings(render2d_surface_t *surf) {
    render2d_fill_rect(surf, 0u, 0u, surf->width, surf->height, COLOR_BODY_BG);

    render2d_fill_rect(surf, 0u, 0u, surf->width, HEADER_H, COLOR_HEADER_BG);
    render2d_draw_text(surf, 8u, (HEADER_H - 16u) / 2u, "Einstellungen", COLOR_TEXT, COLOR_HEADER_BG, 0u);
    render2d_fill_rect(surf, surf->width - CLOSE_BTN_SIZE - 6u, (HEADER_H - CLOSE_BTN_SIZE) / 2u,
                        CLOSE_BTN_SIZE, CLOSE_BTN_SIZE, COLOR_CLOSE_BTN);
    render2d_draw_text(surf, surf->width - CLOSE_BTN_SIZE - 6u + 3u, (HEADER_H - 16u) / 2u, "x",
                        COLOR_TEXT, COLOR_CLOSE_BTN, 0u);

    uint32_t edi, esi;
    syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_GET_DISPLAY_INFO, 0u, 0u, &edi, &esi);
    uint32_t width = edi, height = esi;
    char res_buf[24];
    uint32_t w_len;
    {
        char w_buf[12], h_buf[12];
        format_uint(width, w_buf, sizeof(w_buf));
        format_uint(height, h_buf, sizeof(h_buf));
        uint32_t p = 0u;
        for (uint32_t i = 0u; w_buf[i] != '\0' && p < 23u; ++i) res_buf[p++] = w_buf[i];
        if (p < 23u) res_buf[p++] = 'x';
        for (uint32_t i = 0u; h_buf[i] != '\0' && p < 23u; ++i) res_buf[p++] = h_buf[i];
        res_buf[p] = '\0';
        w_len = p;
    }
    (void)w_len;
    draw_label_value(surf, HEADER_H + 1u * ROW_H, "Bildschirmaufloesung:", res_buf);

    uint32_t free_pages, page_size;
    syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_GET_SYSTEM_INFO, 0u, 0u, &free_pages, &page_size);
    char mem_buf[24];
    {
        /* free_pages*page_size als KiB statt Byte ausgegeben - vermeidet
           einen 32-Bit-Ueberlauf bei vielen freien Seiten UND bleibt eine
           fuer Menschen sinnvolle Einheit. Division durch 1024 per Shift,
           da page_size (4096) immer ein Vielfaches von 1024 ist. */
        uint32_t free_kib = (free_pages * (page_size / 1024u));
        char num_buf[12];
        format_uint(free_kib, num_buf, sizeof(num_buf));
        uint32_t p = 0u;
        for (uint32_t i = 0u; num_buf[i] != '\0' && p < 20u; ++i) mem_buf[p++] = num_buf[i];
        mem_buf[p++] = ' '; mem_buf[p++] = 'K'; mem_buf[p++] = 'i'; mem_buf[p++] = 'B';
        mem_buf[p] = '\0';
    }
    draw_label_value(surf, HEADER_H + 2u * ROW_H, "Freier Speicher:", mem_buf);

    uint32_t hour, minute;
    syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_GET_TIME, 0u, 0u, &hour, &minute);
    char time_buf[6] = "00:00";
    format_two_digits(hour, time_buf);
    format_two_digits(minute, time_buf + 3);
    draw_label_value(surf, HEADER_H + 3u * ROW_H, "Systemzeit:", time_buf);

    /* Taskbar-Auto-Hide-Schalter - EINZIGES Bedienelement dieser App, das
       tatsaechlich etwas AENDERT (siehe Dateikopf-Kommentar). Gefuellte
       Pille = aktiv, nur umrissene Pille = inaktiv - beide Zustaende
       zusaetzlich per Text ("Ein"/"Aus") beschriftet, nicht nur Farbe. */
    render2d_draw_text(surf, 8u, AUTOHIDE_ROW_Y, "Taskbar Auto-Hide:", COLOR_TEXT_SEC, COLOR_BODY_BG, 0u);
    uint32_t autohide_enabled;
    syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_GET_AUTOHIDE_ENABLED, 0u, 0u, &autohide_enabled, 0);
    uint32_t toggle_y = AUTOHIDE_ROW_Y + (16u - AUTOHIDE_TOGGLE_H) / 2u - 1u;
    if (autohide_enabled != 0u) {
        render2d_fill_rect(surf, AUTOHIDE_TOGGLE_X, toggle_y, AUTOHIDE_TOGGLE_W, AUTOHIDE_TOGGLE_H, COLOR_ACCENT);
        render2d_draw_text(surf, AUTOHIDE_TOGGLE_X + 7u, toggle_y + 1u, "Ein", COLOR_HEADER_BG, COLOR_ACCENT, 0u);
    } else {
        render2d_draw_rect_outline(surf, AUTOHIDE_TOGGLE_X, toggle_y, AUTOHIDE_TOGGLE_W, AUTOHIDE_TOGGLE_H, COLOR_TEXT_SEC);
        render2d_draw_text(surf, AUTOHIDE_TOGGLE_X + 7u, toggle_y + 1u, "Aus", COLOR_TEXT_SEC, COLOR_BODY_BG, 0u);
    }

    /* Eckgriff-Markierung - siehe identischer Kommentar in
       explorer_main.c's draw_explorer(). */
    for (uint32_t d = 0u; d < 3u; ++d) {
        render2d_fill_rect(surf, surf->width - 4u - d * 3u, surf->height - 4u - d * 3u,
                            2u, 2u, COLOR_TEXT_SEC);
    }
}

void settings_start(void) __asm__("settings_start");

void settings_start(void) {
    uint32_t edi, esi;

    syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_GET_DISPLAY_INFO, 0u, 0u, &edi, &esi);
    uint32_t width = edi;
    uint32_t height = esi;
    if (width < WIN_X + WIN_W || height < WIN_Y + WIN_H) {
        for (;;) { syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_YIELD, 0u, 0u, 0, 0); }
    }

    uint32_t create_edx = WIN_X | (WIN_Y << 16);
    uint32_t create_esi = WIN_W | (WIN_H << 16);
    syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_CREATE_SURFACE, create_edx, create_esi, &edi, 0);
    uint32_t surface_handle = edi;

    /* Kuehles Silberblau fuer Einstellungen; die eigentliche Liquid-
       Glass-Dekoration bleibt zentral im Compositor. */
    syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_SET_APPEARANCE,
               surface_handle, 0x9BCBFFu, 0, 0);

    syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_ATTACH_BUFFER, surface_handle, 0u, 0, 0);

    render2d_surface_t surf;
    render2d_init_surface(&surf, (uint32_t *)GUI_BUFFER_VADDR, WIN_W, WIN_H, WIN_W);

    uint32_t currently_shown = 0u;
    uint32_t prev_buttons = 0u;
    input_poll_args_t *input_args = (input_poll_args_t *)ARGS_VADDR;

    /* Verschieben per eigener Kopfzeile (2026-11-19, siehe GUI_OP_MOVE in
       syscall.h, identisches Muster zu explorer_main.c) - win_x/win_y sind
       ab hier die massgebliche LAUFENDE Position fuer jeden Hit-Test. */
    uint32_t win_x = WIN_X, win_y = WIN_Y;
    uint32_t dragging = 0u;
    uint32_t drag_offset_x = 0u, drag_offset_y = 0u;
    /* Laufende Fenstergroesse (2026-11-20, Eckgriff-Resize per Maus,
       identisches Muster zu explorer_main.c) - anders als dort reicht
       hier eine einfache Neuabfrage jeden Umlauf, da draw_settings()
       wegen der "live"-Werte ohnehin bereits jeden sichtbaren Umlauf
       neu zeichnet. */
    uint32_t win_w = WIN_W, win_h = WIN_H;

    for (;;) {
        uint32_t visible;
        syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_GET_SETTINGS_VISIBLE, 0u, 0u, &visible, 0);

        if (visible != 0u) {
            uint32_t rect_w, rect_h;
            syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_GET_RECT, surface_handle, 0u, &rect_w, &rect_h);
            if (rect_w != 0u && rect_h != 0u) {
                win_w = rect_w;
                win_h = rect_h;
                surf.width = win_w;
                surf.height = win_h;
                surf.stride = win_w;
            }
            /* Werte sind live (Speicher/Uhrzeit aendern sich) - bei
               sichtbarem Fenster jeden Umlauf neu zeichnen+committen,
               nicht nur beim Sichtbarwerden. */
            draw_settings(&surf);
            syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_COMMIT, surface_handle, 0u, 0, 0);
            currently_shown = 1u;
        } else if (currently_shown != 0u) {
            syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_HIDE, surface_handle, 0u, 0, 0);
            currently_shown = 0u;
        }

        if (currently_shown != 0u) {
            syscall_io(SYSCALL_SERVICE_INPUT, INPUT_OP_POLL, ARGS_VADDR, 0u, 0, 0);
            uint32_t buttons = input_args->mouse_buttons;
            uint32_t cursor_x = input_args->cursor_x;
            uint32_t cursor_y = input_args->cursor_y;
            if ((buttons & 1u) != 0u && (prev_buttons & 1u) == 0u &&
                cursor_x >= win_x && cursor_x < win_x + win_w &&
                cursor_y >= win_y && cursor_y < win_y + win_h) {
                uint32_t local_x = cursor_x - win_x;
                uint32_t local_y = cursor_y - win_y;
                if (local_y < HEADER_H && local_x >= win_w - CLOSE_BTN_SIZE - 6u &&
                    local_x < win_w - 6u) {
                    syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_TOGGLE_SETTINGS, 0u, 0u, 0, 0);
                } else if (local_y < HEADER_H) {
                    /* Rest der Kopfzeile startet ein Ziehen - identisches
                       Muster zu explorer_main.c. */
                    dragging = 1u;
                    drag_offset_x = cursor_x - win_x;
                    drag_offset_y = cursor_y - win_y;
                } else if (local_y >= AUTOHIDE_ROW_Y && local_y < AUTOHIDE_ROW_Y + ROW_H &&
                           local_x >= AUTOHIDE_TOGGLE_X && local_x < AUTOHIDE_TOGGLE_X + AUTOHIDE_TOGGLE_W) {
                    /* Grosszuegige, volle Zeilenhoehe als Trefferzone statt
                       der exakten, etwas kleineren gezeichneten Pille -
                       leichter treffbar, ohne die Optik zu aendern. */
                    syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_TOGGLE_AUTOHIDE, 0u, 0u, 0, 0);
                }
            }
            if (dragging != 0u) {
                if ((buttons & 1u) != 0u) {
                    uint32_t new_x = (cursor_x > drag_offset_x) ? cursor_x - drag_offset_x : 0u;
                    uint32_t new_y = (cursor_y > drag_offset_y) ? cursor_y - drag_offset_y : 0u;
                    if (new_x != win_x || new_y != win_y) {
                        win_x = new_x;
                        win_y = new_y;
                        syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_MOVE, surface_handle,
                                   win_x | (win_y << 16), 0, 0);
                    }
                } else {
                    dragging = 0u;
                }
            }
            prev_buttons = buttons;
        } else {
            prev_buttons = 0u;
            dragging = 0u;
        }

        syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_YIELD, 0u, 0u, 0, 0);
    }
}
