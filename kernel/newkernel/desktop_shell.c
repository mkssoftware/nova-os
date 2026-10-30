/* NovaOS C-Kernel - Nova Desktop Shell (siehe desktop_shell.h fuer Scope). */
#include "desktop_shell.h"
#include "display.h"
#include "surface.h"
#include "window.h"
#include "render2d.h"
#include "interrupt.h"
#include "threadmgr.h"
#include "svgicon.h"
#include "svgicon_assets.h"

/* Bewusst 0, NICHT 1 (= GUI_SINGLE_CLIENT_PID in syscall.c, die echte
   Ring-3-Demo-Anwendung) - sonst wuerde compositor.c's einmaliger
   "Display-Server zeigt echten Client-Inhalt"-Beweis (siehe dort
   compositor_log_client_proof_once) faelschlich bereits auf einem dieser
   Shell-eigenen Chrome-Fenster feuern, statt auf dem tatsaechlichen
   Ring-3-Client zu warten - compositor.c ueberspringt owner_pid==0
   deshalb gezielt in diesem Beweis. */
#define SHELL_OWNER_PID 0u

/* Statt die Taskbar-Pixelflaeche auf display_width() dynamisch zu
   allokieren (kein Heap-Zugriff an dieser Stelle gewuenscht - alle
   anderen GUI-Module nutzen ebenfalls feste statische Puffer, siehe
   compositor.c's win_pixels/surface_pixels in dessen Selbsttest), wird auf
   einen festen Maximalwert begrenzt. Bei einem breiteren Framebuffer bleibt
   der Rest rechts der Taskbar-Fuellfarbe des Compositor-Hintergrunds
   ueberlassen (sichtbarer, aber harmloser Rand - alle real getesteten
   GOP-Aufloesungen dieses Projekts liegen deutlich darunter). */
#define TASKBAR_MAX_WIDTH 1280u

#define TASKBAR_BG_RGB    0x10141Cu
#define TASKBAR_START_RGB 0x2D5FA0u
#define TASKBAR_START_W   64u
#define TASKBAR_FG_RGB    0xFFFFFFu
#define TASKBAR_SYSTRAY_W 56u
#define TASKBAR_CLOCK_W   72u
#define TASKBAR_TASK_W    72u

/* Gleicher Wert wie DESKTOP_BACKGROUND_RGB in desktop.c - Icon-Fenster
   sollen optisch mit dem Compositor-Hintergrund verschmelzen, auf dem sie
   liegen (kein echtes Alpha-Blending fuer window.c-Surfaces, siehe
   compositor.c compositor_draw_surfaces_clip: reines 1:1-Blit). */
#define DESKTOP_SHELL_BG_RGB 0x1E2430u
#define ICON_BOX_RGB         0x3A4150u
#define ICON_FG_RGB          0xE0E0E0u
#define ICON_CELL_W          64u
#define ICON_CELL_H          56u
#define ICON_BOX_SIZE        32u
#define ICON_MARGIN          16u

#define START_MENU_BG_RGB      0x20242Cu
#define START_MENU_HEADER_RGB  0x2D5FA0u
#define START_MENU_ITEM_RGB    0x2A2F38u
#define START_MENU_FG_RGB      0xFFFFFFu

#define NOTIFY_FG_RGB 0xFFFFFFu

/* Mindestabstand zwischen zwei automatischen Taskbar-/Notification-Takten
   des Shell-Threads, in echten Timer-Ticks (siehe compositor.c's
   COMPOSITOR_THREAD_INTERVAL_TICKS fuer dasselbe Muster). ~20 Ticks bei
   100Hz PIT entspricht ~200ms - fuer eine Uhr/Fensterliste/Notification-
   Ablauf voellig ausreichend, kein Grund fuer das engere Compositor-
   Intervall. */
#define SHELL_THREAD_INTERVAL_TICKS 20u

/* ~4s bei 100Hz PIT (siehe Scope-Hinweis in desktop_shell.h: keine RTC,
   nur Timer-Ticks). */
#define NOTIFY_TIMEOUT_TICKS 400u

static uint32_t shell_state_value = SHELL_STATE_UNINITIALIZED;

static uint32_t shell_taskbar_handle = 0;
static uint32_t shell_taskbar_width = 0;
static uint32_t taskbar_pixels[TASKBAR_MAX_WIDTH * TASKBAR_HEIGHT];

