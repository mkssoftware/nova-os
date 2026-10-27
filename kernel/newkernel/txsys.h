/* NovaOS C-Kernel - Nova Transaction System Coordinator (Port aus
   adaptive32.inc, NPSPEC-TRANSACTION-SYSTEM-0001). Two-Phase-Commit
   (2PC) Koordinator fuer systemweite Transaktionen. Verbindet
   konzeptionell tbarrier.h (Phase-1 Ready-Sync), txlog.h (WAL) und
   tcomp.h (Abort-Undo) - die Verdrahtung selbst ist NICHT portiert. */
#ifndef NOVA_TXSYS_H
#define NOVA_TXSYS_H

#include <stdint.h>

#define TXSYS_CAPACITY 4u

#define TXSYS_IDLE      0u
#define TXSYS_PREPARE   1u
#define TXSYS_COMMITTED 2u
#define TXSYS_ABORTED   3u

void txsys_initialize(void);
int txsys_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt, Status PREPARE), -1 wenn sys_txn_id==0,
   participant_count==0 oder kein IDLE-Slot frei. */
int txsys_begin(uint32_t sys_txn_id, uint32_t participant_count, uint32_t *out_slot);

/* Erhoeht den Ready-Zaehler; wenn alle Teilnehmer bereit sind, wechselt
   der Status auf COMMITTED. 0 = Erfolg, -1 wenn sys_txn_id unbekannt oder
   nicht im Status PREPARE. */
int txsys_participant_ready(uint32_t sys_txn_id);

/* 0 = Erfolg (Status ABORTED), -1 wenn sys_txn_id unbekannt. */
int txsys_abort(uint32_t sys_txn_id);

/* 0 = Erfolg (out_state gesetzt), -1 wenn sys_txn_id unbekannt. */
int txsys_query_state(uint32_t sys_txn_id, uint32_t *out_state);

#endif
