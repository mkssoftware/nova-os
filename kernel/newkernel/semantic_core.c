/* NovaOS C-Kernel - Nova Semantic Core Bootstrap (siehe semantic_core.h
   fuer Scope/NPSPEC-Bezug). */
#include "semantic_core.h"

static semantic_core_registry_record_t g_registry[NOVA_REGISTRY_CAPACITY];
static uint32_t g_registry_count = 0;

static semantic_core_object_record_t g_objects[NOVA_OBJECT_CAPACITY];
static uint32_t g_object_count = 0;

static semantic_core_namespace_record_t g_namespaces[NOVA_NAMESPACE_CAPACITY];
static uint32_t g_namespace_count = 0;

static semantic_core_projection_record_t g_projections[NOVA_PROJECTION_CAPACITY];
static uint32_t g_projection_count = 0;

static semantic_core_capability_record_t g_capabilities[NOVA_CAPABILITY_CAPACITY];
static uint32_t g_capability_count = 0;

static semantic_core_handle_record_t g_handles[NOVA_HANDLE_CAPACITY];
static uint32_t g_handle_count = 0;
static uint32_t g_handle_next_id = 1;
static uint32_t g_handle_generation = 1;

static semantic_core_volume_record_t g_volumes[NOVA_VOLUME_CAPACITY];
static uint32_t g_volume_count = 0;

static const char NOVA_REGISTRY_NAME_OBJECT[] = "ObjectID";
static const char NOVA_REGISTRY_NAME_VOLUME[] = "VolumeID";
static const char NOVA_REGISTRY_NAME_NAMESPACE[] = "NamespaceID";
static const char NOVA_REGISTRY_NAME_CAPABILITY[] = "CapabilityID";
static const char NOVA_REGISTRY_NAME_SEMANTIC_TYPE[] = "SemanticTypeID";
static const char NOVA_NS_NAME_ROOT[] = "/";
static const char NOVA_NS_NAME_SYSTEM[] = "System";
static const char NOVA_NS_NAME_BENUTZER[] = "Benutzer";
static const char NOVA_NS_NAME_APPS[] = "Apps";
static const char NOVA_NS_NAME_VOLUMES[] = "Volumes";
static const char NOVA_NS_NAME_BOOT[] = "Boot";
static const char NOVA_NS_NAME_SOLUTIONS[] = "Solutions";

static int semantic_core_string_equals(const char *a, const char *b) {
    if (a == (const char *)0 || b == (const char *)0) return 0;
    while (*a != 0 && *b != 0) {
        if (*a != *b) return 0;
        a++; b++;
    }
    return *a == *b;
}

/* ---- Registry ---- */

int semantic_core_registry_lookup(uint32_t id_high, uint32_t id_low, semantic_core_registry_record_t **out) {
    if (id_high == 0 || id_low == 0) return -1;
    for (uint32_t i = 0; i < g_registry_count; ++i) {
        if (g_registry[i].id_high == id_high && g_registry[i].id_low == id_low) {
            *out = &g_registry[i];
            return 0;
        }
    }
    return -1;
}

int semantic_core_registry_add(uint32_t id_high, uint32_t id_low, uint32_t kind, const char *name) {
    if (id_high == 0 || id_low == 0 || kind == 0 || name == (const char *)0) return -1;
    if (g_registry_count >= NOVA_REGISTRY_CAPACITY) return -1;
    semantic_core_registry_record_t *rec = &g_registry[g_registry_count++];
    rec->kind = kind;
    rec->id_high = id_high;
    rec->id_low = id_low;
    rec->name = name;
    rec->flags = 1;
    return 0;
}

/* ---- Object Registry ---- */

int semantic_core_object_add(uint32_t id_high, uint32_t id_low, uint32_t ns_slot, const char *name, uint32_t flags) {
    if (id_high == 0 || id_low == 0 || ns_slot >= g_namespace_count || name == (const char *)0 || flags == 0) return -1;
    if (g_object_count >= NOVA_OBJECT_CAPACITY) return -1;
    semantic_core_object_record_t *rec = &g_objects[g_object_count++];
    rec->id_high = id_high;
    rec->id_low = id_low;
    rec->ns_slot = ns_slot;
    rec->name = name;
    rec->flags = flags;
    rec->sem_high = NOVA_ID_NAMESPACE_HIGH;
    rec->sem_low = id_low;
    return 0;
}

int semantic_core_object_lookup(uint32_t id_high, uint32_t id_low, semantic_core_object_record_t **out) {
    if (id_high == 0 || id_low == 0) return -1;
    for (uint32_t i = 0; i < g_object_count; ++i) {
        if (g_objects[i].id_high == id_high && g_objects[i].id_low == id_low) {
            *out = &g_objects[i];
            return 0;
        }
    }
    return -1;
}

uint32_t semantic_core_object_count_by_flags(uint32_t required_flags) {
    uint32_t result = 0;
    for (uint32_t i = 0; i < g_object_count; ++i) {
        if (required_flags == 0 || (g_objects[i].flags & required_flags) == required_flags) result++;
    }
    return result;
}

int semantic_core_object_at_by_flags(uint32_t required_flags, uint32_t ordinal, semantic_core_object_record_t **out) {
    uint32_t seen = 0;
    for (uint32_t i = 0; i < g_object_count; ++i) {
        if (required_flags != 0 && (g_objects[i].flags & required_flags) != required_flags) continue;
        if (seen == ordinal) { *out = &g_objects[i]; return 0; }
        seen++;
    }
    return -1;
}

