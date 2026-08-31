# ADR-HAL-0006 – DMA und IOMMU als kontrollierte Systemressourcen

## Status

Angenommen

## Kategorie

HAL / DMA / IOMMU / Sicherheit / Ressourcenverwaltung / Isolation / Zero-Copy

## Kontext

Direct Memory Access (DMA) ermöglicht Geräten den direkten Zugriff auf Speicher, ohne dass die CPU jede Übertragung einzeln durchführen muss.

DMA ist essenziell für leistungsfähige I/O-Pfade wie:

```text
Storage
Network
GPU
NPU
Audio
Video
High-Speed Devices
```

Gleichzeitig ist DMA sicherheitskritisch.

Ein Gerät mit unkontrolliertem DMA-Zugriff könnte prinzipiell:

- beliebige physische Speicherbereiche lesen,
- Speicher verändern,
- Kernelstrukturen beschädigen,
- Prozessisolation umgehen,
- vertrauliche Daten auslesen,
- andere Geräte oder Buffer beeinflussen.

Eine IOMMU kann diesen Zugriff kontrollieren, indem Geräten eigene I/O-Adressräume und explizite Mappings zugewiesen werden.

NovaOS darf DMA deshalb nicht als bloßes Treiberdetail betrachten.

DMA-Fähigkeiten, DMA-Mappings, IOMMU-Domains und Geräteadressräume werden als kontrollierte Systemressourcen behandelt.

---

## Entscheidung

NovaOS verwaltet DMA und IOMMU über zentrale, explizite HAL- und Kernelmechanismen.

Das Grundmodell lautet:

```text
Driver / Device Service
        │
        ▼
DMA Request
        │
        ▼
DMA Manager
        │
        ├── Resource Validation
        ├── Ownership Validation
        ├── Security Validation
        ├── IOMMU Mapping
        └── Cache / Coherency Handling
                │
                ▼
              Device
```

Ein Treiber darf nicht standardmäßig beliebige physische Adressen direkt an Geräte weitergeben.

---

## Grundprinzip

Es gilt:

```text
DMA is authority over memory.
```

und daher:

```text
DMA access must be granted,
scoped,
tracked
and revocable.
```

Eine DMA-Adresse ist nicht nur eine technische Zahl.

Sie repräsentiert eine kontrollierte Zugriffsbeziehung zwischen:

```text
Device
↔
Memory
```

---

## DMA als Systemressource

DMA wird als knappe und sicherheitsrelevante Systemressource behandelt.

Relevante Ressourcen können umfassen:

```text
DMA Address Space
DMA Mapping
Pinned Memory
Bounce Buffer
IOMMU Domain
IOTLB Entries
Device Queue Memory
DMA Bandwidth
```

Diese Ressourcen müssen systemweit verwaltbar und zurechenbar sein.

---

## DMA Authority

Ein Gerät erhält nur Zugriff auf Speicherbereiche, die explizit für seine DMA-Operation freigegeben wurden.

Nicht:

```text
Device
  ↓
Physical Memory
```

sondern:

```text
Device
  ↓
DMA Mapping
  ↓
Authorized Memory Region
```

---

## DMA Handle

Treiber sollen bevorzugt mit einem abstrahierten DMA-Objekt arbeiten.

Beispiel:

```text
DmaMapping {
    device
    direction
    length
    deviceAddress
    memoryObject
    permissions
    lifetime
}
```

Die konkrete physische Adresse muss dadurch nicht Bestandteil höherer Treiberlogik sein.

---

## DMA-Richtungen

DMA-Mappings müssen die Richtung ausdrücken können.

Beispiele:

```text
DeviceReadsMemory
DeviceWritesMemory
Bidirectional
```

Dadurch können Berechtigungen und Cache-Synchronisation präziser erfolgen.

---

## Least Privilege

Ein Gerät soll nur:

- auf benötigte Buffer,
- mit benötigten Rechten,
- für benötigte Dauer

zugreifen können.

Beispiel:

```text
Read-only DMA buffer
```

soll nicht automatisch Schreibzugriff erhalten.

---

## DMA-Lebensdauer

DMA-Mappings müssen eine definierte Lebensdauer besitzen.

Beispiel:

```text
Create Mapping
      ↓
Device Operation
      ↓
Completion
      ↓
Unmap
```

