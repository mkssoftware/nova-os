# NPSPEC-DISTCOMM-BACKPRESSURE-0001 – Nova Distributed Backpressure

## Status

Angenommen

## Kategorie

Distributed Communication / Flow Control / Backpressure

## Zweck

NovaOS definiert ein gemeinsames Backpressure-Modell für verteilte Kommunikation.

Es verhindert, dass schnelle Produzenten langsamere Empfänger, Dienste, Queues oder Netzwerkpfade unbegrenzt mit Daten überlasten.

```text
Producer
   ↓
Distributed Communication
   ↓
Consumer

Producer Rate > Consumer Capacity
              ↓
         Backpressure
              ↓
   Slow / Wait / Reject / Drop
```

Backpressure muss möglichst bis zur tatsächlichen Quelle der Last propagiert werden können.

## Grundprinzip

```text
Backpressure ≠ Failure
Queue ≠ Infinite Buffer
Accepted ≠ Processed
Network Capacity ≠ Consumer Capacity
High Priority ≠ Unlimited Capacity
```

Puffer dürfen kurzfristige Unterschiede ausgleichen, aber keine dauerhaft fehlende Verarbeitungskapazität verbergen.

## Backpressure-Modell

Ein Kommunikationsfluss darf besitzen:

```text
BackpressureState
├── FlowID
├── ProducerID
├── ConsumerID
├── State
├── Queue Pressure
└── Available Capacity
```

Optional:

```text
Priority
QoS
Deadline
Resource Budget
ExecutionContract
Network Path
Retry Policy
```

## Zustände

NovaOS soll mindestens folgende Zustände unterscheiden können:

```text
Normal
   ↓
Pressure
   ↓
Throttled
   ↓
Saturated
```

Nach Entlastung:

```text
Saturated
   ↓
Recovering
   ↓
Normal
```

Zustandswechsel sollen hysterese- oder schwellenwertbasiert erfolgen können, damit schnelle Oszillation vermieden wird.

## Ende-zu-Ende-Propagation

Backpressure soll über mehrere Kommunikationsschichten propagiert werden können.

```text
Consumer
   ↑
Queue
   ↑
Message Bus
   ↑
RPC / Stream
   ↑
Transport
   ↑
Producer
```

Eine Zwischenschicht darf Überlastung nicht dauerhaft durch unbegrenztes Puffern verbergen.

## Reaktionen

Abhängig von Kommunikationsmodell und Policy darf Backpressure folgende Reaktionen auslösen:

```text
Wait
Throttle
Reduce Rate
Reduce Concurrency
Reject
Drop
Disconnect
Degrade Quality
```

Die Reaktion muss zur Semantik der Kommunikation passen.

Verlustfreie Kommunikation darf Daten nicht stillschweigend verwerfen.

## Streaming

Streaming-Protokolle müssen Empfängerkapazität berücksichtigen.

```text
Producer
   ↓
Window / Credit
   ↓
Consumer
```

Credit-, Window- oder Demand-basierte Modelle dürfen verwendet werden.

Ein Producer darf nur innerhalb der verfügbaren Kapazität senden, wenn das jeweilige Protokoll dies verlangt.

## RPC

Bei Distributed RPC darf Backpressure insbesondere begrenzen:

```text
Concurrent Calls
Pending Calls
Streams
Request Rate
Response Rate
```

Neue Aufrufe dürfen verzögert oder kontrolliert abgelehnt werden.

## Message Bus, Pub/Sub und Queue

Messaging-Systeme müssen Queue-Druck sichtbar machen können.

```text
Producer
   ↓
Queue Growth
   ↓
Pressure
   ↓
Producer Backpressure
```

Langsame Subscriber oder Consumer dürfen keine unbegrenzte Speicherbelegung verursachen.

Policies dürfen festlegen:

```text
Throttle Producer
Bound Queue
Drop Oldest
Drop Newest
Reject Publish
Disconnect Subscriber
```

## Netzwerk-Integration

Distributed Backpressure muss mit Netzwerkmechanismen zusammenarbeiten.

```text
Application Backpressure
        ↕
Transport Flow Control
        ↕
Congestion Control
        ↕
Network Path
```

Dabei gilt:

```text
Backpressure ≠ Flow Control
Backpressure ≠ Congestion Control
```

