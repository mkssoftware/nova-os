# ADR-SCHED-0008 – Heterogeneous CPU Scheduling

## Status

Angenommen

## Kategorie

Kernel / Scheduler / Heterogeneous Computing / CPU Placement / Hardwaretopologie / Ressourcen

## Kontext

Moderne Prozessoren bestehen zunehmend aus CPU-Kernen mit unterschiedlichen Eigenschaften.

Beispiele sind:

- Performance-Cores,
- Efficiency-Cores,
- Kerne mit unterschiedlichen Frequenzbereichen,
- unterschiedliche Cache-Konfigurationen,
- unterschiedliche ISA-Erweiterungen,
- unterschiedliche Energie- und Thermal-Eigenschaften.

Damit ist die klassische Annahme:

```text
All CPUs are equivalent.
```

für NovaOS nicht ausreichend.

Ein Scheduler muss nicht nur entscheiden, **wann** ein Task ausgeführt wird, sondern auch, **welche CPU-Klasse** für diesen Task geeignet ist.

Diese Entscheidung muss mit Topology-, NUMA-, Cache-, Energy- und Thermal-Aware Scheduling zusammenarbeiten.

---

## Entscheidung

NovaOS behandelt CPU-Heterogenität als First-Class-Eigenschaft des Schedulers.

CPU Placement erfolgt auf Basis von:

```text
Execution Requirements
        ↓
Eligible CPU Classes
        ↓
Topology + Locality
        ↓
Performance + Energy + Thermal State
        ↓
Scheduling Policy
        ↓
Target CPU
```

Der Scheduler betrachtet logische CPUs nicht als gleichwertig, sondern verwendet die durch HAL und Hardware System Graph beschriebenen Fähigkeiten und Eigenschaften.

---

## Grundprinzip

```text
Schedule work on CPUs
that satisfy its requirements.

Use performance where needed.

Use efficiency where sufficient.

Never assume all cores are equal.
```

---

## CPU-Klassen

NovaOS unterstützt explizite CPU-Klassen.

Typische Klassen können sein:

```text
Performance
Efficiency
Balanced
Specialized
```

Diese Namen sind keine fest verdrahteten Hardwaretypen.

Die tatsächlichen Eigenschaften werden aus dem Hardware System Graph abgeleitet.

Ein zukünftiges System darf beliebig viele unterschiedliche CPU-Klassen besitzen.

---

## CPU Capabilities

Jede CPU beziehungsweise CPU-Klasse kann Eigenschaften bereitstellen wie:

```text
ISA
ISA Extensions
Relative Performance
Energy Efficiency
Frequency Range
Cache Topology
NUMA Association
SMT Relationship
Thermal Characteristics
```

Der Scheduler verwendet diese Eigenschaften zur Bestimmung zulässiger und bevorzugter CPUs.

---

## Required und Preferred CPU Properties

Execution Contracts müssen zwischen zwingenden und bevorzugten CPU-Eigenschaften unterscheiden.

Beispiel:

```text
Required:
    ISA Extension X

Preferred:
    High Performance
    Energy Efficient
```

Required Properties bestimmen die zulässige CPU-Menge.

Preferred Properties beeinflussen lediglich die Optimierung innerhalb dieser Menge.

---

## Keine festen P-/E-Core-Annahmen

NovaOS darf die Architektur nicht auf ein bestimmtes Hybrid-CPU-Modell beschränken.

Insbesondere darf der Scheduler nicht voraussetzen:

```text
exactly two CPU classes
```

oder:

```text
P-Core > E-Core
```

Die Eigenschaften werden semantisch beschrieben und nicht aus Produktbezeichnungen abgeleitet.

---

## Workload Classification

Scheduling Policies dürfen Workloads nach ihren Anforderungen klassifizieren.

Beispielsweise:

```text
Latency Sensitive
Throughput Oriented
Background
Energy Sensitive
Realtime
```

