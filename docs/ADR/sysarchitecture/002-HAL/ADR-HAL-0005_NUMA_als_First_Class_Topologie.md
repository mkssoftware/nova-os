# ADR-HAL-0005 – NUMA als First-Class-Topologie

## Status

Angenommen

## Kategorie

HAL / NUMA / Hardwaretopologie / Speicherarchitektur / Scheduling / Lokalität / Resource Economy

## Kontext

Moderne Mehrsockel-, Server- und heterogene Systeme besitzen häufig keine uniforme Speicherarchitektur.

Bei Non-Uniform Memory Access (NUMA) hängt die Zugriffsleistung auf Speicher davon ab, von welcher CPU, welchem NUMA-Knoten oder welchem Gerät auf diesen Speicher zugegriffen wird.

Vereinfacht:

```text
CPU0 ── local ── Memory0
 │
 └──── remote ── Memory1
```

Ein Speicherzugriff kann daher je nach Topologie unterschiedliche:

- Latenz,
- Bandbreite,
- Energie,
- Interconnect-Belastung,
- Synchronisationskosten

verursachen.

NUMA betrifft nicht nur klassische Multi-Socket-Server.

Ähnliche Lokalitätseigenschaften entstehen zunehmend durch:

- Chiplets,
- CPU-Cluster,
- HBM,
- CXL,
- GPUs,
- NPUs,
- Accelerator Memory,
- Persistent Memory,
- disaggregierten Speicher,
- heterogene Compute-Systeme.

Eine Architektur, die Speicher weiterhin ausschließlich als:

```text
one global pool of RAM
```

betrachtet, verliert wichtige Informationen für Scheduling, Memory Placement, Resource Economy und Execution Planning.

NUMA soll deshalb in NovaOS kein optionales Spezialfeature einzelner Subsysteme sein, sondern Teil des grundlegenden Hardware- und Ressourcenmodells.

---

## Entscheidung

NovaOS behandelt NUMA als **First-Class-Topologieeigenschaft**.

NUMA-Knoten, Speicherzugehörigkeit, Compute-Zugehörigkeit, Gerätebeziehungen und Distanzinformationen werden explizit im Hardware System Graph modelliert.

Das Grundmodell lautet:

```text
Hardware Discovery
      │
      ▼
Hardware System Graph
      │
      ├── NUMA Nodes
      ├── Compute Nodes
      ├── Memory Domains
      ├── Devices
      └── Distance / Cost Relations
              │
              ▼
      Locality-aware Planning
```

NUMA ist damit ein systemweit verfügbares Planungsmerkmal.

---

## Grundprinzip

Es gilt:

```text
Memory is not equally near
to every execution resource.
```

und:

```text
NUMA locality is a property
of the system topology,
not an implementation detail
of the memory allocator.
```

---

## NUMA-Knoten

Ein NUMA-Knoten repräsentiert eine relevante Lokalitätsdomäne.

Beispiel:

```text
NUMA0
├── CPU0
├── CPU1
└── Memory0

NUMA1
├── CPU2
├── CPU3
└── Memory1
```

Ein NUMA-Knoten kann insbesondere Beziehungen besitzen zu:

- CPUs,
- Cores,
- Hardwarethreads,
- Memory Domains,
- GPUs,
- NPUs,
- PCIe Root Complexes,
- IOMMUs,
- Accelerators.

---

## NUMA im Hardwaregraph

NUMA wird gemäß `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph` direkt im Hardwaregraph dargestellt.

Beispiel:

```text
CPU0
  │ local-to
  ▼
NUMA0

Memory0
  │ belongs-to
  ▼
NUMA0

GPU0
  │ nearest-to
  ▼
NUMA0
```

Damit können unterschiedliche Systemkomponenten dieselbe Topologiewahrheit verwenden.

---

## Keine separate NUMA-Welt

NUMA darf nicht als vollständig separates Subsystem neben der allgemeinen Hardwaretopologie entstehen.

Nicht:

```text
Hardware Graph
NUMA Database
Scheduler NUMA Map
Memory NUMA Map
```

sondern:

```text
Hardware System Graph
        │
        └── NUMA Topology
```

Subsysteme dürfen optimierte Views oder Caches ableiten, die normative Topologiequelle bleibt jedoch einheitlich.

---

## NUMA-Distanzen

