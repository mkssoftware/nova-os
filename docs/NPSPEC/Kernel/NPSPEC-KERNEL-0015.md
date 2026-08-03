# NPSPEC-KERNEL-0015 – IPC Framework

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0015 |
| Titel | IPC Framework |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Interprozesskommunikation |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0003, NPSPEC-KERNEL-0004, NPSPEC-KERNEL-0005, NPSPEC-KERNEL-0011, NPSPEC-KERNEL-0012, NPSPEC-KERNEL-0013, NPSPEC-KERNEL-0014 |
| Zugehörige ADRs | ADR-IPC-0001, ADR-IPC-0002, ADR-IPC-0003, ADR-IPC-0004, ADR-IPC-0005, ADR-IPC-0006, ADR-IPC-0007, ADR-IPC-0008 |

---

## 1. Zweck

Diese Spezifikation definiert das Interprocess Communication Framework von NovaOS.

Das IPC Framework stellt einheitliche Mechanismen für die Kommunikation zwischen:

- Prozessen,
- Threads,
- Kernelkomponenten,
- Systemdiensten,
- Userspace-Treibern,
- isolierten Sicherheitsdomains

bereit.

## 2. Ziele

Das IPC Framework muss:

- capability-geschützte Kommunikation ermöglichen,
- synchrone und asynchrone Operationen unterstützen,
- Nachrichten und Handles sicher übertragen,
- Shared Memory für große Datenmengen bereitstellen,
- Prozesse und Treiber voneinander isolieren,
- Timeouts und Abbruch unterstützen,
- Prioritätsinversion begrenzen,
- lokale Diagnose ermöglichen,
- ohne globale zentrale IPC-Sperre skalieren.

## 3. IPC-Mechanismen

NovaOS stellt folgende Mechanismen bereit:

| Mechanismus | Verwendung |
|---|---|
| Message Channel | strukturierte Nachrichten |
| Shared Memory | große gemeinsam genutzte Datenbereiche |
| Event | leichte Signalisierung |
| Signal | asynchrone Prozess- oder Threadbenachrichtigung |
| RPC | Anfrage-Antwort-Kommunikation |
| Pipe | sequenzieller Bytestrom |
| Local Socket | verbindungsorientierte lokale Kommunikation |
| Named Object | Auffinden veröffentlichter IPC-Endpunkte |

Die Mechanismen verwenden das gemeinsame Objekt-, Handle- und Capability-Modell.

## 4. Grundmodell

IPC-Kommunikation erfolgt über Kernelobjekte und Endpunkte.

```text
Sender
  -> lokales Handle
  -> IPC-Endpunkt
  -> Kernelvalidierung
  -> Ziel-Endpunkt
  -> Empfänger
```

Die Kenntnis eines Endpunktnamens oder einer numerischen Kennung verleiht keine Kommunikationsrechte.

## 5. IPC-Objekttypen

```c
typedef enum np_ipc_object_type {
    NP_IPC_CHANNEL,
    NP_IPC_ENDPOINT,
    NP_IPC_MESSAGE_QUEUE,
    NP_IPC_SHARED_MEMORY,
    NP_IPC_EVENT,
    NP_IPC_PIPE,
    NP_IPC_RPC_ENDPOINT,
    NP_IPC_LOCAL_SOCKET
} np_ipc_object_type_t;
```

Jedes IPC-Objekt wird durch den Object Manager verwaltet.

## 6. IPC-Endpunkt

Ein Endpunkt stellt eine gerichtete oder bidirektionale Kommunikationsschnittstelle dar.

```c
typedef struct np_ipc_endpoint {
    np_kernel_object_t object;

    np_message_queue_t receive_queue;
    np_ipc_endpoint_t* peer;

    np_capability_domain_t* owner_domain;
    np_wait_queue_t waiters;

    uint32_t state;
    uint32_t flags;
} np_ipc_endpoint_t;
```

Interne Strukturen sind nicht Bestandteil der öffentlichen ABI.

## 7. Endpunktzustände

