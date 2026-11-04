#include "channel.h"
#include "process.h"
#include <stddef.h>

typedef struct {
    uint32_t channel_id;
    uint32_t endpoint_a;
    uint32_t endpoint_b;
    uint32_t owner_a;
    uint32_t owner_b;
    uint32_t interface_id;
    uint32_t version;
    uint32_t direction;
    uint32_t state;
    uint32_t sent_total;
    uint32_t received_total;
    uint32_t denied_total;
    int in_use;
} channel_t;

static channel_t channels[IPC_CHANNEL_CAPACITY];
static uint32_t channel_next_id = 1;
static int channel_mgr_ready = 0;

void channel_manager_initialize(void) {
    for (uint32_t i = 0; i < IPC_CHANNEL_CAPACITY; ++i) {
        channels[i] = (channel_t){0};
    }
    channel_next_id = 1;
    channel_mgr_ready = 1;
}

static channel_t *channel_find(uint32_t channel_id) {
    if (channel_id == 0) {
        return NULL;
    }
    for (uint32_t i = 0; i < IPC_CHANNEL_CAPACITY; ++i) {
        if (channels[i].in_use && channels[i].channel_id == channel_id) {
            return &channels[i];
        }
    }
    return NULL;
}

static int process_still_alive(uint32_t pid) {
    uint32_t st = process_state(pid);
    return st == PROCESS_STATE_RUNNING || st == PROCESS_STATE_SUSPENDED;
}

/* Lazy-Erkennung von Endpoint-Verlust, analog zu rpc.c's
   rpc_call_check_deadline() - laeuft bei jedem Zugriff, kein Polling. */
static void channel_check_peer_alive(channel_t *ch) {
    if (ch->state == IPC_CHANNEL_STATE_CLOSED || ch->state == IPC_CHANNEL_STATE_BROKEN) {
        return;
    }
    if (!process_still_alive(ch->owner_a) || !process_still_alive(ch->owner_b)) {
        ch->state = IPC_CHANNEL_STATE_BROKEN;
    }
}

int channel_open(uint32_t creator_pid, uint32_t endpoint_a, uint32_t endpoint_b,
                  uint32_t interface_id, uint32_t version, uint32_t direction,
                  uint32_t *out_channel_id) {
    ipc_endpoint_info_t info_a, info_b;
    if (ipc_endpoint_info(endpoint_a, &info_a) != 0) {
        return IPC_CHANNEL_ERR_NOT_FOUND;
    }
    if (ipc_endpoint_info(endpoint_b, &info_b) != 0) {
        return IPC_CHANNEL_ERR_NOT_FOUND;
    }
    if (creator_pid != info_a.owner_pid) {
        return IPC_CHANNEL_ERR_DENIED;
    }
    if (direction != IPC_CHANNEL_DIR_UNIDIRECTIONAL && direction != IPC_CHANNEL_DIR_BIDIRECTIONAL) {
        return IPC_CHANNEL_ERR_DENIED;
    }

    /* Keine neuen Rechte erzeugen (#3/#10) - nur bereits per
       ipc_endpoint_grant_send() bestehende Autorisierung pruefen. */
    if (ipc_endpoint_check_sender(endpoint_b, info_a.owner_pid) != 0) {
        return IPC_CHANNEL_ERR_DENIED;
    }
    if (direction == IPC_CHANNEL_DIR_BIDIRECTIONAL) {
        if (ipc_endpoint_check_sender(endpoint_a, info_b.owner_pid) != 0) {
            return IPC_CHANNEL_ERR_DENIED;
        }
    }

    uint32_t slot;
    for (slot = 0; slot < IPC_CHANNEL_CAPACITY; ++slot) {
        if (!channels[slot].in_use) {
            break;
        }
    }
    if (slot >= IPC_CHANNEL_CAPACITY) {
        return IPC_CHANNEL_ERR_CAPACITY;
    }

    channel_t *ch = &channels[slot];
    ch->channel_id = channel_next_id++;
    ch->endpoint_a = endpoint_a;
    ch->endpoint_b = endpoint_b;
    ch->owner_a = info_a.owner_pid;
    ch->owner_b = info_b.owner_pid;
    ch->interface_id = interface_id;
    ch->version = version;
    ch->direction = direction;
    ch->state = IPC_CHANNEL_STATE_ACTIVE;
    ch->sent_total = 0;
    ch->received_total = 0;
    ch->denied_total = 0;
    ch->in_use = 1;

    *out_channel_id = ch->channel_id;
    return 0;
}

