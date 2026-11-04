#include "gfxoptimize.h"
#include "scenegraph.h"
#include "layercompose.h"
#include "damagetrack.h"
#include "framesched.h"
#include <stddef.h>

static uint32_t current_profile = GFXOPT_QUALITY_BALANCED;
static int gfxoptimize_ready = 0;

/* Profil -> Frame-Scheduler-Power-Modus (framesched.h). Reine Abbildung,
   keine neue Logik (siehe Dateikopf). */
static uint32_t profile_power_mode(uint32_t profile) {
    switch (profile) {
        case GFXOPT_QUALITY_ULTRA:
        case GFXOPT_QUALITY_HIGH:
            return POWER_PERFORMANCE;
        case GFXOPT_QUALITY_POWER_SAVING:
        case GFXOPT_QUALITY_MINIMAL:
            return POWER_SAVING;
        case GFXOPT_QUALITY_BALANCED:
        case GFXOPT_QUALITY_PERFORMANCE:
        case GFXOPT_QUALITY_CUSTOM:
        default:
            return POWER_BALANCED;
    }
}

void gfxoptimize_initialize(void) {
    current_profile = GFXOPT_QUALITY_BALANCED;
    gfxoptimize_ready = 1;
}

int gfxoptimize_should_render(uint32_t node_id, uint32_t layer_id, uint32_t region_id,
                               int *out_should_render) {
    scene_node_info_t ninfo;
    if (scene_node_info(node_id, &ninfo) != 0) return GFXOPTIMIZE_ERR_NOT_FOUND;

    if (layer_id == 0) {
        /* Keine Layer-Zuordnung -> keine Occlusion-Pruefung moeglich,
           konservativ immer rendern (#19 konservative Behandlung). */
        *out_should_render = 1;
        return 0;
    }

    layer_info_t linfo;
    if (layer_info(layer_id, &linfo) != 0) {
        *out_should_render = 1; /* Layer unbekannt -> konservativ rendern */
        return 0;
    }

    if (linfo.visibility_state != LAYER_OCCLUDED) {
        *out_should_render = 1;
        return 0;
    }

    /* Vollstaendig okkludiert: nur dann sicher ueberspringen, wenn
       weder ausstehender Damage noch ein fehlender Cache-Treffer eine
       Neuberechnung erzwingen wuerde. Verdeckte Damage-Informationen
       werden hierbei NICHT aufgeloest (OCCLUSION-0001 §8) - diese
       Funktion ist eine reine Pruefung, kein Zustandsuebergang. */
    uint32_t pending = 0;
    damage_pending_count(node_id, &pending);
    if (pending > 0) {
        *out_should_render = 1;
        return 0;
    }

    uint32_t cache_id;
    if (render_cache_find(node_id, region_id, &cache_id) != 0) {
        *out_should_render = 1; /* kein Treffer -> konservativ rendern */
        return 0;
    }

    *out_should_render = 0;
    return 0;
}

int gfxoptimize_try_reuse(uint32_t node_id, uint32_t region_id, uint32_t *out_value) {
    uint32_t cache_id;
    if (render_cache_find(node_id, region_id, &cache_id) != 0) return GFXOPTIMIZE_ERR_NOT_FOUND;

    render_cache_info_t info;
    if (render_cache_get(cache_id, &info) != 0) return GFXOPTIMIZE_ERR_NOT_FOUND;

    scene_node_info_t ninfo;
    if (scene_node_info(node_id, &ninfo) != 0) return GFXOPTIMIZE_ERR_NOT_FOUND;

    if (info.state_version != ninfo.state_version) return GFXOPTIMIZE_ERR_NOT_FOUND;

    *out_value = info.result_value;
    return 0;
}

int gfxoptimize_set_quality_profile(uint32_t profile) {
    if (profile >= GFXOPT_QUALITY_COUNT) return GFXOPTIMIZE_ERR_INVALID;
    current_profile = profile;
    return 0;
}

uint32_t gfxoptimize_quality_profile(void) {
    return current_profile;
}

int gfxoptimize_apply_profile_to_output(uint32_t output_id) {
    int rc = fsched_output_set_power_mode(output_id, profile_power_mode(current_profile));
    return (rc == 0) ? 0 : GFXOPTIMIZE_ERR_NOT_FOUND;
}

int gfxoptimize_power_should_render_frame(uint32_t output_id, uint32_t target_id,
                                           int *out_should_render) {
    int idle;
    if (fsched_output_is_idle(output_id, target_id, &idle) != 0) return GFXOPTIMIZE_ERR_NOT_FOUND;
    *out_should_render = !idle;
    return 0;
}

int gfxoptimize_vrs_available(void) { return 0; }

