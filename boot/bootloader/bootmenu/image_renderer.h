#ifndef NOVA_BOOT_IMAGE_RENDERER_H
#define NOVA_BOOT_IMAGE_RENDERER_H

#include <stdbool.h>
#include <stdint.h>
#include "compositor.h"
#include "transform2d.h"

#define NOVA_IMAGE_CAPACITY 16u
#define NOVA_IMAGE_PIXEL_BUDGET (512u*1024u)

typedef enum {NOVA_IMAGE_RGBA8888,NOVA_IMAGE_BGRA8888,NOVA_IMAGE_RGB888,
    NOVA_IMAGE_RGB565} nova_image_pixel_format_t;
typedef enum {NOVA_IMAGE_SAMPLE_NEAREST,NOVA_IMAGE_SAMPLE_BILINEAR}
    nova_image_sampling_t;
typedef enum {NOVA_IMAGE_OK,NOVA_IMAGE_INVALID,NOVA_IMAGE_UNSUPPORTED,
    NOVA_IMAGE_NO_MEMORY,NOVA_IMAGE_NOT_FOUND,NOVA_IMAGE_CORRUPT}
    nova_image_result_t;
typedef struct nova_image {
    uint32_t width,height,stride;
    nova_image_pixel_format_t source_format;
    uint32_t *pixels;
    uint64_t resource_id;
    uint32_t references,generation,memory_bytes;
    bool premultiplied,used;
} nova_image_t;
typedef struct {
    nova_rect_t destination,clip;
    nova_image_sampling_t sampling;
    nova_fixed_transform2d_t transform;
    uint16_t mask_scope,opacity;
    uint32_t tint;
    bool mirror_x,mirror_y;
} nova_image_render_options_t;
typedef struct {
    uint32_t initializations,loaded,raw_images,bmp_decodes,png_decodes;
    uint32_t cache_hits,cache_misses,destroys,renders,errors,clipped;
    uint32_t png_validations,png_chunks,png_crc_errors,png_format_errors;
    uint32_t png_filter_rows,png_idat_chunks;
    uint64_t pixels_rendered,memory_used,largest_image;
    uint32_t last_png_width,last_png_height;
    uint8_t last_png_color_type,last_png_bit_depth;
    bool initialized;
} nova_image_diagnostics_t;

bool nova_image_initialize(void);
nova_image_result_t nova_image_create_raw(uint64_t resource_id,uint32_t width,
    uint32_t height,uint32_t stride,nova_image_pixel_format_t format,
    const void *pixels,uint64_t size,nova_image_t **image);
nova_image_result_t nova_image_create_2bit_alpha(uint64_t resource_id,uint32_t width,
    uint32_t height,const uint8_t *pixels,uint64_t size,nova_image_t **image);
nova_image_result_t nova_image_load(const char *resource,nova_image_t **image);
bool nova_png_initialize(void);
nova_image_result_t nova_png_decode(uint64_t resource_id,const void *source,
    uint64_t source_size,nova_image_t **image);
bool nova_png_validate(const void *source,uint64_t source_size);
bool nova_image_destroy(nova_image_t *image);
bool nova_image_get_size(const nova_image_t *image,uint32_t *width,uint32_t *height);
nova_image_result_t nova_image_render(const nova_image_t *image,nova_surface_t *surface,
    const nova_image_render_options_t *options);
const nova_image_diagnostics_t *nova_image_diagnostics(void);

#endif
