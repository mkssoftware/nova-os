# ADR-MEMORY-0002 – NUMA-Aware Physical Memory Allocation

## Status

Angenommen

## Kategorie

Kernel / Memory Management / Physical Memory / NUMA / Locality

## Kontext

`ADR-MEMORY-0001_Hierarchische_Physical_Memory_Architecture` definiert physischen Speicher als hierarchische Menge aus Memory Domains, Memory Regions und Allocation Pools.

Auf NUMA-Systemen besitzen CPUs jedoch keinen gleichwertigen Zugriff auf alle Speicherbereiche.

```text
CPU / NUMA Node 0 ── Local Memory 0
        │
        └──────────── Remote Memory 1

CPU / NUMA Node 1 ── Local Memory 1
        │
        └──────────── Remote Memory 0
```

Remote Memory kann gegenüber lokalem Speicher höhere Latenz, geringere Bandbreite und zusätzliche Interconnect-Kosten verursachen.

Da NovaOS NUMA bereits als First-Class-Hardwaretopologie behandelt, muss diese Information auch die physische Speicherallokation beeinflussen.

Eine NUMA-unabhängige Allokation würde die Entscheidungen des topology-aware Schedulers unterlaufen: Ein Task könnte auf der optimalen CPU laufen, während sein Working Set überwiegend auf einem entfernten NUMA-Node liegt.

---

## Entscheidung

NovaOS implementiert **NUMA-aware Physical Memory Allocation** als grundlegende Eigenschaft des PMM.

Speicherallokationen berücksichtigen:

- zulässige Memory Domains,
- NUMA-Topologie,
- NUMA-Distanzen,
- CPU-/Task-Locality,
- Device-Locality,
- Memory Pressure,
- Resource Reservations,
- explizite Placement Constraints.

Das Grundmodell lautet:

```text
Allocation Request
        ↓
Hard Memory Constraints
        ↓
Eligible NUMA Domains
        ↓
Preferred Locality
        ↓
NUMA Distance + Capacity + Pressure
        ↓
Physical Allocation
```

NUMA Locality ist standardmäßig ein Optimierungsziel. Explizite Required-NUMA-Constraints sind dagegen verbindlich.

---

## Grundprinzip

```text
Allocate memory near its expected users.

Prefer local memory.

Allow remote memory when policy permits.

Never violate required placement constraints.
```

---

## NUMA Domains

Jede NUMA-relevante physische Memory Region wird einer entsprechenden Memory Domain im Hardware System Graph zugeordnet.

Konzeptionell:

```text
NUMA Node
   ├── CPUs
   ├── Memory Regions
   ├── Devices
   └── Distance Relationships
```

Scheduler und Memory Manager verwenden damit dieselbe Topologiebeschreibung.

NUMA-IDs allein dürfen nicht als vollständiges Locality-Modell betrachtet werden.

---

## NUMA-Distanzen

NovaOS behandelt NUMA-Distanzen als relative Zugriffskosten.

```text
Node 0 → Node 0 = local
Node 0 → Node 1 = near
Node 0 → Node 2 = farther
```

Die Architektur darf nicht voraussetzen, dass alle Remote Nodes gleich teuer sind.

Auch asymmetrische Zugriffskosten müssen darstellbar sein.

Sind Distanzinformationen unbekannt, muss dies explizit dargestellt werden.

---

## Preferred NUMA Placement

Normale Speicherallokationen dürfen eine bevorzugte NUMA Domain besitzen.

Beispiel:

```text
Preferred NUMA Node = 2
```

Der PMM versucht zunächst dort zu allokieren.

Ist dort kein geeigneter Speicher verfügbar, darf entsprechend der Allocation Policy auf andere zulässige Domains ausgewichen werden.

---

## Required NUMA Placement

Bestimmte Allokationen benötigen verbindliche NUMA-Platzierung.

```text
Required NUMA Node = 2
```

Kann Node 2 die Anforderung nicht erfüllen, darf nicht still auf Node 1 ausgewichen werden.

Es gilt:

```text
Required NUMA unavailable
        ↓
Defined Failure / Explicit Fallback
```

Ein Fallback ist nur zulässig, wenn der zugehörige Contract ihn ausdrücklich erlaubt.

---

## CPU-Local Allocation

Kernel und Runtime sollen Speicher relativ zur vorgesehenen Ausführungslokalität allokieren können.

Beispiel:

```text
Current / Target CPU
        ↓
NUMA Domain
        ↓
Local Memory Pool
```

Dies ist insbesondere für kurzlebige oder CPU-lokale Datenstrukturen sinnvoll.

Die aktuelle CPU darf jedoch nicht automatisch als langfristig optimale Speicherlokation angenommen werden.

---

## Execution-Domain- und Scope-Locality

Speicher kann einem:

- Execution Domain,
- Execution Scope,
- Task,
- Task Group

