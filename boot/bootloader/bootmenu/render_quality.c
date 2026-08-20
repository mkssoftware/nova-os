#include "render_quality.h"
#include "compositor.h"
#include "dirty_manager.h"
#include "motion.h"
#include "resources.h"
#include "memory.h"
#include "theme.h"

#define MIB (1024u*1024u)

static const nova_render_quality_parameters_t profiles[NOVA_RENDER_QUALITY_COUNT]={
    {NOVA_BLUR_QUALITY_HIGH,NOVA_EFFECT_QUALITY_HIGH,NOVA_ROUNDED_AA_HIGH,
     NOVA_IMAGE_SAMPLE_BILINEAR,64u*MIB,32,8,100,100,1,100,12u*MIB,0,true,true,true,true,true,true},
    {NOVA_BLUR_QUALITY_HIGH,NOVA_EFFECT_QUALITY_HIGH,NOVA_ROUNDED_AA_HIGH,
     NOVA_IMAGE_SAMPLE_BILINEAR,48u*MIB,24,6,100,100,1,100,8u*MIB,0,true,true,true,true,true,true},
    {NOVA_BLUR_QUALITY_STANDARD,NOVA_EFFECT_QUALITY_STANDARD,NOVA_ROUNDED_AA_STANDARD,
     NOVA_IMAGE_SAMPLE_BILINEAR,32u*MIB,16,4,75,75,2,75,6u*MIB,0,true,true,true,true,true,false},
    {NOVA_BLUR_QUALITY_LOW,NOVA_EFFECT_QUALITY_LOW,NOVA_ROUNDED_AA_STANDARD,
     NOVA_IMAGE_SAMPLE_NEAREST,20u*MIB,4,1,50,50,3,50,6u*MIB,2,true,true,false,true,true,false},
    {NOVA_BLUR_QUALITY_LOW,NOVA_EFFECT_QUALITY_LOW,NOVA_ROUNDED_AA_DISABLED,
     NOVA_IMAGE_SAMPLE_NEAREST,12u*MIB,0,0,25,25,4,25,2u*MIB,3,false,false,false,false,false,false}
};
static nova_render_quality_diagnostics_t diagnostics;
static uint8_t consecutive_slow_frames;
static nova_quality_telemetry_t telemetry;
static nova_boot_quality_status_t public_status;
static uint16_t stable_fast_frames;
static nova_low_end_profile_t low_end;
static nova_high_quality_profile_t high_quality;

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

static nova_render_quality_t telemetry_profile(void)
{
    uint64_t pixels=(uint64_t)telemetry.width*telemetry.height;
    uint32_t cache_percent=telemetry.cache_budget?
        (uint32_t)(telemetry.cache_used*100u/telemetry.cache_budget):0;
    if(telemetry.firmware_limited||telemetry.available_memory<16u*MIB)
        return NOVA_RENDER_QUALITY_SAFE;
    if(!telemetry.gpu_available||telemetry.software_renderer||
       telemetry.available_memory<32u*MIB||telemetry.cpu_score<25)
        return NOVA_RENDER_QUALITY_PERFORMANCE;
    if(telemetry.frame_time_us>33334u||cache_percent>90u||pixels>3840ull*2160ull||
       telemetry.cpu_score<50)return NOVA_RENDER_QUALITY_STANDARD;
    if(telemetry.available_memory>=128u*MIB&&telemetry.cpu_score>=80&&
       telemetry.gpu_load_percent<70&&pixels<=2560ull*1440ull)
        return NOVA_RENDER_QUALITY_ULTRA;
    return NOVA_RENDER_QUALITY_HIGH;
}

