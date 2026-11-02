/* NovaOS C-Kernel - Nova Material Engine: Konfiguration und
   Erweiterbarkeit (siehe matext.h fuer Scope). */
#include "matext.h"
#include "materialtypes.h"

typedef struct {
    uint32_t in_use;
    char name[24];
    uint32_t definition_id;
    material_properties_t override;
    uint32_t field_mask;
} preset_t;

static preset_t g_presets[MATEXT_MAX_PRESETS];
static uint32_t g_preset_acrylic_light = 0, g_preset_acrylic_dark = 0;
static uint32_t g_preset_acrylic_subtle = 0, g_preset_acrylic_strong = 0;
static int g_reduce_transparency = 0;

static void copy_name24(char *dst, const char *src) {
    uint32_t i = 0;
    for (; i < 23 && src && src[i]; ++i) dst[i] = src[i];
    dst[i] = '\0';
}

static void zero_props(material_properties_t *p) {
    p->color_rgb = 0; p->tint_rgb = 0; p->tint_strength = 0; p->opacity = 0;
    p->blur_radius = 0; p->refraction = 0; p->reflection = 0; p->highlight = 0;
    p->distortion = 0; p->roughness = 0; p->ext_present_mask = 0;
    for (uint32_t i = 0; i < MATERIAL_EXT_COUNT; ++i) p->ext[i] = 0;
}

static void apply_fields(material_properties_t *base, const material_properties_t *ov, uint32_t mask) {
    if (mask & MATERIAL_FIELD_COLOR) base->color_rgb = ov->color_rgb;
    if (mask & MATERIAL_FIELD_TINT_RGB) base->tint_rgb = ov->tint_rgb;
    if (mask & MATERIAL_FIELD_TINT_STRENGTH) base->tint_strength = ov->tint_strength;
    if (mask & MATERIAL_FIELD_OPACITY) base->opacity = ov->opacity;
    if (mask & MATERIAL_FIELD_BLUR) base->blur_radius = ov->blur_radius;
    if (mask & MATERIAL_FIELD_REFRACTION) base->refraction = ov->refraction;
    if (mask & MATERIAL_FIELD_REFLECTION) base->reflection = ov->reflection;
    if (mask & MATERIAL_FIELD_HIGHLIGHT) base->highlight = ov->highlight;
    if (mask & MATERIAL_FIELD_DISTORTION) base->distortion = ov->distortion;
    if (mask & MATERIAL_FIELD_ROUGHNESS) base->roughness = ov->roughness;
    for (uint32_t i = 0; i < MATERIAL_EXT_COUNT; ++i) {
        if (ov->ext_present_mask & (1u << i)) {
            base->ext[i] = ov->ext[i];
            base->ext_present_mask |= (1u << i);
        }
    }
}

static void apply_fields_to_instance(uint32_t instance_id, const material_properties_t *ov, uint32_t mask) {
    if (mask & MATERIAL_FIELD_COLOR) material_instance_set_property_u32(instance_id, MATERIAL_FIELD_COLOR, ov->color_rgb);
    if (mask & MATERIAL_FIELD_TINT_RGB) material_instance_set_property_u32(instance_id, MATERIAL_FIELD_TINT_RGB, ov->tint_rgb);
    if (mask & MATERIAL_FIELD_TINT_STRENGTH) material_instance_set_property_u32(instance_id, MATERIAL_FIELD_TINT_STRENGTH, ov->tint_strength);
    if (mask & MATERIAL_FIELD_OPACITY) material_instance_set_property_u32(instance_id, MATERIAL_FIELD_OPACITY, ov->opacity);
    if (mask & MATERIAL_FIELD_BLUR) material_instance_set_property_u32(instance_id, MATERIAL_FIELD_BLUR, ov->blur_radius);
    if (mask & MATERIAL_FIELD_REFRACTION) material_instance_set_property_u32(instance_id, MATERIAL_FIELD_REFRACTION, ov->refraction);
    if (mask & MATERIAL_FIELD_REFLECTION) material_instance_set_property_u32(instance_id, MATERIAL_FIELD_REFLECTION, ov->reflection);
    if (mask & MATERIAL_FIELD_HIGHLIGHT) material_instance_set_property_u32(instance_id, MATERIAL_FIELD_HIGHLIGHT, ov->highlight);
    if (mask & MATERIAL_FIELD_DISTORTION) material_instance_set_property_u32(instance_id, MATERIAL_FIELD_DISTORTION, ov->distortion);
    if (mask & MATERIAL_FIELD_ROUGHNESS) material_instance_set_property_u32(instance_id, MATERIAL_FIELD_ROUGHNESS, ov->roughness);
    for (uint32_t i = 0; i < MATERIAL_EXT_COUNT; ++i) {
        if (ov->ext_present_mask & (1u << i)) material_instance_set_ext_u32(instance_id, i, ov->ext[i]);
    }
}

