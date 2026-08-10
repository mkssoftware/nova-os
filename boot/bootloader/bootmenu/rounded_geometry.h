#ifndef NOVA_BOOT_ROUNDED_GEOMETRY_H
#define NOVA_BOOT_ROUNDED_GEOMETRY_H

#include <stdbool.h>
#include <stdint.h>
#include "compositor.h"

typedef enum {
    NOVA_ROUNDED_AA_DISABLED,
    NOVA_ROUNDED_AA_STANDARD,
    NOVA_ROUNDED_AA_HIGH
} nova_rounded_aa_t;

typedef enum {
    NOVA_ROUNDED_BORDER_INSIDE,
    NOVA_ROUNDED_BORDER_CENTERED,
    NOVA_ROUNDED_BORDER_OUTSIDE
} nova_rounded_border_mode_t;

typedef struct {
    uint16_t top_left,top_right,bottom_right,bottom_left;
} nova_corner_radius_t;

typedef struct {
    nova_rect_t bounds;
    nova_corner_radius_t corners;
} nova_round_rect_t;

typedef struct {
    uint32_t created,rendered,hit_tests,rejected,cache_hits,cache_misses;
    uint64_t radius_sum;
    uint16_t maximum_radius;
    bool anti_aliasing,shadow_active;
} nova_rounded_diagnostics_t;

bool nova_round_rect_create(nova_round_rect_t *rect,int32_t x,int32_t y,
    int32_t width,int32_t height,uint16_t radius);
bool nova_round_rect_create_corners(nova_round_rect_t *rect,nova_rect_t bounds,
    nova_corner_radius_t corners);
bool nova_round_rect_set_radius(nova_round_rect_t *rect,uint16_t radius);
bool nova_round_rect_set_corners(nova_round_rect_t *rect,nova_corner_radius_t corners);
bool nova_round_rect_scale(const nova_round_rect_t *rect,uint32_t scale_16_16,
    nova_round_rect_t *scaled);
bool nova_round_rect_lerp(const nova_round_rect_t *from,const nova_round_rect_t *to,
    uint16_t progress_1000,nova_round_rect_t *result);
bool nova_round_rect_contains(const nova_round_rect_t *rect,int32_t x,int32_t y);
bool nova_round_rect_contains_8_8(const nova_round_rect_t *rect,int32_t x_8_8,
    int32_t y_8_8);
uint8_t nova_round_rect_coverage(const nova_round_rect_t *rect,int32_t pixel_x,
    int32_t pixel_y,nova_rounded_aa_t quality);
bool nova_round_rect_render(nova_surface_t *surface,const nova_round_rect_t *rect,
    uint32_t color,nova_rounded_aa_t quality);
bool nova_round_rect_render_border(nova_surface_t *surface,const nova_round_rect_t *rect,
    uint16_t thickness,nova_rounded_border_mode_t mode,uint32_t color,
    nova_rounded_aa_t quality);
void nova_rounded_reset_diagnostics(void);
const nova_rounded_diagnostics_t *nova_rounded_diagnostics(void);

#endif
