# ADR-OBSERVABILITY-0002 – Distributed Tracing

## Status

Angenommen

## Kategorie

Observability Architecture / Distributed Tracing

## Kontext

NovaOS kann eine logische Ausführung über mehrere Komponenten, Prozesse, Compute Provider und Nodes verteilen.

```text
Application
    ↓
Capability
    ↓
Service
    ↓
Remote Node
    ↓
Compute Provider
```

Lokale Logs allein reichen nicht aus, um einen solchen Ausführungsweg vollständig nachzuvollziehen.

NovaOS benötigt deshalb systemweites Distributed Tracing.

## Entscheidung

NovaOS unterstützt **Distributed Tracing** über Prozess-, Service-, Provider- und Node-Grenzen hinweg.

Eine logische Ausführung erhält einen gemeinsamen Trace-Kontext.

```text
Execution
   ↓
Trace ID
   ├── Span A
   ├── Span B
   │    └── Span C
   └── Span D
```

Die `Trace ID` identifiziert den gesamten Ausführungsweg.

Spans beschreiben einzelne Abschnitte innerhalb dieses Weges.

## Trace Context

Der Trace Context enthält mindestens:

```text
Trace ID
Span ID
Parent Span ID
```

Zusätzlich können relevante Kontextinformationen referenziert werden:

```text
Execution ID
Transaction ID
Capability ID
Algorithm ID
Provider ID
Node ID
Object ID
```

Der Kontext muss über relevante Systemgrenzen propagiert werden können.

## Spans

Ein Span repräsentiert einen abgegrenzten Teil einer Ausführung.

Beispiele:

```text
Capability Resolution
Algorithm Selection
Provider Selection
Data Transfer
Remote Execution
Storage Access
Contract Verification
```

Ein Span kann enthalten:

```text
Start Time
End Time
Status
Operation Type
Parent
Relevant Attributes
Events
```

## Parent-Child-Beziehungen

Spans bilden den logischen Ausführungsweg ab.

```text
Execution
└── Capability Resolution
    └── Algorithm Selection
        └── Provider Selection
            ├── Data Transfer
            └── Execution
```

Die Struktur beschreibt logische Abhängigkeiten und nicht zwingend eine rein sequenzielle Ausführung.

## Asynchrone Ausführung

Distributed Tracing muss auch asynchrone und parallele Ausführungen abbilden können.

```text
Parent Span
   ├── Task A
   ├── Task B
   └── Task C
```

Structured Concurrency und Trace-Struktur sollen soweit möglich miteinander korrelierbar sein.

## Remote Execution

Bei Remote Execution wird der Trace Context an den entfernten Execution Provider übertragen.

```text
Node A
  Span A
    ↓ Trace Context
Node B
  Span B
    ↓
Node C
  Span C
```

Dadurch bleibt die logische Ausführung trotz Location Transparency nachvollziehbar.

## Execution Planning

Auch die automatische Ausführungsplanung kann Teil eines Traces sein.

```text
Execution Request
       ↓
Contract Validation
       ↓
Algorithm Selection
       ↓
Provider Selection
       ↓
Resource Reservation
       ↓
Execution
```

Dadurch kann später nachvollzogen werden, wie ein Execution Plan entstanden ist.

## Datenbewegung

Relevante Datenbewegungen sollen als eigene Spans oder Span-Ereignisse darstellbar sein.

```text
Object
  ↓
Transfer
  ↓
Remote Provider
  ↓
Execution
  ↓
Result Transfer
```

Damit lassen sich Compute-Zeit und Transfer-Zeit getrennt analysieren.

## Fehler

Fehler müssen dem relevanten Span zugeordnet werden können.

```text
Span
 ├── Status
 ├── Error Type
 └── Error Context
```

Ein Fehler in einem Child Span bedeutet nicht automatisch, dass der gesamte Trace fehlgeschlagen ist.

Insbesondere bleibt:

```text
Unknown ≠ Failed
```

## Zeitmodell

Nodes können unterschiedliche lokale Uhren besitzen.

Distributed Tracing darf deshalb nicht davon ausgehen, dass alle Timestamps perfekt synchronisiert sind.

```text
Logical Relationships
        +
Local Timestamps
        +
Clock Information
        ↓
Trace Reconstruction
```

Parent-Child- und Kausalitätsinformationen besitzen Vorrang vor unkritischer Interpretation absoluter Zeitstempel.

## Sampling

Nicht jeder Trace muss dauerhaft vollständig gespeichert werden.

NovaOS darf verwenden:

```text
Head Sampling
Tail Sampling
Adaptive Sampling
Rate Limiting
```

Fehler-, Security-, Safety- oder Contract-Violation-Traces können von normalen Sampling-Regeln ausgenommen werden.

## Trace Propagation und Trust

Trace Context ist Kontextinformation und keine Berechtigung.

```text
Trace ID
   ≠
Identity
   ≠
Authorization
   ≠
Trust
```

Ein empfangener Trace Context darf keine zusätzlichen Rechte verleihen.

Trace-Daten von nicht vertrauenswürdigen Quellen müssen entsprechend behandelt werden.

## Privacy

Trace-Daten dürfen nur notwendige Informationen enthalten.

