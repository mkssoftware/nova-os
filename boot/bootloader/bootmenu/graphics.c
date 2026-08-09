#include "graphics.h"
#include "state_model.h"

static nova_graphics_context_t context;
static nova_graphics_diagnostics_t diagnostics;

static uint32_t scale_for_width(uint32_t width)
{
    if(width>=3840)return 2000;
    if(width>=2560)return 1750;
    if(width>=1920)return 1500;
    if(width>=1366)return 1250;
    return 1000;
}
static uint32_t shift_for_mask(uint32_t mask)
{uint32_t shift=0;if(!mask)return 0;while(!(mask&1u)){mask>>=1;++shift;}return shift;}
static uint32_t bits_for_mask(uint32_t mask)
{uint32_t bits=0;while(mask){bits+=mask&1u;mask>>=1;}return bits;}
static uint32_t pack_channel(uint8_t value,uint32_t mask)
{if(!mask)return 0;uint32_t bits=bits_for_mask(mask),maximum=bits>=32?0xffffffffu:((1u<<bits)-1u);
 return (((uint32_t)value*maximum+127u)/255u<<shift_for_mask(mask))&mask;}

uint32_t nova_graphics_convert_pixel(uint32_t rgba,nova_pixel_format_t format,
                                    uint32_t red_mask,uint32_t green_mask,
                                    uint32_t blue_mask,uint32_t alpha_mask)
{
    uint8_t a=(uint8_t)(rgba>>24),r=(uint8_t)(rgba>>16),g=(uint8_t)(rgba>>8),b=(uint8_t)rgba;
    switch(format){
    case NOVA_PIXEL_BGRA8888:return rgba;
    case NOVA_PIXEL_RGBA8888:return ((uint32_t)a<<24)|((uint32_t)b<<16)|((uint32_t)g<<8)|r;
    case NOVA_PIXEL_BGR888:return ((uint32_t)r<<16)|((uint32_t)g<<8)|b;
    case NOVA_PIXEL_RGB888:return ((uint32_t)b<<16)|((uint32_t)g<<8)|r;
    case NOVA_PIXEL_RGB565:return ((uint32_t)(r>>3)<<11)|((uint32_t)(g>>2)<<5)|(b>>3);
    case NOVA_PIXEL_BIT_MASK:return pack_channel(r,red_mask)|pack_channel(g,green_mask)|
        pack_channel(b,blue_mask)|pack_channel(a,alpha_mask);
    default:return 0;
    }
}
bool nova_graphics_initialize(const nova_graphics_context_t *description)
{
    diagnostics=(nova_graphics_diagnostics_t){0};context=(nova_graphics_context_t){0};
    if(!description||!description->framebuffer||!description->width||!description->height||
       (description->bits_per_pixel!=16&&description->bits_per_pixel!=24&&
        description->bits_per_pixel!=32)||description->pixel_format>NOVA_PIXEL_BIT_MASK){
        ++diagnostics.format_errors;return false;}
    uint32_t bytes=(description->bits_per_pixel+7u)/8u;
    uint64_t minimum_pitch=(uint64_t)description->width*bytes;
    uint64_t required=(uint64_t)description->pitch*description->height;
    if(description->pitch<minimum_pitch||required>description->framebuffer_size){
        ++diagnostics.bounds_errors;return false;}
    context=*description;context.backbuffer=0;context.backbuffer_stride=0;
    if(!context.dpi)context.dpi=96;
    if(!context.scale_milli)context.scale_milli=scale_for_width(context.width);
    context.capabilities|=NOVA_GRAPHICS_DOUBLE_BUFFER|NOVA_GRAPHICS_BLEND;
    context.initialized=true;++diagnostics.initializations;return true;
}
void nova_graphics_shutdown(void)
{if(context.initialized){context.initialized=false;context.backbuffer=0;++diagnostics.shutdowns;}}
bool nova_graphics_present(const uint32_t *rgba,uint32_t width,uint32_t height,uint32_t stride)
{
    if(!context.initialized||!rgba||width!=context.width||height!=context.height||stride<width||
       nova_state_diagnostics()->phase!=NOVA_STATE_PHASE_PRESENT){
        ++diagnostics.rejected_presents;return false;}
    context.backbuffer=rgba;context.backbuffer_stride=stride;
    uint32_t bytes=(context.bits_per_pixel+7u)/8u;
    for(uint32_t y=0;y<height;++y){uint8_t *destination=(uint8_t *)context.framebuffer+(uint64_t)y*context.pitch;
        for(uint32_t x=0;x<width;++x){uint32_t source=rgba[(uint64_t)y*stride+x];
            uint32_t converted=nova_graphics_convert_pixel(source,context.pixel_format,
                context.red_mask,context.green_mask,context.blue_mask,context.alpha_mask);
            if(bytes==2)((uint16_t *)destination)[x]=(uint16_t)converted;
            else if(bytes==3){uint8_t *pixel=destination+x*3u;pixel[0]=(uint8_t)converted;
                pixel[1]=(uint8_t)(converted>>8);pixel[2]=(uint8_t)(converted>>16);}
            else ((uint32_t *)destination)[x]=converted;
            if(context.pixel_format==NOVA_PIXEL_BGRA8888)++diagnostics.direct_pixels;
            else ++diagnostics.converted_pixels;}}
    ++diagnostics.presents;diagnostics.presented_bytes+=(uint64_t)width*height*bytes;return true;
}
const nova_graphics_context_t *nova_graphics_context(void){return &context;}
const nova_graphics_diagnostics_t *nova_graphics_diagnostics(void){return &diagnostics;}
uint32_t nova_graphics_width(void){return context.width;}
uint32_t nova_graphics_height(void){return context.height;}
