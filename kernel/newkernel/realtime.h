/* NovaOS C-Kernel - Realtime-Subsystem (Port aus realtime32.inc, NPSPEC-REALTIME-*). */
#ifndef NOVA_REALTIME_H
#define NOVA_REALTIME_H

#include <stdint.h>

#define RT_PRIO_MAX         99u
#define RT_PRIO_HIGH        80u
#define RT_PRIO_NORMAL      50u
#define RT_PRIO_LOW         20u
#define RT_PRIO_BEST_EFFORT 0u

#define RT_SCHED_FIFO     0u
#define RT_SCHED_RR       1u
#define RT_SCHED_DEADLINE 2u
#define RT_SCHED_IDLE     3u

#define RT_LATENCY_HARD     0u
#define RT_LATENCY_SOFT     1u
#define RT_LATENCY_BEST_EFF 2u

#define RT_FLAG_ISOLATED      0x01u
#define RT_FLAG_DETERMINISTIC 0x02u
#define RT_FLAG_ACTIVE        0x04u

#define RT_TASK_CAPACITY 8u

/* Registriert zusaetzlich den Kernel-Idle-Task (ID 0) als RT-Referenz. */
int realtime_initialize(void);
int realtime_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int rtmgr_task_register(uint32_t task_id, uint32_t priority, uint32_t sched_class,
                         uint32_t latency_class);

/* 0 = Erfolg, -1 wenn task_id unbekannt. */
int rt_set_params(uint32_t task_id, uint32_t period_us, uint32_t deadline_us, uint32_t wcet_us);

/* Aktualisiert Max-Jitter pro Task und global (immer 0 = Erfolg). */
int rt_record_jitter(uint32_t task_id, uint32_t jitter_us);

/* 0 = innerhalb Deadline, -1 = Deadline verpasst UND Latenzklasse ist HARD
   (bei SOFT/BEST_EFFORT wird ein Miss nur gezaehlt, nicht als Fehler
   gemeldet), oder task_id unbekannt. */
int rt_deadline_check(uint32_t task_id);

#endif
