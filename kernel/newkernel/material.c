/* NovaOS C-Kernel - Nova Material Engine: Material System (siehe
   material.h).

   Dynamisch wachsende Definitions-/Instanz-Tabellen (2026-11-05, Antwort
   auf "viele feste Grenzen nicht dynamisch loesbar" - siehe
   [[project_general_allocator]], dreizehntes umgestelltes Modul).
   MATERIAL_MAX_DEFINITIONS/MATERIAL_MAX_INSTANCES (material.h) sind
   jetzt nur noch Startkapazitaeten. definition_id/instance_id sind in
   diesem Modul (anders als die meisten anderen bisher umgestellten)
   DIREKTE 1-basierte Array-Indizes (`i+1u`), kein separat gezaehlter
   "naechster ID"-Zaehler - das bleibt nach einem krealloc()-Wachstum
   unveraendert gueltig, da bereits vergebene IDs weiterhin dieselben
   (jetzt ggf. verschobenen, aber weiterhin per Index gueltigen) Slots
   referenzieren. */
#include "material.h"
#include "kalloc.h"
#include <stddef.h>

typedef struct {
    uint32_t in_use;
    char name[24];
    uint32_t type;
    material_properties_t base;
    uint32_t state_mask[MATERIAL_STATE_COUNT];
    material_properties_t state_override[MATERIAL_STATE_COUNT];
} material_definition_t;

typedef struct {
    uint32_t in_use;
    uint32_t definition_id; /* 1-basiert */
    uint32_t state;
    uint32_t override_mask;
    material_properties_t override;
    uint32_t generation;
} material_instance_t;

static material_definition_t *g_defs = NULL;
static uint32_t g_defs_capacity = 0;
static material_instance_t *g_inst = NULL;
static uint32_t g_inst_capacity = 0;
static uint32_t g_def_count = 0;

/* Verdoppelt g_defs[]/g_inst[] (identisches Muster wie die bereits
   umgestellten Module). 0 = Erfolg (*out_slot = erster freier Slot),
   -1 bei Erschoepfung (Tabelle/Kapazitaet bleiben unveraendert gueltig). */
static int defs_grow(uint32_t *out_slot) {
    uint32_t new_capacity = (g_defs_capacity == 0u) ? MATERIAL_MAX_DEFINITIONS : g_defs_capacity * 2u;
    material_definition_t *grown = (material_definition_t *)krealloc(
        g_defs, new_capacity * (uint32_t)sizeof(material_definition_t));
    if (!grown) return -1;
    for (uint32_t i = g_defs_capacity; i < new_capacity; ++i) grown[i] = (material_definition_t){0};
    *out_slot = g_defs_capacity;
    g_defs = grown;
    g_defs_capacity = new_capacity;
    return 0;
}

static int inst_grow(uint32_t *out_slot) {
    uint32_t new_capacity = (g_inst_capacity == 0u) ? MATERIAL_MAX_INSTANCES : g_inst_capacity * 2u;
    material_instance_t *grown = (material_instance_t *)krealloc(
        g_inst, new_capacity * (uint32_t)sizeof(material_instance_t));
    if (!grown) return -1;
    for (uint32_t i = g_inst_capacity; i < new_capacity; ++i) grown[i] = (material_instance_t){0};
    *out_slot = g_inst_capacity;
    g_inst = grown;
    g_inst_capacity = new_capacity;
    return 0;
}

static void zero_properties(material_properties_t *p) {
    p->color_rgb = 0; p->tint_rgb = 0; p->tint_strength = 0; p->opacity = 255;
    p->blur_radius = 0; p->refraction = 0; p->reflection = 0; p->highlight = 0;
    p->distortion = 0; p->roughness = 0; p->ext_present_mask = 0;
    for (uint32_t i = 0; i < MATERIAL_EXT_COUNT; ++i) p->ext[i] = 0;
}

static void copy_name(char *dst, const char *src) {
    uint32_t i = 0;
    for (; i < 23 && src && src[i]; ++i) dst[i] = src[i];
    dst[i] = '\0';
}

