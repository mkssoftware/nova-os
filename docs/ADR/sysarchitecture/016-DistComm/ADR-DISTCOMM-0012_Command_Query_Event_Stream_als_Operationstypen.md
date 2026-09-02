# ADR-DISTCOMM-0012 – Command, Query, Event und Stream als Operationstypen

## Status

Angenommen

## Kategorie

Distributed Communication / Operation Semantics / Nova.Call / Messaging / Streaming

## Kontext

`Nova.Call` stellt eine einheitliche Abstraktion für lokale und entfernte Kommunikation bereit.

Nicht jede Kommunikation besitzt jedoch dieselbe Semantik.

Ein Aufruf kann beispielsweise:

- eine Zustandsänderung anfordern,
- Informationen abfragen,
- über ein eingetretenes Ereignis informieren,
- einen kontinuierlichen Datenstrom übertragen.

Werden alle diese Fälle als identischer generischer Remote Call behandelt, fehlen dem System wichtige semantische Informationen.

Dadurch können unter anderem:

- Retry Policies,
- Idempotency,
- Deduplication,
- Backpressure,
- Caching,
- Scheduling,
- QoS,
- Deadline Handling,
- Resource Accounting

nicht zuverlässig anhand der tatsächlichen Bedeutung einer Operation entschieden werden.

NovaOS benötigt deshalb explizite semantische Operationstypen innerhalb der einheitlichen Distributed-Communication-Architektur.

---

## Entscheidung

NovaOS definiert vier grundlegende Distributed-Operationstypen:

```text
Operation
 ├── Command
 ├── Query
 ├── Event
 └── Stream
```

Diese Typen beschreiben die primäre Kommunikationssemantik einer Operation.

Sie verwenden weiterhin dieselben grundlegenden NovaOS-Mechanismen:

```text
Nova.Call
Typed Schemas
Capabilities
Deadlines
Cancellation
Backpressure
Resource Economy
Tracing
Data Sovereignty
```

Die Operationstypen sind daher keine getrennten Kommunikationssysteme, sondern semantische Spezialisierungen derselben Distributed-Communication-Architektur.

---

## Grundprinzip

```text
One communication architecture.
Explicit operation semantics.
```

und:

```text
Mechanism is shared.
Meaning is explicit.
```

---

## Command

Ein `Command` fordert die Ausführung einer Operation an, die einen beobachtbaren Zustand verändern kann.

```text
Caller
  ↓
Command
  ↓
Target
  ↓
State Change
  ↓
Result / Acknowledgement
```

Beispiele:

```text
CreateDocument
DeleteObject
StartProcess
UpdateConfiguration
DelegateCapability
```

Commands können:

- idempotent,
- deduplizierbar,
- transaktional,
- nicht wiederholbar

sein.

Die konkrete Semantik wird durch den Operationscontract definiert.

Ein Command darf nicht allein aufgrund seines Operationstyps automatisch als retry-safe gelten.

---

## Query

Eine `Query` liest oder berechnet Informationen, ohne gemäß ihrem Contract einen fachlich relevanten persistenten Zustand zu verändern.

```text
Caller
  ↓
Query
  ↓
Target
  ↓
Read / Compute
  ↓
Result
```

Beispiele:

```text
GetDocumentMetadata
ResolveService
GetSystemState
CalculateRoute
```

Eine Query kann intern Ressourcen verwenden oder technische Caches aktualisieren.

Solche internen Implementierungsdetails ändern ihre logische Query-Semantik nicht, solange der fachlich beobachtbare Zustand entsprechend dem Contract unverändert bleibt.

---

## Event

Ein `Event` beschreibt eine Tatsache, die bereits eingetreten ist.

```text
Producer
  ↓
Event
  ↓
Subscribers
```

Beispiele:

```text
DeviceConnected
DocumentChanged
ProcessExited
NetworkPathChanged
CapabilityRevoked
```

Ein Event ist semantisch keine Aufforderung, dass ein bestimmter Consumer eine Operation ausführt.

```text
Command
    =
Do this

Event
    =
This happened
```

Mehrere Consumer können dasselbe Event unabhängig verarbeiten.

---

## Stream

Ein `Stream` beschreibt eine zeitlich fortlaufende oder potentiell umfangreiche Folge typisierter Elemente.

```text
Producer
  ↓
Element
  ↓
Element
  ↓
Element
  ↓
Consumer
```