Diese Klassifikation ist ein Scheduling-Signal und keine Security Authority.

---

## Interactive Workloads

Interaktive Arbeit kann bevorzugt auf CPUs mit hoher kurzfristiger Performance ausgeführt werden, wenn dies die Antwortlatenz verbessert.

Dies bedeutet nicht, dass jede UI-Task dauerhaft einen Performance-Core reserviert.

Nach Abschluss der latenzkritischen Phase kann normale Scheduling Policy wieder greifen.

---

## Background Workloads

Background Work kann bevorzugt auf energieeffizienten CPU-Klassen ausgeführt werden.

Beispiele:

- Indexierung,
- Wartung,
- Prefetching,
- optionale Analyse.

Deadline-, Ressourcen- und Systemzustand können diese Präferenz überschreiben.

---

## Realtime

Realtime Workloads dürfen erforderliche CPU-Eigenschaften explizit definieren.

Beispielsweise:

```text
Required:
    Minimum Performance Class
    No SMT Sharing
    Stable Frequency Characteristics
```

Eine CPU-Klasse darf nur für einen Hard-Realtime-Contract verwendet werden, wenn deren Eigenschaften mit der Garantie vereinbar sind.

---

## Performance Capacity

CPU-Auslastung darf auf heterogenen Systemen nicht ausschließlich anhand runnable Task Count bewertet werden.

Eine CPU mit höherer Rechenkapazität kann mehr Arbeit innerhalb derselben Zeit erledigen.

Der Scheduler muss deshalb unterschiedliche relative CPU-Kapazitäten darstellen können.

---

## Fairness

Fair Scheduling muss CPU-Heterogenität berücksichtigen.

CPU-Zeit auf unterschiedlichen CPU-Klassen ist nicht zwangsläufig gleichwertig.

Die Fairness-Policy darf daher effektive Rechenkapazität beziehungsweise tatsächliche Ressourcennutzung berücksichtigen.

Die genaue Accounting-Methode wird in NPSPECs definiert.

---

## Task Migration

Tasks dürfen zwischen kompatiblen CPU-Klassen migrieren.

Migration muss jedoch Kosten berücksichtigen, insbesondere:

- Cache-Verlust,
- NUMA-Wechsel,
- Performanceänderung,
- Energy Cost,
- Thermal State.

Ein CPU-Klassenwechsel ist deshalb keine kostenlose Scheduling-Operation.

---

## ISA-Kompatibilität

Nicht jede CPU muss exakt dieselben ISA-Erweiterungen besitzen.

Hat ein Task Code ausgeführt oder generiert, der bestimmte CPU-Features voraussetzt, darf er nicht anschließend auf eine inkompatible CPU migriert werden.

Der Scheduler muss solche Feature Requirements berücksichtigen.

---

## Runtime und JIT

Runtimes und JIT-Compiler können CPU-spezifischen Code erzeugen.

Sie müssen deshalb mit dem Scheduler beziehungsweise der CPU-Capability-Infrastruktur koordinieren können.

Mögliche Strategien sind:

- gemeinsamer kompatibler Code,
- mehrere Codevarianten,
- CPU-spezifische Execution Constraints.

Die konkrete Runtime-Strategie wird nicht durch diese ADR festgelegt.

---

## Topology Awareness

CPU-Klasse ist nur ein Placement-Faktor.

Der Scheduler berücksichtigt gleichzeitig:

```text
CPU Class
NUMA Locality
Cache Locality
Affinity
Current Load
Energy
Thermal State
```

Ein Performance-Core auf einem entfernten NUMA Node ist nicht automatisch besser als ein lokaler effizienterer Core.

---

## Energy Awareness

Energy-Aware Scheduling verwendet CPU-Klassen zur Auswahl einer ausreichend leistungsfähigen, möglichst effizienten CPU.

```text
Required Performance
        ↓
Compatible CPUs
        ↓
Energy-Efficient Choice
```

