# NPSPEC-OBJECT-IDENTITY-0001 – Nova Object Identity

## Status

Angenommen

## Kategorie

Object / Identity / Architecture

## Zweck

NovaOS definiert eine stabile und systemweit eindeutige Identität für Objekte.

Die Objektidentität bleibt unabhängig von Name, Pfad, Speicherort, Prozess, Provider, physischer Repräsentation und aktueller Version.

```text
Object
  ↓
ObjectID
  ↓
Stable Logical Identity
```

## Grundprinzipien

```text
ObjectID ≠ Name
ObjectID ≠ Path
ObjectID ≠ Memory Address
ObjectID ≠ Handle
ObjectID ≠ CapabilityID
ObjectID ≠ VersionID
ObjectID ≠ ContentID
ObjectID ≠ Location
Knowledge of ObjectID ≠ Authority
```

## ObjectID

Jedes systemweit referenzierbare NovaOS-Objekt kann eine stabile `ObjectID` besitzen.

```text
NovaObject
├── ObjectID
├── ObjectTypeID
├── State
└── VersionID
```

Die `ObjectID` identifiziert das logische Objekt.

## Identitätsstabilität

Operationen wie:

```text
Rename
Move
Mount
Migration
Replication
Representation Change
Provider Change
Version Change
```

dürfen die logische Objektidentität nicht automatisch verändern.

Beispiel:

```text
ObjectID: 7A42...

Benutzer/Dokumente/Bericht.md
              ↓ Move
Archiv/2026/Bericht.md

ObjectID: 7A42...
```

## ObjectID und VersionID

Eine neue Version desselben logischen Objekts behält die `ObjectID`.

```text
ObjectID
├── VersionID A
├── VersionID B
└── VersionID C
```

Es gilt:

```text
ObjectID = Logical Identity
VersionID = Specific Object State
```

## ObjectID und ContentID

`ContentID` beschreibt einen konkreten Inhalt.

```text
ObjectID
   ↓
VersionID
   ↓
ContentID
```

Mehrere Objekte können denselben Inhalt besitzen.

```text
Object A ─┐
          ├── ContentID X
Object B ─┘
```

Daraus folgt nicht:

```text
Object A = Object B
```

## Neue Objektidentität

Eine neue `ObjectID` wird erzeugt, wenn semantisch ein neues unabhängiges Objekt entsteht.

Beispiele:

```text
Explicit Copy
Export as Independent Object
New Object Creation
Import as New Object
```

Eine Ableitung kann über Relationships dokumentiert werden.

```text
New Object
    ↓ DerivedFrom
Source Object
```

## Referenzen

Systemkomponenten sollen Objekte über stabile Referenzen adressieren.

```text
ObjectReference
├── ObjectID
└── optional VersionID
```

Pfadbasierte Referenzen können zusätzlich existieren, gelten jedoch nicht als stabile Objektidentität.

## Location Transparency

Eine `ObjectID` darf nicht an einen bestimmten Speicherort gebunden sein.

```text
ObjectID
├── Volume A
├── Volume B
├── Remote Node
└── Migrated Storage
```

Dabei gilt:

```text
Identity ≠ Location
```

Die Auflösung von `ObjectID` zu einem aktuellen Standort erfolgt separat.

## Capability Security

Eine bekannte `ObjectID` gewährt keinen Zugriff.

```text
ObjectID
   +
Capability
   ↓
Authorized Operation
```

Es gilt:

```text
Known ObjectID ≠ Read
Known ObjectID ≠ Write
Known ObjectID ≠ Delete
Known ObjectID ≠ Share
```

Autorität wird ausschließlich über geeignete Capabilities bestimmt.

## Handles

Lokale Handles dürfen auf Objekte verweisen.

```text
Handle
  ↓
Capability
  ↓
ObjectID
```

Ein Handle ist:

```text
Local
Context-bound
Temporary
```

Eine `ObjectID` ist dagegen die logische Identität des Objekts.

## Beziehungen

Semantic Relationships verwenden bevorzugt `ObjectID`s.

```text
Object A
   ↓ DerivedFrom
Object B
```

Dadurch bleiben Beziehungen auch nach Verschieben oder Umbenennen stabil.

## Löschen und Retirement

Das Entfernen eines Objekts darf nicht zur sofortigen Wiederverwendung seiner `ObjectID` führen.

```text
Active
  ↓
Retired
  ↓
Reclaimed
```

Historische Provenance-, Audit- oder Relationship-Daten können weiterhin auf die ehemalige `ObjectID` verweisen.

Eine einmal vergebene `ObjectID` darf nicht für ein semantisch anderes Objekt wiederverwendet werden.

## Verteilte Systeme

`ObjectID`s müssen auch über System- und Standortgrenzen hinweg eindeutig interpretierbar sein.

```text
Node A
  ↓
ObjectID
  ↓ Migration
Node B
```

Migration darf keine neue Objektidentität erzwingen.

Replikation muss dagegen zwischen:

```text
Same Logical Object
```

und

```text
Independent Copy
```

unterscheiden können.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ObjectID
ObjectTypeID
Current VersionID
State
Location Class
Creation Identity
Relationships
Provenance
```

Die Kenntnis dieser Informationen erzeugt keine zusätzliche Autorität.

## Normative Anforderungen

1. NovaOS MUSS systemweit stabile `ObjectID`s unterstützen.
2. `ObjectID`s MÜSSEN unabhängig von Name, Pfad und Speicherort sein.
3. `ObjectID`, `VersionID`, `ContentID`, `CapabilityID` und Handle MÜSSEN getrennte Konzepte bleiben.
4. Verschieben oder Umbenennen DARF die `ObjectID` NICHT verändern.
5. Eine neue Version desselben logischen Objekts SOLL dieselbe `ObjectID` behalten.
6. Unabhängige Kopien MÜSSEN eigene `ObjectID`s erhalten können.
7. Eine `ObjectID` DARF keine Autorität darstellen.
8. Einmal vergebene `ObjectID`s DÜRFEN NICHT für semantisch andere Objekte wiederverwendet werden.
9. Relationships und Provenance SOLLEN stabile `ObjectID`s verwenden.
10. Objektidentität MUSS Location Transparency und Migration unterstützen.

## Abhängigkeiten

- `NPSPEC-OBJECT-MODEL-0001`
- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-RELATIONSHIP-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-CONTENTADDRESS-0001`
- `NPSPEC-STORAGE-VERSIONING-0001`
- `NPSPEC-STORAGE-PROVENANCE-0001`
- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-ARCH-0022`

## Ergebnis

```text
Logical Object
      ↓
Stable ObjectID
      ↓
Versions + Content
      ↓
Location-independent References
      ↓
Capability-controlled Access
```

NovaOS erhält damit eine stabile Objektidentität, die logische Systemobjekte dauerhaft von ihren Namen, Pfaden, Versionen, Inhalten, Handles und physischen Speicherorten trennt.