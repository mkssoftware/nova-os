#ifndef NOVA_BOOT_GRAPHICS_H
#define NOVA_BOOT_GRAPHICS_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    NOVA_GRAPHICS_FIRMWARE_BIOS_VBE,
    NOVA_GRAPHICS_FIRMWARE_UEFI_GOP,
    NOVA_GRAPHICS_FIRMWARE_TEST
} nova_graphics_firmware_t;

typedef enum {
    NOVA_PIXEL_RGBA8888,
    NOVA_PIXEL_BGRA8888,
    NOVA_PIXEL_RGB888,
    NOVA_PIXEL_BGR888,
    NOVA_PIXEL_RGB565,
    NOVA_PIXEL_BIT_MASK
} nova_pixel_format_t;

enum {
    NOVA_GRAPHICS_ALPHA=1u,
    NOVA_GRAPHICS_BLEND=2u,
    NOVA_GRAPHICS_DOUBLE_BUFFER=4u,
    NOVA_GRAPHICS_TRIPLE_BUFFER=8u,
    NOVA_GRAPHICS_HARDWARE_BLIT=16u,
    NOVA_GRAPHICS_LINEAR_FRAMEBUFFER=32u,
    NOVA_GRAPHICS_POINTER=64u,
    NOVA_GRAPHICS_NATIVE_RESOLUTION=128u
};

typedef struct {
    uint32_t width,height,pitch,bits_per_pixel,dpi,scale_milli;
    nova_pixel_format_t pixel_format;
    nova_graphics_firmware_t firmware;
    void *framebuffer;
    uint64_t framebuffer_size,framebuffer_address;
    const uint32_t *backbuffer;
    uint32_t backbuffer_stride,capabilities;
    uint32_t red_mask,green_mask,blue_mask,alpha_mask;
    const char *vendor,*device;
    bool dpi_reliable,initialized;
} nova_graphics_context_t;

typedef struct {
    uint32_t initializations,shutdowns,presents,rejected_presents;
    uint32_t converted_pixels,direct_pixels,bounds_errors,format_errors;
    uint32_t mode_queries,mode_changes,fallbacks;
    uint64_t presented_bytes;
} nova_graphics_diagnostics_t;

typedef struct {int32_t x,y,width,height;} nova_graphics_region_t;

bool nova_graphics_initialize(const nova_graphics_context_t *description);
void nova_graphics_shutdown(void);
bool nova_graphics_present(const uint32_t *rgba,uint32_t width,uint32_t height,
                           uint32_t stride);
bool nova_graphics_present_regions(const uint32_t *rgba,uint32_t width,uint32_t height,
    uint32_t stride,const nova_graphics_region_t *regions,uint32_t region_count,bool full);
uint32_t nova_graphics_convert_pixel(uint32_t rgba,nova_pixel_format_t format,
                                    uint32_t red_mask,uint32_t green_mask,
                                    uint32_t blue_mask,uint32_t alpha_mask);
const nova_graphics_context_t *nova_graphics_context(void);
const nova_graphics_diagnostics_t *nova_graphics_diagnostics(void);
uint32_t nova_graphics_width(void);
uint32_t nova_graphics_height(void);

#endif
