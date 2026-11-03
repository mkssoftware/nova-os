/* NovaOS C-Kernel - Boot-Splash (siehe bootsplash.h fuer Scope). */
#include "bootsplash.h"
#include "display.h"
#include "framebuffer.h"
#include "render2d.h"
#include "bootsplash_logo.h"
#include <stdint.h>

#define BOOTSPLASH_BG_RGB       0x000000u
#define BOOTSPLASH_DOT_OFF_RGB  0x1C2230u /* dunkles Blaugrau, wie DESKTOP_BACKGROUND_RGB-Familie */
#define BOOTSPLASH_DOT_TAIL_RGB 0x2B5BA8u
#define BOOTSPLASH_DOT_MID_RGB  0x4F8EE0u
#define BOOTSPLASH_DOT_HEAD_RGB 0xBFE0FFu

#define BOOTSPLASH_DOT_COUNT    8u
#define BOOTSPLASH_DOT_RADIUS   5u
#define BOOTSPLASH_DOT_GAP      22u
#define BOOTSPLASH_LOGO_GAP     36u /* Abstand Logo-Unterkante zu Ladebalken-Mitte */
#define BOOTSPLASH_TICK_DIVISOR 3u  /* ein Lauflicht-Schritt alle N bootsplash_tick()-Aufrufe */

static int splash_active = 0;
static uint32_t splash_tick_counter = 0;
static uint32_t splash_dot_center_x[BOOTSPLASH_DOT_COUNT];
static uint32_t splash_dot_center_y = 0;

/* Welche Farbe Punkt 'index' bei Lauflicht-Schritt 'step' hat - ein
   dreiteiliger Kometenschweif (hellster Kopf bei step, zwei dunklere
   Punkte rueckwaerts) lueft kontinuierlich im Kreis (Modulo
   BOOTSPLASH_DOT_COUNT). Reine Logik, kein Framebuffer-Zugriff - separat
   von draw_dot_bar(), damit bootsplash_self_test() sie ohne Hardware
   pruefen kann. */
static uint32_t dot_color_for_index(uint32_t index, uint32_t step) {
    if (index == step) {
        return BOOTSPLASH_DOT_HEAD_RGB;
    }
    if (index == (step + BOOTSPLASH_DOT_COUNT - 1u) % BOOTSPLASH_DOT_COUNT) {
        return BOOTSPLASH_DOT_MID_RGB;
    }
    if (index == (step + BOOTSPLASH_DOT_COUNT - 2u) % BOOTSPLASH_DOT_COUNT) {
        return BOOTSPLASH_DOT_TAIL_RGB;
    }
    return BOOTSPLASH_DOT_OFF_RGB;
}

/* Einfacher, scanline-gefuellter Kreis - render2d.c bietet bewusst kein
   Kreis-Primitiv (siehe render2d.h-Scope-Hinweis "kein Vektor-Pfad-
   Tessellator"). Ganzzahlige Integer-Quadratwurzel per simpler Schleife
   statt sqrt()/Division (Radius hier immer <= BOOTSPLASH_DOT_RADIUS,
   also hoechstens wenige Iterationen - vermeidet jede libgcc-Abhaengigkeit,
   siehe svgicon.c-Lektion zu __divdi3/__chkstk_ms-Fallen im freestanding
   Build). */
static void draw_dot(render2d_surface_t *surf, uint32_t cx, uint32_t cy,
                      uint32_t radius, uint32_t rgb) {
    int32_t r = (int32_t)radius;
    for (int32_t dy = -r; dy <= r; ++dy) {
        int32_t span = r * r - dy * dy;
        if (span < 0) {
            continue;
        }
        int32_t dx = 0;
        while ((dx + 1) * (dx + 1) <= span) {
            ++dx;
        }
        int32_t row = (int32_t)cy + dy;
        if (row < 0) {
            continue;
        }
        int32_t x0 = (int32_t)cx - dx;
        if (x0 < 0) {
            x0 = 0;
        }
        render2d_fill_rect(surf, (uint32_t)x0, (uint32_t)row, (uint32_t)(dx * 2 + 1), 1u, rgb);
    }
}

static void draw_dot_bar(render2d_surface_t *surf, uint32_t step) {
    for (uint32_t i = 0; i < BOOTSPLASH_DOT_COUNT; ++i) {
        draw_dot(surf, splash_dot_center_x[i], splash_dot_center_y,
                 BOOTSPLASH_DOT_RADIUS, dot_color_for_index(i, step));
    }
}

