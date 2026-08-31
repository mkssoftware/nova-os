# ADR-DATAMOVE-0004 – DMA-Based Transfer

## Status

Angenommen

## Kategorie

Kernel / Data Movement / DMA / IOMMU / Zero-Copy

## Kontext

Viele I/O- und Accelerator-Datenpfade können Daten direkt zwischen Speicher und Hardware übertragen, ohne dass die CPU die Nutzdaten selbst kopieren muss.

```text
CPU Copy:
Memory → CPU → Device

DMA:
Memory ───────→ Device
```

DMA reduziert CPU-Last und kann Latenz, Speicherbandbreitenverbrauch und Energiebedarf senken.

Direkter Gerätezugriff auf Speicher erzeugt jedoch zusätzliche Anforderungen an Isolation, Address Translation, Pinning, Ownership und Lifetime.

---

## Entscheidung

NovaOS behandelt DMA als **native Data-Movement-Funktion** und bevorzugt DMA-basierte Transfers für geeignete Geräte und Datenpfade.

```text
Shared Buffer
      ↓
DMA Preparation
      ↓
IOMMU Mapping
      ↓
Device
```

DMA wird über HAL- und Data-Movement-Abstraktionen bereitgestellt. Anwendungen und allgemeine Systemkomponenten arbeiten mit Shared Buffers beziehungsweise Memory Objects und nicht mit rohen physischen Adressen.

DMA bleibt eine kontrollierte Systemressource.

---

## Grundprinzip

```text
Let devices move data directly
when safe and efficient.

Never trade isolation
for transfer performance.
```

---

## DMA und IOMMU

Wenn eine IOMMU verfügbar ist, soll NovaOS sie zur Isolation und Adressübersetzung von DMA-Zugriffen verwenden.

```text
Device
   ↓
IOMMU Domain
   ↓
Authorized Memory
```

Ein Device darf nur auf Speicher zugreifen, der für den jeweiligen DMA-Context explizit freigegeben wurde.

Fehlende IOMMU-Unterstützung darf nicht automatisch zu uneingeschränktem physischem Speicherzugriff führen.

---

## Shared Buffers

DMA-basierte Transfers verwenden bevorzugt die systemweite Shared-Buffer-Abstraktion.

```text
Producer
    ↓
Shared Buffer
    ↓
DMA
    ↓
Device / Consumer
```

Dadurch bleiben:

- Ownership,
- Authority,
- Lifetime,
- Resource Accounting

über den gesamten Datenpfad erhalten.

---

## Pinning und Lifetime

DMA kann erfordern, dass physisches Backing während eines Transfers stabil bleibt.

Notwendiges Pinning erfolgt deshalb über die budgetierte Pinned-Memory-Architektur.

Ein DMA-Transfer hält die benötigten Memory Objects und Mappings mindestens für seine eigene Lifetime gültig.

Nach Abschluss sollen temporäre Pins und DMA-Mappings wieder freigegeben werden.

---

## Scatter/Gather DMA

DMA muss mit Scatter/Gather I/O kombinierbar sein.

```text
Buffer Segments
      ↓
Scatter/Gather Mapping
      ↓
DMA
      ↓
Device
```

Physisch nicht zusammenhängende Daten sollen nicht allein aufgrund von DMA-Anforderungen vorab in einen zusammenhängenden Buffer kopiert werden müssen.

---

## Coherency

NovaOS darf Cache-Coherency zwischen CPU und Device nicht allgemein voraussetzen.

Die jeweilige Plattform beziehungsweise das Device-Modell muss beschreiben, welche Synchronisations- oder Cache-Operationen für einen korrekten Ownership-Übergang erforderlich sind.

Die generische Data-Movement-Abstraktion bleibt von der konkreten Hardwareimplementierung getrennt.

---

## DMA Engines

Neben geräteeigenem DMA dürfen auch dedizierte DMA- beziehungsweise Copy-Engines als Data-Movement-Provider verwendet werden.

NovaOS kann damit geeignete Transfers von der CPU auf spezialisierte Hardware auslagern.

Die Auswahl bleibt policygesteuert und darf einen CPU-basierten Transfer verwenden, wenn dieser für kleine oder ungeeignete Transfers günstiger ist.

---

## Resource Economy

DMA-Ressourcen werden in die systemweite Resource Economy integriert.

Dies betrifft insbesondere:

- DMA Channels,
- Descriptoren,
- IOMMU-Mappings,
- Pinned Memory,
- Transfer Queues,
- Hardware Engines.

Ein Workload darf durch DMA keine Memory- oder Device-Budgets umgehen.

---

## Security und Data Sovereignty

DMA unterliegt denselben Hard Constraints wie andere Datenpfade.

Insbesondere müssen:

- Device Authority,
- Memory Authority,
- Isolation,
- Trust,
- Data Sovereignty

vor Aufbau eines DMA-Pfads erfüllt sein.

Ein Device darf niemals allein aufgrund seiner DMA-Fähigkeit Zugriff auf nicht autorisierte Daten erhalten.

---

## Realtime und Determinismus

Realtime-Domains müssen benötigte DMA-Ressourcen vorab reservieren können.

Dazu können gehören:

```text
Shared Buffers
Pinned Memory
IOMMU Mappings
DMA Descriptors
DMA Channels
```

Deterministic Mode muss adaptive Provider- und Transferentscheidungen kontrollieren können.

---

## Fallback

DMA ist nicht für jeden Transfer die optimale oder verfügbare Lösung.

NovaOS muss deshalb alternative Transferpfade unterstützen.

```text
DMA suitable
    ↓ yes
DMA Transfer

    ↓ no
CPU / Controlled Copy
```

Ein Fallback darf Security- oder Isolationseigenschaften nicht abschwächen.

---

## Architecture Introspection

DMA-Datenpfade müssen kontrolliert introspektierbar sein.

Relevant sind insbesondere:

- beteiligtes Device,
- Shared Buffer beziehungsweise Memory Object,
- Transfergröße,
- DMA-/IOMMU-Nutzung,
- Pinning,
- verwendeter Data-Movement-Provider,
- Fallbacks und Fehler.

---

## Normative Anforderungen

1. NovaOS MUSS DMA als native Data-Movement-Funktion unterstützen.
2. DMA SOLL für geeignete I/O- und Accelerator-Datenpfade bevorzugt werden.
3. Allgemeine DMA-Schnittstellen DÜRFEN NICHT von rohen physischen Adressen abhängig sein.
4. DMA SOLL auf Shared Buffers beziehungsweise expliziten Memory Objects aufbauen.
5. DMA-Zugriffe MÜSSEN auf autorisierte Speicherbereiche begrenzt werden.
6. Verfügbare IOMMU-Isolation SOLL für DMA verwendet werden.
7. Fehlende IOMMU-Unterstützung DARF NICHT automatisch uneingeschränkten DMA-Zugriff erlauben.
8. Für DMA erforderliches Pinning MUSS über die budgetierte Pinned-Memory-Architektur erfolgen.
9. DMA MUSS mit Scatter/Gather I/O kombinierbar sein.
10. Cache- und Device-Coherency MÜSSEN entsprechend den Plattformfähigkeiten korrekt behandelt werden.
11. DMA-Ressourcen MÜSSEN in die Resource Economy integriert sein.
12. Security-, Trust- und Data-Sovereignty-Constraints MÜSSEN Vorrang vor DMA-Optimierungen besitzen.
13. Realtime-Domains MÜSSEN benötigte DMA-Ressourcen reservieren können.
14. NovaOS MUSS einen kontrollierten Nicht-DMA-Fallback unterstützen.
15. DMA-Nutzung, Ressourcenbindung und Fallbacks MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- geringere CPU-Belastung bei großen Datentransfers,
- bessere Zero-Copy-I/O-Pfade,
- effiziente Nutzung von Storage-, Netzwerk- und Accelerator-Hardware,
- direkte Integration mit Shared Buffers und Scatter/Gather,
- kontrollierte DMA-Isolation über IOMMU.

### Negative Konsequenzen

- zusätzliche DMA-/IOMMU-Ressourcenverwaltung,
- Pinning kann Memory Management einschränken,
- nicht kohärente Hardware benötigt zusätzliche Synchronisation,
- kleine Transfers können durch DMA-Setup teurer werden als CPU-Kopien.

---

## Verworfene Alternativen

### DMA nur als treiberinterne Implementierungsentscheidung

Verworfen.

DMA ist für systemweite Zero-Copy- und Data-Movement-Pfade architektonisch relevant.

### Direkte physische Adressen als allgemeine DMA-Schnittstelle

Verworfen.

Dies würde Memory-Abstraktion, Isolation und Portabilität schwächen.

### DMA für jeden Transfer erzwingen

Verworfen.

Setup-Kosten und Hardwareeigenschaften können einen CPU-basierten Transfer effizienter machen.

### DMA ohne Resource Accounting

Verworfen.

DMA verwendet begrenzte Memory-, IOMMU- und Device-Ressourcen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-DATAMOVE-0001_Zero_Copy_Datenpfade`
- `ADR-DATAMOVE-0002_Shared_Buffers_als_systemweite_Abstraktion`
- `ADR-DATAMOVE-0003_Scatter_Gather_IO`
- `ADR-MEMORY-0014_Pinned_Memory_als_budgetierte_Ressource`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

---

## Zugehörige NPSPECs

- `NPSPEC-DATAMOVE-DMA-0001`
- `NPSPEC-DATAMOVE-DMA-MAPPING-0001`
- `NPSPEC-DATAMOVE-DMA-TRANSFER-0001`
- `NPSPEC-DATAMOVE-DMA-COHERENCY-0001`
- `NPSPEC-DATAMOVE-DMA-RESOURCE-0001`
- `NPSPEC-DATAMOVE-DMA-TEST-0001`

---

## Ergebnis

DMA wird zu einem integrierten Bestandteil der NovaOS-Data-Movement-Architektur:

```text
Shared Buffer
      ↓
Authorized DMA Mapping
      ↓
IOMMU
      ↓
DMA Provider
      ↓
Device
```

Damit können geeignete Daten direkt zwischen Speicher und Hardware bewegt werden, während Ownership, Isolation, Resource Accounting und Data Sovereignty erhalten bleiben.