NUMA-Beziehungen sind nicht nur:

```text
local
```

oder:

```text
remote
```

NovaOS muss unterschiedliche Distanzen oder Kosten ausdrücken können.

Beispiel:

```text
NUMA0 → NUMA0 = 10
NUMA0 → NUMA1 = 18
NUMA0 → NUMA2 = 35
```

Die konkrete Zahl ist eine relative oder plattformspezifische Kostenmetrik und nicht zwangsläufig eine direkte Nanosekundenangabe.

---

## Asymmetrische Distanzen

Das Modell darf nicht erzwingen, dass Kosten immer symmetrisch sind.

Es muss möglich sein:

```text
Cost(A → B) != Cost(B → A)
```

zu modellieren, wenn zukünftige Hardware dies erfordert.

Damit bleibt die Architektur auch für komplexere Interconnects geeignet.

---

## Erweiterte Kosten

Neben einer einfachen Distanz dürfen weitere Eigenschaften verfügbar sein:

```text
Latency
Bandwidth
TransferCost
EnergyCost
HopCount
```

Nicht jede Plattform muss alle Metriken liefern.

Unbekannte Werte bleiben explizit unbekannt.

---

## NUMA und Speicher

Physischer Speicher muss einer NUMA- oder Memory-Domain-Zuordnung folgen können.

Beispiel:

```text
PhysicalMemoryRegion {
    base
    length
    numaNode
}
```

Der Physical Memory Manager muss diese Information erhalten können.

---

## NUMA-aware Allocation

Speicherallokation soll NUMA-Präferenzen ausdrücken können.

Beispiele:

```text
AllocateLocal()
AllocatePreferred(node)
AllocateInterleaved(nodes)
AllocateOn(node)
```

Die konkreten API-Namen werden durch nachgelagerte Spezifikationen festgelegt.

---

## Allocation Policy

Die HAL beschreibt, welche Speicherressourcen wo existieren.

Sie entscheidet nicht allgemein, wo Speicher für eine Anwendung angelegt werden soll.

Es gilt weiterhin:

```text
HAL:
    describes topology

Memory Policy:
    chooses placement
```

---

## Local Allocation

Für CPU-lokale Workloads soll Speicher bevorzugt in der Nähe des ausführenden Compute-Kontexts angelegt werden können.

Beispiel:

```text
Task on NUMA0
      ↓
Allocate memory on NUMA0
```

sofern:

- Kapazität vorhanden,
- Contract erfüllt,
- Policy erlaubt,
- keine wichtigere Systemanforderung entgegensteht.

---

## Remote Allocation

Remote Memory bleibt zulässig.

Eine lokale Allokation ist keine absolute Pflicht.

Gründe können sein:

- lokaler Speicher erschöpft,
- Ressourcenbudget,
- Interleaving,
- Shared Data,
- Hotplug,
- Datenmigration,
- explizite Policy,
- Gesamtoptimierung.

---

## Memory Interleaving

NovaOS darf Speicher über mehrere NUMA-Knoten verteilen.

Beispiel:

```text
NUMA0
NUMA1
NUMA2
NUMA3
  ↓
Interleaved Allocation
```

Dies kann für bestimmte bandbreitenorientierte Workloads sinnvoll sein.

Interleaving ist Policy und keine allgemeine Standardeigenschaft.

---

## Memory Binding

Execution Contracts oder Systempolicy dürfen eine harte Bindung verlangen.

Beispiel:

```text
MemoryNode = NUMA0
```

Ein solcher Hard Constraint darf nicht stillschweigend auf einen anderen NUMA-Knoten ausweichen.

Wenn er nicht erfüllt werden kann, muss dies explizit behandelt werden.

---

## Preferred Placement

Für Optimierungsfälle wird eine weichere Präferenz verwendet.

Beispiel:

```text
PreferredNUMANode = NUMA0
```

Ist NUMA0 nicht sinnvoll verfügbar, kann ein anderer gültiger Knoten gewählt werden.

---

## CPU Scheduling

Der Scheduler muss NUMA-Informationen berücksichtigen können.

Beispiel:

```text
Task
 │
 ├── Working Set mostly NUMA0
 │
 ▼
Prefer CPU in NUMA0
```

Ein CPU-Wechsel zwischen NUMA-Knoten kann indirekt die Speicherlokalität verschlechtern.

