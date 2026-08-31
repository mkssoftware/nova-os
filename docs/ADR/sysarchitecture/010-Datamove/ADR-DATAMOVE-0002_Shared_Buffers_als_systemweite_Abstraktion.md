# ADR-DATAMOVE-0002 – Shared Buffers als systemweite Abstraktion

## Status

Angenommen

## Kategorie

Kernel / Data Movement / Shared Memory / Zero-Copy / I/O

## Kontext

Zero-Copy-Datenpfade benötigen eine einheitliche Abstraktion für Speicherbereiche, die zwischen mehreren Systemkomponenten verwendet werden können.

Ohne ein gemeinsames Modell würden Subsysteme wie:

- IPC,
- Netzwerk,
- Storage,
- Audio und Video,
- Grafik,
- GPU/NPU,
- Treiber und DMA

jeweils eigene Buffer- und Ownership-Modelle entwickeln.

Dies würde Zero-Copy über Subsystemgrenzen erschweren und zu unnötigen Konvertierungen und Kopien führen.

---

## Entscheidung

NovaOS führt **Shared Buffers** als systemweite Abstraktion für gemeinsam nutzbare Datenpuffer ein.

Ein Shared Buffer basiert auf einem expliziten Memory Object und ergänzt dieses um die für Datenbewegung notwendigen Eigenschaften.

```text
Shared Buffer
    ├── Memory Object
    ├── Size / Layout
    ├── Ownership
    ├── Access Authority
    ├── Memory Properties
    └── Synchronization State
```

Shared Buffers bilden damit eine gemeinsame Grundlage für Datenpfade zwischen Execution Domains, Systemdiensten und geeigneten Devices.

---

## Grundprinzip

```text
One buffer abstraction.

Multiple producers and consumers.

Explicit ownership and authority.

No unnecessary copies between subsystems.
```

---

## Verhältnis zu Shared Memory Objects

Shared Buffers ersetzen nicht die Shared Memory Objects aus `ADR-MEMORY-0008`.

Es gilt:

```text
Shared Memory Object
        ↓
Memory Sharing Mechanism

Shared Buffer
        ↓
Data Movement Abstraction
```

Ein Shared Buffer verwendet Memory Objects als Speichergrundlage und ergänzt die für Datenpipelines benötigte Semantik.

---

## Einheitliche Buffer-Identität

Ein Shared Buffer besitzt eine stabile logische Identität, die unabhängig von:

- virtueller Adresse,
- physischer Adresse,
- Execution Domain,
- Device Mapping

ist.

Dadurch kann derselbe Buffer unterschiedliche Mappings besitzen:

```text
Shared Buffer
   ├── CPU Mapping
   ├── Domain Mapping
   ├── GPU Mapping
   └── DMA Mapping
```

Die beteiligten Komponenten müssen keine gemeinsamen virtuellen Adressen verwenden.

---

## Ownership

Shared Buffers besitzen explizite Ownership.

Typischer Datenfluss:

```text
Producer owns buffer
        ↓
Publish / Transfer
        ↓
Consumer owns buffer
```

Ownership Transfer soll ohne Kopie des Buffer-Inhalts möglich sein.

Für echte gemeinsame Nutzung können mehrere autorisierte Teilnehmer gleichzeitig Zugriff erhalten.

Ownership Transfer und Shared Access bleiben unterschiedliche Semantiken.

---

## Zugriff und Capabilities

Zugriff auf einen Shared Buffer benötigt explizite Authority.

Mögliche Rechte umfassen konzeptionell:

```text
Read
Write
Map
Transfer
Share
Device Access
```

Der Besitz einer Buffer-ID allein gewährt keinen Zugriff.

Least Privilege bleibt auch innerhalb von Zero-Copy-Pipelines verbindlich.

---

## Buffer-Eigenschaften

Shared Buffers müssen die für ihre Verwendung relevanten Memory-Eigenschaften beschreiben können.

Dazu gehören beispielsweise:

- Größe,
- Alignment,
- Memory Domain,
- CPU-Zugänglichkeit,
- Device-Zugänglichkeit,
- Pinning-Anforderungen,
- Cache-/Coherency-Eigenschaften.

