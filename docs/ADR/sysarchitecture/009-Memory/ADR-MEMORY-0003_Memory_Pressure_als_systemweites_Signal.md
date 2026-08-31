# ADR-MEMORY-0003 – Memory Pressure als systemweites Signal

## Status

Angenommen

## Kategorie

Kernel / Memory Management / Resource Economy / Memory Pressure / System Policy

## Kontext

Freier Speicher allein ist kein ausreichendes Maß für den Zustand des Speichersystems.

Ein System kann beispielsweise noch freien Speicher besitzen und trotzdem unter erheblichem Druck stehen, weil:

- bestimmte NUMA-Domains nahezu ausgelastet sind,
- physisch zusammenhängender Speicher knapp wird,
- nicht verschiebbare Allokationen wachsen,
- Realtime-Reservations geschützt werden müssen,
- große Mengen reclaimable Memory vorhanden sind,
- Working Sets miteinander konkurrieren,
- Device- oder Accelerator-Memory knapp wird,
- Reclamation bereits hohe CPU- oder I/O-Kosten erzeugt.

Memory Pressure betrifft außerdem nicht nur den Memory Manager.

Auch:

- Scheduler,
- Resource Economy,
- Execution Domains,
- Cache-Subsysteme,
- Anwendungen,
- Systemdienste,
- Compute Runtime,
- GPU/NPU-Runtimes

können auf Speicherdruck reagieren.

NovaOS benötigt deshalb Memory Pressure als explizites systemweites Signal statt als rein interne PMM-Heuristik.

---

## Entscheidung

NovaOS definiert **Memory Pressure als systemweit verfügbares, hierarchisches Ressourcensignal**.

```text
Physical Memory State
        ↓
Memory Manager
        ↓
Pressure Evaluation
        ↓
Memory Pressure Signal
        ↓
Resource Economy
        ↓
System / Domain / Scope Policies
```

Memory Pressure beschreibt nicht lediglich die Menge freien Speichers, sondern den zunehmenden Aufwand und das zunehmende Risiko, weitere Speicheranforderungen zuverlässig zu erfüllen.

Das Signal darf sowohl global als auch für einzelne Memory Domains und Ressourcenhierarchien existieren.

---

## Grundprinzip

```text
Memory pressure is not
"memory is full".

Memory pressure means
"memory is becoming expensive or difficult to satisfy".
```

---

## Pressure-Zustände

NovaOS verwendet semantische Pressure-Zustände.

Konzeptionell:

```text
Normal
  ↓
Elevated
  ↓
Constrained
  ↓
Critical
```

Die exakten Schwellenwerte sind Policy und dürfen abhängig sein von:

- Hardware,
- Memory Domain,
- Memory Class,
- Workload,
- Reservations,
- Reclamation-Kosten.

Die Zustände dürfen deshalb nicht ausschließlich an einen globalen Prozentwert freien Speichers gekoppelt werden.

---

## Pressure-Metriken

Zur Bewertung können unter anderem berücksichtigt werden:

- verfügbare physische Kapazität,
- reservierte Kapazität,
- reclaimable Memory,
- Reclamation Rate,
- Allocation Failure Rate,
- Fragmentierung,
- NUMA-lokale Verfügbarkeit,
- nicht verschiebbare Allokationen,
- Working-Set-Verhalten,
- Swap-/Backing-Store-Aktivität,
- Memory-Compaction-Kosten.

Nicht jede Plattform muss jede Metrik bereitstellen.

---

## Hierarchisches Pressure-Modell

Memory Pressure kann auf unterschiedlichen Ebenen auftreten.

```text
System
 ├── Memory Domain 0
 ├── Memory Domain 1
 └── Execution Domains
      ├── Domain A
      └── Domain B
```

Beispiele:

```text
Global Pressure: Normal
NUMA Node 0: Critical
NUMA Node 1: Normal
```

oder:

```text
System Pressure: Elevated
Execution Domain A: Within Budget
Execution Domain B: Over Budget
```

Lokaler Pressure darf nicht automatisch als globaler Speichermangel interpretiert werden.

---

## Resource Economy

Memory Pressure wird mit der systemweiten Resource Economy integriert.

Diese kennt insbesondere:

- Ownership,
- Budgets,
- Reservations,
- Limits,
- Prioritäten,
- Degradation Policies.

Dadurch kann NovaOS unterscheiden zwischen:

```text
Memory is globally scarce

und

One domain exceeds its budget
```

Ein speicherintensiver Execution Domain darf nicht automatisch geschützte Ressourcen anderer Domains verdrängen.

---

## Pressure und Reservations

Reservierter Speicher wird bei der Pressure-Bewertung berücksichtigt.

Freier, aber reservierter Speicher ist nicht automatisch für allgemeine Allokationen verfügbar.

Beispiel:

```text
Physical Free:        2 GiB
Realtime Reserved:    1.5 GiB
General Available:    0.5 GiB
```

Die Pressure-Bewertung muss die tatsächlich nutzbare Kapazität widerspiegeln.

---

## NUMA Pressure

Memory Pressure wird NUMA-aware behandelt.

Ein NUMA Node kann unter hohem Pressure stehen, obwohl andere Nodes noch freie Kapazität besitzen.

Bei Soft-Locality-Anforderungen darf der PMM deshalb entfernte zulässige Domains verwenden.

```text
Preferred Node
      ↓
High Pressure
      ↓
Remote Allowed?
   ┌──┴──┐
  Yes    No
   ↓      ↓
Fallback  Allocation Failure /
          Reclamation
```

Required NUMA Constraints bleiben verbindlich.

---

## Reclamation

Memory Pressure ist ein wesentlicher Trigger für Reclamation.

Reclamation darf jedoch nicht erst beginnen, wenn Allokationen bereits scheitern.

NovaOS soll frühzeitig auf steigenden Pressure reagieren können.

Mögliche Maßnahmen sind:

```text
Drop Reconstructable Cache
Reclaim Unused Memory
Compact Memory
Reduce Optional Working Sets
Request Cooperative Release
Use Backing Store
```

Die konkrete Reihenfolge gehört in die jeweiligen NPSPECs und Policies.

---

## Cooperative Memory Release

Execution Domains und Systemdienste können auf Memory-Pressure-Signale reagieren und freiwillig entbehrlichen Speicher freigeben.

Beispiele:

- Caches verkleinern,
- vorgerenderte Inhalte verwerfen,
- speculative/preloaded Daten entfernen,
- optionale AI-Modelle entladen,
- temporäre Buffers reduzieren.

Dies erlaubt semantisch bessere Entscheidungen als ausschließlich blindes Kernel-Reclamation.

---

## Graceful Degradation

Memory Pressure wird mit Graceful Degradation verbunden.

Unter zunehmendem Druck können zunächst optionale Funktionen reduziert werden:

```text
Normal
  ↓
Reduce speculative allocations
  ↓
Shrink optional caches
  ↓
Reduce background workloads
  ↓
Reclaim aggressively
  ↓
Protect critical workloads
```

Hard Constraints und geschützte Reservations besitzen Vorrang.

---

## Scheduler-Integration

Der Scheduler darf Memory Pressure als Soft-Policy-Signal berücksichtigen.

Beispiele:

- speicherintensive Background-Arbeit verzögern,
- Tasks näher an Domains mit ausreichender Kapazität platzieren,
- optionalen Parallelismus reduzieren,
- NUMA-Placement anpassen.

Memory Pressure darf jedoch nicht eigenständig Security-, Realtime-, Affinity- oder andere Hard Constraints überschreiben.

---

## Adaptive Parallelism

Parallelisierung kann zusätzlichen Speicherbedarf erzeugen.

Beispiel:

```text
16 Workers
   ↓
16 Working Sets
   ↓
High Memory Pressure
```

