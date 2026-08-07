#include "input.h"

typedef struct { uint32_t id; nova_control_t *owner; bool active; } capture_t;
typedef struct { nova_control_t *source, *target; nova_direction_t direction; } neighbor_t;
typedef struct { uint32_t key, modifiers, command; uint8_t scope; bool used; } shortcut_t;
typedef struct { uint32_t id; uint64_t next; bool active; } repeat_t;

static nova_input_event_t queue[NOVA_INPUT_QUEUE_CAPACITY];
static uint16_t head, tail, count;
static uint64_t sequence;
static nova_input_device_t devices[NOVA_INPUT_DEVICE_CAPACITY];
static bool device_used[NOVA_INPUT_DEVICE_CAPACITY];
static capture_t captures[NOVA_INPUT_POINTER_CAPACITY];
static neighbor_t neighbors[NOVA_CONTROL_CAPACITY * 2u];
static uint16_t neighbor_count;
static shortcut_t shortcuts[NOVA_INPUT_SHORTCUT_CAPACITY];
static repeat_t repeats[NOVA_INPUT_REPEAT_CAPACITY];
static nova_control_t *focused;
static nova_input_diagnostics_t diagnostics;
static bool secure_mode;

void nova_input_initialize(void)
{
    head = tail = count = 0; sequence = 0; focused = 0; neighbor_count = 0;
    diagnostics = (nova_input_diagnostics_t){0}; secure_mode = false;
    for (uint32_t i=0;i<NOVA_INPUT_DEVICE_CAPACITY;++i) device_used[i]=false;
    for (uint32_t i=0;i<NOVA_INPUT_POINTER_CAPACITY;++i) captures[i].active=false;
    for (uint32_t i=0;i<NOVA_INPUT_SHORTCUT_CAPACITY;++i) shortcuts[i].used=false;
    for (uint32_t i=0;i<NOVA_INPUT_REPEAT_CAPACITY;++i) repeats[i].active=false;
}

bool nova_input_device_set(uint32_t id, nova_device_type_t type, bool connected)
{
    if (!id || type > NOVA_DEVICE_ACCESSIBILITY) return false;
    for (uint32_t i=0;i<NOVA_INPUT_DEVICE_CAPACITY;++i) {
        if (device_used[i] && devices[i].id == id) {
            devices[i].connected = connected;
            if (!connected) {
                for (uint32_t p=0;p<NOVA_INPUT_POINTER_CAPACITY;++p) captures[p].active=false;
                for (uint32_t r=0;r<NOVA_INPUT_REPEAT_CAPACITY;++r) repeats[r].active=false;
            }
            return true;
        }
        if (!device_used[i]) {
            device_used[i]=true; devices[i]=(nova_input_device_t){id,type,connected};
            ++diagnostics.devices; return true;
        }
    }
    return false;
}

bool nova_input_post(nova_input_event_t event)
{
    if (count == NOVA_INPUT_QUEUE_CAPACITY) { ++diagnostics.dropped; return false; }
    event.sequence = ++sequence;
    if (secure_mode) { event.flags |= 0x80000000u; ++diagnostics.secure_events; }
    queue[tail] = event; tail = (uint16_t)((tail + 1u) % NOVA_INPUT_QUEUE_CAPACITY);
    ++count; ++diagnostics.posted; diagnostics.last_sequence = sequence; return true;
}

bool nova_input_next(nova_input_event_t *event)
{
    if (!event || !count) return false;
    *event = queue[head]; head = (uint16_t)((head + 1u) % NOVA_INPUT_QUEUE_CAPACITY);
    --count; ++diagnostics.dispatched; return true;
}

bool nova_input_pointer_capture(uint32_t id, nova_control_t *control)
{
    if (!control) return false;
    for (uint32_t i=0;i<NOVA_INPUT_POINTER_CAPACITY;++i) {
        if (captures[i].active && captures[i].id == id) return false;
        if (!captures[i].active) { captures[i]=(capture_t){id,control,true};
            ++diagnostics.captures; return true; }
    }
    return false;
}

