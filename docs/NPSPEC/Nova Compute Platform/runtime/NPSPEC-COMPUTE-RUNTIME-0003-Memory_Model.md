# NPSPEC-COMPUTE-RUNTIME-0003 — Memory Model

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Runtime  
**Komponente:** NovaOS Compute Runtime Memory Model  
**Bezeichner:** `NPSPEC-COMPUTE-RUNTIME-0003`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Kurzname:** Compute Memory Model  

---

## 1. Zweck

Diese Spezifikation definiert das **Memory Model** der NovaOS Compute Runtime.

Das Compute Memory Model beschreibt, wie Speicher für heterogene Recheneinheiten:

- erzeugt,
- adressiert,
- zugeordnet,
- geteilt,
- synchronisiert,
- migriert,
- geschützt,
- freigegeben,
- zwischen Devices übertragen

wird.

Das Memory Model MUSS mit unterschiedlichen Hardwarearchitekturen funktionieren.

Dazu gehören insbesondere:

- klassische CPU-Systeme,
- diskrete GPUs mit eigenem VRAM,
- integrierte GPUs mit gemeinsamem Hauptspeicher,
- NPUs mit lokalem Speicher,
- DSPs,
- NUMA-Systeme,
- Multi-GPU-Systeme,
- Unified-Memory-Systeme,
- zukünftige heterogene Speicherarchitekturen.

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Compute-Anwendungen arbeiten primär mit logischen Ressourcen. Die Compute Runtime verwaltet deren physische Speicherplatzierung, Zugriffsrechte, Kohärenz und erforderliche Datenbewegungen.

Die Anwendung SOLL nur dann konkrete physische Speicherentscheidungen treffen müssen, wenn dies für Performance, deterministisches Verhalten oder spezielle Hardwareanforderungen notwendig ist.

---

## 3. Ziele

Das Compute Memory Model MUSS folgende Ziele verfolgen:

1. einheitliche Speicherabstraktion,
2. Hardwareunabhängigkeit,
3. geringe Kopierkosten,
4. effiziente Datenlokalität,
5. Unterstützung von Unified Memory,
6. Unterstützung von dediziertem Device Memory,
7. Zero-Copy-Unterstützung,
8. sichere Prozessisolation,
9. kontrollierte Speicherfreigabe,
10. Memory Residency Management,
11. NUMA-Awareness,
12. explizite Synchronisation,
13. Cache-Kohärenz,
14. Ressourcenfreigabe zwischen Subsystemen,
15. robuste Fehlerbehandlung,
16. Erweiterbarkeit für zukünftige Speichertechnologien.

---

## 4. Nicht-Ziele

Diese Spezifikation definiert nicht vollständig:

- den allgemeinen virtuellen Speicher von NovaOS,
- den physischen Kernel Memory Manager,
- Dateisystem-Caching,
- allgemeinen Swap,
- vollständige GPU-Treiberdetails,
- konkrete Device Page Tables,
- das öffentliche Nova Compute API-Layout.

Sie definiert die Anforderungen der Compute Runtime an diese Mechanismen.

---

## 5. Architektur

Die logische Architektur lautet:

```text
Application
    |
    v
Compute Resource
    |
    v
+-----------------------------+
| Compute Memory Manager      |
|                             |
| - Allocation                |
| - Residency                 |
| - Mapping                   |
| - Migration                 |
| - Coherency                 |
| - Synchronization           |
| - Protection                |
| - Budgeting                 |
| - Eviction                  |
+-----------------------------+
    |
    +-----------+-----------+----------+
    |           |           |          |
    v           v           v          v
Host RAM     GPU VRAM    NPU RAM     Shared
                                     Memory
```

---

## 6. Speicherobjekte

Die Compute Runtime MUSS mindestens folgende logische Speicherobjekte unterstützen können:

```text
Buffer
Image
Tensor
SharedResource
TemporaryResource
ImportedResource
```

`Buffer` ist die grundlegende verpflichtende Speicherressource.

Weitere Ressourcentypen DÜRFEN auf diesem Modell aufbauen.

---

## 7. Compute Resource

Eine Compute Resource ist ein logisches Datenobjekt.

Sie MUSS mindestens folgende Eigenschaften besitzen können:

```text
ResourceID
OwnerContext
Size
Alignment
ResourceType
MemoryClass
UsageFlags
AccessFlags
ResidencyState
MappingState
CoherencyMode
SecurityDomain
```

---

## 8. Opaque Resource Handles

Anwendungen SOLLEN Speicherressourcen ausschließlich über opaque Handles adressieren.

Beispiel:

```text
NovaComputeBufferHandle
```

Physische Adressen DÜRFEN nicht als öffentliche Compute Handles verwendet werden.

---

## 9. Speicherklassen

Die Runtime MUSS mindestens folgende Speicherklassen modellieren:

```text
HOST
DEVICE
SHARED
UNIFIED
LOCAL
CONSTANT
```

Zusätzliche Klassen DÜRFEN ergänzt werden.

---

## 10. HOST

`HOST` bezeichnet regulären CPU-adressierbaren Systemspeicher.

Typischerweise:

```text
System RAM
```

HOST Memory kann von Compute Devices:

- direkt adressierbar,
- indirekt adressierbar,
- nur über Transfers erreichbar

sein.

---

## 11. DEVICE

`DEVICE` bezeichnet Speicher, der primär einem Compute Device zugeordnet ist.

Beispiele:

```text
GPU VRAM
NPU Local DRAM
Accelerator Memory
```

DEVICE Memory MUSS nicht direkt durch die CPU adressierbar sein.

---

## 12. SHARED

`SHARED` bezeichnet Speicher, der von mehreren Komponenten direkt verwendet werden kann.

Beispiel:

```text
CPU + integrated GPU
```

Die tatsächliche Kohärenz MUSS separat beschrieben werden.

---

## 13. UNIFIED

`UNIFIED` bezeichnet einen logisch gemeinsamen Adressraum für mehrere Compute Devices.

Unified Memory bedeutet nicht automatisch:

- identische Cache-Sicht,
- identische Zugriffsgeschwindigkeit,
- vollständige Hardware-Kohärenz.

Die Runtime MUSS diese Eigenschaften getrennt modellieren.

---

## 14. LOCAL

`LOCAL` bezeichnet schnellen, lokal an eine Compute Unit oder Workgroup gebundenen Speicher.

Beispiele:

```text
GPU Shared Memory
GPU LDS
Scratchpad
NPU SRAM
```

LOCAL Memory ist typischerweise nicht dauerhaft.

---

## 15. CONSTANT

`CONSTANT` bezeichnet logisch read-only Speicher.

Backends DÜRFEN dafür spezielle Hardwarepfade verwenden.

---

## 16. Memory Domains

Die Runtime SOLL physische Speicherbereiche in Memory Domains einteilen können.

Beispiele:

```text
CPU_RAM_0
CPU_RAM_1
GPU0_VRAM
GPU1_VRAM
NPU0_MEMORY
SHARED_SOC_MEMORY
```

---

## 17. Memory Domain Properties

Eine Memory Domain SOLL mindestens folgende Eigenschaften besitzen können:

```text
DomainID
Capacity
AvailableCapacity
Bandwidth
Latency
Addressability
Coherency
DeviceAffinity
NUMANode
Persistence
```

