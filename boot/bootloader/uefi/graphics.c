#include "uefi_min.h"
#include "../bootmenu/graphics.h"
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
    EFI_STATUS status = system_table->BootServices->LocateProtocol(
        &gop_guid, 0, (VOID **)&gop);
    if (EFI_ERROR(status) || !gop || !gop->Mode || !gop->Mode->Info)
        return status;

    if (NOVA_GOP_PREFERRED_WIDTH && NOVA_GOP_PREFERRED_HEIGHT &&
        gop->QueryMode && gop->SetMode) {
        bool selected = false;
        for (uint32_t mode = 0; mode < gop->Mode->MaxMode; ++mode) {
            EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info = 0;
            UINTN info_size = 0;
            status = gop->QueryMode(gop, mode, &info_size, &info);
            if (!EFI_ERROR(status) && info) {
                bool match = info->HorizontalResolution == NOVA_GOP_PREFERRED_WIDTH &&
                             info->VerticalResolution == NOVA_GOP_PREFERRED_HEIGHT;
                system_table->BootServices->FreePool(info);
                if (match && !EFI_ERROR(gop->SetMode(gop, mode))) {
                    selected = true;
                    break;
                }
            }
        }
        if (!selected) {
            nova_debug_string("UEFI:GOP-PREFERRED-UNAVAILABLE\n");
            return 1;
        }
        nova_debug_string("UEFI:GOP-PREFERRED-READY\n");
    }

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info=gop->Mode->Info;
    nova_pixel_format_t format;
    if(info->PixelFormat==0)format=NOVA_PIXEL_RGBA8888;
    else if(info->PixelFormat==1)format=NOVA_PIXEL_BGRA8888;
    else if(info->PixelFormat==2)format=NOVA_PIXEL_BIT_MASK;
    else {nova_debug_string("UEFI:GOP-BLT-ONLY-UNSUPPORTED\n");return 1;}
    nova_graphics_context_t context={
        .width=info->HorizontalResolution,.height=info->VerticalResolution,
        .pitch=info->PixelsPerScanLine*4u,.bits_per_pixel=32,.dpi=96,
        .pixel_format=format,.firmware=NOVA_GRAPHICS_FIRMWARE_UEFI_GOP,
        .framebuffer=(void *)(UINTN)gop->Mode->FrameBufferBase,
        .framebuffer_size=gop->Mode->FrameBufferSize,
        .framebuffer_address=gop->Mode->FrameBufferBase,
        .capabilities=NOVA_GRAPHICS_ALPHA|NOVA_GRAPHICS_BLEND|
            NOVA_GRAPHICS_DOUBLE_BUFFER|NOVA_GRAPHICS_LINEAR_FRAMEBUFFER|
            NOVA_GRAPHICS_POINTER|NOVA_GRAPHICS_NATIVE_RESOLUTION,
        .red_mask=info->PixelInformation[0],.green_mask=info->PixelInformation[1],
        .blue_mask=info->PixelInformation[2],.alpha_mask=info->PixelInformation[3],
        .vendor="UEFI",.device="Graphics Output Protocol"};
    if(!nova_graphics_initialize(&context))return 1;
    nova_debug_string("UEFI:GAL-READY\n");
    nova_debug_string("UEFI:GOP-READY\n");
    return EFI_SUCCESS;
}
