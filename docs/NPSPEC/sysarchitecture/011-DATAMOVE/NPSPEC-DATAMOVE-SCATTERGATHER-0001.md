# NPSPEC-DATAMOVE-SCATTERGATHER-0001 – Nova Scatter/Gather I/O

## Status

Angenommen

## Kategorie

DataMove / Scatter-Gather / I/O / Zero-Copy / DMA

## Zweck

NovaOS definiert Scatter/Gather als Mechanismus, mit dem mehrere nicht zusammenhängende Speicherbereiche als ein logischer Datenstrom verarbeitet werden können.

```text
Buffer A ─┐
Buffer B ─┼→ Scatter/Gather List → Consumer / Device
Buffer C ─┘
```

Dadurch können Daten direkt verarbeitet oder übertragen werden, ohne sie zuvor in einen zusammenhängenden Zwischenbuffer kopieren zu müssen.

## Grundprinzip

Logisch zusammenhängende Daten müssen nicht physisch zusammenhängend gespeichert sein.

```text
Logical Data
    ≠
Physically Contiguous Memory
```

Scatter/Gather trennt daher die logische Datenstruktur von ihrer physischen Speicherverteilung.

Dabei gilt:

```text
Scatter/Gather ≠ Memory Copy
Scatter/Gather ≠ Shared Memory
Scatter/Gather ≠ DMA
```

Scatter/Gather kann jedoch mit Zero-Copy, DMA, IPC, Netzwerk und Storage kombiniert werden.

## Scatter/Gather Descriptor

Ein Scatter/Gather-Descriptor beschreibt eine geordnete Menge von Speichersegmenten.

```text
SGDescriptor
├── Segment 0
├── Segment 1
├── Segment 2
└── ...
```

Ein Segment beschreibt mindestens:

```text
Buffer Reference
Offset
Length
```

Optional:

```text
Permissions
Device Address
Flags
Alignment
```

Physische Adressen dürfen nicht Bestandteil der allgemeinen, architekturunabhängigen API sein.

## Logische Sicht

Mehrere Segmente bilden einen gemeinsamen logischen Datenstrom.

```text
Segment A: [0 ... 4095]
Segment B: [4096 ... 8191]
Segment C: [8192 ... 12287]

Logical Stream:
[ A | B | C ]
```

Der Consumer soll die Daten als zusammenhängende logische Sequenz behandeln können.

## Gather

Gather beschreibt das Lesen mehrerer Speicherbereiche als einen Datenstrom.

```text
Buffer A ─┐
Buffer B ─┼→ Gather → Output
Buffer C ─┘
```

Typische Anwendungen:

```text
Network Send
Storage Write
IPC Transfer
Checksums
Compression
Encryption
```

## Scatter

Scatter beschreibt die Verteilung eingehender Daten auf mehrere Speicherbereiche.

```text
Input
  ↓
Scatter
├→ Buffer A
├→ Buffer B
└→ Buffer C
```

Typische Anwendungen:

```text
Network Receive
Storage Read
Message Parsing
Streaming
```

## Zero-Copy

Scatter/Gather soll Zero-Copy-Datenpfade unterstützen.

```text
Application Buffers
       ↓
Scatter/Gather Descriptor
       ↓
Kernel / Device
```

Ein zusätzlicher Buffer:

```text
A + B + C
    ↓
Copy
    ↓
Contiguous Buffer
```

soll vermieden werden, wenn der Zielpfad Scatter/Gather direkt verarbeiten kann.

## DMA

DMA-fähige Geräte können Scatter/Gather-Listen direkt verwenden.

```text
Physical Frames
      ↓
DMA Mapping
      ↓
Scatter/Gather List
      ↓
Device
```

Die HAL muss Hardwaregrenzen berücksichtigen.

Beispiele:

```text
Maximum Segment Count
Maximum Segment Size
Alignment
Address Width
Boundary Restrictions
DMA Engine Limits
```

## IOMMU

Bei vorhandener IOMMU dürfen Scatter/Gather-Segmente in einen kontrollierten Device Address Space gemappt werden.

```text
Memory Buffers
      ↓
IOMMU Mapping
      ↓
Device-visible SG List
```

Ein Gerät darf ausschließlich auf autorisierte Segmente zugreifen.

## Segment Coalescing

Benachbarte kompatible Segmente dürfen zusammengeführt werden.

```text
Segment A
Segment B
   ↓
Physically contiguous?
Compatible?
   ↓
Combined Segment
```

Dadurch kann die Anzahl der Scatter/Gather-Einträge reduziert werden.

## Segment Splitting

Ein Segment darf aufgeteilt werden, wenn Hardwaregrenzen dies erfordern.

```text
Large Segment
     ↓
Hardware Limit
     ↓
Segment A
Segment B
Segment C
```

Die logische Datenreihenfolge muss dabei erhalten bleiben.

## Chaining

Wenn eine Scatter/Gather-Liste die maximale Größe einer Hardwarestruktur überschreitet, dürfen mehrere Descriptoren verkettet werden.

