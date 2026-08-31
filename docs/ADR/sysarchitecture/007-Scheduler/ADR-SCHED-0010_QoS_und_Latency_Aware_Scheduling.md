# ADR-SCHED-0010 – QoS und Latency-Aware Scheduling

## Status

Angenommen

## Kategorie

Kernel / Scheduler / QoS / Latenz / Interaktivität / Execution Contracts

## Kontext

NovaOS unterstützt unterschiedliche Scheduling-Klassen, Deadlines, Realtime-Domänen, heterogene CPUs und Accelerators. Nicht jeder Workload benötigt jedoch eine harte Deadline oder Realtime-Garantie.

Viele Workloads besitzen stattdessen Anforderungen an die wahrgenommene oder statistische Latenz, beispielsweise:

- UI-Interaktionen,
- Audio-/Video-Pipelines,
- Eingabeverarbeitung,
- Kommunikationsdienste,
- interaktive Anwendungen,
- Datenbankanfragen,
- KI-Inferenz,
- Systemdienste.

Ein Task mit niedriger durchschnittlicher Laufzeit kann dennoch schlechte Benutzererfahrung verursachen, wenn Queueing, Wakeup oder Scheduling sporadisch hohe Latenzen erzeugen.

NovaOS benötigt deshalb ein explizites Modell für **Quality of Service (QoS)** und **Latency Awareness**, ohne jede latenzkritische Arbeit als Realtime-Workload behandeln zu müssen.

---

## Entscheidung

NovaOS integriert QoS- und Latenzanforderungen als explizite Eigenschaften von Execution Contracts und Scheduling Policies.

```text
Execution Contract
      ↓
Hard Constraints
      ↓
QoS / Latency Requirements
      ↓
Scheduling Class
      ↓
Eligible Resources
      ↓
Placement + Dispatch Policy
```

QoS beschreibt gewünschte Dienstqualität. Latency Awareness verwendet diese Anforderungen zur Optimierung von Dispatch, Wakeup, Placement und Ressourcenverteilung.

QoS erzeugt keine implizite Hard-Realtime-Garantie.

---

## Grundprinzip

```text
Optimize for the latency
the workload actually needs.

Do not confuse low latency
with hard realtime.

Protect responsiveness
without destroying fairness.
```

---

## QoS-Modell

Ein Execution Contract kann QoS-Eigenschaften enthalten.

Dazu können insbesondere gehören:

```text
Latency Target
Latency Class
Throughput Requirement
Jitter Sensitivity
Importance
Degradation Policy
```

Nicht jeder Workload muss alle Eigenschaften definieren.

Fehlende Angaben werden durch die zuständige Systempolicy ergänzt.

---

## Latency Classes

NovaOS unterstützt semantische Latenzklassen.

Beispielsweise:

```text
Critical Interactive
Interactive
Responsive
Normal
Throughput
Background
```

Diese Klassen sind keine festen numerischen Prioritäten.

Sie dienen als Policy-Eingabe für den Scheduler.

---

## Latency Target

Workloads können ein gewünschtes Latenzziel definieren.

Beispiel:

```text
LatencyTarget = 10 ms
```

Ein solches Ziel ist standardmäßig ein QoS-Ziel und keine Garantie.

Für garantierte zeitliche Grenzen müssen Deadline- beziehungsweise Realtime-Contracts verwendet werden.

---

## Latenzkomponenten

Latency-Aware Scheduling betrachtet nicht ausschließlich CPU-Ausführungszeit.

Relevante Komponenten können sein:

```text
Wakeup Latency
Queueing Latency
Dispatch Latency
Execution Latency
Preemption Delay
I/O Completion Delay
Accelerator Queueing
```

Der Scheduler ist nur für die von ihm kontrollierbaren beziehungsweise beeinflussbaren Komponenten verantwortlich.

---

## Interaktive Workloads

Interaktive Tasks sollen nach:

```text
Input
Event
Completion
Wakeup
```

mit geringer Scheduling-Verzögerung fortgesetzt werden können.

Kurzfristige Latenzbevorzugung darf verwendet werden, ohne dem Workload dauerhaft unbegrenzte CPU-Priorität zu geben.

---

## Wakeup Placement

Beim Wakeup soll der Scheduler zwischen:

- sofort verfügbarer CPU,
- Warm-Cache-Lokalität,
- NUMA-Lokalität,
- CPU-Klasse,
- erwarteter Queueing-Latenz

abwägen.

Eine etwas weiter entfernte CPU kann für einen stark latenzkritischen Task sinnvoll sein, wenn dadurch erhebliche Queueing-Latenz vermieden wird.

Hard Locality Constraints bleiben bindend.

---

## Tail Latency

