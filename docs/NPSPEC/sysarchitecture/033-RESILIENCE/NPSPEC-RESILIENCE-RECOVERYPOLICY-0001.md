# NPSPEC-RESILIENCE-RECOVERYPOLICY-0001 – Nova Resilience Recovery Policy

## Status

Angenommen

## Kategorie

Resilience / Recovery / Policy

## Zweck

NovaOS definiert ein systemweites Recovery-Policy-Modell, das festlegt, welche Wiederherstellungsmaßnahmen bei einem Fehler zulässig sind, in welcher Reihenfolge sie versucht werden und wann eine Eskalation erfolgen muss.

```text
Failure
   ↓
Classification
   ↓
Recovery Policy
   ↓
Allowed Strategies
   ↓
Select + Execute
   ↓
Verify
```

Die Recovery Policy trennt die Entscheidung über eine Wiederherstellungsstrategie von deren technischem Mechanismus.

## Grundprinzipien

```text
Recovery Policy ≠ Recovery Mechanism
Recovery Policy ≠ Self-Healing
Recovery Policy ≠ Diagnosis
Recovery Policy ≠ Authority
Possible Recovery ≠ Allowed Recovery
Repair Attempt ≠ Successful Recovery
```

## Recovery Policy Model

```text
RecoveryPolicy
├── PolicyID
├── TargetScope
├── FailureConditions
├── AllowedStrategies
├── StrategyOrder
├── RecoveryBudget
└── EscalationPolicy
```

Optional:

```text
Criticality
AutonomyLevel
RiskLimit
DegradationProfiles
RecoveryDeadline
RequiredCapabilities
ExecutionContractID
VerificationPolicy
ProvenanceID
```

## Policy Scope

Policies können gelten für:

```text
Operation
Task
Process
Service
Driver
Device
Provider
Storage
Subsystem
Node
Distributed Service
System
```

Spezifischere Policies dürfen allgemeinere Policies verfeinern, aber keine übergeordneten Hard Constraints verletzen.

## Recovery Strategies

Eine Policy kann zulässige Strategien definieren:

```text
Retry
Reconnect
Restart
Reinitialize
Reconfigure
Failover
Rollback
Checkpoint Restore
Snapshot Restore
Provider Replacement
Resource Reallocation
Degradation
Recovery Mode
Fail-safe
```

## Strategy Order

Recovery soll möglichst mit der kleinsten ausreichenden Maßnahme beginnen.

Beispiel:

```text
Retry
  ↓ failed
Restart
  ↓ failed
Failover
  ↓ failed
Rollback
  ↓ failed
Degraded Mode
  ↓ failed
Recovery Mode
```

Die Reihenfolge kann abhängig von Fehlerklasse, Criticality und Systemzustand variieren.

## Policy Evaluation

```text
Failure Classification
        +
Current State
        +
Recovery Policy
        +
Hard Constraints
        +
Available Resources
        ↓
Valid Recovery Candidates
```

Ausgeführt werden darf nur eine gültige Recovery-Strategie.

## Failure-Aware Policy

Policies können auf Fehlerklassen reagieren.

Beispiel:

```text
Transient
→ Retry / Backoff

Recoverable
→ Restart / Reinitialize

Persistent
→ Failover / Rollback

Integrity Failure
→ Isolation / Verified Restore

Critical
→ Containment / Recovery Mode

Security Event
→ Self-Protection Policy
```

Die tatsächliche Strategie bleibt von Kontext und Constraints abhängig.

## Recovery Budget

Jede Policy kann Recovery begrenzen.

```text
RecoveryBudget
├── MaxAttempts
├── MaximumTime
├── CPU
├── Memory
├── IO
├── Network
└── Energy
```

```text
Budget Exhausted
      ↓
Escalate
```

Ein Recovery Budget darf nicht durch wiederholtes Neustarten derselben Policy implizit zurückgesetzt werden.

## Recovery Deadline

Zeitkritische Komponenten können eine Recovery Deadline besitzen.

```text
Failure
   ↓
Recovery
   ↓
Verification
   ↓
RecoveryDeadline
```

Ist rechtzeitige Wiederherstellung nicht möglich, muss eine definierte Alternative wie Failover, Degradation oder Fail-safe verwendet werden.

## Autonomy

Recovery Policies bestimmen gemeinsam mit dem Autonomy-Modell, welche Aktionen automatisch ausgeführt werden dürfen.

```text
Allowed by Recovery Policy
          +
Allowed by Autonomy Policy
          +
Required Authority
          ↓
Executable
```

```text
Policy Permission ≠ Capability Authority
```

## Constraints

Recovery-Entscheidungen folgen der NovaOS-Priorität:

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

Eine niedrigere Ebene darf eine höhere nicht überschreiben.

## Degradation Policy

Falls vollständige Recovery nicht möglich ist, kann eine Policy zulässige Degradation Profiles definieren.

```text
Full Recovery Impossible
        ↓
Allowed Degradation Profile
        ↓
Validate Guarantees
        ↓
Continue Reduced Operation
```

## Escalation

Policies müssen definieren können, wann Recovery eskaliert.

