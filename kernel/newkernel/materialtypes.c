/* NovaOS C-Kernel - Nova Material Engine: Standard-Materialtypen (siehe
   materialtypes.h). */
#include "materialtypes.h"
#include "material.h"

static material_properties_t make_props(void) {
    material_properties_t p;
    p.color_rgb = 0; p.tint_rgb = 0; p.tint_strength = 0; p.opacity = 255;
    p.blur_radius = 0; p.refraction = 0; p.reflection = 0; p.highlight = 0;
    p.distortion = 0; p.roughness = 0; p.ext_present_mask = 0;
    for (uint32_t i = 0; i < MATERIAL_EXT_COUNT; ++i) p.ext[i] = 0;
    return p;
}

static void set_ext(material_properties_t *p, uint32_t key, uint32_t value) {
    p->ext[key] = value;
    p->ext_present_mask |= (1u << key);
}

void materialtypes_initialize(void) {
    /* ACRYLIC (ACRYLIC-0001 Abschnitt 2/3): transluzent, mittlere
       Unschaerfe, dezente Farbueberlagerung und leichte Reflexionen,
       standardmaessig ohne Lichtbrechung/Verformung. */
    material_properties_t acrylic = make_props();
    acrylic.color_rgb = 0xF2F2F2u;
    acrylic.opacity = 200u;
    acrylic.tint_rgb = 0xFFFFFFu;
    acrylic.tint_strength = 40u;
    acrylic.blur_radius = 6u;
    acrylic.reflection = 30u;
    acrylic.highlight = 20u;
    acrylic.roughness = 120u;
    set_ext(&acrylic, MATERIAL_EXT_SATURATION, 180u);
    set_ext(&acrylic, MATERIAL_EXT_NOISE, 10u);
    material_register("acrylic", MATERIAL_TYPE_ACRYLIC, &acrylic);

    /* LIQUID GLASS (LIQUID-0001 Abschnitt 2/3): Lichtbrechung, Reflexionen
       und dynamische Verformung, glatte (niedrige Roughness) Oberflaeche. */
    material_properties_t liquid = make_props();
    liquid.color_rgb = 0xDDEEFFu;
    liquid.opacity = 180u;
    liquid.tint_rgb = 0xBFE3FFu;
    liquid.tint_strength = 20u;
    liquid.blur_radius = 4u;
    liquid.refraction = 160u;
    liquid.reflection = 140u;
    liquid.highlight = 160u;
    liquid.distortion = 90u;
    liquid.roughness = 40u;
    set_ext(&liquid, MATERIAL_EXT_THICKNESS, 120u);
    set_ext(&liquid, MATERIAL_EXT_EDGE_INTENSITY, 180u);
    set_ext(&liquid, MATERIAL_EXT_FLUIDITY, 150u);
    set_ext(&liquid, MATERIAL_EXT_LIGHT_RESPONSE, 160u);
    material_register("liquid-glass", MATERIAL_TYPE_LIQUID, &liquid);

    /* FROSTED GLASS (FROSTED-0001 Abschnitt 2/3): starke Unschaerfe,
       diffuse Streuung (hohe Roughness), keine ausgepraegte Brechung/
       Verformung. */
    material_properties_t frosted = make_props();
    frosted.color_rgb = 0xFFFFFFu;
    frosted.opacity = 210u;
    frosted.tint_rgb = 0xFFFFFFu;
    frosted.tint_strength = 50u;
    frosted.blur_radius = 14u;
    frosted.roughness = 200u;
    set_ext(&frosted, MATERIAL_EXT_SATURATION, 140u);
    set_ext(&frosted, MATERIAL_EXT_DIFFUSION, 200u);
    set_ext(&frosted, MATERIAL_EXT_NOISE, 30u);
    set_ext(&frosted, MATERIAL_EXT_BRIGHTNESS, 10u);
    material_register("frosted-glass", MATERIAL_TYPE_FROSTED, &frosted);

    /* CRYSTAL (CRYSTAL-0001 Abschnitt 2/3): hohe Transparenz, starke
       Brechung/Reflexion/Glanzlichter, scharfe (niedrige Roughness)
       Oberflaeche, standardmaessig ohne ausgepraegte Unschaerfe. */
    material_properties_t crystal = make_props();
    crystal.color_rgb = 0xFFFFFFu;
    crystal.opacity = 60u;
    crystal.tint_rgb = 0xFFFFFFu;
    crystal.tint_strength = 10u;
    crystal.refraction = 220u;
    crystal.reflection = 180u;
    crystal.highlight = 200u;
    crystal.roughness = 20u;
    set_ext(&crystal, MATERIAL_EXT_THICKNESS, 150u);
    set_ext(&crystal, MATERIAL_EXT_EDGE_INTENSITY, 220u);
    set_ext(&crystal, MATERIAL_EXT_DISPERSION, 120u);
    set_ext(&crystal, MATERIAL_EXT_CLARITY, 230u);
    set_ext(&crystal, MATERIAL_EXT_LIGHT_RESPONSE, 180u);
    material_register("crystal", MATERIAL_TYPE_CRYSTAL, &crystal);

    /* SOLID (SOLID-0001 Abschnitt 2/3): vollstaendig undurchsichtig, keine
       Hintergrundabtastung/-effekte - material.c's resolve() erzwingt das
       zusaetzlich fuer jede Instanz dieses Typs. */
    material_properties_t solid = make_props();
    solid.color_rgb = 0x202020u;
    solid.opacity = 255u;
    set_ext(&solid, MATERIAL_EXT_BRIGHTNESS, 128u);
    set_ext(&solid, MATERIAL_EXT_SATURATION, 128u);
    set_ext(&solid, MATERIAL_EXT_BORDER_COLOR, 0x000000u);
    set_ext(&solid, MATERIAL_EXT_BORDER_THICKNESS, 0u);
    material_register("solid", MATERIAL_TYPE_SOLID, &solid);
}

