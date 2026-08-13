#ifndef NOVA_BOOT_RESOURCE_VERSION_H
#define NOVA_BOOT_RESOURCE_VERSION_H

#include <stdbool.h>
#include <stdint.h>

#define NOVA_RESOURCE_VERSION_CAPACITY 32u
#define NOVA_RESOURCE_VERSION_HASH_CAPACITY 64u
#define NOVA_RESOURCE_VERSION_DEPENDENCY_CAPACITY 8u

typedef struct {
    uint16_t major,minor,patch,build;
} nova_resource_version_t;

typedef struct {
    uint64_t resource_id;
    nova_resource_version_t minimum;
} nova_resource_version_dependency_t;

typedef struct {
    uint64_t logical_id,backing_resource_id;
    nova_resource_version_t version,minimum,maximum;
    const nova_resource_version_dependency_t *dependencies;
    uint8_t dependency_count;
} nova_resource_version_descriptor_t;

typedef struct {
    uint64_t logical_id,backing_resource_id;
    nova_resource_version_t version,minimum,maximum;
    nova_resource_version_dependency_t dependencies[
        NOVA_RESOURCE_VERSION_DEPENDENCY_CAPACITY];
    uint8_t dependency_count;
    bool active,valid;
} nova_resource_version_entry_t;

typedef struct {
    uint32_t registered,lookups,compatibility_checks,compatible,incompatible;
    uint32_t selections,conflicts,rollbacks,dependency_checks,dependency_failures;
    uint32_t duplicates,invalid_entries,cycles;
    bool initialized;
} nova_resource_version_diagnostics_t;

bool nova_resource_version_initialize(void);
bool nova_resource_version_valid(nova_resource_version_t version);
int8_t nova_resource_version_compare(nova_resource_version_t left,
                                     nova_resource_version_t right);
bool nova_resource_version_compatible(const nova_resource_version_t *required,
                                      const nova_resource_version_t *available);
bool nova_resource_version_register(const nova_resource_version_descriptor_t *descriptor);
const nova_resource_version_entry_t *nova_resource_version(uint64_t resource_id);
const nova_resource_version_entry_t *nova_resource_version_select(
    uint64_t logical_id,const nova_resource_version_t *minimum,
    const nova_resource_version_t *maximum);
const nova_resource_version_entry_t *nova_resource_version_rollback(uint64_t logical_id);
bool nova_resource_version_dependencies(const nova_resource_version_entry_t *entry);
const nova_resource_version_diagnostics_t *nova_resource_version_diagnostics(void);

#endif
