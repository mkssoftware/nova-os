/* NovaOS C-Kernel - Kernel Component Manager (Port aus memory32.inc). */
#include "component.h"
#include "object.h"
#include <stddef.h>

#define COMPONENT_STATE_EMPTY      0u
#define COMPONENT_STATE_REGISTERED 1u
#define COMPONENT_STATE_ACTIVE     2u

typedef struct {
    uint32_t id;
    uint32_t version;
    uint32_t flags;
    uint32_t state;
    uint32_t handle;
} component_record_t;

static component_record_t component_table[COMPONENT_CAPACITY];
static uint32_t component_count_value = 0;

void component_manager_initialize(void) {
    for (uint32_t i = 0; i < COMPONENT_CAPACITY; ++i) {
        component_table[i] = (component_record_t){0};
    }
    component_count_value = 0;
}

uint32_t component_count(void) {
    return component_count_value;
}

void *component_lookup(uint32_t id) {
    for (uint32_t i = 0; i < COMPONENT_CAPACITY; ++i) {
        component_record_t *rec = &component_table[i];
        if (rec->state == COMPONENT_STATE_EMPTY) {
            continue;
        }
        if (rec->id == id) {
            return rec;
        }
    }
    return 0;
}

uint32_t component_register(uint32_t id, uint32_t version, uint32_t flags) {
    if (id == 0) {
        return 0;
    }

    uint32_t slot;
    for (slot = 0; slot < COMPONENT_CAPACITY; ++slot) {
        component_record_t *rec = &component_table[slot];
        if (rec->state == COMPONENT_STATE_EMPTY) {
            break;
        }
        if (rec->id == id) {
            return 0; /* bereits registriert */
        }
    }
    if (slot >= COMPONENT_CAPACITY) {
        return 0;
    }

    uint32_t handle = object_create(OBJECT_TYPE_COMPONENT, id, version);
    if (handle == 0) {
        return 0;
    }

    component_record_t *rec = &component_table[slot];
    rec->id = id;
    rec->version = version;
    rec->flags = flags;
    rec->state = COMPONENT_STATE_REGISTERED;
    rec->handle = handle;
    ++component_count_value;
    return handle;
}

int component_activate(uint32_t id) {
    component_record_t *rec = (component_record_t *)component_lookup(id);
    if (!rec) {
        return -1;
    }
    if (rec->state != COMPONENT_STATE_REGISTERED) {
        return -1;
    }
    rec->state = COMPONENT_STATE_ACTIVE;
    return 0;
}

int component_is_active(uint32_t id) {
    component_record_t *rec = (component_record_t *)component_lookup(id);
    if (!rec) return -1;
    return (rec->state == COMPONENT_STATE_ACTIVE) ? 0 : -1;
}

int component_manager_self_test(void) {
    uint32_t handle = component_register(0x434F5245u /* "CORE" */, 0x00010000u, 1);
    if (handle == 0) return -1;

    if (component_activate(0x434F5245u) != 0) return -1;

    component_record_t *rec = (component_record_t *)component_lookup(0x434F5245u);
    if (!rec) return -1;
    if (rec->version != 0x00010000u) return -1;
    if (rec->state != COMPONENT_STATE_ACTIVE) return -1;
    if (rec->handle != handle) return -1;
    if (component_count_value != 1) return -1;

    return 0;
}
