#ifndef NOVA_BOOT_CONTROLS_H
#define NOVA_BOOT_CONTROLS_H

#include <stdint.h>
#include <stdbool.h>
#include "compositor.h"
#include "design.h"

#define NOVA_CONTROL_CAPACITY 128u
#define NOVA_CONTROL_TEXT_CAPACITY 96u
#define NOVA_CONTROL_NONE 0xffffu
#define NOVA_LIST_ITEM_CAPACITY 64u
#define NOVA_STYLE_CAPACITY 32u
#define NOVA_TEMPLATE_CAPACITY 64u
#define NOVA_CONTROL_TEST_CAPACITY 12u

typedef enum {
    NOVA_CONTROL_BUTTON, NOVA_CONTROL_ICON_BUTTON, NOVA_CONTROL_MENU_BUTTON,
    NOVA_CONTROL_CHECKBOX, NOVA_CONTROL_SWITCH, NOVA_CONTROL_SLIDER,
    NOVA_CONTROL_TEXT_FIELD, NOVA_CONTROL_PASSWORD_FIELD, NOVA_CONTROL_LABEL,
    NOVA_CONTROL_ICON, NOVA_CONTROL_IMAGE, NOVA_CONTROL_LIST,
    NOVA_CONTROL_LIST_ITEM, NOVA_CONTROL_SCROLL_VIEW, NOVA_CONTROL_SCROLLBAR,
    NOVA_CONTROL_PROGRESS, NOVA_CONTROL_SPINNER, NOVA_CONTROL_CARD,
    NOVA_CONTROL_TILE, NOVA_CONTROL_STATUS_BADGE, NOVA_CONTROL_SEPARATOR,
    NOVA_CONTROL_TOOLTIP, NOVA_CONTROL_CONTEXT_MENU, NOVA_CONTROL_BREADCRUMB
} nova_control_type_t;

typedef enum {
    NOVA_CONTROL_CREATED, NOVA_CONTROL_INITIALIZED, NOVA_CONTROL_VISIBLE,
    NOVA_CONTROL_ACTIVE, NOVA_CONTROL_DISABLED, NOVA_CONTROL_DESTROYED
} nova_control_state_t;

typedef enum {
    NOVA_TEXT_INPUT_STANDARD, NOVA_TEXT_INPUT_NUMERIC, NOVA_TEXT_INPUT_HEXADECIMAL,
    NOVA_TEXT_INPUT_PASSWORD, NOVA_TEXT_INPUT_FILENAME, NOVA_TEXT_INPUT_SEARCH
} nova_text_input_mode_t;

typedef enum {
    NOVA_ACTIVITY_SPINNER, NOVA_ACTIVITY_RING,
    NOVA_ACTIVITY_DOTS, NOVA_ACTIVITY_ARC
} nova_activity_style_t;

typedef enum { NOVA_SCROLLBAR_HORIZONTAL, NOVA_SCROLLBAR_VERTICAL }
nova_scrollbar_orientation_t;
typedef enum { NOVA_CARD_STANDARD,NOVA_CARD_INFORMATION,NOVA_CARD_WARNING,
    NOVA_CARD_ERROR,NOVA_CARD_SUCCESS,NOVA_CARD_INTERACTIVE,NOVA_CARD_CUSTOM }
nova_card_type_t;
typedef enum { NOVA_TILE_STANDARD,NOVA_TILE_PRIMARY,NOVA_TILE_RECOVERY,
    NOVA_TILE_MAINTENANCE,NOVA_TILE_DIAGNOSTIC,NOVA_TILE_DESTRUCTIVE,NOVA_TILE_CUSTOM }
nova_boot_tile_type_t;
typedef enum { NOVA_BUTTON_STANDARD,NOVA_BUTTON_PRIMARY,NOVA_BUTTON_SECONDARY,
    NOVA_BUTTON_DESTRUCTIVE,NOVA_BUTTON_ICON,NOVA_BUTTON_TEXT,NOVA_BUTTON_TOGGLE }
