/* NovaOS C-Kernel - §101 Event Bus (Port aus security32.inc). */
#include "evbus.h"

typedef struct {
    uint32_t type_id;
    uint8_t event_class;
    uint8_t min_payload;
    uint16_t max_payload;
    uint32_t flags;
    int present;
} evbus_schema_t;

typedef struct {
    uint32_t id;
    uint32_t type_filter;
    evbus_handler_fn_t handler;
    uint8_t state;
    uint8_t delivery;
    uint8_t flags;
    int present;
    uint32_t queued;
    uint32_t lost;
} evbus_sub_t;

static evbus_schema_t evbus_schemas[EVBUS_MAX_SCHEMAS];
static evbus_sub_t evbus_subs[EVBUS_MAX_SUBS];
static uint32_t evbus_next_sub_id = 0;
static uint32_t evbus_sequence = 0;
static int evbus_depth = 0;
static uint32_t evbus_stat_published = 0;
static uint32_t evbus_stat_delivered = 0;
static uint32_t evbus_stat_rejected = 0;
static uint32_t evbus_test_flag = 0;

static evbus_schema_t *evbus_find_schema(uint32_t type_id) {
    for (uint32_t i = 0; i < EVBUS_MAX_SCHEMAS; ++i) {
        if (evbus_schemas[i].present && evbus_schemas[i].type_id == type_id) return &evbus_schemas[i];
    }
    return 0;
}

int evbus_register_schema(uint32_t type_id, uint32_t event_class, uint32_t max_payload_size, uint32_t flags) {
    if (evbus_find_schema(type_id)) return -1; /* bereits vorhanden */

    for (uint32_t i = 0; i < EVBUS_MAX_SCHEMAS; ++i) {
        if (!evbus_schemas[i].present) {
            evbus_schemas[i].type_id = type_id;
            evbus_schemas[i].event_class = (uint8_t)event_class;
            evbus_schemas[i].min_payload = 0;
            evbus_schemas[i].max_payload = (uint16_t)max_payload_size;
            evbus_schemas[i].flags = flags;
            evbus_schemas[i].present = 1;
            return 0;
        }
    }
    return -1;
}

int evbus_subscribe(uint32_t type_filter, evbus_handler_fn_t handler, uint32_t delivery_mode, uint32_t *out_sub_id) {
    for (uint32_t i = 0; i < EVBUS_MAX_SUBS; ++i) {
        if (!evbus_subs[i].present) {
            uint32_t id = ++evbus_next_sub_id;
            evbus_subs[i].id = id;
            evbus_subs[i].type_filter = type_filter;
            evbus_subs[i].handler = handler;
            evbus_subs[i].state = NP_SUB_ACTIVE;
            evbus_subs[i].delivery = (uint8_t)delivery_mode;
            evbus_subs[i].flags = 0;
            evbus_subs[i].present = 1;
            evbus_subs[i].queued = 0;
            evbus_subs[i].lost = 0;
            *out_sub_id = id;
            return 0;
        }
    }
    return -1;
}

int evbus_publish(uint32_t type_id, uint32_t src_id, uint32_t subj_id, const void *payload, uint32_t payload_size) {
    (void)payload;

    if (evbus_depth >= 1) return -1; /* Re-Entrant-Schutz */

    evbus_schema_t *schema = evbus_find_schema(type_id);
    if (!schema) {
        ++evbus_stat_rejected;
        return -1;
    }
    if (payload_size > schema->max_payload) {
        ++evbus_stat_rejected;
        return -1;
    }

    ++evbus_sequence;
    evbus_depth = 1;

    for (uint32_t i = 0; i < EVBUS_MAX_SUBS; ++i) {
        evbus_sub_t *sub = &evbus_subs[i];
        if (!sub->present || sub->state != NP_SUB_ACTIVE) continue;
        if (sub->type_filter != 0 && sub->type_filter != type_id) continue;

        if (sub->delivery != NP_EVENT_DELIVERY_CALLBACK || sub->handler == 0) {
            ++sub->lost; /* Bootstrap: kein Queue-/IPC-Deliver */
            continue;
        }

        sub->handler(type_id, src_id, subj_id);
        ++evbus_stat_delivered;
    }

    evbus_depth = 0;
    ++evbus_stat_published;
    return 0;
}

