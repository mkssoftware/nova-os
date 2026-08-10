#include "surface_manager.h"

#define MIB(value) ((value)*1024u*1024u)
#define POOL_BYTES MIB(4)
typedef struct {nova_surface_info_t info;nova_surface_t native;uint32_t offset,capacity;
    bool occupied;} slot_t;
_Alignas(64) static uint8_t pixels[POOL_BYTES];
static slot_t slots[NOVA_MANAGED_SURFACE_CAPACITY];
static nova_surface_manager_diagnostics_t diagnostics;
static const uint32_t pool_base[NOVA_SURFACE_POOL_COUNT]={0,MIB(1),MIB(2),MIB(2)+MIB(1)/2,MIB(3)};
static const uint32_t pool_size[NOVA_SURFACE_POOL_COUNT]={MIB(1),MIB(1),MIB(1)/2,MIB(1)/2,MIB(1)};
static uint32_t pool_offset[NOVA_SURFACE_POOL_COUNT];

static nova_surface_handle_t make_handle(uint16_t index,uint16_t generation)
{return ((uint32_t)generation<<16)|index;}
static slot_t *lookup(nova_surface_handle_t handle)
{
    uint16_t index=(uint16_t)(handle&0xffffu),generation=(uint16_t)(handle>>16);
    if(index>=NOVA_MANAGED_SURFACE_CAPACITY||!generation||!slots[index].occupied||
       slots[index].info.generation!=generation){++diagnostics.invalid_handles;return 0;}
    return &slots[index];
}
static bool checked_size(uint32_t width,uint32_t height,uint32_t bpp,
    uint32_t *pitch,uint64_t *size)
{
    if(!width||!height||width>NOVA_MANAGED_SURFACE_MAX_DIMENSION||
       height>NOVA_MANAGED_SURFACE_MAX_DIMENSION||width>UINT32_MAX/bpp)return false;
    uint32_t raw=width*bpp;if(raw>UINT32_MAX-63u)return false;
    *pitch=(raw+63u)&~63u;*size=(uint64_t)*pitch*height;
    return *size/height==*pitch;
}
static uint32_t format_bpp(nova_pixel_format_t format)
{return format==NOVA_PIXEL_RGBA8888||format==NOVA_PIXEL_BGRA8888?4u:
    format==NOVA_PIXEL_RGB888||format==NOVA_PIXEL_BGR888?3u:
    format==NOVA_PIXEL_RGB565?2u:0u;}
static uint16_t free_slot(void)
{for(uint16_t i=0;i<NOVA_MANAGED_SURFACE_CAPACITY;++i)if(!slots[i].occupied)return i;
 return NOVA_MANAGED_SURFACE_CAPACITY;}
