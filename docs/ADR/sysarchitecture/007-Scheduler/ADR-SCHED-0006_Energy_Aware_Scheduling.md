# ADR-SCHED-0006 – Energy-Aware Scheduling

## Status

Angenommen

## Kategorie

Kernel / Scheduler / Energie / Performance / Thermal / Ressourcen / Hardwaretopologie

## Kontext

NovaOS berücksichtigt bei Scheduling-Entscheidungen bereits:

- Scheduling Class,
- Deadlines,
- Realtime-Anforderungen,
- CPU-Auslastung,
- NUMA-Lokalität,
- Cache-Lokalität,
- CPU-Affinity.

Moderne Prozessoren unterscheiden sich jedoch zusätzlich erheblich hinsichtlich Energieverbrauch und Energieeffizienz.

Dies betrifft insbesondere:

- Performance- und Efficiency-Cores,
- unterschiedliche Frequenz- und Power-States,
- CPU Packages und Power Domains,
- Idle States,
- mobile Systeme mit begrenzter Akkukapazität,
- thermisch begrenzte Systeme.

Ein Scheduler, der ausschließlich maximale Performance anstrebt, kann unnötig Energie verbrauchen. Eine ausschließlich energiesparende Strategie kann dagegen Latenz-, Deadline- oder Realtime-Anforderungen verletzen.

Energie muss deshalb als systemweites Optimierungsziel in die Scheduling-Architektur integriert werden.

---

## Entscheidung

NovaOS verwendet **Energy-Aware Scheduling** als integrierte Soft Policy des Mehrklassen-Schedulers.

```text
Runnable Work
      ↓
Hard Constraints
      ↓
Eligible CPUs
      ↓
Performance Requirement
      +
Energy Cost
      +
Topology / Locality
      +
Thermal State
      ↓
Scheduling Policy
      ↓
Target CPU
```

Der Scheduler darf CPU-Placement, Workload-Konsolidierung und Parallelitätsgrad so wählen, dass der Energieverbrauch reduziert wird, solange höherwertige Execution Contracts eingehalten werden.

---

## Grundprinzip

```text
Use only as much performance
as the workload requires.

Save energy when possible.

Never trade hard guarantees
for energy efficiency.
```

---

## Energie als Soft Constraint

Energieeffizienz ist standardmäßig ein Optimierungsziel.

Sie besitzt keinen Vorrang vor:

- Safety,
- Security,
- Hard Realtime,
- Hard Deadlines,
- Required Affinity,
- Resource Reservations,
- Deterministic Constraints.

Innerhalb der verbleibenden zulässigen Entscheidungen darf der Scheduler energieeffiziente Varianten bevorzugen.

---

## Hardware Energy Model

Die HAL beziehungsweise Platform Provider stellen verfügbare Informationen über Energieeigenschaften der Hardware bereit.

Dazu können gehören:

```text
CPU Performance Class
Power Domain
Frequency Range
Idle States
Energy Cost
Thermal State
```

Nicht jede Plattform muss sämtliche Informationen liefern.

Der Scheduler muss auch mit unvollständigen Energy Models funktionieren.

---

## Heterogene CPUs

Energy-Aware Scheduling muss heterogene CPU-Klassen unterstützen.

Beispielsweise:

```text
Performance Core
    → hohe Leistung
    → höherer Energieverbrauch

Efficiency Core
    → geringere Leistung
    → bessere Energieeffizienz
```

Geeignete Background- oder Normal-Workloads können bevorzugt auf Efficiency-Cores ausgeführt werden.

Latenzkritische Arbeit kann Performance-Cores bevorzugen.

---

## Performance Requirement

Der Scheduler soll nicht pauschal den energieärmsten CPU-Typ auswählen.

Stattdessen gilt:

```text
Required Performance
        ↓
Eligible CPU Set
        ↓
Most efficient suitable target
```

Eine CPU ist nur dann energieeffizient für einen Workload, wenn sie dessen relevante Anforderungen ausreichend erfüllen kann.

---

## Workload Consolidation

Bei geringer Systemlast darf NovaOS Arbeit auf weniger CPUs beziehungsweise Power Domains konzentrieren.

```text
Low Load
   ↓
Consolidate Work
   ↓
Idle Remaining CPUs
   ↓
Deeper Power States
```

Dadurch können ungenutzte Hardwarebereiche in energiesparende Zustände wechseln.

---

