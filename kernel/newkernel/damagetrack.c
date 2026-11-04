#include "damagetrack.h"
#include "gfxstate.h"
#include "scenegraph.h"
#include <stddef.h>

typedef struct {
    uint32_t region_id;
    uint32_t target_id;
    render_rect_t bounds;
    uint32_t state_version;
    uint32_t region_type;
    int deferred;
    int in_use;
} damage_region_t;

static damage_region_t regions[DAMAGE_REGION_CAPACITY];
static uint32_t damage_next_region_id = 1;
static int damagetrack_ready = 0;

void damagetrack_initialize(void) {
    for (uint32_t i = 0; i < DAMAGE_REGION_CAPACITY; ++i) {
        regions[i] = (damage_region_t){0};
    }
    damage_next_region_id = 1;
    damagetrack_ready = 1;
}

static int rect_empty(render_rect_t r) {
    return r.width == 0 || r.height == 0;
}

render_rect_t damage_rect_union(render_rect_t a, render_rect_t b) {
    if (rect_empty(a)) return b;
    if (rect_empty(b)) return a;

    uint32_t left = (a.x < b.x) ? a.x : b.x;
    uint32_t top = (a.y < b.y) ? a.y : b.y;
    uint32_t a_right = a.x + a.width;
    uint32_t b_right = b.x + b.width;
    uint32_t right = (a_right > b_right) ? a_right : b_right;
    uint32_t a_bottom = a.y + a.height;
    uint32_t b_bottom = b.y + b.height;
    uint32_t bottom = (a_bottom > b_bottom) ? a_bottom : b_bottom;

    render_rect_t out;
    out.x = left;
    out.y = top;
    out.width = right - left;
    out.height = bottom - top;
    return out;
}

int damage_rect_overlaps_or_adjacent(render_rect_t a, render_rect_t b, uint32_t margin) {
    if (rect_empty(a) || rect_empty(b)) return 0;

    uint32_t ax0 = (a.x > margin) ? a.x - margin : 0;
    uint32_t ay0 = (a.y > margin) ? a.y - margin : 0;
    uint32_t ax1 = a.x + a.width + margin;
    uint32_t ay1 = a.y + a.height + margin;

    if (ax0 >= b.x + b.width || b.x >= ax1) return 0;
    if (ay0 >= b.y + b.height || b.y >= ay1) return 0;
    return 1;
}

render_rect_t damage_rect_expand(render_rect_t r, uint32_t margin) {
    if (rect_empty(r)) return r;

    uint32_t left = (r.x > margin) ? r.x - margin : 0;
    uint32_t top = (r.y > margin) ? r.y - margin : 0;
    uint32_t right = r.x + r.width + margin;
    uint32_t bottom = r.y + r.height + margin;

    render_rect_t out;
    out.x = left;
    out.y = top;
    out.width = right - left;
    out.height = bottom - top;
    return out;
}

static damage_region_t *region_find(uint32_t region_id) {
    if (region_id == 0) return NULL;
    for (uint32_t i = 0; i < DAMAGE_REGION_CAPACITY; ++i) {
        if (regions[i].in_use && regions[i].region_id == region_id) return &regions[i];
    }
    return NULL;
}

int damage_add(uint32_t target_id, render_rect_t rect, uint32_t state_version,
                uint32_t *out_region_id) {
    uint32_t slot;
    for (slot = 0; slot < DAMAGE_REGION_CAPACITY; ++slot) {
        if (!regions[slot].in_use) break;
    }
    if (slot >= DAMAGE_REGION_CAPACITY) return DAMAGE_ERR_CAPACITY;

    regions[slot].region_id = damage_next_region_id++;
    regions[slot].target_id = target_id;
    regions[slot].bounds = rect;
    regions[slot].state_version = state_version;
    regions[slot].region_type = DAMAGE_TYPE_PARTIAL;
    regions[slot].deferred = 0;
    regions[slot].in_use = 1;

    *out_region_id = regions[slot].region_id;
    return 0;
}

int damage_from_node_change(uint32_t node_id, render_rect_t local_bounds, uint32_t *out_count) {
    scene_node_info_t ninfo;
    if (scene_node_info(node_id, &ninfo) != 0) {
        return DAMAGE_ERR_NOT_FOUND;
    }

    uint32_t region_id;
    if (damage_add(node_id, local_bounds, ninfo.state_version, &region_id) != 0) {
        return DAMAGE_ERR_CAPACITY;
    }
    uint32_t count = 1;

    uint32_t affected[DAMAGE_PROPAGATE_MAX];
    uint32_t affected_count;
    gfx_dependency_invalidate_from(node_id, affected, DAMAGE_PROPAGATE_MAX, &affected_count);

    for (uint32_t i = 0; i < affected_count; ++i) {
        uint32_t target_version = 0;
        scene_node_info_t tinfo;
        if (scene_node_info(affected[i], &tinfo) == 0) {
            target_version = tinfo.state_version;
        }
        uint32_t dep_region_id;
        if (damage_add(affected[i], local_bounds, target_version, &dep_region_id) == 0) {
            count++;
        }
    }

    *out_count = count;
    return 0;
}