Auslöser können sein:

```text
Repeated Failure
Budget Exhaustion
Verification Failure
Unknown State
Critical Integrity Failure
Unavailable Recovery Target
Recovery Deadline Miss
```

Mögliche Eskalationsziele:

```text
Larger Recovery Domain
Failover
Rollback
Degradation
Recovery Mode
Manual Intervention
Fail-safe
```

## Distributed Recovery

Für verteilte Systeme müssen Policies berücksichtigen:

```text
Network Partition
Replica State
Consistency
Membership
Failure Domains
Quorum
Split-Brain Risk
```

Lokale Fehlerbeobachtung darf nicht automatisch globale Recovery-Aktionen autorisieren.

## Verification Policy

Eine Recovery Policy muss definieren können, wann eine Wiederherstellung als erfolgreich gilt.

```text
Recovery Action
      ↓
Integrity
Health
Dependencies
Capabilities
Trust
Execution Contract
      ↓
Verified?
```

```text
Action Completed ≠ Recovery Successful
```

## Policy Composition

Mehrere Policies können gleichzeitig relevant sein.

```text
System Policy
    ∩
Security Policy
    ∩
Recovery Policy
    ∩
Component Policy
    ∩
User Policy
```

Das Ergebnis darf keine Authority oder Garantie erweitern.

## Policy Evolution

Recovery Policies dürfen aktualisiert werden.

Änderungen müssen:

```text
Versioned
Validated
Auditable
Rollback-safe
```

sein.

Eine Policy-Änderung darf laufende Recovery nicht unkontrolliert verändern.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
PolicyID
Target Scope
Failure Conditions
Allowed Strategies
Selected Strategy
Recovery Budget
Autonomy Level
Escalation Rules
Verification Requirements
Current Recovery State
Policy Version
```

## Normative Anforderungen

1. NovaOS MUSS Recovery-Entscheidungen über explizite Recovery Policies steuerbar machen.
2. Recovery Policy MUSS von Recovery-Mechanismen getrennt bleiben.
3. Policies MÜSSEN unterschiedliche Failure Classes berücksichtigen können.
4. Policies MÜSSEN zulässige Recovery-Strategien definieren können.
5. Policies SOLLEN eine bevorzugte Strategy Order definieren können.
6. Die kleinste ausreichende Recovery-Maßnahme SOLL bevorzugt werden.
7. Recovery Policies DÜRFEN keine Capability Authority erzeugen.
8. Recovery-Aktionen MÜSSEN Hard Constraints respektieren.
9. Recovery-Versuche MÜSSEN durch Budgets begrenzbar sein.
10. Budget Exhaustion MUSS eine definierte Eskalation ermöglichen.
11. Recovery Deadlines MÜSSEN definierbar sein.
12. Autonome Recovery MUSS zusätzlich das Autonomy-Modell respektieren.
13. Degradation Profiles MÜSSEN policy-gesteuert auswählbar sein.
14. Distributed Recovery MUSS Consistency und Split-Brain berücksichtigen.
15. Lokale Fehlerbeobachtung DARF NICHT automatisch globale Recovery autorisieren.
16. Recovery-Erfolg MUSS durch eine Verification Policy bestimmbar sein.
17. Mehrere Policies MÜSSEN ohne Erweiterung von Authority kombinierbar sein.
18. Policy-Änderungen MÜSSEN versionierbar und nachvollziehbar sein.
19. Laufende Recovery DARF durch Policy-Änderungen NICHT unkontrolliert verändert werden.
20. Recovery-Policy-Entscheidungen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-RESILIENCE-FAILUREDOMAIN-0001`
- `NPSPEC-RESILIENCE-RESTART-0001`
- `NPSPEC-RESILIENCE-RETRY-0001`
- `NPSPEC-RESILIENCE-BACKOFF-0001`
- `NPSPEC-RESILIENCE-CIRCUITBREAKER-0001`
- `NPSPEC-RESILIENCE-FAILOVER-0001`
- `NPSPEC-RESILIENCE-CHECKPOINT-0001`
- `NPSPEC-RESILIENCE-ROLLBACK-0001`
- `NPSPEC-RESILIENCE-DEGRADATION-0001`
- `NPSPEC-RESILIENCE-RECOVERYMODE-0001`
- `NPSPEC-RESILIENCE-SELFHEALING-0001`
- `NPSPEC-AUTONOMY-POLICY-0001`
- `NPSPEC-AUTONOMY-CONSTRAINT-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `ADR-ARCH-0127`

## Ergebnis

```text
Failure
   ↓
Detect + Classify
   ↓
Recovery Policy
   ↓
Determine Valid Strategies
   ↓
Check Constraints + Authority
   ↓
Execute
   ↓
Verify
├── Recovered → Monitor
├── Degraded → Continue Limited
└── Failed → Escalate
```

NovaOS erhält damit ein einheitliches Recovery-Policy-Modell, das festlegt, wie Fehler abhängig von Fehlerklasse, Criticality, verfügbaren Ressourcen, Authority und Systemgarantien behandelt werden und wann von lokaler Recovery bis zum Recovery Mode eskaliert werden muss.