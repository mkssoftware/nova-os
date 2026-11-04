#include "taskbarglass.h"
#include "scenegraph.h"
#include "materender.h"
#include "liquidglass.h"
#include "damagetrack.h"
#include "layercompose.h"
#include "framesched.h"
#include "gfxoptimize.h"
#include "gfxperf.h"
#include "material.h"
#include "matfx.h"
#include "render2d.h"
#include "surface.h"
#include "display.h"
#include <stddef.h>

/* Synthetischer Backdrop (siehe desktop_shell.c's gleichlautender
   Scope-Hinweis bei DESKTOP_SHELL_BG_RGB: dieser Kernel hat weder eine
   Bildtapete noch ueberlappende Fenster unter der Taskbar - der
   tatsaechliche Hintergrund ist IMMER diese bekannte Desktop-Farbe). */
#define TASKBARGLASS_BACKDROP_RGB 0x1E2430u

/* Feste, projektweit eindeutige Frame-Scheduler-Ausgabe fuer die Taskbar
   (siehe framesched.h - Ausgaben werden per Aufrufer-ID verwaltet, keine
   eigene Vergabe). Mindestintervall: ~100ms bei 100Hz PIT - deutlich
   dichter als shell_main.c's eigener ~Spin-Takt, drosselt also nur
   wirklich dichte Commit-Folgen. */
#define TASKBARGLASS_OUTPUT_ID       424242u
#define TASKBARGLASS_REFRESH_TICKS  10u
#define TASKBARGLASS_MIN_INTERVAL   10u

static uint32_t glass_surface_handle = 0;
static uint32_t glass_width = 0;

static uint32_t glass_scene_id, glass_root_node_id;
static uint32_t glass_node_id = 0, glass_backdrop_node_id = 0;
static uint32_t glass_material_instance = 0;
static uint32_t glass_layer_id = 0;

static uint32_t glass_bg_pixels[TASKBARGLASS_MAX_WIDTH * TASKBARGLASS_HEIGHT];
static uint32_t glass_backdrop_pixels[TASKBARGLASS_MAX_WIDTH * TASKBARGLASS_HEIGHT];
static int glass_bg_valid = 0;

static int glass_ready = 0;

void taskbarglass_initialize(void) {
    glass_surface_handle = 0;
    glass_width = 0;
    glass_bg_valid = 0;
    glass_node_id = 0;
    glass_backdrop_node_id = 0;
    glass_layer_id = 0;

    glass_material_instance = material_instance_create(MATERIAL_ID_LIQUID);
    if (glass_material_instance != 0) {
        material_instance_set_property_u32(glass_material_instance, MATERIAL_FIELD_TINT_RGB, TASKBARGLASS_BG_RGB);
        material_instance_set_property_u32(glass_material_instance, MATERIAL_FIELD_TINT_STRENGTH, 150u);
        material_instance_set_property_u32(glass_material_instance, MATERIAL_FIELD_OPACITY, 215u);
        material_instance_set_property_u32(glass_material_instance, MATERIAL_FIELD_BLUR, 4u);
        material_instance_set_property_u32(glass_material_instance, MATERIAL_FIELD_REFLECTION, 60u);
        material_instance_set_property_u32(glass_material_instance, MATERIAL_FIELD_HIGHLIGHT, 90u);
        material_instance_set_property_u32(glass_material_instance, MATERIAL_FIELD_DISTORTION, 20u);
        material_instance_set_property_u32(glass_material_instance, MATERIAL_FIELD_REFRACTION, 40u);
    }

    if (scene_create(&glass_scene_id, &glass_root_node_id) == 0) {
        uint32_t tx;
        if (scene_transaction_begin(glass_scene_id, &tx) == 0) {
            scene_node_create(tx, glass_root_node_id, 1u, &glass_node_id);
            scene_node_create(tx, glass_root_node_id, 1u, &glass_backdrop_node_id);
            uint32_t version;
            scene_transaction_commit(tx, &version);
        }
    }

    if (glass_node_id != 0 && glass_material_instance != 0) {
        materender_bind(glass_node_id, glass_material_instance);
        liquidglass_set_backdrop(glass_node_id, glass_backdrop_node_id);

        layer_create(0, LAYER_CATEGORY_SYSTEM, 0, glass_node_id,
                     (render_rect_t){0, 0, 0, 0}, 1000, &glass_layer_id);
        if (glass_layer_id != 0) layer_set_needs_backdrop(glass_layer_id, 1);
    }

    fsched_output_register(TASKBARGLASS_OUTPUT_ID, TASKBARGLASS_REFRESH_TICKS);
    fsched_output_set_throttle(TASKBARGLASS_OUTPUT_ID, TASKBARGLASS_MIN_INTERVAL);

    glass_ready = 1;
}

