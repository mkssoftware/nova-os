/* NovaOS C-Kernel - Nova Remote Procedure Call (siehe rpc.h fuer Scope). */
#include "rpc.h"
#include "interrupt.h"

typedef struct {
    uint32_t operation_id;
    uint32_t idempotency;
} rpc_operation_t;

typedef struct {
    uint32_t interface_id;
    uint32_t owner_pid;
    uint32_t endpoint_id;
    uint32_t version;
    rpc_operation_t operations[RPC_OPERATIONS_PER_SERVICE];
    uint32_t operation_count;
    int in_use;
} rpc_service_t;

typedef struct {
    uint32_t call_id;
    uint32_t interface_id;
    uint32_t operation_id;
    uint32_t caller_pid;
    uint32_t state;
    int32_t error;
    ipc_message_t request;
    ipc_message_t response;
    uint32_t submit_ticks;
    uint32_t complete_ticks;
    uint32_t deadline_ticks;
    int in_use;
} rpc_call_t;

static rpc_service_t rpc_services[RPC_SERVICE_CAPACITY];
static rpc_call_t rpc_calls[RPC_CALL_CAPACITY];
static uint32_t rpc_next_call_id = 1;
static int rpc_mgr_ready = 0;

static uint32_t current_timer_ticks(void) {
    return timer_get_ticks();
}

void rpc_manager_initialize(void) {
    for (uint32_t i = 0; i < RPC_SERVICE_CAPACITY; ++i) rpc_services[i] = (rpc_service_t){0};
    for (uint32_t i = 0; i < RPC_CALL_CAPACITY; ++i) rpc_calls[i] = (rpc_call_t){0};
    rpc_next_call_id = 1;
    rpc_mgr_ready = 1;
}

static rpc_service_t *rpc_service_find(uint32_t interface_id) {
    for (uint32_t i = 0; i < RPC_SERVICE_CAPACITY; ++i) {
        if (rpc_services[i].in_use && rpc_services[i].interface_id == interface_id) {
            return &rpc_services[i];
        }
    }
    return 0;
}

static rpc_call_t *rpc_call_find(uint32_t call_id) {
    if (call_id == 0) return 0;
    for (uint32_t i = 0; i < RPC_CALL_CAPACITY; ++i) {
        if (rpc_calls[i].in_use && rpc_calls[i].call_id == call_id) {
            return &rpc_calls[i];
        }
    }
    return 0;
}

static int rpc_call_is_terminal(uint32_t state) {
    return state == RPC_CALL_STATE_COMPLETED || state == RPC_CALL_STATE_FAILED ||
           state == RPC_CALL_STATE_CANCELLED || state == RPC_CALL_STATE_TIMEOUT ||
           state == RPC_CALL_STATE_UNAVAILABLE;
}

/* Prueft bei jedem Beruehren eines nicht-terminalen Aufrufs, ob dessen
   deadline_ticks bereits ueberschritten ist (#7: Deadline-Miss ist ein
   EIGENER Endzustand, kein automatisches COMPLETED/FAILED). */
static void rpc_call_check_deadline(rpc_call_t *call) {
    if (rpc_call_is_terminal(call->state)) return;
    if (call->deadline_ticks == RPC_NO_DEADLINE) return;
    if (current_timer_ticks() >= call->deadline_ticks) {
        call->state = RPC_CALL_STATE_TIMEOUT;
        call->error = RPC_ERR_DEADLINE;
        call->complete_ticks = current_timer_ticks();
    }
}

