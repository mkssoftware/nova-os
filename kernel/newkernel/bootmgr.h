/* NovaOS C-Kernel - Boot-Manager (Port aus bootmgr32.inc, NPSPEC-BOOT-*).
   BIOS-Boot wird gemaess Projektentscheidung nicht unterstuetzt - BOOT_MODE_UEFI
   ist der einzige tatsaechlich genutzte Modus. */
#ifndef NOVA_BOOTMGR_H
#define NOVA_BOOTMGR_H

#include <stdint.h>

#define BOOT_SLOT_A 0u
#define BOOT_SLOT_B 1u

#define BOOT_HEALTH_OK       0u
#define BOOT_HEALTH_DEGRADED 1u
#define BOOT_HEALTH_FAIL     2u
#define BOOT_HEALTH_RECOVERY 3u

#define BOOT_FLAG_SECURE      0x01u
#define BOOT_FLAG_VERIFIED    0x02u
#define BOOT_FLAG_MEASURED    0x04u
#define BOOT_FLAG_TPM         0x08u
#define BOOT_FLAG_ROLLBACK_EN 0x10u
#define BOOT_FLAG_RECOVERY    0x20u

#define BOOTMGR_CHAIN_CAPACITY 4u

void bootmgr_initialize(void);
int bootmgr_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn die Boot-Chain-Tabelle voll ist. */
int boot_record_chain(uint32_t stage, uint32_t flags, uint32_t hash, uint32_t version);

/* 0 = Slot A ok, -1 = Rollback auf Slot B ausgeloest (kein Fehlerfall). */
int boot_check_rollback(void);

#endif