int damage_mark_occluded(uint32_t region_id) {
    damage_region_t *r = region_find(region_id);
    if (!r) return DAMAGE_ERR_NOT_FOUND;
    r->deferred = 1;
    return 0;
}

int damage_mark_visible(uint32_t region_id) {
    damage_region_t *r = region_find(region_id);
    if (!r) return DAMAGE_ERR_NOT_FOUND;
    r->deferred = 0;
    return 0;
}

int damage_resolve(uint32_t region_id) {
    damage_region_t *r = region_find(region_id);
    if (!r) return DAMAGE_ERR_NOT_FOUND;
    *r = (damage_region_t){0};
    return 0;
}

int damage_region_info(uint32_t region_id, damage_region_info_t *out_info) {
    damage_region_t *r = region_find(region_id);
    if (!r) return DAMAGE_ERR_NOT_FOUND;
    out_info->region_id = r->region_id;
    out_info->target_id = r->target_id;
    out_info->bounds = r->bounds;
    out_info->state_version = r->state_version;
    out_info->region_type = r->region_type;
    out_info->deferred = r->deferred;
    return 0;
}

int damage_pending_count(uint32_t target_id, uint32_t *out_count) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < DAMAGE_REGION_CAPACITY; ++i) {
        if (regions[i].in_use && regions[i].target_id == target_id && !regions[i].deferred) {
            count++;
        }
    }
    *out_count = count;
    return 0;
}

int damage_merge_target(uint32_t target_id, render_rect_t *out_merged, uint32_t max_out,
                        uint32_t *out_count, uint32_t *out_region_type) {
    uint32_t pending_count = 0;
    for (uint32_t i = 0; i < DAMAGE_REGION_CAPACITY; ++i) {
        if (regions[i].in_use && regions[i].target_id == target_id && !regions[i].deferred) {
            pending_count++;
        }
    }

    if (pending_count == 0) {
        *out_count = 0;
        *out_region_type = DAMAGE_TYPE_PARTIAL;
        return 0;
    }

    if (pending_count > DAMAGE_MAX_PER_TARGET_BEFORE_FULL) {
        render_rect_t full = (render_rect_t){0, 0, 0, 0};
        for (uint32_t i = 0; i < DAMAGE_REGION_CAPACITY; ++i) {
            if (regions[i].in_use && regions[i].target_id == target_id && !regions[i].deferred) {
                full = damage_rect_union(full, regions[i].bounds);
            }
        }
        if (max_out > 0) out_merged[0] = full;
        *out_count = 1;
        *out_region_type = DAMAGE_TYPE_FULL;
        return 0;
    }

    uint32_t working_count = 0;
    for (uint32_t i = 0; i < DAMAGE_REGION_CAPACITY && working_count < max_out; ++i) {
        if (!regions[i].in_use || regions[i].target_id != target_id || regions[i].deferred) continue;

        int merged = 0;
        for (uint32_t w = 0; w < working_count; ++w) {
            if (damage_rect_overlaps_or_adjacent(out_merged[w], regions[i].bounds, 0)) {
                out_merged[w] = damage_rect_union(out_merged[w], regions[i].bounds);
                merged = 1;
                break;
            }
        }
        if (!merged) {
            out_merged[working_count++] = regions[i].bounds;
        }
    }

    *out_count = working_count;
    *out_region_type = DAMAGE_TYPE_PARTIAL;
    return 0;
}

int damage_resolve_target(uint32_t target_id, uint32_t *out_count) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < DAMAGE_REGION_CAPACITY; ++i) {
        if (regions[i].in_use && regions[i].target_id == target_id && !regions[i].deferred) {
            regions[i] = (damage_region_t){0};
            count++;
        }
    }
    *out_count = count;
    return 0;
}

/* ----------------------------------------------------------------------- */
/* Selbsttest                                                              */
/* ----------------------------------------------------------------------- */

