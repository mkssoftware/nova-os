#include "eventbus.h"
#include "pubsub.h"
#include "interrupt.h"
#include "process.h"
#include <stddef.h>

/* Einziger genuin neuer Zustand des Event Bus gegenueber Phase 4: pro
   Subscription ein optionaler Source-Filter (pubsub.c kennt diesen nicht -
   siehe eventbus.h). Alles andere (Autorisierung, Queue, Backpressure,
   lazy Cleanup bei Prozess-Ende) bleibt vollstaendig in pubsub.c. */
typedef struct {
    uint32_t subscription_id;
    uint32_t source_filter;
    uint32_t filtered_total;
    int in_use;
} eventbus_filter_t;

static eventbus_filter_t filters[EVENTBUS_SUBSCRIPTION_CAPACITY];
static uint32_t eventbus_next_event_id = 1;
static int eventbus_mgr_ready = 0;

void eventbus_manager_initialize(void) {
    for (uint32_t i = 0; i < EVENTBUS_SUBSCRIPTION_CAPACITY; ++i) {
        filters[i] = (eventbus_filter_t){0};
    }
    eventbus_next_event_id = 1;
    eventbus_mgr_ready = 1;
}

static eventbus_filter_t *filter_find(uint32_t subscription_id) {
    for (uint32_t i = 0; i < EVENTBUS_SUBSCRIPTION_CAPACITY; ++i) {
        if (filters[i].in_use && filters[i].subscription_id == subscription_id) {
            return &filters[i];
        }
    }
    return NULL;
}

static int map_pubsub_error(int rc) {
    switch (rc) {
        case 0: return 0;
        case IPC_PUBSUB_ERR_NOT_FOUND:     return EVENTBUS_ERR_NOT_FOUND;
        case IPC_PUBSUB_ERR_DENIED:        return EVENTBUS_ERR_DENIED;
        case IPC_PUBSUB_ERR_CAPACITY:      return EVENTBUS_ERR_CAPACITY;
        case IPC_PUBSUB_ERR_CLOSED:        return EVENTBUS_ERR_CLOSED;
        case IPC_PUBSUB_ERR_INCOMPATIBLE:  return EVENTBUS_ERR_INCOMPATIBLE;
        case IPC_PUBSUB_ERR_EMPTY:         return EVENTBUS_ERR_EMPTY;
        case IPC_PUBSUB_ERR_INVALID_STATE: return EVENTBUS_ERR_INVALID_STATE;
        default:                           return EVENTBUS_ERR_NOT_FOUND;
    }
}

int eventbus_register_type(uint32_t creator_pid, uint32_t event_type,
                            uint32_t allow_multi_publish, uint32_t *out_topic_id) {
    int rc = topic_create(creator_pid, 0, event_type, 1u, allow_multi_publish, out_topic_id);
    return map_pubsub_error(rc);
}

int eventbus_grant_publish(uint32_t creator_pid, uint32_t topic_id, uint32_t publisher_pid) {
    return map_pubsub_error(topic_grant_publish(creator_pid, topic_id, publisher_pid));
}

int eventbus_grant_subscribe(uint32_t creator_pid, uint32_t topic_id, uint32_t subscriber_pid) {
    return map_pubsub_error(topic_grant_subscribe(creator_pid, topic_id, subscriber_pid));
}

int eventbus_publish(uint32_t publisher_pid, uint32_t topic_id, uint32_t payload,
                      uint32_t *out_event_id) {
    ipc_topic_info_t info;
    int rc = topic_info(topic_id, &info);
    if (rc != 0) {
        return map_pubsub_error(rc);
    }

    uint32_t event_id = eventbus_next_event_id++;
    ipc_message_t msg;
    msg.words[0] = event_id;
    msg.words[1] = publisher_pid;
    msg.words[2] = timer_get_ticks();
    msg.words[3] = payload;

    rc = topic_publish(publisher_pid, topic_id, info.interface_id, info.version, &msg);
    if (rc != 0) {
        return map_pubsub_error(rc);
    }
    *out_event_id = event_id;
    return 0;
}

