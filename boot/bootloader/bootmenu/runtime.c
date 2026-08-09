#include "runtime.h"
#include "recovery.h"
#include "state_model.h"

static nova_runtime_diagnostics_t diagnostics;
static nova_runtime_task_t tasks[NOVA_RUNTIME_TASK_CAPACITY];
static nova_state_object_t *runtime_state;

static bool transition(nova_runtime_state_t expected,nova_runtime_state_t next)
{
    if(diagnostics.state!=expected||!runtime_state||
       !nova_state_transition(runtime_state,(uint8_t)next)){
        ++diagnostics.invalid_transitions;return false;}
    diagnostics.state=next;++diagnostics.transitions;
    diagnostics.input_allowed=next==NOVA_RUNTIME_RUNNING;return true;
}

void nova_runtime_create(void)
{
    diagnostics=(nova_runtime_diagnostics_t){.state=NOVA_RUNTIME_CREATED,
        .frame_stage=NOVA_FRAME_INPUT};
    nova_state_model_initialize();
    runtime_state=nova_state_create(1,NOVA_STATE_DOMAIN_RUNTIME,
                                    NOVA_RUNTIME_CREATED,1,NOVA_STATE_NONE);
    for(uint8_t i=0;i<NOVA_RUNTIME_TASK_CAPACITY;++i)
        tasks[i]=(nova_runtime_task_t){0};
}
bool nova_runtime_begin_initialization(void)
{return transition(NOVA_RUNTIME_CREATED,NOVA_RUNTIME_INITIALIZING);}

static uint32_t dependencies(nova_runtime_subsystem_t subsystem)
{
    switch(subsystem){
    case NOVA_RUNTIME_MEMORY:return 0;
    case NOVA_RUNTIME_PLATFORM:return 1u<<NOVA_RUNTIME_MEMORY;
    case NOVA_RUNTIME_GRAPHICS:return (1u<<NOVA_RUNTIME_MEMORY)|(1u<<NOVA_RUNTIME_PLATFORM);
    case NOVA_RUNTIME_DIAGNOSTICS:return 1u<<NOVA_RUNTIME_MEMORY;
    case NOVA_RUNTIME_CONFIGURATION:return (1u<<NOVA_RUNTIME_MEMORY)|
        (1u<<NOVA_RUNTIME_DIAGNOSTICS);
    case NOVA_RUNTIME_RESOURCES:return (1u<<NOVA_RUNTIME_GRAPHICS)|
        (1u<<NOVA_RUNTIME_CONFIGURATION);
    case NOVA_RUNTIME_SCENE:return 1u<<NOVA_RUNTIME_RESOURCES;
    case NOVA_RUNTIME_LAYOUT_ENGINE:return 1u<<NOVA_RUNTIME_SCENE;
    case NOVA_RUNTIME_MOTION:return 1u<<NOVA_RUNTIME_LAYOUT_ENGINE;
    case NOVA_RUNTIME_INPUT:return 1u<<NOVA_RUNTIME_MOTION;
    case NOVA_RUNTIME_RENDERER:return (1u<<NOVA_RUNTIME_INPUT)|
        (1u<<NOVA_RUNTIME_GRAPHICS);
    default:return UINT32_MAX;
    }
}