static int map_endpoint_error(int ipc_rc) {
    switch (ipc_rc) {
        case 0: return 0;
        case IPC_ERR_NOT_FOUND: return IPC_CHANNEL_ERR_NOT_FOUND;
        case IPC_ERR_CLOSED:    return IPC_CHANNEL_ERR_INVALID_STATE;
        case IPC_ERR_DENIED:    return IPC_CHANNEL_ERR_DENIED;
        case IPC_ERR_FULL:      return IPC_CHANNEL_ERR_FULL;
        case IPC_ERR_CAPACITY:  return IPC_CHANNEL_ERR_CAPACITY;
        default:                return IPC_CHANNEL_ERR_NOT_FOUND;
    }
}

int channel_send(uint32_t sender_pid, uint32_t channel_id, uint32_t interface_id,
                  uint32_t version, const ipc_message_t *msg) {
    channel_t *ch = channel_find(channel_id);
    if (!ch) {
        return IPC_CHANNEL_ERR_NOT_FOUND;
    }
    channel_check_peer_alive(ch);
    if (ch->state == IPC_CHANNEL_STATE_BROKEN) {
        return IPC_CHANNEL_ERR_BROKEN;
    }
    if (ch->state != IPC_CHANNEL_STATE_ACTIVE) {
        return IPC_CHANNEL_ERR_INVALID_STATE;
    }
    if (interface_id != ch->interface_id || version != ch->version) {
        return IPC_CHANNEL_ERR_INCOMPATIBLE;
    }

    uint32_t target_endpoint;
    if (sender_pid == ch->owner_a) {
        target_endpoint = ch->endpoint_b;
    } else if (sender_pid == ch->owner_b && ch->direction == IPC_CHANNEL_DIR_BIDIRECTIONAL) {
        target_endpoint = ch->endpoint_a;
    } else {
        ch->denied_total++;
        return IPC_CHANNEL_ERR_DENIED;
    }

    int rc = ipc_endpoint_send(sender_pid, target_endpoint, msg);
    if (rc == 0) {
        ch->sent_total++;
        return 0;
    }
    if (rc == IPC_ERR_DENIED) {
        ch->denied_total++;
    }
    return map_endpoint_error(rc);
}

int channel_receive(uint32_t receiver_pid, uint32_t channel_id, uint32_t interface_id,
                     uint32_t version, ipc_message_t *out_msg) {
    channel_t *ch = channel_find(channel_id);
    if (!ch) {
        return IPC_CHANNEL_ERR_NOT_FOUND;
    }
    channel_check_peer_alive(ch);
    if (ch->state == IPC_CHANNEL_STATE_BROKEN) {
        return IPC_CHANNEL_ERR_BROKEN;
    }
    if (ch->state != IPC_CHANNEL_STATE_ACTIVE && ch->state != IPC_CHANNEL_STATE_CLOSING) {
        return IPC_CHANNEL_ERR_INVALID_STATE;
    }
    if (interface_id != ch->interface_id || version != ch->version) {
        return IPC_CHANNEL_ERR_INCOMPATIBLE;
    }

    uint32_t source_endpoint;
    if (receiver_pid == ch->owner_b) {
        source_endpoint = ch->endpoint_b;
    } else if (receiver_pid == ch->owner_a && ch->direction == IPC_CHANNEL_DIR_BIDIRECTIONAL) {
        source_endpoint = ch->endpoint_a;
    } else {
        ch->denied_total++;
        return IPC_CHANNEL_ERR_DENIED;
    }

    int rc = ipc_endpoint_receive(receiver_pid, source_endpoint, out_msg);
    if (rc == 0) {
        ch->received_total++;
        return 0;
    }
    if (rc == IPC_ERR_DENIED) {
        ch->denied_total++;
    }
    return map_endpoint_error(rc);
}

int channel_close_request(uint32_t pid, uint32_t channel_id) {
    channel_t *ch = channel_find(channel_id);
    if (!ch) {
        return IPC_CHANNEL_ERR_NOT_FOUND;
    }
    if (pid != ch->owner_a && pid != ch->owner_b) {
        return IPC_CHANNEL_ERR_DENIED;
    }
    channel_check_peer_alive(ch);

    if (ch->state == IPC_CHANNEL_STATE_CLOSING || ch->state == IPC_CHANNEL_STATE_CLOSED) {
        return 0; /* idempotent */
    }
    if (ch->state == IPC_CHANNEL_STATE_ACTIVE || ch->state == IPC_CHANNEL_STATE_BROKEN) {
        ch->state = IPC_CHANNEL_STATE_CLOSING;
        return 0;
    }
    return IPC_CHANNEL_ERR_INVALID_STATE;
}

static int endpoint_queue_empty(uint32_t endpoint_id) {
    ipc_endpoint_info_t info;
    if (ipc_endpoint_info(endpoint_id, &info) != 0) {
        return 1; /* Endpoint verschwunden - nichts mehr zu drainen */
    }
    return info.queue_depth == 0;
}

