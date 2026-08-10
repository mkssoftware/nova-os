#ifndef NOVA_BOOT_TRANSFORM2D_H
#define NOVA_BOOT_TRANSFORM2D_H

#include <stdbool.h>
#include <stdint.h>
#include "compositor.h"

#define NOVA_TRANSFORM_FIXED_ONE 65536
#define NOVA_TRANSFORM_STACK_CAPACITY 32u

typedef struct {float m11,m12,m21,m22,tx,ty;} nova_transform2d_t;
typedef struct {int32_t m11,m12,m21,m22,tx,ty;} nova_fixed_transform2d_t;
typedef struct {float x,y;} nova_point2d_t;
typedef struct {int32_t x,y;} nova_fixed_point2d_t;

typedef struct {
    uint32_t identities,validations,rejected,translations,rotations,scales;
    uint32_t mirrors,shears,pivots,multiplications,inversions,points,bounds;
    uint32_t fixed_conversions,fixed_multiplications,dirty_regions;
    uint32_t stack_pushes,stack_pops,stack_overflows;
    uint16_t stack_peak;
} nova_transform_diagnostics_t;

nova_transform2d_t nova_transform_identity(void);
bool nova_transform_valid(const nova_transform2d_t *transform);
bool nova_transform_translate(nova_transform2d_t *transform,float x,float y);
bool nova_transform_rotate(nova_transform2d_t *transform,float radians);
bool nova_transform_scale(nova_transform2d_t *transform,float sx,float sy);
bool nova_transform_mirror(nova_transform2d_t *transform,bool horizontal,
                           bool vertical);
bool nova_transform_shear(nova_transform2d_t *transform,float x,float y);
bool nova_transform_pivot(nova_transform2d_t *transform,
                          const nova_transform2d_t *operation,float x,float y);
bool nova_transform_multiply(nova_transform2d_t *result,
                             const nova_transform2d_t *left,
                             const nova_transform2d_t *right);
bool nova_transform_inverse(nova_transform2d_t *result,
                            const nova_transform2d_t *transform);
bool nova_transform_point(const nova_transform2d_t *transform,
                          nova_point2d_t point,nova_point2d_t *result);
bool nova_transform_bounds(const nova_transform2d_t *transform,nova_rect_t bounds,
                           nova_rect_t *result);
bool nova_transform_to_fixed(const nova_transform2d_t *transform,
                             nova_fixed_transform2d_t *fixed);
nova_fixed_transform2d_t nova_transform_fixed_identity(void);
bool nova_transform_fixed_valid(nova_fixed_transform2d_t transform);
bool nova_transform_fixed_multiply(nova_fixed_transform2d_t *result,
                                   nova_fixed_transform2d_t left,
                                   nova_fixed_transform2d_t right);
bool nova_transform_fixed_inverse(nova_fixed_transform2d_t *result,
                                  nova_fixed_transform2d_t transform);
bool nova_transform_fixed_point(nova_fixed_transform2d_t transform,
                                nova_fixed_point2d_t point,
                                nova_fixed_point2d_t *result);
bool nova_transform_fixed_bounds(nova_fixed_transform2d_t transform,
                                 nova_rect_t bounds,nova_rect_t *result);
bool nova_transform_dirty(nova_rect_t old_bounds,nova_rect_t new_bounds);
void nova_transform_stack_reset(void);
bool nova_transform_stack_push(const nova_transform2d_t *local);
bool nova_transform_stack_pop(void);
nova_transform2d_t nova_transform_stack_current(void);
uint16_t nova_transform_stack_depth(void);
const nova_transform_diagnostics_t *nova_transform_diagnostics(void);

#endif
