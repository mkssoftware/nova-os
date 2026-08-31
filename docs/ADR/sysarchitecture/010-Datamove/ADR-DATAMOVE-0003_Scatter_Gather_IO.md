# ADR-DATAMOVE-0003 – Scatter/Gather I/O

## Status

Angenommen

## Kategorie

Kernel / Data Movement / I/O / Zero-Copy

## Kontext

I/O-Daten liegen häufig nicht in einem einzigen zusammenhängenden Speicherbereich vor. Ohne Scatter/Gather-Unterstützung müssten solche Daten zunächst in temporäre zusammenhängende Buffer kopiert oder nach einer I/O-Operation erneut verteilt werden.

```text
Buffer A ─┐
Buffer B ─┼── Copy → Temporary Buffer → Device
Buffer C ─┘
```

Dies widerspricht dem Zero-Copy-Prinzip von NovaOS und erzeugt zusätzliche Speicherbandbreite, Latenz und CPU-Last.

---

## Entscheidung

NovaOS unterstützt **Scatter/Gather I/O als systemweite Data-Movement-Funktion**.

Eine I/O-Operation kann eine geordnete Liste mehrerer Buffer beziehungsweise Buffer-Bereiche beschreiben.

```text
I/O Request
    ↓
Scatter/Gather List
    ├── Buffer A / Range
    ├── Buffer B / Range
    └── Buffer C / Range
    ↓
I/O Provider
```

Die logische Datenfolge bleibt zusammenhängend, obwohl das physische Backing fragmentiert sein kann.

Scatter/Gather soll von gemeinsamen Data-Movement-Abstraktionen verwendet werden und nicht für jedes Subsystem separat definiert werden.

---

## Grundprinzip

```text
Logical continuity
does not require
physical continuity.
```

---

## Scatter und Gather

**Gather I/O** liest Daten aus mehreren Speicherbereichen und behandelt sie als einen logischen Datenstrom.

```text
A + B + C → Device
```

**Scatter I/O** verteilt eingehende Daten auf mehrere Zielbereiche.

```text
Device → A + B + C
```

Beide Operationen verwenden dasselbe grundlegende Segmentmodell.

---

## Segmentmodell

Ein Scatter/Gather-Request besteht aus geordneten Segmenten.

Ein Segment referenziert mindestens:

```text
Buffer
Offset
Length
```

Rohe physische Adressen sind nicht Teil der allgemeinen systemweiten Schnittstelle.

Die konkrete Übersetzung auf Pages, DMA-Segmente oder hardwareabhängige Deskriptoren erfolgt durch die zuständigen Memory-, HAL- und Device-Komponenten.

---

## Shared Buffers

Scatter/Gather I/O baut auf den Shared-Buffer- und Memory-Object-Abstraktionen von NovaOS auf.

Ein Request kann Bereiche mehrerer Shared Buffers oder mehrere Bereiche desselben Buffers referenzieren.

Dadurch bleiben:

- Ownership,
- Authority,
- Lifetime,
- Resource Accounting

auch bei fragmentierten Datenpfaden erhalten.

---

## DMA und Hardware

Unterstützt ein Device Scatter/Gather-DMA, kann NovaOS die Segmente direkt auf geeignete DMA-/IOMMU-Mappings abbilden.

```text
Shared Buffers
      ↓
Scatter/Gather Description
      ↓
IOMMU / DMA
      ↓
Device
```

Hardwaregrenzen wie maximale Segmentanzahl, Alignment oder Segmentgröße werden durch den jeweiligen Provider berücksichtigt.

---

## Controlled Fallback

Nicht jede Hardware unterstützt beliebige Scatter/Gather-Strukturen.

NovaOS darf deshalb:

- Segmente zusammenführen,
- Requests aufteilen,
- IOMMU-Mappings verwenden,
- oder als letzten Fallback einen Bounce Buffer einsetzen.

```text
Native Scatter/Gather
        ↓ unavailable
IOMMU / Request Transformation
        ↓ unavailable
Controlled Copy
```

Ein Copy-Fallback ist zulässig, muss aber als tatsächliche Datenkopie behandelt und entsprechend sichtbar sein.

---

## Security und Authority

Alle referenzierten Buffer-Bereiche müssen für die angeforderte Operation autorisiert sein.

Ein Scatter/Gather-Request darf nicht verwendet werden, um Zugriffsgrenzen zwischen Memory Objects oder Execution Domains zu umgehen.

Für Device-I/O gelten zusätzlich die DMA-/IOMMU-Sicherheitsregeln.

---

## Resource Economy

Scatter/Gather-Operationen müssen ihre Ressourcen zurechenbar halten.

Dies betrifft insbesondere:

- referenzierte Buffer,
- temporäres Pinning,
- DMA-Mappings,
- Descriptor-Ressourcen,
- Bounce Buffer.

Ein Scatter/Gather-Request darf Ressourcenbudgets nicht durch eine große Anzahl kleiner Segmente umgehen.

---

## Locality

Scatter/Gather erlaubt die Verarbeitung physisch verteilter Daten ohne vorherige Zusammenführung.