---

## 18. Speicherallokation

Eine Anwendung MUSS Compute Memory über die Runtime allokieren können.

Eine Allocation MUSS mindestens definieren können:

```text
Size
Alignment
Usage
Access
PreferredMemoryClass
AllowedDevices
LifetimeHint
```

---

## 19. Allocation Result

Die Runtime DARF eine andere physische Speicherplatzierung wählen als angefordert, sofern:

- die Semantik erhalten bleibt,
- kein zwingender Placement Constraint existiert,
- Sicherheitsregeln eingehalten werden.

---

## 20. Preferred Memory

Eine Anwendung DARF eine Speicherklasse bevorzugen.

Beispiel:

```text
PreferredMemory = DEVICE
```

Dies ist ein Hint.

Die Runtime DARF stattdessen beispielsweise `SHARED` verwenden.

---

## 21. Required Memory

Eine Anwendung DARF eine Speicherklasse zwingend anfordern.

Beispiel:

```text
RequiredMemory = DEVICE
```

Kann die Anforderung nicht erfüllt werden, MUSS die Allocation fehlschlagen.

---

## 22. Alignment

Die Runtime MUSS Alignment-Anforderungen berücksichtigen.

Diese können entstehen durch:

- CPU SIMD,
- GPU Hardware,
- DMA,
- Page Mapping,
- NPU Tensor Layouts.

---

## 23. Default Alignment

Wenn kein Alignment angegeben wird, MUSS die Runtime ein für alle vorgesehenen Zugriffe gültiges Alignment wählen.

---

## 24. Usage Flags

Mindestens folgende Usage Flags SOLLEN unterstützt werden:

```text
READ
WRITE
READ_WRITE

UPLOAD
DOWNLOAD

STORAGE
CONSTANT

TEMPORARY
PERSISTENT

STREAMING
RANDOM_ACCESS

ZERO_COPY_PREFERRED
DEVICE_LOCAL_PREFERRED
HOST_VISIBLE_REQUIRED
```

---

## 25. Access Flags

Zugriffsrechte SOLLEN mindestens darstellen können:

```text
HOST_READ
HOST_WRITE

DEVICE_READ
DEVICE_WRITE

MULTI_DEVICE_READ
MULTI_DEVICE_WRITE
```

---

## 26. Read-Only Resources

Ressourcen mit ausschließlich lesendem Zugriff SOLLEN optimiert behandelt werden können.

Beispiel:

```text
CONSTANT
```

kann auf mehreren Devices repliziert werden.

---

## 27. Write Resources

Schreibbare Ressourcen erfordern eine definierte Ownership- und Synchronisationsstrategie.

Die Runtime MUSS verhindern, dass widersprüchliche Schreibzugriffe zu undefiniertem Zustand führen.

---

## 28. Resource Lifetime

Eine Resource MUSS mindestens folgende Lebensdauerzustände besitzen können:

```text
CREATED
ALLOCATED
RESIDENT
MAPPED
IN_USE
RELEASE_PENDING
DESTROYED
```

---

## 29. Deferred Free

Die Runtime DARF eine Resource nach `release()` erst später physisch freigeben, wenn noch laufende Jobs darauf zugreifen.

---

## 30. Use-After-Free-Schutz

Eine logisch freigegebene Resource DARF von neuen Jobs nicht mehr verwendet werden.

Bereits registrierte interne Zugriffe DÜRFEN kontrolliert auslaufen.

---

## 31. Memory Mapping

CPU-seitiger Zugriff auf Compute Memory MUSS über ein definiertes Mapping erfolgen.

Beispiel:

```text
Map(resource)
```

Nach Abschluss:

```text
Unmap(resource)
```

---

## 32. Mapping Modes

Mindestens folgende Mapping-Modi SOLLEN unterstützt werden:

```text
READ
WRITE
READ_WRITE
```

Optional:

```text
WRITE_DISCARD
WRITE_NO_OVERWRITE
```

---

## 33. Mapping State

Eine Resource MUSS ihren Mapping-Zustand verwalten.

Beispiel:

```text
UNMAPPED
MAPPED_READ
MAPPED_WRITE
MAPPED_READ_WRITE
```

---

## 34. Host Visible Memory

Nicht jede Resource MUSS host-sichtbar sein.

Eine CPU-Abbildung kann daher:

- direkt,
- über temporären Staging Memory,
- über Migration

implementiert werden.

---

## 35. Staging Memory

Die Runtime DARF Staging Buffers automatisch erzeugen.

Beispiel:

```text
GPU VRAM
    |
    v
Staging Buffer
    |
    v
CPU Mapping
```

---

## 36. Persistent Mapping

Die Runtime DARF persistentes Mapping unterstützen.

Dies SOLL nur bei geeigneten Speicherarchitekturen verwendet werden.

---

## 37. Mapping Synchronisation

Vor CPU-Zugriff MUSS sichergestellt werden, dass vorherige Device Writes sichtbar sind.

Vor erneutem Device-Zugriff MUSS sichergestellt werden, dass CPU Writes sichtbar sind.

---

## 38. Zero-Copy

Zero-Copy bedeutet, dass Host und Device dieselben physischen Daten verwenden können, ohne explizite Kopie.

Die Runtime SOLL Zero-Copy verwenden, wenn dies effizient und sicher ist.

---

## 39. Zero-Copy ist kein Selbstzweck

Zero-Copy DARF nicht verwendet werden, wenn dedizierter Device Memory deutlich effizienter ist und die Transferkosten vertretbar sind.

---

## 40. Unified Addressing

Die Runtime SOLL Plattformen unterstützen, auf denen CPU und Device denselben virtuellen Adressraum sehen.

Eine identische virtuelle Adresse garantiert jedoch nicht automatisch Kohärenz.

---

## 41. Shared Virtual Addressing

Optional DARF die Runtime Shared Virtual Addressing unterstützen.

Beispiel:

```text
CPU VA 0x100000
GPU VA 0x100000
```

Die zugrunde liegende physische Zuordnung kann gemeinsam oder unterschiedlich sein.

---

## 42. Device Address

Backends DÜRFEN interne Device-Adressen verwenden.

Diese DÜRFEN nicht unkontrolliert an Anwendungen weitergegeben werden.

---

## 43. Pointer-Sicherheit

Ein CPU-Zeiger DARF nicht automatisch als gültiger Device Pointer interpretiert werden.

Nur bei explizit unterstützten Shared-Address-Modellen ist dies zulässig.

---

## 44. Residency

Residency beschreibt, in welchem Memory Domain eine Resource aktuell physisch verfügbar ist.

Mögliche Zustände:

```text
HOST_RESIDENT
DEVICE_RESIDENT
MULTI_RESIDENT
MIGRATING
EVICTED
```

---

## 45. Primary Residency

Eine Resource SOLL ein primäres Residency-Ziel besitzen können.

Beispiel:

```text
PrimaryResidence = GPU0_VRAM
```

---

## 46. Multi Residency

Read-only oder replizierbare Ressourcen DÜRFEN gleichzeitig in mehreren Memory Domains resident sein.

Beispiel:

```text
GPU0 VRAM
GPU1 VRAM
CPU RAM
```

