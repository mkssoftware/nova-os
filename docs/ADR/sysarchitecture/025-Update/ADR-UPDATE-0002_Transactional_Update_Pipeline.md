# ADR-UPDATE-0002 – Transactional Update Pipeline

## Status

Angenommen

## Kategorie

Update Architecture / Transactions / Software Lifecycle

## Kontext

Durch `ADR-UPDATE-0001_Deklarativer_Softwarezustand` werden Softwareupdates in NovaOS als Änderungen des gewünschten Softwarezustands beschrieben.

Die tatsächliche Umsetzung eines Updates umfasst jedoch mehrere voneinander abhängige Schritte:

```text
Resolve
Download
Verify
Prepare
Install
Activate
Verify
Cleanup
```

Ein Fehler während dieser Pipeline darf das laufende System nicht in einem teilweise aktualisierten oder inkonsistenten Zustand zurücklassen.

Typische Fehler sind:

- unterbrochener Download,
- beschädigte Update-Daten,
- ungültige Signatur,
- fehlende Abhängigkeiten,
- Ressourcenmangel,
- Storage-Fehler,
- Stromausfall,
- fehlgeschlagene Aktivierung,
- inkompatible Komponenten,
- fehlgeschlagene Post-Update-Verifikation.

NovaOS benötigt deshalb eine transaktionale Update-Pipeline.

## Entscheidung

NovaOS führt Softwareupdates als **Transactional Update Pipeline** aus.

```text
Update Intent
     ↓
Resolve
     ↓
Acquire
     ↓
Verify
     ↓
Prepare
     ↓
Validate
     ↓
Commit
     ↓
Activate
     ↓
Verify
     ↓
Complete
```

Fehler vor dem Commit dürfen den bisher aktiven Softwarezustand nicht beschädigen.

Fehler nach dem Commit werden über Rollback, Compensation oder Recovery behandelt.

## Grundprinzip

```text
Prepare first.
Commit atomically.
Activate safely.
Verify before completion.
```

Ein Update gilt erst dann als vollständig abgeschlossen, wenn der resultierende Zustand erfolgreich verifiziert wurde.

## Update Intent

Ausgangspunkt ist eine Änderung des Desired Software State.

```text
Desired State V1
       ↓
Update Intent
       ↓
Desired State V2
```

Die Pipeline ermittelt daraus die notwendigen technischen Änderungen.

## Resolve

In der Resolve-Phase werden die benötigten Komponenten und Abhängigkeiten bestimmt.

```text
Desired State
     ↓
Resolve
     ↓
Update Plan
```

Der Update Plan kann enthalten:

- neue Komponenten,
- zu ersetzende Komponenten,
- Abhängigkeiten,
- Provider,
- Konfigurationsänderungen,
- Datenmigrationen,
- Ressourcenanforderungen.

## Acquire

Benötigte Update-Artefakte werden zunächst beschafft, ohne den aktiven Softwarezustand zu verändern.

```text
Update Plan
     ↓
Acquire
     ↓
Staging Area
```

Unterbrochene Downloads dürfen keinen teilweise installierten Zustand erzeugen.

## Verify

Alle sicherheits- und integritätsrelevanten Artefakte müssen vor ihrer Aktivierung verifiziert werden.

Dazu können gehören:

```text
Signature
Hash
Provenance
Trust Policy
Compatibility
Manifest
Dependencies
```

Nicht vertrauenswürdige oder beschädigte Artefakte dürfen nicht aktiviert werden.

## Prepare

Das Update wird außerhalb des aktiven Zustands vorbereitet.

```text
Active State
     │
     ├───────────────┐
     │               ↓
     │          Prepared State
     │               ↓
     └─────────── Commit
```

Dadurch bleibt der bisherige Zustand während der Vorbereitung nutzbar.

## Validation

Vor Commit muss geprüft werden, ob der vorbereitete Zustand die erforderlichen Systembedingungen erfüllt.

Dazu gehören insbesondere:

```text
Dependencies
Resource Requirements
Security Policies
Trust Requirements
Storage Requirements
Data Sovereignty
Execution Contracts
System Invariants
```

Ein Update darf nicht committed werden, wenn Hard Constraints verletzt sind.

## Resource Reservation

Benötigte Ressourcen können vor Commit über eine Resource Transaction reserviert werden.

```text
Prepared Update
      ↓
Resource Transaction
      ↓
Resources Available?
```

Dadurch soll verhindert werden, dass ein Update committed wird, dessen notwendige Ressourcen offensichtlich nicht verfügbar sind.

