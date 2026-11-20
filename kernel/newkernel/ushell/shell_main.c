/* NovaOS - Userspace-Taskbar (Ring 3). Erster Schritt der schrittweisen
   Portierung der Desktop Shell aus dem Kernel (siehe kernel/newkernel/
   ushell_process.c fuer den Kernel-seitigen Prozess-Lader und dessen
   Scope-Hinweis). KEINE Kernel-Header - dieser Code laeuft vollstaendig
   in Ring 3 und darf nur ueber int-0x80-Syscalls (syscall.h) mit dem
   Kernel sprechen. render2d.c/font.c werden UNVERAENDERT aus dem
   Kernel-Baum mitkompiliert (siehe build-ushell.ps1) - beide sind reine
   Pixel-/Glyph-Mathematik ohne jede Kernel-Abhaengigkeit (kein PMM/
   Paging/Syscall-Zugriff), daher sicher in Ring 3 ausfuehrbar. */
#include <stdint.h>
#include "render2d.h"

/* Wire-Protokoll-Konstanten 1:1 aus syscall.h uebernommen (dort fuer den
   Kernel; dieser Baum kompiliert komplett unabhaengig vom Kernel, daher
   keine gemeinsame Header-Datei - siehe ushell_process.c fuer den
   Kernel-seitigen Gegenpart, der dieselben Werte verwendet). */
#define SYSCALL_SERVICE_CORE            1u
#define SYSCALL_CORE_GET_DISPLAY_INFO   3u
#define SYSCALL_CORE_GET_TIME           4u
#define SYSCALL_CORE_TOGGLE_START_MENU  5u
#define SYSCALL_CORE_TOGGLE_EXPLORER    8u
#define SYSCALL_CORE_TOGGLE_SETTINGS    10u
#define SYSCALL_CORE_YIELD              13u

#define SYSCALL_SERVICE_GUI       2u
#define GUI_OP_CREATE_SURFACE     1u
#define GUI_OP_ATTACH_BUFFER      2u
#define GUI_OP_COMMIT             3u

#define SYSCALL_SERVICE_INPUT 4u
#define INPUT_OP_POLL         1u

#define SYSCALL_ABI_VERSION 1u

/* Layout dieses Prozesses (siehe ushell_process.c fuer die passenden
   paging_map_page_into()-Aufrufe - MUSS mit dort uebereinstimmen).
   ARGS_VADDR liegt im von syscall.h vorgeschriebenen Bootstrap-Userspace-
   Fenster [0x400000, 0x404000) (syscall_validate_user_range).
   GUI_BUFFER_VADDR ist bewusst DIESELBE Adresse wie ring3.c's Demoprozess
   (RING3_GUI_BUFFER_VADDR 0x402000, siehe ring3.h) - KEIN Konflikt, jeder
   Prozess hat seinen eigenen Adressraum, aber syscall.c's GUI_OP_ATTACH_
   BUFFER loest zur Laufzeit genau DIESE eine, fest verdrahtete virtuelle
   Adresse auf (siehe dessen Scope-Hinweis) - ein anderer Wert wuerde dort
   ins Leere laufen. */
#define ARGS_VADDR       0x00400000u
#define GUI_BUFFER_VADDR 0x00402000u

#define TASKBAR_HEIGHT  28u
#define TASKBAR_START_X 16u
#define TASKBAR_START_W 40u

/* Hit-Boxen der drei angehefteten Programme (siehe draw_taskbar()'s
   draw_folder_icon(72,5)/draw_grid_icon(106,6)/draw_terminal_icon(138,6) -
   dieselben X-Koordinaten, mit etwas Rand um die jeweilige Icon-Breite
   (18/16/20px) herum). Ordner -> Explorer (SYSCALL_CORE_TOGGLE_EXPLORER,
   identisches Sichtbarkeits-Flag-Muster wie beim Startmenue-Eintrag).
   Grid -> Einstellungen (SYSCALL_CORE_TOGGLE_SETTINGS, gleiches Muster).
   Terminal bleibt bewusst OHNE Hit-Box/Funktion (keine Kommandoumgebung
   vorhanden - NovaDOS zurueckgestellt, siehe project_desktop_os_mandate -
   ein Klick waere sonst ein wirkungsloser Schalter, identisch zur
   Begruendung in startmenu_main.c). */
