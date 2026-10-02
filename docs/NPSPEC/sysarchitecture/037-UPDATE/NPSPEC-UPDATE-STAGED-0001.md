# NPSPEC-UPDATE-STAGED-0001 – Nova Staged Update

## Status

Angenommen

## Kategorie

Update / Staging / Deployment Control

## Zweck

NovaOS definiert Staged Updates als kontrollierte Vorbereitung eines Updates außerhalb des aktiven Systemzustands.

Pakete, Artefakte, Abhängigkeiten, Migrationen und Aktivierungsdaten werden vollständig vorbereitet und validiert, bevor sie Auswirkungen auf das laufende System erhalten.

```text
Acquire
  ↓
Validate
  ↓
Stage
  ↓
Verify
  ↓
Ready
  ↓
Activate
```

## Grundprinzipien

```text
Downloaded ≠ Staged
Staged ≠ Installed
Installed ≠ Active
Ready ≠ Authorized to Activate
Validated ≠ Verified Runtime
Staging ≠ Commit
Staging ≠ User-Visible Change
```

## Staging Model

```text
StagedUpdate
├── StagingID
├── UpdateID
├── TransactionID
├── Target
├── BaseVersion
├── TargetVersion
├── Artifacts
├── Dependencies
└── State
```

Optional:

```text
ContentIDs
SnapshotID
TargetSlot
TargetGeneration
MigrationPlan
ActivationPolicy
ResourceReservation
VerificationPlan
ProvenanceID
```

## Staging States

```text
Created
Downloading
Validating
Preparing
Staged
Verified
Ready
Activating
Consumed
Invalid
Failed
Expired
Unknown
```

`Unknown` darf nicht als aktivierungsbereit gelten.

## Staging Area

Staged Updates müssen logisch vom aktiven Systemzustand getrennt sein.

```text
Active System
     │
     │ unaffected
     │
Staging Area
├── Packages
├── Components
├── Metadata
├── Migration Data
└── Activation Data
```

Unvollständige Updates dürfen den aktiven Zustand nicht verändern.

## Vorbereitung

Während Staging können bereits durchgeführt werden:

```text
Download
Content Verification
Signature Verification
Trust Validation
Dependency Resolution
Compatibility Checks
Delta Reconstruction
Generation Construction
Migration Preparation
Resource Planning
```

Dadurch wird die kritische Aktivierungsphase möglichst klein gehalten.

## Content Verification

Alle Artefakte müssen gegen ihre erwarteten ContentIDs geprüft werden.

```text
Downloaded Content
       ↓
Hash
       ↓
Expected ContentID
       ↓
Staging Accepted
```

Beschädigte oder unerwartete Inhalte dürfen nicht als `Ready` markiert werden.

## Dependency Preparation

Der vollständige benötigte Update Set soll vor Aktivierung verfügbar sein.

```text
Package A
├── Requires B
└── Requires C

Staging
├── A
├── B
└── C
```

Fehlende harte Dependencies blockieren die Aktivierung.

## Revalidation

Zwischen Staging und Aktivierung kann sich der Systemzustand ändern.

Deshalb gilt:

```text
Stage
 ↓
Time Passes
 ↓
System Changes
 ↓
Revalidate
 ↓
Activate
```

Vor Aktivierung müssen dynamische Bedingungen erneut geprüft werden.

Dazu gehören insbesondere:

```text
Current State Version
Capabilities
Trust State
Revocations
Dependencies
Resources
Security Policy
Sovereignty Policy
```

## Activation Policy

Staging und Aktivierung bleiben getrennte Entscheidungen.

Mögliche Aktivierungsarten:

```text
Immediate
Scheduled
Next Restart
Next Boot
A/B Switch
Live Update
Hotpatch
Manual
Maintenance Window
```

```text
Ready ≠ Activate Now
```

## Atomic Integration

Nach erfolgreichem Staging kann die eigentliche Aktivierung atomar erfolgen.

```text
Verified Staging
      ↓
Prepare Commit
      ↓
Atomic Activation
```

Das reduziert die Menge an Arbeit innerhalb des kritischen Commit-Fensters.

## Transaction Integration

Staging gehört zur Prepare-Phase der Update-Transaktion.

```text
Begin
 ↓
Validate
 ↓
Stage
 ↓
Prepared
 ↓
Revalidate
 ↓
Commit
```

Ein verworfener Staging-Bereich darf keinen erfolgreichen Commit vortäuschen.

## A/B Integration

Bei A/B-Systemen kann der inaktive Slot als Staging-Ziel dienen.

```text
Active A
   │
   └── running

Inactive B
   ↓
Stage Update
   ↓
Verify
   ↓
Ready for Boot Switch
```

## Immutable Integration

Bei immutable Updates wird während Staging eine vollständige neue Generation erzeugt.

```text
Generation N
      ↓
Staging
      ↓
Generation N+1
      ↓
Verify
      ↓
Activate
```

## Live Update und Hotpatch

Auch Live Updates und Hotpatches können voraktiviert vorbereitet werden.

```text
Load
Validate
Prepare State
Prepare Patch
      ↓
Staged
      ↓
Runtime Switch
```

Dadurch bleibt die eigentliche Unterbrechung möglichst kurz.

## Offline Staging

Updates dürfen vollständig vorbereitet werden, obwohl das Zielsystem aktuell nicht aktiviert werden soll.

Dies ermöglicht:

```text
Download Now
Activate Later

Prepare Online
Activate Offline

Prepare During Idle Time
Activate During Maintenance
```

## Resource Economy

