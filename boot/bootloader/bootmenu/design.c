#include "design.h"
#include "resources.h"
#include "icons.h"
#include "motion.h"
#include "font_resources.h"

static nova_design_manifest_t manifest;
static nova_oem_design_t oem;
static nova_design_diagnostics_t diagnostics;
static nova_visual_continuity_context_t continuity;
static nova_visual_continuity_diagnostics_t continuity_diagnostics;
static const nova_typography_tokens_t typography={32,14,12,40,20,16,20,10,40,400,600};
static const nova_component_tokens_t components={48,12,16,16,48,8,2,24};
static const nova_design_motion_tokens_t motion_tokens={180,180,240,120,120,950,
    NOVA_EASE_OUT_CUBIC,NOVA_EASE_OUT_CUBIC};
static const nova_effect_tokens_t effects={12,1,18,0,6,210,12,96};
static const uint16_t spacing[NOVA_THEME_COUNT][NOVA_SPACING_COUNT]={
    {2,4,8,12,16,24,32,48},{2,4,8,12,16,24,32,48},{4,6,10,14,20,28,38,52}};
static const uint16_t radii[NOVA_THEME_COUNT][NOVA_RADIUS_COUNT]={
    {0,2,4,8,12,16,32767,32767},{0,2,4,8,12,16,32767,32767},
    {0,0,2,4,6,8,32767,32767}};
static const nova_shadow_style_t shadows[NOVA_THEME_COUNT][NOVA_ELEVATION_COUNT]={
 [NOVA_THEME_DARK]={
  {{{0,0,0,0,0}},0,true},{{{0,1,2,28,0xff000000u}},1,false},
  {{{0,2,4,42,0xff000000u},{0,1,2,24,0xff000000u}},2,false},
  {{{0,3,7,54,0xff000000u},{0,1,3,28,0xff000000u}},2,false},
  {{{0,4,10,72,0xff000000u},{0,1,4,36,0xff000000u}},2,false},
  {{{0,6,14,88,0xff000000u},{0,2,5,44,0xff000000u}},2,false}},
 [NOVA_THEME_LIGHT]={
  {{{0,0,0,0,0}},0,true},{{{0,1,2,24,0xff202830u}},1,false},
  {{{0,2,4,34,0xff202830u},{0,1,2,18,0xff202830u}},2,false},
  {{{0,3,7,44,0xff202830u},{0,1,3,24,0xff202830u}},2,false},
  {{{0,4,10,58,0xff202830u},{0,1,4,30,0xff202830u}},2,false},
  {{{0,6,14,70,0xff202830u},{0,2,5,36,0xff202830u}},2,false}},
 [NOVA_THEME_HIGH_CONTRAST]={
  {{{0,0,0,0,0}},0,true},{{{0,0,0,0,0}},0,true},{{{0,0,0,0,0}},0,true},
  {{{0,0,0,0,0}},0,true},{{{0,0,0,0,0}},0,true},{{{0,0,0,0,0}},0,true}}};
