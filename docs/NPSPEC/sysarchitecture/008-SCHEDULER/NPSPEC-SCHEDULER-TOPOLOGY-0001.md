# NPSPEC-SCHEDULER-TOPOLOGY-0001 – Nova Topology-Aware Scheduling

## Status

Angenommen

## Kategorie

Scheduler / Topology / SMP / NUMA / Locality

## Zweck

NovaOS definiert topology-aware Scheduling, damit Tasks unter Berücksichtigung der tatsächlichen Hardwarestruktur auf geeigneten CPUs ausgeführt werden.

```text
Task
  ↓
Scheduling Constraints
  ↓
Hardware Topology
  ↓
CPU Selection
```

Ziel ist, Cache-Lokalität, NUMA-Nähe und verfügbare Rechenressourcen zu nutzen, ohne harte Ausführungsanforderungen zu verletzen.

## Grundprinzip

Nicht jede CPU ist für jeden Task gleich geeignet.

```text
CPU Availability
+
Affinity
+
Cache Topology
+
NUMA Topology
+
Resource State
=
Placement Decision
```

Topologie ist eine Scheduling-Information, keine eigenständige Scheduling-Policy.

## Topologie-Modell

Der Scheduler verwendet die normalisierte Hardwaretopologie der HAL.

```text
System
├── NUMA Node
│   ├── CPU Package
│   │   ├── Core
│   │   │   ├── Thread
│   │   │   └── Thread
│   │   └── Shared Cache
│   └── Memory
└── NUMA Node
```

Hardwarespezifische IDs dürfen nicht direkt die Scheduling-Architektur bestimmen.

## CPU-Auswahl

Bei der Auswahl einer CPU können berücksichtigt werden:

```text
Allowed CPU Set
Current CPU
Core Topology
Shared Cache
NUMA Node
Memory Locality
CPU Load
CPU Capability
Energy State
```

Harte Constraints werden zuerst ausgewertet.

```text
Hard Constraints
      ↓
Eligible CPUs
      ↓
Locality / Load Optimization
      ↓
Target CPU
```

## Cache-Lokalität

Tasks sollen nach Möglichkeit auf CPUs ausgeführt werden, deren Caches relevante Daten oder vorherigen Task-Zustand enthalten können.

```text
Previous CPU
    ↓
Warm Cache
    ↓
Prefer Local Execution
```

Migration darf erfolgen, wenn andere Anforderungen den Lokalitätsvorteil überwiegen.

## NUMA-Lokalität

Auf NUMA-Systemen soll die Ausführung möglichst nahe am verwendeten Speicher stattfinden.

```text
Task Memory @ Node 1
        ↓
Prefer CPU @ Node 1
```

Dabei gilt:

```text
Balanced CPU Load ≠ Optimal Placement
```

Eine CPU mit geringerer Last kann aufgrund hoher Speicherzugriffskosten trotzdem ungeeignet sein.

## CPU-Affinität

Affinity begrenzt die zulässigen Ausführungsorte.

```text
AllowedCPUs = Hard Constraint
PreferredCPUs = Soft Preference
```

Topology-aware Scheduling darf harte CPU-Affinitäten nicht überschreiben.

## Task-Migration

Vor einer Migration sollen Kosten und Nutzen bewertet werden.

```text
Load Benefit
+
Deadline Pressure
-
Cache Loss
-
NUMA Cost
-
Migration Cost
```

Unnötige oder häufige Migrationen sollen vermieden werden.

## Work Stealing

Work Stealing soll zunächst topologisch nahe Quellen bevorzugen.

```text
Local CPU
   ↓
Same Core Group
   ↓
Same NUMA Node
   ↓
Remote NUMA Node
```

Diese Reihenfolge darf durch harte Constraints oder dringende zeitliche Anforderungen verändert werden.

## Realtime

Realtime-Tasks können feste CPU- oder Topologieanforderungen besitzen.

```text
Realtime Task
      ↓
Reserved CPU Set
      ↓
Controlled Placement
```

Topology-Optimierung darf garantierte Realtime-Anforderungen nicht verletzen.

## Topologieänderungen

CPU- oder Memory-Hotplug muss eine Neubewertung betroffener Scheduling-Zustände ermöglichen.

```text
Topology Change
      ↓
Update Model
      ↓
Reevaluate Placement
```

Nicht mehr verfügbare CPUs dürfen nicht als gültige Ziele verwendet werden.

## Normative Anforderungen

1. NovaOS MUSS Hardwaretopologie bei Scheduling-Entscheidungen berücksichtigen können.
2. Der Scheduler MUSS die normalisierte HAL-Topologie verwenden.
3. Harte CPU-Affinitäten MÜSSEN eingehalten werden.
4. Cache-Lokalität SOLL bei CPU-Auswahl berücksichtigt werden.
5. NUMA-Lokalität SOLL bei Task- und Speicherplatzierung berücksichtigt werden.
6. Topologieoptimierung DARF Hard Constraints NICHT überschreiben.
7. Task-Migration SOLL nur bei ausreichendem erwarteten Nutzen erfolgen.
8. Work Stealing SOLL topologisch nahe CPUs bevorzugen.
9. Realtime-Reservierungen MÜSSEN Vorrang vor weichen Lokalitätsoptimierungen haben.
10. Nicht verfügbare CPUs DÜRFEN NICHT als Scheduling-Ziele verwendet werden.
11. Topologieänderungen MÜSSEN eine Neubewertung betroffener Placements ermöglichen.
12. Placement-Entscheidungen SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SCHEDULER-FAIR-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `NPSPEC-CONCURRENCY-WORKSTEALING-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-NUMA-0001`
- `NPSPEC-HAL-HOTPLUG-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-SCHED-0006`

## Ergebnis

```text
Runnable Task
      ↓
Hard Constraints
      ↓
Topology + Locality + Load
      ↓
Target CPU
      ↓
Locality-Aware Execution
```

NovaOS erhält damit ein topology-aware Scheduling-Modell, das CPU-, Cache- und NUMA-Strukturen für effiziente Task-Platzierung nutzt, ohne Affinitäts-, Realtime- oder andere harte Anforderungen zu verletzen.