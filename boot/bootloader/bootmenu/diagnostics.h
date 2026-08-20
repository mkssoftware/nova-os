#ifndef NOVA_BOOT_DIAGNOSTICS_H
#define NOVA_BOOT_DIAGNOSTICS_H

#include <stdint.h>
#include <stdbool.h>
#include "memory.h"

#define NOVA_DIAG_CAPACITY 256u
#define NOVA_DIAG_MODULE_CAPACITY 16u
#define NOVA_DIAG_TEXT_CAPACITY 64u
#define NOVA_RENDER_FRAME_HISTORY 600u
typedef enum { NOVA_DIAG_TRACE, NOVA_DIAG_DEBUG, NOVA_DIAG_INFO,
    NOVA_DIAG_WARNING, NOVA_DIAG_ERROR, NOVA_DIAG_CRITICAL } nova_diag_level_t;
typedef enum { NOVA_QUALITY_ULTRA, NOVA_QUALITY_HIGH, NOVA_QUALITY_STANDARD,
    NOVA_QUALITY_PERFORMANCE, NOVA_QUALITY_SAFE, NOVA_QUALITY_AUTO,
    NOVA_QUALITY_BALANCED=NOVA_QUALITY_STANDARD,
    NOVA_QUALITY_LOW=NOVA_QUALITY_PERFORMANCE } nova_quality_t;
typedef struct {
    uint64_t timestamp_us;
    nova_diag_level_t level;
    uint32_t category, event_id, value_a, value_b;
} nova_diag_event_t;
typedef enum {
    NOVA_DIAG_AREA_BOOT,NOVA_DIAG_AREA_HARDWARE,NOVA_DIAG_AREA_FIRMWARE,
    NOVA_DIAG_AREA_RENDERING,NOVA_DIAG_AREA_MEMORY,NOVA_DIAG_AREA_PERFORMANCE,
    NOVA_DIAG_AREA_RESOURCES,NOVA_DIAG_AREA_ANIMATION,NOVA_DIAG_AREA_INPUT,
    NOVA_DIAG_AREA_NETWORK,NOVA_DIAG_AREA_SECURITY,NOVA_DIAG_AREA_COUNT
} nova_diag_area_t;
typedef enum {
    NOVA_DIAG_PHASE_ANY,NOVA_DIAG_PHASE_FIRMWARE,NOVA_DIAG_PHASE_STAGE1,
    NOVA_DIAG_PHASE_STAGE2,NOVA_DIAG_PHASE_HARDWARE,NOVA_DIAG_PHASE_RESOURCES,
    NOVA_DIAG_PHASE_UI,NOVA_DIAG_PHASE_MANAGER,NOVA_DIAG_PHASE_KERNEL_LOAD,
    NOVA_DIAG_PHASE_HANDOFF
} nova_diag_phase_t;
typedef enum {
    NOVA_DIAG_EXPORT_NDF,NOVA_DIAG_EXPORT_JSON,NOVA_DIAG_EXPORT_TEXT,
    NOVA_DIAG_EXPORT_BINARY
} nova_diag_export_format_t;
typedef struct {
    nova_diag_event_t event;
    const char *module,*source,*message,*metadata,*cause,*recommendation;
    nova_diag_phase_t phase;
    uint32_t error_class;
    bool sensitive;
} nova_diag_event_descriptor_t;
typedef struct {
    nova_diag_event_t event;
    uint64_t sequence;
    nova_diag_phase_t phase;
    uint32_t error_class;
    char module[24],source[24],message[NOVA_DIAG_TEXT_CAPACITY],metadata[48],
         cause[48],recommendation[48];
    bool redacted;
} nova_diag_record_t;
typedef struct {
    uint64_t from_us,to_us;
    nova_diag_level_t minimum_level,maximum_level;
    nova_diag_phase_t phase;
    uint32_t category,error_class;
    const char *module;
    bool use_time,use_level,use_phase,use_category,use_error_class,use_module;
} nova_diag_filter_t;
typedef struct {
    uint16_t capacity,count,registered_modules;
    uint32_t exports,denied_exports,redactions,filtered_queries;
    bool initialized,live_enabled,read_only,boot_unaffected;
} nova_diag_framework_status_t;
typedef struct {
    uint32_t fps, frame_count, draw_calls, dirty_regions, rendered_glyphs,
             rendered_images, rendered_icons,rendered_svgs;
    uint32_t minimum_frame_us,maximum_frame_us,average_frame_us,idle_time_us,
             presentation_time_us,layers,rectangles,lines,circles,paths,gradients,
             shadows,blur_effects,maximum_draw_calls,average_draw_calls;
    uint64_t frame_time_us, render_time_us, layout_time_us, compositor_time_us;
    uint64_t dirty_area,maximum_dirty_area,screen_area;
    uint32_t dirty_ratio_permille,glyph_cache_hits,glyph_cache_misses,
             glyph_rasterizations,font_switches,svg_cache_hits,svg_cache_misses,
             svg_raster_time_us,svg_transform_time_us,resource_cache_hits,
             resource_cache_misses,resource_releases,active_animations,
             completed_animations,dropped_animations,motion_tokens;
    uint64_t total_memory,used_memory,cache_memory,pool_memory,temporary_memory,
             peak_memory,gpu_memory,gpu_cache;
    uint32_t gpu_time_us,gpu_transfer_time_us;
    uint8_t gpu_load_percent;
    bool gpu_available,software_renderer;
} nova_render_statistics_t;
typedef enum {NOVA_RENDER_WINDOW_CURRENT,NOVA_RENDER_WINDOW_SECOND,
    NOVA_RENDER_WINDOW_TEN_SECONDS,NOVA_RENDER_WINDOW_SESSION} nova_render_window_t;
