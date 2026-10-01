# NPSPEC-REALTIME-LATENCY-0001 – Nova Realtime Latency

## Status

Angenommen

## Kategorie

Realtime / Latency / Temporal Guarantees

## Zweck

NovaOS definiert ein Realtime-Latency-Modell für messbare und begrenzbare Reaktionszeiten zeitkritischer Executions.

```text
Event
  ↓
Detection
  ↓
Scheduling
  ↓
Execution
  ↓
Response
```

Realtime-Latency betrachtet nicht nur Durchschnittswerte, sondern insbesondere garantierbare obere Zeitgrenzen.

## Grundprinzipien

```text
Average Latency ≠ Worst-Case Latency
Low Latency ≠ Realtime Guarantee
Deadline ≠ Latency
Priority ≠ Latency Bound
Prediction ≠ Guarantee
Measured Fast ≠ Bounded
```

Für Realtime-Systeme ist die obere Grenze entscheidend.

## Latency Model

```text
RealtimeLatency
├── LatencyID
├── ExecutionID
├── LatencyType
├── MaximumLatency
├── MeasuredLatency
└── GuaranteeState
```

Optional:

```text
RealtimeProfile
DeadlineID
ResourceReservation
MeasurementPoint
JitterBound
PolicyID
TraceID
```

## Latency Types

NovaOS unterscheidet unter anderem:

```text
Interrupt Latency
Scheduling Latency
Wakeup Latency
Dispatch Latency
IPC Latency
IO Latency
Network Latency
Device Latency
End-to-End Latency
```

## End-to-End Latency

Eine Realtime-Garantie muss den vollständigen relevanten Pfad betrachten.

```text
Input
 ↓
Interrupt
 ↓
Scheduler
 ↓
Execution
 ↓
IPC / IO
 ↓
Device
 ↓
Response
```

Eine einzelne schnelle Komponente garantiert keine niedrige End-to-End-Latenz.

## Latency Bound

Execution Contracts können maximale Latenzen verlangen.

```text
Required Latency
      ↓
Admission Control
      ↓
Can Bound?
   ├── Yes → Guarantee
   └── No  → Reject / Degrade
```

NovaOS darf keine Latency Guarantee zusagen, deren obere Grenze nicht abgesichert werden kann.

## Worst-Case Latency

Für Hard Realtime muss insbesondere betrachtet werden:

```text
Worst-Case Interrupt Delay
Worst-Case Scheduling Delay
Worst-Case Blocking
Worst-Case Execution Time
Worst-Case IO Delay
```

Unbegrenzte Komponenten machen eine harte End-to-End-Garantie unmöglich.

## Jitter

Zusätzlich zur absoluten Latenz kann deren Schwankung begrenzt werden.

```text
Latency
  ↓
Variation over Time
  ↓
Jitter
```

Realtime Profiles können einen maximal zulässigen Jitter definieren.

## Blocking

Blockierende Operationen müssen bei garantierten Pfaden begrenzbar sein.

```text
Locks
Resource Contention
IPC
Memory Allocation
IO
Page Faults
```

```text
Unbounded Blocking
       ↓
No Hard Latency Guarantee
```

## Interrupt Latency

Interrupt-Verarbeitung muss zeitlich kontrollierbar sein.

Zu berücksichtigen sind:

```text
Interrupt Masking
Interrupt Priority
Interrupt Affinity
Nested Interrupts
Deferred Work
```

Lange nicht unterbrechbare Kernelabschnitte müssen für Realtime-Pfade vermieden werden.

## Scheduling Latency

Scheduling Latency beschreibt die Zeit zwischen:

```text
Task becomes runnable
        ↓
Task begins execution
```

Realtime Scheduling muss diese Zeit für garantierte Profile begrenzen können.

## Memory Latency

Realtime-Pfade sollen unvorhersehbare Speicheroperationen vermeiden können.

Beispiele:

```text
Page Fault
Swap
Memory Reclaim
Unbounded Allocation
Unexpected COW
```

Kritische Ressourcen können vorbereitet oder reserviert werden.

## IO und Device Latency

Gerätezugriffe müssen in End-to-End-Latency einbezogen werden.

```text
Execution
   ↓
IO Scheduler
   ↓
Driver
   ↓
Device
   ↓
Completion
```

Nicht begrenzbare Geräte dürfen nicht Teil einer zugesicherten Hard-Realtime-Latenz sein.

## Network Latency

