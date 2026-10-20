/* NovaOS C-Kernel - Nova Object-Subsystem (Port aus object32.inc). */
#include "nobj.h"

#define FSPERM_0644 0644u

typedef struct {
    uint32_t id;
    uint32_t type;
    uint32_t flags;
    uint32_t refcount;
    uint32_t version;
    uint32_t perm_mask;
    uint32_t parent_id;
    uint32_t prov_hash;
    uint32_t data_ptr;
    uint32_t data_len;
    uint32_t pipeline_next;
} nova_obj_t;

static nova_obj_t obj_table[OBJ_CAPACITY];
static uint32_t obj_count = 0;
static uint32_t obj_next_id = 1;
static int obj_ready = 0;

/* TODO: durch timer_ticks ersetzen, sobald das Time-Subsystem portiert ist. */
static uint32_t current_timer_ticks(void) {
    return 0;
}

void object_initialize(void) {
    for (uint32_t i = 0; i < OBJ_CAPACITY; ++i) {
        obj_table[i] = (nova_obj_t){0};
    }
    obj_count = 0;
    obj_next_id = 1;
    obj_ready = 1;
}

int obj_create(uint32_t type, uint32_t flags, uint32_t parent_id, uint32_t *out_obj_id) {
    if (obj_count >= OBJ_CAPACITY) {
        return -1;
    }
    nova_obj_t *obj = &obj_table[obj_count];
    obj->id = obj_next_id;
    obj->type = type;
    obj->flags = flags;
    obj->refcount = 1;
    obj->version = 1;
    obj->perm_mask = FSPERM_0644;
    obj->parent_id = parent_id;
    obj->prov_hash = current_timer_ticks() ^ obj_next_id; /* XOR(id, ticks) */
    obj->data_ptr = 0;
    obj->data_len = 0;
    obj->pipeline_next = 0xFFFFFFFFu;

    if (out_obj_id) {
        *out_obj_id = obj_next_id;
    }
    ++obj_next_id;
    ++obj_count;
    return 0;
}

static nova_obj_t *nobj_find(uint32_t obj_id) {
    for (uint32_t i = 0; i < obj_count; ++i) {
        if (obj_table[i].id == obj_id) {
            return &obj_table[i];
        }
    }
    return 0;
}

int obj_addref(uint32_t obj_id) {
    nova_obj_t *obj = nobj_find(obj_id);
    if (!obj) {
        return -1;
    }
    ++obj->refcount;
    return 0;
}

int obj_release(uint32_t obj_id) {
    nova_obj_t *obj = nobj_find(obj_id);
    if (!obj) {
        return -1;
    }
    if (obj->refcount == 0) {
        return -1;
    }
    --obj->refcount;
    return 0;
}

int obj_version_bump(uint32_t obj_id) {
    nova_obj_t *obj = nobj_find(obj_id);
    if (!obj) {
        return -1;
    }
    ++obj->version;
    return 0;
}

int object_self_test(void) {
    if (obj_ready != 1) return -1;

    uint32_t obj_id = 0;
    if (obj_create(OBJ_TYPE_DATA, OBJ_FLAG_VERSIONED, 0xFFFFFFFFu, &obj_id) != 0) return -1;
    if (obj_addref(obj_id) != 0) return -1;
    if (obj_release(obj_id) != 0) return -1;
    if (obj_version_bump(obj_id) != 0) return -1;

    return 0;
}
