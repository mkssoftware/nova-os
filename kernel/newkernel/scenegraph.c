/* Dynamisch wachsende Scene-/Node-/Property-/Cache-Tabellen (2026-11-05,
   Antwort auf "viele feste Grenzen nicht dynamisch loesbar" - siehe
   [[project_general_allocator]], viertes umgestelltes Modul nach
   process.c/layercompose.c/gfxstate.c). SCENE_CAPACITY/
   SCENE_NODE_CAPACITY/SCENE_PROPERTY_DEF_CAPACITY/SCENE_PROPERTY_CAPACITY/
   SCENE_CACHE_CAPACITY (scenegraph.h) sind jetzt nur noch
   Startkapazitaeten - genau die Tabelle (`scenes[]`, urspruenglich
   SCENE_CAPACITY=4), deren Erschoepfung in einer fruehen Session dieses
   Vorhabens real einen Boot-Test scheitern liess (manuell auf 16
   angehoben, siehe [[project_gui_graphics_engine]] Debugging-Lektion),
   kann jetzt gar nicht mehr erschoepfen.

   BEWUSST UNVERAENDERT: `transactions[]` (SCENE_TX_CAPACITY=8 - ein
   Szenen-Transaktion-Slot wird spaetestens bei Commit/Abort SOFORT
   wieder frei, kein unbegrenzt waechsendes Register wie bei gfxstate.c's
   Dependency-Kanten), `SCENE_NODE_MAX_CHILDREN`/`SCENE_TX_MAX_OPS`
   (In-Struct-Arrays - ein Kind-Limit pro Node bzw. ein Operations-Limit
   pro Transaktion sind echte, bewusst bemessene Praxisgrenzen, keine
   global waechsende Sammlung; eine dynamische Umstellung braeuchte pro
   Node/Transaktion eine eigene Heap-Allokation - eigene, groessere
   Erweiterung, nicht Teil dieser Aenderung). */
#include "scenegraph.h"
#include "gfxstate.h"
#include "kalloc.h"
#include <stddef.h>

#define SCENE_OP_PROPERTY 1u
#define SCENE_OP_REMOVE   2u
#define SCENE_OP_REPARENT 3u

typedef struct {
    uint32_t scene_id;
    uint32_t root_node_id;
    uint32_t state_version;
    int in_use;
} scene_t;

typedef struct {
    uint32_t node_id;
    uint32_t scene_id;
    uint32_t node_type;
    uint32_t parent_id;
    uint32_t children[SCENE_NODE_MAX_CHILDREN];
    uint32_t child_count;
    uint32_t state_version;
    uint32_t lifecycle_state;
    int in_use;
} scene_node_t;

typedef struct {
    uint32_t property_id;
    uint32_t property_type;
    uint32_t default_value;
    uint32_t flags;
    int in_use;
} scene_property_def_t;

typedef struct {
    uint32_t node_id;
    uint32_t property_id;
    uint32_t value;
    int in_use;
} scene_property_value_t;

typedef struct {
    uint32_t cache_id;
    uint32_t owner_node_id;
    uint32_t result_type;
    uint32_t state_version;
    uint32_t cached_value;
    int in_use;
} scene_cache_entry_t;

typedef struct {
    uint32_t op_type;
    uint32_t node_id;
    uint32_t a; /* property_id (PROPERTY) oder new_parent_id (REPARENT) */
    uint32_t b; /* value (PROPERTY) */
} scene_pending_op_t;

typedef struct {
    uint32_t gfx_tx_id;
    uint32_t scene_id;
    uint32_t touched_nodes[SCENE_TX_MAX_OPS];
    uint32_t touched_count;
    scene_pending_op_t ops[SCENE_TX_MAX_OPS];
    uint32_t op_count;
    int in_use;
} scene_tx_t;

static scene_t *scenes = NULL;
static uint32_t scenes_capacity = 0;
static scene_node_t *nodes = NULL;
static uint32_t nodes_capacity = 0;
static scene_property_def_t *property_defs = NULL;
static uint32_t property_defs_capacity = 0;
static scene_property_value_t *property_values = NULL;
static uint32_t property_values_capacity = 0;
static scene_cache_entry_t *caches = NULL;
static uint32_t caches_capacity = 0;
static scene_tx_t transactions[SCENE_TX_CAPACITY];

static uint32_t scene_next_id = 1;
static uint32_t node_next_id = 1;
static uint32_t cache_next_id = 1;
static int scenegraph_ready = 0;

/* Gemeinsames Verdoppelungs-Muster (identisch zu process.c/
   layercompose.c/gfxstate.c) - je eine kleine, typisierte Hilfsfunktion
   statt eines generischen void*-Allokators: bleibt dadurch ohne
   Funktionszeiger/memcpy-Tricks typsicher und einfach lesbar. */
