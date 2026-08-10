#include "gop_backend.h"

static nova_gop_descriptor_t descriptor;
static nova_gop_diagnostics_t diagnostics;
static bool masks_valid(const nova_gop_mode_candidate_t *mode)
{return mode->red_mask&&mode->green_mask&&mode->blue_mask&&
 !(mode->red_mask&mode->green_mask)&&!(mode->red_mask&mode->blue_mask)&&
 !(mode->green_mask&mode->blue_mask)&&
 !(mode->alpha_mask&(mode->red_mask|mode->green_mask|mode->blue_mask));}
void nova_gop_reset(void)
{descriptor=(nova_gop_descriptor_t){0};diagnostics=(nova_gop_diagnostics_t){0};}
bool nova_gop_validate_candidate(const nova_gop_mode_candidate_t *mode)
{
    if(!mode||!mode->width||!mode->height||mode->pixels_per_scanline<mode->width||
       mode->format>NOVA_GOP_PIXEL_BLT_ONLY||mode->format==NOVA_GOP_PIXEL_BLT_ONLY||
       (mode->format==NOVA_GOP_PIXEL_BIT_MASK&&!masks_valid(mode)))return false;
    return mode->width<=UINT32_MAX/4u&&mode->pixels_per_scanline<=UINT32_MAX/4u;
}
nova_gop_result_t nova_gop_select_mode(const nova_gop_mode_candidate_t *modes,uint32_t count,
    uint32_t current,uint32_t preferred_width,uint32_t preferred_height,uint32_t *selected)
{
    if(!modes||!count||count>NOVA_GOP_MODE_CAPACITY||!selected)return NOVA_GOP_INVALID_MODE;
    diagnostics.candidates+=count;uint32_t valid=0;
    for(uint32_t i=0;i<count;++i)if(nova_gop_validate_candidate(&modes[i]))++valid;
        else ++diagnostics.rejected_modes;
    diagnostics.valid_modes+=valid;if(!valid){++diagnostics.errors;return NOVA_GOP_INVALID_MODE;}
    if(preferred_width&&preferred_height)for(uint32_t i=0;i<count;++i)
        if(modes[i].width==preferred_width&&modes[i].height==preferred_height&&
           nova_gop_validate_candidate(&modes[i])){*selected=modes[i].index;
            ++diagnostics.selections;++diagnostics.preferred_selections;diagnostics.selected_mode=*selected;return NOVA_GOP_OK;}
    if(preferred_width||preferred_height){++diagnostics.errors;return NOVA_GOP_INVALID_MODE;}
    for(uint32_t i=0;i<count;++i)if(modes[i].index==current&&nova_gop_validate_candidate(&modes[i])){
        *selected=current;++diagnostics.selections;++diagnostics.current_selections;
        diagnostics.selected_mode=*selected;return NOVA_GOP_OK;}
    uint32_t best=UINT32_MAX;uint64_t best_area=0;
    for(uint32_t i=0;i<count;++i)if(nova_gop_validate_candidate(&modes[i])){
        uint64_t area=(uint64_t)modes[i].width*modes[i].height;
        if(area>best_area||(area==best_area&&modes[i].index<best)){best_area=area;best=i;}}
    *selected=modes[best].index;++diagnostics.selections;++diagnostics.fallback_selections;
    diagnostics.selected_mode=*selected;return NOVA_GOP_OK;
}
nova_gop_result_t nova_gop_initialize(const nova_gop_mode_candidate_t *active,
    uint32_t mode_count,uint64_t address,uint64_t size)
{
    if(!nova_gop_validate_candidate(active)){++diagnostics.errors;return NOVA_GOP_INVALID_MODE;}
    if(!address){++diagnostics.errors;return NOVA_GOP_INVALID_FRAMEBUFFER;}
    uint64_t pitch=(uint64_t)active->pixels_per_scanline*4u,required=pitch*active->height;
    if(pitch>UINT32_MAX){++diagnostics.errors;return NOVA_GOP_INVALID_PITCH;}
    if(required/active->height!=pitch||size<required){++diagnostics.errors;return NOVA_GOP_FRAMEBUFFER_TOO_SMALL;}
    nova_pixel_format_t format=active->format==NOVA_GOP_PIXEL_RGB_RESERVED?
        NOVA_PIXEL_RGBA8888:active->format==NOVA_GOP_PIXEL_BGR_RESERVED?
        NOVA_PIXEL_BGRA8888:NOVA_PIXEL_BIT_MASK;
    descriptor=(nova_gop_descriptor_t){address,size,active->width,active->height,(uint32_t)pitch,
        4,active->index,mode_count,format,active->red_mask,active->green_mask,
        active->blue_mask,active->alpha_mask};
    diagnostics.available=true;diagnostics.selected_mode=active->index;++diagnostics.initializations;
    return NOVA_GOP_OK;
}
void nova_gop_record_mode_change(void){++diagnostics.mode_changes;}
nova_gop_result_t nova_gop_get_descriptor(nova_gop_descriptor_t *out)
{if(!out)return NOVA_GOP_INVALID_MODE;if(!diagnostics.available)return NOVA_GOP_NOT_READY;
 *out=descriptor;++diagnostics.descriptor_queries;return NOVA_GOP_OK;}
nova_gop_result_t nova_gop_shutdown(void)
{if(!diagnostics.available)return NOVA_GOP_NOT_READY;diagnostics.available=false;++diagnostics.shutdowns;return NOVA_GOP_OK;}
bool nova_gop_is_available(void){return diagnostics.available;}
const nova_gop_diagnostics_t *nova_gop_diagnostics(void){return &diagnostics;}