Ein Mapping darf nicht unbegrenzt bestehen bleiben, wenn es nicht mehr benötigt wird.

---

## Ownership

DMA-Mappings müssen mit dem Ownership-Modell von NovaOS kompatibel sein.

Ein Memory Object darf nicht freigegeben oder anderweitig wiederverwendet werden, solange ein aktives DMA-Mapping darauf existiert.

---

## Pinning

Manche DMA-Szenarien erfordern gepinnten Speicher.

Pinned Memory ist eine kontrollierte Ressource.

Unbegrenztes Pinning darf nicht möglich sein, da es:

- Memory Reclamation verhindert,
- Fragmentierung erhöhen kann,
- Migration erschwert,
- NUMA-Placement beeinflusst.

Pinned Memory muss daher in Resource Economy berücksichtigt werden.

---

## IOMMU

Wenn eine IOMMU vorhanden und nutzbar ist, soll NovaOS sie als primären Mechanismus zur DMA-Isolation verwenden.

Das Modell lautet:

```text
Device
    ↓
IOMMU Domain
    ↓
I/O Virtual Address Space
    ↓
Explicit Mappings
    ↓
Physical Memory
```

Damit kann ein Gerät nur auf freigegebene Speicherbereiche zugreifen.

---

## IOMMU Domains

Geräte werden IOMMU-Domains zugeordnet.

Eine Domain beschreibt mindestens:

```text
Devices
Address Space
Mappings
Permissions
Isolation Properties
```

Geräte dürfen nur dann dieselbe Domain teilen, wenn dies sicher und architektonisch zulässig ist.

---

## IOMMU Groups

Hardware kann mehrere Geräte so koppeln, dass sie nicht zuverlässig voneinander isoliert werden können.

Solche Gruppen müssen explizit dargestellt werden.

Beispiel:

```text
IOMMU Group 7
├── Device A
└── Device B
```

NovaOS darf keine stärkere Isolation behaupten als die Hardware tatsächlich bereitstellt.

---

## Device Identity

DMA-Autorisierung muss an eine stabile Geräteidentität gebunden werden.

Ein Mapping darf nicht versehentlich für ein anderes Gerät wiederverwendet werden.

---

## Address Spaces

DMA muss nicht zwingend physische CPU-Adressen verwenden.

Ein Gerät kann einen eigenen I/O Virtual Address Space erhalten.

Beispiel:

```text
CPU Virtual Address
    !=
Physical Address
    !=
Device DMA Address
```

Diese Adressräume müssen logisch getrennt behandelt werden.

---

## Scatter/Gather

NovaOS unterstützt Scatter/Gather DMA.

Ein logisch zusammenhängender Buffer kann aus mehreren physischen Bereichen bestehen.

Beispiel:

```text
Logical Buffer
    ↓
Segment 0
Segment 1
Segment 2
```

Die DMA-Infrastruktur erstellt daraus eine geeignete Gerätebeschreibung.

---

## DMA Constraints

Geräte können Einschränkungen besitzen.

Beispiele:

```text
32-bit DMA address limit
alignment requirement
maximum segment size
segment count limit
boundary restriction
```

Diese Constraints müssen durch das Device- oder DMA-Modell beschrieben werden können.

---

## Bounce Buffer

Kann ein Gerät den ursprünglichen Speicherbereich nicht direkt adressieren, darf NovaOS einen Bounce Buffer verwenden.

Beispiel:

```text
Original Buffer
      ↓
Bounce Buffer
      ↓
Device
```

Dies erzeugt eine Kopie und ist deshalb kein Zero-Copy-Pfad.

Der Einsatz muss sichtbar und als Kostenfaktor modellierbar sein.

---

## Zero-Copy

DMA ist ein wichtiger Mechanismus für Zero-Copy.

Ein idealer Pfad kann beispielsweise sein:

```text
Storage Device
      ↓ DMA
Shared Buffer
      ↓
Application / Pipeline
```

ohne zusätzliche Zwischenkopie.

Zero-Copy darf jedoch niemals:

- Ownership,
- Security,
- IOMMU-Isolation,
- Cache-Coherency,
- Data Sovereignty

umgehen.

---

## Coherency

Nicht jede Architektur und jedes Gerät besitzt vollständig cache-kohärentes DMA.

Das DMA-Modell muss deshalb ausdrücken können:

```text
Coherent
NonCoherent
PartiallyCoherent
```

Bei nicht kohärentem DMA sind explizite Synchronisationsoperationen erforderlich.

---

## Cache Synchronisation

Die HAL muss geeignete Mechanismen bereitstellen, beispielsweise:

```text
SyncForDevice
SyncForCpu
Flush
Invalidate
```

Die genaue Implementierung ist architekturspezifisch.

Treiber sollen keine unbelegten Annahmen über Cache-Coherency treffen.

---

## Memory Ordering

DMA-Operationen benötigen definierte Ordering-Regeln.

Beispiel:

```text
prepare buffer
    ↓
memory barrier
    ↓
submit descriptor
```

oder:

```text
device completion
    ↓
I/O barrier
    ↓
consume buffer
```

Die HAL muss geeignete Barrier-Primitive bereitstellen.

---

## NUMA

DMA Allocation soll `ADR-HAL-0005_NUMA_als_First_Class_Topologie` berücksichtigen können.

Beispiel:

```text
NIC
  │ nearest-to
  ▼
NUMA1
```

Daraus kann folgen:

```text
Prefer DMA buffers on NUMA1
```

sofern keine wichtigeren Constraints entgegenstehen.

---

## Geräte-Lokalität

Neben NUMA können auch andere Hardwarebeziehungen relevant sein:

```text
PCIe Root Complex
Memory Domain
Accelerator Memory
CXL Link
```

DMA-Planung darf den Hardware System Graph verwenden.

---

## Resource Economy

DMA-bezogene Ressourcen werden der systemweiten Resource Economy zugänglich gemacht.

Beispiele:

```text
PinnedMemory
IOMMUEntries
DMAQueueMemory
BounceBufferMemory
DMAThroughput
```

Damit können einzelne Geräte oder Anwendungen nicht unbegrenzt Ressourcen binden.

---

## Budgetierung

Execution Contracts oder Systempolicy dürfen DMA-relevante Budgets definieren.

Beispielsweise:

```text
MaxPinnedMemory
MaxDmaMappings
MaxQueueMemory
```

Die genaue Contract-Syntax wird nachgelagert spezifiziert.

---

## Isolation von Treibern

User-Mode- und isolierte Treiber dürfen DMA verwenden, ohne dafür vollständigen Zugriff auf physischen Speicher zu erhalten.

Das Modell lautet:

```text
Driver
   ↓
DMA Capability
   ↓
DMA Manager
   ↓
IOMMU Mapping
```

Dies ist eine wichtige Grundlage für starke Treiberisolation.

---

## Capability Enforcement

DMA-Funktionen benötigen explizite Capabilities.

Ein Prozess oder Treiber darf DMA nicht allein deshalb ausführen, weil er ein Device Handle besitzt.

Berechtigungen können getrennt sein für:

```text
DeviceControl
DMARead
DMAWrite
DMAAllocate
DMAMap
```

---

## Keine Autoritätseskalation

Ein Treiber darf durch DMA keine Speicherrechte erhalten, die sein eigener Sicherheitskontext nicht besitzt.

Beispiel:

```text
Driver cannot read MemoryObject X
```

darf nicht durch:

```text
Map X for device DMA
```

umgangen werden.

---

## Shared Buffers

Shared Buffers zwischen:

```text
Application
Driver
Device
```

müssen explizite Ownership- und Zugriffsmodelle besitzen.

Beispiel:

```text
Application: Read
Device: Write
Driver: Control
```

Diese Rechte dürfen getrennt beschrieben werden.

---

## Lifecycle und Revocation

DMA-Zugriff muss widerrufbar sein.

Bei:

```text
Driver crash
Device removal
Process termination
Capability revocation
Security violation
```

müssen zugehörige DMA-Mappings kontrolliert entfernt oder isoliert werden können.

---

## Hotplug

Bei Geräteentfernung muss NovaOS DMA-Zugriffe des Geräts deaktivieren.

Konzeptionell:

```text
Device removal
      ↓
Stop submissions
      ↓
Quiesce device
      ↓
Revoke DMA mappings
      ↓
Release resources
```

Ein physisch entferntes oder fehlerhaftes Gerät darf keine gültige DMA-Autorität behalten.

---

## Driver Crash

Ein Treiberabsturz darf aktive DMA-Operationen nicht unkontrolliert zurücklassen.

