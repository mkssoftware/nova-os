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
 *out=length==7?0xff000000u|rgb:((rgb&255u)<<24)|(rgb>>8);return true;}
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
static bool document(const char *source,uint32_t size,nova_surface_t *target,nova_rect_t destination,const nova_svg_theme_t *theme,bool render)
{const char *p=source,*end=p+size;uint32_t elements=0;bool root=false;
 while(p<end){if(*p++!='<')continue;if(p==end)return false;if(*p=='!'||*p=='?')return false;
  bool closing=*p=='/';if(closing)++p;const char *ns=p;while(p<end&&((*p>='a'&&*p<='z')||(*p>='A'&&*p<='Z')))++p;
  uint32_t nl=(uint32_t)(p-ns);if(!nl||!allowed_element(ns,nl))return false;if(!closing&&++elements>NOVA_SVG_ELEMENT_CAPACITY)return false;
  if(!root){if(closing||!nameeq(ns,nl,"svg"))return false;root=true;}
  const char *d=0,*de=0,*fill=0,*fille=0;bool evenodd=false;
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
   ++p;}
  if(p==end||*p++!='>')return false;
  if(!closing&&nameeq(ns,nl,"path")){uint32_t c=0xff000000u;if(fill&&!parsed_color(fill,fille,theme,&c))return false;
   if(!d||!path_data(d,de))return false;
   ++diag.paths;
   if(render&&c&&!nova_vector_fill(target,&path,destination,c,evenodd?NOVA_VECTOR_EVENODD:NOVA_VECTOR_NONZERO))return false;}}
 return root&&elements;}
bool nova_svg_initialize(void){diag=(nova_svg_diagnostics_t){.initialized=true};return true;}
bool nova_svg_validate(const void *source,uint32_t size)
{++diag.validations;bool ok=diag.initialized&&source&&size&&size<=NOVA_SVG_SOURCE_CAPACITY&&document(source,size,0,(nova_rect_t){0},0,false);if(!ok)++diag.failures;return ok;}
bool nova_svg_render(const void *source,uint32_t size,nova_surface_t *target,nova_rect_t destination,const nova_svg_theme_t *theme)
{bool ok=diag.initialized&&source&&target&&target->pixels&&destination.width>0&&destination.height>0&&size&&size<=NOVA_SVG_SOURCE_CAPACITY&&document(source,size,target,destination,theme,true);
 if(ok)++diag.renders;else ++diag.failures;return ok;}
const nova_svg_diagnostics_t *nova_svg_diagnostics(void){return &diag;}
