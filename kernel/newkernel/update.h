/* NovaOS C-Kernel - Update-Manager (Port aus update32.inc, NPSPEC-UPDATE-*). */
#ifndef NOVA_UPDATE_H
#define NOVA_UPDATE_H

#include <stdint.h>

#define UPD_TYPE_KERNEL     0u
#define UPD_TYPE_DRIVER     1u
#define UPD_TYPE_FIRMWARE   2u
#define UPD_TYPE_BOOTLOADER 3u
#define UPD_TYPE_PACKAGE    4u
#define UPD_TYPE_HOTPATCH   5u

#define UPD_STATE_PENDING     0u
#define UPD_STATE_STAGING     1u
#define UPD_STATE_CANARY      2u
#define UPD_STATE_ROLLING     3u
#define UPD_STATE_APPLIED     4u
#define UPD_STATE_FAILED      5u
#define UPD_STATE_ROLLED_BACK 6u

#define UPD_FLAG_ATOMIC    0x01u
#define UPD_FLAG_SIGNED    0x02u
#define UPD_FLAG_IMMUTABLE 0x04u
#define UPD_FLAG_LIVE      0x08u
#define UPD_FLAG_AB        0x10u
#define UPD_FLAG_DELTA     0x20u
#define UPD_FLAG_VERIFIED  0x40u
#define UPD_FLAG_HAS_VULN  0x80u

#define UPD_CAPACITY 8u

#define UPD_SLOT_A       0u
#define UPD_SLOT_B       1u
#define UPD_SLOT_UNKNOWN 0xFFu

void update_initialize(void);
int update_self_test(void); /* 0 = ok */

/* 0 = Erfolg, *out_upd_id erhaelt die neue ID; -1 wenn Tabelle voll. */
int upd_submit(uint32_t type, uint32_t target_id, uint32_t new_version, uint32_t flags,
               uint32_t *out_upd_id);

/* 0 = Erfolg (tauscht bei UPD_FLAG_AB den aktiven Slot), -1 wenn upd_id
   unbekannt oder UPD_FLAG_SIGNED ohne Signatur-Hash gesetzt ist. */
int upd_apply(uint32_t upd_id);

/* 0 = Erfolg (tauscht bei UPD_FLAG_AB den aktiven Slot zurueck), -1 wenn
   upd_id unbekannt. */
int upd_rollback(uint32_t upd_id);

#endif