## Commit

Der Commit bildet den atomaren Wechsel auf den neuen deklarativen Softwarezustand.

```text
Software State N
       ↓
Atomic Commit
       ↓
Software State N+1
```

Der Commit muss über das systemweite Transaction-Modell erfolgen.

## Aktivierung

Nach Commit werden die vorbereiteten Komponenten kontrolliert aktiviert.

Je nach Komponente kann dies erfolgen durch:

```text
Hot Replacement
Service Restart
Process Restart
Provider Switch
Reboot
A/B Boot
```

Die notwendige Aktivierungssemantik muss explizit beschreibbar sein.

## Hot Replacement

Unterstützt eine Komponente Hot Replacement, kann der Wechsel ohne vollständigen Neustart erfolgen.

```text
Provider A
    ↓
Prepare Provider B
    ↓
Atomic Switch
    ↓
Provider B
```

Der alte Provider darf erst freigegeben werden, wenn der neue Zustand ausreichend hergestellt wurde.

## Reboot Required

Bestimmte Updates können einen Neustart benötigen.

```text
Prepare Update
      ↓
Commit Boot Target
      ↓
Reboot
      ↓
Boot New State
      ↓
Verify
```

Der Neustart ist Teil der Transaction-Recovery-Semantik und darf nicht als unkontrollierte Unterbrechung behandelt werden.

## A/B Update

Für kritische Systemkomponenten kann NovaOS ein A/B-Modell verwenden.

```text
Active A
   ↓
Prepare B
   ↓
Verify B
   ↓
Boot B
   ↓
Health Verification
   ↓
Mark B Good
```

Schlägt die Verifikation fehl:

```text
Boot B
  ↓
Failure
  ↓
Rollback
  ↓
Boot A
```

A/B ist eine mögliche Implementierungsstrategie und keine Voraussetzung für jede Softwarekomponente.

## Rollback Barrier

Irreversible Schritte müssen durch eine explizite Rollback Barrier gekennzeichnet werden.

```text
Prepare
   ↓
Rollbackfähig
   ↓
Rollback Barrier
   ↓
Irreversible Operation
```

Solche Operationen sollen möglichst spät innerhalb der Pipeline ausgeführt werden.

## Datenmigrationen

Softwareupdates können Datenmigrationen benötigen.

```text
Old Software
     +
Old Data Format
      ↓
Migration
      ↓
New Software
     +
New Data Format
```

Datenmigrationen müssen ihre:

```text
Rollback
Compensation
Compatibility
Versioning
Recovery
```

explizit beschreiben.

Irreversible Migrationen benötigen eine Rollback Barrier.

## Verification

Nach Aktivierung muss der neue Zustand verifiziert werden.

```text
Activate
   ↓
Health Checks
   ↓
Contract Checks
   ↓
System Verification
   ↓
Mark Update Complete
```

Ein erfolgreicher Start allein reicht nicht als vollständige Update-Verifikation.

## Health Milestones

Für kritische Updates können mehrere Health Milestones verwendet werden.

Beispiel:

```text
Kernel started
      ↓
Storage available
      ↓
Core services available
      ↓
Security services available
      ↓
User session available
      ↓
Update verified
```

Erst nach Erreichen der erforderlichen Milestones darf der neue Zustand als stabil gelten.

## Failure vor Commit

Fehler vor Commit führen zum Abbruch der vorbereiteten Transaction.

```text
Prepare
   ↓
Failure
   ↓
Abort
   ↓
Active State unchanged
```

Temporäre Ressourcen und Staging-Daten werden anschließend kontrolliert freigegeben.

## Failure nach Commit

Fehler nach Commit lösen Recovery aus.

```text
Commit
   ↓
Activate
   ↓
Failure
   ↓
Rollback / Compensation / Recovery
```

Die konkrete Strategie hängt von der Operationssemantik ab.

## Power Failure

Die Pipeline muss gegen Power Failure abgesichert sein.

```text
Update
   ↓
Power Failure
   ↓
Boot
   ↓
Transaction Log
   ↓
Determine Update State
   ↓
Continue / Rollback / Recover
```

NovaOS darf nach einem Neustart keinen Updatezustand allein aus flüchtigen Informationen ableiten.

## Update State

Die Pipeline muss mindestens folgende logische Zustände unterscheiden können:

```text
Planned
Acquiring
Verified
Prepared
Committed
Activating
Verifying
Completed
Failed
RecoveryRequired
```

Die konkrete Zustandsmaschine wird in einer NPSPEC definiert.

