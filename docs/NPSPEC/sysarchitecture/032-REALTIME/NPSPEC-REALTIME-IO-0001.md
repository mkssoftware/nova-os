# NPSPEC-REALTIME-IO-0001 – Nova Realtime I/O

## Status

Angenommen

## Kategorie

Realtime / I/O / Temporal Guarantees

## Zweck

NovaOS definiert ein Realtime-I/O-Modell für zeitkritische Ein- und Ausgabeoperationen mit kontrollierbaren Latenzen, Deadlines, Ressourcenbudgets und Prioritäten.

```text
Realtime Execution
        ↓
I/O Request
        ↓
Admission / Reservation
        ↓
Realtime I/O Scheduling
        ↓
Driver / Device
        ↓
Completion
        ↓
Deadline Verification
```

Realtime-I/O ist Bestandteil einer End-to-End-Realtime-Garantie und darf nicht isoliert von Scheduler, Treiber und physischem Gerät betrachtet werden.

## Grundprinzipien

```text
Fast I/O ≠ Realtime I/O
Priority ≠ Guarantee
Async I/O ≠ Realtime
Deadline ≠ Timeout
Queue Priority ≠ Bounded Completion
Device Available ≠ Device Predictable
Average Latency ≠ Worst-Case Latency
```

## Realtime I/O Model

```text
RealtimeIORequest
├── RequestID
├── ExecutionID
├── Operation
├── Resource
├── Deadline
├── IOBudget
└── State
```

Optional:

```text
RealtimeProfile
Priority
LatencyBound
BandwidthReservation
QueueReservation
DeviceID
DriverID
CompletionID
ExecutionContractID
```

## Unterstützte I/O-Bereiche

Realtime-I/O kann gelten für:

```text
Storage
Network
Audio
Video
Sensors
Input Devices
Industrial I/O
Accelerators
Custom Devices
```

Die konkrete Garantie hängt von den Eigenschaften des jeweiligen Geräts ab.

## I/O Request

Ein Realtime-I/O-Request muss seine zeitlichen Anforderungen transportieren können.

```text
Operation
+
Deadline
+
Latency Bound
+
Realtime Profile
+
Resource Reservation
```

Diese Informationen müssen über den relevanten I/O-Pfad erhalten bleiben.

## Admission Control

Garantierte Realtime-I/O-Operationen müssen vor ihrer Zusicherung prüfbar sein.

```text
I/O Request
    ↓
Device Capacity
    ↓
Queue Load
    ↓
Existing Reservations
    ↓
Worst-Case Service Time
    ↓
Admission
```

Ist keine belastbare obere Grenze bestimmbar, darf keine Hard-Realtime-Garantie zugesichert werden.

## I/O Scheduling

Der I/O-Scheduler muss zeitkritische Requests entsprechend ihrer Verträge behandeln können.

Berücksichtigt werden können:

```text
Deadline
Realtime Profile
Priority
Reserved Bandwidth
Execution Budget
Device Characteristics
```

Normale I/O-Last darf reservierte Realtime-Kapazität nicht unkontrolliert verdrängen.

## Queue Isolation

Realtime-I/O kann eigene Queue-Ressourcen benötigen.

```text
Device Queue
├── Realtime Reserved
└── General I/O
```

Geeignete Geräte können mehrere Hardware-Queues verwenden.

## Bandwidth Reservation

Realtime-Executions können I/O-Bandbreite reservieren.

```text
Available Bandwidth
├── Guaranteed Realtime
└── Best Effort
```

Reservierung muss durch Resource Accounting und Admission Control abgesichert werden.

## Latency

Der vollständige I/O-Pfad muss betrachtet werden.

```text
Request
  ↓
I/O Scheduler
  ↓
Driver
  ↓
Device Queue
  ↓
Hardware
  ↓
Interrupt / Completion
  ↓
Execution
```

Jede relevante Stufe kann zur End-to-End-Latenz beitragen.

## Driver

Treiber in einem garantierten Realtime-Pfad müssen zeitlich geeignet sein.

Problematisch sind insbesondere:

```text
Unbounded Locks
Unbounded Retry
Unbounded Allocation
Long Critical Sections
Unknown Device Wait
Uncontrolled Deferred Work
```

Ein nicht zeitlich begrenzbarer Treiber verhindert entsprechende Hard-Realtime-Garantien.

## Device Capability

NovaOS muss unterscheiden zwischen:

```text
Device supports operation
```

und:

```text
Device supports bounded realtime operation
```

Realtime-Eigenschaften sollen als explizite Device-/Provider-Capabilities beschreibbar sein.

## Completion

Realtime-I/O verwendet das reguläre Completion-Modell.

```text
Submit
  ↓
Execute
  ↓
Complete
  ↓
Wake / Notify
```

Completion-Latenz muss Bestandteil der zeitlichen Betrachtung sein.

## Cancellation

Nicht mehr sinnvolle Requests sollen kontrolliert abbrechbar sein, sofern Gerät und Operation dies unterstützen.

```text
Deadline Unreachable
        ↓
Cancel Request
        ↓
Release Reservation
```

Nicht abbrechbare Operationen müssen als solche gekennzeichnet werden.

## DMA und Zero-Copy

Realtime-I/O kann verwenden:

```text
DMA
Scatter/Gather
Shared Buffers
Zero-Copy
Pinned Memory
```

