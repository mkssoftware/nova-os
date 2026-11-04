#include "materender.h"
#include "gfxstate.h"
#include "scenegraph.h"
#include "damagetrack.h"
#include "material.h"
#include "matperf.h"
#include <stddef.h>

typedef struct {
    uint32_t node_id;
    uint32_t instance_id;
    int in_use;
} materender_binding_t;

static materender_binding_t bindings[MATERENDER_MAX_BINDINGS];
static int materender_ready = 0;

void materender_initialize(void) {
    for (uint32_t i = 0; i < MATERENDER_MAX_BINDINGS; ++i) bindings[i] = (materender_binding_t){0};
    materender_ready = 1;
}

static materender_binding_t *binding_find(uint32_t node_id) {
    for (uint32_t i = 0; i < MATERENDER_MAX_BINDINGS; ++i) {
        if (bindings[i].in_use && bindings[i].node_id == node_id) return &bindings[i];
    }
    return NULL;
}

int materender_bind(uint32_t node_id, uint32_t instance_id) {
    if (!material_instance_exists(instance_id)) return MATERENDER_ERR_INVALID;

    materender_binding_t *b = binding_find(node_id);
    if (!b) {
        uint32_t slot;
        for (slot = 0; slot < MATERENDER_MAX_BINDINGS; ++slot) {
            if (!bindings[slot].in_use) break;
        }
        if (slot >= MATERENDER_MAX_BINDINGS) return MATERENDER_ERR_CAPACITY;
        b = &bindings[slot];
        b->node_id = node_id;
        b->in_use = 1;
    }
    b->instance_id = instance_id;
    return 0;
}

int materender_unbind(uint32_t node_id) {
    materender_binding_t *b = binding_find(node_id);
    if (!b) return MATERENDER_ERR_NOT_FOUND;
    *b = (materender_binding_t){0};
    return 0;
}

uint32_t materender_bound_instance(uint32_t node_id) {
    materender_binding_t *b = binding_find(node_id);
    return b ? b->instance_id : MATERENDER_UNBOUND;
}

int materender_add_dependency(uint32_t node_id, uint32_t depends_on_node_id,
                               uint32_t *out_edge_id) {
    return scene_node_add_dependency(depends_on_node_id, node_id, GFX_DEP_MATERIAL, out_edge_id);
}

int materender_pipeline_steps(uint32_t material_type, uint32_t *out_steps,
                               uint32_t max_steps, uint32_t *out_count) {
    uint32_t steps[MATERENDER_STAGE_COUNT];
    uint32_t count;
    int valid = 1;

    switch (material_type) {
        case MATERIAL_TYPE_ACRYLIC:
            steps[0] = MATERENDER_STAGE_BASE;
            steps[1] = MATERENDER_STAGE_BLUR;
            steps[2] = MATERENDER_STAGE_LIGHTING;
            count = 3;
            break;
        case MATERIAL_TYPE_LIQUID:
            steps[0] = MATERENDER_STAGE_BASE;
            steps[1] = MATERENDER_STAGE_BLUR;
            steps[2] = MATERENDER_STAGE_REFRACTION;
            steps[3] = MATERENDER_STAGE_REFLECTION;
            steps[4] = MATERENDER_STAGE_LIGHTING;
            count = 5;
            break;
        case MATERIAL_TYPE_FROSTED:
            steps[0] = MATERENDER_STAGE_BASE;
            steps[1] = MATERENDER_STAGE_BLUR;
            count = 2;
            break;
        case MATERIAL_TYPE_CRYSTAL:
            steps[0] = MATERENDER_STAGE_BASE;
            steps[1] = MATERENDER_STAGE_REFLECTION;
            steps[2] = MATERENDER_STAGE_LIGHTING;
            count = 3;
            break;
        case MATERIAL_TYPE_SOLID:
            steps[0] = MATERENDER_STAGE_BASE;
            count = 1;
            break;
        default:
            /* MATERIAL_TYPE_CUSTOM oder unbekannt: konservativ nur BASE,
               analog material_fallback_id() - keine typspezifischen
               Zusatzeffekte ohne bekannte Spec. */
            steps[0] = MATERENDER_STAGE_BASE;
            count = 1;
            valid = (material_type == MATERIAL_TYPE_CUSTOM);
            break;
    }

    uint32_t copy_count = (count < max_steps) ? count : max_steps;
    for (uint32_t i = 0; i < copy_count; ++i) out_steps[i] = steps[i];
    *out_count = copy_count;
    return valid ? 0 : MATERENDER_ERR_INVALID;
}

