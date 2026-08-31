# ADR-SCHED-0011 – Predictive und Adaptive Scheduling

## Status

Angenommen

## Kategorie

Kernel / Scheduler / Adaptive Systems / Prediction / Policy Optimization

## Kontext

NovaOS verfügt über eine Scheduler-Architektur, die bereits zahlreiche Laufzeitfaktoren berücksichtigt:

- Scheduling Class,
- Fairness,
- Deadlines,
- Realtime,
- CPU- und Hardwaretopologie,
- NUMA- und Cache-Lokalität,
- Energie,
- Thermal State,
- heterogene CPUs,
- Accelerators,
- QoS und Latenz.

Klassische Scheduler reagieren überwiegend auf den aktuellen Systemzustand. NovaOS soll zusätzlich bekannte Nutzungsmuster und beobachtete Workload-Eigenschaften verwenden können, um zukünftigen Ressourcenbedarf vorherzusagen.

Beispiele sind:

- wahrscheinliche Task-Wakeups,
- erwartete CPU-Last,
- erwartete Laufzeit,
- benötigter Parallelitätsgrad,
- bevorzugte CPU-Klasse,
- Cache- und NUMA-Verhalten,
- Accelerator-Eignung,
- erwartete Latenz,
- Energiebedarf.

Vorhersagen sind jedoch grundsätzlich unsicher.

Eine falsche Prognose darf niemals die Korrektheit, Sicherheit oder Isolation des Systems gefährden.

---

## Entscheidung

NovaOS unterstützt **Predictive und Adaptive Scheduling** als optionale Soft-Policy-Schicht über dem deterministisch korrekten Scheduler-Kern.

```text
Current System State
        +
Execution History
        +
Workload Characteristics
        ↓
Prediction
        ↓
Soft Scheduling Policy
        ↓
Scheduling Decision
        ↓
Actual Result
        ↓
Prediction Error
        ↓
Model Correction
```

Der Scheduler muss vollständig korrekt und funktionsfähig bleiben, wenn:

- keine Vorhersagedaten existieren,
- ein Modell deaktiviert ist,
- eine Prognose falsch ist,
- adaptive Funktionen vollständig abgeschaltet werden.

---

## Grundprinzip

```text
Predict to optimize.

Observe what actually happened.

Learn from prediction error.

Never depend on prediction for correctness.
```

---

## Trennung von Mechanismus und Adaptation

Der Scheduler-Kern stellt weiterhin die grundlegenden Mechanismen bereit:

- Runnable-State-Verwaltung,
- Preemption,
- CPU Placement,
- Affinity,
- Reservations,
- Accounting,
- Deadline- und Realtime-Mechanismen.

Predictive Scheduling beeinflusst ausschließlich zulässige Policy-Entscheidungen.

```text
Scheduler Mechanisms
        ↑
Hard Constraints
        ↑
Base Scheduling Policy
        ↑
Adaptive Soft Policy
```

Adaptive Policy darf keine darunterliegenden Constraints umgehen.

---

## Predictive Scheduling

Predictive Scheduling verwendet bekannte Informationen, um wahrscheinliche zukünftige Zustände abzuschätzen.

Mögliche Prognosen sind:

```text
Expected Runtime
Expected Wakeup
Expected CPU Demand
Expected Parallelism
Expected Memory Locality
Expected Queueing Latency
Expected Energy Cost
Expected Accelerator Benefit
```

Die konkrete Modellierung wird nicht durch diese ADR vorgeschrieben.

---

## Adaptive Scheduling

Adaptive Scheduling verändert Soft Policies anhand beobachteter Ergebnisse.

Beispielsweise kann NovaOS feststellen, dass ein bestimmter Workload regelmäßig:

- kürzer läuft als erwartet,
- stärker parallelisiert,
- von einem Efficiency-Core ausreichend bedient wird,
- auf einer GPU durch Transferkosten langsamer wird,
- von Cache-Affinität besonders profitiert.

Diese Erkenntnisse dürfen zukünftige Optimierungsentscheidungen beeinflussen.