Je nach Gerät kann NovaOS:

- Device Reset auslösen,
- DMA blockieren,
- IOMMU Domain invalidieren,
- Buffer quarantänisieren,
- Gerät offline setzen.

---

## Fault Handling

IOMMU Faults müssen explizit behandelt werden.

Beispiele:

```text
UnmappedAccess
PermissionViolation
InvalidAddress
DeviceFault
```

Ein Fault soll mindestens enthalten:

```text
Device
Domain
Address
AccessType
Reason
```

---

## Fault Isolation

Ein fehlerhaftes Gerät soll soweit möglich isoliert werden, ohne das gesamte System zu stoppen.

Beispiel:

```text
DMA violation
      ↓
Block device
      ↓
Revoke mappings
      ↓
Notify driver / resilience
```

Kernel Panic bleibt für Fälle reserviert, in denen Systemintegrität nicht mehr gewährleistet werden kann.

---

## Fail-Safe Defaults

Wenn IOMMU- oder DMA-Eigenschaften unbekannt sind, gilt ein konservativer Standard.

Beispiel:

```text
Unknown DMA capability
      ↓
Do not grant unrestricted DMA
```

oder:

```text
Unknown IOMMU isolation
      ↓
Do not assume device isolation
```

---

## Systeme ohne IOMMU

NovaOS muss auch auf Systemen ohne IOMMU funktionieren können.

Dies bedeutet jedoch nicht, dass dieselbe Isolation garantiert werden kann.

Mögliche Maßnahmen:

```text
restricted driver model
trusted kernel driver
bounce buffers
dedicated memory regions
device-specific constraints
reduced functionality
```

Die reduzierte Sicherheitsgarantie muss explizit sichtbar sein.

---

## No-IOMMU Mode

Ein No-IOMMU-Modus darf nicht so dargestellt werden, als biete er vollständige DMA-Isolation.

Architecture Introspection und Security Policy müssen unterscheiden können:

```text
IOMMU Protected
```

und:

```text
Direct Physical DMA
```

---

## IOMMU als Sicherheitsgrenze

Eine IOMMU kann eine starke technische Isolationsgrenze darstellen.

Sie ersetzt jedoch nicht:

- Device Trust,
- Driver Security,
- Memory Ownership,
- Capability Checks,
- Software Validation.

Sie ist Teil eines mehrschichtigen Sicherheitsmodells.

---

## Trusted Devices

Auch vertrauenswürdige Geräte sollen nach Möglichkeit nur minimal notwendige DMA-Rechte erhalten.

Trust erweitert nicht automatisch die DMA-Autorität.

---

## DMA und Data Sovereignty

DMA-Mappings dürfen Data-Sovereignty- oder Information-Flow-Regeln nicht umgehen.

Ein Gerät darf nur dann Zugriff auf einen Datenbereich erhalten, wenn der resultierende Datenfluss zulässig ist.

Dies ist insbesondere relevant für:

- externe Geräte,
- Netzwerkhardware,
- Accelerators,
- Shared Devices.

---

## DMA und Verschlüsselung

Speicherverschlüsselung oder Memory Encryption kann DMA beeinflussen.

Das DMA-Modell muss zukünftige Mechanismen berücksichtigen können wie:

```text
encrypted memory
shared memory regions
device-accessible decrypted windows
confidential-computing domains
```

Konkrete Verfahren werden separat spezifiziert.

---

## GPU und NPU

GPUs und NPUs können eigene Memory-Management- und DMA-Mechanismen besitzen.

NovaOS soll diese soweit möglich in dasselbe kontrollierte Ressourcenmodell integrieren.

Beispiel:

```text
GPU Virtual Address Space
        ↓
Device Memory Manager
        ↓
IOMMU / Device MMU
```

Gerätespezifische Erweiterungen bleiben erlaubt.

---

## Peer-to-Peer DMA

Geräte können zukünftig direkt untereinander Daten übertragen.

Beispiel:

```text
NVMe
  ↓
GPU
```

oder:

```text
GPU0
  ↓
GPU1
```

Peer-to-Peer DMA muss ebenfalls kontrolliert und autorisiert werden.

Es darf nicht automatisch erlaubt sein, nur weil zwei Geräte technisch miteinander kommunizieren können.

---

