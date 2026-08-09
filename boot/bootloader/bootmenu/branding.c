#include "branding.h"
#include "resources.h"
#include "boot_art_data.h"

static uint32_t mix(uint32_t background, uint32_t foreground, uint8_t alpha)
{
    uint32_t inverse = 255u - alpha;
    uint32_t red = ((((foreground >> 16) & 255u) * alpha) +
                    (((background >> 16) & 255u) * inverse)) / 255u;
    uint32_t green = ((((foreground >> 8) & 255u) * alpha) +
                      (((background >> 8) & 255u) * inverse)) / 255u;
    uint32_t blue = (((foreground & 255u) * alpha) +
                     ((background & 255u) * inverse)) / 255u;
    return 0xff000000u | (red << 16) | (green << 8) | blue;
}

bool nova_branding_initialize(void)
{
    return nova_resource_register("boot://branding/novaos/logo", NOVA_RESOURCE_IMAGE,
                                  1, nova_logo_data, sizeof(nova_logo_data), 0, 0) &&
           nova_resource_load(nova_resource_id("boot://branding/novaos/logo"));
}

void nova_branding_draw(nova_surface_t *surface, int32_t x, int32_t y,
                        uint16_t width, uint16_t height, uint32_t tint)
{
    nova_rect_t clip={0,0,surface?(int32_t)surface->width:0,
                           surface?(int32_t)surface->height:0};
    nova_branding_draw_clipped(surface,x,y,width,height,tint,clip);
}

void nova_branding_draw_clipped(nova_surface_t *surface,int32_t x,int32_t y,
    uint16_t width,uint16_t height,uint32_t tint,nova_rect_t clip)
{
    if (!surface || !width || !height) return;
    for (uint16_t dy = 0; dy < height; ++dy) for (uint16_t dx = 0; dx < width; ++dx) {
        uint32_t sx = NOVA_LOGO_WIDTH - 1u -
                      ((uint32_t)dx * NOVA_LOGO_WIDTH / width);
        uint32_t sy = (uint32_t)dy * NOVA_LOGO_HEIGHT / height;
        uint32_t pixel = sy * NOVA_LOGO_WIDTH + sx;
        uint8_t coverage = (nova_logo_data[pixel / 4u] >> ((pixel & 3u) * 2u)) & 3u;
        int32_t px = x + dx, py = y + dy;
        if (!coverage || px < clip.x || py < clip.y ||
            px >= clip.x+clip.width || py >= clip.y+clip.height || px < 0 || py < 0 ||
            (uint32_t)px >= surface->width || (uint32_t)py >= surface->height) continue;
        uint32_t *destination = &surface->pixels[py * surface->stride + px];
        *destination = mix(*destination, tint, (uint8_t)(coverage * 85u));
    }
    nova_damage_add(surface, (nova_rect_t){x, y, width, height});
}

uint16_t nova_branding_intrinsic_width(void){return NOVA_LOGO_WIDTH;}
uint16_t nova_branding_intrinsic_height(void){return NOVA_LOGO_HEIGHT;}
