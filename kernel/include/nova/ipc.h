#ifndef NOVA_KERNEL_IPC_H
#define NOVA_KERNEL_IPC_H

#include <stdint.h>

#define NOVA_IPC_ABI_MAJOR 1u
#define NOVA_IPC_ABI_MINOR 0u
#define NOVA_IPC_MESSAGE_SIZE 16u

#define NOVA_IPC_CAP_FIFO     (1u << 0)
#define NOVA_IPC_CAP_IRQ_SAFE (1u << 1)

typedef struct NovaIpcApiV1 {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t MessageSize;
    uint32_t QueueCapacity;
    uint32_t Capabilities;
    uint32_t SendEntry;
    uint32_t ReceiveEntry;
    uint32_t PendingCountAddress;
} NovaIpcApiV1;

_Static_assert(sizeof(NovaIpcApiV1) == 32, "NovaIpcApiV1 ABI size");

#endif
