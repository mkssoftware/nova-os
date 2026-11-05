/* Dynamisch wachsende Layer-/Composition-Cache-Tabellen (2026-11-05,
   Antwort auf "viele feste Grenzen nicht dynamisch loesbar" - siehe
   [[project_general_allocator]] fuer kalloc.c, zweites umgestelltes
   Modul nach process.c). LAYER_CAPACITY/COMPOSITION_CACHE_CAPACITY
   (layercompose.h) sind jetzt nur noch Startkapazitaeten. */
#include "layercompose.h"
#include "damagetrack.h"
#include "kalloc.h"
#include <stddef.h>

typedef struct {
    uint32_t layer_id;
    uint32_t parent_id;
    uint32_t category;
    uint32_t surface_id;
    uint32_t node_id;
    render_rect_t bounds;
    int32_t z_order;
    uint32_t opacity;
    int opaque;
    int needs_backdrop;
    uint32_t visibility_state;
    uint32_t state_version;
    int in_use;
} layer_t;

typedef struct {
    uint32_t layer_id;
    uint32_t state_version;
    uint32_t value;
    int in_use;
} composition_cache_entry_t;

static layer_t *layers = NULL;
static uint32_t layers_capacity = 0;
static composition_cache_entry_t *caches = NULL;
static uint32_t caches_capacity = 0;
static uint32_t layer_next_id = 1;
static int layercompose_ready = 0;

void layercompose_initialize(void) {
    if (layers) kfree(layers);
    layers = (layer_t *)kcalloc(LAYER_CAPACITY, sizeof(layer_t));
    layers_capacity = layers ? LAYER_CAPACITY : 0u;

    if (caches) kfree(caches);
    caches = (composition_cache_entry_t *)kcalloc(COMPOSITION_CACHE_CAPACITY, sizeof(composition_cache_entry_t));
    caches_capacity = caches ? COMPOSITION_CACHE_CAPACITY : 0u;

    layer_next_id = 1;
    layercompose_ready = 1;
}

/* Verdoppelt layers[] (siehe process.c's process_table_grow() fuer das
   identische Muster: krealloc() kopiert nur alte Bytes, der Zuwachs wird
   hier explizit genullt). 0 = Erfolg (*out_slot = erster freier Slot im
   neuen Bereich), -1 bei Erschoepfung (layers/layers_capacity bleiben
   unveraendert gueltig). */
static int layers_grow(uint32_t *out_slot) {
    uint32_t new_capacity = (layers_capacity == 0u) ? LAYER_CAPACITY : layers_capacity * 2u;
    layer_t *grown = (layer_t *)krealloc(layers, new_capacity * (uint32_t)sizeof(layer_t));
    if (!grown) return -1;
    for (uint32_t i = layers_capacity; i < new_capacity; ++i) grown[i] = (layer_t){0};
    *out_slot = layers_capacity;
    layers = grown;
    layers_capacity = new_capacity;
    return 0;
}

/* Gegenstueck zu layers_grow() fuer caches[]. */
static int caches_grow(uint32_t *out_slot) {
    uint32_t new_capacity = (caches_capacity == 0u) ? COMPOSITION_CACHE_CAPACITY : caches_capacity * 2u;
    composition_cache_entry_t *grown = (composition_cache_entry_t *)krealloc(
        caches, new_capacity * (uint32_t)sizeof(composition_cache_entry_t));
    if (!grown) return -1;
    for (uint32_t i = caches_capacity; i < new_capacity; ++i) grown[i] = (composition_cache_entry_t){0};
    *out_slot = caches_capacity;
    caches = grown;
    caches_capacity = new_capacity;
    return 0;
}

static layer_t *layer_find(uint32_t layer_id) {
    if (layer_id == 0) return NULL;
    for (uint32_t i = 0; i < layers_capacity; ++i) {
        if (layers[i].in_use && layers[i].layer_id == layer_id) return &layers[i];
    }
    return NULL;
}

static void touch_layer(layer_t *l) {
    l->state_version++;
    uint32_t region_id;
    damage_add(l->layer_id, l->bounds, l->state_version, &region_id);
}