nova_button_type_t;
typedef enum { NOVA_LABEL_STANDARD,NOVA_LABEL_HEADER,NOVA_LABEL_SUBHEADER,
    NOVA_LABEL_CAPTION,NOVA_LABEL_STATUS,NOVA_LABEL_ERROR,NOVA_LABEL_WARNING,
    NOVA_LABEL_SUCCESS,NOVA_LABEL_INFORMATION } nova_label_type_t;
typedef enum { NOVA_ALIGN_LEFT_TOP,NOVA_ALIGN_CENTER_TOP,NOVA_ALIGN_RIGHT_TOP,
    NOVA_ALIGN_LEFT_MIDDLE,NOVA_ALIGN_CENTER_MIDDLE,NOVA_ALIGN_RIGHT_MIDDLE,
    NOVA_ALIGN_LEFT_BOTTOM,NOVA_ALIGN_CENTER_BOTTOM,NOVA_ALIGN_RIGHT_BOTTOM }
nova_control_alignment_t;
typedef enum { NOVA_IMAGE_ORIGINAL,NOVA_IMAGE_STRETCH,NOVA_IMAGE_FIT,
    NOVA_IMAGE_FILL,NOVA_IMAGE_CENTER } nova_image_scaling_mode_t;
typedef enum { NOVA_IMAGE_BRANDING_LOGO } nova_image_resource_t;
typedef enum { NOVA_SEPARATOR_HORIZONTAL,NOVA_SEPARATOR_VERTICAL }
nova_separator_orientation_t;
typedef enum { NOVA_LIST_SELECTION_NONE,NOVA_LIST_SELECTION_SINGLE,
    NOVA_LIST_SELECTION_MULTIPLE } nova_list_selection_mode_t;
typedef enum { NOVA_LIST_ITEM_NORMAL,NOVA_LIST_ITEM_HOVER,NOVA_LIST_ITEM_FOCUS,
    NOVA_LIST_ITEM_SELECTED,NOVA_LIST_ITEM_DISABLED,NOVA_LIST_ITEM_ERROR }
nova_list_item_state_t;
typedef enum { NOVA_BADGE_INFORMATION,NOVA_BADGE_SUCCESS,NOVA_BADGE_WARNING,
    NOVA_BADGE_ERROR,NOVA_BADGE_CRITICAL,NOVA_BADGE_NEUTRAL,NOVA_BADGE_CUSTOM }
nova_badge_type_t;
typedef enum { NOVA_CONTROL_MATERIAL_OPAQUE,NOVA_CONTROL_MATERIAL_GLASS,
    NOVA_CONTROL_MATERIAL_ACRYLIC,NOVA_CONTROL_MATERIAL_TRANSPARENT }
nova_control_material_t;
enum { NOVA_STYLE_BACKGROUND=1u,NOVA_STYLE_FOREGROUND=2u,NOVA_STYLE_ACCENT=4u,
    NOVA_STYLE_DISABLED=8u,NOVA_STYLE_BORDER=16u,NOVA_STYLE_INFORMATION=32u,
    NOVA_STYLE_SUCCESS=64u,NOVA_STYLE_WARNING=128u,NOVA_STYLE_ERROR=256u,
    NOVA_STYLE_GEOMETRY=512u,NOVA_STYLE_ALL=1023u };
enum { NOVA_TEMPLATE_PART_BACKGROUND=1u,NOVA_TEMPLATE_PART_BORDER=2u,
    NOVA_TEMPLATE_PART_ICON=4u,NOVA_TEMPLATE_PART_TEXT=8u,
    NOVA_TEMPLATE_PART_CONTENT=16u,NOVA_TEMPLATE_PART_FOCUS=32u,
    NOVA_TEMPLATE_PART_TRACK=64u,NOVA_TEMPLATE_PART_THUMB=128u,
    NOVA_TEMPLATE_PART_STATUS=256u,NOVA_TEMPLATE_PART_IMAGE=512u };

