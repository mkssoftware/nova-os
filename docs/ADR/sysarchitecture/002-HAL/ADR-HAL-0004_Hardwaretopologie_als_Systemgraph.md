# ADR-HAL-0004 – Hardwaretopologie als Systemgraph

## Status

Angenommen

## Kategorie

HAL / Hardwaretopologie / Systemgraph / Ressourcenmodell / Lokalität / Introspection

## Kontext

Moderne Computersysteme bestehen nicht nur aus einer flachen Liste von CPUs und Geräten.

Relevante Beziehungen umfassen beispielsweise:

```text
CPU
  ↓
Core
  ↓
Cache
  ↓
NUMA Node
  ↓
Memory Domain
```

sowie:

```text
GPU
  ↓
PCIe Root Complex
  ↓
NUMA Node
```

oder:

```text
Device
  ↓
IOMMU Domain
  ↓
Interrupt Controller
```

Für Scheduling, Ressourcenplanung, DMA, Zero-Copy, Lokalität, Power Management und Fehlerisolation reicht deshalb eine einfache Geräteliste nicht aus.

NovaOS benötigt ein einheitliches Modell, das Hardwareobjekte und ihre Beziehungen als Graph beschreibt.

---

## Entscheidung

NovaOS modelliert die erkannte Hardwaretopologie als **versionierten Systemgraphen**.

Das Grundmodell lautet:

```text
Hardware Sources
      │
      ▼
Platform Provider
      │
      ▼
Topology Discovery
      │
      ▼
Hardware System Graph
      │
      ├── Scheduler
      ├── Resource Economy
      ├── Execution Planner
      ├── Driver Infrastructure
      ├── Power Management
      └── Architecture Introspection
```

Der Graph beschreibt sowohl Hardwareknoten als auch semantisch typisierte Beziehungen zwischen ihnen.

---

## Graphmodell

Der Hardwaregraph besteht aus:

```text
Nodes
+
Edges
+
Properties
```

Beispiel:

```text
CPU0
  │ member-of
  ▼
Core0
  │ member-of
  ▼
Package0
```

und:

```text
CPU0
  │ local-to
  ▼
NUMA0

GPU0
  │ attached-to
  ▼
PCIeRoot0

GPU0
  │ nearest-to
  ▼
NUMA0
```

---

## Knotentypen

Der Graph muss mindestens folgende Hardwareklassen ausdrücken können:

```text
System
Package
CPU
Core
HardwareThread
Cache
NUMANode
MemoryDomain
MemoryRegion
Bus
Bridge
RootComplex
Device
Accelerator
InterruptController
IOMMU
Timer
Clock
PowerDomain
ThermalZone
FirmwareNode
```

Weitere Knotentypen dürfen ergänzt werden.

---

## Beziehungen

Beziehungen müssen explizit typisiert sein.

Beispiele:

```text
contains
member-of
attached-to
connected-to
local-to
shares-cache-with
uses-interrupt-controller
protected-by-iommu
powered-by
cooled-by
nearest-to
depends-on
```

Semantisch unterschiedliche Beziehungen dürfen nicht auf eine einzige generische Kante reduziert werden.

---

## Hierarchie und Graph

Ein Teil der Hardware lässt sich hierarchisch darstellen.

Beispiel:

```text
System
└── Package
    └── Core
        └── HardwareThread
```

Andere Beziehungen sind jedoch nicht hierarchisch.

Beispiel:

```text
GPU
 ├── connected-to → PCIe Root
 ├── local-to → NUMA Node
 ├── protected-by → IOMMU
 └── uses → Interrupt Controller
```

Daher ist das normative Modell ein Graph und kein reiner Baum.

---

## Stabile Identität

Jeder relevante Hardwareknoten erhält eine stabile Identität für die Dauer seiner Existenz im laufenden System.

Beispiel:

```text
HardwareNodeId
```

Darstellungsnamen wie:

```text
CPU 0
GPU 1
```

dürfen nicht allein als dauerhafte Identität verwendet werden.

---

## Eigenschaften

Knoten können typisierte Eigenschaften besitzen.

Beispiel:

```text
CPU {
    architecture
    vendor
    model
    features
    performanceClass
    numaNode
}
```

oder:

```text
Cache {
    level
    size
    lineSize
    sharing
}
```

oder:

```text
BusLink {
    bandwidth
    latency
    width
    generation
}
```

---

## Herkunft der Daten

Der Hardwaregraph kann aus mehreren Quellen aufgebaut werden.

Beispiele:

```text
UEFI
ACPI
Device Tree
CPUID
PCIe Enumeration
USB Enumeration
IOMMU Discovery
Runtime Device Discovery
```

Diese Quellen werden durch HAL und Platform Provider normalisiert.

---

## Konfliktbehandlung

Mehrere Quellen können widersprüchliche Informationen liefern.

Der Topology Builder muss deshalb:

- Quelle identifizieren,
- Validität prüfen,
- Prioritätsregeln anwenden,
- Konflikte markieren,
- unsichere Informationen explizit kennzeichnen.

Ein Konflikt darf nicht stillschweigend verschwinden.

---

## Topologiezustände

Eigenschaften können unterschiedliche Vertrauens- oder Erkenntniszustände besitzen.

Beispiel:

```text
Known
Derived
Estimated
Unknown
Conflicting
Invalid
```

Damit kann der Planner zwischen sicher bekannten und nur geschätzten Informationen unterscheiden.

---

## CPU-Topologie

Der Graph beschreibt mindestens:

```text
Package
Core
HardwareThread
```

und deren Beziehungen.

Beispiel:

```text
Package0
├── Core0
│   ├── CPU0
│   └── CPU1
└── Core1
    ├── CPU2
    └── CPU3
```

SMT wird dadurch als Topologieeigenschaft sichtbar und nicht als Sonderfall des Schedulers.

---

## Cache-Topologie

Caches werden als eigene Objekte modelliert.

Beispiel:

```text
CPU0 ─┐
      ├── shares → L1i
CPU1 ─┘
```

oder:

```text
Core0 ─── uses → L2
Core1 ─── uses → L2
```

Dadurch kann der Scheduler Cache-Sharing berücksichtigen.

---

## NUMA

NUMA-Nodes werden explizit dargestellt.

Beispiel:

```text
CPU0 ─ local-to ─ NUMA0
CPU1 ─ local-to ─ NUMA0
GPU0 ─ nearest-to ─ NUMA0

Memory0 ─ belongs-to ─ NUMA0
```

Entfernung oder Kosten zwischen NUMA-Nodes dürfen als gewichtete Beziehungen modelliert werden.

---

## Memory Domains

Speicher wird nicht ausschließlich als globale RAM-Menge betrachtet.

Mögliche Memory Domains:

```text
System RAM
NUMA Memory
Persistent Memory
GPU VRAM
NPU Memory
Device Memory
Shared Coherent Memory
```

Der Hardwaregraph beschreibt ihre Beziehungen zu Compute Units und Devices.

---

## Geräte- und Bustopologie

Busse und Bridges werden explizit modelliert.

Beispiel:

```text
CPU Package
    ↓
PCIe Root Complex
    ↓
PCIe Switch
    ├── GPU
    └── NVMe
```

Dadurch können gemeinsame Bandbreite und Engpässe erkannt werden.

---

## Accelerators

Accelerators werden als Compute Nodes betrachtet.

Beispiele:

```text
GPU
NPU
DSP
FPGA
AI Accelerator
```

Sie besitzen Beziehungen zu:

- Speicher,
- Bus,
- NUMA Node,
- Power Domain,
- IOMMU,
- Treibern.

---

## IOMMU-Topologie

IOMMU-Zuordnungen werden im Graph sichtbar.

