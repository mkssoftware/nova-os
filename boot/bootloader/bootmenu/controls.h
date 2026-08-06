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

enum {
    NOVA_CONTROL_FLAG_VISIBLE = 1u, NOVA_CONTROL_FLAG_ENABLED = 2u,
    NOVA_CONTROL_FLAG_FOCUSED = 4u, NOVA_CONTROL_FLAG_DIRTY = 8u,
    NOVA_CONTROL_FLAG_HOVERED = 16u, NOVA_CONTROL_FLAG_PRESSED = 32u,
    NOVA_CONTROL_FLAG_SELECTED = 64u, NOVA_CONTROL_FLAG_MODAL = 128u,
    NOVA_CONTROL_FLAG_PASSWORD = 256u, NOVA_CONTROL_FLAG_DECORATIVE = 512u
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
    int32_t value, minimum, maximum;
    uint32_t action;
    uint16_t template_id;
    uint16_t accessibility_role;
    bool action_fired;
    char text[NOVA_CONTROL_TEXT_CAPACITY];
    char accessibility_name[NOVA_CONTROL_TEXT_CAPACITY];
} nova_control_t;

typedef struct {
    uint16_t active, focused, created, destroyed;
    uint32_t invalid_transitions, rejected_actions, hit_tests;
} nova_control_diagnostics_t;

void nova_controls_initialize(const nova_control_style_t *base_style);
nova_control_t *nova_control_create(nova_control_type_t type);
bool nova_control_destroy(nova_control_t *control);
bool nova_control_set_parent(nova_control_t *child, nova_control_t *parent);
bool nova_control_set_state(nova_control_t *control, nova_control_state_t state);
bool nova_control_set_bounds(nova_control_t *control, nova_rect_t bounds);
bool nova_control_set_text(nova_control_t *control, const char *text);
bool nova_control_set_accessibility(nova_control_t *control, uint16_t role,
                                    const char *name, bool decorative);
bool nova_control_set_range(nova_control_t *control, int32_t minimum,
                            int32_t maximum, int32_t value);
bool nova_control_focus(nova_control_t *control);
nova_control_t *nova_control_hit_test(int32_t x, int32_t y);
bool nova_control_invoke(nova_control_t *control, uint32_t *action);
void nova_control_release(nova_control_t *control);
void nova_control_render(nova_control_t *control, nova_surface_t *surface);
const nova_control_diagnostics_t *nova_control_diagnostics(void);

#endif