Die Mechanismen dürfen Signale austauschen, bleiben aber logisch getrennt.

## QoS und Prioritäten

Backpressure darf Prioritäten berücksichtigen.

```text
Realtime
Interactive
Normal
Background
```

Höhere Priorität darf bevorzugte Ressourcen erhalten.

Sie darf jedoch keine unbegrenzte Ressourcenbelegung oder vollständige dauerhafte Verdrängung anderer zulässiger Kommunikation verursachen.

## Deadlines

Operationen mit Deadline dürfen bei Überlastung frühzeitig abgelehnt werden, wenn eine rechtzeitige Verarbeitung nicht mehr realistisch möglich ist.

```text
Estimated Completion > Deadline
              ↓
        Early Rejection
```

Dadurch wird unnötige Arbeit vermieden.

## Resource Budgets

Backpressure muss mit Nova Resource Economy integrierbar sein.

Grenzen dürfen gelten für:

```text
Memory
Buffers
Queue Depth
Bandwidth
Concurrent Operations
CPU Time
Persistent Storage
```

Das Erreichen eines Budgets muss kontrollierte Gegenmaßnahmen auslösen können.

## Verteilte Fehler

Netzwerkunterbrechungen oder langsame Gegenstellen dürfen nicht automatisch unbegrenztes lokales Puffern verursachen.

```text
Remote Unavailable
       ↓
Bounded Buffer
       ↓
Wait / Reject / Retry Policy
```

Retries müssen ebenfalls Backpressure berücksichtigen.

Retry-Stürme sind zu vermeiden.

## Sicherheit

Nicht vertrauenswürdige Gegenstellen dürfen Backpressure nicht verwenden können, um unbegrenzt Ressourcen zu reservieren.

NovaOS muss insbesondere gegen:

```text
Slow Consumer Attacks
Queue Exhaustion
Connection Exhaustion
Retry Amplification
Buffer Exhaustion
```

begrenzen können.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
FlowID
Backpressure State
Queue Depth
Available Capacity
Producer Rate
Consumer Rate
Buffer Usage
Throttle State
Dropped / Rejected Count
Pressure Reason
```

Damit muss erkennbar sein, wo eine Kommunikationskette tatsächlich begrenzt wird.

## Normative Anforderungen

1. NovaOS MUSS Distributed Backpressure unterstützen.
2. Backpressure SOLL möglichst bis zum ursprünglichen Producer propagiert werden.
3. Kommunikationspuffer MÜSSEN begrenzt sein.
4. Zwischenschichten DÜRFEN Überlastung NICHT durch unbegrenztes Puffern verbergen.
5. Streaming MUSS Empfängerkapazität berücksichtigen können.
6. RPC MUSS konkurrierende und ausstehende Aufrufe begrenzen können.
7. Messaging-Systeme MÜSSEN Queue-Druck kontrolliert behandeln.
8. Verlustfreie Kommunikation DARF Daten NICHT stillschweigend verwerfen.
9. Backpressure MUSS mit QoS und Resource Budgets integrierbar sein.
10. Retries DÜRFEN Backpressure NICHT umgehen.
11. Nicht vertrauenswürdige Gegenstellen DÜRFEN keine unbegrenzten Ressourcen reservieren.
12. Backpressure-Zustände und Ursachen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IPC-BACKPRESSURE-0001`
- `NPSPEC-DISTCOMM-RPC-0001`
- `NPSPEC-DISTCOMM-MESSAGEBUS-0001`
- `NPSPEC-DISTCOMM-PUBSUB-0001`
- `NPSPEC-DISTCOMM-QUEUE-0001`
- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-NETWORK-QOS-0001`
- `NPSPEC-NETWORK-CONGESTION-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-DISTCOMM-0011`

## Ergebnis

```text
Consumer Capacity
        ↑
Distributed Backpressure
        ↑
Queue / RPC / Messaging
        ↑
Transport
        ↑
Producer
```

NovaOS erhält damit ein durchgängiges Backpressure-Modell, das Überlastung nicht lediglich puffert, sondern kontrolliert bis zur Lastquelle zurückmeldet und dadurch Speicher, Netzwerk, CPU und entfernte Dienste vor unkontrollierter Überlastung schützt.