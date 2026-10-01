# NPSPEC-OBSERVABILITY-TRACING-0001 – Nova Observability Tracing

## Status

Angenommen

## Kategorie

Observability / Tracing / Execution Flow

## Zweck

NovaOS definiert ein systemweites Tracing-Modell zur Nachverfolgung von Ausführungsabläufen über Kernel-, Prozess-, IPC-, Provider-, Geräte- und Systemgrenzen hinweg.

Tracing beschreibt den kausalen Ablauf einer Operation und ergänzt Logging und Metrics.

```text
Execution
   ↓
Trace
   ├── Span A
   ├── Span B
   └── Span C
```

## Grundprinzipien

```text
Trace ≠ Log
Trace ≠ Metric
Trace ≠ Audit
TraceID ≠ ExecutionID
TraceID ≠ Permission
Span ≠ Task
Observed Causality ≠ Authority
```

Tracing darf keine neuen Berechtigungen erzeugen.

## Trace Model

Ein Trace beschreibt einen zusammenhängenden Ausführungsfluss:

```text
Trace
├── TraceID
├── Root Span
├── Spans
└── Trace State
```

Optional:

```text
ExecutionID
TransactionID
ObjectID
NodeID
ProviderID
Security Context Reference
Sampling State
```

## Span Model

Ein einzelner Abschnitt wird beschrieben durch:

```text
Span
├── SpanID
├── TraceID
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
ProcessID
ProviderID
NodeID
ResourceID
ObjectID
ErrorCode
Attributes
Events
```

## Trace Hierarchie

Spans können kausal miteinander verbunden werden.

```text
Root Span
├── IPC Request
│   └── Remote Execution
│       └── Storage Read
└── Local Processing
```

Die Hierarchie soll den tatsächlichen Ausführungsfluss darstellen.

## Context Propagation

Trace-Kontext kann über Systemgrenzen propagiert werden.

```text
Task A
  ↓
IPC
  ↓
Task B
  ↓
Remote IPC
  ↓
Task C
```

Weitergegeben werden können:

```text
TraceID
ParentSpanID
Trace Flags
Sampling State
```

Trace-Kontext enthält keine implizite Authority.

## Distributed Tracing

Distributed IPC muss Trace-Korrelation unterstützen können.

```text
Node A
Span 1
  ↓
Distributed IPC
  ↓
Node B
Span 2
```

Damit kann eine Operation über mehrere Nodes hinweg rekonstruiert werden.

## Execution Correlation

Tracing kann mit dem Execution Model verbunden werden.

```text
ExecutionID
    ↓
TraceID
    ↓
Multiple Spans
```

Eine Execution kann mehrere interne Operationen und Spans besitzen.

## Asynchrone Abläufe

Bei asynchronen Operationen ist eine reine Parent-Child-Hierarchie nicht immer ausreichend.

NovaOS soll deshalb kausale Links unterstützen:

```text
Span A
  ↓
Event / Queue
  ↓
Span B
```

Beispiel:

```text
Producer Span
     ↓ link
Consumer Span
```

## Structured Concurrency

Task-Hierarchien können mit Trace-Hierarchien korreliert werden.

```text
Parent Task
├── Child Task A
└── Child Task B
```

Dabei gilt:

```text
Task Hierarchy ≠ Trace Hierarchy
```

Beide Modelle dürfen miteinander verbunden, aber nicht gleichgesetzt werden.

## Timing

Spans können Dauer und zeitliche Beziehungen erfassen.

```text
Start
  ↓
Operation
  ↓
End

Duration = End - Start
```

Für Dauerberechnungen soll monotone Zeit verwendet werden.

Bei verteilten Traces müssen Clock Drift und unterschiedliche Zeitquellen berücksichtigt werden.

## Events

Innerhalb eines Spans können strukturierte Ereignisse auftreten.

```text
Span
├── Started
├── Retry
├── Provider Changed
├── Deadline Warning
└── Completed
```

Span Events ersetzen kein allgemeines Logging-System.

## Sampling

Nicht jede Ausführung muss vollständig aufgezeichnet werden.

Unterstützt werden sollen:

```text
Always
Never
Probabilistic
Adaptive
PolicyBased
ErrorTriggered
```

Sampling muss kontrollierbar und ressourcenbegrenzt sein.

## Tail Sampling

NovaOS kann eine Trace-Entscheidung nach Kenntnis des Ergebnisses treffen.

Beispiel:

```text
Execution
   ↓
Normal → discard/sample
Error  → preserve
```

Dies kann insbesondere seltene Fehler sichtbar machen.

## Resource Limits

Tracing darf keine unkontrollierten Ressourcen verbrauchen.

Begrenzbar müssen sein:

```text
Trace Count
Span Count
Buffer Size
Attribute Count
Event Count
Sampling Rate
Retention
Export Rate
```

## Sensitive Data

Trace-Attribute dürfen keine Secrets enthalten.

Insbesondere:

```text
Passwords
Private Keys
Capability Tokens
Credentials
Session Secrets
Encryption Keys
```

