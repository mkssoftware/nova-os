# ADR-SCHED-0001 – Präemptiver Mehrklassen-Scheduler

## Status

Angenommen

## Kategorie

Kernel / Scheduler / Concurrency / Realtime / Ressourcen / Execution Contracts

## Kontext

NovaOS muss sehr unterschiedliche Arten von Arbeit gleichzeitig ausführen können:

- interaktive Anwendungen,
- allgemeine Tasks,
- Systemdienste,
- Hintergrundarbeit,
- Treiberarbeit,
- Realtime-Workloads,
- deterministische Workloads.

Ein einzelnes Scheduling-Verfahren ist für diese unterschiedlichen Anforderungen nicht optimal.

Insbesondere dürfen allgemeine Durchsatzoptimierungen wie Work Stealing nicht die Garantien von Realtime- oder isolierten Workloads verletzen.

Gleichzeitig soll NovaOS keinen vollständig getrennten Scheduler pro Workload-Typ besitzen.

---

## Entscheidung

NovaOS verwendet einen **präemptiven Mehrklassen-Scheduler**.

```text
Runnable Work
      ↓
Scheduling Class
      ↓
Class-specific Policy
      ↓
Global Constraint Resolution
      ↓
CPU Selection
      ↓
Execution
```

Der Scheduler besitzt einen gemeinsamen Kern für:

- Präemption,
- Runnable-State,
- CPU-Zuordnung,
- Accounting,
- Affinity,
- Topologie,
- Ressourcenconstraints,

während unterschiedliche Scheduling Classes spezialisierte Policies verwenden können.

---

## Grundprinzip

```text
One scheduler framework.

Multiple scheduling classes.

Hard constraints before optimization.

Preemption where required.
```

---

## Scheduling Classes

NovaOS definiert mindestens konzeptionell folgende Klassen:

```text
Realtime
Interactive
Normal
Background
Idle
```

Weitere spezialisierte Klassen können später ergänzt werden, ohne das grundlegende Scheduler-Modell zu verändern.

Die Scheduling Class beschreibt Policy, nicht Security Authority.

---

## Realtime

Die Realtime-Klasse ist für Workloads vorgesehen, die über explizite Realtime Execution Contracts zugelassen wurden.

Sie kann spezialisierte Verfahren wie:

- Fixed Priority,
- Earliest Deadline First,
- Budget-/Periodenmodelle

verwenden.

Realtime Scheduling unterliegt den Regeln der Realtime Concurrency Domains.

---

## Interactive

Die Interactive-Klasse optimiert auf geringe wahrgenommene Antwortlatenz.

Typische Workloads sind:

- UI-Interaktion,
- Eingabeverarbeitung,
- kurzzeitige Benutzeraktionen.

Interaktive Arbeit darf bevorzugt werden, ohne allgemeine Workloads dauerhaft auszuhungern.

---

## Normal

Die Normal-Klasse ist der Standard für allgemeine Anwendungen und Tasks.

Sie optimiert primär auf:

- Fairness,
- Durchsatz,
- Skalierbarkeit,
- Locality.

Work Stealing kann innerhalb dieser Klasse als wesentlicher Load-Balancing-Mechanismus verwendet werden.

---

## Background

Background Work besitzt geringere zeitliche Dringlichkeit.

Dazu können beispielsweise gehören:

- Indexierung,
- Wartungsaufgaben,
- Prefetching,
- optionale Optimierungen.

Background Work darf freie Ressourcen nutzen, muss aber gegenüber wichtigeren Workloads zurücktreten können.

---

## Idle

Idle Work wird nur ausgeführt, wenn keine höherwertige zulässige Arbeit vorhanden ist.

Diese Klasse eignet sich für vollständig opportunistische Tätigkeiten.

---

## Präemption

NovaOS verwendet präemptives Scheduling.

Der Scheduler muss laufende Arbeit unterbrechen können, wenn beispielsweise:

- eine höherwertige Scheduling Class runnable wird,
- eine Realtime-Anforderung dies verlangt,
- ein Scheduling Budget endet,
- Fairness dies erfordert.

Präemption muss kontrolliert und an definierten kernelinternen Grenzen sicher erfolgen.

---

## Class Ordering

Scheduling Classes besitzen keine einfache universelle numerische Priorität.

Die Auswahl folgt den systemweiten Constraints und Policies.

Grundsätzlich gilt:

```text
Safety
Security / Isolation
Hard Realtime
Deterministic Constraints
Required Resource Constraints
Scheduling Class Policy
Fairness / Performance
Energy / Adaptive Optimization
```

