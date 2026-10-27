/* NovaOS C-Kernel - Nova Transaction Resource Manager (Port aus
   adaptive32.inc, NPSPEC-TRANSACTION-RESOURCE-0001). Verfolgt Resource-
   Locks aktiver Transaktionen: vor Aenderung einer gemeinsam genutzten
   Ressource erwirbt eine Transaktion hier einen Lock, bei Commit/Abort
   werden alle Locks einer Transaktion freigegeben. */
#ifndef NOVA_TXRES_H
#define NOVA_TXRES_H

#include <stdint.h>

#define TXRES_CAPACITY 8u

void txres_initialize(void);
int txres_self_test(void); /* 0 = ok */

/* 0 = erworben, -1 = resource_id bereits von einer anderen Transaktion
   gehalten, Tabelle voll, oder txn_id==0. */
int txres_acquire(uint32_t txn_id, uint32_t resource_id);

/* Gibt alle von txn_id gehaltenen Locks frei. 0 = Erfolg, -1 wenn
   txn_id==0. */
int txres_release_all(uint32_t txn_id);

#endif
