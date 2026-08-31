# ADR-SCHED-0007 – Thermal-Aware Scheduling

## Status

Angenommen

## Kategorie

Kernel / Scheduler / Thermal Management / Hardwaretopologie / Ressourcen / Energie

## Kontext

`ADR-SCHED-0006_Energy_Aware_Scheduling` integriert Energieeffizienz als Soft Objective in den NovaOS-Scheduler. Temperatur ist damit verwandt, besitzt aber eine andere Semantik.

Thermische Zustände können unmittelbar beeinflussen:

- zulässige CPU-Leistung,
- Taktraten,
- verfügbare CPU- und Accelerator-Kapazität,
- Lebensdauer und Stabilität von Hardware,
- Realtime- und Deadline-Garantien.

Moderne Systeme besitzen zudem mehrere thermische Zonen. Ein einzelner CPU-Core, ein Package, eine GPU, ein NPU oder eine andere Komponente kann thermisch begrenzt sein, während andere Ressourcen weiterhin Kapazität besitzen.

Der Scheduler muss solche Zustände berücksichtigen können, darf aber nicht selbst zum Hardware-Thermal-Controller werden.

---

## Entscheidung

NovaOS verwendet **Thermal-Aware Scheduling** als integrierte Eigenschaft des Mehrklassen-Schedulers.

```text
Runnable Work
      ↓
Hard Execution Constraints
      ↓
Eligible Resources
      ↓
Topology + Load + Locality
      +
Thermal State
      ↓
Scheduling Policy
      ↓
Placement / Migration / Throttling
```

Der Scheduler erhält normalisierte thermische Informationen aus der Plattform- und Thermal-Infrastruktur und verwendet sie für Placement-, Migration-, Parallelitäts- und Degradationsentscheidungen.

**Thermal Safety bleibt eine harte Systemgrenze und besitzt Vorrang vor Performance- und Realtime-Zielen.**

---

## Grundprinzip

```text
Avoid thermal hotspots when possible.

Distribute heat when beneficial.

Protect hardware before performance.

Never pretend a thermally impossible
contract can still be guaranteed.
```

---

## Trennung von Mechanismus und Policy

Der Scheduler steuert nicht direkt Lüfter, Sensoren oder hardwareinterne Schutzmechanismen.

Die Architektur bleibt getrennt:

```text
Sensors / Firmware / Hardware
            ↓
      Thermal Subsystem
            ↓
   Normalized Thermal State
            ↓
        Scheduler
            ↓
     Placement Policy
```

Hardware-Schutzmechanismen bleiben unabhängig vom Scheduler wirksam.

---

## Thermal Domains

Thermische Zustände werden geeigneten Hardware- beziehungsweise Thermal Domains zugeordnet.

Beispiele:

```text
CPU Core
CPU Cluster
CPU Package
GPU
NPU
SoC
Memory Domain
System Thermal Zone
```

Diese Domains sollen mit dem Hardware System Graph verknüpft sein.

---

## Thermal State

Der Scheduler benötigt keine gerätespezifischen Rohsensorwerte als primäre Semantik.

Die Thermal-Infrastruktur soll normalisierte Zustände bereitstellen können, beispielsweise:

```text
Normal
Elevated
Constrained
Critical
```

Die konkreten Schwellenwerte bleiben Plattformpolicy.

---

## Thermal Headroom

Neben dem aktuellen Zustand kann verfügbarer Thermal Headroom als Scheduling-Signal dienen.

Ein thermisch kühlerer CPU-Bereich kann für zusätzliche Arbeit geeigneter sein als ein bereits stark belasteter Bereich.

Thermal Headroom bleibt ein Soft Placement-Faktor, solange keine Safety-Grenze erreicht ist.

---

## Thermal Load Distribution

Bei steigender thermischer Belastung darf der Scheduler Arbeit auf geeignete kühlere Ressourcen verteilen.

```text
Hot CPU Domain
      ↓
Eligible Cooler Domain
      ↓
Migration
```

Eine Migration erfolgt nur, wenn andere Hard Constraints dies zulassen.

---

## Thermal Consolidation

Verteilung ist nicht grundsätzlich optimal.

Bei niedriger Last kann Workload Consolidation weiterhin sinnvoll sein, damit andere Hardwarebereiche vollständig in energiesparende Zustände wechseln können.

Energy-Aware und Thermal-Aware Scheduling müssen daher gemeinsam zwischen:

```text
Consolidation
     ↔
Heat Distribution
```

abwägen.

