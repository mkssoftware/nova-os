#include "diagnostics.h"
#include "compositor.h"
#include "input.h"
#include "motion.h"
#include "resources.h"
#include "icons.h"
#include "memory.h"
#include "render_quality.h"
#include "render_queue.h"
#include "dirty_manager.h"
#include "layer_manager.h"
#include "effects.h"
#include "background_blur.h"
#include "image_renderer.h"
#include "framebuffer_backend.h"
#include "theme.h"
#include "font_resources.h"
#include "icons.h"
#include "animation_resources.h"
#include "software_renderer.h"
#include "recovery.h"

static nova_diag_event_t events[NOVA_DIAG_CAPACITY];
static nova_diag_record_t records[NOVA_DIAG_CAPACITY];
static uint16_t event_head, event_count;
static uint16_t effective_capacity=NOVA_DIAG_CAPACITY;
static uint64_t next_sequence;
static nova_diag_framework_status_t framework;
static struct {uint32_t category;char name[24];} modules[NOVA_DIAG_MODULE_CAPACITY];
static nova_render_statistics_t render_stats;
static nova_render_statistics_t render_average;
static nova_performance_regression_result_t regression_results[NOVA_BENCH_COUNT];
static nova_performance_regression_status_t regression_status;
static nova_fallback_test_result_t fallback_results[NOVA_FALLBACK_MODE_COUNT];
static nova_fallback_test_status_t fallback_status;
static nova_render_frame_record_t render_frames[NOVA_RENDER_FRAME_HISTORY];
static uint16_t render_frame_head,render_frame_count;
static uint64_t render_clock_us,render_frame_total_us,render_draw_total;
static nova_frame_budget_t frame_budget;
static nova_quality_status_t quality;
static nova_diag_statistics_t stats;
static nova_boot_perf_metrics_t perf;
static nova_boot_startup_metrics_t startup;
static uint64_t frame_time_total;
static nova_boot_time_budget_t boot_phases[NOVA_BOOT_PHASE_COUNT];
static nova_boot_budget_diagnostics_t boot_budget;
static nova_boot_clock_t boot_clock;

static void copy_text(char *target,uint32_t capacity,const char *source)
{
    uint32_t i=0;if(!capacity)return;
    if(source)for(;i+1<capacity&&source[i];++i)target[i]=source[i];
    target[i]=0;
}

static bool text_equal(const char *a,const char *b)
{if(!a||!b)return false;while(*a&&*b&&*a==*b){++a;++b;}return *a==*b;}

static char lower_ascii(char value)
{return value>='A'&&value<='Z'?(char)(value-'A'+'a'):value;}
static bool contains_text(const char *text,const char *needle)
{
    if(!text||!needle||!needle[0])return false;
    for(;*text;++text){
        uint32_t i=0;
        while(needle[i]&&text[i]&&lower_ascii(text[i])==lower_ascii(needle[i]))++i;
        if(!needle[i])return true;
    }
    return false;
}
static bool contains_secret(const char *text)
{return contains_text(text,"password")||contains_text(text,"private-key")||
 contains_text(text,"recovery-secret")||contains_text(text,"secret")||
 contains_text(text,"token=");}

static uint16_t physical_index(uint32_t chronological)
{return (uint16_t)((event_head+chronological)%effective_capacity);}

static bool filter_matches(const nova_diag_record_t *record,const nova_diag_filter_t *filter)
{
    if(!filter)return true;
    if(filter->use_time&&(record->event.timestamp_us<filter->from_us||
       record->event.timestamp_us>filter->to_us))return false;
    if(filter->use_level&&(record->event.level<filter->minimum_level||
       record->event.level>filter->maximum_level))return false;
    if(filter->use_phase&&record->phase!=filter->phase)return false;
    if(filter->use_category&&record->event.category!=filter->category)return false;
    if(filter->use_error_class&&record->error_class!=filter->error_class)return false;
    if(filter->use_module&&!text_equal(record->module,filter->module))return false;
    return true;
}

static bool append_byte(uint8_t *output,uint32_t capacity,uint32_t *position,uint8_t value)
{if(*position>=capacity)return false;output[(*position)++]=value;return true;}
static bool append_text(uint8_t *output,uint32_t capacity,uint32_t *position,const char *value)
{if(!value)return true;while(*value)if(!append_byte(output,capacity,position,(uint8_t)*value++))return false;return true;}
static bool append_json_text(uint8_t *output,uint32_t capacity,uint32_t *position,const char *value)
{
    if(!value)return true;
    while(*value){uint8_t c=(uint8_t)*value++;
        if(c=='"'||c=='\\'){if(!append_byte(output,capacity,position,'\\'))return false;}
        else if(c<' '){if(!append_text(output,capacity,position,"?"))return false;continue;}
        if(!append_byte(output,capacity,position,c))return false;}
    return true;
}
static bool append_u64(uint8_t *output,uint32_t capacity,uint32_t *position,uint64_t value)
{
    char digits[21];uint8_t count=0;if(!value)return append_byte(output,capacity,position,'0');
    while(value&&count<sizeof(digits)){digits[count++]=(char)('0'+value%10u);value/=10u;}
    while(count)if(!append_byte(output,capacity,position,(uint8_t)digits[--count]))return false;
    return true;
}

