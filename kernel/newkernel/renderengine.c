#include "renderengine.h"
#include "scenegraph.h"
#include "gfxstate.h"
#include <stddef.h>

typedef struct {
    uint32_t cache_id;
    uint32_t owner_node_id;
    uint32_t region_id;
    uint32_t state_version;
    uint32_t result_value;
    uint32_t strategy_used;
    int in_use;
} render_cache_entry_t;

static render_cache_entry_t caches[RENDER_CACHE_CAPACITY];
static uint32_t render_next_cache_id = 1;
static int renderengine_ready = 0;

void renderengine_initialize(void) {
    for (uint32_t i = 0; i < RENDER_CACHE_CAPACITY; ++i) {
        caches[i] = (render_cache_entry_t){0};
    }
    render_next_cache_id = 1;
    renderengine_ready = 1;
}

static render_cache_entry_t *cache_find_slot(uint32_t node_id, uint32_t region_id) {
    for (uint32_t i = 0; i < RENDER_CACHE_CAPACITY; ++i) {
        if (caches[i].in_use && caches[i].owner_node_id == node_id && caches[i].region_id == region_id) {
            return &caches[i];
        }
    }
    return NULL;
}

static render_cache_entry_t *cache_find_by_id(uint32_t cache_id) {
    if (cache_id == 0) return NULL;
    for (uint32_t i = 0; i < RENDER_CACHE_CAPACITY; ++i) {
        if (caches[i].in_use && caches[i].cache_id == cache_id) {
            return &caches[i];
        }
    }
    return NULL;
}

/* Platzhalter-"Rendering": eine deterministische, aus Node/Version/Region/
   Strategie abgeleitete Pruefsumme statt echter Pixel - siehe Dateikopf. */
static uint32_t compute_placeholder_result(uint32_t node_id, uint32_t version,
                                            uint32_t region_id, uint32_t strategy) {
    return (node_id * 2654435761u) ^ (version * 40503u) ^ (region_id * 2246822519u) ^ strategy;
}

int render_execute_retained(uint32_t node_id, uint32_t region_id,
                             uint32_t damage_area, uint32_t total_area,
                             uint32_t *out_result_value, int *out_cache_hit,
                             uint32_t *out_strategy) {
    scene_node_info_t ninfo;
    if (scene_node_info(node_id, &ninfo) != 0) {
        return RENDER_ERR_NOT_FOUND;
    }

    render_cache_entry_t *entry = cache_find_slot(node_id, region_id);
    if (entry && entry->state_version == ninfo.state_version) {
        *out_result_value = entry->result_value;
        *out_strategy = entry->strategy_used;
        *out_cache_hit = 1;
        return 0;
    }

    uint32_t strategy = gfx_adaptive_select_strategy(1, damage_area, total_area);
    uint32_t result = compute_placeholder_result(node_id, ninfo.state_version, region_id, strategy);

    if (!entry) {
        uint32_t slot;
        for (slot = 0; slot < RENDER_CACHE_CAPACITY; ++slot) {
            if (!caches[slot].in_use) break;
        }
        if (slot >= RENDER_CACHE_CAPACITY) return RENDER_ERR_CAPACITY;
        entry = &caches[slot];
        entry->cache_id = render_next_cache_id++;
        entry->owner_node_id = node_id;
        entry->region_id = region_id;
        entry->in_use = 1;
    }
    entry->state_version = ninfo.state_version;
    entry->result_value = result;
    entry->strategy_used = strategy;

    *out_result_value = result;
    *out_strategy = strategy;
    *out_cache_hit = 0;
    return 0;
}

int render_cache_find(uint32_t node_id, uint32_t region_id, uint32_t *out_cache_id) {
    render_cache_entry_t *entry = cache_find_slot(node_id, region_id);
    if (!entry) return RENDER_ERR_NOT_FOUND;
    *out_cache_id = entry->cache_id;
    return 0;
}

int render_cache_get(uint32_t cache_id, render_cache_info_t *out_info) {
    render_cache_entry_t *entry = cache_find_by_id(cache_id);
    if (!entry) return RENDER_ERR_NOT_FOUND;
    out_info->cache_id = entry->cache_id;
    out_info->owner_node_id = entry->owner_node_id;
    out_info->region_id = entry->region_id;
    out_info->state_version = entry->state_version;
    out_info->result_value = entry->result_value;
    out_info->strategy_used = entry->strategy_used;
    return 0;
}

int render_execute_immediate(uint32_t target_id, uint32_t payload, uint32_t *out_result_value) {
    *out_result_value = (target_id * 2654435761u) ^ (payload * 2246822519u);
    return 0;
}

void render_cost_estimate(uint32_t change_count, uint32_t damage_area,
                           uint32_t total_area, render_cost_t *out_cost) {
    out_cost->cpu_cost = damage_area / 4u + change_count * 10u;
    out_cost->memory_cost = damage_area * 4u; /* 4 Bytes/Pixel, Platzhalter-Format */
    out_cost->latency_cost = out_cost->cpu_cost / 2u;
    out_cost->confidence = (total_area == 0u) ? 50u : 100u;
}

int render_scissor_clamp(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                          uint32_t target_width, uint32_t target_height,
                          render_rect_t *out_rect) {
    if (target_width == 0u || target_height == 0u) {
        return RENDER_ERR_INVALID;
    }
    if (x >= target_width || y >= target_height) {
        *out_rect = (render_rect_t){0, 0, 0, 0}; /* vollstaendig ausserhalb -> leer */
        return 0;
    }

    uint32_t max_width = target_width - x;
    uint32_t max_height = target_height - y;
    out_rect->x = x;
    out_rect->y = y;
    out_rect->width = (width > max_width) ? max_width : width;
    out_rect->height = (height > max_height) ? max_height : height;
    return 0;
}

