/* NovaOS C-Kernel - §100 Kernel Object Graph (Port aus security32.inc). */
#include "kog.h"

typedef struct {
    uint32_t id;
    uint32_t type;
    uint32_t state;
    uint32_t refcount;
    uint32_t flags;
    int present;
} kog_object_t;

typedef struct {
    uint32_t id;
    uint32_t src_id;
    uint32_t dst_id;
    uint8_t type;
    uint8_t strength;
    uint8_t flags;
    int present;
    uint32_t generation;
} kog_relation_t;

typedef struct {
    uint32_t type;
    uint8_t max_strength;
    uint8_t flags;
    int present;
} kog_schema_t;

static kog_object_t kog_objects[KOG_MAX_OBJECTS];
static kog_relation_t kog_relations[KOG_MAX_RELATIONS];
static kog_schema_t kog_schemas[NP_RELATION_TYPE_COUNT];
static uint32_t kog_next_id = 0;
static uint32_t kog_next_rel_id = 0;
static uint32_t kog_graph_generation = 0;
static uint32_t kog_root_kernel_id = 0;
static uint32_t kog_root_machine_id = 0;
static uint32_t kog_root_security_id = 0;
static uint32_t kog_root_namespace_id = 0;
static uint32_t kog_root_device_id = 0;
static uint32_t kog_root_service_id = 0;
static uint32_t kog_root_recovery_id = 0;

static kog_object_t *kog_find_object(uint32_t obj_id) {
    for (uint32_t i = 0; i < KOG_MAX_OBJECTS; ++i) {
        if (kog_objects[i].present && kog_objects[i].id == obj_id) return &kog_objects[i];
    }
    return 0;
}

static kog_relation_t *kog_find_relation(uint32_t rel_id) {
    for (uint32_t i = 0; i < KOG_MAX_RELATIONS; ++i) {
        if (kog_relations[i].present && kog_relations[i].id == rel_id) return &kog_relations[i];
    }
    return 0;
}

static kog_schema_t *kog_find_schema(uint32_t rel_type) {
    for (uint32_t i = 0; i < NP_RELATION_TYPE_COUNT; ++i) {
        if (kog_schemas[i].present && kog_schemas[i].type == rel_type) return &kog_schemas[i];
    }
    return 0;
}

int kog_alloc_object(uint32_t type, uint32_t *out_obj_id) {
    for (uint32_t i = 0; i < KOG_MAX_OBJECTS; ++i) {
        if (!kog_objects[i].present) {
            uint32_t id = ++kog_next_id;
            kog_objects[i].id = id;
            kog_objects[i].type = type;
            kog_objects[i].state = NP_GRAPH_OBJ_INITIALIZING;
            kog_objects[i].refcount = 1;
            kog_objects[i].flags = 0;
            kog_objects[i].present = 1;
            *out_obj_id = id;
            return 0;
        }
    }
    return -1;
}

int kog_activate_object(uint32_t obj_id) {
    kog_object_t *obj = kog_find_object(obj_id);
    if (!obj) return -1;
    if (obj->state != NP_GRAPH_OBJ_INITIALIZING) return -1;
    obj->state = NP_GRAPH_OBJ_ACTIVE;
    return 0;
}

int kog_check_cycle(uint32_t src_id, uint32_t dst_id, uint32_t rel_type) {
    if (rel_type != NP_RELATION_OWNS && rel_type != NP_RELATION_CONTAINS &&
        rel_type != NP_RELATION_BOUND_TO) {
        return 0;
    }

    kog_schema_t *schema = kog_find_schema(rel_type);
    if (schema && (schema->flags & KOG_SCHEMA_ALLOW_CYCLE)) return 0;

    /* Iterative DFS von dst aus: ist src erreichbar ueber starke Kanten? */
    uint32_t stack[KOG_CYCLE_MAX_DEPTH];
    uint32_t depth = 1;
    stack[0] = dst_id;

    while (depth > 0) {
        if (depth >= KOG_CYCLE_MAX_DEPTH) break; /* restriktiv: bei Tiefe pass */
        --depth;
        uint32_t node = stack[depth];
        if (node == src_id) return -1; /* Zyklus */

        for (uint32_t i = 0; i < KOG_MAX_RELATIONS; ++i) {
            kog_relation_t *rel = &kog_relations[i];
            if (!rel->present || rel->src_id != node) continue;
            if (rel->strength < NP_RELSTR_STRONG) continue;
            if (depth >= KOG_CYCLE_MAX_DEPTH - 1) continue;
            stack[depth] = rel->dst_id;
            ++depth;
        }
    }
    return 0;
}