---

## Prediction Error

Prediction Error ist das zentrale Lernsignal.

```text
Prediction
    ↓
Scheduling Decision
    ↓
Actual Result
    ↓
Difference
    ↓
Prediction Error
```

Beispiel:

```text
Predicted Runtime:
    4 ms

Actual Runtime:
    11 ms

Prediction Error:
    +7 ms
```

Wiederholte Abweichungen können zur Anpassung des Modells führen.

---

## Hard und Soft Constraints

Predictive Scheduling darf ausschließlich Entscheidungen innerhalb der bereits zulässigen Lösungsmenge optimieren.

Es gilt:

```text
Hard Constraints
      ↓
Eligible Choices
      ↓
Predictive Optimization
      ↓
Preferred Choice
```

Nicht:

```text
Prediction
      ↓
Override Hard Constraint
```

---

## Scheduling Priority

Adaptive Policies bleiben der allgemeinen NovaOS-Prioritätsordnung untergeordnet.

Insbesondere besitzen Vorrang:

1. Safety,
2. Security und Isolation,
3. Data Sovereignty,
4. Hard Realtime,
5. Deterministic Constraints,
6. Required Resource Constraints,
7. explizite User-/Developer-Vorgaben,
8. Resource Budgets,
9. Performance-, Energie- und adaptive Optimierung.

---

## Workload History

NovaOS darf historische Workload-Eigenschaften verwenden.

Die Historie soll bevorzugt semantisch und aggregiert gespeichert werden, beispielsweise:

```text
Operation Type
Runtime Distribution
CPU Demand
Memory Behaviour
Provider Performance
Energy Characteristics
```

Der Scheduler soll nicht unnötig an konkrete Prozessinstanzen oder personenbezogene Aktivitätsprofile gekoppelt werden.

---

## Confidence

Vorhersagen müssen eine Unsicherheit beziehungsweise Confidence besitzen können.

Beispiel:

```text
Prediction:
    Runtime = 5 ms

Confidence:
    High
```

Bei geringer Confidence soll die adaptive Policy konservativer agieren.

Eine Vorhersage ohne belastbare Confidence darf nicht wie eine Garantie behandelt werden.

---

## Cold Start

Für unbekannte Workloads verwendet NovaOS normale Scheduling Policies.

```text
Unknown Workload
      ↓
Base Scheduler Policy
      ↓
Observe
      ↓
Build History
```

Predictive Scheduling darf keinen trainierten Zustand voraussetzen.

---

## Wakeup Prediction

NovaOS darf zukünftige Wakeups prognostizieren.

Dies kann beispielsweise genutzt werden, um:

- CPU Placement vorzubereiten,
- unnötige Migrationen zu vermeiden,
- Power-State-Entscheidungen zu verbessern.

Eine prognostizierte Aktivität erzeugt jedoch noch keine reale runnable Task.

---

## CPU Placement

Predictive Scheduling darf erwartete Workload-Eigenschaften zur CPU-Auswahl verwenden.

Beispielsweise:

```text
Predicted:
    short latency-sensitive burst

Preferred:
    available performance CPU
```

oder:

```text
Predicted:
    long background workload

Preferred:
    energy-efficient CPU
```

Required Affinity und andere Hard Constraints bleiben bindend.

---

## NUMA und Cache Locality

Adaptive Modelle dürfen lernen, wie stark bestimmte Workloads von:

- NUMA-Lokalität,
- Cache-Affinität,
- Shared-Cache-Platzierung

profitieren.

Diese Informationen dürfen die entsprechenden Soft Placement Costs verbessern.

---

## QoS und Latency

Predictive Scheduling darf erwartete Queueing- und Wakeup-Latenzen verwenden, um QoS-Ziele besser einzuhalten.

Beispielsweise kann ein Task auf eine weniger lokale CPU gelegt werden, wenn die erwartete Gesamtlatenz dort deutlich geringer ist.

Hard Locality Constraints bleiben bindend.

---

## Energy und Thermal