void matext_initialize(void) {
    for (uint32_t i = 0; i < MATEXT_MAX_PRESETS; ++i) g_presets[i].in_use = 0;
    g_preset_acrylic_light = 0; g_preset_acrylic_dark = 0;
    g_preset_acrylic_subtle = 0; g_preset_acrylic_strong = 0;
    g_reduce_transparency = 0;
}

/* --- Custom Materials --- */

uint32_t matext_custom_derive(const char *name, uint32_t base_definition_id,
                                const material_properties_t *override_props,
                                uint32_t field_mask) {
    if (!name || name[0] == '\0') return 0;
    material_properties_t base;
    if (material_definition_properties(base_definition_id, &base) != 0) return 0;
    if (override_props) apply_fields(&base, override_props, field_mask);
    uint32_t base_type = material_definition_type(base_definition_id);
    return material_register(name, base_type, &base);
}

/* --- Presets --- */

uint32_t matext_preset_register(const char *name, uint32_t definition_id,
                                  const material_properties_t *override_props,
                                  uint32_t field_mask) {
    if (!name || name[0] == '\0' || !material_definition_exists(definition_id)) return 0;
    for (uint32_t i = 0; i < MATEXT_MAX_PRESETS; ++i) {
        if (!g_presets[i].in_use) {
            g_presets[i].in_use = 1;
            copy_name24(g_presets[i].name, name);
            g_presets[i].definition_id = definition_id;
            if (override_props) g_presets[i].override = *override_props;
            else zero_props(&g_presets[i].override);
            g_presets[i].field_mask = field_mask;
            return i + 1u;
        }
    }
    return 0;
}

static int preset_exists(uint32_t preset_id) {
    return (preset_id != 0 && preset_id <= MATEXT_MAX_PRESETS && g_presets[preset_id - 1u].in_use);
}

uint32_t matext_preset_definition(uint32_t preset_id) {
    if (!preset_exists(preset_id)) return 0;
    return g_presets[preset_id - 1u].definition_id;
}

int matext_preset_apply(uint32_t preset_id, uint32_t instance_id) {
    if (!preset_exists(preset_id) || !material_instance_exists(instance_id)) return -1;
    preset_t *p = &g_presets[preset_id - 1u];
    if (material_instance_rebind(instance_id, p->definition_id) != 0) return -1;
    apply_fields_to_instance(instance_id, &p->override, p->field_mask);
    return 0;
}

uint32_t matext_preset_find(const char *name) {
    if (!name || name[0] == '\0') return 0;
    for (uint32_t i = 0; i < MATEXT_MAX_PRESETS; ++i) {
        if (!g_presets[i].in_use) continue;
        uint32_t j = 0;
        for (; g_presets[i].name[j] && name[j] && g_presets[i].name[j] == name[j]; ++j) { }
        if (g_presets[i].name[j] == '\0' && name[j] == '\0') return i + 1u;
    }
    return 0;
}

static uint32_t register_preset_variant(const char *name, uint32_t definition_id,
                                          uint32_t field_mask, material_properties_t ov) {
    return matext_preset_register(name, definition_id, &ov, field_mask);
}

