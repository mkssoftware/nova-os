#ifndef NOVA_BOOT_LAYOUT_H
#define NOVA_BOOT_LAYOUT_H

#include <stdint.h>
#include <stdbool.h>
#include "compositor.h"

typedef enum { NOVA_LAYOUT_COMPACT, NOVA_LAYOUT_STANDARD, NOVA_LAYOUT_WIDE }
    nova_layout_class_t;
typedef struct {
    uint32_t min_width, min_height, preferred_width, preferred_height,
             max_width, max_height;
} nova_size_constraints_t;
typedef struct {
    nova_rect_t safe, panel, top_bar, title, status, brand, list;
    int32_t item_height, item_gap, icon_size, item_padding, text_gap;
    uint32_t scale_milli, dpi;
    nova_layout_class_t layout_class;
    bool brand_compact, accessibility;
} nova_boot_layout_t;
typedef struct {
    uint32_t measures, arranges, finalizes, invalidations, constraint_errors,
             fallbacks;
    uint32_t width, height, scale_milli;
    nova_layout_class_t layout_class;
} nova_layout_diagnostics_t;

void nova_layout_initialize(void);
int32_t nova_dlu_to_pixels(int32_t dlu, uint32_t scale_milli);
bool nova_constraints_resolve(const nova_size_constraints_t *constraints,
                              uint32_t available_width, uint32_t available_height,
                              uint32_t *width, uint32_t *height);
bool nova_layout_compute(uint32_t width, uint32_t height, bool accessibility,
                         nova_boot_layout_t *layout);
const nova_layout_diagnostics_t *nova_layout_diagnostics(void);

#endif
