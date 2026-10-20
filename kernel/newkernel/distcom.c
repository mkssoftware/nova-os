/* NovaOS C-Kernel - Distributed Communications (Port aus distcom32.inc). */
#include "distcom.h"

typedef struct {
    uint32_t id;
    uint32_t type;
    uint32_t proto;
    uint32_t flags;
    uint32_t idempotency_key;
    uint32_t deadline;
    uint32_t retry_count;
    uint32_t max_retries;
    uint32_t trace_id;
    uint32_t schema_version;
    uint32_t payload_len;
    uint32_t payload_ptr;
} dc_message_t;

typedef struct {
    uint32_t topic_id;
    uint32_t flags;
    uint32_t subscriber_mask;
    uint32_t msg_count;
    uint32_t backpressure;
    uint32_t last_trace;
} dc_topic_t;

static dc_message_t dc_msg_table[DC_MSG_CAPACITY];
static dc_topic_t dc_topic_table[DC_TOPIC_CAPACITY];
static uint32_t dc_msg_count = 0;
static uint32_t dc_topic_count = 0;
static uint32_t dc_next_msg_id = 1;
static uint32_t dc_next_trace_id = 1;
static int dc_ready = 0;

/* TODO: durch timer_ticks ersetzen, sobald das Time-Subsystem portiert ist. */
static uint32_t current_timer_ticks(void) {
    return 0;
}

void distcom_initialize(void) {
    for (uint32_t i = 0; i < DC_MSG_CAPACITY; ++i) {
        dc_msg_table[i] = (dc_message_t){0};
    }
    for (uint32_t i = 0; i < DC_TOPIC_CAPACITY; ++i) {
        dc_topic_table[i] = (dc_topic_t){0};
    }
    dc_msg_count = 0;
    dc_topic_count = 0;
    dc_next_msg_id = 1;
    dc_next_trace_id = 1;
    dc_ready = 1;
}

int dc_send(uint32_t type, uint32_t proto, uint32_t deadline_ticks, uint32_t *out_msg_id) {
    if (dc_msg_count >= DC_MSG_CAPACITY) {
        return -1;
    }
    dc_message_t *msg = &dc_msg_table[dc_msg_count];
    msg->id = dc_next_msg_id;
    msg->type = type;
    msg->proto = proto;
    msg->flags = 0;
    msg->idempotency_key = current_timer_ticks() ^ dc_next_msg_id; /* msg_id XOR timer_ticks */
    msg->deadline = deadline_ticks;
    msg->retry_count = 0;
    msg->max_retries = 3;
    msg->trace_id = dc_next_trace_id;
    ++dc_next_trace_id;
    msg->schema_version = 1;
    msg->payload_len = 0;
    msg->payload_ptr = 0;

    if (out_msg_id) {
        *out_msg_id = dc_next_msg_id;
    }
    ++dc_next_msg_id;
    ++dc_msg_count;
    return 0;
}

int dc_topic_register(uint32_t topic_id) {
    if (dc_topic_count >= DC_TOPIC_CAPACITY) {
        return -1;
    }
    dc_topic_t *topic = &dc_topic_table[dc_topic_count];
    topic->topic_id = topic_id;
    topic->flags = 0;
    topic->subscriber_mask = 0;
    topic->msg_count = 0;
    topic->backpressure = DC_BP_OK;
    ++dc_topic_count;
    return 0;
}

static dc_topic_t *distcom_find_topic(uint32_t topic_id) {
    for (uint32_t i = 0; i < dc_topic_count; ++i) {
        if (dc_topic_table[i].topic_id == topic_id) {
            return &dc_topic_table[i];
        }
    }
    return 0;
}

int dc_subscribe(uint32_t topic_id, uint32_t subscriber_bit) {
    dc_topic_t *topic = distcom_find_topic(topic_id);
    if (!topic) {
        return -1;
    }
    topic->subscriber_mask |= subscriber_bit;
    return 0;
}

int dc_backpressure_check(uint32_t topic_id, uint32_t *out_bp_state) {
    dc_topic_t *topic = distcom_find_topic(topic_id);
    if (!topic) {
        return -1;
    }
    *out_bp_state = topic->backpressure;
    return 0;
}

int distcom_self_test(void) {
    if (dc_ready != 1) return -1;

    uint32_t msg_id = 0;
    if (dc_send(DC_MSG_REQUEST, DC_PROTO_LOCAL, 1000, &msg_id) != 0) return -1;
    if (dc_topic_register(1) != 0) return -1;
    if (dc_subscribe(1, 0x01u) != 0) return -1;

    return 0;
}
