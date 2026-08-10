#include "transform2d.h"
#include "dirty_manager.h"

#define NOVA_TRANSFORM_LIMIT 32767.0f
#define NOVA_PI 3.14159265358979323846f
#define NOVA_TWO_PI 6.28318530717958647692f

static nova_transform2d_t stack[NOVA_TRANSFORM_STACK_CAPACITY];
static uint16_t stack_depth;
static nova_transform_diagnostics_t diagnostics;

static bool finite_float(float value)
{
    union {float f;uint32_t u;} bits={value};
    return (bits.u&0x7f800000u)!=0x7f800000u;
}

static float absolute(float value){return value<0?-value:value;}

static bool component_valid(float value)
{
    return finite_float(value)&&absolute(value)<=NOVA_TRANSFORM_LIMIT;
}

nova_transform2d_t nova_transform_identity(void)
{
    ++diagnostics.identities;
    return (nova_transform2d_t){1,0,0,1,0,0};
}

bool nova_transform_valid(const nova_transform2d_t *t)
{
    ++diagnostics.validations;
    bool valid=t&&component_valid(t->m11)&&component_valid(t->m12)&&
        component_valid(t->m21)&&component_valid(t->m22)&&
        component_valid(t->tx)&&component_valid(t->ty);
    if(!valid)++diagnostics.rejected;
    return valid;
}

bool nova_transform_multiply(nova_transform2d_t *result,
                             const nova_transform2d_t *a,
                             const nova_transform2d_t *b)
{
    if(!result||!nova_transform_valid(a)||!nova_transform_valid(b))return false;
    nova_transform2d_t value={
        a->m11*b->m11+a->m12*b->m21,
        a->m11*b->m12+a->m12*b->m22,
        a->m21*b->m11+a->m22*b->m21,
        a->m21*b->m12+a->m22*b->m22,
        a->m11*b->tx+a->m12*b->ty+a->tx,
        a->m21*b->tx+a->m22*b->ty+a->ty};
    if(!nova_transform_valid(&value))return false;
    *result=value;++diagnostics.multiplications;return true;
}

static bool prepend(nova_transform2d_t *target,nova_transform2d_t operation)
{
    if(!target)return false;
    nova_transform2d_t result;
    if(!nova_transform_multiply(&result,&operation,target))return false;
    *target=result;return true;
}

bool nova_transform_translate(nova_transform2d_t *t,float x,float y)
{
    if(!component_valid(x)||!component_valid(y))return false;
    if(!prepend(t,(nova_transform2d_t){1,0,0,1,x,y}))return false;
    ++diagnostics.translations;return true;
}

static float normalize_angle(float angle)
{
    while(angle>NOVA_PI)angle-=NOVA_TWO_PI;
    while(angle<-NOVA_PI)angle+=NOVA_TWO_PI;
    return angle;
}

static void sine_cosine(float angle,float *sine,float *cosine)
{
    angle=normalize_angle(angle);
    float cosine_sign=1.0f;
    if(angle>NOVA_PI/2.0f){angle=NOVA_PI-angle;cosine_sign=-1.0f;}
    else if(angle<-NOVA_PI/2.0f){angle=-NOVA_PI-angle;cosine_sign=-1.0f;}
    float x2=angle*angle;
    *sine=angle*(1.0f-x2/6.0f+x2*x2/120.0f-
        x2*x2*x2/5040.0f+x2*x2*x2*x2/362880.0f);
    *cosine=cosine_sign*(1.0f-x2/2.0f+x2*x2/24.0f-
        x2*x2*x2/720.0f+x2*x2*x2*x2/40320.0f);
    if(absolute(*sine)<0.0001f)*sine=0;
    if(absolute(*cosine)<0.0001f)*cosine=0;
    if(absolute(*sine-1.0f)<0.0001f)*sine=1.0f;
    if(absolute(*sine+1.0f)<0.0001f)*sine=-1.0f;
    if(absolute(*cosine-1.0f)<0.0001f)*cosine=1.0f;
    if(absolute(*cosine+1.0f)<0.0001f)*cosine=-1.0f;
}

bool nova_transform_rotate(nova_transform2d_t *t,float radians)
{
    if(!finite_float(radians)||absolute(radians)>1000000.0f)return false;
    float sine,cosine;sine_cosine(radians,&sine,&cosine);
    if(!prepend(t,(nova_transform2d_t){cosine,-sine,sine,cosine,0,0}))return false;
    ++diagnostics.rotations;return true;
}

bool nova_transform_scale(nova_transform2d_t *t,float sx,float sy)
{
    if(!component_valid(sx)||!component_valid(sy))return false;
    if(!prepend(t,(nova_transform2d_t){sx,0,0,sy,0,0}))return false;
    ++diagnostics.scales;return true;
}