Staging benötigt temporäre Ressourcen:

```text
Storage
Memory
CPU
I/O
Network
Energy
```

Der Update Manager muss diese Ressourcen planen und begrenzen können.

Staging darf harte Ressourcenreservierungen laufender Workloads nicht verletzen.

## Staging Expiration

Ein lange vorbereitetes Update kann durch Änderungen ungültig werden.

Beispiele:

```text
Key Revocation
Repository Metadata Expiration
New Security Policy
Dependency Change
State Change
Superseding Update
```

Ein Staging-Zustand muss daher invalidierbar oder als `Expired` markierbar sein.

## Cleanup

Nicht mehr benötigte Staging-Daten dürfen entfernt werden.

Vorher müssen Referenzen geprüft werden:

```text
Active Transaction
Rollback
Snapshot
A/B Slot
Recovery
Pinned Update
```

```text
Not Active ≠ Safe to Delete
```

## Crash Recovery

Der Staging-State muss nach einem Crash rekonstruierbar sein.

Mögliche Ergebnisse:

```text
Incomplete
Valid
Ready
Invalid
CleanupRequired
Unknown
```

Teilweise heruntergeladene Inhalte dürfen weiterverwendet werden, wenn ihre Integrität eindeutig validiert werden kann.

## Security

Staging darf keine Sicherheitsprüfung umgehen.

```text
Staged
≠
Trusted

Trusted
≠
Authorized

Authorized
≠
Activated
```

Security- und Trust-Zustände müssen unmittelbar vor kritischer Aktivierung erneut berücksichtigt werden.

## Provenance

NovaOS soll nachvollziehen können:

```text
StagingID
UpdateID
TransactionID
Source Repository
PackageIDs
ContentIDs
Base Version
Target Version
Validation Result
Creation Time
Last Revalidation
Activation Result
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Staged Updates
Staging State
Target Version
Required Storage
Dependencies
Integrity State
Trust State
Ready State
Expiration State
Activation Policy
Failure Reason
```

## Normative Anforderungen

1. NovaOS MUSS Updates getrennt vom aktiven Systemzustand vorbereiten können.
2. Jeder Staging-Vorgang MUSS eindeutig identifizierbar sein.
3. Unvollständiges Staging DARF den aktiven Zustand NICHT verändern.
4. Staged Artefakte MÜSSEN auf Integrität prüfbar sein.
5. ContentIDs MÜSSEN vor Ready-Markierung validiert werden.
6. Signatur- und Trust-Prüfungen MÜSSEN vor Aktivierung erfolgen.
7. Harte Dependencies MÜSSEN vor Aktivierung verfügbar sein.
8. Staging und Aktivierung MÜSSEN getrennte Zustände bleiben.
9. `Ready` DARF NICHT automatisch eine Aktivierung auslösen.
10. Dynamische Bedingungen MÜSSEN vor Aktivierung revalidiert werden.
11. Revocations MÜSSEN auch nach abgeschlossenem Staging berücksichtigt werden.
12. Staging MUSS in transaktionale Updates integrierbar sein.
13. Staging MUSS atomare Aktivierung unterstützen.
14. Staging MUSS mit A/B-Updates kombinierbar sein.
15. Staging MUSS immutable Generationen vorbereiten können.
16. Live Updates und Hotpatches SOLLEN Staging verwenden können.
17. Offline- und verzögerte Aktivierung MUSS unterstützt werden können.
18. Staging MUSS Resource Budgets berücksichtigen.
19. Harte Ressourcenreservierungen DÜRFEN NICHT verletzt werden.
20. Veraltete Staging-Zustände MÜSSEN invalidierbar sein.
21. Nicht mehr benötigte Staging-Daten MÜSSEN kontrolliert bereinigbar sein.
22. Recovery-relevante Daten DÜRFEN NICHT vorzeitig entfernt werden.
23. Staging-State MUSS nach Crash rekonstruierbar sein.
24. `Unknown` DARF NICHT als aktivierungsbereit interpretiert werden.
25. Staging-Vorgänge MÜSSEN nachvollziehbare Provenance besitzen.
26. Staging-State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-PACKAGE-0001`
- `NPSPEC-UPDATE-DEPENDENCY-0001`
- `NPSPEC-UPDATE-SIGNING-0001`
- `NPSPEC-UPDATE-ATOMIC-0001`
- `NPSPEC-UPDATE-TRANSACTIONAL-0001`
- `NPSPEC-UPDATE-AB-0001`
- `NPSPEC-UPDATE-DELTA-0001`
- `NPSPEC-UPDATE-CONTENTADDRESS-0001`
- `NPSPEC-UPDATE-IMMUTABLE-0001`
- `NPSPEC-UPDATE-LIVE-0001`
- `NPSPEC-UPDATE-HOTPATCH-0001`
- `NPSPEC-RESOURCE-ECONOMY-0001`
- `ADR-ARCH-0180`

## Ergebnis

```text
Update Available
      ↓
Acquire
      ↓
Validate
      ↓
Stage Separately
      ↓
Verify
      ↓
Ready
      ↓
Wait for Activation Policy
      ↓
Revalidate
     ↙   ↘
  Valid  Invalid
    ↓      ↓
Activate  Restage /
          Abort
```

NovaOS erhält damit eine klare Trennung zwischen Vorbereitung und Aktivierung eines Updates. Aufwendige Arbeiten können außerhalb des kritischen Umschaltzeitpunkts durchgeführt werden, während das aktive System bis zum kontrollierten Commit unverändert bleibt.