## Peer-to-Peer Authority

Für P2P DMA muss explizit geprüft werden:

```text
Source Device
Destination Device
Memory Ownership
IOMMU Topology
Security Policy
Data Sovereignty
```

---

## DMA Domains und Virtualisierung

Virtuelle Maschinen oder isolierte Hardwaredomänen können eigene DMA-Adressräume erhalten.

IOMMU-Domains müssen dafür geeignet partitionierbar sein.

Virtualisierung bleibt jedoch eine Nutzungsmöglichkeit und keine Voraussetzung der DMA-Architektur.

---

## Deterministic Mode

Deterministic Mode kann relevante DMA-Parameter fixieren.

Beispielsweise:

```text
Buffer Placement
Queue Selection
Mapping Layout
Device Provider
```

Adaptive Remapping darf einen Hard Determinism Contract nicht verletzen.

---

## Realtime

Realtime-Geräte können vorab reservierte DMA-Ressourcen benötigen.

Beispiele:

```text
Pinned Buffer Pool
Reserved IOMMU Mappings
Preallocated Descriptor Ring
```

Damit können Laufzeitallokationen und unvorhersehbare Fehler reduziert werden.

---

## Transaktionale Aktivierung

Komplexe DMA-Bindungen sollen atomar vorbereitet werden können.

Beispiel:

```text
Allocate Buffer
      ↓
Pin
      ↓
Create IOMMU Mapping
      ↓
Prepare Device Queue
      ↓
Validate
      ↓
Activate
```

Schlägt ein Schritt fehl, werden vorbereitete Ressourcen zurückgenommen.

---

## Structured Concurrency

DMA-Operationen sollen soweit sinnvoll an Execution Scopes gebunden sein.

Bei Cancellation eines Scopes müssen ausstehende DMA-Operationen definiert behandelt werden.

Nicht jede Hardwareoperation kann sofort abgebrochen werden.

Deshalb sind Zustände nötig wie:

```text
CancellationRequested
InFlight
Completed
Aborted
UnknownOutcome
```

---

## Completion

DMA Completion muss eindeutig einem Auftrag und Buffer zugeordnet werden können.

Ein Buffer darf nicht vor bestätigtem Abschluss wiederverwendet werden, sofern das Device Contract dies nicht ausdrücklich erlaubt.

---

## Architecture Introspection

DMA und IOMMU müssen introspektierbar sein.

Beispiel:

```text
Device:
    NVMe0

IOMMU:
    IOMMU0

Domain:
    Domain17

DMA Isolation:
    Enabled

Active Mappings:
    42

Pinned Memory:
    128 MiB
```

Für einzelne Mappings dürfen sicherheitsabhängig Details gefiltert werden.

---

## Decision Trace

Relevante Entscheidungen sollen erklärbar sein.

Beispiel:

```text
DMA Buffer Placement:
    NUMA1

Reason:
    device local to NUMA1
```

oder:

```text
Zero-Copy rejected

Reason:
    device cannot address source memory

Fallback:
    bounce buffer
```

---

## Audit

Sicherheitsrelevante DMA-Ereignisse sollen auditierbar sein.

Beispiele:

```text
Domain creation
Mapping grant
Mapping revocation
Permission violation
IOMMU fault
Device isolation
```

Auditierung darf den normalen Fast Path nicht unnötig blockieren.

---

## Performance

Die kontrollierte DMA-Architektur darf nicht bedeuten, dass für jedes I/O-Paket eine teure globale Policy-Entscheidung durchgeführt werden muss.

Stattdessen gilt:

```text
Validate and prepare control plane
        ↓
Fast pre-authorized data path
```

Vorbereitete Mapping Handles und Buffer Pools dürfen im Fast Path verwendet werden.

---

## Mapping Cache

Sichere Mapping-Caches dürfen eingesetzt werden, um wiederholte IOMMU-Operationen zu vermeiden.

Ein Cache darf:

- Ownership,
- Permission Changes,
- Revocation,
- Device Lifetime

nicht ignorieren.

---

## Large Transfers

Für große Datenmengen darf NovaOS:

- größere Mappingbereiche,
- Huge I/O Pages,
- Batched Mapping,
- Persistent Mappings

verwenden, wenn Security und Resource Policy dies erlauben.

---

## Evolvierbarkeit

