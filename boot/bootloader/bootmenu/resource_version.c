#include "resource_version.h"
#include "resources.h"

static nova_resource_version_entry_t entries[NOVA_RESOURCE_VERSION_CAPACITY];
static bool used[NOVA_RESOURCE_VERSION_CAPACITY];
static int8_t active_slots[NOVA_RESOURCE_VERSION_HASH_CAPACITY];
static nova_resource_version_diagnostics_t diagnostics;

static uint8_t hash_slot(uint64_t id)
{ return (uint8_t)id&(NOVA_RESOURCE_VERSION_HASH_CAPACITY-1u); }

static int8_t active_index(uint64_t id)
{
    if(!id||!diagnostics.initialized)return -1;
    uint8_t slot=hash_slot(id);
    for(uint8_t probe=0;probe<NOVA_RESOURCE_VERSION_HASH_CAPACITY;++probe){
        int8_t index=active_slots[(slot+probe)&(NOVA_RESOURCE_VERSION_HASH_CAPACITY-1u)];
        if(index<0)return -1;
        if(used[(uint8_t)index]&&entries[(uint8_t)index].logical_id==id&&
           entries[(uint8_t)index].active)return index;
    }
    return -1;
}

static bool set_active_hash(uint64_t id,int8_t index)
{
    uint8_t slot=hash_slot(id);
    for(uint8_t probe=0;probe<NOVA_RESOURCE_VERSION_HASH_CAPACITY;++probe){
        int8_t *entry=&active_slots[(slot+probe)&(NOVA_RESOURCE_VERSION_HASH_CAPACITY-1u)];
        if(*entry<0||entries[(uint8_t)*entry].logical_id==id){*entry=index;return true;}
    }
    return false;
}

bool nova_resource_version_initialize(void)
{
    for(uint8_t i=0;i<NOVA_RESOURCE_VERSION_CAPACITY;++i){
        used[i]=false;entries[i]=(nova_resource_version_entry_t){0};
    }
    for(uint8_t i=0;i<NOVA_RESOURCE_VERSION_HASH_CAPACITY;++i)active_slots[i]=-1;
    diagnostics=(nova_resource_version_diagnostics_t){.initialized=true};return true;
}

bool nova_resource_version_valid(nova_resource_version_t version)
{ return version.major!=0; }

int8_t nova_resource_version_compare(nova_resource_version_t left,
                                     nova_resource_version_t right)
{
    if(left.major!=right.major)return left.major<right.major?-1:1;
    if(left.minor!=right.minor)return left.minor<right.minor?-1:1;
    if(left.patch!=right.patch)return left.patch<right.patch?-1:1;
    if(left.build!=right.build)return left.build<right.build?-1:1;
    return 0;
}

bool nova_resource_version_compatible(const nova_resource_version_t *required,
                                      const nova_resource_version_t *available)
{
    ++diagnostics.compatibility_checks;
    bool result=required&&available&&nova_resource_version_valid(*required)&&
        nova_resource_version_valid(*available)&&required->major==available->major&&
        (available->minor>required->minor||
         (available->minor==required->minor&&available->patch>=required->patch));
    if(result)++diagnostics.compatible;else ++diagnostics.incompatible;return result;
}

static bool range_valid(nova_resource_version_t minimum,nova_resource_version_t maximum)
{
    return nova_resource_version_valid(minimum)&&nova_resource_version_valid(maximum)&&
        minimum.major==maximum.major&&
        (minimum.minor<maximum.minor||(minimum.minor==maximum.minor&&
         minimum.patch<=maximum.patch));
}

static int8_t compatibility_compare(nova_resource_version_t left,
                                    nova_resource_version_t right)
{
    if(left.major!=right.major)return left.major<right.major?-1:1;
    if(left.minor!=right.minor)return left.minor<right.minor?-1:1;
    if(left.patch!=right.patch)return left.patch<right.patch?-1:1;
    return 0;
}

bool nova_resource_version_register(const nova_resource_version_descriptor_t *descriptor)
{
    if(!diagnostics.initialized||!descriptor||!descriptor->logical_id||
       !descriptor->backing_resource_id||!nova_resource_version_valid(descriptor->version)||
       !range_valid(descriptor->minimum,descriptor->maximum)||
       descriptor->version.major!=descriptor->minimum.major||
       compatibility_compare(descriptor->version,descriptor->minimum)<0||
       compatibility_compare(descriptor->version,descriptor->maximum)>0||
       descriptor->dependency_count>NOVA_RESOURCE_VERSION_DEPENDENCY_CAPACITY||
       (descriptor->dependency_count&&!descriptor->dependencies)||
       !nova_resource_find(descriptor->backing_resource_id)){
        ++diagnostics.invalid_entries;return false;
    }
    for(uint8_t i=0;i<NOVA_RESOURCE_VERSION_CAPACITY;++i)if(used[i]&&
       entries[i].logical_id==descriptor->logical_id&&
       nova_resource_version_compare(entries[i].version,descriptor->version)==0){
        ++diagnostics.duplicates;return false;
    }
    for(uint8_t i=0;i<descriptor->dependency_count;++i)
        if(!descriptor->dependencies[i].resource_id||
           !nova_resource_version_valid(descriptor->dependencies[i].minimum)){
            ++diagnostics.invalid_entries;return false;}
    for(uint8_t i=0;i<NOVA_RESOURCE_VERSION_CAPACITY;++i)if(!used[i]){
        entries[i]=(nova_resource_version_entry_t){.logical_id=descriptor->logical_id,
            .backing_resource_id=descriptor->backing_resource_id,.version=descriptor->version,
            .minimum=descriptor->minimum,.maximum=descriptor->maximum,.valid=true,
            .dependency_count=descriptor->dependency_count};
        for(uint8_t d=0;d<descriptor->dependency_count;++d)
            entries[i].dependencies[d]=descriptor->dependencies[d];
        used[i]=true;++diagnostics.registered;return true;
    }
    ++diagnostics.invalid_entries;return false;
}