int layer_create(uint32_t parent_id, uint32_t category, uint32_t surface_id, uint32_t node_id,
                  render_rect_t bounds, int32_t z_order, uint32_t *out_layer_id) {
    if (parent_id != 0 && !layer_find(parent_id)) return LAYER_ERR_NOT_FOUND;

    uint32_t slot;
    for (slot = 0; slot < layers_capacity; ++slot) {
        if (!layers[slot].in_use) break;
    }
    if (slot >= layers_capacity) {
        if (layers_grow(&slot) != 0) return LAYER_ERR_CAPACITY;
    }

    layers[slot] = (layer_t){0};
    layers[slot].layer_id = layer_next_id++;
    layers[slot].parent_id = parent_id;
    layers[slot].category = category;
    layers[slot].surface_id = surface_id;
    layers[slot].node_id = node_id;
    layers[slot].bounds = bounds;
    layers[slot].z_order = z_order;
    layers[slot].opacity = 100;
    layers[slot].opaque = 0;
    layers[slot].needs_backdrop = 0;
    layers[slot].visibility_state = LAYER_VISIBLE;
    layers[slot].state_version = 1;
    layers[slot].in_use = 1;

    uint32_t region_id;
    damage_add(layers[slot].layer_id, bounds, 1, &region_id);

    *out_layer_id = layers[slot].layer_id;
    return 0;
}

int layer_set_bounds(uint32_t layer_id, render_rect_t bounds) {
    layer_t *l = layer_find(layer_id);
    if (!l) return LAYER_ERR_NOT_FOUND;
    l->bounds = bounds;
    touch_layer(l);
    return 0;
}

int layer_set_opacity(uint32_t layer_id, uint32_t opacity) {
    layer_t *l = layer_find(layer_id);
    if (!l) return LAYER_ERR_NOT_FOUND;
    l->opacity = opacity;
    touch_layer(l);
    return 0;
}

int layer_set_z_order(uint32_t layer_id, int32_t z_order) {
    layer_t *l = layer_find(layer_id);
    if (!l) return LAYER_ERR_NOT_FOUND;
    l->z_order = z_order;
    touch_layer(l);
    return 0;
}

int layer_set_opaque(uint32_t layer_id, int opaque) {
    layer_t *l = layer_find(layer_id);
    if (!l) return LAYER_ERR_NOT_FOUND;
    l->opaque = opaque;
    return 0;
}

int layer_set_needs_backdrop(uint32_t layer_id, int needs_backdrop) {
    layer_t *l = layer_find(layer_id);
    if (!l) return LAYER_ERR_NOT_FOUND;
    l->needs_backdrop = needs_backdrop;
    return 0;
}

int layer_remove(uint32_t layer_id) {
    layer_t *l = layer_find(layer_id);
    if (!l) return LAYER_ERR_NOT_FOUND;
    uint32_t resolved;
    damage_resolve_target(layer_id, &resolved);
    for (uint32_t i = 0; i < caches_capacity; ++i) {
        if (caches[i].in_use && caches[i].layer_id == layer_id) caches[i] = (composition_cache_entry_t){0};
    }
    *l = (layer_t){0};
    return 0;
}

int layer_info(uint32_t layer_id, layer_info_t *out_info) {
    layer_t *l = layer_find(layer_id);
    if (!l) return LAYER_ERR_NOT_FOUND;
    out_info->layer_id = l->layer_id;
    out_info->parent_id = l->parent_id;
    out_info->category = l->category;
    out_info->surface_id = l->surface_id;
    out_info->node_id = l->node_id;
    out_info->bounds = l->bounds;
    out_info->z_order = l->z_order;
    out_info->opacity = l->opacity;
    out_info->opaque = l->opaque;
    out_info->needs_backdrop = l->needs_backdrop;
    out_info->visibility_state = l->visibility_state;
    out_info->state_version = l->state_version;
    return 0;
}

static int rect_contains(render_rect_t outer, render_rect_t inner) {
    if (outer.width == 0 || outer.height == 0 || inner.width == 0 || inner.height == 0) return 0;
    if (inner.x < outer.x || inner.y < outer.y) return 0;
    if (inner.x + inner.width > outer.x + outer.width) return 0;
    if (inner.y + inner.height > outer.y + outer.height) return 0;
    return 1;
}

int compositor_occlusion_pass(void) {
    for (uint32_t i = 0; i < layers_capacity; ++i) {
        if (!layers[i].in_use) continue;
        if (layers[i].needs_backdrop) {
            layers[i].visibility_state = LAYER_EFFECT_DEPENDENT;
            continue;
        }

        int occluded = 0;
        for (uint32_t j = 0; j < layers_capacity; ++j) {
            if (!layers[j].in_use || i == j) continue;
            if (layers[j].z_order <= layers[i].z_order) continue;
            if (!layers[j].opaque || layers[j].opacity != 100) continue;
            if (rect_contains(layers[j].bounds, layers[i].bounds)) {
                occluded = 1;
                break;
            }
        }
        layers[i].visibility_state = occluded ? LAYER_OCCLUDED : LAYER_VISIBLE;
    }
    return 0;
}

static composition_cache_entry_t *cache_find(uint32_t layer_id) {
    for (uint32_t i = 0; i < caches_capacity; ++i) {
        if (caches[i].in_use && caches[i].layer_id == layer_id) return &caches[i];
    }
    return NULL;
}

