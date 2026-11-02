/* NovaOS C-Kernel - Nova Material Engine: Integration (siehe
   matintegrate.h fuer Scope). */
#include "matintegrate.h"
#include "matfx.h"

#define MATINTEGRATE_MAX_LAYERED_INSTANCES 16u
#define MATINTEGRATE_MAX_STYLE_BINDINGS    32u

typedef struct {
    int enabled;
    uint32_t definition_id;
    uint32_t layer_opacity;
} layer_slot_t;

typedef struct {
    uint32_t in_use;
    uint32_t instance_id;
    layer_slot_t layers[MATINTEGRATE_MAX_LAYERS];
} layered_instance_t;

typedef struct {
    uint32_t in_use;
    char name[24];
    uint32_t definition_id;
} matstyle_t;

typedef struct {
    uint32_t in_use;
    uint32_t instance_id;
    uint32_t style_id;
} style_binding_t;

typedef struct {
    uint32_t in_use;
    char role_name[24];
    uint32_t light_definition_id;
    uint32_t dark_definition_id;
} theme_role_t;

typedef struct {
    uint32_t in_use;
    uint32_t window_id;
    uint32_t instance_id;
    uint32_t style_id;
    uint32_t theme_role_id;
    uint32_t cached_instance_id;
} ui_binding_t;

static layered_instance_t g_layered[MATINTEGRATE_MAX_LAYERED_INSTANCES];
static matstyle_t g_styles[MATINTEGRATE_MAX_STYLES];
static style_binding_t g_style_bindings[MATINTEGRATE_MAX_STYLE_BINDINGS];
static theme_role_t g_roles[MATINTEGRATE_MAX_THEME_ROLES];
static ui_binding_t g_ui[MATINTEGRATE_MAX_BINDINGS];
static int g_dark_mode = 0;

static void copy_name24(char *dst, const char *src) {
    uint32_t i = 0;
    for (; i < 23 && src && src[i]; ++i) dst[i] = src[i];
    dst[i] = '\0';
}

void matintegrate_initialize(void) {
    for (uint32_t i = 0; i < MATINTEGRATE_MAX_LAYERED_INSTANCES; ++i) g_layered[i].in_use = 0;
    for (uint32_t i = 0; i < MATINTEGRATE_MAX_STYLES; ++i) g_styles[i].in_use = 0;
    for (uint32_t i = 0; i < MATINTEGRATE_MAX_STYLE_BINDINGS; ++i) g_style_bindings[i].in_use = 0;
    for (uint32_t i = 0; i < MATINTEGRATE_MAX_THEME_ROLES; ++i) g_roles[i].in_use = 0;
    for (uint32_t i = 0; i < MATINTEGRATE_MAX_BINDINGS; ++i) g_ui[i].in_use = 0;
    g_dark_mode = 0;
}

/* --- Layer Composition --- */

static layered_instance_t *find_layered(uint32_t instance_id) {
    for (uint32_t i = 0; i < MATINTEGRATE_MAX_LAYERED_INSTANCES; ++i)
        if (g_layered[i].in_use && g_layered[i].instance_id == instance_id) return &g_layered[i];
    return 0;
}

static layered_instance_t *get_or_create_layered(uint32_t instance_id) {
    layered_instance_t *found = find_layered(instance_id);
    if (found) return found;
    for (uint32_t i = 0; i < MATINTEGRATE_MAX_LAYERED_INSTANCES; ++i) {
        if (!g_layered[i].in_use) {
            g_layered[i].in_use = 1;
            g_layered[i].instance_id = instance_id;
            for (uint32_t l = 0; l < MATINTEGRATE_MAX_LAYERS; ++l) g_layered[i].layers[l].enabled = 0;
            return &g_layered[i];
        }
    }
    return 0;
}

int matlayer_set(uint32_t instance_id, uint32_t index, uint32_t definition_id,
                   uint32_t layer_opacity, int enabled) {
    if (!material_instance_exists(instance_id) || index >= MATINTEGRATE_MAX_LAYERS) return -1;
    if (enabled && !material_definition_exists(definition_id)) return -1;
    layered_instance_t *li = get_or_create_layered(instance_id);
    if (!li) return -1;
    li->layers[index].enabled = enabled ? 1 : 0;
    li->layers[index].definition_id = definition_id;
    li->layers[index].layer_opacity = layer_opacity > 255u ? 255u : layer_opacity;
    return 0;
}

int matlayer_clear(uint32_t instance_id) {
    layered_instance_t *li = find_layered(instance_id);
    if (!li) return -1;
    li->in_use = 0;
    return 0;
}