enum {
    NOVA_CONTROL_FLAG_VISIBLE = 1u, NOVA_CONTROL_FLAG_ENABLED = 2u,
    NOVA_CONTROL_FLAG_FOCUSED = 4u, NOVA_CONTROL_FLAG_DIRTY = 8u,
    NOVA_CONTROL_FLAG_HOVERED = 16u, NOVA_CONTROL_FLAG_PRESSED = 32u,
    NOVA_CONTROL_FLAG_SELECTED = 64u, NOVA_CONTROL_FLAG_MODAL = 128u,
    NOVA_CONTROL_FLAG_PASSWORD = 256u, NOVA_CONTROL_FLAG_DECORATIVE = 512u
    , NOVA_CONTROL_FLAG_CHECKED = 1024u, NOVA_CONTROL_FLAG_INDETERMINATE = 2048u,
    NOVA_CONTROL_FLAG_BUSY = 4096u, NOVA_CONTROL_FLAG_ERROR = 8192u,
    NOVA_CONTROL_FLAG_READONLY = 16384u, NOVA_CONTROL_FLAG_LOCKED = 32768u,
    NOVA_CONTROL_FLAG_EXPANDED = 65536u,
    NOVA_CONTROL_FLAG_REVEALED = 131072u
};

typedef struct {
    uint32_t background, foreground, accent, disabled;
    uint32_t border, information, success, warning, error;
    uint16_t corner_dlu, border_dlu, padding_dlu;
} nova_control_style_t;
typedef struct {uint16_t id,parent;uint32_t override_mask;
    nova_control_material_t material;nova_control_style_t resolved;bool valid;}
nova_style_descriptor_t;
typedef struct {uint16_t id,parent;nova_control_type_t control_type;
    uint32_t own_parts,resolved_parts,required_parts;uint16_t visual_nodes;bool valid;}
nova_control_template_t;

typedef struct {
    uint16_t id, parent, first_child, next_sibling;
    nova_control_type_t type;
    nova_control_state_t state;
    nova_interaction_state_t interaction_state;
    uint32_t flags;
    nova_rect_t bounds;
    nova_control_style_t style;
    int32_t value, minimum, maximum, step;
    int32_t scroll_x,scroll_y,content_width,content_height,viewport_width,viewport_height;
    uint32_t action;
    uint16_t template_id,visual_template_id,style_id;
    uint16_t accessibility_role;
    bool action_fired;
    char text[NOVA_CONTROL_TEXT_CAPACITY];
    char accessibility_name[NOVA_CONTROL_TEXT_CAPACITY];
    char placeholder[NOVA_CONTROL_TEXT_CAPACITY];
    char status_text[NOVA_CONTROL_TEXT_CAPACITY];
    uint16_t text_length, caret, selection_start, selection_end, maximum_length;
    nova_text_input_mode_t input_mode;
} nova_control_t;

typedef struct {
    uint16_t active, focused, created, destroyed;
    uint32_t invalid_transitions, rejected_actions, hit_tests, value_changes,
             toggles, renders, fallback_renders,events_dispatched,events_handled,
             events_bubbled,event_errors;
} nova_control_diagnostics_t;
typedef enum {NOVA_CONTROL_EVENT_CREATE,NOVA_CONTROL_EVENT_DESTROY,
    NOVA_CONTROL_EVENT_FOCUS,NOVA_CONTROL_EVENT_BLUR,NOVA_CONTROL_EVENT_MOUSE_ENTER,
    NOVA_CONTROL_EVENT_MOUSE_LEAVE,NOVA_CONTROL_EVENT_MOUSE_MOVE,
    NOVA_CONTROL_EVENT_MOUSE_DOWN,NOVA_CONTROL_EVENT_MOUSE_UP,NOVA_CONTROL_EVENT_CLICK,
    NOVA_CONTROL_EVENT_DOUBLE_CLICK,NOVA_CONTROL_EVENT_KEY_DOWN,NOVA_CONTROL_EVENT_KEY_UP,
    NOVA_CONTROL_EVENT_CHARACTER_INPUT,NOVA_CONTROL_EVENT_VALUE_CHANGED,
    NOVA_CONTROL_EVENT_STATE_CHANGED,NOVA_CONTROL_EVENT_LAYOUT_CHANGED,
    NOVA_CONTROL_EVENT_THEME_CHANGED,NOVA_CONTROL_EVENT_CAPTURE_CANCEL}
