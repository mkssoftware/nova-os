#include "clip_mask.h"

typedef struct {
    nova_rect_t effective;
    uint16_t parent;
    bool active;
} clip_scope_t;

typedef struct {
    nova_mask_descriptor_t descriptor;
    uint16_t generation;
    bool active,in_query;
} mask_slot_t;

typedef struct {
    nova_mask_handle_t mask;
    uint16_t parent;
    bool active;
} mask_scope_t;

typedef struct {
    nova_mask_handle_t mask;
    int32_t x,y;
    uint8_t coverage;
    bool valid;
} mask_cache_entry_t;

static clip_scope_t clip_scopes[NOVA_CLIP_SCOPE_CAPACITY];
static uint16_t clip_stack[NOVA_CLIP_STACK_CAPACITY],clip_depth,clip_used;
static mask_slot_t masks[NOVA_MASK_CAPACITY];
static mask_scope_t mask_scopes[NOVA_MASK_SCOPE_CAPACITY];
static uint16_t mask_stack[NOVA_MASK_STACK_CAPACITY],mask_depth,mask_scope_used;
static mask_cache_entry_t cache[NOVA_MASK_CACHE_CAPACITY];
static uint32_t screen_width,screen_height;
static nova_clip_mask_diagnostics_t diagnostics;

static bool rect_valid(nova_rect_t r){return r.width>0&&r.height>0;}

static nova_rect_t intersection(nova_rect_t a,nova_rect_t b)
{
    int64_t left=a.x>b.x?a.x:b.x,top=a.y>b.y?a.y:b.y;
    int64_t ar=(int64_t)a.x+a.width,br=(int64_t)b.x+b.width;
    int64_t ab=(int64_t)a.y+a.height,bb=(int64_t)b.y+b.height;
    int64_t right=ar<br?ar:br,bottom=ab<bb?ab:bb;
    if(right<=left||bottom<=top||left<INT32_MIN||top<INT32_MIN||
       right>INT32_MAX||bottom>INT32_MAX)return (nova_rect_t){0};
    return (nova_rect_t){(int32_t)left,(int32_t)top,
        (int32_t)(right-left),(int32_t)(bottom-top)};
}

bool nova_clip_mask_initialize(uint32_t width,uint32_t height)
{
    diagnostics=(nova_clip_mask_diagnostics_t){0};
    if(!width||!height||width>INT32_MAX||height>INT32_MAX)return false;
    screen_width=width;screen_height=height;
    for(uint16_t i=0;i<NOVA_MASK_CAPACITY;++i){
        masks[i]=(mask_slot_t){0};masks[i].generation=1;
    }
    for(uint16_t i=0;i<NOVA_MASK_CACHE_CAPACITY;++i)cache[i]=(mask_cache_entry_t){0};
    diagnostics.initialized=true;++diagnostics.initializations;
    return true;
}

bool nova_clip_mask_begin_frame(void)
{
    if(!diagnostics.initialized||diagnostics.frame_active)return false;
    clip_depth=1;clip_used=1;
    clip_scopes[0]=(clip_scope_t){{0,0,(int32_t)screen_width,
        (int32_t)screen_height},0,true};
    clip_stack[0]=0;
    mask_depth=mask_scope_used=0;
    diagnostics.frame_active=true;++diagnostics.frames;
    if(diagnostics.clip_depth_peak<1)diagnostics.clip_depth_peak=1;
    return true;
}

bool nova_clip_mask_end_frame(void)
{
    if(!diagnostics.frame_active||clip_depth!=1||mask_depth)return false;
    diagnostics.frame_active=false;return true;
}
void nova_clip_mask_abort_frame(void)
{
    diagnostics.frame_active=false;clip_depth=mask_depth=0;
    clip_used=mask_scope_used=0;
}

bool nova_clip_push(nova_rect_t rectangle)
{
    if(!diagnostics.frame_active||!rect_valid(rectangle)){
        ++diagnostics.clip_rejections;return false;
    }
    if(clip_depth==NOVA_CLIP_STACK_CAPACITY||clip_used==NOVA_CLIP_SCOPE_CAPACITY){
        ++diagnostics.clip_stack_overflows;return false;
    }
    uint16_t parent=clip_stack[clip_depth-1];
    nova_rect_t effective=intersection(clip_scopes[parent].effective,rectangle);
    clip_scopes[clip_used]=(clip_scope_t){effective,parent,true};
    clip_stack[clip_depth++]=clip_used++;
    ++diagnostics.clip_pushes;
    if(clip_depth>diagnostics.clip_depth_peak)diagnostics.clip_depth_peak=clip_depth;
    return true;
}