```text
SG List 1
    ↓
SG List 2
    ↓
SG List 3
```

Chaining darf nur verwendet werden, wenn der jeweilige Provider dies unterstützt.

## Ownership und Lifetime

Alle referenzierten Buffer müssen während der gesamten Operation gültig bleiben.

```text
Submit SG Operation
        ↓
Buffers remain valid
        ↓
Operation Complete
        ↓
Buffers may be released
```

Ein Buffer darf nicht freigegeben oder inkompatibel remapped werden, solange ein aktiver Consumer ihn verwendet.

## Pinning

DMA kann erfordern, dass physische Frames während der Operation nicht verschoben oder reclaimed werden.

```text
Buffer
  ↓
Pin
  ↓
DMA Operation
  ↓
Completion
  ↓
Unpin
```

Pinning muss zeitlich und mengenmäßig begrenzt werden können.

## Cache-Kohärenz

Bei Device-I/O müssen die Cache-Kohärenzregeln der Plattform eingehalten werden.

```text
CPU
 ↕
Cache
 ↕
Memory
 ↕
Device
```

Notwendige:

```text
Flush
Clean
Invalidate
Memory Barrier
```

Operationen werden über die HAL ausgeführt.

## NUMA

Scatter/Gather kann Buffer von unterschiedlichen NUMA-Nodes enthalten.

```text
Node 0 Buffer ─┐
Node 1 Buffer ─┼→ SG Operation
Node 2 Buffer ─┘
```

NovaOS soll die Kosten solcher Transfers berücksichtigen.

Eine zusätzliche Kopie oder Migration darf gewählt werden, wenn sie insgesamt günstiger ist als dauerhaft ungünstiger Remote-Zugriff.

## Sicherheit

Vor der Erstellung eines Scatter/Gather-Descriptors müssen alle Segmente validiert werden.

Geprüft werden mindestens:

```text
Buffer Validity
Range
Permissions
Capability
Lifetime
Operation Direction
```

Integer Overflows bei:

```text
Offset + Length
Total Length
Segment Count
```

müssen verhindert werden.

## Fallback

Nicht jeder Consumer unterstützt Scatter/Gather.

```text
Scatter/Gather supported?
      ┌──────┴──────┐
     Yes            No
      ↓              ↓
Direct SG       Bounce / Copy Buffer
```

Ein sicherer Copy-Fallback darf verwendet werden.

Der Fallback muss für höhere Schichten semantisch äquivalent bleiben.

## Resource Economy

Die Entscheidung für Scatter/Gather darf berücksichtigen:

```text
Segment Count
Payload Size
Descriptor Cost
DMA Limits
Cache Effects
NUMA Distance
Copy Cost
Latency
Energy
```

Sehr kleine oder stark fragmentierte Transfers können durch eine direkte Kopie effizienter sein.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Scatter/Gather Operations
Segment Count
Transferred Bytes
Coalesced Segments
Split Segments
DMA Mappings
Pinned Memory
Fallback Copies
Operation Latency
Validation Failures
```

## Normative Anforderungen

1. NovaOS MUSS mehrere nicht zusammenhängende Speicherbereiche als einen logischen Datenstrom darstellen können.
2. Scatter/Gather-Descriptoren MÜSSEN Buffer, Offset und Länge eindeutig beschreiben.
3. Die allgemeine Scatter/Gather-API DARF NICHT von physischen Adressen abhängen.
4. Die logische Reihenfolge der Daten MUSS bei Segmentierung erhalten bleiben.
5. Alle Segmente MÜSSEN vor Verwendung auf Range, Permissions und Gültigkeit geprüft werden.
6. Buffer-Lifetime MUSS bis zum Abschluss der jeweiligen Operation garantiert sein.
7. DMA-Segmente MÜSSEN entsprechend den Hardware- und IOMMU-Regeln gemappt werden.
8. Segment Coalescing und Splitting DÜRFEN die logische Datensemantik NICHT verändern.
9. Pinning MUSS kontrolliert und nach Abschluss wieder aufgehoben werden.
10. Cache-Kohärenz MUSS entsprechend der Plattform korrekt behandelt werden.
11. NovaOS SOLL einen sicheren Copy- oder Bounce-Buffer-Fallback unterstützen.
12. Scatter/Gather-Nutzung, Fragmentierung, DMA-Mappings und Fallbacks SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-MEMORY-MAPPING-0001`
- `NPSPEC-MEMORY-SHARED-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-MEMORY-PROTECTION-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-DATAMOVE-0002`

## Ergebnis

```text
Distributed Buffers
   ┌────┼────┐
   A    B    C
   └────┼────┘
        ↓
Scatter/Gather Descriptor
        ↓
Zero-Copy / DMA / I/O
        ↓
Logical Data Stream
```

NovaOS erhält damit eine einheitliche Scatter/Gather-Grundlage, mit der fragmentierte Speicherbereiche effizient als logischer Datenstrom für IPC-, Storage-, Netzwerk- und DMA-Pfade genutzt werden können, ohne unnötige Zwischenkopien zu erzwingen.