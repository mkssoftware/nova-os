#include "resources.h"
#include "memory.h"

static nova_resource_t registry[NOVA_RESOURCE_CAPACITY];
static bool used[NOVA_RESOURCE_CAPACITY];
static int16_t hash_slots[NOVA_RESOURCE_HASH_CAPACITY];
_Alignas(64) static uint8_t decoded_pool[NOVA_RESOURCE_DECODE_BLOCK_SIZE*
                                         NOVA_RESOURCE_DECODE_BLOCK_COUNT];
static bool decoded_blocks[NOVA_RESOURCE_DECODE_BLOCK_COUNT];
static nova_resource_diagnostics_t diagnostics;
static uint64_t use_clock;
static nova_resource_trace_t resource_trace[NOVA_RESOURCE_TRACE_CAPACITY];
static uint16_t resource_trace_head,resource_trace_count;
static uint64_t resource_trace_sequence,resource_trace_clock;
static nova_resource_diag_status_t resource_diag;
static nova_resource_corruption_result_t corruption_results[NOVA_CORRUPTION_RESULT_CAPACITY];
static nova_resource_corruption_status_t corruption_status;
static uint8_t corruption_buffer[NOVA_CORRUPTION_BUFFER_CAPACITY];

static uint16_t read_u16(const uint8_t *p){return (uint16_t)p[0]|(uint16_t)p[1]<<8;}
static uint32_t read_u32(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|
    (uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static uint64_t read_u64(const uint8_t *p){return (uint64_t)read_u32(p)|
    (uint64_t)read_u32(p+4)<<32;}
static bool region_valid(uint32_t offset,uint32_t length,uint32_t total)
{return !(offset&7u)&&offset<=total&&length<=total-offset;}
static uint32_t checksum_zeroed(const uint8_t *p,uint32_t size,uint32_t zero,uint32_t count)
{uint32_t crc=0xffffffffu;for(uint32_t i=0;i<size;++i){uint8_t v=i>=zero&&i<zero+count?0:p[i];
 crc^=v;for(uint8_t b=0;b<8;++b)crc=(crc>>1)^(0xedb88320u&(0u-(crc&1u)));}return ~crc;}

bool nova_bap_index_validate(const void *data,uint32_t size,uint16_t *count)
{
    const uint8_t *p=data;if(count)*count=0;
    if(!p||size<40||p[0]!='N'||p[1]!='O'||p[2]!='V'||p[3]!='A'||
       p[4]!='I'||p[5]!='D'||p[6]!='X'||p[7]||read_u16(p+8)!=1||
       read_u16(p+10)!=40||read_u16(p+12)!=64||read_u32(p+32)||read_u32(p+36)||
       checksum_zeroed(p,size,28,4)!=read_u32(p+28))return false;
    uint16_t entries=read_u16(p+14);uint32_t table=read_u32(p+16),eo=read_u32(p+20),ho=read_u32(p+24);
    if(!table||(table&(table-1))||entries*10u>table*7u||
       !region_valid(eo,(uint32_t)entries*64u,size)||
       !region_valid(ho,table*4u,size))return false;
    for(uint16_t i=0;i<entries;++i){
        const uint8_t *entry=p+eo+(uint32_t)i*64u;uint64_t id=read_u64(entry);
        uint16_t dependencies=read_u16(entry+56);uint32_t dependency_offset=read_u32(entry+52);
        if(!id||read_u32(entry+16)==0||read_u32(entry+8)>=NOVA_RESOURCE_TYPE_COUNT||
           read_u32(entry+20)>=NOVA_COMPRESSION_COUNT||read_u16(entry+58)||
           read_u32(entry+60)||!read_u64(entry+32)||!read_u64(entry+40)||
           (read_u32(entry+20)==NOVA_COMPRESSION_NONE&&read_u64(entry+32)!=read_u64(entry+40))||
           (dependencies&&(!region_valid(dependency_offset,(uint32_t)dependencies*8u,size))))
            return false;
        for(uint16_t prior=0;prior<i;++prior)
            if(read_u64(p+eo+(uint32_t)prior*64u)==id)return false;
    }
    for(uint32_t slot=0;slot<table;++slot){uint32_t index=read_u32(p+ho+slot*4u);
        if(index!=UINT32_MAX&&index>=entries)return false;
        if(index!=UINT32_MAX)for(uint32_t prior=0;prior<slot;++prior)
            if(read_u32(p+ho+prior*4u)==index)return false;}
    for(uint16_t i=0;i<entries;++i){uint64_t id=read_u64(p+eo+(uint32_t)i*64u);
        uint32_t slot=(uint32_t)id&(table-1u),probes=0;
        while(probes<table&&read_u32(p+ho+slot*4u)!=i){
            if(read_u32(p+ho+slot*4u)==UINT32_MAX)return false;
            slot=(slot+1u)&(table-1u);++probes;}
        if(probes==table)return false;}
    if(count)*count=entries;
    return true;
}

static bool overlaps(uint32_t ao,uint32_t as,uint32_t bo,uint32_t bs)
{return as&&bs&&ao<bo+bs&&bo<ao+as;}

bool nova_bap_validate(const void *data,uint32_t size,nova_bap_info_t *out)
{
    const uint8_t *p=data;if(out)*out=(nova_bap_info_t){0};
    if(!p||size<64||p[0]!='N'||p[1]!='O'||p[2]!='V'||p[3]!='A'||p[4]!='B'||
       p[5]!='A'||p[6]!='P'||p[7]||read_u16(p+8)!=1||read_u16(p+10)!=64||
       read_u32(p+12)!=size||(read_u32(p+24)&~3u)||read_u32(p+60)||
       checksum_zeroed(p,size,56,4)!=read_u32(p+56))return false;
    uint32_t io=read_u32(p+32),is=read_u32(p+36),d=read_u32(p+40),ds=read_u32(p+44),
             m=read_u32(p+48),ms=read_u32(p+52);
    if(!region_valid(io,is,size)||!region_valid(d,ds,size)||!region_valid(m,ms,size)||
       overlaps(io,is,d,ds)||overlaps(io,is,m,ms)||overlaps(d,ds,m,ms)||
       !nova_bap_index_validate(p+io,is,0)||read_u16(p+io+14)!=read_u32(p+28))return false;
    uint16_t entries=read_u16(p+io+14);uint32_t eo=read_u32(p+io+20);
    for(uint16_t i=0;i<entries;++i){const uint8_t *entry=p+io+eo+(uint32_t)i*64u;
        uint64_t offset=read_u64(entry+24),packed=read_u64(entry+32);
        if(offset<d||offset>(uint64_t)d+ds||packed>(uint64_t)d+ds-offset||
           packed>UINT32_MAX||nova_resource_checksum(p+(uint32_t)offset,packed)!=read_u32(entry+48))
            return false;
        for(uint16_t prior=0;prior<i;++prior){const uint8_t *other=p+io+eo+(uint32_t)prior*64u;
            uint64_t oo=read_u64(other+24),os=read_u64(other+32);
            if(offset<oo+os&&oo<offset+packed)return false;}}
    if(out)*out=(nova_bap_info_t){read_u64(p+16),read_u32(p+24),read_u32(p+28),io,is,d,ds,m,ms};
    return true;
}

static void resource_trace_event(const nova_resource_t *resource,uint64_t id,
    nova_resource_event_type_t event,nova_resource_result_t result,bool cache_hit)
{
    nova_resource_trace_t trace={.timestamp_us=++resource_trace_clock,
        .resource_id=resource?resource->id:id,.event=event,
        .resource_type=resource?resource->type:NOVA_RESOURCE_BINARY,.result=result,
        .state=resource?resource->state:NOVA_RESOURCE_INVALID,
        .compression=resource?resource->compression:NOVA_COMPRESSION_NONE,
        .integrity=resource?resource->trust:NOVA_RESOURCE_TRUST_UNKNOWN,
        .version=resource?resource->version:0,.packed_size=resource?resource->packed_size:0,
        .original_size=resource?resource->size:0,.cache_bytes=diagnostics.cached_bytes,
        .peak_cache_bytes=diagnostics.peak_cached_bytes,.cache_hit=cache_hit,
        .integrity_ok=resource&&resource->trust!=NOVA_RESOURCE_TRUST_INVALID&&
            resource->trust!=NOVA_RESOURCE_TRUST_CORRUPTED,
        .checksum_ok=resource&&resource->integrity_verified,
        .signature_ok=resource&&resource->signature!=NOVA_SIGNATURE_INVALID,
        .version_ok=resource&&resource->version>0};
    (void)nova_resource_diag_record(&trace);
}

static uint64_t next_use(void)
{
    if(use_clock==UINT64_MAX){
        for(uint16_t i=0;i<NOVA_RESOURCE_CAPACITY;++i)if(used[i])registry[i].last_use>>=1;
        use_clock>>=1;
    }
    return ++use_clock;
}

static void record_reference(void)
{
    if(diagnostics.current_references<UINT32_MAX)++diagnostics.current_references;
    if(diagnostics.current_references>diagnostics.peak_references)
        diagnostics.peak_references=diagnostics.current_references;
}

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
    (void)nova_resource_diag_initialize();
    (void)nova_resource_corruption_initialize();
    diagnostics.cache_budget=cache_budget?cache_budget:NOVA_RESOURCE_DEFAULT_CACHE_BUDGET;
    diagnostics.cache_critical=diagnostics.cache_budget-(diagnostics.cache_budget/8u);
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
    diagnostics.current_references=0;
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
       descriptor->cache_policy>=NOVA_CACHE_POLICY_COUNT||
       descriptor->compression>=NOVA_COMPRESSION_COUNT||
       diagnostics.registered_bytes>UINT64_MAX-descriptor->size){
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
            .cache_policy=descriptor->cache_policy,
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
        int16_t candidate=-1;uint64_t oldest=UINT64_MAX;uint32_t least_used=UINT32_MAX;
        nova_cache_policy_t selected_policy=NOVA_CACHE_LRU;
        for(uint8_t policy_pass=0;policy_pass<2&&candidate<0;++policy_pass){
            nova_cache_policy_t policy=policy_pass?NOVA_CACHE_LFU:NOVA_CACHE_LRU;
            for(int16_t i=0;i<(int16_t)NOVA_RESOURCE_CAPACITY;++i)if(used[i]&&
               i!=protected_index&&!registry[i].reference_count&&
               (registry[i].state==NOVA_RESOURCE_CACHED||
                registry[i].state==NOVA_RESOURCE_RELEASED)&&
               registry[i].cache_policy==policy){
                bool better=candidate<0||registry[i].priority>registry[candidate].priority;
                if(!better&&registry[i].priority==registry[candidate].priority)
                    better=policy==NOVA_CACHE_LRU?registry[i].last_use<oldest:
                        (registry[i].access_count<least_used||
                         (registry[i].access_count==least_used&&
                          registry[i].last_use<oldest));
                if(better){candidate=i;oldest=registry[i].last_use;
                    least_used=registry[i].access_count;selected_policy=policy;}
            }
        }
        if(candidate<0)return false;
        diagnostics.cached_bytes-=registry[candidate].size;
        nova_memory_budget_record_eviction(registry[candidate].size);
        if(registry[candidate].decoded_block_count){
            for(uint8_t block=0;block<registry[candidate].decoded_block_count;++block)
                decoded_blocks[registry[candidate].decoded_block+block]=false;
            registry[candidate].decoded_block_count=0;registry[candidate].decoded_block=0xff;
            registry[candidate].data=0;
        }
        registry[candidate].state=NOVA_RESOURCE_UNLOADED;++diagnostics.evictions;
        if(selected_policy==NOVA_CACHE_LRU)++diagnostics.lru_evictions;
        else ++diagnostics.lfu_evictions;
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
    int16_t candidate=-1;uint64_t oldest=UINT64_MAX;uint32_t least_used=UINT32_MAX;
    nova_cache_policy_t selected_policy=NOVA_CACHE_LRU;
    for(uint8_t pass=0;pass<2&&candidate<0;++pass){
        nova_cache_policy_t policy=pass?NOVA_CACHE_LFU:NOVA_CACHE_LRU;
        for(int16_t i=0;i<(int16_t)NOVA_RESOURCE_CAPACITY;++i)if(used[i]&&
           i!=protected_index&&!registry[i].reference_count&&registry[i].decoded_block_count&&
           registry[i].cache_policy==policy&&
           (registry[i].state==NOVA_RESOURCE_CACHED||registry[i].state==NOVA_RESOURCE_RELEASED)){
            bool better=candidate<0||registry[i].priority>registry[candidate].priority;
            if(!better&&registry[i].priority==registry[candidate].priority)
                better=policy==NOVA_CACHE_LRU?registry[i].last_use<oldest:
                    (registry[i].access_count<least_used||
                     (registry[i].access_count==least_used&&registry[i].last_use<oldest));
            if(better){candidate=i;oldest=registry[i].last_use;
                least_used=registry[i].access_count;selected_policy=policy;}
        }
    }
    if(candidate<0)return false;
    diagnostics.cached_bytes-=registry[candidate].size;
    nova_memory_budget_record_eviction(registry[candidate].size);
    release_decoded(&registry[candidate]);registry[candidate].state=NOVA_RESOURCE_UNLOADED;
    registry[candidate].integrity_verified=false;
    ++diagnostics.evictions;
    if(selected_policy==NOVA_CACHE_LRU)++diagnostics.lru_evictions;
    else ++diagnostics.lfu_evictions;
    return true;
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
    if(resource->reference_count){
        if(resource->reference_count==UINT32_MAX){++diagnostics.invalid_requests;
            return NOVA_RESOURCE_INVALID_STATE;}
        ++resource->reference_count;
        if(resource->access_count<UINT32_MAX)++resource->access_count;
        resource->last_use=next_use();record_reference();
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
    resource->reference_count=1;
    if(resource->access_count<UINT32_MAX)++resource->access_count;
    resource->last_use=next_use();
    record_reference();
    ++diagnostics.cache_misses;++diagnostics.loaded;if(result)*result=resource;
    return NOVA_RESOURCE_OK;
}

nova_resource_result_t nova_resource_load_mode(uint64_t id,nova_resource_load_mode_t mode,
                                                const nova_resource_t **result)
{
    if(result)*result=0;
    resource_trace_event(0,id,NOVA_RESOURCE_EVENT_REQUEST,NOVA_RESOURCE_OK,false);
    if(!diagnostics.initialized){resource_trace_event(0,id,NOVA_RESOURCE_EVENT_ERROR,
        NOVA_RESOURCE_INVALID_STATE,false);return NOVA_RESOURCE_INVALID_STATE;}
    if(!id||mode>NOVA_LOAD_BACKGROUND){++diagnostics.invalid_requests;
        resource_trace_event(0,id,NOVA_RESOURCE_EVENT_ERROR,NOVA_RESOURCE_INVALID_ARGUMENT,false);
        return NOVA_RESOURCE_INVALID_ARGUMENT;}
    if(mode==NOVA_LOAD_BACKGROUND){++diagnostics.unsupported_requests;
        resource_trace_event(0,id,NOVA_RESOURCE_EVENT_ERROR,NOVA_RESOURCE_UNSUPPORTED,false);
        return NOVA_RESOURCE_UNSUPPORTED;}
    if(diagnostics.busy){resource_trace_event(0,id,NOVA_RESOURCE_EVENT_ERROR,
        NOVA_RESOURCE_BUSY,false);return NOVA_RESOURCE_BUSY;}
    diagnostics.busy=true;
    int16_t index=lookup_index(id);nova_resource_t *requested=index>=0?&registry[index]:0;
    resource_trace_event(requested,id,NOVA_RESOURCE_EVENT_LOAD_BEGIN,NOVA_RESOURCE_OK,false);
    uint32_t validations=nova_integrity_diagnostics()->verifications;
    uint32_t decompressions=diagnostics.decompressions;
    bool was_cached=requested&&(requested->reference_count||
        requested->state==NOVA_RESOURCE_CACHED||requested->state==NOVA_RESOURCE_RELEASED);
    uint64_t visiting[2]={0};
    nova_resource_result_t status=load_index(index,visiting,1,result);
    if(status!=NOVA_RESOURCE_OK&&index>=0&&registry[index].fallback_id){
        ++diagnostics.fallback_uses;status=load_index(lookup_index(registry[index].fallback_id),
                                                     visiting,1,result);}
    const nova_resource_t *loaded=result&&*result?*result:requested;
    if(nova_integrity_diagnostics()->verifications>validations)
        resource_trace_event(loaded,id,NOVA_RESOURCE_EVENT_VALIDATE,status,false);
    if(diagnostics.decompressions>decompressions)
        resource_trace_event(loaded,id,NOVA_RESOURCE_EVENT_DECOMPRESS,status,false);
    resource_trace_event(loaded,id,was_cached?NOVA_RESOURCE_EVENT_CACHE_HIT:
        NOVA_RESOURCE_EVENT_CACHE_MISS,status,was_cached);
    resource_trace_event(loaded,id,status==NOVA_RESOURCE_OK?NOVA_RESOURCE_EVENT_LOAD_COMPLETE:
        NOVA_RESOURCE_EVENT_ERROR,status,was_cached);
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
    if(diagnostics.current_references)--diagnostics.current_references;
    if(resource->reference_count){resource->state=NOVA_RESOURCE_IN_USE;
        resource_trace_event(resource,id,NOVA_RESOURCE_EVENT_RELEASE,NOVA_RESOURCE_OK,true);return true;}
    resource->state=NOVA_RESOURCE_RELEASED;resource->last_use=next_use();
    for(uint8_t i=0;i<resource->dependency_count;++i)
        (void)nova_resource_release(resource->dependencies[i]);
    resource_trace_event(resource,id,NOVA_RESOURCE_EVENT_RELEASE,NOVA_RESOURCE_OK,true);
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

bool nova_resource_cache_configure(uint64_t minimum,uint64_t maximum,
    uint64_t reserved,uint64_t critical)
{
    if(!diagnostics.initialized||!maximum||minimum>maximum||reserved>maximum||
       critical<minimum||critical>maximum||diagnostics.cached_bytes>maximum)return false;
    diagnostics.cache_minimum=minimum;diagnostics.cache_budget=maximum;
    diagnostics.cache_reserved=reserved;diagnostics.cache_critical=critical;return true;
}

bool nova_resource_cache_set_policy(uint64_t id,nova_cache_policy_t policy)
{
    int16_t index=lookup_index(id);
    if(index<0||policy>=NOVA_CACHE_POLICY_COUNT)return false;
    registry[index].cache_policy=policy;return true;
}

uint32_t nova_resource_cache_collect(void)
{
    if(!diagnostics.initialized||diagnostics.busy)return 0;
    uint32_t collected=0;++diagnostics.collections;
    for(uint8_t pass=0;pass<2;++pass){
        nova_cache_policy_t policy=pass?NOVA_CACHE_LFU:NOVA_CACHE_LRU;
        for(;;){int16_t candidate=-1;uint64_t oldest=UINT64_MAX;
            uint32_t frequency=UINT32_MAX;
            for(int16_t i=0;i<(int16_t)NOVA_RESOURCE_CAPACITY;++i)if(used[i]&&
               !registry[i].reference_count&&registry[i].cache_policy==policy&&
               (registry[i].state==NOVA_RESOURCE_CACHED||
                registry[i].state==NOVA_RESOURCE_RELEASED)){
                bool better=candidate<0;
                if(policy==NOVA_CACHE_LRU)better=better||registry[i].last_use<oldest;
                else better=better||registry[i].access_count<frequency||
                    (registry[i].access_count==frequency&&registry[i].last_use<oldest);
                if(better){candidate=i;frequency=registry[i].access_count;
                    oldest=registry[i].last_use;}}
            if(candidate<0)break;
            uint64_t evicted_size=registry[candidate].size;
            if(nova_resource_unload(registry[candidate].id)!=NOVA_RESOURCE_OK)break;
            nova_memory_budget_record_eviction(evicted_size);
            ++collected;++diagnostics.evictions;
            if(policy==NOVA_CACHE_LRU)++diagnostics.lru_evictions;
            else ++diagnostics.lfu_evictions;
        }
    }
    for(uint16_t i=0;i<NOVA_RESOURCE_CAPACITY;++i)if(used[i]&&
        registry[i].cache_policy==NOVA_CACHE_PERMANENT&&!registry[i].reference_count&&
        (registry[i].state==NOVA_RESOURCE_CACHED||registry[i].state==NOVA_RESOURCE_RELEASED))
        ++diagnostics.permanent_skips;
    return collected;
}

const nova_resource_diagnostics_t *nova_resource_diagnostics(void){return &diagnostics;}

bool nova_resource_diag_initialize(void)
{
    for(uint16_t i=0;i<NOVA_RESOURCE_TRACE_CAPACITY;++i)
        resource_trace[i]=(nova_resource_trace_t){0};
    resource_trace_head=resource_trace_count=0;resource_trace_sequence=0;
    resource_trace_clock=0;
    resource_diag=(nova_resource_diag_status_t){.initialized=true,.passive=true,
        .read_only=true,.boot_unaffected=true};
    return true;
}
bool nova_resource_diag_record(const nova_resource_trace_t *input)
{
    if(!resource_diag.initialized||!input||input->event>NOVA_RESOURCE_EVENT_ERROR)return false;
    uint16_t out=(uint16_t)((resource_trace_head+resource_trace_count)%NOVA_RESOURCE_TRACE_CAPACITY);
    if(resource_trace_count==NOVA_RESOURCE_TRACE_CAPACITY){out=resource_trace_head;
        resource_trace_head=(uint16_t)((resource_trace_head+1)%NOVA_RESOURCE_TRACE_CAPACITY);
        ++resource_diag.overwritten;}else ++resource_trace_count;
    nova_resource_trace_t trace=*input;trace.sequence=++resource_trace_sequence;
    if(!trace.timestamp_us)trace.timestamp_us=++resource_trace_clock;
    if(trace.event==NOVA_RESOURCE_EVENT_LOAD_COMPLETE){
        for(uint32_t i=resource_trace_count-1;i>0;--i){const nova_resource_trace_t *previous=
            &resource_trace[(resource_trace_head+i-1)%NOVA_RESOURCE_TRACE_CAPACITY];
            if(previous->resource_id==trace.resource_id&&previous->event==NOVA_RESOURCE_EVENT_LOAD_BEGIN){
                trace.load_begin_us=previous->timestamp_us;trace.load_end_us=trace.timestamp_us;
                trace.total_time_us=trace.timestamp_us>=previous->timestamp_us?
                    trace.timestamp_us-previous->timestamp_us:0;break;}}
        resource_diag.total_load_us+=trace.total_time_us;
        if(!resource_diag.fastest_load_us||trace.total_time_us<resource_diag.fastest_load_us)
            resource_diag.fastest_load_us=trace.total_time_us;
        if(trace.total_time_us>resource_diag.slowest_load_us)
            resource_diag.slowest_load_us=trace.total_time_us;}
    resource_trace[out]=trace;++resource_diag.recorded;resource_diag.count=resource_trace_count;
    resource_diag.cache_bytes=trace.cache_bytes;
    if(trace.peak_cache_bytes>resource_diag.peak_cache_bytes)
        resource_diag.peak_cache_bytes=trace.peak_cache_bytes;
    if(trace.event==NOVA_RESOURCE_EVENT_REQUEST)++resource_diag.requests;
    else if(trace.event==NOVA_RESOURCE_EVENT_LOAD_COMPLETE)++resource_diag.loads;
    else if(trace.event==NOVA_RESOURCE_EVENT_VALIDATE)++resource_diag.validations;
    else if(trace.event==NOVA_RESOURCE_EVENT_DECOMPRESS)++resource_diag.decompressions;
    else if(trace.event==NOVA_RESOURCE_EVENT_CACHE_HIT)++resource_diag.cache_hits;
    else if(trace.event==NOVA_RESOURCE_EVENT_CACHE_MISS)++resource_diag.cache_misses;
    else if(trace.event==NOVA_RESOURCE_EVENT_RELEASE)++resource_diag.releases;
    else if(trace.event==NOVA_RESOURCE_EVENT_ERROR)++resource_diag.errors;
    return true;
}
const nova_resource_trace_t *nova_resource_diag_get(uint32_t chronological_index)
{
    if(chronological_index>=resource_trace_count)return 0;
    return &resource_trace[(resource_trace_head+chronological_index)%NOVA_RESOURCE_TRACE_CAPACITY];
}
static bool resource_trace_matches(const nova_resource_trace_t *trace,
    const nova_resource_diag_filter_t *filter)
{
    if(!filter)return true;
    if(filter->use_type&&trace->resource_type!=filter->type)return false;
    if(filter->use_result&&trace->result!=filter->result)return false;
    if(filter->use_integrity&&trace->integrity!=filter->integrity)return false;
    if(filter->use_time&&(trace->timestamp_us<filter->start_us||
       trace->timestamp_us>filter->end_us))return false;
    if(filter->use_boot_phase&&trace->boot_phase!=filter->boot_phase)return false;
    if(filter->use_module&&trace->module_id!=filter->module_id)return false;
    if(filter->cache_hits_only&&!trace->cache_hit)return false;
    return !filter->errors_only||trace->event==NOVA_RESOURCE_EVENT_ERROR;
}
const nova_resource_trace_t *nova_resource_diag_query(
    const nova_resource_diag_filter_t *filter,uint32_t matching_index)
{
    for(uint32_t i=0;i<resource_trace_count;++i){const nova_resource_trace_t *trace=
        nova_resource_diag_get(i);if(resource_trace_matches(trace,filter)){
            if(!matching_index)return trace;
            --matching_index;}}
    return 0;
}
static bool resource_put(char *output,uint32_t capacity,uint32_t *position,char value)
{if(*position>=capacity)return false;output[(*position)++]=value;return true;}
static bool resource_text(char *output,uint32_t capacity,uint32_t *position,const char *value)
{while(*value)if(!resource_put(output,capacity,position,*value++))return false;return true;}
static bool resource_number(char *output,uint32_t capacity,uint32_t *position,uint64_t value)
{char digits[24];uint8_t count=0;do{digits[count++]=(char)('0'+value%10u);value/=10u;}while(value);
 while(count){if(!resource_put(output,capacity,position,digits[--count]))return false;}return true;}
bool nova_resource_diag_export(nova_resource_export_format_t format,
    bool authorized,uint8_t *output,uint32_t capacity,uint32_t *written)
{
    if(written)*written=0;
    if(!authorized||!output||!written){++resource_diag.denied_exports;return false;}
    uint32_t position=0;
    if(format==NOVA_RESOURCE_EXPORT_BINARY){uint32_t required=
        (uint32_t)resource_trace_count*sizeof(nova_resource_trace_t);if(required>capacity)return false;
        for(uint32_t i=0;i<resource_trace_count;++i){const uint8_t *source=
            (const uint8_t*)nova_resource_diag_get(i);for(uint32_t j=0;j<sizeof(nova_resource_trace_t);++j)
                output[position++]=source[j];}}
    else {char *text=(char*)output;
        if(format==NOVA_RESOURCE_EXPORT_NDF&&!resource_text(text,capacity,&position,"NDF-RESOURCE-1\n"))return false;
        if(format==NOVA_RESOURCE_EXPORT_JSON&&!resource_text(text,capacity,&position,"{\"resources\":["))return false;
        if(format==NOVA_RESOURCE_EXPORT_CSV&&!resource_text(text,capacity,&position,"sequence,timestamp,id,event,type,result,duration,cache_hit\n"))return false;
        for(uint32_t i=0;i<resource_trace_count;++i){const nova_resource_trace_t *trace=nova_resource_diag_get(i);
            if(format==NOVA_RESOURCE_EXPORT_JSON){if(i&&!resource_put(text,capacity,&position,','))return false;
                if(!resource_text(text,capacity,&position,"{\"sequence\":"))return false;}
            uint64_t values[7]={trace->sequence,trace->timestamp_us,trace->resource_id,
                trace->event,trace->resource_type,trace->result,trace->total_time_us};
            for(uint8_t v=0;v<7;++v){if(!resource_number(text,capacity,&position,values[v]))return false;
                if(v<6&&!resource_put(text,capacity,&position,format==NOVA_RESOURCE_EXPORT_CSV?',':' '))return false;}
            if(format==NOVA_RESOURCE_EXPORT_JSON){if(!resource_text(text,capacity,&position,"}"))return false;}
            else {if(!resource_put(text,capacity,&position,format==NOVA_RESOURCE_EXPORT_CSV?',':' '))return false;
                if(!resource_number(text,capacity,&position,trace->cache_hit?1u:0u)||
                   !resource_put(text,capacity,&position,'\n'))return false;}}
        if(format==NOVA_RESOURCE_EXPORT_JSON&&!resource_text(text,capacity,&position,"]}"))return false;
        if(position<capacity)text[position]=0;}
    *written=position;++resource_diag.exports;return true;
}
void nova_resource_diag_reset(void){(void)nova_resource_diag_initialize();}
const nova_resource_diag_status_t *nova_resource_diag_status(void){return &resource_diag;}

bool nova_resource_corruption_initialize(void)
{
    for(uint8_t i=0;i<NOVA_CORRUPTION_RESULT_CAPACITY;++i)
        corruption_results[i]=(nova_resource_corruption_result_t){0};
    corruption_status=(nova_resource_corruption_status_t){.initialized=true,.isolated=true,
        .deterministic=true,.heap_free=true,.productive_data_unchanged=true};
    return true;
}
bool nova_resource_corruption_execute(uint64_t resource_id,nova_corruption_type_t corruption)
{
    if(!corruption_status.initialized||!resource_id||corruption==NOVA_CORRUPTION_NONE||
       corruption>=NOVA_CORRUPTION_TYPE_COUNT||corruption_status.count==NOVA_CORRUPTION_RESULT_CAPACITY)
        return false;
    int16_t index=lookup_index(resource_id);if(index<0)return false;
    const nova_resource_t *source=&registry[index];if(!source->packed_data||
       source->packed_size>NOVA_CORRUPTION_BUFFER_CAPACITY)return false;
    for(uint64_t i=0;i<source->packed_size;++i)
        corruption_buffer[i]=((const uint8_t*)source->packed_data)[i];
    nova_resource_descriptor_t descriptor={.uri="test://corruption/probe",
        .type=source->type,.version=source->version,.data=corruption_buffer,
        .size=source->packed_size,.checksum=source->packed_checksum,
        .fallback_id=source->fallback_id,.origin=source->origin,.priority=source->priority,
        .cache_policy=source->cache_policy,.compression=source->compression,
        .original_size=source->size,.original_checksum=source->checksum,
        .packed_sha256=source->packed_sha256,.original_sha256=source->original_sha256,
        .signature=source->signature};
    if(corruption==NOVA_CORRUPTION_BIT_ERROR)corruption_buffer[source->packed_size/2]^=1u;
    else if(corruption==NOVA_CORRUPTION_TRUNCATED)descriptor.size=source->packed_size-1u;
    else if(corruption==NOVA_CORRUPTION_EMPTY)descriptor.size=0;
    else if(corruption==NOVA_CORRUPTION_INVALID_HEADER)corruption_buffer[0]^=0xffu;
    else if(corruption==NOVA_CORRUPTION_INVALID_SIGNATURE)descriptor.signature=NOVA_SIGNATURE_INVALID;
    else if(corruption==NOVA_CORRUPTION_INVALID_CHECKSUM)descriptor.checksum^=0xffffffffu;
    else if(corruption==NOVA_CORRUPTION_INVALID_VERSION)descriptor.version=0;
    else {descriptor.type=NOVA_RESOURCE_TYPE_COUNT;}
    uint64_t before=diagnostics.cached_bytes;uint32_t registered_before=diagnostics.registered;
    nova_resource_result_t result=nova_resource_register_descriptor(&descriptor,0);
    bool detected=result!=NOVA_RESOURCE_OK;
    bool fallback=detected&&source->fallback_id&&lookup_index(source->fallback_id)>=0;
    nova_resource_corruption_result_t record={.resource_id=resource_id,
        .fallback_id=source->fallback_id,.resource_type=source->type,.corruption=corruption,
        .classification=fallback?NOVA_CORRUPTION_RECOVERABLE:
            source->priority<=NOVA_RESOURCE_PRIORITY_HIGH?NOVA_CORRUPTION_CRITICAL:
            NOVA_CORRUPTION_WARNING,.error_code=result,.source_size=source->packed_size,
        .test_size=descriptor.size,.memory_before=before,.memory_after=diagnostics.cached_bytes,
        .integrity_detected=detected,.fallback_successful=fallback,.boot_continued=detected,
        .source_unchanged=nova_resource_checksum(source->packed_data,source->packed_size)==
            source->packed_checksum,.pool_consistent=detected&&diagnostics.registered==registered_before,
        .manipulated_used=!detected};
    corruption_results[corruption_status.count++]=record;++corruption_status.executed;
    if(detected)++corruption_status.detected;
    if(fallback)++corruption_status.fallbacks;
    if(record.boot_continued)++corruption_status.continued;
    if(record.classification==NOVA_CORRUPTION_WARNING)++corruption_status.warnings;
    else if(record.classification==NOVA_CORRUPTION_RECOVERABLE)++corruption_status.recoverable;
    else if(record.classification==NOVA_CORRUPTION_CRITICAL)++corruption_status.critical;
    else ++corruption_status.fatal;
    if(!record.source_unchanged||!record.pool_consistent||record.manipulated_used)
        corruption_status.productive_data_unchanged=false;
    return detected&&record.source_unchanged&&record.pool_consistent&&!record.manipulated_used;
}
const nova_resource_corruption_result_t *nova_resource_corruption_results(void)
{return corruption_results;}
const nova_resource_corruption_status_t *nova_resource_corruption_status(void)
{return &corruption_status;}
bool nova_resource_corruption_generate_report(bool authorized,uint8_t *output,
    uint32_t capacity,uint32_t *written)
{
    if(written)*written=0;
    if(!authorized||!output||!written)return false;
    const char *header="NOVA-RESOURCE-CORRUPTION-1\nid type corruption detected fallback continued class error\n";
    uint32_t position=0;while(*header){if(position>=capacity)return false;
        output[position++]=(uint8_t)*header++;}
    for(uint8_t i=0;i<corruption_status.count;++i){const nova_resource_corruption_result_t *record=
        &corruption_results[i];uint64_t values[8]={record->resource_id,record->resource_type,
        record->corruption,record->integrity_detected,record->fallback_successful,
        record->boot_continued,record->classification,record->error_code};
        for(uint8_t v=0;v<8;++v){char digits[24];uint8_t count=0;
            do{digits[count++]=(char)('0'+values[v]%10u);values[v]/=10u;}while(values[v]);
            while(count){if(position>=capacity)return false;output[position++]=(uint8_t)digits[--count];}
            if(position>=capacity)return false;
            output[position++]=(uint8_t)(v==7?'\n':' ');}}
    *written=position;++corruption_status.reports;return true;
}