void evbus_initialize(void) {
    for (uint32_t i = 0; i < EVBUS_MAX_SCHEMAS; ++i) evbus_schemas[i] = (evbus_schema_t){0};
    for (uint32_t i = 0; i < EVBUS_MAX_SUBS; ++i) evbus_subs[i] = (evbus_sub_t){0};
    evbus_next_sub_id = 0;
    evbus_sequence = 0;
    evbus_depth = 0;
    evbus_stat_published = 0;
    evbus_stat_delivered = 0;
    evbus_stat_rejected = 0;

    evbus_register_schema(NP_EVENT_BOOT_READY, NP_EVENT_CLASS_LIFECYCLE, 0, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY);
    evbus_register_schema(NP_EVENT_BOOT_PANIC, NP_EVENT_CLASS_ERROR, 4, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY);
    evbus_register_schema(NP_EVENT_OBJ_CREATED, NP_EVENT_CLASS_LIFECYCLE, 8, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY);
    evbus_register_schema(NP_EVENT_OBJ_ACTIVATED, NP_EVENT_CLASS_LIFECYCLE, 8, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY);
    evbus_register_schema(NP_EVENT_OBJ_DESTROYED, NP_EVENT_CLASS_LIFECYCLE, 8, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY);
    evbus_register_schema(NP_EVENT_OBJ_STATE_CHANGED, NP_EVENT_CLASS_STATE, 8, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY);
    evbus_register_schema(NP_EVENT_OBJ_LINK_ADDED, NP_EVENT_CLASS_STATE, 8, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY);
    evbus_register_schema(NP_EVENT_OBJ_LINK_REMOVED, NP_EVENT_CLASS_STATE, 8, NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY);
}

static void evbus_test_callback(uint32_t type_id, uint32_t src_id, uint32_t subj_id) {
    (void)type_id;
    (void)src_id;
    (void)subj_id;
    evbus_test_flag = 0xCAFEBABEu;
}

int evbus_self_test(void) {
    evbus_schema_t *schema = evbus_find_schema(NP_EVENT_BOOT_READY);
    if (!schema || schema->event_class != NP_EVENT_CLASS_LIFECYCLE) return -1;

    if (evbus_register_schema(NP_EVENT_BOOT_READY, NP_EVENT_CLASS_LIFECYCLE, 0, 0) == 0) return -1;

    evbus_test_flag = 0;
    uint32_t sub1;
    if (evbus_subscribe(NP_EVENT_BOOT_READY, evbus_test_callback, NP_EVENT_DELIVERY_CALLBACK, &sub1) != 0) return -1;

    if (evbus_publish(NP_EVENT_BOOT_READY, 0, 0, 0, 0) != 0) return -1;
    if (evbus_test_flag != 0xCAFEBABEu) return -1;

    evbus_test_flag = 0;
    uint32_t sub2;
    if (evbus_subscribe(NP_EVENT_OBJ_CREATED, evbus_test_callback, NP_EVENT_DELIVERY_CALLBACK, &sub2) != 0) return -1;

    if (evbus_publish(NP_EVENT_OBJ_CREATED, 1, 2, 0, 0) != 0) return -1;
    if (evbus_test_flag != 0xCAFEBABEu) return -1;

    uint8_t test_buf[16] = {0};
    if (evbus_publish(NP_EVENT_OBJ_CREATED, 0, 0, test_buf, 4) != 0) return -1;
    if (evbus_publish(NP_EVENT_OBJ_CREATED, 0, 0, test_buf, 64) == 0) return -1; /* zu gross */

    uint32_t seq_before = evbus_sequence;
    if (evbus_publish(NP_EVENT_BOOT_READY, 0, 0, 0, 0) != 0) return -1;
    if (evbus_sequence <= seq_before) return -1;

    return 0;
}