zugeordnet sein.

Bei längerlebenden Allokationen kann deshalb die erwartete Ausführungstopologie des Owners wichtiger sein als die CPU, auf der die Allokation zufällig erzeugt wurde.

Scheduler und Memory Manager sollen solche Locality-Informationen austauschen können.

---

## First-Touch

NovaOS darf First-Touch-Placement als Policy für geeignete anonyme Speicherbereiche unterstützen.

Dabei wird physischer Speicher bevorzugt auf dem NUMA Node bereitgestellt, auf dem der tatsächliche erste relevante Zugriff erfolgt.

First-Touch ist jedoch keine universelle Regel.

Es kann ungeeignet sein, wenn die spätere Nutzungstopologie bereits bekannt ist oder Initialisierung und Nutzung durch unterschiedliche Tasks erfolgen.

---

## Interleave

Für Workloads mit breit verteilter paralleler Nutzung darf Speicher über mehrere NUMA Domains verteilt werden.

```text
Pages
  ├── Node 0
  ├── Node 1
  ├── Node 2
  └── Node 3
```

Interleaving ist eine explizite Placement Policy und kein Standard für alle Allokationen.

Es kann insbesondere für große, gleichmäßig parallel genutzte Datenmengen sinnvoll sein.

---

## Shared Memory

Gemeinsam verwendeter Speicher besitzt häufig keinen einzelnen optimalen NUMA Node.

Der PMM muss deshalb Shared-Memory-Placement anhand der erwarteten Nutzer berücksichtigen können.

Mögliche Policies sind:

```text
Preferred Home Node
Interleaved
Partitioned
Replicated Read-Mostly Data
```

Replication ist dabei eine höhere Memory-Management-Policy und wird nicht automatisch durch den PMM durchgeführt.

---

## Task Groups

Structured-Concurrency-Task-Groups können Arbeit und Daten NUMA-lokal partitionieren.

Beispiel:

```text
Task Group
   ├── Tasks Node 0 → Data Node 0
   └── Tasks Node 1 → Data Node 1
```

NovaOS soll solche Locality-Beziehungen ausdrücken können, ohne die Anwendung an konkrete physische NUMA-IDs zu koppeln, sofern dies nicht ausdrücklich gewünscht ist.

---

## Scheduler-Koordination

CPU Placement und Memory Placement müssen koordiniert werden.

```text
Task Placement
      ↕
Memory Placement
```

Der Scheduler darf berücksichtigen, wo sich das dominante Working Set eines Tasks befindet.

Der Memory Manager darf berücksichtigen, wo ein Task beziehungsweise Scope bevorzugt ausgeführt wird.

Keines der beiden Subsysteme darf die Entscheidung des anderen blind als unveränderlich voraussetzen.

---

## Remote Allocation

Remote Memory ist zulässig, wenn:

- keine Required-Locality-Regel entgegensteht,
- lokaler Speicher unter Druck steht,
- eine entfernte Domain insgesamt günstiger ist,
- Interleaving oder eine andere Policy dies verlangt.

Die Auswahl soll NUMA-Distanz und verfügbare Kapazität berücksichtigen.

---

## Memory Pressure

Memory Pressure wird NUMA-lokal betrachtet.

```text
Node 0:
    95 % used

Node 1:
    45 % used
```

Der PMM darf bei Soft-Locality-Anforderungen auf Node 1 ausweichen, wenn dies gegenüber starker lokaler Verdrängung oder Reclamation sinnvoller ist.

Dadurch wird NUMA Locality gegen tatsächliche Ressourcenverfügbarkeit abgewogen.

---

## Memory Migration

Eine einmal getroffene NUMA-Platzierung ist nicht zwingend permanent.

NovaOS muss spätere Memory Migration architektonisch ermöglichen.

Gründe können sein:

- Task Migration,
- dauerhaft veränderte Zugriffslokalität,
- Memory Pressure,
- NUMA Hotplug,
- Resource Rebalancing.

CPU Migration und Memory Migration bleiben getrennte Mechanismen.

---

## Migration Costs

Memory Migration besitzt Kosten:

- Speicherbandbreite,
- CPU-Zeit,
- Cache-Effekte,
- temporären zusätzlichen Speicher,
- mögliche Page-Fault- oder Mapping-Arbeit.

Migration darf deshalb nicht bei jeder kurzfristigen Locality-Änderung ausgelöst werden.

Policies sollen Hysterese und Mindestnutzen berücksichtigen.

---

## Device Locality

NUMA Placement muss auch Device-Topologie berücksichtigen können.

Beispiel:

```text
GPU / NIC
   ↓
PCIe Root Complex
   ↓
NUMA Node 1
   ↓
Preferred DMA Memory
```

Für I/O-intensive Workloads kann Device-Locality wichtiger sein als die Locality einer einzelnen CPU.