Beispiele:

```text
Audio Stream
Video Stream
Telemetry Stream
Sensor Stream
File Transfer
Incremental Query Result
```

Streams besitzen eine explizite Lifecycle-, Flow-Control- und Backpressure-Semantik.

---

## Operation Type als Contract-Eigenschaft

Der Operationstyp ist Bestandteil des expliziten Operationscontracts.

Konzeptionell:

```text
Operation Contract
 ├── Operation Identity
 ├── Operation Type
 ├── Input Schema
 ├── Output Schema
 ├── Authority Requirements
 ├── Idempotency
 ├── Deadline
 ├── Resource Budget
 └── Sovereignty Policy
```

Dadurch können Systemkomponenten die Semantik einer Operation erkennen, ohne sie aus Methodennamen oder Payloads erraten zu müssen.

---

## Keine Namensheuristik

NovaOS darf den Operationstyp nicht aus Namen wie:

```text
Get...
Set...
Update...
On...
```

ableiten.

```text
Operation Name
      ≠
Operation Semantics
```

Der Typ muss explizit durch den Contract definiert sein.

---

## Einheitliche Nova.Call-Abstraktion

Alle Operationstypen werden auf der gemeinsamen `Nova.Call`-Architektur aufgebaut.

```text
                  Nova.Call
                     │
        ┌────────────┼────────────┐
        ↓            ↓            ↓
     Command        Query        Event
                                   │
                                   ↓
                                 Stream
```

Die konkrete API-Oberfläche darf für die jeweiligen Semantiken spezialisierte Formen anbieten.

Intern bleiben gemeinsame Contracts, Security-, Transport- und Introspection-Mechanismen erhalten.

---

## Command und Exactly-Once Effects

Commands sind besonders relevant für:

- Idempotency,
- Deduplication,
- Exactly-Once Effects.

Ein zustandsverändernder Command kann eine stabile Operation Identity verwenden:

```text
Command
   ↓
Operation ID
   ↓
Deduplication
   ↓
Single Semantic Effect
```

Ob dies erforderlich ist, wird durch den jeweiligen Contract definiert.

---

## Query und Caching

Queries können grundsätzlich für Caching geeignet sein.

```text
Query
  ↓
Cache Policy
  ↓
Cached Result
```

Dies ist jedoch keine automatische Eigenschaft jeder Query.

Caching muss unter anderem berücksichtigen:

- Freshness,
- Consistency,
- Security Context,
- Capability Context,
- Data Sovereignty,
- Input Parameters.

Der Operationstyp liefert lediglich die notwendige semantische Grundlage für entsprechende Policies.

---

## Events und Delivery Semantics

Events benötigen explizite Delivery Semantics.

Je nach Contract können beispielsweise unterschiedliche Anforderungen bestehen hinsichtlich:

- Verlusttoleranz,
- Reihenfolge,
- Deduplication,
- Persistenz,
- Replay,
- Acknowledgement.

Ein Event darf nicht automatisch als zuverlässig oder verlustbehaftet angenommen werden.

Die konkreten Delivery-Modelle werden in NPSPECs definiert.

---

## Event Identity

Events können eine stabile Event Identity besitzen.

```text
Event
 ├── Event Identity
 ├── Event Type
 ├── Source Identity
 ├── Timestamp / Logical Ordering
 └── Payload
```

Damit können unter anderem:

- Deduplication,
- Replay,
- Audit,
- Causation

unterstützt werden.

---

## Streams und Backpressure

Streams müssen Backpressure als grundlegende Eigenschaft unterstützen.

```text
Producer
   ↓
Bounded Buffer
   ↓
Consumer

Consumer slower
      ↓
Backpressure
      ↑
Producer
```

Ein schneller Producer darf nicht unbegrenzt Daten erzeugen, wenn der Consumer sie nicht entsprechend verarbeiten kann.

---

## Stream Lifecycle

Streams besitzen einen expliziten Lifecycle.

Konzeptionell:

```text
Open
 ↓
Active
 ↓
Complete
```

Zusätzlich können Zustände beziehungsweise Outcomes wie:

```text
Cancelled
Failed
DeadlineExceeded
```

auftreten.

Die konkrete State Machine wird in NPSPECs definiert.

---

## Typed Streams

Stream-Elemente verwenden Typed Schemas und Semantic Types.