int kog_link(uint32_t src_id, uint32_t dst_id, uint32_t rel_type, uint32_t strength, uint32_t *out_rel_id) {
    kog_object_t *src = kog_find_object(src_id);
    if (!src || src->state == NP_GRAPH_OBJ_DESTROYED) return -1;

    kog_object_t *dst = kog_find_object(dst_id);
    if (!dst || dst->state == NP_GRAPH_OBJ_DESTROYED) return -1;

    if (!kog_find_schema(rel_type)) return -1;

    if (kog_check_cycle(src_id, dst_id, rel_type) != 0) return -1;

    kog_relation_t *slot = 0;
    for (uint32_t i = 0; i < KOG_MAX_RELATIONS; ++i) {
        if (!kog_relations[i].present) { slot = &kog_relations[i]; break; }
    }
    if (!slot) return -1;

    uint32_t id = ++kog_next_rel_id;
    slot->id = id;
    slot->src_id = src_id;
    slot->dst_id = dst_id;
    slot->type = (uint8_t)rel_type;
    slot->strength = (uint8_t)strength;
    slot->flags = 0;
    slot->present = 1;
    slot->generation = kog_graph_generation;
    ++kog_graph_generation;

    if (strength >= NP_RELSTR_STRONG) {
        ++dst->refcount;
    }

    *out_rel_id = id;
    return 0;
}

int kog_unlink(uint32_t rel_id) {
    kog_relation_t *rel = kog_find_relation(rel_id);
    if (!rel) return -1;

    if (rel->strength >= NP_RELSTR_STRONG) {
        kog_object_t *dst = kog_find_object(rel->dst_id);
        if (dst) --dst->refcount;
    }
    rel->present = 0;
    ++kog_graph_generation;
    return 0;
}

static void kog_schema_entry(uint32_t slot, uint32_t type, uint32_t max_strength, uint32_t flags) {
    kog_schemas[slot].type = type;
    kog_schemas[slot].max_strength = (uint8_t)max_strength;
    kog_schemas[slot].flags = (uint8_t)flags;
    kog_schemas[slot].present = 1;
}

static int kog_create_root(uint32_t type, uint32_t *out_id) {
    uint32_t id;
    if (kog_alloc_object(type, &id) != 0) return -1;
    if (kog_activate_object(id) != 0) return -1;
    *out_id = id;
    return 0;
}

