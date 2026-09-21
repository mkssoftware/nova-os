# ADR-OBSERVABILITY-0007 – Explainable Resource Allocation

## Status

Angenommen

## Kategorie

Observability Architecture / Resource Allocation

## Kontext

NovaOS verwaltet Ressourcen systemweit und kann sie automatisch reservieren, zuweisen, begrenzen oder zwischen Executions verteilen.

Betroffene Ressourcen sind beispielsweise:

```text
CPU
Memory
Storage
Network
I/O
GPU / NPU / Accelerator
Energy
Remote Resources
```

Für Diagnose, Scheduling, Resource Economy und Self-Healing muss nachvollziehbar sein, **warum eine Ressource einer bestimmten Execution zugewiesen oder verweigert wurde**.

## Entscheidung

NovaOS macht relevante Entscheidungen der Resource Allocation **erklärbar und tracebar**.

```text
Resource Requests
       ↓
Requirements
       ↓
Available Resources
       ↓
Allocation Policy
       ↓
Allocation Decision
       ↓
Decision Trace
```

Die Resource Allocation muss die wesentlichen Gründe einer Entscheidung strukturiert bereitstellen können.

## Allocation Decisions

Relevante Entscheidungen umfassen:

```text
Reserve
Allocate
Deny
Limit
Throttle
Release
Reallocate
```

Jede relevante Entscheidung kann mit:

```text
Decision ID
Execution ID
Resource ID
Trace ID
```

korreliert werden.

## Decision Reasons

Resource-Entscheidungen sollen stabile Reason Codes verwenden.

Beispiele:

```text
ResourceAvailable
ResourceUnavailable
BudgetExceeded
ReservationConflict
HigherPriorityReservation
HardwareRequirement
LocalityConstraint
EnergyConstraint
ProviderUnavailable
ContractConstraint
SecurityConstraint
```

Menschenlesbare Erklärungen können daraus erzeugt werden.

## Requested, Reserved und Used

Explainable Resource Allocation unterscheidet klar zwischen:

```text
Requested
Reserved
Allocated
Used
Released
```

Beispiel:

```text
Requested Memory: 4 GiB
Reserved Memory: 4 GiB
Peak Used Memory: 2.7 GiB
```

Eine Reservierung darf nicht als tatsächlicher Verbrauch interpretiert werden.

## Execution Contract

Resource Allocation wird durch den `Nova.ExecutionContract` begrenzt.

```text
Execution Contract
       ↓
Resource Requirements
       ↓
Resource Allocation
```

Hard Resource Requirements müssen von Soft Preferences unterscheidbar bleiben.

## Hard Constraints

Hard Constraints dürfen durch Resource Allocation nicht verletzt werden.

Beispiele:

```text
Maximum Memory
Required GPU Memory
Hard CPU Budget
Energy Budget
Required Hardware
Realtime Reservation
```

Kann eine notwendige Ressource nicht bereitgestellt werden:

```text
Required Resource
      ↓
Unavailable
      ↓
Allocation Rejected
```

Der Constraint darf nicht stillschweigend abgeschwächt werden.

## Konkurrenz um Ressourcen

Konkurrieren mehrere Executions um dieselbe Ressource, soll nachvollziehbar sein, warum eine bestimmte Allocation erfolgte.

```text
Execution A ─┐
Execution B ─┼→ Limited Resource
Execution C ─┘
                  ↓
            Allocation Policy
                  ↓
             Allocation
```

Relevante Faktoren können sein:

```text
Priority
Deadline
Reservation
Fairness
Resource Budget
Realtime Requirement
System Policy
```

## Scheduling Integration

Scheduling und Resource Allocation werden miteinander korreliert.

```text
Resource Allocation
        ↕
Scheduling Decision
```

Dadurch kann unterschieden werden zwischen:

```text
Execution wartet wegen Scheduling
Execution wartet wegen fehlender Ressource
Execution wartet wegen Reservation Conflict
```

## Distributed Resources

Bei Distributed Execution müssen auch entfernte Ressourcen erklärbar zugewiesen werden.

```text
Local Resources
      +
Remote Resources
      +
Network Cost
      +
Data Locality
      ↓
Allocation Decision
```

Location Transparency darf die tatsächliche Resource Location nicht verbergen.

## Reallocation

Ändert sich der Systemzustand, kann eine Ressource neu zugewiesen werden.

```text
Current Allocation
       ↓
State Change
       ↓
Reallocation
       ↓
New Allocation
```

Mögliche Gründe:

```text
Provider Failure
Resource Exhaustion
Priority Change
Deadline Risk
Thermal Constraint
Energy Constraint
Node Failure
```

Der Grund der Reallocation muss nachvollziehbar sein.

## Resource Tracing

Explainable Resource Allocation beschreibt die **Entscheidung**.

Resource Tracing beschreibt den **tatsächlichen Verbrauch**.

