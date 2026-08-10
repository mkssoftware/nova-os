#include "render_quality.h"
#include "compositor.h"
#include "dirty_manager.h"
#include "motion.h"

#define MIB (1024u*1024u)

static const nova_render_quality_parameters_t profiles[NOVA_RENDER_QUALITY_COUNT]={
    {NOVA_BLUR_QUALITY_HIGH,NOVA_EFFECT_QUALITY_HIGH,NOVA_ROUNDED_AA_HIGH,
     NOVA_IMAGE_SAMPLE_BILINEAR,64u*MIB,32,8,0,true,true,true,true,true,true},
    {NOVA_BLUR_QUALITY_HIGH,NOVA_EFFECT_QUALITY_HIGH,NOVA_ROUNDED_AA_HIGH,
     NOVA_IMAGE_SAMPLE_BILINEAR,48u*MIB,24,6,0,true,true,true,true,true,true},
    {NOVA_BLUR_QUALITY_STANDARD,NOVA_EFFECT_QUALITY_STANDARD,NOVA_ROUNDED_AA_STANDARD,
     NOVA_IMAGE_SAMPLE_BILINEAR,32u*MIB,16,4,0,true,true,true,true,true,false},
    {NOVA_BLUR_QUALITY_LOW,NOVA_EFFECT_QUALITY_LOW,NOVA_ROUNDED_AA_STANDARD,
     NOVA_IMAGE_SAMPLE_NEAREST,20u*MIB,4,2,2,true,true,false,true,true,false},
    {NOVA_BLUR_QUALITY_LOW,NOVA_EFFECT_QUALITY_LOW,NOVA_ROUNDED_AA_DISABLED,
     NOVA_IMAGE_SAMPLE_NEAREST,12u*MIB,0,0,3,false,false,false,false,false,false}
};
static nova_render_quality_diagnostics_t diagnostics;
static uint8_t consecutive_slow_frames;

bool nova_render_quality_is_supported(nova_render_quality_t profile)
{return profile<NOVA_RENDER_QUALITY_COUNT;}

static nova_render_quality_t hardware_profile(uint64_t memory,bool firmware_limited,
                                               bool software_renderer)
{
    if(firmware_limited||memory<16u*MIB)return NOVA_RENDER_QUALITY_SAFE;
    if(software_renderer||memory<32u*MIB)return NOVA_RENDER_QUALITY_PERFORMANCE;
    if(memory>=64u*MIB)return NOVA_RENDER_QUALITY_HIGH;
    return NOVA_RENDER_QUALITY_STANDARD;
}

bool nova_render_quality_apply(void)
{
    if(!diagnostics.initialized||!nova_render_quality_is_supported(diagnostics.active))
        return false;
    const nova_render_quality_parameters_t *profile=&profiles[diagnostics.active];
    nova_blur_set_quality(profile->blur_quality);
    nova_blur_clear_cache();
    nova_effect_set_quality(profile->effect_quality);
    nova_effect_set_density(profile->effect_density);
    nova_motion_set_reduced(!profile->animations_enabled);
    nova_compositor_set_fallback(profile->compositor_fallback);
    nova_dirty_force_full(NOVA_DIRTY_FULL_FORCED);
    ++diagnostics.applications;++diagnostics.cache_invalidations;
    ++diagnostics.full_redraws;
    diagnostics.disabled_effects=(profile->blur_enabled?0u:1u)+
        (profile->shadows_enabled?0u:1u)+(profile->glows_enabled?0u:1u)+
        (profile->transparency_enabled?0u:1u)+(profile->animations_enabled?0u:1u);
    return true;
}

static bool switch_profile(nova_render_quality_t profile,nova_quality_change_reason_t reason)
{
    if(!nova_render_quality_is_supported(profile)){++diagnostics.rejected_profiles;return false;}
    bool changed=diagnostics.active!=profile;diagnostics.active=profile;
    diagnostics.last_reason=reason;if(changed)++diagnostics.switches;
    return nova_render_quality_apply();
}

