# NPSPEC-STATE-ROLLBACK-0001 – Nova State Rollback

## Status

Angenommen

## Kategorie

State / Rollback / Recovery

## Zweck

NovaOS definiert State Rollback als kontrollierte Wiederherstellung eines früheren bekannten Zustandsinhalts nach Fehlern, fehlgeschlagenen Änderungen oder expliziten Recovery-Entscheidungen.

```text
Current State
     ↓
Rollback Decision
     ↓
Select Historical State
     ↓
Validate
     ↓
Restore
     ↓
Create New State Version
     ↓
Verify
```

Rollback verändert die aktuelle State-Historie vorwärts und setzt Versionsnummern nicht zurück.

## Grundprinzipien

```text
Rollback ≠ Undo
Rollback ≠ Compensation
Rollback ≠ Snapshot Restore
Rollback ≠ Recovery
Rollback ≠ History Rewriting
Previous State ≠ Valid State
Restored ≠ Verified
Old Authority ≠ Current Authority
```

## Rollback Model

```text
StateRollback
├── RollbackID
├── StateID
├── CurrentVersion
├── TargetVersion
├── Source
├── Scope
├── Policy
└── State
```

Optional:

```text
SnapshotID
TransactionID
ExecutionContractID
RequiredCapabilities
Dependencies
Deadline
ProvenanceID
```

## Rollback Sources

Ein Rollback kann auf unterschiedlichen Quellen basieren:

```text
Historical State Version
Snapshot
Checkpoint
Transaction Log
Known-Good State
Configuration Version
A/B State
```

Die Quelle muss eindeutig identifizierbar und validierbar sein.

## Rollback Scope

NovaOS bevorzugt den kleinsten ausreichenden Scope.

```text
Object
Process
Service
Configuration
Resource
Subsystem
System
```

```text
Local Failure ≠ System-wide Rollback
```

## Reversibility

Vor Rollback muss die betroffene Änderung klassifiziert werden:

```text
Fully Reversible
Conditionally Reversible
Compensatable
Irreversible
Unknown
```

Irreversible externe Effekte können durch State Rollback allein nicht zurückgenommen werden.

## Version Semantik

Rollback erzeugt eine neue Version.

```text
v10 → v11 → v12
             ↓
Restore content of v10
             ↓
            v13
```

Nicht:

```text
v12 → v10
```

Damit bleibt die tatsächliche Historie erhalten.

## Validation

Vor der Wiederherstellung müssen mindestens geprüft werden:

```text
Source Integrity
State Compatibility
Dependencies
Capabilities
Security State
Trust State
Resource State
Current Version
```

Ein historischer Zustand darf nicht allein aufgrund seines Alters als gültig angenommen werden.

## Security State

State Rollback darf monotone Sicherheitsentscheidungen nicht rückgängig machen.

Geschützt sind insbesondere:

```text
Capability Revocation
Trust Revocation
Compromised Keys
Minimum Security Version
Security Counters
Rollback Protection
```

Beispiel:

```text
v10: Capability valid
v12: Capability revoked
Rollback content to v10
      ↓
Capability remains revoked
```

```text
State Rollback ≠ Authority Rollback
```

## Transactions

Rollback kann Bestandteil einer Transaction sein.

```text
Failure
   ↓
Abort
   ↓
Rollback State
   ↓
Verify
```

Für irreversible Operationen kann zusätzlich Compensation erforderlich sein.

## Concurrent Changes

Vor Anwendung eines Rollbacks muss geprüft werden, ob sich der aktuelle State seit der Entscheidung verändert hat.

```text
Rollback based on v20
        ↓
Current becomes v21
        ↓
Revalidate
```

Veraltete Rollback-Pläne dürfen neuere Änderungen nicht unbemerkt überschreiben.

## Dependencies

State kann von anderem State abhängig sein.

```text
State A rollback
      ↓
Dependency B changed
      ↓
Compatibility Check
```

Rollback kann deshalb Reconciliation weiterer Zustände auslösen.

## Distributed State

Verteilte Rollbacks benötigen das jeweilige Consistency Model.

```text
Node A → Rolled Back
Node B → Current
Node C → Unknown
```

NovaOS setzt keinen universellen globalen atomaren Rollback voraus.

Teilweise Rollbacks müssen explizit sichtbar bleiben.

## Failure During Rollback

Rollback selbst kann fehlschlagen.

```text
Rollback
   ↓
Failure
   ↓
Determine Actual State
   ↓
Recovery / Safe State / Escalation
```

```text
Rollback Failed ≠ Previous State Preserved
```

Kann der tatsächliche Zustand nicht bestimmt werden, wird er als `Unknown` behandelt.

