#include "resources.h"

static nova_resource_t registry[NOVA_RESOURCE_CAPACITY];
static bool used[NOVA_RESOURCE_CAPACITY];
static int16_t hash_slots[NOVA_RESOURCE_HASH_CAPACITY];
_Alignas(64) static uint8_t decoded_pool[NOVA_RESOURCE_DECODE_BLOCK_SIZE*
                                         NOVA_RESOURCE_DECODE_BLOCK_COUNT];
static bool decoded_blocks[NOVA_RESOURCE_DECODE_BLOCK_COUNT];
static nova_resource_diagnostics_t diagnostics;
static uint32_t use_clock;

static void copy_name(char *destination,const char *source)
{
    uint32_t i=0;for(;source&&source[i]&&i+1<NOVA_RESOURCE_NAME_CAPACITY;++i)
        destination[i]=source[i];
    destination[i]=0;
}

static bool same_name(const char *a,const char *b)
{
    uint32_t i=0;if(!a||!b)return false;
    while(a[i]&&b[i]&&a[i]==b[i])++i;
    return a[i]==b[i];
}

uint64_t nova_resource_id(const char *uri)
{
    if(!uri||!*uri)return 0;
    uint64_t hash=1469598103934665603ull;
    while(*uri){hash^=(uint8_t)*uri++;hash*=1099511628211ull;}
    return hash?hash:1;
}

uint32_t nova_resource_checksum(const void *data,uint64_t size)
{
    if(!data||!size)return 0;
    const uint8_t *bytes=data;uint32_t crc=0xffffffffu;
    for(uint64_t i=0;i<size;++i){crc^=bytes[i];for(uint8_t bit=0;bit<8;++bit)
        crc=(crc>>1)^(0xedb88320u&(0u-(crc&1u)));}return ~crc;
}

static int16_t lookup_index(uint64_t id)
{
    if(!id||!diagnostics.initialized)return -1;
    uint32_t slot=(uint32_t)id&255u;
    for(uint16_t probe=0;probe<NOVA_RESOURCE_HASH_CAPACITY;++probe){
        int16_t index=hash_slots[(slot+probe)&255u];if(index<0)return -1;
        if(used[index]&&registry[index].id==id)return index;
    }
    return -1;
}

static bool insert_hash(uint64_t id,int16_t index)
{
    uint32_t slot=(uint32_t)id&255u;
    for(uint16_t probe=0;probe<NOVA_RESOURCE_HASH_CAPACITY;++probe){
        int16_t *entry=&hash_slots[(slot+probe)&255u];
        if(*entry<0){*entry=index;return true;}
    }
    return false;
}

bool nova_resource_loader_initialize(uint64_t cache_budget)
{
    for(uint16_t i=0;i<NOVA_RESOURCE_CAPACITY;++i){used[i]=false;registry[i]=(nova_resource_t){0};}
    for(uint16_t i=0;i<NOVA_RESOURCE_HASH_CAPACITY;++i)hash_slots[i]=-1;
    for(uint8_t i=0;i<NOVA_RESOURCE_DECODE_BLOCK_COUNT;++i)decoded_blocks[i]=false;
    diagnostics=(nova_resource_diagnostics_t){0};use_clock=0;
    diagnostics.cache_budget=cache_budget?cache_budget:NOVA_RESOURCE_DEFAULT_CACHE_BUDGET;
    diagnostics.initialized=nova_compression_initialize()&&
        nova_integrity_initialize(NOVA_INTEGRITY_STANDARD);return diagnostics.initialized;
}

void nova_resource_manager_initialize(void)
{(void)nova_resource_loader_initialize(NOVA_RESOURCE_DEFAULT_CACHE_BUDGET);}

