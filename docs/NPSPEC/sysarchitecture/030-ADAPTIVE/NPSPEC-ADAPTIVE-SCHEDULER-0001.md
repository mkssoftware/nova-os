# NPSPEC-ADAPTIVE-SCHEDULER-0001 – Nova Adaptive Scheduler

## Status

Angenommen

## Kategorie

Adaptive System / Scheduling / Runtime Optimization

## Zweck

NovaOS definiert eine adaptive Erweiterung des Schedulers, die beobachtete Systemzustände, Vorhersagen und Feedback nutzt, um Scheduling-Entscheidungen innerhalb bestehender Scheduling-, Sicherheits- und Echtzeitregeln zu optimieren.

```text
System State
     ↓
Prediction
     ↓
Adaptive Scheduling
     ↓
Execution
     ↓
Feedback
     ↺
```

Der Adaptive Scheduler ersetzt nicht den regulären Scheduler, sondern liefert ihm kontrollierte Optimierungsinformationen.

## Grundprinzipien

```text
Adaptive Scheduler ≠ Base Scheduler
Prediction ≠ Scheduling Decision
Optimization ≠ Guarantee
Learned Policy ≠ Scheduling Authority
High Probability ≠ Certainty
Adaptive Priority ≠ Security Authority
```

Der reguläre Scheduler muss auch ohne adaptive Komponenten vollständig funktionsfähig bleiben.

## Architektur

```text
Tasks
  ↓
Base Scheduler
  ↑
Adaptive Scheduler
  ↑
Metrics + Prediction + Feedback
```

Der Base Scheduler erzwingt weiterhin:

```text
Safety
Security
Capabilities
Realtime Constraints
Deadlines
Reservations
Guarantees
Fairness
Hard Resource Constraints
```

## Adaptive Inputs

Der Adaptive Scheduler kann verwenden:

```text
CPU Load
Run Queue State
Memory Pressure
Cache Locality
NUMA Locality
Energy State
Thermal State
Task History
Execution History
Deadline History
Prediction
Prediction Error
Feedback
```

Die Qualität und Aktualität dieser Daten muss berücksichtigt werden.

## Scheduling Prediction

NovaOS kann zukünftige Scheduling-Anforderungen vorhersagen.

Beispiele:

```text
Expected Task Wakeup
Expected CPU Demand
Expected Accelerator Demand
Expected Memory Pressure
Expected IO Completion
Expected Thermal Pressure
```

Vorhersagen bleiben Soft Inputs.

## Adaptive Decisions

Optimiert werden können beispielsweise:

```text
CPU Selection
Core Selection
Task Placement
Migration Timing
Load Balancing
Work Stealing
Wakeup Placement
NUMA Placement
Cache Locality
Energy Preference
Thermal Distribution
```

## Constraint Hierarchy

Adaptive Scheduling folgt:

```text
Safety
  ↓
Security
  ↓
Realtime / Deadline Guarantees
  ↓
Hard Resource Constraints
  ↓
Explicit Scheduling Policy
  ↓
Fairness
  ↓
Adaptive Optimization
```

Adaptive Optimierung darf keine höhere Ebene überschreiben.

## Predictive Scheduling

Der Scheduler kann Ressourcen vorbereiten, bevor eine Last tatsächlich entsteht.

```text
Predicted Workload
       ↓
Prepare Scheduling State
       ↓
Task Arrival
       ↓
Reduced Scheduling Latency
```

Beispiele:

```text
Core Wakeup
Frequency Preparation
Cache Locality Preparation
NUMA Preparation
Accelerator Preparation
```

## Load Balancing

Adaptive Vorhersagen können Load Balancing unterstützen.

```text
Current Load
     +
Predicted Load
     ↓
Placement Adjustment
```

Migration darf nur erfolgen, wenn ihr erwarteter Nutzen ihre Kosten rechtfertigt.

## Locality

Der Adaptive Scheduler kann berücksichtigen:

```text
CPU Cache
NUMA Memory
Shared Data
Device Locality
Accelerator Locality
```

Ziel ist die Reduzierung unnötiger Datenbewegungen.

## Energy und Thermal

Bei ausreichendem Scheduling-Spielraum können berücksichtigt werden:

```text
Energy Efficiency
Core Frequency
Idle States
Battery State
Thermal Pressure
```

Deadline-, Realtime- und Sicherheitsanforderungen besitzen Vorrang.

## Feedback

Scheduling-Ergebnisse werden beobachtet:

```text
Scheduling Decision
        ↓
Execution
        ↓
Latency / Throughput / Energy
        ↓
Feedback
```

Damit können adaptive Heuristiken bewertet werden.

## Prediction Error

Fehlvorhersagen werden explizit berücksichtigt.

```text
Prediction
    ↓
Scheduling Decision
    ↓
Actual Result
    ↓
Prediction Error
```

Häufige Fehlvorhersagen können den Einfluss eines Predictors reduzieren.

## Policy Learning

Adaptive Scheduling-Parameter können kontrolliert gelernt werden.

Beispiele:

```text
Migration Threshold
Load Balance Threshold
Prediction Weight
Energy Preference
Locality Weight
Wakeup Strategy
```

Hard Scheduling Policies dürfen nicht gelernt oder überschrieben werden.

## Stability

Adaptive Scheduling muss Oszillation verhindern.

Beispiel:

```text
CPU A → CPU B → CPU A → CPU B
```

