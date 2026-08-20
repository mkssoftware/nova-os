#ifndef NOVA_BOOT_INPUT_H
#define NOVA_BOOT_INPUT_H

#include <stdint.h>
#include <stdbool.h>
#include "controls.h"

#define NOVA_INPUT_QUEUE_CAPACITY 64u
#define NOVA_INPUT_DEVICE_CAPACITY 16u
#define NOVA_INPUT_POINTER_CAPACITY 8u
#define NOVA_INPUT_SHORTCUT_CAPACITY 32u
#define NOVA_INPUT_SHORTCUT_HASH_CAPACITY 64u
#define NOVA_INPUT_REPEAT_CAPACITY 8u
#define NOVA_INPUT_TRACE_CAPACITY 256u
#define NOVA_FOCUS_SCOPE_CAPACITY 8u
#define NOVA_FOCUS_ORDER_CAPACITY 32u

typedef enum {
    NOVA_EVENT_KEY_DOWN, NOVA_EVENT_KEY_UP, NOVA_EVENT_CHARACTER,
    NOVA_EVENT_POINTER_MOVE, NOVA_EVENT_POINTER_DOWN, NOVA_EVENT_POINTER_UP,
    NOVA_EVENT_POINTER_WHEEL, NOVA_EVENT_TOUCH_BEGIN, NOVA_EVENT_TOUCH_UPDATE,
    NOVA_EVENT_TOUCH_END, NOVA_EVENT_CONTROLLER_BUTTON,
    NOVA_EVENT_CONTROLLER_AXIS, NOVA_EVENT_NAVIGATION, NOVA_EVENT_SYSTEM
} nova_event_type_t;
typedef enum { NOVA_DEVICE_UNKNOWN, NOVA_DEVICE_KEYBOARD, NOVA_DEVICE_MOUSE,
    NOVA_DEVICE_TOUCH, NOVA_DEVICE_CONTROLLER, NOVA_DEVICE_ACCESSIBILITY } nova_device_type_t;
typedef enum { NOVA_DEVICE_STATE_UNKNOWN, NOVA_DEVICE_STATE_DETECTED,
    NOVA_DEVICE_STATE_INITIALIZING, NOVA_DEVICE_STATE_READY,
    NOVA_DEVICE_STATE_DISCONNECTED, NOVA_DEVICE_STATE_ERROR } nova_device_state_t;
typedef enum { NOVA_DEVICE_EVENT_ADDED=1, NOVA_DEVICE_EVENT_REMOVED,
    NOVA_DEVICE_EVENT_CHANGED, NOVA_DEVICE_EVENT_ERROR,
    NOVA_DEVICE_EVENT_READY } nova_input_device_event_t;
typedef enum { NOVA_DIRECTION_UP, NOVA_DIRECTION_DOWN, NOVA_DIRECTION_LEFT,
    NOVA_DIRECTION_RIGHT, NOVA_DIRECTION_FORWARD, NOVA_DIRECTION_BACKWARD } nova_direction_t;
typedef enum {NOVA_NAV_EDGE_STOP,NOVA_NAV_EDGE_WRAP,NOVA_NAV_EDGE_CUSTOM}
nova_navigation_edge_t;
typedef enum {NOVA_CAPTURE_INACTIVE,NOVA_CAPTURE_PENDING,NOVA_CAPTURE_ACTIVE,
    NOVA_CAPTURE_RELEASED,NOVA_CAPTURE_CANCELLED} nova_capture_state_t;
typedef enum {NOVA_REPEAT_IDLE,NOVA_REPEAT_WAITING,NOVA_REPEAT_REPEATING,
    NOVA_REPEAT_PAUSED,NOVA_REPEAT_STOPPED} nova_repeat_state_t;

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
typedef struct { uint32_t id,instance,vendor_id,product_id;
    nova_device_type_t type;nova_device_state_t state;bool connected;
    uint64_t initialization_ms,registration_us;uint32_t changes,errors;
} nova_input_device_t;
typedef struct {
    uint32_t posted, dispatched, dropped, devices, captures, shortcuts,
             repeats, secure_events,double_clicks,capture_releases,
             capture_cancels,capture_errors,device_changes,device_errors;
    uint64_t last_sequence;
} nova_input_diagnostics_t;
typedef struct {uint32_t pointer_id,owner_id,releases,cancels,errors;
    uint64_t begin_ms,end_ms,duration_ms;nova_capture_state_t state;}
nova_capture_diagnostics_t;
typedef enum {NOVA_TRACE_KEY_DOWN,NOVA_TRACE_KEY_UP,NOVA_TRACE_KEY_REPEAT,
    NOVA_TRACE_MOUSE_MOVE,NOVA_TRACE_MOUSE_BUTTON,NOVA_TRACE_MOUSE_WHEEL,
    NOVA_TRACE_MOUSE_DOUBLE_CLICK,
    NOVA_TRACE_TOUCH_DOWN,NOVA_TRACE_TOUCH_MOVE,NOVA_TRACE_TOUCH_UP,
    NOVA_TRACE_FOCUS_CHANGED,NOVA_TRACE_SHORTCUT,NOVA_TRACE_CONTROLLER,
    NOVA_TRACE_SYSTEM} nova_input_trace_type_t;
