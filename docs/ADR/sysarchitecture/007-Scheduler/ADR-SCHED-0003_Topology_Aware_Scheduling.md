# ADR-SCHED-0003 – Topology-Aware Scheduling

## Status

Angenommen

## Kategorie

Kernel / Scheduler / Hardwaretopologie / NUMA / Locality / CPU Placement

## Kontext

Moderne Systeme bestehen nicht aus einer Menge gleichwertiger CPUs.

Die tatsächliche Hardwaretopologie kann unter anderem enthalten:

- mehrere CPU Packages,
- physische Cores,
- SMT Threads,
- gemeinsam genutzte Caches,
- NUMA Nodes,
- heterogene Performance- und Efficiency-Cores,
- unterschiedliche Memory Domains,
- lokale oder entfernte Accelerators.

Eine Scheduling-Entscheidung, die ausschließlich CPU-Auslastung betrachtet, kann deshalb erhebliche zusätzliche Kosten verursachen.

Beispiele sind:

- Remote-NUMA-Zugriffe,
- Verlust warmer CPU-Caches,
- Konkurrenz zwischen SMT-Siblings,
- unnötige Task-Migration,
- schlechte Nähe zwischen Daten, CPU und Geräten.

NovaOS besitzt mit dem Hardware System Graph bereits eine einheitliche Beschreibung dieser Beziehungen.

Der Scheduler soll diese Topologie systematisch nutzen.

---

## Entscheidung

NovaOS verwendet **Topology-Aware Scheduling** als grundlegende Eigenschaft des CPU-Schedulers.

```text
Runnable Task
      ↓
Hard Scheduling Constraints
      ↓
Eligible CPUs
      ↓
Hardware System Graph
      ↓
Locality / Topology Cost
      ↓
Scheduling Policy
      ↓
Target CPU
```

Die Hardwaretopologie wird dabei nicht als statische CPU-Liste, sondern als Systemgraph betrachtet.

Topology Awareness ist grundsätzlich ein Optimierungsmechanismus. Hard Constraints besitzen weiterhin Vorrang.

---

## Grundprinzip

```text
Schedule near the workload's resources.

Preserve useful locality.

Move work only when the benefit
outweighs the topology cost.
```

---

## Hardware System Graph

Der Scheduler verwendet die durch `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph` bereitgestellte Topologie.

Relevant sind insbesondere:

```text
System
 └── Package
      └── Core
           └── Hardware Thread

Cache Domains

NUMA Nodes

Memory Domains

Device / Accelerator Topology
```

Der Scheduler soll keine eigene konkurrierende Hardwaretopologie verwalten.

---

## Scheduling Domains

Aus dem Hardware System Graph können Scheduling Domains abgeleitet werden.

Beispielsweise:

```text
SMT Domain
Cache Domain
Core Domain
NUMA Domain
Package Domain
System Domain
```

Diese Domains dienen der effizienten CPU-Auswahl und dem Load Balancing.

Sie sind keine Security- oder Execution Domains.

---

## CPU Eligibility

Vor jeder Topologieoptimierung wird zunächst bestimmt, welche CPUs überhaupt zulässig sind.

Dabei können insbesondere berücksichtigt werden:

- Required Affinity,
- Realtime Reservations,
- CPU Isolation,
- Execution Contracts,
- CPU Availability,
- Security Constraints.

Erst innerhalb dieser zulässigen Menge darf Topology-Aware Scheduling optimieren.

---

## Locality

NovaOS behandelt Locality als Scheduling-Ziel.

Bevorzugt werden kann beispielsweise:

```text
Current CPU
      ↓
Same Core / Cache Domain
      ↓
Same NUMA Node
      ↓
Same Package
      ↓
Remote Domain
```

Diese Reihenfolge ist keine universelle feste Regel, sondern eine Policy auf Basis der tatsächlichen Plattformtopologie.

---

## Cache Locality

Ein Task soll nach Möglichkeit auf einer CPU weiterlaufen, auf der seine relevanten Daten noch cachelokal sind.

Unnötige Migrationen sollen vermieden werden.

Cache Locality darf jedoch Fairness, Realtime oder andere Hard Constraints nicht außer Kraft setzen.

---

## NUMA

NUMA besitzt besondere Bedeutung für Scheduling.

