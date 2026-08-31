# ADR-DATAMOVE-0006 – Data- und Cache-Locality

## Status

Angenommen

## Kategorie

Kernel / Data Movement / Locality / Cache / Performance

## Kontext

Die Kosten eines Datenzugriffs hängen nicht nur von der Datenmenge ab, sondern wesentlich davon, **wo sich die Daten relativ zum Consumer befinden**.

Relevant sind insbesondere:

- CPU-Caches,
- NUMA-Domänen,
- Memory Domains,
- Device Memory,
- Accelerator Memory.

Ein formal kopierfreier Datenpfad kann ineffizient sein, wenn Daten wiederholt aus entfernten oder cache-ungünstigen Speicherbereichen gelesen werden.

NovaOS muss deshalb Data- und Cache-Locality als Bestandteil seiner Data-Movement-Entscheidungen berücksichtigen.

---

## Entscheidung

NovaOS behandelt **Data- und Cache-Locality als systemweites Optimierungsziel für Datenbewegung**.

```text
Data + Consumer + Topology
            ↓
     Locality Evaluation
            ↓
Reuse / Move / Copy / Migrate
```

Daten sollen bevorzugt dort verarbeitet werden, wo sie bereits effizient verfügbar sind, oder kontrolliert dorthin bewegt werden, wo ihre weitere Nutzung insgesamt günstiger ist.

Locality ist grundsätzlich ein Soft Constraint, sofern sie nicht durch einen expliziten Contract als Hard Constraint definiert wurde.

---

## Grundprinzip

```text
Move computation to data
or data to computation
based on total cost.

Locality before nominal zero-copy.
```

---

## Data Locality

NovaOS berücksichtigt die räumliche Beziehung zwischen Daten und ihren Nutzern.

Dazu gehören insbesondere:

```text
CPU ↔ Memory
CPU ↔ NUMA Domain
Device ↔ Memory
Accelerator ↔ Memory
Producer ↔ Consumer
```

Die Hardwaretopologie aus dem Hardware System Graph bildet die Grundlage für diese Entscheidungen.

---

## Cache Locality

Neben der physischen Speicherplatzierung soll NovaOS berücksichtigen, ob Daten wahrscheinlich bereits in relevanten CPU-Caches vorhanden sind.

Kurzfristige Weiterverarbeitung auf demselben Core oder innerhalb einer gemeinsamen Cache-Domäne kann günstiger sein als eine Migration zu einem nominell weniger ausgelasteten Core.

Cache Locality ist daher auch mit dem Scheduler zu koordinieren.

---

## Verhältnis zum Scheduler

Data Placement und Task Placement dürfen nicht unabhängig voneinander optimiert werden.

```text
Move Task
Move Data
Keep Both
```

NovaOS soll die Variante wählen können, die unter den gegebenen Constraints die geringsten Gesamtkosten verursacht.

Der Scheduler kann deshalb Locality-Informationen aus Data-Movement- und Memory-Systemen als Soft Signal verwenden.

---

## NUMA

NUMA-Locality ist ein wesentlicher Bestandteil der Data Locality.

Bei Datenpipelines sollen insbesondere berücksichtigt werden:

- aktueller Memory Node,
- Producer Node,
- Consumer Node,
- NUMA Distance,
- erwartete Zugriffshäufigkeit.

Eine lokale Kopie kann einem dauerhaft entfernten Zugriff vorzuziehen sein.

---

## Shared Buffers und Zero-Copy

Shared Buffers ermöglichen die Wiederverwendung desselben Backings, garantieren aber keine optimale Locality.

```text
Shared Buffer
     ↓
Local enough?
 ├── Yes → Reuse
 └── No  → Copy / Migrate / Replan
```

Zero-Copy darf deshalb nicht automatisch Vorrang vor einer besseren Speicherplatzierung besitzen.

---

## Device- und Accelerator-Locality

Data Locality umfasst auch:

- GPU,
- NPU,
- Storage Controller,
- Network Device,
- andere Accelerators.

Bei mehreren geeigneten Devices darf die Nähe zu bereits vorhandenen Daten in die Provider- und Device-Auswahl einfließen.

Ebenso kann es sinnvoll sein, Daten gezielt in Device-lokalen Speicher zu übertragen, wenn sie dort mehrfach verwendet werden.

---

## Pipeline Locality

Bei mehrstufigen Datenpipelines soll nicht jede Stufe isoliert optimiert werden.

```text
Producer
   ↓
Stage A
   ↓
Stage B
   ↓
Stage C
```

NovaOS darf Placement-Entscheidungen anhand des erwarteten gesamten Datenpfades treffen.

Damit können unnötige Wechsel zwischen Memory Domains oder Devices vermieden werden.

---

## Adaptive Locality

NovaOS darf tatsächliche Zugriffsmuster beobachten und Locality-Policies anpassen.

