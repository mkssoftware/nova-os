# ADR-AUTONOMY-0006 – Self-Optimization

## Status

Angenommen

## Kategorie

Autonomy / Optimization / System Control

## Kontext

NovaOS soll seine Ressourcennutzung und Ausführungsstrategien kontinuierlich verbessern können.

Optimierbare Bereiche sind beispielsweise:

```text
Scheduling
Algorithm Selection
Provider Selection
Resource Allocation
Memory
Storage
Networking
Caching
Preloading
Power Management
Data Locality
```

Self-Optimization darf jedoch weder Korrektheit noch explizite Policies und Hard Constraints verändern.

## Entscheidung

NovaOS unterstützt **Self-Optimization** als kontinuierlichen, messbaren und constraint-gesteuerten Optimierungsprozess.

```text
Observe
   ↓
Measure
   ↓
Predict
   ↓
Optimize
   ↓
Execute
   ↓
Measure Result
   ↓
Prediction Error
   ↓
Adapt
```

Optimiert wird ausschließlich innerhalb des gültigen Entscheidungsraums.

## Optimierungsziele

Self-Optimization darf mehrere Ziele berücksichtigen:

```text
Latency
Throughput
Resource Efficiency
Energy
Locality
Memory Usage
Network Traffic
Storage I/O
Responsiveness
```

Die Gewichtung kann durch Policies, Execution Contracts, Kontext und explizite Nutzerpräferenzen bestimmt werden.

## Hard Constraints

Vor jeder Optimierung wird der gültige Lösungsraum bestimmt.

```text
Possible Optimizations
        ↓
Hard Constraint Filtering
        ↓
Valid Optimizations
        ↓
Optimization
```

Hard Constraints umfassen insbesondere:

```text
Safety
Security
Authorization
Data Sovereignty
Trust
Correctness
Hard Realtime
Required Determinism
Resource Limits
Explicit User Constraints
```

Eine Optimierung darf diese Grenzen niemals abschwächen.

## Baseline

Self-Optimization benötigt eine Vergleichsbasis.

```text
Current Behavior
      ↓
Baseline
      ↓
Optimization
      ↓
Measured Result
      ↓
Comparison
```

Nur messbare Verbesserungen sollen langfristig übernommen werden.

## Multi-Objective Optimization

Optimierungsziele können miteinander konkurrieren.

Beispiel:

```text
Lower Latency
     ↕
Lower Energy
     ↕
Lower Memory Usage
```

NovaOS darf deshalb keine einzelne globale Kennzahl als universelles Optimierungsziel voraussetzen.

Die jeweilige Policy bestimmt die zulässigen Trade-offs.

## Adaptive Modelle

Self-Optimization darf adaptive Modelle verwenden.

```text
Prediction
    ↓
Optimization Decision
    ↓
Actual Result
    ↓
Prediction Error
    ↓
Model Correction
```

Wiederholte Abweichungen sollen zukünftige Entscheidungen verbessern.

## Context Awareness

Optimierungen dürfen vom aktuellen Kontext abhängen.

Beispiele:

```text
Interactive Workload
Background Workload
Battery Operation
AC Power
Memory Pressure
Network Congestion
Thermal Pressure
Current Workspace
```

Eine unter einem Kontext erfolgreiche Optimierung darf nicht automatisch für jeden anderen Kontext übernommen werden.

## Cross-Subsystem Optimization

NovaOS darf Optimierungen subsystemübergreifend planen.

Beispiel:

```text
Storage Placement
       +
Network Cost
       +
Compute Location
       +
Energy
       ↓
Joint Optimization
```

Dadurch sollen lokale Optimierungen vermieden werden, die das Gesamtsystem verschlechtern.

## Optimization Scope

Optimierungen besitzen einen definierten Scope.

Beispiele:

```text
Execution
Capability
Process
Workload
Provider
Device
Node
System
```

NovaOS soll bevorzugt den kleinsten Scope verwenden, der das Optimierungsziel sinnvoll erreicht.

## Controlled Exploration

NovaOS darf alternative Strategien kontrolliert erproben.

```text
Valid Solution Space
       ↓
Controlled Exploration
       ↓
Measurement
       ↓
Keep / Reject
```

Exploration darf keine Hard Constraints verletzen oder garantierte Workloads gefährden.

## Regression Detection

Eine vermeintliche Optimierung kann das System verschlechtern.

NovaOS muss deshalb Regressionen erkennen können.

```text
Optimization
     ↓
Measured Result
     ↓
Worse than Baseline
     ↓
Regression
     ↓
Rollback / Policy Correction
```

## Optimization Rollback

Optimierungsentscheidungen sollen reversibel sein, soweit dies technisch möglich ist.

```text
Optimization State N
        ↓
Regression
        ↓
Rollback
        ↓
Previous Valid State
```

Nicht reversible Optimierungen benötigen strengere Autonomy Policies.

## Resource Economy

Self-Optimization selbst verbraucht Ressourcen.

Der erwartete Nutzen muss deshalb gegen die Optimierungskosten bewertet werden.

```text
Expected Benefit
      >
Optimization Cost
```

Zu berücksichtigen sind beispielsweise:

```text
CPU Time
Memory
Energy
Profiling Cost
Data Collection
Migration Cost
Network Traffic
```

## Nutzerautorität

Explizite Nutzerentscheidungen besitzen innerhalb der Hard Constraints Vorrang.

Beispiele:

```text
Prefer Performance
Prefer Efficiency
Keep Local
Disable Automatic Migration
Disable Specific Optimization
```

Self-Optimization darf solche Entscheidungen nicht eigenständig überschreiben.

## Deterministic Mode

Im Deterministic Mode müssen relevante Optimierungszustände:

