#include "input.h"

typedef struct {nova_control_t *owner;nova_capture_state_t state;} capture_t;
typedef struct {nova_control_t *target;bool explicit,wrap;} neighbor_t;
typedef struct {nova_shortcut_t value;bool used,tombstone;} shortcut_t;
typedef struct {nova_repeat_context_t context;nova_input_event_t event;bool used;} repeat_t;

static nova_input_event_t queue[NOVA_INPUT_QUEUE_CAPACITY];
static uint16_t head, tail, count;
static uint64_t sequence;
static nova_input_device_t devices[NOVA_INPUT_DEVICE_CAPACITY];
static bool device_used[NOVA_INPUT_DEVICE_CAPACITY];
static capture_t captures[NOVA_INPUT_POINTER_CAPACITY];
static nova_capture_diagnostics_t capture_diagnostics[NOVA_INPUT_POINTER_CAPACITY];
static uint64_t input_clock_ms;
static neighbor_t neighbors[NOVA_CONTROL_CAPACITY][6];
static nova_directional_diagnostics_t directional_diagnostics;
static bool directional_navigating;
static shortcut_t shortcuts[NOVA_INPUT_SHORTCUT_HASH_CAPACITY];
static nova_shortcut_diagnostics_t shortcut_diagnostics;
static nova_shortcut_command_handler_t shortcut_handler;
static void *shortcut_handler_context;
static uint16_t shortcut_context_id,shortcut_dialog_id;
static bool shortcut_dispatching;
static repeat_t repeats[NOVA_INPUT_REPEAT_CAPACITY];
static nova_repeat_diagnostics_t repeat_diagnostics;
static uint64_t repeat_clock_ms;
static nova_control_t *focused;
static nova_focus_scope_t *focus_scopes[NOVA_FOCUS_SCOPE_CAPACITY];
static uint8_t focus_scope_depth;
static nova_focus_diagnostics_t focus_diagnostics;
static bool focus_changing;
typedef struct {nova_control_t *target;uint64_t released_ms;int32_t x,y;
    bool armed,suppress_release;} nova_pointer_click_state_t;
static nova_pointer_click_state_t pointer_clicks[NOVA_INPUT_POINTER_CAPACITY];
static nova_input_diagnostics_t diagnostics;
static bool secure_mode;
static nova_input_trace_event_t trace_events[NOVA_INPUT_TRACE_CAPACITY];
static uint16_t trace_head,trace_count;
static nova_input_trace_status_t trace_status;
static uint64_t last_trace_signature;
static nova_input_device_scanner_t device_scanner;
static void *device_scanner_context;
static void repeat_stop_all(uint64_t now,uint8_t reason);

static nova_device_type_t device_type(uint32_t id)
{const nova_input_device_t *device=nova_input_device_get(id);
 return device?device->type:NOVA_DEVICE_UNKNOWN;}
static uint32_t device_slot(uint32_t id)
{return id-1u;}
static nova_input_trace_type_t trace_type(const nova_input_event_t *event)
{
    if(event->type==NOVA_EVENT_KEY_DOWN)return event->key.repeat?NOVA_TRACE_KEY_REPEAT:NOVA_TRACE_KEY_DOWN;
    if(event->type==NOVA_EVENT_KEY_UP)return NOVA_TRACE_KEY_UP;
    if(event->type==NOVA_EVENT_POINTER_MOVE)return NOVA_TRACE_MOUSE_MOVE;
    if(event->type==NOVA_EVENT_POINTER_DOWN||event->type==NOVA_EVENT_POINTER_UP)return NOVA_TRACE_MOUSE_BUTTON;
    if(event->type==NOVA_EVENT_POINTER_WHEEL)return NOVA_TRACE_MOUSE_WHEEL;
    if(event->type==NOVA_EVENT_TOUCH_BEGIN)return NOVA_TRACE_TOUCH_DOWN;
    if(event->type==NOVA_EVENT_TOUCH_UPDATE)return NOVA_TRACE_TOUCH_MOVE;
    if(event->type==NOVA_EVENT_TOUCH_END)return NOVA_TRACE_TOUCH_UP;
    if(event->type==NOVA_EVENT_CONTROLLER_BUTTON||event->type==NOVA_EVENT_CONTROLLER_AXIS)return NOVA_TRACE_CONTROLLER;
    return NOVA_TRACE_SYSTEM;
}
static nova_input_trace_event_t *trace_find(uint64_t id)
{for(uint16_t i=0;i<trace_count;++i){uint16_t n=(uint16_t)((trace_head+i)%NOVA_INPUT_TRACE_CAPACITY);
 if(trace_events[n].event_id==id)return &trace_events[n];}return 0;}
static void trace_store(nova_input_trace_event_t event)
{
    if(!trace_status.enabled)return;
    uint64_t signature=(event.event_id<<32)^((uint64_t)event.type<<24)^event.device_id;
    if(signature==last_trace_signature){++trace_status.duplicates;return;}last_trace_signature=signature;
    uint16_t index=(uint16_t)((trace_head+trace_count)%NOVA_INPUT_TRACE_CAPACITY);
    if(trace_count==NOVA_INPUT_TRACE_CAPACITY){index=trace_head;trace_head=(uint16_t)((trace_head+1)%NOVA_INPUT_TRACE_CAPACITY);++trace_status.overwritten;}
    else ++trace_count;
    trace_events[index]=event;++trace_status.recorded;trace_status.count=trace_count;
}

void nova_input_initialize(void)
{
    head = tail = count = 0; sequence = 0; focused = 0;
    input_clock_ms=0;
    for(uint8_t i=0;i<NOVA_INPUT_POINTER_CAPACITY;++i)
        pointer_clicks[i]=(nova_pointer_click_state_t){0};
    diagnostics = (nova_input_diagnostics_t){0}; secure_mode = false;
    (void)nova_input_device_initialize();
    for (uint32_t i=0;i<NOVA_INPUT_POINTER_CAPACITY;++i){
        captures[i]=(capture_t){0,NOVA_CAPTURE_INACTIVE};
        capture_diagnostics[i]=(nova_capture_diagnostics_t){
            .pointer_id=i,.owner_id=NOVA_CONTROL_NONE,.state=NOVA_CAPTURE_INACTIVE};}
    for(uint32_t i=0;i<NOVA_INPUT_SHORTCUT_HASH_CAPACITY;++i)shortcuts[i]=(shortcut_t){0};
    shortcut_diagnostics=(nova_shortcut_diagnostics_t){0};shortcut_handler=0;
    shortcut_handler_context=0;shortcut_context_id=shortcut_dialog_id=0;
    shortcut_dispatching=false;
    for(uint32_t i=0;i<NOVA_INPUT_REPEAT_CAPACITY;++i)repeats[i]=(repeat_t){0};
    repeat_diagnostics=(nova_repeat_diagnostics_t){.initial_delay_ms=400,
        .interval_ms=50,.initialized=true,.enabled=true};repeat_clock_ms=0;
    (void)nova_input_trace_initialize();
    (void)nova_input_focus_initialize();
    directional_diagnostics=(nova_directional_diagnostics_t){
        .source=NOVA_CONTROL_NONE,.target=NOVA_CONTROL_NONE,.scope=NOVA_CONTROL_NONE};
    directional_navigating=false;
    for(uint16_t c=0;c<NOVA_CONTROL_CAPACITY;++c)
        for(uint8_t d=0;d<6;++d)neighbors[c][d]=(neighbor_t){0};
}

