#include "memory.h"
#include "recovery.h"

#define KIB(value) ((value)*1024u)
#define NOVA_MEMORY_ARENA_SIZE KIB(2176)

typedef struct {
    uint8_t *base;
    uint32_t capacity;
    uint32_t offset;
    uint32_t external_used;
    nova_memory_pool_statistics_t statistics;
} nova_memory_pool_t;

_Alignas(64) static uint8_t arena[NOVA_MEMORY_ARENA_SIZE];
static nova_memory_pool_t pools[NOVA_MEMORY_POOL_COUNT];
static nova_memory_object_t objects[NOVA_MEMORY_OBJECT_CAPACITY];
static nova_memory_statistics_t statistics;
static nova_memory_budget_t memory_budget;
static uint32_t next_id;

static const uint32_t pool_budgets[NOVA_MEMORY_POOL_COUNT]={
    8u*1024u*1024u,4u*1024u*1024u,16u*1024u*1024u,2u*1024u*1024u,
    8u*1024u*1024u,16u*1024u*1024u,8u*1024u*1024u,2u*1024u*1024u
};
static const uint32_t arena_capacities[NOVA_MEMORY_POOL_COUNT]={
    KIB(256),KIB(128),KIB(512),KIB(128),KIB(256),KIB(256),KIB(512),KIB(128)
};

/* Standard profile from NPSPEC-BOOTPERF-0003, excluding the platform-owned
 * framebuffer. Minimal and comfort profiles preserve deterministic ratios. */
static const uint8_t standard_area_mib[NOVA_MEMORY_AREA_COUNT]={
    8,12,4,4,1,2,2,2,2,27
};

static void refresh_budget(void)
{
    uint64_t areas=0;uint32_t fragmentation=0;
    for(uint8_t i=0;i<NOVA_MEMORY_AREA_COUNT;++i)areas+=memory_budget.area_used[i];
    for(uint8_t i=0;i<NOVA_MEMORY_POOL_COUNT;++i)
        fragmentation+=pools[i].statistics.fragmentation;
    memory_budget.pool_memory=statistics.total_used;
    memory_budget.cache_memory=memory_budget.area_used[NOVA_MEMORY_AREA_RESOURCE_CACHE]+
        memory_budget.area_used[NOVA_MEMORY_AREA_GLYPH_CACHE]+
        memory_budget.area_used[NOVA_MEMORY_AREA_SVG_CACHE]+
        memory_budget.area_used[NOVA_MEMORY_AREA_THEME_CACHE];
    memory_budget.used_memory=areas+statistics.total_used+memory_budget.framebuffer_memory;
    memory_budget.free_memory=memory_budget.used_memory<memory_budget.total_budget?
        memory_budget.total_budget-memory_budget.used_memory:0;
    memory_budget.fragmentation=fragmentation;
    memory_budget.within_budget=memory_budget.used_memory<=memory_budget.total_budget;
    if(memory_budget.used_memory>memory_budget.peak_memory)
        memory_budget.peak_memory=memory_budget.used_memory;
}

bool nova_memory_budget_configure(nova_memory_profile_t profile)
{
    if(profile!=NOVA_MEMORY_PROFILE_MINIMAL&&profile!=NOVA_MEMORY_PROFILE_STANDARD&&
       profile!=NOVA_MEMORY_PROFILE_COMFORT&&profile!=NOVA_MEMORY_PROFILE_HIGH_QUALITY)
        return false;
    bool runtime_locked=memory_budget.runtime_locked;
    memory_budget=(nova_memory_budget_t){.total_budget=(uint64_t)profile*1024u*1024u,
        .profile=profile,.within_budget=true,.runtime_locked=runtime_locked};
    for(uint8_t i=0;i<NOVA_MEMORY_AREA_COUNT;++i)
        memory_budget.area_budget[i]=(uint64_t)standard_area_mib[i]*1024u*1024u*
            (uint32_t)profile/(uint32_t)NOVA_MEMORY_PROFILE_STANDARD;
    refresh_budget();return true;
}

