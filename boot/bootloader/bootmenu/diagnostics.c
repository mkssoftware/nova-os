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
static nova_memory_budget_t memory_budget;
static nova_frame_budget_t frame_budget;
static nova_quality_status_t quality;
static nova_diag_statistics_t stats;

void nova_diag_initialize(void)
{
    event_head=event_count=0;render_stats=(nova_render_statistics_t){0};
    frame_budget=(nova_frame_budget_t){0};stats=(nova_diag_statistics_t){0};
    memory_budget=(nova_memory_budget_t){64ull*1024*1024,
        3ull*NOVA_SURFACE_WIDTH*NOVA_SURFACE_HEIGHT*4ull,0,0,0,0};
    memory_budget.free_memory=memory_budget.total_budget-memory_budget.used_memory;
    memory_budget.pool_memory=memory_budget.used_memory;memory_budget.peak_memory=memory_budget.used_memory;
    quality=(nova_quality_status_t){NOVA_QUALITY_STANDARD,true,false,true,true,30};
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
{
    frame_budget=(nova_frame_budget_t){frame_us,layout_us,animation_us,render_us,
                                       compositor_us,0,frame_budget.violations};
    if(frame_us>33334u){++frame_budget.violations;quality.performance_limited=true;}
    nova_render_quality_report_frame(frame_us);
    if(nova_render_quality_diagnostics()->initialized){
        quality.quality=(nova_quality_t)nova_render_quality_get();
        quality.low_end=quality.quality>=NOVA_QUALITY_PERFORMANCE;
        quality.target_fps=quality.low_end?30:60;
    }
    render_stats.frame_time_us=frame_us;render_stats.layout_time_us=layout_us;
    render_stats.render_time_us=render_us;render_stats.compositor_time_us=compositor_us;
    ++render_stats.frame_count;render_stats.fps=frame_us?1000000u/frame_us:0;
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
    memory_budget.cache_memory=r->cached_bytes;
    const nova_memory_statistics_t *memory=nova_memory_statistics();
    memory_budget.pool_memory=memory->total_used;
    memory_budget.used_memory=3ull*NOVA_SURFACE_WIDTH*NOVA_SURFACE_HEIGHT*4ull+
                              memory->total_used+r->cached_bytes;
    memory_budget.free_memory=memory_budget.used_memory<memory_budget.total_budget?
        memory_budget.total_budget-memory_budget.used_memory:0;
    if(memory_budget.used_memory>memory_budget.peak_memory)
        memory_budget.peak_memory=memory_budget.used_memory;
    nova_render_quality_report_memory(memory_budget.free_memory);
    stats.input_events=i->dispatched;stats.animation_events=m->completed+m->cancelled;
    stats.resource_events=r->loaded+r->releases+r->integrity_errors;
}

void nova_diag_set_quality(nova_quality_t selected,bool automatic)
{
    if(selected>NOVA_QUALITY_AUTO)selected=NOVA_QUALITY_SAFE;
    quality.automatic=automatic;
    if(nova_render_quality_diagnostics()->initialized){
        if(automatic||selected==NOVA_QUALITY_AUTO)
            (void)nova_render_quality_set_auto(memory_budget.free_memory,false,
                                               quality.software_renderer);
        else (void)nova_render_quality_set((nova_render_quality_t)selected);
        quality.quality=(nova_quality_t)nova_render_quality_get();
    }else quality.quality=selected==NOVA_QUALITY_AUTO?NOVA_QUALITY_STANDARD:selected;
    quality.low_end=quality.quality>=NOVA_QUALITY_PERFORMANCE;
    quality.target_fps=quality.low_end?30:60;
}

const nova_render_statistics_t *nova_diag_render_statistics(void){return &render_stats;}
const nova_memory_budget_t *nova_diag_memory_budget(void){return &memory_budget;}
const nova_frame_budget_t *nova_diag_frame_budget(void){return &frame_budget;}
const nova_quality_status_t *nova_diag_quality(void){return &quality;}
const nova_diag_statistics_t *nova_diag_statistics(void){return &stats;}
