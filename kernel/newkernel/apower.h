/* NovaOS C-Kernel - Nova Adaptive Power Manager (Port aus adaptive32.inc,
   NPSPEC-ADAPTIVE-POWER-0001). Verwaltet CPU-Power-Domains mit
   last-getriebenen P-State-Uebergaengen (0=max-perf ... 3=min-power).
   Ruft bei jeder Load-Aktualisierung `apred_update` (domain_id als
   metric_id) - echte Cross-Modul-Verdrahtung, Rueckgabewert wird wie im
   alten ASM ignoriert (die Metrik muss noch nicht registriert sein). */
#ifndef NOVA_APOWER_H
#define NOVA_APOWER_H

#include <stdint.h>

#define APOWER_CAPACITY 4u

#define APOWER_PSTATE_MAX 0u
#define APOWER_PSTATE_MIN 3u

void apower_initialize(void);
int apower_self_test(void); /* 0 = ok */

/* 0 = Erfolg (P-State=1/balanced, Load=50 als Startwert), -1 wenn
   domain_id==0 oder Tabelle voll. */
int apower_register(uint32_t domain_id);

/* load (0-100): >=75 -> P-State MAX (0), <=25 -> P-State MIN (3), sonst
   P-State 1 (balanced). 0 = Erfolg (Transitions-Zaehler bei Wechsel
   erhoeht), -1 wenn domain_id unbekannt. */
int apower_update_load(uint32_t domain_id, uint32_t load);

/* 0 = Erfolg (out_pstate/out_load/out_transitions gesetzt), -1 wenn
   domain_id unbekannt. */
int apower_query(uint32_t domain_id, uint32_t *out_pstate, uint32_t *out_load,
                 uint32_t *out_transitions);

#endif
