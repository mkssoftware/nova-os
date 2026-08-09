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
static uint32_t next_id;

static const uint32_t pool_budgets[NOVA_MEMORY_POOL_COUNT]={
    8u*1024u*1024u,4u*1024u*1024u,16u*1024u*1024u,2u*1024u*1024u,
    8u*1024u*1024u,16u*1024u*1024u,8u*1024u*1024u,2u*1024u*1024u
};
static const uint32_t arena_capacities[NOVA_MEMORY_POOL_COUNT]={
    KIB(256),KIB(128),KIB(512),KIB(128),KIB(256),KIB(256),KIB(512),KIB(128)
};

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
}

void *nova_memory_allocate(nova_memory_pool_id_t pool,uint32_t size,
    uint32_t owner,uint8_t alignment)
{
    if(pool>=NOVA_MEMORY_POOL_COUNT||!size||!valid_alignment(alignment)){
        if(!valid_alignment(alignment))++statistics.alignment_errors;
        else ++statistics.invalid_pointers;
        return 0;
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
    ++statistics.allocations;update_total();return address;
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
    ++target->statistics.active_objects;++statistics.allocations;update_total();return true;
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
    ++statistics.releases;update_total();return true;
}

bool nova_memory_reset_frame(void)
{
    nova_memory_pool_t *frame=&pools[NOVA_MEMORY_FRAME];
    for(uint16_t i=0;i<NOVA_MEMORY_OBJECT_CAPACITY;++i)
        if(objects[i].pool==NOVA_MEMORY_FRAME&&objects[i].state==NOVA_MEMORY_OBJECT_ACTIVE){
            objects[i].state=NOVA_MEMORY_OBJECT_RELEASED;objects[i].references=0;
        }
    frame->offset=0;frame->statistics.fragmentation=0;
    frame->statistics.active_objects=0;++statistics.frame_resets;update_total();return true;
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
