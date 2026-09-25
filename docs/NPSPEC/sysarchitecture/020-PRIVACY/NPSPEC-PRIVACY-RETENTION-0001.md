# NPSPEC-PRIVACY-RETENTION-0001 – Nova Data Retention

## Status

Angenommen

## Kategorie

Privacy / Data Protection / Retention

## Zweck

NovaOS definiert ein systemweites Modell zur kontrollierten Aufbewahrung und automatischen Entfernung von Daten.

```text
Data Created
    ↓
Retention Policy
    ↓
Active Lifetime
    ↓
Expire
    ↓
Delete / Anonymize / Archive
```

Daten sollen nicht länger gespeichert werden als für ihren definierten Zweck erforderlich.

## Grundprinzipien

```text
Stored ≠ Needed Forever
Backup ≠ Retention Exemption
Expired ≠ Immediately Physically Erased
Archive ≠ Unlimited Retention
Deletion Request ≠ Confirmed Deletion
```

## Retention-Modell

Eine Retention Policy besitzt mindestens:

```text
RetentionPolicy
├── PolicyID
├── Target
├── Retention Period
├── Expiration Action
└── State
```

Optional:

```text
Purpose
Creation Time
Last Relevant Use
Legal Hold
Archive Policy
Security Domain
Sovereignty Policy
```

## Aufbewahrungsdauer

Aufbewahrung kann definiert werden anhand von:

```text
Fixed Duration
Expiration Date
Purpose Completion
Account Lifetime
Transaction Completion
Policy Event
```

Unbegrenzte Speicherung muss eine explizite Policy-Entscheidung sein.

## Ablauf

Nach Ablauf einer Retention Policy muss eine definierte Aktion erfolgen:

```text
Delete
Secure Erase
Anonymize
Archive
Review
```

Die Aktion muss zum Datentyp und Schutzbedarf passen.

## Automatische Durchsetzung

NovaOS soll Retention Policies automatisch durchsetzen können.

```text
Retention Timer
      ↓
Expiration
      ↓
Policy Evaluation
      ↓
Expiration Action
```

Anwendungen sollen nicht allein dafür verantwortlich sein, abgelaufene Daten zu erkennen.

## Abgeleitete und temporäre Daten

Retention muss auch berücksichtigen:

```text
Caches
Temporary Files
Derived Data
Logs
Indexes
Metadata
```

Eine temporäre Kopie darf nicht unbeabsichtigt länger existieren als die zugrunde liegende Policy erlaubt.

## Snapshots, Backups und Replikate

Retention Policies müssen historische und verteilte Kopien berücksichtigen können.

```text
Primary Data
├── Snapshot
├── Backup
└── Replica
```

Das Löschen des primären Objekts bedeutet nicht automatisch, dass sämtliche Kopien entfernt wurden.

## Legal Hold

Eine autorisierte Policy darf Löschung vorübergehend verhindern:

```text
Retention Expired
      +
Legal Hold
      ↓
Preserve
```

Legal Hold muss explizit, nachvollziehbar und wieder aufhebbar sein.

## Anonymisierung

Daten dürfen nach Ablauf anonymisiert statt gelöscht werden, wenn die Policy dies erlaubt.

```text
Personal Data
     ↓
Anonymization
     ↓
Non-personal Data
```

Pseudonymisierung darf nicht automatisch als vollständige Anonymisierung behandelt werden.

## Secure Erase

Für besonders sensible Daten kann die Retention Policy `Secure Erase` verlangen.

```text
Expiration
    ↓
Secure Erase
    ↓
Verification
```

Dabei gelten die Regeln von `NPSPEC-CRYPTO-SECUREERASE-0001`.

## Verteilte Systeme

Der Retention-Status verteilter Kopien muss unterscheidbar bleiben:

```text
Replica A → Deleted
Replica B → Deleted
Replica C → Pending
Replica D → Unknown
```

`Unknown` darf nicht als erfolgreiche Löschung dargestellt werden.

## Introspection

NovaOS soll mindestens darstellen können:

```text
TargetID
Retention Policy
Expiration
Expiration Action
Legal Hold
Deletion State
```

## Normative Anforderungen

1. NovaOS MUSS systemweite Retention Policies unterstützen.
2. Aufbewahrungsdauer MUSS explizit definierbar sein.
3. Unbegrenzte Speicherung DARF NICHT impliziter Standard sein.
4. Ablaufaktionen MÜSSEN policygesteuert definierbar sein.
5. Temporäre und abgeleitete Daten MÜSSEN Retention berücksichtigen.
6. Snapshots, Backups und Replikate MÜSSEN bei Retention berücksichtigt werden können.
7. Legal Hold MUSS explizit autorisiert und nachvollziehbar sein.
8. Pseudonymisierung DARF NICHT automatisch als Anonymisierung gelten.
9. `Unknown` DARF NICHT als bestätigte Löschung gelten.
10. Retention-Zustände SOLLEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-CRYPTO-SECUREERASE-0001`
- `NPSPEC-SECURITY-DATASOVEREIGNTY-0001`
- `NPSPEC-SECURITY-AUDIT-0001`
- `NPSPEC-STORAGE-SNAPSHOT-0001`
- `ADR-PRIVACY-0002`

## Ergebnis

```text
Data
 ↓
Retention Policy
 ↓
Required Lifetime
 ↓
Expiration
 ↓
Delete / Anonymize / Archive / Secure Erase
```

NovaOS erhält damit eine zentrale Retention-Architektur, die verhindert, dass Daten ohne definierten Zweck unbegrenzt gespeichert werden, und die Aufbewahrung über aktive Daten, temporäre Kopien, Backups und Replikate hinweg kontrollierbar macht.