void matext_presets_register_standard(void) {
    material_properties_t ov;

    /* Acrylic (PRESET-0001 Abschnitt 4). */
    zero_props(&ov); ov.color_rgb = 0xF7F7F7u; ov.tint_rgb = 0xFFFFFFu; ov.tint_strength = 20u; ov.opacity = 220u;
    g_preset_acrylic_light = register_preset_variant("AcrylicLight", MATERIAL_ID_ACRYLIC,
        MATERIAL_FIELD_COLOR | MATERIAL_FIELD_TINT_RGB | MATERIAL_FIELD_TINT_STRENGTH | MATERIAL_FIELD_OPACITY, ov);
    zero_props(&ov); ov.color_rgb = 0x202020u; ov.tint_rgb = 0x000000u; ov.tint_strength = 40u; ov.opacity = 200u;
    g_preset_acrylic_dark = register_preset_variant("AcrylicDark", MATERIAL_ID_ACRYLIC,
        MATERIAL_FIELD_COLOR | MATERIAL_FIELD_TINT_RGB | MATERIAL_FIELD_TINT_STRENGTH | MATERIAL_FIELD_OPACITY, ov);
    zero_props(&ov); ov.blur_radius = 3u; ov.reflection = 10u; ov.highlight = 5u; ov.opacity = 180u;
    g_preset_acrylic_subtle = register_preset_variant("AcrylicSubtle", MATERIAL_ID_ACRYLIC,
        MATERIAL_FIELD_BLUR | MATERIAL_FIELD_REFLECTION | MATERIAL_FIELD_HIGHLIGHT | MATERIAL_FIELD_OPACITY, ov);
    zero_props(&ov); ov.blur_radius = 12u; ov.reflection = 60u; ov.highlight = 40u; ov.opacity = 220u;
    g_preset_acrylic_strong = register_preset_variant("AcrylicStrong", MATERIAL_ID_ACRYLIC,
        MATERIAL_FIELD_BLUR | MATERIAL_FIELD_REFLECTION | MATERIAL_FIELD_HIGHLIGHT | MATERIAL_FIELD_OPACITY, ov);

    /* Liquid Glass. */
    zero_props(&ov); ov.refraction = 60u; ov.distortion = 30u; ov.opacity = 160u;
    register_preset_variant("LiquidGlassClear", MATERIAL_ID_LIQUID,
        MATERIAL_FIELD_REFRACTION | MATERIAL_FIELD_DISTORTION | MATERIAL_FIELD_OPACITY, ov);
    zero_props(&ov); ov.refraction = 100u; ov.distortion = 60u; ov.reflection = 90u;
    register_preset_variant("LiquidGlassSoft", MATERIAL_ID_LIQUID,
        MATERIAL_FIELD_REFRACTION | MATERIAL_FIELD_DISTORTION | MATERIAL_FIELD_REFLECTION, ov);
    zero_props(&ov); ov.refraction = 200u; ov.distortion = 150u; ov.reflection = 180u;
    register_preset_variant("LiquidGlassDynamic", MATERIAL_ID_LIQUID,
        MATERIAL_FIELD_REFRACTION | MATERIAL_FIELD_DISTORTION | MATERIAL_FIELD_REFLECTION, ov);
    zero_props(&ov); ov.refraction = 30u; ov.distortion = 10u; ov.reflection = 40u;
    register_preset_variant("LiquidGlassMinimal", MATERIAL_ID_LIQUID,
        MATERIAL_FIELD_REFRACTION | MATERIAL_FIELD_DISTORTION | MATERIAL_FIELD_REFLECTION, ov);

    /* Frosted Glass. */
    zero_props(&ov); ov.blur_radius = 6u; ov.opacity = 220u;
    register_preset_variant("FrostedLight", MATERIAL_ID_FROSTED, MATERIAL_FIELD_BLUR | MATERIAL_FIELD_OPACITY, ov);
    zero_props(&ov); ov.color_rgb = 0x202020u; ov.blur_radius = 14u; ov.opacity = 200u;
    register_preset_variant("FrostedDark", MATERIAL_ID_FROSTED,
        MATERIAL_FIELD_COLOR | MATERIAL_FIELD_BLUR | MATERIAL_FIELD_OPACITY, ov);
    zero_props(&ov); ov.blur_radius = 4u;
    register_preset_variant("FrostedSoft", MATERIAL_ID_FROSTED, MATERIAL_FIELD_BLUR, ov);
    zero_props(&ov); ov.blur_radius = 16u;
    register_preset_variant("FrostedStrong", MATERIAL_ID_FROSTED, MATERIAL_FIELD_BLUR, ov);

    /* Crystal. */
    zero_props(&ov); ov.refraction = 220u; ov.reflection = 180u; ov.opacity = 50u;
    register_preset_variant("CrystalClear", MATERIAL_ID_CRYSTAL,
        MATERIAL_FIELD_REFRACTION | MATERIAL_FIELD_REFLECTION | MATERIAL_FIELD_OPACITY, ov);
    zero_props(&ov); ov.refraction = 150u; ov.roughness = 80u;
    register_preset_variant("CrystalSoft", MATERIAL_ID_CRYSTAL, MATERIAL_FIELD_REFRACTION | MATERIAL_FIELD_ROUGHNESS, ov);
    zero_props(&ov); ov.reflection = 230u; ov.highlight = 220u;
    register_preset_variant("CrystalReflective", MATERIAL_ID_CRYSTAL, MATERIAL_FIELD_REFLECTION | MATERIAL_FIELD_HIGHLIGHT, ov);
    zero_props(&ov); ov.refraction = 60u; ov.reflection = 40u; ov.highlight = 30u;
    register_preset_variant("CrystalMinimal", MATERIAL_ID_CRYSTAL,
        MATERIAL_FIELD_REFRACTION | MATERIAL_FIELD_REFLECTION | MATERIAL_FIELD_HIGHLIGHT, ov);

    /* Solid. */
    zero_props(&ov); ov.color_rgb = 0xF0F0F0u;
    register_preset_variant("SolidLight", MATERIAL_ID_SOLID, MATERIAL_FIELD_COLOR, ov);
    zero_props(&ov); ov.color_rgb = 0x1A1A1Au;
    register_preset_variant("SolidDark", MATERIAL_ID_SOLID, MATERIAL_FIELD_COLOR, ov);
    zero_props(&ov); ov.color_rgb = 0x0078D4u;
    register_preset_variant("SolidAccent", MATERIAL_ID_SOLID, MATERIAL_FIELD_COLOR, ov);
    zero_props(&ov); ov.color_rgb = 0x808080u; ov.tint_strength = 50u;
    register_preset_variant("SolidTransparent", MATERIAL_ID_SOLID, MATERIAL_FIELD_COLOR | MATERIAL_FIELD_TINT_STRENGTH, ov);
}