bool nova_memory_budget_initialize(void)
{return nova_memory_budget_configure(NOVA_MEMORY_PROFILE_STANDARD);}
bool nova_memory_budget_configure_low_end(void)
{
    if(!nova_memory_budget_configure(NOVA_MEMORY_PROFILE_MINIMAL))return false;
    memory_budget.area_budget[NOVA_MEMORY_AREA_RENDERING]=4ull*1024u*1024u;
    memory_budget.area_budget[NOVA_MEMORY_AREA_RESOURCE_CACHE]=6ull*1024u*1024u;
    memory_budget.area_budget[NOVA_MEMORY_AREA_GLYPH_CACHE]=2ull*1024u*1024u;
    memory_budget.area_budget[NOVA_MEMORY_AREA_SVG_CACHE]=2ull*1024u*1024u;
    memory_budget.area_budget[NOVA_MEMORY_AREA_THEME_CACHE]=512ull*1024u;
    memory_budget.area_budget[NOVA_MEMORY_AREA_ANIMATION_POOL]=512ull*1024u;
    memory_budget.area_budget[NOVA_MEMORY_AREA_CONTROL_POOL]=2ull*1024u*1024u;
    memory_budget.area_budget[NOVA_MEMORY_AREA_LAYOUT_POOL]=1ull*1024u*1024u;
    memory_budget.area_budget[NOVA_MEMORY_AREA_DIAGNOSTICS]=1ull*1024u*1024u;
    uint64_t assigned=0;for(uint8_t i=0;i<NOVA_MEMORY_AREA_RESERVE;++i)
        assigned+=memory_budget.area_budget[i];
    memory_budget.area_budget[NOVA_MEMORY_AREA_RESERVE]=
        memory_budget.total_budget-assigned;
    refresh_budget();return true;
}
bool nova_memory_budget_configure_high_quality(void)
{
    if(!nova_memory_budget_configure(NOVA_MEMORY_PROFILE_HIGH_QUALITY))return false;
    memory_budget.area_budget[NOVA_MEMORY_AREA_RENDERING]=32ull*1024u*1024u;
    memory_budget.area_budget[NOVA_MEMORY_AREA_RESOURCE_CACHE]=64ull*1024u*1024u;
    memory_budget.area_budget[NOVA_MEMORY_AREA_GLYPH_CACHE]=16ull*1024u*1024u;
    memory_budget.area_budget[NOVA_MEMORY_AREA_SVG_CACHE]=16ull*1024u*1024u;
    memory_budget.area_budget[NOVA_MEMORY_AREA_THEME_CACHE]=4ull*1024u*1024u;
    memory_budget.area_budget[NOVA_MEMORY_AREA_ANIMATION_POOL]=16ull*1024u*1024u;
    memory_budget.area_budget[NOVA_MEMORY_AREA_CONTROL_POOL]=16ull*1024u*1024u;
    memory_budget.area_budget[NOVA_MEMORY_AREA_LAYOUT_POOL]=4ull*1024u*1024u;
    memory_budget.area_budget[NOVA_MEMORY_AREA_DIAGNOSTICS]=4ull*1024u*1024u;
    uint64_t assigned=0;for(uint8_t i=0;i<NOVA_MEMORY_AREA_RESERVE;++i)
        assigned+=memory_budget.area_budget[i];
    memory_budget.area_budget[NOVA_MEMORY_AREA_RESERVE]=
        memory_budget.total_budget-assigned;
    refresh_budget();return true;
}
const nova_memory_budget_t *nova_memory_budget_status(void){refresh_budget();return &memory_budget;}
bool nova_memory_budget_available(uint64_t bytes)
{refresh_budget();return bytes<=memory_budget.free_memory;}
bool nova_memory_budget_reset(void)
{nova_memory_profile_t profile=memory_budget.profile?memory_budget.profile:
    NOVA_MEMORY_PROFILE_STANDARD;return nova_memory_budget_configure(profile);}