#define SCENEGRAPH_DEFINE_GROWER(fn_name, table, capacity_var, element_t, initial_capacity) \
    static int fn_name(uint32_t *out_slot) { \
        uint32_t new_capacity = (capacity_var == 0u) ? (initial_capacity) : capacity_var * 2u; \
        element_t *grown = (element_t *)krealloc(table, new_capacity * (uint32_t)sizeof(element_t)); \
        if (!grown) return -1; \
        for (uint32_t i = capacity_var; i < new_capacity; ++i) grown[i] = (element_t){0}; \
        *out_slot = capacity_var; \
        table = grown; \
        capacity_var = new_capacity; \
        return 0; \
    }

SCENEGRAPH_DEFINE_GROWER(scenes_grow, scenes, scenes_capacity, scene_t, SCENE_CAPACITY)
SCENEGRAPH_DEFINE_GROWER(nodes_grow, nodes, nodes_capacity, scene_node_t, SCENE_NODE_CAPACITY)
SCENEGRAPH_DEFINE_GROWER(property_defs_grow, property_defs, property_defs_capacity,
                          scene_property_def_t, SCENE_PROPERTY_DEF_CAPACITY)
SCENEGRAPH_DEFINE_GROWER(property_values_grow, property_values, property_values_capacity,
                          scene_property_value_t, SCENE_PROPERTY_CAPACITY)
SCENEGRAPH_DEFINE_GROWER(caches_grow, caches, caches_capacity, scene_cache_entry_t, SCENE_CACHE_CAPACITY)

static void *scenegraph_alloc_table(uint32_t count, uint32_t elem_size, uint32_t *out_capacity) {
    void *table = kcalloc(count, elem_size);
    *out_capacity = table ? count : 0u;
    return table;
}

void scenegraph_initialize(void) {
    if (scenes) kfree(scenes);
    scenes = (scene_t *)scenegraph_alloc_table(SCENE_CAPACITY, sizeof(scene_t), &scenes_capacity);

    if (nodes) kfree(nodes);
    nodes = (scene_node_t *)scenegraph_alloc_table(SCENE_NODE_CAPACITY, sizeof(scene_node_t), &nodes_capacity);

    if (property_defs) kfree(property_defs);
    property_defs = (scene_property_def_t *)scenegraph_alloc_table(
        SCENE_PROPERTY_DEF_CAPACITY, sizeof(scene_property_def_t), &property_defs_capacity);

    if (property_values) kfree(property_values);
    property_values = (scene_property_value_t *)scenegraph_alloc_table(
        SCENE_PROPERTY_CAPACITY, sizeof(scene_property_value_t), &property_values_capacity);

    if (caches) kfree(caches);
    caches = (scene_cache_entry_t *)scenegraph_alloc_table(
        SCENE_CACHE_CAPACITY, sizeof(scene_cache_entry_t), &caches_capacity);

    for (uint32_t i = 0; i < SCENE_TX_CAPACITY; ++i) transactions[i] = (scene_tx_t){0};
    scene_next_id = 1;
    node_next_id = 1;
    cache_next_id = 1;
    scenegraph_ready = 1;
}

static scene_t *scene_find(uint32_t scene_id) {
    if (scene_id == 0) return NULL;
    for (uint32_t i = 0; i < scenes_capacity; ++i) {
        if (scenes[i].in_use && scenes[i].scene_id == scene_id) return &scenes[i];
    }
    return NULL;
}

static scene_node_t *node_find(uint32_t node_id) {
    if (node_id == 0) return NULL;
    for (uint32_t i = 0; i < nodes_capacity; ++i) {
        if (nodes[i].in_use && nodes[i].node_id == node_id) return &nodes[i];
    }
    return NULL;
}

static scene_property_def_t *propdef_find(uint32_t property_id) {
    for (uint32_t i = 0; i < property_defs_capacity; ++i) {
        if (property_defs[i].in_use && property_defs[i].property_id == property_id) return &property_defs[i];
    }
    return NULL;
}

static scene_property_value_t *propval_find(uint32_t node_id, uint32_t property_id) {
    for (uint32_t i = 0; i < property_values_capacity; ++i) {
        if (property_values[i].in_use && property_values[i].node_id == node_id &&
            property_values[i].property_id == property_id) {
            return &property_values[i];
        }
    }
    return NULL;
}

static int propval_write(uint32_t node_id, uint32_t property_id, uint32_t value) {
    scene_property_value_t *v = propval_find(node_id, property_id);
    if (v) {
        v->value = value;
        return 0;
    }
    for (uint32_t i = 0; i < property_values_capacity; ++i) {
        if (!property_values[i].in_use) {
            property_values[i].node_id = node_id;
            property_values[i].property_id = property_id;
            property_values[i].value = value;
            property_values[i].in_use = 1;
            return 0;
        }
    }
    uint32_t slot;
    if (property_values_grow(&slot) != 0) return SCENE_ERR_CAPACITY;
    property_values[slot].node_id = node_id;
    property_values[slot].property_id = property_id;
    property_values[slot].value = value;
    property_values[slot].in_use = 1;
    return 0;
}

static scene_tx_t *tx_find(uint32_t gfx_tx_id) {
    for (uint32_t i = 0; i < SCENE_TX_CAPACITY; ++i) {
        if (transactions[i].in_use && transactions[i].gfx_tx_id == gfx_tx_id) return &transactions[i];
    }
    return NULL;
}