typedef enum {NOVA_RENDER_EXPORT_NDF,NOVA_RENDER_EXPORT_JSON,
    NOVA_RENDER_EXPORT_CSV,NOVA_RENDER_EXPORT_BINARY} nova_render_export_format_t;
typedef struct {uint64_t frame_id,start_us,end_us;uint32_t duration_us,draw_calls,
    dirty_regions,layers;uint64_t dirty_area;} nova_render_frame_record_t;
typedef struct {
    uint32_t frame_time_us, layout_time_us, animation_time_us, render_time_us,
             compositor_time_us, presentation_time_us, violations;
} nova_frame_budget_t;
typedef enum {NOVA_FRAME_CLASS_A,NOVA_FRAME_CLASS_B,NOVA_FRAME_CLASS_C,
    NOVA_FRAME_CLASS_D,NOVA_FRAME_CLASS_E} nova_frame_class_t;
typedef struct {
    uint32_t fps,frame_time_us,draw_calls,dirty_regions;
    uint64_t memory_usage;
    uint64_t cache_usage;
    uint8_t cpu_score,gpu_load_percent;
    bool gpu_available;
    uint32_t input_time_us,event_time_us,layout_time_us,animation_time_us,
             text_time_us,render_time_us,compositor_time_us,presentation_time_us;
    uint32_t minimum_frame_us,maximum_frame_us,average_frame_us;
    uint32_t soft_violations,hard_violations,phase_violations,optimization_actions;
    nova_frame_class_t frame_class;
    bool software_renderer,within_budget;
} nova_boot_perf_metrics_t;
typedef struct {uint32_t ui_engine_us,theme_us,resources_us,first_window_us,
    first_frame_us;bool valid;} nova_boot_startup_metrics_t;
typedef enum {
    NOVA_BOOT_PHASE_STAGE1,NOVA_BOOT_PHASE_STAGE2,NOVA_BOOT_PHASE_HARDWARE,
    NOVA_BOOT_PHASE_GRAPHICS,NOVA_BOOT_PHASE_RESOURCES,NOVA_BOOT_PHASE_THEME,
    NOVA_BOOT_PHASE_UI,NOVA_BOOT_PHASE_MANAGER,NOVA_BOOT_PHASE_KERNEL,
    NOVA_BOOT_PHASE_HANDOFF,NOVA_BOOT_PHASE_COUNT
} nova_boot_phase_t;
typedef enum {NOVA_BOOT_START_STANDARD,NOVA_BOOT_START_RECOVERY,
    NOVA_BOOT_START_NETWORK,NOVA_BOOT_START_DIAGNOSTIC} nova_boot_start_type_t;