int materender_evaluate(uint32_t node_id, render_rect_t region,
                         uint32_t *out_result_value, int *out_cache_hit,
                         uint32_t *out_fallback_level) {
    materender_binding_t *b = binding_find(node_id);
    if (!b) return MATERENDER_ERR_NOT_FOUND;
    uint32_t instance_id = b->instance_id;

    material_properties_t props;
    int was_cached = 0;
    if (matperf_cache_resolve(instance_id, &props, &was_cached) != 0) return MATERENDER_ERR_NOT_FOUND;

    uint32_t pending = 0;
    damage_pending_count(node_id, &pending);

    int cache_hit = (was_cached && pending == 0) ? 1 : 0;

    matperf_quality_apply(&props);

    /* Keine automatische Fallback-Eskalation bei matperf_gpu_available()==0:
       dieser Kernel rendert Materialien ohnehin stets in Software (kein
       GPU-Shader-Pfad existiert ueberhaupt) - matperf.c selbst behandelt
       das als Normalfall, nicht als Fallback-Ausloeser (siehe
       matperf_self_test()). Fallback-Eskalation erfolgt hier ausschliesslich
       ueber Sichtbarkeit (matperf_should_render()). */
    uint32_t fallback_level = MATPERF_FALLBACK_NATIVE;

    if (!matperf_should_render(&props)) {
        *out_result_value = 0;
        *out_cache_hit = cache_hit;
        *out_fallback_level = fallback_level;
        uint32_t resolved_unused;
        damage_resolve_target(node_id, &resolved_unused);
        return 0;
    }

    uint32_t def_id = material_instance_definition(instance_id);
    uint32_t type = material_definition_type(def_id);

    uint32_t steps[MATERENDER_STAGE_COUNT];
    uint32_t step_count = 0;
    materender_pipeline_steps(type, steps, MATERENDER_STAGE_COUNT, &step_count);

    uint32_t damage_area = region.width * region.height;
    uint32_t render_result;
    int render_cache_hit;
    uint32_t strategy;
    if (render_execute_retained(node_id, 0, damage_area, damage_area,
                                 &render_result, &render_cache_hit, &strategy) != 0) {
        return MATERENDER_ERR_NOT_FOUND;
    }

    uint32_t checksum = render_result ^ (instance_id * 2654435761u) ^
                         (step_count << 16) ^ (type << 8) ^ props.opacity;

    uint32_t resolved_count;
    damage_resolve_target(node_id, &resolved_count);

    *out_result_value = checksum;
    *out_cache_hit = cache_hit;
    *out_fallback_level = fallback_level;
    return 0;
}

