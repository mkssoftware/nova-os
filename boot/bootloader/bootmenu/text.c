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

#define NOVA_TEXT_LAYOUT_CACHE_CAPACITY 24u
#define NOVA_TEXT_LAYOUT_CACHE_TEXT_CAPACITY 192u
typedef struct {
    bool used;
    char text[NOVA_TEXT_LAYOUT_CACHE_TEXT_CAPACITY];
    int32_t available_width,letter_spacing_dlu;
    uint32_t scale_milli,generation;
    uint16_t maximum_lines;
    uint8_t wrap,ellipsis,accessibility;
    nova_text_layout_metrics_t metrics;
} nova_text_layout_cache_entry_t;
static nova_text_layout_cache_entry_t text_layout_cache[NOVA_TEXT_LAYOUT_CACHE_CAPACITY];
static nova_text_layout_diagnostics_t text_layout_diagnostics;
static uint8_t text_layout_cache_cursor;

static bool text_equal(const char *left,const char *right)
{if(!left||!right)return left==right;while(*left&&*left==*right){++left;++right;}return *left==*right;}
static bool text_copy(char *destination,uint32_t capacity,const char *source)
{uint32_t i=0;if(!destination||!capacity||!source)return false;
 while(source[i]&&i+1<capacity){destination[i]=source[i];++i;}
 destination[i]=0;return source[i]==0;}

void nova_text_layout_initialize(void)
{
    for(uint8_t i=0;i<NOVA_TEXT_LAYOUT_CACHE_CAPACITY;++i)
        text_layout_cache[i]=(nova_text_layout_cache_entry_t){0};
    text_layout_diagnostics=(nova_text_layout_diagnostics_t){0};
    text_layout_cache_cursor=0;
}

void nova_text_layout_begin(uint32_t generation)
{
    if(text_layout_diagnostics.generation!=generation){
        text_layout_diagnostics.generation=generation;
        ++text_layout_diagnostics.invalidations;
    }
}

static bool text_layout_key_matches(const nova_text_layout_cache_entry_t *entry,
 const nova_text_layout_request_t *request)
{
 return entry->used&&entry->generation==text_layout_diagnostics.generation&&
    entry->available_width==request->available_width&&
    entry->letter_spacing_dlu==request->letter_spacing_dlu&&
    entry->scale_milli==request->scale_milli&&entry->maximum_lines==request->maximum_lines&&
    entry->wrap==(uint8_t)request->wrap&&entry->ellipsis==(uint8_t)request->ellipsis&&
    entry->accessibility==(uint8_t)request->accessibility&&text_equal(entry->text,request->utf8);
}