int rpc_service_register(uint32_t owner_pid, uint32_t interface_id, uint32_t version,
                          uint32_t *out_endpoint_id) {
    if (rpc_service_find(interface_id) != 0) return -1;
    rpc_service_t *slot = 0;
    for (uint32_t i = 0; i < RPC_SERVICE_CAPACITY; ++i) {
        if (!rpc_services[i].in_use) {
            slot = &rpc_services[i];
            break;
        }
    }
    if (!slot) return -1;

    uint32_t endpoint_id = 0;
    if (ipc_endpoint_create(owner_pid, IPC_ENDPOINT_QUEUE_CAPACITY, &endpoint_id) != 0) {
        return -1;
    }

    *slot = (rpc_service_t){0};
    slot->interface_id = interface_id;
    slot->owner_pid = owner_pid;
    slot->endpoint_id = endpoint_id;
    slot->version = version;
    slot->in_use = 1;

    if (out_endpoint_id) *out_endpoint_id = endpoint_id;
    return 0;
}

int rpc_service_declare_operation(uint32_t owner_pid, uint32_t interface_id,
                                   uint32_t operation_id, uint32_t idempotency) {
    rpc_service_t *svc = rpc_service_find(interface_id);
    if (!svc || svc->owner_pid != owner_pid) return -1;
    for (uint32_t i = 0; i < svc->operation_count; ++i) {
        if (svc->operations[i].operation_id == operation_id) {
            svc->operations[i].idempotency = idempotency; /* Redeklaration erlaubt */
            return 0;
        }
    }
    if (svc->operation_count >= RPC_OPERATIONS_PER_SERVICE) return -1;
    svc->operations[svc->operation_count].operation_id = operation_id;
    svc->operations[svc->operation_count].idempotency = idempotency;
    ++svc->operation_count;
    return 0;
}

static uint32_t rpc_operation_idempotency(const rpc_service_t *svc, uint32_t operation_id) {
    for (uint32_t i = 0; i < svc->operation_count; ++i) {
        if (svc->operations[i].operation_id == operation_id) {
            return svc->operations[i].idempotency;
        }
    }
    return RPC_IDEMPOTENCY_UNKNOWN; /* #9: unbekannt -> wie nicht nachgewiesen behandeln */
}

static int rpc_call_submit_internal(uint32_t caller_pid, uint32_t interface_id,
                                     uint32_t operation_id, const ipc_message_t *request,
                                     uint32_t deadline_ticks, uint32_t *out_call_id) {
    rpc_service_t *svc = rpc_service_find(interface_id);
    if (!svc) return RPC_ERR_TRANSPORT;

    int auth = ipc_endpoint_check_sender(svc->endpoint_id, caller_pid);
    if (auth == IPC_ERR_CLOSED) return RPC_ERR_UNAVAILABLE;
    if (auth == IPC_ERR_NOT_FOUND) return RPC_ERR_UNAVAILABLE;
    if (auth == IPC_ERR_DENIED) return RPC_ERR_DENIED;

    rpc_call_t *slot = 0;
    for (uint32_t i = 0; i < RPC_CALL_CAPACITY; ++i) {
        if (!rpc_calls[i].in_use) {
            slot = &rpc_calls[i];
            break;
        }
    }
    if (!slot) return RPC_ERR_INVALID;

    *slot = (rpc_call_t){0};
    slot->call_id = rpc_next_call_id++;
    slot->interface_id = interface_id;
    slot->operation_id = operation_id;
    slot->caller_pid = caller_pid;
    slot->state = RPC_CALL_STATE_SUBMITTED;
    slot->error = RPC_ERR_NONE;
    if (request) slot->request = *request;
    slot->submit_ticks = current_timer_ticks();
    slot->deadline_ticks = deadline_ticks;
    slot->in_use = 1;

    if (out_call_id) *out_call_id = slot->call_id;
    return 0;
}

int rpc_call_submit(uint32_t caller_pid, uint32_t interface_id, uint32_t operation_id,
                     const ipc_message_t *request, uint32_t deadline_ticks,
                     uint32_t *out_call_id) {
    return rpc_call_submit_internal(caller_pid, interface_id, operation_id, request,
                                     deadline_ticks, out_call_id);
}

