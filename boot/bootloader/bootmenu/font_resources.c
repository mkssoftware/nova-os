#include "font_resources.h"
#include "resources.h"

static nova_font_resource_t fonts[NOVA_FONT_RESOURCE_CAPACITY];
static int8_t slots[32];static nova_font_resource_diagnostics_t diagnostics;

static void copy_text(char *destination,uint8_t capacity,const char *source)
{uint8_t i=0;for(;source&&source[i]&&i+1<capacity;++i)destination[i]=source[i];
 destination[i]=0;}
static int8_t lookup(uint64_t id)
{if(!id||!diagnostics.initialized)return -1;uint8_t slot=(uint8_t)id&31u;
 for(uint8_t probe=0;probe<32;++probe){int8_t index=slots[(slot+probe)&31u];
  if(index<0)return -1;
  if(fonts[(uint8_t)index].valid&&fonts[(uint8_t)index].font_id==id)return index;}
 return -1;}
static bool insert(uint64_t id,uint8_t index)
{uint8_t slot=(uint8_t)id&31u;for(uint8_t probe=0;probe<32;++probe){int8_t *entry=&slots[(slot+probe)&31u];
 if(*entry<0){*entry=(int8_t)index;return true;}}return false;}

bool nova_font_resource_initialize(void)
{for(uint8_t i=0;i<NOVA_FONT_RESOURCE_CAPACITY;++i)fonts[i]=(nova_font_resource_t){0};
 for(uint8_t i=0;i<32;++i)slots[i]=-1;
 diagnostics=(nova_font_resource_diagnostics_t){0};
 diagnostics.initialized=true;diagnostics.initializations=1;return true;}

bool nova_font_resource_register(const nova_font_resource_descriptor_t *descriptor)
{
    if(!diagnostics.initialized||!descriptor||!descriptor->font_id||!descriptor->resource_id||
       !descriptor->name||!*descriptor->name||!descriptor->family||!*descriptor->family||
       !descriptor->style||!*descriptor->style||!descriptor->version||
       !descriptor->resource_version||descriptor->family_type>NOVA_FONT_FAMILY_THEME||
       (descriptor->weight!=NOVA_FONT_WEIGHT_LIGHT&&descriptor->weight!=NOVA_FONT_WEIGHT_REGULAR&&
        descriptor->weight!=NOVA_FONT_WEIGHT_MEDIUM&&descriptor->weight!=NOVA_FONT_WEIGHT_SEMIBOLD&&
        descriptor->weight!=NOVA_FONT_WEIGHT_BOLD)||!descriptor->coverage||
       !descriptor->coverage_count||descriptor->coverage_count>NOVA_FONT_COVERAGE_CAPACITY){
        ++diagnostics.invalid_fonts;return false;}
    if(lookup(descriptor->font_id)>=0){++diagnostics.duplicates;return false;}
    const nova_resource_t *resource=nova_resource_find(descriptor->resource_id);
    if(!resource||resource->type!=NOVA_RESOURCE_FONT||resource->version!=descriptor->resource_version){
        ++diagnostics.invalid_fonts;return false;}
    uint32_t previous=0;for(uint8_t i=0;i<descriptor->coverage_count;++i){
        if(descriptor->coverage[i].first>descriptor->coverage[i].last||
           (i&&descriptor->coverage[i].first<=previous)){++diagnostics.invalid_fonts;return false;}
        previous=descriptor->coverage[i].last;}
    for(uint8_t i=0;i<NOVA_FONT_RESOURCE_CAPACITY;++i)if(!fonts[i].valid){
        nova_font_resource_t font={.font_id=descriptor->font_id,.resource_id=descriptor->resource_id,
            .fallback_id=descriptor->fallback_id,.version=descriptor->version,
            .resource_version=descriptor->resource_version,.family_type=descriptor->family_type,
            .weight=descriptor->weight,.priority=descriptor->priority,
            .coverage_count=descriptor->coverage_count,.valid=true};
        copy_text(font.name,sizeof(font.name),descriptor->name);
        copy_text(font.family,sizeof(font.family),descriptor->family);
        copy_text(font.style,sizeof(font.style),descriptor->style);
        for(uint8_t range=0;range<font.coverage_count;++range)font.coverage[range]=descriptor->coverage[range];
        if(!insert(font.font_id,i))return false;
        fonts[i]=font;++diagnostics.registered;return true;}
    ++diagnostics.invalid_fonts;return false;
}