static void apply_override(material_properties_t *base, const material_properties_t *ov, uint32_t mask) {
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

void material_initialize(void) {
    if (g_defs) kfree(g_defs);
    g_defs = (material_definition_t *)kcalloc(MATERIAL_MAX_DEFINITIONS, sizeof(material_definition_t));
    g_defs_capacity = g_defs ? MATERIAL_MAX_DEFINITIONS : 0u;

    if (g_inst) kfree(g_inst);
    g_inst = (material_instance_t *)kcalloc(MATERIAL_MAX_INSTANCES, sizeof(material_instance_t));
    g_inst_capacity = g_inst ? MATERIAL_MAX_INSTANCES : 0u;

    g_def_count = 0;
}

uint32_t material_register(const char *name, uint32_t type, const material_properties_t *defaults) {
    if (!name || name[0] == '\0') return 0;
    uint32_t slot;
    for (slot = 0; slot < g_defs_capacity; ++slot) {
        if (!g_defs[slot].in_use) break;
    }
    if (slot >= g_defs_capacity) {
        if (defs_grow(&slot) != 0) return 0;
    }
    g_defs[slot].in_use = 1;
    copy_name(g_defs[slot].name, name);
    g_defs[slot].type = type;
    if (defaults) g_defs[slot].base = *defaults;
    else zero_properties(&g_defs[slot].base);
    for (uint32_t s = 0; s < MATERIAL_STATE_COUNT; ++s) g_defs[slot].state_mask[s] = 0;
    ++g_def_count;
    return slot + 1u;
}

int material_set_state_override(uint32_t definition_id, uint32_t state,
                                  const material_properties_t *props, uint32_t field_mask) {
    if (definition_id == 0 || definition_id > g_defs_capacity) return -1;
    if (state >= MATERIAL_STATE_COUNT || !props) return -1;
    material_definition_t *d = &g_defs[definition_id - 1u];
    if (!d->in_use) return -1;
    d->state_override[state] = *props;
    d->state_mask[state] = field_mask;
    return 0;
}

int material_definition_exists(uint32_t definition_id) {
    if (definition_id == 0 || definition_id > g_defs_capacity) return 0;
    return g_defs[definition_id - 1u].in_use ? 1 : 0;
}

uint32_t material_definition_type(uint32_t definition_id) {
    if (!material_definition_exists(definition_id)) return 0xFFFFFFFFu;
    return g_defs[definition_id - 1u].type;
}

int material_definition_properties(uint32_t definition_id, material_properties_t *out) {
    if (!material_definition_exists(definition_id) || !out) return -1;
    *out = g_defs[definition_id - 1u].base;
    return 0;
}

uint32_t material_fallback_id(void) { return MATERIAL_ID_SOLID; }

uint32_t material_instance_create(uint32_t definition_id) {
    if (!material_definition_exists(definition_id)) definition_id = material_fallback_id();
    uint32_t slot;
    for (slot = 0; slot < g_inst_capacity; ++slot) {
        if (!g_inst[slot].in_use) break;
    }
    if (slot >= g_inst_capacity) {
        if (inst_grow(&slot) != 0) return 0;
    }
    g_inst[slot].in_use = 1;
    g_inst[slot].definition_id = definition_id;
    g_inst[slot].state = MATERIAL_STATE_NORMAL;
    g_inst[slot].override_mask = 0;
    g_inst[slot].generation = 0;
    zero_properties(&g_inst[slot].override);
    return slot + 1u;
}

int material_instance_destroy(uint32_t instance_id) {
    if (!material_instance_exists(instance_id)) return -1;
    g_inst[instance_id - 1u].in_use = 0;
    return 0;
}

int material_instance_exists(uint32_t instance_id) {
    if (instance_id == 0 || instance_id > g_inst_capacity) return 0;
    return g_inst[instance_id - 1u].in_use ? 1 : 0;
}

static material_instance_t *inst_at(uint32_t instance_id) {
    if (!material_instance_exists(instance_id)) return 0;
    return &g_inst[instance_id - 1u];
}

int material_instance_set_property_u32(uint32_t instance_id, uint32_t field, uint32_t value) {
    material_instance_t *in = inst_at(instance_id);
    if (!in) return -1;
    if (field != MATERIAL_FIELD_BLUR && value > 255u) value = 255u;
    if (field == MATERIAL_FIELD_BLUR && value > 16u) value = 16u;
    switch (field) {
        case MATERIAL_FIELD_COLOR: in->override.color_rgb = value; break;
        case MATERIAL_FIELD_TINT_RGB: in->override.tint_rgb = value; break;
        case MATERIAL_FIELD_TINT_STRENGTH: in->override.tint_strength = value; break;
        case MATERIAL_FIELD_OPACITY: in->override.opacity = value; break;
        case MATERIAL_FIELD_BLUR: in->override.blur_radius = value; break;
        case MATERIAL_FIELD_REFRACTION: in->override.refraction = value; break;
        case MATERIAL_FIELD_REFLECTION: in->override.reflection = value; break;
        case MATERIAL_FIELD_HIGHLIGHT: in->override.highlight = value; break;
        case MATERIAL_FIELD_DISTORTION: in->override.distortion = value; break;
        case MATERIAL_FIELD_ROUGHNESS: in->override.roughness = value; break;
        default: return -1;
    }
    in->override_mask |= field;
    ++in->generation;
    return 0;
}

int material_instance_set_ext_u32(uint32_t instance_id, uint32_t ext_key, uint32_t value) {
    material_instance_t *in = inst_at(instance_id);
    if (!in || ext_key >= MATERIAL_EXT_COUNT) return -1;
    in->override.ext[ext_key] = value;
    in->override.ext_present_mask |= (1u << ext_key);
    ++in->generation;
    return 0;
}

int material_instance_clear_overrides(uint32_t instance_id) {
    material_instance_t *in = inst_at(instance_id);
    if (!in) return -1;
    in->override_mask = 0;
    zero_properties(&in->override);
    ++in->generation;
    return 0;
}

int material_instance_clear_property(uint32_t instance_id, uint32_t field) {
    material_instance_t *in = inst_at(instance_id);
    if (!in) return -1;
    in->override_mask &= ~field;
    ++in->generation;
    return 0;
}

int material_instance_set_state(uint32_t instance_id, uint32_t state) {
    material_instance_t *in = inst_at(instance_id);
    if (!in || state >= MATERIAL_STATE_COUNT) return -1;
    in->state = state;
    ++in->generation;
    return 0;
}

uint32_t material_instance_state(uint32_t instance_id) {
    material_instance_t *in = inst_at(instance_id);
    return in ? in->state : 0xFFFFFFFFu;
}

uint32_t material_instance_definition(uint32_t instance_id) {
    material_instance_t *in = inst_at(instance_id);
    return in ? in->definition_id : 0;
}

uint32_t material_instance_generation(uint32_t instance_id) {
    material_instance_t *in = inst_at(instance_id);
    return in ? in->generation : 0xFFFFFFFFu;
}

int material_instance_rebind(uint32_t instance_id, uint32_t new_definition_id) {
    material_instance_t *in = inst_at(instance_id);
    if (!in || !material_definition_exists(new_definition_id)) return -1;
    in->definition_id = new_definition_id;
    ++in->generation;
    return 0;
}

int material_instance_resolve(uint32_t instance_id, material_properties_t *out) {
    material_instance_t *in = inst_at(instance_id);
    if (!in || !out) return -1;
    uint32_t definition_id = in->definition_id;
    if (!material_definition_exists(definition_id)) definition_id = material_fallback_id();
    material_definition_t *d = &g_defs[definition_id - 1u];

    *out = d->base;
    apply_override(out, &d->state_override[in->state], d->state_mask[in->state]);
    apply_override(out, &in->override, in->override_mask);

    if (d->type == MATERIAL_TYPE_SOLID) {
        out->opacity = 255u;
        out->blur_radius = 0u;
        out->refraction = 0u;
        out->reflection = 0u;
        out->distortion = 0u;
    }
    return 0;
}

int material_self_test(void) {
    material_initialize();
    if (g_def_count != 0u) return -1;

    material_properties_t defaults;
    zero_properties(&defaults);
    defaults.color_rgb = 0x112233u;
    defaults.opacity = 200u;
    defaults.blur_radius = 6u;

    uint32_t def_id = material_register("test-acrylic", MATERIAL_TYPE_ACRYLIC, &defaults);
    if (def_id == 0) return -1;
    if (!material_definition_exists(def_id)) return -1;
    if (material_definition_type(def_id) != MATERIAL_TYPE_ACRYLIC) return -1;

    uint32_t inst_id = material_instance_create(def_id);
    if (inst_id == 0) return -1;

    material_properties_t resolved;
    if (material_instance_resolve(inst_id, &resolved) != 0) return -1;
    if (resolved.color_rgb != 0x112233u || resolved.opacity != 200u) return -1;

    /* Instanz-Override darf die Definition nicht veraendern. */
    if (material_instance_set_property_u32(inst_id, MATERIAL_FIELD_OPACITY, 90u) != 0) return -1;
    if (material_instance_resolve(inst_id, &resolved) != 0) return -1;
    if (resolved.opacity != 90u) return -1;

    uint32_t inst_id2 = material_instance_create(def_id);
    material_properties_t resolved2;
    if (material_instance_resolve(inst_id2, &resolved2) != 0) return -1;
    if (resolved2.opacity != 200u) return -1; /* zweite Instanz unbeeinflusst */

    /* Zustandsueberlagerung. */
    material_properties_t hover_props;
    zero_properties(&hover_props);
    hover_props.highlight = 80u;
    if (material_set_state_override(def_id, MATERIAL_STATE_HOVER, &hover_props, MATERIAL_FIELD_HIGHLIGHT) != 0) return -1;
    if (material_instance_set_state(inst_id2, MATERIAL_STATE_HOVER) != 0) return -1;
    if (material_instance_resolve(inst_id2, &resolved2) != 0) return -1;
    if (resolved2.highlight != 80u) return -1;
    if (resolved2.opacity != 200u) return -1; /* nicht ueberlagertes Feld bleibt Basiswert */

    /* Ungueltige Definition-ID faellt auf die Solid-Fallback-ID zurueck
       (SYSTEM-0001 Abschnitt 8). Solid selbst ist in diesem isolierten
       Selbsttest - ohne materialtypes.c - nicht registriert; resolve()
       muss das trotzdem fehlerfrei (mit Nullwerten) abhandeln. */
    uint32_t bad_inst = material_instance_create(999u);
    if (material_instance_definition(bad_inst) != material_fallback_id()) return -1;
    material_properties_t dummy;
    if (material_instance_resolve(bad_inst, &dummy) != 0) return -1;

    /* Generation steigt bei aendernden Operationen, bleibt bei lesenden
       Zugriffen unveraendert (CACHE-0001-Grundlage). */
    uint32_t gen_before = material_instance_generation(inst_id);
    material_instance_resolve(inst_id, &resolved);
    if (material_instance_generation(inst_id) != gen_before) return -1;
    material_instance_set_property_u32(inst_id, MATERIAL_FIELD_ROUGHNESS, 5u);
    if (material_instance_generation(inst_id) == gen_before) return -1;

    if (material_instance_destroy(inst_id) != 0) return -1;
    if (material_instance_exists(inst_id)) return -1;

    /* Beweist die eigentliche Aenderung dieser Session: mehr Definitionen
       UND mehr Instanzen anlegen, als die urspruenglichen festen
       Kapazitaeten je erlaubt haetten - mindestens je ein defs_grow()/
       inst_grow() ist dafuer noetig. Definitionswachstum bewusst klein
       (+5) gehalten, um den obigen `999u`-Invalid-ID-Test nicht
       nachtraeglich gueltig werden zu lassen (der lief bereits VOR
       dieser Stelle, aber auch kuenftige Umordnungen bleiben so sicher). */
    uint32_t def_growth_count = MATERIAL_MAX_DEFINITIONS + 5u;
    uint32_t last_def = 0;
    for (uint32_t i = 0; i < def_growth_count; ++i) {
        material_properties_t d;
        zero_properties(&d);
        last_def = material_register("growth-def", MATERIAL_TYPE_CUSTOM, &d);
        if (last_def == 0) return -1;
    }
    if (!material_definition_exists(last_def)) return -1;
    if (last_def >= 999u) return -1; /* darf den obigen Invalid-ID-Test nicht beruehren */

    uint32_t inst_growth_count = MATERIAL_MAX_INSTANCES + 5u;
    uint32_t last_growth_inst = 0;
    for (uint32_t i = 0; i < inst_growth_count; ++i) {
        last_growth_inst = material_instance_create(last_def);
        if (last_growth_inst == 0) return -1;
    }
    if (material_instance_definition(last_growth_inst) != last_def) return -1;

    return 0;
}
