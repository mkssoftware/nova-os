#include "vector_geometry.h"
static bool add(nova_vector_path_t *p,nova_vector_point_t a,nova_vector_point_t b)
{if(!p||p->count==NOVA_VECTOR_SEGMENT_CAPACITY){if(p)p->overflow=true;return false;}
 p->segments[p->count++]=(nova_vector_segment_t){a,b};return true;}
void nova_vector_path_reset(nova_vector_path_t *p)
{if(!p)return;volatile uint8_t *bytes=(volatile uint8_t *)p;
 for(uint32_t i=0;i<sizeof(*p);++i)bytes[i]=0;}
bool nova_vector_move_to(nova_vector_path_t *p,int32_t x,int32_t y)
{if(!p)return false;p->current=p->first=(nova_vector_point_t){x,y};p->open=true;return true;}
bool nova_vector_line_to(nova_vector_path_t *p,int32_t x,int32_t y)
{if(!p||!p->open)return false;nova_vector_point_t q={x,y};if(!add(p,p->current,q))return false;
 p->current=q;return true;}
static int32_t lerp(int32_t a,int32_t b,uint32_t t)
{return (int32_t)(((int64_t)a*(65536u-t)+(int64_t)b*t+32768)>>16);}
bool nova_vector_quad_to(nova_vector_path_t *p,int32_t cx,int32_t cy,int32_t x,int32_t y)
{if(!p||!p->open)return false;nova_vector_point_t start=p->current,control={cx,cy},end={x,y};
 for(uint32_t i=1;i<=16;++i){uint32_t t=i*4096u;nova_vector_point_t a={lerp(start.x,control.x,t),lerp(start.y,control.y,t)};
  nova_vector_point_t b={lerp(control.x,end.x,t),lerp(control.y,end.y,t)};
  if(!nova_vector_line_to(p,lerp(a.x,b.x,t),lerp(a.y,b.y,t)))return false;}return true;}
bool nova_vector_cubic_to(nova_vector_path_t *p,int32_t x1,int32_t y1,int32_t x2,int32_t y2,int32_t x,int32_t y)
{if(!p||!p->open)return false;nova_vector_point_t a=p->current,b={x1,y1},c={x2,y2},d={x,y};
 for(uint32_t i=1;i<=24;++i){uint32_t t=(i*65536u)/24u;
  nova_vector_point_t ab={lerp(a.x,b.x,t),lerp(a.y,b.y,t)},bc={lerp(b.x,c.x,t),lerp(b.y,c.y,t)},cd={lerp(c.x,d.x,t),lerp(c.y,d.y,t)};
  nova_vector_point_t q1={lerp(ab.x,bc.x,t),lerp(ab.y,bc.y,t)},q2={lerp(bc.x,cd.x,t),lerp(bc.y,cd.y,t)};
  if(!nova_vector_line_to(p,lerp(q1.x,q2.x,t),lerp(q1.y,q2.y,t)))return false;}return true;}
bool nova_vector_close(nova_vector_path_t *p)
{if(!p||!p->open)return false;
 if((p->current.x!=p->first.x||p->current.y!=p->first.y)&&
    !add(p,p->current,p->first))return false;
 p->current=p->first;p->open=false;return true;}
static bool inside(const nova_vector_path_t *p,int32_t x,int32_t y,nova_vector_fill_rule_t r)
{int32_t winding=0;for(uint16_t i=0;i<p->count;++i){nova_vector_point_t a=p->segments[i].a,b=p->segments[i].b;
 int64_t cross=(int64_t)(b.x-a.x)*(y-a.y)-(int64_t)(x-a.x)*(b.y-a.y);
 if(a.y<=y&&b.y>y&&cross>0)++winding;
 else if(a.y>y&&b.y<=y&&cross<0)--winding;}
 return r==NOVA_VECTOR_EVENODD?(winding&1)!=0:winding!=0;}
uint8_t nova_vector_coverage(const nova_vector_path_t *p,int32_t x,int32_t y,nova_vector_fill_rule_t r)
{if(!p||p->overflow||r>NOVA_VECTOR_EVENODD)return 0;uint8_t hits=0;
 static const int32_t o[4]={8192,24576,40960,57344};
 for(uint8_t i=0;i<4;++i)
  if(inside(p,(int32_t)((int64_t)x*65536)+o[i],
              (int32_t)((int64_t)y*65536)+o[3-i],r))++hits;
 return (uint8_t)(((uint32_t)hits*255u+2u)/4u);}
static uint32_t over(uint32_t src,uint32_t dst)
{uint32_t sa=src>>24,inv=255u-sa;
 uint32_t da=dst>>24,oa=sa+(da*inv+127u)/255u,out=oa<<24;
 for(uint8_t sh=0;sh<24;sh+=8){uint32_t sc=(src>>sh)&255u,dc=(dst>>sh)&255u;
  uint32_t oc=sc+(dc*inv+127u)/255u;if(oc>255u)oc=255u;out|=oc<<sh;}return out;}
bool nova_vector_fill(nova_surface_t *s,const nova_vector_path_t *p,nova_rect_t c,uint32_t color,nova_vector_fill_rule_t r)
{if(!s||!s->pixels||!p||p->overflow||c.width<=0||c.height<=0)return false;
 for(int32_t y=c.y;y<c.y+c.height;++y)for(int32_t x=c.x;x<c.x+c.width;++x){if(x<0||y<0||(uint32_t)x>=s->width||(uint32_t)y>=s->height)continue;
  uint32_t cov=nova_vector_coverage(p,x,y,r);if(!cov)continue;uint32_t a=((color>>24)*cov+127u)/255u;
  uint32_t rgb=0;for(uint8_t sh=0;sh<24;sh+=8)rgb|=((((color>>sh)&255u)*cov+127u)/255u)<<sh;
  uint32_t at=(uint32_t)y*s->stride+(uint32_t)x;
  s->pixels[at]=over(a<<24|rgb,s->pixels[at]);}return true;}
