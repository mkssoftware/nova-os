# ADR-MEMORY-0009 – Huge Pages policygesteuert

## Status

Angenommen

## Kategorie

Kernel / Memory Management / Virtual Memory / Huge Pages / Performance Policy

## Kontext

Moderne Prozessoren unterstützen neben normalen Pages häufig größere Seitengrößen.

Beispielhaft:

```text
4 KiB
2 MiB
1 GiB
```

Größere Pages können:

- TLB-Druck reduzieren,
- Page-Table-Größe verringern,
- große zusammenhängende Speicherbereiche effizienter abbilden,
- bestimmte Compute-, Datenbank-, Grafik- und Accelerator-Workloads beschleunigen.

Gleichzeitig besitzen Huge Pages erhebliche Nachteile.

Sie können:

- physische Kontiguität verlangen,
- Fragmentierung verschärfen,
- unnötig große Speicherbereiche resident halten,
- Copy-On-Write verteuern,
- NUMA-Migration grobgranular machen,
- Reclamation erschweren,
- Memory Pressure verschärfen,
- interne Fragmentierung erzeugen.

Daher darf NovaOS Huge Pages weder grundsätzlich deaktivieren noch pauschal für große Speicherbereiche erzwingen.

Die Page-Größe muss als **policygesteuerte Optimierungsentscheidung** behandelt werden.

---

## Entscheidung

NovaOS unterstützt mehrere Hardware-Page-Größen und behandelt Huge Pages als **optionale, policygesteuerte Speicheroptimierung**.

Die Auswahl erfolgt innerhalb der bereits zulässigen Memory-Konfiguration:

```text
Memory Mapping Request
        ↓
Hard Constraints
        ↓
Eligible Page Sizes
        ↓
Memory / Locality State
        ↓
Performance Policy
        ↓
Selected Page Size
```

Huge Pages sind damit kein semantisches Merkmal eines Memory Objects.

Die sichtbare Semantik eines Memory Objects darf grundsätzlich unabhängig davon bleiben, ob dessen Backing über normale Pages oder Huge Pages abgebildet wird.

---

## Grundprinzip

```text
Page size is an optimization.

Use large pages when beneficial.

Split or avoid them when costly.

Never sacrifice correctness for TLB efficiency.
```

---

## Unterstützte Page-Größen

Die verfügbaren Page-Größen werden von der jeweiligen Hardware- und MMU-Plattform bereitgestellt.

NovaOS darf keine universelle feste Menge an Page-Größen voraussetzen.

Konzeptionell:

```text
PageSizeCapabilities:
    Base
    Large
    Huge
    Architecture-specific variants
```

Der HAL beziehungsweise das architekturspezifische VM-Backend stellt die tatsächlich unterstützten Größen bereit.

---

## Basisseitengröße

Jede Plattform besitzt mindestens eine reguläre Basisseitengröße.

Diese bildet den allgemeinen Fallback.

Kann eine Huge-Page-Allokation nicht sinnvoll oder sicher durchgeführt werden, muss der VMM auf kleinere zulässige Pages zurückgreifen können, sofern kein expliziter Hard Constraint eine bestimmte Page-Größe fordert.

---

## Huge Pages als Soft Policy

Standardmäßig ist die Verwendung einer Huge Page eine Soft-Policy-Entscheidung.

Sie darf unter anderem berücksichtigen:

- Mapping-Größe,
- Alignment,
- physische Kontiguität,
- erwartete Lebensdauer,
- Zugriffsmuster,
- Memory Pressure,
- Fragmentierung,
- NUMA Locality,
- COW-Wahrscheinlichkeit,
- Reclamation-Verhalten,
- TLB Pressure.

Die reine Größe eines Mappings ist nicht ausreichend, um automatisch Huge Pages zu erzwingen.

---

## Explizite Page-Size-Anforderungen

Execution Contracts oder spezialisierte Memory Requests dürfen Page-Size-Präferenzen ausdrücken.

Beispielsweise:

```text
Preferred Page Size = Huge
```

oder:

```text
Required Page Size = 2 MiB
```

Preferred bleibt eine Optimierungspräferenz.

Required ist ein Hard Constraint und darf nicht still abgeschwächt werden.

Ein Required-Page-Size-Request ist nur für spezialisierte Anwendungsfälle vorgesehen.

