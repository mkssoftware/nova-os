#include "animation_resources.h"
#include "resources.h"

static nova_animation_resource_t animations[NOVA_ANIMATION_RESOURCE_CAPACITY];
static int8_t slots[64];static nova_animation_resource_diagnostics_t diagnostics;

static void copy_name(char *destination,const char *source)
{uint8_t i=0;for(;source&&source[i]&&i+1<48;++i)destination[i]=source[i];destination[i]=0;}
static int8_t lookup(uint64_t id)
{if(!id||!diagnostics.initialized)return -1;uint8_t slot=(uint8_t)id&63u;
 for(uint8_t probe=0;probe<64;++probe){int8_t index=slots[(slot+probe)&63u];
  if(index<0)return -1;
  if(animations[(uint8_t)index].valid&&
     animations[(uint8_t)index].animation_id==id)return index;}return -1;}
static bool insert(uint64_t id,uint8_t index)
{uint8_t slot=(uint8_t)id&63u;for(uint8_t probe=0;probe<64;++probe){int8_t *entry=&slots[(slot+probe)&63u];
 if(*entry<0){*entry=(int8_t)index;return true;}}return false;}

bool nova_animation_resource_initialize(void)
{for(uint8_t i=0;i<NOVA_ANIMATION_RESOURCE_CAPACITY;++i)animations[i]=(nova_animation_resource_t){0};
 for(uint8_t i=0;i<64;++i)slots[i]=-1;
 diagnostics=(nova_animation_resource_diagnostics_t){0};
 diagnostics.initialized=true;diagnostics.initializations=1;diagnostics.speed_per_mille=1000;return true;}

static bool descriptor_valid(const nova_animation_resource_descriptor_t *d)
{
    if(!d||!d->animation_id||!d->resource_id||!d->name||!*d->name||!d->version||
       !d->resource_version||!d->duration_ms||d->repeat>NOVA_ANIMATION_REPEAT_INFINITE||
       (d->repeat==NOVA_ANIMATION_REPEAT_COUNT&&!d->repeat_count)||
       (d->repeat!=NOVA_ANIMATION_REPEAT_COUNT&&d->repeat_count)||
       d->trigger>NOVA_ANIMATION_TRIGGER_SYSTEM||d->category>NOVA_ANIMATION_CATEGORY_THEME||
       d->priority>3||!d->keyframes||d->keyframe_count<2||
       d->keyframe_count>NOVA_ANIMATION_KEYFRAME_CAPACITY)return false;
    uint32_t last[NOVA_PROPERTY_COUNT]={0};bool seen[NOVA_PROPERTY_COUNT]={0};
    bool start[NOVA_PROPERTY_COUNT]={0},end[NOVA_PROPERTY_COUNT]={0};
    for(uint16_t i=0;i<d->keyframe_count;++i){const nova_animation_keyframe_t *k=&d->keyframes[i];
        if(k->property>=NOVA_PROPERTY_COUNT||k->easing>NOVA_EASE_SPRING||k->time_ms>d->duration_ms||
           (seen[k->property]&&k->time_ms<=last[k->property]))return false;
        seen[k->property]=true;last[k->property]=k->time_ms;
        if(!k->time_ms)start[k->property]=true;
        if(k->time_ms==d->duration_ms)end[k->property]=true;}
    for(uint8_t p=0;p<NOVA_PROPERTY_COUNT;++p)if(seen[p]&&(!start[p]||!end[p]))return false;
    return true;
}

bool nova_animation_resource_register(const nova_animation_resource_descriptor_t *d)
{
    if(!diagnostics.initialized||!descriptor_valid(d)){++diagnostics.invalid_resources;return false;}
    if(lookup(d->animation_id)>=0){++diagnostics.duplicates;return false;}
    const nova_resource_t *resource=nova_resource_find(d->resource_id);
    if(!resource||resource->type!=NOVA_RESOURCE_ANIMATION||resource->version!=d->resource_version){
        ++diagnostics.invalid_resources;return false;}
    for(uint8_t i=0;i<NOVA_ANIMATION_RESOURCE_CAPACITY;++i)if(!animations[i].valid){
        nova_animation_resource_t a={.animation_id=d->animation_id,.resource_id=d->resource_id,
            .version=d->version,.resource_version=d->resource_version,.duration_ms=d->duration_ms,
            .repeat=d->repeat,.repeat_count=d->repeat_count,.keyframe_count=d->keyframe_count,
            .trigger=d->trigger,.category=d->category,.priority=d->priority,.valid=true};
        copy_name(a.name,d->name);for(uint16_t k=0;k<a.keyframe_count;++k)a.keyframes[k]=d->keyframes[k];
        if(!insert(a.animation_id,i))return false;
        animations[i]=a;++diagnostics.registered;return true;}
    ++diagnostics.invalid_resources;return false;
}
const nova_animation_resource_t *nova_animation_resource_find(uint64_t id)
{int8_t index=lookup(id);return index<0?0:&animations[(uint8_t)index];}
bool nova_animation_resource_validate(const nova_animation_resource_t *a)
{return a&&a->valid&&a->animation_id&&a->resource_id&&a->duration_ms&&a->keyframe_count>=2;}
const nova_animation_resource_t *nova_animation_resource_load(uint64_t id)
{int8_t index=lookup(id);if(index<0)return 0;nova_animation_resource_t *a=&animations[(uint8_t)index];
 if(a->loaded){if(!a->references&&!nova_resource_load(a->resource_id))return 0;
  ++a->references;++diagnostics.cache_hits;return a;}
 if(!nova_resource_load(a->resource_id))return 0;
 a->loaded=true;a->references=1;++diagnostics.loads;return a;}
