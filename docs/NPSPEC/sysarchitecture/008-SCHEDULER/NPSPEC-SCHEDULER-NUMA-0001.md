# NPSPEC-SCHEDULER-NUMA-0001 – Nova NUMA-Aware Scheduling

## Status

Angenommen

## Kategorie

Scheduler / NUMA / Locality / Memory / Multiprocessing

## Zweck

NovaOS definiert NUMA-aware Scheduling, damit Tasks möglichst auf CPUs ausgeführt werden, die einen günstigen Zugriff auf den von ihnen verwendeten Speicher und ihre Geräte besitzen.

```text
Task
  ↓
Memory / Device Locality
  ↓
NUMA Evaluation
  ↓
Target Node
  ↓
Target CPU
```

## Grundprinzip

Auf NUMA-Systemen besitzen CPUs unterschiedliche Zugriffskosten auf Speicher und Geräte.

```text
CPU @ Node 0
    ↓
Local Memory  → Low Cost
Remote Memory → Higher Cost
```

Scheduling und Speicherlokalität sollen deshalb gemeinsam betrachtet werden.

```text
CPU Placement + Memory Placement
```

## NUMA-Modell

Der Scheduler verwendet das normalisierte NUMA-Modell der HAL.

Mindestens relevant sind:

```text
NUMA Node
CPUs
Memory Regions
Distance / Access Cost
Device Locality
Node State
```

Die konkrete Hardwaredarstellung bleibt hinter der HAL verborgen.

## Task-Lokalität

Für einen Task kann eine bevorzugte NUMA-Lokalität bestimmt werden.

Berücksichtigt werden können:

```text
Current Memory Placement
Working Set
Previous Execution
Device Access
Shared Memory
Communication Partners
CPU Load
```

Die Lokalität darf sich während der Laufzeit ändern.

## CPU-Auswahl

Die CPU-Auswahl erfolgt grundsätzlich:

```text
Hard Constraints
      ↓
Eligible CPUs
      ↓
NUMA Locality
      ↓
CPU Load
      ↓
Target CPU
```

Eine entfernte CPU darf gewählt werden, wenn lokale CPUs ungeeignet oder überlastet sind und keine Hard Constraints verletzt werden.

## Speicherplatzierung

Scheduler und Memory Manager sollen Informationen austauschen können.

```text
Task Placement
      ↕
Memory Placement
```

Mögliche Strategien sind:

```text
Move Task to Memory
Move Memory to Task
Keep Existing Placement
Replicate Read-Mostly Data
```

Die Entscheidung muss Kosten und Nutzen berücksichtigen.

## Migration

Task-Migration zwischen NUMA Nodes kann teuer sein.

```text
Migration Benefit
        -
Cache Loss
Memory Distance
State Migration
Device Distance
```

Häufige Migrationen zwischen Nodes sollen vermieden werden.

## Memory Migration

NovaOS darf häufig verwendete Speicherseiten näher an den ausführenden Task verschieben.

```text
Remote Memory Access
        ↓
Observed Locality
        ↓
Page Migration
        ↓
Local Memory
```

Memory Migration ist eine Optimierung und darf harte Speicher- oder Echtzeitanforderungen nicht verletzen.

## Shared Memory

Bei gemeinsam genutztem Speicher kann keine einzelne optimale NUMA-Platzierung existieren.

```text
Task A @ Node 0
Task B @ Node 1
       ↓
Shared Memory
```

NovaOS darf hierfür Interleaving, Replikation oder andere geeignete Placement-Strategien verwenden.

## Device Locality

NUMA-Zugehörigkeit von Geräten soll berücksichtigt werden.

```text
Task
  ↓
Device
  ↓
Local NUMA Node
```

Insbesondere DMA-intensive Workloads können von CPU-, Speicher- und Device-Lokalität profitieren.

## Realtime

Realtime-Tasks können feste NUMA-Anforderungen besitzen.

```text
Realtime Task
      ↓
CPU Reservation
+
Memory Reservation
+
NUMA Constraint
```

Automatische NUMA-Optimierung darf garantierte Realtime-Eigenschaften nicht verändern.

## Hotplug

Wird ein NUMA Node, CPU oder Speicherbereich entfernt:

```text
Topology Change
      ↓
Reevaluate Placement
      ↓
Migrate / Rebind / Degrade
```

Nicht mehr verfügbare Ressourcen dürfen nicht weiter als gültige Placement-Ziele gelten.

## UMA-Systeme

Auf Systemen ohne relevante NUMA-Unterschiede wird die Architektur als vereinfachtes Modell behandelt.

```text
UMA
 ↓
NUMA Node 0
```

Dadurch bleibt die Scheduler-Schnittstelle einheitlich.

## Normative Anforderungen

1. NovaOS MUSS NUMA-Topologie bei geeigneten Scheduling-Entscheidungen berücksichtigen können.
2. Der Scheduler MUSS das normalisierte NUMA-Modell der HAL verwenden.
3. Harte CPU- und NUMA-Constraints MÜSSEN eingehalten werden.
4. Speicherlokalität SOLL bei der CPU-Auswahl berücksichtigt werden.
5. Device-Lokalität SOLL bei I/O-intensiven Tasks berücksichtigt werden.
6. Scheduler und Memory Manager SOLLEN Placement-Informationen austauschen können.
7. Task- und Memory-Migration DÜRFEN nur kontrolliert erfolgen.
8. Unnötige Migrationen zwischen NUMA Nodes SOLLEN vermieden werden.
9. Adaptive NUMA-Optimierung DARF Hard Constraints NICHT überschreiben.
10. Realtime-Reservierungen MÜSSEN Vorrang vor weichen NUMA-Optimierungen haben.
11. Topologieänderungen MÜSSEN eine Neubewertung betroffener Placements ermöglichen.
12. NUMA-Placement und Migrationen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SCHEDULER-TOPOLOGY-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-NUMA-0001`
- `NPSPEC-HAL-HOTPLUG-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-SCHED-0007`

## Ergebnis

```text
Task + Working Set + Devices
           ↓
NUMA Locality Evaluation
           ↓
CPU / Memory Placement
           ↓
Controlled Migration
           ↓
Locality-Aware Execution
```

NovaOS erhält damit ein NUMA-aware Scheduling-Modell, das CPU-, Speicher- und Gerätelokalität gemeinsam berücksichtigt und Remote-Zugriffe reduziert, ohne harte Ausführungsanforderungen zu verletzen.