---

## Promotion

NovaOS darf bestehende kleinere Pages zu einer größeren Page zusammenfassen.

Beispiel:

```text
512 × 4 KiB
      ↓
2 MiB Huge Page
```

Promotion ist nur zulässig, wenn:

- die virtuelle Region geeignet ausgerichtet ist,
- physisches Backing kompatibel ist,
- Schutzrechte übereinstimmen,
- Memory-Object-Semantik erhalten bleibt,
- NUMA- und Security-Constraints eingehalten werden.

Promotion ist eine Optimierung und darf transparent erfolgen.

---

## Demotion und Splitting

Huge Pages müssen bei Bedarf wieder in kleinere Pages zerlegt werden können.

Gründe umfassen:

- Copy-On-Write,
- partielle Protection Changes,
- Memory Reclamation,
- NUMA Migration,
- Memory Pressure,
- partielles Unmapping,
- Hotplug,
- Debugging oder Guard-Region-Anforderungen.

Beispiel:

```text
2 MiB Huge Page
      ↓
Split
      ↓
512 × 4 KiB
```

Splitting darf die logische Memory-Object-Semantik nicht verändern.

---

## Physische Kontiguität

Bestimmte Hardware-Huge-Pages benötigen physisch zusammenhängendes Backing.

Daher muss eine Huge-Page-Entscheidung mit dem Physical Memory Manager abgestimmt werden.

```text
Huge Page Request
      ↓
Contiguous Backing available?
```

NovaOS darf allgemeinen Speicher nicht unverhältnismäßig fragmentieren oder aggressive Compaction erzwingen, nur um eine optionale Huge Page zu erzeugen.

---

## Fragmentierung

Huge Pages können sowohl externe als auch interne Fragmentierung beeinflussen.

Interne Fragmentierung entsteht beispielsweise, wenn ein großer physischer Bereich reserviert wird, obwohl nur ein kleiner Teil tatsächlich benötigt wird.

Externe Fragmentierung kann verhindern, dass später ausreichend große zusammenhängende Bereiche verfügbar sind.

Die Huge-Page-Policy muss deshalb den aktuellen Fragmentierungszustand berücksichtigen können.

---

## Memory Pressure

Unter hohem Memory Pressure kann die Verwendung großer Pages ungünstig werden.

Mögliche Reaktionen sind:

```text
Stop new promotion
Split existing huge pages
Prefer base pages
Reclaim finer-grained regions
```

Huge Pages dürfen nicht so behandelt werden, als seien sie grundsätzlich wertvoller als die allgemeine Systemstabilität.

---

## Demand Paging

Huge Pages müssen mit Demand Paging kompatibel sein.

Eine große virtuelle Region muss nicht automatisch als vollständige Huge Page materialisiert werden.

NovaOS darf zunächst kleinere Pages materialisieren und später bei geeignetem Nutzungsverhalten eine Promotion durchführen.

```text
Lazy Mapping
    ↓
Base Pages on Demand
    ↓
Stable Dense Usage
    ↓
Possible Promotion
```

Dadurch wird unnötige physische Belegung großer, nur teilweise genutzter Bereiche vermieden.

---

## Copy-On-Write

COW kann Huge Pages besonders teuer machen.

Ein einzelner Schreibzugriff darf nicht zwangsläufig die Kopie einer sehr großen Page erzwingen, wenn ein Split wirtschaftlicher und semantisch zulässig ist.

Die Policy kann deshalb zwischen:

```text
Copy entire Huge Page
```

und:

```text
Split
 ↓
COW smaller page
```

wählen.

Die Entscheidung kann von:

- Änderungsrate,
- Page-Größe,
- Memory Pressure,
- Locality,
- erwarteter weiterer Nutzung

abhängen.

---

## Shared Memory

Shared Memory Objects dürfen Huge Pages verwenden.

Dabei müssen alle beteiligten Mappings weiterhin dieselbe logische Shared-Memory-Semantik sehen.

Eine Domain darf ein Shared Object gegebenenfalls mit anderer Page-Table-Granularität abbilden, sofern die Hardware und VMM-Semantik dies zulassen und die Objektidentität erhalten bleibt.

---

## NUMA

Huge Pages müssen NUMA-aware platziert werden.

