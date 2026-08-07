#ifndef NOVA_BOOT_RESOURCES_H
#define NOVA_BOOT_RESOURCES_H

#include <stdint.h>
#include <stdbool.h>

#define NOVA_RESOURCE_CAPACITY 128u

typedef enum {
    NOVA_RESOURCE_IMAGE, NOVA_RESOURCE_FONT, NOVA_RESOURCE_THEME,
    NOVA_RESOURCE_ANIMATION, NOVA_RESOURCE_LAYOUT, NOVA_RESOURCE_LOCALIZATION,
    NOVA_RESOURCE_CONFIGURATION, NOVA_RESOURCE_ICON
} nova_resource_type_t;
typedef enum {
    NOVA_RESOURCE_REGISTERED, NOVA_RESOURCE_UNLOADED, NOVA_RESOURCE_LOADED,
    NOVA_RESOURCE_VERIFIED, NOVA_RESOURCE_CACHED, NOVA_RESOURCE_IN_USE,
    NOVA_RESOURCE_RELEASED, NOVA_RESOURCE_INVALID
} nova_resource_state_t;
typedef struct {
    uint64_t id, fallback_id;
    nova_resource_type_t type;
    nova_resource_state_t state;
    uint32_t version, checksum, reference_count, last_use;
    uint64_t size;
    const void *data;
} nova_resource_t;
typedef struct {
    uint32_t registered, loaded, cache_hits, cache_misses, integrity_errors,
             fallback_uses, releases;
    uint64_t cached_bytes;
} nova_resource_diagnostics_t;

void nova_resource_manager_initialize(void);
uint64_t nova_resource_id(const char *uri);
bool nova_resource_register(const char *uri, nova_resource_type_t type,
                            uint32_t version, const void *data, uint64_t size,
                            uint32_t checksum, uint64_t fallback_id);
const nova_resource_t *nova_resource_find(uint64_t id);
const nova_resource_t *nova_resource_load(uint64_t id);
bool nova_resource_release(uint64_t id);
uint32_t nova_resource_checksum(const void *data, uint64_t size);
const nova_resource_diagnostics_t *nova_resource_diagnostics(void);

#endif
