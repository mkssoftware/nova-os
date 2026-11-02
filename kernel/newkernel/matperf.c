/* NovaOS C-Kernel - Nova Material Engine: Performance (siehe matperf.h
   fuer Scope). */
#include "matperf.h"

#define MATPERF_QUALITY_HYSTERESIS  3u
#define MATPERF_QUALITY_SLOW_TICKS  4u
#define MATPERF_QUALITY_FAST_TICKS  1u
#define MATPERF_MAX_PRIORITY_INSTANCES 16u

typedef struct {
    uint32_t in_use;
    uint32_t instance_id;
    uint32_t generation;
    material_properties_t cached;
} cache_entry_t;

typedef struct {
    uint32_t in_use;
    uint32_t instance_id;
    int high_priority;
} priority_entry_t;

static cache_entry_t g_cache[MATPERF_MAX_CACHE_ENTRIES];
static matperf_rect_t g_damage[MATPERF_MAX_DAMAGE_REGIONS];
static uint32_t g_damage_count = 0;
static uint32_t g_quality_level = MATPERF_QUALITY_HIGH;
static uint32_t g_slow_streak = 0, g_fast_streak = 0;
static priority_entry_t g_priority[MATPERF_MAX_PRIORITY_INSTANCES];

void matperf_initialize(void) {
    for (uint32_t i = 0; i < MATPERF_MAX_CACHE_ENTRIES; ++i) g_cache[i].in_use = 0;
    g_damage_count = 0;
    g_quality_level = MATPERF_QUALITY_HIGH;
    g_slow_streak = 0;
    g_fast_streak = 0;
    for (uint32_t i = 0; i < MATPERF_MAX_PRIORITY_INSTANCES; ++i) g_priority[i].in_use = 0;
}

/* --- Cache --- */

static cache_entry_t *find_cache_entry(uint32_t instance_id) {
    for (uint32_t i = 0; i < MATPERF_MAX_CACHE_ENTRIES; ++i)
        if (g_cache[i].in_use && g_cache[i].instance_id == instance_id) return &g_cache[i];
    return 0;
}

int matperf_cache_resolve(uint32_t instance_id, material_properties_t *out, int *out_was_cached) {
    if (!material_instance_exists(instance_id) || !out) return -1;
    uint32_t gen = material_instance_generation(instance_id);

    cache_entry_t *e = find_cache_entry(instance_id);
    if (e && e->generation == gen) {
        *out = e->cached;
        if (out_was_cached) *out_was_cached = 1;
        return 0;
    }

    if (material_instance_resolve(instance_id, out) != 0) return -1;
    if (!e) {
        for (uint32_t i = 0; i < MATPERF_MAX_CACHE_ENTRIES; ++i) {
            if (!g_cache[i].in_use) { e = &g_cache[i]; break; }
        }
        if (!e) e = &g_cache[0]; /* einfache Verdraengung bei voller Tabelle */
    }
    e->in_use = 1;
    e->instance_id = instance_id;
    e->generation = gen;
    e->cached = *out;
    if (out_was_cached) *out_was_cached = 0;
    return 0;
}

void matperf_cache_invalidate(uint32_t instance_id) {
    cache_entry_t *e = find_cache_entry(instance_id);
    if (e) e->in_use = 0;
}

void matperf_cache_clear(void) {
    for (uint32_t i = 0; i < MATPERF_MAX_CACHE_ENTRIES; ++i) g_cache[i].in_use = 0;
}

/* --- Damage Tracking --- */

static int rects_touch(const matperf_rect_t *a, const matperf_rect_t *b) {
    if (a->x + a->w < b->x || b->x + b->w < a->x) return 0;
    if (a->y + a->h < b->y || b->y + b->h < a->y) return 0;
    return 1;
}

static void rect_union(matperf_rect_t *a, const matperf_rect_t *b) {
    uint32_t x0 = a->x < b->x ? a->x : b->x;
    uint32_t y0 = a->y < b->y ? a->y : b->y;
    uint32_t x1 = (a->x + a->w) > (b->x + b->w) ? (a->x + a->w) : (b->x + b->w);
    uint32_t y1 = (a->y + a->h) > (b->y + b->h) ? (a->y + a->h) : (b->y + b->h);
    a->x = x0; a->y = y0; a->w = x1 - x0; a->h = y1 - y0;
}

