# NPSPEC-RESILIENCE-SELFHEALING-0001 – Nova Resilience Self-Healing

## Status

Angenommen

## Kategorie

Resilience / Self-Healing / Recovery

## Zweck

NovaOS definiert einen systemweiten Self-Healing-Mechanismus, der erkannte Fehler kontrolliert eindämmt, diagnostiziert, repariert und die erfolgreiche Wiederherstellung anschließend verifiziert.

```text
Failure
   ↓
Detect
   ↓
Contain
   ↓
Diagnose
   ↓
Plan
   ↓
Repair
   ↓
Verify
   ↓
Monitor
```

Self-Healing verbindet die einzelnen Resilience-Mechanismen zu einem kontrollierten Recovery-Zyklus.

## Grundprinzipien

```text
Self-Healing ≠ Unlimited Autonomy
Self-Healing ≠ Restart
Self-Healing ≠ Rollback
Self-Healing ≠ Self-Protection
Repair Attempt ≠ Recovery
Recovered ≠ Verified
Prediction ≠ Permission
```

Self-Healing darf keine Security-, Safety-, Trust-, Sovereignty- oder Capability-Grenzen umgehen.

## Self-Healing Model

```text
HealingOperation
├── HealingID
├── TargetID
├── DomainID
├── FailureID
├── RecoveryPlan
├── RecoveryBudget
└── State
```

Optional:

```text
ClassificationID
DiagnosisID
ExecutionContractID
CheckpointID
ProviderID
AutonomyLevel
RiskLevel
VerificationState
ProvenanceID
```

## Zustände

```text
Idle
Detecting
Containing
Diagnosing
Planning
Repairing
Verifying
Monitoring
Recovered
Degraded
Escalated
Failed
Unknown
```

## Healing Cycle

```text
Observe
   ↓
Failure Detection
   ↓
Classification
   ↓
Containment
   ↓
Self-Diagnosis
   ↓
Recovery Planning
   ↓
Constraint Validation
   ↓
Recovery Action
   ↓
Verification
   ↓
Monitoring
```

Jede Phase muss unabhängig fehlschlagen oder eskalieren können.

## Recovery Planning

Self-Healing wählt aus zulässigen Recovery-Strategien.

```text
Retry
Restart
Reconnect
Reinitialize
Reconfigure
Failover
Rollback
Checkpoint Restore
Provider Replacement
Resource Reallocation
Degradation
Recovery Mode
```

Bevorzugt wird:

```text
Smallest Sufficient Recovery Action
```

## Constraint Validation

Vor einer autonomen Reparatur müssen relevante Constraints geprüft werden.

```text
Recovery Candidate
       ↓
Safety
       ↓
Security
       ↓
Trust
       ↓
Sovereignty
       ↓
Hard System Constraints
       ↓
User Decisions
       ↓
Execution Contract
       ↓
Allowed?
```

```text
Technically Possible ≠ Authorized
```

## Autonomy

Self-Healing integriert sich in das NovaOS-Autonomy-Modell.

```text
Level 0 → Manual
Level 1 → Suggest
Level 2 → Confirm
Level 3 → Limited Autonomous
Level 4 → Autonomous
```

Risiko und Reversibilität bestimmen mit, welche Aktionen automatisch ausgeführt werden dürfen.

## Recovery Budget

Automatische Reparaturen müssen begrenzt sein.

```text
RecoveryBudget
├── Attempts
├── MaximumTime
├── CPU
├── Memory
├── IO
├── Network
└── Energy
```

Budget-Erschöpfung führt zu Degradation oder Eskalation statt zu endlosen Reparaturversuchen.

## Recovery Loops

NovaOS muss Schleifen erkennen können.

```text
Failure
 ↓
Restart
 ↓
Failure
 ↓
Restart
 ↓
...
```

Gegenmaßnahmen:

```text
Retry Limit
Backoff
Cooldown
Circuit Breaker
Failure Reclassification
Alternative Recovery
Escalation
```

## State Recovery

Zustand kann wiederhergestellt werden über:

```text
Checkpoint
Snapshot
Transaction Rollback
Previous Version
Replica
Persistent State
```

Vor Verwendung müssen Integrität und Kompatibilität validiert werden.

## Capability Handling

Self-Healing besitzt keine implizite universelle Authority.

Jede Recovery-Aktion benötigt passende Capabilities.

```text
Healing Intent
      ↓
Required Capability
      ↓
Authorization
      ↓
Recovery Action
```

Widerrufene oder abgelaufene Authority darf nicht wiederhergestellt werden.

## Provider Recovery

Bei Provider-Ausfall kann Self-Healing:

```text
Restart Provider
Reinitialize Provider
Replace Provider
Failover
Rebind Capability
```

Das neue Ziel muss erneut gegen Execution Contract, Trust und Sovereignty geprüft werden.

## Resource Recovery

Bei Ressourcenproblemen kann Self-Healing:

```text
Reclaim
Reallocate
Throttle
Move Workload
Reduce Optional Work
Degrade
```

auslösen.

Resource Economy und bestehende Reservations müssen berücksichtigt werden.

## Security Integration

Self-Healing und Self-Protection bleiben getrennte Verantwortlichkeiten.

```text
Normal Failure
      ↓
Self-Healing

Security Event
      ↓
Self-Protection
      ↓
Controlled Recovery
```

Ein möglicher Angriff darf nicht wie ein gewöhnlicher transienter Fehler blind repariert werden.

## Distributed Self-Healing

In verteilten Systemen muss Self-Healing partielle Fehler berücksichtigen.

```text
No Response ≠ Remote Failure
Network Partition ≠ Node Failure
Local Recovery ≠ Global Recovery
```

