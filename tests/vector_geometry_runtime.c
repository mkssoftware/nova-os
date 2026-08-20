#include <stdio.h>
#include "../boot/bootloader/bootmenu/vector_geometry.h"
int main(void){nova_vector_path_t p;nova_vector_path_reset(&p);
 if(!nova_vector_move_to(&p,0,0)||!nova_vector_line_to(&p,2<<16,0)||
    !nova_vector_line_to(&p,2<<16,2<<16)||!nova_vector_line_to(&p,0,2<<16)||
    !nova_vector_close(&p))return 1;
 uint8_t inside=nova_vector_coverage(&p,0,0,NOVA_VECTOR_NONZERO);
 uint8_t outside=nova_vector_coverage(&p,3,3,NOVA_VECTOR_NONZERO);
 if(inside!=255||outside){printf("coverage=%u/%u\n",inside,outside);return 1;}
 uint32_t pixels[4]={0x40000040u,0x40000040u,0x40000040u,0x40000040u};
 nova_surface_t s={.pixels=pixels,.width=2,.height=2,.stride=2};
 nova_rect_t clip={0,0,2,2};
 if(!nova_vector_fill(&s,&p,clip,0x80808080u,NOVA_VECTOR_NONZERO)||
    pixels[0]!=0xa08080a0u){printf("pixel=%08lx\n",(unsigned long)pixels[0]);return 3;}
 nova_vector_path_reset(&p);if(!nova_vector_move_to(&p,0,0)||
    !nova_vector_quad_to(&p,1<<16,2<<16,2<<16,0)||
    !nova_vector_cubic_to(&p,3<<16,-(2<<16),4<<16,2<<16,5<<16,0)||
    p.count!=40||p.overflow)return 2;
 puts("VECTOR_GEOMETRY_RUNTIME_OK");return 0;}
