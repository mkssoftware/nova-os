/* NovaOS C-Kernel - Nova Transaction Config Manager (Port aus
   adaptive32.inc, NPSPEC-TRANSACTION-CONFIG-0001). Atomare Konfigurations-
   transaktionen: staged eine Menge von sconf-Aenderungen, committet oder
   verwirft sie als Einheit, um partielle Konfigurations-Updates zu
   verhindern. Nutzt `sconf_get`/`sconf_set` (echte Cross-Modul-
   Verdrahtung mit sconf.c, 1:1 wie im alten ASM). */
#ifndef NOVA_TXCONFIG_H
#define NOVA_TXCONFIG_H

#include <stdint.h>

#define TXCFG_CAPACITY 8u

void txconfig_initialize(void);
int txconfig_self_test(void); /* 0 = ok */

/* 0 = Erfolg (alter Wert via sconf_get gelesen, 0 wenn Key noch unbekannt),
   -1 wenn cfg_txn_id==0 oder Tabelle voll. */
int txconfig_stage(uint32_t cfg_txn_id, uint32_t key, uint32_t new_value);

/* Wendet alle gestageten Aenderungen via sconf_set an und leert die Slots.
   0 = immer Erfolg. */
int txconfig_commit(uint32_t cfg_txn_id);

/* Stellt die alten Werte via sconf_set wieder her und leert die Slots.
   0 = immer Erfolg. */
int txconfig_abort(uint32_t cfg_txn_id);

#endif
