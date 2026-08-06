#include "motion.h"

#define FIXED_ONE 65536
#define QUALITY_SAFE 4u

static nova_animation_t pool[NOVA_MOTION_CAPACITY];
static bool used[NOVA_MOTION_CAPACITY];
static nova_motion_diagnostics_t diagnostics;

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
