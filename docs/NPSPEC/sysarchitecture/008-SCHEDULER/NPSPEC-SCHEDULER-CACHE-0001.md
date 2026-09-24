# NPSPEC-SCHEDULER-CACHE-0001 – Nova Cache-Aware Scheduling

## Status

Entwurf

## Kategorie

Scheduler / Cache Locality / CPU Topology / Performance

## Zweck

NovaOS definiert Cache-aware Scheduling, damit Tasks möglichst auf CPUs ausgeführt werden, auf denen ihre Daten und ihr Ausführungszustand bereits im Cache vorhanden sind.

```text
Task
  ↓
Cache Locality
  ↓
CPU Selection
  ↓
Reduced Cache Misses
```

Ziel ist, unnötige Cache-Verluste und Speicherzugriffe durch ungeeignete Task-Migrationen zu reduzieren.

## Grundprinzip

Eine freie CPU ist nicht automatisch die günstigste CPU.

```text
CPU Availability
+
Cache Locality
+
Migration Cost
+
System Load
=
Placement Decision
```

Dabei gilt:

```text
Idle CPU ≠ Best CPU
```

## Cache-Topologie

Der Scheduler verwendet die von der HAL bereitgestellte CPU- und Cache-Topologie.

Relevant können sein:

```text
L1 Cache
L2 Cache
L3 / Last-Level Cache
Shared Cache Groups
Core
Hardware Thread
CPU Package
NUMA Node
```

Die konkrete Cache-Architektur bleibt hardwareabhängig.

## Task Cache Affinity

Der Scheduler darf für Tasks eine temporäre Cache-Affinität berücksichtigen.

```text
Task
  ↓
Last CPU
  ↓
Shared Cache Domain
  ↓
Preferred CPUs
```

Diese Affinität beschreibt eine Optimierung und ist standardmäßig kein Hard Constraint.

## CPU-Auswahl

Bei mehreren zulässigen CPUs soll die CPU mit günstiger Cache-Lokalität bevorzugt werden.

```text
Hard Constraints
      ↓
Eligible CPUs
      ↓
Cache Locality
      ↓
Load / NUMA / Energy
      ↓
Target CPU
```

Andere Anforderungen können den Cache-Vorteil überwiegen.

## Task-Migration

Jede Migration kann Cache-Zustand verlieren.

```text
CPU 0
Task + Warm Cache
      ↓
Migration
      ↓
CPU 3
Cold Cache
```

Vor einer Migration sollen daher berücksichtigt werden:

```text
Expected Load Benefit
Cache Warmth
Working Set
Migration Frequency
NUMA Distance
Deadline Pressure
```

Kurzfristige Lastunterschiede sollen nicht automatisch Migrationen auslösen.

## Shared Caches

Wenn mehrere CPUs einen Cache teilen, soll der Scheduler bevorzugt innerhalb derselben Cache-Domain migrieren.

```text
L3 Cache
├── CPU 0
├── CPU 1
├── CPU 2
└── CPU 3
```

Eine Migration innerhalb dieser Gruppe kann günstiger sein als eine Migration zu einer CPU mit anderem Last-Level Cache.

## Cache Contention

Cache-Lokalität allein reicht nicht aus.

Mehrere speicherintensive Tasks können denselben Cache überlasten.

```text
Task A ─┐
Task B ─┼→ Shared Cache → Contention
Task C ─┘
```

NovaOS darf bekannte Cache-intensive Workloads auf verschiedene Cache-Domains verteilen.

## Work Stealing

Work Stealing soll Cache-Topologie berücksichtigen.

Bevorzugt wird:

```text
Same Cache Domain
       ↓
Nearby CPU Domain
       ↓
Remote Domain
```

Ein Task darf dennoch weiter entfernt gestohlen werden, wenn der erwartete Nutzen größer als der Lokalitätsverlust ist.

## NUMA

Cache- und NUMA-Lokalität müssen gemeinsam betrachtet werden.

```text
Cache Locality
      +
Memory Locality
      ↓
Placement
```

Eine cache-nahe CPU darf ungeeignet sein, wenn sie hohe Remote-Memory-Kosten verursacht.

## Realtime

Realtime-Tasks können feste CPU-Platzierungen verwenden, um Cache-Verhalten vorhersehbarer zu machen.

```text
Realtime Task
      ↓
Controlled CPU Set
      ↓
Stable Cache Behaviour
```

Cache-Optimierungen dürfen garantierte zeitliche Anforderungen nicht verletzen.

## Adaptive Optimierung

NovaOS darf Laufzeitinformationen zur Verbesserung der Cache-Platzierung verwenden.

Beispiele:

```text
Migration Frequency
Cache Miss Behaviour
Working Set Behaviour
CPU Residency
Execution History
```

Adaptive Optimierung darf Hard Constraints nicht überschreiben.

## Normative Anforderungen

1. NovaOS SOLL Cache-Topologie bei Scheduling-Entscheidungen berücksichtigen.
2. Der Scheduler MUSS Cache-Informationen aus dem normalisierten Hardwaremodell verwenden.
3. Vorherige CPU-Platzierung SOLL als Cache-Lokalitätshinweis verwendet werden können.
4. Migrationen mit hohem erwarteten Cache-Verlust SOLLEN vermieden werden.
5. Shared-Cache-Domains SOLLEN bei Task-Migration berücksichtigt werden.
6. Work Stealing SOLL cache-nahe Quellen bevorzugen.
7. Cache Contention DARF bei Placement-Entscheidungen berücksichtigt werden.
8. Cache- und NUMA-Lokalität SOLLEN gemeinsam bewertet werden.
9. Cache-Optimierungen DÜRFEN harte CPU-Affinitäten NICHT verletzen.
10. Deadline- und Realtime-Anforderungen MÜSSEN Vorrang vor weicher Cache-Lokalität haben.
11. Adaptive Cache-Optimierung DARF Hard Constraints NICHT überschreiben.
12. Cache-bezogene Placement- und Migrationsentscheidungen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SCHEDULER-TOPOLOGY-0001`
- `NPSPEC-SCHEDULER-NUMA-0001`
- `NPSPEC-CONCURRENCY-WORKSTEALING-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-NUMA-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-SCHED-0008`

## Ergebnis

```text
Runnable Task
      ↓
CPU Candidates
      ↓
Cache + NUMA + Load Evaluation
      ↓
Locality-Aware Placement
      ↓
Reduced Migration and Cache Cost
```

NovaOS erhält damit ein Cache-aware Scheduling-Modell, das vorhandene Cache-Lokalität möglichst erhält, unnötige Migrationen reduziert und Cache-, NUMA- und Lastinformationen gemeinsam für die CPU-Platzierung nutzt.