```text
Fixed
Versioned
oder
Ignored
```

werden können.

Online-Optimierung darf eine deterministische Ausführung nicht unkontrolliert verändern.

## Explainability

NovaOS soll relevante Optimierungsentscheidungen erklären können.

Beispiele:

```text
Was wurde optimiert?

Warum wurde die Änderung vorgenommen?

Welche Ziele wurden berücksichtigt?

Welche Alternative wurde gewählt?

Welche Verbesserung wurde gemessen?

Wurde eine Regression erkannt?
```

## Observability

Self-Optimization muss auf messbaren Daten basieren.

Relevante Informationen können sein:

```text
Baseline
Prediction
Optimization Decision
Resource Usage
Latency
Energy
Throughput
Result
Prediction Error
Regression
Rollback
```

Decision Tracing und Resource Tracing bilden hierfür die Grundlage.

## Fail-Safe Verhalten

Fällt das Optimierungssystem aus oder ist das Ergebnis unsicher, muss NovaOS auf einen sicheren nicht-adaptiven Zustand zurückfallen können.

```text
Optimization Failure
        ↓
Known Safe Policy
```

Self-Optimization ist keine Voraussetzung für die korrekte Grundfunktion des Systems.

## Normative Anforderungen

1. NovaOS SOLL Self-Optimization als autonome Systemfunktion unterstützen.
2. Optimierungen DÜRFEN ausschließlich innerhalb gültiger Hard Constraints erfolgen.
3. Self-Optimization MUSS auf messbaren Beobachtungen basieren.
4. Eine Baseline MUSS für relevante Optimierungen bestimmbar sein.
5. Mehrere Optimierungsziele MÜSSEN gleichzeitig berücksichtigt werden können.
6. Adaptive Modelle DÜRFEN Prediction Error zur Verbesserung zukünftiger Entscheidungen verwenden.
7. Context Awareness DARF Optimierungsentscheidungen beeinflussen.
8. Subsystemübergreifende Optimierungen MÜSSEN möglich sein.
9. Optimierungen SOLLEN den kleinsten sinnvollen Scope verwenden.
10. Exploration DARF ausschließlich innerhalb des gültigen Constraint Space erfolgen.
11. Garantierte Workloads DÜRFEN durch Exploration nicht gefährdet werden.
12. Regressionen MÜSSEN erkennbar sein.
13. Optimierungen SOLLEN soweit möglich reversibel sein.
14. Optimierungskosten MÜSSEN gegenüber dem erwarteten Nutzen bewertbar sein.
15. Explizite Nutzerentscheidungen MÜSSEN Vorrang vor adaptiven Optimierungspräferenzen besitzen.
16. Deterministic Mode DARF durch Self-Optimization nicht unkontrolliert beeinflusst werden.
17. Relevante Optimierungsentscheidungen SOLLEN erklärbar sein.
18. Self-Optimization MUSS mit Observability, Resource Tracing und Decision Tracing korrelierbar sein.
19. Bei Ausfall der Optimierungslogik MUSS ein sicherer nicht-adaptiver Zustand verfügbar sein.
20. NovaOS MUSS ohne Self-Optimization vollständig korrekt funktionieren.

## Konsequenzen

### Positive Konsequenzen

- NovaOS kann seine Leistung kontinuierlich an reale Workloads anpassen,
- Ressourcen, Energie und Datenbewegung können systemweit optimiert werden,
- lokale Optimierungen können gegen globale Auswirkungen bewertet werden,
- Regressionen können erkannt und zurückgenommen werden,
- Prediction Error verbessert zukünftige Entscheidungen.

### Negative Konsequenzen

- Optimierung und Messung erzeugen selbst Ressourcenverbrauch,
- mehrere Optimierungsziele können Zielkonflikte erzeugen,
- Cross-Subsystem Optimization erhöht die Planungskomplexität.

## Abhängigkeiten

- `ADR-AUTONOMY-0001_Policy_Driven_Autonomy`
- `ADR-AUTONOMY-0002_Constraint_Driven_Autonomy`
- `ADR-AUTONOMY-0004_Self_Diagnosis`
- `ADR-AUTONOMY-0005_Self_Healing`
- `ADR-ADAPTIVE-0001_Prediction_Error_als_zentrales_Lernsignal`
- `ADR-ADAPTIVE-0011_Context_Awareness`
- `ADR-ADAPTIVE-0012_Policy_Learning_mit_harten_Grenzen`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`

## Zugehörige NPSPECs

- `NPSPEC-AUTONOMY-SELF-OPTIMIZATION-0001`
- `NPSPEC-AUTONOMY-OPTIMIZATION-BASELINE-0001`
- `NPSPEC-AUTONOMY-MULTI-OBJECTIVE-OPTIMIZATION-0001`
- `NPSPEC-AUTONOMY-REGRESSION-DETECTION-0001`
- `NPSPEC-AUTONOMY-OPTIMIZATION-ROLLBACK-0001`

## Ergebnis

NovaOS behandelt Optimierung als kontinuierlichen kontrollierten Feedback-Prozess:

```text
Observe
   ↓
Measure
   ↓
Optimize
   ↓
Execute
   ↓
Compare with Baseline
   ↓
Keep / Rollback
   ↓
Learn
```

Der zentrale Grundsatz lautet:

```text
NovaOS darf kontinuierlich lernen,
wie es seine Aufgaben besser erledigt.

Eine Änderung gilt jedoch nicht
allein deshalb als Verbesserung,
weil ein Modell sie vorgeschlagen hat.

Verbesserungen müssen messbar sein,
dürfen keine Hard Constraints verletzen
und müssen bei Regressionen
zurückgenommen werden können.
```