typedef enum {NOVA_INPUT_TRACE_NDF,NOVA_INPUT_TRACE_JSON,NOVA_INPUT_TRACE_CSV,
    NOVA_INPUT_TRACE_BINARY} nova_input_trace_export_t;
typedef struct {
    uint64_t event_id,capture_us,dispatch_us,handler_end_us,completion_us;
    uint32_t device_id,target_object,focus_object,window_id,dialog_id,flags;
    nova_input_trace_type_t type;
    nova_device_type_t device_type;
    uint32_t scan_code,virtual_key,unicode,modifiers,repeat_count,shortcut_id;
    int32_t x,y,delta_x,delta_y,wheel_x,wheel_y;
    uint32_t buttons,pointer_id,touch_id,contact_width,contact_height,pressure,
             queue_length,queue_wait_us,dispatch_duration_us,handler_duration_us;
    uint16_t previous_focus,new_focus;
    nova_direction_t direction;
    bool pointer_capture,render_triggered,masked,completed;
} nova_input_trace_event_t;
typedef struct {
    uint64_t from_us,to_us;uint32_t device_id,target_object,window_id,dialog_id;
    nova_input_trace_type_t type;nova_device_type_t device_type;
    bool use_time,use_device_id,use_device_type,use_type,use_target,use_window,
         use_dialog,actions_only;
} nova_input_trace_filter_t;
typedef struct {uint32_t recorded,overwritten,dropped,duplicates,invalid_focus,
    queue_overflows,dispatch_timeouts,latency_warnings,exports,denied_exports,
    masked_events;uint16_t count;bool initialized,enabled,passive,boot_unaffected;}
nova_input_trace_status_t;
typedef bool (*nova_input_device_scanner_t)(void *context);
typedef struct nova_focus_scope {
    uint16_t id,count,current_index;bool active;
    nova_control_t *default_control,*saved_focus;
    nova_control_t *order[NOVA_FOCUS_ORDER_CAPACITY];
} nova_focus_scope_t;
typedef struct {uint16_t current_focus,current_scope;uint8_t scope_depth;
    uint32_t changes,navigations,restores,announcements,errors;
    uint64_t last_latency_us;bool focus_visible,changing;}
nova_focus_diagnostics_t;
typedef struct {uint16_t source,target,scope;uint32_t navigations,explicit_hits,
    geometric_hits,wraps,failed,errors;uint64_t duration_us;
    nova_direction_t direction;bool navigating;} nova_directional_diagnostics_t;
typedef enum {NOVA_SHORTCUT_GLOBAL,NOVA_SHORTCUT_CONTEXT,NOVA_SHORTCUT_DIALOG,
    NOVA_SHORTCUT_DEVELOPER} nova_shortcut_scope_t;
typedef struct {uint32_t key,modifiers,command;uint16_t context_id,focus_id;
    nova_shortcut_scope_t scope;bool focus_dependent,enabled;} nova_shortcut_t;
typedef struct {uint32_t registered,unregistered,executions,conflicts,failed,
    unauthorized,recursive,announcements;uint32_t last_key,last_modifiers,
    last_command;uint16_t last_context,last_focus;uint8_t last_scope;
    uint64_t last_duration_us;bool dispatching,developer_enabled;}
nova_shortcut_diagnostics_t;
typedef bool (*nova_shortcut_command_handler_t)(uint32_t command,void *context);
typedef struct {uint32_t input_id,device_id,target_id,count,errors;
    uint64_t start_time,next_repeat,stop_time,duration_ms;
    nova_repeat_state_t state;bool active,critical;} nova_repeat_context_t;
typedef struct {uint32_t starts,stops,pauses,resumes,repeats,rejected,errors,
    focus_stops,device_stops;uint32_t initial_delay_ms,interval_ms;
    bool initialized,enabled;} nova_repeat_diagnostics_t;

void nova_input_initialize(void);
bool nova_input_device_initialize(void);
bool nova_input_device_scan(void);
bool nova_input_device_scanner_set(nova_input_device_scanner_t scanner,void *context);
bool nova_input_device_set(uint32_t id, nova_device_type_t type, bool connected);
bool nova_input_device_transition(uint32_t id,nova_device_type_t type,
    nova_input_device_event_t event,uint64_t timestamp);