void nova_resource_manager_shutdown(void)
{
    if(!diagnostics.initialized)return;
    for(uint16_t i=0;i<NOVA_RESOURCE_CAPACITY;++i)if(used[i]){
        registry[i].reference_count=0;registry[i].state=NOVA_RESOURCE_UNLOADED;}
    for(uint8_t i=0;i<NOVA_RESOURCE_DECODE_BLOCK_COUNT;++i)decoded_blocks[i]=false;
    diagnostics.cached_bytes=0;diagnostics.busy=false;diagnostics.initialized=false;
    ++diagnostics.shutdowns;
}

const nova_resource_t *nova_resource_find(uint64_t id)
{
    int16_t index=lookup_index(id);
    if(index<0){++diagnostics.lookup_misses;return 0;}
    ++diagnostics.lookup_hits;return &registry[index];
}

nova_resource_result_t nova_resource_register_descriptor(
    const nova_resource_descriptor_t *descriptor,nova_resource_t **result)
{
    if(result)*result=0;
    if(!diagnostics.initialized)return NOVA_RESOURCE_INVALID_STATE;
    if(!descriptor||!descriptor->uri||!*descriptor->uri||
       descriptor->type>=NOVA_RESOURCE_TYPE_COUNT||!descriptor->version||
       !descriptor->data||!descriptor->size||descriptor->origin>NOVA_RESOURCE_ORIGIN_RECOVERY||
       descriptor->priority>NOVA_RESOURCE_PRIORITY_LOW||
       descriptor->compression>=NOVA_COMPRESSION_COUNT){
        ++diagnostics.invalid_requests;return NOVA_RESOURCE_INVALID_ARGUMENT;}
    uint64_t id=nova_resource_id(descriptor->uri);int16_t existing=lookup_index(id);
    if(existing>=0){++diagnostics.duplicate_requests;
        return same_name(registry[existing].name,descriptor->uri)?NOVA_RESOURCE_BUSY:
            NOVA_RESOURCE_INTEGRITY_ERROR;}
    uint32_t actual=nova_resource_checksum(descriptor->data,descriptor->size);
    if(!actual||(descriptor->checksum&&descriptor->checksum!=actual)){
        ++diagnostics.integrity_errors;return NOVA_RESOURCE_INTEGRITY_ERROR;}
    nova_integrity_descriptor_t integrity={.resource_id=id,.type=(uint32_t)descriptor->type+1,
        .version=descriptor->version,.data=descriptor->data,.size=descriptor->size,
        .expected_crc32=descriptor->checksum?descriptor->checksum:actual,
        .expected_sha256=descriptor->packed_sha256,.signature=descriptor->signature,
        .trusted_origin=descriptor->origin==NOVA_RESOURCE_ORIGIN_EMBEDDED};
    nova_integrity_report_t integrity_report;
    if(nova_resource_verify(&integrity,&integrity_report)!=NOVA_INTEGRITY_OK){
        ++diagnostics.integrity_errors;return NOVA_RESOURCE_INTEGRITY_ERROR;}
    bool compressed=descriptor->compression!=NOVA_COMPRESSION_NONE;
    uint64_t original_size=compressed?descriptor->original_size:descriptor->size;
    uint32_t original_checksum=compressed?descriptor->original_checksum:actual;
    if(!original_size||!original_checksum||
       (!compressed&&descriptor->original_size&&descriptor->original_size!=descriptor->size)){
        ++diagnostics.invalid_requests;return NOVA_RESOURCE_INVALID_ARGUMENT;}
    for(int16_t i=0;i<(int16_t)NOVA_RESOURCE_CAPACITY;++i)if(!used[i]){
        nova_resource_t resource={.id=id,.fallback_id=descriptor->fallback_id,
            .type=descriptor->type,.state=NOVA_RESOURCE_REGISTERED,
            .origin=descriptor->origin,.priority=descriptor->priority,
            .version=descriptor->version,.checksum=original_checksum,.size=original_size,
            .data=compressed?0:descriptor->data,.packed_data=descriptor->data,
            .packed_size=descriptor->size,.packed_checksum=actual,
            .compression=descriptor->compression,.packed_sha256=descriptor->packed_sha256,
            .original_sha256=descriptor->original_sha256,.signature=descriptor->signature,
            .trust=integrity_report.trust,.decoded_block=0xff};
        copy_name(resource.name,descriptor->uri);
        if(!insert_hash(id,i))return NOVA_RESOURCE_NO_MEMORY;
        registry[i]=resource;used[i]=true;++diagnostics.registered;
        diagnostics.registered_bytes+=descriptor->size;if(result)*result=&registry[i];
        return NOVA_RESOURCE_OK;
    }
    return NOVA_RESOURCE_NO_MEMORY;
}

