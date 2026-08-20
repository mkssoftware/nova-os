#include "layout.h"
#include "resolution.h"

static nova_layout_diagnostics_t diagnostics;
static nova_alignment_diagnostics_t alignment_diagnostics;
static nova_stack_diagnostics_t stack_diagnostics;
static nova_grid_diagnostics_t grid_diagnostics;
static nova_overlay_diagnostics_t overlay_diagnostics;
static nova_constraint_diagnostics_t constraint_diagnostics;
static nova_responsive_state_t responsive_state;
static nova_responsive_diagnostics_t responsive_diagnostics;
static void layout_clear(volatile void *memory,uint32_t bytes)
{volatile uint8_t *cursor=(volatile uint8_t *)memory;while(bytes--)*cursor++=0;}
static int32_t clamp_i32(int32_t value, int32_t minimum, int32_t maximum)
{
    if (value < minimum) return minimum;
    if (value > maximum) return maximum;
    return value;
}

void nova_layout_initialize(void) { diagnostics = (nova_layout_diagnostics_t){0};
 alignment_diagnostics=(nova_alignment_diagnostics_t){0};
 stack_diagnostics=(nova_stack_diagnostics_t){0};
 grid_diagnostics=(nova_grid_diagnostics_t){0};
 overlay_diagnostics=(nova_overlay_diagnostics_t){.minimum_z=32767,.maximum_z=-32768};
 constraint_diagnostics=(nova_constraint_diagnostics_t){0};
 responsive_state=(nova_responsive_state_t){0};
 responsive_diagnostics=(nova_responsive_diagnostics_t){0};}

static bool valid_insets(nova_layout_insets_t insets)
{return insets.left>=0&&insets.top>=0&&insets.right>=0&&insets.bottom>=0;}
bool nova_layout_apply_alignment(const nova_rect_t *container,
 const nova_rect_t *measured,const nova_alignment_spec_t *spec,
 uint32_t scale,nova_rect_t *result)
{
 if(!container||!measured||!spec||!result||container->width<0||container->height<0||
    measured->width<0||measured->height<0||scale<500||scale>4000||
    spec->horizontal>NOVA_ALIGN_STRETCH||spec->vertical>NOVA_ALIGN_BASELINE||
    spec->distribution>NOVA_DISTRIBUTE_SPACE_EVENLY||spec->spacing_dlu<0||
    !valid_insets(spec->margin)||!valid_insets(spec->padding)){
    ++alignment_diagnostics.errors;++alignment_diagnostics.fallbacks;return false;}
 uint32_t effective_scale=nova_scaling_effective(scale,spec->accessibility);
 int32_t ml=nova_dlu_to_pixels(spec->margin.left,effective_scale);
 int32_t mt=nova_dlu_to_pixels(spec->margin.top,effective_scale);
 int32_t mr=nova_dlu_to_pixels(spec->margin.right,effective_scale);
 int32_t mb=nova_dlu_to_pixels(spec->margin.bottom,effective_scale);
 int32_t pl=nova_dlu_to_pixels(spec->padding.left,effective_scale);
 int32_t pt=nova_dlu_to_pixels(spec->padding.top,effective_scale);
 int32_t pr=nova_dlu_to_pixels(spec->padding.right,effective_scale);
 int32_t pb=nova_dlu_to_pixels(spec->padding.bottom,effective_scale);
 nova_rect_t available={container->x+ml+pl,container->y+mt+pt,
    container->width-ml-mr-pl-pr,container->height-mt-mb-pt-pb};
 if(available.width<0||available.height<0){++alignment_diagnostics.errors;return false;}
 nova_rect_t out={available.x,available.y,measured->width,measured->height};
 if(spec->horizontal==NOVA_ALIGN_STRETCH)out.width=available.width;
 else if(spec->horizontal==NOVA_ALIGN_CENTER)out.x+= (available.width-out.width)/2;
 else if(spec->horizontal==NOVA_ALIGN_RIGHT)out.x+=available.width-out.width;
 if(spec->vertical==NOVA_ALIGN_STRETCH_VERTICAL)out.height=available.height;
 else if(spec->vertical==NOVA_ALIGN_CENTER_VERTICAL)out.y+=(available.height-out.height)/2;
 else if(spec->vertical==NOVA_ALIGN_BOTTOM)out.y+=available.height-out.height;
 else if(spec->vertical==NOVA_ALIGN_BASELINE)
    out.y+=nova_dlu_to_pixels(spec->baseline_dlu,effective_scale)-measured->y;
 if(out.width<0||out.height<0||out.x<available.x||out.y<available.y||
    out.x+out.width>available.x+available.width||
    out.y+out.height>available.y+available.height){++alignment_diagnostics.errors;return false;}
 *result=out;++alignment_diagnostics.applications;++alignment_diagnostics.invalidations;
 alignment_diagnostics.scale_milli=effective_scale;
 alignment_diagnostics.last_container=*container;alignment_diagnostics.last_result=out;
 return true;
}
bool nova_layout_apply_spacing(const nova_rect_t *content,uint16_t count,
 int32_t extent_dlu,int32_t spacing_dlu,nova_distribution_t distribution,
 bool horizontal,uint32_t scale,int32_t *positions,uint16_t capacity)
{
 if(!content||!positions||!count||capacity<count||extent_dlu<0||spacing_dlu<0||
    distribution>NOVA_DISTRIBUTE_SPACE_EVENLY||scale<500||scale>4000){
    ++alignment_diagnostics.errors;return false;}
 int32_t extent=nova_dlu_to_pixels(extent_dlu,scale);
 int32_t base=nova_dlu_to_pixels(spacing_dlu,scale);
 int32_t available=horizontal?content->width:content->height;
 int32_t used=extent*(int32_t)count,free_space=available-used;
 if(free_space<0){++alignment_diagnostics.errors;return false;}
 int32_t gap=base,start=0;
 if(distribution==NOVA_DISTRIBUTE_EQUAL_SPACE)gap=count>1?free_space/(count-1):0;
 else if(distribution==NOVA_DISTRIBUTE_SPACE_BETWEEN)gap=count>1?free_space/(count-1):0;
 else if(distribution==NOVA_DISTRIBUTE_SPACE_AROUND){gap=free_space/(int32_t)count;start=gap/2;}
 else if(distribution==NOVA_DISTRIBUTE_SPACE_EVENLY){gap=free_space/(int32_t)(count+1);start=gap;}
 else if(count>1&&used+gap*(count-1)>available){++alignment_diagnostics.errors;return false;}
 int32_t origin=horizontal?content->x:content->y;
 for(uint16_t i=0;i<count;++i)positions[i]=origin+start+(int32_t)i*(extent+gap);
 ++alignment_diagnostics.spacing_applications;++alignment_diagnostics.invalidations;
 alignment_diagnostics.last_spacing_pixels=gap;alignment_diagnostics.scale_milli=scale;
 return true;
}
bool nova_layout_apply_text(const nova_text_layout_request_t *request,
 nova_layout_insets_t padding,int32_t icon_width_dlu,int32_t icon_gap_dlu,
 nova_rect_t *desired,nova_text_layout_metrics_t *metrics)
{
 if(!request||!desired||!metrics||!valid_insets(padding)||icon_width_dlu<0||
    icon_gap_dlu<0){++alignment_diagnostics.errors;return false;}
 if(!nova_text_layout_measure(request,metrics)){
    ++alignment_diagnostics.errors;return false;}
 uint32_t scale=request->scale_milli;
 if(request->accessibility)scale=scale<=3200?(scale*5u)/4u:4000u;
 int64_t horizontal=(int64_t)nova_dlu_to_pixels(padding.left+padding.right,scale);
 int64_t vertical=(int64_t)nova_dlu_to_pixels(padding.top+padding.bottom,scale);
 if(icon_width_dlu)horizontal+=nova_dlu_to_pixels(icon_width_dlu+icon_gap_dlu,scale);
 int64_t width=(int64_t)metrics->width+horizontal;
 int64_t height=(int64_t)metrics->height+vertical;
 if(width>INT32_MAX||height>INT32_MAX){++alignment_diagnostics.errors;return false;}
 *desired=(nova_rect_t){0,nova_dlu_to_pixels(padding.top,scale)+metrics->baseline,
                       (int32_t)width,(int32_t)height};
 ++alignment_diagnostics.invalidations;return true;
}
const nova_alignment_diagnostics_t *nova_alignment_diagnostics(void)
{return &alignment_diagnostics;}

