# ADR-AUTONOMY-0005 – Self-Healing

## Status

Angenommen

## Kategorie

Autonomy / Resilience / Self-Healing

## Kontext

NovaOS soll erkannte Fehler nicht nur diagnostizieren, sondern geeignete Störungen innerhalb definierter Grenzen selbstständig beheben können.

Typische Fälle sind:

```text
Service Failure
Driver Failure
Resource Exhaustion
Configuration Drift
Provider Failure
Storage Failure
Network Failure
Dependency Failure
Contract Violation
Degraded System State
```

Self-Healing darf jedoch keine unkontrollierte automatische Reparatur sein. Recovery muss auf Diagnose, Policies, Constraints und überprüfbaren Ergebnissen basieren.

## Entscheidung

NovaOS unterstützt **Self-Healing** als policy- und constraint-gesteuerten autonomen Recovery-Prozess.

```text
Failure / Degradation
        ↓
Self-Diagnosis
        ↓
Recovery Candidates
        ↓
Policies + Constraints
        ↓
Recovery Plan
        ↓
Execute
        ↓
Verify
        ↓
Recovered / Degraded / Failed / Unknown
```

Self-Healing arbeitet grundsätzlich auf dem kleinsten sicheren Recovery Scope.

## Recovery Scope

NovaOS soll Fehler möglichst lokal beheben.

Bevorzugte Reihenfolge:

```text
Operation
   ↓
Capability
   ↓
Process / Service
   ↓
Provider / Driver
   ↓
Subsystem
   ↓
Node
   ↓
System
```

Eine größere Recovery-Domain darf nur verwendet werden, wenn eine kleinere Maßnahme nicht ausreichend oder nicht sicher ist.

## Diagnose vor Recovery

Self-Healing basiert auf Self-Diagnosis.

```text
Observation
    ↓
Diagnosis
    ↓
Evidence
    ↓
Recovery Planning
```

Eine unsichere Diagnose darf berücksichtigt werden, muss jedoch als solche sichtbar bleiben.

Bei `Unknown` dürfen nur Recovery-Maßnahmen ausgeführt werden, die für diesen Unsicherheitsgrad zulässig sind.

## Recovery Candidates

Mögliche Recovery-Aktionen sind beispielsweise:

```text
Retry
Restart
Reconfigure
Reallocate Resources
Switch Provider
Switch Algorithm
Reconnect
Rebuild Cache
Restore Version
Rollback
Failover
Migrate
Quarantine
Enter Safe State
```

Welche Aktionen zulässig sind, hängt von Semantik, Policies und Constraints ab.

## Recovery Planning

Recovery wird als Planungsproblem behandelt.

```text
Diagnosis
    +
Current State
    +
Recovery Candidates
    +
Policies
    +
Constraints
    ↓
Valid Recovery Space
    ↓
Recovery Plan
```

Performance oder Verfügbarkeit dürfen keine ungültige Recovery-Maßnahme rechtfertigen.

## Operationssemantik

Retries und Wiederholungen sind nur zulässig, wenn die Operation dies erlaubt.

NovaOS muss insbesondere unterscheiden:

```text
Idempotent
Retry-Safe
Compensatable
Transactional
Irreversible
Unknown
```

Eine Operation mit unbekanntem Abschlussstatus darf nicht blind wiederholt werden.

```text
Unknown Completion
      ≠
Safe to Retry
```

## Transactional Recovery

Soweit möglich, sollen Recovery-Maßnahmen transaktional erfolgen.

```text
Prepare
   ↓
Validate
   ↓
Apply Recovery
   ↓
Verify
   ↓
Commit
```

Schlägt die Recovery fehl, soll ein vorheriger sicherer Zustand erhalten oder wiederhergestellt werden.

## Desired State

Self-Healing orientiert sich am deklarativen Desired State.

```text
Desired State
      ↕
Actual State
      ↓
Failure / Drift
      ↓
Recovery
      ↓
Reconciliation
```

Self-Healing darf den Desired State nicht eigenmächtig verändern, um einen Fehler scheinbar zu beseitigen.

## Provider und Algorithm Failover

Fällt ein Provider oder Algorithmus aus, darf NovaOS eine Alternative wählen, sofern der ursprüngliche Execution Contract weiterhin erfüllt wird.

