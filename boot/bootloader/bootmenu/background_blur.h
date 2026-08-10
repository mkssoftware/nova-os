#ifndef NOVA_BOOT_BACKGROUND_BLUR_H
#define NOVA_BOOT_BACKGROUND_BLUR_H

#include <stdbool.h>
#include <stdint.h>
#include "rounded_geometry.h"

typedef enum {NOVA_BLUR_QUALITY_LOW,NOVA_BLUR_QUALITY_STANDARD,
    NOVA_BLUR_QUALITY_HIGH} nova_blur_quality_t;
typedef enum {NOVA_BLUR_KERNEL_BOX,NOVA_BLUR_KERNEL_GAUSSIAN,
    NOVA_BLUR_KERNEL_DUAL_PASS_GAUSSIAN} nova_blur_kernel_t;

typedef struct {
    nova_rect_t bounds;
    float radius;
    nova_blur_quality_t quality;
    nova_blur_kernel_t kernel;
    const nova_round_rect_t *rounded_geometry;
    uint16_t mask_scope;
} nova_blur_region_t;
typedef enum {NOVA_BLUR_OK,NOVA_BLUR_INVALID,NOVA_BLUR_NOT_INITIALIZED,
    NOVA_BLUR_RECURSIVE,NOVA_BLUR_NO_MEMORY} nova_blur_result_t;
typedef struct {
    uint32_t initializations,shutdowns,regions,cache_hits,cache_misses;
    uint32_t invalidations,pixels_copied,pixels_blurred,memory_bytes,rejections;
    uint16_t last_radius,maximum_radius;
    nova_blur_quality_t quality;
    nova_blur_kernel_t kernel;
    bool initialized,rendering,cache_valid,degraded;
} nova_blur_diagnostics_t;

bool nova_blur_initialize(uint32_t width,uint32_t height,uint32_t memory_budget);
void nova_blur_shutdown(void);
nova_blur_result_t nova_blur_render(const nova_blur_region_t *region,
    const nova_surface_t *background,nova_surface_t *destination);
void nova_blur_invalidate(const nova_rect_t *area);
void nova_blur_clear_cache(void);
void nova_blur_set_quality(nova_blur_quality_t quality);
const nova_blur_diagnostics_t *nova_blur_diagnostics(void);

#endif
