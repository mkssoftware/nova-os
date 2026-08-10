#include "branding.h"
#include "resources.h"
#include "boot_art_data.h"
#include "image_renderer.h"
#include "render_quality.h"

static nova_image_t *branding_image;

bool nova_branding_initialize(void)
{
    uint64_t id=nova_resource_id("boot://branding/novaos/logo");
    return nova_resource_register("boot://branding/novaos/logo", NOVA_RESOURCE_IMAGE,
                                  1, nova_logo_data, sizeof(nova_logo_data), 0, 0) &&
           nova_resource_cache_set_policy(id,NOVA_CACHE_PERMANENT) &&
           nova_resource_load(id)&&
           nova_image_create_2bit_alpha(id,NOVA_LOGO_WIDTH,NOVA_LOGO_HEIGHT,
               nova_logo_data,sizeof(nova_logo_data),&branding_image)==NOVA_IMAGE_OK;
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
    if(!surface||!branding_image||!width||!height)return;
    nova_image_render_options_t options={.destination={x,y,width,height},.clip=clip,
        .sampling=width==branding_image->width&&height==branding_image->height?
            NOVA_IMAGE_SAMPLE_NEAREST:nova_render_quality_parameters()->image_sampling,
        .transform=nova_transform_fixed_identity(),
        .opacity=1000,.tint=tint,.mirror_x=true};
    (void)nova_image_render(branding_image,surface,&options);
}

uint16_t nova_branding_intrinsic_width(void){return NOVA_LOGO_WIDTH;}
uint16_t nova_branding_intrinsic_height(void){return NOVA_LOGO_HEIGHT;}
