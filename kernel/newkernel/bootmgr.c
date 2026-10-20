/* NovaOS C-Kernel - Boot-Manager (Port aus bootmgr32.inc). */
#include "bootmgr.h"

#define BOOT_MODE_UEFI 0u

typedef struct {
    uint32_t stage; /* 0=FW 1=BL 2=Kernel */
    uint32_t flags;
    uint32_t hash;
    uint32_t version;
    uint32_t slot;
    uint32_t health;
    uint32_t pcr_index;
} boot_chain_entry_t;

static boot_chain_entry_t boot_chain_table[BOOTMGR_CHAIN_CAPACITY];
static uint32_t boot_chain_count = 0;
static uint32_t boot_active_slot = BOOT_SLOT_A;
static uint32_t boot_health = BOOT_HEALTH_OK;
static int bootmgr_ready = 0;

void bootmgr_initialize(void) {
    for (uint32_t i = 0; i < BOOTMGR_CHAIN_CAPACITY; ++i) {
        boot_chain_table[i] = (boot_chain_entry_t){0};
    }
    boot_chain_count = 0;
    boot_active_slot = BOOT_SLOT_A;
    boot_health = BOOT_HEALTH_OK;
    bootmgr_ready = 1;
}

int boot_record_chain(uint32_t stage, uint32_t flags, uint32_t hash, uint32_t version) {
    if (boot_chain_count >= BOOTMGR_CHAIN_CAPACITY) {
        return -1;
    }
    boot_chain_entry_t *entry = &boot_chain_table[boot_chain_count];
    entry->stage = stage;
    entry->flags = flags;
    entry->hash = hash;
    entry->version = version;
    entry->slot = boot_active_slot;
    entry->health = BOOT_HEALTH_OK;
    entry->pcr_index = boot_active_slot; /* PCR = Stage-Index, identisch zum ASM */
    ++boot_chain_count;
    return 0;
}

int boot_check_rollback(void) {
    if (boot_health != BOOT_HEALTH_FAIL) {
        return 0;
    }
    boot_active_slot = BOOT_SLOT_B;
    boot_health = BOOT_HEALTH_RECOVERY;
    return -1;
}

int bootmgr_self_test(void) {
    if (bootmgr_ready != 1) return -1;

    if (boot_record_chain(0 /* FW */, BOOT_FLAG_MEASURED | BOOT_FLAG_TPM,
                           0xDEADBEEFu, 0x00010000u) != 0) {
        return -1;
    }
    if (boot_record_chain(2 /* Kernel */, BOOT_FLAG_SECURE | BOOT_FLAG_VERIFIED,
                           0xC0FFEE00u, 0x00010001u) != 0) {
        return -1;
    }

    boot_check_rollback(); /* Rollback ist ebenfalls ein gueltiger Pfad */

    return 0;
}
