# NPSPEC-UPDATE-ROLLING-0001 – Nova Rolling Update

## Status

Angenommen

## Kategorie

Update / Distributed Systems / Rolling Deployment

## Zweck

NovaOS definiert Rolling Updates für replizierte, verteilte oder mehrfach bereitgestellte Komponenten. Instanzen werden schrittweise aktualisiert, während ausreichend funktionsfähige Instanzen der bisherigen oder neuen Version verfügbar bleiben.

```text
v1 v1 v1 v1
      ↓
v2 v1 v1 v1
      ↓
v2 v2 v1 v1
      ↓
v2 v2 v2 v1
      ↓
v2 v2 v2 v2
```

## Grundprinzipien

```text
Rolling Update ≠ Atomic Global Update
Instance Updated ≠ Deployment Updated
Available ≠ Healthy
Healthy ≠ Compatible
Mixed Versions ≠ Automatically Safe
Partial Success ≠ Update Success
Rollback ≠ Instant Global Reversal
```

## Rolling Update Model

```text
RollingUpdate
├── UpdateID
├── DeploymentID
├── TargetVersion
├── InstanceSet
├── BatchPolicy
├── AvailabilityPolicy
├── CompatibilityPolicy
├── VerificationPolicy
└── State
```

Optional:

```text
MinimumHealthy
MaximumUnavailable
MaximumSurge
BatchSize
PausePolicy
RollbackPolicy
ResourceBudget
ExecutionContract
ProvenanceID
```

## Deployment States

```text
Planned
Preparing
Rolling
Paused
Verifying
Completed
RollingBack
Failed
Blocked
Unknown
```

`Unknown` darf nicht als erfolgreicher Zustand interpretiert werden.

## Batch Update

Instanzen werden in kontrollierten Gruppen aktualisiert.

```text
Select Batch
    ↓
Drain
    ↓
Update
    ↓
Activate
    ↓
Verify
    ↓
Next Batch
```

Die nächste Gruppe darf erst entsprechend der definierten Policy fortgesetzt werden.

## Availability

Während des Rollouts muss die notwendige Mindestverfügbarkeit erhalten bleiben.

```text
HealthyInstances
>=
MinimumHealthy
```

NovaOS muss vor jedem Batch prüfen, ob dessen Aktualisierung die zugesicherte Verfügbarkeit gefährden würde.

## Mixed-Version Operation

Während eines Rolling Updates existieren zeitweise mehrere Versionen gleichzeitig.

```text
Provider v1
Provider v1
Provider v2
Provider v2
```

Daher müssen insbesondere geprüft werden:

```text
API Compatibility
ABI Compatibility
Protocol Compatibility
State Compatibility
IPC Compatibility
Schema Compatibility
Capability Compatibility
```

```text
Individually Compatible
≠
Mutually Compatible
```

## Compatibility Window

Komponenten können ein explizites Kompatibilitätsfenster definieren.

Beispiel:

```text
v4 ↔ v5 = Supported
v3 ↔ v5 = Unsupported
```

Ein Rolling Update darf keine nicht unterstützte Versionskombination erzeugen.

## Traffic und Workload Drain

Vor dem Update einer Instanz können neue Arbeiten von ihr entfernt werden.

```text
Active Instance
      ↓
Stop New Work
      ↓
Drain Existing Work
      ↓
Update
```

Structured Concurrency muss laufende Tasks kontrolliert behandeln.

## Provider Discovery

Service- und Capability-Discovery müssen Versionsänderungen berücksichtigen.

```text
Stable CapabilityID
        ↓
Provider Pool
├── v1
├── v1
├── v2
└── v2
```

Providerwechsel dürfen Capability-Grenzen nicht verändern.

## Health Verification

Nach jedem Batch muss der neue Zustand überprüfbar sein.

```text
Update Batch
    ↓
Health Check
    ↓
Contract Check
    ↓
Compatibility Check
    ↓
Operational Verification
```

Bei kritischen Fehlern darf der nächste Batch nicht automatisch gestartet werden.

## Pause

Rolling Updates müssen pausierbar sein.

Auslöser können sein:

```text
Health Degradation
Error Rate Increase
Contract Violation
Resource Pressure
Dependency Failure
Security Event
Manual Decision
```

```text
Pause ≠ Failure
```

Nach erneuter Validierung kann der Rollout fortgesetzt, zurückgerollt oder abgebrochen werden.

## Rollback

Rollback erfolgt ebenfalls kontrolliert über Instanzen oder Batches.

```text
v2 v2 v1 v1
      ↓
Failure
      ↓
v1 v2 v1 v1
      ↓
v1 v1 v1 v1
```

Vor jedem Rollback müssen State-, Dependency- und Security-Kompatibilität erneut geprüft werden.

## State Migration

Rolling Updates mit gemeinsam genutztem State benötigen migrationsfähige Schemas.

Bevorzugt werden Übergänge wie:

```text
Expand
  ↓
Mixed-Version Operation
  ↓
Migrate
  ↓
Contract
```

Neue State-Strukturen sollen während der Übergangsphase mit den noch laufenden alten Instanzen kompatibel bleiben.

## Irreversible Migration

Ist eine Migration nicht rückwärtskompatibel, kann Rolling Update ungeeignet sein.

Dann muss NovaOS auf einen anderen Update-Pfad ausweichen, beispielsweise:

```text
Atomic Update
A/B Update
Immutable Generation
Coordinated Downtime
Recovery Migration
```

