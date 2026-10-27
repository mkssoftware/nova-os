/* NovaOS C-Kernel - Nova Tx MultiService Coordinator (Port aus
   adaptive32.inc, NPSPEC-TRANSACTION-MULTISERVICE-0001). Erweitert txsys
   um pro-Service-Stimmen in einer Cross-Service-Transaktion: jeder Service
   registriert eine Stimme (COMMIT/ABORT), der Koordinator entscheidet
   global. Ein COMMIT-Vote leitet an `txsys_participant_ready` weiter
   (echte Cross-Modul-Verdrahtung mit txsys.c, 1:1 wie im alten ASM). */
#ifndef NOVA_TXMULTI_H
#define NOVA_TXMULTI_H

#include <stdint.h>

#define TXMULTI_CAPACITY 4u

#define TXMULTI_PENDING 0u
#define TXMULTI_COMMIT  1u
#define TXMULTI_ABORT   2u

void txmulti_initialize(void);
int txmulti_self_test(void); /* 0 = ok */

/* 0 = Erfolg (Stimme PENDING), -1 wenn ms_txn_id==0 oder Tabelle voll. */
int txmulti_register_service(uint32_t ms_txn_id, uint32_t service_id);

/* 0 = Erfolg (Stimme gesetzt; bei COMMIT wird txsys_participant_ready
   aufgerufen), -1 wenn ms_txn_id/service_id unbekannt. */
int txmulti_vote(uint32_t ms_txn_id, uint32_t service_id, uint32_t vote);

/* 0 = alle Stimmen COMMIT, -1 wenn mind. eine ABORT/PENDING ist oder
   ms_txn_id unbekannt. */
int txmulti_check(uint32_t ms_txn_id);

#endif
