# NPSPEC-AUTONOMY-SELFDIAGNOSIS-0001 – Nova Autonomous Self-Diagnosis

## Status

Angenommen

## Kategorie

Autonomy / Self-Diagnosis / Health / Fault Analysis

## Zweck

NovaOS definiert einen autonomen Self-Diagnosis-Mechanismus, der Fehler, Anomalien und degradierte Systemzustände strukturiert analysiert und mögliche Ursachen bestimmt.

```text
Observe
   ↓
Detect Anomaly
   ↓
Collect Evidence
   ↓
Analyze
   ↓
Diagnosis
   ↓
Confidence
```

Self-Diagnosis liefert eine begründete Zustandsbewertung, führt jedoch nicht automatisch Reparaturen oder Schutzmaßnahmen aus.

## Grundprinzipien

```text
Anomaly ≠ Failure
Failure ≠ Root Cause
Correlation ≠ Causation
Diagnosis ≠ Proof
Prediction ≠ Diagnosis
Diagnosis ≠ Repair
Diagnosis ≠ Authority

Unknown ≠ Healthy
Unknown ≠ Failed
```

## Diagnosis Model

```text
Diagnosis
├── DiagnosisID
├── Target
├── ObservedSymptoms
├── CandidateCauses
├── DiagnosisState
├── Confidence
└── Evidence
```

Optional:

```text
FailureID
ObjectID
ResourceID
ExecutionID
ProviderID
DeviceID
PredictionID
TraceID
PolicyID
Timestamp
ProvenanceID
```

## Diagnosis States

NovaOS definiert mindestens:

```text
Healthy
Degraded
SuspectedFailure
ConfirmedFailure
Unknown
```

Ein Zustand darf nur entsprechend vorhandener Evidenz klassifiziert werden.

## Evidence Collection

Self-Diagnosis kann Informationen aus mehreren Quellen kombinieren:

```text
Logs
Metrics
Traces
Resource State
Health Checks
Integrity Checks
Execution Results
Crash Dumps
Boot State
Prediction Errors
Hardware Status
Provenance
```

Fehlende oder veraltete Evidenz muss als solche behandelt werden.

## Candidate Causes

Ein Symptom kann mehrere mögliche Ursachen besitzen.

```text
Symptom
├── Cause A
├── Cause B
└── Cause C
```

Candidate Causes müssen voneinander unterscheidbar bleiben, solange keine ausreichende Evidenz für eine eindeutige Ursache existiert.

## Root Cause Analysis

NovaOS kann Abhängigkeiten und Kausalitätsinformationen verwenden.

```text
Failure
   ↓
Dependency Graph
   ↓
Causal Evidence
   ↓
Probable Root Cause
```

Zeitliche Nähe allein darf nicht als Kausalitätsnachweis gelten.

## Confidence

Diagnosen müssen Unsicherheit ausdrücken können.

```text
Diagnosis
   +
Evidence Quality
   ↓
Confidence
```

Beispiel:

```text
Confirmed
High Confidence
Medium Confidence
Low Confidence
Unknown
```

Confidence darf nicht in einen künstlich sicheren Zustand umgewandelt werden.

## Dependency Analysis

Self-Diagnosis kann den NovaOS State Graph verwenden.

```text
Failed Service
      ↓ depends on
Provider
      ↓ depends on
Device
```

Damit kann zwischen primären Fehlern und Folgefehlern unterschieden werden.

## Distributed Diagnosis

Bei verteilten Systemen können Diagnosen mehrere Nodes umfassen.

```text
Node A Evidence
      +
Node B Evidence
      +
Distributed Trace
      ↓
Distributed Diagnosis
```

Eine verteilte Sicht darf nicht automatisch als atomarer globaler Zustand interpretiert werden.

## Prediction Integration

Prediction kann mögliche zukünftige Fehler oder Zusammenhänge liefern.

```text
Prediction
    ↓
Diagnostic Hint
```

Prediction darf jedoch nicht als bestätigte Diagnose behandelt werden.

## Self-Protection Integration

Self-Diagnosis kann Self-Protection informieren:

```text
Diagnosis
   ↓
Threat Assessment
   ↓
Self-Protection
```

Self-Protection führt weiterhin eigene Policy- und Constraint-Prüfungen durch.

## Self-Healing Integration

Self-Diagnosis liefert die Grundlage für Recovery-Planung.

```text
Failure
   ↓
Self-Diagnosis
   ↓
Diagnosis
   ↓
Self-Healing
   ↓
Recovery Plan
```

Eine Diagnose erzeugt keine automatische Repair Authority.

## Diagnostic Tests

NovaOS kann kontrollierte Tests durchführen:

```text
Health Check
Read Test
Memory Test
Dependency Test
Provider Test
Integrity Test
Connectivity Test
```

