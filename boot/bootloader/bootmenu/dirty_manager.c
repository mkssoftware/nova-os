#include "dirty_manager.h"

static nova_damage_set_t damage_set;
static nova_dirty_diagnostics_t diagnostics;

static uint64_t area(nova_rect_t rectangle)
{
    return rectangle.width > 0 && rectangle.height > 0 ?
        (uint64_t)(uint32_t)rectangle.width * (uint32_t)rectangle.height : 0;
}

static bool same(nova_rect_t a,nova_rect_t b)
{
    return a.x==b.x&&a.y==b.y&&a.width==b.width&&a.height==b.height;
}

static bool intersects(nova_rect_t a,nova_rect_t b)
{
    int64_t ar=(int64_t)a.x+a.width,ab=(int64_t)a.y+a.height;
    int64_t br=(int64_t)b.x+b.width,bb=(int64_t)b.y+b.height;
    return a.x<br&&b.x<ar&&a.y<bb&&b.y<ab;
}

static nova_rect_t unite(nova_rect_t a,nova_rect_t b)
{
    int32_t x=a.x<b.x?a.x:b.x,y=a.y<b.y?a.y:b.y;
    int32_t ar=a.x+a.width,br=b.x+b.width,ab=a.y+a.height,bb=b.y+b.height;
    int32_t right=ar>br?ar:br,bottom=ab>bb?ab:bb;
    return (nova_rect_t){x,y,right-x,bottom-y};
}

static bool clip(nova_rect_t *rectangle)
{
    int64_t left=rectangle->x,top=rectangle->y;
    int64_t right=left+rectangle->width,bottom=top+rectangle->height;
    if(right<=0||bottom<=0||left>=(int64_t)diagnostics.width||
       top>=(int64_t)diagnostics.height)return false;
    if(left<0)left=0;
    if(top<0)top=0;
    if(right>(int64_t)diagnostics.width)right=diagnostics.width;
    if(bottom>(int64_t)diagnostics.height)bottom=diagnostics.height;
    *rectangle=(nova_rect_t){(int32_t)left,(int32_t)top,
        (int32_t)(right-left),(int32_t)(bottom-top)};
    ++diagnostics.clips;
    return rectangle->width>0&&rectangle->height>0;
}

bool nova_dirty_initialize(uint32_t width,uint32_t height,uint32_t capacity,
                           uint32_t threshold_permille)
{
    damage_set=(nova_damage_set_t){0};
    diagnostics=(nova_dirty_diagnostics_t){0};
    if(!width||!height||capacity==0||capacity>NOVA_DIRTY_MAX_REGIONS||
       threshold_permille==0||threshold_permille>1000)return false;
    diagnostics.width=width;diagnostics.height=height;
    diagnostics.capacity=capacity;
    diagnostics.threshold_permille=threshold_permille;
    diagnostics.initialized=true;
    nova_dirty_force_full(NOVA_DIRTY_FULL_INITIALIZATION);
    return true;
}

void nova_dirty_clear(void)
{
    if(!diagnostics.initialized)return;
    damage_set=(nova_damage_set_t){0};
    diagnostics.full_reason=NOVA_DIRTY_FULL_NONE;
    ++diagnostics.clears;
}

void nova_dirty_force_full(nova_dirty_full_reason_t reason)
{
    if(!diagnostics.initialized||reason==NOVA_DIRTY_FULL_NONE)return;
    if(!damage_set.full_damage)++diagnostics.full_damage_count;
    damage_set.full_damage=true;damage_set.count=0;
    diagnostics.full_reason=reason;
}

static void check_threshold(void)
{
    uint64_t dirty=0;
    for(uint32_t i=0;i<damage_set.count;++i)dirty+=area(damage_set.regions[i]);
    uint64_t screen=(uint64_t)diagnostics.width*diagnostics.height;
    if(dirty*1000u>=screen*diagnostics.threshold_permille){
        ++diagnostics.threshold_promotions;
        nova_dirty_force_full(NOVA_DIRTY_FULL_THRESHOLD);
    }
}

bool nova_dirty_add(const nova_rect_t *requested)
{
    if(!diagnostics.initialized||!requested)return false;
    ++diagnostics.additions;
    if(requested->width<=0||requested->height<=0){
        ++diagnostics.rejected;return false;
    }
    if(damage_set.full_damage)return true;
    nova_rect_t rectangle=*requested;
    if(!clip(&rectangle)){++diagnostics.rejected;return false;}
    for(uint32_t i=0;i<damage_set.count;++i)if(same(damage_set.regions[i],rectangle)){
        ++diagnostics.duplicates;return true;
    }
    for(uint32_t i=0;i<damage_set.count;){
        if(!intersects(damage_set.regions[i],rectangle)){++i;continue;}
        rectangle=unite(damage_set.regions[i],rectangle);
        damage_set.regions[i]=damage_set.regions[--damage_set.count];
        ++diagnostics.merges;i=0;
    }
    if(damage_set.count==diagnostics.capacity){
        ++diagnostics.capacity_promotions;
        nova_dirty_force_full(NOVA_DIRTY_FULL_CAPACITY);
        return true;
    }
    uint64_t rectangle_area=area(rectangle);
    damage_set.regions[damage_set.count++]=rectangle;
    ++diagnostics.accepted;
    ++diagnostics.partial_damage_count;
    diagnostics.total_region_area+=rectangle_area;
    if(rectangle_area>diagnostics.maximum_region_area)
        diagnostics.maximum_region_area=rectangle_area;
    check_threshold();
    return true;
}

bool nova_dirty_add_expanded(const nova_rect_t *requested,uint32_t blur_radius,
                             uint32_t shadow_extent,uint32_t alpha_extent)
{
    if(!requested)return false;
    uint64_t extent=(uint64_t)blur_radius+shadow_extent+alpha_extent;
    if(extent>INT32_MAX)return false;
    int64_t left=(int64_t)requested->x-(int64_t)extent;
    int64_t top=(int64_t)requested->y-(int64_t)extent;
    int64_t right=(int64_t)requested->x+requested->width+(int64_t)extent;
    int64_t bottom=(int64_t)requested->y+requested->height+(int64_t)extent;
    if(left<INT32_MIN||top<INT32_MIN||right>INT32_MAX||bottom>INT32_MAX)
        return false;
    nova_rect_t expanded={(int32_t)left,(int32_t)top,
        (int32_t)(right-left),(int32_t)(bottom-top)};
    ++diagnostics.expansions;
    return nova_dirty_add(&expanded);
}

bool nova_dirty_is_full(void){return damage_set.full_damage;}

bool nova_dirty_intersects(nova_rect_t bounds)
{
    if(damage_set.full_damage)return true;
    for(uint32_t i=0;i<damage_set.count;++i)
        if(intersects(damage_set.regions[i],bounds))return true;
    return false;
}

const nova_damage_set_t *nova_dirty_get(void){return &damage_set;}
const nova_dirty_diagnostics_t *nova_dirty_diagnostics(void){return &diagnostics;}
