# ADR-SCHED-0005 – Cache-Aware Scheduling

## Status

Angenommen

## Kategorie

Kernel / Scheduler / Cache / Locality / CPU Placement / Performance

## Kontext

`ADR-SCHED-0003_Topology_Aware_Scheduling` und `ADR-SCHED-0004_NUMA_Aware_Scheduling` berücksichtigen bereits die physische Struktur von CPUs und Speicher.

Innerhalb dieser Topologie besitzen moderne Prozessoren jedoch komplexe Cache-Hierarchien:

```text
CPU Package
    │
    ├── Core 0
    │    ├── L1
    │    └── L2
    │
    ├── Core 1
    │    ├── L1
    │    └── L2
    │
    └── Shared L3
```

Task-Migration kann vorhandene Cache-Lokalität zerstören. Umgekehrt können eng zusammenarbeitende Tasks davon profitieren, auf CPUs mit gemeinsamem Cache ausgeführt zu werden.

Cache-Lokalität darf deshalb nicht nur als Nebeneffekt der CPU-Affinity betrachtet werden.

---

## Entscheidung

NovaOS verwendet **Cache-Aware Scheduling** als Bestandteil des Topology-Aware Scheduling.

Der Scheduler berücksichtigt Cache-Hierarchie und Cache-Sharing-Beziehungen bei CPU-Placement und Task-Migration:

```text
Runnable Task
      ↓
Eligible CPUs
      ↓
Cache Topology
      +
Execution History
      +
Workload Relations
      ↓
Placement Policy
      ↓
Target CPU
```

Bestehende nützliche Cache-Lokalität soll erhalten werden, sofern der Nutzen einer Migration diese Locality-Kosten nicht überwiegt.

---

## Grundprinzip

```text
Preserve warm caches when useful.

Place communicating work near shared caches.

Do not sacrifice hard constraints for cache locality.
```

---

## Cache-Topologie

Der Scheduler verwendet die Cache-Informationen des Hardware System Graph.

Dazu gehören insbesondere:

- Cache-Level,
- zugeordnete CPUs,
- Shared-Cache-Domains,
- Cache-Größe,
- bekannte relevante Eigenschaften.

Der Scheduler führt kein unabhängiges konkurrierendes Cache-Topologiemodell.

---

## Cache Domains

Aus gemeinsam genutzten Caches können Scheduling Domains abgeleitet werden.

Beispiel:

```text
L3 Cache Domain
   ├── CPU 0
   ├── CPU 1
   ├── CPU 2
   └── CPU 3
```

Solche Domains helfen bei der Bewertung möglicher CPU-Ziele.

---

## Warm-Cache-Affinität

Wurde ein Task kürzlich auf einer CPU ausgeführt, kann diese CPU beziehungsweise ihre Cache Domain bevorzugt werden.

```text
Task
 ↓
Previous CPU
 ↓
Warm Cache
 ↓
Preferred Placement
```

Diese Affinität ist grundsätzlich eine Soft Policy.

---

## Task Migration

Eine Migration auf eine andere CPU kann Cache-Zustand verlieren.

Der Scheduler behandelt diesen Verlust als Teil der Migration Cost.

Eine kurzfristig weniger ausgelastete CPU ist deshalb nicht automatisch das bessere Ziel.

---

## Shared Cache

Tasks mit gemeinsam verwendeten Daten können davon profitieren, innerhalb derselben Shared-Cache-Domain ausgeführt zu werden.

Dies gilt insbesondere für:

- Task Groups,
- Producer/Consumer-Workloads,
- parallele Algorithmen,
- gemeinsam genutzte Read-Mostly-Daten.

Der Scheduler muss solche Beziehungen berücksichtigen können, ohne eine feste Co-Location zu erzwingen.

---

## Cache Contention

Shared Cache kann zugleich zu Konkurrenz führen.

Mehrere speicherintensive Workloads innerhalb derselben Cache Domain können sich gegenseitig verdrängen.

Cache-Aware Scheduling berücksichtigt daher nicht nur:

```text
cache sharing benefit
```

sondern auch:

```text
cache contention cost
```

Die konkrete Bewertung bleibt Scheduling Policy.

---

## SMT

SMT-Siblings teilen typischerweise weitere CPU-Ressourcen und teilweise Cache-Strukturen.

Cache-Aware Scheduling muss deshalb mit SMT-Awareness zusammenarbeiten.

Für bestimmte Workloads kann ein anderer physischer Core geeigneter sein als ein freier SMT-Sibling.

---

## NUMA

Cache- und NUMA-Locality werden gemeinsam betrachtet.

Eine optimale Cache-Entscheidung darf nicht blind einen wesentlich schlechteren NUMA-Zugriff erzeugen.

Konzeptionell:

```text
CPU Load
   +
Cache Locality
   +
NUMA Locality
   +
Migration Cost
      ↓
Placement Decision
```

---

## Task Groups

Task Groups können Cache-Locality-Hinweise bereitstellen.

Datenparallelisierte Tasks können beispielsweise voneinander getrennt werden, während eng kommunizierende Tasks innerhalb derselben Shared-Cache-Domain bevorzugt werden.

Diese Hinweise bleiben den Execution Contracts untergeordnet.

---

## Work Stealing

Work Stealing soll Cache Domains berücksichtigen.

Ein Worker soll bevorzugt innerhalb einer geeigneten nahen Cache Domain stehlen, bevor weiter entfernte CPUs betrachtet werden.

```text
Local Queue
    ↓
Shared Cache Domain
    ↓
NUMA Node
    ↓
Remote Domain
```

Dies ist eine Präferenz, kein universelles starres Verfahren.

---

## Interactive Workloads

Bei interaktiven Workloads kann das Erhalten warmer Caches die Reaktionslatenz reduzieren.

Der Scheduler darf deshalb kürzlich ausgeführte interaktive Tasks bevorzugt auf geeigneten bisherigen CPUs fortsetzen.

Fairness und andere Hard Constraints bleiben bindend.

---

## Realtime

Realtime Domains können Cache-Topologie als Teil ihrer Placement Constraints berücksichtigen.

Bei Hard Realtime dürfen jedoch nur ausreichend vorhersehbare Cache-Eigenschaften für Garantien verwendet werden.

Cache-Heuristiken allein stellen keine Realtime-Garantie dar.

---

## Heterogene CPUs

Bei heterogenen CPUs können sich Cache-Größe und Cache-Hierarchie zwischen CPU-Klassen unterscheiden.

Der Scheduler darf deshalb nicht voraussetzen, dass ein Task beim Wechsel zwischen CPU-Klassen dieselben Cache-Eigenschaften vorfindet.

---

## Cache-sensitive Workloads

Execution Contracts beziehungsweise Scheduling Hints dürfen ausdrücken, dass ein Workload besonders cache-sensitiv ist.

Beispielsweise:

```text
CacheSensitivity:
    Low
    Normal
    High
```

Dies bleibt ein Optimierungshinweis und erzeugt keine zusätzliche Authority.

---

## Cache-Hinweise

Anwendungen dürfen keine direkten Scheduler-Befehle wie:

```text
keep my data in L3
```

erzwingen.

Sie können semantische Anforderungen beziehungsweise Hints bereitstellen.

Die tatsächliche Placement-Entscheidung bleibt Aufgabe des Systems.

---

## Resource Economy

Cache-Ressourcen sind grundsätzlich gemeinsam genutzte Hardware.

NovaOS muss Cache Contention beobachten und bei Scheduling-Entscheidungen berücksichtigen können.

Eine vollständige feste Partitionierung von Caches wird durch diese ADR nicht vorgeschrieben.

Hardwaremechanismen für Cache Partitioning können später als zusätzliche Resource-Control-Mechanismen integriert werden.

---

## Deterministic Mode

Adaptive Cache-Heuristiken müssen für entsprechend deterministische Workloads einschränkbar sein.

