#include "runtime_bridge.h"

static nova_uefi_runtime_bridge_context_t context __attribute__((aligned(16)));
static EFI_GUID boot_control_guid={0x4e4f5641u,0x4243u,0x4f4eu,
    {0x54u,0x52u,0x4fu,0x4cu,0x56u,0x31u,0x00u,0x01u}};
static CHAR16 health_name[]={'N','o','v','a','B','o','o','t','H','e','a','l','t','h',0};

static void zero_bytes(void *target,UINTN length)
{
    uint8_t *bytes=(uint8_t *)target;while(length--)*bytes++=0;
}

static bool low_address(const void *pointer)
{
    return pointer&&(uint64_t)(UINTN)pointer<=UINT32_MAX;
}

bool uefi_runtime_bridge_prepare(EFI_SYSTEM_TABLE *system_table)
{
    zero_bytes(&context,sizeof(context));
    if(!system_table||!system_table->RuntimeServices||
       !system_table->RuntimeServices->SetVariable||
       !low_address(&context)||!low_address(system_table->RuntimeServices->SetVariable)||
       !low_address(health_name)||!low_address(&boot_control_guid)||
       !low_address(uefi_runtime_bridge_entry32)||
       !low_address(uefi_runtime_bridge_entry64)||
       !low_address(uefi_runtime_bridge_return32))return false;
    uint64_t cr3=0;__asm__ volatile("mov %%cr3,%0":"=r"(cr3));
    if(cr3>UINT32_MAX)return false;
    context.size=sizeof(context);context.version=NOVA_UEFI_RUNTIME_BRIDGE_VERSION;
    context.capabilities=NOVA_FIRMWARE_RUNTIME_PERSIST_BOOT_HEALTH;
    context.firmware_cr3=cr3;
    context.set_variable=(uint64_t)(UINTN)system_table->RuntimeServices->SetVariable;
    context.variable_name=(uint64_t)(UINTN)health_name;
    context.vendor_guid=(uint64_t)(UINTN)&boot_control_guid;
    context.gdt[0]=0;
    context.gdt[1]=UINT64_C(0x00af9a000000ffff);
    context.gdt[2]=UINT64_C(0x00cf92000000ffff);
    context.gdt[3]=UINT64_C(0x00cf9a000000ffff);
    context.gdtr.limit=(uint16_t)(sizeof(context.gdt)-1u);
    context.gdtr.base=(uint32_t)(UINTN)&context.gdt[0];
    context.enter64.offset=(uint32_t)(UINTN)uefi_runtime_bridge_entry64;
    context.enter64.selector=0x08;
    context.return32.offset=(uint32_t)(UINTN)uefi_runtime_bridge_return32;
    context.return32.selector=0x18;
    return true;
}

bool uefi_runtime_bridge_descriptor(nova_bib_firmware_runtime_t *descriptor)
{
    if(!descriptor||context.size!=sizeof(context)||
       !(context.capabilities&NOVA_FIRMWARE_RUNTIME_PERSIST_BOOT_HEALTH))return false;
    descriptor->provider=NOVA_FIRMWARE_RUNTIME_PROVIDER_UEFI_X64;
    descriptor->capabilities=context.capabilities;
    descriptor->context_address=(uint64_t)(UINTN)&context;
    descriptor->persist_boot_health_entry=(uint64_t)(UINTN)uefi_runtime_bridge_entry32;
    descriptor->maximum_payload_size=sizeof(context.evidence);
    descriptor->flags=0;
    return true;
}
