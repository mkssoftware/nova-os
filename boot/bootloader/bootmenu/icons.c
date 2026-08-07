#include "icons.h"
#include "resources.h"
#include "boot_icon_data.h"

typedef struct { const char *uri; const uint8_t *bitmap; } icon_entry_t;
static const icon_entry_t entries[NOVA_ICON_COUNT] = {
    [NOVA_ICON_HOME] = {"boot://icons/home", bm_icon_home},
    [NOVA_ICON_INSTALL] = {"boot://icons/install", bm_icon_install},
    [NOVA_ICON_SETTINGS] = {"boot://icons/settings", bm_icon_settings},
    [NOVA_ICON_DIAGNOSTICS] = {"boot://icons/diagnostics", bm_icon_diagnostics},
    [NOVA_ICON_RECOVERY] = {"boot://icons/recovery", bm_icon_recover},
    [NOVA_ICON_POWER] = {"boot://icons/power", bm_icon_power}
};
static nova_icon_diagnostics_t diagnostics;

bool nova_icons_initialize(void)
{
    diagnostics = (nova_icon_diagnostics_t){0};
    for (uint32_t i = 0; i < NOVA_ICON_COUNT; ++i) if (entries[i].bitmap) {
        if (!nova_resource_register(entries[i].uri, NOVA_RESOURCE_ICON, 1,
                                    entries[i].bitmap, 100, 0, 0)) return false;
    }
    return true;
}

bool nova_icon_exists(nova_icon_token_t token)
{
    return token < NOVA_ICON_COUNT && entries[token].bitmap;
}

static uint32_t blend(uint32_t background, uint32_t foreground, uint8_t alpha)
{
    uint32_t inverse = 255u - alpha;
    uint32_t r = ((((foreground >> 16) & 255u) * alpha) +
                  (((background >> 16) & 255u) * inverse)) / 255u;
    uint32_t g = ((((foreground >> 8) & 255u) * alpha) +
                  (((background >> 8) & 255u) * inverse)) / 255u;
    uint32_t b = (((foreground & 255u) * alpha) + ((background & 255u) * inverse)) / 255u;
    return 0xff000000u | (r << 16) | (g << 8) | b;
}

void nova_icon_draw(nova_surface_t *surface, nova_icon_token_t token,
                    int32_t x, int32_t y, uint16_t size, uint32_t color)
{
    if (!surface || !size) return;
    ++diagnostics.lookups;
    const uint8_t *bitmap = token < NOVA_ICON_COUNT ? entries[token].bitmap : 0;
    if (!bitmap) { bitmap = bm_icon_home; ++diagnostics.fallbacks; }
    else ++diagnostics.cache_hits;
    for (uint16_t dy = 0; dy < size; ++dy) for (uint16_t dx = 0; dx < size; ++dx) {
        uint32_t sx = (uint32_t)dx * 20u / size;
        uint32_t sy = (uint32_t)dy * 20u / size;
        uint32_t pixel = sy * 20u + sx;
        uint8_t coverage = (bitmap[pixel / 4] >> ((pixel & 3u) * 2)) & 3u;
        if (!coverage) continue;
        int32_t px = x + dx, py = y + dy;
        if (px < 0 || py < 0 || (uint32_t)px >= surface->width || (uint32_t)py >= surface->height) continue;
        uint32_t *destination = &surface->pixels[py * surface->stride + px];
        *destination = blend(*destination, color, (uint8_t)(coverage * 85u));
    }
    nova_damage_add(surface, (nova_rect_t){x, y, size, size});
    ++diagnostics.renders;
}

const nova_icon_diagnostics_t *nova_icon_diagnostics(void) { return &diagnostics; }
