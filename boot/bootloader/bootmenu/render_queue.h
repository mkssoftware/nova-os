#ifndef NOVA_BOOT_RENDER_QUEUE_H
#define NOVA_BOOT_RENDER_QUEUE_H

#include <stdbool.h>
#include <stdint.h>
#include "compositor.h"
#include "transform2d.h"
#include "rounded_geometry.h"

#define NOVA_RENDER_COMMAND_CAPACITY 1024u
#define NOVA_RENDER_CLIP_CAPACITY 16u

typedef enum {NOVA_RENDER_BACKGROUND,NOVA_RENDER_IMAGE,NOVA_RENDER_SHAPE,
    NOVA_RENDER_CONTROL,NOVA_RENDER_TEXT,NOVA_RENDER_DIALOG,
    NOVA_RENDER_OVERLAY,NOVA_RENDER_CURSOR} nova_render_layer_t;
typedef enum {NOVA_RENDER_CLEAR,NOVA_RENDER_RECT,NOVA_RENDER_LINE,
    NOVA_RENDER_CIRCLE,NOVA_RENDER_ROUNDED_RECT} nova_render_command_type_t;
typedef struct {
    uint32_t id,creation;
    nova_render_command_type_t type;
    nova_render_layer_t layer;
    int16_t z_index;
    uint16_t clip_id,mask_scope,opacity;
    uint64_t resource;
    nova_surface_t *surface;
    nova_fixed_transform2d_t transform;
    nova_rect_t geometry;
    nova_corner_radius_t corners;
    int32_t x2,y2,radius;
    uint32_t color;
} nova_render_command_t;
typedef struct {uint32_t frames,submitted,executed,rejected,sorted,batches,
    overflows,clip_pushes,clip_pops;uint16_t peak_commands;bool active;}
nova_render_diagnostics_t;

void nova_render_initialize(void);
bool nova_render_begin_frame(void);
bool nova_render_push_clip(nova_rect_t clip);
bool nova_render_pop_clip(void);
bool nova_render_push_transform(const nova_transform2d_t *transform);
bool nova_render_pop_transform(void);
bool nova_render_submit(const nova_render_command_t *command);
bool nova_render_clear(nova_surface_t *surface,uint32_t color,nova_render_layer_t layer);
bool nova_render_rect(nova_surface_t *surface,nova_rect_t rect,uint32_t color,
    nova_render_layer_t layer,int16_t z_index);
bool nova_render_line(nova_surface_t *surface,int32_t x1,int32_t y1,int32_t x2,
    int32_t y2,uint32_t color,nova_render_layer_t layer,int16_t z_index);
bool nova_render_circle(nova_surface_t *surface,int32_t cx,int32_t cy,int32_t radius,
    uint32_t color,nova_render_layer_t layer,int16_t z_index);
bool nova_render_rounded_rect(nova_surface_t *surface,const nova_round_rect_t *rect,
    uint32_t color,nova_render_layer_t layer,int16_t z_index);
bool nova_render_validate(void);
bool nova_render_sort_optimize(void);
bool nova_render_execute(void);
bool nova_render_end_frame(void);
void nova_render_abort_frame(void);
uint16_t nova_render_command_count(void);
const nova_render_command_t *nova_render_command(uint16_t index);
const nova_render_diagnostics_t *nova_render_diagnostics(void);

#endif