bool nova_memory_budget_set_runtime(bool active)
{memory_budget.runtime_locked=active;return true;}
bool nova_memory_budget_report(nova_memory_area_t area,uint64_t used)
{
    if(area>=NOVA_MEMORY_AREA_COUNT)return false;
    memory_budget.area_used[area]=used;refresh_budget();
    if(used>memory_budget.area_budget[area]||!memory_budget.within_budget){
        ++memory_budget.budget_overruns;return false;
    }
    return true;
}
void nova_memory_budget_record_eviction(uint64_t bytes)
{
    ++memory_budget.cache_evictions;
    uint64_t *used=&memory_budget.area_used[NOVA_MEMORY_AREA_RESOURCE_CACHE];
    *used=bytes<*used?*used-bytes:0;refresh_budget();
}
bool nova_memory_budget_apply_pressure(void)
{
    ++memory_budget.pressure_events;
    if(memory_budget.overload_step<5)++memory_budget.overload_step;
    return memory_budget.overload_step<5;
}
void nova_memory_secure_zero(void *address,uint32_t size)
{
    volatile uint8_t *bytes=(volatile uint8_t *)address;
    if(!bytes)return;
    while(size--)*bytes++=0;
}

static uint32_t align_up(uint32_t value,uint8_t alignment)
{return (value+(uint32_t)alignment-1u)&~((uint32_t)alignment-1u);}
static bool valid_alignment(uint8_t alignment)
{return alignment==16u||alignment==64u;}

static nova_memory_object_t *find_object(const void *address,bool include_released)
{
    for(uint16_t i=0;i<NOVA_MEMORY_OBJECT_CAPACITY;++i)
        if(objects[i].address==address&&(include_released||
           objects[i].state==NOVA_MEMORY_OBJECT_ACTIVE||
           objects[i].state==NOVA_MEMORY_OBJECT_STATIC))return &objects[i];
    return 0;
}

static nova_memory_object_t *new_object(void)
{
    for(uint16_t i=0;i<NOVA_MEMORY_OBJECT_CAPACITY;++i)
        if(objects[i].state==NOVA_MEMORY_OBJECT_FREE||
           objects[i].state==NOVA_MEMORY_OBJECT_RELEASED)return &objects[i];
    ++statistics.metadata_overflows;return 0;
}

static void update_total(void)
{
    uint32_t used=0;
    for(uint8_t i=0;i<NOVA_MEMORY_POOL_COUNT;++i){
        pools[i].statistics.used=pools[i].offset+pools[i].external_used;
        if(pools[i].statistics.used>pools[i].statistics.peak)
            pools[i].statistics.peak=pools[i].statistics.used;
        used+=pools[i].statistics.used;
    }
    statistics.total_used=used;
    if(used>statistics.total_peak)statistics.total_peak=used;
}

static void request_recovery(nova_memory_pool_id_t pool)
{
    ++statistics.recovery_requests;
    nova_recovery_report(0x80020000u+(uint32_t)pool,NOVA_UI_SUBSYSTEM_RESOURCES,
        NOVA_UI_ERROR_CRITICAL,NOVA_RECOVERY_SAFE_MODE,0);
}

void nova_memory_initialize(void)
{
    statistics=(nova_memory_statistics_t){.total_budget=NOVA_MEMORY_TOTAL_BUDGET};
    next_id=1;
    uint32_t offset=0;
    for(uint8_t i=0;i<NOVA_MEMORY_POOL_COUNT;++i){
        pools[i]=(nova_memory_pool_t){arena+offset,arena_capacities[i],0,0,
            {pool_budgets[i],arena_capacities[i],0,0,0,0,0,0,0}};
        offset+=arena_capacities[i];
    }
    for(uint16_t i=0;i<NOVA_MEMORY_OBJECT_CAPACITY;++i)
        objects[i]=(nova_memory_object_t){0};
    (void)nova_memory_budget_initialize();
}

void *nova_memory_allocate(nova_memory_pool_id_t pool,uint32_t size,
    uint32_t owner,uint8_t alignment)
{
    if(pool>=NOVA_MEMORY_POOL_COUNT||!size||!valid_alignment(alignment)){
        if(!valid_alignment(alignment))++statistics.alignment_errors;
        else ++statistics.invalid_pointers;
        return 0;
    }
    if(memory_budget.runtime_locked&&pool!=NOVA_MEMORY_FRAME){
        ++statistics.recovery_requests;return 0;
    }
    nova_memory_pool_t *target=&pools[pool];
    uint32_t aligned=align_up(target->offset,alignment);
    if(aligned>target->capacity||size>target->capacity-aligned||
       target->external_used+aligned+size>target->statistics.budget){
        ++target->statistics.overflows;request_recovery(pool);return 0;
    }
    nova_memory_object_t *object=new_object();if(!object){request_recovery(pool);return 0;}
    uint32_t padding=aligned-target->offset;target->statistics.fragmentation+=padding;
    void *address=target->base+aligned;target->offset=aligned+size;
    *object=(nova_memory_object_t){next_id++,address,size,owner,1,alignment,pool,
                                   NOVA_MEMORY_OBJECT_ACTIVE};
    ++target->statistics.allocations;++target->statistics.active_objects;
    ++statistics.allocations;update_total();refresh_budget();return address;
}

