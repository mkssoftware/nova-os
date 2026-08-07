#include "uefi_min.h"
#include <stdbool.h>

static EFI_RUNTIME_SERVICES *runtime;

bool uefi_power_initialize(EFI_SYSTEM_TABLE *system_table)
{
    runtime = system_table ? system_table->RuntimeServices : 0;
    if (!runtime || !runtime->ResetSystem) {
        nova_debug_string("UEFI:POWER-UNAVAILABLE\n");
        runtime = 0;
        return false;
    }
    nova_debug_string("UEFI:POWER-READY\n");
    return true;
}

bool uefi_power_execute(bool shutdown)
{
    if (!runtime) return false;
    nova_debug_string(shutdown ? "UEFI:POWER-SHUTDOWN\n" : "UEFI:POWER-RESTART\n");
    runtime->ResetSystem(shutdown ? EfiResetShutdown : EfiResetCold,
                         EFI_SUCCESS, 0, 0);
    nova_debug_string("UEFI:POWER-FAILED\n");
    return false;
}
