# ADR-AUTONOMY-0004 – Self-Diagnosis

## Status

Angenommen

## Kategorie

Autonomy / Diagnosis / System Health

## Kontext

NovaOS soll Fehler und ungewöhnliche Systemzustände möglichst selbstständig erkennen und deren Ursache eingrenzen können.

Mögliche Probleme sind beispielsweise:

```text
Service Failure
Driver Failure
Resource Exhaustion
Storage Error
Network Failure
Hardware Degradation
Configuration Drift
Performance Regression
Contract Violation
Dependency Failure
```

Reines Fehler-Logging reicht dafür nicht aus. NovaOS benötigt eine systemweite Diagnosefunktion, welche Observability, Systemzustand, Abhängigkeiten und Provenance zusammenführt.

## Entscheidung

NovaOS unterstützt **Self-Diagnosis** als autonome, evidence-basierte Systemfunktion.

```text
Observations
     ↓
Anomaly / Failure Detection
     ↓
Evidence Collection
     ↓
Dependency Analysis
     ↓
Diagnosis
     ↓
Confidence
     ↓
Recovery Decision
```

Self-Diagnosis erkennt und bewertet Probleme, führt jedoch nicht automatisch jede mögliche Recovery-Maßnahme aus.

## Diagnosis Scope

Diagnosen können unterschiedliche Bereiche betreffen:

```text
Hardware
Drivers
Processes
Services
Capabilities
Resources
Storage
Network
Configuration
Distributed Nodes
Execution Contracts
```

Die Diagnose soll auf den kleinsten sinnvollen Fehlerbereich eingegrenzt werden.

## Evidence

Diagnosen müssen auf beobachtbaren Informationen beruhen.

Mögliche Quellen sind:

```text
Logs
Metrics
Traces
Resource Tracing
Health State
System State Graph
Decision Traces
Configuration State
Provenance
Hardware Status
Contract Violations
```

Eine Prediction allein gilt nicht als ausreichender Fehlernachweis.

## Symptom und Ursache

NovaOS muss zwischen Symptom und vermuteter Ursache unterscheiden.

```text
Application Timeout
       ↓
Symptom

Storage Latency
       ↓
Possible Cause

Failing Storage Device
       ↓
Possible Root Cause
```

Self-Diagnosis darf eine Korrelation nicht automatisch als Kausalität behandeln.

## Dependency Analysis

Der System State Graph und Architecture Introspection werden verwendet, um Fehler entlang von Abhängigkeiten zu verfolgen.

```text
Failed Capability
      ↓
Provider
      ↓
Driver
      ↓
Device
```

Dadurch kann NovaOS Auswirkungen und mögliche Ursachen systemweit eingrenzen.

## Diagnosis State

Eine Diagnose besitzt einen expliziten Status.

```text
Confirmed
Probable
Possible
Unknown
```

`Probable` oder `Possible` dürfen nicht als bestätigte Ursache dargestellt werden.

## Confidence

Diagnosen dürfen eine Confidence besitzen.

```text
High
Medium
Low
Unknown
```

Confidence beschreibt die Stärke der verfügbaren Evidence und nicht die Kritikalität des Fehlers.

## Multi-Fault Diagnosis

NovaOS darf nicht grundsätzlich von einer einzelnen Fehlerursache ausgehen.

```text
Observed Failure
      ↓
Possible Cause A
Possible Cause B
Possible Cause C
```

Mehrere gleichzeitig auftretende Fehler müssen darstellbar bleiben.

## Contract Violations

Self-Diagnosis integriert Verletzungen von Execution Contracts.

Beispiele:

```text
Deadline Miss
Resource Budget Exceeded
Required Provider Unavailable
Trust Requirement Lost
Sovereignty Violation
Determinism Violation
```

Dabei muss zwischen Ursache und Folge unterschieden werden.

## Distributed Diagnosis

Bei verteilten Ausführungen muss Self-Diagnosis Node- und Netzwerkgrenzen berücksichtigen.

```text
Local Observation
      +
Remote Observation
      +
Distributed Trace
      ↓
Distributed Diagnosis
```

Ein nicht erreichbarer Node darf nicht automatisch als ausgefallen gelten.

```text
Unreachable ≠ Failed
```

Ebenso gilt:

```text
Unknown Completion ≠ Failed Execution
```

## Adaptive Diagnosis

Adaptive Modelle dürfen bei Mustererkennung und Ursachenbewertung unterstützen.

```text
Historical Failures
      +
Current Evidence
      ↓
Diagnostic Prediction
```

Adaptive Modelle dürfen jedoch keine fehlende Evidence vortäuschen.

Self-Diagnosis muss auch ohne lernende Modelle funktionsfähig bleiben.

## Diagnosis und Recovery

Diagnosis und Recovery bleiben logisch getrennt.

```text
Diagnosis
    ↓
Possible Recovery Actions
    ↓
Policy + Constraints
    ↓
Recovery Decision
```

Eine erkannte Ursache bedeutet nicht automatisch, dass NovaOS jede technisch mögliche Reparatur durchführen darf.

## Self-Healing

Self-Diagnosis bildet eine Grundlage für Self-Healing.

```text
Detect
   ↓
Diagnose
   ↓
Plan Recovery
   ↓
Recover
   ↓
Verify
```

Recovery unterliegt weiterhin den jeweiligen Autonomy Boundaries.

## Unknown State

Kann keine belastbare Ursache bestimmt werden, muss dies explizit dargestellt werden.

```text
Evidence Insufficient
        ↓
Diagnosis: Unknown
```

NovaOS darf keine Ursache erfinden, nur um eine vollständige Diagnose präsentieren zu können.

## Explainability

NovaOS soll eine Diagnose nachvollziehbar darstellen können.

