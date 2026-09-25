# NPSPEC-DISTCOMM-MESSAGEBUS-0001 – Nova Distributed Message Bus

## Status

Angenommen

## Kategorie

Distributed Communication / Messaging / Message Bus

## Zweck

NovaOS definiert einen verteilten Message Bus für asynchrone Kommunikation zwischen Diensten über Prozess-, System- und Netzwerkgrenzen hinweg.

```text
Producer
   ↓
Message Bus
   ↓
Consumer(s)
```

Der Message Bus ergänzt direkte IPC- und RPC-Kommunikation um entkoppelte, nachrichtenbasierte Kommunikation.

## Grundprinzip

```text
Message Sent ≠ Message Delivered
Delivered ≠ Processed
Processed ≠ Committed
Message Bus ≠ Authorization
Service Identity ≠ Network Location
```

Zustellung, Verarbeitung und fachliche Bestätigung müssen getrennt betrachtet werden.

## Nachrichtenmodell

Eine Nachricht besitzt mindestens:

```text
DistributedMessage
├── MessageID
├── Type
├── Payload
├── Producer
├── Timestamp
└── State
```

Optional:

```text
CorrelationID
CausationID
Destination
ReplyTarget
Priority
Deadline
Security Context
ExecutionContract
Network Intent
Trace Context
```

`MessageID` muss unabhängig von Transport, Broker und Netzwerkstandort sein.

## Kommunikationsmodelle

Der Message Bus soll mehrere Muster unterstützen können:

```text
Point-to-Point
Publish / Subscribe
Request / Reply
Event Distribution
Work Queue
```

Die konkrete Transportimplementierung darf die logische Nachrichtensemantik nicht bestimmen.

## Topics und Queues

Nachrichten dürfen über logische Ziele adressiert werden.

```text
Message
   ↓
Topic / Queue
   ↓
Consumer(s)
```

Topics und Queues besitzen stabile logische Identitäten und dürfen unabhängig vom physischen Standort ihrer Implementierung sein.

## Zustellungssemantik

NovaOS muss unterschiedliche Zustellungsmodelle ausdrücken können:

```text
At-Most-Once
At-Least-Once
```

`Exactly-Once` darf nur angegeben werden, wenn die gesamte relevante Verarbeitungskette diese Semantik tatsächlich garantiert.

```text
Exactly-Once Delivery ≠ Exactly-Once Effect
```

Für fachlich einmalige Effekte sollen Idempotenz und Transaktionen verwendet werden.

## Acknowledgement

Nachrichten dürfen explizit bestätigt werden.

```text
Deliver
   ↓
Process
   ↓
Acknowledge
```

Ein Acknowledgement darf je nach Policy erst nach erfolgreicher Verarbeitung oder Commit erfolgen.

Fehlende Bestätigungen dürfen kontrollierte erneute Zustellung auslösen.

## Ordering

Nachrichtenreihenfolge muss explizit definiert werden.

```text
Global Ordering      → teuer / optional
Per-Topic Ordering   → optional
Per-Key Ordering     → bevorzugt möglich
No Ordering          → zulässig
```

NovaOS darf keine globale Reihenfolge vortäuschen, wenn diese technisch nicht garantiert wird.

## Persistenz

Nachrichten dürfen abhängig von Policy:

```text
Transient
Buffered
Durable
```

sein.

Durable Messaging muss Crash- und Recovery-Semantik eindeutig definieren.

Nicht jede Nachricht muss persistent gespeichert werden.

## Backpressure

Der Message Bus muss Überlastung kontrolliert behandeln.

```text
Producer
   ↓
Queue Pressure
   ↓
Backpressure
   ↓
Throttle / Wait / Reject
```

Unbegrenztes Puffern ist nicht zulässig.

Backpressure muss bis zum Producer propagiert werden können.

## Fehlerbehandlung

Nicht verarbeitbare Nachrichten müssen kontrolliert behandelt werden.

Mögliche Aktionen:

```text
Retry
Delay
Reject
Dead-Letter
Quarantine
Drop
```

Retry-Versuche müssen begrenzt sein.

Fehlerhafte Nachrichten dürfen keine unendlichen Wiederholungsschleifen verursachen.

## Sicherheit

Producer und Consumer müssen unabhängig autorisiert werden können.

```text
Producer
   ↓
Publish Capability
   ↓
Topic
   ↓
Consume Capability
   ↓
Consumer
```

Das Wissen über einen Topic- oder Queue-Namen erzeugt keine Berechtigung.

Capabilities dürfen auf Operationen wie:

```text
Publish
Consume
Subscribe
Acknowledge
Administer
```

begrenzt werden.

## Netzwerk und Location Transparency

Verteilte Nachrichten dürfen über unterschiedliche Netzwerkpfade transportiert werden.

```text
Logical Message
      ↓
Message Bus
      ↓
Network Intent
      ↓
Transport
```

Routing, VPN, Sovereignty und Security Policy müssen berücksichtigt werden.

Location Transparency darf Sicherheitsgrenzen nicht verbergen.

## Transaktionen

Message Bus und NovaOS-Transaktionen sollen koordinierbar sein.

Beispiel:

```text
Receive Message
      ↓
Modify State
      ↓
Publish Message
      ↓
Commit
```

Teilweise ausgeführte Operationen dürfen nicht fälschlich als vollständig abgeschlossen dargestellt werden.

Eine globale verteilte ACID-Transaktion ist nicht grundsätzlich erforderlich.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Queues
Topics
Pending Messages
Message Size
Subscribers
Persistent Storage
Retries
Connections
Buffers
```

Resource Budgets dürfen pro Dienst, Namespace oder Sicherheitsdomäne gelten.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
MessageID
Type
Producer
Destination
State
Delivery Count
Acknowledgement State
Queue Depth
Consumer State
Timestamp
Failure Reason
```

Payloads und sensible Metadaten dürfen nur entsprechend der jeweiligen Berechtigung sichtbar sein.

## Normative Anforderungen

1. NovaOS MUSS einen verteilten Message Bus unterstützen können.
2. Nachrichtenidentität MUSS unabhängig vom Transport sein.
3. Message Bus und Autorisierung MÜSSEN getrennt bleiben.
4. Point-to-Point und Publish/Subscribe SOLLEN unterstützt werden.
5. Zustellungssemantik MUSS explizit definierbar sein.
6. `Exactly-Once` DARF NICHT behauptet werden, wenn nur Zustellung, nicht aber der Effekt garantiert wird.
7. Acknowledgements MÜSSEN explizit modellierbar sein.
8. Nachrichtenreihenfolge DARF NICHT stärker garantiert werden als technisch umgesetzt.
9. Backpressure MUSS unterstützt werden.
10. Retries und Queue-Größen MÜSSEN begrenzt sein.
11. Network-, Security- und Sovereignty-Regeln DÜRFEN NICHT umgangen werden.
12. Nachrichten-, Queue- und Zustellungszustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IPC-EVENTBUS-0001`
- `NPSPEC-IPC-PUBSUB-0001`
- `NPSPEC-IPC-BACKPRESSURE-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-DISTCOMM-RPC-0001`
- `NPSPEC-DISTCOMM-TLS-0001`
- `NPSPEC-NETWORK-INTENT-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-DISTCOMM-0004`

## Ergebnis

```text
Producer
   ↓
Typed Message
   ↓
Nova Distributed Message Bus
   ↓
Queue / Topic
   ↓
Secure Transport
   ↓
Consumer(s)
```

NovaOS erhält damit eine verteilte, asynchrone Kommunikationsinfrastruktur für entkoppelte Dienste, die Zustellung, Backpressure, Sicherheit, Transaktionen und Fehlerzustände explizit behandelt, ohne verteilte Kommunikation als garantiert erfolgreiche lokale Kommunikation darzustellen.