# NPSPEC-IPC-EVENTBUS-0001 – Nova IPC Event Bus

## Status

Angenommen

## Kategorie

IPC / Event Bus / Asynchronous Communication

## Zweck

NovaOS definiert einen Event Bus für asynchrone, entkoppelte Kommunikation zwischen Systemkomponenten.

```text
Publisher
    ↓
Event Bus
  ↙   ↓   ↘
Sub A Sub B Sub C
```

Publisher müssen die Empfänger eines Events nicht kennen. Der Event Bus übernimmt Routing, Berechtigungsprüfung und Zustellung.

## Grundprinzip

```text
Event ≠ Command
Publish ≠ Delivery Guarantee
Subscription ≠ Authority
Event Bus ≠ Global Broadcast
```

Events beschreiben eingetretene Zustandsänderungen oder Ereignisse. Direkte Befehle und Request/Response-Kommunikation sollen weiterhin über dafür vorgesehene IPC-Mechanismen erfolgen.

## Event-Modell

Ein Event besitzt eine eindeutig definierte Struktur.

```text
Event
├── EventID
├── EventType
├── Source
├── Timestamp
└── Payload
```

Optional:

```text
Sequence
CorrelationID
Semantic Type
Priority
Security Context
```

## Event Types

Events müssen typisierbar und versionierbar sein.

Beispiele:

```text
Device.Added
Device.Removed
Storage.Mounted
Process.Terminated
Network.Changed
Power.StateChanged
```

Eventtypen sollen hierarchische oder namespaced Bezeichnungen unterstützen.

## Publish

Publisher senden Events an den Event Bus.

```text
Publisher
   ↓
Publish(Event)
   ↓
Validation
   ↓
Event Bus
```

Vor Annahme eines Events müssen Typ, Publisher-Berechtigung und grundlegende Struktur validierbar sein.

Ein Prozess darf nicht allein aufgrund seiner Bus-Verbindung beliebige Systemevents erzeugen.

## Subscription

Empfänger registrieren Subscriptions.

```text
Subscriber
    ↓
Subscription
    ↓
Event Filter
```

Eine Subscription kann beispielsweise filtern nach:

```text
Event Type
Source
Namespace
Semantic Type
Security Context
```

Subscriptions dürfen nur Events sichtbar machen, für die der Subscriber autorisiert ist.

## Routing

Der Event Bus verteilt Events nur an passende Subscriptions.

```text
Event
  ↓
Routing
 ├→ Subscriber A
 ├→ Subscriber B
 └→ Subscriber C
```

Nicht passende oder nicht autorisierte Subscriber erhalten das Event nicht.

## Capability-Modell

Publishing und Subscription müssen durch Capabilities kontrollierbar sein.

```text
PublishCapability<EventType>
SubscribeCapability<EventType>
```

Eine Subscribe-Capability darf keine Publish-Rechte erzeugen und umgekehrt.

Capabilities müssen auf Eventtypen oder Namespaces begrenzbar sein.

## Zustellung

NovaOS soll unterschiedliche Zustellungsmodelle unterstützen können:

```text
Best Effort
Reliable
Persistent
```

Die gewünschte Semantik muss explizit definiert werden.

`Publish Success` bedeutet nicht automatisch, dass alle Subscriber das Event verarbeitet haben.

## Ordering

Event-Reihenfolge darf nur garantiert werden, wenn dies für den jeweiligen Eventstrom definiert wurde.

```text
Source A:
E1 → E2 → E3
```

Eine globale Reihenfolge aller Systemevents darf nicht vorausgesetzt werden.

Sequence-IDs dürfen zur Erkennung von Reihenfolge oder Verlust verwendet werden.

## Backpressure

Subscriber dürfen den Event Bus nicht unbegrenzt blockieren.

```text
Publisher
   ↓
Event Bus
   ↓
Bounded Subscriber Queue
```

Bei Überlastung müssen abhängig von der Eventklasse definierte Maßnahmen möglich sein:

```text
Throttle
Drop
Disconnect
Persist
Coalesce
```

Kritische Events dürfen nicht stillschweigend verloren gehen, wenn ihre Zustellungssemantik dies verbietet.

## Event Coalescing

Häufige gleichartige Events dürfen zusammengefasst werden, sofern ihre Semantik dies erlaubt.

```text
Changed
Changed
Changed
   ↓
Changed
```

Sicherheitskritische oder verlustfrei erforderliche Events dürfen nicht ohne explizite Freigabe zusammengefasst werden.

## Prozess- und Service-Lifecycle

Beim Ende eines Subscribers müssen dessen Subscriptions automatisch entfernt oder deaktiviert werden können.

```text
Subscriber Terminated
        ↓
Subscription Removed
```

Publisher-Ausfall darf den Event Bus selbst nicht blockieren.

## Sicherheit

Der Event Bus muss mindestens prüfen können:

```text
Publisher Identity
Publish Capability
Event Type
Subscriber Capability
Event Visibility
Security Context
```

Sensitive Payloads dürfen nur autorisierten Subscribern zugestellt werden.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Event Type
Publisher
Subscription Count
Queue Depth
Published Events
Delivered Events
Dropped Events
Errors
```

Event-Inhalte dürfen dabei nicht ohne entsprechende Berechtigung offengelegt werden.

## Normative Anforderungen

1. NovaOS SOLL einen zentral kontrollierten IPC Event Bus bereitstellen.
2. Events MÜSSEN eindeutig typisierbar sein.
3. Eventtypen MÜSSEN versionierbar sein können.
4. Publishing MUSS durch explizite Berechtigungen kontrollierbar sein.
5. Subscriptions MÜSSEN durch Capabilities begrenzbar sein.
6. Subscribe-Rechte DÜRFEN NICHT automatisch Publish-Rechte erzeugen.
7. Events DÜRFEN nur autorisierten und passenden Subscriptions zugestellt werden.
8. Zustellungsgarantien MÜSSEN explizit definiert werden.
9. Eine globale Event-Reihenfolge DARF NICHT vorausgesetzt werden.
10. Subscriber-Queues MÜSSEN begrenzbar sein und Backpressure unterstützen.
11. Event Coalescing DARF nur bei kompatibler Event-Semantik erfolgen.
12. Event-Bus-Zustand und Ressourcenverbrauch SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IPC-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-IPC-CHANNEL-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-IPC-0007`

## Ergebnis

```text
Publisher
   ↓
Typed Event
   ↓
Capability Check
   ↓
Event Bus
   ↓
Filter + Routing
   ↓
Authorized Subscribers
```

NovaOS erhält damit einen kontrollierten Event-Bus-Mechanismus für asynchrone Many-to-Many-Kommunikation, ohne Publisher und Subscriber direkt miteinander zu koppeln oder Sicherheits- und Ressourcengrenzen aufzugeben.