Eine Huge Page darf nicht über physisch ungeeignete NUMA Domains konstruiert werden, wenn die Plattform ein lokales zusammenhängendes Backing verlangt.

Promotion soll die Locality bestehender Pages berücksichtigen.

Beispiel:

```text
Pages spread across Node 0 + Node 1
        ↓
No local Huge Page promotion
```

Eine Promotion darf nicht still Required-NUMA-Constraints verletzen.

---

## Memory Migration

Huge Pages können als Ganzes migriert oder vor der Migration gesplittet werden.

Die Policy kann entscheiden:

```text
Migrate Huge Page
```

oder:

```text
Split
 ↓
Migrate selected pages
```

Die Wahl hängt unter anderem von:

- Zugriffslokalität,
- Migration Cost,
- Memory Pressure,
- verbleibender Nutzungsdauer

ab.

---

## Realtime

Realtime-Domains dürfen Huge Pages verwenden, wenn dies Teil ihres vorbereiteten Memory Contracts ist.

Vorteile können reduzierte TLB-Misses und vorhersehbarere Mapping-Strukturen sein.

Gleichzeitig dürfen Promotion, Splitting, Compaction oder unerwartete Huge-Page-Faults nicht unkontrolliert im Hard-Realtime-Pfad stattfinden.

Realtime-Speicher muss deshalb gegebenenfalls:

```text
Reserve
 ↓
Materialize
 ↓
Select Page Size
 ↓
Lock configuration
 ↓
Enter realtime section
```

vorbereitet werden.

---

## Deterministic Mode

Deterministic Mode muss adaptive Huge-Page-Promotion und -Demotion einschränken können.

Eine deterministische Konfiguration kann beispielsweise festlegen:

```text
Base pages only
```

oder:

```text
Fixed huge-page regions
```

oder:

```text
Predefined promotion plan
```

Laufzeitheuristiken dürfen nicht zu nicht reproduzierbaren Page-Size-Entscheidungen führen, wenn der Execution Contract deterministische Memory-Eigenschaften verlangt.

---

## Zero-Copy

Zero-Copy-Buffers dürfen Huge Pages nutzen, wenn:

- beteiligte Devices die Page-Größe beziehungsweise das Mapping unterstützen,
- IOMMU-Constraints erfüllt sind,
- physische Placement-Anforderungen erfüllt sind.

Eine CPU-Huge-Page-Abbildung impliziert nicht automatisch, dass ein Device dieselbe Mapping-Größe verwenden muss.

CPU-MMU und IOMMU dürfen unterschiedliche Granularitäten verwenden.

---

## Device und Accelerator Memory

GPU-, NPU- und andere Device-Memory-Systeme können eigene Page-Größen oder Translation Granules besitzen.

NovaOS muss deshalb unterscheiden zwischen:

```text
CPU Virtual Page Size
IOMMU Mapping Size
Device Page Size
Physical Allocation Granularity
```

Diese Größen dürfen nicht implizit als identisch angenommen werden.

Die gemeinsame Policy muss jedoch unnötige Translation- und Fragmentierungskosten vermeiden können.

---

## Security

Huge Pages dürfen Security- und Protection-Grenzen nicht vergröbern.

Wenn innerhalb einer großen Page unterschiedliche Protection Policies notwendig werden, muss die Page gegebenenfalls gesplittet werden.

Beispiel:

```text
Huge Page
 ├── RX Code
 └── RW Data
```

ist nicht als ein gemeinsames Mapping zulässig, wenn dadurch das Least-Privilege-Prinzip verletzt würde.

Security besitzt Vorrang vor Huge-Page-Effizienz.

---

## Data Sovereignty

Huge-Page-Promotion darf keine Daten aus unterschiedlichen Sovereignty- oder Security-Domains unzulässig in ein gemeinsames physisches oder administratives Backing zwingen.

Placement- und Sharing-Grenzen bleiben auch bei Promotion verbindlich.

---

## Hotplug

Memory Hotplug kann eine Huge Page betreffen.

Vor dem Offline-Nehmen einer Memory Region muss NovaOS eine Huge Page:

- migrieren,
- splitten,
- oder den Hotplug blockieren

können.

Ein nicht migrierbares Huge-Page-Backing darf nicht still verloren gehen.

---

## Adaptive Huge-Page-Policy