```c
typedef enum np_ipc_endpoint_state {
    NP_IPC_ENDPOINT_CREATED,
    NP_IPC_ENDPOINT_CONNECTED,
    NP_IPC_ENDPOINT_CLOSING,
    NP_IPC_ENDPOINT_CLOSED,
    NP_IPC_ENDPOINT_PEER_CLOSED
} np_ipc_endpoint_state_t;
```

Ein geschlossener Endpunkt darf keine neuen Nachrichten mehr annehmen.

Bereits zugestellte Nachrichten können abhängig von der Schließrichtlinie noch gelesen werden.

## 8. Channel-Erzeugung

Ein Channel erzeugt zwei verbundene Endpunkte.

```c
np_status_t np_ipc_channel_create(
    uint32_t flags,
    np_handle_t* endpoint_a,
    np_handle_t* endpoint_b
);
```

Standardmäßig sind Channels bidirektional.

Die beiden Handles können unterschiedliche Rechte besitzen und an verschiedene Prozesse übertragen werden.

## 9. Channel-Flags

```c
typedef enum np_ipc_channel_flags {
    NP_IPC_CHANNEL_BIDIRECTIONAL = 1U << 0,
    NP_IPC_CHANNEL_ORDERED       = 1U << 1,
    NP_IPC_CHANNEL_RELIABLE      = 1U << 2,
    NP_IPC_CHANNEL_NO_HANDLE_TRANSFER = 1U << 3,
    NP_IPC_CHANNEL_AUDITED       = 1U << 4
} np_ipc_channel_flags_t;
```

Lokale Kernelchannels sind standardmäßig geordnet und zuverlässig, solange beide Endpunkte existieren.

## 10. Nachrichtenmodell

Eine IPC-Nachricht besteht aus:

- festem Header,
- optionalen Inline-Daten,
- optionalen Out-of-Line-Daten,
- optionalen Handles,
- optionalen Shared-Memory-Referenzen,
- Metadaten.

```c
typedef struct np_ipc_message_header {
    uint32_t structure_size;
    uint16_t protocol_version;
    uint16_t flags;

    uint64_t message_id;
    uint64_t correlation_id;

    uint32_t payload_size;
    uint16_t handle_count;
    uint16_t descriptor_count;
} np_ipc_message_header_t;
```

## 11. Nachrichtengrößen

Empfohlene Grenzwerte sind:

| Bestandteil | Standardgrenze |
|---|---:|
| Inline-Daten | 4 KiB |
| Handles pro Nachricht | 16 |
| Deskriptoren | 32 |
| Gesamtgröße Kernelkopie | 64 KiB |

Größere Datenmengen sollen über Shared Memory übertragen werden.

Konkrete Grenzen müssen abfragbar und pro Sicherheitsprofil begrenzbar sein.

## 12. Nachrichtenflags

```c
typedef enum np_ipc_message_flags {
    NP_IPC_MESSAGE_REQUEST      = 1U << 0,
    NP_IPC_MESSAGE_RESPONSE     = 1U << 1,
    NP_IPC_MESSAGE_ONE_WAY      = 1U << 2,
    NP_IPC_MESSAGE_HIGH_PRIORITY = 1U << 3,
    NP_IPC_MESSAGE_HAS_HANDLES  = 1U << 4,
    NP_IPC_MESSAGE_HAS_SHARED_MEMORY = 1U << 5
} np_ipc_message_flags_t;
```

High-Priority-Nachrichten benötigen eine geeignete Capability oder Endpunktrichtlinie.

## 13. Senden

```c
np_status_t np_ipc_send(
    np_handle_t endpoint,
    const np_ipc_send_args_t* arguments,
    np_time_ns_t deadline
);
```

Der Sendeablauf umfasst:

1. Endpunkthandle auflösen,
2. Senderecht prüfen,
3. Nachrichtengröße validieren,
4. Payload in Kernelspeicher kopieren,
5. Handle-Deskriptoren validieren,
6. Zielkapazität prüfen,
7. Nachricht atomar veröffentlichen,
8. wartenden Empfänger aufwecken.

## 14. Empfangen

```c
np_status_t np_ipc_receive(
    np_handle_t endpoint,
    np_ipc_receive_args_t* arguments,
    np_time_ns_t deadline
);
```

Beim Empfang werden:

- die nächste zulässige Nachricht ausgewählt,
- Metadaten validiert,
- Payload in den Zielpuffer kopiert,
- übertragene Handles veröffentlicht,
- die tatsächlichen Größen zurückgegeben,
- Queue-Kapazität freigegeben,
- gegebenenfalls wartende Sender aufgeweckt.

## 15. Atomare Nachrichtenzustellung

Eine Nachricht wird entweder vollständig zugestellt oder nicht zugestellt.

Dies umfasst:

- Payload,
- Handle-Übertragungen,
- Shared-Memory-Deskriptoren,
- Queue-Eintrag,
- Sequenzinformationen.

Teilweise veröffentlichte Handles oder Nachrichten sind unzulässig, sofern die konkrete Streaming-API keine ausdrücklich andere Semantik definiert.

## 16. Nachrichtenreihenfolge

Geordnete Channels garantieren FIFO-Reihenfolge pro sendendem Endpunkt.

Bei mehreren sendenden Quellen wird keine globale zeitliche Reihenfolge garantiert, sofern sie nicht über denselben Queue-Endpunkt serialisiert werden.

Priorisierte Nachrichten dürfen die FIFO-Reihenfolge nur innerhalb ausdrücklich definierter Prioritätsklassen verändern.

## 17. Message IDs

Jede Nachricht kann eine senderseitig eindeutige `message_id` besitzen.

Antworten verwenden zusätzlich eine `correlation_id`.

Diese Kennungen:

- dienen Zuordnung und Diagnose,
- verleihen keine Rechte,
- werden nicht als Kernelobjektkennungen interpretiert,
- müssen nicht systemweit eindeutig sein.

## 18. Handle-Übertragung

Handles werden über Deskriptoren übertragen.

```c
typedef struct np_ipc_handle_descriptor {
    np_handle_t source_handle;
    np_capability_rights_t target_rights;
    uint32_t transfer_mode;
    uint32_t target_flags;
} np_ipc_handle_descriptor_t;
```

Zielrechte dürfen die delegierbaren Quellrechte nicht überschreiten.

## 19. Copy- und Move-Transfer

```c
typedef enum np_ipc_handle_transfer_mode {
    NP_IPC_HANDLE_COPY,
    NP_IPC_HANDLE_MOVE
} np_ipc_handle_transfer_mode_t;
```

Bei `COPY` bleibt das Quellhandle gültig.

Bei `MOVE` wird es erst geschlossen, nachdem Nachricht und Zielhandle erfolgreich veröffentlicht wurden.

Schlägt die Zustellung fehl, bleibt das Quellhandle gültig.

## 20. Queue-Modell

Jeder empfangende Endpunkt besitzt eine begrenzte Nachrichtenqueue.

```c
typedef struct np_message_queue_limits {
    uint32_t maximum_messages;
    uint64_t maximum_payload_bytes;
    uint32_t maximum_handles;
} np_message_queue_limits_t;
```

Die Grenzwerte werden bei Endpunkterzeugung festgelegt oder durch Sicherheitsrichtlinien begrenzt.

## 21. Backpressure

Ist die Zielqueue voll, kann der Sender:

- blockieren,
- sofort `NP_ERR_WOULD_BLOCK` erhalten,
- bis zu einer Deadline warten,
- bei unterstützter API eine asynchrone Completion erhalten.

Unbegrenztes Wachstum der Zielqueue ist unzulässig.

Der Empfänger darf den Sender nicht durch nicht begrenzte Kernelpufferbelegung zum Speicherverbrauch zwingen.

## 22. Timeouts

IPC-Timeouts verwenden absolute monotone Deadlines.

```c
#define NP_IPC_NO_WAIT 0
#define NP_IPC_WAIT_INFINITE UINT64_MAX
```

Ein Timeout verändert eine noch nicht veröffentlichte Nachricht nicht.

Wurde eine Nachricht bereits vollständig zugestellt, darf ein späterer Timeout nicht fälschlich als fehlgeschlagene Zustellung gemeldet werden.

## 23. Abbruch

Blockierende IPC-Operationen können abgebrochen werden durch:

- Thread-Cancellation,
- Prozessbeendigung,
- Endpunktschließung,
- Signalzustellung,
- Systemabschaltung,
- explizites Abbruchobjekt.

