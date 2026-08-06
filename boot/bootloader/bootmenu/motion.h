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

#endif