NovaOS darf prognostizieren:

- erwartete CPU-Auslastung,
- Energiebedarf,
- Thermal Pressure.

Damit können Workloads frühzeitig verteilt oder konsolidiert werden.

Thermal Safety darf niemals von einer Prognose abhängen.

---

## Accelerator Selection

Predictive Modelle dürfen die Provider-Auswahl zwischen:

- CPU,
- GPU,
- NPU,
- anderen Accelerators

unterstützen.

Dabei können insbesondere tatsächliche historische:

- Laufzeit,
- Transferkosten,
- Queueing-Latenz,
- Energieverbrauch

berücksichtigt werden.

Die Provider-Auswahl bleibt an den Execution Contract gebunden.

---

## Parallelitätsgrad

NovaOS darf den geeigneten Parallelitätsgrad eines Workloads adaptiv bestimmen.

Mehr Threads beziehungsweise Tasks sind nicht automatisch schneller.

```text
Observed Scaling
      ↓
Adaptive Parallelism
```

Der Scheduler darf daraus Soft-Empfehlungen für geeignete Runtime- oder Task-Group-Mechanismen ableiten.

---

## Work Stealing

Predictive Informationen dürfen Work Stealing unterstützen.

Beispielsweise kann ein Worker Stealing vermeiden, wenn bekannt ist, dass:

- lokale Arbeit unmittelbar erwartet wird,
- eine Migration hohe Locality-Kosten verursacht,
- der gestohlene Task voraussichtlich sehr kurz ist.

Das grundlegende Work-Stealing-Modell bleibt auch ohne Prediction funktionsfähig.

---

## Realtime

Hard-Realtime-Garantien dürfen nicht von Predictive Scheduling abhängen.

Vorhersagen dürfen Realtime-Systeme lediglich bei Soft-Optimierungen unterstützen.

Admission Control muss weiterhin auf garantierbaren beziehungsweise konservativ bestimmten Ressourcen basieren.

```text
Prediction != WCET guarantee
```

---

## Deterministic Mode

Deterministic Mode muss adaptive Scheduling Policies deaktivieren oder vollständig fixieren können.

Damit dürfen:

- Prediction,
- dynamische Modellanpassung,
- adaptive CPU-Auswahl,
- adaptive Migration

keine nicht reproduzierbaren Scheduling-Entscheidungen erzeugen, wenn der Contract dies verbietet.

---

## Exploration

Adaptive Systeme benötigen gelegentlich alternative Entscheidungen, um bessere Policies zu entdecken.

NovaOS darf kontrollierte Exploration verwenden, jedoch ausschließlich für Soft Objectives.

Exploration ist unzulässig, wenn sie:

- Hard Constraints gefährdet,
- Realtime-Garantien beeinflusst,
- Security oder Data Sovereignty verändert,
- explizite Required Policies verletzt.

---

## Stabilität

Adaptive Scheduling darf nicht zu ständig wechselnden Policies führen.

Daher müssen Mechanismen gegen Oszillation möglich sein, beispielsweise:

- Hysterese,
- Mindestbeobachtungszeiträume,
- Confidence-Schwellen,
- begrenzte Änderungsraten.

Die konkreten Algorithmen gehören in die NPSPECs.

---

## Modell-Lebenszyklus

Adaptive Modelle müssen kontrolliert:

- initialisiert,
- aktualisiert,
- versioniert,
- zurückgesetzt,
- deaktiviert

werden können.

Fehlerhafte oder degradierte Modelle dürfen auf einen sicheren Basiszustand zurückgesetzt werden.

---

## Datenschutz und Data Sovereignty

Scheduling-Telemetrie unterliegt den Data-Sovereignty-Regeln von NovaOS.

Workload-Historie darf nicht unbegrenzt oder zweckfremd gesammelt werden.

Persistente Modelle und Telemetriedaten müssen ihrem zulässigen Informationsfluss folgen.

---

## Architecture Introspection

Predictive Scheduling muss nachvollziehbar sein.

