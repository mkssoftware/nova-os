# NPSPEC-REALTIME-DEADLINE-0001 – Nova Realtime Deadline

## Status

Angenommen

## Kategorie

Realtime / Deadline / Temporal Guarantees

## Zweck

NovaOS definiert ein einheitliches Deadline-Modell für zeitkritische Executions. Eine Deadline beschreibt den spätesten zulässigen Zeitpunkt, zu dem eine Operation oder Execution einen definierten Zustand erreicht haben muss.

```text
Execution Start
      ↓
Execution
      ↓
Required Completion
      ↓
Deadline
```

## Grundprinzipien

```text
Deadline ≠ Timeout
Deadline ≠ Priority
Deadline ≠ Latency
Deadline ≠ Guaranteed Completion
Earlier Deadline ≠ Higher Authority
Prediction ≠ Deadline Guarantee
```

Eine Deadline wird erst durch Admission Control, Ressourcenreservierung und begrenzbare Ausführung zu einer möglichen Garantie.

## Deadline Model

```text
RealtimeDeadline
├── DeadlineID
├── ExecutionID
├── DeadlineType
├── TargetTime
├── Profile
└── State
```

Optional:

```text
StartTime
RelativeDeadline
Period
ExecutionBudget
LatencyBudget
JitterBound
MissPolicy
ResourceReservation
ExecutionContractID
```

## Deadline Types

NovaOS unterstützt mindestens:

```text
Absolute Deadline
Relative Deadline
Periodic Deadline
Derived Deadline
```

Beispiel:

```text
Start
  ↓
+ 5 ms
  ↓
Relative Deadline
```

## Deadline States

```text
Pending
Active
Met
Missed
Cancelled
Invalid
Unknown
```

```text
Unknown ≠ Met
```

## Realtime Profiles

Die Bedeutung eines Deadline Miss hängt vom Realtime Profile ab.

```text
Soft Realtime
→ Ergebnis weiterhin verwendbar

Firm Realtime
→ verspätetes Ergebnis möglicherweise wertlos

Hard Realtime
→ Deadline Miss = Contract Violation
```

## Admission Control

Vor einer garantierten Deadline muss geprüft werden:

```text
Execution Demand
      +
Available Resources
      +
Worst-Case Delays
      +
Existing Reservations
      ↓
Admission Control
```

Ergebnis:

```text
Accepted
Rejected
Degraded
```

Eine angeforderte Deadline ist keine zugesicherte Deadline.

## Deadline Budget

Eine End-to-End-Deadline kann in Teilbudgets zerlegt werden.

```text
Total Deadline
├── Scheduling
├── Execution
├── IPC
├── IO
├── Network
└── Response
```

Teilbudgets müssen mit der Gesamtdeadline vereinbar sein.

## Scheduling

Realtime Scheduling kann Deadlines direkt berücksichtigen.

```text
Runnable Executions
        ↓
Deadline Evaluation
        ↓
Scheduling Decision
```

Die konkrete Scheduling-Strategie bleibt austauschbar.

## Deadline Propagation

Deadlines müssen über abhängige Operationen weitergegeben werden können.

```text
Execution A
   ↓
IPC
   ↓
Execution B
   ↓
IO
   ↓
Device
```

Dabei darf eine nachgelagerte Operation keine spätere effektive Deadline erhalten, wenn dadurch die ursprüngliche End-to-End-Deadline verletzt würde.

## Distributed Deadlines

Bei verteilter Execution müssen berücksichtigt werden:

```text
Network Delay
Remote Scheduling
Remote Execution
Clock Properties
Serialization
Response Path
```

Eine verteilte Deadline darf keine perfekte globale Uhr voraussetzen.

## Deadline Miss

NovaOS muss Deadline Misses explizit erkennen.

```text
Current Time > Deadline
        ↓
Deadline Miss
```

Mögliche Reaktionen:

```text
Soft
→ Continue / Degrade

Firm
→ Cancel / Discard / Degrade

Hard
→ Contract Violation / Fail-safe
```

## Cancellation

Ist eine Deadline nicht mehr erreichbar, kann frühzeitige Cancellation sinnvoll sein.

```text
Deadline Cannot Be Met
        ↓
Cancel Remaining Work
        ↓
Release Resources
```

Cancellation muss Structured-Concurrency- und Cleanup-Regeln beachten.

## Resource Reservation

Garantierte Deadlines können Reservierungen benötigen:

```text
CPU
Memory
IO
Network
Device
Interrupt Capacity
```

Eine CPU-Reservation allein garantiert keine End-to-End-Deadline.

## Priority Inversion

Blocking durch niedrigere Prioritäten muss begrenzt werden.

```text
Realtime Task
     ↓ waits
Lower Priority Task
```

Priority Inheritance oder vergleichbare Mechanismen sollen solche Verzögerungen kontrollieren.

## Adaptive Systeme

Adaptive Mechanismen dürfen die Wahrscheinlichkeit einer frühzeitigen Fertigstellung verbessern.

Beispiele:

```text
Prediction
Preload
Prefetch
Adaptive Scheduling
Placement
Provider Selection
```

Hard-Realtime-Deadlines dürfen jedoch nicht von spekulativen Optimierungen abhängen.

## Determinismus

Für kritische Deadlines können erforderlich sein:

```text
Bounded Execution
Bounded Blocking
Fixed Resource Reservation
Controlled Scheduling
Fixed Provider
Deterministic Execution
```

## Deadline Verification

Nach Abschluss wird geprüft:

```text
CompletionTime <= Deadline
        ↓
Met

CompletionTime > Deadline
        ↓
Missed
```

`Completed` darf nicht automatisch als `Deadline Met` interpretiert werden.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
DeadlineID
ExecutionID
Deadline Type
Target Time
Remaining Time
Realtime Profile
Budget
Reservation
Admission State
Deadline State
Miss Count
Miss Reason
```

## Normative Anforderungen

1. NovaOS MUSS Deadlines als eigenständige zeitliche Constraints modellieren.
2. Deadline DARF NICHT mit Timeout, Priority oder Latency gleichgesetzt werden.
3. Absolute, relative und periodische Deadlines MÜSSEN unterstützt werden können.
4. Eine angeforderte Deadline DARF NICHT automatisch als Garantie gelten.
5. Garantierte Deadlines MÜSSEN Admission Control durchlaufen.
6. Nicht erfüllbare Hard-Realtime-Deadlines DÜRFEN NICHT zugesichert werden.
7. End-to-End-Deadlines SOLLEN in Teilbudgets zerlegbar sein.
8. Deadlines MÜSSEN über abhängige Executions, IPC und IO propagierbar sein.
9. Deadline Propagation DARF die ursprüngliche End-to-End-Anforderung NICHT stillschweigend abschwächen.
10. Distributed Deadlines DÜRFEN keine perfekte globale Uhr voraussetzen.
11. Deadline Misses MÜSSEN explizit erkannt werden.
12. Deadline Misses MÜSSEN entsprechend dem Realtime Profile behandelbar sein.
13. Nicht mehr sinnvoll abschließbare Arbeit SOLL kontrolliert cancellierbar sein.
14. Cancellation MUSS Structured-Concurrency- und Cleanup-Regeln respektieren.
15. Hard-Realtime-Deadlines DÜRFEN NICHT von unsicheren Predictions abhängen.
16. `Completed` DARF NICHT automatisch als `Deadline Met` gelten.
17. Deadline-Erfüllung MUSS nach Abschluss verifizierbar sein.
18. Deadline-Zustände und Verletzungen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-REALTIME-PROFILE-0001`
- `NPSPEC-REALTIME-SCHEDULING-0001`
- `NPSPEC-REALTIME-LATENCY-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-DEADLINE-0001`
- `NPSPEC-EXECUTION-LATENCY-0001`
- `NPSPEC-RESOURCE-DEADLINE-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESOURCE-GUARANTEE-0001`
- `NPSPEC-RESOURCE-ADMISSION-0001`
- `NPSPEC-SCHEDULER-DEADLINE-0001`
- `NPSPEC-IO-DEADLINE-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-0001`
- `NPSPEC-CONCURRENCY-DEADLINE-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `ADR-ARCH-0103`

## Ergebnis

```text
Deadline Requirement
        ↓
Admission Control
        ↓
Budget + Reservation
        ↓
Deadline Propagation
        ↓
Realtime Execution
        ↓
Completion
        ↓
Deadline Verification
        ↓
Met / Missed
```

NovaOS erhält damit ein durchgängiges Deadline-Modell, das zeitliche Anforderungen vom ursprünglichen Execution Contract über Scheduling, IPC, IO und verteilte Ausführung bis zur abschließenden Verifikation erhalten kann.