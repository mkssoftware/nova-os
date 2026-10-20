/* NovaOS C-Kernel - ABI/API-Layer (Port aus abi32.inc). */
#include "abi.h"

typedef struct {
    uint32_t number;
    uint32_t flags;
    uint32_t handler;
    uint32_t min_version;
    uint32_t max_version;
    uint32_t arg_count;
    uint32_t intent_hash;
} abi_syscall_entry_t;

static abi_syscall_entry_t abimgr_syscall_table[ABI_SYSCTL_CAPACITY];
static uint32_t abimgr_syscall_count = 0;
static uint32_t abimgr_version = NOVA_ABI_VERSION;
static int abimgr_ready = 0;

void abimgr_initialize(void) {
    for (uint32_t i = 0; i < ABI_SYSCTL_CAPACITY; ++i) {
        abimgr_syscall_table[i] = (abi_syscall_entry_t){0};
    }
    abimgr_syscall_count = 0;
    abimgr_version = NOVA_ABI_VERSION;
    abimgr_ready = 1;
}

int abimgr_register_syscall(uint32_t number, uint32_t flags, uint32_t handler) {
    if (abimgr_syscall_count >= ABI_SYSCTL_CAPACITY) {
        return -1;
    }
    if (number >= NOVA_SYSCALL_COUNT) {
        return -1;
    }
    abi_syscall_entry_t *entry = &abimgr_syscall_table[abimgr_syscall_count];
    entry->number = number;
    entry->flags = flags;
    entry->handler = handler;
    entry->min_version = abimgr_version;
    entry->max_version = 0;
    entry->arg_count = 0;
    entry->intent_hash = 0;
    ++abimgr_syscall_count;
    return 0;
}

int abimgr_negotiate_version(uint32_t requested_version, uint32_t *out_negotiated) {
    uint32_t current = abimgr_version;
    if ((requested_version >> 16) != (current >> 16)) {
        return -1; /* Major muss gleich sein */
    }
    *out_negotiated = (requested_version <= current) ? requested_version : current;
    return 0;
}

int abimgr_self_test(void) {
    return (abimgr_ready == 1) ? 0 : -1;
}
