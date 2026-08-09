#include "recovery.h"
#include "diagnostics.h"

typedef struct {
    uint32_t timeout_ms;
    uint32_t deadline_ms;
    bool enabled;
    bool expired;
} nova_recovery_watchdog_t;

static nova_recovery_record_t records[NOVA_RECOVERY_RECORD_CAPACITY];
static nova_recovery_watchdog_t watchdogs[NOVA_RECOVERY_WATCHDOG_COUNT];
static nova_recovery_diagnostics_t diagnostics;
static uint16_t record_head, record_count;
static uint32_t active_code;
static nova_ui_subsystem_t active_subsystem;
static nova_ui_error_severity_t active_severity;
static bool features[NOVA_RECOVERY_FEATURE_COUNT];

static void store_record(uint32_t code, nova_ui_subsystem_t subsystem,
    nova_ui_error_severity_t severity, nova_recovery_level_t level,
    uint32_t timestamp_ms, bool successful)
{
    uint16_t index=(uint16_t)((record_head+record_count)%NOVA_RECOVERY_RECORD_CAPACITY);
    if(record_count==NOVA_RECOVERY_RECORD_CAPACITY){
        index=record_head;
        record_head=(uint16_t)((record_head+1u)%NOVA_RECOVERY_RECORD_CAPACITY);
    }else ++record_count;
    records[index]=(nova_recovery_record_t){code,timestamp_ms,subsystem,severity,
                                            level,successful};
    nova_diag_level_t diag_level=severity>=NOVA_UI_ERROR_CRITICAL?NOVA_DIAG_CRITICAL:
        severity==NOVA_UI_ERROR_RECOVERABLE?NOVA_DIAG_ERROR:
        severity==NOVA_UI_ERROR_WARNING?NOVA_DIAG_WARNING:NOVA_DIAG_INFO;
    nova_diag_log((nova_diag_event_t){(uint64_t)timestamp_ms*1000u,diag_level,
        8u,code,(uint32_t)subsystem,(uint32_t)level});
}

static void apply_level(nova_recovery_level_t level)
{
    diagnostics.level=level;
    if(level>=NOVA_RECOVERY_LOCAL_FALLBACK)++diagnostics.fallbacks;
    if(level==NOVA_RECOVERY_DISABLE_SUBSYSTEM){
        if(active_subsystem==NOVA_UI_SUBSYSTEM_MOTION)features[NOVA_RECOVERY_FEATURE_MOTION]=false;
        if(active_subsystem==NOVA_UI_SUBSYSTEM_RENDERING){
            features[NOVA_RECOVERY_FEATURE_GLASS]=false;
            features[NOVA_RECOVERY_FEATURE_BLUR]=false;
            features[NOVA_RECOVERY_FEATURE_SHADOWS]=false;
            features[NOVA_RECOVERY_FEATURE_VECTOR_FILTERS]=false;
        }
    }
    if(level>=NOVA_RECOVERY_SAFE_MODE){
        if(!diagnostics.safe_mode)++diagnostics.safe_mode_entries;
        diagnostics.safe_mode=true;
        for(uint8_t i=0;i<NOVA_RECOVERY_FEATURE_COUNT;++i)features[i]=false;
        nova_diag_set_quality(NOVA_QUALITY_SAFE,false);
    }
    if(level==NOVA_RECOVERY_TEXT_MODE){
        if(!diagnostics.text_mode)++diagnostics.text_mode_entries;
        diagnostics.text_mode=true;
    }
}

void nova_recovery_initialize(void)
{
    record_head=record_count=0;active_code=0;
    diagnostics=(nova_recovery_diagnostics_t){NOVA_RECOVERY_HEALTHY,
        NOVA_RECOVERY_LOG_ONLY,0,0,0,0,0,0,0,false,false,true};
    for(uint8_t i=0;i<NOVA_RECOVERY_FEATURE_COUNT;++i)features[i]=true;
    for(uint8_t i=0;i<NOVA_RECOVERY_WATCHDOG_COUNT;++i)
        watchdogs[i]=(nova_recovery_watchdog_t){0};
}