bool nova_input_device_initialize(void)
{
    for(uint32_t i=0;i<NOVA_INPUT_DEVICE_CAPACITY;++i){device_used[i]=false;
        devices[i]=(nova_input_device_t){0};}
    diagnostics.devices=0;device_scanner=0;device_scanner_context=0;return true;
}
bool nova_input_device_scanner_set(nova_input_device_scanner_t scanner,void *context)
{if(!scanner)return false;device_scanner=scanner;device_scanner_context=context;return true;}
bool nova_input_device_scan(void)
{return device_scanner&&device_scanner(device_scanner_context);}
uint32_t nova_input_device_count(void){return diagnostics.devices;}
const nova_input_device_t *nova_input_device_get(uint32_t id)
{if(!id||id>NOVA_INPUT_DEVICE_CAPACITY)return 0;uint32_t slot=device_slot(id);
 return device_used[slot]&&devices[slot].id==id?&devices[slot]:0;}
bool nova_input_device_connected(uint32_t id)
{const nova_input_device_t *device=nova_input_device_get(id);
 return device&&device->connected&&device->state==NOVA_DEVICE_STATE_READY;}
static bool device_emit(uint32_t id,nova_input_device_event_t event,uint64_t timestamp)
{nova_input_event_t notification={.timestamp=timestamp,.device_id=id,
 .type=NOVA_EVENT_SYSTEM};notification.system.command=(uint32_t)event;
 return nova_input_post(notification);}
bool nova_input_device_transition(uint32_t id,nova_device_type_t type,
 nova_input_device_event_t event,uint64_t timestamp)
{
 if(!id||id>NOVA_INPUT_DEVICE_CAPACITY||type==NOVA_DEVICE_UNKNOWN||
    type>NOVA_DEVICE_ACCESSIBILITY||event<NOVA_DEVICE_EVENT_ADDED||
    event>NOVA_DEVICE_EVENT_READY){++diagnostics.device_errors;return false;}
 uint32_t slot=device_slot(id);nova_input_device_t *device=&devices[slot];
 if(device_used[slot]&&device->id!=id){++diagnostics.device_errors;return false;}
 if(!device_used[slot]){
   if(event!=NOVA_DEVICE_EVENT_ADDED)return false;
   device_used[slot]=true;*device=(nova_input_device_t){.id=id,.instance=id,
     .type=type,.state=NOVA_DEVICE_STATE_DETECTED,.changes=1};
   ++diagnostics.devices;++diagnostics.device_changes;
   return device_emit(id,event,timestamp);
 }
 if(device->type!=type){++device->errors;++diagnostics.device_errors;
   device->state=NOVA_DEVICE_STATE_ERROR;device->connected=false;
   (void)device_emit(id,NOVA_DEVICE_EVENT_ERROR,timestamp);return false;}
 if(event==NOVA_DEVICE_EVENT_ADDED){
   if(device->state!=NOVA_DEVICE_STATE_DISCONNECTED)return false;
   device->state=NOVA_DEVICE_STATE_DETECTED;device->connected=false;
 }else if(event==NOVA_DEVICE_EVENT_CHANGED){
   if(device->state!=NOVA_DEVICE_STATE_DETECTED&&device->state!=NOVA_DEVICE_STATE_READY)return false;
   device->state=NOVA_DEVICE_STATE_INITIALIZING;
 }else if(event==NOVA_DEVICE_EVENT_READY){
   if(device->state!=NOVA_DEVICE_STATE_INITIALIZING)return false;
   device->state=NOVA_DEVICE_STATE_READY;device->connected=true;
   device->initialization_ms=timestamp;
 }else if(event==NOVA_DEVICE_EVENT_REMOVED){
   if(device->state==NOVA_DEVICE_STATE_DISCONNECTED)return false;
   device->state=NOVA_DEVICE_STATE_DISCONNECTED;device->connected=false;
   nova_input_pointer_cancel_all();
   repeat_stop_all(input_clock_ms,2);
 }else {
   device->state=NOVA_DEVICE_STATE_ERROR;device->connected=false;++device->errors;
   ++diagnostics.device_errors;
 }
 ++device->changes;++diagnostics.device_changes;
 return device_emit(id,event,timestamp);
}
bool nova_input_device_set(uint32_t id,nova_device_type_t type,bool connected)
{
 const nova_input_device_t *device=nova_input_device_get(id);
 if(connected){
   if(device&&device->state==NOVA_DEVICE_STATE_READY)return true;
   if(!device&&!nova_input_device_transition(id,type,NOVA_DEVICE_EVENT_ADDED,input_clock_ms))return false;
   else if(device&&!nova_input_device_transition(id,type,NOVA_DEVICE_EVENT_ADDED,input_clock_ms))return false;
   if(!nova_input_device_transition(id,type,NOVA_DEVICE_EVENT_CHANGED,input_clock_ms))return false;
   return nova_input_device_transition(id,type,NOVA_DEVICE_EVENT_READY,input_clock_ms);
 }
 return device&&nova_input_device_transition(id,type,NOVA_DEVICE_EVENT_REMOVED,input_clock_ms);
}

