#include "../uefi/uefi_min.h"

void screen_clear(uint32_t color);
void rect_draw(UINTN x, UINTN y, UINTN w, UINTN h, uint32_t color);
UINTN grafik_width(void);
UINTN grafik_height(void);

static void rounded_panel(UINTN x, UINTN y, UINTN w, UINTN h, uint32_t color)
{
    rect_draw(x + 6, y, w - 12, h, color);
    rect_draw(x, y + 6, w, h - 12, color);
}

void bootmenu_draw(UINTN selection)
{
    UINTN width = grafik_width();
    UINTN panel_x = width > 800 ? (width - 800) / 2 + 184 : 184;
    screen_clear(0x00101113);
    rounded_panel(panel_x, 55, 567, 473, 0x00191b1e);
    rounded_panel(panel_x + 20, 77 + selection * 68, 523, 62, 0x0022262a);
    rounded_panel(panel_x + 20, 91 + selection * 68, 12, 34, 0x004cc2ff);
    rounded_panel((width - 628) / 2, 0, 628, 13, 0x00267cc1);
    nova_debug_string("UEFI:MENU-DRAWN\n");
}