static void tx_touch(scene_tx_t *tx, uint32_t node_id) {
    for (uint32_t i = 0; i < tx->touched_count; ++i) {
        if (tx->touched_nodes[i] == node_id) return;
    }
    if (tx->touched_count < SCENE_TX_MAX_OPS) {
        tx->touched_nodes[tx->touched_count++] = node_id;
    }
}

static int node_add_child(scene_node_t *parent, uint32_t child_id) {
    if (parent->child_count >= SCENE_NODE_MAX_CHILDREN) return SCENE_ERR_CAPACITY;
    parent->children[parent->child_count++] = child_id;
    return 0;
}

static void node_remove_child(scene_node_t *parent, uint32_t child_id) {
    for (uint32_t i = 0; i < parent->child_count; ++i) {
        if (parent->children[i] == child_id) {
            for (uint32_t j = i + 1; j < parent->child_count; ++j) {
                parent->children[j - 1] = parent->children[j];
            }
            parent->child_count--;
            return;
        }
    }
}

int scene_create(uint32_t *out_scene_id, uint32_t *out_root_node_id) {
    uint32_t sslot;
    for (sslot = 0; sslot < scenes_capacity; ++sslot) {
        if (!scenes[sslot].in_use) break;
    }
    if (sslot >= scenes_capacity) {
        if (scenes_grow(&sslot) != 0) return SCENE_ERR_CAPACITY;
    }

    uint32_t nslot;
    for (nslot = 0; nslot < nodes_capacity; ++nslot) {
        if (!nodes[nslot].in_use) break;
    }
    if (nslot >= nodes_capacity) {
        if (nodes_grow(&nslot) != 0) return SCENE_ERR_CAPACITY;
    }

    uint32_t scene_id = scene_next_id++;
    uint32_t root_id = node_next_id++;

    nodes[nslot] = (scene_node_t){0};
    nodes[nslot].node_id = root_id;
    nodes[nslot].scene_id = scene_id;
    nodes[nslot].node_type = SCENE_NODE_TYPE_GROUP;
    nodes[nslot].parent_id = 0;
    nodes[nslot].state_version = 1;
    nodes[nslot].lifecycle_state = SCENE_NODE_ATTACHED;
    nodes[nslot].in_use = 1;

    scenes[sslot] = (scene_t){0};
    scenes[sslot].scene_id = scene_id;
    scenes[sslot].root_node_id = root_id;
    scenes[sslot].state_version = 1;
    scenes[sslot].in_use = 1;

    *out_scene_id = scene_id;
    *out_root_node_id = root_id;
    return 0;
}

int scene_property_define(uint32_t property_id, uint32_t property_type,
                           uint32_t default_value, uint32_t flags) {
    scene_property_def_t *def = propdef_find(property_id);
    if (def) {
        def->property_type = property_type;
        def->default_value = default_value;
        def->flags = flags;
        return 0;
    }
    for (uint32_t i = 0; i < property_defs_capacity; ++i) {
        if (!property_defs[i].in_use) {
            property_defs[i].property_id = property_id;
            property_defs[i].property_type = property_type;
            property_defs[i].default_value = default_value;
            property_defs[i].flags = flags;
            property_defs[i].in_use = 1;
            return 0;
        }
    }
    uint32_t slot;
    if (property_defs_grow(&slot) != 0) return SCENE_ERR_CAPACITY;
    property_defs[slot].property_id = property_id;
    property_defs[slot].property_type = property_type;
    property_defs[slot].default_value = default_value;
    property_defs[slot].flags = flags;
    property_defs[slot].in_use = 1;
    return 0;
}

int scene_transaction_begin(uint32_t scene_id, uint32_t *out_tx_id) {
    if (!scene_find(scene_id)) return SCENE_ERR_NOT_FOUND;

    uint32_t slot;
    for (slot = 0; slot < SCENE_TX_CAPACITY; ++slot) {
        if (!transactions[slot].in_use) break;
    }
    if (slot >= SCENE_TX_CAPACITY) return SCENE_ERR_CAPACITY;

    uint32_t gfx_tx_id;
    int rc = gfx_transaction_begin(&gfx_tx_id);
    if (rc != 0) return rc;

    transactions[slot] = (scene_tx_t){0};
    transactions[slot].gfx_tx_id = gfx_tx_id;
    transactions[slot].scene_id = scene_id;
    transactions[slot].in_use = 1;

    *out_tx_id = gfx_tx_id;
    return 0;
}

