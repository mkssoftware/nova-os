#include "present_scheduler.h"

#include "framebuffer_backend.h"
#include "recovery.h"
#include "state_model.h"

static nova_present_configuration_t configuration;
static nova_present_diagnostics_t diagnostics;
static nova_rect_t damage[NOVA_PRESENT_DAMAGE_CAPACITY];
static uint32_t damage_count;
static const uint32_t *render_pixels;
static uint64_t frame_started_us, render_finished_us;
static bool force_full, pending_valid;

static uint64_t clock_now(void)
{
    if (configuration.clock)
        return configuration.clock(configuration.clock_context);
    return (uint64_t)nova_state_diagnostics()->current_time_ms * 1000u;
}

static nova_present_result_t invalid_state(void)
{
    ++diagnostics.invalid_transitions;
    return NOVA_PRESENT_INVALID_STATE;
}

static bool rectangle_valid(nova_rect_t region)
{
    return region.x >= 0 && region.y >= 0 && region.width > 0 &&
           region.height > 0 && (uint32_t)region.x < configuration.width &&
           (uint32_t)region.y < configuration.height &&
           (uint32_t)region.width <= configuration.width - (uint32_t)region.x &&
           (uint32_t)region.height <= configuration.height - (uint32_t)region.y;
}

static bool overlaps(nova_rect_t a, nova_rect_t b)
{
    int64_t ar = (int64_t)a.x + a.width, ab = (int64_t)a.y + a.height;
    int64_t br = (int64_t)b.x + b.width, bb = (int64_t)b.y + b.height;
    return a.x <= br && b.x <= ar && a.y <= bb && b.y <= ab;
}

static nova_rect_t rectangle_union(nova_rect_t a, nova_rect_t b)
{
    int32_t x = a.x < b.x ? a.x : b.x;
    int32_t y = a.y < b.y ? a.y : b.y;
    int32_t right_a = a.x + a.width, right_b = b.x + b.width;
    int32_t bottom_a = a.y + a.height, bottom_b = b.y + b.height;
    int32_t right = right_a > right_b ? right_a : right_b;
    int32_t bottom = bottom_a > bottom_b ? bottom_a : bottom_b;
    return (nova_rect_t){x, y, right - x, bottom - y};
}

nova_present_result_t nova_present_initialize(
    const nova_present_configuration_t *requested)
{
    diagnostics = (nova_present_diagnostics_t){0};
    configuration = (nova_present_configuration_t){0};
    damage_count = 0;
    render_pixels = 0;
    force_full = true;
    pending_valid = false;
    if (!requested || !requested->width || !requested->height ||
        requested->width > NOVA_SURFACE_WIDTH ||
        requested->height > NOVA_SURFACE_HEIGHT ||
        requested->source_stride < requested->width ||
        !nova_framebuffer_info()->initialized ||
        nova_framebuffer_info()->width != requested->width ||
        nova_framebuffer_info()->height != requested->height)
        return NOVA_PRESENT_INVALID_ARGUMENT;

    uint64_t frame_bytes = (uint64_t)requested->width *
                           requested->height * sizeof(uint32_t);
    configuration = *requested;
    diagnostics.buffering = requested->request_triple_buffer &&
        requested->triple_buffer &&
        requested->triple_buffer_pixels >=
            (uint64_t)requested->width * requested->height &&
        requested->available_memory >= frame_bytes ?
        NOVA_PRESENT_TRIPLE_BUFFER : NOVA_PRESENT_DOUBLE_BUFFER;
    diagnostics.vsync = requested->vsync;
    diagnostics.render_state = NOVA_BUFFER_FREE;
    diagnostics.pending_state = NOVA_BUFFER_FREE;
    diagnostics.display_state = NOVA_BUFFER_DISPLAYED;
    diagnostics.initialized = true;
    diagnostics.first_frame = true;
    ++diagnostics.initializations;
    return NOVA_PRESENT_OK;
}

