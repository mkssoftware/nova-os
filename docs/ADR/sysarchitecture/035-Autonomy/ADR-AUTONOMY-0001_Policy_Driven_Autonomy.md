# ADR-AUTONOMY-0001 – Policy-Driven Autonomy

## Status

Angenommen

## Kategorie

Autonomy / Policy / System Control

## Kontext

NovaOS soll viele Systementscheidungen selbstständig treffen können, ohne für jede einzelne Aktion eine Nutzerentscheidung zu benötigen.

Beispiele:

```text
Scheduling
Resource Allocation
Algorithm Selection
Provider Selection
Caching
Preloading
Storage Placement
Power Management
Networking
Recovery
Maintenance
```

Autonomie darf jedoch nicht bedeuten, dass Subsysteme eigenständig ihre Regeln definieren.

NovaOS benötigt deshalb ein gemeinsames Modell, bei dem autonome Entscheidungen durch explizite Policies und Contracts begrenzt werden.

## Entscheidung

NovaOS verwendet **Policy-Driven Autonomy** als grundlegendes Modell für autonome Systementscheidungen.

```text
System State
     +
Intent
     +
Contracts
     +
Policies
     ↓
Allowed Decision Space
     ↓
Autonomous Decision
     ↓
Execution
     ↓
Verification
```

Autonomie entscheidet ausschließlich innerhalb eines zuvor definierten zulässigen Entscheidungsraums.

## Autonomy Boundary

Policies definieren, welche Entscheidungen ein autonomes Subsystem treffen darf.

```text
Possible Actions
      ↓
Policy + Contract Evaluation
      ↓
Allowed Actions
      ↓
Autonomous Selection
```

Autonomie darf ihre eigene Boundary nicht selbstständig erweitern.

## Policy Sources

Relevante Policies können aus unterschiedlichen Ebenen stammen:

```text
System Policy
Security Policy
Safety Policy
Sovereignty Policy
Resource Policy
Execution Contract
Administrator Policy
Explicit User Policy
Adaptive Policy
```

Konflikte werden nach der bestehenden systemweiten Constraint- und Prioritätslogik behandelt.

## Autonomy Levels

Autonomie kann unterschiedliche Freiheitsgrade besitzen.

Beispiel:

```text
Observe
Recommend
Prepare
Execute
Execute + Verify
Execute + Recover
```

Der zulässige Autonomiegrad wird durch Policy und Kontext bestimmt.

Ein Subsystem erhält nicht automatisch den höchsten Autonomiegrad.

## Hard Constraints

Autonome Entscheidungen dürfen insbesondere folgende Grenzen nicht verletzen:

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

Das Grundprinzip lautet:

```text
Hard Constraints
       ↓
Valid Solution Space
       ↓
Autonomous Optimization
```

## Nutzerautorität

Explizite Nutzerentscheidungen besitzen innerhalb des zulässigen Systemrahmens Vorrang vor autonomen Präferenzen.

```text
Hard Constraints
      ↓
Explicit User Decision
      ↓
Autonomous Decision Space
```

Das System darf eine explizite Nutzerentscheidung nicht aufgrund einer eigenen Optimierungspräferenz ignorieren.

## Adaptive Autonomie

Adaptive Modelle dürfen autonome Entscheidungen verbessern.

```text
Observation
    ↓
Prediction
    ↓
Policy-Constrained Decision
    ↓
Result
    ↓
Prediction Error
    ↓
Adaptation
```

Adaptive Modelle dürfen jedoch weder Autonomy Boundaries noch Hard Constraints verändern.

## Reversible Actions

Autonome Systeme sollen reversible Aktionen bevorzugen, wenn mehrere gleichwertige Möglichkeiten bestehen.

```text
Reversible
    >
Compensatable
    >
Irreversible
```

Irreversible oder besonders folgenreiche Aktionen können einen höheren Autonomiegrad oder explizite Bestätigung erfordern.

## Verification

Autonome Ausführung endet nicht mit der Aktion.

```text
Decide
   ↓
Execute
   ↓
Verify
```

NovaOS muss prüfen können, ob der erwartete Zustand tatsächlich erreicht wurde.

Bei Abweichungen greifen die definierten Resilience- und Recovery-Mechanismen.

## Failure und Uncertainty

Unsicherheit darf nicht als Erfolg interpretiert werden.

```text
Unknown ≠ Success
Unknown ≠ Policy Satisfied
```

Kann eine erforderliche Policy nicht zuverlässig ausgewertet werden, muss das System fail-safe reagieren.

## Explainability

Relevante autonome Entscheidungen sollen nachvollziehbar sein.

NovaOS soll erklären können:

```text
Was wurde entschieden?

Warum durfte NovaOS selbst entscheiden?

Welche Policies galten?

Welche Alternativen bestanden?

Warum wurde diese Aktion gewählt?

Wurde das erwartete Ergebnis erreicht?
```

Decision Tracing bildet hierfür die technische Grundlage.

## Autonomy und Self-Healing

Self-Healing ist eine spezialisierte Anwendung von Policy-Driven Autonomy.

```text
Failure
   ↓
Diagnosis
   ↓
Allowed Recovery Actions
   ↓
Autonomous Recovery
   ↓
Verification
```