#define TASKBAR_FOLDER_X 72u
#define TASKBAR_GRID_X   106u
#define TASKBAR_ICON_W   22u
#define TASKBAR_BG_RGB  0x10141Cu
#define TASKBAR_FG_RGB  0xEEF6FFu
#define TASKBAR_DIM_RGB 0x8EA3BCu
#define NOVA_BLUE_RGB   0x2C8CFFu
#define NOVA_CYAN_RGB   0x5FE7FFu

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

static void put_pixel(render2d_surface_t *surf, uint32_t x, uint32_t y, uint32_t rgb) {
    if (x < surf->width && y < surf->height) surf->pixels[y * surf->stride + x] = rgb;
}

static void draw_nova_star(render2d_surface_t *surf, uint32_t cx, uint32_t cy) {
    /* Vierstrahliges Nova-Zeichen aus der visuellen Referenz. Die drei
       Farbstufen erzeugen auf dem kleinen 18px-Raster einen leuchtenden,
       aber weiterhin scharfen Startpunkt ohne Bitmap-Abhaengigkeit. */
    for (int32_t dy = -9; dy <= 9; ++dy) {
        for (int32_t dx = -9; dx <= 9; ++dx) {
            uint32_t ax = (uint32_t)(dx < 0 ? -dx : dx);
            uint32_t ay = (uint32_t)(dy < 0 ? -dy : dy);
            int inside = (ax <= 1u && ay <= 9u) || (ay <= 1u && ax <= 9u) ||
                         (ax + ay <= 7u && (ax <= 3u || ay <= 3u));
            if (!inside) continue;
            uint32_t color = (ax + ay <= 3u) ? 0xFFFFFFu :
                             (ax + ay <= 6u ? NOVA_CYAN_RGB : NOVA_BLUE_RGB);
            put_pixel(surf, (uint32_t)((int32_t)cx + dx),
                      (uint32_t)((int32_t)cy + dy), color);
        }
    }
}

static void draw_folder_icon(render2d_surface_t *surf, uint32_t x, uint32_t y) {
    render2d_fill_rect(surf, x + 2u, y + 5u, 18u, 12u, 0x2F7DE1u);
    render2d_fill_rect(surf, x + 4u, y + 3u, 7u, 3u, 0x68B8FFu);
    render2d_draw_hline(surf, x + 3u, y + 6u, 16u, 0x9AD5FFu);
}

static void draw_grid_icon(render2d_surface_t *surf, uint32_t x, uint32_t y) {
    for (uint32_t row = 0; row < 2u; ++row) {
        for (uint32_t col = 0; col < 2u; ++col) {
            render2d_fill_rect(surf, x + col * 8u, y + row * 8u, 6u, 6u,
                               (row == 0u && col == 0u) ? NOVA_CYAN_RGB : NOVA_BLUE_RGB);
        }
    }
}

static void draw_terminal_icon(render2d_surface_t *surf, uint32_t x, uint32_t y) {
    render2d_draw_rect_outline(surf, x, y, 20u, 16u, 0x7289A6u);
    put_pixel(surf, x + 4u, y + 5u, TASKBAR_FG_RGB);
    put_pixel(surf, x + 5u, y + 6u, TASKBAR_FG_RGB);
    put_pixel(surf, x + 4u, y + 7u, TASKBAR_FG_RGB);
    render2d_draw_hline(surf, x + 9u, y + 10u, 6u, NOVA_CYAN_RGB);
}

