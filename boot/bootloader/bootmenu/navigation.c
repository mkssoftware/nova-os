#include "navigation.h"

static nova_navigation_entry_t entries[NOVA_NAVIGATION_CAPACITY];
static nova_navigation_diagnostics_t diagnostics;
static nova_navigation_visual_t visual;
static nova_animation_t *offset_animation;
static nova_animation_t *opacity_animation;
static uint16_t visual_group;
static nova_navigation_test_result_t test_results[NOVA_NAVIGATION_TEST_CAPACITY];
static nova_navigation_test_summary_t test_summary;

void nova_navigation_initialize(nova_navigation_entry_t root)
{
    diagnostics = (nova_navigation_diagnostics_t){0};
    visual = (nova_navigation_visual_t){0};
    offset_animation = opacity_animation = 0;
    visual_group = 40;
    entries[0] = root;
    diagnostics.depth = diagnostics.peak_depth = 1;
}

bool nova_navigation_push(nova_navigation_entry_t entry,
                          nova_navigation_transition_t transition)
{
    if (!diagnostics.depth || diagnostics.depth >= NOVA_NAVIGATION_CAPACITY ||
        diagnostics.transition_running) {
        ++diagnostics.rejected;
        return false;
    }
    entries[diagnostics.depth++] = entry;
    if (diagnostics.depth > diagnostics.peak_depth)
        diagnostics.peak_depth = diagnostics.depth;
    ++diagnostics.pushes;
    diagnostics.last_transition = transition;
    diagnostics.transition_running = true;
    return true;
}

bool nova_navigation_replace(nova_navigation_entry_t entry)
{
    if (!diagnostics.depth || diagnostics.transition_running) {
        ++diagnostics.rejected;
        return false;
    }
    entries[diagnostics.depth - 1] = entry;
    ++diagnostics.replaces;
    diagnostics.last_transition = NOVA_NAV_REPLACE;
    diagnostics.transition_running = true;
    return true;
}

bool nova_navigation_reset(void)
{
    if (!diagnostics.depth || diagnostics.transition_running) {
        ++diagnostics.rejected;
        return false;
    }
    entries[0].selection=0;
    entries[0].focus_id=0;
    entries[0].scroll=0;
    entries[0].context=0;
    diagnostics.depth=1;
    diagnostics.last_transition=NOVA_NAV_REPLACE;
    ++diagnostics.resets;
    return true;
}

bool nova_navigation_back(nova_navigation_entry_t *restored)
{
    if (!restored || diagnostics.depth <= 1 || diagnostics.transition_running) {
        ++diagnostics.rejected;
        return false;
    }
    --diagnostics.depth;
    *restored = entries[diagnostics.depth - 1];
    ++diagnostics.pops;
    ++diagnostics.focus_restores;
    diagnostics.last_transition = NOVA_NAV_POP;
    diagnostics.transition_running = true;
    return true;
}

bool nova_navigation_can_go_back(void) { return diagnostics.depth > 1; }

const nova_navigation_entry_t *nova_navigation_current(void)
{
    return diagnostics.depth ? &entries[diagnostics.depth - 1] : 0;
}

bool nova_navigation_update(uint16_t selection, uint16_t focus_id,
                            int32_t scroll, uint32_t context)
{
    if (!diagnostics.depth) { ++diagnostics.rejected; return false; }
    nova_navigation_entry_t *entry = &entries[diagnostics.depth - 1];
    entry->selection = selection;
    entry->focus_id = focus_id;
    entry->scroll = scroll;
    entry->context = context;
    return true;
}

void nova_navigation_transition_complete(void)
{
    diagnostics.transition_running = false;
}

