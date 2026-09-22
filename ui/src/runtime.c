#include "nova/ui/runtime.h"

#include <string.h>

static bool id_valid(uint64_t value) { return value != 0u; }
static bool node_id_equal(nova_ui_node_id_t a, nova_ui_node_id_t b) {
    return a.value == b.value;
}
static bool owner_id_equal(nova_ui_owner_id_t a, nova_ui_owner_id_t b) {
    return a.value == b.value;
}
static bool rect_valid(nova_ui_rect_t rect) {
    return rect.width > 0 && rect.height > 0;
}
static bool rect_contains(nova_ui_rect_t rect, nova_ui_dlu_t x,
                          nova_ui_dlu_t y) {
    return x >= rect.x && y >= rect.y && x < rect.x + rect.width &&
           y < rect.y + rect.height;
}
static bool rect_overlaps(nova_ui_rect_t a, nova_ui_rect_t b) {
    return a.x < b.x + b.width && b.x < a.x + a.width &&
           a.y < b.y + b.height && b.y < a.y + a.height;
}
static nova_ui_rect_t rect_union(nova_ui_rect_t a, nova_ui_rect_t b) {
    nova_ui_dlu_t left = a.x < b.x ? a.x : b.x;
    nova_ui_dlu_t top = a.y < b.y ? a.y : b.y;
    nova_ui_dlu_t right_a = a.x + a.width;
    nova_ui_dlu_t right_b = b.x + b.width;
    nova_ui_dlu_t bottom_a = a.y + a.height;
    nova_ui_dlu_t bottom_b = b.y + b.height;
    nova_ui_dlu_t right = right_a > right_b ? right_a : right_b;
    nova_ui_dlu_t bottom = bottom_a > bottom_b ? bottom_a : bottom_b;
    return (nova_ui_rect_t){left, top, right - left, bottom - top};
}
static void copy_label(char target[NOVA_UI_LABEL_MAX], const char *source) {
    size_t i = 0u;
    if (source != NULL) {
        while (i + 1u < NOVA_UI_LABEL_MAX && source[i] != '\0') {
            target[i] = source[i];
            ++i;
        }
    }
    target[i] = '\0';
}

static bool transform_matrix_empty(nova_ui_transform_t transform) {
    return transform.m11_milli == 0 && transform.m12_milli == 0 &&
           transform.m21_milli == 0 && transform.m22_milli == 0;
}

static nova_ui_transform_t transform_identity(void) {
    return (nova_ui_transform_t){1000, 0, 0, 1000, 0, 0};
}

static nova_ui_transform_t transform_multiply(nova_ui_transform_t a,
                                               nova_ui_transform_t b) {
    nova_ui_transform_t result;
    result.m11_milli = (int32_t)(((int64_t)a.m11_milli * b.m11_milli +
                                  (int64_t)a.m21_milli * b.m12_milli) / 1000);
    result.m12_milli = (int32_t)(((int64_t)a.m12_milli * b.m11_milli +
                                  (int64_t)a.m22_milli * b.m12_milli) / 1000);
    result.m21_milli = (int32_t)(((int64_t)a.m11_milli * b.m21_milli +
                                  (int64_t)a.m21_milli * b.m22_milli) / 1000);
    result.m22_milli = (int32_t)(((int64_t)a.m12_milli * b.m21_milli +
                                  (int64_t)a.m22_milli * b.m22_milli) / 1000);
    result.translate_x_dlu =
        (nova_ui_dlu_t)(((int64_t)a.m11_milli * b.translate_x_dlu +
                         (int64_t)a.m21_milli * b.translate_y_dlu) / 1000) +
        a.translate_x_dlu;
    result.translate_y_dlu =
        (nova_ui_dlu_t)(((int64_t)a.m12_milli * b.translate_x_dlu +
                         (int64_t)a.m22_milli * b.translate_y_dlu) / 1000) +
        a.translate_y_dlu;
    return result;
}

static void transform_point(nova_ui_transform_t transform, nova_ui_dlu_t x,
                            nova_ui_dlu_t y, nova_ui_dlu_t *out_x,
                            nova_ui_dlu_t *out_y) {
    *out_x = (nova_ui_dlu_t)(((int64_t)transform.m11_milli * x +
                              (int64_t)transform.m21_milli * y) / 1000) +
             transform.translate_x_dlu;
    *out_y = (nova_ui_dlu_t)(((int64_t)transform.m12_milli * x +
                              (int64_t)transform.m22_milli * y) / 1000) +
             transform.translate_y_dlu;
}

static ptrdiff_t node_index(const nova_ui_runtime_t *runtime,
                            nova_ui_node_id_t id) {
    size_t i;
    for (i = 0u; i < runtime->node_count; ++i) {
        if (node_id_equal(runtime->nodes[i].id, id)) return (ptrdiff_t)i;
    }
    return -1;
}
static ptrdiff_t accessibility_index(const nova_ui_runtime_t *runtime,
                                     nova_ui_accessibility_id_t id) {
    size_t i;
    for (i = 0u; i < runtime->accessibility_count; ++i) {
        if (runtime->accessibility[i].id.value == id.value) return (ptrdiff_t)i;
    }
    return -1;
}
static ptrdiff_t display_index(const nova_ui_runtime_t *runtime,
                               nova_ui_display_id_t id) {
    size_t i;
    for (i = 0u; i < runtime->display_count; ++i) {
        if (runtime->displays[i].id.value == id.value) return (ptrdiff_t)i;
    }
    return -1;
}
static ptrdiff_t surface_index(const nova_ui_runtime_t *runtime,
                               nova_ui_surface_id_t id) {
    size_t i;
    for (i = 0u; i < runtime->surface_count; ++i) {
        if (runtime->surfaces[i].id.value == id.value) return (ptrdiff_t)i;
    }
    return -1;
}
static ptrdiff_t window_index(const nova_ui_runtime_t *runtime,
                              nova_ui_window_id_t id) {
    size_t i;
    for (i = 0u; i < runtime->window_count; ++i) {
        if (runtime->windows[i].id.value == id.value) return (ptrdiff_t)i;
    }
    return -1;
}
static ptrdiff_t capability_index(const nova_ui_runtime_t *runtime,
                                  nova_ui_capability_id_t id) {
    size_t i;
    for (i = 0u; i < runtime->capability_count; ++i) {
        if (runtime->capabilities[i].id.value == id.value) return (ptrdiff_t)i;
    }
    return -1;
}