---

## Task Migration

Bei Task Migration muss daher bewertet werden:

```text
Move Task
```

gegen:

```text
Move Memory
```

gegen:

```text
Keep Current Placement
```

Die Entscheidung soll auf Gesamtkosten basieren.

---

## Memory Migration

NovaOS darf physische Pages zwischen NUMA-Domänen migrieren, wenn die Speicherarchitektur dies unterstützt.

Konzeptionell:

```text
Task moved to NUMA1
        ↓
Working Set remains NUMA0
        ↓
Migration beneficial?
        ↓
Move selected pages to NUMA1
```

Migration ist eine Optimierung und darf bestehende Memory-, Security- und Determinism-Contracts nicht verletzen.

---

## Data Gravity

Große oder häufig verwendete Datenmengen erzeugen Datenlokalität.

Daher gilt:

```text
Large Working Set
      ↓
Strong Data Gravity
      ↓
Prefer moving compute toward memory
```

statt automatisch große Speicherbereiche zu verschieben.

Dies folgt `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`.

---

## NUMA und Process Placement

Bei Prozessstart kann NovaOS initiale Compute- und Memory-Lokalität gemeinsam planen.

Beispiel:

```text
Process
   ↓
NUMA Placement Plan
   ├── CPU Set
   └── Memory Preference
```

Die Bindung darf sich später dynamisch verändern, sofern kein Hard Contract dies verhindert.

---

## Structured Concurrency

Execution Scopes können NUMA-Präferenzen erben.

Beispiel:

```text
Parent Scope
    PreferredNUMA = NUMA1
          │
          ├── Child Task A
          └── Child Task B
```

Untergeordnete Aufgaben dürfen davon abweichen, wenn Contracts oder Resource Planning dies erfordern.

---

## Resource Economy

NUMA-Speicher wird als lokalisierte Ressource betrachtet.

Nicht nur:

```text
FreeMemory = 64 GiB
```

sondern beispielsweise:

```text
NUMA0 FreeMemory = 8 GiB
NUMA1 FreeMemory = 24 GiB
NUMA2 FreeMemory = 32 GiB
```

Damit kann Resource Economy tatsächliche lokale Engpässe erkennen.

---

## Lokaler Ressourcendruck

Ein System kann global ausreichend freien Speicher besitzen und trotzdem lokalen Speicherdruck aufweisen.

Beispiel:

```text
System:
    40 GiB free

NUMA0:
    200 MiB free
```

NovaOS muss diesen Zustand darstellen können.

---

## NUMA und Accelerators

Accelerators können eine natürliche NUMA-Nähe besitzen.

Beispiel:

```text
GPU0
  │ nearest-to
  ▼
NUMA1
```

Wird ein GPU-Workload primär durch NUMA0 gespeist, können zusätzliche Interconnect-Kosten entstehen.

Execution Planning muss solche Beziehungen berücksichtigen können.

---

## PCIe und NUMA

Geräte werden nicht allein über ihre PCIe-Adresse betrachtet.

Relevant ist auch:

```text
Device
    ↓
PCIe Root Complex
    ↓
NUMA Node
```

Dies kann insbesondere für:

- GPU,
- NVMe,
- NIC,
- Accelerator

wichtig sein.

---

## DMA

DMA-Puffer sollten nach Möglichkeit in einer geeigneten Speicherlokalität angelegt werden.

Beispiel:

```text
NIC on NUMA1
      ↓
RX buffers preferably NUMA1
```

Dies darf jedoch DMA-, IOMMU- oder Treiberanforderungen nicht verletzen.

---

## Netzwerk

High-Performance-Networking kann NUMA-sensitive sein.

Beispiel:

```text
NIC on NUMA1
+
Network Worker on NUMA0
+
Buffers on NUMA0
```

kann zusätzliche Transfers verursachen.

Scheduler, Network Stack und Resource Economy dürfen deshalb NUMA gemeinsam berücksichtigen.

---

## Storage

Auch Storage-Pipelines können NUMA-Effekte besitzen.

Beispiel:

```text
NVMe on NUMA1
      ↓
I/O buffers
      ↓
CPU processing
```

I/O-Planung darf diese Lokalität berücksichtigen.

---

## Zero-Copy