bool nova_input_post(nova_input_event_t event)
{
    if(event.type!=NOVA_EVENT_SYSTEM&&!nova_input_device_connected(event.device_id)){
        ++diagnostics.dropped;return false;}
    if (count == NOVA_INPUT_QUEUE_CAPACITY) { ++diagnostics.dropped;++trace_status.queue_overflows;
        ++trace_status.dropped;return false; }
    event.sequence = ++sequence;
    if (secure_mode) { event.flags |= 0x80000000u; ++diagnostics.secure_events; }
    queue[tail] = event; tail = (uint16_t)((tail + 1u) % NOVA_INPUT_QUEUE_CAPACITY);
    ++count; ++diagnostics.posted; diagnostics.last_sequence = sequence;
    nova_input_trace_event_t trace={.event_id=event.sequence,.capture_us=event.timestamp,
        .device_id=event.device_id,.flags=event.flags,.type=trace_type(&event),
        .device_type=device_type(event.device_id),.queue_length=count};
    if(event.type<=NOVA_EVENT_CHARACTER){trace.scan_code=event.key.key;
        trace.virtual_key=event.key.key;trace.modifiers=event.key.modifiers;
        trace.repeat_count=event.key.repeat?1u:0u;trace.masked=secure_mode;
        trace.unicode=secure_mode?0:event.key.unicode;if(secure_mode)++trace_status.masked_events;}
    else if(event.type>=NOVA_EVENT_POINTER_MOVE&&event.type<=NOVA_EVENT_TOUCH_END){
        trace.pointer_id=event.pointer.pointer_id;trace.buttons=event.pointer.buttons;
        trace.x=event.pointer.x;trace.y=event.pointer.y;trace.wheel_x=event.pointer.wheel_x;
        trace.wheel_y=event.pointer.wheel_y;}
    trace_store(trace);return true;
}

bool nova_input_next(nova_input_event_t *event)
{
    if (!event || !count) return false;
    *event = queue[head]; head = (uint16_t)((head + 1u) % NOVA_INPUT_QUEUE_CAPACITY);
    --count; ++diagnostics.dispatched;
    nova_input_trace_event_t *trace=trace_find(event->sequence);if(trace){
        trace->dispatch_us=event->timestamp;trace->queue_wait_us=0;}
    return true;
}

bool nova_input_pointer_capture(uint32_t id, nova_control_t *control)
{
    if(id>=NOVA_INPUT_POINTER_CAPACITY||!control||
       control->state==NOVA_CONTROL_DESTROYED||
       captures[id].state==NOVA_CAPTURE_ACTIVE){++diagnostics.capture_errors;
        if(id<NOVA_INPUT_POINTER_CAPACITY)++capture_diagnostics[id].errors;
        return false;}
    captures[id]=(capture_t){control,NOVA_CAPTURE_ACTIVE};
    capture_diagnostics[id].owner_id=control->id;
    capture_diagnostics[id].begin_ms=input_clock_ms;
    capture_diagnostics[id].end_ms=0;capture_diagnostics[id].duration_ms=0;
    capture_diagnostics[id].state=NOVA_CAPTURE_ACTIVE;
    ++diagnostics.captures;return true;
}

void nova_input_pointer_release(uint32_t id)
{if(id<NOVA_INPUT_POINTER_CAPACITY&&captures[id].state==NOVA_CAPTURE_ACTIVE){
    captures[id].owner=0;captures[id].state=NOVA_CAPTURE_RELEASED;
    capture_diagnostics[id].end_ms=input_clock_ms;
    capture_diagnostics[id].duration_ms=input_clock_ms-capture_diagnostics[id].begin_ms;
    capture_diagnostics[id].state=NOVA_CAPTURE_RELEASED;
    ++capture_diagnostics[id].releases;
    ++diagnostics.capture_releases;}}

void nova_input_pointer_cancel(uint32_t id)
{if(id<NOVA_INPUT_POINTER_CAPACITY&&captures[id].state==NOVA_CAPTURE_ACTIVE){
    nova_control_t *owner=captures[id].owner;
    if(owner&&owner->state!=NOVA_CONTROL_DESTROYED){nova_control_event_t cancel={
        .type=NOVA_CONTROL_EVENT_CAPTURE_CANCEL,.device_id=2,.value=(int32_t)id};
        (void)nova_control_dispatch_event(owner,&cancel);}
    captures[id].owner=0;captures[id].state=NOVA_CAPTURE_CANCELLED;
    capture_diagnostics[id].end_ms=input_clock_ms;
    capture_diagnostics[id].duration_ms=input_clock_ms-capture_diagnostics[id].begin_ms;
    capture_diagnostics[id].state=NOVA_CAPTURE_CANCELLED;
    ++capture_diagnostics[id].cancels;
    ++diagnostics.capture_cancels;}}
void nova_input_pointer_cancel_all(void)
{for(uint32_t i=0;i<NOVA_INPUT_POINTER_CAPACITY;++i)nova_input_pointer_cancel(i);}
void nova_input_advance_time(uint32_t elapsed_ms){input_clock_ms+=elapsed_ms;}
bool nova_input_pointer_is_captured(uint32_t id)
{return id<NOVA_INPUT_POINTER_CAPACITY&&captures[id].state==NOVA_CAPTURE_ACTIVE;}
nova_control_t *nova_input_pointer_capture_owner(uint32_t id)
{if(!nova_input_pointer_is_captured(id))return 0;
 if(captures[id].owner->state==NOVA_CONTROL_DESTROYED){nova_input_pointer_cancel(id);return 0;}
 return captures[id].owner;}
nova_capture_state_t nova_input_pointer_capture_state(uint32_t id)
{return id<NOVA_INPUT_POINTER_CAPACITY?captures[id].state:NOVA_CAPTURE_INACTIVE;}
const nova_capture_diagnostics_t *nova_input_pointer_capture_diagnostics(uint32_t id)
{return id<NOVA_INPUT_POINTER_CAPACITY?&capture_diagnostics[id]:0;}

nova_control_t *nova_input_pointer_target(const nova_input_event_t *event)
{
    if (!event || (event->type < NOVA_EVENT_POINTER_MOVE || event->type > NOVA_EVENT_POINTER_WHEEL)) return 0;
    nova_control_t *captured=nova_input_pointer_capture_owner(event->pointer.pointer_id);
    if(captured)return captured;
    return nova_control_hit_test(event->pointer.x,event->pointer.y);
}

int32_t nova_input_pointer_scale_delta(int32_t delta,uint16_t speed_percent)
{
    if(!delta||speed_percent<25||speed_percent>400)return 0;
    /* mouse_speed_percent is an actual percentage.  At 100 percent one
       firmware movement unit therefore maps to one logical pixel. */
    int64_t scaled=(int64_t)delta*speed_percent/100;
    if(!scaled)scaled=delta<0?-1:1;
    if(scaled>INT32_MAX)return INT32_MAX;
    if(scaled<INT32_MIN)return INT32_MIN;
    return (int32_t)scaled;
}