---

## 47. Dirty State

Bei mehreren Kopien MUSS verfolgt werden, welche Kopie aktuell ist.

Beispiel:

```text
GPU0 = DIRTY / CURRENT
GPU1 = STALE
CPU  = STALE
```

---

## 48. Versioning

Die Runtime DARF intern Resource-Versionen verwenden.

Beispiel:

```text
ResourceVersion = 42
```

Jede Kopie kann ihre bekannte Version besitzen.

---

## 49. Migration

Eine Resource DARF zwischen Memory Domains verschoben werden.

Beispiel:

```text
CPU RAM
   |
   v
GPU0 VRAM
```

---

## 50. Migration Trigger

Migration kann ausgelöst werden durch:

- Job Scheduling,
- Mapping,
- Memory Pressure,
- Device Failure,
- Device Migration,
- User Hint,
- Power Policy,
- Topologieoptimierung.

---

## 51. Migration State

Während einer Migration SOLL die Resource einen Zustand besitzen wie:

```text
MIGRATING
```

Zugriffe MÜSSEN währenddessen korrekt synchronisiert werden.

---

## 52. Prefetch

Anwendungen und Runtime SOLLEN Resources proaktiv in eine erwartete Memory Domain verschieben können.

Beispiel:

```text
Prefetch(Resource, GPU0)
```

---

## 53. Automatic Prefetch

Der Scheduler DARF Resources automatisch vorladen, wenn ein zukünftiger Device-Zugriff wahrscheinlich ist.

---

## 54. Memory Advice

Anwendungen SOLLEN Usage Hints setzen können.

Beispiele:

```text
READ_MOSTLY
WRITE_MOSTLY
STREAMING
RANDOM
SEQUENTIAL
PREFER_HOST
PREFER_DEVICE
PREFER_SHARED
```

---

## 55. Advice-Semantik

Memory Advice ist standardmäßig ein Hint.

Die Runtime DARF davon abweichen.

Zwingende Anforderungen MÜSSEN separat angegeben werden.

---

## 56. Data Locality

Die Runtime SOLL Ressourcen möglichst dort halten, wo sie voraussichtlich wieder benötigt werden.

Beispiel:

```text
Kernel A -> GPU0
Kernel B -> GPU0
Kernel C -> GPU0
```

Die Daten SOLLEN zwischen A, B und C möglichst nicht unnötig in Host RAM zurückkopiert werden.

---

## 57. Transfer

Explizite und implizite Transfers MÜSSEN unterstützt werden.

Mindestens:

```text
HOST_TO_DEVICE
DEVICE_TO_HOST
DEVICE_TO_DEVICE
HOST_TO_HOST
```

---

## 58. Asynchronous Transfer

Transfers SOLLEN asynchron möglich sein.

Beispiel:

```text
CopyAsync(...)
```

---

## 59. Transfer Dependency

Ein Job, der auf transferierte Daten zugreift, DARF erst starten, wenn der notwendige Transfer abgeschlossen ist.

---

## 60. Transfer Engines

Besitzt ein Device separate DMA- oder Copy Engines, SOLL die Runtime diese nutzen können.

---

## 61. Overlap von Transfer und Compute

Die Runtime SOLL Transfer und Compute parallelisieren können.

Beispiel:

```text
Time --->

GPU Compute:   [Kernel A][Kernel B]

DMA Engine: [Upload B][Download A]
```

---

## 62. Device-to-Device Transfer

Direkte Device-to-Device-Transfers SOLLEN bevorzugt werden, wenn sie günstiger sind.

Beispiel:

```text
GPU0 VRAM
    |
    v
GPU1 VRAM
```

---

## 63. Peer Memory

Devices DÜRFEN direkten Zugriff auf den Speicher anderer Devices besitzen.

Diese Fähigkeit MUSS explizit als Capability erkennbar sein.

---

## 64. Peer Access

Peer Access DARF nur aktiviert werden, wenn:

- Hardware dies unterstützt,
- Topologie dies erlaubt,
- Sicherheitsrichtlinien dies erlauben.

---

## 65. Fallback Transfer

Ist direkter Device-to-Device-Transfer nicht möglich, MUSS die Runtime über Host Memory übertragen können.

```text
GPU0
 |
 v
RAM
 |
 v
GPU1
```

---

## 66. Transfer Cost Model

Die Runtime SOLL Transferkosten abschätzen können.

Zu berücksichtigen sind:

```text
Bandwidth
Latency
TransferSize
Topology
Contention
CurrentLoad
```

---

## 67. Pinned Memory

Die Runtime SOLL Host Memory für DMA pinnen können.

Pinned Memory DARF aufgrund seiner Systemkosten nicht unbegrenzt verwendet werden.

---

## 68. Pinned Memory Limits

Jeder Context SOLL ein Limit für pinned Memory besitzen.

---

## 69. Pageable Memory

Normales Host Memory DARF pageable sein.

Ein Backend kann für Device Transfers automatisch Staging oder Pinning verwenden.

---

## 70. Memory Budget

Jeder Compute Context MUSS durch Speicherbudgets begrenzbar sein.

Mindestens:

```text
HostBudget
DeviceBudget
PinnedBudget
SharedBudget
```

---

## 71. Device Memory Budget

Die Runtime SOLL pro Device ein verfügbares Budget verwalten.

Beispiel:

```text
GPU0 VRAM:

Physical = 8 GiB
Reserved = 1 GiB
AvailableForCompute = 7 GiB
```

---

## 72. System Reserve

NovaOS DARF Speicher für kritische Systemfunktionen reservieren.

Normale Anwendungen DÜRFEN diese Reserve nicht vollständig beanspruchen.

---

## 73. Soft Budget

Ein Soft Budget DARF temporär überschritten werden.

Die Runtime kann anschließend:

- evicten,
- migrieren,
- drosseln.

---

## 74. Hard Budget

Ein Hard Budget DARF nicht überschritten werden.

Die Allocation MUSS andernfalls fehlschlagen.

---

## 75. Memory Pressure

Die Runtime SOLL mindestens folgende Druckzustände kennen:

```text
LOW
MODERATE
HIGH
CRITICAL
```

---

## 76. Pressure Response

Bei steigendem Memory Pressure DARF die Runtime:

1. temporäre Ressourcen freigeben,
2. Caches reduzieren,
3. Resources evicten,
4. Resources migrieren,
5. neue Jobs verzögern,
6. Allocations ablehnen.

---

## 77. Eviction

Eine Resource DARF aus einem teuren Memory Domain entfernt werden.

Beispiel:

```text
GPU VRAM
   |
   v
System RAM
```

---

## 78. Evictable Flag

Eine Resource SOLL deklarieren können, ob sie evictable ist.

Beispiel:

```text
EVICTABLE
NON_EVICTABLE
```

`NON_EVICTABLE` MUSS restriktiv verwendet werden.

---

## 79. Resource Priority

Resources DÜRFEN eine Residency-Priorität besitzen.

Beispiel:

```text
LOW
NORMAL
HIGH
PINNED
```

---

## 80. Eviction Policy

Bei Memory Pressure SOLL bevorzugt evictiert werden:

1. ungenutzte temporäre Resources,
2. regenerierbare Caches,
3. niedrig priorisierte Resources,
4. lange nicht verwendete Resources.

