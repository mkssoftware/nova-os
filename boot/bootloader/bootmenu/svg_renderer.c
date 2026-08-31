#include "svg_renderer.h"
#include "vector_geometry.h"

typedef struct {const char *p,*end;} reader_t;
static nova_svg_diagnostics_t diag;
static nova_vector_path_t path;
static bool space(char c){return c==' '||c=='\t'||c=='\r'||c=='\n'||c==',';}
static void skip(reader_t *r){while(r->p<r->end&&space(*r->p))++r->p;}
static bool number(reader_t *r,int32_t *fixed)
{skip(r);bool neg=false;if(r->p<r->end&&(*r->p=='-'||*r->p=='+'))neg=*r->p++=='-';
 uint64_t whole=0,fraction=0,scale=1;bool any=false;
 while(r->p<r->end&&*r->p>='0'&&*r->p<='9'){any=true;if(whole>32767)return false;
  whole=whole*10u+(uint8_t)(*r->p++-'0');}
 if(r->p<r->end&&*r->p=='.'){++r->p;while(r->p<r->end&&*r->p>='0'&&*r->p<='9'){
  any=true;if(scale<1000000u){fraction=fraction*10u+(uint8_t)(*r->p-'0');scale*=10u;}++r->p;}}
 if(!any)return false;
 int64_t value=(int64_t)whole*65536+(int64_t)((fraction*65536+scale/2)/scale);
 if(neg)value=-value;
 if(value<INT32_MIN||value>INT32_MAX)return false;
 *fixed=(int32_t)value;return true;}
static bool literal(const char *p,const char *end,const char *s)
{while(*s){if(p==end||*p++!=*s++)return false;}return true;}
static uint8_t hex(char c){if(c>='0'&&c<='9')return (uint8_t)(c-'0');
 if(c>='a'&&c<='f')return (uint8_t)(c-'a'+10);
 if(c>='A'&&c<='F')return (uint8_t)(c-'A'+10);
 return 0xff;}
static bool color(const char *p,const char *end,const nova_svg_theme_t *t,uint32_t *out)
{if(end-p==4&&literal(p,end,"none")){*out=0;return true;}
 if(end-p==18&&literal(p,end,"var(--nova-accent)")){*out=t?t->accent:0xff2997d6u;return true;}
 if(end-p==16&&literal(p,end,"var(--nova-text)")){*out=t?t->text:0xffffffffu;return true;}
 if(end-p==17&&literal(p,end,"var(--nova-error)")){*out=t?t->error:0xffb9134fu;return true;}
 return false;}
static bool parsed_color(const char *p,const char *end,const nova_svg_theme_t *t,uint32_t *out)
{uint32_t length=(uint32_t)(end-p);if(length>=4&&*p!='#')return color(p,end,t,out);
 if((length!=7&&length!=9)||*p++!='#')return false;
 uint32_t rgb=0;for(uint32_t i=1;i<length;++i){uint8_t h=hex(*p++);if(h>15)return false;rgb=rgb<<4|h;}
 *out=length==7?0xff000000u|rgb:((rgb&255u)<<24)|(rgb>>8);
 uint32_t a=*out>>24,premul=a<<24;
 for(uint8_t shift=0;shift<24;shift+=8)premul|=((((*out>>shift)&255u)*a+127u)/255u)<<shift;
 *out=premul;return true;}