bool nova_navigation_visual_begin(nova_navigation_transition_t type, bool entering,
                                  uint64_t start_ms)
{
    if (visual.running || type > NOVA_NAV_RECOVERY) {
        ++diagnostics.rejected;
        return false;
    }
    bool reduced = nova_motion_is_reduced();
    int32_t distance = reduced || type == NOVA_NAV_REPLACE ||
                       type == NOVA_NAV_RECOVERY ? 0 : 32;
    if (type == NOVA_NAV_POP) distance = -distance;
    visual = (nova_navigation_visual_t){
        entering ? distance : 0, entering ? 0 : 255,
        type == NOVA_NAV_RECOVERY ? 240u : reduced ? 150u : 180u,
        type, entering, true, reduced
    };
    diagnostics.transition_running = true;
    diagnostics.focus_locked = true;
    diagnostics.last_transition = type;
    ++diagnostics.visual_started;
    ++visual_group;
    opacity_animation = nova_motion_create(&(nova_animation_t){
        &visual.opacity,visual.opacity,entering?255:0,start_ms,0,visual.duration_ms,
        visual_group,2,0,NOVA_PROPERTY_OPACITY,NOVA_EASE_OUT_CUBIC,
        NOVA_MOTION_CREATED,false,false,true,0,0,0
    });
    offset_animation = 0;
    if (distance) offset_animation = nova_motion_create(&(nova_animation_t){
        &visual.offset_dlu,visual.offset_dlu,entering?0:-distance,start_ms,0,
        visual.duration_ms,visual_group,2,0,NOVA_PROPERTY_X,NOVA_EASE_OUT_CUBIC,
        NOVA_MOTION_CREATED,false,false,true,0,0,0
    });
    if (!opacity_animation || (distance && !offset_animation)) {
        if (opacity_animation) nova_motion_cancel(opacity_animation);
        if (offset_animation) nova_motion_cancel(offset_animation);
        visual.running=false; diagnostics.transition_running=false;
        diagnostics.focus_locked=false; ++diagnostics.rejected;
        return false;
    }
    return true;
}

void nova_navigation_visual_complete(void)
{
    if (!visual.running) return;
    visual.offset_dlu=0;
    visual.opacity=visual.entering?255:0;
    visual.running=false;
    diagnostics.transition_running=false;
    diagnostics.focus_locked=false;
    ++diagnostics.visual_completed;
}

void nova_navigation_visual_cancel(void)
{
    if (!visual.running) return;
    if (offset_animation) nova_motion_cancel(offset_animation);
    if (opacity_animation) nova_motion_cancel(opacity_animation);
    visual.running=false;
    diagnostics.transition_running=false;
    diagnostics.focus_locked=false;
    ++diagnostics.visual_cancelled;
}

const nova_navigation_visual_t *nova_navigation_visual(void) { return &visual; }

const nova_navigation_diagnostics_t *nova_navigation_diagnostics(void)
{
    return &diagnostics;
}

bool nova_navigation_test_initialize(void)
{
    for(uint8_t i=0;i<NOVA_NAVIGATION_TEST_CAPACITY;++i)
        test_results[i]=(nova_navigation_test_result_t){0};
    test_summary=(nova_navigation_test_summary_t){.initialized=true,.isolated=true,
        .deterministic=true,.configuration_unchanged=true};return true;
}

