# NPSPEC-OBSERVABILITY-DISTRIBUTEDTRACE-0001 – Nova Distributed Trace

## Status

Angenommen

## Kategorie

Observability / Distributed Tracing / Cross-System Causality

## Zweck

NovaOS definiert ein systemweites Modell zur Rekonstruktion kausaler Ausführungsabläufe über Node-, Prozess-, IPC-, Provider- und Clustergrenzen hinweg.

```text
Node A
  ↓
Node B
  ↓
Node C
  ↓
Distributed Trace
```

Ein Distributed Trace erweitert das allgemeine Nova-Tracing-Modell um verteilte Kausalität, unvollständige Sichtweisen, unterschiedliche Zeitquellen und dynamische Ausführungsorte.

## Grundprinzipien

```text
Distributed Trace ≠ Central Log
TraceID ≠ ExecutionID
TraceID ≠ Capability
Trace Context ≠ Authority
Network Order ≠ Causal Order
Timestamp Order ≠ Causal Order
Missing Span ≠ Failed Operation
Unreachable Node ≠ Failed Node
```

## Distributed Trace Model

```text
DistributedTrace
├── TraceID
├── Root Context
├── Span Graph
└── State
```

Optional:

```text
ExecutionID
TransactionID
Participating Nodes
Providers
Objects
Resources
Locations
Sampling State
Completeness
Security Labels
```

## Distributed Span

Jeder beteiligte Node kann eigene Spans erzeugen.

```text
DistributedSpan
├── SpanID
├── TraceID
├── NodeID
├── OperationID
├── Start
├── End
└── Status
```

Optional:

```text
ParentSpanID
ExecutionID
TaskID
ProviderID
ObjectID
ResourceID
LocationID
Links
Events
ErrorCode
```

## Trace Context Propagation

Trace-Kontext wird entlang verteilter Kommunikation propagiert.

```text
Node A
Span A
  ↓
Distributed IPC
  ↓
Node B
Span B
```

Übertragen werden können:

```text
TraceID
ParentSpanID
Trace Flags
Sampling State
Correlation Context
```

Der Kontext darf keine Credentials oder Capability Secrets enthalten.

## Kausale Beziehungen

Distributed Tracing basiert primär auf expliziter Kausalität.

```text
Span A
  ↓ caused
Span B
  ↓ caused
Span C
```

Für asynchrone Abläufe werden Links verwendet:

```text
Producer Span
      ↓ link
Queue / Event
      ↓
Consumer Span
```

## Zeitmodell

Verteilte Nodes besitzen möglicherweise unterschiedliche Uhren.

```text
Clock A ≠ Clock B
```

Deshalb darf NovaOS nicht ausschließlich anhand von Wall-Clock-Timestamps auf Kausalität schließen.

Verwendbar sind:

```text
Monotonic Local Time
Clock Synchronization Metadata
Sequence Information
Explicit Parent Relationships
Causal Links
```

## Execution Correlation

Ein Distributed Trace kann eine verteilte Execution abbilden.

```text
ExecutionID
   ↓
TraceID
   ├── Node A
   ├── Node B
   └── Node C
```

Execution und Trace bleiben getrennte Konzepte.

## Migration

Bei Task- oder Provider-Migration bleibt der Trace erhalten.

```text
Task @ Node A
      ↓ migration
Task @ Node B
```

Die neuen Spans verwenden weiterhin denselben `TraceID`.

Die Migration selbst kann als eigener Span oder Event dargestellt werden.

## Placement

Placement-Entscheidungen können im Trace korreliert werden:

```text
Placement Decision
      ↓
Node Selection
      ↓
Remote Execution
```

Dadurch können beispielsweise Latenz- oder Ressourcenprobleme auf konkrete Placement-Entscheidungen zurückgeführt werden.

## Distributed Transactions

Transaktionen können über:

```text
TransactionID
```

mit einem Distributed Trace verbunden werden.

Beispiel:

```text
Prepare A
Prepare B
Commit Decision
Commit A
Commit B
```

Trace-Daten stellen dabei keine verbindliche Transaction-Wahrheit dar.

```text
Trace ≠ Transaction State Authority
```

## Partial Traces

Distributed Traces können unvollständig sein.

Ursachen:

```text
Node Failure
Network Partition
Sampling
Buffer Overflow
Dropped Span
Collector Failure
Privacy Policy
Security Policy
```

NovaOS muss daher unterscheiden:

```text
Complete
PartiallyComplete
Incomplete
Unknown
```

```text
Missing Span ≠ Operation Did Not Execute
```

## Sampling

Sampling kann lokal oder verteilt erfolgen.

```text
Root Sampling Decision
        ↓
Propagated Sampling State
```

Nodes dürfen entsprechend ihrer lokalen Security-, Privacy- und Resource-Policy zusätzliche Einschränkungen anwenden.

## Collection

