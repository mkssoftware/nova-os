#include "render_queue.h"
#include "clip_mask.h"

static nova_render_command_t commands[NOVA_RENDER_COMMAND_CAPACITY];
static uint16_t command_count,executed_count;
static uint32_t next_id;
static nova_render_diagnostics_t diagnostics;

void nova_render_initialize(void)
{command_count=executed_count=0;next_id=1;diagnostics=(nova_render_diagnostics_t){0};}
bool nova_render_begin_frame(void)
{
    if(diagnostics.active)return false;
    if(!nova_clip_mask_begin_frame())return false;
    nova_transform_stack_reset();
    command_count=executed_count=0;diagnostics.active=true;++diagnostics.frames;return true;
}
static bool rect_valid(nova_rect_t r)
{return r.width>0&&r.height>0;}
static nova_rect_t intersect(nova_rect_t a,nova_rect_t b);
bool nova_render_push_clip(nova_rect_t clip)
{
    if(!diagnostics.active||!nova_clip_push(clip))return false;
    ++diagnostics.clip_pushes;return true;
}
bool nova_render_pop_clip(void)
{if(!diagnostics.active||!nova_clip_pop())return false;++diagnostics.clip_pops;return true;}
bool nova_render_push_transform(const nova_transform2d_t *transform)
{return diagnostics.active&&nova_transform_stack_push(transform);}
bool nova_render_pop_transform(void)
{return diagnostics.active&&nova_transform_stack_pop();}
static bool valid_surface(const nova_surface_t *surface)
{return surface&&surface->pixels&&surface->width&&surface->height&&surface->stride>=surface->width;}
static bool command_valid(const nova_render_command_t *command)
{
    if(!command||!valid_surface(command->surface)||command->type>NOVA_RENDER_ROUNDED_RECT||
       command->layer>NOVA_RENDER_CURSOR||command->opacity>1000)return false;
    if(command->id&& !nova_transform_fixed_valid(command->transform))return false;
    if(command->geometry.x<-32767||command->geometry.x>32767||
       command->geometry.y<-32767||command->geometry.y>32767||
       command->x2<-32767||command->x2>32767||
       command->y2<-32767||command->y2>32767)return false;
    nova_rect_t ignored;
    if(command->clip_id&&!nova_clip_scope_get(command->clip_id,&ignored))return false;
    if(command->type==NOVA_RENDER_RECT&&!rect_valid(command->geometry))return false;
    if(command->type==NOVA_RENDER_ROUNDED_RECT){
        nova_round_rect_t validated;
        if(!nova_round_rect_create_corners(&validated,command->geometry,command->corners)||
           validated.corners.top_left!=command->corners.top_left||
           validated.corners.top_right!=command->corners.top_right||
           validated.corners.bottom_right!=command->corners.bottom_right||
           validated.corners.bottom_left!=command->corners.bottom_left)return false;
    }
    if(command->type==NOVA_RENDER_CIRCLE&&(command->radius<=0||
       command->radius>16383||command->geometry.x-command->radius<-32767||
       command->geometry.x+command->radius>32767||
       command->geometry.y-command->radius<-32767||
       command->geometry.y+command->radius>32767))return false;
    if(command->type==NOVA_RENDER_RECT&&
       ((int64_t)command->geometry.x+command->geometry.width>32767||
        (int64_t)command->geometry.y+command->geometry.height>32767))return false;
    return true;
}
bool nova_render_submit(const nova_render_command_t *description)
{
    if(!diagnostics.active||!command_valid(description)){++diagnostics.rejected;return false;}
    if(command_count==NOVA_RENDER_COMMAND_CAPACITY){++diagnostics.overflows;return false;}
    nova_render_command_t command=*description;command.id=next_id++;command.creation=diagnostics.submitted;
    command.clip_id=nova_clip_current_scope();
    command.mask_scope=nova_mask_current_scope();
    nova_transform2d_t current_transform=nova_transform_stack_current();
    if(!nova_transform_to_fixed(&current_transform,&command.transform))
        return false;
    commands[command_count++]=command;++diagnostics.submitted;
    if(command_count>diagnostics.peak_commands)diagnostics.peak_commands=command_count;
    return true;
}
bool nova_render_clear(nova_surface_t *surface,uint32_t color,nova_render_layer_t layer)
{nova_render_command_t c={.type=NOVA_RENDER_CLEAR,.layer=layer,.opacity=1000,
    .surface=surface,.color=color};return nova_render_submit(&c);}
bool nova_render_rect(nova_surface_t *surface,nova_rect_t rect,uint32_t color,
    nova_render_layer_t layer,int16_t z)
{nova_render_command_t c={.type=NOVA_RENDER_RECT,.layer=layer,.z_index=z,.opacity=1000,
    .surface=surface,.geometry=rect,.color=color};return nova_render_submit(&c);}