Beispiel:

```text
GPU0
  │ protected-by
  ▼
IOMMU0
```

oder:

```text
DeviceGroup7
  │ shares-domain
  ▼
IOMMU Domain 12
```

Diese Informationen sind sicherheitsrelevant.

---

## Interrupttopologie

Interrupt Controller und Routing dürfen im Graph dargestellt werden.

Beispiel:

```text
Device
  ↓
MSI-X
  ↓
Interrupt Controller
  ↓
CPU Set
```

Dies unterstützt Routing und Affinitätsentscheidungen.

---

## Power Domains

Hardwarekomponenten können gemeinsamen Power Domains angehören.

Beispiel:

```text
CPU Cluster
    ↓
PowerDomain0
```

oder:

```text
GPU
    ↓
PowerDomain3
```

Power Management kann diese Beziehungen verwenden.

---

## Thermal Topology

Thermal Zones können ebenfalls Teil des Graphen sein.

Beispiel:

```text
CPU Package
  │ monitored-by
  ▼
ThermalZone0
```

Dadurch können Resource Economy und Scheduler thermische Zusammenhänge berücksichtigen.

---

## Gewichtete Kanten

Einige Beziehungen besitzen Kosten.

Beispiel:

```text
NUMA0
  │ latency=80ns
  ▼
NUMA1
```

oder:

```text
GPU0
  │ bandwidth=32GB/s
  ▼
CPU Memory
```

Mögliche Metriken:

```text
Latency
Bandwidth
EnergyCost
HopCount
TransferCost
```

Nicht jede Plattform muss alle Werte exakt bereitstellen.

---

## Lokalität

Diese ADR bildet die strukturelle Grundlage für `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`.

Der Planner kann aus dem Graphen ableiten:

```text
Where is the data?
Where is the compute?
What is between them?
What does movement cost?
```

---

## Resource Economy

Die systemweite Ressourcenökonomie nutzt den Graphen, um Ressourcen nicht isoliert zu betrachten.

Beispiel:

```text
GPU0 available
```

reicht nicht aus.

Zusätzlich relevant:

```text
GPU0
├── local memory
├── PCIe bandwidth
├── power domain
├── thermal state
└── NUMA relation
```

---

## Scheduler

Der Scheduler kann Topologieinformationen verwenden für:

- CPU Affinity,
- Cache Locality,
- SMT Placement,
- NUMA Placement,
- Performance-/Efficiency-Core-Auswahl,
- Interrupt Affinity.

Diese Informationen sind Inputs für Policy, keine direkten Schedulingbefehle.

---

## Execution Planner

Der Execution Planner kann Provider und Ausführungsorte anhand des Hardwaregraphen bewerten.

Beispiel:

```text
Data in GPU0 VRAM
      +
Provider on GPU0
      ↓
High locality
```

gegen:

```text
Provider on GPU1
      ↓
PCIe transfer required
```

---

## Zero-Copy

Der Graph kann bestimmen helfen, ob physisch gemeinsamer oder direkt zugänglicher Speicher existiert.

Dadurch können mögliche Zero-Copy-Pfade erkannt werden.

Zero-Copy bleibt jedoch von Ownership-, Security- und Coherency-Regeln abhängig.

---

## Hotplug

Der Hardwaregraph ist dynamisch.

Ein Gerät kann:

```text
Added
Removed
Offline
Online
Failed
```

werden.

Graphänderungen müssen versioniert oder ereignisbasiert veröffentlicht werden.

---

## Graph-Updates

Topologieänderungen sollen transaktional sichtbar werden.

Nicht:

```text
half-updated topology
```

sondern:

```text
Old Graph
    ↓
Validated Update
    ↓
New Graph Generation
```

Leser können dadurch mit konsistenten Snapshots arbeiten.

---

## Generationen

Der Graph besitzt eine Generation oder Version.

Beispiel:

```text
TopologyGeneration = 481
```