Die Operation liefert einen definierten Status und entfernt den Thread atomar aus der Wait Queue.

## 24. Shared Memory

Große Datenmengen werden über Shared-Memory-Objekte übertragen.

```c
np_status_t np_shared_memory_create(
    uint64_t size,
    uint32_t flags,
    np_handle_t* shared_memory
);

np_status_t np_shared_memory_map(
    np_handle_t shared_memory,
    np_virtual_address_t requested_address,
    uint32_t protection,
    np_virtual_address_t* mapped_address
);
```

Jedes Mapping besitzt eigene Schutzrechte.

## 25. Shared-Memory-Rechte

Mindestens folgende Rechte werden unterschieden:

- Map Read,
- Map Write,
- Resize,
- Seal,
- Transfer,
- Query.

Ein Empfänger kann ein Shared-Memory-Handle mit ausschließlich lesenden Rechten erhalten.

Die Übertragung eines Handles darf keine weitergehenden Rechte erzeugen.

## 26. Shared-Memory-Seals

Shared-Memory-Objekte können versiegelt werden.

```c
typedef enum np_shared_memory_seal {
    NP_SHM_SEAL_RESIZE = 1U << 0,
    NP_SHM_SEAL_WRITE  = 1U << 1,
    NP_SHM_SEAL_FUTURE_WRITE = 1U << 2
} np_shared_memory_seal_t;
```

Ein gesetztes Seal kann ohne spezielle administrative Berechtigung nicht entfernt werden.

Seals eignen sich für unveränderliche Nachrichtenpuffer und signierte Daten.

## 27. Events

Events dienen der leichten Zustandsbenachrichtigung.

```c
typedef enum np_event_mode {
    NP_EVENT_AUTO_RESET,
    NP_EVENT_MANUAL_RESET,
    NP_EVENT_COUNTING
} np_event_mode_t;
```

```c
np_status_t np_event_create(
    np_event_mode_t mode,
    uint64_t initial_value,
    np_handle_t* event
);

np_status_t np_event_signal(
    np_handle_t event,
    uint64_t value
);

np_status_t np_event_reset(
    np_handle_t event
);
```

Events transportieren keine beliebigen Payload-Daten.

## 28. Signale

Signale sind asynchrone Benachrichtigungen an Prozesse oder Threads.

Sie werden nicht in beliebigen Kernelabschnitten direkt ausgeführt.

Die Zustellung erfolgt:

- vor der Rückkehr in den Userspace,
- bei unterbrechbaren Warteoperationen,
- an einem registrierten Signalendpunkt.

Signale ersetzen keine strukturierten IPC-Nachrichten.

## 29. RPC

RPC verwendet Message Channels mit Anfrage-Antwort-Zuordnung.

```c
np_status_t np_rpc_call(
    np_handle_t endpoint,
    const np_rpc_request_t* request,
    np_rpc_response_t* response,
    np_time_ns_t deadline
);
```

Ein RPC-Aufruf:

1. sendet eine Request-Nachricht,
2. blockiert oder wartet asynchron,
3. ordnet die Antwort über eine Correlation ID zu,
4. behandelt Timeout, Abbruch und Peer-Ende.

## 30. RPC-Endpunkte

Ein RPC-Service kann einen benannten oder capability-übertragenen Endpunkt veröffentlichen.

Die Veröffentlichung eines Namens erfolgt über den Object Manager.

Vor jeder Verbindung werden geprüft:

- Namespace-Sicht,
- Connect-Recht,
- Protokollversion,
- Sicherheitsrichtlinie,
- Serviceverfügbarkeit.

## 31. RPC-Fehler

RPC unterscheidet:

- Transportfehler,
- Timeout,
- Peer-Abbruch,
- Protokollfehler,
- Remote-Statuscode,
- unvollständige Antwort.

Ein Remote-Fehler darf nicht mit einem Kernel-Transportfehler vermischt werden.

## 32. Pipes

Pipes stellen geordnete Bytestromkommunikation bereit.

```c
np_status_t np_pipe_create(
    uint32_t flags,
    np_handle_t* read_end,
    np_handle_t* write_end
);
```