bool nova_stack_initialize(nova_stack_t *stack,nova_stack_orientation_t orientation,
 nova_rect_t bounds,nova_layout_insets_t padding,int32_t spacing_dlu,
 nova_distribution_t distribution,uint32_t scale,bool accessibility)
{
 if(!stack||orientation>NOVA_STACK_HORIZONTAL||bounds.width<0||bounds.height<0||
    !valid_insets(padding)||spacing_dlu<0||distribution>NOVA_DISTRIBUTE_SPACE_EVENLY||
    scale<500||scale>4000){++stack_diagnostics.errors;return false;}
 layout_clear(stack,sizeof(*stack));stack->orientation=orientation;stack->bounds=bounds;
 stack->padding=padding;stack->spacing_dlu=spacing_dlu;stack->distribution=distribution;
 stack->scale_milli=scale;stack->accessibility=accessibility;stack->dirty=true;return true;
}
bool nova_stack_add(nova_stack_t *stack,const nova_stack_child_t *child)
{
 if(!stack||!child||stack->child_count>=NOVA_STACK_CHILD_CAPACITY||
    child->desired.width<0||child->desired.height<0||
    child->horizontal>NOVA_ALIGN_STRETCH||child->vertical>NOVA_ALIGN_BASELINE){
    ++stack_diagnostics.errors;return false;}
 for(uint16_t i=0;i<stack->child_count;++i)
    if((child->id&&stack->children[i].id==child->id)||
       (child->nested&&stack->children[i].nested==child->nested)){
       ++stack_diagnostics.duplicates;return false;}
 if(child->nested){
    for(nova_stack_t *p=stack;p;p=p->parent)if(p==child->nested){
       ++stack_diagnostics.cycles;return false;}
    if(child->nested->parent&&child->nested->parent!=stack){
       ++stack_diagnostics.duplicates;return false;}
    child->nested->parent=stack;
 }
 stack->children[stack->child_count++]=*child;stack->dirty=true;
 ++stack_diagnostics.invalidations;return true;
}
bool nova_stack_remove(nova_stack_t *stack,uint16_t index)
{
 if(!stack||index>=stack->child_count){++stack_diagnostics.errors;return false;}
 if(stack->children[index].nested)stack->children[index].nested->parent=0;
 for(uint16_t i=index+1;i<stack->child_count;++i)stack->children[i-1]=stack->children[i];
 --stack->child_count;stack->dirty=true;++stack_diagnostics.invalidations;return true;
}
static bool stack_measure_depth(nova_stack_t *stack,uint16_t depth)
{
 if(!stack||depth>NOVA_STACK_DEPTH_LIMIT){++stack_diagnostics.cycles;return false;}
 if(depth>stack_diagnostics.max_depth)stack_diagnostics.max_depth=depth;
 uint32_t scale=nova_scaling_effective(stack->scale_milli,stack->accessibility);
 int32_t pl=nova_dlu_to_pixels(stack->padding.left,scale),
    pt=nova_dlu_to_pixels(stack->padding.top,scale),
    pr=nova_dlu_to_pixels(stack->padding.right,scale),
    pb=nova_dlu_to_pixels(stack->padding.bottom,scale),
    spacing=nova_dlu_to_pixels(stack->spacing_dlu,scale),main=0,cross=0;
 uint16_t visible=0;
 for(uint16_t i=0;i<stack->child_count;++i){nova_stack_child_t *child=&stack->children[i];
    if(!child->visible){++stack_diagnostics.skipped;continue;}
    if(child->nested){if(child->nested->parent!=stack||
       !stack_measure_depth(child->nested,(uint16_t)(depth+1)))return false;
       child->desired=child->nested->measured;}
    if(child->use_text_layout){
       nova_text_layout_request_t request=child->text_layout;
       request.scale_milli=stack->scale_milli;request.accessibility=stack->accessibility;
       if(!request.available_width)request.available_width=stack->bounds.width-pl-pr;
       if(!nova_layout_apply_text(&request,child->adaptive_constraints.padding,
          child->icon_width_dlu,child->icon_gap_dlu,&child->desired,
          &child->text_metrics)){++stack_diagnostics.errors;return false;}
    }
    if(child->use_adaptive_constraints){nova_rect_t resolved;
       if(!nova_constraints_resolve_dlu(&child->adaptive_constraints,stack->bounds,
          &resolved,stack->scale_milli))return false;
       child->desired.width=resolved.width;child->desired.height=resolved.height;}
    uint32_t width=0,height=0;
    nova_size_constraints_t c=child->constraints;
    if(!c.max_width)c.max_width=0x7fffffffu;
    if(!c.max_height)c.max_height=0x7fffffffu;
    if(!c.preferred_width)c.preferred_width=(uint32_t)child->desired.width;
    if(!c.preferred_height)c.preferred_height=(uint32_t)child->desired.height;
    if(!nova_constraints_resolve(&c,0x7fffffffu,0x7fffffffu,&width,&height)){
       ++stack_diagnostics.errors;return false;}
    child->desired.width=(int32_t)width;child->desired.height=(int32_t)height;
    int32_t cm=stack->orientation==NOVA_STACK_VERTICAL?child->desired.height:child->desired.width;
    int32_t cc=stack->orientation==NOVA_STACK_VERTICAL?child->desired.width:child->desired.height;
    main+=cm;if(cc>cross)cross=cc;++visible;}
 if(visible>1)main+=(int32_t)(visible-1)*spacing;
 stack->measured=stack->orientation==NOVA_STACK_VERTICAL?
    (nova_rect_t){0,0,cross+pl+pr,main+pt+pb}:(nova_rect_t){0,0,main+pl+pr,cross+pt+pb};
 ++stack_diagnostics.measures;stack_diagnostics.measured_area+=
    (uint64_t)stack->measured.width*(uint64_t)stack->measured.height;return true;
}
bool nova_stack_measure(nova_stack_t *stack){return stack_measure_depth(stack,1);}
static bool stack_arrange_depth(nova_stack_t *stack,uint16_t depth)
{
 if(!stack||depth>NOVA_STACK_DEPTH_LIMIT){++stack_diagnostics.cycles;return false;}
 uint32_t scale=nova_scaling_effective(stack->scale_milli,stack->accessibility);
 int32_t pl=nova_dlu_to_pixels(stack->padding.left,scale),pt=nova_dlu_to_pixels(stack->padding.top,scale),
    pr=nova_dlu_to_pixels(stack->padding.right,scale),pb=nova_dlu_to_pixels(stack->padding.bottom,scale);
 nova_rect_t content={stack->bounds.x+pl,stack->bounds.y+pt,
    stack->bounds.width-pl-pr,stack->bounds.height-pt-pb};
 if(content.width<0||content.height<0){++stack_diagnostics.errors;return false;}
 uint16_t visible=0,stretch=0;int32_t fixed=0;
 for(uint16_t i=0;i<stack->child_count;++i)if(stack->children[i].visible){
    ++visible;if(stack->children[i].stretch)++stretch;else fixed+=stack->orientation==NOVA_STACK_VERTICAL?
       stack->children[i].desired.height:stack->children[i].desired.width;}
 int32_t available=stack->orientation==NOVA_STACK_VERTICAL?content.height:content.width;
 int32_t gap=nova_dlu_to_pixels(stack->spacing_dlu,scale);
 int32_t remaining=available-fixed-(visible>1?(int32_t)(visible-1)*gap:0);
 if(remaining<0){++stack_diagnostics.errors;return false;}
 int32_t stretch_extent=stretch?remaining/stretch:0,position=stack->orientation==NOVA_STACK_VERTICAL?content.y:content.x;
 if(!stretch&&stack->distribution!=NOVA_DISTRIBUTE_NONE&&visible){
    int32_t used=fixed,free_space=available-used;
    if(stack->distribution==NOVA_DISTRIBUTE_SPACE_BETWEEN||stack->distribution==NOVA_DISTRIBUTE_EQUAL_SPACE)
       gap=visible>1?free_space/(visible-1):0;
    else if(stack->distribution==NOVA_DISTRIBUTE_SPACE_AROUND){gap=free_space/visible;position+=gap/2;}
    else {gap=free_space/(visible+1);position+=gap;}}
 for(uint16_t i=0;i<stack->child_count;++i){nova_stack_child_t *child=&stack->children[i];if(!child->visible)continue;
    int32_t main=child->stretch?stretch_extent:(stack->orientation==NOVA_STACK_VERTICAL?child->desired.height:child->desired.width);
    nova_rect_t slot=stack->orientation==NOVA_STACK_VERTICAL?(nova_rect_t){content.x,position,content.width,main}:
       (nova_rect_t){position,content.y,main,content.height};
    nova_alignment_spec_t spec={.horizontal=child->horizontal,.vertical=child->vertical};
    if(stack->orientation==NOVA_STACK_VERTICAL&&child->stretch)spec.vertical=NOVA_ALIGN_STRETCH_VERTICAL;
    if(stack->orientation==NOVA_STACK_HORIZONTAL&&child->stretch)spec.horizontal=NOVA_ALIGN_STRETCH;
    if(!nova_layout_apply_alignment(&slot,&child->desired,&spec,scale,&child->bounds))return false;
    if(child->nested){child->nested->bounds=child->bounds;if(!stack_arrange_depth(child->nested,(uint16_t)(depth+1)))return false;}
    position+=main+gap;}
 ++stack_diagnostics.arranges;stack->dirty=false;return true;
}
bool nova_stack_arrange(nova_stack_t *stack){return stack_arrange_depth(stack,1);}
bool nova_stack_update(nova_stack_t *stack)
{if(!stack)return false;if(!stack->dirty)return true;
 if(!nova_stack_measure(stack)||!nova_stack_arrange(stack))return false;
 ++stack_diagnostics.updates;stack_diagnostics.last_child_count=stack->child_count;
 stack_diagnostics.last_orientation=stack->orientation;return true;}
