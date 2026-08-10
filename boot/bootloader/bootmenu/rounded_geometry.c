#include "rounded_geometry.h"

static nova_rounded_diagnostics_t diagnostics;

static uint16_t minimum(uint16_t a,uint16_t b){return a<b?a:b;}
static bool bounds_valid(nova_rect_t b)
{
    return b.width>0&&b.height>0&&b.x>=-32767&&b.y>=-32767&&
        b.x<=32767&&b.y<=32767&&(int64_t)b.x+b.width<=32767&&
        (int64_t)b.y+b.height<=32767;
}
static nova_corner_radius_t normalized(nova_rect_t b,nova_corner_radius_t c)
{
    uint16_t cap=(uint16_t)((b.width<b.height?b.width:b.height)/2);
    c.top_left=minimum(c.top_left,cap);c.top_right=minimum(c.top_right,cap);
    c.bottom_right=minimum(c.bottom_right,cap);c.bottom_left=minimum(c.bottom_left,cap);
    return c;
}
static void account(nova_corner_radius_t c)
{
    uint16_t values[4]={c.top_left,c.top_right,c.bottom_right,c.bottom_left};
    for(uint8_t i=0;i<4;++i){diagnostics.radius_sum+=values[i];
        if(values[i]>diagnostics.maximum_radius)diagnostics.maximum_radius=values[i];}
}
bool nova_round_rect_create_corners(nova_round_rect_t *rect,nova_rect_t bounds,
                                    nova_corner_radius_t corners)
{
    if(!rect||!bounds_valid(bounds)){++diagnostics.rejected;return false;}
    rect->bounds=bounds;rect->corners=normalized(bounds,corners);
    ++diagnostics.created;account(rect->corners);return true;
}
bool nova_round_rect_create(nova_round_rect_t *rect,int32_t x,int32_t y,
                            int32_t width,int32_t height,uint16_t radius)
{
    return nova_round_rect_create_corners(rect,(nova_rect_t){x,y,width,height},
        (nova_corner_radius_t){radius,radius,radius,radius});
}
bool nova_round_rect_set_corners(nova_round_rect_t *rect,nova_corner_radius_t corners)
{
    if(!rect||!bounds_valid(rect->bounds)){++diagnostics.rejected;return false;}
    rect->corners=normalized(rect->bounds,corners);account(rect->corners);return true;
}
bool nova_round_rect_set_radius(nova_round_rect_t *rect,uint16_t radius)
{return nova_round_rect_set_corners(rect,(nova_corner_radius_t){radius,radius,radius,radius});}
static bool scale_value(int32_t value,uint32_t scale,int32_t *result)
{
    int64_t v=(int64_t)value*scale;
    v=(v+(v>=0?32768:-32768))/65536;
    if(v<INT32_MIN||v>INT32_MAX)return false;
    *result=(int32_t)v;
    return true;
}
bool nova_round_rect_scale(const nova_round_rect_t *rect,uint32_t scale,nova_round_rect_t *out)
{
    if(!rect||!out||!scale||scale>0x00100000u)return false;
    nova_rect_t b;
    if(!scale_value(rect->bounds.x,scale,&b.x)||!scale_value(rect->bounds.y,scale,&b.y)||
       !scale_value(rect->bounds.width,scale,&b.width)||!scale_value(rect->bounds.height,scale,&b.height))return false;
    nova_corner_radius_t c={
        (uint16_t)(((uint64_t)rect->corners.top_left*scale+32768u)/65536u),
        (uint16_t)(((uint64_t)rect->corners.top_right*scale+32768u)/65536u),
        (uint16_t)(((uint64_t)rect->corners.bottom_right*scale+32768u)/65536u),
        (uint16_t)(((uint64_t)rect->corners.bottom_left*scale+32768u)/65536u)};
    return nova_round_rect_create_corners(out,b,c);
}
static int32_t lerp_i32(int32_t a,int32_t b,uint16_t p)
{return a+(int32_t)(((int64_t)(b-a)*p+500)/1000);}
bool nova_round_rect_lerp(const nova_round_rect_t *a,const nova_round_rect_t *b,
                          uint16_t p,nova_round_rect_t *out)
{
    if(!a||!b||!out||p>1000)return false;
    nova_rect_t r={lerp_i32(a->bounds.x,b->bounds.x,p),lerp_i32(a->bounds.y,b->bounds.y,p),
        lerp_i32(a->bounds.width,b->bounds.width,p),lerp_i32(a->bounds.height,b->bounds.height,p)};
    nova_corner_radius_t c={(uint16_t)lerp_i32(a->corners.top_left,b->corners.top_left,p),
        (uint16_t)lerp_i32(a->corners.top_right,b->corners.top_right,p),
        (uint16_t)lerp_i32(a->corners.bottom_right,b->corners.bottom_right,p),
        (uint16_t)lerp_i32(a->corners.bottom_left,b->corners.bottom_left,p)};
    return nova_round_rect_create_corners(out,r,c);
}
/* Coordinates use 1/256 pixel. Each corner is tested against its exact circle. */
static bool contains_subpixel(const nova_round_rect_t *rect,int64_t x,int64_t y)
{
    int64_t l=(int64_t)rect->bounds.x*256,t=(int64_t)rect->bounds.y*256;
    int64_t r=((int64_t)rect->bounds.x+rect->bounds.width)*256;
    int64_t b=((int64_t)rect->bounds.y+rect->bounds.height)*256;
    if(x<l||x>=r||y<t||y>=b)return false;
    uint16_t radius=0;int64_t cx=0,cy=0;
    if(x<l+(int64_t)rect->corners.top_left*256&&y<t+(int64_t)rect->corners.top_left*256)
        {radius=rect->corners.top_left;cx=l+(int64_t)radius*256;cy=t+(int64_t)radius*256;}
    else if(x>=r-(int64_t)rect->corners.top_right*256&&y<t+(int64_t)rect->corners.top_right*256)
        {radius=rect->corners.top_right;cx=r-(int64_t)radius*256;cy=t+(int64_t)radius*256;}
    else if(x>=r-(int64_t)rect->corners.bottom_right*256&&y>=b-(int64_t)rect->corners.bottom_right*256)
        {radius=rect->corners.bottom_right;cx=r-(int64_t)radius*256;cy=b-(int64_t)radius*256;}
    else if(x<l+(int64_t)rect->corners.bottom_left*256&&y>=b-(int64_t)rect->corners.bottom_left*256)
        {radius=rect->corners.bottom_left;cx=l+(int64_t)radius*256;cy=b-(int64_t)radius*256;}
    if(!radius)return true;
    int64_t dx=x-cx,dy=y-cy,rr=(int64_t)radius*256;
    return dx*dx+dy*dy<=rr*rr;
}
bool nova_round_rect_contains(const nova_round_rect_t *rect,int32_t x,int32_t y)
{
    ++diagnostics.hit_tests;
    return rect&&bounds_valid(rect->bounds)&&contains_subpixel(rect,(int64_t)x*256+128,(int64_t)y*256+128);
}
bool nova_round_rect_contains_8_8(const nova_round_rect_t *rect,int32_t x,int32_t y)
{return rect&&bounds_valid(rect->bounds)&&contains_subpixel(rect,x,y);}
uint8_t nova_round_rect_coverage(const nova_round_rect_t *rect,int32_t x,int32_t y,
                                 nova_rounded_aa_t quality)
{
    if(!rect||!bounds_valid(rect->bounds)||quality>NOVA_ROUNDED_AA_HIGH)return 0;
    if(quality==NOVA_ROUNDED_AA_DISABLED)
        return contains_subpixel(rect,(int64_t)x*256+128,(int64_t)y*256+128)?255:0;
    uint8_t grid=quality==NOVA_ROUNDED_AA_HIGH?4:2,hits=0;
    for(uint8_t sy=0;sy<grid;++sy)for(uint8_t sx=0;sx<grid;++sx)
        if(contains_subpixel(rect,(int64_t)x*256+(2*sx+1)*128/grid,
            (int64_t)y*256+(2*sy+1)*128/grid))++hits;
    diagnostics.anti_aliasing=true;
    return (uint8_t)(((uint16_t)hits*255u+(grid*grid)/2)/(grid*grid));
}
static void write_coverage(nova_surface_t *s,int32_t x,int32_t y,uint32_t color,uint8_t coverage)
{
    if(!coverage||x<0||y<0||(uint32_t)x>=s->width||(uint32_t)y>=s->height)return;
    uint32_t alpha=((color>>24)*coverage+127u)/255u;
    s->pixels[(uint32_t)y*s->stride+(uint32_t)x]=(color&0x00ffffffu)|(alpha<<24);
}
bool nova_round_rect_render(nova_surface_t *surface,const nova_round_rect_t *rect,
                            uint32_t color,nova_rounded_aa_t quality)
{
    if(!surface||!surface->pixels||surface->stride<surface->width||!rect||
       !bounds_valid(rect->bounds)||quality>NOVA_ROUNDED_AA_HIGH){++diagnostics.rejected;return false;}
    int32_t left=rect->bounds.x<0?0:rect->bounds.x,top=rect->bounds.y<0?0:rect->bounds.y;
    int64_t raw_right=(int64_t)rect->bounds.x+rect->bounds.width;
    int64_t raw_bottom=(int64_t)rect->bounds.y+rect->bounds.height;
    int32_t right=raw_right>(int64_t)surface->width?(int32_t)surface->width:(int32_t)raw_right;
    int32_t bottom=raw_bottom>(int64_t)surface->height?(int32_t)surface->height:(int32_t)raw_bottom;
    for(int32_t y=top;y<bottom;++y)for(int32_t x=left;x<right;++x)
        write_coverage(surface,x,y,color,nova_round_rect_coverage(rect,x,y,quality));
    nova_damage_add(surface,(nova_rect_t){left,top,right>left?right-left:0,bottom>top?bottom-top:0});
    ++diagnostics.rendered;return true;
}
static nova_round_rect_t inset_rect(const nova_round_rect_t *source,int32_t inset)
{
    nova_round_rect_t r=*source;r.bounds.x+=inset;r.bounds.y+=inset;
    r.bounds.width-=inset*2;r.bounds.height-=inset*2;
    if(inset>=0){uint16_t amount=(uint16_t)inset;
        r.corners.top_left=r.corners.top_left>amount?r.corners.top_left-amount:0;
        r.corners.top_right=r.corners.top_right>amount?r.corners.top_right-amount:0;
        r.corners.bottom_right=r.corners.bottom_right>amount?r.corners.bottom_right-amount:0;
        r.corners.bottom_left=r.corners.bottom_left>amount?r.corners.bottom_left-amount:0;
    }else{uint16_t amount=(uint16_t)(-inset);
        r.corners.top_left+=amount;r.corners.top_right+=amount;
        r.corners.bottom_right+=amount;r.corners.bottom_left+=amount;}
    return r;
}
bool nova_round_rect_render_border(nova_surface_t *surface,const nova_round_rect_t *rect,
    uint16_t thickness,nova_rounded_border_mode_t mode,uint32_t color,nova_rounded_aa_t quality)
{
    if(!surface||!rect||!thickness||mode>NOVA_ROUNDED_BORDER_OUTSIDE)return false;
    int32_t outer_delta=mode==NOVA_ROUNDED_BORDER_INSIDE?0:
        mode==NOVA_ROUNDED_BORDER_CENTERED?(thickness+1)/2:thickness;
    int32_t inner_delta=mode==NOVA_ROUNDED_BORDER_OUTSIDE?0:
        mode==NOVA_ROUNDED_BORDER_CENTERED?thickness/2:thickness;
    nova_round_rect_t outer=inset_rect(rect,-outer_delta),inner=inset_rect(rect,inner_delta);
    if(!bounds_valid(outer.bounds))return false;
    int32_t l=outer.bounds.x<0?0:outer.bounds.x,t=outer.bounds.y<0?0:outer.bounds.y;
    int32_t r=outer.bounds.x+outer.bounds.width,bt=outer.bounds.y+outer.bounds.height;
    if(r>(int32_t)surface->width)r=(int32_t)surface->width;
    if(bt>(int32_t)surface->height)bt=(int32_t)surface->height;
    for(int32_t y=t;y<bt;++y)for(int32_t x=l;x<r;++x){
        uint8_t oc=nova_round_rect_coverage(&outer,x,y,quality);
        uint8_t ic=bounds_valid(inner.bounds)?nova_round_rect_coverage(&inner,x,y,quality):0;
        write_coverage(surface,x,y,color,oc>ic?(uint8_t)(oc-ic):0);}
    nova_damage_add(surface,(nova_rect_t){l,t,r>l?r-l:0,bt>t?bt-t:0});++diagnostics.rendered;return true;
}
void nova_rounded_reset_diagnostics(void){diagnostics=(nova_rounded_diagnostics_t){0};}
const nova_rounded_diagnostics_t *nova_rounded_diagnostics(void){return &diagnostics;}
