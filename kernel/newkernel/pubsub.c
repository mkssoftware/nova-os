#include "pubsub.h"
#include "process.h"
#include <stddef.h>

typedef struct {
    uint32_t topic_id;
    uint32_t creator_pid;
    uint32_t namespace_id;
    uint32_t interface_id;
    uint32_t version;
    uint32_t allow_multi_publish;
    uint32_t state;
    uint32_t allowed_publishers[IPC_TOPIC_ALLOWED_PUBLISHERS];
    uint32_t allowed_publisher_count;
    uint32_t allowed_subscribers[IPC_TOPIC_ALLOWED_SUBSCRIBERS];
    uint32_t allowed_subscriber_count;
    uint32_t subscriber_count;
    uint32_t published_total;
    uint32_t delivered_total;
    uint32_t dropped_total;
    uint32_t denied_total;
    int in_use;
} topic_t;

typedef struct {
    uint32_t subscription_id;
    uint32_t topic_id;
    uint32_t subscriber_pid;
    uint32_t state;
    ipc_message_t queue[IPC_SUBSCRIPTION_QUEUE_CAPACITY];
    uint32_t queue_head;
    uint32_t queue_count;
    uint32_t received_total;
    uint32_t dropped_total;
    int in_use;
} subscription_t;

static topic_t topics[IPC_TOPIC_CAPACITY];
static subscription_t subscriptions[IPC_SUBSCRIPTION_CAPACITY];
static uint32_t topic_next_id = 1;
static uint32_t subscription_next_id = 1;
static int pubsub_mgr_ready = 0;

void pubsub_manager_initialize(void) {
    for (uint32_t i = 0; i < IPC_TOPIC_CAPACITY; ++i) {
        topics[i] = (topic_t){0};
    }
    for (uint32_t i = 0; i < IPC_SUBSCRIPTION_CAPACITY; ++i) {
        subscriptions[i] = (subscription_t){0};
    }
    topic_next_id = 1;
    subscription_next_id = 1;
    pubsub_mgr_ready = 1;
}

static topic_t *topic_find(uint32_t topic_id) {
    if (topic_id == 0) {
        return NULL;
    }
    for (uint32_t i = 0; i < IPC_TOPIC_CAPACITY; ++i) {
        if (topics[i].in_use && topics[i].topic_id == topic_id) {
            return &topics[i];
        }
    }
    return NULL;
}

static subscription_t *subscription_find(uint32_t subscription_id) {
    if (subscription_id == 0) {
        return NULL;
    }
    for (uint32_t i = 0; i < IPC_SUBSCRIPTION_CAPACITY; ++i) {
        if (subscriptions[i].in_use && subscriptions[i].subscription_id == subscription_id) {
            return &subscriptions[i];
        }
    }
    return NULL;
}

static int process_still_alive(uint32_t pid) {
    uint32_t st = process_state(pid);
    return st == PROCESS_STATE_RUNNING || st == PROCESS_STATE_SUSPENDED;
}

static int pid_in_list(const uint32_t *list, uint32_t count, uint32_t pid) {
    for (uint32_t i = 0; i < count; ++i) {
        if (list[i] == pid) {
            return 1;
        }
    }
    return 0;
}

/* Lazy-Abmeldung bei Prozess-Ende (#11 "Prozessende MUSS zugehoerige
   Subscriptions kontrolliert bereinigen"), analog zu channel.c's
   channel_check_peer_alive() - kein Hook in process_terminate(), stattdessen
   Erkennung bei jedem Zugriff. subscriber_count wird genau einmal beim
   Uebergang auf CLOSED dekrementiert. */
static void subscription_check_alive(subscription_t *sub) {
    if (sub->state == IPC_SUB_STATE_CLOSED) {
        return;
    }
    if (!process_still_alive(sub->subscriber_pid)) {
        sub->state = IPC_SUB_STATE_CLOSED;
        topic_t *topic = topic_find(sub->topic_id);
        if (topic && topic->subscriber_count > 0) {
            topic->subscriber_count--;
        }
    }
}

