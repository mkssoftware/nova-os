/* NovaOS - Userspace-Desktop-Icons (Ring 3). Dritter Schritt der
   schrittweisen Desktop-Shell-Portierung nach Ring 3 (siehe shell_main.c
   fuer die Taskbar, startmenu_main.c fuer das Startmenue,
   kernel/newkernel/ushell_process.c fuer den Kernel-seitigen
   Prozess-Lader). KEINE Kernel-Header fuer die Syscall-Schicht - laeuft
   vollstaendig in Ring 3, spricht nur ueber int-0x80-Syscalls mit dem
   Kernel. render2d.c/font.c/svgicon.c werden UNVERAENDERT aus dem
   Kernel-Baum mitkompiliert (reine Pixel-/Glyph-/SVG-Rastermathematik,
   keine Kernel-Abhaengigkeiten - svgicon.c inkludiert nur render2d.h).
   svgicon_assets.h liefert dieselben eingebetteten SVG-Quellen wie der
   bisherige Kernel-Code (desktop_shell.c's svg_source_for_label).

   Bewusst IDENTISches Verhalten zum bisherigen Kernel-Code: die Icons
   sind statisch (Explorer/Terminal), es gibt KEINE Klick-Behandlung
   (desktop_shell.c's desktop_shell_handle_click() behandelte bisher nur
   Taskbar-/Start-Menue-Klicks, nie Icon-Klicks) - dieser Port fuegt also
   bewusst KEINE neue Funktionalitaet hinzu, nur dieselbe Darstellung in
   einem eigenen Ring-3-Prozess statt im Kernel. */
#include <stdint.h>
#include "render2d.h"
#include "svgicon.h"
#include "svgicon_assets.h"

#define SYSCALL_SERVICE_CORE            1u
#define SYSCALL_CORE_GET_DISPLAY_INFO   3u

#define SYSCALL_SERVICE_GUI       2u
#define GUI_OP_CREATE_SURFACE     1u
#define GUI_OP_ATTACH_BUFFER      2u
#define GUI_OP_COMMIT             3u

#define SYSCALL_ABI_VERSION 1u

/* Layout dieses Prozesses - MUSS mit ushell_process.c uebereinstimmen.
   Dieselbe feste GUI_BUFFER_VADDR wie shell_main.c/startmenu_main.c
   (0x402000) - KEIN Konflikt, jeder Prozess hat seinen eigenen
   Adressraum. */
#define GUI_BUFFER_VADDR 0x00402000u

#define TASKBAR_HEIGHT       28u
#define ICON_MARGIN          16u
#define ICON_CELL_W          64u
#define ICON_CELL_H          56u
#define ICON_BOX_SIZE        32u
#define ICON_COUNT           2u

#define DESKTOP_SHELL_BG_RGB 0x1E2430u
#define ICON_BOX_RGB         0x3A4150u
#define ICON_FG_RGB          0xE0E0E0u

#define GRID_WIDTH  (ICON_COUNT * ICON_CELL_W)
#define GRID_HEIGHT ICON_CELL_H

typedef struct {
    const char *label;
    const char *svg_src;
    uint32_t svg_len;
} icon_entry_t;

static const icon_entry_t icon_entries[ICON_COUNT] = {
    { "Explorer", SVGICON_ASSET_FOLDER,   sizeof(SVGICON_ASSET_FOLDER) - 1u },
    { "Terminal", SVGICON_ASSET_TERMINAL, sizeof(SVGICON_ASSET_TERMINAL) - 1u },
};

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

/* Einzelner, nicht statischer Scratch-Puffer fuer svgicon_render() -
   bewusst OHNE den 8-Slot-Cache aus svgicon_render_cached() (jener
   verbraucht alleine ~144 KiB statisches .bss, hier unnoetig: diese
   zwei Icons werden nur EINMAL beim Prozessstart gerendert, kein
   wiederholtes Neuzeichnen pro Frame wie bei der Taskbar-Uhr). */
static svgicon_bitmap_t icon_bitmap;

static void draw_icons(render2d_surface_t *surf) {
    render2d_fill_rect(surf, 0u, 0u, GRID_WIDTH, GRID_HEIGHT, DESKTOP_SHELL_BG_RGB);

    for (uint32_t i = 0; i < ICON_COUNT; ++i) {
        uint32_t cell_x = i * ICON_CELL_W;
        uint32_t box_x = cell_x + (ICON_CELL_W - ICON_BOX_SIZE) / 2u;
        render2d_fill_rect(surf, box_x, 2u, ICON_BOX_SIZE, ICON_BOX_SIZE, ICON_BOX_RGB);

        if (svgicon_render(icon_entries[i].svg_src, icon_entries[i].svg_len, ICON_BOX_SIZE,
                            SVGICON_NO_THEME_OVERRIDE, &icon_bitmap) == 0) {
            svgicon_blit_blend(surf, box_x, 2u, &icon_bitmap);
        }

        render2d_draw_text(surf, cell_x + 2u, ICON_BOX_SIZE + 6u, icon_entries[i].label,
                            ICON_FG_RGB, DESKTOP_SHELL_BG_RGB, 0);
    }
}

void icons_start(void) __asm__("icons_start");

void icons_start(void) {
    uint32_t edi, esi;

    syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_GET_DISPLAY_INFO, 0u, 0u, &edi, &esi);
    uint32_t width = edi;
    uint32_t height = esi;
    if (width < GRID_WIDTH || height <= TASKBAR_HEIGHT + ICON_MARGIN + GRID_HEIGHT) {
        for (;;) { __asm__ volatile ("pause"); }
    }

    uint32_t create_edx = ICON_MARGIN | (ICON_MARGIN << 16);
    uint32_t create_esi = GRID_WIDTH | (GRID_HEIGHT << 16);
    syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_CREATE_SURFACE, create_edx, create_esi, &edi, 0);
    uint32_t surface_handle = edi;

    syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_ATTACH_BUFFER, surface_handle, 0u, 0, 0);

    render2d_surface_t surf;
    render2d_init_surface(&surf, (uint32_t *)GUI_BUFFER_VADDR, GRID_WIDTH, GRID_HEIGHT, GRID_WIDTH);
    draw_icons(&surf);

    syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_COMMIT, surface_handle, 0u, 0, 0);

    /* Statische Darstellung, kein Redraw-/Eingabe-Polling noetig (siehe
       Dateikopf) - idle, damit der Scheduler-Slot keine CPU-Zeit
       verschwendet. */
    for (;;) { __asm__ volatile ("pause"); }
}
