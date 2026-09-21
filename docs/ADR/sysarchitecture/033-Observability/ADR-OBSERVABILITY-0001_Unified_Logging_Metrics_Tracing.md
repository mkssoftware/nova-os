# ADR-OBSERVABILITY-0001 – Unified Logging, Metrics und Tracing

## Status

Angenommen

## Kategorie

Observability Architecture

## Kontext

NovaOS besteht aus Kernel-Komponenten, Systemdiensten, Capabilities, Compute Providern und verteilten Ausführungen.

Für Diagnose, Performance-Analyse und Self-Healing müssen Ereignisse systemweit nachvollziehbar sein.

Getrennte Observability-Systeme für:

```text
Logs
Metrics
Traces
```

würden Korrelation und Ursachenanalyse erschweren.

## Entscheidung

NovaOS verwendet eine gemeinsame **Observability Architecture** für:

```text
Logging
Metrics
Tracing
```

Die drei Signalarten bleiben semantisch getrennt, verwenden jedoch gemeinsame Identitäten, Zeitinformationen und Kontextdaten.

```text
System Activity
      ↓
┌───────────────┐
│ Observability │
├───────────────┤
│ Logs          │
│ Metrics       │
│ Traces        │
└───────────────┘
      ↓
Correlation / Analysis
```

## Gemeinsamer Kontext

Observability-Signale können gemeinsame Kontextinformationen tragen.

Beispiele:

```text
Timestamp
Component ID
Process / Workload ID
Execution ID
Transaction ID
Trace ID
Object ID
Capability ID
Algorithm ID
Provider ID
Node ID
```

Nicht jedes Signal muss alle Felder enthalten.

## Logging

Logs beschreiben diskrete Ereignisse.

```text
Event
  ↓
Structured Log Record
```

Logs sollen bevorzugt strukturiert statt ausschließlich als Freitext erzeugt werden.

Beispiele:

```text
Event Type
Severity
Timestamp
Source
Execution ID
Structured Fields
```

Freitext bleibt für menschenlesbare Zusatzinformationen möglich.

## Metrics

Metrics beschreiben messbare Zustände oder Entwicklungen.

Beispiele:

```text
CPU Usage
Memory Usage
Latency
Queue Depth
Error Rate
Energy Consumption
Provider Utilization
```

Metrics müssen eindeutig einer Messgröße und ihrem relevanten Kontext zugeordnet werden können.

## Tracing

Tracing bildet zusammenhängende Ausführungswege ab.

```text
Execution Request
       ↓
Capability Resolution
       ↓
Algorithm Selection
       ↓
Provider Selection
       ↓
Execution
       ↓
Result
```

Verteilte Ausführungen können denselben logischen Trace über mehrere Komponenten oder Nodes fortführen.

## Korrelation

Logs, Metrics und Traces sollen über gemeinsame Identitäten korrelierbar sein.

```text
Trace ID
   ├── Logs
   ├── Metrics
   └── Spans
```

Dadurch kann NovaOS beispielsweise eine erhöhte Latenz mit einem konkreten Execution Plan und dessen Ereignissen verbinden.

## Execution Observability

Die automatische Ausführungsplanung muss beobachtbar sein.

Relevante Informationen können sein:

```text
Execution Contract
Selected Algorithm
Selected Provider
Execution Location
Resource Usage
Latency
Contract Violations
Replanning
Result Status
```

Damit können Planungsentscheidungen und deren tatsächliche Auswirkungen verglichen werden.

## Structured Events

Systemrelevante Ereignisse sollen stabile Event Types besitzen.

Beispiel:

```text
execution.started
execution.completed
execution.failed
execution.replanned

algorithm.selected

provider.selected
provider.unavailable

contract.unsatisfied
contract.violation
```

Event Types sind stabiler als die konkrete menschenlesbare Meldung.

## Fehler und Zustände

Observability muss unterschiedliche Zustände eindeutig unterscheiden können.

Beispiele:

```text
Success
Failed
Timeout
Cancelled
Unavailable
Unknown
ContractViolation
```

Insbesondere gilt:

```text
Unknown ≠ Failed
```

## Performance

Observability darf das beobachtete System nicht unverhältnismäßig beeinflussen.

NovaOS darf deshalb Mechanismen verwenden wie:

```text
Buffering
Aggregation
Sampling
Rate Limiting
Deferred Processing
```

Verbindliche Audit- oder Safety-Ereignisse dürfen jedoch nicht aufgrund normaler Performance-Optimierung stillschweigend verworfen werden.

## Privacy und Security

Observability-Daten können sensible Informationen enthalten.

Daher gilt:

```text
Collect only required data
       ↓
Classify
       ↓
Protect
       ↓
Retain only as required
```

Secrets, Credentials und unnötige Nutzdaten dürfen nicht in Logs, Metrics oder Traces übernommen werden.

## Verteilte Systeme

Observability muss lokale und verteilte Ausführungen nach demselben Grundmodell unterstützen.

```text
Node A ─┐
Node B ─┼→ Distributed Trace
Node C ─┘
```

Zeitabweichungen zwischen Nodes müssen bei der Interpretation berücksichtigt werden.

## Self-Healing

Observability liefert Signale für `Nova.Resilience`.

```text
Observability
      ↓
Detection
      ↓
Classification
      ↓
Recovery Planning
```

Observability selbst entscheidet jedoch nicht über Recovery Policies.

Damit bleiben Beobachtung und Reaktion getrennt.

## Introspection

Observability und Architecture Introspection ergänzen sich.

```text
Introspection
    → Was existiert und wie ist es aufgebaut?

Observability
    → Was passiert und wie verhält es sich?
```

Beide Informationen können für Diagnose und Self-Healing gemeinsam verwendet werden.

## Normative Anforderungen

1. NovaOS MUSS Logging, Metrics und Tracing als gemeinsame Observability Architecture behandeln.
2. Logging, Metrics und Tracing MÜSSEN semantisch unterscheidbare Signalarten bleiben.
3. Observability-Signale MÜSSEN gemeinsame Kontextidentitäten verwenden können.
4. Systemrelevante Logs SOLLEN strukturiert erzeugt werden.
5. Traces MÜSSEN komponenten- und nodeübergreifend fortgeführt werden können.
6. Logs, Metrics und Traces MÜSSEN miteinander korrelierbar sein.
7. Execution Requests SOLLEN über eine stabile Execution ID nachvollziehbar sein.
8. Execution Planning und Contract Violations MÜSSEN beobachtbar sein.
9. `Unknown` MUSS von `Failed` unterschieden werden.
10. Observability MUSS Sampling, Aggregation und Rate Limiting unterstützen können.
11. Verbindliche Audit- oder Safety-Ereignisse DÜRFEN nicht durch normale Sampling-Regeln verloren gehen.
12. Secrets und Credentials DÜRFEN nicht in Observability-Daten geschrieben werden.
13. Observability MUSS Privacy-, Security-, Retention- und Data-Sovereignty-Regeln einhalten.
14. Observability DARF Recovery Policy nicht selbst bestimmen.
15. Observability-Daten MÜSSEN für Introspection und Resilience nutzbar sein können.

## Konsequenzen

### Positive Konsequenzen

- systemweite Ereignisse werden korrelierbar,
- Fehlerursachen können über Komponenten hinweg verfolgt werden,
- Execution Planning wird nachvollziehbar,
- Performance- und Ressourcenprobleme können mit konkreten Ausführungen verbunden werden,
- Self-Healing erhält strukturierte Beobachtungssignale.

### Negative Konsequenzen

- Observability erzeugt zusätzlichen Ressourcenverbrauch,
- Kontextinformationen müssen konsistent propagiert werden,
- sensible Telemetriedaten benötigen Schutz und Lifecycle-Regeln.

## Abhängigkeiten

- `ADR-ARCH-0013_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-EXECUTION-0001_Nova_ExecutionContract`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-RESILIENCE-0001_Nova.Resilience`
- `ADR-RESILIENCE-0008_Health_State_Model`
- `ADR-CRYPTO-0010_Privacy-Preserving_Logs_und_Crash_Dumps`
- `ADR-PRIVACY-0001_Data_Minimization`
- `ADR-DISTRIBUTED-0001_Distributed_Execution`

## Zugehörige NPSPECs

- `NPSPEC-OBSERVABILITY-CORE-0001`
- `NPSPEC-OBSERVABILITY-LOGGING-0001`
- `NPSPEC-OBSERVABILITY-METRICS-0001`
- `NPSPEC-OBSERVABILITY-TRACING-0001`
- `NPSPEC-OBSERVABILITY-CONTEXT-0001`

## Ergebnis

NovaOS erhält eine gemeinsame Observability-Schicht:

```text
Kernel
Services
Capabilities
Execution
Compute
Distributed Nodes
       ↓
Logging + Metrics + Tracing
       ↓
Correlation
       ↓
Diagnosis
Introspection
Performance Analysis
Resilience
```

Der zentrale Grundsatz lautet:

```text
Logs zeigen,
was passiert ist.

Metrics zeigen,
wie sich das System verhält.

Traces zeigen,
wie eine Ausführung durch das System lief.

Gemeinsamer Kontext verbindet
alle drei Sichtweisen.
```