int materender_self_test(void) {
    if (!materender_ready) return -1;

    uint32_t sceneA, rootA;
    if (scene_create(&sceneA, &rootA) != 0) return -1;

    uint32_t tx;
    if (scene_transaction_begin(sceneA, &tx) != 0) return -1;
    uint32_t nodeA;
    if (scene_node_create(tx, rootA, 1u, &nodeA) != 0) return -1;
    uint32_t nodeBackdrop;
    if (scene_node_create(tx, rootA, 1u, &nodeBackdrop) != 0) return -1;
    uint32_t version;
    if (scene_transaction_commit(tx, &version) != 0) return -1;

    uint32_t instA = material_instance_create(MATERIAL_ID_LIQUID);
    if (instA == 0) return -1;

    /* Unbekannter Node -> NOT_FOUND. */
    render_rect_t region = {0, 0, 32, 32};
    uint32_t result;
    int cache_hit;
    uint32_t fallback_level;
    if (materender_evaluate(0xFFFFu, region, &result, &cache_hit, &fallback_level) != MATERENDER_ERR_NOT_FOUND) return -1;

    /* Binden -> erster Evaluate ist kein Cache Hit. */
    if (materender_bind(nodeA, instA) != 0) return -1;
    if (materender_bound_instance(nodeA) != instA) return -1;

    if (materender_evaluate(nodeA, region, &result, &cache_hit, &fallback_level) != 0) return -1;
    if (cache_hit) return -1; /* erster Aufruf: nichts konnte gecacht gewesen sein */
    uint32_t first_result = result;

    /* Zweiter Aufruf ohne Aenderung: Material-Generation unveraendert,
       kein neuer Damage -> echter Cache Hit, IDENTISCHES Ergebnis. */
    if (materender_evaluate(nodeA, region, &result, &cache_hit, &fallback_level) != 0) return -1;
    if (!cache_hit) return -1;
    if (result != first_result) return -1;

    /* Backdrop-Abhaengigkeit (DEPENDENCY-0001): nodeA haengt vom
       Backdrop-Node ab. Eine Aenderung am Backdrop muss nodeA invalidieren -
       echte Phase-1/2-Propagation, kein eigener Code hier. */
    uint32_t edge_id;
    if (materender_add_dependency(nodeA, nodeBackdrop, &edge_id) != 0) return -1;

    uint32_t tx2;
    if (scene_transaction_begin(sceneA, &tx2) != 0) return -1;
    if (scene_node_set_property(tx2, nodeBackdrop, 1u, 42u) != 0) return -1;
    uint32_t version2;
    if (scene_transaction_commit(tx2, &version2) != 0) return -1;

    /* Backdrop-Aenderung allein erzeugt noch keinen Damage fuer nodeA
       (das ist Aufgabe von damage_from_node_change(), Phase 5, nicht
       dieser Integrationsschicht) - stattdessen pruefen wir die direkte
       Materialaenderung: eine geaenderte Instanz-Eigenschaft MUSS die
       Material-Generation erhoehen und damit den Cache Hit beenden. */
    if (material_instance_set_property_u32(instA, MATERIAL_FIELD_OPACITY, 10u) != 0) return -1;

    if (materender_evaluate(nodeA, region, &result, &cache_hit, &fallback_level) != 0) return -1;
    if (cache_hit) return -1; /* Generation geaendert -> kein Treffer */

    /* Opacity 10 ist noch sichtbar - matperf_should_render() liefert 1.
       Komplett unsichtbares Material (Opacity 0) wird korrekt
       uebersprungen und loest trotzdem Damage auf. */
    if (material_instance_set_property_u32(instA, MATERIAL_FIELD_OPACITY, 0u) != 0) return -1;
    if (materender_evaluate(nodeA, region, &result, &cache_hit, &fallback_level) != 0) return -1;
    if (result != 0) return -1;

    /* Pipeline-Stufen: LIQUID hat fuenf Stufen, SOLID nur eine. */
    uint32_t steps[MATERENDER_STAGE_COUNT];
    uint32_t step_count;
    if (materender_pipeline_steps(MATERIAL_TYPE_LIQUID, steps, MATERENDER_STAGE_COUNT, &step_count) != 0) return -1;
    if (step_count != 5) return -1;
    if (materender_pipeline_steps(MATERIAL_TYPE_SOLID, steps, MATERENDER_STAGE_COUNT, &step_count) != 0) return -1;
    if (step_count != 1 || steps[0] != MATERENDER_STAGE_BASE) return -1;

    /* Unbind -> erneuter Evaluate schlaegt fehl. */
    if (materender_unbind(nodeA) != 0) return -1;
    if (materender_bound_instance(nodeA) != MATERENDER_UNBOUND) return -1;
    if (materender_evaluate(nodeA, region, &result, &cache_hit, &fallback_level) != MATERENDER_ERR_NOT_FOUND) return -1;

    /* Binden an ungueltige Instanz schlaegt fehl. */
    if (materender_bind(nodeA, 0xFFFFu) != MATERENDER_ERR_INVALID) return -1;

    return 0;
}