int gfxoptimize_self_test(void) {
    if (!gfxoptimize_ready) return -1;

    uint32_t sceneA, rootA;
    if (scene_create(&sceneA, &rootA) != 0) return -1;

    uint32_t tx;
    if (scene_transaction_begin(sceneA, &tx) != 0) return -1;
    uint32_t nodeA;
    if (scene_node_create(tx, rootA, 1u, &nodeA) != 0) return -1;
    uint32_t version;
    if (scene_transaction_commit(tx, &version) != 0) return -1;

    int should_render;

    /* Unbekannter Node -> NOT_FOUND. */
    if (gfxoptimize_should_render(0xFFFFu, 0, 0, &should_render) != GFXOPTIMIZE_ERR_NOT_FOUND) return -1;

    /* Keine Layer-Zuordnung (layer_id=0) -> konservativ immer rendern. */
    if (gfxoptimize_should_render(nodeA, 0, 0, &should_render) != 0) return -1;
    if (!should_render) return -1;

    /* Layer erzeugen, aber (noch) nicht okkludiert -> rendern. */
    uint32_t layerA;
    if (layer_create(0, LAYER_CATEGORY_APPLICATION, 0, nodeA,
                      (render_rect_t){0, 0, 50, 50}, 0, &layerA) != 0) return -1;
    if (layer_set_opaque(layerA, 1) != 0) return -1;

    if (gfxoptimize_should_render(nodeA, layerA, 0, &should_render) != 0) return -1;
    if (!should_render) return -1; /* noch kein Occluder vorhanden */

    /* Okkludierenden Layer davor legen - layerA wird vollstaendig
       verdeckt, hat aber weiterhin ausstehenden Damage (frisch erzeugt,
       nie aufgeloest) -> dennoch 1 (Damage erzwingt Rendern). */
    uint32_t layerOccluder;
    if (layer_create(0, LAYER_CATEGORY_APPLICATION, 0, 0,
                      (render_rect_t){0, 0, 50, 50}, 10, &layerOccluder) != 0) return -1;
    if (layer_set_opaque(layerOccluder, 1) != 0) return -1;
    if (compositor_occlusion_pass() != 0) return -1;

    uint32_t pending_region;
    if (damage_add(nodeA, (render_rect_t){0, 0, 50, 50}, version, &pending_region) != 0) return -1;

    if (gfxoptimize_should_render(nodeA, layerA, 0, &should_render) != 0) return -1;
    if (!should_render) return -1; /* okkludiert, aber Damage ausstehend */

    /* Damage aufloesen: jetzt vollstaendig okkludiert UND kein Damage UND
       (noch) kein Render-Cache-Treffer -> konservativ dennoch 1. */
    uint32_t resolved;
    if (damage_resolve_target(nodeA, &resolved) != 0) return -1;

    if (gfxoptimize_should_render(nodeA, layerA, 0, &should_render) != 0) return -1;
    if (!should_render) return -1; /* kein Cache-Treffer -> konservativ rendern */

    /* Render-Cache fuellen (echter Phase-3-Aufruf), danach: okkludiert +
       kein Damage + gueltiger Cache-Treffer -> jetzt sicher ueberspringbar. */
    uint32_t render_result;
    int render_cache_hit;
    uint32_t strategy;
    if (render_execute_retained(nodeA, 0, 0, 0, &render_result, &render_cache_hit, &strategy) != 0) return -1;

    if (gfxoptimize_should_render(nodeA, layerA, 0, &should_render) != 0) return -1;
    if (should_render) return -1; /* jetzt sicher ueberspringbar */

    /* Reuse-Wrapper liefert denselben Wert. */
    uint32_t reused_value;
    if (gfxoptimize_try_reuse(nodeA, 0, &reused_value) != 0) return -1;
    if (reused_value != render_result) return -1;

    /* Quality-Profile: setzen/lesen, ungueltiges Profil schlaegt fehl. */
    if (gfxoptimize_quality_profile() != GFXOPT_QUALITY_BALANCED) return -1;
    if (gfxoptimize_set_quality_profile(GFXOPT_QUALITY_POWER_SAVING) != 0) return -1;
    if (gfxoptimize_quality_profile() != GFXOPT_QUALITY_POWER_SAVING) return -1;
    if (gfxoptimize_set_quality_profile(0xFFu) != GFXOPTIMIZE_ERR_INVALID) return -1;

    /* Power-/Frame-Scheduler-Integration: echter Phase-8-Output. */
    uint32_t outputA = 9001u;
    if (fsched_output_register(outputA, 16u) != 0) return -1;

    if (gfxoptimize_apply_profile_to_output(outputA) != 0) return -1;
    output_sched_info_t oinfo;
    if (fsched_output_info(outputA, &oinfo) != 0) return -1;
    if (oinfo.power_mode != POWER_SAVING) return -1; /* POWER_SAVING-Profil */

    uint32_t targetY = 888001u;
    int power_should_render;
    if (gfxoptimize_power_should_render_frame(outputA, targetY, &power_should_render) != 0) return -1;
    if (power_should_render) return -1; /* kein Damage fuer targetY */

    uint32_t region_id2;
    if (damage_add(targetY, (render_rect_t){0, 0, 10, 10}, 1u, &region_id2) != 0) return -1;
    if (gfxoptimize_power_should_render_frame(outputA, targetY, &power_should_render) != 0) return -1;
    if (!power_should_render) return -1; /* Damage vorhanden */

    /* Unbekannte Ausgabe -> NOT_FOUND. */
    if (gfxoptimize_apply_profile_to_output(0xFFFFu) != GFXOPTIMIZE_ERR_NOT_FOUND) return -1;
    if (gfxoptimize_power_should_render_frame(0xFFFFu, targetY, &power_should_render) != GFXOPTIMIZE_ERR_NOT_FOUND) return -1;

    /* VRS ist in diesem Kernel nie verfuegbar. */
    if (gfxoptimize_vrs_available() != 0) return -1;

    return 0;
}