DMA- und IOMMU-Contracts müssen neue Hardwaremechanismen aufnehmen können.

Dazu gehören zukünftig beispielsweise:

```text
PCIe ATS
PASID
PRI
Shared Virtual Addressing
Device Page Faulting
CXL
Advanced Device MMUs
```

Die Architektur darf nicht auf ein ausschließlich klassisches IOVA-Modell festgelegt werden.

---

## Shared Virtual Addressing

NovaOS darf Geräte unterstützen, die einen Adressraum enger mit Prozessen teilen.

Dies darf jedoch nicht zu ungeprüfter Speicherautorität führen.

Auch bei Shared Virtual Addressing müssen:

```text
Identity
Permissions
Lifetime
Fault Handling
```

explizit kontrolliert werden.

---

## KI-Unabhängigkeit

DMA-Sicherheit, Mapping und IOMMU-Konfiguration müssen ohne KI funktionieren.

KI darf unterstützen bei:

- Queue-Tuning,
- Buffer-Größen,
- NUMA Placement,
- Transferprognosen,
- Anomalieerkennung.

KI darf keine DMA-Autorität erzeugen oder IOMMU-Prüfungen ersetzen.

---

## Normative Anforderungen

1. NovaOS MUSS DMA als kontrollierte Systemressource behandeln.
2. DMA DARF NICHT standardmäßig unbeschränkten Zugriff auf physischen Speicher erhalten.
3. DMA-Zugriffe MÜSSEN explizit an Geräte gebunden sein.
4. DMA-Mappings MÜSSEN explizite Speicherbereiche referenzieren.
5. DMA-Mappings MÜSSEN definierte Zugriffsrechte besitzen.
6. DMA-Richtung MUSS ausdrückbar sein.
7. DMA-Mappings MÜSSEN eine definierte Lebensdauer besitzen.
8. Memory Objects DÜRFEN während aktiver DMA-Nutzung nicht unsicher freigegeben werden.
9. Pinned Memory MUSS als kontrollierte Ressource behandelt werden.
10. Resource Economy MUSS DMA-relevante Ressourcen berücksichtigen können.
11. NovaOS SOLL eine verfügbare IOMMU zur DMA-Isolation verwenden.
12. IOMMU-Domains MÜSSEN explizit modellierbar sein.
13. Gerätezuordnungen zu IOMMU-Domains MÜSSEN nachvollziehbar sein.
14. Hardwarebedingte IOMMU-Gruppen MÜSSEN berücksichtigt werden.
15. NovaOS DARF keine stärkere DMA-Isolation behaupten als die Hardware ermöglicht.
16. CPU Virtual Address, Physical Address und Device Address MÜSSEN logisch getrennt bleiben.
17. Scatter/Gather DMA MUSS unterstützt werden können.
18. Gerätebezogene DMA-Adressbeschränkungen MÜSSEN modellierbar sein.
19. Bounce Buffer MÜSSEN als zulässiger Fallback unterstützt werden können.
20. Bounce Buffer MÜSSEN als Datenkopie und Kostenfaktor erkennbar sein.
21. Zero-Copy DARF DMA-Sicherheitsregeln NICHT umgehen.
22. DMA-Coherency MÜSSEN explizit modellierbar sein.
23. Nicht kohärente DMA-Plattformen MÜSSEN Cache-Synchronisation unterstützen.
24. DMA Memory Ordering MUSS korrekt implementiert werden.
25. NUMA-Lokalität SOLL bei DMA Buffer Placement berücksichtigt werden können.
26. Device Topology SOLL bei DMA-Planung berücksichtigt werden können.
27. DMA-Nutzung SOLL budgetierbar sein.
28. User-Mode-Treiber MÜSSEN DMA ohne direkten unbeschränkten physischen Speicherzugriff verwenden können.
29. DMA-Operationen MÜSSEN Capability Enforcement unterliegen.
30. DMA DARF keine bestehende Speicherberechtigung umgehen.
31. Shared DMA Buffers MÜSSEN explizite Ownership- und Zugriffsregeln besitzen.
32. DMA-Mappings MÜSSEN widerrufbar sein.
33. Geräteentfernung MUSS zu kontrollierter DMA-Deaktivierung führen.
34. Treiberabstürze MÜSSEN aktive DMA-Ressourcen berücksichtigen.
35. IOMMU Faults MÜSSEN explizit behandelbar sein.
36. IOMMU Faults SOLLEN Gerät, Domain, Adresse und Zugriffsart identifizieren können.
37. Ein fehlerhaftes Gerät SOLL isoliert werden können.
38. Unbekannte DMA-Fähigkeiten DÜRFEN NICHT als unbeschränkte Fähigkeiten behandelt werden.
39. Unbekannte IOMMU-Isolation DARF NICHT als vorhandene Isolation interpretiert werden.
40. NovaOS MUSS ohne IOMMU grundsätzlich funktionsfähig bleiben können.
41. Ein No-IOMMU-Modus MUSS reduzierte Sicherheitsgarantien explizit darstellen.
42. IOMMU-Isolation DARF Driver Trust und Capability Security NICHT ersetzen.
43. Vertrauenswürdige Geräte SOLLEN weiterhin Least-Privilege-DMA erhalten.
44. DMA MUSS Data-Sovereignty- und Information-Flow-Regeln berücksichtigen können.
45. GPU-, NPU- und Accelerator-DMA SOLLEN in dasselbe kontrollierte Ressourcenmodell integrierbar sein.
46. Peer-to-Peer DMA MUSS explizit autorisiert werden.
47. Realtime Workloads DÜRFEN DMA-Ressourcen vorreservieren.
48. Komplexe DMA-Aktivierungen SOLLEN transaktional vorbereitbar sein.
49. DMA-Operationen SOLLEN mit Structured Concurrency integrierbar sein.
50. Buffer-Lebensdauer MUSS ausstehende Device Operations berücksichtigen.
51. DMA- und IOMMU-Zustand MUSS introspektierbar sein.
52. Sicherheitsrelevante DMA-Ereignisse SOLLEN auditierbar sein.
53. Der DMA Fast Path SOLL vorbereitete und validierte Ressourcen verwenden können.
54. Mapping Caches DÜRFEN Revocation und Permission Changes NICHT umgehen.
55. DMA-Contracts MÜSSEN evolvierbar sein.
56. Zukünftige Mechanismen wie PASID, ATS, PRI und Shared Virtual Addressing MÜSSEN integrierbar bleiben.
57. Shared Virtual Addressing DARF keine unkontrollierte Speicherautorität erzeugen.
58. DMA-Sicherheit und IOMMU-Konfiguration MÜSSEN ohne KI funktionieren.
59. KI DARF DMA-Optimierungen unterstützen, aber keine DMA-Autorität erzeugen.
60. NovaOS MUSS Gerätezugriff auf Speicher als explizite, kontrollierte Beziehung modellieren.

