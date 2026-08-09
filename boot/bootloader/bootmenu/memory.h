#ifndef NOVA_BOOT_MEMORY_H
#define NOVA_BOOT_MEMORY_H

#include <stdbool.h>
#include <stdint.h>

#define NOVA_MEMORY_OBJECT_CAPACITY 256u
#define NOVA_MEMORY_TOTAL_BUDGET (64u*1024u*1024u)

typedef enum {
    NOVA_MEMORY_PERMANENT,
    NOVA_MEMORY_RUNTIME,
    NOVA_MEMORY_SCENE,
    NOVA_MEMORY_ANIMATION,
    NOVA_MEMORY_FRAME,
    NOVA_MEMORY_RENDER,
    NOVA_MEMORY_CACHE,
    NOVA_MEMORY_DIAGNOSTICS,
    NOVA_MEMORY_POOL_COUNT
} nova_memory_pool_id_t;

typedef enum {
    NOVA_MEMORY_OBJECT_FREE,
    NOVA_MEMORY_OBJECT_ACTIVE,
    NOVA_MEMORY_OBJECT_RELEASED,
    NOVA_MEMORY_OBJECT_STATIC
} nova_memory_object_state_t;

typedef struct {
    uint32_t id;
    void *address;
    uint32_t size;
    uint32_t owner;
    uint16_t references;
    uint8_t alignment;
    nova_memory_pool_id_t pool;
    nova_memory_object_state_t state;
} nova_memory_object_t;

typedef struct {
    uint32_t budget;
    uint32_t arena_capacity;
    uint32_t used;
    uint32_t peak;
    uint32_t allocations;
    uint32_t releases;
    uint32_t active_objects;
    uint32_t fragmentation;
    uint32_t overflows;
} nova_memory_pool_statistics_t;

typedef struct {
    uint32_t total_budget;
    uint32_t total_used;
    uint32_t total_peak;
    uint32_t allocations;
    uint32_t releases;
    uint32_t frame_resets;
    uint32_t invalid_pointers;
    uint32_t double_frees;
    uint32_t alignment_errors;
    uint32_t metadata_overflows;
    uint32_t recovery_requests;
} nova_memory_statistics_t;

void nova_memory_initialize(void);
void *nova_memory_allocate(nova_memory_pool_id_t pool,uint32_t size,
    uint32_t owner,uint8_t alignment);
bool nova_memory_track_static(nova_memory_pool_id_t pool,void *address,
    uint32_t size,uint32_t owner,uint8_t alignment);
bool nova_memory_retain(void *address);
bool nova_memory_release(void *address);
bool nova_memory_reset_frame(void);
bool nova_memory_validate_pointer(const void *address,
    nova_memory_pool_id_t expected_pool,uint32_t minimum_size);
const nova_memory_object_t *nova_memory_object(const void *address);
const nova_memory_pool_statistics_t *nova_memory_pool_statistics(
    nova_memory_pool_id_t pool);
const nova_memory_statistics_t *nova_memory_statistics(void);

#endif