int scene_node_create(uint32_t tx_id, uint32_t parent_id, uint32_t node_type,
                       uint32_t *out_node_id) {
    scene_tx_t *tx = tx_find(tx_id);
    if (!tx) return SCENE_ERR_NOT_FOUND;

    scene_node_t *parent = node_find(parent_id);
    if (!parent || parent->scene_id != tx->scene_id) return SCENE_ERR_NOT_FOUND;
    if (parent->lifecycle_state != SCENE_NODE_ATTACHED && parent->lifecycle_state != SCENE_NODE_CREATED) {
        return SCENE_ERR_INVALID_STATE;
    }

    uint32_t slot;
    for (slot = 0; slot < nodes_capacity; ++slot) {
        if (!nodes[slot].in_use) break;
    }
    if (slot >= nodes_capacity) {
        if (nodes_grow(&slot) != 0) return SCENE_ERR_CAPACITY;
    }

    uint32_t node_id = node_next_id++;
    nodes[slot] = (scene_node_t){0};
    nodes[slot].node_id = node_id;
    nodes[slot].scene_id = tx->scene_id;
    nodes[slot].node_type = node_type;
    nodes[slot].parent_id = parent_id;
    nodes[slot].lifecycle_state = SCENE_NODE_CREATED; /* noch nicht veroeffentlicht */
    nodes[slot].in_use = 1;

    gfx_transaction_record_change(tx->gfx_tx_id, node_id, SCENE_CHANGE_NODE_CREATED, 0, 0, 1);
    tx_touch(tx, node_id);
    tx_touch(tx, parent_id); /* children[] des Parents aendert sich beim Attach */

    *out_node_id = node_id;
    return 0;
}

int scene_node_remove(uint32_t tx_id, uint32_t node_id) {
    scene_tx_t *tx = tx_find(tx_id);
    if (!tx) return SCENE_ERR_NOT_FOUND;
    scene_node_t *node = node_find(node_id);
    if (!node || node->scene_id != tx->scene_id) return SCENE_ERR_NOT_FOUND;
    if (node->lifecycle_state != SCENE_NODE_ATTACHED) return SCENE_ERR_INVALID_STATE;
    if (tx->op_count >= SCENE_TX_MAX_OPS) return SCENE_ERR_CAPACITY;

    tx->ops[tx->op_count].op_type = SCENE_OP_REMOVE;
    tx->ops[tx->op_count].node_id = node_id;
    tx->op_count++;

    gfx_transaction_record_change(tx->gfx_tx_id, node_id, SCENE_CHANGE_NODE_REMOVED, 0, node->state_version, 0);
    tx_touch(tx, node_id);
    tx_touch(tx, node->parent_id); /* children[] des Parents aendert sich beim Detach */
    return 0;
}

int scene_node_reparent(uint32_t tx_id, uint32_t node_id, uint32_t new_parent_id) {
    scene_tx_t *tx = tx_find(tx_id);
    if (!tx) return SCENE_ERR_NOT_FOUND;
    scene_node_t *node = node_find(node_id);
    if (!node || node->scene_id != tx->scene_id) return SCENE_ERR_NOT_FOUND;
    if (node->lifecycle_state != SCENE_NODE_ATTACHED) return SCENE_ERR_INVALID_STATE;
    scene_node_t *new_parent = node_find(new_parent_id);
    if (!new_parent || new_parent->scene_id != tx->scene_id) return SCENE_ERR_NOT_FOUND;
    if (tx->op_count >= SCENE_TX_MAX_OPS) return SCENE_ERR_CAPACITY;

    tx->ops[tx->op_count].op_type = SCENE_OP_REPARENT;
    tx->ops[tx->op_count].node_id = node_id;
    tx->ops[tx->op_count].a = new_parent_id;
    tx->op_count++;

    gfx_transaction_record_change(tx->gfx_tx_id, node_id, SCENE_CHANGE_NODE_REPARENTED, 0,
                                   node->parent_id, new_parent_id);
    tx_touch(tx, node_id);
    tx_touch(tx, node->parent_id); /* alter Parent verliert ein Kind */
    tx_touch(tx, new_parent_id);   /* neuer Parent erhaelt ein Kind */
    return 0;
}

int scene_node_set_property(uint32_t tx_id, uint32_t node_id, uint32_t property_id,
                             uint32_t value) {
    scene_tx_t *tx = tx_find(tx_id);
    if (!tx) return SCENE_ERR_NOT_FOUND;
    scene_node_t *node = node_find(node_id);
    if (!node || node->scene_id != tx->scene_id) return SCENE_ERR_NOT_FOUND;
    if (node->lifecycle_state != SCENE_NODE_CREATED && node->lifecycle_state != SCENE_NODE_ATTACHED) {
        return SCENE_ERR_INVALID_STATE;
    }

    if (node->lifecycle_state == SCENE_NODE_CREATED) {
        /* Noch nicht veroeffentlicht - fuer niemand anderen sichtbar, darf
           sofort geschrieben werden (siehe Dateikopf). */
        int rc = propval_write(node_id, property_id, value);
        if (rc != 0) return rc;
    } else {
        /* Bereits ATTACHED - Aenderung wird erst bei Commit sichtbar. */
        if (tx->op_count >= SCENE_TX_MAX_OPS) return SCENE_ERR_CAPACITY;
        tx->ops[tx->op_count].op_type = SCENE_OP_PROPERTY;
        tx->ops[tx->op_count].node_id = node_id;
        tx->ops[tx->op_count].a = property_id;
        tx->ops[tx->op_count].b = value;
        tx->op_count++;
    }

    gfx_transaction_record_change(tx->gfx_tx_id, node_id, SCENE_CHANGE_PROPERTY_CHANGED, property_id,
                                   0, node->state_version + 1);
    tx_touch(tx, node_id);
    return 0;
}