int semantic_core_object_validate_projection(uint32_t id_high, uint32_t id_low, semantic_core_projection_record_t **out) {
    if (id_high == 0 || id_low == 0) return -1;
    semantic_core_object_record_t *obj;
    if (semantic_core_object_lookup(id_high, id_low, &obj) != 0) return -1;
    uint32_t ns_slot = obj->ns_slot;
    uint32_t sem_high = obj->sem_high, sem_low = obj->sem_low;
    semantic_core_projection_record_t *proj;
    if (semantic_core_projection_lookup_by_object(id_high, id_low, &proj) != 0) return -1;
    if (proj->ns_slot != ns_slot) return -1;
    if (proj->target_high != sem_high || proj->target_low != sem_low) return -1;
    *out = proj;
    return 0;
}

int semantic_core_object_resolve_path(const char *path, semantic_core_object_record_t **out) {
    if (path == (const char *)0) return -1;
    semantic_core_namespace_record_t *ns;
    if (semantic_core_namespace_resolve_path(path, &ns) != 0) return -1;
    uint32_t object_high = ns->object_high, object_low = ns->object_low;
    semantic_core_projection_record_t *proj;
    if (semantic_core_object_validate_projection(object_high, object_low, &proj) != 0) return -1;
    return semantic_core_object_lookup(object_high, object_low, out);
}

/* ---- Volumes ---- */

int semantic_core_volume_add(uint32_t id_high, uint32_t id_low, uint32_t ns_slot,
                              uint32_t object_high, uint32_t object_low, uint32_t flags, uint32_t fs_type) {
    if (id_high == 0 || id_low == 0 || ns_slot >= g_namespace_count || object_high == 0 || object_low == 0) return -1;
    semantic_core_projection_record_t *proj;
    if (semantic_core_object_validate_projection(object_high, object_low, &proj) != 0) return -1;
    if (proj->ns_slot != ns_slot) return -1;
    if (g_volume_count >= NOVA_VOLUME_CAPACITY) return -1;
    semantic_core_volume_record_t *rec = &g_volumes[g_volume_count++];
    rec->id_high = id_high;
    rec->id_low = id_low;
    rec->ns_slot = ns_slot;
    rec->object_high = object_high;
    rec->object_low = object_low;
    rec->flags = flags;
    rec->fs_type = fs_type;
    return 0;
}

int semantic_core_volume_lookup(uint32_t id_high, uint32_t id_low, semantic_core_volume_record_t **out) {
    if (id_high == 0 || id_low == 0) return -1;
    for (uint32_t i = 0; i < g_volume_count; ++i) {
        if (g_volumes[i].id_high == id_high && g_volumes[i].id_low == id_low) {
            *out = &g_volumes[i];
            return 0;
        }
    }
    return -1;
}

int semantic_core_volume_lookup_by_namespace(uint32_t ns_slot, semantic_core_volume_record_t **out) {
    if (ns_slot >= g_namespace_count) return -1;
    for (uint32_t i = 0; i < g_volume_count; ++i) {
        if (g_volumes[i].ns_slot == ns_slot) { *out = &g_volumes[i]; return 0; }
    }
    return -1;
}

/* ---- Namespace ---- */

int semantic_core_namespace_add(uint32_t id_high, uint32_t id_low, uint32_t parent, const char *name,
                                 uint32_t object_high, uint32_t object_low, uint32_t flags) {
    if (id_high == 0 || id_low == 0 || name == (const char *)0 || object_high == 0 || object_low == 0) return -1;
    if (g_namespace_count >= NOVA_NAMESPACE_CAPACITY) return -1;
    semantic_core_namespace_record_t *rec = &g_namespaces[g_namespace_count++];
    rec->id_high = id_high;
    rec->id_low = id_low;
    rec->parent = parent;
    rec->name = name;
    rec->kind = NOVA_ID_KIND_NAMESPACE;
    rec->flags = flags;
    rec->object_high = object_high;
    rec->object_low = object_low;
    return 0;
}

int semantic_core_namespace_lookup(uint32_t id_high, uint32_t id_low, semantic_core_namespace_record_t **out) {
    if (id_high == 0 || id_low == 0) return -1;
    for (uint32_t i = 0; i < g_namespace_count; ++i) {
        if (g_namespaces[i].id_high == id_high && g_namespaces[i].id_low == id_low) {
            *out = &g_namespaces[i];
            return 0;
        }
    }
    return -1;
}

/* Fruehe Bootstrap-Variante: nur "/" und genau EIN Segment direkt unter
   Root (z.B. "/System") - siehe semantic_core.h. */
int semantic_core_namespace_resolve_path(const char *path, semantic_core_namespace_record_t **out) {
    if (path == (const char *)0 || path[0] != '/') return -1;
    if (path[1] == 0) return semantic_core_namespace_record(NOVA_NAMESPACE_ROOT, out);
    const char *name = path + 1;
    for (uint32_t i = 0; i < g_namespace_count; ++i) {
        if (g_namespaces[i].parent != NOVA_NAMESPACE_ROOT) continue;
        if (semantic_core_string_equals(name, g_namespaces[i].name)) { *out = &g_namespaces[i]; return 0; }
    }
    return -1;
}

int semantic_core_namespace_count_children(uint32_t parent_slot, uint32_t required_flags, uint32_t *out_count) {
    if (parent_slot >= g_namespace_count) return -1;
    uint32_t result = 0;
    for (uint32_t i = 0; i < g_namespace_count; ++i) {
        if (g_namespaces[i].parent != parent_slot) continue;
        if (required_flags != 0 && (g_namespaces[i].flags & required_flags) != required_flags) continue;
        result++;
    }
    *out_count = result;
    return 0;
}

int semantic_core_namespace_child_at(uint32_t parent_slot, uint32_t required_flags, uint32_t ordinal,
                                      semantic_core_namespace_record_t **out) {
    if (parent_slot >= g_namespace_count) return -1;
    uint32_t seen = 0;
    for (uint32_t i = 0; i < g_namespace_count; ++i) {
        if (g_namespaces[i].parent != parent_slot) continue;
        if (required_flags != 0 && (g_namespaces[i].flags & required_flags) != required_flags) continue;
        if (seen == ordinal) { *out = &g_namespaces[i]; return 0; }
        seen++;
    }
    return -1;
}

