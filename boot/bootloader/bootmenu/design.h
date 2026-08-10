#ifndef NOVA_BOOT_DESIGN_H
#define NOVA_BOOT_DESIGN_H

#include <stdbool.h>
#include <stdint.h>
#include "theme.h"

#define NOVA_DESIGN_VERSION_MAJOR 1u
#define NOVA_DESIGN_VERSION_MINOR 0u
#define NOVA_DESIGN_VERSION_PATCH 0u

typedef struct {uint16_t major,minor,patch;} nova_design_version_t;
typedef struct {
    nova_design_version_t design,tokens,typography,icons,fonts,motion;
    uint32_t checksum;
} nova_design_manifest_t;
typedef struct {
    uint16_t heading_dlu,body_dlu,caption_dlu;
    uint16_t heading_line_dlu,body_line_dlu,caption_line_dlu;
    int16_t heading_tracking_milli,body_tracking_milli,caption_tracking_milli;
    uint16_t regular_weight,semibold_weight;
} nova_typography_tokens_t;
typedef struct {
    uint16_t button_height_dlu,button_radius_dlu,button_padding_dlu;
    uint16_t dialog_radius_dlu,list_item_height_dlu,control_gap_dlu;
    uint16_t focus_border_dlu,icon_standard_dlu;
} nova_component_tokens_t;
typedef struct {
    uint16_t dialog_ms,navigation_ms,recovery_ms,focus_ms,progress_ms;
    uint16_t dialog_scale_from_milli;
    uint8_t dialog_easing,navigation_easing;
} nova_design_motion_tokens_t;
typedef struct {
    uint16_t glass_blur_dlu,glass_edge_dlu,shadow_radius_dlu;
    int16_t shadow_x_dlu,shadow_y_dlu;
    uint8_t glass_opacity,glass_noise,shadow_opacity;
} nova_effect_tokens_t;
typedef struct {
    uint64_t background_resource,logo_resource,animation_resource;
    uint32_t accent;
} nova_oem_design_t;
typedef struct {
    uint32_t initializations,validations,version_errors,resource_errors;
    uint32_t checksum_errors,oem_applies,oem_rejections;
    bool compatible;
} nova_design_diagnostics_t;

bool nova_design_initialize(void);
bool nova_design_validate_manifest(const nova_design_manifest_t *manifest);
bool nova_design_validate_resources(void);
bool nova_design_apply_oem(const nova_oem_design_t *oem);
const nova_design_manifest_t *nova_design_manifest(void);
const nova_typography_tokens_t *nova_design_typography(void);
const nova_component_tokens_t *nova_design_components(void);
const nova_design_motion_tokens_t *nova_design_motion(void);
const nova_effect_tokens_t *nova_design_effects(void);
const nova_oem_design_t *nova_design_oem(void);
uint32_t nova_design_accent(void);
const nova_theme_tokens_t *nova_design_tokens(void);
const nova_design_diagnostics_t *nova_design_diagnostics(void);

#endif
