#include "motion.h"
#include "design.h"

#define FIXED_ONE 65536
#define QUALITY_SAFE 4u

static nova_animation_t pool[NOVA_MOTION_CAPACITY];
static bool used[NOVA_MOTION_CAPACITY];
static nova_motion_diagnostics_t diagnostics;
static nova_motion_budget_t budget;
static bool scheduler_updating;
static nova_animation_trace_t animation_trace[NOVA_ANIMATION_DIAG_CAPACITY];
static uint16_t animation_trace_head,animation_trace_count;
static uint64_t animation_event_sequence,animation_frame_total;
static uint32_t animation_frames,last_cycle[NOVA_MOTION_CAPACITY];
static nova_animation_diag_status_t animation_diag;
static uint32_t animation_render_us,animation_compositor_us,animation_layout_us;
static bool animation_software_renderer;
static uint8_t animation_quality,animation_hardware_profile;

static uint32_t animation_index(const nova_animation_t *animation)
{return animation>=pool&&animation<pool+NOVA_MOTION_CAPACITY?
 (uint32_t)(animation-pool):UINT32_MAX;}
static nova_animation_type_t animation_kind(const nova_animation_t *animation)
{return (animation->property==NOVA_PROPERTY_BLUR||animation->property==NOVA_PROPERTY_GLOW||
 animation->property==NOVA_PROPERTY_SHADOW)?NOVA_ANIMATION_MATERIAL:
 animation->property==NOVA_PROPERTY_WIDTH?NOVA_ANIMATION_PROGRESS:
 animation->easing==NOVA_EASE_SPRING?NOVA_ANIMATION_SPRING:NOVA_ANIMATION_GENERAL;}
static void animation_record(nova_animation_t *animation,nova_animation_diag_event_t event,
    uint64_t timestamp_ms,bool error)
{
    uint32_t index=animation_index(animation);if(index==UINT32_MAX)return;
    uint16_t out=(uint16_t)((animation_trace_head+animation_trace_count)%NOVA_ANIMATION_DIAG_CAPACITY);
    if(animation_trace_count==NOVA_ANIMATION_DIAG_CAPACITY){out=animation_trace_head;
        animation_trace_head=(uint16_t)((animation_trace_head+1)%NOVA_ANIMATION_DIAG_CAPACITY);
        ++animation_diag.overwritten;}else ++animation_trace_count;
    uint64_t begin=animation->start_ms+animation->delay_ms;
    uint64_t planned_end=begin+(uint64_t)animation->duration_ms*(animation->repeats+1u);
    nova_animation_trace_t trace={.event_id=++animation_event_sequence,
        .timestamp_us=timestamp_ms*1000u,.start_us=begin*1000u,
        .end_us=(event==NOVA_ANIMATION_DIAG_COMPLETED||event==NOVA_ANIMATION_DIAG_CANCELLED||error)?timestamp_ms*1000u:0,
        .animation_id=index+1u,.target_id=index+1u,.motion_token=((uint32_t)animation->property<<8)|animation->easing,
        .event=event,.type=animation_kind(animation),.property=animation->property,
        .easing=animation->easing,.state=animation->state,
        .planned_duration_us=animation->duration_ms*1000u,
        .actual_duration_us=timestamp_ms>=begin?(uint32_t)((timestamp_ms-begin)*1000u):0,
        .start_latency_us=timestamp_ms>begin&&event==NOVA_ANIMATION_DIAG_STARTED?
            (uint32_t)((timestamp_ms-begin)*1000u):0,
        .completion_latency_us=timestamp_ms>planned_end&&event==NOVA_ANIMATION_DIAG_COMPLETED?
            (uint32_t)((timestamp_ms-planned_end)*1000u):0,
        .frame_time_us=animation_diag.last_frame_us,.render_time_us=animation_render_us,
        .compositor_time_us=animation_compositor_us,.layout_time_us=animation_layout_us,
        .cpu_time_us=animation_render_us+animation_compositor_us+animation_layout_us,
        .gpu_time_us=0,.rendered_frames=animation_frames,
        .group=animation->group,.repeats=animation->repeats,.priority=animation->priority,
        .quality=animation_quality,.hardware_profile=animation_hardware_profile,
        .software_renderer=animation_software_renderer,.error=error,
        .budget_exceeded=animation_diag.last_frame_us>16667u};
    animation_trace[out]=trace;++animation_diag.recorded;animation_diag.count=animation_trace_count;
    if(event==NOVA_ANIMATION_DIAG_STARTED)++animation_diag.started;
    else if(event==NOVA_ANIMATION_DIAG_PAUSED)++animation_diag.paused;
    else if(event==NOVA_ANIMATION_DIAG_RESUMED)++animation_diag.resumed;
    else if(event==NOVA_ANIMATION_DIAG_COMPLETED)++animation_diag.completed;
    else if(event==NOVA_ANIMATION_DIAG_CANCELLED)++animation_diag.cancelled;
    else if(event==NOVA_ANIMATION_DIAG_REPEATED)++animation_diag.repeated;
    else if(event==NOVA_ANIMATION_DIAG_FAILED)++animation_diag.failed;
}
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
    (void)nova_animation_diag_initialize();
}