int topic_create(uint32_t creator_pid, uint32_t namespace_id, uint32_t interface_id,
                  uint32_t version, uint32_t allow_multi_publish, uint32_t *out_topic_id) {
    uint32_t slot;
    for (slot = 0; slot < IPC_TOPIC_CAPACITY; ++slot) {
        if (!topics[slot].in_use) {
            break;
        }
    }
    if (slot >= IPC_TOPIC_CAPACITY) {
        return IPC_PUBSUB_ERR_CAPACITY;
    }

    topic_t *t = &topics[slot];
    *t = (topic_t){0};
    t->topic_id = topic_next_id++;
    t->creator_pid = creator_pid;
    t->namespace_id = namespace_id;
    t->interface_id = interface_id;
    t->version = version;
    t->allow_multi_publish = allow_multi_publish;
    t->state = IPC_TOPIC_STATE_ACTIVE;
    t->in_use = 1;

    *out_topic_id = t->topic_id;
    return 0;
}

int topic_close(uint32_t pid, uint32_t topic_id) {
    topic_t *t = topic_find(topic_id);
    if (!t) {
        return IPC_PUBSUB_ERR_NOT_FOUND;
    }
    if (pid != t->creator_pid) {
        return IPC_PUBSUB_ERR_DENIED;
    }
    t->state = IPC_TOPIC_STATE_CLOSED; /* idempotent */
    return 0;
}

int topic_grant_publish(uint32_t creator_pid, uint32_t topic_id, uint32_t publisher_pid) {
    topic_t *t = topic_find(topic_id);
    if (!t) {
        return IPC_PUBSUB_ERR_NOT_FOUND;
    }
    if (creator_pid != t->creator_pid) {
        return IPC_PUBSUB_ERR_DENIED;
    }
    if (!t->allow_multi_publish) {
        /* Policy bindet das Topic an genau einen Publisher (#6). */
        return IPC_PUBSUB_ERR_DENIED;
    }
    if (publisher_pid == t->creator_pid || pid_in_list(t->allowed_publishers, t->allowed_publisher_count, publisher_pid)) {
        return 0; /* bereits autorisiert */
    }
    if (t->allowed_publisher_count >= IPC_TOPIC_ALLOWED_PUBLISHERS) {
        return IPC_PUBSUB_ERR_CAPACITY;
    }
    t->allowed_publishers[t->allowed_publisher_count++] = publisher_pid;
    return 0;
}

int topic_grant_subscribe(uint32_t creator_pid, uint32_t topic_id, uint32_t subscriber_pid) {
    topic_t *t = topic_find(topic_id);
    if (!t) {
        return IPC_PUBSUB_ERR_NOT_FOUND;
    }
    if (creator_pid != t->creator_pid) {
        return IPC_PUBSUB_ERR_DENIED;
    }
    if (subscriber_pid == t->creator_pid || pid_in_list(t->allowed_subscribers, t->allowed_subscriber_count, subscriber_pid)) {
        return 0; /* bereits autorisiert */
    }
    if (t->allowed_subscriber_count >= IPC_TOPIC_ALLOWED_SUBSCRIBERS) {
        return IPC_PUBSUB_ERR_CAPACITY;
    }
    t->allowed_subscribers[t->allowed_subscriber_count++] = subscriber_pid;
    return 0;
}

