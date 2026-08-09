#ifndef NOVA_BOOT_RUNTIME_H
#define NOVA_BOOT_RUNTIME_H

#include <stdbool.h>
#include <stdint.h>

#define NOVA_RUNTIME_TASK_CAPACITY 32u

typedef enum {
    NOVA_RUNTIME_CREATED,
    NOVA_RUNTIME_INITIALIZING,
    NOVA_RUNTIME_LOADING,
    NOVA_RUNTIME_BUILDING_SCENE,
    NOVA_RUNTIME_LAYOUT,
    NOVA_RUNTIME_READY,
    NOVA_RUNTIME_RUNNING,
    NOVA_RUNTIME_SUSPENDED,
    NOVA_RUNTIME_RECOVERY,
    NOVA_RUNTIME_SHUTDOWN,
    NOVA_RUNTIME_DESTROYED
} nova_runtime_state_t;

typedef enum {
    NOVA_RUNTIME_MEMORY,
    NOVA_RUNTIME_PLATFORM,
    NOVA_RUNTIME_GRAPHICS,
    NOVA_RUNTIME_DIAGNOSTICS,
    NOVA_RUNTIME_CONFIGURATION,
    NOVA_RUNTIME_RESOURCES,
    NOVA_RUNTIME_SCENE,
    NOVA_RUNTIME_LAYOUT_ENGINE,
    NOVA_RUNTIME_MOTION,
    NOVA_RUNTIME_INPUT,
    NOVA_RUNTIME_RENDERER,
    NOVA_RUNTIME_SUBSYSTEM_COUNT
} nova_runtime_subsystem_t;

typedef enum {
    NOVA_FRAME_INPUT,
    NOVA_FRAME_EVENT_QUEUE,
    NOVA_FRAME_CONTROL_UPDATE,
    NOVA_FRAME_APPLICATION,
    NOVA_FRAME_MOTION,
    NOVA_FRAME_LAYOUT,
    NOVA_FRAME_DIRTY_DETECTION,
    NOVA_FRAME_RENDER_QUEUE,
    NOVA_FRAME_RENDERING,
    NOVA_FRAME_COMPOSITOR,
    NOVA_FRAME_PRESENT,
    NOVA_FRAME_DIAGNOSTICS,
    NOVA_FRAME_STAGE_COUNT
} nova_runtime_frame_stage_t;

typedef enum {
    NOVA_TASK_CRITICAL,
    NOVA_TASK_INPUT,
    NOVA_TASK_RENDERING,
    NOVA_TASK_ANIMATION,
    NOVA_TASK_LAYOUT,
    NOVA_TASK_BACKGROUND,
    NOVA_TASK_IDLE
} nova_runtime_task_priority_t;

typedef void (*nova_runtime_task_callback_t)(uint32_t id,void *context);
typedef struct {
    uint32_t id,due_ms,interval_ms;
    nova_runtime_task_callback_t callback;
    void *context;
    nova_runtime_task_priority_t priority;
    bool active,repeating,running;
} nova_runtime_task_t;

typedef struct {
    nova_runtime_state_t state;
    nova_runtime_frame_stage_t frame_stage;
    uint32_t ready_subsystems;
    uint32_t transitions;
    uint32_t invalid_transitions;
    uint32_t initialization_failures;
    uint32_t frames;
    uint32_t completed_frames;
    uint32_t rejected_frames;
    uint32_t scheduled_tasks;
    uint32_t executed_tasks;
    uint32_t cancelled_tasks;
    uint32_t scheduler_overflows;
    uint32_t recovery_entries;
    uint32_t suspends;
    uint32_t resumes;
    uint32_t shutdowns;
    uint32_t current_time_ms;
    bool frame_active;
    bool input_allowed;
} nova_runtime_diagnostics_t;

void nova_runtime_create(void);
bool nova_runtime_begin_initialization(void);
bool nova_runtime_subsystem_ready(nova_runtime_subsystem_t subsystem);
bool nova_runtime_loading(void);
bool nova_runtime_building_scene(void);
bool nova_runtime_layout(void);
bool nova_runtime_ready(void);
bool nova_runtime_run(void);
bool nova_runtime_suspend(void);
bool nova_runtime_resume(void);
bool nova_runtime_enter_recovery(void);
bool nova_runtime_leave_recovery(bool recovered);
bool nova_runtime_shutdown(void);
bool nova_runtime_destroy(void);
bool nova_runtime_frame_begin(void);
bool nova_runtime_frame_step(nova_runtime_frame_stage_t stage);
bool nova_runtime_frame_end(void);
bool nova_runtime_frame_abort(void);
bool nova_runtime_schedule(const nova_runtime_task_t *task);
bool nova_runtime_cancel_task(uint32_t id);
void nova_runtime_tick(uint32_t elapsed_ms);
nova_runtime_state_t nova_runtime_state(void);
bool nova_runtime_input_allowed(void);
const nova_runtime_diagnostics_t *nova_runtime_diagnostics(void);

#endif
