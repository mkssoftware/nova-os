#include "motion.h"
#include "design.h"

#define FIXED_ONE 65536
#define QUALITY_SAFE 4u

static nova_animation_t pool[NOVA_MOTION_CAPACITY];
static bool used[NOVA_MOTION_CAPACITY];
static nova_motion_diagnostics_t diagnostics;
static nova_motion_budget_t budget;
static bool scheduler_updating;
static void budget_refresh_policy(void)
{
    budget.glow_enabled=budget.fallback_step<1&&!diagnostics.reduced_motion;
    budget.shadow_enabled=budget.fallback_step<2&&!diagnostics.reduced_motion;
    budget.blur_enabled=budget.fallback_step<3&&!diagnostics.reduced_motion;
    budget.spring_enabled=budget.fallback_step<4&&!diagnostics.reduced_motion;
    budget.material_enabled=budget.fallback_step<5&&!diagnostics.reduced_motion;
    budget.decorative_enabled=budget.fallback_step<6&&!diagnostics.reduced_motion;
    budget.safe_mode=budget.fallback_step>=6;
}

static nova_easing_t design_easing(nova_design_easing_token_t easing)
{
    if(easing==NOVA_DESIGN_EASE_LINEAR)return NOVA_EASE_LINEAR;
    if(easing==NOVA_DESIGN_EASE_OUT||easing==NOVA_DESIGN_EASE_DECELERATE)
        return NOVA_EASE_OUT_CUBIC;
    if(easing==NOVA_DESIGN_EASE_SPRING||easing==NOVA_DESIGN_EASE_OVERSHOOT)
        return NOVA_EASE_SPRING;
    return NOVA_EASE_IN_OUT;
}
static nova_animation_t *dialog_fade_animation;
static nova_animation_t *dialog_scale_animation;

static int32_t clamp_fixed(int64_t value)
{
    if (value < 0) return 0;
    if (value > FIXED_ONE) return FIXED_ONE;
    return (int32_t)value;
}

int32_t nova_ease_apply(nova_easing_t easing, int32_t t)
{
    int64_t x = clamp_fixed(t);
    if (easing == NOVA_EASE_LINEAR) return (int32_t)x;
    if (easing == NOVA_EASE_OUT_CUBIC) {
        int64_t inverse = FIXED_ONE - x;
        int64_t cube = (inverse * inverse / FIXED_ONE) * inverse / FIXED_ONE;
        return clamp_fixed(FIXED_ONE - cube);
    }
    if (easing == NOVA_EASE_IN_OUT) {
        if (x < FIXED_ONE / 2)
            return clamp_fixed((4 * x * x / FIXED_ONE) * x / FIXED_ONE);
        int64_t inverse = (2 * FIXED_ONE) - (2 * x);
        return clamp_fixed(FIXED_ONE - ((inverse * inverse / FIXED_ONE) *
                           inverse / FIXED_ONE) / 2);
    }
    /* Bounded critically damped approximation; no recursive solver. */
    int64_t inverse = FIXED_ONE - x;
    int64_t damped = inverse * (FIXED_ONE + (3 * x)) / FIXED_ONE;
    return clamp_fixed(FIXED_ONE - damped);
}

void nova_motion_initialize(void)
{
    for (uint32_t i = 0; i < NOVA_MOTION_CAPACITY; ++i) used[i] = false;
    diagnostics = (nova_motion_diagnostics_t){0};
    (void)nova_motion_budget_initialize();scheduler_updating=false;
    dialog_fade_animation=dialog_scale_animation=0;
}

static bool valid(const nova_animation_t *a)
{
    return a && a->target && a->duration_ms && a->property < NOVA_PROPERTY_COUNT &&
           a->easing <= NOVA_EASE_SPRING && a->priority <= 3;
}