## Parallel Updates

Unabhängige Updates können parallel vorbereitet werden.

```text
Update A ── Prepare
Update B ── Prepare
Update C ── Prepare
```

Konfligierende Updates dürfen jedoch nicht unabhängig committed werden.

Abhängigkeiten und gemeinsame Ressourcen müssen vor Commit berücksichtigt werden.

## Update Bundles

Mehrere zusammengehörige Komponenten können als gemeinsames Update Change Set behandelt werden.

```text
Update Transaction
 ├── Kernel
 ├── Driver
 ├── Service
 └── Configuration
```

Wenn diese Komponenten gemeinsam konsistent sein müssen, werden sie unter einem gemeinsamen Transaction Scope aktualisiert.

## Capability Provider

Provider können während eines Updates ausgetauscht werden.

```text
Capability
    ↓
Provider A
    ↓
Update Transaction
    ↓
Provider B
```

Der neue Provider muss vor Aktivierung die erforderlichen Capability-, Trust- und Execution Contracts erfüllen.

## Security

Die Update Pipeline darf keine Security-Grenzen umgehen.

Insbesondere müssen gelten:

```text
Authorization
Signature Verification
Trust Policy
Capability Checks
Integrity Verification
Data Sovereignty
```

Ein Update besitzt keine automatische administrative Authority.

## Supply Chain

Update-Artefakte müssen mit der systemweiten Software- und Supply-Chain-Provenance integrierbar sein.

NovaOS muss nachvollziehen können:

```text
Artifact
   ↓
Source
   ↓
Build Provenance
   ↓
Signature
   ↓
Trust Decision
```

Eine gültige Signatur allein bedeutet nicht automatisch Vertrauen.

## Recovery

Nova.Resilience muss den Zustand einer unterbrochenen Update Pipeline erkennen und behandeln können.

```text
Interrupted Update
       ↓
Transaction State
       ↓
Classify
       ↓
Continue / Rollback / Recover
       ↓
Verify
```

Recovery darf keinen größeren Scope verwenden als notwendig.

## NovaDOS

Kann der normale Systemzustand nicht mehr gestartet werden, muss NovaDOS kritische Update Transactions offline analysieren können.

NovaDOS kann abhängig vom Recovery Contract:

```text
Inspect
Verify
Rollback
Select Previous Boot State
Repair Transaction State
```

durchführen.

## Cleanup

Alte Update-Artefakte und vorherige Versionen dürfen erst entfernt werden, wenn sie nicht mehr für:

```text
Rollback
Recovery
Audit
Retention
```

benötigt werden.

Cleanup ist damit ein nachgelagerter Schritt und darf die Recovery-Fähigkeit nicht vorzeitig zerstören.

## Normative Anforderungen

1. NovaOS MUSS Softwareupdates über eine transaktionale Update Pipeline ausführen können.
2. Die Pipeline MUSS auf dem deklarativen Softwarezustand basieren.
3. Update-Artefakte MÜSSEN vor Aktivierung verifiziert werden.
4. Updates SOLLEN außerhalb des aktiven Zustands vorbereitet werden.
5. Hard Constraints MÜSSEN vor Commit validiert werden.
6. Der Wechsel des Desired Software State MUSS transaktional erfolgen.
7. Fehler vor Commit DÜRFEN den aktiven Softwarezustand nicht beschädigen.
8. Fehler nach Commit MÜSSEN Rollback, Compensation oder Recovery auslösen können.
9. Ein Update DARF erst nach erfolgreicher Verification als abgeschlossen gelten.
10. Kritische Updates MÜSSEN Health Milestones verwenden können.
11. Updates MÜSSEN unterschiedliche Aktivierungsarten wie Hot Replacement, Restart oder Reboot unterstützen können.
12. Kritische Systemupdates MÜSSEN A/B- oder vergleichbare rollbackfähige Strategien unterstützen können.
13. Irreversible Update-Schritte MÜSSEN durch Rollback Barriers gekennzeichnet werden.
14. Datenmigrationen MÜSSEN ihre Rollback-, Compensation- und Recovery-Semantik deklarieren.
15. Die Pipeline MUSS gegen Power Failure recoverbar sein.
16. Persistente Update-Zustände MÜSSEN über den Transaction Log rekonstruierbar sein.
17. Konfligierende parallele Updates MÜSSEN erkannt werden.
18. Zusammengehörige Komponenten MÜSSEN als gemeinsamer Update Transaction Scope behandelbar sein.
19. Provider-Wechsel MÜSSEN vor Aktivierung auf ihre Contracts geprüft werden.
20. Security-, Trust-, Integrity- und Data-Sovereignty-Hard-Constraints MÜSSEN während der gesamten Pipeline gelten.
21. Supply-Chain-Provenance MUSS in die Update-Verifikation integrierbar sein.
22. Cleanup DARF notwendige Rollback- oder Recovery-Daten nicht vorzeitig entfernen.
23. Kritische fehlgeschlagene Updates MÜSSEN über NovaDOS offline recoverbar sein können.
24. Update Plan, Transaction State, Commit, Activation, Verification und Recovery MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Updates hinterlassen keine unkontrollierten Teilzustände,
- Update und Recovery verwenden dieselbe Transaction Architecture,
- Power Failure kann kontrolliert behandelt werden,
- kritische Updates können automatisch zurückgerollt werden,
- Supply-Chain- und Trust-Prüfungen erfolgen vor Aktivierung,
- Datenmigrationen werden explizit in die Update-Semantik integriert.

