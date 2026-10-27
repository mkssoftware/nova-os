/* NovaOS C-Kernel - Nova Task Scope Manager (Port aus concurrency32.inc,
   NPSPEC-CONCURRENCY-TASK/STRUCTURED/CANCELLATION-0001). Strukturierte
   Concurrency-Scopes mit Hierarchie und kooperativer Cancellation-
   Propagation; feste Tabellenkapazitaet begrenzt Laufzeit/Speicher
   deterministisch. Der initiale Kernelprozess (PID 1) erhaelt beim
   Initialisieren einen dauerhaften Root-Scope. */
#ifndef NOVA_TASKSCOPE_H
#define NOVA_TASKSCOPE_H

#include <stdint.h>

#define TASK_SCOPE_CAPACITY 8u

#define TASK_SCOPE_STATE_EMPTY      0u
#define TASK_SCOPE_STATE_OPEN       1u
#define TASK_SCOPE_STATE_CANCELLING 2u
#define TASK_SCOPE_STATE_CLOSED     3u

#define TASK_SCOPE_CAP_HIERARCHY    0x00000001u
#define TASK_SCOPE_CAP_CANCELLATION 0x00000002u
#define TASK_SCOPE_CAP_BOUNDED      0x00000004u

typedef struct {
    uint32_t id;
    uint32_t owner;
    uint32_t parent;
    uint32_t state;
    uint32_t children;
    uint32_t flags;
    uint32_t cancel_reason;
    uint32_t active_tasks;
} task_scope_record_t;

extern uint32_t task_scope_kernel_root_id;

/* 0 = Erfolg (task_scope_kernel_root_id gesetzt), -1 wenn der Root-Scope
   nicht angelegt werden konnte. */
int task_scope_manager_initialize(void);
int task_scope_manager_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_scope_id gesetzt), -1 wenn owner_pid unbekannt,
   parent_scope ungueltig/nicht offen/falscher Owner, oder Tabelle voll. */
int task_scope_create(uint32_t owner_pid, uint32_t parent_scope, uint32_t flags, uint32_t *out_scope_id);

/* Gibt den internen Datensatz zurueck, oder NULL wenn scope_id unbekannt. */
task_scope_record_t *task_scope_lookup(uint32_t scope_id);

/* Markiert den Scope (und hierarchisch alle Kind-Scopes offener Eltern)
   als CANCELLING und ruft `task_manager_cancel_scopes` (task.h) auf, um
   aktive Tasks darin kooperativ zur Beendigung zu markieren. 0 = Erfolg,
   -1 wenn scope_id unbekannt oder bereits CLOSED. */
int task_scope_cancel(uint32_t scope_id, uint32_t cancel_reason);

/* 0 = Erfolg (Scope CLOSED), -1 wenn scope_id unbekannt, nicht OPEN/
   CANCELLING, oder noch Kinder/aktive Tasks hat. */
int task_scope_close(uint32_t scope_id);

/* Implementiert in task.c (siehe task.h) - markiert alle aktiven Tasks in
   CANCELLING-Scopes zur kooperativen Beendigung. Hier nur deklariert, um
   einen zirkulaeren Include zwischen taskscope.h und task.h zu vermeiden.
   Vor task_manager_initialize() ein No-Op (siehe task.c). */
void task_manager_cancel_scopes(uint32_t cancel_reason);

#endif