bool nova_transform_mirror(nova_transform2d_t *t,bool horizontal,bool vertical)
{
    if(!horizontal&&!vertical)return true;
    if(!nova_transform_scale(t,horizontal?-1.0f:1.0f,vertical?-1.0f:1.0f))
        return false;
    ++diagnostics.mirrors;return true;
}

bool nova_transform_shear(nova_transform2d_t *t,float x,float y)
{
    if(!component_valid(x)||!component_valid(y)||
       !prepend(t,(nova_transform2d_t){1,x,y,1,0,0}))return false;
    ++diagnostics.shears;return true;
}

bool nova_transform_pivot(nova_transform2d_t *t,
                          const nova_transform2d_t *operation,float x,float y)
{
    if(!t||!nova_transform_valid(operation)||!component_valid(x)||
       !component_valid(y))return false;
    nova_transform2d_t around=*operation;
    around.tx=x-operation->m11*x-operation->m12*y;
    around.ty=y-operation->m21*x-operation->m22*y;
    if(!prepend(t,around))return false;
    ++diagnostics.pivots;return true;
}

bool nova_transform_inverse(nova_transform2d_t *result,
                            const nova_transform2d_t *t)
{
    if(!result||!nova_transform_valid(t))return false;
    float determinant=t->m11*t->m22-t->m12*t->m21;
    if(!finite_float(determinant)||absolute(determinant)<0.000001f)return false;
    nova_transform2d_t inverse={t->m22/determinant,-t->m12/determinant,
        -t->m21/determinant,t->m11/determinant,0,0};
    inverse.tx=-(inverse.m11*t->tx+inverse.m12*t->ty);
    inverse.ty=-(inverse.m21*t->tx+inverse.m22*t->ty);
    if(!nova_transform_valid(&inverse))return false;
    *result=inverse;++diagnostics.inversions;return true;
}

bool nova_transform_point(const nova_transform2d_t *t,nova_point2d_t point,
                          nova_point2d_t *result)
{
    if(!result||!nova_transform_valid(t)||!component_valid(point.x)||
       !component_valid(point.y))return false;
    nova_point2d_t value={t->m11*point.x+t->m12*point.y+t->tx,
        t->m21*point.x+t->m22*point.y+t->ty};
    if(!component_valid(value.x)||!component_valid(value.y))return false;
    *result=value;++diagnostics.points;return true;
}

static int32_t floor_i32(float value)
{
    int32_t integer=(int32_t)value;
    return value<(float)integer?integer-1:integer;
}

static int32_t ceil_i32(float value)
{
    int32_t integer=(int32_t)value;
    return value>(float)integer?integer+1:integer;
}

bool nova_transform_bounds(const nova_transform2d_t *t,nova_rect_t b,
                           nova_rect_t *result)
{
    if(!result||b.width<0||b.height<0)return false;
    int64_t right_i=(int64_t)b.x+b.width,bottom_i=(int64_t)b.y+b.height;
    if(right_i>INT32_MAX||right_i<INT32_MIN||
       bottom_i>INT32_MAX||bottom_i<INT32_MIN)return false;
    nova_point2d_t input[4]={{(float)b.x,(float)b.y},
        {(float)right_i,(float)b.y},
        {(float)b.x,(float)bottom_i},
        {(float)right_i,(float)bottom_i}},output[4];
    for(uint8_t i=0;i<4;++i)if(!nova_transform_point(t,input[i],&output[i]))return false;
    float left=output[0].x,right=left,top=output[0].y,bottom=top;
    for(uint8_t i=1;i<4;++i){
        if(output[i].x<left)left=output[i].x;
        if(output[i].x>right)right=output[i].x;
        if(output[i].y<top)top=output[i].y;
        if(output[i].y>bottom)bottom=output[i].y;
    }
    int32_t x=floor_i32(left),y=floor_i32(top),r=ceil_i32(right),bot=ceil_i32(bottom);
    *result=(nova_rect_t){x,y,r-x,bot-y};++diagnostics.bounds;return true;
}

static bool fixed_component(float value,int32_t *result)
{
    if(!component_valid(value)||!result)return false;
    float scaled=value*(float)NOVA_TRANSFORM_FIXED_ONE;
    if(scaled>2147483520.0f||scaled<-2147483648.0f)return false;
    *result=(int32_t)(scaled+(scaled>=0?0.5f:-0.5f));return true;
}

