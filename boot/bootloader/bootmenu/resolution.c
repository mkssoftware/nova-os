#include "resolution.h"

static nova_viewport_t viewport;
static nova_resolution_diagnostics_t diagnostics;

static uint32_t clamp_scale(uint32_t value)
{if(value<1000)return 1000;if(value>3000)return 3000;return value;}
static uint32_t quantize_scale(uint32_t value)
{value=clamp_scale(value);return ((value+125u)/250u)*250u;}
static uint32_t automatic_scale(uint32_t width,uint32_t height,uint32_t dpi,bool reliable)
{
    if(reliable&&dpi>=48&&dpi<=768)return quantize_scale((dpi*1000u+48u)/96u);
    uint32_t horizontal=(uint32_t)(((uint64_t)width*1000u+960u)/1920u);
    uint32_t vertical=(uint32_t)(((uint64_t)height*1000u+540u)/1080u);
    return quantize_scale(horizontal<vertical?horizontal:vertical);
}
static nova_resolution_class_t classify(uint32_t width,uint32_t height)
{
    (void)height;
    if(width<=640)return NOVA_RESOLUTION_XS;
    if(width<=800)return NOVA_RESOLUTION_S;
    if(width<=1024)return NOVA_RESOLUTION_M;
    if(width<1920)return NOVA_RESOLUTION_L;
    if(width<2560)return NOVA_RESOLUTION_XL;
    if(width<3840)return NOVA_RESOLUTION_XXL;
    return NOVA_RESOLUTION_ULTRA;
}
static uint64_t difference(uint64_t left,uint64_t right)
{return left>right?left-right:right-left;}
static nova_aspect_ratio_t aspect(uint32_t width,uint32_t height)
{
    static const uint8_t ratios[][2]={{4,3},{5,4},{16,9},{16,10},{21,9},{32,9}};
    uint64_t best=UINT64_MAX;uint8_t index=0;
    for(uint8_t i=0;i<6;++i){uint64_t d=difference((uint64_t)width*ratios[i][1],
                                                  (uint64_t)height*ratios[i][0]);
        if(d<best){best=d;index=i;}}
    uint64_t base=(uint64_t)height*ratios[index][0];
    return best*100u<=base*3u?(nova_aspect_ratio_t)index:NOVA_ASPECT_CUSTOM;
}
static void recalculate(void)
{
    uint32_t scale=viewport.automatic?viewport.automatic_scale:viewport.user_scale_milli;
    viewport.scale_milli=clamp_scale(scale);
    viewport.logical_width_dlu=(int32_t)(((uint64_t)viewport.physical_width*1000u+
                                          viewport.scale_milli/2u)/viewport.scale_milli);
    viewport.logical_height_dlu=(int32_t)(((uint64_t)viewport.physical_height*1000u+
                                           viewport.scale_milli/2u)/viewport.scale_milli);
    uint32_t margin_x=viewport.physical_width*3u/100u;
    uint32_t margin_y=viewport.physical_height*3u/100u;
    if(margin_x<12)margin_x=12;
    if(margin_y<10)margin_y=10;
    viewport.safe_pixels=(nova_rect_t){(int32_t)margin_x,(int32_t)margin_y,
        (int32_t)(viewport.physical_width-margin_x*2u),
        (int32_t)(viewport.physical_height-margin_y*2u)};
    viewport.safe_dlu=(nova_rect_t){nova_pixel_to_logical(viewport.safe_pixels.x),
        nova_pixel_to_logical(viewport.safe_pixels.y),
        nova_pixel_to_logical(viewport.safe_pixels.width),
        nova_pixel_to_logical(viewport.safe_pixels.height)};
    viewport.resolution_class=classify(viewport.physical_width,viewport.physical_height);
    viewport.aspect_ratio=aspect(viewport.physical_width,viewport.physical_height);
    ++diagnostics.recalculations;++diagnostics.generation;
}
void nova_resolution_initialize(void)
{
    diagnostics=(nova_resolution_diagnostics_t){.initializations=1};
    viewport=(nova_viewport_t){.physical_width=1920,.physical_height=1080,.dpi=96,
        .automatic_scale=1000,.user_scale_milli=1000,.automatic=true};recalculate();
}
bool nova_resolution_configure(uint32_t width,uint32_t height,uint32_t dpi,bool reliable)
{
    if(width<320||height<240||width>7680||height>4320){++diagnostics.invalid_resolutions;return false;}
    if(dpi<48||dpi>768){dpi=96;reliable=false;++diagnostics.fallbacks;}
    viewport.physical_width=width;viewport.physical_height=height;
    viewport.dpi=dpi;viewport.dpi_reliable=reliable;viewport.automatic=true;
    viewport.automatic_scale=automatic_scale(width,height,dpi,reliable);recalculate();return true;
}
bool nova_resolution_set_resolution(uint32_t width,uint32_t height)
{
    if(width<320||height<240||width>7680||height>4320){++diagnostics.invalid_resolutions;return false;}
    if(width==viewport.physical_width&&height==viewport.physical_height)return true;
    viewport.physical_width=width;viewport.physical_height=height;
    viewport.automatic_scale=automatic_scale(width,height,viewport.dpi,viewport.dpi_reliable);
    ++diagnostics.resolution_changes;recalculate();return true;
}
bool nova_resolution_set_dpi(uint32_t dpi,bool reliable)
{
    if(dpi<48||dpi>768){++diagnostics.fallbacks;dpi=96;reliable=false;}
    if(dpi==viewport.dpi&&reliable==viewport.dpi_reliable)return true;
    viewport.dpi=dpi;viewport.dpi_reliable=reliable;
    viewport.automatic_scale=automatic_scale(viewport.physical_width,
        viewport.physical_height,dpi,reliable);++diagnostics.dpi_changes;recalculate();return true;
}
bool nova_resolution_set_scale(uint32_t scale_milli)
{
    if(scale_milli<1000||scale_milli>3000){++diagnostics.invalid_scales;return false;}
    viewport.user_scale_milli=scale_milli;viewport.automatic=false;
    ++diagnostics.scale_changes;recalculate();return true;
}
bool nova_resolution_set_automatic(void)
{if(viewport.automatic)return true;viewport.automatic=true;++diagnostics.scale_changes;recalculate();return true;}
int32_t nova_logical_to_pixel(int32_t dlu)
{
    int64_t value=(int64_t)dlu*viewport.scale_milli;++diagnostics.conversions;
    return value>=0?(int32_t)((value+500)/1000):(int32_t)((value-500)/1000);
}
int32_t nova_pixel_to_logical(int32_t pixel)
{
    if(!viewport.scale_milli)return pixel;
    int64_t value=(int64_t)pixel*1000;
    ++diagnostics.conversions;
    return value>=0?(int32_t)((value+viewport.scale_milli/2)/viewport.scale_milli):
                    (int32_t)((value-viewport.scale_milli/2)/viewport.scale_milli);
}
const nova_viewport_t *nova_resolution_viewport(void){return &viewport;}
const nova_resolution_diagnostics_t *nova_resolution_diagnostics(void){return &diagnostics;}