bool nova_render_line(nova_surface_t *surface,int32_t x1,int32_t y1,int32_t x2,
    int32_t y2,uint32_t color,nova_render_layer_t layer,int16_t z)
{nova_render_command_t c={.type=NOVA_RENDER_LINE,.layer=layer,.z_index=z,.opacity=1000,
    .surface=surface,.geometry={x1,y1,1,1},.x2=x2,.y2=y2,.color=color};
    return nova_render_submit(&c);}
bool nova_render_circle(nova_surface_t *surface,int32_t cx,int32_t cy,int32_t radius,
    uint32_t color,nova_render_layer_t layer,int16_t z)
{nova_render_command_t c={.type=NOVA_RENDER_CIRCLE,.layer=layer,.z_index=z,.opacity=1000,
    .surface=surface,.geometry={cx,cy,1,1},.radius=radius,.color=color};
    return nova_render_submit(&c);}
bool nova_render_rounded_rect(nova_surface_t *surface,const nova_round_rect_t *rect,
    uint32_t color,nova_render_layer_t layer,int16_t z)
{
    if(!rect)return false;
    nova_render_command_t c={.type=NOVA_RENDER_ROUNDED_RECT,.layer=layer,
        .z_index=z,.opacity=1000,.surface=surface,.geometry=rect->bounds,
        .corners=rect->corners,.color=color};
    return nova_render_submit(&c);
}
bool nova_render_validate(void)
{
    if(!diagnostics.active)return false;
    for(uint16_t i=0;i<command_count;++i)if(!command_valid(&commands[i]))return false;
    return true;
}
static bool after(const nova_render_command_t *a,const nova_render_command_t *b)
{return a->layer>b->layer||(a->layer==b->layer&&(a->z_index>b->z_index||
    (a->z_index==b->z_index&&a->creation>b->creation)));}
static bool same_transform(nova_fixed_transform2d_t a,nova_fixed_transform2d_t b)
{return a.m11==b.m11&&a.m12==b.m12&&a.m21==b.m21&&a.m22==b.m22&&
    a.tx==b.tx&&a.ty==b.ty;}