dürfen nicht gespeichert werden.

## Capability Security

Tracing benötigt keine Authority über die beobachtete Operation hinaus.

Zugriff auf Trace-Daten wird separat kontrolliert:

```text
TraceRead
TraceConfigure
TraceExport
TraceDelete
```

```text
TraceID ≠ Capability
```

## Privacy

Trace-Daten können sensible Beziehungen zwischen:

```text
Users
Objects
Services
Devices
Locations
```

sichtbar machen.

Daher gelten:

```text
Data Minimization
Security Labels
Retention
Controlled Export
```

## Logging Integration

Logs können einem Trace zugeordnet werden:

```text
TraceID
SpanID
```

Dadurch kann beispielsweise ein Fehlerlog direkt mit dem verursachenden Ausführungspfad verbunden werden.

```text
Trace → Ablauf
Log   → Ereignis
```

## Metrics Integration

Tracing kann Metrics erzeugen oder ergänzen.

Beispiele:

```text
Operation Duration
IPC Latency
Provider Latency
Error Rate
Queue Delay
```

Tracing und Metrics bleiben getrennte Datenmodelle.

## Deadlines

Spans können Execution Deadlines korrelieren.

```text
Execution Deadline
      ↓
Span A
      ↓
Span B
      ↓
Span C
```

Dadurch kann analysiert werden, welcher Teil einer Ausführung das Deadline-Budget verbraucht.

## Failure Analysis

Tracing soll insbesondere ermöglichen:

```text
Where did execution fail?
Which provider was involved?
Which IPC path was used?
Where was latency introduced?
Which dependency failed?
```

Ein unvollständiger Trace darf nicht als vollständiger Ablauf dargestellt werden.

## Trace State

NovaOS muss mindestens unterscheiden können:

```text
Active
Completed
Failed
Cancelled
Incomplete
Dropped
Unknown
```

Dabei gilt:

```text
Incomplete ≠ Failed
Unknown ≠ Completed
```

## Retention

Trace-Daten unterliegen expliziten Retention Policies.

```text
Trace
  ↓
Retention
  ↓
Aggregate / Expire / Delete
```

Fehler-Traces können andere Aufbewahrungsregeln besitzen als normale Traces.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
TraceID
ExecutionID
Span Graph
Current Span
Duration
Provider
Node
IPC Path
Errors
Sampling State
Dropped Spans
Trace Completeness
Retention State
```

## Normative Anforderungen

1. NovaOS MUSS ein strukturiertes systemweites Tracing-Modell bereitstellen.
2. Tracing MUSS von Logging, Metrics und Auditing getrennt bleiben.
3. Traces MÜSSEN über stabile `TraceID`s identifizierbar sein.
4. Spans MÜSSEN über stabile `SpanID`s identifizierbar sein.
5. Kausale Beziehungen zwischen Spans MÜSSEN darstellbar sein.
6. Trace Context MUSS über IPC- und Distributed-IPC-Grenzen propagierbar sein.
7. Trace Context DARF keine Authority erzeugen oder übertragen.
8. Asynchrone Kausalität MUSS über Links darstellbar sein.
9. Task- und Trace-Hierarchie MÜSSEN getrennte Konzepte bleiben.
10. Für Dauermessungen SOLL monotone Zeit verwendet werden.
11. Distributed Tracing MUSS Clock-Unterschiede berücksichtigen.
12. Sampling und Ressourcenverbrauch MÜSSEN kontrollierbar sein.
13. Secrets und übertragbare Capability Tokens DÜRFEN NICHT in Trace-Daten gespeichert werden.
14. Zugriff auf Trace-Daten MUSS capabilitybasiert kontrollierbar sein.
15. Privacy- und Retention-Policies MÜSSEN auf Trace-Daten anwendbar sein.
16. Logs und Metrics SOLLEN über stabile IDs mit Traces korrelierbar sein.
17. Unvollständige Traces DÜRFEN NICHT als vollständige Ausführungsabläufe dargestellt werden.
18. Trace- und Sampling-State MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-OBSERVABILITY-LOGGING-0001`
- `NPSPEC-OBSERVABILITY-METRICS-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-LATENCY-0001`
- `NPSPEC-EXECUTION-DEADLINE-0001`
- `NPSPEC-DISTRIBUTED-EXECUTION-0001`
- `NPSPEC-DISTRIBUTED-IPC-0001`
- `NPSPEC-DISTCOMM-TRACE-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-RETENTION-0001`
- `ADR-ARCH-0072`

## Ergebnis

```text
Execution
    ↓
Trace Context
    ↓
Local + Remote Spans
    ↓
Causal Trace Graph
    ↓
Logging + Metrics Correlation
    ↓
Failure + Latency Analysis
```

NovaOS erhält damit ein durchgängiges Tracing-Modell, das Ausführungsabläufe vom lokalen Kernelpfad bis zu verteilten Operationen kausal nachvollziehbar macht, ohne Trace-Informationen mit Authority, Logging, Metrics oder Audit-Daten gleichzusetzen.