# NPSPEC-DATAMOVE-DMA-0001 – Nova Direct Memory Access

## Status

Entwurf

## Kategorie

DataMove / DMA / I/O / Zero-Copy / Hardware

## Zweck

NovaOS definiert Direct Memory Access (DMA) als kontrollierten Mechanismus, mit dem Geräte Daten direkt zwischen Device und Hauptspeicher übertragen können, ohne dass die CPU jedes Datenwort kopieren muss.

```text
Traditional I/O:

Device
  ↓
CPU Copy
  ↓
Memory

DMA:

Device
  ↓
DMA Engine
  ↓
Memory
```

DMA soll CPU-Last, Kopieroperationen und I/O-Latenz reduzieren und bildet eine Grundlage für effiziente Storage-, Netzwerk-, Grafik- und Accelerator-Datenpfade.

## Grundprinzip

DMA überträgt Daten unabhängig von normalen CPU-Load/Store-Operationen.

```text
CPU configures transfer
        ↓
DMA Engine transfers data
        ↓
Device / Memory
        ↓
Completion
```

Dabei gilt:

```text
DMA ≠ Unrestricted Physical Memory Access
DMA ≠ Zero-Copy
DMA ≠ Scatter/Gather
```

DMA kann jedoch Bestandteil eines Zero-Copy- oder Scatter/Gather-Datenpfads sein.

## DMA-Operation

Eine DMA-Operation beschreibt mindestens:

```text
Device
Direction
Buffer
Offset
Length
State
```

Optional:

```text
ScatterGatherList
Alignment
Deadline
NUMANode
Completion
CachePolicy
```

## Transfer-Richtungen

NovaOS muss mindestens unterscheiden können:

```text
Device → Memory
Memory → Device
Bidirectional
```

Die Richtung beeinflusst unter anderem Permissions, Cache-Kohärenz und IOMMU-Mappings.

## DMA Buffer

DMA-fähige Buffer müssen die Anforderungen des jeweiligen Geräts erfüllen.

Mögliche Anforderungen:

```text
Alignment
Address Width
Maximum Transfer Size
Boundary Restrictions
Contiguous Memory
Scatter/Gather Support
Cache Properties
```

Diese Anforderungen müssen über die HAL beziehungsweise den Device Provider beschrieben werden.

## DMA Mapping

Ein normaler virtueller Buffer darf nicht automatisch als DMA-Adresse verwendet werden.

```text
Virtual Buffer
      ↓
DMA Mapping
      ↓
Device-visible Address
```

Die konkrete Device-Adresse kann durch:

```text
Physical Mapping
IOMMU Mapping
Bounce Buffer
```

bereitgestellt werden.

## IOMMU

Wenn eine IOMMU verfügbar ist, soll NovaOS DMA-Zugriffe darüber isolieren.

```text
Device
  ↓
IOMMU
  ↓
Authorized Frames
```

Ein Device erhält nur Zugriff auf die für seine aktuelle Operation freigegebenen Speicherbereiche.

```text
Device DMA Capability
        ≠
Access to all RAM
```

## DMA ohne IOMMU

Plattformen ohne IOMMU müssen weiterhin unterstützt werden können.

In diesem Fall muss NovaOS physische DMA-Anforderungen besonders streng validieren.

Falls ein Gerät einen Buffer nicht direkt adressieren kann:

```text
Original Buffer
      ↓
Bounce Buffer
      ↓
DMA
```

Der Bounce Buffer dient als kontrollierter Fallback.

## Pinning

Während eines aktiven DMA-Transfers müssen die verwendeten Frames stabil bleiben.

```text
Buffer
  ↓
Pin
  ↓
DMA Mapping
  ↓
Transfer
  ↓
Unmap
  ↓
Unpin
```

Gepinnte Frames dürfen währenddessen nicht:

```text
Reclaimed
Swapped
Compressed
Migrated
Freed
```

werden.

Pinning muss mengenmäßig und zeitlich begrenzbar sein.

## Scatter/Gather

DMA soll Scatter/Gather unterstützen können.

```text
Frame A ─┐
Frame B ─┼→ DMA SG List → Device
Frame C ─┘
```

Dadurch können physisch nicht zusammenhängende Speicherbereiche ohne vorherige Zusammenführung übertragen werden.

Hardwarelimits müssen berücksichtigt werden.

## Zero-Copy

DMA kann Bestandteil eines Zero-Copy-Pfades sein.

```text
Application Buffer
        ↓
DMA Mapping
        ↓
Device
```

Dies ist nur zulässig, wenn:

```text
Lifetime
Permissions
Alignment
Device Access
Cache Coherency
```

korrekt gewährleistet sind.

Andernfalls muss ein sicherer Copy-Fallback möglich sein.

## Cache-Kohärenz

Nicht jede Plattform garantiert kohärenten Zugriff zwischen CPU und Device.

NovaOS muss deshalb zwischen:

```text
Coherent DMA
Non-Coherent DMA
```

unterscheiden können.

Bei nicht kohärentem DMA können erforderlich sein:

```text
Cache Clean
Cache Flush
Cache Invalidate
Memory Barrier
```

Die konkrete Umsetzung erfolgt über die HAL.

## Memory Ordering

DMA-Descriptoren und Nutzdaten müssen in korrekter Reihenfolge sichtbar werden.

Konzeptionell:

```text
Prepare Data
    ↓
Memory Barrier
    ↓
Publish DMA Descriptor
    ↓
Start Device
```

Nach Abschluss:

```text
DMA Completion
      ↓
Required Barrier / Cache Operation
      ↓
CPU consumes data
```

