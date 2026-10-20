/* NovaOS C-Kernel - Out-of-Memory-Handler (Port aus memory32.inc,
   NPSPEC-MEMORY-OOM-0001). Score-basierte Kandidatenauswahl + Terminierung. */
#ifndef NOVA_OOM_H
#define NOVA_OOM_H

#include <stdint.h>

#define OOM_CAND_COUNT 8

void oom_initialize(void);
int oom_self_test(void); /* 0 = ok */

/* Gibt den Slot (>=0) zurueck, oder -1 wenn alle Slots belegt sind. */
int oom_register_candidate(uint32_t pid, uint32_t score);

/* Terminiert den Kandidaten mit dem hoechsten Score und gibt dessen PID
   zurueck, oder -1 wenn kein aktiver Kandidat existiert. */
int32_t oom_kill_victim(void);

#endif
