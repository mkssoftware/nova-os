#include "nova/ui/runtime.h"

#include <stdio.h>
#include <string.h>

static int failures;

static void expect(bool condition, const char *message) {
    if (condition) {
        printf("[OK] %s\n", message);
    } else {
        fprintf(stderr, "[FAIL] %s\n", message);
        ++failures;
    }
}

static nova_ui_node_t node(uint64_t id, uint64_t parent, int x, int y,
                           int width, int height, int z, const char *label) {
    nova_ui_node_t result;
    memset(&result, 0, sizeof(result));
    result.id.value = id;
    result.parent.value = parent;
    result.desired_bounds = (nova_ui_rect_t){x, y, width, height};
    result.z_order = z;
    result.opacity_milli = 1000u;
    result.state = NOVA_UI_STATE_AVAILABLE;
    result.role = NOVA_UI_ROLE_CONTENT;
    result.desired_visible = true;
    result.background_token = NOVA_UI_COLOR_SURFACE_PRIMARY;
    result.foreground_token = NOVA_UI_COLOR_TEXT_PRIMARY;
    (void)snprintf(result.label, sizeof(result.label), "%s", label);
    return result;
}

int main(void) {
    nova_ui_runtime_t runtime;
    nova_ui_node_t root;
    nova_ui_node_t action;
    nova_ui_node_t overlay;
    nova_ui_display_t display;
    nova_ui_surface_t app_surface;
    nova_ui_window_t app_window;
    nova_ui_capability_t edit_capability;
    nova_ui_capability_t raw_capability;
    nova_ui_contribution_t ribbon;
    nova_ui_input_event_t input;
    nova_ui_input_route_t route;
    nova_ui_color_t dark;
    nova_ui_rect_t world_bounds;
    size_t generation;

    nova_ui_runtime_init(&runtime);
    dark = nova_ui_theme_color(&runtime, NOVA_UI_COLOR_SURFACE_PRIMARY);
    expect(runtime.theme.kind == NOVA_UI_THEME_DARK && dark.alpha == 255u,
           "semantisches Dark-Theme ist initial aktiv");
    expect(nova_ui_theme_apply(&runtime, NOVA_UI_THEME_HIGH_CONTRAST, false,
                               false, 1250u) == NOVA_UI_OK &&
               runtime.theme.reduce_motion && runtime.theme.reduce_transparency &&
               runtime.theme.text_scale_milli == 1250u,
           "Accessibility überschreibt dekorative Theme-Eigenschaften");

    root = node(10u, 0u, 0, 0, 800, 600, 0, "Desktop");
    root.role = NOVA_UI_ROLE_DESKTOP;
    root.transform.translate_x_dlu = 5;
    action = node(11u, 10u, 20, 20, 120, 32, 1, "Bild bearbeiten");
    action.role = NOVA_UI_ROLE_PRIMARY_ACTION;
    action.action = NOVA_UI_ACTION_INVOKE;
    action.target_object.value = 77u;
    action.required_capability.value = 200u;
    overlay = node(12u, 10u, 15, 15, 150, 80, 9, "Systemhinweis");
    overlay.role = NOVA_UI_ROLE_INFORMATION;
    overlay.system_layer = true;
    expect(nova_ui_node_upsert(&runtime, &root) == NOVA_UI_OK &&
               nova_ui_node_upsert(&runtime, &action) == NOVA_UI_OK &&
               nova_ui_node_upsert(&runtime, &overlay) == NOVA_UI_OK,
           "deklarative Nodes mit stabilen semantischen IDs angelegt");
    expect(nova_ui_reconcile(&runtime) == NOVA_UI_OK &&
               runtime.scene_damage_count != 0u &&
               runtime.nodes[1].actual_visible,
           "Desired State inkrementell in Actual State reconciled");
    expect(nova_ui_scene_publish(&runtime) == NOVA_UI_OK &&
               runtime.diagnostics.scene_generation == 1u,
           "konsistente Scene-Generation atomar veröffentlicht");
    expect(nova_ui_scene_world_bounds(&runtime, (nova_ui_node_id_t){11u},
                                      &world_bounds) == NOVA_UI_OK &&
               world_bounds.x == 25 && world_bounds.y == 20,
           "hierarchische Scene-Transformation wird kombiniert");
    generation = (size_t)runtime.diagnostics.scene_generation;
    expect(nova_ui_accessibility_rebuild(&runtime) == NOVA_UI_OK &&
               runtime.accessibility_count == 3u &&
               (size_t)runtime.diagnostics.scene_generation == generation,
           "Accessibility Tree bleibt vom Scene Graph getrennt");
    expect(nova_ui_accessibility_set_virtual_range(
               &runtime, (nova_ui_accessibility_id_t){10u}, 10000u, 400u,
               20u) == NOVA_UI_OK,
           "virtualisierte Accessibility-Inhalte ohne Materialisierung");
    expect(nova_ui_set_accessibility_focus(&runtime,
                                           (nova_ui_node_id_t){12u}) == NOVA_UI_OK &&
               nova_ui_set_keyboard_focus(&runtime,
                                          (nova_ui_node_id_t){11u}) == NOVA_UI_OK &&
               runtime.accessibility_focus.value != runtime.keyboard_focus.value,
           "Accessibility- und Keyboard-Fokus sind unabhängig");

    memset(&display, 0, sizeof(display));
    display.id.value = 300u;
    display.mode = (nova_ui_display_mode_t){1920u, 1080u, 60000u};
    display.scale_milli = 1250u;
    display.vrr_supported = true;
    display.vrr_min_millihz = 48000u;
    display.vrr_max_millihz = 144000u;
    expect(nova_ui_display_connect(&runtime, display) == NOVA_UI_OK &&
               nova_ui_display_set_vrr(&runtime, display.id, true) == NOVA_UI_OK,
           "Display Server verwaltet Scaling und VRR pro Display");

    memset(&app_surface, 0, sizeof(app_surface));
    app_surface.id.value = 400u;
    app_surface.owner.value = 500u;
    app_surface.width = 800u;
    app_surface.height = 600u;
    app_surface.stride = 3200u;
    app_surface.format = NOVA_UI_PIXEL_ARGB8888;
    app_surface.visible = true;
    app_surface.scanout_capable = true;
    expect(nova_ui_surface_create(&runtime, app_surface) == NOVA_UI_OK &&
               nova_ui_surface_damage(&runtime, app_surface.id,
                                      (nova_ui_rect_t){0, 0, 100, 100}) == NOVA_UI_OK,
           "isolierte Surface akzeptiert begrenzte Damage Region");
    expect(nova_ui_surface_damage(&runtime, app_surface.id,
                                  (nova_ui_rect_t){790, 0, 20, 20}) ==
               NOVA_UI_INVALID,
           "ungültiger Surface-Schaden wird isoliert");
    runtime.surfaces[0].valid = true;

    memset(&app_window, 0, sizeof(app_window));
    app_window.id.value = 600u;
    app_window.owner = app_surface.owner;
    app_window.surface = app_surface.id;
    app_window.display = display.id;
    app_window.bounds = (nova_ui_rect_t){0, 0, 800, 600};
    app_window.state = NOVA_UI_WINDOW_FULLSCREEN;
    expect(nova_ui_window_create(&runtime, app_window) == NOVA_UI_OK,
           "Window Server bindet WindowID kontrolliert an Owner und SurfaceID");
    expect(nova_ui_compose(&runtime, display.id) ==
               NOVA_UI_COMPOSE_DIRECT_SCANOUT,
           "Direct Scanout für einzelne geeignete Fullscreen-Surface");
    runtime.windows[0].state = NOVA_UI_WINDOW_NORMAL;
    runtime.surfaces[0].scanout_capable = false;
    runtime.gpu_available = true;
    expect(nova_ui_compose(&runtime, display.id) == NOVA_UI_COMPOSE_GPU,
           "GPU-Composition wird über hardwareunabhängigen Pfad bevorzugt");
    runtime.resource_pressure = true;
    expect(nova_ui_compose(&runtime, display.id) == NOVA_UI_COMPOSE_SOFTWARE,
           "Software-Composition bleibt sicherer Fallback");

    memset(&edit_capability, 0, sizeof(edit_capability));
    edit_capability.id.value = 200u;
    edit_capability.provider.value = 700u;
    edit_capability.discoverable = true;
    edit_capability.available = true;
    (void)snprintf(edit_capability.semantic_type,
                   sizeof(edit_capability.semantic_type), "Image.Edit");
    expect(nova_ui_capability_register(&runtime, edit_capability) == NOVA_UI_OK &&
               !nova_ui_capability_can_execute(&runtime, edit_capability.id),
           "Discovery einer Capability autorisiert sie nicht");
    expect(nova_ui_accessibility_invoke(
               &runtime, (nova_ui_accessibility_id_t){11u},
               NOVA_UI_ACTION_INVOKE) == NOVA_UI_DENIED &&
               nova_ui_capability_authorize(&runtime, edit_capability.id, true) ==
                   NOVA_UI_OK &&
               nova_ui_accessibility_invoke(
                   &runtime, (nova_ui_accessibility_id_t){11u},
                   NOVA_UI_ACTION_INVOKE) == NOVA_UI_OK,
           "semantische Aktion prüft Capability erst bei Ausführung");

    memset(&raw_capability, 0, sizeof(raw_capability));
    raw_capability.id.value = 1u;
    raw_capability.provider.value = 701u;
    raw_capability.discoverable = false;
    raw_capability.available = true;
    (void)snprintf(raw_capability.semantic_type,
                   sizeof(raw_capability.semantic_type), "UI.Input.Raw");
    expect(nova_ui_capability_register(&runtime, raw_capability) == NOVA_UI_OK,
           "Raw-Input-Capability registriert");

    memset(&ribbon, 0, sizeof(ribbon));
    ribbon.id = 800u;
    ribbon.kind = NOVA_UI_CONTRIBUTION_RIBBON;
    ribbon.provider = edit_capability.provider;
    ribbon.capability = edit_capability.id;
    ribbon.action = NOVA_UI_ACTION_INVOKE;
    ribbon.priority = 100u;
    ribbon.visible = true;
    (void)snprintf(ribbon.label, sizeof(ribbon.label), "Bearbeiten");
    expect(nova_ui_contribution_add(&runtime, ribbon) == NOVA_UI_OK &&
               nova_ui_contribution_count_visible(
                   &runtime, NOVA_UI_CONTRIBUTION_RIBBON) == 1u,
           "kontrollierte Ribbon-Capability-Contribution sichtbar");
    ribbon.id = 801u;
    ribbon.provider.value = 702u;
    expect(nova_ui_contribution_add(&runtime, ribbon) == NOVA_UI_CONFLICT,
           "äquivalente Provider erzeugen keine doppelte Ribbon-Aktion");
    ribbon.id = 802u;
    ribbon.provider = edit_capability.provider;
    ribbon.kind = NOVA_UI_CONTRIBUTION_TASKBAR;
    (void)snprintf(ribbon.label, sizeof(ribbon.label), "Bild bearbeiten");
    expect(nova_ui_contribution_add(&runtime, ribbon) == NOVA_UI_OK &&
               nova_ui_contribution_count_visible(
                   &runtime, NOVA_UI_CONTRIBUTION_TASKBAR) == 1u,
           "Taskleiste übernimmt kontrollierte semantische Contributions");

    memset(&input, 0, sizeof(input));
    input.type = NOVA_UI_INPUT_POINTER_BUTTON;
    input.monotonic_ns = 100u;
    input.session_id = 1u;
    input.x_dlu = 25;
    input.y_dlu = 25;
    input.pressed = true;
    expect(nova_ui_route_input(&runtime, &input, &route) == NOVA_UI_OK &&
               route.node.value == 12u,
           "Input Router berücksichtigt Z-Order beim Hit-Test");
    input.raw = true;
    expect(nova_ui_route_input(&runtime, &input, &route) == NOVA_UI_DENIED &&
               nova_ui_capability_authorize(&runtime, raw_capability.id, true) ==
                   NOVA_UI_OK &&
               nova_ui_route_input(&runtime, &input, &route) == NOVA_UI_OK,
           "Raw Input erfordert explizit autorisierte semantische Capability");
    input.raw = false;
    input.session_id = 2u;
    expect(nova_ui_route_input(&runtime, &input, &route) == NOVA_UI_DENIED,
           "Input Streams unterschiedlicher Sessions bleiben isoliert");
    expect(nova_ui_capture_pointer(&runtime, app_surface.owner,
                                   (nova_ui_node_id_t){11u}) == NOVA_UI_OK,
           "Pointer Capture ist explizit und owner-gebunden");
    nova_ui_release_owner(&runtime, app_surface.owner);
    expect(runtime.pointer_capture.value == 0u && !runtime.windows[0].alive,
           "Owner-Ausfall gibt Capture frei und isoliert sein Fenster");

    nova_ui_responsive_layout(&runtime, 90, true);
    expect(runtime.nodes[1].desired_bounds.width == 90 &&
               runtime.nodes[1].desired_bounds.height >= 24,
           "responsive DLU-Layout berücksichtigt Touch-Mindestflächen");
    expect(!nova_ui_adaptive_change_allowed(true, false, 999u, false) &&
               !nova_ui_adaptive_change_allowed(false, true, 999u, false) &&
               nova_ui_adaptive_change_allowed(false, false, 900u, false),
           "Nutzerwahl und Accessibility gewinnen gegen adaptive Vorhersage");

    nova_ui_request_frame(&runtime, false);
    nova_ui_request_frame(&runtime, false);
    nova_ui_request_frame(&runtime, true);
    expect(runtime.scheduler.queued_frames == 2u &&
               runtime.scheduler.coalesced_requests != 0u &&
               nova_ui_frame_due(&runtime, 1000000u, display.id),
           "Frame Scheduler begrenzt Queue, bündelt Arbeit und priorisiert Input");
    nova_ui_frame_presented(&runtime, 1000000u, false);
    expect(runtime.displays[0].current_refresh_millihz == 144000u,
           "VRR nutzt für interaktive Frames geringe Presentation-Latenz");

    expect(nova_ui_provider_failed(&runtime, edit_capability.provider) == NOVA_UI_OK &&
               nova_ui_contribution_count_visible(
                   &runtime, NOVA_UI_CONTRIBUTION_RIBBON) == 0u &&
               nova_ui_contribution_count_visible(
                   &runtime, NOVA_UI_CONTRIBUTION_TASKBAR) == 0u &&
               runtime.diagnostics.isolated_provider_failures == 1u,
           "Provider-Ausfall entfernt Ribbon und Taskleiste ohne Ausfall der UI");
    expect(nova_ui_display_disconnect(&runtime, display.id) == NOVA_UI_OK,
           "Display-Hot-Unplug wird kontrolliert reconciled");

    if (failures != 0) {
        fprintf(stderr, "%d UI-Architekturtests fehlgeschlagen\n", failures);
        return 1;
    }
    puts("Nova UI architecture runtime: alle Tests bestanden");
    return 0;
}
