# NPSPEC-DISTCOMM-PUBSUB-0001 – Nova Distributed Publish/Subscribe

## Status

Angenommen

## Kategorie

Distributed Communication / Messaging / Publish-Subscribe

## Zweck

NovaOS definiert eine verteilte Publish/Subscribe-Infrastruktur für asynchrone Kommunikation zwischen Produzenten und Konsumenten über Prozess-, System- und Netzwerkgrenzen hinweg.

```text
Publisher
    ↓
Topic
 ┌──┼──┐
 ↓  ↓  ↓
S1  S2  S3
```

Publisher müssen die konkreten Subscriber weder kennen noch direkt adressieren.

## Grundprinzip

```text
Publish ≠ Delivery
Delivery ≠ Processing
Subscription ≠ Permission
Topic Name ≠ Authority
Subscriber ≠ Network Location
```

Publish/Subscribe dient der logischen Entkopplung von Sendern und Empfängern.

## Pub/Sub-Modell

Ein Topic besitzt eine stabile logische Identität.

```text
Topic
├── TopicID
├── Name
├── Message Type
├── Policy
└── State
```

Eine Subscription besitzt:

```text
Subscription
├── SubscriptionID
├── TopicID
├── SubscriberID
├── State
└── Delivery Policy
```

Optional:

```text
Filter
QoS
Security Context
ExecutionContract
Resource Budget
Network Intent
```

`TopicID` und `SubscriptionID` dürfen nicht an Broker, IP-Adresse oder physischen Standort gebunden sein.

## Publisher

Publisher veröffentlichen typisierte Nachrichten auf einem Topic.

```text
Publisher
    ↓
Publish(Message)
    ↓
Topic
```

Ein Publisher benötigt eine entsprechende Publish-Berechtigung.

Das Wissen über die Topic-ID oder den Topic-Namen erzeugt keine Berechtigung.

## Subscriber

Subscriber registrieren eine Subscription für ein Topic.

```text
Topic
   ↓
Subscription
   ↓
Subscriber
```

Mehrere Subscriber dürfen dasselbe Topic unabhängig konsumieren.

Subscriber dürfen unterschiedliche Delivery-, Filter- und Ressourcenregeln besitzen.

## Typisierte Topics

Topics sollen definierte Nachrichtentypen besitzen.

```text
Topic<T>
   ↓
Message<T>
```

Schema und Version müssen überprüfbar sein.

Inkompatible Nachrichtentypen müssen kontrolliert abgelehnt werden.

## Zustellung

Subscriptions dürfen unterschiedliche Zustellungsmodelle definieren:

```text
Transient
Buffered
Durable
```

Zusätzlich müssen Zustellungssemantiken wie:

```text
At-Most-Once
At-Least-Once
```

ausdrückbar sein.

`Exactly-Once` darf nur verwendet werden, wenn die gesamte relevante Verarbeitungskette diese Semantik tatsächlich erfüllt.

## Acknowledgement

Durable oder bestätigungspflichtige Subscriptions dürfen explizite Acknowledgements verwenden.

```text
Publish
   ↓
Deliver
   ↓
Process
   ↓
Acknowledge
```

Fehlende Bestätigung darf entsprechend der Delivery Policy eine erneute Zustellung auslösen.

## Filter

Subscriptions dürfen Nachrichten filtern.

```text
Topic
  ↓
Filter
  ↓
Subscription
```

Filter sollen auf strukturierten Metadaten oder typisierten Feldern basieren.

Filter dürfen keine Sicherheitsprüfung ersetzen.

## Ordering

Ordering muss explizit definiert werden.

Unterstützt werden können:

```text
No Ordering
Per-Publisher Ordering
Per-Key Ordering
Per-Topic Ordering
```

Eine globale Reihenfolge darf nicht vorausgesetzt werden.

## Backpressure

Langsame Subscriber dürfen das System nicht unbegrenzt mit Nachrichten füllen.

```text
Publisher
    ↓
Topic
    ↓
Slow Subscriber
    ↓
Backpressure Policy
```

