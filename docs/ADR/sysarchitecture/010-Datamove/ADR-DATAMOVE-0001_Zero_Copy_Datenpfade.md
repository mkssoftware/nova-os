# ADR-DATAMOVE-0001 – Zero-Copy-Datenpfade

## Status

Angenommen

## Kategorie

Kernel / Data Movement / Zero-Copy / Memory / IPC / I/O

## Kontext

Daten werden in klassischen Systemen häufig mehrfach zwischen Anwendungen, Kernel, Treibern und Geräten kopiert.

```text
Producer
   ↓ Copy
Kernel Buffer
   ↓ Copy
Consumer
```

Solche Kopien verursachen:

- zusätzliche Speicherbandbreite,
- CPU-Last,
- Cache Pollution,
- höhere Latenz,
- zusätzlichen Energieverbrauch.

Dies wird besonders relevant bei großen Datenströmen wie:

- Storage-I/O,
- Netzwerkverkehr,
- Audio und Video,
- Kamera- und Sensordaten,
- GPU-/NPU-Workloads,
- Daten- und Compute-Pipelines.

NovaOS definiert Zero-Copy deshalb als systemweites Architekturprinzip. Zero-Copy darf jedoch Security, Isolation, Ownership oder Data Sovereignty nicht umgehen.

---

## Entscheidung

NovaOS entwirft Datenpfade grundsätzlich so, dass **unnötige Datenkopien vermieden werden können**.

Daten sollen bevorzugt über gemeinsame beziehungsweise übertragbare Memory Objects bewegt werden.

```text
Producer
    ↓
Memory Object
    ↓
Ownership / Authority Transfer
    ↓
Consumer
```

Statt den Inhalt zu kopieren, werden soweit möglich:

- Referenzen,
- Capabilities,
- Mapping-Rechte,
- Ownership

übertragen oder geteilt.

Zero-Copy ist dabei ein Optimierungsziel und keine absolute Garantie.

---

## Grundprinzip

```text
Move ownership before moving bytes.

Share backing where safe.

Copy only when required.
```

---

## Memory Objects als Grundlage

Zero-Copy-Datenpfade basieren auf expliziten Memory Objects.

Ein Datenobjekt kann dadurch zwischen mehreren Komponenten verwendet werden, ohne seine logische Identität zu verlieren.

```text
Memory Object
   ├── CPU Mapping
   ├── Consumer Mapping
   └── Device Mapping
```

Virtuelle oder physische Adressen selbst dienen nicht als systemweite Datenidentität.

---

## Ownership und Authority

Zero-Copy erfordert klare Ownership- und Zugriffsregeln.

NovaOS unterscheidet insbesondere:

```text
Ownership
Access Authority
Mapping
Physical Backing
```

Eine Übergabe kann beispielsweise die Schreibberechtigung vom Producer auf den Consumer übertragen, ohne die Nutzdaten zu kopieren.

Zero-Copy darf keine implizite gemeinsame Schreibberechtigung erzeugen.

---

## Shared Access

Wenn mehrere Teilnehmer gleichzeitig auf dieselben Daten zugreifen sollen, erfolgt dies über explizite Shared Objects und entsprechende Synchronisation.

```text
Producer ──┐
           ├── Shared Object
Consumer ──┘
```

Shared Access und Ownership Transfer sind unterschiedliche Semantiken und müssen explizit unterscheidbar bleiben.

---

## Geräte und DMA

Zero-Copy muss CPU-, Device- und Accelerator-Datenpfade einbeziehen können.

```text
Application
     ↓
Memory Object
     ↓
IOMMU / DMA Mapping
     ↓
Device
```

DMA- und IOMMU-Zugriffe unterliegen weiterhin den Hardware-, Security- und Capability-Regeln.

Zero-Copy impliziert weder unbeschränktes DMA noch dauerhaftes Pinning.

---

## Location und Locality

Eine vermiedene Kopie ist nicht automatisch die effizienteste Lösung.

Ein Memory Object kann beispielsweise physisch weit vom Consumer oder Accelerator entfernt liegen.

Die Policy darf deshalb zwischen:

```text
Reuse existing backing
```

und:

```text
Copy to better location
```

abwägen.

NUMA-, Cache-, Device- und Accelerator-Locality sind dabei relevante Optimierungsziele.

---

## Copy als zulässiger Fallback

NovaOS verbietet Kopien nicht.

Eine Kopie ist erforderlich oder sinnvoll, wenn beispielsweise:

- Isolation dies verlangt,
- Hardware kein kompatibles Sharing unterstützt,
- Data-Sovereignty-Regeln es verlangen,
- unterschiedliche Speicherklassen benötigt werden,
- Locality den Kopieraufwand rechtfertigt,
- Daten transformiert werden müssen.

```text
Zero-Copy preferred
      ↓
Not suitable
      ↓
Controlled Copy
```

Korrektheit besitzt Vorrang vor Zero-Copy.

---

## Resource Economy

Zero-Copy-Datenpfade müssen in die systemweite Resource Economy integriert sein.

Insbesondere müssen:

- Ownership,
- physische Speicherbelegung,
- Shared Usage,
- Pinning,
- DMA-Ressourcen

zurechenbar bleiben.

Shared Backing darf nicht zur Umgehung von Resource Budgets führen.

---

## Security und Data Sovereignty

Ein Zero-Copy-Pfad darf nur aufgebaut werden, wenn alle beteiligten Domains die erforderliche Authority besitzen.

Ein gemeinsames Backing stellt einen Informationsfluss dar.

Daher bleiben:

- Security,
- Trust,
- Data Sovereignty,
- Isolation