int rpc_call_next_pending(uint32_t owner_pid, uint32_t interface_id, uint32_t *out_call_id,
                           ipc_message_t *out_request) {
    rpc_service_t *svc = rpc_service_find(interface_id);
    if (!svc || svc->owner_pid != owner_pid) return RPC_ERR_INVALID;

    rpc_call_t *oldest = 0;
    for (uint32_t i = 0; i < RPC_CALL_CAPACITY; ++i) {
        rpc_call_t *call = &rpc_calls[i];
        if (!call->in_use || call->interface_id != interface_id) continue;
        rpc_call_check_deadline(call);
        if (call->state != RPC_CALL_STATE_SUBMITTED) continue;
        if (!oldest || call->call_id < oldest->call_id) oldest = call;
    }
    if (!oldest) return -1;

    oldest->state = RPC_CALL_STATE_EXECUTING;
    if (out_call_id) *out_call_id = oldest->call_id;
    if (out_request) *out_request = oldest->request;
    return 0;
}

int rpc_call_complete(uint32_t owner_pid, uint32_t call_id, const ipc_message_t *response) {
    rpc_call_t *call = rpc_call_find(call_id);
    if (!call) return RPC_ERR_INVALID;
    rpc_service_t *svc = rpc_service_find(call->interface_id);
    if (!svc || svc->owner_pid != owner_pid) return RPC_ERR_INVALID;
    if (call->state != RPC_CALL_STATE_EXECUTING) return RPC_ERR_INVALID;

    if (response) call->response = *response;
    call->state = RPC_CALL_STATE_COMPLETED;
    call->error = RPC_ERR_NONE;
    call->complete_ticks = current_timer_ticks();
    return 0;
}

int rpc_call_fail(uint32_t owner_pid, uint32_t call_id, int32_t remote_error) {
    (void)remote_error; /* dienstspezifischer Code, siehe rpc.h-Kommentar */
    rpc_call_t *call = rpc_call_find(call_id);
    if (!call) return RPC_ERR_INVALID;
    rpc_service_t *svc = rpc_service_find(call->interface_id);
    if (!svc || svc->owner_pid != owner_pid) return RPC_ERR_INVALID;
    if (call->state != RPC_CALL_STATE_EXECUTING) return RPC_ERR_INVALID;

    call->state = RPC_CALL_STATE_FAILED;
    call->error = RPC_ERR_REMOTE;
    call->complete_ticks = current_timer_ticks();
    return 0;
}

static int rpc_call_state_to_error(uint32_t state) {
    switch (state) {
        case RPC_CALL_STATE_FAILED:      return RPC_ERR_REMOTE;
        case RPC_CALL_STATE_CANCELLED:   return RPC_ERR_CANCELLED;
        case RPC_CALL_STATE_TIMEOUT:     return RPC_ERR_DEADLINE;
        case RPC_CALL_STATE_UNAVAILABLE: return RPC_ERR_UNAVAILABLE;
        default: return RPC_ERR_TRANSPORT;
    }
}

int rpc_call_poll(uint32_t caller_pid, uint32_t call_id, ipc_message_t *out_response) {
    rpc_call_t *call = rpc_call_find(call_id);
    if (!call || call->caller_pid != caller_pid) return -1;

    rpc_call_check_deadline(call);

    if (call->state == RPC_CALL_STATE_COMPLETED) {
        if (out_response) *out_response = call->response;
        return 0;
    }
    if (rpc_call_is_terminal(call->state)) {
        return rpc_call_state_to_error(call->state);
    }
    return RPC_PENDING;
}

int rpc_call_cancel(uint32_t caller_pid, uint32_t call_id) {
    rpc_call_t *call = rpc_call_find(call_id);
    if (!call || call->caller_pid != caller_pid) return -1;
    if (rpc_call_is_terminal(call->state)) return -1; /* bereits in einem Endzustand */
    call->state = RPC_CALL_STATE_CANCELLED;
    call->error = RPC_ERR_CANCELLED;
    call->complete_ticks = current_timer_ticks();
    return 0;
}

