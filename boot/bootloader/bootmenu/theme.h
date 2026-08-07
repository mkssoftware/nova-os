#ifndef NOVA_BOOT_THEME_H
#define NOVA_BOOT_THEME_H

#include <stdint.h>
#include <stdbool.h>

typedef enum { NOVA_THEME_DARK, NOVA_THEME_LIGHT, NOVA_THEME_HIGH_CONTRAST,
               NOVA_THEME_COUNT } nova_theme_id_t;
typedef struct {
    uint32_t background, surface, surface_secondary, text_primary,
             text_secondary, accent, focus, selected, border, disabled,
             success, warning, error, information;
    uint16_t radius_small, radius_medium, spacing_small, spacing_medium;
    uint8_t overlay_opacity;
    bool opaque_materials, reduced_motion, high_contrast;
} nova_theme_tokens_t;
typedef struct {
    uint32_t activations, validation_errors, accessibility_overrides;
    nova_theme_id_t active;
} nova_theme_diagnostics_t;

bool nova_theme_initialize(void);
bool nova_theme_activate(nova_theme_id_t id);
nova_theme_id_t nova_theme_active(void);
const nova_theme_tokens_t *nova_theme_tokens(void);
bool nova_theme_validate(nova_theme_id_t id);
void nova_theme_set_reduced_motion(bool enabled);
bool nova_theme_reduced_motion(void);
const nova_theme_diagnostics_t *nova_theme_diagnostics(void);

#endif
