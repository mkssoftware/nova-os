/* NovaOS C-Kernel - Nova Desktop Shell (siehe desktop.h fuer Scope). */
#include "desktop.h"
#include "display.h"
#include "compositor.h"

#define DESKTOP_BACKGROUND_RGB 0x1E2430u /* dunkles Blaugrau */
#define DESKTOP_TASKBAR_RGB    0x10141Cu
#define DESKTOP_TASKBAR_HEIGHT 40u
#define DESKTOP_WINDOW_RGB     0x2D3542u
#define DESKTOP_WINDOW_ALPHA   235u
#define DESKTOP_CURSOR_RGB     0xFFFFFFu

int desktop_show(void) {
    uint32_t width = display_width();
    uint32_t height = display_height();
    if (width == 0 || height == 0) {
        return -1;
    }

    compositor_initialize();
    compositor_set_background(DESKTOP_BACKGROUND_RGB);

    uint32_t taskbar_id;
    if (height > DESKTOP_TASKBAR_HEIGHT) {
        compositor_window_create(0, height - DESKTOP_TASKBAR_HEIGHT, width,
                                  DESKTOP_TASKBAR_HEIGHT, DESKTOP_TASKBAR_RGB,
                                  255u, &taskbar_id);
    }

    uint32_t sample_window_w = width / 3u;
    uint32_t sample_window_h = height / 3u;
    if (sample_window_w > 0 && sample_window_h > 0) {
        uint32_t sample_window_id;
        if (compositor_window_create(width / 8u, height / 8u, sample_window_w,
                                      sample_window_h, DESKTOP_WINDOW_RGB,
                                      DESKTOP_WINDOW_ALPHA, &sample_window_id) == 0) {
            compositor_window_set_shadow(sample_window_id, 1);
        }
    }

    compositor_set_cursor(width / 2u, height / 2u, DESKTOP_CURSOR_RGB);

    compositor_composite();

    display_set_running();

    return 0;
}
