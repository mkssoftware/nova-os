#include <stddef.h>
#include "../kernel/include/nova/boot_log.h"
#include "../kernel/include/nova/boot_sequence.h"
#include "../kernel/include/nova/device.h"
#include "../kernel/include/nova/panic.h"
#include "../kernel/include/nova/pmm.h"
#include "../kernel/include/nova/vfs.h"
#include "../kernel/include/nova/syscall.h"

_Static_assert(NOVA_BOOT_PHASE_COUNT==12,"Kernel boot phase count changed");
_Static_assert(NOVA_BOOT_PHASE_OPERATIONAL==11,"Kernel operational phase changed");
_Static_assert(offsetof(nova_panic_report_t,CurrentBootPhase)==32,
               "Panic ABI 1.0 prefix changed");
_Static_assert(offsetof(nova_panic_report_t,SecurityStatus)==44,
               "Panic ABI 1.1 extension changed");

int nova_kernel_abi_layout_is_valid(void)
{
    return sizeof(NovaBootSequenceApiV1)==32&&
           sizeof(NovaBootEvent)==24&&sizeof(NovaBootLogApiV1)==32&&
           sizeof(NovaDeviceRecord)==32&&sizeof(NovaDeviceApiV1)==32&&
           sizeof(NovaVfsStateV1)==32&&sizeof(NovaVfsApiV1)==32&&
           sizeof(NovaSyscallRequestV1)==32&&
           sizeof(NovaCoreExitArgumentsV1)==16&&
           sizeof(NovaIdentityResultV1)==16&&
           sizeof(nova_panic_report_t)==48&&sizeof(NovaPmmApiV1)==32;
}
