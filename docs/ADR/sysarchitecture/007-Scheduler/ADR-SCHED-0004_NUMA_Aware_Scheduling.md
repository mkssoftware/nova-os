# ADR-SCHED-0004 – NUMA-Aware Scheduling

## Status

Angenommen

## Kategorie

Kernel / Scheduler / NUMA / Memory Locality / CPU Placement / Ressourcen

## Kontext

`ADR-SCHED-0003_Topology_Aware_Scheduling` definiert Hardwaretopologie als Grundlage für CPU-Placement. NUMA benötigt darüber hinaus eine eigene Scheduling-Entscheidung, da CPU- und Speicherort unmittelbar miteinander gekoppelt sind.

Auf NUMA-Systemen besitzt ein CPU-Knoten typischerweise schnelleren Zugriff auf lokalen Speicher als auf Speicher anderer Nodes.

```text
NUMA Node 0                NUMA Node 1
┌─────────────┐            ┌─────────────┐
│ CPUs        │            │ CPUs        │
│ Local RAM   │            │ Local RAM   │
└──────┬──────┘            └──────┬──────┘
       │       Interconnect       │
       └──────────────────────────┘
```

Eine reine CPU-Lastverteilung kann daher einen Task von seinen Daten entfernen und trotz freier CPU-Kapazität die Gesamtleistung verschlechtern.

NovaOS behandelt NUMA bereits als First-Class-Topologie. Der Scheduler muss diese Information direkt in seine Placement- und Migration-Entscheidungen einbeziehen.

---

## Entscheidung

NovaOS verwendet **NUMA-Aware Scheduling** als Spezialisierung des allgemeinen Topology-Aware Scheduling.

Der Scheduler betrachtet CPU- und Memory-Locality gemeinsam:

```text
Runnable Task
      ↓
Eligible CPUs
      ↓
Task Memory Locality
      +
NUMA Topology
      +
NUMA Distance
      ↓
Placement Policy
      ↓
Target CPU / NUMA Node
```

Bevorzugt wird eine Ausführung nahe den für den Task relevanten Daten, sofern keine höherwertigen Constraints entgegenstehen.

---

## Grundprinzip

```text
Move computation toward data
when that is cheaper than moving data.

Treat remote memory access as a cost.

Balance CPU load without ignoring memory locality.
```

---

## NUMA Node

NovaOS verwendet die durch den Hardware System Graph definierten NUMA Nodes.

Ein NUMA Node beschreibt eine Locality Domain mit zugeordneten:

- CPUs,
- Memory Regions,
- Entfernungs- beziehungsweise Kostenbeziehungen.

Der Scheduler erzeugt kein unabhängiges NUMA-Modell.

---

## NUMA Distance

NUMA Nodes dürfen nicht lediglich als `local` oder `remote` betrachtet werden.

NovaOS muss unterschiedliche Zugriffskosten darstellen können:

```text
Node A → Node A = 10
Node A → Node B = 20
Node A → Node C = 35
```

Die Werte sind relative Kosten und keine garantierten Latenzen.

Asymmetrische Topologien müssen darstellbar bleiben.

---

## Task Memory Locality

Der Scheduler soll bestimmen können, mit welchen Memory Domains ein Task hauptsächlich arbeitet.

Dazu können Informationen aus:

- Memory Manager,
- Execution Scope,
- Working Set,
- Shared Memory,
- Buffer Ownership

verwendet werden.

Die konkrete Ermittlung ist Policy und wird nicht durch diese ADR festgelegt.

---

## Preferred NUMA Node

Ein Task kann einen bevorzugten NUMA Node besitzen.

```text
Task
Preferred NUMA Node = 2
```

Diese Präferenz kann aus seinem aktuellen Working Set oder einem expliziten Execution Contract entstehen.

Eine Preferred Locality ist kein Hard Constraint.

---

## Required NUMA Placement

Bestimmte Workloads müssen einen NUMA Node beziehungsweise eine definierte Node-Menge als Hard Constraint festlegen können.

Beispiele sind:

- Realtime Workloads,
- spezielle Device Workloads,
- explizite Memory Reservations.

Required NUMA Constraints dürfen nicht durch Load Balancing überschrieben werden.

---

## CPU Placement

