# NPSPEC-IPC-PUBSUB-0001 – Nova IPC Publish/Subscribe

## Status

Entwurf

## Kategorie

IPC / Publish-Subscribe / Asynchronous Communication

## Zweck

NovaOS definiert ein Publish/Subscribe-Modell für asynchrone Many-to-Many-Kommunikation über benannte Topics.

```text
Publishers
    ↓
  Topic
    ↓
Subscribers
```

Publisher und Subscriber bleiben voneinander entkoppelt. Beide kennen primär das Topic und dessen definierten Nachrichtenvertrag.

## Grundprinzip

```text
Publisher ≠ Subscriber
Topic Knowledge ≠ Permission
Publish ≠ Delivery
Subscription ≠ Guaranteed Processing
```

Publish/Subscribe dient dem Austausch von Datenströmen, Zustandsänderungen und Nachrichten zwischen mehreren unabhängigen Teilnehmern.

## Topic-Modell

Ein Topic bildet einen logischen Kommunikationskanal.

```text
Topic
├── TopicID
├── Namespace
├── Message Type
├── Schema Version
└── Policy
```

Optional:

```text
QoS
Retention
Ordering
Queue Limits
Security Context
```

Die Topic-Identität soll unabhängig von Publishern und Subscribern bleiben.

## Publishing

Publisher senden Nachrichten an ein Topic.

```text
Publisher
   ↓
Publish(Message)
   ↓
Topic
```

Vor Annahme müssen mindestens Topic, Nachrichtentyp und Publish-Berechtigung geprüft werden können.

Mehrere Publisher dürfen dasselbe Topic verwenden, sofern dessen Policy dies erlaubt.

## Subscription

Subscriber registrieren sich für ein oder mehrere Topics.

```text
Subscriber
    ↓
Subscription
    ↓
Topic
```

Eine Subscription kann zusätzliche Filter enthalten:

```text
Source
Message Type
Metadata
Semantic Type
```

Filter dürfen keine Nachrichten sichtbar machen, für die der Subscriber nicht autorisiert ist.

## Typed Pub/Sub

Topics sollen typisierte Nachrichtenverträge verwenden.

```text
Topic<DeviceStatus>
Topic<NetworkChange>
Topic<SensorData>
```

Publisher müssen kompatible Nachrichtentypen liefern.

Subscriber müssen die unterstützte Schema-Version bestimmen können.

## Capability-Modell

Publish- und Subscribe-Rechte müssen getrennt kontrollierbar sein.

```text
PublishCapability<Topic>
SubscribeCapability<Topic>
```

Eine Publish-Capability darf keine Subscribe-Rechte erzeugen und umgekehrt.

Capabilities sollen auf einzelne Topics oder Namespaces begrenzbar sein.

## Zustellung

NovaOS soll unterschiedliche Zustellungsmodelle unterstützen können:

```text
Best Effort
Reliable
Persistent
```

Die jeweilige Semantik muss pro Topic oder Subscription definiert werden.

Erfolgreiches Publishing bedeutet nicht automatisch erfolgreiche Verarbeitung durch alle Subscriber.

## Ordering

Ordering muss explizit definiert werden.

NovaOS darf beispielsweise Reihenfolge innerhalb eines Publishers unterstützen:

```text
Publisher A:

M1 → M2 → M3
```

Eine globale Reihenfolge zwischen mehreren Publishern darf nicht vorausgesetzt werden.

## Retention

Topics dürfen Nachrichten zeitweise speichern.

```text
Publish
   ↓
Topic Retention
   ↓
Later Subscriber
```

Retention kann abhängig von Policy definiert werden durch:

```text
Time
Message Count
Storage Budget
No Retention
```

Pub/Sub darf jedoch nicht grundsätzlich persistente Speicherung voraussetzen.

## Backpressure

Langsame Subscriber dürfen Publisher und System nicht unbegrenzt belasten.

```text
Topic
  ↓
Bounded Subscription Queue
  ↓
Subscriber
```

Mögliche Reaktionen:

```text
Throttle
Drop
Coalesce
Disconnect
Persist
```

Die gewählte Reaktion muss mit der Topic-Semantik vereinbar sein.

## Event Bus Integration

Der Nova Event Bus darf auf dem Pub/Sub-Modell aufbauen.

```text
Pub/Sub
   ↓
Event Topics
   ↓
Event Bus
```

Dabei gilt:

```text
Pub/Sub = allgemeines Kommunikationsmodell
Event Bus = systemorientierte Event-Infrastruktur
```

Nicht jedes Pub/Sub-Topic ist automatisch ein Systemevent.

## Lifecycle

Subscriptions müssen einen kontrollierten Lifecycle besitzen.

```text
Create
  ↓
Active
  ↓
Pause
  ↓
Close
```

Beim Prozessende müssen zugehörige Subscriptions automatisch bereinigt werden können.

## Ressourcensteuerung

Pub/Sub-Ressourcen müssen begrenzbar sein.

```text
Topic Count
Subscription Count
Queue Depth
Message Size
Buffered Bytes
Retention Storage
Bandwidth
```

Ein Teilnehmer darf durch Publishing oder Subscription keine unbegrenzten Ressourcen beanspruchen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
TopicID
Message Type
Publishers
Subscribers
Queue Depth
Retention
Published Messages
Delivered Messages
Dropped Messages
Errors
```

Nachrichteninhalte dürfen ohne entsprechende Berechtigung nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS SOLL ein allgemeines Publish/Subscribe-Modell bereitstellen.
2. Topics MÜSSEN eindeutig identifizierbar sein.
3. Topics SOLLEN typisierte und versionierte Nachrichtenverträge unterstützen.
4. Publish- und Subscribe-Rechte MÜSSEN getrennt kontrollierbar sein.
5. Topic-Kenntnis DARF NICHT automatisch Kommunikationsrechte gewähren.
6. Mehrere Publisher und Subscriber MÜSSEN pro Topic möglich sein können.
7. Zustellungssemantik MUSS explizit definierbar sein.
8. Eine globale Nachrichtenreihenfolge DARF NICHT standardmäßig vorausgesetzt werden.
9. Subscriptions MÜSSEN begrenzte Queues und Backpressure unterstützen können.
10. Retention MUSS durch Ressourcenbudgets begrenzbar sein.
11. Prozessende MUSS zugehörige Subscriptions kontrolliert bereinigen können.
12. Topic-, Subscription- und Ressourcenzustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IPC-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-IPC-CHANNEL-0001`
- `NPSPEC-IPC-EVENTBUS-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-IPC-0008`

## Ergebnis

```text
Publisher
    ↓
Typed Topic
    ↓
Capability + Policy
    ↓
Routing
  ↙   ↓   ↘
Sub A Sub B Sub C
```

NovaOS erhält damit ein allgemeines Publish/Subscribe-Modell für entkoppelte Many-to-Many-Kommunikation mit typisierten Topics, getrennten Berechtigungen, definierter Zustellungssemantik und kontrolliertem Ressourcenverbrauch.