void matperf_damage_mark(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    if (w == 0u || h == 0u) return;
    matperf_rect_t r = { x, y, w, h };

    for (uint32_t i = 0; i < g_damage_count; ++i) {
        if (rects_touch(&g_damage[i], &r)) {
            rect_union(&g_damage[i], &r);
            return;
        }
    }
    if (g_damage_count < MATPERF_MAX_DAMAGE_REGIONS) {
        g_damage[g_damage_count++] = r;
        return;
    }
    /* Fragmentierung zu hoch - alles zu einer Region zusammenfassen. */
    matperf_rect_t merged = r;
    for (uint32_t i = 0; i < g_damage_count; ++i) rect_union(&merged, &g_damage[i]);
    g_damage_count = 1;
    g_damage[0] = merged;
}

uint32_t matperf_damage_region_count(void) { return g_damage_count; }

int matperf_damage_get(uint32_t index, matperf_rect_t *out) {
    if (index >= g_damage_count || !out) return -1;
    *out = g_damage[index];
    return 0;
}

void matperf_damage_clear(void) { g_damage_count = 0; }

/* --- Fallback --- */

void matperf_fallback_apply(uint32_t level, uint32_t type, material_properties_t *props) {
    if (!props || level == MATPERF_FALLBACK_NATIVE) return;

    if (level == MATPERF_FALLBACK_SOLID) {
        props->opacity = 255u;
        props->blur_radius = 0u;
        props->refraction = 0u;
        props->reflection = 0u;
        props->highlight = 0u;
        props->distortion = 0u;
        return;
    }

    switch (type) {
        case MATERIAL_TYPE_ACRYLIC:
            props->blur_radius /= 2u;
            if (level == MATPERF_FALLBACK_ALTERNATIVE) {
                props->blur_radius = 0u; props->opacity = 255u;
                props->refraction = 0u; props->reflection = 0u; props->distortion = 0u;
            }
            break;
        case MATERIAL_TYPE_LIQUID:
            props->refraction = 0u;
            props->distortion = 0u;
            if (level == MATPERF_FALLBACK_ALTERNATIVE) { props->blur_radius /= 2u; props->reflection /= 2u; }
            break;
        case MATERIAL_TYPE_FROSTED:
            props->blur_radius /= 2u;
            if (level == MATPERF_FALLBACK_ALTERNATIVE) { props->blur_radius = 0u; props->opacity = 255u; }
            break;
        case MATERIAL_TYPE_CRYSTAL:
            props->reflection /= 2u;
            props->highlight /= 2u;
            if (level == MATPERF_FALLBACK_ALTERNATIVE) {
                props->refraction /= 2u; props->reflection = 0u; props->highlight = 0u;
            }
            break;
        default:
            props->tint_strength = 0u;
            break;
    }
}

/* --- GPU --- */

int matperf_gpu_available(void) { return 0; }

void matperf_gpu_query_caps(matperf_gpu_caps_t *out) {
    if (!out) return;
    out->shader_support = 0u;
    out->compute_support = 0u;
    out->max_texture_size = 0u;
    out->gpu_memory_bytes = 0u;
}

/* --- Rendering-Optimierung --- */

int matperf_should_render(const material_properties_t *props) {
    if (!props) return 0;
    return props->opacity == 0u ? 0 : 1;
}

/* --- Qualitaet --- */

void matperf_quality_initialize(void) {
    g_quality_level = MATPERF_QUALITY_HIGH;
    g_slow_streak = 0;
    g_fast_streak = 0;
}

uint32_t matperf_quality_level(void) { return g_quality_level; }

void matperf_quality_report_frame_ticks(uint32_t ticks) {
    if (ticks >= MATPERF_QUALITY_SLOW_TICKS) {
        ++g_slow_streak;
        g_fast_streak = 0;
        if (g_slow_streak >= MATPERF_QUALITY_HYSTERESIS && g_quality_level > MATPERF_QUALITY_LOW) {
            --g_quality_level;
            g_slow_streak = 0;
        }
    } else if (ticks <= MATPERF_QUALITY_FAST_TICKS) {
        ++g_fast_streak;
        g_slow_streak = 0;
        if (g_fast_streak >= MATPERF_QUALITY_HYSTERESIS && g_quality_level < MATPERF_QUALITY_HIGH) {
            ++g_quality_level;
            g_fast_streak = 0;
        }
    } else {
        g_slow_streak = 0;
        g_fast_streak = 0;
    }
}

