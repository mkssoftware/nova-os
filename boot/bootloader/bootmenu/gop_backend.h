#ifndef NOVA_BOOT_GOP_BACKEND_H
#define NOVA_BOOT_GOP_BACKEND_H

#include <stdbool.h>
#include <stdint.h>
#include "graphics.h"

#define NOVA_GOP_MODE_CAPACITY 64u
typedef enum {NOVA_GOP_PIXEL_RGB_RESERVED,NOVA_GOP_PIXEL_BGR_RESERVED,
    NOVA_GOP_PIXEL_BIT_MASK,NOVA_GOP_PIXEL_BLT_ONLY} nova_gop_pixel_format_t;
typedef struct {uint32_t index,width,height,pixels_per_scanline;
    nova_gop_pixel_format_t format;uint32_t red_mask,green_mask,blue_mask,alpha_mask;}
nova_gop_mode_candidate_t;
typedef struct {uint64_t framebuffer_address,framebuffer_size;uint32_t width,height,
    pitch,bytes_per_pixel,mode_index,mode_count;nova_pixel_format_t pixel_format;
    uint32_t red_mask,green_mask,blue_mask,alpha_mask;}
nova_gop_descriptor_t;
typedef enum {NOVA_GOP_OK,NOVA_GOP_NOT_FOUND,NOVA_GOP_INVALID_MODE,
    NOVA_GOP_INVALID_FRAMEBUFFER,NOVA_GOP_FRAMEBUFFER_TOO_SMALL,
    NOVA_GOP_UNSUPPORTED_FORMAT,NOVA_GOP_INVALID_PITCH,NOVA_GOP_NOT_READY}
nova_gop_result_t;
typedef struct {uint32_t initializations,shutdowns,candidates,valid_modes,
    rejected_modes,selections,preferred_selections,current_selections,fallback_selections,
    mode_changes,descriptor_queries,errors;uint32_t selected_mode;bool available;}
nova_gop_diagnostics_t;

void nova_gop_reset(void);
bool nova_gop_validate_candidate(const nova_gop_mode_candidate_t *candidate);
nova_gop_result_t nova_gop_select_mode(const nova_gop_mode_candidate_t *modes,uint32_t count,
    uint32_t current,uint32_t preferred_width,uint32_t preferred_height,uint32_t *selected);
nova_gop_result_t nova_gop_initialize(const nova_gop_mode_candidate_t *active,
    uint32_t mode_count,uint64_t framebuffer_address,uint64_t framebuffer_size);
void nova_gop_record_mode_change(void);
nova_gop_result_t nova_gop_get_descriptor(nova_gop_descriptor_t *descriptor);
nova_gop_result_t nova_gop_shutdown(void);
bool nova_gop_is_available(void);
const nova_gop_diagnostics_t *nova_gop_diagnostics(void);

#endif
