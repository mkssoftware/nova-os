/* NovaOS C-Kernel - System-Transaction-Manager (Port aus txmgr32.inc,
   NPSPEC-TRANSACTION-*). */
#ifndef NOVA_TXMGR_H
#define NOVA_TXMGR_H

#include <stdint.h>

#define TX_TYPE_LOCAL       0u
#define TX_TYPE_DISTRIBUTED 1u
#define TX_TYPE_CONFIG      2u
#define TX_TYPE_RESOURCE    3u

#define TX_STATE_ACTIVE      0u
#define TX_STATE_PREPARED    1u
#define TX_STATE_COMMITTED   2u
#define TX_STATE_ROLLED_BACK 3u
#define TX_STATE_FAILED      4u

#define TX_FLAG_ATOMIC     0x01u
#define TX_FLAG_BARRIER    0x02u
#define TX_FLAG_COMPENSATE 0x04u
#define TX_FLAG_LOGGED     0x08u
#define TX_FLAG_MULTI_SVC  0x10u

#define SYSMGR_TX_CAPACITY 8u

typedef void (*txmgr_compensation_fn_t)(uint32_t tx_id);

void txmgr_initialize(void);
int txmgr_self_test(void); /* 0 = ok */

/* 0 = Erfolg, *out_tx_id erhaelt die ID; -1 wenn Tabelle voll. */
int txmgr_begin(uint32_t type, uint32_t flags, uint32_t timeout_ticks, uint32_t *out_tx_id);

/* 0 = Erfolg, -1 wenn tx_id unbekannt, nicht ACTIVE oder Timeout ueberschritten. */
int txmgr_commit(uint32_t tx_id);

/* 0 = Erfolg (fuehrt die Kompensationsfunktion aus, falls registriert),
   -1 wenn tx_id unbekannt. */
int txmgr_rollback(uint32_t tx_id);

#endif