void matlayer_composite(render2d_surface_t *dst, uint32_t x, uint32_t y,
                          uint32_t w, uint32_t h,
                          const render2d_surface_t *backdrop, uint32_t bx, uint32_t by,
                          uint32_t instance_id) {
    layered_instance_t *li = find_layered(instance_id);
    int any_layer = 0;
    if (li) {
        int drawn_any = 0;
        for (uint32_t l = 0; l < MATINTEGRATE_MAX_LAYERS; ++l) {
            if (!li->layers[l].enabled) continue;
            any_layer = 1;

            material_properties_t props;
            if (material_definition_properties(li->layers[l].definition_id, &props) != 0) continue;
            props.opacity = (props.opacity * li->layers[l].layer_opacity) / 255u;

            const render2d_surface_t *use_backdrop = backdrop;
            uint32_t use_bx = bx, use_by = by;
            /* Nach der ersten gezeichneten Ebene ist dst an (x,y) bereits das
               Zwischenergebnis - weitere Ebenen stapeln sich darueber
               (LAYER-0001), kein Self-Sampling einer FREMDEN Instanz. */
            if (drawn_any) { use_backdrop = dst; use_bx = x; use_by = y; }

            uint32_t type = material_definition_type(li->layers[l].definition_id);
            matfx_apply(dst, x, y, w, h,
                        type == MATERIAL_TYPE_SOLID ? 0 : use_backdrop, use_bx, use_by,
                        &props, MATFX_BLEND_NORMAL);
            drawn_any = 1;
        }
    }

    if (!any_layer) {
        material_properties_t resolved;
        if (material_instance_resolve(instance_id, &resolved) != 0) return;
        uint32_t def = material_instance_definition(instance_id);
        uint32_t type = material_definition_type(def);
        matfx_apply(dst, x, y, w, h, type == MATERIAL_TYPE_SOLID ? 0 : backdrop, bx, by,
                    &resolved, MATFX_BLEND_NORMAL);
    }
}

/* --- Styles --- */

uint32_t matstyle_register(const char *name, uint32_t definition_id) {
    if (!name || name[0] == '\0' || !material_definition_exists(definition_id)) return 0;
    for (uint32_t i = 0; i < MATINTEGRATE_MAX_STYLES; ++i) {
        if (!g_styles[i].in_use) {
            g_styles[i].in_use = 1;
            copy_name24(g_styles[i].name, name);
            g_styles[i].definition_id = definition_id;
            return i + 1u;
        }
    }
    return 0;
}

static int style_exists(uint32_t style_id) {
    return (style_id != 0 && style_id <= MATINTEGRATE_MAX_STYLES && g_styles[style_id - 1u].in_use);
}

int matstyle_set_definition(uint32_t style_id, uint32_t new_definition_id) {
    if (!style_exists(style_id) || !material_definition_exists(new_definition_id)) return -1;
    g_styles[style_id - 1u].definition_id = new_definition_id;
    for (uint32_t i = 0; i < MATINTEGRATE_MAX_STYLE_BINDINGS; ++i) {
        if (g_style_bindings[i].in_use && g_style_bindings[i].style_id == style_id) {
            material_instance_rebind(g_style_bindings[i].instance_id, new_definition_id);
        }
    }
    return 0;
}

uint32_t matstyle_definition(uint32_t style_id) {
    if (!style_exists(style_id)) return 0;
    return g_styles[style_id - 1u].definition_id;
}

int matstyle_bind_instance(uint32_t instance_id, uint32_t style_id) {
    if (!material_instance_exists(instance_id) || !style_exists(style_id)) return -1;
    style_binding_t *slot = 0;
    for (uint32_t i = 0; i < MATINTEGRATE_MAX_STYLE_BINDINGS; ++i) {
        if (g_style_bindings[i].in_use && g_style_bindings[i].instance_id == instance_id) { slot = &g_style_bindings[i]; break; }
    }
    if (!slot) {
        for (uint32_t i = 0; i < MATINTEGRATE_MAX_STYLE_BINDINGS; ++i) {
            if (!g_style_bindings[i].in_use) { slot = &g_style_bindings[i]; break; }
        }
    }
    if (!slot) return -1;
    slot->in_use = 1;
    slot->instance_id = instance_id;
    slot->style_id = style_id;
    return material_instance_rebind(instance_id, matstyle_definition(style_id));
}