bool nova_input_pointer_click_update(uint32_t device_id,uint32_t pointer_id,nova_control_t *target,
    int32_t x,int32_t y,uint64_t now_ms,bool button_down,uint32_t threshold_ms,
    uint32_t tolerance_pixels)
{
    if(pointer_id>=NOVA_INPUT_POINTER_CAPACITY||!target||threshold_ms<1)return false;
    nova_pointer_click_state_t *state=&pointer_clicks[pointer_id];
    if(button_down){
        int64_t dx=(int64_t)x-state->x,dy=(int64_t)y-state->y;
        if(dx<0)dx=-dx;
        if(dy<0)dy=-dy;
        bool matched=state->armed&&state->target==target&&
            now_ms-state->released_ms<=threshold_ms&&
            (uint64_t)dx<=tolerance_pixels&&(uint64_t)dy<=tolerance_pixels;
        if(matched){state->armed=false;state->suppress_release=true;
            ++diagnostics.double_clicks;
            if(nova_input_trace_enabled())
                trace_store((nova_input_trace_event_t){.event_id=++sequence,
                    .capture_us=now_ms*1000u,.dispatch_us=now_ms*1000u,
                    .handler_end_us=now_ms*1000u,.completion_us=now_ms*1000u,
                    .device_id=device_id,.target_object=target->id,
                    .type=NOVA_TRACE_MOUSE_DOUBLE_CLICK,.device_type=NOVA_DEVICE_MOUSE,
                    .x=x,.y=y,.pointer_id=pointer_id,.buttons=1,.completed=true});
            return true;}
        state->armed=false;
        return false;
    }
    if(state->suppress_release){state->suppress_release=false;return false;}
    state->target=target;state->released_ms=now_ms;state->x=x;state->y=y;
    state->armed=true;return false;
}

bool nova_input_focus_initialize(void)
{focused=0;focus_scope_depth=0;focus_changing=false;
 focus_diagnostics=(nova_focus_diagnostics_t){.current_focus=NOVA_CONTROL_NONE,
    .current_scope=NOVA_CONTROL_NONE};
 for(uint8_t i=0;i<NOVA_FOCUS_SCOPE_CAPACITY;++i)focus_scopes[i]=0;
 return true;}
static int16_t focus_order_index(const nova_focus_scope_t *scope,
    const nova_control_t *control)
{if(!scope||!control)return -1;
 for(uint16_t i=0;i<scope->count;++i)
    if(scope->order[i]==control)return (int16_t)i;
 return -1;}
bool nova_input_focus_set(nova_control_t *control)
{ if(focus_changing||!nova_control_focusable(control)){
    ++trace_status.invalid_focus;++focus_diagnostics.errors;return false;}
  nova_focus_scope_t *scope=focus_scope_depth?focus_scopes[focus_scope_depth-1]:0;
  int16_t index=scope?focus_order_index(scope,control):0;
  if(scope&&index<0){++trace_status.invalid_focus;++focus_diagnostics.errors;return false;}
  focus_changing=true;focus_diagnostics.changing=true;
  if(!nova_control_focus(control)){focus_changing=false;focus_diagnostics.changing=false;
    ++trace_status.invalid_focus;++focus_diagnostics.errors;return false;}
  uint16_t previous=focused?focused->id:NOVA_CONTROL_NONE;
  if(focused&&focused!=control)repeat_stop_all(input_clock_ms,1);
  focused=control;
  if(scope)scope->current_index=(uint16_t)index;
  focus_diagnostics.current_focus=control->id;focus_diagnostics.focus_visible=true;
  ++focus_diagnostics.changes;++focus_diagnostics.announcements;
  trace_store((nova_input_trace_event_t){.event_id=++sequence,.type=NOVA_TRACE_FOCUS_CHANGED,
    .previous_focus=previous,.new_focus=control->id,.focus_object=control->id,
    .device_type=NOVA_DEVICE_ACCESSIBILITY,.completed=true});
  focus_changing=false;focus_diagnostics.changing=false;return true; }
nova_control_t *nova_input_focus_get(void){return focused;}
bool nova_input_focus_scope_configure(nova_focus_scope_t *scope,uint16_t id,
 nova_control_t *default_control)
{if(!scope||id==NOVA_CONTROL_NONE)return false;
 for(uint8_t i=0;i<focus_scope_depth;++i)if(focus_scopes[i]==scope)return false;
 *scope=(nova_focus_scope_t){.id=id,.default_control=default_control};return true;}
bool nova_input_focus_scope_register(nova_focus_scope_t *scope,nova_control_t *control)
{if(!scope||scope->active||!nova_control_focusable(control)||
    scope->count>=NOVA_FOCUS_ORDER_CAPACITY||focus_order_index(scope,control)>=0)return false;
 scope->order[scope->count++]=control;return true;}
