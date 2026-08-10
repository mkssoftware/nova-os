#include "effects.h"
#include "clip_mask.h"

static nova_effect_diagnostics_t diagnostics;
static uint8_t effect_limit=NOVA_EFFECT_CAPACITY;

static bool finite_float(float value)
{union {float f;uint32_t u;} bits={value};return (bits.u&0x7f800000u)!=0x7f800000u;}
static bool effect_valid(const nova_effect_t *effect,bool glow)
{
    if(!effect||effect->type>NOVA_EFFECT_ACCENT_GLOW||!finite_float(effect->radius)||
       !finite_float(effect->opacity)||!finite_float(effect->offset_x)||
       !finite_float(effect->offset_y)||effect->radius<0.0f||effect->radius>64.0f||
       effect->opacity<0.0f||effect->opacity>1.0f||effect->offset_x<-128.0f||
       effect->offset_x>128.0f||effect->offset_y<-128.0f||effect->offset_y>128.0f)return false;
    bool is_glow=effect->type>=NOVA_EFFECT_OUTER_GLOW;
    return is_glow==glow;
}
void nova_effect_initialize(nova_effect_quality_t quality)
{diagnostics=(nova_effect_diagnostics_t){0};effect_limit=NOVA_EFFECT_CAPACITY;
 nova_effect_set_quality(quality);}
