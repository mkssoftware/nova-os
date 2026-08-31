# ADR-DATAMOVE-0005 – Copy Avoidance vor blinder Zero-Copy-Optimierung

## Status

Angenommen

## Kategorie

Kernel / Data Movement / Zero-Copy / Performance / Resource Economy

## Kontext

Zero-Copy ist ein grundlegendes Architekturprinzip von NovaOS. Eine vollständig kopierfreie Verarbeitung ist jedoch nicht automatisch die effizienteste Lösung.

Zero-Copy kann zusätzliche Kosten verursachen, beispielsweise durch:

- komplexe Mappings,
- Pinning,
- IOMMU-Operationen,
- Synchronisation,
- schlechte NUMA- oder Device-Locality,
- Cache-Kohärenz,
- lange Buffer-Lifetimes.

Bei kleinen Datenmengen kann eine einfache Kopie günstiger sein als der Aufbau eines komplexen Zero-Copy-Pfades.

NovaOS benötigt deshalb nicht **Zero-Copy um jeden Preis**, sondern systematische **Copy Avoidance**.

---

## Entscheidung

NovaOS definiert **Copy Avoidance** als übergeordnetes Prinzip der Data-Movement-Architektur.

```text
Data Transfer
     ↓
Can existing backing be reused efficiently?
     ├── Yes → Zero-Copy / Ownership Transfer / Sharing
     └── No  → Controlled Copy
```

Zero-Copy ist die bevorzugte Umsetzung, wenn dadurch die Gesamtkosten reduziert werden.

Eine Kopie ist ausdrücklich zulässig, wenn sie notwendig oder insgesamt günstiger ist.

---

## Grundprinzip

```text
Avoid unnecessary copies.

Do not avoid necessary or beneficial copies.

Optimize total transfer cost,
not copy count alone.
```

---

## Kostenmodell

Die Entscheidung zwischen Zero-Copy und Copy darf insbesondere berücksichtigen:

- Datenmenge,
- Mapping-Kosten,
- Pinning-Kosten,
- Synchronisationskosten,
- NUMA-Locality,
- Device-Locality,
- Cache-Verhalten,
- DMA-Setup,
- IOMMU-Kosten,
- erwartete Wiederverwendung,
- Latenz- und Energieziele.

Hard Constraints werden vor diesem Kostenmodell ausgewertet.

---

## Kleine Transfers

Für kleine Datenmengen kann eine direkte Kopie effizienter sein als:

```text
Mapping
+ Authority Transfer
+ Synchronization
+ Unmapping
```

NovaOS darf deshalb abhängig vom Datenpfad und den Plattformfähigkeiten zwischen Copy und Zero-Copy wählen.

Feste universelle Größenlimits werden nicht auf ADR-Ebene definiert.

---

## Locality vor nominellem Zero-Copy

Ein vorhandener Buffer kann physisch ungünstig für den nächsten Consumer liegen.

Beispiel:

```text
Remote NUMA Memory
        ↓
Zero-Copy

versus

Local Copy
        ↓
Local Processing
```

Wenn die lokale Kopie insgesamt günstiger ist, darf NovaOS sie bevorzugen.

Dasselbe gilt für Device- und Accelerator-Memory.

---

## Security und Data Sovereignty

Zero-Copy darf keine Security- oder Information-Flow-Grenzen umgehen.

Wenn gemeinsames Backing zwischen zwei Domains nicht zulässig ist, kann eine kontrollierte Kopie in einen neuen autorisierten Speicherbereich erforderlich sein.

```text
Shared backing forbidden
        ↓
Controlled Copy
        ↓
Independent authorized object
```

Security und Data Sovereignty bleiben Hard Constraints.

---

## Ownership und Semantik

Eine Optimierungsentscheidung darf die logische Semantik eines Datenpfads nicht verändern.

Insbesondere müssen unabhängig von Copy oder Zero-Copy erhalten bleiben:

- Datenidentität,
- Ownership-Regeln,
- Zugriffsrechte,
- Synchronisationsanforderungen,
- Fehlersemantik.

Für Consumer soll die gewählte physische Transferstrategie soweit möglich transparent bleiben.

---

## Adaptive Auswahl

NovaOS darf aus vergangenen Transfers lernen, welche Strategie für vergleichbare Situationen günstiger ist.

```text
Prediction
    ↓
Transfer Strategy
    ↓
Measured Result
    ↓
Prediction Error
    ↓
Policy Adjustment
```

Adaptive Entscheidungen bleiben Soft Policies.

Explizite Anforderungen und Hard Constraints besitzen Vorrang.

---

## Deterministic Mode

Deterministic Mode muss adaptive Copy-/Zero-Copy-Entscheidungen kontrollieren können.

