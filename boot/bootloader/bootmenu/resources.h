#ifndef NOVA_BOOT_RESOURCES_H
#define NOVA_BOOT_RESOURCES_H

#include <stdbool.h>
#include <stdint.h>
#include "compression.h"
#include "integrity.h"

#define NOVA_RESOURCE_CAPACITY 128u
#define NOVA_RESOURCE_HASH_CAPACITY 256u
#define NOVA_RESOURCE_NAME_CAPACITY 96u
#define NOVA_RESOURCE_DEPENDENCY_CAPACITY 8u
#define NOVA_RESOURCE_DEFAULT_CACHE_BUDGET (8u*1024u*1024u)
#define NOVA_RESOURCE_DECODE_BLOCK_SIZE 8192u
#define NOVA_RESOURCE_DECODE_BLOCK_COUNT 64u
#define NOVA_RESOURCE_TRACE_CAPACITY 256u
#define NOVA_CORRUPTION_RESULT_CAPACITY 32u
#define NOVA_CORRUPTION_BUFFER_CAPACITY 65536u

typedef enum {
    NOVA_RESOURCE_IMAGE,NOVA_RESOURCE_FONT,NOVA_RESOURCE_THEME,
    NOVA_RESOURCE_ANIMATION,NOVA_RESOURCE_LAYOUT,NOVA_RESOURCE_LOCALIZATION,
    NOVA_RESOURCE_CONFIGURATION,NOVA_RESOURCE_ICON,NOVA_RESOURCE_CURSOR,
    NOVA_RESOURCE_BINARY,NOVA_RESOURCE_TYPE_COUNT
} nova_resource_type_t;
typedef enum {
    NOVA_RESOURCE_REGISTERED,NOVA_RESOURCE_UNLOADED,NOVA_RESOURCE_LOADING,
    NOVA_RESOURCE_LOADED,NOVA_RESOURCE_VERIFIED,NOVA_RESOURCE_CACHED,
    NOVA_RESOURCE_IN_USE,NOVA_RESOURCE_RELEASED,NOVA_RESOURCE_INVALID
} nova_resource_state_t;
typedef enum {NOVA_RESOURCE_ORIGIN_EMBEDDED,NOVA_RESOURCE_ORIGIN_PACKAGE,
    NOVA_RESOURCE_ORIGIN_RECOVERY} nova_resource_origin_t;
typedef enum {NOVA_RESOURCE_PRIORITY_CRITICAL,NOVA_RESOURCE_PRIORITY_HIGH,
    NOVA_RESOURCE_PRIORITY_NORMAL,NOVA_RESOURCE_PRIORITY_LOW} nova_resource_priority_t;
typedef enum {NOVA_LOAD_LAZY,NOVA_LOAD_PRELOAD,NOVA_LOAD_BACKGROUND}
nova_resource_load_mode_t;
typedef enum {NOVA_CACHE_LRU,NOVA_CACHE_LFU,NOVA_CACHE_PERMANENT,
    NOVA_CACHE_POLICY_COUNT} nova_cache_policy_t;
typedef enum {NOVA_RESOURCE_OK,NOVA_RESOURCE_NOT_FOUND,NOVA_RESOURCE_INVALID_ARGUMENT,
    NOVA_RESOURCE_INTEGRITY_ERROR,NOVA_RESOURCE_VERSION_ERROR,NOVA_RESOURCE_TYPE_ERROR,
    NOVA_RESOURCE_NO_MEMORY,NOVA_RESOURCE_DEPENDENCY_ERROR,NOVA_RESOURCE_CYCLE,
    NOVA_RESOURCE_BUSY,NOVA_RESOURCE_UNSUPPORTED,NOVA_RESOURCE_INVALID_STATE}
nova_resource_result_t;

typedef struct {uint64_t package_id;uint32_t flags,resource_count,index_offset,
    index_size,data_offset,data_size,manifest_offset,manifest_size;} nova_bap_info_t;
bool nova_bap_validate(const void *package,uint32_t size,nova_bap_info_t *info);
bool nova_bap_index_validate(const void *index,uint32_t size,uint16_t *entry_count);

