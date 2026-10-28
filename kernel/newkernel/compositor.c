/* NovaOS C-Kernel - Nova Compositor (siehe compositor.h fuer Scope). */
#include "compositor.h"
#include "framebuffer.h"
#include "surface.h"
#include "window.h"
#include "render2d.h"
#include "threadmgr.h"
#include "interrupt.h"
#include "serial.h"

/* Mindestabstand zwischen zwei automatischen Redraws des Compositor-
   Threads, in echten Timer-Ticks (keine VSync/VRR-Quelle vorhanden, siehe
   NPSPEC-COMPOSITOR-RENDER-0005/0006 - BLOCKED). ~5 Ticks bei 100Hz PIT
   entspricht ~50ms (~20 FPS) - ausreichend fuer sichtbare Aktualisierung,
   ohne jeden Scheduler-Durchlauf unnoetig einen vollen Frame neu zu
   zeichnen. */
#define COMPOSITOR_THREAD_INTERVAL_TICKS 5u

#define COMPOSITOR_SHADOW_OFFSET 6u
#define COMPOSITOR_SHADOW_RGB    0x000000u
#define COMPOSITOR_SHADOW_ALPHA  80u

/* NPSPEC-WINDOW-DECORATION-0001..0003 (Rahmen/Titelleiste/Controls),
   reduziert auf eine feste Titelleisten-Hoehe + Fuellfarbe (kein Theme-
   System, keine Minimize/Maximize/Close-Buttons - die bräuchten Klick-
   Erkennung, die ohne Input-Subsystem (siehe window.h) ohnehin wirkungslos
   waere). Nur window.c-Fenster mit decorated!=0 bekommen eine Titelleiste;
   compositor_windows (Desktop-Chrome wie die Taskbar) bleiben unberuehrt. */
#define COMPOSITOR_TITLEBAR_HEIGHT     20u
#define COMPOSITOR_TITLEBAR_RGB        0x30343Cu
#define COMPOSITOR_TITLEBAR_FOCUS_RGB  0x2D5FA0u

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
                                                 uint32_t cx0, uint32_t cy0, uint32_t cx1, uint32_t cy1) {
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
    for (uint32_t row = y0; row < y_end; ++row) {
        const uint32_t *src_row = pixels + (uint32_t)(row - y) * w;
        for (uint32_t col = x0; col < x_end; ++col) {
            framebuffer_set_pixel(col, row, src_row[col - x]);
        }
    }
}

/* NPSPEC-COMPOSITOR-DAMAGE-0001 Abschnitt 2 ("Surface Damage Tracker"/
   "Scene Damage Tracker"): konsumiert einmal pro compositor_composite()-
   Aufruf das pro-Surface-Damage aus surface.c (SURFACE-0005,
   surface_damage_rect/surface_clear_damage) und speist es in die
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
            if (surface_damage_rect(handle, &dx, &dy, &dw, &dh) == 0) {
                compositor_damage_add(dx, dy, dw, dh);
                int decorated = window_is_decorated(handle) && window_is_visible(handle);
                if (decorated && y >= COMPOSITOR_TITLEBAR_HEIGHT) {
                    compositor_damage_add(x, y - COMPOSITOR_TITLEBAR_HEIGHT, w, COMPOSITOR_TITLEBAR_HEIGHT);
                }
                surface_clear_damage(handle);
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
static void compositor_draw_surfaces_clip(uint32_t cx0, uint32_t cy0, uint32_t cx1, uint32_t cy1) {
    uint32_t capacity = surface_capacity();
    for (uint32_t slot = 0; slot < capacity; ++slot) {
        uint32_t handle = surface_handle_at_slot(slot);
        if (handle == 0) continue;
        if (surface_state(handle) != SURFACE_STATE_ACTIVE) continue;

        uint32_t buffer_addr = surface_buffer_addr(handle);
        if (buffer_addr == 0) continue;

        uint32_t x, y, w, h;
        if (surface_rect(handle, &x, &y, &w, &h) != 0) continue;

        compositor_blit_surface_buffer_clip(x, y, w, h, buffer_addr, cx0, cy0, cx1, cy1);
    }
}

static void compositor_uint_to_str(uint32_t value, char *out, uint32_t out_cap) {
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

/* Zeichnet eine feste Titelleiste (+ "W<Handle>"-Beschriftung) oberhalb
   jedes sichtbaren, dekorierten window.c-Fensters, direkt in den
   Framebuffer (nach den Surfaces, vor dem Cursor) - siehe render2d.h fuer
   den Pixelpuffer-Adapter. Fokussierte Fenster bekommen eine andere
   Fuellfarbe (NPSPEC-WINDOW-MANAGEMENT-0006 Datenmodell-Teil: reiner
   visueller Hinweis, keine Eingabe-gesteuerte Fokus-Aenderung moeglich,
   siehe window.h). */