Für deterministische Workloads können:

- feste Schwellenwerte,
- feste Transferstrategien,
- deaktiviertes adaptives Lernen

verwendet werden.

---

## Architecture Introspection

NovaOS muss sichtbar machen können:

- ob ein Transfer kopiert wurde,
- ob Zero-Copy verwendet wurde,
- wie viele Bytes kopiert wurden,
- welcher Provider verwendet wurde,
- warum ein Copy-Fallback gewählt wurde.

Damit kann Copy Avoidance systemweit analysiert und optimiert werden.

---

## Normative Anforderungen

1. NovaOS MUSS Copy Avoidance als übergeordnetes Ziel der Data-Movement-Architektur behandeln.
2. Zero-Copy SOLL verwendet werden, wenn es die Gesamtkosten eines Transfers reduziert.
3. Zero-Copy DARF NICHT unabhängig von seinen Mapping-, Pinning-, Synchronisations- und Locality-Kosten bevorzugt werden.
4. Kontrollierte Kopien MÜSSEN zulässig bleiben, wenn sie erforderlich oder insgesamt günstiger sind.
5. Hard Constraints MÜSSEN vor Performanceentscheidungen zwischen Copy und Zero-Copy ausgewertet werden.
6. NUMA-, Device- und Cache-Locality SOLLEN bei der Transferentscheidung berücksichtigt werden.
7. Kleine Transfers DÜRFEN direkt kopiert werden, wenn Zero-Copy-Setup unverhältnismäßig teuer wäre.
8. Security- und Data-Sovereignty-Anforderungen DÜRFEN durch Zero-Copy NICHT abgeschwächt werden.
9. Die Transferstrategie DARF die logische Daten- und Ownership-Semantik NICHT verändern.
10. Adaptive Strategiewahl DARF ausschließlich Soft Policies beeinflussen.
11. Deterministic Mode MUSS reproduzierbare Transferentscheidungen ermöglichen.
12. Copy- und Zero-Copy-Entscheidungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Zero-Copy wird dort eingesetzt, wo es tatsächlich Vorteile bringt,
- unnötige Mapping- und Pinning-Kosten werden vermieden,
- NUMA- und Device-Locality können stärker berücksichtigt werden,
- kleine Transfers bleiben effizient,
- NovaOS optimiert die Gesamtkosten statt einer einzelnen Metrik.

### Negative Konsequenzen

- Transferentscheidungen benötigen ein Kostenmodell,
- Datenpfade können abhängig von Laufzeitbedingungen unterschiedliche Strategien verwenden,
- Performanceanalyse wird komplexer als bei einer festen Transferstrategie.

---

## Verworfene Alternativen

### Zero-Copy als absolute Regel

Verworfen.

Zero-Copy kann in bestimmten Situationen teurer als eine kontrollierte Kopie sein.

### Copy als Standard

Verworfen.

Dies würde unnötige Speicherbandbreite und CPU-Zeit verbrauchen.

### Feste globale Copy-Schwelle

Verworfen.

Die optimale Strategie hängt von Hardware, Locality, Datenpfad und Laufzeitbedingungen ab.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-DATAMOVE-0001_Zero_Copy_Datenpfade`
- `ADR-DATAMOVE-0002_Shared_Buffers_als_systemweite_Abstraktion`
- `ADR-DATAMOVE-0003_Scatter_Gather_IO`
- `ADR-DATAMOVE-0004_DMA_Based_Transfer`
- `ADR-MEMORY-0002_NUMA_Aware_Physical_Memory_Allocation`
- `ADR-MEMORY-0014_Pinned_Memory_als_budgetierte_Ressource`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0018_Locality_als_Optimierungsziel`

---

## Zugehörige NPSPECs

- `NPSPEC-DATAMOVE-COPY-AVOIDANCE-0001`
- `NPSPEC-DATAMOVE-COST-MODEL-0001`
- `NPSPEC-DATAMOVE-STRATEGY-0001`
- `NPSPEC-DATAMOVE-LOCALITY-0001`
- `NPSPEC-DATAMOVE-ADAPTIVE-0001`
- `NPSPEC-DATAMOVE-COPY-AVOIDANCE-TEST-0001`

---

## Ergebnis

NovaOS optimiert nicht auf eine möglichst kleine Anzahl formaler Kopieroperationen, sondern auf einen möglichst effizienten Gesamtdatenpfad.

```text
Hard Constraints
      ↓
Eligible Transfer Strategies
      ↓
Total Cost Evaluation
      ↓
Zero-Copy / DMA / Mapping / Copy
```

Die zentrale Architekturregel lautet:

```text
Avoid unnecessary copies.

Zero-copy is a means,
not the goal.
```