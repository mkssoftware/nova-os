/* NovaOS C-Kernel - Nova Material Engine: Dynamik und Interaktion (siehe
   matanim.h fuer Scope). */
#include "matanim.h"

#define MATANIM_MAX_ANIMATIONS   16u
#define MATANIM_MAX_TRANSITIONS  8u
#define MATANIM_MAX_DEFORMATIONS 16u

typedef struct {
    uint32_t in_use;
    uint32_t instance_id;
    uint32_t field;
    uint32_t from_value, to_value;
    uint32_t duration_ticks, elapsed_ticks, last_tick;
    uint32_t flags;
    uint32_t state;
    uint32_t direction; /* 0 = vorwaerts (from->to), 1 = rueckwaerts */
} matanim_entry_t;

typedef struct {
    uint32_t in_use;
    uint32_t instance_id;
    uint32_t to_definition_id;
    material_properties_t from_snapshot;
    uint32_t duration_ticks, elapsed_ticks, last_tick;
} matanim_transition_entry_t;

typedef struct {
    uint32_t in_use;
    uint32_t instance_id;
    uint32_t amplitude;
    uint32_t effective_duration_ticks, elapsed_ticks, last_tick;
} matanim_deform_entry_t;

static matanim_entry_t g_anim[MATANIM_MAX_ANIMATIONS];
static matanim_transition_entry_t g_trans[MATANIM_MAX_TRANSITIONS];
static matanim_deform_entry_t g_deform[MATANIM_MAX_DEFORMATIONS];

void matanim_initialize(void) {
    for (uint32_t i = 0; i < MATANIM_MAX_ANIMATIONS; ++i) g_anim[i].in_use = 0;
    for (uint32_t i = 0; i < MATANIM_MAX_TRANSITIONS; ++i) g_trans[i].in_use = 0;
    for (uint32_t i = 0; i < MATANIM_MAX_DEFORMATIONS; ++i) g_deform[i].in_use = 0;
}

static uint32_t lerp_u32(uint32_t a, uint32_t b, uint32_t progress_0_255) {
    int32_t delta = (int32_t)b - (int32_t)a;
    return (uint32_t)((int32_t)a + (delta * (int32_t)progress_0_255) / 255);
}

/* --- Animation --- */

uint32_t matanim_start(uint32_t instance_id, uint32_t field,
                        uint32_t from_value, uint32_t to_value,
                        uint32_t duration_ticks, uint32_t flags, uint32_t now_tick) {
    if (!material_instance_exists(instance_id)) return 0;
    for (uint32_t i = 0; i < MATANIM_MAX_ANIMATIONS; ++i) {
        if (!g_anim[i].in_use || g_anim[i].state == MATANIM_STATE_COMPLETED) {
            g_anim[i].in_use = 1;
            g_anim[i].instance_id = instance_id;
            g_anim[i].field = field;
            g_anim[i].from_value = from_value;
            g_anim[i].to_value = to_value;
            g_anim[i].duration_ticks = duration_ticks ? duration_ticks : 1u;
            g_anim[i].elapsed_ticks = 0;
            g_anim[i].last_tick = now_tick;
            g_anim[i].flags = flags;
            g_anim[i].state = MATANIM_STATE_RUNNING;
            g_anim[i].direction = 0;
            material_instance_set_property_u32(instance_id, field, from_value);
            return i + 1u;
        }
    }
    return 0;
}

static matanim_entry_t *anim_at(uint32_t anim_id) {
    if (anim_id == 0 || anim_id > MATANIM_MAX_ANIMATIONS) return 0;
    matanim_entry_t *e = &g_anim[anim_id - 1u];
    return e->in_use ? e : 0;
}

int matanim_pause(uint32_t anim_id) {
    matanim_entry_t *e = anim_at(anim_id);
    if (!e || e->state != MATANIM_STATE_RUNNING) return -1;
    e->state = MATANIM_STATE_PAUSED;
    return 0;
}

int matanim_resume(uint32_t anim_id, uint32_t now_tick) {
    matanim_entry_t *e = anim_at(anim_id);
    if (!e || e->state != MATANIM_STATE_PAUSED) return -1;
    e->state = MATANIM_STATE_RUNNING;
    e->last_tick = now_tick;
    return 0;
}

int matanim_stop(uint32_t anim_id) {
    matanim_entry_t *e = anim_at(anim_id);
    if (!e) return -1;
    e->state = MATANIM_STATE_COMPLETED;
    return 0;
}

uint32_t matanim_state(uint32_t anim_id) {
    matanim_entry_t *e = anim_at(anim_id);
    return e ? e->state : 0xFFFFFFFFu;
}