int taskbarglass_on_surface_created(uint32_t handle, uint32_t x, uint32_t y,
                                     uint32_t w, uint32_t h) {
    if (!glass_ready) return 0;
    if (h != TASKBARGLASS_HEIGHT || x != 0u) return 0;

    uint32_t disp_h = display_height();
    if (disp_h == 0u || y != disp_h - TASKBARGLASS_HEIGHT) return 0;
    if (w == 0u || w > TASKBARGLASS_MAX_WIDTH) return 0;

    glass_surface_handle = handle;
    glass_width = w;
    glass_bg_valid = 0;

    if (glass_layer_id != 0) layer_set_bounds(glass_layer_id, (render_rect_t){x, y, w, h});
    return 1;
}

static void glass_recompute_background(void) {
    material_properties_t props;
    if (material_instance_resolve(glass_material_instance, &props) != 0) return;

    render2d_surface_t bg_surf, backdrop_surf;
    render2d_init_surface(&bg_surf, glass_bg_pixels, glass_width, TASKBARGLASS_HEIGHT, glass_width);
    render2d_init_surface(&backdrop_surf, glass_backdrop_pixels, glass_width, TASKBARGLASS_HEIGHT, glass_width);

    render2d_fill_rect(&backdrop_surf, 0u, 0u, glass_width, TASKBARGLASS_HEIGHT, TASKBARGLASS_BACKDROP_RGB);
    render2d_fill_rect(&bg_surf, 0u, 0u, glass_width, TASKBARGLASS_HEIGHT, TASKBARGLASS_BG_RGB);
    matfx_apply(&bg_surf, 0u, 0u, glass_width, TASKBARGLASS_HEIGHT,
                &backdrop_surf, 0u, 0u, &props, MATFX_BLEND_NORMAL);

    glass_bg_valid = 1;
}

int taskbarglass_on_commit(uint32_t handle) {
    if (!glass_ready || handle != glass_surface_handle || glass_surface_handle == 0) return 0;

    int should_render;
    if (gfxoptimize_should_render(glass_node_id, glass_layer_id, 0, &should_render) == 0 && !should_render) {
        return 1;
    }

    uint32_t region_id;
    damage_add(glass_node_id, (render_rect_t){0, 0, glass_width, TASKBARGLASS_HEIGHT}, 1u, &region_id);

    uint32_t frame_id;
    int scheduled;
    if (fsched_frame_request(TASKBARGLASS_OUTPUT_ID, glass_node_id, 1u, &frame_id, &scheduled) != 0) return 1;
    if (!scheduled) return 1; /* gedrosselt - naechster zugelassener Commit holt die Substitution nach */

    fsched_frame_render_start(frame_id);
    uint32_t perf_span;
    gfxperf_begin(GFXPERF_SUBSYS_COMPOSITOR, frame_id, &perf_span);

    render_rect_t region = {0, 0, glass_width, TASKBARGLASS_HEIGHT};
    uint32_t material_result;
    int material_cache_hit = 0;
    uint32_t fallback_level;
    int have_material = (glass_material_instance != 0 &&
                          materender_evaluate(glass_node_id, region, &material_result,
                                               &material_cache_hit, &fallback_level) == 0);

    if (have_material && (!glass_bg_valid || !material_cache_hit)) {
        glass_recompute_background();
    }

    if (glass_bg_valid) {
        uint32_t buffer_addr = surface_buffer_addr(handle);
        if (buffer_addr != 0) {
            uint32_t *pixels = (uint32_t *)buffer_addr;
            uint32_t count = glass_width * TASKBARGLASS_HEIGHT;
            for (uint32_t i = 0; i < count; ++i) {
                if (pixels[i] == TASKBARGLASS_BG_RGB) pixels[i] = glass_bg_pixels[i];
            }
        }
    }

    uint32_t perf_duration;
    gfxperf_end(perf_span, &perf_duration);
    fsched_frame_render_complete(frame_id);
    fsched_frame_mark_presented(frame_id);

    uint32_t resolved_count;
    damage_resolve_target(glass_node_id, &resolved_count);

    return 1;
}