void matperf_quality_apply(material_properties_t *props) {
    if (!props || g_quality_level == MATPERF_QUALITY_HIGH) return;
    if (g_quality_level == MATPERF_QUALITY_MEDIUM) {
        props->blur_radius = (props->blur_radius + 1u) / 2u;
        props->reflection /= 2u;
        props->highlight = (props->highlight * 3u) / 4u;
        props->distortion /= 2u;
        return;
    }
    props->blur_radius = 0u;
    props->reflection = 0u;
    props->highlight = 0u;
    props->distortion = 0u;
}

int matperf_quality_set_instance_priority(uint32_t instance_id, int high_priority) {
    if (!material_instance_exists(instance_id)) return -1;
    for (uint32_t i = 0; i < MATPERF_MAX_PRIORITY_INSTANCES; ++i) {
        if (g_priority[i].in_use && g_priority[i].instance_id == instance_id) {
            g_priority[i].high_priority = high_priority;
            return 0;
        }
    }
    for (uint32_t i = 0; i < MATPERF_MAX_PRIORITY_INSTANCES; ++i) {
        if (!g_priority[i].in_use) {
            g_priority[i].in_use = 1;
            g_priority[i].instance_id = instance_id;
            g_priority[i].high_priority = high_priority;
            return 0;
        }
    }
    return -1;
}

uint32_t matperf_quality_level_for_instance(uint32_t instance_id) {
    for (uint32_t i = 0; i < MATPERF_MAX_PRIORITY_INSTANCES; ++i) {
        if (g_priority[i].in_use && g_priority[i].instance_id == instance_id && g_priority[i].high_priority) {
            return MATPERF_QUALITY_HIGH;
        }
    }
    return g_quality_level;
}