Required DMA- und IOMMU-Constraints bleiben dabei Hard Constraints.

---

## Realtime

Realtime Concurrency Domains können NUMA-lokale Speicherressourcen reservieren.

Hard-Realtime-Workloads dürfen insbesondere Anforderungen definieren wie:

```text
Required CPU Domain = Node 1
Required Memory Domain = Node 1
No automatic remote fallback
```

Damit kann unvorhersehbare Remote-Memory-Latenz vermieden werden.

Solche Reservations müssen bereits bei Admission Control berücksichtigt werden.

---

## Heterogeneous Memory

Ein NUMA Node kann unterschiedliche Memory Classes enthalten oder eine Memory Class kann über mehrere Domains verteilt sein.

Daher gilt:

```text
NUMA Locality != Memory Class
```

Beide Dimensionen müssen getrennt modelliert und gemeinsam ausgewertet werden.

Beispiel:

```text
Required Memory Class = High-Bandwidth
Preferred NUMA Node = 0
```

Zuerst wird der Hard Constraint erfüllt, danach innerhalb geeigneter Speicherressourcen Locality optimiert.

---

## Zero-Copy

Zero-Copy-Pipelines können Speicher zwischen CPUs und Devices teilen.

In solchen Fällen muss der PMM die gesamte Nutzungstopologie berücksichtigen können.

```text
CPU
  ↘
   Shared Buffer
  ↗
Device
```

Die beste Platzierung kann deshalb von einer rein CPU-zentrierten NUMA-Entscheidung abweichen.

---

## Deterministic Mode

Deterministic Mode muss adaptive NUMA-Placement- und Migration-Policies einschränken können.

Eine deterministische Ausführung kann beispielsweise:

- feste Memory Domains,
- feste Interleave-Regeln,
- deaktivierte automatische Migration

verwenden.

Die NUMA-Semantik selbst bleibt unverändert.

---

## Hotplug

Wird eine NUMA Memory Domain offline genommen, müssen ihre Allokationen klassifiziert werden:

```text
Movable
Pinned
Reserved
Device-bound
Realtime-bound
```

Verschiebbare Allokationen können auf zulässige Domains migriert werden.

Nicht verschiebbare Allokationen können das Offline-Nehmen verhindern.

---

## Adaptive Placement

NovaOS darf reale Zugriffsmuster beobachten und daraus NUMA-Placement- oder Migrationsempfehlungen ableiten.

Beispiel:

```text
Expected Locality
       ↓
Actual Memory Access
       ↓
Prediction Error
       ↓
Policy Adjustment
```

Adaptive Entscheidungen bleiben Soft Policy.

Sie dürfen keine Required-NUMA-, Realtime-, Security-, DMA- oder Data-Sovereignty-Constraints verletzen.

---

## Introspection

NUMA Placement muss über Architecture Introspection nachvollziehbar sein.

Relevant sind insbesondere:

```text
Allocation Owner
Physical NUMA Domain
Preferred NUMA Domain
Required NUMA Domain
Memory Class
Remote / Local Status
Migration History
Memory Pressure
Placement Reason
```

Dadurch können schlechte Locality und unerwartete Remote-Memory-Nutzung diagnostiziert werden.

---

## Normative Anforderungen

1. NovaOS MUSS NUMA-aware Physical Memory Allocation unterstützen.
2. NUMA-relevante Memory Regions MÜSSEN ihrer Hardwaretopologie zugeordnet sein.
3. Scheduler und Memory Manager MÜSSEN eine gemeinsame NUMA-Topologie verwenden.
4. NUMA-Distanzen MÜSSEN als relative Zugriffskosten darstellbar sein.
5. Unterschiedliche Remote-Distanzen DÜRFEN NICHT pauschal als gleich angenommen werden.
6. Asymmetrische NUMA-Kosten MÜSSEN darstellbar sein.
7. Preferred und Required NUMA Placement MÜSSEN unterschieden werden.
8. Required NUMA Placement DARF NICHT still verletzt werden.
9. Preferred NUMA Placement DARF bei Ressourcenknappheit kontrolliert auf entfernte Domains ausweichen.
10. CPU-lokale physische Allokation MUSS möglich sein.
11. Längerlebende Allokationen MÜSSEN unabhängig von der zufälligen Allocator-CPU platzierbar sein.
12. First-Touch DARF als optionale Placement Policy unterstützt werden.
13. Interleaved NUMA Placement MUSS für geeignete Speicherbereiche möglich sein.
14. Shared Memory MUSS NUMA-aware platziert werden können.
15. Task- und Datenpartitionierung innerhalb von Task Groups MUSS NUMA-aware unterstützt werden können.
16. Scheduler und Memory Manager MÜSSEN CPU- und Memory-Locality koordinieren können.
17. Remote Allocation MUSS NUMA-Distanz und Ressourcenverfügbarkeit berücksichtigen können.
18. NUMA-lokaler Memory Pressure MUSS bei Soft-Placement-Entscheidungen berücksichtigt werden.
19. Memory Migration zwischen NUMA Domains MUSS architektonisch möglich sein.
20. CPU Migration und Memory Migration MÜSSEN getrennte Mechanismen bleiben.
21. Migration Policies MÜSSEN Kosten und Hysterese berücksichtigen.
22. Device Locality MUSS in NUMA-Placement-Entscheidungen einbezogen werden können.
23. Realtime-Domains MÜSSEN NUMA-lokale Speicherressourcen reservieren können.
24. NUMA Locality und Memory Class MÜSSEN getrennt modelliert werden.
25. Zero-Copy-Placement MUSS CPU- und Device-Locality gemeinsam berücksichtigen können.
26. Deterministic Mode MUSS adaptive NUMA-Migration und Placement-Policies einschränken können.
27. NUMA Memory Hotplug MUSS mit Memory Migration und Reservations koordiniert werden.
28. Adaptive NUMA-Policies DÜRFEN ausschließlich innerhalb der durch Hard Constraints zulässigen Lösungsmenge arbeiten.
29. NUMA Placement und Migration MÜSSEN introspektierbar sein.
30. Unbekannte NUMA-Topologieinformationen DÜRFEN NICHT als optimale Locality interpretiert werden.

