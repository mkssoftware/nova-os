#include "graphics.h"
#include "state_model.h"
#include "resolution.h"

static nova_graphics_context_t context;
static nova_graphics_diagnostics_t diagnostics;
static nova_render_reference_result_t reference_results[NOVA_RENDER_REFERENCE_CAPACITY];
static nova_render_reference_summary_t reference_summary;

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
            if(bytes==4&&context.pixel_format==NOVA_PIXEL_BGRA8888){
                uint32_t *target=(uint32_t *)destination+left;
                const uint32_t *source=rgba+(uint64_t)y*stride+left;
                int32_t x=0;
                for(;x+1<right-left;x+=2){target[x]=source[x];target[x+1]=source[x+1];}
                if(x<right-left)target[x]=source[x];
                diagnostics.direct_pixels+=(uint32_t)(right-left);
                continue;
            }
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

bool nova_render_reference_initialize(void)
{
    for(uint8_t i=0;i<NOVA_RENDER_REFERENCE_CAPACITY;++i)
        reference_results[i]=(nova_render_reference_result_t){0};
    reference_summary=(nova_render_reference_summary_t){.initialized=true,
        .isolated=true,.deterministic=true};return true;
}
static uint8_t channel_delta(uint32_t a,uint32_t b,uint8_t shift)
{uint8_t x=(uint8_t)(a>>shift),y=(uint8_t)(b>>shift);return x>y?x-y:y-x;}
static uint8_t pixel_delta(uint32_t a,uint32_t b)
{uint8_t maximum=0;const uint8_t shifts[4]={0,8,16,24};for(uint8_t i=0;i<4;++i){
 uint8_t d=channel_delta(a,b,shifts[i]);if(d>maximum)maximum=d;}return maximum;}
bool nova_render_reference_execute(uint32_t test_id,
    const nova_render_reference_metadata_t *metadata,const uint32_t *reference,
    const uint32_t *actual,const uint8_t *mask,uint32_t stride,
    uint32_t *difference,uint32_t difference_stride)
{
    if(!reference_summary.initialized||reference_summary.count==NOVA_RENDER_REFERENCE_CAPACITY||
       !metadata||!metadata->validated||!metadata->read_only||!metadata->reference_id||
       !metadata->version||!metadata->width||!metadata->height||!metadata->dpi||
       metadata->theme>NOVA_REFERENCE_HIGH_CONTRAST||metadata->profile>NOVA_REFERENCE_GPU||
       !reference||!actual||stride<metadata->width||!difference||
       difference_stride<metadata->width)return false;
    nova_render_reference_result_t result={.test_id=test_id,
        .reference_id=metadata->reference_id,.dimensions_match=true,.alpha_compared=true,
        .masked=mask!=0,.first_difference_x=UINT32_MAX,.first_difference_y=UINT32_MAX};
    uint64_t compared=0;
    for(uint32_t y=0;y<metadata->height;++y)for(uint32_t x=0;x<metadata->width;++x){
        uint64_t index=(uint64_t)y*stride+x,out=(uint64_t)y*difference_stride+x;
        if(mask&&!mask[index]){difference[out]=0;continue;}++compared;
        uint8_t delta=pixel_delta(reference[index],actual[index]);
        if(delta>result.maximum_channel_delta)result.maximum_channel_delta=delta;
        if(delta<=1u){difference[out]=0;continue;}
        bool positioned=false;
        for(int32_t oy=-1;oy<=1&&!positioned;++oy)for(int32_t ox=-1;ox<=1;++ox){
            int32_t px=(int32_t)x+ox,py=(int32_t)y+oy;
            if((ox||oy)&&px>=0&&py>=0&&(uint32_t)px<metadata->width&&
               (uint32_t)py<metadata->height&&
               pixel_delta(reference[(uint64_t)py*stride+(uint32_t)px],actual[index])<=1u){
                positioned=true;result.position_tolerance_used=true;break;}}
        if(positioned){difference[out]=0;continue;}
        difference[out]=0xffff00ffu;++result.differing_pixels;
        if(result.first_difference_x==UINT32_MAX){result.first_difference_x=x;
            result.first_difference_y=y;}
    }
    result.similarity_per_mille=compared?(uint32_t)((compared-result.differing_pixels)*1000u/compared):1000u;
    result.passed=result.differing_pixels==0;
    reference_results[reference_summary.count++]=result;
    if(result.passed)++reference_summary.passed;else ++reference_summary.failed;
    return result.passed;
}
const nova_render_reference_result_t *nova_render_reference_results(void){return reference_results;}
const nova_render_reference_summary_t *nova_render_reference_summary(void){return &reference_summary;}
static bool ref_text(uint8_t *o,uint32_t c,uint32_t *p,const char *s)
{while(*s){if(*p+1>=c)return false;o[(*p)++]=(uint8_t)*s++;}o[*p]=0;return true;}
static bool ref_u32(uint8_t *o,uint32_t c,uint32_t *p,uint32_t v)
{char d[10];uint8_t n=0;do{d[n++]=(char)('0'+v%10u);v/=10u;}while(v&&n<10);
 while(n){if(*p+1>=c)return false;o[(*p)++]=(uint8_t)d[--n];}o[*p]=0;return true;}
bool nova_render_reference_generate_report(bool authorized,uint8_t *output,
                                           uint32_t capacity,uint32_t *written)
{
    if(written) *written=0;
    if(!authorized||!output||capacity<128||!written||
       !reference_summary.initialized)return false;
    uint32_t p=0;
    if(!ref_text(output,capacity,&p,"NOVA_RENDER_REFERENCE_REPORT\ncount=")||
       !ref_u32(output,capacity,&p,reference_summary.count)||!ref_text(output,capacity,&p," passed=")||
       !ref_u32(output,capacity,&p,reference_summary.passed)||!ref_text(output,capacity,&p," failed=")||
       !ref_u32(output,capacity,&p,reference_summary.failed)||!ref_text(output,capacity,&p,"\n"))return false;
    for(uint8_t i=0;i<reference_summary.count;++i){const nova_render_reference_result_t *r=&reference_results[i];
      if(!ref_text(output,capacity,&p,"test=")||!ref_u32(output,capacity,&p,r->test_id)||
         !ref_text(output,capacity,&p," reference=")||!ref_u32(output,capacity,&p,r->reference_id)||
         !ref_text(output,capacity,&p," differing=")||!ref_u32(output,capacity,&p,r->differing_pixels)||
         !ref_text(output,capacity,&p," similarity_pm=")||!ref_u32(output,capacity,&p,r->similarity_per_mille)||
         !ref_text(output,capacity,&p,"\n"))return false;}
    *written=p;++reference_summary.reports;return true;
}
uint32_t nova_graphics_width(void){return context.width;}
uint32_t nova_graphics_height(void){return context.height;}