static nova_ui_color_t color(uint8_t r, uint8_t g, uint8_t b) {
    return (nova_ui_color_t){r, g, b, 255u};
}

static void theme_fill(nova_ui_theme_t *theme, nova_ui_theme_kind_t kind) {
    bool light = kind == NOVA_UI_THEME_LIGHT;
    theme->kind = kind;
    theme->colors[NOVA_UI_COLOR_SURFACE_PRIMARY] =
        light ? color(246, 248, 251) : color(12, 16, 22);
    theme->colors[NOVA_UI_COLOR_SURFACE_SECONDARY] =
        light ? color(231, 236, 243) : color(23, 30, 40);
    theme->colors[NOVA_UI_COLOR_SURFACE_ELEVATED] =
        light ? color(255, 255, 255) : color(31, 40, 53);
    theme->colors[NOVA_UI_COLOR_TEXT_PRIMARY] =
        light ? color(20, 26, 34) : color(240, 246, 252);
    theme->colors[NOVA_UI_COLOR_TEXT_SECONDARY] =
        light ? color(70, 79, 91) : color(169, 183, 199);
    theme->colors[NOVA_UI_COLOR_TEXT_DISABLED] = color(112, 122, 135);
    theme->colors[NOVA_UI_COLOR_ACCENT_PRIMARY] = color(43, 139, 217);
    theme->colors[NOVA_UI_COLOR_ACCENT_SECONDARY] = color(99, 190, 255);
    theme->colors[NOVA_UI_COLOR_STATE_SUCCESS] = color(45, 184, 121);
    theme->colors[NOVA_UI_COLOR_STATE_WARNING] = color(245, 174, 66);
    theme->colors[NOVA_UI_COLOR_STATE_ERROR] = color(220, 61, 96);
    theme->colors[NOVA_UI_COLOR_STATE_INFORMATION] = color(75, 157, 224);
    theme->colors[NOVA_UI_COLOR_FOCUS] = color(255, 202, 71);
    theme->colors[NOVA_UI_COLOR_SELECTION] = color(43, 139, 217);
    if (kind == NOVA_UI_THEME_HIGH_CONTRAST) {
        size_t i;
        for (i = 0u; i < NOVA_UI_COLOR_COUNT; ++i)
            theme->colors[i] = color(0, 0, 0);
        theme->colors[NOVA_UI_COLOR_TEXT_PRIMARY] = color(255, 255, 255);
        theme->colors[NOVA_UI_COLOR_TEXT_SECONDARY] = color(255, 255, 255);
        theme->colors[NOVA_UI_COLOR_ACCENT_PRIMARY] = color(0, 255, 255);
        theme->colors[NOVA_UI_COLOR_ACCENT_SECONDARY] = color(255, 255, 0);
        theme->colors[NOVA_UI_COLOR_FOCUS] = color(255, 255, 0);
        theme->colors[NOVA_UI_COLOR_SELECTION] = color(0, 255, 255);
        theme->colors[NOVA_UI_COLOR_STATE_ERROR] = color(255, 80, 80);
    }
}

void nova_ui_runtime_init(nova_ui_runtime_t *runtime) {
    if (runtime == NULL) return;
    memset(runtime, 0, sizeof(*runtime));
    runtime->active_session = 1u;
    runtime->theme.generation = 1u;
    runtime->theme.spacing_dlu = 4;
    runtime->theme.corner_radius_dlu = 3;
    runtime->theme.text_scale_milli = 1000u;
    runtime->theme.effects_enabled = true;
    theme_fill(&runtime->theme, NOVA_UI_THEME_DARK);
}

nova_ui_result_t nova_ui_theme_apply(nova_ui_runtime_t *runtime,
                                     nova_ui_theme_kind_t kind,
                                     bool reduce_motion,
                                     bool reduce_transparency,
                                     uint16_t text_scale_milli) {
    size_t i;
    if (runtime == NULL || kind > NOVA_UI_THEME_HIGH_CONTRAST ||
        text_scale_milli < 750u || text_scale_milli > 3000u)
        return NOVA_UI_INVALID;
    theme_fill(&runtime->theme, kind);
    runtime->theme.reduce_motion = reduce_motion ||
                                   kind == NOVA_UI_THEME_HIGH_CONTRAST;
    runtime->theme.reduce_transparency = reduce_transparency ||
                                         kind == NOVA_UI_THEME_HIGH_CONTRAST;
    runtime->theme.effects_enabled = !runtime->theme.reduce_transparency &&
                                     !runtime->resource_pressure;
    runtime->theme.text_scale_milli = text_scale_milli;
    ++runtime->theme.generation;
    ++runtime->diagnostics.theme_changes;
    for (i = 0u; i < runtime->node_count; ++i)
        runtime->nodes[i].dirty |= NOVA_UI_DIRTY_VISUAL | NOVA_UI_DIRTY_LAYOUT;
    runtime->scheduler.damage_pending = runtime->node_count != 0u;
    return NOVA_UI_OK;
}

nova_ui_color_t nova_ui_theme_color(const nova_ui_runtime_t *runtime,
                                    nova_ui_color_token_t token) {
    if (runtime == NULL || token >= NOVA_UI_COLOR_COUNT)
        return (nova_ui_color_t){0u, 0u, 0u, 0u};
    return runtime->theme.colors[token];
}

