#ifndef NOVA_BOOT_MOTION_H
#define NOVA_BOOT_MOTION_H

#include <stdint.h>
#include <stdbool.h>

#define NOVA_MOTION_CAPACITY 256u
#define NOVA_ANIMATION_DIAG_CAPACITY 256u

typedef enum {
    NOVA_MOTION_CREATED,
    NOVA_MOTION_WAITING,
    NOVA_MOTION_RUNNING,
    NOVA_MOTION_PAUSED,
    NOVA_MOTION_INTERRUPTED,
    NOVA_MOTION_RESUMING,
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
typedef enum {NOVA_MOTION_POLICY_ALLOWED,NOVA_MOTION_POLICY_REPLACED,
    NOVA_MOTION_POLICY_DISABLED,NOVA_MOTION_POLICY_INVALID} nova_motion_policy_t;

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
    int32_t current_velocity_16_16;
    uint32_t remaining_ms;
    uint64_t paused_at_ms;
} nova_animation_t;

typedef struct {
    uint64_t current_ms;
    uint64_t previous_ms;
    uint32_t active;
    uint32_t completed;
    uint32_t cancelled;
    uint32_t rejected;
    uint32_t interruptions,pauses,resumes,reversals,redirects;
    uint32_t policy_reloads,policy_replacements,policy_disables,policy_errors;
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
    uint32_t property_time_us,material_time_us,memory_bytes;
    uint16_t active_animations,material_animations;
    uint16_t violations;
    uint8_t quality,fallback_step;
    bool glow_enabled,shadow_enabled,blur_enabled,spring_enabled;
    bool material_enabled,decorative_enabled,safe_mode;
} nova_motion_budget_t;
typedef enum {NOVA_ANIMATION_GENERAL,NOVA_ANIMATION_TIMELINE,NOVA_ANIMATION_SPRING,
    NOVA_ANIMATION_MATERIAL,NOVA_ANIMATION_PROGRESS,NOVA_ANIMATION_TYPE_COUNT}
    nova_animation_type_t;
typedef enum {NOVA_ANIMATION_DIAG_STARTED,NOVA_ANIMATION_DIAG_PAUSED,
    NOVA_ANIMATION_DIAG_RESUMED,NOVA_ANIMATION_DIAG_COMPLETED,
    NOVA_ANIMATION_DIAG_CANCELLED,NOVA_ANIMATION_DIAG_REPEATED,
    NOVA_ANIMATION_DIAG_FAILED} nova_animation_diag_event_t;
typedef enum {NOVA_ANIMATION_EXPORT_NDF,NOVA_ANIMATION_EXPORT_JSON,
    NOVA_ANIMATION_EXPORT_CSV,NOVA_ANIMATION_EXPORT_BINARY}
    nova_animation_export_format_t;
typedef struct {
    uint64_t event_id,timestamp_us,start_us,end_us;
    uint32_t animation_id,target_id,parent_id,transition_id,motion_token;
    nova_animation_diag_event_t event;
    nova_animation_type_t type;
    nova_property_t property;
    nova_easing_t easing;
    nova_motion_state_t state;
    uint32_t planned_duration_us,actual_duration_us,start_latency_us,
             completion_latency_us,frame_time_us,render_time_us,compositor_time_us,
             layout_time_us,cpu_time_us,gpu_time_us,rendered_frames,window_id,dialog_id;
    uint16_t group;
    uint8_t repeats,priority,quality,hardware_profile;
    bool software_renderer,error,budget_exceeded;
} nova_animation_trace_t;
typedef struct {
    nova_animation_type_t type;uint32_t window_id,dialog_id,motion_token;
    uint8_t priority;uint32_t minimum_duration_us,maximum_duration_us;
    bool use_type,use_window,use_dialog,use_token,use_priority,use_duration,
         errors_only;
} nova_animation_diag_filter_t;
typedef struct {
    uint32_t recorded,overwritten,started,paused,resumed,completed,cancelled,
             repeated,failed,dropped_frames,duplicate_frames,skipped_frames,
             synchronization_errors,jitter_events,token_violations,
             conflicts,infinite_loops,budget_violations,exports,denied_exports;
    uint16_t count,active,peak_active,groups;
    uint32_t last_frame_us,average_frame_us,maximum_jitter_us;
    bool initialized,passive,read_only,boot_unaffected;
} nova_animation_diag_status_t;

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
bool nova_motion_set_reduced(bool enabled);
bool nova_motion_is_reduced(void);
nova_motion_policy_t nova_motion_policy(nova_property_t property);
bool nova_motion_apply_policy(nova_animation_t *animation);
bool nova_motion_reload_policy(void);
const nova_motion_diagnostics_t *nova_motion_diagnostics(void);
int32_t nova_ease_apply(nova_easing_t easing, int32_t progress_16_16);
bool nova_transition_begin(nova_transition_t *transition, int32_t from, int32_t to,
                           uint32_t duration_ms, nova_easing_t easing);
void nova_transition_cancel(nova_transition_t *transition);
bool nova_dialog_enter(nova_dialog_motion_t *dialog);
bool nova_dialog_exit(nova_dialog_motion_t *dialog);
bool nova_dialog_motion_running(void);
void nova_dialog_motion_cancel(nova_dialog_motion_t *dialog);
bool nova_navigation_begin(nova_navigation_motion_t *navigation, bool forward);
bool nova_focus_set(nova_focus_motion_t *focus, bool focused, bool selected);
bool nova_progress_set(nova_progress_motion_t *progress, int32_t value_per_mille);
void nova_progress_set_indeterminate(nova_progress_motion_t *progress, bool enabled);
void nova_motion_budget_update(uint32_t frame_time_us, uint32_t scheduler_time_us);
bool nova_motion_budget_initialize(void);
const nova_motion_budget_t *nova_motion_budget_get(void);
bool nova_motion_budget_can_allocate(nova_animation_type_t type);
void nova_motion_budget_apply_fallback(void);
const nova_motion_budget_t *nova_motion_budget(void);
bool nova_animation_diag_initialize(void);
const nova_animation_trace_t *nova_animation_diag_get(uint32_t chronological_index);
const nova_animation_trace_t *nova_animation_diag_query(
    const nova_animation_diag_filter_t *filter,uint32_t matching_index);
void nova_animation_diag_frame(uint32_t frame_time_us,uint32_t render_time_us,
    uint32_t compositor_time_us,uint32_t layout_time_us,bool software_renderer,
    uint8_t quality,uint8_t hardware_profile);
bool nova_animation_diag_export(nova_animation_export_format_t format,
    bool user_authorized,uint8_t *output,uint32_t capacity,uint32_t *written);
void nova_animation_diag_reset(void);
const nova_animation_diag_status_t *nova_animation_diag_status(void);

#endif
