# NPSPEC-RESOURCE-ECONOMY-0001 – Nova Resource Economy

## Status

Angenommen

## Kategorie

Resource / Economy / Resource Management

## Zweck

NovaOS definiert eine systemweite Resource Economy zur kontrollierten Verteilung begrenzter Ressourcen zwischen Prozessen, Services, Anwendungen, Tasks, Agents und Systemkomponenten.

```text
Resource Demand
      ↓
Budget + Policy
      ↓
Resource Allocation
      ↓
Execution
      ↓
Measurement + Feedback
```

Ressourcen werden nicht als unbegrenzt verfügbare Systemleistung betrachtet, sondern als explizit verwaltete und messbare Güter.

## Grundprinzipien

```text
Available Resource ≠ Authorized Resource
Resource Access ≠ Unlimited Consumption
Priority ≠ Unlimited Budget
Allocation ≠ Ownership
Reservation ≠ Consumption
Budget ≠ Guarantee
Optimization ≠ Policy Override
Unused Budget ≠ Permanent Entitlement
```

## Ressourcenklassen

Die Resource Economy umfasst mindestens:

```text
CPU Time
Memory
Storage
I/O
Network
GPU / Accelerator
Energy
Thermal Budget
Device Time
IPC Capacity
```

Weitere Ressourcenklassen können ergänzt werden.

## Resource Budget

Workloads können explizite Budgets erhalten.

```text
ResourceBudget
├── Resource Type
├── Limit
├── Time Window
└── Enforcement Policy
```

Optional:

```text
Reservation
Burst Limit
Priority
Deadline
Minimum Guarantee
Maximum Consumption
Accounting Domain
Parent Budget
```

## Hierarchische Budgets

Budgets können hierarchisch aufgebaut werden.

```text
System Budget
    ↓
Application Budget
    ↓
Process Budget
    ↓
Task Budget
```

Dabei gilt:

```text
Child Budget ≤ Parent Available Budget
```

Untergeordnete Workloads dürfen die Ressourcenbegrenzung ihres übergeordneten Domains nicht umgehen.

## Reservation

Ressourcen können für kritische Operationen reserviert werden.

```text
Request
  ↓
Reserve
  ↓
Execute
  ↓
Release
```

Reservation und tatsächlicher Verbrauch müssen getrennt erfasst werden.

## Resource Accounting

NovaOS muss Ressourcenverbrauch messbar machen.

```text
Consumer
   ↓
Usage
   ↓
Accounting
```

Accounting kann erfolgen nach:

```text
Identity
Application
Process
Task
Service
Agent
ExecutionContract
Transaction
Security Domain
```

## ExecutionContract

Resource Budgets sind Bestandteil des `Nova.ExecutionContract`.

```text
ExecutionContract
├── CPU Budget
├── Memory Budget
├── I/O Budget
├── Network Budget
├── Energy Budget
└── Accelerator Budget
```

Damit beschreibt eine Operation nicht nur, was ausgeführt werden soll, sondern auch unter welchen Ressourcenbedingungen.

## Resource Resolution

Semantische Ressourcenanforderungen werden gegen verfügbare Ressourcen und Budgets aufgelöst.

```text
Semantic Requirement
        ↓
Resource Discovery
        ↓
Budget Check
        ↓
Policy
        ↓
Allocation
```

Die Existenz einer Ressource bedeutet nicht automatisch, dass sie verwendet werden darf.

## Überlastung

Bei Ressourcenknappheit muss NovaOS kontrolliert reagieren können.

```text
Normal
  ↓
Pressure
  ↓
Throttling
  ↓
Reclaim
  ↓
Degradation
  ↓
Controlled Failure
```

Unkontrollierte globale Ressourcenerschöpfung soll vermieden werden.

## Graceful Degradation

Workloads können alternative Ausführungsprofile definieren.

Beispiel:

```text
Preferred:
GPU Processing

Fallback:
CPU Processing

Reduced:
Lower Resolution

Final:
Controlled Failure
```

Hard Requirements dürfen dabei nicht verletzt werden.

## Priorisierung

Priorität beeinflusst die Ressourcenverteilung, hebt Budgets jedoch nicht automatisch auf.

```text
Priority ≠ Unlimited Resources
```

Priorisierung kann berücksichtigen:

```text
Safety
Realtime
Security
User Interaction
Deadline
System Services
Background Work
```

## Borrowing und Burst

Temporäre Überschreitungen können kontrolliert erlaubt werden.

```text
Base Budget
    +
Temporary Burst
```

Burst-Ressourcen müssen begrenzt, nachvollziehbar und widerrufbar sein.

## Ressourcenfreigabe

Nicht mehr benötigte Ressourcen müssen zeitnah freigegeben werden.

```text
Acquire
  ↓
Use
  ↓
Release
```

Structured Concurrency soll sicherstellen, dass Ressourcen an klar definierte Task-Lebenszyklen gebunden werden können.

## Adaptive Optimierung

NovaOS darf historische Nutzung zur Optimierung verwenden.

```text
Prediction
   ↓
Allocation
   ↓
Actual Usage
   ↓
Prediction Error
   ↓
Model Adjustment
```

Adaptive Optimierung darf niemals Safety-, Security-, Trust-, Sovereignty- oder Hard-Constraint-Regeln überschreiben.

## Capability Security

Ressourcenzugriff benötigt geeignete Capabilities.

```text
Resource Capability
        +
Resource Budget
        ↓
Permitted Consumption
```

Dabei gilt:

```text
Capability ≠ Budget
Budget ≠ Capability
```

Beide Bedingungen können gleichzeitig erforderlich sein.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Available Resources
Allocated Resources
Reserved Resources
Current Usage
Budget
Pressure State
Throttling State
Resource Owner
Accounting Domain
```

## Normative Anforderungen

1. NovaOS MUSS begrenzte Systemressourcen explizit verwalten können.
2. Ressourcenverbrauch MUSS messbar und einem Accounting Domain zuordenbar sein.
3. Resource Budgets MÜSSEN hierarchisch begrenzbar sein.
4. Resource Capability und Resource Budget MÜSSEN getrennte Konzepte bleiben.
5. ExecutionContracts MÜSSEN Ressourcenbudgets definieren können.
6. Ressourcenknappheit MUSS kontrollierte Degradation oder Fehlerbehandlung ermöglichen.
7. Priorität DARF Ressourcenlimits NICHT automatisch aufheben.
8. Temporäre Burst-Ressourcen MÜSSEN begrenzbar und widerrufbar sein.
9. Adaptive Ressourcenoptimierung DARF Hard Requirements und Sicherheitsregeln NICHT überschreiben.
10. Resource Economy MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-SEMANTIC-DISCOVERY-0001`
- `NPSPEC-SEMANTIC-EXECUTION-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-CAPABILITY-RESOURCE-0001`
- `NPSPEC-SCHED-QOS-0001`
- `NPSPEC-MEMORY-PRESSURE-0001`
- `NPSPEC-MEMORY-RECLAIM-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `ADR-ARCH-0031`

## Ergebnis

```text
Resource Supply
      +
Resource Demand
      ↓
Budget + Capability + Policy
      ↓
Controlled Allocation
      ↓
Execution + Accounting
      ↓
Feedback + Adaptation
```

NovaOS erhält damit eine systemweite Resource Economy, die Ressourcenverbrauch explizit budgetiert, misst und kontrolliert und dadurch Performance, Fairness, Vorhersagbarkeit und Systemstabilität miteinander verbindet.