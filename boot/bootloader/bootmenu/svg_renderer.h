#ifndef NOVA_SVG_RENDERER_H
#define NOVA_SVG_RENDERER_H
#include <stdbool.h>
#include <stdint.h>
#include "compositor.h"
#define NOVA_SVG_SOURCE_CAPACITY 65536u
#define NOVA_SVG_ELEMENT_CAPACITY 64u
typedef struct {uint32_t accent,text,error;} nova_svg_theme_t;
typedef struct {uint32_t validations,renders,elements,paths,failures;bool initialized;}
 nova_svg_diagnostics_t;
bool nova_svg_initialize(void);
bool nova_svg_validate(const void *source,uint32_t source_size);
bool nova_svg_render(const void *source,uint32_t source_size,nova_surface_t *target,
 nova_rect_t destination,const nova_svg_theme_t *theme);
const nova_svg_diagnostics_t *nova_svg_diagnostics(void);
#endif
