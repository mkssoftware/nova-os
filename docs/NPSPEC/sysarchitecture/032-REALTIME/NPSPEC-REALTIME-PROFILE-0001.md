# NPSPEC-REALTIME-PROFILE-0001 – Nova Realtime Profile

## Status

Angenommen

## Kategorie

Realtime / Execution / Scheduling / Temporal Guarantees

## Zweck

NovaOS definiert standardisierte Realtime Profiles, mit denen Executions ihre zeitlichen Anforderungen deklarieren können.

```text
Execution
   ↓
Realtime Profile
   ↓
Admission Control
   ↓
Resource Reservation
   ↓
Realtime Scheduling
   ↓
Execution
```

Ein Realtime Profile beschreibt die benötigte zeitliche Verlässlichkeit und bestimmt, wie streng NovaOS Deadlines, Ressourcen und Scheduling behandeln muss.

## Grundprinzipien

```text
Realtime ≠ Fast
Priority ≠ Realtime
Deadline ≠ Realtime
Low Latency ≠ Determinism
Realtime ≠ Guaranteed Success

Realtime Guarantee
    requires
Bounded Resources + Admission + Isolation
```

## Profile

NovaOS definiert mindestens:

```text
None
Soft Realtime
Firm Realtime
Hard Realtime
```

### None

Keine zeitliche Garantie.

```text
Deadline Miss
→ normale Ausführung
```

Geeignet für:

```text
Background Tasks
Batch Processing
Maintenance
```

### Soft Realtime

Deadline-Verletzungen reduzieren die Qualität, machen das Ergebnis jedoch nicht grundsätzlich unbrauchbar.

```text
Deadline Miss
→ Degraded Quality
```

Beispiele:

```text
UI Rendering
Audio/Video
Interactive Workloads
```

### Firm Realtime

Ein verspätetes Ergebnis besitzt keinen oder nur noch geringen Nutzen.

```text
Deadline Miss
→ Result may be discarded
```

NovaOS soll Deadline Misses erkennen und unnötige Weiterverarbeitung vermeiden können.

### Hard Realtime

Eine Deadline ist Bestandteil der korrekten Ausführung.

```text
Deadline Miss
→ Contract Violation
```

Hard Realtime darf nur akzeptiert werden, wenn die erforderlichen Ressourcen und zeitlichen Grenzen tatsächlich zugesichert werden können.

## Realtime Profile Model

```text
RealtimeProfile
├── ProfileID
├── Class
├── Deadline
├── Period
├── ExecutionBudget
└── State
```

Optional:

```text
ExecutionID
LatencyBound
JitterBound
ResourceReservation
CPUAffinity
InterruptRequirements
IORequirements
DeterminismMode
FailurePolicy
```

## Execution Contract

Das Realtime Profile wird Bestandteil des Execution Contracts.

```text
ExecutionContract
├── RealtimeProfile
├── Deadline
├── Latency
├── ResourceBudget
├── Determinism
└── ResourceGuarantees
```

## Admission Control

Vor Firm- oder Hard-Realtime-Ausführung muss NovaOS prüfen, ob die Anforderungen erfüllbar sind.

```text
Realtime Request
      ↓
Resource Analysis
      ↓
Timing Analysis
      ↓
Admission Control
      ↓
Accept / Reject
```

```text
Requested Guarantee ≠ Granted Guarantee
```

NovaOS darf keine Garantie zusagen, die nicht eingehalten werden kann.

## Resource Reservation

Realtime Executions können Ressourcen reservieren.

Beispiele:

```text
CPU Time
Memory
IO Bandwidth
Network Bandwidth
Interrupt Capacity
Device Access
```

Reservierte Ressourcen besitzen Vorrang vor adaptiver Optimierung.

## Temporal Isolation

Realtime Workloads müssen vor unkontrollierter Beeinflussung durch andere Workloads geschützt werden können.

```text
Realtime Execution
       ↓
Temporal Isolation
       ↓
Bounded Interference
```

Dies kann betreffen:

```text
CPU
Memory
Cache
IO
Interrupts
Network
Devices
```

## Scheduling

Realtime Profiles beeinflussen Scheduling Policy und Priorität.

```text
Hard Realtime
     ↓
Firm Realtime
     ↓
Soft Realtime
     ↓
Normal Workload
```

Diese Reihenfolge beschreibt zeitliche Anforderungen und ersetzt keine Security- oder Safety-Prioritäten.

## Deadline Handling

NovaOS muss Deadline-Verletzungen profilabhängig behandeln können.

```text
Soft
→ Continue / Degrade

Firm
→ Discard / Cancel / Degrade

Hard
→ Contract Violation / Fail-safe Action
```

## Determinismus