## Verification

Nach Rollback muss der Actual State erneut beobachtet werden.

```text
Restore
   ↓
Observe
   ↓
Validate
   ↓
Verify
```

Erst danach kann der Rollback als erfolgreich abgeschlossen gelten.

## Reconciliation

Nach erfolgreichem Rollback kann Reconciliation notwendig sein.

```text
Rollback
   ↓
New Actual State
   ↓
Compare Desired State
   ↓
Reconcile
```

Der wiederhergestellte historische Zustand muss nicht dem aktuellen Desired State entsprechen.

## Rollback Loops

Wiederholte Rollbacks müssen begrenzt werden.

Mechanismen:

```text
Rollback Budget
Retry Limit
Cooldown
Failure Counter
Known-Good Escalation
Recovery Mode
```

NovaOS darf nicht unbegrenzt zwischen fehlerhaften Zuständen wechseln.

## Provenance

Kritische Rollbacks sollen erfassen:

```text
RollbackID
StateID
Previous Current Version
Rollback Source
Restored Content Version
New State Version
Reason
Actor
TransactionID
Verification Result
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
RollbackID
StateID
Current Version
Target Version
Source
Scope
Reversibility
Dependencies
Rollback State
Verification State
Failure Reason
```

## Normative Anforderungen

1. NovaOS MUSS kontrollierten State Rollback unterstützen können.
2. Rollback MUSS von Undo, Compensation und Recovery getrennt bleiben.
3. Rollback MUSS einen expliziten Scope besitzen.
4. Der kleinste ausreichende Rollback Scope SOLL bevorzugt werden.
5. Rollback Sources MÜSSEN eindeutig identifizierbar sein.
6. Rollback Sources MÜSSEN vor Verwendung validierbar sein.
7. Reversibility MUSS vor kritischem Rollback berücksichtigt werden.
8. Irreversible Effekte DÜRFEN NICHT als durch State Rollback rückgängig gemacht gelten.
9. Rollback MUSS eine neue State-Version erzeugen.
10. Historische Versionskennungen DÜRFEN NICHT als neue aktuelle Version wiederverwendet werden.
11. Rollback DARF State History NICHT überschreiben.
12. State Rollback DARF aktuelle Authority NICHT durch historische Authority ersetzen.
13. Monotone Security States DÜRFEN NICHT unzulässig zurückgesetzt werden.
14. Rollback MUSS mit Transactions integrierbar sein.
15. Irreversible Effekte MÜSSEN Compensation verwenden können.
16. Concurrent State Changes MÜSSEN vor Rollback revalidierbar sein.
17. Veraltete Rollback-Pläne DÜRFEN neueren State NICHT unbemerkt überschreiben.
18. State Dependencies MÜSSEN berücksichtigt werden.
19. Distributed Rollback DARF keinen universellen globalen atomaren Rollback voraussetzen.
20. Teilweise Rollbacks MÜSSEN explizit darstellbar sein.
21. Rollback Failure MUSS den Actual State erneut bestimmen oder als Unknown markieren.
22. Nach Rollback MUSS der Actual State verifiziert werden.
23. Rollback Loops MÜSSEN begrenzt werden.
24. Kritische Rollbacks SOLLEN Provenance besitzen.
25. State Rollback MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STATE-GLOBAL-0001`
- `NPSPEC-STATE-DESIRED-0001`
- `NPSPEC-STATE-ACTUAL-0001`
- `NPSPEC-STATE-MACHINE-0001`
- `NPSPEC-STATE-RECONCILIATION-0001`
- `NPSPEC-STATE-SNAPSHOT-0001`
- `NPSPEC-STATE-VERSIONING-0001`
- `NPSPEC-STATE-HISTORY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-TRANSACTION-ROLLBACK-0001`
- `NPSPEC-TRANSACTION-COMPENSATION-0001`
- `NPSPEC-RESILIENCE-ROLLBACK-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `ADR-ARCH-0161`

## Ergebnis

```text
Failure / Rollback Decision
          ↓
Select Rollback Scope
          ↓
Select Historical Source
          ↓
Validate Integrity + Compatibility
          ↓
Validate Security + Authority
          ↓
Restore State Content
          ↓
Create New State Version
          ↓
Verify Actual State
          ↓
Reconcile Desired State
```

NovaOS erhält damit ein kontrolliertes State-Rollback-Modell, das frühere Zustandsinhalte wiederherstellen kann, ohne Historie oder Versionsfolge zurückzusetzen, aktuelle Sicherheitsentscheidungen zu umgehen oder einen wiederhergestellten Zustand ungeprüft als gültig anzunehmen.