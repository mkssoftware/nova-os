/* NovaOS C-Kernel - Nova Task Manager (Port aus concurrency32.inc,
   NPSPEC-CONCURRENCY-TASK/CANCELLATION-0001). Verwaltete Kernel-Tasks
   innerhalb strukturierter Scopes (taskscope.h): hierarchische,
   kooperative Cancellation-Propagation sowie Completion-/Fail-Pfade.
   `io_cancel_for_requested_tasks` (io32.inc) wird NICHT aufgerufen - das
   I/O-Request-Subsystem ist noch nicht portiert, der Aufruf waere im
   ASM selbst ueber dessen eigenes Ready-Flag ohnehin ein No-Op gewesen
   (bewusste Vereinfachung, 1:1 im Effekt). */
#ifndef NOVA_TASK_H
#define NOVA_TASK_H

#include <stdint.h>

#define TASK_CAPACITY 8u

#define TASK_STATE_EMPTY          0u
#define TASK_STATE_CREATED        1u
#define TASK_STATE_READY          2u
#define TASK_STATE_RUNNING        3u
#define TASK_STATE_WAITING        4u
#define TASK_STATE_CANCEL_REQUEST 5u
#define TASK_STATE_COMPLETED      6u
#define TASK_STATE_CANCELLED      7u
#define TASK_STATE_FAILED         8u

#define TASK_CAP_HIERARCHY 0x00000001u
#define TASK_CAP_CANCEL    0x00000002u
#define TASK_CAP_RESULT    0x00000004u

typedef struct {
    uint32_t id;
    uint32_t owner;
    uint32_t scope;
    uint32_t state;
    uint32_t parent;
    uint32_t flags;
    uint32_t result;
    uint32_t cancel_reason;
} task_record_t;

int task_manager_initialize(void);
int task_manager_self_test(void); /* 0 = ok */

uint32_t task_count(void);

/* 0 = Erfolg (*out_task_id gesetzt), -1 wenn owner_pid unbekannt,
   scope_id ungueltig/nicht offen/falscher Owner, parent_task ungueltig
   (falscher Owner/Scope oder bereits terminal) oder Tabelle voll. */
int task_create(uint32_t owner_pid, uint32_t scope_id, uint32_t parent_task, uint32_t flags, uint32_t *out_task_id);

/* Gibt den internen Datensatz zurueck, oder NULL wenn task_id unbekannt. */
task_record_t *task_lookup(uint32_t task_id);

/* Paralleler Tabellenindex fuer taskdeadline.c/taskgroup.c (entspricht
   der Position von rec in der internen Task-Tabelle). */
uint32_t task_slot_index(const task_record_t *rec);

/* Direkter Zugriff auf einen Tabellenslot (0..TASK_CAPACITY-1) fuer
   taskdeadline.c/taskgroup.c, die ueber alle Slots scannen muessen
   (Pendant zum parallelen Index aus task_slot_index). Der Aufrufer
   prueft selbst rec->state (kann EMPTY sein). */
task_record_t *task_at_slot(uint32_t slot);

/* Entfernt genau eine aktive Task aus ihrem Scope. 0 = Erfolg, -1 wenn
   der Scope unbekannt ist oder keine aktiven Tasks mehr zaehlt. */
int task_release_scope(task_record_t *rec);

/* Fordert kooperativen Abbruch an und propagiert entlang der Task-
   Hierarchie; beendet aber keinen Task hart. 0 = Erfolg, -1 wenn
   task_id unbekannt oder bereits terminal. */
int task_request_cancel(uint32_t task_id, uint32_t cancel_reason);

/* Kooperativer Cancellation Point. 0 = Erfolg (*out_cancelled gesetzt),
   -1 wenn task_id unbekannt. */
int task_checkpoint(uint32_t task_id, int *out_cancelled);

/* 0 = Erfolg (Task COMPLETED/FAILED), -1 wenn task_id unbekannt oder
   nicht in einem aktiven Zustand (CREATED..WAITING). */
int task_complete(uint32_t task_id, uint32_t result);
int task_fail(uint32_t task_id, uint32_t result);

/* Implementiert in taskscope.h - markiert alle aktiven Tasks in
   CANCELLING-Scopes zur kooperativen Beendigung. Wird von
   task_scope_cancel aufgerufen; vor task_manager_initialize() ein
   No-Op. */
void task_manager_cancel_scopes(uint32_t cancel_reason);

/* Hooks, implementiert in taskgroup.c/taskdeadline.c (vermeidet
   zirkulaere Includes, gleiches Muster wie taskscope.h). Vor der
   jeweiligen Modul-Initialisierung No-Ops. */
void task_group_on_terminal(const task_record_t *terminal_task, uint32_t slot);
void task_group_on_slot_reused(uint32_t slot);
void task_deadline_on_slot_reused(uint32_t slot);

#endif