static void compositor_draw_decorations_clip(uint32_t cx0, uint32_t cy0, uint32_t cx1, uint32_t cy1) {
    if (!framebuffer_available()) return;

    render2d_surface_t fb_surf;
    render2d_init_surface(&fb_surf, framebuffer_pixels(), framebuffer_width(),
                           framebuffer_height(), framebuffer_stride());

    uint32_t focused = window_focused_handle();
    uint32_t capacity = window_capacity();
    for (uint32_t slot = 0; slot < capacity; ++slot) {
        uint32_t handle = window_handle_at_slot(slot);
        if (handle == 0) continue;
        if (!window_is_decorated(handle)) continue;
        if (!window_is_visible(handle)) continue;

        uint32_t x, y, w, h;
        if (surface_rect(handle, &x, &y, &w, &h) != 0) continue;
        if (y < COMPOSITOR_TITLEBAR_HEIGHT) continue; /* kein Platz oberhalb */

        uint32_t bar_y = y - COMPOSITOR_TITLEBAR_HEIGHT;
        /* Beruehrt die Titelleiste die aktuelle Redraw-Region ueberhaupt?
           Geringfuegiges Overdraw der ganzen Leiste bei Teiltreffern ist
           nach Abschnitt 5 zulaessig - so bleibt render2d.h unveraendert. */
        if (x >= cx1 || x + w <= cx0 || bar_y >= cy1 || bar_y + COMPOSITOR_TITLEBAR_HEIGHT <= cy0) {
            continue;
        }

        uint32_t bar_rgb = (handle == focused) ? COMPOSITOR_TITLEBAR_FOCUS_RGB
                                                : COMPOSITOR_TITLEBAR_RGB;
        render2d_fill_rect(&fb_surf, x, bar_y, w, COMPOSITOR_TITLEBAR_HEIGHT, bar_rgb);

        char label[16] = "W";
        compositor_uint_to_str(handle, &label[1], sizeof(label) - 1u);
        render2d_draw_text(&fb_surf, x + 4u, bar_y + 2u, label, 0xFFFFFFu, bar_rgb, 0);
    }
}

/* NPSPEC-COMPOSITOR-DAMAGE-0001 Abschnitt 6 (Partial Redraw Controller):
   zeichnet Hintergrund, Schatten, Fenster, Surfaces, Dekorationen und
   Cursor neu, aber ausschliesslich innerhalb von [cx0,cx1) x [cy0,cy1) -
   fuer den Full-Redraw-Fall ist das der gesamte Bildschirm, fuer Partial
   Redraw genau eine Damage-Region. */
static void compositor_redraw_region(uint32_t cx0, uint32_t cy0, uint32_t cx1, uint32_t cy1) {
    if (cx0 >= cx1 || cy0 >= cy1) return;

    compositor_blend_rect_clip(cx0, cy0, cx1 - cx0, cy1 - cy0, compositor_background_rgb, 255u,
                                cx0, cy0, cx1, cy1);

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
    compositor_draw_decorations_clip(cx0, cy0, cx1, cy1);

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

static void compositor_log_client_proof_once(void) {
    if (compositor_client_proof_logged) return;

    uint32_t capacity = surface_capacity();
    for (uint32_t slot = 0; slot < capacity; ++slot) {
        uint32_t handle = surface_handle_at_slot(slot);
        if (handle == 0) continue;
        if (surface_state(handle) != SURFACE_STATE_ACTIVE) continue;
        if (surface_buffer_addr(handle) == 0) continue;

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
    if (framebuffer_get_pixel(10u, 10u) != COMPOSITOR_TITLEBAR_RGB) return -1;

    int found_label_pixel = 0;
    for (uint32_t row = 10u; row < 10u + COMPOSITOR_TITLEBAR_HEIGHT; ++row) {
        for (uint32_t col = 10u; col < 10u + 30u; ++col) {
            if (framebuffer_get_pixel(col, row) != COMPOSITOR_TITLEBAR_RGB) {
                found_label_pixel = 1;
            }
        }
    }
    if (!found_label_pixel) return -1;

    if (window_focus(win_handle) != 0) return -1;
    compositor_composite();
    if (framebuffer_get_pixel(10u, 10u) != COMPOSITOR_TITLEBAR_FOCUS_RGB) return -1;

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
