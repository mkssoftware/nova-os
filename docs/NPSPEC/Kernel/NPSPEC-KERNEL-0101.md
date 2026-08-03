# NPSPEC-KERNEL-0101 – Event Bus

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0101 |
| Titel | Event Bus |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Ereignisarchitektur |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0012, NPSPEC-KERNEL-0013, NPSPEC-KERNEL-0014, NPSPEC-KERNEL-0015, NPSPEC-KERNEL-0016, NPSPEC-KERNEL-0020, NPSPEC-KERNEL-0100 |
| Zugehörige ADRs | ADR-KERNEL-0100, ADR-KERNEL-0101, ADR-KERNEL-0102, ADR-KERNEL-0103, ADR-KERNEL-0104, ADR-IPC-0003 |

---

## 1. Zweck

Diese Spezifikation definiert den Event Bus des NovaOS-Kernels.

Der Event Bus stellt eine gemeinsame ereignisgesteuerte Infrastruktur bereit für:

- Kernelkomponenten,
- Kernel Object Graph,
- Device Manager,
- Process und Thread Manager,
- VFS,
- Network Stack,
- Power Manager,
- Security Manager,
- Userspace-Systemdienste,
- lokale Diagnose.

## 2. Ziele

Der Event Bus muss:

- typisierte Ereignisse bereitstellen,
- synchrone und asynchrone Zustellung unterstützen,
- Publisher und Subscriber entkoppeln,
- Capability- und Sichtbarkeitsregeln erzwingen,
- auf SMP-Systemen skalieren,
- Queuegrößen und Ressourcenverbrauch begrenzen,
- Ereignisverlust eindeutig anzeigen,
- Objekt- und Graphlebenszyklen berücksichtigen,
- sensible Payloads filtern,
- im frühen Boot eingeschränkt verfügbar sein.

## 3. Abgrenzung

| Komponente | Verantwortung |
|---|---|
| Event Bus | Veröffentlichung, Routing und Zustellung von Ereignissen |
| IPC Framework | allgemeine Prozesskommunikation und Payloadübertragung |
| Event Object | leichter wartbarer Signalisierungszustand |
| Logging Framework | Diagnose- und Fehlerprotokolle |
| Audit Subsystem | manipulationsgeschützte Sicherheitsereignisse |
| Kernel Object Graph | Objekt- und Beziehungsmodell |
| Timer Subsystem | zeitgesteuerte Ereignisauslösung |
| Notification Service | benutzersichtbare Mitteilungen |

Ein Event-Bus-Ereignis ist nicht automatisch eine Benutzerbenachrichtigung.

## 4. Grundmodell

Der Event Bus verwendet ein Publish-Subscribe-Modell.

```text
Publisher
  -> typisiertes Ereignis
  -> Event Bus
  -> Sicherheits- und Filterprüfung
  -> ein oder mehrere Subscriber
```

Publisher müssen Subscriber nicht direkt kennen.

Subscriber erhalten nur Ereignisse, für die sie registriert und autorisiert sind.

## 5. Eventtypen

```c
typedef uint64_t np_event_type_id_t;
```

Jeder Eventtyp besitzt eine systemweit eindeutige und registrierte Kennung.

Eine Event Type ID:

- verleiht keine Rechte,
- ist keine Objekt-ID,
- wird nach Veröffentlichung nicht mit anderer Bedeutung wiederverwendet,
- kann nach Namespace und Kategorie strukturiert sein.

## 6. Event-Namespace

Eventtypen werden in logische Namespaces eingeteilt:

```text
kernel.boot.*
kernel.object.*
kernel.process.*
kernel.thread.*
kernel.memory.*
kernel.device.*
kernel.vfs.*
kernel.network.*
kernel.power.*
kernel.security.*
kernel.diagnostic.*
```

Module dürfen nur Eventtypen innerhalb ihrer registrierten Namespaces veröffentlichen.

## 7. Eventklassen

```c
typedef enum np_event_class {
    NP_EVENT_CLASS_STATE,
    NP_EVENT_CLASS_LIFECYCLE,
    NP_EVENT_CLASS_RESOURCE,
    NP_EVENT_CLASS_SECURITY,
    NP_EVENT_CLASS_ERROR,
    NP_EVENT_CLASS_DIAGNOSTIC,
    NP_EVENT_CLASS_COMPLETION,
    NP_EVENT_CLASS_NOTIFICATION
} np_event_class_t;
```

