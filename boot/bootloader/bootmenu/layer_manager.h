#ifndef NOVA_BOOT_LAYER_MANAGER_H
#define NOVA_BOOT_LAYER_MANAGER_H

#include <stdbool.h>
#include <stdint.h>
#include "scene_graph.h"
#include "surface_manager.h"

#define NOVA_MANAGED_LAYER_CAPACITY 512u
#define NOVA_MANAGED_LAYER_MAX_DEPTH 32u
typedef uint32_t nova_layer_handle_t;
typedef enum {NOVA_LAYER_ROOT,NOVA_LAYER_BACKGROUND,NOVA_LAYER_CONTENT,NOVA_LAYER_CONTROL,
    NOVA_LAYER_POPUP,NOVA_LAYER_DIALOG,NOVA_LAYER_OVERLAY,NOVA_LAYER_NOTIFICATION,
    NOVA_LAYER_DIAGNOSTIC,NOVA_LAYER_CURSOR,NOVA_LAYER_EMERGENCY,NOVA_LAYER_CUSTOM}
nova_managed_layer_type_t;
typedef enum {NOVA_LAYER_SURFACE_NONE,NOVA_LAYER_SURFACE_AUTO,
    NOVA_LAYER_SURFACE_REQUIRED,NOVA_LAYER_SURFACE_CACHED,NOVA_LAYER_SURFACE_TRANSIENT}
nova_layer_surface_policy_t;
typedef enum {NOVA_LAYER_PHASE_INITIALIZATION,NOVA_LAYER_PHASE_STATE_UPDATE,
    NOVA_LAYER_PHASE_SCENE_UPDATE,NOVA_LAYER_PHASE_RESOURCE_COMPLETION,
    NOVA_LAYER_PHASE_RECOVERY,NOVA_LAYER_PHASE_TRAVERSAL,NOVA_LAYER_PHASE_COMPOSITING,
    NOVA_LAYER_PHASE_PRESENT} nova_layer_phase_t;
enum {NOVA_LAYER_VISIBLE=1u,NOVA_LAYER_ENABLED=2u,NOVA_LAYER_ISOLATED=4u,
    NOVA_LAYER_CACHEABLE=8u,NOVA_LAYER_CACHED=16u,NOVA_LAYER_MODAL=32u,
    NOVA_LAYER_INPUT_BLOCKING=64u,NOVA_LAYER_CLIPPED=128u,
    NOVA_LAYER_TRANSFORMED=256u,NOVA_LAYER_HAS_OPACITY=512u,
    NOVA_LAYER_HAS_BLUR=1024u,NOVA_LAYER_HAS_SHADOW=2048u,
    NOVA_LAYER_HAS_MASK=4096u,NOVA_LAYER_DIRTY=8192u};
typedef struct {nova_managed_layer_type_t type;nova_layer_handle_t parent;
    int32_t z_index;nova_rect_t bounds,clip;uint16_t opacity;
    nova_scene_matrix_t transform;uint32_t flags,effects;
    nova_layer_surface_policy_t surface_policy;} nova_layer_descriptor_t;
typedef struct {nova_layer_handle_t handle,parent,first_child,next_sibling;
    nova_managed_layer_type_t type;int32_t z_index;nova_rect_t bounds,clip,effective_clip,damage;
    uint16_t opacity,effective_opacity,depth;uint32_t flags,effects;
    nova_scene_matrix_t transform,world;uint64_t creation,generation;
    nova_surface_handle_t surface;nova_layer_surface_policy_t surface_policy;bool active;}
nova_managed_layer_t;
typedef struct {uint32_t creations,destroys,attachments,detachments,traversals,
    mutations,rejected_mutations,rejected_cycles,invalid_handles,depth_errors,
    damage_propagations,isolations;uint16_t active,peak;}
nova_layer_manager_diagnostics_t;

void nova_layer_manager_initialize(void);
void nova_layer_manager_set_phase(nova_layer_phase_t phase);
nova_layer_handle_t nova_layer_manager_root(void);
bool nova_managed_layer_create(const nova_layer_descriptor_t *descriptor,
    nova_layer_handle_t *out_handle);
bool nova_managed_layer_destroy(nova_layer_handle_t handle);
bool nova_managed_layer_attach(nova_layer_handle_t parent,nova_layer_handle_t child);
bool nova_managed_layer_detach(nova_layer_handle_t handle);
bool nova_managed_layer_set_visible(nova_layer_handle_t handle,bool visible);
bool nova_managed_layer_set_bounds(nova_layer_handle_t handle,nova_rect_t bounds);
bool nova_managed_layer_set_opacity(nova_layer_handle_t handle,uint16_t opacity);
bool nova_managed_layer_set_transform(nova_layer_handle_t handle,nova_scene_matrix_t transform);
bool nova_managed_layer_bind_surface(nova_layer_handle_t handle,nova_surface_handle_t surface);
bool nova_managed_layer_mark_dirty(nova_layer_handle_t handle,nova_rect_t damage);
uint16_t nova_layer_manager_build_order(nova_layer_handle_t *output,uint16_t capacity);
const nova_managed_layer_t *nova_managed_layer_get(nova_layer_handle_t handle);
const nova_layer_manager_diagnostics_t *nova_layer_manager_diagnostics(void);

#endif