bool nova_resource_register(const char *uri,nova_resource_type_t type,
    uint32_t version,const void *data,uint64_t size,uint32_t checksum,uint64_t fallback_id)
{
    nova_resource_descriptor_t descriptor={.uri=uri,.type=type,.version=version,
        .data=data,.size=size,.checksum=checksum,.fallback_id=fallback_id,
        .origin=NOVA_RESOURCE_ORIGIN_EMBEDDED,.priority=NOVA_RESOURCE_PRIORITY_NORMAL,
        .compression=NOVA_COMPRESSION_NONE};
    return nova_resource_register_descriptor(&descriptor,0)==NOVA_RESOURCE_OK;
}

bool nova_resource_add_dependency(uint64_t id,uint64_t dependency_id)
{
    int16_t index=lookup_index(id),dependency=lookup_index(dependency_id);
    if(index<0||dependency<0||id==dependency_id)return false;
    nova_resource_t *resource=&registry[index];
    for(uint8_t i=0;i<resource->dependency_count;++i)
        if(resource->dependencies[i]==dependency_id)return true;
    if(resource->dependency_count==NOVA_RESOURCE_DEPENDENCY_CAPACITY)return false;
    resource->dependencies[resource->dependency_count++]=dependency_id;return true;
}

static bool evict_for(uint64_t required,int16_t protected_index)
{
    if(required>diagnostics.cache_budget)return false;
    while(diagnostics.cached_bytes>diagnostics.cache_budget-required){
        int16_t candidate=-1;uint32_t oldest=UINT32_MAX;
        for(int16_t i=0;i<(int16_t)NOVA_RESOURCE_CAPACITY;++i)if(used[i]&&i!=protected_index&&
           !registry[i].reference_count&&(registry[i].state==NOVA_RESOURCE_CACHED||
           registry[i].state==NOVA_RESOURCE_RELEASED)&&registry[i].last_use<=oldest){
            if(candidate<0||registry[i].priority>registry[candidate].priority||
               (registry[i].priority==registry[candidate].priority&&
                registry[i].last_use<oldest)){candidate=i;oldest=registry[i].last_use;}}
        if(candidate<0)return false;
        diagnostics.cached_bytes-=registry[candidate].size;
        if(registry[candidate].decoded_block_count){
            for(uint8_t block=0;block<registry[candidate].decoded_block_count;++block)
                decoded_blocks[registry[candidate].decoded_block+block]=false;
            registry[candidate].decoded_block_count=0;registry[candidate].decoded_block=0xff;
            registry[candidate].data=0;
        }
        registry[candidate].state=NOVA_RESOURCE_UNLOADED;++diagnostics.evictions;
        registry[candidate].integrity_verified=false;
    }
    return true;
}

static void *allocate_decoded(nova_resource_t *resource)
{
    if(!resource||!resource->size||resource->size>
       (uint64_t)NOVA_RESOURCE_DECODE_BLOCK_SIZE*NOVA_RESOURCE_DECODE_BLOCK_COUNT)return 0;
    uint8_t required=(uint8_t)((resource->size+NOVA_RESOURCE_DECODE_BLOCK_SIZE-1)/
                               NOVA_RESOURCE_DECODE_BLOCK_SIZE);
    for(uint8_t start=0;start+required<=NOVA_RESOURCE_DECODE_BLOCK_COUNT;++start){
        bool free=true;for(uint8_t block=0;block<required;++block)
            if(decoded_blocks[start+block]){free=false;start=(uint8_t)(start+block);break;}
        if(!free)continue;
        for(uint8_t block=0;block<required;++block)decoded_blocks[start+block]=true;
        resource->decoded_block=start;resource->decoded_block_count=required;
        return decoded_pool+(uint32_t)start*NOVA_RESOURCE_DECODE_BLOCK_SIZE;
    }
    return 0;
}