```text
Stream<VideoFrame>
Stream<AudioSample>
Stream<TelemetrySample>
Stream<DocumentChunk>
```

Der Elementtyp ist Bestandteil des Stream Contracts.

Ein Stream ist daher nicht lediglich eine untypisierte Folge von Bytes.

Raw Byte Streams bleiben als expliziter semantischer Typ möglich.

---

## Bidirektionale Streams

Die Architektur muss auch bidirektionale Streaming-Kommunikation ermöglichen können.

```text
Endpoint A
   ⇅
Typed Stream
   ⇅
Endpoint B
```

Dabei besitzt jede Richtung eigene:

- Flow-Control,
- Backpressure,
- Lifecycle-,
- Cancellation-

Eigenschaften.

Die konkrete API-Semantik wird in NPSPECs definiert.

---

## Operation Composition

Operationstypen dürfen miteinander kombiniert werden.

Beispielsweise:

```text
Query
  ↓
Stream<Result>
```

oder:

```text
Command
  ↓
Progress Event Stream
```

oder:

```text
Command
  ↓
Result
```

Die primäre Operation und ihre Result-/Event-/Stream-Kanäle bleiben dabei semantisch explizit.

---

## Retry-Semantik

Der Operationstyp beeinflusst Retry Policies, bestimmt sie aber nicht vollständig.

```text
Command
  → Retry only according to idempotency/deduplication

Query
  → Often retryable, but contract decides

Event
  → Delivery policy decides

Stream
  → Resume/reconnect semantics decide
```

Operation-Aware Retry verwendet deshalb sowohl den Operationstyp als auch den vollständigen Operationscontract.

---

## Deadline Propagation

Alle vier Operationstypen können Deadline Constraints besitzen.

Bei Streams kann zwischen:

- Deadline für den gesamten Stream,
- Deadline für Stream Setup,
- Deadlines einzelner Elemente

unterschieden werden.

Die konkrete Semantik wird in NPSPECs definiert.

---

## Cancellation

Commands, Queries und Streams müssen mit Structured Cancellation integrierbar sein.

Events repräsentieren bereits eingetretene Tatsachen und können deshalb nicht semantisch „rückgängig gemacht“ werden.

Die Verarbeitung oder Zustellung eines Events kann jedoch abgebrochen werden.

```text
Cancel Event Delivery
    ≠
Undo Event
```

---

## Capability und Authority

Operationstypen verändern keine Authority-Regeln.

Ein Caller benötigt weiterhin die erforderliche Capability für die jeweilige Operation.

Bei Events kann getrennte Authority für:

- Publish,
- Subscribe,
- Replay

existieren.

Bei Streams können getrennte Authority für:

- Open,
- Read,
- Write,
- Control

definiert werden.

---

## Data Sovereignty

Command-, Query-, Event- und Stream-Payloads unterliegen denselben Data-Sovereignty- und Information-Flow-Regeln wie andere Distributed Communication.

Insbesondere dürfen Event-Broadcast oder Stream-Subscription keine Sovereignty-Grenzen umgehen.

---

## Resource Economy

Jeder Operationstyp besitzt unterschiedliche typische Ressourcenprofile.

```text
Command → Execution + State Change
Query   → Compute + Result Data
Event   → Fan-Out + Delivery
Stream  → Sustained Bandwidth + Buffers
```

Diese Semantik kann von der Resource Economy für:

- Budgets,
- Scheduling,
- Queue Capacity,
- Bandwidth,
- QoS

verwendet werden.

---

## QoS

Operationstyp und QoS bleiben getrennte Eigenschaften.

```text
Command
 ├── Realtime
 └── Background

Query
 ├── Interactive
 └── Batch

Stream
 ├── Latency Critical
 └── Throughput Optimized
```

Ein Stream ist beispielsweise nicht automatisch Realtime und ein Command nicht automatisch hoch priorisiert.

---

## Location Transparency

Die Operationstypen gelten unabhängig davon, ob die Kommunikation:

- innerhalb derselben Execution Domain,
- über IPC,
- über ein lokales Netzwerk,
- über ein entferntes Netzwerk

erfolgt.

```text
Command
Query
Event
Stream
   ↓
same semantic model
   ↓
Direct / IPC / Network
```

Damit bleibt die Operationsemantik unabhängig vom Ausführungsort.

---

## Zero-Serialization Local Fast Path

