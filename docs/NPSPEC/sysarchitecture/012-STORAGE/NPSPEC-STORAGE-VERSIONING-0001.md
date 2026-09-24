# NPSPEC-STORAGE-VERSIONING-0001 – Nova Storage Versioning

## Status

Entwurf

## Kategorie

Storage / Versioning / History / Recovery

## Zweck

NovaOS definiert Storage Versioning als Mechanismus zur Speicherung mehrerer Zustände eines Objekts, ohne dessen stabile Identität zu verändern.

```text
ObjectID
├── Version 1
├── Version 2
└── Version 3 ← Current
```

Versionierung ermöglicht Historie, Rollback, Snapshots und nachvollziehbare Änderungen.

## Grundprinzip

```text
ObjectID ≠ VersionID
New Version ≠ New Object
Rollback ≠ Delete History
```

Die `ObjectID` bleibt stabil. Jeder persistierte Zustand erhält eine eigene `VersionID`.

## Versionsmodell

Eine Version beschreibt einen definierten Zustand eines Objekts.

```text
Version
├── VersionID
├── ObjectID
├── ContentID
├── Metadata
├── ParentVersion
└── Timestamp
```

Zusätzliche Provenance- oder Transaktionsinformationen dürfen ergänzt werden.

## Versionserzeugung

Eine neue persistierte Änderung darf eine neue Version erzeugen.

```text
Version N
    ↓
Modify
    ↓
Transaction
    ↓
Version N+1
```

Die vorherige Version bleibt erhalten, sofern die Versionierungs-Policy dies verlangt.

## Current Version

Ein Objekt besitzt eine aktuell gültige Version.

```text
ObjectID
   ↓
CurrentVersion
   ↓
VersionID
```

Der Wechsel der aktuellen Version muss konsistent erfolgen.

## Content Addressing

Versionen dürfen ContentIDs referenzieren.

```text
Version 1 → ContentID A
Version 2 → ContentID B
Version 3 → ContentID B
```

Identische Payloads müssen dadurch nicht mehrfach physisch gespeichert werden.

## Metadaten

Versionierung darf sowohl Payload als auch relevante Metadaten umfassen.

```text
Version
├── Payload Reference
├── Metadata
└── Relationships
```

Welche Metadaten versioniert werden, wird durch Storage- und Objekt-Policy bestimmt.

## Rollback

Eine ältere Version darf wieder zum aktuellen Zustand werden.

```text
Version 1
Version 2
Version 3 ← Current

Rollback to Version 1

Version 1
   ↓
New Current State
```

Rollback soll die bestehende Historie nicht zerstören.

## Snapshots

Snapshots dürfen bestimmte Versionen dauerhaft referenzieren.

```text
Snapshot
├→ Object A / Version 4
├→ Object B / Version 7
└→ Object C / Version 2
```

Referenzierte Versionen dürfen nicht vorzeitig reclaimed werden.

## Garbage Collection

Nicht mehr benötigte Versionen dürfen gemäß Policy entfernt werden.

Berücksichtigt werden müssen:

```text
Retention Policy
Snapshots
Recovery Points
References
Legal / Security Requirements
```

Erst danach dürfen nicht mehr referenzierte Inhalte reclaimed werden.

## Konflikte

Parallele Änderungen können unterschiedliche Versionen erzeugen.

```text
Version A
   ├→ Version B
   └→ Version C
```

NovaOS darf solche Konflikte erkennen und über höhere Storage- oder Synchronisationsmechanismen auflösen lassen.

## Sicherheit

Der Zugriff auf ältere Versionen benötigt eigene Autorisierung.

```text
Access Current
      ≠
Access History
```

Gelöschte oder vertrauliche Daten dürfen nicht allein durch Versionierung unkontrolliert wieder zugänglich werden.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
ObjectID
Current VersionID
Version History
Parent Version
ContentID
Creation Time
Snapshot References
Retention State
```

## Normative Anforderungen

1. NovaOS MUSS mehrere persistente Versionen eines Storage-Objekts unterstützen können.
2. Versionen MÜSSEN eindeutig von der stabilen `ObjectID` getrennt sein.
3. Eine neue Version DARF die `ObjectID` des Objekts NICHT verändern.
4. Jede persistierte Version MUSS eindeutig identifizierbar sein.
5. Der Wechsel der aktuellen Version MUSS konsistent erfolgen.
6. Versionen DÜRFEN ContentIDs gemeinsam verwenden.
7. Rollback DARF bestehende Historie nicht automatisch zerstören.
8. Snapshot-referenzierte Versionen DÜRFEN NICHT vorzeitig reclaimed werden.
9. Version-Reclamation MUSS definierte Retention- und Referenzregeln beachten.
10. Parallele Versionszweige MÜSSEN erkennbar sein.
11. Zugriff auf historische Versionen MUSS autorisiert werden.
12. Versionshistorie und Retention-Zustand SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-VFS-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-CONTENTADDRESS-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-STORAGE-0005`

## Ergebnis

```text
Stable ObjectID
      ↓
Version History
      ↓
Current Version
      ↓
Content + Metadata
      ↓
Rollback / Snapshot / Recovery
```

NovaOS erhält damit eine kompakte Versionierungsschicht, die Objektidentität und Objektzustand trennt und Historie, Rollback sowie Snapshots auf einer gemeinsamen Storage-Grundlage ermöglicht.