const nova_stack_diagnostics_t *nova_stack_diagnostics(void){return &stack_diagnostics;}

bool nova_grid_initialize(nova_grid_t *grid,nova_rect_t bounds,uint32_t scale,
 int32_t row_spacing,int32_t column_spacing,bool accessibility)
{
 if(!grid||bounds.width<0||bounds.height<0||scale<500||scale>4000||
    row_spacing<0||column_spacing<0){++grid_diagnostics.errors;return false;}
 layout_clear(grid,sizeof(*grid));grid->bounds=bounds;grid->scale_milli=scale;
 grid->row_spacing_dlu=row_spacing;grid->column_spacing_dlu=column_spacing;
 grid->accessibility=accessibility;grid->dirty=true;return true;
}
static bool grid_add_track(nova_grid_t *grid,bool row,nova_grid_unit_t unit,uint16_t value)
{
 if(!grid||unit>NOVA_GRID_PERCENT||(unit!=NOVA_GRID_AUTO&&!value)||
    (unit==NOVA_GRID_PERCENT&&value>1000)||
    (row?grid->row_count:grid->column_count)>=NOVA_GRID_TRACK_CAPACITY){
    ++grid_diagnostics.errors;return false;}
 nova_grid_track_t track={unit,value,0};
 if(row)grid->rows[grid->row_count++]=track;
 else grid->columns[grid->column_count++]=track;
 grid->dirty=true;++grid_diagnostics.invalidations;return true;
}
bool nova_grid_add_row(nova_grid_t *grid,nova_grid_unit_t unit,uint16_t value)
{return grid_add_track(grid,true,unit,value);}
bool nova_grid_add_column(nova_grid_t *grid,nova_grid_unit_t unit,uint16_t value)
{return grid_add_track(grid,false,unit,value);}
bool nova_grid_add(nova_grid_t *grid,const nova_grid_child_t *child)
{
 if(!grid||!child||grid->child_count>=NOVA_GRID_CHILD_CAPACITY||!child->row_span||
    !child->column_span||child->row>=grid->row_count||child->column>=grid->column_count||
    child->row+child->row_span>grid->row_count||
    child->column+child->column_span>grid->column_count||
    child->desired.width<0||child->desired.height<0||!valid_insets(child->margin)||
    !valid_insets(child->padding)||child->horizontal>NOVA_ALIGN_STRETCH||
    child->vertical>NOVA_ALIGN_BASELINE){++grid_diagnostics.errors;return false;}
 for(uint8_t i=0;i<grid->child_count;++i){nova_grid_child_t *existing=&grid->children[i];
    if((child->id&&existing->id==child->id)||
       (child->nested&&existing->nested==child->nested)){
       ++grid_diagnostics.duplicates;return false;}
    bool rows=child->row<existing->row+existing->row_span&&
              existing->row<child->row+child->row_span;
    bool columns=child->column<existing->column+existing->column_span&&
                 existing->column<child->column+child->column_span;
    if(rows&&columns){++grid_diagnostics.overlaps;return false;}}
 if(child->nested){
    for(nova_grid_t *parent=grid;parent;parent=parent->parent)
       if(parent==child->nested){++grid_diagnostics.cycles;return false;}
    if(child->nested->parent&&child->nested->parent!=grid){
       ++grid_diagnostics.duplicates;return false;}
    child->nested->parent=grid;
 }
 grid->children[grid->child_count++]=*child;grid->dirty=true;
 if(child->row_span>1||child->column_span>1)++grid_diagnostics.spans;
 ++grid_diagnostics.invalidations;return true;
}
static bool grid_measure_depth(nova_grid_t *grid,uint8_t depth)
{
 if(!grid||depth>NOVA_GRID_DEPTH_LIMIT||!grid->row_count||!grid->column_count){
    ++grid_diagnostics.errors;return false;}
 if(depth>grid_diagnostics.max_depth)grid_diagnostics.max_depth=depth;
 uint32_t scale=nova_scaling_effective(grid->scale_milli,grid->accessibility);
 for(uint8_t r=0;r<grid->row_count;++r)grid->rows[r].pixels=
    grid->rows[r].unit==NOVA_GRID_FIXED?nova_dlu_to_pixels(grid->rows[r].value,scale):0;
 for(uint8_t c=0;c<grid->column_count;++c)grid->columns[c].pixels=
    grid->columns[c].unit==NOVA_GRID_FIXED?nova_dlu_to_pixels(grid->columns[c].value,scale):0;
 for(uint8_t i=0;i<grid->child_count;++i){nova_grid_child_t *child=&grid->children[i];
    if(!child->visible&&!child->reserve_when_hidden){++grid_diagnostics.hidden_excluded;continue;}
    if(child->nested){if(child->nested->parent!=grid||
       !grid_measure_depth(child->nested,(uint8_t)(depth+1)))return false;
       child->desired=child->nested->measured;}
    if(child->use_text_layout){nova_text_layout_request_t request=child->text_layout;
       request.scale_milli=grid->scale_milli;request.accessibility=grid->accessibility;
       if(!request.available_width)request.available_width=grid->bounds.width;
       if(!nova_layout_apply_text(&request,child->padding,child->icon_width_dlu,
          child->icon_gap_dlu,&child->desired,&child->text_metrics)){
          ++grid_diagnostics.errors;return false;}}
    if(child->use_adaptive_constraints){nova_rect_t resolved;
       if(!nova_constraints_resolve_dlu(&child->adaptive_constraints,grid->bounds,
          &resolved,grid->scale_milli))return false;
       child->desired.width=resolved.width;child->desired.height=resolved.height;}
    nova_size_constraints_t constraints=child->constraints;
    if(!constraints.max_width)constraints.max_width=0x7fffffffu;
    if(!constraints.max_height)constraints.max_height=0x7fffffffu;
    if(!constraints.preferred_width)constraints.preferred_width=(uint32_t)child->desired.width;
    if(!constraints.preferred_height)constraints.preferred_height=(uint32_t)child->desired.height;
    uint32_t desired_width=0,desired_height=0;
    if(!nova_constraints_resolve(&constraints,0x7fffffffu,0x7fffffffu,
       &desired_width,&desired_height)){++grid_diagnostics.errors;return false;}
    child->desired.width=(int32_t)desired_width;child->desired.height=(int32_t)desired_height;
    if(child->row_span==1&&grid->rows[child->row].unit==NOVA_GRID_AUTO&&
       child->desired.height>grid->rows[child->row].pixels)
       grid->rows[child->row].pixels=child->desired.height;
    if(child->column_span==1&&grid->columns[child->column].unit==NOVA_GRID_AUTO&&
       child->desired.width>grid->columns[child->column].pixels)
       grid->columns[child->column].pixels=child->desired.width;}
 for(uint8_t i=0;i<grid->child_count;++i){nova_grid_child_t *child=&grid->children[i];
    if((!child->visible&&!child->reserve_when_hidden)||
       (child->row_span==1&&child->column_span==1))continue;
    int32_t width=0,height=0;int8_t auto_column=-1,auto_row=-1;
    for(uint8_t c=0;c<child->column_span;++c){uint8_t index=child->column+c;
       width+=grid->columns[index].pixels;if(grid->columns[index].unit==NOVA_GRID_AUTO)
          auto_column=(int8_t)index;}
    for(uint8_t r=0;r<child->row_span;++r){uint8_t index=child->row+r;
       height+=grid->rows[index].pixels;if(grid->rows[index].unit==NOVA_GRID_AUTO)
          auto_row=(int8_t)index;}
    if(width<child->desired.width&&auto_column>=0)
       grid->columns[(uint8_t)auto_column].pixels+=child->desired.width-width;
    if(height<child->desired.height&&auto_row>=0)
       grid->rows[(uint8_t)auto_row].pixels+=child->desired.height-height;}
 int32_t width=0,height=0;
 for(uint8_t r=0;r<grid->row_count;++r)height+=grid->rows[r].pixels;
 for(uint8_t c=0;c<grid->column_count;++c)width+=grid->columns[c].pixels;
 width+=(grid->column_count-1)*nova_dlu_to_pixels(grid->column_spacing_dlu,scale);
 height+=(grid->row_count-1)*nova_dlu_to_pixels(grid->row_spacing_dlu,scale);
 grid->measured=(nova_rect_t){0,0,width,height};++grid_diagnostics.measures;return true;
}
bool nova_grid_measure(nova_grid_t *grid){return grid_measure_depth(grid,1);}
static bool grid_resolve_tracks(nova_grid_track_t *tracks,uint8_t count,
 int32_t available,uint32_t scale)
{
 int32_t reserved=0;uint32_t star_weight=0;
 for(uint8_t i=0;i<count;++i){
    if(tracks[i].unit==NOVA_GRID_PERCENT){tracks[i].pixels=(int32_t)
       ((int64_t)available*tracks[i].value/1000);reserved+=tracks[i].pixels;}
    else if(tracks[i].unit==NOVA_GRID_STAR)star_weight+=tracks[i].value;
    else reserved+=tracks[i].pixels;}
 if(reserved>available||(!star_weight&&reserved<0)){++grid_diagnostics.errors;return false;}
 int32_t remaining=available-reserved;
 for(uint8_t i=0;i<count;++i)if(tracks[i].unit==NOVA_GRID_STAR)
    tracks[i].pixels=(int32_t)((int64_t)remaining*tracks[i].value/star_weight);
 (void)scale;return true;
}
static bool grid_arrange_depth(nova_grid_t *grid,uint8_t depth)
{
 if(!grid||depth>NOVA_GRID_DEPTH_LIMIT){++grid_diagnostics.cycles;return false;}
 uint32_t scale=nova_scaling_effective(grid->scale_milli,grid->accessibility);
 int32_t row_gap=nova_dlu_to_pixels(grid->row_spacing_dlu,scale),
    column_gap=nova_dlu_to_pixels(grid->column_spacing_dlu,scale);
 int32_t track_width=grid->bounds.width-(grid->column_count-1)*column_gap;
 int32_t track_height=grid->bounds.height-(grid->row_count-1)*row_gap;
 if(track_width<0||track_height<0||!grid_resolve_tracks(grid->columns,
    grid->column_count,track_width,scale)||!grid_resolve_tracks(grid->rows,
    grid->row_count,track_height,scale))return false;
 int32_t row_pos[NOVA_GRID_TRACK_CAPACITY],column_pos[NOVA_GRID_TRACK_CAPACITY];
 int32_t p=grid->bounds.y;for(uint8_t r=0;r<grid->row_count;++r){row_pos[r]=p;
    p+=grid->rows[r].pixels+row_gap;}
 p=grid->bounds.x;for(uint8_t c=0;c<grid->column_count;++c){column_pos[c]=p;
    p+=grid->columns[c].pixels+column_gap;}
 for(uint8_t i=0;i<grid->child_count;++i){nova_grid_child_t *child=&grid->children[i];
    if(!child->visible&&!child->reserve_when_hidden)continue;
    int32_t cell_width=0,cell_height=0;
    for(uint8_t c=0;c<child->column_span;++c)cell_width+=grid->columns[child->column+c].pixels;
    for(uint8_t r=0;r<child->row_span;++r)cell_height+=grid->rows[child->row+r].pixels;
    cell_width+=(child->column_span-1)*column_gap;cell_height+=(child->row_span-1)*row_gap;
    nova_alignment_spec_t spec={.horizontal=child->horizontal,.vertical=child->vertical,
       .margin=child->margin,.padding=child->padding,.accessibility=grid->accessibility};
    nova_rect_t cell={column_pos[child->column],row_pos[child->row],cell_width,cell_height};
    if(!nova_layout_apply_alignment(&cell,&child->desired,&spec,scale,&child->bounds)){
       ++grid_diagnostics.errors;return false;}
    if(child->nested){child->nested->bounds=child->bounds;
       if(!grid_arrange_depth(child->nested,(uint8_t)(depth+1)))return false;}}
 ++grid_diagnostics.arranges;grid->dirty=false;return true;
}
bool nova_grid_arrange(nova_grid_t *grid){return grid_arrange_depth(grid,1);}
bool nova_grid_update(nova_grid_t *grid)
{if(!grid)return false;if(!grid->dirty)return true;
 if(!nova_grid_measure(grid)||!nova_grid_arrange(grid))return false;
 ++grid_diagnostics.updates;grid_diagnostics.last_rows=grid->row_count;
 grid_diagnostics.last_columns=grid->column_count;
 grid_diagnostics.last_children=grid->child_count;return true;}