nova_control_event_type_t;
typedef enum {NOVA_CONTROL_EVENT_TARGET,NOVA_CONTROL_EVENT_BUBBLE}
nova_control_event_phase_t;
typedef struct {nova_control_event_type_t type;nova_control_event_phase_t phase;
    uint16_t target_id,current_id;uint16_t route_depth;uint32_t device_id,key,character;
    int32_t x,y,value;bool handled;} nova_control_event_t;
typedef bool (*nova_control_event_handler_t)(nova_control_t *control,
    nova_control_event_t *event,void *context);
typedef struct {uint32_t field_id,input_length,focus_changes,failed_attempts,
    validation_errors,errors,clears,reveals;bool reveal_enabled,revealed,
    content_exposed_to_accessibility;} nova_password_diagnostics_t;

typedef enum {NOVA_CONTROL_TEST_PASSED,NOVA_CONTROL_TEST_FAILED,
    NOVA_CONTROL_TEST_SKIPPED} nova_control_test_outcome_t;
typedef enum {NOVA_CONTROL_TEST_BUTTON,NOVA_CONTROL_TEST_TOGGLE_BUTTON,
    NOVA_CONTROL_TEST_CHECKBOX,NOVA_CONTROL_TEST_RADIO_BUTTON,NOVA_CONTROL_TEST_LIST,
    NOVA_CONTROL_TEST_COMBOBOX,NOVA_CONTROL_TEST_TEXT_FIELD,NOVA_CONTROL_TEST_SLIDER,
    NOVA_CONTROL_TEST_PROGRESS,NOVA_CONTROL_TEST_SCROLLBAR,NOVA_CONTROL_TEST_DIALOG,
    NOVA_CONTROL_TEST_NAVIGATION} nova_control_test_case_t;
typedef struct {uint32_t control_id,interaction_id;nova_control_test_outcome_t status;
    uint64_t duration_us;uint32_t state_mask,event_count,detected_errors;
    uint8_t input_device;bool focus_changed,event_order_valid,visual_invalidated,
    accessibility_valid,configuration_changed;} nova_control_test_result_t;
typedef struct {uint8_t count,passed,failed,skipped;uint32_t reports;
    bool initialized,isolated,deterministic,configuration_unchanged;
} nova_control_test_summary_t;

void nova_controls_initialize(const nova_control_style_t *base_style);
nova_control_t *nova_control_create(nova_control_type_t type);
bool nova_control_destroy(nova_control_t *control);
bool nova_control_set_parent(nova_control_t *child, nova_control_t *parent);
bool nova_control_set_event_handler(nova_control_t *control,
    nova_control_event_handler_t handler,void *context);
bool nova_control_dispatch_event(nova_control_t *target,nova_control_event_t *event);
bool nova_control_set_state(nova_control_t *control, nova_control_state_t state);
bool nova_control_set_interaction(nova_control_t *control,
    nova_interaction_state_t state,bool active,bool pointer_device);