---

## Thermal Throttling

Hardware oder Power Management kann aufgrund thermischer Grenzen verfügbare Rechenleistung reduzieren.

Der Scheduler muss solche Änderungen als veränderte effektive Ressourcenkapazität behandeln können.

Ein gedrosselter Core darf nicht weiterhin so bewertet werden, als besitze er seine nominelle Maximalleistung.

---

## Realtime

Realtime Contracts können thermisch bedingt unerfüllbar werden.

NovaOS darf in diesem Fall nicht versuchen, Hardware-Safety-Grenzen zugunsten der Deadline einzuhalten.

Es gilt:

```text
Thermal Safety
      >
Realtime Guarantee
```

Wird ein zugelassener Hard-Realtime-Contract thermisch unerfüllbar, muss dies als explizite Contract-Verletzung beziehungsweise Ressourcenstörung sichtbar werden.

---

## Admission Control

Realtime Admission Control soll den aktuellen und, soweit belastbar bekannt, erwartbaren thermischen Ressourcenrahmen berücksichtigen können.

NovaOS darf keine Hard-Realtime-Garantie auf Kapazität stützen, die aufgrund bestehender thermischer Begrenzung nicht verfügbar ist.

---

## Deadlines

Soft- und Firm-Deadline-Workloads dürfen abhängig vom Thermal State:

- auf andere CPUs verschoben,
- mit geringerer Parallelität ausgeführt,
- degradiert,
- auf einen anderen geeigneten Provider verlagert

werden.

Hard Constraints bleiben bindend.

---

## Heterogene CPUs

Performance- und Efficiency-Cores können unterschiedliche thermische Eigenschaften besitzen.

Der Scheduler darf deshalb Workloads abhängig von:

- Leistungsbedarf,
- Energieeffizienz,
- Thermal Headroom,
- Execution Contract

zwischen CPU-Klassen verteilen.

---

## NUMA und Cache Locality

Thermal Placement muss gemeinsam mit NUMA- und Cache-Awareness bewertet werden.

Eine Migration auf eine kühlere CPU kann zusätzliche Kosten erzeugen durch:

- Remote Memory Access,
- Cache-Verlust,
- schlechtere Device Locality.

Thermal-Aware Scheduling muss diese Kosten berücksichtigen, solange keine kritische Thermal-Safety-Grenze die Migration erzwingt.

---

## Work Stealing

Work Stealing darf Thermal State berücksichtigen.

Ein thermisch belasteter Worker beziehungsweise eine belastete Scheduling Domain soll zusätzliche Arbeit vermeiden können, wenn geeignete Alternativen existieren.

Victim- und Target-Auswahl bleiben jedoch den allgemeinen Hard Constraints untergeordnet.

---

## Parallelität

Hohe Parallelität kann thermische Belastung erhöhen.

NovaOS darf deshalb für Soft Workloads den Parallelitätsgrad reduzieren.

```text
Thermal Pressure
      ↓
Reduce Optional Parallelism
      ↓
Lower Power Density
```

Required Parallelism beziehungsweise Hard Contracts dürfen nur entsprechend ihrer definierten Degradations- oder Failure-Policy verändert werden.

---

## Accelerator Scheduling

Thermal-Aware Scheduling darf nicht auf CPUs beschränkt sein.

GPU-, NPU- und andere Accelerator Provider können ebenfalls thermisch eingeschränkt sein.

Die systemweite Algorithmus- und Provider-Auswahl darf einen anderen geeigneten Provider wählen, wenn:

- Semantik,
- Trust,
- Data Sovereignty,
- Determinismus,
- Ressourcen- und Deadline-Constraints

dies erlauben.

---

## Resource Economy

Thermisch bedingte Leistungsgrenzen werden als Änderung verfügbarer Ressourcen in die Resource Economy eingebunden.

Damit können Scheduler, Realtime Admission Control und andere Resource Policies auf dieselbe effektive Kapazität reagieren.

---

## Graceful Degradation

Bei Thermal Pressure soll NovaOS zunächst optionale Qualität oder Arbeit reduzieren.

Beispiel:

```text
Thermal Pressure
      ↓
Throttle Background Work
      ↓
Reduce Optional Parallelism
      ↓
Use Efficient Provider / Algorithm
      ↓
Degrade Optional Quality
      ↓
Protect Required Work
```

Safety-, Security-, Sovereignty- und Correctness-Anforderungen dürfen dabei nicht abgeschwächt werden.

---

## Hysterese

