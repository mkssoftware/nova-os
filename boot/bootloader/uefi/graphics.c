#include "uefi_min.h"
#include <stdbool.h>

static EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
static uint32_t *framebuffer;
static UINTN width;
static UINTN height;
static UINTN pixels_per_scanline;
static UINTN framebuffer_pixels;

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

    width = gop->Mode->Info->HorizontalResolution;
    height = gop->Mode->Info->VerticalResolution;
    pixels_per_scanline = gop->Mode->Info->PixelsPerScanLine;
    framebuffer = (uint32_t *)(UINTN)gop->Mode->FrameBufferBase;
    framebuffer_pixels = gop->Mode->FrameBufferSize / sizeof(uint32_t);
    if (!framebuffer || !width || !height || pixels_per_scanline < width ||
        framebuffer_pixels < pixels_per_scanline * height)
        return 1;
    nova_debug_string("UEFI:GOP-READY\n");
    return EFI_SUCCESS;
}

UINTN grafik_width(void) { return width; }
UINTN grafik_height(void) { return height; }

void pixel_set(UINTN x, UINTN y, uint32_t color)
{
    if (!framebuffer || x >= width || y >= height) return;
    framebuffer[y * pixels_per_scanline + x] = color;
}

void rect_draw(UINTN x, UINTN y, UINTN w, UINTN h, uint32_t color)
{
    if (x >= width || y >= height) return;
    if (w > width - x) w = width - x;
    if (h > height - y) h = height - y;
    for (UINTN iy = 0; iy < h; ++iy) {
        uint32_t *row = framebuffer + (y + iy) * pixels_per_scanline + x;
        for (UINTN ix = 0; ix < w; ++ix)
            row[ix] = color;
    }
}

void screen_clear(uint32_t color)
{
    rect_draw(0, 0, width, height, color);
}
