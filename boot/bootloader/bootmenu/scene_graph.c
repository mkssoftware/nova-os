#include "scene_graph.h"

#define ONE 65536
static nova_scene_node_t nodes[NOVA_SCENE_CAPACITY];
static nova_scene_diagnostics_t diagnostics;

nova_scene_matrix_t nova_scene_identity(void)
{return (nova_scene_matrix_t){ONE,0,0,ONE,0,0};}
static bool valid(const nova_scene_node_t *node)
{return node&&node>=nodes&&node<nodes+NOVA_SCENE_CAPACITY&&node->active&&
    &nodes[node->id]==node;}
void nova_scene_initialize(void)
{
    diagnostics=(nova_scene_diagnostics_t){0};
    for(uint16_t i=0;i<NOVA_SCENE_CAPACITY;++i)nodes[i]=(nova_scene_node_t){.id=i,
        .parent=NOVA_SCENE_NONE,.first_child=NOVA_SCENE_NONE,.next_sibling=NOVA_SCENE_NONE};
    nodes[NOVA_SCENE_ROOT]=(nova_scene_node_t){.id=NOVA_SCENE_ROOT,.parent=NOVA_SCENE_NONE,
        .first_child=NOVA_SCENE_NONE,.next_sibling=NOVA_SCENE_NONE,.type=NOVA_SCENE_CONTAINER,
        .visibility=NOVA_SCENE_VISIBLE,.local={ONE,0,0,ONE,0,0},
        .world={ONE,0,0,ONE,0,0},.opacity=1000,.world_opacity=1000,
        .enabled=true,.active=true,.dirty=NOVA_SCENE_DIRTY_ALL};
}
nova_scene_node_t *nova_scene_root(void){return &nodes[NOVA_SCENE_ROOT];}
nova_scene_node_t *nova_scene_find(uint16_t id)
{return id<NOVA_SCENE_CAPACITY&&nodes[id].active?&nodes[id]:0;}
nova_scene_node_t *nova_scene_create(nova_scene_node_type_t type)
{
    if(type>NOVA_SCENE_CURSOR)return 0;
    for(uint16_t i=1;i<NOVA_SCENE_CAPACITY;++i)if(!nodes[i].active){
        nodes[i]=(nova_scene_node_t){.id=i,.parent=NOVA_SCENE_NONE,
            .first_child=NOVA_SCENE_NONE,.next_sibling=NOVA_SCENE_NONE,.type=type,
            .visibility=NOVA_SCENE_VISIBLE,.local={ONE,0,0,ONE,0,0},
            .world={ONE,0,0,ONE,0,0},.opacity=1000,.world_opacity=1000,
            .enabled=true,.active=true,.dirty=NOVA_SCENE_DIRTY_ALL};
        ++diagnostics.created;return &nodes[i];}
    ++diagnostics.rejected_handles;return 0;
}
static bool descendant(const nova_scene_node_t *node,const nova_scene_node_t *candidate)
{
    for(const nova_scene_node_t *at=candidate;valid(at)&&at->parent!=NOVA_SCENE_NONE;
        at=nova_scene_find(at->parent))if(at==node)return true;
    return false;
}
bool nova_scene_detach(nova_scene_node_t *node)
{
    if(!valid(node)||node->id==NOVA_SCENE_ROOT)return false;
    if(node->parent==NOVA_SCENE_NONE)return true;
    nova_scene_node_t *parent=nova_scene_find(node->parent);
    if(!parent)return false;
    uint16_t *link=&parent->first_child;
    while(*link!=NOVA_SCENE_NONE&&*link!=node->id)link=&nodes[*link].next_sibling;
    if(*link!=node->id)return false;
    *link=node->next_sibling;node->parent=NOVA_SCENE_NONE;node->next_sibling=NOVA_SCENE_NONE;
    parent->dirty|=NOVA_SCENE_DIRTY_LAYOUT|NOVA_SCENE_DIRTY_RENDER;
    ++diagnostics.detachments;return true;
}
bool nova_scene_attach(nova_scene_node_t *parent,nova_scene_node_t *child)
{
    if(!valid(parent)||!valid(child)||child->id==NOVA_SCENE_ROOT||parent==child||
       descendant(child,parent)){++diagnostics.rejected_cycles;return false;}
    if(child->parent!=NOVA_SCENE_NONE&&!nova_scene_detach(child))return false;
    uint16_t *link=&parent->first_child;
    while(*link!=NOVA_SCENE_NONE)link=&nodes[*link].next_sibling;
    *link=child->id;child->parent=parent->id;child->dirty|=NOVA_SCENE_DIRTY_ALL;
    ++diagnostics.attachments;return true;
}
bool nova_scene_destroy(nova_scene_node_t *node)
{
    if(!valid(node)||node->id==NOVA_SCENE_ROOT||node->first_child!=NOVA_SCENE_NONE)return false;
    if(!nova_scene_detach(node))return false;
    uint16_t id=node->id;*node=(nova_scene_node_t){.id=id,.parent=NOVA_SCENE_NONE,
        .first_child=NOVA_SCENE_NONE,.next_sibling=NOVA_SCENE_NONE};
    ++diagnostics.destroyed;return true;
}
bool nova_scene_mark_dirty(nova_scene_node_t *node,uint32_t flags)
{
    if(!valid(node)||!flags||(flags&~NOVA_SCENE_DIRTY_ALL))return false;
    node->dirty|=flags;return true;
}
bool nova_scene_set_bounds(nova_scene_node_t *node,nova_rect_t bounds)
{
    if(!valid(node)||bounds.width<0||bounds.height<0)return false;
    if(node->bounds.x==bounds.x&&node->bounds.y==bounds.y&&node->bounds.width==bounds.width&&
       node->bounds.height==bounds.height)return true;
    node->bounds=bounds;return nova_scene_mark_dirty(node,NOVA_SCENE_DIRTY_LAYOUT|NOVA_SCENE_DIRTY_RENDER);
}
bool nova_scene_set_transform(nova_scene_node_t *node,nova_scene_matrix_t transform)
{
    if(!valid(node)||!transform.m11||!transform.m22)return false;
    node->local=transform;return nova_scene_mark_dirty(node,NOVA_SCENE_DIRTY_TRANSFORM|NOVA_SCENE_DIRTY_RENDER);
}
bool nova_scene_set_opacity(nova_scene_node_t *node,uint16_t opacity)
{
    if(!valid(node)||opacity>1000)return false;
    if(node->opacity==opacity)return true;
    node->opacity=opacity;return nova_scene_mark_dirty(node,NOVA_SCENE_DIRTY_STATE|NOVA_SCENE_DIRTY_RENDER);
}
bool nova_scene_set_visibility(nova_scene_node_t *node,nova_scene_visibility_t visibility)
{
    if(!valid(node)||visibility>NOVA_SCENE_COLLAPSED)return false;
    if(node->visibility==visibility)return true;
    node->visibility=visibility;return nova_scene_mark_dirty(node,NOVA_SCENE_DIRTY_STATE|
        NOVA_SCENE_DIRTY_LAYOUT|NOVA_SCENE_DIRTY_RENDER);
}
bool nova_scene_set_enabled(nova_scene_node_t *node,bool enabled)
{if(!valid(node))return false;if(node->enabled==enabled)return true;node->enabled=enabled;
 return nova_scene_mark_dirty(node,NOVA_SCENE_DIRTY_STATE);}
