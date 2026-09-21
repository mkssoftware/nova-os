# ADR-ADAPTIVE-0004 – Adaptive Scheduling

## Status

Angenommen

## Kategorie

Adaptive Systems / Scheduling

## Kontext

Der NovaOS-Scheduler muss Entscheidungen unter wechselnden Bedingungen treffen.

Relevante Faktoren sind beispielsweise:

```text
Workload
Priorität
Deadlines
Resource Availability
Provider Load
Locality
Energy State
Historical Runtime
```

Statische Scheduling-Regeln können diese Bedingungen berücksichtigen, lernen jedoch nicht aus vergangenen Entscheidungen.

NovaOS soll deshalb Scheduling-Entscheidungen anhand beobachteter Ergebnisse kontinuierlich verbessern können.

## Entscheidung

NovaOS unterstützt **Adaptive Scheduling** auf Basis beobachteter Scheduling-Ergebnisse und Prediction Error.

```text
System State
     ↓
Prediction
     ↓
Scheduling Decision
     ↓
Execution
     ↓
Measured Result
     ↓
Prediction Error
     ↓
Scheduling Model Update
```

Adaptive Scheduling optimiert ausschließlich innerhalb des durch Hard Constraints erlaubten Scheduling-Raums.

## Scheduling Predictions

Der Scheduler darf unter anderem vorhersagen:

```text
Expected Runtime
Queue Time
Resource Demand
Deadline Risk
Provider Availability
Transfer Cost
Energy Cost
Contention
```

Diese Vorhersagen können zur Bewertung gültiger Scheduling-Alternativen verwendet werden.

## Prediction Error

Nach der Ausführung werden Vorhersagen mit tatsächlichen Messwerten verglichen.

Beispiel:

```text
Predicted Runtime: 20 ms
Actual Runtime:    31 ms
        ↓
Prediction Error
        ↓
Scheduling Model Correction
```

Wiederholte Abweichungen sollen das zukünftige Scheduling entsprechend beeinflussen.

## Hard Constraints

Adaptive Scheduling darf niemals verbindliche Anforderungen verändern.

Dazu gehören insbesondere:

```text
Safety
Security
Data Sovereignty
Hard Realtime
Required Determinism
Correctness
Trust Requirements
Resource Limits
Explicit User Constraints
```

Das Modell lautet:

```text
Runnable Work
      ↓
Hard Constraint Filtering
      ↓
Valid Scheduling Space
      ↓
Adaptive Optimization
      ↓
Scheduling Decision
```

## Scheduling-Ziele

Innerhalb des gültigen Lösungsraums darf adaptiv optimiert werden auf:

```text
Latency
Throughput
Fairness
Energy
Locality
Resource Utilization
Queue Time
Provider Utilization
```

Mehrere Ziele dürfen gemeinsam bewertet werden.

## Workload Learning

NovaOS darf wiederkehrende Workload-Muster erkennen.

Beispiele:

```text
Capability
Algorithm
Input Characteristics
Provider
Resource Usage
Execution Duration
Time / Context
```

Dadurch kann der Scheduler ähnliche zukünftige Executions besser einschätzen.

## Provider Learning

Der Scheduler darf aus tatsächlichem Provider-Verhalten lernen.

```text
Workload + Provider
        ↓
Predicted Performance
        ↓
Measured Performance
        ↓
Prediction Error
        ↓
Updated Provider Model
```

Eine theoretisch schnellere Ressource muss dadurch nicht automatisch bevorzugt werden, wenn reale Messwerte ein anderes Verhalten zeigen.

## Resource Learning

Resource Tracing liefert tatsächliche Verbrauchsdaten.

```text
Predicted Resources
        ↓
Scheduling
        ↓
Measured Resources
        ↓
Prediction Error
        ↓
Resource Model Update
```

Dadurch können zukünftige Reservations- und Scheduling-Entscheidungen verbessert werden.

## Deadline Learning

Adaptive Modelle dürfen Deadline-Risiken anhand vergangener Ausführungen besser einschätzen.

```text
Expected Completion
        ↓
Actual Completion
        ↓
Timing Error
        ↓
Deadline Model Update
```

Hard Realtime darf jedoch nicht allein auf statistischen Vorhersagen beruhen, wenn eine garantierte Einhaltung erforderlich ist.

## Distributed Scheduling

Adaptive Scheduling kann auch verteilte Ressourcen berücksichtigen.

Beispiele:

```text
Remote Provider Performance
Network Latency
Transfer Cost
Remote Queue Time
Data Locality
Node Load
```

Sovereignty-, Trust- und Authorization-Constraints bleiben dabei verbindlich.

## Exploration

NovaOS darf alternative Scheduling-Strategien kontrolliert erproben, wenn:

```text
Hard Constraints erfüllt bleiben
Safety nicht beeinträchtigt wird
Realtime nicht gefährdet wird
Resource Budgets eingehalten werden
```

Kritische Workloads dürfen nicht für unkontrollierte Exploration verwendet werden.

## Deterministic Mode

Adaptive Scheduling darf deterministische Ausführungen nicht unkontrolliert verändern.

