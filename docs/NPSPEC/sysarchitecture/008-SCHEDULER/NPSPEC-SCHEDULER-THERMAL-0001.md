# NPSPEC-SCHEDULER-THERMAL-0001 – Nova Thermal-Aware Scheduling

## Status

Angenommen

## Kategorie

Scheduler / Thermal Management / Energy / Resource Economy

## Zweck

NovaOS definiert Thermal-aware Scheduling, damit Tasks unter Berücksichtigung thermischer Grenzen und verfügbarer Kühlreserven auf geeigneten CPUs ausgeführt werden.

```text
Runnable Tasks
      ↓
Thermal State
      ↓
Scheduler
      ↓
Thermal-Aware Placement
```

Ziel ist, thermische Überlastung zu vermeiden und gleichzeitig möglichst viel nutzbare Rechenleistung bereitzustellen.

## Grundprinzip

Thermische Grenzen sind Ressourcenconstraints.

```text
Performance Demand
       +
Thermal Headroom
       +
Execution Constraints
       ↓
Scheduling Decision
```

Dabei gilt:

```text
Maximum Performance ≠ Sustainable Performance
```

Kurzzeitig verfügbare Leistung darf nicht mit dauerhaft verfügbarer Leistung gleichgesetzt werden.

## Thermal-Modell

NovaOS verwendet abstrahierte thermische Informationen der Plattform.

Relevant können sein:

```text
Temperature
Thermal Limit
Thermal Headroom
Thermal Zone
Power Domain
Cooling State
Throttling State
Temperature Trend
```

Der Scheduler darf nicht von konkreten Sensor- oder Firmwareimplementierungen abhängig sein.

## Thermal Zones

Hardware kann in thermische Zonen unterteilt werden.

```text
System
├── CPU Zone
├── GPU Zone
├── Memory Zone
└── Platform Zone
```

Mehrere Ressourcen können dieselbe thermische Grenze beeinflussen.

## Thermal Headroom

Der Scheduler darf berücksichtigen, wie weit eine Ressource von ihrer thermischen Grenze entfernt ist.

```text
Current Temperature
        ↓
Thermal Limit
        ↓
Available Headroom
```

Geringer Headroom kann die Eignung einer CPU für zusätzliche Last reduzieren.

## Task-Platzierung

Bei mehreren zulässigen CPUs darf thermische Belastung in die Auswahl einfließen.

```text
Hard Constraints
      ↓
Eligible CPUs
      ↓
Topology / NUMA / Cache
      ↓
Thermal State
      ↓
Target CPU
```

Eine thermisch günstigere CPU darf bevorzugt werden, wenn dadurch keine wichtigeren Anforderungen verletzt werden.

## Lastverteilung

Bei lokal hoher Temperatur darf der Scheduler Arbeit auf andere geeignete CPUs verteilen.

```text
CPU 0 → Hot
CPU 1 → Warm
CPU 2 → Cool

Task Migration
     ↓
CPU 2
```

Migration muss gegen Cache-, NUMA- und Migrationskosten abgewogen werden.

## Sustainable Performance

Für länger laufende Tasks soll der Scheduler nachhaltige Leistung berücksichtigen.

```text
Short Boost
    ↓
High Temperature
    ↓
Throttling
    ↓
Lower Sustained Performance
```

Eine dauerhaft etwas langsamere CPU-Platzierung kann effizienter sein als wiederholtes Boosting mit anschließendem Thermal Throttling.

## Thermal Pressure

NovaOS darf einen abstrahierten Thermal-Pressure-Wert bereitstellen.

Beispiel:

```text
Normal
Elevated
High
Critical
```

Dieser Zustand kann Scheduling- und Resource-Economy-Entscheidungen beeinflussen.

## Critical Thermal State

Bei kritischer thermischer Belastung gilt:

```text
Thermal Safety
      ↓
Performance
```

NovaOS darf:

```text
Reduce Load
Migrate Tasks
Reduce Parallelism
Restrict Boost
Degrade Services
Suspend Work
```

System- und Hardware-Sicherheitsgrenzen dürfen nicht zugunsten höherer Performance überschritten werden.

## Realtime

Realtime-Reservierungen müssen thermische Grenzen berücksichtigen.

Eine Realtime-Garantie darf nicht auf Leistung beruhen, die thermisch nicht dauerhaft bereitgestellt werden kann.

```text
Realtime Requirement
        +
Sustainable Capacity
        ↓
Admission Control
```

Thermische Sicherheitsgrenzen bleiben dennoch verbindlich.

## Energy Management

Thermal- und Energy-aware Scheduling arbeiten zusammen.

```text
Energy Cost
    ↕
Heat Generation
    ↕
Performance
```

Eine energieeffizientere Ausführung kann gleichzeitig thermische Belastung reduzieren.

## Adaptive Steuerung

NovaOS darf Temperaturverlauf und Workload-Verhalten zur Verbesserung zukünftiger Entscheidungen verwenden.

```text
Workload
   ↓
Thermal Response
   ↓
Observed Behaviour
   ↓
Future Placement
```

Adaptive Steuerung darf Sicherheits- und Hard Constraints nicht überschreiben.

## Normative Anforderungen

1. NovaOS MUSS thermische Plattformgrenzen respektieren.
2. Der Scheduler SOLL thermische Zustände bei CPU-Platzierungen berücksichtigen.
3. Thermal Safety MUSS Vorrang vor Performance-Optimierung haben.
4. Thermal Headroom SOLL bei länger laufenden oder rechenintensiven Tasks berücksichtigt werden.
5. NovaOS SOLL zwischen kurzfristiger Peak- und nachhaltiger Rechenleistung unterscheiden können.
6. Thermisch motivierte Migrationen SOLLEN Cache-, NUMA- und Migrationskosten berücksichtigen.
7. Kritische thermische Zustände MÜSSEN kontrollierte Gegenmaßnahmen auslösen können.
8. Realtime Admission Control MUSS nachhaltige thermische Kapazität berücksichtigen können.
9. Energie- und Thermal-Optimierung SOLLEN koordiniert werden.
10. Adaptive Thermal-Optimierung DARF Hard Constraints NICHT überschreiben.
11. Nicht verfügbare thermische Kapazität DARF NICHT als garantierte Rechenkapazität behandelt werden.
12. Thermal-Zustände und thermisch bedingte Scheduling-Entscheidungen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SCHEDULER-TOPOLOGY-0001`
- `NPSPEC-SCHEDULER-NUMA-0001`
- `NPSPEC-SCHEDULER-CACHE-0001`
- `NPSPEC-SCHEDULER-ENERGY-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-SCHED-0010`
- `ADR-REALTIME-0009_Realtime_Constraints_vor_Energieoptimierung`

## Ergebnis

```text
Task Demand
    ↓
Thermal Capacity
    ↓
Constraint-Aware Placement
    ↓
Sustainable Execution
    ↓
Thermal Safety
```

NovaOS erhält damit ein Thermal-aware Scheduling-Modell, das thermische Kapazität als reale Systemressource behandelt und Performance, Energieverbrauch und Hardware-Sicherheit kontrolliert miteinander verbindet.