Insbesondere sollen keine unnötigen:

```text
Payloads
Credentials
Secrets
Personal Data
Sensitive Object Contents
```

in Trace Attributes übernommen werden.

Trace-Daten unterliegen den normalen Privacy-, Retention-, Security- und Data-Sovereignty-Regeln.

## Korrelation

Distributed Traces müssen mit Logs und Metrics korrelierbar sein.

```text
Trace ID
   ├── Trace Spans
   ├── Structured Logs
   └── Relevant Metrics
```

Damit kann beispielsweise eine langsame Remote Execution mit Provider-Auslastung und konkreten Fehlerereignissen verbunden werden.

## Trace Lifecycle

Ein Trace endet logisch, wenn die zugehörige Ausführung abgeschlossen oder ihr weiterer Zustand nicht mehr beobachtbar ist.

Mögliche Endzustände:

```text
Completed
Failed
Cancelled
Timeout
Unknown
```

Ein abgebrochener Kommunikationsweg darf nicht automatisch als fehlgeschlagene Remote-Ausführung interpretiert werden.

## Normative Anforderungen

1. NovaOS MUSS Distributed Tracing über relevante Systemgrenzen unterstützen.
2. Eine logische verteilte Ausführung MUSS über eine gemeinsame Trace ID korrelierbar sein.
3. Einzelne Ausführungsabschnitte MÜSSEN als Spans darstellbar sein.
4. Parent-Child-Beziehungen MÜSSEN unterstützt werden.
5. Trace Context MUSS über Prozess-, Service- und Node-Grenzen propagierbar sein.
6. Asynchrone und parallele Ausführungen MÜSSEN abbildbar sein.
7. Remote Execution MUSS den bestehenden Trace Context fortführen können.
8. Execution Planning SOLL tracebar sein.
9. Relevante Datenbewegungen SOLLEN im Trace sichtbar sein.
10. Fehler MÜSSEN relevanten Spans zugeordnet werden können.
11. `Unknown` MUSS von `Failed` unterschieden werden.
12. Trace-Rekonstruktion DARF keine perfekt synchronisierten Node-Uhren voraussetzen.
13. Sampling MUSS unterstützt werden können.
14. Verbindliche Security-, Safety- oder Audit-Anforderungen DÜRFEN nicht durch normales Sampling umgangen werden.
15. Trace Context DARF keine Authorization oder Trust implizieren.
16. Secrets und Credentials DÜRFEN nicht in Trace-Daten übernommen werden.
17. Distributed Tracing MUSS Privacy-, Retention-, Security- und Data-Sovereignty-Regeln einhalten.
18. Traces MÜSSEN mit Logs und Metrics korrelierbar sein.

## Konsequenzen

### Positive Konsequenzen

- verteilte Ausführungen werden Ende-zu-Ende nachvollziehbar,
- Remote-Latenzen und Datenbewegungen können analysiert werden,
- Execution Planning und tatsächliche Ausführung lassen sich korrelieren,
- Fehler können über Node-Grenzen hinweg verfolgt werden,
- Self-Healing erhält bessere Ursacheninformationen.

### Negative Konsequenzen

- Trace Context muss über Systemgrenzen propagiert werden,
- Tracing erzeugt zusätzlichen Daten- und Speicheraufwand,
- Sampling und Datenschutz benötigen klare Regeln.

## Abhängigkeiten

- `ADR-OBSERVABILITY-0001_Unified_Logging_Metrics_Tracing`
- `ADR-ARCH-0010_Structured_Concurrency`
- `ADR-ARCH-0013_Introspection`
- `ADR-EXECUTION-0001_Nova_ExecutionContract`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-DISTRIBUTED-0001_Distributed_Execution`
- `ADR-DISTRIBUTED-0009_Location_Transparent_Execution`
- `ADR-DISTRIBUTED-0010_Remote_Execution_unter_Sovereignty_und_Trust_Constraints`
- `ADR-CRYPTO-0010_Privacy-Preserving_Logs_und_Crash_Dumps`
- `ADR-PRIVACY-0001_Data_Minimization`

## Zugehörige NPSPECs

- `NPSPEC-OBSERVABILITY-DISTRIBUTED-TRACING-0001`
- `NPSPEC-OBSERVABILITY-TRACE-CONTEXT-0001`
- `NPSPEC-OBSERVABILITY-SPAN-0001`
- `NPSPEC-OBSERVABILITY-TRACE-PROPAGATION-0001`
- `NPSPEC-OBSERVABILITY-TRACE-SAMPLING-0001`

## Ergebnis

NovaOS kann einen logischen Ausführungsweg über lokale und entfernte Komponenten hinweg verfolgen:

```text
Execution Request
       ↓
Trace ID
       ↓
Local Span
       ↓
Remote Span
       ↓
Compute Span
       ↓
Result Span
       ↓
Logs + Metrics + Trace
       ↓
End-to-End Analysis
```

Der zentrale Grundsatz lautet:

```text
Eine verteilte Ausführung
bleibt für NovaOS
eine zusammenhängende Ausführung.

System-, Prozess- und Node-Grenzen
dürfen den logischen Trace
nicht unterbrechen.
```