int compositor_compose(uint32_t damage_area, uint32_t total_area,
                        uint32_t *out_composited_value, uint32_t *out_processed_count) {
    uint32_t composited = 0;
    uint32_t processed = 0;

    /* Painter's Algorithm: aufsteigende Z-Order (hinten->vorne). Einfache
       Auswahl-Sortierung ueber die Layer-Tabelle reicht hier aus. "done"
       wird dynamisch auf die AKTUELLE layers_capacity bemessen (statt
       eines fest LAYER_CAPACITY grossen Arrays) - sie kann durch
       layer_create()/layers_grow() seit dem letzten Aufruf gewachsen
       sein. */
    int *done = (int *)kcalloc(layers_capacity, sizeof(int));
    if (!done) return -1;
    for (uint32_t step = 0; step < layers_capacity; ++step) {
        int best = -1;
        for (uint32_t i = 0; i < layers_capacity; ++i) {
            if (!layers[i].in_use || done[i]) continue;
            if (best < 0 || layers[i].z_order < layers[(uint32_t)best].z_order) best = (int)i;
        }
        if (best < 0) break;
        done[(uint32_t)best] = 1;

        layer_t *l = &layers[(uint32_t)best];
        if (l->visibility_state == LAYER_OCCLUDED) continue;

        uint32_t pending;
        damage_pending_count(l->layer_id, &pending);

        composition_cache_entry_t *entry = cache_find(l->layer_id);
        uint32_t value;
        if (pending > 0 || !entry || entry->state_version != l->state_version) {
            if (l->node_id != 0) {
                uint32_t result_value, strategy;
                int cache_hit;
                render_execute_retained(l->node_id, 0, damage_area, total_area,
                                         &result_value, &cache_hit, &strategy);
                value = result_value;
            } else {
                value = (l->layer_id * 2654435761u) ^ (l->state_version * 40503u);
            }
            if (!entry) {
                for (uint32_t c = 0; c < caches_capacity; ++c) {
                    if (!caches[c].in_use) {
                        entry = &caches[c];
                        break;
                    }
                }
                if (!entry) {
                    uint32_t new_slot;
                    if (caches_grow(&new_slot) == 0) entry = &caches[new_slot];
                }
                if (entry) {
                    entry->layer_id = l->layer_id;
                    entry->in_use = 1;
                }
            }
            if (entry) {
                entry->state_version = l->state_version;
                entry->value = value;
            }
            uint32_t resolved;
            damage_resolve_target(l->layer_id, &resolved);
            processed++;
        } else {
            value = entry->value;
        }

        composited ^= value;
    }

    kfree(done);
    *out_composited_value = composited;
    *out_processed_count = processed;
    return 0;
}

/* ----------------------------------------------------------------------- */
/* Selbsttest                                                              */
/* ----------------------------------------------------------------------- */

#include "scenegraph.h"