| Klasse | Bedeutung |
|---|---|
| `STATE` | Änderung eines Objekt- oder Systemzustands |
| `LIFECYCLE` | Erzeugung, Start, Stop oder Entfernung |
| `RESOURCE` | Ressourcenlimit oder Ressourcenänderung |
| `SECURITY` | sicherheitsrelevantes Ereignis |
| `ERROR` | Fehlerzustand |
| `DIAGNOSTIC` | technische Diagnoseinformation |
| `COMPLETION` | Abschluss einer asynchronen Operation |
| `NOTIFICATION` | allgemeine Systembenachrichtigung |

## 8. Eventschema

Jeder Eventtyp besitzt ein registriertes Schema.

```c
typedef struct np_event_schema {
    np_event_type_id_t type_id;
    np_string_view_t name;

    np_abi_version_t version;
    np_event_class_t event_class;

    size_t minimum_payload_size;
    size_t maximum_payload_size;

    np_event_validation_t validator;
    np_event_redaction_t redactor;

    uint32_t flags;
} np_event_schema_t;
```

Unregistrierte Eventtypen dürfen nicht über öffentliche Event-Bus-Schnittstellen veröffentlicht werden.

## 9. Schemaregistrierung

```c
np_status_t np_event_schema_register(
    const np_event_schema_descriptor_t* descriptor
);
```

Vor der Registrierung werden geprüft:

- eindeutige Event Type ID,
- eindeutiger Name,
- gültige ABI-Version,
- Payloadgrenzen,
- Modulnamespace,
- Sicherheitsklasse,
- erforderliche Filterregeln.

Bestehende Eventtypen dürfen innerhalb derselben Major-Version ihre Bedeutung nicht ändern.

## 10. Event Record

```c
typedef struct np_event_record_header {
    uint16_t major_version;
    uint16_t minor_version;
    uint32_t header_size;

    np_event_type_id_t type_id;
    np_event_class_t event_class;
    uint32_t flags;

    np_event_id_t event_id;
    np_event_id_t correlation_id;

    np_time_ns_t timestamp;
    uint64_t sequence_number;

    np_object_id_t source_object_id;
    np_object_id_t subject_object_id;

    uint32_t payload_size;
    uint32_t security_class;
} np_event_record_header_t;
```

Auf den Header folgt die versionierte Payload.

## 11. Event ID

```c
typedef np_uuid_t np_event_id_t;
```

Jedes veröffentlichte Ereignis erhält eine eindeutige Event ID oder eine ausreichend eindeutige Kombination aus:

- Boot ID,
- CPU ID,
- Sequenznummer,
- Zeitstempel.

Die ID dient Korrelation und Diagnose, nicht der Autorisierung.

## 12. Correlation ID

Zusammengehörige Ereignisse können über eine Correlation ID verbunden werden.

Beispiel:

```text
DEVICE_REMOVAL_REQUESTED
DEVICE_STOPPING
DRIVER_STOPPED
DEVICE_REMOVED
```

Eine Correlation ID darf nicht als Beweis für die Herkunft oder Berechtigung eines Ereignisses verwendet werden.

## 13. Event-Payload

Payloads müssen:

- einem registrierten Schema entsprechen,
- eine definierte Maximalgröße besitzen,
- vollständig initialisiert sein,
- keine ungeprüften Kernelzeiger enthalten,
- sicherheitsklassifizierte Felder kennzeichnen,
- vor der Zustellung validiert werden.

Große Datenmengen werden nicht direkt über Event-Payloads übertragen, sondern über capability-geschützte Objekt- oder Shared-Memory-Handles.

## 14. Payload-Feldtypen

Strukturierte Payloads können enthalten:

- Ganzzahlen,
- Booleans,
- Statuscodes,
- Objekt-IDs,
- Handles für gezielte Empfänger,
- UUIDs,
- begrenzte UTF-8-Zeichenketten,
- Zeitwerte,
- typisierte Arrays,
- versionierte Unterstrukturen.

Direkte Funktionszeiger und interne Kernelzeiger sind unzulässig.

## 15. Eventflags

```c
typedef enum np_event_flags {
    NP_EVENT_SYNCHRONOUS     = 1U << 0,
    NP_EVENT_ASYNCHRONOUS    = 1U << 1,
    NP_EVENT_RELIABLE        = 1U << 2,
    NP_EVENT_COALESCABLE     = 1U << 3,
    NP_EVENT_HIGH_PRIORITY   = 1U << 4,
    NP_EVENT_REPLAYABLE      = 1U << 5,
    NP_EVENT_AUDITED         = 1U << 6,
    NP_EVENT_SENSITIVE       = 1U << 7,
    NP_EVENT_KERNEL_ONLY     = 1U << 8
} np_event_flags_t;
```