typedef struct {
    int used;
    uint32_t grid_x, grid_y;
    const char *label; /* Aufrufer haelt die Lebensdauer (String-Literal,
                           analog zu anderen Modulen, die const char* ohne
                           eigene Kopie speichern). */
    uint32_t handle;
} desktop_icon_slot_t;
static desktop_icon_slot_t desktop_icons[DESKTOP_ICON_CAPACITY];
static uint32_t desktop_icon_used_count = 0;
static uint32_t icon_pixels[DESKTOP_ICON_CAPACITY][ICON_CELL_W * ICON_CELL_H];

typedef struct {
    const char *label;
} start_menu_entry_t;
static start_menu_entry_t start_menu_entries[START_MENU_CAPACITY];
static uint32_t start_menu_entry_used = 0;
static uint32_t shell_start_menu_handle = 0;
#define START_MENU_MAX_HEIGHT (START_MENU_ITEM_H * (START_MENU_CAPACITY + 1u))
static uint32_t start_menu_pixels[START_MENU_WIDTH * START_MENU_MAX_HEIGHT];

typedef struct {
    const char *label;
} systray_entry_t;
static systray_entry_t systray_entries[SYSTRAY_ICON_CAPACITY];
static uint32_t systray_entry_used = 0;

typedef struct {
    int used;
    uint32_t type;
    const char *text;
    uint32_t handle;
    uint32_t expire_tick;
} notify_slot_t;
static notify_slot_t notify_queue[NOTIFY_QUEUE_CAPACITY];
static uint32_t notify_toast_pixels[NOTIFY_QUEUE_CAPACITY][NOTIFY_TOAST_WIDTH * NOTIFY_TOAST_HEIGHT];

static uint32_t shell_last_thread_tick = 0;

static void desktop_shell_uint_to_str(uint32_t value, char *out, uint32_t out_cap) {
    /* Kleine, bewusste Duplikation von compositor.c's gleichnamigem
       Helfer: dort ist die Funktion static (kein geteiltes String-Util-
       Modul im Kernel vorhanden) - analoges Vorgehen wie bei den bereits
       mehrfach parallel existierenden Capability-/Objekt-Konzepten an
       anderer Stelle im Projekt. */
    if (out_cap == 0) return;
    if (value == 0) {
        out[0] = '0';
        if (out_cap > 1) out[1] = '\0';
        return;
    }
    char tmp[12];
    uint32_t n = 0;
    while (value > 0 && n < 11u) {
        tmp[n++] = (char)('0' + (value % 10u));
        value /= 10u;
    }
    uint32_t i = 0;
    while (n > 0 && i < out_cap - 1u) {
        out[i++] = tmp[--n];
    }
    out[i] = '\0';
}

/* Haengt den (unveraenderten) Pufferzeiger erneut an und committet - der
   einzige Weg, den Compositor ueber frisch gezeichnete Pixel in einem
   bereits ACTIVE Fenster zu informieren (surface_attach_buffer nimmt auch
   ACTIVE an, surface_commit erfordert danach ATTACHED/HIDDEN - siehe
   surface.h). Funktioniert unveraendert fuer die allererste Zeichnung
   (Zustand CONFIGURED). */
static int shell_window_mark_dirty(uint32_t handle, void *buffer) {
    if (surface_attach_buffer(handle, (uint32_t)(uintptr_t)buffer) != 0) return -1;
    if (surface_commit(handle) != 0) return -1;
    return 0;
}

static int shell_handle_is_internal(uint32_t handle) {
    if (handle == shell_taskbar_handle) return 1;
    if (handle == shell_start_menu_handle) return 1;
    for (uint32_t i = 0; i < DESKTOP_ICON_CAPACITY; ++i) {
        if (desktop_icons[i].used && desktop_icons[i].handle == handle) return 1;
    }
    for (uint32_t i = 0; i < NOTIFY_QUEUE_CAPACITY; ++i) {
        if (notify_queue[i].used && notify_queue[i].handle == handle) return 1;
    }
    return 0;
}