typedef struct {
    const char *uri;
    nova_resource_type_t type;
    uint32_t version;
    const void *data;
    uint64_t size;
    uint32_t checksum;
    uint64_t fallback_id;
    nova_resource_origin_t origin;
    nova_resource_priority_t priority;
    nova_cache_policy_t cache_policy;
    nova_compression_type_t compression;
    uint64_t original_size;
    uint32_t original_checksum;
    const uint8_t *packed_sha256;
    const uint8_t *original_sha256;
    nova_signature_status_t signature;
} nova_resource_descriptor_t;

typedef struct {
    uint64_t id,fallback_id;
    char name[NOVA_RESOURCE_NAME_CAPACITY];
    nova_resource_type_t type;
    nova_resource_state_t state;
    nova_resource_origin_t origin;
    nova_resource_priority_t priority;
    nova_cache_policy_t cache_policy;
    uint32_t version,checksum,reference_count,access_count;
    uint64_t last_use,size;
    const void *data;
    const void *packed_data;
    uint64_t packed_size;
    uint32_t packed_checksum;
    nova_compression_type_t compression;
    const uint8_t *packed_sha256,*original_sha256;
    nova_signature_status_t signature;
    nova_resource_trust_t trust;
    bool integrity_verified;
    uint8_t decoded_block,decoded_block_count;
    uint64_t dependencies[NOVA_RESOURCE_DEPENDENCY_CAPACITY];
    uint8_t dependency_count;
} nova_resource_t;

typedef struct {
    uint32_t registered,loaded,unloaded,cache_hits,cache_misses,integrity_errors;
    uint32_t fallback_uses,releases,preloads,evictions,lookup_hits,lookup_misses;
    uint32_t dependency_loads,dependency_failures,cycles,duplicate_requests;
    uint32_t invalid_requests,unsupported_requests,shutdowns,max_dependency_depth;
    uint32_t decompressions,decompression_errors;
    uint32_t collections,lru_evictions,lfu_evictions,permanent_skips;
    uint32_t current_references,peak_references;
    uint64_t cached_bytes,cache_budget,peak_cached_bytes,registered_bytes;
    uint64_t cache_minimum,cache_reserved,cache_critical;
    uint64_t compressed_bytes,decoded_bytes;
    bool initialized,busy;
} nova_resource_diagnostics_t;
typedef enum {NOVA_RESOURCE_EVENT_REQUEST,NOVA_RESOURCE_EVENT_LOAD_BEGIN,
    NOVA_RESOURCE_EVENT_VALIDATE,NOVA_RESOURCE_EVENT_DECOMPRESS,
    NOVA_RESOURCE_EVENT_CACHE_HIT,NOVA_RESOURCE_EVENT_CACHE_MISS,
    NOVA_RESOURCE_EVENT_LOAD_COMPLETE,NOVA_RESOURCE_EVENT_RELEASE,
    NOVA_RESOURCE_EVENT_ERROR} nova_resource_event_type_t;
typedef enum {NOVA_RESOURCE_EXPORT_NDF,NOVA_RESOURCE_EXPORT_JSON,
    NOVA_RESOURCE_EXPORT_CSV,NOVA_RESOURCE_EXPORT_BINARY} nova_resource_export_format_t;
typedef struct {
    uint64_t sequence,timestamp_us,resource_id,parent_id;
    nova_resource_event_type_t event;
    nova_resource_type_t resource_type;
    nova_resource_result_t result;
    nova_resource_state_t state;
    nova_compression_type_t compression;
    nova_resource_trust_t integrity;
    uint32_t version,boot_phase,module_id,error_code;
    uint64_t packed_size,original_size,request_time_us,load_begin_us,load_end_us,
             validation_time_us,decompression_time_us,cache_time_us,total_time_us,
             cache_bytes,peak_cache_bytes,temporary_bytes;
    bool cache_hit,integrity_ok,checksum_ok,signature_ok,version_ok;
} nova_resource_trace_t;
typedef struct {
    nova_resource_type_t type;nova_resource_result_t result;
    nova_resource_trust_t integrity;uint64_t start_us,end_us;
    uint32_t boot_phase,module_id;
    bool use_type,use_result,use_integrity,use_time,use_boot_phase,use_module,
         cache_hits_only,errors_only;
} nova_resource_diag_filter_t;
typedef struct {
    uint32_t recorded,overwritten,requests,loads,validations,decompressions,
             cache_hits,cache_misses,releases,errors,exports,denied_exports;
    uint16_t count;uint64_t total_load_us,fastest_load_us,slowest_load_us,
        cache_bytes,peak_cache_bytes;
    bool initialized,passive,read_only,boot_unaffected;
} nova_resource_diag_status_t;
typedef enum {NOVA_CORRUPTION_NONE,NOVA_CORRUPTION_BIT_ERROR,
    NOVA_CORRUPTION_TRUNCATED,NOVA_CORRUPTION_EMPTY,NOVA_CORRUPTION_INVALID_HEADER,
    NOVA_CORRUPTION_INVALID_SIGNATURE,NOVA_CORRUPTION_INVALID_CHECKSUM,
    NOVA_CORRUPTION_INVALID_VERSION,NOVA_CORRUPTION_INVALID_METADATA,
    NOVA_CORRUPTION_TYPE_COUNT} nova_corruption_type_t;