---

## Konsequenzen

### Positive Konsequenzen

- starke DMA-Isolation,
- bessere Unterstützung isolierter und User-Mode-Treiber,
- sichere Grundlage für Zero-Copy,
- kontrolliertes Pinned Memory,
- bessere NUMA-Lokalität,
- Fehler von Geräten können stärker eingegrenzt werden,
- IOMMU-Faults werden sichtbar und diagnostizierbar,
- DMA-Ressourcen werden budgetierbar,
- bessere Grundlage für GPU-, NPU- und Peer-to-Peer-I/O,
- zukünftige Shared-Virtual-Memory-Modelle bleiben integrierbar.

### Negative Konsequenzen

- DMA- und IOMMU-Management erhöht Kernelkomplexität,
- Mapping und Unmapping erzeugen Laufzeitkosten,
- IOTLB-Invalidierungen können teuer sein,
- Systeme ohne IOMMU benötigen schwächere Fallbackmodelle,
- Geräte mit ungewöhnlichen DMA-Beschränkungen benötigen zusätzliche Logik,
- Buffer-Lifetime und Ownership müssen präzise verwaltet werden.

---

## Verworfene Alternativen

### Treiber erhalten direkten Zugriff auf beliebige physische Adressen

Verworfen.

Dies würde Isolation und Capability Security erheblich schwächen.

### IOMMU ausschließlich für Virtualisierung verwenden

Verworfen.

Die IOMMU ist auch für native Treiber- und Geräteisolation wertvoll.

### DMA als ausschließlich treiberinternes Detail

Verworfen.

DMA beeinflusst Security, Memory Ownership, NUMA, Zero-Copy und Resource Economy.

### IOMMU als zwingende Voraussetzung für NovaOS