void desktop_shell_initialize(void) {
    shell_state_value = SHELL_STATE_UNINITIALIZED;
    shell_taskbar_handle = 0;
    shell_taskbar_width = 0;
    for (uint32_t i = 0; i < DESKTOP_ICON_CAPACITY; ++i) desktop_icons[i] = (desktop_icon_slot_t){0};
    desktop_icon_used_count = 0;
    for (uint32_t i = 0; i < START_MENU_CAPACITY; ++i) start_menu_entries[i] = (start_menu_entry_t){0};
    start_menu_entry_used = 0;
    shell_start_menu_handle = 0;
    for (uint32_t i = 0; i < SYSTRAY_ICON_CAPACITY; ++i) systray_entries[i] = (systray_entry_t){0};
    systray_entry_used = 0;
    for (uint32_t i = 0; i < NOTIFY_QUEUE_CAPACITY; ++i) notify_queue[i] = (notify_slot_t){0};
    shell_last_thread_tick = 0;
}

uint32_t desktop_shell_state(void) {
    return shell_state_value;
}

int start_menu_register_static(const char *label, uint32_t *out_index) {
    if (label == 0) return -1;
    if (start_menu_entry_used >= START_MENU_CAPACITY) return -1;
    uint32_t idx = start_menu_entry_used++;
    start_menu_entries[idx].label = label;
    if (out_index != 0) *out_index = idx;
    return 0;
}

uint32_t start_menu_entry_count(void) {
    return start_menu_entry_used;
}

int desktop_icon_register_static(uint32_t grid_x, uint32_t grid_y, const char *label) {
    if (label == 0) return -1;
    if (desktop_icon_used_count >= DESKTOP_ICON_CAPACITY) return -1;
    uint32_t slot = desktop_icon_used_count++;
    desktop_icons[slot].used = 1;
    desktop_icons[slot].grid_x = grid_x;
    desktop_icons[slot].grid_y = grid_y;
    desktop_icons[slot].label = label;
    desktop_icons[slot].handle = 0;
    return 0;
}

uint32_t desktop_icon_count(void) {
    return desktop_icon_used_count;
}

int systray_register_static(const char *label) {
    if (label == 0) return -1;
    if (systray_entry_used >= SYSTRAY_ICON_CAPACITY) return -1;
    systray_entries[systray_entry_used++].label = label;
    return 0;
}

uint32_t systray_entry_count(void) {
    return systray_entry_used;
}

int taskbar_redraw(void) {
    if (shell_taskbar_handle == 0 || shell_taskbar_width == 0) return -1;

    render2d_surface_t surf;
    render2d_init_surface(&surf, taskbar_pixels, shell_taskbar_width, TASKBAR_HEIGHT,
                           shell_taskbar_width);

    render2d_fill_rect(&surf, 0u, 0u, shell_taskbar_width, TASKBAR_HEIGHT, TASKBAR_BG_RGB);
    render2d_fill_rect(&surf, 0u, 0u, TASKBAR_START_W, TASKBAR_HEIGHT, TASKBAR_START_RGB);
    render2d_draw_text(&surf, 8u, (TASKBAR_HEIGHT - 16u) / 2u, "Start", TASKBAR_FG_RGB,
                        TASKBAR_START_RGB, 0);

    /* NPSPEC-SHELL-TASKBAR-0002 (Running Tasks): reines Auflisten der
       aktuell sichtbaren, nicht zur Shell gehoerenden Fenster - kein
       Klick-Handler (kein Input-Subsystem, siehe window.h). */
    uint32_t list_x = TASKBAR_START_W + 8u;
    uint32_t list_limit = (shell_taskbar_width > TASKBAR_SYSTRAY_W + TASKBAR_CLOCK_W + 8u)
                               ? shell_taskbar_width - TASKBAR_SYSTRAY_W - TASKBAR_CLOCK_W - 8u
                               : list_x;
    uint32_t capacity = window_capacity();
    for (uint32_t slot = 0; slot < capacity; ++slot) {
        uint32_t handle = window_handle_at_slot(slot);
        if (handle == 0) continue;
        if (shell_handle_is_internal(handle)) continue;
        if (!window_is_visible(handle)) continue;
        if (list_x + TASKBAR_TASK_W > list_limit) break;

        char label[12] = "W";
        desktop_shell_uint_to_str(handle, &label[1], sizeof(label) - 1u);
        render2d_fill_rect(&surf, list_x, 2u, TASKBAR_TASK_W - 4u, TASKBAR_HEIGHT - 4u,
                            START_MENU_ITEM_RGB);
        render2d_draw_text(&surf, list_x + 4u, (TASKBAR_HEIGHT - 16u) / 2u, label,
                            TASKBAR_FG_RGB, START_MENU_ITEM_RGB, 0);
        list_x += TASKBAR_TASK_W;
    }

    /* Systemablage (NPSPEC-SHELL-SYSTEM-0002): reine Text-Platzhalter, da
       apower.h/network.h keine fuer eine Anzeige sinnvolle Akkustand-/
       Lautstaerke-/Signalstaerke-Abfrage anbieten (siehe desktop_shell.h
       Scope-Hinweis). */
    uint32_t tray_x = (shell_taskbar_width > TASKBAR_SYSTRAY_W + TASKBAR_CLOCK_W)
                          ? shell_taskbar_width - TASKBAR_SYSTRAY_W - TASKBAR_CLOCK_W
                          : 0u;
    for (uint32_t i = 0; i < systray_entry_used && tray_x + 16u <= shell_taskbar_width; ++i) {
        render2d_draw_text(&surf, tray_x, (TASKBAR_HEIGHT - 16u) / 2u, "N/A", TASKBAR_FG_RGB,
                            TASKBAR_BG_RGB, 0);
        tray_x += 16u;
    }

    /* Uhr (NPSPEC-SHELL-TASKBAR-0001 Clock Display), reduziert auf die
       Laufzeit in Timer-Ticks - keine RTC-Anbindung an dieser Stelle. */
    char clock_label[12];
    desktop_shell_uint_to_str(timer_get_ticks(), clock_label, sizeof(clock_label));
    uint32_t clock_x = (shell_taskbar_width > TASKBAR_CLOCK_W) ? shell_taskbar_width - TASKBAR_CLOCK_W
                                                                : 0u;
    render2d_draw_text(&surf, clock_x, (TASKBAR_HEIGHT - 16u) / 2u, clock_label, TASKBAR_FG_RGB,
                        TASKBAR_BG_RGB, 0);

    return shell_window_mark_dirty(shell_taskbar_handle, taskbar_pixels);
}