Diese Mechanismen können Latenz und CPU-Last reduzieren.

Sie erzeugen jedoch keine Realtime-Garantie und müssen weiterhin Memory-, DMA- und IOMMU-Sicherheitsregeln einhalten.

## Interrupts

Device Interrupts sind Teil des Realtime-I/O-Pfads.

Zu berücksichtigen sind:

```text
Interrupt Priority
Interrupt Affinity
Interrupt Latency
Deferred Processing
Interrupt Coalescing
```

Coalescing darf zugesicherte Latency Bounds nicht verletzen.

## Temporal Isolation

Realtime-I/O muss gegen konkurrierende I/O-Last isolierbar sein.

```text
Realtime I/O
      ↓
Reserved Capacity
      ↓
Bounded Interference
```

Die erforderliche Isolation hängt vom Gerät und dessen Queue-Modell ab.

## Deadline Miss

```text
CompletionTime > Deadline
        ↓
I/O Deadline Miss
```

Behandlung:

```text
Soft → Continue / Degrade
Firm → Cancel / Discard
Hard → Contract Violation
```

Ein erfolgreich abgeschlossener I/O-Request kann trotzdem seine Deadline verletzt haben.

## Adaptive Systeme

Adaptive I/O-Scheduling, Prefetch, Cache und Prediction dürfen Realtime-I/O unterstützen.

```text
Realtime Guarantee
        >
Adaptive Optimization
```

Hard-Realtime-I/O darf nicht von spekulativem Prefetch oder Prediction abhängen.

## Failure Handling

Gerätefehler können eine Garantie ungültig machen.

```text
Device Failure
      ↓
Guarantee Lost
      ↓
Fallback / Failover / Fail-safe
```

Ein alternativer Provider darf nur verwendet werden, wenn dessen Realtime-Eigenschaften die Anforderungen ebenfalls erfüllen.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
RequestID
Operation
Device
Realtime Profile
Deadline
Latency Bound
Queue State
Reservation
Submit Time
Completion Time
Measured Latency
Deadline Miss
Guarantee State
```

## Normative Anforderungen

1. NovaOS MUSS Realtime-I/O als Teil der End-to-End-Realtime-Architektur unterstützen.
2. Realtime-I/O DARF NICHT mit hoher I/O-Geschwindigkeit gleichgesetzt werden.
3. I/O-Requests MÜSSEN Realtime Profile, Deadline und relevante zeitliche Constraints transportieren können.
4. Garantiertes Realtime-I/O MUSS Admission Control unterstützen.
5. Nicht begrenzbare Geräte DÜRFEN NICHT als Hard-Realtime-fähig behandelt werden.
6. Realtime-I/O MUSS reservierbare Bandbreite oder vergleichbare Kapazitätskontrolle unterstützen können.
7. Realtime-I/O MUSS gegen unkontrollierte konkurrierende I/O-Last isolierbar sein.
8. Treiberlatenz MUSS Bestandteil der Realtime-Betrachtung sein.
9. Unbegrenzte Treiberoperationen DÜRFEN NICHT Teil eines garantierten Hard-Realtime-Pfads sein.
10. Completion-Latenz MUSS in End-to-End-Garantien berücksichtigt werden.
11. Cancellation SOLL unterstützt werden, sofern Operation und Gerät dies zulassen.
12. DMA und Zero-Copy DÜRFEN bestehende Memory- und Security-Grenzen NICHT umgehen.
13. Interrupt-Latenz MUSS bei Realtime-I/O berücksichtigt werden.
14. Interrupt Coalescing DARF zugesicherte Latency Bounds NICHT verletzen.
15. Hard-Realtime-I/O DARF NICHT von unsicheren Predictions oder spekulativem Prefetch abhängen.
16. I/O Deadline Misses MÜSSEN explizit erkannt werden.
17. Verlust einer I/O-Garantie MUSS abhängige End-to-End-Garantien revalidieren.
18. Realtime-I/O-Zustände und Verletzungen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-REALTIME-PROFILE-0001`
- `NPSPEC-REALTIME-SCHEDULING-0001`
- `NPSPEC-REALTIME-LATENCY-0001`
- `NPSPEC-REALTIME-DEADLINE-0001`
- `NPSPEC-REALTIME-TEMPORALISOLATION-0001`
- `NPSPEC-REALTIME-DETERMINISTIC-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-RESOURCE-IO-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESOURCE-GUARANTEE-0001`
- `NPSPEC-RESOURCE-ADMISSION-0001`
- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-IO-SCHEDULER-0001`
- `NPSPEC-IO-QOS-0001`
- `NPSPEC-IO-DEADLINE-0001`
- `NPSPEC-IO-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-DMA-0001`
- `ADR-ARCH-0106`

## Ergebnis

```text
Realtime I/O Request
        ↓
Admission + Reservation
        ↓
Queue Isolation
        ↓
Bounded Driver / Device Path
        ↓
Completion
        ↓
Latency + Deadline Verification
        ↓
Guarantee State
```

NovaOS erhält damit ein durchgängiges Realtime-I/O-Modell, das zeitkritische Ein- und Ausgabe nicht nur priorisiert, sondern den vollständigen Pfad von der Anfrage über Scheduler, Treiber und Hardware bis zur Completion in Realtime-Garantien einbezieht.