static const nova_material_style_t materials[NOVA_THEME_COUNT][NOVA_DESIGN_MATERIAL_COUNT]={
 [NOVA_THEME_DARK]={
  {0,0,0,0,0,100,NOVA_ELEVATION_NONE,true},
  {0xff191b1eu,0xff3a3e43u,0,255,0,100,NOVA_ELEVATION_LEVEL1,true},
  {0xd9191b1eu,0x803a3e43u,8,217,8,102,NOVA_ELEVATION_LEVEL2,false},
  {0xe6191b1eu,0x703a3e43u,4,230,6,103,NOVA_ELEVATION_LEVEL2,false},
  {0xd0191b1eu,0x903a3e43u,8,208,10,102,NOVA_ELEVATION_LEVEL3,false},
  {0xb8191b1eu,0xa03a3e43u,12,184,14,104,NOVA_ELEVATION_LEVEL4,false},
  {0xb0101113u,0x803a3e43u,10,176,8,95,NOVA_ELEVATION_LEVEL5,false},
  {0xe0191b1eu,0xa03a3e43u,12,224,12,102,NOVA_ELEVATION_LEVEL4,false}},
 [NOVA_THEME_LIGHT]={
  {0,0,0,0,0,100,NOVA_ELEVATION_NONE,true},
  {0xffffffffu,0xffb2bac4u,0,255,0,100,NOVA_ELEVATION_LEVEL1,true},
  {0xe6ffffffu,0x80b2bac4u,6,230,8,100,NOVA_ELEVATION_LEVEL2,false},
  {0xf0ffffffu,0x70b2bac4u,3,240,5,101,NOVA_ELEVATION_LEVEL2,false},
  {0xdcffffffu,0x90b2bac4u,7,220,9,100,NOVA_ELEVATION_LEVEL3,false},
  {0xc8ffffffu,0xa0b2bac4u,10,200,12,102,NOVA_ELEVATION_LEVEL4,false},
  {0xb8f2f4f7u,0x80b2bac4u,8,184,7,98,NOVA_ELEVATION_LEVEL5,false},
  {0xecffffffu,0xa0b2bac4u,10,236,10,100,NOVA_ELEVATION_LEVEL4,false}},
 [NOVA_THEME_HIGH_CONTRAST]={
  {0,0,0,0,0,100,NOVA_ELEVATION_NONE,true},
  {0xff000000u,0xffffffffu,0,255,0,100,NOVA_ELEVATION_LEVEL1,true},
  {0xff000000u,0xffffffffu,0,255,0,100,NOVA_ELEVATION_LEVEL2,true},
  {0xff000000u,0xffffffffu,0,255,0,100,NOVA_ELEVATION_LEVEL3,true},
  {0xff000000u,0xffffffffu,0,255,0,100,NOVA_ELEVATION_LEVEL4,true},
  {0xff000000u,0xffffffffu,0,255,0,100,NOVA_ELEVATION_LEVEL5,true},
  {0xff000000u,0xffffffffu,0,255,0,100,NOVA_ELEVATION_LEVEL5,true},
  {0xff000000u,0xffffffffu,0,255,0,100,NOVA_ELEVATION_LEVEL4,true}}};
static const uint16_t durations[NOVA_THEME_COUNT][NOVA_DURATION_COUNT]={
    {0,60,120,180,240,360},{0,60,120,180,240,360},{0,1,1,1,1,1}};
static const nova_motion_style_t motion_profiles[NOVA_THEME_COUNT][NOVA_TRANSITION_COUNT]={
 [NOVA_THEME_DARK]={{180,NOVA_DESIGN_EASE_OUT,2,2,false},{180,NOVA_DESIGN_EASE_OUT,3,3,false},
  {120,NOVA_DESIGN_EASE_OUT,2,2,false},{60,NOVA_DESIGN_EASE_OUT,1,2,false},
  {120,NOVA_DESIGN_EASE_IN_OUT,1,1,false},{120,NOVA_DESIGN_EASE_OUT,2,3,false},
  {120,NOVA_DESIGN_EASE_IN_OUT,2,2,false}},
 [NOVA_THEME_LIGHT]={{180,NOVA_DESIGN_EASE_OUT,2,2,false},{180,NOVA_DESIGN_EASE_OUT,3,3,false},
  {120,NOVA_DESIGN_EASE_OUT,2,2,false},{60,NOVA_DESIGN_EASE_OUT,1,2,false},
  {120,NOVA_DESIGN_EASE_IN_OUT,1,1,false},{120,NOVA_DESIGN_EASE_OUT,2,3,false},
  {120,NOVA_DESIGN_EASE_IN_OUT,2,2,false}},
 [NOVA_THEME_HIGH_CONTRAST]={{1,NOVA_DESIGN_EASE_LINEAR,0,2,true},
  {1,NOVA_DESIGN_EASE_LINEAR,0,3,true},{1,NOVA_DESIGN_EASE_LINEAR,0,2,true},
  {1,NOVA_DESIGN_EASE_LINEAR,0,2,true},{1,NOVA_DESIGN_EASE_LINEAR,0,1,true},
  {1,NOVA_DESIGN_EASE_LINEAR,0,3,true},{1,NOVA_DESIGN_EASE_LINEAR,0,2,true}}};
