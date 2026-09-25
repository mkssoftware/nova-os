# NPSPEC-MEMORY-NUMA-0001 – Nova NUMA Memory Management

## Status

Angenommen

## Kategorie

Memory / NUMA / Physical Memory / Locality

## Zweck

NovaOS definiert NUMA-aware Memory Management zur topologiebewussten Verwaltung physischen Speichers auf Systemen mit Non-Uniform Memory Access.

```text
CPU Node 0 ── Local Memory 0
    │
    └──────── Remote Memory 1

CPU Node 1 ── Local Memory 1
    │
    └──────── Remote Memory 0
```

Ziel ist, Speicher möglichst nahe an den CPUs und Geräten zu platzieren, die ihn tatsächlich verwenden.

## Grundprinzip

Bei NUMA besitzen Speicherzugriffe unterschiedliche Kosten.

```text
Local Memory Access
        <
Remote Memory Access
```

Dabei gilt:

```text
Available Memory ≠ Equally Suitable Memory
```

Speicherlokalität ist daher Bestandteil der Allokationsentscheidung.

Harte Anforderungen haben jedoch Vorrang vor NUMA-Optimierung.

## NUMA-Topologie

NovaOS muss die durch HAL und Firmware erkannte NUMA-Topologie verwenden.

Ein NUMA-Node beschreibt mindestens:

```text
NodeID
CPUs
Physical Memory
Distance Information
State
```

Optional:

```text
Devices
Accelerators
Memory Types
Bandwidth
Latency
```

## Memory Nodes

Physische Speicherbereiche müssen einem NUMA-Node zugeordnet werden können.

```text
Node 0
├── CPU 0
├── CPU 1
└── Memory Region A

Node 1
├── CPU 2
├── CPU 3
└── Memory Region B
```

Speicher ohne bekannte NUMA-Zuordnung muss kontrolliert behandelt werden.

```text
Unknown Node ≠ Local Node
```

## Distance Model

NovaOS soll relative Kosten zwischen NUMA-Nodes abbilden können.

```text
        N0   N1   N2
N0      10   20   35
N1      20   10   25
N2      35   25   10
```

Die Werte stellen relative Zugriffskosten dar und dürfen nicht als absolute Zeitwerte interpretiert werden.

## Allocation Policy

Eine Speicherallokation darf NUMA-Anforderungen enthalten.

Beispiele:

```text
PreferredNode
AllowedNodes
StrictNode
Interleave
LocalToCPU
LocalToDevice
```

Harte Constraints dürfen durch Fallback nicht verletzt werden.

## Local Allocation

Standardmäßig soll Speicher möglichst lokal zum ausführenden Task oder dessen bevorzugter CPU allokiert werden.

```text
Task
 ↓
Current / Preferred CPU
 ↓
NUMA Node
 ↓
Local Memory
```

Ist lokaler Speicher nicht verfügbar, darf entsprechend der Policy auf andere Nodes ausgewichen werden.

## Strict Allocation

Für bestimmte Ressourcen darf eine harte Node-Bindung verwendet werden.

```text
StrictNode = Node 2
```

Kann dort kein geeigneter Speicher bereitgestellt werden:

```text
Allocation Failure
```

Ein stiller Remote-Fallback ist bei einer harten Bindung nicht erlaubt.

## Interleaving

Speicher darf über mehrere NUMA-Nodes verteilt werden.

```text
Page 0 → Node 0
Page 1 → Node 1
Page 2 → Node 0
Page 3 → Node 1
```

Interleaving kann für große parallele Workloads sinnvoll sein, deren Zugriffsmuster nicht eindeutig einem einzelnen Node zugeordnet werden können.

## First Touch

NovaOS darf eine First-Touch-Policy unterstützen.

```text
Virtual Allocation
      ↓
First Physical Access
      ↓
Current NUMA Node
      ↓
Physical Frame Allocation
```

Dadurch kann Speicher entsprechend der tatsächlichen Ausführungslokalität platziert werden.

## Memory Migration

NovaOS darf physische Seiten zwischen NUMA-Nodes migrieren.

```text
Task moves to Node 1
        ↓
Remote Memory Access
        ↓
Migration beneficial?
        ↓
Move Pages to Node 1
```

Migration darf nur erfolgen, wenn Kosten und Sicherheitsbedingungen dies erlauben.

```text
Migration Cost < Expected Locality Benefit
```

