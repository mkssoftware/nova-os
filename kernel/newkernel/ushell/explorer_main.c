/* NovaOS - Userspace-Dateiexplorer (Ring 3). Vierter Ring-3-Desktop-
   Prozess (siehe shell_main.c/startmenu_main.c/icons_main.c fuer die
   vorherigen drei, kernel/newkernel/ushell_process.c fuer den
   Kernel-seitigen Prozess-Lader). KEINE Kernel-Header - laeuft
   vollstaendig in Ring 3, spricht nur ueber int-0x80-Syscalls mit dem
   Kernel. render2d.c/font.c werden UNVERAENDERT aus dem Kernel-Baum
   mitkompiliert (reine Pixel-/Glyph-Mathematik, keine Kernel-
   Abhaengigkeiten, siehe icons_main.c fuer dasselbe Muster).

   Echte VFS-Anbindung ueber SYSCALL_SERVICE_VFS (syscall.h/vfsapi.h,
   bereits vorhandene, boot-verifizierte Schicht - dieser Prozess ist nur
   ihr erster Ring-3-Nutzer): oeffnet beim Start die NovaFS-Wurzel,
   listet deren echte Eintraege per VFS_OP_READ_DIRECTORY, zeigt Name,
   Typ (Datei/Verzeichnis) und echte Groesse. Klick auf ein Verzeichnis
   navigiert tatsaechlich per VFS_OP_LOOKUP hinein (ein neues Handle, das
   alte wird auf dem Navigations-Stack gemerkt statt geschlossen); eine
   ".."-Zeile (nur sichtbar, wenn nicht an der Wurzel) navigiert per
   VFS_OP_CLOSE+Stack-Pop zurueck. Klick auf eine Datei markiert sie nur
   (echte Auswahl-Hervorhebung + Namens-/Groessenanzeige in der
   Statuszeile) - KEIN Oeffnen/Keine Vorschau, da dafuer keine echte
   Anwendungs-/Dateityp-Infrastruktur existiert (waere vorgetaeuschte
   Funktionalitaet, siehe NovaOS-GUI-Verbot von Dummy-Buttons).

   Bewusste, dokumentierte Grenze (keine versteckte Einschraenkung):
   maximal EXPLORER_MAX_VISIBLE_ROWS Eintraege gleichzeitig sichtbar,
   kein Scrollen in dieser ersten Version - bei mehr Eintraegen werden
   die ueberzaehligen schlicht nicht gezeichnet (kein Crash, kein
   Datenverlust, nur noch nicht implementiertes Scrollen).

   On-Demand-Start (2026-11-19, NACHTRAEGLICH ergaenzt): dieser Prozess
   laeuft wie die Taskbar/das Startmenue dauerhaft, zeigt seine Surface
   aber erst, nachdem der Startmenue-"Dateiexplorer"-Eintrag (siehe
   startmenu_main.c) per SYSCALL_CORE_TOGGLE_EXPLORER das gemeinsame,
   rein kernelseitige Sichtbarkeits-Flag gesetzt hat (SYSCALL_CORE_GET_
   EXPLORER_VISIBLE wird hier jeden Umlauf abgefragt) - exakt dasselbe
   Zwei-Prozess-Koordinationsmuster wie Taskbar<->Startmenue, siehe
   syscall.c. Der Schliessen-Button ruft denselben Toggle auf (verbergen
   statt GUI_OP_DESTROY) - das Fenster bleibt dadurch ueber das
   Startmenue erneut oeffenbar. */
#include <stdint.h>
#include "render2d.h"

#define SYSCALL_SERVICE_CORE               1u
#define SYSCALL_CORE_GET_DISPLAY_INFO      3u
#define SYSCALL_CORE_TOGGLE_EXPLORER       8u
#define SYSCALL_CORE_GET_EXPLORER_VISIBLE  9u

#define SYSCALL_SERVICE_GUI       2u
#define GUI_OP_CREATE_SURFACE     1u
#define GUI_OP_ATTACH_BUFFER      2u
#define GUI_OP_COMMIT             3u
#define GUI_OP_DESTROY            4u
#define GUI_OP_HIDE               6u

#define SYSCALL_SERVICE_VFS       3u
#define VFS_OP_LOOKUP             1u
#define VFS_OP_READ_DIRECTORY     5u
#define VFS_OP_CLOSE              9u

#define SYSCALL_SERVICE_INPUT 4u
#define INPUT_OP_POLL         1u

#define SYSCALL_ABI_VERSION 1u

