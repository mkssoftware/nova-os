/* NovaOS C-Kernel - IPC Channels (NPSPEC-IPC-CHANNEL-0001, Phase 3 des
   IPC-Neuentwurfs, AUFBAU AUF dem Phase-1-Endpoint-Modell aus ipc.h). */
#ifndef NOVA_CHANNEL_H
#define NOVA_CHANNEL_H

#include <stdint.h>
#include "ipc.h"

/* Ein Channel ist ein eigenstaendiges IPC-Objekt, das zwei bereits
   existierende Phase-1-Endpoints zu einer typisierten, zustandsbehafteten
   Verbindung buendelt (#1/#2 "kontrolliertes Kommunikationsobjekt, eindeutig
   identifizierbar"). Er erzeugt dabei KEINE neuen Rechte (#3/#10) - die
   Autorisierung MUSS bereits vorher ueber ipc_endpoint_grant_send() auf den
   beteiligten Endpoints bestehen; channel_open() prueft diese bestehende
   Autorisierung nur und verweigert sonst (#5 "erst nach erfolgreicher
   Autorisierung aktiv").

   Lifecycle: die Spec nennt Created->Connecting->Active->Closing->Closed.
   Da dieser Kernel keine entfernten Endpoints/Mehrknoten-Handshakes besitzt
   (siehe bereits bestehende Projekt-Entscheidung bei distcom.c/dc_*),
   gibt es keinen echten mehrstufigen Verbindungsaufbau - Created/Connecting
   kollabieren bewusst zu EINEM atomaren channel_open()-Aufruf (kein
   Scheinzustand ohne echtes Verhalten). ACTIVE/CLOSING/CLOSED/BROKEN sind
   dagegen echte, mit eigenem Verhalten unterscheidbare Zustaende:
   - CLOSING erfordert echtes Drain (#Schliessen "Drain/Cancel") - erst wenn
     beide beteiligten Endpoint-Warteschlangen leer sind, gelingt
     channel_close_finish().
   - BROKEN wird LAZY bei jedem Zugriff erkannt (gleiches Muster wie rpc.c's
     rpc_call_check_deadline()): sobald process_state() fuer einen der beiden
     Owner-PIDs nicht mehr RUNNING/SUSPENDED ist (#Endpoint-Verlust), kippt
     der Channel irreversibel nach BROKEN - "andere Teilnehmer duerfen nicht
     unbegrenzt auf den ausgefallenen Endpoint warten" ist damit erfuellt,
     ohne Polling/Timer: die Pruefung laeuft bei der naechsten Operation.

   Bewusst NICHT Teil dieser Phase: Shared-Memory-/Zero-Copy-Nutzlasten
   (NPSPEC-IPC-SHAREDMEMORY/-ZEROCOPY-0001 - dieser Kernel hat noch keine
   Shared-Memory-Capability, auf die ein Channel verweisen koennte), echte
   QoS/Priority/Bandwidth-Begrenzung (NEEDS-NEW-CODE, keine Resource-Economy-
   Anbindung fuer IPC vorhanden) - Queue-/Message-Size-Begrenzung (#6) ist
   ueber die bereits bestehenden Endpoint-/Message-Konstanten (IPC_ENDPOINT_
   QUEUE_CAPACITY/IPC_MESSAGE_SIZE) kostenlos mitgeerbt, kein neuer Code
   noetig. Automatisches channel_close() bei Prozess-Ende bleibt ebenso
   unverdrahtet wie bei Phase 1 (siehe dortige Begruendung). */

#define IPC_CHANNEL_CAPACITY 8u

#define IPC_CHANNEL_DIR_UNIDIRECTIONAL 1u
#define IPC_CHANNEL_DIR_BIDIRECTIONAL  2u

#define IPC_CHANNEL_STATE_CREATED   0u /* nur transient innerhalb channel_open(), nie persistiert */
#define IPC_CHANNEL_STATE_CONNECTING 0u /* siehe oben: kollabiert mit CREATED, kein eigener Wert */
#define IPC_CHANNEL_STATE_ACTIVE    1u
#define IPC_CHANNEL_STATE_CLOSING   2u
#define IPC_CHANNEL_STATE_CLOSED    3u
#define IPC_CHANNEL_STATE_BROKEN    4u

