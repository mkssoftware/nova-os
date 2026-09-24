# NPSPEC-IO-ZEROCOPY-0001 – Nova I/O Zero-Copy

## Status

Entwurf

## Kategorie

I/O / DataMove / Zero-Copy

## Zweck

NovaOS definiert die Integration von Zero-Copy-Datenübertragung in das I/O-System.

```text
Consumer Buffer
      ↓
Shared / Mapped Buffer
      ↓
I/O Provider
      ↓
Device
```

Ziel ist die Vermeidung unnötiger Speicher-Kopien zwischen Anwendung, Kernel, Treiber und Hardware.

## Grundprinzip

```text
Zero-Copy ≠ Zero Data Movement
Zero-Copy ≠ Unrestricted Shared Memory
Zero-Copy ≠ Mandatory
```

Zero-Copy bedeutet, dass Daten möglichst nicht zwischen mehreren Software-Buffern kopiert werden.

## I/O-Pfad

Ein klassischer Pfad kann mehrere Kopien erzeugen:

```text
Application
    ↓ copy
Kernel Buffer
    ↓ copy
Driver Buffer
    ↓
Device
```

Ein Zero-Copy-Pfad soll stattdessen gemeinsame oder direkt abgebildete Speicherbereiche verwenden:

```text
Application
      ↓
Shared Buffer
      ↓
Driver / DMA
      ↓
Device
```

## IORequest

Ein `IORequest` darf einen Zero-Copy-fähigen Buffer referenzieren.

```text
IORequest
├── Operation
├── Target
├── Buffer Reference
└── Buffer Properties
```

Der Request muss keine Kenntnis über die konkrete physische Speicherposition besitzen.

## Buffer-Modell

Zero-Copy darf insbesondere mit folgenden Mechanismen verwendet werden:

```text
Shared Buffer
Mapped Memory
Scatter/Gather
DMA Buffer
Pinned Memory
```

Ownership und Lebensdauer müssen eindeutig definiert sein.

## Lebensdauer

Ein Zero-Copy-Buffer muss während der gesamten Operation gültig bleiben.

```text
Acquire
   ↓
Submit
   ↓
I/O Running
   ↓
Completion
   ↓
Release
```

Eine Freigabe oder unsichere Wiederverwendung vor Completion ist nicht zulässig.

## DMA

Hardware-I/O darf Buffer direkt über DMA verwenden.

```text
IORequest
   ↓
DMA Mapping
   ↓
IOMMU
   ↓
Device
```

DMA-Zugriffe müssen auf die tatsächlich benötigten Speicherbereiche beschränkt werden.

## Scatter/Gather

Nicht zusammenhängende Speicherbereiche dürfen ohne vorherige Zusammenführung verwendet werden.

```text
Buffer A ─┐
Buffer B ─┼→ Scatter/Gather → Device
Buffer C ─┘
```

Dadurch sollen zusätzliche Kopien in einen zusammenhängenden Zwischenbuffer vermieden werden.

## Sicherheit

Zero-Copy darf Speicherisolation nicht umgehen.

Vor einer Freigabe müssen mindestens geprüft werden:

```text
Security Context
Capability
Access Direction
Memory Range
Lifetime
Provider
```

Ein Provider darf nur auf die für die jeweilige Operation freigegebenen Speicherbereiche zugreifen.

## Synchronisation

Gemeinsam verwendete Buffer benötigen definierte Ownership-Übergänge.

```text
Consumer Owns
     ↓
Submit
     ↓
Provider Owns
     ↓
Completion
     ↓
Consumer Owns
```

Gleichzeitige Zugriffe müssen verhindert oder ausdrücklich als sicher definiert sein.

## Cache-Kohärenz

Auf Plattformen ohne vollständig kohärente DMA-Zugriffe müssen erforderliche Cache-Synchronisationen durchgeführt werden.

```text
CPU Cache
   ↕
Memory
   ↕
DMA Device
```

Zero-Copy darf keine veralteten oder noch nicht sichtbaren Daten erzeugen.

## Fallback

Zero-Copy ist eine Optimierung und darf nicht Voraussetzung für korrekte I/O sein.

```text
Zero-Copy possible?
├── Yes → Direct / Shared Path
└── No  → Safe Copy Path
```

Ein sicherer Copy-Fallback muss verfügbar sein, wenn Mapping, Alignment, Security oder Provider-Fähigkeiten Zero-Copy verhindern.

## Kostenmodell

Zero-Copy darf nur verwendet werden, wenn der erwartete Nutzen die zusätzlichen Kosten rechtfertigt.

Zu berücksichtigen sind beispielsweise:

```text
Mapping Cost
Pinning Cost
IOMMU Cost
Buffer Size
Transfer Size
Cache Effects
```

Für kleine Transfers kann eine normale Kopie effizienter sein.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
RequestID
Zero-Copy State
Buffer Type
Mapping State
DMA State
Fallback State
Transferred Bytes
```

## Normative Anforderungen

1. NovaOS MUSS Zero-Copy-I/O unterstützen können.
2. Zero-Copy DARF NICHT Voraussetzung für korrekte I/O sein.
3. Ein sicherer Copy-Fallback MUSS verfügbar sein.
4. Buffer-Lebensdauer MUSS bis zum sicheren Completion-Punkt gewährleistet sein.
5. Ownership-Übergänge MÜSSEN eindeutig definiert sein.
6. Zero-Copy DARF Speicherisolation oder Capability-Prüfungen NICHT umgehen.
7. DMA-Zugriffe MÜSSEN auf autorisierte Speicherbereiche beschränkt werden.
8. Scatter/Gather SOLL ohne unnötige Zwischenkopien unterstützt werden.
9. Erforderliche Cache- und DMA-Synchronisation MUSS durchgeführt werden.
10. Zero-Copy SOLL nur verwendet werden, wenn es gegenüber Copy-I/O sinnvoll ist.
11. Provider MÜSSEN ihre Zero-Copy-Fähigkeiten beschreiben können.
12. Zero-Copy-, Mapping- und Fallback-Zustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-SCATTERGATHER-0001`
- `NPSPEC-DATAMOVE-DMA-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `ADR-IO-0008`

## Ergebnis

```text
IORequest
   ↓
Zero-Copy Decision
   ├→ Shared / DMA Path
   └→ Safe Copy Fallback
             ↓
          Provider
             ↓
         Completion
```

NovaOS erhält damit einen sicheren Zero-Copy-I/O-Pfad, der unnötige Speichertransfers reduziert, ohne Isolation, Korrektheit oder Portabilität von I/O abhängig zu machen.