NovaOS soll nicht ausschließlich durchschnittliche Latenz optimieren.

Für geeignete QoS-Workloads müssen auch hohe Perzentile beziehungsweise Tail-Latency-Effekte berücksichtigt werden können.

Beispielsweise:

```text
Average Latency: acceptable
P99 Latency: poor
```

Eine gute Durchschnittslatenz darf nicht automatisch als gute QoS interpretiert werden.

---

## Jitter

Bestimmte Workloads benötigen nicht die minimal mögliche Latenz, sondern möglichst geringe Schwankungen.

Dies betrifft beispielsweise:

- Audio,
- Video,
- Streaming,
- periodische Verarbeitung.

Execution Contracts können daher Jitter Sensitivity ausdrücken.

---

## QoS und Deadline

QoS Target und Deadline bleiben getrennte Konzepte.

```text
QoS:
    desired service quality

Deadline:
    temporal completion boundary

Realtime:
    admitted temporal guarantee
```

Ein Workload kann gleichzeitig QoS- und Deadline-Anforderungen besitzen.

---

## QoS und Scheduling Class

QoS ersetzt die Scheduling-Klassen nicht.

Die Scheduling Class definiert den grundlegenden Scheduling-Charakter; QoS liefert zusätzliche Anforderungen innerhalb dieses Modells.

Beispielsweise kann ein Interactive Task ein aggressiveres Latency Target besitzen als ein anderer Interactive Task.

---

## Fairness

Latency-Aware Scheduling darf Fairness kurzfristig zugunsten von Responsiveness verschieben.

Langfristig darf ein normaler QoS-Workload jedoch nicht unbegrenzt CPU-Kapazität anderer Workloads verdrängen.

Hard Reservations und Realtime Contracts bleiben davon getrennt.

---

## Priority Inversion

QoS muss über Synchronisationsgrenzen berücksichtigt werden können.

Blockiert ein latenzkritischer Task auf einer Ressource, die von weniger dringlicher Arbeit gehalten wird, müssen vorhandene Priority-Inversion-Mechanismen die relevante Dringlichkeit temporär propagieren können.

QoS darf dabei keine zusätzliche Authority übertragen.

---

## Structured Concurrency

QoS kann entlang der Structured-Concurrency-Hierarchie propagiert werden.

Ein Child Task darf die erforderliche Dienstqualität seines Parents übernehmen, wenn dies für dessen Completion notwendig ist.

Eine Child Operation darf Hard Resource Limits des Parents dadurch nicht überschreiten.

---

## I/O

Latency-Aware Scheduling muss mit I/O-Scheduling koordinierbar sein.

Ein CPU-Scheduler allein kann keine End-to-End-Latenz optimieren, wenn I/O-Queues die dominierende Verzögerung verursachen.

QoS-Metadaten müssen deshalb über geeignete Systemgrenzen propagierbar sein.

---

## Accelerator Scheduling

GPU-, NPU- und Accelerator-Queues können Teil der End-to-End-Latenz sein.

Provider Selection darf daher berücksichtigen:

```text
Execution Time
+
Transfer Time
+
Queueing Time
=
Expected Completion Latency
```

Der theoretisch schnellste Provider ist nicht zwangsläufig der Provider mit der niedrigsten End-to-End-Latenz.

---

## Heterogene CPUs

Latency-sensitive Workloads dürfen leistungsfähigere CPU-Klassen bevorzugen.

Dabei müssen weiterhin:

- Energy Cost,
- Thermal State,
- Locality,
- Resource Constraints

berücksichtigt werden.

QoS allein erzeugt keinen Anspruch auf einen bestimmten Core.

---

## Energy und Latency

Energy-Aware und Latency-Aware Scheduling können konkurrieren.

Bei ausreichend großem Latency Slack kann eine energieeffizientere Ausführung gewählt werden.

Bei geringem Slack kann höhere Performance sinnvoll sein.

```text
Large Latency Margin
    → Efficiency

Small Latency Margin
    → Performance
```

---

## Thermal Pressure

Thermal Pressure kann verfügbare Low-Latency-Kapazität reduzieren.

NovaOS darf Hardware-Safety nicht zugunsten eines QoS-Ziels verletzen.

Kann ein QoS Target nicht mehr erfüllt werden, muss die definierte Degradationspolicy greifen.

---

## Resource Economy

QoS ist in die Resource Economy integriert.

Ein Workload darf nicht allein durch die Deklaration hoher QoS-Anforderungen unbegrenzt bevorzugte Ressourcen erhalten.

QoS kann abhängig sein von:

- Resource Budget,
- Berechtigungen,
- Systempolicy,
- aktueller Kapazität.

