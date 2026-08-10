#include "design.h"
#include "resources.h"
#include "icons.h"
#include "motion.h"

static nova_design_manifest_t manifest;
static nova_oem_design_t oem;
static nova_design_diagnostics_t diagnostics;
static const nova_typography_tokens_t typography={32,14,12,40,20,16,20,10,40,400,600};
static const nova_component_tokens_t components={48,12,16,16,48,8,2,24};
static const nova_design_motion_tokens_t motion_tokens={180,180,240,120,120,950,
    NOVA_EASE_OUT_CUBIC,NOVA_EASE_OUT_CUBIC};
static const nova_effect_tokens_t effects={12,1,18,0,6,210,12,96};

static uint32_t manifest_checksum(const nova_design_manifest_t *value)
{
    const uint8_t *bytes=(const uint8_t *)value;uint32_t hash=2166136261u;
    for(uint32_t i=0;i<sizeof(*value)-sizeof(value->checksum);++i){hash^=bytes[i];hash*=16777619u;}
    return hash;
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
    nova_design_version_t version={NOVA_DESIGN_VERSION_MAJOR,NOVA_DESIGN_VERSION_MINOR,
                                   NOVA_DESIGN_VERSION_PATCH};
    manifest=(nova_design_manifest_t){version,version,version,version,version,version,0};
    manifest.checksum=manifest_checksum(&manifest);
    if(!nova_design_validate_manifest(&manifest)||!nova_design_validate_resources())return false;
    if(!nova_resource_register("boot://design/manifest",NOVA_RESOURCE_CONFIGURATION,1,
        &manifest,sizeof(manifest),0,0))return false;
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
const nova_design_manifest_t *nova_design_manifest(void){return &manifest;}
const nova_typography_tokens_t *nova_design_typography(void){return &typography;}
const nova_component_tokens_t *nova_design_components(void){return &components;}
const nova_design_motion_tokens_t *nova_design_motion(void){return &motion_tokens;}
const nova_effect_tokens_t *nova_design_effects(void){return &effects;}
const nova_oem_design_t *nova_design_oem(void){return &oem;}
uint32_t nova_design_accent(void){return oem.accent?oem.accent:nova_theme_tokens()->accent;}
const nova_theme_tokens_t *nova_design_tokens(void){return nova_theme_tokens();}
const nova_design_diagnostics_t *nova_design_diagnostics(void){return &diagnostics;}
