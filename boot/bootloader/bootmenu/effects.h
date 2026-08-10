#ifndef NOVA_BOOT_EFFECTS_H
#define NOVA_BOOT_EFFECTS_H

#include <stdbool.h>
#include <stdint.h>
#include "rounded_geometry.h"

#define NOVA_EFFECT_CAPACITY 8u

typedef enum {
    NOVA_EFFECT_DROP_SHADOW,NOVA_EFFECT_INNER_SHADOW,NOVA_EFFECT_AMBIENT_SHADOW,
    NOVA_EFFECT_CONTACT_SHADOW,NOVA_EFFECT_OUTER_GLOW,NOVA_EFFECT_INNER_GLOW,
    NOVA_EFFECT_FOCUS_GLOW,NOVA_EFFECT_ACCENT_GLOW
} nova_effect_type_t;
typedef enum {NOVA_EFFECT_QUALITY_LOW,NOVA_EFFECT_QUALITY_STANDARD,
    NOVA_EFFECT_QUALITY_HIGH} nova_effect_quality_t;
typedef enum {NOVA_EFFECT_PHASE_SHADOW,NOVA_EFFECT_PHASE_GLOW} nova_effect_phase_t;

typedef struct {
    nova_effect_type_t type;
    uint32_t color;
    float radius,opacity,offset_x,offset_y;
} nova_effect_t;
typedef struct {
    nova_round_rect_t geometry;
    nova_effect_t effects[NOVA_EFFECT_CAPACITY];
    uint8_t effect_count;
} nova_effect_object_t;
typedef struct {
    uint32_t objects,effects_set,effects_cleared,shadows,glows,rendered_pixels;
    uint32_t rejected,cache_hits,cache_misses,frames;
    uint16_t maximum_radius,effects_this_frame;
    nova_effect_quality_t quality;
} nova_effect_diagnostics_t;

void nova_effect_initialize(nova_effect_quality_t quality);
bool nova_effect_object_create(nova_effect_object_t *object,
    const nova_round_rect_t *geometry);
bool nova_shadow_set(nova_effect_object_t *object,const nova_effect_t *shadow);
bool nova_glow_set(nova_effect_object_t *object,const nova_effect_t *glow);
void nova_effect_clear(nova_effect_object_t *object);
bool nova_effect_lerp(const nova_effect_t *from,const nova_effect_t *to,
    uint16_t progress_1000,nova_effect_t *result);
bool nova_effect_render(const nova_effect_object_t *object,nova_surface_t *surface,
    nova_effect_phase_t phase,nova_rect_t clip,uint16_t mask_scope);
void nova_effect_begin_frame(void);
void nova_effect_set_quality(nova_effect_quality_t quality);
void nova_effect_set_density(uint8_t maximum_effects);
const nova_effect_diagnostics_t *nova_effect_diagnostics(void);

#endif
