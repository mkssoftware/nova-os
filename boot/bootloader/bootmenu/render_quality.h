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
    uint8_t svg_raster_percent,glyph_atlas_percent,animation_frame_step,
            icon_raster_percent;
    uint32_t resource_cache_budget;
    uint8_t compositor_fallback;
    bool blur_enabled,shadows_enabled,glows_enabled,transparency_enabled;
    bool animations_enabled,dithering_enabled;
} nova_render_quality_parameters_t;

typedef struct {
    uint32_t frame_time_us,width,height;
    uint64_t available_memory,cache_used,cache_budget;
    uint8_t cpu_score,gpu_load_percent;
    bool gpu_available,software_renderer,firmware_limited;
} nova_quality_telemetry_t;

typedef struct {
    uint32_t initializations,applications,switches,automatic_adjustments;
    uint32_t rejected_profiles,full_redraws,cache_invalidations;
    uint32_t frame_budget_violations,memory_degradations,graphics_degradations;
    uint32_t disabled_effects,last_frame_us;
    uint32_t telemetry_updates,stable_frames,suppressed_spikes,recoveries;
    uint32_t atomic_generation,cache_reconfigurations;
    uint64_t observed_memory;
    nova_render_quality_t active,requested;
    nova_quality_change_reason_t last_reason;
    bool initialized,automatic,software_renderer;
} nova_render_quality_diagnostics_t;

typedef enum {NOVA_BOOT_QUALITY_ULTRA,NOVA_BOOT_QUALITY_HIGH,
    NOVA_BOOT_QUALITY_BALANCED,NOVA_BOOT_QUALITY_LOW,NOVA_BOOT_QUALITY_SAFE,
    NOVA_BOOT_QUALITY_AUTO} nova_boot_quality_t;
typedef struct {
    nova_boot_quality_t current_quality;
    bool automatic,performance_limited,state_preserved,security_preserved;
    nova_quality_change_reason_t last_reason;
    nova_quality_telemetry_t telemetry;
    nova_render_quality_parameters_t parameters;
} nova_boot_quality_status_t;
typedef struct {
    bool enabled,automatic,software_renderer,reduced_effects,reduced_animations;
    bool dirty_regions,double_buffered,event_driven,layout_preserved;
    bool functionality_preserved,security_preserved,runtime_heap_locked;
    uint32_t target_fps,last_fps,last_frame_us,activations,deactivations;
    uint64_t memory_budget,memory_used,cache_used;
} nova_low_end_profile_t;
typedef struct {
    bool enabled,automatic,gpu_backend_available,gpu_acceleration;
    bool advanced_materials,high_quality_shadows,advanced_blur,layer_caching;
    bool dirty_regions,double_buffered,software_fallback_available;
    bool layout_preserved,functionality_preserved,security_preserved;
    bool runtime_heap_locked,fallback_active;
    uint32_t target_fps,last_fps,last_frame_us,activations,deactivations,gpu_failures;
    uint8_t gpu_load_percent;
    uint64_t memory_budget,memory_used,cache_used;
} nova_high_quality_profile_t;

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
bool nova_render_quality_observe(const nova_quality_telemetry_t *telemetry);
bool nova_render_quality_update(void);
bool nova_boot_quality_initialize(void);
bool nova_boot_quality_set(nova_boot_quality_t quality);
const nova_boot_quality_status_t *nova_boot_quality_status(void);
bool nova_boot_quality_update(void);
bool nova_low_end_profile_initialize(void);
bool nova_low_end_profile_detect(uint8_t cpu_cores,uint32_t cpu_mhz,
    uint64_t available_ui_memory,bool gpu_available,bool recovery_or_safe);
bool nova_low_end_profile_enable(void);
bool nova_low_end_profile_disable(void);
bool nova_low_end_profile_update(uint32_t fps,uint32_t frame_time_us,
    uint64_t memory_used,uint64_t cache_used);
const nova_low_end_profile_t *nova_low_end_profile_status(void);
bool nova_high_quality_profile_initialize(void);
void nova_high_quality_profile_set_gpu_backend(bool available);
bool nova_high_quality_profile_detect(uint8_t cpu_cores,uint64_t system_memory,
    uint64_t available_ui_memory,bool gpu_available,bool fast_storage);
bool nova_high_quality_profile_enable(void);
bool nova_high_quality_profile_disable(void);
bool nova_high_quality_profile_report_gpu_failure(void);
bool nova_high_quality_profile_update(uint32_t fps,uint32_t frame_time_us,
    uint64_t memory_used,uint64_t cache_used,uint8_t gpu_load_percent);
const nova_high_quality_profile_t *nova_high_quality_profile_status(void);
const nova_render_quality_parameters_t *nova_render_quality_parameters(void);
const nova_render_quality_diagnostics_t *nova_render_quality_diagnostics(void);

#endif
