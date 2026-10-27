/* NovaOS C-Kernel - Nova Priority Inheritance Mutex (Port aus sync32.inc,
   NPSPEC-SYNC-PRIORITYINHERITANCE-0001). Verhindert Priority Inversion:
   der Lock-Besitzer erbt die hoechste Prioritaet aller Wartenden bis zur
   Freigabe. */
#ifndef NOVA_PIMUTEX_H
#define NOVA_PIMUTEX_H

#include <stdint.h>

#define PI_CAPACITY 8u

void pi_initialize(void);
int pi_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn Tabelle voll. */
int pi_mutex_create(uint32_t *out_slot);

/* 0 = Erfolg (Mutex gehoert jetzt task_id), -1 = bereits gesperrt (die
   Priority-Inheritance-Anhebung des Besitzers greift trotzdem, wenn
   task_base_prio hoeher ist als die aktuelle effektive Prioritaet) oder
   slot ungueltig. */
int pi_mutex_lock(uint32_t slot, uint32_t task_id, uint32_t task_base_prio);

/* 0 = Erfolg (effektive Prioritaet auf Basis zurueckgesetzt), -1 wenn
   slot ungueltig oder nicht gesperrt. */
int pi_mutex_unlock(uint32_t slot);

/* 0 = Erfolg, -1 wenn slot ungueltig/frei. */
int pi_mutex_query(uint32_t slot, uint32_t *out_eff_prio, uint32_t *out_owner, uint32_t *out_waiters);

#endif
