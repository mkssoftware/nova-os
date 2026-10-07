# NPSPEC-POWER-ENERGYBUDGET-0001 – Nova Energy Budget

## Status

Angenommen

## Kategorie

Power / Energy Budget

## Zweck

NovaOS definiert Energy Budgets als begrenzte Energiemengen, die Workloads, Tasks, Apps, Solutions, Services oder anderen Ausführungskontexten für einen bestimmten Zeitraum oder eine Operation zugeordnet werden können.

Energy Budgets integrieren Energie als planbare Ressource in die Nova Resource Economy und Execution Contracts.

## Grundprinzipien

```text
Energy Budget ≠ Power Limit
Energy Budget ≠ Battery Level
Energy Budget ≠ Permission
Energy Budget ≠ Guaranteed Energy
Budget Exhaustion ≠ Immediate Termination
Estimated Usage ≠ Measured Usage
```

## Modell

```text
EnergyBudget
├── BudgetID
├── SubjectID
├── EnergyLimit
├── TimeScope
├── Priority
├── Hardness
├── ConsumedEnergy
├── RemainingEnergy
└── State
```

Ein `SubjectID` kann beispielsweise referenzieren:

```text
Task
Process
Program
Solution
Service
Workspace
Session
System Component
```

## Budgettypen

NovaOS unterscheidet mindestens:

```text
Hard Budget
Soft Budget
Advisory Budget
```

### Hard Budget

Darf nicht überschritten werden, sofern keine höhere Sicherheits- oder Systemanforderung dies zwingend erforderlich macht.

### Soft Budget

Darf kontrolliert überschritten werden, wenn dies für Performance, Deadline oder Benutzeranforderungen sinnvoll ist.

### Advisory Budget

Dient primär als Optimierungsziel.

## Zeitbezug

Ein Energy Budget kann gelten für:

```text
Operation
Task Lifetime
Execution Window
Session
Time Interval
Battery Cycle
Custom Scope
```

Energy Budget und Power Limit bleiben getrennt:

```text
Energy = Power × Time
```

Ein Workload kann kurzzeitig hohe Leistung verwenden und dennoch innerhalb seines Energie-Budgets bleiben.

## Execution Contract

Energy Budgets können Bestandteil eines Execution Contracts sein:

```text
Execution Contract
├── Performance
├── Latency
├── Deadline
├── Resource Budget
└── Energy Budget
```

NovaOS muss Zielkonflikte zwischen Energie, Performance und Latenz anhand der Constraint-Prioritäten behandeln.

## Accounting

```text
Energy Budget
      ↕
Energy Accounting
      ↓
Consumed Energy
      ↓
Remaining Budget
```

Gemessene Werte sind zu bevorzugen.

Falls nur Schätzungen verfügbar sind, muss deren Unsicherheit berücksichtigt werden.

## Budgethierarchie

Budgets dürfen hierarchisch organisiert werden:

```text
System Budget
    ↓
Workspace Budget
    ↓
App / Solution Budget
    ↓
Task Budget
```

Unterbudgets dürfen das verfügbare Budget ihres übergeordneten Kontexts nicht unkontrolliert erweitern.

## Reaktion auf Budgetdruck

Bei sinkendem verbleibendem Budget darf NovaOS abgestuft reagieren:

```text
Optimize
  ↓
Reduce Background Work
  ↓
Prefer Efficient Resources
  ↓
Reduce Performance
  ↓
Defer Optional Work
  ↓
Reject New Optional Work
```

Hard Constraints und Safety bleiben vorrangig.

## Ressourcenwahl

Energy Budgets dürfen die Auswahl beeinflussen von:

```text
CPU Class
Performance Level
Accelerator
Execution Location
Storage Strategy
Network Interface
Background Scheduling
```

Die energieärmste Variante ist nicht automatisch zulässig, wenn sie andere Hard Constraints verletzt.

## Budgetänderung

Budgets dürfen zur Laufzeit angepasst werden durch:

```text
User Decision
Policy
Power Source Change
Battery State
Execution Contract Change
System Constraint
```

Änderungen müssen nachvollziehbar bleiben.

## Normative Anforderungen

1. NovaOS MUSS Energie als budgetierbare Ressource behandeln können.
2. Energy Budget und Power Limit MÜSSEN getrennte Konzepte bleiben.
3. Energy Budgets MÜSSEN einem definierten Subject und Scope zugeordnet sein.
4. Hard-, Soft- und Advisory-Budgets MÜSSEN unterscheidbar sein.
5. Verbrauch MUSS über Energy Accounting mit dem Budget vergleichbar sein.
6. Geschätzter und gemessener Verbrauch MÜSSEN unterscheidbar bleiben.
7. Energy Budgets MÜSSEN Bestandteil von Execution Contracts sein können.
8. Budgets MÜSSEN hierarchisch delegiert und begrenzt werden können.
9. Unterbudgets DÜRFEN übergeordnete Hard Limits nicht unkontrolliert erweitern.
10. Budgetdruck MUSS abgestufte Optimierungsmaßnahmen ermöglichen.
11. Safety und andere Hard Constraints DÜRFEN durch Energieoptimierung nicht verletzt werden.
12. BudgetID, Limit, Verbrauch, Restbudget, Scope und aktive Maßnahmen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-ENERGYACCOUNTING-0001`
- `NPSPEC-POWER-BATTERY-0001`
- `NPSPEC-POWER-CPUPERFORMANCE-0001`
- `NPSPEC-POWER-ACCELERATOR-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann Energie wie andere knappe Systemressourcen budgetieren. Workloads erhalten definierte Energy Budgets, deren tatsächlicher Verbrauch über Energy Accounting verfolgt wird. Scheduler, Power Management und Resource Economy können daraufhin Ausführung, Performance und Ressourcenwahl optimieren, ohne höhere Sicherheits- oder Systemanforderungen zu verletzen.