const nova_grid_diagnostics_t *nova_grid_diagnostics(void){return &grid_diagnostics;}

static bool overlay_before(const nova_overlay_child_t *a,const nova_overlay_child_t *b)
{return a->layer<b->layer||(a->layer==b->layer&&a->z_index<=b->z_index);}
static nova_rect_t overlay_intersect(nova_rect_t a,nova_rect_t b)
{int32_t left=a.x>b.x?a.x:b.x,top=a.y>b.y?a.y:b.y;
 int32_t right=a.x+a.width<b.x+b.width?a.x+a.width:b.x+b.width;
 int32_t bottom=a.y+a.height<b.y+b.height?a.y+a.height:b.y+b.height;
 return right>left&&bottom>top?(nova_rect_t){left,top,right-left,bottom-top}:(nova_rect_t){0};}
bool nova_overlay_initialize(nova_overlay_t *overlay,nova_rect_t bounds,
 nova_layout_insets_t padding,uint32_t scale,bool clipping,bool accessibility)
{
 if(!overlay||bounds.width<0||bounds.height<0||!valid_insets(padding)||
    scale<500||scale>4000){++overlay_diagnostics.errors;return false;}
 layout_clear(overlay,sizeof(*overlay));overlay->bounds=bounds;overlay->clip=bounds;
 overlay->padding=padding;overlay->scale_milli=scale;overlay->clipping=clipping;
 overlay->accessibility=accessibility;overlay->dirty=true;
 return true;
}
bool nova_overlay_add(nova_overlay_t *overlay,const nova_overlay_child_t *child)
{
 if(!overlay||!child||overlay->child_count>=NOVA_OVERLAY_CHILD_CAPACITY||!child->id||
    child->desired.width<0||child->desired.height<0||!valid_insets(child->margin)||
    child->horizontal>NOVA_ALIGN_STRETCH||child->vertical>NOVA_ALIGN_BASELINE||
    child->layer>NOVA_OVERLAY_DEBUG||child->material>NOVA_MATERIAL_SOLID||
    child->visibility>NOVA_OVERLAY_COLLAPSED){++overlay_diagnostics.errors;return false;}
 for(uint8_t i=0;i<overlay->child_count;++i){nova_overlay_child_t *existing=&overlay->children[i];
    if(existing->id==child->id||(child->nested&&existing->nested==child->nested)){
       ++overlay_diagnostics.duplicates;return false;}
    if(child->modal&&existing->modal&&child->layer==existing->layer){
       ++overlay_diagnostics.modal_conflicts;return false;}}
 if(child->nested){
    for(nova_overlay_t *parent=overlay;parent;parent=parent->parent)
       if(parent==child->nested){++overlay_diagnostics.cycles;return false;}
    if(child->nested->parent&&child->nested->parent!=overlay){
       ++overlay_diagnostics.duplicates;return false;}
    child->nested->parent=overlay;}
 uint8_t index=overlay->child_count;overlay->children[index]=*child;
 uint8_t position=0;
 while(position<overlay->child_count&&overlay_before(
    &overlay->children[overlay->order[position]],child))++position;
 for(uint8_t i=overlay->child_count;i>position;--i)overlay->order[i]=overlay->order[i-1];
 overlay->order[position]=index;++overlay->child_count;overlay->dirty=true;
 if(child->z_index<overlay_diagnostics.minimum_z)overlay_diagnostics.minimum_z=child->z_index;
 if(child->z_index>overlay_diagnostics.maximum_z)overlay_diagnostics.maximum_z=child->z_index;
 ++overlay_diagnostics.invalidations;return true;
}
bool nova_overlay_set_visibility(nova_overlay_t *overlay,uint32_t id,
 nova_overlay_visibility_t visibility)
{
 if(!overlay||visibility>NOVA_OVERLAY_COLLAPSED){++overlay_diagnostics.errors;return false;}
 for(uint8_t i=0;i<overlay->child_count;++i)if(overlay->children[i].id==id){
    if(overlay->children[i].visibility!=visibility){overlay->children[i].visibility=visibility;
       overlay->dirty=true;++overlay_diagnostics.visibility_changes;
       ++overlay_diagnostics.invalidations;}return true;}
 ++overlay_diagnostics.errors;return false;
}
static bool overlay_measure_depth(nova_overlay_t *overlay,uint8_t depth)
{
 if(!overlay||depth>NOVA_OVERLAY_DEPTH_LIMIT){++overlay_diagnostics.cycles;return false;}
 if(depth>overlay_diagnostics.max_depth)overlay_diagnostics.max_depth=depth;
 for(uint8_t i=0;i<overlay->child_count;++i){nova_overlay_child_t *child=&overlay->children[i];
    if(child->visibility==NOVA_OVERLAY_COLLAPSED)continue;
    if(child->nested){if(child->nested->parent!=overlay||
       !overlay_measure_depth(child->nested,(uint8_t)(depth+1)))return false;
       child->desired=child->nested->bounds;}
    if(child->use_text_layout){nova_text_layout_request_t request=child->text_layout;
       request.scale_milli=overlay->scale_milli;
       request.accessibility=overlay->accessibility;
       if(!request.available_width)request.available_width=overlay->bounds.width;
       if(!nova_layout_apply_text(&request,child->text_padding,child->icon_width_dlu,
          child->icon_gap_dlu,&child->desired,&child->text_metrics)){
          ++overlay_diagnostics.errors;return false;}}
    if(child->use_adaptive_constraints){nova_rect_t resolved;
       if(!nova_constraints_resolve_dlu(&child->adaptive_constraints,overlay->bounds,
          &resolved,overlay->scale_milli))return false;
       child->desired.width=resolved.width;child->desired.height=resolved.height;}}
 ++overlay_diagnostics.measures;return true;
}
bool nova_overlay_measure(nova_overlay_t *overlay){return overlay_measure_depth(overlay,1);}
static bool overlay_arrange_depth(nova_overlay_t *overlay,uint8_t depth)
{
 if(!overlay||depth>NOVA_OVERLAY_DEPTH_LIMIT){++overlay_diagnostics.cycles;return false;}
 uint32_t scale=nova_scaling_effective(overlay->scale_milli,overlay->accessibility);
 int32_t pl=nova_dlu_to_pixels(overlay->padding.left,scale),
    pt=nova_dlu_to_pixels(overlay->padding.top,scale),
    pr=nova_dlu_to_pixels(overlay->padding.right,scale),
    pb=nova_dlu_to_pixels(overlay->padding.bottom,scale);
 nova_rect_t content={overlay->bounds.x+pl,overlay->bounds.y+pt,
    overlay->bounds.width-pl-pr,overlay->bounds.height-pt-pb};
 if(content.width<0||content.height<0){++overlay_diagnostics.errors;return false;}
 overlay->clip=overlay->clipping?content:overlay->bounds;overlay->modal_active=false;
 for(uint8_t order=0;order<overlay->child_count;++order){
    nova_overlay_child_t *child=&overlay->children[overlay->order[order]];
    if(child->visibility==NOVA_OVERLAY_COLLAPSED)continue;
    nova_rect_t measured=child->desired;
    if(!measured.width&&child->horizontal==NOVA_ALIGN_STRETCH)measured.width=content.width;
    if(!measured.height&&child->vertical==NOVA_ALIGN_STRETCH_VERTICAL)measured.height=content.height;
    if(overlay->accessibility&&child->interactive){int32_t minimum=nova_dlu_to_pixels(44,scale);
       if(measured.width<minimum)measured.width=minimum;
       if(measured.height<minimum)measured.height=minimum;}
    nova_alignment_spec_t spec={.horizontal=child->horizontal,.vertical=child->vertical,
       .margin=child->margin,.accessibility=overlay->accessibility};
    if(!nova_layout_apply_alignment(&content,&measured,&spec,scale,&child->bounds)){
       ++overlay_diagnostics.errors;return false;}
    if(child->absolute){child->bounds.x=content.x+nova_dlu_to_pixels(child->offset_x_dlu,scale);
       child->bounds.y=content.y+nova_dlu_to_pixels(child->offset_y_dlu,scale);}
    if(overlay->clipping){nova_rect_t clipped=overlay_intersect(child->bounds,overlay->clip);
       if(clipped.width!=child->bounds.width||clipped.height!=child->bounds.height)
          ++overlay_diagnostics.clips;
       child->bounds=clipped;}
    if(child->modal&&child->visibility==NOVA_OVERLAY_VISIBLE){overlay->modal_active=true;
       overlay->modal_id=child->id;}
    if(child->nested){child->nested->bounds=child->bounds;
       if(!overlay_arrange_depth(child->nested,(uint8_t)(depth+1)))return false;}}
 ++overlay_diagnostics.arranges;overlay->dirty=false;return true;
}
bool nova_overlay_arrange(nova_overlay_t *overlay){return overlay_arrange_depth(overlay,1);}
bool nova_overlay_update(nova_overlay_t *overlay)
{if(!overlay)return false;if(!overlay->dirty)return true;
 if(!nova_overlay_measure(overlay)||!nova_overlay_arrange(overlay))return false;
 ++overlay_diagnostics.updates;overlay_diagnostics.last_children=overlay->child_count;
 uint8_t layers=0;bool seen[7]={false};for(uint8_t i=0;i<overlay->child_count;++i)
    if(!seen[overlay->children[i].layer]){seen[overlay->children[i].layer]=true;++layers;}
 overlay_diagnostics.last_layers=layers;return true;}