static bool direction_candidate(const nova_rect_t *from,const nova_rect_t *to,
 nova_direction_t direction,int64_t *primary,int64_t *secondary)
{
 int64_t fx=(int64_t)from->x*2+from->width,fy=(int64_t)from->y*2+from->height;
 int64_t tx=(int64_t)to->x*2+to->width,ty=(int64_t)to->y*2+to->height;
 int64_t dx=tx-fx,dy=ty-fy;
 if(direction==NOVA_DIRECTION_UP&&dy<0){*primary=-dy;*secondary=dx<0?-dx:dx;return true;}
 if(direction==NOVA_DIRECTION_DOWN&&dy>0){*primary=dy;*secondary=dx<0?-dx:dx;return true;}
 if(direction==NOVA_DIRECTION_LEFT&&dx<0){*primary=-dx;*secondary=dy<0?-dy:dy;return true;}
 if(direction==NOVA_DIRECTION_RIGHT&&dx>0){*primary=dx;*secondary=dy<0?-dy:dy;return true;}
 return false;
}
bool nova_input_focus_scope_build_navigation(nova_focus_scope_t *scope,
 nova_navigation_edge_t edge)
{
 if(!scope||scope->active||!scope->count||edge>NOVA_NAV_EDGE_CUSTOM)return false;
 for(uint16_t s=0;s<scope->count;++s){nova_control_t *source=scope->order[s];
  for(uint8_t d=0;d<4;++d){if(neighbors[source->id][d].explicit)continue;
   nova_control_t *best=0,*wrap=0;int64_t best_score=INT64_MAX,wrap_score=INT64_MIN;
   for(uint16_t t=0;t<scope->count;++t)if(t!=s&&nova_control_focusable(scope->order[t])){
    nova_control_t *candidate=scope->order[t];int64_t primary=0,secondary=0;
    if(direction_candidate(&source->bounds,&candidate->bounds,(nova_direction_t)d,
        &primary,&secondary)){int64_t score=primary*4096+secondary;
        if(score<best_score){best_score=score;best=candidate;}}
    nova_direction_t opposite=d==NOVA_DIRECTION_UP?NOVA_DIRECTION_DOWN:
        d==NOVA_DIRECTION_DOWN?NOVA_DIRECTION_UP:d==NOVA_DIRECTION_LEFT?
        NOVA_DIRECTION_RIGHT:NOVA_DIRECTION_LEFT;
    if(direction_candidate(&source->bounds,&candidate->bounds,opposite,&primary,&secondary)){
        int64_t score=primary*4096-secondary;if(score>wrap_score){wrap_score=score;wrap=candidate;}}
   }
   neighbors[source->id][d].target=best?best:(edge==NOVA_NAV_EDGE_WRAP?wrap:0);
   neighbors[source->id][d].wrap=!best&&edge==NOVA_NAV_EDGE_WRAP&&wrap;
  }
  if(!neighbors[source->id][NOVA_DIRECTION_FORWARD].explicit)
   neighbors[source->id][NOVA_DIRECTION_FORWARD].target=scope->order[(s+1u)%scope->count];
  if(!neighbors[source->id][NOVA_DIRECTION_BACKWARD].explicit)
   neighbors[source->id][NOVA_DIRECTION_BACKWARD].target=scope->order[s?s-1u:scope->count-1u];
 }
 return true;
}
bool nova_input_focus_enter_scope(nova_focus_scope_t *scope)
{if(!scope||scope->active||!scope->count||
    focus_scope_depth>=NOVA_FOCUS_SCOPE_CAPACITY)return false;
 nova_control_t *target=scope->default_control?scope->default_control:scope->order[0];
 if(focus_order_index(scope,target)<0||!nova_control_focusable(target))return false;
 scope->saved_focus=focused;focus_scopes[focus_scope_depth++]=scope;scope->active=true;
 focus_diagnostics.scope_depth=focus_scope_depth;focus_diagnostics.current_scope=scope->id;
 if(nova_input_focus_set(target))return true;
 scope->active=false;focus_scopes[--focus_scope_depth]=0;return false;}
bool nova_input_focus_leave_scope(void)
{if(!focus_scope_depth)return false;nova_focus_scope_t *scope=focus_scopes[focus_scope_depth-1];
 nova_control_t *restore=scope->saved_focus;scope->active=false;
 focus_scopes[--focus_scope_depth]=0;focus_diagnostics.scope_depth=focus_scope_depth;
 nova_focus_scope_t *parent=focus_scope_depth?focus_scopes[focus_scope_depth-1]:0;
 focus_diagnostics.current_scope=parent?parent->id:NOVA_CONTROL_NONE;
 if(!nova_control_focusable(restore)||(parent&&focus_order_index(parent,restore)<0))
    restore=parent?(parent->default_control?parent->default_control:parent->order[0]):0;
 if(!restore){focused=0;focus_diagnostics.current_focus=NOVA_CONTROL_NONE;
    ++focus_diagnostics.errors;return false;}
 if(!nova_input_focus_set(restore))return false;
 ++focus_diagnostics.restores;return true;}
static bool focus_step(bool previous)
{nova_focus_scope_t *scope=focus_scope_depth?focus_scopes[focus_scope_depth-1]:0;
 if(!scope||!scope->count)return false;
 uint16_t index=scope->current_index;
 index=previous?(index?index-1u:scope->count-1u):(uint16_t)((index+1u)%scope->count);
 ++focus_diagnostics.navigations;return nova_input_focus_set(scope->order[index]);}
bool nova_input_focus_next(void){return focus_step(false);}
bool nova_input_focus_previous(void){return focus_step(true);}
bool nova_input_focus_visible(void){return focus_diagnostics.focus_visible;}
const nova_focus_scope_t *nova_input_focus_scope(void)
{return focus_scope_depth?focus_scopes[focus_scope_depth-1]:0;}
const nova_focus_diagnostics_t *nova_input_focus_diagnostics(void)
{return &focus_diagnostics;}

bool nova_input_neighbor_set(nova_control_t *source, nova_direction_t direction, nova_control_t *target)
{
    if(!nova_control_focusable(source)||!nova_control_focusable(target)||
       direction>NOVA_DIRECTION_BACKWARD||neighbors[source->id][direction].explicit)return false;
    neighbors[source->id][direction]=(neighbor_t){target,true,false};return true;
}
nova_control_t *nova_input_neighbor_find(nova_control_t *source,nova_direction_t direction)
{if(!source||source->id>=NOVA_CONTROL_CAPACITY||direction>NOVA_DIRECTION_BACKWARD)return 0;
 return neighbors[source->id][direction].target;}
bool nova_input_focus_navigate(nova_direction_t direction)
{
    if(directional_navigating||!focused||direction>NOVA_DIRECTION_BACKWARD){
        ++directional_diagnostics.errors;return false;}
    directional_navigating=true;directional_diagnostics.navigating=true;
    directional_diagnostics.source=focused->id;directional_diagnostics.direction=direction;
    const nova_focus_scope_t *scope=nova_input_focus_scope();
    directional_diagnostics.scope=scope?scope->id:NOVA_CONTROL_NONE;
    neighbor_t link=neighbors[focused->id][direction];nova_control_t *target=link.target;
    if(!target||!nova_control_focusable(target)||(scope&&focus_order_index(scope,target)<0)){
        ++directional_diagnostics.failed;directional_diagnostics.target=NOVA_CONTROL_NONE;
        directional_navigating=false;directional_diagnostics.navigating=false;return false;}
    bool result=nova_input_focus_set(target);directional_diagnostics.target=target->id;
    if(result){++directional_diagnostics.navigations;
        if(link.explicit)++directional_diagnostics.explicit_hits;
        else ++directional_diagnostics.geometric_hits;}
    if(result&&link.wrap)++directional_diagnostics.wraps;
    else ++directional_diagnostics.failed;
    directional_navigating=false;directional_diagnostics.navigating=false;return result;
}
const nova_directional_diagnostics_t *nova_input_directional_diagnostics(void)
{return &directional_diagnostics;}

