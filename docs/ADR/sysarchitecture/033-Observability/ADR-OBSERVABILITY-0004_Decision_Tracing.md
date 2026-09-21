# ADR-OBSERVABILITY-0004 – Decision Tracing

## Status

Angenommen

## Kategorie

Observability Architecture / Decision Tracing

## Kontext

NovaOS trifft zahlreiche automatische Entscheidungen.

Beispiele:

```text
Algorithm Selection
Provider Selection
Execution Location
Resource Allocation
Fallback
Replanning
Recovery
```

Für Diagnose, Introspection und Self-Healing reicht es nicht aus zu wissen, **was** NovaOS ausgeführt hat.

Es muss auch nachvollziehbar sein, **warum** eine bestimmte Entscheidung getroffen wurde.

## Entscheidung

NovaOS führt **Decision Tracing** als Bestandteil der Unified Observability Architecture ein.

```text
Decision Request
      ↓
Candidates
      ↓
Constraints
      ↓
Evaluation
      ↓
Decision
      ↓
Decision Trace
```

Ein Decision Trace dokumentiert die für eine automatische Systementscheidung relevanten Eingaben, Ausschlüsse und Auswahlgründe.

## Decision Context

Eine Entscheidung kann mit bestehenden Observability-Identitäten korreliert werden.

Beispiele:

```text
Decision ID
Execution ID
Trace ID
Span ID
Capability ID
Algorithm Family
Provider ID
Node ID
```

Jede relevante Entscheidung erhält eine stabile `Decision ID`.

## Decision Types

Decision Tracing ist nicht auf die Execution Planning Pipeline beschränkt.

Typische Decision Types sind:

```text
algorithm.selection
provider.selection
location.selection
resource.selection
execution.replanning
fallback.selection
recovery.selection
```

Weitere Entscheidungstypen können ergänzt werden.

## Kandidaten

Der Decision Trace soll nachvollziehbar machen können, welche Kandidaten betrachtet wurden.

```text
Candidate A
Candidate B
Candidate C
      ↓
Evaluation
```

Dabei muss nicht zwangsläufig der vollständige interne Zustand jedes Kandidaten gespeichert werden.

## Ausschlussgründe

Wird ein Kandidat aufgrund eines Hard Requirements ausgeschlossen, soll der relevante Grund sichtbar sein.

```text
Provider A
    ↓
Data Sovereignty Violation
    ↓
Rejected
```

Beispiele:

```text
SemanticTypeMismatch
DeadlineUnsatisfied
ResourceBudgetUnsatisfied
TrustUnsatisfied
SovereigntyUnsatisfied
DeterminismUnsatisfied
HardwareRequirementUnsatisfied
```

## Soft Optimization

Nach dem Hard-Constraint-Filtering kann Decision Tracing dokumentieren, welche Soft Requirements die Auswahl beeinflusst haben.

```text
Valid Candidates
      ↓
Performance
Energy
Latency
Locality
Preference
      ↓
Selected Candidate
```

Dadurch bleiben Zulässigkeit und Optimierung auch in der Observability getrennt.

## Auswahlgrund

Der ausgewählte Kandidat soll mit einem strukturierten Decision Reason verbunden werden können.

Beispiel:

```text
Selected: Algorithm B

Reasons:
    Contract Valid
    Lower Expected Latency
    Data Already Local
```

Decision Reasons sollen bevorzugt maschinenlesbar und stabil definiert sein.

## Execution Planning

Decision Tracing bildet die wesentlichen Entscheidungen des Execution Planners ab.

```text
Execution Request
       ↓
Algorithm Decision
       ↓
Provider Decision
       ↓
Location Decision
       ↓
Resource Decision
       ↓
Execution Plan
```

Die einzelnen Entscheidungen können über dieselbe Execution ID miteinander korreliert werden.

## Replanning

Auch spätere Änderungen eines Plans müssen nachvollziehbar sein.

```text
Original Plan
      ↓
Provider Failure
      ↓
Replanning Decision
      ↓
New Provider
```

Der Trace soll erkennen lassen:

```text
Was änderte sich?
Warum wurde neu geplant?
Welche Alternativen bestanden?
Warum wurde der neue Plan gewählt?
```

## Adaptive Entscheidungen

Verwendet NovaOS adaptive Modelle, muss erkennbar sein, dass deren Bewertung in eine Entscheidung eingeflossen ist.

```text
Static Model
     +
Runtime Profile
     +
Adaptive Prediction
     ↓
Decision
```

Es ist nicht erforderlich, sämtliche internen Modellparameter zu protokollieren.

Relevant ist die nachvollziehbare Entscheidungsgrundlage.

## Deterministic Mode

Im Deterministic Mode kann Decision Tracing zur Überprüfung reproduzierbarer Entscheidungen verwendet werden.

```text
Same Defined Inputs
       ↓
Same Decision Process
       ↓
Same Decision
```

Relevante Policy-, Algorithmus-, Provider- oder Profilversionen sollen referenzierbar sein.

## Decision Trace und Audit

Decision Tracing und Security Audit sind getrennte Konzepte.

```text
Decision Trace
    → Warum entschied das System so?

Audit
    → Welche sicherheitsrelevante Aktion geschah?
```

