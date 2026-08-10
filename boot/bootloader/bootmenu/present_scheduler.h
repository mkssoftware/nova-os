#ifndef NOVA_BOOT_PRESENT_SCHEDULER_H
#define NOVA_BOOT_PRESENT_SCHEDULER_H

#include <stdbool.h>
#include <stdint.h>
#include "compositor.h"

#define NOVA_PRESENT_DAMAGE_CAPACITY 64u

typedef enum {
    NOVA_BUFFER_FREE,
    NOVA_BUFFER_RENDERING,
    NOVA_BUFFER_READY,
    NOVA_BUFFER_PRESENTING,
    NOVA_BUFFER_DISPLAYED
} nova_buffer_state_t;

typedef enum {
    NOVA_PRESENT_DOUBLE_BUFFER,
    NOVA_PRESENT_TRIPLE_BUFFER
} nova_present_buffering_t;

typedef enum {
    NOVA_PRESENT_VSYNC_DISABLED,
    NOVA_PRESENT_VSYNC_EMULATED,
    NOVA_PRESENT_VSYNC_HARDWARE
} nova_present_vsync_t;

typedef enum {
    NOVA_PRESENT_OK,
    NOVA_PRESENT_RECOVERED,
    NOVA_PRESENT_INVALID_ARGUMENT,
    NOVA_PRESENT_INVALID_STATE,
    NOVA_PRESENT_BACKEND_ERROR
} nova_present_result_t;

typedef uint64_t (*nova_present_clock_t)(void *context);

typedef struct {
    uint32_t width, height, source_stride;
    uint64_t available_memory;
    uint32_t *triple_buffer;
    uint64_t triple_buffer_pixels;
    bool request_triple_buffer;
    bool page_flip_available;
    nova_present_vsync_t vsync;
    nova_present_clock_t clock;
    void *clock_context;
} nova_present_configuration_t;

typedef struct {
    uint32_t initializations, shutdowns, frames, completed_frames;
    uint32_t full_presents, partial_presents, forced_presents;
    uint32_t double_buffer_frames, triple_buffer_frames, pending_copies;
    uint32_t merged_regions, damage_overflows, invalid_transitions;
    uint32_t lock_conflicts, backend_errors, recovery_attempts;
    uint32_t recovered_frames, safe_mode_entries, dropped_frames;
    uint32_t page_flips, memory_copies, vsync_waits;
    uint64_t rendered_pixels, copied_pixels;
    uint64_t last_frame_us, last_render_us, last_present_us;
    uint64_t maximum_present_us;
    nova_present_buffering_t buffering;
    nova_present_vsync_t vsync;
    nova_buffer_state_t render_state, pending_state, display_state;
    bool initialized, busy, first_frame;
} nova_present_diagnostics_t;

nova_present_result_t nova_present_initialize(
    const nova_present_configuration_t *configuration);
nova_present_result_t nova_present_begin(const uint32_t *pixels);
nova_present_result_t nova_present_damage(nova_rect_t region);
nova_present_result_t nova_present_frame(void);
nova_present_result_t nova_present_full(void);
nova_present_result_t nova_present_partial(const nova_rect_t *regions,
                                           uint32_t count);
bool nova_present_is_busy(void);
void nova_present_shutdown(void);
const nova_present_diagnostics_t *nova_present_diagnostics(void);

#endif
