# NPSPEC-RESILIENCE-VERIFICATION-0001 – Nova Resilience Recovery Verification

## Status

Angenommen

## Kategorie

Resilience / Recovery / Verification

## Zweck

NovaOS definiert eine systemweite Recovery Verification, mit der nach einer Recovery-Maßnahme geprüft wird, ob ein Systemzustand tatsächlich wieder korrekt, sicher und ausreichend funktionsfähig ist.

```text
Failure
   ↓
Recovery Action
   ↓
Verification
   ├── Valid → Recovered
   ├── Limited → Degraded
   └── Invalid → Escalate
```

Eine technisch abgeschlossene Recovery-Aktion gilt nicht automatisch als erfolgreiche Wiederherstellung.

## Grundprinzipien

```text
Recovered ≠ Verified
Running ≠ Healthy
Reachable ≠ Functional
No Error ≠ Correct
Integrity ≠ Availability
Health Check ≠ Complete Verification
Unknown ≠ Valid
```

## Verification Model

```text
RecoveryVerification
├── VerificationID
├── TargetID
├── DomainID
├── RecoveryID
├── VerificationPolicy
├── Checks
├── Result
└── State
```

Optional:

```text
FailureID
CheckpointID
ExecutionContractID
ExpectedState
ObservedState
HealthState
IntegrityState
TrustState
ProvenanceID
```

## Zustände

```text
Pending
Preparing
Verifying
Passed
PartiallyPassed
Failed
Inconclusive
Cancelled
Unknown
```

`Inconclusive` oder `Unknown` dürfen bei erforderlichen Hard Guarantees nicht als Erfolg behandelt werden.

## Verification Scope

Verification kann erfolgen für:

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

Der Verification Scope soll mindestens den Recovery Scope und dessen kritische Abhängigkeiten umfassen.

## Verification Checks

Abhängig vom Ziel können geprüft werden:

```text
Health
Integrity
State Consistency
Dependencies
Capabilities
Trust
Security
Resources
Configuration
Storage
Connectivity
Execution Contracts
Realtime Guarantees
```

## Expected State

Verification vergleicht beobachteten mit erwartetem Zustand.

```text
Expected State
      ↓
Compare
      ↑
Observed State
```

Der erwartete Zustand kann aus:

```text
System Model
Recovery Policy
Execution Contract
Checkpoint Metadata
Configuration
Health Policy
```

abgeleitet werden.

## Health Verification

Ein gestarteter Prozess oder Service gilt nicht allein aufgrund seiner Existenz als gesund.

```text
Started
   ↓
Responsive
   ↓
Functional
   ↓
Dependencies Valid
   ↓
Healthy
```

## Integrity Verification

Nach Recovery müssen relevante Komponenten auf Integrität geprüft werden können.

Beispiele:

```text
Memory State
Executable Code
Configuration
Filesystem
Checkpoint
Snapshot
Recovered Data
```

Ein Integritätsfehler muss erneute Containment- oder Recovery-Maßnahmen auslösen können.

## Capability Verification

Recovery darf keine ungültige Authority erzeugen.

Zu prüfen sind:

```text
Capability Validity
Revocation
Expiration
Scope
Delegation
Attenuation
Target Binding
```

```text
Previously Valid ≠ Currently Valid
```

## Dependency Verification

Abhängigkeiten müssen nach Recovery erneut bewertet werden.

```text
Recovered Service
      ↓
Dependency A → Healthy
Dependency B → Degraded
Dependency C → Failed
```

Der lokale Zustand `Healthy` darf nicht gesetzt werden, wenn zwingende Abhängigkeiten die erforderlichen Guarantees nicht erfüllen.

## Execution Contract Verification

Nach Recovery muss geprüft werden, ob relevante Execution Contracts weiterhin erfüllt werden.

```text
Semantic Requirements
Resource Budget
Latency
Deadline
Determinism
Trust
Security
Sovereignty
Provider Constraints
```

## Realtime Verification

Für Realtime-Komponenten reicht funktionale Korrektheit nicht aus.

```text
Correct Result
     +
Deadline Met
     +
Bounded Latency
     +
Required Determinism
     ↓
Realtime Valid
```

Eine funktionierende Komponente kann trotzdem ihre Realtime-Garantie verloren haben.

## Distributed Verification

Verteilte Recovery muss zusätzlich berücksichtigen:

```text
Membership
Replica State
Consistency
Quorum
Causality
Network Partition
Split-Brain
```

```text
Local Healthy ≠ Distributed Healthy
```

Die Wiederherstellung eines einzelnen Nodes beweist nicht die Wiederherstellung des Gesamtdienstes.

## Verification Levels

NovaOS kann unterschiedliche Prüftiefen verwenden:

```text
Basic
Functional
Integrity
Contract
Full
```

