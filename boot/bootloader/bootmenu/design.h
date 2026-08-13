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
typedef enum {
    NOVA_TYPOGRAPHY_DISPLAY,NOVA_TYPOGRAPHY_HEADLINE,NOVA_TYPOGRAPHY_TITLE,
    NOVA_TYPOGRAPHY_SUBTITLE,NOVA_TYPOGRAPHY_BODY,NOVA_TYPOGRAPHY_BODY_SMALL,
    NOVA_TYPOGRAPHY_CAPTION,NOVA_TYPOGRAPHY_BUTTON,NOVA_TYPOGRAPHY_MENU,
    NOVA_TYPOGRAPHY_STATUS,NOVA_TYPOGRAPHY_MONOSPACE,NOVA_TYPOGRAPHY_COUNT
} nova_typography_role_t;
typedef enum {NOVA_TRACKING_CONDENSED,NOVA_TRACKING_STANDARD,NOVA_TRACKING_EXPANDED,
    NOVA_TRACKING_COUNT} nova_tracking_t;
typedef enum {NOVA_TEXT_ALIGN_LEFT,NOVA_TEXT_ALIGN_CENTER,NOVA_TEXT_ALIGN_RIGHT,
    NOVA_TEXT_ALIGN_COUNT} nova_text_alignment_token_t;
typedef struct {
    uint64_t font_id;
    uint16_t size_dlu,line_min_dlu,line_dlu,line_max_dlu,weight;
    int16_t tracking_milli;
    nova_tracking_t tracking;
} nova_typography_style_t;
typedef enum {NOVA_SPACING_XXS,NOVA_SPACING_XS,NOVA_SPACING_S,NOVA_SPACING_M,
    NOVA_SPACING_L,NOVA_SPACING_XL,NOVA_SPACING_XXL,NOVA_SPACING_XXXL,
    NOVA_SPACING_COUNT} nova_spacing_token_t;
typedef enum {NOVA_RADIUS_NONE,NOVA_RADIUS_TINY,NOVA_RADIUS_SMALL,NOVA_RADIUS_MEDIUM,
    NOVA_RADIUS_LARGE,NOVA_RADIUS_XLARGE,NOVA_RADIUS_PILL,NOVA_RADIUS_CIRCLE,
    NOVA_RADIUS_COUNT} nova_radius_token_t;
typedef enum {NOVA_ELEVATION_NONE,NOVA_ELEVATION_LEVEL1,NOVA_ELEVATION_LEVEL2,
    NOVA_ELEVATION_LEVEL3,NOVA_ELEVATION_LEVEL4,NOVA_ELEVATION_LEVEL5,
    NOVA_ELEVATION_COUNT} nova_elevation_token_t;
typedef struct {int16_t x_dlu,y_dlu;uint16_t blur_dlu;uint8_t opacity;uint32_t color;}
    nova_shadow_layer_t;
typedef struct {nova_shadow_layer_t layers[2];uint8_t layer_count;bool disabled;}
    nova_shadow_style_t;
typedef enum {NOVA_DESIGN_MATERIAL_NONE,NOVA_DESIGN_MATERIAL_SURFACE,
    NOVA_DESIGN_MATERIAL_GLASS,NOVA_DESIGN_MATERIAL_GLASS_LIGHT,
    NOVA_DESIGN_MATERIAL_GLASS_MEDIUM,NOVA_DESIGN_MATERIAL_GLASS_STRONG,
    NOVA_DESIGN_MATERIAL_OVERLAY,NOVA_DESIGN_MATERIAL_DIALOG,
    NOVA_DESIGN_MATERIAL_COUNT} nova_material_token_t;
typedef struct {uint32_t surface,border;uint16_t blur_dlu;uint8_t opacity,reflection,
    brightness;nova_elevation_token_t elevation;bool opaque;} nova_material_style_t;
typedef enum {NOVA_DURATION_INSTANT,NOVA_DURATION_ULTRA_FAST,NOVA_DURATION_FAST,
    NOVA_DURATION_NORMAL,NOVA_DURATION_SLOW,NOVA_DURATION_VERY_SLOW,
    NOVA_DURATION_COUNT} nova_duration_token_t;
typedef enum {NOVA_DESIGN_EASE_LINEAR,NOVA_DESIGN_EASE_IN,NOVA_DESIGN_EASE_OUT,
    NOVA_DESIGN_EASE_IN_OUT,NOVA_DESIGN_EASE_DECELERATE,NOVA_DESIGN_EASE_ACCELERATE,
    NOVA_DESIGN_EASE_SPRING,NOVA_DESIGN_EASE_OVERSHOOT,NOVA_DESIGN_EASE_COUNT}
    nova_design_easing_token_t;
