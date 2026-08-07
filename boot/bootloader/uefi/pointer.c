#include "uefi_min.h"
#include <stdbool.h>
#include <stdint.h>

static EFI_SIMPLE_POINTER_PROTOCOL *pointer;
static EFI_GUID pointer_guid = {
    0x31878c87, 0x0b75, 0x11d5,
    {0x9a,0x4f,0x00,0x90,0x27,0x3f,0xc1,0x4d}
};

bool uefi_pointer_initialize(EFI_SYSTEM_TABLE *system_table)
{
    EFI_STATUS status = system_table->BootServices->LocateProtocol(
        &pointer_guid, 0, (VOID **)&pointer);
    if (EFI_ERROR(status) || !pointer || !pointer->GetState) {
        pointer = 0;
        nova_debug_string("UEFI:POINTER-UNAVAILABLE\n");
        return false;
    }
    if (pointer->Reset) pointer->Reset(pointer, false);
    nova_debug_string("UEFI:POINTER-READY\n");
    return true;
}

bool uefi_pointer_poll(int32_t *dx, int32_t *dy, int32_t *wheel,
                       bool *left, bool *right)
{
    if (!pointer) return false;
    EFI_SIMPLE_POINTER_STATE state;
    EFI_STATUS status = pointer->GetState(pointer, &state);
    if (EFI_ERROR(status)) return false;
    if (dx) *dx = state.RelativeMovementX;
    if (dy) *dy = state.RelativeMovementY;
    if (wheel) *wheel = state.RelativeMovementZ;
    if (left) *left = state.LeftButton;
    if (right) *right = state.RightButton;
    /* EFI_NOT_READY kennzeichnet Stillstand; EFI_SUCCESS ist auch beim Loslassen
       einer Taste ein echtes Ereignis mit komplett leerem Zustand. */
    return true;
}
