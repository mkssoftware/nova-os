/* NovaOS C-Kernel - Scheduler-Familie (Port aus scheduler32.inc,
   NPSPEC-SCHEDULER-FAIR-0001 / NPSPEC-SCHEDULER-REALTIME-0001 /
   NPSPEC-SCHEDULER-QOS-0001 / NPSPEC-CONCURRENCY-WORKSTEALING-0001).

   WICHTIG (Nutzer-Entscheidung 2026-10-26): Nur Datenstrukturen + Selbsttests
   portiert, OHNE echten Taskwechsel. scheduler_on_tick wird NICHT an
   interrupt_dispatch_c angebunden - der bereits verifizierte Minimal-Dispatch
   aus interrupt.c bleibt unveraendert. scheduler_initialize/create_frame
   bauen echte, gueltige Interrupt-Frames (kompatibel mit interrupt_isr.asm's
   Stack-Layout), aber niemand schaltet bisher tatsaechlich auf sie um.
   Echtes Preemptive-Multitasking (inkl. SMP/AP-Bringup aus dem noch nicht
   portierten sync32.inc) ist ein spaeterer Schritt. */
#ifndef NOVA_SCHEDULER_H
#define NOVA_SCHEDULER_H

#include <stdint.h>

#define SCHEDULER_THREAD_COUNT 8u
#define SCHEDULER_FRAME_SIZE   68u

/* Erzeugt zwei synthetische Thread-Kontexte (scheduler_thread1/2). 0 = ok. */
void scheduler_initialize(void);
/* Reduzierter Selbsttest (siehe Hinweis oben): prueft Initialisierung und
   dass beide Thread-Frames gueltig aufgebaut wurden, NICHT dass die Threads
   real liefen (das setzt echten Taskwechsel voraus). 0 = ok. */
int scheduler_self_test(void);

/* Alloziert eine Seite und baut darin einen synthetischen Interrupt-Frame
   (SCHEDULER_FRAME_SIZE Bytes, endend am Seitenende) fuer entry_point auf.
   0 = Fehler, sonst Zeiger auf den Frame-Anfang. */
uint32_t scheduler_create_frame(uint32_t entry_point);

#define WS_SLOT_COUNT      SCHEDULER_THREAD_COUNT
#define WS_QUEUE_DEPTH     4u
#define WS_TASK_FLAG_RT       0x01u
#define WS_TASK_FLAG_HARD_AFF 0x02u

typedef struct {
    uint32_t entry_point;
    uint32_t pid;
    uint32_t cpu_mask;
    uint32_t flags;
} ws_task_entry_t;

void worksteal_initialize(void);
int worksteal_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Slot ungueltig oder Queue voll. */
int worksteal_enqueue(uint32_t slot, const ws_task_entry_t *task);
/* 0 = gestohlen (*out_task gefuellt), -1 wenn kein stehlfaehiger Task
   (RT-Bit gesetzt, inkompatible harte Affinitaet, oder Victim-Queue leer). */
int worksteal_try_steal(uint32_t victim_slot, uint32_t thief_slot, ws_task_entry_t *out_task);

#define RT_CAPACITY    8u
#define RT_POLICY_EDF  1u
#define RT_POLICY_FIFO 2u
#define RT_POLICY_RR   3u

void rt_scheduler_initialize(void);
int rt_scheduler_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn Tabelle voll. */
int rt_task_register(uint32_t owner, uint32_t deadline_ticks, uint32_t priority,
                      uint32_t policy, uint32_t *out_slot);
/* 0 = Erfolg (*out_slot gesetzt, EDF: fruehste Deadline; FIFO: erster
   READY-Slot), -1 wenn keine Task bereit. */
int rt_select_next(uint32_t *out_slot);
/* 0 = Deadline eingehalten, -1 = verpasst (erhoeht den Miss-Counter). */
int rt_check_deadline(uint32_t slot, uint32_t current_tick);

#define CFS_CAPACITY 8u

void cfs_initialize(void);
int cfs_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn Tabelle voll. */
int cfs_task_enqueue(uint32_t owner, uint32_t weight, uint32_t *out_slot);
/* 0 = Erfolg (*out_slot = Slot mit kleinster vruntime), -1 wenn keiner runnable. */
int cfs_pick_next(uint32_t *out_slot);
void cfs_update_vruntime(uint32_t slot, uint32_t elapsed_ticks);

#define QOS_CLASS_COUNT  4u
#define QOS_CLASS_RT     0u
#define QOS_CLASS_HIGH   1u
#define QOS_CLASS_NORMAL 2u
#define QOS_CLASS_IDLE   3u

void qos_initialize(void);
int qos_self_test(void); /* 0 = ok */

/* 0 = Budget vorhanden (dekrementiert), -1 = ausgeschoepft (throttled). */
int qos_consume(uint32_t qos_class);
void qos_new_epoch(void); /* setzt alle Budgets auf ihr Max zurueck */
/* 0 = Erfolg (Budget und Max gesetzt), -1 wenn class ungueltig. */
int qos_query(uint32_t qos_class, uint32_t *out_budget, uint32_t *out_max);

/* Pipeline 3 (Port aus integration32.inc, NPSPEC-INTEGRATION-0001):
   Scheduler Task Admission. priority>=200 -> RT (EDF, Deadline =
   aktuelle Ticks + 500); sonst QoS-Budget pruefen -> CFS-Enqueue mit
   invertiertem Gewicht (256-priority, min. 1). 0 = Erfolg (*out_slot
   gesetzt), -1 wenn kein Platz/Budget. */
int scheduler_admit_task(uint32_t owner, uint32_t priority, uint32_t qos_class, uint32_t *out_slot);

#endif
