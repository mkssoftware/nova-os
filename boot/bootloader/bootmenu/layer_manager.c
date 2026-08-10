#include "layer_manager.h"

static nova_managed_layer_t layers[NOVA_MANAGED_LAYER_CAPACITY];
static nova_layer_manager_diagnostics_t diagnostics;
static nova_layer_phase_t phase;
static uint64_t creation_counter;
static nova_layer_handle_t make_handle(uint16_t index,uint16_t generation)
{return ((uint32_t)generation<<16)|index;}
static nova_managed_layer_t *lookup(nova_layer_handle_t handle)
{uint16_t index=(uint16_t)handle,generation=(uint16_t)(handle>>16);
 if(index>=NOVA_MANAGED_LAYER_CAPACITY||!generation||!layers[index].active||
    layers[index].generation!=generation){++diagnostics.invalid_handles;return 0;}return &layers[index];}
static bool mutable(void)
{return phase<=NOVA_LAYER_PHASE_RECOVERY;}
static uint16_t class_of(nova_managed_layer_type_t type)
{static const uint16_t classes[]={0,0,100,200,300,400,500,600,700,800,900,100};
 return type<=NOVA_LAYER_CUSTOM?classes[type]:0xffffu;}
void nova_layer_manager_initialize(void)
{diagnostics=(nova_layer_manager_diagnostics_t){0};phase=NOVA_LAYER_PHASE_INITIALIZATION;creation_counter=1;
 for(uint16_t i=0;i<NOVA_MANAGED_LAYER_CAPACITY;++i){uint64_t generation=layers[i].generation+1u;
  if(!generation||generation>0xffffu)generation=1;
  layers[i]=(nova_managed_layer_t){.generation=generation};}
 layers[0]=(nova_managed_layer_t){.handle=make_handle(0,(uint16_t)layers[0].generation),
  .type=NOVA_LAYER_ROOT,.parent=0,.first_child=0,.next_sibling=0,.opacity=1000,
  .effective_opacity=1000,.transform={65536,0,0,65536,0,0},.world={65536,0,0,65536,0,0},
  .flags=NOVA_LAYER_VISIBLE|NOVA_LAYER_ENABLED,.creation=creation_counter++,
  .generation=layers[0].generation,.active=true};diagnostics.active=diagnostics.peak=1;}
void nova_layer_manager_set_phase(nova_layer_phase_t value){if(value<=NOVA_LAYER_PHASE_PRESENT)phase=value;}
nova_layer_handle_t nova_layer_manager_root(void){return layers[0].handle;}
static uint16_t free_slot(void)
{for(uint16_t i=1;i<NOVA_MANAGED_LAYER_CAPACITY;++i)if(!layers[i].active)return i;return NOVA_MANAGED_LAYER_CAPACITY;}
static bool would_isolate(const nova_layer_descriptor_t *d)
{return d->surface_policy==NOVA_LAYER_SURFACE_REQUIRED||d->surface_policy==NOVA_LAYER_SURFACE_CACHED||
 d->surface_policy==NOVA_LAYER_SURFACE_TRANSIENT||(d->surface_policy==NOVA_LAYER_SURFACE_AUTO&&
 (d->opacity<1000||(d->flags&(NOVA_LAYER_HAS_BLUR|NOVA_LAYER_HAS_MASK|NOVA_LAYER_TRANSFORMED|
 NOVA_LAYER_CACHED))));}
bool nova_managed_layer_create(const nova_layer_descriptor_t *d,nova_layer_handle_t *out)
{if(!mutable()){++diagnostics.rejected_mutations;return false;}if(!d||!out||d->type>NOVA_LAYER_CUSTOM||
 d->opacity>1000||d->surface_policy>NOVA_LAYER_SURFACE_TRANSIENT)return false;
 nova_managed_layer_t *parent=lookup(d->parent?d->parent:nova_layer_manager_root());if(!parent)return false;
 if((uint32_t)parent->depth+1u>NOVA_MANAGED_LAYER_MAX_DEPTH){++diagnostics.depth_errors;return false;}
 uint16_t index=free_slot();if(index==NOVA_MANAGED_LAYER_CAPACITY)return false;uint64_t generation=layers[index].generation+1u;
 if(!generation||generation>0xffffu)generation=1;
 layers[index]=(nova_managed_layer_t){
  .handle=make_handle(index,(uint16_t)generation),.parent=0,.first_child=0,.next_sibling=0,
  .type=d->type,.z_index=d->z_index,.bounds=d->bounds,.clip=d->clip,.opacity=d->opacity,
  .effective_opacity=d->opacity,.depth=(uint16_t)(parent->depth+1),.flags=d->flags|NOVA_LAYER_DIRTY,
  .effects=d->effects,.transform=d->transform.m11?d->transform:nova_scene_identity(),
  .world=nova_scene_identity(),.creation=creation_counter++,.generation=generation,
  .surface_policy=d->surface_policy,.active=true};
 if(would_isolate(d)){layers[index].flags|=NOVA_LAYER_ISOLATED;++diagnostics.isolations;}
 ++diagnostics.creations;++diagnostics.active;if(diagnostics.active>diagnostics.peak)diagnostics.peak=diagnostics.active;
 *out=layers[index].handle;if(!nova_managed_layer_attach(parent->handle,*out)){layers[index].active=false;return false;}return true;}
