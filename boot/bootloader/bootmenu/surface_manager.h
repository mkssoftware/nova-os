#ifndef NOVA_BOOT_SURFACE_MANAGER_H
#define NOVA_BOOT_SURFACE_MANAGER_H

#include <stdbool.h>
#include <stdint.h>
#include "compositor.h"
#include "graphics.h"

#define NOVA_MANAGED_SURFACE_CAPACITY 256u
#define NOVA_MANAGED_SURFACE_MAX_DIMENSION 8192u
#define NOVA_MANAGED_SURFACE_DEFAULT_MAX_SIZE (32u*1024u*1024u)
typedef uint32_t nova_surface_handle_t;
typedef enum {NOVA_SURFACE_FRONTBUFFER,NOVA_SURFACE_BACKBUFFER,NOVA_SURFACE_PRESENT,
    NOVA_SURFACE_LAYER,NOVA_SURFACE_OFFSCREEN,NOVA_SURFACE_EFFECT,NOVA_SURFACE_MASK,
    NOVA_SURFACE_GLYPH_ATLAS,NOVA_SURFACE_IMAGE_CACHE,NOVA_SURFACE_TEMPORARY,
    NOVA_SURFACE_EMERGENCY} nova_managed_surface_type_t;
typedef enum {NOVA_SURFACE_POOL_PERSISTENT,NOVA_SURFACE_POOL_SCENE,
    NOVA_SURFACE_POOL_EFFECT,NOVA_SURFACE_POOL_FRAME,NOVA_SURFACE_POOL_EMERGENCY,
    NOVA_SURFACE_POOL_COUNT} nova_surface_pool_t;
typedef enum {NOVA_SURFACE_UNALLOCATED,NOVA_SURFACE_RESERVED,NOVA_SURFACE_INITIALIZED,
    NOVA_SURFACE_READY,NOVA_SURFACE_IN_USE,NOVA_SURFACE_IDLE,NOVA_SURFACE_RECYCLED,
    NOVA_SURFACE_RELEASED,NOVA_SURFACE_FAILED} nova_surface_state_t;
typedef enum {NOVA_SURFACE_CLEAR_NONE,NOVA_SURFACE_CLEAR_TRANSPARENT,
    NOVA_SURFACE_CLEAR_OPAQUE_BLACK,NOVA_SURFACE_CLEAR_THEME_BACKGROUND,
    NOVA_SURFACE_CLEAR_CUSTOM} nova_surface_clear_mode_t;
typedef enum {NOVA_SURFACE_OK,NOVA_SURFACE_ERR_INVALID_ARGUMENT,
    NOVA_SURFACE_ERR_INVALID_ID,NOVA_SURFACE_ERR_OUT_OF_MEMORY,
    NOVA_SURFACE_ERR_BUDGET_EXCEEDED,NOVA_SURFACE_ERR_SIZE_OVERFLOW,
    NOVA_SURFACE_ERR_UNSUPPORTED_FORMAT,NOVA_SURFACE_ERR_LOCKED,
    NOVA_SURFACE_ERR_STILL_REFERENCED,NOVA_SURFACE_ERR_INVALID_STATE}
nova_surface_result_t;
typedef struct {nova_managed_surface_type_t type;uint32_t width,height;
    nova_pixel_format_t format;uint32_t usage_flags;nova_surface_clear_mode_t clear_mode;
    uint32_t clear_color,owner_layer;nova_surface_pool_t pool;uint64_t maximum_size;}
nova_surface_descriptor_t;
typedef struct {uint32_t *pixels,width,height,pitch;nova_pixel_format_t format;}
nova_surface_mapping_t;
typedef struct {nova_surface_handle_t handle;nova_managed_surface_type_t type;
    nova_surface_state_t state;nova_surface_pool_t pool;nova_pixel_format_t format;
    uint32_t width,height,pitch,bytes_per_pixel,owner_layer,flags,references,locks;
    uint64_t allocation_size,generation;bool external;}
nova_surface_info_t;
typedef struct {uint32_t creations,imports,reuses,releases,locks,unlocks,
    invalid_handles,size_overflows,budget_failures,lock_conflicts,reference_overflows,
    frame_resets,emergency_activations;uint64_t used[NOVA_SURFACE_POOL_COUNT],
    peak[NOVA_SURFACE_POOL_COUNT];nova_surface_handle_t emergency;}
nova_surface_manager_diagnostics_t;

void nova_surface_manager_initialize(void);
nova_surface_result_t nova_managed_surface_create(const nova_surface_descriptor_t *descriptor,
    nova_surface_handle_t *out_handle);
nova_surface_result_t nova_managed_surface_import(nova_surface_t *surface,
    nova_managed_surface_type_t type,nova_surface_pool_t pool,uint32_t owner,
    nova_surface_handle_t *out_handle);
nova_surface_result_t nova_managed_surface_retain(nova_surface_handle_t handle);
nova_surface_result_t nova_managed_surface_release(nova_surface_handle_t handle);
nova_surface_result_t nova_managed_surface_lock(nova_surface_handle_t handle,
    nova_surface_mapping_t *mapping);
nova_surface_result_t nova_managed_surface_unlock(nova_surface_handle_t handle,
    const nova_rect_t *modified);
nova_surface_result_t nova_managed_surface_clear(nova_surface_handle_t handle,uint32_t color);
nova_surface_result_t nova_managed_surface_info(nova_surface_handle_t handle,
    nova_surface_info_t *info);
nova_surface_t *nova_managed_surface_native(nova_surface_handle_t handle);
bool nova_surface_manager_reset_frame(void);
nova_surface_handle_t nova_surface_manager_emergency(void);
const nova_surface_manager_diagnostics_t *nova_surface_manager_diagnostics(void);

#endif