Zero-Copy und NUMA sind unterschiedliche Dimensionen.

Ein Zero-Copy-Pfad kann trotzdem teuer sein, wenn er über eine entfernte NUMA-Verbindung läuft.

Daher gilt:

```text
Zero copies
```

ist nicht automatisch:

```text
Zero topology cost
```

NovaOS muss beide Faktoren berücksichtigen.

---

## Shared Memory

Bei gemeinsam genutztem Speicher zwischen Tasks auf mehreren NUMA-Knoten kann keine einzelne optimale Lokalität existieren.

Mögliche Strategien sind:

```text
Single-node placement
Interleaving
Replication
Read-mostly duplication
Migration
```

Die Entscheidung erfolgt durch höhere Policy- und Planning-Schichten.

---

## Replikation

Read-mostly-Daten dürfen lokal repliziert werden, sofern Semantik und Konsistenzmodell dies erlauben.

Beispiel:

```text
Read-only Dataset
       ↓
Replica NUMA0
Replica NUMA1
```

Dies kann Remote Reads reduzieren.

---

## Cache Coherency

NUMA ist von Cache-Coherency-Eigenschaften zu unterscheiden.

Ein System kann:

```text
NUMA + coherent
```

oder zukünftig:

```text
NUMA + partially coherent
```

sein.

Der Hardwaregraph muss solche Eigenschaften getrennt darstellen können.

---

## Heterogene Memory Domains

NUMA darf nicht ausschließlich als klassischer DRAM-pro-Socket-Fall modelliert werden.

Das Modell muss zukünftige Speicherklassen integrieren können:

```text
DRAM
HBM
Persistent Memory
CXL Memory
Accelerator Memory
Shared Unified Memory
```

NUMA-Beziehungen können diese Domains verbinden.

---

## CXL und disaggregierter Speicher

Technologien wie Compute Express Link können Speicher bereitstellen, der physisch nicht klassischem CPU-lokalem DRAM entspricht.

NovaOS darf deshalb nicht voraussetzen:

```text
NUMA Node == CPU Socket
```

Ein NUMA-Knoten ist eine Lokalitätsdomäne, kein Synonym für einen CPU-Sockel.

---

## Memory Tiering

NUMA und Memory Tiering sind verwandte, aber getrennte Konzepte.

Beispiel:

```text
NUMA0 DRAM
NUMA0 Persistent Memory
Remote CXL Memory
```

Placement kann sowohl:

```text
Topology
```

als auch:

```text
Memory Class
```

berücksichtigen.

---

## Realtime

Hard-Realtime-Workloads dürfen NUMA-Bindungen verwenden, wenn vorhersehbare Speicherlatenz erforderlich ist.

Beispiel:

```text
CPU Set = NUMA0
Memory = NUMA0
Migration = Forbidden
```

Diese Anforderungen werden über Contracts ausgedrückt.

---

## Deterministic Mode

Deterministic Mode kann NUMA-Platzierungen fixieren.

Beispiel:

```text
Execution:
    CPU Node = NUMA1
    Memory Node = NUMA1
```

Automatische Migration muss dann unterbleiben, sofern sie nicht explizit Teil des deterministischen Plans ist.

---

## Hotplug

NUMA-Topologie kann sich durch Hardware-Hotplug verändern.

Beispiele:

```text
CPU offline
Memory node offline
CXL memory added
```

Solche Änderungen erzeugen eine neue Hardwaregraph-Generation.

---

## Memory Node Removal

Das Entfernen eines NUMA-Memory-Nodes erfordert kontrollierte Behandlung vorhandener Speicherobjekte.

Mögliche Maßnahmen:

```text
Migrate
Drain
Reject Removal
Degrade
```

Ein Node darf nicht einfach aus dem Graph entfernt werden, solange aktive Ressourcen unkontrolliert darauf verweisen.

---

## Fail-Safe Defaults

Wenn NUMA-Informationen fehlen oder widersprüchlich sind, darf NovaOS keine unbelegten Lokalitätsannahmen treffen.

Beispiel:

```text
NUMA relation unknown
      ↓
Treat locality as unknown
```

Nicht:

```text
Assume local
```

Fundamentale Kernelkorrektheit darf niemals von korrekter NUMA-Optimierung abhängen.

---

## UMA-Systeme

UMA-Systeme werden als gültiger Spezialfall behandelt.

