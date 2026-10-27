/* NovaOS C-Kernel - Nova 2D Render-Grundbibliothek (siehe render2d.h fuer
   Scope). */
#include "render2d.h"
#include "font.h"

void render2d_init_surface(render2d_surface_t *surf, uint32_t *pixels,
                            uint32_t width, uint32_t height, uint32_t stride) {
    surf->pixels = pixels;
    surf->width = width;
    surf->height = height;
    surf->stride = stride;
}

static inline void render2d_put(render2d_surface_t *surf, uint32_t x, uint32_t y,
                                 uint32_t rgb) {
    if (x >= surf->width || y >= surf->height) return;
    surf->pixels[(uint32_t)y * surf->stride + x] = rgb;
}

static inline uint32_t render2d_get(const render2d_surface_t *surf, uint32_t x,
                                     uint32_t y) {
    if (x >= surf->width || y >= surf->height) return 0;
    return surf->pixels[(uint32_t)y * surf->stride + x];
}

void render2d_fill_rect(render2d_surface_t *surf, uint32_t x, uint32_t y,
                         uint32_t w, uint32_t h, uint32_t rgb) {
    uint32_t x_end = x + w;
    uint32_t y_end = y + h;
    if (x_end > surf->width) x_end = surf->width;
    if (y_end > surf->height) y_end = surf->height;

    for (uint32_t row = y; row < y_end; ++row) {
        for (uint32_t col = x; col < x_end; ++col) {
            render2d_put(surf, col, row, rgb);
        }
    }
}

void render2d_draw_hline(render2d_surface_t *surf, uint32_t x, uint32_t y,
                          uint32_t length, uint32_t rgb) {
    render2d_fill_rect(surf, x, y, length, 1u, rgb);
}

void render2d_draw_vline(render2d_surface_t *surf, uint32_t x, uint32_t y,
                          uint32_t length, uint32_t rgb) {
    render2d_fill_rect(surf, x, y, 1u, length, rgb);
}

void render2d_draw_rect_outline(render2d_surface_t *surf, uint32_t x, uint32_t y,
                                 uint32_t w, uint32_t h, uint32_t rgb) {
    if (w == 0 || h == 0) return;
    render2d_draw_hline(surf, x, y, w, rgb);
    render2d_draw_hline(surf, x, y + h - 1u, w, rgb);
    render2d_draw_vline(surf, x, y, h, rgb);
    render2d_draw_vline(surf, x + w - 1u, y, h, rgb);
}

void render2d_blit(render2d_surface_t *dst, uint32_t dst_x, uint32_t dst_y,
                    const render2d_surface_t *src, uint32_t src_x, uint32_t src_y,
                    uint32_t w, uint32_t h) {
    for (uint32_t row = 0; row < h; ++row) {
        for (uint32_t col = 0; col < w; ++col) {
            uint32_t sx = src_x + col;
            uint32_t sy = src_y + row;
            if (sx >= src->width || sy >= src->height) continue;
            render2d_put(dst, dst_x + col, dst_y + row,
                         render2d_get(src, sx, sy));
        }
    }
}

/* Zeichnet ein einzelnes 8x16-Bitmap-Glyph (siehe font.h: 16 Bytes, ein
   Byte pro Zeile, Bit 7 = linkestes Pixel - MSB-first, uebliche Konvention
   fuer eingebettete Bitmap-Fonts). */
static void render2d_draw_glyph(render2d_surface_t *surf, uint32_t x, uint32_t y,
                                  const uint8_t *glyph, uint32_t fg_rgb,
                                  uint32_t bg_rgb, int draw_bg) {
    for (uint32_t row = 0; row < FONT_SYS_GLYPH_H; ++row) {
        uint8_t bits = glyph[row];
        for (uint32_t col = 0; col < FONT_SYS_GLYPH_W; ++col) {
            int set = (bits & (0x80u >> col)) != 0;
            if (set) {
                render2d_put(surf, x + col, y + row, fg_rgb);
            } else if (draw_bg) {
                render2d_put(surf, x + col, y + row, bg_rgb);
            }
        }
    }
}

uint32_t render2d_draw_text(render2d_surface_t *surf, uint32_t x, uint32_t y,
                             const char *text, uint32_t fg_rgb, uint32_t bg_rgb,
                             int draw_bg) {
    uint32_t cursor_x = x;
    for (uint32_t i = 0; text[i] != '\0'; ++i) {
        const uint8_t *glyph;
        font_get_glyph((uint8_t)text[i], 0u, &glyph);
        render2d_draw_glyph(surf, cursor_x, y, glyph, fg_rgb, bg_rgb, draw_bg);
        cursor_x += FONT_SYS_GLYPH_W;
    }
    return cursor_x - x;
}