NovaOS darf deshalb unter Pressure den optionalen Parallelismus reduzieren:

```text
16 → 8 → 4 Workers
```

sofern der Execution Contract dies erlaubt.

---

## Preloading und Prediction

Vorhersagegestütztes Preloading ist optionaler Ressourcenverbrauch.

Bei steigendem Memory Pressure soll NovaOS:

- neue Preloads reduzieren,
- geringe Confidence stärker berücksichtigen,
- bereits vorgeladene, leicht rekonstruierbare Daten bevorzugt freigeben.

Prediction darf niemals kritischen Speicher verdrängen.

---

## Accelerator Memory

GPU-, NPU- und andere Accelerator-Runtimes können eigene Memory Domains besitzen.

Pressure dieser Ressourcen muss in dasselbe systemweite Ressourcenmodell integrierbar sein.

Beispiel:

```text
System RAM: Normal
GPU Memory: Critical
```

Dies kann eine Provider- oder Algorithmusauswahl beeinflussen, sofern der Execution Contract einen alternativen Provider erlaubt.

---

## Zero-Copy

Zero-Copy-Buffers können gleichzeitig von mehreren Komponenten referenziert werden und dadurch schwieriger reclaimbar sein.

Pressure-Diagnostics müssen deshalb zwischen:

- allokiertem Speicher,
- reclaimable Memory,
- pinned Memory,
- shared Zero-Copy Memory

unterscheiden können.

Zero-Copy darf nicht automatisch als kostenloser Speicherverbrauch betrachtet werden.

---

## Realtime

Realtime-Reservations werden vor allgemeiner Pressure-Reclamation geschützt.

Memory Pressure darf eine garantierte Realtime-Reservation nicht still aufheben.

Kann eine garantierte Ressourcenzusage aufgrund einer unvermeidbaren Systemstörung nicht mehr erfüllt werden, muss dies als explizite Contract-Verletzung behandelt werden.

---

## Critical Pressure

Critical Pressure bedeutet, dass normale Soft-Policy-Maßnahmen nicht mehr ausreichend sind.

NovaOS darf dann stärkere, definierte Maßnahmen auslösen.

Beispielsweise:

```text
Aggressive Reclamation
Cancel Optional Work
Throttle Memory Consumers
Reject New Allocations
Restart Recoverable Services
Terminate Contract-defined Domains
```

Die Auswahl erfolgt über explizite Resource- und Supervision-Policies.

Ein zufälliges Beenden beliebiger Prozesse ist kein vorgesehenes Basismodell.

---

## Allocation Failure

Memory Pressure und Allocation Failure sind unterschiedliche Zustände.

```text
High Pressure != Allocation Failure

Low Global Pressure != Allocation Guaranteed
```

Eine spezifische Allokation kann beispielsweise aufgrund von:

- Required NUMA Placement,
- physischer Kontiguität,
- DMA Constraints,
- Memory Class,
- Fragmentierung

scheitern, obwohl global noch viel Speicher verfügbar ist.

---

## Hysterese

Pressure-Zustände dürfen nicht bei kleinen Schwankungen ständig wechseln.

Die Pressure Engine muss geeignete Stabilitätsmechanismen unterstützen:

- Hysterese,
- geglättete Metriken,
- Mindestverweilzeiten,
- Trendbewertung.

Dadurch werden oszillierende Reclamation- und Degradation-Policies vermieden.

---

## Ereignismodell

Subsysteme sollen Pressure-Änderungen ereignisbasiert beobachten können.

Beispiel:

```text
Normal → Elevated
Elevated → Constrained
Constrained → Critical
Critical → Constrained
```

Nicht jede einzelne Änderung einer freien Page darf einen systemweiten Event erzeugen.

Events repräsentieren semantisch relevante Zustandsänderungen.

---

## Deterministic Mode

Deterministic Mode muss Memory-Pressure-Policies reproduzierbar konfigurieren können.