static uint16_t reusable(const nova_surface_descriptor_t *d,uint64_t size,uint32_t pitch)
{
    for(uint16_t i=0;i<NOVA_MANAGED_SURFACE_CAPACITY;++i)if(slots[i].occupied&&
       !slots[i].info.external&&slots[i].info.state==NOVA_SURFACE_RECYCLED&&
       slots[i].info.pool==d->pool&&slots[i].info.format==d->format&&
       slots[i].capacity>=size&&slots[i].native.stride*slots[i].info.bytes_per_pixel>=pitch)
        return i;
    return NOVA_MANAGED_SURFACE_CAPACITY;
}
static void clear_native(slot_t *slot,uint32_t color)
{nova_surface_clear(&slot->native,color);slot->native.full_damage=true;}
void nova_surface_manager_initialize(void)
{
    diagnostics=(nova_surface_manager_diagnostics_t){0};
    for(uint8_t p=0;p<NOVA_SURFACE_POOL_COUNT;++p)pool_offset[p]=0;
    for(uint16_t i=0;i<NOVA_MANAGED_SURFACE_CAPACITY;++i){uint64_t generation=slots[i].info.generation+1u;
        if(!generation||generation>0xffffu)generation=1;
        slots[i]=(slot_t){.info={.generation=generation}};}
    nova_surface_descriptor_t emergency={NOVA_SURFACE_EMERGENCY,320,200,
        NOVA_PIXEL_BGRA8888,0,NOVA_SURFACE_CLEAR_OPAQUE_BLACK,0xff000000u,0,
        NOVA_SURFACE_POOL_EMERGENCY,MIB(1)};
    nova_managed_surface_create(&emergency,&diagnostics.emergency);
}
nova_surface_result_t nova_managed_surface_create(const nova_surface_descriptor_t *d,
    nova_surface_handle_t *out)
{
    if(!d||!out||d->type>NOVA_SURFACE_EMERGENCY||d->pool>=NOVA_SURFACE_POOL_COUNT){return NOVA_SURFACE_ERR_INVALID_ARGUMENT;}
    uint32_t bpp=format_bpp(d->format),pitch=0;uint64_t size=0;
    if(!bpp)return NOVA_SURFACE_ERR_UNSUPPORTED_FORMAT;
    if(!checked_size(d->width,d->height,bpp,&pitch,&size)){++diagnostics.size_overflows;return NOVA_SURFACE_ERR_SIZE_OVERFLOW;}
    uint64_t maximum=d->maximum_size?d->maximum_size:NOVA_MANAGED_SURFACE_DEFAULT_MAX_SIZE;
    if(size>maximum){++diagnostics.size_overflows;return NOVA_SURFACE_ERR_SIZE_OVERFLOW;}
    uint16_t index=reusable(d,size,pitch);bool reused=index<NOVA_MANAGED_SURFACE_CAPACITY;
    if(!reused){index=free_slot();if(index==NOVA_MANAGED_SURFACE_CAPACITY)return NOVA_SURFACE_ERR_OUT_OF_MEMORY;
        uint32_t aligned=(pool_offset[d->pool]+63u)&~63u;
        if(size>pool_size[d->pool]||aligned>pool_size[d->pool]-(uint32_t)size){
            ++diagnostics.budget_failures;return NOVA_SURFACE_ERR_BUDGET_EXCEEDED;}
        slots[index].offset=pool_base[d->pool]+aligned;slots[index].capacity=(uint32_t)size;
        pool_offset[d->pool]=aligned+(uint32_t)size;
        diagnostics.used[d->pool]+=size;if(diagnostics.used[d->pool]>diagnostics.peak[d->pool])
            diagnostics.peak[d->pool]=diagnostics.used[d->pool];
    }else ++diagnostics.reuses;
    uint64_t generation=slots[index].info.generation+1u;if(!generation||generation>0xffffu)generation=1;
    slots[index].occupied=true;slots[index].info=(nova_surface_info_t){
        make_handle(index,(uint16_t)generation),d->type,NOVA_SURFACE_READY,d->pool,d->format,
        d->width,d->height,pitch,bpp,d->owner_layer,d->usage_flags,1,0,size,generation,false};
    slots[index].native=(nova_surface_t){(uint32_t *)(pixels+slots[index].offset),d->width,d->height,
        pitch/bpp,{{0}},0,true};
    uint32_t color=d->clear_mode==NOVA_SURFACE_CLEAR_TRANSPARENT?0:
        d->clear_mode==NOVA_SURFACE_CLEAR_CUSTOM?d->clear_color:0xff000000u;
    if(d->clear_mode!=NOVA_SURFACE_CLEAR_NONE)clear_native(&slots[index],color);
    *out=slots[index].info.handle;++diagnostics.creations;return NOVA_SURFACE_OK;
}
nova_surface_result_t nova_managed_surface_import(nova_surface_t *surface,
    nova_managed_surface_type_t type,nova_surface_pool_t pool,uint32_t owner,nova_surface_handle_t *out)
{
    if(!surface||!surface->pixels||!surface->width||!surface->height||
       surface->stride<surface->width||type>NOVA_SURFACE_EMERGENCY||pool>=NOVA_SURFACE_POOL_COUNT||!out)
        return NOVA_SURFACE_ERR_INVALID_ARGUMENT;
    uint16_t index=free_slot();if(index==NOVA_MANAGED_SURFACE_CAPACITY)return NOVA_SURFACE_ERR_OUT_OF_MEMORY;
    uint64_t generation=slots[index].info.generation+1u;if(!generation||generation>0xffffu)generation=1;
    uint64_t size=(uint64_t)surface->stride*surface->height*4u;
    slots[index].occupied=true;slots[index].native=*surface;
    slots[index].info=(nova_surface_info_t){make_handle(index,(uint16_t)generation),type,
        NOVA_SURFACE_READY,pool,NOVA_PIXEL_BGRA8888,surface->width,surface->height,
        surface->stride*4u,4,owner,0,1,0,size,generation,true};
    *out=slots[index].info.handle;++diagnostics.imports;return NOVA_SURFACE_OK;
}
nova_surface_result_t nova_managed_surface_retain(nova_surface_handle_t handle)
{slot_t *s=lookup(handle);if(!s)return NOVA_SURFACE_ERR_INVALID_ID;
 if(s->info.state!=NOVA_SURFACE_READY&&s->info.state!=NOVA_SURFACE_IN_USE&&
    s->info.state!=NOVA_SURFACE_IDLE)return NOVA_SURFACE_ERR_INVALID_STATE;
 if(s->info.references==UINT32_MAX){++diagnostics.reference_overflows;return NOVA_SURFACE_ERR_STILL_REFERENCED;}
 ++s->info.references;s->info.state=NOVA_SURFACE_IN_USE;return NOVA_SURFACE_OK;}
