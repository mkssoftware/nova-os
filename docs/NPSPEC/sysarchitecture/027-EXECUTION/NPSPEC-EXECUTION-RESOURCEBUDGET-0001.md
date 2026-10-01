# NPSPEC-EXECUTION-RESOURCEBUDGET-0001 – Nova Execution Resource Budget

## Status

Angenommen

## Kategorie

Execution / Resource Budget / Resource Economy

## Zweck

NovaOS definiert Resource Budgets als Bestandteil des Execution Contracts.

Ein Resource Budget legt fest, wie viele Systemressourcen eine Ausführung innerhalb eines definierten Bereichs maximal oder garantiert verwenden darf.

```text
ExecutionContract
      ↓
Resource Budget
      ↓
Admission + Planning
      ↓
Execution
      ↓
Accounting
      ↓
Enforcement
```

Damit werden Ressourcenverbrauch, Vorhersagbarkeit und Isolation bereits auf Ebene der Ausführungsanforderung beschrieben.

## Grundprinzipien

```text
Budget ≠ Reservation
Budget ≠ Guarantee
Budget ≠ Allocation
Budget ≠ Consumption
Budget ≠ Capability
Budget ≠ Priority

Budget = erlaubter Ressourcenrahmen
Reservation = bereitgehaltene Kapazität
Guarantee = zugesicherte Eigenschaft
Consumption = tatsächlicher Verbrauch
```

## Resource Budget Model

Ein Execution Resource Budget enthält:

```text
ExecutionResourceBudget
├── BudgetID
├── ExecutionContractID
├── ResourceTypeID
├── Limit
├── Time Window
└── Enforcement Policy
```

Optional:

```text
Minimum
Maximum
Reservation
Burst Limit
Accounting Domain
Deadline
Priority
Parent Budget
Degradation Policy
Failure Policy
```

## Unterstützte Ressourcen

Budgets können mindestens definiert werden für:

```text
CPU Time
Memory
Storage
I/O
Network
GPU
NPU
Energy
Device Usage
```

Beispiel:

```text
CPU:
  Maximum: 50 ms / 100 ms

Memory:
  Maximum: 512 MiB

Network:
  Maximum: 10 MiB

GPU:
  Maximum: 30 ms

Energy:
  Maximum: 20 J
```

## Hard und Soft Budget

NovaOS unterscheidet:

```text
Hard Budget
Soft Budget
```

### Hard Budget

Das definierte Limit darf nicht überschritten werden.

### Soft Budget

Eine Überschreitung kann abhängig von Policy und verfügbarer Kapazität erlaubt werden.

```text
Soft Limit
    ↓
Burst
    ↓
Hard Maximum
```

## Hierarchische Budgets

Budgets können hierarchisch organisiert werden.

```text
System Budget
     ↓
Application Budget
     ↓
Process Budget
     ↓
Execution Budget
     ↓
Task Budget
```

Dabei gilt:

```text
Child Budget ≤ verfügbare Parent Capacity
```

Eine untergeordnete Ausführung darf durch eigene Budgetdefinitionen kein zusätzliches Ressourcenrecht erzeugen.

## Budget Propagation

Erzeugt eine Ausführung Child Tasks, müssen Ressourcenbudgets kontrolliert propagiert werden.

```text
Execution
├── Task A
├── Task B
└── Task C
```

Das Parent Budget kann dabei aufgeteilt werden:

```text
Parent Budget
    ↓
Budget A
+
Budget B
+
Budget C
```

Eine Mehrfachzählung derselben verfügbaren Kapazität muss verhindert werden.

## Reservation Integration

Ein Budget kann mit einer Reservation kombiniert werden.

```text
Budget:      100 ms CPU
Reservation:  40 ms CPU
```

Damit darf die Ausführung maximal 100 ms verwenden, während 40 ms davon reserviert sein können.

```text
Reservation ≤ zulässiger Budgetrahmen
```

Eine Reservation erhöht das Budget nicht automatisch.

## Guarantee Integration

Garantien können auf Budgets aufbauen.

```text
Resource Budget
      +
Reservation
      +
Admission
      ↓
Resource Guarantee
```

Ein Budget allein garantiert keine Ressourcenverfügbarkeit.

## Admission Control

Vor der Ausführung kann NovaOS prüfen:

```text
Requested Budget
      ↓
Parent Budget
      +
Available Resources
      +
Reservations
      +
Guarantees
      ↓
Admission Decision
```

Ein syntaktisch gültiges Budget bedeutet nicht automatisch, dass die Ausführung zugelassen werden kann.

