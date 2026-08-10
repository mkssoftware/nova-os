#ifndef NOVA_BOOT_FRAMEBUFFER_BACKEND_H
#define NOVA_BOOT_FRAMEBUFFER_BACKEND_H

#include <stdbool.h>
#include <stdint.h>
#include "graphics.h"
#include "compositor.h"

#define NOVA_FB_DAMAGE_CAPACITY 32u
typedef enum {NOVA_FB_BACKEND_VBE,NOVA_FB_BACKEND_GOP,NOVA_FB_BACKEND_TEST,
    NOVA_FB_BACKEND_VIRTUAL} nova_fb_backend_type_t;
typedef enum {NOVA_FB_SINGLE_BUFFER,NOVA_FB_DOUBLE_BUFFER} nova_fb_buffer_mode_t;
typedef enum {NOVA_FB_OK,NOVA_FB_INVALID_ARGUMENT,NOVA_FB_INVALID_ADDRESS,
    NOVA_FB_INVALID_PITCH,NOVA_FB_UNSUPPORTED_FORMAT,NOVA_FB_MAPPING_FAILED,
    NOVA_FB_PRESENT_FAILED,NOVA_FB_INVALID_STATE,NOVA_FB_BOUNDS_ERROR}
nova_fb_result_t;
typedef struct {nova_fb_backend_type_t backend;uint32_t width,height,pitch,bits_per_pixel;
    nova_pixel_format_t format;void *framebuffer;uint64_t framebuffer_size,address;
    nova_fb_buffer_mode_t buffering;uint32_t capabilities;bool mapped,initialized;}
nova_framebuffer_info_t;
typedef struct {uint32_t initializations,shutdowns,frames,presents,full_presents,
    partial_presents,rejected_presents,pixel_reads,pixel_writes,line_writes,
    rectangle_writes,block_copies,damage_regions,damage_overflows,bounds_errors,
    mapping_errors,format_errors,last_error;uint64_t bytes_written;}
nova_framebuffer_diagnostics_t;

nova_fb_result_t nova_framebuffer_initialize(void);
nova_fb_result_t nova_framebuffer_begin_frame(void);
nova_fb_result_t nova_framebuffer_damage(nova_rect_t region);
nova_fb_result_t nova_framebuffer_present(const uint32_t *pixels,uint32_t width,
    uint32_t height,uint32_t stride);
nova_fb_result_t nova_framebuffer_write_pixel(int32_t x,int32_t y,uint32_t color);
nova_fb_result_t nova_framebuffer_read_pixel(int32_t x,int32_t y,uint32_t *color);
nova_fb_result_t nova_framebuffer_horizontal_line(int32_t x,int32_t y,int32_t width,uint32_t color);
nova_fb_result_t nova_framebuffer_vertical_line(int32_t x,int32_t y,int32_t height,uint32_t color);
nova_fb_result_t nova_framebuffer_rectangle(nova_rect_t rect,uint32_t color);
nova_fb_result_t nova_framebuffer_copy(nova_rect_t destination,const uint32_t *source,uint32_t stride);
void nova_framebuffer_shutdown(void);
const nova_framebuffer_info_t *nova_framebuffer_info(void);
const nova_framebuffer_diagnostics_t *nova_framebuffer_diagnostics(void);

#endif