Eine Scheduling Class darf übergeordnete Hard Constraints nicht überschreiben.

---

## Prioritäten

Innerhalb einer Scheduling Class können zusätzliche Prioritäten existieren.

```text
Scheduling Class
      ↓
Class-local Priority / Policy
```

Eine hohe Priorität innerhalb `Normal` macht einen Task nicht automatisch zu einem Realtime Task.

---

## Zeitscheiben und Budgets

Scheduling Classes dürfen unterschiedliche Ausführungsmodelle verwenden.

Dazu gehören:

- Zeitscheiben,
- CPU Budgets,
- Perioden,
- Deadlines,
- kooperative Laufgrenzen.

Der Scheduler-Core darf deshalb nicht von einem einzigen festen Timeslice-Modell abhängen.

---

## Resource Economy

CPU Scheduling ist Bestandteil der systemweiten Resource Economy.

CPU-Verbrauch wird den verursachenden:

- Tasks,
- Task Groups,
- Execution Scopes,
- Execution Domains

zugeordnet.

Scheduling-Priorität darf Resource Budgets nicht umgehen.

---

## Structured Concurrency

Der Scheduler plant ausführbare Tasks, verändert aber nicht deren Ownership.

```text
Task Ownership
    ≠
CPU Placement
```

Cancellation, Deadlines und Task-Group-Lifecycle bleiben unabhängig von einem CPU-Wechsel erhalten.

---

## Deadline Integration

Deadlines werden als Execution Constraints an den Scheduler übergeben.

Eine Deadline ist nicht automatisch identisch mit einer Scheduling-Priorität.

Die jeweilige Scheduling Class entscheidet, wie sie Deadline-Informationen verwendet.

---

## Affinity

Tasks können zulässige CPU Sets definieren.

Der Scheduler darf einen Task nur auf CPUs platzieren, die mit seiner effektiven Affinity vereinbar sind.

Affinity kann:

```text
Required
Preferred
```

sein.

---

## Topology Awareness

CPU-Auswahl muss den Hardware System Graph berücksichtigen können.

Relevant sind insbesondere:

- CPU Packages,
- Cores,
- SMT Threads,
- Shared Caches,
- NUMA Nodes,
- Memory Locality.

---

## NUMA

Der Scheduler soll CPU- und Memory-Placement gemeinsam optimieren können.

Eine kurzfristig freie CPU auf einem entfernten NUMA Node ist nicht automatisch die beste Ziel-CPU.

Locality bleibt jedoch ein Optimierungsziel und darf Hard Constraints nicht überschreiben.

---

## Work Stealing

Allgemeine Scheduling Classes dürfen Work Stealing für Load Balancing verwenden.

Realtime-, gepinnte oder anderweitig eingeschränkte Tasks dürfen davon ausgenommen werden.

Damit bleibt Work Stealing ein Policy-Baustein und nicht das universelle Scheduler-Modell.

---

## CPU Hotplug

Der Scheduler muss auf das Hinzufügen und Entfernen von CPUs reagieren können.

Bei CPU Removal müssen betroffene Tasks kontrolliert auf zulässige CPUs verschoben oder entsprechend ihrer Contracts behandelt werden.

Pinned Work benötigt eine explizite Fehler- beziehungsweise Degradationspolicy.

---

## Priority Inversion

Der Scheduler muss mit Synchronisationsmechanismen zur Behandlung von Priority Inversion zusammenarbeiten.

Insbesondere müssen Verfahren wie Priority Inheritance oder vergleichbare Mechanismen integrierbar sein.

---

## Starvation

Scheduling Policies müssen unbeabsichtigte dauerhafte Starvation vermeiden.

Ausnahmen sind nur zulässig, wenn ein expliziter Hard Contract die Ressource vollständig reserviert.

Background- und Idle-Klassen besitzen keinen Anspruch auf dieselbe Latenz wie höhere Klassen.

---

## Deterministic Mode

Deterministic Mode muss Scheduling-Entscheidungen einschränken können.

Dazu können gehören:

- feste CPU-Zuordnung,
- stabile Runnable-Reihenfolge,
- deaktiviertes adaptives Load Balancing,
- deterministisches Work Stealing,
- kontrollierte Präemptionspunkte.

Determinismus ist eine Execution-Eigenschaft und keine eigene zwingende Scheduling Class.

---

## Graceful Degradation

Bei Ressourcenknappheit darf NovaOS zunächst weniger wichtige Arbeit reduzieren.

