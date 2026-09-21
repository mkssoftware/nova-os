# ADR-ADAPTIVE-0012 – Policy Learning mit harten Grenzen

## Status

Angenommen

## Kategorie

Adaptive Systems / Policy Learning

## Kontext

NovaOS verwendet adaptive Modelle, um Entscheidungen anhand realer Erfahrungen zu verbessern.

Dabei können adaptive Policies beispielsweise beeinflussen:

```text
Scheduling
Algorithm Selection
Provider Selection
Resource Allocation
Caching
Preloading
Memory Management
Power Management
Networking
Storage Placement
```

Lernende Policies dürfen jedoch niemals selbst bestimmen, welche grundlegenden Regeln des Systems gelten.

## Entscheidung

NovaOS erlaubt **Policy Learning ausschließlich innerhalb explizit definierter harter Grenzen**.

Das Grundmodell lautet:

```text
Hard Constraints
      ↓
Allowed Decision Space
      ↓
Policy Learning
      ↓
Adaptive Decision
      ↓
Execution
      ↓
Observed Result
      ↓
Prediction Error
      ↓
Policy Update
```

Die lernende Policy optimiert ausschließlich innerhalb des bereits zulässigen Entscheidungsraums.

## Hard Boundary

Vor jeder adaptiven Policy-Entscheidung wird der zulässige Lösungsraum bestimmt.

```text
Candidate Actions
      ↓
Hard Constraint Filtering
      ↓
Allowed Actions
      ↓
Adaptive Policy
```

Nicht zulässige Aktionen dürfen dem lernenden System nicht als ausführbare Alternative zur Verfügung stehen.

## Harte Grenzen

Zu den nicht adaptiv veränderbaren Grenzen gehören insbesondere:

```text
Safety
Security
Authorization
Data Sovereignty
Trust Requirements
Correctness
Hard Realtime
Required Determinism
Resource Limits
Explicit User Constraints
```

Diese Regeln stammen aus Contracts, Policies und Systemanforderungen außerhalb des lernenden Modells.

## Soft Policy

Innerhalb der gültigen Grenzen darf Policy Learning optimieren.

Beispiele:

```text
Performance
Latency
Energy
Resource Efficiency
Locality
Cache Efficiency
Provider Preference
User Preference
```

Dabei dürfen mehrere Ziele gegeneinander abgewogen werden.

## Trennung von Policy und Boundary

NovaOS trennt ausdrücklich:

```text
Boundary
    → Was ist erlaubt?

Adaptive Policy
    → Welche erlaubte Option ist vorteilhaft?
```

Ein lernendes Modell darf seine eigene Boundary weder verändern noch umgehen.

## Policy Update

Policy Learning folgt dem allgemeinen adaptiven Feedback-Modell:

```text
Prediction
    ↓
Decision
    ↓
Observed Result
    ↓
Prediction Error
    ↓
Policy Correction
```

Eine Policy-Anpassung verändert zukünftige Bewertungen, nicht rückwirkend bereits getroffene Entscheidungen.

## Explizite Nutzerentscheidung

Explizite Nutzerentscheidungen besitzen Vorrang vor adaptiven Präferenzen.

```text
Hard Constraints
      ↓
Explicit User Decision
      ↓
Remaining Decision Space
      ↓
Adaptive Policy
```

Eine lernende Policy darf keine explizite Nutzerentscheidung selbstständig überschreiben.

## Policy Scope

Gelernte Policies besitzen einen definierten Scope.

Beispiele:

```text
Execution
Workload
Capability
Workspace
Device
User
System
```

Erfahrungen eines Scopes dürfen nicht ohne definierte Semantik auf andere Scopes übertragen werden.

## Context Awareness

Policy Learning darf Context Awareness verwenden.

```text
Context
   +
Historical Experience
   +
Current State
   ↓
Adaptive Policy
```

Dadurch kann dieselbe Policy abhängig vom Kontext unterschiedliche erlaubte Entscheidungen bevorzugen.

## Exploration

Lernende Policies dürfen kontrolliert alternative Strategien erproben.

Exploration ist nur innerhalb des zulässigen Entscheidungsraums erlaubt.

```text
Hard Boundary
     ↓
Safe Candidates
     ↓
Controlled Exploration
```

Kritische oder garantierte Workloads dürfen nicht durch unkontrollierte Exploration gefährdet werden.

## Unsicherheit

Policy Learning muss Unsicherheit berücksichtigen können.

```text
Known
Estimated
Low Confidence
Unknown
```

Bei Hard Requirements gilt:

```text
Unknown ≠ Satisfied
```

Unsicherheit darf niemals verwendet werden, um eine harte Grenze als wahrscheinlich erfüllt zu behandeln.

## Policy Versioning

Adaptive Policies müssen versionierbar sein.

Relevant sind insbesondere:

```text
Policy ID
Policy Version
Model Version
Relevant Context
Training State
Boundary Version
```

Damit können Entscheidungen nachvollzogen und bei Bedarf reproduziert werden.

## Deterministic Mode

Im Deterministic Mode muss der adaptive Policy-Zustand:

```text
Fixed
Versioned
oder
Ignored
```

werden können.

Online-Learning darf eine deterministische Ausführung nicht unkontrolliert verändern.

## Policy Rollback