/* ----------------------------------------------------------------------- */
/* Selbsttest                                                              */
/* ----------------------------------------------------------------------- */

int renderengine_self_test(void) {
    if (!renderengine_ready) return -1;

    uint32_t scene1, root1;
    if (scene_create(&scene1, &root1) != 0) return -1;

    uint32_t tx1, nodeA;
    if (scene_transaction_begin(scene1, &tx1) != 0) return -1;
    if (scene_node_create(tx1, root1, SCENE_NODE_TYPE_GEOMETRY, &nodeA) != 0) return -1;
    uint32_t v1;
    if (scene_transaction_commit(tx1, &v1) != 0) return -1;

    /* Erster Aufruf: Cache Miss, PARTIAL (kleine Damage-Flaeche). */
    uint32_t result1, strategy1;
    int hit1;
    if (render_execute_retained(nodeA, 0, 10, 1000, &result1, &hit1, &strategy1) != 0) return -1;
    if (hit1 != 0 || strategy1 != GFX_STRATEGY_PARTIAL) return -1;

    /* Zweiter Aufruf, unveraenderter Node: Cache Hit, identisches Ergebnis. */
    uint32_t result2, strategy2;
    int hit2;
    if (render_execute_retained(nodeA, 0, 10, 1000, &result2, &hit2, &strategy2) != 0) return -1;
    if (hit2 != 1 || result2 != result1 || strategy2 != strategy1) return -1;

    /* Grosse Damage-Flaeche -> FULL (unabhaengiger Cache-Test ueber region_id=1). */
    uint32_t result3, strategy3;
    int hit3;
    if (render_execute_retained(nodeA, 1, 600, 1000, &result3, &hit3, &strategy3) != 0) return -1;
    if (hit3 != 0 || strategy3 != GFX_STRATEGY_FULL) return -1;
    if (result3 == result1) return -1; /* andere Region -> anderer Platzhalterwert */

    /* Node-Aenderung committen -> state_version steigt -> naechster Zugriff
       auf region 0 muss lazy als Cache Miss erkannt werden (gleiches Muster
       wie Scene Cache in Phase 2, hier auf der getrennten Render-Cache-
       Tabelle). */
    if (scene_property_define(1u, SCENE_PROP_TYPE_INT, 0, 0) != 0) return -1;
    uint32_t tx2;
    if (scene_transaction_begin(scene1, &tx2) != 0) return -1;
    if (scene_node_set_property(tx2, nodeA, 1u, 7u) != 0) return -1;
    uint32_t v2;
    if (scene_transaction_commit(tx2, &v2) != 0) return -1;
    if (v2 == v1) return -1;

    uint32_t result4, strategy4;
    int hit4;
    if (render_execute_retained(nodeA, 0, 10, 1000, &result4, &hit4, &strategy4) != 0) return -1;
    if (hit4 != 0) return -1; /* lazy invalidiert */
    if (result4 == result1) return -1; /* neue state_version -> anderer Platzhalterwert */

    uint32_t cache_id;
    if (render_cache_find(nodeA, 0, &cache_id) != 0) return -1;
    render_cache_info_t cinfo;
    if (render_cache_get(cache_id, &cinfo) != 0) return -1;
    if (cinfo.state_version != v2 || cinfo.result_value != result4) return -1;

    /* Immediate Rendering: voellig zustandslos, deterministisch. */
    uint32_t imm1, imm2;
    if (render_execute_immediate(777u, 42u, &imm1) != 0) return -1;
    if (render_execute_immediate(777u, 42u, &imm2) != 0) return -1;
    if (imm1 != imm2) return -1;
    uint32_t imm3;
    if (render_execute_immediate(777u, 43u, &imm3) != 0) return -1;
    if (imm3 == imm1) return -1;

    /* Kostenschaetzung: mehr Damage -> hoehere Kosten; unbekannte Flaeche
       -> geringere Konfidenz. */
    render_cost_t cost_small, cost_large, cost_unknown;
    render_cost_estimate(1, 10, 1000, &cost_small);
    render_cost_estimate(1, 800, 1000, &cost_large);
    render_cost_estimate(1, 10, 0, &cost_unknown);
    if (cost_large.cpu_cost <= cost_small.cpu_cost) return -1;
    if (cost_small.confidence != 100 || cost_unknown.confidence != 50) return -1;

    /* Scissor-Clamping. */
    render_rect_t rect;
    if (render_scissor_clamp(10, 10, 50, 50, 100, 100, &rect) != 0) return -1;
    if (rect.x != 10 || rect.y != 10 || rect.width != 50 || rect.height != 50) return -1;

    if (render_scissor_clamp(80, 80, 50, 50, 100, 100, &rect) != 0) return -1;
    if (rect.width != 20 || rect.height != 20) return -1; /* auf Zielgrenzen begrenzt */

    if (render_scissor_clamp(150, 50, 10, 10, 100, 100, &rect) != 0) return -1;
    if (rect.width != 0 || rect.height != 0) return -1; /* vollstaendig ausserhalb -> leer */

    if (render_scissor_clamp(0, 0, 10, 10, 0, 100, &rect) != RENDER_ERR_INVALID) return -1;

    /* Unbekannter Node. */
    uint32_t dummy_val, dummy_strategy;
    int dummy_hit;
    if (render_execute_retained(0xFFFFu, 0, 0, 1000, &dummy_val, &dummy_hit, &dummy_strategy) != RENDER_ERR_NOT_FOUND) {
        return -1;
    }

    return 0;
}
