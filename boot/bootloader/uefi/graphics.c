#include "uefi_min.h"
#include "../bootmenu/graphics.h"
#include "../bootmenu/framebuffer_backend.h"
#include "../bootmenu/gop_backend.h"
#include <stdbool.h>

static EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

static EFI_GUID gop_guid = {
    0x9042a9de, 0x23dc, 0x4a38,
    {0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a}
};

#ifndef NOVA_GOP_PREFERRED_WIDTH
#define NOVA_GOP_PREFERRED_WIDTH 0
#endif
#ifndef NOVA_GOP_PREFERRED_HEIGHT
#define NOVA_GOP_PREFERRED_HEIGHT 0
#endif

EFI_STATUS grafik_init(EFI_SYSTEM_TABLE *system_table)
{
    nova_gop_reset();
    EFI_STATUS status = system_table->BootServices->LocateProtocol(
        &gop_guid, 0, (VOID **)&gop);
    if (EFI_ERROR(status) || !gop || !gop->Mode || !gop->Mode->Info)
        return status;
    static nova_gop_mode_candidate_t candidates[NOVA_GOP_MODE_CAPACITY];
    uint32_t candidate_count=0,limit=gop->Mode->MaxMode;
    if(limit>NOVA_GOP_MODE_CAPACITY)limit=NOVA_GOP_MODE_CAPACITY;
    if(gop->QueryMode)for(uint32_t mode=0;mode<limit;++mode){
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *query=0;UINTN query_size=0;
        status=gop->QueryMode(gop,mode,&query_size,&query);
        if(EFI_ERROR(status)||!query)continue;
        candidates[candidate_count++]=(nova_gop_mode_candidate_t){mode,
            query->HorizontalResolution,query->VerticalResolution,query->PixelsPerScanLine,
            (nova_gop_pixel_format_t)query->PixelFormat,query->PixelInformation[0],
            query->PixelInformation[1],query->PixelInformation[2],query->PixelInformation[3]};
        system_table->BootServices->FreePool(query);
    }
    if(!candidate_count){EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *current=gop->Mode->Info;
        candidates[candidate_count++]=(nova_gop_mode_candidate_t){gop->Mode->Mode,
            current->HorizontalResolution,current->VerticalResolution,current->PixelsPerScanLine,
            (nova_gop_pixel_format_t)current->PixelFormat,current->PixelInformation[0],
            current->PixelInformation[1],current->PixelInformation[2],current->PixelInformation[3]};}
    uint32_t selected=0;
    if(nova_gop_select_mode(candidates,candidate_count,gop->Mode->Mode,
            NOVA_GOP_PREFERRED_WIDTH,NOVA_GOP_PREFERRED_HEIGHT,&selected)!=NOVA_GOP_OK){
        if(NOVA_GOP_PREFERRED_WIDTH||NOVA_GOP_PREFERRED_HEIGHT)
            nova_debug_string("UEFI:GOP-PREFERRED-UNAVAILABLE\n");
        return 1;
    }
    if(selected!=gop->Mode->Mode){
        if(!gop->SetMode||EFI_ERROR(gop->SetMode(gop,selected)))return 1;
        nova_gop_record_mode_change();
    }
    if(NOVA_GOP_PREFERRED_WIDTH&&NOVA_GOP_PREFERRED_HEIGHT)
        nova_debug_string("UEFI:GOP-PREFERRED-READY\n");
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info=gop->Mode->Info;
    nova_gop_mode_candidate_t active={gop->Mode->Mode,info->HorizontalResolution,
        info->VerticalResolution,info->PixelsPerScanLine,(nova_gop_pixel_format_t)info->PixelFormat,
        info->PixelInformation[0],info->PixelInformation[1],info->PixelInformation[2],
        info->PixelInformation[3]};
    if(nova_gop_initialize(&active,gop->Mode->MaxMode,gop->Mode->FrameBufferBase,
                           gop->Mode->FrameBufferSize)!=NOVA_GOP_OK){
        if(info->PixelFormat==3)nova_debug_string("UEFI:GOP-BLT-ONLY-UNSUPPORTED\n");
        return 1;
    }
    nova_gop_descriptor_t descriptor;
    if(nova_gop_get_descriptor(&descriptor)!=NOVA_GOP_OK)return 1;
    nova_graphics_context_t context={
        .width=descriptor.width,.height=descriptor.height,
        .pitch=descriptor.pitch,.bits_per_pixel=descriptor.bytes_per_pixel*8u,.dpi=96,
        .pixel_format=descriptor.pixel_format,.firmware=NOVA_GRAPHICS_FIRMWARE_UEFI_GOP,
        .framebuffer=(void *)(UINTN)descriptor.framebuffer_address,
        .framebuffer_size=descriptor.framebuffer_size,
        .framebuffer_address=descriptor.framebuffer_address,
        .capabilities=NOVA_GRAPHICS_ALPHA|NOVA_GRAPHICS_BLEND|
            NOVA_GRAPHICS_DOUBLE_BUFFER|NOVA_GRAPHICS_LINEAR_FRAMEBUFFER|
            NOVA_GRAPHICS_POINTER|NOVA_GRAPHICS_NATIVE_RESOLUTION,
        .red_mask=descriptor.red_mask,.green_mask=descriptor.green_mask,
        .blue_mask=descriptor.blue_mask,.alpha_mask=descriptor.alpha_mask,
        .vendor="UEFI",.device="Graphics Output Protocol"};
    if(!nova_graphics_initialize(&context))return 1;
    if(nova_framebuffer_initialize()!=NOVA_FB_OK)return 1;
    nova_debug_string("UEFI:GOP-MODES-VALIDATED\n");
    nova_debug_string("UEFI:GOP-DESCRIPTOR-READY\n");
    nova_debug_string("UEFI:GAL-READY\n");
    nova_debug_string("UEFI:FRAMEBUFFER-BACKEND-READY\n");
    nova_debug_string("UEFI:GOP-READY\n");
    return EFI_SUCCESS;
}
