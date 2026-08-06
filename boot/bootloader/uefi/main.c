#include "uefi_min.h"
#include "../bootmenu/motion.h"

EFI_STATUS grafik_init(EFI_SYSTEM_TABLE *system_table);
bool bootmenu_initialize(void);
void bootmenu_draw(UINTN selection, uint8_t opacity);
EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table);

/*
 * Keep one absolute image reference so the PE/COFF linker emits a .reloc
 * directory.  UEFI firmware is free to load an application away from its
 * preferred image base and rejects non-relocatable images in that case.
 */
void *uefi_relocation_anchor = (void *)&efi_main;

static EFI_INPUT_KEY read_key(EFI_SYSTEM_TABLE *system_table)
{
    EFI_INPUT_KEY key = {0, 0};
    if (!system_table->ConIn ||
        EFI_ERROR(system_table->ConIn->ReadKeyStroke(system_table->ConIn, &key))) {
        key.ScanCode = 0;
        key.UnicodeChar = 0;
    }
    return key;
}

static void boot_selected(UINTN selection)
{
    if (selection == 0) nova_debug_string("UEFI:START\n");
    else if (selection == 1) nova_debug_string("UEFI:INSTALL-UNAVAILABLE\n");
    else if (selection == 2) nova_debug_string("UEFI:SETTINGS\n");
    else if (selection == 3) nova_debug_string("UEFI:DIAGNOSTICS\n");
    else if (selection == 4) nova_debug_string("UEFI:RECOVERY\n");
    else nova_debug_string("UEFI:POWEROFF\n");
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table)
{
    (void)image_handle;
    nova_debug_string("UEFI:NOVA-ENTRY\n");
    if (!system_table || !system_table->BootServices ||
        EFI_ERROR(grafik_init(system_table))) {
        nova_debug_string("UEFI:GOP-UNAVAILABLE\n");
        return 1;
    }

    UINTN selection = 0;
    if (!bootmenu_initialize()) {
        nova_debug_string("UEFI:COMPOSITOR-UNAVAILABLE\n");
        return 1;
    }
    nova_motion_initialize();
    int32_t entrance_opacity = 0;
    nova_animation_t entrance = {
        &entrance_opacity, 0, 255, 0, 0, 180, 1, 3, 0,
        NOVA_PROPERTY_OPACITY, NOVA_EASE_OUT_CUBIC, NOVA_MOTION_CREATED,
        false, false, true
    };
    if (!nova_motion_create(&entrance)) return 1;
    for (uint64_t elapsed = 0; elapsed <= 180; elapsed += 20) {
        nova_motion_update(elapsed);
        bootmenu_draw(selection, (uint8_t)entrance_opacity);
        system_table->BootServices->Stall(20000);
    }
    nova_debug_string("UEFI:MOTION-READY\n");

    /* Fuenf Sekunden; jede Taste beendet den automatischen Start. */
    EFI_INPUT_KEY key = {0, 0};
    for (UINTN tick = 0; tick < 50; ++tick) {
        system_table->BootServices->Stall(100000);
        key = read_key(system_table);
        if (key.ScanCode || key.UnicodeChar) {
            nova_debug_string("UEFI:COUNTDOWN-CANCELLED\n");
            break;
        }
    }

    if (!key.ScanCode && !key.UnicodeChar) {
        boot_selected(0);
        return EFI_SUCCESS;
    }

    for (;;) {
        if (key.ScanCode == 1) {
            selection = selection ? selection - 1 : 5;
            bootmenu_draw(selection, 255);
        } else if (key.ScanCode == 2) {
            selection = (selection + 1) % 6;
            bootmenu_draw(selection, 255);
        } else if (key.UnicodeChar == 13) {
            boot_selected(selection);
            return EFI_SUCCESS;
        } else if (key.ScanCode == 23) {
            nova_debug_string("UEFI:ESC\n");
            return EFI_SUCCESS;
        }
        do {
            system_table->BootServices->Stall(20000);
            key = read_key(system_table);
        } while (!key.ScanCode && !key.UnicodeChar);
    }
}
