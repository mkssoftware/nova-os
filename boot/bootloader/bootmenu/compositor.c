#include "compositor.h"

#include "graphics.h"

static uint32_t surface_pixels[2][NOVA_SURFACE_WIDTH * NOVA_SURFACE_HEIGHT];
static nova_surface_t surfaces[2];
static bool surface_used[2];
static nova_layer_t layers[NOVA_LAYER_CAPACITY];
static uint32_t composed[NOVA_SURFACE_WIDTH * NOVA_SURFACE_HEIGHT];
static uint8_t layer_count;
static uint32_t output_width, output_height;
static nova_compositor_diagnostics_t diagnostics;

static bool rect_valid(nova_rect_t r)
{
    return r.width > 0 && r.height > 0 && r.x >= 0 && r.y >= 0;
}

static nova_rect_t clip(nova_rect_t r, uint32_t width, uint32_t height)
{
    if (r.x < 0) { r.width += r.x; r.x = 0; }
    if (r.y < 0) { r.height += r.y; r.y = 0; }
    if ((uint32_t)r.x >= width || (uint32_t)r.y >= height) return (nova_rect_t){0};
    if ((uint32_t)(r.x + r.width) > width) r.width = (int32_t)width - r.x;
    if ((uint32_t)(r.y + r.height) > height) r.height = (int32_t)height - r.y;
    return r;
}

bool nova_compositor_initialize(uint32_t width, uint32_t height)
{
    if (!width || !height || width > NOVA_SURFACE_WIDTH || height > NOVA_SURFACE_HEIGHT)
        return false;
    output_width = width;
    output_height = height;
    layer_count = 0;
    diagnostics = (nova_compositor_diagnostics_t){0};
    for (uint32_t i = 0; i < 2; ++i) {
        surface_used[i] = false;
        surfaces[i] = (nova_surface_t){surface_pixels[i], width, height, width, {{0}}, 0, true};
    }
    return true;
}

nova_surface_t *nova_surface_acquire(void)
{
    for (uint32_t i = 0; i < 2; ++i) {
        if (!surface_used[i]) { surface_used[i] = true; return &surfaces[i]; }
    }
    return 0;
}

void nova_damage_add(nova_surface_t *surface, nova_rect_t rect)
{
    if (!surface || !rect_valid(rect)) return;
    rect = clip(rect, surface->width, surface->height);
    if (!rect_valid(rect)) return;
    if (surface->damage_count == NOVA_DAMAGE_CAPACITY) {
        surface->full_damage = true;
        ++diagnostics.damage_overflows;
        return;
    }
    surface->damage[surface->damage_count++] = rect;
}

void nova_damage_merge(nova_surface_t *surface)
{
    if (!surface || surface->full_damage || surface->damage_count < 2) return;
    nova_rect_t merged = surface->damage[0];
    for (uint8_t i = 1; i < surface->damage_count; ++i) {
        nova_rect_t r = surface->damage[i];
        int32_t right = merged.x + merged.width;
        int32_t bottom = merged.y + merged.height;
        int32_t rright = r.x + r.width;
        int32_t rbottom = r.y + r.height;
        if (r.x < merged.x) merged.x = r.x;
        if (r.y < merged.y) merged.y = r.y;
        if (rright > right) right = rright;
        if (rbottom > bottom) bottom = rbottom;
        merged.width = right - merged.x;
        merged.height = bottom - merged.y;
    }
    surface->damage[0] = merged;
    surface->damage_count = 1;
}

void nova_damage_clear(nova_surface_t *surface)
{
    if (surface) { surface->damage_count = 0; surface->full_damage = false; }
}

void nova_surface_rect(nova_surface_t *surface, nova_rect_t rect, uint32_t color)
{
    if (!surface) return;
    rect = clip(rect, surface->width, surface->height);
    if (!rect_valid(rect)) return;
    for (int32_t y = 0; y < rect.height; ++y) {
        uint32_t *row = surface->pixels + (rect.y + y) * surface->stride + rect.x;
        for (int32_t x = 0; x < rect.width; ++x) row[x] = color;
    }
    nova_damage_add(surface, rect);
}

void nova_surface_clear(nova_surface_t *surface, uint32_t color)
{
    if (!surface) return;
    nova_surface_rect(surface, (nova_rect_t){0, 0, (int32_t)surface->width,
                      (int32_t)surface->height}, color);
    surface->full_damage = true;
}

bool nova_compositor_begin_frame(void)
{
    layer_count = 0;
    diagnostics.modal_active = false;
    return output_width && output_height;
}

bool nova_compositor_submit_layer(const nova_layer_t *layer)
{
    if (!layer || !layer->surface || !layer->visible || layer_count == NOVA_LAYER_CAPACITY ||
        !rect_valid(layer->source) || !rect_valid(layer->destination)) {
        ++diagnostics.rejected_layers;
        return false;
    }
    for (uint8_t i = 0; i < layer_count; ++i)
        if (layers[i].id == layer->id) { ++diagnostics.rejected_layers; return false; }
    uint8_t insert = layer_count;
    while (insert && layers[insert - 1].z_index > layer->z_index) {
        layers[insert] = layers[insert - 1];
        --insert;
    }
    layers[insert] = *layer;
    ++layer_count;
    ++diagnostics.submitted;
    if (layer->modal) diagnostics.modal_active = true;
    return true;
}

