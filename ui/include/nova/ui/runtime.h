#ifndef NOVA_UI_RUNTIME_H
#define NOVA_UI_RUNTIME_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NOVA_UI_MAX_NODES 128u
#define NOVA_UI_MAX_DAMAGE 32u
#define NOVA_UI_MAX_DISPLAYS 8u
#define NOVA_UI_MAX_SURFACES 32u
#define NOVA_UI_MAX_WINDOWS 32u
#define NOVA_UI_MAX_CAPABILITIES 64u
#define NOVA_UI_MAX_CONTRIBUTIONS 64u
#define NOVA_UI_LABEL_MAX 48u

/* Semantic ID wrappers deliberately prevent accidental cross-domain use. */
typedef struct { uint64_t value; } nova_ui_node_id_t;
typedef struct { uint64_t value; } nova_ui_accessibility_id_t;
typedef struct { uint64_t value; } nova_ui_display_id_t;
typedef struct { uint64_t value; } nova_ui_surface_id_t;
typedef struct { uint64_t value; } nova_ui_window_id_t;
typedef struct { uint64_t value; } nova_ui_object_id_t;
typedef struct { uint64_t value; } nova_ui_capability_id_t;
typedef struct { uint64_t value; } nova_ui_provider_id_t;
typedef struct { uint64_t value; } nova_ui_owner_id_t;

typedef int32_t nova_ui_dlu_t;

typedef struct {
    nova_ui_dlu_t x;
    nova_ui_dlu_t y;
    nova_ui_dlu_t width;
    nova_ui_dlu_t height;
} nova_ui_rect_t;

/* 2D affine transform; matrix values use fixed-point 1/1000 units. */
typedef struct {
    int32_t m11_milli;
    int32_t m12_milli;
    int32_t m21_milli;
    int32_t m22_milli;
    nova_ui_dlu_t translate_x_dlu;
    nova_ui_dlu_t translate_y_dlu;
} nova_ui_transform_t;

typedef enum {
    NOVA_UI_OK = 0,
    NOVA_UI_INVALID,
    NOVA_UI_NOT_FOUND,
    NOVA_UI_CAPACITY,
    NOVA_UI_DENIED,
    NOVA_UI_UNAVAILABLE,
    NOVA_UI_CONFLICT
} nova_ui_result_t;

typedef enum {
    NOVA_UI_ROLE_NONE = 0,
    NOVA_UI_ROLE_CONTENT,
    NOVA_UI_ROLE_PRIMARY_ACTION,
    NOVA_UI_ROLE_SECONDARY_ACTION,
    NOVA_UI_ROLE_NAVIGATION,
    NOVA_UI_ROLE_SEARCH,
    NOVA_UI_ROLE_SELECTION,
    NOVA_UI_ROLE_CONFIRMATION,
    NOVA_UI_ROLE_DESTRUCTIVE_ACTION,
    NOVA_UI_ROLE_INFORMATION,
    NOVA_UI_ROLE_WARNING,
    NOVA_UI_ROLE_PROGRESS,
    NOVA_UI_ROLE_TOOL,
    NOVA_UI_ROLE_WINDOW,
    NOVA_UI_ROLE_DESKTOP
} nova_ui_semantic_role_t;

typedef enum {
    NOVA_UI_ACTION_NONE = 0,
    NOVA_UI_ACTION_INVOKE,
    NOVA_UI_ACTION_SELECT,
    NOVA_UI_ACTION_TOGGLE,
    NOVA_UI_ACTION_EXPAND,
    NOVA_UI_ACTION_COLLAPSE,
    NOVA_UI_ACTION_FOCUS,
    NOVA_UI_ACTION_SET_VALUE,
    NOVA_UI_ACTION_CLOSE,
    NOVA_UI_ACTION_MOVE,
    NOVA_UI_ACTION_RESIZE
} nova_ui_action_t;

