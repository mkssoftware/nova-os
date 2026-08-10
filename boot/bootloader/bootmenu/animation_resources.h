#ifndef NOVA_BOOT_ANIMATION_RESOURCES_H
#define NOVA_BOOT_ANIMATION_RESOURCES_H

#include <stdbool.h>
#include <stdint.h>
#include "motion.h"

#define NOVA_ANIMATION_RESOURCE_CAPACITY 32u
#define NOVA_ANIMATION_KEYFRAME_CAPACITY 32u
#define NOVA_ANIMATION_THEME_SLOT_COUNT 8u

typedef enum {NOVA_ANIMATION_REPEAT_ONCE,NOVA_ANIMATION_REPEAT_COUNT,
    NOVA_ANIMATION_REPEAT_INFINITE} nova_animation_repeat_t;
typedef enum {NOVA_ANIMATION_TRIGGER_NONE,NOVA_ANIMATION_TRIGGER_FOCUS,
    NOVA_ANIMATION_TRIGGER_CLICK,NOVA_ANIMATION_TRIGGER_PAGE_CHANGE,
    NOVA_ANIMATION_TRIGGER_DIALOG_OPEN,NOVA_ANIMATION_TRIGGER_DIALOG_CLOSE,
    NOVA_ANIMATION_TRIGGER_SYSTEM} nova_animation_trigger_t;
typedef enum {NOVA_ANIMATION_CATEGORY_GENERAL,NOVA_ANIMATION_CATEGORY_NAVIGATION,
    NOVA_ANIMATION_CATEGORY_DIALOG,NOVA_ANIMATION_CATEGORY_INTERACTION,
    NOVA_ANIMATION_CATEGORY_ACTIVITY,NOVA_ANIMATION_CATEGORY_STATUS,
    NOVA_ANIMATION_CATEGORY_THEME} nova_animation_category_t;
typedef struct {uint32_t time_ms;nova_property_t property;int32_t value;
    nova_easing_t easing;} nova_animation_keyframe_t;
typedef struct {
    uint64_t animation_id,resource_id;
    const char *name;
    uint32_t version,resource_version,duration_ms;
    nova_animation_repeat_t repeat;
    uint16_t repeat_count;
    nova_animation_trigger_t trigger;
    nova_animation_category_t category;
    uint8_t priority;
    const nova_animation_keyframe_t *keyframes;
    uint16_t keyframe_count;
} nova_animation_resource_descriptor_t;
typedef struct {
    uint64_t animation_id,resource_id;
    char name[48];
    uint32_t version,resource_version,duration_ms,references;
    nova_animation_repeat_t repeat;
    uint16_t repeat_count,keyframe_count;
    nova_animation_trigger_t trigger;
    nova_animation_category_t category;
    uint8_t priority;
    nova_animation_keyframe_t keyframes[NOVA_ANIMATION_KEYFRAME_CAPACITY];
    bool loaded,valid;
} nova_animation_resource_t;
typedef struct {int32_t values[NOVA_PROPERTY_COUNT];uint32_t property_mask;
    uint32_t local_time_ms,cycle;bool complete;} nova_animation_sample_t;
typedef struct {
    uint32_t initializations,registered,loads,cache_hits,releases,samples;
    uint32_t keyframes_evaluated,trigger_resolutions,theme_changes,duplicates;
    uint32_t invalid_resources,reduced_samples;
    uint64_t theme_slots[NOVA_ANIMATION_THEME_SLOT_COUNT];
    uint16_t speed_per_mille;
    bool initialized,reduced_motion;
} nova_animation_resource_diagnostics_t;

bool nova_animation_resource_initialize(void);
bool nova_animation_resource_register(const nova_animation_resource_descriptor_t *descriptor);
const nova_animation_resource_t *nova_animation_resource_load(uint64_t animation_id);
const nova_animation_resource_t *nova_animation_resource_find(uint64_t animation_id);
bool nova_animation_resource_release(uint64_t animation_id);
bool nova_animation_resource_validate(const nova_animation_resource_t *animation);
bool nova_animation_resource_sample(uint64_t animation_id,uint64_t elapsed_ms,
                                    nova_animation_sample_t *sample);
const nova_animation_resource_t *nova_animation_resource_for_trigger(
    nova_animation_trigger_t trigger,nova_animation_category_t category);
bool nova_animation_theme_bind(uint8_t slot,uint64_t animation_id);
uint64_t nova_animation_theme_get(uint8_t slot);
bool nova_animation_theme_speed(uint16_t speed_per_mille);
void nova_animation_resource_set_reduced(bool enabled);
const nova_animation_resource_diagnostics_t *nova_animation_resource_diagnostics(void);

#endif
