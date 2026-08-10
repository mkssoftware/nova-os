#include "background_blur.h"
#include "clip_mask.h"

#define NOVA_BLUR_MAX_RADIUS 32u
#define NOVA_BLUR_RING_ROWS (NOVA_BLUR_MAX_RADIUS*2u+1u)
#define NOVA_BLUR_CACHE_PIXELS (512u*512u)

static uint32_t result_cache[NOVA_BLUR_CACHE_PIXELS];
static uint32_t horizontal_ring[NOVA_BLUR_RING_ROWS*NOVA_SURFACE_WIDTH];
static nova_blur_diagnostics_t diagnostics;
static uint32_t blur_width,blur_height;
static nova_blur_region_t cached_region;
static nova_round_rect_t cached_geometry;

static bool finite_float(float value)
{union {float f;uint32_t u;} bits={value};return (bits.u&0x7f800000u)!=0x7f800000u;}
static bool valid_rect(nova_rect_t r)
{
    return r.width>0&&r.height>0&&r.x>=0&&r.y>=0&&
        (int64_t)r.x+r.width<=blur_width&&(int64_t)r.y+r.height<=blur_height;
}
static bool same_rect(nova_rect_t a,nova_rect_t b)
{return a.x==b.x&&a.y==b.y&&a.width==b.width&&a.height==b.height;}
static bool intersects(nova_rect_t a,nova_rect_t b)
{return a.x<b.x+b.width&&b.x<a.x+a.width&&a.y<b.y+b.height&&b.y<a.y+a.height;}
static bool same_corners(nova_corner_radius_t a,nova_corner_radius_t b)
{return a.top_left==b.top_left&&a.top_right==b.top_right&&
    a.bottom_right==b.bottom_right&&a.bottom_left==b.bottom_left;}
static uint16_t effective_radius(const nova_blur_region_t *region)
{
    uint16_t radius=(uint16_t)(region->radius+0.5f);
    uint16_t limit=region->quality==NOVA_BLUR_QUALITY_LOW?4:
        region->quality==NOVA_BLUR_QUALITY_STANDARD?16:NOVA_BLUR_MAX_RADIUS;
    if(diagnostics.degraded&&limit>4)limit=4;
    return radius<limit?radius:limit;
}
static bool region_valid(const nova_blur_region_t *region)
{
    if(!region||!valid_rect(region->bounds)||!finite_float(region->radius)||
       region->radius<0||region->radius>NOVA_BLUR_MAX_RADIUS||
       region->quality>NOVA_BLUR_QUALITY_HIGH||
       region->kernel>NOVA_BLUR_KERNEL_DUAL_PASS_GAUSSIAN)return false;
    if(region->rounded_geometry&&
       !same_rect(region->rounded_geometry->bounds,region->bounds))return false;
    return true;
}
bool nova_blur_initialize(uint32_t width,uint32_t height,uint32_t budget)
{
    uint64_t required=(uint64_t)NOVA_BLUR_CACHE_PIXELS*4u+
        (uint64_t)NOVA_BLUR_RING_ROWS*width*4u;
    diagnostics=(nova_blur_diagnostics_t){0};
    if(!width||!height||width>NOVA_SURFACE_WIDTH||height>NOVA_SURFACE_HEIGHT)return false;
    blur_width=width;blur_height=height;diagnostics.initialized=true;
    diagnostics.initializations=1;diagnostics.memory_bytes=(uint32_t)required;
    diagnostics.degraded=budget<required;diagnostics.quality=diagnostics.degraded?
        NOVA_BLUR_QUALITY_LOW:NOVA_BLUR_QUALITY_STANDARD;return true;
}
void nova_blur_shutdown(void)
{if(diagnostics.initialized){diagnostics.initialized=false;diagnostics.cache_valid=false;
    diagnostics.rendering=false;++diagnostics.shutdowns;}}