static void release_decoded(nova_resource_t *resource)
{
    if(!resource||!resource->decoded_block_count)return;
    for(uint8_t block=0;block<resource->decoded_block_count;++block)
        decoded_blocks[resource->decoded_block+block]=false;
    resource->decoded_block_count=0;resource->decoded_block=0xff;resource->data=0;
}

static bool evict_decoded_for(int16_t protected_index)
{
    int16_t candidate=-1;uint32_t oldest=UINT32_MAX;
    for(int16_t i=0;i<(int16_t)NOVA_RESOURCE_CAPACITY;++i)if(used[i]&&
       i!=protected_index&&!registry[i].reference_count&&registry[i].decoded_block_count&&
       (registry[i].state==NOVA_RESOURCE_CACHED||registry[i].state==NOVA_RESOURCE_RELEASED)){
        if(candidate<0||registry[i].priority>registry[candidate].priority||
           (registry[i].priority==registry[candidate].priority&&
            registry[i].last_use<oldest)){candidate=i;oldest=registry[i].last_use;}}
    if(candidate<0)return false;
    diagnostics.cached_bytes-=registry[candidate].size;
    release_decoded(&registry[candidate]);registry[candidate].state=NOVA_RESOURCE_UNLOADED;
    registry[candidate].integrity_verified=false;
    ++diagnostics.evictions;return true;
}

static void invalidate_resource(nova_resource_t *resource)
{
    if(!resource)return;
    if(resource->state==NOVA_RESOURCE_CACHED||resource->state==NOVA_RESOURCE_RELEASED)
        diagnostics.cached_bytes-=resource->size;
    release_decoded(resource);resource->state=NOVA_RESOURCE_INVALID;
    ++diagnostics.integrity_errors;
}

static bool verify_resource_data(nova_resource_t *resource,const void *data,uint64_t size,
                                 uint32_t checksum,const uint8_t *sha256)
{
    nova_integrity_descriptor_t descriptor={.resource_id=resource->id,
        .type=(uint32_t)resource->type+1,.version=resource->version,.data=data,.size=size,
        .expected_crc32=checksum,.expected_sha256=sha256,.signature=resource->signature,
        .trusted_origin=resource->origin==NOVA_RESOURCE_ORIGIN_EMBEDDED};
    nova_integrity_report_t report;
    if(nova_resource_verify(&descriptor,&report)!=NOVA_INTEGRITY_OK)return false;
    resource->trust=report.trust;return true;
}

