#include "text.h"
#include "boot_font_data.h"
#include "unicode.h"
#include "resources.h"
#include "font_resources.h"

bool nova_text_register_font_resource(void)
{
    static const nova_font_coverage_t coverage[]={
        {0x20,0x7e},{0x00c4,0x00c4},{0x00d6,0x00d6},{0x00dc,0x00dc},
        {0x00df,0x00df},{0x00e4,0x00e4},{0x00f6,0x00f6},{0x00fc,0x00fc}};
    const char *uri="boot://fonts/segoe-ui/semibold/15";
    if(!nova_resource_register(uri,NOVA_RESOURCE_FONT,1,nova_font_bitmap,
        sizeof(nova_font_bitmap),0,0)||
       !nova_resource_cache_set_policy(nova_resource_id(uri),NOVA_CACHE_PERMANENT)||
       !nova_font_resource_initialize())return false;
    nova_font_resource_descriptor_t descriptor={.font_id=NOVA_SYSTEM_FONT_ID,
        .resource_id=nova_resource_id(uri),.name="NovaOS UI 15",
        .family="Segoe UI",.style="SemiBold",.version=1,.resource_version=1,
        .family_type=NOVA_FONT_FAMILY_UI,.weight=NOVA_FONT_WEIGHT_SEMIBOLD,
        .priority=255,.coverage=coverage,
        .coverage_count=(uint8_t)(sizeof(coverage)/sizeof(coverage[0]))};
    if(!nova_font_resource_register(&descriptor))return false;
    for(uint8_t role=0;role<NOVA_FONT_ROLE_COUNT;++role)
        if(!nova_font_theme_set((nova_font_role_t)role,NOVA_SYSTEM_FONT_ID))return false;
    return true;
}

static uint8_t font_code(uint32_t codepoint)
{
    if(!nova_font_resource_resolve(nova_font_theme_get(NOVA_FONT_ROLE_PRIMARY),codepoint))
        return 0;
    return nova_unicode_latin_glyph_code(codepoint, 0);
}

static int32_t glyph_index(uint8_t code)
{
    for (uint32_t i = 0; i < NOVA_FONT_GLYPH_COUNT; ++i)
        if (nova_font_characters[i] == code) return (int32_t)i;
    return -1;
}

static uint32_t alpha_blend(uint32_t background, uint32_t foreground, uint8_t alpha)
{
    uint32_t inverse = 255u - alpha;
    uint32_t r = ((((foreground >> 16) & 255u) * alpha) +
                  (((background >> 16) & 255u) * inverse)) / 255u;
    uint32_t g = ((((foreground >> 8) & 255u) * alpha) +
                  (((background >> 8) & 255u) * inverse)) / 255u;
    uint32_t b = (((foreground & 255u) * alpha) + ((background & 255u) * inverse)) / 255u;
    return 0xff000000u | (r << 16) | (g << 8) | b;
}

static int32_t glyph_advance(uint8_t code)
{
    if (code == ' ') return 6;
    int32_t index = glyph_index(code);
    return index < 0 ? 8 : nova_font_widths[index] + 1;
}

static void draw_glyph(nova_surface_t *surface, int32_t x, int32_t y,
                       uint8_t code, uint32_t color);

nova_text_metrics_t nova_text_measure(const char *text, int32_t max_width)
{
    return nova_text_measure_scaled(text, max_width, 1000);
}

nova_text_metrics_t nova_text_measure_scaled(const char *text, int32_t max_width,
                                             uint32_t scale_milli)
{
    if (scale_milli < 500) scale_milli = 500;
    if (scale_milli > 3000) scale_milli = 3000;
    int32_t line_height = (int32_t)((NOVA_FONT_HEIGHT * scale_milli + 500) / 1000);
    nova_text_metrics_t metrics = {0, line_height, 1, false};
    int32_t line = 0;
    if (!text) return (nova_text_metrics_t){0};
    while (*text) {
        uint32_t cp; nova_unicode_next(&text, &cp);
        if (cp == '\n') { if (line > metrics.width) metrics.width = line; line = 0;
            ++metrics.lines; metrics.height += line_height; continue; }
        int32_t advance = (glyph_advance(font_code(cp)) * (int32_t)scale_milli + 500) / 1000;
        if (max_width > 0 && line + advance > max_width) { metrics.truncated = true; break; }
        line += advance;
    }
    if (line > metrics.width) metrics.width = line;
    return metrics;
}