Ein einfaches System kann effektiv besitzen:

```text
NUMA0
├── all CPUs
└── all system memory
```

Damit müssen höhere Komponenten nicht überall separate UMA-Sonderpfade besitzen.

Die Implementierung darf intern natürlich optimieren.

---

## Systeme ohne NUMA-Information

Wenn die Plattform keine NUMA-Topologie bereitstellt und keine verlässliche Ableitung möglich ist, kann NovaOS eine konservative einheitliche Lokalitätsdomäne erzeugen.

Diese darf nicht vorgeben, mehr über die Hardware zu wissen als tatsächlich bekannt ist.

---

## Architecture Introspection

NUMA muss systemweit introspektierbar sein.

Beispiel:

```text
NUMA Node 0
├── CPU0
├── CPU1
├── Memory: 31.8 GiB
├── Free: 12.3 GiB
└── Local Devices
    ├── NVMe0
    └── GPU0
```

Zusätzlich sollen Distanzinformationen sichtbar sein.

Beispiel:

```text
       N0  N1  N2
N0     10  18  35
N1     18  10  22
N2     35  22  10
```

---

## Decision Trace

Wenn NUMA eine relevante Planungsentscheidung beeinflusst, soll dies nachvollziehbar sein.

Beispiel:

```text
Selected CPU:
    CPU4

Reason:
    dominant working set located on NUMA1

Alternative:
    CPU0

Rejected because:
    estimated remote-memory cost higher
```

---

## Performance Telemetrie

NovaOS darf NUMA-bezogene Laufzeitmetriken erfassen.

Beispiele:

```text
LocalMemoryAccess
RemoteMemoryAccess
NUMAMigration
RemoteBandwidth
NodePressure
```

Diese Telemetrie kann für Adaptive Planning verwendet werden.

---

## Adaptive Optimierung

NovaOS darf seine NUMA-Kostenmodelle anhand realer Messwerte verbessern.

Beispiel:

```text
Predicted Transfer Cost
        ↓
Observed Transfer Cost
        ↓
Prediction Error
        ↓
Cost Model Adjustment
```

Die zugrunde liegende Hardwaretopologie darf dadurch nicht probabilistisch erfunden werden.

---

## KI-Unabhängigkeit

NUMA Discovery, Speicherzuordnung und korrekte Allocation müssen ohne KI funktionieren.

KI darf:

- Placement empfehlen,
- Kosten prognostizieren,
- Working Sets analysieren,
- Migration empfehlen.

Der sichere Baseline-Pfad bleibt deterministisch.

---

## Normative Anforderungen

