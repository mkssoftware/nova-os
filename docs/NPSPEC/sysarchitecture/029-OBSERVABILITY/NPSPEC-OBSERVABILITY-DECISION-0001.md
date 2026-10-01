# NPSPEC-OBSERVABILITY-DECISION-0001 – Nova Decision Observability

## Status

Angenommen

## Kategorie

Observability / Decision / Explainability

## Zweck

NovaOS definiert ein systemweites Modell zur Beobachtung und Rekonstruktion wichtiger automatischer Systementscheidungen.

Damit soll nachvollziehbar sein:

```text
Was wurde entschieden?
Warum wurde es entschieden?
Welche Alternativen existierten?
Welche Constraints galten?
Welche Daten beeinflussten die Entscheidung?
```

Dies betrifft insbesondere Scheduler, Placement, Provider-, Algorithmus-, Ressourcen- und Recovery-Entscheidungen.

## Grundprinzipien

```text
Decision Record ≠ Log
Decision Record ≠ Audit
Decision Record ≠ Policy
Decision Record ≠ Authority

Observation ≠ Decision
Correlation ≠ Causality
Prediction ≠ Fact
Rejected Alternative ≠ Invalid Alternative
```

Decision Observability soll Entscheidungen erklären, nicht selbst treffen.

## Decision Model

Eine beobachtbare Entscheidung wird beschrieben durch:

```text
DecisionRecord
├── DecisionID
├── DecisionType
├── Timestamp
├── SelectedOption
└── Outcome
```

Optional:

```text
ExecutionID
TraceID
TransactionID
ObjectID
ResourceID
ProviderID
NodeID
PolicyID
ContractID
Constraints
Candidates
Observations
ReasonCodes
Confidence
```

## Entscheidungsablauf

```text
Intent / Request
      ↓
Hard Constraints
      ↓
Eligible Candidates
      ↓
Soft Preferences
      ↓
Optimization
      ↓
Decision
```

Die Entscheidungsaufzeichnung soll diesen Ablauf rekonstruierbar machen.

## Constraint Priority

NovaOS muss insbesondere erkennen lassen, welche Prioritäten angewendet wurden:

```text
Safety
  ↓
Security
  ↓
Sovereignty / Trust
  ↓
Hard System Constraints
  ↓
Explicit User Decisions
  ↓
Soft Preferences
  ↓
Adaptive Optimization
```

Eine niedrigere Ebene darf eine höhere Ebene nicht überschreiben.

## Candidate Evaluation

Bei Auswahlentscheidungen können Kandidaten mit ihren relevanten Eigenschaften erfasst werden.

Beispiel:

```text
Provider A
├── Eligible: Yes
├── Latency: 4 ms
└── Energy: 8 J

Provider B
├── Eligible: No
└── Rejected: Sovereignty Constraint

Provider C
├── Eligible: Yes
├── Latency: 7 ms
└── Energy: 3 J
```

Nicht jede interne Bewertungsinformation muss dauerhaft gespeichert werden.

## Reason Codes

Entscheidungen sollen strukturierte Reason Codes verwenden.

Beispiele:

```text
DEADLINE_REQUIRED
INSUFFICIENT_MEMORY
TRUST_REQUIREMENT
SOVEREIGNTY_RESTRICTION
RESOURCE_PRESSURE
ENERGY_PREFERENCE
DATA_LOCALITY
PROVIDER_UNAVAILABLE
USER_FORCED_PROVIDER
DETERMINISM_REQUIRED
```

Reason Codes ermöglichen maschinenlesbare Erklärungen.

## Observability Inputs

Entscheidungen können auf Observability-Daten beruhen:

```text
Metrics
Resource Observations
Profiling
Tracing
Health State
```

Dabei muss unterscheidbar bleiben zwischen:

```text
Measured
Estimated
Predicted
Unknown
Stale
```

## Data Freshness

Wenn eine Entscheidung auf Messwerten basiert, soll deren Aktualität nachvollziehbar sein.

```text
Decision
   ↓
Observation Reference
   ↓
Timestamp + Quality
```

```text
Stale Observation ≠ Current State
```

## Adaptive Decisions

Adaptive Komponenten können Entscheidungen aufgrund veränderter Bedingungen neu treffen.

```text
Decision A
    ↓
Environment Change
    ↓
Reevaluation
    ↓
Decision B
```

Die Beziehung zwischen beiden Entscheidungen soll über IDs rekonstruierbar sein.

## Execution Integration

Execution-Entscheidungen können beispielsweise betreffen:

```text
Algorithm Selection
Provider Selection
Resource Allocation
Placement
Migration
Fallback
Degradation
Retry
Cancellation
```

Sie können über `ExecutionID` mit der betroffenen Ausführung verbunden werden.

## Distributed Decisions

Verteilte Entscheidungen können mehrere Nodes betreffen.

```text
Coordinator
    ↓
Placement Decision
    ↓
Node B
```