Widersprüchliche Flags müssen abgelehnt werden.

## 16. Veröffentlichung

```c
np_status_t np_event_publish(
    const np_event_publish_request_t* request
);
```

Der Ablauf umfasst:

1. Eventtyp auflösen,
2. Publish-Recht prüfen,
3. Schema und Payload validieren,
4. Quell- und Zielobjekt prüfen,
5. Event Record erzeugen,
6. Subscriber bestimmen,
7. Sicherheitsfilter anwenden,
8. Ereignis zustellen oder einreihen,
9. Statistik und Audit aktualisieren.

## 17. Publisher

Ein Publisher kann sein:

- Kernelkomponente,
- Kernelobjekt,
- Treiber,
- autorisierter Systemdienst,
- kontrollierte Userspace-Anwendung.

Der Kernel bestimmt die tatsächliche Publisheridentität aus dem aktuellen Sicherheitskontext.

Eine selbst angegebene Publisher-ID aus der Payload ist nicht vertrauenswürdig.

## 18. Subscription

```c
np_status_t np_event_subscribe(
    const np_event_subscription_request_t* request,
    np_event_subscription_handle_t* subscription
);
```

Eine Subscription kann filtern nach:

- Eventtyp,
- Eventklasse,
- Quellobjekt,
- Subject-Objekt,
- Objekt-Subtree,
- Namespace,
- Sicherheitsdomain,
- Schweregrad,
- benutzerdefinierten sicheren Feldern.

## 19. Subscription-Objekt

```c
typedef struct np_event_subscription {
    np_kernel_object_t object;

    np_event_filter_t filter;
    np_event_delivery_mode_t delivery_mode;

    np_event_queue_t queue;
    np_capability_domain_t* owner_domain;

    uint32_t state;
    uint32_t flags;
} np_event_subscription_t;
```

Subscriptions sind capability-geschützte Kernelobjekte.

## 20. Subscription-Zustände

```c
typedef enum np_subscription_state {
    NP_SUBSCRIPTION_CREATED,
    NP_SUBSCRIPTION_ACTIVE,
    NP_SUBSCRIPTION_PAUSED,
    NP_SUBSCRIPTION_CLOSING,
    NP_SUBSCRIPTION_CLOSED
} np_subscription_state_t;
```

Nur aktive Subscriptions erhalten neue Ereignisse.

Pausierte Subscriptions können abhängig von ihrer Richtlinie Ereignisse puffern oder verwerfen.

## 21. Zustellungsarten

```c
typedef enum np_event_delivery_mode {
    NP_EVENT_DELIVERY_CALLBACK,
    NP_EVENT_DELIVERY_QUEUE,
    NP_EVENT_DELIVERY_IPC,
    NP_EVENT_DELIVERY_SIGNAL,
    NP_EVENT_DELIVERY_WAITABLE
} np_event_delivery_mode_t;
```

| Modus | Verwendung |
|---|---|
| `CALLBACK` | interne Kernel-Subscriber |
| `QUEUE` | Kernel- oder Userspace-Eventqueue |
| `IPC` | Zustellung an IPC-Endpunkt |
| `SIGNAL` | leichte asynchrone Thread- oder Prozessinformation |
| `WAITABLE` | signalisierbares Subscriptionobjekt |

Userspace erhält keine direkten Kernelcallback-Zeiger.

## 22. Synchrone Zustellung

Bei synchroner Zustellung wird der Subscriber im Kontext der Veröffentlichung oder eines kontrollierten synchronen Dispatchers aufgerufen.

Sie ist nur zulässig für:

- kurze Kernelcallbacks,
- nicht blockierende Validierungs- oder Zustandsoperationen,
- streng definierte Lifecycle-Hooks.

Synchrone Userspace-Callbacks sind unzulässig.

## 23. Regeln synchroner Callbacks

Ein synchroner Callback darf:

- keine unzulässigen Graph- oder Bus-Sperren rekursiv erwerben,
- nicht unbegrenzt blockieren,
- keine unbekannte Userspace-Arbeit ausführen,
- den Event Record nicht verändern,
- nur explizit erlaubte Folgeereignisse veröffentlichen.

Rekursionstiefe und Ausführungszeit werden begrenzt.

## 24. Asynchrone Zustellung

Asynchrone Ereignisse werden in eine begrenzte Queue eingestellt und später verarbeitet.

Vorteile sind:

- Entkopplung von Publisher und Subscriber,
- geringere Sperrabhängigkeiten,
- Zustellung an Userspace,
- kontrollierte Priorisierung,
- Batch-Verarbeitung.

Der Publisher erhält standardmäßig keine Garantie, dass der Subscriber das Ereignis bereits verarbeitet hat.

## 25. Eventqueues

```c
typedef struct np_event_queue_limits {
    uint32_t maximum_events;
    uint64_t maximum_bytes;
    uint32_t maximum_priority_events;
} np_event_queue_limits_t;
```

Jede Queue besitzt feste oder kontrolliert konfigurierbare Limits.

Unbegrenzte Kernelqueues sind unzulässig.

## 26. Backpressure

Bei gefüllter Queue kann die Subscriptionrichtlinie festlegen:

- neues Ereignis verwerfen,
- ältestes coalescable Ereignis ersetzen,
- gleichartige Ereignisse zusammenfassen,
- Publisher im blockierbaren Kontext kurz warten lassen,
- Subscription deaktivieren,
- Overflow-Ereignis erzeugen.

Interrupt- und Panic-Kontexte dürfen nicht auf eine Userspace-Queue warten.

## 27. Eventverlust

Eventverlust muss eindeutig sichtbar sein.

Dafür wird mindestens erfasst:

- Zahl verworfener Ereignisse,
- erste und letzte verlorene Sequenz,
- betroffener Eventtyp,
- Overflow-Zeitpunkt,
- Subscription-ID.

Der Subscriber muss nach einem Overflow bei Bedarf seinen Zustand über eine Query-API neu synchronisieren.

## 28. Coalescing

Coalescable Ereignisse können zusammengefasst werden.

Beispiele:

- wiederholte Temperaturänderungen,
- Queueauslastung,
- Fortschrittswerte,
- wiederholte Linkstatusprüfungen,
- Ressourcenwarnungen.

Nicht zusammengefasst werden standardmäßig:

- Sicherheitsverletzungen,
- Objektzerstörung,
- einmalige Completion-Ereignisse,
- Handle- oder Capability-Widerrufe.

## 29. Reliable Events

`RELIABLE` bedeutet, dass das Framework besondere Maßnahmen gegen stillen Verlust verwendet.

Mögliche Maßnahmen sind:

- reservierte Queuekapazität,
- Bestätigungen,
- persistente lokale Übergabe,
- Publisherfehler bei nicht möglicher Zustellung,
- Fallback an Audit oder Recovery.

Reliable bedeutet nicht, dass ein abgestürzter Empfänger beliebig lange Kernelressourcen reservieren darf.

## 30. Bestätigung

Bestätigungspflichtige Ereignisse verwenden ein Ack-Token.

```c
np_status_t np_event_acknowledge(
    np_event_subscription_handle_t subscription,
    np_event_ack_token_t token,
    np_status_t processing_status
);
```

Tokens sind:

- einmalig verwendbar,
- zeitlich begrenzt,
- an Subscription und Event gebunden,
- nicht übertragbar, sofern nicht ausdrücklich definiert.

## 31. Bestätigungs-Timeout

Wird ein bestätigungspflichtiges Ereignis nicht rechtzeitig bestätigt, kann der Event Bus:

- erneut zustellen,
- Subscription als fehlerhaft markieren,
- Subscriberprozess neu starten lassen,
- Fallback-Handler verwenden,
- die zuständige Recovery-Policy aktivieren.

Wiederholungen sind begrenzt.

## 32. Ereignisreihenfolge

Der Event Bus garantiert FIFO-Reihenfolge für Ereignisse:

- derselben Quelle,
- desselben Typs,
- an dieselbe Subscription,
- sofern sie dieselbe Prioritätsklasse besitzen.

Eine vollständige globale Reihenfolge aller CPUs und Quellen wird nicht garantiert.

Für stärkere Semantik müssen Generationen oder Correlation IDs verwendet werden.

## 33. Generationswerte

Zustandsereignisse sollen die Generation des betroffenen Objekts enthalten.

Der Subscriber kann dadurch:

- veraltete Ereignisse erkennen,
- verlorene Änderungen feststellen,
- seinen Zustand neu abfragen,
- Reordering sicher behandeln.

Eine niedrigere Generation darf einen bereits beobachteten neueren Zustand nicht überschreiben.

## 34. Prioritäten

```c
typedef enum np_event_priority {
    NP_EVENT_PRIORITY_LOW,
    NP_EVENT_PRIORITY_NORMAL,
    NP_EVENT_PRIORITY_HIGH,
    NP_EVENT_PRIORITY_CRITICAL
} np_event_priority_t;
```

