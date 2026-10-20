/* NovaOS C-Kernel - Prozesslokaler Handle Manager (Port aus memory32.inc). */
#include "handle.h"
#include "object.h"
#include <stddef.h>

#define HANDLE_STATE_ACTIVE 1u

typedef struct {
    uint32_t object_handle;
    uint32_t rights;
    uint32_t flags;
    uint32_t generation;
    uint32_t type;
    uint32_t state;
} handle_entry_t; /* 24 Byte = HANDLE_ENTRY_SIZE */

/* Minimaler Object-Header-Blick, nur um OBJ_TYPE auszulesen (identisch zum
   Layout in object.c; object.h kapselt den Header absichtlich). */
typedef struct {
    uint32_t struct_size;
    uint32_t abi;
    uint32_t type;
} object_header_peek_t;

static handle_entry_t handle_table[HANDLE_CAPACITY];
static uint32_t handle_generations[HANDLE_CAPACITY];
static uint32_t handle_owner_pid = 0;
static uint32_t handle_active_count_value = 0;

void handle_manager_initialize(void) {
    for (uint32_t i = 0; i < HANDLE_CAPACITY; ++i) {
        handle_table[i] = (handle_entry_t){0};
        handle_generations[i] = 1;
    }
    handle_owner_pid = 0;
    handle_active_count_value = 0;
}

uint32_t handle_active_count(void) {
    return handle_active_count_value;
}

uint32_t handle_create(uint32_t pid, uint32_t object_handle, uint32_t type,
                        uint32_t rights, uint32_t flags) {
    if (pid != handle_owner_pid || pid == 0 || rights == 0) {
        return 0;
    }

    object_header_peek_t *obj = (object_header_peek_t *)object_lookup(object_handle);
    if (!obj) {
        return 0;
    }
    if (obj->type != type) {
        return 0;
    }

    uint32_t slot;
    for (slot = 0; slot < HANDLE_CAPACITY; ++slot) {
        if (handle_table[slot].state == 0) {
            break;
        }
    }
    if (slot >= HANDLE_CAPACITY) {
        return 0;
    }

    if (object_retain(object_handle) != 0) {
        return 0;
    }

    handle_entry_t *entry = &handle_table[slot];
    entry->object_handle = object_handle;
    entry->rights = rights;
    entry->flags = flags;
    entry->generation = handle_generations[slot];
    entry->type = type;
    entry->state = HANDLE_STATE_ACTIVE;
    ++handle_active_count_value;

    return (handle_generations[slot] << 16) | (slot + 1u);
}

void *handle_resolve(uint32_t pid, uint32_t handle, uint32_t expected_type,
                      uint32_t required_rights) {
    if (pid != handle_owner_pid) {
        return 0;
    }
    uint32_t slot_plus_one = handle & 0xFFFFu;
    if (slot_plus_one == 0) {
        return 0;
    }
    uint32_t slot = slot_plus_one - 1u;
    if (slot >= HANDLE_CAPACITY) {
        return 0;
    }
    handle_entry_t *entry = &handle_table[slot];
    if (entry->state != HANDLE_STATE_ACTIVE) {
        return 0;
    }
    if ((handle >> 16) != entry->generation) {
        return 0;
    }
    if (expected_type != entry->type) {
        return 0;
    }
    if ((entry->rights & required_rights) != required_rights) {
        return 0;
    }
    return object_lookup(entry->object_handle);
}

int handle_close(uint32_t pid, uint32_t handle) {
    if (pid != handle_owner_pid) {
        return -1;
    }
    uint32_t slot_plus_one = handle & 0xFFFFu;
    if (slot_plus_one == 0) {
        return -1;
    }
    uint32_t slot = slot_plus_one - 1u;
    if (slot >= HANDLE_CAPACITY) {
        return -1;
    }
    handle_entry_t *entry = &handle_table[slot];
    if (entry->state != HANDLE_STATE_ACTIVE) {
        return -1;
    }
    if ((handle >> 16) != entry->generation) {
        return -1;
    }
    if (entry->flags & HANDLE_FLAG_PROTECTED) {
        return -1;
    }
    if (object_release(entry->object_handle) != 0) {
        return -1;
    }

    entry->state = 0;
    ++handle_generations[slot];
    handle_generations[slot] &= 0xFFFFu;
    if (handle_generations[slot] == 0) {
        handle_generations[slot] = 1;
    }
    --handle_active_count_value;
    return 0;
}

int handle_manager_self_test(void) {
    int errors = 0;
    uint32_t obj_handle = 0;
    uint32_t proc_handle = 0;

    if (handle_active_count_value != 0) ++errors;

    handle_owner_pid = 1;
    obj_handle = object_create(0x48444C54u /* "HDLT" */, 0, 0);
    if (obj_handle == 0) {
        ++errors;
        goto cleanup;
    }

    proc_handle = handle_create(1, obj_handle, 0x48444C54u,
                                 HANDLE_RIGHT_QUERY | HANDLE_RIGHT_WAIT, 0);
    if (proc_handle == 0) {
        ++errors;
        goto cleanup_obj;
    }

    if (handle_active_count_value != 1) ++errors;

    if (handle_resolve(1, proc_handle, 0x48444C54u, HANDLE_RIGHT_QUERY) == 0) ++errors;

    if (handle_resolve(1, proc_handle, 0x44454144u /* "DEAD" */, HANDLE_RIGHT_QUERY) != 0) ++errors;

    if (handle_close(1, proc_handle) != 0) {
        ++errors;
        goto cleanup_handle;
    }

    if (handle_resolve(1, proc_handle, 0x48444C54u, HANDLE_RIGHT_QUERY) != 0) ++errors;
    if (handle_active_count_value != 0) ++errors;
    goto cleanup_obj;

cleanup_handle:
    handle_close(1, proc_handle);

cleanup_obj:
    if (obj_handle != 0) {
        object_release(obj_handle);
    }

cleanup:
    handle_owner_pid = 0;
    return (errors == 0) ? 0 : -1;
}