uint32_t matext_preset_acrylic_light(void) { return g_preset_acrylic_light; }
uint32_t matext_preset_acrylic_dark(void) { return g_preset_acrylic_dark; }
uint32_t matext_preset_acrylic_subtle(void) { return g_preset_acrylic_subtle; }
uint32_t matext_preset_acrylic_strong(void) { return g_preset_acrylic_strong; }

/* --- Barrierefreiheit --- */

void matext_accessibility_set_reduce_transparency(int enabled) { g_reduce_transparency = enabled ? 1 : 0; }
int matext_accessibility_reduce_transparency(void) { return g_reduce_transparency; }

void matext_accessibility_apply(material_properties_t *props) {
    if (!props || !g_reduce_transparency) return;
    if (props->opacity < 230u) props->opacity = 230u;
    props->blur_radius = 0u;
    props->reflection = 0u;
    props->highlight = 0u;
    props->distortion = 0u;
}

int matext_accessibility_should_sample_backdrop(void) { return g_reduce_transparency ? 0 : 1; }

static uint32_t luminance(uint32_t rgb) {
    uint32_t r = (uint8_t)(rgb >> 16), g = (uint8_t)(rgb >> 8), b = (uint8_t)rgb;
    return (r * 299u + g * 587u + b * 114u) / 1000u;
}

int matext_contrast_sufficient(uint32_t foreground_rgb, uint32_t background_rgb) {
    uint32_t lf = luminance(foreground_rgb), lb = luminance(background_rgb);
    uint32_t diff = lf > lb ? lf - lb : lb - lf;
    return diff >= 125u ? 1 : 0;
}

static uint8_t step_channel(uint8_t c, int toward_white, uint32_t step) {
    if (toward_white) {
        uint32_t v = (uint32_t)c + step;
        return (uint8_t)(v > 255u ? 255u : v);
    }
    uint32_t v = (c > step) ? (uint32_t)c - step : 0u;
    return (uint8_t)v;
}

void matext_contrast_ensure(material_properties_t *props, uint32_t foreground_rgb) {
    if (!props) return;
    int toward_white = (luminance(foreground_rgb) < 128u);

    for (uint32_t iter = 0; iter < 32u && !matext_contrast_sufficient(foreground_rgb, props->color_rgb); ++iter) {
        uint8_t r = (uint8_t)(props->color_rgb >> 16);
        uint8_t g = (uint8_t)(props->color_rgb >> 8);
        uint8_t b = (uint8_t)props->color_rgb;
        r = step_channel(r, toward_white, 8u);
        g = step_channel(g, toward_white, 8u);
        b = step_channel(b, toward_white, 8u);
        uint32_t new_rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
        if (new_rgb == props->color_rgb) break;
        props->color_rgb = new_rgb;
    }
}