Aktive Resources DÜRFEN nicht unkontrolliert entfernt werden.

---

## 81. Oversubscription

Die Runtime DARF mehr logischen Device Memory bereitstellen als physisch vorhanden ist.

Dies ist nur zulässig, wenn das Backend sicheren Paging-, Migration- oder Eviction-Support bereitstellt.

---

## 82. Device Paging

Falls ein Device Paging unterstützt, DARF die Runtime Resources seitenweise zwischen Host und Device Memory verschieben.

---

## 83. Page Faulting

Hardwareseitige oder softwareseitige Compute Page Faults DÜRFEN unterstützt werden.

Sie MÜSSEN kontrolliert behandelt werden.

---

## 84. Fault Handling

Ein gültiger Compute Page Fault kann:

```text
Page not resident
      |
      v
Pause Workload
      |
      v
Migrate Page
      |
      v
Update Mapping
      |
      v
Resume
```

auslösen.

---

## 85. Invalid Memory Access

Ein Zugriff außerhalb erlaubter Resource-Grenzen MUSS als Fehler behandelt werden.

Er DARF nicht auf fremden Speicher durchfallen.

---

## 86. Guard Regions

Debug- oder Sicherheitsmodi DÜRFEN Guard Pages oder Guard Regions um Compute Allocations verwenden.

---

## 87. Bounds Validation

Portable Compute Code SOLL vor Ausführung auf mögliche unzulässige Speicherzugriffe validiert werden, soweit dies technisch möglich ist.

---

## 88. Memory Protection

Compute Memory MUSS dem NovaOS-Sicherheitsmodell unterliegen.

Zugriffe DÜRFEN nur durch autorisierte Contexts erfolgen.

---

## 89. Cross-Context Access

Ein Context DARF nicht auf Resources eines anderen Contexts zugreifen, außer die Resource wurde explizit geteilt.

---

## 90. Resource Sharing

Resources SOLLEN explizit zwischen Contexts oder Subsystemen geteilt werden können.

Sharing MUSS durch kontrollierte Handles erfolgen.

---

## 91. Export Handle

Eine Resource kann einen Export Handle erhalten.

Dieser MUSS:

- validiert,
- zugriffsbeschränkt,
- widerrufbar

sein können.

---

## 92. Import Handle

Ein anderer Context kann eine Resource über einen gültigen Export Handle importieren.

Der Zugriff MUSS auf die gewährten Rechte begrenzt sein.

---

## 93. Shared Access Rights

Beim Teilen MUSS mindestens angegeben werden können:

```text
READ
WRITE
READ_WRITE
```

---

## 94. Resource Revocation

Der Besitzer SOLL eine Freigabe widerrufen können.

Bereits laufende Operationen MÜSSEN dabei definiert behandelt werden.

---

## 95. Graphics Interop

Compute Resources SOLLEN mit Grafikressourcen geteilt werden können.

Beispiel:

```text
Graphics Texture
      |
      v
Compute Image
```

Eine unnötige Kopie SOLL vermieden werden.

---

## 96. Video Interop

Video Decoder und Encoder SOLLEN Buffers direkt mit Compute teilen können.

Beispiel:

```text
Video Decode
     |
     v
Compute Filter
     |
     v
Display
```

---

## 97. Camera Interop

Kamera- und Sensorbuffers SOLLEN direkt an Compute-Pipelines übergeben werden können.

---

## 98. AI Interop

Tensor-Ressourcen SOLLEN zwischen AI Runtime und allgemeiner Compute Runtime ohne unnötige Datenkopien geteilt werden können.

---

## 99. Ownership

Jede Resource MUSS einen logischen Owner besitzen.

Ownership ist von physischer Residency zu unterscheiden.

---

## 100. Exclusive Write Ownership

Bei nicht kohärenten Schreibzugriffen SOLL jeweils ein eindeutiger Write Owner existieren.

Beispiel:

```text
WriteOwner = GPU0
```

---

## 101. Read Sharing

Mehrere Devices DÜRFEN gleichzeitig lesend auf dieselbe Resource zugreifen, wenn Kohärenz und Speicherarchitektur dies erlauben.

---

## 102. Simultaneous Write

Gleichzeitige Schreibzugriffe mehrerer Devices auf dieselbe Resource DÜRFEN nur erfolgen, wenn die verwendete Synchronisation und das Memory Model dies explizit erlauben.

---

## 103. Data Race

Nicht synchronisierte konkurrierende Schreibzugriffe führen zu einem Data Race.

Portable Compute Code DARF sich nicht auf undefiniertes Race-Verhalten verlassen.

---

## 104. Synchronisation

Speichersichtbarkeit MUSS durch explizite Synchronisationsoperationen kontrollierbar sein.

Mindestens:

```text
Event
Fence
Barrier
```

---

## 105. Execution Dependency

Eine Execution Dependency bestimmt, wann eine Operation starten darf.

Sie garantiert nicht automatisch alle erforderlichen Memory-Visibility-Eigenschaften.

---

## 106. Memory Dependency

Eine Memory Dependency definiert die Sichtbarkeit vorheriger Speicheroperationen.

---

## 107. Memory Barrier

Die Runtime MUSS Memory Barriers darstellen können.

Mindestens:

```text
HOST_TO_DEVICE
DEVICE_TO_HOST
DEVICE_TO_DEVICE
QUEUE_TO_QUEUE
```

---

## 108. Memory Scope

Barriers SOLLEN einen Scope besitzen können.

Beispiele:

```text
WORKGROUP
DEVICE
CONTEXT
SYSTEM
```

---

## 109. Acquire

Eine Acquire-Operation stellt sicher, dass nachfolgende Reads relevante vorherige Writes sehen.

---

## 110. Release

Eine Release-Operation stellt sicher, dass vorherige Writes für nachfolgende synchronisierte Zugriffe sichtbar gemacht werden.

---

## 111. Acquire-Release

Die Runtime SOLL Acquire/Release-Semantik unterstützen.

---

## 112. Sequential Consistency

Eine vollständige globale Sequential Consistency MUSS nicht standardmäßig garantiert werden.

Sie kann optional für bestimmte Operationen oder atomare Zugriffe angeboten werden.

---

## 113. Relaxed Memory

Backends DÜRFEN intern schwächere Memory-Ordering-Modelle verwenden, sofern die definierte Nova Compute Memory Semantik eingehalten wird.

---

## 114. Atomic Operations

Die Runtime SOLL atomare Speicheroperationen modellieren können.

Mindestens optional:

```text
ATOMIC32
ATOMIC64
```

---

## 115. Atomic Capabilities

Ein Device MUSS deklarieren, welche atomaren Typen und Operationen es unterstützt.

---

## 116. Atomic Scope

Atomics SOLLEN einen definierten Scope besitzen können.

Beispiele:

```text
WORKGROUP
DEVICE
SYSTEM
```

---

## 117. Cross-Device Atomics

Cross-Device Atomics DÜRFEN nur angeboten werden, wenn Hardware und Runtime dies tatsächlich garantieren können.

---

## 118. Cache Coherency

Die Runtime MUSS unterscheiden können zwischen:

```text
COHERENT
NON_COHERENT
PARTIALLY_COHERENT
```

