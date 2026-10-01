# NPSPEC-EXECUTION-LATENCY-0001 – Nova Execution Latency

## Status

Angenommen

## Kategorie

Execution / Latency / Temporal Execution

## Zweck

NovaOS definiert Latenzanforderungen als expliziten Bestandteil des Execution Contracts.

```text
ExecutionContract
      ↓
Latency Requirement
      ↓
Planning
      ↓
Provider + Resource Selection
      ↓
Execution
      ↓
Latency Measurement
```

Damit kann NovaOS eine Operation anhand ihrer gesamten End-to-End-Latenz planen und nicht nur anhand der Geschwindigkeit einzelner Komponenten.

## Grundprinzipien

```text
Latency ≠ Deadline
Latency ≠ Timeout
Latency ≠ Execution Time
Latency ≠ Throughput
Latency ≠ Priority

Low Average Latency ≠ Predictable Latency
Fast Provider ≠ Low End-to-End Latency
```

## Latency Requirement

Ein Execution Contract kann enthalten:

```text
ExecutionLatencyRequirement
├── Target Latency
├── Maximum Latency
└── Requirement Class
```

Optional:

```text
Maximum Jitter
Percentile Target
Measurement Scope
Warmup Policy
Fallback Policy
Violation Policy
```

## Requirement Classes

NovaOS unterscheidet:

```text
Hard
Firm
Soft
Preferred
```

### Hard

Die maximale Latenz ist ein Hard Constraint.

### Firm

Eine Überschreitung reduziert oder beseitigt den Nutzen des Ergebnisses.

### Soft

Überschreitungen sind zulässig, verschlechtern jedoch die Dienstqualität.

### Preferred

Die Latenz ist lediglich ein Optimierungsziel.

## End-to-End-Latenz

NovaOS betrachtet die gesamte Ausführungskette.

```text
Request
  ↓
IPC
  ↓
Queue
  ↓
Scheduling
  ↓
Compute
  ↓
I/O
  ↓
Network
  ↓
Result
```

Vereinfacht:

```text
L_total =
    L_ipc
  + L_queue
  + L_schedule
  + L_compute
  + L_io
  + L_network
  + L_sync
```

Nicht verwendete Komponenten entfallen.

## Latency Budget

Eine End-to-End-Latenz kann auf Teiloperationen verteilt werden.

```text
Total Latency Budget
├── IPC
├── Scheduling
├── Compute
├── I/O
└── Network
```

Teilbudgets dürfen zusammen die zulässige End-to-End-Latenz nicht unkontrolliert überschreiten.

## Provider Selection

Provider werden anhand ihrer erwarteten Gesamtlatenz bewertet.

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
```

Ein theoretisch schnellerer Accelerator kann ungeeignet sein, wenn beispielsweise Transfer- oder Queue-Latenzen den Vorteil aufheben.

## Resource Selection

Latency Planning kann berücksichtigen:

```text
CPU Placement
NUMA Locality
Cache Locality
Memory Latency
I/O Queue
Network Path
Accelerator Queue
Data Transfer Cost
Resource Contention
```

## Semantic Execution

Die semantische Operation bleibt unabhängig von der gewählten Implementierung.

```text
Semantic Operation
       ↓
ExecutionContract
       ↓
Latency Constraints
       ↓
Compatible Providers
```

Nur Provider, die alle Hard Constraints erfüllen, dürfen ausgewählt werden.

## Deadline Integration

Deadline und Latenz bleiben getrennte Anforderungen.

```text
Deadline:
Wann muss das Ergebnis spätestens fertig sein?

Latency:
Wie lange darf die Operation benötigen?
```

Beide können gleichzeitig gelten.

```text
Start Time + Maximum Latency ≤ Deadline
```

wenn der Contract dies verlangt.

## Resource Budget Integration

Niedrige Latenz darf keine unbegrenzte Ressourcennutzung erzeugen.

```text
Latency Requirement
        +
Resource Budget
        ↓
Execution Planning
```

Der Planner muss beide Anforderungen gleichzeitig berücksichtigen.

## Reservation

Für kritische Latenzanforderungen können Ressourcen reserviert werden.

```text
Latency Requirement
      ↓
Reservation Planning
      ↓
Reduced Contention
```

Eine Reservation garantiert jedoch nicht automatisch die End-to-End-Latenz.

## Admission Control

Hard-Latency-Anforderungen können eine Feasibility-Prüfung erfordern.

```text
Expected Latency
      +
