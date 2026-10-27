/* NovaOS C-Kernel - Nova Task Group Manager (Port aus
   concurrency32.inc, NPSPEC-CONCURRENCY-TASKGROUP-0001). Task-Gruppen
   mit WaitAll/FailFast-Policy, Cancellation und Drain: eine Gruppe wird
   terminal (COMPLETED/CANCELLED/FAILED), sobald ihr letztes Mitglied
   terminal wird. */
#ifndef NOVA_TASKGROUP_H
#define NOVA_TASKGROUP_H

#include <stdint.h>

#define TASK_GROUP_CAPACITY 4u

#define TASK_GROUP_STATE_EMPTY      0u
#define TASK_GROUP_STATE_OPEN       1u
#define TASK_GROUP_STATE_CANCELLING 2u
#define TASK_GROUP_STATE_FAILING    3u
#define TASK_GROUP_STATE_COMPLETED  4u
#define TASK_GROUP_STATE_CANCELLED  5u
#define TASK_GROUP_STATE_FAILED     6u

#define TASK_GROUP_POLICY_WAIT_ALL  1u
#define TASK_GROUP_POLICY_FAIL_FAST 2u

#define TASK_CANCEL_REASON_GROUP 0x47525043u

typedef struct {
    uint32_t id;
    uint32_t owner;
    uint32_t scope;
    uint32_t state;
    uint32_t policy;
    uint32_t required;
    uint32_t active;
    uint32_t failures;
} task_group_record_t;

int task_group_manager_initialize(void);
int task_group_manager_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_group_id gesetzt), -1 wenn policy ungueltig,
   owner_pid unbekannt, scope_id ungueltig/nicht offen/falscher Owner,
   oder Tabelle voll. */
int task_group_create(uint32_t owner_pid, uint32_t scope_id, uint32_t policy, uint32_t *out_group_id);

/* Gibt den internen Datensatz zurueck, oder NULL wenn group_id
   unbekannt. */
task_group_record_t *task_group_lookup(uint32_t group_id);

/* 0 = Erfolg, -1 wenn group_id nicht OPEN, task_id ungueltig/nicht
   aktiv/falscher Owner/Scope, oder die Task bereits einer Gruppe
   zugeordnet ist. */
int task_group_attach(uint32_t group_id, uint32_t task_id);

/* 0 = Erfolg, -1 wenn group_id unbekannt oder nicht OPEN. */
int task_group_cancel(uint32_t group_id, uint32_t cancel_reason);

#endif
