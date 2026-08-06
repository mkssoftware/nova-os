#ifndef NOVA_BOOT_MOTION_H
#define NOVA_BOOT_MOTION_H

#include <stdint.h>
#include <stdbool.h>

#define NOVA_MOTION_CAPACITY 256u

typedef enum {
    NOVA_MOTION_CREATED,
    NOVA_MOTION_WAITING,
    NOVA_MOTION_RUNNING,
    NOVA_MOTION_PAUSED,
    NOVA_MOTION_COMPLETED,
    NOVA_MOTION_CANCELLED
} nova_motion_state_t;

typedef enum {
    NOVA_EASE_LINEAR,
    NOVA_EASE_OUT_CUBIC,
    NOVA_EASE_IN_OUT,
    NOVA_EASE_SPRING
} nova_easing_t;

typedef enum {
    NOVA_PROPERTY_X,
    NOVA_PROPERTY_Y,
    NOVA_PROPERTY_WIDTH,
    NOVA_PROPERTY_HEIGHT,
    NOVA_PROPERTY_OPACITY,
    NOVA_PROPERTY_SCALE,
    NOVA_PROPERTY_BLUR,
    NOVA_PROPERTY_GLOW,
    NOVA_PROPERTY_SHADOW,
    NOVA_PROPERTY_TINT,
    NOVA_PROPERTY_BORDER,
    NOVA_PROPERTY_CORNER_RADIUS,
    NOVA_PROPERTY_COUNT
} nova_property_t;

typedef struct {
    int32_t *target;
    int32_t from;
    int32_t to;
    uint64_t start_ms;
    uint32_t delay_ms;
    uint32_t duration_ms;
    uint16_t group;
    uint8_t priority;
    uint8_t repeats;
    nova_property_t property;
    nova_easing_t easing;
    nova_motion_state_t state;
    bool auto_reverse;
    bool reversed;
    bool interruptible;
} nova_animation_t;

typedef struct {
    uint64_t current_ms;
    uint64_t previous_ms;
    uint32_t active;
    uint32_t completed;
    uint32_t cancelled;
    uint32_t rejected;
    bool paused;
    bool reduced_motion;
    uint8_t quality;
} nova_motion_diagnostics_t;

typedef struct {
    uint32_t id;
    int32_t value;
    nova_animation_t *animation;
} nova_transition_t;

typedef struct { int32_t opacity, scale; bool visible, focused; } nova_dialog_motion_t;
typedef struct { int32_t source_x, target_x; bool forward, running; } nova_navigation_motion_t;
typedef struct { int32_t ring_opacity, highlight; bool focused, selected; } nova_focus_motion_t;
typedef struct { int32_t displayed, requested; bool indeterminate, running, failed; } nova_progress_motion_t;
typedef struct {
    uint32_t frame_time_us;
    uint32_t scheduler_time_us;
    uint16_t violations;
    uint8_t quality;
} nova_motion_budget_t;

void nova_motion_initialize(void);
nova_animation_t *nova_motion_create(const nova_animation_t *description);
void nova_motion_update(uint64_t current_ms);
void nova_motion_cancel(nova_animation_t *animation);
bool nova_motion_interrupt(nova_animation_t *animation);
bool nova_motion_resume(nova_animation_t *animation, uint64_t current_ms);
bool nova_motion_reverse(nova_animation_t *animation, uint64_t current_ms);
bool nova_motion_redirect(nova_animation_t *animation, int32_t target,
                          uint64_t current_ms);
void nova_motion_pause_group(uint16_t group);
void nova_motion_set_reduced(bool enabled);
bool nova_motion_is_reduced(void);
const nova_motion_diagnostics_t *nova_motion_diagnostics(void);
int32_t nova_ease_apply(nova_easing_t easing, int32_t progress_16_16);
bool nova_transition_begin(nova_transition_t *transition, int32_t from, int32_t to,
                           uint32_t duration_ms, nova_easing_t easing);
void nova_transition_cancel(nova_transition_t *transition);
bool nova_dialog_enter(nova_dialog_motion_t *dialog);
bool nova_dialog_exit(nova_dialog_motion_t *dialog);
bool nova_navigation_begin(nova_navigation_motion_t *navigation, bool forward);
bool nova_focus_set(nova_focus_motion_t *focus, bool focused, bool selected);
bool nova_progress_set(nova_progress_motion_t *progress, int32_t value_per_mille);
void nova_progress_set_indeterminate(nova_progress_motion_t *progress, bool enabled);
void nova_motion_budget_update(uint32_t frame_time_us, uint32_t scheduler_time_us);
const nova_motion_budget_t *nova_motion_budget(void);

#endif