Recovery darf nur innerhalb des jeweils erlaubten Autonomie- und Sicherheitsrahmens erfolgen.

## Autonomy und Declarative System Model

Das deklarative Systemmodell beschreibt den gewünschten Zustand.

Autonomie darf selbstständig geeignete Schritte zur Annäherung an diesen Zustand auswählen.

```text
Desired State
      ↓
Current State
      ↓
Policy Boundary
      ↓
Autonomous Planning
      ↓
Reconciliation
```

Der gewünschte Zustand selbst wird dadurch nicht eigenmächtig verändert.

## Deterministic Mode

Autonome Entscheidungen müssen im Deterministic Mode reproduzierbar gemacht werden können.

Dazu müssen relevante:

```text
Policies
Policy Versions
System State
Decision Rules
Adaptive State
```

fixiert, versioniert oder ignoriert werden können.

## Audit und Observability

Autonome Aktionen müssen mit bestehenden Observability-Mechanismen korrelierbar sein.

Relevante Informationen sind insbesondere:

```text
Decision ID
Policy Version
Autonomy Level
Execution ID
Selected Action
Decision Reason
Verification Result
Recovery Action
```

## Normative Anforderungen

1. NovaOS MUSS autonome Systementscheidungen durch explizite Policies begrenzen.
2. Autonome Komponenten DÜRFEN ihre eigene Autonomy Boundary nicht erweitern.
3. Hard Constraints MÜSSEN vor autonomer Optimierung ausgewertet werden.
4. Autonomie MUSS unterschiedliche Autonomy Levels unterstützen können.
5. Explizite Nutzerentscheidungen MÜSSEN innerhalb gültiger Hard Constraints Vorrang besitzen.
6. Adaptive Modelle DÜRFEN autonome Entscheidungen optimieren, aber keine Hard Constraints verändern.
7. Reversible Aktionen SOLLEN bei vergleichbarer Eignung bevorzugt werden.
8. Irreversible Aktionen DÜRFEN strengere Autonomy Policies erfordern.
9. Autonome Aktionen MÜSSEN verifizierbar sein.
10. `Unknown` DARF nicht als erfolgreiche Policy-Erfüllung interpretiert werden.
11. Nicht auswertbare kritische Policies MÜSSEN fail-safe behandelt werden.
12. Relevante autonome Entscheidungen SOLLEN erklärbar sein.
13. Self-Healing MUSS denselben Autonomy Boundaries unterliegen.
14. Autonomie DARF Desired State nicht eigenmächtig außerhalb ihrer Policy verändern.
15. Deterministic Mode MUSS autonome Entscheidungen reproduzierbar machen können.
16. Autonome Entscheidungen MÜSSEN mit Decision Tracing und Observability korrelierbar sein.
17. NovaOS MUSS auch ohne adaptive Modelle sichere autonome Grundfunktionen bereitstellen können.

## Konsequenzen

### Positive Konsequenzen

- NovaOS kann viele Aufgaben selbstständig erledigen,
- autonome Entscheidungen bleiben kontrollierbar,
- Self-Healing und adaptive Systeme erhalten ein gemeinsames Governance-Modell,
- Nutzerautorität und Hard Constraints bleiben erhalten,
- Autonomie kann schrittweise und abhängig vom Risiko freigegeben werden.

### Negative Konsequenzen

- jede autonome Funktion benötigt klar definierte Policies und Boundaries,
- Policy-Konflikte müssen systemweit konsistent behandelt werden,
- Verification und Decision Tracing erzeugen zusätzlichen Aufwand.

## Abhängigkeiten

- `ADR-ADAPTIVE-0003_Explizite_Nutzerentscheidung_hat_Vorrang_vor_Adaptation`
- `ADR-ADAPTIVE-0011_Context_Awareness`
- `ADR-ADAPTIVE-0012_Policy_Learning_mit_harten_Grenzen`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-DECLARATIVE-0001_Desired_State_und_Actual_State`
- `ADR-DECLARATIVE-0002_State_Reconciliation`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-OBSERVABILITY-0009_Architecture_Introspection_als_Systemfunktion`
- `ADR-RESILIENCE-0020_Explainable_Self_Healing`

## Zugehörige NPSPECs

- `NPSPEC-AUTONOMY-POLICY-0001`
- `NPSPEC-AUTONOMY-BOUNDARY-0001`
- `NPSPEC-AUTONOMY-LEVEL-0001`
- `NPSPEC-AUTONOMY-VERIFICATION-0001`
- `NPSPEC-AUTONOMY-DECISION-0001`

## Ergebnis

NovaOS erhält kontrollierte Autonomie:

```text
Intent
   ↓
Policies + Contracts
   ↓
Allowed Decision Space
   ↓
Autonomous Decision
   ↓
Execution
   ↓
Verification
```

Der zentrale Grundsatz lautet:

```text
NovaOS darf selbstständig entscheiden,
wie ein erlaubtes Ziel erreicht wird.

Es entscheidet jedoch nicht selbst,
welche Grenzen dafür gelten.

Autonomie handelt innerhalb
expliziter Policies –
nicht außerhalb von ihnen.
```