Maximale Performance wird nicht standardmäßig verwendet, wenn sie keinen relevanten Nutzen bringt.

---

## Thermal Awareness

Thermal State kann die effektive Leistungsfähigkeit einer CPU-Klasse verändern.

Ein thermisch gedrosselter Performance-Core kann zeitweise weniger geeignet sein als ein unbelasteter anderer Core.

Der Scheduler muss deshalb aktuelle effektive Kapazität statt ausschließlich nomineller CPU-Klasse berücksichtigen können.

---

## Work Stealing

Work Stealing muss CPU-Heterogenität berücksichtigen.

Ein Task darf nur auf einen Worker gestohlen werden, dessen CPU die Required Execution Constraints erfüllt.

Preferred CPU Properties dürfen bei Victim- und Target-Auswahl berücksichtigt werden.

---

## Load Balancing

Load Balancing darf nicht allein auf gleiche Queue-Längen abzielen.

Beispielsweise:

```text
Performance CPU:
    4 runnable tasks

Efficiency CPU:
    2 runnable tasks
```

bedeutet nicht automatisch, dass Arbeit vom Performance-Core auf den Efficiency-Core verschoben werden sollte.

Die effektive CPU-Kapazität muss berücksichtigt werden.

---

## Parallelität

Bei parallelen Workloads kann die Verteilung über unterschiedliche CPU-Klassen zu unterschiedlich schnellen Tasks führen.

Task Groups und Parallel Runtime dürfen deshalb die verfügbare heterogene Kapazität bei:

- Partitionierung,
- Parallelitätsgrad,
- Work Distribution

berücksichtigen.

---

## Resource Economy

CPU-Kapazität wird über die Resource Economy abgebildet.

Die Architektur darf nicht voraussetzen, dass:

```text
1 ms CPU time
```

auf jeder CPU exakt denselben wirtschaftlichen beziehungsweise leistungsbezogenen Wert besitzt.

Accounting und Budgets müssen dennoch stabile systemweite Semantik besitzen.

---

## Graceful Degradation

Bei Ressourcen-, Energie- oder Thermal Pressure kann NovaOS Workloads auf schwächere CPU-Klassen verlagern, sofern deren Contract dies erlaubt.

```text
Preferred Performance CPU
        ↓
Unavailable / Constrained
        ↓
Compatible Efficiency CPU
        ↓
Continue with reduced performance
```

Hard Performance- oder Realtime-Requirements dürfen nicht still degradiert werden.

---

## Deterministic Mode

Deterministic Mode kann CPU-Klassen und konkrete CPU Sets festlegen.

Damit können Laufzeitunterschiede durch dynamische Migration zwischen heterogenen CPUs reduziert werden.

Adaptive CPU-Class-Selection muss für entsprechend deterministische Workloads deaktivierbar sein.

---

## Hotplug

CPU Hotplug kann die verfügbaren CPU-Klassen verändern.

Der Scheduler muss darauf reagieren können.

Wird die letzte CPU entfernt, die ein Required CPU Feature bereitstellt, müssen betroffene Workloads entsprechend ihrem Contract:

- gestoppt,
- suspendiert,
- migriert,
- degradiert,
- oder als nicht ausführbar markiert

werden.

---

## Virtualisierung

Virtuelle CPUs können ebenfalls unterschiedliche oder eingeschränkte Capabilities besitzen.

NovaOS darf nicht davon ausgehen, dass die vom Hypervisor präsentierte CPU-Topologie physischer Hardware exakt entspricht.

Der Scheduler arbeitet mit den tatsächlich bereitgestellten und vertrauenswürdig ermittelten Eigenschaften.

---

## Introspection

Heterogeneous CPU Scheduling muss introspektierbar sein.

Mindestens relevant sind:

```text
CPU
CPU Class
Capabilities
Effective Capacity
Energy Characteristics
Thermal State
Assigned Workload
Placement Reason
```

Für Tasks soll erkennbar sein, warum eine bestimmte CPU-Klasse gewählt wurde.

