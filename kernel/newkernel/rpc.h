/* NovaOS C-Kernel - Nova Remote Procedure Call (NPSPEC-IPC-RPC-0001),
   IPC-Phase 2 - typisiertes Request/Response AUF dem Endpoint-Modell aus
   ipc.h (Phase 1) aufgebaut, nicht als Ersatz dafuer. Ein RPC-Service
   bekommt bei der Registrierung einen eigenen ipc.c-Endpoint - dessen
   bestehende Owner-/Allowlist-Pruefung (ipc_endpoint_check_sender)
   gatekeeped JEDEN rpc_call_submit() (NPSPEC-IPC-RPC-0001 #5/#6: "Service-
   oder Endpoint-Kenntnis DARF NICHT automatisch Aufrufrechte gewaehren") -
   RPC erfindet dafuer bewusst KEIN eigenes, paralleles Capability-Modell.

   Der eigentliche typisierte Request/Response-Zustand (CallID/Interface/
   Operation/State/Deadline/Fehlercode) lebt NICHT in der rohen 16-Byte-
   IPC-Warteschlange (dafuer zu reichhaltig), sondern in einer eigenen
   Call-Tabelle - der Dienst holt wartende Aufrufe per rpc_call_next_
   pending() ab (Polling, kein Blockieren - konsistent mit dem Rest dieses
   Kernels ohne echte Scheduler-Warteschlangen fuer I/O).

   Bewusst NICHT Teil dieser Phase: echte entfernte (Multi-Node-)Services
   (Location Transparency bleibt BLOCKED wie ueberall sonst im Projekt -
   kein Transport), Zero-Copy/Shared-Memory-Payloads (Payload bleibt die
   16-Byte ipc_message_t, wie in Phase 1), automatische Wiederholung
   (rpc_call_retry() erlaubt sie nur EXPLIZIT und nur fuer als idempotent
   deklarierte Operationen - siehe #9). */
#ifndef NOVA_RPC_H
#define NOVA_RPC_H

#include <stdint.h>
#include "ipc.h"

#define RPC_SERVICE_CAPACITY         8u
#define RPC_CALL_CAPACITY            8u
#define RPC_OPERATIONS_PER_SERVICE   4u

#define RPC_IDEMPOTENCY_UNKNOWN        0u
#define RPC_IDEMPOTENCY_IDEMPOTENT     1u
#define RPC_IDEMPOTENCY_NON_IDEMPOTENT 2u

/* NPSPEC-IPC-RPC-0001 Lifecycle, reduziert um Pending (fehlt kein
   eigener Scheduler-Zwischenschritt - ein Call ist entweder SUBMITTED
   (wartet auf Abholung) oder bereits EXECUTING (abgeholt, Dienst
   arbeitet), beides ohne echte Nebenlaeufigkeit innerhalb dieses
   Kernels). */
#define RPC_CALL_STATE_CREATED     0u
#define RPC_CALL_STATE_SUBMITTED   1u
#define RPC_CALL_STATE_EXECUTING   2u
#define RPC_CALL_STATE_COMPLETED   3u
#define RPC_CALL_STATE_FAILED      4u
#define RPC_CALL_STATE_CANCELLED   5u
#define RPC_CALL_STATE_TIMEOUT     6u
#define RPC_CALL_STATE_UNAVAILABLE 7u

/* #10: Transport-/Protokollfehler (lokal erkannt, VOR Ausfuehrung) und
   fachliche Fehler (vom Dienst selbst gemeldet, per rpc_call_fail())
   bleiben unterscheidbar - RPC_ERR_REMOTE ist IMMER ein Dienstfehler,
   alle anderen sind Transport-/Lokalfehler. */
#define RPC_ERR_NONE        0
#define RPC_ERR_TRANSPORT   (-1) /* Interface/Operation unbekannt oder ungueltiger Aufruf */
#define RPC_ERR_UNAVAILABLE (-2) /* Service-Endpoint existiert nicht mehr/ist geschlossen */
#define RPC_ERR_DENIED      (-3) /* Aufrufer nicht autorisiert (ipc.c-Endpoint-Allowlist) */
#define RPC_ERR_INVALID     (-4) /* unbekannte call_id, falscher Aufrufer/Owner, falscher Zustand */
#define RPC_ERR_REMOTE      (-5) /* Dienst hat den Aufruf per rpc_call_fail() aktiv abgelehnt */
#define RPC_ERR_TIMEOUT     (-6) /* lokale Wartezeit abgelaufen (Timeout != Deadline, siehe RPC-0001) */
#define RPC_ERR_CANCELLED   (-7)
#define RPC_ERR_DEADLINE    (-8) /* deadline_ticks ueberschritten */
#define RPC_ERR_PROTOCOL    (-9) /* z.B. Wiederholung einer nicht-idempotenten Operation */

