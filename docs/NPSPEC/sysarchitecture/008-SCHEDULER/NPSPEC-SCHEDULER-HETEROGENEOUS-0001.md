# NPSPEC-SCHEDULER-HETEROGENEOUS-0001 – Nova Heterogeneous Scheduling

## Status

Angenommen

## Kategorie

Scheduler / Heterogeneous Computing / CPU Topology / Resource Economy

## Zweck

NovaOS definiert Heterogeneous Scheduling für Systeme mit unterschiedlich leistungsfähigen CPU-Kernen und Ausführungseinheiten.

```text
Runnable Tasks
      ↓
Execution Requirements
      ↓
Hardware Capabilities
      ↓
Scheduler
      ↓
Suitable Processing Unit
```

Ziel ist, Tasks nicht nur auf freie, sondern auf für ihre Anforderungen geeignete Recheneinheiten zu verteilen.

## Grundprinzip

Nicht alle CPUs besitzen dieselben Eigenschaften.

```text
CPU Availability ≠ CPU Suitability
```

Eine Scheduling-Entscheidung berücksichtigt daher:

```text
Task Requirements
        +
CPU Capabilities
        +
Performance
        +
Energy Cost
        +
Thermal State
        ↓
Placement
```

## Heterogene CPU-Klassen

NovaOS muss unterschiedliche CPU-Klassen modellieren können.

Beispiele:

```text
Performance Core
Efficiency Core
Low-Power Core
Specialized Core
```

Die Architektur darf nicht von konkreten Herstellerbezeichnungen wie P-Core oder E-Core abhängig sein.

## CPU-Capabilities

Eine CPU oder CPU-Gruppe kann Eigenschaften besitzen wie:

```text
Instruction Sets
Performance Capacity
Energy Efficiency
Frequency Range
Vector Extensions
Security Features
Realtime Suitability
Virtualization Features
```

Diese Eigenschaften werden über das normalisierte Hardwaremodell bereitgestellt.

## Task-Anforderungen

Tasks können Anforderungen oder Präferenzen besitzen.

Beispiele:

```text
Required ISA Features
Minimum Performance
Latency Requirement
Energy Preference
Realtime Requirement
CPU Affinity
Execution Contract
```

Fehlt eine zwingend benötigte CPU-Capability, darf der Task dort nicht ausgeführt werden.

## CPU-Auswahl

Die Auswahl erfolgt grundsätzlich:

```text
Hard Requirements
       ↓
Compatible CPUs
       ↓
Performance / Energy / Thermal
       ↓
Topology / NUMA / Cache
       ↓
Target CPU
```

Soft Preferences dürfen nur innerhalb der kompatiblen CPU-Menge optimiert werden.

## Performance-Klassen

NovaOS darf CPUs anhand ihrer verfügbaren Rechenkapazität klassifizieren.

```text
High Performance
Medium Performance
High Efficiency
```

Diese Klassen sind dynamische Eigenschaften und keine festen Architekturannahmen.

Thermal Throttling oder Power Limits können die tatsächlich verfügbare Kapazität verändern.

## Workload-Klassifikation

Der Scheduler darf Workloads anhand ihres beobachteten Verhaltens klassifizieren.

Beispiele:

```text
Latency Sensitive
Compute Intensive
Background
Interactive
Memory Bound
Energy Sensitive
```

Diese Klassifikation ist eine Optimierung.

Explizite Anforderungen aus dem Execution Contract haben Vorrang.

## Migration

Tasks dürfen zwischen unterschiedlichen CPU-Klassen migriert werden.

```text
Efficiency Core
      ↓
Demand Increases
      ↓
Performance Core
```

oder:

```text
Performance Core
      ↓
Task Becomes Background
      ↓
Efficiency Core
```

Migrationen müssen deren Kosten berücksichtigen.

## Cache und NUMA

CPU-Klasse allein bestimmt nicht die optimale Platzierung.

```text
CPU Capability
      +
Cache Locality
      +
NUMA Locality
      +
Current Load
      ↓
Placement
```