nova_animation_t *nova_motion_create(const nova_animation_t *description)
{
    if (!valid(description)) { ++diagnostics.rejected; return 0; }
    nova_animation_type_t allocation_type=
        (description->property==NOVA_PROPERTY_BLUR||description->property==NOVA_PROPERTY_GLOW||
         description->property==NOVA_PROPERTY_SHADOW)?NOVA_ANIMATION_MATERIAL:
        description->property==NOVA_PROPERTY_WIDTH?NOVA_ANIMATION_PROGRESS:
        description->easing==NOVA_EASE_SPRING?NOVA_ANIMATION_SPRING:NOVA_ANIMATION_GENERAL;
    if(!diagnostics.reduced_motion&&!nova_motion_budget_can_allocate(allocation_type)){
        ++diagnostics.rejected;return 0;}
    for (uint32_t i = 0; i < NOVA_MOTION_CAPACITY; ++i) {
        if (used[i]) {
            if (pool[i].target == description->target &&
                pool[i].property == description->property &&
                pool[i].state != NOVA_MOTION_COMPLETED &&
                pool[i].state != NOVA_MOTION_CANCELLED) {
                ++diagnostics.rejected;
                return 0;
            }
            continue;
        }
        used[i] = true;
        pool[i] = *description;
        pool[i].state = description->delay_ms ? NOVA_MOTION_WAITING : NOVA_MOTION_CREATED;
        if (diagnostics.reduced_motion && pool[i].easing == NOVA_EASE_SPRING)
            pool[i].easing = NOVA_EASE_OUT_CUBIC;
        if (diagnostics.quality == QUALITY_SAFE) pool[i].duration_ms = 1;
        pool[i].remaining_ms=pool[i].duration_ms;
        pool[i].current_velocity_16_16=(int32_t)(((int64_t)(pool[i].to-pool[i].from)*
            FIXED_ONE)/(int32_t)pool[i].duration_ms);
        ++diagnostics.active;
        if(diagnostics.reduced_motion&&!nova_motion_apply_policy(&pool[i])){
            used[i]=false;if(diagnostics.active)--diagnostics.active;return 0;}
        return &pool[i];
    }
    ++diagnostics.rejected;
    return 0;
}

static void finish(nova_animation_t *a, nova_motion_state_t state)
{
    a->state = state;
    if (diagnostics.active) --diagnostics.active;
    if (state == NOVA_MOTION_COMPLETED) ++diagnostics.completed;
    else ++diagnostics.cancelled;
}

void nova_motion_update(uint64_t now)
{
    if(scheduler_updating){++diagnostics.rejected;return;}
    scheduler_updating=true;
    if (now < diagnostics.current_ms) now = diagnostics.current_ms;
    diagnostics.previous_ms = diagnostics.current_ms;
    diagnostics.current_ms = now;
    if (diagnostics.paused){scheduler_updating=false;return;}
    for (uint32_t i = 0; i < NOVA_MOTION_CAPACITY; ++i) {
        if (!used[i]) continue;
        nova_animation_t *a = &pool[i];
        if (a->state == NOVA_MOTION_COMPLETED || a->state == NOVA_MOTION_CANCELLED ||
            a->state == NOVA_MOTION_PAUSED||a->state==NOVA_MOTION_INTERRUPTED) continue;
        uint64_t begin = a->start_ms + a->delay_ms;
        if (now < begin) { a->state = NOVA_MOTION_WAITING; continue; }
        a->state = NOVA_MOTION_RUNNING;
        uint64_t elapsed = now - begin;
        uint64_t cycle = elapsed / a->duration_ms;
        uint64_t within = elapsed % a->duration_ms;
        if (cycle > a->repeats) {
            *a->target = (a->auto_reverse && (a->repeats & 1u)) ? a->from : a->to;
            finish(a, NOVA_MOTION_COMPLETED);
            continue;
        }
        bool reverse = a->reversed ^ (a->auto_reverse && (cycle & 1u));
        int32_t progress = (int32_t)((within * FIXED_ONE) / a->duration_ms);
        if (reverse) progress = FIXED_ONE - progress;
        int32_t eased = nova_ease_apply(a->easing, progress);
        int32_t previous=*a->target;
        *a->target = a->from + (int32_t)(((int64_t)(a->to - a->from) * eased) / FIXED_ONE);
        uint64_t frame_ms=now>diagnostics.previous_ms?now-diagnostics.previous_ms:1;
        a->current_velocity_16_16=(int32_t)(((int64_t)(*a->target-previous)*FIXED_ONE)/
                                            (int64_t)frame_ms);
        a->remaining_ms=(uint32_t)(a->duration_ms-within);
    }
    scheduler_updating=false;
}

