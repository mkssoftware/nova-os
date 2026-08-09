#ifndef NOVA_BOOT_STATE_MODEL_H
#define NOVA_BOOT_STATE_MODEL_H

#include <stdbool.h>
#include <stdint.h>

#define NOVA_STATE_CAPACITY 256u
#define NOVA_STATE_LOG_CAPACITY 256u
#define NOVA_STATE_VALUE_CAPACITY 16u
#define NOVA_STATE_NONE 0xffffu
#define NOVA_STATE_SNAPSHOT_MAGIC 0x4e535431u

typedef enum {
    NOVA_STATE_DOMAIN_RUNTIME,
    NOVA_STATE_DOMAIN_SCENE,
    NOVA_STATE_DOMAIN_DIALOG,
    NOVA_STATE_DOMAIN_OVERLAY,
    NOVA_STATE_DOMAIN_CONTROL,
    NOVA_STATE_DOMAIN_FOCUS,
    NOVA_STATE_DOMAIN_POINTER,
    NOVA_STATE_DOMAIN_KEYBOARD,
    NOVA_STATE_DOMAIN_TOUCH,
    NOVA_STATE_DOMAIN_NAVIGATION,
    NOVA_STATE_DOMAIN_PAGE,
    NOVA_STATE_DOMAIN_ANIMATION,
    NOVA_STATE_DOMAIN_RENDER,
    NOVA_STATE_DOMAIN_RESOURCE,
    NOVA_STATE_DOMAIN_THEME,
    NOVA_STATE_DOMAIN_ERROR,
    NOVA_STATE_DOMAIN_COUNT
} nova_state_domain_t;

typedef enum {
    NOVA_STATE_PHASE_IDLE,
    NOVA_STATE_PHASE_INPUT,
    NOVA_STATE_PHASE_UPDATE,
    NOVA_STATE_PHASE_ANIMATION,
    NOVA_STATE_PHASE_LAYOUT,
    NOVA_STATE_PHASE_RENDER,
    NOVA_STATE_PHASE_PRESENT
} nova_state_phase_t;

typedef enum {
    NOVA_STATE_EVENT_EXIT,
    NOVA_STATE_EVENT_TRANSITION,
    NOVA_STATE_EVENT_ENTER,
    NOVA_STATE_EVENT_ROLLBACK,
    NOVA_STATE_EVENT_RESET
} nova_state_event_type_t;

struct nova_state_object;
typedef void (*nova_state_callback_t)(const struct nova_state_object *object,
                                      uint8_t old_state,uint8_t new_state,
                                      void *context);

typedef struct nova_state_object {
    uint16_t id,owner,parent;
    nova_state_domain_t domain;
    uint8_t current,previous,initial,checkpoint;
    uint32_t timestamp_ms,transitions,errors;
    nova_state_callback_t on_exit,on_transition,on_enter;
    void *callback_context;
    bool active,has_checkpoint,in_transition;
} nova_state_object_t;

typedef struct {
    uint32_t sequence,timestamp_ms;
    uint16_t object_id;
    nova_state_domain_t domain;
    nova_state_event_type_t type;
    uint8_t old_state,new_state;
} nova_state_event_t;

typedef struct {
    uint32_t magic;
    uint16_t version,size,id,owner,parent,domain;
    uint8_t current,previous,initial,checkpoint;
    uint32_t timestamp_ms,transitions,errors,checksum;
} nova_state_snapshot_t;

typedef struct {
    uint16_t active,peak_active,log_count;
    uint32_t created,destroyed,transitions,invalid_transitions,rollbacks,resets;
    uint32_t serialized,deserialized,checksum_errors,hierarchy_errors;
    uint32_t callback_events,log_overwrites,current_time_ms;
    nova_state_phase_t phase;
} nova_state_diagnostics_t;

void nova_state_model_initialize(void);
nova_state_object_t *nova_state_create(uint16_t id,nova_state_domain_t domain,
                                      uint8_t initial,uint16_t owner,uint16_t parent);
bool nova_state_set_callbacks(nova_state_object_t *object,
                              nova_state_callback_t on_exit,
                              nova_state_callback_t on_transition,
                              nova_state_callback_t on_enter,void *context);
bool nova_state_validate(const nova_state_object_t *object,uint8_t next);
bool nova_state_transition(nova_state_object_t *object,uint8_t next);
bool nova_state_checkpoint(nova_state_object_t *object);
bool nova_state_rollback(nova_state_object_t *object);
bool nova_state_reset(nova_state_object_t *object);
bool nova_state_destroy(nova_state_object_t *object);
bool nova_state_serialize(const nova_state_object_t *object,nova_state_snapshot_t *snapshot);
bool nova_state_deserialize(nova_state_object_t *object,const nova_state_snapshot_t *snapshot);
const nova_state_object_t *nova_state_find(uint16_t id);
const nova_state_event_t *nova_state_event(uint16_t newest_index);
void nova_state_set_phase(nova_state_phase_t phase);
void nova_state_advance_time(uint32_t elapsed_ms);
const nova_state_diagnostics_t *nova_state_diagnostics(void);

#endif
