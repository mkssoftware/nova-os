/* NovaOS C-Kernel - Nova Material Engine: Optical-Effects-Pipeline (siehe
   matfx.h fuer Scope). */
#include "matfx.h"

#define MATFX_BLUR_MAX_RADIUS 4u

/* Feste Integer-Wellentabelle statt sin() (kein libm in diesem
   freistehenden Kernel) - Werte in -4..4, 16 Eintraege pro Periode. */
static const int SIN_TABLE[16] = {
    0, 2, 3, 4, 4, 4, 3, 2, 0, -2, -3, -4, -4, -4, -3, -2
};

static inline uint32_t clamp_u32(uint32_t v, uint32_t max) { return v > max ? max : v; }

static inline void matfx_put(render2d_surface_t *surf, uint32_t x, uint32_t y, uint32_t rgb) {
    if (x >= surf->width || y >= surf->height) return;
    surf->pixels[(uint32_t)y * surf->stride + x] = rgb;
}

static inline uint32_t matfx_get(const render2d_surface_t *surf, uint32_t x, uint32_t y) {
    if (!surf || x >= surf->width || y >= surf->height) return 0;
    return surf->pixels[(uint32_t)y * surf->stride + x];
}

static inline uint8_t blend_channel(uint8_t src, uint8_t dst, uint32_t alpha) {
    uint32_t blended = (uint32_t)src * alpha + (uint32_t)dst * (255u - alpha);
    return (uint8_t)(blended / 255u);
}

static uint32_t rgb_blend(uint32_t src, uint32_t dst, uint32_t alpha) {
    if (alpha >= 255u) return src;
    if (alpha == 0u) return dst;
    uint8_t sr = (uint8_t)(src >> 16), sg = (uint8_t)(src >> 8), sb = (uint8_t)src;
    uint8_t dr = (uint8_t)(dst >> 16), dg = (uint8_t)(dst >> 8), db = (uint8_t)dst;
    uint32_t r = blend_channel(sr, dr, alpha);
    uint32_t g = blend_channel(sg, dg, alpha);
    uint32_t b = blend_channel(sb, db, alpha);
    return (r << 16) | (g << 8) | b;
}

static uint32_t rgb_multiply(uint32_t a, uint32_t b) {
    uint32_t ar = (uint32_t)(uint8_t)(a >> 16), ag = (uint32_t)(uint8_t)(a >> 8), ab = (uint32_t)(uint8_t)a;
    uint32_t br = (uint32_t)(uint8_t)(b >> 16), bg = (uint32_t)(uint8_t)(b >> 8), bb = (uint32_t)(uint8_t)b;
    uint32_t r = (ar * br) / 255u, g = (ag * bg) / 255u, bl = (ab * bb) / 255u;
    return (r << 16) | (g << 8) | bl;
}

static uint32_t rgb_screen(uint32_t a, uint32_t b) {
    uint32_t ar = (uint32_t)(uint8_t)(a >> 16), ag = (uint32_t)(uint8_t)(a >> 8), ab = (uint32_t)(uint8_t)a;
    uint32_t br = (uint32_t)(uint8_t)(b >> 16), bg = (uint32_t)(uint8_t)(b >> 8), bb = (uint32_t)(uint8_t)b;
    uint32_t r = 255u - (((255u - ar) * (255u - br)) / 255u);
    uint32_t g = 255u - (((255u - ag) * (255u - bg)) / 255u);
    uint32_t bl = 255u - (((255u - ab) * (255u - bb)) / 255u);
    return (r << 16) | (g << 8) | bl;
}

static uint32_t rgb_scale(uint32_t rgb, uint32_t factor_0_255) {
    uint32_t r = ((uint32_t)(uint8_t)(rgb >> 16) * factor_0_255) / 255u;
    uint32_t g = ((uint32_t)(uint8_t)(rgb >> 8) * factor_0_255) / 255u;
    uint32_t b = ((uint32_t)(uint8_t)rgb * factor_0_255) / 255u;
    return (r << 16) | (g << 8) | b;
}

static uint32_t rgb_add_clamped(uint32_t rgb, uint32_t amount_0_255) {
    uint32_t r = clamp_u32((uint32_t)(uint8_t)(rgb >> 16) + amount_0_255, 255u);
    uint32_t g = clamp_u32((uint32_t)(uint8_t)(rgb >> 8) + amount_0_255, 255u);
    uint32_t b = clamp_u32((uint32_t)(uint8_t)rgb + amount_0_255, 255u);
    return (r << 16) | (g << 8) | b;
}

/* Box-Blur (BLUR-0001): Mittelwert ueber ein (2*radius+1)^2-Fenster um
   (sx,sy) im backdrop, radius intern auf MATFX_BLUR_MAX_RADIUS begrenzt. */