Hard Constraints.

Wenn ein direktes Sharing diese Anforderungen nicht erfüllt, muss NovaOS einen zulässigen alternativen Datenpfad verwenden oder die Operation ablehnen.

---

## Realtime und Determinismus

Zero-Copy kann Latenz und Speicherbandbreitenbedarf reduzieren und ist daher für Realtime-Systeme besonders relevant.

Realtime-Datenpfade können benötigte:

- Memory Objects,
- Mappings,
- DMA-Ressourcen,
- Pinned Memory

vorab reservieren.

Deterministic Mode muss adaptive Entscheidungen zwischen Zero-Copy und Copy kontrollieren beziehungsweise festlegen können.

---

## Architecture Introspection

NovaOS muss Datenbewegungen kontrolliert introspektierbar machen.

Mindestens relevant sind:

- Producer und Consumer,
- verwendetes Memory Object,
- Zero-Copy oder Copy,
- Ownership-Übergänge,
- kopierte Datenmenge,
- DMA-/Device-Beteiligung,
- Gründe für Copy-Fallbacks.

Damit kann sichtbar werden, wo Datenpfade unnötige Kopien erzeugen.

---

## Normative Anforderungen

1. NovaOS MUSS Zero-Copy als bevorzugtes Architekturprinzip für geeignete Datenpfade unterstützen.
2. Zero-Copy SOLL über explizite Memory Objects statt über rohe Adressen realisiert werden.
3. Ownership, Authority und Mapping MÜSSEN bei Zero-Copy getrennt modelliert bleiben.
4. Ownership Transfer MUSS ohne notwendige Kopie der Nutzdaten möglich sein.
5. Shared Access MUSS von Ownership Transfer semantisch unterschieden werden.
6. Zero-Copy MUSS zwischen Execution Domains sowie zu geeigneten Devices und Accelerators möglich sein.
7. DMA- und IOMMU-Security DÜRFEN durch Zero-Copy NICHT umgangen werden.
8. Zero-Copy DARF NICHT automatisch dauerhaftes Pinned Memory voraussetzen.
9. NUMA-, Device- und andere Locality-Kosten MÜSSEN bei der Datenpfadwahl berücksichtigt werden können.
10. NovaOS MUSS kontrollierte Kopien verwenden können, wenn Zero-Copy nicht zulässig oder nicht sinnvoll ist.
11. Security-, Trust- und Data-Sovereignty-Anforderungen MÜSSEN Vorrang vor Zero-Copy besitzen.
12. Zero-Copy-Ressourcen MÜSSEN in die Resource Economy integriert sein.
13. Realtime-Datenpfade MÜSSEN benötigte Zero-Copy-Ressourcen vorab reservieren können.
14. Datenpfade und Copy-Fallbacks MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- weniger unnötige Speichertransfers,
- geringere CPU- und Speicherbandbreitenbelastung,
- niedrigere Latenz für große Datenströme,
- effizientere CPU-/GPU-/NPU-/Device-Pipelines,
- einheitliches Datenbewegungsmodell über Systemgrenzen hinweg.

### Negative Konsequenzen

- Ownership- und Lifetime-Management werden wichtiger,
- Hardware- und Locality-Unterschiede verhindern nicht jede Kopie,
- Shared Backing benötigt explizites Security- und Resource-Accounting,
- Zero-Copy-Pfade können komplexer als einfache Copy-Pfade sein.

---

## Verworfene Alternativen

### Kopieren als Standard zwischen allen Systemkomponenten

Verworfen.

Dies erzeugt unnötige CPU-, Cache- und Speicherbandbreitenkosten.

### Zero-Copy um jeden Preis

Verworfen.

Eine Kopie kann aus Security-, Locality-, Hardware- oder Transformationsgründen notwendig beziehungsweise effizienter sein.

### Rohe Pointer als Zero-Copy-Schnittstelle

Verworfen.

Adressen besitzen keine systemweite Authority-, Ownership- oder Location-Semantik.

### Zero-Copy ausschließlich für Netzwerk und Storage

Verworfen.

Das Prinzip soll ebenso für IPC, Multimedia, Compute und Accelerators gelten.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern`
- `ADR-ARCH-0009_Einheitliches_Daten_und_Objekt_Pipelining`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0018_Locality_als_Optimierungsziel`
- `ADR-MEMORY-0002_NUMA_Aware_Physical_Memory_Allocation`
- `ADR-MEMORY-0008_Shared_Memory_über_explizite_Shared_Objects`
- `ADR-MEMORY-0014_Pinned_Memory_als_budgetierte_Ressource`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`

---

## Zugehörige NPSPECs

- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-OBJECT-0001`
- `NPSPEC-DATAMOVE-OWNERSHIP-0001`
- `NPSPEC-DATAMOVE-MAPPING-0001`
- `NPSPEC-DATAMOVE-DMA-0001`
- `NPSPEC-DATAMOVE-FALLBACK-0001`
- `NPSPEC-DATAMOVE-TEST-0001`

---

## Ergebnis

NovaOS behandelt Datenbewegung primär als Transfer von Zugriff, Ownership und Mapping statt als Transfer identischer Bytes:

```text
Producer
    ↓
Memory Object
    ↓
Ownership / Authority / Mapping
    ↓
Consumer / Device
```

Eine tatsächliche Kopie bleibt ein kontrollierter Fallback, wenn sie durch Hard Constraints erforderlich oder nach dem Kostenmodell sinnvoller ist.

Die zentrale Architekturregel lautet:

```text
Move ownership before bytes.

Zero-copy where beneficial.

Copy where necessary.
```