ist eine Optimierungsentscheidung und keine harte Regel.

## Task Migration

Speicherlokalität muss gemeinsam mit Scheduler-Entscheidungen betrachtet werden.

Mögliche Strategien:

```text
Move Task to Memory
Move Memory to Task
Keep Current Placement
Replicate Read-Mostly Data
```

Scheduler und Memory Manager sollen widersprüchliche Migrationen vermeiden.

## Shared Memory

Gemeinsam verwendeter Speicher besitzt möglicherweise keinen eindeutig optimalen Node.

```text
Task A / Node 0 ─┐
                 ├→ Shared Memory
Task B / Node 1 ─┘
```

NovaOS darf dafür Interleaving, Replikation oder eine kostenbasierte Platzierung verwenden.

Schreibbarer gemeinsam genutzter Speicher darf nicht ohne definierte Konsistenzsemantik repliziert werden.

## Device Locality

Speicherplatzierung darf die physische Nähe zu Geräten berücksichtigen.

```text
Device
  ↓
PCIe / IOMMU Topology
  ↓
NUMA Node
  ↓
Local DMA Memory
```

Dies ist insbesondere für DMA, Netzwerkgeräte und Beschleuniger relevant.

## Memory Pressure

Speicherdruck muss pro NUMA-Node erkennbar sein.

```text
Node 0 → Low Memory
Node 1 → Free Memory
```

Mögliche Reaktionen:

```text
Remote Allocation
Migration
Reclamation
Cache Reduction
Load Migration
```

Die konkrete Entscheidung erfolgt entsprechend Resource- und Allocation-Policy.

## Hotplug

NUMA-Nodes und Speicherbereiche dürfen dynamisch online oder offline gehen.

```text
Node Offline
    ↓
Stop New Allocations
    ↓
Drain / Migrate Memory
    ↓
Remove Node
```

Nicht migrierbare Ressourcen müssen ein Offline-Schalten verhindern können.

## UMA-Systeme

Auf Systemen ohne NUMA muss dieselbe API verwendbar bleiben.

```text
UMA System
    ↓
Single Logical Memory Node
```

NUMA-Unterstützung darf keine unnötige Sonderbehandlung für höhere Komponenten erzwingen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
NUMA Nodes
Memory per Node
Free Memory per Node
Allocation Distribution
Remote Allocation Count
Migration Count
Memory Pressure
Distance Information
```

## Normative Anforderungen

1. NovaOS MUSS NUMA-Topologie über die HAL abbilden können.
2. Physischer Speicher MUSS auf NUMA-Systemen einem Node zugeordnet werden können.
3. Lokaler Speicher SOLL gegenüber entferntem Speicher bevorzugt werden.
4. Harte Node-Constraints DÜRFEN NICHT durch automatische Fallbacks verletzt werden.
5. Remote-Fallback MUSS durch die jeweilige Allocation Policy kontrollierbar sein.
6. Interleaving und First-Touch SOLLEN unterstützt werden können.
7. Speicher- und Task-Migration SOLLEN gemeinsam hinsichtlich Lokalität bewertet werden.
8. Shared Memory MUSS explizit als potenziell Multi-Node-Ressource behandelt werden.
9. Device Locality SOLL bei DMA- und Accelerator-Speicher berücksichtigt werden.
10. Memory Pressure MUSS pro NUMA-Node erkennbar sein.
11. NUMA Memory Hotplug DARF keine aktiven Speicherreferenzen beschädigen.
12. NUMA-Platzierung, Remote-Zugriffe und Migrationen SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-FRAMEALLOCATOR-0001`
- `NPSPEC-MEMORY-BUDDY-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-NUMA-0001`
- `NPSPEC-HAL-HOTPLUG-0001`
- `NPSPEC-SCHEDULER-NUMA-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-MEMORY-0006`

## Ergebnis

```text
Hardware Topology
       ↓
NUMA Memory Model
       ↓
Allocation Policy
   ┌───┼─────────┐
   ↓   ↓         ↓
Local Interleave Remote
   ↓
Runtime Observation
   ↓
Migration / Rebalancing
```

NovaOS erhält damit eine NUMA-bewusste Speicherverwaltung, die CPU-, Speicher- und Gerätelokalität gemeinsam berücksichtigt und physische Speicherplatzierung mit Scheduler, Resource Economy und dynamischer Migration verbindet.