```text
Provider Failure
      ↓
Valid Alternatives
      ↓
Contract Check
      ↓
Failover
```

Hard Requirements dürfen beim Failover nicht abgeschwächt werden.

## Resource Exhaustion

Bei Ressourcenknappheit darf Self-Healing beispielsweise:

```text
Release Speculative Resources
Reduce Preloading
Evict Cache
Reclaim Resources
Throttle Soft Workloads
Replan Executions
```

Garantierte Ressourcen und Hard Constraints besitzen Vorrang.

## Security Failures

Security-relevante Fehler werden nicht wie gewöhnliche Verfügbarkeitsfehler behandelt.

```text
Security Violation
      ↓
Contain / Quarantine
      ↓
Diagnose
      ↓
Authorized Recovery
```

Ein kompromittierter Zustand darf nicht durch einen einfachen Neustart automatisch wieder als vertrauenswürdig gelten.

## Distributed Recovery

Bei Distributed Execution muss Self-Healing Unsicherheit über entfernte Zustände berücksichtigen.

```text
Connection Lost
      ↓
Remote State Unknown
      ↓
Reconcile / Determine State
      ↓
Recovery Decision
```

Dabei gilt:

```text
Unreachable ≠ Failed

Unknown Completion ≠ Failed Execution
```

Failover oder Retry dürfen keine unkontrollierten Doppeloperationen erzeugen.

## Recovery Budgets

Self-Healing muss Recovery-Schleifen begrenzen.

Beispiele:

```text
Retry Budget
Restart Budget
Recovery Time Budget
Resource Budget
Escalation Limit
```

Wiederholtes Scheitern führt zur Eskalation statt zu unbegrenzten Recovery-Versuchen.

## Escalation

Kann NovaOS einen Fehler innerhalb des erlaubten Autonomiegrades nicht sicher beheben, muss eskaliert werden können.

```text
Automatic Recovery
      ↓
Recovery Failed
      ↓
Higher Recovery Scope
      ↓
Safe Degradation
      ↓
User / Administrator Decision
```

## Verification

Jede relevante Recovery-Maßnahme muss anschließend überprüft werden.

```text
Recover
   ↓
Observe
   ↓
Verify
   ↓
Healthy?
 ├── Yes → Continue
 └── No  → Replan / Escalate
```

Eine erfolgreich ausgeführte Recovery-Aktion bedeutet nicht automatisch, dass das System wieder gesund ist.

## Adaptive Self-Healing

Adaptive Modelle dürfen Recovery-Entscheidungen verbessern.

Beispiele:

```text
Recovery Success Rate
Expected Recovery Time
Previous Failures
Resource Cost
Context
```

Prediction Error darf zur Verbesserung zukünftiger Recovery-Auswahl verwendet werden.

Adaptive Modelle dürfen jedoch keine Recovery Boundary oder Hard Constraints verändern.

## Explainability

Self-Healing muss nachvollziehbar sein.

NovaOS soll erklären können:

```text
Was ist ausgefallen?

Welche Diagnose lag vor?

Welche Recovery-Optionen bestanden?

Welche Optionen wurden ausgeschlossen?

Warum wurde diese Recovery gewählt?

War die Recovery erfolgreich?

Welche weiteren Maßnahmen wurden notwendig?
```

## Deterministic Mode

Self-Healing muss im Deterministic Mode kontrollierbar und reproduzierbar sein.

Relevante:

```text
Diagnosis
Recovery Policy
Recovery Rules
System State
Adaptive State
Recovery History
```

müssen fixiert, versioniert oder berücksichtigt werden können.

## Observability

Self-Healing muss vollständig mit Observability und Decision Tracing korrelierbar sein.

Relevante Informationen sind:

```text
Failure ID
Diagnosis ID
Recovery ID
Decision ID
Recovery Scope
Selected Action
Recovery Attempt
Verification Result
Escalation
Final State
```

## Normative Anforderungen