Für relevante Entscheidungen soll erkennbar sein:

```text
Base Policy Decision
Prediction
Confidence
Adaptive Adjustment
Final Decision
Observed Result
Prediction Error
```

Damit muss unterscheidbar bleiben, ob eine Entscheidung aus einer Hard Policy oder einer adaptiven Optimierung entstand.

---

## Fail-Safe Verhalten

Fällt die adaptive Komponente aus, gilt:

```text
Adaptive Scheduler unavailable
        ↓
Base Scheduling Policy
        ↓
Normal correct operation
```

Ein fehlerhaftes Modell darf den Scheduler nicht funktionsunfähig machen.

---

## KI als optionale Implementierung

Predictive Scheduling darf statistische Modelle, Heuristiken oder Machine Learning verwenden.

Die Architektur schreibt keine KI-Technik vor.

Zulässig sind beispielsweise:

- gleitende Mittelwerte,
- statistische Modelle,
- heuristische Modelle,
- Online Learning,
- Machine Learning.

Der Scheduler-Kern darf niemals ein KI-Modell für seine Korrektheit benötigen.

---

## Normative Anforderungen

1. NovaOS MUSS Predictive und Adaptive Scheduling als optionale Soft-Policy unterstützen können.
2. Der Scheduler MUSS ohne Predictive Scheduling vollständig korrekt funktionieren.
3. Adaptive Policies DÜRFEN ausschließlich innerhalb bereits zulässiger Scheduling-Entscheidungen optimieren.
4. Hard Constraints DÜRFEN durch Prediction NICHT verändert oder überschrieben werden.
5. Prediction Error MUSS als Feedbacksignal für adaptive Modelle verwendbar sein.
6. Vorhersagen MÜSSEN Unsicherheit beziehungsweise Confidence ausdrücken können.
7. Unbekannte Workloads MÜSSEN über normale Basis-Policies ausführbar sein.
8. Workload History DARF zur Verbesserung zukünftiger Soft Decisions verwendet werden.
9. Wakeup Prediction DARF keine nicht-runnable Task künstlich runnable machen.
10. CPU Placement DARF historische Workload-Eigenschaften berücksichtigen.
11. NUMA- und Cache-Awareness DÜRFEN durch adaptive Kostenmodelle verbessert werden.
12. QoS- und Latency-Aware Scheduling DARF Prediction verwenden.
13. Energy- und Thermal-Aware Scheduling DARF Prediction verwenden.
14. Thermal Safety DARF NICHT von einer Prognose abhängen.
15. Accelerator Selection DARF historische Provider-Ergebnisse berücksichtigen.
16. Adaptive Parallelism MUSS möglich sein, ohne Structured Concurrency zu verletzen.
17. Work Stealing DARF Prediction als Soft Signal verwenden.
18. Hard-Realtime-Admission DARF NICHT von optimistischen Vorhersagen abhängen.
19. Prediction DARF NICHT als WCET-Garantie behandelt werden.
20. Deterministic Mode MUSS adaptive Scheduling-Entscheidungen deaktivieren oder fixieren können.
21. Exploration DARF ausschließlich innerhalb von Soft Objectives erfolgen.
22. Adaptive Policies MÜSSEN gegen instabile Oszillation begrenzbar sein.
23. Adaptive Modelle MÜSSEN deaktivierbar und zurücksetzbar sein.
24. Modellversionen MÜSSEN kontrollierbar sein.
25. Ein Modellfehler MUSS einen Fallback auf die Base Scheduling Policy ermöglichen.
26. Scheduling-Telemetrie MUSS Data-Sovereignty-Regeln einhalten.
27. Adaptive Entscheidungen MÜSSEN introspektierbar sein.
28. Prediction Error MUSS für Diagnose und Modellkorrektur beobachtbar sein.
29. Anwendungen DÜRFEN adaptive Modelle NICHT verwenden, um zusätzliche Scheduling-Authority zu erhalten.
30. Scheduler-Korrektheit, Safety, Security und Isolation MÜSSEN vollständig ohne KI gewährleistet sein.

