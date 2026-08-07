#ifndef NOVA_BOOT_CONTROLS_H
#define NOVA_BOOT_CONTROLS_H

#include <stdint.h>
#include <stdbool.h>
#include "compositor.h"

#define NOVA_CONTROL_CAPACITY 128u
#define NOVA_CONTROL_TEXT_CAPACITY 96u
#define NOVA_CONTROL_NONE 0xffffu

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

enum {
    NOVA_CONTROL_FLAG_VISIBLE = 1u, NOVA_CONTROL_FLAG_ENABLED = 2u,
    NOVA_CONTROL_FLAG_FOCUSED = 4u, NOVA_CONTROL_FLAG_DIRTY = 8u,
    NOVA_CONTROL_FLAG_HOVERED = 16u, NOVA_CONTROL_FLAG_PRESSED = 32u,
    NOVA_CONTROL_FLAG_SELECTED = 64u, NOVA_CONTROL_FLAG_MODAL = 128u,
    NOVA_CONTROL_FLAG_PASSWORD = 256u, NOVA_CONTROL_FLAG_DECORATIVE = 512u
    , NOVA_CONTROL_FLAG_CHECKED = 1024u, NOVA_CONTROL_FLAG_INDETERMINATE = 2048u,
    NOVA_CONTROL_FLAG_BUSY = 4096u, NOVA_CONTROL_FLAG_ERROR = 8192u,
    NOVA_CONTROL_FLAG_READONLY = 16384u, NOVA_CONTROL_FLAG_LOCKED = 32768u,
    NOVA_CONTROL_FLAG_EXPANDED = 65536u
};

typedef struct {
    uint32_t background, foreground, accent, disabled;
    uint16_t corner_dlu, border_dlu, padding_dlu;
} nova_control_style_t;

typedef struct {
    uint16_t id, parent, first_child, next_sibling;
    nova_control_type_t type;
    nova_control_state_t state;
    uint32_t flags;
    nova_rect_t bounds;
    nova_control_style_t style;
    int32_t value, minimum, maximum, step;
    int32_t scroll_x,scroll_y,content_width,content_height,viewport_width,viewport_height;
    uint32_t action;
    uint16_t template_id;
    uint16_t accessibility_role;
    bool action_fired;
    char text[NOVA_CONTROL_TEXT_CAPACITY];
    char accessibility_name[NOVA_CONTROL_TEXT_CAPACITY];
    char placeholder[NOVA_CONTROL_TEXT_CAPACITY];
    uint16_t text_length, caret, selection_start, selection_end, maximum_length;
    nova_text_input_mode_t input_mode;
} nova_control_t;

typedef struct {
    uint16_t active, focused, created, destroyed;
    uint32_t invalid_transitions, rejected_actions, hit_tests, value_changes,
             toggles, renders, fallback_renders;
} nova_control_diagnostics_t;

void nova_controls_initialize(const nova_control_style_t *base_style);
nova_control_t *nova_control_create(nova_control_type_t type);
bool nova_control_destroy(nova_control_t *control);
bool nova_control_set_parent(nova_control_t *child, nova_control_t *parent);
bool nova_control_set_state(nova_control_t *control, nova_control_state_t state);
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
bool nova_control_set_style(nova_control_t *control,
                            const nova_control_style_t *style);
bool nova_control_invalidate(nova_control_t *control);
bool nova_control_focus(nova_control_t *control);
nova_control_t *nova_control_hit_test(int32_t x, int32_t y);
bool nova_control_invoke(nova_control_t *control, uint32_t *action);
void nova_control_release(nova_control_t *control);
void nova_control_render(nova_control_t *control, nova_surface_t *surface);
const nova_control_diagnostics_t *nova_control_diagnostics(void);

#endif
