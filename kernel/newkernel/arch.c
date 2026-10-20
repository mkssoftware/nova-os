/* NovaOS C-Kernel - Architecture-Model Introspection-Registry (Port aus arch32.inc). */
#include "arch.h"
#include <stddef.h>

#define ARCH_CONTRACT_BEST_EFFORT 2u

#define ARCH_NOVA_PROPERTIES (ARCH_PROP_DETERMINISTIC | ARCH_PROP_ZEROCOPY | \
    ARCH_PROP_TRANSACTIONAL | ARCH_PROP_STRUCTURED_CC | \
    ARCH_PROP_INTROSPECTABLE | ARCH_PROP_RESOURCE_ECO)

typedef struct {
    uint32_t module_id;
    uint32_t version;
    uint32_t properties;
    uint32_t contract;
    uint32_t entry_point;
    uint32_t init_time;
    uint32_t self_test_result;
} arch_intro_record_t;

static arch_intro_record_t arch_intro_table[ARCH_INTRO_CAPACITY];
static uint32_t arch_module_count = 0;
static uint32_t arch_system_properties = 0;
static int arch_ready = 0;

/* TODO: durch timer_ticks ersetzen, sobald das Time-Subsystem portiert ist. */
static uint32_t current_timer_ticks(void) {
    return 0;
}

void arch_initialize(void) {
    for (uint32_t i = 0; i < ARCH_INTRO_CAPACITY; ++i) {
        arch_intro_table[i] = (arch_intro_record_t){0};
    }
    arch_module_count = 0;
    arch_system_properties = ARCH_NOVA_PROPERTIES;
    arch_ready = 1;
}

int arch_register_module(uint32_t module_id, uint32_t version, uint32_t properties,
                          uint32_t entry_point) {
    if (arch_module_count >= ARCH_INTRO_CAPACITY) {
        return -1;
    }
    arch_intro_record_t *rec = &arch_intro_table[arch_module_count];
    rec->module_id = module_id;
    rec->version = version;
    rec->properties = properties;
    rec->contract = ARCH_CONTRACT_BEST_EFFORT;
    rec->entry_point = entry_point;
    rec->init_time = current_timer_ticks();
    rec->self_test_result = 0;

    arch_system_properties |= properties;
    ++arch_module_count;
    return 0;
}

void *arch_introspect(uint32_t module_id) {
    for (uint32_t i = 0; i < arch_module_count; ++i) {
        if (arch_intro_table[i].module_id == module_id) {
            return &arch_intro_table[i];
        }
    }
    return 0;
}

int arch_self_test(void) {
    if (arch_ready != 1) return -1;

    if (arch_register_module(0x0001u, 0x00010000u,
                              ARCH_PROP_DETERMINISTIC | ARCH_PROP_INTROSPECTABLE,
                              0x100000u) != 0) {
        return -1;
    }
    if (arch_introspect(0x0001u) == 0) return -1;

    return 0;
}