static nova_resource_result_t load_index(int16_t index,uint64_t *visiting,uint8_t depth,
                                         const nova_resource_t **result)
{
    if(index<0)return NOVA_RESOURCE_NOT_FOUND;
    nova_resource_t *resource=&registry[index];uint64_t bit=1ull<<(index&63);
    if(visiting[index>>6]&bit){++diagnostics.cycles;return NOVA_RESOURCE_CYCLE;}
    if(depth>diagnostics.max_dependency_depth)diagnostics.max_dependency_depth=depth;
    if(resource->state==NOVA_RESOURCE_INVALID)return NOVA_RESOURCE_INTEGRITY_ERROR;
    if(!resource->integrity_verified&&
       !verify_resource_data(resource,resource->packed_data,resource->packed_size,
                            resource->packed_checksum,resource->packed_sha256)){
        invalidate_resource(resource);return NOVA_RESOURCE_INTEGRITY_ERROR;}
    if(!resource->integrity_verified&&resource->compression==NOVA_COMPRESSION_NONE&&
       !verify_resource_data(resource,resource->data,resource->size,resource->checksum,
                            resource->original_sha256)){
        invalidate_resource(resource);return NOVA_RESOURCE_INTEGRITY_ERROR;}
    if(resource->reference_count){++resource->reference_count;resource->last_use=++use_clock;
        ++diagnostics.cache_hits;nova_integrity_record_cache_skip();
        if(result)*result=resource;
        return NOVA_RESOURCE_OK;}
    bool cached=resource->state==NOVA_RESOURCE_CACHED||resource->state==NOVA_RESOURCE_RELEASED;
    visiting[index>>6]|=bit;resource->state=NOVA_RESOURCE_LOADING;
    uint8_t dependencies_loaded=0;
    for(uint8_t i=0;i<resource->dependency_count;++i){int16_t dependency=lookup_index(resource->dependencies[i]);
        nova_resource_result_t status=load_index(dependency,visiting,(uint8_t)(depth+1),0);
        if(status!=NOVA_RESOURCE_OK){++diagnostics.dependency_failures;
            while(dependencies_loaded){--dependencies_loaded;
                nova_resource_release(resource->dependencies[dependencies_loaded]);}
            visiting[index>>6]&=~bit;resource->state=NOVA_RESOURCE_REGISTERED;return status;}
        ++dependencies_loaded;++diagnostics.dependency_loads;
    }
    if(!cached&&!evict_for(resource->size,index)){
        while(dependencies_loaded){--dependencies_loaded;
            nova_resource_release(resource->dependencies[dependencies_loaded]);}
        visiting[index>>6]&=~bit;resource->state=NOVA_RESOURCE_REGISTERED;
        return NOVA_RESOURCE_NO_MEMORY;}
    if(resource->compression!=NOVA_COMPRESSION_NONE&&!resource->data){
        void *destination=allocate_decoded(resource);
        while(!destination&&evict_decoded_for(index))destination=allocate_decoded(resource);
        if(!destination){while(dependencies_loaded){--dependencies_loaded;
                nova_resource_release(resource->dependencies[dependencies_loaded]);}
            visiting[index>>6]&=~bit;resource->state=NOVA_RESOURCE_REGISTERED;
            return NOVA_RESOURCE_NO_MEMORY;}
        uint64_t written=0;nova_compression_result_t decompressed=nova_resource_decompress(
            resource->compression,resource->packed_data,resource->packed_size,destination,
            resource->size,&written);
        if(decompressed!=NOVA_COMPRESSION_OK||written!=resource->size||
           nova_resource_checksum(destination,resource->size)!=resource->checksum){
            ++diagnostics.decompression_errors;invalidate_resource(resource);
            while(dependencies_loaded){--dependencies_loaded;
                nova_resource_release(resource->dependencies[dependencies_loaded]);}
            visiting[index>>6]&=~bit;resource->state=NOVA_RESOURCE_INVALID;
            return NOVA_RESOURCE_INTEGRITY_ERROR;}
        resource->data=destination;
        if(!verify_resource_data(resource,resource->data,resource->size,resource->checksum,
                                 resource->original_sha256)){
            ++diagnostics.decompression_errors;invalidate_resource(resource);
            while(dependencies_loaded){--dependencies_loaded;
                nova_resource_release(resource->dependencies[dependencies_loaded]);}
            visiting[index>>6]&=~bit;return NOVA_RESOURCE_INTEGRITY_ERROR;}
        ++diagnostics.decompressions;diagnostics.compressed_bytes+=resource->packed_size;
        diagnostics.decoded_bytes+=resource->size;
    }
    resource->integrity_verified=true;
    if(!cached){diagnostics.cached_bytes+=resource->size;
        if(diagnostics.cached_bytes>diagnostics.peak_cached_bytes)
            diagnostics.peak_cached_bytes=diagnostics.cached_bytes;}
    visiting[index>>6]&=~bit;resource->state=NOVA_RESOURCE_IN_USE;
    resource->reference_count=1;resource->last_use=++use_clock;
    ++diagnostics.cache_misses;++diagnostics.loaded;if(result)*result=resource;
    return NOVA_RESOURCE_OK;
}

