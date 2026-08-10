#include "theme.h"
#include "resources.h"
#include "motion.h"
#include "compositor.h"

static const nova_theme_tokens_t themes[NOVA_THEME_COUNT] = {
    [NOVA_THEME_DARK] = {
        0xff101113u, 0xff191b1eu, 0xff22262au, 0xfff4f6f8u,
        0xffaeb2b7u, 0xff267cc1u, 0xff4cc2ffu, 0xd022262au,
        0xff3a3e43u, 0xff66686au, 0xff44b878u, 0xffffb547u,
        0xffc52b58u, 0xff4cc2ffu, 4, 8, 8, 16, 210, false, false, false
    },
    [NOVA_THEME_LIGHT] = {
        0xfff2f4f7u, 0xffffffffu, 0xffe7ebf0u, 0xff17191cu,
        0xff555b63u, 0xff1769aau, 0xff005fccu, 0xd9d9e8f5u,
        0xffb2bac4u, 0xff8b929bu, 0xff16794bu, 0xff8a5700u,
        0xffa3123fu, 0xff005fccu, 4, 8, 8, 16, 235, false, false, false
    },
    [NOVA_THEME_HIGH_CONTRAST] = {
        0xff000000u, 0xff000000u, 0xff101010u, 0xffffffffu,
        0xffffffffu, 0xff00b7ffu, 0xffffff00u, 0xff003f75u,
        0xffffffffu, 0xffb0b0b0u, 0xff00ff70u, 0xffffff00u,
        0xffff4060u, 0xff00ffffu, 0, 2, 10, 18, 255, true, true, true
    }
};
static nova_theme_id_t active;
static nova_theme_resource_t registry[NOVA_THEME_COUNT];
#define NOVA_THEME_HASH_CAPACITY 8u
#define NOVA_THEME_TOKEN_COUNT 22u
static int8_t theme_slots[NOVA_THEME_HASH_CAPACITY];
static bool reduced_override;
static nova_theme_diagnostics_t diagnostics;

static bool theme_hash_insert(uint64_t id,uint8_t index)
{
    uint8_t slot=(uint8_t)id&(NOVA_THEME_HASH_CAPACITY-1u);
    for(uint8_t probe=0;probe<NOVA_THEME_HASH_CAPACITY;++probe){
        int8_t *entry=&theme_slots[(slot+probe)&(NOVA_THEME_HASH_CAPACITY-1u)];
        if(*entry<0){*entry=(int8_t)index;return true;}
        if(registry[(uint8_t)*entry].theme_id==id)return false;
    }
    return false;
}

static uint32_t brightness(uint32_t color)
{
    return (((color >> 16) & 255u) * 299u + ((color >> 8) & 255u) * 587u +
            (color & 255u) * 114u) / 1000u;
}

bool nova_theme_validate(nova_theme_id_t id)
{
    if (id >= NOVA_THEME_COUNT) return false;
    const nova_theme_tokens_t *t = &themes[id];
    uint32_t bg = brightness(t->background), fg = brightness(t->text_primary);
    uint32_t difference = bg > fg ? bg - fg : fg - bg;
    return t->background && t->surface && t->text_primary && t->accent &&
           t->focus && difference >= 100u;
}

bool nova_theme_initialize(void)
{
    diagnostics = (nova_theme_diagnostics_t){0};
    reduced_override = false;
    active=NOVA_THEME_COUNT;
    for(uint8_t i=0;i<NOVA_THEME_COUNT;++i)registry[i]=(nova_theme_resource_t){0};
    for(uint8_t i=0;i<NOVA_THEME_HASH_CAPACITY;++i)theme_slots[i]=-1;
    for (uint32_t i = 0; i < NOVA_THEME_COUNT; ++i) {
        if (!nova_theme_validate((nova_theme_id_t)i)) {
            ++diagnostics.validation_errors;
            return false;
        }
    }
    static const char *const uris[NOVA_THEME_COUNT]={"boot://themes/dark",
        "boot://themes/light","boot://themes/high-contrast"};
    static const char *const names[NOVA_THEME_COUNT]={"NovaOS Dark","NovaOS Light",
        "NovaOS High Contrast"};
    if (!nova_resource_register(uris[0], NOVA_RESOURCE_THEME, 1,
                                &themes[0], sizeof(themes[0]), 0, 0) ||
        !nova_resource_register(uris[1], NOVA_RESOURCE_THEME, 1,
                                &themes[1], sizeof(themes[1]), 0, 0) ||
        !nova_resource_register(uris[2], NOVA_RESOURCE_THEME, 1,
                                &themes[2], sizeof(themes[2]), 0, 0)) return false;
    for(uint8_t i=0;i<NOVA_THEME_COUNT;++i){uint64_t id=nova_resource_id(uris[i]);
        registry[i]=(nova_theme_resource_t){.theme_id=id,.resource_id=id,.name=names[i],
            .author="NovaOS Architecture",.version=1,.resource_version=1,
            .token_count=NOVA_THEME_TOKEN_COUNT,.tokens=&themes[i],.valid=true};
        if(!theme_hash_insert(id,i)){registry[i].valid=false;
            ++diagnostics.duplicate_ids;return false;}
        ++diagnostics.registered;}
    diagnostics.active = active;
    return true;
}