int rpc_call_sync(uint32_t caller_pid, uint32_t interface_id, uint32_t operation_id,
                   const ipc_message_t *request, uint32_t timeout_ticks,
                   ipc_message_t *out_response) {
    uint32_t call_id = 0;
    int rc = rpc_call_submit_internal(caller_pid, interface_id, operation_id, request,
                                       RPC_NO_DEADLINE, &call_id);
    if (rc != 0) return rc;

    uint32_t wait_deadline = current_timer_ticks() + timeout_ticks;
    for (;;) {
        rc = rpc_call_poll(caller_pid, call_id, out_response);
        if (rc != RPC_PENDING) return rc;
        if (timeout_ticks != 0 && current_timer_ticks() >= wait_deadline) {
            rpc_call_t *call = rpc_call_find(call_id);
            if (call && !rpc_call_is_terminal(call->state)) {
                call->state = RPC_CALL_STATE_TIMEOUT;
                call->error = RPC_ERR_TIMEOUT;
                call->complete_ticks = current_timer_ticks();
            }
            return RPC_ERR_TIMEOUT;
        }
    }
}

int rpc_call_retry(uint32_t caller_pid, uint32_t call_id, uint32_t *out_new_call_id) {
    rpc_call_t *call = rpc_call_find(call_id);
    if (!call || call->caller_pid != caller_pid) return RPC_ERR_INVALID;

    rpc_service_t *svc = rpc_service_find(call->interface_id);
    if (!svc) return RPC_ERR_TRANSPORT;
    if (rpc_operation_idempotency(svc, call->operation_id) != RPC_IDEMPOTENCY_IDEMPOTENT) {
        return RPC_ERR_PROTOCOL; /* #9: NON_IDEMPOTENT und UNKNOWN beide verweigert */
    }

    return rpc_call_submit_internal(caller_pid, call->interface_id, call->operation_id,
                                     &call->request, call->deadline_ticks, out_new_call_id);
}

int rpc_call_info(uint32_t call_id, rpc_call_info_t *out_info) {
    rpc_call_t *call = rpc_call_find(call_id);
    if (!call) return -1;
    rpc_call_check_deadline(call);
    if (out_info) {
        out_info->call_id = call->call_id;
        out_info->interface_id = call->interface_id;
        out_info->operation_id = call->operation_id;
        out_info->caller_pid = call->caller_pid;
        out_info->state = call->state;
        out_info->error = call->error;
        out_info->submit_ticks = call->submit_ticks;
        out_info->complete_ticks = call->complete_ticks;
        out_info->deadline_ticks = call->deadline_ticks;
    }
    return 0;
}