static bool path_data(const char *begin,const char *end)
{reader_t r={begin,end};int32_t x=0,y=0,sx=0,sy=0;char command=0;nova_vector_path_reset(&path);
 while(1){skip(&r);if(r.p==r.end)break;if((*r.p>='A'&&*r.p<='Z')||(*r.p>='a'&&*r.p<='z'))command=*r.p++;
  if(!command)return false;
  bool rel=command>='a';char op=rel?(char)(command-'a'+'A'):command;
  if(op=='Z'){if(!nova_vector_close(&path))return false;x=sx;y=sy;command=0;continue;}
  int32_t a,b,c,d,e,f;if(!number(&r,&a))return false;
  if(op=='H'){if(rel)a+=x;x=a;if(!nova_vector_line_to(&path,x,y))return false;continue;}
  if(op=='V'){if(rel)a+=y;y=a;if(!nova_vector_line_to(&path,x,y))return false;continue;}
  if(!number(&r,&b))return false;
  if(op=='M'||op=='L'){if(rel){a+=x;b+=y;}x=a;y=b;
   if(op=='M'){if(!nova_vector_move_to(&path,x,y))return false;sx=x;sy=y;command=rel?'l':'L';}
   else if(!nova_vector_line_to(&path,x,y))return false;
   continue;}
  if(op=='Q'){if(!number(&r,&c)||!number(&r,&d))return false;if(rel){a+=x;b+=y;c+=x;d+=y;}
   if(!nova_vector_quad_to(&path,a,b,c,d))return false;
   x=c;y=d;continue;}
  if(op=='C'){if(!number(&r,&c)||!number(&r,&d)||!number(&r,&e)||!number(&r,&f))return false;
   if(rel){a+=x;b+=y;c+=x;d+=y;e+=x;f+=y;}if(!nova_vector_cubic_to(&path,a,b,c,d,e,f))return false;x=e;y=f;continue;}
  return false;}return path.count&&!path.overflow;}
static bool nameeq(const char *p,uint32_t n,const char *s)
{uint32_t i=0;while(s[i]&&i<n&&p[i]==s[i])++i;return i==n&&!s[i];}
static bool allowed_element(const char *p,uint32_t n)
{static const char *names[]={"svg","g","path","rect","circle","ellipse","line","polyline","polygon"};
 for(uint8_t i=0;i<9;++i)if(nameeq(p,n,names[i]))return true;
 return false;}
static bool allowed_attribute(const char *p,uint32_t n)
{static const char *names[]={"viewBox","fill","fill-rule","stroke","stroke-width","stroke-linecap","stroke-linejoin","transform","d","x","y","width","height","cx","cy","r","rx","ry","x1","y1","x2","y2","points"};
 for(uint8_t i=0;i<23;++i)if(nameeq(p,n,names[i]))return true;
 return false;}
typedef struct {int32_t x,y,width,height,cx,cy,r,rx,ry,x1,y1,x2,y2;uint16_t have;} geometry_t;
static bool scalar(const char *begin,const char *end,int32_t *value)
{reader_t reader={begin,end};return number(&reader,value)&&(skip(&reader),reader.p==reader.end);}
static bool geometry_attribute(geometry_t *g,const char *name,uint32_t length,const char *v,const char *ve)
{int32_t value;if(!scalar(v,ve,&value))return false;
#define SVG_SCALAR(field,bit) if(nameeq(name,length,#field)){g->field=value;g->have|=bit;return true;}
 SVG_SCALAR(x,1u) SVG_SCALAR(y,2u) SVG_SCALAR(width,4u) SVG_SCALAR(height,8u)
 SVG_SCALAR(cx,16u) SVG_SCALAR(cy,32u) SVG_SCALAR(r,64u) SVG_SCALAR(rx,128u)
 SVG_SCALAR(ry,256u) SVG_SCALAR(x1,512u) SVG_SCALAR(y1,1024u)
 SVG_SCALAR(x2,2048u) SVG_SCALAR(y2,4096u)
#undef SVG_SCALAR
 return false;}
static bool rectangle(const geometry_t *g)
{if((g->have&15u)!=15u||g->width<=0||g->height<=0)return false;nova_vector_path_reset(&path);
 return nova_vector_move_to(&path,g->x,g->y)&&nova_vector_line_to(&path,g->x+g->width,g->y)&&
  nova_vector_line_to(&path,g->x+g->width,g->y+g->height)&&nova_vector_line_to(&path,g->x,g->y+g->height)&&nova_vector_close(&path);}