bool nova_animation_resource_release(uint64_t id)
{int8_t index=lookup(id);if(index<0||!animations[(uint8_t)index].references)return false;
 nova_animation_resource_t *a=&animations[(uint8_t)index];--a->references;++diagnostics.releases;
 return a->references?true:nova_resource_release(a->resource_id);}

bool nova_animation_resource_sample(uint64_t id,uint64_t elapsed,nova_animation_sample_t *sample)
{
    const nova_animation_resource_t *a=nova_animation_resource_find(id);if(!a||!sample)return false;
    *sample=(nova_animation_sample_t){0};++diagnostics.samples;
    uint64_t scaled=elapsed*diagnostics.speed_per_mille/1000u;
    uint64_t cycle=scaled/a->duration_ms;uint32_t local=(uint32_t)(scaled%a->duration_ms);
    uint64_t maximum=a->repeat==NOVA_ANIMATION_REPEAT_ONCE?0:a->repeat==NOVA_ANIMATION_REPEAT_COUNT?
        a->repeat_count:UINT64_MAX;
    if(diagnostics.reduced_motion){cycle=maximum==UINT64_MAX?0:maximum;local=a->duration_ms;
        sample->complete=true;++diagnostics.reduced_samples;}
    else if(cycle>maximum){cycle=maximum;local=a->duration_ms;sample->complete=true;}
    sample->cycle=(uint32_t)(cycle>UINT32_MAX?UINT32_MAX:cycle);sample->local_time_ms=local;
    for(uint8_t property=0;property<NOVA_PROPERTY_COUNT;++property){const nova_animation_keyframe_t *left=0,*right=0;
        for(uint16_t i=0;i<a->keyframe_count;++i)if(a->keyframes[i].property==property){
            if(a->keyframes[i].time_ms<=local)left=&a->keyframes[i];
            if(a->keyframes[i].time_ms>=local){right=&a->keyframes[i];break;}}
        if(!left)left=right;
        if(!right)right=left;
        if(!left)continue;
        int32_t value=left->value;if(right!=left){uint32_t span=right->time_ms-left->time_ms;
            int32_t progress=(int32_t)(((uint64_t)(local-left->time_ms)*65536u)/span);
            int32_t eased=nova_ease_apply(right->easing,progress);
            value=left->value+(int32_t)(((int64_t)(right->value-left->value)*eased)/65536);}
        sample->values[property]=value;sample->property_mask|=1u<<property;++diagnostics.keyframes_evaluated;}
    return true;
}
const nova_animation_resource_t *nova_animation_resource_for_trigger(
    nova_animation_trigger_t trigger,nova_animation_category_t category)
{if(trigger>NOVA_ANIMATION_TRIGGER_SYSTEM||category>NOVA_ANIMATION_CATEGORY_THEME)return 0;
 ++diagnostics.trigger_resolutions;const nova_animation_resource_t *best=0;
 for(uint8_t i=0;i<NOVA_ANIMATION_RESOURCE_CAPACITY;++i){if(animations[i].valid&&
    animations[i].trigger==trigger&&animations[i].category==category&&
    (!best||animations[i].priority>best->priority))best=&animations[i];}
 return best;}
bool nova_animation_theme_bind(uint8_t slot,uint64_t id)
{if(slot>=NOVA_ANIMATION_THEME_SLOT_COUNT||!nova_animation_resource_find(id))return false;
 diagnostics.theme_slots[slot]=id;++diagnostics.theme_changes;return true;}
uint64_t nova_animation_theme_get(uint8_t slot)
{return slot<NOVA_ANIMATION_THEME_SLOT_COUNT?diagnostics.theme_slots[slot]:0;}
bool nova_animation_theme_speed(uint16_t speed)
{if(speed<250||speed>4000)return false;diagnostics.speed_per_mille=speed;++diagnostics.theme_changes;return true;}
void nova_animation_resource_set_reduced(bool enabled)
{diagnostics.reduced_motion=enabled;nova_motion_set_reduced(enabled);}
const nova_animation_resource_diagnostics_t *nova_animation_resource_diagnostics(void){return &diagnostics;}
