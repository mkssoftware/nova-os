#ifndef NOVA_UEFI_KERNEL_LOADER_H
#define NOVA_UEFI_KERNEL_LOADER_H

#include "uefi_min.h"

EFI_STATUS uefi_boot_kernel(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table);

#endif