static bool ellipse(const geometry_t *g,bool circle)
{int32_t rx=circle?g->r:g->rx,ry=circle?g->r:g->ry;uint16_t need=circle?112u:432u;
 if((g->have&need)!=need||rx<=0||ry<=0)return false;
 int32_t kx=(int32_t)((int64_t)rx*36195>>16),ky=(int32_t)((int64_t)ry*36195>>16);
 nova_vector_path_reset(&path);return nova_vector_move_to(&path,g->cx+rx,g->cy)&&
  nova_vector_cubic_to(&path,g->cx+rx,g->cy+ky,g->cx+kx,g->cy+ry,g->cx,g->cy+ry)&&
  nova_vector_cubic_to(&path,g->cx-kx,g->cy+ry,g->cx-rx,g->cy+ky,g->cx-rx,g->cy)&&
  nova_vector_cubic_to(&path,g->cx-rx,g->cy-ky,g->cx-kx,g->cy-ry,g->cx,g->cy-ry)&&
  nova_vector_cubic_to(&path,g->cx+kx,g->cy-ry,g->cx+rx,g->cy-ky,g->cx+rx,g->cy)&&nova_vector_close(&path);}
static bool point_list(const char *begin,const char *end,bool close)
{reader_t r={begin,end};int32_t x,y;nova_vector_path_reset(&path);if(!number(&r,&x)||!number(&r,&y)||!nova_vector_move_to(&path,x,y))return false;
 while(1){skip(&r);if(r.p==r.end)break;if(!number(&r,&x)||!number(&r,&y)||!nova_vector_line_to(&path,x,y))return false;}
 return !close||nova_vector_close(&path);}
static bool line_path(const geometry_t *g)
{if((g->have&7680u)!=7680u)return false;nova_vector_path_reset(&path);
 return nova_vector_move_to(&path,g->x1,g->y1)&&nova_vector_line_to(&path,g->x2,g->y2);}
static bool map_path(int32_t vx,int32_t vy,int32_t vw,int32_t vh,nova_rect_t d)
{if(vw<=0||vh<=0)return false;for(uint16_t i=0;i<path.count;++i){nova_vector_point_t *points[2]={&path.segments[i].a,&path.segments[i].b};
  for(uint8_t q=0;q<2;++q){int64_t x=(int64_t)(points[q]->x-vx)*d.width*65536/vw+(int64_t)d.x*65536;
   int64_t y=(int64_t)(points[q]->y-vy)*d.height*65536/vh+(int64_t)d.y*65536;
   if(x<INT32_MIN||x>INT32_MAX||y<INT32_MIN||y>INT32_MAX)return false;
   points[q]->x=(int32_t)x;points[q]->y=(int32_t)y;}}
 return true;}