Explizite Reservations bleiben das Mittel für garantierte Kapazität.

---

## Overload

Bei Überlast muss NovaOS QoS kontrolliert priorisieren.

Konzeptionell:

```text
Protect admitted hard contracts
        ↓
Protect critical system responsiveness
        ↓
Preserve interactive QoS
        ↓
Reduce optional throughput
        ↓
Throttle background work
```

Die konkrete Gewichtung wird durch Scheduler- und Resource-Policies definiert.

---

## Graceful Degradation

Kann ein QoS-Ziel nicht gehalten werden, muss ein Workload kontrolliert degradieren können.

Mögliche Reaktionen sind:

```text
Reduce Quality
Reduce Parallelism
Use Alternative Algorithm
Use Alternative Provider
Defer Optional Work
Report QoS Violation
```

Die zulässigen Reaktionen werden durch den Execution Contract begrenzt.

---

## QoS Violation

Das Verfehlen eines Soft QoS Targets ist kein Kernelfehler.

NovaOS muss solche Verletzungen jedoch messen und introspektierbar machen können.

Wiederholte Verletzungen können Scheduling- oder Resource-Policies beeinflussen.

---

## Deterministic Mode

Adaptive QoS-Optimierungen müssen für deterministische Workloads einschränkbar sein.

Deterministic Mode kann insbesondere:

- CPU Placement,
- Dispatch Order,
- Migration,
- Provider Selection

festlegen.

QoS bleibt dabei beschreibbar, darf aber deterministische Hard Constraints nicht überschreiben.

---

## Introspection

QoS- und Latenzentscheidungen müssen introspektierbar sein.

Beispiel:

```text
Task:
    ui.input.handler

QoS:
    Critical Interactive

Latency Target:
    8 ms

Observed:
    2.4 ms

CPU:
    Performance Class

Placement Reason:
    low queueing latency
```

Systemweite Werkzeuge sollen insbesondere QoS-Verletzungen und deren Ursachen analysieren können.

---

## Adaptive Optimierung

NovaOS darf Latenzverhalten beobachten und Soft Policies anpassen.

```text
Predicted Latency
       ↓
Scheduling Decision
       ↓
Observed Latency
       ↓
Prediction Error
       ↓
Policy Adjustment
```

Adaptive Modelle können beispielsweise:

- Wakeup Placement,
- CPU-Class Preference,
- Provider Selection,
- Queueing-Schätzungen

verbessern.

Sie dürfen keine Hard Contracts verändern.

---

## Fail-Safe Defaults

Fehlen QoS-Informationen, verwendet NovaOS die normale Scheduling Policy der jeweiligen Klasse.

Eine Anwendung darf durch falsche oder aggressive QoS-Deklaration keine:

- Security-Grenzen,
- Resource Budgets,
- Realtime Reservations,
- System-Safety-Regeln

umgehen.

Systemkorrektheit darf nicht von QoS-Prognosen abhängen.

---

## Normative Anforderungen

1. NovaOS MUSS QoS- und Latency-Aware Scheduling unterstützen.
2. QoS-Anforderungen MÜSSEN über Execution Contracts ausdrückbar sein.
3. QoS Targets DÜRFEN NICHT automatisch als Hard-Realtime-Garantien interpretiert werden.
4. Latency Classes MÜSSEN semantisch und unabhängig von universellen numerischen Prioritäten darstellbar sein.
5. Workloads MÜSSEN Latency Targets ausdrücken können.
6. Jitter Sensitivity MUSS ausdrückbar sein.
7. Der Scheduler MUSS Wakeup- und Queueing-Latenz berücksichtigen können.
8. Interaktive Tasks DÜRFEN kurzfristig bevorzugt werden.
9. Kurzfristige QoS-Bevorzugung DARF NICHT automatisch unbegrenzte langfristige CPU-Priorität erzeugen.
10. Wakeup Placement MUSS Locality und erwartete Latenz gegeneinander abwägen können.
11. Tail Latency MUSS für geeignete QoS Policies mess- und optimierbar sein.
12. QoS, Deadline und Realtime MÜSSEN getrennte Semantiken behalten.
13. QoS MUSS mit Scheduling Classes kombinierbar sein.
14. Fair Scheduling MUSS trotz Soft QoS langfristig erhalten bleiben.
15. QoS MUSS mit Priority-Inversion-Mechanismen integrierbar sein.
16. QoS-Propagation DARF keine zusätzliche Authority erzeugen.
17. QoS MUSS über Structured-Concurrency-Abhängigkeiten propagierbar sein.
18. QoS-Metadaten MÜSSEN für End-to-End-Optimierung an relevante I/O-Subsysteme propagierbar sein.
19. Accelerator Scheduling MUSS End-to-End-Latenz einschließlich Queueing und Transfers berücksichtigen können.
20. Heterogeneous CPU Scheduling MUSS Latency Requirements berücksichtigen können.
21. Energy- und Latency-Aware Scheduling MÜSSEN gemeinsam optimierbar sein.
22. Thermal Safety MUSS Vorrang vor Soft QoS Targets besitzen.
23. QoS MUSS in die systemweite Resource Economy integrierbar sein.
24. QoS-Deklarationen DÜRFEN Resource Budgets und Reservations NICHT umgehen.
25. Overload Policies MÜSSEN QoS kontrolliert degradieren können.
26. QoS-Verletzungen MÜSSEN messbar und introspektierbar sein.
27. Graceful Degradation MUSS für nicht erfüllbare QoS Targets unterstützt werden.
28. Deterministic Mode MUSS adaptive QoS-Optimierung einschränken können.
29. QoS-bezogene Scheduling- und Placement-Entscheidungen MÜSSEN introspektierbar sein.
30. Adaptive Latency-Optimierung DARF Hard Constraints NICHT verändern und Systemkorrektheit MUSS ohne KI gewährleistet sein.

