/* NovaOS C-Kernel - Nova Resource Economy Manager (Port aus
   adaptive32.inc, NPSPEC-RESOURCE-ECONOMY-0001). Credit-basierte
   Ressourcen-Oekonomie: jeder Owner erhaelt ein Budget, Operationen
   verbrauchen Credits. `reconomy_spend` ruft bei Erfolg `raccount_charge`
   auf (echte Cross-Modul-Verdrahtung mit raccount.c, 1:1 wie im alten
   ASM) - mem_delta wird dabei immer als 0 uebergeben. */
#ifndef NOVA_RECONOMY_H
#define NOVA_RECONOMY_H

#include <stdint.h>

#define RECON_CAPACITY 8u

void reconomy_initialize(void);
int reconomy_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn owner==0 oder Tabelle voll. */
int reconomy_register(uint32_t owner, uint32_t initial_budget);

/* 0 = Erfolg (Budget verringert, Spent erhoeht, raccount_charge
   aufgerufen), -1 = Budget erschoepft oder owner unbekannt. */
int reconomy_spend(uint32_t owner, uint32_t cost);

/* 0 = Erfolg (Budget erhoeht, Refills-Zaehler erhoeht), -1 wenn owner
   unbekannt. */
int reconomy_refill(uint32_t owner, uint32_t refill_amount);

#endif