int matext_self_test(void) {
    material_initialize();
    materialtypes_initialize();
    matext_initialize();

    /* Custom: Ableitung von Acrylic mit geaenderter Farbe, Basis bleibt
       unveraendert. */
    material_properties_t ov;
    zero_props(&ov);
    ov.color_rgb = 0xABCDEFu;
    uint32_t custom_def = matext_custom_derive("my-acrylic", MATERIAL_ID_ACRYLIC, &ov, MATERIAL_FIELD_COLOR);
    if (custom_def == 0) return -1;
    if (material_definition_type(custom_def) != MATERIAL_TYPE_ACRYLIC) return -1;
    material_properties_t base_check;
    material_definition_properties(custom_def, &base_check);
    if (base_check.color_rgb != 0xABCDEFu) return -1;
    material_properties_t original_base;
    material_definition_properties(MATERIAL_ID_ACRYLIC, &original_base);
    if (original_base.color_rgb == 0xABCDEFu) return -1; /* Basis unveraendert */

    /* Preset: Registrierung + Anwendung auf eine Instanz, Preset-
       Definition selbst bleibt unveraendert. */
    zero_props(&ov);
    ov.opacity = 77u;
    uint32_t preset = matext_preset_register("test-preset", MATERIAL_ID_CRYSTAL, &ov, MATERIAL_FIELD_OPACITY);
    if (preset == 0) return -1;
    if (matext_preset_definition(preset) != MATERIAL_ID_CRYSTAL) return -1;
    uint32_t inst = material_instance_create(MATERIAL_ID_SOLID);
    if (matext_preset_apply(preset, inst) != 0) return -1;
    if (material_instance_definition(inst) != MATERIAL_ID_CRYSTAL) return -1;
    material_properties_t resolved;
    material_instance_resolve(inst, &resolved);
    if (resolved.opacity != 77u) return -1;
    /* Instanz kann danach weiterhin individuell ueberschreiben, ohne das
       Preset zu veraendern. */
    material_instance_set_property_u32(inst, MATERIAL_FIELD_OPACITY, 10u);
    material_instance_resolve(inst, &resolved);
    if (resolved.opacity != 10u) return -1;
    uint32_t inst2 = material_instance_create(MATERIAL_ID_SOLID);
    matext_preset_apply(preset, inst2);
    material_instance_resolve(inst2, &resolved);
    if (resolved.opacity != 77u) return -1; /* unbeeinflusst von inst's Aenderung */

    /* Standard-Presets. */
    matext_presets_register_standard();
    if (matext_preset_acrylic_light() == 0) return -1;
    if (matext_preset_definition(matext_preset_acrylic_light()) != MATERIAL_ID_ACRYLIC) return -1;
    uint32_t found = matext_preset_find("CrystalReflective");
    if (found == 0) return -1;
    if (matext_preset_definition(found) != MATERIAL_ID_CRYSTAL) return -1;
    if (matext_preset_find("DoesNotExist") != 0) return -1;

    /* Accessibility: Transparenzreduzierung erzwingt Deckkraft + keine
       Backdrop-Abhaengigkeit. */
    matext_accessibility_set_reduce_transparency(0);
    if (matext_accessibility_should_sample_backdrop() != 1) return -1;
    material_properties_t glassy;
    zero_props(&glassy);
    glassy.opacity = 100u; glassy.blur_radius = 8u; glassy.reflection = 50u;
    matext_accessibility_apply(&glassy); /* bei deaktiviert: keine Aenderung */
    if (glassy.opacity != 100u) return -1;
    matext_accessibility_set_reduce_transparency(1);
    if (matext_accessibility_should_sample_backdrop() != 0) return -1;
    matext_accessibility_apply(&glassy);
    if (glassy.opacity < 230u || glassy.blur_radius != 0u || glassy.reflection != 0u) return -1;
    matext_accessibility_set_reduce_transparency(0);

    /* Kontrast: Schwarz-auf-Schwarz ist unzureichend, Schwarz-auf-Weiss
       ausreichend; matext_contrast_ensure() muss aus unzureichendem
       Kontrast ausreichenden machen. */
    if (matext_contrast_sufficient(0x000000u, 0x101010u)) return -1;
    if (!matext_contrast_sufficient(0x000000u, 0xFFFFFFu)) return -1;
    material_properties_t low_contrast;
    zero_props(&low_contrast);
    low_contrast.color_rgb = 0x101010u;
    matext_contrast_ensure(&low_contrast, 0x000000u); /* dunkler Vordergrund -> Hintergrund muss heller werden */
    if (!matext_contrast_sufficient(0x000000u, low_contrast.color_rgb)) return -1;

    return 0;
}
