# ADR-MEMORY-0001 – Hierarchische Physical Memory Architecture

## Status

Angenommen

## Kategorie

Kernel / Memory Management / Physical Memory / NUMA / Resource Economy

## Kontext

Der Physical Memory Manager (PMM) von NovaOS darf physischen Speicher nicht als homogene Menge gleichwertiger Seiten betrachten.

Moderne und zukünftige Systeme können unterschiedliche physische Speicherbereiche besitzen:

- mehrere NUMA-Nodes,
- unterschiedliche Memory Domains,
- DRAM und andere Speichertechnologien,
- unterschiedliche Zugriffslatenzen und Bandbreiten,
- firmware-reservierte Bereiche,
- DMA-beschränkte Speicherbereiche,
- persistenten oder nichtflüchtigen Speicher,
- hotpluggable Memory,
- accelerator-nahe Speicherressourcen.

Gleichzeitig benötigen verschiedene Kernel- und Execution-Domain-Komponenten unterschiedliche Allokationseigenschaften.

Eine einzige globale Free-Page-Liste würde wichtige Informationen über:

- physische Lage,
- NUMA-Zugehörigkeit,
- Speicherklasse,
- Topologie,
- Verfügbarkeit,
- Reservierung,
- Nutzungseinschränkungen

verlieren.

NovaOS benötigt deshalb eine hierarchische Architektur für die Verwaltung physischen Speichers.

---

## Entscheidung

NovaOS organisiert physischen Speicher als **hierarchische Menge explizit beschriebener Memory Domains, Memory Regions und Allocation Pools**.

Konzeptionell:

```text
Physical Memory
      ↓
Memory Domains
      ↓
Memory Regions
      ↓
Allocation Pools
      ↓
Physical Pages / Frames
```

Die Hierarchie bildet sowohl Hardwaretopologie als auch Allokationseigenschaften ab.

Der PMM stellt darüber eine gemeinsame Allocation-Schnittstelle bereit, die Anforderungen als Constraints und Präferenzen ausdrückt.

---

## Grundprinzip

```text
Physical memory is not uniform.

Preserve topology and memory properties.

Allocate from the best valid region.

Never hide hard placement constraints.
```

---

## Memory Domain

Eine Memory Domain beschreibt eine logisch zusammengehörige physische Speicherressource.

Sie kann beispielsweise einem:

- NUMA-Node,
- SoC-Memory-Cluster,
- accelerator-nahen Speicherbereich,
- persistenten Speicherbereich,
- speziellen Hardware-Memory-Domain

entsprechen.

Memory Domains werden mit dem Hardware System Graph verknüpft.

```text
Hardware System Graph
        ↓
Memory Domain
        ↓
Memory Regions
```

Damit können Scheduler, PMM, DMA-System und Resource Economy dieselbe Topologie verwenden.

---

## Memory Region

Eine Memory Region beschreibt einen zusammenhängenden physischen Adressbereich mit gemeinsamen Eigenschaften.

Beispiel:

```text
Memory Region
    Base:       0x100000000
    Size:       8 GiB
    Domain:     NUMA-1
    Type:       Normal
    State:      Online
```

Regionen können zusätzliche Eigenschaften besitzen, beispielsweise:

- allocatable,
- reserved,
- firmware-owned,
- persistent,
- hotpluggable,
- DMA-reachable,
- device-owned.

Die genaue Metadatenstruktur wird in den NPSPECs definiert.

---

## Allocation Pools

Innerhalb geeigneter Regionen dürfen Allocation Pools für unterschiedliche Anforderungen gebildet werden.

Beispiele:

```text
Normal Memory
DMA-compatible Memory
Realtime Reserved Memory
Kernel Emergency Memory
Contiguous Memory
Special-purpose Memory
```

Pools sind keine eigenständige Hardwaretopologie.

Sie stellen eine Allokations- und Ressourcenpolicy über physischem Speicher dar.

---

## Allocation Constraints

Eine physische Speicheranforderung kann Hard Constraints enthalten.

Beispiele:

```text
Required NUMA Domain
Required Address Range
Required DMA Reachability
Required Contiguity
Required Memory Type
Required Alignment
Required Reservation
```

Diese Anforderungen müssen vor Optimierungszielen ausgewertet werden.

Damit folgt der PMM demselben Architekturprinzip wie der Scheduler:

```text
Allocation Request
        ↓
Hard Constraints
        ↓
Eligible Memory
        ↓
Soft Preferences
        ↓
Optimization
        ↓
Physical Allocation
```

---

## Allocation Preferences

Zusätzlich können Soft Preferences angegeben werden.

Beispiele:

- lokale NUMA-Domain,
- geringe Zugriffslatenz,
- hohe Bandbreite,
- Nähe zu einem Device,
- bevorzugte Memory Class,
- Energieeffizienz,
- geringe Fragmentierung.

Kann eine Präferenz nicht erfüllt werden, darf auf anderen zulässigen Speicher ausgewichen werden.

Hard Constraints dürfen dabei nicht verletzt werden.

---

## NUMA

NUMA ist ein First-Class-Aspekt der Physical Memory Architecture.

Jede NUMA-relevante Memory Region muss ihrer Memory Domain beziehungsweise ihrem NUMA-Kontext zugeordnet werden können.

Der PMM stellt Informationen bereit, damit Speicher möglichst in der Nähe der ausführenden oder vorgesehenen CPUs allokiert werden kann.

```text
Task / Execution Scope
        ↓
Preferred NUMA Domain
        ↓
Local Physical Memory
```

CPU Placement und Memory Placement werden dabei zwischen Scheduler und Memory Manager koordiniert.

---

## Memory Locality

Locality ist ein Optimierungsziel, solange kein stärkerer Constraint entgegensteht.

Beispielsweise:

```text
Required DMA Domain
        >
Preferred NUMA Locality
```

oder:

```text
Realtime Reservation
        >
General Locality Preference
```

Der PMM darf lokale Speicherallokation nicht erzwingen, wenn sie einen Hard Constraint verletzen würde.

---

## Speicherklassen

Die Architektur muss unterschiedliche Memory Classes unterstützen können.

Beispiele können sein:

```text
Volatile DRAM
High-Bandwidth Memory
Persistent Memory
Device-attached Memory
Future Memory Technology
```

NovaOS schreibt keine feste Liste zukünftiger Speichertechnologien fest.

Stattdessen werden Eigenschaften und Fähigkeiten beschrieben.

Dadurch bleibt die Architektur erweiterbar.

---

## Heterogeneous Memory

Unterschiedliche Speicherarten können sich hinsichtlich:

- Latenz,
- Bandbreite,
- Persistenz,
- CPU-Zugänglichkeit,
- Device-Zugänglichkeit,
- Energieverbrauch,
- Zuverlässigkeit

unterscheiden.

Diese Eigenschaften dürfen bei Placement- und Resource-Policies berücksichtigt werden.

Eine Anwendung soll jedoch normalerweise keine konkrete Hardwaretechnologie voraussetzen müssen.

---

## DMA und IOMMU

DMA-Anforderungen werden mit der Physical Memory Architecture integriert.

Ein Device kann beispielsweise nur Speicher verwenden, der:

- physisch erreichbar,
- korrekt ausgerichtet,
- IOMMU-mappbar,
- für DMA freigegeben

ist.

Der PMM stellt geeignete physische Ressourcen bereit, während DMA/IOMMU-Subsysteme deren kontrollierte Device-Zugänglichkeit verwalten.

---

## Physische Kontiguität

NovaOS darf Anforderungen an physisch zusammenhängenden Speicher unterstützen.

Solche Allokationen sind jedoch teurer und fragmentierungsanfälliger als gewöhnliche Page-Allokationen.

Physische Kontiguität muss deshalb explizit angefordert werden.

Sie darf nicht unnötig vorausgesetzt werden.

---

## Reservierter Speicher

Bestimmte Speicherbereiche können bereits während Boot und Hardware Discovery reserviert sein.

Beispiele:

- Kernel Image,
- Bootstrukturen,
- Firmwarebereiche,
- MMIO,
- Crash-/Recovery-Speicher,
- Realtime Reservations,
- Hardware-spezifische Regionen.

Reservierter Speicher darf nicht in allgemeine Allocation Pools gelangen, solange seine Reservation besteht.

---

## Realtime Memory

Realtime Concurrency Domains können physische Speicherressourcen reservieren.

Damit können kritische Workloads vor:

- allgemeinem Memory Pressure,
- unvorhersehbarer Reclamation,
- konkurrierenden Allokationen

geschützt werden.

Eine Realtime-Reservation muss der Resource Economy explizit bekannt sein.

---

## Resource Economy

Physischer Speicher ist eine systemweite Ressource.

Allokationen müssen deshalb hierarchisch zurechenbar sein:

```text
System
  ↓
Execution Domain
  ↓
Execution Scope
  ↓
Allocation
```

Die Resource Economy kann darauf:

- Budgets,
- Reservations,
- Limits,
- Pressure Policies

anwenden.

Physische Topologie und Ressourcen-Ownership bleiben dabei getrennte Dimensionen.

---

## Memory Pressure

Memory Pressure kann lokal oder global auftreten.

