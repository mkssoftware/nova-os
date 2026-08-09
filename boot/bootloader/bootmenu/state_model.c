#include "state_model.h"

static nova_state_object_t objects[NOVA_STATE_CAPACITY];
static nova_state_event_t events[NOVA_STATE_LOG_CAPACITY];
static nova_state_diagnostics_t diagnostics;
static uint16_t event_head;

static uint32_t checksum_bytes(const void *data,uint32_t size)
{
    const uint8_t *bytes=(const uint8_t *)data;uint32_t hash=2166136261u;
    for(uint32_t i=0;i<size;++i){hash^=bytes[i];hash*=16777619u;}return hash;
}
static uint16_t allowed(nova_state_domain_t domain,uint8_t state)
{
    if(state>=NOVA_STATE_VALUE_CAPACITY)return 0;
    switch(domain){
    case NOVA_STATE_DOMAIN_RUNTIME:{
        static const uint16_t m[]={2u,4u|(1u<<8),8u|(1u<<8),16u|(1u<<8),32u|(1u<<8),64u|(1u<<8),128u|(1u<<7)|(1u<<8)|(1u<<9),64u|(1u<<9),64u|(1u<<9),1u<<10,0};
        return state<sizeof(m)/sizeof(m[0])?m[state]:0;}
    case NOVA_STATE_DOMAIN_SCENE:{static const uint16_t m[]={2,4,8,16,32,64,0};return state<7?m[state]:0;}
    case NOVA_STATE_DOMAIN_DIALOG:{static const uint16_t m[]={2,4,8,16,32,64,0};return state<7?m[state]:0;}
    case NOVA_STATE_DOMAIN_OVERLAY:{static const uint16_t m[]={2,4,8,1};return state<4?m[state]:0;}
    case NOVA_STATE_DOMAIN_CONTROL:{static const uint16_t m[]={2,4,8|256,16|128,32|64|128,64,16|128,4|8|256,0};return state<9?m[state]:0;}
    case NOVA_STATE_DOMAIN_FOCUS:{static const uint16_t m[]={2,4|1,8,1};return state<4?m[state]:0;}
    case NOVA_STATE_DOMAIN_POINTER:{static const uint16_t m[]={2,1|4,8|2,16|2,2};return state<5?m[state]:0;}
    case NOVA_STATE_DOMAIN_KEYBOARD:{static const uint16_t m[]={2,1|4,1|2};return state<3?m[state]:0;}
    case NOVA_STATE_DOMAIN_TOUCH:{static const uint16_t m[]={2,4|8,4|8,16,1};return state<5?m[state]:0;}
    case NOVA_STATE_DOMAIN_NAVIGATION:{static const uint16_t m[]={2,4,8,1};return state<4?m[state]:0;}
    case NOVA_STATE_DOMAIN_PAGE:{static const uint16_t m[]={2,4,8|16,4|16,0};return state<5?m[state]:0;}
    case NOVA_STATE_DOMAIN_ANIMATION:{static const uint16_t m[]={2,4|8,8|16|32,4|16|32,32|64,64,0};return state<7?m[state]:0;}
    case NOVA_STATE_DOMAIN_RENDER:{static const uint16_t m[]={2,4,8,1};return state<4?m[state]:0;}
    case NOVA_STATE_DOMAIN_RESOURCE:{static const uint16_t m[]={2,4,8,16,32,64,128,0};return state<8?m[state]:0;}
    case NOVA_STATE_DOMAIN_THEME:{static const uint16_t m[]={2,4,8,16,8};return state<5?m[state]:0;}
    case NOVA_STATE_DOMAIN_ERROR:{static const uint16_t m[]={2,1|4,8|16,1|16|32,1|32,64,0};return state<7?m[state]:0;}
    default:return 0;
    }
}
static bool terminal(const nova_state_object_t *object)
{
    if(!object||!object->active)return true;
    return allowed(object->domain,object->current)==0;
}
static void log_event(nova_state_object_t *object,nova_state_event_type_t type,
                      uint8_t old_state,uint8_t new_state)
{
    uint16_t slot=event_head++%NOVA_STATE_LOG_CAPACITY;
    if(diagnostics.log_count<NOVA_STATE_LOG_CAPACITY)++diagnostics.log_count;
    else ++diagnostics.log_overwrites;
    events[slot]=(nova_state_event_t){.sequence=diagnostics.callback_events+1,
        .timestamp_ms=diagnostics.current_time_ms,.object_id=object->id,
        .domain=object->domain,.type=type,.old_state=old_state,.new_state=new_state};
    ++diagnostics.callback_events;
}
void nova_state_model_initialize(void)
{
    diagnostics=(nova_state_diagnostics_t){0};event_head=0;
    for(uint16_t i=0;i<NOVA_STATE_CAPACITY;++i)objects[i]=(nova_state_object_t){0};
    for(uint16_t i=0;i<NOVA_STATE_LOG_CAPACITY;++i)events[i]=(nova_state_event_t){0};
}
nova_state_object_t *nova_state_create(uint16_t id,nova_state_domain_t domain,
                                      uint8_t initial,uint16_t owner,uint16_t parent)
{
    if(!id||domain>=NOVA_STATE_DOMAIN_COUNT||initial>=NOVA_STATE_VALUE_CAPACITY||
       nova_state_find(id))return 0;
    if(parent!=NOVA_STATE_NONE&&!nova_state_find(parent))return 0;
    for(uint16_t i=0;i<NOVA_STATE_CAPACITY;++i)if(!objects[i].active){
        objects[i]=(nova_state_object_t){.id=id,.owner=owner,.parent=parent,
            .domain=domain,.current=initial,.previous=initial,.initial=initial,
            .checkpoint=initial,.active=true};
        ++diagnostics.active;++diagnostics.created;
        if(diagnostics.active>diagnostics.peak_active)diagnostics.peak_active=diagnostics.active;
        return &objects[i];}
    return 0;
}
bool nova_state_set_callbacks(nova_state_object_t *object,nova_state_callback_t on_exit,
                              nova_state_callback_t on_transition,
                              nova_state_callback_t on_enter,void *context)
{
    if(!object||!object->active||object->in_transition)return false;
    object->on_exit=on_exit;object->on_transition=on_transition;
    object->on_enter=on_enter;object->callback_context=context;return true;
}
bool nova_state_validate(const nova_state_object_t *object,uint8_t next)
{
    if(!object||!object->active||object->in_transition||next>=NOVA_STATE_VALUE_CAPACITY||
       next==object->current||!(allowed(object->domain,object->current)&(1u<<next)))return false;
    if(object->parent!=NOVA_STATE_NONE){const nova_state_object_t *parent=nova_state_find(object->parent);
        if(!parent||terminal(parent))return false;}
    return true;
}
bool nova_state_transition(nova_state_object_t *object,uint8_t next)
{
    if(!nova_state_validate(object,next)){if(object){++object->errors;}++diagnostics.invalid_transitions;return false;}
    uint8_t old=object->current;object->in_transition=true;
    log_event(object,NOVA_STATE_EVENT_EXIT,old,next);
    if(object->on_exit)object->on_exit(object,old,next,object->callback_context);
    log_event(object,NOVA_STATE_EVENT_TRANSITION,old,next);
    if(object->on_transition)object->on_transition(object,old,next,object->callback_context);
    object->previous=old;object->current=next;object->timestamp_ms=diagnostics.current_time_ms;
    ++object->transitions;++diagnostics.transitions;
    log_event(object,NOVA_STATE_EVENT_ENTER,old,next);
    if(object->on_enter)object->on_enter(object,old,next,object->callback_context);
    object->in_transition=false;return true;
}
bool nova_state_checkpoint(nova_state_object_t *object)
{if(!object||!object->active||object->in_transition)return false;
 object->checkpoint=object->current;object->has_checkpoint=true;return true;}