static uint32_t shortcut_hash(uint32_t key,uint32_t modifiers,
 nova_shortcut_scope_t scope,uint16_t context,uint16_t focus)
{uint32_t hash=key*2654435761u^(modifiers*2246822519u)^((uint32_t)scope*3266489917u);
 hash^=(uint32_t)context*668265263u^(uint32_t)focus*374761393u;
 return hash&(NOVA_INPUT_SHORTCUT_HASH_CAPACITY-1u);}
static bool shortcut_identity(const nova_shortcut_t *value,uint32_t key,uint32_t modifiers,
 nova_shortcut_scope_t scope,uint16_t context,uint16_t focus)
{return value->key==key&&value->modifiers==modifiers&&value->scope==scope&&
 value->context_id==context&&value->focus_id==focus;}
static shortcut_t *shortcut_find(uint32_t key,uint32_t modifiers,
 nova_shortcut_scope_t scope,uint16_t context,uint16_t focus)
{uint32_t start=shortcut_hash(key,modifiers,scope,context,focus);
 for(uint32_t probe=0;probe<NOVA_INPUT_SHORTCUT_HASH_CAPACITY;++probe){
  shortcut_t *slot=&shortcuts[(start+probe)&(NOVA_INPUT_SHORTCUT_HASH_CAPACITY-1u)];
  if(!slot->used&&!slot->tombstone)return 0;
  if(slot->used&&shortcut_identity(&slot->value,key,modifiers,scope,context,focus))return slot;
 }return 0;}
bool nova_input_shortcut_register_ex(const nova_shortcut_t *value)
{
 if(!value||!value->key||!value->command||value->scope>NOVA_SHORTCUT_DEVELOPER||
    (value->focus_dependent&&value->focus_id==NOVA_CONTROL_NONE)){
    ++shortcut_diagnostics.failed;return false;}
 uint16_t focus=value->focus_dependent?value->focus_id:NOVA_CONTROL_NONE;
 if(shortcut_find(value->key,value->modifiers,value->scope,value->context_id,focus)){
    ++shortcut_diagnostics.conflicts;return false;}
 if(shortcut_diagnostics.registered-shortcut_diagnostics.unregistered>=
    NOVA_INPUT_SHORTCUT_CAPACITY){++shortcut_diagnostics.failed;return false;}
 uint32_t start=shortcut_hash(value->key,value->modifiers,value->scope,value->context_id,focus);
 for(uint32_t probe=0;probe<NOVA_INPUT_SHORTCUT_HASH_CAPACITY;++probe){
  shortcut_t *slot=&shortcuts[(start+probe)&(NOVA_INPUT_SHORTCUT_HASH_CAPACITY-1u)];
  if(!slot->used){*slot=(shortcut_t){.value=*value,.used=true};
    slot->value.focus_id=focus;slot->value.enabled=true;
    ++shortcut_diagnostics.registered;return true;}
 }
 ++shortcut_diagnostics.failed;return false;
}
bool nova_input_shortcut_register(uint32_t key,uint32_t modifiers,uint8_t scope,uint32_t command)
{nova_shortcut_t value={.key=key,.modifiers=modifiers,.command=command,
 .scope=(nova_shortcut_scope_t)scope,.focus_id=NOVA_CONTROL_NONE,.enabled=true};
 return nova_input_shortcut_register_ex(&value);}
bool nova_input_shortcut_unregister(uint32_t key,uint32_t modifiers,
 nova_shortcut_scope_t scope,uint16_t context,uint16_t focus)
{shortcut_t *slot=shortcut_find(key,modifiers,scope,context,focus);
 if(!slot){++shortcut_diagnostics.failed;return false;}
 slot->used=false;slot->tombstone=true;++shortcut_diagnostics.unregistered;return true;}
void nova_input_shortcut_set_context(uint16_t context,uint16_t dialog)
{shortcut_context_id=context;shortcut_dialog_id=dialog;}
void nova_input_shortcut_set_developer(bool enabled,bool authorized)
{if(enabled&&!authorized){++shortcut_diagnostics.unauthorized;return;}
 shortcut_diagnostics.developer_enabled=enabled;}
bool nova_input_shortcut_set_handler(nova_shortcut_command_handler_t handler,void *context)
{if(!handler)return false;shortcut_handler=handler;shortcut_handler_context=context;return true;}
static shortcut_t *shortcut_resolve_scope(const nova_input_event_t *event,
 nova_shortcut_scope_t scope,uint16_t context)
{uint16_t focus=focused?focused->id:NOVA_CONTROL_NONE;
 shortcut_t *slot=shortcut_find(event->key.key,event->key.modifiers,scope,context,focus);
 if(slot&&slot->value.enabled)return slot;
 slot=shortcut_find(event->key.key,event->key.modifiers,scope,context,NOVA_CONTROL_NONE);
 return slot&&slot->value.enabled?slot:0;}
bool nova_input_shortcut_resolve(const nova_input_event_t *event,uint32_t *command)
{
 if(!event||event->type!=NOVA_EVENT_KEY_DOWN){++shortcut_diagnostics.failed;return false;}
 shortcut_t *slot=0;
 if(shortcut_dialog_id)slot=shortcut_resolve_scope(event,NOVA_SHORTCUT_DIALOG,shortcut_dialog_id);
 if(!slot&&shortcut_diagnostics.developer_enabled)
    slot=shortcut_resolve_scope(event,NOVA_SHORTCUT_DEVELOPER,shortcut_context_id);
 if(!slot&&shortcut_context_id)
    slot=shortcut_resolve_scope(event,NOVA_SHORTCUT_CONTEXT,shortcut_context_id);
 if(!slot)slot=shortcut_resolve_scope(event,NOVA_SHORTCUT_GLOBAL,0);
 if(!slot){++shortcut_diagnostics.failed;return false;}
 if(command)*command=slot->value.command;
 ++diagnostics.shortcuts;
 ++shortcut_diagnostics.executions;++shortcut_diagnostics.announcements;
 shortcut_diagnostics.last_key=event->key.key;
 shortcut_diagnostics.last_modifiers=event->key.modifiers;
 shortcut_diagnostics.last_command=slot->value.command;
 shortcut_diagnostics.last_context=slot->value.context_id;
 shortcut_diagnostics.last_focus=focused?focused->id:NOVA_CONTROL_NONE;
 shortcut_diagnostics.last_scope=(uint8_t)slot->value.scope;
 trace_store((nova_input_trace_event_t){.event_id=++sequence,.capture_us=event->timestamp,
    .type=NOVA_TRACE_SHORTCUT,.device_id=event->device_id,
    .device_type=device_type(event->device_id),.shortcut_id=slot->value.command,
    .scan_code=event->key.key,.modifiers=event->key.modifiers,.completed=true});
 return true;
}
bool nova_input_shortcut_execute(const nova_input_event_t *event)
{if(shortcut_dispatching){++shortcut_diagnostics.recursive;return false;}
 uint32_t command=0;if(!shortcut_handler||!nova_input_shortcut_resolve(event,&command))return false;
 shortcut_dispatching=true;shortcut_diagnostics.dispatching=true;
 bool result=shortcut_handler(command,shortcut_handler_context);
 shortcut_dispatching=false;shortcut_diagnostics.dispatching=false;
 if(!result)++shortcut_diagnostics.failed;
 return result;}