static nova_state_style_t state_styles[NOVA_INTERACTION_COUNT];
static const uint16_t state_transitions[NOVA_INTERACTION_COUNT]={
 [NOVA_INTERACTION_NORMAL]=(1u<<NOVA_INTERACTION_HOVER)|(1u<<NOVA_INTERACTION_FOCUSED)|
  (1u<<NOVA_INTERACTION_SELECTED)|(1u<<NOVA_INTERACTION_CHECKED)|
  (1u<<NOVA_INTERACTION_EXPANDED)|(1u<<NOVA_INTERACTION_COLLAPSED)|
  (1u<<NOVA_INTERACTION_DISABLED)|(1u<<NOVA_INTERACTION_READONLY)|
  (1u<<NOVA_INTERACTION_LOADING)|(1u<<NOVA_INTERACTION_ERROR),
 [NOVA_INTERACTION_HOVER]=(1u<<NOVA_INTERACTION_NORMAL)|(1u<<NOVA_INTERACTION_FOCUSED)|
  (1u<<NOVA_INTERACTION_PRESSED)|(1u<<NOVA_INTERACTION_DISABLED)|
  (1u<<NOVA_INTERACTION_ERROR),
 [NOVA_INTERACTION_FOCUSED]=(1u<<NOVA_INTERACTION_NORMAL)|(1u<<NOVA_INTERACTION_HOVER)|
  (1u<<NOVA_INTERACTION_PRESSED)|(1u<<NOVA_INTERACTION_SELECTED)|
  (1u<<NOVA_INTERACTION_CHECKED)|(1u<<NOVA_INTERACTION_EXPANDED)|
  (1u<<NOVA_INTERACTION_DISABLED)|(1u<<NOVA_INTERACTION_READONLY)|
  (1u<<NOVA_INTERACTION_LOADING)|(1u<<NOVA_INTERACTION_ERROR),
 [NOVA_INTERACTION_PRESSED]=(1u<<NOVA_INTERACTION_NORMAL)|(1u<<NOVA_INTERACTION_HOVER)|
  (1u<<NOVA_INTERACTION_FOCUSED)|(1u<<NOVA_INTERACTION_SELECTED)|
  (1u<<NOVA_INTERACTION_CHECKED)|(1u<<NOVA_INTERACTION_ERROR),
 [NOVA_INTERACTION_SELECTED]=(1u<<NOVA_INTERACTION_NORMAL)|(1u<<NOVA_INTERACTION_HOVER)|
  (1u<<NOVA_INTERACTION_FOCUSED)|(1u<<NOVA_INTERACTION_PRESSED)|
  (1u<<NOVA_INTERACTION_DISABLED)|(1u<<NOVA_INTERACTION_ERROR),
 [NOVA_INTERACTION_CHECKED]=(1u<<NOVA_INTERACTION_NORMAL)|(1u<<NOVA_INTERACTION_HOVER)|
  (1u<<NOVA_INTERACTION_FOCUSED)|(1u<<NOVA_INTERACTION_PRESSED)|
  (1u<<NOVA_INTERACTION_DISABLED)|(1u<<NOVA_INTERACTION_ERROR),
 [NOVA_INTERACTION_EXPANDED]=(1u<<NOVA_INTERACTION_COLLAPSED)|
  (1u<<NOVA_INTERACTION_FOCUSED)|(1u<<NOVA_INTERACTION_DISABLED),
 [NOVA_INTERACTION_COLLAPSED]=(1u<<NOVA_INTERACTION_EXPANDED)|
  (1u<<NOVA_INTERACTION_FOCUSED)|(1u<<NOVA_INTERACTION_DISABLED),
 [NOVA_INTERACTION_DISABLED]=(1u<<NOVA_INTERACTION_NORMAL),
 [NOVA_INTERACTION_READONLY]=(1u<<NOVA_INTERACTION_NORMAL)|
  (1u<<NOVA_INTERACTION_FOCUSED)|(1u<<NOVA_INTERACTION_ERROR),
 [NOVA_INTERACTION_LOADING]=(1u<<NOVA_INTERACTION_NORMAL)|(1u<<NOVA_INTERACTION_ERROR),
 [NOVA_INTERACTION_ERROR]=(1u<<NOVA_INTERACTION_NORMAL)|(1u<<NOVA_INTERACTION_FOCUSED)|
  (1u<<NOVA_INTERACTION_DISABLED)};
#define TYPO(font,size,min,line,max,weight,track,kind) \
    {font,size,min,line,max,weight,track,kind}
