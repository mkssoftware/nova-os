#ifndef NOVA_BOOT_DIAGNOSTICS_H
#define NOVA_BOOT_DIAGNOSTICS_H

#include <stdint.h>
#include <stdbool.h>

#define NOVA_DIAG_CAPACITY 256u
typedef enum { NOVA_DIAG_TRACE, NOVA_DIAG_DEBUG, NOVA_DIAG_INFO,
    NOVA_DIAG_WARNING, NOVA_DIAG_ERROR, NOVA_DIAG_CRITICAL } nova_diag_level_t;
typedef enum { NOVA_QUALITY_ULTRA, NOVA_QUALITY_HIGH, NOVA_QUALITY_BALANCED,
    NOVA_QUALITY_LOW, NOVA_QUALITY_SAFE, NOVA_QUALITY_AUTO } nova_quality_t;
typedef struct {
    uint64_t timestamp_us;
    nova_diag_level_t level;
    uint32_t category, event_id, value_a, value_b;
} nova_diag_event_t;
typedef struct {
    uint32_t fps, frame_count, draw_calls, dirty_regions, rendered_glyphs,
             rendered_images, rendered_icons;
    uint64_t frame_time_us, render_time_us, layout_time_us, compositor_time_us;
} nova_render_statistics_t;
typedef struct {
    uint64_t total_budget, used_memory, free_memory, cache_memory, pool_memory,
             peak_memory;
} nova_memory_budget_t;
typedef struct {
    uint32_t frame_time_us, layout_time_us, animation_time_us, render_time_us,
             compositor_time_us, presentation_time_us, violations;
} nova_frame_budget_t;
typedef struct {
    nova_quality_t quality;
    bool automatic, performance_limited, low_end, software_renderer;
    uint32_t target_fps;
} nova_quality_status_t;
typedef struct {
    uint32_t events, overwritten, warnings, errors, input_events,
             animation_events, resource_events;
} nova_diag_statistics_t;

void nova_diag_initialize(void);
void nova_diag_log(nova_diag_event_t event);
const nova_diag_event_t *nova_diag_get(uint32_t chronological_index);
void nova_diag_frame(uint32_t frame_us, uint32_t layout_us, uint32_t animation_us,
                     uint32_t render_us, uint32_t compositor_us);
void nova_diag_snapshot(void);
void nova_diag_set_quality(nova_quality_t quality, bool automatic);
const nova_render_statistics_t *nova_diag_render_statistics(void);
const nova_memory_budget_t *nova_diag_memory_budget(void);
const nova_frame_budget_t *nova_diag_frame_budget(void);
const nova_quality_status_t *nova_diag_quality(void);
const nova_diag_statistics_t *nova_diag_statistics(void);

#endif
