#ifndef NOVA_BOOT_BRANDING_H
#define NOVA_BOOT_BRANDING_H

#include <stdint.h>
#include <stdbool.h>
#include "compositor.h"

bool nova_branding_initialize(void);
void nova_branding_draw(nova_surface_t *surface, int32_t x, int32_t y,
                        uint16_t width, uint16_t height, uint32_t tint);
void nova_branding_draw_clipped(nova_surface_t *surface,int32_t x,int32_t y,
    uint16_t width,uint16_t height,uint32_t tint,nova_rect_t clip);
uint16_t nova_branding_intrinsic_width(void);
uint16_t nova_branding_intrinsic_height(void);

#endif
