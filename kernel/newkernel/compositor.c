/* NovaOS C-Kernel - Nova Compositor (siehe compositor.h fuer Scope). */
#include "compositor.h"
#include "framebuffer.h"

#define COMPOSITOR_SHADOW_OFFSET 6u
#define COMPOSITOR_SHADOW_RGB    0x000000u
#define COMPOSITOR_SHADOW_ALPHA  80u

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

static uint8_t compositor_blend_channel(uint8_t src, uint8_t dst, uint32_t alpha) {
    /* Porter-Duff Source-Over (NPSPEC-COMPOSITOR-EFFECTS-0001, Abschnitt 6),
       hier mit straight statt premultiplied Alpha, da jede Layer hier nur
       eine einzelne Flaechenfarbe ohne eigenen Alphakanal pro Pixel ist. */
    uint32_t blended = (uint32_t)src * alpha + (uint32_t)dst * (255u - alpha);
    return (uint8_t)((blended + 127u) / 255u);
}

static void compositor_blend_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                                   uint32_t rgb, uint32_t alpha) {
    if (alpha == 0) return;

    uint32_t fb_w = framebuffer_width();
    uint32_t fb_h = framebuffer_height();
    uint32_t x_end = x + w;
    uint32_t y_end = y + h;
    if (x_end > fb_w) x_end = fb_w;
    if (y_end > fb_h) y_end = fb_h;

    uint8_t src_r = (uint8_t)((rgb >> 16) & 0xFFu);
    uint8_t src_g = (uint8_t)((rgb >> 8) & 0xFFu);
    uint8_t src_b = (uint8_t)(rgb & 0xFFu);

    for (uint32_t row = y; row < y_end; ++row) {
        for (uint32_t col = x; col < x_end; ++col) {
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
}

void compositor_set_background(uint32_t rgb) {
    compositor_background_rgb = rgb;
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
            return 0;
        }
    }
    return -1;
}

int compositor_window_set_shadow(uint32_t window_id, int enabled) {
    for (uint32_t i = 0; i < COMPOSITOR_WINDOW_CAPACITY; ++i) {
        if (compositor_windows[i].id == window_id) {
            compositor_windows[i].shadow = enabled ? 1 : 0;
            return 0;
        }
    }
    return -1;
}

void compositor_set_cursor(uint32_t x, uint32_t y, uint32_t rgb) {
    compositor_cursor_x = x;
    compositor_cursor_y = y;
    compositor_cursor_rgb = rgb;
    compositor_cursor_visible = 1;
}

void compositor_composite(void) {
    if (!framebuffer_available()) return;

    framebuffer_clear(compositor_background_rgb);

    for (uint32_t i = 0; i < COMPOSITOR_WINDOW_CAPACITY; ++i) {
        compositor_window_t *win = &compositor_windows[i];
        if (win->id == 0) continue;
        if (win->shadow) {
            compositor_blend_rect(win->x + COMPOSITOR_SHADOW_OFFSET,
                                   win->y + COMPOSITOR_SHADOW_OFFSET,
                                   win->w, win->h, COMPOSITOR_SHADOW_RGB,
                                   COMPOSITOR_SHADOW_ALPHA);
        }
    }

    for (uint32_t i = 0; i < COMPOSITOR_WINDOW_CAPACITY; ++i) {
        compositor_window_t *win = &compositor_windows[i];
        if (win->id == 0) continue;
        compositor_blend_rect(win->x, win->y, win->w, win->h, win->rgb, win->alpha);
    }

    if (compositor_cursor_visible) {
        compositor_blend_rect(compositor_cursor_x, compositor_cursor_y, 8u, 8u,
                               compositor_cursor_rgb, 255u);
    }
}

int compositor_self_test(void) {
    if (!framebuffer_available()) return -1;

    compositor_initialize();
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

    return 0;
}