int render2d_self_test(void) {
    static uint32_t test_buf[16 * 16];
    render2d_surface_t surf;
    render2d_init_surface(&surf, test_buf, 16u, 16u, 16u);

    for (uint32_t i = 0; i < 16u * 16u; ++i) test_buf[i] = 0x000000u;

    render2d_fill_rect(&surf, 2u, 2u, 4u, 3u, 0xFF0000u);
    if (render2d_get(&surf, 2u, 2u) != 0xFF0000u) return -1;
    if (render2d_get(&surf, 5u, 4u) != 0xFF0000u) return -1;
    if (render2d_get(&surf, 6u, 2u) != 0x000000u) return -1; /* ausserhalb des Rects */
    if (render2d_get(&surf, 2u, 5u) != 0x000000u) return -1;

    /* Clipping: ein Rect, das teilweise ausserhalb der Surface liegt, darf
       nicht ausserhalb schreiben (kein Crash/Wraparound) und muss den
       sichtbaren Teil trotzdem korrekt zeichnen. */
    render2d_fill_rect(&surf, 14u, 14u, 10u, 10u, 0x00FF00u);
    if (render2d_get(&surf, 15u, 15u) != 0x00FF00u) return -1;

    for (uint32_t i = 0; i < 16u * 16u; ++i) test_buf[i] = 0x000000u;
    render2d_draw_rect_outline(&surf, 1u, 1u, 5u, 4u, 0x0000FFu);
    if (render2d_get(&surf, 1u, 1u) != 0x0000FFu) return -1; /* Ecke */
    if (render2d_get(&surf, 5u, 1u) != 0x0000FFu) return -1; /* rechte obere Ecke */
    if (render2d_get(&surf, 1u, 4u) != 0x0000FFu) return -1; /* linke untere Ecke */
    if (render2d_get(&surf, 5u, 4u) != 0x0000FFu) return -1; /* rechte untere Ecke */
    if (render2d_get(&surf, 3u, 2u) != 0x000000u) return -1; /* Innenraum bleibt leer */

    /* Blit: Quelle und Ziel unterschiedlich gross, Kopie muss 1:1 landen. */
    static uint32_t src_buf[4] = {0x111111u, 0x222222u, 0x333333u, 0x444444u};
    render2d_surface_t src_surf;
    render2d_init_surface(&src_surf, src_buf, 2u, 2u, 2u);
    for (uint32_t i = 0; i < 16u * 16u; ++i) test_buf[i] = 0x000000u;
    render2d_blit(&surf, 10u, 10u, &src_surf, 0u, 0u, 2u, 2u);
    if (render2d_get(&surf, 10u, 10u) != 0x111111u) return -1;
    if (render2d_get(&surf, 11u, 10u) != 0x222222u) return -1;
    if (render2d_get(&surf, 10u, 11u) != 0x333333u) return -1;
    if (render2d_get(&surf, 11u, 11u) != 0x444444u) return -1;

    /* Text: ein Zeichen zeichnen und gegen die tatsaechlichen font.c-
       Bitmap-Daten verifizieren (bit-exakt, unabhaengig vom visuellen
       Erscheinungsbild). */
    static uint32_t text_buf[8 * 16];
    render2d_surface_t text_surf;
    render2d_init_surface(&text_surf, text_buf, 8u, 16u, 8u);
    for (uint32_t i = 0; i < 8u * 16u; ++i) text_buf[i] = 0x000000u;

    uint32_t width = render2d_draw_text(&text_surf, 0u, 0u, "A", 0xFFFFFFu, 0x000000u, 0);
    if (width != FONT_SYS_GLYPH_W) return -1;

    const uint8_t *expect_glyph;
    font_get_glyph((uint8_t)'A', 0u, &expect_glyph);
    for (uint32_t row = 0; row < FONT_SYS_GLYPH_H; ++row) {
        uint8_t bits = expect_glyph[row];
        for (uint32_t col = 0; col < FONT_SYS_GLYPH_W; ++col) {
            int expect_set = (bits & (0x80u >> col)) != 0;
            uint32_t got = render2d_get(&text_surf, col, row);
            if (expect_set && got != 0xFFFFFFu) return -1;
            if (!expect_set && got != 0x000000u) return -1;
        }
    }

    return 0;
}
