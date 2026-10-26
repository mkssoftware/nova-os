/* NovaOS C-Kernel - Nova Transaction Barrier (Port aus adaptive32.inc,
   NPSPEC-TRANSACTION-BARRIER-0001). All-ready-Synchronisationsgatter:
   wartet auf N Teilnehmer-Signale, bevor es sich oeffnet. */
#ifndef NOVA_TBARRIER_H
#define NOVA_TBARRIER_H

#include <stdint.h>

#define TBARRIER_CAPACITY 4u

void tbarrier_initialize(void);
int tbarrier_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn barrier_id==0, total==0 oder
   Tabelle voll. */
int tbarrier_create(uint32_t barrier_id, uint32_t total_participants, uint32_t *out_slot);

/* 0 = Barrier jetzt offen (alle angekommen), -1 = weiterhin geschlossen
   oder barrier_id unbekannt/ungueltig. */
int tbarrier_signal(uint32_t barrier_id);

#endif
