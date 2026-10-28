/* NovaOS C-Kernel - §102 Unified Object API (Port aus security32.inc). */
#include "uobj.h"
#include "kog.h"
#include "evbus.h"

typedef struct {
    uint32_t type_id;
    uint32_t abi_version;
    uint32_t flags;
    uobj_init_fn_t init_fn;
    uobj_destroy_fn_t destroy_fn;
    int present;
} uobj_type_t;

typedef struct {
    uint32_t obj_id;
    uint32_t type_id;
    uint32_t generation;
    uint8_t state;
    int present;
    uint16_t flags;
    uint32_t refcount;
    uint32_t kog_node;
} uobj_object_t;

typedef struct {
    uint32_t handle_id;
    uint32_t obj_id;
    uint32_t generation;
    uint16_t caps;
    int present;
} uobj_handle_t;

static uobj_type_t uobj_types[UOBJ_MAX_TYPES];
static uobj_object_t uobj_objects[UOBJ_MAX_OBJECTS];
static uobj_handle_t uobj_handles[UOBJ_MAX_HANDLES];
static uint32_t uobj_next_id = 0;
static uint32_t uobj_next_gen = 0;
static uint32_t uobj_next_hdl = 0;
static uint32_t uobj_stat_created = 0;
static uint32_t uobj_stat_destroyed = 0;
static uint32_t uobj_stat_failed = 0;

static uobj_type_t *uobj_find_type(uint32_t type_id) {
    for (uint32_t i = 0; i < UOBJ_MAX_TYPES; ++i) {
        if (uobj_types[i].present && uobj_types[i].type_id == type_id) return &uobj_types[i];
    }
    return 0;
}

static uobj_object_t *uobj_find_object(uint32_t obj_id) {
    for (uint32_t i = 0; i < UOBJ_MAX_OBJECTS; ++i) {
        if (uobj_objects[i].present && uobj_objects[i].obj_id == obj_id) return &uobj_objects[i];
    }
    return 0;
}

static uobj_handle_t *uobj_find_handle(uint32_t handle_id) {
    for (uint32_t i = 0; i < UOBJ_MAX_HANDLES; ++i) {
        if (uobj_handles[i].present && uobj_handles[i].handle_id == handle_id) return &uobj_handles[i];
    }
    return 0;
}

int uobj_register_type(uint32_t type_id, uint32_t abi_version, uint32_t flags,
                        uobj_init_fn_t init_fn, uobj_destroy_fn_t destroy_fn) {
    if (uobj_find_type(type_id)) return -1; /* bereits vorhanden */

    for (uint32_t i = 0; i < UOBJ_MAX_TYPES; ++i) {
        if (!uobj_types[i].present) {
            uobj_types[i].type_id = type_id;
            uobj_types[i].abi_version = abi_version;
            uobj_types[i].flags = flags;
            uobj_types[i].init_fn = init_fn;
            uobj_types[i].destroy_fn = destroy_fn;
            uobj_types[i].present = 1;
            return 0;
        }
    }
    return -1;
}

static uobj_object_t *uobj_alloc_object(uint32_t *out_obj_id) {
    for (uint32_t i = 0; i < UOBJ_MAX_OBJECTS; ++i) {
        if (!uobj_objects[i].present) {
            uint32_t id = ++uobj_next_id;
            uobj_objects[i].obj_id = id;
            uobj_objects[i].present = 1;
            *out_obj_id = id;
            return &uobj_objects[i];
        }
    }
    return 0;
}

static int uobj_alloc_handle(uint32_t obj_id, uint32_t generation, uint32_t caps, uint32_t *out_handle_id) {
    for (uint32_t i = 0; i < UOBJ_MAX_HANDLES; ++i) {
        if (!uobj_handles[i].present) {
            uint32_t id = ++uobj_next_hdl;
            uobj_handles[i].handle_id = id;
            uobj_handles[i].obj_id = obj_id;
            uobj_handles[i].generation = generation;
            uobj_handles[i].caps = (uint16_t)caps;
            uobj_handles[i].present = 1;
            *out_handle_id = id;
            return 0;
        }
    }
    return -1;
}