Ein deterministischer Contract kann beispielsweise feste:

- CPU Sets,
- Placement-Regeln,
- Migration Points

verlangen.

---

## Hotplug

CPU Hotplug kann Cache Domains verändern.

Der Scheduler muss Änderungen über neue Generationen des Hardware System Graph übernehmen können.

Tasks dürfen nicht dauerhaft auf veralteten Cache-Topologieannahmen geplant werden.

---

## Introspection

Cache-Aware Scheduling muss ausreichend introspektierbar sein.

Beispielsweise:

```text
Task:
    compiler.worker.7

Current CPU:
    6

Previous CPU:
    4

Shared Cache Domain:
    L3-1

Migration:
    avoided

Reason:
    cache locality
```

Aggregierte Cache-Miss- und Migration-Daten können für Performanceanalyse verwendet werden, sofern die Hardware entsprechende Informationen bereitstellt.

---

## Adaptive Optimierung

NovaOS darf messen, ob Cache-Affinitätsentscheidungen tatsächlich vorteilhaft sind.

Beispielsweise:

```text
Predicted cache benefit
        ↓
Placement
        ↓
Observed performance
        ↓
Prediction Error
        ↓
Policy adjustment
```

Dies darf nur Soft Scheduling Policies beeinflussen.

---

## Fail-Safe Defaults

Fehlen zuverlässige Cache-Informationen, arbeitet der Scheduler mit den verbleibenden Topologie- und Scheduling-Informationen weiter.

Unbekannte Cache-Lokalität darf nicht als Hard Constraint interpretiert werden.

Safety-, Security-, Realtime-, Affinity-, Resource- und NUMA-Constraints besitzen Vorrang.

---

## Normative Anforderungen

1. NovaOS MUSS Cache-Aware Scheduling unterstützen können.
2. Cache-Aware Scheduling MUSS Bestandteil des Topology-Aware Scheduling sein.
3. Der Scheduler MUSS Cache-Topologie aus dem Hardware System Graph beziehen.
4. Shared-Cache-Domains MÜSSEN als Scheduling-Topologie darstellbar sein.
5. Bestehende Warm-Cache-Lokalität SOLL bei CPU Placement berücksichtigt werden.
6. Cache-Verlust MUSS als möglicher Task-Migrationskostenfaktor berücksichtigt werden können.
7. Cache-Lokalität DARF Hard Scheduling Constraints NICHT überschreiben.
8. Gemeinsam arbeitende Tasks SOLLEN Shared-Cache-Locality nutzen können.
9. Cache Contention MUSS als Gegenfaktor zu Cache Sharing berücksichtigt werden können.
10. Cache-Aware Scheduling MUSS mit SMT-Awareness integrierbar sein.
11. Cache- und NUMA-Locality MÜSSEN gemeinsam bewertet werden können.
12. Task Groups MÜSSEN Cache-Locality-Hints bereitstellen können.
13. Work Stealing MUSS Cache Domains berücksichtigen können.
14. Lokales Stealing innerhalb geeigneter Cache Domains SOLL bevorzugt werden.
15. Interaktive Tasks DÜRFEN von Warm-Cache-Affinität profitieren.
16. Realtime Constraints MÜSSEN Vorrang vor Soft Cache-Heuristiken besitzen.
17. Heterogene Cache-Hierarchien MÜSSEN unterstützt werden können.
18. Workloads MÜSSEN ihre Cache-Sensitivität als Soft Hint ausdrücken können.
19. Anwendungen DÜRFEN Cache Placement NICHT unter Umgehung der Scheduler Policy erzwingen.
20. Cache Contention MUSS beobachtbar sein können, sofern Hardwareinformationen verfügbar sind.
21. Cache-Aware Scheduling MUSS mit der Resource Economy integrierbar sein.
22. Hardwaregestütztes Cache Partitioning MUSS später integrierbar bleiben.
23. Deterministic Mode MUSS adaptive Cache-Heuristiken einschränken können.
24. CPU Hotplug MUSS Cache-Topologieänderungen berücksichtigen.
25. Unnötige Task-Migrationen SOLLEN zur Erhaltung nützlicher Cache-Lokalität vermieden werden.
26. Cache Locality DARF notwendiges Load Balancing NICHT grundsätzlich verhindern.
27. Scheduler Policies MÜSSEN zwischen Cache Sharing und Cache Contention abwägen können.
28. Cache-bezogene Placement-Entscheidungen MÜSSEN introspektierbar sein.
29. Fehlende Cache-Informationen MÜSSEN einen sicheren Fallback ermöglichen.
30. Adaptive Cache-Optimierung DARF Hard Execution Constraints NICHT verändern.

