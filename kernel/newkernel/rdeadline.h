/* NovaOS C-Kernel - Nova Resource Deadline Manager (Port aus
   adaptive32.inc, NPSPEC-RESOURCE-DEADLINE-0001). Earliest-Deadline-
   First (EDF) Durchsetzung: verfolgt Task-Deadlines und meldet
   ueberfaellige Tasks. Bei Ueberfaelligkeit: `aschedsw_feedback(4, 32)`
   (REALTIME-Klasse-Boost) - echte Cross-Modul-Verdrahtung, 1:1 wie im
   alten ASM. */
#ifndef NOVA_RDEADLINE_H
#define NOVA_RDEADLINE_H

#include <stdint.h>

#define RDEAD_CAPACITY 8u

#define RDEAD_ACTIVE 1u
#define RDEAD_DONE   2u
#define RDEAD_MISSED 3u

void rdeadline_initialize(void);
int rdeadline_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn task_id==0 oder Tabelle voll. */
int rdeadline_register(uint32_t task_id, uint32_t deadline_tick);

/* 0 = Erfolg (Status->DONE), -1 wenn task_id unbekannt. */
int rdeadline_done(uint32_t task_id);

/* Prueft alle ACTIVE Tasks gegen current_tick; ueberfaellige werden auf
   MISSED gesetzt (Missed-Zaehler erhoeht, `aschedsw_feedback` aufgerufen). */
void rdeadline_check_all(uint32_t current_tick);

#endif