Thermal States können um Schwellenwerte schwanken.

Scheduling Policies müssen deshalb Hysterese beziehungsweise geeignete Stabilisierung verwenden können.

Dies verhindert wiederholtes Hin- und Hermigrieren von Tasks aufgrund kleiner Temperaturschwankungen.

Die konkreten Schwellenwerte gehören in die entsprechende NPSPEC beziehungsweise Platform Policy.

---

## Deterministic Mode

Thermal Hardwarezustände sind externe Einflüsse und können deterministische Scheduling-Pläne beeinträchtigen.

Deterministic Mode kann adaptive Thermal-Optimierungen einschränken.

Safety-Reaktionen dürfen jedoch niemals deaktiviert werden.

Thermal Safety besitzt auch im Deterministic Mode Vorrang.

---

## Introspection

Thermal-Aware Scheduling muss introspektierbar sein.

Relevante Informationen umfassen mindestens:

```text
Thermal Domain
Thermal State
Available Capacity
Affected CPUs / Devices
Scheduling Restriction
Migration / Throttling Reason
```

Damit muss erkennbar sein, wenn ein Workload aufgrund thermischer Bedingungen anders geplant oder degradiert wurde.

---

## Adaptive Optimierung

NovaOS darf thermische Entwicklung für Soft Scheduling Decisions prognostizieren.

Beispielsweise:

```text
Predicted Thermal Load
        ↓
Placement Decision
        ↓
Observed Thermal State
        ↓
Prediction Error
        ↓
Model Correction
```

Solche Modelle dürfen frühzeitig Hotspots vermeiden helfen.

Safety und Hard Constraints dürfen niemals von dieser Vorhersage abhängen.

---

## Fail-Safe Defaults

Fehlen Thermalinformationen, darf der Scheduler keine zusätzliche thermische Kapazität annehmen.

Meldet die Plattform einen kritischen Thermal State, besitzt die Schutzreaktion Vorrang vor Performanceoptimierung.

KI oder adaptive Modelle dürfen keine hardwareseitige Thermal-Safety-Grenze überschreiben.

---

## Normative Anforderungen

1. NovaOS MUSS Thermal-Aware Scheduling unterstützen können.
2. Thermal Safety MUSS Vorrang vor Performance-, Energie- und Realtime-Optimierung besitzen.
3. Scheduler und Thermal Subsystem MÜSSEN getrennte Verantwortlichkeiten behalten.
4. Hardwareseitige Thermal-Schutzmechanismen DÜRFEN NICHT vom Scheduler abhängig sein.
5. Thermal Domains MÜSSEN mit dem Hardware System Graph verknüpfbar sein.
6. Der Scheduler MUSS normalisierte Thermal States berücksichtigen können.
7. Thermal Headroom DARF als Soft Placement-Faktor verwendet werden.
8. Thermisch belastete Ressourcen SOLLEN bei geeigneten Alternativen entlastet werden können.
9. Thermal Load Distribution MUSS CPU-Affinity und andere Hard Constraints respektieren.
10. Energy-Aware und Thermal-Aware Scheduling MÜSSEN gemeinsam optimierbar sein.
11. Thermisch reduzierte Rechenleistung MUSS als reduzierte effektive Ressourcenkapazität darstellbar sein.
12. Realtime Admission Control MUSS thermisch nicht verfügbare Kapazität berücksichtigen können.
13. Thermal Safety DARF NICHT zur Einhaltung eines Realtime Contracts verletzt werden.
14. Thermisch verursachte Hard-Realtime-Verletzungen MÜSSEN explizit sichtbar sein.
15. Soft- und Firm-Deadline-Workloads MÜSSEN thermisch bedingt replazierbar oder degradierbar sein.
16. Heterogene CPU-Klassen MÜSSEN unterschiedliche thermische Eigenschaften besitzen können.
17. Thermal Placement MUSS mit NUMA- und Cache-Locality abwägbar sein.
18. Work Stealing MUSS Thermal State als Soft Policy berücksichtigen können.
19. Optionaler Parallelismus MUSS bei Thermal Pressure reduzierbar sein.
20. Thermal-Aware Scheduling MUSS auf Accelerator-Ressourcen erweiterbar sein.
21. Thermisch bedingte Kapazitätsänderungen MÜSSEN in die Resource Economy integrierbar sein.
22. Graceful Degradation MUSS bei Thermal Pressure unterstützt werden.
23. Safety-, Security-, Sovereignty- und Correctness-Constraints DÜRFEN dabei NICHT degradiert werden.
24. Thermal Scheduling Policies MÜSSEN Hysterese beziehungsweise vergleichbare Stabilisierung unterstützen können.
25. Häufige thermisch verursachte Task-Migrationen SOLLEN vermieden werden.
26. Deterministic Mode DARF Thermal-Safety-Reaktionen NICHT deaktivieren.
27. Thermal-bezogene Scheduling-Entscheidungen MÜSSEN introspektierbar sein.
28. Fehlende Thermalinformationen MÜSSEN konservativ behandelt werden.
29. Adaptive Thermal-Prognosen DÜRFEN ausschließlich Soft Policies beeinflussen.
30. Thermal Safety und Scheduler-Korrektheit MÜSSEN vollständig ohne KI gewährleistet sein.