void matanim_tick(uint32_t now_tick) {
    for (uint32_t i = 0; i < MATANIM_MAX_ANIMATIONS; ++i) {
        matanim_entry_t *e = &g_anim[i];
        if (!e->in_use || e->state != MATANIM_STATE_RUNNING) continue;

        uint32_t delta = now_tick - e->last_tick;
        e->last_tick = now_tick;
        e->elapsed_ticks += delta;

        int done = 0;
        if (e->elapsed_ticks >= e->duration_ticks) {
            e->elapsed_ticks = e->duration_ticks;
            done = 1;
        }
        uint32_t progress = (e->elapsed_ticks * 255u) / e->duration_ticks;
        uint32_t a = e->direction ? e->to_value : e->from_value;
        uint32_t b = e->direction ? e->from_value : e->to_value;
        material_instance_set_property_u32(e->instance_id, e->field, lerp_u32(a, b, progress));

        if (done) {
            if (e->flags & MATANIM_FLAG_REPEAT) {
                if (e->flags & MATANIM_FLAG_REVERSE) e->direction ^= 1u;
                e->elapsed_ticks = 0;
            } else {
                e->state = MATANIM_STATE_COMPLETED;
            }
        }
    }
}

/* --- Transition --- */

static matanim_transition_entry_t *find_transition_for_instance(uint32_t instance_id) {
    for (uint32_t i = 0; i < MATANIM_MAX_TRANSITIONS; ++i)
        if (g_trans[i].in_use && g_trans[i].instance_id == instance_id) return &g_trans[i];
    return 0;
}

uint32_t matanim_transition_start(uint32_t instance_id, uint32_t to_definition_id,
                                    uint32_t duration_ticks, uint32_t now_tick) {
    if (!material_instance_exists(instance_id) || !material_definition_exists(to_definition_id)) return 0;

    material_properties_t snapshot;
    if (material_instance_resolve(instance_id, &snapshot) != 0) return 0;

    matanim_transition_entry_t *slot = find_transition_for_instance(instance_id);
    if (!slot) {
        for (uint32_t i = 0; i < MATANIM_MAX_TRANSITIONS; ++i) {
            if (!g_trans[i].in_use) { slot = &g_trans[i]; break; }
        }
    }
    if (!slot) return 0;

    slot->in_use = 1;
    slot->instance_id = instance_id;
    slot->to_definition_id = to_definition_id;
    slot->from_snapshot = snapshot;
    slot->duration_ticks = duration_ticks ? duration_ticks : 1u;
    slot->elapsed_ticks = 0;
    slot->last_tick = now_tick;
    return (uint32_t)(slot - g_trans) + 1u;
}

int matanim_transition_stop(uint32_t transition_id) {
    if (transition_id == 0 || transition_id > MATANIM_MAX_TRANSITIONS) return -1;
    matanim_transition_entry_t *t = &g_trans[transition_id - 1u];
    if (!t->in_use) return -1;
    t->in_use = 0;
    return 0;
}

int matanim_transition_is_running(uint32_t transition_id) {
    if (transition_id == 0 || transition_id > MATANIM_MAX_TRANSITIONS) return 0;
    return g_trans[transition_id - 1u].in_use ? 1 : 0;
}

static void apply_lerp_field(uint32_t instance_id, uint32_t field,
                               uint32_t a, uint32_t b, uint32_t progress) {
    material_instance_set_property_u32(instance_id, field, lerp_u32(a, b, progress));
}