bool nova_runtime_subsystem_ready(nova_runtime_subsystem_t subsystem)
{
    if(subsystem>=NOVA_RUNTIME_SUBSYSTEM_COUNT||
       (diagnostics.state!=NOVA_RUNTIME_INITIALIZING&&
        diagnostics.state!=NOVA_RUNTIME_LOADING&&
        diagnostics.state!=NOVA_RUNTIME_BUILDING_SCENE&&
        diagnostics.state!=NOVA_RUNTIME_LAYOUT)){
        ++diagnostics.initialization_failures;return false;}
    uint32_t bit=1u<<subsystem,required=dependencies(subsystem);
    if((diagnostics.ready_subsystems&bit)||
       (diagnostics.ready_subsystems&required)!=required){
        ++diagnostics.initialization_failures;return false;}
    diagnostics.ready_subsystems|=bit;return true;
}
bool nova_runtime_loading(void)
{return transition(NOVA_RUNTIME_INITIALIZING,NOVA_RUNTIME_LOADING);}
bool nova_runtime_building_scene(void)
{return transition(NOVA_RUNTIME_LOADING,NOVA_RUNTIME_BUILDING_SCENE);}
bool nova_runtime_layout(void)
{return transition(NOVA_RUNTIME_BUILDING_SCENE,NOVA_RUNTIME_LAYOUT);}
bool nova_runtime_ready(void)
{
    uint32_t all=(1u<<NOVA_RUNTIME_SUBSYSTEM_COUNT)-1u;
    if(diagnostics.ready_subsystems!=all){++diagnostics.initialization_failures;return false;}
    return transition(NOVA_RUNTIME_LAYOUT,NOVA_RUNTIME_READY);
}
bool nova_runtime_run(void){return transition(NOVA_RUNTIME_READY,NOVA_RUNTIME_RUNNING);}
bool nova_runtime_suspend(void)
{if(!transition(NOVA_RUNTIME_RUNNING,NOVA_RUNTIME_SUSPENDED))return false;
 ++diagnostics.suspends;return true;}
bool nova_runtime_resume(void)
{if(!transition(NOVA_RUNTIME_SUSPENDED,NOVA_RUNTIME_RUNNING))return false;
 ++diagnostics.resumes;return true;}
bool nova_runtime_enter_recovery(void)
{
    if(diagnostics.state!=NOVA_RUNTIME_RUNNING&&diagnostics.state!=NOVA_RUNTIME_INITIALIZING&&
       diagnostics.state!=NOVA_RUNTIME_LOADING&&diagnostics.state!=NOVA_RUNTIME_LAYOUT){
        ++diagnostics.invalid_transitions;return false;}
    if(!runtime_state||!nova_state_transition(runtime_state,NOVA_RUNTIME_RECOVERY)){
        ++diagnostics.invalid_transitions;return false;}
    diagnostics.state=NOVA_RUNTIME_RECOVERY;diagnostics.input_allowed=false;
    ++diagnostics.transitions;++diagnostics.recovery_entries;return true;
}
bool nova_runtime_leave_recovery(bool recovered)
{
    if(diagnostics.state!=NOVA_RUNTIME_RECOVERY){++diagnostics.invalid_transitions;return false;}
    nova_runtime_state_t next=recovered?NOVA_RUNTIME_RUNNING:NOVA_RUNTIME_SHUTDOWN;
    if(!runtime_state||!nova_state_transition(runtime_state,(uint8_t)next)){
        ++diagnostics.invalid_transitions;return false;}
    diagnostics.state=next;
    diagnostics.input_allowed=recovered;++diagnostics.transitions;return true;
}
bool nova_runtime_shutdown(void)
{
    if(diagnostics.state==NOVA_RUNTIME_DESTROYED||diagnostics.state==NOVA_RUNTIME_SHUTDOWN){
        ++diagnostics.invalid_transitions;return false;}
    if(!runtime_state||!nova_state_transition(runtime_state,NOVA_RUNTIME_SHUTDOWN)){
        ++diagnostics.invalid_transitions;return false;}
    diagnostics.state=NOVA_RUNTIME_SHUTDOWN;diagnostics.input_allowed=false;
    diagnostics.frame_active=false;++diagnostics.transitions;++diagnostics.shutdowns;
    for(uint8_t i=0;i<NOVA_RUNTIME_TASK_CAPACITY;++i)tasks[i].active=false;
    return true;
}
bool nova_runtime_destroy(void)
{return transition(NOVA_RUNTIME_SHUTDOWN,NOVA_RUNTIME_DESTROYED);}