const nova_overlay_child_t *nova_overlay_hit_test(nova_overlay_t *overlay,int32_t x,int32_t y)
{
 if(!overlay)return 0;
 ++overlay_diagnostics.hit_tests;
 for(uint8_t cursor=overlay->child_count;cursor;--cursor){nova_overlay_child_t *child=
    &overlay->children[overlay->order[cursor-1]];
    if(child->visibility!=NOVA_OVERLAY_VISIBLE||!child->interactive)continue;
    if(x>=child->bounds.x&&y>=child->bounds.y&&x<child->bounds.x+child->bounds.width&&
       y<child->bounds.y+child->bounds.height&&nova_overlay_input_allowed(overlay,child->id))return child;}
 return 0;
}
bool nova_overlay_input_allowed(const nova_overlay_t *overlay,uint32_t id)
{
 if(!overlay)return false;
 if(!overlay->modal_active)return true;
 const nova_overlay_child_t *modal=0,*target=0;
 for(uint8_t i=0;i<overlay->child_count;++i){
    if(overlay->children[i].id==overlay->modal_id)modal=&overlay->children[i];
    if(overlay->children[i].id==id)target=&overlay->children[i];}
 bool allowed=modal&&target&&(target==modal||target->layer>modal->layer||
    (target->layer==modal->layer&&target->z_index>=modal->z_index));
 if(!allowed)++overlay_diagnostics.input_blocks;
 return allowed;
}
const nova_overlay_diagnostics_t *nova_overlay_diagnostics(void)
{return &overlay_diagnostics;}