void nova_effect_set_quality(nova_effect_quality_t quality)
{if(quality<=NOVA_EFFECT_QUALITY_HIGH)diagnostics.quality=quality;}
void nova_effect_set_density(uint8_t maximum_effects)
{effect_limit=maximum_effects<=NOVA_EFFECT_CAPACITY?maximum_effects:NOVA_EFFECT_CAPACITY;}
void nova_effect_begin_frame(void)
{diagnostics.effects_this_frame=0;++diagnostics.frames;}
bool nova_effect_object_create(nova_effect_object_t *object,const nova_round_rect_t *geometry)
{
    if(!object||!geometry){++diagnostics.rejected;return false;}
    *object=(nova_effect_object_t){.geometry=*geometry};++diagnostics.objects;return true;
}
static bool effect_add(nova_effect_object_t *object,const nova_effect_t *effect,bool glow)
{
    if(!object||!effect_valid(effect,glow)||object->effect_count>=effect_limit){
        ++diagnostics.rejected;return false;}
    object->effects[object->effect_count++]=*effect;++diagnostics.effects_set;return true;
}
bool nova_shadow_set(nova_effect_object_t *object,const nova_effect_t *effect)
{return effect_add(object,effect,false);}
bool nova_glow_set(nova_effect_object_t *object,const nova_effect_t *effect)
{return effect_add(object,effect,true);}
void nova_effect_clear(nova_effect_object_t *object)
{if(object){object->effect_count=0;++diagnostics.effects_cleared;}}
static float lerp_float(float a,float b,uint16_t p)
{return a+(b-a)*(float)p/1000.0f;}
static uint8_t channel(uint32_t color,uint8_t shift){return (uint8_t)(color>>shift);}
bool nova_effect_lerp(const nova_effect_t *a,const nova_effect_t *b,uint16_t p,
                      nova_effect_t *result)
{
    if(!a||!b||!result||p>1000||a->type!=b->type||
       !effect_valid(a,a->type>=NOVA_EFFECT_OUTER_GLOW)||
       !effect_valid(b,b->type>=NOVA_EFFECT_OUTER_GLOW))return false;
    result->type=a->type;result->radius=lerp_float(a->radius,b->radius,p);
    result->opacity=lerp_float(a->opacity,b->opacity,p);
    result->offset_x=lerp_float(a->offset_x,b->offset_x,p);
    result->offset_y=lerp_float(a->offset_y,b->offset_y,p);
    uint32_t c=0;for(uint8_t shift=0;shift<32;shift+=8)
        c|=(uint32_t)((channel(a->color,shift)*(1000-p)+channel(b->color,shift)*p+500)/1000)<<shift;
    result->color=c;return true;
}
static nova_round_rect_t changed(const nova_round_rect_t *source,int32_t offset_x,
                                 int32_t offset_y,int32_t amount)
{
    nova_round_rect_t result=*source;
    result.bounds.x+=offset_x-amount;result.bounds.y+=offset_y-amount;
    result.bounds.width+=amount*2;result.bounds.height+=amount*2;
    uint16_t add=amount>0?(uint16_t)amount:0;
    uint16_t sub=amount<0?(uint16_t)(-amount):0;
#define CHANGE_RADIUS(v) (uint16_t)(amount>=0?(v)+add:(v)>sub?(v)-sub:0)
    result.corners=(nova_corner_radius_t){CHANGE_RADIUS(source->corners.top_left),
        CHANGE_RADIUS(source->corners.top_right),CHANGE_RADIUS(source->corners.bottom_right),
        CHANGE_RADIUS(source->corners.bottom_left)};
#undef CHANGE_RADIUS
    return result;
}
static bool inside(const nova_round_rect_t *shape,int32_t x,int32_t y)
{return shape->bounds.width>0&&shape->bounds.height>0&&nova_round_rect_contains(shape,x,y);}
static uint8_t effect_coverage(const nova_effect_object_t *object,const nova_effect_t *effect,
                               int32_t x,int32_t y,uint16_t radius)
{
    int32_t ox=(int32_t)(effect->offset_x+(effect->offset_x>=0?0.5f:-0.5f));
    int32_t oy=(int32_t)(effect->offset_y+(effect->offset_y>=0?0.5f:-0.5f));
    bool inner=effect->type==NOVA_EFFECT_INNER_SHADOW||effect->type==NOVA_EFFECT_INNER_GLOW;
    nova_round_rect_t shifted=changed(&object->geometry,ox,oy,0);
    if(inner){
        if(!inside(&object->geometry,x,y))return 0;
        for(uint16_t distance=1;distance<=radius+1;++distance){
            nova_round_rect_t inset=changed(&shifted,0,0,-(int32_t)distance);
            if(!inside(&inset,x,y))return (uint8_t)(((uint32_t)(radius+2-distance)*255)/(radius+1));
        }
        return 0;
    }
    /* Outer effects are covered by the later fill inside the object. Skipping
       those pixels is equivalent visually and keeps the firmware responsive. */
    if(inside(&shifted,x,y)||!radius)return 0;
    nova_round_rect_t maximum=changed(&shifted,0,0,radius);
    if(!inside(&maximum,x,y))return 0;
    uint16_t low=1,high=radius;
    while(low<high){uint16_t middle=(uint16_t)((low+high)/2);
        nova_round_rect_t expanded=changed(&shifted,0,0,middle);
        if(inside(&expanded,x,y))high=middle;else low=(uint16_t)(middle+1);}
    return (uint8_t)(((uint32_t)(radius+1-low)*255)/(radius+1));
}
static void effect_pixel(nova_surface_t *surface,int32_t x,int32_t y,uint32_t color,
                         uint8_t coverage,uint8_t mask)
{
    uint32_t alpha=(color>>24);alpha=(alpha*coverage+127)/255;
    alpha=(alpha*mask+127)/255;if(!alpha)return;
    surface->pixels[(uint32_t)y*surface->stride+(uint32_t)x]=(color&0x00ffffffu)|(alpha<<24);
    ++diagnostics.rendered_pixels;
}
bool nova_effect_render(const nova_effect_object_t *object,nova_surface_t *surface,
                        nova_effect_phase_t phase,nova_rect_t clip,uint16_t mask_scope)
{
    if(!object||!surface||!surface->pixels||surface->stride<surface->width||
       phase>NOVA_EFFECT_PHASE_GLOW||clip.width<=0||clip.height<=0){++diagnostics.rejected;return false;}
    for(uint8_t i=0;i<object->effect_count;++i){const nova_effect_t *effect=&object->effects[i];
        bool glow=effect->type>=NOVA_EFFECT_OUTER_GLOW;if(glow!=(phase==NOVA_EFFECT_PHASE_GLOW))continue;
        uint16_t radius=(uint16_t)(effect->radius+0.5f);
        if(diagnostics.quality==NOVA_EFFECT_QUALITY_LOW&&radius>4)radius=4;
        else if(diagnostics.quality==NOVA_EFFECT_QUALITY_STANDARD&&radius>12)radius=12;
        if(radius>diagnostics.maximum_radius)diagnostics.maximum_radius=radius;
        ++diagnostics.cache_misses;
        int32_t ox=(int32_t)(effect->offset_x+(effect->offset_x>=0?0.5f:-0.5f));
        int32_t oy=(int32_t)(effect->offset_y+(effect->offset_y>=0?0.5f:-0.5f));
        int32_t extent=(int32_t)radius+1,l=object->geometry.bounds.x+ox-extent;
        int32_t t=object->geometry.bounds.y+oy-extent;
        int32_t r=object->geometry.bounds.x+ox+object->geometry.bounds.width+extent;
        int32_t b=object->geometry.bounds.y+oy+object->geometry.bounds.height+extent;
        if(l<clip.x)l=clip.x;
        if(t<clip.y)t=clip.y;
        if(r>clip.x+clip.width)r=clip.x+clip.width;
        if(b>clip.y+clip.height)b=clip.y+clip.height;
        if(l<0)l=0;
        if(t<0)t=0;
        if(r>(int32_t)surface->width)r=(int32_t)surface->width;
        if(b>(int32_t)surface->height)b=(int32_t)surface->height;
        uint32_t opacity=(uint32_t)(effect->opacity*1000.0f+0.5f);
        uint32_t alpha=(effect->color>>24)*opacity/1000u;
        uint32_t color=(effect->color&0x00ffffffu)|(alpha<<24);
        bool inner=effect->type==NOVA_EFFECT_INNER_SHADOW||
                   effect->type==NOVA_EFFECT_INNER_GLOW;
        uint16_t corner=object->geometry.corners.top_left;
        if(object->geometry.corners.top_right>corner)corner=object->geometry.corners.top_right;
        if(object->geometry.corners.bottom_right>corner)corner=object->geometry.corners.bottom_right;
        if(object->geometry.corners.bottom_left>corner)corner=object->geometry.corners.bottom_left;
        int32_t object_right=object->geometry.bounds.x+object->geometry.bounds.width;
        int32_t object_bottom=object->geometry.bounds.y+object->geometry.bounds.height;
        for(int32_t y=t;y<b;++y)for(int32_t x=l;x<r;++x){
            if(!inner&&y>=object->geometry.bounds.y&&y<object_bottom){
                bool middle_y=y>=object->geometry.bounds.y+corner&&
                    y<object_bottom-corner;
                int32_t safe_left=object->geometry.bounds.x+(middle_y?0:corner);
                int32_t safe_right=object_right-(middle_y?0:corner);
                if(x>=safe_left&&x<safe_right)continue;
            }
            uint8_t coverage=effect_coverage(object,effect,x,y,radius);
            if(coverage)effect_pixel(surface,x,y,color,coverage,nova_mask_coverage(mask_scope,x,y));}
        nova_damage_add(surface,(nova_rect_t){l,t,r>l?r-l:0,b>t?b-t:0});
        if(glow)++diagnostics.glows;else ++diagnostics.shadows;
        ++diagnostics.effects_this_frame;
    }
    return true;
}
const nova_effect_diagnostics_t *nova_effect_diagnostics(void){return &diagnostics;}
