#include "diagnostics.h"
#include "compositor.h"
#include "input.h"
#include "motion.h"
#include "resources.h"
#include "icons.h"
#include "memory.h"
#include "render_quality.h"

static nova_diag_event_t events[NOVA_DIAG_CAPACITY];
static uint16_t event_head, event_count;
static nova_render_statistics_t render_stats;
static nova_frame_budget_t frame_budget;
static nova_quality_status_t quality;
static nova_diag_statistics_t stats;
static nova_boot_perf_metrics_t perf;
static nova_boot_startup_metrics_t startup;
static uint64_t frame_time_total;

void nova_diag_initialize(void)
{
    event_head=event_count=0;render_stats=(nova_render_statistics_t){0};
    frame_budget=(nova_frame_budget_t){0};stats=(nova_diag_statistics_t){0};
    (void)nova_memory_budget_initialize();
    (void)nova_memory_budget_report(NOVA_MEMORY_AREA_RENDERING,
        3ull*NOVA_SURFACE_WIDTH*NOVA_SURFACE_HEIGHT*4ull);
    quality=(nova_quality_status_t){NOVA_QUALITY_STANDARD,true,false,true,true,30};
    (void)nova_boot_perf_initialize();
}

void nova_diag_log(nova_diag_event_t event)
{
    uint16_t index=(uint16_t)((event_head+event_count)%NOVA_DIAG_CAPACITY);
    if(event_count==NOVA_DIAG_CAPACITY){index=event_head;event_head=(uint16_t)((event_head+1)%NOVA_DIAG_CAPACITY);++stats.overwritten;}
    else ++event_count;
    events[index]=event;++stats.events;
    if(event.level==NOVA_DIAG_WARNING)++stats.warnings;
    if(event.level>=NOVA_DIAG_ERROR)++stats.errors;
}

const nova_diag_event_t *nova_diag_get(uint32_t index)
{ if(index>=event_count)return 0;return &events[(event_head+index)%NOVA_DIAG_CAPACITY]; }

void nova_diag_frame(uint32_t frame_us,uint32_t layout_us,uint32_t animation_us,
                     uint32_t render_us,uint32_t compositor_us)
{nova_diag_frame_extended(frame_us,0,0,layout_us,animation_us,0,render_us,
                          compositor_us,0);}

void nova_diag_frame_extended(uint32_t frame_us,uint32_t input_us,uint32_t event_us,
    uint32_t layout_us,uint32_t animation_us,uint32_t text_us,uint32_t render_us,
    uint32_t compositor_us,uint32_t presentation_us)
{
    frame_budget=(nova_frame_budget_t){frame_us,layout_us,animation_us,render_us,
                                       compositor_us,0,frame_budget.violations};
    bool phase_exceeded=input_us>500u||event_us>500u||layout_us>2000u||
        animation_us>2000u||text_us>1500u||render_us>6000u||
        compositor_us>2000u||presentation_us>1500u;
    bool frame_exceeded=frame_us>16667u;
    if(frame_exceeded||phase_exceeded){++frame_budget.violations;
        ++perf.optimization_actions;quality.performance_limited=true;}
    if(phase_exceeded)++perf.phase_violations;
    if(frame_us>20000u)++perf.soft_violations;
    if(frame_us>33334u)++perf.hard_violations;
    nova_render_quality_report_frame(frame_us);
    if(nova_render_quality_diagnostics()->initialized){
        quality.quality=(nova_quality_t)nova_render_quality_get();
        quality.low_end=quality.quality>=NOVA_QUALITY_PERFORMANCE;
        quality.target_fps=quality.low_end?30:60;
    }
    render_stats.frame_time_us=frame_us;render_stats.layout_time_us=layout_us;
    render_stats.render_time_us=render_us;render_stats.compositor_time_us=compositor_us;
    ++render_stats.frame_count;render_stats.fps=frame_us?1000000u/frame_us:0;
    perf=(nova_boot_perf_metrics_t){.fps=render_stats.fps,.frame_time_us=frame_us,
        .draw_calls=render_stats.draw_calls,.dirty_regions=render_stats.dirty_regions,
        .memory_usage=nova_memory_budget_status()->used_memory,.input_time_us=input_us,
        .event_time_us=event_us,.layout_time_us=layout_us,.animation_time_us=animation_us,
        .text_time_us=text_us,.render_time_us=render_us,.compositor_time_us=compositor_us,
        .presentation_time_us=presentation_us,.minimum_frame_us=perf.minimum_frame_us,
        .maximum_frame_us=perf.maximum_frame_us,.soft_violations=perf.soft_violations,
        .hard_violations=perf.hard_violations,.phase_violations=perf.phase_violations,
        .optimization_actions=perf.optimization_actions,.software_renderer=quality.software_renderer,
        .within_budget=!frame_exceeded&&!phase_exceeded};
    if(!perf.minimum_frame_us||frame_us<perf.minimum_frame_us)perf.minimum_frame_us=frame_us;
    if(frame_us>perf.maximum_frame_us)perf.maximum_frame_us=frame_us;
    frame_time_total+=frame_us;perf.average_frame_us=render_stats.frame_count?
        (uint32_t)(frame_time_total/render_stats.frame_count):0;
    perf.frame_class=frame_us<=16667u?NOVA_FRAME_CLASS_A:frame_us<=20000u?
        NOVA_FRAME_CLASS_B:frame_us<=25000u?NOVA_FRAME_CLASS_C:frame_us<=33334u?
        NOVA_FRAME_CLASS_D:NOVA_FRAME_CLASS_E;
    nova_motion_budget_update(frame_us,animation_us);
}

