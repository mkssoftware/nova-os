#include "resources.h"

static nova_resource_t registry[NOVA_RESOURCE_CAPACITY];
static bool used[NOVA_RESOURCE_CAPACITY];
static nova_resource_diagnostics_t diagnostics;
static uint32_t use_clock;

void nova_resource_manager_initialize(void)
{
    for (uint32_t i = 0; i < NOVA_RESOURCE_CAPACITY; ++i) used[i] = false;
    diagnostics = (nova_resource_diagnostics_t){0}; use_clock = 0;
}

uint64_t nova_resource_id(const char *uri)
{
    if (!uri || !*uri) return 0;
    uint64_t hash = 1469598103934665603ull;
    while (*uri) { hash ^= (uint8_t)*uri++; hash *= 1099511628211ull; }
    return hash ? hash : 1;
}

uint32_t nova_resource_checksum(const void *data, uint64_t size)
{
    if (!data || !size) return 0;
    const uint8_t *bytes = data;
    uint32_t crc = 0xffffffffu;
    for (uint64_t i = 0; i < size; ++i) {
        crc ^= bytes[i];
        for (uint32_t bit = 0; bit < 8; ++bit)
            crc = (crc >> 1) ^ (0xedb88320u & (0u - (crc & 1u)));
    }
    return ~crc;
}

const nova_resource_t *nova_resource_find(uint64_t id)
{
    if (!id) return 0;
    for (uint32_t i = 0; i < NOVA_RESOURCE_CAPACITY; ++i)
        if (used[i] && registry[i].id == id) return &registry[i];
    return 0;
}

bool nova_resource_register(const char *uri, nova_resource_type_t type,
                            uint32_t version, const void *data, uint64_t size,
                            uint32_t checksum, uint64_t fallback_id)
{
    uint64_t id = nova_resource_id(uri);
    if (!id || type > NOVA_RESOURCE_ICON || !version || !data || !size ||
        nova_resource_find(id)) return false;
    uint32_t actual = nova_resource_checksum(data, size);
    if (checksum && checksum != actual) { ++diagnostics.integrity_errors; return false; }
    for (uint32_t i = 0; i < NOVA_RESOURCE_CAPACITY; ++i) if (!used[i]) {
        used[i] = true;
        registry[i] = (nova_resource_t){ id, fallback_id, type,
            NOVA_RESOURCE_REGISTERED, version, actual, 0, 0, size, data };
        ++diagnostics.registered; return true;
    }
    return false;
}

const nova_resource_t *nova_resource_load(uint64_t id)
{
    nova_resource_t *resource = 0;
    for (uint32_t i = 0; i < NOVA_RESOURCE_CAPACITY; ++i)
        if (used[i] && registry[i].id == id) { resource = &registry[i]; break; }
    if (!resource || resource->state == NOVA_RESOURCE_INVALID) {
        ++diagnostics.cache_misses;
        if (resource && resource->fallback_id) {
            ++diagnostics.fallback_uses; return nova_resource_load(resource->fallback_id);
        }
        return 0;
    }
    if (nova_resource_checksum(resource->data, resource->size) != resource->checksum) {
        resource->state = NOVA_RESOURCE_INVALID; ++diagnostics.integrity_errors;
        if (resource->fallback_id) { ++diagnostics.fallback_uses;
            return nova_resource_load(resource->fallback_id); }
        return 0;
    }
    if (resource->reference_count) ++diagnostics.cache_hits;
    else { ++diagnostics.cache_misses; ++diagnostics.loaded;
        diagnostics.cached_bytes += resource->size; }
    ++resource->reference_count; resource->last_use = ++use_clock;
    resource->state = NOVA_RESOURCE_IN_USE;
    return resource;
}

bool nova_resource_release(uint64_t id)
{
    for (uint32_t i = 0; i < NOVA_RESOURCE_CAPACITY; ++i) if (used[i] && registry[i].id == id) {
        if (!registry[i].reference_count) return false;
        --registry[i].reference_count; ++diagnostics.releases;
        registry[i].state = registry[i].reference_count ? NOVA_RESOURCE_CACHED : NOVA_RESOURCE_RELEASED;
        return true;
    }
    return false;
}

const nova_resource_diagnostics_t *nova_resource_diagnostics(void) { return &diagnostics; }
