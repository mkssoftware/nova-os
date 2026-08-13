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
    uint64_t theme_id,resource_id;
    const char *name,*author;
    uint32_t version,resource_version,references,token_count;
    const nova_theme_tokens_t *tokens;
    bool loaded,valid;
} nova_theme_resource_t;
typedef struct {
    uint64_t theme_id;
    const char *name;
    uint32_t version;
    bool is_default,is_dark;
} nova_theme_descriptor_t;
typedef struct {
    uint64_t theme_id;
    bool accessibility_theme,high_contrast,reduced_motion,opaque_materials;
    uint16_t minimum_focus_dlu;
} nova_accessibility_theme_t;
typedef struct {
    uint32_t activations, validation_errors, accessibility_overrides;
    uint32_t registered,loads,cache_hits,releases,atomic_switches;
    uint32_t missing_resources,duplicate_ids;
    uint32_t token_lookups,contrast_checks;
    nova_theme_id_t active;
} nova_theme_diagnostics_t;

bool nova_theme_initialize(void);
bool nova_theme_activate(nova_theme_id_t id);
const nova_theme_descriptor_t *nova_theme_default(void);
const nova_theme_descriptor_t *nova_theme_light(void);
const nova_accessibility_theme_t *nova_theme_high_contrast(void);
bool nova_theme_is_dark(void);
bool nova_theme_is_light(void);
bool nova_theme_is_high_contrast(void);
bool nova_theme_activate_default(void);
bool nova_theme_activate_light(void);
bool nova_theme_activate_high_contrast(void);
const nova_theme_resource_t *nova_theme_resource_load(nova_theme_id_t id);
const nova_theme_resource_t *nova_theme_resource_find(uint64_t theme_id);
const nova_theme_resource_t *nova_theme_current(void);
bool nova_theme_resource_release(nova_theme_id_t id);
nova_theme_id_t nova_theme_active(void);
const nova_theme_tokens_t *nova_theme_tokens(void);
bool nova_theme_validate(nova_theme_id_t id);
void nova_theme_set_reduced_motion(bool enabled);
bool nova_theme_reduced_motion(void);
const nova_theme_diagnostics_t *nova_theme_diagnostics(void);

#endif
