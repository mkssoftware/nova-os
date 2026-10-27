/* NovaOS C-Kernel - minimaler Framebuffer-Zugriff. Nur 32bpp wird unterstuetzt
   (Standardfall fuer UEFI GOP); andere Formate werden als nicht verfuegbar
   behandelt, bis ein Bedarf fuer sie entsteht. */
#include "framebuffer.h"

static uint8_t *fb_base = 0;
static uint32_t fb_pitch = 0;
static uint32_t fb_width = 0;
static uint32_t fb_height = 0;
static int fb_ok = 0;

int framebuffer_initialize(uint32_t address, uint32_t pitch, uint32_t width,
                            uint32_t height, uint32_t bits_per_pixel) {
    if (address == 0 || width == 0 || height == 0 || bits_per_pixel != 32) {
        fb_ok = 0;
        return -1;
    }
    fb_base = (uint8_t *)(uintptr_t)address;
    fb_pitch = pitch;
    fb_width = width;
    fb_height = height;
    fb_ok = 1;
    return 0;
}

int framebuffer_available(void) {
    return fb_ok;
}

uint32_t framebuffer_width(void) {
    return fb_width;
}

uint32_t framebuffer_height(void) {
    return fb_height;
}

uint32_t *framebuffer_pixels(void) {
    if (!fb_ok) return 0;
    return (uint32_t *)fb_base;
}

uint32_t framebuffer_stride(void) {
    if (!fb_ok) return 0;
    return fb_pitch / 4u;
}

void framebuffer_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t rgb) {
    if (!fb_ok) {
        return;
    }
    uint32_t x_end = x + w;
    uint32_t y_end = y + h;
    if (x_end > fb_width) x_end = fb_width;
    if (y_end > fb_height) y_end = fb_height;

    for (uint32_t row = y; row < y_end; ++row) {
        uint32_t *pixel_row = (uint32_t *)(fb_base + (uint64_t)row * fb_pitch);
        for (uint32_t col = x; col < x_end; ++col) {
            pixel_row[col] = rgb;
        }
    }
}

void framebuffer_clear(uint32_t rgb) {
    framebuffer_fill_rect(0, 0, fb_width, fb_height, rgb);
}

uint32_t framebuffer_get_pixel(uint32_t x, uint32_t y) {
    if (!fb_ok || x >= fb_width || y >= fb_height) {
        return 0;
    }
    uint32_t *pixel_row = (uint32_t *)(fb_base + (uint64_t)y * fb_pitch);
    return pixel_row[x];
}

void framebuffer_set_pixel(uint32_t x, uint32_t y, uint32_t rgb) {
    if (!fb_ok || x >= fb_width || y >= fb_height) {
        return;
    }
    uint32_t *pixel_row = (uint32_t *)(fb_base + (uint64_t)y * fb_pitch);
    pixel_row[x] = rgb;
}