int32_t nova_dlu_to_pixels(int32_t dlu, uint32_t scale_milli)
{
    int64_t product = (int64_t)dlu * scale_milli;
    if (product >= 0) return (int32_t)((product + 500) / 1000);
    return (int32_t)((product - 500) / 1000);
}

bool nova_constraints_validate(const nova_constraint_spec_t *spec)
{
 ++constraint_diagnostics.validations;
 if(!spec||spec->width_mode>NOVA_SIZE_REMAINING||spec->height_mode>NOVA_SIZE_REMAINING||
    spec->width_dlu<0||spec->height_dlu<0||spec->min_width_dlu<0||
    spec->min_height_dlu<0||spec->preferred_width_dlu<0||
    spec->preferred_height_dlu<0||spec->max_width_dlu<0||spec->max_height_dlu<0||
    spec->content_width_dlu<0||spec->content_height_dlu<0||
    spec->width_percent_milli>1000||spec->height_percent_milli>1000||
    !valid_insets(spec->margin)||!valid_insets(spec->padding)||
    ((spec->aspect_width==0)!=(spec->aspect_height==0))||
    (spec->max_width_dlu&&spec->min_width_dlu>spec->max_width_dlu)||
    (spec->max_height_dlu&&spec->min_height_dlu>spec->max_height_dlu)){
    ++constraint_diagnostics.errors;return false;}
 return true;
}
static int32_t constraint_axis(nova_size_mode_t mode,int32_t fixed_dlu,
 int32_t content_dlu,int32_t preferred_dlu,uint16_t percent,uint16_t stretch_weight,
 int32_t available,uint32_t scale)
{
 int32_t value=0;
 if(mode==NOVA_SIZE_FIXED)value=nova_dlu_to_pixels(fixed_dlu,scale);
 else if(preferred_dlu>0&&mode!=NOVA_SIZE_AUTO&&mode!=NOVA_SIZE_CONTENT){
    value=nova_dlu_to_pixels(preferred_dlu,scale);++constraint_diagnostics.preferred_wins;}
 else if(mode==NOVA_SIZE_PERCENT){value=(int32_t)((int64_t)available*percent/1000);
    ++constraint_diagnostics.percent_uses;}
 else if(mode==NOVA_SIZE_STRETCH||mode==NOVA_SIZE_REMAINING){
    value=available;++constraint_diagnostics.stretch_uses;(void)stretch_weight;}
 else value=nova_dlu_to_pixels(content_dlu,scale);
 return value;
}
bool nova_constraints_resolve_dlu(const nova_constraint_spec_t *spec,
 nova_rect_t available,nova_rect_t *result,uint32_t scale)
{
 if(!result||available.width<0||available.height<0||scale<500||scale>4000||
    !nova_constraints_validate(spec)){
    if(result)*result=available;
    ++constraint_diagnostics.fallbacks;return false;}
 uint32_t effective=nova_scaling_effective(scale,spec->accessibility);
 int32_t ml=nova_dlu_to_pixels(spec->margin.left,effective),
    mt=nova_dlu_to_pixels(spec->margin.top,effective),
    mr=nova_dlu_to_pixels(spec->margin.right,effective),
    mb=nova_dlu_to_pixels(spec->margin.bottom,effective);
 nova_rect_t inner={available.x+ml,available.y+mt,available.width-ml-mr,
    available.height-mt-mb};
 if(inner.width<0||inner.height<0){*result=available;++constraint_diagnostics.fallbacks;
    ++constraint_diagnostics.errors;return false;}
 int32_t padding_width=nova_dlu_to_pixels(spec->padding.left+spec->padding.right,effective);
 int32_t padding_height=nova_dlu_to_pixels(spec->padding.top+spec->padding.bottom,effective);
 int32_t width=constraint_axis(spec->width_mode,spec->width_dlu,
    spec->content_width_dlu,spec->preferred_width_dlu,spec->width_percent_milli,
    spec->stretch_width_weight,inner.width,effective)+padding_width;
 int32_t height=constraint_axis(spec->height_mode,spec->height_dlu,
    spec->content_height_dlu,spec->preferred_height_dlu,spec->height_percent_milli,
    spec->stretch_height_weight,inner.height,effective)+padding_height;
 int32_t min_width=nova_dlu_to_pixels(spec->min_width_dlu,effective),
    min_height=nova_dlu_to_pixels(spec->min_height_dlu,effective);
 int32_t max_width=spec->max_width_dlu?nova_dlu_to_pixels(spec->max_width_dlu,effective):inner.width;
 int32_t max_height=spec->max_height_dlu?nova_dlu_to_pixels(spec->max_height_dlu,effective):inner.height;
 if(spec->accessibility&&spec->interactive){int32_t touch=nova_dlu_to_pixels(44,effective);
    if(min_width<touch){min_width=touch;++constraint_diagnostics.accessibility_adjustments;}
    if(min_height<touch){min_height=touch;++constraint_diagnostics.accessibility_adjustments;}}
 if(max_width<min_width){max_width=min_width;++constraint_diagnostics.minimum_wins;
    ++constraint_diagnostics.violations;}
 if(max_height<min_height){max_height=min_height;++constraint_diagnostics.minimum_wins;
    ++constraint_diagnostics.violations;}
 if(width<min_width){width=min_width;++constraint_diagnostics.minimum_wins;
    ++constraint_diagnostics.violations;}
 if(height<min_height){height=min_height;++constraint_diagnostics.minimum_wins;
    ++constraint_diagnostics.violations;}
 if(width>max_width){width=max_width;++constraint_diagnostics.maximum_wins;
    ++constraint_diagnostics.violations;}
 if(height>max_height){height=max_height;++constraint_diagnostics.maximum_wins;
    ++constraint_diagnostics.violations;}
 if(spec->aspect_width&&spec->aspect_height){
    int32_t aspect_height=(int32_t)((int64_t)width*spec->aspect_height/spec->aspect_width);
    int32_t aspect_width=(int32_t)((int64_t)height*spec->aspect_width/spec->aspect_height);
    if(aspect_height>=min_height&&aspect_height<=max_height)height=aspect_height;
    else if(aspect_width>=min_width&&aspect_width<=max_width)width=aspect_width;
    else {++constraint_diagnostics.violations;++constraint_diagnostics.fallbacks;}
    ++constraint_diagnostics.aspect_adjustments;}
 *result=(nova_rect_t){inner.x,inner.y,width,height};
 ++constraint_diagnostics.resolutions;++constraint_diagnostics.invalidations;
 constraint_diagnostics.scale_milli=effective;constraint_diagnostics.last_available=available;
 constraint_diagnostics.last_result=*result;return true;
}
const nova_constraint_diagnostics_t *nova_constraint_diagnostics(void)
{return &constraint_diagnostics;}