static const nova_typography_style_t typography_styles[NOVA_THEME_COUNT]
    [NOVA_TYPOGRAPHY_COUNT]={
 [NOVA_THEME_DARK]={
  TYPO(NOVA_SYSTEM_FONT_ID,32,36,40,48,600,20,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,26,30,34,42,600,15,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,22,26,30,38,600,10,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,18,22,26,34,400,10,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,14,18,20,28,400,10,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,12,16,18,24,400,10,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,11,14,16,22,400,40,NOVA_TRACKING_EXPANDED),
  TYPO(NOVA_SYSTEM_FONT_ID,14,18,20,28,600,10,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,14,18,20,28,400,10,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,12,16,18,24,400,30,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,14,18,20,28,400,0,NOVA_TRACKING_STANDARD)},
 [NOVA_THEME_LIGHT]={
  TYPO(NOVA_SYSTEM_FONT_ID,32,36,40,48,600,20,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,26,30,34,42,600,15,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,22,26,30,38,600,10,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,18,22,26,34,400,10,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,14,18,20,28,400,10,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,12,16,18,24,400,10,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,11,14,16,22,400,40,NOVA_TRACKING_EXPANDED),
  TYPO(NOVA_SYSTEM_FONT_ID,14,18,20,28,600,10,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,14,18,20,28,400,10,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,12,16,18,24,400,30,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,14,18,20,28,400,0,NOVA_TRACKING_STANDARD)},
 [NOVA_THEME_HIGH_CONTRAST]={
  TYPO(NOVA_SYSTEM_FONT_ID,36,40,46,54,600,25,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,30,34,40,48,600,20,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,24,28,34,42,600,15,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,20,24,30,38,600,15,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,16,22,26,34,400,15,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,14,20,24,32,400,15,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,13,18,22,30,400,45,NOVA_TRACKING_EXPANDED),
  TYPO(NOVA_SYSTEM_FONT_ID,16,22,26,34,600,15,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,16,22,26,34,600,15,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,14,20,24,32,600,35,NOVA_TRACKING_STANDARD),
  TYPO(NOVA_SYSTEM_FONT_ID,16,22,26,34,400,0,NOVA_TRACKING_STANDARD)}};
#undef TYPO

static nova_theme_id_t design_theme(void)
{nova_theme_id_t id=nova_theme_active();return id<NOVA_THEME_COUNT?id:NOVA_THEME_DARK;}

static void prepare_state_styles(void)
{
    const nova_theme_tokens_t *t=nova_theme_tokens();
    nova_state_style_t base={t->surface,t->text_primary,t->border,255,1,
        NOVA_ELEVATION_LEVEL1,NOVA_RADIUS_SMALL,NOVA_TRANSITION_SELECTION,false,false};
    for(uint8_t i=0;i<NOVA_INTERACTION_COUNT;++i)state_styles[i]=base;
    state_styles[NOVA_INTERACTION_NORMAL].motion=NOVA_TRANSITION_CARD;
    state_styles[NOVA_INTERACTION_HOVER].background=t->surface_secondary;
    state_styles[NOVA_INTERACTION_HOVER].elevation=NOVA_ELEVATION_LEVEL2;
    state_styles[NOVA_INTERACTION_FOCUSED].border=t->focus;
    state_styles[NOVA_INTERACTION_FOCUSED].motion=NOVA_TRANSITION_FOCUS;
    state_styles[NOVA_INTERACTION_PRESSED].background=t->selected;
    state_styles[NOVA_INTERACTION_PRESSED].opacity=220;
    state_styles[NOVA_INTERACTION_SELECTED].background=t->selected;
    state_styles[NOVA_INTERACTION_SELECTED].border=t->accent;
    state_styles[NOVA_INTERACTION_CHECKED]=state_styles[NOVA_INTERACTION_SELECTED];
    state_styles[NOVA_INTERACTION_EXPANDED].elevation=NOVA_ELEVATION_LEVEL3;
    state_styles[NOVA_INTERACTION_EXPANDED].motion=NOVA_TRANSITION_MENU;
    state_styles[NOVA_INTERACTION_COLLAPSED].motion=NOVA_TRANSITION_MENU;
    state_styles[NOVA_INTERACTION_DISABLED].foreground=t->disabled;
    state_styles[NOVA_INTERACTION_DISABLED].opacity=150;
    state_styles[NOVA_INTERACTION_DISABLED].cursor=4;
    state_styles[NOVA_INTERACTION_DISABLED].input_blocked=true;
    state_styles[NOVA_INTERACTION_READONLY].background=t->surface_secondary;
    state_styles[NOVA_INTERACTION_READONLY].cursor=2;
    state_styles[NOVA_INTERACTION_LOADING].cursor=3;
    state_styles[NOVA_INTERACTION_LOADING].input_blocked=true;
    state_styles[NOVA_INTERACTION_LOADING].motion=NOVA_TRANSITION_SELECTION;
    state_styles[NOVA_INTERACTION_ERROR].border=t->error;
    state_styles[NOVA_INTERACTION_ERROR].indicator_required=true;
}