nova_resource_result_t nova_resource_load_mode(uint64_t id,nova_resource_load_mode_t mode,
                                                const nova_resource_t **result)
{
    if(result)*result=0;
    if(!diagnostics.initialized)return NOVA_RESOURCE_INVALID_STATE;
    if(!id||mode>NOVA_LOAD_BACKGROUND){++diagnostics.invalid_requests;
        return NOVA_RESOURCE_INVALID_ARGUMENT;}
    if(mode==NOVA_LOAD_BACKGROUND){++diagnostics.unsupported_requests;
        return NOVA_RESOURCE_UNSUPPORTED;}
    if(diagnostics.busy)return NOVA_RESOURCE_BUSY;
    diagnostics.busy=true;
    int16_t index=lookup_index(id);uint64_t visiting[2]={0};
    nova_resource_result_t status=load_index(index,visiting,1,result);
    if(status!=NOVA_RESOURCE_OK&&index>=0&&registry[index].fallback_id){
        ++diagnostics.fallback_uses;status=load_index(lookup_index(registry[index].fallback_id),
                                                     visiting,1,result);}
    diagnostics.busy=false;return status;
}

const nova_resource_t *nova_resource_load(uint64_t id)
{const nova_resource_t *resource=0;return nova_resource_load_mode(id,NOVA_LOAD_LAZY,&resource)==
    NOVA_RESOURCE_OK?resource:0;}
bool nova_resource_loaded(uint64_t id)
{int16_t index=lookup_index(id);return index>=0&&registry[index].state>=NOVA_RESOURCE_LOADED&&
    registry[index].state<NOVA_RESOURCE_INVALID&&registry[index].state!=NOVA_RESOURCE_UNLOADED;}

bool nova_resource_preload(uint64_t id)
{
    const nova_resource_t *resource=0;
    if(nova_resource_load_mode(id,NOVA_LOAD_PRELOAD,&resource)!=NOVA_RESOURCE_OK)return false;
    ++diagnostics.preloads;return nova_resource_release(resource->id);
}

uint32_t nova_resource_preload_priority(nova_resource_priority_t maximum)
{
    if(maximum>NOVA_RESOURCE_PRIORITY_LOW)return 0;
    uint32_t count=0;
    for(uint8_t priority=NOVA_RESOURCE_PRIORITY_CRITICAL;priority<=maximum;++priority)
        for(uint16_t i=0;i<NOVA_RESOURCE_CAPACITY;++i)if(used[i]&&registry[i].priority==priority&&
            nova_resource_preload(registry[i].id))++count;
    return count;
}

bool nova_resource_release(uint64_t id)
{
    int16_t index=lookup_index(id);if(index<0||!registry[index].reference_count)return false;
    nova_resource_t *resource=&registry[index];--resource->reference_count;++diagnostics.releases;
    if(resource->reference_count){resource->state=NOVA_RESOURCE_IN_USE;return true;}
    resource->state=NOVA_RESOURCE_RELEASED;resource->last_use=++use_clock;
    for(uint8_t i=0;i<resource->dependency_count;++i)
        (void)nova_resource_release(resource->dependencies[i]);
    return true;
}

nova_resource_result_t nova_resource_unload(uint64_t id)
{
    int16_t index=lookup_index(id);if(index<0)return NOVA_RESOURCE_NOT_FOUND;
    nova_resource_t *resource=&registry[index];if(resource->reference_count)return NOVA_RESOURCE_BUSY;
    if(resource->state==NOVA_RESOURCE_CACHED||resource->state==NOVA_RESOURCE_RELEASED)
        diagnostics.cached_bytes-=resource->size;
    release_decoded(resource);
    resource->integrity_verified=false;resource->trust=NOVA_RESOURCE_TRUST_UNKNOWN;
    resource->state=NOVA_RESOURCE_UNLOADED;++diagnostics.unloaded;return NOVA_RESOURCE_OK;
}

const nova_resource_diagnostics_t *nova_resource_diagnostics(void){return &diagnostics;}
