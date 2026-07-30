#ifndef NOVA_VERSION_H
#define NOVA_VERSION_H

#include "nova_types.h"

/*
 * Nova OS – Versionsstruktur
 *
 * Wird für Module, ABI-Strukturen, Treiber, Dienste und spätere APIs genutzt.
 */

typedef struct
{
    nova_u16 major;
    nova_u16 minor;
    nova_u16 patch;
    nova_u16 build;
} nova_version;

#define NOVA_VERSION_MAKE(major_value, minor_value, patch_value, build_value) \
    ((nova_version){                                                         \
        (nova_u16)(major_value),                                             \
        (nova_u16)(minor_value),                                             \
        (nova_u16)(patch_value),                                             \
        (nova_u16)(build_value)                                              \
    })

#endif