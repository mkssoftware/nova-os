#ifndef NOVA_BOOT_SCENE_GRAPH_H
#define NOVA_BOOT_SCENE_GRAPH_H

#include <stdbool.h>
#include <stdint.h>
#include "compositor.h"
#include "transform2d.h"

#define NOVA_SCENE_CAPACITY 256u
#define NOVA_SCENE_NONE 0xffffu
#define NOVA_SCENE_ROOT 0u

typedef enum {NOVA_SCENE_CONTAINER,NOVA_SCENE_CONTROL,NOVA_SCENE_DIALOG,
    NOVA_SCENE_GRAPHIC,NOVA_SCENE_TEXT,NOVA_SCENE_OVERLAY,NOVA_SCENE_CURSOR}
nova_scene_node_type_t;
typedef enum {NOVA_SCENE_VISIBLE,NOVA_SCENE_HIDDEN,NOVA_SCENE_COLLAPSED}
nova_scene_visibility_t;
enum {NOVA_SCENE_DIRTY_LAYOUT=1u,NOVA_SCENE_DIRTY_RENDER=2u,
    NOVA_SCENE_DIRTY_STATE=4u,NOVA_SCENE_DIRTY_ANIMATION=8u,
    NOVA_SCENE_DIRTY_TRANSFORM=16u,NOVA_SCENE_DIRTY_ALL=31u};

typedef nova_fixed_transform2d_t nova_scene_matrix_t;
typedef struct {
    uint16_t id,parent,first_child,next_sibling;
    nova_scene_node_type_t type;
    nova_scene_visibility_t visibility;
    nova_rect_t bounds,world_bounds;
    nova_scene_matrix_t local,world;
    uint64_t resource;
    uint32_t dirty;
    uint16_t opacity,world_opacity;
    int16_t layer,z_index;
    bool enabled,active;
} nova_scene_node_t;
typedef bool (*nova_scene_visit_t)(nova_scene_node_t *node,void *context);
typedef struct {uint32_t created,destroyed,attachments,detachments,traversals,
    visited,dirty_nodes,rejected_cycles,rejected_handles,max_depth;}
nova_scene_diagnostics_t;

void nova_scene_initialize(void);
nova_scene_node_t *nova_scene_create(nova_scene_node_type_t type);
bool nova_scene_destroy(nova_scene_node_t *node);
bool nova_scene_attach(nova_scene_node_t *parent,nova_scene_node_t *child);
bool nova_scene_detach(nova_scene_node_t *node);
nova_scene_node_t *nova_scene_find(uint16_t id);
nova_scene_node_t *nova_scene_root(void);
bool nova_scene_set_bounds(nova_scene_node_t *node,nova_rect_t bounds);
bool nova_scene_set_transform(nova_scene_node_t *node,nova_scene_matrix_t transform);
bool nova_scene_set_opacity(nova_scene_node_t *node,uint16_t opacity_milli);
bool nova_scene_set_visibility(nova_scene_node_t *node,nova_scene_visibility_t visibility);
bool nova_scene_set_enabled(nova_scene_node_t *node,bool enabled);
bool nova_scene_mark_dirty(nova_scene_node_t *node,uint32_t flags);
bool nova_scene_traverse(nova_scene_visit_t visit,void *context,bool dirty_only);
nova_scene_matrix_t nova_scene_identity(void);
const nova_scene_diagnostics_t *nova_scene_diagnostics(void);

#endif