Lokale Teilentscheidungen müssen mit der übergeordneten Entscheidung korrelierbar sein.

Ein zentraler Decision Service ist nicht erforderlich.

## Failure Decisions

Recovery- und Self-Healing-Entscheidungen sollen nachvollziehbar sein.

Beispiele:

```text
Restart Provider
Rollback Version
Switch Replica
Migrate Task
Enter Safe Mode
Reject Operation
```

Dabei soll insbesondere sichtbar sein, welcher beobachtete Zustand die Entscheidung ausgelöst hat.

## Determinismus

Im deterministischen Modus müssen entscheidungsrelevante Eingaben ausreichend reproduzierbar oder aufgezeichnet sein.

```text
Same Inputs
+ Same Policy
+ Same Constraints
→ Same Decision
```

sofern die jeweilige Entscheidung als deterministisch definiert ist.

## Security

Decision Records können sensible Informationen über:

```text
Policies
Resources
Trust
Security Domains
Providers
System Topology
```

enthalten.

Zugriff muss capabilitybasiert kontrolliert werden.

Beispiele:

```text
DecisionRead
DecisionInspect
DecisionExport
DecisionConfigure
```

Decision Records übertragen keine Authority.

## Privacy

Entscheidungsdaten dürfen nur notwendige Informationen enthalten.

Es gelten:

```text
Data Minimization
Security Labels
Retention
Controlled Export
```

Personenbezogene oder sensible Eingaben sollen nach Möglichkeit referenziert statt dupliziert werden.

## Resource Limits

Decision Observability muss ressourcenbegrenzt sein.

Begrenzbar sind:

```text
Record Count
Candidate Detail
History
Retention
Buffer Size
Export Rate
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
DecisionID
Decision Type
Selected Option
Rejected Alternatives
Reason Codes
Hard Constraints
Soft Preferences
Policy
Observation Inputs
Data Quality
Execution Correlation
Trace Correlation
Decision Outcome
```

## Normative Anforderungen

1. NovaOS MUSS wichtige automatische Systementscheidungen strukturiert beobachtbar machen können.
2. Jede aufgezeichnete Entscheidung MUSS über eine `DecisionID` identifizierbar sein.
3. Decision Observability MUSS von Logging, Auditing und Policy Enforcement getrennt bleiben.
4. Ausgewählte Optionen und relevante Ablehnungsgründe SOLLEN rekonstruierbar sein.
5. Hard Constraints MÜSSEN von Soft Preferences unterscheidbar sein.
6. Reason Codes SOLLEN maschinenlesbar sein.
7. Entscheidungsrelevante Observability-Daten SOLLEN referenzierbar sein.
8. Gemessene, geschätzte, vorhergesagte und unbekannte Daten MÜSSEN unterscheidbar bleiben.
9. Veraltete Beobachtungen DÜRFEN NICHT als aktuelle Fakten dargestellt werden.
10. Adaptive Neuentscheidungen SOLLEN mit vorherigen Entscheidungen korrelierbar sein.
11. Execution-, Trace- und Decision-Identitäten MÜSSEN getrennt bleiben.
12. Distributed Decisions MÜSSEN über Node-Grenzen korrelierbar sein.
13. Decision Records DÜRFEN keine Authority erzeugen.
14. Sensitive Entscheidungsinformationen MÜSSEN capabilitybasiert geschützt werden.
15. Secrets und Capability Tokens DÜRFEN NICHT in Decision Records gespeichert werden.
16. Privacy- und Retention-Policies MÜSSEN berücksichtigt werden.
17. Ressourcenverbrauch der Decision Observability MUSS begrenzbar sein.
18. Decision State und Entscheidungsgrundlagen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-OBSERVABILITY-LOGGING-0001`
- `NPSPEC-OBSERVABILITY-METRICS-0001`
- `NPSPEC-OBSERVABILITY-TRACING-0001`
- `NPSPEC-OBSERVABILITY-DISTRIBUTEDTRACE-0001`
- `NPSPEC-OBSERVABILITY-PROFILING-0001`
- `NPSPEC-OBSERVABILITY-RESOURCE-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-ALGORITHM-0001`
- `NPSPEC-EXECUTION-PROVIDER-0001`
- `NPSPEC-DISTRIBUTED-SCHEDULER-0001`
- `NPSPEC-DISTRIBUTED-PLACEMENT-0001`
- `NPSPEC-RESOURCE-ARBITRATION-0001`
- `ADR-ARCH-0076`

## Ergebnis

```text
System State + Constraints
          ↓
Candidate Evaluation
          ↓
Decision
          ↓
Structured Decision Record
          ↓
Reason + Evidence + Context
          ↓
Introspection / Diagnosis
```

NovaOS erhält damit eine systemweite Decision-Observability-Schicht, durch die wichtige automatische Entscheidungen nachvollziehbar und diagnostizierbar werden, ohne Decision Records mit Policy, Authority oder der eigentlichen Entscheidungslogik gleichzusetzen.