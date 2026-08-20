#include "configuration.h"

static nova_boot_configuration_t current,staging,effective,override_values;
static nova_configuration_diagnostics_t diagnostics;
static uint32_t override_mask;

static nova_boot_configuration_t defaults(void)
{
    nova_boot_configuration_t value={
        NOVA_CONFIGURATION_VERSION_MAJOR,NOVA_CONFIGURATION_VERSION_MINOR,
        NOVA_CONFIGURATION_VERSION_PATCH,NOVA_THEME_DARK,NOVA_QUALITY_BALANCED,
        0,750,5000,500,50,100,500,2,false,true,false,true,true,false,0
    };
    value.checksum=nova_configuration_checksum(&value);return value;
}

uint32_t nova_configuration_checksum(const nova_boot_configuration_t *value)
{
    if(!value)return 0;
    uint32_t hash=2166136261u;
#define MIX(v) do{uint32_t n=(uint32_t)(v);for(uint8_t b=0;b<4;++b){hash^=(n>>(b*8))&0xffu;hash*=16777619u;}}while(0)
    MIX(value->version_major);MIX(value->version_minor);MIX(value->version_patch);
    MIX(value->theme);MIX(value->quality);MIX(value->scale_milli);
    MIX(value->tooltip_delay_ms);MIX(value->watchdog_timeout_ms);
    MIX(value->key_repeat_delay_ms);MIX(value->key_repeat_rate_ms);
    MIX(value->mouse_speed_percent);
    MIX(value->double_click_time_ms);
    MIX(value->recovery_retries);MIX(value->reduced_motion);MIX(value->tooltips);
    MIX(value->safe_mode);MIX(value->text_fallback);MIX(value->auto_recovery);
    MIX(value->debug_overlay);
#undef MIX
    return hash;
}

bool nova_configuration_validate(const nova_boot_configuration_t *value)
{
    if(!value||value->version_major!=NOVA_CONFIGURATION_VERSION_MAJOR||
       value->version_minor>NOVA_CONFIGURATION_VERSION_MINOR||
       value->theme>=NOVA_THEME_COUNT||value->quality>NOVA_QUALITY_AUTO||
       (value->scale_milli&&
        (value->scale_milli<1000||value->scale_milli>3000))||
       value->tooltip_delay_ms<250||value->tooltip_delay_ms>1500||
       (value->tooltip_delay_ms-250)%250||value->watchdog_timeout_ms<1000||
       value->watchdog_timeout_ms>30000||value->key_repeat_delay_ms<100||
       value->key_repeat_delay_ms>2000||value->key_repeat_rate_ms<20||
       value->key_repeat_rate_ms>1000||value->mouse_speed_percent<25||
       value->mouse_speed_percent>400||value->double_click_time_ms<200||
       value->double_click_time_ms>1000||value->recovery_retries>3)return false;
    return !value->checksum||value->checksum==nova_configuration_checksum(value);
}

static void rebuild_effective(void)
{
    effective=current;
#define APPLY(key,field) if(override_mask&(1u<<(key)))effective.field=override_values.field
    APPLY(NOVA_CONFIG_THEME,theme);APPLY(NOVA_CONFIG_REDUCED_MOTION,reduced_motion);
    APPLY(NOVA_CONFIG_TOOLTIPS,tooltips);APPLY(NOVA_CONFIG_TOOLTIP_DELAY,tooltip_delay_ms);
    APPLY(NOVA_CONFIG_SCALE,scale_milli);APPLY(NOVA_CONFIG_QUALITY,quality);
    APPLY(NOVA_CONFIG_SAFE_MODE,safe_mode);APPLY(NOVA_CONFIG_TEXT_FALLBACK,text_fallback);
    APPLY(NOVA_CONFIG_AUTO_RECOVERY,auto_recovery);
    APPLY(NOVA_CONFIG_RECOVERY_RETRIES,recovery_retries);
    APPLY(NOVA_CONFIG_WATCHDOG_TIMEOUT,watchdog_timeout_ms);
    APPLY(NOVA_CONFIG_KEY_REPEAT_DELAY,key_repeat_delay_ms);
    APPLY(NOVA_CONFIG_KEY_REPEAT_RATE,key_repeat_rate_ms);
    APPLY(NOVA_CONFIG_MOUSE_SPEED,mouse_speed_percent);
    APPLY(NOVA_CONFIG_DOUBLE_CLICK_TIME,double_click_time_ms);
    APPLY(NOVA_CONFIG_DEBUG_OVERLAY,debug_overlay);
#undef APPLY
    effective.checksum=nova_configuration_checksum(&effective);
}