---

## Konsequenzen

### Positive Konsequenzen

- bessere Scheduling-Entscheidungen für wiederkehrende Workloads,
- bessere CPU-, NUMA- und Cache-Platzierung,
- bessere QoS- und Latenzoptimierung,
- intelligentere Energie- und Thermal-Policies,
- bessere CPU/GPU/NPU-Provider-Auswahl,
- NovaOS kann sich an reale statt nur theoretische Workload-Eigenschaften anpassen.

### Negative Konsequenzen

- zusätzliche Telemetrie und Zustandsverwaltung,
- Vorhersagen können falsch sein,
- adaptive Policies benötigen Schutz gegen Oszillation,
- Persistenz von Workload-Historie erzeugt zusätzliche Data-Sovereignty-Anforderungen.

---

## Verworfene Alternativen

### Ausschließlich reaktives Scheduling

Verworfen.

Es nutzt wiederkehrende und gut beobachtbare Workload-Muster nicht zur Optimierung.

### KI entscheidet direkt über Scheduling Constraints

Verworfen.

Hard Constraints müssen deterministisch und überprüfbar bleiben.

### Prediction als Voraussetzung für Scheduler-Funktion

Verworfen.

NovaOS muss auch ohne Modell, Historie oder KI vollständig funktionieren.

### Unbegrenztes Online Learning

Verworfen.

Unkontrollierte Adaptation kann instabile und schwer reproduzierbare Scheduling-Entscheidungen erzeugen.

### Nur ein globales Scheduling-Modell

Verworfen.

Unterschiedliche Workloads, Hardwaretypen und Policies benötigen unterschiedliche Modelle beziehungsweise Heuristiken.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-SCHED-0001_Präemptiver_Mehrklassen_Scheduler`
- `ADR-SCHED-0003_Topology_Aware_Scheduling`
- `ADR-SCHED-0004_NUMA_Aware_Scheduling`
- `ADR-SCHED-0005_Cache_Aware_Scheduling`
- `ADR-SCHED-0006_Energy_Aware_Scheduling`
- `ADR-SCHED-0007_Thermal_Aware_Scheduling`
- `ADR-SCHED-0008_Heterogeneous_CPU_Scheduling`
- `ADR-SCHED-0009_GPU_NPU_und_Accelerator_Scheduling`
- `ADR-SCHED-0010_QoS_und_Latency_Aware_Scheduling`
- `ADR-CONCURRENCY-0006_Work_Stealing_für_allgemeine_Task_Workloads`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0020_AI_nicht_als_Kernel_Korrektheitsvoraussetzung`

---

## Zugehörige NPSPECs

- `NPSPEC-SCHED-PREDICTIVE-0001`
- `NPSPEC-SCHED-ADAPTIVE-0001`
- `NPSPEC-SCHED-PREDICTION-MODEL-0001`
- `NPSPEC-SCHED-PREDICTION-ERROR-0001`
- `NPSPEC-SCHED-WORKLOAD-HISTORY-0001`
- `NPSPEC-SCHED-ADAPTIVE-POLICY-0001`
- `NPSPEC-SCHED-ADAPTIVE-STABILITY-0001`
- `NPSPEC-SCHED-ADAPTIVE-INTROSPECTION-0001`
- `NPSPEC-SCHED-PREDICTIVE-TEST-0001`

---

## Ergebnis

NovaOS erweitert den Scheduler von einem ausschließlich reaktiven System zu einem lernfähigen, aber weiterhin deterministisch abgesicherten Scheduling-System:

```text
Predict
   ↓
Schedule
   ↓
Observe
   ↓
Compare
   ↓
Prediction Error
   ↓
Adapt
```

Die adaptive Ebene optimiert die Entscheidung.

Der klassische Scheduler-Kern garantiert weiterhin deren Zulässigkeit.

Die zentrale Architekturregel lautet:

```text
Prediction may improve a decision.

Prediction must never authorize it.

Learn from every deviation.

Always keep a correct fallback.
```