#define IPC_CHANNEL_ERR_NOT_FOUND     (-1) /* unbekannte channel_id, oder Endpoint-ID beim Open unbekannt */
#define IPC_CHANNEL_ERR_DENIED        (-2) /* Aufrufer nicht autorisiert (Owner-Identitaet oder fehlende Endpoint-Allowlist) */
#define IPC_CHANNEL_ERR_INVALID_STATE (-3) /* Operation in diesem Zustand nicht erlaubt (z.B. send auf CLOSING) */
#define IPC_CHANNEL_ERR_CAPACITY      (-4) /* Channel-Tabelle voll */
#define IPC_CHANNEL_ERR_BROKEN        (-5) /* Channel ist BROKEN (Endpoint-Verlust erkannt) */
#define IPC_CHANNEL_ERR_INCOMPATIBLE  (-6) /* interface_id/version passt nicht zum ausgehandelten Typ */
#define IPC_CHANNEL_ERR_NOT_DRAINED   (-7) /* close_finish vor leerer Warteschlange */
#define IPC_CHANNEL_ERR_FULL          (-8) /* Backpressure-Reject der Ziel-Endpoint-Warteschlange */

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
} ipc_channel_info_t;

void channel_manager_initialize(void);
int channel_manager_self_test(void); /* 0 = ok */

/* creator_pid MUSS Owner von endpoint_a sein. Bei BIDIRECTIONAL muessen
   BEIDE Richtungen bereits vorher per ipc_endpoint_grant_send() autorisiert
   sein (owner_a -> endpoint_b UND owner_b -> endpoint_a), bei
   UNIDIRECTIONAL nur owner_a -> endpoint_b. 0 = Erfolg (*out_channel_id
   gesetzt, Zustand ACTIVE), sonst IPC_CHANNEL_ERR_NOT_FOUND/_DENIED/
   _CAPACITY (kein persistenter Channel bei Fehler). */
int channel_open(uint32_t creator_pid, uint32_t endpoint_a, uint32_t endpoint_b,
                  uint32_t interface_id, uint32_t version, uint32_t direction,
                  uint32_t *out_channel_id);

/* sender_pid MUSS owner_a (immer erlaubt) oder owner_b (nur bei
   BIDIRECTIONAL) sein. interface_id/version MUESSEN zum Channel passen.
   0 = Erfolg, sonst IPC_CHANNEL_ERR_*. */
int channel_send(uint32_t sender_pid, uint32_t channel_id, uint32_t interface_id,
                  uint32_t version, const ipc_message_t *msg);

/* Empfang erlaubt waehrend ACTIVE und CLOSING (Drain). receiver_pid MUSS
   owner_b (Nachrichten von A) oder owner_a (nur bei BIDIRECTIONAL,
   Nachrichten von B) sein. 0 = Erfolg (*out_msg gefuellt), sonst
   IPC_CHANNEL_ERR_* (IPC_CHANNEL_ERR_FULL wiederverwendet als "aktuell
   nichts verfuegbar", analog zu ipc_endpoint_receive()). */
int channel_receive(uint32_t receiver_pid, uint32_t channel_id, uint32_t interface_id,
                     uint32_t version, ipc_message_t *out_msg);

/* pid MUSS owner_a oder owner_b sein. ACTIVE/BROKEN -> CLOSING (keine neuen
   Sends mehr). Idempotent (CLOSING/CLOSED bleiben unveraendert, 0). Schliesst
   NICHT die zugrunde liegenden Endpoints (die bleiben eigene IPC-Objekte,
   "Channel != Endpoint"). */
int channel_close_request(uint32_t pid, uint32_t channel_id);

/* pid MUSS owner_a oder owner_b sein. Nur aus CLOSING heraus: wechselt nach
   CLOSED, wenn BEIDE beteiligten Endpoint-Warteschlangen leer sind (echtes
   Drain), sonst IPC_CHANNEL_ERR_NOT_DRAINED (Aufrufer muss erst weiter
   channel_receive() aufrufen). CLOSED ist idempotent (0). */
int channel_close_finish(uint32_t pid, uint32_t channel_id);

/* Reine Metadaten, NIE Nachrichteninhalte (Introspection-Offenlegungsverbot,
   wie ipc_endpoint_info()). Aktualisiert die BROKEN-Erkennung lazy mit.
   0 = Erfolg, IPC_CHANNEL_ERR_NOT_FOUND sonst - funktioniert auch fuer
   bereits CLOSED/BROKEN Channels (letzter bekannter Zustand bleibt
   introspizierbar). */
int channel_info(uint32_t channel_id, ipc_channel_info_t *out_info);

#endif