bool nova_recovery_report(uint32_t code, nova_ui_subsystem_t subsystem,
    nova_ui_error_severity_t severity, nova_recovery_level_t preferred,
    uint32_t timestamp_ms)
{
    if(!code||subsystem>=NOVA_RECOVERY_WATCHDOG_COUNT||severity>NOVA_UI_ERROR_FATAL||
       preferred>NOVA_RECOVERY_TEXT_MODE){++diagnostics.rejected;return false;}
    ++diagnostics.reports;diagnostics.state=NOVA_RECOVERY_FAILURE;
    active_code=code;active_subsystem=subsystem;active_severity=severity;
    nova_recovery_level_t level=preferred;
    if(severity==NOVA_UI_ERROR_INFO)level=NOVA_RECOVERY_LOG_ONLY;
    else if(severity==NOVA_UI_ERROR_WARNING&&level>NOVA_RECOVERY_LOCAL_FALLBACK)
        level=NOVA_RECOVERY_LOCAL_FALLBACK;
    else if(severity==NOVA_UI_ERROR_CRITICAL&&level<NOVA_RECOVERY_DISABLE_SUBSYSTEM)
        level=NOVA_RECOVERY_DISABLE_SUBSYSTEM;
    else if(severity==NOVA_UI_ERROR_FATAL)level=NOVA_RECOVERY_TEXT_MODE;
    if(level==NOVA_RECOVERY_RETRY)++diagnostics.retries;
    diagnostics.state=NOVA_RECOVERY_RUNNING;apply_level(level);
    diagnostics.state=level>=NOVA_RECOVERY_LOCAL_FALLBACK?
        NOVA_RECOVERY_FALLBACK:NOVA_RECOVERY_VALIDATING;
    bool automatic=level!=NOVA_RECOVERY_RETRY&&level!=NOVA_RECOVERY_REINITIALIZE;
    if(automatic)diagnostics.state=level==NOVA_RECOVERY_TEXT_MODE?
        NOVA_RECOVERY_FATAL:NOVA_RECOVERY_HEALTHY;
    store_record(code,subsystem,severity,level,timestamp_ms,automatic);
    return true;
}

bool nova_recovery_validate(bool recovered, uint32_t timestamp_ms)
{
    if(diagnostics.state!=NOVA_RECOVERY_VALIDATING||!active_code){
        ++diagnostics.rejected;return false;
    }
    if(recovered){
        diagnostics.state=NOVA_RECOVERY_HEALTHY;
        store_record(active_code,active_subsystem,active_severity,
            diagnostics.level,timestamp_ms,true);return true;
    }
    nova_recovery_level_t next=(nova_recovery_level_t)(diagnostics.level+1u);
    if(next>NOVA_RECOVERY_TEXT_MODE)next=NOVA_RECOVERY_TEXT_MODE;
    diagnostics.state=NOVA_RECOVERY_RUNNING;apply_level(next);
    diagnostics.state=next==NOVA_RECOVERY_TEXT_MODE?NOVA_RECOVERY_FATAL:
        NOVA_RECOVERY_HEALTHY;
    store_record(active_code,active_subsystem,active_severity,next,timestamp_ms,true);
    return true;
}

bool nova_recovery_feature_enabled(nova_recovery_feature_t feature)
{return feature<NOVA_RECOVERY_FEATURE_COUNT&&features[feature];}
bool nova_recovery_safe_mode(void){return diagnostics.safe_mode;}
bool nova_recovery_text_mode(void){return diagnostics.text_mode;}
bool nova_recovery_continue_boot(void){return diagnostics.continue_boot;}
uint16_t nova_recovery_record_count(void){return record_count;}
const nova_recovery_record_t *nova_recovery_record(uint32_t index)
{return index<record_count?&records[(record_head+index)%NOVA_RECOVERY_RECORD_CAPACITY]:0;}
const nova_recovery_diagnostics_t *nova_recovery_diagnostics(void){return &diagnostics;}

bool nova_recovery_watchdog_configure(nova_ui_subsystem_t subsystem,
    uint32_t timeout_ms,uint32_t now_ms)
{
    if(subsystem>=NOVA_RECOVERY_WATCHDOG_COUNT||!timeout_ms){
        ++diagnostics.rejected;return false;}
    watchdogs[subsystem]=(nova_recovery_watchdog_t){timeout_ms,
        now_ms+timeout_ms,true,false};return true;
}
bool nova_recovery_watchdog_kick(nova_ui_subsystem_t subsystem,uint32_t now_ms)
{
    if(subsystem>=NOVA_RECOVERY_WATCHDOG_COUNT||!watchdogs[subsystem].enabled){
        ++diagnostics.rejected;return false;}
    watchdogs[subsystem].deadline_ms=now_ms+watchdogs[subsystem].timeout_ms;
    watchdogs[subsystem].expired=false;return true;
}
bool nova_recovery_watchdog_check(uint32_t now_ms)
{
    bool healthy=true;
    for(uint8_t i=0;i<NOVA_RECOVERY_WATCHDOG_COUNT;++i){
        nova_recovery_watchdog_t *watchdog=&watchdogs[i];
        if(watchdog->enabled&&!watchdog->expired&&
           (int32_t)(now_ms-watchdog->deadline_ms)>=0){
            watchdog->expired=true;healthy=false;++diagnostics.watchdog_timeouts;
            nova_recovery_report(0x80010000u+i,(nova_ui_subsystem_t)i,
                NOVA_UI_ERROR_CRITICAL,NOVA_RECOVERY_REINITIALIZE,now_ms);
        }
    }
    return healthy;
}
