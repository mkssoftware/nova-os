# NPSPEC-OBJECT-RELATIONSHIP-0001 – Nova Object Relationship

## Status

Angenommen

## Kategorie

Object / Relationship / Architecture

## Zweck

NovaOS definiert ein einheitliches Modell für stabile, typisierte Beziehungen zwischen Objekten.

```text
Object A
   ↓
Relationship
   ↓
Object B
```

Beziehungen werden über `ObjectID`s aufgebaut und bleiben dadurch unabhängig von Namen, Pfaden, Speicherorten und konkreten Objektinstanzen.

## Grundprinzipien

```text
Relationship ≠ Ownership
Relationship ≠ Authority
Relationship ≠ Permission
Relationship ≠ Containment
Object Reference ≠ Relationship
Known Relationship ≠ Access
Related Objects ≠ Same Object
Relationship Direction ≠ Automatic Inverse
```

## Relationship Model

Eine Objektbeziehung besitzt mindestens:

```text
ObjectRelationship
├── RelationshipID
├── RelationshipTypeID
├── Source ObjectID
├── Target ObjectID
└── State
```

Optional:

```text
Source VersionID
Target VersionID
Metadata
Provenance
Constraints
Timestamp
Creator Identity
TransactionID
Confidence
```

## Beziehungstypen

Typische Beziehungen sind:

```text
Contains
References
DependsOn
DerivedFrom
GeneratedFrom
CreatedBy
PartOf
AssociatedWith
VersionOf
Supersedes
Uses
```

Objekttypen dürfen zusätzliche Relationship Types definieren.

## Richtung

Beziehungen können sein:

```text
Directed
Symmetric
Bidirectional
```

Beispiel:

```text
Document A
   ↓ References
Document B
```

bedeutet nicht automatisch:

```text
Document B
   ↓ References
Document A
```

Inverse Beziehungen müssen explizit definiert sein.

## Objektidentität

Relationships verwenden stabile `ObjectID`s.

```text
Object A
ObjectID: A42
     ↓
References
     ↓
ObjectID: B71
Object B
```

Umbenennen, Verschieben oder Migration eines Objekts darf die Beziehung nicht automatisch verändern.

## Versionen

Beziehungen können entweder auf ein logisches Objekt oder eine konkrete Version zeigen.

```text
ObjectID
```

oder:

```text
ObjectID + VersionID
```

Damit kann beispielsweise festgelegt werden:

```text
Document A:v3
    ↓ DerivedFrom
Dataset B:v7
```

## Relationship Identity

Eine Beziehung kann selbst eine stabile `RelationshipID` besitzen.

Dadurch kann sie:

```text
Referenced
Versioned
Audited
Revoked
Retired
Inspected
```

werden, ohne ihre Identität mit den verbundenen Objekten zu vermischen.

## Relationship Lifecycle

```text
Created
   ↓
Active
   ↓
Modified
   ↓
Retired
```

Historisch relevante Beziehungen können entsprechend Retention- und Provenance-Regeln erhalten bleiben.

## Semantik

`RelationshipTypeID` definiert die Bedeutung und zulässige Struktur einer Beziehung.

Beispiel:

```text
DerivedFrom

Source:
Nova.Document

Target:
Nova.Document | Nova.Data.*
```

Die Beziehung muss gegen ihre semantischen Regeln validierbar sein.

## Graphmodell

Objektbeziehungen bilden einen systemweiten logischen Graphen.

```text
Object A ──References──→ Object B
   │
   └──DerivedFrom──→ Object C
                         │
                         └──CreatedBy──→ Identity D
```

Dieser Graph kann für:

```text
Navigation
Dependency Analysis
Provenance
Semantic Query
Impact Analysis
Discovery
```

verwendet werden.

## Transitivität

NovaOS darf Transitivität nicht automatisch annehmen.

```text
A → B
B → C
```

bedeutet nur dann:

```text
A → C
```

wenn der jeweilige `RelationshipTypeID` dies ausdrücklich definiert.

## Berechtigungen

Relationships erzeugen keine Autorität.

```text
Access(Object A)
        +
A References B
        ≠
Access(Object B)
```

Zugriff auf jedes Zielobjekt benötigt weiterhin geeignete Capabilities.

Auch das Lesen oder Ändern einer Beziehung kann eigene Berechtigungen erfordern.

## Transaktionen

Relationship-Änderungen sollen transaktional erfolgen können.

```text
Begin
 ↓
Create / Modify / Remove Relationship
 ↓
Validate
 ↓
Commit
```

Objekt- und Relationship-Änderungen können Bestandteil derselben Transaktion sein.

## Provenance

Für Relationship-Änderungen kann Provenance gespeichert werden.

```text
Who
What
When
Why
Source
Transaction
```

Dadurch bleibt nachvollziehbar, wie eine Verbindung zwischen Objekten entstanden ist.

## Löschen von Objekten

Beim Retirement oder Löschen eines Objekts müssen bestehende Relationships kontrolliert behandelt werden.

Mögliche Zustände:

```text
Retain Historical Reference
Retire Relationship
Mark Target Unavailable
Remove Relationship
```

Die Behandlung wird durch Relationship Type, Retention Policy und Objektpolicy bestimmt.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
RelationshipID
RelationshipTypeID
Source ObjectID
Target ObjectID
Version References
State
Metadata
Provenance
Constraints
```

## Normative Anforderungen

1. NovaOS MUSS typisierte Beziehungen zwischen Objekten unterstützen.
2. Relationships SOLLEN stabile `ObjectID`s verwenden.
3. Beziehungen MÜSSEN von Objektidentität, Berechtigungen und Capabilities getrennt bleiben.
4. Relationship Types MÜSSEN Richtung und zulässige Semantik definieren können.
5. Transitivität DARF NICHT automatisch angenommen werden.
6. Beziehungen MÜSSEN auf logische Objekte oder konkrete Versionen verweisen können.
7. Relationship-Änderungen SOLLEN transaktional ausführbar sein.
8. Relationships DÜRFEN keine implizite Autorität auf verbundene Objekte erzeugen.
9. Objekt-Retirement MUSS bestehende Relationships kontrolliert behandeln.
10. Relationships MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-OBJECT-MODEL-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-OBJECT-REFERENCE-0001`
- `NPSPEC-OBJECT-VERSIONING-0001`
- `NPSPEC-OBJECT-PROVENANCE-0001`
- `NPSPEC-OBJECT-PERMISSION-0001`
- `NPSPEC-SEMANTIC-RELATIONSHIP-0001`
- `NPSPEC-SEMANTIC-VALIDATION-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `ADR-ARCH-0027`

## Ergebnis

```text
Stable ObjectIDs
      ↓
Typed Relationships
      ↓
Object Graph
      ↓
Semantic Validation
      ↓
Query + Provenance + Dependencies
```

NovaOS erhält damit einen stabilen, typisierten Objektgraphen, in dem Beziehungen unabhängig von Pfaden und Speicherorten bestehen bleiben und für Navigation, Abhängigkeiten, Provenance, Discovery und semantische Verarbeitung genutzt werden können, ohne dabei implizite Zugriffsrechte zu erzeugen.