Lokale Command-, Query-, Event- und Stream-Kommunikation darf den Zero-Serialization Local Fast Path verwenden, sofern die dafür notwendigen Bedingungen erfüllt sind.

Die Optimierung verändert den Operationstyp nicht.

---

## Introspection und Tracing

Der Operationstyp muss in Distributed Tracing und Architecture Introspection sichtbar sein.

Beispielsweise:

```text
Operation
 ├── Type: Command
 ├── Identity
 ├── Target
 ├── Caller
 ├── Deadline
 ├── Retry Policy
 └── Outcome
```

oder:

```text
Operation
 ├── Type: Stream
 ├── Element Type
 ├── Producer
 ├── Consumer
 ├── Backpressure State
 └── Resource Usage
```

Dadurch können Tools und Systemdienste Kommunikationsverhalten semantisch analysieren.

---

## Normative Anforderungen

1. NovaOS MUSS `Command`, `Query`, `Event` und `Stream` als grundlegende Distributed-Operationstypen unterstützen.
2. Die Operationstypen MÜSSEN auf einer gemeinsamen Distributed-Communication-Architektur aufbauen.
3. Der Operationstyp MUSS explizit im Operationscontract darstellbar sein.
4. NovaOS DARF den Operationstyp NICHT ausschließlich aus Operationsnamen ableiten.
5. Commands MÜSSEN zustandsverändernde Operationssemantik ausdrücken können.
6. Queries MÜSSEN nicht fachlich zustandsverändernde Abfrage- beziehungsweise Berechnungssemantik ausdrücken können.
7. Events MÜSSEN bereits eingetretene Tatsachen ausdrücken können.
8. Streams MÜSSEN fortlaufende Folgen typisierter Elemente darstellen können.
9. Der Operationstyp DARF NICHT allein die Retry-Semantik bestimmen.
10. Commands MÜSSEN mit Idempotency, Deduplication und Exactly-Once-Effect-Contracts integrierbar sein.
11. Queries MÜSSEN mit expliziten Cache Policies integrierbar sein.
12. Events MÜSSEN explizite Delivery Semantics besitzen können.
13. Events MÜSSEN stabile Event Identities unterstützen können.
14. Streams MÜSSEN Bounded Queues beziehungsweise kontrollierte Buffer und Backpressure unterstützen.
15. Streams MÜSSEN einen expliziten Lifecycle besitzen.
16. Stream-Elemente MÜSSEN Typed Schemas und Semantic Types verwenden können.
17. Bidirektionale Streams MÜSSEN unterstützt werden können.
18. Operationstypen MÜSSEN miteinander komponierbar sein.
19. Alle Operationstypen MÜSSEN mit Deadline- und Cancellation-Mechanismen integrierbar sein, soweit deren Semantik dies zulässt.
20. Operationstypen DÜRFEN Capability-, Trust-, Security-, Data-Sovereignty- oder Information-Flow-Regeln NICHT umgehen.
21. Operationstypen MÜSSEN in die systemweite Resource Economy und QoS-Architektur integrierbar sein.
22. Die Operationsemantik MUSS unabhängig vom konkreten Direct-, IPC- oder Network-Provider bleiben.
23. Lokale Fast-Path-Optimierungen DÜRFEN die Operationsemantik NICHT verändern.
24. Operationstyp, Lifecycle und relevante Kommunikationsentscheidungen MÜSSEN kontrolliert introspektierbar und tracebar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Distributed Operations erhalten explizite maschinenlesbare Semantik,
- Retry und Deduplication können Operationen korrekt behandeln,
- Queries können kontrolliert gecacht werden,
- Events erhalten klare Publish-/Subscribe- und Delivery-Semantik,
- Streams integrieren Backpressure und Resource Economy,
- dieselbe Architektur funktioniert lokal und remote,
- Systemoptimierungen müssen Kommunikationssemantik nicht aus API-Namen erraten,
- NovaOS erhält eine stabile Grundlage für RPC, Messaging, Eventing und Streaming ohne vier unabhängige Kommunikationsstacks.

### Negative Konsequenzen

- Operationscontracts benötigen zusätzliche semantische Metadaten,
- Entwickler müssen den korrekten Operationstyp explizit definieren,
- Event- und Stream-Semantik benötigen zusätzliche Delivery- und Lifecycle-Spezifikationen,
- komplexe Operationen können mehrere Operationstypen kombinieren.