static void draw_taskbar(render2d_surface_t *surf, uint32_t width, uint32_t hour, uint32_t minute) {
    render2d_fill_rect(surf, 0u, 0u, width, TASKBAR_HEIGHT, TASKBAR_BG_RGB);

    draw_nova_star(surf, TASKBAR_START_X + 18u, TASKBAR_HEIGHT / 2u);

    /* Drei angeheftete Anwendungen. Der feine Punkt darunter entspricht
       der aktiven/angehefteten Markierung aus den NovaOS-Referenzen. */
    draw_folder_icon(surf, 72u, 5u);
    draw_grid_icon(surf, 106u, 6u);
    draw_terminal_icon(surf, 138u, 6u);
    put_pixel(surf, 82u, 25u, NOVA_CYAN_RGB);
    put_pixel(surf, 113u, 25u, 0x496C98u);
    put_pixel(surf, 148u, 25u, 0x496C98u);

    char clock_label[6] = "00:00";
    if (hour > 99u) hour = 99u;
    if (minute > 99u) minute = 99u;
    clock_label[0] = (char)('0' + (hour / 10u));
    clock_label[1] = (char)('0' + (hour % 10u));
    clock_label[3] = (char)('0' + (minute / 10u));
    clock_label[4] = (char)('0' + (minute % 10u));
    /* Vorher standen hier drei rein dekorative Tray-Icons (WLAN/Lautstaerke/
       Akku) ohne jede echte Datenquelle - dieser Kernel hat weder einen
       NIC-Treiber noch ein Audio-Subsystem noch einen ACPI-Akku-/AC-
       Adapter-Treiber (siehe project_taskbar_tray_icons_removed.md fuer die
       Begruendung). Nova zeigt deshalb nur, was es tatsaechlich weiss: die
       echte Uhrzeit (SYSCALL_CORE_GET_TIME). */
    uint32_t clock_w = 5u * 8u;
    uint32_t clock_x = (width > clock_w + 12u) ? width - clock_w - 12u : 0u;
    render2d_draw_text(surf, clock_x, (TASKBAR_HEIGHT - 16u) / 2u,
                       clock_label, TASKBAR_FG_RGB, TASKBAR_BG_RGB, 0);
}

void ushell_start(void) __asm__("ushell_start");

void ushell_start(void) {
    uint32_t edi, esi;

    syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_GET_DISPLAY_INFO, 0u, 0u, &edi, &esi);
    uint32_t width = edi;
    uint32_t height = esi;
    if (width == 0u || height <= TASKBAR_HEIGHT) {
        for (;;) { syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_YIELD, 0u, 0u, 0, 0); }
    }

    uint32_t create_edx = 0u | ((height - TASKBAR_HEIGHT) << 16);
    uint32_t create_esi = width | (TASKBAR_HEIGHT << 16);
    syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_CREATE_SURFACE, create_edx, create_esi, &edi, 0);
    uint32_t surface_handle = edi;

    syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_ATTACH_BUFFER, surface_handle, 0u, 0, 0);

    render2d_surface_t surf;
    render2d_init_surface(&surf, (uint32_t *)GUI_BUFFER_VADDR, width, TASKBAR_HEIGHT, width);

    uint32_t prev_buttons = 0;
    input_poll_args_t *args = (input_poll_args_t *)ARGS_VADDR;

    for (;;) {
        uint32_t hour, minute;
        syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_GET_TIME, 0u, 0u, &hour, &minute);
        draw_taskbar(&surf, width, hour, minute);
        syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_COMMIT, surface_handle, 0u, 0, 0);

        syscall_io(SYSCALL_SERVICE_INPUT, INPUT_OP_POLL, ARGS_VADDR, 0u, 0, 0);
        uint32_t buttons = args->mouse_buttons;
        if ((buttons & 1u) != 0u && (prev_buttons & 1u) == 0u) {
            if (args->cursor_y >= height - TASKBAR_HEIGHT) {
                if (args->cursor_x >= TASKBAR_START_X &&
                    args->cursor_x < TASKBAR_START_X + TASKBAR_START_W) {
                    syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_TOGGLE_START_MENU, 0u, 0u, 0, 0);
                } else if (args->cursor_x >= TASKBAR_FOLDER_X &&
                           args->cursor_x < TASKBAR_FOLDER_X + TASKBAR_ICON_W) {
                    syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_TOGGLE_EXPLORER, 0u, 0u, 0, 0);
                } else if (args->cursor_x >= TASKBAR_GRID_X &&
                           args->cursor_x < TASKBAR_GRID_X + TASKBAR_ICON_W) {
                    syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_TOGGLE_SETTINGS, 0u, 0u, 0, 0);
                }
            }
        }
        prev_buttons = buttons;

        syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_YIELD, 0u, 0u, 0, 0);
    }
}
