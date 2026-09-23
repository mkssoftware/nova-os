#ifndef NOVA_UI_DESIGN_H
#define NOVA_UI_DESIGN_H

#include "nova/ui/runtime.h"

typedef enum {
    NOVA_DESIGN_DESKTOP = 0,
    NOVA_DESIGN_ACRYLIC,
    NOVA_DESIGN_SURFACE,
    NOVA_DESIGN_SURFACE_RAISED,
    NOVA_DESIGN_SURFACE_HOVER,
    NOVA_DESIGN_BORDER,
    NOVA_DESIGN_BORDER_ACTIVE,
    NOVA_DESIGN_TEXT,
    NOVA_DESIGN_TEXT_SECONDARY,
    NOVA_DESIGN_TEXT_DISABLED,
    NOVA_DESIGN_BLUE,
    NOVA_DESIGN_PURPLE,
    NOVA_DESIGN_CYAN,
    NOVA_DESIGN_MAGENTA,
    NOVA_DESIGN_SUCCESS,
    NOVA_DESIGN_WARNING,
    NOVA_DESIGN_DANGER,
    NOVA_DESIGN_INFORMATION,
    NOVA_DESIGN_COLOR_COUNT
} nova_design_color_token_t;

typedef enum {
    NOVA_DESIGN_RADIUS_SMALL = 0,
    NOVA_DESIGN_RADIUS_MEDIUM,
    NOVA_DESIGN_RADIUS_LARGE,
    NOVA_DESIGN_RADIUS_WINDOW,
    NOVA_DESIGN_RADIUS_TASKBAR,
    NOVA_DESIGN_RADIUS_START_MENU,
    NOVA_DESIGN_RADIUS_COUNT
} nova_design_radius_token_t;

typedef struct {
    nova_ui_color_t colors[NOVA_DESIGN_COLOR_COUNT];
    nova_ui_dlu_t radii[NOVA_DESIGN_RADIUS_COUNT];
    nova_ui_dlu_t spacing_unit;
    nova_ui_dlu_t control_height;
    nova_ui_dlu_t taskbar_height;
    nova_ui_dlu_t window_border;
    uint16_t acrylic_opacity_milli;
    uint16_t acrylic_blur_dlu;
    uint16_t window_shadow_opacity_milli;
    uint16_t transition_fast_ms;
    uint16_t transition_normal_ms;
    bool subtle_glow;
} nova_design_system_t;

void nova_design_system_init(nova_design_system_t *design);
bool nova_design_system_valid(const nova_design_system_t *design);
nova_ui_color_t nova_design_color(const nova_design_system_t *design,
                                  nova_design_color_token_t token);
nova_ui_dlu_t nova_design_radius(const nova_design_system_t *design,
                                 nova_design_radius_token_t token);

#endif