---

## Verworfene Alternativen

### Alle Kommunikation ausschließlich als RPC behandeln

Verworfen.

Commands, Queries, Events und Streams besitzen unterschiedliche semantische Anforderungen, die durch ein generisches Request/Response-Modell nicht ausreichend beschrieben werden.

### Getrennte Kommunikationsstacks für RPC, Messaging und Streaming

Verworfen.

Dies würde Security, Capabilities, Serialization, Deadlines, Resource Accounting und Introspection unnötig duplizieren.

### Operationstyp aus Methodennamen ableiten

Verworfen.

Namenskonventionen sind keine verlässlichen maschinenlesbaren Contracts.

### Command und Query nicht unterscheiden

Verworfen.

Zustandsverändernde und nicht zustandsverändernde Operationen benötigen unterschiedliche Retry-, Caching- und Reliability-Entscheidungen.

### Events als Commands an mehrere Empfänger behandeln

Verworfen.

Ein Event beschreibt eine eingetretene Tatsache und besitzt eine andere semantische Bedeutung als eine Aufforderung zur Zustandsänderung.

### Streams ausschließlich als große Nachrichten behandeln

Verworfen.

Kontinuierliche Daten benötigen explizite Lifecycle-, Flow-Control-, Backpressure- und Ressourcenmodelle.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0009_Unified_Data_Object_Pipelining`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-IPC-0001_Typed_IPC`
- `ADR-IPC-0004_Unified_Call_Abstraction`
- `ADR-IPC-0006_Backpressure_als_systemweite_IPC_Eigenschaft`
- `ADR-IPC-0007_Causation_und_Tracing_über_IPC_Grenzen`
- `ADR-DISTCOMM-0001_Nova_Call_als_einheitliche_Distributed_Call_Abstraktion`
- `ADR-DISTCOMM-0005_Typed_Schemas_und_versionierte_Serialisierung`
- `ADR-DISTCOMM-0006_Zero_Serialization_Local_Fast_Path`
- `ADR-DISTCOMM-0007_Bounded_Queues_und_Backpressure`
- `ADR-DISTCOMM-0008_Idempotency_und_Deduplication_für_Exactly_Once_Effects`
- `ADR-DISTCOMM-0009_Operation_Aware_Retry_und_Circuit_Breaking`
- `ADR-DISTCOMM-0010_End_To_End_Deadline_Propagation`
- `ADR-DISTCOMM-0011_Sovereignty_Preflight_vor_Remote_Execution`

---

## Zugehörige NPSPECs

- `NPSPEC-DISTCOMM-OPERATION-0001`
- `NPSPEC-DISTCOMM-COMMAND-0001`
- `NPSPEC-DISTCOMM-QUERY-0001`
- `NPSPEC-DISTCOMM-EVENT-0001`
- `NPSPEC-DISTCOMM-EVENT-DELIVERY-0001`
- `NPSPEC-DISTCOMM-STREAM-0001`
- `NPSPEC-DISTCOMM-STREAM-LIFECYCLE-0001`
- `NPSPEC-DISTCOMM-STREAM-BACKPRESSURE-0001`
- `NPSPEC-DISTCOMM-OPERATION-COMPOSITION-0001`
- `NPSPEC-DISTCOMM-OPERATION-TYPE-TEST-0001`

---

## Ergebnis

NovaOS behandelt Distributed Communication nicht als Sammlung untypisierter Remote Calls, sondern beschreibt die semantische Art jeder Operation explizit:

```text
                         Nova.Call
                            ↓
                    Operation Contract
                            ↓
        ┌───────────┬───────┴───────┬───────────┐
        ↓           ↓               ↓           ↓
     Command      Query           Event       Stream
        ↓           ↓               ↓           ↓
 State Change   Information     Fact/Event   Data Flow
        │           │               │           │
        └───────────┴───────┬───────┴───────────┘
                            ↓
                  Shared Infrastructure
                            ↓
             Capabilities / Typed Schemas
            Deadlines / Backpressure / QoS
          Resource Economy / Sovereignty
                    Tracing / Security
```

Damit erhält NovaOS eine einheitliche Kommunikationsarchitektur, in der Commands, Queries, Events und Streams dieselben grundlegenden Mechanismen verwenden, während ihre unterschiedliche fachliche und technische Semantik ausdrücklich erhalten bleibt.