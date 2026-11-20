/* NovaOS C-Kernel - Nova Compositor (siehe compositor.h fuer Scope). */
#include "compositor.h"
#include "framebuffer.h"
#include "surface.h"
#include "window.h"
#include "render2d.h"
#include "threadmgr.h"
#include "interrupt.h"
#include "serial.h"
#include "taskbarglass.h"
#include "paging.h"

/* Mindestabstand zwischen zwei automatischen Redraws des Compositor-
   Threads, in echten Timer-Ticks (keine VSync/VRR-Quelle vorhanden, siehe
   NPSPEC-COMPOSITOR-RENDER-0005/0006 - BLOCKED). ~5 Ticks bei 100Hz PIT
   entspricht ~50ms (~20 FPS) - ausreichend fuer sichtbare Aktualisierung,
   ohne jeden Scheduler-Durchlauf unnoetig einen vollen Frame neu zu
   zeichnen. */
#define COMPOSITOR_THREAD_INTERVAL_TICKS 2u

#define COMPOSITOR_SHADOW_OFFSET 6u
#define COMPOSITOR_SHADOW_RGB    0x000000u
#define COMPOSITOR_SHADOW_ALPHA  80u

/* NPSPEC-WINDOW-DECORATION-0001..0003 (Rahmen/Titelleiste/Controls),
   reduziert auf eine feste Titelleisten-Hoehe + Fuellfarbe (kein Theme-
   System, keine Minimize/Maximize/Close-Buttons als eigene Klickziele -
   Fenster-Drag per Titelleiste UND Klick-Fokuswechsel sind seit dem
   Input-System dagegen echt verdrahtet, siehe window.h/input.c). Nur
   window.c-Fenster mit decorated!=0 bekommen eine Titelleiste;
   compositor_windows (Desktop-Chrome wie die Taskbar) bleiben unberuehrt.
   Hoehe kommt aus window.h (WINDOW_TITLEBAR_HEIGHT), da window.c/input.c
   dieselbe Geometrie fuer das Titelleisten-Hit-Testing kennen muessen. */
#define COMPOSITOR_TITLEBAR_HEIGHT     WINDOW_TITLEBAR_HEIGHT
#define COMPOSITOR_TITLEBAR_RGB        0x17243Au
#define COMPOSITOR_TITLEBAR_FOCUS_RGB  0x203B67u
#define COMPOSITOR_GLASS_ALPHA         218u
#define COMPOSITOR_CORNER_RADIUS       20u
#define COMPOSITOR_SIGNATURE_CORE       5u
#define COMPOSITOR_SIGNATURE_GLOW      18u
#define COMPOSITOR_GLASS_TINT_RGB      0x0B1830u
#define COMPOSITOR_ACCENT_RGB          0x258BFFu
#define COMPOSITOR_ACCENT_DIM_RGB      0x28547Fu
#define COMPOSITOR_BORDER_RGB          0x739ED0u
#define COMPOSITOR_CONTROL_CLOSE_RGB   0xE75A72u

/* NPSPEC Liquid-Glass Abschnitt 10 ("Visuelle Verschmelzung von Fenster und
   Taskbar"): maximale Abweichung zwischen Fenster-Unterkante und Taskbar-
   Dock-Oberkante, bei der die Kontakterkennung noch anspringt - verhindert,
   dass der Effekt bereits bei blossem Vorbeiziehen (noch deutlicher
   Abstand) oder bei normaler Ueberlappung anderer Fenster faelschlich
   ausloest. */
#define COMPOSITOR_DOCK_MERGE_THRESHOLD 4u
#define COMPOSITOR_DOCK_MERGE_ALPHA     90u

/* Animierter Uebergang (NPSPEC Liquid-Glass Abschnitt 10.4: "~150-250ms,
   unterbrechbar") statt des bisherigen Sofort-An/Aus: COMPOSITOR_DOCK_
   ANIM_STEP ist der Fortschritts-Zuwachs pro Compositor-Redraw in Richtung
   des Ziels (0 = geloest, 255 = voll angedockt). Bei ~COMPOSITOR_THREAD_
   INTERVAL_TICKS=2 Ticks/Redraw (~20ms bei 100Hz PIT) ergeben 24 Schritte
   ~210ms fuer die volle Strecke - innerhalb der geforderten Spanne. */
#define COMPOSITOR_DOCK_ANIM_STEP       24u
#define COMPOSITOR_DOCK_ANIM_SLOTS      WINDOW_CAPACITY

typedef struct {
    uint32_t handle; /* 0 = frei */
    uint32_t progress;
    uint32_t last_ox0, last_ox1; /* letzter bekannter Kontaktbereich */
} compositor_dock_anim_t;

static compositor_dock_anim_t compositor_dock_anim[COMPOSITOR_DOCK_ANIM_SLOTS];

/* Liefert den LAUFENDEN Uebergangs-Fortschritt (0..255) fuer handle und
   bewegt ihn einen Schritt in Richtung target_docked - rein kosmetischer,
   pro-Fenster-Animationszustand, KEINE strukturelle Kopplung an die
   Taskbar (siehe compositor_draw_dock_merge_clip's Dateikopf). Bei
   target_docked wird [ox0,ox1) als LETZTER bekannter Kontaktbereich
   gemerkt und ueber out_ox0/out_ox1 zurueckgegeben - auch waehrend des
   Ausblendens (target_docked==0, Fenster bereits von der Taskbar entfernt)
   bleibt dieser Bereich erhalten, damit die Bruecke sauber ausblendet
   statt abrupt zu verschwinden. Ein Handle, das keinen freien Slot mehr
   findet (alle COMPOSITOR_DOCK_ANIM_SLOTS belegt), bekommt degradiert
   sofortige Werte statt eines Uebergangs - kein Fehler, nur ein seltener,
   harmloser Komfortverlust bei sehr vielen gleichzeitig angedockten
   Fenstern. */
static uint32_t compositor_dock_anim_step(uint32_t handle, int target_docked,
                                           uint32_t ox0, uint32_t ox1,
                                           uint32_t *out_ox0, uint32_t *out_ox1) {
    uint32_t target = target_docked ? 255u : 0u;
    int free_idx = -1;
    for (uint32_t i = 0; i < COMPOSITOR_DOCK_ANIM_SLOTS; ++i) {
        if (compositor_dock_anim[i].handle == handle) {
            uint32_t *p = &compositor_dock_anim[i].progress;
            if (*p < target) { *p += COMPOSITOR_DOCK_ANIM_STEP; if (*p > target) *p = target; }
            else if (*p > target) { *p = (*p > COMPOSITOR_DOCK_ANIM_STEP) ? *p - COMPOSITOR_DOCK_ANIM_STEP : target; }
            if (target_docked) { compositor_dock_anim[i].last_ox0 = ox0; compositor_dock_anim[i].last_ox1 = ox1; }
            if (out_ox0) *out_ox0 = compositor_dock_anim[i].last_ox0;
            if (out_ox1) *out_ox1 = compositor_dock_anim[i].last_ox1;
            if (*p == 0u && !target_docked) compositor_dock_anim[i].handle = 0u; /* Slot freigeben */
            return *p;
        }
        if (free_idx < 0 && compositor_dock_anim[i].handle == 0u) free_idx = (int)i;
    }
    if (!target_docked) return 0u; /* kein Uebergang noetig, bereits geloest */
    if (free_idx < 0) { if (out_ox0) *out_ox0 = ox0; if (out_ox1) *out_ox1 = ox1; return target; }
    compositor_dock_anim[free_idx].handle = handle;
    compositor_dock_anim[free_idx].progress = COMPOSITOR_DOCK_ANIM_STEP;
    compositor_dock_anim[free_idx].last_ox0 = ox0;
    compositor_dock_anim[free_idx].last_ox1 = ox1;
    if (out_ox0) *out_ox0 = ox0;
    if (out_ox1) *out_ox1 = ox1;
    return compositor_dock_anim[free_idx].progress;
}

typedef struct {
    uint32_t id; /* 0 = frei */
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
    uint32_t rgb;
    uint32_t alpha;
    int shadow;
} compositor_window_t;

static compositor_window_t compositor_windows[COMPOSITOR_WINDOW_CAPACITY];
static uint32_t compositor_next_id = 1;
static uint32_t compositor_background_rgb = 0x000000u;
/* Optionales Wallpaper-Bild (siehe compositor_set_background_image in
   compositor.h) - 0 = kein Bild gesetzt, es gilt weiter die einfarbige
   Flaeche aus compositor_background_rgb. Der Zeiger wird NICHT kopiert,
   der Aufrufer (wallpaper.c) haelt den eigentlichen Pixelspeicher. */
