#ifndef NOVA_UEFI_BOOT_CONTROL_H
#define NOVA_UEFI_BOOT_CONTROL_H

#include "uefi_min.h"
#include <stdbool.h>
#include <stdint.h>

#define NOVA_BOOT_CONTROL_VERSION 1u
#define NOVA_BOOT_CONTROL_NO_SLOT 0xffffffffu
#define NOVA_BOOT_CONTROL_DEFAULT_MAX_ATTEMPTS 2u

typedef enum nova_boot_control_result {
    NOVA_BOOT_RESULT_UNKNOWN = 0,
    NOVA_BOOT_RESULT_PENDING = 1,
    NOVA_BOOT_RESULT_HEALTH_CONFIRMED = 2,
    NOVA_BOOT_RESULT_ARTIFACT_INVALID = 3,
    NOVA_BOOT_RESULT_ATTEMPT_LIMIT = 4
} nova_boot_control_result_t;

typedef enum nova_boot_milestone {
    NOVA_BOOT_MILESTONE_NONE = 0,
    NOVA_BOOT_MILESTONE_BOOTLOADER_STARTED = 1,
    NOVA_BOOT_MILESTONE_KERNEL_ENTERED = 2,
    NOVA_BOOT_MILESTONE_KERNEL_INITIALIZED = 3,
    NOVA_BOOT_MILESTONE_SYSTEM_ROOT_READY = 4,
    NOVA_BOOT_MILESTONE_CRITICAL_SERVICES_READY = 5,
    NOVA_BOOT_MILESTONE_OPERATIONAL = 6
} nova_boot_milestone_t;

#pragma pack(push, 1)
typedef struct nova_boot_control_record {
    uint8_t magic[8];
    uint16_t version;
    uint16_t size;
    uint64_t sequence;
    uint32_t active_slot;
    uint32_t candidate_slot;
    uint32_t known_good_slot;
    uint32_t attempt_count;
    uint32_t max_attempts;
    uint32_t last_result;
    uint32_t last_milestone;
    uint32_t flags;
    uint32_t checksum;
    uint8_t reserved[8];
} nova_boot_control_record_t;
#pragma pack(pop)

_Static_assert(sizeof(nova_boot_control_record_t) == 64,
               "Boot-Control-Datensatz muss 64 Byte gross sein");

void nova_boot_control_default(nova_boot_control_record_t *record);
bool nova_boot_control_validate(const nova_boot_control_record_t *record);
bool nova_boot_control_choose_newest(const nova_boot_control_record_t *copy_a,
                                     const nova_boot_control_record_t *copy_b,
                                     nova_boot_control_record_t *selected);
bool nova_boot_control_prepare_candidate(nova_boot_control_record_t *record, uint32_t slot,
                                         uint32_t max_attempts);
uint32_t nova_boot_control_select(nova_boot_control_record_t *record, bool *state_changed);
bool nova_boot_control_begin_attempt(nova_boot_control_record_t *record, uint32_t slot);
bool nova_boot_control_artifact_failed(nova_boot_control_record_t *record, uint32_t slot);

bool uefi_boot_control_initialize(EFI_SYSTEM_TABLE *system_table);
const nova_boot_control_record_t *uefi_boot_control_state(void);
bool uefi_boot_control_persistent(void);
uint32_t uefi_boot_control_select(void);
bool uefi_boot_control_begin_attempt(uint32_t slot);
bool uefi_boot_control_artifact_failed(uint32_t slot);

#endif