int matperf_self_test(void) {
    material_initialize();
    matperf_initialize();

    material_properties_t p;
    p.color_rgb = 0x123456u; p.tint_rgb = 0; p.tint_strength = 100u; p.opacity = 200u;
    p.blur_radius = 8u; p.refraction = 100u; p.reflection = 100u; p.highlight = 100u;
    p.distortion = 100u; p.roughness = 50u; p.ext_present_mask = 0;
    for (uint32_t i = 0; i < MATERIAL_EXT_COUNT; ++i) p.ext[i] = 0;
    uint32_t def = material_register("perf-test", MATERIAL_TYPE_ACRYLIC, &p);
    uint32_t inst = material_instance_create(def);

    /* Cache: erster Aufruf ist ein Miss, zweiter (ohne Aenderung) ein Hit,
       nach einer Aenderung wieder ein Miss. */
    material_properties_t resolved;
    int was_cached = 1;
    if (matperf_cache_resolve(inst, &resolved, &was_cached) != 0) return -1;
    if (was_cached) return -1;
    if (matperf_cache_resolve(inst, &resolved, &was_cached) != 0) return -1;
    if (!was_cached) return -1;
    material_instance_set_property_u32(inst, MATERIAL_FIELD_OPACITY, 50u);
    if (matperf_cache_resolve(inst, &resolved, &was_cached) != 0) return -1;
    if (was_cached) return -1;
    if (resolved.opacity != 50u) return -1;
    matperf_cache_invalidate(inst);
    if (matperf_cache_resolve(inst, &resolved, &was_cached) != 0) return -1;
    if (was_cached) return -1; /* nach explizitem Invalidate erneut ein Miss */

    /* Damage: ueberlappende Regionen werden gemerged, leere ignoriert. */
    matperf_damage_clear();
    matperf_damage_mark(0u, 0u, 0u, 0u); /* ignoriert */
    if (matperf_damage_region_count() != 0u) return -1;
    matperf_damage_mark(0u, 0u, 10u, 10u);
    matperf_damage_mark(5u, 5u, 10u, 10u); /* ueberlappt -> merge */
    if (matperf_damage_region_count() != 1u) return -1;
    matperf_rect_t rect;
    if (matperf_damage_get(0u, &rect) != 0) return -1;
    if (rect.x != 0u || rect.y != 0u || rect.w != 15u || rect.h != 15u) return -1;
    matperf_damage_mark(100u, 100u, 5u, 5u); /* nicht ueberlappend -> neue Region */
    if (matperf_damage_region_count() != 2u) return -1;
    /* Tabellenueberlauf: alle Regionen zu einer zusammenfassen. Es sind
       bereits 2 Regionen vorhanden; MATPERF_MAX_DAMAGE_REGIONS-2 weitere,
       nicht ueberlappende Regionen fuellen die Tabelle exakt bis zur
       Kapazitaet, der darauffolgende (MATPERF_MAX_DAMAGE_REGIONS-1.) Aufruf
       loest den Ueberlauf-Merge aus. */
    for (uint32_t i = 0; i < MATPERF_MAX_DAMAGE_REGIONS - 1u; ++i) {
        matperf_damage_mark(1000u + i * 20u, 200u, 1u, 1u);
    }
    if (matperf_damage_region_count() != 1u) return -1;

    /* Fallback: Solid-Stufe erzwingt volle Deckkraft/keine Effekte. */
    material_properties_t fb = p;
    matperf_fallback_apply(MATPERF_FALLBACK_SOLID, MATERIAL_TYPE_ACRYLIC, &fb);
    if (fb.opacity != 255u || fb.blur_radius != 0u) return -1;
    fb = p;
    matperf_fallback_apply(MATPERF_FALLBACK_SIMPLIFIED, MATERIAL_TYPE_ACRYLIC, &fb);
    if (fb.blur_radius != p.blur_radius / 2u) return -1;
    matperf_fallback_apply(MATPERF_FALLBACK_NATIVE, MATERIAL_TYPE_ACRYLIC, &fb); /* darf nichts aendern */
    if (fb.blur_radius != p.blur_radius / 2u) return -1;

    /* GPU: in diesem Kernel immer nicht verfuegbar. */
    if (matperf_gpu_available() != 0) return -1;
    matperf_gpu_caps_t caps;
    matperf_gpu_query_caps(&caps);
    if (caps.shader_support != 0u || caps.max_texture_size != 0u) return -1;

    /* Sichtbarkeit. */
    material_properties_t invisible = p;
    invisible.opacity = 0u;
    if (matperf_should_render(&invisible) != 0) return -1;
    if (matperf_should_render(&p) != 1) return -1;

    /* Qualitaet: Hysterese verhindert sofortigen Wechsel bei einzelnen
       Lastspitzen, greift aber nach genug aufeinanderfolgenden Berichten. */
    matperf_quality_initialize();
    if (matperf_quality_level() != MATPERF_QUALITY_HIGH) return -1;
    matperf_quality_report_frame_ticks(10u); /* 1x langsam - noch kein Wechsel */
    if (matperf_quality_level() != MATPERF_QUALITY_HIGH) return -1;
    matperf_quality_report_frame_ticks(10u);
    matperf_quality_report_frame_ticks(10u); /* 3x in Folge -> Wechsel auf MEDIUM */
    if (matperf_quality_level() != MATPERF_QUALITY_MEDIUM) return -1;
    matperf_quality_report_frame_ticks(0u);
    matperf_quality_report_frame_ticks(0u);
    matperf_quality_report_frame_ticks(0u); /* 3x schnell -> zurueck auf HIGH */
    if (matperf_quality_level() != MATPERF_QUALITY_HIGH) return -1;

    material_properties_t q = p;
    /* kuenstlich auf MEDIUM herunterfahren (direkt ueber mehrere Slow-Reports) */
    matperf_quality_report_frame_ticks(10u);
    matperf_quality_report_frame_ticks(10u);
    matperf_quality_report_frame_ticks(10u);
    if (matperf_quality_level() != MATPERF_QUALITY_MEDIUM) return -1;
    matperf_quality_apply(&q);
    if (q.blur_radius == p.blur_radius) return -1; /* reduziert */

    /* Hochprioritaere Instanz bleibt trotz globaler Qualitaetsabsenkung HIGH. */
    if (matperf_quality_set_instance_priority(inst, 1) != 0) return -1;
    if (matperf_quality_level_for_instance(inst) != MATPERF_QUALITY_HIGH) return -1;
    uint32_t other_inst = material_instance_create(def);
    if (matperf_quality_level_for_instance(other_inst) != MATPERF_QUALITY_MEDIUM) return -1;

    return 0;
}