static bool semantic_tokens_valid(void)
{
    for(uint8_t theme=0;theme<NOVA_THEME_COUNT;++theme){
        for(uint8_t i=1;i<NOVA_SPACING_COUNT;++i)if(spacing[theme][i]<spacing[theme][i-1])return false;
        for(uint8_t i=0;i<NOVA_TYPOGRAPHY_COUNT;++i){const nova_typography_style_t *s=&typography_styles[theme][i];
            if(!s->font_id||!s->size_dlu||s->line_min_dlu>s->line_dlu||
               s->line_dlu>s->line_max_dlu||s->tracking>=NOVA_TRACKING_COUNT)return false;}
        for(uint8_t i=0;i<NOVA_ELEVATION_COUNT;++i){const nova_shadow_style_t *s=&shadows[theme][i];
            if(s->layer_count>2)return false;
            for(uint8_t layer=0;layer<s->layer_count;++layer)
                if(s->layers[layer].opacity&&!s->layers[layer].blur_dlu)return false;}
        for(uint8_t i=0;i<NOVA_DESIGN_MATERIAL_COUNT;++i){const nova_material_style_t *m=&materials[theme][i];
            if(m->elevation>=NOVA_ELEVATION_COUNT||m->brightness>200)return false;}
        for(uint8_t i=0;i<NOVA_TRANSITION_COUNT;++i)
            if(!motion_profiles[theme][i].duration_ms||
               motion_profiles[theme][i].easing>=NOVA_DESIGN_EASE_COUNT)return false;
    }
    for(uint8_t i=0;i<NOVA_INTERACTION_COUNT;++i){
        const nova_state_style_t *s=&state_styles[i];
        if(s->elevation>=NOVA_ELEVATION_COUNT||s->radius>=NOVA_RADIUS_COUNT||
           s->motion>=NOVA_TRANSITION_COUNT||!s->opacity)return false;
    }
    return true;
}