int topic_publish(uint32_t publisher_pid, uint32_t topic_id, uint32_t interface_id,
                   uint32_t version, const ipc_message_t *msg) {
    topic_t *t = topic_find(topic_id);
    if (!t) {
        return IPC_PUBSUB_ERR_NOT_FOUND;
    }
    if (t->state != IPC_TOPIC_STATE_ACTIVE) {
        return IPC_PUBSUB_ERR_INVALID_STATE;
    }
    if (publisher_pid != t->creator_pid && !pid_in_list(t->allowed_publishers, t->allowed_publisher_count, publisher_pid)) {
        t->denied_total++;
        return IPC_PUBSUB_ERR_DENIED;
    }
    if (interface_id != t->interface_id || version != t->version) {
        return IPC_PUBSUB_ERR_INCOMPATIBLE;
    }

    t->published_total++;
    for (uint32_t i = 0; i < IPC_SUBSCRIPTION_CAPACITY; ++i) {
        subscription_t *sub = &subscriptions[i];
        if (!sub->in_use || sub->topic_id != topic_id) {
            continue;
        }
        subscription_check_alive(sub);
        if (sub->state != IPC_SUB_STATE_ACTIVE) {
            continue; /* PAUSED: bewusst kein Zustellversuch; CLOSED: vorbei */
        }
        if (sub->queue_count >= IPC_SUBSCRIPTION_QUEUE_CAPACITY) {
            sub->dropped_total++;
            t->dropped_total++;
            continue; /* Backpressure: REJECT statt unbegrenztem Wachstum (#9) */
        }
        uint32_t tail = (sub->queue_head + sub->queue_count) % IPC_SUBSCRIPTION_QUEUE_CAPACITY;
        sub->queue[tail] = *msg;
        sub->queue_count++;
        t->delivered_total++;
    }
    return 0;
}

int topic_subscribe(uint32_t subscriber_pid, uint32_t topic_id, uint32_t *out_subscription_id) {
    topic_t *t = topic_find(topic_id);
    if (!t) {
        return IPC_PUBSUB_ERR_NOT_FOUND;
    }
    if (t->state != IPC_TOPIC_STATE_ACTIVE) {
        return IPC_PUBSUB_ERR_INVALID_STATE;
    }
    if (subscriber_pid != t->creator_pid && !pid_in_list(t->allowed_subscribers, t->allowed_subscriber_count, subscriber_pid)) {
        t->denied_total++;
        return IPC_PUBSUB_ERR_DENIED;
    }

    uint32_t slot;
    for (slot = 0; slot < IPC_SUBSCRIPTION_CAPACITY; ++slot) {
        if (!subscriptions[slot].in_use) {
            break;
        }
    }
    if (slot >= IPC_SUBSCRIPTION_CAPACITY) {
        return IPC_PUBSUB_ERR_CAPACITY;
    }

    subscription_t *sub = &subscriptions[slot];
    *sub = (subscription_t){0};
    sub->subscription_id = subscription_next_id++;
    sub->topic_id = topic_id;
    sub->subscriber_pid = subscriber_pid;
    sub->state = IPC_SUB_STATE_ACTIVE;
    sub->in_use = 1;

    t->subscriber_count++;
    *out_subscription_id = sub->subscription_id;
    return 0;
}

int subscription_pause(uint32_t pid, uint32_t subscription_id) {
    subscription_t *sub = subscription_find(subscription_id);
    if (!sub) {
        return IPC_PUBSUB_ERR_NOT_FOUND;
    }
    if (pid != sub->subscriber_pid) {
        return IPC_PUBSUB_ERR_DENIED;
    }
    subscription_check_alive(sub);
    if (sub->state == IPC_SUB_STATE_PAUSED) {
        return 0; /* idempotent */
    }
    if (sub->state != IPC_SUB_STATE_ACTIVE) {
        return IPC_PUBSUB_ERR_INVALID_STATE;
    }
    sub->state = IPC_SUB_STATE_PAUSED;
    return 0;
}

int subscription_resume(uint32_t pid, uint32_t subscription_id) {
    subscription_t *sub = subscription_find(subscription_id);
    if (!sub) {
        return IPC_PUBSUB_ERR_NOT_FOUND;
    }
    if (pid != sub->subscriber_pid) {
        return IPC_PUBSUB_ERR_DENIED;
    }
    subscription_check_alive(sub);
    if (sub->state == IPC_SUB_STATE_ACTIVE) {
        return 0; /* idempotent */
    }
    if (sub->state != IPC_SUB_STATE_PAUSED) {
        return IPC_PUBSUB_ERR_INVALID_STATE;
    }
    sub->state = IPC_SUB_STATE_ACTIVE;
    return 0;
}