Planner können Entscheidungen gegen eine bestimmte Generation treffen.

Ändert sich die Topologie relevant, kann Replanning erforderlich sein.

---

## Snapshots

Systemkomponenten dürfen stabile Topology Snapshots beziehen.

Beispiel:

```text
TopologySnapshot {
    generation
    nodes
    edges
}
```

Ein Snapshot bleibt für seine Lebensdauer konsistent.

---

## Events

Änderungen werden als strukturierte Events veröffentlicht.

Beispiele:

```text
NodeAdded
NodeRemoved
NodeChanged
EdgeAdded
EdgeRemoved
TopologyInvalidated
```

---

## Fail-Safe Defaults

Unbekannte Topologie darf nicht als optimale oder sichere Topologie interpretiert werden.

Beispiel:

```text
Unknown NUMA relation
    ↓
Do not assume local access
```

oder:

```text
Unknown IOMMU relation
    ↓
Do not assume DMA isolation
```

---

## Security

Nicht jede Topologieinformation darf jedem Prozess vollständig sichtbar sein.

Detaillierte Hardwareinformationen können:

- Fingerprinting ermöglichen,
- Side-Channel-Informationen liefern,
- Sicherheitsgrenzen offenlegen.

Architecture Introspection muss deshalb Capability- und Policy-basierte Filterung unterstützen.

---

## Architecture Introspection

Der Hardwaregraph bildet einen Teil des systemweiten Architecture Graph.

Beispiel:

```text
System
├── NUMA0
│   ├── CPU0
│   ├── CPU1
│   ├── Memory0
│   └── GPU0
└── NUMA1
    ├── CPU2
    ├── CPU3
    └── Memory1
```

Entwicklungswerkzeuge dürfen den Graph visualisieren.

---

## Hardwaregraph und Systemgraph

Der Hardwaregraph ist nicht notwendigerweise der gesamte Architecture Graph.

Er stellt einen spezialisierten Teilgraph dar:

```text
Architecture Graph
    ├── Hardware Graph
    ├── Capability Graph
    ├── Execution Graph
    ├── Resource Graph
    └── Trust Graph
```

Diese Graphen können über stabile Identitäten miteinander verknüpft werden.

---

## Keine Hardwarepolicy im Graph

Der Graph beschreibt:

```text
what exists
```

und:

```text
how it is related
```

nicht:

```text
what must be selected
```

Beispiel:

```text
CPU0 shares L3 with CPU1
```

ist Topologie.

```text
schedule Task X on CPU0
```

ist Policy/Execution Planning.

---

## Evolvierbarkeit

Neue Hardwaretypen und Beziehungstypen müssen ergänzt werden können.

Unbekannte Knotentypen dürfen ältere Komponenten nicht unnötig brechen.

Der Graph muss deshalb versionierbar und erweiterbar sein.

---

## KI-Unabhängigkeit

Der Hardwaregraph muss deterministisch aus verfügbaren Hardware- und Firmwareinformationen aufgebaut werden können.

KI darf:

- Kostenmodelle verbessern,
- Anomalien erkennen,
- Topologieprobleme analysieren.

Der korrekte Graphaufbau darf keine KI benötigen.

---

## Normative Anforderungen