Fehlerhafte oder verschlechterte adaptive Policies müssen zurücksetzbar sein.

```text
Policy Version N
      ↓
Detected Regression
      ↓
Rollback
      ↓
Policy Version N-1
```

Ein Policy Rollback verändert keine Hard Constraints.

## Observability

Relevante adaptive Entscheidungen sollen nachvollziehbar machen können:

```text
Welche Boundary galt?
Welche Optionen waren zulässig?
Welche Policy-Version wurde verwendet?
Welche Option wurde gewählt?
Welche Prediction lag zugrunde?
Was trat tatsächlich ein?
Wie wurde die Policy angepasst?
```

## Fail-Safe Verhalten

Ist die adaptive Policy:

```text
Unavailable
Invalid
Corrupted
Unknown
```

muss NovaOS auf eine sichere nicht-adaptive Policy zurückfallen können.

```text
Adaptive Policy Failure
        ↓
Safe Static Policy
```

Adaptive Policy Learning ist keine Voraussetzung für die korrekte Grundfunktion des Systems.

## Normative Anforderungen

1. NovaOS MUSS adaptive Policy und Hard Boundary strikt trennen.
2. Policy Learning DARF ausschließlich innerhalb des gültigen Entscheidungsraums arbeiten.
3. Nicht zulässige Aktionen DÜRFEN einer adaptiven Policy nicht als ausführbare Alternative bereitgestellt werden.
4. Hard Constraints DÜRFEN durch Policy Learning weder verändert noch abgeschwächt werden.
5. Prediction Error SOLL als zentrales Feedbacksignal für Policy Updates verwendet werden.
6. Explizite Nutzerentscheidungen MÜSSEN Vorrang vor adaptiven Präferenzen besitzen.
7. Gelernte Policies MÜSSEN einen definierten Scope besitzen.
8. Context Awareness DARF zur Policy-Auswahl und -Bewertung verwendet werden.
9. Exploration DARF ausschließlich innerhalb gültiger Hard Boundaries erfolgen.
10. Kritische oder garantierte Workloads DÜRFEN nicht durch unkontrollierte Exploration gefährdet werden.
11. `Unknown` DARF nicht als erfüllter Hard Constraint interpretiert werden.
12. Adaptive Policies MÜSSEN versionierbar sein.
13. Deterministic Mode MUSS adaptive Policy-Zustände fixieren, versionieren oder ignorieren können.
14. Fehlerhafte adaptive Policies MÜSSEN deaktivierbar oder zurücksetzbar sein.
15. Relevante Policy Decisions SOLLEN über Decision Tracing nachvollziehbar sein.
16. Bei Ausfall des adaptiven Modells MUSS eine sichere nicht-adaptive Policy verfügbar sein.
17. NovaOS MUSS ohne Policy Learning korrekt funktionsfähig bleiben.

## Konsequenzen

### Positive Konsequenzen

- NovaOS kann Policies kontinuierlich verbessern,
- adaptive Optimierung bleibt klar von Sicherheits- und Korrektheitsregeln getrennt,
- Exploration kann kontrolliert erfolgen,
- fehlerhafte Modelle können keine Hard Constraints selbstständig aufheben,
- adaptive Policies bleiben versionierbar und nachvollziehbar.

### Negative Konsequenzen

- jede adaptive Policy benötigt eine klar definierte Boundary,
- Policy-Versionen und Modellzustände erzeugen zusätzlichen Verwaltungsaufwand,
- strikte Grenzen können theoretisch bessere, aber nicht zulässige Optimierungen verhindern.

## Abhängigkeiten

- `ADR-ADAPTIVE-0001_Prediction_Error_als_zentrales_Lernsignal`
- `ADR-ADAPTIVE-0002_Prediction_User_Decision_Deviation_Model_Correction`
- `ADR-ADAPTIVE-0003_Explizite_Nutzerentscheidung_hat_Vorrang_vor_Adaptation`
- `ADR-ADAPTIVE-0011_Context_Awareness`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-EXECUTION-0001_Nova.ExecutionContract`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-OBSERVABILITY-0009_Architecture_Introspection_als_Systemfunktion`

## Zugehörige NPSPECs

- `NPSPEC-ADAPTIVE-POLICY-LEARNING-0001`
- `NPSPEC-ADAPTIVE-POLICY-BOUNDARY-0001`
- `NPSPEC-ADAPTIVE-POLICY-VERSIONING-0001`
- `NPSPEC-ADAPTIVE-POLICY-EXPLORATION-0001`
- `NPSPEC-ADAPTIVE-POLICY-FALLBACK-0001`

## Ergebnis

NovaOS erlaubt lernende Policies, ohne ihnen Kontrolle über die grundlegenden Systemregeln zu geben:

```text
Hard Constraints
       ↓
Allowed Decision Space
       ↓
Adaptive Policy
       ↓
Decision
       ↓
Reality
       ↓
Prediction Error
       ↓
Policy Learning
```

Der zentrale Grundsatz lautet:

```text
NovaOS darf lernen,
wie es innerhalb seiner Regeln
bessere Entscheidungen trifft.

Es darf jedoch niemals lernen,
diese Regeln zu umgehen.

Adaptation optimiert
innerhalb harter Grenzen –
sie definiert diese Grenzen nicht.
```