## Workload Spreading

Konsolidierung ist nicht immer optimal.

Bei:

- hoher Last,
- Thermal Pressure,
- Deadline-Anforderungen,
- Cache Contention

kann eine breitere Verteilung effizienter sein.

Energy-Aware Scheduling muss deshalb Konsolidierung und Verteilung gegeneinander abwägen können.

---

## Idle States

Der Scheduler soll längere zusammenhängende Idle-Zeiten ermöglichen können, damit CPUs tiefere Idle States erreichen.

Dabei müssen die Wakeup-Kosten berücksichtigt werden.

Tiefe Idle States dürfen nicht verwendet werden, wenn ihre Wakeup-Latenz mit einem Hard Execution Contract unvereinbar ist.

---

## Frequency Scaling

Energy-Aware Scheduling muss mit CPU-Frequenz- und Performance-State-Management koordinierbar sein.

Der Scheduler kann Anforderungen wie:

```text
Low
Balanced
High
Realtime Required
```

an die zuständige Power Policy übermitteln.

Die konkrete Frequenzsteuerung bleibt Aufgabe der Power-/Platform-Infrastruktur.

---

## Realtime

Hard-Realtime-Contracts besitzen Vorrang vor Energieoptimierung.

Eine Realtime Domain kann erforderliche:

- CPU-Klassen,
- Performance States,
- Wakeup-Latenzen,
- Reservations

festlegen.

Energy-Aware Scheduling darf diese Anforderungen nicht abschwächen.

---

## Deadlines

Bei Workloads mit Soft Deadlines darf der Scheduler energieeffiziente Ausführung wählen, solange ausreichend zeitlicher Spielraum besteht.

Konzeptionell:

```text
Large Slack
    → Energy Efficient Execution

Small Slack
    → Higher Performance
```

Deadline Slack darf damit als Soft Scheduling-Signal verwendet werden.

---

## Background Work

Background- und Idle-Workloads eignen sich besonders für Energy-Aware Scheduling.

Sie können beispielsweise:

- verzögert,
- gebündelt,
- auf Efficiency-Cores verschoben,
- bei Thermal Pressure gedrosselt

werden.

Required Work darf dadurch nicht unbegrenzt verzögert werden.

---

## Work Stealing

Work Stealing muss mit Energy-Aware Scheduling koordinierbar sein.

Ein Worker soll nicht allein deshalb aufgeweckt werden, weil Arbeit gestohlen werden könnte.

Bei geringer Last kann es effizienter sein, Arbeit auf bereits aktiven CPUs zu belassen.

Bei hoher Last darf Work Stealing zusätzliche CPUs aktivieren.

---

## NUMA und Cache Locality

Energieoptimierung darf NUMA- und Cache-Kosten nicht isoliert betrachten.

Eine scheinbar energieeffiziente CPU kann durch:

- Remote Memory Access,
- Cache-Verlust,
- zusätzliche Interconnect-Aktivität

insgesamt ungünstiger sein.

Die Scheduling Policy muss diese Kosten gemeinsam bewerten können.

---

## Thermal State

Thermal Management und Energy-Aware Scheduling müssen zusammenarbeiten.

Bei hoher thermischer Belastung kann der Scheduler:

- Arbeit verlagern,
- Parallelität reduzieren,
- weniger kritische Arbeit drosseln,
- andere CPU-Klassen verwenden.

Thermal Safety besitzt Vorrang vor Performance-Garantien. Wird dadurch ein Contract unerfüllbar, muss dies explizit behandelt werden.

---

## Resource Economy

Energie ist Bestandteil der systemweiten Resource Economy.

Execution Domains beziehungsweise Workloads können Energie- oder Power-Budgets besitzen.

Beispiel:

```text
Execution Domain
      ↓
Energy Budget
      ↓
Tasks
```

Scheduling und Resource Accounting müssen solche Budgets berücksichtigen können.

---

## Energy Budget

Energy Budgets können insbesondere für:

- Background Services,
- Anwendungen,
- Agents,
- Batch Workloads

verwendet werden.

Das Überschreiten eines Budgets kann abhängig vom Contract zu:

```text
Throttle
Defer
Degrade
Reject
```

führen.

---

## Battery State

Auf mobilen Systemen darf der verfügbare Energiezustand die Soft Scheduling Policy beeinflussen.

Beispielsweise kann NovaOS zwischen:

```text
Performance
Balanced
Energy Saving
Critical Battery
```

unterschiedliche Policy-Gewichtungen verwenden.

Anwendungen erhalten dadurch keine direkte Kontrolle über Hardware-Power-Mechanismen.

---

## Graceful Degradation

Bei Energieknappheit darf NovaOS optionale Arbeit reduzieren.

Beispiel:

```text
Energy Pressure
      ↓
Reduce Background Work
      ↓
Reduce Optional Parallelism
      ↓
Select Efficient Algorithm
      ↓
Preserve Required Work
```

Safety, Security, Correctness und andere Hard Constraints bleiben erhalten.

---

## Algorithmusauswahl

Energy-Aware Scheduling kann mit der automatischen Algorithmusauswahl von NovaOS zusammenarbeiten.

Ein Execution Contract kann beispielsweise:

```text
Preferred Objective:
    Energy
```

angeben.

NovaOS darf dann einen energieeffizienteren Algorithmus oder Provider auswählen, sofern dessen Semantik und Hard Constraints kompatibel sind.

Scheduler und Algorithmusauswahl bleiben getrennte Mechanismen.

---

## Deterministic Mode

Energy-Aware Scheduling darf deterministische Ausführung nicht unkontrolliert verändern.

Deterministic Mode kann adaptive:

- CPU-Auswahl,
- Frequency Policies,
- Workload Consolidation,
- Migration

einschränken oder festlegen.

---

## Introspection

Energy-Aware Scheduling muss ausreichend introspektierbar sein.

Beispiel:

```text
Task:
    index.background

CPU Class:
    Efficiency

Reason:
    Background + low urgency

Energy Policy:
    Energy Saving

Deadline:
    None
```

Aggregierte Energie- und Schedulingdaten sollen für Systemdiagnose verfügbar sein, soweit die Hardware Messwerte bereitstellt.

---

## Adaptive Optimierung

NovaOS darf lernen, welche Scheduling-Entscheidungen für bestimmte Workloads tatsächlich energieeffizient sind.

Dabei kann gelten:

```text
Predicted Energy / Performance
          ↓
Scheduling Decision
          ↓
Observed Result
          ↓
Prediction Error
          ↓
Model Correction
```

Diese Optimierung darf ausschließlich Soft Policies beeinflussen.

---

## Fail-Safe Defaults

Fehlen zuverlässige Energieinformationen, verwendet NovaOS konservative allgemeine Scheduling Policies.

Unbekannte Energieeigenschaften dürfen nicht als garantierter Effizienzvorteil interpretiert werden.

Hard Execution Constraints besitzen immer Vorrang.

---

## Normative Anforderungen

1. NovaOS MUSS Energy-Aware Scheduling unterstützen können.
2. Energieeffizienz MUSS standardmäßig als Soft Scheduling Objective behandelt werden.
3. Hard Execution Constraints MÜSSEN Vorrang vor Energieoptimierung besitzen.
4. Hardware-Energieinformationen MÜSSEN über HAL beziehungsweise Platform Provider bereitstellbar sein.
5. Der Scheduler MUSS mit unvollständigen Energy Models funktionieren können.
6. Heterogene CPU-Energie- und Performanceklassen MÜSSEN unterstützt werden.
7. CPU-Auswahl SOLL Performance Requirement und Energieeffizienz gemeinsam berücksichtigen.
8. Background Work SOLL auf geeigneten energieeffizienten CPUs bevorzugt werden können.
9. Latenzkritische Workloads MÜSSEN erforderliche Performanceklassen anfordern können.
10. Workload Consolidation MUSS zur Energieeinsparung möglich sein.
11. Workload Spreading MUSS bei Last-, Thermal- oder Performancebedarf weiterhin möglich sein.
12. Scheduler und Idle-State-Management MÜSSEN koordinierbar sein.
13. Wakeup-Latenzen MÜSSEN bei tiefen Idle States berücksichtigt werden können.
14. Scheduler und Frequency-/Performance-State-Management MÜSSEN koordinierbar sein.
15. Hard-Realtime-Contracts DÜRFEN durch Energieoptimierung NICHT abgeschwächt werden.
16. Deadline Slack DARF als Soft Energy-Scheduling-Signal verwendet werden.
17. Background- und Idle-Arbeit MUSS bei Energieknappheit drosselbar oder verschiebbar sein.
18. Work Stealing MUSS Energy-Aware Worker-Wakeup berücksichtigen können.
19. Energy-Aware Scheduling MUSS NUMA- und Cache-Locality berücksichtigen können.
20. Thermal State MUSS als Scheduling-Faktor integrierbar sein.
21. Thermal Safety MUSS Vorrang vor Performance- und Energieoptimierung besitzen.
22. Energie MUSS in die systemweite Resource Economy integrierbar sein.
23. Execution Domains MÜSSEN Energy Budgets besitzen können.
24. Energy-Budget-Verletzungen MÜSSEN kontrollierte Policy-Reaktionen auslösen können.
25. Systemweiter Battery State DARF Soft Scheduling Policies beeinflussen.
26. Energy Pressure MUSS Graceful Degradation optionaler Arbeit ermöglichen.
27. Energy-Aware Scheduling MUSS mit automatischer Algorithmusauswahl koordinierbar sein.
28. Deterministic Mode MUSS adaptive Energy-Scheduling-Entscheidungen einschränken können.
29. Energy-bezogene Scheduling-Entscheidungen MÜSSEN introspektierbar sein.
30. Adaptive Energieoptimierung DARF Hard Constraints NICHT verändern und MUSS ohne KI ersetzbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- geringerer Energieverbrauch,
- bessere Akkulaufzeit,
- sinnvolle Nutzung heterogener CPUs,
- bessere Nutzung tiefer Idle States,
- Integration von Scheduling, Thermal und Resource Economy,
- energieeffizientere Background- und Batch-Verarbeitung.

