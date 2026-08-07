#ifndef NOVA_UEFI_FIRMWARE_H
#define NOVA_UEFI_FIRMWARE_H

#include "uefi_min.h"
#include <stdbool.h>

typedef struct {
    bool uefi;
    bool secure_boot_known;
    bool secure_boot;
    bool setup_mode_known;
    bool setup_mode;
    bool firmware_setup_supported;
    uint32_t revision;
    char vendor[40];
} nova_uefi_firmware_status_t;

bool uefi_firmware_initialize(EFI_SYSTEM_TABLE *system_table);
const nova_uefi_firmware_status_t *uefi_firmware_status(void);
bool uefi_firmware_refresh(void);
bool uefi_firmware_request_setup(void);

#endif