bool nova_render_quality_initialize(bool software_renderer,uint64_t memory)
{
    diagnostics=(nova_render_quality_diagnostics_t){0};consecutive_slow_frames=0;
    diagnostics.initialized=true;diagnostics.initializations=1;
    diagnostics.software_renderer=software_renderer;diagnostics.observed_memory=memory;
    diagnostics.requested=NOVA_RENDER_QUALITY_STANDARD;
    diagnostics.active=NOVA_RENDER_QUALITY_STANDARD;
    diagnostics.last_reason=NOVA_QUALITY_REASON_INITIALIZATION;
    return nova_render_quality_apply();
}

bool nova_render_quality_set(nova_render_quality_t profile)
{
    if(!diagnostics.initialized||!nova_render_quality_is_supported(profile)){
        ++diagnostics.rejected_profiles;return false;
    }
    diagnostics.automatic=false;diagnostics.requested=profile;
    return switch_profile(profile,profile==NOVA_RENDER_QUALITY_SAFE?
        NOVA_QUALITY_REASON_SAFE_MODE:NOVA_QUALITY_REASON_USER);
}

bool nova_render_quality_set_auto(uint64_t memory,bool firmware_limited,
                                  bool software_renderer)
{
    if(!diagnostics.initialized)return false;
    diagnostics.automatic=true;diagnostics.software_renderer=software_renderer;
    diagnostics.observed_memory=memory;diagnostics.requested=NOVA_RENDER_QUALITY_STANDARD;
    return switch_profile(hardware_profile(memory,firmware_limited,software_renderer),
                          NOVA_QUALITY_REASON_HARDWARE);
}

nova_render_quality_t nova_render_quality_get(void){return diagnostics.active;}
const nova_render_quality_parameters_t *nova_render_quality_parameters(void)
{return diagnostics.initialized?&profiles[diagnostics.active]:
    &profiles[NOVA_RENDER_QUALITY_STANDARD];}

void nova_render_quality_report_frame(uint32_t frame_time_us)
{
    if(!diagnostics.initialized)return;
    diagnostics.last_frame_us=frame_time_us;
    if(frame_time_us<=33334u){consecutive_slow_frames=0;return;}
    ++diagnostics.frame_budget_violations;
    if(consecutive_slow_frames<3)++consecutive_slow_frames;
    if(diagnostics.automatic&&consecutive_slow_frames>=3&&
       diagnostics.active<NOVA_RENDER_QUALITY_SAFE){
        ++diagnostics.automatic_adjustments;consecutive_slow_frames=0;
        (void)switch_profile((nova_render_quality_t)(diagnostics.active+1),
                             NOVA_QUALITY_REASON_FRAME_BUDGET);
    }
}

void nova_render_quality_report_memory(uint64_t memory)
{
    if(!diagnostics.initialized)return;
    diagnostics.observed_memory=memory;
    if(!diagnostics.automatic)return;
    nova_render_quality_t target=memory<16u*MIB?NOVA_RENDER_QUALITY_SAFE:
        memory<32u*MIB?NOVA_RENDER_QUALITY_PERFORMANCE:diagnostics.active;
    if(target>diagnostics.active){++diagnostics.memory_degradations;
        ++diagnostics.automatic_adjustments;
        (void)switch_profile(target,NOVA_QUALITY_REASON_MEMORY);}
}

void nova_render_quality_report_graphics_error(bool critical)
{
    if(!diagnostics.initialized)return;
    ++diagnostics.graphics_degradations;
    nova_render_quality_t target=critical?NOVA_RENDER_QUALITY_SAFE:
        diagnostics.active<NOVA_RENDER_QUALITY_PERFORMANCE?
            NOVA_RENDER_QUALITY_PERFORMANCE:diagnostics.active;
    if(target!=diagnostics.active){++diagnostics.automatic_adjustments;
        (void)switch_profile(target,NOVA_QUALITY_REASON_GRAPHICS_ERROR);}
}

const nova_render_quality_diagnostics_t *nova_render_quality_diagnostics(void)
{return &diagnostics;}