int subscription_close(uint32_t pid, uint32_t subscription_id) {
    subscription_t *sub = subscription_find(subscription_id);
    if (!sub) {
        return IPC_PUBSUB_ERR_NOT_FOUND;
    }
    if (pid != sub->subscriber_pid) {
        return IPC_PUBSUB_ERR_DENIED;
    }
    if (sub->state == IPC_SUB_STATE_CLOSED) {
        return 0; /* idempotent */
    }
    sub->state = IPC_SUB_STATE_CLOSED;
    topic_t *t = topic_find(sub->topic_id);
    if (t && t->subscriber_count > 0) {
        t->subscriber_count--;
    }
    return 0;
}

int subscription_receive(uint32_t pid, uint32_t subscription_id, ipc_message_t *out_msg) {
    subscription_t *sub = subscription_find(subscription_id);
    if (!sub) {
        return IPC_PUBSUB_ERR_NOT_FOUND;
    }
    if (pid != sub->subscriber_pid) {
        return IPC_PUBSUB_ERR_DENIED;
    }
    subscription_check_alive(sub);
    if (sub->state == IPC_SUB_STATE_CLOSED) {
        return IPC_PUBSUB_ERR_CLOSED;
    }
    if (sub->queue_count == 0) {
        return IPC_PUBSUB_ERR_EMPTY;
    }
    *out_msg = sub->queue[sub->queue_head];
    sub->queue_head = (sub->queue_head + 1) % IPC_SUBSCRIPTION_QUEUE_CAPACITY;
    sub->queue_count--;
    sub->received_total++;
    return 0;
}

int topic_info(uint32_t topic_id, ipc_topic_info_t *out_info) {
    topic_t *t = topic_find(topic_id);
    if (!t) {
        return IPC_PUBSUB_ERR_NOT_FOUND;
    }
    out_info->topic_id = t->topic_id;
    out_info->creator_pid = t->creator_pid;
    out_info->namespace_id = t->namespace_id;
    out_info->interface_id = t->interface_id;
    out_info->version = t->version;
    out_info->allow_multi_publish = t->allow_multi_publish;
    out_info->state = t->state;
    out_info->subscriber_count = t->subscriber_count;
    out_info->published_total = t->published_total;
    out_info->delivered_total = t->delivered_total;
    out_info->dropped_total = t->dropped_total;
    out_info->denied_total = t->denied_total;
    return 0;
}

int subscription_info(uint32_t subscription_id, ipc_subscription_info_t *out_info) {
    subscription_t *sub = subscription_find(subscription_id);
    if (!sub) {
        return IPC_PUBSUB_ERR_NOT_FOUND;
    }
    subscription_check_alive(sub);
    out_info->subscription_id = sub->subscription_id;
    out_info->topic_id = sub->topic_id;
    out_info->subscriber_pid = sub->subscriber_pid;
    out_info->state = sub->state;
    out_info->queue_depth = sub->queue_count;
    out_info->received_total = sub->received_total;
    out_info->dropped_total = sub->dropped_total;
    return 0;
}

/* ----------------------------------------------------------------------- */
/* Selbsttest                                                              */
/* ----------------------------------------------------------------------- */