bool nova_runtime_frame_begin(void)
{
    if(diagnostics.state!=NOVA_RUNTIME_RUNNING||diagnostics.frame_active){
        ++diagnostics.rejected_frames;return false;}
    diagnostics.frame_active=true;diagnostics.frame_stage=NOVA_FRAME_INPUT;
    nova_state_set_phase(NOVA_STATE_PHASE_INPUT);
    ++diagnostics.frames;return true;
}
bool nova_runtime_frame_step(nova_runtime_frame_stage_t stage)
{
    if(!diagnostics.frame_active||stage!=diagnostics.frame_stage||stage>=NOVA_FRAME_STAGE_COUNT){
        ++diagnostics.rejected_frames;return false;}
    diagnostics.frame_stage=(nova_runtime_frame_stage_t)(stage+1u);
    nova_state_phase_t phase=NOVA_STATE_PHASE_UPDATE;
    if(stage>=NOVA_FRAME_MOTION)phase=NOVA_STATE_PHASE_ANIMATION;
    if(stage>=NOVA_FRAME_LAYOUT)phase=NOVA_STATE_PHASE_LAYOUT;
    if(stage>=NOVA_FRAME_DIRTY_DETECTION)phase=NOVA_STATE_PHASE_RENDER;
    if(stage>=NOVA_FRAME_PRESENT)phase=NOVA_STATE_PHASE_PRESENT;
    nova_state_set_phase(phase);return true;
}
bool nova_runtime_frame_end(void)
{
    if(!diagnostics.frame_active||diagnostics.frame_stage!=NOVA_FRAME_STAGE_COUNT){
        ++diagnostics.rejected_frames;return false;}
    diagnostics.frame_active=false;diagnostics.frame_stage=NOVA_FRAME_INPUT;
    nova_state_set_phase(NOVA_STATE_PHASE_IDLE);
    ++diagnostics.completed_frames;return true;
}
bool nova_runtime_frame_abort(void)
{
    if(!diagnostics.frame_active)return false;
    diagnostics.frame_active=false;diagnostics.frame_stage=NOVA_FRAME_INPUT;
    nova_state_set_phase(NOVA_STATE_PHASE_IDLE);
    ++diagnostics.rejected_frames;return true;
}

bool nova_runtime_schedule(const nova_runtime_task_t *description)
{
    if(!description||!description->id||!description->callback||
       description->priority>NOVA_TASK_IDLE)return false;
    for(uint8_t i=0;i<NOVA_RUNTIME_TASK_CAPACITY;++i)
        if(tasks[i].active&&tasks[i].id==description->id)return false;
    for(uint8_t i=0;i<NOVA_RUNTIME_TASK_CAPACITY;++i)if(!tasks[i].active){
        tasks[i]=*description;tasks[i].active=true;tasks[i].running=false;
        ++diagnostics.scheduled_tasks;return true;}
    ++diagnostics.scheduler_overflows;return false;
}
bool nova_runtime_cancel_task(uint32_t id)
{
    for(uint8_t i=0;i<NOVA_RUNTIME_TASK_CAPACITY;++i)if(tasks[i].active&&tasks[i].id==id){
        if(tasks[i].running)return false;
        tasks[i].active=false;
        ++diagnostics.cancelled_tasks;return true;}
    return false;
}
void nova_runtime_tick(uint32_t elapsed_ms)
{
    if(elapsed_ms>1000u)elapsed_ms=1000u;
    diagnostics.current_time_ms+=elapsed_ms;
    nova_state_advance_time(elapsed_ms);
    for(uint8_t priority=NOVA_TASK_CRITICAL;priority<=NOVA_TASK_IDLE;++priority)
        for(uint8_t i=0;i<NOVA_RUNTIME_TASK_CAPACITY;++i){
            nova_runtime_task_t *task=&tasks[i];
            if(!task->active||task->priority!=priority||
               (int32_t)(diagnostics.current_time_ms-task->due_ms)<0)continue;
            task->running=true;task->callback(task->id,task->context);task->running=false;
            ++diagnostics.executed_tasks;
            if(task->repeating&&task->interval_ms)task->due_ms+=task->interval_ms;
            else task->active=false;
        }
}
nova_runtime_state_t nova_runtime_state(void){return diagnostics.state;}
bool nova_runtime_input_allowed(void){return diagnostics.input_allowed;}
const nova_runtime_diagnostics_t *nova_runtime_diagnostics(void){return &diagnostics;}