void matanim_transition_tick(uint32_t now_tick) {
    for (uint32_t i = 0; i < MATANIM_MAX_TRANSITIONS; ++i) {
        matanim_transition_entry_t *t = &g_trans[i];
        if (!t->in_use) continue;

        uint32_t delta = now_tick - t->last_tick;
        t->last_tick = now_tick;
        t->elapsed_ticks += delta;
        int done = t->elapsed_ticks >= t->duration_ticks;
        if (done) t->elapsed_ticks = t->duration_ticks;
        uint32_t progress = (t->elapsed_ticks * 255u) / t->duration_ticks;

        material_properties_t target;
        if (material_definition_properties(t->to_definition_id, &target) != 0) {
            t->in_use = 0;
            continue;
        }
        const material_properties_t *f = &t->from_snapshot;
        apply_lerp_field(t->instance_id, MATERIAL_FIELD_COLOR, f->color_rgb, target.color_rgb, progress);
        apply_lerp_field(t->instance_id, MATERIAL_FIELD_TINT_RGB, f->tint_rgb, target.tint_rgb, progress);
        apply_lerp_field(t->instance_id, MATERIAL_FIELD_TINT_STRENGTH, f->tint_strength, target.tint_strength, progress);
        apply_lerp_field(t->instance_id, MATERIAL_FIELD_OPACITY, f->opacity, target.opacity, progress);
        apply_lerp_field(t->instance_id, MATERIAL_FIELD_BLUR, f->blur_radius, target.blur_radius, progress);
        apply_lerp_field(t->instance_id, MATERIAL_FIELD_REFRACTION, f->refraction, target.refraction, progress);
        apply_lerp_field(t->instance_id, MATERIAL_FIELD_REFLECTION, f->reflection, target.reflection, progress);
        apply_lerp_field(t->instance_id, MATERIAL_FIELD_HIGHLIGHT, f->highlight, target.highlight, progress);
        apply_lerp_field(t->instance_id, MATERIAL_FIELD_DISTORTION, f->distortion, target.distortion, progress);
        apply_lerp_field(t->instance_id, MATERIAL_FIELD_ROUGHNESS, f->roughness, target.roughness, progress);

        if (done) {
            material_instance_rebind(t->instance_id, t->to_definition_id);
            material_instance_clear_overrides(t->instance_id);
            t->in_use = 0;
        }
    }
}

/* --- Interaction --- */

int matanim_handle_event(uint32_t instance_id, uint32_t event, uint32_t now_tick) {
    if (!material_instance_exists(instance_id)) return -1;
    switch (event) {
        case MATANIM_EVENT_HOVER_ENTER: material_instance_set_state(instance_id, MATERIAL_STATE_HOVER); break;
        case MATANIM_EVENT_HOVER_LEAVE: material_instance_set_state(instance_id, MATERIAL_STATE_NORMAL); break;
        case MATANIM_EVENT_PRESS_DOWN: {
            material_instance_set_state(instance_id, MATERIAL_STATE_PRESSED);
            uint32_t def = material_instance_definition(instance_id);
            if (material_definition_type(def) == MATERIAL_TYPE_LIQUID) {
                matanim_deformation_trigger(instance_id, 180u, 24u, 60u, now_tick);
            }
            break;
        }
        case MATANIM_EVENT_PRESS_UP: material_instance_set_state(instance_id, MATERIAL_STATE_HOVER); break;
        case MATANIM_EVENT_FOCUS_GAIN: material_instance_set_state(instance_id, MATERIAL_STATE_FOCUSED); break;
        case MATANIM_EVENT_FOCUS_LOSE: material_instance_set_state(instance_id, MATERIAL_STATE_NORMAL); break;
        case MATANIM_EVENT_DISABLE: material_instance_set_state(instance_id, MATERIAL_STATE_DISABLED); break;
        case MATANIM_EVENT_ENABLE: material_instance_set_state(instance_id, MATERIAL_STATE_NORMAL); break;
        default: return -1;
    }
    return 0;
}

/* --- Deformation --- */

uint32_t matanim_deformation_trigger(uint32_t instance_id, uint32_t amplitude,
                                       uint32_t duration_ticks, uint32_t damping,
                                       uint32_t now_tick) {
    if (!material_instance_exists(instance_id)) return 0;
    if (damping > 255u) damping = 255u;
    uint32_t effective_duration = (duration_ticks * (255u - damping)) / 255u;
    if (effective_duration == 0u) effective_duration = 1u;

    for (uint32_t i = 0; i < MATANIM_MAX_DEFORMATIONS; ++i) {
        if (!g_deform[i].in_use) {
            g_deform[i].in_use = 1;
            g_deform[i].instance_id = instance_id;
            g_deform[i].amplitude = amplitude > 255u ? 255u : amplitude;
            g_deform[i].effective_duration_ticks = effective_duration;
            g_deform[i].elapsed_ticks = 0;
            g_deform[i].last_tick = now_tick;
            return i + 1u;
        }
    }
    return 0;
}

void matanim_deformation_tick(uint32_t now_tick) {
    for (uint32_t i = 0; i < MATANIM_MAX_DEFORMATIONS; ++i) {
        matanim_deform_entry_t *d = &g_deform[i];
        if (!d->in_use) continue;
        uint32_t delta = now_tick - d->last_tick;
        d->last_tick = now_tick;
        d->elapsed_ticks += delta;
        if (d->elapsed_ticks >= d->effective_duration_ticks) d->in_use = 0;
    }
}