static uint32_t box_blur_sample(const render2d_surface_t *backdrop, uint32_t sx, uint32_t sy,
                                  uint32_t radius) {
    if (radius > MATFX_BLUR_MAX_RADIUS) radius = MATFX_BLUR_MAX_RADIUS;
    if (radius == 0u) return matfx_get(backdrop, sx, sy);

    uint32_t sum_r = 0, sum_g = 0, sum_b = 0, count = 0;
    for (int32_t dy = -(int32_t)radius; dy <= (int32_t)radius; ++dy) {
        for (int32_t dx = -(int32_t)radius; dx <= (int32_t)radius; ++dx) {
            int32_t px = (int32_t)sx + dx;
            int32_t py = (int32_t)sy + dy;
            if (px < 0 || py < 0) continue;
            uint32_t rgb = matfx_get(backdrop, (uint32_t)px, (uint32_t)py);
            sum_r += (uint8_t)(rgb >> 16);
            sum_g += (uint8_t)(rgb >> 8);
            sum_b += (uint8_t)rgb;
            ++count;
        }
    }
    if (count == 0u) return 0u;
    return ((sum_r / count) << 16) | ((sum_g / count) << 8) | (sum_b / count);
}

/* Distanz zur naechsten Kante des w*h-Materialbereichs (fuer EdgeIntensity
   aus REFRACTION-0001/CRYSTAL-0001: "Materialkanten koennen staerkere
   Brechung als Materialmitte aufweisen"). */
static uint32_t edge_distance(uint32_t lx, uint32_t ly, uint32_t w, uint32_t h) {
    uint32_t dx = (lx < w - lx) ? lx : (w - 1u - lx);
    uint32_t dy = (ly < h - ly) ? ly : (h - 1u - ly);
    return (dx < dy) ? dx : dy;
}

void matfx_apply(render2d_surface_t *dst, uint32_t x, uint32_t y,
                  uint32_t w, uint32_t h,
                  const render2d_surface_t *backdrop, uint32_t bx, uint32_t by,
                  const material_properties_t *props, uint32_t blend_mode) {
    if (!dst || !props || w == 0u || h == 0u) return;

    if (!backdrop) {
        /* SOLID-0001: keine Hintergrundabtastung - Flaeche direkt fuellen. */
        uint32_t color = props->tint_strength > 0u
            ? rgb_blend(props->tint_rgb, props->color_rgb, props->tint_strength)
            : props->color_rgb;
        for (uint32_t ly = 0; ly < h; ++ly)
            for (uint32_t lx = 0; lx < w; ++lx)
                matfx_put(dst, x + lx, y + ly, color);
        return;
    }

    uint32_t edge_boost = (props->ext_present_mask & (1u << MATERIAL_EXT_EDGE_INTENSITY))
        ? props->ext[MATERIAL_EXT_EDGE_INTENSITY] : 0u;

    for (uint32_t ly = 0; ly < h; ++ly) {
        for (uint32_t lx = 0; lx < w; ++lx) {
            uint32_t edist = edge_distance(lx, ly, w, h);
            uint32_t edge_factor = (edge_boost > 0u && edist < 8u)
                ? 255u + ((edge_boost * (8u - edist)) / 8u) : 255u;

            /* 1) Distortion/Refraction: Verschiebungsvektor aus der
               Wellentabelle, an den Kanten verstaerkt. */
            int32_t disp_x = (SIN_TABLE[ly & 15u] * (int32_t)props->distortion) / 255;
            int32_t disp_y = (SIN_TABLE[lx & 15u] * (int32_t)props->refraction) / 255;
            disp_x = (disp_x * (int32_t)edge_factor) / 255;
            disp_y = (disp_y * (int32_t)edge_factor) / 255;

            int32_t sample_x = (int32_t)(bx + lx) + disp_x;
            int32_t sample_y = (int32_t)(by + ly) + disp_y;
            if (sample_x < 0) sample_x = 0;
            if (sample_y < 0) sample_y = 0;

            /* 2) Blur ueber der verschobenen Abtastposition. */
            uint32_t sampled = box_blur_sample(backdrop, (uint32_t)sample_x, (uint32_t)sample_y,
                                                 props->blur_radius);

            /* 3) Tint + Transparency: Grundfarbe/Tint ueber das Backdrop legen,
               dann mit der effektiven Deckkraft gegen das Backdrop blenden. */
            uint32_t colored = sampled;
            if (props->tint_strength > 0u) {
                switch (blend_mode) {
                    case MATFX_BLEND_MULTIPLY: colored = rgb_multiply(sampled, props->tint_rgb); break;
                    case MATFX_BLEND_SCREEN: colored = rgb_screen(sampled, props->tint_rgb); break;
                    default: colored = rgb_blend(props->tint_rgb, sampled, props->tint_strength); break;
                }
            }
            uint32_t result = rgb_blend(colored, sampled, props->opacity);

            /* 4) Lighting: Ambient-Grundhelligkeit + Directional-Light von
               oben-links (einfacher Helligkeits-Multiplikator). */
            uint32_t ambient = 220u;
            uint32_t directional = (lx + ly < (w + h) / 3u) ? 35u : 0u;
            uint32_t light = clamp_u32(ambient + directional, 255u);
            result = rgb_scale(result, light);

            /* 5) Highlight: diagonales Glanzlicht-Band, Breite nach Roughness. */
            if (props->highlight > 0u) {
                int32_t diag = (int32_t)lx - (int32_t)ly;
                if (diag < 0) diag = -diag;
                uint32_t band = 2u + (props->roughness / 32u);
                if ((uint32_t)diag < band) {
                    uint32_t strength = (props->highlight * (band - (uint32_t)diag)) / band;
                    result = rgb_add_clamped(result, strength);
                }
            }

            /* 6) Reflection: vertikal gespiegelte, abgedunkelte Probe,
               staerker an den Kanten (einfacher Fresnel-Ersatz). */
            if (props->reflection > 0u) {
                uint32_t mirror_y = (by + h > ly) ? (by + h - 1u - ly) : by;
                uint32_t mirrored = matfx_get(backdrop, bx + lx, mirror_y);
                uint32_t fresnel = clamp_u32(props->reflection + (edge_factor - 255u), 255u);
                result = rgb_blend(rgb_scale(mirrored, 200u), result, 255u - fresnel);
            }

            matfx_put(dst, x + lx, y + ly, result);
        }
    }
}