enum {
    NOVA_UI_STATE_AVAILABLE = 1u << 0,
    NOVA_UI_STATE_ACTIVE = 1u << 1,
    NOVA_UI_STATE_SELECTED = 1u << 2,
    NOVA_UI_STATE_BUSY = 1u << 3,
    NOVA_UI_STATE_PENDING = 1u << 4,
    NOVA_UI_STATE_COMPLETED = 1u << 5,
    NOVA_UI_STATE_FAILED = 1u << 6,
    NOVA_UI_STATE_FOCUSED = 1u << 7,
    NOVA_UI_STATE_DISABLED = 1u << 8
};

enum {
    NOVA_UI_DIRTY_NONE = 0u,
    NOVA_UI_DIRTY_LAYOUT = 1u << 0,
    NOVA_UI_DIRTY_VISUAL = 1u << 1,
    NOVA_UI_DIRTY_SEMANTICS = 1u << 2,
    NOVA_UI_DIRTY_CHILDREN = 1u << 3,
    NOVA_UI_DIRTY_ALL = 0x0fu
};

typedef enum {
    NOVA_UI_COLOR_SURFACE_PRIMARY = 0,
    NOVA_UI_COLOR_SURFACE_SECONDARY,
    NOVA_UI_COLOR_SURFACE_ELEVATED,
    NOVA_UI_COLOR_TEXT_PRIMARY,
    NOVA_UI_COLOR_TEXT_SECONDARY,
    NOVA_UI_COLOR_TEXT_DISABLED,
    NOVA_UI_COLOR_ACCENT_PRIMARY,
    NOVA_UI_COLOR_ACCENT_SECONDARY,
    NOVA_UI_COLOR_STATE_SUCCESS,
    NOVA_UI_COLOR_STATE_WARNING,
    NOVA_UI_COLOR_STATE_ERROR,
    NOVA_UI_COLOR_STATE_INFORMATION,
    NOVA_UI_COLOR_FOCUS,
    NOVA_UI_COLOR_SELECTION,
    NOVA_UI_COLOR_COUNT
} nova_ui_color_token_t;

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
} nova_ui_color_t;

typedef enum {
    NOVA_UI_THEME_DARK = 0,
    NOVA_UI_THEME_LIGHT,
    NOVA_UI_THEME_HIGH_CONTRAST
} nova_ui_theme_kind_t;

typedef struct {
    nova_ui_theme_kind_t kind;
    uint64_t generation;
    nova_ui_color_t colors[NOVA_UI_COLOR_COUNT];
    nova_ui_dlu_t spacing_dlu;
    nova_ui_dlu_t corner_radius_dlu;
    uint16_t text_scale_milli;
    bool reduce_motion;
    bool reduce_transparency;
    bool effects_enabled;
} nova_ui_theme_t;

typedef struct {
    nova_ui_node_id_t id;
    nova_ui_node_id_t parent;
    nova_ui_rect_t desired_bounds;
    nova_ui_rect_t actual_bounds;
    nova_ui_transform_t transform;
    int32_t z_order;
    uint16_t opacity_milli;
    uint32_t state;
    uint32_t dirty;
    nova_ui_semantic_role_t role;
    nova_ui_action_t action;
    nova_ui_object_id_t target_object;
    nova_ui_capability_id_t required_capability;
    nova_ui_color_token_t background_token;
    nova_ui_color_token_t foreground_token;
    bool desired_visible;
    bool actual_visible;
    bool system_layer;
    bool protected_content;
    char label[NOVA_UI_LABEL_MAX];
} nova_ui_node_t;

typedef struct {
    nova_ui_accessibility_id_t id;
    nova_ui_node_id_t source_node;
    nova_ui_semantic_role_t role;
    nova_ui_action_t action;
    uint32_t state;
    uint32_t virtual_item_count;
    uint32_t virtual_range_start;
    uint32_t virtual_range_count;
    bool secure;
    char name[NOVA_UI_LABEL_MAX];
} nova_ui_accessibility_node_t;

typedef enum {
    NOVA_UI_PIXEL_XRGB8888 = 0,
    NOVA_UI_PIXEL_ARGB8888
} nova_ui_pixel_format_t;