uint32_t matanim_deformation_bias(uint32_t instance_id) {
    uint32_t total = 0;
    for (uint32_t i = 0; i < MATANIM_MAX_DEFORMATIONS; ++i) {
        matanim_deform_entry_t *d = &g_deform[i];
        if (!d->in_use || d->instance_id != instance_id) continue;
        uint32_t remaining = d->amplitude - (d->amplitude * d->elapsed_ticks) / d->effective_duration_ticks;
        total += remaining;
    }
    return total > 255u ? 255u : total;
}

int matanim_self_test(void) {
    material_initialize();
    matanim_initialize();

    material_properties_t defaults;
    defaults.color_rgb = 0; defaults.tint_rgb = 0; defaults.tint_strength = 0;
    defaults.opacity = 100u; defaults.blur_radius = 0; defaults.refraction = 0;
    defaults.reflection = 0; defaults.highlight = 0; defaults.distortion = 0;
    defaults.roughness = 0; defaults.ext_present_mask = 0;
    for (uint32_t i = 0; i < MATERIAL_EXT_COUNT; ++i) defaults.ext[i] = 0;

    uint32_t def_a = material_register("anim-a", MATERIAL_TYPE_CUSTOM, &defaults);
    defaults.opacity = 250u;
    uint32_t def_b = material_register("anim-b", MATERIAL_TYPE_CUSTOM, &defaults);
    uint32_t inst = material_instance_create(def_a);

    /* Animation: linear von 0 nach 100 ueber 10 Ticks. */
    uint32_t anim = matanim_start(inst, MATERIAL_FIELD_OPACITY, 0u, 100u, 10u, 0u, 0u);
    if (anim == 0) return -1;
    matanim_tick(5u); /* now_tick=5 -> 5 Ticks seit Start */
    material_properties_t resolved;
    material_instance_resolve(inst, &resolved);
    if (resolved.opacity < 40u || resolved.opacity > 60u) return -1; /* ~50% */

    matanim_tick(10u);
    material_instance_resolve(inst, &resolved);
    if (resolved.opacity != 100u) return -1;
    if (matanim_state(anim) != MATANIM_STATE_COMPLETED) return -1;

    /* Pause/Resume. */
    uint32_t anim2 = matanim_start(inst, MATERIAL_FIELD_HIGHLIGHT, 0u, 200u, 20u, 0u, 0u);
    matanim_tick(10u); /* 50% */
    if (matanim_pause(anim2) != 0) return -1;
    matanim_tick(15u); /* darf sich waehrend Pause nicht weiterbewegen */
    material_instance_resolve(inst, &resolved);
    uint32_t paused_value = resolved.highlight;
    matanim_tick(16u);
    material_instance_resolve(inst, &resolved);
    if (resolved.highlight != paused_value) return -1;
    if (matanim_resume(anim2, 16u) != 0) return -1;
    matanim_tick(26u); /* weitere 10 Ticks -> fertig */
    material_instance_resolve(inst, &resolved);
    if (resolved.highlight != 200u) return -1;

    /* Transition: Instanz wechselt vollstaendig zu def_b. */
    uint32_t trans = matanim_transition_start(inst, def_b, 10u, 0u);
    if (trans == 0) return -1;
    matanim_transition_tick(5u);
    material_instance_resolve(inst, &resolved);
    if (resolved.opacity == 100u || resolved.opacity == 250u) return -1; /* dazwischen */
    matanim_transition_tick(10u);
    if (matanim_transition_is_running(trans)) return -1;
    if (material_instance_definition(inst) != def_b) return -1;
    material_instance_resolve(inst, &resolved);
    if (resolved.opacity != 250u) return -1;

    /* Interaction + Deformation (nur fuer LIQUID-Typ). */
    uint32_t def_liquid = material_register("anim-liquid", MATERIAL_TYPE_LIQUID, &defaults);
    uint32_t liquid_inst = material_instance_create(def_liquid);
    if (matanim_handle_event(liquid_inst, MATANIM_EVENT_PRESS_DOWN, 0u) != 0) return -1;
    if (material_instance_state(liquid_inst) != MATERIAL_STATE_PRESSED) return -1;
    uint32_t bias_start = matanim_deformation_bias(liquid_inst);
    if (bias_start == 0u) return -1;
    matanim_deformation_tick(100u); /* weit ueber die Impulsdauer hinaus */
    if (matanim_deformation_bias(liquid_inst) != 0u) return -1;

    /* Solid-Instanz loest keine Deformation aus. */
    uint32_t def_solid_like = material_register("anim-solid", MATERIAL_TYPE_SOLID, &defaults);
    uint32_t solid_inst = material_instance_create(def_solid_like);
    matanim_handle_event(solid_inst, MATANIM_EVENT_PRESS_DOWN, 0u);
    if (matanim_deformation_bias(solid_inst) != 0u) return -1;

    return 0;
}