Bei verteilten Realtime-Executions müssen Netzwerkgrenzen berücksichtigt werden.

```text
Local Processing
      +
Network
      +
Remote Processing
      ↓
End-to-End Latency
```

Nicht kontrollierbare Netzwerke können harte Garantien verhindern.

## Latency Budget

Eine End-to-End-Grenze kann auf Teilkomponenten verteilt werden.

```text
Total Latency Budget
├── Interrupt
├── Scheduling
├── Execution
├── IPC
├── IO
└── Response
```

Teilbudgets dürfen das Gesamtbudget nicht überschreiten.

## Adaptive Systeme

Adaptive Mechanismen dürfen Latency verbessern:

```text
Prediction
Preload
Prefetch
Cache
Placement
Adaptive Scheduling
```

Sie dürfen jedoch keine Grundlage einer Hard-Realtime-Garantie sein, wenn ihr Erfolg nicht garantiert werden kann.

## Latency Violation

Wird eine zugesicherte Grenze überschritten:

```text
MeasuredLatency > MaximumLatency
        ↓
Latency Violation
```

Die Reaktion richtet sich nach dem Realtime Profile:

```text
Soft → Record / Degrade
Firm → Cancel / Discard / Degrade
Hard → Contract Violation
```

## Measurement

Messpunkte müssen eindeutig definiert sein.

```text
Start Timestamp
      ↓
Operation
      ↓
End Timestamp
      ↓
Measured Latency
```

Messungen sollen möglichst monotone, hochauflösende Systemzeit verwenden.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
Latency Type
Required Bound
Measured Latency
Worst Observed Latency
Jitter
Latency Budget
Budget Consumption
Violation Count
Guarantee State
Measurement Source
```

## Normative Anforderungen

1. NovaOS MUSS Realtime-Latency explizit modellieren können.
2. Average Latency DARF NICHT als Worst-Case-Latency interpretiert werden.
3. Hard-Realtime-Garantien MÜSSEN auf begrenzbaren oberen Latenzen beruhen.
4. End-to-End-Latency MUSS alle relevanten Komponenten des Ausführungspfads berücksichtigen können.
5. Execution Contracts MÜSSEN maximale Latency Bounds deklarieren können.
6. Nicht erfüllbare Latency Guarantees DÜRFEN NICHT zugesichert werden.
7. Scheduling Latency MUSS für garantierte Realtime-Pfade begrenzbar sein.
8. Interrupt Latency MUSS berücksichtigt werden.
9. Unbegrenztes Blocking DARF NICHT Bestandteil eines garantierten Hard-Realtime-Pfads sein.
10. Memory-, IO-, Device- und Network-Latency MÜSSEN bei relevanten End-to-End-Garantien berücksichtigt werden.
11. Gesamt-Latency SOLL in kontrollierbare Teilbudgets zerlegbar sein.
12. Jitter MUSS optional begrenzbar sein.
13. Adaptive Optimierung DARF zugesicherte Latency Bounds NICHT verletzen.
14. Hard-Realtime-Garantien DÜRFEN NICHT von unsicheren Predictions abhängen.
15. Latency Violations MÜSSEN erkannt werden.
16. Latency Violations MÜSSEN entsprechend dem Realtime Profile behandelt werden können.
17. Messpunkte und Zeitbasis MÜSSEN eindeutig definiert sein.
18. Latency Bounds, Messwerte und Violations MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-REALTIME-PROFILE-0001`
- `NPSPEC-REALTIME-SCHEDULING-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-LATENCY-0001`
- `NPSPEC-EXECUTION-DEADLINE-0001`
- `NPSPEC-RESOURCE-LATENCY-0001`
- `NPSPEC-RESOURCE-DEADLINE-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESOURCE-GUARANTEE-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `NPSPEC-IO-DEADLINE-0001`
- `NPSPEC-INTERRUPT-PRIORITY-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `ADR-ARCH-0102`

## Ergebnis

```text
Realtime Event
      ↓
Bounded Interrupt
      ↓
Bounded Scheduling
      ↓
Bounded Execution
      ↓
Bounded IO / IPC
      ↓
Response
      ↓
Latency Verification
```

NovaOS erhält damit ein durchgängiges Realtime-Latency-Modell, bei dem zeitliche Garantien nicht auf Durchschnittsgeschwindigkeit, sondern auf kontrollierbaren oberen Grenzen des vollständigen Ausführungspfads beruhen.