# ADR-OBSERVABILITY-0006 – Explainable Scheduling

## Status

Angenommen

## Kategorie

Observability Architecture / Scheduling

## Kontext

NovaOS trifft Scheduling-Entscheidungen automatisch anhand von:

```text
Priorität
Deadlines
Ressourcen
Abhängigkeiten
Locality
Provider Availability
System Load
Energy
Execution Contracts
```

Für Diagnose, Realtime-Verifikation und Self-Healing muss nachvollziehbar sein, warum eine Execution ausgeführt, verzögert, priorisiert, verschoben oder abgebrochen wurde.

## Entscheidung

NovaOS macht relevante Scheduling-Entscheidungen **erklärbar und tracebar**.

```text
Runnable Executions
        ↓
Constraints
        ↓
Scheduling Policy
        ↓
Scheduling Decision
        ↓
Decision Trace
```

Der Scheduler muss nicht seinen vollständigen internen Zustand protokollieren, sondern die für eine Entscheidung relevanten Gründe strukturiert bereitstellen.

## Scheduling Decision

Relevante Entscheidungen umfassen insbesondere:

```text
Schedule
Delay
Preempt
Resume
Migrate
Reprioritize
Reject
```

Jede relevante Entscheidung kann mit einer `Decision ID` sowie der betroffenen `Execution ID` korreliert werden.

## Decision Reasons

Scheduling-Gründe werden bevorzugt als stabile Reason Codes dargestellt.

Beispiele:

```text
HigherPriorityExecution
DeadlineRequirement
ResourceUnavailable
ResourceReservation
DependencyPending
ProviderUnavailable
LocalityPreference
EnergyPolicy
ContractConstraint
RealtimeGuarantee
```

Menschenlesbare Erklärungen können daraus erzeugt werden.

## Hard Constraints

Hard Requirements müssen als solche erkennbar bleiben.

Beispiel:

```text
Execution A delayed
Reason:
    Required GPU unavailable
```

Der Scheduler darf eine solche Verzögerung nicht als reine Optimierungsentscheidung darstellen.

## Soft Optimization

Auch Soft Requirements können Scheduling beeinflussen.

```text
Execution A
Execution B
    ↓
Both Contract Valid
    ↓
Energy / Locality / Performance
    ↓
Execution B scheduled first
```

Der Decision Trace soll erkennen lassen, welche Soft Requirements relevant waren.

## Priorität und Fairness

Scheduling-Prioritäten und Fairness-Regeln müssen nachvollziehbar sein.

NovaOS soll beispielsweise erklären können:

```text
Warum wartet diese Execution?

Warum wurde eine andere Execution bevorzugt?

Warum wurde eine Execution preempted?

Welche Ressource verhindert die Ausführung?
```

## Deadline und Realtime

Deadline- und Realtime-Entscheidungen müssen besonders eindeutig nachvollziehbar sein.

```text
Execution
    ↓
Deadline
    ↓
Scheduling Decision
    ↓
Measured Completion
```

Deadline Risk und Deadline Miss müssen mit den verursachenden Scheduling-Ereignissen korrelierbar sein.

## Ressourcen

Scheduling-Entscheidungen werden mit Resource Tracing verbunden.

```text
Execution
    ↓
Resource Requirement
    ↓
Reservation / Availability
    ↓
Scheduling Decision
```

Damit kann zwischen fehlenden Ressourcen, Ressourcenkonflikten und Policy-Entscheidungen unterschieden werden.

## Distributed Scheduling

Bei Distributed Execution umfasst Explainable Scheduling auch:

```text
Node Selection
Provider Selection
Placement
Network State
Data Locality
Failure Domain
Remote Resource Availability
```

Die physische Verteilung darf durch Location Transparency nicht verborgen werden.

## Replanning

Ändert sich eine Scheduling-Entscheidung, muss der Grund nachvollziehbar sein.

```text
Original Schedule
       ↓
State Change
       ↓
Replanning
       ↓
New Schedule
```

Beispiele:

```text
Provider Failure
Resource Exhaustion
Deadline Risk
Node Failure
Network Degradation
Contract Change
```

## Deterministic Mode

Im Deterministic Mode müssen relevante Scheduling-Entscheidungen reproduzierbar sein.

Decision Tracing soll dazu relevante Informationen referenzieren können:

```text
Scheduling Policy Version
Defined Scheduling State
Priority
Dependencies
Relevant Resource State
```

## Adaptive Scheduling

Adaptive Modelle dürfen Scheduling innerhalb der zulässigen Grenzen optimieren.