nova_surface_result_t nova_managed_surface_release(nova_surface_handle_t handle)
{slot_t *s=lookup(handle);if(!s)return NOVA_SURFACE_ERR_INVALID_ID;
 if(s->info.locks){++diagnostics.lock_conflicts;return NOVA_SURFACE_ERR_LOCKED;}
 if(!s->info.references)return NOVA_SURFACE_ERR_INVALID_STATE;
 if(--s->info.references)return NOVA_SURFACE_OK;
 s->info.owner_layer=0;s->info.state=s->info.external?NOVA_SURFACE_RELEASED:NOVA_SURFACE_RECYCLED;
 if(s->info.external)s->occupied=false;
 ++diagnostics.releases;return NOVA_SURFACE_OK;}
nova_surface_result_t nova_managed_surface_lock(nova_surface_handle_t handle,nova_surface_mapping_t *mapping)
{slot_t *s=lookup(handle);if(!s)return NOVA_SURFACE_ERR_INVALID_ID;if(!mapping)return NOVA_SURFACE_ERR_INVALID_ARGUMENT;
 if(s->info.state!=NOVA_SURFACE_READY&&s->info.state!=NOVA_SURFACE_IN_USE&&
    s->info.state!=NOVA_SURFACE_IDLE)return NOVA_SURFACE_ERR_INVALID_STATE;
 if(s->info.locks){++diagnostics.lock_conflicts;return NOVA_SURFACE_ERR_LOCKED;}
 s->info.locks=1;s->info.state=NOVA_SURFACE_IN_USE;*mapping=(nova_surface_mapping_t){s->native.pixels,
 s->info.width,s->info.height,s->info.pitch,s->info.format};++diagnostics.locks;return NOVA_SURFACE_OK;}
nova_surface_result_t nova_managed_surface_unlock(nova_surface_handle_t handle,const nova_rect_t *modified)
{slot_t *s=lookup(handle);if(!s)return NOVA_SURFACE_ERR_INVALID_ID;if(!s->info.locks)return NOVA_SURFACE_ERR_INVALID_STATE;
 s->info.locks=0;s->info.state=NOVA_SURFACE_READY;if(modified)nova_damage_add(&s->native,*modified);
 else s->native.full_damage=true;
 ++diagnostics.unlocks;return NOVA_SURFACE_OK;}
nova_surface_result_t nova_managed_surface_clear(nova_surface_handle_t handle,uint32_t color)
{slot_t *s=lookup(handle);if(!s)return NOVA_SURFACE_ERR_INVALID_ID;if(s->info.locks)return NOVA_SURFACE_ERR_LOCKED;
 clear_native(s,color);return NOVA_SURFACE_OK;}
nova_surface_result_t nova_managed_surface_info(nova_surface_handle_t handle,nova_surface_info_t *info)
{slot_t *s=lookup(handle);if(!s)return NOVA_SURFACE_ERR_INVALID_ID;if(!info)return NOVA_SURFACE_ERR_INVALID_ARGUMENT;
 *info=s->info;return NOVA_SURFACE_OK;}
nova_surface_t *nova_managed_surface_native(nova_surface_handle_t handle)
{slot_t *s=lookup(handle);return s?&s->native:0;}
bool nova_surface_manager_reset_frame(void)
{for(uint16_t i=0;i<NOVA_MANAGED_SURFACE_CAPACITY;++i)if(slots[i].occupied&&
 slots[i].info.pool==NOVA_SURFACE_POOL_FRAME&&!slots[i].info.external){
    if(slots[i].info.locks)return false;
    slots[i].info.references=0;slots[i].info.owner_layer=0;
    slots[i].info.state=NOVA_SURFACE_RECYCLED;}++diagnostics.frame_resets;return true;}
nova_surface_handle_t nova_surface_manager_emergency(void)
{++diagnostics.emergency_activations;return diagnostics.emergency;}
const nova_surface_manager_diagnostics_t *nova_surface_manager_diagnostics(void){return &diagnostics;}