int scene_node_get_property(uint32_t node_id, uint32_t property_id, uint32_t *out_value) {
    scene_node_t *node = node_find(node_id);
    if (!node) return SCENE_ERR_NOT_FOUND;

    scene_property_value_t *v = propval_find(node_id, property_id);
    if (v) {
        *out_value = v->value;
        return 0;
    }
    scene_property_def_t *def = propdef_find(property_id);
    *out_value = def ? def->default_value : 0;
    return 0;
}

int scene_node_info(uint32_t node_id, scene_node_info_t *out_info) {
    scene_node_t *node = node_find(node_id);
    if (!node) return SCENE_ERR_NOT_FOUND;
    out_info->node_id = node->node_id;
    out_info->scene_id = node->scene_id;
    out_info->node_type = node->node_type;
    out_info->parent_id = node->parent_id;
    out_info->child_count = node->child_count;
    out_info->state_version = node->state_version;
    out_info->lifecycle_state = node->lifecycle_state;
    return 0;
}

int scene_node_release(uint32_t node_id) {
    scene_node_t *node = node_find(node_id);
    if (!node) return SCENE_ERR_NOT_FOUND;
    if (node->lifecycle_state != SCENE_NODE_DETACHED) return SCENE_ERR_INVALID_STATE;

    for (uint32_t i = 0; i < property_values_capacity; ++i) {
        if (property_values[i].in_use && property_values[i].node_id == node_id) {
            property_values[i] = (scene_property_value_t){0};
        }
    }
    *node = (scene_node_t){0};
    return 0;
}

int scene_node_add_dependency(uint32_t source_node_id, uint32_t target_node_id,
                               uint32_t dep_type, uint32_t *out_edge_id) {
    if (!node_find(source_node_id) || !node_find(target_node_id)) {
        return SCENE_ERR_NOT_FOUND;
    }
    return gfx_dependency_add(source_node_id, target_node_id, dep_type, out_edge_id);
}

int scene_cache_put(uint32_t owner_node_id, uint32_t result_type, uint32_t value,
                     uint32_t *out_cache_id) {
    scene_node_t *node = node_find(owner_node_id);
    if (!node) return SCENE_ERR_NOT_FOUND;

    uint32_t slot;
    for (slot = 0; slot < caches_capacity; ++slot) {
        if (!caches[slot].in_use) break;
    }
    if (slot >= caches_capacity) {
        if (caches_grow(&slot) != 0) return SCENE_ERR_CAPACITY;
    }

    caches[slot].cache_id = cache_next_id++;
    caches[slot].owner_node_id = owner_node_id;
    caches[slot].result_type = result_type;
    caches[slot].state_version = node->state_version;
    caches[slot].cached_value = value;
    caches[slot].in_use = 1;

    *out_cache_id = caches[slot].cache_id;
    return 0;
}

int scene_cache_get(uint32_t cache_id, scene_cache_info_t *out_info) {
    scene_cache_entry_t *c = NULL;
    for (uint32_t i = 0; i < caches_capacity; ++i) {
        if (caches[i].in_use && caches[i].cache_id == cache_id) {
            c = &caches[i];
            break;
        }
    }
    if (!c) return SCENE_ERR_NOT_FOUND;

    scene_node_t *owner = node_find(c->owner_node_id);
    uint32_t validity;
    if (!owner || owner->lifecycle_state == SCENE_NODE_RELEASED) {
        validity = SCENE_CACHE_INVALID;
    } else if (owner->state_version != c->state_version) {
        validity = SCENE_CACHE_INVALID; /* lazy Versions-Abgleich, siehe Dateikopf */
    } else {
        validity = SCENE_CACHE_VALID;
    }

    out_info->cache_id = c->cache_id;
    out_info->owner_node_id = c->owner_node_id;
    out_info->result_type = c->result_type;
    out_info->state_version = c->state_version;
    out_info->cached_value = c->cached_value;
    out_info->validity_state = validity;
    return 0;
}