int uobj_create(uint32_t type_id, uint32_t capabilities, uint32_t *out_handle_id, uint32_t *out_obj_id) {
    uobj_type_t *type = uobj_find_type(type_id);
    if (!type) { ++uobj_stat_failed; return -1; }

    uint32_t obj_id;
    uobj_object_t *obj = uobj_alloc_object(&obj_id);
    if (!obj) { ++uobj_stat_failed; return -1; }

    uint32_t gen = ++uobj_next_gen;
    obj->generation = gen;
    obj->type_id = type->type_id;
    obj->state = UOBJ_STATE_CREATING;
    obj->refcount = 1;
    obj->kog_node = 0;
    obj->flags = 0;

    if (type->init_fn) {
        if (type->init_fn(obj) != 0) {
            obj->present = 0;
            ++uobj_stat_failed;
            return -1;
        }
    }
    obj->state = UOBJ_STATE_ACTIVE;

    uint32_t handle_id;
    if (uobj_alloc_handle(obj_id, obj->generation, capabilities, &handle_id) != 0) {
        obj->present = 0;
        ++uobj_stat_failed;
        return -1;
    }

    evbus_publish(NP_EVENT_OBJ_CREATED, obj_id, 0, 0, 0);

    ++uobj_stat_created;
    *out_handle_id = handle_id;
    *out_obj_id = obj_id;
    return 0;
}

int uobj_create_handle_for_object(uint32_t obj_id, uint32_t capabilities, uint32_t *out_handle_id) {
    uobj_object_t *obj = uobj_find_object(obj_id);
    if (!obj) return -1;
    return uobj_alloc_handle(obj_id, obj->generation, capabilities, out_handle_id);
}

int uobj_from_handle(uint32_t handle_id, uint32_t required_capabilities, void **out_obj_ptr) {
    uobj_handle_t *hdl = uobj_find_handle(handle_id);
    if (!hdl) return -1;

    if (required_capabilities != 0) {
        if ((hdl->caps & required_capabilities) != required_capabilities) return -1;
    }

    uobj_object_t *obj = uobj_find_object(hdl->obj_id);
    if (!obj) return -1;

    if (hdl->generation != obj->generation) return -1; /* veraltete Referenz */
    if (obj->state >= UOBJ_STATE_DESTROYING) return -1;

    *out_obj_ptr = obj;
    return 0;
}

int uobj_retain(uint32_t obj_id) {
    uobj_object_t *obj = uobj_find_object(obj_id);
    if (!obj) return -1;
    ++obj->refcount;
    return 0;
}

int uobj_release(uint32_t obj_id) {
    uobj_object_t *obj = uobj_find_object(obj_id);
    if (!obj) return -1;

    if (--obj->refcount != 0) return 0;

    obj->state = UOBJ_STATE_DESTROYING;

    uobj_type_t *type = uobj_find_type(obj->type_id);
    if (type && type->destroy_fn) {
        type->destroy_fn(obj);
    }

    uint32_t obj_id_copy = obj->obj_id;
    obj->state = UOBJ_STATE_DESTROYED;
    obj->present = 0;
    ++uobj_stat_destroyed;

    evbus_publish(NP_EVENT_OBJ_DESTROYED, obj_id_copy, 0, 0, 0);
    return 0;
}

int uobj_close_handle(uint32_t handle_id) {
    uobj_handle_t *hdl = uobj_find_handle(handle_id);
    if (!hdl) return -1;
    hdl->present = 0;
    uobj_release(hdl->obj_id); /* Fehler ignorieren: Objekt koennte schon weg sein */
    return 0;
}