### Negative Konsequenzen

- Updates benötigen zusätzlichen Staging-Speicher,
- alte Versionen müssen zeitweise erhalten bleiben,
- Update-Komponenten benötigen explizite Activation- und Recovery-Contracts,
- komplexe Datenmigrationen können die Pipeline verlängern.

## Verworfene Alternativen

### Dateien direkt im laufenden System überschreiben

Verworfen, da Fehler oder Power Failure einen inkonsistenten Mischzustand erzeugen können.

### Update gilt nach Installation als abgeschlossen

Verworfen, da Installation nicht beweist, dass der neue Zustand funktionsfähig ist.

### Immer vollständiges Systemupdate

Verworfen, da dies unnötig große Transaction- und Recovery-Scopes erzeugt.

### Immer A/B für jede Software

Verworfen, da dies für kleine oder problemlos austauschbare Komponenten unnötigen Storage-Aufwand erzeugt.

### Alte Version sofort nach Commit löschen

Verworfen, da dadurch Rollback und Recovery unnötig verhindert würden.

## Abhängigkeiten

- `ADR-UPDATE-0001_Deklarativer_Softwarezustand`
- `ADR-TRANSACTION-0001_System_Transactions`
- `ADR-TRANSACTION-0002_Atomic_Desired_State_Changes`
- `ADR-TRANSACTION-0004_Resource_Transactions`
- `ADR-TRANSACTION-0005_Multi-Service_Transactions`
- `ADR-TRANSACTION-0006_Transaction_Log_und_Power_Failure_Safety`
- `ADR-TRANSACTION-0007_Rollback_und_Compensation`
- `ADR-TRANSACTION-0008_Irreversible_Operationen_mit_Rollback_Barrier`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-TRUST-0003_Signatur_ist_nicht_Vertrauen`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`
- `ADR-TRUST-0011_Reproducible_Builds_als_Trust_Evidence`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-RESILIENCE-0022_NovaDOS_für_Offline_Recovery`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-TRANSACTION-0001`
- `NPSPEC-UPDATE-PIPELINE-0001`
- `NPSPEC-UPDATE-PLAN-0001`
- `NPSPEC-UPDATE-STAGING-0001`
- `NPSPEC-UPDATE-VERIFICATION-0001`
- `NPSPEC-UPDATE-ACTIVATION-0001`
- `NPSPEC-UPDATE-HEALTH-MILESTONE-0001`
- `NPSPEC-UPDATE-ROLLBACK-0001`
- `NPSPEC-UPDATE-DATA-MIGRATION-0001`
- `NPSPEC-UPDATE-POWER-FAILURE-0001`
- `NPSPEC-UPDATE-RECOVERY-0001`
- `NPSPEC-UPDATE-PIPELINE-TEST-0001`

## Ergebnis

NovaOS führt Updates als vorbereitete, verifizierte und recoverbare System Transactions aus:

```text
Desired State Change
        ↓
Resolve
        ↓
Acquire
        ↓
Verify
        ↓
Prepare
        ↓
Validate
        ↓
Commit
        ↓
Activate
        ↓
Verify
        ↓
Complete
```

Fehler vor dem Commit lassen den bisherigen Zustand unangetastet.

Fehler nach dem Commit führen kontrolliert zu:

```text
Rollback
Compensation
Recovery
Reconciliation
```

Ein Update gilt erst dann als erfolgreich abgeschlossen, wenn der neue Zustand tatsächlich aktiviert und verifiziert wurde.