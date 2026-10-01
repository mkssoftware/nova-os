# NPSPEC-REALTIME-TEMPORALISOLATION-0001 – Nova Realtime Temporal Isolation

## Status

Angenommen

## Kategorie

Realtime / Temporal Isolation / Resource Isolation

## Zweck

NovaOS definiert Temporal Isolation, damit zeitkritische Executions vor unkontrollierter zeitlicher Beeinflussung durch andere Workloads geschützt werden können.

```text
Realtime Execution
        ↓
Reserved Resources
        ↓
Temporal Isolation
        ↓
Bounded Interference
        ↓
Predictable Execution
```

Temporal Isolation ist eine Grundlage für belastbare Realtime-Latency- und Deadline-Garantien.

## Grundprinzipien

```text
Spatial Isolation ≠ Temporal Isolation
Priority ≠ Isolation
CPU Reservation ≠ End-to-End Isolation
Low Load ≠ Guaranteed Capacity
Average Interference ≠ Worst-Case Interference
Dedicated Core ≠ Complete Isolation
```

Ziel ist nicht zwingend vollständige physische Trennung, sondern eine kontrollierbare obere Grenze fremder zeitlicher Einflüsse.

## Isolation Model

```text
TemporalIsolation
├── IsolationID
├── ExecutionID
├── Scope
├── ReservedResources
├── InterferenceBounds
└── State
```

Optional:

```text
RealtimeProfile
CPUSet
MemoryDomain
CachePartition
IOReservation
NetworkReservation
InterruptPolicy
DeviceReservation
ExecutionContractID
```

## Isolation Domains

Temporal Isolation kann mehrere Ressourcen umfassen:

```text
CPU
Scheduler
Memory
Cache
Interrupts
IO
Network
Devices
Accelerators
```

Welche Domains benötigt werden, hängt vom Realtime Profile und Execution Contract ab.

## CPU Isolation

Realtime Executions können CPU-Kapazität erhalten durch:

```text
Dedicated Core
Reserved CPU Time
CPU Partition
Execution Budget
Controlled Preemption
CPU Affinity
```

Normale Workloads dürfen reservierte CPU-Kapazität nicht unkontrolliert verbrauchen.

## Scheduler Isolation

Der Scheduler muss verhindern können, dass konkurrierende Workloads zugesicherte Realtime-Budgets verdrängen.

```text
CPU Capacity
├── Realtime Reservation
└── General Workloads
```

Nicht reservierte Kapazität kann weiterhin normal genutzt werden.

## Memory Isolation

Realtime-Ausführung kann vor unvorhersehbarem Speicherdruck geschützt werden.

Mechanismen können sein:

```text
Memory Reservation
Preallocation
Pinned Pages
Dedicated Pools
NUMA Placement
Bounded Allocators
```

Kritische Realtime-Pfade sollen nicht von unkontrolliertem Reclaim oder Swap abhängig sein.

## Cache Isolation

Gemeinsam genutzte CPU-Caches können zeitliche Interferenz erzeugen.

NovaOS soll, sofern Hardware und Plattform dies ermöglichen, unterstützen können:

```text
Cache Partitioning
Core Placement
Cache-aware Scheduling
Controlled Co-location
```

Cache-Isolation ist eine Optimierungs- und Garantiefrage, keine Security-Berechtigung.

## Interrupt Isolation

Interrupts können Realtime-Ausführung verdrängen.

NovaOS muss kontrollieren können:

```text
Interrupt Affinity
Interrupt Priority
Interrupt Rate
Deferred Work
Interrupt Coalescing
```

Unbegrenzte Interrupt-Last ist mit garantierter Temporal Isolation nicht vereinbar.

## IO Isolation

Realtime IO kann eigene Kapazitäten benötigen.

```text
Realtime IO
     ↓
Reserved Queue / Bandwidth
     ↓
IO Scheduler
     ↓
Device
```

Andere IO-Workloads dürfen zugesicherte IO-Grenzen nicht unkontrolliert verdrängen.

## Network Isolation

Für netzwerkabhängige Realtime-Ausführung können notwendig sein:

```text
Bandwidth Reservation
QoS
Queue Isolation
Traffic Priority
Controlled Congestion
```

Nicht kontrollierbare externe Netzwerke können End-to-End-Garantien begrenzen.

## Device Isolation

Gemeinsam genutzte Geräte können versteckte zeitliche Abhängigkeiten erzeugen.

```text
Realtime Execution
       ↓
Shared Device
       ↑
Other Workloads
```

Für garantierte Pfade muss deren maximale Interferenz begrenzbar sein.

## Accelerator Isolation

GPU- und NPU-Workloads können durch:

```text
Queue Reservation
Execution Partitioning
Memory Reservation
Preemption
Dedicated Accelerator
```

temporal isoliert werden, sofern die Hardware dies unterstützt.

## Interference Budget

Temporal Isolation kann maximale Fremdeinflüsse definieren.

```text
InterferenceBudget
├── CPU
├── Interrupt
├── Memory
├── Cache
├── IO
└── Device
```