static void start_menu_draw(void) {
    uint32_t h = (start_menu_entry_used + 1u) * START_MENU_ITEM_H;
    render2d_surface_t surf;
    render2d_init_surface(&surf, start_menu_pixels, START_MENU_WIDTH, h, START_MENU_WIDTH);

    render2d_fill_rect(&surf, 0u, 0u, START_MENU_WIDTH, START_MENU_ITEM_H, START_MENU_HEADER_RGB);
    render2d_draw_text(&surf, 6u, (START_MENU_ITEM_H - 16u) / 2u, "Start Menu", START_MENU_FG_RGB,
                        START_MENU_HEADER_RGB, 0);

    for (uint32_t i = 0; i < start_menu_entry_used; ++i) {
        uint32_t y = (i + 1u) * START_MENU_ITEM_H;
        render2d_fill_rect(&surf, 0u, y, START_MENU_WIDTH, START_MENU_ITEM_H, START_MENU_ITEM_RGB);
        render2d_draw_text(&surf, 6u, y + (START_MENU_ITEM_H - 16u) / 2u,
                            start_menu_entries[i].label, START_MENU_FG_RGB, START_MENU_ITEM_RGB, 0);
    }
}

/* Ordnet einem Desktop-Icon-Label (siehe desktop_icon_register_static) den
   passenden eingebetteten SVG-Quellcode zu (svgicon_assets.h) - 0/0, falls
   kein Icon fuer dieses Label existiert (dann bleibt die bisherige
   Farbbox+Textlabel-Darstellung die einzige Darstellung, siehe
   desktop_icon_draw). cache_key ist willkuerlich, aber pro Label STABIL
   gewaehlt (siehe svgicon_render_cached) - muss nur innerhalb dieses
   Moduls eindeutig sein. */
static int svg_source_for_label(const char *label, const char **out_src, uint32_t *out_len, uint32_t *out_cache_key) {
    if (label[0] == 'E' && label[1] == 'x') { /* "Explorer" */
        *out_src = SVGICON_ASSET_FOLDER; *out_len = sizeof(SVGICON_ASSET_FOLDER) - 1; *out_cache_key = 1u; return 1;
    }
    if (label[0] == 'T' && label[1] == 'e') { /* "Terminal" */
        *out_src = SVGICON_ASSET_TERMINAL; *out_len = sizeof(SVGICON_ASSET_TERMINAL) - 1; *out_cache_key = 2u; return 1;
    }
    if (label[0] == 'S' && label[1] == 'e') { /* "Settings" */
        *out_src = SVGICON_ASSET_SETTINGS; *out_len = sizeof(SVGICON_ASSET_SETTINGS) - 1; *out_cache_key = 3u; return 1;
    }
    return 0;
}