1. NovaOS MUSS NUMA als First-Class-Topologieeigenschaft behandeln.
2. NUMA MUSS in den Hardware System Graph integriert werden.
3. NUMA DARF NICHT als separate, widersprüchliche Topologiedatenbank modelliert werden.
4. NUMA-Knoten MÜSSEN stabile Laufzeitidentitäten besitzen.
5. CPU-Zuordnungen zu NUMA-Knoten MÜSSEN modellierbar sein.
6. Memory-Zuordnungen zu NUMA-Knoten MÜSSEN modellierbar sein.
7. Gerätebeziehungen zu NUMA-Knoten SOLLEN modellierbar sein.
8. Accelerator-Beziehungen zu NUMA-Knoten SOLLEN modellierbar sein.
9. NUMA-Distanzen MÜSSEN modellierbar sein.
10. Das Modell DARF keine zwingende Symmetrie der NUMA-Kosten voraussetzen.
11. NUMA-Kosten SOLLEN Latenz und Bandbreite ausdrücken können.
12. Unbekannte NUMA-Kosten MÜSSEN als unbekannt darstellbar sein.
13. Der Physical Memory Manager MUSS NUMA-Zugehörigkeiten physischer Speicherregionen erhalten können.
14. Speicherallokation MUSS NUMA-Präferenzen ausdrücken können.
15. Harte NUMA-Bindungen MÜSSEN von weichen Präferenzen unterscheidbar sein.
16. Nicht erfüllbare harte NUMA-Bindungen DÜRFEN NICHT stillschweigend abgeschwächt werden.
17. Remote Allocation MUSS grundsätzlich möglich bleiben.
18. Interleaved Allocation SOLL unterstützt werden können.
19. Der Scheduler MUSS NUMA-Topologie berücksichtigen können.
20. Task Migration SOLL Speicherlokalität berücksichtigen.
21. Memory Migration SOLL unterstützt werden können, wenn Hardware und Memory Model dies erlauben.
22. Resource Economy MUSS Speicherressourcen pro NUMA-Domain darstellen können.
23. Lokaler Speicherdruck MUSS von globalem Speicherdruck unterscheidbar sein.
24. Execution Planning MUSS NUMA-Lokalität bewerten können.
25. Accelerator Placement SOLL NUMA-Beziehungen berücksichtigen können.
26. DMA Allocation SOLL Gerätelokalität berücksichtigen können.
27. Netzwerk- und Storage-Subsysteme DÜRFEN NUMA-Topologie zur Optimierung verwenden.
28. Zero-Copy und NUMA-Kosten MÜSSEN als getrennte Dimensionen behandelt werden.
29. Shared-Memory-Placement MUSS mehrere NUMA-Nutzer berücksichtigen können.
30. Replikation DARF zur NUMA-Optimierung verwendet werden, wenn Semantik und Konsistenz dies erlauben.
31. NUMA DARF NICHT mit Cache Coherency gleichgesetzt werden.
32. NUMA DARF NICHT mit CPU-Sockeln gleichgesetzt werden.
33. Heterogene Memory Domains MÜSSEN in das Lokalitätsmodell integrierbar sein.
34. CXL- und zukünftige disaggregierte Speicherformen MÜSSEN modellierbar bleiben.
35. Memory Tier und NUMA Location MÜSSEN getrennte Eigenschaften bleiben.
36. Realtime Contracts DÜRFEN NUMA-Bindungen festlegen.
37. Deterministic Mode MUSS NUMA-Platzierungen fixieren können.
38. Automatische NUMA-Migration DARF deterministische Hard Contracts NICHT verletzen.
39. NUMA-Hotplug MUSS neue Topologiegraph-Generationen erzeugen können.
40. Memory Node Removal MUSS aktive Ressourcen berücksichtigen.
41. Fehlende NUMA-Informationen DÜRFEN NICHT als lokale Beziehung interpretiert werden.
42. UMA-Systeme MÜSSEN ohne unnötige Sonderbehandlung integrierbar sein.
43. NUMA-Topologie MUSS introspektierbar sein.
44. NUMA-relevante Planning Decisions SOLLEN erklärbar sein.
45. NUMA Discovery und Baseline-Management MÜSSEN ohne KI funktionieren.
46. Adaptive Kostenmodelle DÜRFEN NUMA-Optimierungen verbessern.
47. Adaptive Modelle DÜRFEN keine nicht erkannte Hardwaretopologie erfinden.
48. NUMA-Optimierung DARF grundlegende Kernelkorrektheit NICHT bestimmen.
49. NUMA-Policy MUSS von HAL-Topologiebeschreibung getrennt bleiben.
50. NovaOS MUSS Speicherlokalität als systemweit nutzbare Information behandeln.

---

## Konsequenzen

### Positive Konsequenzen

- bessere Speicherlokalität,
- geringere Remote-Memory-Latenzen,
- effizienteres Scheduling,
- bessere Skalierung auf Multi-Socket-Systemen,
- bessere GPU-/NPU-/NIC-Lokalität,
- präzisere Resource Economy,
- bessere Grundlage für Zero-Copy- und Datenbewegungsplanung,
- Unterstützung zukünftiger CXL- und heterogener Memory-Systeme,
- NUMA-Verhalten wird introspektierbar und erklärbar.

### Negative Konsequenzen

- Memory Manager und Scheduler werden komplexer,
- Task- und Memory-Migration benötigen Kostenmodelle,
- Hardwareinformationen können unvollständig oder fehlerhaft sein,
- lokale Optimierung kann mit globalem Load Balancing konkurrieren,
- NUMA-Telemetrie und Topologiepflege erzeugen zusätzlichen Aufwand.

---

## Verworfene Alternativen

### NUMA vollständig ignorieren

Verworfen.

Dies würde auf größeren Systemen unnötige Remote-Memory-Kosten verursachen.

### NUMA ausschließlich im Memory Allocator behandeln

Verworfen.

