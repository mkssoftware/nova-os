#include "render_queue.h"

static nova_render_command_t commands[NOVA_RENDER_COMMAND_CAPACITY];
static nova_rect_t clips[NOVA_RENDER_CLIP_CAPACITY];
static uint16_t clip_stack[NOVA_RENDER_CLIP_CAPACITY];
static uint16_t command_count,clip_depth,clip_used,executed_count;
static uint32_t next_id;
static nova_render_diagnostics_t diagnostics;

void nova_render_initialize(void)
{command_count=clip_depth=clip_used=executed_count=0;next_id=1;diagnostics=(nova_render_diagnostics_t){0};}
bool nova_render_begin_frame(void)
{
    if(diagnostics.active)return false;
    command_count=clip_depth=clip_used=executed_count=0;diagnostics.active=true;++diagnostics.frames;return true;
}
static bool rect_valid(nova_rect_t r)
{return r.width>0&&r.height>0;}
static nova_rect_t intersect(nova_rect_t a,nova_rect_t b);
bool nova_render_push_clip(nova_rect_t clip)
{
    if(!diagnostics.active||!rect_valid(clip)||clip_depth==NOVA_RENDER_CLIP_CAPACITY||
       clip_used==NOVA_RENDER_CLIP_CAPACITY)return false;
    if(clip_depth)clip=intersect(clip,clips[clip_stack[clip_depth-1]]);
    if(!rect_valid(clip))return false;
    clips[clip_used]=clip;clip_stack[clip_depth++]=clip_used++;++diagnostics.clip_pushes;return true;
}
bool nova_render_pop_clip(void)
{if(!diagnostics.active||!clip_depth)return false;--clip_depth;++diagnostics.clip_pops;return true;}
static bool valid_surface(const nova_surface_t *surface)
{return surface&&surface->pixels&&surface->width&&surface->height&&surface->stride>=surface->width;}
static bool command_valid(const nova_render_command_t *command)
{
    if(!command||!valid_surface(command->surface)||command->type>NOVA_RENDER_CIRCLE||
       command->layer>NOVA_RENDER_CURSOR||command->opacity>1000||
       command->clip_id>clip_used)return false;
    if(command->type==NOVA_RENDER_RECT&&!rect_valid(command->geometry))return false;
    if(command->type==NOVA_RENDER_CIRCLE&&command->radius<=0)return false;
    return true;
}
bool nova_render_submit(const nova_render_command_t *description)
{
    if(!diagnostics.active||!command_valid(description)){++diagnostics.rejected;return false;}
    if(command_count==NOVA_RENDER_COMMAND_CAPACITY){++diagnostics.overflows;return false;}
    nova_render_command_t command=*description;command.id=next_id++;command.creation=diagnostics.submitted;
    command.clip_id=clip_depth?(uint16_t)(clip_stack[clip_depth-1]+1u):0;
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
bool nova_render_validate(void)
{
    if(!diagnostics.active)return false;
    for(uint16_t i=0;i<command_count;++i)if(!command_valid(&commands[i]))return false;
    return true;
}
static bool after(const nova_render_command_t *a,const nova_render_command_t *b)
{return a->layer>b->layer||(a->layer==b->layer&&(a->z_index>b->z_index||
    (a->z_index==b->z_index&&a->creation>b->creation)));}
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
           a->clip_id==b->clip_id&&a->geometry.y==b->geometry.y&&
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
static void pixel(nova_surface_t *surface,int32_t x,int32_t y,uint32_t color,nova_rect_t clip)
{
    if(x<clip.x||y<clip.y||x>=clip.x+clip.width||y>=clip.y+clip.height||
       x<0||y<0||(uint32_t)x>=surface->width||(uint32_t)y>=surface->height)return;
    surface->pixels[y*surface->stride+x]=color;
}
static void execute_line(const nova_render_command_t *c,nova_rect_t clip)
{
    int32_t x=c->geometry.x,y=c->geometry.y,dx=c->x2>x?c->x2-x:x-c->x2;
    int32_t sx=x<c->x2?1:-1,dy=-(c->y2>y?c->y2-y:y-c->y2),sy=y<c->y2?1:-1,err=dx+dy;
    for(;;){pixel(c->surface,x,y,c->color,clip);if(x==c->x2&&y==c->y2)break;
        int32_t twice=err*2;if(twice>=dy){err+=dy;x+=sx;}if(twice<=dx){err+=dx;y+=sy;}}
    nova_damage_add(c->surface,intersect((nova_rect_t){c->geometry.x<c->x2?c->geometry.x:c->x2,
        c->geometry.y<c->y2?c->geometry.y:c->y2,dx+1,-dy+1},clip));
}
static void execute_circle(const nova_render_command_t *c,nova_rect_t clip)
{
    for(int32_t y=-c->radius;y<=c->radius;++y)for(int32_t x=-c->radius;x<=c->radius;++x)
        if(x*x+y*y<=c->radius*c->radius)pixel(c->surface,c->geometry.x+x,c->geometry.y+y,c->color,clip);
    nova_damage_add(c->surface,intersect((nova_rect_t){c->geometry.x-c->radius,
        c->geometry.y-c->radius,c->radius*2+1,c->radius*2+1},clip));
}
bool nova_render_execute(void)
{
    if(!diagnostics.active||executed_count||!nova_render_sort_optimize())return false;
    for(uint16_t i=0;i<command_count;++i){nova_render_command_t *c=&commands[i];
        nova_rect_t clip={0,0,(int32_t)c->surface->width,(int32_t)c->surface->height};
        if(c->clip_id)clip=intersect(clip,clips[c->clip_id-1]);
        if(c->type==NOVA_RENDER_CLEAR)nova_surface_clear(c->surface,c->color);
        else if(c->type==NOVA_RENDER_RECT)nova_surface_rect(c->surface,intersect(c->geometry,clip),c->color);
        else if(c->type==NOVA_RENDER_LINE)execute_line(c,clip);
        else execute_circle(c,clip);
        ++executed_count;++diagnostics.executed;}
    return true;
}
bool nova_render_end_frame(void)
{
    if(!diagnostics.active||clip_depth||executed_count!=command_count)return false;
    diagnostics.active=false;return true;
}
uint16_t nova_render_command_count(void){return command_count;}
const nova_render_command_t *nova_render_command(uint16_t index)
{return index<command_count?&commands[index]:0;}
const nova_render_diagnostics_t *nova_render_diagnostics(void){return &diagnostics;}
