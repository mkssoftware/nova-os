# NPSPEC-STORAGE-SNAPSHOT-0001 – Nova Storage Snapshots

## Status

Angenommen

## Kategorie

Storage / Snapshot / Versioning / Recovery

## Zweck

NovaOS definiert Snapshots als konsistente Referenzen auf einen bestimmten Storage-Zustand.

```text
Storage State
     ↓
Snapshot
     ↓
Stable Recovery Point
```

Snapshots ermöglichen Recovery, Rollback, Backup und konsistente Zustandsaufnahmen, ohne alle referenzierten Daten physisch kopieren zu müssen.

## Grundprinzip

```text
Snapshot ≠ Full Copy
Snapshot ≠ Version
Snapshot ≠ Backup
```

Ein Snapshot referenziert bestehende Objektversionen und hält einen definierten Zustand fest.

## Snapshot-Modell

Ein Snapshot besitzt mindestens:

```text
SnapshotID
Scope
Timestamp
State
References
```

Optional:

```text
Parent Snapshot
TransactionID
Label
Retention Policy
Provenance
```

## Scope

Snapshots dürfen unterschiedliche Bereiche erfassen:

```text
Object
Directory
Volume
Filesystem
System
```

Der Scope muss eindeutig definiert sein.

Ein systemweiter Snapshot darf aus mehreren koordinierten Storage-Snapshots bestehen.

## Erstellung

Die Snapshot-Erstellung muss einen konsistenten Zustand erfassen.

```text
Current State
     ↓
Prepare
     ↓
Capture References
     ↓
Commit Snapshot
```

Neue Änderungen nach dem Commit gehören nicht zum bereits erstellten Snapshot.

## Versionierung

Snapshots referenzieren konkrete Objektversionen.

```text
Snapshot A
├→ Object 1 / Version 4
├→ Object 2 / Version 7
└→ Object 3 / Version 2
```

Dadurch kann ein Snapshot unabhängig vom späteren aktuellen Zustand der Objekte bestehen bleiben.

## Copy-on-Write

NovaOS soll effiziente Snapshots über Copy-on-Write oder vergleichbare Mechanismen ermöglichen.

```text
Snapshot
   ↓
Shared Existing Data
   ↓
Write
   ↓
New Version / New Content
```

Unveränderte Daten müssen nicht dupliziert werden.

## Content Addressing

Snapshots dürfen ContentIDs indirekt über Objektversionen referenzieren.

```text
Snapshot
   ↓
VersionID
   ↓
ContentID
```

Identischer Content darf dadurch zwischen mehreren Snapshots gemeinsam verwendet werden.

## Rollback

Ein Snapshot darf als Ausgangspunkt für einen Rollback verwendet werden.

```text
Current State
     ↓
Rollback
     ↓
Snapshot State
```

Rollback muss als kontrollierte Storage-Operation erfolgen und darf den Snapshot selbst nicht automatisch verändern.

## Recovery

Snapshots können Recovery Points für NovaOS bereitstellen.

```text
Failure
   ↓
Select Snapshot
   ↓
Restore
   ↓
Verify
```

Ein vorhandener Snapshot gilt nicht automatisch als gültiger Recovery Point. Integrität und benötigte Abhängigkeiten müssen überprüfbar sein.

## Retention

Snapshots dürfen unterschiedliche Aufbewahrungsregeln besitzen.

```text
Temporary
Automatic
User Protected
Recovery
Backup
```

Ein geschützter Snapshot darf nicht durch normale automatische Bereinigung entfernt werden.

## Reclamation

Von Snapshots referenzierte Versionen und Inhalte dürfen nicht reclaimed werden.

```text
Snapshot Reference > 0
        ↓
Content Retained
```

Nach Löschen eines Snapshots dürfen nicht mehr benötigte Versionen durch Garbage Collection freigegeben werden.

## Sicherheit

Snapshots unterliegen denselben oder strengeren Zugriffsregeln wie die enthaltenen Daten.

Ein Snapshot darf keine Möglichkeit bieten, aktuelle Security-Regeln durch Zugriff auf ältere Zustände zu umgehen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
SnapshotID
Scope
Timestamp
State
Parent
Referenced Versions
Retention Policy
Integrity State
```

## Normative Anforderungen

1. NovaOS MUSS konsistente Storage-Snapshots unterstützen können.
2. Jeder Snapshot MUSS eine stabile `SnapshotID` besitzen.
3. Der Snapshot-Scope MUSS eindeutig definiert sein.
4. Snapshots SOLLEN konkrete Objektversionen referenzieren.
5. Snapshot-Erstellung SOLL ohne vollständige Datenkopie möglich sein.
6. NovaOS SOLL Copy-on-Write oder vergleichbare effiziente Mechanismen unterstützen.
7. Nach Snapshot-Commit erfolgte Änderungen DÜRFEN den bestehenden Snapshot NICHT verändern.
8. Referenzierte Versionen und Inhalte DÜRFEN NICHT vorzeitig reclaimed werden.
9. Rollback MUSS als kontrollierte Storage-Operation erfolgen.
10. Ein Snapshot DARF NICHT automatisch als gültiger Recovery Point gelten.
11. Geschützte Snapshots DÜRFEN NICHT durch normale automatische Bereinigung entfernt werden.
12. Snapshot-Zustand, Scope, Retention und Integrität SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-CONTENTADDRESS-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-STORAGE-VERSIONING-0001`
- `NPSPEC-STORAGE-METADATA-0001`
- `NPSPEC-STORAGE-PROVENANCE-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `ADR-STORAGE-0009`

## Ergebnis

```text
Objects + Versions
        ↓
     Snapshot
        ↓
Stable Storage State
   ↓         ↓
Rollback   Recovery
```

NovaOS erhält damit eine effiziente Snapshot-Schicht, die konsistente Storage-Zustände festhält und Versionierung, Recovery und Rollback verbindet, ohne vollständige Kopien der gespeicherten Daten vorauszusetzen.