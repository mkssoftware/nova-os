#ifndef NOVA_VECTOR_GEOMETRY_H
#define NOVA_VECTOR_GEOMETRY_H
#include <stdint.h>
#include <stdbool.h>
#include "compositor.h"
#define NOVA_VECTOR_SEGMENT_CAPACITY 4096u
typedef struct {int32_t x,y;} nova_vector_point_t;
typedef struct {nova_vector_point_t a,b;} nova_vector_segment_t;
typedef enum {NOVA_VECTOR_NONZERO,NOVA_VECTOR_EVENODD} nova_vector_fill_rule_t;
typedef struct {nova_vector_segment_t segments[NOVA_VECTOR_SEGMENT_CAPACITY];
 uint16_t count;nova_vector_point_t current,first;bool open,overflow;} nova_vector_path_t;
void nova_vector_path_reset(nova_vector_path_t *path);
bool nova_vector_move_to(nova_vector_path_t *path,int32_t x,int32_t y);
bool nova_vector_line_to(nova_vector_path_t *path,int32_t x,int32_t y);
bool nova_vector_quad_to(nova_vector_path_t *path,int32_t cx,int32_t cy,int32_t x,int32_t y);
bool nova_vector_cubic_to(nova_vector_path_t *path,int32_t c1x,int32_t c1y,
 int32_t c2x,int32_t c2y,int32_t x,int32_t y);
bool nova_vector_close(nova_vector_path_t *path);
uint8_t nova_vector_coverage(const nova_vector_path_t *path,int32_t x,int32_t y,
 nova_vector_fill_rule_t rule);
bool nova_vector_fill(nova_surface_t *surface,const nova_vector_path_t *path,
 nova_rect_t clip,uint32_t premultiplied_color,nova_vector_fill_rule_t rule);
#endif
