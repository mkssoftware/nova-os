# NPSPEC-UPDATE-IMMUTABLE-0001 – Nova Immutable Update

## Status

Angenommen

## Kategorie

Update / Immutable System / System Integrity

## Zweck

NovaOS definiert unveränderliche Systemgenerationen für kritische Systembestandteile. Eine aktive Generation wird nicht direkt durch ein Update verändert. Stattdessen wird eine neue Generation getrennt aufgebaut, verifiziert und anschließend atomar aktiviert.

```text
Generation N
    ↓
Build Generation N+1
    ↓
Verify
    ↓
Atomic Activation
    ↓
Generation N+1
```

## Grundprinzipien

```text
Immutable ≠ Read-Only Everywhere
Immutable ≠ No Runtime State
Immutable ≠ No Updates
Immutable ≠ A/B
Immutable ≠ Snapshot
Generation ≠ Version Number
Activated ≠ Verified
Old Generation ≠ Automatically Safe
```

## Generation Model

```text
SystemGeneration
├── GenerationID
├── SystemVersion
├── BuildID
├── ContentRoot
├── Components
├── CreationTime
├── IntegrityState
└── VerificationState
```

Optional:

```text
ParentGeneration
UpdateID
TransactionID
StateSchema
Dependencies
SecurityMetadata
ProvenanceID
```

## Immutable Scope

Unveränderlich können insbesondere sein:

```text
Kernel
Boot Components
System Libraries
System Modules
Drivers
Core Services
Runtime Components
System Configuration Baseline
```

Veränderliche Daten werden davon getrennt.

```text
Immutable System State
        +
Mutable Runtime State
        +
Persistent User State
```

## Generation Creation

Updates verändern die aktive Generation nicht direkt.

```text
Active Generation N
        ↓
Resolve Update
        ↓
Construct Generation N+1
        ↓
Verify N+1
```

Generation N bleibt währenddessen unverändert verfügbar.

## Content Addressing

Komponenten einer Generation sollen über ContentIDs identifiziert werden.

```text
Generation
├── ContentID A
├── ContentID B
└── ContentID C
```

Der vollständige Generation-Inhalt kann über einen `ContentRoot` eindeutig gebunden werden.

## Activation

Nach erfolgreicher Vorbereitung wird die neue Generation atomar aktiviert.

```text
Generation N
     ↓
Atomic Switch
     ↓
Generation N+1
```

Der Aktivierungsmechanismus kann abhängig vom System beispielsweise erfolgen durch:

```text
Boot Selection
A/B Switch
Filesystem Root Switch
Versioned Object Switch
Live Replacement
```

## Verification

Vor Aktivierung müssen mindestens relevante Eigenschaften geprüft werden:

```text
Integrity
Signatures
Trust
Dependencies
Compatibility
Contracts
Security Policy
State Compatibility
```

Nach Aktivierung erfolgt zusätzlich Runtime- und Health-Verifikation.

```text
Prepared ≠ Healthy
```

## Mutable State

Runtime- und Benutzerdaten dürfen nicht unkontrolliert Teil der unveränderlichen Systemgeneration werden.

Beispiele:

```text
User Files
Logs
Caches
Runtime State
Databases
Application Data
Device State
```

Die Schnittstelle zwischen immutable und mutable State muss explizit definiert sein.

## Configuration

Systemkonfiguration kann aufgeteilt werden in:

```text
Immutable Baseline
        +
Transactional Mutable Configuration
```

Änderungen an sicherheitskritischer Baseline erzeugen eine neue Generation.

## Rollback

Frühere Generationen können als Rollback-Ziel erhalten bleiben.

```text
Generation 41
     ↓
Generation 42
     ↓
Failure
     ↓
Activate Generation 41
```

Dabei entsteht logisch ein neuer aktueller Systemzustand; Historie wird nicht zurückgeschrieben.

## Security State

Monotone Security States werden nicht Bestandteil eines unsicheren Generation-Rollbacks.

Beispiele:

```text
Revocations
Minimum Secure Version
Compromise State
Security Counters
Trust Revocations
```

```text
Old Generation
      +
Current Security State
```

## A/B Integration

A/B kann zur Speicherung und Aktivierung immutable Generationen verwendet werden.

```text
Slot A → Generation N
Slot B → Generation N+1
```

A/B beschreibt dabei die Slot-Strategie.

Immutable Update beschreibt die Unveränderlichkeit einer fertiggestellten Generation.

## Snapshot Integration

Snapshots können mutable Zustände sichern, während Systemgenerationen selbst unverändert bleiben.

```text
Immutable Generation
        +
Mutable State Snapshot
```

Beide Mechanismen bleiben getrennt versioniert.

## Delta Updates

Delta Updates dürfen zur effizienten Konstruktion einer neuen Generation verwendet werden.

