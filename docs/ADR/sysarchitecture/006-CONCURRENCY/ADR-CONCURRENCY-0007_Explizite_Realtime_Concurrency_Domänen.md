# ADR-CONCURRENCY-0007 – Explizite Realtime Concurrency Domänen

## Status

Angenommen

## Kategorie

Kernel / Concurrency / Realtime / Scheduling / Execution Domains / Ressourcen

## Kontext

NovaOS verwendet Structured Concurrency, Task Groups, Deadline Propagation und Work Stealing als allgemeines Concurrency-Modell.

Allgemeine Workloads werden dabei auf Durchsatz, Fairness, Energieeffizienz und Hardwareauslastung optimiert. Realtime-Workloads besitzen dagegen andere Anforderungen:

- begrenzte Scheduling-Latenzen,
- vorhersehbare Ausführung,
- reservierte Ressourcen,
- kontrollierte Interrupt-Einflüsse,
- begrenzte Synchronisationszeiten,
- definierte Deadline-Garantien.

Diese Anforderungen dürfen nicht lediglich durch eine besonders hohe Task-Priorität ausgedrückt werden.

NovaOS benötigt deshalb explizite Ausführungsbereiche, innerhalb derer strengere Realtime-Regeln gelten.

---

## Entscheidung

NovaOS führt **Realtime Concurrency Domains** als explizite Concurrency- und Scheduling-Domänen ein.

```text
Execution Domain
      ↓
Realtime Concurrency Domain
      ├── Execution Scope
      │      ├── Task
      │      └── Task
      ├── Reserved CPU Capacity
      ├── Realtime Scheduling Policy
      ├── Deadline Constraints
      └── Resource Reservations
```

Eine Realtime Concurrency Domain bündelt die für zeitkritische Ausführung notwendigen Scheduling-, Ressourcen- und Isolationseigenschaften.

Realtime-Verhalten entsteht nicht implizit durch hohe Priorität.

---

## Grundprinzip

```text
Realtime is an explicit execution contract.

Predictability takes precedence over throughput.

Realtime resources must be controlled,
not merely prioritized.
```

---

## Beziehung zur Execution Domain

Eine Realtime Concurrency Domain ersetzt keine Execution Domain.

Die Execution Domain bleibt die Sicherheits-, Ressourcen- und Prozessgrenze.

Die Realtime Concurrency Domain definiert innerhalb oder über kontrolliert verbundene Execution Domains hinweg die zeitkritische Ausführungsumgebung.

```text
Execution Domain
      ↓
Realtime Concurrency Domain
      ↓
Realtime Execution Scopes
```

---

## Realtime-Klassen

NovaOS muss unterschiedliche Realtime-Anforderungen ausdrücken können.

Mindestens werden konzeptionell unterschieden:

```text
Soft Realtime
Firm Realtime
Hard Realtime
```

### Soft Realtime

Deadline-Verletzungen reduzieren die Servicequalität, führen aber nicht unmittelbar zu einem ungültigen Systemzustand.

### Firm Realtime

Ein Ergebnis nach Ablauf seiner Deadline besitzt keinen oder nur noch begrenzten Nutzen.

### Hard Realtime

Die Einhaltung definierter zeitlicher Grenzen ist Bestandteil des verbindlichen Execution Contracts.

---

## Explizite Erstellung

Eine normale Task wird nicht automatisch zu Realtime Work, nur weil sie:

- hohe Priorität besitzt,
- eine kurze Deadline besitzt,
- CPU-intensiv ist.

Realtime-Ausführung muss explizit angefordert und durch die Systempolicy zugelassen werden.

---

## Admission Control

Hard-Realtime-Arbeit darf nur aktiviert werden, wenn NovaOS die notwendigen Ressourcen ausreichend garantieren kann.

```text
Realtime Request
      ↓
Validate Contract
      ↓
Admission Control
      ↓
Reserve Resources
      ↓
Accept / Reject
```

Kann NovaOS den Contract nicht erfüllen, muss die Ausführung abgelehnt oder ausdrücklich auf eine schwächere Realtime-Klasse zurückgestuft werden.

Eine solche Herabstufung darf niemals still erfolgen.

---

## CPU-Reservierung

Realtime Domains können CPU-Zeit oder CPUs beziehungsweise CPU Sets reservieren.

Mögliche Modelle umfassen:

```text
Dedicated CPU
Reserved CPU Capacity
Budget per Period
Controlled Shared CPU
```

Die konkrete Scheduling Policy bleibt austauschbar.

---

## Scheduling

Realtime Scheduling ist eine spezialisierte Policy des NovaOS-Schedulers.

NovaOS darf abhängig von Plattform und Contract geeignete Verfahren verwenden, beispielsweise:

- Fixed Priority,
- Earliest Deadline First,
- budgetbasierte Verfahren.