void kog_initialize(void) {
    for (uint32_t i = 0; i < KOG_MAX_OBJECTS; ++i) kog_objects[i] = (kog_object_t){0};
    for (uint32_t i = 0; i < KOG_MAX_RELATIONS; ++i) kog_relations[i] = (kog_relation_t){0};
    for (uint32_t i = 0; i < NP_RELATION_TYPE_COUNT; ++i) kog_schemas[i] = (kog_schema_t){0};
    kog_next_id = 0;
    kog_next_rel_id = 0;
    kog_graph_generation = 1;

    kog_schema_entry(0, NP_RELATION_OWNS, NP_RELSTR_OWNERSHIP, 0);
    kog_schema_entry(1, NP_RELATION_CONTAINS, NP_RELSTR_STRONG, 0);
    kog_schema_entry(2, NP_RELATION_PARENT_OF, NP_RELSTR_REFERENCE, 0);
    kog_schema_entry(3, NP_RELATION_CHILD_OF, NP_RELSTR_REFERENCE, 0);
    kog_schema_entry(4, NP_RELATION_DEPENDS_ON, NP_RELSTR_REFERENCE, 0);
    kog_schema_entry(5, NP_RELATION_BOUND_TO, NP_RELSTR_STRONG, 0);
    kog_schema_entry(6, NP_RELATION_PROVIDES, NP_RELSTR_REFERENCE, KOG_SCHEMA_ALLOW_CYCLE);
    kog_schema_entry(7, NP_RELATION_CONSUMES, NP_RELSTR_REFERENCE, KOG_SCHEMA_ALLOW_CYCLE);
    kog_schema_entry(8, NP_RELATION_MEMBER_OF, NP_RELSTR_REFERENCE, KOG_SCHEMA_ALLOW_CYCLE);
    kog_schema_entry(9, NP_RELATION_MAPPED_TO, NP_RELSTR_REFERENCE, KOG_SCHEMA_ALLOW_CYCLE);
    kog_schema_entry(10, NP_RELATION_OBSERVES, NP_RELSTR_WEAK, KOG_SCHEMA_ALLOW_CYCLE);
    kog_schema_entry(11, NP_RELATION_SECURED_BY, NP_RELSTR_REFERENCE, 0);

    if (kog_create_root(NP_OBJTYPE_KERNEL, &kog_root_kernel_id) != 0) return;
    if (kog_create_root(NP_OBJTYPE_MACHINE, &kog_root_machine_id) != 0) return;
    if (kog_create_root(NP_OBJTYPE_SECURITY, &kog_root_security_id) != 0) return;
    if (kog_create_root(NP_OBJTYPE_NAMESPACE, &kog_root_namespace_id) != 0) return;
    if (kog_create_root(NP_OBJTYPE_DEVICE, &kog_root_device_id) != 0) return;
    if (kog_create_root(NP_OBJTYPE_GENERIC, &kog_root_service_id) != 0) return;
    if (kog_create_root(NP_OBJTYPE_GENERIC, &kog_root_recovery_id) != 0) return;

    uint32_t rel_id;
    kog_link(kog_root_kernel_id, kog_root_machine_id, NP_RELATION_OWNS, NP_RELSTR_OWNERSHIP, &rel_id);
}

int kog_self_test(void) {
    kog_schema_t *schema = kog_find_schema(NP_RELATION_OWNS);
    if (!schema || schema->max_strength != NP_RELSTR_OWNERSHIP) return -1;

    uint32_t obj_a, obj_b;
    if (kog_alloc_object(NP_OBJTYPE_GENERIC, &obj_a) != 0) return -1;
    if (kog_activate_object(obj_a) != 0) return -1;
    if (kog_alloc_object(NP_OBJTYPE_GENERIC, &obj_b) != 0) return -1;
    if (kog_activate_object(obj_b) != 0) return -1;

    uint32_t rel_id;
    if (kog_link(obj_a, obj_b, NP_RELATION_OWNS, NP_RELSTR_OWNERSHIP, &rel_id) != 0) return -1;

    if (kog_link(0xDEADBEEFu, obj_b, NP_RELATION_OWNS, NP_RELSTR_OWNERSHIP, &rel_id) == 0) return -1;

    kog_object_t *b = kog_find_object(obj_b);
    if (!b || b->refcount != 2) return -1; /* initial=1 + 1 von OWNS */

    kog_relation_t *rel = kog_find_relation(rel_id);
    if (!rel || rel->type != NP_RELATION_OWNS || rel->strength != NP_RELSTR_OWNERSHIP) return -1;

    if (!kog_find_schema(NP_RELATION_PARENT_OF)) return -1;
    if (!kog_find_schema(NP_RELATION_DEPENDS_ON)) return -1;
    if (!kog_find_schema(NP_RELATION_BOUND_TO)) return -1;
    if (!kog_find_schema(NP_RELATION_SECURED_BY)) return -1;

    if (kog_graph_generation <= 1) return -1;

    if (kog_unlink(rel_id) != 0) return -1;
    if (kog_find_relation(rel_id) != 0) return -1;

    b = kog_find_object(obj_b);
    if (!b || b->refcount != 1) return -1;

    if (kog_check_cycle(obj_b, obj_b, NP_RELATION_OWNS) == 0) return -1; /* muss Zyklus erkennen */

    kog_schema_t *observes = kog_find_schema(NP_RELATION_OBSERVES);
    if (!observes || (observes->flags & KOG_SCHEMA_ALLOW_CYCLE) == 0) return -1;

    kog_object_t *kernel_root = kog_find_object(kog_root_kernel_id);
    if (!kernel_root || kernel_root->state != NP_GRAPH_OBJ_ACTIVE) return -1;

    return 0;
}
