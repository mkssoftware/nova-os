# ADR-IO-0004 – Zero-Copy I/O Pipelines

## Status

Angenommen

## Kategorie

I/O / Data Movement / Zero-Copy / Pipelines / Performance

## Kontext

I/O-Daten durchlaufen häufig mehrere Systemkomponenten.

Ein klassischer Datenpfad kann dabei unnötige Kopien erzeugen:

```text
Device
  ↓
Kernel Buffer
  ↓ copy
Application Buffer
  ↓ copy
Library Buffer
  ↓ copy
Another I/O Buffer
  ↓
Device
```

Diese Kopien verursachen Speicherbandbreite, CPU-Zeit, Cache-Verdrängung, zusätzliche Latenz und Energieverbrauch.

NovaOS besitzt bereits gemeinsame Shared Buffers, DMA-basierte Transfers, Scatter/Gather I/O und Copy Avoidance. Diese Mechanismen sollen auch über vollständige I/O-Pipelines hinweg zusammenarbeiten.

---

## Entscheidung

NovaOS entwirft I/O-Pipelines grundsätzlich für **Zero-Copy beziehungsweise Copy-Avoidance zwischen Pipeline-Stufen**.

```text
Producer
   ↓
Shared Buffer
   ↓
Stage A
   ↓
Stage B
   ↓
Consumer
```

Pipeline-Stufen sollen Daten nach Möglichkeit über gemeinsame Buffer-Identitäten, Mappings oder Ownership Transfer weitergeben, anstatt neue Byte-Kopien zu erzeugen.

Zero-Copy bleibt dabei ein Optimierungsziel und keine absolute Anforderung.

---

## Grundprinzip

```text
Move ownership and references
before moving bytes.
```

---

## Gemeinsame Buffer

Zero-Copy I/O Pipelines verwenden die systemweite Shared-Buffer-Abstraktion.

```text
I/O Request
     ↓
Shared Buffer
     ↓
Processing
     ↓
Next I/O Request
```

Eine Pipeline soll dadurch denselben zugrunde liegenden Buffer über mehrere Stufen weiterreichen können.

Die einzelnen Komponenten dürfen unterschiedliche virtuelle Mappings besitzen, ohne dass daraus eine Datenkopie entstehen muss.

---

## Ownership Transfer

Wenn eine Pipeline-Stufe ihre Daten nach der Übergabe nicht mehr benötigt, soll Ownership Transfer bevorzugt werden.

```text
Producer owns Buffer
        ↓
Transfer Ownership
        ↓
Consumer owns Buffer
```

Damit können unnötige gemeinsame Schreibzugriffe und zusätzliche Kopien vermieden werden.

Shared Access bleibt möglich, wenn mehrere Pipeline-Stufen gleichzeitig Zugriff benötigen.

---

## Geräteübergreifende Pipelines

Zero-Copy Pipelines sollen CPU-, Storage-, Netzwerk- und Accelerator-I/O über dasselbe Data-Movement-Modell verbinden können.

Beispiel:

```text
Storage
   ↓ DMA
Shared Buffer
   ↓
GPU / NPU
   ↓
Shared Buffer
   ↓ DMA
Network
```

IOMMU- und DMA-Mechanismen können dafür verwendet werden, sofern Hardware, Security und Memory Properties dies erlauben.

---

## Copy Fallback

Eine Kopie ist zulässig und teilweise ausdrücklich vorzuziehen, wenn sie für Korrektheit, Isolation oder Gesamteffizienz erforderlich ist.

Gründe können beispielsweise sein:

- inkompatible Memory Properties,
- Security- oder Trust-Grenzen,
- Data-Sovereignty-Anforderungen,
- ungeeignete NUMA- oder Device-Locality,
- notwendige Datenkonvertierung,
- Hardwarebeschränkungen.

```text
Zero-Copy possible?
    ├── Yes → Reuse / Transfer Buffer
    └── No  → Controlled Copy
```

Die Architektur folgt damit Copy Avoidance und nicht blindem Zero-Copy.

---

## Pipeline-Optimierung

Die Entscheidung darf nicht ausschließlich für jede einzelne Pipeline-Stufe isoliert erfolgen.

NovaOS soll den Datenpfad als zusammenhängende Pipeline betrachten können.

Eine lokal optimale Zero-Copy-Entscheidung kann global ungünstig sein, wenn dadurch beispielsweise spätere Stufen auf entfernten Speicher zugreifen müssen.

```text
Pipeline
   ↓
Data Movement + Locality Evaluation
   ↓
Best permitted path
```

---

## Completion und Synchronisation

Buffer-Weitergabe muss mit dem Completion-Based-I/O-Modell koordiniert werden.

Ein Buffer darf erst wiederverwendet oder an eine inkompatible nächste Verwendung übergeben werden, wenn die vorherige Nutzung entsprechend abgeschlossen ist.

Die Synchronisation soll über die gemeinsamen Completion-, Fence-, Event- oder Future-Mechanismen erfolgen.

---

## Resource Economy

Zero-Copy ist nicht kostenlos.

Shared Buffers können Ressourcen wie:

- Pinned Memory,
- IOMMU Mappings,
- DMA Descriptors,
- Device Memory,
- längere Buffer-Lebensdauer

binden.

Diese Kosten müssen in die systemweite Resource Economy einfließen.

Eine Kopie darf gewählt werden, wenn sie insgesamt günstiger ist.

---

## Security und Data Sovereignty

Gemeinsame Buffer dürfen Sicherheitsgrenzen nicht umgehen.