Die Architektur legt keinen einzelnen Realtime-Scheduling-Algorithmus für alle Workloads fest.

---

## Deadline Integration

Realtime Domains verwenden die systemweite Deadline-Semantik.

```text
Realtime Domain
      ↓
Execution Scope
      ↓
Task Group
      ↓
Task
```

Child Deadlines dürfen die Deadline ihres Parents verschärfen, aber nicht verlängern.

---

## Ressourcenreservierung

CPU-Priorität allein reicht für Realtime nicht aus.

Realtime Domains müssen weitere benötigte Ressourcen reservieren oder kontrollieren können, beispielsweise:

- Speicher,
- I/O-Kapazität,
- DMA-Ressourcen,
- Interrupt-Ressourcen,
- GPU-/NPU-Kapazität.

Die Resource Economy bleibt dafür die gemeinsame Systeminfrastruktur.

---

## Speicher

Hard-Realtime-Code darf nicht von unvorhersehbaren Speicherallokationen abhängig sein.

Realtime Domains müssen deshalb:

- Speicher vorreservieren,
- begrenzte Pools verwenden,
- oder andere deterministisch begrenzte Allocation-Mechanismen nutzen können.

---

## Synchronisation

Synchronisationsprimitive innerhalb einer Realtime Domain müssen Priority Inversion berücksichtigen.

Geeignete Mechanismen können beispielsweise:

- Priority Inheritance,
- Priority Ceiling,
- lockfreie Strukturen

verwenden.

Die konkrete Auswahl ist Mechanismus- beziehungsweise Policy-abhängig.

---

## Interrupts

Interrupt Routing muss mit Realtime Domains koordiniert werden können.

Nicht benötigte Interrupts sollen von dedizierten Hard-Realtime-CPUs ferngehalten werden können.

Benötigte Interrupts müssen kontrollierte Affinity- und Priority-Regeln besitzen.

---

## Deferred Work

Interrupt Deferred Work, das zu einem Realtime Workload gehört, muss dessen Realtime- und Ressourcenattribution erhalten können.

Es darf nicht unkontrolliert in einen allgemeinen Worker Pool verschoben werden.

---

## Work Stealing

Allgemeines dynamisches Work Stealing ist für Hard-Realtime-Domänen nicht der Standard.

Work Stealing darf nur verwendet werden, wenn der Realtime Contract dies zulässt und die zulässigen Worker begrenzt sind.

```text
General Domain:
    Dynamic Work Stealing

Hard Realtime Domain:
    Controlled / Restricted Scheduling
```

---

## Async/Await

Async/Await bleibt innerhalb von Realtime Domains zulässig.

Eine Await-Operation darf jedoch nur Bestandteil eines Hard-Realtime-Pfads sein, wenn deren maximale relevante Latenz mit dem Contract vereinbar ist.

Async/Await selbst erzeugt keine Realtime-Garantie.

---

## Blocking

Unbegrenztes Blocking ist innerhalb einer Hard-Realtime-Domain nicht zulässig.

Blockierende Operationen müssen entweder:

- zeitlich begrenzt,
- reserviert,
- vorhersehbar,
- oder außerhalb des kritischen Realtime-Pfads

sein.

---

## Cancellation

Realtime Tasks unterstützen die systemweite Cancellation Propagation.

Cancellation und Cleanup müssen jedoch ebenfalls begrenzte Ausführungszeiten besitzen können.

Eine Cancellation darf keinen unbegrenzten Cleanup-Pfad erzeugen.

---

## Isolation

Realtime Domains müssen vor störenden allgemeinen Workloads geschützt werden können.

Dazu gehören insbesondere:

- CPU-Überlastung,
- unkontrollierte Interrupts,
- Speicherpressure,
- ungebundene Kernelarbeit.

Isolation bedeutet dabei nicht zwingend vollständige Hardwareexklusivität.

---

## Hardwaretopologie

Realtime Placement muss die Hardwaretopologie berücksichtigen können.

Relevant sind insbesondere:

- NUMA,
- SMT,
- Shared Caches,
- Interrupt Controller,
- Accelerator Topology.

Für besonders strenge Contracts kann die gemeinsame Nutzung bestimmter Hardware-Ressourcen eingeschränkt werden.

---

## SMT

SMT-Siblings teilen Teile der CPU-Ressourcen.

Hard-Realtime-Domänen müssen deshalb festlegen können, ob:

```text
SMT sharing allowed
```

oder:

```text
exclusive physical core required
```

gilt.

---

## Energie- und Thermalpolitik

Energiesparmechanismen und dynamische Frequenzänderungen können Ausführungslatenzen beeinflussen.

Realtime Domains müssen deshalb geeignete CPU-Power- und Performance-Constraints anfordern können.

