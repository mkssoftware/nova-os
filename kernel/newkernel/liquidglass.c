#include "liquidglass.h"
#include "materender.h"
#include "scenegraph.h"
#include "material.h"
#include "matperf.h"
#include <stddef.h>

typedef struct {
    uint32_t node_id;
    uint32_t stage;
    uint32_t cache_id;      /* letzte scene_cache_put()-ID, 0 = noch nie gespeichert */
    uint32_t backdrop_version_at_cache; /* state_version des Backdrop-Nodes zum Speicherzeitpunkt */
    int in_use;
} liquidglass_slot_t;

typedef struct {
    uint32_t node_id;
    uint32_t backdrop_node_id;
    int in_use;
} liquidglass_backdrop_t;

static liquidglass_slot_t slots[LIQUIDGLASS_MAX_SLOTS];
static liquidglass_backdrop_t backdrops[LIQUIDGLASS_MAX_SLOTS];
static int liquidglass_ready = 0;

void liquidglass_initialize(void) {
    for (uint32_t i = 0; i < LIQUIDGLASS_MAX_SLOTS; ++i) {
        slots[i] = (liquidglass_slot_t){0};
        backdrops[i] = (liquidglass_backdrop_t){0};
    }
    liquidglass_ready = 1;
}

static liquidglass_slot_t *slot_find(uint32_t node_id, uint32_t stage) {
    for (uint32_t i = 0; i < LIQUIDGLASS_MAX_SLOTS; ++i) {
        if (slots[i].in_use && slots[i].node_id == node_id && slots[i].stage == stage) return &slots[i];
    }
    return NULL;
}

static liquidglass_slot_t *slot_alloc(uint32_t node_id, uint32_t stage) {
    liquidglass_slot_t *s = slot_find(node_id, stage);
    if (s) return s;
    for (uint32_t i = 0; i < LIQUIDGLASS_MAX_SLOTS; ++i) {
        if (!slots[i].in_use) {
            slots[i] = (liquidglass_slot_t){0};
            slots[i].node_id = node_id;
            slots[i].stage = stage;
            slots[i].in_use = 1;
            return &slots[i];
        }
    }
    return NULL;
}

static uint32_t backdrop_find(uint32_t node_id) {
    for (uint32_t i = 0; i < LIQUIDGLASS_MAX_SLOTS; ++i) {
        if (backdrops[i].in_use && backdrops[i].node_id == node_id) return backdrops[i].backdrop_node_id;
    }
    return 0;
}

static int is_liquid_bound(uint32_t node_id, uint32_t *out_instance) {
    uint32_t instance_id = materender_bound_instance(node_id);
    if (instance_id == MATERENDER_UNBOUND) return 0;
    uint32_t def_id = material_instance_definition(instance_id);
    uint32_t type = material_definition_type(def_id);
    if (type != MATERIAL_TYPE_LIQUID) return 0;
    if (out_instance) *out_instance = instance_id;
    return 1;
}

static int stage_uses_backdrop(uint32_t stage) {
    return stage == LIQUIDGLASS_STAGE_BLUR || stage == LIQUIDGLASS_STAGE_REFRACTION;
}

int liquidglass_set_backdrop(uint32_t node_id, uint32_t backdrop_node_id) {
    if (!is_liquid_bound(node_id, NULL)) return LIQUIDGLASS_ERR_INVALID;

    liquidglass_backdrop_t *b = NULL;
    for (uint32_t i = 0; i < LIQUIDGLASS_MAX_SLOTS; ++i) {
        if (backdrops[i].in_use && backdrops[i].node_id == node_id) { b = &backdrops[i]; break; }
    }
    if (!b) {
        for (uint32_t i = 0; i < LIQUIDGLASS_MAX_SLOTS; ++i) {
            if (!backdrops[i].in_use) { b = &backdrops[i]; break; }
        }
        if (!b) return LIQUIDGLASS_ERR_CAPACITY;
        b->node_id = node_id;
        b->in_use = 1;
    }
    b->backdrop_node_id = backdrop_node_id;

    uint32_t edge_id;
    int rc = materender_add_dependency(node_id, backdrop_node_id, &edge_id);
    if (rc != 0 && rc != SCENE_ERR_CYCLE) return LIQUIDGLASS_ERR_INVALID;
    return 0;
}

