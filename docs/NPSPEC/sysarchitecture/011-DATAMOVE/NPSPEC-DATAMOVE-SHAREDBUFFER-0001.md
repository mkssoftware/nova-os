# NPSPEC-DATAMOVE-SHAREDBUFFER-0001 – Nova Shared Buffer

## Status

Entwurf

## Kategorie

DataMove / Shared Buffer / Zero-Copy / IPC / I/O

## Zweck

NovaOS definiert Shared Buffers als kontrollierte Datenobjekte, deren Speicher von mehreren Komponenten, Prozessen oder Geräten gemeinsam verwendet werden kann.

```text
Producer
   ↓
Shared Buffer
   ├→ Consumer A
   ├→ Consumer B
   └→ Device
```

Shared Buffers bilden eine gemeinsame Grundlage für Zero-Copy-Datenpfade zwischen IPC, Netzwerk, Storage, Grafik, Multimedia und Hardwaregeräten.

## Grundprinzip

Ein Shared Buffer ist ein eigenständiges Datenobjekt mit stabiler Identität und definiertem Lifecycle.

```text
Shared Buffer
├── Identity
├── Backing Memory
├── Size
├── Permissions
├── Ownership
└── Lifetime
```

Dabei gilt:

```text
Shared Buffer ≠ Shared Virtual Address
Shared Buffer ≠ Synchronization
Shared Buffer ≠ Unrestricted Shared Memory
```

Teilnehmer dürfen denselben Buffer an unterschiedlichen virtuellen Adressen mappen.

## Buffer-Modell

Ein Shared Buffer besitzt mindestens:

```text
BufferID
Size
Backing
Owner
State
References
```

Optional:

```text
Alignment
Permissions
NUMAPolicy
CachePolicy
SecurityContext
DeviceMappings
ResourceBudget
```

Die `BufferID` bleibt unabhängig von virtueller Adresse, physischer Adresse oder Device-Adresse.

## Buffer-Zustände

Ein Buffer kann konzeptionell folgende Zustände besitzen:

```text
Created
Mapped
InUse
Transferred
Released
```

Für asynchrone I/O-Operationen dürfen zusätzliche Zustände existieren:

```text
Submitted
DeviceOwned
Completing
```

Zustandsübergänge müssen eindeutig definiert sein.

## Backing Memory

Ein Shared Buffer darf aus einem oder mehreren physischen Speicherbereichen bestehen.

```text
Shared Buffer
   ├→ Frame A
   ├→ Frame B
   └→ Frame C
```

Physische Zusammenhängigkeit darf nur verlangt werden, wenn Hardware oder Provider dies benötigen.

Scatter/Gather soll bevorzugt werden, wenn dadurch unnötige Kopien vermieden werden können.

## Mapping

Ein Buffer darf in mehrere Address Spaces gemappt werden.

```text
Buffer
├→ VAS A @ 0x...
├→ VAS B @ 0x...
└→ Kernel VAS @ 0x...
```

Jedes Mapping besitzt eigene Permissions.

```text
Producer → Read/Write
Consumer → Read
Device   → Read
```

## Ownership

Shared Buffer und Shared Ownership sind getrennte Konzepte.

NovaOS muss mindestens unterscheiden können:

```text
Exclusive Ownership
Shared Read Ownership
Transferred Ownership
Device Ownership
```

Ein typischer Datenpfad kann sein:

```text
Producer owns
     ↓
Producer fills
     ↓
Transfer
     ↓
Consumer owns
```

Ownership Transfer soll Synchronisationsaufwand reduzieren können.

## Referenzen

Mehrere Teilnehmer dürfen gültige Referenzen auf denselben Buffer besitzen.

```text
Buffer
├── Producer Reference
├── Consumer Reference
└── Device Reference
```

Der Buffer darf erst freigegeben werden, wenn keine relevante Referenz und kein aktiver Hardwarezugriff mehr besteht.

## Capability-basierter Zugriff

Zugriff auf Shared Buffers muss explizit autorisiert werden.

Mögliche Rechte:

```text
Map
Read
Write
Transfer
Delegate
DMA
Resize
Release
```

Delegierte Rechte dürfen eingeschränkt werden.

```text
Read/Write
    ↓
Delegate
    ↓
Read-Only
```

## Zero-Copy

Shared Buffers sind ein primärer Zero-Copy-Mechanismus.

```text
Producer
   ↓
Shared Buffer
   ↓
Transfer Reference
   ↓
Consumer
```

Der Payload muss dabei nicht kopiert werden.

Zero-Copy ist jedoch eine Optimierung.

Wenn Sharing nicht sicher oder wirtschaftlich möglich ist:

```text
Shared Buffer
     ↓
Safe Copy
     ↓
Destination Buffer
```

## Scatter/Gather

Ein Shared Buffer darf intern aus mehreren Segmenten bestehen.

```text
Logical Buffer
   ├→ Segment A
   ├→ Segment B
   └→ Segment C
```

Higher-Level-Komponenten sollen den Buffer trotzdem als logische Dateneinheit behandeln können.

## DMA

Shared Buffers dürfen für DMA verwendet werden.

```text
Shared Buffer
      ↓
Pin
      ↓
IOMMU / DMA Mapping
      ↓
Device
```

Während eines aktiven DMA-Zugriffs müssen benötigte Frames stabil bleiben.

Der Buffer darf erst freigegeben oder inkompatibel verändert werden, wenn der Device-Zugriff sicher beendet ist.