void nova_motion_cancel(nova_animation_t *a)
{
    if (a && a->state != NOVA_MOTION_COMPLETED && a->state != NOVA_MOTION_CANCELLED)
        finish(a, NOVA_MOTION_CANCELLED);
}

bool nova_motion_interrupt(nova_animation_t *a)
{
    if (!a || !a->interruptible || a->state != NOVA_MOTION_RUNNING) return false;
    a->state = NOVA_MOTION_INTERRUPTED;a->paused_at_ms=diagnostics.current_ms;
    ++diagnostics.interruptions;++diagnostics.pauses;
    return true;
}

bool nova_motion_resume(nova_animation_t *a, uint64_t now)
{
    if (!a || (a->state != NOVA_MOTION_PAUSED&&a->state!=NOVA_MOTION_INTERRUPTED)) return false;
    a->from = *a->target;
    a->start_ms = now;
    a->delay_ms = 0;
    if(a->remaining_ms)a->duration_ms=a->remaining_ms;
    a->state = NOVA_MOTION_RESUMING;++diagnostics.resumes;
    a->state = NOVA_MOTION_RUNNING;
    return true;
}

bool nova_motion_reverse(nova_animation_t *a, uint64_t now)
{
    if (!a || !a->interruptible) return false;
    int32_t origin=a->from;a->from=*a->target;a->to=origin;
    a->reversed = !a->reversed;
    a->start_ms = now;
    a->delay_ms=0;if(a->remaining_ms)a->duration_ms=a->remaining_ms;
    a->current_velocity_16_16=-a->current_velocity_16_16;
    a->state = NOVA_MOTION_RUNNING;
    ++diagnostics.reversals;
    return true;
}

bool nova_motion_redirect(nova_animation_t *a, int32_t target, uint64_t now)
{
    if (!a || !a->interruptible) return false;
    a->from = *a->target;
    a->to = target;
    a->start_ms = now;
    a->delay_ms = 0;
    a->state = NOVA_MOTION_RUNNING;
    int64_t speed=a->current_velocity_16_16;
    if(speed<0)speed=-speed;
    int64_t distance=(int64_t)target-a->from;if(distance<0)distance=-distance;
    if(speed>0&&distance>0){uint64_t duration=(uint64_t)(distance*FIXED_ONE/speed);
        if(duration<1)duration=1;
        if(duration>60000)duration=60000;
        a->duration_ms=(uint32_t)duration;}
    a->remaining_ms=a->duration_ms;++diagnostics.redirects;
    return true;
}

void nova_motion_pause_group(uint16_t group)
{
    for (uint32_t i = 0; i < NOVA_MOTION_CAPACITY; ++i)
        if (used[i] && pool[i].group == group && pool[i].state == NOVA_MOTION_RUNNING)
            {pool[i].state = NOVA_MOTION_PAUSED;pool[i].paused_at_ms=diagnostics.current_ms;
             ++diagnostics.pauses;}
}