static uint8_t glyph_coverage(const uint8_t *bitmap, uint32_t x, uint32_t y)
{
    if (x >= NOVA_FONT_WIDTH || y >= NOVA_FONT_HEIGHT) return 0;
    uint32_t pixel = y * NOVA_FONT_WIDTH + x;
    return (bitmap[pixel / 4] >> ((pixel & 3u) * 2)) & 3u;
}

static void draw_glyph_scaled(nova_surface_t *surface, int32_t x, int32_t y,
                              uint8_t code, uint32_t color, uint32_t scale_milli)
{
    if (scale_milli == 1000) { draw_glyph(surface,x,y,code,color); return; }
    int32_t index = glyph_index(code);
    if (index < 0) { draw_glyph(surface,x,y,code,color); return; }
    const uint8_t *bitmap = nova_font_bitmap + index * NOVA_FONT_BYTES_PER_GLYPH;
    uint32_t out_w = (NOVA_FONT_WIDTH * scale_milli + 999) / 1000;
    uint32_t out_h = (NOVA_FONT_HEIGHT * scale_milli + 999) / 1000;
    for (uint32_t dy=0;dy<out_h;++dy) for(uint32_t dx=0;dx<out_w;++dx) {
        uint32_t fx=dx*1000u/scale_milli, fy=dy*1000u/scale_milli;
        uint32_t rx=(dx*1000u%scale_milli)*256u/scale_milli;
        uint32_t ry=(dy*1000u%scale_milli)*256u/scale_milli;
        uint32_t c00=glyph_coverage(bitmap,fx,fy)*85u;
        uint32_t c10=glyph_coverage(bitmap,fx+1,fy)*85u;
        uint32_t c01=glyph_coverage(bitmap,fx,fy+1)*85u;
        uint32_t c11=glyph_coverage(bitmap,fx+1,fy+1)*85u;
        uint32_t top=(c00*(256u-rx)+c10*rx)/256u;
        uint32_t bottom=(c01*(256u-rx)+c11*rx)/256u;
        uint8_t alpha=(uint8_t)((top*(256u-ry)+bottom*ry)/256u);
        int32_t px=x+(int32_t)dx,py=y+(int32_t)dy;
        if(!alpha||px<0||py<0||(uint32_t)px>=surface->width||(uint32_t)py>=surface->height)continue;
        uint32_t *destination=&surface->pixels[py*surface->stride+px];
        *destination=alpha_blend(*destination,color,alpha);
    }
}

static void draw_glyph(nova_surface_t *surface, int32_t x, int32_t y,
                       uint8_t code, uint32_t color)
{
    int32_t index = glyph_index(code);
    if (index < 0) {
        nova_surface_rect(surface, (nova_rect_t){x, y + 3, 8, 1}, color);
        nova_surface_rect(surface, (nova_rect_t){x, y + 14, 8, 1}, color);
        nova_surface_rect(surface, (nova_rect_t){x, y + 3, 1, 12}, color);
        nova_surface_rect(surface, (nova_rect_t){x + 7, y + 3, 1, 12}, color);
        return;
    }
    const uint8_t *bitmap = nova_font_bitmap + index * NOVA_FONT_BYTES_PER_GLYPH;
    for (uint32_t py = 0; py < NOVA_FONT_HEIGHT; ++py) for (uint32_t px = 0; px < NOVA_FONT_WIDTH; ++px) {
        uint32_t pixel = py * NOVA_FONT_WIDTH + px;
        uint8_t coverage = (bitmap[pixel / 4] >> ((pixel & 3u) * 2)) & 3u;
        if (!coverage) continue;
        int32_t dx = x + (int32_t)px, dy = y + (int32_t)py;
        if (dx < 0 || dy < 0 || (uint32_t)dx >= surface->width || (uint32_t)dy >= surface->height) continue;
        uint32_t *destination = &surface->pixels[dy * surface->stride + dx];
        *destination = alpha_blend(*destination, color, (uint8_t)(coverage * 85u));
    }
}