---

## 119. Coherent Memory

Bei kohärentem Speicher MUSS die Runtime keine unnötigen expliziten Cache Flushes von Anwendungen verlangen.

---

## 120. Non-Coherent Memory

Bei nicht kohärentem Speicher MUSS die Runtime erforderliche Flush-, Invalidate- oder Transferoperationen durchführen oder explizit modellieren.

---

## 121. Cache Maintenance

Hardwarespezifische Cache Maintenance SOLL vollständig im Backend, Treiber oder Memory Manager gekapselt sein.

---

## 122. Cache Lines

Die öffentliche API SOLL nicht von einer festen Cache-Line-Größe abhängig sein.

---

## 123. NUMA

Die Runtime MUSS NUMA-Systeme modellieren können.

Memory Domains SOLLEN einem NUMA Node zugeordnet werden können.

---

## 124. NUMA Local Allocation

CPU Compute Memory SOLL möglichst auf dem NUMA Node allokiert werden, auf dem die ausführenden Worker laufen.

---

## 125. NUMA Migration

Die Runtime DARF Speicher zwischen NUMA Nodes migrieren, wenn sich die Zugriffslokalität dauerhaft ändert.

---

## 126. NUMA Cost

Der Compute Scheduler SOLL Remote-Memory-Zugriffe als zusätzlichen Kostenfaktor berücksichtigen können.

---

## 127. Multi-GPU

Jede diskrete GPU SOLL ihren eigenen Memory Domain besitzen können.

Beispiel:

```text
GPU0_VRAM
GPU1_VRAM
GPU2_VRAM
```

---

## 128. Replication

Read-mostly Data DARF auf mehreren GPUs repliziert werden.

---

## 129. Replication Cost

Die Runtime MUSS Replikationskosten gegen erwartete Zugriffsvorteile abwägen können.

---

## 130. Writable Replicas

Mehrere schreibbare Kopien derselben Resource DÜRFEN nur mit definierter Konsistenzstrategie existieren.

---

## 131. Copy-on-Write

Die Runtime DARF Copy-on-Write für geeignete Compute Resources unterstützen.

---

## 132. Sparse Resources

Die Runtime SOLL langfristig Sparse Resources unterstützen können.

Dabei kann der logische Adressraum größer als die physisch belegte Speichermenge sein.

---

## 133. Sparse Mapping

Teilbereiche einer Resource können unterschiedlichen physischen Pages zugeordnet werden.

---

## 134. Sparse Residency

Nicht verwendete Teile einer großen Resource MÜSSEN nicht zwingend resident sein.

---

## 135. Virtual Memory

Compute Devices mit eigenem virtuellen Speicher SOLLEN in das NovaOS-Compute-Memory-Modell integriert werden.

---

## 136. Device Page Tables

Device Page Tables sind Backend- oder Treiberdetails.

Die Compute Runtime MUSS jedoch sichere Mapping- und Isolationseigenschaften verlangen.

---

## 137. Shared Page Tables

Wenn Hardware CPU und Device gemeinsame Page Tables unterstützt, DARF NovaOS diese verwenden.

---

## 138. Page Size

Die Runtime MUSS unterschiedliche Page Sizes unterstützen können.

Beispiele:

```text
4 KiB
64 KiB
2 MiB
```

Die öffentliche API DARF nicht von einer einzelnen Page Size abhängig sein.

---

## 139. Huge Pages

Große Compute Allocations DÜRFEN Huge Pages verwenden, wenn dies Performance verbessert.

---

## 140. Fragmentation

Der Memory Manager SOLL interne und externe Fragmentierung minimieren.

---

## 141. Suballocation

Große Device Memory Blocks DÜRFEN intern in kleinere Compute Allocations unterteilt werden.

---

## 142. Memory Pools

Anwendungen oder die Runtime DÜRFEN Memory Pools verwenden.

Vorteile:

- geringerer Allocation Overhead,
- weniger Fragmentierung,
- bessere Wiederverwendung.

---

## 143. Pool Types

Mögliche Pools:

```text
HOST_POOL
DEVICE_POOL
UPLOAD_POOL
DOWNLOAD_POOL
TEMPORARY_POOL
```

---

## 144. Temporary Resources

Kurzlebige Resources SOLLEN besonders günstig verwaltet werden.

Die Runtime DARF Ring Buffer, Arenas oder Transient Pools verwenden.

---

## 145. Alias Memory

Nicht gleichzeitig verwendete temporäre Resources DÜRFEN denselben physischen Speicher verwenden.

Dies wird als Aliasing bezeichnet.

---

## 146. Alias Safety

Aliasing DARF nur erfolgen, wenn die Lebenszeiten der Resources nicht überlappen.

---

## 147. Lifetime Analysis

Compute Graphs DÜRFEN Resource Lifetimes analysieren und Speicher automatisch wiederverwenden.

---

## 148. Defragmentation

Die Runtime DARF Device Memory defragmentieren, wenn Ressourcen sicher verschiebbar sind.

---

## 149. Defragmentation Restrictions

Nicht verschiebbare oder aktiv verwendete Ressourcen DÜRFEN nicht unkontrolliert umplatziert werden.

---

## 150. Memory Compaction

Die Runtime DARF freien Speicher durch Migration zusammenfassen.

---

## 151. Persistent Resources

Langfristig benötigte Resources SOLLEN als persistent markiert werden können.

---

## 152. Persistent Device Memory

`PERSISTENT` bedeutet nicht, dass Daten einen Neustart überleben.

Es beschreibt die erwartete Runtime-Lebensdauer.

---

## 153. Data Persistence

Nichtflüchtiger Compute Memory wird durch diese Version der Spezifikation nicht vorausgesetzt.

---

## 154. Memory Initialization

Neu allokierter Compute Memory MUSS hinsichtlich Informationssicherheit definiert behandelt werden.

Er DARF keine fremden Prozessdaten offenlegen.

---

## 155. Zero Initialization

NovaOS DARF neue Compute Resources logisch mit Null initialisieren.

Alternativ DARF die Runtime sicherstellen, dass alte Daten nicht lesbar sind.

---

## 156. Secure Reuse

Vor Wiederverwendung einer physischen Speicherregion durch einen anderen Security Context MUSS verhindert werden, dass alte Daten sichtbar bleiben.

---

## 157. Secure Free

Sicherheitskritische Resources SOLLEN optional explizit gelöscht werden können.

Beispiel:

```text
SECURE_ERASE
```

---

## 158. Secure Erase Limitations

Bei Hardware mit Caches, Wear-Leveling oder proprietärer Speicherverwaltung MUSS dokumentiert werden, welche Garantien tatsächlich möglich sind.

---

## 159. Encryption

Die Runtime DARF verschlüsselten Compute Memory unterstützen.

Dies ist jedoch keine Mindestanforderung dieser Spezifikation.

---

## 160. Protected Compute Memory

Spezielle Devices DÜRFEN geschützten oder vertraulichen Compute Memory bereitstellen.

Dieser MUSS als eigene Capability modelliert werden.

---

## 161. Memory Faults

Mindestens folgende Fehler MÜSSEN unterscheidbar sein:

```text
OUT_OF_MEMORY
INVALID_RESOURCE
INVALID_MAPPING
ACCESS_DENIED
OUT_OF_BOUNDS
DEVICE_MEMORY_FAULT
PAGE_FAULT_FAILED
MIGRATION_FAILED
TRANSFER_FAILED
RESOURCE_LOST
```

---

## 162. Out-of-Memory

Eine Allocation MUSS kontrolliert fehlschlagen können.

Out-of-Memory DARF nicht automatisch zu Kernel-Panic oder Systemabsturz führen.

---

## 163. Retry nach OOM

Die Runtime DARF vor einem endgültigen OOM:

1. Caches freigeben,
2. evictable Resources verschieben,
3. temporäre Resources freigeben,
4. alternative Memory Domains prüfen.

---

## 164. Resource Lost

Eine Resource kann verloren gehen, wenn ihr Device irreparabel ausfällt.

Zustand:

```text
RESOURCE_LOST
```

---

## 165. Recoverable Resource

Resources DÜRFEN als regenerierbar markiert werden.

Beispiel:

```text
RECREATABLE
```

Die Runtime oder Anwendung kann sie nach Device Recovery erneut erzeugen.

---

## 166. Shadow Copy

Für kritische Resources DARF eine Host Shadow Copy gehalten werden.

Beispiel:

```text
GPU Copy
+
Host Backup
```

Dies erhöht Speicherverbrauch und ist daher nicht standardmäßig verpflichtend.

---

## 167. Checkpointing

Lange Compute Jobs DÜRFEN Daten-Checkpoints erzeugen.

Diese können bei Device Failure für Recovery verwendet werden.

---

## 168. Device Removal

Vor geplantem Hot-Unplug SOLLEN relevante Resources:

- migriert,
- synchronisiert,
- invalidiert

werden können.

---

## 169. Device Lost

Bei ungeplantem Device Loss MUSS die Runtime bestimmen:

```text
Which Resources Survived?
Which Resources Are Lost?
Which Resources Have Host Copies?
```

---

## 170. Suspend

Vor Suspend MUSS für Device Memory entschieden werden, ob es:

```text
PRESERVE
MIGRATE
DISCARD
CHECKPOINT
```

wird.

---

## 171. Resume

Nach Resume MUSS die Runtime Device Memory Residency erneut validieren.

---

## 172. Hibernation

Die vollständige Persistenz großer Compute Device Memories ist nicht zwingend erforderlich.

Resources DÜRFEN nach Resume neu erzeugt werden, wenn sie als regenerierbar markiert sind.

---

## 173. Memory Telemetrie

Die Runtime MUSS grundlegende Speichermetriken bereitstellen können.

Mindestens:

```text
AllocatedBytes
ResidentBytes
MappedBytes
PinnedBytes
EvictedBytes
TransferredBytes
MigrationCount
AllocationFailures
```

---

## 174. Device Memory Metrics

Pro Device SOLLEN verfügbar sein:

```text
PhysicalCapacity
RuntimeBudget
Used
Free
Reserved
Evictable
Pinned
```

---

## 175. Transfer Metrics

Die Runtime SOLL messen können:

```text
HostToDeviceBytes
DeviceToHostBytes
DeviceToDeviceBytes
TransferLatency
TransferBandwidth
```

---

## 176. Residency Metrics

Mögliche Metriken:

```text
ResidencyHitRate
MigrationRate
EvictionRate
PrefetchHitRate
```

---

## 177. Debugging

Debug Builds SOLLEN zusätzliche Memory Validation bereitstellen können.

Beispiele:

- Bounds Checks,
- Double-Free Detection,
- Invalid Handle Detection,
- Use-After-Free Detection,
- Mapping Validation,
- Race Diagnostics.

---

## 178. Memory Leak Detection

Entwicklerwerkzeuge SOLLEN nicht freigegebene Compute Resources erkennen können.

---

## 179. Allocation Tracing

Optional SOLL jede Allocation mit folgenden Daten protokollierbar sein:

```text
ResourceID
Size
MemoryClass
OwnerContext
Timestamp
CallSite
```

---

## 180. Performance Mode

In Release Builds DÜRFEN teure Debug-Prüfungen reduziert werden.

Sicherheitsrelevante Grenzen MÜSSEN erhalten bleiben.

---

## 181. Scheduler-Integration

Der Compute Scheduler MUSS auf Memory-Informationen zugreifen können.

Mindestens:

```text
ResourceResidency
TransferCost
AvailableMemory
MemoryPressure
Topology
```

---

## 182. Scheduling-Beispiel

Gegeben:

```text
Input Resource = GPU0 Resident
Output Resource = GPU0 Resident
```

Devices:

```text
GPU0 = Moderate Load
GPU1 = Idle
```

Trotz niedrigerer Auslastung von GPU1 kann GPU0 günstiger sein, wenn die Transfers zu GPU1 mehr kosten als die zusätzliche Queue-Wartezeit.

---

## 183. Power-Integration

Memory Transfers verursachen Energieverbrauch.

Die Runtime DARF in energieeffizienten Policies unnötige Migrationen vermeiden.

---

## 184. Thermal Integration

Hohe Speicherbandbreite kann thermische Last erzeugen.

Der Scheduler DARF Memory-intensive Jobs entsprechend verteilen.

---

## 185. Compression

Die Runtime DARF Speicherkompression für evictable oder temporäre Resources verwenden.

Dies ist optional.

---

## 186. Compressed Residency

Ein komprimiertes Objekt MUSS als eigener Residency-Zustand darstellbar sein.

Beispiel:

```text
COMPRESSED_HOST
```

---

## 187. Compression Cost

Kompression DARF nur verwendet werden, wenn:

```text
CompressionCost
+
DecompressionCost
```

gegenüber Speicher- oder Transferersparnis sinnvoll ist.

---

## 188. Deduplication

Read-only Resources mit identischem Inhalt DÜRFEN intern dedupliziert werden.

Dies ist optional.

---

## 189. Copy-on-Write bei Deduplication

Wird eine deduplizierte Resource beschreibbar, MUSS die Runtime eine getrennte Kopie erzeugen.

---

## 190. Memory API Thread Safety

Speicheroperationen der öffentlichen Compute Runtime MÜSSEN Thread-safe sein.

Mehrere Threads DÜRFEN gleichzeitig:

- Resources erzeugen,
- Resources freigeben,
- Mappings erstellen,
- Transfers einreichen.

---

## 191. Concurrency

Der Memory Manager MUSS konkurrierende Zugriffe korrekt synchronisieren.

---

## 192. Allocation Fast Path

Für häufige kleine Allocations SOLL ein Fast Path existieren können.

Beispiele:

- Pool Allocation,
- slab-ähnliche Strukturen,
- per-Context Caches.

---

## 193. Large Allocation Path

Große Allocations DÜRFEN über einen separaten Pfad behandelt werden.

---

## 194. Allocation Caching

Freigegebene interne Blöcke DÜRFEN für spätere Allocations wiederverwendet werden.

Vor Cross-Context-Reuse MUSS die Sicherheitsbereinigung gewährleistet sein.

---

## 195. Fragmentation Metrics

Die Runtime SOLL Fragmentierung diagnostizieren können.

Mögliche Werte:

```text
LargestFreeBlock
TotalFreeMemory
FragmentationRatio
```