Distributed Tracing darf keine zentrale Laufzeitabhängigkeit erzeugen.

```text
Node A ─┐
Node B ─┼→ Optional Collector
Node C ─┘
```

Nodes müssen Trace-Daten lokal puffern können.

Ein Collector-Ausfall darf die eigentliche Execution nicht unnötig blockieren.

## Correlation

Distributed Traces können korreliert werden mit:

```text
Logs
Metrics
Execution Contracts
Transactions
IPC
Placement
Resource Accounting
```

Gemeinsame IDs ermöglichen die Verbindung der jeweiligen Datenmodelle.

## Security

Trace-Daten können interne Systemstrukturen offenlegen.

Zugriff benötigt deshalb explizite Capabilities:

```text
DistributedTraceRead
DistributedTraceConfigure
DistributedTraceExport
DistributedTraceDelete
```

Trace-Kontext selbst erzeugt keine Authority.

## Privacy

Node-, Object-, Location- und Identity-Informationen können sensibel sein.

Daher gelten:

```text
Data Minimization
Security Labels
Selective Visibility
Retention
Controlled Export
```

Ein Node darf Informationen aus einer anderen Security Domain nicht allein aufgrund eines gemeinsamen `TraceID` lesen.

## Sovereignty

Trace-Daten unterliegen denselben relevanten Sovereignty-Anforderungen wie andere Systemdaten.

Ein zentraler Collector darf Trace-Daten nicht automatisch über erlaubte Sovereignty Domains hinweg verschieben.

## Resource Limits

Begrenzbar müssen sein:

```text
Trace Count
Span Count
Buffer Size
Sampling Rate
Attribute Count
Retention
Export Bandwidth
```

Observability darf Distributed Execution nicht dominieren.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
TraceID
ExecutionID
Participating Nodes
Span Graph
Causal Links
Locations
Providers
IPC Paths
Migration Events
Transaction Correlation
Sampling State
Missing Spans
Completeness
Clock Information
```

## Normative Anforderungen

1. NovaOS MUSS verteilte Ausführungen über gemeinsame `TraceID`s korrelieren können.
2. Jeder Span MUSS über eine eindeutige `SpanID` identifizierbar sein.
3. Trace Context MUSS über Distributed IPC propagierbar sein.
4. Trace Context DARF keine Authority erzeugen.
5. Capability Secrets und Credentials DÜRFEN NICHT im Trace Context enthalten sein.
6. NovaOS MUSS explizite kausale Beziehungen zwischen verteilten Spans unterstützen.
7. Timestamp-Reihenfolge DARF NICHT allein als Kausalitätsbeweis verwendet werden.
8. Asynchrone Beziehungen MÜSSEN über Trace Links darstellbar sein.
9. Migration DARF die bestehende Trace-Korrelation NICHT verlieren.
10. Execution-, Transaction- und Trace-Identitäten MÜSSEN getrennt bleiben.
11. Distributed Traces MÜSSEN unvollständige Zustände explizit darstellen können.
12. Fehlende Spans DÜRFEN NICHT als Beweis für eine nicht ausgeführte Operation interpretiert werden.
13. Distributed Tracing DARF keine permanente zentrale Collector-Infrastruktur voraussetzen.
14. Collector-Ausfälle DÜRFEN die eigentliche Execution NICHT unnötig blockieren.
15. Sampling und Ressourcenverbrauch MÜSSEN begrenzbar sein.
16. Zugriff auf Trace-Daten MUSS capabilitybasiert kontrolliert werden.
17. Privacy-, Security-, Retention- und Sovereignty-Policies MÜSSEN erhalten bleiben.
18. Distributed Trace State und Completeness MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-OBSERVABILITY-LOGGING-0001`
- `NPSPEC-OBSERVABILITY-METRICS-0001`
- `NPSPEC-OBSERVABILITY-TRACING-0001`
- `NPSPEC-DISTRIBUTED-EXECUTION-0001`
- `NPSPEC-DISTRIBUTED-SCHEDULER-0001`
- `NPSPEC-DISTRIBUTED-IPC-0001`
- `NPSPEC-DISTRIBUTED-TRANSACTION-0001`
- `NPSPEC-DISTRIBUTED-LOCATION-0001`
- `NPSPEC-DISTCOMM-TRACE-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-RETENTION-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-0001`
- `ADR-ARCH-0073`

## Ergebnis

```text
Distributed Execution
        ↓
Trace Context Propagation
        ↓
Local + Remote Spans
        ↓
Causal Span Graph
        ↓
Cross-Node Correlation
        ↓
Logs + Metrics + IPC + Transactions
        ↓
Distributed Diagnosis
```

NovaOS erhält damit ein durchgängiges Distributed-Tracing-Modell, das kausale Ausführungsabläufe über Systemgrenzen hinweg rekonstruierbar macht, ohne globale Zeitsynchronität, zentrale Collector-Infrastruktur oder implizite Authority vorauszusetzen.