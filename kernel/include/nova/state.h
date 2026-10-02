#ifndef NOVA_STATE_H
#define NOVA_STATE_H

#include <stdint.h>

#define NOVA_STATE_ABI_MAJOR 1u
#define NOVA_STATE_ABI_MINOR 0u
#define NOVA_STATE_UNKNOWN_VERSION 0u

typedef struct NovaStateIdV1 {
    uint64_t High;
    uint64_t Low;
} NovaStateIdV1;

typedef enum NovaStateKind {
    NOVA_STATE_KIND_KERNEL = 1,
    NOVA_STATE_KIND_RESOURCE = 2,
    NOVA_STATE_KIND_SECURITY = 3,
    NOVA_STATE_KIND_OBJECT = 4,
    NOVA_STATE_KIND_SERVICE = 5,
    NOVA_STATE_KIND_DEVICE = 6,
    NOVA_STATE_KIND_CONFIGURATION = 7
} NovaStateKind;

typedef enum NovaStateValidity {
    NOVA_STATE_VALIDITY_UNKNOWN = 0,
    NOVA_STATE_VALIDITY_VALID = 1,
    NOVA_STATE_VALIDITY_INVALID = 2,
    NOVA_STATE_VALIDITY_STALE = 3,
    NOVA_STATE_VALIDITY_CONFLICT = 4
} NovaStateValidity;

typedef struct NovaStateVersionV1 {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    NovaStateIdV1 StateId;
    uint64_t Version;
    uint64_t Generation;
    uint64_t PreviousVersion;
    uint64_t TransactionId;
    uint32_t Validity;
    uint32_t Reserved;
} NovaStateVersionV1;

typedef struct NovaStateRecordV1 {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    NovaStateIdV1 StateId;
    uint32_t StateType;
    uint32_t OwnerId;
    uint64_t Version;
    uint64_t Generation;
    uint64_t Timestamp;
    uint64_t SourceId;
    uint64_t TransactionId;
    uint32_t Validity;
    uint32_t Flags;
    uint32_t ValueSize;
    uint32_t Reserved;
} NovaStateRecordV1;

typedef struct NovaStateTransitionV1 {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    NovaStateIdV1 StateId;
    uint64_t ExpectedVersion;
    uint64_t NewVersion;
    uint64_t TransactionId;
    uint32_t FromValidity;
    uint32_t ToValidity;
    uint32_t Operation;
    uint32_t Reserved;
} NovaStateTransitionV1;

_Static_assert(sizeof(NovaStateIdV1) == 16,
               "NovaStateIdV1 ABI size changed");
_Static_assert(sizeof(NovaStateVersionV1) == 64,
               "NovaStateVersionV1 ABI size changed");
_Static_assert(sizeof(NovaStateRecordV1) == 88,
               "NovaStateRecordV1 ABI size changed");
_Static_assert(sizeof(NovaStateTransitionV1) == 64,
               "NovaStateTransitionV1 ABI size changed");

#endif