Die Policy darf dennoch eine Kopie oder Reorganisation wählen, wenn dies aufgrund von:

- NUMA-Locality,
- Device-Locality,
- Segmentanzahl,
- Hardwaregrenzen,
- Gesamtkosten

effizienter ist.

Zero-Copy bleibt ein Optimierungsziel und kein Zwang.

---

## Realtime und Determinismus

Realtime-I/O muss benötigte:

- Buffer,
- Descriptoren,
- DMA-Mappings,
- gegebenenfalls Pinned Memory

vorab reservieren können.

Deterministic Mode muss adaptive Entscheidungen über Segmentzusammenführung oder Copy-Fallbacks kontrollieren können.

---

## Architecture Introspection

NovaOS muss Scatter/Gather-Datenpfade kontrolliert introspektierbar machen.

Relevant sind insbesondere:

- Segmentanzahl,
- Gesamtgröße,
- beteiligte Buffer,
- verwendeter I/O-Provider,
- native Scatter/Gather-Nutzung,
- DMA-/IOMMU-Nutzung,
- Copy- beziehungsweise Bounce-Buffer-Fallbacks.

---

## Normative Anforderungen

1. NovaOS MUSS Scatter/Gather I/O als gemeinsame Data-Movement-Funktion unterstützen.
2. Scatter und Gather MÜSSEN auf einem gemeinsamen Segmentmodell basieren.
3. Segmente MÜSSEN Shared Buffers beziehungsweise Memory Objects referenzieren können.
4. Die systemweite Schnittstelle DARF NICHT von rohen physischen Adressen abhängig sein.
5. Ownership, Authority und Lifetime der referenzierten Buffer MÜSSEN erhalten bleiben.
6. Hardware Scatter/Gather und Scatter/Gather-DMA SOLLEN verwendet werden, wenn sie verfügbar und geeignet sind.
7. IOMMU-basierte Abbildung SOLL unnötige Datenkopien vermeiden können.
8. NovaOS MUSS kontrollierte Fallbacks für Hardware ohne ausreichende Scatter/Gather-Unterstützung bereitstellen.
9. Security- und Data-Sovereignty-Grenzen DÜRFEN durch Scatter/Gather NICHT umgangen werden.
10. DMA-Ressourcen, Pinning und temporäre Buffer MÜSSEN budgetiert bleiben.
11. Hardwaregrenzen wie Segmentanzahl und Alignment MÜSSEN durch den zuständigen Provider behandelt werden.
12. Realtime-Domains MÜSSEN benötigte Scatter/Gather-Ressourcen vorab reservieren können.
13. Scatter/Gather-Nutzung und Copy-Fallbacks MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- weniger temporäre Datenkopien,
- effiziente Verarbeitung fragmentierter Buffer,
- bessere Nutzung von DMA- und IOMMU-Hardware,
- einheitliche Grundlage für Netzwerk, Storage und andere I/O-Systeme,
- direkte Integration in NovaOS-Zero-Copy-Datenpfade.

### Negative Konsequenzen

- komplexere I/O-Deskriptoren,
- hardwareabhängige Segmentgrenzen,
- zusätzliche Lifetime- und Pinning-Anforderungen,
- manche Geräte benötigen weiterhin Bounce Buffer oder andere Copy-Fallbacks.

---

## Verworfene Alternativen

### Zusammenhängende Buffer für jede I/O-Operation erzwingen

Verworfen.

Dies würde unnötige Kopien oder physisch zusammenhängende Allokationen erzwingen.

### Scatter/Gather nur innerhalb einzelner Treiber

Verworfen.

Die Semantik soll bereits auf der gemeinsamen Data-Movement-Ebene verfügbar sein.

### Scatter/Gather ohne kontrollierten Fallback

Verworfen.

Hardwarefähigkeiten und Segmentgrenzen unterscheiden sich zwischen Plattformen und Devices.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-DATAMOVE-0001_Zero_Copy_Datenpfade`
- `ADR-DATAMOVE-0002_Shared_Buffers_als_systemweite_Abstraktion`
- `ADR-MEMORY-0014_Pinned_Memory_als_budgetierte_Ressource`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0018_Locality_als_Optimierungsziel`

---

## Zugehörige NPSPECs

- `NPSPEC-DATAMOVE-SCATTERGATHER-0001`
- `NPSPEC-DATAMOVE-SEGMENT-0001`
- `NPSPEC-DATAMOVE-SCATTERGATHER-DMA-0001`
- `NPSPEC-DATAMOVE-SCATTERGATHER-FALLBACK-0001`
- `NPSPEC-DATAMOVE-SCATTERGATHER-TEST-0001`

---

## Ergebnis

NovaOS trennt die logische Zusammengehörigkeit von Daten von ihrer physischen Speicheranordnung:

```text
Multiple Buffer Segments
          ↓
Scatter/Gather I/O
          ↓
DMA / IOMMU / I/O Provider
          ↓
Device
```

Damit können fragmentierte Daten direkt verarbeitet werden, ohne sie grundsätzlich in temporäre zusammenhängende Speicherbereiche kopieren zu müssen.