static const uint64_t phase_budgets[NOVA_BOOT_PHASE_COUNT]={
    50000,150000,100000,100000,80000,20000,30000,50000,300000,20000
};

static void refresh_boot_budget(void)
{
    uint64_t first=UINT64_MAX,last=0;uint32_t complete=0,violations=0;
    for(uint8_t i=0;i<NOVA_BOOT_PHASE_COUNT;++i)if(boot_phases[i].complete){
        ++complete;if(boot_phases[i].start_time_us<first)first=boot_phases[i].start_time_us;
        if(boot_phases[i].end_time_us>last)last=boot_phases[i].end_time_us;
        violations+=boot_phases[i].violations;
    }
    boot_budget.completed_phases=complete;boot_budget.violations=violations;
    boot_budget.optimization_hints=violations;
    boot_budget.total_time_us=complete&&first!=UINT64_MAX?last-first:0;
    uint64_t target=boot_budget.start_type==NOVA_BOOT_START_STANDARD?1000000u:
        boot_budget.start_type==NOVA_BOOT_START_RECOVERY?2000000u:
        boot_budget.start_type==NOVA_BOOT_START_DIAGNOSTIC?3000000u:UINT64_MAX;
    boot_budget.total_budget_us=target;
    uint64_t total=boot_budget.total_time_us;
    boot_budget.performance_class=total<=1000000u?NOVA_BOOT_TIME_CLASS_A:
        total<=1500000u?NOVA_BOOT_TIME_CLASS_B:total<=2000000u?NOVA_BOOT_TIME_CLASS_C:
        total<=3000000u?NOVA_BOOT_TIME_CLASS_D:NOVA_BOOT_TIME_CLASS_E;
    boot_budget.within_budget=!violations&&(target==UINT64_MAX||total<=target)&&
        (!boot_budget.first_frame_valid||boot_budget.ui_to_first_frame_us<=50000u);
}

bool nova_boot_budget_initialize(void)
{
    for(uint8_t i=0;i<NOVA_BOOT_PHASE_COUNT;++i)
        boot_phases[i]=(nova_boot_time_budget_t){.phase=(nova_boot_phase_t)i,
            .budget_us=phase_budgets[i]};
    boot_budget=(nova_boot_budget_diagnostics_t){.start_type=NOVA_BOOT_START_STANDARD,
        .security_checks_preserved=true,.within_budget=true,.clock_available=boot_clock!=0};
    refresh_boot_budget();return true;
}
void nova_boot_budget_set_clock(nova_boot_clock_t clock)
{boot_clock=clock;boot_budget.clock_available=clock!=0;}
bool nova_boot_budget_set_start_type(nova_boot_start_type_t type,uint8_t hardware_class)
{if(type>NOVA_BOOT_START_DIAGNOSTIC)return false;boot_budget.start_type=type;
 boot_budget.hardware_class=hardware_class;refresh_boot_budget();return true;}
bool nova_boot_budget_begin_at(nova_boot_phase_t phase,uint64_t now)
{
    if(phase>=NOVA_BOOT_PHASE_COUNT||boot_phases[phase].started)return false;
    boot_phases[phase].start_time_us=now;boot_phases[phase].started=true;return true;
}
bool nova_boot_budget_end_at(nova_boot_phase_t phase,uint64_t now)
{
    if(phase>=NOVA_BOOT_PHASE_COUNT||!boot_phases[phase].started||
       boot_phases[phase].complete||now<boot_phases[phase].start_time_us)return false;
    nova_boot_time_budget_t *record=&boot_phases[phase];record->end_time_us=now;
    record->duration_us=now-record->start_time_us;record->complete=true;
    record->exceeded=record->duration_us>record->budget_us;
    if(record->exceeded){++record->violations;nova_diag_log((nova_diag_event_t){now,
        NOVA_DIAG_WARNING,0x50455246u,0x100u+(uint32_t)phase,
        (uint32_t)record->duration_us,(uint32_t)record->budget_us});}
    refresh_boot_budget();return true;
}
bool nova_boot_budget_begin(nova_boot_phase_t phase)
{return boot_clock?nova_boot_budget_begin_at(phase,boot_clock()):false;}
bool nova_boot_budget_end(nova_boot_phase_t phase)
{return boot_clock?nova_boot_budget_end_at(phase,boot_clock()):false;}
bool nova_boot_budget_first_frame(uint64_t now)
{
    if(!boot_phases[NOVA_BOOT_PHASE_UI].complete||
       now<boot_phases[NOVA_BOOT_PHASE_UI].end_time_us)return false;
    boot_budget.ui_to_first_frame_us=now-boot_phases[NOVA_BOOT_PHASE_UI].end_time_us;
    boot_budget.first_frame_valid=true;refresh_boot_budget();return true;
}
const nova_boot_time_budget_t *nova_boot_budget_status(void){return boot_phases;}
const nova_boot_time_budget_t *nova_boot_budget_phase(nova_boot_phase_t phase)
{return phase<NOVA_BOOT_PHASE_COUNT?&boot_phases[phase]:0;}
const nova_boot_budget_diagnostics_t *nova_boot_budget_diagnostics(void){return &boot_budget;}