Beispiel:

```text
NUMA Node 0:
    high pressure

NUMA Node 1:
    free capacity
```

NovaOS muss deshalb Memory Pressure pro relevanter Memory Domain beziehungsweise Region erfassen können.

Eine freie Speichermenge auf einer entfernten Domain bedeutet nicht automatisch, dass eine lokale Hard-Constraint-Anforderung erfüllbar ist.

---

## Fragmentierung

Der PMM muss Fragmentierung als Eigenschaft einzelner Regionen beziehungsweise Pools behandeln können.

Eine große Gesamtmenge freien Speichers garantiert nicht, dass eine große physisch zusammenhängende Allokation möglich ist.

Fragmentierungsinformationen sollen für:

- Allocation Policy,
- Compaction,
- Diagnostics

verwendbar sein.

---

## Hotplug

Memory Domains und Regions müssen Lifecycle-Zustände besitzen.

Konzeptionell:

```text
Discovered
    ↓
Online
    ↓
Draining
    ↓
Offline
```

Vor dem Entfernen einer Memory Region müssen verschiebbare Allokationen migriert und nicht verschiebbare Allokationen erkannt werden.

Eine Region darf erst offline gehen, wenn keine unzulässigen aktiven Referenzen verbleiben.

---

## Bootstrapping

Während früher Bootphasen steht möglicherweise noch nicht die vollständige Physical Memory Architecture zur Verfügung.

NovaOS darf deshalb einen einfachen Bootstrap Allocator verwenden.

Nach Initialisierung des regulären PMM werden verbleibende geeignete Ressourcen in die normale Hierarchie überführt.

Der Bootstrap Allocator ist kein dauerhaftes alternatives Memory-Management-Modell.

---

## Zero-Copy

Zero-Copy-Operationen können besondere Anforderungen an physischen Speicher besitzen.

Beispielsweise kann ein Buffer zwischen:

```text
CPU
 ↓
Device
 ↓
Accelerator
```

geteilt werden.

Die Physical Memory Architecture muss deshalb Placement-, DMA- und Ownership-Anforderungen ausdrücken können, ohne unnötige Kopien zu erzwingen.

---

## Security und Data Sovereignty

Nicht jede physische Speicherressource muss für jeden Execution Context geeignet sein.

Security-, Trust- oder Data-Sovereignty-Policies können die zulässigen Memory Domains einschränken.

Diese Einschränkungen sind Hard Constraints und werden vor Performance- oder Locality-Optimierung angewendet.

---

## Fail-Safe Verhalten

Kann der PMM keine Region finden, die sämtliche Hard Constraints erfüllt, muss die Allokation kontrolliert fehlschlagen oder einer explizit definierten Degradationspolicy folgen.

Nicht zulässig ist:

```text
Required Memory Property unavailable
        ↓
silently allocate incompatible memory
```

Es gilt:

```text
No valid memory
      ↓
Allocation failure
```

sofern kein expliziter Fallback erlaubt ist.

---

## Architecture Introspection

Die Physical Memory Architecture muss introspektierbar sein.

Mindestens sichtbar sein sollen:

```text
Memory Domains
Memory Regions
Memory Classes
Online / Offline State
Total Capacity
Free Capacity
Reserved Capacity
Allocation Pools
NUMA Association
Fragmentation
Memory Pressure
```

Damit können Kernel-Diagnostics und Systemwerkzeuge nachvollziehen, wo physischer Speicher verfügbar und wie er verwendet wird.

---

## Normative Anforderungen

