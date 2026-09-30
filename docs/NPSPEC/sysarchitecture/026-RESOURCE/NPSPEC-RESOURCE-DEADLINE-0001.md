# NPSPEC-RESOURCE-DEADLINE-0001 – Nova Deadline Resource Model

## Status

Angenommen

## Kategorie

Resource / Deadline / Temporal Resource Management

## Zweck

NovaOS definiert Deadlines als systemweit propagierbare zeitliche Ausführungsanforderungen.

```text
Operation
   ↓
Deadline
   ↓
ExecutionContract
   ↓
Scheduling + Resource Planning
   ↓
Execution
   ↓
Completion / Deadline Miss
```

Eine Deadline beschreibt den spätesten zulässigen Abschlusszeitpunkt einer Operation und ermöglicht NovaOS, Ressourcenentscheidungen an zeitlichen Anforderungen auszurichten.

## Grundprinzipien

```text
Deadline ≠ Timeout
Deadline ≠ Latency
Deadline ≠ Priority
Deadline ≠ Execution Budget
Deadline ≠ Guaranteed Completion
High Priority ≠ Deadline Guarantee
Deadline Miss ≠ Automatic Retry
```

## Deadline-Modell

Eine Deadline wird beschrieben durch:

```text
DeadlineRequirement
├── Deadline
├── Deadline Class
└── Clock Domain
```

Optional:

```text
Start Time
Remaining Time
Latency Budget
Execution Budget
Jitter Requirement
Miss Policy
Safety Criticality
```

## Deadline-Klassen

NovaOS unterscheidet mindestens:

```text
Hard
Firm
Soft
Preferred
```

### Hard

Das Ergebnis ist nach Überschreiten der Deadline nicht mehr zulässig.

### Firm

Ein verspätetes Ergebnis besitzt keinen oder stark reduzierten Nutzen.

### Soft

Verspätungen sind zulässig, verschlechtern jedoch die Dienstqualität.

### Preferred

Die Deadline ist ein Optimierungsziel und kein Hard Constraint.

## Absolute und relative Deadline

Deadlines können angegeben werden als:

```text
Absolute:
Complete before T

Relative:
Complete within Δt
```

Relative Deadlines müssen beim Eintritt in die Ausführung auf eine definierte Zeitbasis bezogen werden.

## Deadline Propagation

Eine End-to-End-Deadline muss über abhängige Operationen propagiert werden können.

```text
Operation
   ↓
Stage A
   ↓
Stage B
   ↓
Stage C
```

Dabei gilt:

```text
Remaining Deadline =
Original Deadline
-
Elapsed Time
```

Unteroperationen dürfen nicht unabhängig wieder das ursprüngliche vollständige Zeitbudget erhalten.

## Deadline Budgeting

NovaOS kann verbleibende Zeit auf Teiloperationen verteilen.

```text
End-to-End Deadline
├── Compute Budget
├── IPC Budget
├── I/O Budget
└── Network Budget
```

Die Aufteilung kann statisch oder dynamisch erfolgen.

## Scheduling

Der Scheduler kann Deadlines bei Ausführungsentscheidungen berücksichtigen.

```text
Runnable Tasks
      ↓
Deadline + Budget + Policy
      ↓
Scheduler
      ↓
CPU Allocation
```

Eine hohe Priorität ersetzt keine Deadline-Analyse.

## Resource Selection

Provider können anhand der verbleibenden Deadline bewertet werden.

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
Deadline Feasibility
```

Ein schneller Provider darf nicht gewählt werden, wenn dadurch andere Hard Constraints verletzt werden.

## Admission Control

Bei Hard- oder Firm-Deadlines kann NovaOS vor der Ausführung prüfen:

```text
Required Resources
        ↓
Available Capacity
        ↓
Estimated Worst Case
        ↓
Deadline Feasible?
```

Ist eine Hard Deadline nicht realistisch erfüllbar, soll die Operation frühzeitig abgelehnt oder neu geplant werden.

## Deadline Miss

Ein Deadline Miss muss explizit erkennbar sein.

```text
Running
   ↓