typedef enum {NOVA_CORRUPTION_WARNING,NOVA_CORRUPTION_RECOVERABLE,
    NOVA_CORRUPTION_CRITICAL,NOVA_CORRUPTION_FATAL} nova_corruption_class_t;
typedef struct {
    uint64_t resource_id,fallback_id;nova_resource_type_t resource_type;
    nova_corruption_type_t corruption;nova_corruption_class_t classification;
    nova_resource_result_t error_code;uint64_t source_size,test_size,memory_before,memory_after;
    bool integrity_detected,fallback_successful,boot_continued,source_unchanged,
         pool_consistent,manipulated_used;
} nova_resource_corruption_result_t;
typedef struct {uint16_t count;uint32_t executed,detected,fallbacks,continued,
    warnings,recoverable,critical,fatal,reports;bool initialized,isolated,
    deterministic,heap_free,productive_data_unchanged;}
    nova_resource_corruption_status_t;

void nova_resource_manager_initialize(void);
bool nova_resource_loader_initialize(uint64_t cache_budget);
void nova_resource_manager_shutdown(void);
uint64_t nova_resource_id(const char *uri);
bool nova_resource_register(const char *uri,nova_resource_type_t type,
    uint32_t version,const void *data,uint64_t size,uint32_t checksum,
    uint64_t fallback_id);
nova_resource_result_t nova_resource_register_descriptor(
    const nova_resource_descriptor_t *descriptor,nova_resource_t **resource);
bool nova_resource_add_dependency(uint64_t resource_id,uint64_t dependency_id);
const nova_resource_t *nova_resource_find(uint64_t id);
const nova_resource_t *nova_resource_load(uint64_t id);
nova_resource_result_t nova_resource_load_mode(uint64_t id,
    nova_resource_load_mode_t mode,const nova_resource_t **resource);
bool nova_resource_loaded(uint64_t id);
bool nova_resource_preload(uint64_t id);
uint32_t nova_resource_preload_priority(nova_resource_priority_t maximum_priority);
bool nova_resource_release(uint64_t id);
nova_resource_result_t nova_resource_unload(uint64_t id);
bool nova_resource_cache_configure(uint64_t minimum,uint64_t maximum,
    uint64_t reserved,uint64_t critical);
bool nova_resource_cache_set_policy(uint64_t id,nova_cache_policy_t policy);
uint32_t nova_resource_cache_collect(void);
uint32_t nova_resource_checksum(const void *data,uint64_t size);
const nova_resource_diagnostics_t *nova_resource_diagnostics(void);
bool nova_resource_diag_initialize(void);
bool nova_resource_diag_record(const nova_resource_trace_t *trace);
const nova_resource_trace_t *nova_resource_diag_get(uint32_t chronological_index);
const nova_resource_trace_t *nova_resource_diag_query(
    const nova_resource_diag_filter_t *filter,uint32_t matching_index);
bool nova_resource_diag_export(nova_resource_export_format_t format,
    bool user_authorized,uint8_t *output,uint32_t capacity,uint32_t *written);
void nova_resource_diag_reset(void);
const nova_resource_diag_status_t *nova_resource_diag_status(void);
bool nova_resource_corruption_initialize(void);
bool nova_resource_corruption_execute(uint64_t resource_id,nova_corruption_type_t corruption);
const nova_resource_corruption_result_t *nova_resource_corruption_results(void);
const nova_resource_corruption_status_t *nova_resource_corruption_status(void);
bool nova_resource_corruption_generate_report(bool user_authorized,
    uint8_t *output,uint32_t capacity,uint32_t *written);

#endif