NovaOS darf Laufzeitmetriken verwenden, um Huge-Page-Entscheidungen zu optimieren.

Beispiel:

```text
Large Mapping
   ↓
High TLB miss rate
   ↓
Dense stable access
   ↓
Promotion candidate
```

oder:

```text
Huge Page
   ↓
Frequent COW / partial reclaim
   ↓
High split cost
   ↓
Future promotion discouraged
```

Prediction Error darf das Modell korrigieren.

Adaptive Entscheidungen bleiben Soft Policy.

---

## Kostenmodell

Die Policy soll nicht nur den potenziellen TLB-Gewinn betrachten.

Sie kann konzeptionell bewerten:

```text
Benefit:
    Lower TLB pressure
    Smaller page tables
    Better sequential translation

Cost:
    Contiguous allocation
    Internal fragmentation
    Compaction
    COW amplification
    Reclaim granularity
    Migration cost
```

Die genaue Heuristik wird in einer NPSPEC definiert.

---

## Architecture Introspection

Huge-Page-Nutzung muss introspektierbar sein.

Relevant sind insbesondere:

```text
Page Size
Mapping
Memory Object
Execution Domain
NUMA Domain
Promotions
Demotions
Splits
Promotion Failures
TLB-related metrics
Fragmentation
Huge-page residency
COW-induced splits
```

Damit kann beurteilt werden, ob Huge Pages für eine konkrete Workload tatsächlich einen Vorteil erzeugen.

---

## Normative Anforderungen

1. NovaOS MUSS mehrere von der jeweiligen Hardware unterstützte Page-Größen verwenden können.
2. Die verfügbaren Page-Größen MÜSSEN über das architekturspezifische Memory Backend ermittelbar sein.
3. Huge Pages MÜSSEN standardmäßig als Soft-Policy-Optimierung behandelt werden.
4. Die sichtbare Memory-Object-Semantik DARF NICHT von einer bestimmten Page-Größe abhängen.
5. Preferred und Required Page Size MÜSSEN unterscheidbar sein.
6. Ein Required-Page-Size-Constraint DARF NICHT still abgeschwächt werden.
7. Der VMM MUSS auf kleinere zulässige Pages zurückfallen können, wenn eine optionale Huge Page nicht verfügbar oder nicht sinnvoll ist.
8. Promotion kleiner Pages zu Huge Pages MUSS möglich sein.
9. Demotion beziehungsweise Splitting von Huge Pages MUSS möglich sein.
10. Promotion und Splitting MÜSSEN die Memory-Object-Semantik erhalten.
11. Huge-Page-Allokation MUSS mit dem Physical Memory Manager und dessen Contiguity-Modell koordiniert werden.
12. Optionale Huge-Page-Nutzung DARF keine unverhältnismäßige Compaction erzwingen.
13. Fragmentierung und Memory Pressure MÜSSEN in Huge-Page-Policies berücksichtigt werden können.
14. Demand Paging MUSS mit Huge Pages kombinierbar sein.
15. COW MUSS Huge Pages bei Bedarf splitten oder vollständig kopieren können.
16. Shared Memory MUSS mit Huge Pages kompatibel sein.
17. Huge-Page-Placement MUSS NUMA-aware erfolgen.
18. Required-NUMA-Constraints DÜRFEN durch Huge-Page-Promotion NICHT verletzt werden.
19. Memory Migration MUSS Huge Pages direkt oder nach Splitting behandeln können.
20. Hard-Realtime-Pfade DÜRFEN NICHT von unkontrollierter Promotion, Compaction oder Demotion abhängen.
21. Deterministic Mode MUSS adaptive Huge-Page-Policies einschränken können.
22. CPU-, IOMMU- und Device-Page-Größen MÜSSEN als getrennte Eigenschaften modellierbar sein.
23. Huge Pages DÜRFEN Security- oder Protection-Grenzen NICHT abschwächen.
24. Hotplug MUSS Huge Pages migrieren, splitten oder kontrolliert blockieren können.
25. Adaptive Huge-Page-Entscheidungen DÜRFEN ausschließlich Soft-Policy-Ziele optimieren.
26. Huge-Page-Nutzung, Promotion, Demotion, Fragmentierung und relevante Kosten MÜSSEN introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- TLB-Druck kann bei großen Working Sets deutlich reduziert werden,
- Page-Table-Overhead kann sinken,
- große Compute- und Datenworkloads können effizienter ausgeführt werden,
- NovaOS kann Huge Pages automatisch einsetzen, ohne Anwendungen grundsätzlich darauf festzulegen,
- Page-Größen können an NUMA-, Realtime- und Device-Anforderungen angepasst werden,
- adaptive Policies können den tatsächlichen Nutzen pro Workload bewerten.