## CPU-/Device-Ownership

Bei CPU- und Device-Sharing muss der aktuelle Zugriffszustand eindeutig sein.

```text
CPU owns
   ↓
Prepare
   ↓
Device owns
   ↓
DMA
   ↓
Completion
   ↓
CPU owns
```

Gleichzeitiger Zugriff ist nur zulässig, wenn das verwendete Protokoll dies ausdrücklich unterstützt.

## Cache-Kohärenz

Shared Buffers können zwischen CPUs und Geräten mit unterschiedlichen Cache-Eigenschaften verwendet werden.

NovaOS muss unterscheiden können:

```text
Coherent
Non-Coherent
Device-specific
```

Erforderliche Cache-Operationen werden über die HAL durchgeführt.

## Synchronisation

Ein Shared Buffer erzeugt keine automatische Synchronisation.

```text
Shared Buffer ≠ Synchronized Buffer
```

Geeignete Mechanismen umfassen:

```text
Atomics
Futex
Semaphore
Completion
Ownership Transfer
Producer/Consumer Protocol
```

Die Memory-Ordering-Regeln des verwendeten Synchronisationsmechanismus müssen eingehalten werden.

## NUMA

Shared Buffers sollen NUMA-aware platziert werden können.

```text
Producer
Consumer
Device
   ↓
Placement Decision
   ↓
Suitable NUMA Node
```

Die Entscheidung darf berücksichtigen:

```text
Producer Locality
Consumer Locality
Device Locality
Transfer Direction
Memory Pressure
Expected Lifetime
```

## Memory Pressure

Shared und insbesondere gepinnte Buffer können Reclamation verhindern.

```text
Long-lived Shared Buffers
        ↓
Reduced Reclaimable Memory
```

NovaOS muss Shared-Buffer-Speicher deshalb in Resource Budgets und Memory Pressure berücksichtigen.

Nichtkritische Komponenten dürfen unbegrenzt gepinnte Shared Buffers nicht erzeugen.

## Realtime

Realtime-Datenpfade dürfen Shared Buffers vorab:

```text
Allocate
Map
Pin
Initialize
```

damit während der zeitkritischen Ausführung keine unkontrollierten Page Faults oder Allokationen notwendig werden.

## Sicherheit

Vor jeder Freigabe eines Buffers an einen neuen Teilnehmer müssen mindestens geprüft werden:

```text
Identity
Capability
Permissions
Range
Operation
Security Domain
```

Ein Teilnehmer darf nur die für ihn autorisierten Bufferbereiche sehen.

Bei Wiederverwendung zwischen Sicherheitsdomänen müssen alte Daten entsprechend der Security Policy entfernt werden.

## Lifecycle

```text
Create
  ↓
Allocate Backing
  ↓
Map / Share
  ↓
Use / Transfer
  ↓
Unmap
  ↓
Release References
  ↓
Destroy
```

Aktive DMA-, I/O- oder Consumer-Operationen müssen vor der endgültigen Freigabe abgeschlossen oder sicher abgebrochen sein.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
BufferID
Size
Owner
State
Reference Count
Mappings
Permissions
Pinned State
DMA Mappings
NUMA Placement
Resource Usage
```

Sicherheitskritische Informationen müssen entsprechend dem Security Context gefiltert werden.

## Normative Anforderungen

1. NovaOS MUSS Shared Buffers als eigenständige Datenobjekte mit stabiler Identität modellieren können.
2. Buffer-Identität DARF NICHT von virtueller, physischer oder Device-Adresse abhängen.
3. Ein Shared Buffer MUSS in mehrere autorisierte Address Spaces gemappt werden können.
4. Teilnehmer MÜSSEN unterschiedliche Permissions besitzen können.
5. Zugriff auf Shared Buffers MUSS explizit autorisiert werden.
6. Buffer-Lifetime MUSS alle aktiven CPU-, Consumer- und Device-Zugriffe überdauern.
7. Ownership Transfer MUSS eindeutig definierte Zugriffsrechte erzeugen.
8. Shared Buffers DÜRFEN NICHT als impliziter Synchronisationsmechanismus behandelt werden.
9. DMA-fähige Buffer MÜSSEN während aktiver Transfers gegen inkompatible Freigabe oder Migration geschützt werden.
10. Cache-Kohärenz MUSS entsprechend Plattform und Device behandelt werden.
11. Shared-Buffer-Speicher MUSS in Resource Economy und Memory Pressure berücksichtigt werden.
12. Buffer-Zustand, Ownership, Mappings und Hardwarezugriffe SOLLEN sicherheitsgefiltert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-SCATTERGATHER-0001`
- `NPSPEC-DATAMOVE-DMA-0001`
- `NPSPEC-MEMORY-SHARED-0001`
- `NPSPEC-MEMORY-MAPPING-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-MEMORY-PROTECTION-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-ARCH-ZEROCOPY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-DATAMOVE-0004`

## Ergebnis

```text
                    Shared Buffer
                         ↓
        ┌────────────────┼────────────────┐
        ↓                ↓                ↓
     Producer         Consumer          Device
        ↕                ↕                ↕
     Mapping          Mapping        DMA Mapping
        └────────────────┼────────────────┘
                         ↓
              Shared Physical Backing
```

NovaOS erhält damit eine einheitliche Shared-Buffer-Abstraktion, über die Prozesse, Systemdienste und Geräte große Datenmengen kontrolliert, capability-basiert und möglichst ohne unnötige Kopien austauschen können.