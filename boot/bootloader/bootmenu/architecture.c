#include "architecture.h"

#define BIT(value) (1u<<(value))

static const nova_architecture_descriptor_t descriptors[NOVA_ARCH_SUBSYSTEM_COUNT]={
    {NOVA_ARCH_PLATFORM,"platform",1,0,0,false,true},
    {NOVA_ARCH_RESOURCE,"resource",1,0,BIT(NOVA_ARCH_PLATFORM),true,true},
    {NOVA_ARCH_GRAPHICS,"graphics",1,0,BIT(NOVA_ARCH_PLATFORM),true,true},
    {NOVA_ARCH_RENDERER,"renderer",1,0,BIT(NOVA_ARCH_RESOURCE)|BIT(NOVA_ARCH_GRAPHICS),true,true},
    {NOVA_ARCH_SCENE,"scene",1,0,BIT(NOVA_ARCH_RESOURCE),true,true},
    {NOVA_ARCH_LAYOUT,"layout",1,0,BIT(NOVA_ARCH_SCENE),true,true},
    {NOVA_ARCH_MOTION,"motion",1,0,BIT(NOVA_ARCH_LAYOUT),true,true},
    {NOVA_ARCH_CONTROL,"control",1,0,BIT(NOVA_ARCH_SCENE)|BIT(NOVA_ARCH_LAYOUT)|BIT(NOVA_ARCH_MOTION),true,true},
    {NOVA_ARCH_NAVIGATION,"navigation",1,0,BIT(NOVA_ARCH_CONTROL),true,true},
    {NOVA_ARCH_DIALOG,"dialog",1,0,BIT(NOVA_ARCH_CONTROL)|BIT(NOVA_ARCH_NAVIGATION),true,true},
    {NOVA_ARCH_APPLICATION,"application",1,0,BIT(NOVA_ARCH_NAVIGATION)|BIT(NOVA_ARCH_DIALOG),true,true},
    {NOVA_ARCH_DIAGNOSTICS,"diagnostics",1,0,BIT(NOVA_ARCH_PLATFORM),true,true}
};
static nova_architecture_diagnostics_t diagnostics;

void nova_architecture_initialize(void)
{diagnostics=(nova_architecture_diagnostics_t){0};}

const nova_architecture_descriptor_t *nova_architecture_descriptor(
    nova_architecture_subsystem_t subsystem)
{return subsystem<NOVA_ARCH_SUBSYSTEM_COUNT?&descriptors[subsystem]:0;}

bool nova_architecture_register(nova_architecture_subsystem_t subsystem)
{
    const nova_architecture_descriptor_t *item=nova_architecture_descriptor(subsystem);
    if(!item||item->id!=subsystem||item->abi_major!=NOVA_ARCHITECTURE_ABI_MAJOR||
       item->abi_minor>NOVA_ARCHITECTURE_ABI_MINOR||
       (diagnostics.registered_mask&BIT(subsystem))){++diagnostics.invalid_descriptors;return false;}
    diagnostics.registered_mask|=BIT(subsystem);return true;
}

bool nova_architecture_ready(nova_architecture_subsystem_t subsystem)
{
    const nova_architecture_descriptor_t *item=nova_architecture_descriptor(subsystem);
    if(!item||!(diagnostics.registered_mask&BIT(subsystem))||
       (diagnostics.ready_mask&BIT(subsystem))||
       (diagnostics.ready_mask&item->dependencies)!=item->dependencies){
        ++diagnostics.missing_dependencies;return false;}
    diagnostics.ready_mask|=BIT(subsystem);return true;
}

bool nova_architecture_validate(void)
{
    ++diagnostics.validations;uint32_t all=(1u<<NOVA_ARCH_SUBSYSTEM_COUNT)-1u;
    diagnostics.valid=diagnostics.registered_mask==all;
    diagnostics.complete=diagnostics.valid&&diagnostics.ready_mask==all;
    return diagnostics.complete;
}

const nova_architecture_diagnostics_t *nova_architecture_diagnostics(void)
{return &diagnostics;}