int uobj_query(uint32_t handle_id, uint32_t query_class, void *out_buf) {
    void *obj_ptr;
    if (uobj_from_handle(handle_id, UOBJ_CAP_QUERY, &obj_ptr) != 0) return -1;
    if (query_class != 0) return -1; /* nur Klasse 0 implementiert */

    uobj_object_t *obj = (uobj_object_t *)obj_ptr;
    uint32_t *out = (uint32_t *)out_buf;
    out[0] = obj->obj_id;
    out[1] = obj->type_id;
    out[2] = obj->generation;
    out[3] = obj->state;
    return 16;
}

void uobj_initialize(void) {
    for (uint32_t i = 0; i < UOBJ_MAX_TYPES; ++i) uobj_types[i] = (uobj_type_t){0};
    for (uint32_t i = 0; i < UOBJ_MAX_OBJECTS; ++i) uobj_objects[i] = (uobj_object_t){0};
    for (uint32_t i = 0; i < UOBJ_MAX_HANDLES; ++i) uobj_handles[i] = (uobj_handle_t){0};
    uobj_next_id = 0;
    uobj_next_gen = 0;
    uobj_next_hdl = 0;
    uobj_stat_created = 0;
    uobj_stat_destroyed = 0;
    uobj_stat_failed = 0;

    static const uint32_t builtin_types[] = {
        NP_OBJTYPE_KERNEL, NP_OBJTYPE_MACHINE, NP_OBJTYPE_NAMESPACE, NP_OBJTYPE_PROCESS,
        NP_OBJTYPE_THREAD, NP_OBJTYPE_CPU, NP_OBJTYPE_MEMORY, NP_OBJTYPE_IPC,
        NP_OBJTYPE_DEVICE, NP_OBJTYPE_DRIVER, NP_OBJTYPE_VFS_NODE, NP_OBJTYPE_SECURITY,
        NP_OBJTYPE_DIAGNOSTIC, NP_OBJTYPE_POWER, NP_OBJTYPE_GENERIC,
    };
    for (uint32_t i = 0; i < sizeof(builtin_types) / sizeof(builtin_types[0]); ++i) {
        uobj_register_type(builtin_types[i], 1, 0, 0, 0);
    }
}

int uobj_self_test(void) {
    uobj_type_t *generic_type = uobj_find_type(NP_OBJTYPE_GENERIC);
    if (!generic_type) return -1;

    if (uobj_register_type(NP_OBJTYPE_GENERIC, 1, 0, 0, 0) == 0) return -1; /* Duplikat */

    uint32_t handle_id, obj_id;
    if (uobj_create(NP_OBJTYPE_GENERIC, UOBJ_CAP_QUERY | UOBJ_CAP_DELETE, &handle_id, &obj_id) != 0) return -1;

    void *obj_ptr;
    if (uobj_from_handle(handle_id, UOBJ_CAP_QUERY, &obj_ptr) != 0) return -1;

    if (uobj_from_handle(handle_id, UOBJ_CAP_ADMIN, &obj_ptr) == 0) return -1; /* fehlende Rechte */

    if (uobj_retain(obj_id) != 0) return -1;
    uobj_object_t *obj = (uobj_object_t *)uobj_find_object(obj_id);
    if (!obj || obj->refcount != 2) return -1;

    if (uobj_release(obj_id) != 0) return -1;
    obj = (uobj_object_t *)uobj_find_object(obj_id);
    if (!obj || obj->refcount != 1 || !obj->present) return -1;

    uint8_t buf[16];
    int written = uobj_query(handle_id, 0, buf);
    if (written != 16) return -1;
    uint32_t returned_obj_id = *(uint32_t *)buf;
    if (returned_obj_id != obj_id) return -1;

    if (uobj_close_handle(handle_id) != 0) return -1;

    if (uobj_find_handle(handle_id) != 0) return -1; /* muss weg sein */

    return 0;
}