int liquidglass_evaluate_stage(uint32_t node_id, uint32_t stage, render_rect_t region,
                                uint32_t *out_value, int *out_cache_hit) {
    uint32_t instance_id;
    if (!is_liquid_bound(node_id, &instance_id)) return LIQUIDGLASS_ERR_NOT_FOUND;

    uint32_t steps[MATERENDER_STAGE_COUNT];
    uint32_t step_count;
    materender_pipeline_steps(MATERIAL_TYPE_LIQUID, steps, MATERENDER_STAGE_COUNT, &step_count);
    int stage_valid = 0;
    for (uint32_t i = 0; i < step_count; ++i) if (steps[i] == stage) stage_valid = 1;
    if (!stage_valid) return LIQUIDGLASS_ERR_INVALID;

    uint32_t backdrop_id = stage_uses_backdrop(stage) ? backdrop_find(node_id) : 0;

    liquidglass_slot_t *slot = slot_find(node_id, stage);
    if (slot && slot->cache_id != 0) {
        scene_cache_info_t info;
        int own_valid = (scene_cache_get(slot->cache_id, &info) == 0 &&
                          info.validity_state == SCENE_CACHE_VALID);

        int backdrop_valid = 1;
        if (stage_uses_backdrop(stage)) {
            if (backdrop_id == 0) {
                backdrop_valid = 0; /* kein Backdrop bekannt -> konservativ invalidieren */
            } else {
                scene_node_info_t binfo;
                if (scene_node_info(backdrop_id, &binfo) == 0) {
                    backdrop_valid = (binfo.state_version == slot->backdrop_version_at_cache);
                } else {
                    backdrop_valid = 0; /* Backdrop verschwunden -> konservativ invalidieren */
                }
            }
        }

        if (own_valid && backdrop_valid) {
            *out_value = info.cached_value;
            *out_cache_hit = 1;
            return 0;
        }
    }

    uint32_t backdrop_version = 0;
    if (stage_uses_backdrop(stage) && backdrop_id != 0) {
        scene_node_info_t binfo;
        if (scene_node_info(backdrop_id, &binfo) == 0) backdrop_version = binfo.state_version;
    }

    uint32_t quality = matperf_quality_level();
    uint32_t value = (node_id * 2654435761u) ^ (stage << 24) ^
                      (region.width * 131u + region.height) ^
                      (backdrop_version * 97u) ^ (quality << 4) ^ instance_id;

    if (!slot) slot = slot_alloc(node_id, stage);
    if (slot) {
        uint32_t new_cache_id;
        if (scene_cache_put(node_id, stage, value, &new_cache_id) == 0) {
            slot->cache_id = new_cache_id;
            slot->backdrop_version_at_cache = backdrop_version;
        }
    }

    *out_value = value;
    *out_cache_hit = 0;
    return 0;
}

int liquidglass_evaluate_all(uint32_t node_id, render_rect_t region,
                              uint32_t *out_values, int *out_cache_hits,
                              uint32_t *out_count) {
    if (!is_liquid_bound(node_id, NULL)) return LIQUIDGLASS_ERR_NOT_FOUND;

    uint32_t steps[MATERENDER_STAGE_COUNT];
    uint32_t step_count;
    materender_pipeline_steps(MATERIAL_TYPE_LIQUID, steps, MATERENDER_STAGE_COUNT, &step_count);

    for (uint32_t i = 0; i < step_count; ++i) {
        uint32_t value;
        int cache_hit;
        if (liquidglass_evaluate_stage(node_id, steps[i], region, &value, &cache_hit) != 0) {
            return LIQUIDGLASS_ERR_INVALID;
        }
        out_values[i] = value;
        out_cache_hits[i] = cache_hit;
    }
    *out_count = step_count;
    return 0;
}