int taskbarglass_self_test(void) {
    if (!glass_ready) return -1;
    if (glass_node_id == 0 || glass_material_instance == 0) return -1;

    /* Geometrie-Erkennung: nur exakt die Taskbar-Geometrie wird erkannt. */
    uint32_t disp_h = display_height();
    if (disp_h == 0u || disp_h <= TASKBARGLASS_HEIGHT) return -1;
    uint32_t taskbar_y = disp_h - TASKBARGLASS_HEIGHT;

    uint32_t fake_handle;
    if (surface_create(1u, 0u, 100u, 64u, 64u, &fake_handle) != 0) return -1;
    if (taskbarglass_on_surface_created(fake_handle, 0u, 100u, 64u, 64u) != 0) return -1; /* falsche Geometrie */

    uint32_t handle;
    if (surface_create(1u, 0u, taskbar_y, 200u, TASKBARGLASS_HEIGHT, &handle) != 0) return -1;
    if (surface_configure(handle, 0u, taskbar_y, 200u, TASKBARGLASS_HEIGHT) != 0) return -1;
    if (taskbarglass_on_surface_created(handle, 0u, taskbar_y, 200u, TASKBARGLASS_HEIGHT) != 1) return -1;
    if (glass_surface_handle != handle) return -1;
    if (glass_width != 200u) return -1;

    /* Kommittierung einer FALSCHEN Surface -> No-Op (0). */
    if (taskbarglass_on_commit(fake_handle) != 0) return -1;

    /* Puffer anheften und mit flacher Fuellfarbe + einem "Start"-Fleck
       simulieren, wie ihn ring3's draw_taskbar() tatsaechlich erzeugt. */
    static uint32_t test_buffer[200u * TASKBARGLASS_HEIGHT];
    for (uint32_t i = 0; i < 200u * TASKBARGLASS_HEIGHT; ++i) test_buffer[i] = TASKBARGLASS_BG_RGB;
    for (uint32_t y = 0; y < TASKBARGLASS_HEIGHT; ++y) {
        for (uint32_t x = 0; x < 64u; ++x) test_buffer[y * 200u + x] = 0x2D5FA0u; /* Start-Button */
    }
    if (surface_attach_buffer(handle, (uint32_t)(uintptr_t)test_buffer) != 0) return -1;

    if (taskbarglass_on_commit(handle) != 1) return -1;
    if (!glass_bg_valid) return -1;

    /* Start-Button-Pixel bleiben UNVERAENDERT (scharf), Hintergrund-Pixel
       wurden durch den Glas-Hintergrund ersetzt (ungleich der flachen
       Originalfarbe, sofern das Material tatsaechlich wirkt). */
    if (test_buffer[0] != 0x2D5FA0u) return -1;
    int any_substituted = 0;
    for (uint32_t x = 70u; x < 200u; ++x) {
        if (test_buffer[x] != TASKBARGLASS_BG_RGB) { any_substituted = 1; break; }
    }
    if (!any_substituted) return -1;

    /* Zweiter Commit unmittelbar danach: durch das Mindestintervall
       gedrosselt (TASKBARGLASS_MIN_INTERVAL) - liefert weiterhin 1 (kein
       Fehler), aendert aber nichts Weiteres (kein Crash/kein Fehlerpfad). */
    if (taskbarglass_on_commit(handle) != 1) return -1;

    return 0;
}