Hard Realtime kann deterministische oder begrenzte Ausführung verlangen.

Begrenzbar sein müssen insbesondere:

```text
Scheduling Delay
Interrupt Latency
Memory Allocation
Page Faults
Blocking
IO Latency
Lock Contention
```

Unbegrenzte Operationen dürfen nicht Teil eines zugesicherten Hard-Realtime-Pfads sein.

## Memory

Hard-Realtime-Ausführung soll kritische Speicheroperationen vorbereiten können.

Beispiele:

```text
Preallocated Memory
Pinned Pages
Reserved Buffers
Prebuilt Page Tables
Bounded Allocators
```

Unvorhersehbares Reclaim oder Swapping muss im kritischen Pfad vermeidbar sein.

## IO

Realtime IO benötigt kontrollierte Ressourcen.

```text
Realtime IO
    ↓
IO Reservation
    ↓
Priority / Deadline Scheduling
    ↓
Device
```

Eine CPU-Garantie allein reicht nicht für eine End-to-End-Realtime-Garantie.

## Adaptive Systeme

Adaptive Systeme dürfen Realtime-Ausführung optimieren, aber keine zugesicherten Grenzen gefährden.

```text
Realtime Guarantees
        >
Adaptive Optimization
```

Prediction darf zur Vorbereitung genutzt werden, aber Hard-Realtime-Garantien dürfen nicht von unsicheren Vorhersagen abhängen.

## Energy und Thermal

```text
Safety
  ↓
Hard Realtime Guarantee
  ↓
Energy Optimization
```

Kann eine Garantie aufgrund zwingender thermischer oder physischer Grenzen nicht mehr eingehalten werden, muss dies als Verlust der Garantie behandelt werden.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
Realtime Profile
Deadline
Execution Budget
Latency Bound
Jitter Bound
Reservations
Admission State
Deadline Misses
Guarantee State
Resource Pressure
```

## Normative Anforderungen

1. NovaOS MUSS standardisierte Realtime Profiles unterstützen.
2. Mindestens `None`, `Soft`, `Firm` und `Hard Realtime` MÜSSEN unterscheidbar sein.
3. Realtime DARF NICHT mit hoher Priorität oder hoher Geschwindigkeit gleichgesetzt werden.
4. Realtime Profiles MÜSSEN in Execution Contracts integrierbar sein.
5. Firm- und Hard-Realtime-Anforderungen MÜSSEN durch Admission Control prüfbar sein.
6. NovaOS DARF keine nicht erfüllbare Realtime-Garantie zusagen.
7. Hard Realtime MUSS notwendige Ressourcen reservieren können.
8. Realtime Workloads MÜSSEN temporal isolierbar sein.
9. Deadline Misses MÜSSEN entsprechend dem Realtime Profile behandelbar sein.
10. Hard-Realtime-Pfade DÜRFEN NICHT von unbegrenzt blockierenden Operationen abhängen.
11. Kritische Speicherressourcen SOLLEN vor der Realtime-Ausführung vorbereitet werden können.
12. Realtime IO MUSS in End-to-End-Garantien einbezogen werden können.
13. Adaptive Optimierung DARF zugesicherte Realtime-Garantien NICHT verletzen.
14. Hard-Realtime-Garantien DÜRFEN NICHT von unsicheren Predictions abhängen.
15. Energieoptimierung MUSS gegenüber gültigen Hard-Realtime-Garantien zurücktreten, soweit Safety und physische Grenzen dies erlauben.
16. Verlust einer zugesicherten Garantie MUSS explizit erkennbar sein.
17. Realtime Profile und Garantiezustand MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-LATENCY-0001`
- `NPSPEC-EXECUTION-DEADLINE-0001`
- `NPSPEC-EXECUTION-DETERMINISM-0001`
- `NPSPEC-RESOURCE-LATENCY-0001`
- `NPSPEC-RESOURCE-DEADLINE-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESOURCE-GUARANTEE-0001`
- `NPSPEC-RESOURCE-ADMISSION-0001`
- `NPSPEC-SCHEDULER-DEADLINE-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `NPSPEC-IO-DEADLINE-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `ADR-ARCH-0100`

## Ergebnis

```text
Realtime Requirement
        ↓
Realtime Profile
        ↓
Admission Control
        ↓
Reservation + Isolation
        ↓
Realtime Execution
        ↓
Deadline Verification
        ↓
Guarantee State
```

NovaOS erhält damit ein einheitliches Realtime-Profilmodell, das zwischen normalen, Soft-, Firm- und Hard-Realtime-Anforderungen unterscheidet und zeitliche Garantien nur dann zulässt, wenn Ressourcen, Scheduling, Isolation und End-to-End-Ausführung diese tatsächlich tragen können.