---

## Konsequenzen

### Positive Konsequenzen

- geringere Bildung lokaler Hotspots,
- bessere Nutzung thermisch verfügbarer Rechenkapazität,
- kontrollierte Zusammenarbeit von Scheduler, Power und Thermal Management,
- bessere Unterstützung heterogener CPUs und Accelerators,
- thermisch bedingte Kapazitätsverluste werden systemweit sichtbar,
- Realtime-Garantien werden nicht fälschlich trotz Thermal Throttling aufrechterhalten.

### Negative Konsequenzen

- zusätzliche Scheduling-Signale und Kostenfaktoren,
- Thermal-, Energy-, NUMA- und Cache-Ziele können miteinander konkurrieren,
- Thermalzustände können sich schneller ändern als langfristige Scheduling Policies,
- Plattformen liefern unterschiedlich detaillierte Thermalinformationen.

---

## Verworfene Alternativen

### Thermal Management vollständig außerhalb des Schedulers

Verworfen.

Hardware kann sich zwar selbst schützen, der Scheduler würde thermische Hotspots aber erst nach erfolgtem Throttling berücksichtigen.

### Scheduler steuert Thermal Hardware direkt

Verworfen.

Sensorik, Lüftersteuerung und Hardwaregrenzen gehören in die Thermal- und Platform-Infrastruktur.

### Immer auf den kühlsten Core migrieren

Verworfen.

NUMA-, Cache-, Affinity-, Migrations- und Performancekosten können den Vorteil übersteigen.

### Realtime besitzt Vorrang vor Thermal Safety

Verworfen.

Hardware-Safety-Grenzen dürfen nicht zur Einhaltung einer zeitlichen Garantie überschritten werden.

### KI-basierte Thermal Control als Voraussetzung

Verworfen.

Vorhersagemodelle dürfen optimieren, aber keine Safety-Funktion ersetzen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-SCHED-0001_Präemptiver_Mehrklassen_Scheduler`
- `ADR-SCHED-0002_Fair_Deadline_und_Realtime_Scheduling_in_einer_Architektur`
- `ADR-SCHED-0003_Topology_Aware_Scheduling`
- `ADR-SCHED-0004_NUMA_Aware_Scheduling`
- `ADR-SCHED-0005_Cache_Aware_Scheduling`
- `ADR-SCHED-0006_Energy_Aware_Scheduling`
- `ADR-CONCURRENCY-0006_Work_Stealing_für_allgemeine_Task_Workloads`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`

---

## Zugehörige NPSPECs

- `NPSPEC-SCHED-THERMAL-0001`
- `NPSPEC-SCHED-THERMAL-DOMAIN-0001`
- `NPSPEC-SCHED-THERMAL-STATE-0001`
- `NPSPEC-SCHED-THERMAL-PLACEMENT-0001`
- `NPSPEC-SCHED-THERMAL-CAPACITY-0001`
- `NPSPEC-SCHED-THERMAL-REALTIME-0001`
- `NPSPEC-SCHED-THERMAL-INTROSPECTION-0001`
- `NPSPEC-SCHED-THERMAL-TEST-0001`

---

## Ergebnis

NovaOS integriert den thermischen Zustand der Hardware in seine Scheduling-Entscheidungen:

```text
Execution Constraints
        ↓
Eligible Resources
        ↓
Load + Locality + Energy + Thermal State
        ↓
Scheduling Decision
```

Thermal-Aware Scheduling optimiert die Platzierung von Workloads, während die eigentliche Hardware-Safety unabhängig davon gewährleistet bleibt.

Die zentrale Architekturregel lautet:

```text
Avoid heat when possible.

React before throttling when useful.

Protect hardware before performance.

Never override thermal safety.
```