1. NovaOS SOLL Self-Healing als autonome Recovery-Funktion unterstützen.
2. Self-Healing MUSS auf Diagnose, Policies und Constraints basieren.
3. Recovery SOLL auf dem kleinsten sicheren Scope erfolgen.
4. Recovery-Aktionen MÜSSEN innerhalb der gültigen Autonomy Boundary liegen.
5. Hard Constraints DÜRFEN durch Recovery nicht abgeschwächt werden.
6. Retry MUSS die Semantik der betroffenen Operation berücksichtigen.
7. `Unknown Completion` DARF nicht automatisch als sichere Retry-Situation behandelt werden.
8. Recovery SOLL soweit möglich transaktional und reversibel erfolgen.
9. Self-Healing DARF Desired State nicht eigenmächtig verändern.
10. Provider- oder Algorithm-Failover MUSS den ursprünglichen Execution Contract weiterhin erfüllen.
11. Security Failures MÜSSEN containment- und trust-aware behandelt werden.
12. Distributed Recovery MUSS unbekannte Remote-Zustände explizit berücksichtigen.
13. Recovery-Versuche MÜSSEN durch Budgets begrenzbar sein.
14. Wiederholtes Recovery-Versagen MUSS eskalierbar sein.
15. Relevante Recovery-Aktionen MÜSSEN nach ihrer Ausführung verifiziert werden.
16. Eine ausgeführte Recovery-Aktion DARF nicht automatisch als erfolgreiche Recovery gelten.
17. Adaptive Modelle DÜRFEN Recovery optimieren, aber keine Recovery Boundaries verändern.
18. Self-Healing SOLL erklärbar sein.
19. Deterministic Mode MUSS Self-Healing kontrollierbar und reproduzierbar machen können.
20. Recovery Decisions MÜSSEN mit Observability und Decision Tracing korrelierbar sein.
21. Kann keine sichere Recovery bestimmt werden, MUSS NovaOS fail-safe degradieren oder eskalieren.

## Konsequenzen

### Positive Konsequenzen

- viele Systemfehler können ohne manuellen Eingriff behoben werden,
- Recovery bleibt durch Policies und Constraints kontrolliert,
- Fehler werden bevorzugt lokal statt durch großflächige Neustarts behandelt,
- Recovery-Ergebnisse werden überprüft,
- adaptive Modelle können erfolgreiche Recovery-Strategien verbessern.

### Negative Konsequenzen

- Recovery Planning erhöht die Systemkomplexität,
- verteilte und irreversible Operationen benötigen besonders vorsichtige Behandlung,
- Verification und Recovery History erzeugen zusätzlichen Systemzustand.

## Abhängigkeiten

- `ADR-AUTONOMY-0001_Policy_Driven_Autonomy`
- `ADR-AUTONOMY-0002_Constraint_Driven_Autonomy`
- `ADR-AUTONOMY-0003_Self_Configuration`
- `ADR-AUTONOMY-0004_Self_Diagnosis`
- `ADR-ADAPTIVE-0012_Policy_Learning_mit_harten_Grenzen`
- `ADR-DECLARATIVE-0001_Desired_State_und_Actual_State`
- `ADR-DECLARATIVE-0002_State_Reconciliation`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-RESILIENCE-0001`
- `ADR-RESILIENCE-0008_Recovery_Verification`
- `ADR-RESILIENCE-0020_Explainable_Self_Healing`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-OBSERVABILITY-0005_System_State_Graph`

## Zugehörige NPSPECs

- `NPSPEC-AUTONOMY-SELF-HEALING-0001`
- `NPSPEC-AUTONOMY-RECOVERY-PLANNING-0001`
- `NPSPEC-AUTONOMY-RECOVERY-SCOPE-0001`
- `NPSPEC-AUTONOMY-RECOVERY-BUDGET-0001`
- `NPSPEC-AUTONOMY-RECOVERY-VERIFICATION-0001`

## Ergebnis

NovaOS behandelt Self-Healing als kontrollierten autonomen Recovery-Zyklus:

```text
Detect
   ↓
Diagnose
   ↓
Plan
   ↓
Recover
   ↓
Verify
   ↓
Continue / Replan / Escalate
```

Der zentrale Grundsatz lautet:

```text
NovaOS soll Fehler
möglichst selbst beheben.

Aber nicht durch blindes
Neustarten oder Wiederholen.

Es diagnostiziert,
wählt eine zulässige Recovery,
führt sie kontrolliert aus
und prüft anschließend,
ob das Problem wirklich behoben ist.
```