---

## Adaptive Optimierung

NovaOS darf lernen, welche CPU-Klasse für einen bestimmten Workload tatsächlich geeignet ist.

```text
Predicted CPU suitability
        ↓
Placement
        ↓
Observed latency / energy / throughput
        ↓
Prediction Error
        ↓
Policy adjustment
```

Dies darf ausschließlich Preferred Properties und andere Soft Policies beeinflussen.

Required Execution Constraints bleiben unverändert.

---

## Fail-Safe Defaults

Ist unbekannt, ob eine CPU ein Required Feature unterstützt, gilt sie für diesen Workload als nicht geeignet.

Unbekannte Performance- oder Energieeigenschaften dürfen konservativ bewertet werden.

Scheduler-Korrektheit darf nicht von einer korrekten Workload-Prognose abhängen.

---

## Normative Anforderungen

1. NovaOS MUSS heterogene CPU-Architekturen als First-Class-Scheduling-Eigenschaft unterstützen.
2. Der Scheduler DARF NICHT voraussetzen, dass alle logischen CPUs gleichwertig sind.
3. CPU-Eigenschaften MÜSSEN über HAL und Hardware System Graph beschreibbar sein.
4. NovaOS MUSS mehr als zwei CPU-Klassen unterstützen können.
5. CPU-Klassen DÜRFEN NICHT an herstellerspezifische P-/E-Core-Bezeichnungen gebunden sein.
6. Required und Preferred CPU Properties MÜSSEN getrennt behandelt werden.
7. Required CPU Properties MÜSSEN die Menge zulässiger CPUs begrenzen.
8. Preferred CPU Properties DÜRFEN nur innerhalb zulässiger CPUs optimieren.
9. Workload-Klassifikation DARF Scheduling Policy beeinflussen, aber keine zusätzliche Authority erzeugen.
10. Interaktive Workloads DÜRFEN leistungsfähige CPUs bevorzugen.
11. Background Workloads DÜRFEN energieeffiziente CPUs bevorzugen.
12. Realtime Workloads MÜSSEN erforderliche CPU-Eigenschaften explizit definieren können.
13. Unterschiedliche CPU-Kapazitäten MÜSSEN im Load Balancing darstellbar sein.
14. Fair Scheduling MUSS heterogene CPU-Kapazität berücksichtigen können.
15. Task Migration zwischen CPU-Klassen MUSS CPU-Kompatibilität prüfen.
16. CPU-spezifische ISA Requirements MÜSSEN bei Migration erhalten bleiben.
17. Runtimes und JIT-Systeme MÜSSEN CPU-Capability-Constraints ausdrücken können.
18. CPU-Class-Selection MUSS mit Topology-Aware Scheduling integriert sein.
19. NUMA- und Cache-Locality MÜSSEN bei CPU-Class-Selection berücksichtigt werden können.
20. Energy-Aware Scheduling MUSS unterschiedliche CPU-Effizienzklassen berücksichtigen können.
21. Thermal-Aware Scheduling MUSS die effektive Kapazität heterogener CPUs beeinflussen können.
22. Work Stealing DARF Tasks NICHT auf inkompatible CPU-Klassen verschieben.
23. Load Balancing DARF NICHT ausschließlich auf Queue-Längen basieren.
24. Parallele Workloads MÜSSEN heterogene CPU-Kapazität berücksichtigen können.
25. Heterogene CPU-Kapazität MUSS mit der Resource Economy integrierbar sein.
26. Graceful Degradation auf schwächere CPU-Klassen MUSS explizit contractabhängig sein.
27. Deterministic Mode MUSS CPU-Class-Selection einschränken oder fixieren können.
28. CPU Hotplug MUSS Capability-Verlust korrekt behandeln.
29. CPU-Class- und Placement-Entscheidungen MÜSSEN introspektierbar sein.
30. Adaptive CPU-Auswahl DARF Required Execution Constraints NICHT verändern und MUSS ohne KI korrekt funktionieren.

