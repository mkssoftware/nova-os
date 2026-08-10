#ifndef NOVA_BOOT_ARCHITECTURE_H
#define NOVA_BOOT_ARCHITECTURE_H

#include <stdbool.h>
#include <stdint.h>

#define NOVA_ARCHITECTURE_ABI_MAJOR 1u
#define NOVA_ARCHITECTURE_ABI_MINOR 0u

typedef enum {
    NOVA_ARCH_PLATFORM,
    NOVA_ARCH_RESOURCE,
    NOVA_ARCH_GRAPHICS,
    NOVA_ARCH_RENDERER,
    NOVA_ARCH_SCENE,
    NOVA_ARCH_LAYOUT,
    NOVA_ARCH_MOTION,
    NOVA_ARCH_CONTROL,
    NOVA_ARCH_NAVIGATION,
    NOVA_ARCH_DIALOG,
    NOVA_ARCH_APPLICATION,
    NOVA_ARCH_DIAGNOSTICS,
    NOVA_ARCH_SUBSYSTEM_COUNT
} nova_architecture_subsystem_t;

typedef struct {
    nova_architecture_subsystem_t id;
    const char *name;
    uint16_t abi_major,abi_minor;
    uint32_t dependencies;
    bool platform_neutral,required;
} nova_architecture_descriptor_t;

typedef struct {
    uint32_t validations,invalid_descriptors,missing_dependencies;
    uint32_t registered_mask,ready_mask;
    bool valid,complete;
} nova_architecture_diagnostics_t;

void nova_architecture_initialize(void);
bool nova_architecture_register(nova_architecture_subsystem_t subsystem);
bool nova_architecture_ready(nova_architecture_subsystem_t subsystem);
bool nova_architecture_validate(void);
const nova_architecture_descriptor_t *nova_architecture_descriptor(
    nova_architecture_subsystem_t subsystem);
const nova_architecture_diagnostics_t *nova_architecture_diagnostics(void);

#endif