1. NovaOS MUSS Hardwaretopologie als Systemgraph modellieren.
2. Der Graph MUSS Knoten und typisierte Beziehungen unterstützen.
3. Der Graph DARF NICHT auf eine reine Baumstruktur beschränkt sein.
4. Hardwareknoten MÜSSEN stabile Laufzeitidentitäten besitzen.
5. CPU-Packages MÜSSEN modellierbar sein.
6. CPU-Cores MÜSSEN modellierbar sein.
7. Hardwarethreads MÜSSEN modellierbar sein.
8. Cache-Topologie SOLL modellierbar sein.
9. NUMA-Nodes MÜSSEN modellierbar sein, wenn die Plattform NUMA unterstützt.
10. Memory Domains MÜSSEN modellierbar sein.
11. Busse und Bridges MÜSSEN modellierbar sein.
12. Devices MÜSSEN modellierbar sein.
13. Accelerators MÜSSEN modellierbar sein.
14. IOMMU-Beziehungen SOLLEN modellierbar sein.
15. Interrupttopologie SOLL modellierbar sein.
16. Power Domains SOLLEN modellierbar sein.
17. Thermal Zones SOLLEN modellierbar sein.
18. Beziehungen MÜSSEN semantisch typisiert sein.
19. Topologiebeziehungen DÜRFEN gewichtete Kosten besitzen.
20. Latency SOLL als Kantenmetrik unterstützt werden können.
21. Bandwidth SOLL als Kantenmetrik unterstützt werden können.
22. Der Graph MUSS aus mehreren Hardwarequellen aufgebaut werden können.
23. Die Herkunft relevanter Topologieinformationen SOLL nachvollziehbar sein.
24. Widersprüchliche Quellen MÜSSEN explizit behandelt werden.
25. Unsichere Informationen MÜSSEN als solche kennzeichnbar sein.
26. Der Scheduler MUSS Topologieinformationen beziehen können.
27. Resource Economy MUSS Topologieinformationen beziehen können.
28. Execution Planning MUSS Topologieinformationen beziehen können.
29. Lokalitätsoptimierung MUSS den Hardwaregraphen verwenden können.
30. Zero-Copy-Planung DARF Topologieinformationen verwenden.
31. Hardware-Hotplug MUSS Graphänderungen auslösen können.
32. Graphänderungen SOLLEN atomar oder generationenbasiert sichtbar werden.
33. Der Hardwaregraph MUSS eine Generation besitzen.
34. Konsistente Topology Snapshots MÜSSEN möglich sein.
35. Strukturierte Topologieevents SOLLEN verfügbar sein.
36. Unbekannte Lokalität DARF NICHT automatisch als lokal gelten.
37. Unbekannte IOMMU-Zuordnung DARF NICHT automatisch als isoliert gelten.
38. Hardwaretopologie MUSS introspektierbar sein.
39. Introspection MUSS sicherheitsabhängig filterbar sein.
40. Der Hardwaregraph SOLL in den systemweiten Architecture Graph integrierbar sein.
41. Hardwaregraph und Execution Policy MÜSSEN getrennt bleiben.
42. Der Graph MUSS neue Knotentypen aufnehmen können.
43. Der Graph MUSS neue Beziehungstypen aufnehmen können.
44. Der Hardwaregraph MUSS ohne KI korrekt aufgebaut werden können.
45. NovaOS MUSS Hardware als relationale Topologie und nicht nur als Geräteliste behandeln.

---

## Konsequenzen

### Positive Konsequenzen

- Scheduler erhält einheitliche Topologieinformationen.
- NUMA- und Cache-Lokalität werden explizit.
- GPUs und NPUs können räumlich korrekt eingeordnet werden.
- Resource Economy erhält ein realistisches Hardwaremodell.
- Zero-Copy- und Datenbewegungskosten lassen sich besser planen.
- IOMMU- und Interruptbeziehungen werden sichtbar.
- Hotplug kann strukturiert behandelt werden.
- Hardwarearchitektur wird introspektierbar.
- zukünftige heterogene Systeme lassen sich besser modellieren.

### Negative Konsequenzen

- Aufbau und Pflege des Graphen erzeugen zusätzliche Komplexität.
- Firmware- und Runtime-Informationen müssen zusammengeführt werden.
- dynamische Änderungen benötigen Snapshot- und Generationslogik.
- Hardwareinformationen können sicherheitsrelevant sein und benötigen Zugriffskontrolle.

---

## Verworfene Alternativen

### Flache Geräteliste

Verworfen.