bool nova_navigation_test_execute(uint32_t route_id)
{
    if(!test_summary.initialized||route_id>=NOVA_NAVIGATION_TEST_CAPACITY||
       test_summary.count==NOVA_NAVIGATION_TEST_CAPACITY)return false;
    nova_navigation_entry_t saved_entries[NOVA_NAVIGATION_CAPACITY];
    for(uint16_t i=0;i<NOVA_NAVIGATION_CAPACITY;++i)saved_entries[i]=entries[i];
    nova_navigation_diagnostics_t saved_diag=diagnostics;
    nova_navigation_visual_t saved_visual=visual;nova_animation_t *saved_offset=offset_animation;
    nova_animation_t *saved_opacity=opacity_animation;uint16_t saved_group=visual_group;
    nova_navigation_test_result_t result={.route_id=route_id,.status=NOVA_NAVIGATION_TEST_FAILED,
        .start_page=0,.start_focus=10,.target_page=(uint16_t)(route_id+1),
        .target_focus=(uint16_t)(20+route_id),.input_device=(uint8_t)(route_id==NOVA_NAV_TEST_POINTER?2:
            route_id==NOVA_NAV_TEST_TOUCH?3:1)};
    if(route_id==NOVA_NAV_TEST_INSTALLER||route_id==NOVA_NAV_TEST_TOUCH){
        result.status=NOVA_NAVIGATION_TEST_SKIPPED;goto restore;
    }
    nova_navigation_initialize((nova_navigation_entry_t){0,2,10,0,0});++result.step_count;
    if(nova_navigation_can_go_back()||!nova_navigation_current()||
       nova_navigation_current()->focus_id!=10)result.detected_errors|=1u;
    if(!nova_navigation_push((nova_navigation_entry_t){result.target_page,0,
        result.target_focus,0,route_id},route_id==NOVA_NAV_TEST_RECOVERY?
        NOVA_NAV_RECOVERY:route_id==NOVA_NAV_TEST_DIALOG?NOVA_NAV_DIALOG:NOVA_NAV_PUSH))
        result.detected_errors|=2u;
    ++result.step_count;nova_navigation_transition_complete();
    if(!nova_navigation_update(4,result.target_focus,72,route_id)||
       !nova_navigation_current()||nova_navigation_current()->page!=result.target_page||
       nova_navigation_current()->selection!=4||nova_navigation_current()->scroll!=72)
        result.detected_errors|=4u;
    else result.focus_path=(uint16_t)((result.start_focus<<8)|result.target_focus);
    ++result.step_count;
    nova_navigation_entry_t restored={0};
    if(!nova_navigation_back(&restored)||restored.page!=result.start_page||
       restored.focus_id!=result.start_focus||restored.selection!=2)
        result.detected_errors|=8u;
    else result.back_restored=true;
    ++result.step_count;nova_navigation_transition_complete();
    if(nova_navigation_can_go_back()||nova_navigation_back(&restored))
        result.detected_errors|=16u;
    ++result.step_count;
    result.dead_end=!result.back_restored;result.loop_detected=false;
    if(!result.detected_errors)result.status=NOVA_NAVIGATION_TEST_PASSED;
restore:
    for(uint16_t i=0;i<NOVA_NAVIGATION_CAPACITY;++i)entries[i]=saved_entries[i];
    diagnostics=saved_diag;visual=saved_visual;offset_animation=saved_offset;
    opacity_animation=saved_opacity;visual_group=saved_group;
    result.configuration_changed=false;
    test_results[test_summary.count++]=result;
    if(result.status==NOVA_NAVIGATION_TEST_PASSED)++test_summary.passed;
    else if(result.status==NOVA_NAVIGATION_TEST_SKIPPED)++test_summary.skipped;
    else ++test_summary.failed;
    test_summary.configuration_unchanged=diagnostics.depth==saved_diag.depth&&
        diagnostics.transition_running==saved_diag.transition_running;
    return result.status!=NOVA_NAVIGATION_TEST_FAILED;
}

const nova_navigation_test_result_t *nova_navigation_test_results(void){return test_results;}
const nova_navigation_test_summary_t *nova_navigation_test_summary(void){return &test_summary;}
static bool nav_report_text(uint8_t *o,uint32_t c,uint32_t *p,const char *s)
{while(*s){if(*p+1>=c)return false;o[(*p)++]=(uint8_t)*s++;}o[*p]=0;return true;}
static bool nav_report_u32(uint8_t *o,uint32_t c,uint32_t *p,uint32_t v)
{char d[10];uint8_t n=0;do{d[n++]=(char)('0'+v%10u);v/=10u;}while(v&&n<10);
 while(n){if(*p+1>=c)return false;o[(*p)++]=(uint8_t)d[--n];}o[*p]=0;return true;}
bool nova_navigation_test_generate_report(bool authorized,uint8_t *output,
                                          uint32_t capacity,uint32_t *written)
{
    if(written)*written=0;
    if(!authorized||!output||capacity<128||!written||!test_summary.initialized)return false;
    uint32_t p=0;
    if(!nav_report_text(output,capacity,&p,"NOVA_NAVIGATION_TEST_REPORT\ncount=")||
       !nav_report_u32(output,capacity,&p,test_summary.count)||
       !nav_report_text(output,capacity,&p," passed=")||
       !nav_report_u32(output,capacity,&p,test_summary.passed)||
       !nav_report_text(output,capacity,&p," skipped=")||
       !nav_report_u32(output,capacity,&p,test_summary.skipped)||
       !nav_report_text(output,capacity,&p,"\n"))return false;
    for(uint8_t i=0;i<test_summary.count;++i){const nova_navigation_test_result_t *r=&test_results[i];
        if(!nav_report_text(output,capacity,&p,"route=")||!nav_report_u32(output,capacity,&p,r->route_id)||
           !nav_report_text(output,capacity,&p," status=")||!nav_report_u32(output,capacity,&p,r->status)||
           !nav_report_text(output,capacity,&p," steps=")||!nav_report_u32(output,capacity,&p,r->step_count)||
           !nav_report_text(output,capacity,&p," errors=")||!nav_report_u32(output,capacity,&p,r->detected_errors)||
           !nav_report_text(output,capacity,&p,"\n"))return false;}
    *written=p;++test_summary.reports;return true;
}