const nova_shortcut_diagnostics_t *nova_input_shortcut_diagnostics(void)
{return &shortcut_diagnostics;}

static repeat_t *repeat_find(uint32_t id)
{if(!id)return 0;
 for(uint8_t i=0;i<NOVA_INPUT_REPEAT_CAPACITY;++i)
    if(repeats[i].used&&repeats[i].context.input_id==id)return &repeats[i];
 return 0;}
static bool repeat_event_allowed(const nova_input_event_t *event)
{return event&&(event->type==NOVA_EVENT_KEY_DOWN||event->type==NOVA_EVENT_CHARACTER||
 event->type==NOVA_EVENT_NAVIGATION||event->type==NOVA_EVENT_CONTROLLER_BUTTON||
 event->type==NOVA_EVENT_CONTROLLER_AXIS||event->type==NOVA_EVENT_POINTER_WHEEL);}
bool nova_input_repeat_configure(uint32_t delay,uint32_t interval,bool enabled)
{if(delay<100||delay>5000||interval<20||interval>2000){
 ++repeat_diagnostics.errors;return false;}
 repeat_diagnostics.initial_delay_ms=delay;repeat_diagnostics.interval_ms=interval;
 repeat_diagnostics.enabled=enabled;if(!enabled)repeat_stop_all(repeat_clock_ms,0);
 return true;}
bool nova_input_repeat_start_event(uint32_t id,const nova_input_event_t *event,
 uint64_t now,bool critical)
{
 if(!id||!repeat_event_allowed(event)||critical||!repeat_diagnostics.enabled||
    !nova_input_device_connected(event->device_id)||repeat_find(id)){
    ++repeat_diagnostics.rejected;return false;}
 for(uint8_t i=0;i<NOVA_INPUT_REPEAT_CAPACITY;++i)if(!repeats[i].used||
    !repeats[i].context.active){repeats[i]=(repeat_t){.used=true,.event=*event,
      .context={.input_id=id,.device_id=event->device_id,
       .target_id=focused?focused->id:NOVA_CONTROL_NONE,.start_time=now,
       .next_repeat=now+repeat_diagnostics.initial_delay_ms,
       .state=NOVA_REPEAT_WAITING,.active=true,.critical=false}};
    ++repeat_diagnostics.starts;return true;}
 ++repeat_diagnostics.errors;return false;
}
bool nova_input_repeat_start(uint32_t id,uint64_t now)
{nova_input_event_t event={.device_id=1,.type=NOVA_EVENT_KEY_DOWN};event.key.key=id;
 return nova_input_repeat_start_event(id,&event,now,false);}
void nova_input_repeat_stop(uint32_t id)
{repeat_t *repeat=repeat_find(id);if(!repeat||!repeat->context.active)return;
 repeat->context.active=false;repeat->context.state=NOVA_REPEAT_STOPPED;
 repeat->context.stop_time=repeat_clock_ms;
 repeat->context.duration_ms=repeat_clock_ms-repeat->context.start_time;
 ++repeat_diagnostics.stops;}
static void repeat_stop_all(uint64_t now,uint8_t reason)
{repeat_clock_ms=now;for(uint8_t i=0;i<NOVA_INPUT_REPEAT_CAPACITY;++i)
 if(repeats[i].used&&repeats[i].context.active){nova_input_repeat_stop(
    repeats[i].context.input_id);if(reason==1)++repeat_diagnostics.focus_stops;
    else if(reason==2)++repeat_diagnostics.device_stops;}}
bool nova_input_repeat_pause(uint32_t id)
{repeat_t *repeat=repeat_find(id);if(!repeat||!repeat->context.active||
 repeat->context.state==NOVA_REPEAT_PAUSED)return false;
 repeat->context.state=NOVA_REPEAT_PAUSED;++repeat_diagnostics.pauses;return true;}
bool nova_input_repeat_resume(uint32_t id,uint64_t now)
{repeat_t *repeat=repeat_find(id);if(!repeat||!repeat->context.active||
 repeat->context.state!=NOVA_REPEAT_PAUSED)return false;
 repeat->context.state=NOVA_REPEAT_WAITING;
 repeat->context.next_repeat=now+repeat_diagnostics.interval_ms;
 ++repeat_diagnostics.resumes;return true;}
bool nova_input_repeat_active(uint32_t id)
{repeat_t *repeat=repeat_find(id);return repeat&&repeat->context.active;}
void nova_input_repeat_update(uint64_t now)
{
 repeat_clock_ms=now;input_clock_ms=now;
 for(uint8_t i=0;i<NOVA_INPUT_REPEAT_CAPACITY;++i){repeat_t *repeat=&repeats[i];
  if(!repeat->used||!repeat->context.active||repeat->context.state==NOVA_REPEAT_PAUSED||
     now<repeat->context.next_repeat)continue;
  if(!nova_input_device_connected(repeat->context.device_id)||
     (repeat->context.target_id!=NOVA_CONTROL_NONE&&
      (!focused||focused->id!=repeat->context.target_id))){
      nova_input_repeat_stop(repeat->context.input_id);++repeat_diagnostics.device_stops;
      continue;}
  nova_input_event_t event=repeat->event;event.timestamp=now;event.flags|=1u;
  if(event.type==NOVA_EVENT_KEY_DOWN||event.type==NOVA_EVENT_CHARACTER)event.key.repeat=true;
  if(nova_input_post(event)){repeat->context.state=NOVA_REPEAT_REPEATING;
    ++repeat->context.count;++repeat_diagnostics.repeats;++diagnostics.repeats;}
  else{++repeat->context.errors;++repeat_diagnostics.errors;}
  repeat->context.next_repeat=now+repeat_diagnostics.interval_ms;
 }
}
const nova_repeat_context_t *nova_input_repeat_context(uint32_t id)
{repeat_t *repeat=repeat_find(id);return repeat?&repeat->context:0;}
const nova_repeat_diagnostics_t *nova_input_repeat_diagnostics(void)
{return &repeat_diagnostics;}
void nova_input_set_secure(bool enabled){secure_mode=enabled;}
const nova_input_diagnostics_t *nova_input_diagnostics(void){return &diagnostics;}