Der Scheduler soll berücksichtigen:

```text
Task Location
Memory Location
CPU Location
NUMA Distance
```

CPU Placement und Memory Placement sollen langfristig gemeinsam optimiert werden können.

Ein Task soll nicht allein deshalb auf einen entfernten NUMA Node verschoben werden, weil dort kurzfristig eine CPU frei ist.

---

## SMT

SMT Threads teilen Teile eines physischen Cores.

Der Scheduler muss diese Beziehung kennen.

Abhängig vom Workload kann es sinnvoll sein:

- SMT-Siblings gemeinsam zu nutzen,
- zunächst freie physische Cores zu bevorzugen,
- SMT für bestimmte Realtime- oder Isolation-Workloads einzuschränken.

Die konkrete Entscheidung bleibt Policy.

---

## Heterogene CPUs

NovaOS darf nicht voraussetzen, dass alle CPUs dieselben Eigenschaften besitzen.

Der Hardware System Graph kann unterschiedliche CPU-Klassen beschreiben.

Der Scheduler kann dadurch beispielsweise berücksichtigen:

- Performance,
- Energieeffizienz,
- unterstützte ISA-Erweiterungen,
- Realtime-Eignung.

Eine Task darf nur auf CPUs ausgeführt werden, die ihre Required Execution Constraints erfüllen.

---

## Device Locality

Workloads können eine starke Beziehung zu Geräten besitzen.

Beispiele:

```text
Network Queue
      ↓
Interrupt CPU
      ↓
Processing Task

NVMe Queue
      ↓
NUMA Node
      ↓
Worker

GPU
      ↓
Local Memory Domain
      ↓
Submission Task
```

Der Scheduler soll solche Beziehungen berücksichtigen können.

---

## Interrupt Affinity

CPU Scheduling und Interrupt Affinity sollen koordiniert werden.

Für datenintensive Geräte kann es sinnvoll sein, Interrupt-Verarbeitung und zugehörige Tasks innerhalb derselben geeigneten Locality Domain zu halten.

Eine starre Kopplung zwischen Interrupt CPU und Task CPU wird jedoch nicht vorgeschrieben.

---

## Work Stealing

Work Stealing muss topology-aware erfolgen.

Ein Worker soll bevorzugt Arbeit aus nahegelegenen Scheduling Domains übernehmen.

Konzeptionell:

```text
Local Queue
    ↓
Same Cache Domain
    ↓
Same NUMA Node
    ↓
Remote NUMA Node
```

Remote Stealing ist zulässig, wenn sein erwarteter Nutzen die Locality-Kosten rechtfertigt.

---

## Task Migration

Task Migration verursacht Kosten.

Dazu gehören:

- Cache-Verlust,
- NUMA-Effekte,
- Migration von Scheduler State,
- mögliche Memory Migration.

Der Scheduler soll deshalb unnötiges Hin- und Herwandern vermeiden.

---

## Migration Hysteresis

Kleine kurzfristige Lastunterschiede sollen nicht automatisch Task Migration auslösen.

Topology-Aware Scheduling soll Hysterese beziehungsweise geeignete Migration Thresholds unterstützen.

Dadurch wird Scheduler Thrashing reduziert.

---

## Load Balancing

Locality darf nicht zu dauerhafter Überlastung einzelner CPUs führen.

Der Scheduler muss zwischen:

```text
Locality
    ↔
Load Balance
```

abwägen.

Die konkrete Gewichtung hängt von Scheduling Class und Execution Contract ab.

---

## Realtime

Realtime Concurrency Domains können Topologieanforderungen explizit festlegen.

Beispielsweise:

```text
Dedicated Core
No SMT Sharing
Specific NUMA Node
Local Device
```

Solche Required Constraints besitzen Vorrang vor allgemeinen Locality-Heuristiken.

---

## Deterministic Mode

Deterministic Mode muss topologyabhängige CPU-Auswahl kontrollieren können.

Dazu können gehören:

- feste CPU Sets,
- stabile Placement-Regeln,
- deaktivierte adaptive Migration,
- deterministisches Work Stealing.

Die Hardwaretopologie selbst bleibt dabei weiterhin Teil des Execution Environment.

---

## Resource Economy

Topology-Aware Scheduling muss mit der Resource Economy zusammenarbeiten.