Sie kann NUMA-, Cache-, Bus-, IOMMU- und Lokalitätsbeziehungen nicht ausreichend ausdrücken.

### Reiner Hardwarebaum

Verworfen.

Viele relevante Beziehungen sind nicht hierarchisch.

### Separates proprietäres Topologiemodell je Subsystem

Verworfen.

Scheduler, Resource Manager und Treibersystem würden inkonsistente Hardwarebilder entwickeln.

### Topologie ausschließlich aus Firmware

Verworfen.

Runtime Discovery und Bus Enumeration liefern zusätzliche Informationen.

### Topologie direkt als Scheduling Policy

Verworfen.

Hardwarebeschreibung und Policy müssen getrennt bleiben.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-HAL-0001_Einheitliche_Hardware_Abstraction_Layer`
- `ADR-HAL-0002_Plattformabstraktion_für_x86_ARM_und_zukünftige_Architekturen`
- `ADR-HAL-0003_Firmware_über_Platform_Provider_abstrahieren`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-ARCH-0019_Evolvierbare_Systemarchitektur`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`

---

## Zugehörige NPSPECs

- `NPSPEC-HAL-TOPOLOGY-GRAPH-0001`
- `NPSPEC-HAL-TOPOLOGY-NODE-0001`
- `NPSPEC-HAL-TOPOLOGY-EDGE-0001`
- `NPSPEC-HAL-TOPOLOGY-PROPERTY-0001`
- `NPSPEC-HAL-TOPOLOGY-GENERATION-0001`
- `NPSPEC-HAL-TOPOLOGY-SNAPSHOT-0001`
- `NPSPEC-HAL-TOPOLOGY-EVENT-0001`
- `NPSPEC-HAL-CPU-TOPOLOGY-0001`
- `NPSPEC-HAL-CACHE-TOPOLOGY-0001`
- `NPSPEC-HAL-NUMA-TOPOLOGY-0001`
- `NPSPEC-HAL-MEMORY-TOPOLOGY-0001`
- `NPSPEC-HAL-BUS-TOPOLOGY-0001`
- `NPSPEC-HAL-DEVICE-TOPOLOGY-0001`
- `NPSPEC-HAL-ACCELERATOR-TOPOLOGY-0001`
- `NPSPEC-HAL-IOMMU-TOPOLOGY-0001`
- `NPSPEC-HAL-INTERRUPT-TOPOLOGY-0001`
- `NPSPEC-HAL-POWER-TOPOLOGY-0001`
- `NPSPEC-HAL-THERMAL-TOPOLOGY-0001`
- `NPSPEC-HAL-TOPOLOGY-COST-0001`
- `NPSPEC-HAL-TOPOLOGY-INTROSPECTION-0001`

---

## Verifikation

Die Architektur muss insbesondere getestet werden mit:

- Single-Socket-System,
- Multi-Core-System,
- SMT-System,
- Multi-Socket-System,
- NUMA-System,
- geteilter und privater Cache-Topologie,
- PCIe-Root-Complex mit mehreren Geräten,
- GPU mit NUMA-Zuordnung,
- IOMMU-Gruppen,
- Interrupt Routing,
- Power Domains,
- Hotplug eines Geräts,
- Entfernung eines Geräts,
- widersprüchlichen Firmwareinformationen,
- Topology Snapshot während paralleler Graphänderung,
- Architecture Introspection des vollständigen Hardwaregraphen.

---

## Ergebnis

NovaOS behandelt Hardware nicht als flache Sammlung isolierter Geräte.

Stattdessen gilt:

```text
Hardware
    ↓
Nodes
+
Relationships
+
Topology Costs
    ↓
Hardware System Graph
    ↓
Planning / Scheduling / Resources / Introspection
```

Damit wird die reale Struktur moderner heterogener Systeme systemweit nutzbar.

Die zentrale Architekturregel lautet:

```text
Hardware is not a list.

Hardware is a topology.

Model that topology as a graph.
```