int damagetrack_self_test(void) {
    if (!damagetrack_ready) return -1;

    /* Rect-Operationen. */
    render_rect_t a = {10, 10, 20, 20}; /* [10,30)x[10,30) */
    render_rect_t b = {25, 25, 20, 20}; /* [25,45)x[25,45) - ueberlappt a */
    render_rect_t c = {100, 100, 10, 10}; /* weit entfernt */

    render_rect_t u = damage_rect_union(a, b);
    if (u.x != 10 || u.y != 10 || u.width != 35 || u.height != 35) return -1;

    if (!damage_rect_overlaps_or_adjacent(a, b, 0)) return -1;
    if (damage_rect_overlaps_or_adjacent(a, c, 0)) return -1;
    if (!damage_rect_overlaps_or_adjacent(a, c, 100)) return -1; /* grosse Margin macht sie "benachbart" */

    render_rect_t expanded = damage_rect_expand(a, 5);
    if (expanded.x != 5 || expanded.y != 5 || expanded.width != 30 || expanded.height != 30) return -1;

    render_rect_t at_edge = {2, 2, 10, 10};
    render_rect_t edge_expanded = damage_rect_expand(at_edge, 5);
    if (edge_expanded.x != 0 || edge_expanded.y != 0) return -1; /* Clamp an 0 */

    /* Hierarchical/Dependency/Propagation: echte Scene-Node- +
       Dependency-Integration (Phase 1/2). */
    uint32_t scene1, root1;
    if (scene_create(&scene1, &root1) != 0) return -1;
    uint32_t tx1, nodeA, nodeB;
    if (scene_transaction_begin(scene1, &tx1) != 0) return -1;
    if (scene_node_create(tx1, root1, SCENE_NODE_TYPE_GEOMETRY, &nodeA) != 0) return -1;
    if (scene_node_create(tx1, root1, SCENE_NODE_TYPE_MATERIAL, &nodeB) != 0) return -1;
    uint32_t v1;
    if (scene_transaction_commit(tx1, &v1) != 0) return -1;

    uint32_t edge_ab;
    if (scene_node_add_dependency(nodeA, nodeB, GFX_DEP_MATERIAL, &edge_ab) != 0) return -1;

    render_rect_t trigger = {10, 10, 20, 20};
    uint32_t prop_count;
    if (damage_from_node_change(nodeA, trigger, &prop_count) != 0) return -1;
    if (prop_count != 2) return -1; /* nodeA selbst + nodeB via Dependency */

    uint32_t pending_a, pending_b;
    if (damage_pending_count(nodeA, &pending_a) != 0) return -1;
    if (damage_pending_count(nodeB, &pending_b) != 0) return -1;
    if (pending_a != 1 || pending_b != 1) return -1;

    /* Merge: zwei ueberlappende Regionen fuer denselben Node -> 1 Rect. */
    uint32_t region_extra;
    if (damage_add(nodeA, (render_rect_t){15, 15, 20, 20}, v1, &region_extra) != 0) return -1;
    render_rect_t merged[8];
    uint32_t merged_count, merged_type;
    if (damage_merge_target(nodeA, merged, 8, &merged_count, &merged_type) != 0) return -1;
    if (merged_count != 1 || merged_type != DAMAGE_TYPE_PARTIAL) return -1;
    if (merged[0].x != 10 || merged[0].y != 10) return -1;

    /* Fragmentierung -> Full Damage Fallback. */
    for (uint32_t i = 0; i < DAMAGE_MAX_PER_TARGET_BEFORE_FULL + 1; ++i) {
        uint32_t dummy_id;
        render_rect_t far_rect = {(uint32_t)(1000 + i * 50), 1000, 5, 5};
        if (damage_add(nodeB, far_rect, v1, &dummy_id) != 0) return -1;
    }
    render_rect_t merged2[8];
    uint32_t merged2_count, merged2_type;
    if (damage_merge_target(nodeB, merged2, 8, &merged2_count, &merged2_type) != 0) return -1;
    if (merged2_count != 1 || merged2_type != DAMAGE_TYPE_FULL) return -1;

    /* Occlusion: deferred Region zaehlt nicht als pending, bleibt aber
       erhalten (kein Datenverlust) und kann reaktiviert werden. */
    uint32_t occl_region;
    if (damage_add(999u, (render_rect_t){1, 1, 1, 1}, v1, &occl_region) != 0) return -1;
    uint32_t before_occl;
    if (damage_pending_count(999u, &before_occl) != 0) return -1;
    if (before_occl != 1) return -1;
    if (damage_mark_occluded(occl_region) != 0) return -1;
    uint32_t after_occl;
    if (damage_pending_count(999u, &after_occl) != 0) return -1;
    if (after_occl != 0) return -1;
    damage_region_info_t rinfo;
    if (damage_region_info(occl_region, &rinfo) != 0) return -1; /* bleibt erhalten */
    if (!rinfo.deferred) return -1;
    if (damage_mark_visible(occl_region) != 0) return -1;
    uint32_t after_visible;
    if (damage_pending_count(999u, &after_visible) != 0) return -1;
    if (after_visible != 1) return -1;

    /* Resolve: endgueltig entfernt. */
    if (damage_resolve(occl_region) != 0) return -1;
    if (damage_region_info(occl_region, &rinfo) != DAMAGE_ERR_NOT_FOUND) return -1;
    if (damage_resolve(occl_region) != DAMAGE_ERR_NOT_FOUND) return -1; /* bereits entfernt */

    /* Resolve-Target: loest alle ausstehenden Regionen eines Ziels auf einmal. */
    uint32_t resolve_target_count;
    if (damage_resolve_target(nodeA, &resolve_target_count) != 0) return -1;
    if (resolve_target_count != 2) return -1; /* trigger + extra */
    if (damage_pending_count(nodeA, &pending_a) != 0) return -1;
    if (pending_a != 0) return -1;

    /* Unbekannte IDs. */
    if (damage_from_node_change(0xFFFFu, trigger, &prop_count) != DAMAGE_ERR_NOT_FOUND) return -1;
    if (damage_mark_occluded(0xFFFFu) != DAMAGE_ERR_NOT_FOUND) return -1;

    return 0;
}