typedef struct {
    nova_ui_surface_id_t id;
    nova_ui_owner_id_t owner;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    nova_ui_pixel_format_t format;
    uint32_t buffer_age;
    bool visible;
    bool valid;
    bool protected_content;
    bool scanout_capable;
    nova_ui_rect_t damage[NOVA_UI_MAX_DAMAGE];
    size_t damage_count;
} nova_ui_surface_t;

typedef enum {
    NOVA_UI_WINDOW_NORMAL = 0,
    NOVA_UI_WINDOW_MINIMIZED,
    NOVA_UI_WINDOW_MAXIMIZED,
    NOVA_UI_WINDOW_FULLSCREEN,
    NOVA_UI_WINDOW_HIDDEN,
    NOVA_UI_WINDOW_SUSPENDED
} nova_ui_window_state_t;

typedef struct {
    nova_ui_window_id_t id;
    nova_ui_owner_id_t owner;
    nova_ui_surface_id_t surface;
    nova_ui_display_id_t display;
    nova_ui_rect_t bounds;
    nova_ui_window_state_t state;
    uint32_t workspace;
    int32_t z_order;
    bool system_ui;
    bool always_on_top;
    bool modal;
    bool alive;
} nova_ui_window_t;

typedef struct {
    uint32_t width_px;
    uint32_t height_px;
    uint32_t refresh_millihz;
} nova_ui_display_mode_t;

typedef struct {
    nova_ui_display_id_t id;
    nova_ui_display_mode_t mode;
    uint16_t scale_milli;
    uint32_t orientation_degrees;
    uint32_t vrr_min_millihz;
    uint32_t vrr_max_millihz;
    uint32_t current_refresh_millihz;
    bool connected;
    bool virtual_display;
    bool vrr_supported;
    bool vrr_enabled;
    bool fallback_mode;
} nova_ui_display_t;

typedef struct {
    nova_ui_capability_id_t id;
    nova_ui_provider_id_t provider;
    bool discoverable;
    bool available;
    bool authorized;
    bool remote;
    char semantic_type[NOVA_UI_LABEL_MAX];
} nova_ui_capability_t;

typedef enum {
    NOVA_UI_INPUT_KEY = 0,
    NOVA_UI_INPUT_POINTER_MOVE,
    NOVA_UI_INPUT_POINTER_BUTTON,
    NOVA_UI_INPUT_TOUCH_BEGIN,
    NOVA_UI_INPUT_TOUCH_UPDATE,
    NOVA_UI_INPUT_TOUCH_END,
    NOVA_UI_INPUT_PEN,
    NOVA_UI_INPUT_SYSTEM
} nova_ui_input_type_t;

typedef struct {
    nova_ui_input_type_t type;
    uint64_t monotonic_ns;
    uint64_t session_id;
    nova_ui_owner_id_t source;
    nova_ui_dlu_t x_dlu;
    nova_ui_dlu_t y_dlu;
    uint32_t contact_id;
    uint32_t code;
    bool pressed;
    bool virtual_source;
    bool raw;
    bool secure;
} nova_ui_input_event_t;

typedef struct {
    nova_ui_owner_id_t recipient;
    nova_ui_node_id_t node;
    bool consumed_by_system;
} nova_ui_input_route_t;

typedef enum {
    NOVA_UI_CONTRIBUTION_START = 0,
    NOVA_UI_CONTRIBUTION_RIBBON,
    NOVA_UI_CONTRIBUTION_DASHBOARD
} nova_ui_contribution_kind_t;

typedef struct {
    uint64_t id;
    nova_ui_contribution_kind_t kind;
    nova_ui_provider_id_t provider;
    nova_ui_capability_id_t capability;
    nova_ui_action_t action;
    uint16_t priority;
    bool visible;
    bool available;
    bool user_pinned;
    bool user_positioned;
    bool critical;
    char label[NOVA_UI_LABEL_MAX];
} nova_ui_contribution_t;

typedef enum {
    NOVA_UI_COMPOSE_SOFTWARE = 0,
    NOVA_UI_COMPOSE_GPU,
    NOVA_UI_COMPOSE_DIRECT_SCANOUT
} nova_ui_compose_path_t;