void nova_diag_initialize(void)
{
    event_head=event_count=0;effective_capacity=NOVA_DIAG_CAPACITY;next_sequence=1;
    framework=(nova_diag_framework_status_t){.capacity=NOVA_DIAG_CAPACITY,
        .registered_modules=NOVA_DIAG_AREA_COUNT,.initialized=true,.read_only=true,
        .boot_unaffected=true};
    static const char *const names[NOVA_DIAG_AREA_COUNT]={"Boot","Hardware","Firmware",
        "Rendering","Memory","Performance","Resources","Animation","Input","Network","Security"};
    for(uint16_t i=0;i<NOVA_DIAG_AREA_COUNT;++i){modules[i].category=i;
        copy_text(modules[i].name,sizeof(modules[i].name),names[i]);}
    (void)nova_render_statistics_initialize();
    frame_budget=(nova_frame_budget_t){0};stats=(nova_diag_statistics_t){0};
    (void)nova_memory_budget_initialize();
    (void)nova_memory_budget_report(NOVA_MEMORY_AREA_RENDERING,
        3ull*NOVA_SURFACE_WIDTH*NOVA_SURFACE_HEIGHT*4ull);
    quality=(nova_quality_status_t){NOVA_QUALITY_STANDARD,true,false,true,true,30};
    (void)nova_boot_perf_initialize();
    (void)nova_boot_budget_initialize();
}

void nova_diag_log(nova_diag_event_t event)
{
    uint16_t index=(uint16_t)((event_head+event_count)%effective_capacity);
    if(event_count==effective_capacity){index=event_head;event_head=(uint16_t)((event_head+1)%effective_capacity);++stats.overwritten;}
    else ++event_count;
    events[index]=event;records[index]=(nova_diag_record_t){.event=event,
        .sequence=next_sequence++,.phase=NOVA_DIAG_PHASE_ANY};
    ++stats.events;framework.count=event_count;
    if(event.level==NOVA_DIAG_WARNING)++stats.warnings;
    if(event.level>=NOVA_DIAG_ERROR)++stats.errors;
}

const nova_diag_event_t *nova_diag_get(uint32_t index)
{if(index>=event_count)return 0;return &events[physical_index(index)];}

bool nova_diag_configure_capacity(uint16_t capacity)
{if(!capacity||capacity>NOVA_DIAG_CAPACITY||event_count)return false;
 effective_capacity=capacity;framework.capacity=capacity;return true;}

bool nova_diag_register_module(uint32_t category,const char *name)
{
    if(!name||!name[0]||framework.registered_modules>=NOVA_DIAG_MODULE_CAPACITY)return false;
    for(uint16_t i=0;i<framework.registered_modules;++i)
        if(modules[i].category==category||text_equal(modules[i].name,name))return false;
    uint16_t index=framework.registered_modules++;modules[index].category=category;
    copy_text(modules[index].name,sizeof(modules[index].name),name);return true;
}

bool nova_diag_log_extended(const nova_diag_event_descriptor_t *descriptor)
{
    if(!descriptor||descriptor->event.level>NOVA_DIAG_CRITICAL)return false;
    nova_diag_log(descriptor->event);nova_diag_record_t *record=&records[physical_index(event_count-1)];
    record->phase=descriptor->phase;record->error_class=descriptor->error_class;
    copy_text(record->module,sizeof(record->module),descriptor->module);
    copy_text(record->source,sizeof(record->source),descriptor->source);
    bool sensitive=descriptor->sensitive||contains_secret(descriptor->message)||
        contains_secret(descriptor->metadata)||contains_secret(descriptor->cause);
    if(sensitive){copy_text(record->message,sizeof(record->message),"[redacted]");
        copy_text(record->metadata,sizeof(record->metadata),"[redacted]");
        copy_text(record->cause,sizeof(record->cause),"[redacted]");
        copy_text(record->recommendation,sizeof(record->recommendation),"[redacted]");
        record->redacted=true;
        ++framework.redactions;}
    else{copy_text(record->message,sizeof(record->message),descriptor->message);
        copy_text(record->metadata,sizeof(record->metadata),descriptor->metadata);
        copy_text(record->cause,sizeof(record->cause),descriptor->cause);
        copy_text(record->recommendation,sizeof(record->recommendation),descriptor->recommendation);}
    return true;
}

const nova_diag_record_t *nova_diag_get_record(uint32_t index)
{return index<event_count?&records[physical_index(index)]:0;}

const nova_diag_record_t *nova_diag_query(const nova_diag_filter_t *filter,uint32_t match)
{
    ++framework.filtered_queries;
    for(uint32_t i=0;i<event_count;++i){const nova_diag_record_t *record=nova_diag_get_record(i);
        if(filter_matches(record,filter)&&match--==0)return record;}
    return 0;
}

bool nova_diag_set_live(bool enabled,bool authorized)
{if(!authorized)return false;framework.live_enabled=enabled;return true;}