---

## Konsequenzen

### Positive Konsequenzen

- geringere Remote-Memory-Latenz,
- bessere Speicherbandbreite auf Multi-Socket- und NUMA-Systemen,
- Scheduler- und Memory-Placement arbeiten gemeinsam statt gegeneinander,
- Realtime-Workloads können kontrollierte CPU-/Memory-Locality erhalten,
- Device-, DMA- und Zero-Copy-Workloads können topology-aware platziert werden,
- große Systeme skalieren besser mit steigender Anzahl von NUMA Domains.

### Negative Konsequenzen

- Allocation Policies werden komplexer,
- CPU- und Memory-Migration müssen koordiniert werden,
- optimale Platzierung kann sich während der Laufzeit ändern,
- NUMA-Monitoring und Migration erzeugen zusätzliche Kosten.

---

## Verworfene Alternativen

### Physische Allokation ohne NUMA-Berücksichtigung

Verworfen.

Dies würde Topology-Aware Scheduling teilweise wirkungslos machen und unnötige Remote-Memory-Zugriffe erzeugen.

### Immer ausschließlich lokalen Speicher verwenden

Verworfen.

Lokaler Speicher kann erschöpft sein oder andere Hard Constraints können eine andere Domain verlangen.

### Automatisches Remote-Fallback auch bei Required Placement

Verworfen.

Required Placement ist ein Hard Constraint und darf nicht still abgeschwächt werden.

### NUMA ausschließlich durch den Scheduler behandeln

Verworfen.

CPU Placement allein kann keine Datenlokalität herstellen, wenn physischer Speicher auf einer ungeeigneten Domain liegt.

### Permanente automatische Memory Migration

Verworfen.

Migration besitzt erhebliche Kosten und kann bei wechselnden Zugriffsmustern zu Thrashing führen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-MEMORY-0001_Hierarchische_Physical_Memory_Architecture`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0018_Locality_als_Optimierungsziel`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-CONCURRENCY-0002_Task_Groups_mit_hierarchischer_Lebensdauer`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-SCHED-0003_Topology_Aware_Scheduling`
- `ADR-SCHED-0004_NUMA_Aware_Scheduling`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-MEMORY-NUMA-DISTANCE-0001`
- `NPSPEC-MEMORY-NUMA-PLACEMENT-0001`
- `NPSPEC-MEMORY-NUMA-POLICY-0001`
- `NPSPEC-MEMORY-NUMA-MIGRATION-0001`
- `NPSPEC-MEMORY-NUMA-PRESSURE-0001`
- `NPSPEC-MEMORY-NUMA-DEVICE-0001`
- `NPSPEC-MEMORY-NUMA-TEST-0001`

---

## Ergebnis

NovaOS verbindet physische Speicherallokation unmittelbar mit der NUMA-Topologie:

```text
Allocation Request
        ↓
Hard Constraints
        ↓
Eligible Memory Domains
        ↓
NUMA Locality
 + Distance
 + Capacity
 + Pressure
 + Device Locality
        ↓
Physical Memory Placement
```

Damit werden CPU Placement und Memory Placement als zusammenhängendes Locality-Problem behandelt, ohne Hard Constraints zugunsten vermeintlich optimaler Nähe zu verletzen.

Die zentrale Architekturregel lautet:

```text
Keep computation close to its data.

Keep data close to its users.

Treat locality as topology, not guesswork.

Prefer local memory, but obey constraints first.
```