int semantic_core_namespace_record(uint32_t slot, semantic_core_namespace_record_t **out) {
    if (slot >= g_namespace_count) return -1;
    *out = &g_namespaces[slot];
    return 0;
}

/* ---- Projections ---- */

int semantic_core_projection_add(uint32_t object_high, uint32_t object_low, uint32_t ns_slot, const char *name,
                                  uint32_t target_high, uint32_t target_low, uint32_t flags) {
    if (object_high == 0 || object_low == 0 || ns_slot >= g_namespace_count || name == (const char *)0 ||
        target_high == 0 || target_low == 0) return -1;
    if (g_projection_count >= NOVA_PROJECTION_CAPACITY) return -1;
    semantic_core_projection_record_t *rec = &g_projections[g_projection_count++];
    rec->object_high = object_high;
    rec->object_low = object_low;
    rec->ns_slot = ns_slot;
    rec->name = name;
    rec->flags = flags;
    rec->target_high = target_high;
    rec->target_low = target_low;
    return 0;
}

int semantic_core_projection_lookup_by_object(uint32_t object_high, uint32_t object_low, semantic_core_projection_record_t **out) {
    if (object_high == 0 || object_low == 0) return -1;
    for (uint32_t i = 0; i < g_projection_count; ++i) {
        if (g_projections[i].object_high == object_high && g_projections[i].object_low == object_low) {
            *out = &g_projections[i];
            return 0;
        }
    }
    return -1;
}

int semantic_core_projection_count_by_namespace(uint32_t ns_slot, uint32_t *out_count) {
    if (ns_slot >= g_namespace_count) return -1;
    uint32_t result = 0;
    for (uint32_t i = 0; i < g_projection_count; ++i) {
        if (g_projections[i].ns_slot == ns_slot) result++;
    }
    *out_count = result;
    return 0;
}

int semantic_core_projection_at_namespace(uint32_t ns_slot, uint32_t required_flags, uint32_t ordinal,
                                           semantic_core_projection_record_t **out) {
    if (ns_slot >= g_namespace_count) return -1;
    uint32_t seen = 0;
    for (uint32_t i = 0; i < g_projection_count; ++i) {
        if (g_projections[i].ns_slot != ns_slot) continue;
        (void)required_flags; /* im ASM-Original ungenutzt (immer 0 uebergeben) - 1:1 uebernommen */
        if (seen == ordinal) { *out = &g_projections[i]; return 0; }
        seen++;
    }
    return -1;
}

/* ---- Capabilities ---- */

int semantic_core_capability_add(uint32_t id_high, uint32_t id_low, uint32_t object_high, uint32_t object_low,
                                  uint32_t ns_slot, uint32_t rights, uint32_t flags) {
    if (id_high == 0 || id_low == 0 || object_high == 0 || object_low == 0 ||
        ns_slot >= g_namespace_count || rights == 0) return -1;
    if (g_capability_count >= NOVA_CAPABILITY_CAPACITY) return -1;
    semantic_core_capability_record_t *rec = &g_capabilities[g_capability_count++];
    rec->id_high = id_high;
    rec->id_low = id_low;
    rec->object_high = object_high;
    rec->object_low = object_low;
    rec->ns_slot = ns_slot;
    rec->rights = rights;
    rec->flags = flags;
    return 0;
}

int semantic_core_capability_lookup(uint32_t id_high, uint32_t id_low, semantic_core_capability_record_t **out) {
    if (id_high == 0 || id_low == 0) return -1;
    for (uint32_t i = 0; i < g_capability_count; ++i) {
        if ((g_capabilities[i].flags & NOVA_CAP_FLAG_ACTIVE) == 0) continue;
        if (g_capabilities[i].id_high == id_high && g_capabilities[i].id_low == id_low) {
            *out = &g_capabilities[i];
            return 0;
        }
    }
    return -1;
}

int semantic_core_capability_revoke(uint32_t id_high, uint32_t id_low) {
    semantic_core_capability_record_t *rec;
    if (semantic_core_capability_lookup(id_high, id_low, &rec) != 0) return -1;
    rec->flags &= ~NOVA_CAP_FLAG_ACTIVE;
    return 0;
}

int semantic_core_capability_check_object_rights(uint32_t object_high, uint32_t object_low, uint32_t rights,
                                                  semantic_core_capability_record_t **out) {
    if (object_high == 0 || object_low == 0 || rights == 0) return -1;
    for (uint32_t i = 0; i < g_capability_count; ++i) {
        if ((g_capabilities[i].flags & NOVA_CAP_FLAG_ACTIVE) == 0) continue;
        if (g_capabilities[i].object_high != object_high || g_capabilities[i].object_low != object_low) continue;
        if ((g_capabilities[i].rights & rights) != rights) continue;
        *out = &g_capabilities[i];
        return 0;
    }
    return -1;
}

/* ---- Handles ---- */

