#ifndef NOVA_BOOT_HEALTH_H
#define NOVA_BOOT_HEALTH_H

#include <stdint.h>

#define NOVA_BOOT_HEALTH_ABI_MAJOR 1u
#define NOVA_BOOT_HEALTH_ABI_MINOR 0u

enum nova_boot_health_status {
    NOVA_BOOT_HEALTH_UNKNOWN = 0,
    NOVA_BOOT_HEALTH_PENDING = 1,
    NOVA_BOOT_HEALTH_HEALTHY = 2,
    NOVA_BOOT_HEALTH_DEGRADED = 3,
    NOVA_BOOT_HEALTH_FAILED = 4,
    NOVA_BOOT_HEALTH_TIMED_OUT = 5
};

enum nova_boot_health_milestone {
    NOVA_BOOT_HEALTH_NONE = 0,
    NOVA_BOOT_HEALTH_BOOT_STARTED = 1,
    NOVA_BOOT_HEALTH_KERNEL_ENTERED = 2,
    NOVA_BOOT_HEALTH_KERNEL_INITIALIZED = 3,
    NOVA_BOOT_HEALTH_SYSTEM_ROOT_READY = 4,
    NOVA_BOOT_HEALTH_CRITICAL_SERVICES_READY = 5,
    NOVA_BOOT_HEALTH_OPERATIONAL = 6,
    NOVA_BOOT_HEALTH_CONFIRMED = 7
};

#define NOVA_BOOT_HEALTH_PROVIDER_KERNEL_CORE (1u << 0)
#define NOVA_BOOT_HEALTH_PROVIDER_MEMORY      (1u << 1)
#define NOVA_BOOT_HEALTH_PROVIDER_SYSTEM_ROOT (1u << 2)
#define NOVA_BOOT_HEALTH_PROVIDER_TRUST       (1u << 3)
#define NOVA_BOOT_HEALTH_PROVIDER_CAPABILITY  (1u << 4)
#define NOVA_BOOT_HEALTH_PROVIDER_IPC         (1u << 5)
#define NOVA_BOOT_HEALTH_PROVIDER_SESSION     (1u << 6)

enum nova_boot_health_provider_status {
    NOVA_BOOT_HEALTH_PROVIDER_READY = 1,
    NOVA_BOOT_HEALTH_PROVIDER_DEGRADED = 2,
    NOVA_BOOT_HEALTH_PROVIDER_FAILED = 3
};

typedef struct nova_boot_health_report {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint64_t Generation;
    uint32_t BootAttempt;
    uint32_t Milestone;
    uint32_t Provider;
    uint32_t ProviderStatus;
} nova_boot_health_report_t;

typedef struct nova_boot_health_record {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint64_t Generation;
    uint32_t BootAttempt;
    uint32_t Status;
    uint32_t ReachedMilestones;
    uint32_t RequiredMilestones;
    uint32_t LastMilestone;
    uint32_t FailedMilestone;
    uint32_t Sequence;
    uint32_t AuthorizedReports;
    uint32_t RejectedReports;
    uint32_t Flags;
    uint32_t Reserved[2];
} nova_boot_health_record_t;

typedef struct nova_boot_health_evidence {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint64_t Generation;
    uint32_t BootAttempt;
    uint32_t ReachedMilestones;
    uint32_t Status;
    uint32_t TrustVerified;
} nova_boot_health_evidence_t;

typedef struct nova_boot_health_api {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    uint32_t Capabilities;
    uint32_t SubmitReportEntry;
    uint32_t ExportEvidenceEntry;
    uint32_t RecordAddress;
    uint32_t ProviderReadyAddress;
    uint32_t ReportSize;
    uint32_t EvidenceSize;
    uint32_t Reserved;
} nova_boot_health_api_t;

_Static_assert(sizeof(nova_boot_health_report_t) == 32,
               "nova_boot_health_report_t ABI size changed");
_Static_assert(sizeof(nova_boot_health_record_t) == 64,
               "nova_boot_health_record_t ABI size changed");
_Static_assert(sizeof(nova_boot_health_evidence_t) == 32,
               "nova_boot_health_evidence_t ABI size changed");
_Static_assert(sizeof(nova_boot_health_api_t) == 40,
               "nova_boot_health_api_t ABI size changed");

#endif
