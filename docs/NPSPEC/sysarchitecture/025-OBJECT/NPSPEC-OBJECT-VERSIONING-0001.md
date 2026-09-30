# NPSPEC-OBJECT-VERSIONING-0001 – Nova Object Versioning

## Status

Angenommen

## Kategorie

Object / Versioning / Architecture

## Zweck

NovaOS definiert ein einheitliches Versionierungsmodell für Systemobjekte, bei dem verschiedene Zustände eines logischen Objekts nachvollziehbar erhalten und eindeutig referenziert werden können.

```text
ObjectID
├── VersionID 1
├── VersionID 2
└── VersionID 3
```

Die logische Objektidentität bleibt dabei von ihren einzelnen Versionen getrennt.

## Grundprinzipien

```text
ObjectID ≠ VersionID
VersionID ≠ ContentID
New Version ≠ New Object
New Object ≠ New Version
Current Version ≠ Only Version
Version History ≠ Backup
Version Access ≠ Object Authority
Rollback ≠ History Deletion
```

## Versionsmodell

Ein versioniertes Objekt besitzt:

```text
ObjectVersion
├── ObjectID
├── VersionID
├── Parent VersionID
├── State
└── Timestamp
```

Optional:

```text
ContentID
SemanticTypeID
Metadata Version
Relationship State
Provenance
TransactionID
Creator Identity
Change Description
```

## ObjectID und VersionID

`ObjectID` identifiziert das logische Objekt.

`VersionID` identifiziert einen konkreten Zustand dieses Objekts.

```text
ObjectID: A

A:v1
A:v2
A:v3
```

Eine neue Version darf die logische Identität nicht automatisch verändern.

## Versionserzeugung

Eine neue Version kann entstehen durch:

```text
Payload Change
Metadata Change
Relationship Change
Semantic Transformation
Transactional Update
Explicit Snapshot
```

Nicht jede interne Zustandsänderung muss zwingend eine persistente Version erzeugen.

Die jeweilige Objektklasse kann ihre Versionierungsregeln definieren.

## Immutable Version Identity

Eine veröffentlichte `VersionID` muss einen eindeutig bestimmten Objektzustand bezeichnen.

```text
ObjectID + VersionID
        ↓
Specific State
```

Ein bereits identifizierter Versionszustand darf nicht still durch andere Daten ersetzt werden.

## ContentID

Versionen können zusätzlich über ihren Inhalt identifiziert werden.

```text
ObjectID
   ↓
VersionID
   ↓
ContentID
```

Zwei Versionen können denselben `ContentID` besitzen, wenn ihre Payload identisch ist, während sich beispielsweise Metadaten unterscheiden.

## Versionsgraph

Versionen dürfen neben einer linearen Historie auch Verzweigungen bilden.

```text
        v1
        ↓
        v2
       /  \
     v3a  v3b
       \  /
        v4
```

Damit können parallele Änderungen, Synchronisation und Merge-Szenarien dargestellt werden.

## Current Version

Ein Objekt kann eine definierte aktuelle Version besitzen.

```text
ObjectID
   ↓
CurrentVersionID
```

`CurrentVersionID` ist veränderlicher Zustand und kein Bestandteil der stabilen Objektidentität.

## Referenzen

Eine Object Reference kann entweder das logische Objekt oder eine konkrete Version adressieren.

```text
ObjectID
```

oder:

```text
ObjectID + VersionID
```

Eine versionierte Referenz darf nicht still auf die aktuelle Version umgeleitet werden.

## Transaktionen

Versionserzeugung soll mit Nova Transactions integrierbar sein.

```text
Begin
 ↓
Prepare New Version
 ↓
Validate
 ↓
Commit
 ↓
Publish VersionID
```

Unvollständige Änderungen dürfen nicht als gültige veröffentlichte Version erscheinen.

## Rollback

Rollback erzeugt keinen Verlust der Historie.

```text
v1 → v2 → v3
          ↓
     Restore v1 State
          ↓
         v4
```

`v4` kann semantisch den Zustand von `v1` wiederherstellen, bleibt aber eine neue Version.

## Beziehungen und Provenance

Versionsbeziehungen müssen nachvollziehbar sein.

```text
v3
↓ DerivedFrom
v2
```

Merge-Versionen können mehrere Vorgänger besitzen.

Provenance kann dokumentieren:

```text
Who
What
When
How
Source Version
Transaction
```

## Capability Security

Versionierung erzeugt keine Autorität.

```text
Read(Object) ≠ Read(All Versions)
Write(Object) ≠ Rewrite History
Rollback ≠ Write Permission
```

Capabilities können Rechte auf:

```text
Current Version
Specific Version
Version History
Create Version
Rollback
```

getrennt kontrollieren.

## Retention

Alte Versionen können entsprechend Retention Policy:

```text
Retain
Archive
Compact
Expire
Reclaim
```

werden.

Referenzen, Provenance, Audit und rechtliche Anforderungen müssen berücksichtigt werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ObjectID
CurrentVersionID
VersionID
Parent Versions
ContentID
Timestamp
Version State
Provenance
TransactionID
```

## Normative Anforderungen

1. NovaOS MUSS `ObjectID` und `VersionID` getrennt behandeln.
2. Eine neue Version DARF NICHT automatisch eine neue `ObjectID` erzeugen.
3. Veröffentlichte `VersionID`s MÜSSEN einen eindeutig bestimmten Zustand referenzieren.
4. Versionierte Referenzen DÜRFEN NICHT still auf andere Versionen umgeleitet werden.
5. NovaOS SOLL lineare und verzweigte Versionshistorien unterstützen können.
6. Versionserzeugung MUSS transaktional veröffentlichbar sein.
7. Rollback DARF bestehende Versionshistorie NICHT still überschreiben.
8. Versionierung MUSS mit Provenance integrierbar sein.
9. Zugriff auf Versionshistorien MUSS durch Capabilities kontrollierbar sein.
10. Alte Versionen MÜSSEN kontrollierten Retention- und Reclaim-Regeln unterliegen können.

## Abhängigkeiten

- `NPSPEC-OBJECT-MODEL-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-OBJECT-REFERENCE-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-VERSIONING-0001`
- `NPSPEC-STORAGE-CONTENTADDRESS-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-STORAGE-PROVENANCE-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `ADR-ARCH-0024`

## Ergebnis

```text
Stable ObjectID
      ↓
Version History
      ↓
Immutable Version Identity
      ↓
Transactional Updates
      ↓
Provenance + Rollback
      ↓
Capability-controlled Access
```

NovaOS erhält damit ein systemweites Objektversionierungsmodell, das die dauerhafte Identität eines Objekts von seinen einzelnen Zuständen trennt und dadurch Historie, Rollback, Verzweigungen, Synchronisation und nachvollziehbare Änderungen ermöglicht.