#include "framebuffer_backend.h"
#include "state_model.h"

static nova_framebuffer_info_t info;
static nova_framebuffer_diagnostics_t diagnostics;
static nova_graphics_region_t damage[NOVA_FB_DAMAGE_CAPACITY];
static uint8_t damage_count;
static bool full_damage,frame_active;

static nova_fb_result_t fail(nova_fb_result_t error)
{diagnostics.last_error=error;return error;}
static bool rectangle_valid(nova_rect_t r)
{return r.width>0&&r.height>0&&r.x>=0&&r.y>=0&&
 (uint32_t)r.x<info.width&&(uint32_t)r.y<info.height&&
 (uint32_t)r.width<=info.width-(uint32_t)r.x&&
 (uint32_t)r.height<=info.height-(uint32_t)r.y;}
static uint32_t bytes_per_pixel(void){return (info.bits_per_pixel+7u)/8u;}
nova_fb_result_t nova_framebuffer_initialize(void)
{
    diagnostics=(nova_framebuffer_diagnostics_t){0};info=(nova_framebuffer_info_t){0};
    const nova_graphics_context_t *graphics=nova_graphics_context();
    if(!graphics||!graphics->initialized||!graphics->framebuffer){++diagnostics.mapping_errors;return fail(NOVA_FB_INVALID_ADDRESS);}
    uint32_t bytes=(graphics->bits_per_pixel+7u)/8u;
    if(graphics->pitch<(uint64_t)graphics->width*bytes){++diagnostics.mapping_errors;return fail(NOVA_FB_INVALID_PITCH);}
    if((uint64_t)graphics->pitch*graphics->height>graphics->framebuffer_size){++diagnostics.mapping_errors;return fail(NOVA_FB_MAPPING_FAILED);}
    if(graphics->pixel_format>NOVA_PIXEL_BIT_MASK){++diagnostics.format_errors;return fail(NOVA_FB_UNSUPPORTED_FORMAT);}
    nova_fb_backend_type_t backend=graphics->firmware==NOVA_GRAPHICS_FIRMWARE_BIOS_VBE?
        NOVA_FB_BACKEND_VBE:graphics->firmware==NOVA_GRAPHICS_FIRMWARE_UEFI_GOP?
        NOVA_FB_BACKEND_GOP:NOVA_FB_BACKEND_TEST;
    info=(nova_framebuffer_info_t){backend,graphics->width,graphics->height,graphics->pitch,
        graphics->bits_per_pixel,graphics->pixel_format,graphics->framebuffer,
        graphics->framebuffer_size,graphics->framebuffer_address,NOVA_FB_DOUBLE_BUFFER,
        graphics->capabilities,true,true};
    damage_count=0;full_damage=true;frame_active=false;++diagnostics.initializations;
    return NOVA_FB_OK;
}
nova_fb_result_t nova_framebuffer_begin_frame(void)
{if(!info.initialized||frame_active)return fail(NOVA_FB_INVALID_STATE);
 frame_active=true;damage_count=0;full_damage=false;++diagnostics.frames;return NOVA_FB_OK;}
nova_fb_result_t nova_framebuffer_damage(nova_rect_t region)
{
    if(!frame_active||!rectangle_valid(region)){++diagnostics.bounds_errors;return fail(NOVA_FB_BOUNDS_ERROR);}
    if(damage_count==NOVA_FB_DAMAGE_CAPACITY){full_damage=true;++diagnostics.damage_overflows;return NOVA_FB_OK;}
    for(uint8_t i=0;i<damage_count;++i){nova_graphics_region_t *r=&damage[i];int32_t right=r->x+r->width;
        int32_t bottom=r->y+r->height,rright=region.x+region.width,rbottom=region.y+region.height;
        if(region.x<=right&&region.y<=bottom&&rright>=r->x&&rbottom>=r->y){
            int32_t x=region.x<r->x?region.x:r->x,y=region.y<r->y?region.y:r->y;
            int32_t max_right=rright>right?rright:right,max_bottom=rbottom>bottom?rbottom:bottom;
            *r=(nova_graphics_region_t){x,y,max_right-x,max_bottom-y};++diagnostics.damage_regions;return NOVA_FB_OK;}}
    damage[damage_count++]=(nova_graphics_region_t){region.x,region.y,region.width,region.height};
    ++diagnostics.damage_regions;return NOVA_FB_OK;
}
nova_fb_result_t nova_framebuffer_present(const uint32_t *pixels,uint32_t width,
    uint32_t height,uint32_t stride)
{
    if(!info.initialized||!frame_active||!pixels||width!=info.width||height!=info.height||stride<width||
       nova_state_diagnostics()->phase!=NOVA_STATE_PHASE_PRESENT){++diagnostics.rejected_presents;
        frame_active=false;return fail(NOVA_FB_INVALID_STATE);}
    if(!damage_count)full_damage=true;
    bool result=nova_graphics_present_regions(pixels,width,height,stride,damage,damage_count,full_damage);
    frame_active=false;if(!result){++diagnostics.rejected_presents;return fail(NOVA_FB_PRESENT_FAILED);}
    ++diagnostics.presents;if(full_damage)++diagnostics.full_presents;else ++diagnostics.partial_presents;
    uint64_t written=full_damage?(uint64_t)info.pitch*info.height:0;
    if(!full_damage)for(uint8_t i=0;i<damage_count;++i)
        written+=(uint64_t)bytes_per_pixel()*damage[i].width*damage[i].height;
    diagnostics.bytes_written+=written;
    diagnostics.last_error=NOVA_FB_OK;return NOVA_FB_OK;
}
static nova_fb_result_t address_of(int32_t x,int32_t y,uint8_t **address)
{if(!info.initialized||!address||x<0||y<0||(uint32_t)x>=info.width||(uint32_t)y>=info.height){
 ++diagnostics.bounds_errors;return fail(NOVA_FB_BOUNDS_ERROR);}uint64_t offset=(uint64_t)y*info.pitch+(uint64_t)x*bytes_per_pixel();
 if(offset+bytes_per_pixel()>info.framebuffer_size){++diagnostics.bounds_errors;return fail(NOVA_FB_BOUNDS_ERROR);}
 *address=(uint8_t *)info.framebuffer+offset;return NOVA_FB_OK;}