Subsystemspezifische Eigenschaften sollen über explizite Contracts ergänzt werden, ohne separate inkompatible Buffer-Grundmodelle zu erzeugen.

---

## CPU- und Device-Nutzung

Shared Buffers können zwischen CPU und geeigneten Devices verwendet werden.

```text
CPU Producer
     ↓
Shared Buffer
     ↓
GPU / NPU / Device
```

IOMMU-, DMA-, Coherency- und Pinning-Anforderungen bleiben explizite Eigenschaften des jeweiligen Datenpfads.

Ein Shared Buffer ist nicht automatisch für jedes Device zugänglich.

---

## Synchronisation

Shared Buffers definieren keine implizite globale Synchronisation.

Ein Datenpfad muss explizit festlegen, wann ein Buffer:

```text
Writable
Ready
In Use
Completed
Reusable
```

ist.

Die konkrete Synchronisationssemantik kann abhängig vom Subsystem über:

- Events,
- Futures,
- Fences,
- Atomics,
- andere geeignete Mechanismen

realisiert werden.

---

## Buffer Pools

Subsysteme dürfen Pools wiederverwendbarer Shared Buffers verwenden.

```text
Buffer Pool
    ↓
Acquire
    ↓
Use
    ↓
Release
    ↓
Reuse
```

Dadurch können wiederholte Allokationen, Mapping-Operationen und Pinning-Kosten reduziert werden.

Pools bleiben budgetierte Ressourcen und dürfen Speicher nicht unbegrenzt zurückhalten.

---

## Resource Economy

Shared Buffers müssen vollständig in die Resource Economy integriert sein.

Insbesondere müssen:

- physisches Backing,
- Ownership,
- Shared Usage,
- Pinning,
- Device-Mappings,
- Pool-Reservierungen

zurechenbar bleiben.

Shared Buffers dürfen Memory- oder Device-Budgets nicht umgehen.

---

## Locality

Die Platzierung eines Shared Buffers soll die beteiligten Producer, Consumer und Devices berücksichtigen.

```text
Producer Locality
       +
Consumer Locality
       +
Device Locality
       ↓
Buffer Placement
```

Wenn eine Kopie in einen besser geeigneten Speicherort insgesamt günstiger ist, darf die Data-Movement-Policy einen Copy-Fallback wählen.

Zero-Copy bleibt Optimierungsziel, kein Selbstzweck.

---

## Security und Data Sovereignty

Jede Freigabe oder Übertragung eines Shared Buffers stellt einen möglichen Informationsfluss dar.

Daher müssen:

- Security,
- Trust,
- Capabilities,
- Data Sovereignty

vor Mapping, Sharing oder Ownership Transfer geprüft werden.

Nicht zulässiges Sharing darf nicht durch Zero-Copy-Anforderungen erzwungen werden.

---

## Realtime und Determinismus

Realtime-Datenpfade können Shared Buffers und Buffer Pools vorab:

- reservieren,
- materialisieren,
- mappen,
- gegebenenfalls pinnen.

Dadurch lassen sich dynamische Speicheroperationen im kritischen Pfad vermeiden.

Deterministic Mode muss Buffer-Pool-, Placement- und Ownership-Policies reproduzierbar konfigurieren können.

---

## Architecture Introspection

Shared Buffers müssen kontrolliert introspektierbar sein.

Relevant sind insbesondere:

- Buffer Identity,
- Größe,
- Owner,
- aktive Nutzer,
- Memory Domain,
- Mappings,
- Device-Zugriffe,
- Pinning,
- Pool-Zugehörigkeit,
- Resource Accounting.

Damit können Datenpfade und Ressourcenbindungen systemweit nachvollzogen werden.

---

## Normative Anforderungen