Deadline Reached
   ↓
Miss Policy
```

Mögliche Reaktionen:

```text
Continue
Cancel
Degrade
Fallback
Return DeadlineMiss
Trigger Safety Action
```

Die Reaktion hängt von Deadline Class und ExecutionContract ab.

## Cancellation

Das Überschreiten einer Deadline kann Cancellation auslösen.

```text
Deadline Miss
     ↓
Cancellation Request
     ↓
Structured Concurrency
```

Cancellation muss kontrolliert durch abhängige Tasks propagiert werden können.

## Transaktionen

Eine Deadline darf keine inkonsistenten Teilzustände hinterlassen.

```text
Deadline Miss
     ↓
Abort / Compensate / Rollback
```

Transaktionale Operationen müssen ihre definierte Fehlersemantik einhalten.

## Distributed Execution

Bei verteilter Ausführung müssen berücksichtigt werden:

```text
Clock Uncertainty
Network Delay
Queue Delay
Remote Processing
Transport Overhead
```

Eine entfernte Komponente darf eine unbekannte oder abgelaufene Deadline nicht stillschweigend als gültig behandeln.

## Deadline Pressure

NovaOS kann erkennen, wenn eine Deadline gefährdet ist.

```text
Safe
 ↓
At Risk
 ↓
Critical
 ↓
Missed
```

Mögliche Reaktionen:

```text
Increase Scheduling Urgency
Reserve Resources
Change Provider
Reduce Optional Work
Improve Locality
Graceful Degradation
```

## ExecutionContract

Deadlines sind Bestandteil des ExecutionContract.

```text
ExecutionContract
├── Deadline
├── Deadline Class
├── Latency Requirement
├── Resource Budget
├── Determinism
└── Miss Policy
```

Hard Deadlines dürfen durch adaptive Optimierung nicht abgeschwächt werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Deadline
Deadline Class
Remaining Time
Execution Budget
Current State
Risk State
Predicted Completion
Miss Count
Miss Policy
```

## Normative Anforderungen

1. NovaOS MUSS Deadlines als explizite zeitliche Ausführungsanforderungen behandeln.
2. Deadline, Timeout, Latency und Priority MÜSSEN getrennte Konzepte bleiben.
3. NovaOS MUSS Hard-, Firm-, Soft- und Preferred-Deadlines unterscheiden können.
4. Deadlines MÜSSEN über abhängige Operationen propagierbar sein.
5. Unteroperationen DÜRFEN das bereits verbrauchte Zeitbudget NICHT erneut erhalten.
6. Hard-Deadline-Ausführung SOLL Admission Control unterstützen.
7. Deadline Misses MÜSSEN explizit erkennbar sein.
8. Deadline-basierte Cancellation MUSS mit Structured Concurrency integrierbar sein.
9. Adaptive Optimierung DARF Hard Deadlines NICHT abschwächen.
10. Deadline-Zustände MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-RESOURCE-CPU-0001`
- `NPSPEC-RESOURCE-IO-0001`
- `NPSPEC-RESOURCE-NETWORK-0001`
- `NPSPEC-RESOURCE-LATENCY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-SCHED-DEADLINE-0001`
- `NPSPEC-IO-DEADLINE-0001`
- `NPSPEC-DISTCOMM-DEADLINE-0001`
- `ADR-ARCH-0042`

## Ergebnis

```text
End-to-End Deadline
        ↓
Deadline Propagation
        ↓
Budget + Admission Control
        ↓
Scheduling + Resource Selection
        ↓
Execution
        ↓
Completion / Controlled Miss
```

NovaOS erhält damit ein systemweites Deadline-Modell, das zeitliche Anforderungen über Scheduler, I/O, IPC, Netzwerk und Compute-Ressourcen hinweg propagiert und Hard-, Firm- und Soft-Realtime-Ausführung kontrolliert in die Resource Economy integriert.