---

## Konsequenzen

### Positive Konsequenzen

- bessere wahrgenommene Systemreaktion,
- explizite Latenzanforderungen ohne Missbrauch von Realtime,
- bessere Tail-Latency-Kontrolle,
- gemeinsame QoS-Semantik über CPU, I/O und Accelerators,
- bessere Balance zwischen Responsiveness, Fairness, Energie und Durchsatz,
- QoS-Verletzungen werden systemweit analysierbar.

### Negative Konsequenzen

- zusätzliche Scheduling-Metriken und Policies,
- End-to-End-Latenz erfordert Kooperation mehrerer Subsysteme,
- aggressive Latenzoptimierung kann Fairness und Energieeffizienz beeinträchtigen,
- Soft QoS kann unter Überlast nicht garantiert werden.

---

## Verworfene Alternativen

### Alle latenzkritischen Workloads als Realtime behandeln

Verworfen.

Die meisten interaktiven Workloads benötigen niedrige Latenz, aber keine harte zeitliche Garantie.

### QoS ausschließlich über numerische Prioritäten abbilden

Verworfen.

Priorität beschreibt weder Latency Target noch Jitter, Tail Latency oder End-to-End-QoS ausreichend.

### Nur durchschnittliche Latenz optimieren

Verworfen.

Hohe Tail Latency kann trotz guter Durchschnittswerte zu schlechter Responsiveness führen.

### Anwendungen dürfen beliebig maximale QoS verlangen

Verworfen.

QoS muss Resource Budgets, Systempolicy und verfügbare Kapazität respektieren.

### QoS nur im CPU-Scheduler behandeln

Verworfen.

End-to-End-Latenz kann durch I/O-, Accelerator- und Transfer-Queues dominiert werden.

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
- `ADR-SCHED-0008_Heterogeneous_CPU_Scheduling`
- `ADR-SCHED-0009_GPU_NPU_und_Accelerator_Scheduling`
- `ADR-CONCURRENCY-0001_Structured_Concurrency`
- `ADR-CONCURRENCY-0004_Deadline_Propagation`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`

---

## Zugehörige NPSPECs

- `NPSPEC-SCHED-QOS-0001`
- `NPSPEC-SCHED-LATENCY-0001`
- `NPSPEC-SCHED-LATENCY-CLASS-0001`
- `NPSPEC-SCHED-WAKEUP-LATENCY-0001`
- `NPSPEC-SCHED-TAIL-LATENCY-0001`
- `NPSPEC-SCHED-JITTER-0001`
- `NPSPEC-SCHED-QOS-PROPAGATION-0001`
- `NPSPEC-SCHED-QOS-OVERLOAD-0001`
- `NPSPEC-SCHED-QOS-INTROSPECTION-0001`
- `NPSPEC-SCHED-QOS-TEST-0001`

---

## Ergebnis

NovaOS behandelt niedrige Latenz als explizites Scheduling-Ziel, ohne daraus automatisch Realtime zu machen:

```text
Execution Contract
        ↓
QoS + Latency Target
        ↓
Scheduling Policy
        ↓
CPU / I/O / Accelerator Decisions
        ↓
Observed End-to-End Latency
```

Damit kann NovaOS interaktive und latenzsensitive Workloads gezielt bevorzugen, während Fairness, Ressourcenbudgets und harte Systemgarantien erhalten bleiben.

Die zentrale Architekturregel lautet:

```text
Low latency is a requirement.

Realtime is a guarantee.

Keep them distinct.

Optimize the whole execution path.
```