bool nova_transform_to_fixed(const nova_transform2d_t *t,
                             nova_fixed_transform2d_t *fixed)
{
    if(!fixed||!nova_transform_valid(t)||
       !fixed_component(t->m11,&fixed->m11)||!fixed_component(t->m12,&fixed->m12)||
       !fixed_component(t->m21,&fixed->m21)||!fixed_component(t->m22,&fixed->m22)||
       !fixed_component(t->tx,&fixed->tx)||!fixed_component(t->ty,&fixed->ty))
        return false;
    ++diagnostics.fixed_conversions;return true;
}

nova_fixed_transform2d_t nova_transform_fixed_identity(void)
{return (nova_fixed_transform2d_t){NOVA_TRANSFORM_FIXED_ONE,0,0,
    NOVA_TRANSFORM_FIXED_ONE,0,0};}

bool nova_transform_fixed_valid(nova_fixed_transform2d_t t)
{
    const int32_t coefficient_limit=256*NOVA_TRANSFORM_FIXED_ONE;
    if(t.m11>coefficient_limit||t.m11<-coefficient_limit||
       t.m12>coefficient_limit||t.m12<-coefficient_limit||
       t.m21>coefficient_limit||t.m21<-coefficient_limit||
       t.m22>coefficient_limit||t.m22<-coefficient_limit)return false;
    int64_t determinant=(int64_t)t.m11*t.m22-(int64_t)t.m12*t.m21;
    return determinant!=0;
}

static bool fixed_value(int64_t value,int32_t *result)
{
    value/=NOVA_TRANSFORM_FIXED_ONE;
    if(value>INT32_MAX||value<INT32_MIN)return false;
    *result=(int32_t)value;return true;
}

bool nova_transform_fixed_multiply(nova_fixed_transform2d_t *result,
                                   nova_fixed_transform2d_t a,
                                   nova_fixed_transform2d_t b)
{
    if(!result)return false;
    nova_fixed_transform2d_t value;
    if(!fixed_value((int64_t)a.m11*b.m11+(int64_t)a.m12*b.m21,&value.m11)||
       !fixed_value((int64_t)a.m11*b.m12+(int64_t)a.m12*b.m22,&value.m12)||
       !fixed_value((int64_t)a.m21*b.m11+(int64_t)a.m22*b.m21,&value.m21)||
       !fixed_value((int64_t)a.m21*b.m12+(int64_t)a.m22*b.m22,&value.m22))
        return false;
    int32_t translated_x,translated_y;
    if(!fixed_value((int64_t)a.m11*b.tx+(int64_t)a.m12*b.ty,&translated_x)||
       !fixed_value((int64_t)a.m21*b.tx+(int64_t)a.m22*b.ty,&translated_y)||
       (int64_t)a.tx+translated_x>INT32_MAX||(int64_t)a.tx+translated_x<INT32_MIN||
       (int64_t)a.ty+translated_y>INT32_MAX||(int64_t)a.ty+translated_y<INT32_MIN)
        return false;
    value.tx=a.tx+translated_x;value.ty=a.ty+translated_y;
    *result=value;++diagnostics.fixed_multiplications;return true;
}

bool nova_transform_fixed_inverse(nova_fixed_transform2d_t *result,
                                  nova_fixed_transform2d_t t)
{
    if(!result)return false;
    int64_t determinant=((int64_t)t.m11*t.m22-(int64_t)t.m12*t.m21)/
        NOVA_TRANSFORM_FIXED_ONE;
    if(!determinant)return false;
    nova_fixed_transform2d_t inverse;
    int64_t values[4]={
        (int64_t)t.m22*NOVA_TRANSFORM_FIXED_ONE/determinant,
        -(int64_t)t.m12*NOVA_TRANSFORM_FIXED_ONE/determinant,
        -(int64_t)t.m21*NOVA_TRANSFORM_FIXED_ONE/determinant,
        (int64_t)t.m11*NOVA_TRANSFORM_FIXED_ONE/determinant};
    for(uint8_t i=0;i<4;++i)if(values[i]>INT32_MAX||values[i]<INT32_MIN)return false;
    inverse.m11=(int32_t)values[0];inverse.m12=(int32_t)values[1];
    inverse.m21=(int32_t)values[2];inverse.m22=(int32_t)values[3];
    int32_t tx,ty;
    if(!fixed_value(-((int64_t)inverse.m11*t.tx+(int64_t)inverse.m12*t.ty),&tx)||
       !fixed_value(-((int64_t)inverse.m21*t.tx+(int64_t)inverse.m22*t.ty),&ty))
        return false;
    inverse.tx=tx;inverse.ty=ty;*result=inverse;return true;
}