int scene_transaction_commit(uint32_t tx_id, uint32_t *out_state_version) {
    scene_tx_t *tx = tx_find(tx_id);
    if (!tx) return SCENE_ERR_NOT_FOUND;

    /* Pending Ops anwenden (Property-Writes auf ATTACHED Nodes, Remove,
       Reparent) - erst JETZT wird fuer andere sichtbar mutiert. */
    for (uint32_t i = 0; i < tx->op_count; ++i) {
        scene_pending_op_t *op = &tx->ops[i];
        scene_node_t *node = node_find(op->node_id);
        if (!node) continue; /* kann bei Fehlbedienung nicht vorkommen, defensiv */

        if (op->op_type == SCENE_OP_PROPERTY) {
            propval_write(op->node_id, op->a, op->b);
        } else if (op->op_type == SCENE_OP_REMOVE) {
            scene_node_t *parent = node_find(node->parent_id);
            if (parent) node_remove_child(parent, op->node_id);
            node->lifecycle_state = SCENE_NODE_DETACHED;
        } else if (op->op_type == SCENE_OP_REPARENT) {
            scene_node_t *old_parent = node_find(node->parent_id);
            scene_node_t *new_parent = node_find(op->a);
            if (old_parent) node_remove_child(old_parent, op->node_id);
            if (new_parent) node_add_child(new_parent, op->node_id);
            node->parent_id = op->a;
        }
    }

    /* Neue (noch CREATED) Nodes in Erzeugungsreihenfolge an ihren Parent
       anhaengen - ein Parent, der in DERSELBEN Transaktion erzeugt wurde,
       ist an dieser Stelle bereits ATTACHED, da touched_nodes die
       Erzeugungsreihenfolge erhaelt (Parent wird immer vor seinem Child
       erzeugt). */
    for (uint32_t i = 0; i < tx->touched_count; ++i) {
        scene_node_t *node = node_find(tx->touched_nodes[i]);
        if (node && node->lifecycle_state == SCENE_NODE_CREATED) {
            scene_node_t *parent = node_find(node->parent_id);
            if (parent) node_add_child(parent, node->node_id);
            node->lifecycle_state = SCENE_NODE_ATTACHED;
        }
    }

    uint32_t version;
    int rc = gfx_transaction_commit(tx->gfx_tx_id, &version);
    if (rc != 0) return rc; /* kann bei korrekter Nutzung nicht scheitern */

    scene_t *scene = scene_find(tx->scene_id);
    if (scene) scene->state_version = version;
    for (uint32_t i = 0; i < tx->touched_count; ++i) {
        scene_node_t *node = node_find(tx->touched_nodes[i]);
        if (node) node->state_version = version;
    }

    *out_state_version = version;
    tx->in_use = 0;
    return 0;
}

int scene_transaction_abort(uint32_t tx_id) {
    scene_tx_t *tx = tx_find(tx_id);
    if (!tx) return SCENE_ERR_NOT_FOUND;

    /* Nie veroeffentlichte (noch CREATED) Nodes komplett verwerfen - sie
       waren fuer niemanden ausser dieser Transaktion sichtbar. Pending Ops
       (Property/Remove/Reparent auf ATTACHED Nodes) wurden nie angewandt,
       muessen also nicht zurueckgerollt werden (siehe Dateikopf). */
    for (uint32_t i = 0; i < tx->touched_count; ++i) {
        scene_node_t *node = node_find(tx->touched_nodes[i]);
        if (node && node->lifecycle_state == SCENE_NODE_CREATED) {
            for (uint32_t p = 0; p < property_values_capacity; ++p) {
                if (property_values[p].in_use && property_values[p].node_id == node->node_id) {
                    property_values[p] = (scene_property_value_t){0};
                }
            }
            *node = (scene_node_t){0};
        }
    }

    int rc = gfx_transaction_abort(tx->gfx_tx_id);
    tx->in_use = 0;
    return rc;
}

/* ----------------------------------------------------------------------- */
/* Selbsttest                                                              */
/* ----------------------------------------------------------------------- */