int eventbus_subscribe(uint32_t subscriber_pid, uint32_t topic_id, uint32_t source_filter,
                        uint32_t *out_subscription_id) {
    uint32_t sub_id;
    int rc = topic_subscribe(subscriber_pid, topic_id, &sub_id);
    if (rc != 0) {
        return map_pubsub_error(rc);
    }

    uint32_t slot;
    for (slot = 0; slot < EVENTBUS_SUBSCRIPTION_CAPACITY; ++slot) {
        if (!filters[slot].in_use) {
            break;
        }
    }
    if (slot >= EVENTBUS_SUBSCRIPTION_CAPACITY) {
        subscription_close(subscriber_pid, sub_id); /* kein halb-lebendiger Zustand */
        return EVENTBUS_ERR_CAPACITY;
    }

    filters[slot].subscription_id = sub_id;
    filters[slot].source_filter = source_filter;
    filters[slot].filtered_total = 0;
    filters[slot].in_use = 1;

    *out_subscription_id = sub_id;
    return 0;
}

int eventbus_receive(uint32_t pid, uint32_t subscription_id, eventbus_event_t *out_event) {
    eventbus_filter_t *f = filter_find(subscription_id);
    if (!f) {
        return EVENTBUS_ERR_NOT_FOUND;
    }

    for (;;) {
        ipc_message_t raw;
        int rc = subscription_receive(pid, subscription_id, &raw);
        if (rc != 0) {
            return map_pubsub_error(rc);
        }
        uint32_t source_pid = raw.words[1];
        if (f->source_filter != 0 && f->source_filter != source_pid) {
            f->filtered_total++;
            continue; /* naechstes wartendes Event pruefen - Queue schrumpft, Schleife terminiert */
        }
        out_event->event_id = raw.words[0];
        out_event->source_pid = source_pid;
        out_event->timestamp = raw.words[2];
        out_event->payload = raw.words[3];
        out_event->event_type = 0;
        ipc_subscription_info_t sinfo;
        ipc_topic_info_t tinfo;
        if (subscription_info(subscription_id, &sinfo) == 0 && topic_info(sinfo.topic_id, &tinfo) == 0) {
            out_event->event_type = tinfo.interface_id;
        }
        return 0;
    }
}

int eventbus_unsubscribe(uint32_t pid, uint32_t subscription_id) {
    /* Der Filter-Eintrag bleibt bestehen (wie pubsub.c's eigene Subscription-
       Slots nach dem Schliessen) - sonst wuerde ein nachfolgendes
       eventbus_receive() faelschlich NOT_FOUND statt CLOSED liefern, und
       Introspektion auf geschlossenen Subscriptions bliebe nicht moeglich
       (#Introspection, analog zu channel.c/pubsub.c). */
    return map_pubsub_error(subscription_close(pid, subscription_id));
}

int eventbus_subscription_info(uint32_t subscription_id, eventbus_subscription_info_t *out_info) {
    eventbus_filter_t *f = filter_find(subscription_id);
    if (!f) {
        return EVENTBUS_ERR_NOT_FOUND;
    }
    ipc_subscription_info_t sinfo;
    int rc = subscription_info(subscription_id, &sinfo);
    if (rc != 0) {
        return map_pubsub_error(rc);
    }
    out_info->subscription_id = sinfo.subscription_id;
    out_info->topic_id = sinfo.topic_id;
    out_info->subscriber_pid = sinfo.subscriber_pid;
    out_info->source_filter = f->source_filter;
    out_info->state = sinfo.state;
    out_info->queue_depth = sinfo.queue_depth;
    out_info->received_total = sinfo.received_total;
    out_info->filtered_total = f->filtered_total;
    return 0;
}

/* ----------------------------------------------------------------------- */
/* Selbsttest                                                              */
/* ----------------------------------------------------------------------- */

