# NPSPEC-IPC-BACKPRESSURE-0001 – Nova IPC Backpressure

## Status

Angenommen

## Kategorie

IPC / Flow Control / Resource Management

## Zweck

NovaOS definiert Backpressure als grundlegenden IPC-Mechanismus zur kontrollierten Reaktion auf überlastete oder langsamere Empfänger.

```text
Producer
   ↓
Bounded IPC Queue
   ↓
Consumer
```

Ein schneller Sender darf keinen unbegrenzten Speicherverbrauch oder unkontrolliertes Queue-Wachstum verursachen.

## Grundprinzip

```text
Backpressure ≠ Failure
Queue Full ≠ Data Loss
Priority ≠ Unlimited Capacity
Slow Consumer ≠ Unlimited Buffering
```

IPC-Ressourcen sind grundsätzlich endlich und müssen kontrolliert verwaltet werden.

## Begrenzte Queues

IPC-Queues müssen definierte Kapazitätsgrenzen besitzen können.

```text
Queue
├── Message Limit
├── Byte Limit
├── Memory Budget
└── Pending Operation Limit
```

Unbegrenzte Queues dürfen nicht als Standardmechanismus verwendet werden.

## Zustände

Ein Kommunikationspfad soll mindestens folgende Lastzustände unterscheiden können:

```text
Normal
   ↓
Pressure
   ↓
Saturated
```

Optional können zusätzliche Schwellenwerte definiert werden.

Der Zustand soll aus tatsächlicher Queue- und Ressourcenbelegung abgeleitet werden.

## Backpressure-Reaktionen

Bei Ressourcenknappheit müssen abhängig von der IPC-Semantik unterschiedliche Reaktionen möglich sein:

```text
Wait
Throttle
Reject
Drop
Coalesce
Persist
```

Die gewählte Strategie muss zur jeweiligen Kommunikationssemantik passen.

## Wait

Ein Sender darf auf freie Kapazität warten.

```text
Queue Full
   ↓
Wait
   ↓
Capacity Available
   ↓
Continue
```

Warten muss mit Cancellation und gegebenenfalls Deadlines kombinierbar sein.

Unbegrenztes Warten darf nicht vorausgesetzt werden.

## Throttling

NovaOS darf Sender kontrolliert verlangsamen.

```text
High Producer Rate
       ↓
Throttle
       ↓
Acceptable Rate
```

Throttling soll möglichst den verursachenden Kommunikationspfad betreffen und nicht unnötig unabhängige IPC-Verbindungen beeinträchtigen.

## Reject

Neue Nachrichten oder Calls dürfen kontrolliert abgelehnt werden.

```text
Submit
  ↓
No Capacity
  ↓
Backpressure / ResourceLimit
```

Der Sender muss die Ablehnung eindeutig erkennen können.

## Drop

Nachrichten dürfen nur verworfen werden, wenn die jeweilige IPC-Semantik dies ausdrücklich erlaubt.

Geeignet können beispielsweise sein:

```text
Telemetry
Transient Status Updates
Replaceable Events
```

Nicht ersetzbare Requests oder sicherheitskritische Nachrichten dürfen nicht stillschweigend verworfen werden.

## Coalescing

Mehrere ersetzbare Nachrichten dürfen zusammengefasst werden.

```text
State A
State B
State C
   ↓
State C
```

Coalescing ist nur zulässig, wenn Zwischenzustände semantisch nicht erforderlich sind.

## Propagation

Backpressure soll entlang einer Kommunikationskette propagiert werden können.

```text
Service A
   ↓
Service B
   ↓
Service C overloaded

C → Pressure → B → Pressure → A
```

Dadurch soll verhindert werden, dass Überlastung lediglich in vorgelagerte Queues verschoben wird.

## Priorität und QoS

Priorität darf beeinflussen, welche Arbeit bevorzugt verarbeitet wird.

```text
Realtime
Interactive
Normal
Background
```

Hohe Priorität darf jedoch keine unbegrenzten Ressourcen erzeugen.

Fairness und definierte Ressourcenbudgets müssen erhalten bleiben.

## Zero-Copy und Shared Memory

Auch Zero-Copy-Kommunikation benötigt Backpressure.

```text
Shared Buffer Pool
       ↓
Buffers Exhausted
       ↓
Backpressure
```

Zero-Copy verhindert Kopien, aber nicht Ressourcenknappheit.

Buffer-, Mapping- und Speicherlimits müssen berücksichtigt werden.

## Deadlines und Cancellation

Wartende IPC-Operationen müssen auf Deadline und Cancellation reagieren können.

```text
Waiting
 ├→ Capacity → Continue
 ├→ Cancel   → Cancelled
 └→ Deadline → DeadlineMiss / Timeout
```

Backpressure darf Cancellation nicht dauerhaft blockieren.

## Ressourcenökonomie

Backpressure muss mindestens folgende Ressourcen berücksichtigen können:

```text
Queue Memory
Shared Buffers
Pending Calls
Mapped Memory
Bandwidth
CPU Processing Capacity
```

Limits dürfen pro Prozess, Endpoint, Channel, Service oder Sicherheitsdomain gelten.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Queue Capacity
Queue Usage
Pressure State
Waiting Senders
Rejected Operations
Dropped Messages
Throttling State
Resource Usage
```

Damit können Scheduler, Services und Diagnosewerkzeuge Überlastung erkennen.

## Normative Anforderungen

1. NovaOS MUSS Backpressure für begrenzte IPC-Ressourcen unterstützen.
2. IPC-Queues MÜSSEN definierte Kapazitätsgrenzen besitzen können.
3. Unbegrenztes Queue-Wachstum DARF NICHT als Standardstrategie verwendet werden.
4. Sender MÜSSEN kontrollierte Ablehnung oder Verzögerung erkennen können.
5. Backpressure MUSS mit Cancellation und Deadlines kombinierbar sein.
6. Nachrichten DÜRFEN nur verworfen werden, wenn ihre Semantik dies erlaubt.
7. Kritische oder nicht ersetzbare Nachrichten DÜRFEN NICHT stillschweigend verworfen werden.
8. Coalescing DARF nur bei semantisch ersetzbaren Nachrichten erfolgen.
9. Backpressure SOLL entlang abhängiger Kommunikationspfade propagierbar sein.
10. Priorität DARF keine unbegrenzten IPC-Ressourcen erzeugen.
11. Shared-Memory- und Zero-Copy-Ressourcen MÜSSEN ebenfalls Backpressure unterstützen.
12. Pressure-Zustand und Ressourcenverbrauch SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IPC-0001`
- `NPSPEC-IPC-CHANNEL-0001`
- `NPSPEC-IPC-PUBSUB-0001`
- `NPSPEC-IPC-RPC-0001`
- `NPSPEC-IPC-CALL-0001`
- `NPSPEC-IPC-ZEROCOPY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-IPC-0011`

## Ergebnis

```text
Producer
   ↓
Bounded Resources
   ↓
Pressure Detection
   ↓
Wait / Throttle / Reject / Coalesce
   ↓
Consumer
```

NovaOS erhält damit einen einheitlichen Backpressure-Mechanismus, der IPC-Überlastung kontrolliert, Ressourcenverbrauch begrenzt und verhindert, dass langsame Empfänger zu unbegrenztem Queue- oder Speicherwachstum führen.