#define NOVAFS_TYPE_FILE      1u
#define NOVAFS_TYPE_DIRECTORY 2u

/* Wire-Struct-Layouts 1:1 aus syscall.h uebernommen (dieser Baum
   kompiliert unabhaengig vom Kernel, siehe icons_main.c fuer dasselbe
   Muster). */
typedef struct {
    uint32_t dir_handle;
    uint32_t path_addr;
    uint32_t path_len;
    uint32_t flags;
    uint32_t result_handle;
} vfs_lookup_args_t;

typedef struct {
    uint32_t dir_handle;
    uint32_t index;
    uint32_t entry_addr;
} vfs_read_dir_args_t;

typedef struct {
    uint32_t handle;
} vfs_close_args_t;

typedef struct {
    uint32_t object_id;
    uint32_t object_type;
    uint32_t name_len;
    uint32_t size_lo;
    uint32_t size_hi;
    char name[255];
} vfs_dir_entry_t;

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

/* Layout dieses Prozesses - MUSS mit ushell_process.c uebereinstimmen.
   ARGS_VADDR liegt im Bootstrap-Userspace-Fenster [0x400000,0x404000)
   (syscall.h) - genug Platz (4096 Byte) fuer alle vier Wire-Structs
   gleichzeitig an festen, ueberlappungsfreien Unteradressen. */
#define ARGS_VADDR       0x00400000u
#define INPUT_ARGS_ADDR  (ARGS_VADDR + 0u)    /* 36 Byte */
#define LOOKUP_ARGS_ADDR (ARGS_VADDR + 64u)   /* 20 Byte */
#define READDIR_ARGS_ADDR (ARGS_VADDR + 128u) /* 12 Byte */
#define CLOSE_ARGS_ADDR  (ARGS_VADDR + 160u)  /* 4 Byte */
#define DIRENTRY_ADDR    (ARGS_VADDR + 256u)  /* ~275 Byte, endet bei ~531 */
#define GUI_BUFFER_VADDR 0x00402000u

#define WIN_X 60u
#define WIN_Y 60u
#define WIN_W 440u
#define WIN_H 300u

#define HEADER_H   24u
#define FOOTER_H   20u
#define ROW_H      18u
#define CLOSE_BTN_SIZE 14u
#define EXPLORER_MAX_VISIBLE_ROWS ((WIN_H - HEADER_H - FOOTER_H) / ROW_H)
#define NAV_STACK_MAX 7u /* VFSAPI_HANDLE_CAPACITY=8 insgesamt - 1 fuer das aktuell offene Handle */

#define COLOR_HEADER_BG 0x12192Au /* dunkles Liquid-Glass-Chrome, siehe project_gui_design_spec */
#define COLOR_BODY_BG   0x161F30u /* Fenster-Anthrazit (Referenz #161F30, Spec rgba(22,31,48,...)) */
#define COLOR_ROW_ALT   0x1B2436u
#define COLOR_ROW_HOVER_DIR 0x20324Au
#define COLOR_SELECTED  0x234A5Fu
#define COLOR_TEXT      0xF4F7FBu
#define COLOR_TEXT_SEC  0xA8B4C7u
#define COLOR_ACCENT    0x65D9FFu
#define COLOR_CLOSE_BTN 0xC0405Au

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

static int vfs_lookup(uint32_t dir_handle, uint32_t path_addr, uint32_t path_len,
                       uint32_t *out_handle) {
    vfs_lookup_args_t *args = (vfs_lookup_args_t *)LOOKUP_ARGS_ADDR;
    args->dir_handle = dir_handle;
    args->path_addr = path_addr;
    args->path_len = path_len;
    args->flags = 0u;
    args->result_handle = 0u;
    uint32_t status = syscall_io(SYSCALL_SERVICE_VFS, VFS_OP_LOOKUP, LOOKUP_ARGS_ADDR, 0u, 0, 0);
    if (status != 0u) return -1;
    *out_handle = args->result_handle;
    return 0;
}

static int vfs_read_dir(uint32_t dir_handle, uint32_t index, vfs_dir_entry_t *out_entry) {
    vfs_read_dir_args_t *args = (vfs_read_dir_args_t *)READDIR_ARGS_ADDR;
    args->dir_handle = dir_handle;
    args->index = index;
    args->entry_addr = DIRENTRY_ADDR;
    uint32_t status = syscall_io(SYSCALL_SERVICE_VFS, VFS_OP_READ_DIRECTORY, READDIR_ARGS_ADDR, 0u, 0, 0);
    if (status != 0u) return -1;
    const vfs_dir_entry_t *src = (const vfs_dir_entry_t *)DIRENTRY_ADDR;
    *out_entry = *src;
    return 0;
}

