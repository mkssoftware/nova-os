/* NovaOS C-Kernel - Nova Resource Reclaim (Port aus adaptive32.inc,
   NPSPEC-RESOURCE-RECLAIM-0001). Verfolgt freigegebene Ressourcen und
   macht sie fuer die Wiederzuteilung verfuegbar. Verbindet sich
   konzeptionell mit radmit.h (`radmit_release` soll hier posten) - die
   Verdrahtung selbst ist NICHT portiert. */
#ifndef NOVA_RRECLAIM_H
#define NOVA_RRECLAIM_H

#include <stdint.h>

#define RRECLAIM_CAPACITY 8u

void rreclaim_initialize(void);
int rreclaim_self_test(void); /* 0 = ok */

/* 0 = Erfolg (cpu_units/mem_units zum Freigabe-Pool des Owners addiert),
   -1 wenn owner_id==0 oder Tabelle voll. */
int rreclaim_post(uint32_t owner_id, uint32_t cpu_units, uint32_t mem_units);

/* 0 = genug freigegebene Ressourcen vorhanden (vom Pool abgezogen,
   reclaimed-Zaehler erhoeht), -1 = nicht genug vorhanden oder owner_id
   unbekannt. */
int rreclaim_try(uint32_t owner_id, uint32_t need_cpu, uint32_t need_mem);

#endif