typedef struct {
    uint64_t requested_frames;
    uint64_t presented_frames;
    uint64_t missed_deadlines;
    uint64_t coalesced_requests;
    uint64_t last_present_ns;
    uint64_t next_deadline_ns;
    uint8_t queued_frames;
    bool damage_pending;
    bool input_pending;
    bool backpressure;
} nova_ui_frame_scheduler_t;

typedef struct {
    uint64_t scene_generation;
    uint64_t accessibility_generation;
    uint64_t reconciliation_count;
    uint64_t theme_changes;
    uint64_t rejected_surfaces;
    uint64_t isolated_provider_failures;
    uint64_t full_redraws;
    uint64_t input_routes;
    uint64_t denied_inputs;
    nova_ui_compose_path_t last_compose_path;
} nova_ui_diagnostics_t;

typedef struct {
    nova_ui_node_t nodes[NOVA_UI_MAX_NODES];
    size_t node_count;
    nova_ui_accessibility_node_t accessibility[NOVA_UI_MAX_NODES];
    size_t accessibility_count;
    nova_ui_rect_t scene_damage[NOVA_UI_MAX_DAMAGE];
    size_t scene_damage_count;
    nova_ui_display_t displays[NOVA_UI_MAX_DISPLAYS];
    size_t display_count;
    nova_ui_surface_t surfaces[NOVA_UI_MAX_SURFACES];
    size_t surface_count;
    nova_ui_window_t windows[NOVA_UI_MAX_WINDOWS];
    size_t window_count;
    nova_ui_capability_t capabilities[NOVA_UI_MAX_CAPABILITIES];
    size_t capability_count;
    nova_ui_contribution_t contributions[NOVA_UI_MAX_CONTRIBUTIONS];
    size_t contribution_count;
    nova_ui_theme_t theme;
    nova_ui_frame_scheduler_t scheduler;
    nova_ui_diagnostics_t diagnostics;
    nova_ui_node_id_t keyboard_focus;
    nova_ui_node_id_t accessibility_focus;
    nova_ui_node_id_t pointer_capture;
    nova_ui_owner_id_t capture_owner;
    uint64_t active_session;
    uint64_t active_touch_contact;
    nova_ui_node_id_t active_touch_target;
    bool gpu_available;
    bool resource_pressure;
    bool secure_input;
} nova_ui_runtime_t;

void nova_ui_runtime_init(nova_ui_runtime_t *runtime);

nova_ui_result_t nova_ui_theme_apply(nova_ui_runtime_t *runtime,
                                     nova_ui_theme_kind_t kind,
                                     bool reduce_motion,
                                     bool reduce_transparency,
                                     uint16_t text_scale_milli);
nova_ui_color_t nova_ui_theme_color(const nova_ui_runtime_t *runtime,
                                    nova_ui_color_token_t token);

nova_ui_result_t nova_ui_node_upsert(nova_ui_runtime_t *runtime,
                                     const nova_ui_node_t *desired);
nova_ui_result_t nova_ui_node_remove(nova_ui_runtime_t *runtime,
                                     nova_ui_node_id_t id);
nova_ui_result_t nova_ui_reconcile(nova_ui_runtime_t *runtime);
nova_ui_result_t nova_ui_scene_publish(nova_ui_runtime_t *runtime);
nova_ui_result_t nova_ui_scene_world_bounds(const nova_ui_runtime_t *runtime,
                                            nova_ui_node_id_t id,
                                            nova_ui_rect_t *bounds);
nova_ui_result_t nova_ui_accessibility_rebuild(nova_ui_runtime_t *runtime);
nova_ui_result_t nova_ui_accessibility_set_virtual_range(
    nova_ui_runtime_t *runtime, nova_ui_accessibility_id_t id,
    uint32_t total_items, uint32_t range_start, uint32_t range_count);
nova_ui_result_t nova_ui_accessibility_invoke(nova_ui_runtime_t *runtime,
                                              nova_ui_accessibility_id_t id,
                                              nova_ui_action_t action);

nova_ui_result_t nova_ui_damage_add(nova_ui_runtime_t *runtime,
                                    nova_ui_rect_t rect);