nova_motion_policy_t nova_motion_policy(nova_property_t property)
{
    if(property>=NOVA_PROPERTY_COUNT)return NOVA_MOTION_POLICY_INVALID;
    if(property==NOVA_PROPERTY_OPACITY||property==NOVA_PROPERTY_TINT||
       property==NOVA_PROPERTY_BORDER)return NOVA_MOTION_POLICY_ALLOWED;
    if(property==NOVA_PROPERTY_BLUR||property==NOVA_PROPERTY_GLOW||
       property==NOVA_PROPERTY_SHADOW)return NOVA_MOTION_POLICY_DISABLED;
    return NOVA_MOTION_POLICY_REPLACED;
}
bool nova_motion_apply_policy(nova_animation_t *a)
{
    if(!a||!valid(a)){++diagnostics.policy_errors;return false;}
    if(!diagnostics.reduced_motion)return true;
    nova_motion_policy_t policy=nova_motion_policy(a->property);
    if(policy==NOVA_MOTION_POLICY_INVALID){++diagnostics.policy_errors;return false;}
    if(a->easing==NOVA_EASE_SPRING)a->easing=NOVA_EASE_OUT_CUBIC;
    if(policy==NOVA_MOTION_POLICY_ALLOWED){
        uint32_t cap=a->property==NOVA_PROPERTY_BORDER?100u:150u;
        if(a->duration_ms>cap)a->duration_ms=cap;
        a->remaining_ms=a->duration_ms;return true;}
    *a->target=a->to;
    if(policy==NOVA_MOTION_POLICY_DISABLED)++diagnostics.policy_disables;
    else ++diagnostics.policy_replacements;
    if(a->state!=NOVA_MOTION_COMPLETED&&a->state!=NOVA_MOTION_CANCELLED)
        finish(a,NOVA_MOTION_COMPLETED);
    return true;
}
bool nova_motion_reload_policy(void)
{
    ++diagnostics.policy_reloads;
    if(!diagnostics.reduced_motion)return true;
    for(uint32_t i=0;i<NOVA_MOTION_CAPACITY;++i)
        if(used[i]&&pool[i].state!=NOVA_MOTION_COMPLETED&&
           pool[i].state!=NOVA_MOTION_CANCELLED&&!nova_motion_apply_policy(&pool[i]))return false;
    return true;
}
bool nova_motion_set_reduced(bool enabled)
{
    if(diagnostics.reduced_motion==enabled)return true;
    diagnostics.reduced_motion=enabled;budget_refresh_policy();
    return nova_motion_reload_policy();
}
bool nova_motion_is_reduced(void) { return diagnostics.reduced_motion; }
const nova_motion_diagnostics_t *nova_motion_diagnostics(void) { return &diagnostics; }

bool nova_transition_begin(nova_transition_t *transition, int32_t from, int32_t to,
                           uint32_t duration_ms, nova_easing_t easing)
{
    if (!transition || !duration_ms || from == to || easing > NOVA_EASE_SPRING)
        return false;
    transition->value = from;
    nova_animation_t description = {
        &transition->value, from, to, diagnostics.current_ms, 0, duration_ms,
        (uint16_t)transition->id, 2, 0, NOVA_PROPERTY_OPACITY, easing,
        NOVA_MOTION_CREATED, false, false, true, 0, 0, 0
    };
    transition->animation = nova_motion_create(&description);
    return transition->animation != 0;
}

void nova_transition_cancel(nova_transition_t *transition)
{
    if (transition) nova_motion_cancel(transition->animation);
}

bool nova_dialog_enter(nova_dialog_motion_t *dialog)
{
    if (!dialog || dialog->visible) return false;
    dialog->visible = true; dialog->focused = false; dialog->opacity = 0;
    dialog->scale = diagnostics.reduced_motion ? 1000 : 950;
    const nova_motion_style_t *style=nova_design_motion_get(NOVA_TRANSITION_DIALOG);
    if(!style)return false;
    nova_animation_t fade = { &dialog->opacity, 0, 255, diagnostics.current_ms, 0,
        style->duration_ms, 20, style->priority, 0, NOVA_PROPERTY_OPACITY,
        design_easing(style->easing), NOVA_MOTION_CREATED, false, false, true,0,0,0 };
    dialog_fade_animation=nova_motion_create(&fade);
    dialog_scale_animation=0;
    if(!diagnostics.reduced_motion){
        nova_animation_t scale={&dialog->scale,950,1000,diagnostics.current_ms,0,
            style->duration_ms,20,style->priority,0,NOVA_PROPERTY_SCALE,
            design_easing(style->easing),NOVA_MOTION_CREATED,
            false,false,true,0,0,0};
        dialog_scale_animation=nova_motion_create(&scale);
    }
    if(!dialog_fade_animation||(!diagnostics.reduced_motion&&!dialog_scale_animation)){
        if(dialog_fade_animation)nova_motion_cancel(dialog_fade_animation);
        if(dialog_scale_animation)nova_motion_cancel(dialog_scale_animation);
        dialog->opacity=255;dialog->scale=1000;dialog->focused=true;
        return false;
    }
    return true;
}