```text
Allocation Decision
       ↓
Execution
       ↓
Resource Trace
       ↓
Actual Usage
```

Beide Informationen müssen miteinander korrelierbar sein.

## Adaptive Resource Allocation

Adaptive Modelle dürfen zukünftige Allocation Decisions verbessern.

```text
Predicted Usage
      ↓
Allocation
      ↓
Measured Usage
      ↓
Prediction Error
      ↓
Model Adjustment
```

Adaptive Modelle dürfen ausschließlich innerhalb gültiger Hard Constraints optimieren.

## System State Graph

Resource Allocations sollen im System State Graph sichtbar sein.

```text
Execution
    │
 reserves
    ↓
Resource
    │
 located_on
    ↓
Provider / Device
```

Damit können Abhängigkeiten und Auswirkungen von Ressourcenproblemen analysiert werden.

## Self-Healing

`Nova.Resilience` kann Allocation Decisions verwenden, um Ursachen zu unterscheiden:

```text
Resource Exhaustion
Reservation Conflict
Provider Failure
Budget Violation
Resource Leak
Scheduling Conflict
```

Resource Allocation selbst bestimmt keine allgemeine Recovery Policy.

## Normative Anforderungen

1. NovaOS MUSS relevante Resource-Allocation-Entscheidungen nachvollziehbar machen können.
2. Allocation Decisions MÜSSEN mit Execution und Resource korrelierbar sein.
3. Relevante Entscheidungen SOLLEN stabile Reason Codes verwenden.
4. Requested, Reserved, Allocated, Used und Released MÜSSEN unterscheidbar sein.
5. Hard Resource Requirements MÜSSEN von Soft Preferences unterscheidbar bleiben.
6. Hard Constraints DÜRFEN durch Resource Allocation nicht abgeschwächt werden.
7. Konkurrenzentscheidungen um begrenzte Ressourcen SOLLEN erklärbar sein.
8. Scheduling und Resource Allocation MÜSSEN korrelierbar sein.
9. Distributed Resource Allocation MUSS Remote Location und relevante Transferkosten berücksichtigen können.
10. Reallocation MUSS den auslösenden Grund nachvollziehbar machen.
11. Resource Allocation und tatsächlicher Resource Usage MÜSSEN korrelierbar sein.
12. Adaptive Allocation MUSS ihren Einfluss auf relevante Entscheidungen kenntlich machen können.
13. Adaptive Modelle DÜRFEN Hard Constraints nicht verändern.
14. Resource Allocations SOLLEN im System State Graph darstellbar sein.
15. Explainable Resource Allocation MUSS für Diagnose, Introspection und Resilience nutzbar sein.

## Konsequenzen

### Positive Konsequenzen

- Ressourcenentscheidungen werden nachvollziehbar,
- Resource Starvation und Scheduling-Probleme können unterschieden werden,
- Budget- und Reservation-Konflikte werden sichtbar,
- adaptive Resource Allocation kann überprüft werden,
- Self-Healing erhält präzisere Ursacheninformationen.

### Negative Konsequenzen

- Resource Manager müssen strukturierte Decision Reasons erzeugen,
- sehr feingranulare Allocation Decisions können hohe Observability-Datenmengen erzeugen,
- verteilte Resource Allocation erhöht die Korrelation komplexer Zustände.

## Abhängigkeiten

- `ADR-OBSERVABILITY-0001_Unified_Logging_Metrics_Tracing`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-OBSERVABILITY-0005_System_State_Graph`
- `ADR-OBSERVABILITY-0006_Explainable_Scheduling`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0013_Introspection`
- `ADR-EXECUTION-0004_Resource_Budgets_im_ExecutionContract`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-DISTRIBUTED-0003_Distributed_Scheduling`
- `ADR-RESILIENCE-0015_Resource_Exhaustion_Recovery`

## Zugehörige NPSPECs

- `NPSPEC-OBSERVABILITY-RESOURCE-ALLOCATION-0001`
- `NPSPEC-OBSERVABILITY-RESOURCE-ALLOCATION-DECISION-0001`
- `NPSPEC-OBSERVABILITY-RESOURCE-ALLOCATION-REASON-0001`
- `NPSPEC-OBSERVABILITY-RESOURCE-ALLOCATION-CORRELATION-0001`

## Ergebnis

NovaOS macht Resource Allocation nachvollziehbar:

```text
Resource Request
       ↓
Contract + Policy
       ↓
Available Resources
       ↓
Allocation Decision
       ↓
Decision Reason
       ↓
Execution
       ↓
Measured Resource Usage
```

Der zentrale Grundsatz lautet:

```text
NovaOS soll nicht nur wissen,
welche Ressource zugewiesen wurde.

Es soll erklären können,
warum sie zugewiesen,
begrenzt oder verweigert wurde
und welche Execution
davon betroffen ist.
```