---

## Konsequenzen

### Positive Konsequenzen

- weniger unnötige Cache-Verluste,
- geringere Migrationskosten,
- bessere Nutzung gemeinsam genutzter Caches,
- bessere Performance paralleler Workloads,
- bessere Integration von Work Stealing mit realer Hardwaretopologie,
- weniger unnötige Speicherzugriffe.

### Negative Konsequenzen

- CPU Placement benötigt zusätzliche Kostenfaktoren,
- Cache Sharing kann sowohl Vorteil als auch Contention erzeugen,
- Hardwareinformationen unterscheiden sich zwischen Plattformen,
- zu starke Cache-Affinität kann Load Balancing verschlechtern.

---

## Verworfene Alternativen

### Cache-Topologie vollständig ignorieren

Verworfen.

Task-Migration kann dadurch unnötig wertvollen Cache-Zustand zerstören.

### Tasks immer auf ihrer letzten CPU halten

Verworfen.

Dies kann Fairness, Load Balancing und andere Scheduling-Anforderungen beeinträchtigen.

### Tasks mit gemeinsamen Daten immer auf denselben Cache beschränken

Verworfen.

Cache Contention kann den Locality-Vorteil übersteigen.

### Cache Locality über NUMA Locality priorisieren

Verworfen.

Ein kleiner Cache-Vorteil kann durch hohe Remote-Memory-Kosten zunichtegemacht werden.

### Anwendungen direkt über Cache Placement entscheiden lassen

Verworfen.

Cache-Nutzung ist eine gemeinsam genutzte Systemressource und muss durch Scheduler-Policy kontrolliert werden.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-SCHED-0001_Präemptiver_Mehrklassen_Scheduler`
- `ADR-SCHED-0002_Fair_Deadline_und_Realtime_Scheduling_in_einer_Architektur`
- `ADR-SCHED-0003_Topology_Aware_Scheduling`
- `ADR-SCHED-0004_NUMA_Aware_Scheduling`
- `ADR-CONCURRENCY-0006_Work_Stealing_für_allgemeine_Task_Workloads`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`

---

## Zugehörige NPSPECs

- `NPSPEC-SCHED-CACHE-0001`
- `NPSPEC-SCHED-CACHE-TOPOLOGY-0001`
- `NPSPEC-SCHED-CACHE-AFFINITY-0001`
- `NPSPEC-SCHED-CACHE-CONTENTION-0001`
- `NPSPEC-SCHED-CACHE-MIGRATION-0001`
- `NPSPEC-SCHED-CACHE-WORKSTEALING-0001`
- `NPSPEC-SCHED-CACHE-INTROSPECTION-0001`
- `NPSPEC-SCHED-CACHE-TEST-0001`

---

## Ergebnis

NovaOS berücksichtigt CPU-Caches als Teil der Scheduling-Topologie:

```text
Task
  +
Cache Topology
  +
Execution History
  +
NUMA Locality
  +
CPU Load
      ↓
Cache-Aware Placement
```

Cache-Lokalität ist dabei ein wichtiges Optimierungsziel, aber keine isolierte oder übergeordnete Scheduling-Regel.

Die zentrale Architekturregel lautet:

```text
Preserve useful cache locality.

Share caches when beneficial.

Avoid contention when harmful.

Migrate only when the benefit
outweighs the locality cost.
```