int semantic_core_handle_open_by_object(uint32_t object_high, uint32_t object_low, uint32_t rights,
                                         uint32_t *out_handle_id, semantic_core_handle_record_t **out) {
    if (object_high == 0 || object_low == 0 || rights == 0) return -1;
    semantic_core_projection_record_t *proj_unused;
    if (semantic_core_object_validate_projection(object_high, object_low, &proj_unused) != 0) return -1;
    semantic_core_capability_record_t *cap_unused;
    if (semantic_core_capability_check_object_rights(object_high, object_low, rights, &cap_unused) != 0) return -1;
    semantic_core_projection_record_t *proj;
    if (semantic_core_projection_lookup_by_object(object_high, object_low, &proj) != 0) return -1;
    uint32_t ns_slot = proj->ns_slot;

    semantic_core_handle_record_t *rec = (semantic_core_handle_record_t *)0;
    for (uint32_t i = 0; i < g_handle_count; ++i) {
        if ((g_handles[i].flags & NOVA_HANDLE_FLAG_ACTIVE) == 0) { rec = &g_handles[i]; break; }
    }
    if (rec == (semantic_core_handle_record_t *)0) {
        if (g_handle_count >= NOVA_HANDLE_CAPACITY) return -1;
        rec = &g_handles[g_handle_count++];
    }
    rec->id = g_handle_next_id;
    rec->object_high = object_high;
    rec->object_low = object_low;
    rec->ns_slot = ns_slot;
    rec->rights = rights;
    rec->flags = NOVA_HANDLE_FLAG_ACTIVE;
    rec->generation = g_handle_generation;
    *out_handle_id = rec->id;
    g_handle_next_id++;
    g_handle_generation++;
    if (out != (semantic_core_handle_record_t **)0) *out = rec;
    return 0;
}

int semantic_core_handle_lookup(uint32_t handle_id, semantic_core_handle_record_t **out) {
    if (handle_id == 0) return -1;
    for (uint32_t i = 0; i < g_handle_count; ++i) {
        if (g_handles[i].id != handle_id) continue;
        if ((g_handles[i].flags & NOVA_HANDLE_FLAG_ACTIVE) == 0) return -1;
        *out = &g_handles[i];
        return 0;
    }
    return -1;
}

int semantic_core_handle_validate_rights(uint32_t handle_id, uint32_t rights, semantic_core_handle_record_t **out) {
    if (rights == 0) return -1;
    semantic_core_handle_record_t *rec;
    if (semantic_core_handle_lookup(handle_id, &rec) != 0) return -1;
    if ((rec->rights & rights) != rights) return -1;
    semantic_core_capability_record_t *cap_unused;
    if (semantic_core_capability_check_object_rights(rec->object_high, rec->object_low, rights, &cap_unused) != 0) return -1;
    *out = rec;
    return 0;
}

int semantic_core_handle_close(uint32_t handle_id) {
    semantic_core_handle_record_t *rec;
    if (semantic_core_handle_lookup(handle_id, &rec) != 0) return -1;
    rec->flags &= ~NOVA_HANDLE_FLAG_ACTIVE;
    return 0;
}

int semantic_core_handle_open_by_path(const char *path, uint32_t rights,
                                       uint32_t *out_handle_id, semantic_core_handle_record_t **out) {
    if (path == (const char *)0 || rights == 0) return -1;
    semantic_core_namespace_record_t *ns;
    if (semantic_core_namespace_resolve_path(path, &ns) != 0) return -1;
    return semantic_core_handle_open_by_object(ns->object_high, ns->object_low, rights, out_handle_id, out);
}

/* ---- Bootstrap ---- */