Verworfen.

NovaOS soll auch auf älteren oder einfachen Systemen ohne IOMMU funktionieren können, allerdings mit explizit reduzierten Garantien.

### Alle DMA-Buffer permanent pinnen

Verworfen.

Dies würde Memory Reclamation und Resource Economy unnötig beeinträchtigen.

### Alle Transfers über Bounce Buffer

Verworfen.

Dies würde sichere direkte DMA- und Zero-Copy-Pfade unnötig verhindern.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-HAL-0001_Einheitliche_Hardware_Abstraction_Layer`
- `ADR-HAL-0002_Plattformabstraktion_für_x86_ARM_und_zukünftige_Architekturen`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starken_Isolationsgrenzen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0010_Structured_Concurrency_als_systemweites_Ausführungsmodell`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts_zwischen_Systemkomponenten`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-ARCH-0019_Evolvierbare_Systemarchitektur`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`
- `ADR-EXECUTION-0001_Nova_ExecutionContract`

---

## Zugehörige NPSPECs

- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-HAL-DMA-MAPPING-0001`
- `NPSPEC-HAL-DMA-ADDRESSSPACE-0001`
- `NPSPEC-HAL-DMA-SCATTERGATHER-0001`
- `NPSPEC-HAL-DMA-CONSTRAINT-0001`
- `NPSPEC-HAL-DMA-COHERENCY-0001`
- `NPSPEC-HAL-DMA-BOUNCEBUFFER-0001`
- `NPSPEC-HAL-DMA-PINNEDMEMORY-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-HAL-IOMMU-DOMAIN-0001`
- `NPSPEC-HAL-IOMMU-GROUP-0001`
- `NPSPEC-HAL-IOMMU-MAPPING-0001`
- `NPSPEC-HAL-IOMMU-FAULT-0001`
- `NPSPEC-HAL-IOMMU-REVOCATION-0001`
- `NPSPEC-HAL-DMA-NUMA-0001`
- `NPSPEC-HAL-DMA-RESOURCE-0001`
- `NPSPEC-HAL-DMA-CAPABILITY-0001`
- `NPSPEC-HAL-DMA-P2P-0001`
- `NPSPEC-HAL-DMA-REALTIME-0001`
- `NPSPEC-HAL-DMA-INTROSPECTION-0001`
- `NPSPEC-HAL-DMA-AUDIT-0001`
- `NPSPEC-HAL-DMA-SVA-0001`

---

## Verifikation

Die Umsetzung muss insbesondere folgende Szenarien prüfen:

- DMA mit aktivierter IOMMU,
- DMA auf einem System ohne IOMMU,
- Read-only DMA Mapping,
- Write-only beziehungsweise Device-Write-Mapping,
- bidirektionales DMA,
- Scatter/Gather,
- 32-Bit-DMA-Gerät auf großem physischem Adressraum,
- Bounce-Buffer-Fallback,
- kohärentes DMA,
- nicht kohärentes DMA,
- NUMA-aware Buffer Allocation,
- IOMMU-Domain-Isolation zwischen zwei Geräten,
- Hardware-IOMMU-Gruppe mit nicht separierbaren Geräten,
- ungültiger DMA-Zugriff,
- IOMMU Permission Fault,
- Mapping Revocation,
- Treiberabsturz mit aktiven DMA-Mappings,
- Device Hot-Unplug,
- Resource-Limit für Pinned Memory,
- Peer-to-Peer DMA,
- deterministisch gepinnte DMA-Konfiguration,
- Recovery nach Device Fault.

---

## Ergebnis

NovaOS behandelt DMA nicht als unkontrollierten direkten Speicherzugriff eines Geräts.

Stattdessen gilt:

```text
Device
    │
    ▼
Explicit DMA Authority
    │
    ▼
IOMMU / DMA Address Space
    │
    ▼
Authorized Memory
```

Damit werden:

```text
Performance
Zero-Copy
Device Isolation
Memory Ownership
Resource Economy
Security
```

in einem gemeinsamen Architekturmodell verbunden.

Die zentrale Architekturregel lautet:

```text
DMA is not unrestricted memory access.

DMA is a scoped capability
to access explicitly authorized memory.
```

und:

```text
Use the IOMMU as an isolation mechanism
whenever available.

Never confuse hardware access
with permission.
```