bool nova_transform_fixed_point(nova_fixed_transform2d_t t,
                                nova_fixed_point2d_t p,
                                nova_fixed_point2d_t *result)
{
    if(!result)return false;
    int64_t x=(int64_t)t.m11*p.x+(int64_t)t.m12*p.y+
        (int64_t)t.tx*NOVA_TRANSFORM_FIXED_ONE;
    int64_t y=(int64_t)t.m21*p.x+(int64_t)t.m22*p.y+
        (int64_t)t.ty*NOVA_TRANSFORM_FIXED_ONE;
    if(!fixed_value(x,&result->x)||!fixed_value(y,&result->y))return false;
    ++diagnostics.points;return true;
}

bool nova_transform_fixed_bounds(nova_fixed_transform2d_t t,nova_rect_t b,
                                 nova_rect_t *result)
{
    if(!result||b.width<0||b.height<0)return false;
    int64_t bounds_right=(int64_t)b.x+b.width;
    int64_t bounds_bottom=(int64_t)b.y+b.height;
    int64_t fixed_values[4]={(int64_t)b.x*NOVA_TRANSFORM_FIXED_ONE,
        (int64_t)b.y*NOVA_TRANSFORM_FIXED_ONE,
        bounds_right*NOVA_TRANSFORM_FIXED_ONE,
        bounds_bottom*NOVA_TRANSFORM_FIXED_ONE};
    for(uint8_t i=0;i<4;++i)
        if(fixed_values[i]>INT32_MAX||fixed_values[i]<INT32_MIN)return false;
    nova_fixed_point2d_t input[4]={
        {(int32_t)fixed_values[0],(int32_t)fixed_values[1]},
        {(int32_t)fixed_values[2],(int32_t)fixed_values[1]},
        {(int32_t)fixed_values[0],(int32_t)fixed_values[3]},
        {(int32_t)fixed_values[2],(int32_t)fixed_values[3]}};
    nova_fixed_point2d_t output[4];
    for(uint8_t i=0;i<4;++i)if(!nova_transform_fixed_point(t,input[i],&output[i]))return false;
    int32_t left=output[0].x,right=left,top=output[0].y,bottom=top;
    for(uint8_t i=1;i<4;++i){
        if(output[i].x<left)left=output[i].x;
        if(output[i].x>right)right=output[i].x;
        if(output[i].y<top)top=output[i].y;
        if(output[i].y>bottom)bottom=output[i].y;
    }
    int32_t x=left/NOVA_TRANSFORM_FIXED_ONE,y=top/NOVA_TRANSFORM_FIXED_ONE;
    if(left<0&&left%NOVA_TRANSFORM_FIXED_ONE)--x;
    if(top<0&&top%NOVA_TRANSFORM_FIXED_ONE)--y;
    int32_t r=right/NOVA_TRANSFORM_FIXED_ONE,bot=bottom/NOVA_TRANSFORM_FIXED_ONE;
    if(right>0&&right%NOVA_TRANSFORM_FIXED_ONE)++r;
    if(bottom>0&&bottom%NOVA_TRANSFORM_FIXED_ONE)++bot;
    *result=(nova_rect_t){x,y,r-x,bot-y};++diagnostics.bounds;return true;
}

bool nova_transform_dirty(nova_rect_t old_bounds,nova_rect_t new_bounds)
{
    bool first=nova_dirty_add(&old_bounds),second=nova_dirty_add(&new_bounds);
    if(first)++diagnostics.dirty_regions;
    if(second)++diagnostics.dirty_regions;
    return first&&second;
}

void nova_transform_stack_reset(void)
{
    stack_depth=1;stack[0]=nova_transform_identity();
}

bool nova_transform_stack_push(const nova_transform2d_t *local)
{
    if(!local||!stack_depth||stack_depth==NOVA_TRANSFORM_STACK_CAPACITY){
        ++diagnostics.stack_overflows;return false;
    }
    nova_transform2d_t world;
    if(!nova_transform_multiply(&world,&stack[stack_depth-1],local))return false;
    stack[stack_depth++]=world;++diagnostics.stack_pushes;
    if(stack_depth>diagnostics.stack_peak)diagnostics.stack_peak=stack_depth;
    return true;
}

bool nova_transform_stack_pop(void)
{
    if(stack_depth<=1)return false;
    --stack_depth;++diagnostics.stack_pops;return true;
}

nova_transform2d_t nova_transform_stack_current(void)
{
    return stack_depth?stack[stack_depth-1]:nova_transform_identity();
}
uint16_t nova_transform_stack_depth(void){return stack_depth;}

const nova_transform_diagnostics_t *nova_transform_diagnostics(void)
{
    return &diagnostics;
}
