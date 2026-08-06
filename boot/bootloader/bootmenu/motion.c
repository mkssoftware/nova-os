#include "motion.h"

#define FIXED_ONE 65536
#define QUALITY_SAFE 4u

static nova_animation_t pool[NOVA_MOTION_CAPACITY];
static bool used[NOVA_MOTION_CAPACITY];
static nova_motion_diagnostics_t diagnostics;
static nova_motion_budget_t budget;

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
    budget = (nova_motion_budget_t){0, 0, 0, 2};
}

static bool valid(const nova_animation_t *a)
{
    return a && a->target && a->duration_ms && a->property < NOVA_PROPERTY_COUNT &&
           a->easing <= NOVA_EASE_SPRING && a->priority <= 3;
}

nova_animation_t *nova_motion_create(const nova_animation_t *description)
{
    if (!valid(description)) { ++diagnostics.rejected; return 0; }
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
        ++diagnostics.active;
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
    if (now < diagnostics.current_ms) now = diagnostics.current_ms;
    diagnostics.previous_ms = diagnostics.current_ms;
    diagnostics.current_ms = now;
    if (diagnostics.paused) return;
    for (uint32_t i = 0; i < NOVA_MOTION_CAPACITY; ++i) {
        if (!used[i]) continue;
        nova_animation_t *a = &pool[i];
        if (a->state == NOVA_MOTION_COMPLETED || a->state == NOVA_MOTION_CANCELLED ||
            a->state == NOVA_MOTION_PAUSED) continue;
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
        *a->target = a->from + (int32_t)(((int64_t)(a->to - a->from) * eased) / FIXED_ONE);
    }
}

void nova_motion_cancel(nova_animation_t *a)
{
    if (a && a->state != NOVA_MOTION_COMPLETED && a->state != NOVA_MOTION_CANCELLED)
        finish(a, NOVA_MOTION_CANCELLED);
}

bool nova_motion_interrupt(nova_animation_t *a)
{
    if (!a || !a->interruptible || a->state != NOVA_MOTION_RUNNING) return false;
    a->state = NOVA_MOTION_PAUSED;
    return true;
}

bool nova_motion_resume(nova_animation_t *a, uint64_t now)
{
    if (!a || a->state != NOVA_MOTION_PAUSED) return false;
    a->from = *a->target;
    a->start_ms = now;
    a->delay_ms = 0;
    a->state = NOVA_MOTION_RUNNING;
    return true;
}

bool nova_motion_reverse(nova_animation_t *a, uint64_t now)
{
    if (!a || !a->interruptible) return false;
    a->from = *a->target;
    int32_t old_to = a->to;
    a->to = a->reversed ? old_to : 0;
    a->reversed = !a->reversed;
    a->start_ms = now;
    a->state = NOVA_MOTION_RUNNING;
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
    return true;
}

void nova_motion_pause_group(uint16_t group)
{
    for (uint32_t i = 0; i < NOVA_MOTION_CAPACITY; ++i)
        if (used[i] && pool[i].group == group && pool[i].state == NOVA_MOTION_RUNNING)
            pool[i].state = NOVA_MOTION_PAUSED;
}

void nova_motion_set_reduced(bool enabled) { diagnostics.reduced_motion = enabled; }
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
        NOVA_MOTION_CREATED, false, false, true
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
    dialog->scale = diagnostics.reduced_motion ? 1000 : 960;
    nova_animation_t fade = { &dialog->opacity, 0, 255, diagnostics.current_ms, 0,
        diagnostics.reduced_motion ? 150u : 220u, 20, 3, 0, NOVA_PROPERTY_OPACITY,
        NOVA_EASE_OUT_CUBIC, NOVA_MOTION_CREATED, false, false, true };
    return nova_motion_create(&fade) != 0;
}

bool nova_dialog_exit(nova_dialog_motion_t *dialog)
{
    if (!dialog || !dialog->visible) return false;
    nova_animation_t fade = { &dialog->opacity, dialog->opacity, 0,
        diagnostics.current_ms, 0, diagnostics.reduced_motion ? 150u : 220u,
        20, 3, 0, NOVA_PROPERTY_OPACITY, NOVA_EASE_OUT_CUBIC,
        NOVA_MOTION_CREATED, false, false, true };
    dialog->focused = false;
    return nova_motion_create(&fade) != 0;
}

bool nova_navigation_begin(nova_navigation_motion_t *navigation, bool forward)
{
    if (!navigation || navigation->running) return false;
    navigation->forward = forward; navigation->running = true;
    navigation->source_x = 0;
    navigation->target_x = diagnostics.reduced_motion ? 0 : (forward ? 1000 : -1000);
    if (diagnostics.reduced_motion) return true;
    nova_animation_t slide = { &navigation->source_x, 0, forward ? -1000 : 1000,
        diagnostics.current_ms, 0, 180, 21, 2, 0, NOVA_PROPERTY_X,
        NOVA_EASE_OUT_CUBIC, NOVA_MOTION_CREATED, false, false, true };
    return nova_motion_create(&slide) != 0;
}

bool nova_focus_set(nova_focus_motion_t *focus, bool focused, bool selected)
{
    if (!focus || (focus->focused == focused && focus->selected == selected)) return false;
    focus->focused = focused; focus->selected = selected;
    int32_t destination = focused ? 255 : 0;
    nova_animation_t ring = { &focus->ring_opacity, focus->ring_opacity, destination,
        diagnostics.current_ms, 0, diagnostics.reduced_motion ? 100u : 120u,
        22, 3, 0, NOVA_PROPERTY_BORDER, NOVA_EASE_OUT_CUBIC,
        NOVA_MOTION_CREATED, false, false, true };
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
    nova_animation_t change = { &progress->displayed, progress->displayed, value,
        diagnostics.current_ms, 0, 120, 23, 2, 0, NOVA_PROPERTY_WIDTH,
        NOVA_EASE_LINEAR, NOVA_MOTION_CREATED, false, false, true };
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
        if (budget.quality < QUALITY_SAFE) ++budget.quality;
        diagnostics.quality = budget.quality;
    }
}

const nova_motion_budget_t *nova_motion_budget(void) { return &budget; }