int eventbus_manager_self_test(void) {
    if (!eventbus_mgr_ready) {
        return -1;
    }

    uint32_t pid_pub = process_create(0, 0x7000u);
    uint32_t pid_sub1 = process_create(0, 0x8000u);
    uint32_t pid_sub2 = process_create(0, 0x9000u);
    if (pid_pub == 0 || pid_sub1 == 0 || pid_sub2 == 0) {
        return -1;
    }

    const uint32_t EVENT_TYPE = 0x0A01u; /* z.B. "Device.Added" */

    uint32_t topic_id;
    if (eventbus_register_type(pid_pub, EVENT_TYPE, 1u, &topic_id) != 0) return -1;

    /* Unautorisierter Publisher muss abgewiesen werden (#4). */
    uint32_t dummy_event_id;
    if (eventbus_publish(pid_sub1, topic_id, 0x1111u, &dummy_event_id) != EVENTBUS_ERR_DENIED) return -1;

    /* sub1 abonniert OHNE Source-Filter (sieht alles), sub2 NUR von pid_pub
       (#Subscription Filter nach Source). */
    if (eventbus_grant_subscribe(pid_pub, topic_id, pid_sub1) != 0) return -1;
    if (eventbus_grant_subscribe(pid_pub, topic_id, pid_sub2) != 0) return -1;
    uint32_t sub1_id, sub2_id;
    if (eventbus_subscribe(pid_sub1, topic_id, 0, &sub1_id) != 0) return -1;
    if (eventbus_subscribe(pid_sub2, topic_id, pid_pub, &sub2_id) != 0) return -1;

    /* pid_sub1 wird zusaetzlich als zweiter Publisher autorisiert (#6
       "mehrere Publisher...sofern Policy dies erlaubt", hier allow_multi_
       publish=1) - Publish- und Subscribe-Rechte bleiben dabei unabhaengig
       (#5: eine Subscribe-Capability erzeugt kein Publish-Recht und
       umgekehrt - pid_sub1 hatte bereits Subscribe-Rechte, Publish wird
       SEPARAT gewaehrt). */
    if (eventbus_grant_publish(pid_pub, topic_id, pid_sub1) != 0) return -1;

    /* Event von pid_pub: beide Subscriptions sehen es (sub2's Filter
       pid_pub passt exakt). */
    uint32_t event_a;
    if (eventbus_publish(pid_pub, topic_id, 0xAAAAu, &event_a) != 0) return -1;

    eventbus_event_t ev;
    if (eventbus_receive(pid_sub1, sub1_id, &ev) != 0) return -1;
    if (ev.event_id != event_a || ev.source_pid != pid_pub || ev.payload != 0xAAAAu) return -1;
    if (ev.event_type != EVENT_TYPE) return -1;
    if (eventbus_receive(pid_sub2, sub2_id, &ev) != 0) return -1;
    if (ev.event_id != event_a || ev.source_pid != pid_pub) return -1;

    /* Event von pid_sub1 (zweiter Publisher): sub1 (kein Filter) sieht es,
       sub2 (Filter=pid_pub) NICHT - muss als EMPTY erscheinen, nicht als
       Fehler, und filtered_total muss hochzaehlen (#Filter duerfen keine
       unautorisierte Sichtbarkeit erzeugen - hier bewusst umgekehrt: sie
       VERBERGEN autorisiert empfangbare, aber nicht gewollte Events). */
    uint32_t event_b;
    if (eventbus_publish(pid_sub1, topic_id, 0xBBBBu, &event_b) != 0) return -1;
    if (eventbus_receive(pid_sub1, sub1_id, &ev) != 0) return -1;
    if (ev.event_id != event_b || ev.source_pid != pid_sub1) return -1;
    if (eventbus_receive(pid_sub2, sub2_id, &ev) != EVENTBUS_ERR_EMPTY) return -1;

    eventbus_subscription_info_t sinfo;
    if (eventbus_subscription_info(sub2_id, &sinfo) != 0) return -1;
    if (sinfo.filtered_total != 1 || sinfo.source_filter != pid_pub) return -1;

    /* Unsubscribe: danach CLOSED statt EMPTY/Daten. */
    if (eventbus_unsubscribe(pid_sub1, sub1_id) != 0) return -1;
    if (eventbus_receive(pid_sub1, sub1_id, &ev) != EVENTBUS_ERR_CLOSED) return -1;

    /* Lazy Cleanup bei Prozess-Ende (ueber pubsub.c geerbt): sub2's Prozess
       stirbt, Publish darf nicht haengen/fehlschlagen, Subscription muss
       lazy auf CLOSED kippen. */
    if (process_terminate(pid_sub2, 0) != 0) return -1;
    uint32_t event_c;
    if (eventbus_publish(pid_pub, topic_id, 0xCCCCu, &event_c) != 0) return -1;
    if (eventbus_subscription_info(sub2_id, &sinfo) != 0) return -1;
    if (sinfo.state != IPC_SUB_STATE_CLOSED) return -1;

    /* Unbekannte Subscription-ID. */
    if (eventbus_subscription_info(0xFFFFu, &sinfo) != EVENTBUS_ERR_NOT_FOUND) return -1;

    /* Aufraeumen: kein dauerhafter Fussabdruck. pid_sub2 ist bereits
       terminiert, nur noch reapen. */
    if (process_terminate(pid_pub, 0) != 0) return -1;
    if (process_terminate(pid_sub1, 0) != 0) return -1;
    if (process_reap(pid_pub) != 0) return -1;
    if (process_reap(pid_sub1) != 0) return -1;
    if (process_reap(pid_sub2) != 0) return -1;

    return 0;
}