static uint32_t manifest_checksum(const nova_design_manifest_t *value)
{
    const uint8_t *bytes=(const uint8_t *)value;uint32_t hash=2166136261u;
    for(uint32_t i=0;i<sizeof(*value)-sizeof(value->checksum);++i){hash^=bytes[i];hash*=16777619u;}
    return hash;
}
static uint32_t continuity_signature(void)
{
    uint32_t hash=2166136261u;
    const uint16_t counts[]={NOVA_TYPOGRAPHY_COUNT,NOVA_SPACING_COUNT,NOVA_RADIUS_COUNT,
        NOVA_ICON_COUNT,NOVA_DESIGN_MATERIAL_COUNT,NOVA_TRANSITION_COUNT,
        NOVA_INTERACTION_COUNT};
    for(uint8_t i=0;i<sizeof(counts)/sizeof(counts[0]);++i){
        hash^=counts[i]&255u;hash*=16777619u;hash^=counts[i]>>8;hash*=16777619u;}
    hash^=manifest.checksum;hash*=16777619u;return hash;
}
static void continuity_prepare(void)
{
    const nova_theme_resource_t *theme=nova_theme_current();
    const nova_theme_descriptor_t *fallback=nova_theme_default();
    continuity=(nova_visual_continuity_context_t){
        .design_language_version=NOVA_DESIGN_VERSION_MAJOR,
        .theme_id=theme?theme->theme_id:(fallback?fallback->theme_id:0),
        .registry_signature=continuity_signature(),
        .typography_count=NOVA_TYPOGRAPHY_COUNT,.spacing_count=NOVA_SPACING_COUNT,
        .radius_count=NOVA_RADIUS_COUNT,.icon_count=NOVA_ICON_COUNT,
        .material_count=NOVA_DESIGN_MATERIAL_COUNT,.motion_count=NOVA_TRANSITION_COUNT,
        .interaction_count=NOVA_INTERACTION_COUNT,.visual_continuity_enabled=true,
        .accessibility_semantics_preserved=true};
}
static bool compatible(nova_design_version_t version)
{return version.major==NOVA_DESIGN_VERSION_MAJOR&&version.minor<=NOVA_DESIGN_VERSION_MINOR;}
bool nova_design_validate_manifest(const nova_design_manifest_t *value)
{
    ++diagnostics.validations;
    if(!value||!compatible(value->design)||!compatible(value->tokens)||
       !compatible(value->typography)||!compatible(value->icons)||
       !compatible(value->fonts)||!compatible(value->motion)){
        ++diagnostics.version_errors;return false;}
    if(value->checksum!=manifest_checksum(value)){++diagnostics.checksum_errors;return false;}
    return true;
}
static bool resource_valid(uint64_t id,nova_resource_type_t type)
{
    const nova_resource_t *resource=nova_resource_find(id);
    return resource&&resource->type==type&&resource->version==NOVA_DESIGN_VERSION_MAJOR&&
           resource->data&&resource->size&&
           resource->checksum==nova_resource_checksum(resource->data,resource->size);
}
bool nova_design_validate_resources(void)
{
    bool valid=resource_valid(nova_resource_id("boot://themes/dark"),NOVA_RESOURCE_THEME)&&
        resource_valid(nova_resource_id("boot://themes/light"),NOVA_RESOURCE_THEME)&&
        resource_valid(nova_resource_id("boot://themes/high-contrast"),NOVA_RESOURCE_THEME)&&
        resource_valid(nova_resource_id("boot://fonts/segoe-ui/semibold/15"),NOVA_RESOURCE_FONT)&&
        resource_valid(nova_resource_id("boot://branding/novaos/logo"),NOVA_RESOURCE_IMAGE);
    for(uint8_t i=0;i<NOVA_ICON_COUNT&&valid;++i)valid=nova_icon_exists((nova_icon_token_t)i);
    if(!valid)++diagnostics.resource_errors;
    return valid;
}
bool nova_design_initialize(void)
{
    diagnostics=(nova_design_diagnostics_t){.initializations=1};oem=(nova_oem_design_t){0};
    continuity_diagnostics=(nova_visual_continuity_diagnostics_t){0};
    nova_design_version_t version={NOVA_DESIGN_VERSION_MAJOR,NOVA_DESIGN_VERSION_MINOR,
                                   NOVA_DESIGN_VERSION_PATCH};
    manifest=(nova_design_manifest_t){version,version,version,version,version,version,0};
    manifest.checksum=manifest_checksum(&manifest);
    prepare_state_styles();
    if(!nova_design_validate_manifest(&manifest)||!nova_design_validate_resources()||
       !semantic_tokens_valid()){++diagnostics.token_validation_errors;return false;}
    if(!nova_resource_register("boot://design/manifest",NOVA_RESOURCE_CONFIGURATION,1,
        &manifest,sizeof(manifest),0,0))return false;
    continuity_prepare();continuity_diagnostics.consistent=true;
    diagnostics.compatible=true;return true;
}
static bool optional_resource(uint64_t id,nova_resource_type_t expected)
{return !id||resource_valid(id,expected);}
bool nova_design_apply_oem(const nova_oem_design_t *value)
{
    if(!value||!optional_resource(value->background_resource,NOVA_RESOURCE_IMAGE)||
       !optional_resource(value->logo_resource,NOVA_RESOURCE_IMAGE)||
       !optional_resource(value->animation_resource,NOVA_RESOURCE_ANIMATION)||
       (value->accent&&((value->accent>>24)!=0xffu))){++diagnostics.oem_rejections;return false;}
    oem=*value;++diagnostics.oem_applies;return true;
}
const nova_visual_continuity_context_t *nova_visual_continuity_current(void)
{return continuity.visual_continuity_enabled?&continuity:0;}
bool nova_visual_continuity_validate(const nova_component_descriptor_t *component)
{
    ++continuity_diagnostics.validations;
    bool valid=component&&component->semantic_tokens_only&&
        component->design_language_version==continuity.design_language_version&&
        component->registry_signature==continuity.registry_signature&&
        component->typography_count==continuity.typography_count&&
        component->spacing_count==continuity.spacing_count&&
        component->radius_count==continuity.radius_count&&
        component->icon_count==continuity.icon_count&&
        component->material_count==continuity.material_count&&
        component->motion_count==continuity.motion_count&&
        component->interaction_count==continuity.interaction_count;
    if(!valid){++continuity_diagnostics.violations;continuity_diagnostics.consistent=false;}
    return valid;
}
bool nova_visual_continuity_reload(void)
{
    if(!nova_design_tokens_reload())return false;
    continuity_prepare();++continuity_diagnostics.reloads;
    continuity_diagnostics.consistent=true;return true;
}
const nova_visual_continuity_diagnostics_t *nova_visual_continuity_diagnostics(void)
{return &continuity_diagnostics;}
const nova_design_manifest_t *nova_design_manifest(void){return &manifest;}
const nova_typography_tokens_t *nova_design_typography(void){return &typography;}
const nova_typography_style_t *nova_typography_get(nova_typography_role_t role)
{++diagnostics.typography_lookups;return role<NOVA_TYPOGRAPHY_COUNT?
    &typography_styles[design_theme()][role]:0;}