bool nova_clip_pop(void)
{
    if(!diagnostics.frame_active||clip_depth<=1){
        ++diagnostics.clip_rejections;return false;
    }
    --clip_depth;++diagnostics.clip_pops;return true;
}

nova_rect_t nova_clip_current(void)
{
    return diagnostics.frame_active?
        clip_scopes[clip_stack[clip_depth-1]].effective:(nova_rect_t){0};
}

uint16_t nova_clip_current_scope(void)
{
    return diagnostics.frame_active?(uint16_t)(clip_stack[clip_depth-1]+1u):0;
}

bool nova_clip_scope_get(uint16_t scope,nova_rect_t *rectangle)
{
    if(!scope||scope>clip_used||!rectangle||!clip_scopes[scope-1].active)return false;
    *rectangle=clip_scopes[scope-1].effective;return true;
}

nova_clip_visibility_t nova_clip_test(uint16_t scope,nova_rect_t bounds,
                                      nova_rect_t *visible)
{
    ++diagnostics.clip_tests;
    nova_rect_t clip;
    if(!rect_valid(bounds)||!nova_clip_scope_get(scope,&clip)){
        ++diagnostics.fully_clipped;
        if(visible)*visible=(nova_rect_t){0};
        return NOVA_CLIP_OUTSIDE;
    }
    nova_rect_t result=intersection(clip,bounds);
    if(visible)*visible=result;
    if(!rect_valid(result)){++diagnostics.fully_clipped;return NOVA_CLIP_OUTSIDE;}
    if(result.x==bounds.x&&result.y==bounds.y&&result.width==bounds.width&&
       result.height==bounds.height)return NOVA_CLIP_INSIDE;
    ++diagnostics.partially_clipped;return NOVA_CLIP_PARTIAL;
}

static mask_slot_t *mask_from_handle(nova_mask_handle_t handle)
{
    uint16_t index=(uint16_t)(handle&0xffffu),generation=(uint16_t)(handle>>16);
    if(!index||index>NOVA_MASK_CAPACITY)return 0;
    mask_slot_t *slot=&masks[index-1];
    return slot->active&&slot->generation==generation?slot:0;
}

static bool descriptor_valid(const nova_mask_descriptor_t *d)
{
    if(!d||d->type>NOVA_MASK_ROUNDED_RECTANGLE||!rect_valid(d->bounds))return false;
    int64_t right=(int64_t)d->bounds.x+d->bounds.width;
    int64_t bottom=(int64_t)d->bounds.y+d->bounds.height;
    if(right>INT32_MAX||bottom>INT32_MAX)return false;
    uint64_t width=(uint32_t)d->bounds.width,height=(uint32_t)d->bounds.height;
    if(d->type==NOVA_MASK_BITMAP){
        uint64_t minimum=(width+7u)/8u;
        return d->pixels&&d->stride>=minimum&&
            (uint64_t)d->stride*height<=d->size;
    }
    if(d->type==NOVA_MASK_ALPHA)
        return d->pixels&&d->stride>=width&&(uint64_t)d->stride*height<=d->size;
    if(d->type==NOVA_MASK_VECTOR)return d->vector_callback!=0;
    if(d->type==NOVA_MASK_ROUNDED_RECTANGLE)
        return d->radius<=(uint16_t)((d->bounds.width<d->bounds.height?
            d->bounds.width:d->bounds.height)/2);
    return true;
}

bool nova_mask_create(const nova_mask_descriptor_t *descriptor,
                      nova_mask_handle_t *handle)
{
    if(!diagnostics.initialized||!handle||!descriptor_valid(descriptor)){
        ++diagnostics.invalid_masks;return false;
    }
    for(uint16_t i=0;i<NOVA_MASK_CAPACITY;++i)if(!masks[i].active){
        masks[i].descriptor=*descriptor;masks[i].active=true;masks[i].in_query=false;
        *handle=((uint32_t)masks[i].generation<<16)|(i+1u);
        ++diagnostics.masks_created;return true;
    }
    ++diagnostics.invalid_masks;return false;
}

bool nova_mask_destroy(nova_mask_handle_t handle)
{
    mask_slot_t *slot=mask_from_handle(handle);
    if(!slot)return false;
    for(uint16_t i=0;i<mask_depth;++i)
        if(mask_scopes[mask_stack[i]].mask==handle)return false;
    slot->active=false;
    if(++slot->generation==0)slot->generation=1;
    for(uint16_t i=0;i<NOVA_MASK_CACHE_CAPACITY;++i)
        if(cache[i].valid&&cache[i].mask==handle)cache[i].valid=false;
    ++diagnostics.masks_destroyed;return true;
}