Diese Budgets müssen mit der End-to-End-Latency vereinbar sein.

## Admission Control

Vor Zusicherung einer Isolation muss NovaOS prüfen:

```text
Requested Isolation
        ↓
Available Resources
        ↓
Existing Reservations
        ↓
Hardware Capabilities
        ↓
Admission Control
```

Kann eine notwendige Isolation nicht bereitgestellt werden, darf die entsprechende Realtime-Garantie nicht zugesichert werden.

## Shared Resources

Gemeinsam genutzte Ressourcen dürfen verwendet werden, wenn ihre Interferenz ausreichend begrenzt werden kann.

```text
Shared ≠ Unbounded
```

Ist keine belastbare Grenze bestimmbar, darf die Ressource nicht als garantiert isoliert behandelt werden.

## Adaptive Systeme

Adaptive Scheduler, Power-, Memory- oder Placement-Systeme dürfen Temporal Isolation berücksichtigen und verbessern.

Sie dürfen jedoch:

```text
Reserved Capacity
Isolation Boundary
Realtime Budget
Latency Guarantee
Deadline Guarantee
```

nicht zugunsten adaptiver Optimierung verletzen.

## Degradation

Geht eine zugesicherte Isolation verloren:

```text
Isolation Lost
      ↓
Guarantee Revalidation
      ↓
Degrade / Fail-safe / Contract Violation
```

NovaOS darf nicht stillschweigend weiter behaupten, die ursprüngliche Garantie bestehe.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
IsolationID
ExecutionID
Isolation Scope
Reserved Resources
CPU Assignment
Memory Reservation
Interrupt Policy
IO Reservation
Interference Budget
Measured Interference
Isolation State
Guarantee State
```

## Normative Anforderungen

1. NovaOS MUSS Temporal Isolation für garantierte Realtime-Executions unterstützen können.
2. Temporal Isolation MUSS von räumlicher und Security-Isolation unterscheidbar sein.
3. Realtime Executions MÜSSEN gegen unkontrollierte CPU-Interferenz isolierbar sein.
4. CPU-Reservierungen MÜSSEN durch den Scheduler durchsetzbar sein.
5. Kritische Realtime-Pfade SOLLEN vor unkontrolliertem Memory Reclaim und Swap geschützt werden können.
6. Cache-Interferenz SOLL berücksichtigt werden, wenn sie Realtime-Garantien beeinflusst.
7. Interrupt-Interferenz MUSS für Hard-Realtime-Pfade begrenzbar sein.
8. Relevante IO-Ressourcen MÜSSEN temporal isolierbar oder in ihrer Interferenz begrenzbar sein.
9. Netzwerk- und Device-Interferenz MUSS bei entsprechenden End-to-End-Garantien berücksichtigt werden.
10. Accelerator-Ressourcen SOLLEN temporal isolierbar sein, sofern Hardwareunterstützung existiert.
11. Gemeinsam genutzte Ressourcen DÜRFEN nur dann Teil einer harten Garantie sein, wenn ihre maximale Interferenz ausreichend begrenzbar ist.
12. Interference Budgets SOLLEN in das Gesamt-Latency-Budget integrierbar sein.
13. Garantierte Temporal Isolation MUSS Admission Control durchlaufen.
14. Nicht bereitstellbare Isolation DARF NICHT zugesichert werden.
15. Adaptive Optimierung DARF reservierte Kapazitäten und Isolation Boundaries NICHT verletzen.
16. Der Verlust zugesicherter Isolation MUSS eine Revalidierung abhängiger Realtime-Garantien auslösen.
17. Verlustene Garantien DÜRFEN NICHT stillschweigend als weiterhin gültig behandelt werden.
18. Temporal-Isolation-Zustände MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-REALTIME-PROFILE-0001`
- `NPSPEC-REALTIME-SCHEDULING-0001`
- `NPSPEC-REALTIME-LATENCY-0001`
- `NPSPEC-REALTIME-DEADLINE-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESOURCE-GUARANTEE-0001`
- `NPSPEC-RESOURCE-ADMISSION-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `NPSPEC-SCHEDULER-TOPOLOGY-0001`
- `NPSPEC-SCHEDULER-NUMA-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-IO-QOS-0001`
- `NPSPEC-INTERRUPT-AFFINITY-0001`
- `NPSPEC-INTERRUPT-PRIORITY-0001`
- `ADR-ARCH-0104`

## Ergebnis

```text
Realtime Contract
       ↓
Required Isolation
       ↓
Admission Control
       ↓
Resource Reservation
       ↓
Temporal Isolation
       ↓
Bounded Interference
       ↓
Realtime Execution
       ↓
Guarantee Verification
```

NovaOS erhält damit eine systemweite Temporal-Isolation-Schicht, die Realtime-Executions nicht nur priorisiert, sondern ihre zeitliche Beeinflussung durch konkurrierende CPU-, Memory-, Interrupt-, IO-, Netzwerk- und Device-Workloads kontrollierbar begrenzt.