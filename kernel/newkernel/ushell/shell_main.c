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
#define TASKBAR_START_W 64u

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

static void draw_taskbar(render2d_surface_t *surf, uint32_t width, uint32_t hour, uint32_t minute) {
    render2d_fill_rect(surf, 0u, 0u, width, TASKBAR_HEIGHT, 0x10141Cu);
    render2d_fill_rect(surf, 0u, 0u, TASKBAR_START_W, TASKBAR_HEIGHT, 0x2D5FA0u);
    render2d_draw_text(surf, 8u, (TASKBAR_HEIGHT - 16u) / 2u, "Start", 0xFFFFFFu, 0x2D5FA0u, 0);

    char clock_label[6] = "00:00";
    if (hour > 99u) hour = 99u;
    if (minute > 99u) minute = 99u;
    clock_label[0] = (char)('0' + (hour / 10u));
    clock_label[1] = (char)('0' + (hour % 10u));
    clock_label[3] = (char)('0' + (minute / 10u));
    clock_label[4] = (char)('0' + (minute % 10u));
    uint32_t clock_w = 5u * 8u;
    uint32_t clock_x = (width > clock_w + 8u) ? width - clock_w - 8u : 0u;
    render2d_draw_text(surf, clock_x, (TASKBAR_HEIGHT - 16u) / 2u, clock_label, 0xFFFFFFu, 0x10141Cu, 0);
}

void ushell_start(void) __asm__("ushell_start");

void ushell_start(void) {
    uint32_t edi, esi;

    syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_GET_DISPLAY_INFO, 0u, 0u, &edi, &esi);
    uint32_t width = edi;
    uint32_t height = esi;
    if (width == 0u || height <= TASKBAR_HEIGHT) {
        for (;;) { __asm__ volatile ("pause"); }
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
            if (args->cursor_x < TASKBAR_START_W && args->cursor_y >= height - TASKBAR_HEIGHT) {
                syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_TOGGLE_START_MENU, 0u, 0u, 0, 0);
            }
        }
        prev_buttons = buttons;

        for (volatile uint32_t spin = 0; spin < 400000u; ++spin) { }
    }
}
