#ifndef NOVA_BOOT_SOFTWARE_RENDERER_H
#define NOVA_BOOT_SOFTWARE_RENDERER_H

#include <stdbool.h>
#include <stdint.h>
#include "scene_graph.h"

typedef enum {
    NOVA_SW_RENDERER_OK,
    NOVA_SW_RENDERER_RECOVERED,
    NOVA_SW_RENDERER_INVALID_ARGUMENT,
    NOVA_SW_RENDERER_NOT_AVAILABLE,
    NOVA_SW_RENDERER_INVALID_STATE,
    NOVA_SW_RENDERER_RENDER_FAILED,
    NOVA_SW_RENDERER_PRESENT_FAILED,
    NOVA_SW_RENDERER_TEXT_REQUIRED
} nova_sw_renderer_result_t;

typedef enum {
    NOVA_SW_RENDERER_OFFLINE,
    NOVA_SW_RENDERER_ACTIVE,
    NOVA_SW_RENDERER_RESETTING,
    NOVA_SW_RENDERER_SAFE,
    NOVA_SW_RENDERER_TEXT
} nova_sw_renderer_state_t;

typedef enum {
    NOVA_SW_SIMD_SCALAR,
    NOVA_SW_SIMD_SSE2,
    NOVA_SW_SIMD_SSE4,
    NOVA_SW_SIMD_AVX2
} nova_sw_simd_t;

typedef struct {
    uint32_t width,height;
    uint64_t available_memory;
    bool framebuffer_available;
    bool force_software,diagnostic_mode,safe_mode;
} nova_sw_renderer_configuration_t;

typedef struct {
    uint32_t initializations,shutdowns,frames,rendered_frames,presents;
    uint32_t failures,resets,full_repaints,safe_entries,text_requests;
    uint32_t memory_degradations,cache_clears,invalid_calls;
    uint32_t draw_calls,dirty_regions,cache_hits,cache_misses;
    uint64_t rendered_pixels,memory_bytes,last_frame_us,last_render_us,last_present_us;
    nova_sw_renderer_state_t state;
    nova_sw_simd_t simd;
    bool initialized,available,forced,diagnostic_mode;
} nova_sw_renderer_diagnostics_t;

nova_sw_renderer_result_t nova_sw_renderer_initialize(
    const nova_sw_renderer_configuration_t *configuration);
nova_sw_renderer_result_t nova_sw_renderer_shutdown(void);
nova_sw_renderer_result_t nova_sw_renderer_begin_frame(void);
nova_sw_renderer_result_t nova_sw_renderer_render_frame(const nova_scene_node_t *scene);
nova_sw_renderer_result_t nova_sw_renderer_present(void);
nova_sw_renderer_result_t nova_sw_renderer_complete_frame(bool render_success,
                                                           bool present_success);
nova_sw_renderer_result_t nova_sw_renderer_report_memory(uint64_t available_memory);
nova_sw_renderer_result_t nova_sw_renderer_report_failure(bool present_failure);
nova_sw_renderer_result_t nova_sw_renderer_reset(void);
bool nova_sw_renderer_is_available(void);
bool nova_sw_renderer_text_required(void);
const nova_sw_renderer_diagnostics_t *nova_sw_renderer_diagnostics(void);

#endif
