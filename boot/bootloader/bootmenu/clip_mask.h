#ifndef NOVA_BOOT_CLIP_MASK_H
#define NOVA_BOOT_CLIP_MASK_H

#include <stdbool.h>
#include <stdint.h>
#include "compositor.h"

#define NOVA_CLIP_STACK_CAPACITY 32u
#define NOVA_CLIP_SCOPE_CAPACITY 64u
#define NOVA_MASK_CAPACITY 64u
#define NOVA_MASK_STACK_CAPACITY 32u
#define NOVA_MASK_SCOPE_CAPACITY 64u
#define NOVA_MASK_CACHE_CAPACITY 256u

typedef uint32_t nova_mask_handle_t;
typedef uint8_t (*nova_vector_mask_callback_t)(int32_t x,int32_t y,void *context);

typedef enum {
    NOVA_CLIP_OUTSIDE,
    NOVA_CLIP_PARTIAL,
    NOVA_CLIP_INSIDE
} nova_clip_visibility_t;

typedef enum {
    NOVA_MASK_RECTANGLE,
    NOVA_MASK_BITMAP,
    NOVA_MASK_ALPHA,
    NOVA_MASK_VECTOR,
    NOVA_MASK_ROUNDED_RECTANGLE
} nova_mask_type_t;

typedef struct {
    nova_mask_type_t type;
    nova_rect_t bounds;
    const uint8_t *pixels;
    uint32_t stride;
    uint64_t size;
    uint16_t radius;
    nova_vector_mask_callback_t vector_callback;
    void *vector_context;
    bool cacheable;
} nova_mask_descriptor_t;

typedef struct {
    uint32_t initializations,frames,clip_pushes,clip_pops,clip_tests;
    uint32_t clip_rejections,clip_stack_overflows,fully_clipped,partially_clipped;
    uint32_t masks_created,masks_destroyed,mask_pushes,mask_pops;
    uint32_t invalid_masks,mask_stack_overflows,recursive_masks;
    uint32_t mask_tests,cache_hits,cache_misses,masked_pixels;
    uint16_t clip_depth_peak,mask_depth_peak;
    bool initialized,frame_active;
} nova_clip_mask_diagnostics_t;

bool nova_clip_mask_initialize(uint32_t width,uint32_t height);
bool nova_clip_mask_begin_frame(void);
bool nova_clip_mask_end_frame(void);
void nova_clip_mask_abort_frame(void);
bool nova_clip_push(nova_rect_t rectangle);
bool nova_clip_pop(void);
nova_rect_t nova_clip_current(void);
uint16_t nova_clip_current_scope(void);
bool nova_clip_scope_get(uint16_t scope,nova_rect_t *rectangle);
nova_clip_visibility_t nova_clip_test(uint16_t scope,nova_rect_t bounds,
                                      nova_rect_t *visible);
bool nova_mask_create(const nova_mask_descriptor_t *descriptor,
                      nova_mask_handle_t *handle);
bool nova_mask_destroy(nova_mask_handle_t handle);
bool nova_mask_push(nova_mask_handle_t handle);
bool nova_mask_pop(void);
uint16_t nova_mask_current_scope(void);
uint8_t nova_mask_coverage(uint16_t scope,int32_t x,int32_t y);
const nova_clip_mask_diagnostics_t *nova_clip_mask_diagnostics(void);

#endif