bool nova_constraints_resolve(const nova_size_constraints_t *c,
                              uint32_t available_width, uint32_t available_height,
                              uint32_t *width, uint32_t *height)
{
    if (!c || !width || !height || c->min_width > c->max_width ||
        c->min_height > c->max_height) { ++diagnostics.constraint_errors; return false; }
    uint32_t w = c->preferred_width, h = c->preferred_height;
    if (w < c->min_width) w = c->min_width;
    if (h < c->min_height) h = c->min_height;
    if (w > c->max_width) w = c->max_width;
    if (h > c->max_height) h = c->max_height;
    if (w > available_width) w = available_width;
    if (h > available_height) h = available_height;
    if (w < c->min_width || h < c->min_height) {
        ++diagnostics.constraint_errors;
        return false;
    }
    *width = w; *height = h;
    return true;
}

bool nova_responsive_update(bool accessibility)
{
 const nova_viewport_t *viewport=nova_resolution_viewport();
 if(!viewport||viewport->physical_width<320||viewport->physical_height<240||
    viewport->logical_width_dlu<160||viewport->logical_height_dlu<120||
    viewport->scale_milli<1000||viewport->scale_milli>3000){
    responsive_state=(nova_responsive_state_t){.layout_class=NOVA_LAYOUT_STANDARD,
       .panel_flow=NOVA_RESPONSIVE_STACKED,.spacing_dlu=8,
       .max_control_width_dlu=480,.dialog_max_width_dlu=560};
    ++responsive_diagnostics.errors;++responsive_diagnostics.fallbacks;return false;}
 nova_layout_class_t previous=responsive_state.layout_class;
 nova_layout_class_t layout_class=viewport->logical_width_dlu<800?NOVA_LAYOUT_COMPACT:
    (viewport->logical_width_dlu<1280?NOVA_LAYOUT_STANDARD:NOVA_LAYOUT_WIDE);
 bool ultrawide=viewport->aspect_ratio==NOVA_ASPECT_21_9||
    viewport->aspect_ratio==NOVA_ASPECT_32_9||
    (uint64_t)viewport->physical_width*10u>(uint64_t)viewport->physical_height*22u;
 nova_responsive_flow_t flow=layout_class==NOVA_LAYOUT_COMPACT||
    viewport->logical_height_dlu<600||(accessibility&&viewport->logical_width_dlu<1000)?
    NOVA_RESPONSIVE_STACKED:NOVA_RESPONSIVE_SIDE_BY_SIDE;
 int32_t spacing=layout_class==NOVA_LAYOUT_COMPACT?6:
    (layout_class==NOVA_LAYOUT_STANDARD?8:12);
 if(accessibility)spacing=spacing*5/4;
 bool had_state=responsive_state.valid;
 if(had_state&&previous!=layout_class)++responsive_diagnostics.breakpoint_changes;
 if(had_state&&(responsive_state.physical_width!=viewport->physical_width||
    responsive_state.physical_height!=viewport->physical_height))
    ++responsive_diagnostics.resolution_changes;
 if(had_state&&responsive_state.dpi!=viewport->dpi)++responsive_diagnostics.dpi_changes;
 if(had_state&&responsive_state.scale_milli!=viewport->scale_milli)
    ++responsive_diagnostics.scale_changes;
 if(had_state&&responsive_state.accessibility!=accessibility)
    ++responsive_diagnostics.accessibility_changes;
 responsive_state=(nova_responsive_state_t){.physical_width=viewport->physical_width,
    .physical_height=viewport->physical_height,.dpi=viewport->dpi,
    .scale_milli=viewport->scale_milli,.generation=responsive_state.generation+1,
    .logical_width_dlu=viewport->logical_width_dlu,
    .logical_height_dlu=viewport->logical_height_dlu,.spacing_dlu=spacing,
    .max_control_width_dlu=layout_class==NOVA_LAYOUT_COMPACT?480:560,
    .dialog_max_width_dlu=layout_class==NOVA_LAYOUT_COMPACT?520:
       (layout_class==NOVA_LAYOUT_STANDARD?640:720),.layout_class=layout_class,
    .panel_flow=flow,.accessibility=accessibility,.ultrawide=ultrawide,.valid=true};
 ++responsive_diagnostics.updates;++responsive_diagnostics.invalidations;
 if(layout_class==NOVA_LAYOUT_COMPACT)++responsive_diagnostics.compact_count;
 else if(layout_class==NOVA_LAYOUT_STANDARD)++responsive_diagnostics.standard_count;
 else ++responsive_diagnostics.wide_count;
 if(flow==NOVA_RESPONSIVE_STACKED)++responsive_diagnostics.stacked_count;
 else ++responsive_diagnostics.side_by_side_count;
 nova_text_layout_begin(responsive_state.generation);
 responsive_diagnostics.last_generation=responsive_state.generation;return true;
}
nova_layout_class_t nova_responsive_class(void){return responsive_state.layout_class;}
bool nova_responsive_fit_dialog(nova_rect_t desired,nova_rect_t *result)
{
 if(!result||!responsive_state.valid||desired.width<0||desired.height<0){
    ++responsive_diagnostics.errors;return false;}
 const nova_viewport_t *viewport=nova_resolution_viewport();nova_rect_t safe=viewport->safe_pixels;
 int32_t margin=nova_dlu_to_pixels(responsive_state.spacing_dlu*2,
    responsive_state.scale_milli),max_width=safe.width-margin*2,
    max_height=safe.height-margin*2;
 int32_t token_max=nova_dlu_to_pixels(responsive_state.dialog_max_width_dlu,
    responsive_state.scale_milli);
 if(max_width>token_max)max_width=token_max;
 if(max_width<1||max_height<1){++responsive_diagnostics.errors;return false;}
 int32_t width=desired.width?desired.width:max_width,height=desired.height?desired.height:max_height;
 if(width>max_width)width=max_width;
 if(height>max_height)height=max_height;
 int32_t minimum_width=nova_dlu_to_pixels(responsive_state.accessibility?280:240,
    responsive_state.scale_milli),minimum_height=nova_dlu_to_pixels(
    responsive_state.accessibility?180:140,responsive_state.scale_milli);
 if(width<minimum_width&&minimum_width<=max_width)width=minimum_width;
 if(height<minimum_height&&minimum_height<=max_height)height=minimum_height;
 *result=(nova_rect_t){safe.x+(safe.width-width)/2,safe.y+(safe.height-height)/2,width,height};
 ++responsive_diagnostics.dialog_fits;return true;
}
const nova_responsive_state_t *nova_responsive_state(void){return &responsive_state;}
const nova_responsive_diagnostics_t *nova_responsive_diagnostics(void)
{return &responsive_diagnostics;}