bool nova_dialog_exit(nova_dialog_motion_t *dialog)
{
    if (!dialog || !dialog->visible) return false;
    const nova_motion_style_t *style=nova_design_motion_get(NOVA_TRANSITION_DIALOG);
    if(!style)return false;
    nova_animation_t fade = { &dialog->opacity, dialog->opacity, 0,
        diagnostics.current_ms, 0, style->duration_ms,
        20, style->priority, 0, NOVA_PROPERTY_OPACITY, design_easing(style->easing),
        NOVA_MOTION_CREATED, false, false, true,0,0,0 };
    dialog->focused = false;
    dialog_fade_animation=nova_motion_create(&fade);
    dialog_scale_animation=0;
    if(!diagnostics.reduced_motion){
        nova_animation_t scale={&dialog->scale,dialog->scale,950,diagnostics.current_ms,0,
            style->duration_ms,20,style->priority,0,NOVA_PROPERTY_SCALE,
            design_easing(style->easing),
            NOVA_MOTION_CREATED,false,false,true,0,0,0};
        dialog_scale_animation=nova_motion_create(&scale);
    }
    if(!dialog_fade_animation||(!diagnostics.reduced_motion&&!dialog_scale_animation)){
        if(dialog_fade_animation)nova_motion_cancel(dialog_fade_animation);
        if(dialog_scale_animation)nova_motion_cancel(dialog_scale_animation);
        dialog->opacity=0;dialog->scale=diagnostics.reduced_motion?1000:950;
        return false;
    }
    return true;
}

bool nova_dialog_motion_running(void)
{
    return (dialog_fade_animation&&(dialog_fade_animation->state==NOVA_MOTION_WAITING||
            dialog_fade_animation->state==NOVA_MOTION_RUNNING))||
           (dialog_scale_animation&&(dialog_scale_animation->state==NOVA_MOTION_WAITING||
            dialog_scale_animation->state==NOVA_MOTION_RUNNING));
}

void nova_dialog_motion_cancel(nova_dialog_motion_t *dialog)
{
    if(dialog_fade_animation)nova_motion_cancel(dialog_fade_animation);
    if(dialog_scale_animation)nova_motion_cancel(dialog_scale_animation);
    dialog_fade_animation=dialog_scale_animation=0;
    if(dialog){dialog->opacity=dialog->visible?255:0;
        dialog->scale=1000;dialog->focused=dialog->visible;}
}

bool nova_navigation_begin(nova_navigation_motion_t *navigation, bool forward)
{
    if (!navigation || navigation->running) return false;
    navigation->forward = forward; navigation->running = true;
    navigation->source_x = 0;
    navigation->target_x = diagnostics.reduced_motion ? 0 : (forward ? 1000 : -1000);
    if (diagnostics.reduced_motion) return true;
    const nova_motion_style_t *style=nova_design_motion_get(NOVA_TRANSITION_PAGE);
    if(!style)return false;
    nova_animation_t slide = { &navigation->source_x, 0, forward ? -1000 : 1000,
        diagnostics.current_ms, 0, style->duration_ms, 21, style->priority, 0,
        NOVA_PROPERTY_X,design_easing(style->easing), NOVA_MOTION_CREATED, false, false, true,0,0,0 };
    return nova_motion_create(&slide) != 0;
}

bool nova_focus_set(nova_focus_motion_t *focus, bool focused, bool selected)
{
    if (!focus || (focus->focused == focused && focus->selected == selected)) return false;
    focus->focused = focused; focus->selected = selected;
    int32_t destination = focused ? 255 : 0;
    const nova_motion_style_t *style=nova_design_motion_get(NOVA_TRANSITION_FOCUS);
    if(!style)return false;
    nova_animation_t ring = { &focus->ring_opacity, focus->ring_opacity, destination,
        diagnostics.current_ms, 0, style->duration_ms,
        22, style->priority, 0, NOVA_PROPERTY_BORDER, design_easing(style->easing),
        NOVA_MOTION_CREATED, false, false, true,0,0,0 };
    focus->highlight = selected ? 255 : 0;
    return nova_motion_create(&ring) != 0;
}