### Negative Konsequenzen

- Physical Memory Management wird komplexer,
- Huge Pages können Fragmentierung verschärfen,
- COW, Reclamation und Migration werden bei großen Pages teurer,
- Promotion und Demotion benötigen zusätzliche Synchronisation und TLB-Arbeit,
- falsche Heuristiken können mehr Kosten als Nutzen erzeugen.

---

## Verworfene Alternativen

### Huge Pages grundsätzlich deaktivieren

Verworfen.

Dies würde mögliche erhebliche TLB- und Page-Table-Vorteile für große Workloads verschenken.

### Huge Pages für alle großen Mappings erzwingen

Verworfen.

Große virtuelle Bereiche sind nicht automatisch dicht genutzt oder für grobgranulares Backing geeignet.

### Page-Größe ausschließlich durch Anwendungen festlegen

Verworfen.

NovaOS besitzt bessere Sicht auf Memory Pressure, Fragmentierung, NUMA, Hardware und systemweite Ressourcen.

Explizite Anwendungsanforderungen bleiben dennoch möglich.

### Keine Demotion einmal erzeugter Huge Pages

Verworfen.

COW, Reclamation, Protection Changes, Migration und Hotplug benötigen gegebenenfalls feinere Granularität.

### Globale reservierte Huge-Page-Pools als einziges Modell

Verworfen.

Statische Reservierungen können ungenutzten Speicher binden und passen nicht zum dynamischen Resource-Economy-Modell von NovaOS.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-MEMORY-0001_Hierarchische_Physical_Memory_Architecture`
- `ADR-MEMORY-0002_NUMA_Aware_Physical_Memory_Allocation`
- `ADR-MEMORY-0003_Memory_Pressure_als_systemweites_Signal`
- `ADR-MEMORY-0004_Domain_Local_Out_Of_Memory_Handling`
- `ADR-MEMORY-0005_Virtual_Address_Spaces_pro_Execution_Domain`
- `ADR-MEMORY-0006_Demand_Paging_und_Lazy_Allocation`
- `ADR-MEMORY-0007_Copy_On_Write_als_gezielte_Optimierung`
- `ADR-MEMORY-0008_Shared_Memory_über_explizite_Shared_Objects`
- `ADR-HAL-0002_Plattformabstraktion_für_x86_ARM_und_zukünftige_Architekturen`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0018_Locality_als_Optimierungsziel`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`

---

## Zugehörige NPSPECs

- `NPSPEC-MEMORY-HUGEPAGE-0001`
- `NPSPEC-MEMORY-HUGEPAGE-POLICY-0001`
- `NPSPEC-MEMORY-HUGEPAGE-PROMOTION-0001`
- `NPSPEC-MEMORY-HUGEPAGE-SPLIT-0001`
- `NPSPEC-MEMORY-HUGEPAGE-NUMA-0001`
- `NPSPEC-MEMORY-HUGEPAGE-COW-0001`
- `NPSPEC-MEMORY-HUGEPAGE-REALTIME-0001`
- `NPSPEC-MEMORY-HUGEPAGE-TEST-0001`

---

## Ergebnis

NovaOS behandelt die Page-Größe als dynamische Speicheroptimierung:

```text
Memory Mapping
      ↓
Hard Constraints
      ↓
Supported Page Sizes
      ↓
NUMA + Pressure + Fragmentation
+ Access Pattern + COW + Realtime
      ↓
Page-Size Policy
      ↓
Base Page / Huge Page
      ↕
Promotion / Demotion
```

Huge Pages werden damit eingesetzt, wenn ihre realen Vorteile die zusätzlichen Speicher- und Verwaltungskosten rechtfertigen.

Die zentrale Architekturregel lautet:

```text
Use the largest useful page,
not the largest possible page.

Promote when beneficial.

Split when necessary.

Keep page size a policy,
not a semantic dependency.
```