bool nova_layout_compute(uint32_t width, uint32_t height, bool accessibility,
                         nova_boot_layout_t *out)
{
    if (!out || width < 320 || height < 240 || width > 7680 || height > 4320) {
        ++diagnostics.fallbacks;
        return false;
    }
    if(!nova_resolution_set_resolution(width,height)){++diagnostics.fallbacks;return false;}
    if(nova_safe_area_policy()->accessibility!=accessibility){
        nova_safe_area_policy_t policy=*nova_safe_area_policy();
        policy.accessibility=accessibility;
        if(!nova_safe_area_update(&policy)){++diagnostics.fallbacks;return false;}}
    const nova_viewport_t *viewport=nova_resolution_viewport();
    if(!nova_responsive_update(accessibility)){++diagnostics.fallbacks;return false;}
    ++diagnostics.invalidations; ++diagnostics.measures;
    uint32_t margin_x=(uint32_t)viewport->safe_pixels.x;
    uint32_t margin_y=(uint32_t)viewport->safe_pixels.y;
    nova_layout_class_t cls=responsive_state.layout_class;
    if(!nova_scaling_update(accessibility)){++diagnostics.fallbacks;return false;}
    uint32_t scale=nova_scaling_get()->effective_scale_milli;
    int32_t safe_w = (int32_t)width - (int32_t)margin_x * 2;
    int32_t safe_h = (int32_t)height - (int32_t)margin_y * 2;
    int32_t status_h = nova_dlu_to_pixels(22, scale);
    int32_t title_h = nova_dlu_to_pixels(24, scale);
    int32_t panel_y = (int32_t)margin_y + title_h +
                      nova_dlu_to_pixels(responsive_state.spacing_dlu,scale);
    int32_t available_h = (int32_t)height - (int32_t)margin_y - status_h - panel_y - 8;
    int32_t panel_w = nova_dlu_to_pixels(cls==NOVA_LAYOUT_WIDE?640:567, scale);
    int32_t panel_h = nova_dlu_to_pixels(473, scale);
    if (cls == NOVA_LAYOUT_COMPACT) panel_w = safe_w;
    panel_w = clamp_i32(panel_w, safe_w < 480 ? safe_w : 480, safe_w);
    panel_h = clamp_i32(panel_h, 330, available_h);
    int32_t side_brand = nova_dlu_to_pixels(150, scale);
    bool compact_brand = panel_w + side_brand + 20 > safe_w;
    int32_t panel_x = compact_brand ? (int32_t)margin_x :
                      (int32_t)margin_x + (safe_w - panel_w + side_brand) / 2;
    if (panel_x + panel_w > (int32_t)width - (int32_t)margin_x)
        panel_x = (int32_t)width - (int32_t)margin_x - panel_w;
    int32_t inner_pad = nova_dlu_to_pixels(cls==NOVA_LAYOUT_COMPACT?14:
                           (cls==NOVA_LAYOUT_STANDARD?20:24), scale);
    int32_t gap = nova_dlu_to_pixels(responsive_state.spacing_dlu, scale);
    int32_t list_h = panel_h - inner_pad * 2;
    int32_t item_h = (list_h - gap * 5) / 6;
    item_h = clamp_i32(item_h, accessibility ? 48 : 42,
                       nova_dlu_to_pixels(62, scale));
    list_h = item_h * 6 + gap * 5;
    int32_t brand_w = compact_brand ? 64 : nova_dlu_to_pixels(120, scale);
    int32_t brand_h = brand_w * 15 / 16;
    int32_t brand_x = compact_brand ? (int32_t)margin_x : panel_x - brand_w - 20;
    int32_t brand_y = compact_brand ? (int32_t)margin_y : panel_y - 13;
    if (compact_brand) panel_y = (int32_t)margin_y + brand_h + title_h + 8;
    if (panel_y + panel_h > (int32_t)height - (int32_t)margin_y - status_h - 4)
        panel_h = (int32_t)height - (int32_t)margin_y - status_h - 4 - panel_y;
    list_h = panel_h - inner_pad * 2;
    item_h = (list_h - gap * 5) / 6;
    item_h = clamp_i32(item_h, 36, nova_dlu_to_pixels(62, scale));
    list_h = item_h * 6 + gap * 5;
    int32_t title_x = compact_brand ? brand_x + brand_w + 12 : panel_x + inner_pad;
    int32_t title_y = compact_brand ? brand_y + (brand_h - title_h) / 2 : panel_y - title_h;
    int32_t title_w = compact_brand ?
                      (int32_t)width - (int32_t)margin_x - title_x : panel_w - inner_pad * 2;

    *out = (nova_boot_layout_t){
        {(int32_t)margin_x,(int32_t)margin_y,safe_w,safe_h},
        {panel_x,panel_y,panel_w,panel_h},
        {(int32_t)margin_x,0,safe_w,clamp_i32(nova_dlu_to_pixels(13,scale),8,20)},
        {title_x,title_y,title_w,title_h},
        {(int32_t)margin_x,(int32_t)height-(int32_t)margin_y-status_h,safe_w,status_h},
        {brand_x,brand_y,brand_w,brand_h},
        {panel_x+inner_pad,panel_y+inner_pad,panel_w-inner_pad*2,list_h},
        item_h,gap,clamp_i32(nova_dlu_to_pixels(20,scale),16,32),
        clamp_i32(nova_dlu_to_pixels(18,scale),12,32),
        clamp_i32(nova_dlu_to_pixels(15,scale),10,24),
        scale,viewport->dpi,cls,compact_brand,accessibility
    };
    ++diagnostics.arranges; ++diagnostics.finalizes;
    ++responsive_diagnostics.applies;
    diagnostics.width=width; diagnostics.height=height;
    diagnostics.scale_milli=scale; diagnostics.layout_class=cls;
    return out->panel.width > 0 && out->panel.height > 0 &&
           out->panel.x >= out->safe.x && out->panel.y >= out->safe.y &&
           out->panel.x + out->panel.width <= out->safe.x + out->safe.width &&
           out->status.y + out->status.height <= out->safe.y + out->safe.height;
}

const nova_layout_diagnostics_t *nova_layout_diagnostics(void) { return &diagnostics; }