typedef enum {NOVA_BOOT_TIME_CLASS_A,NOVA_BOOT_TIME_CLASS_B,NOVA_BOOT_TIME_CLASS_C,
    NOVA_BOOT_TIME_CLASS_D,NOVA_BOOT_TIME_CLASS_E} nova_boot_time_class_t;
typedef uint64_t (*nova_boot_clock_t)(void);
typedef struct {
    nova_boot_phase_t phase;
    uint64_t start_time_us,end_time_us,duration_us,budget_us;
    uint32_t violations;
    bool started,complete,exceeded;
} nova_boot_time_budget_t;
typedef struct {
    uint64_t total_time_us,total_budget_us,ui_to_first_frame_us;
    uint32_t completed_phases,violations,optimization_hints;
    nova_boot_start_type_t start_type;
    nova_boot_time_class_t performance_class;
    uint8_t hardware_class;
    bool first_frame_valid,security_checks_preserved,within_budget,clock_available;
} nova_boot_budget_diagnostics_t;
typedef struct {
    nova_quality_t quality;
    bool automatic, performance_limited, low_end, software_renderer;
    uint32_t target_fps;
} nova_quality_status_t;
typedef struct {
    uint32_t events, overwritten, warnings, errors, input_events,
             animation_events, resource_events;
} nova_diag_statistics_t;
typedef enum {NOVA_BENCH_BOOT_TIME,NOVA_BENCH_FPS,NOVA_BENCH_FRAME_TIME,
    NOVA_BENCH_MEMORY,NOVA_BENCH_RESOURCE_LOAD,NOVA_BENCH_ANIMATION,
    NOVA_BENCH_INPUT_LATENCY,NOVA_BENCH_CACHE_HIT_RATE,NOVA_BENCH_COUNT}
    nova_benchmark_id_t;
typedef enum {NOVA_REGRESSION_NONE,NOVA_REGRESSION_MINOR,
    NOVA_REGRESSION_MAJOR,NOVA_REGRESSION_CRITICAL} nova_regression_level_t;
typedef struct {
    uint32_t benchmark_id;uint64_t baseline_value,measured_value;
    int32_t deviation_basis_points;nova_regression_level_t level;
    uint16_t threshold_basis_points;bool lower_is_better,valid;
} nova_performance_regression_result_t;
typedef struct {
    char build_id[32],version[16];uint8_t hardware_profile,firmware_type;
    uint32_t executed,regressions,minor,major,critical,reports;
    bool initialized,deterministic,read_only,ci_ready;
} nova_performance_regression_status_t;
typedef enum {NOVA_FALLBACK_SOFTWARE_RENDERER,NOVA_FALLBACK_STANDARD_THEME,
    NOVA_FALLBACK_STANDARD_FONT,NOVA_FALLBACK_PLACEHOLDER_GRAPHIC,
    NOVA_FALLBACK_STANDARD_ANIMATION,NOVA_FALLBACK_RECOVERY_MODE,
    NOVA_FALLBACK_SAFE_MODE,NOVA_FALLBACK_MINIMAL_MODE,
    NOVA_FALLBACK_LOW_END_PROFILE,NOVA_FALLBACK_MODE_COUNT} nova_fallback_mode_t;
typedef struct {nova_fallback_mode_t mode;uint32_t trigger,error_code,boot_phase;
    uint64_t activation_time_us,memory_before,memory_after;bool activated,successful,
    automatic,interactive,security_preserved,integrity_preserved,recovery_available;}
    nova_fallback_test_result_t;
typedef struct {uint16_t count;uint32_t executed,successful,failed,duplicates,reports;
    bool initialized,deterministic,heap_free,security_preserved,interactive;}
    nova_fallback_test_status_t;

void nova_diag_initialize(void);
void nova_diag_log(nova_diag_event_t event);
const nova_diag_event_t *nova_diag_get(uint32_t chronological_index);
bool nova_diag_configure_capacity(uint16_t capacity);
bool nova_diag_register_module(uint32_t category,const char *name);
bool nova_diag_log_extended(const nova_diag_event_descriptor_t *event);
const nova_diag_record_t *nova_diag_get_record(uint32_t chronological_index);
const nova_diag_record_t *nova_diag_query(const nova_diag_filter_t *filter,
    uint32_t matching_index);