int liquidglass_self_test(void) {
    if (!liquidglass_ready) return -1;

    uint32_t sceneA, rootA;
    if (scene_create(&sceneA, &rootA) != 0) return -1;

    uint32_t tx;
    if (scene_transaction_begin(sceneA, &tx) != 0) return -1;
    uint32_t nodeGlass, nodeBackdrop;
    if (scene_node_create(tx, rootA, 1u, &nodeGlass) != 0) return -1;
    if (scene_node_create(tx, rootA, 1u, &nodeBackdrop) != 0) return -1;
    uint32_t version;
    if (scene_transaction_commit(tx, &version) != 0) return -1;

    uint32_t instLiquid = material_instance_create(MATERIAL_ID_LIQUID);
    if (instLiquid == 0) return -1;
    uint32_t instSolid = material_instance_create(MATERIAL_ID_SOLID);
    if (instSolid == 0) return -1;

    render_rect_t region = {0, 0, 16, 16};
    uint32_t value;
    int cache_hit;

    /* Nicht gebundener Node -> NOT_FOUND. */
    if (liquidglass_evaluate_stage(0xFFFFu, LIQUIDGLASS_STAGE_BLUR, region, &value, &cache_hit) != LIQUIDGLASS_ERR_NOT_FOUND) return -1;

    /* An SOLID gebunden -> kein LIQUID, NOT_FOUND. */
    if (materender_bind(nodeBackdrop, instSolid) != 0) return -1;
    if (liquidglass_evaluate_stage(nodeBackdrop, LIQUIDGLASS_STAGE_BLUR, region, &value, &cache_hit) != LIQUIDGLASS_ERR_NOT_FOUND) return -1;

    /* An LIQUID gebunden. */
    if (materender_bind(nodeGlass, instLiquid) != 0) return -1;

    /* LIGHTING ist gueltige Stufe fuer LIQUID, erster Aufruf kein Cache Hit. */
    if (liquidglass_evaluate_stage(nodeGlass, LIQUIDGLASS_STAGE_LIGHTING, region, &value, &cache_hit) != 0) return -1;
    if (cache_hit) return -1;
    uint32_t lighting_first = value;

    /* Zweiter Aufruf unveraendert -> Cache Hit, identisches Ergebnis. */
    if (liquidglass_evaluate_stage(nodeGlass, LIQUIDGLASS_STAGE_LIGHTING, region, &value, &cache_hit) != 0) return -1;
    if (!cache_hit) return -1;
    if (value != lighting_first) return -1;

    /* Ungueltige Stufe (nicht in der LIQUID-Pipeline). */
    if (liquidglass_evaluate_stage(nodeGlass, 0xFFu, region, &value, &cache_hit) != LIQUIDGLASS_ERR_INVALID) return -1;

    /* BLUR ohne gesetzten Backdrop: erster Aufruf berechnet, aber ein
       zweiter Aufruf ohne Backdrop bleibt wegen backdrop_valid==0 IMMER
       ein Miss (konservative Invalidierung, DEPENDENCY-0001 §15). */
    if (liquidglass_evaluate_stage(nodeGlass, LIQUIDGLASS_STAGE_BLUR, region, &value, &cache_hit) != 0) return -1;
    if (cache_hit) return -1;
    if (liquidglass_evaluate_stage(nodeGlass, LIQUIDGLASS_STAGE_BLUR, region, &value, &cache_hit) != 0) return -1;
    if (cache_hit) return -1; /* weiterhin kein Backdrop -> konservativ kein Treffer */

    /* Backdrop setzen (BACKDROP-0001 + echte Phase-9/1-Abhaengigkeit). */
    if (liquidglass_set_backdrop(nodeGlass, nodeBackdrop) != 0) return -1;

    if (liquidglass_evaluate_stage(nodeGlass, LIQUIDGLASS_STAGE_BLUR, region, &value, &cache_hit) != 0) return -1;
    if (cache_hit) return -1; /* Backdrop neu gesetzt -> noch kein gueltiger Eintrag mit Backdrop-Version */
    uint32_t blur_first = value;

    /* Backdrop unveraendert -> echter Cache Hit. */
    if (liquidglass_evaluate_stage(nodeGlass, LIQUIDGLASS_STAGE_BLUR, region, &value, &cache_hit) != 0) return -1;
    if (!cache_hit) return -1;
    if (value != blur_first) return -1;

    /* Backdrop-Node aendert sich (eigene Transaktion) -> Blur-Cache wird
       trotz UNVERAENDERTEM Glass-Node invalidiert (echte transitive
       Backdrop-Abhaengigkeit, nicht nur die eigene state_version). */
    uint32_t tx2;
    if (scene_transaction_begin(sceneA, &tx2) != 0) return -1;
    if (scene_node_set_property(tx2, nodeBackdrop, 1u, 99u) != 0) return -1;
    uint32_t version2;
    if (scene_transaction_commit(tx2, &version2) != 0) return -1;

    if (liquidglass_evaluate_stage(nodeGlass, LIQUIDGLASS_STAGE_BLUR, region, &value, &cache_hit) != 0) return -1;
    if (cache_hit) return -1; /* Backdrop-Version hat sich geaendert */

    /* LIGHTING haengt NICHT vom Backdrop ab - bleibt unberuehrt ein
       Cache Hit (unabhaengige Invalidierung pro Effektstufe,
       INVALIDATION-0001 §5). */
    if (liquidglass_evaluate_stage(nodeGlass, LIQUIDGLASS_STAGE_LIGHTING, region, &value, &cache_hit) != 0) return -1;
    if (!cache_hit) return -1;

    /* evaluate_all: LIQUID hat 5 Stufen. */
    uint32_t values[LIQUIDGLASS_STAGE_COUNT];
    int hits[LIQUIDGLASS_STAGE_COUNT];
    uint32_t count;
    if (liquidglass_evaluate_all(nodeGlass, region, values, hits, &count) != 0) return -1;
    if (count != 5) return -1;

    return 0;
}