/* Positiver Rueckgabewert von rpc_call_poll()/rpc_call_sync(): der Aufruf
   ist noch nicht in einem Endzustand - bewusst von 0 (Erfolg) und
   negativen RPC_ERR_*-Werten (Fehler) unterschieden. */
#define RPC_PENDING 1

/* Sentinel fuer "keine Deadline" - bewusst NICHT 0: Tick 0 ist im ersten
   Boot-Augenblick ein echter, gueltiger Tick-Stand, 0 als Sentinel haette
   genau dort eine Deadline von 0 ungewollt als "keine Deadline"
   fehlinterpretiert (dieselbe Mehrdeutigkeits-Falle, die bei execution.c
   bereits einmal real aufgetreten ist - hier bewusst von vornherein
   vermieden statt nachtraeglich gepatcht). */
#define RPC_NO_DEADLINE 0xFFFFFFFFu

typedef struct {
    uint32_t call_id;
    uint32_t interface_id;
    uint32_t operation_id;
    uint32_t caller_pid;
    uint32_t state;        /* RPC_CALL_STATE_* */
    int32_t  error;        /* RPC_ERR_NONE waehrend/nach Erfolg, sonst RPC_ERR_* */
    uint32_t submit_ticks;
    uint32_t complete_ticks; /* 0 solange nicht in einem Endzustand */
    uint32_t deadline_ticks; /* 0 = keine Deadline */
} rpc_call_info_t;

void rpc_manager_initialize(void);
int rpc_manager_self_test(void); /* 0 = ok */

/* Registriert ein neues Interface samt eigenem ipc.c-Endpoint (Owner =
   owner_pid). 0 = Erfolg (*out_endpoint_id gesetzt, fuer
   ipc_endpoint_grant_send()), -1 wenn Tabelle voll oder interface_id
   bereits vergeben. */
int rpc_service_register(uint32_t owner_pid, uint32_t interface_id, uint32_t version,
                          uint32_t *out_endpoint_id);

/* #8: Operationen deklarieren ihre Idempotenz VOR dem ersten Aufruf -
   unbekannte operation_ids bei rpc_call_submit() werden implizit als
   RPC_IDEMPOTENCY_UNKNOWN behandelt (#9: "Unknown MUSS wie nicht
   nachgewiesene Idempotenz behandelt werden"). 0 = Erfolg, -1 wenn
   interface_id unbekannt, owner_pid nicht der Owner ist, oder die
   Operationstabelle des Interfaces voll ist. */
int rpc_service_declare_operation(uint32_t owner_pid, uint32_t interface_id,
                                   uint32_t operation_id, uint32_t idempotency);

/* #1/#2/#3: asynchrones Submit - erzeugt eine eindeutige call_id, prueft
   per ipc_endpoint_check_sender() die Aufrufberechtigung (#5/#6). 0 =
   Erfolg (*out_call_id gesetzt, Zustand SUBMITTED), RPC_ERR_TRANSPORT
   (unbekanntes Interface), RPC_ERR_UNAVAILABLE (Endpoint geschlossen),
   RPC_ERR_DENIED (keine Berechtigung) oder RPC_ERR_INVALID (Tabelle
   voll). deadline_ticks: RPC_NO_DEADLINE = keine Deadline. */
int rpc_call_submit(uint32_t caller_pid, uint32_t interface_id, uint32_t operation_id,
                     const ipc_message_t *request, uint32_t deadline_ticks,
                     uint32_t *out_call_id);

/* Dienstseite: holt den AELTESTEN wartenden (SUBMITTED) Aufruf fuer eines
   der eigenen Interfaces ab und setzt ihn auf EXECUTING. 0 = Erfolg
   (*out_call_id / *out_request gefuellt), RPC_ERR_INVALID wenn owner_pid
   nicht Owner von interface_id ist, -1 wenn nichts wartet (kein RPC_ERR_*,
   das ist kein Fehlerzustand). */