Beispiel:

```text
Resource Pressure
      ↓
Idle Work suppress
      ↓
Background throttle
      ↓
Optional Normal Work reduce
      ↓
Protect Required / Realtime Work
```

Hard Safety-, Security- und Realtime-Constraints bleiben davon unberührt.

---

## Energie und Thermal

Der Scheduler darf Energie- und Thermalzustände bei Soft-Policy-Entscheidungen berücksichtigen.

Beispielsweise kann Arbeit auf weniger CPUs konzentriert oder auf energieeffiziente Kerne gelegt werden.

Thermal Safety besitzt Vorrang vor Performance- und Realtime-Optimierung; kann eine Garantie dadurch nicht mehr erfüllt werden, muss dies explizit sichtbar werden.

---

## Heterogene CPUs

Das Scheduler-Modell muss unterschiedliche CPU-Leistungsklassen unterstützen können.

Beispiele sind Performance- und Efficiency-Cores.

CPU-Auswahl darf daher nicht voraussetzen, dass alle logischen CPUs gleichwertig sind.

---

## Introspection

Scheduler-Zustand und wesentliche Entscheidungen müssen introspektierbar sein.

Mindestens sollen erkennbar sein:

```text
Task
Scheduling Class
Priority
CPU
Affinity
Runtime
Budget
Deadline
Runnable State
```

Für Diagnosezwecke sollen Scheduling-Entscheidungen nachvollziehbar sein können.

---

## Adaptive Scheduling

NovaOS darf Soft Scheduling Policies adaptiv optimieren.

Dazu können beispielsweise gehören:

- Timeslice-Größe,
- Wakeup Placement,
- Work-Stealing-Parameter,
- Locality-Gewichtung,
- Interactive Heuristics.

Prediction Error darf als Lernsignal verwendet werden.

Hard Constraints und Scheduler-Korrektheit dürfen davon nicht abhängen.

---

## Fail-Safe Defaults

Ist unklar, ob ein CPU-Placement zulässig ist, darf der Task dort nicht ausgeführt werden.

Eine Scheduling-Optimierung darf niemals:

- Security Isolation,
- Hard Affinity,
- Realtime Reservation,
- Data Sovereignty,
- Resource Limits

verletzen.

---

## Keine KI-Abhängigkeit

Der vollständige Scheduler muss ohne KI und adaptive Modelle korrekt funktionieren.

KI darf ausschließlich beratend beziehungsweise zur Optimierung von Soft Policies eingesetzt werden.

---

## Normative Anforderungen

1. NovaOS MUSS einen präemptiven Scheduler verwenden.
2. Der Scheduler MUSS mehrere Scheduling Classes unterstützen.
3. NovaOS MUSS mindestens Realtime-, Interactive-, Normal-, Background- und Idle-Semantik ausdrücken können.
4. Scheduling Classes MÜSSEN spezialisierte Policies verwenden können.
5. Alle Scheduling Classes MÜSSEN auf einem gemeinsamen Scheduler-Core aufbauen können.
6. Scheduling Class und Security Authority MÜSSEN getrennte Konzepte bleiben.
7. Eine hohe normale Priorität DARF NICHT automatisch Realtime-Rechte erzeugen.
8. Präemption MUSS bei höherwertiger runnable Arbeit möglich sein.
9. Präemption MUSS an kernelsicheren Grenzen kontrollierbar sein.
10. Der Scheduler DARF NICHT von einem einzigen Timeslice-Modell abhängig sein.
11. CPU-Verbrauch MUSS der Resource Economy zurechenbar sein.
12. Scheduling-Priorität DARF Resource Budgets NICHT umgehen.
13. Task Ownership DARF durch Scheduling und CPU-Migration NICHT verändert werden.
14. Cancellation und Deadline Contexts MÜSSEN bei CPU-Wechsel erhalten bleiben.
15. Deadline-Information MUSS vom Scheduler berücksichtigt werden können.
16. Deadline und Scheduling Priority MÜSSEN getrennte Konzepte bleiben.
17. Required CPU Affinity MUSS strikt eingehalten werden.
18. Preferred Affinity SOLL als Optimierungsziel behandelt werden.
19. Der Scheduler MUSS Hardwaretopologie berücksichtigen können.
20. NUMA- und Cache-Lokalität SOLLEN bei CPU-Placement berücksichtigt werden.
21. Work Stealing MUSS als klassenspezifische Policy integrierbar sein.
22. Realtime Work MUSS von allgemeinem dynamischem Load Balancing isolierbar sein.
23. CPU Hotplug MUSS ohne Verlust der Scheduler-Konsistenz unterstützt werden.
24. Priority-Inversion-Mechanismen MÜSSEN integrierbar sein.
25. Unbeabsichtigte dauerhafte Starvation SOLL verhindert werden.
26. Deterministic Mode MUSS dynamische Scheduling-Optimierungen einschränken können.
27. Heterogene CPU-Leistungsklassen MÜSSEN darstellbar sein.
28. Scheduler-Zustand und wesentliche Scheduling-Eigenschaften MÜSSEN introspektierbar sein.
29. Adaptive Optimierung DARF ausschließlich Soft Policies verändern.
30. Scheduler-Korrektheit MUSS vollständig ohne KI gewährleistet sein.

