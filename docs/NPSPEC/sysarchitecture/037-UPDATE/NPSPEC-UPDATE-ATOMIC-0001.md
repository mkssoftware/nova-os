# NPSPEC-UPDATE-ATOMIC-0001 – Nova Atomic Update

## Status

Angenommen

## Kategorie

Update / Atomicity / Transactional Update

## Zweck

NovaOS definiert atomare Updates, damit systemkritische Änderungen entweder vollständig in einen konsistenten neuen Zustand überführt oder kontrolliert verworfen bzw. zurückgesetzt werden.

```text
Old State
    ↓
Prepare Update
    ↓
Atomic Transition
   ↙       ↘
New State   Old State
```

Ein Fehler während eines Updates darf keinen undefinierten Mischzustand aus alter und neuer Version erzeugen.

## Grundprinzipien

```text
Atomic ≠ Instant
Atomic ≠ Global
Atomic ≠ Failure-Free
Installed ≠ Committed
Committed ≠ Verified
Staged ≠ Active
Rollback ≠ History Rewrite
Power Loss ≠ Undefined State
```

## Atomic Update Model

```text
AtomicUpdate
├── UpdateID
├── TransactionID
├── Scope
├── BaseState
├── TargetState
├── StagedChanges
├── CommitPoint
└── RecoveryPolicy
```

Optional:

```text
SnapshotID
ABSlot
Dependencies
StateMigrations
VerificationPlan
RollbackPlan
ResourceReservations
ProvenanceID
```

## Update Scope

Atomicität wird für einen expliziten Scope garantiert.

Beispiele:

```text
Single Component
Package Set
Service Group
Driver Set
Boot Environment
System Image
Configuration Set
```

```text
Atomic Scope ≠ Entire Distributed System
```

NovaOS vermeidet eine universelle globale Update-Transaktion.

## Phasen

```text
Begin
 ↓
Resolve
 ↓
Validate
 ↓
Stage
 ↓
Prepare
 ↓
Revalidate
 ↓
Commit
 ↓
Activate
 ↓
Verify
 ↓
Finalize
```

Vor dem Commit darf der bisher aktive Zustand weiterhin autoritativ bleiben.

## Staging

Neue Artefakte werden zunächst außerhalb des aktiven Zustands vorbereitet.

```text
Active State A
      +
Staged State B
```

Staging darf bestehende aktive Komponenten nicht unkontrolliert überschreiben.

## Prepare

Vor Commit werden mindestens geprüft:

```text
Package Integrity
Signature
Trust
Dependencies
Compatibility
State Versions
Capabilities
Resource Availability
Migration Readiness
Recovery Availability
```

Ein fehlgeschlagener Prepare-Schritt verhindert den Commit.

## Commit Point

Der Commit Point definiert den Übergang zum neuen autoritativen Zustand.

```text
State A
  ↓
Commit Point
  ↓
State B
```

Der Commit muss so gestaltet sein, dass nach Crash oder Stromausfall eindeutig bestimmt werden kann, welcher Zustand autoritativ ist.

## A/B Updates

Für bootkritische Updates soll A/B bevorzugt werden.

```text
Slot A = Active
Slot B = Staging

Update B
   ↓
Validate B
   ↓
Atomic Boot Selection
   ↓
Boot B
   ↓
Verify
```

Schlägt B fehl:

```text
B Unhealthy
    ↓
Return to A
```

## Filesystem Integration

Atomare Updates dürfen nutzen:

```text
Copy-on-Write
Snapshots
Atomic Rename
Versioned Objects
Transactional Metadata
Immutable Images
```

Die konkrete Technik hängt vom betroffenen Storage- und Update-Scope ab.

## State Migration

State Migration wird in die Update-Transaktion integriert.

```text
State v1
   ↓
Prepare Migration
   ↓
Commit Update
   ↓
State v2
   ↓
Verify
```

Irreversible Migrationen müssen explizit gekennzeichnet werden.

```text
Irreversible Migration
≠
Rollback Guaranteed
```

## Dependency Atomicity

Zusammengehörige Pakete müssen als konsistenter Update Set behandelt werden.

```text
Package A
Package B
Package C
    ↓
Atomic Update Set
```

Es darf kein Zustand committed werden, in dem harte Dependencies nur teilweise erfüllt sind.

## Crash Consistency

NovaOS muss Crashs während jeder Update-Phase behandeln können.

Nach Neustart muss bestimmt werden können:

```text
Not Committed
Committed
Verification Pending
Rollback Required
Recovery Required
Unknown
```

```text
Unknown ≠ Successful
```

## Power-Loss Safety

Ein Stromausfall darf den Update-Zustand nicht uninterpretierbar machen.

Persistente Commit-Metadaten müssen so geschrieben werden, dass der Boot- und Recovery-Pfad den letzten gültigen Zustand bestimmen kann.

## Verification

Commit beendet nicht die vollständige Update-Verifikation.

```text
Commit
  ↓
Activate
  ↓
Health Check
  ↓
Contract Verification
  ↓
Integrity Verification
  ↓
Operational Verification
```

```text
Committed ≠ Healthy
```

## Finalization

Erst nach erfolgreicher Verifikation wird das Update finalisiert.