int matstyle_unbind_instance(uint32_t instance_id) {
    for (uint32_t i = 0; i < MATINTEGRATE_MAX_STYLE_BINDINGS; ++i) {
        if (g_style_bindings[i].in_use && g_style_bindings[i].instance_id == instance_id) {
            g_style_bindings[i].in_use = 0;
            return 0;
        }
    }
    return -1;
}

/* --- Theme --- */

uint32_t matintegrate_theme_register_role(const char *role_name,
                                            uint32_t light_definition_id,
                                            uint32_t dark_definition_id) {
    if (!role_name || role_name[0] == '\0') return 0;
    for (uint32_t i = 0; i < MATINTEGRATE_MAX_THEME_ROLES; ++i) {
        if (!g_roles[i].in_use) {
            g_roles[i].in_use = 1;
            copy_name24(g_roles[i].role_name, role_name);
            g_roles[i].light_definition_id = light_definition_id;
            g_roles[i].dark_definition_id = dark_definition_id;
            return i + 1u;
        }
    }
    return 0;
}

void matintegrate_set_dark_mode(int enabled) { g_dark_mode = enabled ? 1 : 0; }
int matintegrate_dark_mode(void) { return g_dark_mode; }

uint32_t matintegrate_theme_resolve(uint32_t role_id) {
    if (role_id == 0 || role_id > MATINTEGRATE_MAX_THEME_ROLES || !g_roles[role_id - 1u].in_use)
        return material_fallback_id();
    theme_role_t *r = &g_roles[role_id - 1u];
    uint32_t pick = g_dark_mode ? r->dark_definition_id : r->light_definition_id;
    if (pick == 0 || !material_definition_exists(pick)) return material_fallback_id();
    return pick;
}

/* --- UI-Integration --- */

static ui_binding_t *find_ui_binding(uint32_t window_id) {
    for (uint32_t i = 0; i < MATINTEGRATE_MAX_BINDINGS; ++i)
        if (g_ui[i].in_use && g_ui[i].window_id == window_id) return &g_ui[i];
    return 0;
}

static ui_binding_t *get_or_create_ui_binding(uint32_t window_id) {
    ui_binding_t *found = find_ui_binding(window_id);
    if (found) return found;
    for (uint32_t i = 0; i < MATINTEGRATE_MAX_BINDINGS; ++i) {
        if (!g_ui[i].in_use) {
            g_ui[i].in_use = 1;
            g_ui[i].window_id = window_id;
            g_ui[i].instance_id = 0;
            g_ui[i].style_id = 0;
            g_ui[i].theme_role_id = 0;
            g_ui[i].cached_instance_id = 0;
            return &g_ui[i];
        }
    }
    return 0;
}

int matintegrate_ui_bind_instance(uint32_t window_id, uint32_t instance_id) {
    if (!material_instance_exists(instance_id)) return -1;
    ui_binding_t *b = get_or_create_ui_binding(window_id);
    if (!b) return -1;
    b->instance_id = instance_id;
    return 0;
}

int matintegrate_ui_bind_style(uint32_t window_id, uint32_t style_id) {
    if (!style_exists(style_id)) return -1;
    ui_binding_t *b = get_or_create_ui_binding(window_id);
    if (!b) return -1;
    b->style_id = style_id;
    return 0;
}

int matintegrate_ui_bind_theme_role(uint32_t window_id, uint32_t theme_role_id) {
    if (theme_role_id == 0 || theme_role_id > MATINTEGRATE_MAX_THEME_ROLES || !g_roles[theme_role_id - 1u].in_use)
        return -1;
    ui_binding_t *b = get_or_create_ui_binding(window_id);
    if (!b) return -1;
    b->theme_role_id = theme_role_id;
    return 0;
}

int matintegrate_ui_unbind(uint32_t window_id) {
    ui_binding_t *b = find_ui_binding(window_id);
    if (!b) return -1;
    if (b->cached_instance_id != 0) material_instance_destroy(b->cached_instance_id);
    b->in_use = 0;
    return 0;
}

uint32_t matintegrate_ui_resolve_instance(uint32_t window_id) {
    ui_binding_t *b = find_ui_binding(window_id);
    if (!b) return 0;

    if (b->instance_id != 0 && material_instance_exists(b->instance_id)) return b->instance_id;

    uint32_t target_def;
    if (b->style_id != 0) target_def = matstyle_definition(b->style_id);
    else if (b->theme_role_id != 0) target_def = matintegrate_theme_resolve(b->theme_role_id);
    else target_def = material_fallback_id();
    if (target_def == 0 || !material_definition_exists(target_def)) target_def = material_fallback_id();

    if (b->cached_instance_id != 0 && material_instance_exists(b->cached_instance_id)) {
        material_instance_rebind(b->cached_instance_id, target_def);
        return b->cached_instance_id;
    }
    b->cached_instance_id = material_instance_create(target_def);
    return b->cached_instance_id;
}