uint32_t nova_input_device_count(void);
const nova_input_device_t *nova_input_device_get(uint32_t id);
bool nova_input_device_connected(uint32_t id);
bool nova_input_post(nova_input_event_t event);
bool nova_input_next(nova_input_event_t *event);
bool nova_input_pointer_capture(uint32_t pointer_id, nova_control_t *control);
void nova_input_pointer_release(uint32_t pointer_id);
void nova_input_pointer_cancel(uint32_t pointer_id);
void nova_input_pointer_cancel_all(void);
void nova_input_advance_time(uint32_t elapsed_ms);
bool nova_input_pointer_is_captured(uint32_t pointer_id);
nova_control_t *nova_input_pointer_capture_owner(uint32_t pointer_id);
nova_capture_state_t nova_input_pointer_capture_state(uint32_t pointer_id);
const nova_capture_diagnostics_t *nova_input_pointer_capture_diagnostics(
    uint32_t pointer_id);
nova_control_t *nova_input_pointer_target(const nova_input_event_t *event);
bool nova_input_pointer_click_update(uint32_t device_id,uint32_t pointer_id,nova_control_t *target,
    int32_t x,int32_t y,uint64_t now_ms,bool button_down,uint32_t threshold_ms,
    uint32_t tolerance_pixels);
int32_t nova_input_pointer_scale_delta(int32_t delta,uint16_t speed_percent);
bool nova_input_focus_set(nova_control_t *control);
nova_control_t *nova_input_focus_get(void);
bool nova_input_focus_initialize(void);
bool nova_input_focus_scope_configure(nova_focus_scope_t *scope,uint16_t id,
    nova_control_t *default_control);
bool nova_input_focus_scope_register(nova_focus_scope_t *scope,nova_control_t *control);
bool nova_input_focus_scope_build_navigation(nova_focus_scope_t *scope,
    nova_navigation_edge_t edge);
bool nova_input_focus_enter_scope(nova_focus_scope_t *scope);
bool nova_input_focus_leave_scope(void);
bool nova_input_focus_next(void);
bool nova_input_focus_previous(void);
bool nova_input_focus_visible(void);
const nova_focus_scope_t *nova_input_focus_scope(void);
const nova_focus_diagnostics_t *nova_input_focus_diagnostics(void);
bool nova_input_neighbor_set(nova_control_t *source, nova_direction_t direction,
                             nova_control_t *target);
nova_control_t *nova_input_neighbor_find(nova_control_t *source,
    nova_direction_t direction);
bool nova_input_focus_navigate(nova_direction_t direction);
const nova_directional_diagnostics_t *nova_input_directional_diagnostics(void);
bool nova_input_shortcut_register(uint32_t key, uint32_t modifiers,
                                  uint8_t scope, uint32_t command);
bool nova_input_shortcut_register_ex(const nova_shortcut_t *shortcut);
bool nova_input_shortcut_unregister(uint32_t key,uint32_t modifiers,
    nova_shortcut_scope_t scope,uint16_t context_id,uint16_t focus_id);
void nova_input_shortcut_set_context(uint16_t context_id,uint16_t dialog_id);
void nova_input_shortcut_set_developer(bool enabled,bool authorized);
bool nova_input_shortcut_set_handler(nova_shortcut_command_handler_t handler,void *context);
bool nova_input_shortcut_resolve(const nova_input_event_t *event, uint32_t *command);
bool nova_input_shortcut_execute(const nova_input_event_t *event);
const nova_shortcut_diagnostics_t *nova_input_shortcut_diagnostics(void);
bool nova_input_repeat_start(uint32_t input_id, uint64_t now);
bool nova_input_repeat_start_event(uint32_t input_id,const nova_input_event_t *event,
    uint64_t now,bool critical);
void nova_input_repeat_stop(uint32_t input_id);
bool nova_input_repeat_pause(uint32_t input_id);
bool nova_input_repeat_resume(uint32_t input_id,uint64_t now);
bool nova_input_repeat_active(uint32_t input_id);
bool nova_input_repeat_configure(uint32_t initial_delay_ms,uint32_t interval_ms,
    bool enabled);
void nova_input_repeat_update(uint64_t now);
const nova_repeat_context_t *nova_input_repeat_context(uint32_t input_id);
const nova_repeat_diagnostics_t *nova_input_repeat_diagnostics(void);
void nova_input_set_secure(bool enabled);
const nova_input_diagnostics_t *nova_input_diagnostics(void);
bool nova_input_trace_initialize(void);
bool nova_input_trace_set_enabled(bool enabled,bool user_authorized);
bool nova_input_trace_enabled(void);
const nova_input_trace_event_t *nova_input_trace_get(uint32_t chronological_index);
const nova_input_trace_event_t *nova_input_trace_query(const nova_input_trace_filter_t *filter,
    uint32_t matching_index);
bool nova_input_trace_complete(uint64_t event_id,uint64_t handler_end_us,
    uint64_t completion_us,uint32_t target_object,uint32_t window_id,uint32_t dialog_id,
    bool render_triggered);
bool nova_input_trace_export(nova_input_trace_export_t format,bool user_authorized,
    uint8_t *output,uint32_t capacity,uint32_t *written);
void nova_input_trace_reset(void);
const nova_input_trace_status_t *nova_input_trace_status(void);

#endif