## Distributed State

Rolling Updates dürfen keine globale ACID-Semantik voraussetzen.

```text
Node A → v2
Node B → v2
Node C → v1
```

Partielle Zustände sind normal und müssen explizit modelliert werden.

## Resource Management

Während eines Rollouts können zusätzliche Instanzen gestartet werden.

```text
Current Capacity
      +
Temporary Surge
```

NovaOS muss CPU, Memory, Storage, Network, Energy und weitere Resource Budgets berücksichtigen.

## Security

Jede aktualisierte Instanz muss weiterhin die normalen Update-Prüfungen durchlaufen:

```text
Integrity
Signature
Trust
Authorization
Compatibility
Security Policy
```

Alte Instanzen dürfen nicht weiterbetrieben werden, wenn eine Security Policy deren Version explizit verbietet.

## Transaction Integration

Jeder Batch kann als lokale Update-Transaktion behandelt werden.

```text
Global Rolling Plan
        ↓
Batch Transaction 1
        ↓
Verify
        ↓
Batch Transaction 2
        ↓
Verify
```

Es existiert dabei nicht automatisch eine globale atomare Transaktion.

## Failure Handling

Mögliche Reaktionen:

```text
Retry Instance
Replace Instance
Pause Rollout
Rollback Batch
Rollback Deployment
Failover
Reconcile
Recovery
```

Die kleinstmögliche ausreichende Recovery-Ebene soll bevorzugt werden.

## Provenance

NovaOS soll nachvollziehen können:

```text
UpdateID
DeploymentID
InstanceID
OldVersion
NewVersion
BatchID
StartTime
CompletionTime
HealthResult
RollbackResult
FailureReason
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Deployment Version Distribution
Target Version
Current Batch
Updated Instances
Pending Instances
Healthy Instances
Unavailable Instances
Compatibility State
Rollout State
Rollback State
Failure Reason
```

## Normative Anforderungen

1. NovaOS MUSS Rolling Updates für geeignete replizierte oder verteilte Komponenten unterstützen können.
2. Rolling Updates DÜRFEN keine globale Atomicity voraussetzen.
3. Instanzen MÜSSEN in kontrollierten Batches aktualisierbar sein.
4. Mindestverfügbarkeit MUSS während des Rollouts berücksichtigt werden.
5. Ein neuer Batch DARF harte Availability Constraints NICHT verletzen.
6. Mixed-Version-Betrieb MUSS explizit berücksichtigt werden.
7. API-, ABI-, Protocol-, State- und Capability-Kompatibilität MÜSSEN bei Bedarf geprüft werden.
8. Nicht unterstützte Versionskombinationen DÜRFEN NICHT erzeugt werden.
9. Laufende Workloads MÜSSEN kontrolliert drainbar oder behandelbar sein.
10. Capability Authority DARF durch Providerwechsel NICHT implizit erweitert werden.
11. Jeder Batch MUSS verifizierbar sein.
12. Kritische Verifikationsfehler MÜSSEN weitere Batches blockieren können.
13. Rolling Updates MÜSSEN pausierbar sein.
14. Rollback MUSS batchweise oder deploymentsweit möglich sein können.
15. Rollback MUSS aktuelle Security Constraints berücksichtigen.
16. State Migration MUSS Mixed-Version-Betrieb berücksichtigen.
17. Irreversible inkompatible Migrationen MÜSSEN erkannt werden.
18. Ungeeignete Updates MÜSSEN auf einen anderen Update-Pfad ausweichen können.
19. Partielle verteilte Zustände MÜSSEN explizit modelliert werden.
20. Temporärer Ressourcenmehrbedarf MUSS berücksichtigt werden.
21. Jede Instanz MUSS den normalen Integrity-, Trust- und Authorization-Regeln unterliegen.
22. Lokale Batch-Updates SOLLEN transaktional ausgeführt werden.
23. Fehler SOLLEN auf kleinstmöglicher ausreichender Ebene behandelt werden.
24. `Unknown` DARF NICHT als erfolgreicher Rollout interpretiert werden.
25. Rolling-Update-Entscheidungen MÜSSEN nachvollziehbare Provenance besitzen.
26. Rollout-State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-DEPENDENCY-0001`
- `NPSPEC-UPDATE-TRANSACTIONAL-0001`
- `NPSPEC-UPDATE-ROLLBACK-0001`
- `NPSPEC-UPDATE-LIVE-0001`
- `NPSPEC-DISTRIBUTED-EXECUTION-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-0001`
- `NPSPEC-DISTRIBUTED-TRANSACTION-0001`
- `NPSPEC-DISTRIBUTED-PLACEMENT-0001`
- `NPSPEC-CAPABILITY-PROVIDER-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `ADR-ARCH-0178`

## Ergebnis

```text
Deployment v1
      ↓
Plan Rollout
      ↓
Select Batch
      ↓
Drain + Update
      ↓
Verify
     ↙   ↘
   Good   Failure
    ↓       ↓
Next Batch Pause /
    ↓      Rollback
Repeat
    ↓
All Instances v2
    ↓
Final Verification
    ↓
Complete
```

NovaOS erhält damit einen kontrollierten Rolling-Update-Mechanismus für verteilte und replizierte Komponenten, bei dem neue Versionen schrittweise ausgerollt werden können, ohne Verfügbarkeit, Versionskompatibilität, Capability-Grenzen oder Recovery-Eigenschaften unkontrolliert zu gefährden.