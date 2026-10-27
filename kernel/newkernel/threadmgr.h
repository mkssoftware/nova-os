/* NovaOS C-Kernel - Nova Thread Manager (Port aus sync32.inc, letzter
   verbliebener Abschnitt von §016 "Praeemptiver Round-Robin-Scheduler",
   ADR-2012). Bindet Thread-Datensaetze an Scheduler-Slots (scheduler.h)
   und Tasks (task.h): jede Thread-Registrierung legt eine begleitende
   Task im Kernel-Root-Scope an (taskscope.h). War bisher blockiert durch
   die Abhaengigkeit auf `task_create`/`task_lookup`, die erst mit
   `concurrency32.inc`s Portierung verfuegbar wurden (siehe
   project_c_kernel_migration_status).

   Registriert beim Initialisieren bewusst nur die drei bereits
   bestehenden statischen Scheduler-Threads (kmain selbst fuer Slot 0,
   scheduler_thread1/2 fuer Slot 1/2) - `thread_create_dynamic` steht
   aber bereits als vollwertige API fuer spaeter dynamisch erzeugte
   Threads zur Verfuegung (siehe Selbsttest). Alle Threads laufen unter
   PID 1 (Kernelprozess) - das alte ASM erlaubt explizit nur das (siehe
   `thread_register`s PID-Pruefung). */
#ifndef NOVA_THREADMGR_H
#define NOVA_THREADMGR_H

#include <stdint.h>

#define THREAD_CAPACITY 8u /* == SCHEDULER_THREAD_COUNT, siehe scheduler.h */

#define THREAD_STATE_READY 1u

typedef struct {
    uint32_t tid;
    uint32_t pid;
    uint32_t state;
    uint32_t slot;
    uint32_t handle;
    uint32_t entry;
    uint32_t context;
    uint32_t scope;
} thread_record_t;

/* 0 = Erfolg, -1 wenn einer der drei statischen Threads nicht
   registriert werden konnte. */
int thread_manager_initialize(void);
int thread_manager_self_test(void); /* 0 = ok */

/* EAX/EDX/EBX-Aequivalent aus dem alten ASM: entry_point, pid, slot.
   0 = Erfolg (*out_tid gesetzt), -1 wenn slot ungueltig, pid unbekannt
   oder != 1 (nur der Kernelprozess darf Threads registrieren), Tabelle
   voll, oder die begleitende Task/Objekt-Erzeugung fehlschlaegt. */
int thread_register(uint32_t entry_point, uint32_t pid, uint32_t slot, uint32_t *out_tid);

/* Belegt einen freien Scheduler-Slot (scheduler_claim_dynamic_slot),
   baut dessen Kontextframe und registriert ihn. 0 = Erfolg (*out_tid
   gesetzt), -1 wenn kein freier Slot mehr frei ist oder thread_register
   fehlschlaegt (Slot wird in diesem Fall wieder freigegeben). */
int thread_create_dynamic(uint32_t entry_point, uint32_t pid, int32_t cpu_affinity, uint32_t *out_tid);

/* Gibt den internen Datensatz zurueck, oder NULL wenn tid unbekannt. */
thread_record_t *thread_lookup(uint32_t tid);

#endif