static bool valid(const nova_animation_t *a)
{
    return a && a->target && a->duration_ms && a->property < NOVA_PROPERTY_COUNT &&
           a->easing <= NOVA_EASE_SPRING && a->priority <= 3;
}

nova_animation_t *nova_motion_create(const nova_animation_t *description)
{
    if (!valid(description)) { ++diagnostics.rejected;++animation_diag.token_violations;
        ++animation_diag.failed;return 0; }
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
                ++animation_diag.conflicts;
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
        if(diagnostics.active>animation_diag.peak_active)
            animation_diag.peak_active=(uint16_t)diagnostics.active;
        animation_diag.active=(uint16_t)diagnostics.active;
        if(diagnostics.reduced_motion&&!nova_motion_apply_policy(&pool[i])){
            used[i]=false;if(diagnostics.active)--diagnostics.active;return 0;}
        last_cycle[i]=0;animation_record(&pool[i],NOVA_ANIMATION_DIAG_STARTED,
            pool[i].start_ms,false);return &pool[i];
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
    animation_diag.active=(uint16_t)diagnostics.active;
    animation_record(a,state==NOVA_MOTION_COMPLETED?NOVA_ANIMATION_DIAG_COMPLETED:
        NOVA_ANIMATION_DIAG_CANCELLED,diagnostics.current_ms,false);
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
        uint32_t pool_index=animation_index(a);
        if(pool_index<NOVA_MOTION_CAPACITY&&cycle>last_cycle[pool_index]){
            last_cycle[pool_index]=(uint32_t)cycle;
            animation_record(a,NOVA_ANIMATION_DIAG_REPEATED,now,false);
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
    animation_record(a,NOVA_ANIMATION_DIAG_PAUSED,diagnostics.current_ms,false);
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
    animation_record(a,NOVA_ANIMATION_DIAG_RESUMED,now,false);
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
             ++diagnostics.pauses;animation_record(&pool[i],NOVA_ANIMATION_DIAG_PAUSED,
                diagnostics.current_ms,false);}
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

bool nova_animation_diag_initialize(void)
{
    for(uint16_t i=0;i<NOVA_ANIMATION_DIAG_CAPACITY;++i)animation_trace[i]=(nova_animation_trace_t){0};
    animation_trace_head=animation_trace_count=0;animation_event_sequence=0;
    animation_frame_total=0;animation_frames=0;
    animation_render_us=animation_compositor_us=animation_layout_us=0;
    animation_software_renderer=false;animation_quality=animation_hardware_profile=0;
    animation_diag=(nova_animation_diag_status_t){.initialized=true,.passive=true,
        .read_only=true,.boot_unaffected=true};
    return true;
}
const nova_animation_trace_t *nova_animation_diag_get(uint32_t chronological_index)
{
    if(chronological_index>=animation_trace_count)return 0;
    return &animation_trace[(animation_trace_head+chronological_index)%NOVA_ANIMATION_DIAG_CAPACITY];
}
static bool animation_matches(const nova_animation_trace_t *trace,
    const nova_animation_diag_filter_t *filter)
{
    if(!filter)return true;
    if(filter->use_type&&trace->type!=filter->type)return false;
    if(filter->use_window&&trace->window_id!=filter->window_id)return false;
    if(filter->use_dialog&&trace->dialog_id!=filter->dialog_id)return false;
    if(filter->use_token&&trace->motion_token!=filter->motion_token)return false;
    if(filter->use_priority&&trace->priority!=filter->priority)return false;
    if(filter->use_duration&&(trace->actual_duration_us<filter->minimum_duration_us||
       trace->actual_duration_us>filter->maximum_duration_us))return false;
    return !filter->errors_only||trace->error;
}
const nova_animation_trace_t *nova_animation_diag_query(
    const nova_animation_diag_filter_t *filter,uint32_t matching_index)
{
    for(uint32_t i=0;i<animation_trace_count;++i){const nova_animation_trace_t *trace=nova_animation_diag_get(i);
        if(animation_matches(trace,filter)){if(!matching_index)return trace;--matching_index;}}
    return 0;
}
void nova_animation_diag_frame(uint32_t frame_time_us,uint32_t render_time_us,
    uint32_t compositor_time_us,uint32_t layout_time_us,bool software_renderer,
    uint8_t quality,uint8_t hardware_profile)
{
    uint32_t previous=animation_diag.last_frame_us;animation_diag.last_frame_us=frame_time_us;
    animation_render_us=render_time_us;animation_compositor_us=compositor_time_us;
    animation_layout_us=layout_time_us;animation_software_renderer=software_renderer;
    animation_quality=quality;animation_hardware_profile=hardware_profile;
    animation_frame_total+=frame_time_us;++animation_frames;
    animation_diag.average_frame_us=(uint32_t)(animation_frame_total/animation_frames);
    if(frame_time_us>16667u){++animation_diag.budget_violations;
        animation_diag.dropped_frames+=(frame_time_us-1u)/16667u;}
    if(previous){uint32_t jitter=frame_time_us>previous?frame_time_us-previous:previous-frame_time_us;
        if(jitter>animation_diag.maximum_jitter_us)animation_diag.maximum_jitter_us=jitter;
        if(jitter>4000u)++animation_diag.jitter_events;
        if(frame_time_us<1000u)++animation_diag.duplicate_frames;
        if(frame_time_us>33334u)++animation_diag.skipped_frames;}
}
static bool animation_put(char *output,uint32_t capacity,uint32_t *position,char value)
{if(*position>=capacity)return false;output[(*position)++]=value;return true;}
static bool animation_text(char *output,uint32_t capacity,uint32_t *position,const char *text)
{while(*text)if(!animation_put(output,capacity,position,*text++))return false;return true;}
static bool animation_number(char *output,uint32_t capacity,uint32_t *position,uint64_t number)
{char digits[24];uint8_t count=0;do{digits[count++]=(char)('0'+number%10u);number/=10u;}while(number);
 while(count){if(!animation_put(output,capacity,position,digits[--count]))return false;}
 return true;}
bool nova_animation_diag_export(nova_animation_export_format_t format,
    bool user_authorized,uint8_t *output,uint32_t capacity,uint32_t *written)
{
    if(written)*written=0;
    if(!user_authorized||!output||!written){++animation_diag.denied_exports;return false;}
    uint32_t position=0;
    if(format==NOVA_ANIMATION_EXPORT_BINARY){uint32_t required=(uint32_t)animation_trace_count*sizeof(nova_animation_trace_t);
        if(required>capacity)return false;
        uint8_t *bytes=output;
        for(uint32_t i=0;i<animation_trace_count;++i){const uint8_t *source=(const uint8_t*)nova_animation_diag_get(i);
            for(uint32_t j=0;j<sizeof(nova_animation_trace_t);++j)bytes[position++]=source[j];}}
    else {char *text=(char*)output;
        if(format==NOVA_ANIMATION_EXPORT_JSON&&!animation_text(text,capacity,&position,"{\"animations\":["))return false;
        else if(format==NOVA_ANIMATION_EXPORT_CSV&&!animation_text(text,capacity,&position,"event_id,event,type,duration_us,frame_us,error\n"))return false;
        else if(format==NOVA_ANIMATION_EXPORT_NDF&&!animation_text(text,capacity,&position,"NDF-ANIMATION-1\n"))return false;
        for(uint32_t i=0;i<animation_trace_count;++i){const nova_animation_trace_t *trace=nova_animation_diag_get(i);
            if(format==NOVA_ANIMATION_EXPORT_JSON){if(i&&!animation_put(text,capacity,&position,','))return false;
                if(!animation_text(text,capacity,&position,"{\"id\":"))return false;}
            if(!animation_number(text,capacity,&position,trace->event_id))return false;
            if(!animation_put(text,capacity,&position,format==NOVA_ANIMATION_EXPORT_CSV?',':' '))return false;
            if(!animation_number(text,capacity,&position,trace->event))return false;
            if(!animation_put(text,capacity,&position,format==NOVA_ANIMATION_EXPORT_CSV?',':' '))return false;
            if(!animation_number(text,capacity,&position,trace->type))return false;
            if(!animation_put(text,capacity,&position,format==NOVA_ANIMATION_EXPORT_CSV?',':' '))return false;
            if(!animation_number(text,capacity,&position,trace->actual_duration_us))return false;
            if(!animation_put(text,capacity,&position,format==NOVA_ANIMATION_EXPORT_CSV?',':' '))return false;
            if(!animation_number(text,capacity,&position,trace->frame_time_us))return false;
            if(format==NOVA_ANIMATION_EXPORT_JSON){if(!animation_text(text,capacity,&position,"}"))return false;}
            else if(!animation_put(text,capacity,&position,'\n'))return false;}
        if(format==NOVA_ANIMATION_EXPORT_JSON&&!animation_text(text,capacity,&position,"]}"))return false;
        if(position<capacity)text[position]='\0';}
    *written=position;++animation_diag.exports;return true;
}
void nova_animation_diag_reset(void){(void)nova_animation_diag_initialize();}
const nova_animation_diag_status_t *nova_animation_diag_status(void){return &animation_diag;}