nova_interaction_state_t nova_control_interaction(const nova_control_t *control);
bool nova_control_set_bounds(nova_control_t *control, nova_rect_t bounds);
bool nova_control_set_text(nova_control_t *control, const char *text);
bool nova_text_field_set_placeholder(nova_control_t *control, const char *placeholder);
bool nova_text_field_set_mode(nova_control_t *control, nova_text_input_mode_t mode);
bool nova_text_field_set_maximum(nova_control_t *control, uint16_t maximum_length);
bool nova_text_field_insert(nova_control_t *control, uint32_t codepoint);
bool nova_text_field_backspace(nova_control_t *control);
bool nova_text_field_delete(nova_control_t *control);
bool nova_text_field_move(nova_control_t *control, int16_t codepoints, bool extend);
bool nova_text_field_select_all(nova_control_t *control);
void nova_text_field_clear(nova_control_t *control);
nova_control_t *nova_password_field_create(void);
bool nova_password_field_set_placeholder(nova_control_t *field,const char *placeholder);
bool nova_password_field_enable_reveal(nova_control_t *field,bool enabled);
bool nova_password_field_set_revealed(nova_control_t *field,bool revealed,
                                      uint32_t duration_ms);
bool nova_password_field_tick(nova_control_t *field,uint32_t elapsed_ms);
bool nova_password_field_validate(nova_control_t *field,uint16_t minimum_length,
    bool require_uppercase,bool require_lowercase,bool require_digit);
void nova_password_field_clear(nova_control_t *field);
bool nova_password_field_empty(const nova_control_t *field);
const nova_password_diagnostics_t *nova_password_field_diagnostics(
    const nova_control_t *field);
bool nova_control_set_accessibility(nova_control_t *control, uint16_t role,
                                    const char *name, bool decorative);
bool nova_control_set_range(nova_control_t *control, int32_t minimum,
                            int32_t maximum, int32_t value);
bool nova_control_set_step(nova_control_t *control, int32_t step);
bool nova_control_set_value(nova_control_t *control, int32_t value);
bool nova_control_adjust(nova_control_t *control, int32_t steps);
bool nova_control_set_flags(nova_control_t *control, uint32_t flags);
bool nova_control_set_checked(nova_control_t *control, bool checked);
bool nova_control_toggle(nova_control_t *control);
bool nova_activity_start(nova_control_t *control);
bool nova_activity_stop(nova_control_t *control);
bool nova_activity_set_style(nova_control_t *control,nova_activity_style_t style);
bool nova_activity_set_phase(nova_control_t *control,uint16_t phase_per_mille);
bool nova_activity_running(const nova_control_t *control);
bool nova_scroll_view_configure(nova_control_t *view,int32_t viewport_width,
    int32_t viewport_height,int32_t content_width,int32_t content_height);
bool nova_scroll_view_scroll_to(nova_control_t *view,int32_t x,int32_t y);
bool nova_scroll_view_scroll_by(nova_control_t *view,int32_t dx,int32_t dy);
bool nova_scroll_view_scroll_into_view(nova_control_t *view,const nova_rect_t *child);
bool nova_scrollbar_attach(nova_control_t *scrollbar,nova_control_t *view,
    nova_scrollbar_orientation_t orientation);
bool nova_card_set_type(nova_control_t *card,nova_card_type_t type);
bool nova_card_add_child(nova_control_t *card,nova_control_t *child);
bool nova_tile_set_type(nova_control_t *tile,nova_boot_tile_type_t type);
bool nova_tile_set_description(nova_control_t *tile,const char *description);
bool nova_tile_set_status(nova_control_t *tile,const char *status);
bool nova_icon_button_set_icon(nova_control_t *button,uint16_t icon_id);
bool nova_icon_button_set_tooltip(nova_control_t *button,const char *tooltip);
bool nova_icon_button_set_action(nova_control_t *button,uint32_t action);
bool nova_button_set_type(nova_control_t *button,nova_button_type_t type);
bool nova_button_set_icon(nova_control_t *button,uint16_t icon_id);
bool nova_button_set_action(nova_control_t *button,uint32_t action);
bool nova_button_pressed(const nova_control_t *button);
bool nova_menu_button_bind(nova_control_t *button,nova_control_t *menu);
bool nova_menu_button_open(nova_control_t *button);
bool nova_menu_button_close(nova_control_t *button);
bool nova_menu_button_expanded(const nova_control_t *button);
bool nova_label_set_type(nova_control_t *label,nova_label_type_t type);
bool nova_label_set_alignment(nova_control_t *label,nova_control_alignment_t alignment);
bool nova_label_set_scale(nova_control_t *label,uint16_t scale_milli);
const char *nova_label_get_text(const nova_control_t *label);
bool nova_icon_control_set(nova_control_t *icon,uint16_t icon_id);
uint16_t nova_icon_control_get(const nova_control_t *icon);
bool nova_image_set_resource(nova_control_t *image,nova_image_resource_t resource);
bool nova_image_set_scaling(nova_control_t *image,nova_image_scaling_mode_t mode);
bool nova_image_set_tint(nova_control_t *image,uint32_t color);
bool nova_separator_set_orientation(nova_control_t *separator,
                                    nova_separator_orientation_t orientation);