nova_present_result_t nova_present_begin(const uint32_t *pixels)
{
    if (!diagnostics.initialized || !pixels)
        return NOVA_PRESENT_INVALID_ARGUMENT;
    if (diagnostics.busy || diagnostics.render_state != NOVA_BUFFER_FREE) {
        ++diagnostics.lock_conflicts;
        return invalid_state();
    }
    diagnostics.busy = true;
    diagnostics.render_state = NOVA_BUFFER_RENDERING;
    damage_count = 0;
    render_pixels = pixels;
    force_full = diagnostics.first_frame;
    frame_started_us = clock_now();
    ++diagnostics.frames;
    return NOVA_PRESENT_OK;
}

nova_present_result_t nova_present_damage(nova_rect_t region)
{
    if (!diagnostics.initialized ||
        diagnostics.render_state != NOVA_BUFFER_RENDERING)
        return invalid_state();
    if (!rectangle_valid(region))
        return NOVA_PRESENT_INVALID_ARGUMENT;
    for (uint32_t i = 0; i < damage_count; ++i) {
        if (overlaps(damage[i], region)) {
            damage[i] = rectangle_union(damage[i], region);
            ++diagnostics.merged_regions;
            return NOVA_PRESENT_OK;
        }
    }
    if (damage_count == NOVA_PRESENT_DAMAGE_CAPACITY) {
        force_full = true;
        ++diagnostics.damage_overflows;
        return NOVA_PRESENT_OK;
    }
    damage[damage_count++] = region;
    return NOVA_PRESENT_OK;
}

static uint64_t dirty_area(void)
{
    uint64_t area = 0;
    for (uint32_t i = 0; i < damage_count; ++i)
        area += (uint64_t)damage[i].width * damage[i].height;
    return area;
}

static void copy_region_to_pending(nova_rect_t region)
{
    for (int32_t y = 0; y < region.height; ++y) {
        const uint32_t *source = render_pixels +
            (uint64_t)(region.y + y) * configuration.source_stride + region.x;
        uint32_t *target = configuration.triple_buffer +
            (uint64_t)(region.y + y) * configuration.width + region.x;
        for (int32_t x = 0; x < region.width; ++x)
            target[x] = source[x];
    }
    diagnostics.copied_pixels += (uint64_t)region.width * region.height;
}

static void prepare_pending(bool full)
{
    diagnostics.pending_state = NOVA_BUFFER_RENDERING;
    if (full) {
        copy_region_to_pending((nova_rect_t){0, 0, (int32_t)configuration.width,
                                             (int32_t)configuration.height});
        pending_valid = true;
    } else {
        for (uint32_t i = 0; i < damage_count; ++i)
            copy_region_to_pending(damage[i]);
    }
    diagnostics.pending_state = NOVA_BUFFER_READY;
    ++diagnostics.pending_copies;
}

static nova_fb_result_t backend_present(const uint32_t *pixels,
                                        uint32_t stride, bool full)
{
    nova_fb_result_t result = nova_framebuffer_begin_frame();
    if (result != NOVA_FB_OK) return result;
    if (!full) {
        for (uint32_t i = 0; i < damage_count; ++i) {
            result = nova_framebuffer_damage(damage[i]);
            if (result != NOVA_FB_OK) return result;
        }
    }
    return nova_framebuffer_present(pixels, configuration.width,
                                    configuration.height, stride);
}