bool nova_diag_set_live(bool enabled,bool user_authorized);
bool nova_diag_export(nova_diag_export_format_t format,bool user_authorized,
    uint8_t *output,uint32_t output_capacity,uint32_t *written);
void nova_diag_reset(void);
const nova_diag_framework_status_t *nova_diag_framework_status(void);
void nova_diag_frame(uint32_t frame_us, uint32_t layout_us, uint32_t animation_us,
                     uint32_t render_us, uint32_t compositor_us);
void nova_diag_frame_extended(uint32_t frame_us,uint32_t input_us,uint32_t event_us,
    uint32_t layout_us,uint32_t animation_us,uint32_t text_us,uint32_t render_us,
    uint32_t compositor_us,uint32_t presentation_us);
bool nova_boot_perf_initialize(void);
const nova_boot_perf_metrics_t *nova_boot_perf_metrics(void);
bool nova_boot_perf_reset(void);
bool nova_boot_perf_record_startup(uint32_t ui_engine_us,uint32_t theme_us,
    uint32_t resources_us,uint32_t first_window_us,uint32_t first_frame_us);
const nova_boot_startup_metrics_t *nova_boot_perf_startup(void);
bool nova_boot_budget_initialize(void);
void nova_boot_budget_set_clock(nova_boot_clock_t clock);
bool nova_boot_budget_set_start_type(nova_boot_start_type_t type,uint8_t hardware_class);
bool nova_boot_budget_begin(nova_boot_phase_t phase);
bool nova_boot_budget_end(nova_boot_phase_t phase);
bool nova_boot_budget_begin_at(nova_boot_phase_t phase,uint64_t timestamp_us);
bool nova_boot_budget_end_at(nova_boot_phase_t phase,uint64_t timestamp_us);
bool nova_boot_budget_first_frame(uint64_t timestamp_us);
const nova_boot_time_budget_t *nova_boot_budget_status(void);
const nova_boot_time_budget_t *nova_boot_budget_phase(nova_boot_phase_t phase);
const nova_boot_budget_diagnostics_t *nova_boot_budget_diagnostics(void);
bool nova_frame_budget_exceeded(void);
void nova_diag_snapshot(void);
void nova_diag_set_quality(nova_quality_t quality, bool automatic);
const nova_render_statistics_t *nova_diag_render_statistics(void);
bool nova_render_statistics_initialize(void);
const nova_render_statistics_t *nova_render_statistics_current(void);
const nova_render_statistics_t *nova_render_statistics_average(nova_render_window_t window);
const nova_render_frame_record_t *nova_render_statistics_frame(uint32_t chronological_index);
uint16_t nova_render_statistics_frame_count(void);
bool nova_render_statistics_export(nova_render_export_format_t format,
    bool user_authorized,uint8_t *output,uint32_t capacity,uint32_t *written);
void nova_render_statistics_reset(void);
const nova_memory_budget_t *nova_diag_memory_budget(void);
const nova_frame_budget_t *nova_diag_frame_budget(void);
const nova_quality_status_t *nova_diag_quality(void);
const nova_diag_statistics_t *nova_diag_statistics(void);
bool nova_performance_regression_initialize(const char *build_id,const char *version,
    uint8_t hardware_profile,uint8_t firmware_type);
bool nova_performance_regression_set_baseline(nova_benchmark_id_t benchmark,
    uint64_t value,bool lower_is_better,uint16_t threshold_basis_points);
bool nova_performance_regression_measure(nova_benchmark_id_t benchmark,uint64_t value);
bool nova_performance_regression_execute(void);
const nova_performance_regression_result_t *nova_performance_regression_results(void);
const nova_performance_regression_status_t *nova_performance_regression_status(void);
bool nova_performance_regression_generate_report(bool user_authorized,
    uint8_t *output,uint32_t capacity,uint32_t *written);
bool nova_fallback_test_initialize(void);
bool nova_fallback_test_execute(nova_fallback_mode_t mode);
const nova_fallback_test_result_t *nova_fallback_test_results(void);
const nova_fallback_test_status_t *nova_fallback_test_status(void);
bool nova_fallback_test_generate_report(bool user_authorized,
    uint8_t *output,uint32_t capacity,uint32_t *written);

#endif