nova_ui_result_t nova_ui_damage_add(nova_ui_runtime_t *runtime,
                                    nova_ui_rect_t rect) {
    size_t i;
    if (runtime == NULL || !rect_valid(rect)) return NOVA_UI_INVALID;
    for (i = 0u; i < runtime->scene_damage_count; ++i) {
        if (rect_overlaps(runtime->scene_damage[i], rect)) {
            runtime->scene_damage[i] = rect_union(runtime->scene_damage[i], rect);
            runtime->scheduler.damage_pending = true;
            return NOVA_UI_OK;
        }
    }
    if (runtime->scene_damage_count >= NOVA_UI_MAX_DAMAGE) {
        nova_ui_rect_t full = rect;
        for (i = 0u; i < runtime->scene_damage_count; ++i)
            full = rect_union(full, runtime->scene_damage[i]);
        runtime->scene_damage[0] = full;
        runtime->scene_damage_count = 1u;
        ++runtime->diagnostics.full_redraws;
    } else {
        runtime->scene_damage[runtime->scene_damage_count++] = rect;
    }
    runtime->scheduler.damage_pending = true;
    return NOVA_UI_OK;
}

void nova_ui_damage_full(nova_ui_runtime_t *runtime,
                         nova_ui_rect_t display_bounds) {
    if (runtime == NULL || !rect_valid(display_bounds)) return;
    runtime->scene_damage[0] = display_bounds;
    runtime->scene_damage_count = 1u;
    runtime->scheduler.damage_pending = true;
    ++runtime->diagnostics.full_redraws;
}