Pipes unterstützen:

- blockierendes und nicht blockierendes Lesen,
- blockierendes und nicht blockierendes Schreiben,
- End-of-File nach Schließen aller Writer,
- Fehler bei Schreiben ohne Reader,
- definierte atomare Schreibgröße.

## 33. Atomare Pipe-Schreibvorgänge

Schreibvorgänge bis zu einer definierten Grenze werden nicht mit Daten anderer Writer vermischt.

```text
NP_PIPE_ATOMIC_WRITE_MAX
```

Größere Schreibvorgänge können in mehrere Teilbereiche zerlegt werden.

Die tatsächliche Grenze muss über die Pipe-API abfragbar sein.

## 34. Lokale Sockets

Lokale Sockets stellen eine socketähnliche IPC-Schnittstelle bereit.

Unterstützte Modi können sein:

- Stream,
- Datagram,
- Sequenced Packet.

Lokale Sockets verwenden das native Objekt-, Handle- und Capability-Modell.

Sie dürfen nicht automatisch über das Netzwerk erreichbar sein.

## 35. Benannte IPC-Objekte

IPC-Endpunkte können in einem Object Namespace veröffentlicht werden.

Beispiel:

```text
/system/services/device-manager
/session/42/services/clipboard
/application/example/control
```

Ein Name dient nur zum Auffinden. Die Verbindung benötigt weiterhin ein passendes Recht.

## 36. Service Discovery

Ein Dienst kann einen Endpunkt und eine Protokollbeschreibung veröffentlichen.

Die Beschreibung kann enthalten:

- Service-ID,
- Protokollversion,
- unterstützte Features,
- maximale Nachrichtengröße,
- erforderliche Capabilities,
- Verfügbarkeitsstatus.

Service Discovery darf keine geheimen oder nicht sichtbaren Dienste offenlegen.

## 37. Protokollversionierung

Jedes strukturierte IPC-Protokoll muss eine Version besitzen.

```c
typedef struct np_ipc_protocol_header {
    uint16_t major_version;
    uint16_t minor_version;
    uint32_t structure_size;
    uint64_t feature_flags;
} np_ipc_protocol_header_t;
```

Inkompatible Major-Versionen werden abgelehnt.

Strukturen dürfen kompatibel am Ende erweitert werden.

## 38. Prioritätsvererbung

Bei synchronem RPC kann die Priorität des aufrufenden Threads begrenzt an den bearbeitenden Serverthread vererbt werden.

Die Vererbung:

- ist zeitlich begrenzt,
- überschreitet keine Server- oder Job-Obergrenze,
- endet mit Antwort, Timeout oder Abbruch,
- wird über verschachtelte RPC-Ketten kontrolliert propagiert.

Unbegrenzte Prioritätseskalation ist unzulässig.

## 39. Deadlock-Begrenzung

Das IPC Framework kann nicht alle logischen Anwendungsdeadlocks verhindern.

Es muss jedoch vermeiden:

- Kerneldeadlocks durch gehaltene interne Sperren beim Blockieren,
- verlorene Wakeups,
- zyklische Prioritätsvererbung ohne Begrenzung,
- unbegrenzte synchrone Callback-Ketten.

Timeouts und asynchrone Protokolle werden für komplexe Dienstabhängigkeiten empfohlen.

## 40. Zero-Copy

Zero-Copy wird über Shared Memory und Seitenmapping unterstützt.

Inline-Nachrichten werden aus Sicherheits- und Einfachheitsgründen regulär kopiert.

Ein Zero-Copy-Datentransfer muss:

- Besitz und Lebenszeit der Seiten definieren,
- Schreibrechte begrenzen,
- Änderungen während der Verarbeitung berücksichtigen,
- Seiten pinnen oder snapshotten, wenn erforderlich,
- eine eindeutige Abschlussoperation besitzen.

## 41. Userspace-Treiber

Userspace-Treiber verwenden IPC für:

- Geräteanfragen,
- Interrupt-Ereignisse,
- DMA-Completion,
- Device-Manager-Kommunikation,
- Fehler- und Zustandsmeldungen.

