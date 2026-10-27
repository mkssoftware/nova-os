/* NovaOS C-Kernel - Nova Task Deadline Manager (Port aus
   concurrency32.inc, NPSPEC-CONCURRENCY-DEADLINE-0001 /
   ADR-CONCURRENCY-0004). Hierarchische Task-Deadlines auf monotoner
   Tick-Zeitbasis (`timer_get_ticks`, interrupt.h). Eine Parent-Deadline
   wird niemals verlaengert; eine HARD-Klasse vererbt sich auf Kinder.
   `task_deadline_poll` ist fuer den Aufruf aus IRQ0 vorgesehen (wie im
   alten ASM), wird hier aber noch NICHT automatisch aus
   `interrupt_dispatch_c` heraus aufgerufen - das waere die naechste
   Ausbaustufe, sobald Timer-getriebene Deadline-Durchsetzung gebraucht
   wird. */
#ifndef NOVA_TASKDEADLINE_H
#define NOVA_TASKDEADLINE_H

#include <stdint.h>

#define TASK_DEADLINE_CLOCK_HZ 100u

#define TASK_DEADLINE_CLASS_HARD 1u
#define TASK_DEADLINE_CLASS_FIRM 2u
#define TASK_DEADLINE_CLASS_SOFT 3u

#define TASK_DEADLINE_POLICY_CONTINUE 1u
#define TASK_DEADLINE_POLICY_CANCEL   2u
#define TASK_DEADLINE_POLICY_FAIL     3u

#define TASK_DEADLINE_STATE_NONE   0u
#define TASK_DEADLINE_STATE_ARMED  1u
#define TASK_DEADLINE_STATE_MISSED 2u

#define TASK_CANCEL_REASON_DEADLINE 0x444C4E45u

typedef struct {
    uint32_t absolute_tick;
    uint32_t deadline_class;
    uint32_t policy;
    uint32_t state;
} task_deadline_info_t;

int task_deadline_manager_initialize(void);
int task_deadline_manager_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_effective_tick gesetzt - kann wegen Parent-Vererbung
   frueher als absolute_tick sein), -1 wenn Klasse/Policy ungueltig,
   tick==0, task_id unbekannt/nicht aktiv, oder eine effektive HARD-
   Deadline mit Policy CONTINUE oder bereits abgelaufenem Tick
   vorliegt. */
int task_deadline_set(uint32_t task_id, uint32_t absolute_tick, uint32_t deadline_class,
                       uint32_t miss_policy, uint32_t *out_effective_tick);

/* Scannt alle aktiven Tasks; markiert abgelaufene ARMED-Deadlines als
   MISSED und wendet die Miss-Policy an (CANCEL -> task_request_cancel,
   FAIL -> task_fail). Aufzurufen aus IRQ0 oder Tests. */
void task_deadline_poll(void);

/* 0 = Erfolg (*out_info gesetzt), -1 wenn task_id unbekannt. */
int task_deadline_query(uint32_t task_id, task_deadline_info_t *out_info);

#endif