Kritische Priorität ist für begrenzte Kernel-, Security- und Recovery-Ereignisse reserviert.

Unprivilegierte Publisher dürfen ihre Ereignisse nicht beliebig hochstufen.

## 35. Dispatch-Threads

Asynchrone Kernelereignisse werden durch CPU- oder Subsystem-lokale Dispatch-Threads verarbeitet.

Dispatch-Threads:

- besitzen begrenzte Queues,
- arbeiten nach Prioritätsklassen,
- halten keine Publisher-Sperren,
- führen Callbacks mit Zeitbudgets aus,
- verschieben lange Arbeit in Work Queues.

Nicht jede Subscription erhält einen eigenen Thread.

## 36. SMP-Architektur

Der Event Bus verwendet bevorzugt:

- CPU-lokale Publish-Queues,
- sharded Subscriptionindizes,
- RCU-ähnliche Leser,
- referenzgezählte Event Records,
- Batch-Routing,
- verzögerte Freigabe.

Eine globale Event-Bus-Sperre im regulären Publish-Pfad ist unzulässig.

## 37. Interruptkontext

Im direkten Interruptkontext dürfen nur dafür zugelassene Events veröffentlicht werden.

Die Veröffentlichung muss:

- nicht blockieren,
- keine reguläre Heapallokation benötigen oder einen vorbereiteten Pool verwenden,
- begrenzte Payloads verwenden,
- komplexes Routing später ausführen.

Zeitaufwendige Ereignisverarbeitung wird in Deferred Work oder Threaded Interrupts verschoben.

## 38. Early-Boot-Events

Vor Scheduler und Heap steht ein eingeschränkter Early Event Bus zur Verfügung.

Er unterstützt:

- statische Eventtypen,
- synchrone Kernelbeobachter,
- festen Eventpuffer,
- keine Userspace-Zustellung,
- keine dynamischen Subscriptions.

Nach Initialisierung werden relevante Early Events in den regulären Event Bus übernommen oder als Bootstatus zusammengefasst.

## 39. Panic-Modus

Im Panic-Zustand wird der reguläre Event Bus nicht weiter betrieben.

Zulässig sind nur:

- statische Panic-Subscriber,
- Crash-Dump-Signalisierung,
- Panic Logging,
- CPU-Stop-Koordination,
- Recovery-Marker.

Es erfolgen keine normalen Userspace- oder blockierenden IPC-Zustellungen.

## 40. Kernel Object Graph

Der KOG erzeugt Events für:

- Objekterzeugung,
- Zustandsänderung,
- neue Beziehung,
- entfernte Beziehung,
- Quiesce,
- Fehlerisolation,
- Objektzerstörung.

Graphviews können direkt als Eventfilter verwendet werden.

Eine Subscription erhält nur Ereignisse innerhalb ihrer autorisierten Graphsicht.

## 41. Lifecycle-Ereignisse

Lifecycle-Ereignisse besitzen klar definierte Zeitpunkte.

Beispiel:

```text
OBJECT_CREATED
  -> Objekt vollständig initialisiert, aber noch nicht aktiv

OBJECT_ACTIVATED
  -> Objekt kann regulär verwendet werden

OBJECT_QUIESCING
  -> neue Operationen werden begrenzt

OBJECT_REMOVED
  -> Objekt ist nicht mehr erreichbar
```

Ein Ereignis darf keinen Zustand behaupten, der noch nicht atomar veröffentlicht wurde.

## 42. Device-Events

Der Device Manager verwendet den Event Bus für:

- Device Added,
- Driver Bound,
- Device Started,
- Power Changed,
- Removal Requested,
- Surprise Removed,
- Device Failed,
- Device Recovered.

Treiber erhalten nur Ereignisse für zugewiesene Geräte oder Subtrees.

## 43. Process- und Thread-Events

Mögliche Ereignisse sind:

- Process Created,
- Process Started,
- Process Exited,
- Thread Created,
- Thread Blocked,
- Thread Terminated,
- Job Limit Reached.

Hochfrequente Schedulerereignisse gehören normalerweise in das Tracing Framework und nicht in den allgemeinen Event Bus.

## 44. VFS-Events

VFS-Ereignisse umfassen:

- Datei erzeugt,
- Datei entfernt,
- Datei geändert,
- Rename,
- Mount hinzugefügt,
- Mount entfernt,
- Dateisystemfehler.

Dateisystem-Watches verwenden spezialisierte Filter und begrenzte Queues.

Pfade und Namen werden entsprechend der Namespace- und Datenschutzrichtlinie gefiltert.