Ein Treiber erhält nur Endpunkte und Handles für die ihm zugeordneten Geräte.

Beim Treiberabsturz werden die Verbindungen geschlossen und wartende Clients kontrolliert benachrichtigt.

## 42. Kernel-IPC

Kernelkomponenten können dieselben logischen IPC-Objekte verwenden, greifen jedoch über interne typisierte APIs darauf zu.

Kernelcode ruft keine Userspace-System-Call-Stubs auf.

Kernel-IPC muss dieselben Lebenszyklus-, Rechte- und Queue-Grenzen berücksichtigen, sofern nicht ein ausdrücklich interner Endpunkt verwendet wird.

## 43. Endpunktschließung

Beim Schließen eines Endpunkts muss das Framework:

1. neue Sendungen verhindern,
2. blockierte Sender aufwecken,
3. den Peer benachrichtigen,
4. ausstehende RPC-Aufrufe beenden,
5. übertragene, aber nicht zugestellte Handles zurückrollen,
6. Waiter signalisieren,
7. Queue und Objektreferenzen freigeben.

Ein Peer-Closed-Zustand muss eindeutig erkennbar sein.

## 44. Prozessbeendigung

Beim Prozessende werden seine IPC-Handles durch den Handle Manager geschlossen.

Das IPC Framework muss daraufhin:

- Endpunkte trennen,
- blockierende Operationen abbrechen,
- Shared-Memory-Mappings lösen,
- RPC-Partner benachrichtigen,
- übertragene Rechte korrekt freigeben,
- keine Kernelqueues dauerhaft zurücklassen.

## 45. Ressourcenlimits

Limits können gelten pro:

- Prozess,
- Job Object,
- Endpunkt,
- Nachricht,
- Capability-Domain,
- System.

Begrenzt werden mindestens:

- Queuegröße,
- Nachrichtenzahl,
- Payloadgröße,
- Handleanzahl,
- Shared-Memory-Größe,
- offene Channels,
- parallele RPC-Aufrufe.

## 46. Synchronisation

Das IPC Framework verwendet:

- endpunktlokale Sperren,
- atomare Queuezustände,
- Wait Queues,
- referenzgezählte Nachrichten,
- transaktionale Handle-Übertragung,
- CPU-lokale Pools für häufige Nachrichtenobjekte.

Eine globale IPC-Sperre im regulären Sende- und Empfangspfad ist unzulässig.

## 47. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_IPC_CONNECT` | Verbindung zu einem Endpunkt herstellen |
| `CAP_IPC_SEND` | Nachrichten senden |
| `CAP_IPC_RECEIVE` | Nachrichten empfangen |
| `CAP_IPC_TRANSFER_HANDLE` | Handles übertragen |
| `CAP_IPC_PUBLISH` | benannten Endpunkt veröffentlichen |
| `CAP_IPC_SHARED_MEMORY` | Shared Memory verwenden |
| `CAP_IPC_SIGNAL` | Event oder Signal auslösen |
| `CAP_IPC_QUERY` | Endpunktinformationen abfragen |
| `CAP_IPC_ADMIN` | globale IPC-Richtlinien verwalten |

Rechte müssen auf konkrete Objekte und Sicherheitsdomains begrenzbar sein.

## 48. System-Call-API

Mindestens folgende Operationen werden über die versionierte Kernel-Service-ABI bereitgestellt:

```c
np_status_t np_sys_ipc_channel_create(
    np_ipc_channel_create_args_t* arguments
);

np_status_t np_sys_ipc_send(
    np_ipc_send_args_t* arguments
);

np_status_t np_sys_ipc_receive(
    np_ipc_receive_args_t* arguments
);

np_status_t np_sys_rpc_call(
    np_rpc_call_args_t* arguments
);

np_status_t np_sys_ipc_shutdown(
    np_ipc_shutdown_args_t* arguments
);
```

Alle Userspace-Puffer und Deskriptoren werden vor ihrer Verarbeitung sicher kopiert und validiert.

## 49. Sicherheit

Das IPC Framework muss sicherstellen, dass:

- Endpunkte nur mit gültigen Handles erreichbar sind,
- jede Operation die erforderlichen Rechte prüft,
- Handles bei Übertragung keine zusätzlichen Rechte erhalten,
- Nachrichtenlängen und Zähler auf Überlauf geprüft werden,
- Queue-Grenzen nicht überschritten werden,
- Kernelzeiger niemals in Nachrichtenmetadaten erscheinen,
- Shared-Memory-Rechte pro Mapping geprüft werden,
- ein Prozess keine fremden Nachrichtenqueues lesen kann,
- Peer-Identitäten nicht allein aus Userspace-Angaben übernommen werden,
- fehlgeschlagene Transfers vollständig zurückgerollt werden.

## 50. Datenschutz

IPC-Nachrichten können vertrauliche oder personenbezogene Daten enthalten.

Daher gelten:

- Payload-Inhalte werden standardmäßig nicht protokolliert,
- detailliertes IPC-Tracing benötigt besondere Capabilities,
- Diagnosedaten bleiben standardmäßig lokal,
- Shared-Memory-Inhalte werden nicht durch generische Diagnose-APIs ausgegeben,
- Sicherheitsdomains sehen nur ihre eigenen oder freigegebenen Endpunkte,
- externe Telemetrie ist standardmäßig deaktiviert.

## 51. Diagnose

Das IPC Framework stellt autorisierten Diagnosediensten bereit:

- Anzahl aktiver Channels und Endpunkte,
- Queue-Auslastung,
- gesendete und empfangene Nachrichten,
- blockierte Sender und Empfänger,
- Timeouts und Abbrüche,
- Handle-Übertragungen,
- RPC-Latenzen,
- Peer-Closed-Ereignisse,
- Ressourcenlimitverletzungen.

Payload-Daten werden nicht standardmäßig erfasst.

## 52. Statistik

```c
typedef struct np_ipc_statistics {
    uint64_t sent_messages;
    uint64_t received_messages;
    uint64_t transferred_bytes;
    uint64_t transferred_handles;

    uint64_t blocked_sends;
    uint64_t blocked_receives;
    uint64_t timeouts;
    uint64_t cancelled_operations;

    uint64_t active_endpoints;
    uint64_t active_shared_memory_objects;
    uint64_t failed_deliveries;
} np_ipc_statistics_t;
```

Statistiken sollen pro Prozess, Endpunkt, Job und System abrufbar sein.

## 53. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_INVALID_HANDLE` | Endpunkt- oder Objekthandle ungültig |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_MESSAGE_TOO_LARGE` | Nachricht überschreitet die Grenze |
| `NP_ERR_QUEUE_FULL` | Zielqueue ist vollständig belegt |
| `NP_ERR_BUFFER_TOO_SMALL` | Empfangspuffer ist zu klein |
| `NP_ERR_WOULD_BLOCK` | Operation würde blockieren |
| `NP_ERR_TIMEOUT` | Deadline wurde erreicht |
| `NP_ERR_INTERRUPTED` | Operation wurde unterbrochen |
| `NP_ERR_PEER_CLOSED` | Gegenstelle wurde geschlossen |
| `NP_ERR_PROTOCOL` | ungültige Protokollnachricht |
| `NP_ERR_RIGHTS_ESCALATION` | Handle-Rechte sind unzulässig |
| `NP_ERR_RESOURCE_LIMIT` | IPC-Ressourcenlimit erreicht |
| `NP_ERR_TRANSACTION_ABORTED` | atomare Zustellung wurde zurückgerollt |

## 54. Fehlerbehandlung

Ein Fehler eines Userspace-Endpunkts muss auf dessen Prozess oder Sicherheitsdomain begrenzt bleiben.

Folgende Situationen gelten als schwere Kernelkonsistenzfehler:

- teilweise veröffentlichte atomare Handle-Übertragung,
- Nachricht verweist auf freigegebenen Kernelspeicher,
- beschädigte Queue-Verknüpfung,
- doppelte Freigabe übertragener Handles,
- verlorener Wakeup mit dauerhaft blockiertem Kernelzustand,
- Rechteausweitung während einer IPC-Übertragung,
- Kernelzeiger wird an Userspace offengelegt.