int pubsub_manager_self_test(void) {
    if (!pubsub_mgr_ready) {
        return -1;
    }

    uint32_t pid_pub = process_create(0, 0x4000u);
    uint32_t pid_sub1 = process_create(0, 0x5000u);
    uint32_t pid_sub2 = process_create(0, 0x6000u);
    if (pid_pub == 0 || pid_sub1 == 0 || pid_sub2 == 0) {
        return -1;
    }

    const uint32_t IFACE = 0x300u;
    const uint32_t VER = 1u;

    uint32_t topic_id;
    if (topic_create(pid_pub, 0x1, IFACE, VER, 0, &topic_id) != 0) return -1;

    ipc_message_t msg = {{0xAA, 0xBB, 0xCC, 0xDD}};

    /* Unautorisierter Publisher muss abgewiesen werden (#4/#5: Topic-
       Kenntnis allein gewaehrt kein Recht). */
    if (topic_publish(pid_sub1, topic_id, IFACE, VER, &msg) != IPC_PUBSUB_ERR_DENIED) return -1;

    /* allow_multi_publish==0: auch ein expliziter Grant-Versuch muss die
       Single-Publisher-Policy durchsetzen (#6). */
    if (topic_grant_publish(pid_pub, topic_id, pid_sub1) != IPC_PUBSUB_ERR_DENIED) return -1;

    /* Subscribe ohne Grant muss abgewiesen werden; danach muss es gelingen -
       und eine Publish-Capability darf KEINE Subscribe-Rechte erzeugen (#5):
       pid_sub1 ist hier nirgends als Publisher autorisiert, das testet die
       Trennung bereits implizit, da beide Rechte unabhaengig gewaehrt werden
       muessen. */
    uint32_t sub1_id, sub2_id;
    if (topic_subscribe(pid_sub1, topic_id, &sub1_id) != IPC_PUBSUB_ERR_DENIED) return -1;
    if (topic_grant_subscribe(pid_pub, topic_id, pid_sub1) != 0) return -1;
    if (topic_subscribe(pid_sub1, topic_id, &sub1_id) != 0) return -1;

    if (topic_subscribe(pid_sub2, topic_id, &sub2_id) != IPC_PUBSUB_ERR_DENIED) return -1;
    if (topic_grant_subscribe(pid_pub, topic_id, pid_sub2) != 0) return -1;
    if (topic_subscribe(pid_sub2, topic_id, &sub2_id) != 0) return -1;

    ipc_topic_info_t tinfo;
    if (topic_info(topic_id, &tinfo) != 0) return -1;
    if (tinfo.subscriber_count != 2) return -1;

    /* Typed Pub/Sub: Interface/Version-Mismatch wird abgewiesen (#3). */
    if (topic_publish(pid_pub, topic_id, IFACE, VER + 1u, &msg) != IPC_PUBSUB_ERR_INCOMPATIBLE) return -1;

    /* Many-to-Many: beide Subscriber erhalten dieselbe Nachricht (#6). */
    if (topic_publish(pid_pub, topic_id, IFACE, VER, &msg) != 0) return -1;
    ipc_message_t received = {{0, 0, 0, 0}};
    if (subscription_receive(pid_sub1, sub1_id, &received) != 0) return -1;
    if (received.words[0] != 0xAA || received.words[3] != 0xDD) return -1;
    if (subscription_receive(pid_sub2, sub2_id, &received) != 0) return -1;
    if (subscription_receive(pid_sub1, sub1_id, &received) != IPC_PUBSUB_ERR_EMPTY) return -1;

    /* Pause: pausierter Subscriber erhaelt NICHTS (kein Drop-Zaehler - das
       ist bewusste Pause, kein Ueberlastungsverlust). */
    if (subscription_pause(pid_sub2, sub2_id) != 0) return -1;
    if (topic_publish(pid_pub, topic_id, IFACE, VER, &msg) != 0) return -1;
    if (subscription_receive(pid_sub1, sub1_id, &received) != 0) return -1; /* sub1 bekommt es */
    if (subscription_receive(pid_sub2, sub2_id, &received) != IPC_PUBSUB_ERR_EMPTY) return -1; /* sub2 nicht */

    ipc_subscription_info_t sinfo;
    if (subscription_info(sub2_id, &sinfo) != 0) return -1;
    if (sinfo.state != IPC_SUB_STATE_PAUSED || sinfo.dropped_total != 0) return -1;

    if (subscription_resume(pid_sub2, sub2_id) != 0) return -1;
    if (topic_publish(pid_pub, topic_id, IFACE, VER, &msg) != 0) return -1;
    if (subscription_receive(pid_sub2, sub2_id, &received) != 0) return -1; /* wieder aktiv */

    /* Backpressure: sub1's Queue mit IPC_SUBSCRIPTION_QUEUE_CAPACITY (8)
       Nachrichten fuellen, ohne zu draenen - die naechste muss REJECTed
       werden statt unbegrenzt zu wachsen (#9). sub1 hat noch 1 ungedrainte
       Nachricht aus dem vorigen Publish (resume-Runde) wartend; sub2 wird
       fuer diesen Teil pausiert, damit nur sub1's Queue isoliert getestet
       wird (sonst wuerde auch sub2 beim 9. Publish gleichzeitig droppen). */
    if (subscription_receive(pid_sub1, sub1_id, &received) != 0) return -1; /* diese eine erst draenen */
    if (subscription_pause(pid_sub2, sub2_id) != 0) return -1;
    for (uint32_t i = 0; i < IPC_SUBSCRIPTION_QUEUE_CAPACITY; ++i) {
        if (topic_publish(pid_pub, topic_id, IFACE, VER, &msg) != 0) return -1;
    }
    if (topic_info(topic_id, &tinfo) != 0) return -1;
    uint32_t dropped_before = tinfo.dropped_total;
    if (topic_publish(pid_pub, topic_id, IFACE, VER, &msg) != 0) return -1; /* Queue jetzt voll -> drop fuer sub1 */
    if (topic_info(topic_id, &tinfo) != 0) return -1;
    if (tinfo.dropped_total != dropped_before + 1) return -1;
    if (subscription_info(sub1_id, &sinfo) != 0) return -1;
    if (sinfo.dropped_total != 1) return -1;
    if (subscription_resume(pid_sub2, sub2_id) != 0) return -1;

    /* Subscription schliessen: danach CLOSED statt EMPTY/Daten, idempotent. */
    if (subscription_close(pid_sub1, sub1_id) != 0) return -1;
    if (subscription_receive(pid_sub1, sub1_id, &received) != IPC_PUBSUB_ERR_CLOSED) return -1;
    if (subscription_close(pid_sub1, sub1_id) != 0) return -1; /* idempotent */
    if (topic_info(topic_id, &tinfo) != 0) return -1;
    if (tinfo.subscriber_count != 1) return -1; /* nur sub2 noch lebend */

    /* Lazy Cleanup bei Prozess-Ende (#11): sub2's Prozess stirbt, die
       Subscription muss OHNE Hook in process_terminate() beim naechsten
       Zugriff CLOSED werden, und das Topic darf dadurch NICHT blockieren. */
    if (process_terminate(pid_sub2, 0) != 0) return -1;
    if (topic_publish(pid_pub, topic_id, IFACE, VER, &msg) != 0) return -1; /* darf nicht haengen/fehlschlagen */
    if (subscription_info(sub2_id, &sinfo) != 0) return -1;
    if (sinfo.state != IPC_SUB_STATE_CLOSED) return -1;
    if (topic_info(topic_id, &tinfo) != 0) return -1;
    if (tinfo.subscriber_count != 0) return -1;

    /* Topic schliessen: Creator-only, idempotent, danach kein neues
       Publishing mehr moeglich. */
    if (topic_close(pid_sub1, topic_id) != IPC_PUBSUB_ERR_DENIED) return -1; /* nicht Creator */
    if (topic_close(pid_pub, topic_id) != 0) return -1;
    if (topic_close(pid_pub, topic_id) != 0) return -1; /* idempotent */
    if (topic_publish(pid_pub, topic_id, IFACE, VER, &msg) != IPC_PUBSUB_ERR_INVALID_STATE) return -1;

    /* Unbekannte IDs. */
    ipc_topic_info_t dummy_t;
    ipc_subscription_info_t dummy_s;
    if (topic_info(0xFFFFu, &dummy_t) != IPC_PUBSUB_ERR_NOT_FOUND) return -1;
    if (subscription_info(0xFFFFu, &dummy_s) != IPC_PUBSUB_ERR_NOT_FOUND) return -1;

    /* Aufraeumen: kein dauerhafter Fussabdruck. pid_sub2 ist bereits
       terminiert, nur noch reapen. */
    if (process_terminate(pid_pub, 0) != 0) return -1;
    if (process_terminate(pid_sub1, 0) != 0) return -1;
    if (process_reap(pid_pub) != 0) return -1;
    if (process_reap(pid_sub1) != 0) return -1;
    if (process_reap(pid_sub2) != 0) return -1;

    return 0;
}