bool nova_mask_push(nova_mask_handle_t handle)
{
    if(!diagnostics.frame_active||!mask_from_handle(handle)){
        ++diagnostics.invalid_masks;return false;
    }
    if(mask_depth==NOVA_MASK_STACK_CAPACITY||
       mask_scope_used==NOVA_MASK_SCOPE_CAPACITY){
        ++diagnostics.mask_stack_overflows;return false;
    }
    for(uint16_t i=0;i<mask_depth;++i)
        if(mask_scopes[mask_stack[i]].mask==handle){
            ++diagnostics.recursive_masks;return false;
        }
    uint16_t parent=mask_depth?(uint16_t)(mask_stack[mask_depth-1]+1u):0;
    mask_scopes[mask_scope_used]=(mask_scope_t){handle,parent,true};
    mask_stack[mask_depth++]=mask_scope_used++;
    ++diagnostics.mask_pushes;
    if(mask_depth>diagnostics.mask_depth_peak)diagnostics.mask_depth_peak=mask_depth;
    return true;
}

bool nova_mask_pop(void)
{
    if(!diagnostics.frame_active||!mask_depth){
        ++diagnostics.invalid_masks;return false;
    }
    --mask_depth;++diagnostics.mask_pops;return true;
}

uint16_t nova_mask_current_scope(void)
{
    return mask_depth?(uint16_t)(mask_stack[mask_depth-1]+1u):0;
}

static uint8_t rounded_coverage(nova_rect_t b,uint16_t radius,int32_t x,int32_t y)
{
    if(!radius)return 255;
    int32_t local_x=x-b.x,local_y=y-b.y;
    int32_t nearest_x=local_x<radius?radius:
        local_x>=b.width-radius?b.width-radius-1:local_x;
    int32_t nearest_y=local_y<radius?radius:
        local_y>=b.height-radius?b.height-radius-1:local_y;
    int64_t dx=(int64_t)local_x-nearest_x,dy=(int64_t)local_y-nearest_y;
    return dx*dx+dy*dy<=(int64_t)radius*radius?255:0;
}

static uint8_t query_mask(nova_mask_handle_t handle,int32_t x,int32_t y)
{
    mask_slot_t *slot=mask_from_handle(handle);
    if(!slot)return 0;
    nova_mask_descriptor_t *d=&slot->descriptor;
    int64_t right=(int64_t)d->bounds.x+d->bounds.width;
    int64_t bottom=(int64_t)d->bounds.y+d->bounds.height;
    if(x<d->bounds.x||y<d->bounds.y||x>=right||y>=bottom)return 0;
    uint32_t hash=((uint32_t)x*73856093u)^((uint32_t)y*19349663u)^handle;
    mask_cache_entry_t *entry=&cache[hash%NOVA_MASK_CACHE_CAPACITY];
    if(d->cacheable&&entry->valid&&entry->mask==handle&&entry->x==x&&entry->y==y){
        ++diagnostics.cache_hits;return entry->coverage;
    }
    ++diagnostics.cache_misses;
    int32_t local_x=x-d->bounds.x,local_y=y-d->bounds.y;
    uint8_t coverage=255;
    if(d->type==NOVA_MASK_BITMAP)
        coverage=(d->pixels[(uint64_t)local_y*d->stride+(uint32_t)local_x/8u]&
                  (uint8_t)(0x80u>>((uint32_t)local_x&7u)))?255:0;
    else if(d->type==NOVA_MASK_ALPHA)
        coverage=d->pixels[(uint64_t)local_y*d->stride+local_x];
    else if(d->type==NOVA_MASK_ROUNDED_RECTANGLE)
        coverage=rounded_coverage(d->bounds,d->radius,x,y);
    else if(d->type==NOVA_MASK_VECTOR){
        if(slot->in_query){++diagnostics.recursive_masks;return 0;}
        slot->in_query=true;
        coverage=d->vector_callback(x,y,d->vector_context);
        slot->in_query=false;
    }
    if(d->cacheable)*entry=(mask_cache_entry_t){handle,x,y,coverage,true};
    return coverage;
}

uint8_t nova_mask_coverage(uint16_t scope,int32_t x,int32_t y)
{
    if(!scope)return 255;
    if(scope>mask_scope_used)return 0;
    uint32_t combined=255;
    uint16_t current=scope;
    while(current){
        mask_scope_t *node=&mask_scopes[current-1];
        if(!node->active)return 0;
        uint8_t value=query_mask(node->mask,x,y);
        ++diagnostics.mask_tests;
        combined=(combined*value+127u)/255u;
        if(!combined){++diagnostics.masked_pixels;break;}
        current=node->parent;
    }
    return (uint8_t)combined;
}

const nova_clip_mask_diagnostics_t *nova_clip_mask_diagnostics(void)
{
    return &diagnostics;
}
