#include <stdio.h>
#include "../boot/bootloader/bootmenu/svg_renderer.h"
int main(void)
{static const char svg[]="<svg viewBox='0 0 8 8'><rect x='0' y='0' width='4' height='4' fill='#FF000080'/><circle cx='6' cy='6' r='2' fill='var(--nova-accent)'/></svg>";
 uint32_t pixels[16]={0};nova_surface_t surface={.pixels=pixels,.width=4,.height=4,.stride=4};
 if(!nova_svg_initialize()||!nova_svg_validate(svg,sizeof(svg)-1)||
    !nova_svg_render(svg,sizeof(svg)-1,&surface,(nova_rect_t){0,0,4,4},0))return 1;
 if(pixels[0]!=0x80800000u||pixels[15]==0)return 2;
 puts("SVG_RUNTIME_OK");return 0;}