bool nova_progress_set(nova_progress_motion_t *progress, int32_t value)
{
    if (!progress) return false;
    if (value < 0 || value > 1000) {
        progress->indeterminate = true; progress->failed = true; return false;
    }
    progress->requested = value; progress->indeterminate = false; progress->running = value < 1000;
    const nova_motion_style_t *style=nova_design_motion_get(NOVA_TRANSITION_SELECTION);
    if(!style)return false;
    nova_animation_t change = { &progress->displayed, progress->displayed, value,
        diagnostics.current_ms, 0, style->duration_ms, 23, style->priority, 0, NOVA_PROPERTY_WIDTH,
        NOVA_EASE_LINEAR, NOVA_MOTION_CREATED, false, false, true,0,0,0 };
    return nova_motion_create(&change) != 0;
}

void nova_progress_set_indeterminate(nova_progress_motion_t *progress, bool enabled)
{
    if (progress) { progress->indeterminate = enabled; progress->running = enabled; }
}

void nova_motion_budget_update(uint32_t frame_us, uint32_t scheduler_us)
{
    budget.frame_time_us = frame_us;
    budget.scheduler_time_us = scheduler_us;
    if (frame_us > 16667u || scheduler_us > 1000u) {
        ++budget.violations;
        nova_motion_budget_apply_fallback();
    }
    budget.active_animations=(uint16_t)diagnostics.active;
}

bool nova_motion_budget_initialize(void)
{
    budget=(nova_motion_budget_t){.memory_bytes=sizeof(pool)+sizeof(used),.quality=2,
        .glow_enabled=true,.shadow_enabled=true,.blur_enabled=true,.spring_enabled=true,
        .material_enabled=true,.decorative_enabled=true};
    budget_refresh_policy();
    return budget.memory_bytes<=512u*1024u;
}
const nova_motion_budget_t *nova_motion_budget_get(void){return &budget;}
static uint16_t active_type_count(nova_animation_type_t type)
{
    uint16_t count=0;
    for(uint16_t i=0;i<NOVA_MOTION_CAPACITY;++i)if(used[i]&&
       pool[i].state!=NOVA_MOTION_COMPLETED&&pool[i].state!=NOVA_MOTION_CANCELLED){
        bool material=pool[i].property==NOVA_PROPERTY_BLUR||pool[i].property==NOVA_PROPERTY_GLOW||
                      pool[i].property==NOVA_PROPERTY_SHADOW;
        if((type==NOVA_ANIMATION_MATERIAL&&material)||
           (type==NOVA_ANIMATION_SPRING&&pool[i].easing==NOVA_EASE_SPRING)||
           (type==NOVA_ANIMATION_PROGRESS&&pool[i].property==NOVA_PROPERTY_WIDTH)||
           (type==NOVA_ANIMATION_GENERAL&&!material))++count;}
    return count;
}
bool nova_motion_budget_can_allocate(nova_animation_type_t type)
{
    if(type>=NOVA_ANIMATION_TYPE_COUNT)return false;
    if(diagnostics.active>=NOVA_MOTION_CAPACITY)return false;
    uint16_t limit=type==NOVA_ANIMATION_SPRING?64:type==NOVA_ANIMATION_MATERIAL?64:
                   type==NOVA_ANIMATION_PROGRESS?32:NOVA_MOTION_CAPACITY;
    if(type==NOVA_ANIMATION_MATERIAL&&!budget.material_enabled)return false;
    if(type==NOVA_ANIMATION_SPRING&&!budget.spring_enabled)return false;
    return active_type_count(type)<limit;
}
void nova_motion_budget_apply_fallback(void)
{
    if(budget.fallback_step<6)++budget.fallback_step;
    budget_refresh_policy();
    if(budget.quality<QUALITY_SAFE)++budget.quality;
    diagnostics.quality=budget.quality;
}
const nova_motion_budget_t *nova_motion_budget(void) { return &budget; }