static void vfs_close(uint32_t handle) {
    vfs_close_args_t *args = (vfs_close_args_t *)CLOSE_ARGS_ADDR;
    args->handle = handle;
    syscall_io(SYSCALL_SERVICE_VFS, VFS_OP_CLOSE, CLOSE_ARGS_ADDR, 0u, 0, 0);
}

/* Formatiert value dezimal nach out (Nullterminierung inklusive, max.
   out_cap-1 Ziffern) - keine libc in Ring 3 verfuegbar. */
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

typedef struct {
    vfs_dir_entry_t entry;
    int valid;
} row_slot_t;

static row_slot_t rows[EXPLORER_MAX_VISIBLE_ROWS];
static uint32_t row_count = 0u;
static uint32_t nav_stack[NAV_STACK_MAX];
static uint32_t nav_depth = 0u;
static uint32_t current_handle = 0u;
static int selected_row = -1;

static void explorer_list_current_directory(void) {
    row_count = 0u;
    selected_row = -1;
    for (uint32_t i = 0u; i < EXPLORER_MAX_VISIBLE_ROWS; ++i) {
        vfs_dir_entry_t entry;
        if (vfs_read_dir(current_handle, i, &entry) != 0) break;
        rows[row_count].entry = entry;
        rows[row_count].valid = 1;
        ++row_count;
    }
}

static void explorer_open_root(void) {
    static const char root_path[1] = { '/' };
    /* root_path liegt im eigenen .rdata-Bereich dieses Prozesses, nicht
       im Bootstrap-Userspace-Fenster - syscall_validate_user_range()
       prueft nur den Puffer EINER Lookup-Operation (path_addr), nicht die
       Args-Struct-Adresse selbst, und laesst jede Adresse innerhalb des
       Prozess-eigenen Adressraums zu, die der Dispatch direkt
       dereferenzieren kann (siehe syscall.c: kein Bounce-Copy). Fuer
       einen Ring-3-Prozess ist das derselbe Adressraum wie der, in dem
       der Interrupt-Handler ohnehin laeuft (CR3 bleibt waehrend int 0x80
       unveraendert) - ein Zeiger auf eigenen .rdata ist daher gueltig. */
    vfs_lookup(0u, (uint32_t)(uintptr_t)root_path, 1u, &current_handle);
    nav_depth = 0u;
    explorer_list_current_directory();
}

static void explorer_enter_directory(const vfs_dir_entry_t *entry) {
    if (nav_depth >= NAV_STACK_MAX) return; /* Navigations-Limit erreicht, kein Absturz */
    uint32_t new_handle;
    /* entry->name liegt in DIRENTRY_ADDR (vom letzten Read-Directory-
       Aufruf) - direkt als Pfad fuer den Lookup relativ zum aktuellen
       Verzeichnis-Handle wiederverwendet, kein Kopieren noetig. */
    const vfs_dir_entry_t *live = (const vfs_dir_entry_t *)DIRENTRY_ADDR;
    (void)live;
    if (vfs_lookup(current_handle, (uint32_t)(uintptr_t)entry->name, entry->name_len,
                   &new_handle) != 0) {
        return;
    }
    nav_stack[nav_depth++] = current_handle;
    current_handle = new_handle;
    explorer_list_current_directory();
}

static void explorer_go_up(void) {
    if (nav_depth == 0u) return;
    vfs_close(current_handle);
    current_handle = nav_stack[--nav_depth];
    explorer_list_current_directory();
}