static bool document(const char *source,uint32_t size,nova_surface_t *target,nova_rect_t destination,const nova_svg_theme_t *theme,bool render)
{const char *p=source,*end=p+size;uint32_t elements=0;bool root=false,viewbox=false;
 int32_t vx=0,vy=0,vw=destination.width<<16,vh=destination.height<<16;
 while(p<end){if(*p++!='<')continue;if(p==end)return false;if(*p=='!'||*p=='?')return false;
  bool closing=*p=='/';if(closing)++p;const char *ns=p;while(p<end&&((*p>='a'&&*p<='z')||(*p>='A'&&*p<='Z')))++p;
  uint32_t nl=(uint32_t)(p-ns);if(!nl||!allowed_element(ns,nl))return false;if(!closing&&++elements>NOVA_SVG_ELEMENT_CAPACITY)return false;
  if(!root){if(closing||!nameeq(ns,nl,"svg"))return false;root=true;}
  const char *d=0,*de=0,*fill=0,*fille=0,*points=0,*pointse=0;bool evenodd=false;geometry_t geometry={0};
  while(p<end&&*p!='>'){skip(&(reader_t){p,end});while(p<end&&space(*p))++p;if(p<end&&*p=='/'){++p;break;}if(p==end||*p=='>')break;
   const char *as=p;while(p<end&&((*p>='a'&&*p<='z')||(*p>='A'&&*p<='Z')||*p=='-'))++p;uint32_t al=(uint32_t)(p-as);
   if(!al||!allowed_attribute(as,al))return false;
   while(p<end&&space(*p))++p;
   if(p==end||*p++!='=')return false;
   while(p<end&&space(*p))++p;
   if(p==end||(*p!='\''&&*p!='\"'))return false;
   char quote=*p++;const char *value=p;while(p<end&&*p!=quote)++p;if(p==end)return false;
   if(nameeq(as,al,"d")){d=value;de=p;}else if(nameeq(as,al,"fill")){fill=value;fille=p;}
   else if(nameeq(as,al,"fill-rule")){if(p-value==7&&literal(value,p,"evenodd"))evenodd=true;else if(!(p-value==7&&literal(value,p,"nonzero")))return false;}
   else if(nameeq(as,al,"points")){points=value;pointse=p;}
   else if(nameeq(as,al,"viewBox")){reader_t vr={value,p};if(viewbox||!number(&vr,&vx)||!number(&vr,&vy)||!number(&vr,&vw)||!number(&vr,&vh)||(skip(&vr),vr.p!=vr.end)||vw<=0||vh<=0)return false;viewbox=true;}
   else if(nameeq(as,al,"x")||nameeq(as,al,"y")||nameeq(as,al,"width")||nameeq(as,al,"height")||nameeq(as,al,"cx")||nameeq(as,al,"cy")||nameeq(as,al,"r")||nameeq(as,al,"rx")||nameeq(as,al,"ry")||nameeq(as,al,"x1")||nameeq(as,al,"y1")||nameeq(as,al,"x2")||nameeq(as,al,"y2")){if(!geometry_attribute(&geometry,as,al,value,p))return false;}
   ++p;}
  if(p==end||*p++!='>')return false;
  if(!closing&&!nameeq(ns,nl,"svg")&&!nameeq(ns,nl,"g")){uint32_t c=0xff000000u;if(fill&&!parsed_color(fill,fille,theme,&c))return false;
   bool built=nameeq(ns,nl,"path")?d&&path_data(d,de):nameeq(ns,nl,"rect")?rectangle(&geometry):
    nameeq(ns,nl,"circle")?ellipse(&geometry,true):nameeq(ns,nl,"ellipse")?ellipse(&geometry,false):
    nameeq(ns,nl,"polygon")?points&&point_list(points,pointse,true):
    nameeq(ns,nl,"polyline")?points&&point_list(points,pointse,true):
    nameeq(ns,nl,"line")?line_path(&geometry):true;
   if(!built)return false;
   ++diag.paths;
   if(render&&path.count&&(!map_path(vx,vy,vw,vh,destination)||(c&&!nova_vector_fill(target,&path,destination,c,evenodd?NOVA_VECTOR_EVENODD:NOVA_VECTOR_NONZERO))))return false;}}
 return root&&elements;}
bool nova_svg_initialize(void){diag=(nova_svg_diagnostics_t){.initialized=true};return true;}
bool nova_svg_validate(const void *source,uint32_t size)
{++diag.validations;bool ok=diag.initialized&&source&&size&&size<=NOVA_SVG_SOURCE_CAPACITY&&document(source,size,0,(nova_rect_t){0},0,false);if(!ok)++diag.failures;return ok;}
bool nova_svg_render(const void *source,uint32_t size,nova_surface_t *target,nova_rect_t destination,const nova_svg_theme_t *theme)
{bool ok=diag.initialized&&source&&target&&target->pixels&&destination.width>0&&destination.height>0&&size&&size<=NOVA_SVG_SOURCE_CAPACITY&&document(source,size,target,destination,theme,true);
 if(ok)++diag.renders;else ++diag.failures;return ok;}
const nova_svg_diagnostics_t *nova_svg_diagnostics(void){return &diag;}