```text
Predicted Access
      ↓
Actual Access
      ↓
Prediction Error
      ↓
Policy Adjustment
```

Adaptive Entscheidungen bleiben Soft Policies und dürfen Hard Constraints nicht verändern.

---

## Deterministic Mode

Deterministic Mode muss adaptive Locality-Entscheidungen kontrollieren können.

Bei Bedarf können:

- feste Placements,
- feste Affinities,
- deaktivierte Migration,
- reproduzierbare Data-Movement-Strategien

verwendet werden.

---

## Architecture Introspection

NovaOS muss relevante Locality-Entscheidungen introspektierbar machen.

Dazu gehören insbesondere:

- Datenplatzierung,
- Producer-/Consumer-Position,
- NUMA Distance,
- relevante Cache-Topologie,
- Migrationen und Kopien,
- Locality-bedingte Scheduling-Entscheidungen.

---

## Normative Anforderungen

1. NovaOS MUSS Data Locality als systemweites Optimierungsziel berücksichtigen.
2. Cache Locality SOLL bei geeigneten Workloads berücksichtigt werden.
3. Locality-Entscheidungen MÜSSEN auf der tatsächlichen Hardwaretopologie basieren können.
4. Task Placement und Data Placement SOLLEN koordiniert optimiert werden.
5. NUMA Distance MUSS als relevante Locality-Metrik verfügbar sein.
6. Zero-Copy DARF NICHT automatisch Vorrang vor besserer Locality besitzen.
7. Eine kontrollierte Kopie oder Migration DARF gewählt werden, wenn sie die Gesamtkosten reduziert.
8. Device- und Accelerator-Locality SOLLEN in Data-Movement-Entscheidungen einfließen.
9. Mehrstufige Pipelines SOLLEN als zusammenhängender Datenpfad optimierbar sein.
10. Hard Constraints MÜSSEN Vorrang vor Locality-Optimierungen besitzen.
11. Adaptive Locality-Optimierung DARF ausschließlich Soft Policies beeinflussen.
12. Deterministic Mode MUSS adaptive Locality-Entscheidungen kontrollieren können.
13. Relevante Locality-Entscheidungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- geringere NUMA- und Memory-Latenzen,
- bessere Cache-Nutzung,
- weniger unnötiger Datenverkehr,
- bessere Abstimmung zwischen Scheduler und Memory-System,
- effizientere CPU-/GPU-/NPU-Pipelines.

### Negative Konsequenzen

- Data- und Task-Placement müssen stärker koordiniert werden,
- Locality-Kosten sind hardware- und workloadabhängig,
- Migrationen oder lokale Kopien können zusätzlichen Aufwand erzeugen.

---

## Verworfene Alternativen

### Locality ausschließlich dem Scheduler überlassen

Verworfen.

Der Scheduler besitzt allein nicht genügend Kontrolle über Memory-, Buffer- und Device-Platzierung.

### Zero-Copy unabhängig von Locality bevorzugen

Verworfen.

Entfernte wiederholte Zugriffe können teurer sein als eine einmalige lokale Kopie.

### NUMA als einzige Form von Locality betrachten

Verworfen.

Cache-, Device- und Accelerator-Locality sind ebenfalls relevant.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-DATAMOVE-0001_Zero_Copy_Datenpfade`
- `ADR-DATAMOVE-0002_Shared_Buffers_als_systemweite_Abstraktion`
- `ADR-DATAMOVE-0005_Copy_Avoidance_vor_blinder_Zero_Copy_Optimierung`
- `ADR-MEMORY-0002_NUMA_Aware_Physical_Memory_Allocation`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-SCHED-0003_Topology_Aware_Scheduling`
- `ADR-SCHED-0004_NUMA_Aware_Scheduling`
- `ADR-SCHED-0005_Cache_Aware_Scheduling`
- `ADR-ARCH-0018_Locality_als_Optimierungsziel`

---

## Zugehörige NPSPECs

- `NPSPEC-DATAMOVE-LOCALITY-0001`
- `NPSPEC-DATAMOVE-CACHE-LOCALITY-0001`
- `NPSPEC-DATAMOVE-NUMA-LOCALITY-0001`
- `NPSPEC-DATAMOVE-DEVICE-LOCALITY-0001`
- `NPSPEC-DATAMOVE-PIPELINE-LOCALITY-0001`
- `NPSPEC-DATAMOVE-LOCALITY-TEST-0001`

---

## Ergebnis

NovaOS betrachtet Datenbewegung und Ausführungsort gemeinsam:

```text
Data Location
      +
Execution Location
      +
Hardware Topology
      ↓
Locality-aware Decision
```

Zero-Copy, Kopieren, Migration und Scheduling sind damit keine isolierten Optimierungen.

Die zentrale Architekturregel lautet:

```text
Optimize where data is used,
not only how it is moved.
```