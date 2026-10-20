/* NovaOS C-Kernel - minimaler Desktop-Platzhalter ("Desktop First"). */
#include "desktop.h"
#include "framebuffer.h"

#define DESKTOP_BACKGROUND_RGB 0x1E2430u /* dunkles Blaugrau */
#define DESKTOP_TASKBAR_RGB    0x10141Cu
#define DESKTOP_TASKBAR_HEIGHT 40u

int desktop_show(void) {
    if (!framebuffer_available()) {
        return -1;
    }

    framebuffer_clear(DESKTOP_BACKGROUND_RGB);

    uint32_t height = framebuffer_height();
    uint32_t width = framebuffer_width();
    if (height > DESKTOP_TASKBAR_HEIGHT) {
        framebuffer_fill_rect(0, height - DESKTOP_TASKBAR_HEIGHT, width,
                               DESKTOP_TASKBAR_HEIGHT, DESKTOP_TASKBAR_RGB);
    }

    return 0;
}