Beispiele:

```text
Was wurde beobachtet?

Welche Komponenten sind betroffen?

Welche Evidence liegt vor?

Welche Ursachen wurden geprüft?

Welche Ursachen wurden ausgeschlossen?

Welche Ursache wird vermutet?

Wie sicher ist diese Diagnose?
```

## Privacy und Security

Diagnosedaten können sensible Informationen enthalten.

Daher gelten bestehende:

```text
Access Control
Data Minimization
Privacy
Secret Protection
Sovereignty
```

Diagnosefunktionen erhalten keine zusätzlichen Rechte allein aufgrund ihrer Aufgabe.

## Deterministic Mode

Für reproduzierbare Diagnosen müssen relevante:

```text
Evidence
System State
Diagnostic Rules
Model Version
Policy Version
```

fixiert oder versioniert werden können.

## Observability

Self-Diagnosis baut auf Observability auf, ersetzt sie jedoch nicht.

```text
Observability
    → What happened?

Diagnosis
    → What likely caused it?
```

Diagnosen müssen auf die zugrunde liegenden Observability-Ereignisse referenzieren können.

## Normative Anforderungen

1. NovaOS SOLL Self-Diagnosis als autonome Systemfunktion unterstützen.
2. Diagnosen MÜSSEN auf verfügbarer Evidence basieren.
3. Symptome und Ursachen MÜSSEN unterscheidbar bleiben.
4. Korrelation DARF nicht automatisch als Kausalität behandelt werden.
5. Dependency Analysis MUSS systemweite Abhängigkeiten berücksichtigen können.
6. Diagnosen MÜSSEN `Confirmed`, `Probable`, `Possible` und `Unknown` unterscheiden können.
7. Confidence MUSS getrennt von Fehlerkritikalität behandelt werden.
8. Self-Diagnosis MUSS mehrere mögliche oder gleichzeitige Ursachen darstellen können.
9. Contract Violations MÜSSEN als Diagnoseinformationen verwendbar sein.
10. Distributed Diagnosis MUSS Netzwerk- und Node-Grenzen berücksichtigen.
11. `Unreachable` DARF nicht automatisch als `Failed` interpretiert werden.
12. Adaptive Modelle DÜRFEN Diagnosen unterstützen, aber fehlende Evidence nicht ersetzen.
13. Diagnosis und Recovery MÜSSEN logisch getrennt bleiben.
14. Recovery MUSS weiterhin Policy- und Constraint-gesteuert erfolgen.
15. Unzureichende Evidence MUSS als `Unknown` darstellbar sein.
16. Diagnosen SOLLEN erklärbar und auf ihre Evidence zurückführbar sein.
17. Diagnosedaten MÜSSEN bestehenden Security- und Privacy-Regeln unterliegen.
18. Deterministic Mode MUSS Diagnosen reproduzierbar machen können.
19. Self-Diagnosis MUSS mit Observability, System State Graph und Architecture Introspection korrelierbar sein.
20. NovaOS MUSS grundlegende Self-Diagnosis auch ohne adaptive Modelle durchführen können.

## Konsequenzen

### Positive Konsequenzen

- Fehlerursachen können automatisch eingegrenzt werden,
- Self-Healing erhält eine belastbare Diagnosegrundlage,
- Symptome und Root Causes werden systematisch getrennt,
- verteilte Fehler können über Systemgrenzen hinweg analysiert werden,
- Unsicherheit bleibt explizit sichtbar.

### Negative Konsequenzen

- systemweite Diagnose benötigt umfangreiche Observability-Daten,
- komplexe Abhängigkeiten können mehrere plausible Ursachen erzeugen,
- zusätzliche Evidence-Korrelation verursacht Laufzeit- und Speicheraufwand.

## Abhängigkeiten

- `ADR-AUTONOMY-0001_Policy_Driven_Autonomy`
- `ADR-AUTONOMY-0002_Constraint_Driven_Autonomy`
- `ADR-AUTONOMY-0003_Self_Configuration`
- `ADR-ARCH-0013_Introspection`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-OBSERVABILITY-0001_Unified_Logging_Metrics_und_Tracing`
- `ADR-OBSERVABILITY-0002_Distributed_Tracing`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`
- `ADR-OBSERVABILITY-0005_System_State_Graph`
- `ADR-OBSERVABILITY-0008_Provenance_Inspection`
- `ADR-OBSERVABILITY-0009_Architecture_Introspection_als_Systemfunktion`
- `ADR-RESILIENCE-0001`
- `ADR-RESILIENCE-0020_Explainable_Self_Healing`

## Zugehörige NPSPECs

- `NPSPEC-AUTONOMY-SELF-DIAGNOSIS-0001`
- `NPSPEC-AUTONOMY-DIAGNOSTIC-EVIDENCE-0001`
- `NPSPEC-AUTONOMY-ROOT-CAUSE-ANALYSIS-0001`
- `NPSPEC-AUTONOMY-DIAGNOSIS-CONFIDENCE-0001`
- `NPSPEC-AUTONOMY-DISTRIBUTED-DIAGNOSIS-0001`

## Ergebnis

NovaOS erhält eine evidence-basierte autonome Diagnosefunktion:

```text
Observe
   ↓
Detect
   ↓
Collect Evidence
   ↓
Analyze Dependencies
   ↓
Diagnose
   ↓
Determine Confidence
   ↓
Recovery Planning
```

Der zentrale Grundsatz lautet:

```text
NovaOS soll nicht nur erkennen,
dass etwas nicht funktioniert.

Es soll nachvollziehbar bestimmen,
was wahrscheinlich die Ursache ist.

Wo die Evidence nicht ausreicht,
bleibt die Antwort Unknown –
statt eine Ursache zu erfinden.
```