void semantic_core_initialize(void) {
    g_registry_count = 0;
    g_object_count = 0;
    g_namespace_count = 0;
    g_projection_count = 0;
    g_capability_count = 0;
    g_handle_count = 0;
    g_handle_next_id = 1;
    g_handle_generation = 1;
    g_volume_count = 0;

    semantic_core_registry_add(NOVA_ID_OBJECT_HIGH, 1, NOVA_ID_KIND_OBJECT, NOVA_REGISTRY_NAME_OBJECT);
    semantic_core_registry_add(NOVA_ID_VOLUME_HIGH, 1, NOVA_ID_KIND_VOLUME, NOVA_REGISTRY_NAME_VOLUME);
    semantic_core_registry_add(NOVA_ID_NAMESPACE_HIGH, 1, NOVA_ID_KIND_NAMESPACE, NOVA_REGISTRY_NAME_NAMESPACE);
    semantic_core_registry_add(NOVA_ID_CAPABILITY_HIGH, 1, NOVA_ID_KIND_CAPABILITY, NOVA_REGISTRY_NAME_CAPABILITY);
    semantic_core_registry_add(NOVA_ID_SEMANTIC_HIGH, 1, NOVA_ID_KIND_SEMANTIC_TYPE, NOVA_REGISTRY_NAME_SEMANTIC_TYPE);

    /* Root und Standardbereiche des globalen Namespace (Namespace +
       Projection je Bereich, danach dessen Filesystem-Object). */
    semantic_core_namespace_add(NOVA_ID_NAMESPACE_HIGH, 1, 0xFFFFFFFFu, NOVA_NS_NAME_ROOT,
                                 NOVA_ID_OBJECT_HIGH, 1, NOVA_NS_FLAG_STABLE);
    semantic_core_projection_add(NOVA_ID_OBJECT_HIGH, 1, NOVA_NAMESPACE_ROOT, NOVA_NS_NAME_ROOT,
                                  NOVA_ID_NAMESPACE_HIGH, 1, NOVA_PROJ_FLAG_PRIMARY | NOVA_PROJ_FLAG_STABLE);

    semantic_core_namespace_add(NOVA_ID_NAMESPACE_HIGH, 2, NOVA_NAMESPACE_ROOT, NOVA_NS_NAME_SYSTEM,
                                 NOVA_ID_OBJECT_HIGH, 2, NOVA_NS_FLAG_STABLE | NOVA_NS_FLAG_SYSTEM);
    semantic_core_projection_add(NOVA_ID_OBJECT_HIGH, 2, NOVA_NAMESPACE_SYSTEM, NOVA_NS_NAME_SYSTEM,
                                  NOVA_ID_NAMESPACE_HIGH, 2, NOVA_PROJ_FLAG_PRIMARY | NOVA_PROJ_FLAG_STABLE);

    semantic_core_namespace_add(NOVA_ID_NAMESPACE_HIGH, 3, NOVA_NAMESPACE_ROOT, NOVA_NS_NAME_BENUTZER,
                                 NOVA_ID_OBJECT_HIGH, 3, NOVA_NS_FLAG_STABLE | NOVA_NS_FLAG_USER_VISIBLE);
    semantic_core_projection_add(NOVA_ID_OBJECT_HIGH, 3, NOVA_NAMESPACE_BENUTZER, NOVA_NS_NAME_BENUTZER,
                                  NOVA_ID_NAMESPACE_HIGH, 3, NOVA_PROJ_FLAG_PRIMARY | NOVA_PROJ_FLAG_STABLE);

    semantic_core_namespace_add(NOVA_ID_NAMESPACE_HIGH, 4, NOVA_NAMESPACE_ROOT, NOVA_NS_NAME_APPS,
                                 NOVA_ID_OBJECT_HIGH, 4, NOVA_NS_FLAG_STABLE | NOVA_NS_FLAG_USER_VISIBLE);
    semantic_core_projection_add(NOVA_ID_OBJECT_HIGH, 4, NOVA_NAMESPACE_APPS, NOVA_NS_NAME_APPS,
                                  NOVA_ID_NAMESPACE_HIGH, 4, NOVA_PROJ_FLAG_PRIMARY | NOVA_PROJ_FLAG_STABLE);

    semantic_core_namespace_add(NOVA_ID_NAMESPACE_HIGH, 5, NOVA_NAMESPACE_ROOT, NOVA_NS_NAME_VOLUMES,
                                 NOVA_ID_OBJECT_HIGH, 5, NOVA_NS_FLAG_STABLE | NOVA_NS_FLAG_USER_VISIBLE);
    semantic_core_projection_add(NOVA_ID_OBJECT_HIGH, 5, NOVA_NAMESPACE_VOLUMES, NOVA_NS_NAME_VOLUMES,
                                  NOVA_ID_NAMESPACE_HIGH, 5, NOVA_PROJ_FLAG_PRIMARY | NOVA_PROJ_FLAG_STABLE);

    semantic_core_namespace_add(NOVA_ID_NAMESPACE_HIGH, 6, NOVA_NAMESPACE_ROOT, NOVA_NS_NAME_BOOT,
                                 NOVA_ID_OBJECT_HIGH, 6, NOVA_NS_FLAG_STABLE | NOVA_NS_FLAG_SYSTEM);
    semantic_core_projection_add(NOVA_ID_OBJECT_HIGH, 6, NOVA_NAMESPACE_BOOT, NOVA_NS_NAME_BOOT,
                                  NOVA_ID_NAMESPACE_HIGH, 6, NOVA_PROJ_FLAG_PRIMARY | NOVA_PROJ_FLAG_STABLE);

    semantic_core_namespace_add(NOVA_ID_NAMESPACE_HIGH, 7, NOVA_NAMESPACE_ROOT, NOVA_NS_NAME_SOLUTIONS,
                                 NOVA_ID_OBJECT_HIGH, 7, NOVA_NS_FLAG_STABLE | NOVA_NS_FLAG_USER_VISIBLE);
    semantic_core_projection_add(NOVA_ID_OBJECT_HIGH, 7, NOVA_NAMESPACE_SOLUTIONS, NOVA_NS_NAME_SOLUTIONS,
                                  NOVA_ID_NAMESPACE_HIGH, 7, NOVA_PROJ_FLAG_PRIMARY | NOVA_PROJ_FLAG_STABLE);

    /* Fruehe Filesystem-Object-Registry fuer stabile Namespace-Objekte. */
    semantic_core_object_add(NOVA_ID_OBJECT_HIGH, 1, NOVA_NAMESPACE_ROOT, NOVA_NS_NAME_ROOT,
                              NOVA_OBJECT_FLAG_STABLE | NOVA_OBJECT_FLAG_SYSTEM | NOVA_OBJECT_FLAG_NAMESPACE);
    semantic_core_object_add(NOVA_ID_OBJECT_HIGH, 2, NOVA_NAMESPACE_SYSTEM, NOVA_NS_NAME_SYSTEM,
                              NOVA_OBJECT_FLAG_STABLE | NOVA_OBJECT_FLAG_SYSTEM | NOVA_OBJECT_FLAG_NAMESPACE);
    semantic_core_object_add(NOVA_ID_OBJECT_HIGH, 3, NOVA_NAMESPACE_BENUTZER, NOVA_NS_NAME_BENUTZER,
                              NOVA_OBJECT_FLAG_STABLE | NOVA_OBJECT_FLAG_USER_VISIBLE | NOVA_OBJECT_FLAG_NAMESPACE);
    semantic_core_object_add(NOVA_ID_OBJECT_HIGH, 4, NOVA_NAMESPACE_APPS, NOVA_NS_NAME_APPS,
                              NOVA_OBJECT_FLAG_STABLE | NOVA_OBJECT_FLAG_USER_VISIBLE | NOVA_OBJECT_FLAG_NAMESPACE);
    semantic_core_object_add(NOVA_ID_OBJECT_HIGH, 5, NOVA_NAMESPACE_VOLUMES, NOVA_NS_NAME_VOLUMES,
                              NOVA_OBJECT_FLAG_STABLE | NOVA_OBJECT_FLAG_USER_VISIBLE | NOVA_OBJECT_FLAG_NAMESPACE);
    semantic_core_object_add(NOVA_ID_OBJECT_HIGH, 6, NOVA_NAMESPACE_BOOT, NOVA_NS_NAME_BOOT,
                              NOVA_OBJECT_FLAG_STABLE | NOVA_OBJECT_FLAG_SYSTEM | NOVA_OBJECT_FLAG_NAMESPACE);
    semantic_core_object_add(NOVA_ID_OBJECT_HIGH, 7, NOVA_NAMESPACE_SOLUTIONS, NOVA_NS_NAME_SOLUTIONS,
                              NOVA_OBJECT_FLAG_STABLE | NOVA_OBJECT_FLAG_USER_VISIBLE | NOVA_OBJECT_FLAG_NAMESPACE);

    /* Fruehes Boot-Volume: noch kein Block-I/O, aber stabile Zuordnung
       von VolumeID, Namespace und Filesystem-Object fuer VFS/Mount-Aufbau. */
    semantic_core_volume_add(NOVA_ID_VOLUME_HIGH, 1, NOVA_NAMESPACE_BOOT, NOVA_ID_OBJECT_HIGH, 6,
                              NOVA_VOLUME_FLAG_MOUNTED | NOVA_VOLUME_FLAG_READONLY |
                              NOVA_VOLUME_FLAG_TRUSTED | NOVA_VOLUME_FLAG_BOOT, NOVA_VOLUME_FS_BOOT);

    /* Feste fruehe Kernel-Capabilities fuer Systemdienste. */
    semantic_core_capability_add(NOVA_ID_CAPABILITY_HIGH, 1, NOVA_ID_OBJECT_HIGH, 2, NOVA_NAMESPACE_SYSTEM,
                                  NOVA_HANDLE_RIGHT_READ | NOVA_HANDLE_RIGHT_MANAGE,
                                  NOVA_CAP_FLAG_ACTIVE | NOVA_CAP_FLAG_STABLE);
    semantic_core_capability_add(NOVA_ID_CAPABILITY_HIGH, 2, NOVA_ID_OBJECT_HIGH, 4, NOVA_NAMESPACE_APPS,
                                  NOVA_HANDLE_RIGHT_READ | NOVA_HANDLE_RIGHT_EXECUTE,
                                  NOVA_CAP_FLAG_ACTIVE | NOVA_CAP_FLAG_STABLE);
    semantic_core_capability_add(NOVA_ID_CAPABILITY_HIGH, 3, NOVA_ID_OBJECT_HIGH, 6, NOVA_NAMESPACE_BOOT,
                                  NOVA_HANDLE_RIGHT_READ | NOVA_HANDLE_RIGHT_EXECUTE,
                                  NOVA_CAP_FLAG_ACTIVE | NOVA_CAP_FLAG_STABLE);
    semantic_core_capability_add(NOVA_ID_CAPABILITY_HIGH, 4, NOVA_ID_OBJECT_HIGH, 5, NOVA_NAMESPACE_VOLUMES,
                                  NOVA_HANDLE_RIGHT_READ, NOVA_CAP_FLAG_ACTIVE);
    semantic_core_capability_add(NOVA_ID_CAPABILITY_HIGH, 5, NOVA_ID_OBJECT_HIGH, 7, NOVA_NAMESPACE_SOLUTIONS,
                                  NOVA_HANDLE_RIGHT_READ | NOVA_HANDLE_RIGHT_EXECUTE, NOVA_CAP_FLAG_ACTIVE);
}

