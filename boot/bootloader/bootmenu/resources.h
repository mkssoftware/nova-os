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

#endif