## 45. Netzwerkereignisse

Der Network Stack veröffentlicht:

- Interface Added oder Removed,
- Link Up oder Down,
- Address Changed,
- Route Changed,
- VPN State Changed,
- Connectivity Changed.

Paketereignisse mit hoher Frequenz gehören nicht in den allgemeinen Event Bus.

## 46. Power-Events

Der Power Manager veröffentlicht:

- Energieprofil geändert,
- Batterie niedrig,
- Thermalgrenze erreicht,
- Suspend angefordert,
- System fortgesetzt,
- Wake-Grund,
- Device Power State geändert.

Wiederholte Sensorwerte dürfen zusammengefasst werden.

## 47. Security-Events

Sicherheitsereignisse umfassen:

- Access Denied,
- Capability Revoked,
- Policy Changed,
- Image Verification Failed,
- Debug State Changed,
- Integrity Violation.

Sicherheitskritische Ereignisse können zusätzlich direkt an das Audit Subsystem gehen.

Normale Eventqueues ersetzen nicht die Auditgarantie.

## 48. Timer-Events

Der Timer Subsystem kann Event-Bus-Zustellung für Userspace-Timer verwenden.

Ein Timerereignis enthält mindestens:

- Timerobjekt,
- geplante Deadline,
- tatsächlichen Zeitpunkt,
- Overrun Count,
- Clock ID.

Hochfrequente interne Scheduler-Timer verwenden keinen allgemeinen Event-Bus-Pfad.

## 49. Subscription-Lebenszyklus

Beim Schließen einer Subscription werden:

1. neue Zustellungen verhindert,
2. Subscription aus Routingindizes entfernt,
3. laufende Callbacks abgeschlossen,
4. Ack-Tokens ungültig gemacht,
5. Queue nach Richtlinie geleert,
6. Objektbeziehungen gelöst,
7. Speicher nach Grace Period freigegeben.

Ein Publisher darf danach keine Referenz auf die Subscription verwenden.

## 50. Subscriber-Ausfall

Endet ein Subscriberprozess, muss der Event Bus:

- seine Subscriptions schließen,
- blockierte Publisher aufwecken,
- Ack-Tokens abbrechen,
- zuverlässige Ereignisse an Fallbackpfade übergeben,
- Queue-Ressourcen freigeben,
- den zuständigen Service Manager benachrichtigen.

Ein Subscriberausfall darf den Kernel nicht blockieren.

## 51. Replay

Replayable Events können in einem begrenzten lokalen Verlauf gespeichert werden.

Anwendungsfälle sind:

- aktueller Gerätebestand,
- letzte Power-Zustandsänderung,
- Serviceverfügbarkeit,
- Security-Policy-Generation.

Replay ist keine unbegrenzte vollständige Historie.

Sensible Events unterliegen zusätzlichen Aufbewahrungsregeln.

## 52. Sticky State

Für reine Zustandsinformationen kann der Bus den letzten bekannten Wert eines Eventtyps oder Objekts bereitstellen.

Neue Subscriber können diesen Zustand beim Registrieren erhalten.

Sticky State wird nur für dafür registrierte Eventtypen verwendet und ersetzt keine authoritative Query-API.

## 53. Event-Bus-API

```c
np_status_t np_event_publish(
    const np_event_publish_request_t* request
);

np_status_t np_event_subscribe(
    const np_event_subscription_request_t* request,
    np_event_subscription_handle_t* subscription
);

np_status_t np_event_receive(
    np_event_subscription_handle_t subscription,
    np_event_record_t* record,
    size_t record_size,
    np_time_ns_t deadline
);

np_status_t np_event_unsubscribe(
    np_event_subscription_handle_t subscription
);
```

Alle öffentlichen Strukturen enthalten ABI-Version und Größe.

## 54. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_EVENT_PUBLISH` | Ereignisse im erlaubten Namespace veröffentlichen |
| `CAP_EVENT_SUBSCRIBE` | erlaubte Eventtypen abonnieren |
| `CAP_EVENT_RECEIVE` | Subscriptionqueue lesen |
| `CAP_EVENT_ACK` | Ereignisse bestätigen |
| `CAP_EVENT_REPLAY` | begrenzten Verlauf lesen |
| `CAP_EVENT_QUERY_SCHEMA` | öffentliche Eventschemas abfragen |
| `CAP_EVENT_REGISTER_SCHEMA` | Eventtypen registrieren |
| `CAP_EVENT_DIAGNOSTIC` | detaillierte Event-Bus-Daten lesen |
| `CAP_EVENT_ADMIN` | globale Eventrichtlinien verwalten |