Ein topologisch optimaler CPU-Kandidat darf nicht verwendet werden, wenn dadurch:

- CPU Budgets,
- Reservations,
- Thermal Limits,
- Power Constraints

verletzt würden.

---

## Energie und Thermal

Topologieinformationen können auch für Energie- und Thermalpolitik genutzt werden.

Beispielsweise kann der Scheduler:

- CPUs konsolidieren,
- zusätzliche Packages schlafen lassen,
- thermisch belastete Cores meiden,
- Efficiency-Cores für geeignete Background Workloads nutzen.

Diese Entscheidungen bleiben Soft Policies, sofern kein Hard Constraint vorliegt.

---

## Topologieänderungen

Hardwaretopologie kann sich zur Laufzeit ändern.

Beispiele:

- CPU Hotplug,
- deaktivierte Cores,
- Memory Hotplug,
- virtuelle CPU-Topologieänderungen.

Der Scheduler muss auf neue Generationen des Hardware System Graph reagieren können.

---

## Unvollständige Topologie

Nicht jede Plattform liefert vollständige oder zuverlässige Topologieinformationen.

Fehlende Informationen dürfen nicht zu unsicheren Annahmen führen.

NovaOS muss auf ein konservatives Scheduling-Modell zurückfallen können.

---

## Introspection

Topology-Aware Scheduling muss introspektierbar sein.

Für relevante Entscheidungen sollen beispielsweise sichtbar sein:

```text
Task
Source CPU
Target CPU
Affinity
NUMA Node
Cache Domain
Migration Cost
Placement Reason
```

Aggregierte Daten sollen Performance- und Locality-Probleme analysierbar machen.

---

## Adaptive Optimierung

NovaOS darf Soft-Locality-Policies adaptiv optimieren.

Messwerte können beispielsweise zeigen, ob:

- Cache Locality,
- NUMA Locality,
- Load Balancing,
- bestimmte Migrationen

für einen Workload tatsächlich vorteilhaft sind.

Prediction Error kann zur Korrektur dieser Heuristiken verwendet werden.

Hard Constraints bleiben davon unabhängig.

---

## Fail-Safe Defaults

Ist die Topologiebeziehung zwischen zwei Ressourcen unbekannt, darf NovaOS keine günstige Locality annehmen.

Unbekannte Topologie wird konservativ behandelt.

Required Affinity, Isolation, Security, Realtime und Resource Constraints besitzen immer Vorrang vor Locality-Optimierung.

---

## Normative Anforderungen

1. NovaOS MUSS Topology-Aware Scheduling unterstützen.
2. Der Scheduler MUSS den Hardware System Graph als primäre Topologiequelle verwenden.
3. Der Scheduler DARF KEINE konkurrierende unabhängige Hardwaretopologie als primäres Modell führen.
4. Scheduling Domains MÜSSEN aus der Hardwaretopologie ableitbar sein.
5. CPU Eligibility MUSS vor Soft-Locality-Optimierung bestimmt werden.
6. Required Affinity MUSS Vorrang vor Topology Optimization besitzen.
7. Cache Locality SOLL bei CPU Placement berücksichtigt werden.
8. NUMA Locality MUSS bei geeigneten Plattformen berücksichtigt werden.
9. CPU- und Memory-Placement SOLLEN gemeinsam optimierbar sein.
10. NUMA Distance MUSS als Scheduling-Kostenfaktor darstellbar sein.
11. SMT-Beziehungen MÜSSEN dem Scheduler bekannt sein können.
12. Realtime- und Isolation-Policies MÜSSEN SMT Sharing einschränken können.
13. Der Scheduler MUSS heterogene CPU-Klassen unterstützen können.
14. Required CPU Features MÜSSEN bei CPU Eligibility berücksichtigt werden können.
15. Device- und Accelerator-Locality MÜSSEN als Placement-Faktoren darstellbar sein.
16. Interrupt Affinity und Task Placement MÜSSEN koordinierbar sein.
17. Work Stealing MUSS Hardwaretopologie berücksichtigen können.
18. Lokales Work Stealing SOLL gegenüber entfernten Domains bevorzugt werden.
19. Remote Work Stealing MUSS zulässig bleiben, wenn es insgesamt vorteilhaft ist.
20. Task Migration MUSS als kostenbehaftete Operation behandelt werden.
21. Scheduling Policies SOLLEN Migration Hysteresis unterstützen.
22. Locality DARF notwendiges Load Balancing NICHT grundsätzlich verhindern.
23. Realtime Topology Constraints MÜSSEN als Hard Constraints ausdrückbar sein.
24. Deterministic Mode MUSS dynamische Topology-Heuristiken einschränken können.
25. Topology-Aware Scheduling MUSS mit der Resource Economy integriert sein.
26. Energie- und Thermalzustände DÜRFEN als Soft Placement-Faktoren berücksichtigt werden.
27. Der Scheduler MUSS auf Laufzeitänderungen der Hardwaretopologie reagieren können.
28. Unvollständige Topologie MUSS konservativ behandelt werden.
29. Topology Placement und relevante Migrationen MÜSSEN introspektierbar sein.
30. Adaptive Topology-Optimierung DARF Hard Scheduling Constraints NICHT verändern.

