#ifndef NOVA_TRANSACTION_H
#define NOVA_TRANSACTION_H

#include <stdint.h>

#define NOVA_TRANSACTION_ABI_MAJOR 1u
#define NOVA_TRANSACTION_ABI_MINOR 0u

typedef struct NovaTransactionIdV1 {
    uint64_t High;
    uint64_t Low;
} NovaTransactionIdV1;

typedef enum NovaTransactionState {
    NOVA_TRANSACTION_CREATED = 1,
    NOVA_TRANSACTION_ACTIVE = 2,
    NOVA_TRANSACTION_VALIDATING = 3,
    NOVA_TRANSACTION_PREPARING = 4,
    NOVA_TRANSACTION_PREPARED = 5,
    NOVA_TRANSACTION_COMMITTING = 6,
    NOVA_TRANSACTION_COMMITTED = 7,
    NOVA_TRANSACTION_VERIFYING = 8,
    NOVA_TRANSACTION_COMPLETED = 9,
    NOVA_TRANSACTION_ABORTING = 10,
    NOVA_TRANSACTION_ABORTED = 11,
    NOVA_TRANSACTION_FAILED = 12,
    NOVA_TRANSACTION_UNKNOWN = 13
} NovaTransactionState;

typedef enum NovaTransactionScope {
    NOVA_TRANSACTION_SCOPE_OBJECT = 1,
    NOVA_TRANSACTION_SCOPE_FILE = 2,
    NOVA_TRANSACTION_SCOPE_CONFIGURATION = 3,
    NOVA_TRANSACTION_SCOPE_CAPABILITY = 4,
    NOVA_TRANSACTION_SCOPE_PROCESS = 5,
    NOVA_TRANSACTION_SCOPE_SERVICE = 6,
    NOVA_TRANSACTION_SCOPE_SYSTEM_UPDATE = 7
} NovaTransactionScope;

typedef struct NovaTransactionRecordV1 {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    NovaTransactionIdV1 TransactionId;
    NovaTransactionIdV1 ParentTransactionId;
    uint32_t OwnerId;
    uint32_t Scope;
    uint32_t State;
    uint32_t OperationCount;
    uint64_t DeadlineTick;
    uint64_t ResourceBudgetId;
    uint64_t ExpectedStateVersion;
    uint64_t ProvenanceId;
    uint32_t Flags;
    uint32_t Reserved;
} NovaTransactionRecordV1;

typedef struct NovaTransactionDecisionV1 {
    uint32_t StructSize;
    uint16_t AbiMajor;
    uint16_t AbiMinor;
    NovaTransactionIdV1 TransactionId;
    uint32_t PreviousState;
    uint32_t NewState;
    int32_t Status;
    uint32_t VerificationState;
    uint64_t CommitGeneration;
    uint64_t ConflictStateId;
    uint32_t Reserved[2];
} NovaTransactionDecisionV1;

_Static_assert(sizeof(NovaTransactionIdV1) == 16,
               "NovaTransactionIdV1 ABI size changed");
_Static_assert(sizeof(NovaTransactionRecordV1) == 96,
               "NovaTransactionRecordV1 ABI size changed");
_Static_assert(sizeof(NovaTransactionDecisionV1) == 64,
               "NovaTransactionDecisionV1 ABI size changed");

#endif