Rechte können auf Eventtypen, Objekt-Subtrees und Sicherheitsdomains begrenzt werden.

## 55. Sicherheit

Der Event Bus muss sicherstellen, dass:

- Publisheridentität aus dem Kernelkontext stammt,
- jede Veröffentlichung einem registrierten Schema entspricht,
- Payloads keine direkt nutzbaren Kernelzeiger enthalten,
- Subscriber nur autorisierte Ereignisse erhalten,
- Graph- und Namespacegrenzen berücksichtigt werden,
- Eventprioritäten nicht unautorisiert erhöht werden,
- Ack-Tokens einmalig und gebunden sind,
- Subscriber keine Event Records des Kernels verändern,
- verlorene Ereignisse eindeutig angezeigt werden,
- Sicherheitsereignisse nicht allein von normalen Eventqueues abhängen.

## 56. Datenschutz

Event-Payloads können personenbezogene Informationen enthalten.

Daher gelten:

- Datenminimierung,
- Feldklassifikation,
- Redaction vor Zustellung,
- Namespace- und Prozessisolation,
- begrenzte Replay-Historie,
- lokale Verarbeitung als Standard,
- keine automatische externe Übertragung,
- capability-geschützte Diagnose,
- konfigurierbare Aufbewahrung.

## 57. Diagnose

Der Event Bus stellt autorisierten Diagnosediensten bereit:

- registrierte Eventtypen,
- aktive Subscriptions,
- Queueauslastung,
- Publish- und Zustellungsraten,
- verworfene und zusammengefasste Ereignisse,
- Ack-Timeouts,
- Subscriberfehler,
- Dispatch-Latenzen,
- Schemafehler,
- Capability-Verweigerungen.

Payload-Inhalte werden nur nach Sicherheitsrichtlinie angezeigt.

## 58. Statistik

```c
typedef struct np_event_bus_statistics {
    uint64_t published_events;
    uint64_t delivered_events;

    uint64_t synchronous_deliveries;
    uint64_t asynchronous_deliveries;

    uint64_t dropped_events;
    uint64_t coalesced_events;
    uint64_t replayed_events;

    uint64_t active_subscriptions;
    uint64_t queue_overflows;
    uint64_t acknowledgement_timeouts;

    uint64_t rejected_publications;
    uint64_t maximum_dispatch_latency_ns;
} np_event_bus_statistics_t;
```

Statistiken sollen pro Eventtyp, Subscription, CPU und System verfügbar sein.

