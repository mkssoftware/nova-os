/**
 * Nova OS Bootloader - Grafikmodul (UEFI GOP)
 *
 * Aufgabe:
 * - Aktiviert den Grafikmodus über UEFI GOP
 * - Zugriff auf Framebuffer
 * - Pixel-Rendering
 * - einfache UI Primitive (Rechtecke)
 *
 * WICHTIG:
 * Kein GPU-Treiber! Nur direkter Framebuffer Zugriff.
 */

#include <efi.h>
#include <efilib.h>

// GOP (Graphics Output Protocol)
static EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;

// Framebuffer Informationen
static UINT32 *framebuffer = NULL;
static UINTN width = 0;
static UINTN height = 0;
static UINTN pixels_per_scanline = 0;

/**
 * Grafiksystem initialisieren
 */
EFI_STATUS grafik_init(EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS status;

    Print(L"[NOVA] Initialisiere Grafikmodus...\n");

    // GOP laden
    status = SystemTable->BootServices->LocateProtocol(
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        (VOID**)&gop
    );

    if (EFI_ERROR(status))
    {
        Print(L"[FEHLER] GOP nicht verfügbar!\n");
        return status;
    }

    // Bildschirminformationen holen
    width = gop->Mode->Info->HorizontalResolution;
    height = gop->Mode->Info->VerticalResolution;
    pixels_per_scanline = gop->Mode->Info->PixelsPerScanLine;

    framebuffer = (UINT32*)gop->Mode->FrameBufferBase;

    Print(L"[NOVA] Grafikmodus aktiviert\n");
    Print(L"[NOVA] Aufloesung geladen\n");

    return EFI_SUCCESS;
}

/**
 * Einzelnen Pixel zeichnen
 */
void pixel_set(UINTN x, UINTN y, UINT32 color)
{
    if (x >= width || y >= height)
        return;

    framebuffer[y * pixels_per_scanline + x] = color;
}

/**
 * Rechteck zeichnen (UI Basis)
 */
void rect_draw(UINTN x, UINTN y, UINTN w, UINTN h, UINT32 color)
{
    for (UINTN iy = 0; iy < h; iy++)
    {
        for (UINTN ix = 0; ix < w; ix++)
        {
            pixel_set(x + ix, y + iy, color);
        }
    }
}

/**
 * Hintergrund löschen (dunkler Glass-Effekt Basis)
 */
void screen_clear(UINT32 color)
{
    for (UINTN y = 0; y < height; y++)
    {
        for (UINTN x = 0; x < width; x++)
        {
            pixel_set(x, y, color);
        }
    }
}