void nova_blur_set_quality(nova_blur_quality_t quality)
{if(quality<=NOVA_BLUR_QUALITY_HIGH){diagnostics.quality=quality;diagnostics.cache_valid=false;}}
void nova_blur_clear_cache(void){diagnostics.cache_valid=false;}
void nova_blur_invalidate(const nova_rect_t *area)
{
    if(!area||!valid_rect(*area)){++diagnostics.rejections;return;}
    ++diagnostics.invalidations;
    if(diagnostics.cache_valid&&intersects(*area,cached_region.bounds))diagnostics.cache_valid=false;
}
static uint32_t weight(nova_blur_kernel_t kernel,uint16_t radius,int16_t offset)
{
    uint16_t distance=(uint16_t)(offset<0?-offset:offset);
    if(kernel==NOVA_BLUR_KERNEL_BOX||!radius)return 1;
    uint32_t value=radius+1u-distance;
    return kernel==NOVA_BLUR_KERNEL_GAUSSIAN?value*value:value;
}
static uint32_t convolve_horizontal(const nova_blur_region_t *region,
                                    const nova_surface_t *background,
                                    int32_t x,int32_t y,uint16_t radius)
{
    uint64_t a=0,r=0,g=0,b=0,total=0;
    int32_t left=region->bounds.x,right=left+region->bounds.width-1;
    for(int16_t offset=-(int16_t)radius;offset<=(int16_t)radius;++offset){
        int32_t sx=x+offset;
        if(sx<left)sx=left;
        if(sx>right)sx=right;
        uint32_t pixel=background->pixels[(uint32_t)y*background->stride+(uint32_t)sx];
        uint32_t w=weight(region->kernel,radius,offset);total+=w;
        a+=(pixel>>24)*w;r+=((pixel>>16)&255u)*w;
        g+=((pixel>>8)&255u)*w;b+=(pixel&255u)*w;
    }
    return (uint32_t)((a+total/2)/total)<<24|(uint32_t)((r+total/2)/total)<<16|
        (uint32_t)((g+total/2)/total)<<8|(uint32_t)((b+total/2)/total);
}
static uint32_t ring_slot(int32_t requested_row)
{
    int32_t modulo=requested_row%(int32_t)NOVA_BLUR_RING_ROWS;
    if(modulo<0)modulo+=NOVA_BLUR_RING_ROWS;
    return (uint32_t)modulo;
}
static void prepare_horizontal_row(const nova_blur_region_t *region,
                                   const nova_surface_t *background,
                                   int32_t requested,uint16_t radius)
{
    int32_t source_y=requested,top=region->bounds.y;
    int32_t bottom=top+region->bounds.height-1;
    if(source_y<top)source_y=top;
    if(source_y>bottom)source_y=bottom;
    uint32_t *row=&horizontal_ring[ring_slot(requested)*blur_width];
    int32_t right=region->bounds.x+region->bounds.width;
    for(int32_t x=region->bounds.x;x<right;++x)
        row[x]=convolve_horizontal(region,background,x,source_y,radius);
}
static uint32_t convolve_vertical(const nova_blur_region_t *region,int32_t x,int32_t y,
                                  uint16_t radius)
{
    uint64_t a=0,r=0,g=0,b=0,total=0;
    for(int16_t offset=-(int16_t)radius;offset<=(int16_t)radius;++offset){
        uint32_t pixel=horizontal_ring[ring_slot(y+offset)*blur_width+(uint32_t)x];
        uint32_t w=weight(region->kernel,radius,offset);total+=w;
        a+=(pixel>>24)*w;r+=((pixel>>16)&255u)*w;
        g+=((pixel>>8)&255u)*w;b+=(pixel&255u)*w;
    }
    return (uint32_t)((a+total/2)/total)<<24|(uint32_t)((r+total/2)/total)<<16|
        (uint32_t)((g+total/2)/total)<<8|(uint32_t)((b+total/2)/total);
}
static uint8_t combined_coverage(const nova_blur_region_t *region,int32_t x,int32_t y)
{
    uint32_t coverage=nova_mask_coverage(region->mask_scope,x,y);
    if(region->rounded_geometry)coverage=coverage*
        nova_round_rect_coverage(region->rounded_geometry,x,y,NOVA_ROUNDED_AA_HIGH)/255u;
    return (uint8_t)coverage;
}
static void write_result(const nova_blur_region_t *region,nova_surface_t *destination,
                         int32_t x,int32_t y,uint32_t pixel)
{
    uint32_t coverage=combined_coverage(region,x,y);
    if(!coverage)return;
    uint32_t alpha=(pixel>>24)*coverage/255u;
    destination->pixels[(uint32_t)y*destination->stride+(uint32_t)x]=
        (pixel&0x00ffffffu)|(alpha<<24);
}
static bool cache_matches(const nova_blur_region_t *region)
{
    if(!diagnostics.cache_valid||!same_rect(region->bounds,cached_region.bounds)||
       region->quality!=cached_region.quality||region->kernel!=cached_region.kernel||
       region->mask_scope!=cached_region.mask_scope||region->radius!=cached_region.radius||
       (region->rounded_geometry!=0)!=(cached_region.rounded_geometry!=0))return false;
    return !region->rounded_geometry||(same_rect(region->rounded_geometry->bounds,cached_geometry.bounds)&&
        same_corners(region->rounded_geometry->corners,cached_geometry.corners));
}
nova_blur_result_t nova_blur_render(const nova_blur_region_t *region,
    const nova_surface_t *background,nova_surface_t *destination)
{
    if(!diagnostics.initialized)return NOVA_BLUR_NOT_INITIALIZED;
    if(diagnostics.rendering)return NOVA_BLUR_RECURSIVE;
    if(!region_valid(region)||!background||!destination||!background->pixels||
       !destination->pixels||background->width!=blur_width||background->height!=blur_height||
       destination->width!=blur_width||destination->height!=blur_height||
       background->stride<blur_width||destination->stride<blur_width||
       background==destination){
        ++diagnostics.rejections;return NOVA_BLUR_INVALID;}
    diagnostics.rendering=true;++diagnostics.regions;
    int32_t right=region->bounds.x+region->bounds.width;
    int32_t bottom=region->bounds.y+region->bounds.height;
    if(cache_matches(region)){
        uint32_t cache_index=0;
        for(int32_t y=region->bounds.y;y<bottom;++y)
            for(int32_t x=region->bounds.x;x<right;++x,++cache_index){
                destination->pixels[(uint32_t)y*destination->stride+(uint32_t)x]=
                    result_cache[cache_index];
                ++diagnostics.pixels_copied;
            }
        ++diagnostics.cache_hits;diagnostics.rendering=false;
        nova_damage_add(destination,region->bounds);return NOVA_BLUR_OK;
    }
    ++diagnostics.cache_misses;
    uint64_t region_pixels=(uint64_t)region->bounds.width*region->bounds.height;
    diagnostics.pixels_copied+=(uint32_t)region_pixels;
    uint16_t radius=effective_radius(region);diagnostics.last_radius=radius;
    if(radius>diagnostics.maximum_radius)diagnostics.maximum_radius=radius;
    diagnostics.quality=region->quality;diagnostics.kernel=region->kernel;
    for(int32_t requested=region->bounds.y-(int32_t)radius;
        requested<=region->bounds.y+(int32_t)radius;++requested)
        prepare_horizontal_row(region,background,requested,radius);
    for(int32_t y=region->bounds.y;y<bottom;++y){
        if(y>region->bounds.y)prepare_horizontal_row(region,background,y+radius,radius);
        for(int32_t x=region->bounds.x;x<right;++x){
            uint32_t pixel=convolve_vertical(region,x,y,radius);
            write_result(region,destination,x,y,pixel);++diagnostics.pixels_blurred;}}
    if(region_pixels<=NOVA_BLUR_CACHE_PIXELS){uint32_t cache_index=0;
        for(int32_t y=region->bounds.y;y<bottom;++y)
            for(int32_t x=region->bounds.x;x<right;++x,++cache_index)
                result_cache[cache_index]=destination->pixels[(uint32_t)y*destination->stride+x];
        cached_region=*region;if(region->rounded_geometry){cached_geometry=*region->rounded_geometry;
            cached_region.rounded_geometry=&cached_geometry;}
        diagnostics.cache_valid=true;
    }else diagnostics.cache_valid=false;
    diagnostics.rendering=false;
    nova_damage_add(destination,region->bounds);return NOVA_BLUR_OK;
}
const nova_blur_diagnostics_t *nova_blur_diagnostics(void){return &diagnostics;}
