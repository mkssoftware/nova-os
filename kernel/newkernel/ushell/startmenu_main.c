/* NovaOS - Userspace-Startmenue (Ring 3). Zweiter Schritt der
   schrittweisen Desktop-Shell-Portierung nach Ring 3 (siehe shell_main.c
   fuer die Taskbar und kernel/newkernel/ushell_process.c fuer den
   Kernel-seitigen Prozess-Lader). KEINE Kernel-Header - laeuft
   vollstaendig in Ring 3, spricht nur ueber int-0x80-Syscalls mit dem
   Kernel. render2d.c/font.c werden UNVERAENDERT aus dem Kernel-Baum
   mitkompiliert (reine Pixel-/Glyph-Mathematik, keine Kernel-
   Abhaengigkeiten).

   Koordination mit der Taskbar (ein zweiter, unabhaengiger Ring-3-
   Prozess mit eigenem Adressraum): ueber ein winziges, rein
   kernelseitiges Sichtbarkeits-Flag (SYSCALL_CORE_TOGGLE_START_MENU in
   shell_main.c, SYSCALL_CORE_GET_START_MENU_VISIBLE hier) - keine echte
   IPC-Anbindung fuer Ring 3 noetig. */
#include <stdint.h>
#include "render2d.h"

#define SYSCALL_SERVICE_CORE                 1u
#define SYSCALL_CORE_GET_DISPLAY_INFO        3u
#define SYSCALL_CORE_GET_START_MENU_VISIBLE  6u
#define SYSCALL_CORE_SHUTDOWN                7u

#define SYSCALL_SERVICE_GUI       2u
#define GUI_OP_CREATE_SURFACE     1u
#define GUI_OP_ATTACH_BUFFER      2u
#define GUI_OP_COMMIT             3u
#define GUI_OP_HIDE               6u

#define SYSCALL_SERVICE_INPUT 4u
#define INPUT_OP_POLL         1u

#define SYSCALL_ABI_VERSION 1u

/* Layout dieses Prozesses - MUSS mit ushell_process.c uebereinstimmen.
   Dieselbe feste GUI_BUFFER_VADDR wie shell_main.c (0x402000, siehe
   dessen Scope-Hinweis) - KEIN Konflikt, jeder Prozess hat seinen
   eigenen Adressraum. */
#define ARGS_VADDR       0x00400000u
#define GUI_BUFFER_VADDR 0x00402000u

#define TASKBAR_HEIGHT    28u
#define MENU_WIDTH        160u
#define MENU_ITEM_H       20u
#define MENU_ENTRY_COUNT  4u

static const char *const menu_entries[MENU_ENTRY_COUNT] = {
    "Explorer", "Terminal", "Settings", "Shutdown"
};
#define MENU_HEIGHT ((MENU_ENTRY_COUNT + 1u) * MENU_ITEM_H)

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

static void draw_menu(render2d_surface_t *surf) {
    render2d_fill_rect(surf, 0u, 0u, MENU_WIDTH, MENU_HEIGHT, 0x20242Cu);
    render2d_fill_rect(surf, 0u, 0u, MENU_WIDTH, MENU_ITEM_H, 0x2D5FA0u);
    render2d_draw_text(surf, 6u, (MENU_ITEM_H - 16u) / 2u, "Start Menu", 0xFFFFFFu, 0x2D5FA0u, 0);

    for (uint32_t i = 0; i < MENU_ENTRY_COUNT; ++i) {
        uint32_t y = (i + 1u) * MENU_ITEM_H;
        render2d_draw_text(surf, 6u, y + (MENU_ITEM_H - 16u) / 2u, menu_entries[i],
                            0xFFFFFFu, 0x20242Cu, 0);
    }
}

void startmenu_start(void) __asm__("startmenu_start");

void startmenu_start(void) {
    uint32_t edi, esi;

    syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_GET_DISPLAY_INFO, 0u, 0u, &edi, &esi);
    uint32_t height = esi;
    if (height <= TASKBAR_HEIGHT + MENU_HEIGHT) {
        for (;;) { __asm__ volatile ("pause"); }
    }
    uint32_t menu_y = height - TASKBAR_HEIGHT - MENU_HEIGHT;

    uint32_t create_edx = 0u | (menu_y << 16);
    uint32_t create_esi = MENU_WIDTH | (MENU_HEIGHT << 16);
    syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_CREATE_SURFACE, create_edx, create_esi, &edi, 0);
    uint32_t surface_handle = edi;

    syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_ATTACH_BUFFER, surface_handle, 0u, 0, 0);

    render2d_surface_t surf;
    render2d_init_surface(&surf, (uint32_t *)GUI_BUFFER_VADDR, MENU_WIDTH, MENU_HEIGHT, MENU_WIDTH);
    draw_menu(&surf);

    uint32_t currently_shown = 0;
    uint32_t prev_buttons = 0;
    input_poll_args_t *args = (input_poll_args_t *)ARGS_VADDR;

    for (;;) {
        uint32_t visible;
        syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_GET_START_MENU_VISIBLE, 0u, 0u, &visible, 0);

        if (visible != 0u && currently_shown == 0u) {
            syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_COMMIT, surface_handle, 0u, 0, 0);
            currently_shown = 1u;
        } else if (visible == 0u && currently_shown != 0u) {
            syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_HIDE, surface_handle, 0u, 0, 0);
            currently_shown = 0u;
        }

        if (currently_shown != 0u) {
            syscall_io(SYSCALL_SERVICE_INPUT, INPUT_OP_POLL, ARGS_VADDR, 0u, 0, 0);
            uint32_t buttons = args->mouse_buttons;
            if ((buttons & 1u) != 0u && (prev_buttons & 1u) == 0u) {
                if (args->cursor_x < MENU_WIDTH && args->cursor_y >= menu_y + MENU_ITEM_H &&
                    args->cursor_y < menu_y + MENU_HEIGHT) {
                    uint32_t idx = (args->cursor_y - menu_y) / MENU_ITEM_H - 1u;
                    if (idx < MENU_ENTRY_COUNT) {
                        const char *label = menu_entries[idx];
                        if (label[0] == 'S' && label[1] == 'h') { /* "Shutdown" */
                            syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_SHUTDOWN, 0u, 0u, 0, 0);
                        }
                    }
                }
            }
            prev_buttons = buttons;
        } else {
            prev_buttons = 0;
        }

        for (volatile uint32_t spin = 0; spin < 400000u; ++spin) { }
    }
}
