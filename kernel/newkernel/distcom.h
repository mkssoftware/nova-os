/* NovaOS C-Kernel - Distributed Communications (Port aus distcom32.inc,
   NPSPEC-DISTCOMM-*). */
#ifndef NOVA_DISTCOM_H
#define NOVA_DISTCOM_H

#include <stdint.h>

#define DC_MSG_REQUEST   0u
#define DC_MSG_RESPONSE  1u
#define DC_MSG_EVENT     2u
#define DC_MSG_HEARTBEAT 3u

#define DC_PROTO_LOCAL 0u
#define DC_PROTO_HTTP  1u
#define DC_PROTO_RPC   2u
#define DC_PROTO_TLS   3u

#define DC_RETRY_NONE        0u
#define DC_RETRY_FIXED       1u
#define DC_RETRY_EXPONENTIAL 2u
#define DC_RETRY_JITTER      3u

#define DC_BP_OK      0u
#define DC_BP_SLOW    1u
#define DC_BP_OVERLOAD 2u
#define DC_BP_DROP    3u

#define DC_MSG_CAPACITY   8u
#define DC_TOPIC_CAPACITY 8u

void distcom_initialize(void);
int distcom_self_test(void); /* 0 = ok */

/* 0 = Erfolg, *out_msg_id erhaelt die neue ID; -1 wenn Tabelle voll. */
int dc_send(uint32_t type, uint32_t proto, uint32_t deadline_ticks, uint32_t *out_msg_id);

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int dc_topic_register(uint32_t topic_id);

/* 0 = Erfolg, -1 wenn topic_id unbekannt. */
int dc_subscribe(uint32_t topic_id, uint32_t subscriber_bit);

/* 0 = Erfolg (*out_bp_state erhaelt DC_BP_*), -1 wenn topic_id unbekannt. */
int dc_backpressure_check(uint32_t topic_id, uint32_t *out_bp_state);

#endif
