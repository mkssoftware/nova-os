# NPSPEC-RESOURCE-LATENCY-0001 – Nova Latency Resource Model

## Status

Angenommen

## Kategorie

Resource / Latency / Resource Management

## Zweck

NovaOS definiert Latenz als explizite, messbare und kontrollierbare Systemressource.

```text
Operation
   ↓
Latency Requirement
   ↓
ExecutionContract
   ↓
Resource Selection + Scheduling
   ↓
Execution
   ↓
Latency Measurement
```

Damit kann NovaOS Reaktionszeit systemweit neben CPU, Speicher, I/O, Netzwerk, Energie und thermischen Grenzen berücksichtigen.

## Grundprinzipien

```text
Latency ≠ Throughput
Latency ≠ Deadline
Latency ≠ Execution Time
Low Average Latency ≠ Predictable Latency
Priority ≠ Latency Guarantee
Fast Resource ≠ Low End-to-End Latency
Latency Target ≠ Hard Guarantee
Optimization ≠ Permission to Violate Constraints
```

## Latency Resource

Latenz wird als Eigenschaft eines vollständigen Ausführungspfades betrachtet.

```text
LatencyResource
├── ResourceID
├── ResourceTypeID
├── Latency Domain
├── Current Latency
└── State
```

Optional:

```text
Minimum Latency
Average Latency
Maximum Observed Latency
Percentiles
Jitter
Queue Delay
Execution Delay
Transport Delay
Measurement Quality
```

## Latency Requirement

Workloads können Anforderungen deklarieren.

```text
LatencyRequirement
├── Target Latency
├── Maximum Latency
└── Requirement Class
```

Klassen können sein:

```text
Hard
Firm
Soft
Preferred
```

## End-to-End Latency

NovaOS betrachtet nicht nur einzelne Komponenten.

```text
Request
  ↓
IPC
  ↓
Queue
  ↓
Scheduling
  ↓
Execution
  ↓
I/O
  ↓
Response
```

Die End-to-End-Latenz kann sich aus mehreren Teilzeiten zusammensetzen.

```text
L_total =
L_queue +
L_schedule +
L_compute +
L_io +
L_network +
L_sync
```

## Latency Budget

Ein Gesamtbudget kann auf Teiloperationen verteilt werden.

```text
Total Latency Budget
├── IPC Budget
├── Compute Budget
├── I/O Budget
└── Network Budget
```

Unteroperationen dürfen das verbleibende Budget nicht als unbegrenzt betrachten.

## Deadline

Latency und Deadline bleiben getrennte Konzepte.

```text
Latency = Dauer einer Operation
Deadline = spätester zulässiger Abschlusszeitpunkt
```

Eine Operation kann geringe Latenz besitzen und trotzdem eine Deadline verpassen.

## Jitter

Für zeitkritische Workloads ist Schwankung relevant.

```text
Latency Samples
10 ms
11 ms
9 ms
10 ms
80 ms
```

Ein niedriger Durchschnitt darf hohe Ausreißer nicht verdecken.

NovaOS soll deshalb auch Jitter und Perzentile erfassen können.

## Resource Selection

Latency Requirements können die Provider-Auswahl beeinflussen.

```text
Operation
   ↓
Candidate Providers
├── CPU
├── GPU
├── NPU
├── Local Service
└── Remote Service
   ↓
Latency Evaluation
   ↓
Provider Selection
```

Die schnellste Recheneinheit muss nicht den niedrigsten End-to-End-Wert besitzen.

## Scheduling

Der Scheduler kann latenzkritische Tasks bevorzugt behandeln.

Dabei können berücksichtigt werden:

```text
Queue Delay
Wake-up Latency
Preemption
CPU Placement
Cache Locality
NUMA Locality
Resource Contention
```

Priorisierung darf jedoch Safety- und Hard-Systemgrenzen nicht umgehen.

## Netzwerk und I/O

Latency Requirements können über mehrere Subsysteme propagiert werden.

```text
ExecutionContract
      ↓
Compute
I/O
IPC
Network
```

Jedes beteiligte Subsystem erhält nur das für seine Operation relevante Teilbudget.

## Latency Pressure

NovaOS kann erkennen, wenn Latenzziele zunehmend gefährdet sind.

```text
Normal
 ↓
Elevated
 ↓
Constrained
 ↓
Critical
```

Mögliche Reaktionen:

```text
Increase Priority
Reduce Queueing
Change Provider
Improve Locality
Reserve Resources
Reduce Optional Work
Graceful Degradation
Controlled Failure
```

## Messung

Latency Accounting soll mindestens erfassen können:

```text
Start Timestamp
Completion Timestamp
Queue Time
Execution Time
Blocking Time
End-to-End Time
```

Messqualität kann angegeben werden als:

```text
Exact
Estimated
Sampled
Derived
Unknown
```

## Adaptive Optimierung

NovaOS kann erwartete und tatsächliche Latenz vergleichen.

```text
Predicted Latency
       ↓
Execution
       ↓
Measured Latency
       ↓
Prediction Error
       ↓
Model Adjustment
```

Dadurch können zukünftige Provider-, Placement- und Scheduling-Entscheidungen verbessert werden.

## ExecutionContract

Ein ExecutionContract kann enthalten:

```text
Target Latency
Maximum Latency
Deadline
Jitter Requirement
Determinism Requirement
Resource Budget
```

Hard Latency Requirements dürfen nicht stillschweigend abgeschwächt werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Current Latency
Latency Budget
Queue Latency
Execution Latency
I/O Latency
Network Latency
Jitter
Percentiles
Latency Pressure
Measurement Quality
```

## Normative Anforderungen

1. NovaOS MUSS Latenz als explizite Ausführungseigenschaft behandeln.
2. Latency, Throughput und Deadline MÜSSEN getrennte Konzepte bleiben.
3. End-to-End-Latenz MUSS über mehrere Subsysteme modellierbar sein.
4. ExecutionContracts MÜSSEN Latency Requirements definieren können.
5. Gesamtbudgets SOLLEN auf Teiloperationen verteilbar sein.
6. NovaOS SOLL Jitter und Latency-Perzentile erfassen können.
7. Resource Selection und Scheduling SOLLEN Latency Requirements berücksichtigen.
8. Adaptive Optimierung DARF Hard Latency Requirements NICHT abschwächen.
9. Unbekannte oder geschätzte Messwerte MÜSSEN entsprechend gekennzeichnet werden.
10. Latency Resources MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-RESOURCE-CPU-0001`
- `NPSPEC-RESOURCE-MEMORY-0001`
- `NPSPEC-RESOURCE-IO-0001`
- `NPSPEC-RESOURCE-NETWORK-0001`
- `NPSPEC-RESOURCE-GPU-0001`
- `NPSPEC-RESOURCE-NPU-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-IO-DEADLINE-0001`
- `NPSPEC-DISTCOMM-DEADLINE-0001`
- `ADR-ARCH-0041`

## Ergebnis

```text
Latency Requirement
        ↓
End-to-End Budget
        ↓
Resource Selection + Scheduling
        ↓
Execution
        ↓
Measurement
        ↓
Feedback + Adaptation
```

NovaOS erhält damit ein systemweites Latency Resource Model, das Reaktionszeit über CPU, I/O, IPC, Netzwerk und Accelerators hinweg messbar und planbar macht und dadurch latenzkritische Ausführung kontrolliert in die Resource Economy integriert.