bool nova_text_layout_measure(const nova_text_layout_request_t *request,
                              nova_text_layout_metrics_t *metrics)
{
 if(!request||!metrics||!request->utf8||request->available_width<0||
    request->scale_milli<500||request->scale_milli>4000||
    request->wrap>NOVA_TEXT_WRAP_NONE||request->ellipsis>NOVA_TEXT_ELLIPSIS_START){
    ++text_layout_diagnostics.errors;return false;}
 for(uint8_t i=0;i<NOVA_TEXT_LAYOUT_CACHE_CAPACITY;++i)
    if(text_layout_key_matches(&text_layout_cache[i],request)){
       *metrics=text_layout_cache[i].metrics;++text_layout_diagnostics.cache_hits;return true;}
 ++text_layout_diagnostics.cache_misses;
 uint32_t scale=request->scale_milli;
 if(request->accessibility)scale=scale<=3200?(scale*5u)/4u:4000u;
 int32_t spacing=(request->letter_spacing_dlu*(int32_t)scale+500)/1000;
 int32_t limit=request->available_width;
 uint16_t maximum_lines=request->maximum_lines?request->maximum_lines:0xffffu;
 nova_text_layout_metrics_t result={0};
 result.line_height=(NOVA_FONT_HEIGHT*(int32_t)scale+500)/1000;
 result.ascender=(15*(int32_t)scale+500)/1000;
 result.descender=result.line_height-result.ascender;
 result.baseline=result.ascender;
 result.word_spacing=(glyph_advance(' ')*(int32_t)scale+500)/1000+spacing;
 result.lines=1;
 int32_t line=0,natural_line=0,current_word=0,max_word=0,max_glyph=0;
 const char *cursor=request->utf8;
 while(*cursor){
    uint32_t cp;bool valid=nova_unicode_next(&cursor,&cp);
    if(!valid){result.invalid_unicode=true;++text_layout_diagnostics.invalid_unicode;}
    if(cp=='\n'){
       if(line>result.width)result.width=line;
       if(natural_line>result.maximum_width)result.maximum_width=natural_line;
       if(current_word>max_word)max_word=current_word;
       line=natural_line=current_word=0;
       if(result.lines==maximum_lines){result.truncated=*cursor!=0;break;}
       ++result.lines;continue;
    }
    bool fallback=false;
    uint8_t code=nova_unicode_latin_glyph_code(cp,&fallback);
    if(!nova_font_resource_resolve(nova_font_theme_get(NOVA_FONT_ROLE_PRIMARY),cp)||fallback)
        result.fallback_font=true;
    int32_t advance=(glyph_advance(code)*(int32_t)scale+500)/1000+spacing;
    if(advance<1)advance=1;
    if(advance>max_glyph)max_glyph=advance;
    natural_line+=advance;++result.glyphs;
    if(cp==' '||cp=='\t'){
       if(current_word>max_word)max_word=current_word;
       current_word=0;
    }else current_word+=advance;
    if(limit>0&&request->wrap!=NOVA_TEXT_WRAP_NONE&&line+advance>limit){
       if(result.lines==maximum_lines){result.truncated=true;break;}
       if(request->wrap==NOVA_TEXT_WRAP_WORD&&current_word>advance&&line>=current_word-advance){
          int32_t completed=line-(current_word-advance);
          if(completed>result.width)result.width=completed;
          line=current_word;
       }else{if(line>result.width)result.width=line;line=advance;}
       ++result.lines;++text_layout_diagnostics.wraps;
    }else line+=advance;
 }
 if(current_word>max_word)max_word=current_word;
 if(line>result.width)result.width=line;
 if(natural_line>result.maximum_width)result.maximum_width=natural_line;
 if(request->wrap==NOVA_TEXT_WRAP_WORD)result.minimum_width=max_word;
 else if(request->wrap==NOVA_TEXT_WRAP_CHARACTER)result.minimum_width=max_glyph;
 else result.minimum_width=result.maximum_width;
 if(limit>0&&request->wrap==NOVA_TEXT_WRAP_NONE&&result.width>limit){
    result.width=limit;result.truncated=true;
 }
 result.height=result.line_height*result.lines;
 if(result.truncated)++text_layout_diagnostics.truncations;
 if(result.fallback_font)++text_layout_diagnostics.fallback_fonts;
 *metrics=result;++text_layout_diagnostics.measures;
 nova_text_layout_cache_entry_t *entry=&text_layout_cache[text_layout_cache_cursor];
 if(text_copy(entry->text,sizeof(entry->text),request->utf8)){
    entry->used=true;entry->generation=text_layout_diagnostics.generation;
    entry->available_width=request->available_width;
    entry->letter_spacing_dlu=request->letter_spacing_dlu;
    entry->scale_milli=request->scale_milli;entry->maximum_lines=request->maximum_lines;
    entry->wrap=(uint8_t)request->wrap;entry->ellipsis=(uint8_t)request->ellipsis;
    entry->accessibility=(uint8_t)request->accessibility;entry->metrics=result;
    text_layout_cache_cursor=(uint8_t)((text_layout_cache_cursor+1)%NOVA_TEXT_LAYOUT_CACHE_CAPACITY);
 }
 return true;
}

const nova_text_layout_diagnostics_t *nova_text_layout_diagnostics(void)
{return &text_layout_diagnostics;}

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
    nova_text_draw_ellipsized_scaled(surface,x,y,max_width,text,color,alignment,
        ellipsis?NOVA_TEXT_ELLIPSIS_END:NOVA_TEXT_ELLIPSIS_NONE,scale_milli);
}