Im Deterministic Mode muss der adaptive Zustand:

```text
Fixed
Versioned
oder
Ignored
```

werden können.

Gleicher definierter Scheduling-Zustand muss reproduzierbare Entscheidungen ermöglichen.

## Explainability

Adaptive Scheduling wird mit Decision Tracing und Explainable Scheduling verbunden.

NovaOS soll nachvollziehen können:

```text
Welche Vorhersage wurde verwendet?
Welche Scheduling-Alternativen bestanden?
Welche adaptive Bewertung beeinflusste die Entscheidung?
Was trat tatsächlich ein?
Wie groß war der Prediction Error?
```

## Modellalterung

Scheduling-Erfahrungen können durch Systemänderungen ungültig werden.

Beispiele:

```text
Hardware Change
Provider Update
Algorithm Update
Scheduler Update
Driver Update
Network Change
Workload Change
```

Adaptive Modelle müssen solche Änderungen berücksichtigen und veraltete Informationen abwerten oder invalidieren können.

## Cold Start

Ohne ausreichende historische Daten verwendet NovaOS:

```text
Static Scheduling Policy
Declared Resource Properties
Hardware Models
Safe Defaults
```

Adaptive Scheduling ist eine Optimierung und keine Voraussetzung für korrektes Scheduling.

## Normative Anforderungen

1. NovaOS SOLL Adaptive Scheduling unterstützen.
2. Adaptive Scheduling MUSS Prediction Error als Lernsignal verwenden können.
3. Scheduling Predictions MÜSSEN mit tatsächlichen Ergebnissen korrelierbar sein.
4. Wiederholte relevante Abweichungen SOLLEN zukünftige Scheduling-Entscheidungen beeinflussen.
5. Adaptive Scheduling DARF ausschließlich innerhalb des durch Hard Constraints erlaubten Lösungsraums optimieren.
6. Hard Constraints DÜRFEN durch adaptive Modelle nicht verändert oder abgeschwächt werden.
7. Resource-, Runtime- und Provider-Messwerte DÜRFEN zur Modellverbesserung verwendet werden.
8. Hard-Realtime-Garantien DÜRFEN nicht allein aus statistischen Vorhersagen abgeleitet werden.
9. Distributed Scheduling MUSS Sovereignty-, Trust- und Authorization-Constraints weiterhin vollständig einhalten.
10. Exploration DARF kritische oder garantierte Workloads nicht gefährden.
11. Deterministic Mode MUSS adaptive Scheduling-Zustände fixieren, versionieren oder ignorieren können.
12. Adaptive Scheduling MUSS mit Decision Tracing korrelierbar sein.
13. Relevante adaptive Scheduling-Entscheidungen MÜSSEN erklärbar sein.
14. Veraltete Modelle MÜSSEN invalidiert oder in ihrer Gewichtung reduziert werden können.
15. Ohne adaptives Modell MUSS korrektes Scheduling weiterhin möglich sein.

## Konsequenzen

### Positive Konsequenzen

- Scheduling verbessert sich anhand realer Systemerfahrung,
- Laufzeit- und Ressourcenprognosen werden genauer,
- Provider können workloadabhängig besser ausgewählt werden,
- wiederkehrende Fehlentscheidungen können automatisch korrigiert werden,
- verteilte Ressourcen können realistischer bewertet werden.

### Negative Konsequenzen

- Scheduling erhält zusätzlichen Modellzustand,
- historische Daten können durch Systemänderungen veralten,
- Exploration und Online-Learning benötigen klare Grenzen.

## Abhängigkeiten

- `ADR-ADAPTIVE-0001_Prediction_Error_als_zentrales_Lernsignal`
- `ADR-ADAPTIVE-0003_Explizite_Nutzerentscheidung_hat_Vorrang_vor_Adaptation`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-ALGORITHM-0006_Profile_Guided_Algorithm_Selection`
- `ADR-DISTRIBUTED-0003_Distributed_Scheduling`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-OBSERVABILITY-0006_Explainable_Scheduling`
- `ADR-OBSERVABILITY-0007_Explainable_Resource_Allocation`

## Zugehörige NPSPECs

- `NPSPEC-ADAPTIVE-SCHEDULING-0001`
- `NPSPEC-ADAPTIVE-SCHEDULING-PREDICTION-0001`
- `NPSPEC-ADAPTIVE-SCHEDULING-MODEL-0001`
- `NPSPEC-ADAPTIVE-SCHEDULING-EXPLORATION-0001`
- `NPSPEC-ADAPTIVE-SCHEDULING-FEEDBACK-0001`

## Ergebnis

NovaOS erweitert Scheduling um einen kontrollierten Lernzyklus:

```text
Observe
   ↓
Predict
   ↓
Schedule
   ↓
Execute
   ↓
Measure
   ↓
Prediction Error
   ↓
Adapt
```

Der zentrale Grundsatz lautet:

```text
Der Scheduler darf aus Erfahrung lernen.

Er darf dadurch bessere Entscheidungen treffen,
aber niemals die Regeln verändern,
innerhalb derer er entscheiden darf.

Adaptation optimiert Scheduling –
sie ersetzt keine Contracts.
```