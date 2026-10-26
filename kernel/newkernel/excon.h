/* NovaOS C-Kernel - Nova Execution Contract (Port aus adaptive32.inc,
   NPSPEC-EXECUTION-CONTRACT-0001). Deklarative Beschreibung was ausgefuehrt
   wird und unter welchen Bedingungen. excon_activate verbindet sich mit
   scheduler_admit_task (Pipeline 3, scheduler.h). NICHT zu verwechseln mit
   program.c's cap_register/cap_activate - ein eigenstaendiges, schon im
   alten ASM paralleles Subsystem. */
#ifndef NOVA_EXCON_H
#define NOVA_EXCON_H

#include <stdint.h>

#define EXCON_CAPACITY 8u

#define EXCON_TYPE_BE      1u
#define EXCON_TYPE_BOUNDED 2u
#define EXCON_TYPE_HARD_RT 3u

#define EXCON_STATE_CREATED 1u
#define EXCON_STATE_ACTIVE  2u
#define EXCON_STATE_DONE    3u
#define EXCON_STATE_VIOL    4u

void excon_initialize(void);
int excon_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn type ungueltig/0 oder Tabelle voll.
   Deadline wird intern als aktuelle Ticks + 1000 gesetzt. */
int excon_create(uint32_t type, uint32_t cpu_budget, uint32_t priority, uint32_t *out_slot);

/* Aktiviert den Contract und meldet ihn beim Scheduler an (owner=Contract-Id,
   priority=gespeicherte Prioritaet, qos=QOS_CLASS_NORMAL). 0 = Erfolg,
   -1 wenn slot ungueltig/frei oder scheduler_admit_task ablehnt. */
int excon_activate(uint32_t slot);

/* Prueft Einhaltung: actual_cpu_used <= cpu_budget. 0 = eingehalten (State
   DONE), -1 = verletzt (State VIOLATED, Violation-Counter erhoeht) oder
   slot ungueltig/frei. */
int excon_verify(uint32_t slot, uint32_t actual_cpu_used);

#endif