void nova_text_draw_ellipsized_scaled(nova_surface_t *surface,int32_t x,int32_t y,
 int32_t max_width,const char *text,uint32_t color,nova_text_align_t alignment,
 nova_text_ellipsis_mode_t ellipsis,uint32_t scale_milli)
{
    if (!surface || !text || max_width <= 0 || alignment>NOVA_TEXT_RIGHT ||
        ellipsis>NOVA_TEXT_ELLIPSIS_START) return;
    if (scale_milli < 500) scale_milli=500;
    if (scale_milli > 3000) scale_milli=3000;
    uint8_t codes[192];int32_t advances[192];uint16_t count=0;
    int32_t natural_width=0;
    const char *read=text;
    while(*read&&count<192){uint32_t cp;nova_unicode_next(&read,&cp);if(cp=='\n')break;
        uint8_t code=font_code(cp);int32_t advance=
            (glyph_advance(code)*(int32_t)scale_milli+500)/1000;
        codes[count]=code;advances[count]=advance;natural_width+=advance;++count;}
    bool truncated=*read||natural_width>max_width;
    int32_t dot_advance=(glyph_advance('.')*(int32_t)scale_milli+500)/1000;
    int32_t dots_width=dot_advance*3,rendered_width=natural_width;
    uint16_t prefix=count,suffix=count;
    if(truncated){
        int32_t available=ellipsis==NOVA_TEXT_ELLIPSIS_NONE?max_width:max_width-dots_width;
        if(available<0)available=0;
        if(ellipsis==NOVA_TEXT_ELLIPSIS_START){
            int32_t used=0;suffix=count;
            while(suffix&&used+advances[suffix-1]<=available){--suffix;used+=advances[suffix];}
            prefix=0;rendered_width=used+dots_width;
        }else if(ellipsis==NOVA_TEXT_ELLIPSIS_MIDDLE){
            int32_t left_budget=available/2,left=0,right=0;prefix=0;suffix=count;
            while(prefix<count&&left+advances[prefix]<=left_budget){left+=advances[prefix++];}
            while(suffix>prefix&&left+right+advances[suffix-1]<=available){
                --suffix;right+=advances[suffix];}
            rendered_width=left+right+dots_width;
        }else{
            int32_t used=0;prefix=0;
            while(prefix<count&&used+advances[prefix]<=available){used+=advances[prefix++];}
            suffix=count;rendered_width=used+(ellipsis==NOVA_TEXT_ELLIPSIS_END?dots_width:0);
        }
        if(rendered_width>max_width)rendered_width=max_width;
    }
    int32_t cursor = x;
    if (alignment == NOVA_TEXT_CENTER) cursor += (max_width-rendered_width)/2;
    else if (alignment == NOVA_TEXT_RIGHT) cursor += max_width-rendered_width;
    if(truncated&&ellipsis==NOVA_TEXT_ELLIPSIS_START)
        for(uint8_t i=0;i<3;++i){draw_glyph_scaled(surface,cursor,y,'.',color,scale_milli);cursor+=dot_advance;}
    for(uint16_t i=0;i<prefix;++i){if(codes[i]!=' ')draw_glyph_scaled(surface,cursor,y,codes[i],color,scale_milli);cursor+=advances[i];}
    if(truncated&&(ellipsis==NOVA_TEXT_ELLIPSIS_END||ellipsis==NOVA_TEXT_ELLIPSIS_MIDDLE))
        for(uint8_t i=0;i<3;++i){draw_glyph_scaled(surface,cursor,y,'.',color,scale_milli);cursor+=dot_advance;}
    if(truncated&&(ellipsis==NOVA_TEXT_ELLIPSIS_START||ellipsis==NOVA_TEXT_ELLIPSIS_MIDDLE)){
        for(uint16_t i=suffix;i<count;++i){if(codes[i]!=' ')draw_glyph_scaled(surface,cursor,y,codes[i],color,scale_milli);cursor+=advances[i];}
    }
    nova_damage_add(surface,(nova_rect_t){x,y,max_width,
        (int32_t)((NOVA_FONT_HEIGHT*scale_milli+500)/1000)});
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
