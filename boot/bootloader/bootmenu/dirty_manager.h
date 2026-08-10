#ifndef NOVA_BOOT_DIRTY_MANAGER_H
#define NOVA_BOOT_DIRTY_MANAGER_H

#include <stdbool.h>
#include <stdint.h>
#include "compositor.h"

#define NOVA_DIRTY_MAX_REGIONS 64u

typedef enum {
    NOVA_DIRTY_FULL_NONE,
    NOVA_DIRTY_FULL_INITIALIZATION,
    NOVA_DIRTY_FULL_RESOLUTION,
    NOVA_DIRTY_FULL_THEME,
    NOVA_DIRTY_FULL_RECOVERY,
    NOVA_DIRTY_FULL_MEMORY,
    NOVA_DIRTY_FULL_CAPACITY,
    NOVA_DIRTY_FULL_THRESHOLD,
    NOVA_DIRTY_FULL_FORCED
} nova_dirty_full_reason_t;

typedef struct {
    nova_rect_t regions[NOVA_DIRTY_MAX_REGIONS];
    uint32_t count;
    bool full_damage;
} nova_damage_set_t;

typedef struct {
    uint32_t width, height, capacity, threshold_permille;
    uint32_t additions, accepted, rejected, duplicates, merges, clips;
    uint32_t expansions, clears, full_damage_count, partial_damage_count;
    uint32_t threshold_promotions, capacity_promotions;
    uint64_t total_region_area, maximum_region_area;
    nova_dirty_full_reason_t full_reason;
    bool initialized;
} nova_dirty_diagnostics_t;

bool nova_dirty_initialize(uint32_t width,uint32_t height,uint32_t capacity,
                           uint32_t threshold_permille);
void nova_dirty_clear(void);
bool nova_dirty_add(const nova_rect_t *rect);
bool nova_dirty_add_expanded(const nova_rect_t *rect,uint32_t blur_radius,
                             uint32_t shadow_extent,uint32_t alpha_extent);
void nova_dirty_force_full(nova_dirty_full_reason_t reason);
bool nova_dirty_is_full(void);
bool nova_dirty_intersects(nova_rect_t bounds);
const nova_damage_set_t *nova_dirty_get(void);
const nova_dirty_diagnostics_t *nova_dirty_diagnostics(void);

#endif