Mechanismen können sein:

```text
Hysteresis
Cooldown
Migration Cost
Minimum Residency
Bounded Adjustment
Confidence Threshold
```

## Realtime

Hard-Realtime-Tasks dürfen nicht von unsicheren Predictions abhängig sein.

```text
Hard Realtime
     ↓
Deterministic Scheduling

Adaptive Optimization
     ↓
only remaining freedom
```

Adaptive Mechanismen dürfen nur innerhalb des zulässigen Scheduling-Spielraums arbeiten.

## Determinismus

Deterministische Executions müssen adaptive Einflüsse begrenzen oder fixieren können.

Möglich sind:

```text
Fixed Scheduling Policy
Fixed Adaptive Policy Version
Recorded Decisions
Disabled Adaptation
```

## Safe Fallback

Bei:

```text
Missing Metrics
Low Prediction Confidence
Prediction Failure
Policy Failure
Observability Failure
Adaptive Instability
```

muss automatisch der reguläre Scheduler übernehmen können.

```text
Adaptive Scheduler
      ↓ unavailable
Base Scheduler
```

## Security

Adaptive Daten dürfen keine Scheduling-Authority erzeugen.

```text
Prediction ≠ Permission
Feedback ≠ Priority Right
Learned Policy ≠ Capability
```

Security Domains und Capability-Grenzen bleiben unverändert.

## Resource Economy

Adaptive Scheduling besitzt ein eigenes begrenztes Budget.

```text
CPU
Memory
Prediction Cost
Decision Cost
History
Migration Cost
Energy
```

Die Optimierung darf nicht mehr Ressourcen verbrauchen als sie voraussichtlich einspart oder an Systemqualität gewinnt.

## Observability

Autorisierte Komponenten sollen beobachten können:

```text
Scheduling Decision
Prediction Inputs
Policy Version
Selected CPU
Migration
Reason Codes
Prediction Confidence
Prediction Error
Observed Outcome
Adaptive Benefit
Fallback State
```

Decision Observability soll erklären können, warum eine adaptive Scheduling-Entscheidung getroffen wurde.

## Normative Anforderungen

1. NovaOS MUSS Adaptive Scheduling vom grundlegenden Scheduler trennen.
2. Der Base Scheduler MUSS ohne adaptive Komponenten vollständig funktionieren.
3. Adaptive Scheduling DARF Hard Constraints NICHT überschreiben.
4. Realtime- und Deadline-Garantien MÜSSEN Vorrang vor adaptiver Optimierung besitzen.
5. Predictions DÜRFEN ausschließlich als kontrollierte Scheduling-Eingaben verwendet werden.
6. Prediction Confidence und Freshness SOLLEN berücksichtigt werden.
7. Fehlvorhersagen DÜRFEN die Systemkorrektheit NICHT gefährden.
8. CPU-, NUMA-, Cache-, Energy- und Thermal-Informationen SOLLEN berücksichtigt werden können.
9. Migration MUSS ihre eigenen Kosten berücksichtigen.
10. Adaptive Scheduling MUSS gegen Oszillation begrenzbar sein.
11. Scheduling-Ergebnisse SOLLEN Feedback erzeugen können.
12. Prediction Errors SOLLEN den zukünftigen Einfluss eines Predictors beeinflussen können.
13. Adaptive Scheduling-Parameter DÜRFEN kontrolliert gelernt werden.
14. Hard Scheduling Policies DÜRFEN NICHT durch Policy Learning ersetzt werden.
15. Deterministische Executions MÜSSEN adaptive Einflüsse kontrollieren können.
16. Bei Ausfall adaptiver Komponenten MUSS ein sicherer Fallback auf den Base Scheduler möglich sein.
17. Adaptive Scheduling DARF keine zusätzliche Authority erzeugen.
18. Adaptive Scheduling MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ADAPTIVE-PREDICTION-0001`
- `NPSPEC-ADAPTIVE-PREDICTIONERROR-0001`
- `NPSPEC-ADAPTIVE-FEEDBACK-0001`
- `NPSPEC-ADAPTIVE-POLICYLEARNING-0001`
- `NPSPEC-OBSERVABILITY-METRICS-0001`
- `NPSPEC-OBSERVABILITY-RESOURCE-0001`
- `NPSPEC-OBSERVABILITY-DECISION-0001`
- `NPSPEC-OBSERVABILITY-INTROSPECTION-0001`
- `NPSPEC-RESOURCE-CPU-0001`
- `NPSPEC-RESOURCE-MEMORY-0001`
- `NPSPEC-RESOURCE-ENERGY-0001`
- `NPSPEC-RESOURCE-THERMAL-0001`
- `NPSPEC-RESOURCE-DEADLINE-0001`
- `NPSPEC-EXECUTION-DEADLINE-0001`
- `NPSPEC-EXECUTION-DETERMINISM-0001`
- `ADR-ARCH-0084`

## Ergebnis

```text
Base Scheduler
      +
Observation
      +
Prediction
      +
Adaptive Policy
      ↓
Constraint-Safe Scheduling
      ↓
Execution
      ↓
Feedback
      ↓
Continuous Optimization
```

NovaOS erhält damit einen adaptiven Scheduler, der aus realem Systemverhalten lernen und zukünftige Lasten berücksichtigen kann, während der deterministische und sichere Base Scheduler jederzeit die verbindliche Grundlage der Ausführung bleibt.