### Negative Konsequenzen

- Scheduling benötigt zusätzliche Hardwareinformationen,
- Energie und Performance können konkurrierende Ziele sein,
- Hardware-Energiemodelle sind nicht auf allen Plattformen gleich genau,
- häufige Power-State-Wechsel können selbst Kosten verursachen.

---

## Verworfene Alternativen

### Scheduler ignoriert Energie vollständig

Verworfen.

Dies verschenkt insbesondere auf mobilen und heterogenen Systemen erhebliche Effizienzpotenziale.

### Immer den energieeffizientesten Core verwenden

Verworfen.

Ein langsamer Core kann durch längere Laufzeit oder Deadline-Verletzungen insgesamt ungeeignet sein.

### Immer maximale Performance verwenden

Verworfen.

Viele Workloads benötigen die maximal verfügbare CPU-Leistung nicht.

### Energieoptimierung ausschließlich durch CPU-Frequenzsteuerung

Verworfen.

CPU Placement, Konsolidierung, Parallelität, Idle States und Locality beeinflussen den Energieverbrauch ebenfalls.

### Energieoptimierung über Hard Realtime priorisieren

Verworfen.

Verbindliche zeitliche Garantien dürfen nicht zugunsten niedrigerer Leistungsaufnahme still verletzt werden.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-SCHED-0001_Präemptiver_Mehrklassen_Scheduler`
- `ADR-SCHED-0002_Fair_Deadline_und_Realtime_Scheduling_in_einer_Architektur`
- `ADR-SCHED-0003_Topology_Aware_Scheduling`
- `ADR-SCHED-0004_NUMA_Aware_Scheduling`
- `ADR-SCHED-0005_Cache_Aware_Scheduling`
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

- `NPSPEC-SCHED-ENERGY-0001`
- `NPSPEC-SCHED-ENERGY-MODEL-0001`
- `NPSPEC-SCHED-ENERGY-CPU-0001`
- `NPSPEC-SCHED-ENERGY-IDLE-0001`
- `NPSPEC-SCHED-ENERGY-BUDGET-0001`
- `NPSPEC-SCHED-ENERGY-THERMAL-0001`
- `NPSPEC-SCHED-ENERGY-INTROSPECTION-0001`
- `NPSPEC-SCHED-ENERGY-TEST-0001`

---

## Ergebnis

NovaOS integriert Energieeffizienz als kontrolliertes Optimierungsziel in den Scheduler:

```text
Hard Constraints
      ↓
Performance Requirement
      ↓
Eligible CPUs
      ↓
Energy + Locality + Thermal Cost
      ↓
Target CPU
```

Damit wird Energie nicht isoliert optimiert, sondern gemeinsam mit Performance, Locality und Systemzustand betrachtet.

Die zentrale Architekturregel lautet:

```text
Meet the contract first.

Use the least energy necessary.

Consolidate when useful.

Spend performance only where it matters.
```