Adaptive Schwellenwerte und heuristische Reclamation können dabei eingeschränkt oder durch fest definierte Policies ersetzt werden.

Unvermeidbarer physischer Speichermangel kann jedoch auch im Deterministic Mode nicht verborgen werden.

---

## Predictive Pressure

NovaOS darf zukünftigen Memory Pressure prognostizieren.

Beispiel:

```text
Current Usage
     +
Allocation Rate
     +
Workload History
     ↓
Predicted Pressure
```

Prediction darf präventive Soft-Policy-Maßnahmen auslösen.

Sie darf jedoch keine harte Ressourcenverletzung allein aufgrund einer Vorhersage rechtfertigen.

Prediction Error wird zur Anpassung des Modells verwendet.

---

## Architecture Introspection

Memory Pressure muss introspektierbar sein.

Mindestens relevant sind:

```text
Pressure State
Memory Domain
Available Capacity
Reserved Capacity
Reclaimable Capacity
Pinned Capacity
Fragmentation
Reclamation Activity
Allocation Failure Rate
Pressure Trend
Active Pressure Policies
```

Dadurch kann nachvollzogen werden, warum NovaOS bestimmte Reclamation- oder Degradation-Maßnahmen ausführt.

---

## Normative Anforderungen

1. NovaOS MUSS Memory Pressure als systemweites Ressourcensignal modellieren.
2. Memory Pressure DARF NICHT ausschließlich aus einem globalen Free-Memory-Prozentwert bestehen.
3. Pressure MUSS global und pro relevanter Memory Domain darstellbar sein.
4. Memory Pressure MUSS mit der Resource Economy integriert sein.
5. Budgets und Reservations MÜSSEN bei der Pressure-Bewertung berücksichtigt werden.
6. Reservierter Speicher DARF NICHT automatisch als allgemein verfügbar betrachtet werden.
7. NUMA-lokaler Memory Pressure MUSS separat erfassbar sein.
8. Required NUMA Constraints DÜRFEN aufgrund von Pressure NICHT still verletzt werden.
9. Pressure MUSS Reclamation vor vollständiger Speichererschöpfung auslösen können.
10. Cooperative Memory Release MUSS für geeignete Systemkomponenten möglich sein.
11. Graceful Degradation MUSS Memory Pressure als Auslöser verwenden können.
12. Optionale Ressourcenverwendung SOLL vor kritischen Ressourcen reduziert werden.
13. Der Scheduler DARF Memory Pressure als Soft-Policy-Signal verwenden.
14. Memory Pressure DARF keine Hard Scheduling Constraints überschreiben.
15. Optionaler Parallelismus MUSS unter Pressure reduzierbar sein, wenn der Execution Contract dies erlaubt.
16. Speculative und predictive Allokationen SOLLEN unter Pressure bevorzugt reduzierbar sein.
17. Accelerator-Memory-Pressure MUSS in das gemeinsame Ressourcenmodell integrierbar sein.
18. Pinned und Zero-Copy Memory MUSS von unmittelbar reclaimable Memory unterscheidbar sein.
19. Realtime Reservations MÜSSEN vor allgemeiner Pressure-Reclamation geschützt werden.
20. Critical Pressure MUSS definierte Eskalationsmaßnahmen auslösen können.
21. Beliebige Prozessbeendigung DARF NICHT die grundlegende Memory-Pressure-Policy darstellen.
22. Allocation Failure und Memory Pressure MÜSSEN als unterschiedliche Konzepte behandelt werden.
23. Pressure-Zustände MÜSSEN Stabilitätsmechanismen wie Hysterese unterstützen.
24. Semantische Pressure-Änderungen MÜSSEN ereignisbasiert publizierbar sein.
25. Deterministic Mode MUSS adaptive Pressure-Policies einschränken können.
26. Predictive Pressure DARF ausschließlich Soft-Policy-Maßnahmen beeinflussen.
27. Pressure-Zustand, Ursachen und aktive Maßnahmen MÜSSEN introspektierbar sein.
28. Nicht erfüllbare Hard Memory Constraints DÜRFEN NICHT durch eine günstige globale Pressure-Bewertung als erfüllbar behandelt werden.