static void draw_explorer(render2d_surface_t *surf) {
    render2d_fill_rect(surf, 0u, 0u, WIN_W, WIN_H, COLOR_BODY_BG);

    render2d_fill_rect(surf, 0u, 0u, WIN_W, HEADER_H, COLOR_HEADER_BG);
    render2d_draw_text(surf, 8u, (HEADER_H - 16u) / 2u, "Dateiexplorer", COLOR_TEXT, COLOR_HEADER_BG, 0u);
    render2d_fill_rect(surf, WIN_W - CLOSE_BTN_SIZE - 6u, (HEADER_H - CLOSE_BTN_SIZE) / 2u,
                        CLOSE_BTN_SIZE, CLOSE_BTN_SIZE, COLOR_CLOSE_BTN);
    render2d_draw_text(surf, WIN_W - CLOSE_BTN_SIZE - 6u + 3u, (HEADER_H - 16u) / 2u, "x",
                        COLOR_TEXT, COLOR_CLOSE_BTN, 0u);

    uint32_t visible_row = 0u;
    if (nav_depth > 0u) {
        render2d_fill_rect(surf, 0u, HEADER_H, WIN_W, ROW_H, COLOR_BODY_BG);
        render2d_draw_text(surf, 8u, HEADER_H + (ROW_H - 16u) / 2u, "..", COLOR_ACCENT, COLOR_BODY_BG, 0u);
        ++visible_row;
    }

    for (uint32_t i = 0u; i < row_count; ++i) {
        uint32_t y = HEADER_H + visible_row * ROW_H;
        uint32_t bg = ((int)i == selected_row) ? COLOR_SELECTED
                      : ((visible_row % 2u) ? COLOR_ROW_ALT : COLOR_BODY_BG);
        render2d_fill_rect(surf, 0u, y, WIN_W, ROW_H, bg);

        char name_buf[48];
        uint32_t copy_len = rows[i].entry.name_len;
        if (copy_len > 46u) copy_len = 46u;
        for (uint32_t c = 0u; c < copy_len; ++c) name_buf[c] = rows[i].entry.name[c];
        name_buf[copy_len] = '\0';
        render2d_draw_text(surf, 8u, y + (ROW_H - 16u) / 2u, name_buf, COLOR_TEXT, bg, 0u);

        if (rows[i].entry.object_type == NOVAFS_TYPE_DIRECTORY) {
            render2d_draw_text(surf, WIN_W - 70u, y + (ROW_H - 16u) / 2u, "<DIR>", COLOR_ACCENT, bg, 0u);
        } else {
            char size_buf[12];
            format_uint(rows[i].entry.size_lo, size_buf, sizeof(size_buf));
            render2d_draw_text(surf, WIN_W - 70u, y + (ROW_H - 16u) / 2u, size_buf, COLOR_TEXT_SEC, bg, 0u);
        }
        ++visible_row;
    }

    render2d_fill_rect(surf, 0u, WIN_H - FOOTER_H, WIN_W, FOOTER_H, COLOR_HEADER_BG);
    if (selected_row >= 0 && (uint32_t)selected_row < row_count) {
        char name_buf[48];
        uint32_t copy_len = rows[selected_row].entry.name_len;
        if (copy_len > 30u) copy_len = 30u;
        for (uint32_t c = 0u; c < copy_len; ++c) name_buf[c] = rows[selected_row].entry.name[c];
        name_buf[copy_len] = '\0';
        char size_buf[12];
        format_uint(rows[selected_row].entry.size_lo, size_buf, sizeof(size_buf));
        render2d_draw_text(surf, 8u, WIN_H - FOOTER_H + (FOOTER_H - 16u) / 2u, name_buf,
                            COLOR_TEXT_SEC, COLOR_HEADER_BG, 0u);
        render2d_draw_text(surf, WIN_W - 100u, WIN_H - FOOTER_H + (FOOTER_H - 16u) / 2u, size_buf,
                            COLOR_TEXT_SEC, COLOR_HEADER_BG, 0u);
    } else {
        char count_buf[16] = "Eintraege: ";
        char num_buf[12];
        uint32_t total = row_count + ((nav_depth > 0u) ? 1u : 0u);
        format_uint(total, num_buf, sizeof(num_buf));
        render2d_draw_text(surf, 8u, WIN_H - FOOTER_H + (FOOTER_H - 16u) / 2u, count_buf,
                            COLOR_TEXT_SEC, COLOR_HEADER_BG, 0u);
        render2d_draw_text(surf, 8u + 11u * 8u, WIN_H - FOOTER_H + (FOOTER_H - 16u) / 2u, num_buf,
                            COLOR_TEXT_SEC, COLOR_HEADER_BG, 0u);
    }
}

void explorer_start(void) __asm__("explorer_start");