static void desktop_icon_draw(uint32_t slot) {
    render2d_surface_t surf;
    render2d_init_surface(&surf, icon_pixels[slot], ICON_CELL_W, ICON_CELL_H, ICON_CELL_W);

    render2d_fill_rect(&surf, 0u, 0u, ICON_CELL_W, ICON_CELL_H, DESKTOP_SHELL_BG_RGB);
    uint32_t box_x = (ICON_CELL_W - ICON_BOX_SIZE) / 2u;
    render2d_fill_rect(&surf, box_x, 2u, ICON_BOX_SIZE, ICON_BOX_SIZE, ICON_BOX_RGB);

    /* Echtes SVG-Icon (siehe svgicon.h) ueber die Farbbox blenden, falls
       fuer dieses Label eines hinterlegt ist - sonst bleibt die Farbbox
       mit Text-Label die Darstellung (unveraendertes Fallback-Verhalten,
       z.B. fuer vom Nutzer spaeter registrierte Icons ohne SVG-Quelle). */
    const char *svg_src; uint32_t svg_len; uint32_t cache_key;
    if (svg_source_for_label(desktop_icons[slot].label, &svg_src, &svg_len, &cache_key)) {
        svgicon_bitmap_t *icon;
        if (svgicon_render_cached(cache_key, svg_src, svg_len, ICON_BOX_SIZE,
                                   SVGICON_NO_THEME_OVERRIDE, &icon) == 0) {
            svgicon_blit_blend(&surf, box_x, 2u, icon);
        }
    }

    render2d_draw_text(&surf, 2u, ICON_BOX_SIZE + 6u, desktop_icons[slot].label, ICON_FG_RGB,
                        DESKTOP_SHELL_BG_RGB, 0);
}

int desktop_shell_show(void) {
    uint32_t width = display_width();
    uint32_t height = display_height();
    if (width == 0 || height == 0 || height <= TASKBAR_HEIGHT) return -1;

    shell_taskbar_width = (width > TASKBAR_MAX_WIDTH) ? TASKBAR_MAX_WIDTH : width;
    if (window_create(SHELL_OWNER_PID, 0u, height - TASKBAR_HEIGHT, shell_taskbar_width,
                       TASKBAR_HEIGHT, 0, &shell_taskbar_handle) != 0) {
        return -1;
    }
    if (taskbar_redraw() != 0) return -1;

    for (uint32_t i = 0; i < desktop_icon_used_count; ++i) {
        uint32_t x = ICON_MARGIN + desktop_icons[i].grid_x * ICON_CELL_W;
        uint32_t y = ICON_MARGIN + desktop_icons[i].grid_y * ICON_CELL_H;
        if (x + ICON_CELL_W > width || y + ICON_CELL_H > height - TASKBAR_HEIGHT) continue;

        uint32_t handle;
        if (window_create(SHELL_OWNER_PID, x, y, ICON_CELL_W, ICON_CELL_H, 0, &handle) != 0) {
            continue;
        }
        desktop_icon_draw(i);
        if (shell_window_mark_dirty(handle, icon_pixels[i]) != 0) {
            window_destroy(handle);
            continue;
        }
        desktop_icons[i].handle = handle;
    }

    if (start_menu_entry_used > 0u) {
        uint32_t menu_h = (start_menu_entry_used + 1u) * START_MENU_ITEM_H;
        uint32_t menu_y = (menu_h < height - TASKBAR_HEIGHT) ? (height - TASKBAR_HEIGHT - menu_h) : 0u;
        if (window_create(SHELL_OWNER_PID, 0u, menu_y, START_MENU_WIDTH, menu_h, 0,
                           &shell_start_menu_handle) == 0) {
            start_menu_draw();
            if (shell_window_mark_dirty(shell_start_menu_handle, start_menu_pixels) == 0) {
                surface_hide(shell_start_menu_handle);
            }
        }
    }

    shell_state_value = SHELL_STATE_READY;
    return 0;
}

int start_menu_show(void) {
    if (shell_start_menu_handle == 0) return -1;
    if (window_is_visible(shell_start_menu_handle)) return 0;
    return surface_commit(shell_start_menu_handle);
}