Danach dürfen beispielsweise:

```text
Temporary Data
Old Staging Data
Transaction Metadata
Obsolete Artifacts
```

kontrolliert bereinigt werden.

Ein notwendiger Recovery-Pfad darf dabei nicht vorzeitig entfernt werden.

## Rollback

Scheitert die Aktivierung oder Verifikation:

```text
Failure
  ↓
Contain
  ↓
Rollback / A-B Fallback
  ↓
Verify Previous State
```

Rollback muss selbst konsistent und verifizierbar sein.

Security- und Revocation-State dürfen dabei nicht auf einen unsicheren früheren Zustand zurückgesetzt werden.

## Live Updates

Auch Live Replacement kann innerhalb eines atomaren Update-Scope erfolgen.

```text
Load New
 ↓
Validate
 ↓
Transfer State
 ↓
Atomic Switch
 ↓
Verify
 ↓
Retire Old
```

Die alte Komponente darf erst entfernt werden, wenn der Übergang ausreichend abgesichert ist.

## Concurrency

Während eines Updates können relevante Zustände verändert werden.

Deshalb müssen vor Commit kritische Versionen erneut geprüft werden.

```text
Expected StateVersion
        =
Current StateVersion?
```

Bei Konflikt:

```text
Abort
Replan
Retry
```

## Distributed Updates

Verteilte Updates besitzen keine implizite globale Atomarität.

NovaOS verwendet je nach System:

```text
Local Atomic Commit
Versioned Deployment
Staged Rollout
Compensation
Reconciliation
Consensus
```

Partielle Erreichbarkeit darf nicht als erfolgreicher globaler Commit interpretiert werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
UpdateID
TransactionID
Scope
BaseState
TargetState
Current Phase
Commit State
Verification State
Rollback Availability
Recovery State
```

## Normative Anforderungen

1. Systemkritische NovaOS-Updates MÜSSEN atomar ausführbar sein.
2. Atomicity MUSS einen expliziten Scope besitzen.
3. NovaOS DARF NICHT universelle globale Atomarität voraussetzen.
4. Änderungen MÜSSEN vor Commit stagingfähig sein.
5. Aktiver und gestagter Zustand MÜSSEN unterscheidbar bleiben.
6. Kritische Preconditions MÜSSEN vor Commit revalidiert werden.
7. Der Commit Point MUSS eindeutig definiert sein.
8. Nach Crash MUSS der autoritative Zustand bestimmbar sein.
9. Stromausfall DARF keinen undefinierten Mischzustand erzeugen.
10. Harte Dependencies MÜSSEN am Commit Point konsistent sein.
11. State Migration MUSS Bestandteil der Update-Transaktion sein können.
12. Irreversible Migrationen MÜSSEN explizit gekennzeichnet werden.
13. Bootkritische Updates SOLLEN A/B-Mechanismen verwenden.
14. Commit DARF NICHT automatisch als erfolgreiche Verifikation gelten.
15. Aktivierte Updates MÜSSEN nach Commit verifizierbar sein.
16. Fehlgeschlagene Verifikation MUSS Rollback oder Recovery auslösen können.
17. Rollback MUSS den resultierenden Zustand erneut verifizieren.
18. Rollback DARF aktuelle Security- oder Revocation-Zustände NICHT abschwächen.
19. Recovery-Artefakte DÜRFEN NICHT vor erfolgreicher Finalisierung entfernt werden.
20. Live Updates MÜSSEN einen kontrollierten Switch Point besitzen.
21. Concurrent State Changes MÜSSEN vor Commit erkannt werden können.
22. `Unknown` DARF NICHT als erfolgreicher Commit interpretiert werden.
23. Verteilte Updates DÜRFEN NICHT implizit globale Atomarität annehmen.
24. Update- und Commit-Zustand MÜSSEN persistent nachvollziehbar sein.
25. Atomic-Update-State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-PACKAGE-0001`
- `NPSPEC-UPDATE-DEPENDENCY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-STATE-TRANSACTIONAL-0001`
- `NPSPEC-STATE-SNAPSHOT-0001`
- `NPSPEC-STATE-VERSIONING-0001`
- `NPSPEC-STATE-ROLLBACK-0001`
- `NPSPEC-BOOT-AB-0001`
- `NPSPEC-BOOT-ROLLBACK-0001`
- `NPSPEC-BOOT-HEALTH-0001`
- `NPSPEC-RESILIENCE-RECOVERYPOLICY-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `ADR-ARCH-0168`

## Ergebnis

```text
Current State
     ↓
Stage Update
     ↓
Validate + Prepare
     ↓
Revalidate State
     ↓
Atomic Commit
    ↙      ↘
Success    Failure
   ↓          ↓
Activate    Abort
   ↓          ↓
Verify     Old State
   ↓
Healthy?
├── Yes → Finalize
└── No  → Rollback / Recovery
```

NovaOS erhält damit ein atomisches Update-Modell, bei dem kritische Änderungen vorbereitet, als konsistente Einheit aktiviert und anschließend verifiziert werden, während Crashs, Stromausfälle oder fehlerhafte Updates keinen undefinierten Systemzustand hinterlassen.