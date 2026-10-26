/* NovaOS C-Kernel - Nova Display Manager (siehe display.h fuer Scope). */
#include "display.h"
#include "framebuffer.h"

static uint32_t display_current_state = DISPLAY_STATE_UNINITIALIZED;
static uint32_t display_width_px = 0;
static uint32_t display_height_px = 0;

int display_initialize(uint32_t address, uint32_t pitch, uint32_t width,
                        uint32_t height, uint32_t bits_per_pixel) {
    display_current_state = DISPLAY_STATE_INITIALIZING;

    if (framebuffer_initialize(address, pitch, width, height, bits_per_pixel) != 0) {
        display_current_state = DISPLAY_STATE_FAILED;
        return -1;
    }

    display_width_px = width;
    display_height_px = height;
    display_current_state = DISPLAY_STATE_BASIC_READY;
    return 0;
}

uint32_t display_id(void) {
    return DISPLAY_PRIMARY_ID;
}

uint32_t display_width(void) {
    return display_width_px;
}

uint32_t display_height(void) {
    return display_height_px;
}

uint32_t display_refresh_rate(void) {
    return 60u;
}

uint32_t display_state(void) {
    return display_current_state;
}

int display_set_running(void) {
    if (display_current_state != DISPLAY_STATE_BASIC_READY) return -1;

    display_current_state = DISPLAY_STATE_RUNNING;
    return 0;
}

int display_self_test(void) {
    if (!framebuffer_available()) return -1;
    if (display_current_state != DISPLAY_STATE_BASIC_READY) return -1;
    if (display_id() != DISPLAY_PRIMARY_ID) return -1;
    if (display_width() != framebuffer_width()) return -1;
    if (display_height() != framebuffer_height()) return -1;
    if (display_refresh_rate() != 60u) return -1;

    if (display_set_running() != 0) return -1;
    if (display_state() != DISPLAY_STATE_RUNNING) return -1;

    /* Zustand fuer den restlichen Boot wieder auf BASIC_READY zuruecksetzen,
       damit kmain.c die eigentliche Running-Transition nach dem
       Desktop-Rendering selbst auslösen kann. */
    display_current_state = DISPLAY_STATE_BASIC_READY;

    return 0;
}