Safety- und Thermal-Grenzen besitzen weiterhin Vorrang.

---

## Deterministic Mode

Realtime und Deterministic Mode sind verwandte, aber unterschiedliche Konzepte.

```text
Realtime
    → bounded timing

Deterministic
    → reproducible behavior
```

Eine Realtime Domain kann zusätzlich deterministische Ausführung verlangen.

Keine der beiden Eigenschaften impliziert automatisch die andere.

---

## Graceful Degradation

Soft- und Firm-Realtime-Domänen können bei Ressourcenknappheit degradieren.

Beispielsweise:

```text
Reduce quality
Cancel optional work
Reduce sampling rate
Switch algorithm
```

Hard-Realtime-Garantien dürfen dagegen nicht still degradiert werden.

Kann ein Hard Contract nicht mehr erfüllt werden, muss dies als explizite Contract-Verletzung behandelt werden.

---

## Algorithmusauswahl

Die automatische Algorithmusauswahl von NovaOS darf innerhalb einer Realtime Domain nur Algorithmen wählen, deren Eigenschaften mit dem Execution Contract kompatibel sind.

Ein schnellerer Algorithmus ist nicht automatisch geeignet, wenn seine Worst-Case-Latenz unvorhersehbar ist.

Explizit erzwungene Algorithmen unterliegen denselben Hard Constraints.

---

## Introspection

Realtime Domains müssen systemweit introspektierbar sein.

Mindestens relevant sind:

```text
Realtime Class
Scheduling Policy
CPU Assignment
Resource Reservations
Deadline State
Budget Usage
Deadline Misses
```

Damit können Realtime-Verletzungen nachvollzogen werden.

---

## Fail-Safe Defaults

Eine Realtime-Garantie darf nur ausgewiesen werden, wenn NovaOS sie tatsächlich bereitstellen kann.

Unbekannte Worst-Case-Eigenschaften dürfen nicht als garantiert interpretiert werden.

Bei Unsicherheit gilt:

```text
No proven guarantee
    =
No hard realtime guarantee
```

---

## Keine KI-Abhängigkeit

Hard-Realtime-Korrektheit darf nicht von KI oder Vorhersagemodellen abhängen.

Adaptive Systeme dürfen Soft-Realtime-Workloads optimieren oder Ressourcenbedarf prognostizieren.

Sie dürfen keine Hard-Realtime-Garantie ersetzen.

---

## Normative Anforderungen

1. NovaOS MUSS explizite Realtime Concurrency Domains unterstützen.
2. Realtime-Ausführung DARF NICHT allein aus einer hohen Task-Priorität abgeleitet werden.
3. NovaOS MUSS Soft-, Firm- und Hard-Realtime-Anforderungen ausdrücken können.
4. Hard-Realtime-Ausführung MUSS einem expliziten Execution Contract unterliegen.
5. Hard-Realtime-Arbeit MUSS vor Aktivierung einer geeigneten Admission Control unterliegen können.
6. Nicht erfüllbare Hard-Realtime-Contracts DÜRFEN NICHT still akzeptiert werden.
7. Eine Herabstufung der Realtime-Klasse MUSS explizit sein.
8. Realtime Domains MÜSSEN CPU-Ressourcen reservieren oder kontrollieren können.
9. Realtime Scheduling MUSS als spezialisierte Scheduling Policy behandelt werden.
10. NovaOS DARF NICHT von einem einzigen Realtime-Scheduling-Algorithmus abhängig sein.
11. Realtime Domains MÜSSEN die systemweite Deadline Propagation verwenden.
12. Realtime-Ressourcen MÜSSEN mit der Resource Economy integrierbar sein.
13. Hard-Realtime-Domänen MÜSSEN vorhersehbare Speicherstrategien verwenden können.
14. Synchronisationsmechanismen MÜSSEN Priority Inversion kontrollieren können.
15. Interrupt Affinity MUSS mit Realtime Domains koordinierbar sein.
16. Realtime-bezogenes Deferred Work MUSS seine Scheduling- und Ressourcenattribution erhalten können.
17. Allgemeines unbeschränktes Work Stealing DARF NICHT Standard für Hard-Realtime-Domänen sein.
18. Work Stealing MUSS innerhalb von Realtime Domains einschränkbar oder deaktivierbar sein.
19. Async/Await DARF nur dann Hard-Realtime-Pfade bilden, wenn die verwendeten Mechanismen mit deren Zeitgrenzen kompatibel sind.
20. Unbegrenztes Blocking DARF NICHT Bestandteil eines Hard-Realtime-Pfads sein.
21. Cancellation und Cleanup MÜSSEN für Realtime Workloads begrenzbar sein.
22. Realtime Domains MÜSSEN gegen unkontrollierte Störungen allgemeiner Workloads isolierbar sein.
23. NUMA-, Cache-, SMT- und Interrupt-Topologie MÜSSEN bei Realtime Placement berücksichtigt werden können.
24. Realtime Domains MÜSSEN SMT-Sharing einschränken können.
25. Realtime Domains MÜSSEN notwendige Power- und Performance-Constraints ausdrücken können.
26. Realtime und Deterministic Mode MÜSSEN getrennte, kombinierbare Eigenschaften bleiben.
27. Hard-Realtime-Garantien DÜRFEN NICHT still durch Graceful Degradation abgeschwächt werden.
28. Realtime Domains und Deadline-Verletzungen MÜSSEN introspektierbar sein.
29. Unbewiesene zeitliche Eigenschaften DÜRFEN NICHT als Hard-Realtime-Garantie behandelt werden.
30. Hard-Realtime-Korrektheit MUSS vollständig ohne KI oder adaptive Vorhersagemodelle gewährleistet werden.