bool nova_typography_exists(nova_typography_role_t role)
{return role<NOVA_TYPOGRAPHY_COUNT&&typography_styles[design_theme()][role].font_id!=0;}
uint16_t nova_spacing_get(nova_spacing_token_t token)
{++diagnostics.spacing_lookups;return token<NOVA_SPACING_COUNT?spacing[design_theme()][token]:0;}
bool nova_spacing_exists(nova_spacing_token_t token){return token<NOVA_SPACING_COUNT;}
uint16_t nova_radius_get(nova_radius_token_t token)
{++diagnostics.radius_lookups;return token<NOVA_RADIUS_COUNT?radii[design_theme()][token]:0;}
bool nova_radius_exists(nova_radius_token_t token){return token<NOVA_RADIUS_COUNT;}
bool nova_design_tokens_reload(void)
{if(!semantic_tokens_valid()){++diagnostics.token_validation_errors;return false;}
 prepare_state_styles();++diagnostics.token_reloads;return true;}
const nova_shadow_style_t *nova_shadow_get(nova_elevation_token_t token)
{++diagnostics.shadow_lookups;return token<NOVA_ELEVATION_COUNT?
    &shadows[design_theme()][token]:0;}
bool nova_shadow_exists(nova_elevation_token_t token){return token<NOVA_ELEVATION_COUNT;}
const nova_material_style_t *nova_material_get(nova_material_token_t token)
{++diagnostics.material_lookups;return token<NOVA_DESIGN_MATERIAL_COUNT?
    &materials[design_theme()][token]:0;}
bool nova_material_exists(nova_material_token_t token){return token<NOVA_DESIGN_MATERIAL_COUNT;}
uint16_t nova_duration_get(nova_duration_token_t token)
{return token<NOVA_DURATION_COUNT?(nova_theme_reduced_motion()?1:
    durations[design_theme()][token]):0;}
const nova_motion_style_t *nova_design_motion_get(nova_transition_token_t token)
{++diagnostics.motion_lookups;return token<NOVA_TRANSITION_COUNT?
    &motion_profiles[nova_theme_reduced_motion()?NOVA_THEME_HIGH_CONTRAST:design_theme()][token]:0;}
bool nova_design_motion_exists(nova_transition_token_t token){return token<NOVA_TRANSITION_COUNT;}
const nova_state_style_t *nova_state_get(nova_interaction_state_t state)
{++diagnostics.state_lookups;return state<NOVA_INTERACTION_COUNT?&state_styles[state]:0;}
bool nova_state_exists(nova_interaction_state_t state){return state<NOVA_INTERACTION_COUNT;}
bool nova_state_transition_allowed(nova_interaction_state_t current,
                                   nova_interaction_state_t next)
{
    if(current>=NOVA_INTERACTION_COUNT||next>=NOVA_INTERACTION_COUNT||current==next){
        ++diagnostics.state_rejections;return false;}
    bool allowed=(state_transitions[current]&(1u<<next))!=0;
    if(allowed)++diagnostics.state_transitions;else ++diagnostics.state_rejections;
    return allowed;
}
const nova_component_tokens_t *nova_design_components(void){return &components;}
const nova_design_motion_tokens_t *nova_design_motion(void){return &motion_tokens;}
const nova_effect_tokens_t *nova_design_effects(void){return &effects;}
const nova_oem_design_t *nova_design_oem(void){return &oem;}
uint32_t nova_design_accent(void){return oem.accent?oem.accent:nova_theme_tokens()->accent;}
const nova_theme_tokens_t *nova_design_tokens(void){return nova_theme_tokens();}
const nova_design_diagnostics_t *nova_design_diagnostics(void){return &diagnostics;}