Besitzt ein Task einen dominanten lokalen Memory Node, soll der Scheduler zunächst geeignete CPUs dieses Nodes betrachten.

```text
Task Memory → Node 1
                 ↓
          CPUs in Node 1
                 ↓
          Preferred Targets
```

Erst wenn dies nicht sinnvoll oder zulässig ist, sollen entferntere Nodes berücksichtigt werden.

---

## CPU Load vs Memory Locality

NUMA-Aware Scheduling muss zwischen CPU-Auslastung und Memory Locality abwägen.

```text
Local CPU heavily loaded
        ↕
Remote CPU available
```

Eine Remote-Ausführung kann sinnvoll sein, wenn der Load-Balancing-Vorteil die zusätzlichen Memory-Kosten übersteigt.

Locality wird deshalb nicht absolut erzwungen, sofern sie kein Required Constraint ist.

---

## Task Migration

Wird ein Task auf einen anderen NUMA Node verschoben, muss dies als kostenbehaftete Migration behandelt werden.

Die Entscheidung soll berücksichtigen:

- erwartete Restlaufzeit,
- Working-Set-Größe,
- NUMA Distance,
- CPU-Last,
- Migration Frequency.

Kurzlebige Tasks sollen nicht unnötig wegen kleiner Lastunterschiede migriert werden.

---

## Memory Migration

CPU Migration und Memory Migration sind getrennte Mechanismen.

Nach einer längerfristigen Task Migration kann NovaOS entscheiden:

```text
Move Task
    ↓
Observe Locality
    ↓
Move selected memory pages
```

oder alternativ:

```text
Move Task back toward memory
```

Der Scheduler entscheidet nicht eigenständig über physische Page Migration, sondern koordiniert dies mit dem Memory Manager.

---

## Shared Memory

Bei gemeinsam genutztem Speicher existiert möglicherweise kein einzelner optimaler NUMA Node.

Der Scheduler muss deshalb Shared-Memory-Workloads berücksichtigen können.

Mögliche Policies sind:

- gemeinsame Locality Domain,
- Memory Interleaving,
- Task-Verteilung entlang der Datenpartitionen,
- bewusste Remote-Zugriffe.

---

## Task Groups

Task Groups können NUMA-lokal oder über mehrere NUMA Nodes verteilt ausgeführt werden.

Bei datenparallelisierten Workloads soll eine Partitionierung möglich sein wie:

```text
Task Group
   ├── Partition A → Node 0 Data → Node 0 CPU
   └── Partition B → Node 1 Data → Node 1 CPU
```

Die Structured-Concurrency-Ownership bleibt davon unverändert.

---

## Work Stealing

Work Stealing soll NUMA-hierarchisch erfolgen.

Bevorzugt wird:

```text
Local Worker
      ↓
Same NUMA Node
      ↓
Nearby NUMA Node
      ↓
More distant Node
```

Remote Stealing bleibt möglich, muss aber NUMA-Kosten berücksichtigen.

---

## Realtime

Realtime Concurrency Domains können NUMA Placement als Hard Constraint festlegen.

Beispielsweise:

```text
CPU Set = Node 1
Memory = Node 1
Device = Node 1
```

Damit können vorhersehbarere Speicherzugriffe erreicht werden.

Eine Hard-Realtime-Domain darf nicht allein zur allgemeinen Lastverteilung auf einen entfernten Node verschoben werden.

---

## Device Locality

Geräte können topologisch einem NUMA Node näher sein.

Dies betrifft beispielsweise:

- NVMe,
- Netzwerkadapter,
- GPUs,
- andere PCIe-Geräte.

Für I/O-intensive Workloads soll der Scheduler daher kombinierte Locality berücksichtigen können:

```text
Device
  ↕
Memory
  ↕
CPU
```

---

## Resource Economy

NUMA-Ressourcen sind Teil der systemweiten Resource Economy.

Memory Reservations können nodebezogen sein.

Ein Scheduler darf einen Workload nicht auf einen Node verschieben, wenn dessen erforderliche lokale Ressourcen dort nicht verfügbar oder nicht zulässig sind.

---

## Memory Pressure

Lokaler Memory Pressure kann eine NUMA-Placement-Entscheidung beeinflussen.

NovaOS kann abhängig von Policy:

- entfernten Speicher verwenden,
- Tasks verschieben,
- Speicher migrieren,
- Workloads degradieren.

Hard Memory- und Realtime-Constraints bleiben bindend.

---

## Heterogene Memory Domains

Das Modell darf nicht voraussetzen, dass alle NUMA Nodes denselben Speichertyp besitzen.

Zukünftige Systeme können beispielsweise unterschiedliche:

- Latenz,
- Bandbreite,
- Persistenz,
- Energieeigenschaften

besitzen.

NUMA-Aware Scheduling muss mit solchen heterogenen Memory Domains kompatibel bleiben.

---

## Deterministic Mode

Deterministic Mode muss dynamische NUMA-Migrationen einschränken können.

Ein deterministischer Execution Contract kann beispielsweise:

- feste NUMA Nodes,
- feste CPU Sets,
- deaktivierte adaptive Migration

verlangen.

---

## Hotplug

CPU- und Memory-Hotplug können NUMA-Topologie verändern.

Der Scheduler muss auf neue Generationen des Hardware System Graph reagieren.

Tasks auf nicht mehr verfügbaren Nodes müssen entsprechend ihrer Constraints neu platziert oder explizit als nicht ausführbar behandelt werden.

---

## Introspection

NUMA-bezogene Scheduling-Entscheidungen müssen introspektierbar sein.

Beispiel:

```text
Task:
    simulation.partition.4

CPU Node:
    1

Primary Memory Node:
    1

Remote Memory:
    8 %

Preferred Node:
    1

Placement:
    Local
```

Damit können NUMA-Probleme gezielt analysiert werden.

---

## Adaptive Optimierung

NovaOS darf beobachten, ob NUMA-Placement-Entscheidungen tatsächlich vorteilhaft waren.

Prediction Error kann dabei beispielsweise entstehen aus:

```text
Predicted locality benefit
        ↓
Actual performance
        ↓
Deviation
        ↓
Policy adjustment
```

Dies darf ausschließlich Soft Policies beeinflussen.

---

## Fail-Safe Defaults

Sind NUMA-Beziehungen unbekannt, darf NovaOS keine günstige Locality voraussetzen.

Required Affinity, Security, Realtime, Resource und Data-Sovereignty-Constraints besitzen Vorrang vor NUMA-Optimierung.

---

## Normative Anforderungen

1. NovaOS MUSS NUMA-Aware Scheduling unterstützen.
2. NUMA-Aware Scheduling MUSS auf dem Hardware System Graph aufbauen.
3. Der Scheduler DARF KEIN unabhängiges konkurrierendes NUMA-Modell als primäre Quelle verwenden.
4. CPUs und Memory Regions MÜSSEN NUMA Nodes zugeordnet werden können.
5. NUMA Distance MUSS als Scheduling-Kostenfaktor darstellbar sein.
6. Asymmetrische NUMA-Kosten MÜSSEN darstellbar bleiben.
7. Der Scheduler MUSS Task Memory Locality berücksichtigen können.
8. Tasks MÜSSEN Preferred NUMA Nodes besitzen können.
9. Workloads MÜSSEN Required NUMA Constraints ausdrücken können.
10. Required NUMA Constraints MÜSSEN Vorrang vor Load Balancing besitzen.
11. CPU Placement SOLL lokale Memory Nodes bevorzugen.
12. Memory Locality DARF Soft Load Balancing NICHT vollständig verhindern.
13. Remote CPU Placement MUSS als kostenbehaftete Entscheidung behandelt werden.
14. Task Migration zwischen NUMA Nodes SOLL Migration Cost berücksichtigen.
15. CPU Migration und Memory Migration MÜSSEN getrennte Mechanismen bleiben.
16. Scheduler und Memory Manager MÜSSEN NUMA Placement koordinieren können.
17. Shared-Memory-Workloads MÜSSEN ohne Annahme eines einzelnen lokalen Nodes unterstützt werden.
18. Task Groups MÜSSEN NUMA-lokal oder NUMA-verteilt ausführbar sein.
19. Work Stealing MUSS NUMA Distance berücksichtigen können.
20. Lokales Work Stealing SOLL gegenüber Remote Stealing bevorzugt werden.
21. Realtime Domains MÜSSEN NUMA Placement als Hard Constraint definieren können.
22. Device-, CPU- und Memory-Locality MÜSSEN gemeinsam optimierbar sein.
23. Nodebezogene Resource Reservations MÜSSEN unterstützt werden können.
24. Lokaler Memory Pressure MUSS in Soft Placement Policies berücksichtigt werden können.
25. Heterogene Memory Domains DÜRFEN NICHT durch die Architektur ausgeschlossen werden.
26. Deterministic Mode MUSS dynamische NUMA-Migration einschränken können.
27. NUMA Scheduling MUSS CPU- und Memory-Hotplug berücksichtigen können.
28. NUMA Placement und relevante Locality-Zustände MÜSSEN introspektierbar sein.
29. Unbekannte NUMA-Beziehungen MÜSSEN konservativ behandelt werden.
30. Adaptive NUMA-Optimierung DARF Hard Constraints NICHT verändern.