int scenegraph_self_test(void) {
    if (!scenegraph_ready) return -1;

    const uint32_t PROP_X = 1u;

    uint32_t scene1, root1;
    if (scene_create(&scene1, &root1) != 0) return -1;

    scene_node_info_t ninfo;
    if (scene_node_info(root1, &ninfo) != 0) return -1;
    if (ninfo.lifecycle_state != SCENE_NODE_ATTACHED || ninfo.node_type != SCENE_NODE_TYPE_GROUP) return -1;
    if (ninfo.parent_id != 0 || ninfo.child_count != 0) return -1;

    if (scene_property_define(PROP_X, SCENE_PROP_TYPE_INT, 0, SCENE_PROP_FLAG_AFFECTS_RENDER) != 0) return -1;

    /* Node-Erzeugung: unsichtbar/CREATED bis zum Commit. */
    uint32_t tx1;
    if (scene_transaction_begin(scene1, &tx1) != 0) return -1;
    uint32_t nodeA;
    if (scene_node_create(tx1, root1, SCENE_NODE_TYPE_GEOMETRY, &nodeA) != 0) return -1;
    if (scene_node_info(nodeA, &ninfo) != 0) return -1;
    if (ninfo.lifecycle_state != SCENE_NODE_CREATED) return -1;
    if (scene_node_info(root1, &ninfo) != 0) return -1;
    if (ninfo.child_count != 0) return -1; /* noch nicht angehaengt */

    /* Property auf CREATED Node: wirkt sofort (niemand sonst sieht ihn). */
    if (scene_node_set_property(tx1, nodeA, PROP_X, 42) != 0) return -1;
    uint32_t val;
    if (scene_node_get_property(nodeA, PROP_X, &val) != 0) return -1;
    if (val != 42) return -1;

    uint32_t v1;
    if (scene_transaction_commit(tx1, &v1) != 0) return -1;
    if (scene_node_info(root1, &ninfo) != 0) return -1;
    if (ninfo.child_count != 1 || ninfo.state_version != v1) return -1;
    if (scene_node_info(nodeA, &ninfo) != 0) return -1;
    if (ninfo.lifecycle_state != SCENE_NODE_ATTACHED || ninfo.state_version != v1) return -1;

    /* Property auf ATTACHED Node: Aenderung erst NACH Commit sichtbar. */
    uint32_t tx2;
    if (scene_transaction_begin(scene1, &tx2) != 0) return -1;
    if (scene_node_set_property(tx2, nodeA, PROP_X, 99) != 0) return -1;
    if (scene_node_get_property(nodeA, PROP_X, &val) != 0) return -1;
    if (val != 42) return -1; /* alter Wert, Transaktion noch offen */

    uint32_t v2;
    if (scene_transaction_commit(tx2, &v2) != 0) return -1;
    if (scene_node_get_property(nodeA, PROP_X, &val) != 0) return -1;
    if (val != 99) return -1;
    if (v2 <= v1) return -1;

    /* Abort: weder Property-Aenderung noch neuer Node duerfen ueberleben. */
    uint32_t tx3;
    if (scene_transaction_begin(scene1, &tx3) != 0) return -1;
    if (scene_node_set_property(tx3, nodeA, PROP_X, 777) != 0) return -1;
    uint32_t nodeB;
    if (scene_node_create(tx3, root1, SCENE_NODE_TYPE_GROUP, &nodeB) != 0) return -1;
    if (scene_transaction_abort(tx3) != 0) return -1;

    if (scene_node_get_property(nodeA, PROP_X, &val) != 0) return -1;
    if (val != 99) return -1; /* unveraendert */
    if (scene_node_info(nodeB, &ninfo) != SCENE_ERR_NOT_FOUND) return -1; /* vollstaendig verworfen */
    if (scene_node_info(root1, &ninfo) != 0) return -1;
    if (ninfo.child_count != 1) return -1; /* unveraendert */

    /* Remove: Wirkung erst nach Commit. */
    uint32_t tx4;
    if (scene_transaction_begin(scene1, &tx4) != 0) return -1;
    uint32_t nodeC;
    if (scene_node_create(tx4, root1, SCENE_NODE_TYPE_TEXT, &nodeC) != 0) return -1;
    if (scene_transaction_commit(tx4, &v2) != 0) return -1;
    if (scene_node_info(root1, &ninfo) != 0) return -1;
    if (ninfo.child_count != 2) return -1;

    uint32_t tx5;
    if (scene_transaction_begin(scene1, &tx5) != 0) return -1;
    if (scene_node_remove(tx5, nodeC) != 0) return -1;
    if (scene_node_info(nodeC, &ninfo) != 0) return -1;
    if (ninfo.lifecycle_state != SCENE_NODE_ATTACHED) return -1; /* noch nicht wirksam */
    uint32_t v5;
    if (scene_transaction_commit(tx5, &v5) != 0) return -1;
    if (scene_node_info(nodeC, &ninfo) != 0) return -1;
    if (ninfo.lifecycle_state != SCENE_NODE_DETACHED) return -1;
    if (scene_node_info(root1, &ninfo) != 0) return -1;
    if (ninfo.child_count != 1) return -1;

    if (scene_node_release(nodeC) != 0) return -1;
    if (scene_node_info(nodeC, &ninfo) != SCENE_ERR_NOT_FOUND) return -1;
    if (scene_node_release(nodeC) != SCENE_ERR_NOT_FOUND) return -1;

    /* Reparent. */
    uint32_t tx6;
    if (scene_transaction_begin(scene1, &tx6) != 0) return -1;
    uint32_t nodeD, nodeE;
    if (scene_node_create(tx6, root1, SCENE_NODE_TYPE_GEOMETRY, &nodeD) != 0) return -1;
    if (scene_node_create(tx6, root1, SCENE_NODE_TYPE_GROUP, &nodeE) != 0) return -1;
    if (scene_transaction_commit(tx6, &v5) != 0) return -1;

    uint32_t tx7;
    if (scene_transaction_begin(scene1, &tx7) != 0) return -1;
    if (scene_node_reparent(tx7, nodeD, nodeE) != 0) return -1;
    if (scene_transaction_commit(tx7, &v5) != 0) return -1;
    if (scene_node_info(nodeD, &ninfo) != 0) return -1;
    if (ninfo.parent_id != nodeE) return -1;
    if (scene_node_info(nodeE, &ninfo) != 0) return -1;
    if (ninfo.child_count != 1) return -1;

    /* Dependency-Integration: kostenlos aus Phase 1 geerbte Propagation. */
    uint32_t edge_ae;
    if (scene_node_add_dependency(nodeA, nodeE, GFX_DEP_MATERIAL, &edge_ae) != 0) return -1;
    if (scene_node_add_dependency(nodeE, nodeA, GFX_DEP_MATERIAL, &edge_ae) != SCENE_ERR_CYCLE) return -1;

    gfx_dependency_info_t depinfo;
    if (gfx_dependency_info(edge_ae, &depinfo) != 0) return -1;
    uint32_t invalidations_before = depinfo.invalidation_total;

    uint32_t tx8;
    if (scene_transaction_begin(scene1, &tx8) != 0) return -1;
    if (scene_node_set_property(tx8, nodeA, PROP_X, 5) != 0) return -1;
    if (scene_transaction_commit(tx8, &v5) != 0) return -1;

    if (gfx_dependency_info(edge_ae, &depinfo) != 0) return -1;
    if (depinfo.invalidation_total != invalidations_before + 1) return -1;

    /* Unbekannte Nodes bei add_dependency. */
    uint32_t edge_dummy;
    if (scene_node_add_dependency(0xFFFFu, nodeA, GFX_DEP_PROPERTY, &edge_dummy) != SCENE_ERR_NOT_FOUND) return -1;

    /* Cache: lazy Versions-Abgleich gegen den Owner-Node. */
    uint32_t cache1;
    if (scene_cache_put(nodeA, SCENE_CACHE_BOUNDS, 1234, &cache1) != 0) return -1;
    scene_cache_info_t cinfo;
    if (scene_cache_get(cache1, &cinfo) != 0) return -1;
    if (cinfo.validity_state != SCENE_CACHE_VALID || cinfo.cached_value != 1234) return -1;

    uint32_t tx9;
    if (scene_transaction_begin(scene1, &tx9) != 0) return -1;
    if (scene_node_set_property(tx9, nodeA, PROP_X, 6) != 0) return -1; /* bumpt nodeA's state_version */
    if (scene_transaction_commit(tx9, &v5) != 0) return -1;

    if (scene_cache_get(cache1, &cinfo) != 0) return -1;
    if (cinfo.validity_state != SCENE_CACHE_INVALID) return -1; /* rein lazy erkannt, ohne Sweep */

    /* Unbekannte IDs. */
    if (scene_node_info(0xFFFFu, &ninfo) != SCENE_ERR_NOT_FOUND) return -1;
    if (scene_cache_get(0xFFFFu, &cinfo) != SCENE_ERR_NOT_FOUND) return -1;
    if (scene_transaction_begin(0xFFFFu, &tx9) != SCENE_ERR_NOT_FOUND) return -1;

    /* Beweist die eigentliche Aenderung dieser Session: `scenes[]` war
       GENAU die Tabelle, deren urspruengliche feste Kapazitaet (4, spaeter
       manuell auf 16 angehoben) in einer frueheren Session dieses
       Vorhabens real einen Boot-Test scheitern liess (siehe Dateikopf) -
       jetzt mehr Szenen anlegen, als SCENE_CAPACITY je erlaubt haette,
       beweist mindestens ein scenes_grow(). Jede neue Szene legt
       zusaetzlich einen Root-Node an - beweist bei dieser Gelegenheit
       gleich mit, dass nodes_grow() korrekt mitwaechst. */
    uint32_t scene_growth_count = SCENE_CAPACITY + 3u;
    for (uint32_t i = 0; i < scene_growth_count; ++i) {
        uint32_t sid, rid;
        if (scene_create(&sid, &rid) != 0) return -1;
    }

    /* Zusaetzlich gezielt mehr Nodes IN EINER Szene anlegen, als
       SCENE_NODE_CAPACITY je erlaubt haette - unabhaengig von obigem
       Szenen-Wachstum. */
    uint32_t node_growth_count = SCENE_NODE_CAPACITY + 5u;
    uint32_t last_node = 0;
    for (uint32_t i = 0; i < node_growth_count; ++i) {
        uint32_t tx_n;
        if (scene_transaction_begin(scene1, &tx_n) != 0) return -1;
        uint32_t new_node;
        if (scene_node_create(tx_n, root1, SCENE_NODE_TYPE_GEOMETRY, &new_node) != 0) return -1;
        uint32_t v_n;
        if (scene_transaction_commit(tx_n, &v_n) != 0) return -1;
        last_node = new_node;
    }
    if (scene_node_info(last_node, &ninfo) != 0) return -1;
    if (ninfo.lifecycle_state != SCENE_NODE_ATTACHED) return -1;

    /* Und mehr Cache-Eintraege, als SCENE_CACHE_CAPACITY je erlaubt
       haette (alle auf nodeA, ueberschreiben sich nicht - jeder Aufruf
       von scene_cache_put() legt einen NEUEN Eintrag an). */
    uint32_t cache_growth_count = SCENE_CACHE_CAPACITY + 3u;
    uint32_t last_cache = 0;
    for (uint32_t i = 0; i < cache_growth_count; ++i) {
        if (scene_cache_put(nodeA, SCENE_CACHE_BOUNDS, 1000u + i, &last_cache) != 0) return -1;
    }
    if (scene_cache_get(last_cache, &cinfo) != 0) return -1;
    if (cinfo.cached_value != 1000u + cache_growth_count - 1u) return -1;

    return 0;
}
