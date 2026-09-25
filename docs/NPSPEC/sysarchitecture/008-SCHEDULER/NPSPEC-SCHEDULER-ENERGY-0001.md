# NPSPEC-SCHEDULER-ENERGY-0001 – Nova Energy-Aware Scheduling

## Status

Angenommen

## Kategorie

Scheduler / Energy / Power Management / Resource Economy

## Zweck

NovaOS definiert Energy-aware Scheduling, damit Tasks unter Berücksichtigung von Energieverbrauch, Leistungsbedarf und Hardwarezustand auf geeigneten CPUs ausgeführt werden können.

```text
Runnable Task
      ↓
Performance Requirements
      +
Energy State
      ↓
Scheduler
      ↓
Efficient CPU Placement
```

Ziel ist nicht minimaler Energieverbrauch um jeden Preis, sondern die effizienteste Ausführung innerhalb der geltenden Anforderungen.

## Grundprinzip

```text
Required Performance
        +
Energy Cost
        +
Execution Constraints
        ↓
Scheduling Decision
```

Dabei gilt:

```text
Energy Efficiency ≠ Lowest Performance
```

Energieoptimierung ist grundsätzlich eine Soft Optimization und darf harte Anforderungen nicht verletzen.

## Energie-Modell

Der Scheduler darf Informationen über den aktuellen Energiezustand verwenden.

Beispiele:

```text
CPU Performance State
CPU Idle State
Core Utilization
Frequency
Thermal State
Power Domain
Battery State
Energy Budget
```

Hardwaredetails werden über abstrahierte HAL- und Power-Management-Schnittstellen bereitgestellt.

## CPU-Platzierung

Bei mehreren geeigneten CPUs kann der Scheduler den energieeffizientesten Ausführungsort bevorzugen.

```text
Hard Constraints
      ↓
Eligible CPUs
      ↓
Performance Requirement
      ↓
Energy Cost
      ↓
Target CPU
```

## Core Consolidation

Bei geringer Systemlast darf NovaOS Tasks auf weniger CPUs konzentrieren.

```text
Low Load

CPU 0 ███
CPU 1 ██
CPU 2 █
CPU 3 █

      ↓

CPU 0 ███████
CPU 1 Idle
CPU 2 Idle
CPU 3 Idle
```

Dadurch können ungenutzte CPUs tiefere Energiesparzustände erreichen.

Consolidation darf jedoch keine relevanten Latenz-, Realtime- oder Thermal-Anforderungen verletzen.

## Performance Scaling

Der Scheduler darf mit CPU-Frequenz- und Performance-Management zusammenarbeiten.

```text
Task Demand
    ↓
Required Performance
    ↓
CPU Performance State
```

Hohe Frequenzen sollen nur verwendet werden, wenn sie für die aktuelle Arbeitslast sinnvoll sind.

Der Scheduler steuert dabei nicht zwingend direkt die Hardwarefrequenz, sondern liefert Anforderungen und Scheduling-Informationen an das Power Management.

## Heterogene CPUs

NovaOS muss unterschiedliche CPU-Leistungsklassen berücksichtigen können.

Beispiel:

```text
Performance Core
Efficiency Core
```

Tasks können abhängig von ihren Anforderungen geeigneten Core-Klassen zugeordnet werden.

```text
Background Task
      ↓
Efficiency Core

Latency-Critical Task
      ↓
Performance Core
```

Die konkrete Hardwareklassifikation bleibt plattformabhängig.

## Migration

Eine Task-Migration ausschließlich zur Energieeinsparung muss deren Kosten berücksichtigen.

```text
Expected Energy Saving
        -
Migration Cost
Cache Loss
NUMA Cost
Wakeup Cost
```

Häufiges Verschieben zwischen CPUs soll vermieden werden.

## Idle States

Der Scheduler soll längere zusammenhängende Idle-Phasen ermöglichen, wenn dies ohne Verletzung anderer Anforderungen möglich ist.