Jede Pipeline-Stufe muss für den erforderlichen Zugriff autorisiert sein.

Ein Buffer darf nicht allein deshalb zugänglich werden, weil eine vorherige Pipeline-Stufe Zugriff darauf besaß.

Security, Trust und Data Sovereignty bleiben Hard Constraints.

---

## Determinismus und Realtime

Realtime-Pipelines können Buffer, DMA-Ressourcen und erforderliche Mappings vorab reservieren.

Deterministic Mode kann adaptive Entscheidungen über Copy-, Mapping- oder Placement-Strategien einschränken oder durch feste Policies ersetzen.

---

## Normative Anforderungen

1. NovaOS MUSS I/O-Pipelines für Copy Avoidance und Zero-Copy optimierbar gestalten.
2. Pipeline-Stufen SOLLEN gemeinsame Shared Buffers verwenden können.
3. Ownership Transfer SOLL bevorzugt werden, wenn der bisherige Besitzer den Buffer nicht weiter benötigt.
4. Zero-Copy-Pipelines MÜSSEN über mehrere I/O-Subsysteme und Geräteklassen hinweg möglich sein können.
5. DMA und IOMMU MÜSSEN in Zero-Copy-I/O-Pipelines integrierbar sein.
6. Zero-Copy DARF NICHT als absolute Anforderung behandelt werden.
7. Kontrollierte Kopien MÜSSEN zulässig sein, wenn Hard Constraints oder Gesamtkosten dies erfordern.
8. Pipeline-Optimierung SOLL den gesamten relevanten Datenpfad berücksichtigen können.
9. Buffer-Lebensdauer und Wiederverwendung MÜSSEN mit I/O-Completions synchronisiert werden.
10. Zero-Copy-Ressourcen MÜSSEN der Resource Economy zurechenbar sein.
11. Shared Buffers DÜRFEN Security-, Trust- oder Data-Sovereignty-Grenzen NICHT umgehen.
12. Locality MUSS bei der Auswahl zwischen Sharing, Mapping, Migration und Copy berücksichtigt werden können.
13. Realtime-Pipelines MÜSSEN erforderliche Buffer- und Data-Movement-Ressourcen reservieren können.
14. Deterministic Mode MUSS adaptive Zero-Copy-Entscheidungen kontrollieren können.

---

## Konsequenzen

### Positive Konsequenzen

- weniger unnötige Speicheroperationen,
- geringerer CPU- und Speicherbandbreitenverbrauch,
- niedrigere I/O-Latenzen,
- effiziente CPU-/GPU-/NPU-/Device-Pipelines,
- gemeinsame Datenpfade über unterschiedliche I/O-Subsysteme,
- bessere Grundlage für Multimedia-, Netzwerk- und Compute-Workloads.

### Negative Konsequenzen

- Buffer-Lifetime und Ownership werden wichtiger,
- Hardware kann unterschiedliche Memory-Anforderungen besitzen,
- Zero-Copy kann zusätzliche Mapping- und Synchronisationskosten verursachen,
- Pipeline-weite Optimierung ist komplexer als lokale Copy-Entscheidungen.

---

## Verworfene Alternativen

### Kopie zwischen jeder I/O-Schicht

Verworfen.

Dies würde unnötig CPU-Zeit, Speicherbandbreite und Energie verbrauchen.

### Zero-Copy unter allen Umständen erzwingen

Verworfen.

Eine lokale Kopie kann aufgrund von Locality, Security oder Hardwareanforderungen die bessere Lösung sein.

### Eigenes Buffer-Modell pro I/O-Subsystem

Verworfen.

Dies würde systemweite Zero-Copy-Pipelines erheblich erschweren.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-IO-0001_Completion_Based_Asynchronous_IO`
- `ADR-IO-0002_Einheitliches_IO_Request_Modell`
- `ADR-DATAMOVE-0001_Zero_Copy_Datenpfade`
- `ADR-DATAMOVE-0002_Shared_Buffers_als_systemweite_Abstraktion`
- `ADR-DATAMOVE-0003_Scatter_Gather_IO`
- `ADR-DATAMOVE-0004_DMA_Based_Transfer`
- `ADR-DATAMOVE-0005_Copy_Avoidance_vor_blinder_Zero_Copy_Optimierung`
- `ADR-DATAMOVE-0006_Data_und_Cache_Locality`
- `ADR-MEMORY-0014_Pinned_Memory_als_budgetierte_Ressource`

---

## Zugehörige NPSPECs

- `NPSPEC-IO-ZEROCOPY-0001`
- `NPSPEC-IO-PIPELINE-0001`
- `NPSPEC-IO-PIPELINE-BUFFER-0001`
- `NPSPEC-IO-PIPELINE-OWNERSHIP-0001`
- `NPSPEC-IO-PIPELINE-DEVICE-0001`
- `NPSPEC-IO-PIPELINE-FALLBACK-0001`
- `NPSPEC-IO-ZEROCOPY-TEST-0001`

---

## Ergebnis

NovaOS behandelt I/O-Daten als weiterreichbare Buffer statt als Bytefolgen, die an jeder Systemgrenze kopiert werden:

```text
Producer
   ↓
Shared Buffer
   ↓
Processing
   ↓
Shared Buffer
   ↓
Consumer
```

Kopien bleiben erlaubt, wenn sie technisch notwendig oder insgesamt effizienter sind.

Die zentrale Architekturregel lautet:

```text
Pass the buffer when possible.

Copy the data only when justified.
```