int rpc_manager_self_test(void) {
    if (rpc_mgr_ready != 1) return -1;

    const uint32_t SERVICE_PID = 0xA001u;
    const uint32_t CLIENT_PID = 0xA002u;
    const uint32_t STRANGER_PID = 0xA003u;
    const uint32_t IFACE_ECHO = 0x100u;
    const uint32_t OP_ECHO_IDEMPOTENT = 1u;
    const uint32_t OP_ECHO_NON_IDEMPOTENT = 2u;

    uint32_t endpoint_id = 0;
    if (rpc_service_register(SERVICE_PID, IFACE_ECHO, 1u, &endpoint_id) != 0) { return -1; }
    if (rpc_service_register(SERVICE_PID, IFACE_ECHO, 1u, &endpoint_id) == 0) { return -1; } /* Duplikat */

    if (rpc_service_declare_operation(SERVICE_PID, IFACE_ECHO, OP_ECHO_IDEMPOTENT,
                                       RPC_IDEMPOTENCY_IDEMPOTENT) != 0) {
        return -1;
    }
    if (rpc_service_declare_operation(SERVICE_PID, IFACE_ECHO, OP_ECHO_NON_IDEMPOTENT,
                                       RPC_IDEMPOTENCY_NON_IDEMPOTENT) != 0) {
        return -1;
    }

    ipc_message_t req = {{0x1234u, 0x5678u, 0, 0}};

    /* #5/#6: unautorisierter Aufrufer wird abgelehnt - Kenntnis von
       IFACE_ECHO allein genuegt nicht. */
    uint32_t stray_call = 0;
    if (rpc_call_submit(STRANGER_PID, IFACE_ECHO, OP_ECHO_IDEMPOTENT, &req, RPC_NO_DEADLINE,
                         &stray_call) != RPC_ERR_DENIED) {
        return -1;
    }

    /* Unbekanntes Interface -> Transportfehler, nicht Denied. */
    uint32_t bad_call = 0;
    if (rpc_call_submit(SERVICE_PID, 0xFFFFu, 1u, &req, RPC_NO_DEADLINE, &bad_call) != RPC_ERR_TRANSPORT) {
        return -1;
    }

    if (ipc_endpoint_grant_send(SERVICE_PID, endpoint_id, CLIENT_PID) != 0) { return -1; }

    uint32_t call_id = 0;
    if (rpc_call_submit(CLIENT_PID, IFACE_ECHO, OP_ECHO_IDEMPOTENT, &req, RPC_NO_DEADLINE, &call_id) != 0) {
        return -1;
    }

    rpc_call_info_t info;
    if (rpc_call_info(call_id, &info) != 0) { return -1; }
    if (info.state != RPC_CALL_STATE_SUBMITTED || info.caller_pid != CLIENT_PID) { return -1; }

    /* Noch nichts abgeholt -> PENDING, kein Endzustand. */
    ipc_message_t resp = {{0}};
    if (rpc_call_poll(CLIENT_PID, call_id, &resp) != RPC_PENDING) { return -1; }
    if (rpc_call_poll(STRANGER_PID, call_id, &resp) != -1) { return -1; } /* falscher Aufrufer */

    /* Dienstseite: abholen, bearbeiten, beantworten. */
    uint32_t picked_call = 0;
    ipc_message_t picked_req = {{0}};
    if (rpc_call_next_pending(STRANGER_PID, IFACE_ECHO, &picked_call, &picked_req) !=
        RPC_ERR_INVALID) {
        return -1; /* falscher Owner */
    }
    if (rpc_call_next_pending(SERVICE_PID, IFACE_ECHO, &picked_call, &picked_req) != 0) { return -1; }
    if (picked_call != call_id) { return -1; }
    if (picked_req.words[0] != 0x1234u) { return -1; }

    ipc_message_t response = {{0xAAAAu, 0xBBBBu, 0, 0}};
    if (rpc_call_complete(STRANGER_PID, call_id, &response) != RPC_ERR_INVALID) { return -1; } /* falscher Owner */
    if (rpc_call_complete(SERVICE_PID, call_id, &response) != 0) { return -1; }

    if (rpc_call_poll(CLIENT_PID, call_id, &resp) != 0) { return -1; }
    if (resp.words[0] != 0xAAAAu) { return -1; }

    /* Ein bereits abgeschlossener Aufruf bleibt introspizierbar. */
    if (rpc_call_info(call_id, &info) != 0) { return -1; }
    if (info.state != RPC_CALL_STATE_COMPLETED || info.error != RPC_ERR_NONE) { return -1; }

    /* #9: Wiederholung einer idempotenten Operation ist erlaubt. */
    uint32_t retry_call = 0;
    if (rpc_call_retry(CLIENT_PID, call_id, &retry_call) != 0) { return -1; }
    if (retry_call == call_id) { return -1; } /* echter neuer Aufruf */

    /* rpc_call_retry() hat gerade einen NEUEN, noch SUBMITTED-Aufruf
       erzeugt - der muss abgeholt+abgeschlossen werden, sonst wuerde er
       vom naechsten rpc_call_next_pending()-Aufruf unten VOR dem dort
       eigentlich erwarteten fail_call abgeholt (next_pending liefert
       immer den AELTESTEN wartenden Aufruf fuer das Interface). */
    uint32_t retry_picked = 0;
    ipc_message_t retry_picked_req = {{0}};
    if (rpc_call_next_pending(SERVICE_PID, IFACE_ECHO, &retry_picked, &retry_picked_req) != 0) {
        return -1;
    }
    if (retry_picked != retry_call) { return -1; }
    if (rpc_call_complete(SERVICE_PID, retry_call, &response) != 0) { return -1; }

    /* Fachlicher Fehlerpfad (#10: RPC_ERR_REMOTE, unterscheidbar von
       Transport-/Autorisierungsfehlern). */
    uint32_t fail_call = 0;
    if (rpc_call_submit(CLIENT_PID, IFACE_ECHO, OP_ECHO_NON_IDEMPOTENT, &req, RPC_NO_DEADLINE,
                         &fail_call) != 0) {
        return -1;
    }
    if (rpc_call_next_pending(SERVICE_PID, IFACE_ECHO, &picked_call, &picked_req) != 0) { return -1; }
    if (rpc_call_fail(SERVICE_PID, fail_call, 0x42u) != 0) { return -1; }
    if (rpc_call_poll(CLIENT_PID, fail_call, &resp) != RPC_ERR_REMOTE) { return -1; }

    /* #9: eine NICHT nachgewiesen idempotente Operation darf NICHT
       automatisch wiederholt werden. */
    uint32_t retry_fail = 0;
    if (rpc_call_retry(CLIENT_PID, fail_call, &retry_fail) != RPC_ERR_PROTOCOL) { return -1; }

    /* #7: Cancellation ist eine Anfrage, eigener Endzustand. */
    uint32_t cancel_call = 0;
    if (rpc_call_submit(CLIENT_PID, IFACE_ECHO, OP_ECHO_IDEMPOTENT, &req, RPC_NO_DEADLINE,
                         &cancel_call) != 0) {
        return -1;
    }
    if (rpc_call_cancel(CLIENT_PID, cancel_call) != 0) { return -1; }
    if (rpc_call_poll(CLIENT_PID, cancel_call, &resp) != RPC_ERR_CANCELLED) { return -1; }
    if (rpc_call_cancel(CLIENT_PID, cancel_call) == 0) { return -1; } /* bereits terminal */

    /* #7/#10: deadline_ticks deterministisch "bereits abgelaufen" machen,
       OHNE auf einen bestimmten absoluten Tick-Stand an dieser Boot-
       Stelle zu vertrauen (siehe execution.c's Lektion zu genau dieser
       Falle - current_timer_ticks() ist hier frueh im Boot ungewiss klein,
       NICHT garantiert bereits > 1). Stattdessen: deadline_ticks = der
       JETZT aktuelle Tick-Stand. Da Ticks monoton nicht fallend sind, ist
       jeder SPAETERE Lesevorgang (>= statt >) garantiert >= diesem
       Schnappschuss - die Deadline gilt damit ab dem naechsten Poll immer
       als ueberschritten, unabhaengig vom tatsaechlichen absoluten Wert. */
    uint32_t deadline_snapshot = current_timer_ticks();
    uint32_t deadline_call = 0;
    if (rpc_call_submit(CLIENT_PID, IFACE_ECHO, OP_ECHO_IDEMPOTENT, &req, deadline_snapshot,
                         &deadline_call) != 0) {
        return -1;
    }
    if (rpc_call_poll(CLIENT_PID, deadline_call, &resp) != RPC_ERR_DEADLINE) { return -1; }

    if (rpc_call_info(0xFFFFu, &info) != -1) { return -1; } /* unbekannte call_id */

    return 0;
}