Failover, Replica Promotion und Reconfiguration müssen Split-Brain und Consistency berücksichtigen.

## Graceful Degradation

Kann der ursprüngliche Zustand nicht sicher wiederhergestellt werden:

```text
Full Operation
      ↓
Recovery Impossible
      ↓
Validated Degraded Profile
```

Eine sichere reduzierte Funktion ist einem unsicheren vollständigen Betrieb vorzuziehen.

## Recovery Mode

Sind autonome Recovery-Möglichkeiten ausgeschöpft:

```text
Self-Healing
     ↓
Recovery Budget Exhausted
     ↓
Recovery Mode
```

NovaDOS kann anschließend als unabhängige Recovery-Umgebung übernehmen.

## Verification

Eine Recovery-Aktion gilt erst nach erfolgreicher Prüfung als abgeschlossen.

Zu prüfen sind abhängig vom Ziel:

```text
Health
Integrity
State
Dependencies
Capabilities
Trust
Resources
Execution Contracts
Realtime Guarantees
```

```text
Repair Successful ≠ Recovery Verified
```

## Post-Recovery Monitoring

Nach erfolgreicher Verification folgt eine Beobachtungsphase.

```text
Repair
  ↓
Verify
  ↓
Monitoring
  ├── Stable → Recovered
  └── Failure → Reclassify / Escalate
```

Dadurch werden kurzfristige scheinbare Reparaturen erkannt.

## Provenance

Self-Healing-Aktionen müssen nachvollziehbar bleiben.

```text
What Failed
What Was Diagnosed
What Was Changed
Why It Was Changed
Which Authority Was Used
Which State Was Restored
Verification Result
```

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
HealingID
TargetID
Failure
Diagnosis
Recovery Plan
Current Phase
Autonomy Level
Recovery Budget
Actions Performed
Verification State
Final Health State
Provenance
```

## Normative Anforderungen

1. NovaOS MUSS einen kontrollierten Self-Healing-Zyklus unterstützen können.
2. Self-Healing MUSS Detection, Containment, Diagnosis, Planning, Repair und Verification trennen.
3. Self-Healing DARF NICHT als universelle Authority wirken.
4. Jede Recovery-Aktion MUSS relevante Capabilities besitzen.
5. Safety-, Security-, Trust- und Sovereignty-Constraints MÜSSEN vor Recovery-Aktionen geprüft werden.
6. Die kleinste ausreichende Recovery-Maßnahme SOLL bevorzugt werden.
7. Recovery-Versuche MÜSSEN durch Recovery Budgets begrenzbar sein.
8. Endlose Recovery Loops MÜSSEN erkannt und unterbrochen werden können.
9. Checkpoints, Snapshots und frühere Zustände MÜSSEN vor Wiederherstellung validiert werden.
10. Widerrufene Authority DARF durch Recovery NICHT reaktiviert werden.
11. Provider-Wechsel MUSS relevante Execution Contracts erneut validieren.
12. Security Events MÜSSEN von normalen Fehlern unterscheidbar bleiben.
13. Distributed Self-Healing MUSS Network Partitions und Split-Brain berücksichtigen.
14. Degradation MUSS als zulässige Recovery-Strategie verwendbar sein.
15. Recovery Mode MUSS als Eskalationsziel verfügbar sein können.
16. Eine ausgeführte Reparatur DARF NICHT automatisch als erfolgreiche Recovery gelten.
17. Recovery MUSS vor Rückkehr zum Normalbetrieb verifiziert werden.
18. Nach Recovery SOLL eine kontrollierte Monitoring-Phase erfolgen.
19. Fehlgeschlagene Recovery MUSS Reclassification oder Eskalation ermöglichen.
20. Self-Healing-Aktionen MÜSSEN nachvollziehbar und autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-RESILIENCE-ISOLATION-0001`
- `NPSPEC-RESILIENCE-CONTAINMENT-0001`
- `NPSPEC-RESILIENCE-FAILUREDOMAIN-0001`
- `NPSPEC-RESILIENCE-WATCHDOG-0001`
- `NPSPEC-RESILIENCE-RESTART-0001`
- `NPSPEC-RESILIENCE-RETRY-0001`
- `NPSPEC-RESILIENCE-BACKOFF-0001`
- `NPSPEC-RESILIENCE-CIRCUITBREAKER-0001`
- `NPSPEC-RESILIENCE-FAILOVER-0001`
- `NPSPEC-RESILIENCE-REDUNDANCY-0001`
- `NPSPEC-RESILIENCE-CHECKPOINT-0001`
- `NPSPEC-RESILIENCE-ROLLBACK-0001`
- `NPSPEC-RESILIENCE-DEGRADATION-0001`
- `NPSPEC-RESILIENCE-RECOVERYMODE-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-AUTONOMY-SELFDIAGNOSIS-0001`
- `NPSPEC-AUTONOMY-SELFPROTECTION-0001`
- `ADR-ARCH-0126`

## Ergebnis

```text
Failure
   ↓
Detect
   ↓
Classify
   ↓
Contain
   ↓
Diagnose
   ↓
Plan
   ↓
Validate Authority + Constraints
   ↓
Repair
   ↓
Verify
   ↓
Monitor
├── Stable → Recovered
├── Limited → Degraded
└── Failed → Escalate / Recovery Mode
```

NovaOS erhält damit einen systemweiten Self-Healing-Mechanismus, der vorhandene Resilience-Funktionen zu einem kontrollierten Recovery-Zyklus verbindet und Fehler möglichst autonom behebt, ohne Authority, Sicherheitsgrenzen oder harte Systemgarantien zu umgehen.