nova_fb_result_t nova_framebuffer_write_pixel(int32_t x,int32_t y,uint32_t color)
{uint8_t *address;if(address_of(x,y,&address)!=NOVA_FB_OK)return NOVA_FB_BOUNDS_ERROR;
 const nova_graphics_context_t *g=nova_graphics_context();uint32_t value=nova_graphics_convert_pixel(color,
 info.format,g->red_mask,g->green_mask,g->blue_mask,g->alpha_mask),bytes=bytes_per_pixel();
 if(bytes==2)*(uint16_t *)address=(uint16_t)value;else if(bytes==3){address[0]=(uint8_t)value;
 address[1]=(uint8_t)(value>>8);address[2]=(uint8_t)(value>>16);}else *(uint32_t *)address=value;
 ++diagnostics.pixel_writes;diagnostics.bytes_written+=bytes;return NOVA_FB_OK;}
static uint32_t expand565(uint16_t v)
{uint32_t r=(v>>11)&31u,g=(v>>5)&63u,b=v&31u;r=(r<<3)|(r>>2);g=(g<<2)|(g>>4);b=(b<<3)|(b>>2);
 return 0xff000000u|(r<<16)|(g<<8)|b;}
nova_fb_result_t nova_framebuffer_read_pixel(int32_t x,int32_t y,uint32_t *color)
{uint8_t *address;if(!color)return fail(NOVA_FB_INVALID_ARGUMENT);if(address_of(x,y,&address)!=NOVA_FB_OK)return NOVA_FB_BOUNDS_ERROR;
 uint32_t bytes=bytes_per_pixel(),value=bytes==2?*(uint16_t *)address:bytes==3?
 ((uint32_t)address[0]|((uint32_t)address[1]<<8)|((uint32_t)address[2]<<16)):*(uint32_t *)address;
 if(info.format==NOVA_PIXEL_RGB565)*color=expand565((uint16_t)value);
 else if(info.format==NOVA_PIXEL_RGBA8888)*color=(value&0xff00ff00u)|((value&0xffu)<<16)|((value>>16)&0xffu);
 else *color=value|0xff000000u;
 ++diagnostics.pixel_reads;return NOVA_FB_OK;}
nova_fb_result_t nova_framebuffer_horizontal_line(int32_t x,int32_t y,int32_t width,uint32_t color)
{if(!rectangle_valid((nova_rect_t){x,y,width,1}))return fail(NOVA_FB_BOUNDS_ERROR);
 for(int32_t i=0;i<width;++i)if(nova_framebuffer_write_pixel(x+i,y,color)!=NOVA_FB_OK)return diagnostics.last_error;
 ++diagnostics.line_writes;return NOVA_FB_OK;}
nova_fb_result_t nova_framebuffer_vertical_line(int32_t x,int32_t y,int32_t height,uint32_t color)
{if(!rectangle_valid((nova_rect_t){x,y,1,height}))return fail(NOVA_FB_BOUNDS_ERROR);
 for(int32_t i=0;i<height;++i)if(nova_framebuffer_write_pixel(x,y+i,color)!=NOVA_FB_OK)return diagnostics.last_error;
 ++diagnostics.line_writes;return NOVA_FB_OK;}
nova_fb_result_t nova_framebuffer_rectangle(nova_rect_t rect,uint32_t color)
{
 if(!rectangle_valid(rect))return fail(NOVA_FB_BOUNDS_ERROR);
 for(int32_t y=0;y<rect.height;++y)
  if(nova_framebuffer_horizontal_line(rect.x,rect.y+y,rect.width,color)!=NOVA_FB_OK)
   return diagnostics.last_error;
 ++diagnostics.rectangle_writes;return NOVA_FB_OK;
}
nova_fb_result_t nova_framebuffer_copy(nova_rect_t destination,const uint32_t *source,uint32_t stride)
{if(!source||stride<(uint32_t)destination.width||!rectangle_valid(destination))return fail(NOVA_FB_INVALID_ARGUMENT);
 for(int32_t y=0;y<destination.height;++y)for(int32_t x=0;x<destination.width;++x)
  if(nova_framebuffer_write_pixel(destination.x+x,destination.y+y,source[(uint64_t)y*stride+x])!=NOVA_FB_OK)return diagnostics.last_error;
 ++diagnostics.block_copies;return NOVA_FB_OK;}
void nova_framebuffer_shutdown(void)
{if(info.initialized){info.initialized=false;frame_active=false;++diagnostics.shutdowns;}}
const nova_framebuffer_info_t *nova_framebuffer_info(void){return &info;}
const nova_framebuffer_diagnostics_t *nova_framebuffer_diagnostics(void){return &diagnostics;}