int start_menu_hide(void) {
    if (shell_start_menu_handle == 0) return -1;
    if (!window_is_visible(shell_start_menu_handle)) return 0;
    return surface_hide(shell_start_menu_handle);
}

int start_menu_toggle(void) {
    if (shell_start_menu_handle == 0) return -1;
    return window_is_visible(shell_start_menu_handle) ? start_menu_hide() : start_menu_show();
}

int start_menu_is_visible(void) {
    if (shell_start_menu_handle == 0) return 0;
    return window_is_visible(shell_start_menu_handle);
}

uint32_t desktop_shell_taskbar_handle(void) {
    return shell_taskbar_handle;
}

int desktop_shell_handle_click(uint32_t window_handle, uint32_t local_x, uint32_t local_y) {
    if (window_handle == 0 || window_handle != shell_taskbar_handle) return 0;
    if (local_x < TASKBAR_START_W && local_y < TASKBAR_HEIGHT) {
        start_menu_toggle();
    }
    return 1;
}

static void notify_draw(uint32_t slot) {
    uint32_t bg;
    switch (notify_queue[slot].type) {
        case NOTIFY_TYPE_SUCCESS: bg = 0x2E7D32u; break;
        case NOTIFY_TYPE_WARNING: bg = 0xB8860Bu; break;
        case NOTIFY_TYPE_ERROR:   bg = 0xA0303Au; break;
        default:                  bg = 0x2D5FA0u; break; /* NOTIFY_TYPE_INFO */
    }

    render2d_surface_t surf;
    render2d_init_surface(&surf, notify_toast_pixels[slot], NOTIFY_TOAST_WIDTH,
                           NOTIFY_TOAST_HEIGHT, NOTIFY_TOAST_WIDTH);
    render2d_fill_rect(&surf, 0u, 0u, NOTIFY_TOAST_WIDTH, NOTIFY_TOAST_HEIGHT, bg);
    render2d_draw_text(&surf, 8u, 8u, notify_queue[slot].text, NOTIFY_FG_RGB, bg, 0);
}

int notify_push(uint32_t type, const char *text, uint32_t *out_slot) {
    if (text == 0) return -1;

    uint32_t slot = NOTIFY_QUEUE_CAPACITY;
    for (uint32_t i = 0; i < NOTIFY_QUEUE_CAPACITY; ++i) {
        if (!notify_queue[i].used) { slot = i; break; }
    }
    if (slot == NOTIFY_QUEUE_CAPACITY) return -1;

    uint32_t width = display_width();
    uint32_t x = (width > NOTIFY_TOAST_WIDTH + 8u) ? width - NOTIFY_TOAST_WIDTH - 8u : 0u;
    uint32_t y = 8u + slot * (NOTIFY_TOAST_HEIGHT + 8u);

    notify_queue[slot].type = type;
    notify_queue[slot].text = text;

    uint32_t handle;
    if (window_create(SHELL_OWNER_PID, x, y, NOTIFY_TOAST_WIDTH, NOTIFY_TOAST_HEIGHT, 0,
                       &handle) != 0) {
        return -1;
    }
    notify_draw(slot);
    if (shell_window_mark_dirty(handle, notify_toast_pixels[slot]) != 0) {
        window_destroy(handle);
        return -1;
    }

    notify_queue[slot].handle = handle;
    notify_queue[slot].expire_tick = timer_get_ticks() + NOTIFY_TIMEOUT_TICKS;
    notify_queue[slot].used = 1;
    if (out_slot != 0) *out_slot = slot;
    return 0;
}

void notify_tick(void) {
    uint32_t now = timer_get_ticks();
    for (uint32_t i = 0; i < NOTIFY_QUEUE_CAPACITY; ++i) {
        if (!notify_queue[i].used) continue;
        if ((int32_t)(now - notify_queue[i].expire_tick) < 0) continue;

        window_destroy(notify_queue[i].handle);
        notify_queue[i] = (notify_slot_t){0};
    }
}

uint32_t notify_active_count(void) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < NOTIFY_QUEUE_CAPACITY; ++i) {
        if (notify_queue[i].used) ++count;
    }
    return count;
}

void desktop_shell_thread_body(void) {
    for (;;) {
        uint32_t now = timer_get_ticks();
        if ((now - shell_last_thread_tick) >= SHELL_THREAD_INTERVAL_TICKS) {
            shell_last_thread_tick = now;
            taskbar_redraw();
            notify_tick();
        }
        __asm__ volatile ("pause");
    }
}