const nova_resource_version_entry_t *nova_resource_version(uint64_t resource_id)
{
    ++diagnostics.lookups;int8_t index=active_index(resource_id);
    return index<0?0:&entries[(uint8_t)index];
}

static bool dependencies_recursive(const nova_resource_version_entry_t *entry,
                                   uint64_t *visiting,uint8_t depth)
{
    if(!entry||depth>NOVA_RESOURCE_VERSION_CAPACITY)return false;
    uint8_t index=(uint8_t)(entry-entries);uint64_t bit=1ull<<(index&63u);
    if(visiting[index>>6]&bit){++diagnostics.cycles;return false;}
    visiting[index>>6]|=bit;
    for(uint8_t i=0;i<entry->dependency_count;++i){++diagnostics.dependency_checks;
        int8_t dependency_index=active_index(entry->dependencies[i].resource_id);
        if(dependency_index<0||!nova_resource_version_compatible(
            &entry->dependencies[i].minimum,&entries[(uint8_t)dependency_index].version)||
           !dependencies_recursive(&entries[(uint8_t)dependency_index],visiting,
                                   (uint8_t)(depth+1))){
            visiting[index>>6]&=~bit;++diagnostics.dependency_failures;return false;}
    }
    visiting[index>>6]&=~bit;return true;
}

bool nova_resource_version_dependencies(const nova_resource_version_entry_t *entry)
{ uint64_t visiting[1]={0};return dependencies_recursive(entry,visiting,1); }

const nova_resource_version_entry_t *nova_resource_version_select(
    uint64_t logical_id,const nova_resource_version_t *minimum,
    const nova_resource_version_t *maximum)
{
    if(!logical_id||!minimum||!maximum||!range_valid(*minimum,*maximum)){
        ++diagnostics.conflicts;return 0;}
    int8_t best=-1;
    for(uint8_t i=0;i<NOVA_RESOURCE_VERSION_CAPACITY;++i)if(used[i]&&
       entries[i].logical_id==logical_id&&entries[i].valid&&
       entries[i].version.major==minimum->major&&
       compatibility_compare(entries[i].version,*minimum)>=0&&
       compatibility_compare(entries[i].version,*maximum)<=0&&
       (best<0||nova_resource_version_compare(entries[i].version,
                                               entries[(uint8_t)best].version)>0))best=(int8_t)i;
    if(best<0){++diagnostics.conflicts;return 0;}
    int8_t previous=active_index(logical_id);
    if(previous>=0)entries[(uint8_t)previous].active=false;
    entries[(uint8_t)best].active=true;
    if(!set_active_hash(logical_id,best)||!nova_resource_version_dependencies(&entries[(uint8_t)best])){
        entries[(uint8_t)best].active=false;
        if(previous>=0){entries[(uint8_t)previous].active=true;
            (void)set_active_hash(logical_id,previous);}
        ++diagnostics.conflicts;return 0;
    }
    ++diagnostics.selections;return &entries[(uint8_t)best];
}

const nova_resource_version_entry_t *nova_resource_version_rollback(uint64_t logical_id)
{
    int8_t current=active_index(logical_id);if(current<0){++diagnostics.conflicts;return 0;}
    int8_t candidate=-1;
    for(uint8_t i=0;i<NOVA_RESOURCE_VERSION_CAPACITY;++i)if(used[i]&&
       entries[i].logical_id==logical_id&&i!=(uint8_t)current&&entries[i].valid&&
       entries[i].version.major==entries[(uint8_t)current].version.major&&
       nova_resource_version_compare(entries[i].version,entries[(uint8_t)current].version)<0&&
       (candidate<0||nova_resource_version_compare(entries[i].version,
            entries[(uint8_t)candidate].version)>0))candidate=(int8_t)i;
    if(candidate<0){++diagnostics.conflicts;return 0;}
    entries[(uint8_t)current].active=false;entries[(uint8_t)candidate].active=true;
    if(!set_active_hash(logical_id,candidate)||
       !nova_resource_version_dependencies(&entries[(uint8_t)candidate])){
        entries[(uint8_t)candidate].active=false;entries[(uint8_t)current].active=true;
        (void)set_active_hash(logical_id,current);++diagnostics.conflicts;return 0;}
    ++diagnostics.rollbacks;return &entries[(uint8_t)candidate];
}

const nova_resource_version_diagnostics_t *nova_resource_version_diagnostics(void)
{ return &diagnostics; }