static nova_scene_matrix_t multiply(nova_scene_matrix_t a,nova_scene_matrix_t b)
{
    return (nova_scene_matrix_t){
        (int32_t)(((int64_t)a.m11*b.m11+(int64_t)a.m12*b.m21)/ONE),
        (int32_t)(((int64_t)a.m11*b.m12+(int64_t)a.m12*b.m22)/ONE),
        (int32_t)(((int64_t)a.m21*b.m11+(int64_t)a.m22*b.m21)/ONE),
        (int32_t)(((int64_t)a.m21*b.m12+(int64_t)a.m22*b.m22)/ONE),
        a.tx+(int32_t)(((int64_t)a.m11*b.tx+(int64_t)a.m12*b.ty)/ONE),
        a.ty+(int32_t)(((int64_t)a.m21*b.tx+(int64_t)a.m22*b.ty)/ONE)};
}
static bool walk(nova_scene_node_t *node,nova_scene_matrix_t parent_matrix,
    uint16_t parent_opacity,bool parent_visible,uint32_t depth,nova_scene_visit_t visit,
    void *context,bool dirty_only)
{
    if(depth>diagnostics.max_depth)diagnostics.max_depth=depth;
    node->world=multiply(parent_matrix,node->local);
    node->world.tx+=node->bounds.x*ONE;node->world.ty+=node->bounds.y*ONE;
    node->world_opacity=(uint16_t)((uint32_t)parent_opacity*node->opacity/1000u);
    bool visible=parent_visible&&node->visibility==NOVA_SCENE_VISIBLE;
    node->world_bounds=(nova_rect_t){node->world.tx/ONE,node->world.ty/ONE,
        node->visibility==NOVA_SCENE_COLLAPSED?0:node->bounds.width,
        node->visibility==NOVA_SCENE_COLLAPSED?0:node->bounds.height};
    uint32_t was_dirty=node->dirty;
    if(was_dirty)++diagnostics.dirty_nodes;
    if(visible&&(!dirty_only||was_dirty)){++diagnostics.visited;if(visit&&!visit(node,context))return false;}
    node->dirty=0;
    for(uint16_t id=node->first_child;id!=NOVA_SCENE_NONE;id=nodes[id].next_sibling)
        if(!walk(&nodes[id],node->world,node->world_opacity,visible,depth+1,visit,context,dirty_only))return false;
    return true;
}
bool nova_scene_traverse(nova_scene_visit_t visit,void *context,bool dirty_only)
{
    ++diagnostics.traversals;
    return walk(nova_scene_root(),nova_scene_identity(),1000,true,0,visit,context,dirty_only);
}
const nova_scene_diagnostics_t *nova_scene_diagnostics(void){return &diagnostics;}
