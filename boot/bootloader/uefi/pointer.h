#ifndef NOVA_UEFI_POINTER_H
#define NOVA_UEFI_POINTER_H

#include "uefi_min.h"
#include <stdbool.h>
#include <stdint.h>

bool uefi_pointer_initialize(EFI_SYSTEM_TABLE *system_table);
bool uefi_pointer_refresh(EFI_SYSTEM_TABLE *system_table);
bool uefi_pointer_available(void);
typedef struct {int32_t x,y,dx,dy,wheel;bool left,right,absolute;} uefi_pointer_event_t;
bool uefi_pointer_poll(uint32_t viewport_width,uint32_t viewport_height,
                       uefi_pointer_event_t *event);

#endif