bool nova_separator_set_title(nova_control_t *separator,const char *title);
bool nova_list_add_item(nova_control_t *list,nova_control_t *item);
bool nova_list_remove_item(nova_control_t *list,uint16_t index);
bool nova_list_set_selection_mode(nova_control_t *list,nova_list_selection_mode_t mode);
bool nova_list_select(nova_control_t *list,uint16_t index);
uint16_t nova_list_selected_index(const nova_control_t *list);
uint16_t nova_list_count(const nova_control_t *list);
bool nova_list_set_virtual_window(nova_control_t *list,uint16_t first,uint16_t count);
bool nova_list_item_visible(const nova_control_t *list,uint16_t index);
bool nova_list_item_set_subtitle(nova_control_t *item,const char *subtitle);
bool nova_list_item_set_status(nova_control_t *item,const char *status);
bool nova_list_item_set_state(nova_control_t *item,nova_list_item_state_t state);
bool nova_status_badge_set_type(nova_control_t *badge,nova_badge_type_t type);
bool nova_status_badge_set_icon(nova_control_t *badge,uint16_t icon_id);
bool nova_status_badge_set_visible(nova_control_t *badge,bool visible);
bool nova_style_define(uint16_t id,uint16_t parent,const nova_control_style_t *style,
    uint32_t override_mask,nova_control_material_t material);
const nova_style_descriptor_t *nova_style_get(uint16_t id);
bool nova_style_apply(nova_control_t *control,uint16_t style_id);
void nova_style_theme_changed(void);
bool nova_control_template_define(uint16_t id,uint16_t parent,
    nova_control_type_t control_type,uint32_t parts,uint32_t required_parts,
    uint16_t visual_nodes);
const nova_control_template_t *nova_control_template_get(uint16_t id);
bool nova_control_template_apply(nova_control_t *control,uint16_t template_id);
bool nova_control_template_has_part(const nova_control_template_t *templ,
                                    const char *part_name);
bool nova_control_set_style(nova_control_t *control,
                            const nova_control_style_t *style);
bool nova_control_invalidate(nova_control_t *control);
bool nova_control_focus(nova_control_t *control);
bool nova_control_focusable(const nova_control_t *control);
nova_control_t *nova_control_get(uint16_t id);
nova_control_t *nova_control_hit_test(int32_t x, int32_t y);
bool nova_control_invoke(nova_control_t *control, uint32_t *action);
void nova_control_release(nova_control_t *control);
void nova_control_render(nova_control_t *control, nova_surface_t *surface);
bool nova_controls_sync_scene(void);
const nova_control_diagnostics_t *nova_control_diagnostics(void);
bool nova_control_test_initialize(void);
bool nova_control_test_execute(uint32_t test_case);
const nova_control_test_result_t *nova_control_test_results(void);
const nova_control_test_summary_t *nova_control_test_summary(void);
bool nova_control_test_generate_report(bool authorized,uint8_t *output,
                                       uint32_t capacity,uint32_t *written);

#endif
