/**
 * Nova OS Bootloader - UI Grundsystem
 *
 * Aufgabe:
 * - baut erste "Glass UI" Struktur
 * - nutzt Grafikprimitive
 * - vorbereitet für Animationen
 */

#include <efi.h>

// externe Grafikfunktionen
void screen_clear(UINT32 color);
void rect_draw(UINTN x, UINTN y, UINTN w, UINTN h, UINT32 color);

/**
 * Boot-Menü zeichnen
 */
void bootmenu_draw()
{
    // Dunkler Hintergrund (Fluent Dark Basis)
    screen_clear(0x0A0A0A);

    // halbtransparente Panels (simuliert)
    rect_draw(100, 80, 500, 400, 0x1A1A1A);
    rect_draw(120, 120, 460, 60, 0x2A2A2A);

    rect_draw(120, 200, 460, 50, 0x2A2A2A);
    rect_draw(120, 260, 460, 50, 0x2A2A2A);
    rect_draw(120, 320, 460, 50, 0x2A2A2A);

    Print(L"[NOVA] Boot-Menue UI gerendert\n");
}