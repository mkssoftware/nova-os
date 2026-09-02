#ifndef NOVA_BOOT_SEQUENCE_H
#define NOVA_BOOT_SEQUENCE_H
#include <stdint.h>
#define NOVA_BOOT_SEQUENCE_ABI_MAJOR 1u
#define NOVA_BOOT_SEQUENCE_ABI_MINOR 0u
typedef enum NovaBootPhase {
    NOVA_BOOT_PHASE_KERNEL_ENTRY=0,
    NOVA_BOOT_PHASE_HANDOFF_VALIDATION=1,
    NOVA_BOOT_PHASE_EARLY_ARCHITECTURE=2,
    NOVA_BOOT_PHASE_EARLY_MEMORY=3,
    NOVA_BOOT_PHASE_VIRTUAL_MEMORY=4,
    NOVA_BOOT_PHASE_KERNEL_CORE=5,
    NOVA_BOOT_PHASE_INTERRUPTS_AND_TIME=6,
    NOVA_BOOT_PHASE_SCHEDULER_AND_SMP=7,
    NOVA_BOOT_PHASE_DEVICE_DISCOVERY=8,
    NOVA_BOOT_PHASE_ROOT_FILESYSTEM=9,
    NOVA_BOOT_PHASE_USERSPACE=10,
    NOVA_BOOT_PHASE_OPERATIONAL=11,
    NOVA_BOOT_PHASE_COUNT=12,
    NOVA_BOOT_PHASE_NONE=0xffffffffu
} NovaBootPhase;
typedef enum NovaInitResult {
    NOVA_INIT_SUCCESS=0,NOVA_INIT_DEGRADED=1,NOVA_INIT_RETRY=2,
    NOVA_INIT_RECOVERY_REQUIRED=3,NOVA_INIT_FATAL=4
} NovaInitResult;
typedef struct NovaBootSequenceApiV1 {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t CurrentPhaseAddress;
    uint32_t LastSuccessfulPhaseAddress;
    uint32_t PhaseCount;
    uint32_t Reserved[3];
} NovaBootSequenceApiV1;
_Static_assert(sizeof(NovaBootSequenceApiV1)==32,"NovaBootSequenceApiV1 ABI size");
#endif