---

## 196. Memory Priority Inheritance

Wenn ein hoch priorisierter Job auf eine niedrig priorisierte Resource-Migration wartet, DARF diese Migration temporär höher priorisiert werden.

---

## 197. Transfer Priority

Transfers SOLLEN Prioritäten besitzen können.

Beispiel:

```text
INTERACTIVE_UPLOAD
BACKGROUND_PREFETCH
```

---

## 198. Prefetch Cancellation

Ein noch nicht benötigter Prefetch DARF verworfen werden, wenn höher priorisierte Transfers Ressourcen benötigen.

---

## 199. Read-Mostly Optimization

Eine Resource mit:

```text
READ_MOSTLY
```

DARF auf mehreren Devices repliziert werden.

---

## 200. Write-Mostly Optimization

Eine `WRITE_MOSTLY` Resource SOLL bevorzugt nahe dem primären Schreib-Device gehalten werden.

---

## 201. Streaming Resources

`STREAMING` Resources SOLLEN für sequentielle einmalige Verarbeitung optimiert werden können.

---

## 202. Random Access Resources

`RANDOM_ACCESS` Resources SOLLEN anders geprefetcht und gecacht werden können als Streaming Resources.

---

## 203. Memory Model und Compute Graph

Compute Graphs SOLLEN die Speicherlebensdauer und Datenbewegung vorausplanen können.

Beispiel:

```text
A -> Buffer1 -> B -> Buffer2 -> C
```

Die Runtime kann frühzeitig feststellen:

```text
Buffer1 wird nach B nicht mehr benötigt.
```

Der Speicher kann anschließend wiederverwendet werden.

---

## 204. Graph Resource Planning

Vor Graph-Ausführung DARF die Runtime:

- Aliasing planen,
- Migration planen,
- Prefetch planen,
- temporäre Pools reservieren.

---

## 205. Memory Fusion

Wenn zwei aufeinanderfolgende Compute-Schritte Daten nicht außerhalb ihrer Pipeline benötigen, DARF die Runtime Zwischenkopien vermeiden.

---

## 206. Lazy Allocation

Eine logische Resource DARF erstellt werden, ohne sofort physischen Speicher zu reservieren.

Physische Allocation kann bei erster Nutzung erfolgen.

---

## 207. Lazy Commitment

Bei großen Resources DARF physischer Speicher erst bei tatsächlicher Nutzung committed werden.

---

## 208. Commitment Limits

Logisch reservierter Speicher DARF Systemlimits nicht unkontrolliert umgehen.

---

## 209. Memory Overcommit Accounting

Überbuchter Speicher MUSS separat bilanziert werden.

---

## 210. Import externer Speicher

Die Runtime SOLL externe Speicherobjekte importieren können.

Beispiele:

- Grafikbuffer,
- Videobuffer,
- gemeinsam genutzter IPC-Speicher.

---

## 211. Export von Compute Memory

Compute Memory SOLL exportierbar sein, wenn:

- Resource Type kompatibel ist,
- Backend dies unterstützt,
- Sicherheitsrichtlinien es erlauben.

---

## 212. External Ownership

Bei importierten Resources MUSS klar definiert sein, welches Subsystem deren Lebensdauer kontrolliert.

---

## 213. Synchronisation bei Interop

Zwischen Grafik, Video, AI und Compute MUSS neben Resource Sharing auch Synchronisation geteilt werden können.

Beispiel:

```text
Graphics Fence
      |
      v
Compute Wait
```

---

## 214. Resource State

Eine Resource DARF logische Nutzungszustände besitzen.

Beispiele:

```text
HOST_READ
HOST_WRITE
COMPUTE_READ
COMPUTE_WRITE
TRANSFER_SOURCE
TRANSFER_DESTINATION
```

---

## 215. State Transition

Die Runtime MUSS erforderliche Zustandsübergänge durchführen.

Beispiel:

```text
COMPUTE_WRITE
     |
     v
TRANSFER_SOURCE
```

---

## 216. Implicit vs Explicit Transitions

Die öffentliche API DARF sowohl:

- automatische,
- explizite

Resource Transitions unterstützen.

Automatische Modi erleichtern normale Anwendungen.

Explizite Modi ermöglichen maximale Kontrolle.

---

## 217. Safe Defaults

Wenn eine Anwendung keine erweiterten Memory Hints setzt, MUSS die Runtime sichere und semantisch korrekte Defaults verwenden.

---

## 218. Performance Hints

Performance Hints DÜRFEN niemals Sicherheitsprüfungen deaktivieren.

---

## 219. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens bereitstellen:

```text
HOST Memory
CPU DEVICE Abstraction
Compute Buffer
Allocation
Free
Mapping
Unmapping
Copy
Resource Ownership
Context Isolation
Memory Barriers
Error Handling
```

---

## 220. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich bereitstellen:

```text
Dedicated Device Memory
Unified Memory
Zero-Copy
Memory Migration
Eviction
Oversubscription
NUMA Awareness
Multi-GPU Residency
Peer Transfers
Sparse Resources
Memory Pools
Resource Sharing
Graphics Interop
Video Interop
AI Interop
Prefetch
Memory Advice
Adaptive Residency
```

---

## 221. Testanforderungen — Allocation

Zu testen sind mindestens:

```text
Small Allocation
Large Allocation
Zero Size
Invalid Alignment
Out-of-Memory
Repeated Allocation
Repeated Free
```

---

## 222. Testanforderungen — Mapping

Zu testen sind:

```text
Read Mapping
Write Mapping
Read-Write Mapping
Invalid Mapping
Double Mapping
Unmap without Map
Persistent Mapping
```

---

## 223. Testanforderungen — Transfer

Zu testen sind:

```text
Host -> Device
Device -> Host
Device -> Device
Zero-Length Copy
Large Copy
Overlapping Regions
Async Copy
```

---

## 224. Testanforderungen — Residency

Zu testen sind:

```text
Host Residency
Device Residency
Migration
Multi Residency
Eviction
Re-Residency
```

---

## 225. Testanforderungen — Unified Memory

Zu testen sind:

- CPU Write → Device Read,
- Device Write → CPU Read,
- Device Write → Device Read,
- Migration,
- Cache Visibility,
- Page Fault Handling.

---

## 226. Testanforderungen — Synchronisation

Mindestens:

```text
Acquire
Release
Acquire-Release
Memory Barrier
Cross-Queue Visibility
Cross-Device Visibility
```

---

## 227. Testanforderungen — Security

Zu testen sind:

- Cross-Context Access,
- stale Handles,
- Use-After-Free,
- Out-of-Bounds,
- Residual Data Leakage,
- unauthorized Resource Import,
- unauthorized Mapping.

---

## 228. Testanforderungen — Memory Pressure

Zu testen sind:

```text
LOW
MODERATE
HIGH
CRITICAL
```

und die jeweiligen Eviction- und Allocation-Reaktionen.

---

## 229. Testanforderungen — Multi-Device

Mindestens:

```text
CPU + GPU
GPU0 + GPU1
GPU + NPU
```

mit:

- Migration,
- Replication,
- Peer Copy,
- Fallback über Host.

---

## 230. Testanforderungen — Device Lost

Zu testen sind:

```text
Device Lost with no Resources
Device Lost with Read-Only Resources
Device Lost with Dirty Resources
Device Reset
Recovery
RESOURCE_LOST
```

---

## 231. Stress Tests

Stress Tests SOLLEN beinhalten:

```text
Millions of Small Allocations
Repeated Map/Unmap
Continuous Migration
Continuous Eviction
Multiple Processes
Multiple Devices
Memory Pressure
Device Reset under Load
```

---

## 232. Fuzzing

Folgende Strukturen SOLLTEN fuzz-getestet werden:

```text
ResourceDescriptor
MemoryFlags
MappingDescriptor
TransferDescriptor
ImportDescriptor
ExportDescriptor
```

---

## 233. Fault Injection

Die Testumgebung SOLL mindestens folgende Fehler simulieren können:

```text
OUT_OF_MEMORY
TRANSFER_FAILED
MIGRATION_FAILED
PAGE_FAULT_FAILED
DEVICE_LOST
RESOURCE_CORRUPTED
```

---

## 234. Performance Tests

Mindestens folgende Werte SOLLEN gemessen werden:

```text
AllocationLatency
FreeLatency
MapLatency
UnmapLatency
TransferBandwidth
TransferLatency
MigrationLatency
ResidencyHitRate
```

---

## 235. Regression Tests

Änderungen am Memory Manager SOLLEN hinsichtlich mindestens folgender Größen verglichen werden:

```text
MemoryUsage
Fragmentation
TransferVolume
MigrationCount
AllocationLatency
PeakResidentMemory
```

---

## 236. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-RUNTIME-0003` MUSS:

1. Compute Resources abstrahieren.
2. Buffer-Ressourcen unterstützen.
3. Resource Ownership verwalten.
4. Context-Isolation erzwingen.
5. Speicher allokieren und freigeben können.
6. Host Mapping bereitstellen können.
7. Host-to-Device-Transfers modellieren können.
8. Device-to-Host-Transfers modellieren können.
9. Device-to-Device-Transfers modellieren können.
10. Resource-Zugriffsrechte erzwingen.
11. Speicherbarrieren unterstützen.
12. Cache- und Visibility-Anforderungen korrekt behandeln.
13. Out-of-Memory kontrolliert behandeln.
14. Ressourcen bei Prozessende freigeben.
15. Use-After-Free verhindern.
16. ungültige Cross-Context-Zugriffe verhindern.
17. Memory Budgets ermöglichen.
18. Residency-Zustände verwalten können.
19. Memory-Informationen dem Scheduler bereitstellen.
20. zukünftige Memory Domains erweiterbar halten.

---

## 237. Beziehung zum Compute Scheduler

`NPSPEC-COMPUTE-RUNTIME-0002` verwendet Informationen des Memory Models für Scheduling-Entscheidungen.

```text
Compute Scheduler
      |
      +--> Resource Residency
      |
      +--> Transfer Cost
      |
      +--> Memory Pressure
      |
      +--> Device Memory Availability
      |
      +--> NUMA Locality
      |
      v
Device Selection
```

---

## 238. Beziehung zum NovaOS Memory Manager

Die Compute Runtime baut auf dem allgemeinen NovaOS Memory Management auf.

```text
Compute Memory Model
        |
        v
Compute Backend
        |
        v
NovaOS VMM / PMM / DMA
        |
        v
Physical Memory / Device Memory
```

Der allgemeine Memory Manager verwaltet fundamentale Speichermechanismen.

Das Compute Memory Model ergänzt diese um heterogene Device-Semantik.

---

## 239. Empfohlene Folge-Spezifikationen

Auf dieser Spezifikation SOLLEN weitere Detail-Spezifikationen aufbauen können.

Empfohlen:

```text
NPSPEC-COMPUTE-MEMORY-ALLOC-0001
Compute Allocation

NPSPEC-COMPUTE-MEMORY-BUFFER-0001
Compute Buffer

NPSPEC-COMPUTE-MEMORY-MAP-0001
Memory Mapping

NPSPEC-COMPUTE-MEMORY-TRANSFER-0001
Compute Transfers

NPSPEC-COMPUTE-MEMORY-RESIDENCY-0001
Memory Residency

NPSPEC-COMPUTE-MEMORY-MIGRATE-0001
Memory Migration

NPSPEC-COMPUTE-MEMORY-UNIFIED-0001
Unified Memory

NPSPEC-COMPUTE-MEMORY-ZEROCOPY-0001
Zero-Copy Memory

NPSPEC-COMPUTE-MEMORY-NUMA-0001
NUMA Memory

NPSPEC-COMPUTE-MEMORY-PEER-0001
Peer Device Memory

NPSPEC-COMPUTE-MEMORY-SPARSE-0001
Sparse Resources

NPSPEC-COMPUTE-MEMORY-POOL-0001
Memory Pools

NPSPEC-COMPUTE-MEMORY-SYNC-0001
Memory Synchronization

NPSPEC-COMPUTE-MEMORY-SHARE-0001
Resource Sharing

NPSPEC-COMPUTE-MEMORY-SECURITY-0001
Compute Memory Security

NPSPEC-COMPUTE-MEMORY-TEST-0001
Compute Memory Conformance Tests
```

---

## 240. Zusammenfassung

Das NovaOS Compute Memory Model stellt eine einheitliche Speicherabstraktion für heterogene Recheneinheiten bereit.

Die logische Struktur lautet:

```text
Application
    |
    v
Compute Resource
    |
    v
Compute Memory Model
    |
    +-- Allocation
    |
    +-- Mapping
    |
    +-- Residency
    |
    +-- Migration
    |
    +-- Transfer
    |
    +-- Coherency
    |
    +-- Synchronization
    |
    +-- Protection
    |
    +-- Budgeting
    |
    +-- Eviction
    |
    v
Memory Domains
    |
    +-- Host RAM
    |
    +-- GPU VRAM
    |
    +-- NPU Memory
    |
    +-- Shared Memory
    |
    +-- Future Memory
```

Anwendungen arbeiten primär mit logischen Ressourcen.

Die Runtime entscheidet abhängig von:

```text
Usage
Access Pattern
Current Device
Future Device
Data Locality
Transfer Cost
Memory Pressure
NUMA Topology
Power Policy
Security
```

wo eine Resource physisch gespeichert wird.

Dadurch kann dasselbe Compute-Programm auf sehr unterschiedlichen Speicherarchitekturen ausgeführt werden.

Beispiele:

```text
Diskrete GPU:

CPU RAM
   |
   v
GPU VRAM
```

```text
Integrated GPU:

CPU + GPU
    |
    v
Shared RAM
```

```text
AI SoC:

CPU
 |
Shared DRAM
 |
 +--> GPU
 |
 +--> NPU
```

```text
Multi-GPU:

           +--> GPU0 VRAM
           |
Resource --+
           |
           +--> GPU1 VRAM
```

Das grundlegende Architekturprinzip lautet:

> Compute Memory ist in NovaOS eine logische Ressource mit kontrollierter Residency, Sichtbarkeit und Ownership. Physische Speicherplatzierung und Datenbewegung werden so weit wie möglich durch die Runtime optimiert, ohne Sicherheit, Synchronisation oder determinierte Semantik zu verletzen.