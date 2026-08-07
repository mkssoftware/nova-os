#ifndef NOVA_BOOT_INPUT_H
#define NOVA_BOOT_INPUT_H

#include <stdint.h>
#include <stdbool.h>
#include "controls.h"

#define NOVA_INPUT_QUEUE_CAPACITY 64u
#define NOVA_INPUT_DEVICE_CAPACITY 16u
#define NOVA_INPUT_POINTER_CAPACITY 8u
#define NOVA_INPUT_SHORTCUT_CAPACITY 32u
#define NOVA_INPUT_REPEAT_CAPACITY 8u

typedef enum {
    NOVA_EVENT_KEY_DOWN, NOVA_EVENT_KEY_UP, NOVA_EVENT_CHARACTER,
    NOVA_EVENT_POINTER_MOVE, NOVA_EVENT_POINTER_DOWN, NOVA_EVENT_POINTER_UP,
    NOVA_EVENT_POINTER_WHEEL, NOVA_EVENT_TOUCH_BEGIN, NOVA_EVENT_TOUCH_UPDATE,
    NOVA_EVENT_TOUCH_END, NOVA_EVENT_CONTROLLER_BUTTON,
    NOVA_EVENT_CONTROLLER_AXIS, NOVA_EVENT_NAVIGATION, NOVA_EVENT_SYSTEM
} nova_event_type_t;
typedef enum { NOVA_DEVICE_UNKNOWN, NOVA_DEVICE_KEYBOARD, NOVA_DEVICE_MOUSE,
    NOVA_DEVICE_TOUCH, NOVA_DEVICE_CONTROLLER, NOVA_DEVICE_ACCESSIBILITY } nova_device_type_t;
typedef enum { NOVA_DIRECTION_UP, NOVA_DIRECTION_DOWN, NOVA_DIRECTION_LEFT,
    NOVA_DIRECTION_RIGHT, NOVA_DIRECTION_FORWARD, NOVA_DIRECTION_BACKWARD } nova_direction_t;

typedef struct {
    uint64_t sequence, timestamp;
    uint32_t device_id, flags;
    nova_event_type_t type;
    union {
        struct { uint32_t key, modifiers, unicode; bool repeat; } key;
        struct { uint32_t pointer_id, buttons; int32_t x, y, wheel_x, wheel_y; } pointer;
        struct { nova_direction_t direction; } navigation;
        struct { uint32_t command; } system;
    };
} nova_input_event_t;
typedef struct { uint32_t id; nova_device_type_t type; bool connected; } nova_input_device_t;
typedef struct {
    uint32_t posted, dispatched, dropped, devices, captures, shortcuts,
             repeats, secure_events;
    uint64_t last_sequence;
} nova_input_diagnostics_t;

void nova_input_initialize(void);
bool nova_input_device_set(uint32_t id, nova_device_type_t type, bool connected);
bool nova_input_post(nova_input_event_t event);
bool nova_input_next(nova_input_event_t *event);
bool nova_input_pointer_capture(uint32_t pointer_id, nova_control_t *control);
void nova_input_pointer_release(uint32_t pointer_id);
nova_control_t *nova_input_pointer_target(const nova_input_event_t *event);
bool nova_input_focus_set(nova_control_t *control);
nova_control_t *nova_input_focus_get(void);
bool nova_input_neighbor_set(nova_control_t *source, nova_direction_t direction,
                             nova_control_t *target);
bool nova_input_focus_navigate(nova_direction_t direction);
bool nova_input_shortcut_register(uint32_t key, uint32_t modifiers,
                                  uint8_t scope, uint32_t command);
bool nova_input_shortcut_resolve(const nova_input_event_t *event, uint32_t *command);
bool nova_input_repeat_start(uint32_t input_id, uint64_t now);
void nova_input_repeat_stop(uint32_t input_id);
void nova_input_repeat_update(uint64_t now);
void nova_input_set_secure(bool enabled);
const nova_input_diagnostics_t *nova_input_diagnostics(void);

#endif