bool nova_state_rollback(nova_state_object_t *object)
{
    if(!object||!object->active||!object->has_checkpoint||object->in_transition||
       object->checkpoint==object->current)return false;
    uint8_t old=object->current;object->previous=old;object->current=object->checkpoint;
    object->timestamp_ms=diagnostics.current_time_ms;object->has_checkpoint=false;
    ++object->transitions;++diagnostics.transitions;++diagnostics.rollbacks;
    log_event(object,NOVA_STATE_EVENT_ROLLBACK,old,object->current);return true;
}
bool nova_state_reset(nova_state_object_t *object)
{
    if(!object||!object->active||object->in_transition)return false;
    uint8_t old=object->current;object->previous=old;object->current=object->initial;
    object->has_checkpoint=false;object->timestamp_ms=diagnostics.current_time_ms;
    ++object->transitions;++diagnostics.transitions;++diagnostics.resets;
    log_event(object,NOVA_STATE_EVENT_RESET,old,object->current);return true;
}
bool nova_state_destroy(nova_state_object_t *object)
{
    if(!object||!object->active||object->in_transition)return false;
    for(uint16_t i=0;i<NOVA_STATE_CAPACITY;++i)
        if(objects[i].active&&objects[i].parent==object->id){++diagnostics.hierarchy_errors;return false;}
    object->active=false;--diagnostics.active;++diagnostics.destroyed;return true;
}
bool nova_state_serialize(const nova_state_object_t *object,nova_state_snapshot_t *snapshot)
{
    if(!object||!object->active||!snapshot)return false;
    *snapshot=(nova_state_snapshot_t){.magic=NOVA_STATE_SNAPSHOT_MAGIC,.version=1,
        .size=sizeof(*snapshot),.id=object->id,.owner=object->owner,.parent=object->parent,
        .domain=(uint16_t)object->domain,.current=object->current,.previous=object->previous,
        .initial=object->initial,.checkpoint=object->checkpoint,.timestamp_ms=object->timestamp_ms,
        .transitions=object->transitions,.errors=object->errors,.checksum=0};
    snapshot->checksum=checksum_bytes(snapshot,sizeof(*snapshot));++diagnostics.serialized;return true;
}
bool nova_state_deserialize(nova_state_object_t *object,const nova_state_snapshot_t *snapshot)
{
    if(!object||!object->active||!snapshot)return false;
    nova_state_snapshot_t copy=*snapshot;uint32_t checksum=copy.checksum;copy.checksum=0;
    if(snapshot->magic!=NOVA_STATE_SNAPSHOT_MAGIC||snapshot->version!=1||
       snapshot->size!=sizeof(*snapshot)||checksum_bytes(&copy,sizeof(copy))!=checksum){
        ++diagnostics.checksum_errors;return false;}
    if(snapshot->id!=object->id||snapshot->domain!=(uint16_t)object->domain||
       snapshot->current>=NOVA_STATE_VALUE_CAPACITY){++diagnostics.invalid_transitions;return false;}
    object->owner=snapshot->owner;object->parent=snapshot->parent;object->current=snapshot->current;
    object->previous=snapshot->previous;object->initial=snapshot->initial;
    object->checkpoint=snapshot->checkpoint;object->timestamp_ms=snapshot->timestamp_ms;
    object->transitions=snapshot->transitions;object->errors=snapshot->errors;
    object->has_checkpoint=false;++diagnostics.deserialized;return true;
}
const nova_state_object_t *nova_state_find(uint16_t id)
{for(uint16_t i=0;i<NOVA_STATE_CAPACITY;++i)if(objects[i].active&&objects[i].id==id)return &objects[i];return 0;}
const nova_state_event_t *nova_state_event(uint16_t newest_index)
{
    if(newest_index>=diagnostics.log_count)return 0;
    uint16_t slot=(uint16_t)((event_head-1u-newest_index)%NOVA_STATE_LOG_CAPACITY);return &events[slot];
}
void nova_state_set_phase(nova_state_phase_t phase)
{if(phase<=NOVA_STATE_PHASE_PRESENT)diagnostics.phase=phase;}
void nova_state_advance_time(uint32_t elapsed_ms)
{if(elapsed_ms>1000u)elapsed_ms=1000u;diagnostics.current_time_ms+=elapsed_ms;}
const nova_state_diagnostics_t *nova_state_diagnostics(void){return &diagnostics;}