Die konkreten Regeln sind architektur- und geräteabhängig.

## Ownership

Während eines DMA-Transfers muss klar definiert sein, wer einen Buffer verwenden darf.

```text
CPU owns Buffer
      ↓
Submit DMA
      ↓
Device owns / accesses Buffer
      ↓
Completion
      ↓
CPU owns Buffer
```

CPU und Device dürfen denselben Buffer nur gleichzeitig verändern, wenn das verwendete Protokoll dies ausdrücklich unterstützt.

## Completion

DMA-Operationen benötigen einen definierten Completion-Mechanismus.

Mögliche Mechanismen:

```text
Interrupt
Polling
Completion Queue
Event
Future
```

Dabei gilt:

```text
Submitted ≠ Completed
Interrupt Received ≠ Transfer Successful
```

Der tatsächliche Transferstatus muss geprüft werden.

## Fehlerbehandlung

DMA kann fehlschlagen.

Mögliche Fehler:

```text
Mapping Failure
IOMMU Fault
Device Error
Timeout
Invalid Descriptor
Transfer Abort
Device Removal
```

Ein Fehler muss den DMA-Buffer in einen definierten Zustand überführen.

Ein unbekannter Completion-Zustand darf nicht als erfolgreicher Transfer behandelt werden.

## Cancellation

Eine DMA-Operation darf nur dann abgebrochen werden, wenn das Gerät beziehungsweise der DMA-Provider dies sicher unterstützt.

```text
Cancel Request
      ↓
Stop / Quiesce Device
      ↓
Verify DMA stopped
      ↓
Unmap
      ↓
Unpin
```

Ein Buffer darf nicht freigegeben werden, solange das Gerät möglicherweise noch darauf zugreift.

## NUMA

DMA-Buffer sollen nach Möglichkeit nahe am verwendenden Gerät und den verarbeitenden CPUs platziert werden.

```text
Device
  ↓
Nearest NUMA Node
  ↓
DMA Buffer
```

Die Platzierungsentscheidung darf berücksichtigen:

```text
Device Locality
CPU Locality
Memory Pressure
Transfer Size
Migration Cost
```

## Memory Pressure

DMA-gepinnter Speicher reduziert reclaimbaren Speicher.

Daher müssen DMA-Pinning und langfristige DMA-Mappings in die systemweite Resource Economy einbezogen werden.

```text
Excessive Pinned Memory
        ↓
Memory Pressure
```

Nichtkritische Komponenten dürfen physische Frames nicht unbegrenzt für DMA blockieren.

## Realtime

DMA eignet sich für Realtime-Datenpfade, sofern:

```text
Buffers preallocated
Mappings prepared
Memory resident
Completion bounded
Device latency known
```

sind.

Unkontrollierte Allocation, Mapping oder Reclamation während eines Hard-Realtime-Transfers muss vermieden werden.

## Sicherheit

DMA ist eine direkte Sicherheitsgrenze.

Vor einer DMA-Freigabe müssen mindestens geprüft werden:

```text
Device Identity
Capability
Buffer Range
Direction
Permissions
Lifetime
```

DMA darf keine Möglichkeit schaffen, Kernel- oder fremden Prozessspeicher außerhalb der autorisierten Buffer zu lesen oder zu verändern.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Active DMA Operations
Mapped DMA Bytes
Pinned Memory
Device Mappings
IOMMU Mappings
Scatter/Gather Segments
Bounce Buffer Usage
Transfer Latency
DMA Faults
Timeouts
```

## Normative Anforderungen

1. NovaOS MUSS DMA über eine hardwareunabhängige DataMove-Schnittstelle unterstützen können.
2. Virtuelle CPU-Adressen DÜRFEN NICHT automatisch als Device-DMA-Adressen behandelt werden.
3. DMA-Buffer MÜSSEN vor Verwendung hinsichtlich Range, Permissions und Hardwareanforderungen validiert werden.
4. DMA-Zugriffe SOLLEN bei verfügbarer IOMMU auf autorisierte Speicherbereiche begrenzt werden.
5. Plattformen ohne IOMMU MÜSSEN kontrolliert unterstützt werden können.
6. Aktive DMA-Frames MÜSSEN gegen Freigabe, Reclamation, Swap und inkompatible Migration geschützt sein.
7. Cache-Kohärenz und Memory Ordering MÜSSEN entsprechend Plattform und Device korrekt behandelt werden.
8. DMA-Ownership und Buffer-Lifetime MÜSSEN eindeutig definiert sein.
9. Ein Buffer DARF NICHT freigegeben werden, solange ein Device möglicherweise noch darauf zugreift.
10. DMA-Completion MUSS von erfolgreichem Transferstatus unterscheidbar sein.
11. DMA-Pinning MUSS in Memory Pressure und Resource Economy berücksichtigt werden.
12. DMA-Mappings, Transfers, Pinning und Fehler SOLLEN sicherheitsgefiltert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-SCATTERGATHER-0001`
- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-MEMORY-PRESSURE-0001`
- `NPSPEC-MEMORY-MAPPING-0001`
- `NPSPEC-MEMORY-PROTECTION-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-DATAMOVE-0003`

## Ergebnis

```text
Buffer
  ↓
Validate + Pin
  ↓
DMA / IOMMU Mapping
  ↓
Device Transfer
  ↓
Completion Verification
  ↓
Unmap + Unpin
  ↓
Buffer returned to Owner
```

NovaOS erhält damit eine sichere DMA-Grundlage, die direkte Device-Speichertransfers mit Zero-Copy, Scatter/Gather, IOMMU-Isolation, NUMA und systemweiter Ressourcenverwaltung verbindet.