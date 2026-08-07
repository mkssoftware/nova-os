#ifndef NOVA_BOOT_TEXT_H
#define NOVA_BOOT_TEXT_H

#include <stdint.h>
#include <stdbool.h>
#include "compositor.h"

typedef enum { NOVA_TEXT_LEFT, NOVA_TEXT_CENTER, NOVA_TEXT_RIGHT } nova_text_align_t;
typedef struct { int32_t width, height, lines; bool truncated; } nova_text_metrics_t;

nova_text_metrics_t nova_text_measure(const char *utf8, int32_t max_width);
nova_text_metrics_t nova_text_measure_scaled(const char *utf8, int32_t max_width,
                                             uint32_t scale_milli);
void nova_text_draw(nova_surface_t *surface, int32_t x, int32_t y,
                    int32_t max_width, const char *utf8, uint32_t color,
                    nova_text_align_t alignment, bool ellipsis);
void nova_text_draw_scaled(nova_surface_t *surface, int32_t x, int32_t y,
                           int32_t max_width, const char *utf8, uint32_t color,
                           nova_text_align_t alignment, bool ellipsis,
                           uint32_t scale_milli);
uint16_t nova_text_draw_wrapped_scaled(nova_surface_t *surface, int32_t x, int32_t y,
                                       int32_t max_width, uint16_t max_lines,
                                       const char *utf8, uint32_t color,
                                       uint32_t scale_milli);
bool nova_text_register_font_resource(void);

#endif