static uint32_t blend(uint32_t background, uint32_t foreground, uint8_t opacity)
{
    uint32_t a = ((foreground >> 24) & 0xffu) * opacity / 255u;
    uint32_t inverse = 255u - a;
    uint32_t red = ((((foreground >> 16) & 0xffu) * a) +
                    (((background >> 16) & 0xffu) * inverse)) / 255u;
    uint32_t green = ((((foreground >> 8) & 0xffu) * a) +
                      (((background >> 8) & 0xffu) * inverse)) / 255u;
    uint32_t blue = (((foreground & 0xffu) * a) +
                     ((background & 0xffu) * inverse)) / 255u;
    return 0xff000000u | (red << 16) | (green << 8) | blue;
}

static uint32_t backdrop_blur(int32_t x, int32_t y)
{
    uint32_t red = 0, green = 0, blue = 0, samples = 0;
    for (int32_t oy = -1; oy <= 1; ++oy) {
        int32_t py = y + oy;
        if (py < 0 || (uint32_t)py >= output_height) continue;
        for (int32_t ox = -1; ox <= 1; ++ox) {
            int32_t px = x + ox;
            if (px < 0 || (uint32_t)px >= output_width) continue;
            uint32_t pixel = composed[py * NOVA_SURFACE_WIDTH + px];
            red += (pixel >> 16) & 0xffu;
            green += (pixel >> 8) & 0xffu;
            blue += pixel & 0xffu;
            ++samples;
        }
    }
    if (!samples) return 0xff000000u;
    return 0xff000000u | ((red / samples) << 16) |
           ((green / samples) << 8) | (blue / samples);
}

static uint32_t glass_material_tint=0x80212a33u;
static uint32_t acrylic_material_tint=0xa0182028u;
void nova_compositor_set_material_tokens(uint32_t glass_tint,uint32_t acrylic_tint)
{glass_material_tint=glass_tint;acrylic_material_tint=acrylic_tint;}

static uint32_t material_pixel(const nova_layer_t *layer, uint32_t source,
                               uint32_t backdrop, int32_t x, int32_t y)
{
    if ((source >> 24) == 0) return backdrop;
    if (diagnostics.fallback_level >= 2 || layer->material == NOVA_MATERIAL_SOLID)
        return source | 0xff000000u;
    if (layer->material == NOVA_MATERIAL_GLASS) {
        return blend(backdrop_blur(x, y), glass_material_tint, 210);
    }
    if (layer->material == NOVA_MATERIAL_ACRYLIC) {
        uint32_t noise = (uint32_t)((x * 1103515245u + y * 12345u) >> 28) * 0x010101u;
        return blend(blend(backdrop_blur(x, y), acrylic_material_tint, 220),
                     0x20000000u | noise, 64);
    }
    return blend(backdrop, source, layer->opacity);
}

bool nova_compositor_compose(void)
{
    for (uint8_t index = 0; index < layer_count; ++index) {
        nova_layer_t *layer = &layers[index];
        nova_rect_t destination = clip(layer->destination, output_width, output_height);
        if (!rect_valid(destination)) continue;
        nova_rect_t damage = destination;
        if (!layer->surface->full_damage && layer->surface->damage_count) {
            nova_damage_merge(layer->surface);
            damage = layer->surface->damage[0];
            damage.x += destination.x;
            damage.y += destination.y;
            damage = clip(damage, output_width, output_height);
        }
        for (int32_t y = 0; y < damage.height; ++y) {
            for (int32_t x = 0; x < damage.width; ++x) {
                int32_t dx = damage.x + x, dy = damage.y + y;
                int32_t sx = layer->source.x + dx - destination.x;
                int32_t sy = layer->source.y + dy - destination.y;
                if (sx < 0 || sy < 0 || (uint32_t)sx >= layer->surface->width ||
                    (uint32_t)sy >= layer->surface->height) continue;
                uint32_t source = layer->surface->pixels[sy * layer->surface->stride + sx];
                uint32_t back = composed[dy * NOVA_SURFACE_WIDTH + dx];
                uint32_t result = material_pixel(layer, source, back, dx, dy);
                composed[dy * NOVA_SURFACE_WIDTH + dx] = result;
            }
        }
        ++diagnostics.composed_regions;
        nova_damage_clear(layer->surface);
    }
    ++diagnostics.frames;
    return true;
}

bool nova_compositor_present(void)
{return nova_graphics_present(composed,output_width,output_height,NOVA_SURFACE_WIDTH);}

void nova_compositor_set_fallback(uint8_t level)
{
    diagnostics.fallback_level = level > 3 ? 3 : level;
}

bool nova_compositor_input_allowed(uint32_t id)
{
    if (!diagnostics.modal_active) return true;
    for (uint8_t i = layer_count; i; --i)
        if (layers[i - 1].modal) return layers[i - 1].id == id;
    return false;
}

const nova_compositor_diagnostics_t *nova_compositor_diagnostics(void)
{
    return &diagnostics;
}