1. NovaOS MUSS Shared Buffers als systemweite Data-Movement-Abstraktion bereitstellen.
2. Shared Buffers MÜSSEN auf expliziten Memory Objects aufbauen.
3. Shared Buffers MÜSSEN unabhängig von virtuellen und physischen Adressen identifizierbar sein.
4. Ownership und Access Authority MÜSSEN explizit modelliert werden.
5. Ownership Transfer MUSS ohne notwendige Kopie der Nutzdaten möglich sein.
6. Shared Access MUSS von Ownership Transfer unterscheidbar bleiben.
7. Shared Buffers MÜSSEN für CPU-, Domain- und geeignete Device-Datenpfade verwendbar sein.
8. Device-Zugriff MUSS weiterhin explizite DMA-/IOMMU-Autorisierung verwenden.
9. Shared Buffers DÜRFEN keine implizite Synchronisationssemantik voraussetzen.
10. Wiederverwendbare Buffer Pools MÜSSEN unterstützt werden können.
11. Shared Buffers und Pools MÜSSEN in die Resource Economy integriert sein.
12. Placement MUSS NUMA-, Device- und andere Locality-Anforderungen berücksichtigen können.
13. Security-, Trust- und Data-Sovereignty-Constraints MÜSSEN Vorrang vor Sharing besitzen.
14. Shared-Buffer-Zustände und Ressourcenbindungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- einheitliches Buffer-Modell über Subsystemgrenzen,
- weniger unnötige Kopien und Konvertierungen,
- direkte CPU-/GPU-/NPU-/Device-Pipelines,
- explizite Ownership- und Security-Semantik,
- effiziente Wiederverwendung über Buffer Pools,
- bessere systemweite Introspection von Datenpfaden.

### Negative Konsequenzen

- Lifetime-, Ownership- und Synchronisationsmodelle werden wichtiger,
- Device-Kompatibilität muss explizit berücksichtigt werden,
- Shared Buffer Pools benötigen Budgetierung,
- nicht jeder Datenpfad kann tatsächlich Zero-Copy bleiben.

---

## Verworfene Alternativen

### Eigene Buffer-Abstraktion pro Subsystem

Verworfen.

Dies würde systemweite Zero-Copy-Pipelines erschweren und Konvertierungen zwischen inkompatiblen Buffer-Modellen erzeugen.

### Shared Buffers als rohe Pointer

Verworfen.

Pointer besitzen keine systemweite Ownership-, Authority- oder Location-Semantik.

### Shared Buffers ersetzen Shared Memory Objects

Verworfen.

Memory Objects bilden die Speicher- und Sharing-Grundlage; Shared Buffers bilden darauf die Data-Movement-Abstraktion.

### Jeder Shared Buffer ist automatisch für alle Devices nutzbar

Verworfen.

Hardware-, IOMMU-, Coherency- und Memory-Domain-Anforderungen unterscheiden sich zwischen Devices.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-DATAMOVE-0001_Zero_Copy_Datenpfade`
- `ADR-MEMORY-0008_Shared_Memory_über_explizite_Shared_Objects`
- `ADR-MEMORY-0014_Pinned_Memory_als_budgetierte_Ressource`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0009_Einheitliches_Daten_und_Objekt_Pipelining`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0018_Locality_als_Optimierungsziel`

---

## Zugehörige NPSPECs

- `NPSPEC-DATAMOVE-BUFFER-0001`
- `NPSPEC-DATAMOVE-BUFFER-OWNERSHIP-0001`
- `NPSPEC-DATAMOVE-BUFFER-MAPPING-0001`
- `NPSPEC-DATAMOVE-BUFFER-POOL-0001`
- `NPSPEC-DATAMOVE-BUFFER-DEVICE-0001`
- `NPSPEC-DATAMOVE-BUFFER-SYNC-0001`
- `NPSPEC-DATAMOVE-BUFFER-TEST-0001`

---

## Ergebnis

Shared Buffers bilden die gemeinsame Datenpuffer-Abstraktion von NovaOS:

```text
Producer
    ↓
Shared Buffer
    ↓
Ownership / Sharing
    ↓
Consumer / GPU / NPU / Device
```

Damit können unterschiedliche Subsysteme dieselben Datenobjekte verwenden, ohne jeweils eigene inkompatible Buffer-Modelle oder unnötige Kopierstufen einzuführen.

Die zentrale Architekturregel lautet:

```text
One shared buffer model.

Explicit ownership.

Explicit authority.

Reuse backing instead of copying data.
```