---

## Konsequenzen

### Positive Konsequenzen

- weniger unnötige Remote-Memory-Zugriffe,
- bessere Skalierung auf Multi-Socket-Systemen,
- bessere CPU-/Memory-Koordination,
- NUMA-bewusstes Work Stealing,
- bessere Device Locality,
- Grundlage für heterogene zukünftige Memory-Systeme.

### Negative Konsequenzen

- Scheduler und Memory Manager müssen enger kooperieren,
- Migrationen benötigen Kostenmodelle,
- Shared-Memory-Workloads besitzen nicht immer eine eindeutige optimale Platzierung,
- NUMA-Topologie und Working Sets müssen beobachtbar sein.

---

## Verworfene Alternativen

### NUMA nur im Memory Allocator berücksichtigen

Verworfen.

Lokaler Speicher bringt wenig, wenn der zugehörige Task dauerhaft auf entfernten CPUs ausgeführt wird.

### Tasks immer bei ihren ursprünglichen NUMA Nodes halten

Verworfen.

Dies kann bei ungleichmäßiger Last zu schlechter CPU-Auslastung führen.

### Immer Tasks statt Speicher migrieren

Verworfen.

Je nach Working Set und Laufzeit kann Memory Migration günstiger sein.

### Immer Speicher statt Tasks migrieren

Verworfen.

Bei großen Working Sets kann dies erheblich teurer sein als CPU Migration.

### NUMA Nodes als gleich weit entfernt behandeln

Verworfen.

Moderne Multi-Socket- und Fabric-Systeme können unterschiedliche und asymmetrische Distanzkosten besitzen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-SCHED-0001_Präemptiver_Mehrklassen_Scheduler`
- `ADR-SCHED-0002_Fair_Deadline_und_Realtime_Scheduling_in_einer_Architektur`
- `ADR-SCHED-0003_Topology_Aware_Scheduling`
- `ADR-CONCURRENCY-0006_Work_Stealing_für_allgemeine_Task_Workloads`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-INTERRUPT-0002_Interrupt_Affinity_und_Topology_Awareness`

---

## Zugehörige NPSPECs

- `NPSPEC-SCHED-NUMA-0001`
- `NPSPEC-SCHED-NUMA-DISTANCE-0001`
- `NPSPEC-SCHED-NUMA-PLACEMENT-0001`
- `NPSPEC-SCHED-NUMA-MIGRATION-0001`
- `NPSPEC-SCHED-NUMA-MEMORY-0001`
- `NPSPEC-SCHED-NUMA-WORKSTEALING-0001`
- `NPSPEC-SCHED-NUMA-DEVICE-0001`
- `NPSPEC-SCHED-NUMA-TEST-0001`

---

## Ergebnis

NovaOS behandelt NUMA Scheduling als gemeinsame Optimierung von Ausführungs- und Speicherort:

```text
Task
  +
Working Set
  +
NUMA Topology
  +
CPU Load
      ↓
NUMA-Aware Placement
      ↓
CPU / Memory Locality
```

Dabei gilt nicht grundsätzlich:

```text
move task
```

oder:

```text
move memory
```

sondern NovaOS kann abhängig von Kosten und Contracts die geeignete Strategie wählen.

Die zentrale Architekturregel lautet:

```text
Keep computation near its data.

Treat distance as cost.

Balance load without destroying locality.

Move the cheaper side.
```