void nova_diag_snapshot(void)
{
    const nova_compositor_diagnostics_t *c=nova_compositor_diagnostics();
    const nova_input_diagnostics_t *i=nova_input_diagnostics();
    const nova_motion_diagnostics_t *m=nova_motion_diagnostics();
    const nova_resource_diagnostics_t *r=nova_resource_diagnostics();
    const nova_icon_diagnostics_t *icons=nova_icon_diagnostics();
    render_stats.draw_calls=c->submitted;render_stats.dirty_regions=c->composed_regions;
    render_stats.rendered_icons=icons->renders;
    (void)nova_memory_budget_report(NOVA_MEMORY_AREA_RESOURCE_CACHE,r->cached_bytes);
    const nova_memory_budget_t *budget=nova_memory_budget_status();
    nova_render_quality_report_memory(budget->free_memory);
    stats.input_events=i->dispatched;stats.animation_events=m->completed+m->cancelled;
    stats.resource_events=r->loaded+r->releases+r->integrity_errors;
    perf.memory_usage=budget->used_memory;perf.draw_calls=render_stats.draw_calls;
    perf.dirty_regions=render_stats.dirty_regions;
}

bool nova_boot_perf_initialize(void)
{perf=(nova_boot_perf_metrics_t){.within_budget=true};startup=(nova_boot_startup_metrics_t){0};
 frame_time_total=0;return true;}
const nova_boot_perf_metrics_t *nova_boot_perf_metrics(void){return &perf;}
bool nova_boot_perf_reset(void){return nova_boot_perf_initialize();}
bool nova_boot_perf_record_startup(uint32_t ui,uint32_t theme,uint32_t resources,
    uint32_t window,uint32_t first_frame)
{startup=(nova_boot_startup_metrics_t){ui,theme,resources,window,first_frame,
    ui<=30000u&&theme<=10000u&&resources<=20000u&&window<=50000u&&first_frame<=100000u};
 return startup.valid;}
const nova_boot_startup_metrics_t *nova_boot_perf_startup(void){return &startup;}
bool nova_frame_budget_exceeded(void){return !perf.within_budget;}

void nova_diag_set_quality(nova_quality_t selected,bool automatic)
{
    if(selected>NOVA_QUALITY_AUTO)selected=NOVA_QUALITY_SAFE;
    quality.automatic=automatic;
    if(nova_render_quality_diagnostics()->initialized){
        if(automatic||selected==NOVA_QUALITY_AUTO)
            (void)nova_render_quality_set_auto(nova_memory_budget_status()->free_memory,false,
                                               quality.software_renderer);
        else (void)nova_render_quality_set((nova_render_quality_t)selected);
        quality.quality=(nova_quality_t)nova_render_quality_get();
    }else quality.quality=selected==NOVA_QUALITY_AUTO?NOVA_QUALITY_STANDARD:selected;
    quality.low_end=quality.quality>=NOVA_QUALITY_PERFORMANCE;
    quality.target_fps=quality.low_end?30:60;
}

const nova_render_statistics_t *nova_diag_render_statistics(void){return &render_stats;}
const nova_memory_budget_t *nova_diag_memory_budget(void){return nova_memory_budget_status();}
const nova_frame_budget_t *nova_diag_frame_budget(void){return &frame_budget;}
const nova_quality_status_t *nova_diag_quality(void){return &quality;}
const nova_diag_statistics_t *nova_diag_statistics(void){return &stats;}