static const uint8_t *compositor_background_image = 0;
static uint32_t compositor_background_image_w = 0;
static uint32_t compositor_background_image_h = 0;
static uint32_t compositor_cursor_x = 0;
static uint32_t compositor_cursor_y = 0;
static uint32_t compositor_cursor_rgb = 0xFFFFFFu;
static int compositor_cursor_visible = 0;
static uint32_t compositor_occluded_count = 0;
static uint32_t compositor_state_value = COMPOSITOR_STATE_UNINITIALIZED;
static uint32_t compositor_last_thread_tick = 0;
static uint32_t compositor_last_frame_ticks_value = 0;

/* NPSPEC-COMPOSITOR-DAMAGE-0001 Abschnitt 5: Damage Region Manager - eine
   kleine, feste Liste rechteckiger Schadensregionen (x1/y1 exklusiv). Bei
   Ueberlauf wird auf Full Redraw eskaliert (siehe compositor_damage_mark_
   full), daher muss die Liste selbst nie wachsen. */
#define COMPOSITOR_DAMAGE_CAPACITY 8u

typedef struct {
    uint32_t x0, y0, x1, y1;
} compositor_damage_rect_t;

static compositor_damage_rect_t compositor_damage[COMPOSITOR_DAMAGE_CAPACITY];
static uint32_t compositor_damage_count = 0;
/* 1 = der naechste compositor_composite()-Aufruf MUSS den gesamten
   Bildschirm neu zeichnen (unbekannter/initialer Bufferzustand -
   Abschnitt 13). Wird mit 1 initialisiert, da vor dem allerersten Redraw
   der Framebuffer-Inhalt unbekannt ist. */
static int compositor_full_redraw_pending = 1;
static int compositor_last_full_redraw = 0;
static uint32_t compositor_last_damage_region_count_value = 0;

/* Scene Damage Tracker (Abschnitt 2): Schnappschuss des letzten bekannten
   Zustands jedes surface.c-Slots, um Oberflaechen zu erkennen, die
   zwischen zwei compositor_composite()-Aufrufen verborgen/zerstoert
   wurden (surface.c meldet das selbst nicht als Damage - der Bereich muss
   dennoch zum Hintergrund zurueckgesetzt werden, Abschnitt 4/13). */
typedef struct {
    int active;
    uint32_t x, y, w, h;
    int decorated;
} compositor_surface_snapshot_t;

static compositor_surface_snapshot_t compositor_surface_snapshot[SURFACE_CAPACITY];
static uint32_t compositor_last_focused_handle = 0;

static void compositor_damage_mark_full(void) {
    compositor_full_redraw_pending = 1;
    compositor_damage_count = 0;
}

/* Fuegt ein Schadensrechteck hinzu (Abschnitt 5). Versucht genau EINEN
   Merge mit einer bereits ueberlappenden Region (zulaessige zusaetzliche
   Pixel statt einer vollen Kompaktierung - ausreichend fuer die kleine,
   feste Anzahl an Schadensquellen in diesem Bootstrap-Compositor). Ist die
   Tabelle voll, wird konservativ auf Full Redraw eskaliert statt Damage zu
   verlieren (Abschnitt 11/13). */
static void compositor_damage_add(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    if (compositor_full_redraw_pending) return; /* schon alles abgedeckt */
    if (w == 0 || h == 0) return;

    uint32_t x1 = x + w;
    uint32_t y1 = y + h;

    for (uint32_t i = 0; i < compositor_damage_count; ++i) {
        compositor_damage_rect_t *r = &compositor_damage[i];
        if (x < r->x1 && x1 > r->x0 && y < r->y1 && y1 > r->y0) {
            if (x < r->x0) r->x0 = x;
            if (y < r->y0) r->y0 = y;
            if (x1 > r->x1) r->x1 = x1;
            if (y1 > r->y1) r->y1 = y1;
            return;
        }
    }

    if (compositor_damage_count >= COMPOSITOR_DAMAGE_CAPACITY) {
        compositor_damage_mark_full();
        return;
    }

    compositor_damage[compositor_damage_count].x0 = x;
    compositor_damage[compositor_damage_count].y0 = y;
    compositor_damage[compositor_damage_count].x1 = x1;
    compositor_damage[compositor_damage_count].y1 = y1;
    ++compositor_damage_count;
}

static uint8_t compositor_blend_channel(uint8_t src, uint8_t dst, uint32_t alpha) {
    /* Porter-Duff Source-Over (NPSPEC-COMPOSITOR-EFFECTS-0001, Abschnitt 6),
       hier mit straight statt premultiplied Alpha, da jede Layer hier nur
       eine einzelne Flaechenfarbe ohne eigenen Alphakanal pro Pixel ist. */
    uint32_t blended = (uint32_t)src * alpha + (uint32_t)dst * (255u - alpha);
    return (uint8_t)((blended + 127u) / 255u);
}