---

## Konsequenzen

### Positive Konsequenzen

- Speicherknappheit wird frühzeitig statt erst bei Allocation Failure erkannt,
- NUMA-lokale Engpässe werden sichtbar,
- Anwendungen und Systemdienste können kooperativ Speicher freigeben,
- Resource Economy und Memory Manager verwenden ein gemeinsames Belastungsmodell,
- optionale Workloads können vor kritischen Komponenten degradiert werden,
- Memory Pressure kann CPU-, Accelerator- und adaptive Policies koordinieren.

### Negative Konsequenzen

- Pressure-Bewertung benötigt mehrere Metriken und Policies,
- zusätzliche Telemetrie und Ereignisse verursachen Verwaltungsaufwand,
- falsche Schwellenwerte können zu unnötiger Reclamation führen,
- systemweite Reaktionen benötigen Koordination zwischen mehreren Subsystemen.

---

## Verworfene Alternativen

### Memory Pressure nur als Free-Memory-Prozentwert

Verworfen.

Dies berücksichtigt weder Reservations noch NUMA, Fragmentierung, Reclamation-Kosten oder spezielle Allocation Constraints.

### Reaktion erst bei Allocation Failure

Verworfen.

Zu diesem Zeitpunkt kann kontrollierte Graceful Degradation bereits zu spät sein.

### Memory Pressure ausschließlich intern im PMM

Verworfen.

Andere Subsysteme können häufig semantisch bessere Maßnahmen durchführen als der PMM allein.

### Sofortige Prozessbeendigung bei Critical Pressure

Verworfen.

NovaOS bevorzugt kontrollierte Reclamation, Degradation, Budgets und Supervision vor willkürlicher Terminierung.

### Globales Pressure-Signal ohne NUMA-Bezug

Verworfen.

Lokale Memory Domains können unabhängig voneinander unter sehr unterschiedlichem Ressourcenpressure stehen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-MEMORY-0001_Hierarchische_Physical_Memory_Architecture`
- `ADR-MEMORY-0002_NUMA_Aware_Physical_Memory_Allocation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen`
- `ADR-PROCESS-0005_Systemweite_Process_Supervision`
- `ADR-SCHED-0004_NUMA_Aware_Scheduling`
- `ADR-SCHED-0009_GPU_NPU_und_Accelerator_Scheduling`
- `ADR-SCHED-0011_Predictive_und_Adaptive_Scheduling`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-MEMORY-PRESSURE-0001`
- `NPSPEC-MEMORY-PRESSURE-STATE-0001`
- `NPSPEC-MEMORY-PRESSURE-METRICS-0001`
- `NPSPEC-MEMORY-PRESSURE-NUMA-0001`
- `NPSPEC-MEMORY-RECLAIM-0001`
- `NPSPEC-MEMORY-PRESSURE-EVENT-0001`
- `NPSPEC-MEMORY-PRESSURE-INTROSPECTION-0001`
- `NPSPEC-MEMORY-PRESSURE-TEST-0001`

---

## Ergebnis

NovaOS behandelt Memory Pressure als systemweites, hierarchisches Ressourcensignal:

```text
Memory State
     ↓
Pressure Evaluation
     ↓
Normal / Elevated / Constrained / Critical
     ↓
Resource Economy
     ↓
Reclamation
Degradation
Scheduling
Cooperative Release
Admission Control
```

Damit wird Speichermangel nicht erst dann sichtbar, wenn eine Allokation bereits fehlschlägt.

Die zentrale Architekturregel lautet:

```text
Pressure is a system signal,
not just a free-memory counter.

Detect pressure early.

Degrade optional work first.

Protect hard resource guarantees.
```