static bool descends(nova_managed_layer_t *candidate,nova_managed_layer_t *ancestor)
{for(nova_managed_layer_t *at=candidate;at&&at->parent;at=lookup(at->parent))if(at==ancestor)return true;return false;}
bool nova_managed_layer_detach(nova_layer_handle_t handle)
{if(!mutable()){++diagnostics.rejected_mutations;return false;}nova_managed_layer_t *child=lookup(handle);
 if(!child||!child->parent)return false;
 nova_managed_layer_t *parent=lookup(child->parent);if(!parent)return false;
 nova_layer_handle_t *link=&parent->first_child;while(*link&&*link!=handle){nova_managed_layer_t *n=lookup(*link);if(!n)return false;link=&n->next_sibling;}
 if(*link!=handle)return false;
 *link=child->next_sibling;child->parent=0;child->next_sibling=0;
 parent->flags|=NOVA_LAYER_DIRTY;++diagnostics.detachments;++diagnostics.mutations;return true;}
bool nova_managed_layer_attach(nova_layer_handle_t parent_handle,nova_layer_handle_t child_handle)
{if(!mutable()){++diagnostics.rejected_mutations;return false;}nova_managed_layer_t *parent=lookup(parent_handle),*child=lookup(child_handle);
 if(!parent||!child||parent==child||descends(parent,child)){++diagnostics.rejected_cycles;return false;}
 if((uint32_t)parent->depth+1u>NOVA_MANAGED_LAYER_MAX_DEPTH){++diagnostics.depth_errors;return false;}
 if(child->parent&&!nova_managed_layer_detach(child_handle))return false;
 nova_layer_handle_t *link=&parent->first_child;
 while(*link){nova_managed_layer_t *n=lookup(*link);if(!n)return false;link=&n->next_sibling;}
 *link=child_handle;child->parent=parent_handle;child->depth=(uint16_t)(parent->depth+1);
 parent->flags|=NOVA_LAYER_DIRTY;++diagnostics.attachments;++diagnostics.mutations;return true;}
bool nova_managed_layer_destroy(nova_layer_handle_t handle)
{if(!mutable()){++diagnostics.rejected_mutations;return false;}nova_managed_layer_t *layer=lookup(handle);
 if(!layer||!layer->parent||layer->first_child)return false;
 if(layer->surface&&
 nova_managed_surface_release(layer->surface)!=NOVA_SURFACE_OK)return false;
 if(!nova_managed_layer_detach(handle))return false;
 layer->active=false;--diagnostics.active;++diagnostics.destroys;return true;}
bool nova_managed_layer_set_visible(nova_layer_handle_t handle,bool visible)
{if(!mutable()){++diagnostics.rejected_mutations;return false;}nova_managed_layer_t *l=lookup(handle);if(!l)return false;
 if(visible)l->flags|=NOVA_LAYER_VISIBLE;else l->flags&=~NOVA_LAYER_VISIBLE;l->flags|=NOVA_LAYER_DIRTY;++diagnostics.mutations;return true;}
bool nova_managed_layer_set_bounds(nova_layer_handle_t handle,nova_rect_t bounds)
{if(!mutable()||bounds.width<0||bounds.height<0){++diagnostics.rejected_mutations;return false;}
 nova_managed_layer_t *l=lookup(handle);if(!l)return false;l->bounds=bounds;l->flags|=NOVA_LAYER_DIRTY;++diagnostics.mutations;return true;}
bool nova_managed_layer_set_opacity(nova_layer_handle_t handle,uint16_t opacity)
{if(!mutable()||opacity>1000){++diagnostics.rejected_mutations;return false;}nova_managed_layer_t *l=lookup(handle);if(!l)return false;
 l->opacity=opacity;if(opacity<1000){l->flags|=NOVA_LAYER_HAS_OPACITY|NOVA_LAYER_ISOLATED;}l->flags|=NOVA_LAYER_DIRTY;++diagnostics.mutations;return true;}
bool nova_managed_layer_set_transform(nova_layer_handle_t handle,nova_scene_matrix_t transform)
{if(!mutable()||!nova_transform_fixed_valid(transform)){++diagnostics.rejected_mutations;return false;}nova_managed_layer_t *l=lookup(handle);if(!l)return false;
 l->transform=transform;l->flags|=NOVA_LAYER_TRANSFORMED|NOVA_LAYER_ISOLATED|NOVA_LAYER_DIRTY;++diagnostics.mutations;return true;}