static nova_present_result_t finish_present(bool explicitly_forced)
{
    if (!diagnostics.initialized ||
        diagnostics.render_state != NOVA_BUFFER_RENDERING ||
        !diagnostics.busy || !render_pixels)
        return invalid_state();

    render_finished_us = clock_now();
    diagnostics.render_state = NOVA_BUFFER_READY;
    uint64_t full_area = (uint64_t)configuration.width * configuration.height;
    uint64_t area = dirty_area();
    bool full = force_full || !damage_count || area * 5u >= full_area * 3u ||
                damage_count > NOVA_FB_DAMAGE_CAPACITY;
    if (explicitly_forced) {
        full = true;
        ++diagnostics.forced_presents;
    }

    const uint32_t *source = render_pixels;
    uint32_t stride = configuration.source_stride;
    if (diagnostics.buffering == NOVA_PRESENT_TRIPLE_BUFFER) {
        if (!pending_valid) full = true;
        prepare_pending(full);
        source = configuration.triple_buffer;
        stride = configuration.width;
        ++diagnostics.triple_buffer_frames;
    } else {
        ++diagnostics.double_buffer_frames;
    }

    diagnostics.render_state = NOVA_BUFFER_PRESENTING;
    diagnostics.pending_state = diagnostics.buffering == NOVA_PRESENT_TRIPLE_BUFFER ?
        NOVA_BUFFER_PRESENTING : NOVA_BUFFER_FREE;
    uint64_t present_started = clock_now();
    nova_fb_result_t backend = backend_present(source, stride, full);
    nova_present_result_t result = NOVA_PRESENT_OK;
    if (backend != NOVA_FB_OK) {
        ++diagnostics.backend_errors;
        ++diagnostics.recovery_attempts;
        backend = backend_present(source, stride, true);
        if (backend == NOVA_FB_OK) {
            ++diagnostics.recovered_frames;
            full = true;
            result = NOVA_PRESENT_RECOVERED;
        } else {
            ++diagnostics.backend_errors;
            ++diagnostics.safe_mode_entries;
            ++diagnostics.dropped_frames;
            nova_recovery_report(0x80080001u,NOVA_UI_SUBSYSTEM_RENDERING,
                NOVA_UI_ERROR_CRITICAL,NOVA_RECOVERY_SAFE_MODE,
                (uint32_t)(clock_now() / 1000u));
            result = NOVA_PRESENT_BACKEND_ERROR;
        }
    }
    uint64_t ended = clock_now();
    diagnostics.last_render_us = render_finished_us >= frame_started_us ?
        render_finished_us - frame_started_us : 0;
    diagnostics.last_present_us = ended >= present_started ?
        ended - present_started : 0;
    diagnostics.last_frame_us = ended >= frame_started_us ?
        ended - frame_started_us : 0;
    if (diagnostics.last_present_us > diagnostics.maximum_present_us)
        diagnostics.maximum_present_us = diagnostics.last_present_us;

    if (result != NOVA_PRESENT_BACKEND_ERROR) {
        ++diagnostics.completed_frames;
        if (full) ++diagnostics.full_presents;
        else ++diagnostics.partial_presents;
        diagnostics.rendered_pixels += full ? full_area : area;
        if (configuration.page_flip_available) ++diagnostics.page_flips;
        else ++diagnostics.memory_copies;
        if (configuration.vsync != NOVA_PRESENT_VSYNC_DISABLED)
            ++diagnostics.vsync_waits;
        diagnostics.first_frame = false;
    }
    diagnostics.render_state = NOVA_BUFFER_FREE;
    diagnostics.pending_state = NOVA_BUFFER_FREE;
    diagnostics.display_state = NOVA_BUFFER_DISPLAYED;
    diagnostics.busy = false;
    render_pixels = 0;
    damage_count = 0;
    force_full = false;
    return result;
}

nova_present_result_t nova_present_frame(void)
{
    return finish_present(false);
}

nova_present_result_t nova_present_full(void)
{
    return finish_present(true);
}

nova_present_result_t nova_present_partial(const nova_rect_t *regions,
                                           uint32_t count)
{
    if (!regions || !count || count > NOVA_PRESENT_DAMAGE_CAPACITY)
        return NOVA_PRESENT_INVALID_ARGUMENT;
    for (uint32_t i = 0; i < count; ++i) {
        nova_present_result_t result = nova_present_damage(regions[i]);
        if (result != NOVA_PRESENT_OK) return result;
    }
    return finish_present(false);
}

bool nova_present_is_busy(void)
{
    return diagnostics.busy;
}

void nova_present_shutdown(void)
{
    if (!diagnostics.initialized) return;
    diagnostics.initialized = false;
    diagnostics.busy = false;
    diagnostics.render_state = NOVA_BUFFER_FREE;
    diagnostics.pending_state = NOVA_BUFFER_FREE;
    render_pixels = 0;
    ++diagnostics.shutdowns;
}

const nova_present_diagnostics_t *nova_present_diagnostics(void)
{
    return &diagnostics;
}