Current Contention
      +
Reserved Capacity
      ↓
Admission
```

Ist eine Hard-Latency bereits vor Beginn offensichtlich nicht erfüllbar, soll die Ausführung abgelehnt oder neu geplant werden.

## Jitter

Für zeitkritische Operationen ist nicht nur der Mittelwert relevant.

NovaOS kann erfassen:

```text
Minimum
Average
Maximum
P50
P95
P99
P99.9
Jitter
```

Damit kann zwischen schneller und vorhersehbarer Ausführung unterschieden werden.

## Runtime Monitoring

Während der Ausführung kann NovaOS den verbleibenden Latenzrahmen überwachen.

```text
Latency Budget
      ↓
Elapsed Latency
      ↓
Remaining Latency
```

Zustände können sein:

```text
Normal
AtRisk
Critical
Violated
```

## Replanning

Droht eine Verletzung:

```text
AtRisk
  ↓
Replan
```

Mögliche Maßnahmen:

```text
Change Provider
Increase Resource Reservation
Reduce Queueing
Improve Locality
Reduce Optional Work
Use Allowed Lower-Cost Representation
Graceful Degradation
```

Hard Constraints dürfen dabei nicht abgeschwächt werden.

## Latency Violation

Eine Überschreitung muss explizit erkennbar sein.

```text
Maximum Latency
      ↓ exceeded
LatencyViolation
```

Die Contract Policy kann festlegen:

```text
Continue
Degrade
Cancel
Fallback
Fail
```

## Adaptive Optimization

NovaOS kann historische Messungen verwenden.

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

Adaptive Modelle dürfen keine Hard Requirements überschreiben.

## Distributed Execution

Bei Remote Execution müssen zusätzlich berücksichtigt werden:

```text
Serialization
Transport
Network
Remote Queue
Remote Execution
Return Transport
```

Ein Remote Provider darf nicht nur anhand seiner Compute-Zeit bewertet werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Target Latency
Maximum Latency
Requirement Class
Measured Latency
Remaining Budget
Jitter
Percentiles
Current State
Selected Provider
Latency Breakdown
Violation History
```

## Normative Anforderungen

1. NovaOS MUSS Latency Requirements als Bestandteil von Execution Contracts unterstützen.
2. Latency, Deadline, Timeout, Throughput und Priority MÜSSEN getrennte Konzepte bleiben.
3. Hard-, Firm-, Soft- und Preferred-Latency MÜSSEN unterscheidbar sein.
4. NovaOS MUSS End-to-End-Latenz berücksichtigen können.
5. Teilbudgets DÜRFEN die End-to-End-Anforderung NICHT unkontrolliert verletzen.
6. Provider Selection SOLL Queue-, Transfer-, I/O- und Kommunikationslatenzen berücksichtigen.
7. Hard-Latency-Anforderungen SOLLEN Admission Control unterstützen.
8. Niedrige Latenz DARF Resource Budgets und Hard Constraints NICHT umgehen.
9. Latency Violations MÜSSEN explizit erkennbar sein.
10. Replanning DARF nur innerhalb der Grenzen des Execution Contracts erfolgen.
11. Adaptive Optimierung DARF Hard-Latency-Anforderungen NICHT abschwächen.
12. Latenzzustand und Messwerte MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-RESOURCEBUDGET-0001`
- `NPSPEC-RESOURCE-LATENCY-0001`
- `NPSPEC-RESOURCE-DEADLINE-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESOURCE-GUARANTEE-0001`
- `NPSPEC-RESOURCE-ADMISSION-0001`
- `NPSPEC-RESOURCE-ARBITRATION-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-SEMANTIC-EXECUTION-0001`
- `ADR-ARCH-0051`

## Ergebnis

```text
ExecutionContract
      ↓
End-to-End Latency Requirement
      ↓
Admission + Planning
      ↓
Provider + Resource Selection
      ↓
Execution
      ↓
Measurement + Replanning
      ↓
Latency Requirement Verified
```

NovaOS erhält damit ein durchgängiges Execution-Latency-Modell, bei dem Latenz von der ursprünglichen Operation bis zum fertigen Ergebnis geplant, gemessen und kontrolliert werden kann, ohne sie mit Deadline, Priorität oder Ressourcenverbrauch gleichzusetzen.