Die erforderliche Prüftiefe wird durch Recovery Policy, Criticality und Failure Class bestimmt.

Kritische Fehler dürfen keine unzureichende Verification verwenden.

## Post-Recovery Monitoring

Ein erfolgreicher Verification-Durchlauf kann durch eine Beobachtungsphase ergänzt werden.

```text
Verification Passed
        ↓
Monitoring Window
        ├── Stable → Recovered
        └── Failure → Reclassify
```

Dies reduziert falsche Recovery-Erfolge bei intermittierenden Fehlern.

## Verification Failure

Schlägt Verification fehl:

```text
Verification Failed
        ↓
Reclassify
        ↓
Recovery Policy
        ↓
Alternative Recovery
```

Mögliche Folgen:

```text
Retry Recovery
Restart
Failover
Rollback
Checkpoint Restore
Degradation
Recovery Mode
Fail-safe
```

## Verification Budget

Verification darf eigene Ressourcenbudgets besitzen:

```text
Time
CPU
Memory
IO
Network
Energy
```

Das Budget darf jedoch notwendige Sicherheits- oder Integritätsprüfungen nicht umgehen.

## Provenance

Verification-Ergebnisse müssen nachvollziehbar sein.

```text
What Was Verified
Expected State
Observed State
Checks Performed
Result
Failure Reason
Recovery Relation
```

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
VerificationID
RecoveryID
TargetID
Current State
Checks
Expected State
Observed State
Failed Checks
Execution Contract State
Health State
Final Result
```

## Normative Anforderungen

1. NovaOS MUSS Recovery Verification als eigenständige Phase unterstützen.
2. Eine abgeschlossene Recovery-Aktion DARF NICHT automatisch als erfolgreiche Recovery gelten.
3. Verification MUSS den wiederhergestellten Zustand gegen definierte Erwartungen prüfen.
4. Der Verification Scope MUSS kritische Abhängigkeiten berücksichtigen.
5. `Unknown` DARF bei erforderlichen Hard Guarantees NICHT als gültiger Zustand gelten.
6. Health, Integrity und State Consistency MÜSSEN getrennt prüfbar sein.
7. Capability Authority MUSS nach relevanter Recovery erneut validierbar sein.
8. Widerrufene oder abgelaufene Capabilities DÜRFEN Verification NICHT bestehen.
9. Kritische Dependencies MÜSSEN in die Recovery Verification einbezogen werden.
10. Execution Contracts MÜSSEN nach relevanten Recovery-Aktionen revalidierbar sein.
11. Realtime Verification MUSS zeitliche Guarantees berücksichtigen.
12. Distributed Verification MUSS Consistency, Membership und Split-Brain berücksichtigen.
13. Lokale Gesundheit DARF NICHT automatisch als globale Gesundheit interpretiert werden.
14. Die Verification-Tiefe MUSS anhand von Failure Class und Criticality bestimmbar sein.
15. Kritische Recovery DARF NICHT durch unzureichende Verification bestätigt werden.
16. Verification Failure MUSS erneute Classification oder Recovery-Eskalation ermöglichen.
17. Post-Recovery Monitoring SOLL für intermittierende oder kritische Fehler unterstützt werden.
18. Verification Budgets DÜRFEN notwendige Sicherheits- und Integritätsprüfungen NICHT umgehen.
19. Verification-Ergebnisse MÜSSEN nachvollziehbar sein.
20. Verification-Zustände und Ergebnisse MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-RESILIENCE-FAILUREDOMAIN-0001`
- `NPSPEC-RESILIENCE-CHECKPOINT-0001`
- `NPSPEC-RESILIENCE-ROLLBACK-0001`
- `NPSPEC-RESILIENCE-DEGRADATION-0001`
- `NPSPEC-RESILIENCE-RECOVERYMODE-0001`
- `NPSPEC-RESILIENCE-SELFHEALING-0001`
- `NPSPEC-RESILIENCE-RECOVERYPOLICY-0001`
- `NPSPEC-AUTONOMY-SELFDIAGNOSIS-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-REALTIME-DEADLINE-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `ADR-ARCH-0128`

## Ergebnis

```text
Recovery Action
      ↓
Determine Expected State
      ↓
Observe Actual State
      ↓
Verify
      ├── Valid
      │     ↓
      │   Monitor
      │     ↓
      │   Recovered
      │
      ├── Limited
      │     ↓
      │   Degraded
      │
      └── Invalid / Unknown
            ↓
         Reclassify
            ↓
         Escalate
```

NovaOS erhält damit eine verbindliche Recovery-Verification-Schicht, durch die ein System erst dann als wiederhergestellt gilt, wenn Zustand, Integrität, Abhängigkeiten, Authority und relevante Systemgarantien tatsächlich erneut validiert wurden.