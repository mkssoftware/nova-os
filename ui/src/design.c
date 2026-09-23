#include "nova/ui/design.h"

#include <string.h>

static nova_ui_color_t rgba(uint8_t red, uint8_t green, uint8_t blue,
                            uint8_t alpha) {
    return (nova_ui_color_t){red, green, blue, alpha};
}

void nova_design_system_init(nova_design_system_t *design) {
    if (design == NULL) return;
    memset(design, 0, sizeof(*design));
    design->colors[NOVA_DESIGN_DESKTOP] = rgba(2, 6, 18, 255);
    design->colors[NOVA_DESIGN_ACRYLIC] = rgba(5, 12, 28, 214);
    design->colors[NOVA_DESIGN_SURFACE] = rgba(7, 16, 35, 238);
    design->colors[NOVA_DESIGN_SURFACE_RAISED] = rgba(12, 24, 48, 242);
    design->colors[NOVA_DESIGN_SURFACE_HOVER] = rgba(20, 35, 67, 235);
    design->colors[NOVA_DESIGN_BORDER] = rgba(130, 160, 255, 41);
    design->colors[NOVA_DESIGN_BORDER_ACTIVE] = rgba(78, 145, 255, 148);
    design->colors[NOVA_DESIGN_TEXT] = rgba(241, 245, 255, 255);
    design->colors[NOVA_DESIGN_TEXT_SECONDARY] = rgba(166, 178, 204, 255);
    design->colors[NOVA_DESIGN_TEXT_DISABLED] = rgba(92, 104, 132, 255);
    design->colors[NOVA_DESIGN_BLUE] = rgba(35, 126, 255, 255);
    design->colors[NOVA_DESIGN_PURPLE] = rgba(132, 72, 255, 255);
    design->colors[NOVA_DESIGN_CYAN] = rgba(42, 211, 238, 255);
    design->colors[NOVA_DESIGN_MAGENTA] = rgba(216, 74, 255, 255);
    design->colors[NOVA_DESIGN_SUCCESS] = rgba(78, 207, 118, 255);
    design->colors[NOVA_DESIGN_WARNING] = rgba(245, 174, 66, 255);
    design->colors[NOVA_DESIGN_DANGER] = rgba(239, 74, 93, 255);
    design->colors[NOVA_DESIGN_INFORMATION] = rgba(78, 161, 255, 255);
    design->radii[NOVA_DESIGN_RADIUS_SMALL] = 7;
    design->radii[NOVA_DESIGN_RADIUS_MEDIUM] = 10;
    design->radii[NOVA_DESIGN_RADIUS_LARGE] = 14;
    design->radii[NOVA_DESIGN_RADIUS_WINDOW] = 20;
    design->radii[NOVA_DESIGN_RADIUS_TASKBAR] = 26;
    design->radii[NOVA_DESIGN_RADIUS_START_MENU] = 28;
    design->spacing_unit = 4;
    design->control_height = 36;
    design->taskbar_height = 76;
    design->window_border = 1;
    design->acrylic_opacity_milli = 840;
    design->acrylic_blur_dlu = 20;
    design->window_shadow_opacity_milli = 420;
    design->transition_fast_ms = 140;
    design->transition_normal_ms = 190;
    design->subtle_glow = true;
}

bool nova_design_system_valid(const nova_design_system_t *design) {
    size_t index;
    if (design == NULL || design->spacing_unit <= 0 ||
        design->control_height < 32 || design->taskbar_height < 64 ||
        design->acrylic_opacity_milli > 1000u ||
        design->transition_normal_ms < 120u ||
        design->transition_normal_ms > 250u)
        return false;
    for (index = 0u; index < NOVA_DESIGN_RADIUS_COUNT; ++index)
        if (design->radii[index] <= 0) return false;
    return true;
}

nova_ui_color_t nova_design_color(const nova_design_system_t *design,
                                  nova_design_color_token_t token) {
    if (design == NULL || token >= NOVA_DESIGN_COLOR_COUNT)
        return (nova_ui_color_t){0u, 0u, 0u, 0u};
    return design->colors[token];
}

nova_ui_dlu_t nova_design_radius(const nova_design_system_t *design,
                                 nova_design_radius_token_t token) {
    if (design == NULL || token >= NOVA_DESIGN_RADIUS_COUNT) return 0;
    return design->radii[token];
}
