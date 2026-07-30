#ifndef NOVA_KERNEL_INTERRUPTS_H
#define NOVA_KERNEL_INTERRUPTS_H

#include <stdint.h>

#define NOVA_INTERRUPT_ABI_MAJOR 1u
#define NOVA_INTERRUPT_ABI_MINOR 0u

#define NOVA_INTERRUPT_CAP_IDT256 (1u << 0)
#define NOVA_INTERRUPT_CAP_PIC8259 (1u << 1)
#define NOVA_INTERRUPT_CAP_PIT     (1u << 2)

typedef struct NovaInterruptApiV1 {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t VectorCount;
    uint32_t Capabilities;
    uint32_t EnableEntry;
    uint32_t DisableEntry;
    uint32_t TimerTicksEntry;
    uint32_t TimerFrequencyHz;
} NovaInterruptApiV1;

_Static_assert(sizeof(NovaInterruptApiV1) == 32, "NovaInterruptApiV1 ABI size");

#endif