## 55. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Erzeugung eines Channels,
2. Senden und Empfangen einer Inline-Nachricht,
3. FIFO-Reihenfolge,
4. bidirektionale Kommunikation,
5. nicht blockierendes Senden,
6. nicht blockierendes Empfangen,
7. blockierendes Senden bei voller Queue,
8. Timeout eines Senders,
9. Timeout eines Empfängers,
10. Abbruch einer Warteoperation,
11. maximale Nachrichtengröße,
12. zu kleiner Empfangspuffer,
13. Handle-Copy-Übertragung,
14. Handle-Move-Übertragung,
15. Reduktion übertragener Rechte,
16. Ablehnung einer Rechteerhöhung,
17. Rollback fehlgeschlagener Übertragung,
18. mehrere Handles in einer Nachricht,
19. Shared-Memory-Erzeugung,
20. lesendes und schreibendes Shared-Memory-Mapping,
21. Shared-Memory-Seals,
22. Event mit Auto Reset,
23. Event mit Manual Reset,
24. Counting Event,
25. synchroner RPC-Aufruf,
26. RPC-Timeout,
27. RPC-Abbruch nach Peer-Ende,
28. Prioritätsvererbung bei RPC,
29. Pipe-Lesen und -Schreiben,
30. atomare Pipe-Schreibgröße,
31. End-of-File einer Pipe,
32. lokaler Stream-Socket,
33. benannter IPC-Endpunkt,
34. Namespace-Isolation,
35. Absturz eines Userspace-Treibers,
36. Prozessbeendigung mit offenen Endpunkten,
37. Queue- und Ressourcenlimits,
38. parallele Kommunikation auf mehreren CPUs,
39. korrekte Diagnose und Statistik,
40. Schutz vor Payload-Offenlegung im Standard-Tracing.

## 56. Verbindliche Invarianten

1. Jede IPC-Operation verwendet gültige capability-geschützte Handles.
2. Nachrichten werden atomar vollständig oder gar nicht veröffentlicht.
3. Zielrechte übertragener Handles überschreiten niemals die delegierbaren Quellrechte.
4. Eine fehlgeschlagene Move-Übertragung lässt das Quellhandle gültig.
5. IPC-Queues besitzen feste oder kontrolliert begrenzte Kapazitäten.
6. Ein geschlossener Endpunkt nimmt keine neuen Nachrichten an.
7. Kernelzeiger werden niemals über IPC an Userspace übertragen.
8. Shared-Memory-Mappings besitzen ausschließlich ihre zugewiesenen Rechte.
9. Blockierende IPC-Operationen halten keine unzulässigen Kernel-Spinlocks.
10. Das Ende eines Prozesses weckt oder beendet alle betroffenen IPC-Warteoperationen.
11. Userspace-Treiber können nur ihre zugewiesenen Interrupt- und Geräteendpunkte verwenden.
12. IPC-Zustandsänderungen erfolgen unter geeigneter Synchronisation.

## 57. Referenzablauf einer Nachrichtenzustellung

```text
Sender ruft IPC Send auf
  -> Endpunkthandle und Senderecht prüfen
  -> Header, Payload und Deskriptoren kopieren
  -> Größen und Protokollversion validieren
  -> Handle-Übertragungen vorbereiten
  -> Zielqueue und Ressourcenlimits prüfen
  -> Zielhandles reservieren
  -> Nachricht und Handles atomar veröffentlichen
  -> wartenden Empfänger aufwecken
  -> bei MOVE Quellhandles schließen
  -> Ergebnis an Sender zurückgeben
```

## 58. Zusammenfassung

Das NovaOS IPC Framework stellt ein gemeinsames Kommunikationsmodell für Prozesse, Kernelkomponenten, Systemdienste und Userspace-Treiber bereit.

Kleine strukturierte Daten werden über geordnete Message Channels übertragen. Große Datenmengen verwenden Shared Memory, während Events, Pipes, lokale Sockets und RPC auf denselben Objekt-, Handle- und Capability-Grundlagen aufbauen.

Begrenzte Queues, atomare Handle-Übertragungen, Timeouts, Backpressure und klar definierte Peer-Closed-Zustände verhindern unkontrollierten Ressourcenverbrauch und inkonsistente Kommunikation zwischen Sicherheitsdomains.