bool nova_memory_track_static(nova_memory_pool_id_t pool,void *address,
    uint32_t size,uint32_t owner,uint8_t alignment)
{
    if(pool>=NOVA_MEMORY_POOL_COUNT||!address||!size||!valid_alignment(alignment)||
       ((uintptr_t)address&(alignment-1u))||find_object(address,true)){
        if(address&&valid_alignment(alignment)&&((uintptr_t)address&(alignment-1u)))
            ++statistics.alignment_errors;
        else ++statistics.invalid_pointers;
        return false;
    }
    nova_memory_pool_t *target=&pools[pool];
    if(size>target->statistics.budget-target->external_used-target->offset){
        ++target->statistics.overflows;request_recovery(pool);return false;
    }
    nova_memory_object_t *object=new_object();if(!object){request_recovery(pool);return false;}
    *object=(nova_memory_object_t){next_id++,address,size,owner,1,alignment,pool,
                                   NOVA_MEMORY_OBJECT_STATIC};
    target->external_used+=size;++target->statistics.allocations;
    ++target->statistics.active_objects;++statistics.allocations;update_total();refresh_budget();return true;
}

bool nova_memory_retain(void *address)
{
    nova_memory_object_t *object=find_object(address,false);
    if(!object||object->references==UINT16_MAX){++statistics.invalid_pointers;return false;}
    ++object->references;return true;
}

bool nova_memory_release(void *address)
{
    nova_memory_object_t *object=find_object(address,true);
    if(!object){++statistics.invalid_pointers;return false;}
    if(object->state==NOVA_MEMORY_OBJECT_RELEASED){++statistics.double_frees;return false;}
    if(object->state==NOVA_MEMORY_OBJECT_STATIC){++statistics.invalid_pointers;return false;}
    if(object->references>1){--object->references;return true;}
    nova_memory_pool_t *pool=&pools[object->pool];
    uint32_t object_offset=(uint32_t)((uint8_t *)object->address-pool->base);
    if(object_offset+object->size==pool->offset)pool->offset=object_offset;
    else pool->statistics.fragmentation+=object->size;
    object->references=0;object->state=NOVA_MEMORY_OBJECT_RELEASED;
    --pool->statistics.active_objects;++pool->statistics.releases;
    ++statistics.releases;update_total();refresh_budget();return true;
}

bool nova_memory_reset_frame(void)
{
    nova_memory_pool_t *frame=&pools[NOVA_MEMORY_FRAME];
    for(uint16_t i=0;i<NOVA_MEMORY_OBJECT_CAPACITY;++i)
        if(objects[i].pool==NOVA_MEMORY_FRAME&&objects[i].state==NOVA_MEMORY_OBJECT_ACTIVE){
            objects[i].state=NOVA_MEMORY_OBJECT_RELEASED;objects[i].references=0;
        }
    frame->offset=0;frame->statistics.fragmentation=0;
    frame->statistics.active_objects=0;++statistics.frame_resets;update_total();refresh_budget();return true;
}

bool nova_memory_validate_pointer(const void *address,
    nova_memory_pool_id_t expected_pool,uint32_t minimum_size)
{
    nova_memory_object_t *object=find_object(address,false);
    bool valid=object&&expected_pool<NOVA_MEMORY_POOL_COUNT&&object->pool==expected_pool&&
        object->size>=minimum_size&&((uintptr_t)object->address&(object->alignment-1u))==0;
    if(!valid)++statistics.invalid_pointers;
    return valid;
}
const nova_memory_object_t *nova_memory_object(const void *address)
{return find_object(address,false);}
const nova_memory_pool_statistics_t *nova_memory_pool_statistics(nova_memory_pool_id_t pool)
{return pool<NOVA_MEMORY_POOL_COUNT?&pools[pool].statistics:0;}
const nova_memory_statistics_t *nova_memory_statistics(void){return &statistics;}