void nova_configuration_initialize(void)
{
    current=defaults();staging=current;effective=current;override_values=current;
    override_mask=0;diagnostics=(nova_configuration_diagnostics_t){.generation=1};
}
const nova_boot_configuration_t *nova_configuration_get(void){return &current;}
const nova_boot_configuration_t *nova_configuration_effective(void){return &effective;}
bool nova_configuration_begin(void)
{
    if(diagnostics.transaction_active){++diagnostics.rejected_writes;return false;}
    staging=current;diagnostics.transaction_active=true;++diagnostics.transactions;return true;
}

static bool set_value(nova_boot_configuration_t *target,
    nova_configuration_key_t key,uint32_t value)
{
    switch(key){
    case NOVA_CONFIG_THEME:target->theme=(nova_theme_id_t)value;break;
    case NOVA_CONFIG_REDUCED_MOTION:target->reduced_motion=value!=0;break;
    case NOVA_CONFIG_TOOLTIPS:target->tooltips=value!=0;break;
    case NOVA_CONFIG_TOOLTIP_DELAY:target->tooltip_delay_ms=(uint16_t)value;break;
    case NOVA_CONFIG_SCALE:target->scale_milli=(uint16_t)value;break;
    case NOVA_CONFIG_QUALITY:target->quality=(nova_quality_t)value;break;
    case NOVA_CONFIG_SAFE_MODE:target->safe_mode=value!=0;break;
    case NOVA_CONFIG_TEXT_FALLBACK:target->text_fallback=value!=0;break;
    case NOVA_CONFIG_AUTO_RECOVERY:target->auto_recovery=value!=0;break;
    case NOVA_CONFIG_RECOVERY_RETRIES:target->recovery_retries=(uint8_t)value;break;
    case NOVA_CONFIG_WATCHDOG_TIMEOUT:target->watchdog_timeout_ms=(uint16_t)value;break;
    case NOVA_CONFIG_KEY_REPEAT_DELAY:target->key_repeat_delay_ms=(uint16_t)value;break;
    case NOVA_CONFIG_KEY_REPEAT_RATE:target->key_repeat_rate_ms=(uint16_t)value;break;
    case NOVA_CONFIG_MOUSE_SPEED:target->mouse_speed_percent=(uint16_t)value;break;
    case NOVA_CONFIG_DOUBLE_CLICK_TIME:target->double_click_time_ms=(uint16_t)value;break;
    case NOVA_CONFIG_DEBUG_OVERLAY:target->debug_overlay=value!=0;break;
    default:return false;
    }
    target->checksum=0;return true;
}

bool nova_configuration_set(nova_configuration_key_t key,uint32_t value)
{
    if(!diagnostics.transaction_active||key>=NOVA_CONFIG_KEY_COUNT||
       !set_value(&staging,key,value)){
        ++diagnostics.rejected_writes;return false;}
    return true;
}

bool nova_configuration_commit(void)
{
    if(!diagnostics.transaction_active){++diagnostics.rejected_writes;return false;}
    if(!nova_configuration_validate(&staging)){
        diagnostics.transaction_active=false;++diagnostics.validation_errors;
        ++diagnostics.rollbacks;staging=current;return false;
    }
    staging.checksum=nova_configuration_checksum(&staging);current=staging;
    diagnostics.transaction_active=false;++diagnostics.commits;++diagnostics.generation;
    ++diagnostics.notifications;rebuild_effective();return true;
}
bool nova_configuration_rollback(void)
{
    if(!diagnostics.transaction_active){++diagnostics.rejected_writes;return false;}
    staging=current;diagnostics.transaction_active=false;++diagnostics.rollbacks;return true;
}
bool nova_configuration_reset_defaults(void)
{
    if(diagnostics.transaction_active){++diagnostics.rejected_writes;return false;}
    current=defaults();++diagnostics.resets;++diagnostics.generation;
    ++diagnostics.notifications;rebuild_effective();return true;
}
bool nova_configuration_override(nova_configuration_key_t key,uint32_t value)
{
    if(key>=NOVA_CONFIG_KEY_COUNT||diagnostics.transaction_active){
        ++diagnostics.rejected_writes;return false;}
    nova_boot_configuration_t candidate=override_values;
    if(!set_value(&candidate,key,value)||!nova_configuration_validate(&candidate)){
        ++diagnostics.validation_errors;return false;}
    candidate.checksum=nova_configuration_checksum(&candidate);override_values=candidate;
    override_mask|=1u<<key;++diagnostics.overrides;rebuild_effective();return true;
}
bool nova_configuration_clear_overrides(void)
{
    if(!override_mask)return false;
    override_mask=0;override_values=current;
    ++diagnostics.notifications;rebuild_effective();return true;
}
const nova_configuration_diagnostics_t *nova_configuration_diagnostics(void)
{return &diagnostics;}
