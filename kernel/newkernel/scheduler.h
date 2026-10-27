/* NovaOS C-Kernel - Scheduler-Familie (Port aus scheduler32.inc,
   NPSPEC-SCHEDULER-FAIR-0001 / NPSPEC-SCHEDULER-REALTIME-0001 /
   NPSPEC-SCHEDULER-QOS-0001 / NPSPEC-CONCURRENCY-WORKSTEALING-0001).

   WICHTIG (2026-10-27): `scheduler_on_tick` ist jetzt ein echter,
   vereinfachter (Single-CPU, kein SMP/AP) Port des Round-Robin-
   Taskwechsels und wird von `interrupt_dispatch_c` bei jedem Timer-Tick
   aufgerufen (siehe interrupt.c). Er bleibt aber bis zum expliziten
   Aufruf von `scheduler_enable()` inaktiv (No-Op) - erst DANACH kann
   jeder weitere Tick die aktuell laufende Ausfuehrung permanent zugunsten
   von scheduler_thread1/2 verdraengen (genau wie `ring3_enter`: eine
   EINWEG-Uebergabe an einen fortlaufenden Betriebszustand, kein
   normaler Funktionsaufruf, der zur sequentiellen Boot-Logik
   zurueckkehrt). Deshalb wird `scheduler_enable()` erst als allerletzte
   Aktion aufgerufen (aus `syscall.c`s `syscall_userspace_exit`, NACH dem
   Ring-3-Rundweg - siehe dort). `scheduler_initialize`/`create_frame`
   bauen weiterhin nur die Datenstrukturen/Frames auf, OHNE den
   Taskwechsel scharf zu schalten. Fehlend gegenueber dem alten ASM:
   SMP/AP-Bringup (per_cpu_current_thread wird durch eine einzelne
   Variable ersetzt, da nur 1 CPU existiert; scheduler_lock/
   scheduler_caller_cpu/scheduler_bsp_boot_lock entfallen ganz - ohne
   SMP gibt es keine Nebenlaeufigkeit in scheduler_on_tick, das IDT-
   Interrupt-Gate deaktiviert IF schon automatisch) und die
   Thread-Manager-Synchronisation (`thread_manager_ready`-Zweige, da der
   Thread Manager aus sync32.inc weiterhin nicht portiert ist - siehe
   project_c_kernel_migration_status). */
#ifndef NOVA_SCHEDULER_H
#define NOVA_SCHEDULER_H

#include <stdint.h>

#define SCHEDULER_THREAD_COUNT 8u
#define SCHEDULER_FRAME_SIZE   68u

/* Erzeugt zwei synthetische Thread-Kontexte (scheduler_thread1/2). */
void scheduler_initialize(void);
/* Prueft Initialisierung und dass beide Thread-Frames gueltig aufgebaut
   wurden (EIP/CS/EFLAGS) - NICHT dass die Threads real liefen (echter
   Taskwechsel ist zu diesem Zeitpunkt noch nicht aktiviert, siehe
   Hinweis oben). 0 = ok. */
int scheduler_self_test(void);

/* Alloziert eine Seite und baut darin einen synthetischen Interrupt-Frame
   (SCHEDULER_FRAME_SIZE Bytes, endend am Seitenende) fuer entry_point auf.
   0 = Fehler, sonst Zeiger auf den Frame-Anfang. */
uint32_t scheduler_create_frame(uint32_t entry_point);

/* EINWEG: schaltet den echten Taskwechsel scharf (siehe Hinweis oben). */
void scheduler_enable(void);

/* Von interrupt_dispatch_c bei jedem Timer-Tick (Vektor 32) aufgerufen.
   frame_ptr ist der unterbrochene Trap-Frame; der Rueckgabewert wird an
   iretd weitergereicht (Round-Robin ueber alle Slots mit gueltigem
   Kontext, Slot 0 bleibt dauerhaft reserviert). No-Op (gibt frame_ptr
   unveraendert zurueck), solange scheduler_enable() noch nicht
   aufgerufen wurde. */
uint32_t scheduler_on_tick(uint32_t frame_ptr);

/* Thread-Koerper (scheduler_thread1/2 fuer threadmgr.c's statische
   Registrierung, scheduler_dynamic_thread als Einstiegspunkt fuer per
   threadmgr.c's thread_create_dynamic erzeugte Threads). Nicht direkt
   aufzurufen - nur ihre Adresse wird gebraucht (scheduler_create_frame/
   thread_register). */
void scheduler_thread1(void);
void scheduler_thread2(void);
void scheduler_dynamic_thread(void);

/* Fuer threadmgr.c (NPSPEC-KERNEL-0016-Thread-Manager, sync32.inc):
   kontrollierter Zugriff auf scheduler.c's sonst private Slot-Tabellen,
   ohne sie direkt offenzulegen. */

/* 0 = Erfolg (*out_slot gesetzt, Kontext gebaut, Affinitaet gesetzt,
   als frei-aber-belegt markiert), -1 wenn kein freier Slot (1..COUNT-1)
   mehr frei ist oder scheduler_create_frame fehlschlaegt. Slot 0 bleibt
   dauerhaft reserviert (siehe scheduler_initialize) und wird nie
   vergeben. */
int scheduler_claim_dynamic_slot(uint32_t entry_point, int32_t cpu_affinity, uint32_t *out_slot);
/* Gibt einen per scheduler_claim_dynamic_slot belegten Slot wieder frei
   (Fehlerpfad in thread_create_dynamic). */
void scheduler_release_dynamic_slot(uint32_t slot);
/* Liest scheduler_contexts[slot]/thread_cpu_affinity[slot]/
   thread_running_on[slot], oder 0/-1 wenn slot ausserhalb des gueltigen
   Bereichs liegt. */
uint32_t scheduler_context_at(uint32_t slot);
int32_t scheduler_affinity_at(uint32_t slot);
int32_t scheduler_running_on_at(uint32_t slot);

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
