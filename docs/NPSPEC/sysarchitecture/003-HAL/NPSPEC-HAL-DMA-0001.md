# NPSPEC-HAL-DMA-0001 – Nova Direct Memory Access

## Status

Entwurf

## Kategorie

HAL / DMA / Memory / Devices / I/O

## Zweck

NovaOS stellt Direct Memory Access (DMA) über eine kontrollierte HAL-Schnittstelle bereit, damit Geräte Daten direkt zwischen Gerät und Speicher übertragen können, ohne unnötige CPU-Kopien zu erzeugen.

```text
Device
   ↕
DMA
   ↕
Memory
```

## Grundprinzip

DMA ist ein Mechanismus für effiziente Datenbewegung.

```text
Driver
   ↓
DMA Request
   ↓
HAL / DMA Mapping
   ↓
Device ↔ Memory
```

Gerätetreiber sollen nicht eigenständig beliebige physische Speicherbereiche für DMA verwenden.

## DMA-Mapping

Vor einer DMA-Operation wird ein kontrolliertes Mapping erzeugt.

```text
Buffer
  ↓
DMA Map
  ↓
Device-visible Address
  ↓
Transfer
  ↓
DMA Unmap
```

Das Mapping beschreibt mindestens:

```text
Buffer
Device
Direction
Address
Length
Permissions
Lifetime
```

## Übertragungsrichtung

DMA-Operationen unterscheiden mindestens:

```text
ToDevice
FromDevice
Bidirectional
```

Die Richtung muss für Synchronisation und Zugriffsrechte berücksichtigt werden.

## IOMMU

Ist eine IOMMU verfügbar, soll NovaOS DMA-Zugriffe auf die für das Gerät freigegebenen Speicherbereiche beschränken.

```text
Device
   ↓
IOMMU
   ↓
Allowed Memory
```

Geräte dürfen dadurch nicht automatisch Zugriff auf den gesamten physischen Speicher erhalten.

## Scatter / Gather

DMA soll nicht zusammenhängende Speicherbereiche unterstützen können.

```text
Buffer A ─┐
Buffer B ─┼→ DMA Transfer
Buffer C ─┘
```

Die konkrete Umsetzung hängt von Hardware, IOMMU und Gerät ab.

## Cache-Kohärenz

Auf Plattformen ohne vollständige Hardware-Kohärenz muss die HAL notwendige Cache- und Memory-Synchronisation bereitstellen.

Treiber sollen nicht von einer grundsätzlich kohärenten Architektur ausgehen.

## Zero-Copy

DMA kann für Zero-Copy-Datenpfade verwendet werden.

```text
Device
   ↓ DMA
Shared Buffer
   ↓
Consumer
```

Zero-Copy ist jedoch nur zulässig, wenn Isolation, Ownership und Synchronisation gewährleistet bleiben.

## Fallback

Kann ein Buffer nicht direkt für DMA verwendet werden, darf NovaOS einen Bounce Buffer einsetzen.

```text
Original Buffer
      ↕
Bounce Buffer
      ↕
Device
```

Korrektheit und Sicherheit haben Vorrang vor Zero-Copy.

## Normative Anforderungen

1. NovaOS MUSS DMA über definierte HAL-Schnittstellen bereitstellen.
2. DMA-Zugriffe MÜSSEN an kontrollierte Speicherbereiche gebunden sein.
3. Gerätetreiber DÜRFEN nicht unkontrolliert beliebigen physischen Speicher für DMA freigeben.
4. DMA-Mappings MÜSSEN eine definierte Lebensdauer besitzen.
5. Übertragungsrichtung und Zugriffsrechte MÜSSEN explizit sein.
6. Eine verfügbare IOMMU SOLL zur Isolation von DMA-Zugriffen verwendet werden.
7. Scatter/Gather SOLL unterstützt werden, sofern Plattform und Gerät dies ermöglichen.
8. Cache- und Memory-Kohärenz MÜSSEN von der HAL berücksichtigt werden.
9. DMA SOLL mit Zero-Copy-Datenpfaden kombinierbar sein.
10. Falls direktes DMA nicht möglich ist, MUSS ein sicherer Fallback möglich sein.
11. DMA-Ressourcen MÜSSEN nach Abschluss oder Abbruch kontrolliert freigegeben werden.
12. DMA DARF Speicherisolation und Capability-Grenzen nicht umgehen.

## Abhängigkeiten

- `NPSPEC-HAL-0001`
- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-ARCH-ZEROCOPY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-ARCH-0006_Zero_Copy`
- `ADR-HAL-0007`

## Ergebnis

```text
Driver
   ↓
Controlled DMA Mapping
   ↓
IOMMU / Platform DMA
   ↓
Device ↔ Memory
```

NovaOS erhält damit einen effizienten und isolierten DMA-Mechanismus, der Zero-Copy ermöglicht, ohne Speicher- und Sicherheitsgrenzen aufzugeben.