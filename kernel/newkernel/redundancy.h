/* NovaOS C-Kernel - Nova Redundancy Manager (Port aus adaptive32.inc,
   NPSPEC-RESILIENCE-REDUNDANCY-0001). Verfolgt aktive und Standby-Replicas
   fuer Redundanz-Gruppen; bei Ausfall des Active wird der naechste Standby
   befoerdert. */
#ifndef NOVA_REDUNDANCY_H
#define NOVA_REDUNDANCY_H

#include <stdint.h>

#define REDUN_CAPACITY 4u

void redundancy_initialize(void);
int redundancy_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn group_id==0 oder Tabelle voll. */
int redundancy_register(uint32_t group_id, uint32_t active_id, uint32_t standby_id, uint32_t *out_slot);

/* 0 = befoerdert (*out_new_active gesetzt, Active<->Standby getauscht),
   -1 wenn group_id unbekannt/ungueltig oder kein Standby vorhanden. */
int redundancy_promote(uint32_t group_id, uint32_t *out_new_active);

#endif