Auch Scheduler, Resource Economy, DMA, Netzwerk, Storage und Execution Planning benötigen diese Informationen.

### NUMA ausschließlich als CPU-Socket-Modell

Verworfen.

Zukünftige Systeme können komplexere Lokalitätsdomänen besitzen.

### Immer Speicher zur ausführenden CPU migrieren

Verworfen.

Bei großen Datenmengen kann Compute-to-Data deutlich günstiger sein.

### Immer Tasks beim ursprünglichen NUMA-Knoten halten

Verworfen.

Load Balancing, Ressourcenknappheit und andere Contracts können Migration erforderlich machen.

### NUMA als Hard Constraint für alle Operationen

Verworfen.

NUMA ist standardmäßig ein Optimierungsziel; nur explizite Contracts machen bestimmte Bindungen verpflichtend.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-HAL-0001_Einheitliche_Hardware_Abstraction_Layer`
- `ADR-HAL-0002_Plattformabstraktion_für_x86_ARM_und_zukünftige_Architekturen`
- `ADR-HAL-0003_Firmware_über_Platform_Provider_abstrahieren`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern`
- `ADR-ARCH-0010_Structured_Concurrency_als_systemweites_Ausführungsmodell`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts_zwischen_Systemkomponenten`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-ARCH-0019_Evolvierbare_Systemarchitektur`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`
- `ADR-EXECUTION-0001_Nova_ExecutionContract`

---

## Zugehörige NPSPECs

- `NPSPEC-HAL-NUMA-0001`
- `NPSPEC-HAL-NUMA-NODE-0001`
- `NPSPEC-HAL-NUMA-DISTANCE-0001`
- `NPSPEC-HAL-NUMA-DISCOVERY-0001`
- `NPSPEC-HAL-NUMA-TOPOLOGY-0001`
- `NPSPEC-MEMORY-NUMA-PLACEMENT-0001`
- `NPSPEC-MEMORY-NUMA-ALLOCATION-0001`
- `NPSPEC-MEMORY-NUMA-MIGRATION-0001`
- `NPSPEC-MEMORY-NUMA-INTERLEAVE-0001`
- `NPSPEC-SCHEDULER-NUMA-0001`
- `NPSPEC-RESOURCE-NUMA-0001`
- `NPSPEC-LOCALITY-NUMA-0001`
- `NPSPEC-DEVICE-NUMA-0001`
- `NPSPEC-DMA-NUMA-0001`
- `NPSPEC-HAL-NUMA-HOTPLUG-0001`
- `NPSPEC-HAL-NUMA-INTROSPECTION-0001`

---

## Verifikation

Die Umsetzung muss insbesondere folgende Szenarien prüfen:

- UMA-System mit einer Lokalitätsdomäne,
- Zwei-NUMA-Knoten-System,
- Multi-Socket-NUMA-System,
- unterschiedliche NUMA-Distanzen,
- asymmetrische Kostenmetrik,
- lokale Speicherallokation,
- bevorzugte Speicherallokation,
- harte Node-Bindung,
- Interleaved Allocation,
- lokaler Speicherdruck bei global freiem Speicher,
- Task Migration zwischen NUMA-Nodes,
- Memory Migration,
- GPU mit NUMA-Nähe,
- NIC mit NUMA-Nähe,
- NUMA-aware DMA Buffer Allocation,
- Node Hotplug,
- Memory Node Removal,
- fehlende NUMA-Informationen,
- widersprüchliche Firmwareinformationen,
- deterministisch gepinnte NUMA-Ausführung.

---

## Ergebnis

NUMA wird in NovaOS nicht als nachträgliche Performanceoptimierung behandelt.

Es ist Bestandteil des fundamentalen Hardware-, Speicher- und Lokalitätsmodells.

Das Systemmodell lautet:

```text
Compute
   │
   ├──── locality ────┐
   │                  │
Memory ─────────── NUMA Topology
   │                  │
   ├──── locality ────┤
   │                  │
Devices ──────────────┘
          │
          ▼
Hardware System Graph
          │
          ▼
Scheduling
Memory Placement
Resource Economy
Execution Planning
```

Die zentrale Architekturregel lautet:

```text
Memory capacity is global information.

Memory locality is topology information.

Both matter.
```

und:

```text
NUMA is not an allocator optimization.

NUMA is part of the system architecture.
```