---

## Konsequenzen

### Positive Konsequenzen

- ein gemeinsames Scheduling-Framework für unterschiedliche Workloads,
- klare Trennung von Realtime und allgemeiner Ausführung,
- präemptive Reaktionsfähigkeit,
- Integration mit Structured Concurrency,
- NUMA- und Topology Awareness,
- Unterstützung heterogener CPUs,
- flexible zukünftige Scheduling Policies.

### Negative Konsequenzen

- komplexer als ein Scheduler mit nur einer Policy,
- Interaktionen zwischen Scheduling Classes müssen klar definiert werden,
- Realtime und Resource Reservations benötigen zusätzliche Admission-Control-Mechanismen,
- Introspection und Accounting werden integraler Bestandteil des Scheduler-Designs.

---

## Verworfene Alternativen

### Ein Scheduling-Algorithmus für alle Workloads

Verworfen.

Realtime, interaktive und allgemeine Workloads besitzen unterschiedliche Anforderungen.

### Rein kooperatives Scheduling

Verworfen.

Ein fehlerhafter oder lang laufender Task könnte die Reaktionsfähigkeit anderer Workloads unkontrolliert beeinträchtigen.

### Globale numerische Priorität für sämtliche Arbeit

Verworfen.

Dies kann Realtime-, Ressourcen- und Isolationseigenschaften nicht ausreichend ausdrücken.

### Vollständig getrennte Scheduler pro Klasse

Verworfen.

Dies würde CPU-Placement, Accounting, Topologie und Ressourcenverwaltung unnötig duplizieren.

### KI-gesteuerter Scheduler als Grundlage

Verworfen.

Scheduler-Korrektheit und Hard Constraints müssen unabhängig von Vorhersagemodellen garantiert werden.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-CONCURRENCY-0001_Structured_Concurrency`
- `ADR-CONCURRENCY-0003_Cancellation_Propagation`
- `ADR-CONCURRENCY-0004_Deadline_Propagation`
- `ADR-CONCURRENCY-0005_Async_Await_als_native_Abstraktion`
- `ADR-CONCURRENCY-0006_Work_Stealing_für_allgemeine_Task_Workloads`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-INTERRUPT-0002_Interrupt_Affinity_und_Topology_Awareness`

---

## Zugehörige NPSPECs

- `NPSPEC-SCHED-CORE-0001`
- `NPSPEC-SCHED-CLASS-0001`
- `NPSPEC-SCHED-PREEMPTION-0001`
- `NPSPEC-SCHED-NORMAL-0001`
- `NPSPEC-SCHED-INTERACTIVE-0001`
- `NPSPEC-SCHED-BACKGROUND-0001`
- `NPSPEC-SCHED-REALTIME-0001`
- `NPSPEC-SCHED-AFFINITY-0001`
- `NPSPEC-SCHED-TOPOLOGY-0001`
- `NPSPEC-SCHED-ACCOUNTING-0001`
- `NPSPEC-SCHED-TEST-0001`

---

## Ergebnis

NovaOS verwendet einen gemeinsamen präemptiven Scheduler-Core mit mehreren spezialisierten Scheduling Classes:

```text
                 Scheduler Core
                       │
        ┌──────────────┼──────────────┐
        ↓              ↓              ↓
    Realtime       Interactive      Normal
                                      │
                               Work Stealing
        ↓
    Background
        ↓
       Idle
```

Damit kann NovaOS unterschiedliche Workload-Anforderungen behandeln, ohne mehrere voneinander unabhängige Scheduler-Systeme einzuführen.

Die zentrale Architekturregel lautet:

```text
One scheduling framework.

Multiple workload policies.

Hard constraints first.

Optimization second.
```