typedef enum {NOVA_TRANSITION_PAGE,NOVA_TRANSITION_DIALOG,NOVA_TRANSITION_MENU,
    NOVA_TRANSITION_TOOLTIP,NOVA_TRANSITION_CARD,NOVA_TRANSITION_FOCUS,
    NOVA_TRANSITION_SELECTION,NOVA_TRANSITION_COUNT} nova_transition_token_t;
typedef struct {uint16_t duration_ms;nova_design_easing_token_t easing;
    uint8_t emphasis,priority;bool disabled;} nova_motion_style_t;
typedef enum {NOVA_INTERACTION_NORMAL,NOVA_INTERACTION_HOVER,NOVA_INTERACTION_FOCUSED,
    NOVA_INTERACTION_PRESSED,NOVA_INTERACTION_SELECTED,NOVA_INTERACTION_CHECKED,
    NOVA_INTERACTION_EXPANDED,NOVA_INTERACTION_COLLAPSED,NOVA_INTERACTION_DISABLED,
    NOVA_INTERACTION_READONLY,NOVA_INTERACTION_LOADING,NOVA_INTERACTION_ERROR,
    NOVA_INTERACTION_COUNT} nova_interaction_state_t;
typedef struct {uint32_t background,foreground,border;uint8_t opacity,cursor;
    nova_elevation_token_t elevation;nova_radius_token_t radius;
    nova_transition_token_t motion;bool indicator_required,input_blocked;} nova_state_style_t;
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
    uint32_t design_language_version;
    uint64_t theme_id;
    uint32_t registry_signature;
    uint16_t typography_count,spacing_count,radius_count,icon_count;
    uint16_t material_count,motion_count,interaction_count;
    bool visual_continuity_enabled,accessibility_semantics_preserved;
} nova_visual_continuity_context_t;
typedef struct {
    uint32_t design_language_version,registry_signature;
    uint16_t typography_count,spacing_count,radius_count,icon_count;
    uint16_t material_count,motion_count,interaction_count;
    bool semantic_tokens_only;
} nova_component_descriptor_t;
typedef struct {uint32_t validations,reloads,violations;bool consistent;}
    nova_visual_continuity_diagnostics_t;
typedef struct {
    uint32_t initializations,validations,version_errors,resource_errors;
    uint32_t checksum_errors,oem_applies,oem_rejections;
    uint32_t typography_lookups,spacing_lookups,radius_lookups,token_reloads;
    uint32_t shadow_lookups,material_lookups,motion_lookups;
    uint32_t state_lookups,state_transitions,state_rejections;
    uint32_t token_validation_errors;
    bool compatible;
} nova_design_diagnostics_t;

bool nova_design_initialize(void);
bool nova_design_validate_manifest(const nova_design_manifest_t *manifest);
bool nova_design_validate_resources(void);
bool nova_design_apply_oem(const nova_oem_design_t *oem);
const nova_visual_continuity_context_t *nova_visual_continuity_current(void);
bool nova_visual_continuity_validate(const nova_component_descriptor_t *component);
bool nova_visual_continuity_reload(void);
const nova_visual_continuity_diagnostics_t *nova_visual_continuity_diagnostics(void);
const nova_design_manifest_t *nova_design_manifest(void);
const nova_typography_tokens_t *nova_design_typography(void);
const nova_typography_style_t *nova_typography_get(nova_typography_role_t role);
bool nova_typography_exists(nova_typography_role_t role);
uint16_t nova_spacing_get(nova_spacing_token_t token);
bool nova_spacing_exists(nova_spacing_token_t token);
uint16_t nova_radius_get(nova_radius_token_t token);
bool nova_radius_exists(nova_radius_token_t token);
bool nova_design_tokens_reload(void);
const nova_shadow_style_t *nova_shadow_get(nova_elevation_token_t token);
bool nova_shadow_exists(nova_elevation_token_t token);
const nova_material_style_t *nova_material_get(nova_material_token_t token);
bool nova_material_exists(nova_material_token_t token);
uint16_t nova_duration_get(nova_duration_token_t token);
const nova_motion_style_t *nova_design_motion_get(nova_transition_token_t token);
bool nova_design_motion_exists(nova_transition_token_t token);
const nova_state_style_t *nova_state_get(nova_interaction_state_t state);
bool nova_state_exists(nova_interaction_state_t state);
bool nova_state_transition_allowed(nova_interaction_state_t current,
                                   nova_interaction_state_t next);
const nova_component_tokens_t *nova_design_components(void);
const nova_design_motion_tokens_t *nova_design_motion(void);
const nova_effect_tokens_t *nova_design_effects(void);
const nova_oem_design_t *nova_design_oem(void);
uint32_t nova_design_accent(void);
const nova_theme_tokens_t *nova_design_tokens(void);
const nova_design_diagnostics_t *nova_design_diagnostics(void);

#endif