Mögliche Reaktionen:

```text
Throttle
Buffer
Drop
Reject
Disconnect
```

Die konkrete Reaktion wird durch Topic- und Subscription-Policy bestimmt.

## Verteilte Topologie

Topics dürfen über mehrere Systeme verteilt werden.

```text
Publisher
    ↓
Node A
    ↓
Distributed Topic
    ↓
Node B
    ↓
Subscriber
```

Die logische Topic-Identität bleibt unabhängig vom aktuellen Standort.

Replikation oder Migration dürfen diese Identität nicht verändern.

## Sicherheit

Berechtigungen müssen mindestens getrennt werden können in:

```text
Publish
Subscribe
Consume
Acknowledge
Administer
```

Security Policy muss sowohl Publisher als auch Subscriber prüfen.

Transportverschlüsselung ersetzt keine Topic-Autorisierung.

## Network Intent und Sovereignty

Verteilte Zustellung muss Network Intent und Sovereignty berücksichtigen.

```text
Message
   ↓
Topic
   ↓
Network Intent
   ↓
Allowed Network Path
```

Ein alternativer Pfad darf keine Security-, Trust- oder Sovereignty-Anforderung verletzen.

## Fehlerbehandlung

Fehlgeschlagene Zustellungen müssen kontrolliert behandelt werden.

```text
Delivery Failed
     ↓
Retry / Delay / Dead-Letter / Drop
```

Retries müssen begrenzt sein.

Fehlerhafte Subscriber dürfen die Zustellung an unabhängige Subscriber nicht unnötig blockieren.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Topics
Subscriptions
Queued Messages
Message Size
Persistent Storage
Buffers
Retries
Bandwidth
Subscriber State
```

Ressourcenbudgets dürfen pro Topic, Publisher, Subscriber oder Sicherheitsdomäne gelten.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
TopicID
SubscriptionID
Publisher
Subscriber
Message Type
Queue Depth
Delivery State
Delivery Count
Acknowledgement State
Dropped Messages
Failure Reason
```

Payloads und sensible Metadaten dürfen nur autorisiert sichtbar sein.

## Normative Anforderungen

1. NovaOS MUSS verteiltes Publish/Subscribe unterstützen können.
2. Topic-Identität MUSS unabhängig vom Netzwerkstandort sein.
3. Publish-, Subscribe- und Consume-Berechtigungen MÜSSEN getrennt kontrollierbar sein.
4. Topics SOLLEN typisierte Nachrichten verwenden.
5. Mehrere Subscriber MÜSSEN unabhängig dasselbe Topic konsumieren können.
6. Zustellungssemantik MUSS explizit definierbar sein.
7. Ordering DARF NICHT stärker garantiert werden als tatsächlich umgesetzt.
8. Durable Subscriptions SOLLEN Acknowledgements unterstützen.
9. Pub/Sub MUSS Backpressure unterstützen.
10. Retries, Queues und Persistenz MÜSSEN ressourcenbegrenzt sein.
11. Network-, Security- und Sovereignty-Regeln DÜRFEN NICHT umgangen werden.
12. Topic-, Subscription- und Zustellungszustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IPC-PUBSUB-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-IPC-BACKPRESSURE-0001`
- `NPSPEC-DISTCOMM-MESSAGEBUS-0001`
- `NPSPEC-DISTCOMM-TLS-0001`
- `NPSPEC-NETWORK-INTENT-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-DISTCOMM-0005`

## Ergebnis

```text
Publisher(s)
     ↓
Typed Topic
     ↓
Nova Distributed Pub/Sub
     ↓
Subscriptions
  ┌────┼────┐
  ↓    ↓    ↓
 S1   S2   S3
```

NovaOS erhält damit eine verteilte Publish/Subscribe-Infrastruktur, die Publisher und Subscriber räumlich und zeitlich entkoppelt und dabei Typisierung, Zustellung, Backpressure, Sicherheit, Ressourcensteuerung und Location Transparency in das gemeinsame NovaOS-Kommunikationsmodell integriert.