#ifndef NOVA_BOOT_ICONS_H
#define NOVA_BOOT_ICONS_H

#include <stdint.h>
#include <stdbool.h>
#include "compositor.h"

typedef enum {
    NOVA_ICON_BACK, NOVA_ICON_FORWARD, NOVA_ICON_HOME, NOVA_ICON_INSTALL,
    NOVA_ICON_SETTINGS, NOVA_ICON_DIAGNOSTICS, NOVA_ICON_RECOVERY,
    NOVA_ICON_POWER, NOVA_ICON_RESTART, NOVA_ICON_WARNING, NOVA_ICON_ERROR,
    NOVA_ICON_INFORMATION, NOVA_ICON_SUCCESS, NOVA_ICON_LOCK,
    NOVA_ICON_SEARCH, NOVA_ICON_COUNT
} nova_icon_token_t;
typedef struct { uint32_t lookups, cache_hits, fallbacks, renders; } nova_icon_diagnostics_t;

bool nova_icons_initialize(void);
bool nova_icon_exists(nova_icon_token_t token);
void nova_icon_draw(nova_surface_t *surface, nova_icon_token_t token,
                    int32_t x, int32_t y, uint16_t size_dlu, uint32_t color);
const nova_icon_diagnostics_t *nova_icon_diagnostics(void);

#endif