int channel_close_finish(uint32_t pid, uint32_t channel_id) {
    channel_t *ch = channel_find(channel_id);
    if (!ch) {
        return IPC_CHANNEL_ERR_NOT_FOUND;
    }
    if (pid != ch->owner_a && pid != ch->owner_b) {
        return IPC_CHANNEL_ERR_DENIED;
    }
    if (ch->state == IPC_CHANNEL_STATE_CLOSED) {
        return 0; /* idempotent */
    }
    if (ch->state != IPC_CHANNEL_STATE_CLOSING) {
        return IPC_CHANNEL_ERR_INVALID_STATE;
    }
    if (!endpoint_queue_empty(ch->endpoint_a) || !endpoint_queue_empty(ch->endpoint_b)) {
        return IPC_CHANNEL_ERR_NOT_DRAINED;
    }
    ch->state = IPC_CHANNEL_STATE_CLOSED;
    return 0;
}

int channel_info(uint32_t channel_id, ipc_channel_info_t *out_info) {
    channel_t *ch = channel_find(channel_id);
    if (!ch) {
        return IPC_CHANNEL_ERR_NOT_FOUND;
    }
    channel_check_peer_alive(ch);
    out_info->channel_id = ch->channel_id;
    out_info->endpoint_a = ch->endpoint_a;
    out_info->endpoint_b = ch->endpoint_b;
    out_info->owner_a = ch->owner_a;
    out_info->owner_b = ch->owner_b;
    out_info->interface_id = ch->interface_id;
    out_info->version = ch->version;
    out_info->direction = ch->direction;
    out_info->state = ch->state;
    out_info->sent_total = ch->sent_total;
    out_info->received_total = ch->received_total;
    out_info->denied_total = ch->denied_total;
    return 0;
}

/* ----------------------------------------------------------------------- */
/* Selbsttest                                                              */
/* ----------------------------------------------------------------------- */