int matintegrate_self_test(void) {
    material_initialize();
    matintegrate_initialize();

    material_properties_t p;
    p.color_rgb = 0x111111u; p.tint_rgb = 0; p.tint_strength = 0; p.opacity = 200u;
    p.blur_radius = 0; p.refraction = 0; p.reflection = 0; p.highlight = 0;
    p.distortion = 0; p.roughness = 0; p.ext_present_mask = 0;
    for (uint32_t i = 0; i < MATERIAL_EXT_COUNT; ++i) p.ext[i] = 0;
    uint32_t def_base = material_register("int-base", MATERIAL_TYPE_SOLID, &p);
    p.color_rgb = 0x222222u;
    uint32_t def_top = material_register("int-top", MATERIAL_TYPE_ACRYLIC, &p);
    p.color_rgb = 0x333333u;
    uint32_t def_alt = material_register("int-alt", MATERIAL_TYPE_SOLID, &p);

    /* Layer: zwei aktive Ebenen muessen gezeichnet werden, eine
       deaktivierte Ebene darf keinen Einfluss haben. */
    uint32_t inst = material_instance_create(def_base);
    if (matlayer_set(inst, 0u, def_base, 255u, 1) != 0) return -1;
    if (matlayer_set(inst, 1u, def_top, 255u, 1) != 0) return -1;
    if (matlayer_set(inst, 2u, def_alt, 255u, 0) != 0) return -1; /* deaktiviert */

    static uint32_t dst_buf[16 * 16];
    static uint32_t backdrop_buf[16 * 16];
    render2d_surface_t dst, backdrop;
    render2d_init_surface(&dst, dst_buf, 16u, 16u, 16u);
    render2d_init_surface(&backdrop, backdrop_buf, 16u, 16u, 16u);
    for (uint32_t i = 0; i < 16u * 16u; ++i) backdrop_buf[i] = 0x808080u;
    for (uint32_t i = 0; i < 16u * 16u; ++i) dst_buf[i] = 0x000000u;

    matlayer_composite(&dst, 0u, 0u, 8u, 8u, &backdrop, 0u, 0u, inst);
    if (dst_buf[0] == 0x000000u) return -1; /* etwas wurde gezeichnet */

    /* Style: Rebind propagiert an alle gebundenen Instanzen. */
    uint32_t style = matstyle_register("test-style", def_base);
    if (style == 0) return -1;
    uint32_t styled_inst = material_instance_create(def_base);
    if (matstyle_bind_instance(styled_inst, style) != 0) return -1;
    if (material_instance_definition(styled_inst) != def_base) return -1;
    if (matstyle_set_definition(style, def_top) != 0) return -1;
    if (material_instance_definition(styled_inst) != def_top) return -1;

    /* Theme: Hell-/Dunkelmodus liefert unterschiedliche Definitionen. */
    uint32_t role = matintegrate_theme_register_role("panel", def_base, def_alt);
    if (role == 0) return -1;
    matintegrate_set_dark_mode(0);
    if (matintegrate_theme_resolve(role) != def_base) return -1;
    matintegrate_set_dark_mode(1);
    if (matintegrate_theme_resolve(role) != def_alt) return -1;
    matintegrate_set_dark_mode(0);

    /* UI: Prioritaet Instanz > Style > Theme > Fallback, stabile Instanz-ID. */
    if (matintegrate_ui_bind_theme_role(100u, role) != 0) return -1;
    uint32_t resolved1 = matintegrate_ui_resolve_instance(100u);
    if (resolved1 == 0 || material_instance_definition(resolved1) != def_base) return -1;

    if (matintegrate_ui_bind_style(100u, style) != 0) return -1; /* Style hat Vorrang vor Theme */
    uint32_t resolved2 = matintegrate_ui_resolve_instance(100u);
    if (resolved2 != resolved1) return -1; /* gleiche, gecachte Instanz-ID */
    if (material_instance_definition(resolved2) != def_top) return -1; /* Style zeigt inzwischen auf def_top */

    uint32_t explicit_inst = material_instance_create(def_alt);
    if (matintegrate_ui_bind_instance(100u, explicit_inst) != 0) return -1; /* Instanz hat Vorrang vor Style */
    if (matintegrate_ui_resolve_instance(100u) != explicit_inst) return -1;

    if (matintegrate_ui_resolve_instance(999u) != 0) return -1; /* nie gebunden */

    return 0;
}
