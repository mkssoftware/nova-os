#include "software_renderer.h"
#include "background_blur.h"
#include "compositor.h"
#include "dirty_manager.h"
#include "effects.h"
#include "image_renderer.h"
#include "render_quality.h"
#include "render_queue.h"

static nova_sw_renderer_diagnostics_t diagnostics;
static uint8_t consecutive_failures;

static void collect_diagnostics(void)
{
    const nova_compositor_diagnostics_t *compositor=nova_compositor_diagnostics();
    const nova_image_diagnostics_t *images=nova_image_diagnostics();
    diagnostics.draw_calls=compositor->submitted;
    diagnostics.dirty_regions=compositor->composed_regions;
    diagnostics.rendered_pixels=images->pixels_rendered;
    diagnostics.cache_hits=images->cache_hits+nova_blur_diagnostics()->cache_hits;
    diagnostics.cache_misses=images->cache_misses+nova_blur_diagnostics()->cache_misses;
}

nova_sw_renderer_result_t nova_sw_renderer_initialize(
    const nova_sw_renderer_configuration_t *configuration)
{
    if(!configuration||!configuration->width||!configuration->height||
       configuration->width>1920||configuration->height>1080){
        ++diagnostics.invalid_calls;return NOVA_SW_RENDERER_INVALID_ARGUMENT;
    }
    diagnostics=(nova_sw_renderer_diagnostics_t){0};consecutive_failures=0;
    diagnostics.initialized=true;diagnostics.initializations=1;
    diagnostics.available=configuration->framebuffer_available;
    diagnostics.forced=configuration->force_software;
    diagnostics.diagnostic_mode=configuration->diagnostic_mode;
    diagnostics.memory_bytes=configuration->available_memory;
    diagnostics.simd=NOVA_SW_SIMD_SCALAR;
    diagnostics.state=configuration->framebuffer_available?
        (configuration->safe_mode?NOVA_SW_RENDERER_SAFE:NOVA_SW_RENDERER_ACTIVE):
        NOVA_SW_RENDERER_TEXT;
    if(!configuration->framebuffer_available){++diagnostics.text_requests;
        return NOVA_SW_RENDERER_NOT_AVAILABLE;}
    if(configuration->safe_mode)(void)nova_render_quality_set(NOVA_RENDER_QUALITY_SAFE);
    nova_dirty_force_full(NOVA_DIRTY_FULL_INITIALIZATION);++diagnostics.full_repaints;
    return NOVA_SW_RENDERER_OK;
}

nova_sw_renderer_result_t nova_sw_renderer_shutdown(void)
{
    if(!diagnostics.initialized){++diagnostics.invalid_calls;
        return NOVA_SW_RENDERER_INVALID_STATE;}
    nova_blur_clear_cache();++diagnostics.cache_clears;++diagnostics.shutdowns;
    diagnostics.initialized=false;diagnostics.available=false;
    diagnostics.state=NOVA_SW_RENDERER_OFFLINE;return NOVA_SW_RENDERER_OK;
}

bool nova_sw_renderer_is_available(void)
{return diagnostics.initialized&&diagnostics.available&&
    diagnostics.state!=NOVA_SW_RENDERER_TEXT;}
bool nova_sw_renderer_text_required(void)
{return diagnostics.state==NOVA_SW_RENDERER_TEXT;}

nova_sw_renderer_result_t nova_sw_renderer_begin_frame(void)
{
    if(!nova_sw_renderer_is_available()){++diagnostics.invalid_calls;
        return nova_sw_renderer_text_required()?NOVA_SW_RENDERER_TEXT_REQUIRED:
            NOVA_SW_RENDERER_NOT_AVAILABLE;}
    ++diagnostics.frames;return NOVA_SW_RENDERER_OK;
}