```text
Prediction
    ↓
Scheduling Decision
    ↓
Measured Result
    ↓
Prediction Error
    ↓
Model Adjustment
```

Explainable Scheduling muss erkennen lassen, wenn adaptive Bewertung die Entscheidung beeinflusst hat.

Hard Constraints bleiben unverändert.

## Korrelation

Scheduling Decisions müssen mit anderen Observability-Signalen korrelierbar sein.

```text
Execution ID
   ├── Scheduling Decisions
   ├── Resource Trace
   ├── Distributed Trace
   ├── Metrics
   └── Logs
```

Dadurch kann der vollständige Weg von Planung bis tatsächlicher Ausführung analysiert werden.

## Self-Healing

`Nova.Resilience` kann Scheduling-Erklärungen verwenden, um beispielsweise zu unterscheiden zwischen:

```text
Execution Failure
Resource Starvation
Provider Failure
Scheduling Delay
Deadline Violation
Dependency Block
```

Der Scheduler liefert Gründe, entscheidet aber nicht über allgemeine Recovery Policies.

## Normative Anforderungen

1. NovaOS MUSS relevante Scheduling-Entscheidungen nachvollziehbar machen können.
2. Scheduling Decisions MÜSSEN mit der betroffenen Execution korrelierbar sein.
3. Relevante Entscheidungen SOLLEN stabile Reason Codes verwenden.
4. Hard Constraints MÜSSEN von Soft Optimization unterscheidbar bleiben.
5. Delay, Preemption, Migration und Repriorisierung MÜSSEN erklärbar sein.
6. Prioritäts- und Fairnessentscheidungen SOLLEN nachvollziehbar sein.
7. Deadline- und Realtime-Entscheidungen MÜSSEN tracebar sein.
8. Scheduling und Resource Tracing MÜSSEN korrelierbar sein.
9. Distributed Scheduling MUSS Node-, Provider- und Placement-Entscheidungen erklären können.
10. Replanning MUSS den auslösenden Zustandswechsel referenzieren können.
11. Deterministic Mode MUSS reproduzierbare Scheduling-Entscheidungen unterstützen.
12. Adaptive Scheduling MUSS seinen Einfluss auf Entscheidungen kenntlich machen können.
13. Adaptive Modelle DÜRFEN Hard Constraints nicht verändern.
14. Explainable Scheduling DARF keine Secrets oder unnötigen Nutzdaten offenlegen.
15. Scheduling-Erklärungen MÜSSEN für Introspection, Diagnose und Resilience nutzbar sein.

## Konsequenzen

### Positive Konsequenzen

- Scheduling-Verhalten wird nachvollziehbar,
- Deadline-Probleme lassen sich besser analysieren,
- Resource Starvation und echte Execution-Fehler können unterschieden werden,
- verteilte Placement- und Scheduling-Entscheidungen werden transparent,
- Self-Healing erhält bessere Ursacheninformationen.

### Negative Konsequenzen

- Scheduler müssen strukturierte Entscheidungsinformationen erzeugen,
- sehr häufige Scheduling-Entscheidungen können große Datenmengen erzeugen,
- Detailgrad und Observability-Overhead müssen begrenzt werden.

## Abhängigkeiten

- `ADR-OBSERVABILITY-0001_Unified_Logging_Metrics_Tracing`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-OBSERVABILITY-0005_System_State_Graph`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Introspection`
- `ADR-EXECUTION-0003_Latency_und_Deadline_Requirements`
- `ADR-EXECUTION-0004_Resource_Budgets_im_ExecutionContract`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-DISTRIBUTED-0003_Distributed_Scheduling`
- `ADR-RESILIENCE-0020_Explainable_Self_Healing`

## Zugehörige NPSPECs

- `NPSPEC-OBSERVABILITY-SCHEDULING-0001`
- `NPSPEC-OBSERVABILITY-SCHEDULING-DECISION-0001`
- `NPSPEC-OBSERVABILITY-SCHEDULING-REASON-0001`
- `NPSPEC-OBSERVABILITY-SCHEDULING-CORRELATION-0001`

## Ergebnis

NovaOS macht Scheduling nicht zu einer Black Box:

```text
Runnable Work
     ↓
Constraints
     ↓
Scheduling Policy
     ↓
Decision
     ↓
Decision Reason
     ↓
Execution
     ↓
Measured Result
```

Der zentrale Grundsatz lautet:

```text
NovaOS soll nicht nur wissen,
wann eine Execution ausgeführt wird.

Es soll erklären können,
warum sie genau dann,
auf dieser Ressource
und mit dieser Priorität
ausgeführt wurde.
```