int channel_manager_self_test(void) {
    if (!channel_mgr_ready) {
        return -1;
    }

    uint32_t pid_a = process_create(0, 0x1000u);
    uint32_t pid_b = process_create(0, 0x2000u);
    uint32_t pid_stranger = process_create(0, 0x3000u);
    if (pid_a == 0 || pid_b == 0 || pid_stranger == 0) {
        return -1;
    }

    uint32_t ep_a, ep_b;
    if (ipc_endpoint_create(pid_a, 4, &ep_a) != 0) return -1;
    if (ipc_endpoint_create(pid_b, 4, &ep_b) != 0) return -1;

    const uint32_t IFACE = 0x200u;
    const uint32_t VER = 1u;

    uint32_t chan_id;
    /* Ohne vorherige Autorisierung muss open scheitern (#3: keine neuen
       Rechte - channel_open() darf keine Allowlist-Eintraege ersetzen). */
    if (channel_open(pid_a, ep_a, ep_b, IFACE, VER, IPC_CHANNEL_DIR_BIDIRECTIONAL, &chan_id) != IPC_CHANNEL_ERR_DENIED) {
        return -1;
    }

    /* Nur A->B autorisiert: UNIDIRECTIONAL darf jetzt oeffnen, BIDIRECTIONAL noch nicht. */
    if (ipc_endpoint_grant_send(pid_b, ep_b, pid_a) != 0) return -1;
    if (channel_open(pid_a, ep_a, ep_b, IFACE, VER, IPC_CHANNEL_DIR_BIDIRECTIONAL, &chan_id) != IPC_CHANNEL_ERR_DENIED) {
        return -1;
    }
    if (channel_open(pid_a, ep_a, ep_b, IFACE, VER, IPC_CHANNEL_DIR_UNIDIRECTIONAL, &chan_id) != 0) {
        return -1;
    }

    ipc_channel_info_t info;
    if (channel_info(chan_id, &info) != 0) return -1;
    if (info.state != IPC_CHANNEL_STATE_ACTIVE) return -1;
    if (info.owner_a != pid_a || info.owner_b != pid_b) return -1;

    /* Interface-Mismatch muss abgewiesen werden (#9 Typed Channel). */
    ipc_message_t msg = {{0x11, 0x22, 0x33, 0x44}};
    if (channel_send(pid_a, chan_id, IFACE, VER + 1u, &msg) != IPC_CHANNEL_ERR_INCOMPATIBLE) return -1;

    /* UNIDIRECTIONAL: B darf nicht senden, Fremder sowieso nicht. */
    if (channel_send(pid_b, chan_id, IFACE, VER, &msg) != IPC_CHANNEL_ERR_DENIED) return -1;
    if (channel_send(pid_stranger, chan_id, IFACE, VER, &msg) != IPC_CHANNEL_ERR_DENIED) return -1;

    /* A -> B muss gelingen, B empfaengt es; A darf bei UNI nicht empfangen. */
    if (channel_send(pid_a, chan_id, IFACE, VER, &msg) != 0) return -1;
    if (channel_receive(pid_a, chan_id, IFACE, VER, &msg) != IPC_CHANNEL_ERR_DENIED) return -1;

    ipc_message_t received = {{0, 0, 0, 0}};
    if (channel_receive(pid_b, chan_id, IFACE, VER, &received) != 0) return -1;
    if (received.words[0] != 0x11 || received.words[3] != 0x44) return -1;

    if (channel_info(chan_id, &info) != 0) return -1;
    if (info.sent_total != 1 || info.received_total != 1 || info.denied_total < 3) return -1;

    /* Kontrolliertes Schliessen: close_finish vor Drain muss scheitern,
       nach Drain gelingen. */
    if (channel_send(pid_a, chan_id, IFACE, VER, &msg) != 0) return -1; /* 1 Nachricht wartet in ep_b */
    if (channel_close_request(pid_a, chan_id) != 0) return -1;
    if (channel_info(chan_id, &info) != 0) return -1;
    if (info.state != IPC_CHANNEL_STATE_CLOSING) return -1;

    /* Waehrend CLOSING darf nicht mehr gesendet, aber noch gedraint werden. */
    if (channel_send(pid_a, chan_id, IFACE, VER, &msg) != IPC_CHANNEL_ERR_INVALID_STATE) return -1;
    if (channel_close_finish(pid_a, chan_id) != IPC_CHANNEL_ERR_NOT_DRAINED) return -1;
    if (channel_receive(pid_b, chan_id, IFACE, VER, &received) != 0) return -1; /* drain */
    if (channel_close_finish(pid_a, chan_id) != 0) return -1;
    if (channel_close_finish(pid_b, chan_id) != 0) return -1; /* idempotent */

    if (channel_info(chan_id, &info) != 0) return -1;
    if (info.state != IPC_CHANNEL_STATE_CLOSED) return -1;
    if (channel_send(pid_a, chan_id, IFACE, VER, &msg) != IPC_CHANNEL_ERR_INVALID_STATE) return -1;

    /* Zweiter Channel fuer BIDIRECTIONAL + echten BROKEN-Test (Endpoint-Verlust). */
    uint32_t ep_a2, ep_b2;
    if (ipc_endpoint_create(pid_a, 4, &ep_a2) != 0) return -1;
    if (ipc_endpoint_create(pid_b, 4, &ep_b2) != 0) return -1;
    if (ipc_endpoint_grant_send(pid_b, ep_b2, pid_a) != 0) return -1;
    if (ipc_endpoint_grant_send(pid_a, ep_a2, pid_b) != 0) return -1;

    uint32_t chan2;
    if (channel_open(pid_a, ep_a2, ep_b2, IFACE, VER, IPC_CHANNEL_DIR_BIDIRECTIONAL, &chan2) != 0) return -1;

    /* Jetzt wirklich bidirektional: B -> A muss jetzt gelingen. */
    if (channel_send(pid_b, chan2, IFACE, VER, &msg) != 0) return -1;
    if (channel_receive(pid_a, chan2, IFACE, VER, &received) != 0) return -1;

    /* Endpoint-Verlust: B terminiert -> Channel muss lazy auf BROKEN kippen. */
    if (process_terminate(pid_b, 0) != 0) return -1;
    if (channel_send(pid_a, chan2, IFACE, VER, &msg) != IPC_CHANNEL_ERR_BROKEN) return -1;
    if (channel_info(chan2, &info) != 0) return -1;
    if (info.state != IPC_CHANNEL_STATE_BROKEN) return -1;

    /* Ein BROKEN Channel darf trotzdem kontrolliert geschlossen werden
       (keine unbegrenzte Wartezeit auf den ausgefallenen Peer). */
    if (channel_close_request(pid_a, chan2) != 0) return -1;
    if (channel_close_finish(pid_a, chan2) != 0) return -1; /* beide Queues leer */

    /* Unbekannte channel_id. */
    ipc_channel_info_t dummy;
    if (channel_info(0xFFFFu, &dummy) != IPC_CHANNEL_ERR_NOT_FOUND) return -1;

    /* Aufraeumen: Testprozesse wieder freigeben (kein dauerhafter Fussabdruck,
       analog zu process_manager_self_test()). pid_b ist bereits terminiert. */
    if (process_terminate(pid_a, 0) != 0) return -1;
    if (process_terminate(pid_stranger, 0) != 0) return -1;
    if (process_reap(pid_a) != 0) return -1;
    if (process_reap(pid_b) != 0) return -1;
    if (process_reap(pid_stranger) != 0) return -1;

    return 0;
}