int materialtypes_self_test(void) {
    material_initialize();
    materialtypes_initialize();

    if (material_definition_type(MATERIAL_ID_ACRYLIC) != MATERIAL_TYPE_ACRYLIC) return -1;
    if (material_definition_type(MATERIAL_ID_LIQUID) != MATERIAL_TYPE_LIQUID) return -1;
    if (material_definition_type(MATERIAL_ID_FROSTED) != MATERIAL_TYPE_FROSTED) return -1;
    if (material_definition_type(MATERIAL_ID_CRYSTAL) != MATERIAL_TYPE_CRYSTAL) return -1;
    if (material_definition_type(MATERIAL_ID_SOLID) != MATERIAL_TYPE_SOLID) return -1;

    /* Solid erzwingt volle Deckkraft und keine Glaseffekte, auch wenn eine
       Instanz etwas anderes anfordert. */
    uint32_t solid_inst = material_instance_create(MATERIAL_ID_SOLID);
    if (material_instance_set_property_u32(solid_inst, MATERIAL_FIELD_OPACITY, 50u) != 0) return -1;
    if (material_instance_set_property_u32(solid_inst, MATERIAL_FIELD_BLUR, 8u) != 0) return -1;
    material_properties_t resolved;
    if (material_instance_resolve(solid_inst, &resolved) != 0) return -1;
    if (resolved.opacity != 255u) return -1;
    if (resolved.blur_radius != 0u) return -1;

    /* Crystal ist standardmaessig sehr transparent und stark brechend. */
    uint32_t crystal_inst = material_instance_create(MATERIAL_ID_CRYSTAL);
    if (material_instance_resolve(crystal_inst, &resolved) != 0) return -1;
    if (resolved.opacity >= 128u) return -1;
    if (resolved.refraction < 128u) return -1;

    /* Fallback-ID verweist tatsaechlich auf ein registriertes Solid-Material. */
    if (!material_definition_exists(material_fallback_id())) return -1;
    if (material_definition_type(material_fallback_id()) != MATERIAL_TYPE_SOLID) return -1;

    return 0;
}