static uint32_t compositor_blend_rgb(uint32_t src, uint32_t dst, uint32_t alpha) {
    uint8_t r = compositor_blend_channel((uint8_t)(src >> 16), (uint8_t)(dst >> 16), alpha);
    uint8_t g = compositor_blend_channel((uint8_t)(src >> 8), (uint8_t)(dst >> 8), alpha);
    uint8_t b = compositor_blend_channel((uint8_t)src, (uint8_t)dst, alpha);
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

static int compositor_in_rounded_rect(uint32_t px, uint32_t py, uint32_t x, uint32_t y,
                                      uint32_t w, uint32_t h, uint32_t radius) {
    if (px < x || py < y || px >= x + w || py >= y + h) return 0;
    if (radius == 0u || w <= radius * 2u || h <= radius * 2u) return 1;
    uint32_t lx = px - x;
    uint32_t ly = py - y;
    if (lx >= radius && lx < w - radius) return 1;
    if (ly >= radius && ly < h - radius) return 1;
    int32_t cx = (lx < radius) ? (int32_t)radius - 1 : (int32_t)w - (int32_t)radius;
    int32_t cy = (ly < radius) ? (int32_t)radius - 1 : (int32_t)h - (int32_t)radius;
    int32_t dx = (int32_t)lx - cx;
    int32_t dy = (int32_t)ly - cy;
    return (uint32_t)(dx * dx + dy * dy) <= radius * radius;
}

/* NPSPEC-COMPOSITOR-DAMAGE-0001 Abschnitt 6 (Partial Redraw): wie die
   fruehere, vollbild-weite compositor_blend_rect, aber zusaetzlich auf ein
   Clip-Rechteck (cx0/cy0/cx1/cy1, exklusiv) begrenzt - das ist die aktuell
   bearbeitete Redraw-Region (ganzer Bildschirm bei Full Redraw, sonst
   genau eine Damage-Region). */
static void compositor_blend_rect_clip(uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                                        uint32_t rgb, uint32_t alpha,
                                        uint32_t cx0, uint32_t cy0, uint32_t cx1, uint32_t cy1) {
    if (alpha == 0) return;

    uint32_t fb_w = framebuffer_width();
    uint32_t fb_h = framebuffer_height();
    uint32_t x0 = x, y0 = y;
    uint32_t x_end = x + w;
    uint32_t y_end = y + h;
    if (x0 < cx0) x0 = cx0;
    if (y0 < cy0) y0 = cy0;
    if (x_end > cx1) x_end = cx1;
    if (y_end > cy1) y_end = cy1;
    if (x_end > fb_w) x_end = fb_w;
    if (y_end > fb_h) y_end = fb_h;
    if (x0 >= x_end || y0 >= y_end) return;

    uint8_t src_r = (uint8_t)((rgb >> 16) & 0xFFu);
    uint8_t src_g = (uint8_t)((rgb >> 8) & 0xFFu);
    uint8_t src_b = (uint8_t)(rgb & 0xFFu);

    for (uint32_t row = y0; row < y_end; ++row) {
        for (uint32_t col = x0; col < x_end; ++col) {
            if (alpha >= 255u) {
                framebuffer_set_pixel(col, row, rgb);
                continue;
            }

            uint32_t dst = framebuffer_get_pixel(col, row);
            uint8_t dst_r = (uint8_t)((dst >> 16) & 0xFFu);
            uint8_t dst_g = (uint8_t)((dst >> 8) & 0xFFu);
            uint8_t dst_b = (uint8_t)(dst & 0xFFu);

            uint8_t out_r = compositor_blend_channel(src_r, dst_r, alpha);
            uint8_t out_g = compositor_blend_channel(src_g, dst_g, alpha);
            uint8_t out_b = compositor_blend_channel(src_b, dst_b, alpha);

            uint32_t out = ((uint32_t)out_r << 16) | ((uint32_t)out_g << 8) | out_b;
            framebuffer_set_pixel(col, row, out);
        }
    }
}

void compositor_initialize(void) {
    for (uint32_t i = 0; i < COMPOSITOR_WINDOW_CAPACITY; ++i) compositor_windows[i] = (compositor_window_t){0};
    compositor_next_id = 1;
    compositor_background_rgb = 0x000000u;
    compositor_background_image = 0;
    compositor_background_image_w = 0;
    compositor_background_image_h = 0;
    compositor_cursor_visible = 0;
    if (compositor_state_value == COMPOSITOR_STATE_UNINITIALIZED) {
        compositor_state_value = COMPOSITOR_STATE_READY;
    }

    /* NPSPEC-COMPOSITOR-DAMAGE-0001 Abschnitt 13: unbekannter Bufferzustand
       -> voller Redraw beim naechsten compositor_composite()-Aufruf. */
    for (uint32_t i = 0; i < SURFACE_CAPACITY; ++i) {
        compositor_surface_snapshot[i] = (compositor_surface_snapshot_t){0};
    }
    compositor_last_focused_handle = 0;
    compositor_damage_mark_full();
}

uint32_t compositor_state(void) {
    return compositor_state_value;
}

void compositor_set_background(uint32_t rgb) {
    /* Der Hintergrund bedeckt den gesamten Bildschirm - ein Full Redraw ist
       hier einfacher und genauso korrekt wie eine Einzelregion ueber die
       komplette Flaeche (Abschnitt 13: "entsprechend dem tatsaechlichen
       Einflussbereich" - das ist hier der ganze Schirm). */
    compositor_background_rgb = rgb;
    compositor_damage_mark_full();
}

void compositor_set_background_image(const uint8_t *rgb888, uint32_t width, uint32_t height) {
    if (rgb888 == 0 || width == 0 || height == 0) {
        compositor_background_image = 0;
        compositor_background_image_w = 0;
        compositor_background_image_h = 0;
    } else {
        compositor_background_image = rgb888;
        compositor_background_image_w = width;
        compositor_background_image_h = height;
    }
    compositor_damage_mark_full();
}

int compositor_window_create(uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                              uint32_t rgb, uint32_t alpha, uint32_t *out_window_id) {
    for (uint32_t i = 0; i < COMPOSITOR_WINDOW_CAPACITY; ++i) {
        if (compositor_windows[i].id == 0) {
            compositor_windows[i].id = compositor_next_id++;
            compositor_windows[i].x = x;
            compositor_windows[i].y = y;
            compositor_windows[i].w = w;
            compositor_windows[i].h = h;
            compositor_windows[i].rgb = rgb;
            compositor_windows[i].alpha = (alpha > 255u) ? 255u : alpha;
            compositor_windows[i].shadow = 0;
            *out_window_id = compositor_windows[i].id;
            compositor_damage_add(x, y, w, h);
            return 0;
        }
    }
    return -1;
}

int compositor_window_set_shadow(uint32_t window_id, int enabled) {
    for (uint32_t i = 0; i < COMPOSITOR_WINDOW_CAPACITY; ++i) {
        if (compositor_windows[i].id == window_id) {
            compositor_windows[i].shadow = enabled ? 1 : 0;
            compositor_window_t *win = &compositor_windows[i];
            compositor_damage_add(win->x, win->y, win->w + COMPOSITOR_SHADOW_OFFSET,
                                   win->h + COMPOSITOR_SHADOW_OFFSET);
            return 0;
        }
    }
    return -1;
}

void compositor_set_cursor(uint32_t x, uint32_t y, uint32_t rgb) {
    /* Abschnitt 4: Bei Bewegungen MUESSEN bisherige UND neue sichtbare
       Position beruecksichtigt werden. */
    if (compositor_cursor_visible) {
        compositor_damage_add(compositor_cursor_x, compositor_cursor_y, 8u, 8u);
    }
    compositor_cursor_x = x;
    compositor_cursor_y = y;
    compositor_cursor_rgb = rgb;
    compositor_cursor_visible = 1;
    compositor_damage_add(x, y, 8u, 8u);
}

/* NPSPEC-COMPOSITOR-RENDER-0003 (Occlusion Culling), auf die feste
   Fenstertabelle angewandt: true, wenn das Rechteck von win vollstaendig
   von einem SPAETEREN (also im Z-Order darueber liegenden), voll deckenden
   Fenster ueberdeckt wird - reine AABB-Pruefung, kein echtes Scene-Graph/
   Overlay-System, da hier nur eine feste, kleine Fensterliste existiert. */
static int compositor_window_is_occluded(uint32_t index) {
    compositor_window_t *win = &compositor_windows[index];
    uint32_t win_x_end = win->x + win->w;
    uint32_t win_y_end = win->y + win->h;

    for (uint32_t j = index + 1; j < COMPOSITOR_WINDOW_CAPACITY; ++j) {
        compositor_window_t *above = &compositor_windows[j];
        if (above->id == 0) continue;
        if (above->alpha < 255u) continue; /* nicht voll deckend -> kann nicht verdecken */

        uint32_t above_x_end = above->x + above->w;
        uint32_t above_y_end = above->y + above->h;
        if (above->x <= win->x && above->y <= win->y &&
            above_x_end >= win_x_end && above_y_end >= win_y_end) {
            return 1;
        }
    }
    return 0;
}

/* Kopiert einen client-eigenen Pixel-Buffer (NPSPEC-COMPOSITOR-BUFFER-0001,
   ueber surface.c angehaengt) 1:1 in den Framebuffer - kein Alpha-Blending
   pro Pixel (Vereinfachung, siehe surface.h: w*h aufeinanderfolgende
   uint32_t 0xRRGGBB-Pixel, Stride=w*4). Clippt am Framebuffer-Rand. */
static void compositor_blit_surface_buffer_clip(uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                                                 uint32_t buffer_addr,
                                                 uint32_t cx0, uint32_t cy0, uint32_t cx1, uint32_t cy1,
                                                 uint32_t handle, uint32_t corner_radius,
                                                 int round_all_corners,
                                                 uint32_t dock_ox0, uint32_t dock_ox1) {
    if (buffer_addr == 0) return;

    uint32_t fb_w = framebuffer_width();
    uint32_t fb_h = framebuffer_height();
    uint32_t x0 = x, y0 = y;
    uint32_t x_end = x + w;
    uint32_t y_end = y + h;
    if (x0 < cx0) x0 = cx0;
    if (y0 < cy0) y0 = cy0;
    if (x_end > cx1) x_end = cx1;
    if (y_end > cy1) y_end = cy1;
    if (x_end > fb_w) x_end = fb_w;
    if (y_end > fb_h) y_end = fb_h;
    if (x0 >= x_end || y0 >= y_end) return;

    const uint32_t *pixels = (const uint32_t *)(uintptr_t)buffer_addr;
    uint32_t color_key = 0;
    int have_color_key = (surface_color_key(handle, &color_key) == 0);
    for (uint32_t row = y0; row < y_end; ++row) {
        const uint32_t *src_row = pixels + (uint32_t)(row - y) * w;
        for (uint32_t col = x0; col < x_end; ++col) {
            /* NPSPEC Liquid-Glass Abschnitt 10: im tatsaechlichen Kontakt-
               bereich mit der Taskbar bleibt die Unterkante eckig (keine
               Doppel-Rundung an der Verschmelzungsstelle), ausserhalb davon
               bleibt die normale Eckenrundung erhalten. */
            int in_dock_contact = dock_ox1 > dock_ox0 && col >= dock_ox0 && col < dock_ox1;
            if (corner_radius != 0u &&
                (round_all_corners || row >= y + h - corner_radius) &&
                !in_dock_contact &&
                !compositor_in_rounded_rect(col, row, x, y, w, h, corner_radius)) continue;
            uint32_t pixel = src_row[col - x] & 0x00FFFFFFu;
            if (have_color_key && pixel == color_key) continue;
            framebuffer_set_pixel(col, row, pixel);
        }
    }
}

/* NPSPEC-COMPOSITOR-DAMAGE-0001 Abschnitt 2 ("Surface Damage Tracker"/
   "Scene Damage Tracker"): konsumiert einmal pro compositor_composite()-
   Aufruf das pro-Surface-Damage aus surface.c (SURFACE-0005,
   surface_damage_take(), atomar lesend+loeschend) und speist es in die
   Compositor-eigene Damage-Region-Liste ein. Erkennt zusaetzlich ueber
   einen Schnappschuss des letzten bekannten Zustands jeder Surface, wenn
   eine zuvor aktive Surface verborgen/zerstoert wurde (surface.c meldet
   das selbst nicht als Damage, der alte Bildbereich muss aber dennoch
   zum Hintergrund zurueckgesetzt werden). Dekorierte window.c-Fenster
   bekommen zusaetzlich ihre Titelleiste als Damage markiert, da diese
   ausserhalb des eigentlichen Surface-Rechtecks liegt. */
static void compositor_scan_surface_damage(void) {
    uint32_t capacity = surface_capacity();
    for (uint32_t slot = 0; slot < capacity; ++slot) {
        uint32_t handle = surface_handle_at_slot(slot);
        compositor_surface_snapshot_t *snap = &compositor_surface_snapshot[slot];

        uint32_t x = 0, y = 0, w = 0, h = 0;
        int active = 0;
        if (handle != 0 && surface_state(handle) == SURFACE_STATE_ACTIVE &&
            surface_rect(handle, &x, &y, &w, &h) == 0) {
            active = 1;
        }

        if (active) {
            uint32_t dx, dy, dw, dh;
            /* surface_damage_take() statt getrenntem _damage_rect()+
               _clear_damage(): schliesst das SMP-Race-Fenster, in dem ein
               Commit auf einer anderen vCPU zwischen Lesen und Loeschen
               vorbeikommt (siehe project_taskbar_redraw_freeze_bug.md -
               sichtbarstes Symptom war eine Taskbar, die nach dem ersten
               Boot-Redraw nie wieder sichtbar aktualisiert wurde). */
            if (surface_damage_take(handle, &dx, &dy, &dw, &dh) == 0) {
                compositor_damage_add(dx, dy, dw, dh);
                int decorated = window_is_decorated(handle) && window_is_visible(handle);
                if (decorated && y >= COMPOSITOR_TITLEBAR_HEIGHT) {
                    compositor_damage_add(x, y - COMPOSITOR_TITLEBAR_HEIGHT, w, COMPOSITOR_TITLEBAR_HEIGHT);
                }
                snap->decorated = decorated;
            }
            snap->active = 1;
            snap->x = x;
            snap->y = y;
            snap->w = w;
            snap->h = h;
        } else {
            if (snap->active) {
                compositor_damage_add(snap->x, snap->y, snap->w, snap->h);
                if (snap->decorated && snap->y >= COMPOSITOR_TITLEBAR_HEIGHT) {
                    compositor_damage_add(snap->x, snap->y - COMPOSITOR_TITLEBAR_HEIGHT,
                                           snap->w, COMPOSITOR_TITLEBAR_HEIGHT);
                }
            }
            snap->active = 0;
        }
    }

    /* Fokuswechsel (reiner WM-Metadaten-Wechsel, window.c) aendert nur die
       Titelleistenfarbe, ohne eigenes Surface-Damage auszuloesen - separat
       beobachten (Scene Damage Tracker). */
    uint32_t focused = window_focused_handle();
    if (focused != compositor_last_focused_handle) {
        uint32_t x, y, w, h;
        if (compositor_last_focused_handle != 0 && window_is_decorated(compositor_last_focused_handle) &&
            surface_rect(compositor_last_focused_handle, &x, &y, &w, &h) == 0 &&
            y >= COMPOSITOR_TITLEBAR_HEIGHT) {
            compositor_damage_add(x, y - COMPOSITOR_TITLEBAR_HEIGHT, w, COMPOSITOR_TITLEBAR_HEIGHT);
        }
        if (focused != 0 && window_is_decorated(focused) &&
            surface_rect(focused, &x, &y, &w, &h) == 0 && y >= COMPOSITOR_TITLEBAR_HEIGHT) {
            compositor_damage_add(x, y - COMPOSITOR_TITLEBAR_HEIGHT, w, COMPOSITOR_TITLEBAR_HEIGHT);
        }
        compositor_last_focused_handle = focused;
    }
}

/* Zeichnet alle ACTIVE Client-Surfaces (surface.c), die die aktuelle
   Redraw-Region (cx0/cy0/cx1/cy1) ueberhaupt beruehren - das Damage selbst
   wurde bereits vorher per compositor_scan_surface_damage() konsumiert,
   hier wird nur noch tatsaechlich gezeichnet (auch fuer Surfaces ohne
   eigenes frisches Damage, falls sie zufaellig in einer von ANDEREM
   Damage ausgeloesten Region liegen - sonst blieben sie als Luecke im
   Partial Redraw stehen). */
static void compositor_draw_titlebar_clip(uint32_t handle, uint32_t x, uint32_t y,
                                           uint32_t w, uint32_t h,
                                           uint32_t cx0, uint32_t cy0, uint32_t cx1, uint32_t cy1) {
    if (!window_is_decorated(handle) || window_mode(handle) == WINDOW_MODE_FULLSCREEN ||
        y < COMPOSITOR_TITLEBAR_HEIGHT) return;

    uint32_t bar_y = y - COMPOSITOR_TITLEBAR_HEIGHT;
    uint32_t focused = window_focused_handle();
    uint32_t glass = (handle == focused) ? COMPOSITOR_TITLEBAR_FOCUS_RGB : COMPOSITOR_TITLEBAR_RGB;
    uint32_t accent = (handle == focused) ? COMPOSITOR_ACCENT_RGB : COMPOSITOR_ACCENT_DIM_RGB;
    uint32_t x0 = x > cx0 ? x : cx0;
    uint32_t y0 = bar_y > cy0 ? bar_y : cy0;
    uint32_t x1 = x + w < cx1 ? x + w : cx1;
    uint32_t y1 = y < cy1 ? y : cy1;

    for (uint32_t row = y0; row < y1; ++row) {
        for (uint32_t col = x0; col < x1; ++col) {
            if (!compositor_in_rounded_rect(col, row, x, bar_y, w,
                                            COMPOSITOR_TITLEBAR_HEIGHT + h,
                                            COMPOSITOR_CORNER_RADIUS)) continue;
            uint32_t dst = framebuffer_get_pixel(col, row);
            uint32_t color = compositor_blend_rgb(glass, dst, COMPOSITOR_GLASS_ALPHA);
            if (col < x + 3u) color = compositor_blend_rgb(accent, color, 235u);
            framebuffer_set_pixel(col, row, color);
        }
    }

    render2d_surface_t fb;
    render2d_init_surface(&fb, framebuffer_pixels(), framebuffer_width(), framebuffer_height(),
                          framebuffer_stride());
    render2d_draw_text(&fb, x + 8u, bar_y + 2u, "NovaOS", 0xF4F8FFu, glass, 0);

    if (w >= WINDOW_CONTROL_MIN_WIDTH) {
        uint32_t controls_x = x + w - WINDOW_CONTROL_COUNT * WINDOW_CONTROL_WIDTH;
        uint32_t mid_y = bar_y + COMPOSITOR_TITLEBAR_HEIGHT / 2u;
        uint32_t fg = 0xEAF2FFu;
        render2d_draw_hline(&fb, controls_x + 5u, mid_y + 3u, 8u, fg);
        render2d_draw_rect_outline(&fb, controls_x + WINDOW_CONTROL_WIDTH + 5u,
                                   mid_y - 4u, 8u, 8u, fg);
        uint32_t close_x = controls_x + WINDOW_CONTROL_WIDTH * 2u;
        if (handle == focused) {
            render2d_fill_rect(&fb, close_x, bar_y + 1u, WINDOW_CONTROL_WIDTH - 1u,
                               COMPOSITOR_TITLEBAR_HEIGHT - 2u, COMPOSITOR_CONTROL_CLOSE_RGB);
        }
        for (uint32_t i = 0; i < 7u; ++i) {
            framebuffer_set_pixel(close_x + 5u + i, mid_y - 3u + i, fg);
            framebuffer_set_pixel(close_x + 11u - i, mid_y - 3u + i, fg);
        }
    }

    /* Die charakteristische Nova-Signaturkante bleibt Teil des Fensters:
       heller Kern plus weicher, nach innen abfallender Lichtsaum. */
    uint32_t body_y1 = y + h < cy1 ? y + h : cy1;
    uint32_t body_y0 = y > cy0 ? y : cy0;
    if (x >= cx0 && x < cx1) {
        for (uint32_t row = body_y0; row < body_y1; ++row) {
            framebuffer_set_pixel(x, row, accent);
            if (x + 1u < cx1) {
                uint32_t dst = framebuffer_get_pixel(x + 1u, row);
                framebuffer_set_pixel(x + 1u, row, compositor_blend_rgb(accent, dst, 150u));
            }
            if (x + 2u < cx1) {
                uint32_t dst = framebuffer_get_pixel(x + 2u, row);
                framebuffer_set_pixel(x + 2u, row, compositor_blend_rgb(accent, dst, 65u));
            }
        }
    }
}

/* Zentrale Nova-Liquid-Glass-Dekoration fuer Ring-3-Anwendungsfenster.
   Die App liefert nur window_accent(); Geometrie, Fokusintensitaet,
   Reflex und die mehrstufige Signature Edge bleiben Compositor-Aufgabe. */
static void compositor_draw_glass_frame_clip(uint32_t handle, uint32_t x, uint32_t y,
                                              uint32_t w, uint32_t h,
                                              uint32_t cx0, uint32_t cy0,
                                              uint32_t cx1, uint32_t cy1) {
    uint32_t accent = window_accent(handle);
    if (accent == 0u || w < COMPOSITOR_CORNER_RADIUS * 2u ||
        h < COMPOSITOR_CORNER_RADIUS * 2u) return;

    int focused = handle == window_focused_handle();
    uint32_t intensity = focused ? 235u : 125u;
    uint32_t tint_alpha = focused ? 24u : 14u;
    uint32_t x0 = x > cx0 ? x : cx0;
    uint32_t y0 = y > cy0 ? y : cy0;
    uint32_t x1 = x + w < cx1 ? x + w : cx1;
    uint32_t y1 = y + h < cy1 ? y + h : cy1;

    for (uint32_t row = y0; row < y1; ++row) {
        uint32_t rel_y = row - y;
        uint32_t end_fade = 255u;
        if (rel_y < COMPOSITOR_CORNER_RADIUS) {
            end_fade = (rel_y * 255u) / COMPOSITOR_CORNER_RADIUS;
        } else if (rel_y >= h - COMPOSITOR_CORNER_RADIUS) {
            end_fade = ((h - 1u - rel_y) * 255u) / COMPOSITOR_CORNER_RADIUS;
        }
        for (uint32_t col = x0; col < x1; ++col) {
            uint32_t rel_x = col - x;
            uint32_t dst = framebuffer_get_pixel(col, row);
            dst = compositor_blend_rgb(COMPOSITOR_GLASS_TINT_RGB, dst, tint_alpha);

            /* Feiner Glasrahmen und oben links staerkere Reflexion. */
            if (rel_y <= 1u || rel_x >= w - 2u || rel_y >= h - 2u) {
                uint32_t edge_alpha = (rel_y <= 1u && rel_x < (w * 2u) / 3u) ? 105u : 48u;
                dst = compositor_blend_rgb(COMPOSITOR_BORDER_RGB, dst, edge_alpha);
            }

            /* Signature Edge: heller 5px-Kern, farbige Flaeche und
               exponentiell angenaeherter innerer Glow bis 18px. */
            if (rel_x < COMPOSITOR_SIGNATURE_GLOW) {
                uint32_t a;
                if (rel_x < COMPOSITOR_SIGNATURE_CORE) {
                    a = intensity;
                    if (rel_x == 1u || rel_x == 2u) a = 255u;
                } else {
                    uint32_t falloff = COMPOSITOR_SIGNATURE_GLOW - rel_x;
                    a = (intensity * falloff * falloff) /
                        ((COMPOSITOR_SIGNATURE_GLOW - COMPOSITOR_SIGNATURE_CORE) *
                         (COMPOSITOR_SIGNATURE_GLOW - COMPOSITOR_SIGNATURE_CORE));
                }
                a = (a * end_fade) / 255u;
                dst = compositor_blend_rgb(accent, dst, a > 255u ? 255u : a);
            }
            framebuffer_set_pixel(col, row, dst);
        }
    }
}

/* NPSPEC Liquid-Glass Abschnitt 10: rein geometrische Kontakterkennung
   zwischen einem Fenster und der sichtbaren, abgerundeten Taskbar-Dock-
   Flaeche (taskbarglass_dock_rect()) - beide Objekte bleiben technisch
   vollstaendig unabhaengig, dies liefert nur den ueberlappenden X-Bereich
   fuer den rein optischen Verschmelzungs-Effekt. 0 = kein Kontakt. */
static int compositor_window_touches_taskbar(uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                                              uint32_t *out_ox0, uint32_t *out_ox1) {
    uint32_t dock_x, dock_y, dock_w, dock_h;
    if (taskbarglass_dock_rect(&dock_x, &dock_y, &dock_w, &dock_h) != 0) return 0;

    uint32_t win_bottom = y + h;
    uint32_t diff = (win_bottom > dock_y) ? (win_bottom - dock_y) : (dock_y - win_bottom);
    if (diff > COMPOSITOR_DOCK_MERGE_THRESHOLD) return 0;

    uint32_t dock_x1 = dock_x + dock_w;
    uint32_t win_x1 = x + w;
    uint32_t ox0 = (x > dock_x) ? x : dock_x;
    uint32_t ox1 = (win_x1 < dock_x1) ? win_x1 : dock_x1;
    if (ox0 >= ox1) return 0;

    if (out_ox0) *out_ox0 = ox0;
    if (out_ox1) *out_ox1 = ox1;
    return 1;
}

/* Zeichnet die eigentliche optische Verschmelzung im Kontaktbereich
   [ox0,ox1): eine kurze Bruecke zwischen Fenster-Unterkante und Taskbar-
   Oberkante, die den Fenster-Akzent (dieselbe Farbe wie die Signature
   Edge, je nach Fokuszustand) weich in den darunterliegenden Taskbar-
   Glaston ueberblendet. Malt NUR auf bereits von beiden Surfaces fertig
   gezeichneten Pixeln - keine strukturelle Kopplung der beiden Objekte.
   Wird pro Fenster im Z-Order-Durchlauf aufgerufen, daher ueberschreibt
   bei ueberlappenden Kontaktzonen automatisch das zuletzt (hoeher in der
   Z-Order, also im Vordergrund) gezeichnete Fenster die Bruecke eines
   darunterliegenden, okkludierten Fensters - kein eigener Okklusions-Check
   noetig. */
static void compositor_draw_dock_merge_clip(uint32_t handle, uint32_t x, uint32_t y, uint32_t h,
                                             uint32_t ox0, uint32_t ox1, uint32_t progress,
                                             uint32_t cx0, uint32_t cy0, uint32_t cx1, uint32_t cy1) {
    if (progress == 0u) return;
    uint32_t dock_x, dock_y, dock_w, dock_h;
    if (taskbarglass_dock_rect(&dock_x, &dock_y, &dock_w, &dock_h) != 0) return;
    (void)x;
    (void)dock_x;
    (void)dock_w;
    (void)dock_h;

    uint32_t win_bottom = y + h;
    uint32_t bridge_y0 = (win_bottom < dock_y) ? win_bottom : dock_y;
    uint32_t bridge_y1_full = (win_bottom > dock_y) ? win_bottom : dock_y;
    if (bridge_y1_full <= bridge_y0) bridge_y1_full = bridge_y0 + 1u;
    /* Die Bruecke waechst waehrend des Andockens von der Fenster-/Dock-
       Kontaktlinie aus nach aussen, statt sofort ihre volle Hoehe
       anzunehmen - zusammen mit der unten skalierten Alpha ergibt das den
       geforderten weichen Uebergang. */
    uint32_t bridge_y1 = bridge_y0 + (((bridge_y1_full - bridge_y0) * progress) / 255u);
    if (bridge_y1 <= bridge_y0) bridge_y1 = bridge_y0 + 1u;

    uint32_t rx0 = ox0 > cx0 ? ox0 : cx0;
    uint32_t rx1 = ox1 < cx1 ? ox1 : cx1;
    uint32_t ry0 = bridge_y0 > cy0 ? bridge_y0 : cy0;
    uint32_t ry1 = bridge_y1 < cy1 ? bridge_y1 : cy1;
    if (rx0 >= rx1 || ry0 >= ry1) return;

    uint32_t focused = window_focused_handle();
    uint32_t accent = (handle == focused) ? COMPOSITOR_ACCENT_RGB : COMPOSITOR_ACCENT_DIM_RGB;
    uint32_t alpha = (COMPOSITOR_DOCK_MERGE_ALPHA * progress) / 255u;

    for (uint32_t row = ry0; row < ry1; ++row) {
        for (uint32_t col = rx0; col < rx1; ++col) {
            uint32_t dst = framebuffer_get_pixel(col, row);
            framebuffer_set_pixel(col, row, compositor_blend_rgb(accent, dst, alpha));
        }
    }
}

static int compositor_handle_is_window(uint32_t handle) {
    for (uint32_t i = 0; i < window_capacity(); ++i) {
        if (window_handle_at_slot(i) == handle) return 1;
    }
    return 0;
}

/* Erst rohe Surfaces ohne WM-Metadaten, danach echte Fenster strikt nach
   Z-Order. Inhalt und Dekoration werden gemeinsam gezeichnet, damit die
   Titelleiste eines unteren Fensters niemals ein oberes Fenster uebermalt. */
static void compositor_draw_surfaces_clip(uint32_t cx0, uint32_t cy0, uint32_t cx1, uint32_t cy1) {
    for (uint32_t slot = 0; slot < surface_capacity(); ++slot) {
        uint32_t handle = surface_handle_at_slot(slot);
        if (handle == 0 || compositor_handle_is_window(handle) ||
            surface_state(handle) != SURFACE_STATE_ACTIVE) continue;
        uint32_t x, y, w, h;
        if (surface_rect(handle, &x, &y, &w, &h) != 0) continue;
        uint32_t radius = taskbarglass_is_surface(handle) ? 18u : 0u;
        compositor_blit_surface_buffer_clip(x, y, w, h, surface_buffer_addr(handle),
                                            cx0, cy0, cx1, cy1, handle, radius,
                                            radius != 0u, 0u, 0u);
    }

    uint32_t ordered[WINDOW_CAPACITY];
    uint32_t count = 0;
    for (uint32_t slot = 0; slot < window_capacity(); ++slot) {
        uint32_t handle = window_handle_at_slot(slot);
        if (handle != 0 && window_is_visible(handle)) ordered[count++] = handle;
    }
    for (uint32_t i = 1; i < count; ++i) {
        uint32_t value = ordered[i];
        uint32_t j = i;
        while (j > 0u && window_z_order(ordered[j - 1u]) > window_z_order(value)) {
            ordered[j] = ordered[j - 1u];
            --j;
        }
        ordered[j] = value;
    }

    /* Mehrstufiger weicher Umgebungsschatten fuer echte Glasfenster.
       Alle Schatten werden vor den Window-Surfaces gezeichnet, damit sie
       niemals den Inhalt eines hoeher liegenden Fensters abdunkeln. */
    for (uint32_t i = 0; i < count; ++i) {
        uint32_t handle = ordered[i];
        if (window_accent(handle) == 0u || taskbarglass_is_surface(handle)) continue;
        uint32_t sx, sy, sw, sh;
        if (surface_rect(handle, &sx, &sy, &sw, &sh) != 0) continue;
        for (uint32_t layer = 4u; layer > 0u; --layer) {
            uint32_t spread = layer * 4u;
            uint32_t shadow_x = sx > spread ? sx - spread : 0u;
            uint32_t shadow_y = sy > spread ? sy - spread : 0u;
            compositor_blend_rect_clip(shadow_x, shadow_y,
                                        sw + spread * 2u, sh + spread * 2u,
                                        COMPOSITOR_SHADOW_RGB, 10u + (4u - layer) * 9u,
                                        cx0, cy0, cx1, cy1);
        }
    }

    for (uint32_t i = 0; i < count; ++i) {
        uint32_t handle = ordered[i];
        uint32_t x, y, w, h;
        if (surface_rect(handle, &x, &y, &w, &h) != 0) continue;
        int rounded = (window_is_decorated(handle) || window_accent(handle) != 0u) &&
                      window_mode(handle) != WINDOW_MODE_FULLSCREEN;
        uint32_t radius = taskbarglass_is_surface(handle) ? 18u
                                                          : (rounded ? COMPOSITOR_CORNER_RADIUS : 0u);
        uint32_t dock_ox0 = 0u, dock_ox1 = 0u;
        int docked = !taskbarglass_is_surface(handle) &&
                     compositor_window_touches_taskbar(x, y, w, h, &dock_ox0, &dock_ox1);
        uint32_t anim_ox0 = dock_ox0, anim_ox1 = dock_ox1;
        uint32_t dock_progress = taskbarglass_is_surface(handle) ? 0u
                                  : compositor_dock_anim_step(handle, docked, dock_ox0, dock_ox1,
                                                               &anim_ox0, &anim_ox1);
        dock_ox0 = anim_ox0;
        dock_ox1 = anim_ox1;
        /* Die eckige Unterkante (keine Doppel-Rundung) schaltet erst ab
           Halbzeit des Uebergangs um, statt exakt mit der binaeren
           Kontakterkennung - dadurch wirkt auch dieser Teil des Effekts
           weich statt schlagartig. */
        int square_corners = dock_progress >= 128u;
        compositor_blit_surface_buffer_clip(x, y, w, h, surface_buffer_addr(handle),
                                            cx0, cy0, cx1, cy1, handle, radius,
                                            taskbarglass_is_surface(handle),
                                            square_corners ? dock_ox0 : 0u,
                                            square_corners ? dock_ox1 : 0u);
        compositor_draw_titlebar_clip(handle, x, y, w, h, cx0, cy0, cx1, cy1);
        compositor_draw_glass_frame_clip(handle, x, y, w, h, cx0, cy0, cx1, cy1);
        if (dock_progress != 0u) {
            compositor_draw_dock_merge_clip(handle, x, y, h, dock_ox0, dock_ox1, dock_progress,
                                            cx0, cy0, cx1, cy1);
        }
    }
}

/* Zeichnet die Hintergrundregion [cx0,cx1) x [cy0,cy1): entweder die
   einfarbige Flaeche (Normalfall, unveraendertes Verhalten) oder, falls per
   compositor_set_background_image() gesetzt, das Wallpaper-Bild. Hat das
   Bild nicht die Framebuffer-Aufloesung, wird pro Zielpixel die naechst-
   gelegene Quellzeile/-spalte per Ganzzahl-Multiplikation/Division
   abgebildet (kein Blending noetig - das Bild bedeckt immer vollstaendig,
   genau wie die bisherige alpha=255 Flaechenfuellung). */
static void compositor_draw_background_clip(uint32_t cx0, uint32_t cy0, uint32_t cx1, uint32_t cy1) {
    if (compositor_background_image == 0 || compositor_background_image_w == 0 ||
        compositor_background_image_h == 0) {
        compositor_blend_rect_clip(cx0, cy0, cx1 - cx0, cy1 - cy0, compositor_background_rgb, 255u,
                                    cx0, cy0, cx1, cy1);
        return;
    }

    uint32_t fb_w = framebuffer_width();
    uint32_t fb_h = framebuffer_height();
    if (fb_w == 0 || fb_h == 0) return;

    for (uint32_t y = cy0; y < cy1; ++y) {
        uint32_t src_y = (y * compositor_background_image_h) / fb_h;
        if (src_y >= compositor_background_image_h) src_y = compositor_background_image_h - 1u;
        const uint8_t *row = compositor_background_image +
                              (uint32_t)src_y * compositor_background_image_w * 3u;
        for (uint32_t x = cx0; x < cx1; ++x) {
            uint32_t src_x = (x * compositor_background_image_w) / fb_w;
            if (src_x >= compositor_background_image_w) src_x = compositor_background_image_w - 1u;
            const uint8_t *px = row + (uint32_t)src_x * 3u;
            uint32_t rgb = ((uint32_t)px[0] << 16) | ((uint32_t)px[1] << 8) | (uint32_t)px[2];
            framebuffer_set_pixel(x, y, rgb);
        }
    }
}

/* NPSPEC-COMPOSITOR-DAMAGE-0001 Abschnitt 6 (Partial Redraw Controller):
   zeichnet Hintergrund, Schatten, Fenster, Surfaces, Dekorationen und
   Cursor neu, aber ausschliesslich innerhalb von [cx0,cx1) x [cy0,cy1) -
   fuer den Full-Redraw-Fall ist das der gesamte Bildschirm, fuer Partial
   Redraw genau eine Damage-Region. */
static void compositor_redraw_region(uint32_t cx0, uint32_t cy0, uint32_t cx1, uint32_t cy1) {
    if (cx0 >= cx1 || cy0 >= cy1) return;

    compositor_draw_background_clip(cx0, cy0, cx1, cy1);

    for (uint32_t i = 0; i < COMPOSITOR_WINDOW_CAPACITY; ++i) {
        compositor_window_t *win = &compositor_windows[i];
        if (win->id == 0 || !win->shadow) continue;
        if (compositor_window_is_occluded(i)) continue;
        compositor_blend_rect_clip(win->x + COMPOSITOR_SHADOW_OFFSET,
                                    win->y + COMPOSITOR_SHADOW_OFFSET,
                                    win->w, win->h, COMPOSITOR_SHADOW_RGB,
                                    COMPOSITOR_SHADOW_ALPHA, cx0, cy0, cx1, cy1);
    }

    for (uint32_t i = 0; i < COMPOSITOR_WINDOW_CAPACITY; ++i) {
        compositor_window_t *win = &compositor_windows[i];
        if (win->id == 0) continue;
        if (compositor_window_is_occluded(i)) {
            ++compositor_occluded_count;
            continue;
        }
        compositor_blend_rect_clip(win->x, win->y, win->w, win->h, win->rgb, win->alpha,
                                    cx0, cy0, cx1, cy1);
    }

    compositor_draw_surfaces_clip(cx0, cy0, cx1, cy1);

    if (compositor_cursor_visible) {
        compositor_blend_rect_clip(compositor_cursor_x, compositor_cursor_y, 8u, 8u,
                                    compositor_cursor_rgb, 255u, cx0, cy0, cx1, cy1);
    }
}

void compositor_composite(void) {
    if (!framebuffer_available()) return;

    /* Scene Damage Tracker: Surface-/Fokus-Aenderungen seit dem letzten
       Aufruf einsammeln, BEVOR ueber Skip/Full/Partial entschieden wird. */
    compositor_scan_surface_damage();

    if (!compositor_full_redraw_pending && compositor_damage_count == 0) {
        /* Abschnitt 8/COMPOSITOR-011: keine Aenderung -> kein Redraw. */
        compositor_last_damage_region_count_value = 0;
        compositor_last_full_redraw = 0;
        return;
    }

    uint32_t frame_start_tick = timer_get_ticks();
    compositor_occluded_count = 0;

    if (compositor_full_redraw_pending) {
        compositor_redraw_region(0u, 0u, framebuffer_width(), framebuffer_height());
        compositor_last_full_redraw = 1;
        compositor_last_damage_region_count_value = 1;
    } else {
        compositor_last_full_redraw = 0;
        compositor_last_damage_region_count_value = compositor_damage_count;
        for (uint32_t i = 0; i < compositor_damage_count; ++i) {
            compositor_damage_rect_t *r = &compositor_damage[i];
            compositor_redraw_region(r->x0, r->y0, r->x1, r->y1);
        }
    }

    compositor_last_frame_ticks_value = timer_get_ticks() - frame_start_tick;
    compositor_full_redraw_pending = 0;
    compositor_damage_count = 0;
}

uint32_t compositor_last_occluded_count(void) {
    return compositor_occluded_count;
}

uint32_t compositor_last_frame_ticks(void) {
    return compositor_last_frame_ticks_value;
}

uint32_t compositor_last_damage_region_count(void) {
    return compositor_last_damage_region_count_value;
}

int compositor_last_redraw_was_full(void) {
    return compositor_last_full_redraw;
}

/* Einmaliger Diagnose-Beweis (kein funktionaler Teil des Redraw-Loops):
   sobald irgendeine Client-Surface (egal welcher Owner-PID - nicht an den
   aktuellen Einzel-Client gebunden) erstmals ACTIVE mit angehaengtem
   Buffer wird, deren tatsaechlich im FRAMEBUFFER gelandetes Pixel loggen.
   Beweist den vollstaendigen Rundweg Client->Syscall->surface.c->
   compositor_thread_body->Framebuffer unabhaengig vom spezifischen
   Ring-3-Demoprogramm in ring3.c. */
static int compositor_client_proof_logged = 0;
static int compositor_cr3_recovery_logged = 0;

static void compositor_log_client_proof_once(void) {
    if (compositor_client_proof_logged) return;

    uint32_t capacity = surface_capacity();
    for (uint32_t slot = 0; slot < capacity; ++slot) {
        uint32_t handle = surface_handle_at_slot(slot);
        if (handle == 0) continue;
        if (surface_state(handle) != SURFACE_STATE_ACTIVE) continue;
        if (surface_buffer_addr(handle) == 0) continue;
        /* owner_pid==0 ist die Desktop Shell selbst (siehe desktop_shell.c
           SHELL_OWNER_PID) - deren Chrome-Fenster (Taskbar/Icons/
           Startmenue) entstehen bereits VOR dem echten Ring-3-Client und
           wuerden diesen einmaligen Beweis sonst vorzeitig mit dem
           falschen Fenster feuern. */
        if (surface_owner_pid(handle) == 0u) continue;

        uint32_t x, y, w, h;
        if (surface_rect(handle, &x, &y, &w, &h) != 0) continue;
        (void)w;
        (void)h;

        serial_puts("NOVA: Display-Server zeigt echten Client-Inhalt - Framebuffer-Pixel=");
        serial_put_hex32(framebuffer_get_pixel(x, y));
        serial_puts("\r\n");
        compositor_client_proof_logged = 1;
        return;
    }
}

void compositor_thread_body(void) {
    for (;;) {
        /* Ein Kernel-Renderthread darf niemals Client-Mappings an den
           niedrigen Bootstrap-Adressen sehen. Besonders der Ring-3-GUI-
           Puffer bei 0x00402000 ueberlappt im virtuellen Adressraum noch
           den grossen statischen Wallpaperpuffer des Bootstrap-Kernels.
           Der Scheduler ordnet diesem Slot zwar den Kernel-CR3 zu; diese
           Laufzeitbarriere macht den Display-Server trotzdem robust gegen
           einen veralteten/extern geaenderten CR3 (z.B. beim ersten
           Ring-3-Handoff) und verhindert, dass Clientpixel als scheinbare
           horizontale Stoerzeile in das Wallpaper gelangen. */
        uint32_t kernel_dir = paging_kernel_directory_physical();
        if (kernel_dir != 0u && paging_root_physical() != kernel_dir) {
            paging_switch_address_space(kernel_dir);
            if (!compositor_cr3_recovery_logged) {
                serial_puts("NOVA: Compositor CR3 auf Kernel-Adressraum korrigiert\r\n");
                compositor_cr3_recovery_logged = 1;
            }
        }
        uint32_t now = timer_get_ticks();
        if ((now - compositor_last_thread_tick) >= COMPOSITOR_THREAD_INTERVAL_TICKS) {
            compositor_last_thread_tick = now;
            compositor_composite();
            compositor_log_client_proof_once();
        }
        __asm__ volatile ("pause");
    }
}

int compositor_thread_start(void) {
    uint32_t tid;
    compositor_last_thread_tick = timer_get_ticks();
    if (thread_create_dynamic((uint32_t)(uintptr_t)compositor_thread_body, 1u, -1,
                               &tid) != 0) {
        return -1;
    }
    compositor_state_value = COMPOSITOR_STATE_RUNNING;
    return 0;
}

int compositor_self_test(void) {
    if (!framebuffer_available()) return -1;

    compositor_initialize();
    if (compositor_state() == COMPOSITOR_STATE_UNINITIALIZED) return -1;
    compositor_set_background(0x202020u);

    uint32_t window_id;
    if (compositor_window_create(0, 0, 4, 4, 0xFF0000u, 128u, &window_id) != 0) return -1;

    compositor_composite();

    uint32_t pixel = framebuffer_get_pixel(0, 0);
    uint8_t got_r = (uint8_t)((pixel >> 16) & 0xFFu);
    uint8_t got_g = (uint8_t)((pixel >> 8) & 0xFFu);
    uint8_t got_b = (uint8_t)(pixel & 0xFFu);

    uint8_t expect_r = compositor_blend_channel(0xFFu, 0x20u, 128u);
    uint8_t expect_g = compositor_blend_channel(0x00u, 0x20u, 128u);
    uint8_t expect_b = compositor_blend_channel(0x00u, 0x20u, 128u);

    if (got_r != expect_r || got_g != expect_g || got_b != expect_b) return -1;

    /* vollstaendig deckendes Fenster (alpha=255) muss den Hintergrund exakt
       ersetzen, keine Blend-Rundung */
    compositor_initialize();
    compositor_set_background(0x101010u);
    if (compositor_window_create(1, 1, 2, 2, 0x00FF00u, 255u, &window_id) != 0) return -1;
    compositor_composite();
    if (framebuffer_get_pixel(1, 1) != 0x00FF00u) return -1;

    /* Occlusion Culling: ein kleineres Fenster, das vollstaendig von einem
       spaeter erzeugten, voll deckenden Fenster ueberdeckt wird, darf nicht
       gezeichnet werden (muss als uebersprungen gezaehlt werden) - das
       sichtbare Endergebnis bleibt dabei unveraendert (von oben identisch
       ueberschrieben). */
    compositor_initialize();
    compositor_set_background(0x000000u);
    uint32_t hidden_id, cover_id;
    if (compositor_window_create(10u, 10u, 20u, 20u, 0xFF0000u, 255u, &hidden_id) != 0) return -1;
    if (compositor_window_create(0u, 0u, 100u, 100u, 0x0000FFu, 255u, &cover_id) != 0) return -1;
    compositor_composite();
    if (compositor_last_occluded_count() != 1u) return -1;
    if (framebuffer_get_pixel(15u, 15u) != 0x0000FFu) return -1;

    /* Teiluberdeckte Fenster (nicht VOLLSTAENDIG bedeckt) duerfen NICHT
       uebersprungen werden. */
    compositor_initialize();
    compositor_set_background(0x000000u);
    uint32_t partial_id;
    if (compositor_window_create(0u, 0u, 20u, 20u, 0xFF0000u, 255u, &partial_id) != 0) return -1;
    if (compositor_window_create(10u, 10u, 20u, 20u, 0x0000FFu, 255u, &cover_id) != 0) return -1;
    compositor_composite();
    if (compositor_last_occluded_count() != 0u) return -1;
    if (framebuffer_get_pixel(5u, 5u) != 0xFF0000u) return -1;

    /* Echter Client-Surface-Roundtrip (NPSPEC-COMPOSITOR-BUFFER-0001/
       SURFACE-0004): eine ueber surface.c angehaengte 2x2-Pixelflaeche muss
       nach compositor_composite() unveraendert (1:1, kein Blending) an
       ihrer Zielposition im Framebuffer erscheinen. */
    surface_manager_initialize();
    compositor_initialize();
    compositor_set_background(0x000000u);

    static uint32_t surface_pixels[4] = {0x112233u, 0x445566u, 0x778899u, 0xAABBCCu};
    uint32_t surf_handle;
    if (surface_create(1u, 30u, 40u, 2u, 2u, &surf_handle) != 0) return -1;
    if (surface_configure(surf_handle, 30u, 40u, 2u, 2u) != 0) return -1;
    if (surface_attach_buffer(surf_handle, (uint32_t)(uintptr_t)surface_pixels) != 0) return -1;
    if (surface_commit(surf_handle) != 0) return -1;

    compositor_composite();

    if (framebuffer_get_pixel(30u, 40u) != 0x112233u) return -1;
    if (framebuffer_get_pixel(31u, 40u) != 0x445566u) return -1;
    if (framebuffer_get_pixel(30u, 41u) != 0x778899u) return -1;
    if (framebuffer_get_pixel(31u, 41u) != 0xAABBCCu) return -1;

    /* Eine HIDDEN-Surface darf nicht mehr gezeichnet werden. */
    if (surface_hide(surf_handle) != 0) return -1;
    compositor_set_background(0x000000u);
    compositor_composite();
    if (framebuffer_get_pixel(30u, 40u) != 0x000000u) return -1;

    surface_destroy(surf_handle);

    /* Dekorationen (NPSPEC-WINDOW-DECORATION) ueber einem echten
       window.c-Fenster: Titelleiste in Normal- und Fokus-Farbe, plus
       Beweis, dass ueberhaupt Text gezeichnet wurde (mind. ein Pixel im
       Beschriftungsbereich weicht von der reinen Balkenfarbe ab). */
    surface_manager_initialize();
    window_manager_initialize();
    compositor_initialize();
    compositor_set_background(0x000000u);

    /* Breit genug, damit die Titelleiste auch die Beschriftung "W<Handle>"
       (Text beginnt bei x+4, mindestens ein Zeichen a 8px) vollstaendig
       aufnimmt - ein zu schmales Fenster wuerde den Text ausserhalb des
       sichtbaren Titelleisten-Rechtecks zeichnen (immer noch korrekt
       geclippt, aber dann vom folgenden Scan nicht erfassbar). */
    static uint32_t win_pixels[30 * 4];
    for (uint32_t i = 0; i < 30u * 4u; ++i) win_pixels[i] = 0x654321u;
    uint32_t win_handle;
    if (window_create(1u, 10u, 30u, 30u, 4u, 1, &win_handle) != 0) return -1;
    if (surface_attach_buffer(win_handle, (uint32_t)(uintptr_t)win_pixels) != 0) return -1;
    if (surface_commit(win_handle) != 0) return -1;

    compositor_composite();
    uint32_t title_pixel = framebuffer_get_pixel(25u, 11u);
    if (title_pixel == 0x000000u) return -1;

    int found_label_pixel = 0;
    for (uint32_t row = 10u; row < 10u + COMPOSITOR_TITLEBAR_HEIGHT; ++row) {
        for (uint32_t col = 10u; col < 10u + 30u; ++col) {
            if (framebuffer_get_pixel(col, row) != title_pixel) {
                found_label_pixel = 1;
            }
        }
    }
    if (!found_label_pixel) return -1;

    if (window_focus(win_handle) != 0) return -1;
    compositor_composite();
    if (framebuffer_get_pixel(25u, 11u) == title_pixel) return -1;

    window_destroy(win_handle);

    /* Frame-Timing-Diagnostik (NPSPEC-COMPOSITOR-PERF-0005): nach einem
       abgeschlossenen compositor_composite()-Aufruf muss ein Tick-Delta
       vorliegen (bei einem einzelnen, winzigen Test-Frame typischerweise
       0, da unter einem PIT-Tick - hier nur geprueft, dass kein
       unplausibel riesiger Wert zurueckkommt, z.B. durch einen Tick-
       Unterlauf bei der Subtraktion). */
    compositor_composite();
    if (compositor_last_frame_ticks() > 1000u) return -1;

    /* NPSPEC-COMPOSITOR-DAMAGE-0001: Damage Tracking und Partial Redraw.
       (a) Ohne jegliche Aenderung seit dem letzten Redraw DARF kein neuer
       Frame berechnet werden (Abschnitt 8/COMPOSITOR-011). (b) Bei einer
       isolierten Cursorbewegung MUESSEN bisherige und neue Position
       beruecksichtigt werden (Abschnitt 4), beides ueber einen Partial statt
       Full Redraw (Abschnitt 6/11) - die unveraenderte Fensterflaeche
       dazwischen bleibt dabei unberuehrt. */
    compositor_initialize();
    compositor_set_background(0x101010u);
    uint32_t damage_window_id;
    if (compositor_window_create(0u, 0u, 50u, 50u, 0x00FF00u, 255u, &damage_window_id) != 0) return -1;
    compositor_set_cursor(5u, 5u, 0xFFFFFFu);
    compositor_composite();
    if (!compositor_last_redraw_was_full()) return -1; /* erster Frame: immer voll */
    if (framebuffer_get_pixel(5u, 5u) != 0xFFFFFFu) return -1; /* Cursor ueber Fenster */

    compositor_composite(); /* keine Aenderung -> kein neuer Redraw */
    if (compositor_last_damage_region_count() != 0u) return -1;

    compositor_set_cursor(20u, 20u, 0xFFFFFFu);
    compositor_composite();
    if (compositor_last_redraw_was_full()) return -1;
    if (compositor_last_damage_region_count() == 0u) return -1;
    if (framebuffer_get_pixel(5u, 5u) != 0x00FF00u) return -1; /* alte Position wiederhergestellt */
    if (framebuffer_get_pixel(20u, 20u) != 0xFFFFFFu) return -1; /* neue Position */
    if (framebuffer_get_pixel(45u, 45u) != 0x00FF00u) return -1; /* unbeteiligt, unveraendert */

    return 0;
}