static void init_framebuffer_surface(render2d_surface_t *surf) {
    render2d_init_surface(surf, framebuffer_pixels(), framebuffer_width(),
                           framebuffer_height(), framebuffer_stride());
}

int bootsplash_show(void) {
    if (!framebuffer_available()) {
        return -1;
    }
    uint32_t width = display_width();
    uint32_t height = display_height();
    if (width == 0u || height == 0u) {
        return -1;
    }

    render2d_surface_t fb_surf;
    init_framebuffer_surface(&fb_surf);

    render2d_fill_rect(&fb_surf, 0u, 0u, width, height, BOOTSPLASH_BG_RGB);

    /* Gesamte Komposition (Logo + Ladebalken) vertikal mittig, Logo
       zentriert ueber dem Ladebalken (siehe Mockup-Vorgabe). */
    uint32_t content_height = BOOTSPLASH_LOGO_HEIGHT + BOOTSPLASH_LOGO_GAP
                               + BOOTSPLASH_DOT_RADIUS * 2u;
    uint32_t logo_y = (height > content_height) ? (height - content_height) / 2u : 0u;
    uint32_t logo_x = (width > BOOTSPLASH_LOGO_WIDTH) ? (width - BOOTSPLASH_LOGO_WIDTH) / 2u : 0u;

    render2d_surface_t logo_surf;
    render2d_init_surface(&logo_surf, (uint32_t *)(uintptr_t)bootsplash_logo_pixels,
                           BOOTSPLASH_LOGO_WIDTH, BOOTSPLASH_LOGO_HEIGHT, BOOTSPLASH_LOGO_WIDTH);
    render2d_blit(&fb_surf, logo_x, logo_y, &logo_surf, 0u, 0u,
                  BOOTSPLASH_LOGO_WIDTH, BOOTSPLASH_LOGO_HEIGHT);

    uint32_t bar_width = (BOOTSPLASH_DOT_COUNT - 1u) * BOOTSPLASH_DOT_GAP;
    uint32_t bar_x0 = (width > bar_width) ? (width - bar_width) / 2u : 0u;
    splash_dot_center_y = logo_y + BOOTSPLASH_LOGO_HEIGHT + BOOTSPLASH_LOGO_GAP;
    for (uint32_t i = 0; i < BOOTSPLASH_DOT_COUNT; ++i) {
        splash_dot_center_x[i] = bar_x0 + i * BOOTSPLASH_DOT_GAP;
    }

    splash_tick_counter = 0u;
    splash_active = 1;
    draw_dot_bar(&fb_surf, 0u);
    return 0;
}

void bootsplash_tick(void) {
    if (!splash_active) {
        return;
    }
    ++splash_tick_counter;
    if ((splash_tick_counter % BOOTSPLASH_TICK_DIVISOR) != 0u) {
        return;
    }
    uint32_t step = (splash_tick_counter / BOOTSPLASH_TICK_DIVISOR) % BOOTSPLASH_DOT_COUNT;

    render2d_surface_t fb_surf;
    init_framebuffer_surface(&fb_surf);
    draw_dot_bar(&fb_surf, step);
}

void bootsplash_hide(void) {
    splash_active = 0;
}

int bootsplash_active(void) {
    return splash_active;
}

int bootsplash_self_test(void) {
    if (bootsplash_active() != 0) {
        return -1; /* vor dem ersten bootsplash_show() MUSS die Animation inaktiv sein */
    }
    for (uint32_t step = 0; step < BOOTSPLASH_DOT_COUNT; ++step) {
        uint32_t lit_count = 0u;
        uint32_t head_count = 0u;
        for (uint32_t i = 0; i < BOOTSPLASH_DOT_COUNT; ++i) {
            uint32_t rgb = dot_color_for_index(i, step);
            if (rgb != BOOTSPLASH_DOT_OFF_RGB) {
                ++lit_count;
            }
            if (rgb == BOOTSPLASH_DOT_HEAD_RGB) {
                ++head_count;
            }
        }
        if (lit_count != 3u || head_count != 1u) {
            return -1; /* genau 3 leuchtende Punkte, genau 1 davon der helle Kopf */
        }
        if (dot_color_for_index(step, step) != BOOTSPLASH_DOT_HEAD_RGB) {
            return -1;
        }
    }
    return 0;
}