Ein leistungsfähigerer Core kann aufgrund ungünstiger Speicherlokalität die schlechtere Wahl sein.

## Energie

Nicht zeitkritische Tasks dürfen bevorzugt auf energieeffizienten CPUs ausgeführt werden.

```text
Background Task
      ↓
Efficiency-Oriented CPU
```

Zeitkritische Tasks dürfen leistungsfähigere CPUs bevorzugen.

```text
Interactive Task
      ↓
Performance-Oriented CPU
```

## Thermal State

Thermische Belastung kann die effektive Eignung einer CPU verändern.

```text
Fast CPU
  +
Thermal Throttling
  ↓
Reduced Effective Capacity
```

Der Scheduler soll daher aktuelle statt ausschließlich nominelle Leistungsfähigkeit berücksichtigen.

## Realtime

Realtime-Tasks dürfen nur auf CPUs eingeplant werden, deren Eigenschaften die benötigten Garantien ermöglichen.

```text
Realtime Requirements
        ↓
Compatible CPU Set
        ↓
Reservation
        ↓
Realtime Execution
```

Eine nominell schnelle CPU ist nicht automatisch für Hard Realtime geeignet.

## Asymmetrische Systeme

NovaOS darf keine symmetrische CPU-Leistung voraussetzen.

```text
CPU 0 Capacity: 100
CPU 1 Capacity: 100
CPU 2 Capacity: 45
CPU 3 Capacity: 45
```

Load Balancing muss unterschiedliche CPU-Kapazitäten berücksichtigen.

```text
Equal Queue Length ≠ Equal Load
```

## Hotplug

Neue CPUs oder CPU-Klassen müssen dynamisch in das Scheduling-Modell aufgenommen werden können.

```text
CPU Added / Removed
        ↓
Topology Update
        ↓
Capability Update
        ↓
Placement Recalculation
```

## Normative Anforderungen

1. NovaOS MUSS CPUs mit unterschiedlichen Leistungs- und Capability-Eigenschaften unterstützen können.
2. Der Scheduler DARF keine identische Leistungsfähigkeit aller CPUs voraussetzen.
3. Erforderliche CPU-Capabilities MÜSSEN vor einer Task-Platzierung geprüft werden.
4. Hard Requirements MÜSSEN vor Performance- oder Energieoptimierungen ausgewertet werden.
5. Unterschiedliche CPU-Kapazitäten MÜSSEN beim Load Balancing berücksichtigt werden.
6. Interaktive und latenzkritische Tasks DÜRFEN leistungsfähigere CPUs bevorzugen.
7. Hintergrund- und energieorientierte Tasks DÜRFEN effizientere CPUs bevorzugen.
8. Cache-, NUMA-, Thermal- und Energieeigenschaften SOLLEN gemeinsam mit der CPU-Klasse bewertet werden.
9. Migrationen zwischen CPU-Klassen SOLLEN nur bei ausreichendem erwarteten Nutzen erfolgen.
10. Realtime-Tasks DÜRFEN nur auf CPUs ausgeführt werden, die ihre Hard Requirements erfüllen.
11. Adaptive Workload-Klassifikation DARF explizite Execution Contracts NICHT überschreiben.
12. CPU-Capabilities und daraus resultierende Placement-Entscheidungen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SCHEDULER-TOPOLOGY-0001`
- `NPSPEC-SCHEDULER-NUMA-0001`
- `NPSPEC-SCHEDULER-CACHE-0001`
- `NPSPEC-SCHEDULER-ENERGY-0001`
- `NPSPEC-SCHEDULER-THERMAL-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-HOTPLUG-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-SCHED-0011`

## Ergebnis

```text
Task Requirements
       ↓
CPU Capability Filter
       ↓
Performance + Energy + Thermal
       ↓
Cache + NUMA + Topology
       ↓
Suitable CPU Class
       ↓
Execution
```

NovaOS erhält damit ein Heterogeneous-Scheduling-Modell, das unterschiedliche CPU-Klassen als vollwertige Scheduling-Ressourcen behandelt und Tasks anhand ihrer tatsächlichen Anforderungen auf geeignete Recheneinheiten verteilt.