int rpc_call_next_pending(uint32_t owner_pid, uint32_t interface_id, uint32_t *out_call_id,
                           ipc_message_t *out_request);

/* Dienstseite: schliesst einen EXECUTING-Aufruf erfolgreich ab. 0 =
   Erfolg, RPC_ERR_INVALID wenn call_id unbekannt/falscher Owner/nicht
   EXECUTING. */
int rpc_call_complete(uint32_t owner_pid, uint32_t call_id, const ipc_message_t *response);

/* Dienstseite: schliesst einen EXECUTING-Aufruf mit einem FACHLICHEN
   Fehler ab (#10: das ist IMMER RPC_ERR_REMOTE aus Aufrufersicht,
   remote_error ist der dienstspezifische Code, in rpc_call_info() NICHT
   gesondert sichtbar - nur ueber den urspruenglich zurueckgegebenen
   ipc_message_t-Fehlerkanal der jeweiligen Anwendung, falls gewuenscht).
   0 = Erfolg, RPC_ERR_INVALID sonst. */
int rpc_call_fail(uint32_t owner_pid, uint32_t call_id, int32_t remote_error);

/* Clientseite: 0 = COMPLETED (*out_response gefuellt), RPC_PENDING = noch
   kein Endzustand, sonst der jeweilige RPC_ERR_*-Endzustand (TIMEOUT/
   CANCELLED/DEADLINE/UNAVAILABLE/REMOTE/...). Prueft bei jedem Aufruf
   zusaetzlich deadline_ticks (siehe #7: Timeout/Deadline/tatsaechlicher
   Zustand bleiben getrennt - ein Deadline-Miss setzt NICHT automatisch
   COMPLETED/FAILED, sondern einen eigenen Endzustand). -1 wenn call_id
   unbekannt oder caller_pid nicht der urspruengliche Aufrufer ist. */
int rpc_call_poll(uint32_t caller_pid, uint32_t call_id, ipc_message_t *out_response);

/* #7: Cancellation ist eine ANFRAGE, kein garantiertes Ergebnis - setzt
   einen NICHT bereits terminalen Aufruf auf CANCELLED. 0 = Erfolg, -1
   wenn call_id unbekannt, caller_pid nicht der Aufrufer ist, oder der
   Aufruf bereits in einem Endzustand war (der tatsaechliche Endzustand
   bleibt dann der vorherige, siehe rpc_call_info()). */
int rpc_call_cancel(uint32_t caller_pid, uint32_t call_id);

/* #4: synchrone Bequemlichkeits-API AUF dem asynchronen Grundmodell -
   submit + aktives Warten (timer_get_ticks()-basiert) bis Endzustand
   ODER timeout_ticks lokale Wartezeit ueberschritten ist (RPC_ERR_TIMEOUT,
   UNABHAENGIG von deadline_ticks - siehe #7-Unterscheidung). 0 = Erfolg
   (*out_response gefuellt), sonst der jeweilige RPC_ERR_*-Code. */
int rpc_call_sync(uint32_t caller_pid, uint32_t interface_id, uint32_t operation_id,
                   const ipc_message_t *request, uint32_t timeout_ticks,
                   ipc_message_t *out_response);

/* #9: Wiederholung NUR fuer als RPC_IDEMPOTENCY_IDEMPOTENT deklarierte
   Operationen - erzeugt einen NEUEN Aufruf mit denselben Parametern wie
   der urspruengliche. 0 = Erfolg (*out_new_call_id gesetzt),
   RPC_ERR_PROTOCOL wenn die Operation NICHT nachweislich idempotent war
   (NON_IDEMPOTENT oder UNKNOWN), RPC_ERR_INVALID wenn call_id unbekannt
   oder caller_pid nicht der urspruengliche Aufrufer ist. */
int rpc_call_retry(uint32_t caller_pid, uint32_t call_id, uint32_t *out_new_call_id);

/* #12: reine Metadaten, KEIN Zugriff auf Request-/Response-Payload ohne
   die jeweilige Berechtigung (Owner/Caller - diese Funktion selbst
   erzwingt das bewusst NICHT, da sie fuer Diagnose/Introspektion gedacht
   ist, die per Definition keine Payload enthaelt). 0 = Erfolg (*out_info
   gefuellt), -1 wenn call_id unbekannt. */
int rpc_call_info(uint32_t call_id, rpc_call_info_t *out_info);

#endif
