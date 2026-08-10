#ifndef NOVA_BOOT_RENDER_QUALITY_H
#define NOVA_BOOT_RENDER_QUALITY_H

#include <stdbool.h>
#include <stdint.h>
#include "background_blur.h"
#include "effects.h"
#include "image_renderer.h"
#include "rounded_geometry.h"

typedef enum {
    NOVA_RENDER_QUALITY_ULTRA,
    NOVA_RENDER_QUALITY_HIGH,
    NOVA_RENDER_QUALITY_STANDARD,
    NOVA_RENDER_QUALITY_PERFORMANCE,
    NOVA_RENDER_QUALITY_SAFE,
    NOVA_RENDER_QUALITY_COUNT
} nova_render_quality_t;

typedef enum {
    NOVA_QUALITY_REASON_INITIALIZATION,
    NOVA_QUALITY_REASON_USER,
    NOVA_QUALITY_REASON_HARDWARE,
    NOVA_QUALITY_REASON_MEMORY,
    NOVA_QUALITY_REASON_FRAME_BUDGET,
    NOVA_QUALITY_REASON_GRAPHICS_ERROR,
    NOVA_QUALITY_REASON_SAFE_MODE
} nova_quality_change_reason_t;

typedef struct {
    nova_blur_quality_t blur_quality;
    nova_effect_quality_t effect_quality;
    nova_rounded_aa_t anti_aliasing;
    nova_image_sampling_t image_sampling;
    uint32_t memory_budget;
    uint16_t maximum_blur_radius;
    uint8_t effect_density;
    uint8_t compositor_fallback;
    bool blur_enabled,shadows_enabled,glows_enabled,transparency_enabled;
    bool animations_enabled,dithering_enabled;
} nova_render_quality_parameters_t;

typedef struct {
    uint32_t initializations,applications,switches,automatic_adjustments;
    uint32_t rejected_profiles,full_redraws,cache_invalidations;
    uint32_t frame_budget_violations,memory_degradations,graphics_degradations;
    uint32_t disabled_effects,last_frame_us;
    uint64_t observed_memory;
    nova_render_quality_t active,requested;
    nova_quality_change_reason_t last_reason;
    bool initialized,automatic,software_renderer;
} nova_render_quality_diagnostics_t;

bool nova_render_quality_initialize(bool software_renderer,uint64_t available_memory);
bool nova_render_quality_set(nova_render_quality_t profile);
bool nova_render_quality_set_auto(uint64_t available_memory,bool firmware_limited,
                                  bool software_renderer);
nova_render_quality_t nova_render_quality_get(void);
bool nova_render_quality_is_supported(nova_render_quality_t profile);
bool nova_render_quality_apply(void);
void nova_render_quality_report_frame(uint32_t frame_time_us);
void nova_render_quality_report_memory(uint64_t available_memory);
void nova_render_quality_report_graphics_error(bool critical);
const nova_render_quality_parameters_t *nova_render_quality_parameters(void);
const nova_render_quality_diagnostics_t *nova_render_quality_diagnostics(void);

#endif
