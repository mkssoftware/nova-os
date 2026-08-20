#ifndef NOVA_BOOT_RESOLUTION_H
#define NOVA_BOOT_RESOLUTION_H

#include <stdbool.h>
#include <stdint.h>
#include "compositor.h"

typedef enum {
    NOVA_RESOLUTION_XS,NOVA_RESOLUTION_S,NOVA_RESOLUTION_M,NOVA_RESOLUTION_L,
    NOVA_RESOLUTION_XL,NOVA_RESOLUTION_XXL,NOVA_RESOLUTION_ULTRA
} nova_resolution_class_t;
typedef enum {
    NOVA_ASPECT_4_3,NOVA_ASPECT_5_4,NOVA_ASPECT_16_9,NOVA_ASPECT_16_10,
    NOVA_ASPECT_21_9,NOVA_ASPECT_32_9,NOVA_ASPECT_CUSTOM
} nova_aspect_ratio_t;

typedef struct {
    uint32_t physical_width,physical_height;
    int32_t logical_width_dlu,logical_height_dlu;
    uint32_t dpi,scale_milli,automatic_scale,user_scale_milli;
    nova_rect_t safe_pixels,safe_dlu;
    nova_resolution_class_t resolution_class;
    nova_aspect_ratio_t aspect_ratio;
    bool dpi_reliable,automatic;
} nova_viewport_t;

typedef struct {
    uint32_t initializations,recalculations,resolution_changes,scale_changes;
    uint32_t dpi_changes,invalid_resolutions,invalid_scales,fallbacks,conversions;
    uint32_t generation;
} nova_resolution_diagnostics_t;
typedef enum {NOVA_DISPLAY_UEFI_GOP,NOVA_DISPLAY_BIOS_VBE,NOVA_DISPLAY_VIRTUAL,
    NOVA_DISPLAY_REMOTE} nova_display_kind_t;
typedef struct {int32_t left,top,right,bottom;} nova_safe_insets_t;
typedef struct {nova_display_kind_t display_kind;nova_safe_insets_t firmware_pixels;
    uint16_t left_permille,top_permille,right_permille,bottom_permille;
    bool reserve_enabled,overscan,accessibility;} nova_safe_area_policy_t;
typedef struct {uint32_t initializations,updates,invalidations,resolution_changes,
    overscan_changes,display_changes,accessibility_changes,contains_checks,
    rejected_rects,fallbacks,errors;uint32_t width,height,dpi,scale_milli;
    nova_safe_insets_t effective_pixels;nova_display_kind_t display_kind;
    bool overscan,accessibility,fallback_active;} nova_safe_area_diagnostics_t;
typedef struct {uint32_t dpi_x,dpi_y,global_scale_milli,accessibility_scale_milli,
    effective_scale_milli,generation;bool dpi_reliable,fallback_active;} nova_scaling_info_t;
typedef struct {uint32_t initializations,updates,invalidations,dpi_updates,
    scale_updates,accessibility_updates,conversions,interval_snaps,
    rounding_events,overflows,fallbacks,errors;} nova_scaling_diagnostics_t;
#define NOVA_RESOLUTION_TEST_CAPACITY 80u
typedef struct {uint32_t width,height,scale_percent;nova_aspect_ratio_t aspect_ratio;
    nova_resolution_class_t resolution_class;int32_t logical_width_dlu,logical_height_dlu;
    bool layout_valid,rendering_valid,controls_reachable,dialog_visible,text_readable,
         safe_area_valid;uint32_t detected_errors;}
    nova_resolution_test_result_t;
typedef struct {uint16_t count;uint32_t executed,passed,failed,reports;
    bool initialized,isolated,deterministic,configuration_unchanged;}
    nova_resolution_test_status_t;

void nova_resolution_initialize(void);
bool nova_resolution_configure(uint32_t width,uint32_t height,uint32_t dpi,
                               bool dpi_reliable);
bool nova_resolution_set_resolution(uint32_t width,uint32_t height);
bool nova_resolution_set_dpi(uint32_t dpi,bool reliable);
bool nova_resolution_set_scale(uint32_t scale_milli);
bool nova_resolution_set_automatic(void);
bool nova_safe_area_initialize(void);
bool nova_safe_area_update(const nova_safe_area_policy_t *policy);
const nova_rect_t *nova_safe_area_get(void);
bool nova_safe_area_contains(nova_rect_t rect);
const nova_safe_area_policy_t *nova_safe_area_policy(void);
const nova_safe_area_diagnostics_t *nova_safe_area_diagnostics(void);
int32_t nova_logical_to_pixel(int32_t dlu);
int32_t nova_pixel_to_logical(int32_t pixel);
bool nova_scaling_initialize(void);
bool nova_scaling_update(bool accessibility);
uint32_t nova_scaling_effective(uint32_t global_scale_milli,bool accessibility);
int32_t nova_scale_to_pixels(int32_t logical_value);
bool nova_scale_interval(int32_t start_dlu,int32_t extent_dlu,
    int32_t *pixel_start,int32_t *pixel_extent);
const nova_scaling_info_t *nova_scaling_get(void);
const nova_scaling_diagnostics_t *nova_scaling_diagnostics(void);
const nova_viewport_t *nova_resolution_viewport(void);
const nova_resolution_diagnostics_t *nova_resolution_diagnostics(void);
bool nova_resolution_test_initialize(void);
bool nova_resolution_test_execute(uint32_t width,uint32_t height,uint32_t scale_percent);
const nova_resolution_test_result_t *nova_resolution_test_results(void);
const nova_resolution_test_status_t *nova_resolution_test_status(void);
bool nova_resolution_test_generate_report(bool authorized,uint8_t *output,
    uint32_t capacity,uint32_t *written);

#endif