---

## Konsequenzen

### Positive Konsequenzen

- bessere Cache-Lokalität,
- geringere NUMA-Kosten,
- bessere Skalierung auf großen Mehrprozessorsystemen,
- bessere Unterstützung heterogener CPUs,
- effizienteres Work Stealing,
- koordinierbares CPU-, Memory-, Interrupt- und Device-Placement.

### Negative Konsequenzen

- CPU Placement wird komplexer,
- Scheduler benötigt aktuelle Topologieinformationen,
- Locality und Load Balancing können konkurrierende Ziele sein,
- Plattformen mit unvollständigen Topologieinformationen benötigen konservative Fallbacks.

---

## Verworfene Alternativen

### Alle CPUs als gleichwertig behandeln

Verworfen.

Dies ignoriert NUMA, Cache-Hierarchien, SMT und heterogene CPU-Klassen.

### NUMA ausschließlich durch den Memory Manager behandeln

Verworfen.

CPU- und Memory-Placement beeinflussen sich gegenseitig und müssen koordinierbar sein.

### Immer maximale Locality erzwingen

Verworfen.

Dies kann zu schlechter Lastverteilung und ungenutzter CPU-Kapazität führen.

### Immer auf die aktuell am wenigsten ausgelastete CPU migrieren

Verworfen.

Cache-, NUMA- und Migrationskosten können den kurzfristigen Lastvorteil übersteigen.

### Separate Scheduler-Topologie unabhängig vom Hardware System Graph

Verworfen.

Dies würde redundante und möglicherweise widersprüchliche Systemmodelle erzeugen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-SCHED-0001_Präemptiver_Mehrklassen_Scheduler`
- `ADR-SCHED-0002_Fair_Deadline_und_Realtime_Scheduling_in_einer_Architektur`
- `ADR-CONCURRENCY-0006_Work_Stealing_für_allgemeine_Task_Workloads`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-HAL-0007_Hotplug_über_deklarative_Device_Lifecycle_States`
- `ADR-INTERRUPT-0002_Interrupt_Affinity_und_Topology_Awareness`

---

## Zugehörige NPSPECs

- `NPSPEC-SCHED-TOPOLOGY-0001`
- `NPSPEC-SCHED-DOMAIN-0001`
- `NPSPEC-SCHED-NUMA-0001`
- `NPSPEC-SCHED-CACHE-0001`
- `NPSPEC-SCHED-SMT-0001`
- `NPSPEC-SCHED-HETEROGENEOUS-CPU-0001`
- `NPSPEC-SCHED-DEVICE-LOCALITY-0001`
- `NPSPEC-SCHED-MIGRATION-0001`
- `NPSPEC-SCHED-TOPOLOGY-TEST-0001`

---

## Ergebnis

NovaOS trifft CPU-Placement-Entscheidungen auf Basis der tatsächlichen Hardwaretopologie:

```text
Task
 ↓
Eligible CPUs
 ↓
Topology
 ↓
Locality + Load + Constraints
 ↓
Target CPU
```

Topology Awareness bleibt dabei den Hard Scheduling Constraints untergeordnet.

Die zentrale Architekturregel lautet:

```text
Know the topology.

Preserve useful locality.

Balance only when beneficial.

Never trade hard constraints
for a shorter run queue.
```