nova_ui_result_t nova_ui_node_upsert(nova_ui_runtime_t *runtime,
                                     const nova_ui_node_t *desired) {
    ptrdiff_t index;
    nova_ui_node_t copy;
    if (runtime == NULL || desired == NULL || !id_valid(desired->id.value) ||
        !rect_valid(desired->desired_bounds) || desired->opacity_milli > 1000u)
        return NOVA_UI_INVALID;
    if (desired->parent.value == desired->id.value) return NOVA_UI_CONFLICT;
    index = node_index(runtime, desired->id);
    copy = *desired;
    if (transform_matrix_empty(copy.transform)) {
        copy.transform.m11_milli = 1000;
        copy.transform.m22_milli = 1000;
    }
    copy.dirty |= NOVA_UI_DIRTY_ALL;
    copy.actual_bounds = index >= 0 ? runtime->nodes[index].actual_bounds
                                   : desired->desired_bounds;
    copy.actual_visible = index >= 0 ? runtime->nodes[index].actual_visible : false;
    copy_label(copy.label, desired->label);
    if (index >= 0) {
        runtime->nodes[index] = copy;
        return NOVA_UI_OK;
    }
    if (runtime->node_count >= NOVA_UI_MAX_NODES) return NOVA_UI_CAPACITY;
    runtime->nodes[runtime->node_count++] = copy;
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_node_remove(nova_ui_runtime_t *runtime,
                                     nova_ui_node_id_t id) {
    ptrdiff_t index;
    size_t i;
    if (runtime == NULL || !id_valid(id.value)) return NOVA_UI_INVALID;
    index = node_index(runtime, id);
    if (index < 0) return NOVA_UI_NOT_FOUND;
    for (i = 0u; i < runtime->node_count; ++i)
        if (runtime->nodes[i].parent.value == id.value) return NOVA_UI_CONFLICT;
    (void)nova_ui_damage_add(runtime, runtime->nodes[index].actual_bounds);
    for (i = (size_t)index + 1u; i < runtime->node_count; ++i)
        runtime->nodes[i - 1u] = runtime->nodes[i];
    --runtime->node_count;
    if (runtime->keyboard_focus.value == id.value) runtime->keyboard_focus.value = 0u;
    if (runtime->accessibility_focus.value == id.value)
        runtime->accessibility_focus.value = 0u;
    if (runtime->pointer_capture.value == id.value) {
        runtime->pointer_capture.value = 0u;
        runtime->capture_owner.value = 0u;
    }
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_reconcile(nova_ui_runtime_t *runtime) {
    size_t i;
    if (runtime == NULL) return NOVA_UI_INVALID;
    for (i = 0u; i < runtime->node_count; ++i) {
        nova_ui_node_t *node = &runtime->nodes[i];
        if (node->dirty == NOVA_UI_DIRTY_NONE) continue;
        if (node->actual_visible) (void)nova_ui_damage_add(runtime, node->actual_bounds);
        node->actual_bounds = node->desired_bounds;
        node->actual_visible = node->desired_visible;
        if (node->actual_visible) (void)nova_ui_damage_add(runtime, node->actual_bounds);
        node->dirty = NOVA_UI_DIRTY_NONE;
    }
    ++runtime->diagnostics.reconciliation_count;
    return NOVA_UI_OK;
}

static bool scene_has_cycle_from(const nova_ui_runtime_t *runtime, size_t start) {
    nova_ui_node_id_t parent = runtime->nodes[start].parent;
    size_t steps = 0u;
    while (parent.value != 0u) {
        ptrdiff_t index;
        if (parent.value == runtime->nodes[start].id.value) return true;
        index = node_index(runtime, parent);
        if (index < 0) return true;
        parent = runtime->nodes[index].parent;
        if (++steps > runtime->node_count) return true;
    }
    return false;
}

nova_ui_result_t nova_ui_scene_publish(nova_ui_runtime_t *runtime) {
    size_t i;
    if (runtime == NULL) return NOVA_UI_INVALID;
    for (i = 0u; i < runtime->node_count; ++i)
        if (scene_has_cycle_from(runtime, i)) return NOVA_UI_CONFLICT;
    ++runtime->diagnostics.scene_generation;
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_scene_world_bounds(const nova_ui_runtime_t *runtime,
                                            nova_ui_node_id_t id,
                                            nova_ui_rect_t *bounds) {
    ptrdiff_t index;
    ptrdiff_t chain[NOVA_UI_MAX_NODES];
    size_t depth = 0u;
    nova_ui_transform_t world = transform_identity();
    nova_ui_rect_t local;
    nova_ui_dlu_t x[4];
    nova_ui_dlu_t y[4];
    nova_ui_dlu_t min_x;
    nova_ui_dlu_t min_y;
    nova_ui_dlu_t max_x;
    nova_ui_dlu_t max_y;
    size_t i;
    if (runtime == NULL || bounds == NULL) return NOVA_UI_INVALID;
    index = node_index(runtime, id);
    if (index < 0) return NOVA_UI_NOT_FOUND;
    while (index >= 0) {
        if (depth >= NOVA_UI_MAX_NODES) return NOVA_UI_CONFLICT;
        chain[depth++] = index;
        if (runtime->nodes[index].parent.value == 0u) break;
        index = node_index(runtime, runtime->nodes[index].parent);
        if (index < 0) return NOVA_UI_CONFLICT;
    }
    while (depth != 0u) {
        --depth;
        world = transform_multiply(world, runtime->nodes[chain[depth]].transform);
    }
    index = node_index(runtime, id);
    local = runtime->nodes[index].actual_bounds;
    transform_point(world, local.x, local.y, &x[0], &y[0]);
    transform_point(world, local.x + local.width, local.y, &x[1], &y[1]);
    transform_point(world, local.x, local.y + local.height, &x[2], &y[2]);
    transform_point(world, local.x + local.width, local.y + local.height,
                    &x[3], &y[3]);
    min_x = max_x = x[0];
    min_y = max_y = y[0];
    for (i = 1u; i < 4u; ++i) {
        if (x[i] < min_x) min_x = x[i];
        if (x[i] > max_x) max_x = x[i];
        if (y[i] < min_y) min_y = y[i];
        if (y[i] > max_y) max_y = y[i];
    }
    *bounds = (nova_ui_rect_t){min_x, min_y, max_x - min_x, max_y - min_y};
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_accessibility_rebuild(nova_ui_runtime_t *runtime) {
    size_t i;
    if (runtime == NULL) return NOVA_UI_INVALID;
    runtime->accessibility_count = 0u;
    for (i = 0u; i < runtime->node_count; ++i) {
        const nova_ui_node_t *source = &runtime->nodes[i];
        nova_ui_accessibility_node_t *target;
        if (source->role == NOVA_UI_ROLE_NONE) continue;
        target = &runtime->accessibility[runtime->accessibility_count++];
        memset(target, 0, sizeof(*target));
        target->id.value = source->id.value;
        target->source_node = source->id;
        target->role = source->role;
        target->action = source->action;
        target->state = source->state;
        target->secure = source->protected_content;
        copy_label(target->name, source->protected_content ? "Geschützter Inhalt"
                                                           : source->label);
    }
    ++runtime->diagnostics.accessibility_generation;
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_accessibility_set_virtual_range(
    nova_ui_runtime_t *runtime, nova_ui_accessibility_id_t id,
    uint32_t total_items, uint32_t range_start, uint32_t range_count) {
    ptrdiff_t index;
    if (runtime == NULL || range_start > total_items ||
        range_count > total_items - range_start)
        return NOVA_UI_INVALID;
    index = accessibility_index(runtime, id);
    if (index < 0) return NOVA_UI_NOT_FOUND;
    runtime->accessibility[index].virtual_item_count = total_items;
    runtime->accessibility[index].virtual_range_start = range_start;
    runtime->accessibility[index].virtual_range_count = range_count;
    ++runtime->diagnostics.accessibility_generation;
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_accessibility_invoke(nova_ui_runtime_t *runtime,
                                              nova_ui_accessibility_id_t id,
                                              nova_ui_action_t action) {
    ptrdiff_t ai;
    ptrdiff_t ni;
    if (runtime == NULL) return NOVA_UI_INVALID;
    ai = accessibility_index(runtime, id);
    if (ai < 0) return NOVA_UI_NOT_FOUND;
    if (runtime->accessibility[ai].action != action) return NOVA_UI_INVALID;
    ni = node_index(runtime, runtime->accessibility[ai].source_node);
    if (ni < 0) return NOVA_UI_NOT_FOUND;
    if (runtime->nodes[ni].required_capability.value != 0u &&
        !nova_ui_capability_can_execute(runtime,
                                        runtime->nodes[ni].required_capability))
        return NOVA_UI_DENIED;
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_display_connect(nova_ui_runtime_t *runtime,
                                         nova_ui_display_t display) {
    ptrdiff_t index;
    if (runtime == NULL || !id_valid(display.id.value) ||
        display.mode.width_px == 0u || display.mode.height_px == 0u ||
        display.mode.refresh_millihz == 0u || display.scale_milli < 500u ||
        display.scale_milli > 4000u)
        return NOVA_UI_INVALID;
    if (display.vrr_supported &&
        (display.vrr_min_millihz == 0u ||
         display.vrr_min_millihz > display.vrr_max_millihz))
        return NOVA_UI_INVALID;
    display.connected = true;
    if (!display.vrr_supported) display.vrr_enabled = false;
    display.current_refresh_millihz = display.mode.refresh_millihz;
    index = display_index(runtime, display.id);
    if (index >= 0) {
        runtime->displays[index] = display;
        return NOVA_UI_OK;
    }
    if (runtime->display_count >= NOVA_UI_MAX_DISPLAYS) return NOVA_UI_CAPACITY;
    runtime->displays[runtime->display_count++] = display;
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_display_disconnect(nova_ui_runtime_t *runtime,
                                            nova_ui_display_id_t id) {
    ptrdiff_t index;
    size_t i;
    if (runtime == NULL) return NOVA_UI_INVALID;
    index = display_index(runtime, id);
    if (index < 0) return NOVA_UI_NOT_FOUND;
    for (i = (size_t)index + 1u; i < runtime->display_count; ++i)
        runtime->displays[i - 1u] = runtime->displays[i];
    --runtime->display_count;
    for (i = 0u; i < runtime->window_count; ++i) {
        nova_ui_window_t *window = &runtime->windows[i];
        if (window->display.value != id.value) continue;
        if (runtime->display_count != 0u) {
            window->display = runtime->displays[0].id;
            window->bounds.x = 0;
            window->bounds.y = 0;
        } else {
            window->state = NOVA_UI_WINDOW_SUSPENDED;
        }
    }
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_display_set_vrr(nova_ui_runtime_t *runtime,
                                         nova_ui_display_id_t id,
                                         bool enabled) {
    ptrdiff_t index;
    if (runtime == NULL) return NOVA_UI_INVALID;
    index = display_index(runtime, id);
    if (index < 0) return NOVA_UI_NOT_FOUND;
    if (enabled && !runtime->displays[index].vrr_supported)
        return NOVA_UI_UNAVAILABLE;
    runtime->displays[index].vrr_enabled = enabled;
    runtime->displays[index].fallback_mode = !enabled;
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_surface_create(nova_ui_runtime_t *runtime,
                                        nova_ui_surface_t surface) {
    if (runtime == NULL || !id_valid(surface.id.value) ||
        !id_valid(surface.owner.value) || surface.width == 0u ||
        surface.height == 0u || surface.width > UINT32_MAX / 4u ||
        surface.stride < surface.width * 4u ||
        surface.format > NOVA_UI_PIXEL_ARGB8888)
        return NOVA_UI_INVALID;
    if (surface_index(runtime, surface.id) >= 0) return NOVA_UI_CONFLICT;
    if (runtime->surface_count >= NOVA_UI_MAX_SURFACES) return NOVA_UI_CAPACITY;
    surface.valid = true;
    surface.damage_count = 0u;
    runtime->surfaces[runtime->surface_count++] = surface;
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_surface_damage(nova_ui_runtime_t *runtime,
                                        nova_ui_surface_id_t id,
                                        nova_ui_rect_t damage) {
    ptrdiff_t index;
    nova_ui_surface_t *surface;
    size_t i;
    if (runtime == NULL || !rect_valid(damage)) return NOVA_UI_INVALID;
    index = surface_index(runtime, id);
    if (index < 0) return NOVA_UI_NOT_FOUND;
    surface = &runtime->surfaces[index];
    if (damage.x < 0 || damage.y < 0 ||
        damage.x + damage.width > (nova_ui_dlu_t)surface->width ||
        damage.y + damage.height > (nova_ui_dlu_t)surface->height) {
        ++runtime->diagnostics.rejected_surfaces;
        surface->valid = false;
        return NOVA_UI_INVALID;
    }
    for (i = 0u; i < surface->damage_count; ++i) {
        if (rect_overlaps(surface->damage[i], damage)) {
            surface->damage[i] = rect_union(surface->damage[i], damage);
            runtime->scheduler.damage_pending = true;
            return NOVA_UI_OK;
        }
    }
    if (surface->damage_count >= NOVA_UI_MAX_DAMAGE) {
        surface->damage[0] = (nova_ui_rect_t){0, 0,
            (nova_ui_dlu_t)surface->width, (nova_ui_dlu_t)surface->height};
        surface->damage_count = 1u;
    } else {
        surface->damage[surface->damage_count++] = damage;
    }
    runtime->scheduler.damage_pending = true;
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_window_create(nova_ui_runtime_t *runtime,
                                       nova_ui_window_t window) {
    ptrdiff_t si;
    if (runtime == NULL || !id_valid(window.id.value) ||
        !id_valid(window.owner.value) || !rect_valid(window.bounds))
        return NOVA_UI_INVALID;
    if (window_index(runtime, window.id) >= 0) return NOVA_UI_CONFLICT;
    si = surface_index(runtime, window.surface);
    if (si < 0 || !owner_id_equal(runtime->surfaces[si].owner, window.owner))
        return NOVA_UI_DENIED;
    if (display_index(runtime, window.display) < 0) return NOVA_UI_NOT_FOUND;
    if (window.always_on_top && !window.system_ui) return NOVA_UI_DENIED;
    if (runtime->window_count >= NOVA_UI_MAX_WINDOWS) return NOVA_UI_CAPACITY;
    window.alive = true;
    runtime->windows[runtime->window_count++] = window;
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_window_set_state(nova_ui_runtime_t *runtime,
                                          nova_ui_window_id_t id,
                                          nova_ui_window_state_t state) {
    ptrdiff_t index;
    if (runtime == NULL || state > NOVA_UI_WINDOW_SUSPENDED)
        return NOVA_UI_INVALID;
    index = window_index(runtime, id);
    if (index < 0) return NOVA_UI_NOT_FOUND;
    runtime->windows[index].state = state;
    runtime->scheduler.damage_pending = true;
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_window_move(nova_ui_runtime_t *runtime,
                                     nova_ui_window_id_t id,
                                     nova_ui_display_id_t display,
                                     uint32_t workspace,
                                     nova_ui_rect_t bounds) {
    ptrdiff_t index;
    if (runtime == NULL || !rect_valid(bounds)) return NOVA_UI_INVALID;
    index = window_index(runtime, id);
    if (index < 0) return NOVA_UI_NOT_FOUND;
    if (display_index(runtime, display) < 0) return NOVA_UI_NOT_FOUND;
    runtime->windows[index].display = display;
    runtime->windows[index].workspace = workspace;
    runtime->windows[index].bounds = bounds;
    runtime->scheduler.damage_pending = true;
    return NOVA_UI_OK;
}

static size_t visible_surface_count(const nova_ui_runtime_t *runtime,
                                    nova_ui_display_id_t display,
                                    ptrdiff_t *single_surface) {
    size_t i;
    size_t count = 0u;
    *single_surface = -1;
    for (i = 0u; i < runtime->window_count; ++i) {
        const nova_ui_window_t *window = &runtime->windows[i];
        ptrdiff_t si;
        if (!window->alive || window->display.value != display.value ||
            window->state == NOVA_UI_WINDOW_MINIMIZED ||
            window->state == NOVA_UI_WINDOW_HIDDEN ||
            window->state == NOVA_UI_WINDOW_SUSPENDED)
            continue;
        si = surface_index(runtime, window->surface);
        if (si >= 0 && runtime->surfaces[si].valid &&
            runtime->surfaces[si].visible) {
            *single_surface = si;
            ++count;
        }
    }
    return count;
}

nova_ui_compose_path_t nova_ui_compose(nova_ui_runtime_t *runtime,
                                       nova_ui_display_id_t display) {
    ptrdiff_t di;
    ptrdiff_t single;
    size_t visible;
    size_t i;
    nova_ui_compose_path_t path;
    if (runtime == NULL) return NOVA_UI_COMPOSE_SOFTWARE;
    di = display_index(runtime, display);
    if (di < 0) return NOVA_UI_COMPOSE_SOFTWARE;
    visible = visible_surface_count(runtime, display, &single);
    if (visible == 1u && single >= 0 && runtime->surfaces[single].scanout_capable &&
        !runtime->surfaces[single].protected_content)
        path = NOVA_UI_COMPOSE_DIRECT_SCANOUT;
    else if (runtime->gpu_available && !runtime->resource_pressure)
        path = NOVA_UI_COMPOSE_GPU;
    else
        path = NOVA_UI_COMPOSE_SOFTWARE;
    for (i = 0u; i < runtime->surface_count; ++i) {
        runtime->surfaces[i].damage_count = 0u;
        if (runtime->surfaces[i].buffer_age < UINT32_MAX)
            ++runtime->surfaces[i].buffer_age;
    }
    runtime->scene_damage_count = 0u;
    runtime->scheduler.damage_pending = false;
    runtime->diagnostics.last_compose_path = path;
    return path;
}

nova_ui_result_t nova_ui_capability_register(nova_ui_runtime_t *runtime,
                                             nova_ui_capability_t capability) {
    if (runtime == NULL || !id_valid(capability.id.value) ||
        !id_valid(capability.provider.value) || capability.semantic_type[0] == '\0')
        return NOVA_UI_INVALID;
    if (capability_index(runtime, capability.id) >= 0) return NOVA_UI_CONFLICT;
    if (runtime->capability_count >= NOVA_UI_MAX_CAPABILITIES)
        return NOVA_UI_CAPACITY;
    runtime->capabilities[runtime->capability_count] = capability;
    copy_label(runtime->capabilities[runtime->capability_count].semantic_type,
               capability.semantic_type);
    ++runtime->capability_count;
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_capability_set_available(
    nova_ui_runtime_t *runtime, nova_ui_capability_id_t id, bool available) {
    ptrdiff_t index;
    size_t i;
    if (runtime == NULL) return NOVA_UI_INVALID;
    index = capability_index(runtime, id);
    if (index < 0) return NOVA_UI_NOT_FOUND;
    runtime->capabilities[index].available = available;
    for (i = 0u; i < runtime->contribution_count; ++i)
        if (runtime->contributions[i].capability.value == id.value)
            runtime->contributions[i].available = available;
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_capability_authorize(nova_ui_runtime_t *runtime,
                                              nova_ui_capability_id_t id,
                                              bool authorized) {
    ptrdiff_t index;
    if (runtime == NULL) return NOVA_UI_INVALID;
    index = capability_index(runtime, id);
    if (index < 0) return NOVA_UI_NOT_FOUND;
    runtime->capabilities[index].authorized = authorized;
    return NOVA_UI_OK;
}

bool nova_ui_capability_can_execute(const nova_ui_runtime_t *runtime,
                                    nova_ui_capability_id_t id) {
    ptrdiff_t index;
    if (runtime == NULL || !id_valid(id.value)) return false;
    index = capability_index(runtime, id);
    return index >= 0 && runtime->capabilities[index].available &&
           runtime->capabilities[index].authorized;
}

static bool semantic_capability_can_execute(const nova_ui_runtime_t *runtime,
                                            const char *semantic_type) {
    size_t i;
    for (i = 0u; i < runtime->capability_count; ++i) {
        const nova_ui_capability_t *capability = &runtime->capabilities[i];
        if (strcmp(capability->semantic_type, semantic_type) == 0 &&
            capability->available && capability->authorized)
            return true;
    }
    return false;
}

nova_ui_result_t nova_ui_set_keyboard_focus(nova_ui_runtime_t *runtime,
                                            nova_ui_node_id_t node) {
    ptrdiff_t index;
    size_t i;
    if (runtime == NULL) return NOVA_UI_INVALID;
    index = node_index(runtime, node);
    if (index < 0 || !runtime->nodes[index].actual_visible ||
        (runtime->nodes[index].state & NOVA_UI_STATE_DISABLED) != 0u)
        return NOVA_UI_DENIED;
    for (i = 0u; i < runtime->node_count; ++i)
        runtime->nodes[i].state &= ~NOVA_UI_STATE_FOCUSED;
    runtime->nodes[index].state |= NOVA_UI_STATE_FOCUSED;
    runtime->keyboard_focus = node;
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_set_accessibility_focus(nova_ui_runtime_t *runtime,
                                                 nova_ui_node_id_t node) {
    if (runtime == NULL) return NOVA_UI_INVALID;
    if (node_index(runtime, node) < 0) return NOVA_UI_NOT_FOUND;
    runtime->accessibility_focus = node;
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_capture_pointer(nova_ui_runtime_t *runtime,
                                         nova_ui_owner_id_t owner,
                                         nova_ui_node_id_t node) {
    ptrdiff_t index;
    if (runtime == NULL || !id_valid(owner.value)) return NOVA_UI_INVALID;
    index = node_index(runtime, node);
    if (index < 0 || !runtime->nodes[index].actual_visible)
        return NOVA_UI_NOT_FOUND;
    if (runtime->pointer_capture.value != 0u &&
        !owner_id_equal(runtime->capture_owner, owner))
        return NOVA_UI_CONFLICT;
    runtime->capture_owner = owner;
    runtime->pointer_capture = node;
    return NOVA_UI_OK;
}

void nova_ui_release_owner(nova_ui_runtime_t *runtime,
                           nova_ui_owner_id_t owner) {
    size_t i;
    if (runtime == NULL) return;
    if (owner_id_equal(runtime->capture_owner, owner)) {
        runtime->capture_owner.value = 0u;
        runtime->pointer_capture.value = 0u;
    }
    for (i = 0u; i < runtime->window_count; ++i) {
        if (!owner_id_equal(runtime->windows[i].owner, owner)) continue;
        runtime->windows[i].alive = false;
        runtime->windows[i].state = NOVA_UI_WINDOW_HIDDEN;
    }
    if (runtime->active_touch_target.value != 0u) {
        ptrdiff_t index = node_index(runtime, runtime->active_touch_target);
        if (index < 0) {
            runtime->active_touch_target.value = 0u;
            runtime->active_touch_contact = 0u;
        }
    }
}

static ptrdiff_t hit_test(const nova_ui_runtime_t *runtime, nova_ui_dlu_t x,
                          nova_ui_dlu_t y) {
    ptrdiff_t best = -1;
    int32_t best_z = INT32_MIN;
    size_t i;
    for (i = 0u; i < runtime->node_count; ++i) {
        const nova_ui_node_t *node = &runtime->nodes[i];
        if (node->actual_visible && rect_contains(node->actual_bounds, x, y) &&
            (best < 0 || node->z_order >= best_z)) {
            best = (ptrdiff_t)i;
            best_z = node->z_order;
        }
    }
    return best;
}

static nova_ui_owner_id_t node_owner(const nova_ui_runtime_t *runtime,
                                     nova_ui_node_id_t node) {
    size_t i;
    for (i = 0u; i < runtime->window_count; ++i) {
        const nova_ui_window_t *window = &runtime->windows[i];
        if (!window->alive) continue;
        if (rect_contains(window->bounds,
                          runtime->nodes[node_index(runtime, node)].actual_bounds.x,
                          runtime->nodes[node_index(runtime, node)].actual_bounds.y))
            return window->owner;
    }
    return (nova_ui_owner_id_t){0u};
}

nova_ui_result_t nova_ui_route_input(nova_ui_runtime_t *runtime,
                                     const nova_ui_input_event_t *event,
                                     nova_ui_input_route_t *route) {
    ptrdiff_t index = -1;
    if (runtime == NULL || event == NULL || route == NULL ||
        event->monotonic_ns == 0u)
        return NOVA_UI_INVALID;
    memset(route, 0, sizeof(*route));
    if (event->session_id != runtime->active_session) goto denied;
    if (event->raw &&
        !semantic_capability_can_execute(runtime, "UI.Input.Raw"))
        goto denied;
    if (runtime->secure_input && !event->secure &&
        event->type != NOVA_UI_INPUT_SYSTEM)
        goto denied;
    if (event->type == NOVA_UI_INPUT_SYSTEM) {
        route->consumed_by_system = true;
        ++runtime->diagnostics.input_routes;
        nova_ui_request_frame(runtime, true);
        return NOVA_UI_OK;
    }
    if (event->type == NOVA_UI_INPUT_KEY) {
        index = node_index(runtime, runtime->keyboard_focus);
    } else if (runtime->pointer_capture.value != 0u) {
        index = node_index(runtime, runtime->pointer_capture);
    } else if ((event->type == NOVA_UI_INPUT_TOUCH_UPDATE ||
                event->type == NOVA_UI_INPUT_TOUCH_END) &&
               runtime->active_touch_contact == event->contact_id) {
        index = node_index(runtime, runtime->active_touch_target);
    } else {
        index = hit_test(runtime, event->x_dlu, event->y_dlu);
    }
    if (index < 0) return NOVA_UI_NOT_FOUND;
    if (event->type == NOVA_UI_INPUT_TOUCH_BEGIN) {
        runtime->active_touch_contact = event->contact_id;
        runtime->active_touch_target = runtime->nodes[index].id;
    } else if (event->type == NOVA_UI_INPUT_TOUCH_END &&
               runtime->active_touch_contact == event->contact_id) {
        runtime->active_touch_contact = 0u;
        runtime->active_touch_target.value = 0u;
    }
    route->node = runtime->nodes[index].id;
    route->recipient = runtime->pointer_capture.value != 0u
                           ? runtime->capture_owner
                           : node_owner(runtime, route->node);
    ++runtime->diagnostics.input_routes;
    nova_ui_request_frame(runtime, true);
    return NOVA_UI_OK;
denied:
    ++runtime->diagnostics.denied_inputs;
    return NOVA_UI_DENIED;
}

void nova_ui_request_frame(nova_ui_runtime_t *runtime, bool from_input) {
    if (runtime == NULL) return;
    ++runtime->scheduler.requested_frames;
    if (runtime->scheduler.queued_frames >= 2u) {
        runtime->scheduler.backpressure = true;
        ++runtime->scheduler.coalesced_requests;
    } else {
        ++runtime->scheduler.queued_frames;
    }
    runtime->scheduler.damage_pending = true;
    runtime->scheduler.input_pending |= from_input;
}

bool nova_ui_frame_due(nova_ui_runtime_t *runtime, uint64_t now_ns,
                       nova_ui_display_id_t display) {
    ptrdiff_t index;
    uint32_t refresh;
    uint64_t interval;
    if (runtime == NULL || runtime->scheduler.queued_frames == 0u ||
        !runtime->scheduler.damage_pending)
        return false;
    index = display_index(runtime, display);
    if (index < 0) return false;
    refresh = runtime->displays[index].mode.refresh_millihz;
    if (runtime->displays[index].vrr_enabled) {
        refresh = runtime->scheduler.input_pending
                      ? runtime->displays[index].vrr_max_millihz
                      : runtime->displays[index].vrr_min_millihz;
        runtime->displays[index].current_refresh_millihz = refresh;
    }
    if (refresh == 0u) return false;
    interval = 1000000000000ull / refresh;
    runtime->scheduler.next_deadline_ns =
        runtime->scheduler.last_present_ns + interval;
    return runtime->scheduler.last_present_ns == 0u ||
           now_ns >= runtime->scheduler.next_deadline_ns ||
           runtime->scheduler.input_pending;
}

void nova_ui_frame_presented(nova_ui_runtime_t *runtime, uint64_t now_ns,
                             bool deadline_missed) {
    if (runtime == NULL) return;
    runtime->scheduler.last_present_ns = now_ns;
    ++runtime->scheduler.presented_frames;
    if (deadline_missed) ++runtime->scheduler.missed_deadlines;
    if (runtime->scheduler.queued_frames != 0u)
        --runtime->scheduler.queued_frames;
    runtime->scheduler.input_pending = false;
    runtime->scheduler.backpressure = false;
}

nova_ui_result_t nova_ui_contribution_add(nova_ui_runtime_t *runtime,
                                          nova_ui_contribution_t contribution) {
    size_t i;
    ptrdiff_t ci;
    if (runtime == NULL || contribution.id == 0u ||
        !id_valid(contribution.provider.value) ||
        !id_valid(contribution.capability.value) || contribution.label[0] == '\0')
        return NOVA_UI_INVALID;
    ci = capability_index(runtime, contribution.capability);
    if (ci < 0 || !runtime->capabilities[ci].discoverable)
        return NOVA_UI_UNAVAILABLE;
    for (i = 0u; i < runtime->contribution_count; ++i) {
        const nova_ui_contribution_t *existing = &runtime->contributions[i];
        if (existing->id == contribution.id) return NOVA_UI_CONFLICT;
        if (existing->kind == contribution.kind &&
            existing->capability.value == contribution.capability.value &&
            existing->action == contribution.action)
            return NOVA_UI_CONFLICT;
    }
    if (runtime->contribution_count >= NOVA_UI_MAX_CONTRIBUTIONS)
        return NOVA_UI_CAPACITY;
    contribution.available = runtime->capabilities[ci].available;
    runtime->contributions[runtime->contribution_count] = contribution;
    copy_label(runtime->contributions[runtime->contribution_count].label,
               contribution.label);
    ++runtime->contribution_count;
    return NOVA_UI_OK;
}

nova_ui_result_t nova_ui_provider_failed(nova_ui_runtime_t *runtime,
                                         nova_ui_provider_id_t provider) {
    size_t i;
    bool found = false;
    if (runtime == NULL || !id_valid(provider.value)) return NOVA_UI_INVALID;
    for (i = 0u; i < runtime->capability_count; ++i) {
        if (runtime->capabilities[i].provider.value != provider.value) continue;
        runtime->capabilities[i].available = false;
        found = true;
    }
    for (i = 0u; i < runtime->contribution_count; ++i) {
        if (runtime->contributions[i].provider.value != provider.value) continue;
        runtime->contributions[i].available = false;
        runtime->contributions[i].visible = false;
    }
    if (!found) return NOVA_UI_NOT_FOUND;
    ++runtime->diagnostics.isolated_provider_failures;
    return NOVA_UI_OK;
}

size_t nova_ui_contribution_count_visible(
    const nova_ui_runtime_t *runtime, nova_ui_contribution_kind_t kind) {
    size_t i;
    size_t count = 0u;
    if (runtime == NULL) return 0u;
    for (i = 0u; i < runtime->contribution_count; ++i)
        if (runtime->contributions[i].kind == kind &&
            runtime->contributions[i].visible &&
            runtime->contributions[i].available)
            ++count;
    return count;
}

void nova_ui_responsive_layout(nova_ui_runtime_t *runtime,
                               nova_ui_dlu_t available_width_dlu,
                               bool touch_input) {
    size_t i;
    nova_ui_dlu_t minimum = touch_input ? 24 : 16;
    if (runtime == NULL || available_width_dlu <= 0) return;
    for (i = 0u; i < runtime->node_count; ++i) {
        nova_ui_node_t *node = &runtime->nodes[i];
        if (!node->desired_visible) continue;
        if (node->desired_bounds.width > available_width_dlu) {
            node->desired_bounds.width = available_width_dlu;
            node->dirty |= NOVA_UI_DIRTY_LAYOUT;
        }
        if (node->action != NOVA_UI_ACTION_NONE &&
            node->desired_bounds.height < minimum) {
            node->desired_bounds.height = minimum;
            node->dirty |= NOVA_UI_DIRTY_LAYOUT;
        }
    }
}

bool nova_ui_adaptive_change_allowed(bool explicit_user_choice,
                                     bool accessibility_required,
                                     uint16_t confidence_milli,
                                     bool changes_semantics) {
    if (explicit_user_choice || accessibility_required || changes_semantics)
        return false;
    return confidence_milli >= 800u;
}