int desktop_shell_thread_start(void) {
    uint32_t tid;
    shell_last_thread_tick = timer_get_ticks();
    if (thread_create_dynamic((uint32_t)(uintptr_t)desktop_shell_thread_body, 1u, -1,
                               &tid) != 0) {
        return -1;
    }
    shell_state_value = SHELL_STATE_ACTIVE;
    return 0;
}

int desktop_shell_self_test(void) {
    /* Nutzt bewusst das bereits vom fruehen "Desktop First"-Boot-Pfad
       (kmain.c, BIB-Framebuffer) initialisierte display.c/framebuffer.c -
       ein erneuter display_initialize()-Aufruf mit Phantasiewerten wuerde
       den echten, bereits aktiven Framebuffer stilllegen (siehe
       framebuffer_initialize: address==0 -> fb_ok=0). surface.c/window.c
       werden dagegen gefahrlos zurueckgesetzt, da zu diesem Zeitpunkt im
       Boot noch niemand ausserhalb von desktop_shell.c echte Fenster
       darin angelegt hat (die fruehe desktop_show()-Phase zeichnet nur
       noch Hintergrund+Cursor direkt ueber compositor.c, ohne window.c/
       surface.c zu beruehren, siehe desktop.c). */
    surface_manager_initialize();
    window_manager_initialize();
    desktop_shell_initialize();

    /* NPSPEC-SHELL-DESKTOP-0002 / -START-0004 / -SYSTEM-0002: statische
       Registrierung VOR desktop_shell_show(), wie von der oeffentlichen
       API vorgeschrieben. */
    if (desktop_icon_register_static(0u, 0u, "Explorer") != 0) return -1;
    if (desktop_icon_register_static(1u, 0u, "Terminal") != 0) return -1;
    if (desktop_icon_count() != 2u) return -1;

    uint32_t idx;
    if (start_menu_register_static("Explorer", &idx) != 0 || idx != 0u) return -1;
    if (start_menu_register_static("Settings", 0) != 0) return -1;
    if (start_menu_entry_count() != 2u) return -1;

    if (systray_register_static("Battery") != 0) return -1;
    if (systray_register_static("Network") != 0) return -1;
    if (systray_entry_count() != 2u) return -1;

    if (desktop_shell_show() != 0) return -1;
    if (desktop_shell_state() != SHELL_STATE_READY) return -1;
    if (shell_taskbar_handle == 0) return -1;
    if (!window_is_visible(shell_taskbar_handle)) return -1;

    /* Beide Desktop-Icons muessen als eigene, sichtbare window.c-Fenster
       an den erwarteten Rasterpositionen angelegt worden sein. */
    if (desktop_icons[0].handle == 0 || !window_is_visible(desktop_icons[0].handle)) return -1;
    if (desktop_icons[1].handle == 0 || !window_is_visible(desktop_icons[1].handle)) return -1;
    uint32_t ix, iy, iw, ih;
    if (surface_rect(desktop_icons[1].handle, &ix, &iy, &iw, &ih) != 0) return -1;
    if (ix != ICON_MARGIN + ICON_CELL_W || iy != ICON_MARGIN) return -1;

    /* Startmenue existiert, ist aber anfangs verborgen (NPSPEC-SHELL-
       START-0001 Lifecycle: nicht dauerhaft sichtbar). */
    if (shell_start_menu_handle == 0) return -1;
    if (start_menu_is_visible() != 0) return -1;
    if (start_menu_toggle() != 0) return -1;
    if (start_menu_is_visible() != 1) return -1;
    if (start_menu_toggle() != 0) return -1;
    if (start_menu_is_visible() != 0) return -1;

    /* desktop_shell_handle_click() (input.c's Klick-Routing-Hook): ein
       Klick auf den Start-Button (lokale Koordinaten innerhalb von
       TASKBAR_START_W x TASKBAR_HEIGHT) muss das Startmenue umschalten
       UND als "von der Shell beansprucht" gemeldet werden; ein Klick auf
       die restliche Taskbar-Flaeche ebenfalls beansprucht, aber OHNE das
       Startmenue zu beruehren; ein Klick auf ein fremdes Fenster-Handle
       (0) ist nicht Sache der Shell. */
    if (desktop_shell_taskbar_handle() != shell_taskbar_handle) return -1;
    if (desktop_shell_handle_click(shell_taskbar_handle, 10u, 10u) != 1) return -1;
    if (start_menu_is_visible() != 1) return -1;
    if (desktop_shell_handle_click(shell_taskbar_handle, TASKBAR_START_W + 20u, 10u) != 1) {
        return -1;
    }
    if (start_menu_is_visible() != 1) return -1; /* Klick ausserhalb des Buttons aendert nichts */
    if (desktop_shell_handle_click(0u, 10u, 10u) != 0) return -1;
    if (desktop_shell_handle_click(shell_taskbar_handle, 10u, 10u) != 1) return -1;
    if (start_menu_is_visible() != 0) return -1; /* zurueck auf verborgen */

    /* Ein per window_create() erzeugtes, der Shell unbekanntes Fenster
       ("echte Anwendung") muss in der naechsten Taskbar-Neuzeichnung
       auftauchen (NPSPEC-SHELL-TASKBAR-0002), ein Shell-eigenes Fenster
       (Icon) hingegen nicht erneut als eigener Running-Task. */
    uint32_t app_handle;
    static uint32_t app_pixels[8 * 8];
    if (window_create(2u, 100u, 100u, 8u, 8u, 0, &app_handle) != 0) return -1;
    if (shell_window_mark_dirty(app_handle, app_pixels) != 0) return -1;
    if (shell_handle_is_internal(app_handle)) return -1;
    if (taskbar_redraw() != 0) return -1;
    window_destroy(app_handle);

    /* NPSPEC-SHELL-NOTIFY-0001/0002: Queue, Toast-Fenster, automatischer
       Timeout-Ablauf (keine Benutzer-Dismiss-Interaktion, siehe
       desktop_shell.h). */
    uint32_t slot;
    if (notify_push(NOTIFY_TYPE_WARNING, "Akku niedrig", &slot) != 0) return -1;
    if (notify_active_count() != 1u) return -1;
    if (!window_is_visible(notify_queue[slot].handle)) return -1;

    notify_tick(); /* noch nicht abgelaufen */
    if (notify_active_count() != 1u) return -1;

    notify_queue[slot].expire_tick = timer_get_ticks(); /* "jetzt" erzwingen */
    notify_tick();
    if (notify_active_count() != 0u) return -1;

    if (notify_push(NOTIFY_TYPE_INFO, "Download abgeschlossen", 0) != 0) return -1;
    if (notify_active_count() != 1u) return -1;

    /* Aufraeumen: alle noch aktiven Toasts (hier genau der eine von
       oben) per erzwungenem Ablauf wieder entfernen, damit der
       Selbsttest keine Fenster-/Surface-Slots belegt zurueckbleiben
       laesst. */
    uint32_t now = timer_get_ticks();
    for (uint32_t i = 0; i < NOTIFY_QUEUE_CAPACITY; ++i) {
        if (notify_queue[i].used) notify_queue[i].expire_tick = now;
    }
    notify_tick();
    if (notify_active_count() != 0u) return -1;

    /* Aufraeumen: anders als compositor_thread_start (nur EIN echter Aufruf
       in kmain.c, siehe dortigen Kommentar) wird desktop_shell_thread_start
       bewusst NICHT hier im Selbsttest aufgerufen - genau wie
       compositor_self_test es fuer compositor_thread_start ebenfalls nicht
       tut. Stattdessen werden alle hier angelegten echten window.c/
       surface.c-Fenster (Taskbar, Icons, Startmenue) wieder entfernt, damit
       die anschliessende PRODUKTIVE desktop_shell_show()-Ausfuehrung in
       kmain.c auf einer leeren Fenster-/Surface-Tabelle aufsetzt (gleiches
       Muster wie compositor_self_test/window_self_test, die ihre
       Testfenster ebenfalls vor der Rueckkehr zerstoeren). */
    if (shell_taskbar_handle != 0) window_destroy(shell_taskbar_handle);
    for (uint32_t i = 0; i < DESKTOP_ICON_CAPACITY; ++i) {
        if (desktop_icons[i].used && desktop_icons[i].handle != 0) window_destroy(desktop_icons[i].handle);
    }
    if (shell_start_menu_handle != 0) window_destroy(shell_start_menu_handle);
    desktop_shell_initialize();

    return 0;
}