1. NovaOS MUSS physischen Speicher hierarchisch verwalten.
2. Die Physical Memory Architecture MUSS Memory Domains, Memory Regions und Allocation Pools unterscheiden können.
3. Memory Domains MÜSSEN mit dem Hardware System Graph verknüpfbar sein.
4. Memory Regions MÜSSEN physische Adressbereiche und deren Eigenschaften beschreiben.
5. Nicht allokierbare und reservierte Regionen DÜRFEN NICHT in allgemeine Allocation Pools gelangen.
6. NUMA-Zugehörigkeit MUSS als First-Class-Eigenschaft physischen Speichers behandelt werden.
7. Unterschiedliche Memory Classes MÜSSEN erweiterbar beschreibbar sein.
8. Allocation Requests MÜSSEN Hard Constraints und Soft Preferences unterscheiden können.
9. Hard Constraints MÜSSEN vor Locality- und Performanceoptimierung ausgewertet werden.
10. Ein Hard Constraint DARF NICHT still zugunsten einer erfolgreichen Allokation verletzt werden.
11. Der PMM MUSS physische Kontiguität explizit anfordern können.
12. DMA-relevante Memory Constraints MÜSSEN ausdrückbar sein.
13. PMM und IOMMU-/DMA-Subsystem MÜSSEN koordinierbar sein.
14. Physische Speicherallokationen MÜSSEN der Resource Economy zurechenbar sein.
15. Memory Budgets und Reservations MÜSSEN unterstützt werden können.
16. Realtime Memory Reservations MÜSSEN vor allgemeiner Allokation geschützt werden können.
17. Memory Pressure MUSS pro relevanter Memory Domain erfassbar sein.
18. Fragmentierung MUSS diagnostizierbar sein.
19. Memory Hotplug MUSS über kontrollierte Lifecycle-Zustände erfolgen.
20. Eine Memory Region DARF nicht offline genommen werden, solange unzulässige aktive Allokationen verbleiben.
21. Ein Bootstrap Allocator DARF für frühe Bootphasen verwendet werden.
22. Der reguläre PMM MUSS den Bootstrap Allocator nach Initialisierung ablösen.
23. Zero-Copy-Anforderungen MÜSSEN mit Physical-Memory-Placement koordinierbar sein.
24. Security- und Data-Sovereignty-Constraints MÜSSEN Memory Placement einschränken können.
25. Nicht erfüllbare Hard Constraints MÜSSEN zu einem definierten Fehler oder explizit erlaubten Fallback führen.
26. Memory Domains, Regions, Pools, Capacity, Pressure und Reservations MÜSSEN introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- NUMA und heterogener Speicher werden von Beginn an korrekt modelliert,
- physischer Speicher bleibt topology-aware,
- DMA-, Realtime- und Zero-Copy-Anforderungen können sauber integriert werden,
- Memory Pressure und Fragmentierung können lokal betrachtet werden,
- zukünftige Speichertechnologien können ohne grundlegenden Architekturwechsel ergänzt werden,
- Resource Economy erhält eine klare Grundlage für physische Speicherressourcen.

### Negative Konsequenzen

- der PMM wird komplexer als ein globaler Page Allocator,
- Allocation Policies müssen mehrere Memory Domains und Eigenschaften berücksichtigen,
- Hotplug und heterogene Speicherklassen erhöhen den Verwaltungsaufwand,
- Scheduler, Memory Manager, HAL und Resource Economy benötigen gemeinsame Topologieinformationen.

---

## Verworfene Alternativen

### Eine globale Physical-Page-Liste

Verworfen.

Sie würde NUMA, Memory Classes, Reservations und andere Placement-Eigenschaften unzureichend modellieren.

### NUMA erst als spätere Optimierung ergänzen

Verworfen.

NUMA beeinflusst grundlegende Speicherplatzierung und muss deshalb Bestandteil der Physical Memory Architecture sein.

### Feste Speicherklassen im Kernel hardcodieren

Verworfen.

Zukünftige Hardware kann neue Speicherarten und Eigenschaften einführen.

### Separate Physical Memory Manager pro Subsystem

Verworfen.

Dies würde Ressourcenverwaltung, Ownership, Memory Pressure und Topologie fragmentieren.

### Automatisches Ignorieren nicht erfüllbarer Placement Constraints

Verworfen.

Dies würde Realtime-, DMA-, Security- oder Data-Sovereignty-Anforderungen verletzen können.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0018_Locality_als_Optimierungsziel`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-HAL-0007_Hotplug_über_deklarative_Device_Lifecycle_States`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-SCHED-0003_Topology_Aware_Scheduling`
- `ADR-SCHED-0004_NUMA_Aware_Scheduling`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-DOMAIN-0001`
- `NPSPEC-MEMORY-REGION-0001`
- `NPSPEC-MEMORY-PHYSICAL-ALLOC-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-MEMORY-RESERVATION-0001`
- `NPSPEC-MEMORY-HOTPLUG-0001`
- `NPSPEC-MEMORY-PMM-TEST-0001`

---

## Ergebnis

NovaOS behandelt physischen Speicher nicht als homogenen globalen Pool, sondern als hierarchische, topology-aware Systemressource:

```text
Hardware System Graph
          ↓
     Memory Domains
          ↓
     Memory Regions
          ↓
     Allocation Pools
          ↓
Physical Frames / Pages
```

Allokationen werden zunächst anhand verbindlicher Constraints auf zulässige Speicherressourcen eingeschränkt und erst anschließend anhand von Locality, Performance und anderen Policies optimiert.

Die zentrale Architekturregel lautet:

```text
Preserve physical topology.

Model memory properties explicitly.

Satisfy constraints before optimizing placement.

Treat physical memory as a system-wide resource.
```