const nova_theme_resource_t *nova_theme_resource_find(uint64_t id)
{
    if(!id)return 0;
    uint8_t slot=(uint8_t)id&(NOVA_THEME_HASH_CAPACITY-1u);
    for(uint8_t probe=0;probe<NOVA_THEME_HASH_CAPACITY;++probe){
        int8_t index=theme_slots[(slot+probe)&(NOVA_THEME_HASH_CAPACITY-1u)];
        if(index<0)return 0;
        if(registry[(uint8_t)index].valid&&registry[(uint8_t)index].theme_id==id)
            return &registry[(uint8_t)index];
    }
    return 0;
}
const nova_theme_resource_t *nova_theme_resource_load(nova_theme_id_t id)
{
    if(id>=NOVA_THEME_COUNT||!registry[id].valid){++diagnostics.missing_resources;return 0;}
    nova_theme_resource_t *theme=&registry[id];
    if(theme->loaded){if(!theme->references&&!nova_resource_load(theme->resource_id))return 0;
        ++theme->references;++diagnostics.cache_hits;return theme;}
    const nova_resource_t *resource=nova_resource_load(theme->resource_id);
    if(!resource||resource->type!=NOVA_RESOURCE_THEME||resource->version!=theme->resource_version||
       resource->size!=sizeof(nova_theme_tokens_t)||resource->data!=theme->tokens){
        ++diagnostics.missing_resources;return 0;}
    theme->loaded=true;theme->references=1;++diagnostics.loads;return theme;
}
bool nova_theme_resource_release(nova_theme_id_t id)
{if(id>=NOVA_THEME_COUNT||!registry[id].references||
    (id==active&&registry[id].references==1))return false;
 nova_theme_resource_t *theme=&registry[id];--theme->references;++diagnostics.releases;
 return theme->references?true:nova_resource_release(theme->resource_id);}
const nova_theme_resource_t *nova_theme_current(void)
{return active<NOVA_THEME_COUNT?&registry[active]:0;}

bool nova_theme_activate(nova_theme_id_t id)
{
    if (!nova_theme_validate(id)) { ++diagnostics.validation_errors; return false; }
    nova_theme_id_t previous=active;
    const nova_theme_resource_t *loaded=nova_theme_resource_load(id);
    if(!loaded)return false;
    if(previous==id){(void)nova_theme_resource_release(id);return true;}
    active = id;
    diagnostics.active = id;
    ++diagnostics.activations;
    if (themes[id].high_contrast) ++diagnostics.accessibility_overrides;
    nova_motion_set_reduced(themes[id].reduced_motion || reduced_override);
    nova_compositor_set_fallback(themes[id].opaque_materials ? 3 : 0);
    if(previous<NOVA_THEME_COUNT)(void)nova_theme_resource_release(previous);
    ++diagnostics.atomic_switches;
    return true;
}

nova_theme_id_t nova_theme_active(void) { return active; }
const nova_theme_tokens_t *nova_theme_tokens(void)
{ return &themes[active<NOVA_THEME_COUNT?active:NOVA_THEME_DARK]; }
void nova_theme_set_reduced_motion(bool enabled)
{
    reduced_override = enabled;
    nova_motion_set_reduced((active<NOVA_THEME_COUNT&&themes[active].reduced_motion)||enabled);
    ++diagnostics.accessibility_overrides;
}
bool nova_theme_reduced_motion(void)
{ return (active<NOVA_THEME_COUNT&&themes[active].reduced_motion)||reduced_override; }
const nova_theme_diagnostics_t *nova_theme_diagnostics(void) { return &diagnostics; }
