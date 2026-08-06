#include "../uefi/uefi_min.h"
#include "compositor.h"

UINTN grafik_width(void);
UINTN grafik_height(void);

static nova_surface_t *base_surface;
static nova_surface_t *interaction_surface;
static bool initialized;

static void rounded_panel(nova_surface_t *surface, int32_t x, int32_t y,
                          int32_t width, int32_t height, uint32_t color)
{
    if (width < 12 || height < 12) return;
    nova_surface_rect(surface, (nova_rect_t){x + 6, y, width - 12, height}, color);
    nova_surface_rect(surface, (nova_rect_t){x, y + 6, width, height - 12}, color);
}

bool bootmenu_initialize(void)
{
    if (!nova_compositor_initialize((uint32_t)grafik_width(),
                                    (uint32_t)grafik_height())) return false;
    base_surface = nova_surface_acquire();
    interaction_surface = nova_surface_acquire();
    initialized = base_surface && interaction_surface;
    if (initialized) nova_debug_string("UEFI:COMPOSITOR-READY\n");
    return initialized;
}

void bootmenu_draw(UINTN selection, uint8_t opacity)
{
    if (!initialized) return;
    int32_t width = (int32_t)grafik_width();
    int32_t panel_x = width > 800 ? (width - 800) / 2 + 184 : 184;

    nova_surface_clear(base_surface, 0xff101113u);
    rounded_panel(base_surface, panel_x, 55, 567, 473, 0xff191b1eu);
    rounded_panel(base_surface, (width - 628) / 2, 0, 628, 13, 0xff267cc1u);

    nova_surface_clear(interaction_surface, 0x00000000u);
    rounded_panel(interaction_surface, panel_x + 20,
                  77 + (int32_t)selection * 68, 523, 62, 0xd022262au);
    rounded_panel(interaction_surface, panel_x + 20,
                  91 + (int32_t)selection * 68, 12, 34, 0xff4cc2ffu);

    nova_layer_t base = {
        1, base_surface, {0, 0, width, (int32_t)grafik_height()},
        {0, 0, width, (int32_t)grafik_height()}, 0, opacity,
        NOVA_MATERIAL_NONE, true, false, false, true
    };
    nova_layer_t interaction = {
        2, interaction_surface, {0, 0, width, (int32_t)grafik_height()},
        {0, 0, width, (int32_t)grafik_height()}, 10, opacity,
        NOVA_MATERIAL_GLASS, true, true, false, true
    };
    nova_compositor_begin_frame();
    nova_compositor_submit_layer(&base);
    nova_compositor_submit_layer(&interaction);
    nova_compositor_compose();
    nova_debug_string("UEFI:MENU-DRAWN\n");
}