static void refresh_public_status(void)
{
    public_status=(nova_boot_quality_status_t){
        .current_quality=(nova_boot_quality_t)diagnostics.active,
        .automatic=diagnostics.automatic,
        .performance_limited=diagnostics.active>=NOVA_RENDER_QUALITY_PERFORMANCE,
        .state_preserved=true,.security_preserved=true,.last_reason=diagnostics.last_reason,
        .telemetry=telemetry,.parameters=profiles[diagnostics.active]};
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
    nova_motion_set_reduced(nova_theme_reduced_motion()||!profile->animations_enabled);
    nova_compositor_set_fallback(profile->compositor_fallback);
    nova_dirty_force_full(NOVA_DIRTY_FULL_FORCED);
    ++diagnostics.applications;++diagnostics.cache_invalidations;
    ++diagnostics.full_redraws;
    ++diagnostics.atomic_generation;
    diagnostics.disabled_effects=(profile->blur_enabled?0u:1u)+
        (profile->shadows_enabled?0u:1u)+(profile->glows_enabled?0u:1u)+
        (profile->transparency_enabled?0u:1u)+(profile->animations_enabled?0u:1u);
    if(nova_resource_diagnostics()->initialized){
        (void)nova_resource_cache_collect();
        if(nova_resource_cache_configure(0,profile->resource_cache_budget,0,
                                         profile->resource_cache_budget))
            ++diagnostics.cache_reconfigurations;
    }
    refresh_public_status();return true;
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
    stable_fast_frames=0;telemetry=(nova_quality_telemetry_t){.width=1024,.height=768,
        .available_memory=memory,.cpu_score=50,.gpu_available=!software_renderer,
        .software_renderer=software_renderer};
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
    telemetry.frame_time_us=frame_time_us;++diagnostics.telemetry_updates;
    if(frame_time_us<=16667u){consecutive_slow_frames=0;
        if(diagnostics.automatic&&stable_fast_frames<120)++stable_fast_frames;
        if(diagnostics.automatic&&stable_fast_frames>=120&&diagnostics.active>telemetry_profile()){
            ++diagnostics.recoveries;++diagnostics.automatic_adjustments;stable_fast_frames=0;
            (void)switch_profile((nova_render_quality_t)(diagnostics.active-1),
                                 NOVA_QUALITY_REASON_FRAME_BUDGET);}
        return;}
    if(frame_time_us<=33334u){if(consecutive_slow_frames)++diagnostics.suppressed_spikes;
        consecutive_slow_frames=0;stable_fast_frames=0;return;}
    stable_fast_frames=0;
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
    diagnostics.observed_memory=memory;telemetry.available_memory=memory;
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

bool nova_render_quality_observe(const nova_quality_telemetry_t *observation)
{
    if(!diagnostics.initialized||!observation||observation->cpu_score>100||
       observation->gpu_load_percent>100||!observation->width||!observation->height)return false;
    telemetry=*observation;diagnostics.observed_memory=observation->available_memory;
    diagnostics.software_renderer=observation->software_renderer;
    ++diagnostics.telemetry_updates;refresh_public_status();return true;
}

bool nova_render_quality_update(void)
{
    if(!diagnostics.initialized)return false;
    if(!diagnostics.automatic){refresh_public_status();return true;}
    nova_render_quality_t target=telemetry_profile();
    if(target>diagnostics.active){
        ++diagnostics.automatic_adjustments;
        nova_quality_change_reason_t reason=telemetry.available_memory<32u*MIB?
            NOVA_QUALITY_REASON_MEMORY:!telemetry.gpu_available||telemetry.software_renderer?
            NOVA_QUALITY_REASON_GRAPHICS_ERROR:NOVA_QUALITY_REASON_FRAME_BUDGET;
        return switch_profile(target,reason);
    }
    refresh_public_status();return true;
}

bool nova_boot_quality_initialize(void)
{return nova_render_quality_initialize(false,64u*MIB)&&
    nova_render_quality_set_auto(64u*MIB,false,false);}
bool nova_boot_quality_set(nova_boot_quality_t quality)
{
    if(quality==NOVA_BOOT_QUALITY_AUTO)
        return nova_render_quality_set_auto(telemetry.available_memory,
            telemetry.firmware_limited,telemetry.software_renderer);
    return quality<=NOVA_BOOT_QUALITY_SAFE?
        nova_render_quality_set((nova_render_quality_t)quality):false;
}
const nova_boot_quality_status_t *nova_boot_quality_status(void)
{refresh_public_status();return &public_status;}
bool nova_boot_quality_update(void){return nova_render_quality_update();}

bool nova_low_end_profile_initialize(void)
{
    low_end=(nova_low_end_profile_t){.software_renderer=true,.reduced_effects=true,
        .reduced_animations=true,.dirty_regions=true,.double_buffered=true,
        .event_driven=true,.layout_preserved=true,.functionality_preserved=true,
        .security_preserved=true,.target_fps=30,.memory_budget=32ull*MIB};
    return true;
}

bool nova_low_end_profile_detect(uint8_t cores,uint32_t mhz,uint64_t memory,
    bool gpu_available,bool recovery_or_safe)
{
    if(!cores||!mhz)return false;
    low_end.automatic=true;
    if(recovery_or_safe||cores==1||mhz<=1000||memory<=32ull*MIB||!gpu_available){
        if(!nova_low_end_profile_enable())return false;
        if(recovery_or_safe)return nova_render_quality_set(NOVA_RENDER_QUALITY_SAFE);
        return true;
    }
    return true;
}

bool nova_low_end_profile_enable(void)
{
    if(!diagnostics.initialized)return false;
    if(!low_end.target_fps)(void)nova_low_end_profile_initialize();
    if(!nova_memory_budget_configure_low_end()||
       !nova_render_quality_set(NOVA_RENDER_QUALITY_PERFORMANCE))return false;
    nova_motion_set_reduced(true);
    if(nova_resource_diagnostics()->initialized){
        (void)nova_resource_cache_collect();
        (void)nova_resource_cache_configure(0,6ull*MIB,0,6ull*MIB);
    }
    if(!low_end.enabled)++low_end.activations;
    low_end.enabled=true;low_end.software_renderer=true;
    low_end.runtime_heap_locked=nova_memory_budget_status()->runtime_locked;
    low_end.memory_budget=nova_memory_budget_status()->total_budget;
    low_end.memory_used=nova_memory_budget_status()->used_memory;
    low_end.cache_used=nova_resource_diagnostics()->initialized?
        nova_resource_diagnostics()->cached_bytes:0;
    return low_end.memory_budget==32ull*MIB;
}

bool nova_low_end_profile_disable(void)
{
    if(!low_end.enabled)return false;
    low_end.enabled=false;++low_end.deactivations;
    (void)nova_memory_budget_configure(NOVA_MEMORY_PROFILE_STANDARD);
    if(diagnostics.initialized)(void)nova_render_quality_set_auto(64ull*MIB,false,
        diagnostics.software_renderer);
    return true;
}

bool nova_low_end_profile_update(uint32_t fps,uint32_t frame_us,uint64_t memory,
    uint64_t cache)
{
    if(!low_end.enabled)return false;
    low_end.last_fps=fps;low_end.last_frame_us=frame_us;
    low_end.memory_used=memory;low_end.cache_used=cache;
    low_end.runtime_heap_locked=nova_memory_budget_status()->runtime_locked;
    if(frame_us>33334u)nova_render_quality_report_frame(frame_us);
    if(memory>low_end.memory_budget)nova_render_quality_report_memory(0);
    return true;
}
const nova_low_end_profile_t *nova_low_end_profile_status(void){return &low_end;}

bool nova_high_quality_profile_initialize(void)
{
    bool backend=high_quality.gpu_backend_available;
    high_quality=(nova_high_quality_profile_t){.gpu_backend_available=backend,
        .advanced_materials=true,.high_quality_shadows=true,.advanced_blur=true,
        .layer_caching=true,.dirty_regions=true,.double_buffered=true,
        .software_fallback_available=true,.layout_preserved=true,
        .functionality_preserved=true,.security_preserved=true,.target_fps=60,
        .memory_budget=256ull*MIB};
    return true;
}
void nova_high_quality_profile_set_gpu_backend(bool available)
{high_quality.gpu_backend_available=available;}
bool nova_high_quality_profile_detect(uint8_t cores,uint64_t system_memory,
    uint64_t ui_memory,bool gpu,bool fast_storage)
{
    if(!cores)return false;
    high_quality.automatic=true;
    if(cores>=4&&system_memory>=8ull*1024u*MIB&&ui_memory>=256ull*MIB&&gpu&&fast_storage)
        return nova_high_quality_profile_enable();
    return true;
}
bool nova_high_quality_profile_enable(void)
{
    if(!diagnostics.initialized||!high_quality.gpu_backend_available)return false;
    if(low_end.enabled&&!nova_low_end_profile_disable())return false;
    if(!nova_memory_budget_configure_high_quality()||
       !nova_render_quality_set(NOVA_RENDER_QUALITY_ULTRA))return false;
    if(nova_resource_diagnostics()->initialized){
        (void)nova_resource_cache_configure(0,64ull*MIB,0,64ull*MIB);
    }
    if(!high_quality.enabled)++high_quality.activations;
    high_quality.enabled=true;high_quality.gpu_acceleration=true;
    high_quality.fallback_active=false;
    high_quality.runtime_heap_locked=nova_memory_budget_status()->runtime_locked;
    high_quality.memory_budget=nova_memory_budget_status()->total_budget;
    high_quality.memory_used=nova_memory_budget_status()->used_memory;
    high_quality.cache_used=nova_resource_diagnostics()->initialized?
        nova_resource_diagnostics()->cached_bytes:0;
    return high_quality.memory_budget==256ull*MIB;
}
bool nova_high_quality_profile_disable(void)
{
    if(!high_quality.enabled)return false;
    high_quality.enabled=false;high_quality.gpu_acceleration=false;
    ++high_quality.deactivations;
    (void)nova_memory_budget_configure(NOVA_MEMORY_PROFILE_STANDARD);
    if(diagnostics.initialized)(void)nova_render_quality_set_auto(64ull*MIB,false,
        diagnostics.software_renderer);
    return true;
}
bool nova_high_quality_profile_report_gpu_failure(void)
{
    if(!high_quality.enabled)return false;
    ++high_quality.gpu_failures;high_quality.fallback_active=true;
    high_quality.gpu_backend_available=false;high_quality.gpu_acceleration=false;
    high_quality.enabled=false;++high_quality.deactivations;
    (void)nova_memory_budget_configure(NOVA_MEMORY_PROFILE_STANDARD);
    nova_render_quality_report_graphics_error(false);
    return high_quality.software_fallback_available;
}
bool nova_high_quality_profile_update(uint32_t fps,uint32_t frame_us,uint64_t memory,
    uint64_t cache,uint8_t gpu_load)
{
    if(!high_quality.enabled||gpu_load>100)return false;
    high_quality.last_fps=fps;high_quality.last_frame_us=frame_us;
    high_quality.memory_used=memory;high_quality.cache_used=cache;
    high_quality.gpu_load_percent=gpu_load;
    high_quality.runtime_heap_locked=nova_memory_budget_status()->runtime_locked;
    if(frame_us>16667u)nova_render_quality_report_frame(frame_us);
    return true;
}
const nova_high_quality_profile_t *nova_high_quality_profile_status(void)
{return &high_quality;}