bool nova_managed_layer_bind_surface(nova_layer_handle_t handle,nova_surface_handle_t surface)
{if(!mutable()){++diagnostics.rejected_mutations;return false;}nova_managed_layer_t *l=lookup(handle);nova_surface_info_t info;
 if(!l||nova_managed_surface_info(surface,&info)!=NOVA_SURFACE_OK)return false;
 if(l->surface)return false;
 l->surface=surface;nova_managed_surface_retain(surface);l->flags|=NOVA_LAYER_ISOLATED|NOVA_LAYER_DIRTY;++diagnostics.mutations;return true;}
static nova_rect_t unite(nova_rect_t a,nova_rect_t b)
{if(a.width<=0||a.height<=0)return b;if(b.width<=0||b.height<=0)return a;int32_t x=a.x<b.x?a.x:b.x,y=a.y<b.y?a.y:b.y;
 int32_t r=a.x+a.width>b.x+b.width?a.x+a.width:b.x+b.width,bt=a.y+a.height>b.y+b.height?a.y+a.height:b.y+b.height;
 return (nova_rect_t){x,y,r-x,bt-y};}
bool nova_managed_layer_mark_dirty(nova_layer_handle_t handle,nova_rect_t damage)
{if(!mutable()||damage.width<=0||damage.height<=0){++diagnostics.rejected_mutations;return false;}nova_managed_layer_t *l=lookup(handle);if(!l)return false;
 l->damage=unite(l->damage,damage);l->flags|=NOVA_LAYER_DIRTY;while(l->parent){l=lookup(l->parent);if(!l)return false;
  l->damage=unite(l->damage,damage);l->flags|=NOVA_LAYER_DIRTY;++diagnostics.damage_propagations;}return true;}
static bool after(const nova_managed_layer_t *a,const nova_managed_layer_t *b)
{uint16_t ac=class_of(a->type),bc=class_of(b->type);return ac>bc||(ac==bc&&(a->z_index>b->z_index||
 (a->z_index==b->z_index&&(a->creation>b->creation||
 (a->creation==b->creation&&a->handle>b->handle)))));}
static nova_scene_matrix_t multiply(nova_scene_matrix_t a,nova_scene_matrix_t b)
{nova_scene_matrix_t result=nova_scene_identity();
 if(!nova_transform_fixed_multiply(&result,a,b))++diagnostics.invalid_handles;
 return result;}
static nova_rect_t intersection(nova_rect_t a,nova_rect_t b)
{if(a.width<=0||a.height<=0)return b;if(b.width<=0||b.height<=0)return a;
 int32_t x=a.x>b.x?a.x:b.x,y=a.y>b.y?a.y:b.y,r=a.x+a.width<b.x+b.width?a.x+a.width:b.x+b.width;
 int32_t bot=a.y+a.height<b.y+b.height?a.y+a.height:b.y+b.height;
 return (nova_rect_t){x,y,r>x?r-x:0,bot>y?bot-y:0};}
static bool resolve(nova_managed_layer_t *layer,uint16_t depth)
{if(depth>NOVA_MANAGED_LAYER_MAX_DEPTH){++diagnostics.depth_errors;return false;}
 if(!layer->parent){layer->world=layer->transform;layer->effective_opacity=layer->opacity;
  layer->effective_clip=layer->clip;return (layer->flags&NOVA_LAYER_VISIBLE)!=0;}
 nova_managed_layer_t *parent=lookup(layer->parent);if(!parent)return false;
 bool parent_visible=resolve(parent,(uint16_t)(depth+1));layer->world=multiply(parent->world,layer->transform);
 layer->effective_opacity=(uint16_t)((uint32_t)parent->effective_opacity*layer->opacity/1000u);
 layer->effective_clip=intersection(parent->effective_clip,layer->clip);
 return parent_visible&&(layer->flags&NOVA_LAYER_VISIBLE)!=0;}
uint16_t nova_layer_manager_build_order(nova_layer_handle_t *out,uint16_t capacity)
{if(!out||!capacity)return 0;nova_layer_manager_set_phase(NOVA_LAYER_PHASE_TRAVERSAL);uint16_t count=0;
 for(uint16_t i=1;i<NOVA_MANAGED_LAYER_CAPACITY&&count<capacity;++i)if(layers[i].active&&
  resolve(&layers[i],0))out[count++]=layers[i].handle;
 for(uint16_t i=1;i<count;++i){nova_layer_handle_t value=out[i];uint16_t j=i;while(j&&after(lookup(out[j-1]),lookup(value))){out[j]=out[j-1];--j;}out[j]=value;}
 ++diagnostics.traversals;nova_layer_manager_set_phase(NOVA_LAYER_PHASE_STATE_UPDATE);return count;}
const nova_managed_layer_t *nova_managed_layer_get(nova_layer_handle_t handle){return lookup(handle);}
const nova_layer_manager_diagnostics_t *nova_layer_manager_diagnostics(void){return &diagnostics;}