Ein Ereignis kann gleichzeitig für beide Systeme relevant sein.

## Privacy und Security

Decision Traces dürfen keine unnötigen sensitiven Daten enthalten.

Insbesondere sollen nicht unkontrolliert gespeichert werden:

```text
Payload Content
Secrets
Credentials
Private Data
Sensitive Model Inputs
```

Stattdessen sollen bevorzugt IDs, Klassifikationen und strukturierte Reason Codes verwendet werden.

## Sampling

Nicht jede triviale Entscheidung muss dauerhaft vollständig gespeichert werden.

Sampling oder Aggregation ist zulässig.

Nicht verloren gehen dürfen jedoch erforderliche Informationen für:

```text
Contract Violations
Security Decisions
Safety Decisions
Critical Recovery Decisions
Required Audit Events
```

## Introspection

Decision Tracing ergänzt Architecture Introspection.

```text
Introspection:
    Welche Möglichkeiten besitzt das System?

Decision Tracing:
    Warum wurde eine konkrete Möglichkeit gewählt?
```

Damit kann NovaOS Entscheidungen gegenüber Entwicklern, Administratoren und Self-Healing-Komponenten erklären.

## Normative Anforderungen

1. NovaOS MUSS relevante automatische Systementscheidungen nachvollziehbar machen können.
2. Relevante Entscheidungen MÜSSEN über eine `Decision ID` identifizierbar sein.
3. Decision Traces MÜSSEN mit Execution IDs und Trace IDs korrelierbar sein.
4. Algorithmus-, Provider- und Location-Selection MÜSSEN tracebar sein.
5. Hard-Constraint-Ausschlüsse SOLLEN mit strukturierten Reason Codes dokumentiert werden.
6. Soft Requirements SOLLEN als Auswahlfaktoren nachvollziehbar sein.
7. Der ausgewählte Kandidat MUSS von verworfenen Kandidaten unterscheidbar sein.
8. Replanning und Fallback MÜSSEN als neue oder abgeleitete Entscheidungen nachvollziehbar sein.
9. Adaptive Entscheidungsgrundlagen SOLLEN erkennbar sein.
10. Deterministische Entscheidungen SOLLEN mit ihren relevanten Versions- und Policy-Informationen korrelierbar sein.
11. Decision Tracing DARF nicht mit Authorization oder Security Audit gleichgesetzt werden.
12. Secrets und Credentials DÜRFEN nicht in Decision Traces gespeichert werden.
13. Decision Tracing MUSS Privacy-, Security-, Retention- und Data-Sovereignty-Regeln einhalten.
14. Sampling DARF verbindlich relevante Entscheidungen nicht verbergen.
15. Decision Traces MÜSSEN für Introspection, Diagnose und Resilience nutzbar sein können.

## Konsequenzen

### Positive Konsequenzen

- automatische Entscheidungen werden erklärbar,
- Execution Planning kann detailliert nachvollzogen werden,
- Fehlentscheidungen und ungeeignete Modelle lassen sich leichter analysieren,
- Replanning und Fallback werden transparent,
- Self-Healing erhält Informationen über vorherige Entscheidungswege.

### Negative Konsequenzen

- Decision Tracing erzeugt zusätzliche Observability-Daten,
- Reason Codes und Decision Types benötigen stabile Semantik,
- komplexe Planungen können umfangreiche Decision Traces erzeugen.

## Abhängigkeiten

- `ADR-OBSERVABILITY-0001_Unified_Logging_Metrics_Tracing`
- `ADR-OBSERVABILITY-0002_Distributed_Tracing`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`
- `ADR-ARCH-0013_Introspection`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-ALGORITHM-0002_Automatische_Algorithmusauswahl_als_Standard`
- `ADR-ALGORITHM-0006_Profile_Guided_Algorithm_Selection`
- `ADR-COMPUTE-0009_Providerwahl_unter_Execution_Contracts`
- `ADR-RESILIENCE-0020_Explainable_Self_Healing`

## Zugehörige NPSPECs

- `NPSPEC-OBSERVABILITY-DECISION-TRACING-0001`
- `NPSPEC-OBSERVABILITY-DECISION-CONTEXT-0001`
- `NPSPEC-OBSERVABILITY-DECISION-REASON-0001`
- `NPSPEC-OBSERVABILITY-DECISION-CANDIDATE-0001`
- `NPSPEC-OBSERVABILITY-DECISION-CORRELATION-0001`

## Ergebnis

NovaOS kann nicht nur beobachten, was ausgeführt wurde, sondern auch, warum eine konkrete Entscheidung getroffen wurde:

```text
Request
   ↓
Candidates
   ↓
Hard Constraints
   ↓
Valid Candidates
   ↓
Soft Optimization
   ↓
Decision
   ↓
Decision Trace
   ↓
Explainability
```

Der zentrale Grundsatz lautet:

```text
NovaOS soll nicht nur sagen können:

"Das habe ich getan."

Sondern auch:

"Diese Möglichkeiten hatte ich,
diese wurden ausgeschlossen,
und deshalb habe ich
diese Entscheidung getroffen."
```