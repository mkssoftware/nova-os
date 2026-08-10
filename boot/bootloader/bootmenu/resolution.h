#ifndef NOVA_BOOT_RESOLUTION_H
#define NOVA_BOOT_RESOLUTION_H

#include <stdbool.h>
#include <stdint.h>
#include "compositor.h"

typedef enum {
    NOVA_RESOLUTION_XS,NOVA_RESOLUTION_S,NOVA_RESOLUTION_M,NOVA_RESOLUTION_L,
    NOVA_RESOLUTION_XL,NOVA_RESOLUTION_XXL,NOVA_RESOLUTION_ULTRA
} nova_resolution_class_t;
typedef enum {
    NOVA_ASPECT_4_3,NOVA_ASPECT_5_4,NOVA_ASPECT_16_9,NOVA_ASPECT_16_10,
    NOVA_ASPECT_21_9,NOVA_ASPECT_32_9,NOVA_ASPECT_CUSTOM
} nova_aspect_ratio_t;

typedef struct {
    uint32_t physical_width,physical_height;
    int32_t logical_width_dlu,logical_height_dlu;
    uint32_t dpi,scale_milli,automatic_scale,user_scale_milli;
    nova_rect_t safe_pixels,safe_dlu;
    nova_resolution_class_t resolution_class;
    nova_aspect_ratio_t aspect_ratio;
    bool dpi_reliable,automatic;
} nova_viewport_t;

typedef struct {
    uint32_t initializations,recalculations,resolution_changes,scale_changes;
    uint32_t dpi_changes,invalid_resolutions,invalid_scales,fallbacks,conversions;
    uint32_t generation;
} nova_resolution_diagnostics_t;

void nova_resolution_initialize(void);
bool nova_resolution_configure(uint32_t width,uint32_t height,uint32_t dpi,
                               bool dpi_reliable);
bool nova_resolution_set_resolution(uint32_t width,uint32_t height);
bool nova_resolution_set_dpi(uint32_t dpi,bool reliable);
bool nova_resolution_set_scale(uint32_t scale_milli);
bool nova_resolution_set_automatic(void);
int32_t nova_logical_to_pixel(int32_t dlu);
int32_t nova_pixel_to_logical(int32_t pixel);
const nova_viewport_t *nova_resolution_viewport(void);
const nova_resolution_diagnostics_t *nova_resolution_diagnostics(void);

#endif