## 59. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_EVENT_TYPE_UNKNOWN` | Eventtyp ist nicht registriert |
| `NP_ERR_EVENT_SCHEMA` | Payload entspricht nicht dem Schema |
| `NP_ERR_EVENT_TOO_LARGE` | Event überschreitet die Größenbegrenzung |
| `NP_ERR_SUBSCRIPTION_INVALID` | Subscription ist ungültig |
| `NP_ERR_SUBSCRIPTION_CLOSED` | Subscription wurde geschlossen |
| `NP_ERR_EVENT_QUEUE_FULL` | Zielqueue ist vollständig belegt |
| `NP_ERR_EVENT_DROPPED` | Event konnte nicht zugestellt werden |
| `NP_ERR_ACK_INVALID` | Ack-Token ist ungültig |
| `NP_ERR_ACK_EXPIRED` | Bestätigungsfrist ist abgelaufen |
| `NP_ERR_STALE_GENERATION` | Ereignisgeneration ist veraltet |
| `NP_ERR_WOULD_BLOCK` | Operation würde blockieren |
| `NP_ERR_TIMEOUT` | Wartezeit ist abgelaufen |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_NOT_SUPPORTED` | Funktion wird nicht unterstützt |

## 60. Fehlerbehandlung

Ein fehlerhafter Publisher oder Subscriber wird auf seine Sicherheitsdomain begrenzt.

Mögliche Reaktionen sind:

- Veröffentlichung ablehnen,
- Event verwerfen,
- Subscription pausieren,
- Queue leeren,
- Subscriberprozess benachrichtigen oder neu starten,
- zuverlässiges Ereignis an Fallback weitergeben,
- Security- oder Audit-Ereignis erzeugen.

Ein Event-Bus-Fehler darf nicht automatisch den Kernel stoppen, sofern keine zentrale Kernelintegrität betroffen ist.

## 61. Kritische Fehler

Als kritisch gelten:

- teilweise veröffentlichter Event Record wird als gültig gelesen,
- Subscriber erhält unautorisierte sensible Payload,
- Event Record verweist auf freigegebenen Kernelspeicher,
- beschädigter globaler Routingindex,
- zuverlässiges Security-Event wird stillschweigend verloren,
- Kernelcallback wird nach Modulentladung ausgeführt,
- Event-Bus-Rekursion überschreitet sichere Grenzen.

Abhängig von der Auswirkung wird das Subsystem isoliert oder die Kernel-Panic-Policy angewendet.

## 62. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Registrierung eines Eventschemas,
2. Ablehnung einer doppelten Event Type ID,
3. synchrone Kernelzustellung,
4. asynchrone Queuezustellung,
5. IPC-Zustellung an Userspace,
6. signalisierbare Subscription,
7. Filter nach Eventtyp,
8. Filter nach Objekt,
9. Filter nach Graph-Subtree,
10. Capability-gefilterte Sicht,
11. gültige Payload,
12. ungültige Payloadgröße,
13. unbekanntes zwingendes Feld,
14. Eventreihenfolge derselben Quelle,
15. parallele Publisher auf mehreren CPUs,
16. Queue-Backpressure,
17. Queueüberlauf und Overflow-Ereignis,
18. Event Coalescing,
19. zuverlässige Zustellung,
20. gültige Bestätigung,
21. ungültiges Ack-Token,
22. Bestätigungs-Timeout,
23. begrenzte Wiederholung,
24. Subscriberprozess-Absturz,
25. Subscription-Schließung während Zustellung,
26. Early-Boot-Event,
27. Interruptkontext-Veröffentlichung,
28. Panic-Modus,
29. KOG-Lifecycle-Ereignis,
30. Device- und Power-Ereignis,
31. Security-Event mit Auditweitergabe,
32. Eventgeneration und veraltetes Event,
33. Replayable Event,
34. Sticky State,
35. Rate- und Ressourcenlimits,
36. Modulcallback und Modulentladung,
37. Datenschutz-Redaction,
38. korrekte Diagnose und Statistik.

## 63. Verbindliche Invarianten

1. Jeder veröffentlichte Eventtyp besitzt ein registriertes Schema.
2. Event Records werden vollständig oder gar nicht sichtbar.
3. Publisheridentitäten stammen aus dem Kernel-Sicherheitskontext.
4. Subscriber erhalten nur Ereignisse innerhalb ihrer Capability- und Graphsicht.
5. Event-Payloads enthalten keine direkt nutzbaren Kernelzeiger.
6. Queueüberläufe und Eventverlust werden eindeutig angezeigt.
7. Synchrone Kernelcallbacks blockieren nicht unbegrenzt.
8. Direkte Interruptpublisher warten nicht auf Userspace-Queues.
9. Ack-Tokens können höchstens einmal erfolgreich verwendet werden.
10. Eine geschlossene Subscription erhält keine neuen Ereignisse.
11. Eventprioritäten können nicht unautorisiert erhöht werden.
12. Normale Eventqueues ersetzen nicht die Sicherheitsgarantien des Audit Subsystems.

## 64. Referenzablauf einer asynchronen Zustellung

```text
Publisher erzeugt Event
  -> Event Type ID und Schema auflösen
  -> Publisher-Capability prüfen
  -> Payloadgröße und Felder validieren
  -> Event Record vollständig erzeugen
  -> passende Subscriptions aus Routingindex bestimmen
  -> Graph-, Namespace- und Security-Filter anwenden
  -> Payload pro Empfänger redigieren
  -> Queuekapazität und Priorität prüfen
  -> Event atomar in Zielqueue veröffentlichen
  -> wartenden Subscriber aufwecken
  -> bei Overflow Verluststatus erzeugen
  -> Statistik und gegebenenfalls Audit aktualisieren
```

## 65. Zusammenfassung

Der NovaOS Event Bus bildet die zentrale ereignisgesteuerte Verbindung zwischen Kernelkomponenten, Kernel Object Graph und autorisierten Userspace-Diensten.

Typisierte und versionierte Eventschemas verhindern unstrukturierte oder ungültige Payloads. Capability-, Namespace- und Graphfilter begrenzen Sichtbarkeit und Zustellung, während CPU-lokale Queues und sharded Routingindizes SMP-Skalierbarkeit ermöglichen.

Begrenzte Queues, Backpressure, Coalescing, Bestätigungen und eindeutige Overflow-Ereignisse verhindern stillen Zustandsverlust. Sicherheits-Audit, hochfrequentes Tracing und allgemeine IPC bleiben bewusst getrennte Systeme.