bool nova_diag_export(nova_diag_export_format_t format,bool authorized,uint8_t *output,
    uint32_t capacity,uint32_t *written)
{
    if(written)*written=0;
    if(!authorized){++framework.denied_exports;return false;}
    if(!output||!written||format>NOVA_DIAG_EXPORT_BINARY)return false;
    uint32_t p=0;
    if(format==NOVA_DIAG_EXPORT_BINARY){
        const uint8_t header[8]={'N','D','F','B',1,0,(uint8_t)(event_count&255u),(uint8_t)(event_count>>8)};
        for(uint8_t i=0;i<sizeof(header);++i)if(!append_byte(output,capacity,&p,header[i]))return false;
        for(uint32_t i=0;i<event_count;++i){const nova_diag_record_t *r=nova_diag_get_record(i);
            for(uint8_t b=0;b<8;++b)if(!append_byte(output,capacity,&p,(uint8_t)(r->sequence>>(b*8))))return false;
            for(uint8_t b=0;b<4;++b)if(!append_byte(output,capacity,&p,(uint8_t)(r->event.event_id>>(b*8))))return false;}
    }else{
        if(format==NOVA_DIAG_EXPORT_NDF&&!append_text(output,capacity,&p,"NDF/1\n"))return false;
        if(format==NOVA_DIAG_EXPORT_JSON&&!append_text(output,capacity,&p,"{\"format\":\"NovaDiag/1\",\"events\":["))return false;
        for(uint32_t i=0;i<event_count;++i){const nova_diag_record_t *r=nova_diag_get_record(i);
            if(format==NOVA_DIAG_EXPORT_JSON){if(i&&!append_byte(output,capacity,&p,','))return false;
                if(!append_text(output,capacity,&p,"{\"seq\":" )||!append_u64(output,capacity,&p,r->sequence)||
                   !append_text(output,capacity,&p,",\"id\":" )||!append_u64(output,capacity,&p,r->event.event_id)||
                   !append_text(output,capacity,&p,",\"message\":\"")||!append_json_text(output,capacity,&p,r->message)||
                   !append_text(output,capacity,&p,"\"}"))return false;
            }else{if(!append_u64(output,capacity,&p,r->sequence)||!append_byte(output,capacity,&p,'|')||
                !append_u64(output,capacity,&p,r->event.timestamp_us)||!append_byte(output,capacity,&p,'|')||
                !append_text(output,capacity,&p,r->module)||!append_byte(output,capacity,&p,'|')||
                !append_text(output,capacity,&p,r->message)||!append_byte(output,capacity,&p,'\n'))return false;}
        }
        if(format==NOVA_DIAG_EXPORT_JSON&&!append_text(output,capacity,&p,"]}"))return false;
    }
    *written=p;++framework.exports;return true;
}

void nova_diag_reset(void)
{event_head=event_count=0;next_sequence=1;framework.count=0;stats.events=stats.overwritten=
 stats.warnings=stats.errors=0;}
const nova_diag_framework_status_t *nova_diag_framework_status(void){return &framework;}

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
    render_stats.presentation_time_us=presentation_us;
    uint64_t busy=(uint64_t)input_us+event_us+layout_us+animation_us+text_us+
        render_us+compositor_us+presentation_us;
    render_stats.idle_time_us=busy<frame_us?(uint32_t)(frame_us-busy):0;
    ++render_stats.frame_count;render_stats.fps=frame_us?1000000u/frame_us:0;
    if(!render_stats.minimum_frame_us||frame_us<render_stats.minimum_frame_us)
        render_stats.minimum_frame_us=frame_us;
    if(frame_us>render_stats.maximum_frame_us)render_stats.maximum_frame_us=frame_us;
    render_frame_total_us+=frame_us;
    render_stats.average_frame_us=(uint32_t)(render_frame_total_us/render_stats.frame_count);
    uint16_t frame_index=(uint16_t)((render_frame_head+render_frame_count)%NOVA_RENDER_FRAME_HISTORY);
    if(render_frame_count==NOVA_RENDER_FRAME_HISTORY){frame_index=render_frame_head;
        render_frame_head=(uint16_t)((render_frame_head+1)%NOVA_RENDER_FRAME_HISTORY);}
    else ++render_frame_count;
    render_frames[frame_index]=(nova_render_frame_record_t){.frame_id=render_stats.frame_count,
        .start_us=render_clock_us,.end_us=render_clock_us+frame_us,.duration_us=frame_us};
    render_clock_us+=frame_us;
    perf=(nova_boot_perf_metrics_t){.fps=render_stats.fps,.frame_time_us=frame_us,
        .draw_calls=render_stats.draw_calls,.dirty_regions=render_stats.dirty_regions,
        .memory_usage=nova_memory_budget_status()->used_memory,
        .cache_usage=nova_resource_diagnostics()->cached_bytes,
        .cpu_score=nova_boot_quality_status()->telemetry.cpu_score,
        .gpu_load_percent=nova_boot_quality_status()->telemetry.gpu_load_percent,
        .gpu_available=nova_boot_quality_status()->telemetry.gpu_available,
        .input_time_us=input_us,
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
    nova_animation_diag_frame(frame_us,render_us,compositor_us,layout_us,
        quality.software_renderer,(uint8_t)quality.quality,
        nova_boot_quality_status()->telemetry.gpu_available?2u:
        quality.low_end?0u:1u);
}