/* ---- Selbsttest (1:1 Faelle aus semantic_core32.inc uebernommen) ---- */

int semantic_core_self_test(void) {
    static const char NS_PATH_ROOT[] = "/";
    static const char NS_PATH_SYSTEM[] = "/System";
    static const char NS_PATH_APPS[] = "/Apps";
    static const char NS_PATH_SOLUTIONS[] = "/Solutions";
    static const char NS_PATH_MISSING[] = "/NichtVorhanden";

    if (g_registry_count != 5) return -1;
    if (g_object_count != 7) return -1;
    if (g_namespace_count != 7) return -1;
    if (g_projection_count != 7) return -1;
    if (g_capability_count != 5) return -1;
    if (g_handle_count != 0) return -1;
    if (g_volume_count != 1) return -1;

    semantic_core_registry_record_t *reg;
    if (semantic_core_registry_lookup(NOVA_ID_OBJECT_HIGH, 1, &reg) != 0) return -1;
    if (reg->kind != NOVA_ID_KIND_OBJECT) return -1;
    if (semantic_core_registry_lookup(NOVA_ID_CAPABILITY_HIGH, 1, &reg) != 0) return -1;
    if (reg->kind != NOVA_ID_KIND_CAPABILITY) return -1;

    semantic_core_object_record_t *obj;
    if (semantic_core_object_lookup(NOVA_ID_OBJECT_HIGH, 4, &obj) != 0) return -1;
    if (obj->ns_slot != NOVA_NAMESPACE_APPS) return -1;
    if ((obj->flags & NOVA_OBJECT_FLAG_USER_VISIBLE) == 0) return -1;
    if (obj->sem_high != NOVA_ID_NAMESPACE_HIGH || obj->sem_low != 4) return -1;

    if (semantic_core_object_count_by_flags(NOVA_OBJECT_FLAG_SYSTEM) != 3) return -1;
    if (semantic_core_object_count_by_flags(NOVA_OBJECT_FLAG_USER_VISIBLE) != 4) return -1;

    if (semantic_core_object_at_by_flags(NOVA_OBJECT_FLAG_USER_VISIBLE, 0, &obj) != 0) return -1;
    if (obj->id_low != 3) return -1;
    if (semantic_core_object_at_by_flags(NOVA_OBJECT_FLAG_USER_VISIBLE, 2, &obj) != 0) return -1;
    if (obj->id_low != 5) return -1;
    if (semantic_core_object_at_by_flags(NOVA_OBJECT_FLAG_USER_VISIBLE, 3, &obj) != 0) return -1;
    if (obj->id_low != 7) return -1;
    if (semantic_core_object_at_by_flags(NOVA_OBJECT_FLAG_SYSTEM, 2, &obj) != 0) return -1;
    if (obj->id_low != 6) return -1;
    if (semantic_core_object_at_by_flags(NOVA_OBJECT_FLAG_SYSTEM, 3, &obj) == 0) return -1;

    semantic_core_projection_record_t *proj;
    if (semantic_core_object_validate_projection(NOVA_ID_OBJECT_HIGH, 4, &proj) != 0) return -1;
    if (proj->ns_slot != NOVA_NAMESPACE_APPS) return -1;
    if (semantic_core_object_validate_projection(NOVA_ID_OBJECT_HIGH, 0x7000, &proj) == 0) return -1;
    if (semantic_core_object_lookup(NOVA_ID_OBJECT_HIGH, 0x7000, &obj) == 0) return -1;

    if (semantic_core_object_resolve_path(NS_PATH_SYSTEM, &obj) != 0) return -1;
    if (obj->id_low != 2) return -1;
    if ((obj->flags & NOVA_OBJECT_FLAG_SYSTEM) == 0) return -1;
    if (semantic_core_object_resolve_path(NS_PATH_APPS, &obj) != 0) return -1;
    if (obj->id_low != 4) return -1;
    if ((obj->flags & NOVA_OBJECT_FLAG_USER_VISIBLE) == 0) return -1;
    if (semantic_core_object_resolve_path(NS_PATH_SOLUTIONS, &obj) != 0) return -1;
    if (obj->id_low != 7) return -1;
    if ((obj->flags & NOVA_OBJECT_FLAG_USER_VISIBLE) == 0) return -1;
    if (semantic_core_object_resolve_path(NS_PATH_MISSING, &obj) == 0) return -1;

    semantic_core_volume_record_t *vol;
    if (semantic_core_volume_lookup(NOVA_ID_VOLUME_HIGH, 1, &vol) != 0) return -1;
    if (vol->ns_slot != NOVA_NAMESPACE_BOOT) return -1;
    if (vol->object_high != NOVA_ID_OBJECT_HIGH || vol->object_low != 6) return -1;
    if ((vol->flags & NOVA_VOLUME_FLAG_MOUNTED) == 0) return -1;
    if ((vol->flags & NOVA_VOLUME_FLAG_READONLY) == 0) return -1;
    if ((vol->flags & NOVA_VOLUME_FLAG_TRUSTED) == 0) return -1;
    if ((vol->flags & NOVA_VOLUME_FLAG_BOOT) == 0) return -1;
    if (vol->fs_type != NOVA_VOLUME_FS_BOOT) return -1;
    if (semantic_core_volume_lookup_by_namespace(NOVA_NAMESPACE_BOOT, &vol) != 0) return -1;
    if (vol->id_low != 1) return -1;
    if (semantic_core_volume_lookup_by_namespace(NOVA_NAMESPACE_SYSTEM, &vol) == 0) return -1;
    if (semantic_core_volume_lookup(NOVA_ID_VOLUME_HIGH, 0x7000, &vol) == 0) return -1;

    uint32_t handle_id;
    semantic_core_handle_record_t *hdl;
    if (semantic_core_handle_open_by_object(NOVA_ID_OBJECT_HIGH, 0x7000, NOVA_HANDLE_RIGHT_READ, &handle_id, &hdl) == 0) return -1;

    semantic_core_namespace_record_t *ns;
    if (semantic_core_namespace_lookup(NOVA_ID_NAMESPACE_HIGH, 2, &ns) != 0) return -1;
    if (ns->name != NOVA_NS_NAME_SYSTEM) return -1;
    if (semantic_core_namespace_resolve_path(NS_PATH_ROOT, &ns) != 0) return -1;
    if (ns->id_low != 1) return -1;
    if (semantic_core_namespace_resolve_path(NS_PATH_SYSTEM, &ns) != 0) return -1;
    if (ns->object_low != 2) return -1;
    if (semantic_core_namespace_resolve_path(NS_PATH_APPS, &ns) != 0) return -1;
    if (ns->object_low != 4) return -1;
    if (semantic_core_namespace_resolve_path(NS_PATH_SOLUTIONS, &ns) != 0) return -1;
    if (ns->object_low != 7) return -1;
    if (semantic_core_namespace_resolve_path(NS_PATH_MISSING, &ns) == 0) return -1;

    uint32_t child_count;
    if (semantic_core_namespace_count_children(NOVA_NAMESPACE_ROOT, 0, &child_count) != 0) return -1;
    if (child_count != 6) return -1;
    if (semantic_core_namespace_count_children(NOVA_NAMESPACE_ROOT, NOVA_NS_FLAG_USER_VISIBLE, &child_count) != 0) return -1;
    if (child_count != 4) return -1;
    if (semantic_core_namespace_count_children(NOVA_NAMESPACE_ROOT, NOVA_NS_FLAG_SYSTEM, &child_count) != 0) return -1;
    if (child_count != 2) return -1;

    if (semantic_core_namespace_child_at(NOVA_NAMESPACE_ROOT, NOVA_NS_FLAG_USER_VISIBLE, 0, &ns) != 0) return -1;
    if (ns->object_low != 3) return -1;
    if (semantic_core_namespace_child_at(NOVA_NAMESPACE_ROOT, NOVA_NS_FLAG_USER_VISIBLE, 2, &ns) != 0) return -1;
    if (ns->object_low != 5) return -1;
    if (semantic_core_namespace_child_at(NOVA_NAMESPACE_ROOT, NOVA_NS_FLAG_USER_VISIBLE, 3, &ns) != 0) return -1;
    if (ns->object_low != 7) return -1;
    if (semantic_core_namespace_child_at(NOVA_NAMESPACE_ROOT, NOVA_NS_FLAG_USER_VISIBLE, 4, &ns) == 0) return -1;
    if (semantic_core_namespace_child_at(NOVA_NAMESPACE_ROOT, 0, 4, &ns) != 0) return -1;
    if (ns->object_low != 6) return -1;
    if (semantic_core_namespace_child_at(NOVA_NAMESPACE_ROOT, 0, 5, &ns) != 0) return -1;
    if (ns->object_low != 7) return -1;

    if (semantic_core_handle_open_by_path(NS_PATH_SYSTEM, NOVA_HANDLE_RIGHT_READ, &handle_id, &hdl) != 0) return -1;
    if (hdl->object_low != 2) return -1;
    if (semantic_core_handle_validate_rights(handle_id, NOVA_HANDLE_RIGHT_READ, &hdl) != 0) return -1;
    if (semantic_core_handle_close(handle_id) != 0) return -1;

    if (semantic_core_namespace_record(NOVA_NAMESPACE_ROOT, &ns) != 0) return -1;
    if (ns->parent != 0xFFFFFFFFu) return -1;
    if (ns->id_high != NOVA_ID_NAMESPACE_HIGH || ns->id_low != 1) return -1;
    if (semantic_core_namespace_record(NOVA_NAMESPACE_SYSTEM, &ns) != 0) return -1;
    if (ns->parent != NOVA_NAMESPACE_ROOT) return -1;
    if ((ns->flags & NOVA_NS_FLAG_SYSTEM) == 0) return -1;
    if (semantic_core_namespace_record(NOVA_NAMESPACE_VOLUMES, &ns) != 0) return -1;
    if (ns->parent != NOVA_NAMESPACE_ROOT) return -1;
    if ((ns->flags & NOVA_NS_FLAG_USER_VISIBLE) == 0) return -1;
    if (semantic_core_namespace_record(NOVA_NAMESPACE_BOOT, &ns) != 0) return -1;
    if (ns->id_low != 6) return -1;
    if (semantic_core_namespace_record(NOVA_NAMESPACE_SOLUTIONS, &ns) != 0) return -1;
    if (ns->id_low != 7) return -1;
    if ((ns->flags & NOVA_NS_FLAG_USER_VISIBLE) == 0) return -1;

    if (semantic_core_projection_lookup_by_object(NOVA_ID_OBJECT_HIGH, 4, &proj) != 0) return -1;
    if (proj->ns_slot != NOVA_NAMESPACE_APPS) return -1;
    if (proj->target_low != 4) return -1;

    uint32_t proj_count;
    if (semantic_core_projection_count_by_namespace(NOVA_NAMESPACE_SYSTEM, &proj_count) != 0) return -1;
    if (proj_count != 1) return -1;
    if (semantic_core_projection_count_by_namespace(NOVA_NAMESPACE_ROOT, &proj_count) != 0) return -1;
    if (proj_count != 1) return -1;

    if (semantic_core_projection_at_namespace(NOVA_NAMESPACE_APPS, 0, 0, &proj) != 0) return -1;
    if (proj->object_low != 4) return -1;
    if (semantic_core_projection_at_namespace(NOVA_NAMESPACE_APPS, 0, 1, &proj) == 0) return -1;
    if (semantic_core_projection_at_namespace(NOVA_NAMESPACE_SOLUTIONS, 0, 0, &proj) != 0) return -1;
    if (proj->object_low != 7) return -1;
    if (semantic_core_projection_lookup_by_object(NOVA_ID_OBJECT_HIGH, 0x7000, &proj) == 0) return -1;

    semantic_core_capability_record_t *cap;
    if (semantic_core_capability_check_object_rights(NOVA_ID_OBJECT_HIGH, 2, NOVA_HANDLE_RIGHT_READ, &cap) != 0) return -1;
    if (semantic_core_capability_check_object_rights(NOVA_ID_OBJECT_HIGH, 2, NOVA_HANDLE_RIGHT_WRITE, &cap) == 0) return -1;

    if (semantic_core_capability_lookup(NOVA_ID_CAPABILITY_HIGH, 4, &cap) != 0) return -1;
    if (cap->ns_slot != NOVA_NAMESPACE_VOLUMES) return -1;
    if ((cap->rights & NOVA_HANDLE_RIGHT_READ) == 0) return -1;
    if (semantic_core_capability_check_object_rights(NOVA_ID_OBJECT_HIGH, 5, NOVA_HANDLE_RIGHT_READ, &cap) != 0) return -1;
    if (semantic_core_capability_check_object_rights(NOVA_ID_OBJECT_HIGH, 7, NOVA_HANDLE_RIGHT_EXECUTE, &cap) != 0) return -1;

    if (semantic_core_handle_open_by_object(NOVA_ID_OBJECT_HIGH, 5, NOVA_HANDLE_RIGHT_READ, &handle_id, &hdl) != 0) return -1;
    if (semantic_core_handle_validate_rights(handle_id, NOVA_HANDLE_RIGHT_READ, &hdl) != 0) return -1;

    if (semantic_core_capability_revoke(NOVA_ID_CAPABILITY_HIGH, 4) != 0) return -1;
    if (semantic_core_capability_lookup(NOVA_ID_CAPABILITY_HIGH, 4, &cap) == 0) return -1;
    if (semantic_core_capability_check_object_rights(NOVA_ID_OBJECT_HIGH, 5, NOVA_HANDLE_RIGHT_READ, &cap) == 0) return -1;
    if (semantic_core_handle_validate_rights(handle_id, NOVA_HANDLE_RIGHT_READ, &hdl) == 0) return -1;
    if (semantic_core_handle_close(handle_id) != 0) return -1;

    if (semantic_core_handle_open_by_object(NOVA_ID_OBJECT_HIGH, 2, NOVA_HANDLE_RIGHT_READ, &handle_id, &hdl) != 0) return -1;
    if (hdl->ns_slot != NOVA_NAMESPACE_SYSTEM) return -1;
    if ((hdl->rights & NOVA_HANDLE_RIGHT_READ) == 0) return -1;
    if (semantic_core_handle_lookup(handle_id, &hdl) != 0) return -1;
    if (hdl->object_low != 2) return -1;
    if (semantic_core_handle_close(handle_id) != 0) return -1;
    if (semantic_core_handle_lookup(handle_id, &hdl) == 0) return -1;

    return 0;
}