nova_sw_renderer_result_t nova_sw_renderer_render_frame(const nova_scene_node_t *scene)
{
    if(!nova_sw_renderer_is_available())return NOVA_SW_RENDERER_NOT_AVAILABLE;
    if(!scene||!scene->active){++diagnostics.invalid_calls;
        return NOVA_SW_RENDERER_INVALID_ARGUMENT;}
    if(!nova_scene_traverse(0,0,false))return nova_sw_renderer_report_failure(false);
    ++diagnostics.rendered_frames;collect_diagnostics();return NOVA_SW_RENDERER_OK;
}

nova_sw_renderer_result_t nova_sw_renderer_present(void)
{
    if(!nova_sw_renderer_is_available())return NOVA_SW_RENDERER_NOT_AVAILABLE;
    if(!nova_compositor_present())return nova_sw_renderer_report_failure(true);
    ++diagnostics.presents;collect_diagnostics();return NOVA_SW_RENDERER_OK;
}

nova_sw_renderer_result_t nova_sw_renderer_reset(void)
{
    if(!diagnostics.initialized||diagnostics.state==NOVA_SW_RENDERER_TEXT){
        ++diagnostics.invalid_calls;return NOVA_SW_RENDERER_INVALID_STATE;}
    diagnostics.state=NOVA_SW_RENDERER_RESETTING;
    nova_blur_clear_cache();nova_render_abort_frame();
    nova_dirty_force_full(NOVA_DIRTY_FULL_RECOVERY);
    ++diagnostics.cache_clears;++diagnostics.resets;++diagnostics.full_repaints;
    diagnostics.state=nova_render_quality_get()==NOVA_RENDER_QUALITY_SAFE?
        NOVA_SW_RENDERER_SAFE:NOVA_SW_RENDERER_ACTIVE;
    return NOVA_SW_RENDERER_RECOVERED;
}

nova_sw_renderer_result_t nova_sw_renderer_report_failure(bool present_failure)
{
    if(!diagnostics.initialized)return NOVA_SW_RENDERER_INVALID_STATE;
    ++diagnostics.failures;if(consecutive_failures<3)++consecutive_failures;
    if(consecutive_failures==1)return nova_sw_renderer_reset();
    if(consecutive_failures==2){
        (void)nova_render_quality_set(NOVA_RENDER_QUALITY_SAFE);
        diagnostics.state=NOVA_SW_RENDERER_SAFE;++diagnostics.safe_entries;
        (void)nova_sw_renderer_reset();return NOVA_SW_RENDERER_RECOVERED;
    }
    diagnostics.state=NOVA_SW_RENDERER_TEXT;diagnostics.available=false;
    ++diagnostics.text_requests;
    return present_failure?NOVA_SW_RENDERER_TEXT_REQUIRED:NOVA_SW_RENDERER_RENDER_FAILED;
}

nova_sw_renderer_result_t nova_sw_renderer_complete_frame(bool render_success,
                                                           bool present_success)
{
    if(!render_success)return nova_sw_renderer_report_failure(false);
    if(!present_success)return nova_sw_renderer_report_failure(true);
    consecutive_failures=0;++diagnostics.rendered_frames;collect_diagnostics();
    return NOVA_SW_RENDERER_OK;
}

nova_sw_renderer_result_t nova_sw_renderer_report_memory(uint64_t memory)
{
    if(!diagnostics.initialized)return NOVA_SW_RENDERER_INVALID_STATE;
    diagnostics.memory_bytes=memory;nova_render_quality_report_memory(memory);
    if(memory<16ull*1024u*1024u){++diagnostics.memory_degradations;
        diagnostics.state=NOVA_SW_RENDERER_SAFE;
        nova_blur_clear_cache();++diagnostics.cache_clears;
        nova_dirty_force_full(NOVA_DIRTY_FULL_MEMORY);++diagnostics.full_repaints;
        return NOVA_SW_RENDERER_RECOVERED;}
    return NOVA_SW_RENDERER_OK;
}

const nova_sw_renderer_diagnostics_t *nova_sw_renderer_diagnostics(void)
{return &diagnostics;}