void nova_diag_snapshot(void)
{
    const nova_compositor_diagnostics_t *c=nova_compositor_diagnostics();
    const nova_input_diagnostics_t *i=nova_input_diagnostics();
    const nova_motion_diagnostics_t *m=nova_motion_diagnostics();
    const nova_resource_diagnostics_t *r=nova_resource_diagnostics();
    const nova_icon_diagnostics_t *icons=nova_icon_diagnostics();
    const nova_render_diagnostics_t *commands=nova_render_diagnostics();
    const nova_dirty_diagnostics_t *dirty=nova_dirty_diagnostics();
    const nova_layer_manager_diagnostics_t *layers=nova_layer_manager_diagnostics();
    const nova_effect_diagnostics_t *effects=nova_effect_diagnostics();
    const nova_blur_diagnostics_t *blur=nova_blur_diagnostics();
    const nova_image_diagnostics_t *images=nova_image_diagnostics();
    render_stats.draw_calls=commands->executed?commands->executed:c->submitted;
    render_stats.dirty_regions=c->composed_regions;render_stats.layers=layers->active;
    render_stats.rendered_icons=icons->renders;render_stats.rendered_images=images->renders;
    render_stats.rectangles=nova_framebuffer_diagnostics()->rectangle_writes;
    render_stats.lines=nova_framebuffer_diagnostics()->line_writes;
    render_stats.shadows=effects->shadows;render_stats.blur_effects=blur->regions;
    render_stats.dirty_area=dirty->total_region_area;
    render_stats.maximum_dirty_area=dirty->maximum_region_area;
    render_stats.screen_area=(uint64_t)dirty->width*dirty->height;
    render_stats.dirty_ratio_permille=render_stats.screen_area?
        (uint32_t)((render_stats.dirty_area*1000u)/render_stats.screen_area):0;
    render_stats.resource_cache_hits=r->lookup_hits;
    render_stats.resource_cache_misses=r->lookup_misses;
    render_stats.resource_releases=r->releases;
    render_stats.active_animations=m->active;render_stats.completed_animations=m->completed;
    render_stats.dropped_animations=m->cancelled+m->rejected;
    render_stats.motion_tokens=m->active+m->completed+m->cancelled;
    (void)nova_memory_budget_report(NOVA_MEMORY_AREA_RESOURCE_CACHE,r->cached_bytes);
    const nova_memory_budget_t *budget=nova_memory_budget_status();
    render_stats.total_memory=budget->total_budget;render_stats.used_memory=budget->used_memory;
    render_stats.cache_memory=budget->cache_memory;render_stats.pool_memory=budget->pool_memory;
    render_stats.temporary_memory=budget->area_used[NOVA_MEMORY_AREA_RESERVE];
    render_stats.peak_memory=budget->peak_memory;
    render_stats.gpu_available=nova_boot_quality_status()->telemetry.gpu_available;
    render_stats.gpu_load_percent=nova_boot_quality_status()->telemetry.gpu_load_percent;
    render_stats.software_renderer=!render_stats.gpu_available;
    if(render_stats.draw_calls>render_stats.maximum_draw_calls)
        render_stats.maximum_draw_calls=render_stats.draw_calls;
    render_draw_total+=render_stats.draw_calls;
    render_stats.average_draw_calls=render_stats.frame_count?
        (uint32_t)(render_draw_total/render_stats.frame_count):0;
    if(render_frame_count){nova_render_frame_record_t *last=&render_frames[
        (render_frame_head+render_frame_count-1)%NOVA_RENDER_FRAME_HISTORY];
        last->draw_calls=render_stats.draw_calls;last->dirty_regions=render_stats.dirty_regions;
        last->layers=render_stats.layers;last->dirty_area=render_stats.dirty_area;}
    nova_render_quality_report_memory(budget->free_memory);
    stats.input_events=i->dispatched;stats.animation_events=m->completed+m->cancelled;
    stats.resource_events=r->loaded+r->releases+r->integrity_errors;
    perf.memory_usage=budget->used_memory;perf.draw_calls=render_stats.draw_calls;
    perf.cache_usage=r->cached_bytes;perf.draw_calls=render_stats.draw_calls;
    perf.dirty_regions=render_stats.dirty_regions;
    perf.cpu_score=nova_boot_quality_status()->telemetry.cpu_score;
    perf.gpu_load_percent=nova_boot_quality_status()->telemetry.gpu_load_percent;
    perf.gpu_available=nova_boot_quality_status()->telemetry.gpu_available;
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

bool nova_render_statistics_initialize(void)
{render_stats=(nova_render_statistics_t){0};render_average=(nova_render_statistics_t){0};
 render_frame_head=render_frame_count=0;render_clock_us=render_frame_total_us=render_draw_total=0;
 return true;}
void nova_render_statistics_reset(void){(void)nova_render_statistics_initialize();}
const nova_render_statistics_t *nova_render_statistics_current(void){return &render_stats;}
uint16_t nova_render_statistics_frame_count(void){return render_frame_count;}
const nova_render_frame_record_t *nova_render_statistics_frame(uint32_t index)
{return index<render_frame_count?&render_frames[(render_frame_head+index)%NOVA_RENDER_FRAME_HISTORY]:0;}
const nova_render_statistics_t *nova_render_statistics_average(nova_render_window_t window)
{
    if(window==NOVA_RENDER_WINDOW_CURRENT)return &render_stats;
    uint64_t span=window==NOVA_RENDER_WINDOW_SECOND?1000000u:
        window==NOVA_RENDER_WINDOW_TEN_SECONDS?10000000u:UINT64_MAX;
    uint64_t duration=0,draws=0,dirty_count=0,dirty_area=0;uint32_t count=0,max_frame=0,min_frame=0;
    for(uint32_t n=0;n<render_frame_count;++n){uint32_t i=render_frame_count-1-n;
        const nova_render_frame_record_t *frame=nova_render_statistics_frame(i);
        if(count&&duration>=span)break;
        duration+=frame->duration_us;++count;
        draws+=frame->draw_calls;dirty_count+=frame->dirty_regions;dirty_area+=frame->dirty_area;
        if(!min_frame||frame->duration_us<min_frame)min_frame=frame->duration_us;
        if(frame->duration_us>max_frame)max_frame=frame->duration_us;}
    render_average=render_stats;render_average.frame_count=count;
    render_average.frame_time_us=count?duration/count:0;
    render_average.average_frame_us=(uint32_t)render_average.frame_time_us;
    render_average.minimum_frame_us=min_frame;render_average.maximum_frame_us=max_frame;
    render_average.fps=duration?(uint32_t)((uint64_t)count*1000000u/duration):0;
    render_average.draw_calls=count?(uint32_t)(draws/count):0;
    render_average.dirty_regions=count?(uint32_t)(dirty_count/count):0;
    render_average.dirty_area=count?dirty_area/count:0;return &render_average;
}

bool nova_render_statistics_export(nova_render_export_format_t format,bool authorized,
    uint8_t *output,uint32_t capacity,uint32_t *written)
{
    if(written)*written=0;
    if(!authorized||!output||!written||format>NOVA_RENDER_EXPORT_BINARY)return false;
    uint32_t p=0;
    if(format==NOVA_RENDER_EXPORT_BINARY){const uint8_t h[8]={'N','R','S','B',1,0,
        (uint8_t)render_frame_count,(uint8_t)(render_frame_count>>8)};
        for(uint8_t i=0;i<8;++i)if(!append_byte(output,capacity,&p,h[i]))return false;
        for(uint32_t i=0;i<render_frame_count;++i){const nova_render_frame_record_t *f=nova_render_statistics_frame(i);
            for(uint8_t b=0;b<4;++b)if(!append_byte(output,capacity,&p,(uint8_t)(f->duration_us>>(b*8))))return false;}
    }else{
        const char *head=format==NOVA_RENDER_EXPORT_NDF?"NRS/1\n":format==NOVA_RENDER_EXPORT_CSV?
            "frame,start_us,end_us,duration_us,draw_calls,dirty_regions,layers,dirty_area\n":"{\"renderFrames\":[";
        if(!append_text(output,capacity,&p,head))return false;
        for(uint32_t i=0;i<render_frame_count;++i){const nova_render_frame_record_t *f=nova_render_statistics_frame(i);
            if(format==NOVA_RENDER_EXPORT_JSON){if(i&&!append_byte(output,capacity,&p,','))return false;
                if(!append_text(output,capacity,&p,"{\"id\":")||!append_u64(output,capacity,&p,f->frame_id)||
                   !append_text(output,capacity,&p,",\"duration_us\":")||!append_u64(output,capacity,&p,f->duration_us)||
                   !append_text(output,capacity,&p,"}"))return false;
            }else{uint64_t values[8]={f->frame_id,f->start_us,f->end_us,f->duration_us,
                    f->draw_calls,f->dirty_regions,f->layers,f->dirty_area};
                for(uint8_t v=0;v<8;++v){if(v&&!append_byte(output,capacity,&p,','))return false;
                    if(!append_u64(output,capacity,&p,values[v]))return false;}
                if(!append_byte(output,capacity,&p,'\n'))return false;}
        }
        if(format==NOVA_RENDER_EXPORT_JSON&&!append_text(output,capacity,&p,"]}"))return false;
    }
    *written=p;return true;
}
const nova_memory_budget_t *nova_diag_memory_budget(void){return nova_memory_budget_status();}
const nova_frame_budget_t *nova_diag_frame_budget(void){return &frame_budget;}
const nova_quality_status_t *nova_diag_quality(void){return &quality;}
const nova_diag_statistics_t *nova_diag_statistics(void){return &stats;}

static void regression_copy(char *destination,uint32_t capacity,const char *source)
{uint32_t i=0;for(;source&&source[i]&&i+1<capacity;++i)destination[i]=source[i];destination[i]=0;}
bool nova_performance_regression_initialize(const char *build_id,const char *version,
    uint8_t hardware_profile,uint8_t firmware_type)
{
    regression_status=(nova_performance_regression_status_t){.hardware_profile=hardware_profile,
        .firmware_type=firmware_type,.initialized=true,.deterministic=true,
        .read_only=true,.ci_ready=true};
    regression_copy(regression_status.build_id,sizeof(regression_status.build_id),build_id);
    regression_copy(regression_status.version,sizeof(regression_status.version),version);
    for(uint8_t i=0;i<NOVA_BENCH_COUNT;++i)regression_results[i]=
        (nova_performance_regression_result_t){.benchmark_id=i,
            .threshold_basis_points=i==NOVA_BENCH_FPS?300u:500u,
            .lower_is_better=i!=NOVA_BENCH_FPS&&i!=NOVA_BENCH_CACHE_HIT_RATE};
    return build_id&&*build_id&&version&&*version;
}
bool nova_performance_regression_set_baseline(nova_benchmark_id_t benchmark,
    uint64_t value,bool lower_is_better,uint16_t threshold_basis_points)
{
    if(!regression_status.initialized||benchmark>=NOVA_BENCH_COUNT||!value||
       !threshold_basis_points)return false;
    regression_results[benchmark].baseline_value=value;
    regression_results[benchmark].lower_is_better=lower_is_better;
    regression_results[benchmark].threshold_basis_points=threshold_basis_points;
    return true;
}
bool nova_performance_regression_measure(nova_benchmark_id_t benchmark,uint64_t value)
{
    if(!regression_status.initialized||benchmark>=NOVA_BENCH_COUNT||
       !regression_results[benchmark].baseline_value)return false;
    nova_performance_regression_result_t *result=&regression_results[benchmark];
    result->measured_value=value;uint64_t baseline=result->baseline_value;
    int64_t signed_delta=result->lower_is_better?(int64_t)value-(int64_t)baseline:
        (int64_t)baseline-(int64_t)value;
    int64_t basis=(signed_delta*10000ll)/(int64_t)baseline;
    if(basis>INT32_MAX)basis=INT32_MAX;
    if(basis<INT32_MIN)basis=INT32_MIN;
    result->deviation_basis_points=(int32_t)basis;result->valid=true;
    uint32_t bad=basis>0?(uint32_t)basis:0;uint32_t threshold=result->threshold_basis_points;
    result->level=bad<=threshold?NOVA_REGRESSION_NONE:bad<=threshold*2u?
        NOVA_REGRESSION_MINOR:bad<=threshold*4u?NOVA_REGRESSION_MAJOR:
        NOVA_REGRESSION_CRITICAL;return true;
}
bool nova_performance_regression_execute(void)
{
    if(!regression_status.initialized)return false;
    const nova_resource_diag_status_t *resource=nova_resource_diag_status();
    const nova_animation_diag_status_t *animation=nova_animation_diag_status();
    const nova_input_trace_status_t *input=nova_input_trace_status();
    uint64_t input_latency=0;if(input->count){const nova_input_trace_event_t *last=
        nova_input_trace_get(input->count-1);if(last&&last->completion_us>=last->capture_us)
            input_latency=last->completion_us-last->capture_us;}
    uint64_t cache_total=(uint64_t)resource->cache_hits+resource->cache_misses;
    uint64_t values[NOVA_BENCH_COUNT]={nova_boot_budget_diagnostics()->total_time_us,
        render_stats.fps,render_stats.frame_time_us,nova_memory_budget_status()->peak_memory,
        resource->loads?resource->total_load_us/resource->loads:0,
        animation->average_frame_us,input_latency,
        cache_total?(uint64_t)resource->cache_hits*10000u/cache_total:0};
    regression_status.executed=regression_status.regressions=regression_status.minor=
        regression_status.major=regression_status.critical=0;
    for(uint8_t i=0;i<NOVA_BENCH_COUNT;++i)if(regression_results[i].baseline_value){
        (void)nova_performance_regression_measure((nova_benchmark_id_t)i,values[i]);
        ++regression_status.executed;if(regression_results[i].level){++regression_status.regressions;
            if(regression_results[i].level==NOVA_REGRESSION_MINOR)++regression_status.minor;
            else if(regression_results[i].level==NOVA_REGRESSION_MAJOR)++regression_status.major;
            else ++regression_status.critical;}}
    return regression_status.executed>0;
}
const nova_performance_regression_result_t *nova_performance_regression_results(void)
{return regression_results;}
const nova_performance_regression_status_t *nova_performance_regression_status(void)
{return &regression_status;}
bool nova_performance_regression_generate_report(bool authorized,uint8_t *output,
    uint32_t capacity,uint32_t *written)
{
    if(written)*written=0;
    if(!authorized||!output||!written||capacity<16)return false;
    const char *header="NOVA-PERFORMANCE-REGRESSION-1\nbenchmark baseline measured deviation_bp level\n";
    uint32_t position=0;while(*header&&position<capacity)output[position++]=(uint8_t)*header++;
    for(uint8_t i=0;i<NOVA_BENCH_COUNT&&regression_results[i].valid;++i){
        uint64_t values[5]={i,regression_results[i].baseline_value,
            regression_results[i].measured_value,
            regression_results[i].deviation_basis_points<0?
                (uint32_t)(-regression_results[i].deviation_basis_points):
                (uint32_t)regression_results[i].deviation_basis_points,
            regression_results[i].level};
        for(uint8_t v=0;v<5;++v){char digits[24];uint8_t count=0;
            do{digits[count++]=(char)('0'+values[v]%10u);values[v]/=10u;}while(values[v]);
            while(count){if(position>=capacity)return false;output[position++]=(uint8_t)digits[--count];}
            if(position>=capacity)return false;
            output[position++]=(uint8_t)(v==4?'\n':' ');}}
    *written=position;++regression_status.reports;return true;
}
bool nova_fallback_test_initialize(void)
{
    for(uint8_t i=0;i<NOVA_FALLBACK_MODE_COUNT;++i)fallback_results[i]=
        (nova_fallback_test_result_t){.mode=(nova_fallback_mode_t)i};
    fallback_status=(nova_fallback_test_status_t){.initialized=true,.deterministic=true,
        .heap_free=true,.security_preserved=true,.interactive=true};return true;
}
bool nova_fallback_test_execute(nova_fallback_mode_t mode)
{
    if(!fallback_status.initialized||mode>=NOVA_FALLBACK_MODE_COUNT)return false;
    nova_fallback_test_result_t *result=&fallback_results[mode];
    if(result->activated){++fallback_status.duplicates;return result->successful;}
    result->memory_before=nova_memory_budget_status()->used_memory;
    result->trigger=0xface0000u+(uint32_t)mode;result->automatic=true;
    if(mode==NOVA_FALLBACK_SOFTWARE_RENDERER)result->successful=
        nova_sw_renderer_diagnostics()->initializations>0&&
        (nova_sw_renderer_is_available()||nova_sw_renderer_text_required()||
         nova_sw_renderer_diagnostics()->shutdowns>0);
    else if(mode==NOVA_FALLBACK_STANDARD_THEME){const nova_theme_descriptor_t *fallback=
        nova_theme_default();result->successful=(nova_theme_activate_default()&&
        nova_theme_is_dark())||(fallback&&fallback->is_default);}
    else if(mode==NOVA_FALLBACK_STANDARD_FONT)result->successful=
        nova_font_resource_diagnostics()->initialized&&
        (nova_font_resource_resolve(0xffffffffu,'A')!=0||
         nova_font_theme_get(NOVA_FONT_ROLE_FALLBACK)==NOVA_SYSTEM_FONT_ID);
    else if(mode==NOVA_FALLBACK_PLACEHOLDER_GRAPHIC){static uint32_t pixels[64];
        nova_surface_t surface={.pixels=pixels,.width=8,.height=8,.stride=8};
        uint32_t fallbacks=nova_icon_diagnostics()->fallbacks;
        nova_icon_draw(&surface,(nova_icon_token_t)NOVA_ICON_COUNT,0,0,8,0xffffffffu);
        result->successful=nova_icon_diagnostics()->fallbacks==fallbacks+1;}
    else if(mode==NOVA_FALLBACK_STANDARD_ANIMATION)result->successful=
        nova_animation_resource_diagnostics()->initialized&&
        (nova_animation_resource_for_trigger(NOVA_ANIMATION_TRIGGER_PAGE_CHANGE,
            NOVA_ANIMATION_CATEGORY_NAVIGATION)!=0||nova_motion_budget()->memory_bytes>0);
    else if(mode==NOVA_FALLBACK_RECOVERY_MODE){nova_recovery_initialize();result->successful=
        nova_recovery_report(result->trigger,NOVA_UI_SUBSYSTEM_RESOURCES,
            NOVA_UI_ERROR_RECOVERABLE,NOVA_RECOVERY_LOCAL_FALLBACK,0)&&
        nova_recovery_continue_boot();}
    else if(mode==NOVA_FALLBACK_SAFE_MODE){nova_recovery_initialize();result->successful=
        nova_recovery_report(result->trigger,NOVA_UI_SUBSYSTEM_RENDERING,
            NOVA_UI_ERROR_CRITICAL,NOVA_RECOVERY_SAFE_MODE,0)&&nova_recovery_safe_mode();}
    else if(mode==NOVA_FALLBACK_MINIMAL_MODE){nova_compositor_set_fallback(3);
        nova_motion_set_reduced(true);result->successful=
            nova_compositor_diagnostics()->fallback_level==3&&nova_motion_is_reduced();}
    else result->successful=nova_low_end_profile_status()->enabled||
        (nova_low_end_profile_enable()&&nova_low_end_profile_status()->enabled);
    result->activated=true;result->interactive=true;result->security_preserved=true;
    result->integrity_preserved=nova_integrity_diagnostics()->initialized;
    result->recovery_available=nova_recovery_continue_boot();
    result->memory_after=nova_memory_budget_status()->used_memory;
    ++fallback_status.executed;++fallback_status.count;
    if(result->successful&&result->security_preserved&&result->integrity_preserved)
        ++fallback_status.successful;else {++fallback_status.failed;fallback_status.security_preserved=false;}
    return result->successful&&result->security_preserved&&result->integrity_preserved;
}
const nova_fallback_test_result_t *nova_fallback_test_results(void){return fallback_results;}
const nova_fallback_test_status_t *nova_fallback_test_status(void){return &fallback_status;}
bool nova_fallback_test_generate_report(bool authorized,uint8_t *output,uint32_t capacity,
    uint32_t *written)
{
    if(written)*written=0;
    if(!authorized||!output||!written)return false;
    const char *header="NOVA-FALLBACK-TEST-1\nmode activated successful automatic interactive security integrity recovery memory_before memory_after\n";
    uint32_t position=0;while(*header){if(position>=capacity)return false;output[position++]=(uint8_t)*header++;}
    for(uint8_t i=0;i<fallback_status.count;++i){nova_fallback_test_result_t *record=&fallback_results[i];
        uint64_t values[10]={record->mode,record->activated,record->successful,record->automatic,
            record->interactive,record->security_preserved,record->integrity_preserved,
            record->recovery_available,record->memory_before,record->memory_after};
        for(uint8_t v=0;v<10;++v){char digits[24];uint8_t count=0;
            do{digits[count++]=(char)('0'+values[v]%10u);values[v]/=10u;}while(values[v]);
            while(count){if(position>=capacity)return false;output[position++]=(uint8_t)digits[--count];}
            if(position>=capacity)return false;
            output[position++]=(uint8_t)(v==9?'\n':' ');}}
    *written=position;++fallback_status.reports;return true;
}
