#include "graphics.h"
#include "state_model.h"
#include "resolution.h"

static nova_graphics_context_t context;
static nova_graphics_diagnostics_t diagnostics;

static uint32_t shift_for_mask(uint32_t mask)
{uint32_t shift=0;if(!mask)return 0;while(!(mask&1u)){mask>>=1;++shift;}return shift;}
static uint32_t bits_for_mask(uint32_t mask)
{uint32_t bits=0;while(mask){bits+=mask&1u;mask>>=1;}return bits;}
static uint32_t pack_channel(uint8_t value,uint32_t mask)
{if(!mask)return 0;uint32_t bits=bits_for_mask(mask),maximum=bits>=32?0xffffffffu:((1u<<bits)-1u);
 uint64_t scaled=((uint64_t)value*maximum+127u)/255u;
 return ((uint32_t)(scaled<<shift_for_mask(mask)))&mask;}

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
    if(description->pixel_format==NOVA_PIXEL_BIT_MASK&&
       ((!description->red_mask||!description->green_mask||!description->blue_mask)||
        (description->red_mask&description->green_mask)||
        (description->red_mask&description->blue_mask)||
        (description->green_mask&description->blue_mask)||
        (description->alpha_mask&(description->red_mask|description->green_mask|
                                  description->blue_mask)))){
        ++diagnostics.format_errors;return false;}
    uint32_t bytes=(description->bits_per_pixel+7u)/8u;
    uint64_t minimum_pitch=(uint64_t)description->width*bytes;
    uint64_t required=(uint64_t)description->pitch*description->height;
    if(description->pitch<minimum_pitch||required>description->framebuffer_size){
        ++diagnostics.bounds_errors;return false;}
    context=*description;context.backbuffer=0;context.backbuffer_stride=0;
    if(!context.dpi)context.dpi=96;
    nova_resolution_initialize();
    if(!nova_resolution_configure(context.width,context.height,context.dpi,
                                  context.dpi_reliable)&&
       context.firmware!=NOVA_GRAPHICS_FIRMWARE_TEST)return false;
    if(context.scale_milli){
        if(!nova_resolution_set_scale(context.scale_milli))return false;
    }else context.scale_milli=nova_resolution_viewport()->scale_milli;
    context.capabilities|=NOVA_GRAPHICS_DOUBLE_BUFFER|NOVA_GRAPHICS_BLEND;
    context.initialized=true;++diagnostics.initializations;return true;
}
void nova_graphics_shutdown(void)
{if(context.initialized){context.initialized=false;context.backbuffer=0;++diagnostics.shutdowns;}}
bool nova_graphics_present(const uint32_t *rgba,uint32_t width,uint32_t height,uint32_t stride)
{return nova_graphics_present_regions(rgba,width,height,stride,0,0,true);}

bool nova_graphics_present_regions(const uint32_t *rgba,uint32_t width,uint32_t height,
    uint32_t stride,const nova_graphics_region_t *regions,uint32_t region_count,bool full)
{
    if(!context.initialized||!rgba||width!=context.width||height!=context.height||stride<width||
       nova_state_diagnostics()->phase!=NOVA_STATE_PHASE_PRESENT||
       (!full&&(!regions||!region_count))){++diagnostics.rejected_presents;return false;}
    if(!full)for(uint32_t i=0;i<region_count;++i)if(regions[i].x<0||regions[i].y<0||
       regions[i].width<=0||regions[i].height<=0||
       (uint32_t)regions[i].x>=width||(uint32_t)regions[i].y>=height||
       (uint32_t)regions[i].width>width-(uint32_t)regions[i].x||
       (uint32_t)regions[i].height>height-(uint32_t)regions[i].y){
        ++diagnostics.bounds_errors;++diagnostics.rejected_presents;return false;}
    context.backbuffer=rgba;context.backbuffer_stride=stride;
    uint32_t bytes=(context.bits_per_pixel+7u)/8u,loops=full?1u:region_count;
    for(uint32_t region=0;region<loops;++region){
        int32_t left=full?0:regions[region].x,top=full?0:regions[region].y;
        int32_t right=full?(int32_t)width:left+regions[region].width;
        int32_t bottom=full?(int32_t)height:top+regions[region].height;
        for(int32_t y=top;y<bottom;++y){
            uint8_t *destination=(uint8_t *)context.framebuffer+(uint64_t)y*context.pitch;
            for(int32_t x=left;x<right;++x){uint32_t source=rgba[(uint64_t)y*stride+x];
                uint32_t converted=nova_graphics_convert_pixel(source,context.pixel_format,
                    context.red_mask,context.green_mask,context.blue_mask,context.alpha_mask);
                if(bytes==2)((uint16_t *)destination)[x]=(uint16_t)converted;
                else if(bytes==3){uint8_t *pixel=destination+x*3u;pixel[0]=(uint8_t)converted;
                    pixel[1]=(uint8_t)(converted>>8);pixel[2]=(uint8_t)(converted>>16);}
                else ((uint32_t *)destination)[x]=converted;
                if(context.pixel_format==NOVA_PIXEL_BGRA8888)++diagnostics.direct_pixels;
                else ++diagnostics.converted_pixels;}}
    }
    uint64_t pixel_count=full?(uint64_t)width*height:0;
    if(!full)for(uint32_t i=0;i<region_count;++i)
        pixel_count+=(uint64_t)regions[i].width*regions[i].height;
    ++diagnostics.presents;diagnostics.presented_bytes+=pixel_count*bytes;return true;
}
const nova_graphics_context_t *nova_graphics_context(void){return &context;}
const nova_graphics_diagnostics_t *nova_graphics_diagnostics(void){return &diagnostics;}
uint32_t nova_graphics_width(void){return context.width;}
uint32_t nova_graphics_height(void){return context.height;}
