#ifndef NOVA_BOOT_TEXT_H
#define NOVA_BOOT_TEXT_H

#include <stdint.h>
#include <stdbool.h>
#include "compositor.h"

typedef enum { NOVA_TEXT_LEFT, NOVA_TEXT_CENTER, NOVA_TEXT_RIGHT } nova_text_align_t;
typedef struct { int32_t width, height, lines; bool truncated; } nova_text_metrics_t;

typedef enum { NOVA_TEXT_WRAP_WORD, NOVA_TEXT_WRAP_CHARACTER, NOVA_TEXT_WRAP_NONE }
    nova_text_wrap_mode_t;
typedef enum { NOVA_TEXT_ELLIPSIS_NONE, NOVA_TEXT_ELLIPSIS_END,
    NOVA_TEXT_ELLIPSIS_MIDDLE, NOVA_TEXT_ELLIPSIS_START } nova_text_ellipsis_mode_t;
typedef struct {
    const char *utf8;
    int32_t available_width;
    int32_t letter_spacing_dlu;
    uint32_t scale_milli;
    uint16_t maximum_lines;
    nova_text_wrap_mode_t wrap;
    nova_text_ellipsis_mode_t ellipsis;
    bool accessibility;
} nova_text_layout_request_t;
typedef struct {
    int32_t minimum_width,maximum_width,width,height,line_height;
    int32_t ascender,descender,baseline,word_spacing;
    uint16_t lines,glyphs;
    bool truncated,fallback_font,invalid_unicode;
} nova_text_layout_metrics_t;
typedef struct {
    uint32_t measures,cache_hits,cache_misses,invalidations,wraps,truncations;
    uint32_t fallback_fonts,invalid_unicode,errors,generation;
} nova_text_layout_diagnostics_t;

void nova_text_layout_initialize(void);
void nova_text_layout_begin(uint32_t generation);
bool nova_text_layout_measure(const nova_text_layout_request_t *request,
                              nova_text_layout_metrics_t *metrics);
const nova_text_layout_diagnostics_t *nova_text_layout_diagnostics(void);

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
void nova_text_draw_ellipsized_scaled(nova_surface_t *surface,int32_t x,int32_t y,
    int32_t max_width,const char *utf8,uint32_t color,nova_text_align_t alignment,
    nova_text_ellipsis_mode_t ellipsis,uint32_t scale_milli);
uint16_t nova_text_draw_wrapped_scaled(nova_surface_t *surface, int32_t x, int32_t y,
                                       int32_t max_width, uint16_t max_lines,
                                       const char *utf8, uint32_t color,
                                       uint32_t scale_milli);
bool nova_text_register_font_resource(void);

#endif