void explorer_start(void) {
    uint32_t edi, esi;

    syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_GET_DISPLAY_INFO, 0u, 0u, &edi, &esi);
    uint32_t width = edi;
    uint32_t height = esi;
    if (width < WIN_X + WIN_W || height < WIN_Y + WIN_H) {
        for (;;) { __asm__ volatile ("pause"); }
    }

    uint32_t create_edx = WIN_X | (WIN_Y << 16);
    uint32_t create_esi = WIN_W | (WIN_H << 16);
    syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_CREATE_SURFACE, create_edx, create_esi, &edi, 0);
    uint32_t surface_handle = edi;

    syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_ATTACH_BUFFER, surface_handle, 0u, 0, 0);

    render2d_surface_t surf;
    render2d_init_surface(&surf, (uint32_t *)GUI_BUFFER_VADDR, WIN_W, WIN_H, WIN_W);

    explorer_open_root();
    draw_explorer(&surf);

    /* On-Demand-Start (2026-11-19): wie das Startmenue (startmenu_main.c)
       bleibt dieser Prozess dauerhaft laufend, zeigt seine Surface aber
       erst, nachdem der Startmenue-"Dateiexplorer"-Eintrag per
       SYSCALL_CORE_TOGGLE_EXPLORER das gemeinsame Sichtbarkeits-Flag
       gesetzt hat (syscall.c) - kein permanentes GUI_OP_COMMIT mehr beim
       Start. */
    uint32_t currently_shown = 0u;
    uint32_t prev_buttons = 0u;
    input_poll_args_t *input_args = (input_poll_args_t *)INPUT_ARGS_ADDR;

    for (;;) {
        uint32_t visible;
        syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_GET_EXPLORER_VISIBLE, 0u, 0u, &visible, 0);

        if (visible != 0u && currently_shown == 0u) {
            syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_COMMIT, surface_handle, 0u, 0, 0);
            currently_shown = 1u;
        } else if (visible == 0u && currently_shown != 0u) {
            syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_HIDE, surface_handle, 0u, 0, 0);
            currently_shown = 0u;
        }

        if (currently_shown == 0u) {
            prev_buttons = 0u;
            for (volatile uint32_t spin = 0; spin < 400000u; ++spin) { }
            continue;
        }

        syscall_io(SYSCALL_SERVICE_INPUT, INPUT_OP_POLL, INPUT_ARGS_ADDR, 0u, 0, 0);
        uint32_t buttons = input_args->mouse_buttons;
        uint32_t cursor_x = input_args->cursor_x;
        uint32_t cursor_y = input_args->cursor_y;

        if ((buttons & 1u) != 0u && (prev_buttons & 1u) == 0u &&
            cursor_x >= WIN_X && cursor_x < WIN_X + WIN_W &&
            cursor_y >= WIN_Y && cursor_y < WIN_Y + WIN_H) {
            uint32_t local_x = cursor_x - WIN_X;
            uint32_t local_y = cursor_y - WIN_Y;
            int redraw = 0;

            if (local_y < HEADER_H) {
                if (local_x >= WIN_W - CLOSE_BTN_SIZE - 6u && local_x < WIN_W - 6u) {
                    /* Schliessen = Verbergen (wie das Startmenue per Toggle),
                       nicht mehr dauerhaftes GUI_OP_DESTROY - der Nutzer kann
                       den Explorer ueber das Startmenue erneut oeffnen. */
                    syscall_io(SYSCALL_SERVICE_CORE, SYSCALL_CORE_TOGGLE_EXPLORER, 0u, 0u, 0, 0);
                    prev_buttons = buttons;
                    for (volatile uint32_t spin = 0; spin < 400000u; ++spin) { }
                    continue;
                }
            } else if (local_y < WIN_H - FOOTER_H) {
                uint32_t visible_row = (local_y - HEADER_H) / ROW_H;
                if (nav_depth > 0u) {
                    if (visible_row == 0u) {
                        explorer_go_up();
                        redraw = 1;
                    } else {
                        uint32_t real_index = visible_row - 1u;
                        if (real_index < row_count) {
                            if (rows[real_index].entry.object_type == NOVAFS_TYPE_DIRECTORY) {
                                explorer_enter_directory(&rows[real_index].entry);
                            } else {
                                selected_row = (int)real_index;
                            }
                            redraw = 1;
                        }
                    }
                } else if (visible_row < row_count) {
                    if (rows[visible_row].entry.object_type == NOVAFS_TYPE_DIRECTORY) {
                        explorer_enter_directory(&rows[visible_row].entry);
                    } else {
                        selected_row = (int)visible_row;
                    }
                    redraw = 1;
                }
            }

            if (redraw) {
                draw_explorer(&surf);
                syscall_io(SYSCALL_SERVICE_GUI, GUI_OP_COMMIT, surface_handle, 0u, 0, 0);
            }
        }
        prev_buttons = buttons;

        for (volatile uint32_t spin = 0; spin < 400000u; ++spin) { }
    }
}
