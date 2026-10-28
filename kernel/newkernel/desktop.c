/* NovaOS C-Kernel - Nova Desktop Shell (siehe desktop.h fuer Scope). */
#include "desktop.h"
#include "display.h"
#include "compositor.h"

#define DESKTOP_BACKGROUND_RGB 0x1E2430u /* dunkles Blaugrau */
#define DESKTOP_CURSOR_RGB     0xFFFFFFu

int desktop_show(void) {
    uint32_t width = display_width();
    uint32_t height = display_height();
    if (width == 0 || height == 0) {
        return -1;
    }

    /* Nur noch Hintergrund+Cursor fuer den allerersten sichtbaren Frame
       (siehe desktop.h) - die frueher hier gezeichnete Taskbar/Beispiel-
       fenster-Platzhalter (alte compositor_window_create-Rechteck-API)
       sind entfallen, seit desktop_shell.c (siehe dessen Scope-Hinweis)
       eine echte, auf window.c/surface.c aufsetzende Taskbar liefert -
       beide ueberlagerten sich sonst sichtbar am unteren Bildschirmrand. */
    compositor_initialize();
    compositor_set_background(DESKTOP_BACKGROUND_RGB);
    compositor_set_cursor(width / 2u, height / 2u, DESKTOP_CURSOR_RGB);

    compositor_composite();

    display_set_running();

    return 0;
}