void nova_text_draw(nova_surface_t *surface, int32_t x, int32_t y,
                    int32_t max_width, const char *text, uint32_t color,
                    nova_text_align_t alignment, bool ellipsis)
{
    nova_text_draw_scaled(surface,x,y,max_width,text,color,alignment,ellipsis,1000);
}

void nova_text_draw_scaled(nova_surface_t *surface, int32_t x, int32_t y,
                           int32_t max_width, const char *text, uint32_t color,
                           nova_text_align_t alignment, bool ellipsis,
                           uint32_t scale_milli)
{
    if (!surface || !text || max_width <= 0) return;
    if (scale_milli < 500) scale_milli=500;
    if (scale_milli > 3000) scale_milli=3000;
    nova_text_metrics_t metrics = nova_text_measure_scaled(text, max_width, scale_milli);
    int32_t cursor = x;
    if (alignment == NOVA_TEXT_CENTER) cursor += (max_width - metrics.width) / 2;
    else if (alignment == NOVA_TEXT_RIGHT) cursor += max_width - metrics.width;
    const char *read = text;
    while (*read) {
        uint32_t cp; nova_unicode_next(&read, &cp);
        uint8_t code = font_code(cp);
        int32_t advance = (glyph_advance(code)*(int32_t)scale_milli+500)/1000;
        if (cursor + advance > x + max_width) break;
        if (code != ' ') draw_glyph_scaled(surface,cursor,y,code,color,scale_milli);
        cursor += advance;
    }
    if (*read && ellipsis) {
        int32_t dot_advance=(5*(int32_t)scale_milli+500)/1000;
        for (uint32_t i=0;i<3&&cursor+dot_advance<=x+max_width;++i,cursor+=dot_advance)
            draw_glyph_scaled(surface,cursor,y,'.',color,scale_milli);
    }
    nova_damage_add(surface,(nova_rect_t){x,y,max_width,metrics.height});
}

uint16_t nova_text_draw_wrapped_scaled(nova_surface_t *surface, int32_t x, int32_t y,
                                       int32_t max_width, uint16_t max_lines,
                                       const char *utf8, uint32_t color,
                                       uint32_t scale_milli)
{
    if (!surface || !utf8 || max_width <= 0 || !max_lines) return 0;
    const char *cursor = utf8;
    uint16_t lines = 0;
    int32_t line_height = (20 * (int32_t)scale_milli + 500) / 1000;
    while (*cursor && lines < max_lines) {
        while (*cursor == ' ') ++cursor;
        const char *line_start = cursor;
        const char *best_end = cursor;
        const char *word_end = cursor;
        char candidate[192];
        while (*word_end) {
            while (*word_end && *word_end != ' ') ++word_end;
            uint32_t length = (uint32_t)(word_end - line_start);
            if (length >= sizeof(candidate)) length = sizeof(candidate) - 1;
            for (uint32_t i = 0; i < length; ++i) candidate[i] = line_start[i];
            candidate[length] = 0;
            if (nova_text_measure_scaled(candidate,max_width,scale_milli).truncated) break;
            best_end = word_end;
            while (*word_end == ' ') ++word_end;
        }
        if (best_end == line_start) {
            best_end = word_end;
            if (best_end == line_start) break;
        }
        uint32_t length = (uint32_t)(best_end - line_start);
        if (length >= sizeof(candidate)) length = sizeof(candidate) - 1;
        for (uint32_t i = 0; i < length; ++i) candidate[i] = line_start[i];
        candidate[length] = 0;
        const char *remaining = best_end;
        while (*remaining == ' ') ++remaining;
        bool last_with_more = lines + 1 == max_lines && *remaining;
        nova_text_draw_scaled(surface,x,y + lines * line_height,max_width,candidate,
                              color,NOVA_TEXT_LEFT,last_with_more,scale_milli);
        ++lines;
        cursor = remaining;
    }
    return lines;
}