void nova_input_pointer_release(uint32_t id)
{ for(uint32_t i=0;i<NOVA_INPUT_POINTER_CAPACITY;++i) if(captures[i].active&&captures[i].id==id) captures[i].active=false; }

nova_control_t *nova_input_pointer_target(const nova_input_event_t *event)
{
    if (!event || (event->type < NOVA_EVENT_POINTER_MOVE || event->type > NOVA_EVENT_POINTER_WHEEL)) return 0;
    for(uint32_t i=0;i<NOVA_INPUT_POINTER_CAPACITY;++i)
        if(captures[i].active&&captures[i].id==event->pointer.pointer_id) return captures[i].owner;
    return nova_control_hit_test(event->pointer.x,event->pointer.y);
}

bool nova_input_focus_set(nova_control_t *control)
{ if(!nova_control_focus(control)) return false; focused=control; return true; }
nova_control_t *nova_input_focus_get(void){return focused;}

bool nova_input_neighbor_set(nova_control_t *source, nova_direction_t direction, nova_control_t *target)
{
    if(!source||!target||direction>NOVA_DIRECTION_BACKWARD||neighbor_count>=NOVA_CONTROL_CAPACITY*2u)return false;
    for(uint16_t i=0;i<neighbor_count;++i) if(neighbors[i].source==source&&neighbors[i].direction==direction)return false;
    neighbors[neighbor_count++]=(neighbor_t){source,target,direction}; return true;
}

bool nova_input_focus_navigate(nova_direction_t direction)
{
    if(!focused)return false;
    for(uint16_t i=0;i<neighbor_count;++i) if(neighbors[i].source==focused&&neighbors[i].direction==direction)
        return nova_input_focus_set(neighbors[i].target);
    return false;
}

bool nova_input_shortcut_register(uint32_t key,uint32_t modifiers,uint8_t scope,uint32_t command)
{
    if(!key||!command||scope>3)return false;
    for(uint32_t i=0;i<NOVA_INPUT_SHORTCUT_CAPACITY;++i){
        if(shortcuts[i].used&&shortcuts[i].key==key&&shortcuts[i].modifiers==modifiers&&shortcuts[i].scope==scope)return false;
        if(!shortcuts[i].used){shortcuts[i]=(shortcut_t){key,modifiers,command,scope,true};return true;}
    } return false;
}

bool nova_input_shortcut_resolve(const nova_input_event_t *event,uint32_t *command)
{
    if(!event||event->type!=NOVA_EVENT_KEY_DOWN)return false;
    for(uint32_t i=0;i<NOVA_INPUT_SHORTCUT_CAPACITY;++i)if(shortcuts[i].used&&
       shortcuts[i].key==event->key.key&&shortcuts[i].modifiers==event->key.modifiers){
        if(command) *command=shortcuts[i].command;
        ++diagnostics.shortcuts;
        return true;
    }
    return false;
}

bool nova_input_repeat_start(uint32_t id,uint64_t now)
{
    if(!id)return false;
    for(uint32_t i=0;i<NOVA_INPUT_REPEAT_CAPACITY;++i){
        if(repeats[i].active&&repeats[i].id==id)return false;
        if(!repeats[i].active){repeats[i]=(repeat_t){id,now+400,true};return true;}
    }return false;
}
void nova_input_repeat_stop(uint32_t id){for(uint32_t i=0;i<NOVA_INPUT_REPEAT_CAPACITY;++i)if(repeats[i].id==id)repeats[i].active=false;}
void nova_input_repeat_update(uint64_t now)
{
    for(uint32_t i=0;i<NOVA_INPUT_REPEAT_CAPACITY;++i)if(repeats[i].active&&now>=repeats[i].next){
        nova_input_event_t e={.timestamp=now,.device_id=1,.type=NOVA_EVENT_KEY_DOWN};
        e.key.key=repeats[i].id;e.key.repeat=true;nova_input_post(e);repeats[i].next=now+50;++diagnostics.repeats;}
}
void nova_input_set_secure(bool enabled){secure_mode=enabled;}
const nova_input_diagnostics_t *nova_input_diagnostics(void){return &diagnostics;}