bool nova_input_trace_initialize(void)
{trace_head=trace_count=0;last_trace_signature=0;trace_status=(nova_input_trace_status_t){
 .initialized=true,.passive=true,.boot_unaffected=true};return true;}
bool nova_input_trace_set_enabled(bool enabled,bool authorized)
{if(!authorized){++trace_status.denied_exports;return false;}trace_status.enabled=enabled;return true;}
bool nova_input_trace_enabled(void){return trace_status.enabled;}
const nova_input_trace_event_t *nova_input_trace_get(uint32_t index)
{return index<trace_count?&trace_events[(trace_head+index)%NOVA_INPUT_TRACE_CAPACITY]:0;}
static bool trace_matches(const nova_input_trace_event_t *e,const nova_input_trace_filter_t *f)
{
 if(!f)return true;
 if(f->use_time&&(e->capture_us<f->from_us||e->capture_us>f->to_us))return false;
 if(f->use_device_id&&e->device_id!=f->device_id)return false;
 if(f->use_device_type&&e->device_type!=f->device_type)return false;
 if(f->use_type&&e->type!=f->type)return false;
 if(f->use_target&&e->target_object!=f->target_object)return false;
 if(f->use_window&&e->window_id!=f->window_id)return false;
 if(f->use_dialog&&e->dialog_id!=f->dialog_id)return false;
 if(f->actions_only&&e->type!=NOVA_TRACE_SHORTCUT&&e->type!=NOVA_TRACE_MOUSE_BUTTON&&
    e->type!=NOVA_TRACE_MOUSE_DOUBLE_CLICK&&e->type!=NOVA_TRACE_KEY_DOWN)return false;
 return true;
}
const nova_input_trace_event_t *nova_input_trace_query(const nova_input_trace_filter_t *filter,uint32_t match)
{for(uint32_t i=0;i<trace_count;++i){const nova_input_trace_event_t *e=nova_input_trace_get(i);
 if(trace_matches(e,filter)&&match--==0)return e;}return 0;}
bool nova_input_trace_complete(uint64_t id,uint64_t handler_end,uint64_t completion,
 uint32_t target,uint32_t window,uint32_t dialog,bool render)
{nova_input_trace_event_t *e=trace_find(id);if(!e||completion<e->capture_us||handler_end<e->dispatch_us)return false;
 e->handler_end_us=handler_end;e->completion_us=completion;e->target_object=target;
 e->focus_object=focused?focused->id:NOVA_CONTROL_NONE;e->window_id=window;e->dialog_id=dialog;
 e->queue_wait_us=(uint32_t)(e->dispatch_us-e->capture_us);e->dispatch_duration_us=(uint32_t)(handler_end-e->dispatch_us);
 e->handler_duration_us=(uint32_t)(completion-handler_end);e->render_triggered=render;e->completed=true;
 if(completion-e->capture_us>100000u)++trace_status.latency_warnings;
 if(e->dispatch_duration_us>50000u)++trace_status.dispatch_timeouts;
 return true;
}
static bool trace_append(uint8_t *o,uint32_t c,uint32_t *p,const char *s)
{while(s&&*s){if(*p>=c)return false;o[(*p)++]=(uint8_t)*s++;}return true;}
static bool trace_number(uint8_t *o,uint32_t c,uint32_t *p,uint64_t v)
{char d[21];uint8_t n=0;if(!v)d[n++]='0';while(v){d[n++]=(char)('0'+v%10);v/=10;}
 while(n){if(*p>=c)return false;o[(*p)++]=(uint8_t)d[--n];}return true;}
bool nova_input_trace_export(nova_input_trace_export_t format,bool authorized,uint8_t *o,uint32_t c,uint32_t *written)
{if(written)*written=0;if(!authorized){++trace_status.denied_exports;return false;}
 if(!o||!written||format>NOVA_INPUT_TRACE_BINARY)return false;
 uint32_t p=0;
 if(format==NOVA_INPUT_TRACE_BINARY){const uint8_t h[8]={'N','I','T','B',1,0,(uint8_t)trace_count,(uint8_t)(trace_count>>8)};
  for(uint8_t i=0;i<8;++i){if(p>=c)return false;o[p++]=h[i];}
  for(uint32_t i=0;i<trace_count;++i){const nova_input_trace_event_t *e=nova_input_trace_get(i);
   for(uint8_t b=0;b<8;++b){if(p>=c)return false;o[p++]=(uint8_t)(e->event_id>>(b*8));}}
 }else{const char *h=format==NOVA_INPUT_TRACE_NDF?"NIT/1\n":format==NOVA_INPUT_TRACE_CSV?
  "id,time,type,device,target,focus,queue,latency,masked\n":"{\"inputEvents\":[";if(!trace_append(o,c,&p,h))return false;
  for(uint32_t i=0;i<trace_count;++i){const nova_input_trace_event_t *e=nova_input_trace_get(i);
   if(format==NOVA_INPUT_TRACE_JSON){if(i){if(p>=c)return false;o[p++]=',';}if(!trace_append(o,c,&p,"{\"id\":")||
    !trace_number(o,c,&p,e->event_id)||!trace_append(o,c,&p,",\"type\":")||!trace_number(o,c,&p,e->type)||
    !trace_append(o,c,&p,"}"))return false;
   }else{uint64_t v[9]={e->event_id,e->capture_us,e->type,e->device_id,e->target_object,e->focus_object,
      e->queue_length,e->completion_us>=e->capture_us?e->completion_us-e->capture_us:0,e->masked};
    for(uint8_t n=0;n<9;++n){if(n){if(p>=c)return false;o[p++]=',';}if(!trace_number(o,c,&p,v[n]))return false;}
    if(p>=c)return false;
    o[p++]='\n';}}
  if(format==NOVA_INPUT_TRACE_JSON&&!trace_append(o,c,&p,"]}"))return false;}
 *written=p;++trace_status.exports;return true;}
void nova_input_trace_reset(void){trace_head=trace_count=0;last_trace_signature=0;trace_status.count=0;}
const nova_input_trace_status_t *nova_input_trace_status(void){return &trace_status;}