## Accounting

Während der Ausführung wird der tatsächliche Verbrauch dem Budget gegenübergestellt.

```text
Budget
  vs.
Consumption
```

NovaOS unterscheidet mindestens:

```text
Reserved
Allocated
Consumed
Remaining
Exceeded
Released
```

## Enforcement

Bei Annäherung oder Überschreitung eines Limits kann NovaOS reagieren.

```text
Normal
  ↓
Approaching Limit
  ↓
Budget Pressure
  ↓
Exceeded
```

Mögliche Reaktionen:

```text
Throttle
Reduce Parallelism
Reclaim
Degrade
Suspend
Cancel
Fail
```

Die konkrete Reaktion wird durch Resource Type, Contract und Enforcement Policy bestimmt.

## Burst

Soft Budgets können kontrollierte Burst-Nutzung erlauben.

```text
Normal Limit
    ↓
Temporary Burst
    ↓
Burst Limit
```

Burst darf:

- Parent Budgets nicht verletzen,
- Hard Reservations nicht verdrängen,
- Guarantees nicht gefährden,
- Hard System Constraints nicht überschreiten.

## Deadline Integration

Resource Budgets und Deadlines müssen gemeinsam betrachtet werden.

```text
Remaining Budget
       +
Remaining Time
       ↓
Execution Feasibility
```

Eine Deadline darf nicht durch unbegrenzte Ressourcennutzung erzwungen werden.

## Replanning

Ändert sich die Ressourcensituation:

```text
Resource Pressure
Thermal Pressure
Provider Failure
Reservation Loss
```

kann NovaOS innerhalb des Contracts neu planen.

```text
Current Plan
    ↓
Budget Check
    ↓
Alternative Provider
```

Der neue Provider muss weiterhin innerhalb der zulässigen Budgets arbeiten.

## Structured Concurrency

Budgets folgen der Task-Hierarchie.

```text
ExecutionContract
      ↓
Root Task
      ↓
Child Tasks
```

Beendete Tasks müssen ihre nicht mehr benötigten Ressourcen und Reservationen freigeben.

## Security

Ein Resource Budget gewährt keine Zugriffsrechte.

```text
Budget ≠ Capability
```

Beispiel:

```text
Network Budget vorhanden
        ≠
Network Capability vorhanden
```

Für die tatsächliche Ressourcennutzung müssen weiterhin die notwendigen Capabilities vorhanden sein.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
BudgetID
ExecutionContractID
Resource Type
Limit
Consumed
Remaining
Reserved
Burst Usage
Parent Budget
Pressure State
Enforcement State
```

## Normative Anforderungen

1. NovaOS MUSS Resource Budgets als Bestandteil von Execution Contracts unterstützen.
2. Budget, Reservation, Guarantee, Allocation und Consumption MÜSSEN getrennte Konzepte bleiben.
3. Budgets MÜSSEN für unterschiedliche Ressourcenklassen definierbar sein.
4. Hard und Soft Budgets MÜSSEN unterscheidbar sein.
5. Budgets MÜSSEN hierarchisch begrenzbar sein.
6. Child Executions DÜRFEN das verfügbare Parent Budget NICHT überschreiten.
7. Tatsächlicher Ressourcenverbrauch MUSS gegen das zugehörige Budget abrechenbar sein.
8. Budgetüberschreitungen MÜSSEN kontrollierte Enforcement Policies auslösen können.
9. Burst-Nutzung DARF Hard Constraints, Reservations oder Guarantees NICHT verletzen.
10. Replanning MUSS innerhalb der verbleibenden Budgets erfolgen.
11. Ein Resource Budget DARF keine Capability oder Zugriffsautorität erzeugen.
12. Budgetzustand und Verbrauch MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-SEMANTICTYPES-0001`
- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESOURCE-GUARANTEE-0001`
- `NPSPEC-RESOURCE-ADMISSION-0001`
- `NPSPEC-RESOURCE-ARBITRATION-0001`
- `NPSPEC-RESOURCE-RECLAIM-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `ADR-ARCH-0050`

## Ergebnis

```text
ExecutionContract
      ↓
Resource Budgets
      ↓
Admission + Reservation
      ↓
Execution
      ↓
Accounting
      ↓
Budget Enforcement
```

NovaOS erhält damit einen expliziten Ressourcenrahmen für jede Ausführung. Ressourcenverbrauch wird bereits im Execution Contract begrenzt, während Reservation, Guarantee, tatsächlicher Verbrauch und Zugriffsautorität weiterhin klar getrennte Konzepte bleiben.