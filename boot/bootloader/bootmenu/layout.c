#include "layout.h"
#include "resolution.h"

static nova_layout_diagnostics_t diagnostics;
static int32_t clamp_i32(int32_t value, int32_t minimum, int32_t maximum)
{
    if (value < minimum) return minimum;
    if (value > maximum) return maximum;
    return value;
}

void nova_layout_initialize(void) { diagnostics = (nova_layout_diagnostics_t){0}; }

int32_t nova_dlu_to_pixels(int32_t dlu, uint32_t scale_milli)
{
    int64_t product = (int64_t)dlu * scale_milli;
    if (product >= 0) return (int32_t)((product + 500) / 1000);
    return (int32_t)((product - 500) / 1000);
}

bool nova_constraints_resolve(const nova_size_constraints_t *c,
                              uint32_t available_width, uint32_t available_height,
                              uint32_t *width, uint32_t *height)
{
    if (!c || !width || !height || c->min_width > c->max_width ||
        c->min_height > c->max_height) { ++diagnostics.constraint_errors; return false; }
    uint32_t w = c->preferred_width, h = c->preferred_height;
    if (w < c->min_width) w = c->min_width;
    if (h < c->min_height) h = c->min_height;
    if (w > c->max_width) w = c->max_width;
    if (h > c->max_height) h = c->max_height;
    if (w > available_width) w = available_width;
    if (h > available_height) h = available_height;
    if (w < c->min_width || h < c->min_height) {
        ++diagnostics.constraint_errors;
        return false;
    }
    *width = w; *height = h;
    return true;
}

bool nova_layout_compute(uint32_t width, uint32_t height, bool accessibility,
                         nova_boot_layout_t *out)
{
    if (!out || width < 320 || height < 240 || width > 7680 || height > 4320) {
        ++diagnostics.fallbacks;
        return false;
    }
    if(!nova_resolution_set_resolution(width,height)){++diagnostics.fallbacks;return false;}
    const nova_viewport_t *viewport=nova_resolution_viewport();
    ++diagnostics.invalidations; ++diagnostics.measures;
    uint32_t margin_x=(uint32_t)viewport->safe_pixels.x;
    uint32_t margin_y=(uint32_t)viewport->safe_pixels.y;
    nova_layout_class_t cls = width < 900 || height < 600 ? NOVA_LAYOUT_COMPACT :
                              (width < 1600 ? NOVA_LAYOUT_STANDARD : NOVA_LAYOUT_WIDE);
    uint32_t scale=viewport->scale_milli;
    if (accessibility && scale < 1250) scale = 1250;
    int32_t safe_w = (int32_t)width - (int32_t)margin_x * 2;
    int32_t safe_h = (int32_t)height - (int32_t)margin_y * 2;
    int32_t status_h = nova_dlu_to_pixels(22, scale);
    int32_t title_h = nova_dlu_to_pixels(24, scale);
    int32_t panel_y = (int32_t)margin_y + title_h + 12;
    int32_t available_h = (int32_t)height - (int32_t)margin_y - status_h - panel_y - 8;
    int32_t panel_w = nova_dlu_to_pixels(567, scale);
    int32_t panel_h = nova_dlu_to_pixels(473, scale);
    if (cls == NOVA_LAYOUT_COMPACT) panel_w = safe_w;
    panel_w = clamp_i32(panel_w, safe_w < 480 ? safe_w : 480, safe_w);
    panel_h = clamp_i32(panel_h, 330, available_h);
    int32_t side_brand = nova_dlu_to_pixels(150, scale);
    bool compact_brand = panel_w + side_brand + 20 > safe_w;
    int32_t panel_x = compact_brand ? (int32_t)margin_x :
                      (int32_t)margin_x + (safe_w - panel_w + side_brand) / 2;
    if (panel_x + panel_w > (int32_t)width - (int32_t)margin_x)
        panel_x = (int32_t)width - (int32_t)margin_x - panel_w;
    int32_t inner_pad = nova_dlu_to_pixels(20, scale);
    int32_t gap = nova_dlu_to_pixels(6, scale);
    int32_t list_h = panel_h - inner_pad * 2;
    int32_t item_h = (list_h - gap * 5) / 6;
    item_h = clamp_i32(item_h, accessibility ? 48 : 42,
                       nova_dlu_to_pixels(62, scale));
    list_h = item_h * 6 + gap * 5;
    int32_t brand_w = compact_brand ? 64 : nova_dlu_to_pixels(120, scale);
    int32_t brand_h = brand_w * 15 / 16;
    int32_t brand_x = compact_brand ? (int32_t)margin_x : panel_x - brand_w - 20;
    int32_t brand_y = compact_brand ? (int32_t)margin_y : panel_y - 13;
    if (compact_brand) panel_y = (int32_t)margin_y + brand_h + title_h + 8;
    if (panel_y + panel_h > (int32_t)height - (int32_t)margin_y - status_h - 4)
        panel_h = (int32_t)height - (int32_t)margin_y - status_h - 4 - panel_y;
    list_h = panel_h - inner_pad * 2;
    item_h = (list_h - gap * 5) / 6;
    item_h = clamp_i32(item_h, 36, nova_dlu_to_pixels(62, scale));
    list_h = item_h * 6 + gap * 5;
    int32_t title_x = compact_brand ? brand_x + brand_w + 12 : panel_x + inner_pad;
    int32_t title_y = compact_brand ? brand_y + (brand_h - title_h) / 2 : panel_y - title_h;
    int32_t title_w = compact_brand ?
                      (int32_t)width - (int32_t)margin_x - title_x : panel_w - inner_pad * 2;

    *out = (nova_boot_layout_t){
        {(int32_t)margin_x,(int32_t)margin_y,safe_w,safe_h},
        {panel_x,panel_y,panel_w,panel_h},
        {(int32_t)margin_x,0,safe_w,clamp_i32(nova_dlu_to_pixels(13,scale),8,20)},
        {title_x,title_y,title_w,title_h},
        {(int32_t)margin_x,(int32_t)height-(int32_t)margin_y-status_h,safe_w,status_h},
        {brand_x,brand_y,brand_w,brand_h},
        {panel_x+inner_pad,panel_y+inner_pad,panel_w-inner_pad*2,list_h},
        item_h,gap,clamp_i32(nova_dlu_to_pixels(20,scale),16,32),
        clamp_i32(nova_dlu_to_pixels(18,scale),12,32),
        clamp_i32(nova_dlu_to_pixels(15,scale),10,24),
        scale,viewport->dpi,cls,compact_brand,accessibility
    };
    ++diagnostics.arranges; ++diagnostics.finalizes;
    diagnostics.width=width; diagnostics.height=height;
    diagnostics.scale_milli=scale; diagnostics.layout_class=cls;
    return out->panel.width > 0 && out->panel.height > 0 &&
           out->panel.x >= out->safe.x && out->panel.y >= out->safe.y &&
           out->panel.x + out->panel.width <= out->safe.x + out->safe.width &&
           out->status.y + out->status.height <= out->safe.y + out->safe.height;
}

const nova_layout_diagnostics_t *nova_layout_diagnostics(void) { return &diagnostics; }