```text
Generation N
      +
Delta
      ↓
Generation N+1
```

Die bestehende Generation wird dabei nicht verändert.

Das rekonstruierte Ziel muss vollständig gegen seine erwarteten ContentIDs geprüft werden.

## Transaction Integration

```text
Begin
 ↓
Construct New Generation
 ↓
Validate
 ↓
Prepare
 ↓
Atomic Activate
 ↓
Verify
 ↓
Finalize
```

Fehler vor Aktivierung lassen die aktive Generation unverändert.

## Garbage Collection

Alte Generationen dürfen entfernt werden, wenn sie nicht mehr benötigt werden.

Zu berücksichtigen sind:

```text
Active Generation
Rollback Targets
A/B Slots
Snapshots
Recovery
Pinned Generations
Transactions
```

```text
Inactive ≠ Unneeded
```

## Recovery

Kann keine normale Generation gestartet werden:

```text
Generation Failure
      ↓
Known-Good Generation
      ↓
A/B Fallback
      ↓
Recovery Environment
```

Immutable Updates ersetzen nicht den unabhängigen Recovery-Pfad.

## Provenance

Für jede Generation soll nachvollziehbar sein:

```text
GenerationID
ParentGeneration
BuildID
ContentRoot
UpdateID
TransactionID
Packages
Verification Evidence
Activation History
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Active Generation
Available Generations
GenerationID
Parent Generation
System Version
BuildID
ContentRoot
Integrity State
Verification State
Rollback Eligibility
Recovery Relevance
```

## Normative Anforderungen

1. NovaOS MUSS immutable Systemgenerationen für kritische Systembereiche unterstützen können.
2. Eine fertiggestellte Generation DARF NICHT durch normale Updates in-place verändert werden.
3. Updates MÜSSEN eine neue Generation erzeugen können.
4. Immutable und mutable State MÜSSEN logisch getrennt werden.
5. Die Grenze zwischen System- und Runtime-State MUSS explizit definiert sein.
6. Neue Generationen MÜSSEN vor Aktivierung auf Integrität prüfbar sein.
7. Kritische Generationen MÜSSEN gegen Signatur- und Trust-Policy prüfbar sein.
8. Dependencies MÜSSEN vor Aktivierung validiert werden.
9. Die Aktivierung einer Generation MUSS atomar erfolgen können.
10. Aktivierung DARF NICHT automatisch als erfolgreiche Health-Verifikation gelten.
11. Alte Generationen SOLLEN als Rollback-Ziele erhalten werden können.
12. Rollback DARF monotone Security States NICHT zurücksetzen.
13. Immutable Updates MÜSSEN mit A/B-Updates kombinierbar sein.
14. Immutable Updates MÜSSEN mit Update Snapshots kombinierbar sein.
15. Delta Updates DÜRFEN neue immutable Generationen konstruieren.
16. Delta-Verarbeitung DARF die bestehende Generation NICHT verändern.
17. Systemgenerationen SOLLEN content-addressierbar sein.
18. Generationen SOLLEN mit Build- und Verification-Evidence verknüpfbar sein.
19. Fehlgeschlagene Vorbereitung DARF die aktive Generation NICHT beschädigen.
20. Garbage Collection MUSS Rollback-, Recovery- und Transaction-Referenzen berücksichtigen.
21. `Inactive` DARF NICHT automatisch als löschbar gelten.
22. Immutable Updates DÜRFEN einen unabhängigen Recovery-Pfad NICHT ersetzen.
23. Generation-Provenance MUSS nachvollziehbar sein.
24. Generation-State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-PACKAGE-0001`
- `NPSPEC-UPDATE-ATOMIC-0001`
- `NPSPEC-UPDATE-TRANSACTIONAL-0001`
- `NPSPEC-UPDATE-AB-0001`
- `NPSPEC-UPDATE-SNAPSHOT-0001`
- `NPSPEC-UPDATE-ROLLBACK-0001`
- `NPSPEC-UPDATE-DELTA-0001`
- `NPSPEC-UPDATE-CONTENTADDRESS-0001`
- `NPSPEC-STATE-VERSIONING-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-VERIFY-REPRODUCIBLE-0001`
- `ADR-ARCH-0175`

## Ergebnis

```text
Active Generation N
        ↓
Construct N+1
        ↓
Verify Content + Trust
        ↓
Validate Dependencies
        ↓
Atomic Activation
        ↓
Generation N+1
        ↓
Health Verification
       ↙   ↘
   Healthy Failed
      ↓       ↓
   Finalize  Previous Generation /
             Recovery
```

NovaOS erhält damit ein generationsbasiertes Update-Modell, bei dem das laufende System nicht schrittweise überschrieben wird. Neue Systemstände werden separat aufgebaut, vollständig geprüft und atomar aktiviert, während vorherige bekannte Generationen für Rollback und Recovery erhalten bleiben können.