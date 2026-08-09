#ifndef NOVA_BOOT_COMPOSITOR_H
#define NOVA_BOOT_COMPOSITOR_H

#include <stdint.h>
#include <stdbool.h>

#define NOVA_SURFACE_WIDTH 1920u
#define NOVA_SURFACE_HEIGHT 1080u
#define NOVA_LAYER_CAPACITY 16u
#define NOVA_DAMAGE_CAPACITY 32u

typedef struct { int32_t x, y, width, height; } nova_rect_t;
typedef struct {
    uint32_t *pixels;
    uint32_t width, height, stride;
    nova_rect_t damage[NOVA_DAMAGE_CAPACITY];
    uint8_t damage_count;
    bool full_damage;
} nova_surface_t;

typedef enum {
    NOVA_MATERIAL_NONE,
    NOVA_MATERIAL_GLASS,
    NOVA_MATERIAL_ACRYLIC,
    NOVA_MATERIAL_SOLID
} nova_material_kind_t;

typedef struct {
    uint32_t id;
    nova_surface_t *surface;
    nova_rect_t source;
    nova_rect_t destination;
    int16_t z_index;
    uint8_t opacity;
    nova_material_kind_t material;
    bool visible;
    bool overlay;
    bool modal;
    bool dirty;
} nova_layer_t;

typedef struct {
    uint32_t frames;
    uint32_t submitted;
    uint32_t composed_regions;
    uint32_t rejected_layers;
    uint32_t damage_overflows;
    uint8_t fallback_level;
    bool modal_active;
} nova_compositor_diagnostics_t;

bool nova_compositor_initialize(uint32_t width, uint32_t height);
void nova_compositor_set_material_tokens(uint32_t glass_tint,uint32_t acrylic_tint);
nova_surface_t *nova_surface_acquire(void);
void nova_surface_clear(nova_surface_t *surface, uint32_t color);
void nova_surface_rect(nova_surface_t *surface, nova_rect_t rect, uint32_t color);
void nova_damage_add(nova_surface_t *surface, nova_rect_t rect);
void nova_damage_merge(nova_surface_t *surface);
void nova_damage_clear(nova_surface_t *surface);
bool nova_compositor_begin_frame(void);
bool nova_compositor_submit_layer(const nova_layer_t *layer);
bool nova_compositor_compose(void);
void nova_compositor_set_fallback(uint8_t level);
bool nova_compositor_input_allowed(uint32_t layer_id);
const nova_compositor_diagnostics_t *nova_compositor_diagnostics(void);

#endif
