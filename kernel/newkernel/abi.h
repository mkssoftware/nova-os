/* NovaOS C-Kernel - ABI/API-Layer (Port aus abi32.inc, NPSPEC-ABI- und NPSPEC-API-Spezifikationen). */
#ifndef NOVA_ABI_H
#define NOVA_ABI_H

#include <stdint.h>

#define NOVA_ABI_VERSION_MAJOR 1u
#define NOVA_ABI_VERSION_MINOR 0u
#define NOVA_ABI_VERSION_PATCH 0u
#define NOVA_ABI_VERSION ((NOVA_ABI_VERSION_MAJOR << 16) | (NOVA_ABI_VERSION_MINOR << 8) | NOVA_ABI_VERSION_PATCH)

#define NOVA_SYSCALL_EXIT       0u
#define NOVA_SYSCALL_WRITE      1u
#define NOVA_SYSCALL_READ       2u
#define NOVA_SYSCALL_OPEN       3u
#define NOVA_SYSCALL_CLOSE      4u
#define NOVA_SYSCALL_IPC_SEND   5u
#define NOVA_SYSCALL_IPC_RECV   6u
#define NOVA_SYSCALL_CAP_CREATE 7u
#define NOVA_SYSCALL_CAP_INVOKE 8u
#define NOVA_SYSCALL_MEM_MAP    9u
#define NOVA_SYSCALL_THREAD_NEW 10u
#define NOVA_SYSCALL_COUNT      11u

#define ABI_FLAG_STABLE    0x01u
#define ABI_FLAG_VERSIONED 0x02u
#define ABI_FLAG_INTENT    0x04u
#define ABI_FLAG_SEMANTIC  0x08u
#define ABI_FLAG_FFI       0x10u
#define ABI_FLAG_INTERNAL  0x20u

#define ABI_SYSCTL_CAPACITY 16u

void abimgr_initialize(void);
int abimgr_self_test(void); /* 0 = ok (identisch zum alten ASM: prueft nur abimgr_ready) */

/* 0 = Erfolg, -1 wenn Tabelle voll oder number >= NOVA_SYSCALL_COUNT. */
int abimgr_register_syscall(uint32_t number, uint32_t flags, uint32_t handler);

/* Negotiated-Version in *out_negotiated. 0 = kompatibel (Major stimmt). */
int abimgr_negotiate_version(uint32_t requested_version, uint32_t *out_negotiated);

#endif