int layercompose_self_test(void) {
    if (!layercompose_ready) return -1;

    /* Occlusion: layerBack (opaque) wird von layerFront (opaque, groesser/
       gleich, weiter vorne) vollstaendig verdeckt. */
    uint32_t layerBack, layerFront, layerEffect;
    render_rect_t covering = {0, 0, 1000, 1000};
    render_rect_t inside1 = {10, 10, 5, 5};
    render_rect_t inside2 = {20, 20, 5, 5};

    if (layer_create(0, LAYER_CATEGORY_DESKTOP, 0, 0, inside1, 0, &layerBack) != 0) return -1;
    if (layer_set_opaque(layerBack, 1) != 0) return -1;

    if (layer_create(0, LAYER_CATEGORY_APPLICATION, 0, 0, covering, 1, &layerFront) != 0) return -1;
    if (layer_set_opaque(layerFront, 1) != 0) return -1;

    if (layer_create(0, LAYER_CATEGORY_SYSTEM, 0, 0, inside2, 0, &layerEffect) != 0) return -1;
    if (layer_set_needs_backdrop(layerEffect, 1) != 0) return -1;

    if (compositor_occlusion_pass() != 0) return -1;

    layer_info_t linfo;
    if (layer_info(layerBack, &linfo) != 0) return -1;
    if (linfo.visibility_state != LAYER_OCCLUDED) return -1; /* vollstaendig von layerFront enthalten */
    if (layer_info(layerFront, &linfo) != 0) return -1;
    if (linfo.visibility_state != LAYER_VISIBLE) return -1; /* nichts verdeckt layerFront */
    if (layer_info(layerEffect, &linfo) != 0) return -1;
    if (linfo.visibility_state != LAYER_EFFECT_DEPENDENT) return -1; /* needs_backdrop -> niemals okkludiert */

    /* Compose: Scene-Node-Integration (Phase 3) fuer eine eigene,
       NICHT verdeckte Layer. */
    uint32_t scene1, root1, nodeA;
    if (scene_create(&scene1, &root1) != 0) return -1;
    uint32_t tx1;
    if (scene_transaction_begin(scene1, &tx1) != 0) return -1;
    if (scene_node_create(tx1, root1, SCENE_NODE_TYPE_GEOMETRY, &nodeA) != 0) return -1;
    uint32_t v1;
    if (scene_transaction_commit(tx1, &v1) != 0) return -1;

    uint32_t layerContent;
    render_rect_t content_bounds = {500, 500, 50, 50};
    if (layer_create(0, LAYER_CATEGORY_APPLICATION, 0, nodeA, content_bounds, 2, &layerContent) != 0) return -1;
    if (compositor_occlusion_pass() != 0) return -1; /* niemand enthaelt layerContent vollstaendig */
    if (layer_info(layerContent, &linfo) != 0) return -1;
    if (linfo.visibility_state != LAYER_VISIBLE) return -1;

    uint32_t composited1, processed1;
    if (compositor_compose(10, 1000, &composited1, &processed1) != 0) return -1;
    if (processed1 == 0) return -1; /* layerFront + layerEffect + layerContent sind neu */

    uint32_t composited2, processed2;
    if (compositor_compose(10, 1000, &composited2, &processed2) != 0) return -1;
    if (processed2 != 0) return -1; /* alles Cache-Treffer, nichts veraendert */
    if (composited1 != composited2) return -1; /* gleiches akkumuliertes Ergebnis */

    /* Vergleich mit direktem render_execute_retained()-Aufruf (Phase 3):
       muss Cache Hit liefern, derselbe Wert wie beim Compose. */
    uint32_t direct_value, direct_strategy;
    int direct_hit;
    if (render_execute_retained(nodeA, 0, 10, 1000, &direct_value, &direct_hit, &direct_strategy) != 0) return -1;
    if (direct_hit != 1) return -1;

    /* Aenderung an layerContent erzeugt echten Damage (Phase 5) -> naechster
       compose()-Aufruf muss sie erneut verarbeiten. */
    if (layer_set_bounds(layerContent, (render_rect_t){600, 600, 50, 50}) != 0) return -1;
    uint32_t composited3, processed3;
    if (compositor_compose(10, 1000, &composited3, &processed3) != 0) return -1;
    if (processed3 != 1) return -1; /* nur layerContent wurde neu verarbeitet */

    /* Entfernen + Introspektion. */
    if (layer_remove(layerContent) != 0) return -1;
    if (layer_info(layerContent, &linfo) != LAYER_ERR_NOT_FOUND) return -1;

    /* Zyklus-/Unbekannt-Pruefungen. */
    uint32_t dummy;
    if (layer_create(0xFFFFu, LAYER_CATEGORY_POPUP, 0, 0, inside1, 0, &dummy) != LAYER_ERR_NOT_FOUND) return -1;
    if (layer_set_bounds(0xFFFFu, inside1) != LAYER_ERR_NOT_FOUND) return -1;
    if (layer_info(0xFFFFu, &linfo) != LAYER_ERR_NOT_FOUND) return -1;

    /* Beweist die eigentliche Aenderung dieser Session: mehr Layer
       anlegen, als die urspruengliche feste LAYER_CAPACITY je erlaubt
       haette (vorher: layer_create() haette hier LAYER_ERR_CAPACITY
       geliefert) - muss dafuer mindestens einmal per layers_grow()
       gewachsen sein. compositor_compose() laeuft danach ueber die
       gewachsene Tabelle (beweist, dass das dynamisch allozierte
       "done"-Array in compositor_compose() korrekt mitgewachsen ist)
       und muss dabei auch den Composition-Cache per caches_grow() ueber
       COMPOSITION_CACHE_CAPACITY hinaus wachsen lassen (jeder dieser
       Layer braucht einen eigenen, neuen Cache-Eintrag). */
    uint32_t growth_count = LAYER_CAPACITY + 3u;
    uint32_t growth_ids[LAYER_CAPACITY + 3u];
    for (uint32_t i = 0; i < growth_count; ++i) {
        render_rect_t r = {100u + i, 100u, 2u, 2u};
        if (layer_create(0, LAYER_CATEGORY_APPLICATION, 0, 0, r, 5, &growth_ids[i]) != 0) return -1;
    }
    if (compositor_occlusion_pass() != 0) return -1;

    uint32_t composited_g, processed_g;
    if (compositor_compose(10, 1000, &composited_g, &processed_g) != 0) return -1;
    if (processed_g < growth_count) return -1; /* alle neuen Layer wurden verarbeitet */

    for (uint32_t i = 0; i < growth_count; ++i) {
        if (layer_remove(growth_ids[i]) != 0) return -1;
    }

    return 0;
}