bool nova_render_sort_optimize(void)
{
    if(!nova_render_validate())return false;
    for(uint16_t i=1;i<command_count;++i){nova_render_command_t value=commands[i];uint16_t j=i;
        while(j&&after(&commands[j-1],&value)){commands[j]=commands[j-1];--j;}
        commands[j]=value;}
    diagnostics.sorted+=command_count;
    for(uint16_t i=1;i<command_count;++i){nova_render_command_t *a=&commands[i-1],*b=&commands[i];
        if(a->type==NOVA_RENDER_RECT&&b->type==NOVA_RENDER_RECT&&a->surface==b->surface&&
           a->layer==b->layer&&a->z_index==b->z_index&&a->color==b->color&&
           a->clip_id==b->clip_id&&a->mask_scope==b->mask_scope&&
           same_transform(a->transform,b->transform)&&
           a->geometry.y==b->geometry.y&&
           a->geometry.height==b->geometry.height&&a->geometry.x+a->geometry.width==b->geometry.x){
            a->geometry.width+=b->geometry.width;
            for(uint16_t move=i;move+1<command_count;++move)commands[move]=commands[move+1];
            --command_count;--i;++diagnostics.batches;}}
    return true;
}
static nova_rect_t intersect(nova_rect_t a,nova_rect_t b)
{
    int32_t x=a.x>b.x?a.x:b.x,y=a.y>b.y?a.y:b.y;
    int32_t ar=a.x+a.width,br=b.x+b.width,ab=a.y+a.height,bb=b.y+b.height;
    int32_t r=ar<br?ar:br,bot=ab<bb?ab:bb;
    return (nova_rect_t){x,y,r>x?r-x:0,bot>y?bot-y:0};
}
static void pixel(nova_surface_t *surface,int32_t x,int32_t y,uint32_t color,
                  nova_rect_t clip,uint16_t mask_scope)
{
    if(x<clip.x||y<clip.y||x>=clip.x+clip.width||y>=clip.y+clip.height||
       x<0||y<0||(uint32_t)x>=surface->width||(uint32_t)y>=surface->height)return;
    uint8_t coverage=nova_mask_coverage(mask_scope,x,y);
    if(!coverage)return;
    uint32_t alpha=((color>>24)*coverage+127u)/255u;
    surface->pixels[y*surface->stride+x]=(color&0x00ffffffu)|(alpha<<24);
}
static bool transformed_point(nova_fixed_transform2d_t transform,int32_t x,int32_t y,
                              int32_t *result_x,int32_t *result_y)
{
    int64_t fixed_x=(int64_t)x*NOVA_TRANSFORM_FIXED_ONE;
    int64_t fixed_y=(int64_t)y*NOVA_TRANSFORM_FIXED_ONE;
    if(fixed_x>INT32_MAX||fixed_x<INT32_MIN||fixed_y>INT32_MAX||fixed_y<INT32_MIN)
        return false;
    nova_fixed_point2d_t result;
    if(!nova_transform_fixed_point(transform,
        (nova_fixed_point2d_t){(int32_t)fixed_x,(int32_t)fixed_y},&result))return false;
    *result_x=(result.x+(result.x>=0?NOVA_TRANSFORM_FIXED_ONE/2:
        -NOVA_TRANSFORM_FIXED_ONE/2))/NOVA_TRANSFORM_FIXED_ONE;
    *result_y=(result.y+(result.y>=0?NOVA_TRANSFORM_FIXED_ONE/2:
        -NOVA_TRANSFORM_FIXED_ONE/2))/NOVA_TRANSFORM_FIXED_ONE;
    return true;
}
static void execute_line(const nova_render_command_t *c,nova_rect_t clip)
{
    int32_t x,y,x2,y2;
    if(!transformed_point(c->transform,c->geometry.x,c->geometry.y,&x,&y)||
       !transformed_point(c->transform,c->x2,c->y2,&x2,&y2))return;
    int32_t start_x=x,start_y=y,dx=x2>x?x2-x:x-x2;
    int32_t sx=x<x2?1:-1,dy=-(y2>y?y2-y:y-y2),sy=y<y2?1:-1,err=dx+dy;
    for(;;){pixel(c->surface,x,y,c->color,clip,c->mask_scope);if(x==x2&&y==y2)break;
        int32_t twice=err*2;if(twice>=dy){err+=dy;x+=sx;}if(twice<=dx){err+=dx;y+=sy;}}
    nova_damage_add(c->surface,intersect((nova_rect_t){start_x<x2?start_x:x2,
        start_y<y2?start_y:y2,dx+1,-dy+1},clip));
}
static void execute_inverse_shape(const nova_render_command_t *c,nova_rect_t clip,
                                  nova_rect_t local_bounds,bool circle)
{
    nova_rect_t transformed;
    nova_fixed_transform2d_t inverse;
    if(!nova_transform_fixed_bounds(c->transform,local_bounds,&transformed)||
       !nova_transform_fixed_inverse(&inverse,c->transform))return;
    nova_rect_t target=intersect(transformed,clip);
    int64_t left=(int64_t)local_bounds.x*NOVA_TRANSFORM_FIXED_ONE;
    int64_t top=(int64_t)local_bounds.y*NOVA_TRANSFORM_FIXED_ONE;
    int64_t right=((int64_t)local_bounds.x+local_bounds.width)*NOVA_TRANSFORM_FIXED_ONE;
    int64_t bottom=((int64_t)local_bounds.y+local_bounds.height)*NOVA_TRANSFORM_FIXED_ONE;
    int64_t center_x=(int64_t)c->geometry.x*NOVA_TRANSFORM_FIXED_ONE;
    int64_t center_y=(int64_t)c->geometry.y*NOVA_TRANSFORM_FIXED_ONE;
    int64_t radius=(int64_t)c->radius*NOVA_TRANSFORM_FIXED_ONE;
    for(int32_t y=target.y;y<target.y+target.height;++y)
        for(int32_t x=target.x;x<target.x+target.width;++x){
            int64_t sample_x=(int64_t)x*NOVA_TRANSFORM_FIXED_ONE+
                NOVA_TRANSFORM_FIXED_ONE/2;
            int64_t sample_y=(int64_t)y*NOVA_TRANSFORM_FIXED_ONE+
                NOVA_TRANSFORM_FIXED_ONE/2;
            if(sample_x>INT32_MAX||sample_x<INT32_MIN||
               sample_y>INT32_MAX||sample_y<INT32_MIN)continue;
            nova_fixed_point2d_t local;
            if(!nova_transform_fixed_point(inverse,(nova_fixed_point2d_t){
                (int32_t)sample_x,(int32_t)sample_y},&local))continue;
            bool inside=local.x>=left&&local.x<right&&local.y>=top&&local.y<bottom;
            if(circle){
                int64_t dx=(int64_t)local.x-center_x,dy=(int64_t)local.y-center_y;
                int64_t adx=dx<0?-dx:dx,ady=dy<0?-dy:dy;
                inside=adx<=INT32_MAX&&ady<=INT32_MAX&&
                    dx*dx+dy*dy<=radius*radius;
            }
            if(inside)pixel(c->surface,x,y,c->color,clip,c->mask_scope);
        }
    nova_damage_add(c->surface,target);
}
static void execute_rounded(const nova_render_command_t *c,nova_rect_t clip)
{
    nova_rect_t transformed,target;
    nova_fixed_transform2d_t inverse;
    nova_round_rect_t shape={c->geometry,c->corners};
    if(!nova_transform_fixed_bounds(c->transform,c->geometry,&transformed)||
       !nova_transform_fixed_inverse(&inverse,c->transform))return;
    target=intersect(transformed,clip);
    static const int32_t offsets[2]={64,192};
    for(int32_t y=target.y;y<target.y+target.height;++y)
        for(int32_t x=target.x;x<target.x+target.width;++x){uint8_t hits=0;
            for(uint8_t sy=0;sy<2;++sy)for(uint8_t sx=0;sx<2;++sx){
                int64_t fx=(int64_t)x*NOVA_TRANSFORM_FIXED_ONE+offsets[sx]*256;
                int64_t fy=(int64_t)y*NOVA_TRANSFORM_FIXED_ONE+offsets[sy]*256;
                if(fx>INT32_MAX||fy>INT32_MAX)continue;
                nova_fixed_point2d_t local;
                if(nova_transform_fixed_point(inverse,(nova_fixed_point2d_t){(int32_t)fx,(int32_t)fy},&local)&&
                   nova_round_rect_contains_8_8(&shape,local.x/256,local.y/256))++hits;
            }
            if(hits){uint32_t alpha=((c->color>>24)*hits+2)/4;
                pixel(c->surface,x,y,(c->color&0x00ffffffu)|(alpha<<24),clip,c->mask_scope);}
        }
    nova_damage_add(c->surface,target);
}
bool nova_render_execute(void)
{
    if(!diagnostics.active||executed_count||!nova_render_sort_optimize())return false;
    for(uint16_t i=0;i<command_count;++i){nova_render_command_t *c=&commands[i];
        nova_rect_t clip={0},local_bounds=c->geometry,bounds={0};
        if(!nova_clip_scope_get(c->clip_id,&clip))return false;
        clip=intersect(clip,(nova_rect_t){0,0,(int32_t)c->surface->width,
                                         (int32_t)c->surface->height});
        if(c->type==NOVA_RENDER_CLEAR)
            local_bounds=(nova_rect_t){0,0,(int32_t)c->surface->width,
                                       (int32_t)c->surface->height};
        else if(c->type==NOVA_RENDER_LINE)
            local_bounds=(nova_rect_t){c->geometry.x<c->x2?c->geometry.x:c->x2,
                c->geometry.y<c->y2?c->geometry.y:c->y2,
                (c->geometry.x<c->x2?c->x2-c->geometry.x:c->geometry.x-c->x2)+1,
                (c->geometry.y<c->y2?c->y2-c->geometry.y:c->geometry.y-c->y2)+1};
        else if(c->type==NOVA_RENDER_CIRCLE)
            local_bounds=(nova_rect_t){c->geometry.x-c->radius,
                c->geometry.y-c->radius,c->radius*2+1,c->radius*2+1};
        if(!nova_transform_fixed_bounds(c->transform,local_bounds,&bounds))return false;
        nova_rect_t visible;
        if(nova_clip_test(c->clip_id,bounds,&visible)==NOVA_CLIP_OUTSIDE){
            ++executed_count;++diagnostics.executed;continue;
        }
        nova_fixed_transform2d_t identity=nova_transform_fixed_identity();
        if(c->type==NOVA_RENDER_CLEAR&&c->mask_scope==0&&
           same_transform(c->transform,identity))
            nova_surface_rect(c->surface,intersect(bounds,clip),c->color);
        else if(c->type==NOVA_RENDER_CLEAR||c->type==NOVA_RENDER_RECT){
            execute_inverse_shape(c,clip,local_bounds,false);
        }
        else if(c->type==NOVA_RENDER_LINE)execute_line(c,clip);
        else if(c->type==NOVA_RENDER_CIRCLE)execute_inverse_shape(c,clip,local_bounds,true);
        else execute_rounded(c,clip);
        ++executed_count;++diagnostics.executed;}
    return true;
}
bool nova_render_end_frame(void)
{
    if(!diagnostics.active||executed_count!=command_count||
       nova_transform_stack_depth()!=1||
       !nova_clip_mask_end_frame())return false;
    diagnostics.active=false;return true;
}
void nova_render_abort_frame(void)
{
    nova_clip_mask_abort_frame();nova_transform_stack_reset();
    command_count=executed_count=0;diagnostics.active=false;
}
uint16_t nova_render_command_count(void){return command_count;}
const nova_render_command_t *nova_render_command(uint16_t index)
{return index<command_count?&commands[index]:0;}
const nova_render_diagnostics_t *nova_render_diagnostics(void){return &diagnostics;}