Diagnosetests müssen selbst Ressourcen-, Security- und Safety-Constraints beachten.

## Diagnosis Feedback

Nach einer Reparatur kann die ursprüngliche Diagnose bewertet werden.

```text
Diagnosis
   ↓
Recovery
   ↓
Observed Result
   ↓
Diagnosis Feedback
```

Damit können Diagnosemodelle verbessert werden.

## Safe Behavior

Kann keine belastbare Ursache bestimmt werden:

```text
Insufficient Evidence
        ↓
Diagnosis = Unknown
        ↓
Contain / Monitor / Escalate
```

NovaOS darf keine Ursache erfinden, nur um eine eindeutige Diagnose zu erzeugen.

## Security

Self-Diagnosis besitzt nur die für Diagnose notwendigen Capabilities.

```text
Observe ≠ Modify
Diagnose ≠ Repair
```

Zugriff auf sensible Diagnoseinformationen muss entsprechend geschützt werden.

## Privacy

Diagnosedaten können sensible Informationen enthalten.

Daher gelten:

```text
Data Minimization
Retention
Security Labels
Controlled Access
Controlled Export
```

## Provenance

Eine Diagnose soll nachvollziehbar enthalten:

```text
Evidence Sources
Observations
Tests
Candidate Causes
Selected Cause
Confidence
Timestamp
Diagnostic Method
Policy Version
```

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
DiagnosisID
Target
Symptoms
Diagnosis State
Candidate Causes
Selected Cause
Confidence
Evidence
Evidence Quality
Tests
Dependencies
Timestamp
Provenance
```

## Normative Anforderungen

1. NovaOS MUSS Self-Diagnosis von Self-Healing und Self-Protection logisch trennen.
2. Self-Diagnosis DARF keine zusätzliche Repair Authority erzeugen.
3. Anomalien DÜRFEN NICHT automatisch als Fehler behandelt werden.
4. Diagnosen DÜRFEN NICHT automatisch als bewiesene Root Cause behandelt werden.
5. `Unknown` MUSS als eigener Diagnosezustand unterstützt werden.
6. `Unknown` DARF NICHT automatisch als `Healthy` oder `Failed` interpretiert werden.
7. Diagnosen SOLLEN mehrere Candidate Causes darstellen können.
8. Evidence Quality und Freshness SOLLEN berücksichtigt werden.
9. Zeitliche Korrelation DARF NICHT automatisch als Kausalität interpretiert werden.
10. Diagnosen MÜSSEN Unsicherheit oder Confidence ausdrücken können.
11. Self-Diagnosis SOLL Dependency- und State-Graph-Informationen verwenden können.
12. Prediction DARF nur als diagnostischer Hinweis verwendet werden.
13. Diagnosetests MÜSSEN bestehende Safety-, Security- und Resource Constraints respektieren.
14. Distributed Diagnosis DARF keine atomare globale Sicht voraussetzen.
15. Self-Healing SOLL strukturierte Diagnosen für Recovery Planning verwenden können.
16. Fehlende Evidenz MUSS zu `Unknown` oder reduzierter Confidence führen können.
17. Diagnosen SOLLEN vollständige Provenance besitzen.
18. Self-Diagnosis MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-AUTONOMY-POLICY-0001`
- `NPSPEC-AUTONOMY-CONSTRAINT-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-AUTONOMY-SELFPROTECTION-0001`
- `NPSPEC-ADAPTIVE-PREDICTION-0001`
- `NPSPEC-ADAPTIVE-PREDICTIONERROR-0001`
- `NPSPEC-ADAPTIVE-FEEDBACK-0001`
- `NPSPEC-OBSERVABILITY-LOGGING-0001`
- `NPSPEC-OBSERVABILITY-METRICS-0001`
- `NPSPEC-OBSERVABILITY-TRACING-0001`
- `NPSPEC-OBSERVABILITY-STATEGRAPH-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-BOOT-HEALTH-0001`
- `ADR-ARCH-0099`

## Ergebnis

```text
Symptoms
   +
Observations
   +
Evidence
   +
Dependencies
      ↓
Self-Diagnosis
      ↓
Candidate Causes
      ↓
Evidence Evaluation
      ↓
Diagnosis + Confidence
      ↓
┌──────────────┬─────────────────┬────────────┐
│ Self-Healing │ Self-Protection │ Escalation │
└──────────────┴─────────────────┴────────────┘
```

NovaOS erhält damit eine eigenständige diagnostische Schicht, die Systemprobleme strukturiert analysiert, Unsicherheit ausdrücklich erhält und mögliche Ursachen nachvollziehbar bestimmt. Self-Healing und Self-Protection können diese Diagnosen anschließend nutzen, ohne dass eine Diagnose selbst Authority für Reparatur- oder Schutzmaßnahmen erzeugt.