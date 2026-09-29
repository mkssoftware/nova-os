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
    NOVA_BOOT_RESULT_ATTEMPT_LIMIT = 4,
    NOVA_BOOT_RESULT_HEALTH_FAILED = 5,
    NOVA_BOOT_RESULT_HEALTH_TIMED_OUT = 6,
    NOVA_BOOT_RESULT_HEALTH_DEGRADED = 7
} nova_boot_control_result_t;

typedef enum nova_boot_milestone {
    NOVA_BOOT_MILESTONE_NONE = 0,
    NOVA_BOOT_MILESTONE_BOOTLOADER_STARTED = 1,
    NOVA_BOOT_MILESTONE_KERNEL_ENTERED = 2,
    NOVA_BOOT_MILESTONE_KERNEL_INITIALIZED = 3,
    NOVA_BOOT_MILESTONE_SYSTEM_ROOT_READY = 4,
    NOVA_BOOT_MILESTONE_CRITICAL_SERVICES_READY = 5,
    NOVA_BOOT_MILESTONE_OPERATIONAL = 6,
    NOVA_BOOT_MILESTONE_HEALTH_CONFIRMED = 7
} nova_boot_milestone_t;

#define NOVA_BOOT_MILESTONE_BIT(value) (UINT32_C(1) << (uint32_t)(value))
#define NOVA_BOOT_HEALTH_REQUIRED_DESKTOP \
    (NOVA_BOOT_MILESTONE_BIT(NOVA_BOOT_MILESTONE_KERNEL_ENTERED) | \
     NOVA_BOOT_MILESTONE_BIT(NOVA_BOOT_MILESTONE_KERNEL_INITIALIZED) | \
     NOVA_BOOT_MILESTONE_BIT(NOVA_BOOT_MILESTONE_SYSTEM_ROOT_READY) | \
     NOVA_BOOT_MILESTONE_BIT(NOVA_BOOT_MILESTONE_CRITICAL_SERVICES_READY) | \
     NOVA_BOOT_MILESTONE_BIT(NOVA_BOOT_MILESTONE_OPERATIONAL))
#define NOVA_BOOT_HEALTH_ALL_MILESTONES \
    ((NOVA_BOOT_MILESTONE_BIT(NOVA_BOOT_MILESTONE_HEALTH_CONFIRMED) << 1) - 2u)

typedef enum nova_boot_health_status {
    NOVA_BOOT_HEALTH_UNKNOWN = 0,
    NOVA_BOOT_HEALTH_PENDING = 1,
    NOVA_BOOT_HEALTH_HEALTHY = 2,
    NOVA_BOOT_HEALTH_DEGRADED = 3,
    NOVA_BOOT_HEALTH_FAILED = 4,
    NOVA_BOOT_HEALTH_TIMED_OUT = 5
} nova_boot_health_status_t;

typedef struct nova_boot_health_policy {
    uint32_t required_milestones;
    bool degraded_may_commit;
} nova_boot_health_policy_t;

typedef struct nova_boot_health_evidence {
    uint32_t slot;
    uint32_t generation;
    uint32_t boot_attempt;
    uint32_t reached_milestones;
    nova_boot_milestone_t failed_milestone;
    nova_boot_health_status_t status;
    bool trust_verified;
} nova_boot_health_evidence_t;

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
    uint32_t slot_generation[2];
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
bool nova_boot_control_apply_health(nova_boot_control_record_t *record,
                                    const nova_boot_health_policy_t *policy,
                                    const nova_boot_health_evidence_t *evidence,
                                    bool capability_authorized);

bool uefi_boot_control_initialize(EFI_SYSTEM_TABLE *system_table);
const nova_boot_control_record_t *uefi_boot_control_state(void);
bool uefi_boot_control_persistent(void);
bool uefi_boot_control_requires_recovery(void);
uint32_t uefi_boot_control_select(void);
bool uefi_boot_control_begin_attempt(uint32_t slot);
bool uefi_boot_control_artifact_failed(uint32_t slot);

#endif