void nova_ui_damage_full(nova_ui_runtime_t *runtime,
                         nova_ui_rect_t display_bounds);

nova_ui_result_t nova_ui_display_connect(nova_ui_runtime_t *runtime,
                                         nova_ui_display_t display);
nova_ui_result_t nova_ui_display_disconnect(nova_ui_runtime_t *runtime,
                                            nova_ui_display_id_t id);
nova_ui_result_t nova_ui_display_set_vrr(nova_ui_runtime_t *runtime,
                                         nova_ui_display_id_t id,
                                         bool enabled);

nova_ui_result_t nova_ui_surface_create(nova_ui_runtime_t *runtime,
                                        nova_ui_surface_t surface);
nova_ui_result_t nova_ui_surface_damage(nova_ui_runtime_t *runtime,
                                        nova_ui_surface_id_t id,
                                        nova_ui_rect_t damage);
nova_ui_result_t nova_ui_window_create(nova_ui_runtime_t *runtime,
                                       nova_ui_window_t window);
nova_ui_result_t nova_ui_window_set_state(nova_ui_runtime_t *runtime,
                                          nova_ui_window_id_t id,
                                          nova_ui_window_state_t state);
nova_ui_result_t nova_ui_window_move(nova_ui_runtime_t *runtime,
                                     nova_ui_window_id_t id,
                                     nova_ui_display_id_t display,
                                     uint32_t workspace,
                                     nova_ui_rect_t bounds);
nova_ui_compose_path_t nova_ui_compose(nova_ui_runtime_t *runtime,
                                       nova_ui_display_id_t display);

nova_ui_result_t nova_ui_capability_register(nova_ui_runtime_t *runtime,
                                             nova_ui_capability_t capability);
nova_ui_result_t nova_ui_capability_set_available(
    nova_ui_runtime_t *runtime, nova_ui_capability_id_t id, bool available);
nova_ui_result_t nova_ui_capability_authorize(nova_ui_runtime_t *runtime,
                                              nova_ui_capability_id_t id,
                                              bool authorized);
bool nova_ui_capability_can_execute(const nova_ui_runtime_t *runtime,
                                    nova_ui_capability_id_t id);

nova_ui_result_t nova_ui_set_keyboard_focus(nova_ui_runtime_t *runtime,
                                            nova_ui_node_id_t node);
nova_ui_result_t nova_ui_set_accessibility_focus(nova_ui_runtime_t *runtime,
                                                 nova_ui_node_id_t node);
nova_ui_result_t nova_ui_capture_pointer(nova_ui_runtime_t *runtime,
                                         nova_ui_owner_id_t owner,
                                         nova_ui_node_id_t node);
void nova_ui_release_owner(nova_ui_runtime_t *runtime,
                           nova_ui_owner_id_t owner);
nova_ui_result_t nova_ui_route_input(nova_ui_runtime_t *runtime,
                                     const nova_ui_input_event_t *event,
                                     nova_ui_input_route_t *route);

void nova_ui_request_frame(nova_ui_runtime_t *runtime, bool from_input);
bool nova_ui_frame_due(nova_ui_runtime_t *runtime, uint64_t now_ns,
                       nova_ui_display_id_t display);
void nova_ui_frame_presented(nova_ui_runtime_t *runtime, uint64_t now_ns,
                             bool deadline_missed);

nova_ui_result_t nova_ui_contribution_add(nova_ui_runtime_t *runtime,
                                          nova_ui_contribution_t contribution);
nova_ui_result_t nova_ui_provider_failed(nova_ui_runtime_t *runtime,
                                         nova_ui_provider_id_t provider);
size_t nova_ui_contribution_count_visible(
    const nova_ui_runtime_t *runtime, nova_ui_contribution_kind_t kind);
void nova_ui_responsive_layout(nova_ui_runtime_t *runtime,
                               nova_ui_dlu_t available_width_dlu,
                               bool touch_input);
bool nova_ui_adaptive_change_allowed(bool explicit_user_choice,
                                     bool accessibility_required,
                                     uint16_t confidence_milli,
                                     bool changes_semantics);

#endif
