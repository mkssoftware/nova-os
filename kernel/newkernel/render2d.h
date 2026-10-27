/* NovaOS C-Kernel - Nova 2D Render-Grundbibliothek (Port aus
   docs/NPSPEC/sysarchitecture/049-GUI/004-Render_Engine/
   NPSPEC-GRAPHICS-2D-0002.md (Primitive Drawing), -0004.md (Image
   Rendering), -0005.md (Clipping/Masking), NPSPEC-GRAPHICS-TEXT-0001.md
   (Text Rendering Engine) und -0002/-0003.md (Font Management/
   Rasterization, bereits grossteils durch font.c abgedeckt).

   WICHTIG (Scope dieser Sitzung): operiert NICHT direkt auf dem
   Framebuffer, sondern auf einem generischen Pixelpuffer-Deskriptor
   (render2d_surface_t) - dieselben Funktionen zeichnen damit sowohl in
   framebuffer.c's Backbuffer (ueber einen duennen Adapter) als auch direkt
   in eine von surface.c verwaltete Client-Pufferseite. Reine Software-
   Rasterung, nur achsenparallele Primitive (Rect/H-Linie/V-Linie/Blit),
   kein Vektor-Pfad-Tessellator (GRAPHICS-2D-0003), keine Transformations-
   Matrix (GRAPHICS-2D-0006), kein Anti-Aliasing (GRAPHICS-2D-0007), keine
   Gradients (GRAPHICS-2D-0008) - fuer Bitmap-Text/Fensterdekorationen ohne
   Rotation/Skalierung nicht erforderlich. Text-Rendering nutzt direkt
   font.c's bereits vorrasterisierte 8x16-Bitmap-Glyphen (kein Hinting/
   Shaping/Unicode - nur ASCII 0x20..0x7E, siehe font.h). */
#ifndef NOVA_RENDER2D_H
#define NOVA_RENDER2D_H

#include <stdint.h>

/* Generischer Pixelpuffer: pixels zeigt auf width*height (falls stride==
   width) oder height Zeilen zu je stride aufeinanderfolgenden uint32_t
   0xRRGGBB-Pixeln. stride erlaubt Padding (z.B. wenn der Puffer breiter
   angelegt ist als der sichtbare Bereich) - fuer die aktuellen Aufrufer
   (Framebuffer-Adapter, surface.c-Client-Puffer) ist stride == width. */
typedef struct {
    uint32_t *pixels;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
} render2d_surface_t;

void render2d_init_surface(render2d_surface_t *surf, uint32_t *pixels,
                            uint32_t width, uint32_t height, uint32_t stride);

/* Alle Zeichenoperationen clippen stillschweigend (AABB) an die Grenzen
   von surf - kein Fehlerrueckgabewert nötig, eine teilweise ausserhalb
   liegende Operation zeichnet einfach nur den sichtbaren Teil. */
void render2d_fill_rect(render2d_surface_t *surf, uint32_t x, uint32_t y,
                         uint32_t w, uint32_t h, uint32_t rgb);
void render2d_draw_hline(render2d_surface_t *surf, uint32_t x, uint32_t y,
                          uint32_t length, uint32_t rgb);
void render2d_draw_vline(render2d_surface_t *surf, uint32_t x, uint32_t y,
                          uint32_t length, uint32_t rgb);
/* Rechteck-Umriss (vier Linien) - Grundlage fuer Fenster-Rahmen/-Border. */
void render2d_draw_rect_outline(render2d_surface_t *surf, uint32_t x, uint32_t y,
                                 uint32_t w, uint32_t h, uint32_t rgb);

/* Kopiert ein w*h-Rechteck von src nach dst (1:1, kein Blending/Skalieren -
   NPSPEC-GRAPHICS-2D-0004 reduziert auf reines Pixel-Blit ohne Bild-
   Decoder, da keine Bilddateien/NovaFS existieren). Quelle und Ziel
   duerfen unterschiedliche Groessen/Strides haben; beide Seiten werden
   unabhaengig geclippt. */
void render2d_blit(render2d_surface_t *dst, uint32_t dst_x, uint32_t dst_y,
                    const render2d_surface_t *src, uint32_t src_x, uint32_t src_y,
                    uint32_t w, uint32_t h);

/* Zeichnet einen nullterminierten ASCII-String (8x16-Bitmap-Glyphen aus
   font.c, Zeichen ausserhalb 0x20..0x7E werden als Fallback-Glyph
   (Leerzeichen) dargestellt - siehe font_get_glyph). bg_rgb faerbt die
   Hintergrund-Pixel jedes Glyphen; draw_bg==0 laesst sie unberuehrt
   (transparenter Text, z.B. direkt auf farbigem Fenstergrund). Liefert die
   Breite in Pixeln, die der String eingenommen hat (fuer Layout). */
uint32_t render2d_draw_text(render2d_surface_t *surf, uint32_t x, uint32_t y,
                             const char *text, uint32_t fg_rgb, uint32_t bg_rgb,
                             int draw_bg);

int render2d_self_test(void); /* 0 = ok */

#endif
