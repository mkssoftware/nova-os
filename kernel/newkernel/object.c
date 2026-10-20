/* NovaOS C-Kernel - Kernel Object Manager (Port aus memory32.inc). */
#include "object.h"
#include "heap.h"
#include <stddef.h>

#define OBJECT_HEADER_SIZE 32u

typedef struct {
    uint32_t struct_size;
    uint32_t abi;
    uint32_t type;
    uint32_t state;
    uint32_t refcount;
    uint32_t handle;
    uint32_t payload0;
    uint32_t payload1;
} object_header_t; /* 32 Byte = OBJECT_HEADER_SIZE */

static object_header_t *object_table[OBJECT_TABLE_CAPACITY];
static uint32_t object_generations[OBJECT_TABLE_CAPACITY];
static uint32_t object_semantic_types[OBJECT_TABLE_CAPACITY];
static uint32_t object_semantic_versions[OBJECT_TABLE_CAPACITY];
static uint32_t object_semantic_validation[OBJECT_TABLE_CAPACITY];
static uint32_t object_live_count_value = 0;

void object_manager_initialize(void) {
    for (uint32_t i = 0; i < OBJECT_TABLE_CAPACITY; ++i) {
        object_table[i] = 0;
        object_generations[i] = 1;
        object_semantic_types[i] = 0;
        object_semantic_versions[i] = 0;
        object_semantic_validation[i] = 0;
    }
    object_live_count_value = 0;
}

uint32_t object_live_count(void) {
    return object_live_count_value;
}

uint32_t object_create(uint32_t type, uint32_t payload0, uint32_t payload1) {
    if (type == 0) {
        return 0;
    }
    uint32_t slot;
    for (slot = 0; slot < OBJECT_TABLE_CAPACITY; ++slot) {
        if (object_table[slot] == 0) {
            break;
        }
    }
    if (slot >= OBJECT_TABLE_CAPACITY) {
        return 0;
    }

    object_header_t *obj = (object_header_t *)heap_allocate(OBJECT_HEADER_SIZE);
    if (!obj) {
        return 0;
    }

    obj->struct_size = OBJECT_HEADER_SIZE;
    obj->abi = 1;
    obj->type = type;
    obj->state = OBJECT_STATE_LIVE;
    obj->refcount = 1;
    obj->payload0 = payload0;
    obj->payload1 = payload1;

    uint32_t handle = (object_generations[slot] << 16) | (slot + 1u);
    obj->handle = handle;
    object_table[slot] = obj;
    ++object_live_count_value;
    return handle;
}

void *object_lookup(uint32_t handle) {
    uint32_t slot_plus_one = handle & 0xFFFFu;
    if (slot_plus_one == 0) {
        return 0;
    }
    uint32_t slot = slot_plus_one - 1u;
    if (slot >= OBJECT_TABLE_CAPACITY) {
        return 0;
    }
    uint32_t generation = handle >> 16;
    if (generation != object_generations[slot]) {
        return 0;
    }
    object_header_t *obj = object_table[slot];
    if (!obj) {
        return 0;
    }
    if (obj->state != OBJECT_STATE_LIVE) {
        return 0;
    }
    return obj;
}

int object_retain(uint32_t handle) {
    object_header_t *obj = (object_header_t *)object_lookup(handle);
    if (!obj) {
        return -1;
    }
    ++obj->refcount;
    return 0;
}

int object_release(uint32_t handle) {
    object_header_t *obj = (object_header_t *)object_lookup(handle);
    if (!obj) {
        return -1;
    }
    if (obj->refcount == 0) {
        return -1;
    }
    --obj->refcount;
    if (obj->refcount != 0) {
        return 0;
    }

    obj->state = 0;
    uint32_t slot = (handle & 0xFFFFu) - 1u;
    object_table[slot] = 0;
    object_semantic_types[slot] = 0;
    object_semantic_versions[slot] = 0;
    object_semantic_validation[slot] = 0;
    ++object_generations[slot];
    object_generations[slot] &= 0xFFFFu;
    if (object_generations[slot] == 0) {
        object_generations[slot] = 1;
    }
    --object_live_count_value;
    return 0;
}

int object_manager_self_test(void) {
    uint32_t handle = object_create(1, 0x4E4F5641u, 0x4F424A31u);
    if (handle == 0) return -1;

    object_header_t *obj = (object_header_t *)object_lookup(handle);
    if (!obj) return -1;
    if (obj->type != 1) return -1;
    if (obj->payload0 != 0x4E4F5641u) return -1;

    if (object_retain(handle) != 0) return -1;
    obj = (object_header_t *)object_lookup(handle);
    if (obj->refcount != 2) return -1;

    if (object_release(handle) != 0) return -1;
    if (object_release(handle) != 0) return -1;

    if (object_lookup(handle) != 0) return -1; /* muss jetzt ungueltig sein */
    if (object_live_count_value != 0) return -1;

    return 0;
}