```text
Fragmented Idle Time
        ↓
Task Consolidation
        ↓
Longer Idle Window
        ↓
Deeper Idle State
```

## Realtime

Realtime-Anforderungen haben Vorrang vor Energieoptimierung.

```text
Realtime Constraint
        ↓
Required Resources
        ↓
Execution
        ↓
Energy Optimization
```

NovaOS darf keine Frequenzreduktion, Core-Konsolidierung oder Migration durchführen, die garantierte Realtime-Eigenschaften gefährdet.

## Deadlines

Tasks mit Deadlines können höhere Performance-Zustände benötigen.

```text
Deadline Pressure
      ↑
Required Performance
      ↑
Energy Consumption
```

Nach Ende der zeitkritischen Phase darf das System wieder energieeffizientere Zustände bevorzugen.

## Thermal State

Energie- und Thermal-Management sind miteinander verbunden.

```text
Temperature
    ↓
Thermal Constraint
    ↓
Scheduler Placement
```

Bei thermischer Begrenzung darf der Scheduler Last verteilen, verschieben oder reduzieren, sofern Hard Requirements dies erlauben.

## Ressourcenbudgets

Execution Contracts oder System-Policies können Energieanforderungen definieren.

Beispiele:

```text
Energy Budget
Performance Preference
Latency Requirement
Battery-Saving Preference
```

Diese Anforderungen werden gemeinsam mit anderen Ressourcenconstraints ausgewertet.

## Adaptive Optimierung

NovaOS darf aus beobachtetem Task-Verhalten lernen.

```text
Task
 ↓
Observed Runtime
Energy Cost
CPU Utilization
Wakeup Behaviour
 ↓
Future Placement
```

Adaptive Energieoptimierung bleibt den systemweiten Constraint-Prioritäten untergeordnet.

## Normative Anforderungen

1. NovaOS SOLL energiebezogene Informationen bei Scheduling-Entscheidungen berücksichtigen.
2. Hard Constraints MÜSSEN vor Energieoptimierungen ausgewertet werden.
3. Realtime-Garantien DÜRFEN durch Energieoptimierung NICHT verletzt werden.
4. Deadline-Anforderungen MÜSSEN Vorrang vor weichen Energiepräferenzen haben.
5. Der Scheduler SOLL CPU-Konsolidierung zur Erzeugung längerer Idle-Phasen unterstützen können.
6. Heterogene CPU-Leistungsklassen SOLLEN berücksichtigt werden können.
7. Migrationen zur Energieoptimierung SOLLEN nur bei ausreichendem erwarteten Nutzen erfolgen.
8. Cache- und NUMA-Kosten MÜSSEN bei energiebedingten Migrationen berücksichtigt werden.
9. Thermal Constraints MÜSSEN bei der CPU-Platzierung berücksichtigt werden können.
10. Energie- und Performance-Budgets aus Execution Contracts MÜSSEN respektiert werden.
11. Adaptive Energieoptimierung DARF Hard Constraints NICHT überschreiben.
12. Energiebezogene Scheduling-Entscheidungen SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SCHEDULER-TOPOLOGY-0001`
- `NPSPEC-SCHEDULER-NUMA-0001`
- `NPSPEC-SCHEDULER-CACHE-0001`
- `NPSPEC-SCHEDULER-DEADLINE-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-SCHED-0009`
- `ADR-REALTIME-0009_Realtime_Constraints_vor_Energieoptimierung`

## Ergebnis

```text
Runnable Task
      ↓
Hard Constraints
      ↓
Performance Requirement
      ↓
Topology + Load + Energy State
      ↓
Energy-Aware Placement
      ↓
Efficient Execution
```

NovaOS erhält damit ein Energy-aware Scheduling-Modell, das Performance und Energieverbrauch gemeinsam optimiert, dabei aber Realtime-, Deadline-, Ressourcen- und andere harte Anforderungen konsequent priorisiert.