---

## Konsequenzen

### Positive Konsequenzen

- native Unterstützung moderner Hybrid-Prozessoren,
- bessere Balance zwischen Performance und Energieeffizienz,
- korrekte Behandlung unterschiedlicher CPU-Capabilities,
- bessere Scheduling-Entscheidungen auf zukünftigen heterogenen Systemen,
- Integration mit NUMA-, Cache-, Energy- und Thermal-Aware Scheduling.

### Negative Konsequenzen

- Load Balancing wird komplexer als auf symmetrischen SMP-Systemen,
- CPU-Zeit ist nicht mehr automatisch zwischen allen CPUs direkt vergleichbar,
- Migration muss CPU-Feature-Kompatibilität berücksichtigen,
- Runtime und JIT benötigen gegebenenfalls zusätzliche CPU-Capability-Informationen.

---

## Verworfene Alternativen

### Alle CPUs als symmetrisch behandeln

Verworfen.

Dies ignoriert Performance-, Energie-, ISA- und Thermal-Unterschiede.

### Festes P-Core/E-Core-Modell

Verworfen.

Die Architektur soll zukünftige Systeme mit beliebigen CPU-Klassen unterstützen.

### Anwendungen wählen konkrete CPU-Klassen direkt

Verworfen.

Anwendungen sollen Anforderungen und Präferenzen ausdrücken; die tatsächliche Placement-Entscheidung bleibt Systempolicy.

### Immer leistungsfähigste CPU verwenden

Verworfen.

Dies verschwendet Energie und kann Thermal Pressure erhöhen.

### Immer energieeffizienteste CPU verwenden

Verworfen.

Sie kann für Deadline-, Interactive- oder Realtime-Anforderungen ungeeignet sein.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-SCHED-0001_Präemptiver_Mehrklassen_Scheduler`
- `ADR-SCHED-0002_Fair_Deadline_und_Realtime_Scheduling_in_einer_Architektur`
- `ADR-SCHED-0003_Topology_Aware_Scheduling`
- `ADR-SCHED-0004_NUMA_Aware_Scheduling`
- `ADR-SCHED-0005_Cache_Aware_Scheduling`
- `ADR-SCHED-0006_Energy_Aware_Scheduling`
- `ADR-SCHED-0007_Thermal_Aware_Scheduling`
- `ADR-CONCURRENCY-0006_Work_Stealing_für_allgemeine_Task_Workloads`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-HAL-0002_Plattformabstraktion_für_x86_ARM_und_zukünftige_Architekturen`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`

---

## Zugehörige NPSPECs

- `NPSPEC-SCHED-HETEROGENEOUS-CPU-0001`
- `NPSPEC-SCHED-CPU-CLASS-0001`
- `NPSPEC-SCHED-CPU-CAPABILITY-0001`
- `NPSPEC-SCHED-CPU-CAPACITY-0001`
- `NPSPEC-SCHED-CPU-COMPATIBILITY-0001`
- `NPSPEC-SCHED-CPU-PLACEMENT-0001`
- `NPSPEC-SCHED-CPU-MIGRATION-0001`
- `NPSPEC-SCHED-HETEROGENEOUS-CPU-TEST-0001`

---

## Ergebnis

NovaOS behandelt CPU-Kerne nicht als austauschbare Recheneinheiten, sondern als Ressourcen mit unterschiedlichen Eigenschaften:

```text
Task Requirements
        ↓
Compatible CPUs
        ↓
Performance + Energy + Thermal
        +
NUMA + Cache + Load
        ↓
Best Suitable CPU
```

Damit kann dieselbe Scheduler-Architektur klassische SMP-Systeme ebenso wie moderne und zukünftige heterogene Prozessoren behandeln.

Die zentrale Architekturregel lautet:

```text
Do not schedule by core name.

Schedule by capability.

Meet requirements first.

Optimize the remaining choices.
```