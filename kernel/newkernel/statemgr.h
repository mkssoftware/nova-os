/* NovaOS C-Kernel - State-Manager (Port aus statemgr32.inc, NPSPEC-STATE-*). */
#ifndef NOVA_STATEMGR_H
#define NOVA_STATEMGR_H

#include <stdint.h>

#define SM_STATE_UNKNOWN      0u
#define SM_STATE_INITIALIZING 1u
#define SM_STATE_RUNNING      2u
#define SM_STATE_DEGRADED     3u
#define SM_STATE_FAILED       4u
#define SM_STATE_SHUTDOWN     5u

#define SM_RECONCILE_OK       0u
#define SM_RECONCILE_DRIFT    1u
#define SM_RECONCILE_CONFLICT 2u

#define SM_FLAG_DIRTY         0x01u
#define SM_FLAG_LOCKED        0x02u
#define SM_FLAG_SNAPSHOTTABLE 0x04u
#define SM_FLAG_ROLLBACK      0x08u

#define SM_CAPACITY 16u

void statemgr_initialize(void);
int statemgr_self_test(void); /* 0 = ok */

/* 0 = Erfolg, *out_slot erhaelt den Slot; -1 wenn Tabelle voll. */
int state_register(uint32_t obj_id, uint32_t initial_state, uint32_t *out_slot);

/* 0 = Erfolg, -1 wenn obj_id unbekannt. */
int state_set_desired(uint32_t obj_id, uint32_t desired_state);

/* *out_result erhaelt SM_RECONCILE_*. 0 = Erfolg, -1 wenn obj_id unbekannt
   (dann SM_RECONCILE_CONFLICT). */
int state_reconcile(uint32_t obj_id, uint32_t *out_result);

/* *out_snapshot_id erhaelt die neue Snapshot-ID. 0 = Erfolg, -1 wenn
   obj_id unbekannt. */
int state_snapshot(uint32_t obj_id, uint32_t *out_snapshot_id);

#endif