int matfx_self_test(void) {
    static uint32_t backdrop_buf[32 * 32];
    static uint32_t dst_buf[32 * 32];
    render2d_surface_t backdrop, dst;
    render2d_init_surface(&backdrop, backdrop_buf, 32u, 32u, 32u);
    render2d_init_surface(&dst, dst_buf, 32u, 32u, 32u);

    for (uint32_t i = 0; i < 32u * 32u; ++i) backdrop_buf[i] = 0x808080u;
    for (uint32_t i = 0; i < 32u * 32u; ++i) dst_buf[i] = 0x000000u;

    /* SOLID (backdrop == NULL): flaechig, unabhaengig vom Hintergrund. */
    material_properties_t solid_props;
    solid_props.color_rgb = 0x123456u; solid_props.tint_strength = 0u; solid_props.opacity = 255u;
    solid_props.blur_radius = 0u; solid_props.refraction = 0u; solid_props.reflection = 0u;
    solid_props.highlight = 0u; solid_props.distortion = 0u; solid_props.roughness = 0u;
    solid_props.tint_rgb = 0u; solid_props.ext_present_mask = 0u;
    matfx_apply(&dst, 0u, 0u, 4u, 4u, 0, 0u, 0u, &solid_props, MATFX_BLEND_NORMAL);
    for (uint32_t i = 0; i < 4u; ++i)
        for (uint32_t j = 0; j < 4u; ++j)
            if (dst_buf[i * 32u + j] != 0x123456u) return -1;

    /* Vollstaendig deckendes Material ohne Blur/Tint muss (vor Lighting)
       nahe am Backdrop-Wert bleiben - hier nur grob gegen Null/0xFFFFFF
       pruefen (Lighting multipliziert, aendert die Farbe aber nicht auf
       einen der beiden Extremwerte). */
    for (uint32_t i = 0; i < 32u * 32u; ++i) dst_buf[i] = 0x000000u;
    material_properties_t glass_props = solid_props;
    glass_props.opacity = 255u;
    glass_props.blur_radius = 3u;
    matfx_apply(&dst, 0u, 0u, 8u, 8u, &backdrop, 0u, 0u, &glass_props, MATFX_BLEND_NORMAL);
    uint32_t sample = dst_buf[3 * 32u + 3];
    if (sample == 0x000000u || sample == 0xFFFFFFu) return -1;

    /* Reproduzierbarkeit (REFRACTION-0001): identische Eingaben liefern
       identische Ergebnisse. */
    for (uint32_t i = 0; i < 32u * 32u; ++i) dst_buf[i] = 0x000000u;
    material_properties_t refract_props = solid_props;
    refract_props.refraction = 200u;
    refract_props.distortion = 150u;
    matfx_apply(&dst, 0u, 0u, 8u, 8u, &backdrop, 0u, 0u, &refract_props, MATFX_BLEND_NORMAL);
    uint32_t first_run[8 * 8];
    for (uint32_t i = 0; i < 64u; ++i) first_run[i] = dst_buf[(i / 8u) * 32u + (i % 8u)];

    for (uint32_t i = 0; i < 32u * 32u; ++i) dst_buf[i] = 0x000000u;
    matfx_apply(&dst, 0u, 0u, 8u, 8u, &backdrop, 0u, 0u, &refract_props, MATFX_BLEND_NORMAL);
    for (uint32_t i = 0; i < 64u; ++i)
        if (dst_buf[(i / 8u) * 32u + (i % 8u)] != first_run[i]) return -1;

    /* Tint verschiebt die Farbe erkennbar in Richtung tint_rgb. */
    for (uint32_t i = 0; i < 32u * 32u; ++i) dst_buf[i] = 0x000000u;
    material_properties_t tint_props = solid_props;
    tint_props.tint_rgb = 0xFF0000u;
    tint_props.tint_strength = 255u;
    matfx_apply(&dst, 0u, 0u, 4u, 4u, &backdrop, 0u, 0u, &tint_props, MATFX_BLEND_NORMAL);
    uint32_t tinted = dst_buf[0];
    if ((uint8_t)(tinted >> 16) == 0u) return -1; /* Rotanteil muss vorhanden sein */

    return 0;
}