const nova_font_resource_t *nova_font_resource_find(uint64_t id)
{int8_t index=lookup(id);return index<0?0:&fonts[(uint8_t)index];}
bool nova_font_resource_validate(const nova_font_resource_t *font)
{return font&&font->valid&&font->font_id&&font->resource_id&&font->version&&font->coverage_count;}
const nova_font_resource_t *nova_font_resource_load(uint64_t id)
{int8_t index=lookup(id);if(index<0)return 0;nova_font_resource_t *font=&fonts[(uint8_t)index];
 if(font->loaded){if(!font->references&&!nova_resource_load(font->resource_id))return 0;
  ++font->references;++diagnostics.cache_hits;return font;}
 const nova_resource_t *resource=nova_resource_load(font->resource_id);
 if(!resource||resource->type!=NOVA_RESOURCE_FONT||resource->version!=font->resource_version)return 0;
 font->loaded=true;font->references=1;++diagnostics.loads;return font;}
bool nova_font_resource_release(uint64_t id)
{int8_t index=lookup(id);if(index<0||!fonts[(uint8_t)index].references)return false;
 nova_font_resource_t *font=&fonts[(uint8_t)index];--font->references;++diagnostics.releases;
 return font->references?true:nova_resource_release(font->resource_id);}
bool nova_font_resource_has_glyph(const nova_font_resource_t *font,uint32_t codepoint)
{if(!nova_font_resource_validate(font))return false;
 for(uint8_t i=0;i<font->coverage_count;++i){if(codepoint<font->coverage[i].first)return false;
  if(codepoint<=font->coverage[i].last)return true;}return false;}
const nova_font_resource_t *nova_font_resource_resolve(uint64_t primary,uint32_t codepoint)
{
    ++diagnostics.resolutions;uint64_t visited[NOVA_FONT_RESOURCE_CAPACITY]={0};uint8_t count=0;
    uint64_t current=primary?primary:diagnostics.active_roles[NOVA_FONT_ROLE_PRIMARY];
    while(current&&count<NOVA_FONT_RESOURCE_CAPACITY){for(uint8_t i=0;i<count;++i)if(visited[i]==current){
            ++diagnostics.cycles;current=0;break;}if(!current)break;visited[count++]=current;
        const nova_font_resource_t *font=nova_font_resource_find(current);
        if(font&&nova_font_resource_has_glyph(font,codepoint))return font;
        ++diagnostics.fallback_steps;current=font?font->fallback_id:0;}
    uint64_t fallbacks[2]={diagnostics.active_roles[NOVA_FONT_ROLE_FALLBACK],NOVA_SYSTEM_FONT_ID};
    for(uint8_t i=0;i<2;++i){const nova_font_resource_t *font=nova_font_resource_find(fallbacks[i]);
        if(font&&nova_font_resource_has_glyph(font,codepoint))return font;}
    ++diagnostics.missing_glyphs;return 0;
}
bool nova_font_theme_set(nova_font_role_t role,uint64_t id)
{if(role>=NOVA_FONT_ROLE_COUNT||!nova_font_resource_find(id))return false;
 diagnostics.active_roles[role]=id;return true;}
uint64_t nova_font_theme_get(nova_font_role_t role)
{return role<NOVA_FONT_ROLE_COUNT?diagnostics.active_roles[role]:0;}
const nova_font_resource_diagnostics_t *nova_font_resource_diagnostics(void){return &diagnostics;}
