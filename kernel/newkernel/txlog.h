/* NovaOS C-Kernel - Nova Transaction Log (WAL-style) (Port aus
   adaptive32.inc, NPSPEC-TRANSACTION-LOG-0001). Write-ahead log: zeichnet
   Transaktions-Operationen auf, bevor sie angewendet werden. Append-only
   Operation-Ring. NICHT zu verwechseln mit txroll.c (Rollback-Journal fuer
   Dword-Save/Restore) - ein eigenstaendiges, schon im alten ASM
   paralleles Subsystem. */
#ifndef NOVA_TXLOG_H
#define NOVA_TXLOG_H

#include <stdint.h>

#define TXLOG_CAPACITY 8u

#define TXLOG_WRITE  1u
#define TXLOG_DELETE 2u
#define TXLOG_COMMIT 3u
#define TXLOG_ABORT  4u

void txlog_initialize(void);
int txlog_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn txn_id==0 oder Log nicht bereit. */
int txlog_append(uint32_t txn_id, uint32_t op_type, uint32_t target, uint32_t value);
uint32_t txlog_count_entries(void);

#endif