---

## Konsequenzen

### Positive Konsequenzen

- klare Trennung allgemeiner und zeitkritischer Concurrency,
- echte Ressourcenreservierung statt bloßer Priorisierung,
- Integration mit Structured Concurrency und Deadline Propagation,
- Unterstützung von Soft bis Hard Realtime,
- bessere Isolation zeitkritischer Workloads,
- hardware- und schedulerunabhängige Architektur.

### Negative Konsequenzen

- Realtime Admission Control erhöht Scheduler-Komplexität,
- Hardware und Treiber müssen Worst-Case-Eigenschaften bereitstellen können,
- bestimmte Optimierungen wie aggressives Work Stealing sind eingeschränkt,
- Hard-Realtime-Unterstützung benötigt strengere Ressourcen- und Treibercontracts.

---

## Verworfene Alternativen

### Realtime ausschließlich über hohe Priorität

Verworfen.

Priorität reserviert weder CPU-Zeit noch Speicher-, I/O- oder Interrupt-Ressourcen.

### Alle Tasks als Realtime behandeln

Verworfen.

Dies würde Durchsatz, Energieeffizienz und allgemeine Scheduling-Flexibilität unnötig einschränken.

### Ein fester Realtime-Scheduler für alle Plattformen

Verworfen.

Unterschiedliche Workloads und Hardwareplattformen benötigen unterschiedliche Scheduling Policies.

### Hard Realtime mit normalem dynamischem Work Stealing

Verworfen.

Unkontrollierte Task-Migration erschwert begrenzte Worst-Case-Latenzen.

### KI-basierte Realtime-Garantien

Verworfen.

Vorhersagen können Optimierungen unterstützen, aber keine beweisbaren zeitlichen Garantien ersetzen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-CONCURRENCY-0001_Structured_Concurrency`
- `ADR-CONCURRENCY-0002_Task_Groups_mit_hierarchischer_Lebensdauer`
- `ADR-CONCURRENCY-0003_Cancellation_Propagation`
- `ADR-CONCURRENCY-0004_Deadline_Propagation`
- `ADR-CONCURRENCY-0005_Async_Await_als_native_Abstraktion`
- `ADR-CONCURRENCY-0006_Work_Stealing_für_allgemeine_Task_Workloads`
- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-INTERRUPT-0002_Interrupt_Affinity_und_Topology_Awareness`
- `ADR-INTERRUPT-0003_Deferred_Work_statt_langer_Interrupt_Handler`

---

## Zugehörige NPSPECs

- `NPSPEC-CONCURRENCY-REALTIME-0001`
- `NPSPEC-CONCURRENCY-REALTIME-DOMAIN-0001`
- `NPSPEC-CONCURRENCY-REALTIME-ADMISSION-0001`
- `NPSPEC-CONCURRENCY-REALTIME-SCHEDULING-0001`
- `NPSPEC-CONCURRENCY-REALTIME-RESOURCE-0001`
- `NPSPEC-CONCURRENCY-REALTIME-SYNCHRONIZATION-0001`
- `NPSPEC-CONCURRENCY-REALTIME-ISOLATION-0001`
- `NPSPEC-CONCURRENCY-REALTIME-TEST-0001`

---

## Ergebnis

NovaOS behandelt Realtime Concurrency als explizite Ausführungsdomäne mit kontrollierten zeitlichen und ressourcenbezogenen Garantien.

```text
Realtime Execution Contract
        ↓
Admission Control
        ↓
Realtime Concurrency Domain
        ↓
Reserved Resources
        ↓
Structured Realtime Tasks
```

Damit wird Realtime nicht als bloße Priorität, sondern als überprüfbarer Execution Contract behandelt.

Die zentrale Architekturregel lautet:

```text
Realtime is explicit.

Reserve before promising.

Control interference.

Never claim guarantees
the system cannot enforce.
```